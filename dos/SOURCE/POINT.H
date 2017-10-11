/****************************************************************************
*                   point.h
*
*  This module contains all defines, typedefs, and prototypes for POINT.C.
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


#ifndef POINT_H
#define POINT_H

#include "vlbuffer.h"



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define LIGHT_OBJECT (COMPOUND_OBJECT+PATCH_OBJECT+LIGHT_SOURCE_OBJECT)



/* Light source types. */

#define POINT_SOURCE       1
#define SPOT_SOURCE        2
#define FILL_LIGHT_SOURCE  3
#define CYLINDER_SOURCE    4



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Light_Source_Struct LIGHT_SOURCE;

struct Light_Source_Struct
{
  COMPOUND_FIELDS
  COLOUR Colour;
  VECTOR Direction, Center, Points_At, Axis1, Axis2;
  DBL Coeff, Radius, Falloff;
  DBL Fade_Distance, Fade_Power;
  LIGHT_SOURCE *Next_Light_Source;
  unsigned char Light_Type, Area_Light, Jitter, Track;
  int Area_Size1, Area_Size2;
  int Adaptive_Level;
  int Media_Attenuation;
  int Media_Interaction;
  COLOUR **Light_Grid;
  OBJECT *Shadow_Cached_Object;

  /* Light buffers for the six general directions in space. [DB 9/94] */

  PROJECT_TREE_NODE *Light_Buffer[6];
};




/*****************************************************************************
* Global variables
******************************************************************************/




/*****************************************************************************
* Global functions
******************************************************************************/

LIGHT_SOURCE *Create_Light_Source (void);
DBL Attenuate_Light (LIGHT_SOURCE *Light_Source, RAY *Light_Source_Ray, DBL Distance);
COLOUR **Create_Light_Grid (int Size1, int Size2);



#endif
