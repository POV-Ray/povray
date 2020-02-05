/****************************************************************************
*                   poly.h
*
*  This module contains all defines, typedefs, and prototypes for POLY.C.
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


#ifndef POLY_H
#define POLY_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define POLY_OBJECT    (STURM_OK_OBJECT)
#define CUBIC_OBJECT   (STURM_OK_OBJECT)
#define QUARTIC_OBJECT (STURM_OK_OBJECT)

/* Number of coefficients of a three variable polynomial of order x */

#define term_counts(x) (((x)+1)*((x)+2)*((x)+3)/6)



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Poly_Struct POLY;

struct Poly_Struct
{
  OBJECT_FIELDS
  TRANSFORM *Trans;
  int Order;
  DBL *Coeffs;
};



/*****************************************************************************
* Global variables
******************************************************************************/

extern METHODS Poly_Methods;



/*****************************************************************************
* Global functions
******************************************************************************/

POLY *Create_Poly (int Order);
void Compute_Poly_BBox (POLY *Poly);



#endif
