/*******************************************************************************
 * planes.cpp
 *
 * This module implements functions that manipulate planes.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/planes.cpp $
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
#include "backend/scene/threaddata.h"
#include "backend/shape/planes.h"

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
*   All_Plane_Intersections
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

bool Plane::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
	DBL Depth;
	VECTOR IPoint;

	if (Intersect(ray, &Depth, Thread))
	{
		VEvaluateRay(IPoint, ray.Origin, Depth, ray.Direction);

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
*   Intersect_Plane
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

bool Plane::Intersect(const Ray& ray, DBL *Depth, TraceThreadData *Thread) const
{
	DBL NormalDotOrigin, NormalDotDirection;
	VECTOR P, D;

	Thread->Stats()[Ray_Plane_Tests]++;

	if (Trans == NULL)
	{
		VDot(NormalDotDirection, Normal_Vector, ray.Direction);

		if (fabs(NormalDotDirection) < EPSILON)
		{
			return(false);
		}

		VDot(NormalDotOrigin, Normal_Vector, ray.Origin);
	}
	else
	{
		MInvTransPoint(P, ray.Origin, Trans);
		MInvTransDirection(D, ray.Direction, Trans);

		VDot(NormalDotDirection, Normal_Vector, D);

		if (fabs(NormalDotDirection) < EPSILON)
		{
			return(false);
		}

		VDot(NormalDotOrigin, Normal_Vector, P);
	}

	*Depth = -(NormalDotOrigin + Distance) / NormalDotDirection;

	if ((*Depth >= DEPTH_TOLERANCE) && (*Depth <= MAX_DISTANCE))
	{
		Thread->Stats()[Ray_Plane_Tests_Succeeded]++;
		return (true);
	}
	else
	{
		return (false);
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Plane
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

bool Plane::Inside(const VECTOR IPoint, TraceThreadData *Thread) const
{
	DBL Temp;
	VECTOR P;

	if(Trans == NULL)
	{
		VDot(Temp, IPoint, Normal_Vector);
	}
	else
	{
		MInvTransPoint(P, IPoint, Trans);

		VDot(Temp, P, Normal_Vector);
	}

	return((Temp + Distance) < EPSILON);
}



/*****************************************************************************
*
* FUNCTION
*
*   Plane_Normal
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

void Plane::Normal(VECTOR Result, Intersection *, TraceThreadData *) const
{
	Assign_Vector(Result, Normal_Vector);

	if(Trans != NULL)
	{
		MTransNormal(Result, Result, Trans);

		VNormalize(Result, Result);
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Plane
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

void Plane::Translate(const VECTOR Vector, const TRANSFORM *tr)
{
	VECTOR Translation;

	if(Trans == NULL)
	{
		VEvaluate (Translation, Normal_Vector, Vector);

		Distance -= Translation[X] + Translation[Y] + Translation[Z];

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
*   Rotate_Plane
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

void Plane::Rotate(const VECTOR, const TRANSFORM *tr)
{
	if(Trans == NULL)
	{
		MTransDirection(Normal_Vector, Normal_Vector, tr);

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
*   Scale_Plane
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

void Plane::Scale(const VECTOR Vector, const TRANSFORM *tr)
{
	DBL Length;

	if(Trans == NULL)
	{
		VDivEq(Normal_Vector, Vector);

		VLength(Length, Normal_Vector);

		VInverseScaleEq(Normal_Vector, Length);

		Distance /= Length;

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
*   Invert_Plane
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

void Plane::Invert()
{
	VScaleEq(Normal_Vector, -1.0);

	Distance *= -1.0;
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Plane
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

void Plane::Transform(const TRANSFORM *tr)
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
*   Create_Plane
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

Plane::Plane() : ObjectBase(PLANE_OBJECT)
{
	Make_Vector(Normal_Vector, 0.0, 1.0, 0.0);

	Distance = 0.0;

	Trans = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Plane
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

ObjectPtr Plane::Copy()
{
	Plane *New = new Plane();
	Destroy_Transform(New->Trans);
	*New = *this;
	New->Trans = Copy_Transform(Trans);

	return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Plane
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

Plane::~Plane()
{
	Destroy_Transform(Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Plane_BBox
*
* INPUT
*
*   Plane - Plane
*   
* OUTPUT
*
*   Plane
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a plane (it's always infinite).
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Plane::Compute_BBox()
{
	Make_BBox(BBox, -BOUND_HUGE/2, -BOUND_HUGE/2, -BOUND_HUGE/2,
		BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);

	if (!Clip.empty())
	{
		BBox = Clip[0]->BBox; // FIXME - only supports one clip object? [trf]
	}
}

bool Plane::Intersect_BBox(BBoxDirection, const BBOX_VECT&, const BBOX_VECT&, BBOX_VAL) const
{
	return true;
}

}
