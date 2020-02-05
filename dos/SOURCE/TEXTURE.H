/****************************************************************************
*                   texture.h
*
*  This file contains defines and variables for the txt*.c files
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

/* NOTE: FRAME.H contains other texture stuff. */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "pattern.h"
#include "warps.h"

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define RNDMASK       0x7FFF
#define RNDMULTIPLIER ((DBL)0.000030518509476)

/*
 * Macro to create random number in the [0; 1] range.
 */

#define FRAND() ((DBL)POV_RAND()*RNDMULTIPLIER)

#define FLOOR(x)  ((x) >= 0.0 ? floor(x) : (0.0 - floor(0.0 - (x)) - 1.0))

#define Hash3d(a,b,c) \
  hashTable[(int)(hashTable[(int)(hashTable[(int)((a) & 0xfffL)] ^ ((b) & 0xfffL))] ^ ((c) & 0xfffL))]

#define Hash2d(a,b)   \
  hashTable[(int)(hashTable[(int)((a) & 0xfffL)] ^ ((b) & 0xfffL))]

#define Hash1d(a,b)   \
  hashTable[(int)(a) ^ ((b) & 0xfffL)]

#define INCRSUM(m,s,x,y,z)  \
  ((s)*(RTable[m]*0.5 + RTable[m+1]*(x) + RTable[m+2]*(y) + RTable[m+3]*(z)))

#define INCRSUMP(mp,s,x,y,z) \
  ((s)*((mp[0])*0.5 + (mp[1])*(x) + (mp[2])*(y) + (mp[3])*(z)))


/*****************************************************************************
* Global typedefs
******************************************************************************/



/*****************************************************************************
* Global variables
******************************************************************************/

extern short *hashTable;
extern DBL *frequency;               /* dmf */
extern unsigned int Number_Of_Waves; /* dmf */
extern VECTOR *Wave_Sources;         /* dmf */



/*****************************************************************************
* Global functions
******************************************************************************/

void Compute_Colour (COLOUR Colour,PIGMENT *Pigment, DBL value);
void Initialize_Noise (void);
void Free_Noise_Tables (void);
DBL Noise (VECTOR EPoint);
void DNoise (VECTOR result, VECTOR EPoint);
DBL Turbulence (VECTOR EPoint, TURB *Turb);
void DTurbulence (VECTOR result, VECTOR EPoint, TURB *Turb);
DBL cycloidal (DBL value);
DBL Triangle_Wave (DBL value);
void Transform_Textures (TEXTURE *Textures, TRANSFORM *Trans);
void Destroy_Textures (TEXTURE *Textures);
void Post_Textures (TEXTURE *Textures);
FINISH *Create_Finish (void);
FINISH *Copy_Finish (FINISH *Old);
TEXTURE *Create_PNF_Texture (void);
TEXTURE *Copy_Texture_Pointer (TEXTURE *Texture);
TEXTURE *Copy_Textures (TEXTURE *Textures);
TEXTURE *Create_Texture (void);
int Test_Opacity (TEXTURE *Texture);
TURB *Create_Turb (void);
int POV_RAND (void);
void POV_SRAND (int seed);
int POV_GET_OLD_RAND(void);


#endif
