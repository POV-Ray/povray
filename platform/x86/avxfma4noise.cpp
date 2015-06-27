//******************************************************************************
///
/// @file platform/x86/avxfma4noise.cpp
///
/// This file contains implementations of the noise generator optimized for the
/// AVX and FMA4 instruction set.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#include "syspovconfigbase.h"
#include "avxfma4noise.h"

#include "core/material/pattern.h"
#include "core/material/texture.h"
#include "cpuid.h"

/*****************************************************************************/

#ifdef TRY_OPTIMIZED_NOISE

/********************************************************************************************/
/* AMD Specific optimizations: Its found that more than 50% of the time is spent in         */
/* Noise and DNoise. These functions have been optimized using AVX and FMA4 instructions    */
/********************************************************************************************/

namespace pov
{

extern DBL RTable[];

bool AVXFMA4NoiseSupported()
{
    return HaveAVXFMA4();
}

/*****************************************************************************
*
* FUNCTION
*
*   AVX_FMA4_Noise
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
*     Changes Made by AMD
*    July 28, 2011: Re-wrote Noise function using AVX and FMA4 intrinsic
*
******************************************************************************/

DBL AVXFMA4Noise(const Vector3d& EPoint, int noise_generator)
{
    DBL x, y, z;
    DBL *mp;
    int tmp;
    int ix, iy, iz;
    int ixiy_hash, ixjy_hash, jxiy_hash, jxjy_hash;
    DBL x_ix,y_iy, z_iz;

    DBL sum = 0.0;

    // TODO FIXME - global statistics reference
    // Stats[Calls_To_Noise]++;

    if (noise_generator==kNoiseGen_Perlin)
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

    x = EPoint[X];
    y = EPoint[Y];
    z = EPoint[Z];

    /* its equivalent integer lattice point. */
    /* ix = (int)x; iy = (int)y; iz = (long)z; */
    /* JB fix for the range problem */
    tmp = (x>=0)?(int)x:(int)(x-(1-EPSILON));
    ix = (int)((tmp-NOISE_MINX)&0xFFF);
    x_ix = x-tmp;

    tmp = (y>=0)?(int)y:(int)(y-(1-EPSILON));
    iy = (int)((tmp-NOISE_MINY)&0xFFF);
    y_iy = y-tmp;

    tmp = (z>=0)?(int)z:(int)(z-(1-EPSILON));
    iz = (int)((tmp-NOISE_MINZ)&0xFFF);
    z_iz = z-tmp;


    ixiy_hash = Hash2d(ix,     iy);
    jxiy_hash = Hash2d(ix + 1, iy);
    ixjy_hash = Hash2d(ix,     iy + 1);
    jxjy_hash = Hash2d(ix + 1, iy + 1);

    __m128d three = {3.0,3.0};
    __m128d two = {2.0,2.0};
    __m128d one = {1.0,1.0};

    __m128d ix_mm = _mm_set1_pd(x_ix);
    __m128d iy_mm = _mm_set1_pd(y_iy);
    __m128d iz_mm = _mm_set1_pd(z_iz);

    __m128d jx_mm = _mm_sub_pd(ix_mm,one);
    __m128d jy_mm = _mm_sub_pd(iy_mm,one);
    __m128d jz_mm = _mm_sub_pd(iz_mm,one);


    __m128d mm_sx = _mm_mul_pd(_mm_mul_pd(ix_mm,ix_mm), _mm_nmacc_pd(two,ix_mm,three));
    __m128d mm_sy =  _mm_mul_pd(_mm_mul_sd(iy_mm,iy_mm),_mm_nmacc_sd(two,iy_mm,three));
    __m128d mm_sz =  _mm_mul_pd(_mm_mul_pd(iz_mm,iz_mm),_mm_nmacc_pd(two,iz_mm,three));

    mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz)];
    DBL *mp2 = &RTable[Hash1dRTableIndex(ixjy_hash, iz)];

    __m128d mm_tx = _mm_sub_pd(one,mm_sx);
    __m128d mm_ty = _mm_sub_sd(one,mm_sy);
    __m128d mm_tz = _mm_sub_pd(one,mm_sz);
    __m128d temp_mm = _mm_unpacklo_pd(mm_ty,mm_sy);
    __m128d mm_txty_txsy = _mm_mul_pd(mm_tx,temp_mm);
    __m128d mm_sxty_sxsy = _mm_mul_pd(mm_sx,temp_mm);

    __m128d mp_t1 = _mm_loadu_pd(mp + 1);
    __m128d mp_t2 = _mm_loadu_pd(mp2 + 1);


    __m128d mp4_mm = _mm_unpacklo_pd(_mm_load_sd(mp + 4),_mm_load_sd(mp2 + 4));   // 44
    __m128d mp6_mm = _mm_unpacklo_pd(_mm_load_sd(mp + 6),_mm_load_sd(mp2 + 6));   // 66

    __m128d mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
    __m128d mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

    mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz + 1)];
    mp2 = &RTable[Hash1dRTableIndex(ixjy_hash, iz + 1)];

    __m128d s_mm = _mm_mul_pd(mm_txty_txsy,mm_tz);
    __m128d y_mm = _mm_unpacklo_pd(iy_mm,jy_mm);

    __m128d int_sumf = _mm_macc_pd(ix_mm,mp2_mm,mp1_mm);
    int_sumf = _mm_macc_pd(y_mm,mp4_mm,int_sumf);
    int_sumf = _mm_macc_pd(iz_mm,mp6_mm,int_sumf);
    int_sumf = _mm_mul_pd(s_mm,int_sumf);
//-2---------------------------------------------------------------

     mp_t1 = _mm_loadu_pd(mp + 1);
     mp_t2 = _mm_loadu_pd(mp2 + 1);

     mp4_mm = _mm_unpacklo_pd(_mm_load_sd(mp + 4),_mm_load_sd(mp2 + 4));   // 44
     mp6_mm = _mm_unpacklo_pd(_mm_load_sd(mp + 6),_mm_load_sd(mp2 + 6));   // 66

     mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
     mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

    mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz)];
    mp2 = &RTable[Hash1dRTableIndex(jxjy_hash, iz)];

    s_mm = _mm_mul_pd(mm_txty_txsy,mm_sz);

    __m128d int_sum1 = _mm_macc_pd(ix_mm,mp2_mm,mp1_mm);
    int_sum1 = _mm_macc_pd(y_mm,mp4_mm,int_sum1);
    int_sum1 = _mm_macc_pd(jz_mm,mp6_mm,int_sum1);
    int_sum1 = _mm_macc_pd(s_mm,int_sum1,int_sumf);
//-3---------------------------------------------------------------

     mp_t1 = _mm_loadu_pd(mp + 1);
     mp_t2 = _mm_loadu_pd(mp2 + 1);

     mp4_mm = _mm_unpacklo_pd(_mm_load_sd(mp + 4),_mm_load_sd(mp2 + 4));   // 44
     mp6_mm = _mm_unpacklo_pd(_mm_load_sd(mp + 6),_mm_load_sd(mp2 + 6));   // 66

     mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
     mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

    mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz + 1)];
    mp2 = &RTable[Hash1dRTableIndex(jxjy_hash, iz + 1)];

    s_mm = _mm_mul_pd(mm_sxty_sxsy,mm_tz);

    int_sumf = _mm_macc_pd(jx_mm,mp2_mm,mp1_mm);
    int_sumf = _mm_macc_pd(y_mm,mp4_mm,int_sumf);
    int_sumf = _mm_macc_pd(iz_mm,mp6_mm,int_sumf);
    int_sumf = _mm_macc_pd(s_mm,int_sumf,int_sum1);
//-4---------------------------------------------------------------
     mp_t1 = _mm_loadu_pd(mp + 1);
     mp_t2 = _mm_loadu_pd(mp2 + 1);

     mp4_mm = _mm_unpacklo_pd(_mm_load_sd(mp + 4),_mm_load_sd(mp2 + 4));   // 44
     mp6_mm = _mm_unpacklo_pd(_mm_load_sd(mp + 6),_mm_load_sd(mp2 + 6));   // 66

     mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
     mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22


    s_mm = _mm_mul_pd(mm_sxty_sxsy,mm_sz);

    int_sum1 = _mm_macc_pd(jx_mm,mp2_mm,mp1_mm);
    int_sum1 = _mm_macc_pd(y_mm,mp4_mm,int_sum1);
    int_sum1 = _mm_macc_pd(jz_mm,mp6_mm,int_sum1);
    int_sum1 = _mm_macc_pd(s_mm,int_sum1,int_sumf);
    int_sum1 = _mm_add_sd(_mm_unpackhi_pd(int_sum1,int_sum1),int_sum1);
    _mm_store_sd(&sum,int_sum1);

    if(noise_generator==kNoiseGen_RangeCorrected)
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
    return (sum);
}


/*****************************************************************************
*
* FUNCTION
*
*   AVX_FMA4_DNoise
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
*    Changes Made by AMD
*    July 28, 2011:Re-wrote DNoise function using AVX and FMA4 intrinsic
*
******************************************************************************/

/// Optimized DNoise function using AVX and FMA4 instructions.
/// @author Optimized by AMD
void AVXFMA4DNoise(Vector3d& result, const Vector3d& EPoint)
{
    DBL x, y, z;
    int ix, iy, iz;
    DBL *mp;
    int tmp;
    int ixiy_hash, ixjy_hash, jxiy_hash, jxjy_hash;
    DBL x_ix,y_iy,z_iz;

    // TODO FIXME - global statistics reference
    // Stats[Calls_To_DNoise]++;

    x = EPoint[0];
    y = EPoint[1];
    z = EPoint[2];

    /* its equivalent integer lattice point. */
    /*ix = (int)x; iy = (int)y; iz = (int)z;
    x_ix = x - ix; y_iy = y - iy; z_iz = z - iz;*/
                /* JB fix for the range problem */
    tmp = (x>=0)?(int)x:(int)(x-(1-EPSILON));
    ix = (int)((tmp-NOISE_MINX)&0xFFF);
    x_ix = x-tmp;

    tmp = (y>=0)?(int)y:(int)(y-(1-EPSILON));
    iy = (int)((tmp-NOISE_MINY)&0xFFF);
    y_iy = y-tmp;

    tmp = (z>=0)?(int)z:(int)(z-(1-EPSILON));
    iz = (int)((tmp-NOISE_MINZ)&0xFFF);
    z_iz = z-tmp;

    ixiy_hash = Hash2d(ix,     iy);
    jxiy_hash = Hash2d(ix + 1, iy);
    ixjy_hash = Hash2d(ix,     iy + 1);
    jxjy_hash = Hash2d(ix + 1, iy + 1);

    __m128d three = {3.0,3.0};
    __m128d two = {2.0,2.0};
    __m128d one = {1.0,1.0};

    __m128d ix_mm = _mm_set1_pd(x_ix);
    __m128d iy_mm = _mm_set1_pd(y_iy);
    __m128d iz_mm = _mm_set1_pd(z_iz);

    __m128d jx_mm = _mm_sub_pd(ix_mm,one);
    __m128d jy_mm = _mm_sub_pd(iy_mm,one);
    __m128d jz_mm = _mm_sub_pd(iz_mm,one);

    __m128d mm_sx = _mm_mul_pd(_mm_mul_pd(ix_mm,ix_mm), _mm_nmacc_pd(two,ix_mm,three));
    __m128d mm_sy =  _mm_mul_pd(_mm_mul_pd(iy_mm,iy_mm),_mm_nmacc_pd(two,iy_mm,three));
    __m128d mm_sz =  _mm_mul_pd(_mm_mul_pd(iz_mm,iz_mm),_mm_nmacc_pd(two,iz_mm,three));

    __m128d mm_tx = _mm_sub_pd(one,mm_sx);
    __m128d mm_ty = _mm_sub_pd(one,mm_sy);
    __m128d mm_tz = _mm_sub_pd(one,mm_sz);

    __m128d mm_txty = _mm_mul_pd(mm_tx,mm_ty);
    __m128d mm_s = _mm_mul_pd(mm_txty,mm_tz);

    mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz)];
    DBL *mp2 = mp+8;

    __m128d mp_t1 = _mm_loadu_pd(mp + 1);
    __m128d mp_t2 = _mm_loadu_pd(mp2 + 1);
    __m128d mp_t3 =  _mm_load_sd(mp + 4);
    __m128d mp_t4 =  _mm_load_sd(mp2 + 4);
    __m128d mp_t5 =  _mm_load_sd(mp + 6);
    __m128d mp_t6 =  _mm_load_sd(mp2 + 6);

    __m128d mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
    __m128d mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

    __m128d mp4_mm = _mm_unpacklo_pd(mp_t3,mp_t4);   // 44
    __m128d mp6_mm = _mm_unpacklo_pd(mp_t5,mp_t6);   // 66

    __m128d sum_X_Y = _mm_macc_pd(ix_mm,mp2_mm,mp1_mm);
    sum_X_Y = _mm_macc_pd(iy_mm,mp4_mm,sum_X_Y);
    sum_X_Y = _mm_macc_pd(iz_mm,mp6_mm,sum_X_Y);
    sum_X_Y = _mm_mul_pd(mm_s,sum_X_Y);

    mp2 = mp2+8;
    mp1_mm = _mm_load_sd(mp2 + 1);
    mp2_mm = _mm_load_sd(mp2 + 2);
    mp4_mm = _mm_load_sd(mp2 + 4);
    mp6_mm = _mm_load_sd(mp2 + 6);


    __m128d sum__Z = _mm_macc_sd(ix_mm,mp2_mm,mp1_mm);
    sum__Z = _mm_macc_sd(iy_mm,mp4_mm,sum__Z);
    sum__Z = _mm_macc_sd(iz_mm,mp6_mm,sum__Z);
    sum__Z = _mm_mul_sd(mm_s,sum__Z);

//2--------------------------------------------------

    mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz)];
    mp2 = mp+8;

     mp_t1 = _mm_loadu_pd(mp + 1);
     mp_t2 = _mm_loadu_pd(mp2 + 1);
     mp_t3 =  _mm_load_sd(mp + 4);
     mp_t4 =  _mm_load_sd(mp2 + 4);
     mp_t5 =  _mm_load_sd(mp + 6);
     mp_t6 =  _mm_load_sd(mp2 + 6);

    __m128d mm_sxty = _mm_mul_pd(mm_sx,mm_ty);
    mm_s = _mm_mul_pd(mm_sxty,mm_tz);


     mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
     mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

     mp4_mm = _mm_unpacklo_pd(mp_t3,mp_t4);   // 44
     mp6_mm = _mm_unpacklo_pd(mp_t5,mp_t6);   // 66

    mp2 = mp2+8;

    __m128d t_sum_X_Y = _mm_macc_pd(jx_mm,mp2_mm,mp1_mm);
    t_sum_X_Y = _mm_macc_pd(iy_mm,mp4_mm,t_sum_X_Y);
    t_sum_X_Y = _mm_macc_pd(iz_mm,mp6_mm,t_sum_X_Y);
    sum_X_Y = _mm_macc_pd(mm_s,t_sum_X_Y,sum_X_Y);


    mp1_mm = _mm_load_sd(mp2 + 1);
    mp2_mm = _mm_load_sd(mp2 + 2);
    mp4_mm = _mm_load_sd(mp2 + 4);
    mp6_mm = _mm_load_sd(mp2 + 6);

    __m128d t_sum__Z = _mm_macc_sd(jx_mm,mp2_mm,mp1_mm);
    t_sum__Z = _mm_macc_sd(iy_mm,mp4_mm,t_sum__Z);
    t_sum__Z = _mm_macc_sd(iz_mm,mp6_mm,t_sum__Z);
    sum__Z = _mm_macc_sd(mm_s,t_sum__Z,sum__Z);

//3---------------------------------------------------------------



    mp = &RTable[Hash1dRTableIndex(jxjy_hash, iz)];
    mp2 = mp+8;

     mp_t1 = _mm_loadu_pd(mp + 1);
     mp_t2 = _mm_loadu_pd(mp2 + 1);
     mp_t3 =  _mm_load_sd(mp + 4);
     mp_t4 =  _mm_load_sd(mp2 + 4);
     mp_t5 =  _mm_load_sd(mp + 6);
     mp_t6 =  _mm_load_sd(mp2 + 6);

    __m128d mm_sxsy = _mm_mul_pd(mm_sx,mm_sy);
    mm_s = _mm_mul_pd(mm_sxsy,mm_tz);

     mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
     mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

     mp4_mm = _mm_unpacklo_pd(mp_t3,mp_t4);   // 44
     mp6_mm = _mm_unpacklo_pd(mp_t5,mp_t6);   // 66

    mp2 = mp2+8;

    t_sum_X_Y = _mm_macc_pd(jx_mm,mp2_mm,mp1_mm);
    t_sum_X_Y = _mm_macc_pd(jy_mm,mp4_mm,t_sum_X_Y);
    t_sum_X_Y = _mm_macc_pd(iz_mm,mp6_mm,t_sum_X_Y);
    sum_X_Y = _mm_macc_pd(mm_s,t_sum_X_Y,sum_X_Y);


    mp1_mm = _mm_load_sd(mp2 + 1);
    mp2_mm = _mm_load_sd(mp2 + 2);
    mp4_mm = _mm_load_sd(mp2 + 4);
    mp6_mm = _mm_load_sd(mp2 + 6);

    t_sum__Z = _mm_macc_sd(jx_mm,mp2_mm,mp1_mm);
    t_sum__Z = _mm_macc_sd(jy_mm,mp4_mm,t_sum__Z);
    t_sum__Z = _mm_macc_sd(iz_mm,mp6_mm,t_sum__Z);
    sum__Z = _mm_macc_sd(mm_s,t_sum__Z,sum__Z);

//4-------------------------------------------------------------------
    mp = &RTable[Hash1dRTableIndex(ixjy_hash, iz)];
    mp2 = mp+8;

     mp_t1 = _mm_loadu_pd(mp + 1);
     mp_t2 = _mm_loadu_pd(mp2 + 1);
     mp_t3 =  _mm_load_sd(mp + 4);
     mp_t4 =  _mm_load_sd(mp2 + 4);
     mp_t5 =  _mm_load_sd(mp + 6);
     mp_t6 =  _mm_load_sd(mp2 + 6);

    __m128d mm_txsy = _mm_mul_pd(mm_tx,mm_sy);
    mm_s = _mm_mul_pd(mm_txsy,mm_tz);

     mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
     mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

     mp4_mm = _mm_unpacklo_pd(mp_t3,mp_t4);   // 44
     mp6_mm = _mm_unpacklo_pd(mp_t5,mp_t6);   // 66
    mp2 = mp2+8;

    t_sum_X_Y = _mm_macc_pd(ix_mm,mp2_mm,mp1_mm);
    t_sum_X_Y = _mm_macc_pd(jy_mm,mp4_mm,t_sum_X_Y);
    t_sum_X_Y = _mm_macc_pd(iz_mm,mp6_mm,t_sum_X_Y);
    sum_X_Y = _mm_macc_pd(mm_s,t_sum_X_Y,sum_X_Y);


    mp1_mm = _mm_load_sd(mp2 + 1);
    mp2_mm = _mm_load_sd(mp2 + 2);
    mp4_mm = _mm_load_sd(mp2 + 4);
    mp6_mm = _mm_load_sd(mp2 + 6);

    t_sum__Z = _mm_macc_sd(ix_mm,mp2_mm,mp1_mm);
    t_sum__Z = _mm_macc_sd(jy_mm,mp4_mm,t_sum__Z);
    t_sum__Z = _mm_macc_sd(iz_mm,mp6_mm,t_sum__Z);
    sum__Z = _mm_macc_sd(mm_s,t_sum__Z,sum__Z);
//5-----------------------------------------------------
    mp = &RTable[Hash1dRTableIndex(ixjy_hash, iz + 1)];
    mp2 = mp+8;

     mp_t1 = _mm_loadu_pd(mp + 1);
     mp_t2 = _mm_loadu_pd(mp2 + 1);
     mp_t3 =  _mm_load_sd(mp + 4);
     mp_t4 =  _mm_load_sd(mp2 + 4);
     mp_t5 =  _mm_load_sd(mp + 6);
     mp_t6 =  _mm_load_sd(mp2 + 6);

    mm_s = _mm_mul_pd(mm_txsy,mm_sz);

     mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
     mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

     mp4_mm = _mm_unpacklo_pd(mp_t3,mp_t4);   // 44
     mp6_mm = _mm_unpacklo_pd(mp_t5,mp_t6);   // 66
    mp2 = mp2+8;

    t_sum_X_Y = _mm_macc_pd(ix_mm,mp2_mm,mp1_mm);
    t_sum_X_Y = _mm_macc_pd(jy_mm,mp4_mm,t_sum_X_Y);
    t_sum_X_Y = _mm_macc_pd(jz_mm,mp6_mm,t_sum_X_Y);
    sum_X_Y = _mm_macc_pd(mm_s,t_sum_X_Y,sum_X_Y);


    mp1_mm = _mm_load_sd(mp2 + 1);
    mp2_mm = _mm_load_sd(mp2 + 2);
    mp4_mm = _mm_load_sd(mp2 + 4);
    mp6_mm = _mm_load_sd(mp2 + 6);

    t_sum__Z = _mm_macc_sd(ix_mm,mp2_mm,mp1_mm);
    t_sum__Z = _mm_macc_sd(jy_mm,mp4_mm,t_sum__Z);
    t_sum__Z = _mm_macc_sd(jz_mm,mp6_mm,t_sum__Z);
    sum__Z = _mm_macc_sd(mm_s,t_sum__Z,sum__Z);

//6------------------
    mp = &RTable[Hash1dRTableIndex(jxjy_hash, iz + 1)];
    mp2 = mp+8;

     mp_t1 = _mm_loadu_pd(mp + 1);
     mp_t2 = _mm_loadu_pd(mp2 + 1);
     mp_t3 =  _mm_load_sd(mp + 4);
     mp_t4 =  _mm_load_sd(mp2 + 4);
     mp_t5 =  _mm_load_sd(mp + 6);
     mp_t6 =  _mm_load_sd(mp2 + 6);

    mm_s = _mm_mul_pd(mm_sxsy,mm_sz);

     mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
     mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

     mp4_mm = _mm_unpacklo_pd(mp_t3,mp_t4);   // 44
     mp6_mm = _mm_unpacklo_pd(mp_t5,mp_t6);   // 66

     mp2 = mp2+8;

    t_sum_X_Y = _mm_macc_pd(jx_mm,mp2_mm,mp1_mm);
    t_sum_X_Y = _mm_macc_pd(jy_mm,mp4_mm,t_sum_X_Y);
    t_sum_X_Y = _mm_macc_pd(jz_mm,mp6_mm,t_sum_X_Y);
    sum_X_Y = _mm_macc_pd(mm_s,t_sum_X_Y,sum_X_Y);


    mp1_mm = _mm_load_sd(mp2 + 1);
    mp2_mm = _mm_load_sd(mp2 + 2);
    mp4_mm = _mm_load_sd(mp2 + 4);
    mp6_mm = _mm_load_sd(mp2 + 6);

    t_sum__Z = _mm_macc_sd(jx_mm,mp2_mm,mp1_mm);
    t_sum__Z = _mm_macc_sd(jy_mm,mp4_mm,t_sum__Z);
    t_sum__Z = _mm_macc_sd(jz_mm,mp6_mm,t_sum__Z);
    sum__Z = _mm_macc_sd(mm_s,t_sum__Z,sum__Z);

//7-----------
    mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz + 1)];
    mp2 = mp+8;

     mp_t1 = _mm_loadu_pd(mp + 1);
     mp_t2 = _mm_loadu_pd(mp2 + 1);
     mp_t3 =  _mm_load_sd(mp + 4);
     mp_t4 =  _mm_load_sd(mp2 + 4);
     mp_t5 =  _mm_load_sd(mp + 6);
     mp_t6 =  _mm_load_sd(mp2 + 6);

    mm_s = _mm_mul_pd(mm_sxty,mm_sz);

     mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
     mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

     mp4_mm = _mm_unpacklo_pd(mp_t3,mp_t4);   // 44
     mp6_mm = _mm_unpacklo_pd(mp_t5,mp_t6);   // 66
     mp2 = mp2+8;
    t_sum_X_Y = _mm_macc_pd(jx_mm,mp2_mm,mp1_mm);
    t_sum_X_Y = _mm_macc_pd(iy_mm,mp4_mm,t_sum_X_Y);
    t_sum_X_Y = _mm_macc_pd(jz_mm,mp6_mm,t_sum_X_Y);
    sum_X_Y = _mm_macc_pd(mm_s,t_sum_X_Y,sum_X_Y);


    mp1_mm = _mm_load_sd(mp2 + 1);
    mp2_mm = _mm_load_sd(mp2 + 2);
    mp4_mm = _mm_load_sd(mp2 + 4);
    mp6_mm = _mm_load_sd(mp2 + 6);

    t_sum__Z = _mm_macc_sd(jx_mm,mp2_mm,mp1_mm);
    t_sum__Z = _mm_macc_sd(iy_mm,mp4_mm,t_sum__Z);
    t_sum__Z = _mm_macc_sd(jz_mm,mp6_mm,t_sum__Z);
    sum__Z = _mm_macc_sd(mm_s,t_sum__Z,sum__Z);

//8--------------


    mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz + 1)];
    mp2 = mp+8;

     mp_t1 = _mm_loadu_pd(mp + 1);
     mp_t2 = _mm_loadu_pd(mp2 + 1);
     mp_t3 =  _mm_load_sd(mp + 4);
     mp_t4 =  _mm_load_sd(mp2 + 4);
     mp_t5 =  _mm_load_sd(mp + 6);
     mp_t6 =  _mm_load_sd(mp2 + 6);

    mm_s = _mm_mul_pd(mm_txty,mm_sz);

     mp1_mm = _mm_unpacklo_pd(mp_t1,mp_t2);   // 11
     mp2_mm= _mm_unpackhi_pd(mp_t1,mp_t2);   // 22

     mp4_mm = _mm_unpacklo_pd(mp_t3,mp_t4);   // 44
     mp6_mm = _mm_unpacklo_pd(mp_t5,mp_t6);   // 66

    mp2 = mp2+8;

    t_sum_X_Y = _mm_macc_pd(ix_mm,mp2_mm,mp1_mm);
    t_sum_X_Y = _mm_macc_pd(iy_mm,mp4_mm,t_sum_X_Y);
    t_sum_X_Y = _mm_macc_pd(jz_mm,mp6_mm,t_sum_X_Y);
    sum_X_Y = _mm_macc_pd(mm_s,t_sum_X_Y,sum_X_Y);


    mp1_mm = _mm_load_sd(mp2 + 1);
    mp2_mm = _mm_load_sd(mp2 + 2);
    mp4_mm = _mm_load_sd(mp2 + 4);
    mp6_mm = _mm_load_sd(mp2 + 6);

    t_sum__Z = _mm_macc_sd(ix_mm,mp2_mm,mp1_mm);
    t_sum__Z = _mm_macc_sd(iy_mm,mp4_mm,t_sum__Z);
    t_sum__Z = _mm_macc_sd(jz_mm,mp6_mm,t_sum__Z);
    sum__Z = _mm_macc_sd(mm_s,t_sum__Z,sum__Z);

    _mm_storeu_pd(*result,sum_X_Y);

    _mm_store_sd(&result[Z],sum__Z);

}

}

#endif

