/****************************************************************************
*                   csg.h
*
*  This module contains all defines, typedefs, and prototypes for CSG.C.
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


#ifndef CSG_H
#define CSG_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define UNION_OBJECT        (COMPOUND_OBJECT)
#define MERGE_OBJECT        (COMPOUND_OBJECT)
#define INTERSECTION_OBJECT (COMPOUND_OBJECT)

/* CSG types */

#define CSG_UNION_TYPE             1
#define CSG_INTERSECTION_TYPE      2
#define CSG_DIFFERENCE_TYPE        4
#define CSG_MERGE_TYPE             8
#define CSG_SINGLE_TYPE           16



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct CSG_Struct CSG;

struct CSG_Struct
{
  COMPOUND_FIELDS
};



/*****************************************************************************
* Global variables
******************************************************************************/

extern METHODS CSG_Intersection_Methods;
extern METHODS CSG_Merge_Methods;
extern METHODS CSG_Union_Methods;



/*****************************************************************************
* Global functions
******************************************************************************/

CSG *Create_CSG_Union (void);
CSG *Create_CSG_Merge (void);
CSG *Create_CSG_Intersection (void);
void Compute_CSG_BBox (OBJECT *Object);



#endif
