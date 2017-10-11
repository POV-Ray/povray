/****************************************************************************
*                   prism.h
*
*  This module contains all defines, typedefs, and prototypes for PRISM.C.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1999 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by email to team-coord@povray.org or visit us on the web at
*  http://www.povray.org. The latest version of POV-Ray may be found at this site.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/


#ifndef PRISM_H
#define PRISM_H



/*****************************************************************************
* Global preprocessor definitions
******************************************************************************/

#define PRISM_OBJECT (STURM_OK_OBJECT)

#define LINEAR_SPLINE    1
#define QUADRATIC_SPLINE 2
#define CUBIC_SPLINE     3
#define BEZIER_SPLINE    4

#define LINEAR_SWEEP 1
#define CONIC_SWEEP  2

/* Generate additional prism statistics. */

#define PRISM_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Prism_Struct PRISM;
typedef struct Prism_Spline_Struct PRISM_SPLINE;
typedef struct Prism_Spline_Entry_Struct PRISM_SPLINE_ENTRY;
typedef struct Prism_Intersection_Structure PRISM_INT;

struct Prism_Intersection_Structure
{
  DBL d;  /* Distance of intersection point                  */
  DBL w;  /* Paramter of intersection point on n-th spline   */
  int n;  /* Number of segment hit                           */
  int t;  /* Type of intersection: base/cap plane or segment */
};

struct Prism_Spline_Entry_Struct
{
  DBL x1, y1, x2, y2;  /* Min./Max. coordinates of segment   */
  DBL v1, u2, v2;      /* Min./Max. coordinates of segment in <u,v>, u1 not needed  */
  UV_VECT A, B, C, D;  /* Coefficients of segment            */
};

struct Prism_Spline_Struct
{
  int References;
  PRISM_SPLINE_ENTRY *Entry;
};

struct Prism_Struct
{
  OBJECT_FIELDS
  int Number;
  int Spline_Type;          /* Spline type (linear, quadratic ...)        */
  int Sweep_Type;           /* Sweep type (linear, conic)                 */
  TRANSFORM *Trans;
  DBL Height1, Height2;
  DBL x1, y1, x2, y2;       /* Overall bounding rectangle of spline curve */
  PRISM_SPLINE *Spline;     /* Pointer to array of splines                */
  PRISM_INT *Intersections; /* Prism intersections list                   */
  DBL u1, v1, u2, v2;       /* Overall <u,v> bounding rectangle of spline */
};



/*****************************************************************************
* Global variables
******************************************************************************/




/*****************************************************************************
* Global functions
******************************************************************************/

PRISM *Create_Prism (void);
void  Compute_Prism_BBox (PRISM *Prism);
void  Compute_Prism (PRISM *Prism, UV_VECT *P);



#endif
