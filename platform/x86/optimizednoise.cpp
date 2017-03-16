//******************************************************************************
///
/// @file platform/x86/optimizednoise.cpp
///
/// This file contains implementations related to the optimized versions of the
/// noise generator.
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

#include "optimizednoise.h"

#include "cpuid.h"
#include "avx2fma3/avx2fma3noise.h"
#include "avxfma4/avxfma4noise.h"
#include "avx/avxnoise.h"

#ifdef TRY_OPTIMIZED_NOISE

namespace pov
{

//******************************************************************************

#ifdef TRY_OPTIMIZED_NOISE_AVX2FMA3

bool OptimizedNoiseAVX2FMA3::initialized = false;

OptimizedNoiseAVX2FMA3::OptimizedNoiseAVX2FMA3()
{
    if (!initialized)
    {
        AVX2FMA3NoiseInit();
        initialized = true;
    }
}

#endif

//******************************************************************************

#ifdef TRY_OPTIMIZED_NOISE_AVXFMA4

// no special code

#endif

//******************************************************************************

#ifdef TRY_OPTIMIZED_NOISE_AVX

bool OptimizedNoiseAVX::initialized = false;

OptimizedNoiseAVX::OptimizedNoiseAVX()
{
    if (!initialized)
    {
        AVXNoiseInit();
        initialized = true;
    }
}

#endif

//******************************************************************************

OptimizedNoiseBase* GetOptimizedNoise()
{
    // TODO - review priority
#ifdef TRY_OPTIMIZED_NOISE_AVX2FMA3
    if (HaveAVX2FMA3())
        return new OptimizedNoiseAVX2FMA3();
#endif
#ifdef TRY_OPTIMIZED_NOISE_AVXFMA4
    if (HaveAVXFMA4())
        return new OptimizedNoiseAVXFMA4();
#endif
#ifdef TRY_OPTIMIZED_NOISE_AVX
    if (HaveAVX())
        return new OptimizedNoiseAVX();
#endif
    return NULL;
}

}

#endif // TRY_OPTIMIZED_NOISE
