/*******************************************************************************
 * atmosph.cpp
 *
 * This module contains all functions for atmospheric effects.
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
 * $File: //depot/povray/smp/source/backend/scene/atmosph.cpp $
 * $Revision: #25 $
 * $Change: 6085 $
 * $DateTime: 2013/11/10 07:39:29 $
 * $Author: clipka $
 *******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/scene/atmosph.h"
#include "backend/texture/texture.h"
#include "backend/texture/pigment.h"
#include "backend/pattern/warps.h"
#include "backend/math/vector.h"
#include "backend/math/matrices.h"

#include "povray.h" // TODO

#include <algorithm>

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
*
* FUNCTION
*
*   Create_Fog
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   FOG * - created fog
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a fog.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

FOG *Create_Fog()
{
	FOG *New;

	New = new FOG;

	New->Type = ORIG_FOG;

	New->Distance = 0.0;
	New->Alt      = 0.0;
	New->Offset   = 0.0;

	New->colour.clear();

	Make_Vector(New->Up, 0.0, 1.0, 0.0);

	New->Turb = NULL;
	New->Turb_Depth = 0.5;

	New->Next = NULL;

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Fog
*
* INPUT
*
*   Old - fog to copy
*
* OUTPUT
*
* RETURNS
*
*   FOG * - new fog
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a fog.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

FOG *Copy_Fog(const FOG *Old)
{
	FOG *New;

	New = Create_Fog();

	*New = *Old;

	New->Turb = reinterpret_cast<TURB *>(Copy_Warps((reinterpret_cast<WARP *>(Old->Turb))));

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Fog
*
* INPUT
*
*   Fog - fog to destroy
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Destroy a fog.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Destroy_Fog(FOG *Fog)
{
	if (Fog != NULL)
	{
		Destroy_Turb(Fog->Turb);

		delete Fog;
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Rainbow
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   RAINBOW * - created rainbow
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a rainbow.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

RAINBOW *Create_Rainbow()
{
	RAINBOW *New;

	New = new RAINBOW;

	New->Distance = MAX_DISTANCE;
	New->Jitter   = 0.0;
	New->Angle    = 0.0;
	New->Width    = 0.0;

	New->Falloff_Width  = 0.0;
	New->Arc_Angle      = 180.0;
	New->Falloff_Angle  = 180.0;

	New->Pigment = NULL;

	Make_Vector(New->Antisolar_Vector, 0.0, 0.0, 0.0);

	Make_Vector(New->Right_Vector, 1.0, 0.0, 0.0);
	Make_Vector(New->Up_Vector, 0.0, 1.0, 0.0);

	New->Next = NULL;

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Rainbow
*
* INPUT
*
*   Old - rainbow to copy
*
* OUTPUT
*
* RETURNS
*
*   RAINBOW * - new rainbow
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a rainbow.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

RAINBOW *Copy_Rainbow(const RAINBOW *Old)
{
	RAINBOW *New;

	New = Create_Rainbow();

	*New = *Old;

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Rainbow
*
* INPUT
*
*   Rainbow - rainbow to destroy
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Destroy a rainbow.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Destroy_Rainbow(RAINBOW *Rainbow)
{
	if (Rainbow != NULL)
	{
		Destroy_Pigment(Rainbow->Pigment);

		delete Rainbow;
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Skysphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   SKYSPHERE * - created skysphere
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a skysphere.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

SKYSPHERE *Create_Skysphere()
{
	SKYSPHERE *New;

	New = new SKYSPHERE;

	New->Count = 0;
	New->Emission = RGBColour(1.0);

	New->Pigments = NULL;

	New->Trans = Create_Transform();

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Skysphere
*
* INPUT
*
*   Old - skysphere to copy
*
* OUTPUT
*
* RETURNS
*
*   SKYSPHERE * - copied skysphere
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a skysphere.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

SKYSPHERE *Copy_Skysphere(const SKYSPHERE *Old)
{
	int i;
	SKYSPHERE *New;

	New = Create_Skysphere();

	Destroy_Transform(New->Trans);

	*New = *Old;

	New->Trans = Copy_Transform(Old->Trans);

	if (New->Count > 0)
	{
		New->Pigments = reinterpret_cast<PIGMENT **>(POV_MALLOC(New->Count*sizeof(PIGMENT *), "skysphere pigment"));

		for (i = 0; i < New->Count; i++)
		{
			New->Pigments[i] = Copy_Pigment(Old->Pigments[i]);
		}
	}

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Skysphere
*
* INPUT
*
*   Skysphere - skysphere to destroy
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Destroy a skysphere.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Destroy_Skysphere(SKYSPHERE *Skysphere)
{
	int i;

	if (Skysphere != NULL)
	{
		for (i = 0; i < Skysphere->Count; i++)
		{
			Destroy_Pigment(Skysphere->Pigments[i]);
		}

		POV_FREE(Skysphere->Pigments);

		Destroy_Transform(Skysphere->Trans);

		delete Skysphere;
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Skysphere
*
* INPUT
*
*   Vector - Rotation vector
*
* OUTPUT
*
*   Skysphere - Pointer to skysphere structure
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Rotate a skysphere.
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Rotate_Skysphere(SKYSPHERE *Skysphere, const VECTOR Vector)
{
	TRANSFORM Trans;

	Compute_Rotation_Transform(&Trans, Vector);

	Transform_Skysphere(Skysphere, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Skysphere
*
* INPUT
*
*   Vector - Scaling vector
*
* OUTPUT
*
*   Skysphere - Pointer to skysphere structure
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Scale a skysphere.
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Scale_Skysphere(SKYSPHERE *Skysphere, const VECTOR Vector)
{
	TRANSFORM Trans;

	Compute_Scaling_Transform(&Trans, Vector);

	Transform_Skysphere(Skysphere, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Skysphere
*
* INPUT
*
*   Vector - Translation vector
*
* OUTPUT
*
*   Skysphere - Pointer to skysphere structure
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Translate a skysphere.
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Translate_Skysphere(SKYSPHERE *Skysphere, const VECTOR Vector)
{
	TRANSFORM Trans;

	Compute_Translation_Transform(&Trans, Vector);

	Transform_Skysphere(Skysphere, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Skysphere
*
* INPUT
*
*   Trans  - Pointer to transformation
*
* OUTPUT
*
*   Skysphere - Pointer to skysphere structure
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Transform a skysphere.
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Transform_Skysphere(SKYSPHERE *Skysphere, const TRANSFORM *Trans)
{
	if (Skysphere->Trans == NULL)
	{
		Skysphere->Trans = Create_Transform();
	}

	Compose_Transforms(Skysphere->Trans, Trans);
}

}
