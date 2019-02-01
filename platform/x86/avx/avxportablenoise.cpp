//******************************************************************************
///
/// @file platform/x86/avx/avxportablenoise.cpp
///
/// This file serves as a stub to compile an alternative AVX-optimized version
/// of the default portable noise implementation.
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
#include "avxportablenoise.h"

#include "base/povassert.h"

#include "core/material/pattern.h"
#include "core/material/texture.h"

/// @file
/// @attention
///     This file **must not** contain any code that might get called before CPU
///     support for this optimized implementation has been confirmed. Most
///     notably, the function to detect support itself must not reside in this
///     file.

#ifdef TRY_OPTIMIZED_NOISE_AVX_PORTABLE

#ifndef DISABLE_OPTIMIZED_NOISE_AVX_PORTABLE

namespace pov
{
const bool kAVXPortableNoiseEnabled = true;
}
// end of namespace pov

#define PORTABLE_OPTIMIZED_NOISE
#define PortableNoise  AVXPortableNoise
#define PortableDNoise AVXPortableDNoise
#include "core/material/portablenoise.cpp" // pulls in the actual code

#else // DISABLE_OPTIMIZED_NOISE_AVX_PORTABLE

namespace pov
{
const bool kAVXPortableNoiseEnabled = false;
DBL AVXPortableNoise(const Vector3d& EPoint, int noise_generator) { POV_ASSERT(false); return 0.0; }
void AVXPortableDNoise(Vector3d& result, const Vector3d& EPoint) { POV_ASSERT(false); }
}
// end of namespace pov

#endif // DISABLE_OPTIMIZED_NOISE_AVX_PORTABLE

#endif // TRY_OPTIMIZED_NOISE_AVX_PORTABLE

