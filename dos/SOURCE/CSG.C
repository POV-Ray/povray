/****************************************************************************
*                   csg.c
*
*  This module implements routines for constructive solid geometry.
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
#include "csg.h"
#include "hfield.h"
#include "matrices.h"
#include "objects.h"
#include "planes.h"
#include "quadrics.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/




/*****************************************************************************
* Static functions
******************************************************************************/

static int All_CSG_Union_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int All_CSG_Merge_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int All_CSG_Intersect_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int Inside_CSG_Union (VECTOR point, OBJECT *Object);
static int Inside_CSG_Intersection (VECTOR point, OBJECT *Object);
static CSG *Copy_CSG (OBJECT *Object);
static void Translate_CSG (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_CSG (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_CSG (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_CSG (OBJECT *Object, TRANSFORM *Trans);
static void Destroy_CSG (OBJECT *Object);
static void Invert_CSG_Union (OBJECT *Object);
static void Invert_CSG_Intersection (OBJECT *Object);


/*****************************************************************************
* Local variables
******************************************************************************/

METHODS CSG_Union_Methods =
{
  All_CSG_Union_Intersections,
  Inside_CSG_Union, NULL /*Normal*/,
  (COPY_METHOD)Copy_CSG,
  Translate_CSG, Rotate_CSG,
  Scale_CSG, Transform_CSG, Invert_CSG_Union, Destroy_CSG
};

METHODS CSG_Merge_Methods =
{
  All_CSG_Merge_Intersections,
  Inside_CSG_Union, NULL /*Normal*/,
  (COPY_METHOD)Copy_CSG,
  Translate_CSG, Rotate_CSG,
  Scale_CSG, Transform_CSG, Invert_CSG_Union, Destroy_CSG
};

METHODS CSG_Intersection_Methods =
{
  All_CSG_Intersect_Intersections,
  Inside_CSG_Intersection, NULL /*Normal*/,
  (COPY_METHOD)Copy_CSG,
  Translate_CSG, Rotate_CSG,
  Scale_CSG, Transform_CSG, Invert_CSG_Intersection, Destroy_CSG
};



/*****************************************************************************
*
* FUNCTION
*
*   All_CSG_Union_Intersections
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
*   Sep 1994 : Added code to count intersection tests. [DB]
*
******************************************************************************/

static int All_CSG_Union_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int Found;
  OBJECT *Current_Sib, *Clip;
  ISTACK *Local_Stack;
  INTERSECTION *Sibling_Intersection;

  Increase_Counter(stats[Ray_CSG_Union_Tests]);

  Found = FALSE;

  /* Use shortcut if no clip. */

  if ((Clip = Object->Clip) == NULL)
  {
    for (Current_Sib = ((CSG *)Object)->Children; Current_Sib != NULL; Current_Sib = Current_Sib->Sibling)
    {
      if (Ray_In_Bound (Ray, Current_Sib->Bound))
      {
        if (All_Intersections (Current_Sib, Ray, Depth_Stack))
        {
          Found = TRUE;
        }
      }
    }
  }
  else
  {
    Local_Stack = open_istack();

    for (Current_Sib = ((CSG *)Object)->Children; Current_Sib != NULL; Current_Sib = Current_Sib->Sibling)
    {
      if (Ray_In_Bound (Ray, Current_Sib->Bound))
      {
        if (All_Intersections (Current_Sib, Ray, Local_Stack))
        {
          while ((Sibling_Intersection = pop_entry(Local_Stack)) != NULL)
          {
            if (Point_In_Clip (Sibling_Intersection->IPoint, Clip))
            {
              push_copy (Depth_Stack, Sibling_Intersection);

              Found = TRUE;
            }
          }
        }
      }
    }

    close_istack (Local_Stack);
  }

  if (Found)
  {
    Increase_Counter(stats[Ray_CSG_Union_Tests_Succeeded]);
  }

  return (Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   All_CSG_Intersection_Intersections
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
*   Sep 1994 : Added code to count intersection tests. [DB]
*
******************************************************************************/

static int All_CSG_Intersect_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int Maybe_Found, Found;
  OBJECT *Current_Sib, *Inside_Sib;
  ISTACK *Local_Stack;
  INTERSECTION *Sibling_Intersection;

  Increase_Counter(stats[Ray_CSG_Intersection_Tests]);

  Local_Stack = open_istack ();

  Found = FALSE;

  for (Current_Sib = ((CSG *)Object)->Children; Current_Sib != NULL; Current_Sib = Current_Sib->Sibling)
  {
    if (Ray_In_Bound (Ray, Current_Sib->Bound))
    {
      if (All_Intersections (Current_Sib, Ray, Local_Stack))
      {
        while ((Sibling_Intersection = pop_entry(Local_Stack)) != NULL)
        {
          Maybe_Found = TRUE;

          for (Inside_Sib = ((CSG *)Object)->Children; Inside_Sib != NULL; Inside_Sib = Inside_Sib->Sibling)
          {
            if (Inside_Sib != Current_Sib)
            {
              if (!Inside_Object (Sibling_Intersection->IPoint, Inside_Sib))
              {
                Maybe_Found = FALSE;

                break;
              }
            }
          }

          if (Maybe_Found)
          {
            if (Point_In_Clip (Sibling_Intersection->IPoint, Object->Clip))
            {
              push_copy(Depth_Stack, Sibling_Intersection);

              Found = TRUE;
            }
          }
        }
      }
    }
  }

  close_istack (Local_Stack);

  if (Found)
  {
    Increase_Counter(stats[Ray_CSG_Intersection_Tests_Succeeded]);
  }

  return (Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   All_CSG_Merge_Intersections
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
*   Sep 1994 : Added code to count intersection tests. [DB]
*
******************************************************************************/

static int All_CSG_Merge_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int Found, inside_flag;
  OBJECT *Sib1, *Sib2;
  ISTACK *Local_Stack;
  INTERSECTION *Sibling_Intersection;

  Increase_Counter(stats[Ray_CSG_Merge_Tests]);

  Found = FALSE;

  Local_Stack = open_istack ();

  for (Sib1 = ((CSG *)Object)->Children; Sib1 != NULL; Sib1 = Sib1->Sibling)
  {
    if (Ray_In_Bound (Ray, Sib1->Bound))
    {
      if (All_Intersections (Sib1, Ray, Local_Stack))
      {
        while ((Sibling_Intersection = pop_entry (Local_Stack)) !=  NULL)
        {
          if (Point_In_Clip (Sibling_Intersection->IPoint, Object->Clip))
          {
            inside_flag = TRUE;

            for (Sib2 = ((CSG *)Object)->Children; Sib2 != NULL && inside_flag == TRUE; Sib2 = Sib2->Sibling)
            {
              if (Sib1 != Sib2)
              {
                if (Inside_Object(Sibling_Intersection->IPoint, Sib2))
                {
                  inside_flag = FALSE;
                }
              }
            }

            if (inside_flag == TRUE)
            {
              Found = TRUE;

              push_copy (Depth_Stack, Sibling_Intersection);
            }
          }
        }
      }
    }
  }

  close_istack (Local_Stack);

  if (Found)
  {
    Increase_Counter(stats[Ray_CSG_Merge_Tests_Succeeded]);
  }

  return (Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_CSG_Union
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

static int Inside_CSG_Union (VECTOR IPoint, OBJECT *Object)
{
  OBJECT *Current_Sib;

  for (Current_Sib = ((CSG *)Object)->Children; Current_Sib != NULL; Current_Sib = Current_Sib->Sibling)
  {
    if (Inside_Object (IPoint, Current_Sib))
    {
      return (TRUE);
    }
  }

  return (FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_CSG_Intersection
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

static int Inside_CSG_Intersection (VECTOR IPoint, OBJECT *Object)
{
  OBJECT *Current_Sib;

  for (Current_Sib = ((CSG *)Object)->Children; Current_Sib != NULL; Current_Sib = Current_Sib->Sibling)
  {
    if (!Inside_Object (IPoint, Current_Sib))
    {
      return (FALSE);
    }
  }

  return (TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_CSG
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

static void Translate_CSG (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  OBJECT *Sib;

  for (Sib = ((CSG *) Object)->Children; Sib != NULL; Sib = Sib->Sibling)
  {
    Translate_Object(Sib, Vector, Trans);
  }

  Recompute_BBox(&Object->BBox, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_CSG
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

static void Rotate_CSG (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  OBJECT *Sib;

  for (Sib = ((CSG *) Object)->Children; Sib != NULL; Sib = Sib->Sibling)
  {
    Rotate_Object(Sib, Vector, Trans);
  }

  Recompute_BBox(&Object->BBox, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_CSG
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

static void Scale_CSG (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  OBJECT *Sib;

  for (Sib = ((CSG *) Object)->Children; Sib != NULL; Sib = Sib->Sibling)
  {
    Scale_Object(Sib, Vector, Trans);
  }

  Recompute_BBox(&Object->BBox, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_CSG
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

static void Transform_CSG (OBJECT *Object, TRANSFORM *Trans)
{
  OBJECT *Sib;

  for (Sib = ((CSG *) Object)->Children; Sib != NULL; Sib = Sib->Sibling)
  {
    Transform_Object (Sib, Trans);
  }

  Recompute_BBox(&Object->BBox, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_CSG_Union
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

static void Invert_CSG_Union (OBJECT *Object)
{
  OBJECT *Sib;

  Object->Methods = &CSG_Intersection_Methods;

  for (Sib = ((CSG *)Object)->Children; Sib != NULL; Sib = Sib->Sibling)
  {
    Invert_Object (Sib);
  }

  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_CSG_Intersection
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

static void Invert_CSG_Intersection (OBJECT *Object)
{
  OBJECT *Sib;

  Object->Methods = &CSG_Merge_Methods;

  for (Sib = ((CSG *)Object)->Children; Sib != NULL; Sib = Sib->Sibling)
  {
    Invert_Object (Sib);
  }

  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_CSG_Union
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

CSG *Create_CSG_Union ()
{
  CSG *New;

  New = (CSG *)POV_MALLOC(sizeof (CSG), "union");

  INIT_OBJECT_FIELDS(New, UNION_OBJECT, &CSG_Union_Methods)

  New->Children = NULL;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_CSG_Merge
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

CSG *Create_CSG_Merge ()
{
  CSG *New;

  New = (CSG *)POV_MALLOC(sizeof (CSG), "merge");

  INIT_OBJECT_FIELDS(New, MERGE_OBJECT, &CSG_Merge_Methods)

  New->Children = NULL;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_CSG_Intersection
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

CSG *Create_CSG_Intersection ()
{
  CSG *New;

  New = (CSG *)POV_MALLOC(sizeof (CSG), "intersection");

  INIT_OBJECT_FIELDS(New, INTERSECTION_OBJECT, &CSG_Intersection_Methods)

  New->Children = NULL;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_CSG
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

static CSG *Copy_CSG (OBJECT *Object)
{
  CSG *New;
  OBJECT *Old_Sib, *New_Sib, *Prev_Sib;

  New = (CSG *)POV_MALLOC(sizeof (CSG), "csg");

  *New = *(CSG *)Object;

  New->Children = Prev_Sib = NULL;

  for (Old_Sib = ((CSG *)Object)->Children; Old_Sib != NULL; Old_Sib = Old_Sib->Sibling)
  {
    New_Sib = Copy_Object (Old_Sib);

    if (New->Children == NULL)
    {
      New->Children = New_Sib;
    }

    if (Prev_Sib != NULL)
    {
      Prev_Sib->Sibling = New_Sib;
    }

    Prev_Sib = New_Sib;
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_CSG
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

static void Destroy_CSG (OBJECT *Object)
{
  Destroy_Object (((CSG *) Object)->Children);

  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_CSG_BBox
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
*   Sep 1994 : Improved bounding of quadrics used in CSG intersections. [DB]
*
******************************************************************************/

void Compute_CSG_BBox (OBJECT *Object)
{
  int i, count;
  DBL Old_Volume, New_Volume;
  VECTOR NewMin, NewMax, TmpMin, TmpMax, Min, Max;
  OBJECT *Sib;
  QUADRIC **Quadrics;

  if (Object->Methods == &CSG_Intersection_Methods)
  {
    /*
     * Calculate the bounding box of a CSG intersection
     * by intersecting the bounding boxes of all children.
     */

    Make_Vector(NewMin, -BOUND_HUGE, -BOUND_HUGE, -BOUND_HUGE);
    Make_Vector(NewMax,  BOUND_HUGE,  BOUND_HUGE,  BOUND_HUGE);

    count = 0;

    Quadrics = NULL;

    /* Process all children. */

    for (Sib = ((CSG *)Object)->Children; Sib != NULL; Sib = Sib->Sibling)
    {
      /* Inverted objects and height fields mustn't be considered */

      if (!Test_Flag(Sib, INVERTED_FLAG) && (Sib->Methods != &HField_Methods))
      {
        /* Store quadrics since they'll be processed last. */

        if (Sib->Methods == &Quadric_Methods)
        {
          Quadrics = (QUADRIC **)POV_REALLOC(Quadrics, (count+1)*sizeof(QUADRIC *), "temporary quadric list");

          Quadrics[count++] = (QUADRIC *)Sib;
        }
        else
        {
          if (Sib->Methods == &Plane_Methods)
          {
            Compute_Plane_Min_Max((PLANE *)Sib, TmpMin, TmpMax);
          }
          else
          {
            Make_min_max_from_BBox(TmpMin, TmpMax, Sib->BBox);
          }

          NewMin[X] = max(NewMin[X], TmpMin[X]);
          NewMin[Y] = max(NewMin[Y], TmpMin[Y]);
          NewMin[Z] = max(NewMin[Z], TmpMin[Z]);
          NewMax[X] = min(NewMax[X], TmpMax[X]);
          NewMax[Y] = min(NewMax[Y], TmpMax[Y]);
          NewMax[Z] = min(NewMax[Z], TmpMax[Z]);
        }
      }
    }

    /* Process any quadrics. */

    for (i = 0; i < count; i++)
    {
      Assign_Vector(Min, NewMin);
      Assign_Vector(Max, NewMax);

      Compute_Quadric_BBox(Quadrics[i], Min, Max);

      Make_min_max_from_BBox(TmpMin, TmpMax, Quadrics[i]->BBox);

      NewMin[X] = max(NewMin[X], TmpMin[X]);
      NewMin[Y] = max(NewMin[Y], TmpMin[Y]);
      NewMin[Z] = max(NewMin[Z], TmpMin[Z]);
      NewMax[X] = min(NewMax[X], TmpMax[X]);
      NewMax[Y] = min(NewMax[Y], TmpMax[Y]);
      NewMax[Z] = min(NewMax[Z], TmpMax[Z]);
    }

    if (Quadrics != NULL)
    {
      POV_FREE(Quadrics);
    }
  }
  else
  {
    /* Calculate the bounding box of a CSG merge/union object. */

    Make_Vector(NewMin,  BOUND_HUGE,  BOUND_HUGE,  BOUND_HUGE);
    Make_Vector(NewMax, -BOUND_HUGE, -BOUND_HUGE, -BOUND_HUGE);

    for (Sib = ((CSG *)Object)->Children; Sib != NULL; Sib = Sib->Sibling)
    {
      Make_min_max_from_BBox(TmpMin, TmpMax, Sib->BBox);

      NewMin[X] = min(NewMin[X], TmpMin[X]);
      NewMin[Y] = min(NewMin[Y], TmpMin[Y]);
      NewMin[Z] = min(NewMin[Z], TmpMin[Z]);
      NewMax[X] = max(NewMax[X], TmpMax[X]);
      NewMax[Y] = max(NewMax[Y], TmpMax[Y]);
      NewMax[Z] = max(NewMax[Z], TmpMax[Z]);
    }
  }

  if ((NewMin[X] > NewMax[X]) || (NewMin[Y] > NewMax[Y]) || (NewMin[Z] > NewMax[Z]))
  {
    Warning(0.0, "Degenerate CSG bounding box (not used!).\n");
  }
  else
  {
    New_Volume = (NewMax[X] - NewMin[X]) * (NewMax[Y] - NewMin[Y]) * (NewMax[Z] - NewMin[Z]);

    BOUNDS_VOLUME(Old_Volume, Object->BBox);

    if (New_Volume < Old_Volume)
    {
      Make_BBox_from_min_max(Object->BBox, NewMin, NewMax);

      /* Beware of bounding boxes to large. */

      if ((Object->BBox.Lengths[X] > CRITICAL_LENGTH) ||
          (Object->BBox.Lengths[Y] > CRITICAL_LENGTH) ||
          (Object->BBox.Lengths[Z] > CRITICAL_LENGTH))
      {
        Make_BBox(Object->BBox, -BOUND_HUGE/2, -BOUND_HUGE/2, -BOUND_HUGE/2,
          BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
      }
    }
  }
}
