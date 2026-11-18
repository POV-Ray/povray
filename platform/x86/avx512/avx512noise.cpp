//******************************************************************************
///
/// @file platform/x86/avx512/avx512noise.cpp
///
/// This file contains implementations of the noise generator optimized for the
/// AVX512 instruction set.
///
/// @note
///     This file shares lots of code with @ref platform/x86/avxnoise.cpp,
///     essentially differing only in a few macro definitions and some
//      identifier names.
///
/// @author Original optimizations by MCW
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "avx512noise.h"

#ifdef MACHINE_INTRINSICS_H
#include MACHINE_INTRINSICS_H
#endif

#include "core/material/noise.h"


/// @file
/// @attention
///     This file **must not** contain any code that might get called before CPU
///     support for this optimized implementation has been confirmed. Most
///     notably, the function to detect support itself must not reside in this
///     file.

/*****************************************************************************/


#ifdef TRY_OPTIMIZED_NOISE_AVX512

namespace pov
{

#ifndef DISABLE_OPTIMIZED_NOISE_AVX512

const bool kAVX512NoiseEnabled = true;
// Predefined Vectors used for computations
__m512i mask_incrsump_s11;
__m512i mask_incrsump_s12;
__m512i mask_incrsump_s13;
__m512i mask_ij1;
__m512i mask_incrsump_s21;
__m512i mask_incrsump_s22;
__m512i mask_incrsump_s23;
__m512i mask_ij2;
// Below masks are to fetch and set to specific elements
__m512i mask_permute_fetch1;
__m512i mask_permute_fetch2;
__m512i mask_permute_fetch3;
__m512i mask_permute_fetch4;
__m512d multiplicand_mask;
__m512d omega_values;
__m512d TWO_PD;
__m512d FOUR_PD;
__m512d addition_factor_range;
__m512d multiplication_factor_range;
__m512d addition_factor_perlin;
__m512d multiplication_factor_perlin;
__m512d POINT_FIVE;
// Below three masks are used to combine iii and jjj
__m512i blendmask2;
__m512i blendmask3;
__m512i blendmask4;
// Constant vectors used
__m512d ONE_PD;
__m256d ONE_PD_SHORT;
__m512d epsy;
__m256d short_epsy;
__m256i vector_allhigh;
__m128i short_si128;
__m512d three_vector_pd;
__m256d short_three_vector_pd;
__m512d zero_vector_pd;
__m256d short_zero_vector_pd;
/******************************************************/
/* Use avx2 intrinsics for vpermpd and native fma3    */
/******************************************************/

#define FMA_PD(a,b,c) _mm256_fmadd_pd((a),(b),(c))
#define FMA_PD_AVX512(a,b,c) _mm512_fmadd_pd((a),(b),(c))
#define PERMUTE4x64(a,i) _mm256_permute4x64_pd((a),(i))


#ifdef NO_SPLITS
#define AVX512TABLETYPE __m256d

#define Hash1dRTableIndexAVX(a,b)   \
    (((unsigned char)hashTable[(int)(a) ^ (b)]) * 4)


#define LOAD_32BYTES_FROM_TABLE(m) (*(m))

#else

#define AVX512TABLETYPE DBL

#define Hash1dRTableIndexAVX(a,b)   \
    (((unsigned char)hashTable[(int)(a) ^ (b)]))

#define LOAD_32BYTES_FROM_TABLE(m)  (_mm256_loadu_pd(m))
#define LOAD_32BYTES_FROM_TABLE(m)  (_mm256_loadu_pd(m))

#endif

// Used to accumulate the sum value in the single coordinate noise implementation - sum is reduced to noise
#define INCSUMAVX(sum, m, s_vec, i, j, mask) sum = FMA_PD(s_vec, _mm256_mul_pd(_mm256_blend_pd(i, j, mask),LOAD_32BYTES_FROM_TABLE(m)), sum)

// Used to accumulate the sum value in the single coordinate noise implementation - without a blend operation
#define INCSUMAVX_NOBLEND(sum, m, s_vec, i) sum = FMA_PD(s_vec, _mm256_mul_pd(i, LOAD_32BYTES_FROM_TABLE(m)),sum)

// Used to accumulate the sum value in the multiple coordinate noise implementation - sum is reduced to noise
#define INCSUMAVX512(sum, m1, m2, s_vec, i, j, mask) \
    c1 = _mm512_castpd256_pd512(LOAD_32BYTES_FROM_TABLE(m1)); \
    c1 = _mm512_mask_loadu_pd(c1, 0xF0, m2 - 4); \
    sum = FMA_PD_AVX512(s_vec, _mm512_mul_pd(_mm512_mask_blend_pd(mask, i, j), c1), sum);

// Used to initialise the sum value in the multiple coordinate noise implementation (before start of accumulation)
#define INCSUMAVX512_INITIALIZE(sum, m1, m2, s_vec, i, j, mask) \
    c1 = _mm512_castpd256_pd512(LOAD_32BYTES_FROM_TABLE(m1)); \
    c1 = _mm512_mask_loadu_pd(c1, 0xF0, m2 - 4); \
    sum = _mm512_mul_pd(s_vec, _mm512_mul_pd(_mm512_mask_blend_pd(mask, i, j), c1));

// Used to accumulate x, y, z values that is eventually reduced to DNoise output in single coordinate implementation
#define INCSUMAVX_VECTOR(m, s, blend) \
    x = FMA_PD(s, _mm256_mul_pd(blend, LOAD_32BYTES_FROM_TABLE(m)), x); \
    m+=4; \
    y = FMA_PD(s, _mm256_mul_pd(blend, LOAD_32BYTES_FROM_TABLE(m)), y); \
    m+=4; \
    z = FMA_PD(s, _mm256_mul_pd(blend, LOAD_32BYTES_FROM_TABLE(m)), z);

// Used to accumulate x, y, z values that is eventually reduced to DNoise output in multiple coordinate implementation
// Defined with generic names x_vec, y_vec, z_vec to accomodate different variable for different coordinates
#define INCSUMAVX512_VECTOR(x_vec, y_vec, z_vec, m1, m2, s_vec, blend) \
    c1 = _mm512_castpd256_pd512(LOAD_32BYTES_FROM_TABLE(m1)); \
    c1 = _mm512_mask_loadu_pd(c1, 0xF0, m2 - 4); \
    x_vec = _mm512_fmadd_pd(s_vec, _mm512_mul_pd(blend, c1), x_vec); \
    m1 += 4; \
    c1 = _mm512_castpd256_pd512(LOAD_32BYTES_FROM_TABLE(m1)); \
    c1 = _mm512_mask_loadu_pd(c1, 0xF0, m2); \
    y_vec = _mm512_fmadd_pd(s_vec, _mm512_mul_pd(blend, c1), y_vec); \
    m1 += 4; \
    m2 += 4; \
    c1 = _mm512_castpd256_pd512(LOAD_32BYTES_FROM_TABLE(m1)); \
    c1 = _mm512_mask_loadu_pd(c1, 0xF0, m2); \
    z_vec = _mm512_fmadd_pd(s_vec, _mm512_mul_pd(blend, c1), z_vec);

// Used to initialise x, y, z values that is eventually reduced to DNoise output in multiple coordinate implementation (before accumulation)
// Defined with generic names x_vec, y_vec, z_vec to accomodate different variable for different coordinates
#define INCSUMAVX512_VECTOR_INITIALIZE(x_vec, y_vec, z_vec, m1, m2, s_vec, blend) \
    c1 = _mm512_castpd256_pd512(LOAD_32BYTES_FROM_TABLE(m1)); \
    c1 = _mm512_mask_loadu_pd(c1, 0xF0, m2 - 4); \
    x_vec = _mm512_mul_pd(s_vec, _mm512_mul_pd(blend, c1)); \
    m1 += 4; \
    c1 = _mm512_castpd256_pd512(LOAD_32BYTES_FROM_TABLE(m1)); \
    c1 = _mm512_mask_loadu_pd(c1, 0xF0, m2); \
    y_vec = _mm512_mul_pd(s_vec, _mm512_mul_pd(blend, c1)); \
    m1 += 4; \
    m2 += 4; \
    c1 = _mm512_castpd256_pd512(LOAD_32BYTES_FROM_TABLE(m1)); \
    c1 = _mm512_mask_loadu_pd(c1, 0xF0, m2); \
    z_vec = _mm512_mul_pd(s_vec, _mm512_mul_pd(blend, c1));

// Initial computations in multiple coordinate implementation, defined as a macro for better readability
// Some of these compuatations are i_xyzn based on which hash compuatation is done,  s_xyzn_extended and t_xyzn used for multiplicand formation for sum compuatation,
// and also the masks and vector that are used in the computation of the former
#define COMPUTE_INITIAL_VECTORS_AVX512(xyzn) \
    xyzn_e = _mm512_sub_pd(xyzn, epsy); \
    masktmp_xyzn = _mm512_cmp_pd_mask(xyzn, zero_vector_pd, 1); \
    tmp_xyzn = _mm512_cvttpd_epi32(_mm512_mask_blend_pd(masktmp_xyzn, xyzn, xyzn_e)); \
    xyz_ixyzn_extended = _mm512_sub_pd(xyzn, _mm512_cvtepi32_pd(tmp_xyzn)); \
    xyz_jxyzn_extended = _mm512_sub_pd(xyz_ixyzn_extended, ONE_PD); \
    i_xyzn = _mm256_and_si256(_mm256_sub_epi32(tmp_xyzn, noise_min_xyzn), vector_allhigh); \
    s_xyzn_extended = _mm512_mul_pd(xyz_ixyzn_extended, _mm512_mul_pd(xyz_ixyzn_extended, _mm512_sub_pd(three_vector_pd, _mm512_add_pd(xyz_ixyzn_extended, xyz_ixyzn_extended)))); \
    t_xyzn = _mm512_sub_pd(ONE_PD, s_xyzn_extended); \
    _mm256_storeu_si256((__m256i*)(ints), i_xyzn);

// Initial computations in single coordinate implementation, defined as a macro for better readability
// Some of these compuatations are i_xyzn based on which hash compuatation is done,  s_xyzn and t_xyzn used for multiplicand formation for sum compuatation,
// and also the vectors that are used in the computation of the former
#define COMPUTE_INITIAL_VECTORS_AVX(xyzn) \
    xyzn_e = _mm256_sub_pd(xyzn, short_epsy); \
    tmp_xyzn = _mm256_cvttpd_epi32(_mm256_blendv_pd(xyzn, xyzn_e, xyzn)); \
    xyz_ixyzn = _mm256_sub_pd(xyzn, _mm256_cvtepi32_pd(tmp_xyzn)); \
    xyz_jxyzn = _mm256_sub_pd(xyz_ixyzn, ONE_PD_SHORT); \
    i_xyzn = _mm_and_si128(_mm_sub_epi32(tmp_xyzn, noise_min_xyzn),short_si128); \
    s_xyzn = _mm256_mul_pd(xyz_ixyzn, _mm256_mul_pd(xyz_ixyzn, _mm256_sub_pd(short_three_vector_pd, _mm256_add_pd(xyz_ixyzn, xyz_ixyzn)))); \
    t_xyzn = _mm256_sub_pd(ONE_PD_SHORT, s_xyzn); \
    _mm_storeu_si128((__m128i*)(ints), i_xyzn);

// Used in the computation of vectors that will be permuted to form multiplicand that will be used to compute values that will be accumulated to form noise and dnoise outputs
// Permutation is done in a way that one value from vector incrsump_s will form first 4 values and another value from incrsump_s forms next 4 values of vector
#define COMPUTE_INCRSUMP_AVX512(t_xyzn, s_xyzn_extended) \
    incrsump_s11 = _mm512_permutex2var_pd(t_xyzn, mask_incrsump_s11, s_xyzn_extended); \
    incrsump_s12 = _mm512_permutex2var_pd(t_xyzn, mask_incrsump_s12, s_xyzn_extended); \
    incrsump_s13 = _mm512_permutex2var_pd(t_xyzn, mask_incrsump_s13, s_xyzn_extended); \
    incrsump_s21 = _mm512_permutex2var_pd(t_xyzn, mask_incrsump_s21, s_xyzn_extended); \
    incrsump_s22 = _mm512_permutex2var_pd(t_xyzn, mask_incrsump_s22, s_xyzn_extended); \
    incrsump_s23 = _mm512_permutex2var_pd(t_xyzn, mask_incrsump_s23, s_xyzn_extended); \
    incrsump_s11 = _mm512_mul_pd(incrsump_s11, _mm512_mul_pd(incrsump_s12, incrsump_s13)); \
    incrsump_s21 = _mm512_mul_pd(incrsump_s21, _mm512_mul_pd(incrsump_s22, incrsump_s23));

// Used in the computation of vectors that will be permuted to form multiplicand that will be used to compute values that will be accumulated to form noise and dnoise outputs
// Permutation is done in a way that one value from vector is used for multiplication
#define COMPUTE_INCRSUMP_AVX(t_xyzn, s_xyzn) \
    txtysxsy = _mm256_permute2f128_pd(t_xyzn, s_xyzn, 0x20); \
    txsxtxsx = PERMUTE4x64(txtysxsy, _MM_SHUFFLE(2, 0, 2, 0)); \
    tytysysy = PERMUTE4x64(txtysxsy, _MM_SHUFFLE(3, 3, 1, 1)); \
    txtysxtytxsysxsy = _mm256_mul_pd(txsxtxsx, tytysysy); \
    incrsump_s1 = _mm256_mul_pd(txtysxtytxsysxsy, PERMUTE4x64(t_xyzn, _MM_SHUFFLE(2, 2, 2, 2))); \
    incrsump_s2 = _mm256_mul_pd(txtysxtytxsysxsy, PERMUTE4x64(s_xyzn, _MM_SHUFFLE(2, 2, 2, 2)));

// 4 hashes are formed based on two inputs a  and b (2^2 = 4)
#define COMPUTE_IJ_HASHES(ixiy_hash, jxiy_hash, ixjy_hash, jxjy_hash, a, b) \
    ixiy_hash = Hash2d(a, b); \
    jxiy_hash = Hash2d(a + 1, b); \
    ixjy_hash = Hash2d(a, b + 1); \
    jxjy_hash = Hash2d(a + 1, b + 1);

// Computed outputs will be typically of the form [0.5, a, b, c, 0.5, d, e, f]
// Used in multiplication for accumulation of noise values
#define COMPUTE_MULTIPLICAND_INPUTS(xyz_ixyzn_extended, xyz_jxyzn_extended) \
    iii1 = _mm512_permutex2var_pd(xyz_ixyzn_extended, mask_ij1, multiplicand_mask); \
    jjj1 = _mm512_permutex2var_pd(xyz_jxyzn_extended, mask_ij1, multiplicand_mask); \
    iii2 = _mm512_permutex2var_pd(xyz_ixyzn_extended, mask_ij2, multiplicand_mask); \
    jjj2 = _mm512_permutex2var_pd(xyz_jxyzn_extended, mask_ij2, multiplicand_mask);

// Gets references from AVX512RTable which is used to load values used for sum accumulation for npoise calculation and permuatation of incrsump_s for multiple coordinates as mentioned earlier
#define GET_INCSUMAVX512_INPUTS(ij_hash1, iz_input1, ij_hash2, iz_input2, incrsump_s, permute_mask) \
    mp1 = &AVX512RTable[Hash1dRTableIndexAVX(ij_hash1, iz_input1)]; \
    mp2 = &AVX512RTable[Hash1dRTableIndexAVX(ij_hash2, iz_input2)]; \
    ss1 = _mm512_permutexvar_pd(permute_mask, incrsump_s);

/********************************************************************************************/
/* AVX512 Specific optimizations: Its found that more than 50% of the time is spent in   */
/* Noise and DNoise. These functions have been optimized using AVX512 instructions   */
/********************************************************************************************/

extern DBL RTable[];

ALIGN32 static AVX512TABLETYPE AVX512RTable[267];

// Initialisation function used for initialisation of avx512 table and vectors used for lookup
void AVX512NoiseInit()
{
    int i;
    DBL *avx512table = (DBL *)AVX512RTable;
    for (i = 0; i < 267; i++)
    {
#ifndef NO_SPLITS
        avx512table[i] = RTable[2 * i];
#else
        avx512table[(4 * i) + 0] = RTable[2 * i + 0];
        avx512table[(4 * i) + 1] = RTable[2 * i + 2];
        avx512table[(4 * i) + 2] = RTable[2 * i + 4];
        avx512table[(4 * i) + 3] = RTable[2 * i + 6];
#endif
    // Masks used for permutation purposes
    mask_incrsump_s11 = _mm512_set_epi32(0, 8, 0, 0, 0, 8, 0, 0, 0, 8, 0, 0, 0, 8, 0, 0);
    mask_incrsump_s12 = _mm512_set_epi32(0, 9, 0, 9, 0, 1, 0, 1, 0, 9, 0, 9, 0, 1, 0, 1);
    mask_incrsump_s13 = _mm512_set_epi32(0, 10, 0, 10, 0, 10, 0, 10, 0, 2, 0, 2, 0, 2, 0, 2);
    mask_ij1 = _mm512_set_epi32(0, 2, 0, 1, 0, 0, 0, 8, 0, 2, 0, 1, 0, 0, 0, 8);
    mask_incrsump_s21 = _mm512_set_epi32(0, 12, 0, 4, 0, 12, 0, 4, 0, 12, 0, 4, 0, 12, 0, 4);
    mask_incrsump_s22 = _mm512_set_epi32(0, 13, 0, 13, 0, 5, 0, 5, 0, 13, 0, 13, 0, 5, 0, 5);
    mask_incrsump_s23 = _mm512_set_epi32(0, 14, 0, 14, 0, 14, 0, 14, 0, 6, 0, 6, 0, 6, 0, 6);
    mask_ij2 = _mm512_set_epi32(0, 6, 0, 5, 0, 4, 0, 8, 0, 6, 0, 5, 0, 4, 0, 8);
    // Below masks are to fetch and set to specific elements
    mask_permute_fetch1 = _mm512_set_epi32(0, 7, 0, 7, 0, 7, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0);
    mask_permute_fetch2 = _mm512_set_epi32(0, 3, 0, 3, 0, 3, 0, 3, 0, 2, 0, 2, 0, 2, 0, 2);
    mask_permute_fetch3 = _mm512_set_epi32(0, 5, 0, 5, 0, 5, 0, 5, 0, 4, 0, 4, 0, 4, 0, 4);
    mask_permute_fetch4 = _mm512_set_epi32(0, 1, 0, 1, 0, 1, 0, 1, 0, 6, 0, 6, 0, 6, 0, 6);
    multiplicand_mask = _mm512_set_pd(0, 0, 0, 0.5, 0, 0, 0, 0.5);
    omega_values = _mm512_set_pd(0.00390625, 0.0078125, 0.015625, 0.03125, 0.0625, 0.125, 0.25, 0.5);
    TWO_PD = _mm512_set1_pd(2.0);
    FOUR_PD = _mm512_set1_pd(4.0);
    addition_factor_range = _mm512_set1_pd(1.05242);
    multiplication_factor_range = _mm512_set1_pd(0.48985582);
    addition_factor_perlin = _mm512_set1_pd(0.985);
    multiplication_factor_perlin = _mm512_set1_pd(1.59);
    POINT_FIVE = _mm512_set1_pd(0.5);
    // Constant vectors used
    ONE_PD = _mm512_set1_pd(1.0);
    ONE_PD_SHORT = _mm256_set1_pd(1.0);
    epsy = _mm512_set1_pd(1.0 - EPSILON);
    short_epsy = _mm256_set1_pd(1.0 - EPSILON);
    vector_allhigh = _mm256_set1_epi32(0xfff);
    short_si128 = _mm_set1_epi32(0xfff);
    three_vector_pd = _mm512_set1_pd(3.0);
    short_three_vector_pd = _mm256_set1_pd(3.0);
    zero_vector_pd = _mm512_setzero_pd();
    short_zero_vector_pd = _mm256_setzero_pd();
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   AVX512Noise
*
* INPUT
*
*   EPoint -- 3-D point at which noise is evaluated
*
* OUTPUT
*
* RETURNS
*
*   DBL noise value
*
* AUTHOR
*
*   Robert Skinner based on Ken Perlin
*
* DESCRIPTION
*
* Used for computation of Noise based on single input
* The function is used whenever a single Vector3D input needs to be processed at a time - Uses AVX2 calls to process data for a single input
* Initial computation done for the single Vector3D, followed by Hash Compuatation, sum accumulation and reduction to noise
*
* CHANGES
*   Modified by AAC to ensure uniformly distributed clamped values
*   between 0 and 1.0...
*
*   Feb 8, 2001: modified function based on MegaPov 0.7 to remove
*                bugs that showed up when noise was translated.
*
*
******************************************************************************/

DBL AVX512Noise(const Vector3d& EPoint, int noise_generator)
{
    AVX512TABLETYPE* mp;
    DBL sum = 0.0;

    // TODO FIXME - global statistics reference
    // Stats[Calls_To_Noise]++;

    if (noise_generator == kNoiseGen_Perlin)
    {
        // The 1.59 and 0.985 are to correct for some biasing problems with
        // the random # generator used to create the noise tables.  Final
        // range of values is about 5.0e-4 below 0.0 and above 1.0.  Mean
        // value is 0.49 (ideally it would be 0.5).
        sum = 0.5 * (1.59 * SolidNoise(EPoint) + 0.985);

        // Clamp final value to 0-1 range
        if (sum < 0.0) sum = 0.0;
        if (sum > 1.0) sum = 1.0;

        return sum;
    }

    int ints[4];
    // Initialised input EPoint into xyzn and also a vector used for noise min values
    const __m256d xyzn = _mm256_setr_pd(EPoint[X], EPoint[Y], EPoint[Z], 0);
    const __m128i noise_min_xyzn = _mm_setr_epi32(NOISE_MINX, NOISE_MINY, NOISE_MINZ, 0);

    __m256d xyzn_e, xyz_ixyzn, xyz_jxyzn, s_xyzn, t_xyzn;
    __m128i tmp_xyzn, i_xyzn;

    // Initial set of computation - Computes the vectors defined above, i_xyzn used to generate hashes, s_xyzn, t_xyzn used to compute vectors that will be blended to form multiplicands for noise accumulation
    COMPUTE_INITIAL_VECTORS_AVX(xyzn);

    __m256d txtysxsy, txsxtxsx, tytysysy, txtysxtytxsysxsy, incrsump_s1, incrsump_s2;

    // Used in computing incrsump_s1 and incrsump_s2 whose values are taken one by one and used for noise acculumulation calculation
    COMPUTE_INCRSUMP_AVX(t_xyzn, s_xyzn);

    // Hash values generated using combination of ints[0] and ints[1]
    int ixiy_hash, jxiy_hash, ixjy_hash, jxjy_hash;
    COMPUTE_IJ_HASHES(ixiy_hash, jxiy_hash, ixjy_hash, jxjy_hash, ints[0], ints[1]);

    const int iz = ints[2];

    // Multiplicand inputs - Typically of the form [0.5, a, b, c]
    const __m256d iii = _mm256_blend_pd(PERMUTE4x64(xyz_ixyzn, _MM_SHUFFLE(2, 1, 0, 0)), _mm256_set_pd(0, 0, 0, 0.5), 0x1);
    const __m256d jjj = _mm256_blend_pd(PERMUTE4x64(xyz_jxyzn, _MM_SHUFFLE(2, 1, 0, 0)), _mm256_set_pd(0, 0, 0, 0.5), 0x1);

    __m256d sumr = _mm256_setzero_pd();
    __m256d sumr1 = _mm256_setzero_pd();

    // Gets references from table for all 8 combinations of hash (2^3 from ints[0], ints[1], ints[2]) and uses them along with other precomputed values for noise accumulation
    mp = &AVX512RTable[Hash1dRTableIndexAVX(ixiy_hash, iz)];
    INCSUMAVX_NOBLEND(sumr, mp, PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(0, 0, 0, 0)), iii);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(jxiy_hash, iz)];
    INCSUMAVX(sumr1, mp, PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(1, 1, 1, 1)), iii, jjj, 2);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(ixjy_hash, iz)];
    INCSUMAVX(sumr, mp, PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(2, 2, 2, 2)), iii, jjj, 4);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(jxjy_hash, iz)];
    INCSUMAVX(sumr1, mp, PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(3, 3, 3, 3)), iii, jjj, 6);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(ixiy_hash, iz + 1)];
    INCSUMAVX(sumr, mp, PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(0, 0, 0, 0)), iii, jjj, 8);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(jxiy_hash, iz + 1)];
    INCSUMAVX(sumr1, mp, PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(1, 1, 1, 1)), iii, jjj, 10);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(ixjy_hash, iz + 1)];
    INCSUMAVX(sumr, mp, PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(2, 2, 2, 2)), iii, jjj, 12);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(jxjy_hash, iz + 1)];
    INCSUMAVX_NOBLEND(sumr1, mp, PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(3, 3, 3, 3)), jjj);

    // Reduction to get resultant noise
    {
        sumr = _mm256_add_pd(sumr, sumr1);

        __m128d sumr_up = _mm256_extractf128_pd(sumr, 1);
        sumr_up = _mm_add_pd(_mm256_castpd256_pd128(sumr), sumr_up);
        sumr_up = _mm_hadd_pd(sumr_up, sumr_up);
        sum = _mm_cvtsd_f64(sumr_up);
    }

    if (noise_generator == kNoiseGen_RangeCorrected)
    {
        /* details of range here:
        Min, max: -1.05242, 0.988997
        Mean: -0.0191481, Median: -0.535493, Std Dev: 0.256828

        We want to change it to as close to [0,1] as possible.
        */
        sum += 1.05242;
        sum *= 0.48985582;
        /*sum *= 0.5;
        sum += 0.5;*/

        if (sum < 0.0)
            sum = 0.0;
        if (sum > 1.0)
            sum = 1.0;
    }
    else
    {
        sum = sum + 0.5;                     /* range at this point -0.5 - 0.5... */

        if (sum < 0.0)
            sum = 0.0;
        if (sum > 1.0)
            sum = 1.0;
    }



#if CHECK_FUNCTIONAL
    {
        DBL orig_sum = PortableNoise(EPoint, noise_generator);
        if (fabs(orig_sum - sum) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }

    }

#endif

    _mm256_zeroupper();
    return (sum);
}


/*****************************************************************************
*
* FUNCTION
*
*   AVX512DNoise
*
* INPUT
*
*   EPoint -- 3-D point at which noise is evaluated
*
* OUTPUT
*
*   Vector3d& result
*
* RETURNS
*
* AUTHOR
*
*   Robert Skinner based on Ken Perlin
*
* DESCRIPTION
*
*   Vector-valued version of "Noise"
*   Used for computation of DNoise based on single input
*   The function is used whenever a single Vector3D input needs to be processed at a time - Uses AVX2 calls to process data for a single input
*   Initial computation done for the single Vector3D, followed by Hash Compuatation, sum accumulation and reduction to result vector of DNoise
*
* CHANGES
*   Modified by AAC to ensure uniformly distributed clamped values
*   between 0 and 1.0...
*
*   Feb 8, 2001: modified function based on MegaPov 0.7 to remove
*                bugs that showed up when noise was translated.
*
*
******************************************************************************/

void AVX512DNoise(Vector3d& result, const Vector3d& EPoint)
{
#if CHECK_FUNCTIONAL
    Vector3d param(EPoint);
#endif

    AVX512TABLETYPE* mp;

    // TODO FIXME - global statistics reference
    // Stats[Calls_To_DNoise]++;

    int ints[4];
    // Initialised input EPoint into xyzn and also a vector used for noise min values
    const __m256d xyzn = _mm256_setr_pd(EPoint[X], EPoint[Y], EPoint[Z], 0);
    const __m128i noise_min_xyzn = _mm_setr_epi32(NOISE_MINX, NOISE_MINY, NOISE_MINZ, 0);

    __m256d xyzn_e, xyz_ixyzn, xyz_jxyzn, s_xyzn, t_xyzn;
    __m128i tmp_xyzn, i_xyzn;

    // Initial set of computation - Computes the vectors defined above, i_xyzn used to generate hashes, s_xyzn, t_xyzn used to compute vectors that will be blended to form multiplicands for DNoise output accumulation
    COMPUTE_INITIAL_VECTORS_AVX(xyzn);

    __m256d txtysxsy, txsxtxsx, tytysysy, txtysxtytxsysxsy, incrsump_s1, incrsump_s2;

    // Used in computing incrsump_s1 and incrsump_s2 whose values are taken one by one and used for DNoise acculumulation calculation
    COMPUTE_INCRSUMP_AVX(t_xyzn, s_xyzn);

    int ixiy_hash, jxiy_hash, ixjy_hash, jxjy_hash;
    // Hash values generated using combination of ints[0] and ints[1]
    COMPUTE_IJ_HASHES(ixiy_hash, jxiy_hash, ixjy_hash, jxjy_hash, ints[0], ints[1]);

    const int iz = ints[2];

    // Multiplicand inputs - Typically of the form [0.5, a, b, c]
    const __m256d iii = _mm256_blend_pd(PERMUTE4x64(xyz_ixyzn, _MM_SHUFFLE(2, 1, 0, 0)), _mm256_set_pd(0, 0, 0, 0.5), 0x1);
    const __m256d jjj = _mm256_blend_pd(PERMUTE4x64(xyz_jxyzn, _MM_SHUFFLE(2, 1, 0, 0)), _mm256_set_pd(0, 0, 0, 0.5), 0x1);

    __m256d ss;
    __m256d blend;

    __m256d x = _mm256_setzero_pd(), y = _mm256_setzero_pd(), z = _mm256_setzero_pd();

    // Gets references from table for all 8 combinations of hash (2^3 from ints[0], ints[1], ints[2]) and uses them along with other precomputed values for DNoise accumulation
    mp = &AVX512RTable[Hash1dRTableIndexAVX(ixiy_hash, iz)];
    ss = PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(0, 0, 0, 0));
    //     blend = _mm256_blend_pd(iii, jjj, 0);

    INCSUMAVX_VECTOR(mp, ss, iii);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(jxiy_hash, iz)];
    ss = PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(1, 1, 1, 1));
    blend = _mm256_blend_pd(iii, jjj, 2);

    INCSUMAVX_VECTOR(mp, ss, blend);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(jxjy_hash, iz)];
    ss = PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(3, 3, 3, 3));
    blend = _mm256_blend_pd(iii, jjj, 6);

    INCSUMAVX_VECTOR(mp, ss, blend);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(ixjy_hash, iz)];
    ss = PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(2, 2, 2, 2));
    blend = _mm256_blend_pd(iii, jjj, 4);

    INCSUMAVX_VECTOR(mp, ss, blend);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(ixjy_hash, iz + 1)];
    ss = PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(2, 2, 2, 2));
    blend = _mm256_blend_pd(iii, jjj, 12);

    INCSUMAVX_VECTOR(mp, ss, blend);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(jxjy_hash, iz + 1)];
    ss = PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(3, 3, 3, 3));
    //     blend = _mm256_blend_pd(iii, jjj, 14);

    INCSUMAVX_VECTOR(mp, ss, jjj);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(jxiy_hash, iz + 1)];
    ss = PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(1, 1, 1, 1));
    blend = _mm256_blend_pd(iii, jjj, 10);

    INCSUMAVX_VECTOR(mp, ss, blend);

    mp = &AVX512RTable[Hash1dRTableIndexAVX(ixiy_hash, iz + 1)];
    ss = PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(0, 0, 0, 0));
    blend = _mm256_blend_pd(iii, jjj, 8);

    INCSUMAVX_VECTOR(mp, ss, blend);

    // Reduction to get overall DNoise output result vector
    __m256d xy = _mm256_hadd_pd(x, y);
    __m128d xy_up = _mm256_extractf128_pd(xy, 1);
    xy_up = _mm_add_pd(_mm256_castpd256_pd128(xy), xy_up);
    _mm_storeu_pd(&result[X], xy_up);

    __m128d z_up = _mm256_extractf128_pd(z, 1);
    z_up = _mm_add_pd(_mm256_castpd256_pd128(z), z_up);
    z_up = _mm_hadd_pd(z_up, z_up);
    result[Z] = _mm_cvtsd_f64(z_up);


#if CHECK_FUNCTIONAL
    {
        Vector3d portable_res;
        PortableDNoise(portable_res, param);
        if (fabs(portable_res[X] - result[X]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("DNoise X error");
        }
        if (fabs(portable_res[Y] - result[Y]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("DNoise Y error");
        }
        if (fabs(portable_res[Z] - result[Z]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("DNoise Z error");
        }

    }

#endif



    _mm256_zeroupper();
    return;
}

/*****************************************************************************
*
* FUNCTION
*
*   AVX512Noise2D
*
* INPUT
*
*   EPoint -- Two input 3-D points at which noise is evaluated
*
* OUTPUT
*
*    double& value - Noise values for the input 3-D points
*
* RETURNS
*
* AUTHOR
*
*   Robert Skinner based on Ken Perlin
*
* DESCRIPTION
*
*   Used for computation of Noise based on two input
*   The function is used whenever two Vector3D input needs to be processed at a time - Uses AVX512 calls to process data for a two inputs
*   Initial computation done for the both Vector3D, followed by Hash Compuatation, sum accumulation and reduction to noise for each of the input 3D points
*
* CHANGES
*   Modified by AAC to ensure uniformly distributed clamped values
*   between 0 and 1.0...
*
*   Feb 8, 2001: modified function based on MegaPov 0.7 to remove
*                bugs that showed up when noise was translated.
*
*
******************************************************************************/

// Used for computation of Noise based on two inputs
void AVX512Noise2D(const Vector3d& EPoint, int noise_generator, double& value) {

    AVX512TABLETYPE* mp1, * mp2;
    DBL* sum = &value;
    sum[0] = sum[1] = 0.0;
    const Vector3d* input_vector;
    input_vector = &EPoint;

    if (noise_generator == kNoiseGen_Perlin)
    {
        // The 1.59 and 0.985 are to correct for some biasing problems with
        // the random # generator used to create the noise tables.  Final
        // range of values is about 5.0e-4 below 0.0 and above 1.0.  Mean
        // value is 0.49 (ideally it would be 0.5).
        sum[0] = 0.5 * (1.59 * SolidNoise(input_vector[0]) + 0.985);

        // Clamp final value to 0-1 range
        if (sum[0] < 0.0) sum[0] = 0.0;
        if (sum[0] > 1.0) sum[0] = 1.0;

        sum[1] = 0.5 * (1.59 * SolidNoise(input_vector[1]) + 0.985);
        if (sum[1] < 0.0) sum[1] = 0.0;
        if (sum[1] > 1.0) sum[1] = 1.0;

        return;
    }

    int ints[8];
    // Initialised input EPoints into xyzn and also a vector used for noise min values
    const __m512d xyzn = _mm512_setr_pd(input_vector[0][X], input_vector[0][Y], input_vector[0][Z], 0, input_vector[1][X], input_vector[1][Y], input_vector[1][Z], 0);
    const  __m256i noise_min_xyzn = _mm256_setr_epi32(NOISE_MINX, NOISE_MINY, NOISE_MINZ, 0, NOISE_MINX, NOISE_MINY, NOISE_MINZ, 0);
    __m512d xyzn_e, s_xyzn_extended, t_xyzn, xyz_ixyzn_extended, xyz_jxyzn_extended, iii1, jjj1, iii2, jjj2;
    __m256i tmp_xyzn, i_xyzn;
    __mmask8 masktmp_xyzn;

    // Initial set of computation - Computes the vectors defined above, i_xyzn used to generate hashes, s_xyzn_extended, t_xyzn used to compute vectors that will be blended to form multiplicands for Noise output accumulation
    COMPUTE_INITIAL_VECTORS_AVX512(xyzn);

    __m512d incrsump_s11, incrsump_s12, incrsump_s13, incrsump_s21, incrsump_s22, incrsump_s23;

    // Used in computing incrsump_s1 and incrsump_s2 whose values are taken one by one and used for Noise acculumulation calculation
    COMPUTE_INCRSUMP_AVX512(t_xyzn, s_xyzn_extended);

    int ixiy_hash1, jxiy_hash1, ixjy_hash1, jxjy_hash1;

    // Hash values generated using combination of ints[0] and ints[1]
    COMPUTE_IJ_HASHES(ixiy_hash1, jxiy_hash1, ixjy_hash1, jxjy_hash1, ints[0], ints[1]);

    const int iz1 = ints[2];

    // Multiplicand inputs - Typically of the form [0.5, a, b, c, 0.5, d, e, f]
    COMPUTE_MULTIPLICAND_INPUTS(xyz_ixyzn_extended, xyz_jxyzn_extended);

    __m512d ss1;
    __m512d blend1;
    __m512i blend1mask;
    __m256d m1, m2;
    __m512d c1;
    __m512d sumr1, sumr2;

    // Gets references from table for all 8 combinations of hash (2^3 from ints[0], ints[1], ints[2]) and uses them along with other precomputed values for Noise accumulation
    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1, jxjy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch1);
    INCSUMAVX512_INITIALIZE(sumr1, mp1, mp2, ss1, iii1, jjj1, 0xF0);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1, jxjy_hash1, iz1, incrsump_s11, mask_permute_fetch2);
    INCSUMAVX512(sumr1, mp1, mp2, ss1, iii1, jjj1, 0x64);

    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1 + 1, jxiy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch3);
    INCSUMAVX512(sumr1, mp1, mp2, ss1, iii1, jjj1, 0xA8);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1 + 1, jxiy_hash1, iz1, incrsump_s11, mask_permute_fetch4);
    INCSUMAVX512(sumr1, mp1, mp2, ss1, iii1, jjj1, 0x2C);

    // Similar steps from hash computation done for second input coordinate

    int ixiy_hash2, jxiy_hash2, ixjy_hash2, jxjy_hash2;

    COMPUTE_IJ_HASHES(ixiy_hash2, jxiy_hash2, ixjy_hash2, jxjy_hash2, ints[4], ints[5]);

    const int iz2 = ints[6];

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2, jxjy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch1);
    INCSUMAVX512_INITIALIZE(sumr2, mp1, mp2, ss1, iii2, jjj2, 0xF0);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2, jxjy_hash2, iz2, incrsump_s21, mask_permute_fetch2);
    INCSUMAVX512(sumr2, mp1, mp2, ss1, iii2, jjj2, 0x64);

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2 + 1, jxiy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch3);
    INCSUMAVX512(sumr2, mp1, mp2, ss1, iii2, jjj2, 0xA8);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2 + 1, jxiy_hash2, iz2, incrsump_s21, mask_permute_fetch4);
    INCSUMAVX512(sumr2, mp1, mp2, ss1, iii2, jjj2, 0x2C);

    // Reduction to get noise values for both inputs
    sum[0] = _mm512_reduce_add_pd(sumr1);
    sum[1] = _mm512_reduce_add_pd(sumr2);

    if (noise_generator == kNoiseGen_RangeCorrected)
    {
        /* details of range here:
        Min, max: -1.05242, 0.988997
        Mean: -0.0191481, Median: -0.535493, Std Dev: 0.256828

        We want to change it to as close to [0,1] as possible.
        */
        sum[0] += 1.05242;
        sum[0] *= 0.48985582;

        if (sum[0] < 0.0)
            sum[0] = 0.0;
        if (sum[0] > 1.0)
            sum[0] = 1.0;

        sum[1] += 1.05242;
        sum[1] *= 0.48985582;
        if (sum[1] < 0.0)
            sum[1] = 0.0;
        if (sum[1] > 1.0)
            sum[1] = 1.0;
    }
    else
    {
        sum[0] = sum[0] + 0.5;                     /* range at this point -0.5 - 0.5... */

        if (sum[0] < 0.0)
            sum[0] = 0.0;
        if (sum[0] > 1.0)
            sum[0] = 1.0;

        sum[1] = sum[1] + 0.5;

        if (sum[1] < 0.0)
            sum[1] = 0.0;
        if (sum[1] > 1.0)
            sum[1] = 1.0;
    }

#if CHECK_FUNCTIONAL
    {
        DBL orig_sum[2];
        orig_sum[0] = PortableNoise(input_vector[0], noise_generator);
        orig_sum[1] = PortableNoise(input_vector[1], noise_generator);
        if (fabs(orig_sum[0] - sum[0]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }
        if (fabs(orig_sum[1] - sum[1]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }

    }

#endif
    _mm256_zeroupper();
    return;
}

/*****************************************************************************
*
* FUNCTION
*
*   AVX512Noise2D
*
* INPUT
*
*   EPoint -- Two input 3-D points at which noise is evaluated
*
* OUTPUT
*
*   Vector3D& result - Resultant two 3D points for the two input 3D points
*
* RETURNS
*
* AUTHOR
*
*   Robert Skinner based on Ken Perlin
*
* DESCRIPTION
*
*   Vector valued version of noise for two input 3D points
*   Used for computation of DNoise based on two input
*   The function is used whenever two Vector3D input needs to be processed at a time - Uses AVX512 calls to process data for a two inputs
*   Initial computation done for the both Vector3D, followed by Hash Compuatation, sum accumulation and reduction to result vector for each of the input 3D points
*
*
* CHANGES
*   Modified by AAC to ensure uniformly distributed clamped values
*   between 0 and 1.0...
*
*   Feb 8, 2001: modified function based on MegaPov 0.7 to remove
*                bugs that showed up when noise was translated.
*
*
******************************************************************************/
// Used for computation of DNoise based on two inputs
void AVX512DNoise2D(Vector3d& result, const Vector3d& EPoint) {
    AVX512TABLETYPE* mp1, * mp2;
    // TODO FIXME - global statistics reference
    // Stats[Calls_To_DNoise]++;
    Vector3d* result_vector;
    result_vector = &result;
    const Vector3d* input_vector;
    input_vector = &EPoint;

#if CHECK_FUNCTIONAL
    Vector3d param[2];
    param[0] = input_vector[0];
    param[1] = input_vector[1];
#endif


    int ints[8];
    // Initialised input EPoints into xyzn and also a vector used for noise min values
    const __m512d xyzn = _mm512_setr_pd(input_vector[0][X], input_vector[0][Y], input_vector[0][Z], 0, input_vector[1][X], input_vector[1][Y], input_vector[1][Z], 0);
    const __m256i noise_min_xyzn = _mm256_setr_epi32(NOISE_MINX, NOISE_MINY, NOISE_MINZ, 0, NOISE_MINX, NOISE_MINY, NOISE_MINZ, 0);
    __m512d xyzn_e, s_xyzn_extended, t_xyzn, xyz_ixyzn_extended, xyz_jxyzn_extended, iii1, jjj1, iii2, jjj2;
    __m256i tmp_xyzn, i_xyzn;
    __mmask8 masktmp_xyzn;

    // Initial set of computation - Computes the vectors defined above, i_xyzn used to generate hashes, s_xyzn_extended, t_xyzn used to compute vectors that will be blended to form multiplicands for dnoise output accumulation
    COMPUTE_INITIAL_VECTORS_AVX512(xyzn);

    __m512d incrsump_s11, incrsump_s12, incrsump_s13, incrsump_s21, incrsump_s22, incrsump_s23;

    // Used in computing incrsump_s1 and incrsump_s2 whose values are taken one by one and used for DNoise acculumulation calculation
    COMPUTE_INCRSUMP_AVX512(t_xyzn, s_xyzn_extended);

    int ixiy_hash1, jxiy_hash1, ixjy_hash1, jxjy_hash1;

    // Hash values generated using combination of ints[0] and ints[1]
    COMPUTE_IJ_HASHES(ixiy_hash1, jxiy_hash1, ixjy_hash1, jxjy_hash1, ints[0], ints[1]);

    const int iz1 = ints[2];

    // Multiplicand inputs - Typically of the form [0.5, a, b, c, 0.5, d, e, f]
    COMPUTE_MULTIPLICAND_INPUTS(xyz_ixyzn_extended, xyz_jxyzn_extended);

    __m512d ss1;
    __m512d blend1;
    __m512i blend1mask;
    __m256d m1, m2;
    __m512d c1;
    __m512d x1, y1, z1;

    // Gets references from table for all 8 combinations of hash (2^3 from ints[0], ints[1], ints[2]) and uses them along with other precomputed values for DNoise accumulation
    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1, jxjy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch1);
    blend1 = _mm512_mask_blend_pd(0xF0, iii1, jjj1);
    INCSUMAVX512_VECTOR_INITIALIZE(x1, y1, z1, mp1, mp2, ss1, blend1);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1, jxjy_hash1, iz1, incrsump_s11, mask_permute_fetch2);
    blend1 = _mm512_mask_blend_pd(0x64, iii1, jjj1);
    INCSUMAVX512_VECTOR(x1, y1, z1, mp1, mp2, ss1, blend1);

    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1 + 1, jxiy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch3);
    blend1 = _mm512_mask_blend_pd(0xA8, iii1, jjj1);
    INCSUMAVX512_VECTOR(x1, y1, z1, mp1, mp2, ss1, blend1);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1 + 1, jxiy_hash1, iz1, incrsump_s11, mask_permute_fetch4);
    blend1 = _mm512_mask_blend_pd(0x2C, iii1, jjj1);
    INCSUMAVX512_VECTOR(x1, y1, z1, mp1, mp2, ss1, blend1);


    // Similar steps from hash computation done for second input coordinate
    int ixiy_hash2, jxiy_hash2, ixjy_hash2, jxjy_hash2;
    COMPUTE_IJ_HASHES(ixiy_hash2, jxiy_hash2, ixjy_hash2, jxjy_hash2, ints[4], ints[5]);

    const int iz2 = ints[6];

    __m512d x2, y2, z2;

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2, jxjy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch1);
    blend1 = _mm512_mask_blend_pd(0xF0, iii2, jjj2);
    INCSUMAVX512_VECTOR_INITIALIZE(x2, y2, z2, mp1, mp2, ss1, blend1);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2, jxjy_hash2, iz2, incrsump_s21, mask_permute_fetch2);
    blend1 = _mm512_mask_blend_pd(0x64, iii2, jjj2);
    INCSUMAVX512_VECTOR(x2, y2, z2, mp1, mp2, ss1, blend1);

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2 + 1, jxiy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch3);
    blend1 = _mm512_mask_blend_pd(0xA8, iii2, jjj2);
    INCSUMAVX512_VECTOR(x2, y2, z2, mp1, mp2, ss1, blend1);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2 + 1, jxiy_hash2, iz2, incrsump_s21, mask_permute_fetch4);
    blend1 = _mm512_mask_blend_pd(0x2C, iii2, jjj2);
    INCSUMAVX512_VECTOR(x2, y2, z2, mp1, mp2, ss1, blend1);

    // Reduction to get result vectors
    result_vector[0][X] = _mm512_reduce_add_pd(x1);
    result_vector[0][Y] = _mm512_reduce_add_pd(y1);
    result_vector[0][Z] = _mm512_reduce_add_pd(z1);
    result_vector[1][X] = _mm512_reduce_add_pd(x2);
    result_vector[1][Y] = _mm512_reduce_add_pd(y2);
    result_vector[1][Z] = _mm512_reduce_add_pd(z2);

#if CHECK_FUNCTIONAL
    {
        Vector3d portable_res[2];
        PortableDNoise(portable_res[0], param[0]);
        PortableDNoise(portable_res[1], param[1]);
        if (fabs(portable_res[0][X] - result_vector[0][X]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("DNoise X error");
        }
        if (fabs(portable_res[0][Y] - result_vector[0][Y]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("DNoise Y error");
        }
        if (fabs(portable_res[0][Z] - result_vector[0][Z]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("DNoise Z error");
        }
        if (fabs(portable_res[1][X] - result_vector[1][X]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("DNoise X error");
        }
        if (fabs(portable_res[1][Y] - result_vector[1][Y]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("DNoise Y error");
        }
        if (fabs(portable_res[1][Z] - result_vector[1][Z]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("DNoise Z error");
        }

    }

#endif

    _mm256_zeroupper();
    return;
}

/*****************************************************************************
*
* FUNCTION
*
*   AVX512Noise8D
*
* INPUT
*
*   EPoint -- Input 3-D point whose noise and its multiples noise is evaluated
*
* OUTPUT
*
* RETURNS
*
*   sum_value - Noise values of all 8 eight multiples of input are multiplied with omega_values and reduced add to get the resultant cumulative noise value
*
* AUTHOR
*
*   Robert Skinner based on Ken Perlin
*
* DESCRIPTION
*
*   Used for computation of Noise for 8 multiples of a single input
*   Uses AVX512 calls to process data for the 8 input points based on a single point
*   Initial computation done for the two muliples of input Vector3D, followed by Hash Compuatation, sum accumulation and reduction to noise for each of the two 3D points
*   The above point is repeated four times and input of the next iteration is derived from the input of the previous
*
* CHANGES
*   Modified by AAC to ensure uniformly distributed clamped values
*   between 0 and 1.0...
*
*   Feb 8, 2001: modified function based on MegaPov 0.7 to remove
*                bugs that showed up when noise was translated.
*
*
******************************************************************************/
DBL AVX512Noise8D(const Vector3d& EPoint, int noise_generator) {

    AVX512TABLETYPE* mp1, * mp2;
    //DBL* sum = &value;
    //sum[0] = sum[1] = 0.0;
    DBL sum[8], sum_value;

    if (noise_generator == kNoiseGen_Perlin)
    {
        // The 1.59 and 0.985 are to correct for some biasing problems with
        // the random # generator used to create the noise tables.  Final
        // range of values is about 5.0e-4 below 0.0 and above 1.0.  Mean
        // value is 0.49 (ideally it would be 0.5).
        Vector3d EPoint_temp;
        EPoint_temp = EPoint * 2.0;
        sum[0] = SolidNoise(EPoint_temp);
        EPoint_temp = EPoint_temp * 2.0;
        sum[1] = SolidNoise(EPoint_temp);
        EPoint_temp = EPoint_temp * 2.0;
        sum[2] = SolidNoise(EPoint_temp);
        EPoint_temp = EPoint_temp * 2.0;
        sum[3] = SolidNoise(EPoint_temp);
        EPoint_temp = EPoint_temp * 2.0;
        sum[4] = SolidNoise(EPoint_temp);
        EPoint_temp = EPoint_temp * 2.0;
        sum[5] = SolidNoise(EPoint_temp);
        EPoint_temp = EPoint_temp * 2.0;
        sum[6] = SolidNoise(EPoint_temp);
        EPoint_temp = EPoint_temp * 2.0;
        sum[7] = SolidNoise(EPoint_temp);

        __m512d sum_vector = _mm512_loadu_pd(sum);
        sum_vector = _mm512_mul_pd(sum_vector, multiplication_factor_perlin);
        sum_vector = _mm512_add_pd(sum_vector, addition_factor_perlin);
        sum_vector = _mm512_mul_pd(sum_vector, POINT_FIVE);
        __mmask8 mask_zero = _mm512_cmplt_pd_mask(sum_vector, zero_vector_pd);
        sum_vector = _mm512_mask_mov_pd(sum_vector, mask_zero, zero_vector_pd);
        __mmask8 mask_one = _mm512_cmple_pd_mask(ONE_PD, sum_vector);
        sum_vector = _mm512_mask_mov_pd(sum_vector, mask_one, ONE_PD);
        sum_vector = _mm512_mul_pd(sum_vector, TWO_PD);
        sum_vector = _mm512_sub_pd(sum_vector, POINT_FIVE);
        mask_zero = _mm512_cmplt_pd_mask(sum_vector, zero_vector_pd);
        sum_vector = _mm512_mask_mov_pd(sum_vector, mask_zero, zero_vector_pd);
        mask_one = _mm512_cmple_pd_mask(ONE_PD, sum_vector);
        sum_vector = _mm512_mul_pd(sum_vector, omega_values);
        sum_value = _mm512_reduce_add_pd(sum_vector);
        return sum_value;

}

    int ints[8];
    Vector3d EPointOne = EPoint * 2.0;
    Vector3d EPointTwo = EPointOne * 2.0;

    // Initialised input EPoints into xyzn and also a vector used for noise min values
    __m512d xyzn = _mm512_setr_pd(EPointOne[X], EPointOne[Y], EPointOne[Z], 0, EPointTwo[X], EPointTwo[Y], EPointTwo[Z], 0);
    const __m256i noise_min_xyzn = _mm256_setr_epi32(NOISE_MINX, NOISE_MINY, NOISE_MINZ, 0, NOISE_MINX, NOISE_MINY, NOISE_MINZ, 0);
    __m512d xyzn_e, s_xyzn_extended, t_xyzn, xyz_ixyzn_extended, xyz_jxyzn_extended, iii1, jjj1, iii2, jjj2;
    __m256i tmp_xyzn, i_xyzn;
    __mmask8 masktmp_xyzn;

    // Initial set of computation - Computes the vectors defined above, i_xyzn used to generate hashes, s_xyzn_extended, t_xyzn used to compute vectors that will be blended to form multiplicands for Noise output accumulation
    COMPUTE_INITIAL_VECTORS_AVX512(xyzn);

    __m512d incrsump_s11, incrsump_s12, incrsump_s13, incrsump_s21, incrsump_s22, incrsump_s23;

    // Used in computing incrsump_s1 and incrsump_s2 whose values are taken one by one and used for Noise acculumulation calculation
    COMPUTE_INCRSUMP_AVX512(t_xyzn, s_xyzn_extended);

    int ixiy_hash1, jxiy_hash1, ixjy_hash1, jxjy_hash1;

    // Computes 4 hashes (2^2) based on two inputs - ints[0] and ints[1]
    COMPUTE_IJ_HASHES(ixiy_hash1, jxiy_hash1, ixjy_hash1, jxjy_hash1, ints[0], ints[1]);

    int iz1 = ints[2];

    // Multiplicand inputs - Typically of the form [0.5, a, b, c, 0.5, d, e, f]
    COMPUTE_MULTIPLICAND_INPUTS(xyz_ixyzn_extended, xyz_jxyzn_extended);

    // Noise is accumulated in 8 different vectors which will be reduced to get the noise for input EPoint
    __m512d ss1;
    __m512d blend1;
    __m512i blend1mask;
    __m256d m1, m2;
    __m512d c1;
    __m512d sumr1, sumr2, sumr3, sumr4, sumr5, sumr6, sumr7, sumr8, sum_vector;

    // Gets references from table for all 8 combinations of hash (2^3 from ints[0], ints[1], ints[2]) and uses them along with other precomputed values for Noise accumulation
    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1, jxjy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch1);
    INCSUMAVX512_INITIALIZE(sumr1,mp1,mp2,ss1,iii1,jjj1,0xF0);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1, jxjy_hash1, iz1, incrsump_s11, mask_permute_fetch2);
    INCSUMAVX512(sumr1,mp1,mp2,ss1,iii1,jjj1,0x64);

    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1 + 1, jxiy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch3);
    INCSUMAVX512(sumr1,mp1,mp2,ss1,iii1,jjj1,0xA8);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1 + 1, jxiy_hash1, iz1, incrsump_s11, mask_permute_fetch4);
    INCSUMAVX512(sumr1,mp1,mp2,ss1,iii1,jjj1,0x2C);

    int ixiy_hash2, jxiy_hash2, ixjy_hash2, jxjy_hash2;

    // Hashing and similar compuatations for second vector
    COMPUTE_IJ_HASHES(ixiy_hash2, jxiy_hash2, ixjy_hash2, jxjy_hash2, ints[4], ints[5]);

    int iz2 = ints[6];

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2, jxjy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch1);
    INCSUMAVX512_INITIALIZE(sumr2,mp1,mp2,ss1,iii2,jjj2,0xF0);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2, jxjy_hash2, iz2, incrsump_s21, mask_permute_fetch2);
    INCSUMAVX512(sumr2,mp1,mp2,ss1,iii2,jjj2,0x64);

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2 + 1, jxiy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch3);
    INCSUMAVX512(sumr2,mp1,mp2,ss1,iii2,jjj2,0xA8);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2 + 1, jxiy_hash2, iz2, incrsump_s21, mask_permute_fetch4);
    INCSUMAVX512(sumr2,mp1,mp2,ss1,iii2,jjj2,0x2C);

    // For the next set of computations EPoint * 8, EPoint * 16 is used for computation after storing it in xyzn, similar to EPoint * 2, EPoint * 4
    xyzn = _mm512_mul_pd(xyzn, FOUR_PD);
    COMPUTE_INITIAL_VECTORS_AVX512(xyzn);

    COMPUTE_INCRSUMP_AVX512(t_xyzn, s_xyzn_extended);

    COMPUTE_IJ_HASHES(ixiy_hash1, jxiy_hash1, ixjy_hash1, jxjy_hash1, ints[0], ints[1]);

    iz1 = ints[2];

    COMPUTE_MULTIPLICAND_INPUTS(xyz_ixyzn_extended, xyz_jxyzn_extended);

    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1, jxjy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch1);
    INCSUMAVX512_INITIALIZE(sumr3, mp1, mp2, ss1, iii1, jjj1, 0xF0);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1, jxjy_hash1, iz1, incrsump_s11, mask_permute_fetch2);
    INCSUMAVX512(sumr3, mp1, mp2, ss1, iii1, jjj1, 0x64);

    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1 + 1, jxiy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch3);
    INCSUMAVX512(sumr3, mp1, mp2, ss1, iii1, jjj1, 0xA8);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1 + 1, jxiy_hash1, iz1, incrsump_s11, mask_permute_fetch4);
    INCSUMAVX512(sumr3, mp1, mp2, ss1, iii1, jjj1, 0x2C);

    COMPUTE_IJ_HASHES(ixiy_hash2, jxiy_hash2, ixjy_hash2, jxjy_hash2, ints[4], ints[5]);

    iz2 = ints[6];

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2, jxjy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch1);
    INCSUMAVX512_INITIALIZE(sumr4, mp1, mp2, ss1, iii2, jjj2, 0xF0);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2, jxjy_hash2, iz2, incrsump_s21, mask_permute_fetch2);
    INCSUMAVX512(sumr4, mp1, mp2, ss1, iii2, jjj2, 0x64);

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2 + 1, jxiy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch3);
    INCSUMAVX512(sumr4, mp1, mp2, ss1, iii2, jjj2, 0xA8);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2 + 1, jxiy_hash2, iz2, incrsump_s21, mask_permute_fetch4);
    INCSUMAVX512(sumr4, mp1, mp2, ss1, iii2, jjj2, 0x2C);

    // Computation for EPoint * 32, EPoint * 64
    xyzn = _mm512_mul_pd(xyzn, FOUR_PD);
    COMPUTE_INITIAL_VECTORS_AVX512(xyzn);

    COMPUTE_INCRSUMP_AVX512(t_xyzn, s_xyzn_extended);

    COMPUTE_IJ_HASHES(ixiy_hash1, jxiy_hash1, ixjy_hash1, jxjy_hash1, ints[0], ints[1]);

    iz1 = ints[2];

    COMPUTE_MULTIPLICAND_INPUTS(xyz_ixyzn_extended, xyz_jxyzn_extended);

    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1, jxjy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch1);
    INCSUMAVX512_INITIALIZE(sumr5, mp1, mp2, ss1, iii1, jjj1, 0xF0);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1, jxjy_hash1, iz1, incrsump_s11, mask_permute_fetch2);
    INCSUMAVX512(sumr5, mp1, mp2, ss1, iii1, jjj1, 0x64);

    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1 + 1, jxiy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch3);
    INCSUMAVX512(sumr5, mp1, mp2, ss1, iii1, jjj1, 0xA8);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1 + 1, jxiy_hash1, iz1, incrsump_s11, mask_permute_fetch4);
    INCSUMAVX512(sumr5, mp1, mp2, ss1, iii1, jjj1, 0x2C);

    COMPUTE_IJ_HASHES(ixiy_hash2, jxiy_hash2, ixjy_hash2, jxjy_hash2, ints[4], ints[5]);

    iz2 = ints[6];

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2, jxjy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch1);
    INCSUMAVX512_INITIALIZE(sumr6, mp1, mp2, ss1, iii2, jjj2, 0xF0);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2, jxjy_hash2, iz2, incrsump_s21, mask_permute_fetch2);
    INCSUMAVX512(sumr6, mp1, mp2, ss1, iii2, jjj2, 0x64);

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2 + 1, jxiy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch3);
    INCSUMAVX512(sumr6, mp1, mp2, ss1, iii2, jjj2, 0xA8);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2 + 1, jxiy_hash2, iz2, incrsump_s21, mask_permute_fetch4);
    INCSUMAVX512(sumr6, mp1, mp2, ss1, iii2, jjj2, 0x2C);

    // Computation for EPoint * 128, EPoint * 256
    xyzn = _mm512_mul_pd(xyzn, FOUR_PD);
    COMPUTE_INITIAL_VECTORS_AVX512(xyzn);

    COMPUTE_INCRSUMP_AVX512(t_xyzn, s_xyzn_extended);

    COMPUTE_IJ_HASHES(ixiy_hash1, jxiy_hash1, ixjy_hash1, jxjy_hash1, ints[0], ints[1]);

    iz1 = ints[2];

    COMPUTE_MULTIPLICAND_INPUTS(xyz_ixyzn_extended, xyz_jxyzn_extended);

    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1, jxjy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch1);
    INCSUMAVX512_INITIALIZE(sumr7, mp1, mp2, ss1, iii1, jjj1, 0xF0);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1, jxjy_hash1, iz1, incrsump_s11, mask_permute_fetch2);
    INCSUMAVX512(sumr7, mp1, mp2, ss1, iii1, jjj1, 0x64);

    GET_INCSUMAVX512_INPUTS(ixiy_hash1, iz1 + 1, jxiy_hash1, iz1 + 1, incrsump_s11, mask_permute_fetch3);
    INCSUMAVX512(sumr7, mp1, mp2, ss1, iii1, jjj1, 0xA8);

    GET_INCSUMAVX512_INPUTS(ixjy_hash1, iz1 + 1, jxiy_hash1, iz1, incrsump_s11, mask_permute_fetch4);
    INCSUMAVX512(sumr7, mp1, mp2, ss1, iii1, jjj1, 0x2C);

    COMPUTE_IJ_HASHES(ixiy_hash2, jxiy_hash2, ixjy_hash2, jxjy_hash2, ints[4], ints[5]);

    iz2 = ints[6];

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2, jxjy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch1);
    INCSUMAVX512_INITIALIZE(sumr8, mp1, mp2, ss1, iii2, jjj2, 0xF0);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2, jxjy_hash2, iz2, incrsump_s21, mask_permute_fetch2);
    INCSUMAVX512(sumr8, mp1, mp2, ss1, iii2, jjj2, 0x64);

    GET_INCSUMAVX512_INPUTS(ixiy_hash2, iz2 + 1, jxiy_hash2, iz2 + 1, incrsump_s21, mask_permute_fetch3);
    INCSUMAVX512(sumr8, mp1, mp2, ss1, iii2, jjj2, 0xA8);

    GET_INCSUMAVX512_INPUTS(ixjy_hash2, iz2 + 1, jxiy_hash2, iz2, incrsump_s21, mask_permute_fetch4);
    INCSUMAVX512(sumr8, mp1, mp2, ss1, iii2, jjj2, 0x2C);

    // Reduction of values to get noise for each EPoint whcih we load in a vector
    sum[0] = _mm512_reduce_add_pd(sumr1);
    sum[1] = _mm512_reduce_add_pd(sumr2);
    sum[2] = _mm512_reduce_add_pd(sumr3);
    sum[3] = _mm512_reduce_add_pd(sumr4);
    sum[4] = _mm512_reduce_add_pd(sumr5);
    sum[5] = _mm512_reduce_add_pd(sumr6);
    sum[6] = _mm512_reduce_add_pd(sumr7);
    sum[7] = _mm512_reduce_add_pd(sumr8);


    if (noise_generator == kNoiseGen_RangeCorrected)
    {
        /* details of range here:
        Min, max: -1.05242, 0.988997
        Mean: -0.0191481, Median: -0.535493, Std Dev: 0.256828

        We want to change it to as close to [0,1] as possible.
        */
        sum_vector = _mm512_loadu_pd(sum);
        sum_vector = _mm512_add_pd(sum_vector, addition_factor_range);
        sum_vector = _mm512_mul_pd(sum_vector, multiplication_factor_range);
        __mmask8 mask_zero = _mm512_cmplt_pd_mask(sum_vector, zero_vector_pd);
        sum_vector = _mm512_mask_mov_pd(sum_vector, mask_zero, zero_vector_pd);
        __mmask8 mask_one = _mm512_cmple_pd_mask(ONE_PD, sum_vector);
        sum_vector = _mm512_mask_mov_pd(sum_vector, mask_one, ONE_PD);
        sum_vector = _mm512_mul_pd(sum_vector, TWO_PD);
        sum_vector = _mm512_sub_pd(sum_vector, POINT_FIVE);
        mask_zero = _mm512_cmplt_pd_mask(sum_vector, zero_vector_pd);
        sum_vector = _mm512_mask_mov_pd(sum_vector, mask_zero, zero_vector_pd);
        mask_one = _mm512_cmple_pd_mask(ONE_PD, sum_vector);
        // Multiplication done with omega values which are multiples of 0.5. Reduced to get a double value that is accumulated in the function call
        sum_vector = _mm512_mul_pd(sum_vector, omega_values);
        sum_value = _mm512_reduce_add_pd(sum_vector);
    }
    else
    {
        /* range at this point -0.5 - 0.5... */

        sum_vector = _mm512_loadu_pd(sum);
        sum_vector = _mm512_add_pd(sum_vector, POINT_FIVE);
        __mmask8 mask_zero = _mm512_cmplt_pd_mask(sum_vector, zero_vector_pd);
        sum_vector = _mm512_mask_mov_pd(sum_vector, mask_zero, zero_vector_pd);
        __mmask8 mask_one = _mm512_cmple_pd_mask(ONE_PD, sum_vector);
        sum_vector = _mm512_mask_mov_pd(sum_vector, mask_one, ONE_PD);
        // Multiplication done with omega values which are multiples of 0.5. Reduced to get a double value that is accumulated in the function cal
        sum_vector = _mm512_mul_pd(sum_vector, omega_values);
        sum_value = _mm512_reduce_add_pd(sum_vector);
    }

#if CHECK_FUNCTIONAL
    {
        DBL orig_sum[8];
        Vector3d EPoint_temp;
        EPoint_temp = EPoint * 2.0;
        orig_sum[0] = PortableNoise(EPoint, noise_generator);
        EPoint_temp = EPoint_temp * 2.0;
        orig_sum[1] = PortableNoise(EPoint, noise_generator);
        EPoint_temp = EPoint_temp * 2.0;
        orig_sum[2] = PortableNoise(EPoint, noise_generator);
        EPoint_temp = EPoint_temp * 2.0;
        orig_sum[3] = PortableNoise(EPoint, noise_generator);
        EPoint_temp = EPoint_temp * 2.0;
        orig_sum[4] = PortableNoise(EPoint, noise_generator);
        EPoint_temp = EPoint_temp * 2.0;
        orig_sum[5] = PortableNoise(EPoint, noise_generator);
        EPoint_temp = EPoint_temp * 2.0;
        orig_sum[6] = PortableNoise(EPoint, noise_generator);
        EPoint_temp = EPoint_temp * 2.0;
        orig_sum[7] = PortableNoise(EPoint, noise_generator);
        if (fabs(orig_sum[0] - sum[0]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }
        if (fabs(orig_sum[1] - sum[1]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }
        if (fabs(orig_sum[2] - sum[2]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }
        if (fabs(orig_sum[3] - sum[3]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }
        if (fabs(orig_sum[4] - sum[4]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }
        if (fabs(orig_sum[5] - sum[5]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }
        if (fabs(orig_sum[6] - sum[6]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }
        if (fabs(orig_sum[7] - sum[7]) >= EPSILON)
        {
            throw POV_EXCEPTION_STRING("Noise error");
        }
    }

#endif
    _mm256_zeroupper();
    return sum_value;
}

#else // DISABLE_OPTIMIZED_NOISE_AVX512

const bool kAVX512NoiseEnabled = false;
void AVX512NoiseInit() { POV_ASSERT(false); }
DBL AVX512Noise(const Vector3d& EPoint, int noise_generator) { POV_ASSERT(false); return 0.0; }
void AVX512DNoise(Vector3d& result, const Vector3d& EPoint) { POV_ASSERT(false); }
void AVX512Noise2D(const Vector3d& EPoint, int noise_generator, double& value) { POV_ASSERT(false); }
void AVX512DNoise2D(Vector3d& result, const Vector3d& EPoint) { POV_ASSERT(false); }
DBL AVX512Noise8D(const Vector3d& EPoint, int noise_generator) { POV_ASSERT(false); return 0.0; }

#endif // DISABLE_OPTIMIZED_NOISE_AVX512

}

#endif // TRY_OPTIMIZED_NOISE_AVX512

