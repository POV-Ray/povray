/*******************************************************************************
 * splines.h
 *
 * This module contains all defines, typedefs, and prototypes for splines.cpp.
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
 * $File: //depot/public/povray/3.x/source/backend/math/splines.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef SPLINE_H
#define SPLINE_H
/* Generic header for spline modules */

namespace pov
{

#define INIT_SPLINE_SIZE     16

#define LINEAR_SPLINE         1
#define QUADRATIC_SPLINE      2
#define NATURAL_SPLINE        3
#define CATMULL_ROM_SPLINE    4

SPLINE * Create_Spline(int Type);
SPLINE * Copy_Spline(const SPLINE * Old);
void Acquire_Spline_Reference(SPLINE * Spline);
void Release_Spline_Reference(SPLINE * Spline);
void Destroy_Spline(SPLINE * Spline);
void Insert_Spline_Entry(SPLINE * Spline, DBL p, const EXPRESS v);
DBL Get_Spline_Val(SPLINE * sp, DBL p, EXPRESS v, int *Terms);

}

#endif

