//******************************************************************************
///
/// @file core/shape/quadric.h
///
/// Declarations related to the quadric geometric primitive.
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

#ifndef POVRAY_CORE_QUADRIC_H
#define POVRAY_CORE_QUADRIC_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/math/matrix.h"
#include "core/scene/object.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCoreShape
///
/// @{

class Plane;

//******************************************************************************
///
/// @name Object Types
///
/// @{

#define QUADRIC_OBJECT (BASIC_OBJECT)

/// @}
///
//******************************************************************************

class Quadric final : public ObjectBase
{
    public:
        Vector3d Square_Terms;
        Vector3d Mixed_Terms;
        Vector3d Terms;
        DBL Constant;
        bool Automatic_Bounds;

        Quadric();
        virtual ~Quadric() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual ObjectPtr Invert() override;
        virtual void Compute_BBox() override;

        static void Compute_Plane_Min_Max(const Plane *plane, Vector3d& Min, Vector3d& Max);
        void Compute_BBox(Vector3d& ClipMin, Vector3d& ClipMax);
    protected:
        bool Intersect(const BasicRay& ray, DBL *Depth1, DBL *Depth2) const;
        void Quadric_To_Matrix(MATRIX Matrix) const;
        void Matrix_To_Quadric(const MATRIX Matrix);
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_QUADRIC_H
