/*******************************************************************************
 * triangle.cpp
 *
 * This module implements primitives for triangles and smooth triangles.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/shape/triangle.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/math/vector.h"
#include "backend/math/matrices.h"
#include "backend/scene/objects.h"
#include "backend/shape/triangle.h"
#include "backend/scene/threaddata.h"

#include <algorithm>

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
	VECTOR P3MinusP2, VTemp1, VTemp2;
	DBL x, y, z, uDenominator, Proj;

	VSub(P3MinusP2, P3, P2);

	x = fabs(P3MinusP2[X]);
	y = fabs(P3MinusP2[Y]);
	z = fabs(P3MinusP2[Z]);

	vAxis = max3_coordinate(x, y, z);

	VSub(VTemp1, P2, P3);

	VNormalize(VTemp1, VTemp1);

	VSub(VTemp2, P1, P3);

	VDot(Proj, VTemp2, VTemp1);

	VScaleEq(VTemp1, Proj);

	VSub(Perp, VTemp1, VTemp2);

	VNormalize(Perp, Perp);

	VDot(uDenominator, VTemp2, Perp);

	VInverseScaleEq(Perp, -uDenominator);

	/* Degenerate if smooth normals are more than 90 from actual normal
	   or its inverse. */
	VDot(x,Normal_Vector,N1);
	VDot(y,Normal_Vector,N2);
	VDot(z,Normal_Vector,N3);
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
	int swap, degn;
	VECTOR V1, V2, Temp;
	DBL Length;

	VSub(V1, P1, P2);
	VSub(V2, P3, P2);

	VCross(Normal_Vector, V1, V2);

	VLength(Length, Normal_Vector);

	/* Set up a flag so we can ignore degenerate triangles */

	if (Length == 0.0)
	{
		Set_Flag(this, DEGENERATE_FLAG);

		return(false);
	}

	/* Normalize the normal vector. */

	VInverseScaleEq(Normal_Vector, Length);

	VDot(Distance, Normal_Vector, P1);

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
		Assign_Vector(Temp, P2);
		Assign_Vector(P2, P1);
		Assign_Vector(P1, Temp);

		Assign_Vector(Temp, N2);
		Assign_Vector(N2, N1);
		Assign_Vector(N1, Temp);
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
	int swap;
	VECTOR V1, V2, Temp;
	DBL Length;

	VSub(V1, P1, P2);
	VSub(V2, P3, P2);

	VCross(Normal_Vector, V1, V2);

	VLength(Length, Normal_Vector);

	/* Set up a flag so we can ignore degenerate triangles */

	if (Length == 0.0)
	{
		Set_Flag(this, DEGENERATE_FLAG);

		return(false);
	}

	/* Normalize the normal vector. */

	VInverseScaleEq(Normal_Vector, Length);

	VDot(Distance, Normal_Vector, P1);

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
		Assign_Vector(Temp, P2);
		Assign_Vector(P2, P1);
		Assign_Vector(P1, Temp);
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
	VECTOR IPoint;

	Thread->Stats()[Ray_Triangle_Tests]++;
	if (Intersect(ray, &Depth))
	{
		Thread->Stats()[Ray_Triangle_Tests_Succeeded]++;
		VEvaluateRay(IPoint, ray.Origin, Depth, ray.Direction);

		if (Clip.empty() || Point_In_Clip(IPoint,Clip, Thread))
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

bool Triangle::Intersect(const Ray& ray, DBL *Depth) const
{
	DBL NormalDotOrigin, NormalDotDirection;
	DBL s, t;

	if (Test_Flag(this, DEGENERATE_FLAG))
		return(false);

	VDot(NormalDotDirection, Normal_Vector, ray.Direction);

	if (fabs(NormalDotDirection) < EPSILON)
		return(false);

	VDot(NormalDotOrigin, Normal_Vector, ray.Origin);

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

bool Triangle::Inside(const VECTOR, TraceThreadData *Thread) const
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

void Triangle::Normal(VECTOR Result, Intersection *, TraceThreadData *) const
{
	Assign_Vector(Result, Normal_Vector);
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

void SmoothTriangle::Normal(VECTOR Result, Intersection *Inter, TraceThreadData *Thread) const
{
	int Axis;
	DBL u, v;
	VECTOR PIMinusP1;

	VSub(PIMinusP1, Inter->IPoint, P1);

	VDot(u, PIMinusP1, Perp);

	if (u < EPSILON)
	{
		Assign_Vector(Result, N1);

		return;
	}

	Axis = vAxis;

	v = (PIMinusP1[Axis] / u + P1[Axis] - P2[Axis]) / (P3[Axis] - P2[Axis]);

	/* This is faster. [DB 8/94] */

	Result[X] = N1[X] + u * (N2[X] - N1[X] + v * (N3[X] - N2[X]));
	Result[Y] = N1[Y] + u * (N2[Y] - N1[Y] + v * (N3[Y] - N2[Y]));
	Result[Z] = N1[Z] + u * (N2[Z] - N1[Z] + v * (N3[Z] - N2[Z]));

	VNormalize(Result, Result);
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

void Triangle::Translate(const VECTOR Vector, const TRANSFORM *)
{
	if(!Test_Flag(this, DEGENERATE_FLAG))
	{
		VAddEq(P1, Vector);
		VAddEq(P2, Vector);
		VAddEq(P3, Vector);

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

void Triangle::Rotate(const VECTOR, const TRANSFORM *tr)
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

void Triangle::Scale(const VECTOR Vector, const TRANSFORM *)
{
	if(!Test_Flag(this, DEGENERATE_FLAG))
	{
		VEvaluateEq(P1, Vector);
		VEvaluateEq(P2, Vector);
		VEvaluateEq(P3, Vector);

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
*   Invert_Triangle
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

void Triangle::Invert()
{
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

Triangle::Triangle() : ObjectBase(TRIANGLE_OBJECT)
{
	Make_Vector(Normal_Vector, 0.0, 1.0, 0.0);

	Distance = 0.0;

	Make_Vector(P1, 0.0, 0.0, 0.0);
	Make_Vector(P2, 1.0, 0.0, 0.0);
	Make_Vector(P3, 0.0, 1.0, 0.0);
}

Triangle::Triangle(int t) : ObjectBase(t)
{
	Make_Vector(Normal_Vector, 0.0, 1.0, 0.0);

	Distance = 0.0;

	Make_Vector(P1, 0.0, 0.0, 0.0);
	Make_Vector(P2, 1.0, 0.0, 0.0);
	Make_Vector(P3, 0.0, 1.0, 0.0);
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

void SmoothTriangle::Translate(const VECTOR Vector, const TRANSFORM *)
{
	if(!Test_Flag(this, DEGENERATE_FLAG))
	{
		VAddEq(P1, Vector);
		VAddEq(P2, Vector);
		VAddEq(P3, Vector);

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

void SmoothTriangle::Rotate(const VECTOR, const TRANSFORM *tr)
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

void SmoothTriangle::Scale(const VECTOR Vector, const TRANSFORM *)
{
	DBL Length;

	if(!Test_Flag(this, DEGENERATE_FLAG))
	{
		VEvaluateEq(P1, Vector);
		VEvaluateEq(P2, Vector);
		VEvaluateEq(P3, Vector);

		N1[X] /= Vector[X];
		N1[Y] /= Vector[Y];
		N1[Z] /= Vector[Z];
		VLength(Length,N1);
		VScaleEq(N1,1.0/Length);
		N2[X] /= Vector[X];
		N2[Y] /= Vector[Y];
		N2[Z] /= Vector[Z];
		VLength(Length,N2);
		VScaleEq(N2,1.0/Length);
		N3[X] /= Vector[X];
		N3[Y] /= Vector[Y];
		N3[Z] /= Vector[Z];
		VLength(Length,N3);
		VScaleEq(N3,1.0/Length);

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
*   Invert_Smooth_Triangle
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

void SmoothTriangle::Invert()
{
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
	Make_Vector(Normal_Vector, 0.0, 1.0, 0.0);

	Distance = 0.0;

	Make_Vector(P1, 0.0, 0.0, 0.0);
	Make_Vector(P2, 1.0, 0.0, 0.0);
	Make_Vector(P3, 0.0, 1.0, 0.0);
	Make_Vector(N1, 0.0, 1.0, 0.0);
	Make_Vector(N2, 0.0, 1.0, 0.0);
	Make_Vector(N3, 0.0, 1.0, 0.0);
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
	VECTOR Min, Max, Epsilon;

	Make_Vector(Epsilon, EPSILON, EPSILON, EPSILON);

	Min[X] = min3(P1[X], P2[X], P3[X]);
	Min[Y] = min3(P1[Y], P2[Y], P3[Y]);
	Min[Z] = min3(P1[Z], P2[Z], P3[Z]);

	Max[X] = max3(P1[X], P2[X], P3[X]);
	Max[Y] = max3(P1[Y], P2[Y], P3[Y]);
	Max[Z] = max3(P1[Z], P2[Z], P3[Z]);

	VSubEq(Min, Epsilon);
	VAddEq(Max, Epsilon);

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


DBL SmoothTriangle::Calculate_Smooth_T(const VECTOR IPoint, const VECTOR P1, const VECTOR P2, const VECTOR P3)
{
	DBL a,b,c,d,e,f,g,h,i;
	DBL dm1,dm2,dm3,r,s,t;
	VECTOR Q;

	a=IPoint[0]-P1[0];
	b=P2[0]-P3[0];
	c=P2[0]-P1[0];

	d=IPoint[1]-P1[1];
	e=P2[1]-P3[1];
	f=P2[1]-P1[1];

	g=IPoint[2]-P1[2];
	h=P2[2]-P3[2];
	i=P2[2]-P1[2];

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


	Q[0]=P2[0]+s*(P3[0]-P2[0]);
	Q[1]=P2[1]+s*(P3[1]-P2[1]);
	Q[2]=P2[2]+s*(P3[2]-P2[2]);

	/*
	 t=(M-A)/(Q-A)
	*/

	a=Q[0]-P1[0];
	b=Q[1]-P1[1];
	c=Q[2]-P1[2];

	if(a*a<EPSILON)
	{
		if(b*b<EPSILON)
		{
			if(c*c<EPSILON)
				t=0;
			else
				t=(IPoint[2]-P1[2])/c;
		}
		else
			t=(IPoint[1]-P1[1])/b;
	}
	else
		t=(IPoint[0]-P1[0])/a;

	return t;
}

bool Triangle::Intersect_BBox(BBoxDirection, const BBOX_VECT&, const BBOX_VECT&, BBOX_VAL) const
{
	return true;
}

}
