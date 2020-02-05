/****************************************************************************
*                camera.c
*
*  This module implements methods for managing the viewpoint.
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

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "camera.h"
#include "matrices.h"
#include "normal.h"



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Camera
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Translate_Camera(CAMERA *Camera, VECTOR Vector)
{
  VAddEq(((CAMERA *)Camera)->Location, Vector);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Camera
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Rotate_Camera(CAMERA *Camera, VECTOR Vector)
{
  TRANSFORM Trans;
  
  Compute_Rotation_Transform(&Trans, Vector);
  
  Transform_Camera(Camera, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Camera
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Scale_Camera(CAMERA *Camera, VECTOR Vector)
{
  TRANSFORM Trans;
  
  Compute_Scaling_Transform(&Trans, Vector);
  
  Transform_Camera(Camera, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Camera
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Transform_Camera(CAMERA *Camera, TRANSFORM *Trans)
{
  MTransPoint(Camera->Location, Camera->Location, Trans);
  
  MTransPoint(Camera->Direction, Camera->Direction, Trans);
  
  MTransPoint(Camera->Up, Camera->Up, Trans);
  
  MTransPoint(Camera->Right, Camera->Right, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Camera
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

CAMERA *Create_Camera()
{
  CAMERA *New;
  
  New = (CAMERA *)POV_MALLOC(sizeof (CAMERA), "camera");
  
  Make_Vector(New->Location,  0.0,  0.0, 0.0);
  Make_Vector(New->Direction, 0.0,  0.0, 1.0);
  Make_Vector(New->Up,        0.0,  1.0, 0.0);
  Make_Vector(New->Right,     1.33, 0.0, 0.0);
  Make_Vector(New->Sky,       0.0,  1.0, 0.0);
  Make_Vector(New->Look_At,   0.0,  0.0, 1.0);

  /* Init focal blur stuff (not used by default). */

  New->Blur_Samples   = 0;
  New->Confidence     = 0.9;
  New->Variance       = 1.0 / 128.0;
  New->Aperture       = 0.0;
  New->Focal_Distance = -1.0;

  /* Set default camera type and viewing angle. [DB 7/94] */

  New->Type = PERSPECTIVE_CAMERA;

  New->Angle = 90.0;

  /* Do not perturb primary rays by default. [DB 7/94] */

  New->Tnormal = NULL;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Camera
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

CAMERA *Copy_Camera(CAMERA *Old)
{
  CAMERA *New;

  if (Old != NULL)
  {
    New = Create_Camera();

    *New = *Old;
  }
  else
  {
    New = NULL;
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Camera
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Destroy_Camera(CAMERA *Camera)
{
  if (Camera != NULL)
  {
    Destroy_Tnormal(Camera->Tnormal);

    POV_FREE(Camera);
  }
}
