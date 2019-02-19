//******************************************************************************
///
/// @file core/material/portablenoise.cpp
///
/// Portable implementation of the noise generator.
///
/// This file provides the portable default implementation of the noise
/// generator.
///
/// In addition, this file is designed so that it may be included from other
/// compilation units to provide compiler-optimized alternative implementations,
/// to be selected dynamically at run-time. For an example, see
/// @ref platform/x86/avxportablenoise.cpp.
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

#ifndef PORTABLE_OPTIMIZED_NOISE
// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/material/portablenoise.h"
#endif // PORTABLE_OPTIMIZED_NOISE

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/material/noise.h"

namespace pov
{

extern DBL RTable[]; // defined in core/material/texture.cpp

#define SCURVE(a) ((a)*(a)*(3.0-2.0*(a)))

#define INCRSUM(m,s,x,y,z)  \
    ((s)*(RTable[m+1] + RTable[m+2]*(x) + RTable[m+4]*(y) + RTable[m+6]*(z)))

#define INCRSUMP(mp,s,x,y,z) \
    ((s)*((mp[1]) + (mp[2])*(x) + (mp[4])*(y) + (mp[6])*(z)))

/*****************************************************************************
*
* FUNCTION
*
*   Noise
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
******************************************************************************/

DBL PortableNoise(const Vector3d& EPoint, int noise_generator)
{
    DBL x, y, z;
    DBL *mp;
    int tmp;
    int ix, iy, iz;
    int ixiy_hash, ixjy_hash, jxiy_hash, jxjy_hash;

    DBL sx, sy, sz, tx, ty, tz;
    DBL sum = 0.0;

    DBL x_ix, x_jx, y_iy, y_jy, z_iz, z_jz, txty, sxty, txsy, sxsy;

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

    x_jx = x_ix-1; y_jy = y_iy-1; z_jz = z_iz-1;

    sx = SCURVE(x_ix); sy = SCURVE(y_iy); sz = SCURVE(z_iz);

    /* the complement values of sx,sy,sz */
    tx = 1 - sx; ty = 1 - sy; tz = 1 - sz;

    /*
     *  interpolate!
     */
    txty = tx * ty;
    sxty = sx * ty;
    txsy = tx * sy;
    sxsy = sx * sy;
    ixiy_hash = Hash2d(ix,     iy);
    jxiy_hash = Hash2d(ix + 1, iy);
    ixjy_hash = Hash2d(ix,     iy + 1);
    jxjy_hash = Hash2d(ix + 1, iy + 1);

    mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz)];
    sum = INCRSUMP(mp, (txty*tz), x_ix, y_iy, z_iz);

    mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz)];
    sum += INCRSUMP(mp, (sxty*tz), x_jx, y_iy, z_iz);

    mp = &RTable[Hash1dRTableIndex(ixjy_hash, iz)];
    sum += INCRSUMP(mp, (txsy*tz), x_ix, y_jy, z_iz);

    mp = &RTable[Hash1dRTableIndex(jxjy_hash, iz)];
    sum += INCRSUMP(mp, (sxsy*tz), x_jx, y_jy, z_iz);

    mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz + 1)];
    sum += INCRSUMP(mp, (txty*sz), x_ix, y_iy, z_jz);

    mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz + 1)];
    sum += INCRSUMP(mp, (sxty*sz), x_jx, y_iy, z_jz);

    mp = &RTable[Hash1dRTableIndex(ixjy_hash, iz + 1)];
    sum += INCRSUMP(mp, (txsy*sz), x_ix, y_jy, z_jz);

    mp = &RTable[Hash1dRTableIndex(jxjy_hash, iz + 1)];
    sum += INCRSUMP(mp, (sxsy*sz), x_jx, y_jy, z_jz);

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
*   DNoise
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
******************************************************************************/

void PortableDNoise(Vector3d& result, const Vector3d& EPoint)
{
    DBL x, y, z;
    DBL *mp;
    int tmp;
    int ix, iy, iz;
    int ixiy_hash, ixjy_hash, jxiy_hash, jxjy_hash;
    DBL x_ix, x_jx, y_iy, y_jy, z_iz, z_jz;
    DBL s;
    DBL sx, sy, sz, tx, ty, tz;
    DBL txty, sxty, txsy, sxsy;

    // TODO FIXME - global statistics reference
    // Stats[Calls_To_DNoise]++;

    x = EPoint[X];
    y = EPoint[Y];
    z = EPoint[Z];

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

    x_jx = x_ix-1; y_jy = y_iy-1; z_jz = z_iz-1;

    sx = SCURVE(x_ix); sy = SCURVE(y_iy); sz = SCURVE(z_iz);

    /* the complement values of sx,sy,sz */
    tx = 1 - sx; ty = 1 - sy; tz = 1 - sz;

    /*
     *  interpolate!
     */
    txty = tx * ty;
    sxty = sx * ty;
    txsy = tx * sy;
    sxsy = sx * sy;
    ixiy_hash = Hash2d(ix,     iy);
    jxiy_hash = Hash2d(ix + 1, iy);
    ixjy_hash = Hash2d(ix,     iy + 1);
    jxjy_hash = Hash2d(ix + 1, iy + 1);

    mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz)];
    s = txty*tz;
    result[X] = INCRSUMP(mp, s, x_ix, y_iy, z_iz);
    mp += 8;
    result[Y] = INCRSUMP(mp, s, x_ix, y_iy, z_iz);
    mp += 8;
    result[Z] = INCRSUMP(mp, s, x_ix, y_iy, z_iz);

    mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz)];
    s = sxty*tz;
    result[X] += INCRSUMP(mp, s, x_jx, y_iy, z_iz);
    mp += 8;
    result[Y] += INCRSUMP(mp, s, x_jx, y_iy, z_iz);
    mp += 8;
    result[Z] += INCRSUMP(mp, s, x_jx, y_iy, z_iz);

    mp = &RTable[Hash1dRTableIndex(jxjy_hash, iz)];
    s = sxsy*tz;
    result[X] += INCRSUMP(mp, s, x_jx, y_jy, z_iz);
    mp += 8;
    result[Y] += INCRSUMP(mp, s, x_jx, y_jy, z_iz);
    mp += 8;
    result[Z] += INCRSUMP(mp, s, x_jx, y_jy, z_iz);

    mp = &RTable[Hash1dRTableIndex(ixjy_hash, iz)];
    s = txsy*tz;
    result[X] += INCRSUMP(mp, s, x_ix, y_jy, z_iz);
    mp += 8;
    result[Y] += INCRSUMP(mp, s, x_ix, y_jy, z_iz);
    mp += 8;
    result[Z] += INCRSUMP(mp, s, x_ix, y_jy, z_iz);

    mp = &RTable[Hash1dRTableIndex(ixjy_hash, iz + 1)];
    s = txsy*sz;
    result[X] += INCRSUMP(mp, s, x_ix, y_jy, z_jz);
    mp += 8;
    result[Y] += INCRSUMP(mp, s, x_ix, y_jy, z_jz);
    mp += 8;
    result[Z] += INCRSUMP(mp, s, x_ix, y_jy, z_jz);

    mp = &RTable[Hash1dRTableIndex(jxjy_hash, iz + 1)];
    s = sxsy*sz;
    result[X] += INCRSUMP(mp, s, x_jx, y_jy, z_jz);
    mp += 8;
    result[Y] += INCRSUMP(mp, s, x_jx, y_jy, z_jz);
    mp += 8;
    result[Z] += INCRSUMP(mp, s, x_jx, y_jy, z_jz);

    mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz + 1)];
    s = sxty*sz;
    result[X] += INCRSUMP(mp, s, x_jx, y_iy, z_jz);
    mp += 8;
    result[Y] += INCRSUMP(mp, s, x_jx, y_iy, z_jz);
    mp += 8;
    result[Z] += INCRSUMP(mp, s, x_jx, y_iy, z_jz);

    mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz + 1)];
    s = txty*sz;
    result[X] += INCRSUMP(mp, s, x_ix, y_iy, z_jz);
    mp += 8;
    result[Y] += INCRSUMP(mp, s, x_ix, y_iy, z_jz);
    mp += 8;
    result[Z] += INCRSUMP(mp, s, x_ix, y_iy, z_jz);
}

}
// end of namespace pov
