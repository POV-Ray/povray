//******************************************************************************
///
/// @file core/support/octree_fwd.h
///
/// Forward declarations related to the radiosity sample octree.
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

#ifndef POVRAY_CORE_OCTREE_FWD_H
#define POVRAY_CORE_OCTREE_FWD_H

/// @file
/// @note
///     This file should not pull in any headers whatsoever (except other
///     forward declaration headers or certain select standard headers).

// C++ standard header files
#include <limits>

namespace pov
{

using OctreePass = unsigned short;
constexpr auto kOctreePassInvalid  = 0;
constexpr auto kOctreePassFirst    = 1;
constexpr auto kOctreePassFinal    = std::numeric_limits<OctreePass>::max();
constexpr auto kOctreePassReserved = kOctreePassFinal    - 1;
constexpr auto kOctreePassMax      = kOctreePassReserved - 1;

using OctreeDepth = unsigned char;
constexpr auto kOctreeDepthMax     = std::numeric_limits<OctreeDepth>::max();

}
// end of namespace pov

#endif // POVRAY_CORE_OCTREE_FWD_H
