//******************************************************************************
///
/// @file core/material/texture.cpp
///
/// This module implements texturing functions such as noise, turbulence and
/// texture transformation functions. The actual texture routines are in the
/// files @ref pigment.cpp and @ref normal.cpp.
///
/// The noise function used here is the one described by Ken Perlin in
/// "Hypertexture", SIGGRAPH '89 Conference Proceedings page 253.
///
/// @copyright
/// @parblock
///
/// Some texture ideas garnered from SIGGRAPH '85 Volume 19 Number 3, "An Image
/// Synthesizer" By Ken Perlin. Further Ideas Garnered from "The RenderMan
/// Companion" (Addison Wesley).
///
/// ----------------------------------------------------------------------------
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

#include <algorithm>

// configcore.h must always be the first POV file included in core *.cpp files (pulls in platform config)
#include "core/configcore.h"
#include "core/material/texture.h"

#include "base/pov_err.h"

#include "core/material/pattern.h"
#include "core/material/pigment.h"
#include "core/material/normal.h"
#include "core/material/warp.h"

#include "backend/colour/colour_old.h"
#include "backend/support/imageutil.h"

#if defined(USE_AVX_FMA4_FOR_NOISE)
    #include "backend/texture/avxfma4check.h"
#endif

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

// TODO FIXME GLOBAL VARIABLE
static unsigned int next_rand = 1;

/*****************************************************************************
* Static functions
******************************************************************************/

static void InitTextureTable (void);
static void InitSolidNoise(void);
static DBL SolidNoise(const Vector3d& P);

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Ridiculously large scaling values */

const int MINX = -10000;
const int MINY = MINX;
const int MINZ = MINX;

const int SINTABSIZE = 1000;

#define SCURVE(a) ((a)*(a)*(3.0-2.0*(a)))

// Hash2d assumed values in the range 0..8191
#define Hash2d(a,b)   \
    hashTable[(int)(hashTable[(int)(a)] ^ (b))]

// Hash1dRTableIndex assumed values in the range 0..8191
#define Hash1dRTableIndex(a,b)   \
    ((hashTable[(int)(a) ^ (b)] & 0xFF) * 2)

#define INCRSUM(m,s,x,y,z)  \
    ((s)*(RTable[m+1] + RTable[m+2]*(x) + RTable[m+4]*(y) + RTable[m+6]*(z)))

#define INCRSUMP(mp,s,x,y,z) \
    ((s)*((mp[1]) + (mp[2])*(x) + (mp[4])*(y) + (mp[6])*(z)))


/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

static DBL *sintab; // GLOBAL VARIABLE

#ifdef DYNAMIC_HASHTABLE
unsigned short *hashTable; // GLOBAL VARIABLE
#else
ALIGN16 unsigned short hashTable[8192]; // GLOBAL VARIABLE
#endif

/*****************************************************************************
* Local variables
******************************************************************************/

ALIGN16 DBL RTable[267*2] =
{
            -1, 0.0,    0.604974, 0.0,   -0.937102, 0.0,    0.414115, 0.0,    0.576226, 0.0,  -0.0161593, 0.0,
      0.432334, 0.0,    0.103685, 0.0,    0.590539, 0.0,   0.0286412, 0.0,     0.46981, 0.0,    -0.84622, 0.0,
    -0.0734112, 0.0,   -0.304097, 0.0,    -0.40206, 0.0,   -0.210132, 0.0,   -0.919127, 0.0,    0.652033, 0.0,
      -0.83151, 0.0,   -0.183948, 0.0,   -0.671107, 0.0,    0.852476, 0.0,    0.043595, 0.0,   -0.404532, 0.0,
       0.75494, 0.0,   -0.335653, 0.0,    0.618433, 0.0,    0.605707, 0.0,    0.708583, 0.0,   -0.477195, 0.0,
      0.899474, 0.0,    0.490623, 0.0,    0.221729, 0.0,   -0.400381, 0.0,   -0.853727, 0.0,   -0.932586, 0.0,
      0.659113, 0.0,    0.961303, 0.0,    0.325948, 0.0,   -0.750851, 0.0,    0.842466, 0.0,    0.734401, 0.0,
     -0.649866, 0.0,    0.394491, 0.0,   -0.466056, 0.0,   -0.434073, 0.0,    0.109026, 0.0,   0.0847028, 0.0,
     -0.738857, 0.0,    0.241505, 0.0,     0.16228, 0.0,    -0.71426, 0.0,   -0.883665, 0.0,   -0.150408, 0.0,
      -0.90396, 0.0,   -0.686549, 0.0,   -0.785214, 0.0,    0.488548, 0.0,   0.0246433, 0.0,    0.142473, 0.0,
     -0.602136, 0.0,    0.375845, 0.0, -0.00779736, 0.0,    0.498955, 0.0,   -0.268147, 0.0,    0.856382, 0.0,
     -0.386007, 0.0,   -0.596094, 0.0,   -0.867735, 0.0,   -0.570977, 0.0,   -0.914366, 0.0,     0.28896, 0.0,
      0.672206, 0.0,   -0.233783, 0.0,     0.94815, 0.0,    0.895262, 0.0,    0.343252, 0.0,   -0.173388, 0.0,
     -0.767971, 0.0,   -0.314748, 0.0,    0.824308, 0.0,   -0.342092, 0.0,    0.721431, 0.0,    -0.24004, 0.0,
      -0.63653, 0.0,    0.553277, 0.0,    0.376272, 0.0,    0.158984, 0.0,   -0.452659, 0.0,    0.396323, 0.0,
     -0.420676, 0.0,   -0.454154, 0.0,    0.122179, 0.0,    0.295857, 0.0,   0.0664225, 0.0,   -0.202075, 0.0,
     -0.724788, 0.0,    0.453513, 0.0,    0.224567, 0.0,   -0.908812, 0.0,    0.176349, 0.0,   -0.320516, 0.0,
     -0.697139, 0.0,    0.742702, 0.0,   -0.900786, 0.0,    0.471489, 0.0,   -0.133532, 0.0,    0.119127, 0.0,
     -0.889769, 0.0,    -0.23183, 0.0,   -0.669673, 0.0,   -0.046891, 0.0,   -0.803433, 0.0,   -0.966735, 0.0,
      0.475578, 0.0,   -0.652644, 0.0,   0.0112459, 0.0,   -0.730007, 0.0,    0.128283, 0.0,    0.145647, 0.0,
     -0.619318, 0.0,    0.272023, 0.0,    0.392966, 0.0,    0.646418, 0.0,  -0.0207675, 0.0,   -0.315908, 0.0,
      0.480797, 0.0,    0.535668, 0.0,   -0.250172, 0.0,    -0.83093, 0.0,   -0.653773, 0.0,   -0.443809, 0.0,
      0.119982, 0.0,   -0.897642, 0.0,     0.89453, 0.0,    0.165789, 0.0,    0.633875, 0.0,   -0.886839, 0.0,
      0.930877, 0.0,   -0.537194, 0.0,    0.587732, 0.0,    0.722011, 0.0,   -0.209461, 0.0,  -0.0424659, 0.0,
     -0.814267, 0.0,   -0.919432, 0.0,    0.280262, 0.0,    -0.66302, 0.0,   -0.558099, 0.0,   -0.537469, 0.0,
     -0.598779, 0.0,    0.929656, 0.0,   -0.170794, 0.0,   -0.537163, 0.0,    0.312581, 0.0,    0.959442, 0.0,
      0.722652, 0.0,    0.499931, 0.0,    0.175616, 0.0,   -0.534874, 0.0,   -0.685115, 0.0,    0.444999, 0.0,
       0.17171, 0.0,    0.108202, 0.0,   -0.768704, 0.0,   -0.463828, 0.0,    0.254231, 0.0,    0.546014, 0.0,
      0.869474, 0.0,    0.875212, 0.0,   -0.944427, 0.0,    0.130724, 0.0,   -0.110185, 0.0,    0.312184, 0.0,
      -0.33138, 0.0,   -0.629206, 0.0,   0.0606546, 0.0,    0.722866, 0.0,  -0.0979477, 0.0,    0.821561, 0.0,
     0.0931258, 0.0,   -0.972808, 0.0,   0.0318151, 0.0,   -0.867033, 0.0,   -0.387228, 0.0,    0.280995, 0.0,
     -0.218189, 0.0,   -0.539178, 0.0,   -0.427359, 0.0,   -0.602075, 0.0,    0.311971, 0.0,    0.277974, 0.0,
      0.773159, 0.0,    0.592493, 0.0,  -0.0331884, 0.0,   -0.630854, 0.0,   -0.269947, 0.0,    0.339132, 0.0,
      0.581079, 0.0,    0.209461, 0.0,   -0.317433, 0.0,   -0.284993, 0.0,    0.181323, 0.0,    0.341634, 0.0,
      0.804959, 0.0,   -0.229572, 0.0,   -0.758907, 0.0,   -0.336721, 0.0,    0.605463, 0.0,   -0.991272, 0.0,
    -0.0188754, 0.0,   -0.300191, 0.0,    0.368307, 0.0,   -0.176135, 0.0,     -0.3832, 0.0,   -0.749569, 0.0,
       0.62356, 0.0,   -0.573938, 0.0,    0.278309, 0.0,   -0.971313, 0.0,    0.839994, 0.0,   -0.830686, 0.0,
      0.439078, 0.0,     0.66128, 0.0,    0.694514, 0.0,   0.0565042, 0.0,     0.54342, 0.0,   -0.438804, 0.0,
    -0.0228428, 0.0,   -0.687068, 0.0,    0.857267, 0.0,    0.301991, 0.0,   -0.494255, 0.0,   -0.941039, 0.0,
      0.775509, 0.0,    0.410575, 0.0,   -0.362081, 0.0,   -0.671534, 0.0,   -0.348379, 0.0,    0.932433, 0.0,
      0.886442, 0.0,    0.868681, 0.0,   -0.225666, 0.0,   -0.062211, 0.0,  -0.0976425, 0.0,   -0.641444, 0.0,
     -0.848112, 0.0,    0.724697, 0.0,    0.473503, 0.0,    0.998749, 0.0,    0.174701, 0.0,    0.559625, 0.0,
     -0.029099, 0.0,   -0.337392, 0.0,   -0.958129, 0.0,   -0.659785, 0.0,    0.236042, 0.0,   -0.246937, 0.0,
      0.659449, 0.0,   -0.027512, 0.0,    0.821897, 0.0,   -0.226215, 0.0,   0.0181735, 0.0,    0.500481, 0.0,
     -0.420127, 0.0,   -0.427878, 0.0,    0.566186, 0.0
};

/*****************************************************************************/
/* Platform specific faster noise functions support                          */
/* (Profiling revealed that the noise functions can take up to 50% of        */
/*  all the time required when rendering and current compilers cannot        */
/*  easily optimise them efficiently without some help from programmers!)    */
/*****************************************************************************/

#if USE_FASTER_NOISE
}
#include "fasternoise.h"
namespace pov
{
    #ifndef FASTER_NOISE_INIT
        #define FASTER_NOISE_INIT()
    #endif
#else
#if !defined(USE_AVX_FMA4_FOR_NOISE)
    #define OriNoise Noise
    #define OriDNoise DNoise
#endif

    #define FASTER_NOISE_INIT()
#endif

/*****************************************************************************
*
* FUNCTION
*
*   Initialize_Noise()
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Initialize_Noise()
{
#if defined(USE_AVX_FMA4_FOR_NOISE)
    Initialise_NoiseDispatch();
#endif
    InitTextureTable();

    /* are - initialize Perlin style noise function */
    InitSolidNoise();

    sintab = new DBL[SINTABSIZE];

    for(int i = 0; i < 267; i++)
        RTable[(i * 2) + 1] = RTable[i * 2] * 0.5;
    //    Debug_Info("%.10f  %.10f\n", (DBL)(RTable[i * 2] - (DBL)((float)(RTable[i * 2]))), (DBL)(RTable[(i * 2) + 1] - (DBL)((float)(RTable[(i * 2) + 1]))));

    for(int i = 0; i < SINTABSIZE; i++)
        sintab[i] = sin((DBL)i / SINTABSIZE * TWO_M_PI);
}

void Initialize_Waves(vector<double>& waveFrequencies, vector<Vector3d>& waveSources, unsigned int numberOfWaves)
{
    Vector3d point;

    waveFrequencies.clear();
    waveSources.clear();

    for(int i = 0, next_rand = -560851967; i < numberOfWaves; i++)
    {
        point = Vector3d((double)i,0.0,0.0);
        DNoise(point, point);
        waveSources.push_back(point.normalized());

        next_rand = next_rand * 1812433253L + 12345L;
        waveFrequencies.push_back((double((int)(next_rand >> 16) & 0x7FFF) * 0.000030518509476) + 0.01);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   InitTextureTable()
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void InitTextureTable()
{
    unsigned short j, temp;
    int i;
    int next_rand = 0;

    #ifdef DYNAMIC_HASHTABLE
        hashTable = new unsigned short[8192];
    #endif

    for(i = 0; i < 4096; i++)
        hashTable[i] = i;

    for(i = 4095; i >= 0; i--)
    {
        next_rand = next_rand * 1812433253L + 12345L;
        j = ((int)(next_rand >> 16) & 0x7FFF) % 4096;
        temp = hashTable[i];
        hashTable[i] = hashTable[j];
        hashTable[j] = temp;
    }

    for(i = 0; i < 4096; i++)
        hashTable[4096 + i] = hashTable[i];

    FASTER_NOISE_INIT();
}



/*****************************************************************************
*
* FUNCTION
*
*   Free_Noise_Tables()
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Free_Noise_Tables()
{
    if (sintab != NULL)
    {
        delete[] sintab;
        sintab = NULL;

#ifdef DYNAMIC_HASHTABLE
        delete[] hashTable;
        hashTable = NULL;
#endif
    }
}



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

DBL OriNoise(const Vector3d& EPoint, int noise_generator)
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
    ix = (int)((tmp-MINX)&0xFFF);
    x_ix = x-tmp;

    tmp = (y>=0)?(int)y:(int)(y-(1-EPSILON));
    iy = (int)((tmp-MINY)&0xFFF);
    y_iy = y-tmp;

    tmp = (z>=0)?(int)z:(int)(z-(1-EPSILON));
    iz = (int)((tmp-MINZ)&0xFFF);
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

void OriDNoise(Vector3d& result, const Vector3d& EPoint)
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
    ix = (int)((tmp-MINX)&0xFFF);
    x_ix = x-tmp;

    tmp = (y>=0)?(int)y:(int)(y-(1-EPSILON));
    iy = (int)((tmp-MINY)&0xFFF);
    y_iy = y-tmp;

    tmp = (z>=0)?(int)z:(int)(z-(1-EPSILON));
    iz = (int)((tmp-MINZ)&0xFFF);
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

// Note that the value of NoiseEntries must be a power of 2.  This
// is because bit masking using (NoiseEntries-1) is used to rescale
// the input values to the noise function.
const int NoiseEntries = 2048;
static int NoisePermutation[2*(NoiseEntries+1)];
static Vector3d NoiseGradients[2*(NoiseEntries+1)];

const DBL ROLLOVER = 10000000.023157213;

static void
InitSolidNoise(void)
{
    int i, j, k;
    Vector3d v;
    DBL s;

    // Create an array of random gradient vectors uniformly on the unit sphere
    int next_rand = 1;
    for(i = 0; i < NoiseEntries; i++)
    {
        do
        {
            for(j = 0; j < 3; j++)
            {
                next_rand = next_rand * 1812433253L + 12345L;
                v[j] = (DBL)((((int)(next_rand >> 16) & 0x7FFF) % (NoiseEntries << 1)) - NoiseEntries) / (DBL)NoiseEntries;
            }
            s = v.lengthSqr();
        } while ((s > 1.0) || (s < 1.0e-5));
        v /= sqrt(s);

        NoiseGradients[i] = v;
    }
    // Create a pseudorandom permutation of [0..NoiseEntries]
    for(i = 0; i < NoiseEntries; i++)
        NoisePermutation[i] = i;
    for(i = NoiseEntries; i > 0; i -= 2)
    {
        k = NoisePermutation[i];
        next_rand = next_rand * 1812433253L + 12345L;
        j = ((int)(next_rand >> 16) & 0x7FFF) % NoiseEntries;
        NoisePermutation[i] = NoisePermutation[j];
        NoisePermutation[j] = k;
    }
    // Duplicate the entries so that we don't need a modulus operation
    // to get a value out.
    for(i = 0; i < NoiseEntries + 2; i++)
    {
        NoisePermutation[NoiseEntries + i] = NoisePermutation[i];
        NoiseGradients[NoiseEntries + i] = NoiseGradients[i];
    }
}

// Hermite curve from 0 to 1.  Makes a nice smooth transition of values.
static DBL inline
SCurve(DBL t)
{
    return (t * t * (3.0 - 2.0 * t));
}


// Linear interpolation between a and b, as the value of t goes from 0 to 1.
static DBL inline
Lerp(DBL t, DBL a, DBL b)
{
    return ((a) + (t) * ((b) - (a)));
}

// Linear interpolation between a and b, as the value of t goes from 0 to 1.
static void inline
VLerp(Vector3d& v, DBL t, const Vector3d& a, const Vector3d& b)
{
    v[X] = Lerp(t, a[X], b[X]);
    v[Y] = Lerp(t, a[Y], b[Y]);
    v[Z] = Lerp(t, a[Z], b[Z]);
}

static void inline
SetupSolidNoise(const Vector3d& P, int i, int &b0, int &b1, DBL &r0, DBL &r1)
{
    DBL t = P[i] + ROLLOVER;

    int it = (int)floor(t);
    b0 = it & (NoiseEntries - 1);
    b1 = (b0 + 1) & (NoiseEntries - 1);
    r0 = t - it;
    r1 = r0 - 1.0;
}

static DBL inline
NoiseValueAt(const Vector3d& q, DBL rx, DBL ry, DBL rz)
{
    return (rx * q[X] + ry * q[Y] + rz * q[Z]);
}

static DBL
SolidNoise(const Vector3d& P)
{
    int bx0, bx1, by0, by1, bz0, bz1;
    int b00, b10, b01, b11;
    DBL rx0, rx1, ry0, ry1, rz0, rz1;
    DBL sx, sy, sz, a, b, c, d, t, u, v;
    int i, j;

    SetupSolidNoise(P, 0, bx0, bx1, rx0, rx1);
    SetupSolidNoise(P, 1, by0, by1, ry0, ry1);
    SetupSolidNoise(P, 2, bz0, bz1, rz0, rz1);

    i = NoisePermutation[bx0];
    j = NoisePermutation[bx1];

    b00 = NoisePermutation[i + by0];
    b10 = NoisePermutation[j + by0];
    b01 = NoisePermutation[i + by1];
    b11 = NoisePermutation[j + by1];

    sx = SCurve(rx0);
    sy = SCurve(ry0);
    sz = SCurve(rz0);

    u = NoiseValueAt(NoiseGradients[b00 + bz0], rx0, ry0, rz0);
    v = NoiseValueAt(NoiseGradients[b10 + bz0], rx1, ry0, rz0);
    a = Lerp(sx, u, v);

    u = NoiseValueAt(NoiseGradients[b01 + bz0], rx0, ry1, rz0);
    v = NoiseValueAt(NoiseGradients[b11 + bz0], rx1, ry1, rz0);
    b = Lerp(sx, u, v);

    c = Lerp(sy, a, b);

    u = NoiseValueAt(NoiseGradients[b00 + bz1], rx0, ry0, rz1);
    v = NoiseValueAt(NoiseGradients[b10 + bz1], rx1, ry0, rz1);
    a = Lerp(sx, u, v);

    u = NoiseValueAt(NoiseGradients[b01 + bz1], rx0, ry1, rz1);
    v = NoiseValueAt(NoiseGradients[b11 + bz1], rx1, ry1, rz1);
    b = Lerp(sx, u, v);

    d = Lerp(sy, a, b);

    t = Lerp(sz, c, d);

    return t;
}

static void
SolidDNoise(const Vector3d& P, Vector3d& D)
{
    int bx0, bx1, by0, by1, bz0, bz1;
    int b00, b10, b01, b11;
    DBL rx0, rx1, ry0, ry1, rz0, rz1;
    DBL sx, sy, sz;
    Vector3d a, b, c, d, u, v;
    int i, j;

    SetupSolidNoise(P, 0, bx0, bx1, rx0, rx1);
    SetupSolidNoise(P, 1, by0, by1, ry0, ry1);
    SetupSolidNoise(P, 2, bz0, bz1, rz0, rz1);

    i = NoisePermutation[bx0];
    j = NoisePermutation[bx1];

    b00 = NoisePermutation[i + by0];
    b10 = NoisePermutation[j + by0];
    b01 = NoisePermutation[i + by1];
    b11 = NoisePermutation[j + by1];

    sx = SCurve(rx0);
    sy = SCurve(ry0);
    sz = SCurve(rz0);


    u = NoiseGradients[b00 + bz0];
    v = NoiseGradients[b10 + bz0];
    VLerp(a, sx, u, v);

    u = NoiseGradients[b01 + bz0];
    v = NoiseGradients[b11 + bz0];
    VLerp(b, sx, u, v);

    VLerp(c, sy, a, b);

    u = NoiseGradients[b00 + bz1];
    v = NoiseGradients[b10 + bz1];
    VLerp(a, sx, u, v);

    u = NoiseGradients[b01 + bz1];
    v = NoiseGradients[b11 + bz1];
    VLerp(b, sx, u, v);

    VLerp(d, sy, a, b);

    VLerp(D, sz, c, d);
}


/*****************************************************************************
*
* FUNCTION
*
*   Turbulence
*
* INPUT
*
*   EPoint -- Point at which turb is evaluated.
*   Turb   -- Parameters for fbm calculations.
*
* OUTPUT
*
* RETURNS
*
*   DBL result
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Computes a Fractal Brownian Motion turbulence value
*                 using repeated calls to a Perlin Noise function.
*
* CHANGES
*   ??? ???? : Updated with varible Octaves, Lambda, & Omega by [DMF]
*
******************************************************************************/

DBL Turbulence(const Vector3d& EPoint, const GenericTurbulenceWarp *Turb, int noise_generator)
{
    int i;
    DBL Lambda, Omega, l, o, value;
    Vector3d temp;
    int Octaves=Turb->Octaves;

    // TODO - This distinction (with minor variations that seem to be more of an inconsistency rather than intentional)
    // appears in other places as well; make it a function.
    switch(noise_generator)
    {
        case kNoiseGen_Default:
        case kNoiseGen_Original:
            value = Noise(EPoint, noise_generator);
            break;
        default:
            value = (2.0 * Noise(EPoint, noise_generator) - 0.5);
            value = min(max(value,0.0),1.0);
            break;
    }

    l = Lambda = Turb->Lambda;
    o = Omega  = Turb->Omega;

    for (i = 2; i <= Octaves; i++)
    {
        temp = EPoint * l;
        // TODO - This distinction (with minor variations that seem to be more of an inconsistency rather than intentional)
        // appears in other places as well; make it a function.
        switch(noise_generator)
        {
            case kNoiseGen_Default:
            case kNoiseGen_Original:
                value += o * Noise(temp, noise_generator);
                break;
            default:
                value += o * (2.0 * Noise(temp, noise_generator) - 0.5); // TODO similar code clips the (2.0 * Noise(temp, noise_generator) - 0.5) term
                break;
        }
        if (i < Octaves)
        {
            l *= Lambda;
            o *= Omega;
        }
    }
    return (value);
}



/*****************************************************************************
*
* FUNCTION
*
*   DTurbulence
*
* INPUT
*
*   EPoint -- Point at which turb is evaluated.
*   Turb   -- Parameters for fmb calculations.
*
* OUTPUT
*
*   result -- Vector valued turbulence
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Computes a Fractal Brownian Motion turbulence value
*                 using repeated calls to a Perlin DNoise function.
*
* CHANGES
*   ??? ???? : Updated with varible Octaves, Lambda, & Omega by [DMF]
*
******************************************************************************/


void DTurbulence(Vector3d& result, const Vector3d& EPoint, const GenericTurbulenceWarp *Turb)
{
    DBL Omega, Lambda;
    int i;
    DBL l, o;
    Vector3d value, temp;
    int Octaves=Turb->Octaves;

    result[X] = result[Y] = result[Z] = 0.0;
    value[X]  = value[Y]  = value[Z]  = 0.0;

    DNoise(result, EPoint);

    l = Lambda = Turb->Lambda;
    o = Omega  = Turb->Omega;

    for (i = 2; i <= Octaves; i++)
    {
        temp = EPoint * l;

        DNoise(value, temp);
        result += o * value;
        if (i < Octaves)
        {
            l *= Lambda;
            o *= Omega;
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   cycloidal
*
* INPUT
*
*   DBL value
*
* OUTPUT
*
* RETURNS
*
*   DBL result
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL cycloidal(DBL value)
{
    if (value >= 0.0)
    {
        return sin((DBL) (((value - floor(value)) * 50000.0)) / 50000.0 * TWO_M_PI);
    }
    else
    {
        return 0.0-sin((DBL) (((0.0 - (value + floor(0.0 - value))) * 50000.0)) / 50000.0 * TWO_M_PI);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Triangle_Wave
*
* INPUT
*
*   DBL value
*
* OUTPUT
*
* RETURNS
*
*   DBL result
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Triangle_Wave(DBL value)
{
    register DBL offset;

    if (value >= 0.0)
    {
        offset = value - floor(value);
    }
    else
    {
        offset = value + 1.0 + floor(fabs(value));
    }
    if (offset >= 0.5)
    {
        return (2.0 * (1.0 - offset));
    }
    else
    {
        return (2.0 * offset);
    }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Transform_Textures(TEXTURE *Textures, const TRANSFORM *Trans)
{
    TEXTURE *Layer;

    for (Layer = Textures; Layer != NULL; Layer = Layer->Next)
    {
        if (Layer->Type == PLAIN_PATTERN)
        {
            Transform_Tpattern(Layer->Pigment, Trans);
            Transform_Tpattern(Layer->Tnormal, Trans);
        }
        else
        {
            Transform_Tpattern(Layer, Trans);
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*   6/27/98  MBP  Added initializers for reflection blur
*   8/27/98  MBP  Added initializers for angle-based reflectivity
*
******************************************************************************/

FINISH *Create_Finish()
{
    FINISH *New;

    New = new FINISH;

    New->Ambient.Set(0.1);
    New->Emission.Clear();
    New->Reflection_Max.Clear();
    New->Reflection_Min.Clear();

    New->Reflection_Fresnel     = false;
    New->Reflection_Falloff     = 1;    /* Added by MBP 8/27/98 */
    New->Diffuse                = 0.6;
    New->DiffuseBack            = 0.0;
    New->Brilliance             = 1.0;
    New->BrillianceOut          = 1.0;
    New->BrillianceAdjust       = 1.0;
    New->BrillianceAdjustRad    = 1.0;
    New->Phong                  = 0.0;
    New->Phong_Size             = 40.0;
    New->Specular               = 0.0;
    New->Roughness              = 1.0 / 0.05;

    New->Crand = 0.0;

    New->Metallic = 0.0;
    New->Fresnel  = false;

    New->Irid                = 0.0;
    New->Irid_Film_Thickness = 0.0;
    New->Irid_Turb           = 0.0;
    New->Temp_Caustics = -1.0;
    New->Temp_IOR     = -1.0;
    New->Temp_Dispersion  = 1.0;
    New->Temp_Refract =  1.0;
    New->Reflect_Exp  =  1.0;
    New->Reflect_Metallic = 0.0;
    /* Added Dec 19 1999 by NK */
    New->Conserve_Energy = false;

    New->UseSubsurface = false;
    New->SubsurfaceTranslucency.Clear();
    New->SubsurfaceAnisotropy.Clear();

    return(New);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

FINISH *Copy_Finish(const FINISH *Old)
{
    FINISH *New;

    if (Old != NULL)
    {
        New = Create_Finish();
        *New = *Old;
    }
    else
        New = NULL;
    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TEXTURE *Create_Texture()
{
    TEXTURE *New;

    New = new TEXTURE;

    Init_TPat_Fields(New);

    New->Next = NULL;
    New->References = 1;

    New->Type    = PLAIN_PATTERN;
    New->Flags  |= NO_FLAGS; // [CLi] Already initialized by Init_TPat_Fields

    New->Pigment = NULL;
    New->Tnormal = NULL;
    New->Finish  = NULL;

    New->Next    = NULL;

    return (New);
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TEXTURE *Copy_Texture_Pointer(TEXTURE *Texture)
{
    if (Texture != NULL)
    {
        Texture->References++;
    }

    return(Texture);
}




/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TEXTURE *Copy_Textures(TEXTURE *Textures)
{
    TEXTURE *New, *First, *Previous;
    TEXTURE *Layer;

    Previous = First = NULL;

    for (Layer = Textures; Layer != NULL; Layer = Layer->Next)
    {
        New = Create_Texture();
        Copy_TPat_Fields (New, Layer);
        New->Blend_Map = Copy_Blend_Map<TextureBlendMap>(Layer->Blend_Map);

        /*  Mesh copies a texture pointer that already has multiple
            references.  We just want a clean copy, not a copy
            that's multply referenced.
         */

        New->References = 1;

        switch (Layer->Type)
        {
            case PLAIN_PATTERN:
                New->Pigment = Copy_Pigment(Layer->Pigment);
                New->Tnormal = Copy_Tnormal(Layer->Tnormal);
                New->Finish  = Copy_Finish(Layer->Finish);

                break;

            case BITMAP_PATTERN:
                New->Materials.reserve(Layer->Materials.size());
                for (vector<TEXTURE*>::const_iterator i = Layer->Materials.begin(); i != Layer->Materials.end(); ++ i)
                    New->Materials.push_back(Copy_Textures(*i));

//              Not needed. Copied by Copy_TPat_Fields:
//              New->Vals.Image  = Copy_Image(Layer->Vals.Image);

                break;
        }

        if (First == NULL)
        {
            First = New;
        }

        if (Previous != NULL)
        {
            Previous->Next = New;
        }

        Previous = New;
    }

    return (First);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Destroy_Textures(TEXTURE *Textures)
{
    TEXTURE *Layer = Textures;
    TEXTURE *Temp;

    if ((Textures == NULL) || (--(Textures->References) > 0))
    {
        return;
    }

    while (Layer != NULL)
    {
        // Theoretically these should only be non-NULL for PLAIN_PATTERN, but let's clean them up either way.
        Destroy_Pigment(Layer->Pigment);
        Destroy_Tnormal(Layer->Tnormal);
        if (Layer->Finish)
            delete Layer->Finish;

        // Theoretically these should only be non-empty for BITMAP_PATTERN, but let's clean them up either way.
        for(vector<TEXTURE*>::iterator i = Layer->Materials.begin(); i != Layer->Materials.end(); ++ i)
            Destroy_Textures(*i);

        Temp = Layer->Next;
        delete Layer;
        Layer = Temp;
    }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Post_Textures(TEXTURE *Textures)
{
    TEXTURE *Layer;
    TextureBlendMap *Map;

    if (Textures == NULL)
    {
        return;
    }

    for (Layer = Textures; Layer != NULL; Layer = Layer->Next)
    {
        if (!((Layer->Flags) & POST_DONE))
        {
            switch (Layer->Type)
            {
                case PLAIN_PATTERN:

                    if(Layer->Tnormal)
                    {
                        Layer->Tnormal->Flags |=
                            (Layer->Flags & DONT_SCALE_BUMPS_FLAG);
                    }
                    Post_Pigment(Layer->Pigment);
                    Post_Tnormal(Layer->Tnormal);

                    break;

                case BITMAP_PATTERN:

                    for (vector<TEXTURE*>::iterator i = Layer->Materials.begin(); i != Layer->Materials.end(); ++ i)

                        Post_Textures(*i);

                    break;
            }

            if ((Map=Layer->Blend_Map.get()) != NULL)
            {
                for(vector<TextureBlendMapEntry>::iterator i = Map->Blend_Map_Entries.begin(); i != Map->Blend_Map_Entries.end(); i++)
                {
                    i->Vals->Flags |=
                        (Layer->Flags & DONT_SCALE_BUMPS_FLAG);
                    Post_Textures(i->Vals);
                }
            }
            else
            {
                if (Layer->Type == AVERAGE_PATTERN)
                {
                    throw POV_EXCEPTION_STRING("No texture map in averaged texture.");
                }
            }
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Test_Opacity
*
* INPUT
*
*   Object - Pointer to object
*
* OUTPUT
*
* RETURNS
*
*   int - true, if opaque
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Test wether an object is opaque or not, i.e. wether the texture contains
*   a non-zero filter or alpha channel.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
*   Oct 1994 : Added code to check for opaque image maps. [DB]
*
*   Jun 1995 : Added code to check for alpha channel image maps. [DB]
*
******************************************************************************/

int Test_Opacity(const TEXTURE *Texture)
{
    int Opaque, Help;
    const TEXTURE *Layer;

    if (Texture == NULL)
    {
        return(false);
    }

    /* We assume that the object is not opaque. */

    Opaque = false;

    /* Test all layers. If at least one layer is opaque the object is opaque. */

    for (Layer = Texture; Layer != NULL; Layer = Layer->Next)
    {
        switch (Layer->Type)
        {
            case PLAIN_PATTERN:

                /* Test image map for opacity. */

                if (!(Layer->Pigment->Flags & HAS_FILTER))
                {
                    Opaque = true;
                }

                break;

            case BITMAP_PATTERN:

                /* Layer is not opaque if the image map is used just once. */

                if (dynamic_cast<ImagePattern*>(Layer->pattern.get())->pImage != NULL)
                {
                    if (dynamic_cast<ImagePattern*>(Layer->pattern.get())->pImage->Once_Flag)
                    {
                        break;
                    }
                }

                /* Layer is opaque if all materials are opaque. */

                Help = true;

                for (vector<TEXTURE*>::const_iterator i = Layer->Materials.begin(); i != Layer->Materials.end(); ++ i)
                {
                    if (!Test_Opacity(*i))
                    {
                        /* Material is not opaque --> layer is not opaque. */

                        Help = false;

                        break;
                    }
                }

                if (Help)
                {
                    Opaque = true;
                }

                break;
        }
    }

    return(Opaque);
}


#if defined(USE_AVX_FMA4_FOR_NOISE)

/********************************************************************************************/
/* AMD Specific optimizations: Its found that more than 50% of the time is spent in         */
/* Noise and DNoise. These functions have been optimized using AVX and FMA4 instructions    */
/*                                                                                          */
/********************************************************************************************/
DBL (*Noise) (const Vector3d& EPoint, int noise_generator);
void (*DNoise) (Vector3d& result, const Vector3d& EPoint);

/*****************************************************************************
*
* FUNCTION
*
*   Initialise_NoiseDispatch
*
* INPUT
*
*  None
*
* OUTPUT
*       Initialises the Noise and DNoise Function pointers to the right functions
*
*
* RETURNS
*
*  None
*
* AUTHOR
*
*   AMD
*
* DESCRIPTION
*
* CHANGES
*
*
******************************************************************************/

void Initialise_NoiseDispatch()
{
    static bool cpu_detected = false;

    if(!cpu_detected)
    {
            if(CPU_FMA4_DETECT())
            {
                Noise = AVX_FMA4_Noise;
                DNoise = AVX_FMA4_DNoise;
            }
            else
            {
                Noise = OriNoise;
                DNoise = OriDNoise;
            }

            cpu_detected = true;
    }
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
*    July 28, 2011: Re-wrote OriNoise function using AVX and FMA4 intrinsic
*
******************************************************************************/

DBL AVX_FMA4_Noise(const Vector3d& EPoint, int noise_generator)
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
    ix = (int)((tmp-MINX)&0xFFF);
    x_ix = x-tmp;

    tmp = (y>=0)?(int)y:(int)(y-(1-EPSILON));
    iy = (int)((tmp-MINY)&0xFFF);
    y_iy = y-tmp;

    tmp = (z>=0)?(int)z:(int)(z-(1-EPSILON));
    iz = (int)((tmp-MINZ)&0xFFF);
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
*    July 28, 2011:Re-wrote OriDNoise function using AVX and FMA4 intrinsic
*
******************************************************************************/

void AVX_FMA4_DNoise(Vector3d& result, const Vector3d& EPoint)
{
    DBL x, y, z;
    int ix, iy, iz;
    DBL *mp;
    int tmp;
    int ixiy_hash, ixjy_hash, jxiy_hash, jxjy_hash;
    DBL x_ix,y_iy,z_iz;

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
    ix = (int)((tmp-MINX)&0xFFF);
    x_ix = x-tmp;

    tmp = (y>=0)?(int)y:(int)(y-(1-EPSILON));
    iy = (int)((tmp-MINY)&0xFFF);
    y_iy = y-tmp;

    tmp = (z>=0)?(int)z:(int)(z-(1-EPSILON));
    iz = (int)((tmp-MINZ)&0xFFF);
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

#endif


//******************************************************************************

TextureBlendMap::TextureBlendMap() : BlendMap<TexturePtr>(TEXTURE_TYPE) {}

TextureBlendMap::~TextureBlendMap()
{
    for (Vector::iterator i = Blend_Map_Entries.begin(); i != Blend_Map_Entries.end(); i++)
        Destroy_Textures(i->Vals);
}

}
