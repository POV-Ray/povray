/****************************************************************************
*                   lbuffer.c
*
*  This module implements functions that implement the light buffer.
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
#include "point.h"
#include "povray.h"
#include "bbox.h"
#include "lbuffer.h"
#include "objects.h"
#include "triangle.h"
#include "vlbuffer.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variabls
******************************************************************************/

/* Planes for 3d-clipping. */

static VECTOR VIEW_VX1 = {-0.7071067812, 0.0, -0.7071067812};
static VECTOR VIEW_VX2 = { 0.7071067812, 0.0, -0.7071067812};
static VECTOR VIEW_VY1 = {0.0, -0.7071067812, -0.7071067812};
static VECTOR VIEW_VY2 = {0.0,  0.7071067812, -0.7071067812};
static DBL VIEW_DX1 = 0.0;
static DBL VIEW_DX2 = 0.0;
static DBL VIEW_DY1 = 0.0;
static DBL VIEW_DY2 = 0.0;



/*****************************************************************************
* Static functions
******************************************************************************/

static void calc_points (int Axis, OBJECT *Object, int *Number, VECTOR *Points, VECTOR Origin);

static int bbox_invisible (int Axis, BBOX *BBox, VECTOR Origin);

static void project_rectangle (PROJECT *Project, VECTOR P1, VECTOR P2, VECTOR P3, VECTOR P4, int *visible);
static void project_triangle (PROJECT *Project, VECTOR P1, VECTOR P2, VECTOR P3, int *visible);
static void project_bbox (PROJECT *Project, VECTOR *P, int *visible);
static void project_object (PROJECT *Project, OBJECT *Object, int Axis, VECTOR Origin);

static void project_bounding_slab (int Axis, VECTOR Origin,
  PROJECT *Project, PROJECT_TREE_NODE **Entry, BBOX_TREE *Node);



/*****************************************************************************
*
* FUNCTION
*
*   calc_points
*
* INPUT
*
*   Axis   - Axis along the objects will be projected
*   Object - Object
*   Number - Number of points to project
*   Points - Points to project
*   Origin - Origin of current light source
*   
* OUTPUT
*
*   Number, Points
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the points to project depending on the object type,
*   the light source position and the axis. Note that only three
*   points are used for triangles and eight for all other objects.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void calc_points(int Axis, OBJECT *Object, int *Number, VECTOR *Points, VECTOR  Origin)
{
  register int i;
  DBL Direction;
  VECTOR H[8];

  /* Get points depending on object's type */

  if ((Object->Methods != &Triangle_Methods) &&
      (Object->Methods != &Smooth_Triangle_Methods))
  {
    *Number = 8;

    for (i = 0; i < 8; i++)
    {
      H[i][X] = ((i & 1) ? Object->BBox.Lengths[X] : 0.0) + Object->BBox.Lower_Left[X];
      H[i][Y] = ((i & 2) ? Object->BBox.Lengths[Y] : 0.0) + Object->BBox.Lower_Left[Y];
      H[i][Z] = ((i & 4) ? Object->BBox.Lengths[Z] : 0.0) + Object->BBox.Lower_Left[Z];
    }
  }
  else
  {
    if (Object->Methods == &Triangle_Methods)
    {
      *Number = 3;

      Assign_Vector(H[0], ((TRIANGLE *)Object)->P1);
      Assign_Vector(H[1], ((TRIANGLE *)Object)->P2);
      Assign_Vector(H[2], ((TRIANGLE *)Object)->P3);
    }

    if (Object->Methods == &Smooth_Triangle_Methods)
    {
      *Number = 3;

      Assign_Vector(H[0], ((SMOOTH_TRIANGLE *)Object)->P1);
      Assign_Vector(H[1], ((SMOOTH_TRIANGLE *)Object)->P2);
      Assign_Vector(H[2], ((SMOOTH_TRIANGLE *)Object)->P3);
    }
  }

  /* Modify points so that the new z direction is the projection axis. */

  if ((Axis == XaxisP) || (Axis == YaxisP) || (Axis == ZaxisP))
  {
    Direction = 1.0;
  }
  else
  {
    Direction = -1.0;
  }

  switch (Axis)
  {
    case XaxisP :
    case XaxisM :

      for (i = 0; i < *Number; i++)
      {
        Points[i][X] = (H[i][Y] - Origin[Y]);
        Points[i][Y] = (H[i][Z] - Origin[Z]);
        Points[i][Z] = (H[i][X] - Origin[X]) * Direction;
      }

      break;

    case YaxisP :
    case YaxisM :

      for (i = 0; i < *Number; i++)
      {
        Points[i][X] = (H[i][X] - Origin[X]);
        Points[i][Y] = (H[i][Z] - Origin[Z]);
        Points[i][Z] = (H[i][Y] - Origin[Y]) * Direction;
      }

      break;

    case ZaxisP :
    case ZaxisM :

      for (i = 0; i < *Number; i++)
      {
        Points[i][X] = (H[i][X] - Origin[X]);
        Points[i][Y] = (H[i][Y] - Origin[Y]);
        Points[i][Z] = (H[i][Z] - Origin[Z]) * Direction;
      }

      break;

    default : Error("Illegal axis in module calc_points() in lbuffer.c.\n");
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   bbox_invisible
*
* INPUT
*
*   Axis   - Axis along the objects will be projected
*   BBox   - Bounding box to test
*   Origin - Origin of current light source
*
* OUTPUT
*
* RETURNS
*
*   int - Flag if bounding box is totally invisble
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Do a quick test if a bounding box is totally invisble from the
*   current light source in the specified axis direction.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int bbox_invisible(int Axis, BBOX *BBox, VECTOR Origin)
{
  DBL x1, y1, z1, x2, y2, z2, x, y, z;

  switch (Axis)
  {
    case XaxisP :

      /* Bounding box behind light source? */

      if ((x = BBox->Lower_Left[X] + BBox->Lengths[X] - Origin[X]) <= 0.0)
      {
        return(TRUE);
      }

      /* Bounding box on the right/left side? */

      y1 = BBox->Lower_Left[Y] - Origin[Y];
      y2 = y1 + BBox->Lengths[Y];

      if (((y1 > 0.0) && (y1 > x)) || ((y2 < 0.0) && (-y2 > x)))
      {
        return(TRUE);
      }

      /* Bounding box on the bottom/top side? */

      z1 = BBox->Lower_Left[Z] - Origin[Z];
      z2 = z1 + BBox->Lengths[Z];

      if (((z1 > 0.0) && (z1 > x)) || ((z2 < 0.0) && (-z2 > x)))
      {
        return(TRUE);
      }

      break;

    case XaxisM :

      /* Bounding box behind light source? */

      if ((x = BBox->Lower_Left[X] - Origin[X]) >= 0.0)
      {
        return(TRUE);
      }

      /* Bounding box on the right/left side? */

      y1 = BBox->Lower_Left[Y] - Origin[Y];
      y2 = y1 + BBox->Lengths[Y];

      if (((y1 > 0.0) && (y1 > -x)) || ((y2 < 0.0) && (y2 < x)))
      {
        return(TRUE);
      }

      /* Bounding box on the bottom/top side? */

      z1 = BBox->Lower_Left[Z] - Origin[Z];
      z2 = z1 + BBox->Lengths[Z];

      if (((z1 > 0.0) && (z1 > -x)) || ((z2 < 0.0) && (z2 < x)))
      {
        return(TRUE);
      }

      break;

    case YaxisP :

      /* Bounding box behind light source? */

      if ((y = BBox->Lower_Left[Y] + BBox->Lengths[Y] - Origin[Y]) <= 0.0)
      {
        return(TRUE);
      }

      /* Bounding box on the right/left side? */

      x1 = BBox->Lower_Left[X] - Origin[X];
      x2 = x1 + BBox->Lengths[X];

      if (((x1 > 0.0) && (x1 > y)) || ((x2 < 0.0) && (-x2 > y)))
      {
        return(TRUE);
      }

      /* Bounding box on the bottom/top side? */

      z1 = BBox->Lower_Left[Z] - Origin[Z];
      z2 = z1 + BBox->Lengths[Z];

      if (((z1 > 0.0) && (z1 > y)) || ((z2 < 0.0) && (-z2 > y)))
      {
        return(TRUE);
      }

      break;

    case YaxisM :

      /* Bounding box behind light source? */

      if ((y = BBox->Lower_Left[Y] - Origin[Y]) >= 0.0)
      {
        return(TRUE);
      }

      /* Bounding box on the right/left side? */

      x1 = BBox->Lower_Left[X] - Origin[X];
      x2 = x1 + BBox->Lengths[X];

      if (((x1 > 0.0) && (x1 > -y)) || ((x2 < 0.0) && (x2 < y)))
      {
        return(TRUE);
      }

      /* Bounding box on the bottom/top side? */

      z1 = BBox->Lower_Left[Z] - Origin[Z];
      z2 = z1 + BBox->Lengths[Z];

      if (((z1 > 0.0) && (z1 > -y)) || ((z2 < 0.0) && (z2 < y)))
      {
        return(TRUE);
      }

      break;

    case ZaxisP :

      /* Bounding box behind light source? */

      if ((z = BBox->Lower_Left[Z] + BBox->Lengths[Z] - Origin[Z]) <= 0.0)
      {
        return(TRUE);
      }

      /* Bounding box on the right/left side? */

      x1 = BBox->Lower_Left[X] - Origin[X];
      x2 = x1 + BBox->Lengths[X];

      if (((x1 > 0.0) && (x1 > z)) || ((x2 < 0.0) && (-x2 > z)))
      {
        return(TRUE);
      }

      /* Bounding box on the bottom/top side? */

      y1 = BBox->Lower_Left[Y] - Origin[Y];
      y2 = y1 + BBox->Lengths[Y];

      if (((y1 > 0.0) && (y1 > z)) || ((y2 < 0.0) && (-y2 > z)))
      {
        return(TRUE);
      }

      break;

    case ZaxisM :

      /* Bounding box behind light source? */

      if ((z = BBox->Lower_Left[Z] - Origin[Z]) >= 0.0)
      {
        return(TRUE);
      }

      /* Bounding box on the right/left side? */

      x1 = BBox->Lower_Left[X] - Origin[X];
      x2 = x1 + BBox->Lengths[X];

      if (((x1 > 0.0) && (x1 > -z)) || ((x2 < 0.0) && (x2 < z)))
      {
        return(TRUE);
      }

      /* Bounding box on the bottom/top side? */

      y1 = BBox->Lower_Left[Y] - Origin[Y];
      y2 = y1 + BBox->Lengths[Y];

      if (((y1 > 0.0) && (y1 > -z)) || ((y2 < 0.0) && (y2 < z)))
      {
        return(TRUE);
      }

      break;

    default :

      Error("Illegal axis in bbox_invisible() in lbuffer.c.\n");
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   project_rectangle
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
*   Project a rectangle onto a light source.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_rectangle(PROJECT *Project, VECTOR P1, VECTOR  P2, VECTOR  P3, VECTOR  P4, int *visible)
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
      if (Points[i][Z] < EPSILON)
      {
        Points[i][X] = Points[i][Y] = 0.0;
      }
      else
      {
        Points[i][X] /= Points[i][Z];
        Points[i][Y] /= Points[i][Z];

        if (fabs(Points[i][X]) < EPSILON) Points[i][X] = 0.0;
        if (fabs(Points[i][Y]) < EPSILON) Points[i][Y] = 0.0;
      }

      x = (int)(MAX_BUFFER_ENTRY * Points[i][X]);
      y = (int)(MAX_BUFFER_ENTRY * Points[i][Y]);

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
*   project_triangle
*
* INPUT
*
*   Project    - Triangle's projection
*   P1, P2, P3 - Triangles's edges
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
*   Project a triangle onto a light source.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_triangle(PROJECT *Project, VECTOR P1, VECTOR  P2, VECTOR  P3, int *visible)
{
  VECTOR Points[MAX_CLIP_POINTS];
  int i, number;
  int x, y, clip;

  clip = TRUE;

  /* Check if all points lie "in front" of the light source. */

  if ((P1[Z] > 0.0) && (P2[Z] > 0.0) && (P3[Z] > 0.0))
  {
    /* Check if all points lie inside the "viewing pyramid". */

    if ((fabs(P1[X]) <= P1[Z]) && (fabs(P2[X]) <= P2[Z]) && (fabs(P3[X]) <= P3[Z]) &&
        (fabs(P1[Y]) <= P1[Z]) && (fabs(P2[Y]) <= P2[Z]) && (fabs(P3[Y]) <= P3[Z]))
    {
      /* No clipping is needed. Just project the points. */

      clip = FALSE;
    }
    else
    {
      /* Check if all points lie on the "right side". */

      if ((P1[X] > 0.0) && (P1[X] > P1[Z]) &&
          (P2[X] > 0.0) && (P2[X] > P2[Z]) &&
          (P3[X] > 0.0) && (P3[X] > P3[Z]))
      {
        return;
      }

      /* Check if all points lie on the "left side". */

      if ((P1[X] < 0.0) && (-P1[X] > P1[Z]) &&
          (P2[X] < 0.0) && (-P2[X] > P2[Z]) &&
          (P3[X] < 0.0) && (-P3[X] > P3[Z]))
      {
        return;
      }

      /* Check if all points lie above the "top side". */

      if ((P1[Y] > 0.0) && (P1[Y] > P1[Z]) &&
          (P2[Y] > 0.0) && (P2[Y] > P2[Z]) &&
          (P3[Y] > 0.0) && (P3[Y] > P3[Z]))
      {
        return;
      }

      /* Check if all points lie below the "bottom side". */

      if ((P1[Y] < 0.0) && (-P1[Y] > P1[Z]) &&
          (P2[Y] < 0.0) && (-P2[Y] > P2[Z]) &&
          (P3[Y] < 0.0) && (-P3[Y] > P3[Z]))
      {
        return;
      }
    }
  }

  Assign_Vector(Points[0], P1);
  Assign_Vector(Points[1], P2);
  Assign_Vector(Points[2], P3);

  number = 3;

  if (clip)
  {
    Clip_Polygon(Points, &number, VIEW_VX1, VIEW_VX2, VIEW_VY1, VIEW_VY2,
                                  VIEW_DX1, VIEW_DX2, VIEW_DY1, VIEW_DY2);
  }

  if (number)
  {
    for (i = 0; i < number; i++)
    {
      if (fabs(Points[i][Z]) < EPSILON)
      {
        Points[i][X] = Points[i][Y] = 0.0;
      }
      else
      {
        Points[i][X] /= Points[i][Z];
        Points[i][Y] /= Points[i][Z];

        if (fabs(Points[i][X]) < EPSILON) Points[i][X] = 0.0;
        if (fabs(Points[i][Y]) < EPSILON) Points[i][Y] = 0.0;
      }

      x = (int)(MAX_BUFFER_ENTRY * Points[i][X]);
      y = (int)(MAX_BUFFER_ENTRY * Points[i][Y]);

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
*   Project_BBox
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
*   Project an axis-aligned box onto a light source.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_bbox(PROJECT *Project, VECTOR *P, int *visible)
{
  int i, x, y;

  /* Check if all points lie "in front" of the light source. */

  if ((P[0][Z] > 0.0) && (P[1][Z] > 0.0) && (P[2][Z] > 0.0) && (P[3][Z] > 0.0) &&
      (P[4][Z] > 0.0) && (P[5][Z] > 0.0) && (P[6][Z] > 0.0) && (P[7][Z] > 0.0))
  {
    /* Check if all points lie inside the "viewing pyramid". */

    if ((fabs(P[0][X]) <= P[0][Z]) && (fabs(P[1][X]) <= P[1][Z]) &&
        (fabs(P[2][X]) <= P[2][Z]) && (fabs(P[3][X]) <= P[3][Z]) &&
        (fabs(P[4][X]) <= P[4][Z]) && (fabs(P[5][X]) <= P[5][Z]) &&
        (fabs(P[6][X]) <= P[6][Z]) && (fabs(P[7][X]) <= P[7][Z]) &&
        (fabs(P[0][Y]) <= P[0][Z]) && (fabs(P[1][Y]) <= P[1][Z]) &&
        (fabs(P[2][Y]) <= P[2][Z]) && (fabs(P[3][Y]) <= P[3][Z]) &&
        (fabs(P[4][Y]) <= P[4][Z]) && (fabs(P[5][Y]) <= P[5][Z]) &&
        (fabs(P[6][Y]) <= P[6][Z]) && (fabs(P[7][Y]) <= P[7][Z]))
    {
      /* No clipping is needed. Just project the points. */

      for (i = 0; i < 8; i++)
      {
        if (P[i][Z] < EPSILON)
        {
          P[i][X] = P[i][Y] = 0.0;
        }
        else
        {
          P[i][X] /= P[i][Z];
          P[i][Y] /= P[i][Z];

          if (fabs(P[i][X]) < EPSILON) P[i][X] = 0.0;
          if (fabs(P[i][Y]) < EPSILON) P[i][Y] = 0.0;
        }

        x = (int)(MAX_BUFFER_ENTRY * P[i][X]);
        y = (int)(MAX_BUFFER_ENTRY * P[i][Y]);

        if (x < Project->x1) Project->x1 = x;
        if (x > Project->x2) Project->x2 = x;
        if (y < Project->y1) Project->y1 = y;
        if (y > Project->y2) Project->y2 = y;
      }

      *visible = TRUE;

      return;
    }
    else
    {
      /* Check if all points lie on the "right side". */

      for (i = 0; i < 8; i++)
      {
        if ((P[i][X] < 0.0) || (P[i][X] <= P[i][Z])) break;
      }

      if (i == 8) return;

      /* Check if all points lie on the "left side". */

      for (i = 0; i < 8; i++)
      {
        if ((P[i][X] > 0.0) || (-P[i][X] <= P[i][Z])) break;
      }

      if (i == 8) return;

      /* Check if all points lie above the "top side". */

      for (i = 0; i < 8; i++)
      {
        if ((P[i][Y] < 0.0) || (P[i][Y] <= P[i][Z])) break;
      }

      if (i == 8) return;

      /* Check if all points lie below the "bottom side". */

      for (i = 0; i < 8; i++)
      {
        if ((P[i][Y] > 0.0) || (-P[i][Y] <= P[i][Z])) break;
      }

      if (i == 8) return;
    }
  }

  project_rectangle(Project, P[0], P[1], P[3], P[2], visible);
  project_rectangle(Project, P[4], P[5], P[7], P[6], visible);
  project_rectangle(Project, P[0], P[1], P[5], P[4], visible);
  project_rectangle(Project, P[2], P[3], P[7], P[6], visible);
  project_rectangle(Project, P[1], P[3], P[7], P[5], visible);
  project_rectangle(Project, P[0], P[2], P[6], P[4], visible);
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
*   Get the projection of a single object onto a light source.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_object(PROJECT *Project, OBJECT *Object, int Axis, VECTOR Origin)
{
  int visible, Number;
  VECTOR Points[8];

  /* Do not project infinite objects (always visible!) */

  if (Test_Flag(Object, INFINITE_FLAG))
  {
    Project->x1 = Project->y1 = MIN_BUFFER_ENTRY;
    Project->x2 = Project->y2 = MAX_BUFFER_ENTRY;

    return;
  }

  /* Get points to project */

  calc_points(Axis, Object, &Number, Points, Origin);

  visible = FALSE;

  Project->x1 = Project->y1 = MAX_BUFFER_ENTRY;
  Project->x2 = Project->y2 = MIN_BUFFER_ENTRY;

  if (Number == 3)
  {
    project_triangle(Project, Points[0], Points[1], Points[2], &visible);
  }
  else
  {
    project_bbox(Project, Points, &visible);
  }

  if (!visible)
  {
    /* Object is invisible */

    Project->x1 = Project->y1 = MAX_BUFFER_ENTRY;
    Project->x2 = Project->y2 = MIN_BUFFER_ENTRY;
  }
  else
  {
    /* We don't want to miss something */

    Project->x1 -= 2;
    Project->x2 += 2;
    Project->y1 -= 2;
    Project->y2 += 2;
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
*   Axis     - Axis along the objects will be projected
*   Origin   - Origin of current light source
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
*   Project the bounding slab hierarchy onto a light source and thus create
*   the light buffer hierarchy for this light source.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void project_bounding_slab(int Axis, VECTOR Origin, PROJECT *Project, PROJECT_TREE_NODE **Tree, BBOX_TREE *Node)
{
  short int i;
  PROJECT Temp;
  PROJECT_TREE_LEAF *Leaf;
  PROJECT_TREE_NODE New;

  COOPERATE_1

  /* If the node is totally invisible we are ready. */

  if (bbox_invisible(Axis, &Node->BBox, Origin))
  {
    return;
  }

  if (Node->Entries)
  {
    /* Current object is a bounding object, i.e. a node in the slab tree. */

    /* First, Init new entry. */

    New.Entries = 0;

    New.Node = Node;

    New.Project.x1 = New.Project.y1 = MAX_BUFFER_ENTRY;
    New.Project.x2 = New.Project.y2 = MIN_BUFFER_ENTRY;

    /* Allocate temporary memory for node/leaf entries. */

    New.Entry = (PROJECT_TREE_NODE **)POV_MALLOC(Node->Entries*sizeof(PROJECT_TREE_NODE *), "temporary tree entry");

    /* This is no leaf, it's a node. */

    New.is_leaf = FALSE;

    /* Second, Get new entry, i.e. project bounding slab's entries. */

    for (i = 0; i < Node->Entries; i++)
    {
      New.Entry[i] = NULL;

      project_bounding_slab(Axis, Origin, &Temp, &New.Entry[New.Entries], Node->Node[i]);

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
        /* Allocate memory for new node in the light tree. */

        *Tree = (PROJECT_TREE_NODE *)POV_MALLOC(sizeof(PROJECT_TREE_NODE), "light tree node");

        **Tree = New;

        /* Allocate memory for node/leaf entries. */

        (*Tree)->Entry = (PROJECT_TREE_NODE **)POV_MALLOC(New.Entries*sizeof(PROJECT_TREE_NODE *), "light tree node");

        memcpy((*Tree)->Entry, New.Entry, New.Entries*sizeof(PROJECT_TREE_NODE *));

        *Project = New.Project;
      }
    }

    /* Get rid of temporary node/leaf entries. */

    POV_FREE(New.Entry);
  }
  else
  {
    /* Current object is a normal object, i.e. a leaf in the slab tree. */

    /* If object doesn't cast shadows we can skip. */

    if (!Test_Flag((OBJECT *)Node->Node, NO_SHADOW_FLAG))
    {
      /* Project object onto light source. */

      project_object(Project, (OBJECT *)Node->Node, Axis, Origin);

      /* Is the object visible? */

      if ((Project->x1 <= Project->x2) && (Project->y1 <= Project->y2))
      {
        /* Allocate memory for new leaf in the light tree. */

        *Tree = (PROJECT_TREE_NODE *)POV_MALLOC(sizeof(PROJECT_TREE_LEAF), "light tree leaf");

        /* Init new leaf. */

        Leaf = (PROJECT_TREE_LEAF *)(*Tree);

        Leaf->Node = Node;

        Leaf->Project = *Project;

        /* Yes, this is a leaf. */

        Leaf->is_leaf = TRUE;
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Build_Light_Buffers
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
*   Build the light buffers, i.e. the 2d representations of the bounding slab
*   hierarchy seen from the light sources.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Build_Light_Buffers()
{
  int Axis;
  PROJECT Project;
  LIGHT_SOURCE *Light;

  if (!(opts.Quality_Flags & Q_SHADOW) || (!opts.Use_Slabs))
  {
    opts.Options &= ~USE_LIGHT_BUFFER;
  }

  if (opts.Options & USE_LIGHT_BUFFER)
  {
    Status_Info("\nCreating light buffers");
    
    /* Build the light buffer for all point(!) light sources */
  
    for (Light = Frame.Light_Sources; Light != NULL; Light = Light->Next_Light_Source)
    {
      if ((!Light->Area_Light) && (Light->Light_Type!=FILL_LIGHT_SOURCE))
      {
        Status_Info(".");
    
        /* Project bounding slabs on all six sides */
  
        for (Axis = 0; Axis < 6; Axis++)
        {
          Light->Light_Buffer[Axis] = NULL;
  
          project_bounding_slab(Axis, Light->Center, &Project,
            &Light->Light_Buffer[Axis], Root_Object);
        }
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Light_Buffers
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
*   Destroy the light buffers.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

void Destroy_Light_Buffers()
{
  int Axis;
  LIGHT_SOURCE *Light;

  if (opts.Options & USE_LIGHT_BUFFER)
  {
    for (Light = Frame.Light_Sources; Light != NULL; Light = Light->Next_Light_Source)
    {
      if ((!Light->Area_Light) && (Light->Light_Type!=FILL_LIGHT_SOURCE))
      {
        for (Axis = 0; Axis < 6; Axis++)
        {
          if (Light->Light_Buffer[Axis] != NULL)
          {
            Destroy_Project_Tree(Light->Light_Buffer[Axis]);
          }

          Light->Light_Buffer[Axis] = NULL;
        }
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Light_Tree
*
* INPUT
*
*   Ray               - Shadow ray
*   Tree              - Light tree's top-node
*   x                 - X-coordinate of the shadow ray
*   y                 - Y-coordinate of the shadow ray
*   Best_Intersection - Intersection found
*   Best_Object       - Object found
*   
* OUTPUT
*
*   Best_Intersection, Best_Object
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Intersect a shadow ray with the light tree
*   (same as for the vista tree but without pruning).
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

int Intersect_Light_Tree(RAY *Ray, PROJECT_TREE_NODE *Tree, int x, int  y, INTERSECTION *Best_Intersection, OBJECT **Best_Object)
{
  INTERSECTION New_Intersection;
  unsigned short i;
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

  /* Traverse the tree. */

  Node_Queue->QSize = 0;

  /* Create the direction vectors for this ray */

  Create_Rayinfo(Ray, &rayinfo);

  /* Fill the priority queue with all possible candidates */

  /* Check root */

  Increase_Counter(stats[LBuffer_Tests]);

  if ((x >= Tree->Project.x1) && (x <= Tree->Project.x2) &&
      (y >= Tree->Project.y1) && (y <= Tree->Project.y2))
  {
    Increase_Counter(stats[LBuffer_Tests_Succeeded]);

    Node_Queue->Queue[(Node_Queue->QSize)++] = Tree;
  }

  /* Loop until queue is empty. */

  while (Node_Queue->QSize > 0)
  {
    Tree = Node_Queue->Queue[--(Node_Queue->QSize)];

    if (Tree->is_leaf)
    {
      /* Leaf --> test object's bounding box in 3d */

      Check_And_Enqueue(VLBuffer_Queue,
        ((PROJECT_TREE_LEAF *)Tree)->Node,
        &((PROJECT_TREE_LEAF *)Tree)->Node->BBox, &rayinfo);
    }
    else
    {
      /* Check siblings of the node in 2d */

      for (i = 0; i < Tree->Entries; i++)
      {
        Node = Tree->Entry[i];

        Increase_Counter(stats[LBuffer_Tests]);

        if ((x >= Node->Project.x1) && (x <= Node->Project.x2) &&
            (y >= Node->Project.y1) && (y <= Node->Project.y2))
        {
          Increase_Counter(stats[LBuffer_Tests_Succeeded]);

          /* Reallocate queues if they're too small. */

          Reinitialize_VLBuffer_Code();

          /* Add node to node queue */

          Node_Queue->Queue[(Node_Queue->QSize)++] = Node;
        }
      }
    }
  }

  /* Now test the candidates in the priority queue */

  while (VLBuffer_Queue->QSize > 0)
  {
    Priority_Queue_Delete(VLBuffer_Queue, &key, &BBox_Node);

    if (key > Best_Intersection->Depth)
    {
      break;
    }

    if (Intersection(&New_Intersection, (OBJECT *)BBox_Node->Node, Ray))
    {
      if (New_Intersection.Depth < Best_Intersection->Depth)
      {
        *Best_Intersection = New_Intersection;

        *Best_Object = (OBJECT *)BBox_Node->Node;

        Found = TRUE;
      }
    }
  }

  return(Found);
}



