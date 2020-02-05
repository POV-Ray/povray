//******************************************************************************
///
/// @file core/shape/sphere.h
///
/// Declarations related to the sphere geometric primitive.
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

#ifndef POVRAY_CORE_SPHERE_H
#define POVRAY_CORE_SPHERE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/scene/object.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCoreShape
///
/// @{

//******************************************************************************
///
/// @name Object Types
///
/// @{

#define SPHERE_OBJECT (BASIC_OBJECT)

/// @}
///
//******************************************************************************

class Sphere final : public ObjectBase
{
    public:
        Vector3d Center;
        DBL Radius;

        Sphere();

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void UVCoord(Vector2d&, const Intersection *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;
        virtual bool Intersect_BBox(BBoxDirection, const BBoxVector3d&, const BBoxVector3d&, BBoxScalar) const override;

        static bool Intersect(const BasicRay& ray, const Vector3d& Center, DBL Radius2, DBL *Depth1, DBL  *Depth2);

    private:

        /// Ellipsoid mode flag.
        ///
        /// A value of `false` indicates that the primitive is in _spherical_ mode, while a value of `true` indicates
        /// _ellipsoidal_ mode.
        ///
        /// **Spherical mode** uses the @ref Center and @ref Radius members to track all transformations to the
        /// primitive, while the @ref Trans member only tracks rotations and is used exclusively for UV mapping.
        /// If a transformation cannot be tracked in this manner, the primitive is switched to _ellipsoidal mode_.
        ///
        /// **Ellipsoidal mode** uses the @ref Trans member to track center, radius and all transformations, while the
        /// @ref Center is pegged to the coordinate origin and the @radius to unity.
        ///
        bool Do_Ellipsoid;
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_SPHERE_H
