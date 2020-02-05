/****************************************************************************
*                   prism.c
*
*  This module implements functions that manipulate prisms.
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
*    The prism primitive is defined by a set of points in 2d space which
*    are interpolated by linear, quadratic, or cubic splines. The resulting
*    2d curve is swept along a straight line to form the final prism object.
*
*    All calculations are done in the object's (u,v,w)-coordinate system
*    with the (w)-axis being the sweep axis.
*
*    One spline segment in the (u,v)-plane is given by the equations
*
*      fu(t) = Au * t * t * t + Bu * t * t + Cu * t + Du  and
*      fv(t) = Av * t * t * t + Bv * t * t + Cv * t + Dv,
*
*    with the parameter t ranging from 0 to 1.
*
*    To intersect a ray R = P + k * D transformed into the object's
*    coordinate system with the linear swept prism object, the equation
*
*      Dv * fu(t) - Du * fv(t) - (Pu * Dv - Pv * Du) = 0
*
*    has to be solved for t. For valid intersections (0 <= t <= 1)
*    the corresponding k can be calculated from equation
*
*      Pu + k * Du = fu(t) or Pv + k * Dv = fv(t).
*
*    In the case of conic sweeping the equation
*
*      (Pv * Dw - Pw * Dv) * fu(t) - (Pu * Dw - Pw * Du) * fv(t)
*                                  + (Pu * Dv - Pv * Du) = 0
*
*    has to be solved for t. For valid intersections (0 <= t <= 1)
*    the corresponding k can be calculated from equation
*
*      Pu + k * Du = (Pw + k * Dw) * fu(t) or
*      Pv + k * Dv = (Pw + k * Dw) * fv(t).
*
*    Note that the polynomal to solve has the same degree as the
*    spline segments used.
*
*    Note that Pu, Pv, Pw and Du, Dv, Dw denote the coordinates
*    of the vectors P and D.
*
*  Syntax:
*
*    prism {
*      [ linear_sweep | cubic_sweep ]
*      [ linear_spline | quadratic_spline | cubic_spline ]
*
*      height1, height2,
*      number_of_points
*
*      <P(0)>, <P(1)>, ..., <P(n-1)>
*
*      [ open ]
*      [ sturm ]
*    }
*
*    Note that the P(i) are 2d vectors.
*
*  ---
*
*  Ideas for the prism was taken from:
*
*    James T. Kajiya, "New techniques for ray tracing procedurally
*    defined objects", Computer Graphics, 17(3), July 1983, pp. 91-102
*
*    Kirk ...
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
#include "matrices.h"
#include "objects.h"
#include "polysolv.h"
#include "prism.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Minimal intersection depth for a valid intersection. */

#define DEPTH_TOLERANCE 1.0e-4

/* |Coefficients| < COEFF_LIMIT are regarded to be 0. */

/*#define COEFF_LIMIT 1.0e-20 */

#define COEFF_LIMIT 1.0e-16 /*changed by CEY 11/18/95 */

/* Part of the prism hit. */

#define BASE_HIT   1
#define CAP_HIT    2
#define SPLINE_HIT 3



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int intersect_prism (RAY *Ray, PRISM *Prism, PRISM_INT *Intersection);
static int in_curve (PRISM *Prism, DBL u, DBL v);
static int test_rectangle (VECTOR P, VECTOR D, DBL x1, DBL y1, DBL x2, DBL y2);
static int   All_Prism_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int   Inside_Prism (VECTOR point, OBJECT *Object);
static void  Prism_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static PRISM *Copy_Prism (OBJECT *Object);
static void  Translate_Prism (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void  Rotate_Prism (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void  Scale_Prism (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void  Transform_Prism (OBJECT *Object, TRANSFORM *Trans);
static void  Invert_Prism (OBJECT *Object);
static void  Destroy_Prism (OBJECT *Object);


/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Prism_Methods =
{
  All_Prism_Intersections,
  Inside_Prism, Prism_Normal,
  (COPY_METHOD)Copy_Prism,
  Translate_Prism, Rotate_Prism,
  Scale_Prism, Transform_Prism, Invert_Prism, Destroy_Prism
};



/*****************************************************************************
*
* FUNCTION
*
*   All_Prism_Intersections
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
*   Determine ray/prism intersection and clip intersection found.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int All_Prism_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int i, max_i, Found;
  PRISM_INT *Inter;
  VECTOR IPoint;

  Found = FALSE;

  Inter = ((PRISM *)Object)->Intersections;

  max_i = intersect_prism(Ray, (PRISM *)Object, Inter);

  if (max_i)
  {
    for (i = 0; i < max_i; i++)
    {
      if ((Inter[i].d > DEPTH_TOLERANCE) && (Inter[i].d < Max_Distance))
      {
        VEvaluateRay(IPoint, Ray->Initial, Inter[i].d, Ray->Direction);

        if (Point_In_Clip(IPoint, Object->Clip))
        {
          push_entry_i1_i2_d1(Inter[i].d,IPoint,Object,Inter[i].t,Inter[i].n,Inter[i].w,Depth_Stack);

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
*   intersect_prism
*
* INPUT
*
*   Ray   - Ray
*   Prism - Prism
*   Int   - Prism intersection structure
*   
* OUTPUT
*
*   Int
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
*   Determine ray/prism intersection.
*
*   NOTE: Order reduction cannot be used.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int intersect_prism(RAY *Ray, PRISM *Prism, PRISM_INT *Intersection)
{
  int i, j, n;
  DBL k, u, v, w, h, len;
  DBL x[4], y[3];
  DBL k1, k2, k3;
  VECTOR P, D;
  PRISM_SPLINE_ENTRY Entry;

  /* Don't test degenerate prisms. */

  if (Test_Flag(Prism, DEGENERATE_FLAG))
  {
    return(FALSE);
  }

  Increase_Counter(stats[Ray_Prism_Tests]);

  /* Init intersection structure. */

  Intersection->d =
  Intersection->w = 0.0;
  Intersection->n =
  Intersection->t = 0;

  /* Transform the ray into the prism space */

  MInvTransPoint(P, Ray->Initial, Prism->Trans);

  MInvTransDirection(D, Ray->Direction, Prism->Trans);

  VLength(len, D);

  VInverseScaleEq(D, len);

  /* Test overall bounding rectangle. */

#ifdef PRISM_EXTRA_STATS
  Increase_Counter(stats[Prism_Bound_Tests]);
#endif

  if (((D[X] >= 0.0) && (P[X] > Prism->x2)) ||
      ((D[X] <= 0.0) && (P[X] < Prism->x1)) ||
      ((D[Z] >= 0.0) && (P[Z] > Prism->y2)) ||
      ((D[Z] <= 0.0) && (P[Z] < Prism->y1)))
  {
    return(FALSE);
  }

#ifdef PRISM_EXTRA_STATS
  Increase_Counter(stats[Prism_Bound_Tests_Succeeded]);
#endif

  /* Number of intersections found. */

  i = 0;

  /* What kind of sweep is used? */

  switch (Prism->Sweep_Type)
  {
    /*************************************************************************
    * Linear sweep.
    **************************************************************************/

    case LINEAR_SWEEP :

      if (fabs(D[Y]) < EPSILON)
      {
        if ((P[Y] < Prism->Height1) || (P[Y] > Prism->Height2))
        {
          return(FALSE);
        }
      }
      else
      {
        if (Test_Flag(Prism, CLOSED_FLAG))
        {
          /* Intersect ray with the cap-plane. */

          k = (Prism->Height2 - P[Y]) / D[Y];

          if ((k > DEPTH_TOLERANCE) && (k < Max_Distance))
          {
            u = P[X] + k * D[X];
            v = P[Z] + k * D[Z];

            if (in_curve(Prism, u, v))
            {
              Intersection[i].t   = CAP_HIT;
              Intersection[i++].d = k / len;
            }
          }

          /* Intersect ray with the base-plane. */

          k = (Prism->Height1 - P[Y]) / D[Y];

          if ((k > DEPTH_TOLERANCE) && (k < Max_Distance))
          {
            u = P[X] + k * D[X];
            v = P[Z] + k * D[Z];

            if (in_curve(Prism, u, v))
            {
              Intersection[i].t   = BASE_HIT;
              Intersection[i++].d = k / len;
            }
          }
        }
      }

      /* Intersect ray with all spline segments. */

      if ((fabs(D[X]) > EPSILON) || (fabs(D[Z]) > EPSILON))
      {
        for (j = 0; j < Prism->Number; j++)
        {
          Entry = Prism->Spline->Entry[j];

          /* Test spline's bounding rectangle (modified Cohen-Sutherland). */

#ifdef PRISM_EXTRA_STATS
          Increase_Counter(stats[Prism_Bound_Tests]);
#endif

          if (((D[X] >= 0.0) && (P[X] > Entry.x2)) ||
              ((D[X] <= 0.0) && (P[X] < Entry.x1)) ||
              ((D[Z] >= 0.0) && (P[Z] > Entry.y2)) ||
              ((D[Z] <= 0.0) && (P[Z] < Entry.y1)))
          {
            continue;
          }

          /* Number of roots found. */

          n = 0;

          switch (Prism->Spline_Type)
          {
            case LINEAR_SPLINE :

#ifdef PRISM_EXTRA_STATS
              Increase_Counter(stats[Prism_Bound_Tests_Succeeded]);
#endif

              /* Solve linear equation. */

              x[0] = Entry.C[X] * D[Z] - Entry.C[Y] * D[X];

              x[1] = D[Z] * (Entry.D[X] - P[X]) - D[X] * (Entry.D[Y] - P[Z]);

              if (fabs(x[0]) > EPSILON)
              {
                y[n++] = -x[1] / x[0];
              }

              break;

            case QUADRATIC_SPLINE :

#ifdef PRISM_EXTRA_STATS
              Increase_Counter(stats[Prism_Bound_Tests_Succeeded]);
#endif

              /* Solve quadratic equation. */

              x[0] = Entry.B[X] * D[Z] - Entry.B[Y] * D[X];

              x[1] = Entry.C[X] * D[Z] - Entry.C[Y] * D[X];

              x[2] = D[Z] * (Entry.D[X] - P[X]) - D[X] * (Entry.D[Y] - P[Z]);

              n = Solve_Polynomial(2, x, y, FALSE, 0.0);

              break;

            case CUBIC_SPLINE :
            case BEZIER_SPLINE :

              if (test_rectangle(P, D, Entry.x1, Entry.y1, Entry.x2, Entry.y2))
              {
#ifdef PRISM_EXTRA_STATS
                Increase_Counter(stats[Prism_Bound_Tests_Succeeded]);
#endif

                /* Solve cubic equation. */

                x[0] = Entry.A[X] * D[Z] - Entry.A[Y] * D[X];

                x[1] = Entry.B[X] * D[Z] - Entry.B[Y] * D[X];

                x[2] = Entry.C[X] * D[Z] - Entry.C[Y] * D[X];

                x[3] = D[Z] * (Entry.D[X] - P[X]) - D[X] * (Entry.D[Y] - P[Z]);

                n = Solve_Polynomial(3, x, y, Test_Flag(Prism, STURM_FLAG), 0.0);
              }

              break;
          }

          /* Test roots for valid intersections. */

          while (n--)
          {
            w = y[n];

            if ((w >= 0.0) && (w <= 1.0))
            {
              if (fabs(D[X]) > EPSILON)
              {
                k = (w * (w * (w * Entry.A[X] + Entry.B[X]) + Entry.C[X]) + Entry.D[X] - P[X]) / D[X];
              }
              else
              {
                k = (w * (w * (w * Entry.A[Y] + Entry.B[Y]) + Entry.C[Y]) + Entry.D[Y] - P[Z]) / D[Z];
              }

              /* Verify that intersection height is valid. */

              h = P[Y] + k * D[Y];

              if ((h >= Prism->Height1) && (h <= Prism->Height2))
              {
                Intersection[i].t   = SPLINE_HIT;
                Intersection[i].n   = j;
                Intersection[i].w   = w;
                Intersection[i++].d = k / len;
              }
            }
          }
        }
      }

      break;

    /*************************************************************************
    * Conic sweep.
    **************************************************************************/

    case CONIC_SWEEP :

      if (fabs(D[Y]) < EPSILON)
      {
        if ((P[Y] < Prism->Height1) || (P[Y] > Prism->Height2))
        {
          return(FALSE);
        }
      }
      else
      {
        if (Test_Flag(Prism, CLOSED_FLAG))
        {
          /* Intersect ray with the cap-plane. */

          if (fabs(Prism->Height2) > EPSILON)
          {
            k = (Prism->Height2 - P[Y]) / D[Y];

            if ((k > DEPTH_TOLERANCE) && (k < Max_Distance))
            {
              u = (P[X] + k * D[X]) / Prism->Height2;
              v = (P[Z] + k * D[Z]) / Prism->Height2;

              if (in_curve(Prism, u, v))
              {
                Intersection[i].t   = CAP_HIT;
                Intersection[i++].d = k / len;
              }
            }
          }

          /* Intersect ray with the base-plane. */

          if (fabs(Prism->Height1) > EPSILON)
          {
            k = (Prism->Height1 - P[Y]) / D[Y];

            if ((k > DEPTH_TOLERANCE) && (k < Max_Distance))
            {
              u = (P[X] + k * D[X]) / Prism->Height1;
              v = (P[Z] + k * D[Z]) / Prism->Height1;

              if (in_curve(Prism, u, v))
              {
                Intersection[i].t   = BASE_HIT;
                Intersection[i++].d = k / len;
              }
            }
          }
        }
      }

      /* Precompute ray-only dependant constants. */

      k1 = P[Z] * D[Y] - P[Y] * D[Z];

      k2 = P[Y] * D[X] - P[X] * D[Y];

      k3 = P[X] * D[Z] - P[Z] * D[X];

      /* Intersect ray with the spline segments. */

      if ((fabs(D[X]) > EPSILON) || (fabs(D[Z]) > EPSILON))
      {
        for (j = 0; j < Prism->Number; j++)
        {
          Entry = Prism->Spline->Entry[j];

          /* Test spline's bounding rectangle (modified Cohen-Sutherland). */

          if (((D[X] >= 0.0) && (P[X] > Entry.x2)) ||
              ((D[X] <= 0.0) && (P[X] < Entry.x1)) ||
              ((D[Z] >= 0.0) && (P[Z] > Entry.y2)) ||
              ((D[Z] <= 0.0) && (P[Z] < Entry.y1)))
          {
            continue;
          }

          /* Number of roots found. */

          n = 0;

          switch (Prism->Spline_Type)
          {
            case LINEAR_SPLINE :

              /* Solve linear equation. */

              x[0] = Entry.C[X] * k1 + Entry.C[Y] * k2;

              x[1] = Entry.D[X] * k1 + Entry.D[Y] * k2 + k3;

              if (fabs(x[0]) > EPSILON)
              {
                y[n++] = -x[1] / x[0];
              }

              break;

            case QUADRATIC_SPLINE :

              /* Solve quadratic equation. */

              x[0] = Entry.B[X] * k1 + Entry.B[Y] * k2;

              x[1] = Entry.C[X] * k1 + Entry.C[Y] * k2;

              x[2] = Entry.D[X] * k1 + Entry.D[Y] * k2 + k3;

              n = Solve_Polynomial(2, x, y, FALSE, 0.0);

              break;

            case CUBIC_SPLINE :
            case BEZIER_SPLINE :

              /* Solve cubic equation. */

              x[0] = Entry.A[X] * k1 + Entry.A[Y] * k2;

              x[1] = Entry.B[X] * k1 + Entry.B[Y] * k2;

              x[2] = Entry.C[X] * k1 + Entry.C[Y] * k2;

              x[3] = Entry.D[X] * k1 + Entry.D[Y] * k2 + k3;

              n = Solve_Polynomial(3, x, y, Test_Flag(Prism, STURM_FLAG), 0.0);

              break;
          }

          /* Test roots for valid intersections. */

          while (n--)
          {
            w = y[n];

            if ((w >= 0.0) && (w <= 1.0))
            {
              k = w * (w * (w * Entry.A[X] + Entry.B[X]) + Entry.C[X]) + Entry.D[X];

              h = D[X] - k * D[Y];

              if (fabs(h) > EPSILON)
              {
                k = (k * P[Y] - P[X]) / h;
              }
              else
              {
                k = w * (w * (w * Entry.A[Y] + Entry.B[Y]) + Entry.C[Y]) + Entry.D[Y];

                h = D[Z] - k * D[Y];

                if (fabs(h) > EPSILON)
                {
                  k = (k * P[Y] - P[Z]) / h;
                }
                else
                {
                  /* This should never happen! */
                  continue;
                }
              }

              /* Verify that intersection height is valid. */

              h = P[Y] + k * D[Y];

              if ((h >= Prism->Height1) && (h <= Prism->Height2))
              {
                Intersection[i].t   = SPLINE_HIT;
                Intersection[i].n   = j;
                Intersection[i].w   = w;
                Intersection[i++].d = k / len;
              }
            }
          }
        }
      }

      break;

      default:

        Error("Unknown sweep type in intersect_prism().\n");
  }

  if (i)
  {
    Increase_Counter(stats[Ray_Prism_Tests_Succeeded]);
  }

  return(i);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Prism
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
*   Test if point lies inside a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int Inside_Prism(VECTOR IPoint, OBJECT *Object)
{
  VECTOR P;
  PRISM *Prism = (PRISM *)Object;

  /* Transform the point into the prism space. */

  MInvTransPoint(P, IPoint, Prism->Trans);

  if ((P[Y] >= Prism->Height1) && (P[Y] < Prism->Height2))
  {
    if (Prism->Sweep_Type == CONIC_SWEEP)
    {
      /* Scale x and z coordinate. */

      if (fabs(P[Y]) > EPSILON)
      {
        P[X] /= P[Y];
        P[Z] /= P[Y];
      }
      else
      {
        P[X] = P[Z] = HUGE_VAL;
      }
    }

    if (in_curve(Prism, P[X], P[Z]))
    {
      return(!Test_Flag(Prism, INVERTED_FLAG));
    }
  }

  return(Test_Flag(Prism, INVERTED_FLAG));
}



/*****************************************************************************
*
* FUNCTION
*
*   Prism_Normal
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
*   Calculate the normal of the prism in a given point.
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Jul 1997 : Fixed bug as reported by Darko Rozic. [DB]
*
******************************************************************************/

static void Prism_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  VECTOR P;
  PRISM_SPLINE_ENTRY Entry;
  PRISM *Prism = (PRISM *)Object;
  VECTOR N;

  Make_Vector(N, 0.0, 1.0, 0.0);

  if (Inter->i1 == SPLINE_HIT)
  {
    Entry = Prism->Spline->Entry[Inter->i2];

    switch (Prism->Sweep_Type)
    {
      case LINEAR_SWEEP:

        N[X] =   Inter->d1 * (3.0 * Entry.A[Y] * Inter->d1 + 2.0 * Entry.B[Y]) + Entry.C[Y];
        N[Y] =   0.0;
        N[Z] = -(Inter->d1 * (3.0 * Entry.A[X] * Inter->d1 + 2.0 * Entry.B[X]) + Entry.C[X]);

        break;

      case CONIC_SWEEP:

        /* Transform the point into the prism space. */

        MInvTransPoint(P, Inter->IPoint, Prism->Trans);

        if (fabs(P[Y]) > EPSILON)
        {
          N[X] =   Inter->d1 * (3.0 * Entry.A[Y] * Inter->d1 + 2.0 * Entry.B[Y]) + Entry.C[Y];
          N[Z] = -(Inter->d1 * (3.0 * Entry.A[X] * Inter->d1 + 2.0 * Entry.B[X]) + Entry.C[X]);
          N[Y] = -(P[X] * N[X] + P[Z] * N[Z]) / P[Y];
        }

        break;

      default:

        Error("Unknown sweep type in Prism_Normal().\n");
    }
  }

  /* Transform the normalt out of the prism space. */

  MTransNormal(Result, N, Prism->Trans);

  VNormalize(Result, Result);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Prism
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
*   Translate a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Translate_Prism(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Prism(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Prism
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
*   Rotate a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Rotate_Prism(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Prism(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Prism
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
*   Scale a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Scale_Prism(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Prism(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Prism
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
*   Transform a prism and recalculate its bounding box.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Transform_Prism(OBJECT *Object, TRANSFORM *Trans)
{
  Compose_Transforms(((PRISM *)Object)->Trans, Trans);

  Compute_Prism_BBox((PRISM *)Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Prism
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
*   Invert a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Invert_Prism(OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Prism
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*
*   PRISM * - new prism
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Create a new prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

PRISM *Create_Prism()
{
  PRISM *New;

  New = (PRISM *)POV_MALLOC(sizeof(PRISM), "prism");

  INIT_OBJECT_FIELDS(New,PRISM_OBJECT,&Prism_Methods)

  New->Trans = Create_Transform();

  New->x1      =
  New->x2      =
  New->y1      =
  New->y2      =
  New->u1      =
  New->u2      =
  New->v1      =
  New->v2      =
  New->Height1 =
  New->Height2 = 0.0;

  New->Number = 0;

  New->Spline_Type = LINEAR_SPLINE;
  New->Sweep_Type  = LINEAR_SWEEP;

  New->Spline = NULL;

  New->Intersections = NULL;

  Set_Flag(New, CLOSED_FLAG);

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Prism
*
* INPUT
*
*   Object - Object
*   
* OUTPUT
*   
* RETURNS
*
*   void * - New prism
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Copy a prism structure.
*
*   NOTE: The splines are not copied, only the number of references is
*         counted, so that Destray_Prism() knows if they can be destroyed.
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Sep 1994 : fixed memory leakage [DB]
*
******************************************************************************/

static PRISM *Copy_Prism(OBJECT *Object)
{
  PRISM *New, *Prism = (PRISM *)Object;

  New = Create_Prism();

  /* Get rid of the transformation created in Create_Prism(). */

  Destroy_Transform(New->Trans);

  /* Copy prism. */

  *New = *Prism;

  New->Trans = Copy_Transform(((PRISM *)Object)->Trans);

  New->Spline->References++;

  Prism->Intersections = (PRISM_INT *)POV_MALLOC((Prism->Number+2)*sizeof(PRISM_INT), "prism intersection list");

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Prism
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
*   Destroy a prism.
*
*   NOTE: The splines are destroyed if they are no longer used by any copy.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static void Destroy_Prism (OBJECT *Object)
{
  PRISM *Prism = (PRISM *)Object;

  Destroy_Transform(Prism->Trans);

  POV_FREE(Prism->Intersections);

  if (--(Prism->Spline->References) == 0)
  {
    POV_FREE(Prism->Spline->Entry);

    POV_FREE(Prism->Spline);
  }

  POV_FREE(Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Prism_BBox
*
* INPUT
*
*   Prism - Prism
*   
* OUTPUT
*
*   Prism
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a prism.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Compute_Prism_BBox(PRISM *Prism)
{
  Make_BBox(Prism->BBox, Prism->x1, Prism->Height1, Prism->y1,
    Prism->x2 - Prism->x1, Prism->Height2 - Prism->Height1, Prism->y2 - Prism->y1);

  Recompute_BBox(&Prism->BBox, Prism->Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   in_curve
*
* INPUT
*
*   Prism - Prism to test
*   u, v  - Coordinates
*   
* OUTPUT
*   
* RETURNS
*
*   int - TRUE if inside
*   
* AUTHOR
*
*   Dieter Bayer, June 1994
*   
* DESCRIPTION
*
*   Test if a 2d point lies inside a prism's spline curve.
*
*   We travel from the current point in positive u direction
*   and count the number of crossings with the curve.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int in_curve(PRISM *Prism, DBL u, DBL  v)
{
  int i, n, NC;
  DBL k, w;
  DBL x[4], y[3];
  PRISM_SPLINE_ENTRY Entry;

  NC = 0;

  /* First test overall bounding rectangle. */
  
  if ((u >= Prism->u1) && (u <= Prism->u2) &&
      (v >= Prism->v1) && (v <= Prism->v2))
  {
    for (i = 0; i < Prism->Number; i++)
    {
      Entry = Prism->Spline->Entry[i];

      /* Test if current segment can be hit. */

      if ((v >= Entry.v1) && (v <= Entry.v2) && (u <= Entry.u2))
      {
        x[0] = Entry.A[Y];
        x[1] = Entry.B[Y];
        x[2] = Entry.C[Y];
        x[3] = Entry.D[Y] - v;

        n = Solve_Polynomial(3, x, y, Test_Flag(Prism, STURM_FLAG), 0.0);

        while (n--)
        {
          w = y[n];

          if ((w >= 0.0) && (w <= 1.0))
          {
            k  = w * (w * (w * Entry.A[X] + Entry.B[X]) + Entry.C[X]) + Entry.D[X] - u;

            if (k >= 0.0)
            {
              NC++;
            }
          }
        }
      }
    }
  }

  return(NC & 1);
}



/*****************************************************************************
*
* FUNCTION
*
*   test_rectangle
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer, July 1994
*   
* DESCRIPTION
*
*   Test if the 2d ray (= P + t * D) intersects a rectangle.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

static int test_rectangle(VECTOR P, VECTOR  D, DBL x1, DBL  z1, DBL  x2, DBL  z2)
{
  DBL dmin, dmax, tmin, tmax;

  if (fabs(D[X]) > EPSILON)
  {
    if (D[X] > 0.0)
    {
      dmin = (x1 - P[X]) / D[X];
      dmax = (x2 - P[X]) / D[X];

      if (dmax < EPSILON)
      {
        return(FALSE);
      }
    }
    else
    {
      dmax = (x1 - P[X]) / D[X];

      if (dmax < EPSILON)
      {
        return(FALSE);
      }

      dmin = (x2 - P[X]) / D[X];
    }

    if (dmin > dmax)
    {
      return(FALSE);
    }
  }
  else
  {
    if ((P[X] < x1) || (P[X] > x2))
    {
      return(FALSE);
    }
    else
    {
      dmin = -BOUND_HUGE;
      dmax =  BOUND_HUGE;
    }
  }

  if (fabs(D[Z]) > EPSILON)
  {
    if (D[Z] > 0.0)
    {
      tmin = (z1 - P[Z]) / D[Z];
      tmax = (z2 - P[Z]) / D[Z];
    }
    else
    {
      tmax = (z1 - P[Z]) / D[Z];
      tmin = (z2 - P[Z]) / D[Z];
    }

    if (tmax < dmax)
    {
      if (tmax < EPSILON)
      {
        return(FALSE);
      }

      if (tmin > dmin)
      {
        if (tmin > tmax)
        {
          return(FALSE);
        }

        dmin = tmin;
      }
      else
      {
        if (dmin > tmax)
        {
          return(FALSE);
        }
      }

      /*dmax = tmax; */ /*not needed CEY[1/95]*/
    }
    else
    {
      if (tmin > dmin)
      {
        if (tmin > dmax)
        {
          return(FALSE);
        }

        /* dmin = tmin; */  /*not needed CEY[1/95]*/
      }
    }
  }
  else
  {
    if ((P[Z] < z1) || (P[Z] > z2))
    {
      return(FALSE);
    }
  }

  return(TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Prism
*
* INPUT
*
*   Prism - Prism
*   P     - Points defining prism
*   
* OUTPUT
*
*   Prism
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer, June 1994
*
* DESCRIPTION
*
*   Calculate the spline segments of a prism from a set of points.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Compute_Prism(PRISM *Prism, UV_VECT *P)
{
  int i, n, number_of_splines;
  int i1, i2, i3;

  DBL x[4], xmin, xmax;
  DBL y[4], ymin, ymax;
  DBL c[3], r[2];

  UV_VECT A, B, C, D, First;

  /* Allocate Object->Number segments. */

  if (Prism->Spline == NULL)
  {
    Prism->Spline = (PRISM_SPLINE *)POV_MALLOC(sizeof(PRISM_SPLINE), "spline segments of prism");

    Prism->Spline->References = 1;

    Prism->Spline->Entry = (PRISM_SPLINE_ENTRY *)POV_MALLOC(Prism->Number*sizeof(PRISM_SPLINE_ENTRY), "spline segments of prism");
  }
  else
  {
    /* This should never happen! */

    Error("Prism segments are already defined.\n");
  }

  /* Allocate prism intersections list. */

  Prism->Intersections = (PRISM_INT *)POV_MALLOC((Prism->Number+2)*sizeof(PRISM_INT), "prism intersection list");

  /***************************************************************************
  * Calculate segments.
  ****************************************************************************/

  /* We want to know the size of the overall bounding rectangle. */

  xmax = ymax = -BOUND_HUGE;
  xmin = ymin =  BOUND_HUGE;

  /* Get first segment point. */

  switch (Prism->Spline_Type)
  {
    case LINEAR_SPLINE:

      Assign_UV_Vect(First, P[0]);

      break;

    case QUADRATIC_SPLINE:
    case CUBIC_SPLINE:

      Assign_UV_Vect(First, P[1]);

      break;
  }

  for (i = number_of_splines = 0; i < Prism->Number-1; i++)
  {
    /* Get indices of previous and next two points. */

    i1 = i + 1;
    i2 = i + 2;
    i3 = i + 3;

    switch (Prism->Spline_Type)
    {
      /*************************************************************************
      * Linear spline (nothing more than a simple polygon).
      **************************************************************************/

      case LINEAR_SPLINE :

        if (i1 >= Prism->Number)
        {
          Error("Too few points in prism. Prism not closed? Control points missing?\n");
        }

        /* Use linear interpolation. */

        A[X] =  0.0;
        B[X] =  0.0;
        C[X] = -1.0 * P[i][X] + 1.0 * P[i1][X];
        D[X] =  1.0 * P[i][X];

        A[Y] =  0.0;
        B[Y] =  0.0;
        C[Y] = -1.0 * P[i][Y] + 1.0 * P[i1][Y];
        D[Y] =  1.0 * P[i][Y];

        x[0] = x[2] = P[i][X];
        x[1] = x[3] = P[i1][X];

        y[0] = y[2] = P[i][Y];
        y[1] = y[3] = P[i1][Y];

        break;

      /*************************************************************************
      * Quadratic spline.
      **************************************************************************/

      case QUADRATIC_SPLINE :

        if (i2 >= Prism->Number)
        {
          Error("Too few points in prism.\n");
        }

        /* Use quadratic interpolation. */

        A[X] =  0.0;
        B[X] =  0.5 * P[i][X] - 1.0 * P[i1][X] + 0.5 * P[i2][X];
        C[X] = -0.5 * P[i][X]                  + 0.5 * P[i2][X];
        D[X] =                  1.0 * P[i1][X];

        A[Y] =  0.0;
        B[Y] =  0.5 * P[i][Y] - 1.0 * P[i1][Y] + 0.5 * P[i2][Y];
        C[Y] = -0.5 * P[i][Y]                  + 0.5 * P[i2][Y];
        D[Y] =                  1.0 * P[i1][Y];

        x[0] = x[2] = P[i1][X];
        x[1] = x[3] = P[i2][X];

        y[0] = y[2] = P[i1][Y];
        y[1] = y[3] = P[i2][Y];

        break;

      /*************************************************************************
      * Cubic spline.
      **************************************************************************/

      case CUBIC_SPLINE :

        if (i3 >= Prism->Number)
        {
          Error("Too few points in prism.\n");
        }

        /* Use cubic interpolation. */

        A[X] = -0.5 * P[i][X] + 1.5 * P[i1][X] - 1.5 * P[i2][X] + 0.5 * P[i3][X];
        B[X] =        P[i][X] - 2.5 * P[i1][X] + 2.0 * P[i2][X] - 0.5 * P[i3][X];
        C[X] = -0.5 * P[i][X]                  + 0.5 * P[i2][X];
        D[X] =                        P[i1][X];

        A[Y] = -0.5 * P[i][Y] + 1.5 * P[i1][Y] - 1.5 * P[i2][Y] + 0.5 * P[i3][Y];
        B[Y] =        P[i][Y] - 2.5 * P[i1][Y] + 2.0 * P[i2][Y] - 0.5 * P[i3][Y];
        C[Y] = -0.5 * P[i][Y]                  + 0.5 * P[i2][Y];
        D[Y] =                        P[i1][Y];

        x[0] = x[2] = P[i1][X];
        x[1] = x[3] = P[i2][X];

        y[0] = y[2] = P[i1][Y];
        y[1] = y[3] = P[i2][Y];

        break;

      /*************************************************************************
      * Bezier spline.
      **************************************************************************/

      case BEZIER_SPLINE :

        if (i3 >= Prism->Number)
        {
          Error("Too few points in prism. Prism not closed? Control points missing?\n");
        }

        /* Use Bernstein blending function interpolation. */

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

        Error("Unknown spline type in Compute_Prism().\n");
    }

    Assign_UV_Vect(Prism->Spline->Entry[number_of_splines].A, A);
    Assign_UV_Vect(Prism->Spline->Entry[number_of_splines].B, B);
    Assign_UV_Vect(Prism->Spline->Entry[number_of_splines].C, C);
    Assign_UV_Vect(Prism->Spline->Entry[number_of_splines].D, D);

    if ((Prism->Spline_Type == QUADRATIC_SPLINE) ||
        (Prism->Spline_Type == CUBIC_SPLINE))
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

    /* Set current segment's bounding rectangle. */

    Prism->Spline->Entry[number_of_splines].x1 = min(min(x[0], x[1]), min(x[2], x[3]));

    Prism->Spline->Entry[number_of_splines].x2 =
    Prism->Spline->Entry[number_of_splines].u2 = max(max(x[0], x[1]), max(x[2], x[3]));

    Prism->Spline->Entry[number_of_splines].y1 =
    Prism->Spline->Entry[number_of_splines].v1 = min(min(y[0], y[1]), min(y[2], y[3]));

    Prism->Spline->Entry[number_of_splines].y2 =
    Prism->Spline->Entry[number_of_splines].v2 = max(max(y[0], y[1]), max(y[2], y[3]));

    /* Keep track of overall bounding rectangle. */

    xmin = min(xmin, Prism->Spline->Entry[number_of_splines].x1);
    xmax = max(xmax, Prism->Spline->Entry[number_of_splines].x2);

    ymin = min(ymin, Prism->Spline->Entry[number_of_splines].y1);
    ymax = max(ymax, Prism->Spline->Entry[number_of_splines].y2);

    number_of_splines++;

    /* Advance to next segment. */

    switch (Prism->Spline_Type)
    {
      case LINEAR_SPLINE:

        if ((fabs(P[i1][X] - First[X]) < EPSILON) &&
            (fabs(P[i1][Y] - First[Y]) < EPSILON))
        {
          i++;

          if (i+1 < Prism->Number)
          {
            Assign_UV_Vect(First, P[i+1]);
          }
        }

        break;

      case QUADRATIC_SPLINE:

        if ((fabs(P[i2][X] - First[X]) < EPSILON) &&
            (fabs(P[i2][Y] - First[Y]) < EPSILON))
        {
          i += 2;

          if (i+2 < Prism->Number)
          {
            Assign_UV_Vect(First, P[i+2]);
          }
        }

        break;

      case CUBIC_SPLINE:

        if ((fabs(P[i2][X] - First[X]) < EPSILON) &&
            (fabs(P[i2][Y] - First[Y]) < EPSILON))
        {
          i += 3;

          if (i+2 < Prism->Number)
          {
            Assign_UV_Vect(First, P[i+2]);
          }
        }

        break;

      case BEZIER_SPLINE:

        i += 3;

        break;
    }
  }

  Prism->Number = number_of_splines;

  /* Set overall bounding rectangle. */

  Prism->x1 =
  Prism->u1 = xmin;
  Prism->x2 =
  Prism->u2 = xmax;

  Prism->y1 =
  Prism->v1 = ymin;
  Prism->y2 =
  Prism->v2 = ymax;

  if (Prism->Sweep_Type == CONIC_SWEEP)
  {
    /* Recalculate bounding rectangles. */

    for (i = 0; i < Prism->Number; i++)
    {
      x[0] = Prism->Spline->Entry[i].x1 * Prism->Height1;
      x[1] = Prism->Spline->Entry[i].x1 * Prism->Height2;
      x[2] = Prism->Spline->Entry[i].x2 * Prism->Height1;
      x[3] = Prism->Spline->Entry[i].x2 * Prism->Height2;

      xmin = min(min(x[0], x[1]), min(x[2], x[3]));
      xmax = max(max(x[0], x[1]), max(x[2], x[3]));

      Prism->Spline->Entry[i].x1 = xmin;
      Prism->Spline->Entry[i].x2 = xmax;

      y[0] = Prism->Spline->Entry[i].y1 * Prism->Height1;
      y[1] = Prism->Spline->Entry[i].y1 * Prism->Height2;
      y[2] = Prism->Spline->Entry[i].y2 * Prism->Height1;
      y[3] = Prism->Spline->Entry[i].y2 * Prism->Height2;

      ymin = min(min(y[0], y[1]), min(y[2], y[3]));
      ymax = max(max(y[0], y[1]), max(y[2], y[3]));

      Prism->Spline->Entry[i].y1 = ymin;
      Prism->Spline->Entry[i].y2 = ymax;
    }

    /* Recalculate overall bounding rectangle. */

    x[0] = Prism->x1 * Prism->Height1;
    x[1] = Prism->x1 * Prism->Height2;
    x[2] = Prism->x2 * Prism->Height1;
    x[3] = Prism->x2 * Prism->Height2;

    xmin = min(min(x[0], x[1]), min(x[2], x[3]));
    xmax = max(max(x[0], x[1]), max(x[2], x[3]));

    Prism->x1 = xmin;
    Prism->x2 = xmax;

    y[0] = Prism->y1 * Prism->Height1;
    y[1] = Prism->y1 * Prism->Height2;
    y[2] = Prism->y2 * Prism->Height1;
    y[3] = Prism->y2 * Prism->Height2;

    ymin = min(min(y[0], y[1]), min(y[2], y[3]));
    ymax = max(max(y[0], y[1]), max(y[2], y[3]));

    Prism->y1 = ymin;
    Prism->y2 = ymax;
  }
}



