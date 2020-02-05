/****************************************************************************
*                spheres.c
*
*  This module implements the sphere primitive.
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



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define DEPTH_TOLERANCE 1.0e-6



/*****************************************************************************
* Static functions
******************************************************************************/
static int All_Sphere_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int All_Ellipsoid_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int Inside_Sphere (VECTOR IPoint, OBJECT *Object);
static int Inside_Ellipsoid (VECTOR IPoint, OBJECT *Object);
static void Sphere_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static void Ellipsoid_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static void Translate_Sphere (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Sphere (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Sphere (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Invert_Sphere (OBJECT *Object);



/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Sphere_Methods =
{
  All_Sphere_Intersections,
  Inside_Sphere, Sphere_Normal,
  (COPY_METHOD)Copy_Sphere,
  Translate_Sphere, Rotate_Sphere,
  Scale_Sphere, Transform_Sphere, Invert_Sphere,
  Destroy_Sphere
};



static METHODS Ellipsoid_Methods =
{
  All_Ellipsoid_Intersections,
  Inside_Ellipsoid, Ellipsoid_Normal,
  (COPY_METHOD)Copy_Sphere,
  Translate_Sphere, Rotate_Sphere,
  Scale_Sphere, Transform_Sphere, Invert_Sphere,
  Destroy_Sphere
};



/*****************************************************************************
*
* FUNCTION
*
*   All_Sphere_Intersection
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

static int All_Sphere_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  register int Intersection_Found;
  DBL Depth1, Depth2;
  VECTOR IPoint;
  SPHERE *Sphere = (SPHERE *)Object;

  Intersection_Found = FALSE;

  if (Intersect_Sphere(Ray, Sphere->Center, Sqr(Sphere->Radius), &Depth1, &Depth2))
  {
    if ((Depth1 > DEPTH_TOLERANCE) && (Depth1 < Max_Distance))
    {
      VEvaluateRay(IPoint, Ray->Initial, Depth1, Ray->Direction);

      if (Point_In_Clip(IPoint, Object->Clip))
      {
        push_entry(Depth1, IPoint, Object, Depth_Stack);

        Intersection_Found = TRUE;
      }
    }

    if ((Depth2 > DEPTH_TOLERANCE) && (Depth2 < Max_Distance))
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
*   All_Ellipsoid_Intersection
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

static int All_Ellipsoid_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  register int Intersection_Found;
  DBL Depth1, Depth2, len;
  VECTOR IPoint;
  RAY New_Ray;
  SPHERE *Sphere = (SPHERE *)Object;

  /* Transform the ray into the ellipsoid's space */

  MInvTransPoint(New_Ray.Initial, Ray->Initial, ((SPHERE *)Object)->Trans);
  MInvTransDirection(New_Ray.Direction, Ray->Direction, ((SPHERE *)Object)->Trans);

  VLength(len, New_Ray.Direction);
  VInverseScaleEq(New_Ray.Direction, len);

  Intersection_Found = FALSE;

  if (Intersect_Sphere(&New_Ray, Sphere->Center, Sqr(Sphere->Radius), &Depth1, &Depth2))
  {
    if ((Depth1 > DEPTH_TOLERANCE) && (Depth1 < Max_Distance))
    {
      VEvaluateRay(IPoint, New_Ray.Initial, Depth1, New_Ray.Direction);

      MTransPoint(IPoint, IPoint, ((SPHERE *)Object)->Trans);

      if (Point_In_Clip(IPoint, Object->Clip))
      {
        push_entry(Depth1 / len, IPoint, Object, Depth_Stack);

        Intersection_Found = TRUE;
      }
    }

    if ((Depth2 > DEPTH_TOLERANCE) && (Depth2 < Max_Distance))
    {
      VEvaluateRay(IPoint, New_Ray.Initial, Depth2, New_Ray.Direction);

      MTransPoint(IPoint, IPoint, ((SPHERE *)Object)->Trans);

      if (Point_In_Clip(IPoint, Object->Clip))
      {
        push_entry(Depth2 / len, IPoint, Object, Depth_Stack);

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
*   Intersect_Sphere
*
* INPUT
*
*   Ray     - Ray to test intersection with
*   Center  - Center of the sphere
*   Radius2 - Squared radius of the sphere
*   Depth1  - Lower intersection distance
*   Depth2  - Upper intersection distance
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

int Intersect_Sphere(RAY *Ray, VECTOR Center, DBL Radius2, DBL *Depth1, DBL  *Depth2)
{
  DBL OCSquared, t_Closest_Approach, Half_Chord, t_Half_Chord_Squared;
  VECTOR Origin_To_Center;

  Increase_Counter(stats[Ray_Sphere_Tests]);

  VSub(Origin_To_Center, Center, Ray->Initial);

  VDot(OCSquared, Origin_To_Center, Origin_To_Center);

  VDot(t_Closest_Approach, Origin_To_Center, Ray->Direction);

  if ((OCSquared >= Radius2) && (t_Closest_Approach < EPSILON))
  {
    return(FALSE);
  }

  t_Half_Chord_Squared = Radius2 - OCSquared + Sqr(t_Closest_Approach);

  if (t_Half_Chord_Squared > EPSILON)
  {
    Half_Chord = sqrt(t_Half_Chord_Squared);

    *Depth1 = t_Closest_Approach - Half_Chord;
    *Depth2 = t_Closest_Approach + Half_Chord;

    Increase_Counter(stats[Ray_Sphere_Tests_Succeeded]);

    return(TRUE);
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Sphere
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

static int Inside_Sphere(VECTOR IPoint, OBJECT *Object)
{
  DBL OCSquared;
  VECTOR Origin_To_Center;

  VSub(Origin_To_Center, ((SPHERE *)Object)->Center, IPoint);

  VDot(OCSquared, Origin_To_Center, Origin_To_Center);

  if (Test_Flag(Object, INVERTED_FLAG))
  {
    return(OCSquared > Sqr(((SPHERE *)Object)->Radius));
  }
  else
  {
    return(OCSquared < Sqr(((SPHERE *)Object)->Radius));
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Ellipsoid
*
* INPUT
*
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

static int Inside_Ellipsoid(VECTOR IPoint, OBJECT *Object)
{
  DBL OCSquared;
  VECTOR Origin_To_Center, New_Point;

  /* Transform the point into the sphere's space */

  MInvTransPoint(New_Point, IPoint, ((SPHERE *)Object)->Trans);

  VSub(Origin_To_Center, ((SPHERE *)Object)->Center, New_Point);

  VDot(OCSquared, Origin_To_Center, Origin_To_Center);

  if (Test_Flag(Object, INVERTED_FLAG))
  {
    return(OCSquared > Sqr(((SPHERE *)Object)->Radius));
  }
  else
  {
    return(OCSquared < Sqr(((SPHERE *)Object)->Radius));
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Sphere_Normal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

static void Sphere_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  VSub(Result, Inter->IPoint, ((SPHERE *)Object)->Center);

  VInverseScaleEq(Result, ((SPHERE *)Object)->Radius);
}



/*****************************************************************************
*
* FUNCTION
*
*   Ellipsoid_Normal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

static void Ellipsoid_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  VECTOR New_Point;

  /* Transform the point into the sphere's space */

  MInvTransPoint(New_Point, Inter->IPoint, ((SPHERE *)Object)->Trans);

  VSub(Result, New_Point, ((SPHERE *)Object)->Center);

  MTransNormal(Result, Result, ((SPHERE *)Object)->Trans);

  VNormalize(Result, Result);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Shere
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

SPHERE *Copy_Sphere(OBJECT *Object)
{
  SPHERE *New;

  New = Create_Sphere();

  *New = *((SPHERE *)Object);

  New->Trans = Copy_Transform(((SPHERE *)Object)->Trans);

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Sphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   ?
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

static void Translate_Sphere(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  SPHERE *Sphere = (SPHERE *) Object;

  if (Sphere->Trans == NULL)
  {
    VAddEq(Sphere->Center, Vector);

    Compute_Sphere_BBox(Sphere);
  }
  else
  {
    Transform_Sphere(Object, Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Sphere
*
* INPUT
*
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

static void Rotate_Sphere(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  SPHERE *Sphere = (SPHERE *) Object;

  if (Sphere->Trans == NULL)
  {
    MTransPoint(Sphere->Center, Sphere->Center, Trans);

    Compute_Sphere_BBox(Sphere);
  }
  else
  {
    Transform_Sphere(Object, Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Sphere
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

static void Scale_Sphere(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  SPHERE *Sphere = (SPHERE *) Object;

  if ((Vector[X] != Vector[Y]) || (Vector[X] != Vector[Z]))
  {
    if (Sphere->Trans == NULL)
    {
      Sphere->Methods = &Ellipsoid_Methods;

      Sphere->Trans = Create_Transform();
    }
  }

  if (Sphere->Trans == NULL)
  {
    VScaleEq(Sphere->Center, Vector[X]);

    Sphere->Radius *= Vector[X];

    Compute_Sphere_BBox(Sphere);
  }
  else
  {
    Transform_Sphere(Object, Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Sphere
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

static void Invert_Sphere(OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Sphere
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

SPHERE *Create_Sphere()
{
  SPHERE *New;

  New = (SPHERE *)POV_MALLOC(sizeof(SPHERE), "sphere");

  INIT_OBJECT_FIELDS(New, SPHERE_OBJECT, &Sphere_Methods)

  Make_Vector(New->Center, 0.0, 0.0, 0.0);

  New->Radius = 1.0;

  New->Trans = NULL;

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Sphere
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

void Transform_Sphere(OBJECT *Object, TRANSFORM *Trans)
{
  SPHERE *Sphere = (SPHERE *)Object;

  if (Sphere->Trans == NULL)
  {
    Sphere->Methods = &Ellipsoid_Methods;

    Sphere->Trans = Create_Transform();
  }

  Compose_Transforms(Sphere->Trans, Trans);

  Compute_Sphere_BBox(Sphere);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Sphere
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   ?
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

void Destroy_Sphere(OBJECT *Object)
{
  Destroy_Transform(((SPHERE *)Object)->Trans);

  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Sphere_BBox
*
* INPUT
*
*   Sphere - Sphere
*   
* OUTPUT
*
*   Sphere
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a sphere.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Compute_Sphere_BBox(SPHERE *Sphere)
{
  Make_BBox(Sphere->BBox, Sphere->Center[X] - Sphere->Radius, Sphere->Center[Y] - Sphere->Radius,  Sphere->Center[Z] - Sphere->Radius,
    2.0 * Sphere->Radius, 2.0 * Sphere->Radius, 2.0 * Sphere->Radius);

  if (Sphere->Trans != NULL)
  {
    Recompute_BBox(&Sphere->BBox, Sphere->Trans);
  }
}
