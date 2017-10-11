/****************************************************************************
*                   torus.c
*
*  This module implements functions that manipulate torii.
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
*  ---
*
*  June 1994 : Creation. [DB]
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
#include "torus.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Minimal depth for a valid intersection. */

#define DEPTH_TOLERANCE 1.0e-4

/* Tolerance used for order reduction during root finding. */

#define ROOT_TOLERANCE 1.0e-4



/*****************************************************************************
* Static functions
******************************************************************************/

static int intersect_torus (RAY *Ray, TORUS *Torus, DBL *Depth);
static int   All_Torus_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int   Inside_Torus (VECTOR point, OBJECT *Object);
static void  Torus_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static TORUS *Copy_Torus (OBJECT *Object);
static void  Translate_Torus (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void  Rotate_Torus (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void  Scale_Torus (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void  Transform_Torus (OBJECT *Object, TRANSFORM *Trans);
static void  Invert_Torus (OBJECT *Object);
static void  Destroy_Torus (OBJECT *Object);



/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Torus_Methods =
{
  All_Torus_Intersections, Inside_Torus, Torus_Normal,
  (COPY_METHOD)Copy_Torus, Translate_Torus, Rotate_Torus,
  Scale_Torus, Transform_Torus, Invert_Torus, Destroy_Torus
};


/*****************************************************************************
*
* FUNCTION
*
*   All_Torus_Intersections
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
*   int - TRUE, if an intersection was found
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Determine ray/torus intersection and clip intersection found.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static int All_Torus_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int i, max_i, Found;
  DBL Depth[4];
  VECTOR IPoint;

  Found = FALSE;

  if ((max_i = intersect_torus(Ray, (TORUS *)Object, Depth)) > 0)
  {
    for (i = 0; i < max_i; i++)
    {
      if ((Depth[i] > DEPTH_TOLERANCE) && (Depth[i] < Max_Distance))
      {
        VEvaluateRay(IPoint, Ray->Initial, Depth[i], Ray->Direction);

        if (Point_In_Clip(IPoint, Object->Clip))
        {
          push_entry(Depth[i], IPoint, Object, Depth_Stack);

          Found = TRUE;
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
*   intersect_torus
*
* INPUT
*
*   Ray   - Ray
*   Torus - Torus
*   Depth - Intersections found
*   
* OUTPUT
*
*   Depth
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
*   Determine ray/torus intersection.
*
*   Note that the torus is rotated about the y-axis!
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static int intersect_torus(RAY *Ray, TORUS *Torus, DBL *Depth)
{
  int i, n;
  DBL len, R2, Py2, Dy2, PDy2, k1, k2;
  DBL y1, y2, r1, r2;
  DBL c[5], r[4];
  VECTOR P, D;

  Increase_Counter(stats[Ray_Torus_Tests]);

  /* Transform the ray into the torus space. */

  MInvTransPoint(P, Ray->Initial, Torus->Trans);

  MInvTransDirection(D, Ray->Direction, Torus->Trans);

  VLength(len, D);

  VInverseScaleEq(D, len);

  i = 0;

  y1 = -Torus->r;
  y2 =  Torus->r;
  r1 = Sqr(Torus->R - Torus->r);
  r2 = Sqr(Torus->R + Torus->r);

#ifdef TORUS_EXTRA_STATS
  Increase_Counter(stats[Torus_Bound_Tests]);
#endif

  if (Test_Thick_Cylinder(P, D, y1, y2, r1, r2))
  {
#ifdef TORUS_EXTRA_STATS
    Increase_Counter(stats[Torus_Bound_Tests_Succeeded]);
#endif

    R2   = Sqr(Torus->R);
    r2   = Sqr(Torus->r);

    Py2  = P[Y] * P[Y];
    Dy2  = D[Y] * D[Y];
    PDy2 = P[Y] * D[Y];

    k1   = P[X] * P[X] + P[Z] * P[Z] + Py2 - R2 - r2;
    k2   = P[X] * D[X] + P[Z] * D[Z] + PDy2;

    c[0] = 1.0;

    c[1] = 4.0 * k2;

    c[2] = 2.0 * (k1 + 2.0 * (k2 * k2 + R2 * Dy2));

    c[3] = 4.0 * (k2 * k1 + 2.0 * R2 * PDy2);

    c[4] = k1 * k1 + 4.0 * R2 * (Py2 - r2);

    n = Solve_Polynomial(4, c, r, Test_Flag(Torus, STURM_FLAG), ROOT_TOLERANCE);

    while(n--)
    {
      Depth[i++] = r[n] / len;
    }
  }

  if (i)
  {
    Increase_Counter(stats[Ray_Torus_Tests_Succeeded]);
  }

  return(i);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Torus
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
*   Test if a point lies inside the torus.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static int Inside_Torus(VECTOR IPoint, OBJECT *Object)
{
  DBL r, r2;
  VECTOR P;
  TORUS *Torus = (TORUS *)Object;

  /* Transform the point into the torus space. */

  MInvTransPoint(P, IPoint, Torus->Trans);

  r  = sqrt(Sqr(P[X]) + Sqr(P[Z]));

  r2 = Sqr(P[Y]) + Sqr(r - Torus->R);

  if (r2 <= Sqr(Torus->r))
  {
    return(!Test_Flag(Torus, INVERTED_FLAG));
  }
  else
  {
    return(Test_Flag(Torus, INVERTED_FLAG));
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Torus_Normal
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
*   Calculate the normal of the torus in a given point.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Torus_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  DBL dist;
  VECTOR P, N, M;
  TORUS *Torus = (TORUS *)Object;

  /* Transform the point into the torus space. */

  MInvTransPoint(P, Inter->IPoint, Torus->Trans);

  /* Get normal from derivatives. */

  dist = sqrt(P[X] * P[X] + P[Z] * P[Z]);

  if (dist > EPSILON)
  {
    M[X] = Torus->R * P[X] / dist;
    M[Y] = 0.0;
    M[Z] = Torus->R * P[Z] / dist;
  }
  else
  {
    Make_Vector(M, 0.0, 0.0, 0.0);
  }

  VSub(N, P, M);

  /* Transform the normalt out of the torus space. */

  MTransNormal(Result, N, Torus->Trans);

  VNormalize(Result, Result);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Torus
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
*   Translate a torus.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Translate_Torus(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Torus(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Torus
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
*   Rotate a torus.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Rotate_Torus(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Torus(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Torus
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
*   Scale a torus.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Scale_Torus(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Torus(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Torus
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
*   Transform a torus and recalculate its bounding box.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Transform_Torus(OBJECT *Object, TRANSFORM *Trans)
{
  Compose_Transforms(((TORUS *)Object)->Trans, Trans);

  Compute_Torus_BBox((TORUS *)Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Torus
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
*   Invert a torus.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Invert_Torus(OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Torus
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*
*   TORUS * - new torus
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Create a new torus.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

TORUS *Create_Torus()
{
  TORUS *New;

  New = (TORUS *)POV_MALLOC(sizeof(TORUS), "torus");

  INIT_OBJECT_FIELDS(New,TORUS_OBJECT,&Torus_Methods)

  New->Trans = Create_Transform();

  New->R =
  New->r = 0.0;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Torus
*
* INPUT
*
*   Object - Object
*   
* OUTPUT
*   
* RETURNS
*
*   void * - New torus
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Copy a torus.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
*   Sep 1994 : fixed memory leakage [DB]
*
******************************************************************************/

static TORUS *Copy_Torus(OBJECT *Object)
{
  TORUS *New, *Torus = (TORUS *)Object;

  New = Create_Torus();

  /* Get rid of the transformation created in Create_Torus(). */

  Destroy_Transform(New->Trans);

  /* Copy torus. */

  *New = *Torus;

  New->Trans = Copy_Transform(Torus->Trans);

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Torus
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
*   Destroy a torus.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

static void Destroy_Torus (OBJECT *Object)
{
  Destroy_Transform(((TORUS *)Object)->Trans);

  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Torus_BBox
*
* INPUT
*
*   Torus - Torus
*   
* OUTPUT
*
*   Torus
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a torus.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

void Compute_Torus_BBox(TORUS *Torus)
{
  DBL r1, r2;

  r1 = Torus->r;
  r2 = Torus->R + Torus->r;

  Make_BBox(Torus->BBox, -r2, -r1, -r2, 2.0 * r2, 2.0 * r1, 2.0 * r2);

  Recompute_BBox(&Torus->BBox, Torus->Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Test_Thick_Cylinder
*
* INPUT
*
*   P  - Ray initial
*   D  - Ray direction
*   h1 - Height 1
*   h2 - Height 2
*   r1 - Square of inner radius
*   r2 - Square of outer radius
*   
* OUTPUT
*   
* RETURNS
*
*   int - TRUE, if hit
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Test if a given ray defined in the lathe's coordinate system
*   intersects a "thick" cylinder (rotated about y-axis).
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

int Test_Thick_Cylinder(VECTOR P, VECTOR  D, DBL h1, DBL  h2, DBL  r1, DBL  r2)
{
  DBL a, b, c, d;
  DBL u, v, k, r, h;

  if (fabs(D[Y]) < EPSILON)
  {
    if ((P[Y] < h1) || (P[Y] > h2))
    {
      return(FALSE);
    }
  }
  else
  {
    /* Intersect ray with the cap-plane. */

    k = (h2 - P[Y]) / D[Y];

    u = P[X] + k * D[X];
    v = P[Z] + k * D[Z];

    if ((k > EPSILON) && (k < Max_Distance))
    {
      r = u * u + v * v;

      if ((r >= r1) && (r <= r2))
      {
        return(TRUE);
      }
    }

    /* Intersectionersect ray with the base-plane. */

    k = (h1 - P[Y]) / D[Y];

    u = P[X] + k * D[X];
    v = P[Z] + k * D[Z];

    if ((k > EPSILON) && (k < Max_Distance))
    {
      r = u * u + v * v;

      if ((r >= r1) && (r <= r2))
      {
        return(TRUE);
      }
    }
  }

  a = D[X] * D[X] + D[Z] * D[Z];

  if (a > EPSILON)
  {
    /* Intersect with outer cylinder. */

    b = P[X] * D[X] + P[Z] * D[Z];

    c = P[X] * P[X] + P[Z] * P[Z] - r2;

    d = b * b - a * c;

    if (d >= 0.0)
    {
      d = sqrt(d);

      k = (-b + d) / a;

      if ((k > EPSILON) && (k < Max_Distance))
      {
        h = P[Y] + k * D[Y];

        if ((h >= h1) && (h <= h2))
        {
          return(TRUE);
        }
      }

      k = (-b - d) / a;

      if ((k > EPSILON) && (k < Max_Distance))
      {
        h = P[Y] + k * D[Y];

        if ((h >= h1) && (h <= h2))
        {
          return(TRUE);
        }
      }
    }

    /* Intersect with inner cylinder. */

    c = P[X] * P[X] + P[Z] * P[Z] - r1;

    d = b * b - a * c;

    if (d >= 0.0)
    {
      d = sqrt(d);

      k = (-b + d) / a;

      if ((k > EPSILON) && (k < Max_Distance))
      {
        h = P[Y] + k * D[Y];

        if ((h >= h1) && (h <= h2))
        {
          return(TRUE);
        }
      }

      k = (-b - d) / a;

      if ((k > EPSILON) && (k < Max_Distance))
      {
        h = P[Y] + k * D[Y];

        if ((h >= h1) && (h <= h2))
        {
          return(TRUE);
        }
      }
    }
  }

  return(FALSE);
}



