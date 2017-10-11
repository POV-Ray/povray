/****************************************************************************
*                   discs.h
*
*  This module contains all defines, typedefs, and prototypes for DISCS.C.
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

#ifndef DISCS_H
#define DISCS_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define DISC_OBJECT            (BASIC_OBJECT)



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Disc_Struct DISC;

struct Disc_Struct
{
  OBJECT_FIELDS
  TRANSFORM *Trans; /* Transformation of a Disc object */
  VECTOR center;    /* Center of the disc */
  VECTOR normal;    /* Direction perpendicular to the disc (plane normal) */
  DBL d;            /* The constant part of the plane equation */
  DBL iradius2;     /* Distance from center to inner circle of the disc */
  DBL oradius2;     /* Distance from center to outer circle of the disc */
};



/*****************************************************************************
* Global variables
******************************************************************************/




/*****************************************************************************
* Global functions
******************************************************************************/

DISC *Create_Disc (void);
void Compute_Disc (DISC *Disc);

#endif
