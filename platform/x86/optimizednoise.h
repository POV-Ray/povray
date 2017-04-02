//******************************************************************************
///
/// @file platform/x86/optimizednoise.h
///
/// This file dispatches to declarations related to implementations of the noise
/// generator optimized for various specific extended x86 instruction set.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
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

#ifndef POVRAY_OPTIMIZEDNOISE_H
#define POVRAY_OPTIMIZEDNOISE_H

#include "syspovconfigbase.h"
#include "backend/frame.h"

#ifdef TRY_OPTIMIZED_NOISE_AVX2FMA3
#include "avx2fma3noise.h"
#endif

#ifdef TRY_OPTIMIZED_NOISE_AVXFMA4
#include "avxfma4noise.h"
#endif

#ifdef TRY_OPTIMIZED_NOISE_AVX
#include "avxnoise.h"
#endif

#ifdef TRY_OPTIMIZED_NOISE

namespace pov
{

typedef DBL (*NoiseFunction) (const Vector3d& EPoint, int noise_generator);
typedef void (*DNoiseFunction) (Vector3d& result, const Vector3d& EPoint);

inline bool TryOptimizedNoise(NoiseFunction* pFnNoise, DNoiseFunction* pFnDNoise)
{
    // TODO - review priority
    // NOTE - Any change to the priorization should also be reflected in `pvengine.cpp`.
#ifdef TRY_OPTIMIZED_NOISE_AVX2FMA3
    if (AVX2FMA3NoiseSupported())
    {
        AVX2FMA3NoiseInit();
        *pFnNoise  = AVX2FMA3Noise;
        *pFnDNoise = AVX2FMA3DNoise;
        return true;
    }
#endif
#ifdef TRY_OPTIMIZED_NOISE_AVXFMA4
    if (AVXFMA4NoiseSupported())
    {
        *pFnNoise  = AVXFMA4Noise;
        *pFnDNoise = AVXFMA4DNoise;
        return true;
    }
#endif
#ifdef TRY_OPTIMIZED_NOISE_AVX
    if (AVXNoiseSupported())
    {
        AVXNoiseInit();
        *pFnNoise  = AVXNoise;
        *pFnDNoise = AVXDNoise;
        return true;
    }
#endif
    return false;
}

}

#endif // TRY_OPTIMIZED_NOISE

#endif // POVRAY_OPTIMIZEDNOISE_H
