/****************************************************************************
*                poly.c
*
*  This module implements the code for general 3 variable polynomial shapes
*
*  This file was written by Alexander Enzmann.  He wrote the code for
*  4th - 6th order shapes and generously provided us these enhancements.
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
#include "bbox.h"
#include "polysolv.h"
#include "matrices.h"
#include "objects.h"
#include "poly.h"
#include "povray.h"

/*
 * Basic form of a quartic equation:
 *
 *  a00*x^4    + a01*x^3*y   + a02*x^3*z   + a03*x^3     + a04*x^2*y^2 +
 *  a05*x^2*y*z+ a06*x^2*y   + a07*x^2*z^2 + a08*x^2*z   + a09*x^2     +
 *  a10*x*y^3  + a11*x*y^2*z + a12*x*y^2   + a13*x*y*z^2 + a14*x*y*z   +
 *  a15*x*y    + a16*x*z^3   + a17*x*z^2   + a18*x*z     + a19*x       + a20*y^4   +
 *  a21*y^3*z  + a22*y^3     + a23*y^2*z^2 + a24*y^2*z   + a25*y^2     + a26*y*z^3 +
 *  a27*y*z^2  + a28*y*z     + a29*y       + a30*z^4     + a31*z^3     + a32*z^2   + a33*z + a34
 *
 */



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define DEPTH_TOLERANCE 1.0e-4
#define INSIDE_TOLERANCE 1.0e-4
#define ROOT_TOLERANCE 1.0e-4
#define COEFF_LIMIT 1.0e-20
#define BINOMSIZE 40



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int intersect (RAY *Ray, int Order, DBL *Coeffs, int Sturm_Flag, DBL *Depths);
static void normal0 (VECTOR Result, int Order, DBL *Coeffs, VECTOR IPoint);
static void normal1 (VECTOR Result, int Order, DBL *Coeffs, VECTOR IPoint);
static DBL inside (VECTOR IPoint, int Order, DBL *Coeffs);
static int intersect_linear (RAY *ray, DBL *Coeffs, DBL *Depths);
static int intersect_quadratic (RAY *ray, DBL *Coeffs, DBL *Depths);
static int factor_out (int n, int i, int *c, int *s);
static long binomial (int n, int r);
static void factor1 (int n, int *c, int *s);

/* unused
static DBL evaluate_linear (VECTOR P, DBL *a);
static DBL evaluate_quadratic (VECTOR P, DBL *a);
*/

static int All_Poly_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int Inside_Poly (VECTOR IPoint, OBJECT *Object);
static void Poly_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static POLY *Copy_Poly (OBJECT *Object);
static void Translate_Poly (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Poly (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Poly (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Poly (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Poly (OBJECT *Object);
static void Destroy_Poly (OBJECT *Object);

/*****************************************************************************
* Local variables
******************************************************************************/

METHODS Poly_Methods =
{
  All_Poly_Intersections,
  Inside_Poly, Poly_Normal, (COPY_METHOD)Copy_Poly,
  Translate_Poly, Rotate_Poly,
  Scale_Poly, Transform_Poly, Invert_Poly, Destroy_Poly
};



/* The following table contains the binomial coefficients up to 15 */

static int binomials[15][15] =
{
  {1,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0},
  {1,  1,  0,  0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0},
  {1,  2,  1,  0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0},
  {1,  3,  3,  1,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0},
  {1,  4,  6,  4,   1,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0},
  {1,  5, 10, 10,   5,   1,   0,   0,   0,   0,   0,  0,  0,  0,  0},
  {1,  6, 15, 20,  15,   6,   1,   0,   0,   0,   0,  0,  0,  0,  0},
  {1,  7, 21, 35,  35,  21,   7,   1,   0,   0,   0,  0,  0,  0,  0},
  {1,  8, 28, 56,  70,  56,  28,   8,   1,   0,   0,  0,  0,  0,  0},
  {1,  9, 36, 84, 126, 126,  84,  36,   9,   1,   0,  0,  0,  0,  0},
  {1, 10, 45,120, 210, 252, 210, 120,  45,  10,   1,  0,  0,  0,  0},
  {1, 11, 55,165, 330, 462, 462, 330, 165,  55,  11,  1,  0,  0,  0},
  {1, 12, 66,220, 495, 792, 924, 792, 495, 220,  66, 12,  1,  0,  0},
  {1, 13, 78,286, 715,1287,1716,1716,1287, 715, 286, 78, 13,  1,  0},
  {1, 14, 91,364,1001,2002,3003,3432,3003,2002,1001,364, 91, 14,  1}
};

static DBL eqn_v[3][MAX_ORDER+1], eqn_vt[3][MAX_ORDER+1];




/*****************************************************************************
*
* FUNCTION
*
*   All_Poly_Intersections
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

static int All_Poly_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  POLY *Poly = (POLY *) Object;
  DBL Depths[MAX_ORDER], len;
  VECTOR  IPoint;
  int cnt, i, j, Intersection_Found, same_root;
  RAY New_Ray;

  /* Transform the ray into the polynomial's space */

  MInvTransPoint(New_Ray.Initial, Ray->Initial, Poly->Trans);
  MInvTransDirection(New_Ray.Direction, Ray->Direction, Poly->Trans);

  VLength(len, New_Ray.Direction);
  VInverseScaleEq(New_Ray.Direction, len);

  Intersection_Found = FALSE;

  Increase_Counter(stats[Ray_Poly_Tests]);

  switch (Poly->Order)
  {
    case 1:

      cnt = intersect_linear(&New_Ray, Poly->Coeffs, Depths);

      break;

    case 2:

      cnt = intersect_quadratic(&New_Ray, Poly->Coeffs, Depths);

      break;

    default:

      cnt = intersect(&New_Ray, Poly->Order, Poly->Coeffs, Test_Flag(Poly, STURM_FLAG), Depths);
  }

  if (cnt > 0)
  {
    Increase_Counter(stats[Ray_Poly_Tests_Succeeded]);
  }

  for (i = 0; i < cnt; i++)
  {
    if (Depths[i] > DEPTH_TOLERANCE)
    {
      same_root = FALSE;

      for (j = 0; j < i; j++)
      {
        if (Depths[i] == Depths[j])
        {
          same_root = TRUE;

          break;
        }
      }

      if (!same_root)
      {
        VEvaluateRay(IPoint, New_Ray.Initial, Depths[i], New_Ray.Direction);

        /* Transform the point into world space */

        MTransPoint(IPoint, IPoint, Poly->Trans);

        if (Point_In_Clip(IPoint, Object->Clip))
        {
          push_entry(Depths[i] / len,IPoint,Object,Depth_Stack);

          Intersection_Found = TRUE;
        }
      }
    }
  }

  return (Intersection_Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   evaluate_linear
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

/* For speedup of low order polynomials, expand out the terms
   involved in evaluating the poly. */
/* unused
static DBL evaluate_linear(VECTOR P, DBL *a)
{
  return(a[0] * P[X]) + (a[1] * P[Y]) + (a[2] * P[Z]) + a[3];
}
*/



/*****************************************************************************
*
* FUNCTION
*
*   evaluate_quadratic
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

/*
static DBL evaluate_quadratic(VECTOR P, DBL *a)
{
  DBL x, y, z;

  x = P[X];
  y = P[Y];
  z = P[Z];

  return(a[0] * x * x + a[1] * x * y + a[2] * x * z +
         a[3] * x     + a[4] * y * y + a[5] * y * z +
         a[6] * y     + a[7] * z * z + a[8] * z     + a[9]);
}
*/



/*****************************************************************************
*
* FUNCTION
*
*   factor_out
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
*   Remove all factors of i from n.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int factor_out(int n, int  i, int  *c, int  *s)
{
  while (!(n % i))
  {
    n /= i;

    s[(*c)++] = i;
  }

  return(n);
}



/*****************************************************************************
*
* FUNCTION
*
*   factor1
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
*   Find all prime factors of n. (Note that n must be less than 2^15.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void factor1(int n, int  *c, int  *s)
{
  int i,k;

  /* First factor out any 2s. */

  n = factor_out(n, 2, c, s);

  /* Now any odd factors. */

  k = (int)sqrt((DBL)n) + 1;

  for (i = 3; (n > 1) && (i <= k); i += 2)
  {
    if (!(n % i))
    {
      n = factor_out(n, i, c, s);
      k = (int)sqrt((DBL)n)+1;
    }
  }

  if (n > 1)
  {
    s[(*c)++] = n;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   binomial
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
*   Calculate the binomial coefficent of n, r.
*
* CHANGES
*
*   -
*
******************************************************************************/

static long binomial(int n, int  r)
{
  int h,i,j,k,l;
  unsigned long result;
  static int stack1[BINOMSIZE], stack2[BINOMSIZE];

  if ((n < 0) || (r < 0) || (r > n))
  {
    result = 0L;
  }
  else
  {
    if (r == n)
    {
      result = 1L;
    }
    else
    {
      if ((r < 15) && (n < 15))
      {
        result = (long)binomials[n][r];
      }
      else
      {
        j = 0;

        for (i = r + 1; i <= n; i++)
        {
          stack1[j++] = i;
        }

        for (i = 2; i <= (n-r); i++)
        {
          h = 0;

          factor1(i, &h, stack2);

          for (k = 0; k < h; k++)
          {
            for (l = 0; l < j; l++)
            {
              if (!(stack1[l] % stack2[k]))
              {
                stack1[l] /= stack2[k];

                goto l1;
              }
            }
          }

          /* Error if we get here */
/*        Debug_Info("Failed to factor\n");*/
l1:;
        }

        result = 1;

        for (i = 0; i < j; i++)
        {
          result *= stack1[i];
        }
      }
    }
  }

  return(result);
}



/*****************************************************************************
*
* FUNCTION
*
*   inside
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

static DBL inside(VECTOR IPoint, int Order, DBL *Coeffs)
{
  DBL x[MAX_ORDER+1], y[MAX_ORDER+1];
  DBL z[MAX_ORDER+1], c, Result;
  int i, j, k, term;

  x[0] = 1.0;       y[0] = 1.0;       z[0] = 1.0;
  x[1] = IPoint[X]; y[1] = IPoint[Y]; z[1] = IPoint[Z];

  for (i = 2; i <= Order; i++)
  {
    x[i] = x[1] * x[i-1];
    y[i] = y[1] * y[i-1];
    z[i] = z[1] * z[i-1];
  }

  Result = 0.0;

  term = 0;

  for (i = Order; i >= 0; i--)
  {
    for (j=Order-i;j>=0;j--)
    {
      for (k=Order-(i+j);k>=0;k--)
      {
        if ((c = Coeffs[term]) != 0.0)
        {
          Result += c * x[i] * y[j] * z[k];
        }
        term++;
      }
    }
  }

  return(Result);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect
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
*   Intersection of a ray and an arbitrary polynomial function.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int intersect(RAY *ray, int Order, DBL *Coeffs, int  Sturm_Flag, DBL  *Depths)
{
  DBL eqn[MAX_ORDER+1];
  DBL t[3][MAX_ORDER+1];
  VECTOR  P, D;
  DBL val;
  int h, i, j, k, i1, j1, k1, term;
  int offset;

  /* First we calculate the values of the individual powers
     of x, y, and z as they are represented by the ray */

  Assign_Vector(P,ray->Initial);
  Assign_Vector(D,ray->Direction);

  for (i = 0; i < 3; i++)
  {
    eqn_v[i][0]  = 1.0;
    eqn_vt[i][0] = 1.0;
  }

  eqn_v[0][1] = P[X];
  eqn_v[1][1] = P[Y];
  eqn_v[2][1] = P[Z];

  eqn_vt[0][1] = D[X];
  eqn_vt[1][1] = D[Y];
  eqn_vt[2][1] = D[Z];

  for (i = 2; i <= Order; i++)
  {
    for (j=0;j<3;j++)
    {
     eqn_v[j][i]  = eqn_v[j][1] * eqn_v[j][i-1];
     eqn_vt[j][i] = eqn_vt[j][1] * eqn_vt[j][i-1];
    }
  }

  for (i = 0; i <= Order; i++)
  {
    eqn[i] = 0.0;
  }

  /* Now walk through the terms of the polynomial.  As we go
     we substitute the ray equation for each of the variables. */

  term = 0;

  for (i = Order; i >= 0; i--)
  {
    for (h = 0; h <= i; h++)
    {
      t[0][h] = binomial(i, h) * eqn_vt[0][i-h] * eqn_v[0][h];
    }

    for (j = Order-i; j >= 0; j--)
    {
      for (h = 0; h <= j; h++)
      {
        t[1][h] = binomial(j, h) * eqn_vt[1][j-h] * eqn_v[1][h];
      }

      for (k = Order-(i+j); k >= 0; k--)
      {
        if (Coeffs[term] != 0)
        {
          for (h = 0; h <= k; h++)
          {
            t[2][h] = binomial(k, h) * eqn_vt[2][k-h] * eqn_v[2][h];
          }

          /* Multiply together the three polynomials. */

          offset = Order - (i + j + k);

          for (i1 = 0; i1 <= i; i1++)
          {
            for (j1=0;j1<=j;j1++)
            {
              for (k1=0;k1<=k;k1++)
              {
                h = offset + i1 + j1 + k1;
                val = Coeffs[term];
                val *= t[0][i1];
                val *= t[1][j1];
                val *= t[2][k1];
                eqn[h] += val;
              }
            }
          }
        }

        term++;
      }
    }
  }

  for (i = 0, j = Order; i <= Order; i++)
  {
    if (eqn[i] != 0.0)
    {
      break;
    }
    else
    {
      j--;
    }
  }

  if (j > 1)
  {
    return(Solve_Polynomial(j, &eqn[i], Depths, Sturm_Flag, ROOT_TOLERANCE));
  }
  else
  {
    return(0);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_linear
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
*   Intersection of a ray and a quadratic.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int intersect_linear(RAY *ray, DBL *Coeffs, DBL  *Depths)
{
  DBL t0, t1, *a = Coeffs;

  t0 = a[0] * ray->Initial[X] + a[1] * ray->Initial[Y] + a[2] * ray->Initial[Z];
  t1 = a[0] * ray->Direction[X] + a[1] * ray->Direction[Y] +

  a[2] * ray->Direction[Z];

  if (fabs(t1) < EPSILON)
  {
    return(0);
  }

  Depths[0] = -(a[3] + t0) / t1;

  return(1);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_quadratic
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
*   Intersection of a ray and a quadratic.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int intersect_quadratic(RAY *ray, DBL *Coeffs, DBL  *Depths)
{
  DBL x, y, z, x2, y2, z2;
  DBL xx, yy, zz, xx2, yy2, zz2;
  DBL *a, ac, bc, cc, d, t;

  x  = ray->Initial[X];
  y  = ray->Initial[Y];
  z  = ray->Initial[Z];

  xx = ray->Direction[X];
  yy = ray->Direction[Y];
  zz = ray->Direction[Z];

  x2  = x * x;    y2  = y * y;    z2 = z * z;
  xx2 = xx * xx;  yy2 = yy * yy;  zz2 = zz * zz;

  a = Coeffs;

  /*
   * Determine the coefficients of t^n, where the line is represented
   * as (x,y,z) + (xx,yy,zz)*t.
   */

  ac = (a[0]*xx2 + a[1]*xx*yy + a[2]*xx*zz + a[4]*yy2 + a[5]*yy*zz + a[7]*zz2);

  bc = (2*a[0]*x*xx + a[1]*(x*yy + xx*y) + a[2]*(x*zz + xx*z) +
        a[3]*xx + 2*a[4]*y*yy + a[5]*(y*zz + yy*z) + a[6]*yy +
        2*a[7]*z*zz + a[8]*zz);

  cc = a[0]*x2 + a[1]*x*y + a[2]*x*z + a[3]*x + a[4]*y2 +
       a[5]*y*z + a[6]*y + a[7]*z2 + a[8]*z + a[9];

  if (fabs(ac) < COEFF_LIMIT)
  {
    if (fabs(bc) < COEFF_LIMIT)
    {
      return(0);
    }

    Depths[0] = -cc / bc;

    return(1);
  }

  /*
   * Solve the quadratic formula & return results that are
   * within the correct interval.
   */

  d = bc * bc - 4.0 * ac * cc;

  if (d < 0.0)
  {
    return(0);
  }

  d = sqrt(d);

  bc = -bc;

  t = 2.0 * ac;

  Depths[0] = (bc + d) / t;
  Depths[1] = (bc - d) / t;

  return(2);
}



/*****************************************************************************
*
* FUNCTION
*
*   normal0
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
*   Normal to a polynomial (used for polynomials with order > 4).
*
* CHANGES
*
*   -
*
******************************************************************************/

static void normal0(VECTOR Result, int Order, DBL *Coeffs, VECTOR IPoint)
{
  int i, j, k, term;
  DBL val, *a, x[MAX_ORDER+1], y[MAX_ORDER+1], z[MAX_ORDER+1];

  x[0] = 1.0;
  y[0] = 1.0;
  z[0] = 1.0;

  x[1] = IPoint[X];
  y[1] = IPoint[Y];
  z[1] = IPoint[Z];

  for (i = 2; i <= Order; i++)
  {
    x[i] = IPoint[X] * x[i-1];
    y[i] = IPoint[Y] * y[i-1];
    z[i] = IPoint[Z] * z[i-1];
  }

  a = Coeffs;

  term = 0;

  Make_Vector(Result, 0.0, 0.0, 0.0);

  for (i = Order; i >= 0; i--)
  {
    for (j = Order-i; j >= 0; j--)
    {
      for (k = Order-(i+j); k >= 0; k--)
      {
        if (i >= 1)
        {
          val = x[i-1] * y[j] * z[k];
          Result[X] += i * a[term] * val;
        }

        if (j >= 1)
        {
          val = x[i] * y[j-1] * z[k];
          Result[Y] += j * a[term] * val;
        }

        if (k >= 1)
        {
          val = x[i] * y[j] * z[k-1];
          Result[Z] += k * a[term] * val;
        }

        term++;
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   nromal1
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
*   Normal to a polynomial (for polynomials of order <= 4).
*
* CHANGES
*
*   -
*
******************************************************************************/

static void normal1(VECTOR Result, int Order, DBL *Coeffs, VECTOR IPoint)
{
  DBL *a, x, y, z, x2, y2, z2, x3, y3, z3;

  a = Coeffs;

  x = IPoint[X];
  y = IPoint[Y];
  z = IPoint[Z];

  switch (Order)
  {
    case 1:

      /* Linear partial derivatives */

      Make_Vector(Result, a[0], a[1], a[2])

      break;

    case 2:

      /* Quadratic partial derivatives */

      Result[X] = 2*a[0]*x+a[1]*y+a[2]*z+a[3];
      Result[Y] = a[1]*x+2*a[4]*y+a[5]*z+a[6];
      Result[Z] = a[2]*x+a[5]*y+2*a[7]*z+a[8];

      break;

    case 3:

        x2 = x * x;  y2 = y * y;  z2 = z * z;

        /* Cubic partial derivatives */

        Result[X] = 3*a[0]*x2 + 2*x*(a[1]*y + a[2]*z + a[3]) + a[4]*y2 +
                    y*(a[5]*z + a[6]) + a[7]*z2 + a[8]*z + a[9];
        Result[Y] = a[1]*x2 + x*(2*a[4]*y + a[5]*z + a[6]) + 3*a[10]*y2 +
                    2*y*(a[11]*z + a[12]) + a[13]*z2 + a[14]*z + a[15];
        Result[Z] = a[2]*x2 + x*(a[5]*y + 2*a[7]*z + a[8]) + a[11]*y2 +
                    y*(2*a[13]*z + a[14]) + 3*a[16]*z2 + 2*a[17]*z + a[18];

        break;

    case 4:

      /* Quartic partial derivatives */

      x2 = x * x;  y2 = y * y;  z2 = z * z;
      x3 = x * x2; y3 = y * y2; z3 = z * z2;

      Result[X] = 4*a[ 0]*x3+3*x2*(a[ 1]*y+a[ 2]*z+a[ 3])+
                  2*x*(a[ 4]*y2+y*(a[ 5]*z+a[ 6])+a[ 7]*z2+a[ 8]*z+a[ 9])+
                  a[10]*y3+y2*(a[11]*z+a[12])+y*(a[13]*z2+a[14]*z+a[15])+
                  a[16]*z3+a[17]*z2+a[18]*z+a[19];
      Result[Y] = a[ 1]*x3+x2*(2*a[ 4]*y+a[ 5]*z+a[ 6])+
                  x*(3*a[10]*y2+2*y*(a[11]*z+a[12])+a[13]*z2+a[14]*z+a[15])+
                  4*a[20]*y3+3*y2*(a[21]*z+a[22])+2*y*(a[23]*z2+a[24]*z+a[25])+
                  a[26]*z3+a[27]*z2+a[28]*z+a[29];
      Result[Z] = a[ 2]*x3+x2*(a[ 5]*y+2*a[ 7]*z+a[ 8])+
                  x*(a[11]*y2+y*(2*a[13]*z+a[14])+3*a[16]*z2+2*a[17]*z+a[18])+
                  a[21]*y3+y2*(2*a[23]*z+a[24])+y*(3*a[26]*z2+2*a[27]*z+a[28])+
                  4*a[30]*z3+3*a[31]*z2+2*a[32]*z+a[33];
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Poly
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

static int Inside_Poly (VECTOR IPoint, OBJECT *Object)
{
  VECTOR  New_Point;
  DBL Result;

  /* Transform the point into polynomial's space */

  MInvTransPoint(New_Point, IPoint, ((POLY *)Object)->Trans);

  Result = inside(New_Point, ((POLY *)Object)->Order, ((POLY *)Object)->Coeffs);

  if (Result < INSIDE_TOLERANCE)
  {
    return ((int)(!Test_Flag(Object, INVERTED_FLAG)));
  }
  else
  {
    return ((int)Test_Flag(Object, INVERTED_FLAG));
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Poly_Normal
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
*   Normal to a polynomial.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void Poly_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  DBL val;
  VECTOR  New_Point;
  POLY *Shape = (POLY *)Object;

  /* Transform the point into the polynomials space. */

  MInvTransPoint(New_Point, Inter->IPoint, Shape->Trans);

  if (((POLY *)Object)->Order > 4)
  {
    normal0(Result, Shape->Order, Shape->Coeffs, New_Point);
  }
  else
  {
    normal1(Result, Shape->Order, Shape->Coeffs, New_Point);
  }

  /* Transform back to world space. */

  MTransNormal(Result, Result, Shape->Trans);

  /* Normalize (accounting for the possibility of a 0 length normal). */

  VDot(val, Result, Result);

  if (val > 0.0)
  {
    val = 1.0 / sqrt(val);

    VScaleEq(Result, val);
  }
  else
  {
    Make_Vector(Result, 1.0, 0.0, 0.0)
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Poly
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

static void Translate_Poly (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Poly(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Poly
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

static void Rotate_Poly (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Poly(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Poly
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

static void Scale_Poly (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Poly(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Poly
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

static void Transform_Poly(OBJECT *Object,TRANSFORM *Trans)
{
  Compose_Transforms(((POLY *)Object)->Trans, Trans);

  Compute_Poly_BBox((POLY *)Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Poly
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

static void Invert_Poly(OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Poly
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

POLY *Create_Poly(int Order)
{
  POLY *New;
  int i;

  New = (POLY *)POV_MALLOC(sizeof (POLY), "poly");

  INIT_OBJECT_FIELDS(New,POLY_OBJECT, &Poly_Methods);

  New->Order = Order;

  New->Trans = Create_Transform();

  New->Coeffs = (DBL *)POV_MALLOC(term_counts(Order) * sizeof(DBL), "coefficients for POLY");

  for (i = 0; i < term_counts(Order); i++)
  {
    New->Coeffs[i] = 0.0;
  }

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Poly
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
*   Make a copy of a polynomial object.
*
* CHANGES
*
*   -
*
******************************************************************************/

static POLY *Copy_Poly(OBJECT *Object)
{
  POLY *New;
  int i;

  New = Create_Poly (((POLY *)Object)->Order);

  /* Get rid of transform created in Create_Poly. */

  Destroy_Transform(New->Trans);

  Copy_Flag(New, Object, STURM_FLAG);
  Copy_Flag(New, Object, INVERTED_FLAG);

  New->Trans = Copy_Transform(((POLY *)Object)->Trans);

  for (i = 0; i < term_counts(New->Order); i++)
  {
    New->Coeffs[i] = ((POLY *)Object)->Coeffs[i];
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Poly
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

static void Destroy_Poly(OBJECT *Object)
{
  Destroy_Transform (((POLY *)Object)->Trans);

  POV_FREE (((POLY *)Object)->Coeffs);

  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Poly_BBox
*
* INPUT
*
*   Poly - Poly
*   
* OUTPUT
*
*   Poly
*
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a poly.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Compute_Poly_BBox(POLY *Poly)
{
  Make_BBox(Poly->BBox, -BOUND_HUGE/2, -BOUND_HUGE/2, -BOUND_HUGE/2, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);

  if (Poly->Clip != NULL)
  {
    Poly->BBox = Poly->Clip->BBox;
  }
}

