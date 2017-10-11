/****************************************************************************
*                   camera.h
*
*  This module contains all defines, typedefs, and prototypes for CAMERA.C.
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


#ifndef CAMERA_H
#define CAMERA_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/




/* Available camera types. [DB 8/94] */

#define PERSPECTIVE_CAMERA      1
#define ORTHOGRAPHIC_CAMERA     2
#define FISHEYE_CAMERA          3
#define ULTRA_WIDE_ANGLE_CAMERA 4
#define OMNIMAX_CAMERA          5
#define PANORAMIC_CAMERA        6
#define CYL_1_CAMERA            7
#define CYL_2_CAMERA            8
#define CYL_3_CAMERA            9
#define CYL_4_CAMERA           10



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Camera_Struct CAMERA;

struct Camera_Struct
{
  VECTOR Location;
  VECTOR Direction;
  VECTOR Up;
  VECTOR Right;
  VECTOR Sky;
  VECTOR Look_At;                /* Used only to record the user's preference */
  DBL Focal_Distance, Aperture;  /* ARE 9/92 for focal blur.           */
  int Blur_Samples;              /* ARE 9/92 for focal blur.           */
  DBL Confidence;                /* Probability for confidence test.   */
  DBL Variance;                  /* Max. variance for confidence test. */
  int Type;                      /* Camera type.                       */
  DBL Angle;                     /* Viewing angle.                     */
  TNORMAL *Tnormal;              /* Primary ray pertubation.           */
};



/*****************************************************************************
* Global variables
******************************************************************************/




/*****************************************************************************
* Global functions
******************************************************************************/

void Translate_Camera (CAMERA *Cm, VECTOR Vector);
void Rotate_Camera (CAMERA *Cm, VECTOR Vector);
void Scale_Camera (CAMERA *Cm, VECTOR Vector);
void Transform_Camera (CAMERA *Cm, TRANSFORM *Trans);
CAMERA *Copy_Camera (CAMERA *Old);
CAMERA *Create_Camera (void);
void Destroy_Camera (CAMERA *Cm);


#endif
