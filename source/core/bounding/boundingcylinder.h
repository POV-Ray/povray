//******************************************************************************
///
/// @file core/bounding/boundingcylinder.h
///
/// Declarations related to bounding cylinders (used by lathe and sor).
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_CORE_BOUNDINGCYLINDER_H
#define POVRAY_CORE_BOUNDINGCYLINDER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"
#include "core/bounding/boundingcylinder_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <vector>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/math/vector.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreBoundingCylinder Bounding Cylinders
/// @ingroup PovCoreBounding
///
/// @{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Generate additional bcyl statistics. */

#define BCYL_EXTRA_STATS 1


/*****************************************************************************
* Global typedefs
******************************************************************************/

struct BCyl_Intersection_Struct final
{
    int n;     /* Number of cylinder hit    */
    DBL d[2];  /* Intersection distance(s)  */
    DBL w[2];  /* Intersection parameter(s) */
};
using BCYL_INT = BCyl_Intersection_Struct; ///< @deprecated

struct BCyl_Entry_Struct final
{
    short r1, r2;        /* Index of min/max segment radius */
    short h1, h2;        /* Index of min/max segmnet height */
};
using BCYL_ENTRY = BCyl_Entry_Struct; ///< @deprecated

struct BCyl_Struct final
{
    int number;          /* Number of bounding cylinders.       */
    short nradius;       /* Number of different bound-radii.    */
    short nheight;       /* Number of different bound-heights.  */
    DBL *radius;         /* List of different bound-radii.      */
    DBL *height;         /* List of different bound-heights.    */
    BCYL_ENTRY *entry;   /* BCyl elements.                      */
};


/*****************************************************************************
* Global functions
******************************************************************************/

BCYL *Create_BCyl (int, const DBL *, const DBL *, const DBL *, const DBL *);
void Destroy_BCyl (BCYL *);

int Intersect_BCyl (const BCYL *BCyl, std::vector<BCYL_INT>& Intervals, std::vector<BCYL_INT>& rint, std::vector<BCYL_INT>& hint, const Vector3d& P, const Vector3d& D);

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_BOUNDINGCYLINDER_H
