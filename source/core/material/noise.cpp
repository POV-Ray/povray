//******************************************************************************
///
/// @file core/material/noise.cpp
///
/// Implementations related to noise.
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
#include "core/material/noise.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
#include "base/povassert.h"

// POV-Ray header files (core module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::min;
using std::max;

/*****************************************************************************
* Static functions
******************************************************************************/

static void InitTextureTable(void);
static void InitSolidNoise(void);

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Ridiculously large scaling values */

const int SINTABSIZE = 1000;



/*****************************************************************************
* Local variables
******************************************************************************/

static DBL *sintab; // GLOBAL VARIABLE

#ifdef DYNAMIC_HASHTABLE
unsigned short *hashTable; // GLOBAL VARIABLE
#else
alignas(16) unsigned short hashTable[8192]; // GLOBAL VARIABLE
#endif

alignas(16) DBL RTable[267*2] =
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
#ifdef TRY_OPTIMIZED_NOISE
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

void Initialize_Waves(std::vector<double>& waveFrequencies, std::vector<Vector3d>& waveSources, unsigned int numberOfWaves)
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
    if (sintab != nullptr)
    {
        delete[] sintab;
        sintab = nullptr;

#ifdef DYNAMIC_HASHTABLE
        delete[] hashTable;
        hashTable = nullptr;
#endif
    }
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

DBL
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


#ifdef TRY_OPTIMIZED_NOISE

NoiseFunction Noise;
DNoiseFunction DNoise;

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
        cpu_detected = true;
        const OptimizedNoiseInfo* pNoiseImpl = GetRecommendedOptimizedNoise();
        if (pNoiseImpl->init) pNoiseImpl->init();
        Noise = pNoiseImpl->noise;
        DNoise = pNoiseImpl->dNoise;
    }
}

OptimizedNoiseInfo gPortableNoiseInfo = {
    "generic",      // name,
    "portable",     // info,
    PortableNoise,  // noise,
    PortableDNoise, // dNoise,
    nullptr,        // enabled,
    nullptr,        // supported,
    nullptr,        // recommended,
    nullptr         // init
};

const OptimizedNoiseInfo* GetRecommendedOptimizedNoise()
{
    for (const OptimizedNoiseInfo* p = gaOptimizedNoiseInfo; p->name != nullptr; ++p)
    {
        if ((p->enabled == nullptr) || *p->enabled)
        {
            POV_CORE_ASSERT(p->supported);
            if (p->supported() && ((p->recommended == nullptr) || p->recommended()))
                return p;
        }
    }

    // No optimized implementation found; go for the portable implementation.
    return &gPortableNoiseInfo;
}

const OptimizedNoiseInfo* GetOptimizedNoise(std::string name)
{
    for (const OptimizedNoiseInfo* p = gaOptimizedNoiseInfo; p->name != nullptr; ++p)
    {
        if ((p->enabled == nullptr) || *p->enabled)
        {
            POV_CORE_ASSERT(p->supported);
            if (p->name == name)
                return p;
        }
    }

    // Specified implementation not found; go for the portable implementation.
    return &gPortableNoiseInfo;
}

#endif // TRY_OPTIMIZED_NOISE

}
// end of namespace pov
