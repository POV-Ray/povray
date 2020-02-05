/****************************************************************************
*                   sor.h
*
*  This module contains all defines, typedefs, and prototypes for SOR.C.
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



#ifndef SOR_H
#define SOR_H

#include "bcyl.h"



/*****************************************************************************
* Global preprocessor definitions
******************************************************************************/

#define SOR_OBJECT (STURM_OK_OBJECT)

/* Generate additional surface of revolution statistics. */

#define SOR_EXTRA_STATS 1




/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Sor_Struct SOR;
typedef struct Sor_Spline_Entry_Struct SOR_SPLINE_ENTRY;
typedef struct Sor_Spline_Struct SOR_SPLINE;

struct Sor_Spline_Entry_Struct
{
  DBL A, B, C, D;
};

struct Sor_Spline_Struct
{
  int References;
  SOR_SPLINE_ENTRY *Entry;
  BCYL *BCyl;                 /* bounding cylinder.                  */
};

struct Sor_Struct
{
  OBJECT_FIELDS
  TRANSFORM *Trans;
  int Number;
  SOR_SPLINE *Spline;      /* List of spline segments     */
  DBL Height1, Height2;    /* Min./Max. height            */
  DBL Radius1, Radius2;    /* Min./Max. radius            */
  DBL Base_Radius_Squared; /* Radius**2 of the base plane */
  DBL Cap_Radius_Squared;  /* Radius**2 of the cap plane  */
};



/*****************************************************************************
* Global variables
******************************************************************************/




/*****************************************************************************
* Global functions
******************************************************************************/

SOR  *Create_Sor (void);
void Compute_Sor_BBox (SOR *Sor);
void Compute_Sor (SOR *Sor, UV_VECT *P);



#endif
