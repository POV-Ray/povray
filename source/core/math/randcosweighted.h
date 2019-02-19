//******************************************************************************
///
/// @file core/math/randcosweighted.h
///
/// Declaration of lookup table of cosine-weighted hemispherical random
/// directions.
///
/// This table was originally designed for radiosity, but is also used by photon
/// mapping now.
///
/// @author Jim McElhiney
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

#ifndef POVRAY_CORE_RANDCOSWEIGHTED_H
#define POVRAY_CORE_RANDCOSWEIGHTED_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/coretypes.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreMathRandomsequence Random Number Sequences
/// @ingroup PovCoreMath
///
/// @{

//******************************************************************************
///
/// @name Legacy Sub-Random Number Source
///
/// The following provide a table of 1600 sub-random (low discrepancy) vectors
/// on the unit sphere.
///
/// @{

struct BYTE_XYZ final
{
    unsigned char x, y, z;
};

const int kRandCosWeightedCount = 1600;
extern BYTE_XYZ kaRandCosWeighted[kRandCosWeightedCount];

inline void VUnpack(Vector3d& dest_vec, const BYTE_XYZ * pack_vec)
{
    dest_vec[X] = ((double)pack_vec->x * (1.0 / 255.0)) * 2.0 - 1.0;
    dest_vec[Y] = ((double)pack_vec->y * (1.0 / 255.0));
    dest_vec[Z] = ((double)pack_vec->z * (1.0 / 255.0)) * 2.0 - 1.0;

    dest_vec.normalize(); // already good to about 1%, but we can do better
}

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_RANDCOSWEIGHTED_H
