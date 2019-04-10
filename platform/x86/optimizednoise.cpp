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
#include "optimizednoise.h"

#include "core/material/noise.h"

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

static bool AVXSupported()      { return CPUInfo::SupportsAVX(); }
static bool AVXFMA4Supported()  { return CPUInfo::SupportsAVX() && CPUInfo::SupportsFMA4(); }
static bool AVX2FMA3Supported() { return CPUInfo::SupportsAVX2() && CPUInfo::SupportsFMA3(); }

/// List of optimized noise implementations.
///
/// @note
///     Entries must be listed in descending order of preference.
///
OptimizedNoiseInfo gaOptimizedNoiseInfo[] = {
#ifdef TRY_OPTIMIZED_NOISE_AVX2FMA3
    {
        "avx2fma3-intel",           // name,
        "hand-optimized by Intel",  // info,
        AVX2FMA3Noise,              // noise,
        AVX2FMA3DNoise,             // dNoise,
        &kAVX2FMA3NoiseEnabled,     // enabled,
        AVX2FMA3Supported,          // supported,
        CPUInfo::IsIntel,           // recommended,
        AVX2FMA3NoiseInit           // init
    },
#endif
#ifdef TRY_OPTIMIZED_NOISE_AVXFMA4
    {
        "avxfma4-amd.2",            // name,
        "hand-optimized by AMD, 2017-04 update", // info,
        AVXFMA4Noise,               // noise,
        AVXFMA4DNoise,              // dNoise,
        &kAVXFMA4NoiseEnabled,      // enabled,
        AVXFMA4Supported,           // supported,
        nullptr,                    // recommended,
        nullptr                     // init
    },
#endif
#ifdef TRY_OPTIMIZED_NOISE_AVX
    {
        "avx-intel",                // name,
        "hand-optimized by Intel",  // info,
        AVXNoise,                   // noise,
        AVXDNoise,                  // dNoise,
        &kAVXNoiseEnabled,          // enabled,
        AVXSupported,               // supported,
        CPUInfo::IsIntel,           // recommended,
        AVXNoiseInit                // init
    },
#endif
#ifdef TRY_OPTIMIZED_NOISE_AVX_PORTABLE
    {
        "avx-generic",              // name,
        "compiler-optimized",       // info,
        AVXPortableNoise,           // noise,
        AVXPortableDNoise,          // dNoise,
        &kAVXPortableNoiseEnabled,  // enabled,
        AVXSupported,               // supported,
        nullptr,                    // recommended,
        nullptr                     // init
    },
#endif
    // End-of-list entry.
    { nullptr }
};

}
// end of namespace pov

#endif // TRY_OPTIMIZED_NOISE
