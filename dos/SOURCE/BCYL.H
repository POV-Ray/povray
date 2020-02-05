/****************************************************************************
*                   bcyl.h
*
*  This module contains all defines, typedefs, and prototypes for BCYL.C.
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

#ifndef BCYL_H
#define BCYL_H



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
  BCYL_INT *rint;      /* BCyl intersections list.            */
  BCYL_INT *hint;      /* BCyl intersections list.            */
  BCYL_INT *intervals; /* BCyl intersection intervals.        */
  BCYL_ENTRY *entry;   /* BCyl elements.                      */
};



/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

BCYL *Create_BCyl (int, DBL *, DBL *, DBL *, DBL *);
void Destroy_BCyl (BCYL *);

int  Intersect_BCyl (BCYL *BCyl, VECTOR P, VECTOR D);


#endif
