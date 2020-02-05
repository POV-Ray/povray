/****************************************************************************
*                   torus.h
*
*  This module contains all defines, typedefs, and prototypes for TORUS.C.
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


#ifndef TORUS_H
#define TORUS_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define TORUS_OBJECT (STURM_OK_OBJECT)

/* Generate additional torus statistics. */

#define TORUS_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Torus_Struct TORUS;

/*
 * Torus structure.
 *
 *   R : Major radius
 *   r : Minor radius
 */

struct Torus_Struct
{
  OBJECT_FIELDS
  TRANSFORM *Trans;
  DBL R, r;
};



/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

TORUS *Create_Torus (void);
void  Compute_Torus_BBox (TORUS *Torus);
int   Test_Thick_Cylinder (VECTOR P, VECTOR D, DBL h1, DBL h2, DBL r1, DBL r2);



#endif
