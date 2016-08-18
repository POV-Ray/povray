//******************************************************************************
///
/// @file core/shape/triangle.h
///
/// Declarations related to the triangle geometric primitive.
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

#ifndef POVRAY_CORE_TRIANGLE_H
#define POVRAY_CORE_TRIANGLE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/scene/object.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define TRIANGLE_OBJECT        (PATCH_OBJECT)
#define SMOOTH_TRIANGLE_OBJECT (PATCH_OBJECT)
/* NK 1998 double_illuminate - removed +DOUBLE_ILLUMINATE from smooth_triangle */


/*****************************************************************************
* Global typedefs
******************************************************************************/

class Triangle : public NonsolidObject
{
    public:
        Vector3d        P1, P2, P3;
        Vector3d        Normal_Vector;
        DBL             Distance;
        unsigned int    Dominant_Axis:2;
        unsigned int    vAxis:2;  /* used only for smooth triangles */
        bool            mPointOrderSwapped:1; ///< Whether ordering of points had been swapped

        Triangle();
        Triangle(int t);
        virtual ~Triangle();

        virtual ObjectPtr Copy();

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
        virtual bool Inside(const Vector3d&, TraceThreadData *) const;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;
        // virtual void UVCoord(Vector2d&, const Intersection *, TraceThreadData *) const; // TODO FIXME - why is there no UV-mapping for this trivial object? [trf]
        virtual void Translate(const Vector3d&, const TRANSFORM *);
        virtual void Rotate(const Vector3d&, const TRANSFORM *);
        virtual void Scale(const Vector3d&, const TRANSFORM *);
        virtual void Transform(const TRANSFORM *);
        virtual void Compute_BBox();
        virtual bool Intersect_BBox(BBoxDirection, const BBoxVector3d&, const BBoxVector3d&, BBoxScalar) const;

        virtual bool Compute_Triangle();
    protected:
        bool Intersect(const BasicRay& ray, DBL *Depth) const;
        void find_triangle_dominant_axis();
};

class SmoothTriangle : public Triangle
{
    public:
        Vector3d  N1, N2, N3, Perp;

        SmoothTriangle();

        virtual ObjectPtr Copy();

        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;
        virtual void Translate(const Vector3d&, const TRANSFORM *);
        virtual void Rotate(const Vector3d&, const TRANSFORM *);
        virtual void Scale(const Vector3d&, const TRANSFORM *);
        virtual void Transform(const TRANSFORM *);

        virtual bool Compute_Triangle();

        static DBL Calculate_Smooth_T(const Vector3d& IPoint, const Vector3d& P1, const Vector3d& P2, const Vector3d& P3);
    protected:
        bool Compute_Smooth_Triangle();
};

}

#endif // POVRAY_CORE_TRIANGLE_H
