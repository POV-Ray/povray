/****************************************************************************
*                   bezier.h
*
*  This module contains all defines, typedefs, and prototypes for BEZIER.C.
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


#ifndef BEZIER_H
#define BEZIER_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define BICUBIC_PATCH_OBJECT (PATCH_OBJECT+DOUBLE_ILLUMINATE)

#define BEZIER_INTERIOR_NODE 0
#define BEZIER_LEAF_NODE 1

#define MAX_PATCH_TYPE 4




/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Bicubic_Patch_Struct BICUBIC_PATCH;
typedef struct Bezier_Node_Struct BEZIER_NODE;
typedef struct Bezier_Child_Struct BEZIER_CHILDREN;
typedef struct Bezier_Vertices_Struct BEZIER_VERTICES;

struct Bezier_Child_Struct
{
  BEZIER_NODE *Children[4];
};

struct Bezier_Vertices_Struct
{
  float uvbnds[4];
  VECTOR Vertices[4];
};

struct Bezier_Node_Struct
{
  int Node_Type;      /* Is this an interior node, or a leaf */
  VECTOR Center;      /* Center of sphere bounding the (sub)patch */
  DBL Radius_Squared; /* Radius of bounding sphere (squared) */
  int Count;          /* # of subpatches associated with this node */
  void *Data_Ptr;     /* Either pointer to vertices or pointer to children */
};

struct Bicubic_Patch_Struct
{
  OBJECT_FIELDS
  int Patch_Type, U_Steps, V_Steps;
  VECTOR Control_Points[4][4];
  VECTOR Bounding_Sphere_Center;
  DBL Bounding_Sphere_Radius;
  DBL Flatness_Value;
  BEZIER_NODE *Node_Tree;
};



/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

void Precompute_Patch_Values (BICUBIC_PATCH *Shape);
BICUBIC_PATCH *Create_Bicubic_Patch (void);
void Compute_Bicubic_Patch_BBox (BICUBIC_PATCH *Patch);



#endif
