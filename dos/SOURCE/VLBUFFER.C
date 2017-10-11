/****************************************************************************
*                   vlbuffer.c
*
*  This module implements functions that are used by the vista/light buffer.
*
*  This module was written by Dieter Bayer [DB].
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

/****************************************************************************
*
*  Explanation:
*
*    -
*
*  ---
*
*  Mar 1994 : Creation.
*
*****************************************************************************/

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "bbox.h"
#include "vlbuffer.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define INITIAL_NUMBER_OF_ENTRIES 256


/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

/* Tree node queue. */

PROJECT_QUEUE *Node_Queue;

/* Priority queue. */

PRIORITY_QUEUE *VLBuffer_Queue;



/*****************************************************************************
* Static functions
******************************************************************************/



/*****************************************************************************
*
* FUNCTION
*
*   Initialize_VLBuffer_Code
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
*   Init queues used by the light and vista buffer.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Initialize_VLBuffer_Code()
{
  Node_Queue = (PROJECT_QUEUE *)POV_MALLOC(sizeof(PROJECT_QUEUE),
    "vista/light buffer node queue");

  Node_Queue->QSize = 0;

  Node_Queue->Max_QSize = INITIAL_NUMBER_OF_ENTRIES;

  Node_Queue->Queue = (PROJECT_TREE_NODE **)POV_MALLOC(Node_Queue->Max_QSize*sizeof(PROJECT_TREE_NODE *),
    "vista/light buffer node queue");

  VLBuffer_Queue = Create_Priority_Queue(INITIAL_NUMBER_OF_ENTRIES);
}



/*****************************************************************************
*
* FUNCTION
*
*   Reinitialize_VLBuffer_Code
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
*   Reinit queues used by the light and vista buffer.
*
*   Note that only the node queue needs to be reinitialized.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

void Reinitialize_VLBuffer_Code()
{
  if (Node_Queue->QSize >= Node_Queue->Max_QSize)
  {
    if (Node_Queue->QSize >= INT_MAX/2)
    {
      Error("Node queue overflow.\n");
    }

    Node_Queue->Max_QSize *= 2;

    Node_Queue->Queue = (PROJECT_TREE_NODE **)POV_REALLOC(Node_Queue->Queue,
      Node_Queue->Max_QSize*sizeof(PROJECT_TREE_NODE *),
      "vista/light buffer node queue");
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Deinitialize_VLBuffer_Code
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
*   Deinit queues used by the light and vista buffer.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Deinitialize_VLBuffer_Code()
{
  if (Node_Queue != NULL)
{
    POV_FREE(Node_Queue->Queue);

    POV_FREE(Node_Queue);
  }

  if (VLBuffer_Queue != NULL)
  {
    Destroy_Priority_Queue(VLBuffer_Queue);
  }

  Node_Queue     = NULL;
  VLBuffer_Queue = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Clip_Polygon
*
* INPUT
*
*   Points             - polygon's points
*   PointCnt           - Number of points in polygon
*   VX1, VY1, VX2, VY1 - Normal vectors of the clipping planes
*   DX1, DY1, DX2, DY2 - Distances of the clipping planes from
*   the origin
*   
* OUTPUT
*
*   Points, PointCnt
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Clip polygon at the viewing pyramid define by the normal vectors
*   VX1, VX2, VY1, VY2 and the distances DX1, DX2, DY1, DY2.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Clip_Polygon(VECTOR *Points, int *PointCnt, VECTOR VX1, VECTOR  VX2, VECTOR  VY1, VECTOR  VY2, DBL DX1, DBL  DX2, DBL  DY1, DBL  DY2)
{
  DBL aktd, pred, fird, k;
  VECTOR aktP, intP, preP, firP, d;
  int i, pc;
  VECTOR ClipPoints[MAX_CLIP_POINTS];

  /********** clip polygon at "left" plane **********/

  pc = 0;

  Assign_Vector(firP, Points[0]);

  fird = VX1[X] * firP[X] + VX1[Y] * firP[Y] + VX1[Z] * firP[Z] - DX1;

  if (fird <= 0.0)
  {
    Assign_Vector(ClipPoints[pc++], firP);
  }

  Assign_Vector(aktP, firP);
  Assign_Vector(preP, firP);

  aktd = pred = fird;

  for (i = 1; i < *PointCnt; i++)
  {
    Assign_Vector(aktP, Points[i]);

    aktd = VX1[X] * aktP[X] + VX1[Y] * aktP[Y] + VX1[Z] * aktP[Z] - DX1;

    if (((aktd < 0.0) && (pred > 0.0)) || ((aktd > 0.0) && (pred < 0.0)))
    {
      d[X] = preP[X] - aktP[X];
      d[Y] = preP[Y] - aktP[Y];
      d[Z] = preP[Z] - aktP[Z];

      k = -aktd / (VX1[X] * d[X] + VX1[Y] * d[Y] + VX1[Z] * d[Z]);

      intP[X] = aktP[X] + k * d[X];
      intP[Y] = aktP[Y] + k * d[Y];
      intP[Z] = aktP[Z] + k * d[Z];

      Assign_Vector(ClipPoints[pc++], intP);
    }

    if (aktd <= 0.0)
    {
      Assign_Vector(ClipPoints[pc++], aktP);
    }

    Assign_Vector(preP, aktP);

    pred = aktd;
  }

  if (((fird < 0.0) && (aktd > 0.0)) || ((fird > 0.0) && (aktd < 0.0)))
  {
    d[X] = firP[X] - aktP[X];
    d[Y] = firP[Y] - aktP[Y];
    d[Z] = firP[Z] - aktP[Z];

    k = -aktd / (VX1[X] * d[X] + VX1[Y] * d[Y] + VX1[Z] * d[Z]);

    intP[X] = aktP[X] + k * d[X];
    intP[Y] = aktP[Y] + k * d[Y];
    intP[Z] = aktP[Z] + k * d[Z];

    Assign_Vector(ClipPoints[pc++], intP);
  }

  for (i = 0; i < pc; i++)
  {
    Assign_Vector(Points[i], ClipPoints[i]);
  }

  if ((*PointCnt = pc) == 0)
    return;

  /********** clip polygon at "right" plane **********/

  pc = 0;

  Assign_Vector(firP, Points[0]);

  fird = VX2[X] * firP[X] + VX2[Y] * firP[Y] + VX2[Z] * firP[Z] - DX2;

  if (fird <= 0.0)
  {
    Assign_Vector(ClipPoints[pc++], firP);
  }

  Assign_Vector(aktP, firP);
  Assign_Vector(preP, firP);

  aktd = pred = fird;

  for (i = 1; i < *PointCnt; i++)
  {
    Assign_Vector(aktP, Points[i]);

    aktd = VX2[X] * aktP[X] + VX2[Y] * aktP[Y] + VX2[Z] * aktP[Z] - DX2;

    if (((aktd < 0.0) && (pred > 0.0)) || ((aktd > 0.0) && (pred < 0.0)))
    {
      d[X] = preP[X] - aktP[X];
      d[Y] = preP[Y] - aktP[Y];
      d[Z] = preP[Z] - aktP[Z];

      k = -aktd / (VX2[X] * d[X] + VX2[Y] * d[Y] + VX2[Z] * d[Z]);

      intP[X] = aktP[X] + k * d[X];
      intP[Y] = aktP[Y] + k * d[Y];
      intP[Z] = aktP[Z] + k * d[Z];

      Assign_Vector(ClipPoints[pc++], intP);
    }

    if (aktd <= 0.0)
    {
      Assign_Vector(ClipPoints[pc++], aktP);
    }

    Assign_Vector(preP, aktP);

    pred = aktd;
  }

  if (((fird < 0.0) && (aktd > 0.0)) || ((fird > 0.0) && (aktd < 0.0)))
  {
    d[X] = firP[X] - aktP[X];
    d[Y] = firP[Y] - aktP[Y];
    d[Z] = firP[Z] - aktP[Z];

    k = -aktd / (VX2[X] * d[X] + VX2[Y] * d[Y] + VX2[Z] * d[Z]);

    intP[X] = aktP[X] + k * d[X];
    intP[Y] = aktP[Y] + k * d[Y];
    intP[Z] = aktP[Z] + k * d[Z];

    Assign_Vector(ClipPoints[pc++], intP);
  }

  for (i = 0; i < pc; i++)
  {
    Assign_Vector(Points[i], ClipPoints[i]);
  }

  if ((*PointCnt = pc) == 0)
    return;

  /********** clip polygon at "bottom" plane **********/

  pc = 0;

  Assign_Vector(firP, Points[0]);

  fird = VY1[X] * firP[X] + VY1[Y] * firP[Y] + VY1[Z] * firP[Z] - DY1;

  if (fird <= 0.0)
  {
    Assign_Vector(ClipPoints[pc++], firP);
  }

  Assign_Vector(aktP, firP);
  Assign_Vector(preP, firP);

  aktd = pred = fird;

  for (i = 1; i < *PointCnt; i++)
  {
    Assign_Vector(aktP, Points[i]);

    aktd = VY1[X] * aktP[X] + VY1[Y] * aktP[Y] + VY1[Z] * aktP[Z] - DY1;

    if (((aktd < 0.0) && (pred > 0.0)) || ((aktd > 0.0) && (pred < 0.0)))
    {
      d[X] = preP[X] - aktP[X];
      d[Y] = preP[Y] - aktP[Y];
      d[Z] = preP[Z] - aktP[Z];

      k = -aktd / (VY1[X] * d[X] + VY1[Y] * d[Y] + VY1[Z] * d[Z]);

      intP[X] = aktP[X] + k * d[X];
      intP[Y] = aktP[Y] + k * d[Y];
      intP[Z] = aktP[Z] + k * d[Z];

      Assign_Vector(ClipPoints[pc++], intP);
    }

    if (aktd <= 0.0)
    {
      Assign_Vector(ClipPoints[pc++], aktP);
    }

    Assign_Vector(preP, aktP);

    pred = aktd;
  }

  if (((fird < 0.0) && (aktd > 0.0)) || ((fird > 0.0) && (aktd < 0.0)))
  {
    d[X] = firP[X] - aktP[X];
    d[Y] = firP[Y] - aktP[Y];
    d[Z] = firP[Z] - aktP[Z];

    k = -aktd / (VY1[X] * d[X] + VY1[Y] * d[Y] + VY1[Z] * d[Z]);

    intP[X] = aktP[X] + k * d[X];
    intP[Y] = aktP[Y] + k * d[Y];
    intP[Z] = aktP[Z] + k * d[Z];

    Assign_Vector(ClipPoints[pc++], intP);
  }

  for (i = 0; i < pc; i++)
  {
    Assign_Vector(Points[i], ClipPoints[i]);
  }

  if ((*PointCnt = pc) == 0)
    return;

  /********** clip polygon at "top" plane **********/

  pc = 0;

  Assign_Vector(firP, Points[0]);

  fird = VY2[X] * firP[X] + VY2[Y] * firP[Y] + VY2[Z] * firP[Z] - DY2;

  if (fird <= 0.0)
  {
    Assign_Vector(ClipPoints[pc++], firP);
  }

  Assign_Vector(aktP, firP);
  Assign_Vector(preP, firP);

  aktd = pred = fird;

  for (i = pc = 0; i < *PointCnt; i++)
  {
    Assign_Vector(aktP, Points[i]);

    aktd = VY2[X] * aktP[X] + VY2[Y] * aktP[Y] + VY2[Z] * aktP[Z] - DY2;

    if (((aktd < 0.0) && (pred > 0.0)) || ((aktd > 0.0) && (pred < 0.0)))
    {
      d[X] = preP[X] - aktP[X];
      d[Y] = preP[Y] - aktP[Y];
      d[Z] = preP[Z] - aktP[Z];

      k = -aktd / (VY2[X] * d[X] + VY2[Y] * d[Y] + VY2[Z] * d[Z]);

      intP[X] = aktP[X] + k * d[X];
      intP[Y] = aktP[Y] + k * d[Y];
      intP[Z] = aktP[Z] + k * d[Z];

      Assign_Vector(ClipPoints[pc++], intP);
    }

    if (aktd <= 0.0)
    {
      Assign_Vector(ClipPoints[pc++], aktP);
    }

    Assign_Vector(preP, aktP);

    pred = aktd;
  }

  if (((fird < 0.0) && (aktd > 0.0)) || ((fird > 0.0) && (aktd < 0.0)))
  {
    d[X] = firP[X] - aktP[X];
    d[Y] = firP[Y] - aktP[Y];
    d[Z] = firP[Z] - aktP[Z];

    k = -aktd / (VY2[X] * d[X] + VY2[Y] * d[Y] + VY2[Z] * d[Z]);

    intP[X] = aktP[X] + k * d[X];
    intP[Y] = aktP[Y] + k * d[Y];
    intP[Z] = aktP[Z] + k * d[Z];

    Assign_Vector(ClipPoints[pc++], intP);
  }

  for (i = 0; i < pc; i++)
  {
    Assign_Vector(Points[i], ClipPoints[i]);
  }

  *PointCnt = pc;
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Project_Tree
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
*   Recursively destroy a node in a projection (i.e. vista/light) tree.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
*   Dec 1994 : Fixed memory leakage due to pruned branches. [DB]
*   Mar 1996 : Added COOPERATE for GUIs. [esp]
*
******************************************************************************/

void Destroy_Project_Tree(PROJECT_TREE_NODE *Node)
{
  unsigned short i;

  if (Node->is_leaf & TRUE)
  {
    COOPERATE_1
    POV_FREE(Node);
  }
  else
  {
    for (i = 0; i < Node->Entries; i++)
    {
      Destroy_Project_Tree(Node->Entry[i]);
    }

    POV_FREE(Node->Entry);

    POV_FREE(Node);
  }
}



