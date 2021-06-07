//******************************************************************************
///
/// @file core/shape/sphere.cpp
///
/// Implementation of the sphere geometric primitive.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/shape/sphere.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/bounding/boundingbox.h"
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

const DBL DEPTH_TOLERANCE = 1.0e-6;


/*****************************************************************************
*
* FUNCTION
*
*   All_Sphere_Intersection
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Sphere::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    Thread->Stats()[Ray_Sphere_Tests]++;

    if(Do_Ellipsoid)
    {
        bool Intersection_Found;
        DBL Depth1, Depth2, len;
        Vector3d IPoint;
        BasicRay New_Ray;

        // Transform the ray into the ellipsoid's space

        MInvTransRay(New_Ray, ray, Trans);

        len = New_Ray.Direction.length();
        New_Ray.Direction /= len;

        Intersection_Found = false;

        if(Intersect(New_Ray, Vector3d(0.0), 1.0, &Depth1, &Depth2))
        {
            Thread->Stats()[Ray_Sphere_Tests_Succeeded]++;
            if((Depth1 > DEPTH_TOLERANCE) && (Depth1 < MAX_DISTANCE))
            {
                IPoint = New_Ray.Evaluate(Depth1);
                MTransPoint(IPoint, IPoint, Trans);

                if(Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                {
                    Depth_Stack->push(Intersection(Depth1 / len, IPoint, this));
                    Intersection_Found = true;
                }
            }

            if((Depth2 > DEPTH_TOLERANCE) && (Depth2 < MAX_DISTANCE))
            {
                IPoint = New_Ray.Evaluate(Depth2);
                MTransPoint(IPoint, IPoint, Trans);

                if(Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                {
                    Depth_Stack->push(Intersection(Depth2 / len, IPoint, this));
                    Intersection_Found = true;
                }
            }
        }

        return(Intersection_Found);
    }
    else
    {
        bool Intersection_Found;
        DBL Depth1, Depth2;
        Vector3d IPoint;

        Intersection_Found = false;

        if(Intersect(ray, Center, Sqr(Radius), &Depth1, &Depth2))
        {
            Thread->Stats()[Ray_Sphere_Tests_Succeeded]++;
            if((Depth1 > DEPTH_TOLERANCE) && (Depth1 < MAX_DISTANCE))
            {
                IPoint = ray.Evaluate(Depth1);

                if(Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                {
                    Depth_Stack->push(Intersection(Depth1, IPoint, this));
                    Intersection_Found = true;
                }
            }

            if((Depth2 > DEPTH_TOLERANCE) && (Depth2 < MAX_DISTANCE))
            {
                IPoint = ray.Evaluate(Depth2);

                if(Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                {
                    Depth_Stack->push(Intersection(Depth2, IPoint, this));
                    Intersection_Found = true;
                }
            }
        }

        return(Intersection_Found);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Sphere
*
* INPUT
*
*   Ray     - Ray to test intersection with
*   Center  - Center of the sphere
*   Radius2 - Squared radius of the sphere
*   Depth1  - Lower intersection distance
*   Depth2  - Upper intersection distance
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Sphere::Intersect(const BasicRay& ray, const Vector3d& Center, DBL Radius2, DBL *Depth1, DBL *Depth2)
{
    DBL OCSquared, t_Closest_Approach, Half_Chord, t_Half_Chord_Squared;
    Vector3d Origin_To_Center;

    Origin_To_Center = Center - ray.Origin;

    OCSquared = Origin_To_Center.lengthSqr();

    t_Closest_Approach = dot(Origin_To_Center, ray.Direction);

    if ((OCSquared >= Radius2) && (t_Closest_Approach < EPSILON))
        return(false);

    t_Half_Chord_Squared = Radius2 - OCSquared + Sqr(t_Closest_Approach);

    if (t_Half_Chord_Squared > EPSILON)
    {
        Half_Chord = sqrt(t_Half_Chord_Squared);

        *Depth1 = t_Closest_Approach - Half_Chord;
        *Depth2 = t_Closest_Approach + Half_Chord;

        return(true);
    }

    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Sphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Sphere::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    DBL OCSquared;
    Vector3d Origin_To_Center;

    if(Do_Ellipsoid)
    {
        DBL OCSquared;
        Vector3d New_Point;

        /* Transform the point into the sphere's space */

        MInvTransPoint(New_Point, IPoint, Trans);

        OCSquared = New_Point.lengthSqr();

        if (Test_Flag(this, INVERTED_FLAG))
            return(OCSquared > Sqr(Radius));
        else
            return(OCSquared < Sqr(Radius));
    }
    else
    {
        Origin_To_Center = Center - IPoint;

        OCSquared = Origin_To_Center.lengthSqr();

        if(Test_Flag(this, INVERTED_FLAG))
            return(OCSquared > Sqr(Radius));
        else
            return(OCSquared < Sqr(Radius));
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Sphere_Normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Sphere::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    if(Do_Ellipsoid)
    {
        Vector3d New_Point;

        // Transform the point into the sphere's space
        MInvTransPoint(New_Point, Inter->IPoint, Trans);

        // Compute the result in the sphere's space
        // (this is trivial since ellipsoidal mode is based on the unity sphere)
        Result = New_Point;

        // Transform the result back into regular space
        MTransNormal(Result, Result, Trans);
        Result.normalize();
    }
    else
    {
        Result = (Inter->IPoint - Center) / Radius;
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Shere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

ObjectPtr Sphere::Copy()
{
    Sphere *New = new Sphere();
    Destroy_Transform(New->Trans);
    *New = *this;
    New->Trans = Copy_Transform(Trans);

    return(New);
}


/*****************************************************************************
*
* FUNCTION
*
*   Translate_Sphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Sphere::Translate(const Vector3d& Vector, const TRANSFORM *tr)
{
    if (Do_Ellipsoid)
    {
        Transform(tr);
    }
    else
    {
        Center += Vector;
        Compute_BBox();
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Sphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Sphere::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    if (Do_Ellipsoid)
    {
        Transform(tr);
    }
    else
    {
        if (Trans == nullptr)
            Trans = Create_Transform();
        Compose_Transforms(Trans, tr);

        MTransPoint(Center, Center, tr);
        Compute_BBox();
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Sphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Sphere::Scale(const Vector3d& Vector, const TRANSFORM *tr)
{
    if (Do_Ellipsoid || (Vector[X] != Vector[Y]) || (Vector[X] != Vector[Z]))
    {
        Transform(tr);
    }
    else
    {
        Center *= Vector[X];
        Radius *= fabs(Vector[X]);
    }

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Sphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

Sphere::Sphere() :
    ObjectBase(SPHERE_OBJECT),
    Center(0.0, 0.0, 0.0),
    Radius(1.0),
    Do_Ellipsoid(false)
{}

/*****************************************************************************
*
* FUNCTION
*
*   Transform_Sphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Sphere::Transform(const TRANSFORM *tr)
{
    // Arbitrary transformations can only be tracked in ellipsoidal mode,
    // so we need to switch to that mode now if we're still in spherical mode.
    if (!Do_Ellipsoid)
    {
        Do_Ellipsoid = true;

        // We always have a transformation in ellipsoidal mode.
        if (!Trans)
            Trans = Create_Transform();

        // Ellipsoidal mode is based on the unity sphere, so we need to convert
        // center and radius into corresponding transformations.

        TRANSFORM temp;

        Compute_Scaling_Transform(&temp, Vector3d(Radius));
        Compose_Transforms(Trans, &temp);
        Radius = 1.0; // not really necessary, but avoids confusion when debugging

        Compute_Translation_Transform(&temp, Center);
        Compose_Transforms(Trans, &temp);
        Center = Vector3d(0.0); // not really necessary, but avoids confusion when debugging
    }

    Compose_Transforms(Trans, tr);

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Sphere_BBox
*
* INPUT
*
*   Sphere - Sphere
*
* OUTPUT
*
*   Sphere
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a sphere.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Sphere::Compute_BBox()
{
    if(Do_Ellipsoid)
    {
        // Ellipsoidal mode is based on the unity sphere.
        Make_BBox(BBox, -1.0,-1.0,-1.0, 2.0,2.0,2.0);
        Recompute_BBox(&BBox, Trans);
    }
    else
    {
        Make_BBox(BBox, Center[X] - Radius, Center[Y] - Radius,  Center[Z] - Radius, 2.0 * Radius, 2.0 * Radius, 2.0 * Radius);
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Sphere_UVCoord
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Nathan Kopp   (adapted from spherical_image_map)
*
* DESCRIPTION
*
*   Find the UV coordinates of a sphere.
*   Map a point (x, y, z) on a sphere of radius 1 to a 2-d image. (Or is it the
*   other way around?)
*
* CHANGES
*
*   -
*
******************************************************************************/

void Sphere::UVCoord(Vector2d& Result, const Intersection *Inter) const
{
    DBL len, phi, theta;
    DBL x,y,z;
    Vector3d New_Point, New_Center;

    /* Transform the point into the sphere's space */
    if (Do_Ellipsoid)
    {
        MInvTransPoint(New_Point, Inter->IPoint, Trans);
    }
    else
    {
        New_Point = Inter->IPoint - Center;
        if (Trans != nullptr)
            MInvTransPoint(New_Point, New_Point, Trans);
    }
    x = New_Point[X];
    y = New_Point[Y];
    z = New_Point[Z];

    len = sqrt(x * x + y * y + z * z);

    if (len == 0.0)
        return;
    else
    {
        x /= len;
        y /= len;
        z /= len;
    }

    /* Determine its angle from the x-z plane. */
    phi = 0.5 + asin(y) / M_PI; /* This will be from 0 to 1 */

    /* Determine its angle from the point (1, 0, 0) in the x-z plane. */
    len = x * x + z * z;

    if (len > EPSILON)
    {
        len = sqrt(len);
        if (z == 0.0)
        {
            if (x > 0)
                theta = 0.0;
            else
                theta = M_PI;
        }
        else
        {
            theta = acos(x / len);
            if (z < 0.0)
                theta = TWO_M_PI - theta;
        }

        theta /= TWO_M_PI;  /* This will be from 0 to 1 */
    }
    else
        /* This point is at one of the poles. Any value of xcoord will be ok... */
        theta = 0;

    Result[U] = theta;
    Result[V] = phi;
}

bool Sphere::Intersect_BBox(BBoxDirection, const BBoxVector3d&, const BBoxVector3d&, BBoxScalar) const
{
    return true;
}

}
// end of namespace pov
