/****************************************************************************
*                quadrics.c
*
*  This module implements the code for the quadric shape primitive.
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

#include "frame.h"
#include "povray.h"
#include "vector.h"
#include "povproto.h"
#include "bbox.h"
#include "objects.h"
#include "matrices.h"
#include "planes.h"
#include "quadrics.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* The following defines make typing much easier! [DB 7/94] */

#define Xd Ray->Direction[X]
#define Yd Ray->Direction[Y]
#define Zd Ray->Direction[Z]

#define Xo Ray->Initial[X]
#define Yo Ray->Initial[Y]
#define Zo Ray->Initial[Z]

#define QA Quadric->Square_Terms[X]
#define QE Quadric->Square_Terms[Y]
#define QH Quadric->Square_Terms[Z]

#define QB Quadric->Mixed_Terms[X]
#define QC Quadric->Mixed_Terms[Y]
#define QF Quadric->Mixed_Terms[Z]

#define QD Quadric->Terms[X]
#define QG Quadric->Terms[Y]
#define QI Quadric->Terms[Z]

#define QJ Quadric->Constant



/*****************************************************************************
* Static functions
******************************************************************************/

static int Intersect_Quadric (RAY *Ray, QUADRIC *Quadric, DBL *Depth1, DBL *Depth2);
static void Quadric_To_Matrix (QUADRIC *Quadric, MATRIX Matrix);
static void Matrix_To_Quadric (MATRIX Matrix, QUADRIC *Quadric);
static int All_Quadric_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int Inside_Quadric (VECTOR IPoint, OBJECT *Object);
static void Quadric_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static QUADRIC *Copy_Quadric (OBJECT *Object);
static void Translate_Quadric (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Quadric (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Quadric (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Quadric (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Quadric (OBJECT *Object);
static void Destroy_Quadric (OBJECT *Object);

/*****************************************************************************
* Local variables
******************************************************************************/

METHODS Quadric_Methods =
{
  All_Quadric_Intersections,
  Inside_Quadric, Quadric_Normal,
  (COPY_METHOD)Copy_Quadric,
  Translate_Quadric, Rotate_Quadric,
  Scale_Quadric, Transform_Quadric, Invert_Quadric,
  Destroy_Quadric
};




/*****************************************************************************
*
* FUNCTION
*
*   All_Quadric_Intersections
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static int All_Quadric_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  DBL Depth1, Depth2;
  VECTOR IPoint;
  register int Intersection_Found;

  Intersection_Found = FALSE;

  if (Intersect_Quadric(Ray, (QUADRIC *)Object, &Depth1, &Depth2))
  {
    if ((Depth1 > Small_Tolerance) && (Depth1 < Max_Distance))
    {
      VEvaluateRay(IPoint, Ray->Initial, Depth1, Ray->Direction);

      if (Point_In_Clip(IPoint, Object->Clip))
      {
        push_entry(Depth1, IPoint, Object, Depth_Stack);

        Intersection_Found = TRUE;
      }
    }

    if ((Depth2 > Small_Tolerance) && (Depth2 < Max_Distance))
    {
      VEvaluateRay(IPoint, Ray->Initial, Depth2, Ray->Direction);

      if (Point_In_Clip(IPoint, Object->Clip))
      {
        push_entry(Depth2, IPoint, Object, Depth_Stack);

        Intersection_Found = TRUE;
      }
    }
  }

  return(Intersection_Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Quadric
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static int Intersect_Quadric(RAY *Ray, QUADRIC *Quadric, DBL *Depth1, DBL  *Depth2)
{
  register DBL a, b, c, d;

  Increase_Counter(stats[Ray_Quadric_Tests]);

  a = Xd * (QA * Xd + QB * Yd + QC * Zd) +
                Yd * (QE * Yd + QF * Zd) +
                QH * Zd * Zd;

  b = Xd * (QA * Xo + 0.5 * (QB * Yo + QC * Zo + QD)) +
                Yd * (QE * Yo + 0.5 * (QB * Xo + QF * Zo + QG)) +
                Zd * (QH * Zo + 0.5 * (QC * Xo + QF * Yo + QI));

  c = Xo * (QA * Xo + QB * Yo + QC * Zo + QD) +
                  Yo * (QE * Yo + QF * Zo + QG) +
      Zo * (QH * Zo + QI) +
      QJ;

  if (a != 0.0)
  {
    /* The equation is quadratic - find its roots */

    d = Sqr(b) - a * c;

    if (d <= 0.0)
    {
      return(FALSE);
    }

    d = sqrt (d);

    *Depth1 = (-b + d) / (a);
    *Depth2 = (-b - d) / (a);
  }
  else
  {
    /* There are no quadratic terms. Solve the linear equation instead. */

    if (b == 0.0)
    {
      return(FALSE);
    }

    *Depth1 = - 0.5 * c / b;
    *Depth2 = Max_Distance;
  }

  Increase_Counter(stats[Ray_Quadric_Tests_Succeeded]);

  return(TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Quadric
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static int Inside_Quadric(VECTOR IPoint, OBJECT *Object)
{
  QUADRIC *Quadric = (QUADRIC *)Object;

  /* This is faster and shorter. [DB 7/94] */

  return((IPoint[X] * (QA * IPoint[X] + QB * IPoint[Y] + QD) +
          IPoint[Y] * (QE * IPoint[Y] + QF * IPoint[Z] + QG) +
          IPoint[Z] * (QH * IPoint[Z] + QC * IPoint[X] + QI) + QJ) <= 0.0);
}



/*****************************************************************************
*
* FUNCTION
*
*   Quadric_Normal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static void Quadric_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  QUADRIC *Quadric = (QUADRIC *) Object;
  DBL Len;

  /* This is faster and shorter. [DB 7/94] */

  Result[X] = 2.0 * QA * Inter->IPoint[X] +
                    QB * Inter->IPoint[Y] +
                    QC * Inter->IPoint[Z] +
                    QD;

  Result[Y] =       QB * Inter->IPoint[X] +
              2.0 * QE * Inter->IPoint[Y] +
                    QF * Inter->IPoint[Z] +
                    QG;

  Result[Z] =       QC * Inter->IPoint[X] +
                    QF * Inter->IPoint[Y] +
              2.0 * QH * Inter->IPoint[Z] +
                    QI;

  VLength(Len, Result);

  if (Len == 0.0)
  {
    /* The normal is not defined at this point of the surface. */
    /* Set it to any arbitrary direction. */

    Make_Vector(Result, 1.0, 0.0, 0.0);
  }
  else
  {
    VInverseScaleEq(Result, Len);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Quadric
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static void Transform_Quadric(OBJECT *Object, TRANSFORM *Trans)
{
  QUADRIC *Quadric=(QUADRIC *)Object;
  MATRIX Quadric_Matrix, Transform_Transposed;

  Quadric_To_Matrix (Quadric, Quadric_Matrix);

  MTimes (Quadric_Matrix, Trans->inverse, Quadric_Matrix);
  MTranspose (Transform_Transposed, Trans->inverse);
  MTimes (Quadric_Matrix, Quadric_Matrix, Transform_Transposed);

  Matrix_To_Quadric (Quadric_Matrix, Quadric);

  Recompute_BBox(&Object->BBox, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Quadric_To_Matrix
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static void Quadric_To_Matrix(QUADRIC *Quadric, MATRIX Matrix)
{
  MZero (Matrix);

  Matrix[0][0] = Quadric->Square_Terms[X];
  Matrix[1][1] = Quadric->Square_Terms[Y];
  Matrix[2][2] = Quadric->Square_Terms[Z];
  Matrix[0][1] = Quadric->Mixed_Terms[X];
  Matrix[0][2] = Quadric->Mixed_Terms[Y];
  Matrix[0][3] = Quadric->Terms[X];
  Matrix[1][2] = Quadric->Mixed_Terms[Z];
  Matrix[1][3] = Quadric->Terms[Y];
  Matrix[2][3] = Quadric->Terms[Z];
  Matrix[3][3] = Quadric->Constant;
}



/*****************************************************************************
*
* FUNCTION
*
*   Matrix_To_Quadric
*
* INPUT
*   
* OUTPUT
*
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static void Matrix_To_Quadric(MATRIX Matrix, QUADRIC *Quadric)
{
  Quadric->Square_Terms[X] = Matrix[0][0];
  Quadric->Square_Terms[Y] = Matrix[1][1];
  Quadric->Square_Terms[Z] = Matrix[2][2];

  Quadric->Mixed_Terms[X] = Matrix[0][1] + Matrix[1][0];
  Quadric->Mixed_Terms[Y] = Matrix[0][2] + Matrix[2][0];
  Quadric->Mixed_Terms[Z] = Matrix[1][2] + Matrix[2][1];

  Quadric->Terms[X] = Matrix[0][3] + Matrix[3][0];
  Quadric->Terms[Y] = Matrix[1][3] + Matrix[3][1];
  Quadric->Terms[Z] = Matrix[2][3] + Matrix[3][2];

  Quadric->Constant = Matrix[3][3];
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Quadric
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static void Translate_Quadric(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Quadric (Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Quadric
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static void Rotate_Quadric(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Quadric (Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Quadric
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static void Scale_Quadric(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Quadric (Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Quadric
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static void Invert_Quadric(OBJECT *Object)
{
  QUADRIC *Quadric = (QUADRIC *) Object;

  VScaleEq(Quadric->Square_Terms, -1.0);
  VScaleEq(Quadric->Mixed_Terms, -1.0);
  VScaleEq(Quadric->Terms, -1.0);

  Quadric->Constant *= -1.0;

  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Quadric
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

QUADRIC *Create_Quadric()
{
  QUADRIC *New;

  New = (QUADRIC *)POV_MALLOC(sizeof (QUADRIC), "quadric");

  INIT_OBJECT_FIELDS(New, QUADRIC_OBJECT, &Quadric_Methods)

  Make_Vector (New->Square_Terms, 1.0, 1.0, 1.0);
  Make_Vector (New->Mixed_Terms, 0.0, 0.0, 0.0);
  Make_Vector (New->Terms, 0.0, 0.0, 0.0);
  New->Constant = 1.0;

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Quadric
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static QUADRIC *Copy_Quadric(OBJECT *Object)
{
  QUADRIC *New;

  New = Create_Quadric();

  *New = *((QUADRIC *)Object);

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Quadric
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
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

static void Destroy_Quadric(OBJECT *Object)
{
  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Quadric_BBox
*
* INPUT
*
*   Quadric - Qaudric object
*   
* OUTPUT
*
*   Quadric
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Compute a bounding box around a quadric.
*
*   This function calculates the bounding box of a quadric given in
*   its normal form, i.e. f(x,y,z) = A*x^2 + E*y^2 + H*z^2 + J = 0.
*
*   NOTE: Translated quadrics can also be bounded by this function.
*
*         Supported: cones, cylinders, ellipsoids, hyperboloids.
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Sep 1994 : Added support of hyperpoloids. Improved bounding of
*              quadrics used in CSG intersections. [DB]
*
******************************************************************************/

void Compute_Quadric_BBox(QUADRIC *Quadric, VECTOR ClipMin, VECTOR  ClipMax)
{
  DBL A, B, C, D, E, F, G, H, I, J;
  DBL a, b, c, d;
  DBL rx, ry, rz, rx1, rx2, ry1, ry2, rz1, rz2, x, y, z;
  DBL New_Volume, Old_Volume;
  VECTOR Min, Max, TmpMin, TmpMax, NewMin, NewMax, T1;
  BBOX Old;
  OBJECT *Sib;

  /*
   * Check for 'normal' form. If the quadric isn't in it's normal form
   * we can't do anything (we could, but that would be to tedious!
   * Diagonalising the quadric's 4x4 matrix, i.e. finding its eigenvalues
   * and eigenvectors -> solving a 4th order polynom).
   */

  /* Get quadrics coefficients. */

  A = Quadric->Square_Terms[X];
  E = Quadric->Square_Terms[Y];
  H = Quadric->Square_Terms[Z];
  B = Quadric->Mixed_Terms[X] / 2.0;
  C = Quadric->Mixed_Terms[Y] / 2.0;
  F = Quadric->Mixed_Terms[Z] / 2.0;
  D = Quadric->Terms[X] / 2.0;
  G = Quadric->Terms[Y] / 2.0;
  I = Quadric->Terms[Z] / 2.0;
  J = Quadric->Constant;

  /* Set small values to 0. */

  if (fabs(A) < EPSILON) A = 0.0;
  if (fabs(B) < EPSILON) B = 0.0;
  if (fabs(C) < EPSILON) C = 0.0;
  if (fabs(D) < EPSILON) D = 0.0;
  if (fabs(E) < EPSILON) E = 0.0;
  if (fabs(F) < EPSILON) F = 0.0;
  if (fabs(G) < EPSILON) G = 0.0;
  if (fabs(H) < EPSILON) H = 0.0;
  if (fabs(I) < EPSILON) I = 0.0;
  if (fabs(J) < EPSILON) J = 0.0;

  /* Non-zero mixed terms --> return */

  if ((B != 0.0) || (C != 0.0) || (F != 0.0))
  {
    return;
  }

  /* Non-zero linear terms --> get translation vector */

  if ((D != 0.0) || (G != 0.0) || (I != 0.0))
  {
    if (A != 0.0)
    {
      T1[X] = -D / A;
    }
    else
    {
      if (D != 0.0)
      {
       T1[X] = J / (2.0 * D);
      }
      else
      {
        T1[X] = 0.0;
      }
    }

    if (E != 0.0)
    {
      T1[Y] = -G / E;
    }
    else
    {
      if (G != 0.0)
      {
        T1[Y] = J / (2.0 * G);
      }
      else
      {
        T1[Y] = 0.0;
      }
    }

    if (H != 0.0)
    {
      T1[Z] = -I / H;
    }
    else
    {
      if (I != 0.0)
      {
        T1[Z] = J / (2.0 * I);
      }
      else
      {
        T1[Z] = 0.0;
      }
    }

    /* Recalculate coefficients. */

    D += A * T1[X];
    G += E * T1[Y];
    I += H * T1[Z];
    J -= T1[X]*(A*T1[X] + 2.0*D) + T1[Y]*(E*T1[Y] + 2.0*G) + T1[Z]*(H*T1[Z] + 2.0*I);
  }
  else
  {
    Make_Vector(T1, 0.0, 0.0, 0.0);
  }

  /* Get old bounding box. */

  Old = Quadric->BBox;

  /* Init new bounding box. */

  NewMin[X] = NewMin[Y] = NewMin[Z] = -BOUND_HUGE/2;
  NewMax[X] = NewMax[Y] = NewMax[Z] =  BOUND_HUGE/2;

  /* Get the bounding box of the clipping object. */

  if (Quadric->Clip != NULL)
  {
    Min[X] = Min[Y] = Min[Z] = -BOUND_HUGE;
    Max[X] = Max[Y] = Max[Z] =  BOUND_HUGE;

    /* Intersect the members bounding boxes. */

    for (Sib = Quadric->Clip; Sib != NULL; Sib = Sib->Sibling)
    {
      if (!Test_Flag(Sib, INVERTED_FLAG))
      {
        if (Sib->Methods == &Plane_Methods)
        {
          Compute_Plane_Min_Max((PLANE *)Sib, TmpMin, TmpMax);
        }
        else
        {
          Make_min_max_from_BBox(TmpMin, TmpMax, Sib->BBox);
        }

        Min[X] = max(Min[X], TmpMin[X]);
        Min[Y] = max(Min[Y], TmpMin[Y]);
        Min[Z] = max(Min[Z], TmpMin[Z]);
        Max[X] = min(Max[X], TmpMax[X]);
        Max[Y] = min(Max[Y], TmpMax[Y]);
        Max[Z] = min(Max[Z], TmpMax[Z]);
      }
    }

    Assign_Vector(ClipMin, Min);
    Assign_Vector(ClipMax, Max);
  }

  /* Translate clipping box. */

  VSubEq(ClipMin, T1);
  VSubEq(ClipMax, T1);

  /* We want A to be non-negative. */

  if (A < 0.0)
  {
    A = -A;
    D = -D;
    E = -E;
    G = -G;
    H = -H;
    I = -I;
    J = -J;
  }

  /*
   *
   * Check for ellipsoid.
   *
   *    x*x     y*y     z*z
   *   ----- + ----- + ----- - 1 = 0
   *    a*a     b*b     c*c
   *
   */

  if ((A > 0.0) && (E > 0.0) && (H > 0.0) && (J < 0.0))
  {
    a = sqrt(-J/A);
    b = sqrt(-J/E);
    c = sqrt(-J/H);

    NewMin[X] = -a;
    NewMin[Y] = -b;
    NewMin[Z] = -c;
    NewMax[X] = a;
    NewMax[Y] = b;
    NewMax[Z] = c;
  }

  /*
   *
   * Check for cylinder (x-axis).
   *
   *    y*y     z*z
   *   ----- + ----- - 1 = 0
   *    b*b     c*c
   *
   */

  if ((A == 0.0) && (E > 0.0) && (H > 0.0) && (J < 0.0))
  {
    b = sqrt(-J/E);
    c = sqrt(-J/H);

    NewMin[Y] = -b;
    NewMin[Z] = -c;
    NewMax[Y] = b;
    NewMax[Z] = c;
  }

  /*
   *
   * Check for cylinder (y-axis).
   *
   *    x*x     z*z
   *   ----- + ----- - 1 = 0
   *    a*a     c*c
   *
   */

  if ((A > 0.0) && (E == 0.0) && (H > 0.0) && (J < 0.0))
  {
    a = sqrt(-J/A);
    c = sqrt(-J/H);

    NewMin[X] = -a;
    NewMin[Z] = -c;
    NewMax[X] = a;
    NewMax[Z] = c;
  }

  /*
   *
   * Check for cylinder (z-axis).
   * 
   *    x*x     y*y
   *   ----- + ----- - 1 = 0
   *    a*a     b*b
   *
   */

  if ((A > 0.0) && (E > 0.0) && (H == 0.0) && (J < 0.0))
  {
    a = sqrt(-J/A);
    b = sqrt(-J/E);

    NewMin[X] = -a;
    NewMin[Y] = -b;
    NewMax[X] = a;
    NewMax[Y] = b;
  }

  /*
   *
   * Check for cone (x-axis).
   *
   *    x*x     y*y     z*z
   *   ----- - ----- - ----- = 0
   *    a*a     b*b     c*c
   *
   */

  if ((A > 0.0) && (E < 0.0) && (H < 0.0) && (J == 0.0))
  {
    a = sqrt(1.0/A);
    b = sqrt(-1.0/E);
    c = sqrt(-1.0/H);

    /* Get radii for lower x value. */

    x = ClipMin[X];

    ry1 = fabs(x * b / a);
    rz1 = fabs(x * c / a);

    /* Get radii for upper x value. */

    x = ClipMax[X];

    ry2 = fabs(x * b / a);
    rz2 = fabs(x * c / a);

    ry = max(ry1, ry2);
    rz = max(rz1, rz2);

    NewMin[Y] = -ry;
    NewMin[Z] = -rz;
    NewMax[Y] = ry;
    NewMax[Z] = rz;
  }

  /*
   *
   *  Check for cone (y-axis).
   *
   *    x*x     y*y     z*z
   *   ----- - ----- + ----- = 0
   *    a*a     b*b     c*c
   *
   */

  if ((A > 0.0) && (E < 0.0) && (H > 0.0) && (J == 0.0))
  {
    a = sqrt(1.0/A);
    b = sqrt(-1.0/E);
    c = sqrt(1.0/H);

    /* Get radii for lower y value. */

    y = ClipMin[Y];

    rx1 = fabs(y * a / b);
    rz1 = fabs(y * c / b);

    /* Get radii for upper y value. */

    y = ClipMax[Y];

    rx2 = fabs(y * a / b);
    rz2 = fabs(y * c / b);

    rx = max(rx1, rx2);
    rz = max(rz1, rz2);

    NewMin[X] = -rx;
    NewMin[Z] = -rz;
    NewMax[X] = rx;
    NewMax[Z] = rz;
  }

  /*
   *
   * Check for cone (z-axis).
   * 
   *    x*x     y*y     z*z
   *   ----- + ----- - ----- = 0
   *    a*a     b*b     c*c
   *
   */

  if ((A > 0.0) && (E > 0.0) && (H < 0.0) && (J == 0.0))
  {
    a = sqrt(1.0/A);
    b = sqrt(1.0/E);
    c = sqrt(-1.0/H);

    /* Get radii for lower z value. */

    z = ClipMin[Z];

    rx1 = fabs(z * a / c);
    ry1 = fabs(z * b / c);

    /* Get radii for upper z value. */

    z = ClipMax[Z];

    rx2 = fabs(z * a / c);
    ry2 = fabs(z * b / c);

    rx = max(rx1, rx2);
    ry = max(ry1, ry2);

    NewMin[X] = -rx;
    NewMin[Y] = -ry;
    NewMax[X] = rx;
    NewMax[Y] = ry;
  }

  /*
   *
   * Check for hyperboloid (x-axis).
   *
   *    x*x     y*y     z*z
   *   ----- - ----- - ----- + 1 = 0
   *    a*a     b*b     c*c
   *
   */

  if ((A > 0.0) && (E < 0.0) && (H < 0.0) && (J > 0.0))
  {
    /* Get radii for lower x value. */

    x = ClipMin[X];

    d = 1.0 + A * Sqr(x);

    ry1 = sqrt(-d / E);
    rz1 = sqrt(-d / H);

    /* Get radii for upper x value. */

    x = ClipMax[X];

    d = 1.0 + A * Sqr(x);

    ry2 = sqrt(-d / E);
    rz2 = sqrt(-d / H);

    ry = max(ry1, ry2);
    rz = max(rz1, rz2);

    NewMin[Y] = -ry;
    NewMin[Z] = -rz;
    NewMax[Y] = ry;
    NewMax[Z] = rz;
  }

  /*
   *
   * Check for hyperboloid (y-axis).
   * 
   *    x*x     y*y     z*z
   *   ----- - ----- + ----- - 1 = 0
   *    a*a     b*b     c*c
   *
   */

  if ((A > 0.0) && (E < 0.0) && (H > 0.0) && (J < 0.0))
  {
    /* Get radii for lower y value. */

    y = ClipMin[Y];

    d = 1.0 - E * Sqr(y);

    rx1 = sqrt(d / A);
    rz1 = sqrt(d / H);

    /* Get radii for upper y value. */

    y = ClipMax[Y];

    d = 1.0 - E * Sqr(y);

    rx2 = sqrt(d / A);
    rz2 = sqrt(d / H);

    rx = max(rx1, rx2);
    rz = max(rz1, rz2);

    NewMin[X] = -rx;
    NewMin[Z] = -rz;
    NewMax[X] = rx;
    NewMax[Z] = rz;
  }

  /*
   *
   * Check for hyperboloid (z-axis).
   *
   *    x*x     y*y     z*z
   *   ----- + ----- - ----- - 1 = 0
   *    a*a     b*b     c*c
   *
   */

  if ((A > 0.0) && (E > 0.0) && (H < 0.0) && (J < 0.0))
  {
    /* Get radii for lower z value. */

    z = ClipMin[Z];

    d = 1.0 - H * Sqr(z);

    rx1 = sqrt(d / A);
    ry1 = sqrt(d / E);

    /* Get radii for upper z value. */

    z = ClipMax[Z];

    d = 1.0 - H * Sqr(z);

    rx2 = sqrt(d / A);
    ry2 = sqrt(d / E);

    rx = max(rx1, rx2);
    ry = max(ry1, ry2);

    NewMin[X] = -rx;
    NewMin[Y] = -ry;
    NewMax[X] = rx;
    NewMax[Y] = ry;
  }

  /*
   *
   * Check for paraboloid (x-axis).
   *
   *        y*y     z*z
   *   x - ----- - ----- = 0
   *        b*b     c*c
   *
   */

  if ((A == 0.0) && (D != 0.0) && (E != 0.0) && (H != 0.0) && (J == 0.0))
  {
    /* Get radii for lower x value. */

    x = ClipMin[X];

    ry1 = sqrt(fabs(2.0 * D * x / E));
    rz1 = sqrt(fabs(2.0 * D * x / H));

    /* Get radii for upper x value. */

    x = ClipMax[X];

    ry2 = sqrt(fabs(2.0 * D * x / E));
    rz2 = sqrt(fabs(2.0 * D * x / H));

    ry = max(ry1, ry2);
    rz = max(rz1, rz2);

    NewMin[Y] = -ry;
    NewMin[Z] = -rz;
    NewMax[Y] = ry;
    NewMax[Z] = rz;
  }

  /*
   *
   * Check for paraboloid (y-axis).
   *
   *        x*x     z*z
   *   y - ----- - ----- = 0
   *        a*a     c*c
   *
   */

  if ((E == 0.0) && (G != 0.0) && (A != 0.0) && (H != 0.0) && (J == 0.0))
  {
    /* Get radii for lower y-value. */

    y = ClipMin[Y];

    rx1 = sqrt(fabs(2.0 * G * y / A));
    rz1 = sqrt(fabs(2.0 * G * y / H));

    /* Get radii for upper y value. */

    y = ClipMax[Y];

    rx2 = sqrt(fabs(2.0 * G * y / A));
    rz2 = sqrt(fabs(2.0 * G * y / H));

    rx = max(rx1, rx2);
    rz = max(rz1, rz2);

    NewMin[X] = -rx;
    NewMin[Z] = -rz;
    NewMax[X] = rx;
    NewMax[Z] = rz;
  }

  /*
   *
   * Check for paraboloid (z-axis).
   *
   *        x*x     y*y
   *   z - ----- - ----- = 0
   *        a*a     b*b
   *
   */

  if ((H == 0.0) && (I != 0.0) && (A != 0.0) && (E != 0.0) && (J == 0.0))
  {
    /* Get radii for lower z-value. */

    z = ClipMin[Z];

    rx1 = sqrt(fabs(2.0 * I * z / A));
    ry1 = sqrt(fabs(2.0 * I * z / E));

    /* Get radii for upper z value. */

    z = ClipMax[Z];

    rx2 = sqrt(fabs(2.0 * I * z / A));
    ry2 = sqrt(fabs(2.0 * I * z / E));

    rx = max(rx1, rx2);
    ry = max(ry1, ry2);

    NewMin[X] = -rx;
    NewMin[Y] = -ry;
    NewMax[X] = rx;
    NewMax[Y] = ry;
  }

  /* Intersect clipping object's and quadric's bounding boxes */

  NewMin[X] = max(NewMin[X], ClipMin[X]);
  NewMin[Y] = max(NewMin[Y], ClipMin[Y]);
  NewMin[Z] = max(NewMin[Z], ClipMin[Z]);

  NewMax[X] = min(NewMax[X], ClipMax[X]);
  NewMax[Y] = min(NewMax[Y], ClipMax[Y]);
  NewMax[Z] = min(NewMax[Z], ClipMax[Z]);

  /* Use old or new bounding box? */

  New_Volume = (NewMax[X] - NewMin[X]) * (NewMax[Y] - NewMin[Y]) * (NewMax[Z] - NewMin[Z]);

  BOUNDS_VOLUME(Old_Volume, Old);

  if (New_Volume < Old_Volume)
  {
    /* Add translation. */

    VAddEq(NewMin, T1);
    VAddEq(NewMax, T1);

    Make_BBox_from_min_max(Quadric->BBox, NewMin, NewMax);

    /* Beware of bounding boxes to large. */

    if ((Quadric->BBox.Lengths[X] > CRITICAL_LENGTH) ||
        (Quadric->BBox.Lengths[Y] > CRITICAL_LENGTH) ||
        (Quadric->BBox.Lengths[Z] > CRITICAL_LENGTH))
    {
      Make_BBox(Quadric->BBox, -BOUND_HUGE/2, -BOUND_HUGE/2, -BOUND_HUGE/2,
        BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Plane_Min_Max
*
* INPUT
*
*   Plane    - Plane
*   Min, Max - Vectors containing plane's dimensions
*   
* OUTPUT
*
*   Min, Max
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Compute min/max vectors for planes perpendicular to an axis.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void Compute_Plane_Min_Max(PLANE *Plane, VECTOR Min, VECTOR  Max)
{
  DBL d;
  VECTOR P, N;

  if (Plane->Trans == NULL)
  {
    Assign_Vector(N, Plane->Normal_Vector);

    d = -Plane->Distance;
  }
  else
  {
    MInvTransNormal(N, Plane->Normal_Vector, Plane->Trans);

    MInvTransPoint(P, N, Plane->Trans);

    d = -Plane->Distance - P[X] * N[X] - P[Y] * N[Y] - P[Z] * N[Z] + 1.0;
  }

  Min[X] = Min[Y] = Min[Z] = -BOUND_HUGE/2;
  Max[X] = Max[Y] = Max[Z] =  BOUND_HUGE/2;

  /* y-z-plane */

  if (fabs(1.0 - fabs(N[X])) < EPSILON)
  {
    if (N[X] > 0.0)
    {
      Max[X] = d;
    }
    else
    {
      Min[X] = -d;
    }
  }

  /* x-z-plane */

  if (fabs(1.0 - fabs(N[Y])) < EPSILON)
  {
    if (N[Y] > 0.0)
    {
      Max[Y] = d;
    }
    else
    {
      Min[Y] = -d;
    }
  }

  /* x-y-plane */

  if (fabs(1.0 - fabs(N[Z])) < EPSILON)
  {
    if (N[Z] > 0.0)
    {
      Max[Z] = d;
    }
    else
    {
      Min[Z] = -d;
    }
  }
}



