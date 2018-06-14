//******************************************************************************
///
/// @file core/shape/polyline.cpp
///
/// Implementation of the winding polyline geometric primitive.
///
/// @author Jérôme Grimbert
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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
*  Syntax:
*
*    polyline { \(Px [,]\)\{3,n\} range { \(<low,high>\)\{1,m\} } OBJECT_MODIFIERS }
*
*    n coplanars points Px 
*    <low,high> specification of interval for winding number
*
*  ---
*
*  Jun 2018 : Creation. [JG]
*
*****************************************************************************/

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/shape/polyline.h"

#include <algorithm>

#include "base/pov_err.h"

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

/* Minimal intersection depth for a valid intersection. */
const DBL DEPTH_TOLERANCE = 1.0e-8;

/* If |x| < ZERO_TOLERANCE x is assumed to be 0. */
const DBL ZERO_TOLERANCE = 1.0e-10;



/*****************************************************************************
*
* FUNCTION
*
*   All_Intersections
*
* INPUT
*
*   Object      - Object
*   Ray         - Ray
*   Depth_Stack - Intersection stack
*   Thread      - local data of the thread
*
* OUTPUT
*
*   Depth_Stack
*   Thread
*
* RETURNS
*
*   int - true, if a intersection was found
*
* AUTHOR
*
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Determine ray/polyline intersection and clip intersection found.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

bool Polyline::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    DBL Depth;
    Vector3d IPoint;

    if (Intersect(ray, &Depth, Thread))
    {
        IPoint = ray.Evaluate(Depth);

        if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
        {
            Depth_Stack->push(Intersection(Depth, IPoint, this));

            return(true);
        }
    }

    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect
*
* INPUT
*
*   Ray     - Ray
*   Polyg - Polyline
*   Depth   - Depth of intersection found
*   Thread      - local data of the thread
*
* OUTPUT
*
*   Depth
*   Thread
*
* RETURNS
*
*   int - true if intersection found
*
* AUTHOR
*
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Determine ray/polyline intersection.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

bool Polyline::Intersect(const BasicRay& ray, DBL *Depth, TraceThreadData *Thread) const
{
    DBL x, y, len;
    Vector3d p, d;

    /* Don't test degenerate polylines. */

    if (Test_Flag(this, DEGENERATE_FLAG))
        return(false);

    Thread->Stats()[Ray_Polyline_Tests]++;

    /* Transform the ray into the polyline space. */

    MInvTransPoint(p, ray.Origin, Trans);

    MInvTransDirection(d, ray.Direction, Trans);

    len = d.length();

    d /= len;

    /* Intersect ray with the plane in which the polyline lies. */

    if (fabs(d[Z]) < ZERO_TOLERANCE)
        return(false);

    *Depth = -p[Z] / d[Z];

    if ((*Depth < DEPTH_TOLERANCE) || (*Depth > MAX_DISTANCE))
        return(false);

    /* Does the intersection point lie inside the polyline? */

    x = p[X] + *Depth * d[X];
    y = p[Y] + *Depth * d[Y];

    if (in_polyline(x, y))
    {
        Thread->Stats()[Ray_Polyline_Tests_Succeeded]++;

        *Depth /= len;

        return (true);
    }
    else
        return (false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Polyline
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
*   int - always false
*
* AUTHOR
*
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Polylines can't be used in CSG.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

bool Polyline::Inside(const Vector3d&, TraceThreadData *Thread) const
{
    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Polyline_Normal
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
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Calculate the normal of the polyline in a given point.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

void Polyline::Normal(Vector3d& Result, Intersection *, TraceThreadData *) const
{
    Result = S_Normal;
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Polyline
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
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Translate a polyline.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

void Polyline::Translate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Polyline
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
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Rotate a polyline.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

void Polyline::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Polyline
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
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Scale a polyline.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

void Polyline::Scale(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Polyline
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
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Transform a polyline by transforming all points
*   and recalculating the polyline.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

void Polyline::Transform(const TRANSFORM *tr)
{
    Vector3d N;

    if(Trans == NULL)
        Trans = Create_Transform();

    Compose_Transforms(Trans, tr);

    N = Vector3d(0.0, 0.0, 1.0);
    MTransNormal(S_Normal, N, Trans);

    S_Normal.normalize();

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Polyline
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   POLYLINE * - new polyline
*
* AUTHOR
*
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Create a new polyline.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

Polyline::Polyline() : NonsolidObject(POLYLINE_OBJECT)
{
    Trans = Create_Transform();

    S_Normal = Vector3d(0.0, 0.0, 1.0);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Polyline
*
* INPUT
*
*   Object - Object
*
* OUTPUT
*
* RETURNS
*
*   void * - New polyline
*
* AUTHOR
*
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Copy a polyline structure.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

ObjectPtr Polyline::Copy()
{
    Polyline *New = new Polyline();

    Destroy_Transform(New->Trans);
    *New = *this;
    New->Trans = Copy_Transform(Trans);
    New->Data = Data;

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Polyline
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
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Destroy a polyline.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

Polyline::~Polyline()
{
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Polyline
*
* INPUT
*
*   Polyg - Polyline
*   Points  - 3D points describing the polyline
*
* OUTPUT
*
*   Polyg
*
* RETURNS
* 
*   true in case of problem
*
* AUTHOR
*
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Compute the following things for a given polyline:
*
*     - Polyline transformation
*
*     - Array of 2d points describing the shape of the polyline
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

bool Polyline::Compute_Polyline(std::vector<Vector3d>& points, std::vector<bool>& range)
{
    int i;
    DBL x, y, z, d;
    Vector3d o, u, v, w, N;
    MATRIX a, b;

    /* Create polyline data. */

    if (Data.get() == nullptr)
    {
        Data = std::make_shared<POLYLINE_DATA>();
        Data->Points.resize( points.size() );
        Data->SelectedWindingNumber.swap( range );
    }
    else
    {
        throw POV_EXCEPTION_STRING("Polyline data already computed.");
    }

    /* Get polyline's coordinate system (one of the many possible) */

    o = points[0];

    /* Find valid, i.e. non-zero u vector. */

    for (i = 1; i < points.size(); i++)
    {
        u = points[i] - o;

        if (u.lengthSqr() > EPSILON)
        {
            break;
        }
    }

    if (i == points.size())
    {
        Set_Flag(this, DEGENERATE_FLAG);
        return true;
    }

    /* Find valid, i.e. non-zero v and w vectors. */

    for (i++; i < points.size(); i++)
    {
        v = points[i] - o;

        w = cross(u, v);

        if ((v.lengthSqr() > EPSILON) && (w.lengthSqr() > EPSILON))
        {
            break;
        }
    }

    if (i == points.size())
    {
        Set_Flag(this, DEGENERATE_FLAG);
        return true;
    }

    u = cross(v, w);
    v = cross(w, u);

    u.normalize();
    v.normalize();
    w.normalize();

    MIdentity(a);
    MIdentity(b);

    a[3][0] = -o[X];
    a[3][1] = -o[Y];
    a[3][2] = -o[Z];

    b[0][0] =  u[X];
    b[1][0] =  u[Y];
    b[2][0] =  u[Z];

    b[0][1] =  v[X];
    b[1][1] =  v[Y];
    b[2][1] =  v[Z];

    b[0][2] =  w[X];
    b[1][2] =  w[Y];
    b[2][2] =  w[Z];

    MTimesC(Trans->inverse, a, b);

    MInvers(Trans->matrix, Trans->inverse);

    /* Project points onto the u,v-plane (3D --> 2D) */

    for (i = 0; i < points.size(); i++)
    {
        x = points[i][X] - o[X];
        y = points[i][Y] - o[Y];
        z = points[i][Z] - o[Z];

        d = x * w[X] + y * w[Y] + z * w[Z];

        if (fabs(d) > ZERO_TOLERANCE)
        {
            Set_Flag(this, DEGENERATE_FLAG);
            return true;
        }

        Data->Points[i][X] = x * u[X] + y * u[Y] + z * u[Z];
        Data->Points[i][Y] = x * v[X] + y * v[Y] + z * v[Z];
    }

    N = Vector3d(0.0, 0.0, 1.0);
    MTransNormal(S_Normal, N, Trans);

    S_Normal.normalize();

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Polyline_BBox
*
* INPUT
*
*   Polyg - Polyline
*
* OUTPUT
*
*   Polyg
*
* RETURNS
*
* AUTHOR
*
*   Jérôme Grimbert
*
* DESCRIPTION
*
*   Calculate the bounding box of a polyline.
*
* CHANGES
*
*   Jun 2018 : Creation.
*
******************************************************************************/

void Polyline::Compute_BBox()
{
    int i;
    Vector3d p, Puv, Min, Max;

    Min[X] = Min[Y] = Min[Z] =  BOUND_HUGE;
    Max[X] = Max[Y] = Max[Z] = -BOUND_HUGE;

    for (i = 0; i < Data->Points.size(); i++)
    {
        Puv[X] = Data->Points[i][X];
        Puv[Y] = Data->Points[i][Y];
        Puv[Z] = 0.0;

        MTransPoint(p, Puv, Trans);

        Min = min(Min, p);
        Max = max(Max, p);
    }

    Make_BBox_from_min_max(BBox, Min, Max);

    if (fabs(BBox.size[X]) < SMALL_TOLERANCE)
    {
        BBox.lowerLeft[X] -= SMALL_TOLERANCE;
        BBox.size[X]    += 2.0 * SMALL_TOLERANCE;
    }

    if (fabs(BBox.size[Y]) < SMALL_TOLERANCE)
    {
        BBox.lowerLeft[Y] -= SMALL_TOLERANCE;
        BBox.size[Y]    += 2.0 * SMALL_TOLERANCE;
    }

    if (fabs(BBox.size[Z]) < SMALL_TOLERANCE)
    {
        BBox.lowerLeft[Z] -= SMALL_TOLERANCE;
        BBox.size[Z]    += 2.0 * SMALL_TOLERANCE;
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   in_polyline
*
* INPUT
*
*   u, v   - 2D-coordinates of the point to test
*
* OUTPUT
*
* RETURNS
*
*   int - true, if inside
*
* AUTHOR
*
*
* DESCRIPTION
*
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Polyline::in_polyline(DBL u, DBL  v)const
{
    int i;
    int windingNumber;
    DBL ty, tx;
    const DBL *vtx0, *vtx1, *first;

    tx = u;
    ty = v;

    vtx0 = &Data->Points[0][X];
    vtx1 = &Data->Points[1][X];

    first = vtx0;

    windingNumber = 0;

    for (i = 1; i < Data->Points.size(); )
    {
        if(vtx0[Y] <= ty)
        {
          if(vtx1[Y] > ty)
          { // upward crossing
            if (relative(vtx0, vtx1, tx, ty ) == RELATIVE::LEFT )
            {
              ++windingNumber;
            }
          }
        }
        else
        { // ty > vtx0[Y], no need to test
          if(vtx1[Y] <= ty)
          { // downward crossing
            if(relative(vtx0, vtx1, tx, ty ) == RELATIVE::RIGHT)
            {
              --windingNumber;
            }
          }
        }

        /* Move to the next pair of vertices, retaining info as possible. */

        if ((i < Data->Points.size()-2) && (vtx1[X] == first[X]) && (vtx1[Y] == first[Y]))
        {
            vtx0 = &Data->Points[++i][X];
            vtx1 = &Data->Points[++i][X];

            first = vtx0;
        }
        else
        {
            vtx0 = vtx1;
            vtx1 = &Data->Points[++i][X];
        }
    }
    // handle negative values
    windingNumber = abs( windingNumber );
    return ((windingNumber < Data->SelectedWindingNumber.size())&&(Data->SelectedWindingNumber[windingNumber]));
}


/*****************************************************************************
*
* FUNCTION
*
*   relative
*
* INPUT
*
*   a        - 2D-coordinates of tail of segment
*   b        - 2D-coordinates of head of segment
*   tx, ty   - 2D-coordinates of the point to test
*
* OUTPUT
*
* RETURNS
*
*   LEFT, ON or RIGHT
*
* AUTHOR
*
*
* DESCRIPTION
*
*   tests if a point is Left, On or Right of an infinite line
*   The notion of Left / Right assumes an usual +X to the left, +Y to the top and is futile in 3D without enforced handness, but as long as the absolute value is used for the winding number, only a constant choice is needed for the
*   computation of the winding number.
*
* CHANGES
*
*   -
*
******************************************************************************/

Polyline::RELATIVE Polyline::relative( const double* a, const double* b, DBL tx, DBL ty)const
{
  RELATIVE answer=RELATIVE::ON;
  DBL f= ( (b[X]-a[X])*(ty-a[Y])-(tx-a[X])*(b[Y]-a[Y]));
  if (f<0)
  {
    answer = RELATIVE::RIGHT;
  } 
  else if (f>0)
  {
    answer = RELATIVE::LEFT;
  }
  return answer;
}

}
