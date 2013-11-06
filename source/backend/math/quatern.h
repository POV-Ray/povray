/*******************************************************************************
 * quatern.h
 *
 * This module contains all defines, typedefs, and prototypes for quatern.cpp.
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
 * $File: //depot/public/povray/3.x/source/backend/math/quatern.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef JULIA_H
#define JULIA_H

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

int F_Bound_Julia (const Ray & Ray, const Fractal * Fractal, DBL * Depth_Min, DBL * Depth_Max);
void Normal_Calc_Julia (VECTOR Result, int N_Max, const Fractal *fractal, DBL **);
void Normal_Calc_z3 (VECTOR Result, int N_Max, const Fractal *fractal, DBL **);
int Iteration_Julia (const VECTOR point, const Fractal * Julia, DBL **);
int D_Iteration_Julia (const VECTOR point, const Fractal *Julia, const VECTOR& Direction, DBL *Dist, DBL **);
int Iteration_z3 (const VECTOR point, const Fractal * Julia, DBL **);
int D_Iteration_z3 (const VECTOR point, const Fractal *Julia, const VECTOR& Direction, DBL *Dist, DBL **);

}

#endif
