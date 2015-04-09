//******************************************************************************
///
/// @file core/shape/torus.h
///
/// This module contains all defines, typedefs, and prototypes for
/// @ref torus.cpp.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef TORUS_H
#define TORUS_H

#include "backend/scene/objects.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define TORUS_OBJECT (STURM_OK_OBJECT)

/* Generate additional torus statistics. */

#define TORUS_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

/*
 * Torus structure.
 *
 *   R : Major radius
 *   r : Minor radius
 */

class Torus : public ObjectBase
{
    public:
        DBL MajorRadius, MinorRadius;

        Torus();
        virtual ~Torus();

        virtual ObjectPtr Copy();

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
        virtual bool Inside(const Vector3d&, TraceThreadData *) const;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;
        virtual void UVCoord(Vector2d&, const Intersection *, TraceThreadData *) const;
        virtual void Translate(const Vector3d&, const TRANSFORM *);
        virtual void Rotate(const Vector3d&, const TRANSFORM *);
        virtual void Scale(const Vector3d&, const TRANSFORM *);
        virtual void Transform(const TRANSFORM *);
        virtual void Compute_BBox();
    protected:
        int Intersect(const BasicRay& ray, DBL *Depth, TraceThreadData *Thread) const;
        bool Test_Thick_Cylinder(const Vector3d& P, const Vector3d& D, DBL h1, DBL h2, DBL r1, DBL r2) const;
        void CalcUV(const Vector3d& IPoint, Vector2d& Result) const;
};

// @todo This class may need its own UVCoord() function.
class SpindleTorus : public Torus
{
    public:
        SpindleTorus();
        virtual ~SpindleTorus();

        virtual ObjectPtr Copy();

        virtual bool Precompute();
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;

    protected:
        DBL mSpindleTipDistSqr;
};

}

#endif
