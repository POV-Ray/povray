/****************************************************************************
*                   pigment.h
*
*  This module contains all defines, typedefs, and prototypes for PIGMENT.C.
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

/* NOTE: FRAME.H contains other pigment stuff. */

#ifndef PIGMENT_H
#define PIGMENT_H



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
* Global constants
******************************************************************************/

extern BLEND_MAP Brick_Default_Map;
extern BLEND_MAP Hex_Default_Map;
extern BLEND_MAP Check_Default_Map;


/*****************************************************************************
* Global functions
******************************************************************************/

PIGMENT *Create_Pigment (void);
PIGMENT *Copy_Pigment (PIGMENT *Old);
void Destroy_Pigment (PIGMENT *Pigment);
int Post_Pigment (PIGMENT *Pigment);
int Compute_Pigment (COLOUR Colour, PIGMENT *Pigment, VECTOR IPoint);
void Make_Pigment_Entries (void);

#endif
