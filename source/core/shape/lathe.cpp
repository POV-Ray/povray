//******************************************************************************
///
/// @file core/shape/lathe.cpp
///
/// Implementation of the lathe geometric primitive.
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
*    The lathe primitive is defined by a set of points in 2d space which
*    are interpolated by linear, quadratic, or cubic splines. The resulting
*    2d curve is rotated about an axis to form the final lathe object.
*
*    All calculations are done in the object's (u,v,w)-coordinate system
*    with the (w)-axis being the rotation axis.
*
*    One spline segment in the (r,w)-plane is given by the equations
*
*      fw(t) = Aw * t^3 + Bw * t^2 + Cw * t + Dw  and
*      fr(t) = Ar * t^3 + Br * t^2 + Cr * t + Dr,
*
*    with the parameter t ranging from 0 to 1 and r = sqrt(u*u+v*v).
*
*    To intersect a ray R = P + k * D transformed into the object's
*    coordinate system with the lathe object, the equations
*
*      (Pu + k * Du)^2 + (Pv + k * Dv)^2 = (fr(t))^2
*                          (Pw + k * Dw) = fw(t)
*
*    have to be solved for t. For valid intersections (0 <= t <= 1)
*    the corresponding k can be calculated from one of the above equations.
*
*    Note that the degree of the polynomal to solve is two times the
*    degree of the spline segments used.
*
*    Note that Pu, Pv, Pw and Du, Dv, Dw denote the coordinates
*    of the vectors P and D.
*
*  Syntax:
*
*    lathe
*    {
*      [ linear_spline | quadratic_spline | cubic_spline ]
*
*      number_of_points,
*
*      <P[0]>, <P[1], ..., <P[n-1]>
*
*      [ sturm ]
*    }
*
*    Note that the P[i] are 2d vectors.
*
*    Linear interpolation is used by default. In this case all n Points
*    are used. In the quadratic case the first point is used to determine
*    the derivates at the starting point P[1]. In the cubic case
*    the first and last points are used to determine the derivatives at
*    the starting point P[1] and ending point P[n-2].
*
*    To get a closed (and smooth) curve you have make sure that
*
*      P[0] = P[n-1] in the linear case,
*
*      P[0] = P[n-2] and P[1] = P[n-1] in the quadratic case, and
*
*      P[0] = P[n-3] and P[1] = P[n-2] and P[2] = P[n-1] in the cubic case.
*
*    Note that the x coordinate of a point corresponds to the r coordinate
*    and the y coordinate to the w coordinate;
*
*  ---
*
*  Ideas for the lathe were taken from:
*
*    P. Burger and D. Gillies, "Rapid Ray Tracing of General Surfaces
*    of Revolution", New Advances in Computer Graphics, Proceedings
*    of CG International '89, R. A. Earnshaw, B. Wyvill (Eds.),
*    Springer, ..., pp. 523-531
*
*    P. Burger and D. Gillies, "Swept Surfaces", Interactive Computer
*    Graphics, Addison-Wesley, 1989, pp. 376-380
*
*  ---
*
*  Jun 1994 : Creation. [DB]
*
*****************************************************************************/

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/shape/lathe.h"

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
#include "core/shape/torus.h"
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

/*****************************************************************************
*
* FUNCTION
*
*   All_Lathe_Intersections
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
*   Determine ray/lathe intersection and clip intersection found.
*
* CHANGES
*
*   Jun 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

bool Lathe::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    Thread->Stats()[Ray_Lathe_Tests]++;

    if(Intersect(ray, Depth_Stack, Thread))
    {
        Thread->Stats()[Ray_Lathe_Tests_Succeeded]++;
        return(true);
    }

    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_lathe
*
* INPUT
*
*   Ray          - Ray
*   Lathe        - Lathe
*   Intersection - Lathe intersection structure
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
*   Determine ray/lathe intersection.
*
*   NOTE: The curve is rotated about the y-axis!
*         Order of the polynomial must not be used!
*
* CHANGES
*
*   Jun 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

bool Lathe::Intersect(const BasicRay& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int cnt;
    int found, j, n1, n2;
    DBL k, len, r, m, w, Dy2, r0, Dylen;
    DBL x1[7];
    DBL x2[3];
    DBL y1[6];
    DBL y2[2];
    DBL best;
    Vector3d P, D;
    LATHE_SPLINE_ENTRY *Entry;

    // Transform the ray into the lathe space.

    MInvTransPoint(P, ray.Origin, Trans);
    MInvTransDirection(D, ray.Direction, Trans);

    len = D.length();
    D /= len;

    Dylen = D[Y] * len; // TODO FIXME - why don't we do this *before* normalizing, saving us the multiplication by len?

    #ifdef LATHE_EXTRA_STATS
        Thread->Stats()[Lathe_Bound_Tests]++;
    #endif

    // Test if ray misses lathe's cylindrical bound.
    if(((D[Y] >= 0.0) && (P[Y] >  Height2)) ||
       ((D[Y] <= 0.0) && (P[Y] <  Height1)) ||
       ((D[X] >= 0.0) && (P[X] >  Radius2)) ||
       ((D[X] <= 0.0) && (P[X] < -Radius2)))
        return false;

    // Get distance r0 of the ray from rotation axis (i.e. y axis).
    r0 = fabs(P[X] * D[Z] - P[Z] * D[X]);
    r = D[X] * D[X] + D[Z] * D[Z];
    if(r > 0.0)
        r0 /= sqrt(r);

    // Test if ray misses lathe's cylindrical bound.
    if(r0 > Radius2)
        return false;

    // Intersect all cylindrical bounds.
    std::vector<BCYL_INT>& intervals = Thread->BCyl_Intervals;
    std::vector<BCYL_INT>& rint = Thread->BCyl_RInt;
    std::vector<BCYL_INT>& hint = Thread->BCyl_HInt;

    if((cnt = Intersect_BCyl(Spline->BCyl, intervals, rint, hint, P, D)) == 0)
        return false;

    #ifdef LATHE_EXTRA_STATS
        Thread->Stats()[Lathe_Bound_Tests_Succeeded]++;
    #endif

    // Precalculate some constants that are ray-dependant only.
    m = D[X] * P[X] + D[Z] * P[Z];
    Dy2 = D[Y] * D[Y];

    // Step through the list of intersections.
    found = false;
    best = BOUND_HUGE;

    for(j = 0; j < cnt; j++)
    {
        // Get current segment.
        Entry = &Spline->Entry[intervals[j].n];

        // If we already have the best intersection we may exit.
        if(!(Type & IS_CHILD_OBJECT) && (intervals[j].d[0] > best))
            break;

        // Init number of roots found.
        n1 = 0;

        // Intersect segment.
        switch(Spline_Type)
        {
            // Linear spline
            case LINEAR_SPLINE:
                // Solve 2th order polynomial.
                x1[0] = Entry->C[Y] * Entry->C[Y] * r - Entry->C[X] * Entry->C[X] * Dy2;
                x1[1] = 2.0 * (Entry->C[Y] * ((Entry->D[Y] - P[Y]) * r + D[Y] * m) - Entry->C[X] * Entry->D[X] * Dy2);
                x1[2] = (Entry->D[Y] - P[Y]) * ((Entry->D[Y] - P[Y]) * r + 2.0 * D[Y] * m) + Dy2 * (P[X] * P[X] + P[Z] * P[Z] - Entry->D[X] * Entry->D[X]);
                n1 = Solve_Polynomial(2, x1, y1, false, 0.0, Thread->Stats());
                break;
            // Quadratic spline
            case QUADRATIC_SPLINE:
                // Solve 4th order polynomial.
                x1[0] = Entry->B[Y] * Entry->B[Y] * r - Entry->B[X] * Entry->B[X] * Dy2;
                x1[1] = 2.0 * (Entry->B[Y] * Entry->C[Y] * r - Entry->B[X] * Entry->C[X] * Dy2);
                x1[2] = r * (2.0 * Entry->B[Y] * (Entry->D[Y] - P[Y]) + Entry->C[Y] * Entry->C[Y]) + 2.0 * Entry->B[Y] * D[Y] * m - (2.0 * Entry->B[X] * Entry->D[X] + Entry->C[X] * Entry->C[X]) * Dy2;
                x1[3] = 2.0 * (Entry->C[Y] * ((Entry->D[Y] - P[Y]) * r + D[Y] * m) - Entry->C[X] * Entry->D[X] * Dy2);
                x1[4] = (Entry->D[Y] - P[Y]) * ((Entry->D[Y] - P[Y]) * r + 2.0 * D[Y] * m) + Dy2 * (P[X] * P[X] + P[Z] * P[Z] - Entry->D[X] * Entry->D[X]);
                n1 = Solve_Polynomial(4, x1, y1, Test_Flag(this, STURM_FLAG), 0.0, Thread->Stats());
                break;
            // Cubic spline
            case BEZIER_SPLINE:
            case CUBIC_SPLINE:
                // Solve 6th order polynomial.
                x1[0] = Entry->A[Y] * Entry->A[Y] * r - Entry->A[X] * Entry->A[X] * Dy2;
                x1[1] = 2.0 * (Entry->A[Y] * Entry->B[Y] * r - Entry->A[X] * Entry->B[X] * Dy2);
                x1[2] = (2.0 * Entry->A[Y] * Entry->C[Y] + Entry->B[Y] * Entry->B[Y]) * r - (2.0 * Entry->A[X] * Entry->C[X] + Entry->B[X] * Entry->B[X]) * Dy2;
                x1[3] = 2.0 * ((Entry->A[Y] * Entry->D[Y] + Entry->B[Y] * Entry->C[Y] - Entry->A[Y] * P[Y]) * r + Entry->A[Y] * D[Y] * m - (Entry->A[X] * Entry->D[X] + Entry->B[X] * Entry->C[X]) * Dy2);
                x1[4] = (2.0 * Entry->B[Y] * (Entry->D[Y] - P[Y]) + Entry->C[Y] * Entry->C[Y]) * r + 2.0 * Entry->B[Y] * D[Y] * m - (2.0 * Entry->B[X] * Entry->D[X] + Entry->C[X] * Entry->C[X]) * Dy2;
                x1[5] = 2.0 * (Entry->C[Y] * ((Entry->D[Y] - P[Y]) * r + D[Y] * m) - Entry->C[X] * Entry->D[X] * Dy2);
                x1[6] = (Entry->D[Y] - P[Y]) * ((Entry->D[Y] - P[Y]) * r + 2.0 * D[Y] * m) + Dy2 * (P[X] * P[X] + P[Z] * P[Z] - Entry->D[X] * Entry->D[X]);
                n1 = Solve_Polynomial(6, x1, y1, Test_Flag(this, STURM_FLAG), 0.0, Thread->Stats());
                break;
        }

        // Test roots for valid intersections.
        while(n1--)
        {
            w = y1[n1];
            if((w >= 0.0) && (w <= 1.0))
            {
                if(fabs(D[Y]) > EPSILON)
                {
                    k = (w * (w * (w * Entry->A[Y] + Entry->B[Y]) + Entry->C[Y]) + Entry->D[Y] - P[Y]);

                    if (test_hit(ray, Depth_Stack, k / Dylen, w, intervals[j].n, Thread))
                    {
                        found = true;
                        k /= D[Y];
                        if (k < best)
                            best = k;
                    }
                }
                else
                {
                    k = w * (w * (w * Entry->A[X] + Entry->B[X]) + Entry->C[X]) + Entry->D[X];

                    x2[0] = r;
                    x2[1] = 2.0 * m;
                    x2[2] = P[X] * P[X] + P[Z] * P[Z] - k * k;

                    n2 = Solve_Polynomial(2, x2, y2, false, 0.0, Thread->Stats());
                    while(n2--)
                    {
                        k = y2[n2];
                        if(test_hit(ray, Depth_Stack, k / len, w, intervals[j].n, Thread))
                        {
                            found = true;
                            if(k < best)
                                best = k;
                        }
                    }
                }
            }
        }
    }

    return found;
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Lathe
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
*   Test if a point lies inside the lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

bool Lathe::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    int i, n, NC;
    DBL r, k, w;
    DBL x[4];
    DBL y[3];
    DBL *height;
    Vector3d P;
    BCYL_ENTRY *entry;
    LATHE_SPLINE_ENTRY *Entry;

    height = Spline->BCyl->height;

    entry = Spline->BCyl->entry;

    /* Transform the point into the lathe space. */

    MInvTransPoint(P, IPoint, Trans);

    /* Number of crossings. */

    NC = 0;

    if ((P[Y] >= Height1) && (P[Y] <= Height2))
    {
        r = sqrt(P[X] * P[X] + P[Z] * P[Z]);

        if ((r >= Radius1) && (r <= Radius2))
        {
            for (i = 0; i < Number; i++)
            {
                Entry = &Spline->Entry[i];

                /* Test if we are inside the segments cylindrical bound. */

                if ((P[Y] >= height[entry[i].h1] - EPSILON) &&
                    (P[Y] <= height[entry[i].h2] + EPSILON))
                {
                    x[0] = Entry->A[Y];
                    x[1] = Entry->B[Y];
                    x[2] = Entry->C[Y];
                    x[3] = Entry->D[Y] - P[Y];

                    n = Solve_Polynomial(3, x, y, Test_Flag(this, STURM_FLAG), 0.0, Thread->Stats());

                    while (n--)
                    {
                        w = y[n];

                        if ((w >= 0.0) && (w <= 1.0))
                        {
                            k  = w * (w * (w * Entry->A[X] + Entry->B[X]) + Entry->C[X]) + Entry->D[X] - r;

                            if (k >= 0.0)
                            {
                                NC++;
                            }
                        }
                    }
                }
            }
        }
    }

    if (NC & 1)
    {
        return(!Test_Flag(this, INVERTED_FLAG));
    }
    else
    {
        return(Test_Flag(this, INVERTED_FLAG));
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Lathe_Normal
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
*   Calculate the normal of the lathe in a given point.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

void Lathe::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    DBL r, dx, dy;
    Vector3d P, N;
    LATHE_SPLINE_ENTRY *Entry;

    Entry = &Spline->Entry[Inter->i1];

    /* Transform the point into the lathe space. */

    MInvTransPoint(P, Inter->IPoint, Trans);

    /* Get distance from rotation axis. */

    r = P[X] * P[X] + P[Z] * P[Z];

    if (r > EPSILON)
    {
        r = sqrt(r);

        /* Get derivatives. */

        dx = Inter->d1 * (3.0 * Entry->A[X] * Inter->d1 + 2.0 * Entry->B[X]) + Entry->C[X];
        dy = Inter->d1 * (3.0 * Entry->A[Y] * Inter->d1 + 2.0 * Entry->B[Y]) + Entry->C[Y];

        /* Get normal by rotation. */

        N[X] =  dy * P[X];
        N[Y] = -dx * r;
        N[Z] =  dy * P[Z];
    }
    else
    {
        N[X] = N[Z] = 0.0; N[Y] = 1.0;
    }

    /* Transform the normalt out of the lathe space. */

    MTransNormal(Result, N, Trans);

    Result.normalize();
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Lathe
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
*   Translate a lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

void Lathe::Translate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Lathe
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
*   Rotate a lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

void Lathe::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Lathe
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
*   Scale a lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

void Lathe::Scale(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Lathe
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
*   Transform a lathe and recalculate its bounding box.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

void Lathe::Transform(const TRANSFORM *tr)
{
    Compose_Transforms(Trans, tr);

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Lathe
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   LATHE * - new lathe
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a new lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

Lathe::Lathe() : ObjectBase(LATHE_OBJECT)
{
    Trans = Create_Transform();

    Spline_Type = LINEAR_SPLINE;

    Number = 0;

    Spline = nullptr;

    Radius1 = 0.0;
    Radius2 = 0.0;
    Height1 = 0.0;
    Height2 = 0.0;
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Lathe
*
* INPUT
*
*   Object - Object
*
* OUTPUT
*
* RETURNS
*
*   void * - New lathe
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a lathe structure.
*
*   NOTE: The splines are not copied, only the number of references is
*         counted, so that Destray_Lathe() knows if they can be destroyed.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
*   Sep 1994 : Fixed memory leakage bug. [DB]
*
******************************************************************************/

ObjectPtr Lathe::Copy()
{
    Lathe *New = new Lathe();
    Destroy_Transform(New->Trans);
    *New = *this;
    New->Trans = Copy_Transform(Trans);
    New->Spline = Spline;
    New->Spline->References++;

    return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Lathe
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
*   Destroy a lathe.
*
*   NOTE: The splines are destroyed if they are no longer used by any copy.
*
* CHANGES
*
*   Jun 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

Lathe::~Lathe()
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
*   Compute_Lathe_BBox
*
* INPUT
*
*   Lathe - Lathe
*
* OUTPUT
*
*   Lathe
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

void Lathe::Compute_BBox()
{
    Make_BBox(BBox, -Radius2, Height1, -Radius2,
        2.0 * Radius2, Height2 - Height1, 2.0 * Radius2);

    Recompute_BBox(&BBox, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Lathe
*
* INPUT
*
*   Lathe - Lathe
*   P     - Points defining lathe
*
* OUTPUT
*
*   Lathe
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the spline segments of a lathe from a set of points.
*
*   Note that the number of points in the lathe has to be set.
*
* CHANGES
*
*   Jun 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

void Lathe::Compute_Lathe(Vector2d *P, RenderStatistics& stats)
{
    int i, i1, i2, i3, n, segment, number_of_segments;
    DBL x[4], y[4];
    DBL c[3];
    DBL r[2];
    DBL xmin, xmax, ymin, ymax;
    DBL *tmp_r1;
    DBL *tmp_r2;
    DBL *tmp_h1;
    DBL *tmp_h2;
    Vector2d A, B, C, D;

    /* Get number of segments. */

    switch (Spline_Type)
    {
        case LINEAR_SPLINE:

            number_of_segments = Number - 1;

            break;

        case QUADRATIC_SPLINE:

            number_of_segments = Number - 2;

            break;

        case CUBIC_SPLINE:

            number_of_segments = Number - 3;

            break;

        case BEZIER_SPLINE:

            number_of_segments = Number / 4;

            break;

        default: /* tw */

            number_of_segments = 0; /* tw */
    }

    /* Allocate segments. */

    if (Spline == nullptr)
    {
        Spline = new LATHE_SPLINE;

        /* Init spline. */

        Spline->References = 1;

        Spline->Entry = new LATHE_SPLINE_ENTRY[number_of_segments];
    }
    else
    {
        /* This should never happen! */

        throw POV_EXCEPTION_STRING("Lathe segments are already defined.");
    }

    /* Allocate temporary lists. */

    tmp_r1 = new DBL[number_of_segments];
    tmp_r2 = new DBL[number_of_segments];
    tmp_h1 = new DBL[number_of_segments];
    tmp_h2 = new DBL[number_of_segments];

    /***************************************************************************
    * Calculate segments.
    ****************************************************************************/

    /* We want to know the size of the overall bounding cylinder. */

    xmax = ymax = -BOUND_HUGE;
    xmin = ymin = BOUND_HUGE;

    for (i = segment = 0; segment < number_of_segments; )
    {
        i1 = i + 1;
        i2 = i + 2;
        i3 = i + 3;

        switch (Spline_Type)
        {
            /*************************************************************************
            * Linear spline (nothing more than a simple polygon).
            **************************************************************************/

            case LINEAR_SPLINE:

                /* Use linear interpolation. */

                A =  Vector2d(0.0);
                B =  Vector2d(0.0);
                C = -1.0 * P[i] + 1.0 * P[i1];
                D =  1.0 * P[i];

                /* Get maximum coordinates in current segment. */

                x[0] = x[2] = P[i][X];
                x[1] = x[3] = P[i1][X];

                y[0] = y[2] = P[i][Y];
                y[1] = y[3] = P[i1][Y];

                break;


            /*************************************************************************
            * Quadratic spline.
            **************************************************************************/

            case QUADRATIC_SPLINE:

                /* Use quadratic interpolation. */

                A =  Vector2d(0.0);
                B =  0.5 * P[i] - 1.0 * P[i1] + 0.5 * P[i2];
                C = -0.5 * P[i]               + 0.5 * P[i2];
                D =               1.0 * P[i1];

                /* Get maximum coordinates in current segment. */

                x[0] = x[2] = P[i1][X];
                x[1] = x[3] = P[i2][X];

                y[0] = y[2] = P[i1][Y];
                y[1] = y[3] = P[i2][Y];

                break;


            /*************************************************************************
            * Cubic spline.
            **************************************************************************/

            case CUBIC_SPLINE:

                /* Use cubic interpolation. */

                A = -0.5 * P[i] + 1.5 * P[i1] - 1.5 * P[i2] + 0.5 * P[i3];
                B =        P[i] - 2.5 * P[i1] + 2.0 * P[i2] - 0.5 * P[i3];
                C = -0.5 * P[i]               + 0.5 * P[i2];
                D =                     P[i1];

                /* Get maximum coordinates in current segment. */

                x[0] = x[2] = P[i1][X];
                x[1] = x[3] = P[i2][X];

                y[0] = y[2] = P[i1][Y];
                y[1] = y[3] = P[i2][Y];

                break;

            /*************************************************************************
            * Bezier spline.
            **************************************************************************/

            case BEZIER_SPLINE:

                /* Use Bernstein interpolation. */

                A = P[i3] - 3.0 * P[i2] + 3.0 * P[i1] -       P[i];
                B =         3.0 * P[i2] - 6.0 * P[i1] + 3.0 * P[i];
                C =                       3.0 * P[i1] - 3.0 * P[i];
                D =                                           P[i];

                x[0] = P[i][X];
                x[1] = P[i1][X];
                x[2] = P[i2][X];
                x[3] = P[i3][X];

                y[0] = P[i][Y];
                y[1] = P[i1][Y];
                y[2] = P[i2][Y];
                y[3] = P[i3][Y];

                break;

            default:

                throw POV_EXCEPTION_STRING("Unknown lathe type in Compute_Lathe().");

        }

        Spline->Entry[segment].A = A;
        Spline->Entry[segment].B = B;
        Spline->Entry[segment].C = C;
        Spline->Entry[segment].D = D;

        if ((Spline_Type == QUADRATIC_SPLINE) ||
            (Spline_Type == CUBIC_SPLINE))
        {
            /* Get maximum coordinates in current segment. */

            c[0] = 3.0 * A[X];
            c[1] = 2.0 * B[X];
            c[2] = C[X];

            n = Solve_Polynomial(2, c, r, false, 0.0, stats);

            while (n--)
            {
                if ((r[n] >= 0.0) && (r[n] <= 1.0))
                {
                    x[n] = r[n] * (r[n] * (r[n] * A[X] + B[X]) + C[X]) + D[X];
                }
            }

            c[0] = 3.0 * A[Y];
            c[1] = 2.0 * B[Y];
            c[2] = C[Y];

            n = Solve_Polynomial(2, c, r, false, 0.0, stats);

            while (n--)
            {
                if ((r[n] >= 0.0) && (r[n] <= 1.0))
                {
                    y[n] = r[n] * (r[n] * (r[n] * A[Y] + B[Y]) + C[Y]) + D[Y];
                }
            }
        }

        /* Set current segment's bounding cylinder. */

        tmp_r1[segment] = min(min(x[0], x[1]), min(x[2], x[3]));
        tmp_r2[segment] = max(max(x[0], x[1]), max(x[2], x[3]));

        tmp_h1[segment] = min(min(y[0], y[1]), min(y[2], y[3]));
        tmp_h2[segment] = max(max(y[0], y[1]), max(y[2], y[3]));

        /* Keep track of overall bounding cylinder. */

        xmin = min(xmin, tmp_r1[segment]);
        xmax = max(xmax, tmp_r2[segment]);

        ymin = min(ymin, tmp_h1[segment]);
        ymax = max(ymax, tmp_h2[segment]);

/*
        fprintf(stderr, "bound spline segment %d: ", i);
        fprintf(stderr, "r = %f - %f, h = %f - %f\n", tmp_r1[segment], tmp_r2[segment], tmp_h1[segment], tmp_h2[segment]);
*/

        /* Advance to next segment. */

        switch (Spline_Type)
        {
            case LINEAR_SPLINE:
            case QUADRATIC_SPLINE:
            case CUBIC_SPLINE:

                i++;

                break;

            case BEZIER_SPLINE:

                i += 4;

                break;
        }

        segment++;
    }

    Number = number_of_segments;

    /* Set overall bounding cylinder. */

    Radius1 = xmin;
    Radius2 = xmax;

    Height1 = ymin;
    Height2 = ymax;

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
*   Lathe       - Pointer to lathe structure
*   Ray         - Current ray
*   Depth_Stack - Current depth stack
*   d, w, n     - Intersection depth, parameter and segment number
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

bool Lathe::test_hit(const BasicRay &ray, IStack& Depth_Stack, DBL d, DBL w, int n, TraceThreadData *Thread)
{
    Vector3d IPoint;

    if ((d > DEPTH_TOLERANCE) && (d < MAX_DISTANCE))
    {
        IPoint = ray.Evaluate(d);

        if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
        {
            Depth_Stack->push(Intersection(d, IPoint, this, n, w));

            return(true);
        }
    }

    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Lathe_UVCoord
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

void Lathe::UVCoord(Vector2d& Result, const Intersection *Inter) const
{
    DBL len, theta;
    Vector3d P;
/*
    LATHE_SPLINE_ENTRY *Entry;

    Entry = &Spline->Entry[Inter->i1];
*/

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
    Result[V] = (Inter->d1 + (DBL)(Inter->i1))/Number;

    if (Result[V] > 1.0)
        Result[V] = 1.0;
}

}
// end of namespace pov
