/****************************************************************************
*                discs.c
*
*  This module implements the disc primitive.
*  This file was written by Alexander Enzmann.  He wrote the code for
*  discs and generously provided us these enhancements.
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
#include "discs.h"
#include "matrices.h"
#include "objects.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int Intersect_Disc (RAY *Ray, DISC *Disc, DBL *Depth);
static int All_Disc_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int Inside_Disc (VECTOR point, OBJECT *Object);
static void Disc_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static DISC *Copy_Disc (OBJECT *Object);
static void Translate_Disc (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Disc (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Disc (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Disc (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Disc (OBJECT *Object);
static void Destroy_Disc (OBJECT *Object);
static void Compute_Disc_BBox (DISC *Disc);

/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Disc_Methods =
{
  All_Disc_Intersections,
  Inside_Disc, Disc_Normal,
  (COPY_METHOD)Copy_Disc, Translate_Disc, Rotate_Disc, Scale_Disc, Transform_Disc,
  Invert_Disc, Destroy_Disc
};


/*****************************************************************************
*
* FUNCTION
*
*   All_Disc_Intersections
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

static int All_Disc_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int Intersection_Found;
  DBL Depth;
  VECTOR IPoint;

  Intersection_Found = FALSE;

  if (Intersect_Disc (Ray, (DISC *)Object, &Depth))
  {
    VEvaluateRay(IPoint, Ray->Initial, Depth, Ray->Direction);

    if (Point_In_Clip (IPoint, Object->Clip))
    {
      push_entry(Depth,IPoint,Object,Depth_Stack);
      Intersection_Found = TRUE;
    }
  }

  return (Intersection_Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Disc
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

static int Intersect_Disc (RAY *Ray, DISC *disc, DBL *Depth)
{
  DBL t, u, v, r2, len;
  VECTOR P, D;

  Increase_Counter(stats[Ray_Disc_Tests]);

  /* Transform the point into the discs space */

  MInvTransPoint(P, Ray->Initial, disc->Trans);
  MInvTransDirection(D, Ray->Direction, disc->Trans);

  VLength(len, D);
  VInverseScaleEq(D, len);

  if (fabs(D[Z]) > EPSILON)
  {
    t = -P[Z] / D[Z];

    if (t >= 0.0)
    {
      u = P[X] + t * D[X];
      v = P[Y] + t * D[Y];

      r2 = Sqr(u) + Sqr(v);

      if ((r2 >= disc->iradius2) && (r2 <= disc->oradius2))
      {
        *Depth = t / len;

        if ((*Depth > Small_Tolerance) && (*Depth < Max_Distance))
        {
          Increase_Counter(stats[Ray_Disc_Tests_Succeeded]);

          return (TRUE);
        }
      }
    }
  }

  return (FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Disc
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

static int Inside_Disc (VECTOR IPoint, OBJECT *Object)
{
  VECTOR New_Point;
  DISC *disc = (DISC *) Object;

  /* Transform the point into the discs space */

  MInvTransPoint(New_Point, IPoint, disc->Trans);

  if (New_Point[Z] >= 0.0)
  {
    /* We are outside. */

    return (Test_Flag(disc, INVERTED_FLAG));
  }
  else
  {
    /* We are inside. */

    return (!Test_Flag(disc, INVERTED_FLAG));
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Disc_Normal
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

static void Disc_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  Assign_Vector(Result, ((DISC *)Object)->normal);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Disc
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

static void Translate_Disc (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Disc(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Disc
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

static void Rotate_Disc(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Disc(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Disc
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

static void Scale_Disc(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Disc(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Disc
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

static void Invert_Disc (OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Disc
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

static void Transform_Disc (OBJECT *Object, TRANSFORM *Trans)
{
  DISC *Disc = (DISC *)Object;

  MTransNormal(((DISC *)Object)->normal, ((DISC *)Object)->normal, Trans);

  VNormalize(((DISC *)Object)->normal, ((DISC *)Object)->normal);

  Compose_Transforms(Disc->Trans, Trans);

  /* Recalculate the bounds */

  Compute_Disc_BBox(Disc);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Disc
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

DISC *Create_Disc ()
{
  DISC *New;

  New = (DISC *)POV_MALLOC(sizeof (DISC), "disc");

  INIT_OBJECT_FIELDS(New, DISC_OBJECT, &Disc_Methods)

  Make_Vector (New->center, 0.0, 0.0, 0.0);
  Make_Vector (New->normal, 0.0, 0.0, 1.0);

  New->iradius2 = 0.0;
  New->oradius2 = 1.0;

  New->d = 0.0;

  New->Trans = Create_Transform();

  /* Default bounds */

  Make_BBox(New->BBox, -1.0, -1.0, -Small_Tolerance, 2.0,  2.0, 2.0 * Small_Tolerance);

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Disc
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
*   Sep 1994 : Fixed memory leakage [DB]
*
******************************************************************************/

static DISC *Copy_Disc (OBJECT *Object)
{
  DISC *New;

  New  = Create_Disc();

  /* Get rid of the transformation created in Create_Disc(). */

  Destroy_Transform(New->Trans);

  /* Copy disc. */

  *New = *((DISC *) Object);

  New->Trans = Copy_Transform(((DISC *)Object)->Trans);

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Disc
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

static void Destroy_Disc (OBJECT *Object)
{
  Destroy_Transform(((DISC *)Object)->Trans);

  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Disc
*
* INPUT
*
*   Disc - Disc
*
* OUTPUT
*
*   Disc
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the transformation that scales, rotates, and translates
*   the disc to the desired location and orientation.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Compute_Disc(DISC *Disc)
{
  Compute_Coordinate_Transform(Disc->Trans, Disc->center, Disc->normal, 1.0, 1.0);

  Compute_Disc_BBox(Disc);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Disc_BBox
*
* INPUT
*
*   Disc - Disc
*
* OUTPUT
*
*   Disc
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a disc.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

static void Compute_Disc_BBox(DISC *Disc)
{
  DBL rad;

  rad = sqrt(Disc->oradius2);

  Make_BBox(Disc->BBox, -rad, -rad, -Small_Tolerance, 2.0*rad, 2.0*rad, 2.0*Small_Tolerance);

  Recompute_BBox(&Disc->BBox, Disc->Trans);
}

