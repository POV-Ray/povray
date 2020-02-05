//******************************************************************************
///
/// @file core/shape/prism.cpp
///
/// Implementation of the prism geometric primitive.
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
*    The prism primitive is defined by a set of points in 2d space which
*    are interpolated by linear, quadratic, or cubic splines. The resulting
*    2d curve is swept along a straight line to form the final prism object.
*
*    All calculations are done in the object's (u,v,w)-coordinate system
*    with the (w)-axis being the sweep axis.
*
*    One spline segment in the (u,v)-plane is given by the equations
*
*      fu(t) = Au * t * t * t + Bu * t * t + Cu * t + Du  and
*      fv(t) = Av * t * t * t + Bv * t * t + Cv * t + Dv,
*
*    with the parameter t ranging from 0 to 1.
*
*    To intersect a ray R = P + k * D transformed into the object's
*    coordinate system with the linear swept prism object, the equation
*
*      Dv * fu(t) - Du * fv(t) - (Pu * Dv - Pv * Du) = 0
*
*    has to be solved for t. For valid intersections (0 <= t <= 1)
*    the corresponding k can be calculated from equation
*
*      Pu + k * Du = fu(t) or Pv + k * Dv = fv(t).
*
*    In the case of conic sweeping the equation
*
*      (Pv * Dw - Pw * Dv) * fu(t) - (Pu * Dw - Pw * Du) * fv(t)
*                                  + (Pu * Dv - Pv * Du) = 0
*
*    has to be solved for t. For valid intersections (0 <= t <= 1)
*    the corresponding k can be calculated from equation
*
*      Pu + k * Du = (Pw + k * Dw) * fu(t) or
*      Pv + k * Dv = (Pw + k * Dw) * fv(t).
*
*    Note that the polynomal to solve has the same degree as the
*    spline segments used.
*
*    Note that Pu, Pv, Pw and Du, Dv, Dw denote the coordinates
*    of the vectors P and D.
*
*  Syntax:
*
*    prism {
*      [ linear_sweep | cubic_sweep ]
*      [ linear_spline | quadratic_spline | cubic_spline ]
*
*      height1, height2,
*      number_of_points
*
*      <P(0)>, <P(1)>, ..., <P(n-1)>
*
*      [ open ]
*      [ sturm ]
*    }
*
*    Note that the P(i) are 2d vectors.
*
*  ---
*
*  Ideas for the prism was taken from:
*
*    James T. Kajiya, "New techniques for ray tracing procedurally
*    defined objects", Computer Graphics, 17(3), July 1983, pp. 91-102
*
*    Kirk ...
*
*  ---
*
*  May 1994 : Creation. [DB]
*
*****************************************************************************/

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/shape/prism.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
#include "base/pov_err.h"

// POV-Ray header files (core module)
#include "core/bounding/boundingbox.h"
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

/* Part of the prism hit. */

const int BASE_HIT   = 1;
const int CAP_HIT    = 2;
const int SPLINE_HIT = 3;



/*****************************************************************************
*
* FUNCTION
*
*   All_Prism_Intersections
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
*   Determine ray/prism intersection and clip intersection found.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

bool Prism::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    bool Found = false ;
    Vector3d IPoint;
    int j, n;
    DBL k, u, v, w, h, len;
    DBL x[4];
    DBL y[3];
    DBL k1, k2, k3;
    Vector3d P, D;
    PRISM_SPLINE_ENTRY *Entry;
    DBL distance ;

    /* Don't test degenerate prisms. */
    if (Test_Flag(this, DEGENERATE_FLAG))
        return (false);

    Thread->Stats()[Ray_Prism_Tests]++;

    /* Transform the ray into the prism space */
    MInvTransPoint(P, ray.Origin, Trans);
    MInvTransDirection(D, ray.Direction, Trans);
    len = D.length();
    D /= len;

    /* Test overall bounding rectangle. */

#ifdef PRISM_EXTRA_STATS
    Thread->Stats()[Prism_Bound_Tests]++;
#endif

    if (((D[X] >= 0.0) && (P[X] > x2)) ||
        ((D[X] <= 0.0) && (P[X] < x1)) ||
        ((D[Z] >= 0.0) && (P[Z] > y2)) ||
        ((D[Z] <= 0.0) && (P[Z] < y1)))
    {
        return(false);
    }

#ifdef PRISM_EXTRA_STATS
    Thread->Stats()[Prism_Bound_Tests_Succeeded]++;
#endif

    /* Number of intersections found. */

    /* What kind of sweep is used? */

    switch (Sweep_Type)
    {
        case LINEAR_SWEEP :

            if (fabs(D[Y]) < EPSILON)
            {
                if ((P[Y] < Height1) || (P[Y] > Height2))
                    return(false);
            }
            else
            {
                if (Test_Flag(this, CLOSED_FLAG))
                {
                    /* Intersect ray with the cap-plane. */

                    k = (Height2 - P[Y]) / D[Y];

                    if ((k > DEPTH_TOLERANCE) && (k < MAX_DISTANCE))
                    {
                        u = P[X] + k * D[X];
                        v = P[Z] + k * D[Z];

                        if (in_curve(u, v, Thread->Stats()))
                        {
                            distance = k / len;
                            if ((distance > DEPTH_TOLERANCE) && (distance < MAX_DISTANCE))
                            {
                                IPoint = ray.Evaluate(distance);
                                if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                                {
                                    Depth_Stack->push (Intersection (distance, IPoint, this, CAP_HIT, 0, 0));
                                    Found = true;
                                }
                            }
                        }
                    }

                    /* Intersect ray with the base-plane. */

                    k = (Height1 - P[Y]) / D[Y];

                    if ((k > DEPTH_TOLERANCE) && (k < MAX_DISTANCE))
                    {
                        u = P[X] + k * D[X];
                        v = P[Z] + k * D[Z];

                        if (in_curve(u, v, Thread->Stats()))
                        {
                            distance = k / len;
                            if ((distance > DEPTH_TOLERANCE) && (distance < MAX_DISTANCE))
                            {
                                IPoint = ray.Evaluate(distance);
                                if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                                {
                                    Depth_Stack->push (Intersection (distance, IPoint, this, BASE_HIT, 0, 0));
                                    Found = true;
                                }
                            }
                        }
                    }
                }
            }

            /* Intersect ray with all spline segments. */

            // TODO - review the quick bailout condition.
            // While the test is quite simple, it may not kick in frequently enough to warrant the
            // effort, except in a rather special use case involving an orthographic camera.

            if ((fabs(D[X]) > EPSILON) || (fabs(D[Z]) > EPSILON)) // Quick bailout if ray is parallel to all sides
            {
                Entry = Spline->Entry;
                for (j = 0; j < Number; j++, Entry++)
                {
#ifdef PRISM_EXTRA_STATS
                    Thread->Stats()[Prism_Bound_Tests]++;
#endif
                    /* Test spline's bounding rectangle (modified Cohen-Sutherland). */
                    if (((D[X] >= 0.0) && (P[X] > Entry->x2)) ||
                        ((D[X] <= 0.0) && (P[X] < Entry->x1)) ||
                        ((D[Z] >= 0.0) && (P[Z] > Entry->y2)) ||
                        ((D[Z] <= 0.0) && (P[Z] < Entry->y1)))
                    {
                        continue;
                    }

                    /* Number of roots found. */
                    n = 0;
                    switch (Spline_Type)
                    {
                        case LINEAR_SPLINE :

#ifdef PRISM_EXTRA_STATS
                            Thread->Stats()[Prism_Bound_Tests_Succeeded]++;
#endif
                            /* Solve linear equation. */

                            x[0] = Entry->C[X] * D[Z] - Entry->C[Y] * D[X];
                            x[1] = D[Z] * (Entry->D[X] - P[X]) - D[X] * (Entry->D[Y] - P[Z]);
                            if (fabs(x[0]) > EPSILON)
                                y[n++] = -x[1] / x[0];
                            break;

                        case QUADRATIC_SPLINE :

#ifdef PRISM_EXTRA_STATS
                            Thread->Stats()[Prism_Bound_Tests_Succeeded]++;
#endif

                            /* Solve quadratic equation. */

                            x[0] = Entry->B[X] * D[Z] - Entry->B[Y] * D[X];
                            x[1] = Entry->C[X] * D[Z] - Entry->C[Y] * D[X];
                            x[2] = D[Z] * (Entry->D[X] - P[X]) - D[X] * (Entry->D[Y] - P[Z]);

                            n = Solve_Polynomial(2, x, y, false, 0.0, Thread->Stats());
                            break;

                        case CUBIC_SPLINE :
                        case BEZIER_SPLINE :
                            if (test_rectangle(P, D, Entry->x1, Entry->y1, Entry->x2, Entry->y2))
                            {
#ifdef PRISM_EXTRA_STATS
                                Thread->Stats()[Prism_Bound_Tests_Succeeded]++;
#endif

                                /* Solve cubic equation. */
                                x[0] = Entry->A[X] * D[Z] - Entry->A[Y] * D[X];
                                x[1] = Entry->B[X] * D[Z] - Entry->B[Y] * D[X];
                                x[2] = Entry->C[X] * D[Z] - Entry->C[Y] * D[X];
                                x[3] = D[Z] * (Entry->D[X] - P[X]) - D[X] * (Entry->D[Y] - P[Z]);
                                n = Solve_Polynomial(3, x, y, Test_Flag(this, STURM_FLAG), 0.0, Thread->Stats());
                            }
                            break;
                    }

                    /* Test roots for valid intersections. */
                    while (n--)
                    {
                        w = y[n];

                        if ((w >= 0.0) && (w <= 1.0))
                        {
                            if (fabs(D[X]) > EPSILON)
                            {
                                k = (w * (w * (w * Entry->A[X] + Entry->B[X]) + Entry->C[X]) + Entry->D[X] - P[X]) / D[X];
                            }
                            else
                            {
                                k = (w * (w * (w * Entry->A[Y] + Entry->B[Y]) + Entry->C[Y]) + Entry->D[Y] - P[Z]) / D[Z];
                            }

                            /* Verify that intersection height is valid. */
                            h = P[Y] + k * D[Y];
                            if ((h >= Height1) && (h <= Height2))
                            {
                                distance = k / len;
                                if ((distance > DEPTH_TOLERANCE) && (distance < MAX_DISTANCE))
                                {
                                    IPoint = ray.Evaluate(distance);
                                    if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                                    {
                                        Depth_Stack->push (Intersection (distance, IPoint, this, SPLINE_HIT, j, w));
                                        Found = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            break;

        /*************************************************************************
        * Conic sweep.
        **************************************************************************/

        case CONIC_SWEEP :

            if (fabs(D[Y]) < EPSILON)
            {
                if ((P[Y] < Height1) || (P[Y] > Height2))
                    return(false);
            }
            else
            {
                if (Test_Flag(this, CLOSED_FLAG))
                {
                    /* Intersect ray with the cap-plane. */
                    if (fabs(Height2) > EPSILON)
                    {
                        k = (Height2 - P[Y]) / D[Y];

                        if ((k > DEPTH_TOLERANCE) && (k < MAX_DISTANCE))
                        {
                            u = (P[X] + k * D[X]) / Height2;
                            v = (P[Z] + k * D[Z]) / Height2;

                            if (in_curve(u, v, Thread->Stats()))
                            {
                                distance = k / len;
                                if ((distance > DEPTH_TOLERANCE) && (distance < MAX_DISTANCE))
                                {
                                    IPoint = ray.Evaluate(distance);
                                    if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                                    {
                                        Depth_Stack->push (Intersection (distance, IPoint, this, CAP_HIT, 0, 0));
                                        Found = true;
                                    }
                                }
                            }
                        }
                    }

                    /* Intersect ray with the base-plane. */

                    if (fabs(Height1) > EPSILON)
                    {
                        k = (Height1 - P[Y]) / D[Y];

                        if ((k > DEPTH_TOLERANCE) && (k < MAX_DISTANCE))
                        {
                            u = (P[X] + k * D[X]) / Height1;
                            v = (P[Z] + k * D[Z]) / Height1;

                            if (in_curve(u, v, Thread->Stats()))
                            {
                                distance = k / len;
                                if ((distance > DEPTH_TOLERANCE) && (distance < MAX_DISTANCE))
                                {
                                    IPoint = ray.Evaluate(distance);
                                    if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                                    {
                                        Depth_Stack->push (Intersection (distance, IPoint, this, BASE_HIT, 0, 0));
                                        Found = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            /* Precompute ray-only dependant constants. */
            k1 = P[Z] * D[Y] - P[Y] * D[Z];
            k2 = P[Y] * D[X] - P[X] * D[Y];
            k3 = P[X] * D[Z] - P[Z] * D[X];

            /* Intersect ray with the spline segments. */

            // NB: Unlike in the LINEAR_SWEEP case, the ray can't be parallel to each and every side
            // (except in pathological cases), so there's no quick bailout test for that here.
            // (The equivalent test would be whether the ray runs through the apex point, but that
            // test is more complex, and there's also no clear-cut use case where it could be
            // expected to kick in frequently enough to be of any benefit.)

            Entry = Spline->Entry ;
            for (j = 0; j < Number; j++, Entry++)
            {
                /* Test spline's bounding rectangle (modified Cohen-Sutherland). */
                if (((D[X] >= 0.0) && (P[X] > Entry->x2)) ||
                    ((D[X] <= 0.0) && (P[X] < Entry->x1)) ||
                    ((D[Z] >= 0.0) && (P[Z] > Entry->y2)) ||
                    ((D[Z] <= 0.0) && (P[Z] < Entry->y1)))
                {
                    continue;
                }

                /* Number of roots found. */

                n = 0;
                switch (Spline_Type)
                {
                    case LINEAR_SPLINE :

                        /* Solve linear equation. */
                        x[0] = Entry->C[X] * k1 + Entry->C[Y] * k2;
                        x[1] = Entry->D[X] * k1 + Entry->D[Y] * k2 + k3;

                        if (fabs(x[0]) > EPSILON)
                            y[n++] = -x[1] / x[0];
                        break;

                    case QUADRATIC_SPLINE :

                        /* Solve quadratic equation. */
                        x[0] = Entry->B[X] * k1 + Entry->B[Y] * k2;
                        x[1] = Entry->C[X] * k1 + Entry->C[Y] * k2;
                        x[2] = Entry->D[X] * k1 + Entry->D[Y] * k2 + k3;

                        n = Solve_Polynomial(2, x, y, false, 0.0, Thread->Stats());
                        break;

                    case CUBIC_SPLINE :
                    case BEZIER_SPLINE :

                        /* Solve cubic equation. */
                        x[0] = Entry->A[X] * k1 + Entry->A[Y] * k2;
                        x[1] = Entry->B[X] * k1 + Entry->B[Y] * k2;
                        x[2] = Entry->C[X] * k1 + Entry->C[Y] * k2;
                        x[3] = Entry->D[X] * k1 + Entry->D[Y] * k2 + k3;

                        n = Solve_Polynomial(3, x, y, Test_Flag(this, STURM_FLAG), 0.0, Thread->Stats());
                        break;
                }

                /* Test roots for valid intersections. */

                while (n--)
                {
                    w = y[n];

                    if ((w >= 0.0) && (w <= 1.0))
                    {
                        k = w * (w * (w * Entry->A[X] + Entry->B[X]) + Entry->C[X]) + Entry->D[X];
                        h = D[X] - k * D[Y];

                        if (fabs(h) > EPSILON)
                        {
                            k = (k * P[Y] - P[X]) / h;
                        }
                        else
                        {
                            k = w * (w * (w * Entry->A[Y] + Entry->B[Y]) + Entry->C[Y]) + Entry->D[Y];

                            h = D[Z] - k * D[Y];

                            if (fabs(h) > EPSILON)
                            {
                                k = (k * P[Y] - P[Z]) / h;
                            }
                            else
                            {
                                /* This should never happen! */
                                continue;
                            }
                        }

                        /* Verify that intersection height is valid. */
                        h = P[Y] + k * D[Y];
                        if ((h >= Height1) && (h <= Height2))
                        {
                            distance = k / len;
                            if ((distance > DEPTH_TOLERANCE) && (distance < MAX_DISTANCE))
                            {
                                IPoint = ray.Evaluate(distance);
                                if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                                {
                                    Depth_Stack->push (Intersection (distance, IPoint, this, SPLINE_HIT, j, w));
                                    Found = true;
                                }
                            }
                        }
                    }
                }
            }
            break;

        default:
            throw POV_EXCEPTION_STRING("Unknown sweep type in intersect_prism().");
    }

    if (Found)
        Thread->Stats()[Ray_Prism_Tests_Succeeded]++;
    return(Found);
}

/*****************************************************************************
*
* FUNCTION
*
*   Inside_Prism
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
*   Test if point lies inside a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

bool Prism::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    Vector3d P;

    /* Transform the point into the prism space. */

    MInvTransPoint(P, IPoint, Trans);

    if ((P[Y] >= Height1) && (P[Y] < Height2))
    {
        if (Sweep_Type == CONIC_SWEEP)
        {
            /* Scale x and z coordinate. */

            if (fabs(P[Y]) > EPSILON)
            {
                P[X] /= P[Y];
                P[Z] /= P[Y];
            }
            else
            {
                P[X] = P[Z] = HUGE_VAL;
            }
        }

        if (in_curve(P[X], P[Z], Thread->Stats()))
        {
            return(!Test_Flag(this, INVERTED_FLAG));
        }
    }

    return(Test_Flag(this, INVERTED_FLAG));
}



/*****************************************************************************
*
* FUNCTION
*
*   Prism_Normal
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
*   Calculate the normal of the prism in a given point.
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Jul 1997 : Fixed bug as reported by Darko Rozic. [DB]
*
******************************************************************************/

void Prism::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    Vector3d P;
    PRISM_SPLINE_ENTRY Entry;
    Vector3d N;

    switch (Inter->i1)
    {
        case BASE_HIT:

            N = Vector3d(0.0, -1.0, 0.0);

            break;

        case CAP_HIT:

            N = Vector3d(0.0, 1.0, 0.0);

            break;

        case SPLINE_HIT:

            Entry = Spline->Entry[Inter->i2];

            switch (Sweep_Type)
            {
                case LINEAR_SWEEP:

                    N[X] =   Inter->d1 * (3.0 * Entry.A[Y] * Inter->d1 + 2.0 * Entry.B[Y]) + Entry.C[Y];
                    N[Y] =   0.0;
                    N[Z] = -(Inter->d1 * (3.0 * Entry.A[X] * Inter->d1 + 2.0 * Entry.B[X]) + Entry.C[X]);

                    break;

                case CONIC_SWEEP:

                    /* Transform the point into the prism space. */

                    MInvTransPoint(P, Inter->IPoint, Trans);

                    if (fabs(P[Y]) > EPSILON)
                    {
                        N[X] =   Inter->d1 * (3.0 * Entry.A[Y] * Inter->d1 + 2.0 * Entry.B[Y]) + Entry.C[Y];
                        N[Z] = -(Inter->d1 * (3.0 * Entry.A[X] * Inter->d1 + 2.0 * Entry.B[X]) + Entry.C[X]);
                        N[Y] = -(P[X] * N[X] + P[Z] * N[Z]) / P[Y];
                    }

                    break;

                default:

                    throw POV_EXCEPTION_STRING("Unknown sweep type in Prism::Normal().");
            }

            break;

        default:

            throw POV_EXCEPTION_STRING("Unknown side code in Prism::Normal().");
    }

    /* Transform the normal out of the prism space. */

    MTransNormal(Result, N, Trans);

    Result.normalize();
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Prism
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
*   Translate a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Prism::Translate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Prism
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
*   Rotate a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Prism::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Prism
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
*   Scale a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Prism::Scale(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Prism
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
*   Transform a prism and recalculate its bounding box.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Prism::Transform(const TRANSFORM *tr)
{
    Compose_Transforms(Trans, tr);

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Prism
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   PRISM * - new prism
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a new prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

Prism::Prism() : ObjectBase(PRISM_OBJECT)
{
    Trans = Create_Transform();

    x1      = 0.0;
    x2      = 0.0;
    y1      = 0.0;
    y2      = 0.0;
    u1      = 0.0;
    u2      = 0.0;
    v1      = 0.0;
    v2      = 0.0;
    Height1 = 0.0;
    Height2 = 0.0;

    Number = 0;

    Spline_Type = LINEAR_SPLINE;
    Sweep_Type  = LINEAR_SWEEP;

    Spline = nullptr;

    Set_Flag(this, CLOSED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Prism
*
* INPUT
*
*   Object - Object
*
* OUTPUT
*
* RETURNS
*
*   void * - New prism
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a prism structure.
*
*   NOTE: The splines are not copied, only the number of references is
*         counted, so that Destray_Prism() knows if they can be destroyed.
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Sep 1994 : fixed memory leakage [DB]
*
******************************************************************************/

ObjectPtr Prism::Copy()
{
    Prism *New = new Prism();
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
*   Destroy_Prism
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
*   Destroy a prism.
*
*   NOTE: The splines are destroyed if they are no longer used by any copy.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

Prism::~Prism()
{
    if (--(Spline->References) == 0)
    {
        POV_FREE(Spline->Entry);
        POV_FREE(Spline);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Prism_BBox
*
* INPUT
*
*   Prism - Prism
*
* OUTPUT
*
*   Prism
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Prism::Compute_BBox()
{
    Make_BBox(BBox, x1, Height1, y1,
        x2 - x1, Height2 - Height1, y2 - y1);

    Recompute_BBox(&BBox, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   in_curve
*
* INPUT
*
*   Prism - Prism to test
*   u, v  - Coordinates
*
* OUTPUT
*
* RETURNS
*
*   int - true if inside
*
* AUTHOR
*
*   Dieter Bayer, June 1994
*
* DESCRIPTION
*
*   Test if a 2d point lies inside a prism's spline curve.
*
*   We travel from the current point in positive u direction
*   and count the number of crossings with the curve.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

int Prism::in_curve(DBL u, DBL v, RenderStatistics& stats) const
{
    int i, n, NC;
    DBL k, w;
    DBL x[4];
    DBL y[3];
    PRISM_SPLINE_ENTRY Entry;

    NC = 0;

    /* First test overall bounding rectangle. */

    if ((u >= u1) && (u <= u2) &&
        (v >= v1) && (v <= v2))
    {
        for (i = 0; i < Number; i++)
        {
            Entry = Spline->Entry[i];

            /* Test if current segment can be hit. */

            if ((v >= Entry.v1) && (v <= Entry.v2) && (u <= Entry.u2))
            {
                x[0] = Entry.A[Y];
                x[1] = Entry.B[Y];
                x[2] = Entry.C[Y];
                x[3] = Entry.D[Y] - v;

                n = Solve_Polynomial(3, x, y, Test_Flag(this, STURM_FLAG), 0.0, stats);

                while (n--)
                {
                    w = y[n];

                    if ((w >= 0.0) && (w <= 1.0))
                    {
                        k  = w * (w * (w * Entry.A[X] + Entry.B[X]) + Entry.C[X]) + Entry.D[X] - u;

                        if (k >= 0.0)
                        {
                            NC++;
                        }
                    }
                }
            }
        }
    }

    return(NC & 1);
}



/*****************************************************************************
*
* FUNCTION
*
*   test_rectangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer, July 1994
*
* DESCRIPTION
*
*   Test if the 2d ray (= P + t * D) intersects a rectangle.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

bool Prism::test_rectangle(const Vector3d& P, const Vector3d& D, DBL x1, DBL z1, DBL x2, DBL z2)
{
    DBL dmin, dmax, tmin, tmax;

    if (fabs(D[X]) > EPSILON)
    {
        if (D[X] > 0.0)
        {
            dmin = (x1 - P[X]) / D[X];
            dmax = (x2 - P[X]) / D[X];

            if (dmax < EPSILON)
            {
                return(false);
            }
        }
        else
        {
            dmax = (x1 - P[X]) / D[X];

            if (dmax < EPSILON)
            {
                return(false);
            }

            dmin = (x2 - P[X]) / D[X];
        }

        if (dmin > dmax)
        {
            return(false);
        }
    }
    else
    {
        if ((P[X] < x1) || (P[X] > x2))
        {
            return(false);
        }
        else
        {
            dmin = -BOUND_HUGE;
            dmax =  BOUND_HUGE;
        }
    }

    if (fabs(D[Z]) > EPSILON)
    {
        if (D[Z] > 0.0)
        {
            tmin = (z1 - P[Z]) / D[Z];
            tmax = (z2 - P[Z]) / D[Z];
        }
        else
        {
            tmax = (z1 - P[Z]) / D[Z];
            tmin = (z2 - P[Z]) / D[Z];
        }

        if (tmax < dmax)
        {
            if (tmax < EPSILON)
            {
                return(false);
            }

            if (tmin > dmin)
            {
                if (tmin > tmax)
                {
                    return(false);
                }

                dmin = tmin;
            }
            else
            {
                if (dmin > tmax)
                {
                    return(false);
                }
            }

            /*dmax = tmax; */ /*not needed CEY[1/95]*/
        }
        else
        {
            if (tmin > dmin)
            {
                if (tmin > dmax)
                {
                    return(false);
                }

                /* dmin = tmin; */  /*not needed CEY[1/95]*/
            }
        }
    }
    else
    {
        if ((P[Z] < z1) || (P[Z] > z2))
        {
            return(false);
        }
    }

    return(true);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Prism
*
* INPUT
*
*   Prism - Prism
*   P     - Points defining prism
*
* OUTPUT
*
*   Prism
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer, June 1994
*
* DESCRIPTION
*
*   Calculate the spline segments of a prism from a set of points.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Prism::Compute_Prism(Vector2d *P, RenderStatistics& stats)
{
    int i, n, number_of_splines;
    int i1, i2, i3;

    DBL x[4], y[4];
    DBL xmin, xmax, ymin, ymax;
    DBL c[3];
    DBL r[2];

    Vector2d A, B, C, D, First;

    /* Allocate Object->Number segments. */

    if (Spline == nullptr)
    {
        Spline = reinterpret_cast<PRISM_SPLINE *>(POV_MALLOC(sizeof(PRISM_SPLINE), "spline segments of prism"));
        Spline->References = 1;
        Spline->Entry = reinterpret_cast<PRISM_SPLINE_ENTRY *>(POV_MALLOC(Number*sizeof(PRISM_SPLINE_ENTRY), "spline segments of prism"));
    }
    else
    {
        /* This should never happen! */
        throw POV_EXCEPTION_STRING("Prism segments are already defined.");
    }

    /***************************************************************************
  * Calculate segments.
  ****************************************************************************/

    /* We want to know the size of the overall bounding rectangle. */

    xmax = ymax = -BOUND_HUGE;
    xmin = ymin =  BOUND_HUGE;

    /* Get first segment point. */

    switch (Spline_Type)
    {
        case LINEAR_SPLINE:

            First = P[0];

            break;

        case QUADRATIC_SPLINE:
        case CUBIC_SPLINE:

            First = P[1];

            break;
    }

    for (i = number_of_splines = 0; i < Number-1; i++)
    {
        /* Get indices of previous and next two points. */

        i1 = i + 1;
        i2 = i + 2;
        i3 = i + 3;

        switch (Spline_Type)
        {
            /*************************************************************************
            * Linear spline (nothing more than a simple polygon).
            **************************************************************************/

            case LINEAR_SPLINE :

                if (i1 >= Number)
                {
                    throw POV_EXCEPTION_STRING("Too few points in prism. Prism not closed? Control points missing?");
                }

                /* Use linear interpolation. */

                A =  Vector2d(0.0);
                B =  Vector2d(0.0);
                C = -1.0 * P[i] + 1.0 * P[i1];
                D =  1.0 * P[i];

                x[0] = x[2] = P[i][X];
                x[1] = x[3] = P[i1][X];

                y[0] = y[2] = P[i][Y];
                y[1] = y[3] = P[i1][Y];

                break;

            /*************************************************************************
            * Quadratic spline.
            **************************************************************************/

            case QUADRATIC_SPLINE :

                if (i2 >= Number)
                {
                    throw POV_EXCEPTION_STRING("Too few points in prism.");
                }

                /* Use quadratic interpolation. */

                A =  Vector2d(0.0);
                B =  0.5 * P[i] - 1.0 * P[i1] + 0.5 * P[i2];
                C = -0.5 * P[i]               + 0.5 * P[i2];
                D =               1.0 * P[i1];

                x[0] = x[2] = P[i1][X];
                x[1] = x[3] = P[i2][X];

                y[0] = y[2] = P[i1][Y];
                y[1] = y[3] = P[i2][Y];

                break;

            /*************************************************************************
            * Cubic spline.
            **************************************************************************/

            case CUBIC_SPLINE :

                if (i3 >= Number)
                {
                    throw POV_EXCEPTION_STRING("Too few points in prism.");
                }

                /* Use cubic interpolation. */

                A = -0.5 * P[i] + 1.5 * P[i1] - 1.5 * P[i2] + 0.5 * P[i3];
                B =        P[i] - 2.5 * P[i1] + 2.0 * P[i2] - 0.5 * P[i3];
                C = -0.5 * P[i]               + 0.5 * P[i2];
                D =                     P[i1];

                x[0] = x[2] = P[i1][X];
                x[1] = x[3] = P[i2][X];

                y[0] = y[2] = P[i1][Y];
                y[1] = y[3] = P[i2][Y];

                break;

            /*************************************************************************
            * Bezier spline.
            **************************************************************************/

            case BEZIER_SPLINE :

                if (i3 >= Number)
                {
                    throw POV_EXCEPTION_STRING("Too few points in prism. Prism not closed? Control points missing?");
                }

                /* Use Bernstein blending function interpolation. */

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

                throw POV_EXCEPTION_STRING("Unknown spline type in Compute_Prism().");
        }

        Spline->Entry[number_of_splines].A = A;
        Spline->Entry[number_of_splines].B = B;
        Spline->Entry[number_of_splines].C = C;
        Spline->Entry[number_of_splines].D = D;

        if ((Spline_Type == QUADRATIC_SPLINE) ||
            (Spline_Type == CUBIC_SPLINE))
        {
            /* Get maximum coordinates in current segment. */

            c[0] = 3.0 * A[X];
            c[1] = 2.0 * B[X];
            c[2] =       C[X];

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
            c[2] =       C[Y];

            n = Solve_Polynomial(2, c, r, false, 0.0, stats);

            while (n--)
            {
                if ((r[n] >= 0.0) && (r[n] <= 1.0))
                {
                    y[n] = r[n] * (r[n] * (r[n] * A[Y] + B[Y]) + C[Y]) + D[Y];
                }
            }
        }

        /* Set current segment's bounding rectangle. */

        Spline->Entry[number_of_splines].x1 = min(min(x[0], x[1]), min(x[2], x[3]));

        Spline->Entry[number_of_splines].x2 =
        Spline->Entry[number_of_splines].u2 = max(max(x[0], x[1]), max(x[2], x[3]));

        Spline->Entry[number_of_splines].y1 =
        Spline->Entry[number_of_splines].v1 = min(min(y[0], y[1]), min(y[2], y[3]));

        Spline->Entry[number_of_splines].y2 =
        Spline->Entry[number_of_splines].v2 = max(max(y[0], y[1]), max(y[2], y[3]));

        /* Keep track of overall bounding rectangle. */

        xmin = min(xmin, Spline->Entry[number_of_splines].x1);
        xmax = max(xmax, Spline->Entry[number_of_splines].x2);

        ymin = min(ymin, Spline->Entry[number_of_splines].y1);
        ymax = max(ymax, Spline->Entry[number_of_splines].y2);

        number_of_splines++;

        /* Advance to next segment. */

        switch (Spline_Type)
        {
            case LINEAR_SPLINE:

                if ((fabs(P[i1][X] - First[X]) < EPSILON) &&
                    (fabs(P[i1][Y] - First[Y]) < EPSILON))
                {
                    i++;

                    if (i+1 < Number)
                    {
                        First = P[i+1];
                    }
                }

                break;

            case QUADRATIC_SPLINE:

                if ((fabs(P[i2][X] - First[X]) < EPSILON) &&
                    (fabs(P[i2][Y] - First[Y]) < EPSILON))
                {
                    i += 2;

                    if (i+2 < Number)
                    {
                        First = P[i+2];
                    }
                }

                break;

            case CUBIC_SPLINE:

                if ((fabs(P[i2][X] - First[X]) < EPSILON) &&
                    (fabs(P[i2][Y] - First[Y]) < EPSILON))
                {
                    i += 3;

                    if (i+2 < Number)
                    {
                        First = P[i+2];
                    }
                }

                break;

            case BEZIER_SPLINE:

                i += 3;

                break;
        }
    }

    Number = number_of_splines;

    /* Set overall bounding rectangle. */

    x1 =
    u1 = xmin;
    x2 =
    u2 = xmax;

    y1 =
    v1 = ymin;
    y2 =
    v2 = ymax;

    if (Sweep_Type == CONIC_SWEEP)
    {
        /* Recalculate bounding rectangles. */

        for (i = 0; i < Number; i++)
        {
            x[0] = Spline->Entry[i].x1 * Height1;
            x[1] = Spline->Entry[i].x1 * Height2;
            x[2] = Spline->Entry[i].x2 * Height1;
            x[3] = Spline->Entry[i].x2 * Height2;

            xmin = min(min(x[0], x[1]), min(x[2], x[3]));
            xmax = max(max(x[0], x[1]), max(x[2], x[3]));

            Spline->Entry[i].x1 = xmin;
            Spline->Entry[i].x2 = xmax;

            y[0] = Spline->Entry[i].y1 * Height1;
            y[1] = Spline->Entry[i].y1 * Height2;
            y[2] = Spline->Entry[i].y2 * Height1;
            y[3] = Spline->Entry[i].y2 * Height2;

            ymin = min(min(y[0], y[1]), min(y[2], y[3]));
            ymax = max(max(y[0], y[1]), max(y[2], y[3]));

            Spline->Entry[i].y1 = ymin;
            Spline->Entry[i].y2 = ymax;
        }

        /* Recalculate overall bounding rectangle. */

        x[0] = x1 * Height1;
        x[1] = x1 * Height2;
        x[2] = x2 * Height1;
        x[3] = x2 * Height2;

        xmin = min(min(x[0], x[1]), min(x[2], x[3]));
        xmax = max(max(x[0], x[1]), max(x[2], x[3]));

        x1 = xmin;
        x2 = xmax;

        y[0] = y1 * Height1;
        y[1] = y1 * Height2;
        y[2] = y2 * Height1;
        y[3] = y2 * Height2;

        ymin = min(min(y[0], y[1]), min(y[2], y[3]));
        ymax = max(max(y[0], y[1]), max(y[2], y[3]));

        y1 = ymin;
        y2 = ymax;
    }
}

}
// end of namespace pov
