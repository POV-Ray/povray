/****************************************************************************
*                   sor.c
*
*  This module implements functions that manipulate surfaces of revolution.
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
*    The surface of revolution primitive is defined by a set of points
*    in 2d space wich are interpolated by cubic splines. The resulting
*    2d function is rotated about an axis to form the final object.
*
*    All calculations are done in the object's (u,v,w)-coordinate system
*    with the (w)-axis being the rotation axis.
*
*    One spline segment in the (r,w)-plane is given by the equation
*
*      r^2 = f(w) = A * w * w * w + B * w * w + C * w + D.
*
*    To intersect a ray R = P + k * D transformed into the object's
*    coordinate system with the surface of revolution, the equation
*
*      (Pu + k * Du)^2 + (Pv + k * Dv)^2 = f(Pw + k * Dw)
*
*    has to be solved for k (cubic polynomial in k).
*
*    Note that Pu, Pv, Pw and Du, Dv, Dw denote the coordinates
*    of the vectors P and D.
*
*  Syntax:
*
*    revolution
*    {
*      number_of_points,
*
*      <P[0]>, <P[1]>, ..., <P[n-1]>
*
*      [ open ]
*    }
*
*    Note that the P[i] are 2d vectors where u corresponds to the radius
*    and v to the height.
*
*    Note that the first and last point, i.e. P[0] and P[n-1], are used
*    to determine the derivatives at the end point.
*
*    Note that the x coordinate of a point corresponds to the radius and
*    the y coordinate to the height; the z coordinate isn't used.
*
*  ---
*
*  Ideas for the surface of revolution were taken from:
*
*    P. Burger and D. Gillies, "Rapid Ray Tracing of General Surfaces
*    of Revolution", New Advances in Computer Graphics, Proceedings
*    of CG International '89, R. A. Earnshaw, B. Wyvill (Eds.),
*    Springer, ..., pp. 523-531
*
*  ---
*
*  May 1994 : Creation. [DB]
*
*****************************************************************************/

#include "frame.h"
#include "povray.h"
#include "vector.h"
#include "povproto.h"
#include "bbox.h"
#include "polysolv.h"
#include "matrices.h"
#include "objects.h"
#include "sor.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Minimal intersection depth for a valid intersection. */

#define DEPTH_TOLERANCE 1.0e-4

/* |Coefficients| < COEFF_LIMIT are regarded to be 0. */

#define COEFF_LIMIT 1.0e-20

/* Part of the surface of revolution hit. */

#define BASE_PLANE 1
#define CAP_PLANE  2
#define CURVE      3

/* Max. number of intersecions per spline segment. */

#define MAX_INTERSECTIONS_PER_SEGMENT 4



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int  intersect_sor (RAY *Ray, SOR *Sor, ISTACK *Depth_Stack);
static int  All_Sor_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int  Inside_Sor (VECTOR point, OBJECT *Object);
static void Sor_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static SOR  *Copy_Sor (OBJECT *Object);
static void Translate_Sor (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Sor (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Sor (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Sor (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Sor (OBJECT *Object);
static void Destroy_Sor (OBJECT *Object);

static int test_hit (SOR *, RAY *, ISTACK *, DBL, int, int);

/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Sor_Methods =
{
  All_Sor_Intersections,
  Inside_Sor, Sor_Normal,
  (COPY_METHOD)Copy_Sor,
  Translate_Sor, Rotate_Sor,
  Scale_Sor, Transform_Sor, Invert_Sor, Destroy_Sor
};



/*****************************************************************************
*
* FUNCTION
*
*   All_Sor_Intersections
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
*   Determine ray/surface of revolution intersection and
*   clip intersection found.
*
* CHANGES
*
*   May 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

static int All_Sor_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  Increase_Counter(stats[Ray_Sor_Tests]);

  if (intersect_sor(Ray, (SOR *)Object, Depth_Stack))
  {
    Increase_Counter(stats[Ray_Sor_Tests_Succeeded]);

    return(TRUE);
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_sor
*
* INPUT
*
*   Ray          - Ray
*   Sor   - Sor
*   Intersection - Sor intersection structure
*   
* OUTPUT
*
*   Intersection
*   
* RETURNS
*
*   int - Number of intersections found
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Determine ray/surface of revolution intersection.
*
*   NOTE: The curve is rotated about the y-axis!
*         Order reduction cannot be used.
*
* CHANGES
*
*   May 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

static int intersect_sor(RAY *Ray, SOR *Sor, ISTACK *Depth_Stack)
{
  int cnt;
  int found, j, n;
  DBL a, b, k, h, len, u, v, r0;
  DBL x[4], y[3];
  DBL best;
  VECTOR P, D;
  BCYL_INT *intervals;
  SOR_SPLINE_ENTRY *Entry;

  /* Transform the ray into the surface of revolution space. */

  MInvTransPoint(P, Ray->Initial, Sor->Trans);

  MInvTransDirection(D, Ray->Direction, Sor->Trans);

  VLength(len, D);

  VInverseScaleEq(D, len);

  /* Test if ray misses object's bounds. */

#ifdef SOR_EXTRA_STATS
  Increase_Counter(stats[Sor_Bound_Tests]);
#endif

  if (((D[Y] >= 0.0) && (P[Y] >  Sor->Height2)) ||
      ((D[Y] <= 0.0) && (P[Y] <  Sor->Height1)) ||
      ((D[X] >= 0.0) && (P[X] >  Sor->Radius2)) ||
      ((D[X] <= 0.0) && (P[X] < -Sor->Radius2)))
  {
    return(FALSE);
  }

  /* Get distance of the ray from rotation axis (= y axis). */

  r0 = P[X] * D[Z] - P[Z] * D[X];

  if ((a = D[X] * D[X] + D[Z] * D[Z]) > 0.0)
  {
    r0 /= sqrt(a);
  }

  /* Test if ray misses object's bounds. */

  if (r0 > Sor->Radius2)
  {
    return(FALSE);
  }

  /* Intersect all cylindrical bounds. */

  if ((cnt = Intersect_BCyl(Sor->Spline->BCyl, P, D)) == 0)
  {
    return(FALSE);
  }

#ifdef SOR_EXTRA_STATS
  Increase_Counter(stats[Sor_Bound_Tests_Succeeded]);
#endif

  /* Test base/cap plane. */

  found = FALSE;

  best = BOUND_HUGE;

  if (Test_Flag(Sor, CLOSED_FLAG) && (fabs(D[Y]) > EPSILON))
  {
    /* Test base plane. */

    if (Sor->Base_Radius_Squared > DEPTH_TOLERANCE)
    {
      k = (Sor->Height1 - P[Y]) / D[Y];

      u = P[X] + k * D[X];
      v = P[Z] + k * D[Z];

      b = u * u + v * v;

      if (b <= Sor->Base_Radius_Squared)
      {
        if (test_hit(Sor, Ray, Depth_Stack, k / len, BASE_PLANE, 0))
        {
          found = TRUE;

          if (k < best)
          {
            best = k;
          }
        }
      }
    }

    /* Test cap plane. */

    if (Sor->Cap_Radius_Squared > DEPTH_TOLERANCE)
    {
      k = (Sor->Height2 - P[Y]) / D[Y];

      u = P[X] + k * D[X];
      v = P[Z] + k * D[Z];

      b = u * u + v * v;

      if (b <= Sor->Cap_Radius_Squared)
      {
        if (test_hit(Sor, Ray, Depth_Stack, k / len, CAP_PLANE, 0))
        {
          found = TRUE;

          if (k < best)
          {
            best = k;
          }
        }
      }
    }
  }

  /* Step through the list of intersections. */

  intervals = Sor->Spline->BCyl->intervals;

  for (j = 0; j < cnt; j++)
  {
    /* Get current segment. */

    Entry = &Sor->Spline->Entry[intervals[j].n];

    /* If we already have the best intersection we may exit. */

    if (!(Sor->Type & IS_CHILD_OBJECT) && (intervals[j].d[0] > best))
    {
      break;
    }

    /* Cubic curve. */

    x[0] = Entry->A * D[Y] * D[Y] * D[Y];

/*
    x[1] = D[Y] * D[Y] * (3.0 * Entry->A * P[Y] + Entry->B) - D[X] * D[X] - D[Z] * D[Z];
*/
    x[1] = D[Y] * D[Y] * (3.0 * Entry->A * P[Y] + Entry->B) - a;

    x[2] = D[Y] * (P[Y] * (3.0 * Entry->A * P[Y] + 2.0 * Entry->B) + Entry->C) - 2.0 * (P[X] * D[X] + P[Z] * D[Z]);

    x[3] = P[Y] * (P[Y] * (Entry->A * P[Y] + Entry->B) + Entry->C) + Entry->D - P[X] * P[X] - P[Z] * P[Z];

    n = Solve_Polynomial(3, x, y, Test_Flag(Sor, STURM_FLAG), 0.0);

    while (n--)
    {
      k = y[n];

      h = P[Y] + k * D[Y];

      if ((h >= Sor->Spline->BCyl->height[Sor->Spline->BCyl->entry[intervals[j].n].h1]) &&
          (h <= Sor->Spline->BCyl->height[Sor->Spline->BCyl->entry[intervals[j].n].h2]))
      {
        if (test_hit(Sor, Ray, Depth_Stack, k / len, CURVE, intervals[j].n))
        {
          found = TRUE;

          if (y[n] < best)
          {
            best = k;
          }
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
*   Inside_Sor
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
*   int - TRUE if inside
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Return true if point lies inside the surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int Inside_Sor(VECTOR IPoint, OBJECT *Object)
{
  int i;
  DBL r0, r;
  VECTOR P;
  SOR *Sor = (SOR *)Object;
  SOR_SPLINE_ENTRY *Entry;

  /* Transform the point into the surface of revolution space. */

  MInvTransPoint(P, IPoint, Sor->Trans);

  /* Test if we are inside the cylindrical bound. */

  if ((P[Y] >= Sor->Height1) && (P[Y] <= Sor->Height2))
  {
    r0 = P[X] * P[X] + P[Z] * P[Z];

    /* Test if we are inside the cylindrical bound. */

    if (r0 <= Sqr(Sor->Radius2))
    {
      /* Now find the segment the point is in. */

      for (i = 0; i < Sor->Number; i++)
      {
        Entry = &Sor->Spline->Entry[i];

        if ((P[Y] >= Sor->Spline->BCyl->height[Sor->Spline->BCyl->entry[i].h1]) &&
            (P[Y] <= Sor->Spline->BCyl->height[Sor->Spline->BCyl->entry[i].h2]))
        {
          break;
        }
      }

      /* Have we found any segment? */

      if (i < Sor->Number)
      {
        r = P[Y] * (P[Y] * (P[Y] * Entry->A + Entry->B) + Entry->C) + Entry->D;

        if (r0 <= r)
        {
          /* We're inside. */

          return(!Test_Flag(Sor, INVERTED_FLAG));
        }
      }
    }
  }

  /* We're outside. */

  return(Test_Flag(Sor, INVERTED_FLAG));
}



/*****************************************************************************
*
* FUNCTION
*
*   Sor_Normal
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
*   Calculate the normal of the surface of revolution in a given point.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Sor_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  DBL k;
  VECTOR P;
  SOR *Sor = (SOR *)Object;
  SOR_SPLINE_ENTRY *Entry;
  VECTOR N;

  switch (Inter->i1)
  {
    case CURVE:

      /* Transform the intersection point into the surface of revolution space. */

      MInvTransPoint(P, Inter->IPoint, Sor->Trans);

      if (P[X] * P[X] + P[Z] * P[Z] > DEPTH_TOLERANCE)
      {
        Entry = &Sor->Spline->Entry[Inter->i2];

        k = 0.5 * (P[Y] * (3.0 * Entry->A * P[Y] + 2.0 * Entry->B) + Entry->C);

        N[X] = P[X];
        N[Y] = -k;
        N[Z] = P[Z];
      }

      break;

    case BASE_PLANE:

      Make_Vector(N, 0.0, -1.0, 0.0);

      break;


    case CAP_PLANE:

      Make_Vector(N, 0.0, 1.0, 0.0);

      break;
  }

  /* Transform the normal out of the surface of revolution space. */

  MTransNormal(Result, N, Sor->Trans);

  VNormalize(Result, Result);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Sor
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
*   Translate a surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Translate_Sor(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Sor(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Sor
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
*   Rotate a surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Rotate_Sor(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Sor(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Sor
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
*   Scale a surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Scale_Sor(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Sor(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Sor
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
*   Transform a surface of revolution and recalculate its bounding box.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Transform_Sor(OBJECT *Object, TRANSFORM *Trans)
{
  Compose_Transforms(((SOR *)Object)->Trans, Trans);

  Compute_Sor_BBox((SOR *)Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Sor
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
*   Invert a surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Invert_Sor(OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Sor
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*
*   SOR * - new surface of revolution
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Create a new surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

SOR *Create_Sor()
{
  SOR *New;

  New = (SOR *)POV_MALLOC(sizeof(SOR), "surface of revolution");

  INIT_OBJECT_FIELDS(New,SOR_OBJECT,&Sor_Methods)

  New->Trans = Create_Transform();

  New->Spline = NULL;

  New->Radius2             =
  New->Base_Radius_Squared =
  New->Cap_Radius_Squared  = 0.0;

  /* SOR should have capped ends by default. CEY 3/98*/

  Set_Flag(New, CLOSED_FLAG);

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Sor
*
* INPUT
*
*   Object - Object
*   
* OUTPUT
*   
* RETURNS
*
*   void * - New surface of revolution
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Copy a surface of revolution structure.
*
*   NOTE: The splines are not copied, only the number of references is
*         counted, so that Destray_Sor() knows if they can be destroyed.
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Sep 1994 : fixed memory leakage [DB]
*
******************************************************************************/

static SOR *Copy_Sor(OBJECT *Object)
{
  SOR *New, *Sor = (SOR *)Object;

  New = Create_Sor();

  /* Get rid of the transformation created in Create_Sor(). */

  Destroy_Transform(New->Trans);

  /* Copy surface of revolution. */

  *New = *Sor;

  New->Trans = Copy_Transform(Sor->Trans);

  New->Spline->References++;

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Sor
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
*   Destroy a surface of revolution.
*
*   NOTE: The splines are destroyed if they are no longer used by any copy.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Destroy_Sor (OBJECT *Object)
{
  SOR *Sor = (SOR *)Object;

  Destroy_Transform(Sor->Trans);

  if (--(Sor->Spline->References) == 0)
  {
    Destroy_BCyl(Sor->Spline->BCyl);

    POV_FREE(Sor->Spline->Entry);

    POV_FREE(Sor->Spline);
  }

  POV_FREE(Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Sor_BBox
*
* INPUT
*
*   Sor - Sor
*   
* OUTPUT
*
*   Sor
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a surface of revolution.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Compute_Sor_BBox(SOR *Sor)
{
  Make_BBox(Sor->BBox, -Sor->Radius2, Sor->Height1, -Sor->Radius2,
    2.0 * Sor->Radius2, Sor->Height2 - Sor->Height1, 2.0 * Sor->Radius2);

  Recompute_BBox(&Sor->BBox, Sor->Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Sor
*
* INPUT
*
*   Sor - Sor
*   P          - Points defining surface of revolution
*   
* OUTPUT
*
*   Sor
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer, June 1994
*   
* DESCRIPTION
*
*   Calculate the spline segments of a surface of revolution
*   from a set of points.
*
*   Note that the number of points in the surface of revolution has to be set.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Compute_Sor(SOR *Sor, UV_VECT *P)
{
  int i, n;
  DBL *tmp_r1;
  DBL *tmp_r2;
  DBL *tmp_h1;
  DBL *tmp_h2;
  DBL A, B, C, D, w, k[4];
  DBL x[4], xmax, xmin;
  DBL y[2], ymax, ymin;
  DBL c[3], r[2];
  MATRIX Mat;

  /* Allocate Sor->Number segments. */

  if (Sor->Spline == NULL)
  {
    Sor->Spline = (SOR_SPLINE *)POV_MALLOC(sizeof(SOR_SPLINE), "spline segments of surface of revoluion");

    Sor->Spline->References = 1;

    Sor->Spline->Entry = (SOR_SPLINE_ENTRY *)POV_MALLOC(Sor->Number*sizeof(SOR_SPLINE_ENTRY), "spline segments of surface of revoluion");
  }
  else
  {
    Error("Surface of revolution segments are already defined.\n");
  }

  /* Allocate temporary lists. */

  tmp_r1 = (DBL *)POV_MALLOC(Sor->Number * sizeof(DBL), "temp lathe data");
  tmp_r2 = (DBL *)POV_MALLOC(Sor->Number * sizeof(DBL), "temp lathe data");
  tmp_h1 = (DBL *)POV_MALLOC(Sor->Number * sizeof(DBL), "temp lathe data");
  tmp_h2 = (DBL *)POV_MALLOC(Sor->Number * sizeof(DBL), "temp lathe data");

  /* We want to know the size of the overall bounding cylinder. */

  xmax = ymax = -BOUND_HUGE;
  xmin = ymin =  BOUND_HUGE;

  /* Calculate segments, i.e. cubic patches. */

  for (i = 0; i < Sor->Number; i++)
  {
    if ((fabs(P[i+2][Y] - P[i][Y]) < EPSILON) ||
        (fabs(P[i+3][Y] - P[i+1][Y]) < EPSILON))
    {
      Error("Incorrect point in surface of revolution.\n");
    }

    /* Use cubic interpolation. */

    k[0] = P[i+1][X] * P[i+1][X];
    k[1] = P[i+2][X] * P[i+2][X];
    k[2] = (P[i+2][X] - P[i][X]) / (P[i+2][Y] - P[i][Y]);
    k[3] = (P[i+3][X] - P[i+1][X]) / (P[i+3][Y] - P[i+1][Y]);

    k[2] *= 2.0 * P[i+1][X];
    k[3] *= 2.0 * P[i+2][X];

    w = P[i+1][Y];

    Mat[0][0] = w * w * w;
    Mat[0][1] = w * w;
    Mat[0][2] = w;
    Mat[0][3] = 1.0;

    Mat[2][0] = 3.0 * w * w;
    Mat[2][1] = 2.0 * w;
    Mat[2][2] = 1.0;
    Mat[2][3] = 0.0;

    w = P[i+2][Y];

    Mat[1][0] = w * w * w;
    Mat[1][1] = w * w;
    Mat[1][2] = w;
    Mat[1][3] = 1.0;

    Mat[3][0] = 3.0 * w * w;
    Mat[3][1] = 2.0 * w;
    Mat[3][2] = 1.0;
    Mat[3][3] = 0.0;

    MInvers(Mat, Mat);

    /* Calculate coefficients of cubic patch. */

    A = k[0] * Mat[0][0] + k[1] * Mat[0][1] + k[2] * Mat[0][2] + k[3] * Mat[0][3];
    B = k[0] * Mat[1][0] + k[1] * Mat[1][1] + k[2] * Mat[1][2] + k[3] * Mat[1][3];
    C = k[0] * Mat[2][0] + k[1] * Mat[2][1] + k[2] * Mat[2][2] + k[3] * Mat[2][3];
    D = k[0] * Mat[3][0] + k[1] * Mat[3][1] + k[2] * Mat[3][2] + k[3] * Mat[3][3];

    if (fabs(A) < EPSILON) A = 0.0;
    if (fabs(B) < EPSILON) B = 0.0;
    if (fabs(C) < EPSILON) C = 0.0;
    if (fabs(D) < EPSILON) D = 0.0;

    Sor->Spline->Entry[i].A = A;
    Sor->Spline->Entry[i].B = B;
    Sor->Spline->Entry[i].C = C;
    Sor->Spline->Entry[i].D = D;

    /* Get minimum and maximum radius**2 in current segment. */

    y[0] = P[i+1][Y];
    y[1] = P[i+2][Y];

    x[0] = x[2] = P[i+1][X];
    x[1] = x[3] = P[i+2][X];

    c[0] = 3.0 * A;
    c[1] = 2.0 * B;
    c[2] = C;

    n = Solve_Polynomial(2, c, r, FALSE, 0.0);

    while (n--)
    {
      if ((r[n] >= y[0]) && (r[n] <= y[1]))
      {
        x[n] = sqrt(r[n] * (r[n] * (r[n] * A + B) + C) + D);
      }
    }

    /* Set current segment's bounding cylinder. */

    tmp_r1[i] = min(min(x[0], x[1]), min(x[2], x[3]));
    tmp_r2[i] = max(max(x[0], x[1]), max(x[2], x[3]));

    tmp_h1[i] = y[0];
    tmp_h2[i] = y[1];

    /* Keep track of overall bounding cylinder. */

    xmin = min(xmin, tmp_r1[i]);
    xmax = max(xmax, tmp_r2[i]);

    ymin = min(ymin, tmp_h1[i]);
    ymax = max(ymax, tmp_h2[i]);

/*
    fprintf(stderr, "bound spline segment %d: ", i);
    fprintf(stderr, "r = %f - %f, h = %f - %f\n", tmp_r1[i], tmp_r2[i], tmp_h1[i], tmp_h2[i]);
*/
  }

  /* Set overall bounding cylinder. */

  Sor->Radius1 = ymax;
  Sor->Radius2 = xmax;

  Sor->Height1 = ymin;
  Sor->Height2 = ymax;

  /* Get cap radius. */

  w = tmp_h2[Sor->Number-1];

  A = Sor->Spline->Entry[Sor->Number-1].A;
  B = Sor->Spline->Entry[Sor->Number-1].B;
  C = Sor->Spline->Entry[Sor->Number-1].C;
  D = Sor->Spline->Entry[Sor->Number-1].D;

  if ((Sor->Cap_Radius_Squared = w * (w * (A * w + B) + C) + D) < 0.0)
  {
    Sor->Cap_Radius_Squared = 0.0;
  }

  /* Get base radius. */

  w = tmp_h1[0];

  A = Sor->Spline->Entry[0].A;
  B = Sor->Spline->Entry[0].B;
  C = Sor->Spline->Entry[0].C;
  D = Sor->Spline->Entry[0].D;

  if ((Sor->Base_Radius_Squared = w * (w * (A * w + B) + C) + D) < 0.0)
  {
    Sor->Base_Radius_Squared = 0.0;
  }

  /* Get bounding cylinder. */

  Sor->Spline->BCyl = Create_BCyl(Sor->Number, tmp_r1, tmp_r2, tmp_h1, tmp_h2);

  /* Get rid of temp. memory. */

  POV_FREE(tmp_h2);
  POV_FREE(tmp_h1);
  POV_FREE(tmp_r2);
  POV_FREE(tmp_r1);
}



/*****************************************************************************
*
* FUNCTION
*
*   test_hit
*
* INPUT
*
*   Sor         - Pointer to lathe structure
*   Ray         - Current ray
*   Depth_Stack - Current depth stack
*   d, t, n     - Intersection depth, type and number
*
* OUTPUT
*
*   Depth_Stack
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
*   Oct 1996 : Creation.
*
******************************************************************************/

static int test_hit(SOR *Sor, RAY *Ray, ISTACK *Depth_Stack, DBL d, int t, int  n)
{
  VECTOR IPoint;

  if ((d > DEPTH_TOLERANCE) && (d < Max_Distance))
  {
    VEvaluateRay(IPoint, Ray->Initial, d, Ray->Direction);

    if (Point_In_Clip(IPoint, ((OBJECT *)Sor)->Clip))
    {
      push_entry_i1_i2(d, IPoint, (OBJECT *)Sor, t, n, Depth_Stack);

      return(TRUE);
    }
  }

  return(FALSE);
}



