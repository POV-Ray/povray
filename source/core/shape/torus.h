//******************************************************************************
///
/// @file core/shape/torus.h
///
/// Declarations related to the torus geometric primitive.
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

#ifndef POVRAY_CORE_TORUS_H
#define POVRAY_CORE_TORUS_H

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

#define TORUS_OBJECT (STURM_OK_OBJECT)

/* Generate additional torus statistics. */

#define TORUS_EXTRA_STATS 1

/// @}
///
//******************************************************************************

class Torus : public ObjectBase
{
    public:
        DBL MajorRadius, MinorRadius;

        Torus();
        virtual ~Torus() override;

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
    protected:
        int Intersect(const BasicRay& ray, DBL *Depth, RenderStatistics& stats) const;
        bool Test_Thick_Cylinder(const Vector3d& P, const Vector3d& D, DBL h1, DBL h2, DBL r1, DBL r2) const;
        void CalcUV(const Vector3d& IPoint, Vector2d& Result) const;
};

/// @todo This class may need its own UVCoord() function.
class SpindleTorus final : public Torus
{
    protected:

        static const int SpindleVisible                 = 0x0001;
        static const int NonSpindleVisible              = 0x0002;
        static const int SpindleInside                  = 0x0004;
        static const int NonSpindleInside               = 0x0008;
        static const int SpindleRelevantForIntersection = 0x0010;
        static const int SpindleRelevantForInside       = 0x0020;

        static const int SpindleOnlyVisible             = SpindleRelevantForIntersection | SpindleVisible;
        static const int NonSpindleOnlyVisible          = SpindleRelevantForIntersection | NonSpindleVisible;
        static const int AllVisible                     = SpindleVisible | NonSpindleVisible;

        static const int SpindleOnlyInside              = SpindleRelevantForInside | SpindleInside;
        static const int NonSpindleOnlyInside           = SpindleRelevantForInside | NonSpindleInside;
        static const int AllInside                      = SpindleInside | NonSpindleInside;

    public:

        enum SpindleMode
        {
            UnionSpindle        = AllVisible            | AllInside,
            MergeSpindle        = NonSpindleOnlyVisible | AllInside,
            IntersectionSpindle = SpindleOnlyVisible    | SpindleOnlyInside,
            DifferenceSpindle   = AllVisible            | NonSpindleOnlyInside,
        };

        SpindleMode mSpindleMode;

        SpindleTorus();
        virtual ~SpindleTorus() override;

        virtual ObjectPtr Copy() override;

        virtual bool Precompute() override;
        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void Compute_BBox() override;

    protected:

        DBL mSpindleTipYSqr;
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_TORUS_H
