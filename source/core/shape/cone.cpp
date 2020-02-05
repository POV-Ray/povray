//******************************************************************************
///
/// @file core/shape/cone.cpp
///
/// Implementation of the cone geometric primitive.
///
/// @author Alexander Enzmann
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
#include "core/shape/cone.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"

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

const DBL Cone_Tolerance = 1.0e-9;

#define close(x, y) (fabs(x-y) < EPSILON ? 1 : 0)

/* Part of the cone/cylinder hit. [DB 9/94] */

const int BASE_HIT = 1;
const int CAP_HIT  = 2;
const int SIDE_HIT = 3;



/*****************************************************************************
*
* FUNCTION
*
*   All_Cone_Intersections
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

bool Cone::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int Intersection_Found, cnt, i;
    Vector3d IPoint;
    CONE_INT I[4];

    Intersection_Found = false;

    if ((cnt = Intersect(ray, I, Thread->Stats())) != 0)
    {
        for (i = 0; i < cnt; i++)
        {
            IPoint = ray.Evaluate(I[i].d);

            if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
            {
                Depth_Stack->push(Intersection(I[i].d,IPoint,this,I[i].t));
                Intersection_Found = true;
            }
        }
    }

    return (Intersection_Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_cone
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

int Cone::Intersect(const BasicRay& ray, CONE_INT *Intersection, RenderStatistics& stats) const
{
    int i = 0;
    DBL a, b, c, z, t1, t2, len;
    DBL d;
    Vector3d P, D;

    stats[Ray_Cone_Tests]++;

    /* Transform the ray into the cones space */

    MInvTransPoint(P, ray.Origin, Trans);
    MInvTransDirection(D, ray.Direction, Trans);

    len = D.length();
    D /= len;

    if (Test_Flag(this, CYLINDER_FLAG))
    {
        /* Solve intersections with a cylinder */

        a = D[X] * D[X] + D[Y] * D[Y];

        if (a > EPSILON)
        {
            b = P[X] * D[X] + P[Y] * D[Y];

            c = P[X] * P[X] + P[Y] * P[Y] - 1.0;

            d = b * b - a * c;

            if (d >= 0.0)
            {
                d = sqrt(d);

                t1 = (-b + d) / a;
                t2 = (-b - d) / a;

                z = P[Z] + t1 * D[Z];

                if ((t1 > Cone_Tolerance) && (t1 < MAX_DISTANCE) && (z >= 0.0) && (z <= 1.0))
                {
                    Intersection[i].d   = t1 / len;
                    Intersection[i++].t = SIDE_HIT;
                }

                z = P[Z] + t2 * D[Z];

                if ((t2 > Cone_Tolerance) && (t2 < MAX_DISTANCE) && (z >= 0.0) && (z <= 1.0))
                {
                    Intersection[i].d   = t2 / len;
                    Intersection[i++].t = SIDE_HIT;
                }
            }
        }
    }
    else
    {
        /* Solve intersections with a cone */

        a = D[X] * D[X] + D[Y] * D[Y] - D[Z] * D[Z];

        b = D[X] * P[X] + D[Y] * P[Y] - D[Z] * P[Z];

        c = P[X] * P[X] + P[Y] * P[Y] - P[Z] * P[Z];

        if (fabs(a) < EPSILON)
        {
            if (fabs(b) > EPSILON)
            {
                /* One intersection */

                t1 = -0.5 * c / b;

                z = P[Z] + t1 * D[Z];

                if ((t1 > Cone_Tolerance) && (t1 < MAX_DISTANCE) && (z >= dist) && (z <= 1.0))
                {
                    Intersection[i].d   = t1 / len;
                    Intersection[i++].t = SIDE_HIT;
                }
            }
        }
        else
        {
            /* Check hits against the side of the cone */

            d = b * b - a * c;

            if (d >= 0.0)
            {
                d = sqrt(d);

                t1 = (-b - d) / a;
                t2 = (-b + d) / a;

                z = P[Z] + t1 * D[Z];

                if ((t1 > Cone_Tolerance) && (t1 < MAX_DISTANCE) && (z >= dist) && (z <= 1.0))
                {
                    Intersection[i].d   = t1 / len;
                    Intersection[i++].t = SIDE_HIT;
                }

                z = P[Z] + t2 * D[Z];

                if ((t2 > Cone_Tolerance) && (t2 < MAX_DISTANCE) && (z >= dist) && (z <= 1.0))
                {
                    Intersection[i].d   = t2 / len;
                    Intersection[i++].t = SIDE_HIT;
                }
            }
        }
    }

    if (Test_Flag(this, CLOSED_FLAG) && (fabs(D[Z]) > EPSILON))
    {
        d = (1.0 - P[Z]) / D[Z];

        a = (P[X] + d * D[X]);

        b = (P[Y] + d * D[Y]);

        if (((Sqr(a) + Sqr(b)) <= 1.0) && (d > Cone_Tolerance) && (d < MAX_DISTANCE))
        {
            Intersection[i].d   = d / len;
            Intersection[i++].t = CAP_HIT;
        }

        d = (dist - P[Z]) / D[Z];

        a = (P[X] + d * D[X]);

        b = (P[Y] + d * D[Y]);

        if ((Sqr(a) + Sqr(b)) <= (Test_Flag(this, CYLINDER_FLAG) ? 1.0 : Sqr(dist))
            && (d > Cone_Tolerance) && (d < MAX_DISTANCE))
        {
            Intersection[i].d   = d / len;
            Intersection[i++].t = BASE_HIT;
        }
    }

    if (i)
        stats[Ray_Cone_Tests_Succeeded]++;

    return (i);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Cone
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

bool Cone::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    DBL w2, z2, offset = (Test_Flag(this, CLOSED_FLAG) ? -EPSILON : EPSILON);
    Vector3d New_Point;

    /* Transform the point into the cones space */

    MInvTransPoint(New_Point, IPoint, Trans);

    /* Test to see if we are inside the cone */

    w2 = New_Point[X] * New_Point[X] + New_Point[Y] * New_Point[Y];

    if (Test_Flag(this, CYLINDER_FLAG))
    {
        /* Check to see if we are inside a cylinder */

        if ((w2 > 1.0 + offset) ||
            (New_Point[Z] < 0.0 - offset) ||
            (New_Point[Z] > 1.0 + offset))
        {
            return (Test_Flag(this, INVERTED_FLAG));
        }
        else
        {
            return (!Test_Flag(this, INVERTED_FLAG));
        }
    }
    else
    {
        /* Check to see if we are inside a cone */

        z2 = New_Point[Z] * New_Point[Z];

        if ((w2 > z2 + offset) ||
            (New_Point[Z] < dist - offset) ||
            (New_Point[Z] > 1.0+offset))
        {
            return (Test_Flag(this, INVERTED_FLAG));
        }
        else
        {
            return (!Test_Flag(this, INVERTED_FLAG));
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Cone_Normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

void Cone::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    /* Transform the point into the cones space */

    MInvTransPoint(Result, Inter->IPoint, Trans);

    /* Calculating the normal is real simple in canonical cone space */

    switch (Inter->i1)
    {
        case SIDE_HIT:

            if (Test_Flag(this, CYLINDER_FLAG))
            {
                Result[Z] = 0.0;
            }
            else
            {
                Result[Z] = -Result[Z];
            }

            break;

        case BASE_HIT:

            Result = Vector3d(0.0, 0.0, -1.0);

            break;

        case CAP_HIT:

            Result = Vector3d(0.0, 0.0, 1.0);

            break;
    }

    /* Transform the point out of the cones space */

    MTransNormal(Result, Result, Trans);

    Result.normalize();
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Cone
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

void Cone::Translate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Cone
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

void Cone::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Cone
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

void Cone::Scale(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Cone
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

void Cone::Transform(const TRANSFORM *tr)
{
    Compose_Transforms(Trans, tr);

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Cone
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

Cone::Cone() : ObjectBase(CONE_OBJECT)
{
    apex = Vector3d(0.0, 0.0, 1.0);
    base = Vector3d(0.0, 0.0, 0.0);

    apex_radius = 1.0;
    base_radius = 0.0;

    dist = 0.0;

    Trans = Create_Transform();

    /* Cone/Cylinder has capped ends by default. */

    Set_Flag(this, CLOSED_FLAG);

    /* Default bounds */

    Make_BBox(BBox, -1.0, -1.0, 0.0, 2.0, 2.0, 1.0);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Cone
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

ObjectPtr Cone::Copy()
{
    Cone *New = new Cone();
    Destroy_Transform(New->Trans);
    *New = *this;
    New->Trans = Copy_Transform(Trans);
    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Cylinder
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

void Cone::Cylinder()
{
    apex_radius = 1.0;
    base_radius = 1.0;

    Set_Flag(this, CYLINDER_FLAG); // This is a cylinder.
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Cone_Data
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Feb 1996: check for equal sized ends (cylinder) first [AED]
*
******************************************************************************/

void Cone::Compute_Cone_Data()
{
    DBL tlen, len, tmpf;
    Vector3d tmpv, axis, origin;

    /* Process the primitive specific information */

    /* Find the axis and axis length */

    axis = apex - base;

    len = axis.length();

    if (len < EPSILON)
    {
        throw POV_EXCEPTION_STRING("Degenerate cone/cylinder."); // TODO FIXME - should a possible error
    }
    else
    {
        axis /= len;
    }
    /* we need to trap that case first */
    if (fabs(apex_radius - base_radius) < EPSILON)
    {
        /* What we are dealing with here is really a cylinder */

        Set_Flag(this, CYLINDER_FLAG);

        Compute_Cylinder_Data();

        return;
    }

    if (apex_radius < base_radius)
    {
        /* Want the bigger end at the top */

        tmpv = base;
        base = apex;
        apex = tmpv;

        tmpf = base_radius;
        base_radius = apex_radius;
        apex_radius = tmpf;
        axis.invert();
    }
    /* apex & base are different, yet, it might looks like a cylinder */
    tmpf = base_radius * len / (apex_radius - base_radius);

    origin = base - axis * tmpf;

    tlen = tmpf + len;
    /* apex is always bigger here */
    if (((apex_radius - base_radius)*len/tlen) < EPSILON)
    {
        /* What we are dealing with here is really a cylinder */

        Set_Flag(this, CYLINDER_FLAG);

        Compute_Cylinder_Data();

        return;
    }

    dist = tmpf / tlen;
    /* Determine alignment */
    Compute_Coordinate_Transform(Trans, origin, axis, apex_radius, tlen);

    /* Recalculate the bounds */

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Cylinder_Data
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

void Cone::Compute_Cylinder_Data()
{
    DBL tmpf;
    Vector3d axis;

    axis = apex - base;

    tmpf = axis.length();

    if (tmpf < EPSILON)
    {
        throw POV_EXCEPTION_STRING("Degenerate cylinder, base point = apex point."); // TODO FIXME - should a possible error
    }
    else
    {
        axis /= tmpf;

        Compute_Coordinate_Transform(Trans, base, axis, apex_radius, tmpf);
    }

    dist = 0.0;

    /* Recalculate the bounds */

    Compute_BBox();
}




/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Cone
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

Cone::~Cone()
{}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Cone_BBox
*
* INPUT
*
*   Cone - Cone/Cylinder
*
* OUTPUT
*
*   Cone
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a cone or cylinder.
*
* CHANGES
*
*   Aug 1994 : Creation.
*       2000 : cone  bounding fix
*
******************************************************************************/

void Cone::Compute_BBox()
{
    Make_BBox(BBox, -1.0, -1.0, dist, 2.0, 2.0, 1.0-dist);

    Recompute_BBox(&BBox, Trans);
}



#ifdef POV_ENABLE_CONE_UV

/*****************************************************************************
*
* FUNCTION
*
*   Cone_UVCoord
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
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

void Cone::UVCoord(Vector2d& Result, const Intersection *Inter) const
{
    CalcUV(Inter->IPoint, Result);
}


/*****************************************************************************
*
* FUNCTION
*
*   CalcUV
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
*   Calculate the u/v coordinate of a point on an cone/cylinder (inspired by lemon)
*
* CHANGES
*
*   -
*
******************************************************************************/

void Cone::CalcUV(const Vector3d& IPoint, Vector2d& Result) const
{
    DBL len, x, y;
    DBL phi, theta;
    Vector3d P;

    // Transform the ray into the cone space.
    MInvTransPoint(P, IPoint, Trans);

    // the center of UV coordinate is the <0,0> point
    x = P[X];
    y = P[Y];

    // Determine its angle from the point (1, 0, 0) in the x-y plane.
    len = x * x + y * y;

    if ((P[Z]>(dist+EPSILON))&&(P[Z]<(1.0-EPSILON)))
    {
        // when not on a face, the range 0.25 to 0.75 is used (just plain magic 25% for face, no other reason, but it makes C-Lipka happy)
        phi = 0.75-0.5*(P[Z]-dist)/(1.0-dist);
    }
    else if (P[Z]>(dist+EPSILON))
    {
        // the radii are changed (apex_radius is 1.0 for len)
        // aka P[Z] is 1, use the apex_radius, from 75% to 100% (at the very center)
        phi = (sqrt(len)/4.0);
    }
    else
    {
        // aka P[Z] is dist, use the base_radius, from 0% (at the very center) to 25%
        phi = 1.0;
        if (base_radius)
        {
            phi = 1.0-(sqrt(len)*apex_radius/(base_radius*4.0));
        }
    }


    if (len > EPSILON)
    {
        len = sqrt(len);
        if (y == 0.0)
        {
            if (x > 0)
                theta = 0.0;
            else
                theta = M_PI;
        }
        else
        {
            theta = acos(x / len);
            if (y < 0.0)
                theta = TWO_M_PI - theta;
        }

        theta /= TWO_M_PI; // This will be from 0 to 1
    }
    else
        // This point is at one of the poles. Any value of xcoord will be ok...
        theta = 0;

    Result[U] = theta;
    Result[V] = phi;

}

#endif // POV_ENABLE_CONE_UV

}
// end of namespace pov
