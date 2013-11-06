/*******************************************************************************
 * pigment.h
 *
 * This module contains all defines, typedefs, and prototypes for PIGMENT.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/texture/pigment.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

/* NOTE: FRAME.H contains other pigment stuff. */

#ifndef PIGMENT_H
#define PIGMENT_H

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
* Global constants
******************************************************************************/

extern const BLEND_MAP Brick_Default_Map;
extern const BLEND_MAP Hex_Default_Map;
extern const BLEND_MAP Square_Default_Map;
extern const BLEND_MAP Triangular_Default_Map;
extern const BLEND_MAP Cubic_Default_Map; // JN2007: Cubic pattern
extern const BLEND_MAP Check_Default_Map;


/*****************************************************************************
* Global functions
******************************************************************************/

PIGMENT *Create_Pigment();
PIGMENT *Copy_Pigment(const PIGMENT *Old);
void Destroy_Pigment(PIGMENT *Pigment);
int Post_Pigment(PIGMENT *Pigment);
bool Compute_Pigment(Colour& colour, const PIGMENT *Pigment, const VECTOR IPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);
void Evaluate_Density_Pigment(const PIGMENT *pigm, const Vector3d& p, RGBColour& c, TraceThreadData *ttd);

}

#endif
