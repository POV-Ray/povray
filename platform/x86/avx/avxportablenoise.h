//******************************************************************************
///
/// @file platform/x86/avx/avxportablenoise.h
///
/// This file contains declarations related to implementations of the noise
/// generator optimized for the AVX and FMA4 instruction set.
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

#ifndef POVRAY_AVXPORTABLENOISE_H
#define POVRAY_AVXPORTABLENOISE_H

#include "core/configcore.h"
#include "core/math/vector.h"

#ifdef TRY_OPTIMIZED_NOISE_AVX_PORTABLE

namespace pov
{

extern const bool kAVXPortableNoiseEnabled;

DBL AVXPortableNoise(const Vector3d& EPoint, int noise_generator);

void AVXPortableDNoise(Vector3d& result, const Vector3d& EPoint);

}
// end of namespace pov

#endif // TRY_OPTIMIZED_NOISE_AVX_PORTABLE

#endif // POVRAY_AVXPORTABLENOISE_H
