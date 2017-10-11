/****************************************************************************
*                   polygon.c
*
*  This module implements functions that manipulate polygons.
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
*  Syntax:
*
*  ---
*
*  The "inside polygon"-test was taken from:
*
*    E. Haines, "An Introduction to Ray-Tracing", ..., pp. 53-59
*
*  ---
*
*  May 1994 : Creation. [DB]
*
*  Oct 1994 : Changed polygon structure. Polygon points are now stored
*             in a seperate structure. This - together with the use of
*             a transformation - allows to keep just one point definition
*             for multiple copies of one polygon. [DB]
*
*****************************************************************************/

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "matrices.h"
#include "objects.h"
#include "polygon.h"
#include "povray.h"




/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Minimal intersection depth for a valid intersection. */

#define DEPTH_TOLERANCE 1.0e-8

/* If |x| < ZERO_TOLERANCE x is assumed to be 0. */

#define ZERO_TOLERANCE 1.0e-10




/*****************************************************************************
* Static functions
******************************************************************************/

static int intersect_poylgon (RAY *Ray, POLYGON *Polyg, DBL *Depth);
static int in_polygon (int Number, UV_VECT *Points, DBL u, DBL v);
static int  All_Polygon_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int  Inside_Polygon (VECTOR point, OBJECT *Object);
static void Polygon_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static POLYGON *Copy_Polygon (OBJECT *Object);
static void Translate_Polygon (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Polygon (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Polygon (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Polygon (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Polygon (OBJECT *Object);
static void Destroy_Polygon (OBJECT *Object);
static void Compute_Polygon_BBox (POLYGON *Polyg);



/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Polygon_Methods =
{
  All_Polygon_Intersections,
  Inside_Polygon, Polygon_Normal,
  (COPY_METHOD)Copy_Polygon,
  Translate_Polygon, Rotate_Polygon,
  Scale_Polygon, Transform_Polygon, Invert_Polygon, Destroy_Polygon
};



/*****************************************************************************
*
* FUNCTION
*
*   All_Polygon_Intersections
*
* INPUT
*
*   Object      - Object
*   Ray         - Ray
*   Depth_Stack - Intersection stack
*   
* OUTPUT
*
*   Depth_Stack
*   
* RETURNS
*
*   int - TRUE, if a intersection was found
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Determine ray/polygon intersection and clip intersection found.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int All_Polygon_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  DBL Depth;
  VECTOR IPoint;

  if (intersect_poylgon(Ray, (POLYGON *)Object, &Depth))
  {
    VEvaluateRay(IPoint, Ray->Initial, Depth, Ray->Direction);

    if (Point_In_Clip(IPoint, Object->Clip))
    {
      push_entry(Depth, IPoint, Object, Depth_Stack);

      return(TRUE);
    }
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_poylgon
*
* INPUT
*
*   Ray     - Ray
*   Polyg - Polygon
*   Depth   - Depth of intersection found
*   
* OUTPUT
*
*   Depth
*   
* RETURNS
*
*   int - TRUE if intersection found
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Determine ray/polygon intersection.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int intersect_poylgon(RAY *Ray, POLYGON *Polyg, DBL *Depth)
{
  DBL x, y, len;
  VECTOR p, d;

  /* Don't test degenerate polygons. */

  if (Test_Flag(Polyg, DEGENERATE_FLAG))
  {
    return(FALSE);
  }

  Increase_Counter(stats[Ray_Polygon_Tests]);

  /* Transform the ray into the polygon space. */

  MInvTransPoint(p, Ray->Initial, Polyg->Trans);

  MInvTransDirection(d, Ray->Direction, Polyg->Trans);

  VLength(len, d);

  VInverseScaleEq(d, len);

  /* Intersect ray with the plane in which the polygon lies. */

  if (fabs(d[Z]) < ZERO_TOLERANCE)
  {
    return(FALSE);
  }

  *Depth = -p[Z] / d[Z];

  if ((*Depth < DEPTH_TOLERANCE) || (*Depth > Max_Distance))
  {
    return(FALSE);
  }

  /* Does the intersection point lie inside the polygon? */

  x = p[X] + *Depth * d[X];
  y = p[Y] + *Depth * d[Y];

  if (in_polygon(Polyg->Data->Number, Polyg->Data->Points, x, y))
  {
    Increase_Counter(stats[Ray_Polygon_Tests_Succeeded]);

    *Depth /= len;

    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Polygon
*
* INPUT
*
*   IPoint - Intersection point
*   Object - Object
*   
* OUTPUT
*   
* RETURNS
*
*   int - always FALSE
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Polygons can't be used in CSG.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int Inside_Polygon(VECTOR IPoint, OBJECT *Object)
{
  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Polygon_Normal
*
* INPUT
*
*   Result - Normal vector
*   Object - Object
*   Inter  - Intersection found
*   
* OUTPUT
*
*   Result
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the normal of the polygon in a given point.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Polygon_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  Assign_Vector(Result, ((POLYGON *)Object)->S_Normal);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Polygon
*
* INPUT
*
*   Object - Object
*   Vector - Translation vector
*   
* OUTPUT
*
*   Object
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Translate a polygon.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Translate_Polygon(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Polygon(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Polygon
*
* INPUT
*
*   Object - Object
*   Vector - Rotation vector
*   
* OUTPUT
*
*   Object
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Rotate a polygon.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Rotate_Polygon(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Polygon(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Polygon
*
* INPUT
*
*   Object - Object
*   Vector - Scaling vector
*   
* OUTPUT
*
*   Object
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Scale a polygon.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Scale_Polygon(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Polygon(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Polygon
*
* INPUT
*
*   Object - Object
*   Trans  - Transformation to apply
*   
* OUTPUT
*
*   Object
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Transform a polygon by transforming all points
*   and recalculating the polygon.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Transform_Polygon(OBJECT *Object, TRANSFORM *Trans)
{
  VECTOR N;
  POLYGON *Polyg = (POLYGON *)Object;

  Compose_Transforms(Polyg->Trans, Trans);

  Make_Vector(N, 0.0, 0.0, 1.0);
  MTransNormal(Polyg->S_Normal, N, Polyg->Trans);

  VNormalizeEq(Polyg->S_Normal);

  Compute_Polygon_BBox(Polyg);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Polygon
*
* INPUT
*
*   Object - Object
*   
* OUTPUT
*
*   Object
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Invert a polygon.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Invert_Polygon(OBJECT *Object)
{
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Polygon
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*
*   POLYGON * - new polygon
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Create a new polygon.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

POLYGON *Create_Polygon()
{
  POLYGON *New;

  New = (POLYGON *)POV_MALLOC(sizeof(POLYGON), "polygon");

  INIT_OBJECT_FIELDS(New,POLYGON_OBJECT,&Polygon_Methods)

  New->Trans = Create_Transform();

  Make_Vector(New->S_Normal, 0.0, 0.0, 1.0);

  New->Data = NULL;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Polygon
*
* INPUT
*
*   Object - Object
*   
* OUTPUT
*   
* RETURNS
*
*   void * - New polygon
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Copy a polygon structure.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static POLYGON *Copy_Polygon(OBJECT *Object)
{
  POLYGON *New, *Polyg = (POLYGON *)Object;

  New = Create_Polygon ();

  /* Get rid of the transformation created in Create_Polygon(). */

  Destroy_Transform(New->Trans);

  /* Copy polygon. */

  *New = *Polyg;

  New->Trans = Copy_Transform(Polyg->Trans);

  New->Data->References++;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Polygon
*
* INPUT
*
*   Object - Object
*   
* OUTPUT
*
*   Object
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Destroy a polygon.
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Dec 1994 : Fixed memory leakage. [DB]
*
******************************************************************************/

static void Destroy_Polygon(OBJECT *Object)
{
  POLYGON *Polyg = (POLYGON *)Object;

  if (--(Polyg->Data->References) == 0)
  {
    POV_FREE (Polyg->Data->Points);

    POV_FREE (Polyg->Data);
  }

  Destroy_Transform(Polyg->Trans);

  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Polygon
*
* INPUT
*
*   Polyg - Polygon
*   Points  - 3D points describing the polygon
*   
* OUTPUT
*
*   Polyg
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Compute the following things for a given polygon:
*
*     - Polygon transformation
*
*     - Array of 2d points describing the shape of the polygon
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Compute_Polygon(POLYGON *Polyg, int Number, VECTOR *Points)
{
  int i;
  DBL x, y, z, d;
  VECTOR o, u, v, w, N;
  MATRIX a, b;

  /* Create polygon data. */

  if (Polyg->Data == NULL)
  {
    Polyg->Data = (POLYGON_DATA *)POV_MALLOC(sizeof(POLYGON_DATA), "polygon points");

    Polyg->Data->References = 1;

    Polyg->Data->Number = Number;

    Polyg->Data->Points = (UV_VECT *)POV_MALLOC(Number*sizeof(UV_VECT), "polygon points");
  }
  else
  {
    Error("Polygon data already computed.");
  }

  /* Get polygon's coordinate system (one of the many possible) */

  Assign_Vector(o, Points[0]);

  /* Find valid, i.e. non-zero u vector. */
  
  for (i = 1; i < Number; i++)
  {
    VSub(u, Points[i], o);

    if (VSumSqr(u) > EPSILON)
    {
      break;
    }
  }

  if (i == Number)
  {
    Set_Flag(Polyg, DEGENERATE_FLAG);

    Warning(0.0, "Points in polygon are co-linear. Ignoring polygon.\n");
  }

  /* Find valid, i.e. non-zero v and w vectors. */
  
  for (i++; i < Number; i++)
  {
    VSub(v, Points[i], o);
    
    VCross(w, u, v);

    if ((VSumSqr(v) > EPSILON) && (VSumSqr(w) > EPSILON))
    {
      break;
    }
  }

  if (i == Number)
  {
    Set_Flag(Polyg, DEGENERATE_FLAG);

    Warning(0.0, "Points in polygon are co-linear. Ignoring polygon.\n");
  }

  VCross(u, v, w);
  VCross(v, w, u);

  VNormalize(u, u);
  VNormalize(v, v);
  VNormalize(w, w);

  MIdentity(a);
  MIdentity(b);

  a[3][0] = -o[X];
  a[3][1] = -o[Y];
  a[3][2] = -o[Z];

  b[0][0] =  u[X];
  b[1][0] =  u[Y];
  b[2][0] =  u[Z];

  b[0][1] =  v[X];
  b[1][1] =  v[Y];
  b[2][1] =  v[Z];

  b[0][2] =  w[X];
  b[1][2] =  w[Y];
  b[2][2] =  w[Z];

  MTimes(Polyg->Trans->inverse, a, b);

  MInvers(Polyg->Trans->matrix, Polyg->Trans->inverse);

  /* Project points onto the u,v-plane (3D --> 2D) */

  for (i = 0; i < Number; i++)
  {
    x = Points[i][X] - o[X];
    y = Points[i][Y] - o[Y];
    z = Points[i][Z] - o[Z];

    d = x * w[X] + y * w[Y] + z * w[Z];

    if (fabs(d) > ZERO_TOLERANCE)
    {
      Set_Flag(Polyg, DEGENERATE_FLAG);

      Warning(0.0, "Points in polygon are not co-planar. Ignoring polygons.\n");
    }

    Polyg->Data->Points[i][X] = x * u[X] + y * u[Y] + z * u[Z];
    Polyg->Data->Points[i][Y] = x * v[X] + y * v[Y] + z * v[Z];
  }
  
  Make_Vector(N, 0.0, 0.0, 1.0);
  MTransNormal(Polyg->S_Normal, N, Polyg->Trans);

  VNormalizeEq(Polyg->S_Normal);

  Compute_Polygon_BBox(Polyg);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Polygon_BBox
*
* INPUT
*
*   Polyg - Polygon
*   
* OUTPUT
*
*   Polyg
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a polygon.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Compute_Polygon_BBox(POLYGON *Polyg)
{
  int i;
  VECTOR p, Puv, Min, Max;

  Min[X] = Min[Y] = Min[Z] =  BOUND_HUGE;
  Max[X] = Max[Y] = Max[Z] = -BOUND_HUGE;

  for (i = 0; i < Polyg->Data->Number; i++)
  {
    Puv[X] = Polyg->Data->Points[i][X];
    Puv[Y] = Polyg->Data->Points[i][Y];
    Puv[Z] = 0.0;

    MTransPoint(p, Puv, Polyg->Trans);

    Min[X] = min(Min[X], p[X]);
    Min[Y] = min(Min[Y], p[Y]);
    Min[Z] = min(Min[Z], p[Z]);
    Max[X] = max(Max[X], p[X]);
    Max[Y] = max(Max[Y], p[Y]);
    Max[Z] = max(Max[Z], p[Z]);
  }

  Make_BBox_from_min_max(Polyg->BBox, Min, Max);

  if (fabs(Polyg->BBox.Lengths[X]) < Small_Tolerance)
  {
    Polyg->BBox.Lower_Left[X] -= Small_Tolerance;
    Polyg->BBox.Lengths[X]    += 2.0 * Small_Tolerance;
  }

  if (fabs(Polyg->BBox.Lengths[Y]) < Small_Tolerance)
  {
    Polyg->BBox.Lower_Left[Y] -= Small_Tolerance;
    Polyg->BBox.Lengths[Y]    += 2.0 * Small_Tolerance;
  }

  if (fabs(Polyg->BBox.Lengths[Z]) < Small_Tolerance)
  {
    Polyg->BBox.Lower_Left[Z] -= Small_Tolerance;
    Polyg->BBox.Lengths[Z]    += 2.0 * Small_Tolerance;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   in_polygon
*
* INPUT
*
*   Number - Number of points
*   Points - Points describing polygon's shape
*   u, v   - 2D-coordinates of the point to test
*   
* OUTPUT
*   
* RETURNS
*
*   int - TRUE, if inside
*   
* AUTHOR
*
*   Eric Haines, 3D/Eye Inc, erich@eye.com
*   
* DESCRIPTION
*
* ======= Crossings Multiply algorithm ===================================
*
* This version is usually somewhat faster than the original published in
* Graphics Gems IV; by turning the division for testing the X axis crossing
* into a tricky multiplication test this part of the test became faster,
* which had the additional effect of making the test for "both to left or
* both to right" a bit slower for triangles than simply computing the
* intersection each time.  The main increase is in triangle testing speed,
* which was about 15% faster; all other polygon complexities were pretty much
* the same as before.  On machines where division is very expensive (not the
* case on the HP 9000 series on which I tested) this test should be much
* faster overall than the old code.  Your mileage may (in fact, will) vary,
* depending on the machine and the test data, but in general I believe this
* code is both shorter and faster.  This test was inspired by unpublished
* Graphics Gems submitted by Joseph Samosky and Mark Haigh-Hutchinson.
* Related work by Samosky is in:
*
* Samosky, Joseph, "SectionView: A system for interactively specifying and
* visualizing sections through three-dimensional medical image data",
* M.S. Thesis, Department of Electrical Engineering and Computer Science,
* Massachusetts Institute of Technology, 1993.
*
*
* Shoot a test ray along +X axis.  The strategy is to compare vertex Y values
* to the testing point's Y and quickly discard edges which are entirely to one
* side of the test ray.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int in_polygon(int Number, UV_VECT *Points, DBL u, DBL  v)
{
  register int i, yflag0, yflag1, inside_flag;
  register DBL ty, tx;
  register DBL *vtx0, *vtx1, *first;

  tx = u;
  ty = v;

  vtx0 = &Points[0][X];
  vtx1 = &Points[1][X];

  first = vtx0;

  /* get test bit for above/below X axis */

  yflag0 = (vtx0[Y] >= ty);

  inside_flag = FALSE;

  for (i = 1; i < Number; )
  {
    yflag1 = (vtx1[Y] >= ty);

    /*
     * Check if endpoints straddle (are on opposite sides) of X axis
     * (i.e. the Y's differ); if so, +X ray could intersect this edge.
     * The old test also checked whether the endpoints are both to the
     * right or to the left of the test point.  However, given the faster
     * intersection point computation used below, this test was found to
     * be a break-even proposition for most polygons and a loser for
     * triangles (where 50% or more of the edges which survive this test
     * will cross quadrants and so have to have the X intersection computed
     * anyway).  I credit Joseph Samosky with inspiring me to try dropping
     * the "both left or both right" part of my code.
     */

    if (yflag0 != yflag1)
    {
      /*
       * Check intersection of pgon segment with +X ray.
       * Note if >= point's X; if so, the ray hits it.
       * The division operation is avoided for the ">=" test by checking
       * the sign of the first vertex wrto the test point; idea inspired
       * by Joseph Samosky's and Mark Haigh-Hutchinson's different
       * polygon inclusion tests.
       */

      if (((vtx1[Y]-ty) * (vtx0[X]-vtx1[X]) >= (vtx1[X]-tx) * (vtx0[Y]-vtx1[Y])) == yflag1)
      {
        inside_flag = !inside_flag;
      }
    }

    /* Move to the next pair of vertices, retaining info as possible. */

    if ((i < Number-2) && (vtx1[X] == first[X]) && (vtx1[Y] == first[Y]))
    {
      vtx0 = &Points[++i][X];
      vtx1 = &Points[++i][X];

      yflag0 = (vtx0[Y] >= ty);

      first = vtx0;
    }
    else
    {
      vtx0 = vtx1;
      vtx1 = &Points[++i][X];

      yflag0 = yflag1;
    }
  }

  return(inside_flag);
}

