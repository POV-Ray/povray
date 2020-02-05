/****************************************************************************
*                   parstxtr.h
*
*  This header file is included by all all language parsing C modules in
*  POV-Ray.
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

#ifndef PARSTXTR_H
#define PARSTXTR_H

#include "atmosph.h"
#include "interior.h"


/*****************************************************************************
* Global preprocessor defines
******************************************************************************/



/*****************************************************************************
* Global typedefs
******************************************************************************/


/*****************************************************************************
* Global variables
******************************************************************************/

extern TEXTURE *Default_Texture;



/*****************************************************************************
* Global functions
******************************************************************************/

TEXTURE *Parse_Texture (void);
void Parse_Pigment (PIGMENT **);
void Parse_Tnormal (TNORMAL **);
void Parse_Finish (FINISH **);
void Parse_Media (IMEDIA **);
void Parse_Interior (INTERIOR **);
void Parse_Media_Density_Pattern (PIGMENT **);
FOG *Parse_Fog (void);
RAINBOW *Parse_Rainbow (void);
SKYSPHERE *Parse_Skysphere (void);
IMAGE *Parse_Image (int);
void Parse_Material(MATERIAL *Material);

#endif
