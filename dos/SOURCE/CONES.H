/****************************************************************************
*                   cones.h
*
*  This module contains all defines, typedefs, and prototypes for CONES.C.
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


#ifndef CONES_H
#define CONES_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define CONE_OBJECT (BASIC_OBJECT)



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Cone_Struct CYLINDER;
typedef struct Cone_Struct CONE;

struct Cone_Struct
{
  OBJECT_FIELDS
  TRANSFORM *Trans;   /* Transformation of a Cone object */
  VECTOR apex;        /* Center of the top of the cone */
  VECTOR base;        /* Center of the bottom of the cone */
  DBL apex_radius;    /* Radius of the cone at the top */
  DBL base_radius;    /* Radius of the cone at the bottom */
  DBL dist;           /* Distance to end of cone in canonical coords */
};



/*****************************************************************************
* Global variables
******************************************************************************/




/*****************************************************************************
* Global functions
******************************************************************************/

CONE *Create_Cone (void);
CONE *Create_Cylinder (void);
void Compute_Cone_Data (OBJECT *Object);
void Compute_Cylinder_Data (OBJECT *Object);
void Compute_Cone_BBox (CONE *Cone);



#endif
