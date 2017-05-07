//******************************************************************************
///
/// @file platform/x86/optimizednoise.cpp
///
/// Implementations related to the dynamic dispatch of the optimized noise
/// generator implementations for the x86 family of CPUs.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "optimizednoise.h"

#ifdef TRY_OPTIMIZED_NOISE_AVX2FMA3
#include "avx2fma3/avx2fma3noise.h"
#endif

#ifdef TRY_OPTIMIZED_NOISE_AVXFMA4
#include "avxfma4/avxfma4noise.h"
#endif

#ifdef TRY_OPTIMIZED_NOISE_AVX
#include "avx/avxnoise.h"
#endif

#ifdef TRY_OPTIMIZED_NOISE_AVX_PORTABLE
#include "avx/avxportablenoise.h"
#endif

#include "cpuid.h"

#ifdef TRY_OPTIMIZED_NOISE

namespace pov
{

bool TryOptimizedNoise(NoiseFunction* pFnNoise, DNoiseFunction* pFnDNoise,
                       std::string* pDetected, std::string* pImpl)
{
    bool doInit = (pFnNoise || pFnDNoise);
#ifdef TRY_OPTIMIZED_NOISE_AVX2FMA3
    if (HaveAVX2FMA3() && IsIntelCPU())
    {
        if (doInit)     AVX2FMA3NoiseInit();
        if (pFnNoise)   *pFnNoise   = AVX2FMA3Noise;
        if (pFnDNoise)  *pFnDNoise  = AVX2FMA3DNoise;
        if (pDetected)  *pDetected  = "AVX2,FMA3,Intel";
        if (pImpl)      *pImpl      = "hand-optimized for AVX2/FMA3 by Intel";
        return true;
    }
#endif
#ifdef TRY_OPTIMIZED_NOISE_AVXFMA4
    if (HaveAVXFMA4())
    {
        if (pFnNoise)   *pFnNoise   = AVXFMA4Noise;
        if (pFnDNoise)  *pFnDNoise  = AVXFMA4DNoise;
        if (pDetected)  *pDetected  = "AVX,FMA4";
        if (pImpl)      *pImpl      = "hand-optimized for AVX/FMA4 by AMD (2017-04 update)";
        return true;
    }
#endif
#ifdef TRY_OPTIMIZED_NOISE_AVX
    if (HaveAVX() && IsIntelCPU())
    {
        if (doInit)     AVXNoiseInit();
        if (pFnNoise)   *pFnNoise   = AVXNoise;
        if (pFnDNoise)  *pFnDNoise  = AVXDNoise;
        if (pDetected)  *pDetected  = "AVX,Intel";
        if (pImpl)      *pImpl      = "hand-optimized for AVX by Intel";
        return true;
    }
#endif
#ifdef TRY_OPTIMIZED_NOISE_AVX_PORTABLE
    if (HaveAVX())
    {
        if (pFnNoise)   *pFnNoise   = AVXPortableNoise;
        if (pFnDNoise)  *pFnDNoise  = AVXPortableDNoise;
        if (pDetected)  *pDetected  = "AVX";
        if (pImpl)      *pImpl      = "compiler-optimized for AVX";
        return true;
    }
#endif
    return false;
}

}

#endif // TRY_OPTIMIZED_NOISE
