/*******************************************************************************
 * colour.cpp
 *
 * This module implements routines to manipulate colours.
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
 * $File: //depot/public/povray/3.x/source/backend/colour/colour.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/colour/colour.h"
#include "backend/texture/texture.h"
#include "backend/texture/pigment.h"
#include "backend/texture/normal.h"
#include "backend/math/vector.h"

#include <algorithm>

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/


/*****************************************************************************
* Static functions
******************************************************************************/



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

COLOUR *Create_Colour ()
{
	COLOUR *New;

	New = (COLOUR *)POV_MALLOC(sizeof (COLOUR), "color");

	Make_ColourA (*New, 0.0, 0.0, 0.0, 0.0, 0.0);

	return (New);
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

COLOUR *Copy_Colour (const COLOUR Old)
{
	COLOUR *New;

	if (Old != NULL)
	{
		New = Create_Colour ();

		Assign_Colour(*New,Old);
	}
	else
	{
		New = NULL;
	}

	return (New);
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
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 1995 : Use POV_CALLOC to initialize entries. [DB]
*
******************************************************************************/

BLEND_MAP_ENTRY *Create_BMap_Entries (int Map_Size)
{
	BLEND_MAP_ENTRY *New;

	New = (BLEND_MAP_ENTRY *)POV_CALLOC(Map_Size, sizeof (BLEND_MAP_ENTRY), "blend map entry");

	return (New);
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
*   POV-Ray Team
*   
* DESCRIPTION
*
*
* CHANGES
*
******************************************************************************/

BLEND_MAP_ENTRY *Copy_BMap_Entries (const BLEND_MAP_ENTRY *Old, int Map_Size, int Type)
{
	int i;
	BLEND_MAP_ENTRY *New;

	if (Old != NULL)
	{
		New = Create_BMap_Entries (Map_Size);

		for (i = 0; i < Map_Size; i++)
		{
			switch (Type)
			{
				case PIGMENT_TYPE:

					New[i].Vals.Pigment = Copy_Pigment(Old[i].Vals.Pigment);

					break;

				case NORMAL_TYPE:

					New[i].Vals.Tnormal = Copy_Tnormal(Old[i].Vals.Tnormal);

					break;

				case TEXTURE_TYPE:

					New[i].Vals.Texture = Copy_Textures(Old[i].Vals.Texture);

					break;

				case COLOUR_TYPE:
				case SLOPE_TYPE:

					New[i] = Old[i];

					break;
			}
		}
	}
	else
	{
		New = NULL;
	}

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Blend_Map
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

BLEND_MAP *Create_Blend_Map ()
{
	BLEND_MAP *New;

	New = (BLEND_MAP *)POV_MALLOC(sizeof (BLEND_MAP), "blend map");

	New->Users = 1;

	New->Number_Of_Entries = 0;

	New->Type = COLOUR_TYPE;

	New->Blend_Map_Entries = NULL;

	New->Transparency_Flag = false;

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Blend_Map
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

BLEND_MAP *Copy_Blend_Map (BLEND_MAP *Old)
{
	BLEND_MAP *New;

	New = Old;

	/* 
	 * Do not increase the users field if it is negative.
	 *
	 * A negative users field incicates a reference to a static
	 * or global memory area in the data segment, not on the heap!
	 * Thus it must not be deleted later.
	 */

	if ((New != NULL) && (New->Users >= 0))
	{
		New->Users++;
	}

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Colour_Distance_RGBT
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

DBL Colour_Distance_RGBT (const Colour& colour1, const Colour& colour2)
{
	return (fabs(colour1[pRED]    - colour2[pRED]) +
	        fabs(colour1[pGREEN]  - colour2[pGREEN]) +
	        fabs(colour1[pBLUE]   - colour2[pBLUE]) +
	        fabs(colour1[pTRANSM] - colour2[pTRANSM]));
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Blend_Map
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

void Destroy_Blend_Map (BLEND_MAP *BMap)
{
	int i;
	
	if (BMap != NULL)
	{
		if ((BMap->Users > 0) && (--(BMap->Users) == 0))
		{
			for (i = 0; i < BMap->Number_Of_Entries; i++)
			{
				switch (BMap->Type)
				{
					case PIGMENT_TYPE:
					case DENSITY_TYPE:
						Destroy_Pigment(BMap->Blend_Map_Entries[i].Vals.Pigment);
						break;

					case NORMAL_TYPE:
						Destroy_Tnormal(BMap->Blend_Map_Entries[i].Vals.Tnormal);
						break;

					case TEXTURE_TYPE:
						Destroy_Textures(BMap->Blend_Map_Entries[i].Vals.Texture);
				}
			}

			POV_FREE (BMap->Blend_Map_Entries);

			POV_FREE (BMap);
		}
	}
}

}
