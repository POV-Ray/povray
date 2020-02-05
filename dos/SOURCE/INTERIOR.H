/****************************************************************************
*                   interior.h
*
*  This module contains all defines, typedefs, and prototypes for INTERIOR.C.
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


#ifndef INTERIOR_H
#define INTERIOR_H

#include "media.h"

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/


/*****************************************************************************
* Global typedefs
******************************************************************************/


/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

void Init_Interior (INTERIOR *);

INTERIOR *Create_Interior (void);
INTERIOR *Copy_Interior (INTERIOR *);
INTERIOR *Copy_Interior_Pointer (INTERIOR *);
void Destroy_Interior (INTERIOR *);

void Transform_Interior (INTERIOR *, TRANSFORM *);

MATERIAL *Create_Material (void);
MATERIAL *Copy_Material (MATERIAL *);
void Destroy_Material (MATERIAL *);
#endif
