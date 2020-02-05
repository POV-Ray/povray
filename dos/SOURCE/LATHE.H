/****************************************************************************
*                   lathe.h
*
*  This module contains all defines, typedefs, and prototypes for LATHE.C.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1998 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's GO POVRAY Forum or visit
*  http://www.povray.org. The latest version of POV-Ray may be found at these sites.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

#ifndef LATHE_H
#define LATHE_H

#include "bcyl.h"


/*****************************************************************************
* Global preprocessor definitions
******************************************************************************/

#define LATHE_OBJECT (STURM_OK_OBJECT)

#define LINEAR_SPLINE    1
#define QUADRATIC_SPLINE 2
#define CUBIC_SPLINE     3
#define BEZIER_SPLINE    4

/* Generate additional lathe statistics. */

#define LATHE_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Lathe_Struct LATHE;
typedef struct Lathe_Spline_Struct LATHE_SPLINE;
typedef struct Lathe_Spline_Entry_Struct LATHE_SPLINE_ENTRY;

struct Lathe_Spline_Entry_Struct
{
  UV_VECT A, B, C, D;  /* Coefficients of segment */
};

struct Lathe_Spline_Struct
{
  int References;             /* Count references to this structure. */
  LATHE_SPLINE_ENTRY *Entry;  /* Array of spline segments.           */
  BCYL *BCyl;                 /* bounding cylinder.                  */
};

struct Lathe_Struct
{
  OBJECT_FIELDS
  TRANSFORM *Trans;
  int Spline_Type;          /* Spline type (linear, quadratic ...)  */
  int Number;               /* Number of segments!!!                */
  LATHE_SPLINE *Spline;     /* Pointer to spline array              */
  DBL Height1, Height2;     /* Min./Max. height                     */
  DBL Radius1, Radius2;     /* Min./Max. radius                     */
};



/*****************************************************************************
* Global variables
******************************************************************************/




/*****************************************************************************
* Global functions
******************************************************************************/

void  Compute_Lathe_BBox (LATHE *Lathe);
void  Compute_Lathe (LATHE *Lathe, UV_VECT *P);
LATHE *Create_Lathe (void);

#endif
