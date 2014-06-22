//******************************************************************************
///
/// @file backend/texture/pigment.h
///
/// This module contains all defines, typedefs, and prototypes for
/// `pigment.cpp`.
///
/// @note   `frame.h` contains other pigment stuff.
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
//******************************************************************************

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


/*****************************************************************************
* Global functions
******************************************************************************/

PIGMENT *Create_Pigment();
PIGMENT *Copy_Pigment(PIGMENT *Old);
void Copy_Pigments (vector<PIGMENT*>& New, const vector<PIGMENT*>& Old);
void Destroy_Pigment(PIGMENT *Pigment);
void Post_Pigment(PIGMENT *Pigment, bool* pHasFilter = NULL);
bool Compute_Pigment(TransColour& colour, const PIGMENT *Pigment, const Vector3d& IPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);
void Evaluate_Density_Pigment(vector<PIGMENT*>& Density, const Vector3d& p, RGBColour& c, TraceThreadData *ttd);

}

#endif
