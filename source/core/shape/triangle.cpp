//******************************************************************************
///
/// @file core/shape/triangle.cpp
///
/// Implementation of the triangle geometric primitive.
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
#include "core/shape/triangle.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/mathutil.h"

// POV-Ray header files (core module)
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

const DBL DEPTH_TOLERANCE = 1e-6;

#define max3_coordinate(x,y,z) \
    ((x > y) ? ((x > z) ? X : Z) : ((y > z) ? Y : Z))



/*****************************************************************************
*
* FUNCTION
*
*   find_triangle_dominant_axis
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

void Triangle::find_triangle_dominant_axis()
{
    DBL x, y, z;

    x = fabs(Normal_Vector[X]);
    y = fabs(Normal_Vector[Y]);
    z = fabs(Normal_Vector[Z]);

    Dominant_Axis = max3_coordinate(x, y, z);
}



/*****************************************************************************
*
* FUNCTION
*
*   compute_smooth_triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

bool SmoothTriangle::Compute_Smooth_Triangle()
{
    Vector3d P3MinusP2, VTemp1, VTemp2;
    DBL x, y, z, uDenominator, Proj;

    P3MinusP2 = P3 - P2;

    x = fabs(P3MinusP2[X]);
    y = fabs(P3MinusP2[Y]);
    z = fabs(P3MinusP2[Z]);

    vAxis = max3_coordinate(x, y, z);

    VTemp1 = (P2 - P3).normalized();

    VTemp2 = (P1 - P3);

    Proj = dot(VTemp2, VTemp1);

    VTemp1 *= Proj;

    Perp = (VTemp1 - VTemp2).normalized();

    uDenominator = dot(VTemp2, Perp);

    Perp /= -uDenominator;

    /* Degenerate if smooth normals are more than 90 from actual normal
       or its inverse. */
    x = dot(Normal_Vector,N1);
    y = dot(Normal_Vector,N2);
    z = dot(Normal_Vector,N3);
    if ( ((x<0.0) && (y<0.0) && (z<0.0)) ||
         ((x>0.0) && (y>0.0) && (z>0.0)) )
    {
        return(true);
    }
    Set_Flag(this, DEGENERATE_FLAG);
    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

bool SmoothTriangle::Compute_Triangle()
{
    bool swap, degn;
    Vector3d V1, V2, Temp;
    DBL Length;

    V1 = P1 - P2;
    V2 = P3 - P2;

    Normal_Vector = cross(V1, V2);

    Length = Normal_Vector.length();

    /* Set up a flag so we can ignore degenerate triangles */

    if (Length == 0.0)
    {
        Set_Flag(this, DEGENERATE_FLAG);

        return(false);
    }

    /* Normalize the normal vector. */

    Normal_Vector /= Length;

    Distance = -dot(Normal_Vector, P1);

    find_triangle_dominant_axis();

    swap = false;

    switch (Dominant_Axis)
    {
        case X:

            if ((P2[Y] - P3[Y])*(P2[Z] - P1[Z]) <
                (P2[Z] - P3[Z])*(P2[Y] - P1[Y]))
            {
                swap = true;
            }

            break;

        case Y:

            if ((P2[X] - P3[X])*(P2[Z] - P1[Z]) <
                (P2[Z] - P3[Z])*(P2[X] - P1[X]))
            {
                swap = true;
            }

            break;

        case Z:

            if ((P2[X] - P3[X])*(P2[Y] - P1[Y]) <
                (P2[Y] - P3[Y])*(P2[X] - P1[X]))
            {
                swap = true;
            }

            break;
    }

    if (swap)
    {
        Temp = P2;
        P2 = P1;
        P1 = Temp;

        Temp = N2;
        N2 = N1;
        N1 = Temp;
    }

    degn=Compute_Smooth_Triangle();

    /* Build the bounding information from the vertices. */

    Compute_BBox();

    return(degn);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

bool Triangle::Compute_Triangle()
{
    bool swap;
    Vector3d V1, V2, Temp;
    DBL Length;

    V1 = P1 - P2;
    V2 = P3 - P2;

    Normal_Vector = cross(V1, V2);
    if (mPointOrderSwapped)
        Normal_Vector = -Normal_Vector;

    Length = Normal_Vector.length();

    /* Set up a flag so we can ignore degenerate triangles */

    if (Length == 0.0)
    {
        Set_Flag(this, DEGENERATE_FLAG);

        return(false);
    }

    /* Normalize the normal vector. */

    Normal_Vector /= Length;

    Distance = dot(Normal_Vector, P1);

    Distance *= -1.0;

    find_triangle_dominant_axis();

    swap = false;

    switch (Dominant_Axis)
    {
        case X:

            if ((P2[Y] - P3[Y])*(P2[Z] - P1[Z]) <
                (P2[Z] - P3[Z])*(P2[Y] - P1[Y]))
            {
                swap = true;
            }

            break;

        case Y:

            if ((P2[X] - P3[X])*(P2[Z] - P1[Z]) <
                (P2[Z] - P3[Z])*(P2[X] - P1[X]))
            {
                swap = true;
            }

            break;

        case Z:

            if ((P2[X] - P3[X])*(P2[Y] - P1[Y]) <
                (P2[Y] - P3[Y])*(P2[X] - P1[X]))
            {
                swap = true;
            }

            break;
    }

    if (swap)
    {
        Temp = P2;
        P2 = P1;
        P1 = Temp;

        mPointOrderSwapped = !mPointOrderSwapped;
    }

    /* Build the bounding information from the vertices. */

    Compute_BBox();

    return(true);
}



/*****************************************************************************
*
* FUNCTION
*
*   All_Triangle_Intersections
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

bool Triangle::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    DBL Depth;
    Vector3d IPoint;

    Thread->Stats()[Ray_Triangle_Tests]++;
    if (Intersect(ray, &Depth))
    {
        Thread->Stats()[Ray_Triangle_Tests_Succeeded]++;
        IPoint = ray.Evaluate(Depth);

        if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
        {
            Depth_Stack->push(Intersection(Depth,IPoint,this));

            return(true);
        }
    }

    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

bool Triangle::Intersect(const BasicRay& ray, DBL *Depth) const
{
    DBL NormalDotOrigin, NormalDotDirection;
    DBL s, t;

    if (Test_Flag(this, DEGENERATE_FLAG))
        return(false);

    NormalDotDirection = dot(Normal_Vector, ray.Direction);

    if (fabs(NormalDotDirection) < EPSILON)
        return(false);

    NormalDotOrigin = dot(Normal_Vector, ray.Origin);

    *Depth = -(Distance + NormalDotOrigin) / NormalDotDirection;

    if ((*Depth < DEPTH_TOLERANCE) || (*Depth > MAX_DISTANCE))
        return(false);

    switch (Dominant_Axis)
    {
        case X:

            s = ray.Origin[Y] + *Depth * ray.Direction[Y];
            t = ray.Origin[Z] + *Depth * ray.Direction[Z];

            if ((P2[Y] - s) * (P2[Z] - P1[Z]) <
                (P2[Z] - t) * (P2[Y] - P1[Y]))
            {
                return(false);
            }

            if ((P3[Y] - s) * (P3[Z] - P2[Z]) <
                (P3[Z] - t) * (P3[Y] - P2[Y]))
            {
                return(false);
            }

            if ((P1[Y] - s) * (P1[Z] - P3[Z]) <
                (P1[Z] - t) * (P1[Y] - P3[Y]))
            {
                return(false);
            }

            return(true);

        case Y:

            s = ray.Origin[X] + *Depth * ray.Direction[X];
            t = ray.Origin[Z] + *Depth * ray.Direction[Z];

            if ((P2[X] - s) * (P2[Z] - P1[Z]) <
                (P2[Z] - t) * (P2[X] - P1[X]))
            {
                return(false);
            }

            if ((P3[X] - s) * (P3[Z] - P2[Z]) <
                (P3[Z] - t) * (P3[X] - P2[X]))
            {
                return(false);
            }

            if ((P1[X] - s) * (P1[Z] - P3[Z]) <
                (P1[Z] - t) * (P1[X] - P3[X]))
            {
                return(false);
            }

            return(true);

        case Z:

            s = ray.Origin[X] + *Depth * ray.Direction[X];
            t = ray.Origin[Y] + *Depth * ray.Direction[Y];

            if ((P2[X] - s) * (P2[Y] - P1[Y]) <
                (P2[Y] - t) * (P2[X] - P1[X]))
            {
                return(false);
            }

            if ((P3[X] - s) * (P3[Y] - P2[Y]) <
                (P3[Y] - t) * (P3[X] - P2[X]))
            {
                return(false);
            }

            if ((P1[X] - s) * (P1[Y] - P3[Y]) <
                (P1[Y] - t) * (P1[X] - P3[X]))
            {
                return(false);
            }

            return(true);
    }

    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

bool Triangle::Inside(const Vector3d&, TraceThreadData *Thread) const
{
    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Triangle_Normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

void Triangle::Normal(Vector3d& Result, Intersection *, TraceThreadData *) const
{
    Result = Normal_Vector;
}



/*****************************************************************************
*
* FUNCTION
*
*   Smooth_Triangle_Normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Calculate the Phong-interpolated vector within the triangle
*   at the given intersection point. The math for this is a bit
*   bizarre:
*
*      -         P1
*      |        /|\ \
*      |       / |Perp\
*      |      /  V  \   \
*      |     /   |    \   \
*    u |    /____|_____PI___\
*      |   /     |       \    \
*      -  P2-----|--------|----P3
*                Pbase    PIntersect
*          |-------------------|
*                         v
*
*   Triangle->Perp is a unit vector from P1 to Pbase. We calculate
*
*   u = (PI - P1) DOT Perp / ((P3 - P1) DOT Perp).
*
*   We then calculate where the line from P1 to PI intersects the line P2 to P3:
*   PIntersect = (PI - P1)/u.
*
*   We really only need one coordinate of PIntersect.  We then calculate v as:
*
*        v = PIntersect[X] / (P3[X] - P2[X])
*   or   v = PIntersect[Y] / (P3[Y] - P2[Y])
*   or   v = PIntersect[Z] / (P3[Z] - P2[Z])
*
*   depending on which calculation will give us the best answers.
*
*   Once we have u and v, we can perform the normal interpolation as:
*
*     NTemp1 = N1 + u(N2 - N1);
*     NTemp2 = N1 + u(N3 - N1);
*     Result = normalize (NTemp1 + v(NTemp2 - NTemp1))
*
*   As always, any values which are constant for the triangle are cached
*   in the triangle.
*
* CHANGES
*
*   -
*
******************************************************************************/

void SmoothTriangle::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    int Axis;
    DBL u, v;
    Vector3d PIMinusP1;

    PIMinusP1 = Inter->IPoint - P1;

    u = dot(PIMinusP1, Perp);

    if (u < EPSILON)
    {
        Result = N1;

        return;
    }

    Axis = vAxis;

    v = (PIMinusP1[Axis] / u + P1[Axis] - P2[Axis]) / (P3[Axis] - P2[Axis]);

    /* This is faster. [DB 8/94] */

    Result = N1 + u * (N2 - N1 + v * (N3 - N2));

    Result.normalize();
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

void Triangle::Translate(const Vector3d& Vector, const TRANSFORM *)
{
    if(!Test_Flag(this, DEGENERATE_FLAG))
    {
        P1 += Vector;
        P2 += Vector;
        P3 += Vector;

        Compute_Triangle();
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

void Triangle::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    if (!Test_Flag(this, DEGENERATE_FLAG))
    {
        Transform(tr);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

void Triangle::Scale(const Vector3d& Vector, const TRANSFORM *)
{
    if(!Test_Flag(this, DEGENERATE_FLAG))
    {
        P1 *= Vector;
        P2 *= Vector;
        P3 *= Vector;

        Compute_Triangle();
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Transfrom_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

void Triangle::Transform(const TRANSFORM *tr)
{
    if(!Test_Flag(this, DEGENERATE_FLAG))
    {
        MTransPoint(P1, P1, tr);
        MTransPoint(P2, P2, tr);
        MTransPoint(P3, P3, tr);

        Compute_Triangle();
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

Triangle::Triangle() : NonsolidObject(TRIANGLE_OBJECT)
{
    Normal_Vector = Vector3d(0.0, 1.0, 0.0);

    Distance = 0.0;

    P1 = Vector3d(0.0, 0.0, 0.0);
    P2 = Vector3d(1.0, 0.0, 0.0);
    P3 = Vector3d(0.0, 1.0, 0.0);

    mPointOrderSwapped = false;
}

Triangle::Triangle(int t) : NonsolidObject(t)
{
    Normal_Vector = Vector3d(0.0, 1.0, 0.0);

    Distance = 0.0;

    P1 = Vector3d(0.0, 0.0, 0.0);
    P2 = Vector3d(1.0, 0.0, 0.0);
    P3 = Vector3d(0.0, 1.0, 0.0);

    mPointOrderSwapped = false;
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

ObjectPtr Triangle::Copy()
{
    Triangle *New = new Triangle();
    Destroy_Transform(New->Trans);
    *New = *this;

    return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

Triangle::~Triangle()
{
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Smooth_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

void SmoothTriangle::Translate(const Vector3d& Vector, const TRANSFORM *)
{
    if(!Test_Flag(this, DEGENERATE_FLAG))
    {
        P1 += Vector;
        P2 += Vector;
        P3 += Vector;

        Compute_Triangle();
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Smooth_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

void SmoothTriangle::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    if(!Test_Flag(this, DEGENERATE_FLAG))
    {
        Transform(tr);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Smooth_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

void SmoothTriangle::Scale(const Vector3d& Vector, const TRANSFORM *)
{
    if(!Test_Flag(this, DEGENERATE_FLAG))
    {
        P1 *= Vector;
        P2 *= Vector;
        P3 *= Vector;

        N1 /= Vector;
        N1.normalize();
        N2 /= Vector;
        N2.normalize();
        N3 /= Vector;
        N3.normalize();

        Compute_Triangle();
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Smooth_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

void SmoothTriangle::Transform(const TRANSFORM *tr)
{
    if(!Test_Flag(this, DEGENERATE_FLAG))
    {
        MTransPoint(P1, P1, tr);
        MTransPoint(P2, P2, tr);
        MTransPoint(P3, P3, tr);
        MTransNormal(N1, N1, tr);
        MTransNormal(N2, N2, tr);
        MTransNormal(N3, N3, tr);

        Compute_Triangle();
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Smooth_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

SmoothTriangle::SmoothTriangle() : Triangle(SMOOTH_TRIANGLE_OBJECT)
{
    Normal_Vector = Vector3d(0.0, 1.0, 0.0);

    Distance = 0.0;

    P1 = Vector3d(0.0, 0.0, 0.0);
    P2 = Vector3d(1.0, 0.0, 0.0);
    P3 = Vector3d(0.0, 1.0, 0.0);
    N1 = Vector3d(0.0, 1.0, 0.0);
    N2 = Vector3d(0.0, 1.0, 0.0);
    N3 = Vector3d(0.0, 1.0, 0.0);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Smooth_Triangle
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
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

ObjectPtr SmoothTriangle::Copy()
{
    SmoothTriangle *New = new SmoothTriangle();

    *New = *this;

    return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Triangle_BBox
*
* INPUT
*
*   Triangle - Triangle
*
* OUTPUT
*
*   Triangle
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a triangle.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Triangle::Compute_BBox()
{
    Vector3d Min, Max, Epsilon;

    Epsilon = Vector3d(EPSILON);

    Min[X] = min3(P1[X], P2[X], P3[X]);
    Min[Y] = min3(P1[Y], P2[Y], P3[Y]);
    Min[Z] = min3(P1[Z], P2[Z], P3[Z]);

    Max[X] = max3(P1[X], P2[X], P3[X]);
    Max[Y] = max3(P1[Y], P2[Y], P3[Y]);
    Max[Z] = max3(P1[Z], P2[Z], P3[Z]);

    Min -= Epsilon;
    Max += Epsilon;

    Make_BBox_from_min_max(BBox, Min, Max);
}


/* AP */

/*

  corners A B C
  point inside triangle M
  Q is intersection of line AM with line BC

  1 <= r  Q = A + r(M-A)


  0 <= s <= 1  Q = B + s(C-B)

  0 <= t <=1   M = A + t(Q-A)

  ra+sb=c
  rd+se=f
  rg+sh=i

 */


DBL SmoothTriangle::Calculate_Smooth_T(const Vector3d& IPoint, const Vector3d& P1, const Vector3d& P2, const Vector3d& P3)
{
    DBL a,b,c,d,e,f,g,h,i;
    DBL dm1,dm2,dm3,r,s,t;
    Vector3d Q;

    a=IPoint[X]-P1[X];
    b=P2[X]-P3[X];
    c=P2[X]-P1[X];

    d=IPoint[Y]-P1[Y];
    e=P2[Y]-P3[Y];
    f=P2[Y]-P1[Y];

    g=IPoint[Z]-P1[Z];
    h=P2[Z]-P3[Z];
    i=P2[Z]-P1[Z];

    dm1=a*e-d*b;
    dm2=a*h-g*b;
    dm3=d*h-g*e;

    if(dm1*dm1<EPSILON)
    {
        if(dm2*dm2<EPSILON)
        {
            if(dm3*dm3 < EPSILON)
            {
                // all determinants too small
                return 0.0;
            }
            else
            {
                /* use dm3 */
                r=(f*h-i*e)/dm3;
                s=(d*i-g*f)/dm3;
            }
        }
        else
        {
            /* use dm2 */
            r=(c*h-b*i)/dm2;
            s=(a*i-g*c)/dm2;
        }
    }
    else
    {
        /* use dm1 */
        r=(c*e-f*b)/dm1;
        s=(a*f-d*c)/dm1;
    }

    Q = P2 + s*(P3-P2);

    /*
     t=(M-A)/(Q-A)
    */

    a=Q[X]-P1[X];
    b=Q[Y]-P1[Y];
    c=Q[Z]-P1[Z];

    if(a*a<EPSILON)
    {
        if(b*b<EPSILON)
        {
            if(c*c<EPSILON)
                t=0;
            else
                t=(IPoint[Z]-P1[Z])/c;
        }
        else
            t=(IPoint[Y]-P1[Y])/b;
    }
    else
        t=(IPoint[X]-P1[X])/a;

    return t;
}

bool Triangle::Intersect_BBox(BBoxDirection, const BBoxVector3d&, const BBoxVector3d&, BBoxScalar) const
{
    return true;
}

}
// end of namespace pov
