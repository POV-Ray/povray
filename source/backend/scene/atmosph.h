/*******************************************************************************
 * atmosph.h
 *
 * This module contains all defines, typedefs, and prototypes for ATMOSPH.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/scene/atmosph.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef ATMOSPH_H
#define ATMOSPH_H

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Define fog types. DMF */

#define ORIG_FOG    1
#define GROUND_MIST 2
#define FOG_TYPES   2

/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Fog_Struct FOG;
typedef struct Rainbow_Struct RAINBOW;
typedef struct Skysphere_Struct SKYSPHERE;

struct Fog_Struct
{
	int Type;
	DBL Distance;
	DBL Alt;
	DBL Offset;
	Colour colour; // may have a filter/transmit component
	VECTOR Up;
	TURB *Turb;
	SNGL Turb_Depth;
	FOG *Next;
};

struct Rainbow_Struct
{
	DBL Distance;
	DBL Jitter;
	DBL Angle, Width;
	DBL Arc_Angle, Falloff_Angle, Falloff_Width;
	VECTOR Antisolar_Vector;
	VECTOR Up_Vector, Right_Vector;
	PIGMENT *Pigment;
	RAINBOW *Next;
};

struct Skysphere_Struct
{
	int Count;           ///< Number of pigments.
	RGBColour Emission;  ///< Brightness adjustment.
	PIGMENT **Pigments;  ///< Pigment(s) to use.
	TRANSFORM *Trans;    ///< Skysphere transformation.
};

/*****************************************************************************
* Global functions
******************************************************************************/

FOG *Create_Fog (void);
FOG *Copy_Fog (const FOG *Fog);
void Destroy_Fog (FOG *Fog);

RAINBOW *Create_Rainbow (void);
RAINBOW *Copy_Rainbow (const RAINBOW *Rainbow);
void Destroy_Rainbow (RAINBOW *Rainbow);

SKYSPHERE *Create_Skysphere (void);
SKYSPHERE *Copy_Skysphere (const SKYSPHERE *Skysphere);
void Destroy_Skysphere (SKYSPHERE *Skysphere);
void Scale_Skysphere (SKYSPHERE *Skysphere, const VECTOR Vector);
void Rotate_Skysphere (SKYSPHERE *Skysphere, const VECTOR Vector);
void Translate_Skysphere (SKYSPHERE *Skysphere, const VECTOR Vector);
void Transform_Skysphere (SKYSPHERE *Skysphere, const TRANSFORM *Trans);

}

#endif
