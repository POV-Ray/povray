/****************************************************************************
*                cones.c
*
*  This module implements the cone primitive.
*  This file was written by Alexander Enzmann.    He wrote the code for
*  cones and generously provided us these enhancements.
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
#include "cones.h"
#include "matrices.h"
#include "objects.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define Cone_Tolerance 1.0e-6

#define close(x, y) (fabs(x-y) < EPSILON ? 1 : 0)

/* Part of the cone/cylinder hit. [DB 9/94] */

#define BASE_HIT 1
#define CAP_HIT  2
#define SIDE_HIT 3



/*****************************************************************************
* Local typedefs
******************************************************************************/

typedef struct Cone_Intersection_Structure CONE_INT;

struct Cone_Intersection_Structure
{
  DBL d;  /* Distance of intersection point               */
  int t;  /* Type of intersection: base/cap plane or side */
};



/*****************************************************************************
* Static functions
******************************************************************************/

static int intersect_cone (RAY *Ray, CONE *Cone, CONE_INT *Depths);
static void Destroy_Cone (OBJECT *Object);
static int All_Cone_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int Inside_Cone (VECTOR point, OBJECT *Object);
static void Cone_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static CONE *Copy_Cone (OBJECT *Object);
static void Translate_Cone (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Cone (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Cone (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Cone (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Cone (OBJECT *Object);


/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Cone_Methods =
{
  All_Cone_Intersections,
  Inside_Cone, Cone_Normal,
  (COPY_METHOD)Copy_Cone, Translate_Cone, Rotate_Cone, Scale_Cone, Transform_Cone,
  Invert_Cone, Destroy_Cone
};



/*****************************************************************************
*
* FUNCTION
*
*   All_Cone_Intersections
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

static int All_Cone_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int Intersection_Found, cnt, i;
  VECTOR IPoint;
  CONE_INT I[4];

  Intersection_Found = FALSE;

  if ((cnt = intersect_cone(Ray, (CONE *)Object, I)) != 0)
  {
    for (i = 0; i < cnt; i++)
    {
      VEvaluateRay(IPoint, Ray->Initial, I[i].d, Ray->Direction);

      if (Point_In_Clip(IPoint, Object->Clip))
      {
        push_entry_i1(I[i].d,IPoint,Object,I[i].t,Depth_Stack);

        Intersection_Found = TRUE;
      }
    }
  }

  return (Intersection_Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_cone
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

static int intersect_cone(RAY *Ray, CONE *Cone, CONE_INT *Intersection)
{
  int i = 0;
  DBL a, b, c, z, t1, t2, len;
  DBL d;
  VECTOR P, D;

  Increase_Counter(stats[Ray_Cone_Tests]);

  /* Transform the ray into the cones space */

  MInvTransPoint(P, Ray->Initial, Cone->Trans);
  MInvTransDirection(D, Ray->Direction, Cone->Trans);

  VLength(len, D);
  VInverseScaleEq(D, len);

  if (Test_Flag(Cone, CYLINDER_FLAG))
  {
    /* Solve intersections with a cylinder */

    a = D[X] * D[X] + D[Y] * D[Y];

    if (a > EPSILON)
    {
      b = P[X] * D[X] + P[Y] * D[Y];

      c = P[X] * P[X] + P[Y] * P[Y] - 1.0;

      d = b * b - a * c;

      if (d >= 0.0)
      {
        d = sqrt(d);

        t1 = (-b + d) / a;
        t2 = (-b - d) / a;

        z = P[Z] + t1 * D[Z];

        if ((t1 > Cone_Tolerance) && (t1 < Max_Distance) && (z >= 0.0) && (z <= 1.0))
        {
          Intersection[i].d   = t1 / len;
          Intersection[i++].t = SIDE_HIT;
        }

        z = P[Z] + t2 * D[Z];

        if ((t2 > Cone_Tolerance) && (t1 < Max_Distance) && (z >= 0.0) && (z <= 1.0))
        {
          Intersection[i].d   = t2 / len;
          Intersection[i++].t = SIDE_HIT;
        }
      }
    }
  }
  else
  {
    /* Solve intersections with a cone */

    a = D[X] * D[X] + D[Y] * D[Y] - D[Z] * D[Z];

    b = D[X] * P[X] + D[Y] * P[Y] - D[Z] * P[Z];

    c = P[X] * P[X] + P[Y] * P[Y] - P[Z] * P[Z];

    if (fabs(a) < EPSILON)
    {
      if (fabs(b) > EPSILON)
      {
        /* One intersection */

        t1 = -0.5 * c / b;

        z = P[Z] + t1 * D[Z];

        if ((t1 > Cone_Tolerance) && (t1 < Max_Distance) && (z >= Cone->dist) && (z <= 1.0))
        {
          Intersection[i].d   = t1 / len;
          Intersection[i++].t = SIDE_HIT;
        }
      }
    }
    else
    {
      /* Check hits against the side of the cone */

      d = b * b - a * c;

      if (d >= 0.0)
      {
        d = sqrt(d);

        t1 = (-b - d) / a;
        t2 = (-b + d) / a;

        z = P[Z] + t1 * D[Z];

        if ((t1 > Cone_Tolerance) && (t1 < Max_Distance) && (z >= Cone->dist) && (z <= 1.0))
        {
          Intersection[i].d   = t1 / len;
          Intersection[i++].t = SIDE_HIT;
        }

        z = P[Z] + t2 * D[Z];

        if ((t2 > Cone_Tolerance) && (t1 < Max_Distance) && (z >= Cone->dist) && (z <= 1.0))
        {
          Intersection[i].d   = t2 / len;
          Intersection[i++].t = SIDE_HIT;
        }
      }
    }
  }

  if (Test_Flag(Cone, CLOSED_FLAG) && (fabs(D[Z]) > EPSILON))
  {
    d = (1.0 - P[Z]) / D[Z];

    a = (P[X] + d * D[X]);

    b = (P[Y] + d * D[Y]);

    if (((Sqr(a) + Sqr(b)) <= 1.0) && (d > Cone_Tolerance) && (d < Max_Distance))
    {
      Intersection[i].d   = d / len;
      Intersection[i++].t = CAP_HIT;
    }

    d = (Cone->dist - P[Z]) / D[Z];

    a = (P[X] + d * D[X]);

    b = (P[Y] + d * D[Y]);

    if ((Sqr(a) + Sqr(b)) <= (Test_Flag(Cone, CYLINDER_FLAG) ? 1.0 : Sqr(Cone->dist))
      && (d > Cone_Tolerance) && (d < Max_Distance))
    {
      Intersection[i].d   = d / len;
      Intersection[i++].t = BASE_HIT;
    }
  }

  if (i)
  {
    Increase_Counter(stats[Ray_Cone_Tests_Succeeded]);
  }

  return (i);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Cone
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

static int Inside_Cone(VECTOR IPoint, OBJECT *Object)
{
  CONE *Cone = (CONE *)Object;
  DBL w2, z2, offset = (Test_Flag(Cone, CLOSED_FLAG) ? -EPSILON : EPSILON);
  VECTOR New_Point;

  /* Transform the point into the cones space */

  MInvTransPoint(New_Point, IPoint, Cone->Trans);

  /* Test to see if we are inside the cone */

  w2 = New_Point[X] * New_Point[X] + New_Point[Y] * New_Point[Y];

  if (Test_Flag(Cone, CYLINDER_FLAG))
  {
    /* Check to see if we are inside a cylinder */

    if ((w2 > 1.0 + offset) ||
        (New_Point[Z] < 0.0 - offset) ||
        (New_Point[Z] > 1.0 + offset))
    {
      return (Test_Flag(Cone, INVERTED_FLAG));
    }
    else
    {
      return (!Test_Flag(Cone, INVERTED_FLAG));
    }
  }
  else
  {
    /* Check to see if we are inside a cone */

    z2 = New_Point[Z] * New_Point[Z];

    if ((w2 > z2 + offset) ||
        (New_Point[Z] < Cone->dist - offset) ||
        (New_Point[Z] > 1.0+offset))
    {
      return (Test_Flag(Cone, INVERTED_FLAG));
    }
    else
    {
      return (!Test_Flag(Cone, INVERTED_FLAG));
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Cone_Normal
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

static void Cone_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  CONE *Cone = (CONE *)Object;

  /* Transform the point into the cones space */

  MInvTransPoint(Result, Inter->IPoint, Cone->Trans);

  /* Calculating the normal is real simple in canonical cone space */

  switch (Inter->i1)
  {
    case SIDE_HIT:

      if (Test_Flag(Cone, CYLINDER_FLAG))
      {
        Result[Z] = 0.0;
      }
      else
      {
        Result[Z] = -Result[Z];
      }

      break;

    case BASE_HIT:

      Make_Vector(Result, 0.0, 0.0, -1.0)

      break;

    case CAP_HIT:

      Make_Vector(Result, 0.0, 0.0, 1.0)

      break;
  }

  /* Transform the point out of the cones space */

  MTransNormal(Result, Result, Cone->Trans);

  VNormalize(Result, Result);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Cone
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

static void Translate_Cone(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Cone(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Cone
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

static void Rotate_Cone(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Cone(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Cone
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

static void Scale_Cone(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Cone(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Cone
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

static void Transform_Cone(OBJECT *Object, TRANSFORM *Trans)
{
  CONE *Cone = (CONE *)Object;

  Compose_Transforms(Cone->Trans, Trans);

  Compute_Cone_BBox(Cone);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Cone
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

static void Invert_Cone(OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Cone
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

CONE *Create_Cone()
{
  CONE *New;

  New = (CONE *)POV_MALLOC(sizeof(CONE), "cone");

  INIT_OBJECT_FIELDS(New, CONE_OBJECT, &Cone_Methods)

  Make_Vector(New->apex, 0.0, 0.0, 1.0);
  Make_Vector(New->base, 0.0, 0.0, 0.0);

  New->apex_radius = 1.0;
  New->base_radius = 0.0;

  New->dist = 0.0;

  New->Trans = Create_Transform();

  /* Cone/Cylinder has capped ends by default. */

  Set_Flag(New, CLOSED_FLAG);

  /* Default bounds */

  Make_BBox(New->BBox, -1.0, -1.0, 0.0, 2.0, 2.0, 1.0);

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Cone
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

static CONE *Copy_Cone(OBJECT *Object)
{
  CONE *New;

  New = Create_Cone();

  /* Get rid of the transformation created in Create_Cone(). */

  Destroy_Transform(New->Trans);

  /* Copy cone. */

  *New = *((CONE *)Object);

  New->Trans = Copy_Transform(((CONE *)Object)->Trans);

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Cylinder
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

CONE *Create_Cylinder()
{
  CONE *New;

  New = (CONE *)POV_MALLOC(sizeof(CONE), "cone");

  INIT_OBJECT_FIELDS(New, CONE_OBJECT, &Cone_Methods)

  Make_Vector(New->apex, 0.0, 0.0, 1.0);
  Make_Vector(New->base, 0.0, 0.0, 0.0);

  New->apex_radius = 1.0;
  New->base_radius = 1.0;
  New->dist        = 0.0;

  New->Trans = Create_Transform();

  Set_Flag(New, CYLINDER_FLAG); /* This is a cylinder. */
  Set_Flag(New, CLOSED_FLAG);   /* Has capped ends.    */

  /* Default bounds */

  Make_BBox(New->BBox, -1.0, -1.0, 0.0, 2.0, 2.0, 1.0);

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Cone_Data
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
*   Feb 1996: check for equal sized ends (cylinder) first [AED]
*
******************************************************************************/

void Compute_Cone_Data(OBJECT *Object)
{
  DBL tlen, len, tmpf;
  VECTOR tmpv, axis, origin;
  CONE *Cone = (CONE *)Object;

  /* Process the primitive specific information */

  if (fabs(Cone->apex_radius - Cone->base_radius) < EPSILON)
  {
    /* What we are dealing with here is really a cylinder */

    Set_Flag(Cone, CYLINDER_FLAG);

    Compute_Cylinder_Data(Object);

    return;
  }

  if (Cone->apex_radius < Cone->base_radius)
  {
    /* Want the bigger end at the top */

    Assign_Vector(tmpv,Cone->base);
    Assign_Vector(Cone->base,Cone->apex);
    Assign_Vector(Cone->apex,tmpv);

    tmpf = Cone->base_radius;
    Cone->base_radius = Cone->apex_radius;
    Cone->apex_radius = tmpf;
  }

  /* Find the axis and axis length */

  VSub(axis, Cone->apex, Cone->base);

  VLength(len, axis);

  if (len < EPSILON)
  {
    Error("Degenerate cone/cylinder.\n");
  }
  else
  {
    VInverseScaleEq(axis, len)
  }

  /* Determine alignment */

  tmpf = Cone->base_radius * len / (Cone->apex_radius - Cone->base_radius);

  VScale(origin, axis, tmpf);

  VSub(origin, Cone->base, origin);

  tlen = tmpf + len;

  Cone->dist = tmpf / tlen;

  Compute_Coordinate_Transform(Cone->Trans, origin, axis, Cone->apex_radius, tlen);

  /* Recalculate the bounds */

  Compute_Cone_BBox(Cone);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Cylinder_Data
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

void Compute_Cylinder_Data(OBJECT *Object)
{
  DBL tmpf;
  VECTOR axis;
  CONE *Cone = (CONE *)Object;

  VSub(axis, Cone->apex, Cone->base);

  VLength(tmpf, axis);

  if (tmpf < EPSILON)
  {
    Error("Degenerate cylinder, base point = apex point.\n");
  }
  else
  {
    VInverseScaleEq(axis, tmpf)

    Compute_Coordinate_Transform(Cone->Trans, Cone->base, axis, Cone->apex_radius, tmpf);
  }

  Cone->dist = 0.0;

  /* Recalculate the bounds */

  Compute_Cone_BBox(Cone);
}




/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Cone
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

static void Destroy_Cone(OBJECT *Object)
{
  Destroy_Transform(((CONE *)Object)->Trans);

  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Cone_BBox
*
* INPUT
*
*   Cone - Cone/Cylinder
*
* OUTPUT
*
*   Cone
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a cone or cylinder.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Compute_Cone_BBox(CONE *Cone)
{
  Make_BBox(Cone->BBox, -1.0, -1.0, 0.0, 2.0, 2.0, 1.0);

  Recompute_BBox(&Cone->BBox, Cone->Trans);
}

