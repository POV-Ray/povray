//******************************************************************************
///
/// @file core/math/vector_fwd.h
///
/// Forward declarations related to vector arithmetics.
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

#ifndef POVRAY_CORE_VECTOR_FWD_H
#define POVRAY_CORE_VECTOR_FWD_H

/// @file
/// @note
///     This file should not pull in any headers whatsoever (except other
///     forward declaration headers or certain select standard headers).

namespace pov
{

template<typename T> class GenericVector2d;
template<typename T> class GenericVector3d;

using Vector2d      = GenericVector2d<double>;  ///< Double-precision 2D vector.
using SnglVector2d  = GenericVector2d<float>;   ///< Single-precision 2D vector.

using Vector3d      = GenericVector3d<double>;  ///< Double-precision 3D vector.
using SnglVector3d  = GenericVector3d<float>;   ///< Single-precision 3D vector.
using IntVector3d   = GenericVector3d<int>;     ///< Integer 3D vector.

}
// end of namespace pov

#endif // POVRAY_CORE_VECTOR_FWD_H
