/****************************************************************************
*                   vlbuffer.h
*
*  This module contains all defines, typedefs, and prototypes for VLBUFFER.C.
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


#ifndef VLBUFFER_H
#define VLBUFFER_H

#include "bbox.h"



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* flag to mark a node as pruned */

#define PRUNE_CHECK 128
#define PRUNE_TEMPORARY 128



/* Define minimum and maximum values for buffer coordinates. */

#define MIN_BUFFER_ENTRY -32000
#define MAX_BUFFER_ENTRY  32000



/* Define maximum number of clippoints. */

#define MAX_CLIP_POINTS 20



/* Define all six coordinate axes. */

#define XaxisP 0
#define XaxisM 1
#define YaxisP 2
#define YaxisM 3
#define ZaxisP 4
#define ZaxisM 5



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Project_Struct PROJECT;

typedef struct Project_Tree_Node_Struct PROJECT_TREE_NODE;
typedef struct Project_Tree_Leaf_Struct PROJECT_TREE_LEAF;
typedef struct Project_Queue_Struct PROJECT_QUEUE;

struct Project_Struct
{
  int x1, y1, x2, y2;
};

/*
 * The following structure represent the bounding box hierarchy in 2d space.
 * Because is_leaf, Object and Project are the first elements in both
 * structures they can be accessed without knowing at which structure
 * a pointer is pointing.
 */

struct Project_Tree_Node_Struct
{
  unsigned short is_leaf;
  BBOX_TREE *Node;
  PROJECT Project;
  unsigned short Entries;
  PROJECT_TREE_NODE **Entry;
};

struct Project_Tree_Leaf_Struct
{
  unsigned short is_leaf;
  BBOX_TREE *Node;
  PROJECT Project;
};

struct Project_Queue_Struct
{
  unsigned QSize;
  unsigned Max_QSize;
  PROJECT_TREE_NODE **Queue;
};




/*****************************************************************************
* Global variables
******************************************************************************/

extern PROJECT_QUEUE *Node_Queue;
extern PRIORITY_QUEUE *VLBuffer_Queue;



/*****************************************************************************
* Global functions
******************************************************************************/

void Clip_Polygon (VECTOR *Points, int *PointCnt, VECTOR VX1, VECTOR VX2, VECTOR VY1, VECTOR VY2, DBL DX1, DBL DX2, DBL DY1, DBL DY2);

void Initialize_VLBuffer_Code (void);
void Reinitialize_VLBuffer_Code (void);
void Deinitialize_VLBuffer_Code (void);

void Destroy_Project_Tree (PROJECT_TREE_NODE *Node);



#endif
