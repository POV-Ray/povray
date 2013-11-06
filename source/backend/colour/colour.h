/*******************************************************************************
 * colour.h
 *
 * This module contains all defines, typedefs, and prototypes for COLOUR.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/colour/colour.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

/* NOTE: FRAME.H contains other colour stuff. */

#ifndef COLOUR_H
#define COLOUR_H

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define RED2GRAY   0.297
#define GREEN2GRAY 0.589
#define BLUE2GRAY  0.114
#define GREY_SCALE3(CR,CG,CB) (RED2GRAY*(CR) + GREEN2GRAY*(CG) + BLUE2GRAY*(CB))
#define GREY_SCALE(C) GREY_SCALE3((C)[pRED],(C)[pGREEN],(C)[pBLUE])

/*****************************************************************************
* Global typedefs
******************************************************************************/




/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

COLOUR *Create_Colour (void);
COLOUR *Copy_Colour (const COLOUR Old);
BLEND_MAP_ENTRY *Create_BMap_Entries (int Map_Size);
BLEND_MAP_ENTRY *Copy_BMap_Entries (const BLEND_MAP_ENTRY *Old, int Map_Size, int Type);
BLEND_MAP *Create_Blend_Map (void);
BLEND_MAP *Copy_Blend_Map (BLEND_MAP *Old);
DBL Colour_Distance_RGBT (const Colour& colour1, const Colour& colour2);
void Destroy_Blend_Map (BLEND_MAP *BMap);


/*****************************************************************************
* Inline functions
******************************************************************************/

}

#endif
