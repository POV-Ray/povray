/*****************************************************************************
*                fractal.c
*
*  This module implements the fractal sets primitive.
*
*  This file was written by Pascal Massimino.
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
#include "povray.h"
#include "vector.h"
#include "povproto.h"
#include "bbox.h"
#include "matrices.h"
#include "objects.h"
#include "spheres.h"
#include "fractal.h"
#include "quatern.h"
#include "hcmplx.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#ifndef Fractal_Tolerance
#define Fractal_Tolerance 1e-7
#endif



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int All_Fractal_Intersections (OBJECT * Object, RAY * Ray, ISTACK * Depth_Stack);
static int Inside_Fractal (VECTOR IPoint, OBJECT * Object);
static void Fractal_Normal (VECTOR Result, OBJECT * Object, INTERSECTION * Intersect);
static FRACTAL *Copy_Fractal (OBJECT * Object);
static void Translate_Fractal (OBJECT * Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Fractal (OBJECT * Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Fractal (OBJECT * Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Fractal (OBJECT * Object, TRANSFORM * Trans);
static void Invert_Fractal (OBJECT * Object);
static void Destroy_Fractal (OBJECT * Object);
static void Compute_Fractal_BBox (FRACTAL * Fractal);

/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Fractal_Methods =
{
  All_Fractal_Intersections,
  Inside_Fractal, Fractal_Normal,
  (COPY_METHOD)Copy_Fractal,
  Translate_Fractal, Rotate_Fractal,
  Scale_Fractal, Transform_Fractal, Invert_Fractal,
  Destroy_Fractal
};

static int Allocated_Iteration_Stack_Length = 0;

DBL *Sx = NULL, *Sy = NULL, *Sz = NULL, *Sw = NULL;
DBL Precision;
VECTOR Direction;

static COMPLEX_FUNCTION_METHOD Complex_Function_List[] = 
{
  /* must match STYPE list in fractal.h */
  Complex_Exp,
  Complex_Log,
  Complex_Sin,
  Complex_ASin,
  Complex_Cos,
  Complex_ACos,
  Complex_Tan,
  Complex_ATan,
  Complex_Sinh,
  Complex_ASinh,
  Complex_Cosh,
  Complex_ACosh,
  Complex_Tanh,
  Complex_ATanh,
  Complex_Pwr 
};

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

static int All_Fractal_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int Intersection_Found;
  int Last = 0;
  int CURRENT, NEXT;
  DBL Depth, Depth_Max;
  DBL Dist, Dist_Next, Len;

  VECTOR IPoint, Mid_Point, Next_Point, Real_Pt;
  VECTOR Real_Normal, F_Normal;
  RAY New_Ray;
  FRACTAL *Fractal = (FRACTAL *) Object;

  Increase_Counter(stats[Ray_Fractal_Tests]);

  Intersection_Found = FALSE;
  Precision = Fractal->Precision;

  /* Get into Fractal's world. */

  if (Fractal->Trans != NULL)
  {
    MInvTransDirection(Direction, Ray->Direction, Fractal->Trans);
    VDot(Len, Direction, Direction);

    if (Len == 0.0)
    {
      return (FALSE);
    }

    if (Len != 1.0)
    {
      Len = 1.0 / sqrt(Len);
      VScaleEq(Direction, Len);
    }

    Assign_Vector(New_Ray.Direction, Direction);
    MInvTransPoint(New_Ray.Initial, Ray->Initial, Fractal->Trans);
  }
  else
  {
    Assign_Vector(Direction, Ray->Direction);
    New_Ray = *Ray;
    Len = 1.0;
  }

  /* Bound fractal. */

  if (!F_Bound(&New_Ray, Fractal, &Depth, &Depth_Max))
  {
    return (FALSE);
  }

  if (Depth_Max < Fractal_Tolerance)
  {
    return (FALSE);
  }

  if (Depth < Fractal_Tolerance)
  {
    Depth = Fractal_Tolerance;
  }

  /* Jump to starting point */

  VScale(Next_Point, Direction, Depth);
  VAddEq(Next_Point, New_Ray.Initial);

  CURRENT = D_Iteration(Next_Point, Fractal, &Dist);

  /* Light ray starting inside ? */

  if (CURRENT)
  {
    VAddScaledEq(Next_Point, 2.0 * Fractal_Tolerance, Direction);

    Depth += 2.0 * Fractal_Tolerance;

    if (Depth > Depth_Max)
    {
      return (FALSE);
    }

    CURRENT = D_Iteration(Next_Point, Fractal, &Dist);
  }

  /* Ok. Trace it */

  while (Depth < Depth_Max)
  {
    /*
     * Get close to the root: Advance with Next_Point, keeping track of last
     * position in IPoint...
     */

    while (1)
    {
      if (Dist < Precision)
      {
        Dist = Precision;
      }

      Depth += Dist;

      if (Depth > Depth_Max)
      {
        if (Intersection_Found)
        {
          Increase_Counter(stats[Ray_Fractal_Tests_Succeeded]);
        }

        return (Intersection_Found);
      }

      Assign_Vector(IPoint, Next_Point);
      VAddScaledEq(Next_Point, Dist, Direction);

      NEXT = D_Iteration(Next_Point, Fractal, &Dist_Next);

      if (NEXT != CURRENT)
      {
        /* Set surface was crossed... */

        Depth -= Dist;
        break;
      }
      else
      {
        Dist = Dist_Next; /* not reached */
      }
    }

    /* then, polish the root via bisection method... */

    while (Dist > Fractal_Tolerance)
    {
      Dist *= 0.5;
      VAddScaled(Mid_Point, IPoint, Dist, Direction);

      Last = Iteration(Mid_Point, Fractal);

      if (Last == CURRENT)
      {
        Assign_Vector(IPoint, Mid_Point);

        Depth += Dist;

        if (Depth > Depth_Max)
        {
          if (Intersection_Found)
          {
            Increase_Counter(stats[Ray_Fractal_Tests_Succeeded]);
          }

          return (Intersection_Found);
        }
      }
    }

    if (CURRENT == FALSE) /* Mid_Point isn't inside the set */
    {
      VAddScaledEq(IPoint, Dist, Direction);

      Depth += Dist;

      Iteration(IPoint, Fractal);
    }
    else
    {
      if (Last != CURRENT)
      {
        Iteration(IPoint, Fractal);
      }
    }

    if (Fractal->Trans != NULL)
    {
      MTransPoint(Real_Pt, IPoint, Fractal->Trans);
      Normal_Calc(Fractal, F_Normal);
      MTransNormal(Real_Normal, F_Normal, Fractal->Trans);
    }
    else
    {
      Assign_Vector(Real_Pt, IPoint);
      Normal_Calc(Fractal, Real_Normal);
    }

    if (Point_In_Clip(Real_Pt, Object->Clip))
    {
      VNormalize(Real_Normal, Real_Normal);
      push_normal_entry(Depth * Len, Real_Pt, Real_Normal, Object, Depth_Stack);
      Intersection_Found = TRUE;

      /* If fractal isn't used with CSG we can exit now. */

      if (!(Fractal->Type & IS_CHILD_OBJECT))
      {
        break;
      }
    }

    /* Start over where work was left */

    Assign_Vector(IPoint, Next_Point);
    Dist = Dist_Next;
    CURRENT = NEXT;

  }

  if (Intersection_Found)
  {
    Increase_Counter(stats[Ray_Fractal_Tests_Succeeded]);
  }
  return (Intersection_Found);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

static int Inside_Fractal(VECTOR IPoint, OBJECT *Object)
{
  FRACTAL *Fractal = (FRACTAL *) Object;
  int Result;
  static VECTOR New_Point;

  if (Fractal->Trans != NULL)
  {
    MInvTransPoint(New_Point, IPoint, Fractal->Trans);

    Result = Iteration(New_Point, Fractal);
  }
  else
  {
    Result = Iteration(IPoint, Fractal);
  }

  if (Fractal->Inverted)
  {
    return (!Result);
  }
  else
  {
    return (Result);
  }
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

static void Fractal_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Intersect)
{
  Assign_Vector(Result, Intersect->INormal);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

static void Translate_Fractal(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Fractal(Object, Trans);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

static void Rotate_Fractal(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Fractal(Object, Trans);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

static void Scale_Fractal(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Fractal(Object, Trans);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*   Mar 1996 : Moved call to Recompute_BBox to Compute_Fractal_BBox() (TW)
*
******************************************************************************/

static void Transform_Fractal(OBJECT *Object, TRANSFORM *Trans)
{
  FRACTAL *Fractal = (FRACTAL *) Object;

  if (((FRACTAL *) Object)->Trans == NULL)
  {
    ((FRACTAL *) Object)->Trans = Create_Transform();
  }

  Compose_Transforms(Fractal->Trans, Trans);

  Compute_Fractal_BBox((FRACTAL *) Object);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

static void Invert_Fractal(OBJECT *Object)
{
  ((FRACTAL *) Object)->Inverted ^= TRUE;
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*   Mar 1996 : Added call to recompute_BBox() to bottom (TW)
*
******************************************************************************/

static void Compute_Fractal_BBox(FRACTAL *Fractal)
{
  DBL R;

  switch (Fractal->Algebra)
  {
    case QUATERNION_TYPE:

      R = 1.0 + sqrt(Sqr(Fractal->Julia_Parm[X]) + Sqr(Fractal->Julia_Parm[Y]) + Sqr(Fractal->Julia_Parm[Z]) + Sqr(Fractal->Julia_Parm[T]));
      R += Fractal_Tolerance; /* fix bug when Julia_Parameter exactly 0 */

      if (R > 2.0)
      {
        R = 2.0;
      }

      Fractal->Exit_Value = Sqr(R);

      break;

    case HYPERCOMPLEX_TYPE:
    default:

      R = 4.0;

      Fractal->Exit_Value = 16.0;

      break;
  }

  Fractal->Radius_Squared = Sqr(R);

  Fractal->Inverted = FALSE;

  Make_BBox(Fractal->BBox, -R, -R, -R, 2.0 * R, 2.0 * R, 2.0 * R);

  Recompute_BBox(&Fractal->BBox, Fractal->Trans);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

FRACTAL *Create_Fractal()
{
  FRACTAL *New;

  New = (FRACTAL *) POV_MALLOC(sizeof(FRACTAL), "Fractal Set");

  INIT_OBJECT_FIELDS(New, BASIC_OBJECT, &Fractal_Methods)

  New->Trans = NULL;

  Make_Vector(New->Center, 0.0, 0.0, 0.0);

  New->Julia_Parm[X] = 1.0;
  New->Julia_Parm[Y] = 0.0;
  New->Julia_Parm[Z] = 0.0;
  New->Julia_Parm[T] = 0.0;

  New->Slice[X] = 0.0;
  New->Slice[Y] = 0.0;
  New->Slice[Z] = 0.0;
  New->Slice[T] = 1.0;
  New->SliceDist = 0.0;

  New->Exit_Value = 4.0;

  New->n = 20;

  New->Precision = 1.0 / 20.0;

  New->Inverted = FALSE;

  New->Algebra = QUATERNION_TYPE;

  New->Sub_Type = SQR_STYPE;

  New->Bound = NULL;

  New->Clip = NULL;

  New->Normal_Calc_Method = NULL;
  New->Iteration_Method   = NULL;
  New->D_Iteration_Method = NULL;
  New->F_Bound_Method     = NULL;
  New->Complex_Function_Method = NULL;

  New->Radius_Squared = 0.0;
  New->exponent.x = 0.0;
  New->exponent.y = 0.0;
  
  return (New);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

static FRACTAL *Copy_Fractal(OBJECT *Object)
{
  FRACTAL *New;

  New = Create_Fractal();

  *New = *((FRACTAL *) Object);

  New->Trans = Copy_Transform(((FRACTAL *) Object)->Trans);

  return (New);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

static void Destroy_Fractal(OBJECT *Object)
{
  Destroy_Transform(((FRACTAL *) Object)->Trans);
  POV_FREE(Object);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void SetUp_Fractal(FRACTAL *Fractal)
{
  switch (Fractal->Algebra)
  {
    case QUATERNION_TYPE:

      switch(Fractal->Sub_Type)
      {
        case CUBE_STYPE:
          Fractal->Iteration_Method = Iteration_z3;
          Fractal->Normal_Calc_Method = Normal_Calc_z3;
          Fractal->D_Iteration_Method = D_Iteration_z3;
          break;
        case SQR_STYPE:
          Fractal->Iteration_Method = Iteration_Julia;
          Fractal->D_Iteration_Method = D_Iteration_Julia;
          Fractal->Normal_Calc_Method = Normal_Calc_Julia;
          break;
        default:
          Error("illegal function: quaternion only supports sqr and cube");
      }
      Fractal->F_Bound_Method = F_Bound_Julia;

      break;

    case HYPERCOMPLEX_TYPE:

      switch (Fractal->Sub_Type)
      {
        case RECIPROCAL_STYPE:

          Fractal->Iteration_Method = Iteration_HCompl_Reciprocal;
          Fractal->Normal_Calc_Method = Normal_Calc_HCompl_Reciprocal;
          Fractal->D_Iteration_Method = D_Iteration_HCompl_Reciprocal;
          Fractal->F_Bound_Method = F_Bound_HCompl_Reciprocal;

          break;

        case EXP_STYPE: 
        case LOG_STYPE: 
        case SIN_STYPE: 
        case ASIN_STYPE:
        case COS_STYPE: 
        case ACOS_STYPE:
        case TAN_STYPE: 
        case ATAN_STYPE:
        case SINH_STYPE:
        case ASINH_STYPE:
        case COSH_STYPE:
        case ACOSH_STYPE:
        case TANH_STYPE:
        case ATANH_STYPE:
        case PWR_STYPE: 

          Fractal->Iteration_Method = Iteration_HCompl_Func;
          Fractal->Normal_Calc_Method = Normal_Calc_HCompl_Func;
          Fractal->D_Iteration_Method = D_Iteration_HCompl_Func;
          Fractal->F_Bound_Method = F_Bound_HCompl_Func;
          Fractal->Complex_Function_Method = Complex_Function_List[Fractal->Sub_Type];

          break;

        case CUBE_STYPE:

          Fractal->Iteration_Method = Iteration_HCompl_z3;
          Fractal->Normal_Calc_Method = Normal_Calc_HCompl_z3;
          Fractal->D_Iteration_Method = D_Iteration_HCompl_z3;
          Fractal->F_Bound_Method = F_Bound_HCompl_z3;

          break;

        default:  /* SQR_STYPE or else... */

          Fractal->Iteration_Method = Iteration_HCompl;
          Fractal->Normal_Calc_Method = Normal_Calc_HCompl;
          Fractal->D_Iteration_Method = D_Iteration_HCompl;
          Fractal->F_Bound_Method = F_Bound_HCompl;

          break;
      }

      break;

    default:

      Error("Algebra unknown in fractal.");
  }

  Allocate_Iteration_Stack(Fractal->n);

  Compute_Fractal_BBox(Fractal);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Allocate_Iteration_Stack(int n)
{
  if (n > Allocated_Iteration_Stack_Length)
  {
    Sx = (DBL *) POV_REALLOC(Sx, (n + 1) * sizeof(DBL), "x iteration stack");
    Sy = (DBL *) POV_REALLOC(Sy, (n + 1) * sizeof(DBL), "y iteration stack");
    Sz = (DBL *) POV_REALLOC(Sz, (n + 1) * sizeof(DBL), "w iteration stack");
    Sw = (DBL *) POV_REALLOC(Sw, (n + 1) * sizeof(DBL), "z iteration stack");

    Allocated_Iteration_Stack_Length = n;
  }
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Free_Iteration_Stack()
{
  if (Sx != NULL)
  {
    POV_FREE(Sx);
    POV_FREE(Sy);
    POV_FREE(Sz);
    POV_FREE(Sw);
  }

  Sx = NULL;
  Sy = NULL;
  Sz = NULL;
  Sw = NULL;

  Allocated_Iteration_Stack_Length = 0;
}

  
