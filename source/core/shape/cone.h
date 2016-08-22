//******************************************************************************
///
/// @file core/shape/cone.h
///
/// Declarations related to the cone geometric primitive.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_CORE_CONE_H
#define POVRAY_CORE_CONE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/scene/object.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define CONE_OBJECT (BASIC_OBJECT)



/*****************************************************************************
* Global typedefs
******************************************************************************/

class Cone : public ObjectBase
{
        struct CONE_INT
        {
            DBL d;  /* Distance of intersection point               */
            int t;  /* Type of intersection: base/cap plane or side */
        };
    public:
        Vector3d apex;      ///< Center of the top of the cone.
        Vector3d base;      ///< Center of the bottom of the cone.
        DBL apex_radius;    ///< Radius of the cone at the top.
        DBL base_radius;    ///< Radius of the cone at the bottom.
        DBL dist;           ///< Distance to end of cone in canonical coords.

        Cone();
        virtual ~Cone();

        void Cylinder();

        virtual ObjectPtr Copy();

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
        virtual bool Inside(const Vector3d&, TraceThreadData *) const;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;
        // virtual void UVCoord(Vector2d&, const Intersection *, TraceThreadData *) const; // TODO FIXME - why is there no UV-mapping for this simple object?
        virtual void Translate(const Vector3d&, const TRANSFORM *);
        virtual void Rotate(const Vector3d&, const TRANSFORM *);
        virtual void Scale(const Vector3d&, const TRANSFORM *);
        virtual void Transform(const TRANSFORM *);
        virtual void Compute_BBox();

        void Compute_Cone_Data();
        void Compute_Cylinder_Data();
    protected:
        int Intersect(const BasicRay& ray, CONE_INT *Intersection, TraceThreadData *Thread) const;
};

}

#endif // POVRAY_CORE_CONE_H
