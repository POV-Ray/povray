/****************************************************************************
*                   blob.c
*
*  This module implements functions that manipulate blobs.
*
*  The original file was written by Alexander Enzmann.
*  He wrote the code for blobs and generously provided us these enhancements.
*
*  Modifications and enhancements by Dieter Bayer [DB].
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
*    -
*
*  Syntax:
*
*    blob
*    {
*      threshold THRESHOLD_VALUE
*
*      component STRENGTH, RADIUS, <CENTER>
*
*      sphere { <CENTER>, RADIUS, [strength] STRENGTH
*        [ translate <VECTOR> ]
*        [ rotate <VECTOR> ]
*        [ scale <VECTOR> ]
*        [ finish { ... } ]
*        [ pigment { ... } ]
*        [ tnormal { ... } ]
*        [ texture { ... } ]
*      }
*
*      cylinder { <END1>, <END2>, RADIUS, [strength] STRENGTH
*        [ translate <VECTOR> ]
*        [ rotate <VECTOR> ]
*        [ scale <VECTOR> ]
*        [ finish { ... } ]
*        [ pigment { ... } ]
*        [ tnormal { ... } ]
*        [ texture { ... } ]
*      }
*
*      [ sturm ]
*      [ hierarchy FLAG ]
*    }
*
*  ---
*
*  Jul 1994 : Most functions rewritten, bounding hierarchy added. [DB]
*
*  Aug 1994 : Cylindrical blobs added. [DB]
*
*  Sep 1994 : Multi-texturing added (each component can have its own texture).
*             Translation, rotation and scaling of each component added. [DB]
*
*  Oct 1994 : Adopted the method for the bounding slab creation to build the
*             bounding sphere hierarchy of the blob to get a much better
*             hierarchy. Improved bounding sphere calculation for tighter
*             bounds. [DB]
*
*  Dec 1994 : Added code for dynamic blob queue allocation. [DB]
*
*  Feb 1995 : Moved bounding sphere stuff into a seperate file. [DB]
*
*****************************************************************************/

#include "frame.h"
#include "povray.h"
#include "vector.h"
#include "povproto.h"
#include "blob.h"
#include "bbox.h"
#include "bsphere.h"
#include "lighting.h"
#include "matrices.h"
#include "objects.h"
#include "polysolv.h"
#include "texture.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Minimal intersection depth for a valid intersection. */

#define DEPTH_TOLERANCE 1.0e-2

/* Tolerance for inside test. */

#define INSIDE_TOLERANCE 1.0e-6

/* Ray enters/exits a component. */

#define ENTERING 0
#define EXITING  1



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static void element_normal (VECTOR Result, VECTOR P, BLOB_ELEMENT *Element);
static int intersect_element (VECTOR P, VECTOR D, BLOB_ELEMENT *Element, DBL mindist, DBL *t0, DBL *t1);
static void insert_hit (BLOB_ELEMENT *Element, DBL t0, DBL t1, BLOB_INTERVAL *intervals, int *cnt);
static int determine_influences (VECTOR P, VECTOR D, BLOB *Blob, DBL mindist, BLOB_INTERVAL *intervals);
static DBL calculate_field_value (BLOB *Blob, VECTOR P);
static DBL calculate_element_field (BLOB_ELEMENT *Element, VECTOR P);

static int intersect_cylinder (BLOB_ELEMENT *Element, VECTOR P, VECTOR D, DBL mindist, DBL *tmin, DBL *tmax);
static int intersect_hemisphere (BLOB_ELEMENT *Element, VECTOR P, VECTOR D, DBL mindist, DBL *tmin, DBL *tmax);
static int intersect_sphere (BLOB_ELEMENT *Element, VECTOR P, VECTOR D, DBL mindist, DBL *tmin, DBL *tmax);
static int intersect_ellipsoid (BLOB_ELEMENT *Element, VECTOR P, VECTOR D, DBL mindist, DBL *tmin, DBL *tmax);

static void get_element_bounding_sphere (BLOB_ELEMENT *Element, VECTOR Center, DBL *Radius2);
static void build_bounding_hierarchy (BLOB *Blob);

static void init_blob_element (BLOB_ELEMENT *Element);
static void determine_element_texture (BLOB *Blob,
  BLOB_ELEMENT *Element, TEXTURE *Texture, VECTOR P, int *Count,
  TEXTURE **Textures, DBL *Weights);

static void insert_node (BSPHERE_TREE *Node, int *size);

static int  All_Blob_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int  Inside_Blob (VECTOR point, OBJECT *Object);
static void Blob_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static BLOB *Copy_Blob (OBJECT *Object);
static void Translate_Blob (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Blob (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Blob (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Invert_Blob (OBJECT *Object);
static void Transform_Blob (OBJECT *Object, TRANSFORM *Trans);
static void Destroy_Blob (OBJECT *Object);
static void Compute_Blob_BBox (BLOB *Blob);



/*****************************************************************************
* Local variables
******************************************************************************/

METHODS Blob_Methods =
{
  All_Blob_Intersections,
  Inside_Blob, Blob_Normal,
  (COPY_METHOD)Copy_Blob,
  Translate_Blob, Rotate_Blob, Scale_Blob, Transform_Blob,
  Invert_Blob, Destroy_Blob
};

static BSPHERE_TREE **Queue;

/* Maximum number of entries in queue during hierarchy traversal. */

static unsigned Max_Queue_Size = 1024;



/*****************************************************************************
*
* FUNCTION
*
*   All_Blob_Intersections
*
* INPUT
*
*   Object      - Object
*   Ray         - Ray
*
* OUTPUT
*
*   Depth_Stack - Intersection stack
*
* RETURNS
*
*   int - TRUE, if a intersection was found
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Generate intervals of influence for each component. After these
*   are made, determine their aggregate effect on the ray. As the
*   individual intervals are checked, a quartic is generated
*   that represents the density at a particular point on the ray.
*
*   Explanation for spherical components:
*
*   After making the substitutions in MakeBlob, there is a formula
*   for each component that has the form:
*
*      c0 * r^4 + c1 * r^2 + c2.
*
*   In order to determine the influence on the ray of all of the
*   individual components, we start by determining the distance
*   from any point on the ray to the specified point.  This can
*   be found using the pythagorean theorem, using C as the center
*   of this component, P as the start of the ray, and D as the
*   direction of travel of the ray:
*
*      r^2 = (t * D + P - C) . (t * D + P - C)
*
*   we insert this equation for each appearance of r^2 in the
*   components' formula, giving:
*
*      r^2 = D.D t^2 + 2 t D . (P - C) + (P - C) . (P - C)
*
*   Since the direction vector has been normalized, D.D = 1.
*   Using the substitutions:
*
*      t0 = (P - C) . (P - C),
*      t1 = D . (P - C)
*
*   We can write the formula as:
*
*      r^2 = t0 + 2 t t1 + t^2
*
*   Taking r^2 and substituting into the formula for this component
*   of the Blob we get the formula:
*
*      density = c0 * (r^2)^2 + c1 * r^2 + c2,
*
*   or:
*
*      density = c0 * (t0 + 2 t t1 + t^2)^2 +
*                c1 * (t0 + 2 t t1 + t^2) +
*                c2
*
*   Expanding terms and collecting with respect to "t" gives:
*
*      t^4 * c0 +
*      t^3 * 4 c0 t1 +
*      t^2 * (c1 + 2 * c0 t0 + 4 c0 t1^2)
*      t   * 2 (c1 t1 + 2 c0 t0 t1) +
*            c2 + c1*t0 + c0*t0^2
*
*   This formula can now be solved for "t" by any of the quartic
*   root solvers that are available.
*
* CHANGES
*
*   Jul 1994 : Added code for cylindrical and ellipsoidical blobs. [DB]
*
*   Oct 1994 : Added code to convert polynomial into a bezier curve for
*              a quick test if an intersection exists in an interval. [DB]
*
*   Sep 1995 : Added code to avoid numerical problems with distant blobs. [DB]
*
******************************************************************************/

static int All_Blob_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  int i, j, cnt;
  int root_count, in_flag;
  int Intersection_Found = FALSE;
  DBL t0, t1, t2, c0, c1, c2;
  DBL dist, len, *fcoeffs, coeffs[5], roots[4];
  DBL start_dist;
  VECTOR P, D, V1, PP, DD;
  VECTOR IPoint;
  BLOB_INTERVAL *intervals;
  BLOB_ELEMENT *Element;
  BLOB *Blob = (BLOB *)Object;

  DBL l, w, newcoeffs[5], dk[5];

  Increase_Counter(stats[Ray_Blob_Tests]);

  /* Transform the ray into blob space. */

  if (Blob->Trans != NULL)
  {
    MInvTransPoint(P, Ray->Initial, Blob->Trans);
    MInvTransDirection(D, Ray->Direction, Blob->Trans);

    VLength(len, D);
    VInverseScaleEq(D, len);
  }
  else
  {
    Assign_Vector(P, Ray->Initial);
    Assign_Vector(D, Ray->Direction);

    len = 1.0;
  }

  /* Get the intervals along the ray where each component has an effect. */

  intervals = Blob->Data->Intervals;

  if ((cnt = determine_influences(P, D, Blob, DEPTH_TOLERANCE, intervals)) == 0)
  {
    /* Ray doesn't hit any of the component elements. */

    return (FALSE);
  }

  /* To avoid numerical problems we start at the first interval. */

  if ((start_dist = intervals[0].bound) < Small_Tolerance)
  {
    start_dist = 0.0;
  }

  for (i = 0; i < cnt; i++)
  {
    intervals[i].bound -= start_dist;
  }

  /* Get the new starting point. */

  VAddScaledEq(P, start_dist, D);

  /* Clear out the coefficients. */

  coeffs[0] =
  coeffs[1] =
  coeffs[2] =
  coeffs[3] = 0.0;
  coeffs[4] = - Blob->Data->Threshold;
  
  /*
   * Step through the list of intersection points, adding the
   * influence of each component as it appears. 
   */

  fcoeffs = NULL;
  
  for (i = in_flag = 0; i < cnt; i++)
  {
    if ((intervals[i].type & 1) == ENTERING)
    {
      /*
       * Something is just starting to influence the ray, so calculate
       * its coefficients and add them into the pot. 
       */
      
      in_flag++;

      Element = intervals[i].Element;
      
      switch (Element->Type)
      {
        case BLOB_SPHERE:
        
          VSub(V1, P, Element->O);
        
          VDot(t0, V1, V1);
          VDot(t1, V1, D);

          c0 = Element->c[0];
          c1 = Element->c[1];
          c2 = Element->c[2];
        
          fcoeffs = &(Element->f[0]);
        
          fcoeffs[0] = c0;
          fcoeffs[1] = 4.0 * c0 * t1;
          fcoeffs[2] = 2.0 * c0 * (2.0 * t1 * t1 + t0) + c1;
          fcoeffs[3] = 2.0 * t1 * (2.0 * c0 * t0 + c1);
          fcoeffs[4] = t0 * (c0 * t0 + c1) + c2;
        
          break;
        
        case BLOB_ELLIPSOID:
        
          MInvTransPoint(PP, P, Element->Trans);
          MInvTransDirection(DD, D, Element->Trans);
        
          VSub(V1, PP, Element->O);
        
          VDot(t0, V1, V1);
          VDot(t1, V1, DD);
          VDot(t2, DD, DD);
        
          c0 = Element->c[0];
          c1 = Element->c[1];
          c2 = Element->c[2];
        
          fcoeffs = &(Element->f[0]);
        
          fcoeffs[0] = c0 * t2 * t2;
          fcoeffs[1] = 4.0 * c0 * t1 * t2;
          fcoeffs[2] = 2.0 * c0 * (2.0 * t1 * t1 + t0 * t2) + c1 * t2;
          fcoeffs[3] = 2.0 * t1 * (2.0 * c0 * t0 + c1);
          fcoeffs[4] = t0 * (c0 * t0 + c1) + c2;

          break;
        
        case BLOB_BASE_HEMISPHERE:
        case BLOB_APEX_HEMISPHERE:
        
          MInvTransPoint(PP, P, Element->Trans);
          MInvTransDirection(DD, D, Element->Trans);
        
          if (Element->Type == BLOB_APEX_HEMISPHERE)
          {
            PP[Z] -= Element->len;
          }

          VDot(t0, PP, PP);
          VDot(t1, PP, DD);
          VDot(t2, DD, DD);

          c0 = Element->c[0];
          c1 = Element->c[1];
          c2 = Element->c[2];

          fcoeffs = &(Element->f[0]);

          fcoeffs[0] = c0 * t2 * t2;
          fcoeffs[1] = 4.0 * c0 * t1 * t2;
          fcoeffs[2] = 2.0 * c0 * (2.0 * t1 * t1 + t0 * t2) + c1 * t2;
          fcoeffs[3] = 2.0 * t1 * (2.0 * c0 * t0 + c1);
          fcoeffs[4] = t0 * (c0 * t0 + c1) + c2;
        
          break;
        
        case BLOB_CYLINDER:

          /* Transform ray into cylinder space. */

          MInvTransPoint(PP, P, Element->Trans);
          MInvTransDirection(DD, D, Element->Trans);
        
          t0 = PP[X] * PP[X] + PP[Y] * PP[Y];
          t1 = PP[X] * DD[X] + PP[Y] * DD[Y];
          t2 = DD[X] * DD[X] + DD[Y] * DD[Y];

          c0 = Element->c[0];
          c1 = Element->c[1];
          c2 = Element->c[2];
        
          fcoeffs = &(Element->f[0]);
          
          fcoeffs[0] = c0 * t2 * t2;
          fcoeffs[1] = 4.0 * c0 * t1 * t2;
          fcoeffs[2] = 2.0 * c0 * (2.0 * t1 * t1 + t0 * t2) + c1 * t2;
          fcoeffs[3] = 2.0 * t1 * (2.0 * c0 * t0 + c1);
          fcoeffs[4] = t0 * (c0 * t0 + c1) + c2;

          break;
        
        default:

          Error("Unknown blob component in All_Blob_Intersections().\n");
      }
      
      for (j = 0; j < 5; j++)
      {
        coeffs[j] += fcoeffs[j];
      }
    }
    else
    {
      /* 
       * We are losing the influence of a component -->
       * subtract off its coefficients. 
       */
      
      fcoeffs = intervals[i].Element->f;

      for (j = 0; j < 5; j++)
      {
        coeffs[j] -= fcoeffs[j];
      }
      
      /* If no components are currently affecting the ray ---> skip ahead. */
      
      if (--in_flag == 0)
      {
        continue;
      }
    }

    /*
     * If the following intersection lies close to the current intersection
     * then first add/subtract next region before testing. [DB 7/94] 
     */
    
    if ((i + 1 < cnt) && (fabs(intervals[i].bound - intervals[i + 1].bound) < EPSILON))
    {
      continue;
    }

    /*
     * Transform polynomial in a way that the interval boundaries are moved
     * to 0 and 1, i. e. the roots of interest are between 0 and 1. [DB 10/94]
     */

    l = intervals[i].bound;
    w = intervals[i+1].bound - l;

    newcoeffs[0] = coeffs[0] * w * w * w * w;
    newcoeffs[1] = (coeffs[1] + 4.0 * coeffs[0] * l) * w * w * w;
    newcoeffs[2] = (3.0 * l * (2.0 * coeffs[0] * l + coeffs[1]) + coeffs[2]) * w * w;
    newcoeffs[3] = (2.0 * l * (2.0 * l * (coeffs[0] * l + 0.75 * coeffs[1]) + coeffs[2]) + coeffs[3]) * w;
    newcoeffs[4] = l * (l * (l * (coeffs[0] * l + coeffs[1]) + coeffs[2]) + coeffs[3]) + coeffs[4];

    /* Calculate coefficients of corresponding bezier curve. [DB 10/94] */

    dk[0] = newcoeffs[4];
    dk[1] = newcoeffs[4] + 0.25 * newcoeffs[3];
    dk[2] = newcoeffs[4] + 0.50 * (newcoeffs[3] + newcoeffs[2] / 12.0);
    dk[3] = newcoeffs[4] + 0.50 * (0.375 * newcoeffs[3] + newcoeffs[2] + 0.125 * newcoeffs[1]);
    dk[4] = newcoeffs[4] + newcoeffs[3] + newcoeffs[2] + newcoeffs[1] + newcoeffs[0];

    /*
     * Skip this interval if the ray doesn't intersect the convex hull of the
     * bezier curve, because no valid intersection will be found. [DB 10/94]
     */

    if (((dk[0] >= 0.0) && (dk[1] >= 0.0) && (dk[2] >= 0.0) && (dk[3] >= 0.0) && (dk[4] >= 0.0)) ||
        ((dk[0] <= 0.0) && (dk[1] <= 0.0) && (dk[2] <= 0.0) && (dk[3] <= 0.0) && (dk[4] <= 0.0)))
    {
      continue;
    }

    /*
     * Now we could do bezier clipping to find the roots
     * but I have no idea how this works. [DB 2/95]
     */


    /* Solve polynomial. */

    root_count = Solve_Polynomial(4, coeffs, roots, Test_Flag(Blob, STURM_FLAG), 1.0e-11);

    /* See if any of the roots are valid. */

    for (j = 0; j < root_count; j++)
    {
      dist = roots[j];

      /*
       * First see if the root is in the interval of influence of
       * the currently active components.
       */

      if ((dist >= intervals[i].bound) &&
          (dist <= intervals[i+1].bound))
      {
        /* Correct distance. */

        dist = (dist + start_dist) / len;

        if ((dist > DEPTH_TOLERANCE) && (dist < Max_Distance))
        {
          VEvaluateRay(IPoint, Ray->Initial, dist, Ray->Direction);

          if (Point_In_Clip(IPoint, Object->Clip))
          {
            push_entry(dist, IPoint, Object, Depth_Stack);

            Intersection_Found = TRUE;
          }
        }
      }
    }

    /*
     * If the blob isn't used inside a CSG and we have found at least
     * one intersection then we are ready, because all possible intersections
     * will be further away (we have a sorted list!). [DB 7/94]
     */

    if (!(Blob->Type & IS_CHILD_OBJECT) && (Intersection_Found))
    {
      break;
    }
  }

  if (Intersection_Found)
  {
    Increase_Counter(stats[Ray_Blob_Tests_Succeeded]);
  }

  return (Intersection_Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   insert_hit
*
* INPUT
*
*   Blob      - Pointer to blob structure
*   Element   - Element to insert
*   t0, t1    - Intersection depths
*
* OUTPUT
*
*   intervals - Pointer to sorted list of hits
*   cnt       - Number of hits in intervals
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Store the points of intersection. Keep track of: whether this is
*   the start or end point of the hit, which component was pierced
*   by the ray, and the point along the ray that the hit occured at.
*
* CHANGES
*
*   Oct 1994 : Modified to use memmove instead of loops for copying. [DB]
*   Sep 1995 : Changed to allow use of memcpy if memmove isn't available. [AED]
*   Jul 1996 : Changed to use POV_MEMMOVE, which can be memmove or pov_memmove.
*   Oct 1996 : Changed to avoid unnecessary compares. [DB]
*
******************************************************************************/

static void insert_hit(BLOB_ELEMENT *Element, DBL t0, DBL  t1, BLOB_INTERVAL *intervals, int *cnt)
{
  int k;

  /* We are entering the component. */

  intervals[*cnt].type    = Element->Type | ENTERING;
  intervals[*cnt].bound   = t0;
  intervals[*cnt].Element = Element;

  for (k = 0; t0 > intervals[k].bound; k++);

  if (k < *cnt)
  {
    /*
     * This hit point is smaller than one that already exists -->
     * bump the rest and insert it here.
     */

    POV_MEMMOVE(&intervals[k+1], &intervals[k], (*cnt-k)*sizeof(BLOB_INTERVAL));

    /* We are entering the component. */

    intervals[k].type    = Element->Type | ENTERING;
    intervals[k].bound   = t0;
    intervals[k].Element = Element;

    (*cnt)++;

    /* We are exiting the component. */

    intervals[*cnt].type    = Element->Type | EXITING;
    intervals[*cnt].bound   = t1;
    intervals[*cnt].Element = Element;

    for (k = k + 1; t1 > intervals[k].bound; k++);

    if (k < *cnt)
    {
      POV_MEMMOVE(&intervals[k+1], &intervals[k], (*cnt-k)*sizeof(BLOB_INTERVAL));

      /* We are exiting the component. */

      intervals[k].type    = Element->Type | EXITING;
      intervals[k].bound   = t1;
      intervals[k].Element = Element;
    }

    (*cnt)++;
  }
  else
  {
    /* Just plop the start and end points at the end of the list */

    (*cnt)++;

    /* We are exiting the component. */

    intervals[*cnt].type    = Element->Type | EXITING;
    intervals[*cnt].bound   = t1;
    intervals[*cnt].Element = Element;

    (*cnt)++;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_cylinder
*
* INPUT
*
*   Element    - Pointer to element structure
*   P, D       - Ray = P + t * D
*   mindist    - Min. valid distance
*
* OUTPUT
*
*   tmin, tmax - Intersection depths found
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer (with help from Alexander Enzmann)
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jul 1994 : Creation.
*
******************************************************************************/

static int intersect_cylinder(BLOB_ELEMENT *Element, VECTOR P, VECTOR  D, DBL mindist, DBL  *tmin, DBL  *tmax)
{
  DBL a, b, c, d, t, u, v, w, len;
  VECTOR PP, DD;

  /* Transform ray into cylinder space. */

  MInvTransPoint(PP, P, Element->Trans);
  MInvTransDirection(DD, D, Element->Trans);

  VLength(len, DD);
  VInverseScaleEq(DD, len);

  /* Intersect ray with cylinder. */

  a = DD[X] * DD[X] + DD[Y] * DD[Y];

  if (a > EPSILON)
  {
    b = PP[X] * DD[X] + PP[Y] * DD[Y];
    c = PP[X] * PP[X] + PP[Y] * PP[Y] - Element->rad2;

    d = b * b - a * c;

    if (d > EPSILON)
    {
      d = sqrt(d);

      t = ( - b + d) / a;

      w = PP[Z] + t * DD[Z];

      if ((w >= 0.0) && (w <= Element->len))
      {
        if (t < *tmin) { *tmin = t; }
        if (t > *tmax) { *tmax = t; }
      }

      t = ( - b - d) / a;

      w = PP[Z] + t * DD[Z];

      if ((w >= 0.0) && (w <= Element->len))
      {
        if (t < *tmin) { *tmin = t; }
        if (t > *tmax) { *tmax = t; }
      }
    }
  }

  /* Intersect base/cap plane. */

  if (fabs(DD[Z]) > EPSILON)
  {
    /* Intersect base plane. */

    t = - PP[Z] / DD[Z];

    u = PP[X] + t * DD[X];
    v = PP[Y] + t * DD[Y];

    if ((u * u + v * v) <= Element->rad2)
    {
      if (t < *tmin) { *tmin = t; }
      if (t > *tmax) { *tmax = t; }
    }

    /* Intersect cap plane. */

    t = (Element->len - PP[Z]) / DD[Z];

    u = PP[X] + t * DD[X];
    v = PP[Y] + t * DD[Y];

    if ((u * u + v * v) <= Element->rad2)
    {
      if (t < *tmin) { *tmin = t; }
      if (t > *tmax) { *tmax = t; }
    }
  }

  /* Check if the intersections are valid. */

  *tmin /= len;
  *tmax /= len;

  if (*tmin < mindist) { *tmin = 0.0; }
  if (*tmax < mindist) { *tmax = 0.0; }

  if (*tmin >= *tmax)
  {
    return (FALSE);
  }

  return (TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_ellipsoid
*
* INPUT
*
*   Element    - Pointer to element structure
*   P, D       - Ray = P + t * D
*   mindist    - Min. valid distance
*
* OUTPUT
*
*   tmin, tmax - Intersection depths found
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

static int intersect_ellipsoid(BLOB_ELEMENT *Element, VECTOR P, VECTOR  D, DBL mindist, DBL  *tmin, DBL  *tmax)
{
  DBL b, d, t, len;
  VECTOR V1, PP, DD;

  MInvTransPoint(PP, P, Element->Trans);
  MInvTransDirection(DD, D, Element->Trans);

  VLength(len, DD);
  VInverseScaleEq(DD, len);

  VSub(V1, PP, Element->O);
  VDot(b, V1, DD);
  VDot(t, V1, V1);

  d = b * b - t + Element->rad2;

  if (d < EPSILON)
  {
    return (FALSE);
  }

  d = sqrt(d);

  *tmax = ( - b + d) / len;  if (*tmax < mindist) { *tmax = 0.0; }
  *tmin = ( - b - d) / len;  if (*tmin < mindist) { *tmin = 0.0; }

  if (*tmax == *tmin)
  {
    return (FALSE);
  }
  else
  {
    if (*tmax < *tmin)
    {
      d = *tmin;  *tmin = *tmax;  *tmax = d;
    }
  }

  return (TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_hemisphere
*
* INPUT
*
*   Element    - Pointer to element structure
*   P, D       - Ray = P + t * D
*   mindist    - Min. valid distance
*
* OUTPUT
*
*   tmin, tmax - Intersection depths found
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jul 1994 : Creation (with help from Alexander Enzmann).
*
******************************************************************************/

static int intersect_hemisphere(BLOB_ELEMENT *Element, VECTOR P, VECTOR  D, DBL mindist, DBL  *tmin, DBL  *tmax)
{
  DBL b, d, t, z1, z2, len;
  VECTOR PP, DD;

  /* Transform ray into hemisphere space. */

  MInvTransPoint(PP, P, Element->Trans);
  MInvTransDirection(DD, D, Element->Trans);

  VLength(len, DD);
  VInverseScaleEq(DD, len);

  if (Element->Type == BLOB_BASE_HEMISPHERE)
  {
    VDot(b, PP, DD);
    VDot(t, PP, PP);

    d = b * b - t + Element->rad2;

    if (d < EPSILON)
    {
      return (FALSE);
    }

    d = sqrt(d);

    *tmax = - b + d;
    *tmin = - b - d;

    if (*tmax < *tmin)
    {
      d = *tmin;  *tmin = *tmax;  *tmax = d;
    }

    /* Cut intersection at the plane. */

    z1 = PP[Z] + *tmin * DD[Z];
    z2 = PP[Z] + *tmax * DD[Z];

    /* If both points are inside --> no intersection */

    if ((z1 >= 0.0) && (z2 >= 0.0))
    {
      return (FALSE);
    }

    /* If both points are outside --> intersections found */

    if ((z1 < 0.0) && (z2 < 0.0))
    {
      *tmin /= len;
      *tmax /= len;

      return (TRUE);
    }

    /* Determine intersection with plane. */

    t = - PP[Z] / DD[Z];

    if (z1 >= 0.0)
    {
      /* Ray is crossing the plane from inside to outside. */

      *tmin = (t < mindist) ? 0.0 : t;
    }
    else
    {
      /* Ray is crossing the plane from outside to inside. */

      *tmax = (t < mindist) ? 0.0 : t;
    }

    *tmin /= len;
    *tmax /= len;

    return (TRUE);
  }
  else
  {
    PP[Z] -= Element->len;

    VDot(b, PP, DD);
    VDot(t, PP, PP);

    d = b * b - t + Element->rad2;

    if (d < EPSILON)
    {
      return (FALSE);
    }

    d = sqrt(d);

    *tmax = - b + d;
    *tmin = - b - d;

    if (*tmax < *tmin)
    {
      d = *tmin;  *tmin = *tmax;  *tmax = d;
    }

    /* Cut intersection at the plane. */

    z1 = PP[Z] + *tmin * DD[Z];
    z2 = PP[Z] + *tmax * DD[Z];

    /* If both points are inside --> no intersection */

    if ((z1 <= 0.0) && (z2 <= 0.0))
    {
      return (FALSE);
    }

    /* If both points are outside --> intersections found */

    if ((z1 > 0.0) && (z2 > 0.0))
    {
      *tmin /= len;
      *tmax /= len;

      return (TRUE);
    }

    /* Determine intersection with plane. */

    t = - PP[Z] / DD[Z];

    if (z1 <= 0.0)
    {
      /* Ray is crossing the plane from inside to outside. */

      *tmin = (t < mindist) ? 0.0 : t;
    }
    else
    {
      /* Ray is crossing the plane from outside to inside. */

      *tmax = (t < mindist) ? 0.0 : t;
    }

    *tmin /= len;
    *tmax /= len;

    return (TRUE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_sphere
*
* INPUT
*
*   Element    - Pointer to element structure
*   P, D       - Ray = P + t * D
*   mindist    - Min. valid distance
*
* OUTPUT
*
*   tmin, tmax - Intersection depths found
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jul 1994 : Creation (with help from Alexander Enzmann).
*
******************************************************************************/

static int intersect_sphere(BLOB_ELEMENT *Element, VECTOR P, VECTOR  D, DBL mindist, DBL  *tmin, DBL  *tmax)
{
  DBL b, d, t;
  VECTOR V1;

  VSub(V1, P, Element->O);
  VDot(b, V1, D);
  VDot(t, V1, V1);

  d = b * b - t + Element->rad2;

  if (d < EPSILON)
  {
    return (FALSE);
  }

  d = sqrt(d);

  *tmax = - b + d;  if (*tmax < mindist) { *tmax = 0.0; }
  *tmin = - b - d;  if (*tmin < mindist) { *tmin = 0.0; }

  if (*tmax == *tmin)
  {
    return (FALSE);
  }
  else
  {
    if (*tmax < *tmin)
    {
      d = *tmin;  *tmin = *tmax;  *tmax = d;
    }
  }

  return (TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_element
*
* INPUT
*
*   P, D       - Ray = P + t * D
*   Element    - Pointer to element structure
*   mindist    - Min. valid distance
*
* OUTPUT
*
*   tmin, tmax - Intersection depths found
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jul 1994 : Creation.
*
******************************************************************************/

static int intersect_element(VECTOR P, VECTOR  D, BLOB_ELEMENT *Element, DBL mindist, DBL  *tmin, DBL  *tmax)
{
#ifdef BLOB_EXTRA_STATS
  Increase_Counter(stats[Blob_Element_Tests]);
#endif

  *tmin = BOUND_HUGE;
  *tmax = - BOUND_HUGE;

  switch (Element->Type)
  {
    case BLOB_SPHERE:

      if (!intersect_sphere(Element, P, D, mindist, tmin, tmax))
      {
       return (FALSE);
      }

      break;

    case BLOB_ELLIPSOID:

      if (!intersect_ellipsoid(Element, P, D, mindist, tmin, tmax))
      {
        return (FALSE);
      }

      break;

    case BLOB_BASE_HEMISPHERE:
    case BLOB_APEX_HEMISPHERE:

      if (!intersect_hemisphere(Element, P, D, mindist, tmin, tmax))
      {
        return (FALSE);
      }

      break;

    case BLOB_CYLINDER:

      if (!intersect_cylinder(Element, P, D, mindist, tmin, tmax))
      {
        return (FALSE);
      }

      break;
  }

#ifdef BLOB_EXTRA_STATS
  Increase_Counter(stats[Blob_Element_Tests_Succeeded]);
#endif

  return (TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
*   determine_influences
*
* INPUT
*
*   P, D       - Ray = P + t * D
*   Blob       - Pointer to blob structure
*   mindist    - Min. valid distance
*
* OUTPUT
*
*   intervals  - Sorted list of intersections found
*
* RETURNS
*
*   int - Number of intersection found
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Make a sorted list of points along the ray at which the various blob
*   components start and stop adding their influence.
*
* CHANGES
*
*   Jul 1994 : Added code for bounding hierarchy traversal. [DB]
*
******************************************************************************/

static int determine_influences(VECTOR P, VECTOR  D, BLOB *Blob, DBL mindist, BLOB_INTERVAL *intervals)
{
  int i;
  int cnt, size;
  DBL b, t, t0, t1;
  VECTOR V1;
  BSPHERE_TREE *Tree;

  cnt = 0;

  if (Blob->Data->Tree == NULL)
  {
    /* There's no bounding hierarchy so just step through all elements. */

    for (i = 0; i < Blob->Data->Number_Of_Components; i++)
    {
      if (intersect_element(P, D, &Blob->Data->Entry[i], mindist, &t0, &t1))
      {
        insert_hit(&Blob->Data->Entry[i], t0, t1, intervals, &cnt);
      }
    }
  }
  else
  {
    /* Use blob's bounding hierarchy. */

    size = 0;

    Queue[size++] = Blob->Data->Tree;

    while (size > 0)
    {
      Tree = Queue[--size];

      /* Test if current node is a leaf. */

      if (Tree->Entries <= 0)
      {
        /* Test element. */

        if (intersect_element(P, D, (BLOB_ELEMENT *)Tree->Node, mindist, &t0, &t1))
        {
          insert_hit((BLOB_ELEMENT *)Tree->Node, t0, t1, intervals, &cnt);
        }
      }
      else
      {
        /* Test all sub-nodes. */

        for (i = 0; i < (int)Tree->Entries; i++)
        {
#ifdef BLOB_EXTRA_STATS
          Increase_Counter(stats[Blob_Bound_Tests]);
#endif

          VSub(V1, Tree->Node[i]->C, P);
          VDot(b, V1, D);
          VDot(t, V1, V1);

          if ((t - Sqr(b)) <= Tree->Node[i]->r2)
          {
#ifdef BLOB_EXTRA_STATS
            Increase_Counter(stats[Blob_Bound_Tests_Succeeded]);
#endif

            insert_node(Tree->Node[i], &size);
          }
        }
      }
    }
  }

  return (cnt);
}



/*****************************************************************************
*
* FUNCTION
*
*   calculate_element_field
*
* INPUT
*
*   Element - Pointer to element structure
*   P       - Point whos field value is calculated
*
* OUTPUT
*
* RETURNS
*
*   DBL - Field value
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Calculate the field value of a single element in a given point P
*   (which must already have been transformed into blob space).
*
* CHANGES
*
*   Jul 1994 : Added code for cylindrical and ellipsoidical blobs. [DB]
*
******************************************************************************/

static DBL calculate_element_field(BLOB_ELEMENT *Element, VECTOR P)
{
  DBL rad2, density;
  VECTOR V1, PP;

  density = 0.0;

  switch (Element->Type)
  {
    case BLOB_SPHERE:

      VSub(V1, P, Element->O);

      VDot(rad2, V1, V1);

      if (rad2 < Element->rad2)
      {
        density = rad2 * (rad2 * Element->c[0] + Element->c[1]) + Element->c[2];
      }

      break;

    case BLOB_ELLIPSOID:

      MInvTransPoint(PP, P, Element->Trans);

      VSub(V1, PP, Element->O);

      VDot(rad2, V1, V1);

      if (rad2 < Element->rad2)
      {
        density = rad2 * (rad2 * Element->c[0] + Element->c[1]) + Element->c[2];
      }

      break;

    case BLOB_BASE_HEMISPHERE:

      MInvTransPoint(PP, P, Element->Trans);

      if (PP[Z] <= 0.0)
      {
        VDot(rad2, PP, PP);

        if (rad2 <= Element->rad2)
        {
          density = rad2 * (rad2 * Element->c[0] + Element->c[1]) + Element->c[2];
        }
      }

      break;

    case BLOB_APEX_HEMISPHERE:

      MInvTransPoint(PP, P, Element->Trans);

      PP[Z] -= Element->len;

      if (PP[Z] >= 0.0)
      {
        VDot(rad2, PP, PP);

        if (rad2 <= Element->rad2)
        {
          density = rad2 * (rad2 * Element->c[0] + Element->c[1]) + Element->c[2];
        }
      }

      break;

    case BLOB_CYLINDER:

      MInvTransPoint(PP, P, Element->Trans);

      if ((PP[Z] >= 0.0) && (PP[Z] <= Element->len))
      {
        if ((rad2 = Sqr(PP[X]) + Sqr(PP[Y])) <= Element->rad2)
        {
          density = rad2 * (rad2 * Element->c[0] + Element->c[1]) + Element->c[2];
        }
      }

      break;
  }

  return (density);
}



/*****************************************************************************
*
* FUNCTION
*
*   calculate_field_value
*
* INPUT
*
*   Blob - Pointer to blob structure
*   P       - Point whos field value is calculated
*
* OUTPUT
*
* RETURNS
*
*   DBL - Field value
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the field value of a blob in a given point P
*   (which must already have been transformed into blob space).
*
* CHANGES
*
*   Jul 1994 : Added code for bounding hierarchy traversal. [DB]
*
******************************************************************************/

static DBL calculate_field_value(BLOB *Blob, VECTOR P)
{
  int i;
  int size;
  DBL density, rad2;
  VECTOR V1;
  BSPHERE_TREE *Tree;

  density = 0.0;

  if (Blob->Data->Tree == NULL)
  {
    /* There's no tree --> step through all elements. */

    for (i = 0; i < Blob->Data->Number_Of_Components; i++)
    {
      density += calculate_element_field(&Blob->Data->Entry[i], P);
    }
  }
  else
  {
    /* A tree exists --> step through the tree. */

    size = 0;

    Queue[size++] = Blob->Data->Tree;

    while (size > 0)
    {
      Tree = Queue[--size];

      /* Test if current node is a leaf. */

      if (Tree->Entries <= 0)
      {
        density += calculate_element_field((BLOB_ELEMENT *)Tree->Node, P);
      }
      else
      {
        /* Test all sub-nodes. */

        for (i = 0; i < (int)Tree->Entries; i++)
        {
          /* Insert sub-node if we are inside. */

          VSub(V1, P, Tree->Node[i]->C);

          VDot(rad2, V1, V1);

          if (rad2 <= Tree->Node[i]->r2)
          {
            insert_node(Tree->Node[i], &size);
          }
        }
      }
    }
  }

  return (density);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Blob
*
* INPUT
*
*   Test_Point - Point to test
*   Object     - Pointer to blob structure
*
* OUTPUT
*
* RETURNS
*
*   int - TRUE if Test_Point is inside
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Calculate the density at the given point and then compare to
*   the threshold to see if we are in or out of the blob.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int Inside_Blob(VECTOR Test_Point, OBJECT *Object)
{
  VECTOR New_Point;
  BLOB *Blob = (BLOB *) Object;

  /* Transform the point into blob space. */

  if (Blob->Trans != NULL)
  {
    MInvTransPoint(New_Point, Test_Point, Blob->Trans);
  }
  else
  {
    Assign_Vector(New_Point, Test_Point);
  }

  if (calculate_field_value(Blob, New_Point) > Blob->Data->Threshold - INSIDE_TOLERANCE)
  {
    /* We are inside. */

    return (!Test_Flag(Blob, INVERTED_FLAG));
  }
  else
  {
    /* We are outside. */

    return (Test_Flag(Blob, INVERTED_FLAG));
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   element_normal
*
* INPUT
*
*   P       - Surface point
*   Element - Pointer to element structure
*
* OUTPUT
*
*   Result  - Element's normal
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the normal of a single element in the point P.
*
* CHANGES
*
*   Jul 1994 : Creation (with help from Alexander Enzmann).
*
******************************************************************************/

static void element_normal(VECTOR Result, VECTOR  P, BLOB_ELEMENT *Element)
{
  DBL val, dist;
  VECTOR V1, PP;

  switch (Element->Type)
  {
    case BLOB_SPHERE:

      VSub(V1, P, Element->O);

      VDot(dist, V1, V1);

      if (dist <= Element->rad2)
      {
        val = -2.0 * Element->c[0] * dist - Element->c[1];

        VAddScaledEq(Result, val, V1);
      }

      break;

    case BLOB_ELLIPSOID:

      MInvTransPoint(PP, P, Element->Trans);

      VSub(V1, PP, Element->O);

      VDot(dist, V1, V1);

      if (dist <= Element->rad2)
      {
        val = -2.0 * Element->c[0] * dist - Element->c[1];

        MTransNormal(V1, V1, Element->Trans);

        VAddScaledEq(Result, val, V1);
      }

      break;

    case BLOB_BASE_HEMISPHERE:

      MInvTransPoint(PP, P, Element->Trans);

      if (PP[Z] <= 0.0)
      {
        VDot(dist, PP, PP);

        if (dist <= Element->rad2)
        {
          val = -2.0 * Element->c[0] * dist - Element->c[1];

          MTransNormal(PP, PP, Element->Trans);

          VAddScaledEq(Result, val, PP);
        }
      }

      break;

    case BLOB_APEX_HEMISPHERE:

      MInvTransPoint(PP, P, Element->Trans);

      PP[Z] -= Element->len;

      if (PP[Z] >= 0.0)
      {
        VDot(dist, PP, PP);

        if (dist <= Element->rad2)
        {
          val = -2.0 * Element->c[0] * dist - Element->c[1];

          MTransNormal(PP, PP, Element->Trans);

          VAddScaledEq(Result, val, PP);
        }
      }

      break;

    case BLOB_CYLINDER:

      MInvTransPoint(PP, P, Element->Trans);

      if ((PP[Z] >= 0.0) && (PP[Z] <= Element->len))
      {
        if ((dist = Sqr(PP[X]) + Sqr(PP[Y])) <= Element->rad2)
        {
          val = -2.0 * Element->c[0] * dist - Element->c[1];

          PP[Z] = 0.0;

          MTransNormal(PP, PP, Element->Trans);

          VAddScaledEq(Result, val, PP);
        }
      }

      break;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Blob_Normal
*
* INPUT
*
*   Object  - Pointer to blob structure
*   Inter   - Pointer to intersection
*
* OUTPUT
*
*   Result  - Blob's normal
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Calculate the blob's surface normal in the intersection point.
*
* CHANGES
*
*   Jul 1994 : Added code for bounding hierarchy traversal. [DB]
*
******************************************************************************/

static void Blob_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  int i;
  int size;
  DBL dist, val;
  VECTOR New_Point, V1;
  BLOB *Blob = (BLOB *) Object;
  BSPHERE_TREE *Tree;

  /* Transform the point into the blob space. */

  if (Blob->Trans != NULL)
  {
    MInvTransPoint(New_Point, Inter->IPoint, Blob->Trans);
  }
  else
  {
    Assign_Vector(New_Point, Inter->IPoint);
  }

  Make_Vector(Result, 0.0, 0.0, 0.0);

  /* For each component that contributes to this point, add its bit to the normal */

  if (Blob->Data->Tree == NULL)
  {
    /* There's no tree --> step through all elements. */

    for (i = 0; i < Blob->Data->Number_Of_Components; i++)
    {
      element_normal(Result, New_Point, &(Blob->Data->Entry[i]));
    }
  }
  else
  {
    /* A tree exists --> step through the tree. */

    size = 0;

    Queue[size++] = Blob->Data->Tree;

    while (size > 0)
    {
      Tree = Queue[--size];

      /* Test if current node is a leaf. */

      if (Tree->Entries <= 0)
      {
        element_normal(Result, New_Point, (BLOB_ELEMENT *)Tree->Node);
      }
      else
      {
        /* Test all sub-nodes. */

        for (i = 0; i < (int)Tree->Entries; i++)
        {
          /* Insert sub-node if we are inside. */

          VSub(V1, New_Point, Tree->Node[i]->C);

          VDot(dist, V1, V1);

          if (dist <= Tree->Node[i]->r2)
          {
            insert_node(Tree->Node[i], &size);
          }
        }
      }
    }
  }

  VDot(val, Result, Result);

  if (val == 0.0)
  {
    Make_Vector(Result, 1.0, 0.0, 0.0);
  }
  else
  {
    /* Normalize normal vector. */

    val = 1.0 / sqrt(val);

    VScaleEq(Result, val);
  }

  /* Transform back to world space. */

  if (Blob->Trans != NULL)
  {
    MTransNormal(Result, Result, Blob->Trans);

    VNormalize(Result, Result);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Blob
*
* INPUT
*
*   Vector - Translation vector
*
* OUTPUT
*
*   Object - Pointer to blob structure
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Translate a blob.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void Translate_Blob(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Blob(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Blob
*
* INPUT
*
*   Vector - Rotation vector
*
* OUTPUT
*
*   Object - Pointer to blob structure
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Rotate a blob.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void Rotate_Blob(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Blob(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Blob
*
* INPUT
*
*   Vector - Scaling vector
*
* OUTPUT
*
*   Object - Pointer to blob structure
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Scale a blob.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void Scale_Blob(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Blob(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Blob
*
* INPUT
*
*   Trans  - Pointer to transformation
*
* OUTPUT
*
*   Object - Pointer to blob structure
*
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Transform a blob.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void Transform_Blob(OBJECT *Object, TRANSFORM *Trans)
{
  int i;
  BLOB *Blob = (BLOB *)Object;

  if (Blob->Trans == NULL)
  {
    Blob->Trans = Create_Transform();
  }

  Recompute_BBox(&Object->BBox, Trans);

  Compose_Transforms(Blob->Trans, Trans);

  for (i = 0; i < Blob->Data->Number_Of_Components; i++)
  {
    Transform_Textures(Blob->Element_Texture[i], Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Blob
*
* INPUT
*
*   Object - Pointer to blob structure
*
* OUTPUT
*
*   Object
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Invert a blob.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void Invert_Blob(OBJECT *Object)
{
  Invert_Flag(Object, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Blob
*
* INPUT
*
*   Object - Pointer to blob structure
*
* OUTPUT
*
*   Object
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Create a new blob.
*
* CHANGES
*
*   -
*
******************************************************************************/

BLOB *Create_Blob()
{
  BLOB *New;

  New = (BLOB *)POV_MALLOC(sizeof (BLOB), "blob");

  INIT_OBJECT_FIELDS(New, BLOB_OBJECT, &Blob_Methods)

  Set_Flag(New, HIERARCHY_FLAG);

  New->Trans = NULL;

  New->Data = NULL;

  New->Element_Texture = NULL;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Blob
*
* INPUT
*
*   Object - Pointer to blob structure
*
* OUTPUT
*
*   Object
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Copy a blob.
*
*   NOTE: The components are not copied, only the number of references is
*         counted, so that Destroy_Blob() knows if they can be destroyed.
*
* CHANGES
*
*   Jul 1994 : Added code for blob data reference counting. [DB]
*
******************************************************************************/

static BLOB *Copy_Blob(OBJECT *Object)
{
  int i;
  BLOB *New, *Old = (BLOB *)Object;

  New = Create_Blob();

  /* Copy blob. */

  *New = *Old;

  New->Trans = Copy_Transform(New->Trans);

  New->Data->References++;

  New->Element_Texture = (TEXTURE **)POV_MALLOC(New->Data->Number_Of_Components*sizeof(TEXTURE *), "blob texture list");

  for (i = 0; i < New->Data->Number_Of_Components; i++)
  {
    New->Element_Texture[i] = Copy_Textures(Old->Element_Texture[i]);
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Blob_List_Element
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   BLOB_LIST * - Pointer to blob element
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a new blob element in the component list used during parsing.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

BLOB_LIST *Create_Blob_List_Element()
{
  BLOB_LIST *New;

  New = (BLOB_LIST *)POV_MALLOC(sizeof(BLOB_LIST), "blob component");

  init_blob_element(&New->elem);

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Blob
*
* INPUT
*
*   Object - Pointer to blob structure
*
* OUTPUT
*
*   Object
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Destroy a blob.
*
*   NOTE: The blob data is destroyed if they are no longer used by any copy.
*
* CHANGES
*
*   Jul 1994 : Added code for blob data reference counting. [DB]
*
*   Dec 1994 : Fixed memory leakage. [DB]
*
*   Aug 1995 : Fixed freeing of already freed memory. [DB]
*
******************************************************************************/

static void Destroy_Blob(OBJECT *Object)
{
  int i;
  BLOB *Blob = (BLOB *)Object;

  Destroy_Transform(Blob->Trans);

  for (i = 0; i < Blob->Data->Number_Of_Components; i++)
  {
    Destroy_Textures(Blob->Element_Texture[i]);
  }

  POV_FREE(Blob->Element_Texture);

  if (--(Blob->Data->References) == 0)
  {
    Destroy_Bounding_Sphere_Hierarchy(Blob->Data->Tree);

    for (i = 0; i < Blob->Data->Number_Of_Components; i++)
    {
      /*
       * Make sure to destroy multiple references of a texture
       * and/or transformation only once. Multiple references
       * are only used with cylindrical blobs. Thus it's
       * enough to ignore all cylinder caps.
       */

      if ((Blob->Data->Entry[i].Type == BLOB_SPHERE) ||
          (Blob->Data->Entry[i].Type == BLOB_ELLIPSOID) ||
          (Blob->Data->Entry[i].Type == BLOB_CYLINDER))
      {
        Destroy_Transform(Blob->Data->Entry[i].Trans);

        Blob->Data->Entry[i].Trans   = NULL;
        Blob->Data->Entry[i].Texture = NULL;
      }
    }

    POV_FREE(Blob->Data->Entry);

    POV_FREE(Blob->Data->Intervals);

    POV_FREE(Blob->Data);
  }

  POV_FREE(Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Blob_BBox
*
* INPUT
*
*   Blob - Blob
*
* OUTPUT
*
*   Blob
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a blob.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

static void Compute_Blob_BBox(BLOB *Blob)
{
  int i;
  DBL radius, radius2;
  VECTOR Center, Min, Max;

  Make_Vector(Min, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
  Make_Vector(Max, - BOUND_HUGE, - BOUND_HUGE, - BOUND_HUGE);

  for (i = 0; i < Blob->Data->Number_Of_Components; i++)
  {
    if (Blob->Data->Entry[i].c[2] > 0.0)
    {
      get_element_bounding_sphere(&Blob->Data->Entry[i], Center, &radius2);

      radius = sqrt(radius2);

      Min[X] = min(Min[X], Center[X] - radius);
      Min[Y] = min(Min[Y], Center[Y] - radius);
      Min[Z] = min(Min[Z], Center[Z] - radius);
      Max[X] = max(Max[X], Center[X] + radius);
      Max[Y] = max(Max[Y], Center[Y] + radius);
      Max[Z] = max(Max[Z], Center[Z] + radius);
    }
  }

  Make_BBox_from_min_max(Blob->BBox, Min, Max);

  if (Blob->Trans != NULL)
  {
    Recompute_BBox(&Blob->BBox, Blob->Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   get_element_bounding_sphere
*
* INPUT
*
*   Element - Pointer to element
*   Center  - Bounding sphere's center
*   Radius2 - Bounding sphere's squared radius
*
* OUTPUT
*
*   Center, Radius2
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding sphere of a blob element.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

static void get_element_bounding_sphere(BLOB_ELEMENT *Element, VECTOR Center, DBL *Radius2)
{
  DBL r, r2 = 0.0;
  VECTOR C, H;

  switch (Element->Type)
  {
    case BLOB_SPHERE:
    case BLOB_ELLIPSOID:

      r2 = Element->rad2;

      Assign_Vector(C, Element->O);

      break;

    case BLOB_BASE_HEMISPHERE:

      r2 = Element->rad2;

      Make_Vector(C, 0.0, 0.0, 0.0);

      break;

    case BLOB_APEX_HEMISPHERE:

      r2 = Element->rad2;

      Make_Vector(C, 0.0, 0.0, Element->len);

      break;

    case BLOB_CYLINDER :

      Make_Vector(C, 0.0, 0.0, 0.5 * Element->len);

      r2 = Element->rad2 + Sqr(0.5 * Element->len);

      break;
  }

  /* Transform bounding sphere if necessary. */

  if (Element->Trans != NULL)
  {
    r = sqrt(r2);

    MTransPoint(C, C, Element->Trans);

    Make_Vector(H, r, r, r);

    MTransDirection(H, H, Element->Trans);

    r = max(max(fabs(H[X]), fabs(H[Y])), fabs(H[Z]));

    r2 = Sqr(r) + EPSILON;
  }

  Assign_Vector(Center, C);

  *Radius2 = r2;
}



/*****************************************************************************
*
* FUNCTION
*
*   init_blob_element
*
* INPUT
*
*   Element - Pointer to blob element
*
* OUTPUT
*
*   Element
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Init blob element.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

static void init_blob_element(BLOB_ELEMENT *Element)
{
  Element->Type = 0;

  Element->index = 0;

  Element->len =
  Element->rad2 = 0.0;

  Element->c[0] =
  Element->c[1] =
  Element->c[2] =
  Element->f[0] =
  Element->f[1] =
  Element->f[2] =
  Element->f[3] =
  Element->f[4] = 0.0;

  Make_Vector(Element->O, 0.0, 0.0, 0.0);

  Element->Texture = NULL;

  Element->Trans = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Make_Blob
*
* INPUT
*
*   Blob       - Pointer to blob structure
*   threshold  - Blob's threshold
*   BlobList   - Pointer to elements
*   npoints    - Number of elements
*
* OUTPUT
*
*   Blob
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Create a blob after it was read from the scene file.
*
*   Starting with the density function: (1-r^2)^2, we have a field
*   that varies in strength from 1 at r = 0 to 0 at r = 1. By
*   substituting r/rad for r, we can adjust the range of influence
*   of a particular component. By multiplication by coeff, we can
*   adjust the amount of total contribution, giving the formula:
*
*     coeff * (1 - (r/rad)^2)^2
*
*   This varies in strength from coeff at r = 0, to 0 at r = rad.
*
* CHANGES
*
*   Jul 1994 : Added code for cylindrical and ellipsoidical blobs. [DB]
*
******************************************************************************/

void Make_Blob(BLOB *Blob, DBL threshold, BLOB_LIST *BlobList, int npoints)
{
  int i, count;
  DBL rad2, coeff;
  BLOB_LIST *temp;
  BLOB_ELEMENT *Entry;

  if (npoints < 1)
  {
    Error("Need at least one component in a blob.");
  }

  /* Figure out how many components there will be. */

  temp = BlobList;

  for (i = count = 0; i < npoints; i++)
  {
    if (temp->elem.Type & BLOB_CYLINDER)
    {
      count += 3;
    }
    else
    {
      count++;
    }

    temp = temp->next;

    /* Test for too many components. [DB 12/94] */

    if (count >= MAX_BLOB_COMPONENTS)
    {
      Error("There are more than %d components in a blob.\n", MAX_BLOB_COMPONENTS);
    }
  }

  /* Initialize the blob data. */

  Blob->Data->Threshold = threshold;

  Entry = Blob->Data->Entry;

  for (i = 0; i < npoints; i++)
  {
    temp = BlobList;

    if ((fabs(temp->elem.c[2]) < EPSILON) || (temp->elem.rad2 < EPSILON))
    {
      Warning(0.0, "Degenerate Blob element\n");
    }

    /* Initialize component. */

    *Entry = temp->elem;

    /* We have a multi-texture blob. */

    if (Entry->Texture != NULL)
    {
      Set_Flag(Blob, MULTITEXTURE_FLAG);
    }

    /* Store blob specific information. */

    switch (temp->elem.Type)
    {
      case BLOB_ELLIPSOID :
      case BLOB_SPHERE :

        rad2 = temp->elem.rad2;

        coeff = temp->elem.c[2];

        Entry->c[0] = coeff / (rad2 * rad2);
        Entry->c[1] = -(2.0 * coeff) / rad2;
        Entry->c[2] = coeff;

        Entry++;

        break;

      case BLOB_CYLINDER :

        rad2 = temp->elem.rad2;

        coeff = temp->elem.c[2];

        /* Create cylindrical component. */

        Entry->c[0] = coeff / (rad2 * rad2);
        Entry->c[1] = -(2.0 * coeff) / rad2;
        Entry->c[2] = coeff;

        Entry++;

        /* Create hemispherical component at the base. */

        *Entry = temp->elem;

        Entry->Type = BLOB_BASE_HEMISPHERE;

        Entry->c[0] = coeff / (rad2 * rad2);
        Entry->c[1] = -(2.0 * coeff) / rad2;
        Entry->c[2] = coeff;

        Entry++;

        /* Create hemispherical component at the apex. */

        *Entry = temp->elem;

        Entry->Type = BLOB_APEX_HEMISPHERE;

        Entry->c[0] = coeff / (rad2 * rad2);
        Entry->c[1] = -(2.0 * coeff) / rad2;
        Entry->c[2] = coeff;

        Entry++;

        break;

      default :

        Error("Unknown blob component.\n");
    }

    /* Get rid of texture non longer needed. */

    BlobList = BlobList->next;

    Destroy_Textures(temp->elem.Texture);

    POV_FREE(temp);
  }

  for (i = 0; i < count; i++)
  {
    Blob->Data->Entry[i].index = i;
  }

  /* Compute bounding box. */

  Compute_Blob_BBox(Blob);

  /* Create bounding sphere hierarchy. */

  if (Test_Flag(Blob, HIERARCHY_FLAG))
  {
    build_bounding_hierarchy(Blob);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Test_Blob_Opacity
*
* INPUT
*
*   Blob - Pointer to blob structure
*
* OUTPUT
*
*   Blob
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Set the opacity flag of the blob according to the opacity
*   of the blob's texture(s).
*
* CHANGES
*
*   Apr 1996 : Creation.
*
******************************************************************************/

void Test_Blob_Opacity(BLOB *Blob)
{
  int i;

  /* Initialize opacity flag to the opacity of the object's texture. */

  if ((Blob->Texture == NULL) || (Test_Opacity(Blob->Texture)))
  {
    Set_Flag(Blob, OPAQUE_FLAG);
  }

  if (Test_Flag(Blob, MULTITEXTURE_FLAG))
  {
    for (i = 0; i < Blob->Data->Number_Of_Components; i++)
    {
      if (Blob->Element_Texture[i] != NULL)
      {
        /* If component's texture isn't opaque the blob is neither. */

        if (!Test_Opacity(Blob->Element_Texture[i]))
        {
          Clear_Flag(Blob, OPAQUE_FLAG);
        }
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   build_bounding_hierarchy
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create the bounding sphere hierarchy.
*
* CHANGES
*
*   Oct 1994 : Creation. (Derived from the bounding slab creation code)
*
******************************************************************************/

static void build_bounding_hierarchy(BLOB *Blob)
{
  int i, nElem, maxelements;
  BSPHERE_TREE **Elements;

  nElem = (int)Blob->Data->Number_Of_Components;

  maxelements = 2 * nElem;

  /*
   * Now allocate an array to hold references to these elements.
   */

  Elements = (BSPHERE_TREE **)POV_MALLOC(maxelements*sizeof(BSPHERE_TREE *), "blob bounding hierarchy");

  /* Init list with blob elements. */

  for (i = 0; i < nElem; i++)
  {
    Elements[i] = (BSPHERE_TREE *)POV_MALLOC(sizeof(BSPHERE_TREE), "blob bounding hierarchy");

    Elements[i]->Entries = 0;
    Elements[i]->Node    = (BSPHERE_TREE **)&Blob->Data->Entry[i];

    get_element_bounding_sphere(&Blob->Data->Entry[i], Elements[i]->C, &Elements[i]->r2);
  }

  Build_Bounding_Sphere_Hierarchy(&Blob->Data->Tree, nElem, Elements);

  /* Get rid of the Elements array. */

  POV_FREE(Elements);
}



/*****************************************************************************
*
* FUNCTION
*
*   Determine_Blob_Textures
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Determine the textures and weights of all components affecting
*   the given intersection point. The weights are calculated from
*   the field values and sum to 1.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
*   Mar 1996 : Make the call to resize the textures/weights list just once
*              at the beginning instead of doing it for every element. [DB]
*
******************************************************************************/

void Determine_Blob_Textures(BLOB *Blob, VECTOR IPoint, int *Count, TEXTURE **Textures, DBL *Weights)
{
  int i;
  int size;
  DBL rad2, sum;
  VECTOR V1, P;
  BLOB_ELEMENT *Element;
  BSPHERE_TREE *Tree;

  /* Make sure we have enough room in the textures/weights list. */

  Reinitialize_Lighting_Code(Blob->Data->Number_Of_Components, &Textures, &Weights);

  /* Transform the point into the blob space. */

  if (Blob->Trans != NULL)
  {
    MInvTransPoint(P, IPoint, Blob->Trans);
  }
  else
  {
    Assign_Vector(P, IPoint);
  }

  *Count = 0;

  if (Blob->Data->Tree == NULL)
  {
    /* There's no tree --> step through all elements. */

    for (i = 0; i < Blob->Data->Number_Of_Components; i++)
    {
      Element = &Blob->Data->Entry[i];

      determine_element_texture(Blob, Element, Blob->Element_Texture[i], P, Count, Textures, Weights);
    }
  }
  else
  {
    /* A tree exists --> step through the tree. */

    size = 0;

    Queue[size++] = Blob->Data->Tree;

    while (size > 0)
    {
      Tree = Queue[--size];

      /* Test if current node is a leaf. */

      if (Tree->Entries <= 0)
      {
        determine_element_texture(Blob, (BLOB_ELEMENT *)Tree->Node, Blob->Element_Texture[((BLOB_ELEMENT *)Tree->Node)->index], P, Count, Textures, Weights);
      }
      else
      {
        /* Test all sub-nodes. */

        for (i = 0; i < (int)Tree->Entries; i++)
        {
          /* Insert sub-node if we are inside. */

          VSub(V1, P, Tree->Node[i]->C);

          VDot(rad2, V1, V1);

          if (rad2 <= Tree->Node[i]->r2)
          {
            insert_node(Tree->Node[i], &size);
          }
        }
      }
    }
  }

  /* Normalize weights so that their sum is 1. */

  if (*Count > 0)
  {
    sum = 0.0;

    for (i = 0; i < *Count; i++)
    {
      sum += Weights[i];
    }

    if (sum > 0.0)
    {
      for (i = 0; i < *Count; i++)
      {
        Weights[i] /= sum;
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   determine_element_texture
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   If the intersection point is inside the component calculate
*   the field density and store the element's texture and the field
*   value in the texture/weight list.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

static void determine_element_texture(BLOB *Blob, BLOB_ELEMENT *Element, TEXTURE *Texture, VECTOR P, int *Count, TEXTURE  **Textures, DBL *Weights)
{
  int i;
  DBL density;

  density = fabs(calculate_element_field(Element, P));

  if (density > 0.0)
  {
    if (Texture == NULL)
    {
      Textures[*Count] = Blob->Texture;
    }
    else
    {
      Textures[*Count] = Texture;
    }

    /* Test if this texture is already used. */

    for (i = 0; i < *Count; i++)
    {
      if (Textures[i] == Textures[*Count])
      {
        /* Add current weight to already existing texture weight. */

        Weights[i] += density;

        /* Any texture can only be in the list once --> exit. */

        return;
      }
    }

    Weights[(*Count)++] = density;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Blob_Element
*
* INPUT
*
*   Element - Pointer to blob element
*   Vector  - Translation vector
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
*   Translate a blob element.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

void Translate_Blob_Element(BLOB_ELEMENT *Element, VECTOR Vector)
{
  TRANSFORM Trans;

  Compute_Translation_Transform(&Trans, Vector);

  if (Element->Trans == NULL)
  {
    /* This is a sphere component. */

    VAddEq(Element->O, Vector);
  }
  else
  {
    /* This is one of the other components. */

    Transform_Blob_Element(Element, &Trans);
  }

  Transform_Textures(Element->Texture, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Blob_Element
*
* INPUT
*
*   Element - Pointer to blob element
*   Vector  - Translation vector
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
*   Rotate a blob element.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

void Rotate_Blob_Element(BLOB_ELEMENT *Element, VECTOR Vector)
{
  TRANSFORM Trans;

  Compute_Rotation_Transform(&Trans, Vector);

  if (Element->Trans == NULL)
  {
    /* This is a sphere component. */

    MTransPoint(Element->O, Element->O, &Trans);
  }
  else
  {
    /* This is one of the other components. */

    Transform_Blob_Element(Element, &Trans);
  }

  Transform_Textures(Element->Texture, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Blob_Element
*
* INPUT
*
*   Element - Pointer to blob element
*   Vector  - Translation vector
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
*   Scale a blob element.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

void Scale_Blob_Element(BLOB_ELEMENT *Element, VECTOR Vector)
{
  TRANSFORM Trans;

  if ((Vector[X] != Vector[Y]) || (Vector[X] != Vector[Z]))
  {
    if (Element->Trans == NULL)
    {
      /* This is a sphere component --> change to ellipsoid component. */

      Element->Type = BLOB_ELLIPSOID;

      Element->Trans = Create_Transform();
    }
  }

  Compute_Scaling_Transform(&Trans, Vector);

  if (Element->Trans == NULL)
  {
    /* This is a sphere component. */

    VScaleEq(Element->O, Vector[X]);

    Element->rad2 *= Sqr(Vector[X]);
  }
  else
  {
    /* This is one of the other components. */

    Transform_Blob_Element(Element, &Trans);
  }

  Transform_Textures(Element->Texture, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Blob_Element
*
* INPUT
*
*   Element - Pointer to blob element
*   Trans   - Transformation
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
*   Transform a blob element.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

void Transform_Blob_Element(BLOB_ELEMENT *Element, TRANSFORM *Trans)
{
  if (Element->Trans == NULL)
  {
    /* This is a sphere component --> change to ellipsoid component. */

    Element->Type = BLOB_ELLIPSOID;

    Element->Trans = Create_Transform();
  }

  Compose_Transforms(Element->Trans, Trans);

  Transform_Textures(Element->Texture, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Blob_Element
*
* INPUT
*
*   Element - Pointer to blob element
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
*   Invert blob element by negating its strength.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
******************************************************************************/

void Invert_Blob_Element(BLOB_ELEMENT *Element)
{
  Element->c[2] *= -1.0;
}



/*****************************************************************************
*
* FUNCTION
*
*   Init_Blob_Queue
*
* INPUT
*
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Init queues for blob intersections.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Init_Blob_Queue()
{
  Queue = (BSPHERE_TREE **)POV_MALLOC(Max_Queue_Size*sizeof(BSPHERE_TREE *), "blob queue");
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Blob_Queue
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Destroy queues for blob intersections.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Destroy_Blob_Queue()
{
  if (Queue != NULL)
  {
    POV_FREE(Queue);
  }

  Queue = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   insert_node
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Insert a node into the node queue.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

static void insert_node(BSPHERE_TREE *Node, int *size)
{
  /* Resize queue if necessary. */

  if (*size >= (int)Max_Queue_Size)
  {
    if (Max_Queue_Size >= INT_MAX/2)
    {
      Error("Blob queue overflow!\n");
    }

    Max_Queue_Size *= 2;

    Queue = (BSPHERE_TREE **)POV_REALLOC(Queue, Max_Queue_Size*sizeof(BSPHERE_TREE *), "blob queue");
  }

  Queue[(*size)++] = Node;
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Blob_Element_Texture_List
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a list of all textures in the blob.
*
*   The list actually contains copies of the textures not
*   just references to them.
*
* CHANGES
*
*   Mar 1996 : Created.
*
******************************************************************************/

void Create_Blob_Element_Texture_List(BLOB *Blob, BLOB_LIST *BlobList, int npoints)
{
  int i, element_count, count;
  BLOB_LIST *temp;

  if (npoints < 1)
  {
    Error("Need at least one component in a blob.");
  }

  /* Figure out how many components there will be. */

  temp = BlobList;

  for (i = count = 0; i < npoints; i++)
  {
    if (temp->elem.Type & BLOB_CYLINDER)
    {
      count += 3;
    }
    else
    {
      count++;
    }

    temp = temp->next;

    /* Test for too many components. [DB 12/94] */

    if (count >= MAX_BLOB_COMPONENTS)
    {
      Error("There are more than %d components in a blob.\n", MAX_BLOB_COMPONENTS);
    }
  }

  /* Allocate memory for components. */

  Blob->Data = (BLOB_DATA *)POV_MALLOC(sizeof(BLOB_DATA), "blob data");

  Blob->Data->References = 1;

  Blob->Data->Number_Of_Components = count;

  Blob->Data->Entry = (BLOB_ELEMENT *)POV_MALLOC(count*sizeof(BLOB_ELEMENT), "blob data");

  Blob->Data->Intervals = (BLOB_INTERVAL *)POV_MALLOC(2*Blob->Data->Number_Of_Components*sizeof(BLOB_INTERVAL), "blob intervals");

  Blob->Data->Tree = NULL;

  /* Init components. */

  for (i = 0; i < count; i++)
  {
    init_blob_element(&Blob->Data->Entry[i]);
  }

  /* Allocate memory for list. */

  Blob->Element_Texture = (TEXTURE **)POV_MALLOC(count*sizeof(TEXTURE *), "blob texture list");

  for (i = 0; i < count; i++)
  {
    Blob->Element_Texture[i] = NULL;
  }

  for (i = element_count = 0; i < npoints; i++)
  {
    temp = BlobList;

    /* Copy textures. */

    switch (temp->elem.Type)
    {
      case BLOB_ELLIPSOID :
      case BLOB_SPHERE :

        /*
         * Copy texture into element texture list. This is neccessary
         * because individual textures have to be transformed too if
         * copies of the blob are transformed.
         */

        Blob->Element_Texture[element_count++] = Copy_Textures(temp->elem.Texture);

        break;

      case BLOB_CYLINDER :

        /*
         * Copy texture into element texture list. This is neccessary
         * because individual textures have to be transformed too if
         * copies of the blob are transformed.
         */

        Blob->Element_Texture[element_count++] = Copy_Textures(temp->elem.Texture);

        Blob->Element_Texture[element_count++] = Copy_Textures(temp->elem.Texture);

        Blob->Element_Texture[element_count++] = Copy_Textures(temp->elem.Texture);

        break;
    }

    BlobList = BlobList->next;
  }
}



