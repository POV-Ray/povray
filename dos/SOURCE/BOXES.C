/****************************************************************************
*                boxes.c
*
*  This module implements the box primitive.
*  This file was written by Alexander Enzmann.  He wrote the code for
*  boxes and generously provided us these enhancements.
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
#include "boxes.h"
#include "matrices.h"
#include "objects.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Minimal intersection depth. */

#define DEPTH_TOLERANCE 1.0e-6

/* Two values are equal if their difference is small than CLOSE_TOLERANCE. */

#define CLOSE_TOLERANCE 1.0e-6

/* Side hit. */

#define SIDE_X_0 1
#define SIDE_X_1 2
#define SIDE_Y_0 3
#define SIDE_Y_1 4
#define SIDE_Z_0 5
#define SIDE_Z_1 6



/*****************************************************************************
* Static functions
******************************************************************************/
static int  All_Box_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int  Inside_Box (VECTOR point, OBJECT *Object);
static void Box_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static void Translate_Box (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Box (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Box (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Box (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Box (OBJECT *Object);



/*****************************************************************************
* Local variables
******************************************************************************/

METHODS Box_Methods =
{
  All_Box_Intersections,
  Inside_Box, Box_Normal,
  (COPY_METHOD)Copy_Box, Translate_Box, Rotate_Box, Scale_Box, Transform_Box,
  Invert_Box, Destroy_Box
};



/*****************************************************************************
*
* FUNCTION
*
*   All_Box_Intersections
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

static int All_Box_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int Intersection_Found;
  int Side1, Side2;
  DBL Depth1, Depth2;
  VECTOR IPoint;

  Increase_Counter(stats[Ray_Box_Tests]);

  Intersection_Found = FALSE;

  if (Intersect_Box(Ray, (BOX *)Object, &Depth1, &Depth2, &Side1, &Side2))
  {
    if (Depth1 > DEPTH_TOLERANCE)
    {
      VEvaluateRay(IPoint, Ray->Initial, Depth1, Ray->Direction);

      if (Point_In_Clip(IPoint, Object->Clip))
      {
        push_entry_i1(Depth1,IPoint,Object,Side1,Depth_Stack);

        Intersection_Found = TRUE;
      }
    }

    VEvaluateRay(IPoint, Ray->Initial, Depth2, Ray->Direction);

    if (Point_In_Clip(IPoint, Object->Clip))
    {
      push_entry_i1(Depth2,IPoint,Object,Side2,Depth_Stack);

      Intersection_Found = TRUE;
    }
  }

  if (Intersection_Found)
  {
    Increase_Counter(stats[Ray_Box_Tests_Succeeded]);
  }

  return (Intersection_Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Box
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
*   Sep 1994 : Added code to decide which side was hit in the case
*              intersection points are close to each other. This removes
*              some ugly artefacts one could observe at the corners of
*              boxes due to the usage of the wrong normal vector. [DB]
*
******************************************************************************/

int Intersect_Box(RAY *Ray, BOX *box, DBL *Depth1, DBL  *Depth2, int *Side1, int  *Side2)
{
  int smin = 0, smax = 0;    /* Side hit for min/max intersection. */
  DBL t, tmin, tmax;
  VECTOR P, D;

  /* Transform the point into the boxes space */

  if (box->Trans != NULL)
  {
    MInvTransPoint(P, Ray->Initial, box->Trans);
    MInvTransDirection(D, Ray->Direction, box->Trans);
  }
  else
  {
    Assign_Vector(P, Ray->Initial);
    Assign_Vector(D, Ray->Direction);
  }

  tmin = 0.0;
  tmax = BOUND_HUGE;

  /*
   * Sides first.
   */

  if (D[X] < -EPSILON)
  {
    t = (box->bounds[0][X] - P[X]) / D[X];

    if (t < tmin) return(FALSE);

    if (t <= tmax)
    {
      smax = SIDE_X_0;
      tmax = t;
    }

    t = (box->bounds[1][X] - P[X]) / D[X];

    if (t >= tmin)
    {
      if (t > tmax) return(FALSE);

      smin = SIDE_X_1;
      tmin = t;
    }
  }
  else
  {
    if (D[X] > EPSILON)
    {
      t = (box->bounds[1][X] - P[X]) / D[X];

      if (t < tmin) return(FALSE);

      if (t <= tmax)
      {
        smax = SIDE_X_1;
        tmax = t;
      }

      t = (box->bounds[0][X] - P[X]) / D[X];

      if (t >= tmin)
      {
        if (t > tmax) return(FALSE);

        smin = SIDE_X_0;
        tmin = t;
      }
    }
    else
    {
      if ((P[X] < box->bounds[0][X]) || (P[X] > box->bounds[1][X]))
      {
        return(FALSE);
      }
    }
  }

  /*
   * Check Top/Bottom.
   */

  if (D[Y] < -EPSILON)
  {
    t = (box->bounds[0][Y] - P[Y]) / D[Y];

    if (t < tmin) return(FALSE);

    if (t <= tmax - CLOSE_TOLERANCE)
    {
      smax = SIDE_Y_0;
      tmax = t;
    }
    else
    {
      /*
       * If intersection points are close to each other find out
       * which side to use, i.e. is most probably hit. [DB 9/94]
       */

      if (t <= tmax + CLOSE_TOLERANCE)
      {
        if (-D[Y] > fabs(D[X])) smax = SIDE_Y_0;
      }
    }

    t = (box->bounds[1][Y] - P[Y]) / D[Y];

    if (t >= tmin + CLOSE_TOLERANCE)
    {
      if (t > tmax) return(FALSE);

      smin = SIDE_Y_1;
      tmin = t;
    }
    else
    {
      /*
       * If intersection points are close to each other find out
       * which side to use, i.e. is most probably hit. [DB 9/94]
       */

      if (t >= tmin - CLOSE_TOLERANCE)
      {
        if (-D[Y] > fabs(D[X])) smin = SIDE_Y_1;
      }
    }
  }
  else
  {
    if (D[Y] > EPSILON)
    {
      t = (box->bounds[1][Y] - P[Y]) / D[Y];

      if (t < tmin) return(FALSE);

      if (t <= tmax - CLOSE_TOLERANCE)
      {
        smax = SIDE_Y_1;
        tmax = t;
      }
      else
      {
        /*
         * If intersection points are close to each other find out
         * which side to use, i.e. is most probably hit. [DB 9/94]
         */

        if (t <= tmax + CLOSE_TOLERANCE)
        {
          if (D[Y] > fabs(D[X])) smax = SIDE_Y_1;
        }
      }

      t = (box->bounds[0][Y] - P[Y]) / D[Y];

      if (t >= tmin + CLOSE_TOLERANCE)
      {
        if (t > tmax) return(FALSE);

        smin = SIDE_Y_0;
        tmin = t;
      }
      else
      {
        /*
         * If intersection points are close to each other find out
         * which side to use, i.e. is most probably hit. [DB 9/94]
         */

        if (t >= tmin - CLOSE_TOLERANCE)
        {
          if (D[Y] > fabs(D[X])) smin = SIDE_Y_0;
        }
      }
    }
    else
    {
      if ((P[Y] < box->bounds[0][Y]) || (P[Y] > box->bounds[1][Y]))
      {
        return(FALSE);
      }
    }
  }

  /* Now front/back */

  if (D[Z] < -EPSILON)
  {
    t = (box->bounds[0][Z] - P[Z]) / D[Z];

    if (t < tmin) return(FALSE);

    if (t <= tmax - CLOSE_TOLERANCE)
    {
      smax = SIDE_Z_0;
      tmax = t;
    }
    else
    {
      /*
       * If intersection points are close to each other find out
       * which side to use, i.e. is most probably hit. [DB 9/94]
       */

      if (t <= tmax + CLOSE_TOLERANCE)
      {
        switch (smax)
        {
          case SIDE_X_0 :
          case SIDE_X_1 : if (-D[Z] > fabs(D[X])) smax = SIDE_Z_0; break;

          case SIDE_Y_0 :
          case SIDE_Y_1 : if (-D[Z] > fabs(D[Y])) smax = SIDE_Z_0; break;
        }
      }
    }

    t = (box->bounds[1][Z] - P[Z]) / D[Z];

    if (t >= tmin + CLOSE_TOLERANCE)
    {
      if (t > tmax) return(FALSE);

      smin = SIDE_Z_1;
      tmin = t;
    }
    else
    {
      /*
       * If intersection points are close to each other find out
       * which side to use, i.e. is most probably hit. [DB 9/94]
       */

      if (t >= tmin - CLOSE_TOLERANCE)
      {
        switch (smin)
        {
          case SIDE_X_0 :
          case SIDE_X_1 : if (-D[Z] > fabs(D[X])) smin = SIDE_Z_1; break;

          case SIDE_Y_0 :
          case SIDE_Y_1 : if (-D[Z] > fabs(D[Y])) smin = SIDE_Z_1; break;
        }
      }
    }
  }
  else
  {
    if (D[Z] > EPSILON)
    {
      t = (box->bounds[1][Z] - P[Z]) / D[Z];

      if (t < tmin) return(FALSE);

      if (t <= tmax - CLOSE_TOLERANCE)
      {
        smax = SIDE_Z_1;
        tmax = t;
      }
      else
      {
        /*
         * If intersection points are close to each other find out
         * which side to use, i.e. is most probably hit. [DB 9/94]
         */

        if (t <= tmax + CLOSE_TOLERANCE)
        {
          switch (smax)
          {
            case SIDE_X_0 :
            case SIDE_X_1 : if (D[Z] > fabs(D[X])) smax = SIDE_Z_1; break;

            case SIDE_Y_0 :
            case SIDE_Y_1 : if (D[Z] > fabs(D[Y])) smax = SIDE_Z_1; break;
          }
        }
      }

      t = (box->bounds[0][Z] - P[Z]) / D[Z];

      if (t >= tmin + CLOSE_TOLERANCE)
      {
        if (t > tmax) return(FALSE);

        smin = SIDE_Z_0;
        tmin = t;
      }
      else
      {
        /*
         * If intersection points are close to each other find out
         * which side to use, i.e. is most probably hit. [DB 9/94]
         */

        if (t >= tmin - CLOSE_TOLERANCE)
        {
          switch (smin)
          {
            case SIDE_X_0 :
            case SIDE_X_1 : if (D[Z] > fabs(D[X])) smin = SIDE_Z_0; break;

            case SIDE_Y_0 :
            case SIDE_Y_1 : if (D[Z] > fabs(D[Y])) smin = SIDE_Z_0; break;
          }
        }
      }
    }
    else
    {
      if ((P[Z] < box->bounds[0][Z]) || (P[Z] > box->bounds[1][Z]))
      {
        return(FALSE);
      }
    }
  }

  if (tmax < DEPTH_TOLERANCE)
  {
    return (FALSE);
  }

  *Depth1 = tmin;
  *Depth2 = tmax;

  *Side1 = smin;
  *Side2 = smax;

  return(TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Box
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

static int Inside_Box(VECTOR IPoint, OBJECT *Object)
{
  VECTOR New_Point;
  BOX *box = (BOX *) Object;

  /* Transform the point into box space. */

  if (box->Trans != NULL)
  {
    MInvTransPoint(New_Point, IPoint, box->Trans);
  }
  else
  {
    Assign_Vector(New_Point,IPoint);
  }

  /* Test to see if we are outside the box. */

  if ((New_Point[X] < box->bounds[0][X]) || (New_Point[X] > box->bounds[1][X]))
  {
    return (Test_Flag(box, INVERTED_FLAG));
  }

  if ((New_Point[Y] < box->bounds[0][Y]) || (New_Point[Y] > box->bounds[1][Y]))
  {
    return (Test_Flag(box, INVERTED_FLAG));
  }

  if ((New_Point[Z] < box->bounds[0][Z]) || (New_Point[Z] > box->bounds[1][Z]))
  {
    return (Test_Flag(box, INVERTED_FLAG));
  }

  /* Inside the box. */

  return (!Test_Flag(box, INVERTED_FLAG));
}



/*****************************************************************************
*
* FUNCTION
*
*   Box_Normal
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

static void Box_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  switch (Inter->i1)
  {
    case SIDE_X_0: Make_Vector(Result, -1.0,  0.0,  0.0); break;
    case SIDE_X_1: Make_Vector(Result,  1.0,  0.0,  0.0); break;
    case SIDE_Y_0: Make_Vector(Result,  0.0, -1.0,  0.0); break;
    case SIDE_Y_1: Make_Vector(Result,  0.0,  1.0,  0.0); break;
    case SIDE_Z_0: Make_Vector(Result,  0.0,  0.0, -1.0); break;
    case SIDE_Z_1: Make_Vector(Result,  0.0,  0.0,  1.0); break;

    default: Error("Unknown box side in Box_Normal().\n");
  }

  /* Transform the point into the boxes space. */

  if (((BOX *)Object)->Trans != NULL)
  {
    MTransNormal(Result, Result, ((BOX *)Object)->Trans);

    VNormalize(Result, Result);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Box
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

static void Translate_Box(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  if (((BOX *)Object)->Trans == NULL)
  {
    VAddEq(((BOX *)Object)->bounds[0], Vector);

    VAddEq(((BOX *)Object)->bounds[1], Vector);

    Compute_Box_BBox((BOX *)Object);
  }
  else
  {
    Transform_Box(Object, Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Box
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

static void Rotate_Box(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Box(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Box
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

static void Scale_Box(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  DBL temp;
  BOX *Box = (BOX *)Object;

  if (((BOX *)Object)->Trans == NULL)
  {
    VEvaluateEq(Box->bounds[0], Vector);
    VEvaluateEq(Box->bounds[1], Vector);

    if (Box->bounds[0][X] > Box->bounds[1][X])
    {
      temp = Box->bounds[0][X];

      Box->bounds[0][X] = Box->bounds[1][X];
      Box->bounds[1][X] = temp;
    }

    if (Box->bounds[0][Y] > Box->bounds[1][Y])
    {
      temp = Box->bounds[0][Y];

      Box->bounds[0][Y] = Box->bounds[1][Y];
      Box->bounds[1][Y] = temp;
    }

    if (Box->bounds[0][Z] > Box->bounds[1][Z])
    {
      temp = Box->bounds[0][Z];

      Box->bounds[0][Z] = Box->bounds[1][Z];
      Box->bounds[1][Z] = temp;
    }

    Compute_Box_BBox((BOX *)Object);
  }
  else
  {
    Transform_Box(Object, Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Box
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

static void Invert_Box(OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Box
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

static void Transform_Box(OBJECT *Object, TRANSFORM *Trans)
{
  BOX *box = (BOX *)Object;

  if (box->Trans == NULL)
  {
    box->Trans = Create_Transform();
  }

  Compose_Transforms(box->Trans, Trans);

  Compute_Box_BBox(box);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Box
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

BOX *Create_Box()
{
  BOX *New;

  New = (BOX *)POV_MALLOC(sizeof(BOX), "box");

  INIT_OBJECT_FIELDS(New, BOX_OBJECT, &Box_Methods)

  Make_Vector(New->bounds[0], -1.0, -1.0, -1.0);
  Make_Vector(New->bounds[1],  1.0,  1.0,  1.0);

  Make_BBox(New->BBox, -1.0, -1.0, -1.0, 2.0, 2.0, 2.0);

  New->Trans = NULL;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Box
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

BOX *Copy_Box(OBJECT *Object)
{
  BOX *New;

  New  = Create_Box();

  /* Copy box. */

  *New = *((BOX *)Object);

  New->Trans = Copy_Transform(((BOX *)Object)->Trans);

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Box
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

void Destroy_Box(OBJECT *Object)
{
  Destroy_Transform(((BOX *)Object)->Trans);

  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Box_BBox
*
* INPUT
*
*   Box - Box
*
* OUTPUT
*
*   Box
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a box.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Compute_Box_BBox(BOX *Box)
{
  Assign_BBox_Vect(Box->BBox.Lower_Left, Box->bounds[0]);

  VSub(Box->BBox.Lengths, Box->bounds[1], Box->bounds[0]);

  if (Box->Trans != NULL)
  {
    Recompute_BBox(&Box->BBox, Box->Trans);
  }
}

