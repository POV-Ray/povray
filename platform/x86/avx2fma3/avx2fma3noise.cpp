//******************************************************************************
///
/// @file platform/x86/avx2fma3/avx2fma3noise.cpp
///
/// This file contains implementations of the noise generator optimized for the
/// AVX2 and FMA3 instruction set.
///
/// @note
///     This file shares lots of code with @ref platform/x86/avxnoise.cpp,
///     essentially differing only in a few macro definitions and some
//      identifier names.
///
/// @author Original optimizations by Intel
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
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
#include "avx2fma3noise.h"

#ifdef MACHINE_INTRINSICS_H
#include MACHINE_INTRINSICS_H
#endif

#include "base/povassert.h"

#include "core/material/noise.h"

/// @file
/// @attention
///     This file **must not** contain any code that might get called before CPU
///     support for this optimized implementation has been confirmed. Most
///     notably, the function to detect support itself must not reside in this
///     file.

/*****************************************************************************/


#ifdef TRY_OPTIMIZED_NOISE_AVX2FMA3

namespace pov
{

#ifndef DISABLE_OPTIMIZED_NOISE_AVX2FMA3

const bool kAVX2FMA3NoiseEnabled = true;

/******************************************************/
/* Use avx2 intrinsics for vpermpd and native fma3    */
/******************************************************/

#define FMA_PD(a,b,c) _mm256_fmadd_pd((a),(b),(c))
#define PERMUTE4x64(a,i) _mm256_permute4x64_pd((a),(i))


#ifdef NO_SPLITS
#define AVX2TABLETYPE __m256d

#define Hash1dRTableIndexAVX(a,b)   \
    (((unsigned char)hashTable[(int)(a) ^ (b)]) * 4)


#define LOAD_32BYTES_FROM_TABLE(m) (*(m))

#else

#define AVX2TABLETYPE DBL

#define Hash1dRTableIndexAVX(a,b)   \
    (((unsigned char)hashTable[(int)(a) ^ (b)]))

#define LOAD_32BYTES_FROM_TABLE(m)  (_mm256_loadu_pd(m))

#endif


#define INCSUMAVX(sum,m, s_vec,i,j,mask) sum = FMA_PD(s_vec,_mm256_mul_pd(_mm256_blend_pd(i,j,mask),LOAD_32BYTES_FROM_TABLE(m)),sum)
#define INCSUMAVX_NOBLEND(sum,m, s_vec,i) sum = FMA_PD(s_vec,_mm256_mul_pd(i,LOAD_32BYTES_FROM_TABLE(m)),sum)

#define INCSUMAVX_VECTOR(m, s,blend) \
    x = FMA_PD(s,_mm256_mul_pd(blend,LOAD_32BYTES_FROM_TABLE(m)),x); \
    m+=4; \
    y = FMA_PD(s,_mm256_mul_pd(blend,LOAD_32BYTES_FROM_TABLE(m)),y); \
    m+=4; \
    z = FMA_PD(s,_mm256_mul_pd(blend,LOAD_32BYTES_FROM_TABLE(m)),z);


/********************************************************************************************/
/* AVX2+FMA3 Specific optimizations: Its found that more than 50% of the time is spent in   */
/* Noise and DNoise. These functions have been optimized using AVX2 and FMA3 instructions   */
/********************************************************************************************/




extern DBL RTable[];

alignas(32) static AVX2TABLETYPE AVX2RTable[267];

void AVX2FMA3NoiseInit()
{
    int i;
    DBL *avx2table = (DBL *)AVX2RTable;
    for (i = 0; i < 267; i++)
    {
#ifndef NO_SPLITS
        avx2table[i] = RTable[2 * i];
#else
        avx2table[(4 * i) + 0] = RTable[2 * i + 0];
        avx2table[(4 * i) + 1] = RTable[2 * i + 2];
        avx2table[(4 * i) + 2] = RTable[2 * i + 4];
        avx2table[(4 * i) + 3] = RTable[2 * i + 6];
#endif
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   AVX2FMA3Noise
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
* CHANGES
*   Modified by AAC to ensure uniformly distributed clamped values
*   between 0 and 1.0...
*
*   Feb 8, 2001: modified function based on MegaPov 0.7 to remove
*                bugs that showed up when noise was translated.
*
*
******************************************************************************/

DBL AVX2FMA3Noise(const Vector3d& EPoint, int noise_generator)
{
    AVX2TABLETYPE *mp;
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

    const __m256d ONE_PD = _mm256_set1_pd(1);
    const __m128i short_si128 = _mm_set1_epi32(0xffff);

    const __m256d xyzn = _mm256_setr_pd(EPoint[X], EPoint[Y], EPoint[Z], 0);
    const __m256d epsy = _mm256_set1_pd(1.0 - EPSILON);
    const __m256d xyzn_e = _mm256_sub_pd(xyzn, epsy);
    const __m128i tmp_xyzn = _mm256_cvttpd_epi32(_mm256_blendv_pd(xyzn, xyzn_e, xyzn));

    const __m128i noise_min_xyzn = _mm_setr_epi32(NOISE_MINX, NOISE_MINY, NOISE_MINZ, 0);

    const __m256d xyz_ixyzn = _mm256_sub_pd(xyzn, _mm256_cvtepi32_pd(tmp_xyzn));
    const __m256d xyz_jxyzn = _mm256_sub_pd(xyz_ixyzn, ONE_PD);

    const __m128i i_xyzn = _mm_and_si128(_mm_sub_epi32(tmp_xyzn, noise_min_xyzn),
        _mm_set1_epi32(0xfff));

    const __m256d s_xyzn = _mm256_mul_pd(xyz_ixyzn,
        _mm256_mul_pd(xyz_ixyzn,
            _mm256_sub_pd(_mm256_set1_pd(3.0),
                _mm256_add_pd(xyz_ixyzn, xyz_ixyzn))));

    const __m256d t_xyzn = _mm256_sub_pd(ONE_PD, s_xyzn);

    const __m256d txtysxsy = _mm256_permute2f128_pd(t_xyzn, s_xyzn, 0x20);
    const __m256d txsxtxsx = PERMUTE4x64(txtysxsy, _MM_SHUFFLE(2, 0, 2, 0));
    const __m256d tytysysy = PERMUTE4x64(txtysxsy, _MM_SHUFFLE(3, 3, 1, 1));

    const __m256d txtysxtytxsysxsy = _mm256_mul_pd(txsxtxsx, tytysysy);

    const __m256d incrsump_s1 = _mm256_mul_pd(txtysxtytxsysxsy, PERMUTE4x64(t_xyzn, _MM_SHUFFLE(2, 2, 2, 2)));
    const __m256d incrsump_s2 = _mm256_mul_pd(txtysxtytxsysxsy, PERMUTE4x64(s_xyzn, _MM_SHUFFLE(2, 2, 2, 2)));

    int ints[4];
    _mm_storeu_si128((__m128i*)(ints), i_xyzn);

    const int ixiy_hash = Hash2d(ints[0], ints[1]);
    const int jxiy_hash = Hash2d(ints[0] + 1, ints[1]);
    const int ixjy_hash = Hash2d(ints[0], ints[1] + 1);
    const int jxjy_hash = Hash2d(ints[0] + 1, ints[1] + 1);

    const int iz = ints[2];

    const __m256d iii = _mm256_blend_pd(PERMUTE4x64(xyz_ixyzn, _MM_SHUFFLE(2, 1, 0, 0)), _mm256_set_pd(0, 0, 0, 0.5), 0x1);
    const __m256d jjj = _mm256_blend_pd(PERMUTE4x64(xyz_jxyzn, _MM_SHUFFLE(2, 1, 0, 0)), _mm256_set_pd(0, 0, 0, 0.5), 0x1);

    __m256d sumr = _mm256_setzero_pd();
    __m256d sumr1 = _mm256_setzero_pd();


    mp = &AVX2RTable[Hash1dRTableIndexAVX(ixiy_hash, iz)];
    INCSUMAVX_NOBLEND(sumr, mp, PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(0, 0, 0, 0)), iii);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(jxiy_hash, iz)];
    INCSUMAVX(sumr1, mp, PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(1, 1, 1, 1)), iii, jjj, 2);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(ixjy_hash, iz)];
    INCSUMAVX(sumr, mp, PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(2, 2, 2, 2)), iii, jjj, 4);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(jxjy_hash, iz)];
    INCSUMAVX(sumr1, mp, PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(3, 3, 3, 3)), iii, jjj, 6);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(ixiy_hash, iz + 1)];
    INCSUMAVX(sumr, mp, PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(0, 0, 0, 0)), iii, jjj, 8);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(jxiy_hash, iz + 1)];
    INCSUMAVX(sumr1, mp, PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(1, 1, 1, 1)), iii, jjj, 10);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(ixjy_hash, iz + 1)];
    INCSUMAVX(sumr, mp, PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(2, 2, 2, 2)), iii, jjj, 12);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(jxjy_hash, iz + 1)];
    INCSUMAVX_NOBLEND(sumr1, mp, PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(3, 3, 3, 3)), jjj);

    {
        sumr = _mm256_add_pd(sumr, sumr1);

        __m128d sumr_up = _mm256_extractf128_pd(sumr,1);
        sumr_up = _mm_add_pd(_mm256_castpd256_pd128(sumr),sumr_up);
        sumr_up = _mm_hadd_pd(sumr_up,sumr_up);
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
*   AVX2FMA3DNoise
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
*   Vector-valued version of "Noise"
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

void AVX2FMA3DNoise(Vector3d& result, const Vector3d& EPoint)
{

#if CHECK_FUNCTIONAL
    Vector3d param(EPoint);
#endif

    AVX2TABLETYPE *mp;

    // TODO FIXME - global statistics reference
    // Stats[Calls_To_DNoise]++;

    const __m256d ONE_PD = _mm256_set1_pd(1.0);
    const __m128i short_si128 = _mm_set1_epi32(0xffff);

    const __m256d xyzn = _mm256_setr_pd(EPoint[X], EPoint[Y], EPoint[Z], 0);
    const __m256d epsy = _mm256_set1_pd(1.0 - EPSILON);
    const __m256d xyzn_e = _mm256_sub_pd(xyzn, epsy);
    const __m128i tmp_xyzn = _mm256_cvttpd_epi32(_mm256_blendv_pd(xyzn, xyzn_e, xyzn));

    const __m128i noise_min_xyzn = _mm_setr_epi32(NOISE_MINX, NOISE_MINY, NOISE_MINZ, 0);

    const __m256d xyz_ixyzn = _mm256_sub_pd(xyzn, _mm256_cvtepi32_pd(tmp_xyzn));
    const __m256d xyz_jxyzn = _mm256_sub_pd(xyz_ixyzn, ONE_PD);

    const __m128i i_xyzn = _mm_and_si128(_mm_sub_epi32(tmp_xyzn, noise_min_xyzn),
        _mm_set1_epi32(0xfff));

    const __m256d s_xyzn = _mm256_mul_pd(xyz_ixyzn,
        _mm256_mul_pd(xyz_ixyzn,
            _mm256_sub_pd(_mm256_set1_pd(3.0),
                _mm256_add_pd(xyz_ixyzn, xyz_ixyzn))));

    const __m256d t_xyzn = _mm256_sub_pd(ONE_PD, s_xyzn);

    const __m256d txtysxsy = _mm256_permute2f128_pd(t_xyzn, s_xyzn, 0x20);
    const __m256d txsxtxsx = PERMUTE4x64(txtysxsy, _MM_SHUFFLE(2, 0, 2, 0));
    const __m256d tytysysy = PERMUTE4x64(txtysxsy, _MM_SHUFFLE(3, 3, 1, 1));

    const __m256d txtysxtytxsysxsy = _mm256_mul_pd(txsxtxsx, tytysysy);

    const __m256d incrsump_s1 = _mm256_mul_pd(txtysxtytxsysxsy, PERMUTE4x64(t_xyzn, _MM_SHUFFLE(2, 2, 2, 2)));
    const __m256d incrsump_s2 = _mm256_mul_pd(txtysxtytxsysxsy, PERMUTE4x64(s_xyzn, _MM_SHUFFLE(2, 2, 2, 2)));

    int ints[4];
    _mm_storeu_si128((__m128i*)(ints), i_xyzn);

    const int ixiy_hash = Hash2d(ints[0], ints[1]);
    const int jxiy_hash = Hash2d(ints[0] + 1, ints[1]);
    const int ixjy_hash = Hash2d(ints[0], ints[1] + 1);
    const int jxjy_hash = Hash2d(ints[0] + 1, ints[1] + 1);

    const int iz = ints[2];

    const __m256d iii = _mm256_blend_pd(PERMUTE4x64(xyz_ixyzn, _MM_SHUFFLE(2, 1, 0, 0)), _mm256_set_pd(0, 0, 0, 0.5), 0x1);
    const __m256d jjj = _mm256_blend_pd(PERMUTE4x64(xyz_jxyzn, _MM_SHUFFLE(2, 1, 0, 0)), _mm256_set_pd(0, 0, 0, 0.5), 0x1);

    __m256d ss;
    __m256d blend;

    __m256d x = _mm256_setzero_pd(), y = _mm256_setzero_pd(), z = _mm256_setzero_pd();


    mp = &AVX2RTable[Hash1dRTableIndexAVX(ixiy_hash, iz)];
    ss = PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(0, 0, 0, 0));
    //     blend = _mm256_blend_pd(iii, jjj, 0);

    INCSUMAVX_VECTOR(mp, ss, iii);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(jxiy_hash, iz)];
    ss = PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(1, 1, 1, 1));
    blend = _mm256_blend_pd(iii, jjj, 2);

    INCSUMAVX_VECTOR(mp, ss, blend);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(jxjy_hash, iz)];
    ss = PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(3, 3, 3, 3));
    blend = _mm256_blend_pd(iii, jjj, 6);

    INCSUMAVX_VECTOR(mp, ss, blend);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(ixjy_hash, iz)];
    ss = PERMUTE4x64(incrsump_s1, _MM_SHUFFLE(2, 2, 2, 2));
    blend = _mm256_blend_pd(iii, jjj, 4);

    INCSUMAVX_VECTOR(mp, ss, blend);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(ixjy_hash, iz + 1)];
    ss = PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(2, 2, 2, 2));
    blend = _mm256_blend_pd(iii, jjj, 12);

    INCSUMAVX_VECTOR(mp, ss, blend);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(jxjy_hash, iz + 1)];
    ss = PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(3, 3, 3, 3));
    //     blend = _mm256_blend_pd(iii, jjj, 14);

    INCSUMAVX_VECTOR(mp, ss, jjj);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(jxiy_hash, iz + 1)];
    ss = PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(1, 1, 1, 1));
    blend = _mm256_blend_pd(iii, jjj, 10);

    INCSUMAVX_VECTOR(mp, ss, blend);

    mp = &AVX2RTable[Hash1dRTableIndexAVX(ixiy_hash, iz + 1)];
    ss = PERMUTE4x64(incrsump_s2, _MM_SHUFFLE(0, 0, 0, 0));
    blend = _mm256_blend_pd(iii, jjj, 8);

    INCSUMAVX_VECTOR(mp, ss, blend);


    __m256d xy = _mm256_hadd_pd(x,y);
    __m128d xy_up = _mm256_extractf128_pd(xy,1);
    xy_up = _mm_add_pd(_mm256_castpd256_pd128(xy),xy_up);
    _mm_storeu_pd(&result[X],xy_up);

    __m128d z_up = _mm256_extractf128_pd(z,1);
    z_up = _mm_add_pd(_mm256_castpd256_pd128(z),z_up);
    z_up = _mm_hadd_pd(z_up,z_up);
    result[Z] = _mm_cvtsd_f64(z_up);


#if CHECK_FUNCTIONAL
    {
        Vector3d portable_res;
        PortableDNoise(portable_res , param);
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

#else // DISABLE_OPTIMIZED_NOISE_AVX2FMA3

const bool kAVX2FMA3NoiseEnabled = false;
void AVX2FMA3NoiseInit() { POV_ASSERT(false); }
DBL AVX2FMA3Noise(const Vector3d& EPoint, int noise_generator) { POV_ASSERT(false); return 0.0; }
void AVX2FMA3DNoise(Vector3d& result, const Vector3d& EPoint) { POV_ASSERT(false); }

#endif // DISABLE_OPTIMIZED_NOISE_AVX2FMA3

}
// end of namespace pov

#endif // TRY_OPTIMIZED_NOISE_AVX2FMA3

