//******************************************************************************
///
/// @file core/shape/box.cpp
///
/// Implementation of the box geometric primitive.
///
/// @author Alexander Enzmann
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/shape/box.h"

#include "base/pov_err.h"

#include "core/bounding/boundingbox.h"
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Minimal intersection depth. */

const DBL DEPTH_TOLERANCE = 1.0e-6;

/* Two values are equal if their difference is small than CLOSE_TOLERANCE. */

const DBL CLOSE_TOLERANCE = 1.0e-6;


/*****************************************************************************
*
* FUNCTION
*
*   All_Box_Intersections
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

bool Box::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int Intersection_Found;
    int Side1, Side2;
    DBL Depth1, Depth2;
    Vector3d IPoint;

    Thread->Stats()[Ray_Box_Tests]++;

    Intersection_Found = false;

    if (Intersect(ray, Trans, bounds[0], bounds[1], &Depth1, &Depth2, &Side1, &Side2))
    {
        if (Depth1 > DEPTH_TOLERANCE)
        {
            IPoint = ray.Evaluate(Depth1);

            if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
            {
                Depth_Stack->push(Intersection(Depth1,IPoint,this,Side1));

                Intersection_Found = true;
            }
        }

        IPoint = ray.Evaluate(Depth2);

        if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
        {
            Depth_Stack->push(Intersection(Depth2,IPoint,this,Side2));

            Intersection_Found = true;
        }
    }

    if (Intersection_Found)
        Thread->Stats()[Ray_Box_Tests_Succeeded]++;

    return (Intersection_Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Box
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
*   Sep 1994 : Added code to decide which side was hit in the case
*              intersection points are close to each other. This removes
*              some ugly artefacts one could observe at the corners of
*              boxes due to the usage of the wrong normal vector. [DB]
*
******************************************************************************/

bool Box::Intersect(const BasicRay& ray, const TRANSFORM *Trans, const Vector3d& Corner1, const Vector3d& Corner2, DBL *Depth1, DBL  *Depth2, int *Side1, int  *Side2)
{
    int smin = 0, smax = 0;    /* Side hit for min/max intersection. */
    DBL t, tmin, tmax;
    Vector3d P, D;

    /* Transform the point into the boxes space */

    if (Trans != NULL)
    {
        MInvTransPoint(P, ray.Origin, Trans);
        MInvTransDirection(D, ray.Direction, Trans);
    }
    else
    {
        P = ray.Origin;
        D = ray.Direction;
    }

    tmin = 0.0;
    tmax = BOUND_HUGE;

    /*
     * Sides first.
     */

    if (D[X] < -EPSILON)
    {
        t = (Corner1[X] - P[X]) / D[X];

        if (t < tmin) return(false);

        if (t <= tmax)
        {
            smax = kSideHit_X0;
            tmax = t;
        }

        t = (Corner2[X] - P[X]) / D[X];

        if (t >= tmin)
        {
            if (t > tmax) return(false);

            smin = kSideHit_X1;
            tmin = t;
        }
    }
    else
    {
        if (D[X] > EPSILON)
        {
            t = (Corner2[X] - P[X]) / D[X];

            if (t < tmin) return(false);

            if (t <= tmax)
            {
                smax = kSideHit_X1;
                tmax = t;
            }

            t = (Corner1[X] - P[X]) / D[X];

            if (t >= tmin)
            {
                if (t > tmax) return(false);

                smin = kSideHit_X0;
                tmin = t;
            }
        }
        else
        {
            if ((P[X] < Corner1[X]) || (P[X] > Corner2[X]))
            {
                return(false);
            }
        }
    }

    /*
     * Check Top/Bottom.
     */

    if (D[Y] < -EPSILON)
    {
        t = (Corner1[Y] - P[Y]) / D[Y];

        if (t < tmin) return(false);

        if (t <= tmax - CLOSE_TOLERANCE)
        {
            smax = kSideHit_Y0;
            tmax = t;
        }
        else
        {
            /*
             * If intersection points are close to each other find out
             * which side to use, i.e. is most probably hit. [DB 9/94]
             */

            if (t <= tmax + CLOSE_TOLERANCE)
            {
                if (-D[Y] > fabs(D[X])) smax = kSideHit_Y0;
            }
        }

        t = (Corner2[Y] - P[Y]) / D[Y];

        if (t >= tmin + CLOSE_TOLERANCE)
        {
            if (t > tmax) return(false);

            smin = kSideHit_Y1;
            tmin = t;
        }
        else
        {
            /*
             * If intersection points are close to each other find out
             * which side to use, i.e. is most probably hit. [DB 9/94]
             */

            if (t >= tmin - CLOSE_TOLERANCE)
            {
                if (-D[Y] > fabs(D[X])) smin = kSideHit_Y1;
            }
        }
    }
    else
    {
        if (D[Y] > EPSILON)
        {
            t = (Corner2[Y] - P[Y]) / D[Y];

            if (t < tmin) return(false);

            if (t <= tmax - CLOSE_TOLERANCE)
            {
                smax = kSideHit_Y1;
                tmax = t;
            }
            else
            {
                /*
                 * If intersection points are close to each other find out
                 * which side to use, i.e. is most probably hit. [DB 9/94]
                 */

                if (t <= tmax + CLOSE_TOLERANCE)
                {
                    if (D[Y] > fabs(D[X])) smax = kSideHit_Y1;
                }
            }

            t = (Corner1[Y] - P[Y]) / D[Y];

            if (t >= tmin + CLOSE_TOLERANCE)
            {
                if (t > tmax) return(false);

                smin = kSideHit_Y0;
                tmin = t;
            }
            else
            {
                /*
                 * If intersection points are close to each other find out
                 * which side to use, i.e. is most probably hit. [DB 9/94]
                 */

                if (t >= tmin - CLOSE_TOLERANCE)
                {
                    if (D[Y] > fabs(D[X])) smin = kSideHit_Y0;
                }
            }
        }
        else
        {
            if ((P[Y] < Corner1[Y]) || (P[Y] > Corner2[Y]))
            {
                return(false);
            }
        }
    }

    /* Now front/back */

    if (D[Z] < -EPSILON)
    {
        t = (Corner1[Z] - P[Z]) / D[Z];

        if (t < tmin) return(false);

        if (t <= tmax - CLOSE_TOLERANCE)
        {
            smax = kSideHit_Z0;
            tmax = t;
        }
        else
        {
            /*
             * If intersection points are close to each other find out
             * which side to use, i.e. is most probably hit. [DB 9/94]
             */

            if (t <= tmax + CLOSE_TOLERANCE)
            {
                switch (smax)
                {
                    case kSideHit_X0 :
                    case kSideHit_X1 : if (-D[Z] > fabs(D[X])) smax = kSideHit_Z0; break;

                    case kSideHit_Y0 :
                    case kSideHit_Y1 : if (-D[Z] > fabs(D[Y])) smax = kSideHit_Z0; break;
                }
            }
        }

        t = (Corner2[Z] - P[Z]) / D[Z];

        if (t >= tmin + CLOSE_TOLERANCE)
        {
            if (t > tmax) return(false);

            smin = kSideHit_Z1;
            tmin = t;
        }
        else
        {
            /*
             * If intersection points are close to each other find out
             * which side to use, i.e. is most probably hit. [DB 9/94]
             */

            if (t >= tmin - CLOSE_TOLERANCE)
            {
                switch (smin)
                {
                    case kSideHit_X0 :
                    case kSideHit_X1 : if (-D[Z] > fabs(D[X])) smin = kSideHit_Z1; break;

                    case kSideHit_Y0 :
                    case kSideHit_Y1 : if (-D[Z] > fabs(D[Y])) smin = kSideHit_Z1; break;
                }
            }
        }
    }
    else
    {
        if (D[Z] > EPSILON)
        {
            t = (Corner2[Z] - P[Z]) / D[Z];

            if (t < tmin) return(false);

            if (t <= tmax - CLOSE_TOLERANCE)
            {
                smax = kSideHit_Z1;
                tmax = t;
            }
            else
            {
                /*
                 * If intersection points are close to each other find out
                 * which side to use, i.e. is most probably hit. [DB 9/94]
                 */

                if (t <= tmax + CLOSE_TOLERANCE)
                {
                    switch (smax)
                    {
                        case kSideHit_X0 :
                        case kSideHit_X1 : if (D[Z] > fabs(D[X])) smax = kSideHit_Z1; break;

                        case kSideHit_Y0 :
                        case kSideHit_Y1 : if (D[Z] > fabs(D[Y])) smax = kSideHit_Z1; break;
                    }
                }
            }

            t = (Corner1[Z] - P[Z]) / D[Z];

            if (t >= tmin + CLOSE_TOLERANCE)
            {
                if (t > tmax) return(false);

                smin = kSideHit_Z0;
                tmin = t;
            }
            else
            {
                /*
                 * If intersection points are close to each other find out
                 * which side to use, i.e. is most probably hit. [DB 9/94]
                 */

                if (t >= tmin - CLOSE_TOLERANCE)
                {
                    switch (smin)
                    {
                        case kSideHit_X0 :
                        case kSideHit_X1 : if (D[Z] > fabs(D[X])) smin = kSideHit_Z0; break;

                        case kSideHit_Y0 :
                        case kSideHit_Y1 : if (D[Z] > fabs(D[Y])) smin = kSideHit_Z0; break;
                    }
                }
            }
        }
        else
        {
            if ((P[Z] < Corner1[Z]) || (P[Z] > Corner2[Z]))
            {
                return(false);
            }
        }
    }

    if (tmax < DEPTH_TOLERANCE)
    {
        return (false);
    }

    *Depth1 = tmin;
    *Depth2 = tmax;

    *Side1 = smin;
    *Side2 = smax;

    return(true);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Box
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

bool Box::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    Vector3d New_Point;

    /* Transform the point into box space. */

    if (Trans != NULL)
    {
        MInvTransPoint(New_Point, IPoint, Trans);
    }
    else
    {
        New_Point = IPoint;
    }

    /* Test to see if we are outside the box. */

    if ((New_Point[X] < bounds[0][X]) || (New_Point[X] > bounds[1][X]))
    {
        return (Test_Flag(this, INVERTED_FLAG));
    }

    if ((New_Point[Y] < bounds[0][Y]) || (New_Point[Y] > bounds[1][Y]))
    {
        return (Test_Flag(this, INVERTED_FLAG));
    }

    if ((New_Point[Z] < bounds[0][Z]) || (New_Point[Z] > bounds[1][Z]))
    {
        return (Test_Flag(this, INVERTED_FLAG));
    }

    /* Inside the box. */

    return (!Test_Flag(this, INVERTED_FLAG));
}



/*****************************************************************************
*
* FUNCTION
*
*   Box_Normal
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

void Box::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    switch (Inter->i1)
    {
        case kSideHit_X0: Result = Vector3d(-1.0,  0.0,  0.0); break;
        case kSideHit_X1: Result = Vector3d( 1.0,  0.0,  0.0); break;
        case kSideHit_Y0: Result = Vector3d( 0.0, -1.0,  0.0); break;
        case kSideHit_Y1: Result = Vector3d( 0.0,  1.0,  0.0); break;
        case kSideHit_Z0: Result = Vector3d( 0.0,  0.0, -1.0); break;
        case kSideHit_Z1: Result = Vector3d( 0.0,  0.0,  1.0); break;

        default: throw POV_EXCEPTION_STRING("Unknown box side in Box_Normal().");
    }

    /* Transform the point into the boxes space. */

    if (Trans != NULL)
    {
        MTransNormal(Result, Result, Trans);

        Result.normalize();
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Box
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

void Box::Translate(const Vector3d& Vector, const TRANSFORM *tr)
{
    if (Trans == NULL)
    {
        bounds[0] += Vector;

        bounds[1] += Vector;

        Compute_BBox();
    }
    else
    {
        Transform(tr);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Box
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

void Box::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Box
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

void Box::Scale(const Vector3d& Vector, const TRANSFORM *tr)
{
    DBL temp;

    if (Trans == NULL)
    {
        bounds[0] *= Vector;
        bounds[1] *= Vector;

        if (bounds[0][X] > bounds[1][X])
        {
            temp = bounds[0][X];

            bounds[0][X] = bounds[1][X];
            bounds[1][X] = temp;
        }

        if (bounds[0][Y] > bounds[1][Y])
        {
            temp = bounds[0][Y];

            bounds[0][Y] = bounds[1][Y];
            bounds[1][Y] = temp;
        }

        if (bounds[0][Z] > bounds[1][Z])
        {
            temp = bounds[0][Z];

            bounds[0][Z] = bounds[1][Z];
            bounds[1][Z] = temp;
        }

        Compute_BBox();
    }
    else
    {
        Transform(tr);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Box
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

void Box::Transform(const TRANSFORM *tr)
{
    if(Trans == NULL)
        Trans = Create_Transform();

    Compose_Transforms(Trans, tr);

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Box
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

Box::Box() : ObjectBase(BOX_OBJECT)
{
    bounds[0] = Vector3d(-1.0, -1.0, -1.0);
    bounds[1] = Vector3d( 1.0,  1.0,  1.0);

    Make_BBox(BBox, -1.0, -1.0, -1.0, 2.0, 2.0, 2.0);

    Trans = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Box
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

ObjectPtr Box::Copy()
{
    Box *New = new Box();
    Destroy_Transform(New->Trans);
    *New = *this;
    New->Trans = Copy_Transform(Trans);

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Box
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

Box::~Box()
{}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Box_BBox
*
* INPUT
*
*   Box - Box
*
* OUTPUT
*
*   Box
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a box.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Box::Compute_BBox()
{
    BBox.lowerLeft = BBoxVector3d(bounds[0]);

    BBox.size = BBoxVector3d(bounds[1] - bounds[0]);

    if (Trans != NULL)
    {
        Recompute_BBox(&BBox, Trans);
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Box_UVCoord
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Nathan Kopp, Lutz Kretzschmar
*
* DESCRIPTION
*
*        +-----+
*        ^  4  |
*        z     |
*  +-----+--x>-#--z>-+-<x--+
*  |     ^     |     |     |
*  |  1  y  5  |  2  |  6  |
*  |     |     |     |     |
*  +-----O--x>-+-----+-----+
*        |     |
*        |  3  |
*        +-----+
*
*  planes:
*  1: min x   2: max x
*  3: min y   4: max y
*  5: min z   6: max z
*
*  O : Origin
*  # : <1,1,0>
*
* CHANGES
*
*   The code was changed to use somthing similar to environmental cube mappping
*
*   1        +-----+           #
*            |     |
* V          z  4  |
*            |     |
*  .6  +--z>-+--x>-+-<z--+-<x--+
*      |     ^     |     |     |
*      |  1  y  5  |  2  |  6  |
*      |     |     |     |     |
*  .3  +-----+--x>-+-----+-----+
*            ^     |
*            z  3  |
*            |     |
*  0   O     +-----+
*
*      0    .25    .5   .75    1
*                            U
*
*  planes:
*  1: min x   2: max x
*  3: min y   4: max y
*  5: max z   6: min z
*
*  O : Origin of U,V map
*  # : <1,1,0>
*
******************************************************************************/

void Box::UVCoord(Vector2d& Result, const Intersection *Inter, TraceThreadData *Thread) const
{
    Vector3d P, Box_Diff;

    /* Transform the point into the cube's space */
    if (Trans != NULL)
        MInvTransPoint(P, Inter->IPoint, Trans);
    else
        P = Inter->IPoint;

    Box_Diff = bounds[1] - bounds[0];

    /* this line moves the bottom,left,front corner of the box to <0,0,0> */
    P -= bounds[0];
    /* this line normalizes the face offsets */
    P /= Box_Diff;

    /* The following code does a variation of cube environment mapping. All the
       textures are not mirrored when the cube is viewed from outside. */

    switch (Inter->i1)
    {
        case kSideHit_X0:
            Result[U] =               (P[Z] / 4.0);
            Result[V] = (1.0 / 3.0) + (P[Y] / 3.0);
            break;
        case kSideHit_X1:
            Result[U] = (3.0 / 4.0) - (P[Z] / 4.0);
            Result[V] = (1.0 / 3.0) + (P[Y] / 3.0);
            break;
        case kSideHit_Y0:
            Result[U] = (1.0 / 4.0) + (P[X] / 4.0);
            Result[V] =               (P[Z] / 3.0);
            break;
        case kSideHit_Y1:
            Result[U] = (1.0 / 4.0) + (P[X] / 4.0);
            Result[V] = (3.0 / 3.0) - (P[Z] / 3.0);
            break;
        case kSideHit_Z0:
            Result[U] =  1.0        - (P[X] / 4.0);
            Result[V] = (1.0 / 3.0) + (P[Y] / 3.0);
            break;
        case kSideHit_Z1:
            Result[U] = (1.0 / 4.0) + (P[X] / 4.0);
            Result[V] = (1.0 / 3.0) + (P[Y] / 3.0);
            break;

        default: throw POV_EXCEPTION_STRING("Unknown box side in Box_Normal().");
    }
}

bool Box::Intersect_BBox(BBoxDirection, const BBoxVector3d&, const BBoxVector3d&, BBoxScalar) const
{
    return true;
}

}
