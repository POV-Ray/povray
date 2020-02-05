/****************************************************************************
*                   vbuffer.c
*
*  This module implements functions that implement the vista buffer.
*
*  This module was written by Dieter Bayer [DB].
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
#include "boxes.h"
#include "hfield.h"
#include "lighting.h"
#include "matrices.h"
#include "objects.h"
#include "povray.h"
#include "render.h"
#include "triangle.h"
#include "vbuffer.h"
#include "vlbuffer.h"
#include "userio.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

static DBL Distance;
static MATRIX WC2VC, WC2VCinv;
static VECTOR gO, gU, gV, gW;


/* Planes for 3d-clipping. */

static VECTOR VIEW_VX1 = {-0.8944271910, 0.0, -0.4472135955};
static VECTOR VIEW_VX2 = { 0.8944271910, 0.0, -0.4472135955};
static VECTOR VIEW_VY1 = {0.0, -0.8944271910, -0.4472135955};
static VECTOR VIEW_VY2 = {0.0,  0.8944271910, -0.4472135955};
static DBL VIEW_DX1 = 0.4472135955;
static DBL VIEW_DX2 = 0.4472135955;
static DBL VIEW_DY1 = 0.4472135955;
static DBL VIEW_DY2 = 0.4472135955;

static PROJECT_TREE_NODE *Root_Vista;



/*****************************************************************************
* Static functions
******************************************************************************/

static void init_view_coordinates (void);

static void project_raw_rectangle (PROJECT *Project, VECTOR P1, VECTOR P2, VECTOR P3, VECTOR P4, int *visible);
static void project_raw_triangle (PROJECT *Project, VECTOR P1, VECTOR P2, VECTOR P3, int *visible);

static void project_bbox (PROJECT *Project, VECTOR *P, int *visible);
static void project_bounds (PROJECT *Project, BBOX *BBox, int *visible);

static void get_perspective_projection (OBJECT *Object, PROJECT *Project, int infinite);
static void get_orthographic_projection (OBJECT *Object, PROJECT *Project, int infinite);

static void project_object (OBJECT *Object, PROJECT *Project);

static void project_box (PROJECT *Project, OBJECT *Object, int *visible);
static void project_hfield (PROJECT *Project, OBJECT *Object, int *visible);
static void project_triangle (PROJECT *Project, OBJECT *Object, int *visible);
static void project_smooth_triangle (PROJECT *Project, OBJECT *Object, int *visible);

static void transform_point (VECTOR P);

static void project_bounding_slab (PROJECT *Project, PROJECT_TREE_NODE **Tree, BBOX_TREE *Node);

static int intersect_vista_tree (RAY *Ray, PROJECT_TREE_NODE *Tree, int x, INTERSECTION *Best_Intersection);

static void draw_projection (PROJECT *Project, int color, int *BigRed, int *BigBlue);
static void draw_vista (PROJECT_TREE_NODE *Tree, int *BigRed, int *BigBlue);

/*****************************************************************************
*
* FUNCTION
*
*   Prune_Vista_Tree
*
* INPUT
*
*   y - Current scanline number
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
*   Prune vista tree, i.e. mark all nodes not on the current line inactive.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Prune_Vista_Tree(int y)
{
  unsigned short i;
  PROJECT_TREE_NODE *Node, *Sib;

  /* If there's no vista tree then return. */

  if (Root_Vista == NULL)
  {
    return;
  }

  Node_Queue->QSize = 0;

  Increase_Counter(stats[VBuffer_Tests]);

  if ((y < Root_Vista->Project.y1) || (y > Root_Vista->Project.y2))
  {
    /* Root doesn't lie on current line --> prune root */

    Root_Vista->is_leaf |= PRUNE_TEMPORARY;
  }
  else
  {
    /* Root lies on current line --> unprune root */

    Increase_Counter(stats[VBuffer_Tests_Succeeded]);

    Root_Vista->is_leaf &= ~PRUNE_TEMPORARY;

    Node_Queue->Queue[(Node_Queue->QSize)++] = Root_Vista;
  }

  while (Node_Queue->QSize > 0)
  {
    Node = Node_Queue->Queue[--(Node_Queue->QSize)];

    if (Node->is_leaf & TRUE)
    {
      Increase_Counter(stats[VBuffer_Tests]);

      if ((y < Node->Project.y1) || (y > Node->Project.y2))
      {
        /* Leaf doesn't lie on current line --> prune leaf */

        Node->is_leaf |= PRUNE_TEMPORARY;
      }
      else
      {
        /* Leaf lies on current line --> unprune leaf */

        Increase_Counter(stats[VBuffer_Tests_Succeeded]);

        Node->is_leaf &= ~PRUNE_TEMPORARY;
      }
    }
    else
    {
      /* Check siblings of the node */

      for (i = 0; i < Node->Entries; i++)
      {
        Sib = Node->Entry[i];

        Increase_Counter(stats[VBuffer_Tests]);

        if ((y < Sib->Project.y1) || (y > Sib->Project.y2))
        {
          /* Sibling doesn't lie on current line --> prune sibling */

          Sib->is_leaf |= PRUNE_TEMPORARY;
        }
        else
        {
          /* Sibling lies on current line --> unprune sibling */

          Increase_Counter(stats[VBuffer_Tests_Succeeded]);

          Sib->is_leaf &= ~PRUNE_TEMPORARY;

          /* Add sibling to list */

          /* Reallocate queue if it's too small. */

          Reinitialize_VLBuffer_Code();

          Node_Queue->Queue[(Node_Queue->QSize)++] = Sib;
        }
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Trace_Primary_Ray
*
* INPUT
*
*   Ray    - Current ray
*   Colour - Ray's colour
*   x      - Current x-coordinate
*   
* OUTPUT
*
*   colour
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Trace a primary ray using the vista tree.
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Nov 1994 : Rearranged calls to Fog, Ranibow and Skyblend.
*              Added call to Atmosphere for atmospheric effects. [DB]
*
*   Jan 1995 : Set intersection depth to Max_Distance for infinte rays. [DB]
*   Jul 1995 : Added code to support alpha channel. [DB]
*
******************************************************************************/

void Trace_Primary_Ray (RAY *Ray, COLOUR Colour, DBL Weight, int x)
{
  int i, Intersection_Found, all_hollow;
  INTERSECTION Best_Intersection;

  COOPERATE_0
  Increase_Counter(stats[Number_Of_Rays]);

  /* Transmittance has to be 1 to make alpha channel output to work. [DB] */

  Make_ColourA(Colour, 0.0, 0.0, 0.0, 0.0, 1.0);

  if ((Trace_Level > Max_Trace_Level) || (Weight < ADC_Bailout))
  {
    if (Weight < ADC_Bailout)
    {
      Increase_Counter(stats[ADC_Saves]);
    }

    return;
  }

  if (Trace_Level > Highest_Trace_Level)
  {
    Highest_Trace_Level = Trace_Level;
  }

  Best_Intersection.Depth = BOUND_HUGE;

  /* What objects does this ray intersect? */

  Intersection_Found = intersect_vista_tree(Ray, Root_Vista, x, &Best_Intersection);

  if (Intersection_Found)
  {
    Determine_Apparent_Colour(&Best_Intersection, Colour, Ray, 1.0);
  }
  else
  {
      /* Infinite ray, set intersection distance. */

      Best_Intersection.Depth = Max_Distance;

      Do_Infinite_Atmosphere(Ray, Colour);
  }

  /* Test if all contained objects are hollow. */

  all_hollow = TRUE;

  if (Ray->Index > -1)
  {
    for (i = 0; i <= Ray->Index; i++)
    {
      if (!Ray->Interiors[i]->hollow)
      {
        all_hollow = FALSE;

        break;
      }
    }
  }

  /* Apply finite atmospheric effects. */
  if (all_hollow && (opts.Quality_Flags & Q_VOLUME))
  {
    Do_Finite_Atmosphere(Ray, &Best_Intersection, Colour, FALSE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_vista_tree
*
* INPUT
*
*   Ray               - Primary ray
*   Tree              - Vista tree's top-node
*   x                 - Current x-coordinate
*   Best_Intersection - Intersection found
*   
* OUTPUT
*
*   Best_Intersection
*
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Intersect a PRIMARY ray with the vista tree
*   (tree pruning is used can be primary ray!!!).
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int intersect_vista_tree(RAY *Ray, PROJECT_TREE_NODE *Tree, int x, INTERSECTION *Best_Intersection)
{
  INTERSECTION New_Intersection;
  unsigned short i;
  unsigned size;
  int Found;
  RAYINFO rayinfo;
  DBL key;
  BBOX_TREE *BBox_Node;
  PROJECT_TREE_NODE *Node;

  /* If there's no vista tree then return. */

  if (Tree == NULL)
  {
    return(FALSE);
  }

  /* Start with an empty priority queue */

  VLBuffer_Queue->QSize = 0;

  Found = FALSE;

#ifdef BBOX_EXTRA_STATS
  Increase_Counter(stats[totalQueueResets]);
#endif

  /* Descend tree. */

  Node_Queue->QSize = 0;

  /* Create the direction vectors for this ray */

  Create_Rayinfo(Ray, &rayinfo);

  /* Fill the priority queue with all possible candidates */

  /* Check root */

  Increase_Counter(stats[VBuffer_Tests]);

  if ((x >= Tree->Project.x1) && (x <= Tree->Project.x2))
  {
    Increase_Counter(stats[VBuffer_Tests_Succeeded]);

    Node_Queue->Queue[(Node_Queue->QSize)++] = Tree;
  }

  while (Node_Queue->QSize > 0)
  {
    Tree = Node_Queue->Queue[--(Node_Queue->QSize)];

    switch (Tree->is_leaf)
    {
      case FALSE:

        /* Check siblings of the unpruned node in 2d */

        for (i = 0; i < Tree->Entries; i++)
        {
          Node = Tree->Entry[i];

          /* Check unpruned siblings only */

          if (Node->is_leaf < PRUNE_CHECK)
          {
            Increase_Counter(stats[VBuffer_Tests]);

            if ((x >= Node->Project.x1) && (x <= Node->Project.x2))
            {
              /* Add node to node queue. */

              Increase_Counter(stats[VBuffer_Tests_Succeeded]);

              /* Reallocate queue if it's too small. */

              Reinitialize_VLBuffer_Code();

              Node_Queue->Queue[(Node_Queue->QSize)++] = Node;
            }
          }
        }

    break;

      case TRUE:

        /* Unpruned leaf --> test object's bounding box in 3d */

        Check_And_Enqueue(VLBuffer_Queue,
          ((PROJECT_TREE_LEAF *)Tree)->Node,
          &(((PROJECT_TREE_LEAF *)Tree)->Node->BBox),
          &rayinfo);

        break;

   /* default:

        The node/leaf is pruned and needn't be checked */

    }
  }

  /* Now test the candidates in the priority queue */

  while (VLBuffer_Queue->QSize > 0)
  {
    Priority_Queue_Delete(VLBuffer_Queue, &key, &BBox_Node);

    if (key > Best_Intersection->Depth)
      break;

    if (Intersection(&New_Intersection, (OBJECT *)BBox_Node->Node, Ray))
    {
      if (New_Intersection.Depth < Best_Intersection->Depth)
      {
        *Best_Intersection = New_Intersection;
        Found = TRUE;
      }
    }
  }

  return(Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   project_raw_triangle
*
* INPUT
*
*   Project    - Triangle's projection
*   P1, P2, P3 - Triangle's edges
*   visible    - Flag if triangle is visible
*   
* OUTPUT
*
*   Project, visible
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Project a triangle onto the screen.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_raw_triangle (PROJECT *Project, VECTOR P1, VECTOR  P2, VECTOR  P3, int *visible)
{
  VECTOR Points[MAX_CLIP_POINTS];
  int i, number;
  int x, y;

  Assign_Vector(Points[0], P1);
  Assign_Vector(Points[1], P2);
  Assign_Vector(Points[2], P3);

  number = 3;

  /* Clip triangle only if some quick tests say it's necessary.
     Assuming that only a few triangles need clipping this saves some time.
     (I don't need to write fabs(1+P?[Z]) since the tests succeed anyway if
      P?[Z] < -1. Hope the compiler doesn't change the tests' order!) */

  if ((P1[Z] < -1.0) || (P2[Z] < -1.0) || (P3[Z] < -1.0) ||
      (fabs(P1[X]) > 0.5*(1.0+P1[Z])) || (fabs(P1[Y]) > 0.5*(1.0+P1[Z])) ||
      (fabs(P2[X]) > 0.5*(1.0+P2[Z])) || (fabs(P2[Y]) > 0.5*(1.0+P2[Z])) ||
      (fabs(P3[X]) > 0.5*(1.0+P3[Z])) || (fabs(P3[Y]) > 0.5*(1.0+P3[Z])))
  {
    Clip_Polygon(Points, &number, VIEW_VX1, VIEW_VX2, VIEW_VY1, VIEW_VY2,
                                  VIEW_DX1, VIEW_DX2, VIEW_DY1, VIEW_DY2);
  }

  if (number)
  {
    for (i = 0; i < number; i++)
    {
      if (Points[i][Z] < -1.0 + EPSILON)
      {
        Points[i][X] = Points[i][Y] = 0.0;
      }
      else
      {
        Points[i][X] /= 1.0 + Points[i][Z];
        Points[i][Y] /= 1.0 + Points[i][Z];
      }

      x = Frame.Screen_Width/2  + (int)(Frame.Screen_Width  * Points[i][X]);
      y = Frame.Screen_Height/2 - (int)(Frame.Screen_Height * Points[i][Y]);

      if (x < Project->x1) Project->x1 = x;
      if (x > Project->x2) Project->x2 = x;
      if (y < Project->y1) Project->y1 = y;
      if (y > Project->y2) Project->y2 = y;
    }

    *visible = TRUE;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   project_raw_rectangle
*
* INPUT
*
*   Project        - Rectangle's projection
*   P1, P2, P3, P4 - Rectangle's edges
*   visible        - Flag if rectangle is visible
*   
* OUTPUT
*
*   Project, visible
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Project a rectangle onto the screen.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_raw_rectangle(PROJECT *Project, VECTOR P1, VECTOR  P2, VECTOR  P3, VECTOR  P4, int *visible)
{
  VECTOR Points[MAX_CLIP_POINTS];
  int i, number;
  int x, y;

  Assign_Vector(Points[0], P1);
  Assign_Vector(Points[1], P2);
  Assign_Vector(Points[2], P3);
  Assign_Vector(Points[3], P4);

  number = 4;

  Clip_Polygon(Points, &number, VIEW_VX1, VIEW_VX2, VIEW_VY1, VIEW_VY2,
                                VIEW_DX1, VIEW_DX2, VIEW_DY1, VIEW_DY2);

  if (number)
  {
    for (i = 0; i < number; i++)
    {
      if (Points[i][Z] < -1.0 + EPSILON)
      {
        Points[i][X] = Points[i][Y] = 0.0;
      }
      else
      {
        Points[i][X] /= 1.0 + Points[i][Z];
        Points[i][Y] /= 1.0 + Points[i][Z];
      }

      x = Frame.Screen_Width/2  + (int)(Frame.Screen_Width  * Points[i][X]);
      y = Frame.Screen_Height/2 - (int)(Frame.Screen_Height * Points[i][Y]);

      if (x < Project->x1) Project->x1 = x;
      if (x > Project->x2) Project->x2 = x;
      if (y < Project->y1) Project->y1 = y;
      if (y > Project->y2) Project->y2 = y;
    }

    *visible = TRUE;
  }
}




/*****************************************************************************
*
* FUNCTION
*
*   project_bbox
*
* INPUT
*
*   Project - Box's projection
*   P       - Box's edges
*   visible - Flag if box is visible
*   
* OUTPUT
*
*   Project, visible
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Project a box onto the screen.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_bbox(PROJECT *Project, VECTOR *P, int *visible)
{
  int vis, i, x, y;
  PROJECT New;

  New.x1 = MAX_BUFFER_ENTRY;
  New.x2 = MIN_BUFFER_ENTRY;
  New.y1 = MAX_BUFFER_ENTRY;
  New.y2 = MIN_BUFFER_ENTRY;

  vis = FALSE;

  /* Check if all points lie "in front" of the viewer. */

  if ((P[0][Z] > -1.0) && (P[1][Z] > -1.0) && (P[2][Z] > -1.0) && (P[3][Z] > -1.0) &&
      (P[4][Z] > -1.0) && (P[5][Z] > -1.0) && (P[6][Z] > -1.0) && (P[7][Z] > -1.0))
  {
    /* Check if all points lie inside the "viewing pyramid". */

    if ((fabs(P[0][X]) <= 0.5*(1.0+P[0][Z])) && (fabs(P[1][X]) <= 0.5*(1.0+P[1][Z])) &&
        (fabs(P[2][X]) <= 0.5*(1.0+P[2][Z])) && (fabs(P[3][X]) <= 0.5*(1.0+P[3][Z])) &&
        (fabs(P[4][X]) <= 0.5*(1.0+P[4][Z])) && (fabs(P[5][X]) <= 0.5*(1.0+P[5][Z])) &&
        (fabs(P[6][X]) <= 0.5*(1.0+P[6][Z])) && (fabs(P[7][X]) <= 0.5*(1.0+P[7][Z])) &&
        (fabs(P[0][Y]) <= 0.5*(1.0+P[0][Z])) && (fabs(P[1][Y]) <= 0.5*(1.0+P[1][Z])) &&
        (fabs(P[2][Y]) <= 0.5*(1.0+P[2][Z])) && (fabs(P[3][Y]) <= 0.5*(1.0+P[3][Z])) &&
        (fabs(P[4][Y]) <= 0.5*(1.0+P[4][Z])) && (fabs(P[5][Y]) <= 0.5*(1.0+P[5][Z])) &&
        (fabs(P[6][Y]) <= 0.5*(1.0+P[6][Z])) && (fabs(P[7][Y]) <= 0.5*(1.0+P[7][Z])))
    {
      /* No clipping is needed. Just project the points. */

      vis = TRUE;

      for (i = 0; i < 8; i++)
      {
        if (P[i][Z] < -1.0 + EPSILON)
        {
          P[i][X] = P[i][Y] = 0.0;
        }
        else
        {
          P[i][X] /= 1.0 + P[i][Z];
          P[i][Y] /= 1.0 + P[i][Z];
        }

        x = Frame.Screen_Width/2  + (int)(Frame.Screen_Width  * P[i][X]);
        y = Frame.Screen_Height/2 - (int)(Frame.Screen_Height * P[i][Y]);

        if (x < New.x1) New.x1 = x;
        if (x > New.x2) New.x2 = x;
        if (y < New.y1) New.y1 = y;
        if (y > New.y2) New.y2 = y;
      }
    }
  }

  if (!vis)
  {
    project_raw_rectangle(&New, P[0], P[1], P[3], P[2], &vis);
    project_raw_rectangle(&New, P[4], P[5], P[7], P[6], &vis);
    project_raw_rectangle(&New, P[0], P[1], P[5], P[4], &vis);
    project_raw_rectangle(&New, P[2], P[3], P[7], P[6], &vis);
    project_raw_rectangle(&New, P[1], P[3], P[7], P[5], &vis);
    project_raw_rectangle(&New, P[0], P[2], P[6], P[4], &vis);
  }

  if (vis)
  {
    if (New.x1 > Project->x1) Project->x1 = New.x1;
    if (New.x2 < Project->x2) Project->x2 = New.x2;
    if (New.y1 > Project->y1) Project->y1 = New.y1;
    if (New.y2 < Project->y2) Project->y2 = New.y2;
    *visible = TRUE;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   project_bounds
*
* INPUT
*
*   Project - Bounding box's projection
*   BBox    - Bounding box
*   visible - Flag if bounding box is visible
*   
* OUTPUT
*
*   Project, visible
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Project a bounding box onto the screen.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_bounds(PROJECT *Project, BBOX *BBox, int *visible)
{
  int i;
  VECTOR P[8];

  for (i = 0; i<8; i++)
  {
    P[i][X] = ((i & 1) ? BBox->Lengths[X] : 0.0) + BBox->Lower_Left[X];
    P[i][Y] = ((i & 2) ? BBox->Lengths[Y] : 0.0) + BBox->Lower_Left[Y];
    P[i][Z] = ((i & 4) ? BBox->Lengths[Z] : 0.0) + BBox->Lower_Left[Z];

    transform_point(P[i]);
  }

  project_bbox(Project, P, visible);
}



/*****************************************************************************
*
* FUNCTION
*
*   project_box
*
* INPUT
*
*   Project - Projection
*   Object  - Object
*   visible - Flag if object is visible
*   
* OUTPUT
*
*   Project, visible
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Project a box onto the screen.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_box(PROJECT *Project, OBJECT *Object, int *visible)
{
  int i;
  VECTOR P[8];
  BOX *box;

  box = (BOX *)Object;

  for (i = 0; i<8; i++)
  {
    P[i][X] = (i & 1) ? box->bounds[1][X] : box->bounds[0][X];
    P[i][Y] = (i & 2) ? box->bounds[1][Y] : box->bounds[0][Y];
    P[i][Z] = (i & 4) ? box->bounds[1][Z] : box->bounds[0][Z];

    if (box->Trans != NULL)
    {
      MTransPoint(P[i], P[i], box->Trans);
    }

    transform_point(P[i]);
  }

  project_bbox(Project, P, visible);
}



/*****************************************************************************
*
* FUNCTION
*
*   project_hfield
*
* INPUT
*
*   Project - Projection
*   Object  - Object
*   visible - Flag if object is visible
*   
* OUTPUT
*
*   Project, visible
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Project the bounding box of a height field onto the screen.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_hfield(PROJECT *Project, OBJECT *Object, int *visible)
{
  int i;
  VECTOR P[8];
  HFIELD *hfield;

  hfield = (HFIELD *)Object;

  for (i = 0; i<8; i++)
  {
    Assign_Vector(P[i], hfield->bounding_box->bounds[0]);

    P[i][X] = (i & 1) ? hfield->bounding_box->bounds[1][X] : hfield->bounding_box->bounds[0][X];
    P[i][Y] = (i & 2) ? hfield->bounding_box->bounds[1][Y] : hfield->bounding_box->bounds[0][Y];
    P[i][Z] = (i & 4) ? hfield->bounding_box->bounds[1][Z] : hfield->bounding_box->bounds[0][Z];

    if (hfield->Trans != NULL)
    {
      MTransPoint(P[i], P[i], hfield->Trans);
    }

    transform_point(P[i]);
  }

  project_bbox(Project, P, visible);
}



/*****************************************************************************
*
* FUNCTION
*
*   project_triangle
*
* INPUT
*
*   Project - Projection
*   Object  - Object
*   visible - Flag if object is visible
*   
* OUTPUT
*
*   Project, visible
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Project a triangle onto the screen.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_triangle(PROJECT *Project, OBJECT *Object, int *visible)
{
  int i, vis;
  VECTOR P[3];
  PROJECT New;

  New.x1 = MAX_BUFFER_ENTRY;
  New.x2 = MIN_BUFFER_ENTRY;
  New.y1 = MAX_BUFFER_ENTRY;
  New.y2 = MIN_BUFFER_ENTRY;

  Assign_Vector(P[0], ((TRIANGLE *)Object)->P1);
  Assign_Vector(P[1], ((TRIANGLE *)Object)->P2);
  Assign_Vector(P[2], ((TRIANGLE *)Object)->P3);

  for (i = 0; i < 3; i++)
  {
    transform_point(P[i]);
  }

  vis = FALSE;

  project_raw_triangle(&New, P[0], P[1], P[2], &vis);

  if (vis)
  {
    if (New.x1 > Project->x1) Project->x1 = New.x1;
    if (New.x2 < Project->x2) Project->x2 = New.x2;
    if (New.y1 > Project->y1) Project->y1 = New.y1;
    if (New.y2 < Project->y2) Project->y2 = New.y2;

    *visible = TRUE;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   project_smooth_triangle
*
* INPUT
*
*   Project - Projection
*   Object  - Object
*   visible - Flag if object is visible
*   
* OUTPUT
*
*   Project, visible
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Project a smooth triangle onto the screen.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_smooth_triangle(PROJECT *Project, OBJECT *Object, int *visible)
{
  int i, vis;
  VECTOR P[3];
  PROJECT New;

  New.x1 = MAX_BUFFER_ENTRY;
  New.x2 = MIN_BUFFER_ENTRY;
  New.y1 = MAX_BUFFER_ENTRY;
  New.y2 = MIN_BUFFER_ENTRY;

  Assign_Vector(P[0], ((SMOOTH_TRIANGLE *)Object)->P1);
  Assign_Vector(P[1], ((SMOOTH_TRIANGLE *)Object)->P2);
  Assign_Vector(P[2], ((SMOOTH_TRIANGLE *)Object)->P3);

  for (i = 0; i < 3; i++)
  {
    transform_point(P[i]);
  }

  vis = FALSE;

  project_raw_triangle(&New, P[0], P[1], P[2], &vis);

  if (vis)
  {
    if (New.x1 > Project->x1) Project->x1 = New.x1;
    if (New.x2 < Project->x2) Project->x2 = New.x2;
    if (New.y1 > Project->y1) Project->y1 = New.y1;
    if (New.y2 < Project->y2) Project->y2 = New.y2;

    *visible = TRUE;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   transform_point
*
* INPUT
*
*   P - Point to transform
*   
* OUTPUT
*
*   P
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Transform a point from the world coordinate system to the viewer's
*   coordinate system.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void transform_point(VECTOR P)
{
  DBL x,y,z;

  x = P[X] - gO[X];
  y = P[Y] - gO[Y];
  z = P[Z] - gO[Z];

  P[X] = gU[X] * x + gU[Y] * y + gU[Z] * z;
  P[Y] = gV[X] * x + gV[Y] * y + gV[Z] * z;
  P[Z] = gW[X] * x + gW[Y] * y + gW[Z] * z;
}



/*****************************************************************************
*
* FUNCTION
*
*   Init_View_Coordinates
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
*   Init the matrices and vectors used to transform a point from
*   the world coordinate system to the viewer's coordinate system.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void init_view_coordinates()
{
  DBL k1, k2, k3, up_length, right_length;
  MATRIX A, B;

  Assign_Vector(gU, Frame.Camera->Right);
  Assign_Vector(gV, Frame.Camera->Up);
  Assign_Vector(gW, Frame.Camera->Direction);

  VAdd (gO, Frame.Camera->Location, Frame.Camera->Direction);

  VNormalize(gU,gU);
  VNormalize(gV,gV);
  VNormalize(gW,gW);

  VDot(k1, gU, gV);
  VDot(k2, gU, gW);
  VDot(k3, gV, gW);

  if ((fabs(k1) > EPSILON) || (fabs(k2) > EPSILON) || (fabs(k3) > EPSILON))
  {
    Error("Cannot use non-perpendicular camera vectors with vista buffer.\n");
  }

  VLength (Distance, Frame.Camera->Direction);

  VLength (up_length, Frame.Camera->Up);
  VLength (right_length, Frame.Camera->Right);

  VScaleEq (gU, 1.0/right_length);
  VScaleEq (gV, 1.0/up_length);
  VScaleEq (gW, 1.0/Distance);

  A[0][0] = gU[X]; A[0][1] = gU[Y]; A[0][2] = gU[Z]; A[0][3] = 0.0;
  A[1][0] = gV[X]; A[1][1] = gV[Y]; A[1][2] = gV[Z]; A[1][3] = 0.0;
  A[2][0] = gW[X]; A[2][1] = gW[Y]; A[2][2] = gW[Z]; A[2][3] = 0.0;
  A[3][0] = 0.0;  A[3][1] = 0.0;  A[3][2] = 0.0;  A[3][3] = 1.0;

  B[0][0] = 1.0; B[0][1] = 0.0; B[0][2] = 0.0; B[0][3] = -gO[X];
  B[1][0] = 0.0; B[1][1] = 1.0; B[1][2] = 0.0; B[1][3] = -gO[Y];
  B[2][0] = 0.0; B[2][1] = 0.0; B[2][2] = 1.0; B[2][3] = -gO[Z];
  B[3][0] = 0.0; B[3][1] = 0.0; B[3][2] = 0.0; B[3][3] = 1.0;

  MTimes(WC2VC, A, B);
  MInvers(WC2VCinv, WC2VC);
}



/*****************************************************************************
*
* FUNCTION
*
*   get_perspective_projection
*
* INPUT
*
*   Object   - Object to project
*   Project  - Projection
*   infinite - Flag if object is infinite
*   
* OUTPUT
*
*   Project
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Get the perspective projection of a single object, i.e.
*   the smallest rectangle enclosing the object's image on the screen.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void get_perspective_projection(OBJECT *Object, PROJECT *Project, int infinite)
{
  int visible;
  METHODS *Methods;

  visible = FALSE;

  Methods = Object->Methods;

  /* If the object is infinite, there's no sense of projecting */

  if (!infinite)
  {
    if ((Methods == &Box_Methods) ||
        (Methods == &Smooth_Triangle_Methods) ||
        (Methods == &Triangle_Methods) ||
        (Methods == &HField_Methods))
    {
      if (Methods == &Box_Methods)
        project_box(Project, Object, &visible);

      if (Methods == &HField_Methods)
        project_hfield(Project, Object, &visible);

      if (Methods == &Smooth_Triangle_Methods)
        project_smooth_triangle(Project, Object, &visible);

      if (Methods == &Triangle_Methods)
        project_triangle(Project, Object, &visible);
    }
    else
    {
      project_bounds(Project, &Object->BBox, &visible);
    }
  }

  if (visible)
  {
    if (opts.Options & ANTIALIAS)
    {
      /* Increase the rectangle to make sure that nothing will be missed.
         For anti-aliased images increase by a larger amount. */

      Project->x1 = max (0,                     Project->x1 - 2);
      Project->x2 = min (Frame.Screen_Width-1,  Project->x2 + 2);
      Project->y1 = max (-1,                    Project->y1 - 2);
      Project->y2 = min (Frame.Screen_Height-1, Project->y2 + 2);
    }
    else
    {
      /* Increase the rectangle to make sure that nothing will be missed. */

      Project->x1 = max (0,                     Project->x1 - 1);
      Project->x2 = min (Frame.Screen_Width-1,  Project->x2 + 1);
      Project->y1 = max (0,                     Project->y1 - 1);
      Project->y2 = min (Frame.Screen_Height-1, Project->y2 + 1);
    }
  }
  else
  {
    if (!infinite)
    {
      /* Object is invisible (the camera can't see it) */

      Project->x1 = Project->y1 = MAX_BUFFER_ENTRY;
      Project->x2 = Project->y2 = MIN_BUFFER_ENTRY;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   get_orthographic_projection
*
* INPUT
*
*   Object   - Object to project
*   Project  - Projection
*   infinite - Flag if object is infinite
*   
* OUTPUT
*
*   Project
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Get the orthographic projection of a single object, i.e.
*   the smallest rectangle enclosing the object's image on the screen.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void get_orthographic_projection(OBJECT *Object, PROJECT *Project, int infinite)
{
  int visible, i, x, y;
  VECTOR P[8];

  visible = FALSE;

  /* If the object is infinite, there's no sense of projecting */

  if (!infinite)
  {
    /* The following could be done better but since only a minority of all
       objects in a scene are partially visible I don't think it's worth it. */

    for (i = 0; i < 8; i++)
    {
      P[i][X] = ((i & 1) ? Object->BBox.Lengths[X] : 0.0) + Object->BBox.Lower_Left[X];
      P[i][Y] = ((i & 2) ? Object->BBox.Lengths[Y] : 0.0) + Object->BBox.Lower_Left[Y];
      P[i][Z] = ((i & 4) ? Object->BBox.Lengths[Z] : 0.0) + Object->BBox.Lower_Left[Z];

      transform_point(P[i]);

      /* Check if bounding box is visible */

      if (P[i][Z] >= 0.0) visible = TRUE;
    }

    /* Now get the projection */

    if (visible)
    {
      Project->x1 = Project->y1 = MAX_BUFFER_ENTRY;
      Project->x2 = Project->y2 = MIN_BUFFER_ENTRY;

      for (i = 0; i < 8; i++)
      {
        /* The visible area is -0.5...+0.5/-0.5...+0.5 */

        if (P[i][X] < -0.5) P[i][X] = -0.5;
        if (P[i][X] >  0.5) P[i][X] =  0.5;
        if (P[i][Y] < -0.5) P[i][Y] = -0.5;
        if (P[i][Y] >  0.5) P[i][Y] =  0.5;

        x = Frame.Screen_Width/2  + (int)(Frame.Screen_Width  * P[i][X]);
        y = Frame.Screen_Height/2 - (int)(Frame.Screen_Height * P[i][Y]);

        if (x < Project->x1) Project->x1 = x;
        if (x > Project->x2) Project->x2 = x;
        if (y < Project->y1) Project->y1 = y;
        if (y > Project->y2) Project->y2 = y;
      }
    }
  }

  if (visible)
  {
    if (opts.Options & ANTIALIAS)
    {
      /* Increase the rectangle to make sure that nothing will be missed.
         For anti-aliased images decrease the lower borders. */

      Project->x1 = max (0,                     Project->x1 - 2);
      Project->x2 = min (Frame.Screen_Width-1,  Project->x2 + 1);
      Project->y1 = max (-1,                    Project->y1 - 2);
      Project->y2 = min (Frame.Screen_Height-1, Project->y2 + 1);
    }
    else
    {
      /* Increase the rectangle to make sure that nothing will be missed. */

      Project->x1 = max (0,                     Project->x1 - 1);
      Project->x2 = min (Frame.Screen_Width-1,  Project->x2 + 1);
      Project->y1 = max (0,                     Project->y1 - 1);
      Project->y2 = min (Frame.Screen_Height-1, Project->y2 + 1);
    }
  }
  else
  {
    if (!infinite)
    {
      /* Object is invisible (the camera can't see it) */

      Project->x1 = Project->y1 = MAX_BUFFER_ENTRY;
      Project->x2 = Project->y2 = MIN_BUFFER_ENTRY;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   project_object
*
* INPUT
*
*   Object   - Object to project
*   Project  - Projection
*   
* OUTPUT
*
*   Project
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Get the projection of a single object depending on the camera
*   used (perspective/orthographic).
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_object(OBJECT *Object, PROJECT *Project)
{
  int infinite;

  /* Init project fields, assuming the object is visible! */

  Project->x1 = Project->y1 = MIN_BUFFER_ENTRY;
  Project->x2 = Project->y2 = MAX_BUFFER_ENTRY;

  infinite = Test_Flag(Object, INFINITE_FLAG);

  switch (Frame.Camera->Type)
  {
    case PERSPECTIVE_CAMERA:

      get_perspective_projection(Object, Project, infinite);
      break;

    case ORTHOGRAPHIC_CAMERA:

      get_orthographic_projection(Object, Project, infinite);
      break;

    default:

      Error("Wrong camera type in project_object().\n");
      break;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   project_bounding_slab
*
* INPUT
*
*   Project  - Projection
*   Tree     - Current node/leaf
*   Object   - Node/leaf in bounding slab hierarchy
*   
* OUTPUT
*
*   Project, Tree
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Project the bounding slab hierarchy onto the screen and thus create
*   the vista buffer hierarchy.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_bounding_slab(PROJECT *Project, PROJECT_TREE_NODE **Tree, BBOX_TREE *Node)
{
  short int i;
  PROJECT Temp;
  PROJECT_TREE_LEAF *Leaf;
  PROJECT_TREE_NODE New;

  if (Node->Entries)
  {
    /* Current object is a bounding object, i.e. a node in the slab tree. */

    /* First, init new entry. */

    New.Entries = 0;

    New.Node = Node;

    New.Project.x1 = New.Project.y1 = MAX_BUFFER_ENTRY;
    New.Project.x2 = New.Project.y2 = MIN_BUFFER_ENTRY;

    /* Allocate temporary memory for node/leaf entries. */

    New.Entry = (PROJECT_TREE_NODE **)POV_MALLOC(Node->Entries*sizeof(PROJECT_TREE_NODE *), "temporary tree entry");

    /* This is no leaf, it's a node. */

    New.is_leaf = FALSE;

    /* Second, get new entry, i.e. project node's entries. */

    for (i = 0; i < Node->Entries; i++)
    {
      New.Entry[i] = NULL;

      project_bounding_slab(&Temp, &New.Entry[New.Entries], Node->Node[i]);

      /* Use only visible entries. */

      if (New.Entry[New.Entries] != NULL)
      {
        New.Project.x1 = min(New.Project.x1, Temp.x1);
        New.Project.x2 = max(New.Project.x2, Temp.x2);
        New.Project.y1 = min(New.Project.y1, Temp.y1);
        New.Project.y2 = max(New.Project.y2, Temp.y2);

        New.Entries++;
      }
    }

    /* If there are any visible entries, we'll use them. */

    if (New.Entries > 0)
    {
      /* If there's only one entry, we won't need a new node. */

      if (New.Entries == 1)
      {
        *Tree    = New.Entry[0];
        *Project = New.Project;
      }
      else
      {
        /* Allocate memory for new node in the vista tree. */

        *Tree = (PROJECT_TREE_NODE *)POV_MALLOC(sizeof(PROJECT_TREE_NODE), "vista tree node");

        **Tree = New;

        /* Allocate memory for node/leaf entries. */

        (*Tree)->Entry = (PROJECT_TREE_NODE **)POV_MALLOC(New.Entries*sizeof(PROJECT_TREE_NODE *), "vista tree node");

        memcpy((*Tree)->Entry, New.Entry, New.Entries*sizeof(PROJECT_TREE_NODE *));

        *Project = New.Project;
      }
    }

    /* Get rid of temporary node/leaf entries. */

    POV_FREE(New.Entry);
  }
  else
  {
    COOPERATE_0

    /* Current object is a normal object, i.e. a leaf in the slab tree. */

    /* Get object's projection. */

    project_object((OBJECT *)Node->Node, Project);

    /* Is the object visible? */

    if ((Project->x1 <= Project->x2) && (Project->y1 <= Project->y2))
    {
      /* Allocate memory for new leaf in the vista tree.  */

      *Tree = (PROJECT_TREE_NODE *)POV_MALLOC(sizeof(PROJECT_TREE_LEAF), "vista tree leaf");

      /* Init new leaf. */

      Leaf = (PROJECT_TREE_LEAF *)(*Tree);

      Leaf->Node = Node;

      Leaf->Project = *Project;

      /* Yes, this is a leaf. */

      Leaf->is_leaf = TRUE;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Build_Vista_Buffer
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
*   Build the vista tree, i.e. the 2d representation of the bounding slab
*   hierarchy in image space.
*
*   This only works for perspective and orthographic cameras.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Build_Vista_Buffer()
{
  PROJECT Project;

  Root_Vista = NULL;

  /* Check if vista buffer can be used. */

  if ((!opts.Use_Slabs) ||
      (Frame.Camera->Tnormal != NULL) ||
      ((Frame.Camera->Type != PERSPECTIVE_CAMERA) && (Frame.Camera->Type != ORTHOGRAPHIC_CAMERA)) ||
      ((Frame.Camera->Aperture != 0.0) && (Frame.Camera->Blur_Samples > 0)))
  {
    opts.Options &= ~USE_VISTA_BUFFER;
  }

  if (opts.Options & USE_VISTA_BUFFER)
  {
    Status_Info("\nCreating vista buffer.");

    init_view_coordinates();

    project_bounding_slab(&Project, &Root_Vista, Root_Object);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Vista_Buffer
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
*   Destroy the vista tree.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

void Destroy_Vista_Buffer()
{
  if ((opts.Options & USE_VISTA_BUFFER) && (Root_Vista != NULL))
  {
    Destroy_Project_Tree(Root_Vista);

    Root_Vista = NULL;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   draw_projection
*
* INPUT
*
*   Project - projection to draw
*   color   - Color to be used
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
*   Draws a projection in the specified color.
*
* CHANGES
*
*   May 1994 : Creation.
*   Jul 1996 : Draw boxes in white when doing grayscale preview
*
******************************************************************************/

static void draw_projection(PROJECT *Project, int color, int  *BigRed, int  *BigBlue)
{
  int x1, x2, y1, y2, draw_it;
  unsigned char r, g, b, gray;
  unsigned char a=0;

  gray = (opts.PaletteOption == GREY) ? 255 : 0;

  switch (color)
  {
    case RED   : r = 255; g = b = gray; break;
    case GREEN : g = 255; r = b = gray; break;
    case BLUE  : b = 255; r = g = gray; break;
    default    : r = g = b = 255;
  }

  x1 = Project->x1;
  x2 = Project->x2;
  y1 = Project->y1;
  y2 = Project->y2;

  if ((x1 <= x2) && (y1 <= y2))
  {
    if (x1 < 0) x1 = 0;
    if (x2 < 0) x2 = 0;
    if (y1 < 0) y1 = 0;
    if (y2 < 0) y2 = 0;

    if (x1 >= Frame.Screen_Width)  x1 = Frame.Screen_Width - 1;
    if (x2 >= Frame.Screen_Width)  x2 = Frame.Screen_Width - 1;
    if (y1 >= Frame.Screen_Height) y1 = Frame.Screen_Height - 1;
    if (y2 >= Frame.Screen_Height) y2 = Frame.Screen_Height - 1;

    /* Check for full-screen rectangle. */

    draw_it = TRUE;

    if ((x1 == 0) && (x2 == Frame.Screen_Width - 1) &&
        (y1 == 0) && (y2 == Frame.Screen_Height - 1))
    {
      draw_it = FALSE;

      switch (color)
      {
        case RED   : if (!(*BigRed))  { *BigRed  = draw_it = TRUE; } break;
        case BLUE  : if (!(*BigBlue)) { *BigBlue = draw_it = TRUE; } break;
      }
    }

    if (draw_it)
    {
      POV_DISPLAY_PLOT_BOX(x1,y1,x2,y2,r,g,b,a);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   draw_vista
*
* INPUT
*
*   Tree - current node/leaf in the vista tree
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
*   Draws recursively all projections of subnodes in the current node.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void draw_vista(PROJECT_TREE_NODE *Tree, int  *BigRed, int *BigBlue)
{
  unsigned short i;
  PROJECT_TREE_LEAF *Leaf;

  if (Tree->is_leaf)
  {
    Leaf = (PROJECT_TREE_LEAF *)Tree;

    COOPERATE_1

    if (((OBJECT *)Leaf->Node->Node)->Type & COMPOUND_OBJECT)
    {
      draw_projection(&Leaf->Project, BLUE, BigRed, BigBlue);
    }
    else
    {
      draw_projection(&Leaf->Project, RED, BigRed, BigBlue);
    }
  }
  else
  {
    for (i = 0; i < Tree->Entries; i++)
    {
      draw_vista(Tree->Entry[i], BigRed, BigBlue);
    }
  }

  /* draw bounding object's vista */

/*
  draw_projection(&Tree->Project, GREEN);
*/
}



/*****************************************************************************
*
* FUNCTION
*
*   Draw_Vista_Buffer
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
*   Draw the vista tree.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Draw_Vista_Buffer()
{
  int BigRed, BigBlue;

  BigRed = BigBlue = FALSE;

  if ((Root_Vista != NULL) && (opts.Options & USE_VISTA_DRAW))
  {
    draw_vista(Root_Vista, &BigRed, &BigBlue);
  }
}
