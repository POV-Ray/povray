//******************************************************************************
///
/// @file backend/colour/colour_old.h
///
/// This module contains all defines, typedefs, and prototypes for
/// `colour_old.cpp`.
///
/// @note   `frame.h` contains other colour stuff.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2014 Persistence of Vision Raytracer Pty. Ltd.
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
//*******************************************************************************

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
RGBFTColour *Create_Colour (void);
RGBFTColour *Copy_Colour (const RGBFTColour* Old);

template<typename MAP_T>
shared_ptr<MAP_T> Create_Blend_Map (int type);

template<typename MAP_T>
shared_ptr<MAP_T> Copy_Blend_Map (shared_ptr<MAP_T>& Old);


/*****************************************************************************
* Inline functions
******************************************************************************/

}

#endif
