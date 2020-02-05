/****************************************************************************
*                   bezier.c
*
*  This module implements the code for Bezier bicubic patch shapes
*
*  This file was written by Alexander Enzmann.  He wrote the code for
*  bezier bicubic patches and generously provided us these enhancements.
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
#include "bezier.h"
#include "matrices.h"
#include "objects.h"
#include "povray.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define BEZIER_EPSILON 1.0e-10

#define BEZIER_TOLERANCE 1.0e-5



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int InvertMatrix (VECTOR in[3], VECTOR out[3]);
static void bezier_value (VECTOR(*Control_Points)[4][4], DBL u0, DBL v0, VECTOR P, VECTOR N);
static int intersect_subpatch (BICUBIC_PATCH *, RAY *, VECTOR [3], DBL [3], DBL [3], DBL *, VECTOR , VECTOR , DBL *, DBL *);
static void find_average (int, VECTOR *, VECTOR , DBL *);
static int spherical_bounds_check (RAY *, VECTOR , DBL);
static int intersect_bicubic_patch0 (RAY *, BICUBIC_PATCH *, ISTACK *);
static DBL point_plane_distance (VECTOR , VECTOR , DBL *);
static DBL determine_subpatch_flatness (VECTOR(*)[4][4]);
static int flat_enough (BICUBIC_PATCH *, VECTOR(*)[4][4]);
static void bezier_bounding_sphere (VECTOR(*)[4][4], VECTOR , DBL *);
static int bezier_subpatch_intersect (RAY *, BICUBIC_PATCH *, VECTOR(*)[4][4], DBL, DBL, DBL, DBL, ISTACK *);
static void bezier_split_left_right (VECTOR(*)[4][4], VECTOR(*)[4][4], VECTOR(*)[4][4]);
static void bezier_split_up_down (VECTOR(*)[4][4], VECTOR(*)[4][4], VECTOR(*)[4][4]);
static int bezier_subdivider (RAY *, BICUBIC_PATCH *, VECTOR(*)[4][4], DBL, DBL, DBL, DBL, int, ISTACK *);
static void bezier_tree_deleter (BEZIER_NODE *Node);
static BEZIER_NODE *bezier_tree_builder (BICUBIC_PATCH *Object, VECTOR(*Patch)[4][4], DBL u0, DBL u1, DBL v0, DBL v1, int depth);
static int bezier_tree_walker (RAY *, BICUBIC_PATCH *, BEZIER_NODE *, ISTACK *);
static BEZIER_NODE *create_new_bezier_node (void);
static BEZIER_VERTICES *create_bezier_vertex_block (void);
static BEZIER_CHILDREN *create_bezier_child_block (void);
static int subpatch_normal (VECTOR v1, VECTOR v2, VECTOR v3, VECTOR Result, DBL *d);
static int All_Bicubic_Patch_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int Inside_Bicubic_Patch (VECTOR IPoint, OBJECT *Object);
static void Bicubic_Patch_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static BICUBIC_PATCH *Copy_Bicubic_Patch (OBJECT *Object);
static void Translate_Bicubic_Patch (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Bicubic_Patch (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Bicubic_Patch (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Bicubic_Patch (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Bicubic_Patch (OBJECT *Object);
static void Destroy_Bicubic_Patch (OBJECT *Object);



/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Bicubic_Patch_Methods =
{
  All_Bicubic_Patch_Intersections,
  Inside_Bicubic_Patch, Bicubic_Patch_Normal, (COPY_METHOD)Copy_Bicubic_Patch,
  Translate_Bicubic_Patch, Rotate_Bicubic_Patch,
  Scale_Bicubic_Patch, Transform_Bicubic_Patch, Invert_Bicubic_Patch,
  Destroy_Bicubic_Patch
};

static int max_depth_reached;

static DBL C[4] = { 1.0, 3.0, 3.0, 1.0 };



/*****************************************************************************
*
* FUNCTION
*
*   create_new_bezier_node
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static BEZIER_NODE *create_new_bezier_node()
{
  BEZIER_NODE *Node = (BEZIER_NODE *)POV_MALLOC(sizeof(BEZIER_NODE), "bezier node");
  
  Node->Data_Ptr = NULL;

  return (Node);
}



/*****************************************************************************
*
* FUNCTION
*
*   create_bezier_vertex_block
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static BEZIER_VERTICES *create_bezier_vertex_block()
{
  BEZIER_VERTICES *Vertices;

  Vertices = (BEZIER_VERTICES *)POV_MALLOC(sizeof(BEZIER_VERTICES), "bezier vertices");
  
  return (Vertices);
}



/*****************************************************************************
*
* FUNCTION
*
*   create_bezier_child_block
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static BEZIER_CHILDREN *create_bezier_child_block()
{
  BEZIER_CHILDREN *Children;
  
  Children = (BEZIER_CHILDREN *)POV_MALLOC(sizeof(BEZIER_CHILDREN), "bezier children");
  
  return (Children);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_tree_builder
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static BEZIER_NODE *bezier_tree_builder(BICUBIC_PATCH *Object, VECTOR (*Patch)[4][4], DBL u0, DBL  u1, DBL  v0, DBL  v1, int depth)
{
  VECTOR Lower_Left[4][4], Lower_Right[4][4];
  VECTOR Upper_Left[4][4], Upper_Right[4][4];
  BEZIER_CHILDREN *Children;
  BEZIER_VERTICES *Vertices;
  BEZIER_NODE *Node = create_new_bezier_node();
  
  if (depth > max_depth_reached)
  {
    max_depth_reached = depth;
  }
  
  /* Build the bounding sphere for this subpatch. */
  
  bezier_bounding_sphere(Patch, Node->Center, &(Node->Radius_Squared));
  
  /*
   * If the patch is close to being flat, then just perform
   * a ray-plane intersection test.
   */
  
  if (flat_enough(Object, Patch))
  {
    /* The patch is now flat enough to simply store the corners. */
    
    Node->Node_Type = BEZIER_LEAF_NODE;
    
    Vertices = create_bezier_vertex_block();
    
    Assign_Vector(Vertices->Vertices[0], (*Patch)[0][0]);
    Assign_Vector(Vertices->Vertices[1], (*Patch)[0][3]);
    Assign_Vector(Vertices->Vertices[2], (*Patch)[3][3]);
    Assign_Vector(Vertices->Vertices[3], (*Patch)[3][0]);
    
    Vertices->uvbnds[0] = u0;
    Vertices->uvbnds[1] = u1;
    Vertices->uvbnds[2] = v0;
    Vertices->uvbnds[3] = v1;

    Node->Data_Ptr = (void *)Vertices;
  }
  else
  {
    if (depth >= Object->U_Steps)
    {
      if (depth >= Object->V_Steps)
      {
        /* We are at the max recursion depth. Just store corners. */
        
        Node->Node_Type = BEZIER_LEAF_NODE;
        
        Vertices = create_bezier_vertex_block();
        
        Assign_Vector(Vertices->Vertices[0], (*Patch)[0][0]);
        Assign_Vector(Vertices->Vertices[1], (*Patch)[0][3]);
        Assign_Vector(Vertices->Vertices[2], (*Patch)[3][3]);
        Assign_Vector(Vertices->Vertices[3], (*Patch)[3][0]);
        
        Vertices->uvbnds[0] = u0;
        Vertices->uvbnds[1] = u1;
        Vertices->uvbnds[2] = v0;
        Vertices->uvbnds[3] = v1;
        
        Node->Data_Ptr = (void *)Vertices;
      }
      else
      {
        bezier_split_up_down(Patch, (VECTOR(*)[4][4])Lower_Left, (VECTOR(*)[4][4])Upper_Left);
        
        Node->Node_Type = BEZIER_INTERIOR_NODE;
        
        Children = create_bezier_child_block();
        
        Children->Children[0] = bezier_tree_builder(Object, (VECTOR(*)[4][4])Lower_Left, u0, u1, v0, (v0 + v1) / 2.0, depth + 1);
        Children->Children[1] = bezier_tree_builder(Object, (VECTOR(*)[4][4])Upper_Left, u0, u1, (v0 + v1) / 2.0, v1, depth + 1);
        
        Node->Count = 2;
        
        Node->Data_Ptr = (void *)Children;
      }
    }
    else
    {
      if (depth >= Object->V_Steps)
      {
        bezier_split_left_right(Patch, (VECTOR(*)[4][4])Lower_Left, (VECTOR(*)[4][4])Lower_Right);
        
        Node->Node_Type = BEZIER_INTERIOR_NODE;
        
        Children = create_bezier_child_block();
        
        Children->Children[0] = bezier_tree_builder(Object, (VECTOR(*)[4][4])Lower_Left, u0, (u0 + u1) / 2.0, v0, v1, depth + 1);
        Children->Children[1] = bezier_tree_builder(Object, (VECTOR(*)[4][4])Lower_Right, (u0 + u1) / 2.0, u1, v0, v1, depth + 1);
        
        Node->Count = 2;
        
        Node->Data_Ptr = (void *)Children;
      }
      else
      {
        bezier_split_left_right(Patch, (VECTOR(*)[4][4])Lower_Left, (VECTOR(*)[4][4])Lower_Right);
        
        bezier_split_up_down((VECTOR(*)[4][4])Lower_Left, (VECTOR(*)[4][4])Lower_Left, (VECTOR(*)[4][4])Upper_Left);
        
        bezier_split_up_down((VECTOR(*)[4][4])Lower_Right, (VECTOR(*)[4][4])Lower_Right, (VECTOR(*)[4][4])Upper_Right);
        
        Node->Node_Type = BEZIER_INTERIOR_NODE;
        
        Children = create_bezier_child_block();
        
        Children->Children[0] = bezier_tree_builder(Object, (VECTOR(*)[4][4])Lower_Left, u0, (u0 + u1) / 2.0, v0, (v0 + v1) / 2.0, depth + 1);
        Children->Children[1] = bezier_tree_builder(Object, (VECTOR(*)[4][4])Upper_Left, u0, (u0 + u1) / 2.0, (v0 + v1) / 2.0, v1, depth + 1);
        Children->Children[2] = bezier_tree_builder(Object, (VECTOR(*)[4][4])Lower_Right, (u0 + u1) / 2.0, u1, v0, (v0 + v1) / 2.0, depth + 1);
        Children->Children[3] = bezier_tree_builder(Object, (VECTOR(*)[4][4])Upper_Right, (u0 + u1) / 2.0, u1, (v0 + v1) / 2.0, v1, depth + 1);
        
        Node->Count = 4;
        
        Node->Data_Ptr = (void *)Children;
      }
    }
  }
  
  return (Node);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_value
*
* INPUT
*
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Determine the position and normal at a single coordinate
*   point (u, v) on a Bezier patch.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void bezier_value(VECTOR (*Control_Points)[4][4], DBL u0, DBL  v0, VECTOR P, VECTOR  N)
{
  int i, j;
  DBL c, t, ut, vt;
  DBL u[4], uu[4], v[4], vv[4];
  DBL du[4], duu[4], dv[4], dvv[4];
  VECTOR U1, V1;
  
  /* Calculate binomial coefficients times coordinate positions. */
  
  u[0] = 1.0; uu[0] = 1.0; du[0] = 0.0; duu[0] = 0.0;
  v[0] = 1.0; vv[0] = 1.0; dv[0] = 0.0; dvv[0] = 0.0;

  for (i = 1; i < 4; i++)
  {
    u[i] = u[i - 1] * u0;  uu[i] = uu[i - 1] * (1.0 - u0);
    v[i] = v[i - 1] * v0;  vv[i] = vv[i - 1] * (1.0 - v0);

    du[i] = i * u[i - 1];  duu[i] = -i * uu[i - 1];
    dv[i] = i * v[i - 1];  dvv[i] = -i * vv[i - 1];
  }

  /* Now evaluate position and tangents based on control points. */

  Make_Vector(P, 0, 0, 0);
  Make_Vector(U1, 0, 0, 0);
  Make_Vector(V1, 0, 0, 0);

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      c = C[i] * C[j];
      
      ut = u[i] * uu[3 - i];
      vt = v[j] * vv[3 - j];
      
      t = c * ut * vt;
      
      VAddScaledEq(P, t, (*Control_Points)[i][j]);
      
      t = c * vt * (du[i] * uu[3 - i] + u[i] * duu[3 - i]);
      
      VAddScaledEq(U1, t, (*Control_Points)[i][j]);
      
      t = c * ut * (dv[j] * vv[3 - j] + v[j] * dvv[3 - j]);
      
      VAddScaledEq(V1, t, (*Control_Points)[i][j]);
    }
  }
  
  /* Make the normal from the cross product of the tangents. */
  
  VCross(N, U1, V1);
  
  VDot(t, N, N);
  
  if (t > BEZIER_EPSILON)
  {
    t = 1.0 / sqrt(t);
    
    VScaleEq(N, t);
  }
  else
  {
    Make_Vector(N, 1, 0, 0)
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   subpatch_normal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Calculate the normal to a subpatch (triangle) return the vector
*   <1.0 0.0 0.0> if the triangle is degenerate.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int subpatch_normal(VECTOR v1, VECTOR  v2, VECTOR  v3, VECTOR  Result, DBL *d)
{
  VECTOR V1, V2;
  DBL Length;
  
  VSub(V1, v1, v2);
  VSub(V2, v3, v2);

  VCross(Result, V1, V2);

  VLength(Length, Result);

  if (Length < BEZIER_EPSILON)
  {
    Make_Vector(Result, 1.0, 0.0, 0.0);

    *d = -1.0 * v1[X];

    return (0);
  }
  else
  {
    VInverseScale(Result, Result, Length);

    VDot(*d, Result, v1);

    *d = 0.0 - *d;

    return (1);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   InvertMatrix
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static int InvertMatrix(VECTOR in[3], VECTOR  out[3])
{
  int i;
  DBL det;

  out[0][X] =   (in[1][Y] * in[2][Z] - in[1][Z] * in[2][Y]);
  out[1][X] = - (in[0][Y] * in[2][Z] - in[0][Z] * in[2][Y]);
  out[2][X] =   (in[0][Y] * in[1][Z] - in[0][Z] * in[1][Y]);

  out[0][Y] = - (in[1][X] * in[2][Z] - in[1][Z] * in[2][X]);
  out[1][Y] =   (in[0][X] * in[2][Z] - in[0][Z] * in[2][X]);
  out[2][Y] = - (in[0][X] * in[1][Z] - in[0][Z] * in[1][X]);

  out[0][Z] =   (in[1][X] * in[2][Y] - in[1][Y] * in[2][X]);
  out[1][Z] = - (in[0][X] * in[2][Y] - in[0][Y] * in[2][X]);
  out[2][Z] =   (in[0][X] * in[1][Y] - in[0][Y] * in[1][X]);

  det = in[0][X] * in[1][Y] * in[2][Z] +
        in[0][Y] * in[1][Z] * in[2][X] +
        in[0][Z] * in[1][X] * in[2][Y] -
        in[0][Z] * in[1][Y] * in[2][X] -
        in[0][X] * in[1][Z] * in[2][Y] -
        in[0][Y] * in[1][X] * in[2][Z];

  if (fabs(det) < BEZIER_EPSILON)
  {
    return (0);
  }

  det = 1.0 / det;

  for (i = 0; i < 3; i++)
  {
    VScaleEq(out[i], det)
  }

  return (1);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_subpatch
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static int intersect_subpatch(BICUBIC_PATCH *Shape, RAY *ray, VECTOR V1[3], DBL uu[3], DBL  vv[3], DBL  *Depth, VECTOR P, VECTOR  N, DBL *u, DBL  *v)
{
  DBL d, n, a, b, r;
  VECTOR B[3], IB[3], NN[3], Q, T1;

  VSub(B[0], V1[1], V1[0]);
  VSub(B[1], V1[2], V1[0]);

  VCross(B[2], B[0], B[1]);

  VDot(d, B[2], B[2]);

  if (d < BEZIER_EPSILON)
  {
    return (0);
  }

  d = 1.0 / sqrt(d);

  VScaleEq(B[2], d);

  /* Degenerate triangle. */

  if (!InvertMatrix(B, IB))
  {
    return (0);
  }

  VDot(d, ray->Direction, IB[2]);

  if (fabs(d) < BEZIER_EPSILON)
  {
    return (0);
  }

  VSub(Q, V1[0], ray->Initial);

  VDot(n, Q, IB[2]);

  *Depth = n / d;

  if (*Depth < BEZIER_TOLERANCE)
  {
    return (0);
  }

  VScale(T1, ray->Direction, *Depth);

  VAdd(P, ray->Initial, T1);

  VSub(Q, P, V1[0]);

  VDot(a, Q, IB[0]);
  VDot(b, Q, IB[1]);

  if ((a < 0.0) || (b < 0.0) || (a + b > 1.0))
  {
    return (0);
  }

  r = 1.0 - a - b;

  Make_Vector(N, 0.0, 0.0, 0.0);

  bezier_value((VECTOR(*)[4][4])&Shape->Control_Points, uu[0], vv[0], T1, NN[0]);
  bezier_value((VECTOR(*)[4][4])&Shape->Control_Points, uu[1], vv[1], T1, NN[1]);
  bezier_value((VECTOR(*)[4][4])&Shape->Control_Points, uu[2], vv[2], T1, NN[2]);

  VScale(T1, NN[0], r); VAddEq(N, T1);
  VScale(T1, NN[1], a); VAddEq(N, T1);
  VScale(T1, NN[2], b); VAddEq(N, T1);

  *u = r * uu[0] + a * uu[1] + b * uu[2];
  *v = r * vv[0] + a * vv[1] + b * vv[2];

  VDot(d, N, N);

  if (d > BEZIER_EPSILON)
  {
    d = 1.0 / sqrt(d);

    VScaleEq(N, d);
  }
  else
  {
    Make_Vector(N, 1, 0, 0);
  }

  return (1);
}



/*****************************************************************************
*
* FUNCTION
*
*   find_average
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Find a sphere that contains all of the points in the list "vectors".
*
* CHANGES
*
*   -
*
******************************************************************************/

static void find_average(int vector_count, VECTOR *vectors, VECTOR  center, DBL *radius)
{
  int i;
  DBL r0, r1, xc = 0, yc = 0, zc = 0;
  DBL x0, y0, z0;

  for (i = 0; i < vector_count; i++)
  {
    xc += vectors[i][X];
    yc += vectors[i][Y];
    zc += vectors[i][Z];
  }

  xc /= (DBL)vector_count;
  yc /= (DBL)vector_count;
  zc /= (DBL)vector_count;

  r0 = 0.0;

  for (i = 0; i < vector_count; i++)
  {
    x0 = vectors[i][X] - xc;
    y0 = vectors[i][Y] - yc;
    z0 = vectors[i][Z] - zc;

    r1 = x0 * x0 + y0 * y0 + z0 * z0;

    if (r1 > r0)
    {
      r0 = r1;
    }
  }

  Make_Vector(center, xc, yc, zc);

  *radius = r0;
}



/*****************************************************************************
*
* FUNCTION
*
*   spherical_bounds_check
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static int spherical_bounds_check(RAY *Ray, VECTOR center, DBL radius)
{
  DBL x, y, z, dist1, dist2;

  x = center[X] - Ray->Initial[X];
  y = center[Y] - Ray->Initial[Y];
  z = center[Z] - Ray->Initial[Z];

  dist1 = x * x + y * y + z * z;

  if (dist1 < radius)
  {
    /* ray starts inside sphere - assume it intersects. */

    return (1);
  }
  else
  {
    dist2 = x*Ray->Direction[X] + y*Ray->Direction[Y] + z*Ray->Direction[Z];

    dist2 *= dist2;

    if ((dist2 > 0) && (dist1 - dist2 < radius))
    {
      return (1);
    }
  }

  return (0);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_bounding_sphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Find a sphere that bounds all of the control points of a Bezier patch.
*   The values returned are: the center of the bounding sphere, and the
*   square of the radius of the bounding sphere.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void bezier_bounding_sphere(VECTOR (*Patch)[4][4], VECTOR  center, DBL *radius)
{
  int i, j;
  DBL r0, r1, xc = 0, yc = 0, zc = 0;
  DBL x0, y0, z0;

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      xc += (*Patch)[i][j][X];
      yc += (*Patch)[i][j][Y];
      zc += (*Patch)[i][j][Z];
    }
  }

  xc /= 16.0;
  yc /= 16.0;
  zc /= 16.0;

  r0 = 0.0;

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      x0 = (*Patch)[i][j][X] - xc;
      y0 = (*Patch)[i][j][Y] - yc;
      z0 = (*Patch)[i][j][Z] - zc;

      r1 = x0 * x0 + y0 * y0 + z0 * z0;

      if (r1 > r0)
      {
        r0 = r1;
      }
    }
  }

  Make_Vector(center, xc, yc, zc);

  *radius = r0;
}



/*****************************************************************************
*
* FUNCTION
*
*   Precompute_Patch_Values
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Precompute grid points and normals for a bezier patch.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Precompute_Patch_Values(BICUBIC_PATCH *Shape)
{
  int i, j;
  VECTOR Control_Points[16];
  VECTOR(*Patch_Ptr)[4][4] = (VECTOR(*)[4][4]) Shape->Control_Points;

  /* Calculate the bounding sphere for the entire patch. */

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      Assign_Vector(Control_Points[4*i + j], Shape->Control_Points[i][j]);
    }
  }

  find_average(16, Control_Points, Shape->Bounding_Sphere_Center, &Shape->Bounding_Sphere_Radius);

  if (Shape->Patch_Type != 0)
  {
    if (Shape->Node_Tree != NULL)
    {
      bezier_tree_deleter(Shape->Node_Tree);
    }

    Shape->Node_Tree = bezier_tree_builder(Shape, Patch_Ptr, 0.0, 1.0, 0.0, 1.0, 0);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   point_plane_distance
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Determine the distance from a point to a plane.
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL point_plane_distance(VECTOR p, VECTOR  n, DBL *d)
{
  DBL temp1, temp2;

  VDot(temp1, p, n);

  temp1 += *d;

  VLength(temp2, n);

  if (fabs(temp2) < EPSILON)
  {
    return (0.0);
  }

  temp1 /= temp2;

  return (temp1);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_subpatch_intersect
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static int bezier_subpatch_intersect(RAY *ray, BICUBIC_PATCH *Shape, VECTOR (*Patch)[4][4], DBL u0, DBL  u1, DBL  v0, DBL  v1, ISTACK *Depth_Stack)
{
  int cnt = 0;
  VECTOR V1[3];
  DBL u, v, Depth, uu[3], vv[3];
  VECTOR P, N;

  Assign_Vector(V1[0], (*Patch)[0][0]);
  Assign_Vector(V1[1], (*Patch)[0][3]);
  Assign_Vector(V1[2], (*Patch)[3][3]);

  uu[0] = u0; uu[1] = u0; uu[2] = u1;
  vv[0] = v0; vv[1] = v1; vv[2] = v1;

  if (intersect_subpatch(Shape, ray, V1, uu, vv, &Depth, P, N, &u, &v))
  {
    push_normal_entry(Depth, P, N, (OBJECT *)Shape, Depth_Stack);

    cnt++;
  }

  Assign_Vector(V1[1], V1[2]);
  Assign_Vector(V1[2], (*Patch)[3][0]);

  uu[1] = uu[2]; uu[2] = u1;
  vv[1] = vv[2]; vv[2] = v0;

  if (intersect_subpatch(Shape, ray, V1, uu, vv, &Depth, P, N, &u, &v))
  {
    push_normal_entry(Depth, P, N, (OBJECT *)Shape, Depth_Stack);

    cnt++;
  }

  return (cnt);
}




/*****************************************************************************
*
* FUNCTION
*
*   bezier_split_left_right
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static void bezier_split_left_right(VECTOR (*Patch)[4][4], VECTOR  (*Left_Patch)[4][4], VECTOR  (*Right_Patch)[4][4])
{
  int i, j;
  VECTOR Temp1[4], Temp2[4], Half;

  for (i = 0; i < 4; i++)
  {
    Assign_Vector(Temp1[0], (*Patch)[0][i]);

    VHalf(Temp1[1], (*Patch)[0][i], (*Patch)[1][i]);
    VHalf(Half, (*Patch)[1][i], (*Patch)[2][i]);
    VHalf(Temp1[2], Temp1[1], Half);
    VHalf(Temp2[2], (*Patch)[2][i], (*Patch)[3][i]);
    VHalf(Temp2[1], Half, Temp2[2]);
    VHalf(Temp1[3], Temp1[2], Temp2[1]);

    Assign_Vector(Temp2[0], Temp1[3]);
    Assign_Vector(Temp2[3], (*Patch)[3][i]);

    for (j = 0; j < 4; j++)
    {
      Assign_Vector((*Left_Patch)[j][i], Temp1[j]);
      Assign_Vector((*Right_Patch)[j][i], Temp2[j]);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_split_up_down
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static void bezier_split_up_down(VECTOR (*Patch)[4][4], VECTOR  (*Bottom_Patch)[4][4], VECTOR  (*Top_Patch)[4][4])
{
  int i, j;
  VECTOR Temp1[4], Temp2[4], Half;

  for (i = 0; i < 4; i++)
  {
    Assign_Vector(Temp1[0], (*Patch)[i][0]);

    VHalf(Temp1[1], (*Patch)[i][0], (*Patch)[i][1]);
    VHalf(Half, (*Patch)[i][1], (*Patch)[i][2]);
    VHalf(Temp1[2], Temp1[1], Half);
    VHalf(Temp2[2], (*Patch)[i][2], (*Patch)[i][3]);
    VHalf(Temp2[1], Half, Temp2[2]);
    VHalf(Temp1[3], Temp1[2], Temp2[1]);

    Assign_Vector(Temp2[0], Temp1[3]);
    Assign_Vector(Temp2[3], (*Patch)[i][3]);

    for (j = 0; j < 4; j++)
    {
      Assign_Vector((*Bottom_Patch)[i][j], Temp1[j]);
      Assign_Vector((*Top_Patch)[i][j]   , Temp2[j]);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   determine_subpatch_flatness
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   See how close to a plane a subpatch is, the patch must have at least
*   three distinct vertices. A negative result from this function indicates
*   that a degenerate value of some sort was encountered.
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL determine_subpatch_flatness(VECTOR (*Patch)[4][4])
{
  int i, j;
  DBL d, dist, temp1;
  VECTOR vertices[4], n, TempV;

  Assign_Vector(vertices[0], (*Patch)[0][0]);
  Assign_Vector(vertices[1], (*Patch)[0][3]);

  VSub(TempV, vertices[0], vertices[1]);

  VLength(temp1, TempV);

  if (fabs(temp1) < EPSILON)
  {
    /*
     * Degenerate in the V direction for U = 0. This is ok if the other
     * two corners are distinct from the lower left corner - I'm sure there
     * are cases where the corners coincide and the middle has good values,
     * but that is somewhat pathalogical and won't be considered.
     */

    Assign_Vector(vertices[1], (*Patch)[3][3]);

    VSub(TempV, vertices[0], vertices[1]);

    VLength(temp1, TempV);

    if (fabs(temp1) < EPSILON)
    {
      return (-1.0);
    }

    Assign_Vector(vertices[2], (*Patch)[3][0]);

    VSub(TempV, vertices[0], vertices[1]);

    VLength(temp1, TempV);

    if (fabs(temp1) < EPSILON)
    {
      return (-1.0);
    }

    VSub(TempV, vertices[1], vertices[2]);

    VLength(temp1, TempV);

    if (fabs(temp1) < EPSILON)
    {
      return (-1.0);
    }
  }
  else
  {
    Assign_Vector(vertices[2], (*Patch)[3][0]);

    VSub(TempV, vertices[0], vertices[1]);

    VLength(temp1, TempV);

    if (fabs(temp1) < EPSILON)
    {
      Assign_Vector(vertices[2], (*Patch)[3][3]);

      VSub(TempV, vertices[0], vertices[2]);

      VLength(temp1, TempV);

      if (fabs(temp1) < EPSILON)
      {
        return (-1.0);
      }

      VSub(TempV, vertices[1], vertices[2]);

      VLength(temp1, TempV);

      if (fabs(temp1) < EPSILON)
      {
        return (-1.0);
      }
    }
    else
    {
      VSub(TempV, vertices[1], vertices[2]);

      VLength(temp1, TempV);

      if (fabs(temp1) < EPSILON)
      {
        return (-1.0);
      }
    }
  }

  /*
   * Now that a good set of candidate points has been found,
   * find the plane equations for the patch.
   */

  if (subpatch_normal(vertices[0], vertices[1], vertices[2], n, &d))
  {
    /*
     * Step through all vertices and see what the maximum
     * distance from the plane happens to be.
     */

    dist = 0.0;

    for (i = 0; i < 4; i++)
    {
      for (j = 0; j < 4; j++)
      {
        temp1 = fabs(point_plane_distance(((*Patch)[i][j]), n, &d));

        if (temp1 > dist)
        {
          dist = temp1;
        }
      }
    }

    return (dist);
  }
  else
  {
/*
    Debug_Info("Subpatch normal failed in determine_subpatch_flatness\n");
*/

    return (-1.0);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   flat_enough
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static int flat_enough(BICUBIC_PATCH *Object, VECTOR (*Patch)[4][4])
{
  DBL Dist;

  Dist = determine_subpatch_flatness(Patch);

  if (Dist < 0.0)
  {
    return (0);
  }
  else
  {
    if (Dist < Object->Flatness_Value)
    {
      return (1);
    }
    else
    {
      return (0);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_subdivider
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static int bezier_subdivider(RAY *Ray, BICUBIC_PATCH *Object, VECTOR (*Patch)[4][4], DBL u0, DBL  u1, DBL  v0, DBL  v1,
int recursion_depth, ISTACK *Depth_Stack)
{
  int cnt = 0;
  DBL ut, vt, radius;
  VECTOR Lower_Left[4][4], Lower_Right[4][4];
  VECTOR Upper_Left[4][4], Upper_Right[4][4];
  VECTOR center;

  /*
   * Make sure the ray passes through a sphere bounding
   * the control points of the patch.
   */

  bezier_bounding_sphere(Patch, center, &radius);

  if (!spherical_bounds_check(Ray, center, radius))
  {
    return (0);
  }

  /*
   * If the patch is close to being flat, then just
   * perform a ray-plane intersection test.
   */

  if (flat_enough(Object, Patch))
  {
    return bezier_subpatch_intersect(Ray, Object, Patch, u0, u1, v0, v1, Depth_Stack);
  }

  if (recursion_depth >= Object->U_Steps)
  {
    if (recursion_depth >= Object->V_Steps)
    {
      return bezier_subpatch_intersect(Ray, Object, Patch, u0, u1, v0, v1, Depth_Stack);
    }
    else
    {
      bezier_split_up_down(Patch, (VECTOR(*)[4][4])Lower_Left, (VECTOR(*)[4][4])Upper_Left);

      vt = (v1 + v0) / 2.0;

      cnt += bezier_subdivider(Ray, Object, (VECTOR(*)[4][4])Lower_Left, u0, u1, v0, vt, recursion_depth + 1, Depth_Stack);
      cnt += bezier_subdivider(Ray, Object, (VECTOR(*)[4][4])Upper_Left, u0, u1, vt, v1, recursion_depth + 1, Depth_Stack);
    }
  }
  else
  {
    if (recursion_depth >= Object->V_Steps)
    {
      bezier_split_left_right(Patch, (VECTOR(*)[4][4])Lower_Left, (VECTOR(*)[4][4])Lower_Right);

      ut = (u1 + u0) / 2.0;

      cnt += bezier_subdivider(Ray, Object, (VECTOR(*)[4][4])Lower_Left, u0, ut, v0, v1, recursion_depth + 1, Depth_Stack);
      cnt += bezier_subdivider(Ray, Object, (VECTOR(*)[4][4])Lower_Right, ut, u1, v0, v1, recursion_depth + 1, Depth_Stack);
    }
    else
    {
      ut = (u1 + u0) / 2.0;
      vt = (v1 + v0) / 2.0;

      bezier_split_left_right(Patch, (VECTOR(*)[4][4])Lower_Left, (VECTOR(*)[4][4])Lower_Right);
      bezier_split_up_down((VECTOR(*)[4][4])Lower_Left, (VECTOR(*)[4][4])Lower_Left, (VECTOR(*)[4][4])Upper_Left) ;
      bezier_split_up_down((VECTOR(*)[4][4])Lower_Right, (VECTOR(*)[4][4])Lower_Right, (VECTOR(*)[4][4])Upper_Right);

      cnt += bezier_subdivider(Ray, Object, (VECTOR(*)[4][4])Lower_Left, u0, ut, v0, vt, recursion_depth + 1, Depth_Stack);
      cnt += bezier_subdivider(Ray, Object, (VECTOR(*)[4][4])Upper_Left, u0, ut, vt, v1, recursion_depth + 1, Depth_Stack);
      cnt += bezier_subdivider(Ray, Object, (VECTOR(*)[4][4])Lower_Right, ut, u1, v0, vt, recursion_depth + 1, Depth_Stack);
      cnt += bezier_subdivider(Ray, Object, (VECTOR(*)[4][4])Upper_Right, ut, u1, vt, v1, recursion_depth + 1, Depth_Stack);
    }
  }

  return (cnt);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_tree_deleter
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static void bezier_tree_deleter(BEZIER_NODE *Node)
{
  int i;
  BEZIER_CHILDREN *Children;

  /* If this is an interior node then continue the descent. */

  if (Node->Node_Type == BEZIER_INTERIOR_NODE)
  {
    Children = (BEZIER_CHILDREN *)Node->Data_Ptr;

    for (i = 0; i < Node->Count; i++)
    {
      bezier_tree_deleter(Children->Children[i]);
    }

    POV_FREE(Children);
  }
  else
  {
    if (Node->Node_Type == BEZIER_LEAF_NODE)
    {
      /* Free the memory used for the vertices. */

      POV_FREE(Node->Data_Ptr);
    }
  }

  /* Free the memory used for the node. */

  POV_FREE(Node);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_tree_walker
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static int bezier_tree_walker(RAY *Ray, BICUBIC_PATCH *Shape, BEZIER_NODE *Node, ISTACK *Depth_Stack)
{
  int i, cnt = 0;
  DBL Depth, u, v, uu[3], vv[3];
  VECTOR N, P, V1[3];
  BEZIER_CHILDREN *Children;
  BEZIER_VERTICES *Vertices;

  /*
   * Make sure the ray passes through a sphere bounding
   * the control points of the patch.
   */

  if (!spherical_bounds_check(Ray, Node->Center, Node->Radius_Squared))
  {
    return (0);
  }

  /*
   * If this is an interior node then continue the descent,
   * else do a check against the vertices.
   */

  if (Node->Node_Type == BEZIER_INTERIOR_NODE)
  {
    Children = (BEZIER_CHILDREN *)Node->Data_Ptr;

    for (i = 0; i < Node->Count; i++)
    {
      cnt += bezier_tree_walker(Ray, Shape, Children->Children[i], Depth_Stack);
    }
  }
  else if (Node->Node_Type == BEZIER_LEAF_NODE)
  {
    Vertices = (BEZIER_VERTICES *)Node->Data_Ptr;

    Assign_Vector(V1[0], Vertices->Vertices[0]);
    Assign_Vector(V1[1], Vertices->Vertices[1]);
    Assign_Vector(V1[2], Vertices->Vertices[2]);

    uu[0] = Vertices->uvbnds[0];
    uu[1] = Vertices->uvbnds[0];
    uu[2] = Vertices->uvbnds[1];
    vv[0] = Vertices->uvbnds[2];
    vv[1] = Vertices->uvbnds[3];
    vv[2] = Vertices->uvbnds[3];

    /*
     * Triangulate this subpatch, then check for
     * intersections in the triangles.
     */

    if (intersect_subpatch(Shape, Ray, V1, uu, vv, &Depth, P, N, &u, &v))
    {
      push_normal_entry(Depth, P, N, (OBJECT *)Shape, Depth_Stack);

      cnt++;
    }

    Assign_Vector(V1[1], V1[2]);
    Assign_Vector(V1[2], Vertices->Vertices[3]);

    uu[1] = uu[2]; uu[2] = Vertices->uvbnds[1];
    vv[1] = vv[2]; vv[2] = Vertices->uvbnds[2];

    if (intersect_subpatch(Shape, Ray, V1, uu, vv, &Depth, P, N, &u, &v))
    {
      push_normal_entry(Depth, P, N, (OBJECT *)Shape, Depth_Stack);

      cnt++;
    }
  }
  else
  {
    Error("Bad Node type in bezier_tree_walker().\n");
  }

  return (cnt);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_bicubic_patch0
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
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

static int intersect_bicubic_patch0(RAY *Ray, BICUBIC_PATCH *Shape, ISTACK *Depth_Stack)
{
  VECTOR(*Patch)[4][4] = (VECTOR(*)[4][4]) Shape->Control_Points;
  
  return (bezier_subdivider(Ray, Shape, Patch, 0.0, 1.0, 0.0, 1.0, 0, Depth_Stack));
}



/*****************************************************************************
*
* FUNCTION
*
*   All_Bicubic_Patch_Intersections
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static int All_Bicubic_Patch_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int Found, cnt = 0;

  Found = FALSE;

  Increase_Counter(stats[Ray_Bicubic_Tests]);

  switch (((BICUBIC_PATCH *)Object)->Patch_Type)
  {
    case 0:

      cnt = intersect_bicubic_patch0(Ray, ((BICUBIC_PATCH *)Object), Depth_Stack);

      break;

    case 1:

      cnt = bezier_tree_walker(Ray, (BICUBIC_PATCH *)Object, ((BICUBIC_PATCH *)Object)->Node_Tree, Depth_Stack);

      break;

    default:

      Error("Bad patch type in All_Bicubic_Patch_Intersections.\n");
  }
  
  if (cnt > 0)
  {
    Increase_Counter(stats[Ray_Bicubic_Tests_Succeeded]);
    
    Found = TRUE;
  }
  
  return (Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Bicubic_Patch
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   A patch is not a solid, so an inside test doesn't make sense.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int Inside_Bicubic_Patch(VECTOR IPoint, OBJECT *Object)
{
  return (0);
}



/*****************************************************************************
*
* FUNCTION
*
*   Bicubic_Patch_Normal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static void Bicubic_Patch_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  /* Use preocmputed normal. */

  Assign_Vector(Result, Inter->INormal);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Bicubic_Patch
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static void Translate_Bicubic_Patch(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  int i, j;
  BICUBIC_PATCH *Patch = (BICUBIC_PATCH *) Object;

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      VAdd(Patch->Control_Points[i][j], Patch->Control_Points[i][j], Vector)
    }
  }

  Precompute_Patch_Values(Patch);

  Compute_Bicubic_Patch_BBox(Patch);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Bicubic_Patch
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static void Rotate_Bicubic_Patch(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Bicubic_Patch(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Bicubic_Patch
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static void Scale_Bicubic_Patch(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  int i, j;
  BICUBIC_PATCH *Patch = (BICUBIC_PATCH *) Object;

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      VEvaluate(Patch->Control_Points[i][j], Patch->Control_Points[i][j], Vector);
    }
  }

  Precompute_Patch_Values(Patch);

  Compute_Bicubic_Patch_BBox(Patch);
}




/*****************************************************************************
*
* FUNCTION
*
*   Transform_Bicubic_Patch
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static void Transform_Bicubic_Patch(OBJECT *Object, TRANSFORM *Trans)
{
  int i, j;
  BICUBIC_PATCH *Patch = (BICUBIC_PATCH *) Object;
  
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      MTransPoint(Patch->Control_Points[i][j], Patch->Control_Points[i][j], Trans);
    }
  }
  
  Precompute_Patch_Values(Patch);
  
  Compute_Bicubic_Patch_BBox(Patch);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Bicubic_Patch
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Inversion of a patch really doesn't make sense.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void Invert_Bicubic_Patch(OBJECT *Object)
{
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Bicubic_Patch
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

BICUBIC_PATCH *Create_Bicubic_Patch()
{
  BICUBIC_PATCH *New;
  
  New = (BICUBIC_PATCH *)POV_MALLOC(sizeof (BICUBIC_PATCH), "bicubic patch");
  
  INIT_OBJECT_FIELDS(New, BICUBIC_PATCH_OBJECT, &Bicubic_Patch_Methods)
    
    New->Patch_Type = - 1;
  
  New->U_Steps = 0;
  New->V_Steps = 0;
  
  New->Flatness_Value = 0.0;
  
  New->Node_Tree = NULL;
  
  /*
   * NOTE: Control_Points[4][4] is initialized in Parse_Bicubic_Patch.
   * Bounding_Sphere_Center,Bounding_Sphere_Radius, Normal_Vector[], and
   * IPoint[] are initialized in Precompute_Patch_Values.
   */
  
  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Bicubic_Patch
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static BICUBIC_PATCH *Copy_Bicubic_Patch(OBJECT *Object)
{
  int i, j;
  BICUBIC_PATCH *New;
  
  New = Create_Bicubic_Patch();
  
  /* Do not do *New = *Old so that Precompute works right */
  
  New->Patch_Type = ((BICUBIC_PATCH *)Object)->Patch_Type;

  New->U_Steps = ((BICUBIC_PATCH *)Object)->U_Steps;
  New->V_Steps = ((BICUBIC_PATCH *)Object)->V_Steps;
  
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      Assign_Vector(New->Control_Points[i][j], ((BICUBIC_PATCH *)Object)->Control_Points[i][j]);
    }
  }
  
  New->Flatness_Value = ((BICUBIC_PATCH *)Object)->Flatness_Value;
  
  Precompute_Patch_Values(New);
  
  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Bicubic_Patch
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
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

static void Destroy_Bicubic_Patch(OBJECT *Object)
{
  BICUBIC_PATCH *Patch;
  
  Patch = (BICUBIC_PATCH *)Object;
  
  if (Patch->Patch_Type != 0)
  {
    if (Patch->Node_Tree != NULL)
    {
      bezier_tree_deleter(Patch->Node_Tree);
    }
  }
  
  POV_FREE(Patch);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Bicubic_Patch_BBox
*
* INPUT
*
*   Bicubic_Patch - Bicubic patch
*   
* OUTPUT
*
*   Bicubic_Patch
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a bicubic patch.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Compute_Bicubic_Patch_BBox(BICUBIC_PATCH *Bicubic_Patch)
{
  int i, j;
  VECTOR Min, Max;

  Make_Vector(Min, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
  Make_Vector(Max, -BOUND_HUGE, -BOUND_HUGE, -BOUND_HUGE);

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      Min[X] = min(Min[X], Bicubic_Patch->Control_Points[i][j][X]);
      Min[Y] = min(Min[Y], Bicubic_Patch->Control_Points[i][j][Y]);
      Min[Z] = min(Min[Z], Bicubic_Patch->Control_Points[i][j][Z]);
      Max[X] = max(Max[X], Bicubic_Patch->Control_Points[i][j][X]);
      Max[Y] = max(Max[Y], Bicubic_Patch->Control_Points[i][j][Y]);
      Max[Z] = max(Max[Z], Bicubic_Patch->Control_Points[i][j][Z]);
    }
  }
  
  Make_BBox_from_min_max(Bicubic_Patch->BBox, Min, Max);
}

