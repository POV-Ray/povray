/*******************************************************************************
 * warps.h
 *
 * This module contains all defines, typedefs, and prototypes for WARPS.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/pattern/warps.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

/* NOTE: FRAME.H contains other warp stuff. */

#ifndef WARP_H
#define WARP_H

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Warp types */
#define NO_WARP             0
#define CLASSIC_TURB_WARP   1
#define REPEAT_WARP         2
#define BLACK_HOLE_WARP     3
#define EXTRA_TURB_WARP     4
#define TRANSFORM_WARP      5
#define CYLINDRICAL_WARP    6
#define SPHERICAL_WARP      7
#define TOROIDAL_WARP       8
#define PLANAR_WARP         9
// JN2007: Cubic warp
#define CUBIC_WARP         10


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Repeat_Warp REPEAT;
typedef struct Trans_Warp TRANS;
typedef struct Black_Hole_Warp BLACK_HOLE;
typedef struct Spherical_Warp SPHEREW;
typedef struct Cylindrical_Warp CYLW;
typedef struct Toroidal_Warp TOROIDAL;
typedef struct Planar_Warp PLANARW;

struct Toroidal_Warp : public Warps_Struct
{
	VECTOR Orientation_Vector;
	DBL DistExp;
	DBL MajorRadius;
};

struct Cylindrical_Warp : public Warps_Struct
{
	VECTOR Orientation_Vector;
	DBL DistExp;
};

struct Planar_Warp : public Warps_Struct
{
	VECTOR Orientation_Vector;
	DBL OffSet;
};

struct Spherical_Warp : public Warps_Struct
{
	VECTOR Orientation_Vector;
	DBL DistExp;
};

struct Repeat_Warp : public Warps_Struct
{
	int Axis;
	SNGL Width;
	VECTOR Flip, Offset;
};

struct Trans_Warp : public Warps_Struct
{
	TRANSFORM Trans;
};

struct Black_Hole_Warp : public Warps_Struct
{
	VECTOR      Center ;
	VECTOR      Repeat_Vector ;
	VECTOR      Uncertainty_Vector ;
	DBL         Strength ;
	DBL         Radius ;
	DBL         Radius_Squared ;
	DBL         Inverse_Radius ;
	DBL         Power ;
	short       Inverted ;
	short       Type ;
	short       Repeat ;
	short       Uncertain ;
} ;


/*****************************************************************************
* Global variables
******************************************************************************/


/*****************************************************************************
* Global constants
******************************************************************************/


/*****************************************************************************
* Global functions
******************************************************************************/

void Warp_EPoint (VECTOR TPoint, const VECTOR EPoint, const TPATTERN *TPat);
WARP *Create_Warp (int Warp_Type);
void Destroy_Warps (WARP *Warps);
WARP *Copy_Warps (const WARP *Old);
void Warp_Normal (VECTOR TNorm, const VECTOR ENorm, const TPATTERN *TPat, bool DontScaleBumps);
void UnWarp_Normal (VECTOR TNorm, const VECTOR ENorm, const TPATTERN *TPat, bool DontScaleBumps);

}

#endif
