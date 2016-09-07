//******************************************************************************
///
/// @file core/bounding/boundingsphere.h
///
/// Declarations related to bounding spheres (used by blob).
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

#ifndef POVRAY_CORE_BOUNDINGSPHERE_H
#define POVRAY_CORE_BOUNDINGSPHERE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/math/vector.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct BSphere_Tree_Struct BSPHERE_TREE;

struct BSphere_Tree_Struct
{
    short Entries;       /* Number of components (node if 0)    */
    Vector3d C;          /* Center of bounding sphere           */
    DBL r2;              /* Radius^2 of bounding sphere         */
    BSPHERE_TREE **Node; /* if node: children; if leaf: element */
};



/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

void Build_Bounding_Sphere_Hierarchy (BSPHERE_TREE **Root, int nElem, BSPHERE_TREE ***Elements);
void Destroy_Bounding_Sphere_Hierarchy (BSPHERE_TREE *Node);

}

#endif // POVRAY_CORE_BOUNDINGSPHERE_H
