/****************************************************************************
*                   bsphere.h
*
*  This module contains all defines, typedefs, and prototypes for BSPHERE.C.
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


#ifndef BSPHERE_H
#define BSPHERE_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct BSphere_Tree_Struct BSPHERE_TREE;

struct BSphere_Tree_Struct
{
  short Entries;       /* Number of components (node if 0)    */
  VECTOR C;            /* Center of bounding sphere           */
  DBL r2;              /* Radius^2 of bounding sphere         */
  BSPHERE_TREE **Node; /* if node: children; if leaf: element */
};



/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

void Build_Bounding_Sphere_Hierarchy (BSPHERE_TREE **Root, int nElem, BSPHERE_TREE **Elements);
void Destroy_Bounding_Sphere_Hierarchy (BSPHERE_TREE *Node);




#endif
