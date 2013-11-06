/*******************************************************************************
 * warps.cpp
 *
 * This module implements functions that warp or modify the point at which
 * a texture pattern is evaluated.
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
 * $File: //depot/public/povray/3.x/source/backend/pattern/warps.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/pattern/warps.h"
#include "backend/pattern/pattern.h"
#include "backend/texture/texture.h"
#include "backend/math/vector.h"
#include "backend/math/matrices.h"
#include "backend/support/randomsequences.h"
#include "base/pov_err.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

const DBL COORDINATE_LIMIT = 1.0e17;

RandomDoubleSequence WarpRands(0.0, 1.0, 32768);

/*****************************************************************************
* Static functions
******************************************************************************/
static int warp_cylindrical(VECTOR TPoint, const CYLW *Warp);
static int warp_spherical(VECTOR TPoint, const SPHEREW *Warp);
static int warp_toroidal(VECTOR TPoint, const TOROIDAL *Warp);
static int warp_planar(VECTOR TPoint, const PLANARW *Warp);
static int warp_cubic(VECTOR TPoint); // JN2007: Cubic warp



/*****************************************************************************
*
* FUNCTION
*
*   Warp_EPoint
*
* INPUT
*
*   EPoint -- The original point in 3d space at which a pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*   
* OUTPUT
*
*   TPoint -- Point after turbulence and transform
*   have been applied
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Warp_EPoint (VECTOR TPoint, const VECTOR EPoint, const TPATTERN *TPat)
{
	VECTOR PTurbulence,RP;
	int Axis,i;
	int blockX = 0, blockY = 0, blockZ = 0 ;
	SNGL BlkNum;
	DBL  Length;
	DBL  Strength;
	WARP *Warp=TPat->Warps;
	TURB *Turb;
	TRANS *Tr;
	REPEAT *Repeat;
	BLACK_HOLE *Black_Hole;
	VECTOR Delta, Center;

	Assign_Vector(TPoint, EPoint);

	while (Warp != NULL)
	{
		switch(Warp->Warp_Type)
		{
			case CLASSIC_TURB_WARP:
				if ((TPat->Type == MARBLE_PATTERN) ||
				    (TPat->Type == NO_PATTERN)     ||
				    (TPat->Type == WOOD_PATTERN))
				{
					break;
				}
			/* If not a special type, fall through to next case */

			case EXTRA_TURB_WARP:
				Turb=reinterpret_cast<TURB *>(Warp);
				DTurbulence (PTurbulence, TPoint, Turb);
				TPoint[X] += PTurbulence[X] * Turb->Turbulence[X];
				TPoint[Y] += PTurbulence[Y] * Turb->Turbulence[Y];
				TPoint[Z] += PTurbulence[Z] * Turb->Turbulence[Z];
				break;

			case NO_WARP:
				break;

			case TRANSFORM_WARP:
				Tr=reinterpret_cast<TRANS *>(Warp);
				MInvTransPoint(TPoint, TPoint, &(Tr->Trans));
				break;

			case REPEAT_WARP:
				Repeat=reinterpret_cast<REPEAT *>(Warp);
				Assign_Vector(RP,TPoint);
				Axis=Repeat->Axis;
				BlkNum=(SNGL)floor(TPoint[Axis]/Repeat->Width);

				RP[Axis]=TPoint[Axis]-BlkNum*Repeat->Width;

				if (((int)BlkNum) & 1)
				{
					VEvaluateEq(RP,Repeat->Flip);
					if ( Repeat->Flip[Axis] < 0 )
					{
						RP[Axis] = Repeat->Width+RP[Axis];
					}
				}

				VAddScaledEq(RP,BlkNum,Repeat->Offset);
				Assign_Vector(TPoint,RP);
				break;

			case BLACK_HOLE_WARP:
				Black_Hole = reinterpret_cast<BLACK_HOLE *>(Warp) ;
				Assign_Vector (Center, Black_Hole->Center) ;

				if (Black_Hole->Repeat)
				{
					/* first, get the block number we're in for each dimension  */
					/* block numbers are (currently) calculated relative to 0   */
					/* we use floor () since it correctly returns -1 for the
					   first block below 0 in each axis                         */
					/* one final point - we could run into overflow problems if
					   the repeat vector was small and the scene very large.    */
					if (Black_Hole->Repeat_Vector [X] >= EPSILON)
						blockX = (int) floor (TPoint [X] / Black_Hole->Repeat_Vector [X]) ;

					if (Black_Hole->Repeat_Vector [Y] >= EPSILON)
						blockY = (int) floor (TPoint [Y] / Black_Hole->Repeat_Vector [Y]) ;

					if (Black_Hole->Repeat_Vector [Z] >= EPSILON)
						blockZ = (int) floor (TPoint [Z] / Black_Hole->Repeat_Vector [Z]) ;

					if (Black_Hole->Uncertain)
					{
						/* if the position is uncertain calculate the new one first */
						/* this will allow the same numbers to be returned by frand */

						int seed = Hash3d (blockX, blockY, blockZ);
						Center [X] += WarpRands(seed) * Black_Hole->Uncertainty_Vector [X] ;
						Center [Y] += WarpRands(seed + 1) * Black_Hole->Uncertainty_Vector [Y] ;
						Center [Z] += WarpRands(seed + 2) * Black_Hole->Uncertainty_Vector [Z] ;
					}

					Center [X] += Black_Hole->Repeat_Vector [X] * blockX ;
					Center [Y] += Black_Hole->Repeat_Vector [Y] * blockY ;
					Center [Z] += Black_Hole->Repeat_Vector [Z] * blockZ ;
				}

				VSub (Delta, TPoint, Center) ;
				VLength (Length, Delta) ;

				/* Length is the distance from the centre of the black hole */
				if (Length >= Black_Hole->Radius) break ;

				if (Black_Hole->Type == 0)
				{
					/* now convert the length to a proportion (0 to 1) that the point
					   is from the edge of the black hole. a point on the perimeter
					   of the black hole will be 0.0 ; a point at the centre will be
					   1.0 ; a point exactly halfway will be 0.5, and so forth. */
					Length = (Black_Hole->Radius - Length) / Black_Hole->Radius ;

					/* Strength is the magnitude of the transformation effect. firstly,
					   apply the Power variable to Length. this is meant to provide a
					   means of controlling how fast the power of the Black Hole falls
					   off from its centre. if Power is 2.0, then the effect is inverse
					   square. increasing power will cause the Black Hole to be a lot
					   weaker in its effect towards its perimeter. 
					     
					   finally we multiply Strength with the Black Hole's Strength
					   variable. if the resultant value exceeds 1.0 we clip it to 1.0.
					   this means a point will never be transformed by more than its
					   original distance from the centre. the result of this clipping
					   is that you will have an 'exclusion' area near the centre of
					   the black hole where all points whose final value exceeded or
					   equalled 1.0 were moved by a fixed amount. this only happens
					   if the Strength value of the Black Hole was greater than one. */

					Strength = pow (Length, Black_Hole->Power) * Black_Hole->Strength ;
					if (Strength > 1.0) Strength = 1.0 ;

					/* if the Black Hole is inverted, it gives the impression of 'push-
					   ing' the pattern away from its centre. otherwise it sucks. */
					VScaleEq (Delta, Black_Hole->Inverted ? -Strength : Strength) ;

					/* add the scaled Delta to the input point to end up with TPoint. */
					VAddEq (TPoint, Delta) ;
				}
				break;

			/* 10/23/1998 Talious added SPherical Cylindrical and toroidal
			warps */

			case CYLINDRICAL_WARP:
				warp_cylindrical(TPoint, reinterpret_cast<CYLW *>(Warp));
				break;

			case PLANAR_WARP:
				warp_planar(TPoint, reinterpret_cast<PLANARW *>(Warp));
				break;

			case SPHERICAL_WARP:
				warp_spherical(TPoint, reinterpret_cast<SPHEREW *>(Warp));
				break;

			case TOROIDAL_WARP:
				warp_toroidal(TPoint, reinterpret_cast<TOROIDAL *>(Warp));
				break;

			case CUBIC_WARP:
				warp_cubic(TPoint);
				break;

			default:
				throw POV_EXCEPTION_STRING("Warp type not yet implemented.");
		}
		Warp=Warp->Next_Warp;
	}

	for (i=X; i<=Z; i++)
		if (TPoint[i] > COORDINATE_LIMIT)
			TPoint[i]= COORDINATE_LIMIT;
		else
			if (TPoint[i] < -COORDINATE_LIMIT)
				TPoint[i] = -COORDINATE_LIMIT;

}

void Warp_Normal (VECTOR TNorm, const VECTOR ENorm, const TPATTERN *TPat, bool DontScaleBumps)
{
	const WARP *Warp=TPat->Warps;
	const TRANS *Tr;

	if(!DontScaleBumps)
		VNormalize(TNorm,ENorm);
	else
		Assign_Vector(TNorm,ENorm);

	while(Warp != NULL)
	{
		switch(Warp->Warp_Type)
		{
			default:
			case NO_WARP:
				break;
			case TRANSFORM_WARP:
				Tr=reinterpret_cast<const TRANS *>(Warp);
				MInvTransNormal(TNorm, TNorm, &(Tr->Trans));
				break;
			/*
			default:
				Error("Warp type %d not yet implemented",Warp->Warp_Type);
			*/
		}
		Warp=Warp->Next_Warp;
	}

	if(!DontScaleBumps)
		VNormalizeEq(TNorm);
}

void UnWarp_Normal (VECTOR TNorm, const VECTOR ENorm, const TPATTERN *TPat, bool DontScaleBumps)
{
	const WARP *Warp = NULL;

	if(!DontScaleBumps)
		VNormalize(TNorm,ENorm);
	else
		Assign_Vector(TNorm,ENorm);

	if(TPat->Warps != NULL)
	{
		// go to the last entry
		for(Warp = TPat->Warps; Warp->Next_Warp != NULL; Warp = Warp->Next_Warp) ;

		// walk backwards from the last entry
		for(; Warp != NULL; Warp = Warp->Prev_Warp)
		{
			if(Warp->Warp_Type == TRANSFORM_WARP)
				MTransNormal(TNorm, TNorm, &((reinterpret_cast<const TRANS *>(Warp))->Trans));
		}
	}

	if(!DontScaleBumps)
		VNormalizeEq(TNorm);
}

/*****************************************************************************
*
* FUNCTION
*    warp_planar
*
* INPUT
*    
* OUTPUT
*
* RETURNS
*
* AUTHOR  Matthew Corey Brown (talious)
*
* DESCRIPTION
*    Based on cylindrical_image_map from image.c
*    Its a 3d version of that for warps
*
* CHANGES
*
******************************************************************************/

static int warp_planar(VECTOR EPoint, const PLANARW *Warp)
{
	DBL x = EPoint[X];
	DBL z = Warp->OffSet;
	DBL y = EPoint[Y];

	if((Warp->Orientation_Vector[X] == 0.0) &&
	   (Warp->Orientation_Vector[Y] == 0.0) &&
	   (Warp->Orientation_Vector[Z] == 1.0))
	{
		EPoint[X] = x;
		EPoint[Y] = y;
		EPoint[Z] = z;
	}
	else
	{
		EPoint[X] = (Warp->Orientation_Vector[X] * z) +
		            (Warp->Orientation_Vector[Y] * x) +
		            (Warp->Orientation_Vector[Z] * x);
		EPoint[Y] = (Warp->Orientation_Vector[X] * y) +
		            (Warp->Orientation_Vector[Y] * -z) +
		            (Warp->Orientation_Vector[Z] * y);
		EPoint[Z] = (Warp->Orientation_Vector[X] * -x) +
		            (Warp->Orientation_Vector[Y] * y) +
		            (Warp->Orientation_Vector[Z] * z);
	}

	return 1;
}


/*****************************************************************************
*
* FUNCTION
*    warp_cylindrical
*
* INPUT
*    
* OUTPUT
*
* RETURNS
*
* AUTHOR  Matthew Corey Brown (talious)
*
* DESCRIPTION
*    Based on cylindrical_image_map from image.c
*    Its a 3d version of that for warps
*
* CHANGES
*
******************************************************************************/

static int warp_cylindrical(VECTOR EPoint, const CYLW *Warp)
{
	DBL len, theta;
	DBL x = EPoint[X];
	DBL y = EPoint[Y];
	DBL z = EPoint[Z];

	// Determine its angle from the point (1, 0, 0) in the x-z plane.
	len = sqrt(x * x + z * z);

	if(len == 0.0)
		return 0;
	else
	{
		if(z == 0.0)
		{
			if(x > 0)
				theta = 0.0;
			else
				theta = M_PI;
		}
		else
		{
			theta = acos(x / len);
			if(z < 0.0)
				theta = TWO_M_PI - theta;
		}

		theta /= TWO_M_PI;  // This will be from 0 to 1
	}

	if(Warp->DistExp == 1.0)
		theta *= len;
	else if (Warp->DistExp != 0.0)
		theta *= pow(len,Warp->DistExp);

	x = theta;
	z = len;

	if((Warp->Orientation_Vector[X] == 0.0) &&
	   (Warp->Orientation_Vector[Y] == 0.0) &&
	   (Warp->Orientation_Vector[Z] == 1.0))
	{
		EPoint[X] = x;
		EPoint[Y] = y;
		EPoint[Z] = z;
	}
	else
	{
		EPoint[X] = (Warp->Orientation_Vector[X] * z) +
		            (Warp->Orientation_Vector[Y] * x) +
		            (Warp->Orientation_Vector[Z] * x);
		EPoint[Y] = (Warp->Orientation_Vector[X] * y) +
		            (Warp->Orientation_Vector[Y] * -z) +
		            (Warp->Orientation_Vector[Z] * y);
		EPoint[Z] = (Warp->Orientation_Vector[X] * -x) +
		            (Warp->Orientation_Vector[Y] * y) +
		            (Warp->Orientation_Vector[Z] * z);
	}

	return 1;
}

/*****************************************************************************
*
* FUNCTION
*        warp_toroidal(VECTOR EPoint, TOROIDAL *Warp)
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR   Matthew Corey Brown (Talious)
*
*
* DESCRIPTION
* Warps a point on a torus centered on orgin to a 2 d plane in space
* based on torus_image_map
*
* CHANGES
*
******************************************************************************/

static int warp_toroidal(VECTOR EPoint, const TOROIDAL *Warp)
{
	DBL len, phi, theta;
	DBL r0;
	DBL x = EPoint[X];
	DBL y = EPoint[Y];
	DBL z = EPoint[Z];

	r0 = Warp->MajorRadius;

	// Determine its angle from the x-axis.

	len = sqrt(x * x + z * z);

	if(len == 0.0)
		return 0;
	else
	{
		if(z == 0.0)
		{
			if(x > 0)
				theta = 0.0;
			else
				theta = M_PI;
		}
		else
		{
			theta = acos(x / len);
			if(z < 0.0)
				theta = TWO_M_PI - theta;
		}
	}

	theta = 0.0 - theta;

	// Now rotate about the y-axis to get the point (x, y, z) into the x-y plane.

	x = len - r0;
	len = sqrt(x * x + y * y);
	phi = acos(-x / len);
	if (y > 0.0)
		phi = TWO_M_PI - phi;

	// Determine the parametric coordinates.

	theta /= (-TWO_M_PI);

	phi /= TWO_M_PI;

	if (Warp->DistExp == 1.0)
	{
		theta *= len;
		phi *= len;
	}
	else if (Warp->DistExp != 0.0)
	{
		theta *= pow(len,Warp->DistExp);
		phi *= pow(len,Warp->DistExp);
	}

	x = theta;
	z = len;
	y = phi;

	if((Warp->Orientation_Vector[X] == 0.0) &&
	   (Warp->Orientation_Vector[Y] == 0.0) &&
	   (Warp->Orientation_Vector[Z] == 1.0))
	{
		EPoint[X] = x;
		EPoint[Y] = y;
		EPoint[Z] = z;
	}
	else
	{
		EPoint[X] = (Warp->Orientation_Vector[X] * z) +
		            (Warp->Orientation_Vector[Y] * x) +
		            (Warp->Orientation_Vector[Z] * x);
		EPoint[Y] = (Warp->Orientation_Vector[X] * y) +
		            (Warp->Orientation_Vector[Y] * -z) +
		            (Warp->Orientation_Vector[Z] * y);
		EPoint[Z] = (Warp->Orientation_Vector[X] * -x) +
		            (Warp->Orientation_Vector[Y] * y) +
		            (Warp->Orientation_Vector[Z] * z);
	}

	return 1;
}

/*****************************************************************************
*
* FUNCTION 
*    warp_spherical
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR   Matthew Corey Brown (Talious)
*
*
* DESCRIPTION
* Warps a point on a sphere centered on orgin to a 2 d plane in space
* based on spherical_image_map
*
* CHANGES
*
******************************************************************************/
static int warp_spherical(VECTOR EPoint, const SPHEREW *Warp)
{
	DBL len, phi, theta,dist;
	DBL x = EPoint[X];
	DBL y = EPoint[Y];
	DBL z = EPoint[Z];

	// Make sure this vector is on the unit sphere.

	dist = sqrt(x * x + y * y + z * z);

	if(dist == 0.0)
		return 0;
	else
	{
		x /= dist;
		y /= dist;
		z /= dist;
	}

	// Determine its angle from the x-z plane.
	phi = 0.5 + asin(y) / M_PI; // This will be from 0 to 1

	// Determine its angle from the point (1, 0, 0) in the x-z plane.
	len = sqrt(x * x + z * z);
	if(len == 0.0)
	{
		// This point is at one of the poles. Any value of xcoord will be ok...
		theta = 0;
	}
	else
	{
		if(z == 0.0)
		{
			if(x > 0)
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

	if(Warp->DistExp == 1.0)
	{
		theta *= dist;
		phi *= dist;
	}
	else if(Warp->DistExp != 0.0)
	{
		theta *= pow(dist,Warp->DistExp);
		phi *= pow(dist,Warp->DistExp);
	}

	x = theta;
	z = dist;
	y = phi;

	if((Warp->Orientation_Vector[X] == 0.0) &&
	   (Warp->Orientation_Vector[Y] == 0.0) &&
	   (Warp->Orientation_Vector[Z] == 1.0))
	{
		EPoint[X] = x;
		EPoint[Y] = y;
		EPoint[Z] = z;
	}
	else
	{
		EPoint[X] = (Warp->Orientation_Vector[X] * z) +
		            (Warp->Orientation_Vector[Y] * x) +
		            (Warp->Orientation_Vector[Z] * x);
		EPoint[Y] = (Warp->Orientation_Vector[X] * y) +
		            (Warp->Orientation_Vector[Y] * -z) +
		            (Warp->Orientation_Vector[Z] * y);
		EPoint[Z] = (Warp->Orientation_Vector[X] * -x) +
		            (Warp->Orientation_Vector[Y] * y) +
		            (Warp->Orientation_Vector[Z] * z);
	}

	return 1;
}

/*****************************************************************************
*
* FUNCTION 
*    warp_cubic
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*  Nieminen Juha
*
* DESCRIPTION
*   Warps a point from the surface of an origin-centered cube to the xy-plane,
*   similar to how an uv-mapped box works.
*
* CHANGES
*
******************************************************************************/
static int warp_cubic(VECTOR EPoint)
{
	DBL x = EPoint[X], y = EPoint[Y], z = EPoint[Z];
	const DBL ax = fabs(x), ay = fabs(y), az = fabs(z);

	if(x >= 0 && x >= ay && x >= az)
	{
		EPoint[X] = 0.75 - 0.25*(z/x+1.0)/2.0;
		EPoint[Y] = 1.0/3.0 + (1.0/3.0)*(y/x+1.0)/2.0;
		EPoint[Z] = x;
	}
	else if(y >= 0 && y >= ax && y >= az)
	{
		EPoint[X] = 0.25 + 0.25*(x/y+1.0)/2.0;
		EPoint[Y] = 1.0 - (1.0/3.0)*(z/y+1.0)/2.0;
		EPoint[Z] = y;
	}
	else if(z >= 0 && z >= ax && z >= ay)
	{
		EPoint[X] = 0.25 + 0.25*(x/z+1.0)/2.0;
		EPoint[Y] = 1.0/3.0 + (1.0/3.0)*(y/z+1.0)/2.0;
		EPoint[Z] = z;
	}
	else if(x < 0 && x <= -ay && x <= -az)
	{
		x = -x;
		EPoint[X] = 0.25*(z/x+1.0)/2.0;
		EPoint[Y] = 1.0/3.0 + (1.0/3.0)*(y/x+1.0)/2.0;
		EPoint[Z] = x;
	}
	else if(y < 0 && y <= -ax && y <= -az)
	{
		y = -y;
		EPoint[X] = 0.25 + 0.25*(x/y+1.0)/2.0;
		EPoint[Y] = (1.0/3.0)*(z/y+1.0)/2.0;
		EPoint[Z] = y;
	}
	else
	{
		z = -z;
		EPoint[X] = 1.0 - 0.25*(x/z+1.0)/2.0;
		EPoint[Y] = 1.0/3.0 + (1.0/3.0)*(y/z+1.0)/2.0;
		EPoint[Z] = z;
	}

	return 1;
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

WARP *Create_Warp (int Warp_Type)
{
	WARP *New;
	TURB *TNew;
	REPEAT *RNew;
	TRANS *TRNew;
	BLACK_HOLE *BNew;
	TOROIDAL *TorNew;
	SPHEREW *SNew;
	CYLW *CNew;
	PLANARW *PNew;

	New = NULL;

	switch (Warp_Type)
	{
		case CLASSIC_TURB_WARP:
		case EXTRA_TURB_WARP:

			TNew = reinterpret_cast<TURB *>(POV_MALLOC(sizeof(TURB),"turbulence struct"));

			Make_Vector(TNew->Turbulence,0.0,0.0,0.0);

			TNew->Octaves = 6;
			TNew->Omega = 0.5;
			TNew->Lambda = 2.0;

			New = reinterpret_cast<WARP *>(TNew);

			break;

		case REPEAT_WARP:

			RNew = reinterpret_cast<REPEAT *>(POV_MALLOC(sizeof(REPEAT),"repeat warp"));

			RNew->Axis = -1;
			RNew->Width = 0.0;

			Make_Vector(RNew->Offset,0.0,0.0,0.0);
			Make_Vector(RNew->Flip,1.0,1.0,1.0);

			New = reinterpret_cast<WARP *>(RNew);

			break;

		case BLACK_HOLE_WARP:
			BNew = reinterpret_cast<BLACK_HOLE *>(POV_MALLOC (sizeof (BLACK_HOLE), "black hole warp")) ;
			Make_Vector (BNew->Center, 0.0, 0.0, 0.0) ;
			Make_Vector (BNew->Repeat_Vector, 0.0, 0.0, 0.0) ;
			Make_Vector (BNew->Uncertainty_Vector, 0.0, 0.0, 0.0) ;
			BNew->Strength = 1.0 ;
			BNew->Power = 2.0 ;
			BNew->Radius = 1.0 ;
			BNew->Radius_Squared = 1.0 ;
			BNew->Inverse_Radius = 1.0 ;
			BNew->Inverted = false ;
			BNew->Type = 0 ;
			BNew->Repeat = false ;
			BNew->Uncertain = false ;
			New = reinterpret_cast<WARP *>(BNew) ;
			break ;

		case TRANSFORM_WARP:

			TRNew = reinterpret_cast<TRANS *>(POV_MALLOC(sizeof(TRANS),"pattern transform"));

			MIdentity (TRNew->Trans.matrix);
			MIdentity (TRNew->Trans.inverse);

			New = reinterpret_cast<WARP *>(TRNew);

			break;

		case SPHERICAL_WARP:
			SNew = reinterpret_cast<SPHEREW *>(POV_MALLOC(sizeof(SPHEREW),"spherical warp"));
			Make_Vector (SNew->Orientation_Vector, 0.0, 0.0, 1.0) ;
			SNew->DistExp = 0.0;
			New = reinterpret_cast<WARP *>(SNew);
			break;

		case PLANAR_WARP:
			PNew = reinterpret_cast<PLANARW *>(POV_MALLOC(sizeof(PLANARW),"planar warp"));
			Make_Vector (PNew->Orientation_Vector, 0.0, 0.0, 1.0) ;
			PNew->OffSet = 0.0;
			New = reinterpret_cast<WARP *>(PNew);
			break;

		case CYLINDRICAL_WARP:
			CNew = reinterpret_cast<CYLW *>(POV_MALLOC(sizeof(CYLW),"cylindrical warp"));
			Make_Vector (CNew->Orientation_Vector, 0.0, 0.0, 1.0) ;
			CNew->DistExp = 0.0;
			New = reinterpret_cast<WARP *>(CNew);
			break;

		case TOROIDAL_WARP:
			TorNew = reinterpret_cast<TOROIDAL *>(POV_MALLOC(sizeof(TOROIDAL),"toroidal warp"));
			TorNew->MajorRadius = 1.0 ;
			TorNew->DistExp = 0.0;
			Make_Vector (TorNew->Orientation_Vector, 0.0, 0.0, 1.0) ;
			New = reinterpret_cast<WARP *>(TorNew);
			break;

		// JN2007: Cubic warp
		case CUBIC_WARP:
			New = reinterpret_cast<WARP *>(POV_MALLOC(sizeof(WARP),"cubic warp"));
			break;

		default:
			throw POV_EXCEPTION_STRING("Unknown Warp type.");
	}

	New->Warp_Type = Warp_Type;
	New->Prev_Warp = NULL;
	New->Next_Warp = NULL;

	return(New);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Destroy_Warps (WARP *Warps)
{
	WARP *Temp1 = Warps;
	WARP *Temp2;

	while (Temp1!=NULL)
	{
		Temp2 = Temp1->Next_Warp;

		POV_FREE(Temp1);

		Temp1 = Temp2;
	}
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

WARP *Copy_Warps (const WARP *Old)
{
	WARP *New;

	if (Old != NULL)
	{
		New=Create_Warp(Old->Warp_Type);

		switch (Old->Warp_Type)
		{
			case CYLINDRICAL_WARP:
				POV_MEMCPY(New,Old,sizeof(CYLW));
				break;

			case PLANAR_WARP:
				POV_MEMCPY(New,Old,sizeof(PLANARW));
				break;

			case SPHERICAL_WARP:
				POV_MEMCPY(New,Old,sizeof(SPHEREW));
				break;

			case TOROIDAL_WARP:
				POV_MEMCPY(New,Old,sizeof(TOROIDAL));
				break;

			case CLASSIC_TURB_WARP:
			case EXTRA_TURB_WARP:
				POV_MEMCPY(New,Old,sizeof(TURB));
				break;

			case REPEAT_WARP:
				POV_MEMCPY(New,Old,sizeof(REPEAT));
				break;

			case BLACK_HOLE_WARP:
				POV_MEMCPY(New,Old,sizeof(BLACK_HOLE));
				break;

			case TRANSFORM_WARP:
				POV_MEMCPY(New,Old,sizeof(TRANS));
				break;

			// JN2007: Cubic warp
			case CUBIC_WARP:
				POV_MEMCPY(New,Old,sizeof(WARP));
				break;
		}
		New->Next_Warp = Copy_Warps(Old->Next_Warp);
		if(New->Next_Warp != NULL)
			New->Next_Warp->Prev_Warp = New;
	}
	else
	{
		New = NULL;
	}
	return(New);
}

}
