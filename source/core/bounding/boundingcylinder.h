//******************************************************************************
///
/// @file core/bounding/boundingcylinder.h
///
/// Declarations related to bounding cylinders (used by lathe and sor).
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

#ifndef POVRAY_CORE_BOUNDINGCYLINDER_H
#define POVRAY_CORE_BOUNDINGCYLINDER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/math/vector.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Generate additional bcyl statistics. */

#define BCYL_EXTRA_STATS 1


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct BCyl_Struct BCYL;
typedef struct BCyl_Entry_Struct BCYL_ENTRY;
typedef struct BCyl_Intersection_Struct BCYL_INT;

struct BCyl_Intersection_Struct
{
    int n;     /* Number of cylinder hit    */
    DBL d[2];  /* Intersection distance(s)  */
    DBL w[2];  /* Intersection parameter(s) */
};

struct BCyl_Entry_Struct
{
    short r1, r2;        /* Index of min/max segment radius */
    short h1, h2;        /* Index of min/max segmnet height */
};

struct BCyl_Struct
{
    int number;          /* Number of bounding cylinders.       */
    short nradius;       /* Number of different bound-radii.    */
    short nheight;       /* Number of different bound-heights.  */
    DBL *radius;         /* List of different bound-radii.      */
    DBL *height;         /* List of different bound-heights.    */
    BCYL_ENTRY *entry;   /* BCyl elements.                      */
};



/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

BCYL *Create_BCyl (int, const DBL *, const DBL *, const DBL *, const DBL *);
void Destroy_BCyl (BCYL *);

int Intersect_BCyl (const BCYL *BCyl, vector<BCYL_INT>& Intervals, vector<BCYL_INT>& rint, vector<BCYL_INT>& hint, const Vector3d& P, const Vector3d& D);

}

#endif // POVRAY_CORE_BOUNDINGCYLINDER_H
