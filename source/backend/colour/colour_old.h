/*******************************************************************************
 * colour_old.h
 *
 * This module contains all defines, typedefs, and prototypes for colour_old.cpp.
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
 * $File: //depot/povray/smp/source/backend/colour/colour_old.h $
 * $Revision: #1 $
 * $Change: 6113 $
 * $DateTime: 2013/11/20 20:39:54 $
 * $Author: clipka $
 *******************************************************************************/

/* NOTE: FRAME.H contains other colour stuff. */

#ifndef COLOUR_OLD_H
#define COLOUR_OLD_H

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/


/*****************************************************************************
* Global typedefs
******************************************************************************/


/*****************************************************************************
* Global variables
******************************************************************************/


/*****************************************************************************
* Global functions
******************************************************************************/

// TODO - obsolete
COLOUR *Create_Colour (void);
COLOUR *Copy_Colour (const COLOUR Old);

// TODO - refactor
BLEND_MAP_ENTRY *Create_BMap_Entries (int Map_Size);
BLEND_MAP *Create_Blend_Map (void);
BLEND_MAP *Copy_Blend_Map (BLEND_MAP *Old);
void Destroy_Blend_Map (BLEND_MAP *BMap);


/*****************************************************************************
* Inline functions
******************************************************************************/

}

#endif
