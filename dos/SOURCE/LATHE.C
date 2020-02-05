/****************************************************************************
*                   lathe.c
*
*  This module implements functions that manipulate lathes.
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
*    The lathe primitive is defined by a set of points in 2d space which
*    are interpolated by linear, quadratic, or cubic splines. The resulting
*    2d curve is rotated about an axis to form the final lathe object.
*
*    All calculations are done in the object's (u,v,w)-coordinate system
*    with the (w)-axis being the rotation axis.
*
*    One spline segment in the (r,w)-plane is given by the equations
*
*      fw(t) = Aw * t^3 + Bw * t^2 + Cw * t + Dw  and
*      fr(t) = Ar * t^3 + Br * t^2 + Cr * t + Dr,
*
*    with the parameter t ranging from 0 to 1 and r = sqrt(u*u+v*v).
*
*    To intersect a ray R = P + k * D transformed into the object's
*    coordinate system with the lathe object, the equations
*
*      (Pu + k * Du)^2 + (Pv + k * Dv)^2 = (fr(t))^2
*                          (Pw + k * Dw) = fw(t)
*
*    have to be solved for t. For valid intersections (0 <= t <= 1)
*    the corresponding k can be calculated from one of the above equations.
*
*    Note that the degree of the polynomal to solve is two times the
*    degree of the spline segments used.
*
*    Note that Pu, Pv, Pw and Du, Dv, Dw denote the coordinates
*    of the vectors P and D.
*
*  Syntax:
*
*    lathe
*    {
*      [ linear_spline | quadratic_spline | cubic_spline ]
*
*      number_of_points,
*
*      <P[0]>, <P[1], ..., <P[n-1]>
*
*      [ sturm ]
*    }
*
*    Note that the P[i] are 2d vectors.
*
*    Linear interpolation is used by default. In this case all n Points
*    are used. In the quadratic case the first point is used to determine
*    the derivates at the starting point P[1]. In the cubic case
*    the first and last points are used to determine the derivatives at
*    the starting point P[1] and ending point P[n-2].
*
*    To get a closed (and smooth) curve you have make sure that
*
*      P[0] = P[n-1] in the linear case,
*
*      P[0] = P[n-2] and P[1] = P[n-1] in the quadratic case, and
*
*      P[0] = P[n-3] and P[1] = P[n-2] and P[2] = P[n-1] in the cubic case.
*
*    Note that the x coordinate of a point corresponds to the r coordinate
*    and the y coordinate to the w coordinate;
*
*  ---
*
*  Ideas for the lathe were taken from:
*
*    P. Burger and D. Gillies, "Rapid Ray Tracing of General Surfaces
*    of Revolution", New Advances in Computer Graphics, Proceedings
*    of CG International '89, R. A. Earnshaw, B. Wyvill (Eds.),
*    Springer, ..., pp. 523-531
*
*    P. Burger and D. Gillies, "Swept Surfaces", Interactive Computer
*    Graphics, Addison-Wesley, 1989, pp. 376-380
*
*  ---
*
*  Jun 1994 : Creation. [DB]
*
*****************************************************************************/

#include "frame.h"
#include "povray.h"
#include "vector.h"
#include "povproto.h"
#include "bbox.h"
#include "bcyl.h"
#include "lathe.h"
#include "polysolv.h"
#include "matrices.h"
#include "objects.h"
#include "torus.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Minimal intersection depth for a valid intersection. */

#define DEPTH_TOLERANCE 1.0e-4

/* Max. number of intersecions per spline segment. */

#define MAX_INTERSECTIONS_PER_SEGMENT 4



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int  intersect_lathe (RAY *Ray, LATHE *Lathe, ISTACK *Depth_Stack);
static int  All_Lathe_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int  Inside_Lathe (VECTOR point, OBJECT *Object);
static void Lathe_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static LATHE *Copy_Lathe (OBJECT *Object);
static void Translate_Lathe (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Lathe (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Lathe (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Lathe (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Lathe (OBJECT *Object);
static void Destroy_Lathe (OBJECT *Object);

static int  test_hit (LATHE *, RAY *, ISTACK *, DBL, DBL, int);


/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Lathe_Methods =
{
  All_Lathe_Intersections,
  Inside_Lathe, Lathe_Normal,
  (COPY_METHOD)Copy_Lathe,
  Translate_Lathe, Rotate_Lathe,
  Scale_Lathe, Transform_Lathe, Invert_Lathe, Destroy_Lathe
};



/*****************************************************************************
*
* FUNCTION
*
*   All_Lathe_Intersections
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
*   Determine ray/lathe intersection and clip intersection found.
*
* CHANGES
*
*   Jun 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

static int All_Lathe_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  Increase_Counter(stats[Ray_Lathe_Tests]);

  if (intersect_lathe(Ray, (LATHE *)Object, Depth_Stack))
  {
    Increase_Counter(stats[Ray_Lathe_Tests_Succeeded]);

    return(TRUE);
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_lathe
*
* INPUT
*
*   Ray          - Ray
*   Lathe        - Lathe
*   Intersection - Lathe intersection structure
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
*   Determine ray/lathe intersection.
*
*   NOTE: The curve is rotated about the y-axis!
*         Order of the polynomial must not be used!
*
* CHANGES
*
*   Jun 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

static int intersect_lathe(RAY *Ray, LATHE *Lathe, ISTACK *Depth_Stack)
{
  int cnt;
  int found, j, n1, n2;
  DBL k, len, r, m, w, Dy2, r0;
  DBL x1[7], x2[3], y1[6], y2[2];
  DBL best;
  VECTOR P, D;
  BCYL_INT *intervals;
  LATHE_SPLINE_ENTRY *Entry;

  /* Transform the ray into the lathe space. */

  MInvTransPoint(P, Ray->Initial, Lathe->Trans);

  MInvTransDirection(D, Ray->Direction, Lathe->Trans);

  VLength(len, D);

  VInverseScaleEq(D, len);

  /* Test if ray misses lathe's cylindrical bound. */

#ifdef LATHE_EXTRA_STATS
  Increase_Counter(stats[Lathe_Bound_Tests]);
#endif

  if (((D[Y] >= 0.0) && (P[Y] >  Lathe->Height2)) ||
      ((D[Y] <= 0.0) && (P[Y] <  Lathe->Height1)) ||
      ((D[X] >= 0.0) && (P[X] >  Lathe->Radius2)) ||
      ((D[X] <= 0.0) && (P[X] < -Lathe->Radius2)))
  {
    return(FALSE);
  }

  /* Get distance r0 of the ray from rotation axis (i.e. y axis). */

  r0 = fabs(P[X] * D[Z] - P[Z] * D[X]);

  r = D[X] * D[X] + D[Z] * D[Z];

  if (r > 0.0)
  {
    r0 /= sqrt(r);
  }

  /* Test if ray misses lathe's cylindrical bound. */

  if (r0 > Lathe->Radius2)
  {
    return(FALSE);
  }

  /* Intersect all cylindrical bounds. */

  if ((cnt = Intersect_BCyl(Lathe->Spline->BCyl, P, D)) == 0)
  {
    return(FALSE);
  }

#ifdef LATHE_EXTRA_STATS
  Increase_Counter(stats[Lathe_Bound_Tests_Succeeded]);
#endif

  /* Precalculate some constants that are ray-dependant only. */

  m = D[X] * P[X] + D[Z] * P[Z];

  Dy2 = D[Y] * D[Y];

  /* Step through the list of intersections. */

  found = FALSE;

  best = BOUND_HUGE;

  intervals = Lathe->Spline->BCyl->intervals;

  for (j = 0; j < cnt; j++)
  {
    /* Get current segment. */

    Entry = &Lathe->Spline->Entry[intervals[j].n];

    /* If we already have the best intersection we may exit. */

    if (!(Lathe->Type & IS_CHILD_OBJECT) && (intervals[j].d[0] > best))
    {
      break;
    }

    /* Init number of roots found. */

    n1 = 0;

    /* Intersect segment. */

    switch (Lathe->Spline_Type)
    {
      /***********************************************************************
      * Linear spline.
      ************************************************************************/

      case LINEAR_SPLINE:

        /* Solve 2th order polynomial. */

        x1[0] = Entry->C[Y] * Entry->C[Y] * r - Entry->C[X] * Entry->C[X] * Dy2;

        x1[1] = 2.0 * (Entry->C[Y] * ((Entry->D[Y] - P[Y]) * r + D[Y] * m) - Entry->C[X] * Entry->D[X] * Dy2);

        x1[2] = (Entry->D[Y] - P[Y]) * ((Entry->D[Y] - P[Y]) * r + 2.0 * D[Y] * m) + Dy2 * (P[X] * P[X] + P[Z] * P[Z] - Entry->D[X] * Entry->D[X]);

        n1 = Solve_Polynomial(2, x1, y1, FALSE, 0.0);

        break;

      /***********************************************************************
      * Quadratic spline.
      ************************************************************************/

      case QUADRATIC_SPLINE:

        /* Solve 4th order polynomial. */

        x1[0] = Entry->B[Y] * Entry->B[Y] * r - Entry->B[X] * Entry->B[X] * Dy2;

        x1[1] = 2.0 * (Entry->B[Y] * Entry->C[Y] * r - Entry->B[X] * Entry->C[X] * Dy2);

        x1[2] = r * (2.0 * Entry->B[Y] * (Entry->D[Y] - P[Y]) + Entry->C[Y] * Entry->C[Y]) + 2.0 * Entry->B[Y] * D[Y] * m - (2.0 * Entry->B[X] * Entry->D[X] + Entry->C[X] * Entry->C[X]) * Dy2;

        x1[3] = 2.0 * (Entry->C[Y] * ((Entry->D[Y] - P[Y]) * r + D[Y] * m) - Entry->C[X] * Entry->D[X] * Dy2);

        x1[4] = (Entry->D[Y] - P[Y]) * ((Entry->D[Y] - P[Y]) * r + 2.0 * D[Y] * m) + Dy2 * (P[X] * P[X] + P[Z] * P[Z] - Entry->D[X] * Entry->D[X]);

        n1 = Solve_Polynomial(4, x1, y1, Test_Flag(Lathe, STURM_FLAG), 0.0);

        break;

      /***********************************************************************
      * Cubic spline.
      ************************************************************************/

      case BEZIER_SPLINE:
      case CUBIC_SPLINE:

        /* Solve 6th order polynomial. */

        x1[0] = Entry->A[Y] * Entry->A[Y] * r - Entry->A[X] * Entry->A[X] * Dy2;

        x1[1] = 2.0 * (Entry->A[Y] * Entry->B[Y] * r - Entry->A[X] * Entry->B[X] * Dy2);

        x1[2] = (2.0 * Entry->A[Y] * Entry->C[Y] + Entry->B[Y] * Entry->B[Y]) * r - (2.0 * Entry->A[X] * Entry->C[X] + Entry->B[X] * Entry->B[X]) * Dy2;

        x1[3] = 2.0 * ((Entry->A[Y] * Entry->D[Y] + Entry->B[Y] * Entry->C[Y] - Entry->A[Y] * P[Y]) * r + Entry->A[Y] * D[Y] * m - (Entry->A[X] * Entry->D[X] + Entry->B[X] * Entry->C[X]) * Dy2);

        x1[4] = (2.0 * Entry->B[Y] * (Entry->D[Y] - P[Y]) + Entry->C[Y] * Entry->C[Y]) * r + 2.0 * Entry->B[Y] * D[Y] * m - (2.0 * Entry->B[X] * Entry->D[X] + Entry->C[X] * Entry->C[X]) * Dy2;

        x1[5] = 2.0 * (Entry->C[Y] * ((Entry->D[Y] - P[Y]) * r + D[Y] * m) - Entry->C[X] * Entry->D[X] * Dy2);

        x1[6] = (Entry->D[Y] - P[Y]) * ((Entry->D[Y] - P[Y]) * r + 2.0 * D[Y] * m) + Dy2 * (P[X] * P[X] + P[Z] * P[Z] - Entry->D[X] * Entry->D[X]);

        n1 = Solve_Polynomial(6, x1, y1, Test_Flag(Lathe, STURM_FLAG), 0.0);

        break;
    }

    /* Test roots for valid intersections. */

    while (n1--)
    {
      w = y1[n1];

      if ((w >= 0.0) && (w <= 1.0))
      {
        if (fabs(D[Y]) > EPSILON)
        {
          k = (w * (w * (w * Entry->A[Y] + Entry->B[Y]) + Entry->C[Y]) + Entry->D[Y] - P[Y]) / D[Y];

          if (test_hit(Lathe, Ray, Depth_Stack, k / len, w, intervals[j].n))
          {
            found = TRUE;

            if (k < best)
            {
              best = k;
            }
          }
        }
        else
        {
          k = w * (w * (w * Entry->A[X] + Entry->B[X]) + Entry->C[X]) + Entry->D[X];

          x2[0] = r;
          x2[1] = 2.0 * m;

          x2[2] = P[X] * P[X] + P[Z] * P[Z] - k * k;

          n2 = Solve_Polynomial(2, x2, y2, FALSE, 0.0);

          while (n2--)
          {
            k = y2[n2];

            if (test_hit(Lathe, Ray, Depth_Stack, k / len, w, intervals[j].n))
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
    }
  }

  return(found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Lathe
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
*   Test if a point lies inside the lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static int Inside_Lathe(VECTOR IPoint, OBJECT *Object)
{
  int i, n, NC;
  DBL r, k, w;
  DBL x[4], y[3];
  DBL *height;
  VECTOR P;
  BCYL_ENTRY *entry;
  LATHE_SPLINE_ENTRY *Entry;
  LATHE *Lathe = (LATHE *)Object;

  height = Lathe->Spline->BCyl->height;

  entry = Lathe->Spline->BCyl->entry;

  /* Transform the point into the lathe space. */

  MInvTransPoint(P, IPoint, Lathe->Trans);

  /* Number of crossings. */

  NC = 0;

  if ((P[Y] >= Lathe->Height1) && (P[Y] <= Lathe->Height2))
  {
    r = sqrt(P[X] * P[X] + P[Z] * P[Z]);

    if ((r >= Lathe->Radius1) && (r <= Lathe->Radius2))
    {
      for (i = 0; i < Lathe->Number; i++)
      {
        Entry = &Lathe->Spline->Entry[i];

        /* Test if we are inside the segments cylindrical bound. */

        if ((P[Y] >= height[entry[i].h1] - EPSILON) &&
            (P[Y] <= height[entry[i].h2] + EPSILON))
        {
          x[0] = Entry->A[Y];
          x[1] = Entry->B[Y];
          x[2] = Entry->C[Y];
          x[3] = Entry->D[Y] - P[Y];

          n = Solve_Polynomial(3, x, y, Test_Flag(Lathe, STURM_FLAG), 0.0);

          while (n--)
          {
            w = y[n];

            if ((w >= 0.0) && (w <= 1.0))
            {
              k  = w * (w * (w * Entry->A[X] + Entry->B[X]) + Entry->C[X]) + Entry->D[X] - r;

              if (k >= 0.0)
              {
                NC++;
              }
            }
          }
        }
      }
    }
  }

  if (NC & 1)
  {
    return(!Test_Flag(Lathe, INVERTED_FLAG));
  }
  else
  {
    return(Test_Flag(Lathe, INVERTED_FLAG));
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Lathe_Normal
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
*   Calculate the normal of the lathe in a given point.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Lathe_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  DBL r, dx, dy;
  VECTOR P, N;
  LATHE *Lathe = (LATHE *)Object;
  LATHE_SPLINE_ENTRY *Entry;

  Entry = &Lathe->Spline->Entry[Inter->i1];

  /* Transform the point into the lathe space. */

  MInvTransPoint(P, Inter->IPoint, Lathe->Trans);

  /* Get distance from rotation axis. */

  r = P[X] * P[X] + P[Z] * P[Z];

  if (r > EPSILON)
  {
    r = sqrt(r);

    /* Get derivatives. */

    dx = Inter->d1 * (3.0 * Entry->A[X] * Inter->d1 + 2.0 * Entry->B[X]) + Entry->C[X];
    dy = Inter->d1 * (3.0 * Entry->A[Y] * Inter->d1 + 2.0 * Entry->B[Y]) + Entry->C[Y];

    /* Get normal by rotation. */

    N[X] =  dy * P[X];
    N[Y] = -dx * r;
    N[Z] =  dy * P[Z];
  }
  else
  {
    N[X] = N[Z] = 0.0; N[Y] = 1.0;
  }

  /* Transform the normalt out of the lathe space. */

  MTransNormal(Result, N, Lathe->Trans);

  VNormalize(Result, Result);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Lathe
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
*   Translate a lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Translate_Lathe(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Lathe(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Lathe
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
*   Rotate a lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Rotate_Lathe(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Lathe(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Lathe
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
*   Scale a lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Scale_Lathe(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Lathe(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Lathe
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
*   Transform a lathe and recalculate its bounding box.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Transform_Lathe(OBJECT *Object, TRANSFORM *Trans)
{
  Compose_Transforms(((LATHE *)Object)->Trans, Trans);

  Compute_Lathe_BBox((LATHE *)Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Lathe
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
*   Invert a lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Invert_Lathe(OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Lathe
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*
*   LATHE * - new lathe
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Create a new lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

LATHE *Create_Lathe()
{
  LATHE *New;

  New = (LATHE *)POV_MALLOC(sizeof(LATHE), "lathe");

  INIT_OBJECT_FIELDS(New,LATHE_OBJECT,&Lathe_Methods)

  New->Trans = Create_Transform();

  New->Spline_Type = LINEAR_SPLINE;

  New->Number = 0;

  New->Spline = NULL;

  New->Radius1 =
  New->Radius2 =
  New->Height1 =
  New->Height2 = 0.0;

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Lathe
*
* INPUT
*
*   Object - Object
*   
* OUTPUT
*   
* RETURNS
*
*   void * - New lathe
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Copy a lathe structure.
*
*   NOTE: The splines are not copied, only the number of references is
*         counted, so that Destray_Lathe() knows if they can be destroyed.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
*   Sep 1994 : Fixed memory leakage bug. [DB]
*
******************************************************************************/

static LATHE *Copy_Lathe(OBJECT *Object)
{
  LATHE *New, *Lathe = (LATHE *)Object;

  New = Create_Lathe();

  /* Get rid of the transformation created in Create_Lathe(). */

  Destroy_Transform(New->Trans);

  /* Copy lathe. */

  *New = *Lathe;

  New->Trans = Copy_Transform(Lathe->Trans);

  New->Spline->References++;

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Lathe
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
*   Destroy a lathe.
*
*   NOTE: The splines are destroyed if they are no longer used by any copy.
*
* CHANGES
*
*   Jun 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

static void Destroy_Lathe(OBJECT *Object)
{
  LATHE *Lathe = (LATHE *)Object;

  Destroy_Transform(Lathe->Trans);

  if (--(Lathe->Spline->References) == 0)
  {
    Destroy_BCyl(Lathe->Spline->BCyl);

    POV_FREE(Lathe->Spline->Entry);

    POV_FREE(Lathe->Spline);
  }

  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Lathe_BBox
*
* INPUT
*
*   Lathe - Lathe
*   
* OUTPUT
*
*   Lathe
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a lathe.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

void Compute_Lathe_BBox(LATHE *Lathe)
{
  Make_BBox(Lathe->BBox, -Lathe->Radius2, Lathe->Height1, -Lathe->Radius2,
    2.0 * Lathe->Radius2, Lathe->Height2 - Lathe->Height1, 2.0 * Lathe->Radius2);

  Recompute_BBox(&Lathe->BBox, Lathe->Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Lathe
*
* INPUT
*
*   Lathe - Lathe
*   P     - Points defining lathe
*   
* OUTPUT
*
*   Lathe
*   
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the spline segments of a lathe from a set of points.
*
*   Note that the number of points in the lathe has to be set.
*
* CHANGES
*
*   Jun 1994 : Creation.
*   Oct 1996 : Changed code to include faster version. [DB]
*
******************************************************************************/

void Compute_Lathe(LATHE *Lathe, UV_VECT *P)
{
  int i, i1, i2, i3, n, segment, number_of_segments;
  DBL x[4], xmin, xmax;
  DBL y[4], ymin, ymax;
  DBL c[3], r[2];
  DBL *tmp_r1;
  DBL *tmp_r2;
  DBL *tmp_h1;
  DBL *tmp_h2;
  UV_VECT A, B, C, D;

  /* Get number of segments. */

  switch (Lathe->Spline_Type)
  {
    case LINEAR_SPLINE:

      number_of_segments = Lathe->Number - 1;

      break;

    case QUADRATIC_SPLINE:

      number_of_segments = Lathe->Number - 2;

      break;

    case CUBIC_SPLINE:

      number_of_segments = Lathe->Number - 3;

      break;

    case BEZIER_SPLINE:

      number_of_segments = Lathe->Number / 4;

      break;
  }

  /* Allocate segments. */

  if (Lathe->Spline == NULL)
  {
    Lathe->Spline = (LATHE_SPLINE *)POV_MALLOC(sizeof(LATHE_SPLINE), "spline segments of lathe");

    /* Init spline. */

    Lathe->Spline->References = 1;

    Lathe->Spline->Entry = (LATHE_SPLINE_ENTRY *)POV_MALLOC(number_of_segments*sizeof(LATHE_SPLINE_ENTRY), "spline segments of lathe");
  }
  else
  {
    /* This should never happen! */

    Error("Lathe segments are already defined.\n");
  }

  /* Allocate temporary lists. */

  tmp_r1 = (DBL *)POV_MALLOC(number_of_segments * sizeof(DBL), "temp lathe data");
  tmp_r2 = (DBL *)POV_MALLOC(number_of_segments * sizeof(DBL), "temp lathe data");
  tmp_h1 = (DBL *)POV_MALLOC(number_of_segments * sizeof(DBL), "temp lathe data");
  tmp_h2 = (DBL *)POV_MALLOC(number_of_segments * sizeof(DBL), "temp lathe data");

  /***************************************************************************
  * Calculate segments.
  ****************************************************************************/

  /* We want to know the size of the overall bounding cylinder. */

  xmax = ymax = -BOUND_HUGE;
  xmin = ymin = BOUND_HUGE;

  for (i = segment = 0; segment < number_of_segments; )
  {
    i1 = i + 1;
    i2 = i + 2;
    i3 = i + 3;

    switch (Lathe->Spline_Type)
    {
      /*************************************************************************
      * Linear spline (nothing more than a simple polygon).
      **************************************************************************/

      case LINEAR_SPLINE:

        /* Use linear interpolation. */

        A[X] =  0.0;
        B[X] =  0.0;
        C[X] = -1.0 * P[i][X] + 1.0 * P[i1][X];
        D[X] =  1.0 * P[i][X];

        A[Y] =  0.0;
        B[Y] =  0.0;
        C[Y] = -1.0 * P[i][Y] + 1.0 * P[i1][Y];
        D[Y] =  1.0 * P[i][Y];

        /* Get maximum coordinates in current segment. */

        x[0] = x[2] = P[i][X];
        x[1] = x[3] = P[i1][X];

        y[0] = y[2] = P[i][Y];
        y[1] = y[3] = P[i1][Y];

        break;


      /*************************************************************************
      * Quadratic spline.
      **************************************************************************/

      case QUADRATIC_SPLINE:

        /* Use quadratic interpolation. */

        A[X] =  0.0;
        B[X] =  0.5 * P[i][X] - 1.0 * P[i1][X] + 0.5 * P[i2][X];
        C[X] = -0.5 * P[i][X]                  + 0.5 * P[i2][X];
        D[X] =                  1.0 * P[i1][X];

        A[Y] =  0.0;
        B[Y] =  0.5 * P[i][Y] - 1.0 * P[i1][Y] + 0.5 * P[i2][Y];
        C[Y] = -0.5 * P[i][Y]                  + 0.5 * P[i2][Y];
        D[Y] =                  1.0 * P[i1][Y];

        /* Get maximum coordinates in current segment. */

        x[0] = x[2] = P[i1][X];
        x[1] = x[3] = P[i2][X];

        y[0] = y[2] = P[i1][Y];
        y[1] = y[3] = P[i2][Y];

        break;


      /*************************************************************************
      * Cubic spline.
      **************************************************************************/

      case CUBIC_SPLINE:

        /* Use cubic interpolation. */

        A[X] = -0.5 * P[i][X] + 1.5 * P[i1][X] - 1.5 * P[i2][X] + 0.5 * P[i3][X];
        B[X] =        P[i][X] - 2.5 * P[i1][X] + 2.0 * P[i2][X] - 0.5 * P[i3][X];
        C[X] = -0.5 * P[i][X]                  + 0.5 * P[i2][X];
        D[X] =                        P[i1][X];

        A[Y] = -0.5 * P[i][Y] + 1.5 * P[i1][Y] - 1.5 * P[i2][Y] + 0.5 * P[i3][Y];
        B[Y] =        P[i][Y] - 2.5 * P[i1][Y] + 2.0 * P[i2][Y] - 0.5 * P[i3][Y];
        C[Y] = -0.5 * P[i][Y]                  + 0.5 * P[i2][Y];
        D[Y] =                        P[i1][Y];

        /* Get maximum coordinates in current segment. */

        x[0] = x[2] = P[i1][X];
        x[1] = x[3] = P[i2][X];

        y[0] = y[2] = P[i1][Y];
        y[1] = y[3] = P[i2][Y];

        break;

      /*************************************************************************
      * Bezier spline.
      **************************************************************************/

      case BEZIER_SPLINE:

        /* Use Bernstein interpolation. */

        A[X] = P[i][X] - 3.0 * P[i1][X] + 3.0 * P[i2][X] -       P[i3][X];
        B[X] =           3.0 * P[i1][X] - 6.0 * P[i2][X] + 3.0 * P[i3][X];
        C[X] =           3.0 * P[i2][X] - 3.0 * P[i3][X];
        D[X] =                                                   P[i3][X];

        A[Y] = P[i][Y] - 3.0 * P[i1][Y] + 3.0 * P[i2][Y] -       P[i3][Y];
        B[Y] =           3.0 * P[i1][Y] - 6.0 * P[i2][Y] + 3.0 * P[i3][Y];
        C[Y] =           3.0 * P[i2][Y] - 3.0 * P[i3][Y];
        D[Y] =                                                   P[i3][Y];

        x[0] = P[i][X];
        x[1] = P[i1][X];
        x[2] = P[i2][X];
        x[3] = P[i3][X];

        y[0] = P[i][Y];
        y[1] = P[i1][Y];
        y[2] = P[i2][Y];
        y[3] = P[i3][Y];

        break;

      default:

        Error("Unknown lathe type in Compute_Lathe().\n");

    }

    Assign_UV_Vect(Lathe->Spline->Entry[segment].A, A);
    Assign_UV_Vect(Lathe->Spline->Entry[segment].B, B);
    Assign_UV_Vect(Lathe->Spline->Entry[segment].C, C);
    Assign_UV_Vect(Lathe->Spline->Entry[segment].D, D);

    if ((Lathe->Spline_Type == QUADRATIC_SPLINE) ||
        (Lathe->Spline_Type == CUBIC_SPLINE))
    {
      /* Get maximum coordinates in current segment. */

      c[0] = 3.0 * A[X];
      c[1] = 2.0 * B[X];
      c[2] = C[X];

      n = Solve_Polynomial(2, c, r, FALSE, 0.0);

      while (n--)
      {
        if ((r[n] >= 0.0) && (r[n] <= 1.0))
        {
          x[n] = r[n] * (r[n] * (r[n] * A[X] + B[X]) + C[X]) + D[X];
        }
      }

      c[0] = 3.0 * A[Y];
      c[1] = 2.0 * B[Y];
      c[2] = C[Y];

      n = Solve_Polynomial(2, c, r, FALSE, 0.0);

      while (n--)
      {
        if ((r[n] >= 0.0) && (r[n] <= 1.0))
        {
          y[n] = r[n] * (r[n] * (r[n] * A[Y] + B[Y]) + C[Y]) + D[Y];
        }
      }
    }

    /* Set current segment's bounding cylinder. */

    tmp_r1[segment] = min(min(x[0], x[1]), min(x[2], x[3]));
    tmp_r2[segment] = max(max(x[0], x[1]), max(x[2], x[3]));

    tmp_h1[segment] = min(min(y[0], y[1]), min(y[2], y[3]));
    tmp_h2[segment] = max(max(y[0], y[1]), max(y[2], y[3]));

    /* Keep track of overall bounding cylinder. */

    xmin = min(xmin, tmp_r1[segment]);
    xmax = max(xmax, tmp_r2[segment]);

    ymin = min(ymin, tmp_h1[segment]);
    ymax = max(ymax, tmp_h2[segment]);

/*
    fprintf(stderr, "bound spline segment %d: ", i);
    fprintf(stderr, "r = %f - %f, h = %f - %f\n", tmp_r1[segment], tmp_r2[segment], tmp_h1[segment], tmp_h2[segment]);
*/

    /* Advance to next segment. */

    switch (Lathe->Spline_Type)
    {
      case LINEAR_SPLINE:
      case QUADRATIC_SPLINE:
      case CUBIC_SPLINE:

        i++;

        break;

      case BEZIER_SPLINE:

        i += 4;

        break;
    }

    segment++;
  }

  Lathe->Number = number_of_segments;

  /* Set overall bounding cylinder. */

  Lathe->Radius1 = xmin;
  Lathe->Radius2 = xmax;

  Lathe->Height1 = ymin;
  Lathe->Height2 = ymax;

  /* Get bounding cylinder. */

  Lathe->Spline->BCyl = Create_BCyl(Lathe->Number, tmp_r1, tmp_r2, tmp_h1, tmp_h2);

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
*   Lathe       - Pointer to lathe structure
*   Ray         - Current ray
*   Depth_Stack - Current depth stack
*   d, w, n     - Intersection depth, parameter and segment number
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

static int test_hit(LATHE *Lathe, RAY *Ray, ISTACK *Depth_Stack, DBL d, DBL  w, int n)
{
  VECTOR IPoint;

  if ((d > DEPTH_TOLERANCE) && (d < Max_Distance))
  {
    VEvaluateRay(IPoint, Ray->Initial, d, Ray->Direction);

    if (Point_In_Clip(IPoint, ((OBJECT *)Lathe)->Clip))
    {
      push_entry_i1_d1(d, IPoint, (OBJECT *)Lathe, n, w, Depth_Stack);

      return(TRUE);
    }
  }

  return(FALSE);
}



