//******************************************************************************
///
/// @file core/math/randomsequence_fwd.h
///
/// Forward declarations related to ... what?
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

#ifndef POVRAY_CORE_RANDOMSEQUENCE_FWD_H
#define POVRAY_CORE_RANDOMSEQUENCE_FWD_H

/// @file
/// @note
///     This file should not pull in any headers whatsoever (except other
///     forward declaration headers or certain select standard headers).

// C++ standard header files
#include <memory>

// POV-Ray header files (core module)
#include "core/math/vector_fwd.h"

namespace pov
{

template<typename T> class SequentialNumberGenerator;
using SequentialIntGeneratorPtr         = std::shared_ptr<SequentialNumberGenerator<int>>;
using SequentialDoubleGeneratorPtr      = std::shared_ptr<SequentialNumberGenerator<double>>;
using SequentialVectorGeneratorPtr      = std::shared_ptr<SequentialNumberGenerator<Vector3d>>;
using SequentialVector2dGeneratorPtr    = std::shared_ptr<SequentialNumberGenerator<Vector2d>>;

template<typename T> class SeedableNumberGenerator;
using SeedableIntGeneratorPtr           = std::shared_ptr<SeedableNumberGenerator<int>>;
using SeedableDoubleGeneratorPtr        = std::shared_ptr<SeedableNumberGenerator<double>>;
using SeedableVectorGeneratorPtr        = std::shared_ptr<SeedableNumberGenerator<Vector3d>>;
using SeedableVector2dGeneratorPtr      = std::shared_ptr<SeedableNumberGenerator<Vector2d>>;

template<typename T> class IndexedNumberGenerator;
using IndexedIntGeneratorPtr            = std::shared_ptr<IndexedNumberGenerator<int> const>;
using IndexedDoubleGeneratorPtr         = std::shared_ptr<IndexedNumberGenerator<double> const>;
using IndexedVectorGeneratorPtr         = std::shared_ptr<IndexedNumberGenerator<Vector3d> const>;
using IndexedVector2dGeneratorPtr       = std::shared_ptr<IndexedNumberGenerator<Vector2d> const>;

}
// end of namespace pov

#endif // POVRAY_CORE_RANDOMSEQUENCE_FWD_H
