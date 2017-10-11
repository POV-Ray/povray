/****************************************************************************
*                mesh.c
*
*  This module implements primitives for mesh objects.
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
*  Syntax:
*
*    mesh
*    {
*      triangle { <CORNER1>, <CORNER2>, <CORNER3>, texture { NAME } }
*      smooth_triangle { <CORNER1>, <NORMAL1>, <CORNER2>, <NORMAL2>, <CORNER3>, <NORMAL3>, texture { NAME } }
*      ...
*      [ hierarchy FLAG ]
*    }
*
*  ---
*
*  Feb 1995 : Creation. [DB]
*
*****************************************************************************/

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "bbox.h"
#include "matrices.h"
#include "objects.h"
#include "mesh.h"
#include "texture.h"
#include "povray.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define DEPTH_TOLERANCE 1e-6

#define max3_coordinate(x,y,z) ((x > y) ? ((x > z) ? X : Z) : ((y > z) ? Y : Z))

#define HASH_SIZE 1000

#define INITIAL_NUMBER_OF_ENTRIES 256


/*****************************************************************************
* Local typedefs
******************************************************************************/

typedef struct Hash_Table_Struct HASH_TABLE;

struct Hash_Table_Struct
{
  int Index;
  SNGL_VECT P;
  HASH_TABLE *Next;
};



/*****************************************************************************
* Static functions
******************************************************************************/

static int Intersect_Mesh  (RAY *Ray, MESH *Mesh, ISTACK *Depth_Stack);
static int All_Mesh_Intersections  (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int Inside_Mesh  (VECTOR IPoint, OBJECT *Object);
static void Mesh_Normal  (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static MESH *Copy_Mesh  (OBJECT *Object);
static void Translate_Mesh  (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Mesh  (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Mesh  (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Mesh  (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Mesh  (OBJECT *Object);
static void Destroy_Mesh  (OBJECT *Object);

static void compute_smooth_triangle (MESH_TRIANGLE *Triangle, VECTOR P1, VECTOR P2, VECTOR P3);
static int intersect_mesh_triangle (RAY *Ray, MESH *Mesh, MESH_TRIANGLE *Triangle, DBL *Depth);
static int test_hit (MESH_TRIANGLE *Triangle, MESH *Mesh, RAY *Ray, DBL Depth, ISTACK *Depth_Stack);
static void smooth_mesh_normal (MESH *Mesh, VECTOR Result, MESH_TRIANGLE *Triangle, VECTOR IPoint);
static void get_triangle_bbox (MESH *Mesh, MESH_TRIANGLE *Triangle, BBOX *BBox);

static int intersect_bbox_tree (MESH *Mesh, RAY *Ray, RAY *Orig_Ray, DBL len, ISTACK *Depth_Stack);

static void get_triangle_vertices (MESH *Mesh, MESH_TRIANGLE *Triangle, VECTOR P1, VECTOR P2, VECTOR P3);
static void get_triangle_normals (MESH *Mesh, MESH_TRIANGLE *Triangle, VECTOR N1, VECTOR N2, VECTOR N3);

static int mesh_hash (HASH_TABLE **Hash_Table,
  int *Number, int *Max, SNGL_VECT **Elements, VECTOR aPoint);



/*****************************************************************************
* Local variables
******************************************************************************/

METHODS Mesh_Methods =
{
  All_Mesh_Intersections,
  Inside_Mesh, Mesh_Normal,
  (COPY_METHOD)Copy_Mesh,
  Translate_Mesh, Rotate_Mesh,
  Scale_Mesh, Transform_Mesh, Invert_Mesh, Destroy_Mesh
};

static HASH_TABLE **Vertex_Hash_Table, **Normal_Hash_Table;

static PRIORITY_QUEUE *Mesh_Queue;



/*****************************************************************************
*
* FUNCTION
*
*   All_Mesh_Intersections
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static int All_Mesh_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  Increase_Counter(stats[Ray_Mesh_Tests]);

  if (Intersect_Mesh(Ray, (MESH *)Object, Depth_Stack))
  {
    Increase_Counter(stats[Ray_Mesh_Tests_Succeeded]);

    return(TRUE);
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Mesh
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static int Intersect_Mesh(RAY *Ray, MESH *Mesh, ISTACK *Depth_Stack)
{
  int i;
  unsigned found;
  DBL len, t;
  RAY New_Ray;

  /* Transform the ray into mesh space. */

  if (Mesh->Trans != NULL)
  {
    MInvTransPoint(New_Ray.Initial, Ray->Initial, Mesh->Trans);
    MInvTransDirection(New_Ray.Direction, Ray->Direction, Mesh->Trans);

    VLength(len, New_Ray.Direction);
    VInverseScaleEq(New_Ray.Direction, len);
  }
  else
  {
    New_Ray = *Ray;

    len = 1.0;
  }

  found = FALSE;

  if (Mesh->Data->Tree == NULL)
  {
    /* There's no bounding hierarchy so just step through all elements. */

    for (i = 0; i < Mesh->Data->Number_Of_Triangles; i++)
    {
      if (intersect_mesh_triangle(&New_Ray, Mesh, &Mesh->Data->Triangles[i], &t))
      {
        if (test_hit(&Mesh->Data->Triangles[i], Mesh, Ray, t / len, Depth_Stack))
        {
          found = TRUE;
        }
      }
    }
  }
  else
  {
    /* Use the mesh's bounding hierarchy. */

    return(intersect_bbox_tree(Mesh, &New_Ray, Ray, len, Depth_Stack));
  }

  return(found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Mesh
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static int Inside_Mesh(VECTOR IPoint, OBJECT *Object)
{
  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Mesh_Normal
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
*   Return the normalized normal in the given point.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void Mesh_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  VECTOR IPoint;
  MESH_TRIANGLE *Triangle;
  MESH *Mesh = (MESH *)Object;

  Triangle = (MESH_TRIANGLE *)Inter->Pointer;

  if (Triangle->Smooth)
  {
    if (Mesh->Trans != NULL)
    {
      MInvTransPoint(IPoint, Inter->IPoint, Mesh->Trans);
    }
    else
    {
      Assign_Vector(IPoint, Inter->IPoint);
    }

    smooth_mesh_normal(Mesh, Result, Triangle, IPoint);
  
    if (Mesh->Trans != NULL)
    {
      MTransNormal(Result, Result, Mesh->Trans);
    }

    VNormalize(Result, Result);
  }
  else
  {
    Assign_SNGL_Vect(Result, Mesh->Data->Normals[Triangle->Normal_Ind]);

    if (Mesh->Trans != NULL)
    {
      MTransNormal(Result, Result, Mesh->Trans);

      VNormalize(Result, Result);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   smooth_mesh_normal
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
*   Remove the un-normalized normal of a smoothed triangle.
*
* CHANGES
*
*   Feb 1995 : Creation. (Derived from TRIANGLE.C)
*
******************************************************************************/

static void smooth_mesh_normal(MESH *Mesh, VECTOR Result, MESH_TRIANGLE *Triangle, VECTOR  IPoint)
{
  int axis;
  DBL u, v;
  DBL k1, k2, k3;
  VECTOR PIMinusP1, N1, N2, N3;

  get_triangle_normals(Mesh, Triangle, N1, N2, N3);

  VSub(PIMinusP1, IPoint, Mesh->Data->Vertices[Triangle->P1]);

  VDot(u, PIMinusP1, Triangle->Perp);

  if (u < EPSILON)
  {
    Assign_Vector(Result, N1);
  }
  else
  {
    axis = Triangle->vAxis;

    k1 = Mesh->Data->Vertices[Triangle->P1][axis];
    k2 = Mesh->Data->Vertices[Triangle->P2][axis];
    k3 = Mesh->Data->Vertices[Triangle->P3][axis];

    v = (PIMinusP1[axis] / u + k1 - k2) / (k3 - k2);

    Result[X] = N1[X] + u * (N2[X] - N1[X] + v * (N3[X] - N2[X]));
    Result[Y] = N1[Y] + u * (N2[Y] - N1[Y] + v * (N3[Y] - N2[Y]));
    Result[Z] = N1[Z] + u * (N2[Z] - N1[Z] + v * (N3[Z] - N2[Z]));
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Mesh
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void Translate_Mesh(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Mesh(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Mesh
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void Rotate_Mesh(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Mesh(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Mesh
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void Scale_Mesh(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Mesh(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transfrom_Mesh
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void Transform_Mesh(OBJECT *Object, TRANSFORM *Trans)
{
  int i;
  if (((MESH *)Object)->Trans == NULL)
  {
    ((MESH *)Object)->Trans = Create_Transform();
  }

  Recompute_BBox(&Object->BBox, Trans);

  Compose_Transforms(((MESH *)Object)->Trans, Trans);

  for (i=0; i<((MESH *)Object)->Data->Number_Of_Textures; i++)
  {
    Transform_Textures(((MESH *)Object)->Data->Textures[i], Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Mesh
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void Invert_Mesh(OBJECT *Object)
{
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Mesh
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

MESH *Create_Mesh()
{
  MESH *New;

  New = (MESH *)POV_MALLOC(sizeof(MESH), "mesh");

  INIT_OBJECT_FIELDS(New,MESH_OBJECT,&Mesh_Methods)

  Set_Flag(New, HIERARCHY_FLAG);

  New->Trans = NULL;

  New->Data = NULL;

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Mesh
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
*   Copy a mesh.
*
*   NOTE: The components are not copied, only the number of references is
*         counted, so that Destroy_Mesh() knows if they can be destroyed.
*
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static MESH *Copy_Mesh(OBJECT *Object)
{
  MESH *New;

  New = Create_Mesh();

  /* Copy mesh. */

  *New = *((MESH *)Object);
  
  New->Trans = Copy_Transform(New->Trans);
  
  New->Data->References++;

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Mesh
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void Destroy_Mesh(OBJECT *Object)
{
  int i;
  MESH *Mesh = (MESH *)Object;

  Destroy_Transform(Mesh->Trans);

  if (--(Mesh->Data->References) == 0)
  {
    Destroy_BBox_Tree(Mesh->Data->Tree);

    if (Mesh->Data->Normals != NULL)
    {
      POV_FREE(Mesh->Data->Normals);
    }

    if (Mesh->Data->Vertices != NULL)
    {
      POV_FREE(Mesh->Data->Vertices);
    }

    if (Mesh->Data->Triangles != NULL)
    {
      POV_FREE(Mesh->Data->Triangles);
    }

    if (Mesh->Data->Textures != NULL)
    {
      for (i = 0; i < Mesh->Data->Number_Of_Textures; i++)
      {
        Destroy_Textures(Mesh->Data->Textures[i]);
      }

      POV_FREE(Mesh->Data->Textures);
    }

    POV_FREE(Mesh->Data);
  }

  POV_FREE(Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Mesh_BBox
*
* INPUT
*
*   Mesh - Mesh
*   
* OUTPUT
*
*   Mesh
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a triangle.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

void Compute_Mesh_BBox(MESH *Mesh)
{
  int i;
  VECTOR P1, P2, P3;
  VECTOR mins, maxs;

  Make_Vector(mins, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
  Make_Vector(maxs, -BOUND_HUGE, -BOUND_HUGE, -BOUND_HUGE);

  for (i = 0; i < Mesh->Data->Number_Of_Triangles; i++)
  {
    get_triangle_vertices(Mesh, &Mesh->Data->Triangles[i], P1, P2, P3);

    mins[X] = min(mins[X], min3(P1[X], P2[X], P3[X]));
    mins[Y] = min(mins[Y], min3(P1[Y], P2[Y], P3[Y]));
    mins[Z] = min(mins[Z], min3(P1[Z], P2[Z], P3[Z]));

    maxs[X] = max(maxs[X], max3(P1[X], P2[X], P3[X]));
    maxs[Y] = max(maxs[Y], max3(P1[Y], P2[Y], P3[Y]));
    maxs[Z] = max(maxs[Z], max3(P1[Z], P2[Z], P3[Z]));
  }

  Make_BBox_from_min_max(Mesh->BBox, mins, maxs);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Mesh
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

int Compute_Mesh_Triangle(MESH_TRIANGLE *Triangle, int Smooth, VECTOR P1, VECTOR  P2, VECTOR  P3, VECTOR  S_Normal)
{
  int temp, swap;
  DBL x, y, z;
  VECTOR V1, V2, T1;
  DBL Length;

  VSub(V1, P2, P1);
  VSub(V2, P3, P1);

  VCross(S_Normal, V2, V1);

  VLength(Length, S_Normal);

  /* Set up a flag so we can ignore degenerate triangles */

  if (Length == 0.0)
  {
    return(FALSE);
  }

  /* Normalize the normal vector. */

  VInverseScaleEq(S_Normal, Length);

  VDot(Triangle->Distance, S_Normal, P1);

  Triangle->Distance *= -1.0;

  /* Find triangle's dominant axis. */

  x = fabs(S_Normal[X]);
  y = fabs(S_Normal[Y]);
  z = fabs(S_Normal[Z]);

  Triangle->Dominant_Axis = max3_coordinate(x, y, z);

  swap = FALSE;

  switch (Triangle->Dominant_Axis)
  {
    case X:

      if ((P2[Y] - P3[Y])*(P2[Z] - P1[Z]) < (P2[Z] - P3[Z])*(P2[Y] - P1[Y]))
      {
        swap = TRUE;
      }

      break;

    case Y:

      if ((P2[X] - P3[X])*(P2[Z] - P1[Z]) < (P2[Z] - P3[Z])*(P2[X] - P1[X]))
      {
        swap = TRUE;
      }

      break;

    case Z:

      if ((P2[X] - P3[X])*(P2[Y] - P1[Y]) < (P2[Y] - P3[Y])*(P2[X] - P1[X]))
      {
        swap = TRUE;
      }

      break;
  }

  if (swap)
  {
    temp = Triangle->P2;
    Triangle->P2 = Triangle->P1;
    Triangle->P1 = temp;

    Assign_Vector(T1, P1);
    Assign_Vector(P1, P2);
    Assign_Vector(P2, T1);

    if (Smooth)
    {
      temp = Triangle->N2;
      Triangle->N2 = Triangle->N1;
      Triangle->N1 = temp;
    }
  }

  if (Smooth)
  {
    compute_smooth_triangle(Triangle, P1, P2, P3);
  }

  return(TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
*   compute_smooth_triangle
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void compute_smooth_triangle(MESH_TRIANGLE *Triangle, VECTOR P1, VECTOR  P2, VECTOR  P3)
{
  VECTOR P3MinusP2, VTemp1, VTemp2;
  DBL x, y, z, uDenominator, Proj;

  Triangle->Smooth = TRUE;

  VSub(P3MinusP2, P3, P2);

  x = fabs(P3MinusP2[X]);
  y = fabs(P3MinusP2[Y]);
  z = fabs(P3MinusP2[Z]);

  Triangle->vAxis = max3_coordinate(x, y, z);

  VSub(VTemp1, P2, P3);

  VNormalize(VTemp1, VTemp1);

  VSub(VTemp2, P1, P3);

  VDot(Proj, VTemp2, VTemp1);

  VScaleEq(VTemp1, Proj);

  VSub(Triangle->Perp, VTemp1, VTemp2);

  VNormalize(Triangle->Perp, Triangle->Perp);

  VDot(uDenominator, VTemp2, Triangle->Perp);

  VInverseScaleEq(Triangle->Perp, -uDenominator);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_mesh_triangle
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static int intersect_mesh_triangle(RAY *Ray, MESH *Mesh, MESH_TRIANGLE *Triangle, DBL *Depth)
{
  DBL NormalDotOrigin, NormalDotDirection;
  DBL s, t;
  VECTOR P1, P2, P3, S_Normal;

  Assign_SNGL_Vect(S_Normal, Mesh->Data->Normals[Triangle->Normal_Ind]);

  VDot(NormalDotDirection, S_Normal, Ray->Direction);

  if (fabs(NormalDotDirection) < EPSILON)
  {
    return(FALSE);
  }

  VDot(NormalDotOrigin, S_Normal, Ray->Initial);

  *Depth = -(Triangle->Distance + NormalDotOrigin) / NormalDotDirection;

  if ((*Depth < DEPTH_TOLERANCE) || (*Depth > Max_Distance))
  {
    return(FALSE);
  }

  get_triangle_vertices(Mesh, Triangle, P1, P2, P3);

  switch (Triangle->Dominant_Axis)
  {
    case X:

      s = Ray->Initial[Y] + *Depth * Ray->Direction[Y];
      t = Ray->Initial[Z] + *Depth * Ray->Direction[Z];

      if ((P2[Y] - s) * (P2[Z] - P1[Z]) < (P2[Z] - t) * (P2[Y] - P1[Y]))
      {
        return(FALSE);
      }

      if ((P3[Y] - s) * (P3[Z] - P2[Z]) < (P3[Z] - t) * (P3[Y] - P2[Y]))
      {
        return(FALSE);
      }

      if ((P1[Y] - s) * (P1[Z] - P3[Z]) < (P1[Z] - t) * (P1[Y] - P3[Y]))
      {
        return(FALSE);
      }

      return(TRUE);

    case Y:

      s = Ray->Initial[X] + *Depth * Ray->Direction[X];
      t = Ray->Initial[Z] + *Depth * Ray->Direction[Z];

      if ((P2[X] - s) * (P2[Z] - P1[Z]) < (P2[Z] - t) * (P2[X] - P1[X]))
      {
        return(FALSE);
      }

      if ((P3[X] - s) * (P3[Z] - P2[Z]) < (P3[Z] - t) * (P3[X] - P2[X]))
      {
        return(FALSE);
      }

      if ((P1[X] - s) * (P1[Z] - P3[Z]) < (P1[Z] - t) * (P1[X] - P3[X]))
      {
        return(FALSE);
      }

      return(TRUE);

    case Z:

      s = Ray->Initial[X] + *Depth * Ray->Direction[X];
      t = Ray->Initial[Y] + *Depth * Ray->Direction[Y];

      if ((P2[X] - s) * (P2[Y] - P1[Y]) < (P2[Y] - t) * (P2[X] - P1[X]))
      {
        return(FALSE);
      }

      if ((P3[X] - s) * (P3[Y] - P2[Y]) < (P3[Y] - t) * (P3[X] - P2[X]))
      {
        return(FALSE);
      }

      if ((P1[X] - s) * (P1[Y] - P3[Y]) < (P1[Y] - t) * (P1[X] - P3[X]))
      {
        return(FALSE);
      }

      return(TRUE);
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   test_hit
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
*   Test if a hit is valid and push if on the intersection depth.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static int test_hit(MESH_TRIANGLE *Triangle, MESH *Mesh, RAY *Ray, DBL Depth, ISTACK *Depth_Stack)
{
  VECTOR IPoint;
  OBJECT *Object = (OBJECT *)Mesh;

  VEvaluateRay(IPoint, Ray->Initial, Depth, Ray->Direction);

  if (Point_In_Clip(IPoint, Object->Clip))
  {
    push_entry_pointer(Depth, IPoint, Object, Triangle, Depth_Stack);

    return(TRUE);
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Init_Mesh_Triangle
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

void Init_Mesh_Triangle(MESH_TRIANGLE *Triangle)
{
  Triangle->Smooth = FALSE;

  Triangle->Dominant_Axis = 0;
  Triangle->vAxis         = 0;

  Triangle->P1 =
  Triangle->P2 =
  Triangle->P3 = -1;

  Triangle->Normal_Ind = -1;

  Triangle->Texture = -1;

  Triangle->N1 =
  Triangle->N2 =
  Triangle->N3 = -1;

  Make_Vector(Triangle->Perp, 0.0, 0.0, 0.0);

  Triangle->Distance = 0.0;
}



/*****************************************************************************
*
* FUNCTION
*
*   get_triangle_bbox
*
* INPUT
*
*   Triangle - Pointer to triangle
*   
* OUTPUT
*
*   BBox     - Bounding box
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a triangle.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

static void get_triangle_bbox(MESH *Mesh, MESH_TRIANGLE *Triangle, BBOX *BBox)
{
  VECTOR P1, P2, P3;
  VECTOR Min, Max;

  get_triangle_vertices(Mesh, Triangle, P1, P2, P3);

  Min[X] = min3(P1[X], P2[X], P3[X]);
  Min[Y] = min3(P1[Y], P2[Y], P3[Y]);
  Min[Z] = min3(P1[Z], P2[Z], P3[Z]);

  Max[X] = max3(P1[X], P2[X], P3[X]);
  Max[Y] = max3(P1[Y], P2[Y], P3[Y]);
  Max[Z] = max3(P1[Z], P2[Z], P3[Z]);

  Make_BBox_from_min_max(*BBox, Min, Max);
}



/*****************************************************************************
*
* FUNCTION
*
*   Build_Mesh_BBox_Tree
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
*   Create the bounding box hierarchy.
*
* CHANGES
*
*   Feb 1995 : Creation. (Derived from the bounding slab creation code)
*
******************************************************************************/

void Build_Mesh_BBox_Tree(MESH *Mesh)
{
  int i, nElem, maxelements;
  BBOX_TREE **Triangles;

  if (!Test_Flag(Mesh, HIERARCHY_FLAG))
  {
    return;
  }

  nElem = (int)Mesh->Data->Number_Of_Triangles;

  maxelements = 2 * nElem;

  /* Now allocate an array to hold references to these elements. */

  Triangles = (BBOX_TREE **)POV_MALLOC(maxelements*sizeof(BBOX_TREE *), "mesh bbox tree");

  /* Init list with mesh elements. */

  for (i = 0; i < nElem; i++)
  {
    Triangles[i] = (BBOX_TREE *)POV_MALLOC(sizeof(BBOX_TREE), "mesh bbox tree");

    Triangles[i]->Infinite = FALSE;
    Triangles[i]->Entries  = 0;
    Triangles[i]->Node     = (BBOX_TREE **)&Mesh->Data->Triangles[i];

    get_triangle_bbox(Mesh, &Mesh->Data->Triangles[i], &Triangles[i]->BBox);
  }

  Build_BBox_Tree(&Mesh->Data->Tree, nElem, Triangles, 0, NULL);

  /* Get rid of the Triangles array. */

  POV_FREE(Triangles);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_bbox_tree
*
* INPUT
*
*   Mesh     - Mesh object
*   Ray      - Current ray
*   Orig_Ray - Original, untransformed ray
*   len      - Length of the transformed ray direction
*   
* OUTPUT
*
*   Depth_Stack - Stack of intersections
*   
* RETURNS
*
*   int - TRUE if an intersection was found
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Intersect a ray with the bounding box tree of a mesh.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static int intersect_bbox_tree(MESH *Mesh, RAY *Ray, RAY  *Orig_Ray, DBL len, ISTACK *Depth_Stack)
{
  int i, found;
  DBL Best, Depth;
  RAYINFO rayinfo;
  BBOX_TREE *Node, *Root;

  /* Create the direction vectors for this ray. */

  Create_Rayinfo(Ray, &rayinfo);

  /* Start with an empty priority queue. */

  found = 0;

  Mesh_Queue->QSize = 0;

  Best = BOUND_HUGE;

#ifdef BBOX_EXTRA_STATS
  Increase_Counter(stats[totalQueueResets]);
#endif

  /* Check top node. */

  Root = Mesh->Data->Tree;

  /* Set the root object infinite to avoid a test. */

  Check_And_Enqueue(Mesh_Queue, Root, &Root->BBox, &rayinfo);

  /* Check elements in the priority queue. */

  while (Mesh_Queue->QSize > 0)
  {
    Priority_Queue_Delete(Mesh_Queue, &Depth, &Node);

    /*
     * If current intersection is larger than the best intersection found
     * so far our task is finished, because all other bounding boxes in
     * the priority queue are further away.
     */

    if (Depth > Best)
    {
      break;
    }

    /* Check current node. */

    if (Node->Entries)
    {
      /* This is a node containing leaves to be checked. */

      for (i = 0; i < Node->Entries; i++)
      {
        Check_And_Enqueue(Mesh_Queue, Node->Node[i], &Node->Node[i]->BBox, &rayinfo);
      }
    }
    else
    {
      /* This is a leaf so test the contained triangle. */

      if (intersect_mesh_triangle(Ray, Mesh, (MESH_TRIANGLE *)Node->Node, &Depth))
      {
        if (test_hit((MESH_TRIANGLE *)Node->Node, Mesh, Orig_Ray, Depth / len, Depth_Stack))
        {
          found = TRUE;

          Best = Depth;
        }
      }
    }
  }

  return(found);
}



/*****************************************************************************
*
* FUNCTION
*
*   mesh_hash
*
* INPUT
*
*   aPoint - Normal/Vertex to store
*   
* OUTPUT
*
*   Hash_Table - Normal/Vertex hash table
*   Number     - Number of normals/vertices
*   Max        - Max. number of normals/vertices
*   Elements   - List of normals/vertices
*   
* RETURNS
*
*   int - Index of normal/vertex into the normals/vertices list
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Try to locate a triangle normal/vertex in the normal/vertex list.
*   If the vertex is not found its stored in the normal/vertex list.
*
* CHANGES
*
*   Feb 1995 : Creation. (With help from Steve Anger's RAW2POV code)
*
******************************************************************************/

static int mesh_hash(HASH_TABLE **Hash_Table, int *Number, int  *Max, SNGL_VECT **Elements, VECTOR aPoint)
{
  int hash;
  SNGL_VECT D, P;
  HASH_TABLE *p;

  Assign_SNGL_Vect(P, aPoint);

  /* Get hash value. */

  hash = (unsigned)((int)(326.0*P[X])^(int)(694.7*P[Y])^(int)(1423.6*P[Z])) % HASH_SIZE;

  /* Try to find normal/vertex. */

  for (p = Hash_Table[hash]; p != NULL; p = p->Next)
  {
    VSub(D, p->P, P);

    if ((fabs(D[X]) < EPSILON) && (fabs(D[Y]) < EPSILON) && (fabs(D[Z]) < EPSILON))
    {
      break;
    }
  }

  if ((p != NULL) && (p->Index >= 0))
  {
    return(p->Index);
  }

  /* Add new normal/vertex to the list and hash table. */

  if ((*Number) >= (*Max))
  {
    if ((*Max) >= INT_MAX/2)
    {
      Error("Too many normals/vertices in mesh.\n");
    }

    (*Max) *= 2;

    (*Elements) = (SNGL_VECT *)POV_REALLOC((*Elements), (*Max)*sizeof(SNGL_VECT), "mesh data");
  }

  Assign_SNGL_Vect((*Elements)[*Number], P);

  p = (HASH_TABLE *)POV_MALLOC(sizeof(HASH_TABLE), "mesh data");

  Assign_SNGL_Vect(p->P, P);

  p->Index = *Number;

  p->Next = Hash_Table[hash];

  Hash_Table[hash] = p;

  return((*Number)++);
}



/*****************************************************************************
*
* FUNCTION
*
*   Mesh_Hash_Vertex
*
* INPUT
*
*   Vertex - Vertex to store
*   
* OUTPUT
*
*   Number_Of_Vertices - Number of vertices
*   Max_Vertices       - Max. number of vertices
*   Vertices           - List of vertices
*   
* RETURNS
*
*   int - Index of vertex into the vertices list
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Try to locate a triangle vertex in the vertex list.
*   If the vertex is not found its stored in the vertex list.
*
* CHANGES
*
*   Feb 1995 : Creation. (With help from Steve Anger's RAW2POV code)
*
******************************************************************************/

int Mesh_Hash_Vertex(int *Number_Of_Vertices, int  *Max_Vertices, SNGL_VECT **Vertices, VECTOR Vertex)
{
  return(mesh_hash(Vertex_Hash_Table, Number_Of_Vertices, Max_Vertices, Vertices, Vertex));
}



/*****************************************************************************
*
* FUNCTION
*
*   Mesh_Hash_Normal
*
* INPUT
*
*   Normal - Normal to store
*   
* OUTPUT
*
*   Number_Of_Normals - Number of normals
*   Max_Normals       - Max. number of normals
*   Normals           - List of normals
*   
* RETURNS
*
*   int - Index of normal into the normals list
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Try to locate a triangle normal in the normal list.
*   If the normal is not found its stored in the normal list.
*
* CHANGES
*
*   Feb 1995 : Creation. (With help from Steve Anger's RAW2POV code)
*
******************************************************************************/

int Mesh_Hash_Normal(int *Number_Of_Normals, int  *Max_Normals, SNGL_VECT **Normals, VECTOR S_Normal)
{
  return(mesh_hash(Normal_Hash_Table, Number_Of_Normals, Max_Normals, Normals, S_Normal));
}



/*****************************************************************************
*
* FUNCTION
*
*   Mesh_Hash_Texture
*
* INPUT
*
*   Texture - Texture to store
*   
* OUTPUT
*
*   Number_Of_Textures - Number of textures
*   Max_Textures       - Max. number of textures
*   Textures           - List of textures
*   
* RETURNS
*
*   int - Index of texture into the texture list
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Try to locate a texture in the texture list.
*   If the texture is not found its stored in the texture list.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

int Mesh_Hash_Texture(int *Number_Of_Textures, int  *Max_Textures, TEXTURE ***Textures, TEXTURE  *Texture)
{
  int i;

  if (Texture == NULL)
  {
    return(-1);
  }

  /* Just do a linear search. */

  for (i = 0; i < *Number_Of_Textures; i++)
  {
    if ((*Textures)[i] == Texture)
    {
      break;
    }
  }

  if (i == *Number_Of_Textures)
  {
    if ((*Number_Of_Textures) >= (*Max_Textures))
    {
      if ((*Max_Textures) >= INT_MAX/2)
      {
        Error("Too many textures in mesh.\n");
      }

      (*Max_Textures) *= 2;

      (*Textures) = (TEXTURE **)POV_REALLOC((*Textures), (*Max_Textures)*sizeof(TEXTURE *), "mesh data");
    }

    (*Textures)[(*Number_Of_Textures)++] = Copy_Texture_Pointer(Texture);
  }

  return(i);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Mesh_Hash_Tables
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

void Create_Mesh_Hash_Tables()
{
  int i;

  Vertex_Hash_Table = (HASH_TABLE **)POV_MALLOC(HASH_SIZE*sizeof(HASH_TABLE *), "mesh hash table");

  for (i = 0; i < HASH_SIZE; i++)
  {
    Vertex_Hash_Table[i] = NULL;
  }

  Normal_Hash_Table = (HASH_TABLE **)POV_MALLOC(HASH_SIZE*sizeof(HASH_TABLE *), "mesh hash table");

  for (i = 0; i < HASH_SIZE; i++)
  {
    Normal_Hash_Table[i] = NULL;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Mesh_Hash_Tables
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
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

void Destroy_Mesh_Hash_Tables()
{
  int i;
  HASH_TABLE *Temp;

  for (i = 0; i < HASH_SIZE; i++)
  {
    while (Vertex_Hash_Table[i] != NULL)
    {
      Temp = Vertex_Hash_Table[i];
      
      Vertex_Hash_Table[i] = Temp->Next;
      
      POV_FREE(Temp);
    }
  }

  POV_FREE(Vertex_Hash_Table);

  for (i = 0; i < HASH_SIZE; i++)
  {
    while (Normal_Hash_Table[i] != NULL)
    {
      Temp = Normal_Hash_Table[i];
      
      Normal_Hash_Table[i] = Temp->Next;
      
      POV_FREE(Temp);
    }
  }

  POV_FREE(Normal_Hash_Table);
}



/*****************************************************************************
*
* FUNCTION
*
*   get_triangle_vertices
*
* INPUT
*
*   Mesh     - Mesh object
*   Triangle - Triangle
*   
* OUTPUT
*   
* RETURNS
*
*   P1, P2, P3 - Vertices of the triangle
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void get_triangle_vertices(MESH *Mesh, MESH_TRIANGLE *Triangle, VECTOR P1, VECTOR  P2, VECTOR  P3)
{
  Assign_SNGL_Vect(P1, Mesh->Data->Vertices[Triangle->P1]);
  Assign_SNGL_Vect(P2, Mesh->Data->Vertices[Triangle->P2]);
  Assign_SNGL_Vect(P3, Mesh->Data->Vertices[Triangle->P3]);
}



/*****************************************************************************
*
* FUNCTION
*
*   get_triangle_normals
*
* INPUT
*
*   Mesh     - Mesh object
*   Triangle - Triangle
*   
* OUTPUT
*   
* RETURNS
*
*   N1, N2, N3 - Normals of the triangle
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void get_triangle_normals(MESH *Mesh, MESH_TRIANGLE *Triangle, VECTOR N1, VECTOR  N2, VECTOR  N3)
{
  Assign_SNGL_Vect(N1, Mesh->Data->Normals[Triangle->N1]);
  Assign_SNGL_Vect(N2, Mesh->Data->Normals[Triangle->N2]);
  Assign_SNGL_Vect(N3, Mesh->Data->Normals[Triangle->N3]);
}



/*****************************************************************************
*
* FUNCTION
*
*   Mesh_Degenerate
*
* INPUT
*
*   P1, P2, P3 - Triangle's vertices
*   
* OUTPUT
*   
* RETURNS
*
*   int - TRUE if degenerate
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Test if a triangle is degenerate.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

int Mesh_Degenerate(VECTOR P1, VECTOR  P2, VECTOR  P3)
{
  VECTOR V1, V2, Temp;
  DBL Length;

  VSub(V1, P1, P2);
  VSub(V2, P3, P2);

  VCross(Temp, V1, V2);

  VLength(Length, Temp);

  return(Length == 0.0);
}


/*****************************************************************************
*
* FUNCTION
*
*   Initialize_Mesh_Code
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
*   Initialize mesh specific variables.
*
* CHANGES
*
*   Jul 1995 : Creation.
*
******************************************************************************/

void Initialize_Mesh_Code()
{
  Mesh_Queue = Create_Priority_Queue(INITIAL_NUMBER_OF_ENTRIES);
}



/*****************************************************************************
*
* FUNCTION
*
*   Deinitialize_Mesh_Code
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
*   Deinitialize mesh specific variables.
*
* CHANGES
*
*   Jul 1995 : Creation.
*
******************************************************************************/

void Deinitialize_Mesh_Code()
{
  if (Mesh_Queue != NULL)
  {
    Destroy_Priority_Queue(Mesh_Queue);
  }

  Mesh_Queue = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Test_Mesh_Opacity
*
* INPUT
*
*   Mesh - Pointer to mesh structure
*
* OUTPUT
*
*   Mesh
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Set the opacity flag of the mesh according to the opacity
*   of the mesh's texture(s).
*
* CHANGES
*
*   Apr 1996 : Creation.
*
******************************************************************************/

void Test_Mesh_Opacity(MESH *Mesh)
{
  int i;

  /* Initialize opacity flag to the opacity of the object's texture. */

  if ((Mesh->Texture == NULL) || (Test_Opacity(Mesh->Texture)))
  {
    Set_Flag(Mesh, OPAQUE_FLAG);
  }

  if (Test_Flag(Mesh, MULTITEXTURE_FLAG))
  {
    for (i = 0; i < Mesh->Data->Number_Of_Textures; i++)
    {
      if (Mesh->Data->Textures[i] != NULL)
      {
        /* If component's texture isn't opaque the mesh is neither. */

        if (!Test_Opacity(Mesh->Data->Textures[i]))
        {
          Clear_Flag(Mesh, OPAQUE_FLAG);
        }
      }
    }
  }
}



