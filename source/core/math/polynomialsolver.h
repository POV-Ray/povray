//******************************************************************************
///
/// @file core/math/polynomialsolver.h
///
/// Declarations related to solving polynomial equations.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_POLYNOMIALSOLVER_H
#define POVRAY_POLYNOMIALSOLVER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

namespace pov
{

class RenderStatistics;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define MAX_ORDER 35


/*****************************************************************************
* Global typedefs
******************************************************************************/




/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

int Solve_Polynomial (int n, const DBL *c, DBL *r, int sturm, DBL epsilon, RenderStatistics& stats);

}

#endif // POVRAY_POLYNOMIALSOLVER_H
