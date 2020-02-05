/****************************************************************************
*                   image.h
*
*  This module contains all defines, typedefs, and prototypes for IMAGE.C.
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


#ifndef IMAGE_H
#define IMAGE_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Image/Bump Map projection types. */

#define PLANAR_MAP      0
#define SPHERICAL_MAP   1
#define CYLINDRICAL_MAP 2
#define PARABOLIC_MAP   3
#define HYPERBOLIC_MAP  4
#define TORUS_MAP       5
#define PIRIFORM_MAP    6
#define OLD_MAP         7

/* Bit map interpolation types. */

#define NO_INTERPOLATION 0
#define NEAREST_NEIGHBOR 1
#define BILINEAR         2
#define CUBIC_SPLINE     3
#define NORMALIZED_DIST  4



/*****************************************************************************
* Global typedefs
******************************************************************************/




/*****************************************************************************
* Global variables
******************************************************************************/




/*****************************************************************************
* Global functions
******************************************************************************/

int image_map (VECTOR EPoint, PIGMENT *Pigment, COLOUR colour);
TEXTURE *material_map (VECTOR IPoint, TEXTURE *Texture);
void bump_map (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal);
IMAGE *Copy_Image (IMAGE *Old);
IMAGE *Create_Image (void);
void Destroy_Image (IMAGE *Image);



#endif
