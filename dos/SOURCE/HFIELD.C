/****************************************************************************
*                   hfield.c
*
*    This file implements the height field shape primitive.  The shape is
*    implemented as a collection of triangles which are calculated as
*    needed.  The basic intersection routine first computes the rays
*    intersection with the box marking the limits of the shape, then
*    follows the line from one intersection point to the other, testing the
*    two triangles which form the pixel for an intersection with the ray at
*    each step.
*
*    height field added by Doug Muir with lots of advice and support
*    from David Buck and Drew Wells.
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
*  ---
*
*  Aug 1994 : Merged functions for CSG height fields into functions for
*             non-CSG height fiels. Moved all height field map related
*             data into one data structure. Fixed memory problems. [DB]
*
*  Feb 1995 : Major rewrite of the height field intersection tests. [DB]
*
*****************************************************************************/

#include "frame.h"
#include "povray.h"
#include "vector.h"
#include "povproto.h"
#include "hfield.h"
#include "matrices.h"
#include "objects.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define sign(x) (((x) >= 0.0) ? 1.0 : -1.0)

#define Get_Height(x, z, HField) ((DBL)(HField)->Data->Map[(z)][(x)])

/* Small offest. */

#define HFIELD_OFFSET 0.001



/*****************************************************************************
* Static functions
******************************************************************************/

static DBL normalize (VECTOR A, VECTOR B);

static DBL stretch (DBL x);

static void smooth_height_field (HFIELD *HField, int xsize, int zsize);

static int intersect_pixel (int x,int z,RAY *Ray,HFIELD *HField,DBL height1,DBL height2);

static int add_single_normal (HF_VAL **data, int xsize, int zsize,
  int x0, int z0,int x1, int z1,int x2, int z2,VECTOR N);

static int  All_HField_Intersections (OBJECT *Object,RAY *Ray,ISTACK *Depth_Stack);
static int  Inside_HField (VECTOR IPoint,OBJECT *Object);
static void HField_Normal (VECTOR Result,OBJECT *Object,INTERSECTION *Inter);
static void Translate_HField (OBJECT *Object,VECTOR Vector, TRANSFORM *Trans);
static void Rotate_HField (OBJECT *Object,VECTOR Vector, TRANSFORM *Trans);
static void Scale_HField (OBJECT *Object,VECTOR Vector, TRANSFORM *Trans);
static void Invert_HField (OBJECT *Object);
static void Transform_HField (OBJECT *Object,TRANSFORM *Trans);
static HFIELD *Copy_HField (OBJECT *Object);
static void Destroy_HField (OBJECT *Object);

static int dda_traversal (RAY *Ray, HFIELD *HField, VECTOR Start, HFIELD_BLOCK *Block);
static int block_traversal (RAY *Ray, HFIELD *HField, VECTOR Start);
static void build_hfield_blocks (HFIELD *HField);



/*****************************************************************************
* Local variables
******************************************************************************/

METHODS HField_Methods =
{
  All_HField_Intersections,
  Inside_HField, HField_Normal,
  (COPY_METHOD)Copy_HField, Translate_HField, Rotate_HField,
  Scale_HField, Transform_HField, Invert_HField, Destroy_HField
};

static ISTACK *HField_Stack;
static RAY *RRay;
static DBL mindist, maxdist;


/*****************************************************************************
*
* FUNCTION
*
*   All_HField_Intersections
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Feb 1995 : Modified to work with new intersection functions. [DB]
*
******************************************************************************/

static int All_HField_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int Side1, Side2;
  VECTOR Start;
  RAY Temp_Ray;
  DBL depth1, depth2;
  HFIELD *HField = (HFIELD *) Object;

  Increase_Counter(stats[Ray_HField_Tests]);

  MInvTransPoint(Temp_Ray.Initial, Ray->Initial, HField->Trans);
  MInvTransDirection(Temp_Ray.Direction, Ray->Direction, HField->Trans);

#ifdef HFIELD_EXTRA_STATS
  Increase_Counter(stats[Ray_HField_Box_Tests]);
#endif

  if (!Intersect_Box(&Temp_Ray,HField->bounding_box,&depth1,&depth2,&Side1,&Side2))
  {
    return(FALSE);
  }

#ifdef HFIELD_EXTRA_STATS
  Increase_Counter(stats[Ray_HField_Box_Tests_Succeeded]);
#endif

  HField->Data->cache_pos = 0;

  if (depth1 < EPSILON)
  {
    depth1 = EPSILON;

    if (depth1 > depth2)
    {
      return(FALSE);
    }
  }

  VEvaluateRay(Start, Temp_Ray.Initial, depth1, Temp_Ray.Direction);

  mindist = depth1;
  maxdist = depth2;

  HField_Stack = Depth_Stack;

  RRay = Ray;

  if (block_traversal(&Temp_Ray, HField, Start))
  {
    Increase_Counter(stats[Ray_HField_Tests_Succeeded]);

    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_HField
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static int Inside_HField (VECTOR IPoint, OBJECT *Object)
{
  HFIELD *HField = (HFIELD *) Object;
  int px, pz;
  DBL x,z,y1,y2,y3,water, dot1, dot2;
  VECTOR Local_Origin, H_Normal, Test;

  MInvTransPoint(Test, IPoint, HField->Trans);

  water = HField->bounding_box->bounds[0][Y];

  if ((Test[X] < 0.0) || (Test[X] >= HField->bounding_box->bounds[1][X]) ||
      (Test[Z] < 0.0) || (Test[Z] >= HField->bounding_box->bounds[1][Z]))
  {
    return(Test_Flag(HField, INVERTED_FLAG));
  }

  if (Test[Y] >= HField->bounding_box->bounds[1][Y])
  {
    return(Test_Flag(HField, INVERTED_FLAG));
  }

  if (Test[Y] < water)
  {
    return(!Test_Flag(HField, INVERTED_FLAG));
  }

  px = (int)Test[X];
  pz = (int)Test[Z];

  x = Test[X] - (DBL)px;
  z = Test[Z] - (DBL)pz;

  if ((x+z) < 1.0)
  {
    y1 = max(Get_Height(px,   pz,   HField), water);
    y2 = max(Get_Height(px+1, pz,   HField), water);
    y3 = max(Get_Height(px,   pz+1, HField), water);

    Make_Vector(Local_Origin,(DBL)px,y1,(DBL)pz);

    Make_Vector(H_Normal, y1-y2, 1.0, y1-y3);
  }
  else
  {
    px = (int)ceil(Test[X]);
    pz = (int)ceil(Test[Z]);

    y1 = max(Get_Height(px,   pz,   HField), water);
    y2 = max(Get_Height(px-1, pz,   HField), water);
    y3 = max(Get_Height(px,   pz-1, HField), water);

    Make_Vector(Local_Origin,(DBL)px,y1,(DBL)pz);

    Make_Vector(H_Normal, y2-y1, 1.0, y3-y1);
  }

  VDot(dot1, Test, H_Normal);
  VDot(dot2, Local_Origin, H_Normal);

  if (dot1 < dot2)
  {
    return(!Test_Flag(HField, INVERTED_FLAG));
  }

  return(Test_Flag(HField, INVERTED_FLAG));
}



/*****************************************************************************
*
* FUNCTION
*
*   HField_Normal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static void HField_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  HFIELD *HField = (HFIELD *) Object;
  int px,pz, i;
  DBL x,z,y1,y2,y3,u,v;
  VECTOR Local_Origin;
  VECTOR n[5];

  MInvTransPoint(Local_Origin, Inter->IPoint, HField->Trans);

  for (i = 0; i < HField->Data->cache_pos; i++)
  {
    if ((Local_Origin[X] == HField->Data->Normal_Cache[i].fx) &&
       (Local_Origin[Z] == HField->Data->Normal_Cache[i].fz))
    {
      Assign_Vector(Result,HField->Data->Normal_Cache[i].normal);

      MTransNormal(Result,Result,HField->Trans);

      VNormalize(Result,Result);

      return;
    }
  }

  px = (int)Local_Origin[X];
  pz = (int)Local_Origin[Z];

  x = Local_Origin[X] - (DBL)px;
  z = Local_Origin[Z] - (DBL)pz;

  if (Test_Flag(HField, SMOOTHED_FLAG))
  {
    n[0][X] = HField->Data->Normals[pz][px][0];
    n[0][Y] = HField->Data->Normals[pz][px][1];
    n[0][Z] = HField->Data->Normals[pz][px][2];
    n[1][X] = HField->Data->Normals[pz][px+1][0];
    n[1][Y] = HField->Data->Normals[pz][px+1][1];
    n[1][Z] = HField->Data->Normals[pz][px+1][2];
    n[2][X] = HField->Data->Normals[pz+1][px][0];
    n[2][Y] = HField->Data->Normals[pz+1][px][1];
    n[2][Z] = HField->Data->Normals[pz+1][px][2];
    n[3][X] = HField->Data->Normals[pz+1][px+1][0];
    n[3][Y] = HField->Data->Normals[pz+1][px+1][1];
    n[3][Z] = HField->Data->Normals[pz+1][px+1][2];

    x = stretch(x);
    z = stretch(z);

    u = (1.0 - x);
    v = (1.0 - z);

    Result[X] = v*(u*n[0][X] + x*n[1][X]) + z*(u*n[2][X] + x*n[3][X]);
    Result[Y] = v*(u*n[0][Y] + x*n[1][Y]) + z*(u*n[2][Y] + x*n[3][Y]);
    Result[Z] = v*(u*n[0][Z] + x*n[1][Z]) + z*(u*n[2][Z] + x*n[3][Z]);
  }
  else
  {
    if ((x+z) <= 1.0)
    {
      /* Lower triangle. */

      y1 = Get_Height(px,   pz,   HField);
      y2 = Get_Height(px+1, pz,   HField);
      y3 = Get_Height(px,   pz+1, HField);

      Make_Vector(Result, y1-y2, 1.0, y1-y3);
    }
    else
    {
      /* Upper triangle. */

      y1 = Get_Height(px+1, pz+1, HField);
      y2 = Get_Height(px,   pz+1, HField);
      y3 = Get_Height(px+1, pz,   HField);

      Make_Vector(Result, y2-y1, 1.0, y3-y1);
    }
  }

  MTransNormal(Result, Result, HField->Trans);

  VNormalize(Result, Result);
}



/*****************************************************************************
*
* FUNCTION
*
*   stretch
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static DBL stretch (DBL x)
{
  if (x <= 0.5)
  {
    x = 2.0 * x * x;
  }
  else
  {
    x = 1.0 - (2.0 * (1.0 - x) * (1.0 - x));
  }

  return(x);
}



/*****************************************************************************
*
* FUNCTION
*
*   normalize
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static DBL normalize(VECTOR A, VECTOR  B)
{
  VLength(VTemp, B);

  if (fabs(VTemp) > EPSILON)
  {
    VInverseScale(A, B, VTemp);
  }
  else
  {
    Make_Vector(A, 0.0, 1.0, 0.0);
  }

  return(VTemp);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_pixel
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static int intersect_pixel(int x, int z, RAY *Ray, HFIELD *HField, DBL height1, DBL  height2)
{
  int Found;
  DBL dot, depth1, depth2;
  DBL s, t, y1, y2, y3, y4;
  DBL min_y2_y3, max_y2_y3;
  DBL max_height, min_height;
  VECTOR P, N, V1;

#ifdef HFIELD_EXTRA_STATS
  Increase_Counter(stats[Ray_HField_Cell_Tests]);
#endif

  y1 = Get_Height(x,   z,   HField);
  y2 = Get_Height(x+1, z,   HField);
  y3 = Get_Height(x,   z+1, HField);
  y4 = Get_Height(x+1, z+1, HField);

  /* Do we hit this cell at all? */

  if (y2 < y3)
  {
    min_y2_y3 = y2;
    max_y2_y3 = y3;
  }
  else
  {
    min_y2_y3 = y3;
    max_y2_y3 = y2;
  }

  min_height = min(min(y1, y4), min_y2_y3);
  max_height = max(max(y1, y4), max_y2_y3);

  if ((max_height < height1) || (min_height > height2))
  {
    return(FALSE);
  }

#ifdef HFIELD_EXTRA_STATS
  Increase_Counter(stats[Ray_HField_Cell_Tests_Succeeded]);
#endif

  Found = FALSE;

  /* Check if we'll hit first triangle. */

  min_height = min(y1, min_y2_y3);
  max_height = max(y1, max_y2_y3);

  if ((max_height >= height1) && (min_height <= height2))
  {
#ifdef HFIELD_EXTRA_STATS
    Increase_Counter(stats[Ray_HField_Triangle_Tests]);
#endif

    /* Set up triangle. */

    Make_Vector(P, (DBL)x, y1, (DBL)z);

    /*
     * Calculate the normal vector from:
     *
     * N = V2 x V1, with V1 = <1, y2-y1, 0>, V2 = <0, y3-y1, 1>.
     */

    Make_Vector(N, y1-y2, 1.0, y1-y3);

    /* Now intersect the triangle. */

    VDot(dot, N, Ray->Direction);

    if ((dot > EPSILON) || (dot < -EPSILON))
    {
      VSub(V1, P, Ray->Initial);

      VDot(depth1, N, V1);

      depth1 /= dot;

      if ((depth1 >= mindist) && (depth1 <= maxdist))
      {
        s = Ray->Initial[X] + depth1 * Ray->Direction[X] - (DBL)x;
        t = Ray->Initial[Z] + depth1 * Ray->Direction[Z] - (DBL)z;

        if ((s >= -0.0001) && (t >= -0.0001) && ((s+t) <= 1.0001))
        {
#ifdef HFIELD_EXTRA_STATS
          Increase_Counter(stats[Ray_HField_Triangle_Tests_Succeeded]);
#endif

          VEvaluateRay(P, RRay->Initial, depth1, RRay->Direction);

          if (Point_In_Clip(P, HField->Clip))
          {
            push_entry(depth1, P, (OBJECT *)HField, HField_Stack);

            Found = TRUE;

            /* Cache normal. */

            if (!Test_Flag(HField, SMOOTHED_FLAG))
            {
              if (HField->Data->cache_pos < HF_CACHE_SIZE)
              {
                Assign_Vector(HField->Data->Normal_Cache[HField->Data->cache_pos].normal, N);

                HField->Data->Normal_Cache[HField->Data->cache_pos].fx = x + s;
                HField->Data->Normal_Cache[HField->Data->cache_pos].fz = z + t;

                HField->Data->cache_pos++;
              }
            }
          }
        }
      }
    }
  }

  /* Check if we'll hit second triangle. */

  min_height = min(y4, min_y2_y3);
  max_height = max(y4, max_y2_y3);

  if ((max_height >= height1) && (min_height <= height2))
  {
#ifdef HFIELD_EXTRA_STATS
    Increase_Counter(stats[Ray_HField_Triangle_Tests]);
#endif

    /* Set up triangle. */

    Make_Vector(P, (DBL)(x+1), y4, (DBL)(z+1));

    /*
     * Calculate the normal vector from:
     *
     * N = V2 x V1, with V1 = <-1, y3-y4, 0>, V2 = <0, y2-y4, -1>.
     */

    Make_Vector(N, y3-y4, 1.0, y2-y4);

    /* Now intersect the triangle. */

    VDot(dot, N, Ray->Direction);

    if ((dot > EPSILON) || (dot < -EPSILON))
    {
      VSub(V1, P, Ray->Initial);

      VDot(depth2, N, V1);

      depth2 /= dot;

      if ((depth2 >= mindist) && (depth2 <= maxdist))
      {
        s = Ray->Initial[X] + depth2 * Ray->Direction[X] - (DBL)x;
        t = Ray->Initial[Z] + depth2 * Ray->Direction[Z] - (DBL)z;

        if ((s <= 1.0001) && (t <= 1.0001) && ((s+t) >= 0.9999))
        {
#ifdef HFIELD_EXTRA_STATS
          Increase_Counter(stats[Ray_HField_Triangle_Tests_Succeeded]);
#endif

          VEvaluateRay(P, RRay->Initial, depth2, RRay->Direction);

          if (Point_In_Clip(P, HField->Clip))
          {
            push_entry(depth2, P, (OBJECT *)HField, HField_Stack);

            Found = TRUE;

            /* Cache normal. */

            if (!Test_Flag(HField, SMOOTHED_FLAG))
            {
              if (HField->Data->cache_pos < HF_CACHE_SIZE)
              {
                Assign_Vector(HField->Data->Normal_Cache[HField->Data->cache_pos].normal, N);

                HField->Data->Normal_Cache[HField->Data->cache_pos].fx = x + s;
                HField->Data->Normal_Cache[HField->Data->cache_pos].fz = z + t;

                HField->Data->cache_pos++;
              }
            }
          }
        }
      }
    }
  }

  return(Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   add_single_normal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static int add_single_normal(HF_VAL **data, int xsize, int zsize, int x0, int z0, int x1, int z1, int x2, int z2, VECTOR N)
{
  VECTOR v0, v1, v2, t0, t1, Nt;

  if ((x0 < 0) || (z0 < 0) ||
      (x1 < 0) || (z1 < 0) ||
      (x2 < 0) || (z2 < 0) ||
      (x0 > xsize) || (z0 > zsize) ||
      (x1 > xsize) || (z1 > zsize) ||
      (x2 > xsize) || (z2 > zsize))
  {
    return(0);
  }
  else
  {
    Make_Vector(v0, x0, (DBL)data[z0][x0], z0);
    Make_Vector(v1, x1, (DBL)data[z1][x1], z1);
    Make_Vector(v2, x2, (DBL)data[z2][x2], z2);

    VSub(t0, v2, v0);
    VSub(t1, v1, v0);

    VCross(Nt, t0, t1);

    normalize(Nt, Nt);

    if (Nt[Y] < 0.0)
    {
      VScaleEq(Nt, -1.0);
    }

    VAddEq(N, Nt);

    return(1);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   smooth_height_field
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*   
* DESCRIPTION
*
*   Given a height field that only contains an elevation grid, this
*   routine will walk through the data and produce averaged normals
*   for all points on the grid.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void smooth_height_field(HFIELD *HField, int xsize, int zsize)
{
  int i, j, k;
  VECTOR N;
  HF_VAL **map = HField->Data->Map;

  /* First off, allocate all the memory needed to store the normal information */

  HField->Data->Normals_Height = zsize+1;

  HField->Data->Normals = (HF_Normals **)POV_MALLOC((zsize+1)*sizeof(HF_Normals *), "height field normals");

  for (i = 0; i <= zsize; i++)
  {
    HField->Data->Normals[i] = (HF_Normals *)POV_MALLOC((xsize+1)*sizeof(HF_Normals), "height field normals");
  }

  /*
   * For now we will do it the hard way - by generating the normals
   * individually for each elevation point.
   */

  for (i = 0; i < zsize; i++)
  {
    COOPERATE_0

    for (j = 0; j < xsize; j++)
    {
      Make_Vector(N, 0.0, 0.0, 0.0);

      k = 0;

      k += add_single_normal(map, xsize, zsize, j, i, j+1, i, j, i+1, N);
      k += add_single_normal(map, xsize, zsize, j, i, j, i+1, j-1, i, N);
      k += add_single_normal(map, xsize, zsize, j, i, j-1, i, j, i-1, N);
      k += add_single_normal(map, xsize, zsize, j, i, j, i-1, j+1, i, N);

      if (k == 0)
      {
        Error ("Failed to find any normals at: (%d, %d).\n", i, j);
      }

      normalize(N, N);

      HField->Data->Normals[i][j][0] = (short)(32767 * N[X]);
      HField->Data->Normals[i][j][1] = (short)(32767 * N[Y]);
      HField->Data->Normals[i][j][2] = (short)(32767 * N[Z]);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   Copy image data into height field map. Create bounding blocks
*   for the block traversal. Calculate normals for smoothed height fields.
*
* CHANGES
*
*   Feb 1995 : Modified to work with new intersection functions. [DB]
*
******************************************************************************/

void Compute_HField(HFIELD *HField, IMAGE *Image)
{
  int x, z, max_x, max_z;
  int temp1 = 0, temp2 = 0;
  HF_VAL min_y, max_y, temp_y;

  /* Get height field map size. */

  max_x = Image->iwidth;

  if (Image->File_Type == POT_FILE)
  {
    max_x = max_x / 2;
  }

  max_z = Image->iheight;

  /* Allocate memory for map. */

  HField->Data->Map = (HF_VAL **)POV_MALLOC(max_z*sizeof(HF_VAL *), "height field");

  for (z = 0; z < max_z; z++)
  {
    HField->Data->Map[z] = (HF_VAL *)POV_MALLOC(max_x*sizeof(HF_VAL), "height field");
  }

  /* Copy map. */

  min_y = 65535L;
  max_y = 0;

  for (z = 0; z < max_z; z++)
  {
    COOPERATE_0
    for (x = 0; x < max_x; x++)
    {

      switch (Image->File_Type)
      {
        case GIF_FILE:

          temp1 = Image->data.map_lines[max_z - z - 1][x];
          temp2 = 0;

          break;

        case POT_FILE:

          temp1 = Image->data.map_lines[max_z - z - 1][x];
          temp2 = Image->data.map_lines[max_z - z - 1][x + max_x];

          break;


        case PPM_FILE:

          temp1 = Image->data.rgb_lines[max_z - z - 1].red[x];
          temp2 = Image->data.rgb_lines[max_z - z - 1].green[x];

          break;

        case PGM_FILE:
        case TGA_FILE:
        case PNG_FILE:

          if (Image->Colour_Map == NULL)
          {
            temp1 = Image->data.rgb_lines[max_z - z - 1].red[x];
            temp2 = Image->data.rgb_lines[max_z - z - 1].green[x];
          }
          else
          {
            temp1 = Image->data.map_lines[max_z - z - 1][x];
            temp2 = 0;
          }

          break;

        default:

          Error("Unknown image type in Compute_HField().\n");
      }

      temp_y = (HF_VAL)(256*temp1 + temp2);

      HField->Data->Map[z][x] = temp_y;

      min_y = min(min_y, temp_y);
      max_y = max(max_y, temp_y);
    }
  }

  /* Resize bounding box. */

  HField->Data->min_y = min_y;
  HField->Data->max_y = max_y;

  HField->bounding_box->bounds[0][Y] = max((DBL)min_y, HField->bounding_box->bounds[0][Y]) - HFIELD_OFFSET;
  HField->bounding_box->bounds[1][Y] = (DBL)max_y + HFIELD_OFFSET;

  /* Compute smoothed height field. */

  if (Test_Flag(HField, SMOOTHED_FLAG))
  {
    smooth_height_field(HField, max_x-1, max_z-1);
  }

  HField->Data->max_x = max_x-2;
  HField->Data->max_z = max_z-2;

  build_hfield_blocks(HField);
}



/*****************************************************************************
*
* FUNCTION
*
*   build_hfield_blocks
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
*   Create the bounding block hierarchy used by the block traversal.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void build_hfield_blocks(HFIELD *HField)
{
  int x, z, nx, nz, wx, wz;
  int i, j;
  int xmin, xmax, zmin, zmax;
  DBL y, ymin, ymax, water;

  /* Get block size. */

  nx = max(1, (int)(sqrt((DBL)HField->Data->max_x)));
  nz = max(1, (int)(sqrt((DBL)HField->Data->max_z)));

  /* Get dimensions of sub-block. */

  wx = (int)ceil((DBL)(HField->Data->max_x + 2) / (DBL)nx);
  wz = (int)ceil((DBL)(HField->Data->max_z + 2) / (DBL)nz);

  /* Increase number of sub-blocks if necessary. */

  if (nx * wx < HField->Data->max_x + 2)
  {
    nx++;
  }

  if (nz * wz < HField->Data->max_z + 2)
  {
    nz++;
  }

  if (!Test_Flag(HField, HIERARCHY_FLAG) || ((nx == 1) && (nz == 1)))
  {
    /* We don't want a bounding hierarchy. Just use one block. */

    HField->Data->Block = (HFIELD_BLOCK **)POV_MALLOC(sizeof(HFIELD_BLOCK *), "height field blocks");

    HField->Data->Block[0] = (HFIELD_BLOCK *)POV_MALLOC(sizeof(HFIELD_BLOCK), "height field blocks");

    HField->Data->Block[0][0].xmin = 0;
    HField->Data->Block[0][0].xmax = HField->Data->max_x;
    HField->Data->Block[0][0].zmin = 0;
    HField->Data->Block[0][0].zmax = HField->Data->max_z;

    HField->Data->Block[0][0].ymin = HField->bounding_box->bounds[0][Y];
    HField->Data->Block[0][0].ymax = HField->bounding_box->bounds[1][Y];

    HField->Data->block_max_x = 1;
    HField->Data->block_max_z = 1;

    HField->Data->block_width_x = HField->Data->max_x + 2;
    HField->Data->block_width_z = HField->Data->max_y + 2;

/*
    Debug_Info("\nHeight field: %d x %d (1 x 1 blocks)", HField->Data->max_x+2, HField->Data->max_z+2);
*/

    return;
  }

/*
  Debug_Info("\nHeight field: %d x %d (%d x %d blocks)", HField->Data->max_x+2, HField->Data->max_z+2, nx, nz);
*/

  /* Allocate memory for blocks. */

  HField->Data->Block = (HFIELD_BLOCK **)POV_MALLOC(nz*sizeof(HFIELD_BLOCK *), "height field blocks");

  /* Store block information. */

  HField->Data->block_max_x = nx;
  HField->Data->block_max_z = nz;

  HField->Data->block_width_x = wx;
  HField->Data->block_width_z = wz;

  water = HField->bounding_box->bounds[0][Y];

  for (z = 0; z < nz; z++)
  {
    COOPERATE_1

    HField->Data->Block[z] = (HFIELD_BLOCK *)POV_MALLOC(nx*sizeof(HFIELD_BLOCK), "height field blocks");

    for (x = 0; x < nx; x++)
    {
      /* Get block's borders. */

      xmin = x * wx;
      zmin = z * wz;

      xmax = min((x + 1) * wx - 1, HField->Data->max_x);
      zmax = min((z + 1) * wz - 1, HField->Data->max_z);

      /* Find min. and max. height in current block. */

      ymin = BOUND_HUGE;
      ymax = -BOUND_HUGE;

      for (i = xmin; i <= xmax+1; i++)
      {
        for (j = zmin; j <= zmax+1; j++)
        {
          y = Get_Height(i, j, HField);

          ymin = min(ymin, y);
          ymax = max(ymax, y);
        }
      }

      /* Store block's borders. */

      HField->Data->Block[z][x].xmin = xmin;
      HField->Data->Block[z][x].xmax = xmax;
      HField->Data->Block[z][x].zmin = zmin;
      HField->Data->Block[z][x].zmax = zmax;

      HField->Data->Block[z][x].ymin = max(ymin, water) - HFIELD_OFFSET;
      HField->Data->Block[z][x].ymax = ymax + HFIELD_OFFSET;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_HField
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static void Translate_HField (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_HField(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static void Rotate_HField (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_HField(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_HField
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static void Scale_HField (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_HField(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_HField
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static void Invert_HField (OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_HField
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
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

static void Transform_HField (OBJECT *Object, TRANSFORM *Trans)
{
  Compose_Transforms(((HFIELD *)Object)->Trans, Trans);

  Compute_HField_BBox((HFIELD *)Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_HField
*
* INPUT
*
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*   
* DESCRIPTION
*
*   Allocate and intialize a height field.
*
* CHANGES
*
*   Feb 1995 : Modified to work with new intersection functions. [DB]
*
******************************************************************************/

HFIELD *Create_HField()
{
  HFIELD *New;

  /* Allocate height field. */

  New = (HFIELD *)POV_MALLOC(sizeof(HFIELD), "height field");

  INIT_OBJECT_FIELDS(New, HFIELD_OBJECT, &HField_Methods)

  /* Always uses Trans so always create one. */

  New->Trans = Create_Transform();

  New->bounding_box = Create_Box();

  /* Allocate height field data. */

  New->Data = (HFIELD_DATA *)POV_MALLOC(sizeof(HFIELD_DATA), "height field");

  New->Data->References = 1;

  New->Data->cache_pos = 0;

  New->Data->Normals_Height = 0;

  New->Data->Map     = NULL;
  New->Data->Normals = NULL;

  New->Data->max_x = 0;
  New->Data->max_z = 0;

  New->Data->block_max_x = 0;
  New->Data->block_max_z = 0;

  New->Data->block_width_x = 0;
  New->Data->block_width_z = 0;

  Set_Flag(New, HIERARCHY_FLAG);

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_HField
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*   
* DESCRIPTION
*
*   NOTE: The height field data is not copied, only the number of references
*         is counted, so that Destray_HField() knows if it can be destroyed.
*
* CHANGES
*
*   -
*
******************************************************************************/

static HFIELD *Copy_HField(OBJECT *Object)
{
  HFIELD *New;

  New = Create_HField();

  /* Destroy obsolete things created in Create_HField(). */

  Destroy_Transform(New->Trans);

  Destroy_Box((OBJECT *)(New->bounding_box));

  POV_FREE (New->Data);

  /* Copy height field. */

  *New = *((HFIELD *)Object);

  New->Trans = Copy_Transform(((HFIELD *)Object)->Trans);

  New->bounding_box = Copy_Box((OBJECT *)(((HFIELD *)Object)->bounding_box));

  New->Data->References++;

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_HField
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*   
* DESCRIPTION
*
*   NOTE: The height field data is destroyed if it's no longer
*         used by any copy.
*
* CHANGES
*
*   Feb 1995 : Modified to work with new intersection functions. [DB]
*
******************************************************************************/

static void Destroy_HField (OBJECT *Object)
{
  int i;
  HFIELD *HField = (HFIELD *)Object;

  Destroy_Transform(HField->Trans);

  Destroy_Box((OBJECT *)(HField->bounding_box));

  if (--(HField->Data->References) == 0)
  {
    if (HField->Data->Map != NULL)
    {
      for (i = 0; i < HField->Data->max_z+2; i++)
      {
        if (HField->Data->Map[i] != NULL)
        {
          POV_FREE (HField->Data->Map[i]);
        }
      }

      POV_FREE (HField->Data->Map);
    }

    if (HField->Data->Normals != NULL)
    {
      for (i = 0; i < HField->Data->Normals_Height; i++)
      {
        POV_FREE (HField->Data->Normals[i]);
      }

      POV_FREE (HField->Data->Normals);
    }

    if (HField->Data->Block != NULL)
    {
      for (i = 0; i < HField->Data->block_max_z; i++)
      {
        POV_FREE(HField->Data->Block[i]);
      }

      POV_FREE(HField->Data->Block);
    }

    POV_FREE (HField->Data);
  }

  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_HField_BBox
*
* INPUT
*
*   HField - Height field
*   
* OUTPUT
*
*   HField
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a height field.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Compute_HField_BBox(HFIELD *HField)
{
  Assign_BBox_Vect(HField->BBox.Lower_Left, HField->bounding_box->bounds[0]);

  VSub (HField->BBox.Lengths, HField->bounding_box->bounds[1], HField->bounding_box->bounds[0]);

  if (HField->Trans != NULL)
  {
    Recompute_BBox(&HField->BBox, HField->Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   dda_traversal
*
* INPUT
*
*   Ray    - Current ray
*   HField - Height field
*   Start  - Start point for the walk
*   Block  - Sub-block of the height field to traverse
*   
* OUTPUT
*   
* RETURNS
*
*   int - TRUE if intersection was found
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Traverse the grid cell of the height field using a modified DDA.
*
*   Based on the following article:
*
*     Musgrave, F. Kenton, "Grid Tracing: Fast Ray Tracing for Height
*     Fields", Research Report YALEU-DCS-RR-39, Yale University, July 1988
*
*   You should note that there are (n-1) x (m-1) grid cells in a height
*   field of (image) size n x m. A grid cell (i,j), 0 <= i <= n-1,
*   0 <= j <= m-1, extends from x = i to x = i + 1 - epsilon and
*   y = j to y = j + 1 -epsilon.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static int dda_traversal(RAY *Ray, HFIELD *HField, VECTOR Start, HFIELD_BLOCK *Block)
{
  int found;
  int xmin, xmax, zmin, zmax;
  int x, z, signx, signz;
  DBL ymin, ymax, y1, y2;
  DBL px, pz, dx, dy, dz;
  DBL delta, error, x0, z0;
  DBL neary, fary, deltay;

  /* Setup DDA. */

  found = FALSE;

  px = Start[X];
  pz = Start[Z];

  /* Get dimensions of current block. */

  xmin = Block->xmin;
  xmax = min(Block->xmax + 1, HField->Data->max_x);
  zmin = Block->zmin;
  zmax = min(Block->zmax + 1, HField->Data->max_z);

  ymin = min(Start[Y], Block->ymin) - EPSILON;
  ymax = max(Start[Y], Block->ymax) + EPSILON;

  /* Check for illegal grid values (caused by numerical inaccuracies). */

  if (px < (DBL)xmin)
  {
    if (px < (DBL)xmin - HFIELD_OFFSET)
    {
      Debug_Info("Illegal grid value in dda_traversal().\n");

      return(FALSE);
    }
    else
    {
      px = (DBL)xmin;
    }
  }
  else
  {
    if (px > (DBL)xmax + 1.0 - EPSILON)
    {
      if (px > (DBL)xmax + 1.0 + EPSILON)
      {
        Debug_Info("Illegal grid value in dda_traversal().\n");

        return(FALSE);
      }
      else
      {
        px = (DBL)xmax + 1.0 - EPSILON;
      }
    }
  }

  if (pz < (DBL)zmin)
  {
    if (pz < (DBL)zmin - HFIELD_OFFSET)
    {
      Debug_Info("Illegal grid value in dda_traversal().\n");

      return(FALSE);
    }
    else
    {
      pz = (DBL)zmin;
    }
  }
  else
  {
    if (pz > (DBL)zmax + 1.0 - EPSILON)
    {
      if (pz > (DBL)zmax + 1.0 + EPSILON)
      {
        Debug_Info("Illegal grid value in dda_traversal().\n");

        return(FALSE);
      }
      else
      {
        pz = (DBL)zmax + 1.0 - EPSILON;
      }
    }
  }

  dx = Ray->Direction[X];
  dy = Ray->Direction[Y];
  dz = Ray->Direction[Z];

  /*
   * Here comes the DDA algorithm.
   */

  /* Choose algorithm depending on the driving axis. */

  if (fabs(dx) >= fabs(dz))
  {
    /*
     * X-axis is driving axis.
     */

    delta = fabs(dz / dx);

    x = (int)px;
    z = (int)pz;

    x0 = px - floor(px);
    z0 = pz - floor(pz);

    signx = sign(dx);
    signz = sign(dz);

    /* Get initial error. */

    if (dx >= 0.0)
    {
      if (dz >= 0.0)
      {
        error = z0 + delta * (1.0 - x0) - 1.0;
      }
      else
      {
        error = -(z0 - delta * (1.0 - x0));
      }
    }
    else
    {
      if (dz >= 0.0)
      {
        error = z0 + delta * x0 - 1.0;
      }
      else
      {
        error = -(z0 - delta * x0);
      }
    }

    /* Get y differential. */

    deltay = dy / fabs(dx);

    if (dx >= 0.0)
    {
      neary = Start[Y] - x0 * deltay;

      fary = neary + deltay;
    }
    else
    {
      neary = Start[Y] - (1.0 - x0) * deltay;

      fary = neary + deltay;
    }

    /* Step through the cells. */

    do
    {
      if (neary < fary)
      {
        y1 = neary;
        y2 = fary;
      }
      else
      {
        y1 = fary;
        y2 = neary;
      }

      if (intersect_pixel(x, z, Ray, HField, y1, y2))
      {
        if (HField->Type & IS_CHILD_OBJECT)
        {
          found = TRUE;
        }
        else
        {
          return(TRUE);
        }
      }

      if (error > EPSILON)
      {
        z += signz;

        if ((z < zmin) || (z > zmax))
        {
          break;
        }
        else
        {
          if (intersect_pixel(x, z, Ray, HField, y1, y2))
          {
            if (HField->Type & IS_CHILD_OBJECT)
            {
              found = TRUE;
            }
            else
            {
              return(TRUE);
            }
          }
        }

        error--;
      }
      else
      {
        if (error > -EPSILON)
        {
          z += signz;

          error--;
        }
      }

      x += signx;

      error += delta;

      neary = fary;

      fary += deltay;
    }
    while ((neary >= ymin) && (neary <= ymax) && (x >= xmin) && (x <= xmax) && (z >= zmin) && (z <= zmax));
  }
  else
  {
    /*
     * Z-axis is driving axis.
     */

    delta = fabs(dx / dz);

    x = (int)px;
    z = (int)pz;

    x0 = px - floor(px);
    z0 = pz - floor(pz);

    signx = sign(dx);
    signz = sign(dz);

    /* Get initial error. */

    if (dz >= 0.0)
    {
      if (dx >= 0.0)
      {
        error = x0 + delta * (1.0 - z0) - 1.0;
      }
      else
      {
        error = -(x0 - delta * (1.0 - z0));
      }
    }
    else
    {
      if (dx >= 0.0)
      {
        error = x0 + delta * z0 - 1.0;
      }
      else
      {
        error = -(x0 - delta * z0);
      }
    }

    /* Get y differential. */

    deltay = dy / fabs(dz);

    if (dz >= 0.0)
    {
      neary = Start[Y] - z0 * deltay;

      fary = neary + deltay;
    }
    else
    {
      neary = Start[Y] - (1.0 - z0) * deltay;

      fary = neary + deltay;
    }

    /* Step through the cells. */

    do
    {
      if (neary < fary)
      {
        y1 = neary;
        y2 = fary;
      }
      else
      {
        y1 = fary;
        y2 = neary;
      }

      if (intersect_pixel(x, z, Ray, HField, y1, y2))
      {
        if (HField->Type & IS_CHILD_OBJECT)
        {
          found = TRUE;
        }
        else
        {
          return(TRUE);
        }
      }

      if (error > EPSILON)
      {
        x += signx;

        if ((x < xmin) || (x > xmax))
        {
          break;
        }
        else
        {
          if (intersect_pixel(x, z, Ray, HField, y1, y2))
          {
            if (HField->Type & IS_CHILD_OBJECT)
            {
              found = TRUE;
            }
            else
            {
              return(TRUE);
            }
          }
        }

        error--;
      }
      else
      {
        if (error > -EPSILON)
        {
          x += signx;

          error--;
        }
      }

      z += signz;

      error += delta;

      neary = fary;

      fary += deltay;
    }
    while ((neary >= ymin-EPSILON) && (neary <= ymax+EPSILON) &&
           (x >= xmin) && (x <= xmax) &&
           (z >= zmin) && (z <= zmax));
  }

  return(found);
}



/*****************************************************************************
*
* FUNCTION
*
*   block_traversal
*
* INPUT
*
*   Ray    - Current ray
*   HField - Height field
*   Start  - Start point for the walk
*
* OUTPUT
*
* RETURNS
*
*   int - TRUE if intersection was found
*   
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Traverse the blocks of the height field.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
*   Aug 1996 : Fixed bug as reported by Dean M. Phillips:
*              "I found a bug in the height field code which resulted
*              in "Illegal grid value in dda_traversal()." messages
*              along with dark vertical lines in the height fields.
*              This turned out to be caused by overlooking the units in
*              which some boundary tests in two different places were
*              made. It was easy to fix.
*
******************************************************************************/

static int block_traversal(RAY *Ray, HFIELD *HField, VECTOR Start)
{
  int xmax, zmax;
  int x, z, nx, nz, signx, signz;
  int found = FALSE;
  int dx_zero, dz_zero;
  DBL px, pz, dx, dy, dz;
  DBL maxdv;
  DBL ymin, ymax, y1, y2;
  DBL neary, fary;
  DBL k1, k2, dist;
  VECTOR nearP, farP;
  HFIELD_BLOCK *Block;

  px = Start[X];
  pz = Start[Z];

  dx = Ray->Direction[X];
  dy = Ray->Direction[Y];
  dz = Ray->Direction[Z];

  maxdv = (dx > dz) ? dx : dz;

  /* First test for 'perpendicular' rays. */

  if ((fabs(dx) < EPSILON) && (fabs(dz) < EPSILON))
  {
    x = (int)px;
    z = (int)pz;

    neary = Start[Y];

    if (dy >= 0.0)
    {
      fary = 65536.0;
    }
    else
    {
      fary = 0.0;
    }

    return(intersect_pixel(x, z, Ray, HField, min(neary, fary), max(neary, fary)));
  }

  /* If we don't have blocks we just step through the grid. */

  if ((HField->Data->block_max_x <= 1) && (HField->Data->block_max_z <= 1))
  {
    return(dda_traversal(Ray, HField, Start, &HField->Data->Block[0][0]));
  }

  /* Get dimensions of grid. */

  xmax = HField->Data->block_max_x;
  zmax = HField->Data->block_max_z;

  ymin = (DBL)HField->Data->min_y - EPSILON;
  ymax = (DBL)HField->Data->max_y + EPSILON;

  dx_zero = (fabs(dx) < EPSILON);
  dz_zero = (fabs(dz) < EPSILON);

  signx = sign(dx);
  signz = sign(dz);

  /* Walk on the block grid. */

  px /= HField->Data->block_width_x;
  pz /= HField->Data->block_width_z;

  x = (int)px;
  z = (int)pz;

  Assign_Vector(nearP, Start);

  neary = Start[Y];

  /*
   * Here comes the block walk algorithm.
   */

  do
  {
#ifdef HFIELD_EXTRA_STATS
    Increase_Counter(stats[Ray_HField_Block_Tests]);
#endif

    /* Get current block. */

    Block = &HField->Data->Block[z][x];

    /* Intersect ray with bounding planes. */

    if (dx_zero)
    {
      k1 = BOUND_HUGE;
    }
    else
    {
      if (signx >= 0)
      {
        k1 = ((DBL)Block->xmax + 1.0 - Ray->Initial[X]) / dx;
      }
      else
      {
        k1 = ((DBL)Block->xmin - Ray->Initial[X]) / dx;
      }
    }

    if (dz_zero)
    {
      k2 = BOUND_HUGE;
    }
    else
    {
      if (signz >= 0)
      {
        k2 = ((DBL)Block->zmax + 1.0 - Ray->Initial[Z]) / dz;
      }
      else
      {
        k2 = ((DBL)Block->zmin - Ray->Initial[Z]) / dz;
      }
    }

    /* Figure out the indices of the next block. */

    if (dz_zero || ((!dx_zero) && (k1<k2 - EPSILON / maxdv) && (k1>0.0)))
/*  if ((k1 < k2 - EPSILON / maxdv) && (k1 > 0.0)) */
    {
      /* Step along the x-axis. */

      dist = k1;

      nx = x + signx;
      nz = z;
    }
    else
    {
      if (dz_zero || ((!dx_zero) && (k1<k2 + EPSILON / maxdv) && (k1>0.0)))
/*    if ((k1 < k2 + EPSILON / maxdv) && (k1 > 0.0))  */
      {
        /* Step along both axis (very rare case). */

        dist = k1;

        nx = x + signx;
        nz = z + signz;
      }
      else
      {
        /* Step along the z-axis. */

        dist = k2;

        nx = x;
        nz = z + signz;
      }
    }

    /* Get point where ray leaves current block. */

    VEvaluateRay(farP, Ray->Initial, dist, Ray->Direction);

    fary = farP[Y];

    if (neary < fary)
    {
      y1 = neary;
      y2 = fary;
    }
    else
    {
      y1 = fary;
      y2 = neary;
    }

    /* Can we hit current block at all? */

    if ((y1 <= (DBL)Block->ymax + EPSILON) && (y2 >= (DBL)Block->ymin - EPSILON))
    {
      /* Test current block. */

#ifdef HFIELD_EXTRA_STATS
      Increase_Counter(stats[Ray_HField_Block_Tests_Succeeded]);
#endif

      if (dda_traversal(Ray, HField, nearP, &HField->Data->Block[z][x]))
      {
        if (HField->Type & IS_CHILD_OBJECT)
        {
          found = TRUE;
        }
        else
        {
          return(TRUE);
        }
      }
    }

    /* Step to next block. */

    x = nx;
    z = nz;

    Assign_Vector(nearP, farP);

    neary = fary;
  }
  while ((x >= 0) && (x < xmax) && (z >= 0) && (z < zmax) && (neary >= ymin) && (neary <= ymax));

  return(found);
}

