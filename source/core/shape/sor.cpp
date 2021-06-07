//******************************************************************************
///
/// @file core/shape/sor.cpp
///
/// Implementation of the surface of revolution (sor) geometric primitive.
///
/// @author Dieter Bayer
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

/****************************************************************************
*
*  Explanation:
*
*    The surface of revolution primitive is defined by a set of points
*    in 2d space wich are interpolated by cubic splines. The resulting
*    2d function is rotated about an axis to form the final object.
*
*    All calculations are done in the object's (u,v,w)-coordinate system
*    with the (w)-axis being the rotation axis.
*
*    One spline segment in the (r,w)-plane is given by the equation
*
*      r^2 = f(w) = A * w * w * w + B * w * w + C * w + D.
*
*    To intersect a ray R = P + k * D transformed into the object's
*    coordinate system with the surface of revolution, the equation
*
*      (Pu + k * Du)^2 + (Pv + k * Dv)^2 = f(Pw + k * Dw)
*
*    has to be solved for k (cubic polynomial in k).
*
*    Note that Pu, Pv, Pw and Du, Dv, Dw denote the coordinates
*    of the vectors P and D.
*
*  Syntax:
*
*    revolution
*    {
*      number_of_points,
*
*      <P[0]>, <P[1]>, ..., <P[n-1]>
*
*      [ open ]
*    }
*
*    Note that the P[i] are 2d vectors where u corresponds to the radius
*    and v to the height.
*
*    Note that the first and last point, i.e. P[0] and P[n-1], are used
*    to determine the derivatives at the end point.
*
*    Note that the x coordinate of a point corresponds to the radius and
*    the y coordinate to the height; the z coordinate isn't used.
*
*  ---
*
*  Ideas for the surface of revolution were taken from:
*
*    P. Burger and D. Gillies, "Rapid Ray Tracing of General Surfaces
*    of Revolution", New Advances in Computer Graphics, Proceedings
*    of CG International '89, R. A. Earnshaw, B. Wyvill (Eds.),
*    Springer, ..., pp. 523-531
*
*  ---
*
*  May 1994 : Creation. [DB]
*
*****************************************************************************/

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/shape/sor.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>
#include <vector>

// POV-Ray header files (base module)
#include "base/pov_err.h"

// POV-Ray header files (core module)
#include "core/bounding/boundingbox.h"
#include "core/bounding/boundingcylinder.h"
#include "core/math/matrix.h"
#include "core/math/polynomialsolver.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::min;
using std::max;

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Minimal intersection depth for a valid intersection. */

const DBL DEPTH_TOLERANCE = 1.0e-4;

/* Part of the surface of revolution hit. */

const int BASE_PLANE = 1;
const int CAP_PLANE  = 2;
const int CURVE      = 3;

/* Max. number of intersecions per spline segment. */

const int MAX_INTERSECTIONS_PER_SEGMENT = 4;



/*****************************************************************************
*
* FUNCTION
*
*   All_Sor_Intersections
*
* INPUT
*
*   Object      - Object
*   Ray         - Ray
*   Depth_Stack - Intersection stack
*
* OUTPUT
*
*   Depth_Stack
*
* RETURNS
*
*   int - true, if a intersection was found
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Determine ray/surface of revolution intersection and
*   clip intersection found.
*
* CHANGES
*
*   May 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

bool Sor::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    Thread->Stats()[Ray_Sor_Tests]++;

    if (Intersect(ray, Depth_Stack, Thread))
    {
        Thread->Stats()[Ray_Sor_Tests_Succeeded]++;

        return(true);
    }

    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_sor
*
* INPUT
*
*   Ray          - Ray
*   Sor   - Sor
*   Intersection - Sor intersection structure
*
* OUTPUT
*
*   Intersection
*
* RETURNS
*
*   int - Number of intersections found
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Determine ray/surface of revolution intersection.
*
*   NOTE: The curve is rotated about the y-axis!
*         Order reduction cannot be used.
*
* CHANGES
*
*   May 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

bool Sor::Intersect(const BasicRay& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int cnt;
    int found, j, n;
    DBL a, b, k, h, len, u, v, r0;
    DBL x[4];
    DBL y[3];
    DBL best;
    Vector3d P, D;
    SOR_SPLINE_ENTRY *Entry;

    /* Transform the ray into the surface of revolution space. */

    MInvTransPoint(P, ray.Origin, Trans);

    MInvTransDirection(D, ray.Direction, Trans);

    len = D.length();

    D /= len;

    /* Test if ray misses object's bounds. */

#ifdef SOR_EXTRA_STATS
    Thread->Stats()[Sor_Bound_Tests]++;
#endif

    if (((D[Y] >= 0.0) && (P[Y] >  Height2)) ||
        ((D[Y] <= 0.0) && (P[Y] <  Height1)) ||
        ((D[X] >= 0.0) && (P[X] >  Radius2)) ||
        ((D[X] <= 0.0) && (P[X] < -Radius2)))
    {
        return(false);
    }

    /* Get distance of the ray from rotation axis (= y axis). */

    r0 = P[X] * D[Z] - P[Z] * D[X];

    if ((a = D[X] * D[X] + D[Z] * D[Z]) > 0.0)
    {
        r0 /= sqrt(a);
    }

    /* Test if ray misses object's bounds. */

    if (r0 > Radius2)
    {
        return(false);
    }

    /* Test base/cap plane. */

    found = false;

    best = BOUND_HUGE;

    if (Test_Flag(this, CLOSED_FLAG) && (fabs(D[Y]) > EPSILON))
    {
        /* Test base plane. */

        if (Base_Radius_Squared > DEPTH_TOLERANCE)
        {
            k = (Height1 - P[Y]) / D[Y];

            u = P[X] + k * D[X];
            v = P[Z] + k * D[Z];

            b = u * u + v * v;

            if (b <= Base_Radius_Squared)
            {
                if (test_hit(ray, Depth_Stack, k / len, k, BASE_PLANE, 0, Thread))
                {
                    found = true;

                    if (k < best)
                    {
                        best = k;
                    }
                }
            }
        }

        /* Test cap plane. */

        if (Cap_Radius_Squared > DEPTH_TOLERANCE)
        {
            k = (Height2 - P[Y]) / D[Y];

            u = P[X] + k * D[X];
            v = P[Z] + k * D[Z];

            b = u * u + v * v;

            if (b <= Cap_Radius_Squared)
            {
                if (test_hit(ray, Depth_Stack, k / len, k, CAP_PLANE, 0, Thread))
                {
                    found = true;

                    if (k < best)
                    {
                        best = k;
                    }
                }
            }
        }
    }

    /* Intersect all cylindrical bounds. */
    std::vector<BCYL_INT>& intervals = Thread->BCyl_Intervals;
    std::vector<BCYL_INT>& rint = Thread->BCyl_RInt;
    std::vector<BCYL_INT>& hint = Thread->BCyl_HInt;

    if ((cnt = Intersect_BCyl(Spline->BCyl, intervals, rint, hint, P, D)) == 0)
    {
#ifdef SOR_EXTRA_STATS
        if (found)
            Thread->Stats()[Sor_Bound_Tests_Succeeded]++;
#endif
        return(found);
    }

#ifdef SOR_EXTRA_STATS
    Thread->Stats()[Sor_Bound_Tests_Succeeded]++;
#endif

/* Step through the list of intersections. */

    for (j = 0; j < cnt; j++)
    {
        /* Get current segment. */

        Entry = &Spline->Entry[intervals[j].n];

        /* If we already have the best intersection we may exit. */

        if (!(Type & IS_CHILD_OBJECT) && (intervals[j].d[0] > best))
        {
            break;
        }

        /* Cubic curve. */

        x[0] = Entry->A * D[Y] * D[Y] * D[Y];

/*
        x[1] = D[Y] * D[Y] * (3.0 * Entry->A * P[Y] + Entry->B) - D[X] * D[X] - D[Z] * D[Z];
*/
        x[1] = D[Y] * D[Y] * (3.0 * Entry->A * P[Y] + Entry->B) - a;

        x[2] = D[Y] * (P[Y] * (3.0 * Entry->A * P[Y] + 2.0 * Entry->B) + Entry->C) - 2.0 * (P[X] * D[X] + P[Z] * D[Z]);

        x[3] = P[Y] * (P[Y] * (Entry->A * P[Y] + Entry->B) + Entry->C) + Entry->D - P[X] * P[X] - P[Z] * P[Z];

        n = Solve_Polynomial(3, x, y, Test_Flag(this, STURM_FLAG), 0.0, Thread->Stats());

        while (n--)
        {
            k = y[n];

            h = P[Y] + k * D[Y];

            if ((h >= Spline->BCyl->height[Spline->BCyl->entry[intervals[j].n].h1]) &&
                (h <= Spline->BCyl->height[Spline->BCyl->entry[intervals[j].n].h2]))
            {
                if (test_hit(ray, Depth_Stack, k / len, k, CURVE, intervals[j].n, Thread))
                {
                    found = true;

                    if (y[n] < best)
                    {
                        best = k;
                    }
                }
            }
        }
    }

    return(found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Sor
*
* INPUT
*
*   IPoint - Intersection point
*   Object - Object
*
* OUTPUT
*
* RETURNS
*
*   int - true if inside
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Return true if point lies inside the surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

bool Sor::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    int i;
    DBL r0, r;
    Vector3d P;
    SOR_SPLINE_ENTRY *Entry = nullptr;

    /* Transform the point into the surface of revolution space. */

    MInvTransPoint(P, IPoint, Trans);

    /* Test if we are inside the cylindrical bound. */

    if ((P[Y] >= Height1) && (P[Y] <= Height2))
    {
        r0 = P[X] * P[X] + P[Z] * P[Z];

        /* Test if we are inside the cylindrical bound. */

        if (r0 <= Sqr(Radius2))
        {
            /* Now find the segment the point is in. */

            for (i = 0; i < Number; i++)
            {
                Entry = &Spline->Entry[i];

                if ((P[Y] >= Spline->BCyl->height[Spline->BCyl->entry[i].h1]) &&
                    (P[Y] <= Spline->BCyl->height[Spline->BCyl->entry[i].h2]))
                {
                    break;
                }
            }

            /* Have we found any segment? */

            if (i < Number)
            {
                r = P[Y] * (P[Y] * (P[Y] * Entry->A + Entry->B) + Entry->C) + Entry->D;

                if (r0 <= r)
                {
                    /* We're inside. */

                    return(!Test_Flag(this, INVERTED_FLAG));
                }
            }
        }
    }

    /* We're outside. */

    return(Test_Flag(this, INVERTED_FLAG));
}



/*****************************************************************************
*
* FUNCTION
*
*   Sor_Normal
*
* INPUT
*
*   Result - Normal vector
*   Object - Object
*   Inter  - Intersection found
*
* OUTPUT
*
*   Result
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the normal of the surface of revolution in a given point.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Sor::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    DBL k;
    Vector3d P;
    SOR_SPLINE_ENTRY *Entry;
    Vector3d N;

    switch (Inter->i1)
    {
        case CURVE:

            /* Transform the intersection point into the surface of revolution space. */

            MInvTransPoint(P, Inter->IPoint, Trans);

            Entry = &Spline->Entry[Inter->i2];

            k = 0.5 * (P[Y] * (3.0 * Entry->A * P[Y] + 2.0 * Entry->B) + Entry->C);

            N[X] = P[X];
            N[Y] = -k;
            N[Z] = P[Z];

            break;

        case BASE_PLANE:

            N = Vector3d(0.0, -1.0, 0.0);

            break;


        case CAP_PLANE:

            N = Vector3d(0.0, 1.0, 0.0);

            break;
    }

    /* Transform the normal out of the surface of revolution space. */

    MTransNormal(Result, N, Trans);

    Result.normalize();
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Sor
*
* INPUT
*
*   Object - Object
*   Vector - Translation vector
*
* OUTPUT
*
*   Object
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Translate a surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Sor::Translate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Sor
*
* INPUT
*
*   Object - Object
*   Vector - Rotation vector
*
* OUTPUT
*
*   Object
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Rotate a surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Sor::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Sor
*
* INPUT
*
*   Object - Object
*   Vector - Scaling vector
*
* OUTPUT
*
*   Object
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Scale a surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Sor::Scale(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Sor
*
* INPUT
*
*   Object - Object
*   Trans  - Transformation to apply
*
* OUTPUT
*
*   Object
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Transform a surface of revolution and recalculate its bounding box.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Sor::Transform(const TRANSFORM *tr)
{
    Compose_Transforms(Trans, tr);

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Sor
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   SOR * - new surface of revolution
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a new surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

Sor::Sor() : ObjectBase(SOR_OBJECT)
{
    Trans = Create_Transform();

    Spline = nullptr;

    Radius2             = 0.0;
    Base_Radius_Squared = 0.0;
    Cap_Radius_Squared  = 0.0;

    /* SOR should have capped ends by default. CEY 3/98*/

    Set_Flag(this, CLOSED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Sor
*
* INPUT
*
*   Object - Object
*
* OUTPUT
*
* RETURNS
*
*   void * - New surface of revolution
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a surface of revolution structure.
*
*   NOTE: The splines are not copied, only the number of references is
*         counted, so that Destray_Sor() knows if they can be destroyed.
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Sep 1994 : fixed memory leakage [DB]
*
******************************************************************************/

ObjectPtr Sor::Copy()
{
    Sor *New = new Sor();
    Destroy_Transform(New->Trans);
    *New = *this;
    New->Trans = Copy_Transform(Trans);

    New->Spline->References++;

    return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Sor
*
* INPUT
*
*   Object - Object
*
* OUTPUT
*
*   Object
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Destroy a surface of revolution.
*
*   NOTE: The splines are destroyed if they are no longer used by any copy.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

Sor::~Sor()
{
    if (--(Spline->References) == 0)
    {
        Destroy_BCyl(Spline->BCyl);

        delete[] Spline->Entry;

        delete Spline;
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Sor_BBox
*
* INPUT
*
*   Sor - Sor
*
* OUTPUT
*
*   Sor
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Sor::Compute_BBox()
{
    Make_BBox(BBox, -Radius2, Height1, -Radius2,
        2.0 * Radius2, Height2 - Height1, 2.0 * Radius2);

    Recompute_BBox(&BBox, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Sor
*
* INPUT
*
*   Sor - Sor
*   P          - Points defining surface of revolution
*
* OUTPUT
*
*   Sor
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer, June 1994
*
* DESCRIPTION
*
*   Calculate the spline segments of a surface of revolution
*   from a set of points.
*
*   Note that the number of points in the surface of revolution has to be set.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Sor::Compute_Sor(Vector2d *P, RenderStatistics& stats)
{
    int i, n;
    DBL *tmp_r1;
    DBL *tmp_r2;
    DBL *tmp_h1;
    DBL *tmp_h2;
    DBL A, B, C, D, w;
    DBL xmax, xmin, ymax, ymin;
    DBL k[4], x[4];
    DBL y[2], r[2];
    DBL c[3];
    MATRIX Mat;

    /* Allocate Number segments. */

    if (Spline == nullptr)
    {
        Spline = new SOR_SPLINE;

        Spline->References = 1;

        Spline->Entry = new SOR_SPLINE_ENTRY[Number];
    }
    else
    {
        throw POV_EXCEPTION_STRING("Surface of revolution segments are already defined.");
    }

    /* Allocate temporary lists. */

    tmp_r1 = new DBL[Number];
    tmp_r2 = new DBL[Number];
    tmp_h1 = new DBL[Number];
    tmp_h2 = new DBL[Number];

    /* We want to know the size of the overall bounding cylinder. */

    xmax = ymax = -BOUND_HUGE;
    xmin = ymin =  BOUND_HUGE;

    /* Calculate segments, i.e. cubic patches. */

    for (i = 0; i < Number; i++)
    {
        if ((fabs(P[i+2][Y] - P[i][Y]) < EPSILON) ||
            (fabs(P[i+3][Y] - P[i+1][Y]) < EPSILON))
        {
            throw POV_EXCEPTION_STRING("Incorrect point in surface of revolution.");
        }

        /* Use cubic interpolation. */

        k[0] = P[i+1][X] * P[i+1][X];
        k[1] = P[i+2][X] * P[i+2][X];
        k[2] = (P[i+2][X] - P[i][X]) / (P[i+2][Y] - P[i][Y]);
        k[3] = (P[i+3][X] - P[i+1][X]) / (P[i+3][Y] - P[i+1][Y]);

        k[2] *= 2.0 * P[i+1][X];
        k[3] *= 2.0 * P[i+2][X];

        w = P[i+1][Y];

        Mat[0][0] = w * w * w;
        Mat[0][1] = w * w;
        Mat[0][2] = w;
        Mat[0][3] = 1.0;

        Mat[2][0] = 3.0 * w * w;
        Mat[2][1] = 2.0 * w;
        Mat[2][2] = 1.0;
        Mat[2][3] = 0.0;

        w = P[i+2][Y];

        Mat[1][0] = w * w * w;
        Mat[1][1] = w * w;
        Mat[1][2] = w;
        Mat[1][3] = 1.0;

        Mat[3][0] = 3.0 * w * w;
        Mat[3][1] = 2.0 * w;
        Mat[3][2] = 1.0;
        Mat[3][3] = 0.0;

        MInvers(Mat, Mat);

        /* Calculate coefficients of cubic patch. */

        A = k[0] * Mat[0][0] + k[1] * Mat[0][1] + k[2] * Mat[0][2] + k[3] * Mat[0][3];
        B = k[0] * Mat[1][0] + k[1] * Mat[1][1] + k[2] * Mat[1][2] + k[3] * Mat[1][3];
        C = k[0] * Mat[2][0] + k[1] * Mat[2][1] + k[2] * Mat[2][2] + k[3] * Mat[2][3];
        D = k[0] * Mat[3][0] + k[1] * Mat[3][1] + k[2] * Mat[3][2] + k[3] * Mat[3][3];

        if (fabs(A) < EPSILON) A = 0.0;
        if (fabs(B) < EPSILON) B = 0.0;
        if (fabs(C) < EPSILON) C = 0.0;
        if (fabs(D) < EPSILON) D = 0.0;

        Spline->Entry[i].A = A;
        Spline->Entry[i].B = B;
        Spline->Entry[i].C = C;
        Spline->Entry[i].D = D;

        /* Get minimum and maximum radius**2 in current segment. */

        y[0] = P[i+1][Y];
        y[1] = P[i+2][Y];

        x[0] = x[2] = P[i+1][X];
        x[1] = x[3] = P[i+2][X];

        c[0] = 3.0 * A;
        c[1] = 2.0 * B;
        c[2] = C;

        n = Solve_Polynomial(2, c, r, false, 0.0, stats);

        while (n--)
        {
            if ((r[n] >= y[0]) && (r[n] <= y[1]))
            {
                x[n] = sqrt(r[n] * (r[n] * (r[n] * A + B) + C) + D);
            }
        }

        /* Set current segment's bounding cylinder. */

        tmp_r1[i] = min(min(x[0], x[1]), min(x[2], x[3]));
        tmp_r2[i] = max(max(x[0], x[1]), max(x[2], x[3]));

        tmp_h1[i] = y[0];
        tmp_h2[i] = y[1];

        /* Keep track of overall bounding cylinder. */

        xmin = min(xmin, tmp_r1[i]);
        xmax = max(xmax, tmp_r2[i]);

        ymin = min(ymin, tmp_h1[i]);
        ymax = max(ymax, tmp_h2[i]);

/*
        fprintf(stderr, "bound spline segment %d: ", i);
        fprintf(stderr, "r = %f - %f, h = %f - %f\n", tmp_r1[i], tmp_r2[i], tmp_h1[i], tmp_h2[i]);
*/
    }

    /* Set overall bounding cylinder. */

    Radius1 = xmin;
    Radius2 = xmax;

    Height1 = ymin;
    Height2 = ymax;

    /* Get cap radius. */

    w = tmp_h2[Number-1];

    A = Spline->Entry[Number-1].A;
    B = Spline->Entry[Number-1].B;
    C = Spline->Entry[Number-1].C;
    D = Spline->Entry[Number-1].D;

    if ((Cap_Radius_Squared = w * (w * (A * w + B) + C) + D) < 0.0)
    {
        Cap_Radius_Squared = 0.0;
    }

    /* Get base radius. */

    w = tmp_h1[0];

    A = Spline->Entry[0].A;
    B = Spline->Entry[0].B;
    C = Spline->Entry[0].C;
    D = Spline->Entry[0].D;

    if ((Base_Radius_Squared = w * (w * (A * w + B) + C) + D) < 0.0)
    {
        Base_Radius_Squared = 0.0;
    }

    /* Get bounding cylinder. */

    Spline->BCyl = Create_BCyl(Number, tmp_r1, tmp_r2, tmp_h1, tmp_h2);

    /* Get rid of temp. memory. */

    delete[] tmp_h2;
    delete[] tmp_h1;
    delete[] tmp_r2;
    delete[] tmp_r1;
}



/*****************************************************************************
*
* FUNCTION
*
*   test_hit
*
* INPUT
*
*   Sor         - Pointer to lathe structure
*   Ray         - Current ray
*   Depth_Stack - Current depth stack
*   d, t, n     - Intersection depth, type and number
*
* OUTPUT
*
*   Depth_Stack
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Test if a hit is valid and push if on the intersection depth.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

bool Sor::test_hit(const BasicRay &ray, IStack& Depth_Stack, DBL d, DBL k, int t, int n, TraceThreadData *Thread)
{
    Vector3d IPoint;

    if ((d > DEPTH_TOLERANCE) && (d < MAX_DISTANCE))
    {
        IPoint = ray.Evaluate(d);

        if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
        {
            /* is the extra copy of d redundant? */
            Depth_Stack->push(Intersection(d, IPoint, this, t, n, k));

            return(true);
        }
    }

    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Sor_UVCoord
*
* INPUT
*
*   Result - UV coordinates of intersection (u - rotation, v = height)
*   Object - Object
*   Inter  - Intersection found
*
* OUTPUT
*
*   Result
*
* RETURNS
*
* AUTHOR
*
*   Nathan Kopp
*
* DESCRIPTION
*
*
*
* CHANGES
*
*   Oct 1998 : Creation.
*
******************************************************************************/

void Sor::UVCoord(Vector2d& Result, const Intersection *Inter) const
{
    DBL len, theta;
    DBL h, v_per_segment;
    Vector3d P;

    /* Transform the point into the lathe space. */
    MInvTransPoint(P, Inter->IPoint, Trans);

    /* Determine its angle from the point (1, 0, 0) in the x-z plane. */
    len = P[X] * P[X] + P[Z] * P[Z];

    if (len > EPSILON)
    {
        len = sqrt(len);
        if (P[Z] == 0.0)
        {
            if (P[X] > 0)
                theta = 0.0;
            else
                theta = M_PI;
        }
        else
        {
            theta = acos(P[X] / len);
            if (P[Z] < 0.0)
                theta = TWO_M_PI - theta;
        }

        theta /= TWO_M_PI;  /* This will be from 0 to 1 */
    }
    else
        /* This point is at one of the poles. Any value of xcoord will be ok... */
        theta = 0;

    Result[U] = theta;

    /* ------------------- now figure out v --------------------- */
    switch (Inter->i1)
    {
        case CURVE:
            /* h is width of this segment */
            h =
             Spline->BCyl->height[Spline->BCyl->entry[Inter->i2].h2] -
             Spline->BCyl->height[Spline->BCyl->entry[Inter->i2].h1];

            /* change in v per segment... divide total v (1.0) by number of segments */
            v_per_segment = 1.0/(Number);

            /* now find the current v given the current y */
            Result[V] = (P[Y] - Spline->BCyl->height[Spline->BCyl->entry[Inter->i2].h1]) / h
                       * v_per_segment + (Inter->i2*v_per_segment);

            break;

        case BASE_PLANE:
            /*Result[V] = 0;*/
            Result[V] = sqrt(P[X]*P[X]+P[Z]*P[Z])/sqrt(Base_Radius_Squared)-1;
            break;

        case CAP_PLANE:
            /*Result[V] = 1;*/
            Result[V] = -sqrt(P[X]*P[X]+P[Z]*P[Z])/sqrt(Cap_Radius_Squared)+2;
            break;
    }

    /*Result[V] = 0;*/

}

}
// end of namespace pov
