/*******************************************************************************
 * colutils.cpp
 *
 * This module implements the utility functions for colors.
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
 * $File: //depot/public/povray/3.x/source/backend/colour/colutils.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/colour/colutils.h"
#include "backend/colour/colour.h"
#include "backend/math/vector.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
*
* FUNCTION
*
*   colour2photonRgbe
*
* INPUT
*
*   
* OUTPUT
*
*   
* RETURNS
*   
* AUTHOR
*
  originally float2rgb

  Bruce Walter - http://www.graphics.cornell.edu/online/formats/rgbe/

 This file contains code to read and write four byte rgbe file format
 developed by Greg Ward.  It handles the conversions between rgbe and
 pixels consisting of floats.  The data is assumed to be an array of floats.
 By default there are three floats per pixel in the order red, green, blue.
 (RGBE_DATA_??? values control this.)  Only the mimimal header reading and 
 writing is implemented.  Each routine does error checking and will return
 a status value as defined below.  This code is intended as a skeleton so
 feel free to modify it to suit your needs.

 (Place notice here if you modified the code.)
 posted to http://www.graphics.cornell.edu/~bjw/
 written by Bruce Walter  (bjw@graphics.cornell.edu)  5/26/95
 based on code written by Greg Ward
*   
* DESCRIPTION
*
*   standard conversion from float pixels to rgbe pixels.
*
* CHANGES
*
*  May 25, 20020 - incorporated into POV-Ray - Nathan Kopp
*                  For photons, our exponent will almost always be
*                  negative, so we use e+250 to get a larger range of negative
*                  exponents.
*
******************************************************************************/
void colour2photonRgbe(SMALL_COLOUR rgbe, const RGBColour& c)
{
	float v;
	int e;

	v = c[pRED];
	if (c[pGREEN] > v) v = c[pGREEN];
	if (c[pBLUE] > v) v = c[pBLUE];
	if (v < 1e-32) {
		rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
	}
	else {
		v = frexp(v,&e) * 256.0/v;
		rgbe[0] = (unsigned char) (c[pRED] * v);
		rgbe[1] = (unsigned char) (c[pGREEN] * v);
		rgbe[2] = (unsigned char) (c[pBLUE] * v);
		//rgbe[3] = (unsigned char) (e + 128);
		rgbe[3] = (unsigned char) (e + 250);
	}
}

/*****************************************************************************
*
* FUNCTION
*
*   photonRgbe2colour
*
* INPUT
*
*   
* OUTPUT
*
*   
* RETURNS
*   
* AUTHOR
*
  originally float2rgb

  Bruce Walter - http://www.graphics.cornell.edu/online/formats/rgbe/

 This file contains code to read and write four byte rgbe file format
 developed by Greg Ward.  It handles the conversions between rgbe and
 pixels consisting of floats.  The data is assumed to be an array of floats.
 By default there are three floats per pixel in the order red, green, blue.
 (RGBE_DATA_??? values control this.)  Only the mimimal header reading and 
 writing is implemented.  Each routine does error checking and will return
 a status value as defined below.  This code is intended as a skeleton so
 feel free to modify it to suit your needs.

 (Place notice here if you modified the code.)
 posted to http://www.graphics.cornell.edu/~bjw/
 written by Bruce Walter  (bjw@graphics.cornell.edu)  5/26/95
 based on code written by Greg Ward
*   
* DESCRIPTION
*
*   standard conversion from rgbe to float pixels
*   note: Ward uses ldexp(col+0.5,exp-(128+8)).  However we wanted pixels 
*         in the range [0,1] to map back into the range [0,1].            
*
* CHANGES
*
*  May 25, 20020 - incorporated into POV-Ray - Nathan Kopp
*                  For photons, our exponent will almost always be
*                  negative, so we use e+250 to get a larger range of negative
*                  exponents.
*
******************************************************************************/
void photonRgbe2colour(RGBColour& c, const SMALL_COLOUR rgbe)
{
	float f;

	if (rgbe[3]) {   /*nonzero pixel*/
		f = ldexp(1.0,rgbe[3]-(int)(250+8));
		c[pRED] = rgbe[0] * f;
		c[pGREEN] = rgbe[1] * f;
		c[pBLUE] = rgbe[2] * f;
	}
	else
		c.clear();
}

}
