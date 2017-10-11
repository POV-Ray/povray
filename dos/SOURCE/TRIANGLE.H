/****************************************************************************
*                   triangle.h
*
*  This module contains all defines, typedefs, and prototypes for TRIANGLE.C.
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


#ifndef TRIANGLE_H
#define TRIANGLE_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define TRIANGLE_OBJECT        (PATCH_OBJECT)
#define SMOOTH_TRIANGLE_OBJECT (PATCH_OBJECT+DOUBLE_ILLUMINATE)



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Triangle_Struct TRIANGLE;
typedef struct Smooth_Triangle_Struct SMOOTH_TRIANGLE;

struct Triangle_Struct
{
  OBJECT_FIELDS
  VECTOR  Normal_Vector;
  DBL     Distance;
  unsigned int  Dominant_Axis:2;
  unsigned int  vAxis:2;  /* used only for smooth triangles */
  VECTOR  P1, P2, P3;
};

struct Smooth_Triangle_Struct
{
  OBJECT_FIELDS
  VECTOR  Normal_Vector;
  DBL     Distance;
  unsigned int  Dominant_Axis:2;
  unsigned int  vAxis:2;         /* used only for smooth triangles */
  VECTOR  P1, P2, P3;
  VECTOR  N1, N2, N3, Perp;
};



/*****************************************************************************
* Global variables
******************************************************************************/

extern METHODS Triangle_Methods;
extern METHODS Smooth_Triangle_Methods;



/*****************************************************************************
* Global functions
******************************************************************************/

TRIANGLE *Create_Triangle (void);
SMOOTH_TRIANGLE *Create_Smooth_Triangle (void);
int Compute_Triangle  (TRIANGLE *Triangle, int Smooth);
void Compute_Triangle_BBox (TRIANGLE *Triangle);



#endif
