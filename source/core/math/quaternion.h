//******************************************************************************
///
/// @file core/math/quaternion.h
///
/// Declarations related to Quaternion algebra julia fractals.
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

#ifndef POVRAY_CORE_QUATERNION_H
#define POVRAY_CORE_QUATERNION_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/coretypes.h"
#include "core/math/vector.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreMathQuaternion Quaternions
/// @ingroup PovCoreMath
///
/// @{

class Fractal;

/*****************************************************************************
* Global functions
******************************************************************************/

class QuaternionFractalRules : public FractalRules
{
    public:
        virtual ~QuaternionFractalRules() override {}
        virtual bool Bound (const BasicRay&, const Fractal *, DBL *, DBL *) const override;
};

class JuliaFractalRules final : public QuaternionFractalRules
{
    public:
        virtual ~JuliaFractalRules() override {}
        virtual void CalcNormal (Vector3d&, int, const Fractal *, DBL **) const override;
        virtual bool Iterate (const Vector3d&, const Fractal *, DBL **) const override;
        virtual bool Iterate (const Vector3d&, const Fractal *, const Vector3d&, DBL *, DBL **) const override;
};

class Z3FractalRules final : public QuaternionFractalRules
{
    public:
        virtual ~Z3FractalRules() override {}
        virtual void CalcNormal (Vector3d&, int, const Fractal *, DBL **) const override;
        virtual bool Iterate (const Vector3d&, const Fractal *, DBL **) const override;
        virtual bool Iterate (const Vector3d&, const Fractal *, const Vector3d&, DBL *, DBL **) const override;
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_QUATERNION_H
