/*****************************************************************************
*                bbcyl.c
*
*  This file contains all functions for bounding
*  cylinders used by lathe and sor objects.
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
#include "bcyl.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int  intersect_thick_cylinder (BCYL *BCyl, BCYL_ENTRY *Entry, DBL *dist);
static void insert_hit (BCYL_INT *Element, BCYL_INT *intervals, int *cnt);
static void intersect_bound_elements (BCYL *BCyl, VECTOR P, VECTOR D);


/*****************************************************************************
* Local variables
******************************************************************************/



/*****************************************************************************
*
* FUNCTION
*
*   intersect_thick_cylinder
*
* INPUT
*
*   BCyl - Pointer to lathe structure
*   Entry - Segment whos bounding cylinder to intersect
*   dist  - List of sorted intersection depths
*
* OUTPUT
*
* RETURNS
*
*   int - number of intersections found
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Find all intersections of the current ray with the bounding
*   cylinder of the given segment. The intersection tests are
*   done in intersect_bound_elements() and the results stored
*   in the lathe structure are evaluated here.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

static int intersect_thick_cylinder(BCYL *BCyl, BCYL_ENTRY *Entry, DBL *dist)
{
  int i, j, n;
  DBL k, r, h;

  n = 0;

  /* Intersect ray with the cap-plane. */

  if (BCyl->hint[Entry->h2].n)
  {
    r = BCyl->hint[Entry->h2].w[0];

    if ((r >= BCyl->radius[Entry->r1]) &&
        (r <= BCyl->radius[Entry->r2]))
    {
      dist[n++] = BCyl->hint[Entry->h2].d[0];
    }
  }

  /* Intersect ray with the base-plane. */

  if (BCyl->hint[Entry->h1].n)
  {
    r = BCyl->hint[Entry->h1].w[0];

    if ((r >= BCyl->radius[Entry->r1]) &&
        (r <= BCyl->radius[Entry->r2]))
    {
      dist[n++] = BCyl->hint[Entry->h1].d[0];
    }
  }

  /* Intersect with inner cylinder. */

  if (BCyl->rint[Entry->r1].n)
  {
    h = BCyl->rint[Entry->r1].w[0];

    if ((h >= BCyl->height[Entry->h1]) &&
        (h <= BCyl->height[Entry->h2]))
    {
      dist[n++] = BCyl->rint[Entry->r1].d[0];
    }

    h = BCyl->rint[Entry->r1].w[1];

    if ((h >= BCyl->height[Entry->h1]) &&
        (h <= BCyl->height[Entry->h2]))
    {
      dist[n++] = BCyl->rint[Entry->r1].d[1];
    }
  }

  /* Intersect with outer cylinder. */

  if (BCyl->rint[Entry->r2].n)
  {
    h = BCyl->rint[Entry->r2].w[0];

    if ((h >= BCyl->height[Entry->h1]) &&
        (h <= BCyl->height[Entry->h2]))
    {
      dist[n++] = BCyl->rint[Entry->r2].d[0];
    }

    h = BCyl->rint[Entry->r2].w[1];

    if ((h >= BCyl->height[Entry->h1]) &&
        (h <= BCyl->height[Entry->h2]))
    {
      dist[n++] = BCyl->rint[Entry->r2].d[1];
    }
  }

  /* Sort intersections. */

  for (i = 0; i < n; i++)
  {
    for (j = 0; j < n - i - 1; j++)
    {
      if (dist[j] > dist[j+1])
      {
        k         = dist[j];
        dist[j]   = dist[j+1];
        dist[j+1] = k;
      }
    }
  }

  return(n);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_bound_elements
*
* INPUT
*
*   BCyl - Pointer to lathe structure
*   P, D  - Current ray
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
*   Intersect all bounding discs and cylinders and store
*   the intersections found in the lathe structure.
*
*   By intersecting all different discs and cylinders once
*   we avoid testing the same cylinders and discs more than
*   once. This happened when we tested one bounding cylinder
*   after the other.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

static void intersect_bound_elements(BCYL *BCyl, VECTOR P, VECTOR  D)
{
  int i;
  DBL a, b, bb, b2, c, d, k;

  /* Init constants. */

  a = D[X] * D[X] + D[Z] * D[Z];

  b = P[X] * D[X] + P[Z] * D[Z];

  bb = b * b;

  b2 = 2.0 * b;

  c = P[X] * P[X] + P[Z] * P[Z];

  /* Intersect all rings. */

  if ((D[Y] < -EPSILON) || (D[Y] > EPSILON))
  {
    for (i = 0; i < BCyl->nheight; i++)
    {
      k = (BCyl->height[i] - P[Y]) / D[Y];

      BCyl->hint[i].n = 1;

      BCyl->hint[i].d[0] = k;

      BCyl->hint[i].w[0] = k * (a * k + b2) + c;
    }
  }
  else
  {
    for (i = 0; i < BCyl->nheight; i++)
    {
      BCyl->hint[i].n = 0;
    }
  }

  /* Intersect all cylinders. */

  for (i = 0; i < BCyl->nradius; i++)
  {
    BCyl->rint[i].n = 0;

    if (BCyl->radius[i] > EPSILON)
    {
      d = bb - a * (c - BCyl->radius[i]);

      if (d > 0.0)
      {
        d = sqrt(d);

        k = (-b + d) / a;

        BCyl->rint[i].n = 2;

        BCyl->rint[i].d[0] = k;

        BCyl->rint[i].w[0] = P[Y] + k * D[Y];

        k = (-b - d) / a;

        BCyl->rint[i].d[1] = k;

        BCyl->rint[i].w[1] = P[Y] + k * D[Y];
      }
    }
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   insert_hit
*
* INPUT
*
*   element   - Intersection to insert
*   intervals - List of intervals
*   cnt       - Number of elements in the list
*
* OUTPUT
*
*   intervals, cnt
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Insert an intersection into the depth sorted intersection list.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

static void insert_hit(BCYL_INT *element, BCYL_INT  *intervals, int *cnt)
{
  int k;

  intervals[*cnt] = *element;

  for (k = 0; element->d[0] > intervals[k].d[0]; k++);

  if (k < *cnt)
  {
    POV_MEMMOVE(&intervals[k+1], &intervals[k], (*cnt-k)*sizeof(BCYL_INT));

    intervals[k] = *element;
  }

  (*cnt)++;
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_All_Bounds
*
* INPUT
*
*   BCyl     - Pointer to lathe structure
*   P, D      - Current ray
*   intervals - List of intervals
*   cnt       - Number of elements in the list
*
* OUTPUT
*
*   intervals, cnt
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Intersect given ray with all bounding cylinders of the given lathe
*   and return a sorted list of intersection depths and segments hit.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

int Intersect_BCyl(BCYL *BCyl, VECTOR P, VECTOR  D)
{
  int i, cnt;
  DBL dist[8];
  BCYL_INT Inter;
  BCYL_INT *intervals;
  BCYL_ENTRY *Entry;

  cnt = 0;

  Inter.d[1] = 0.0;

  /* Intersect all cylinder and plane elements. */

  intersect_bound_elements(BCyl, P, D);

  /* Intersect all spline segments. */

  intervals = BCyl->intervals;

  for (i = 0; i < BCyl->number; i++)
  {
    Entry = &BCyl->entry[i];

    switch (intersect_thick_cylinder(BCyl, Entry, dist))
    {
      case 0:
        break;

      case 2:

        if (dist[0] > EPSILON)
        {
          Inter.d[0] = dist[0];
          Inter.n    = i;

          insert_hit(&Inter, intervals, &cnt);
        }
        else
        {
          if (dist[1] > EPSILON)
          {
            Inter.d[0] = 0.0;
            Inter.n    = i;

            insert_hit(&Inter, intervals, &cnt);
          }
        }

        break;

      case 4:

        if (dist[0] > EPSILON)
        {
          Inter.d[0] = dist[0];
          Inter.n    = i;

          insert_hit(&Inter, intervals, &cnt);
        }
        else
        {
          if (dist[1] > EPSILON)
          {
            Inter.d[0] = 0.0;
            Inter.n    = i;

            insert_hit(&Inter, intervals, &cnt);
          }
          else
          {
            if (dist[2] > EPSILON)
            {
              Inter.d[0] = dist[2];
              Inter.n    = i;

              insert_hit(&Inter, intervals, &cnt);
            }
            else
            {
              if (dist[3] > EPSILON)
              {
                Inter.d[0] = 0.0;
                Inter.n    = i;

                insert_hit(&Inter, intervals, &cnt);
              }
            }
          }
        }

        break;

      default:

        /*
         * We weren't able to find an even number of intersections. Thus
         * we can't tell where the ray enters and leaves the bounding
         * cylinder. To avoid problems we assume that the ray is always
         * inside the cylinder in that case.
         */

        Inter.d[0] = dist[0];
        Inter.n    = i;

        insert_hit(&Inter, intervals, &cnt);

        break;
    }
  }

  return(cnt);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_BCyl
*
* INPUT
*
*   number - number of cylindrical segments
*   r1, r2 - list of segment radii
*   h1, h2 - list of segment heights
*
* OUTPUT
*
* RETURNS
*
*   BCYL * - created bounding cylinder.
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a bounding cylinder data structure from the given
*   radii and heights.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

BCYL *Create_BCyl(int number, DBL *tmp_r1, DBL  *tmp_r2, DBL  *tmp_h1, DBL  *tmp_h2)
{
  int i, j, nr, nh;
  int *tmp_r1_index;
  int *tmp_r2_index;
  int *tmp_h1_index;
  int *tmp_h2_index;
  DBL *tmp_radius;
  DBL *tmp_height;
  BCYL *bcyl;

  /* Allocate bounding cylinder. */

  bcyl = (BCYL *)POV_MALLOC(sizeof(BCYL), "bounding cylinder");

  /* Allocate entries. */

  bcyl->number = number;

  bcyl->entry = (BCYL_ENTRY *)POV_MALLOC(bcyl->number*sizeof(BCYL_ENTRY), "bounding cylinder data");

  /* Allocate intersection lists. */

  bcyl->hint = (BCYL_INT *)POV_MALLOC(2*bcyl->number*sizeof(BCYL_INT), "lathe intersection list");
  bcyl->rint = (BCYL_INT *)POV_MALLOC(2*bcyl->number*sizeof(BCYL_INT), "lathe intersection list");

  bcyl->intervals = (BCYL_INT *)POV_MALLOC(4*bcyl->number*sizeof(BCYL_INT), "lathe intersection list");

  /* Allocate temporary lists. */

  tmp_r1_index = (int *)POV_MALLOC(bcyl->number * sizeof(int), "temp lathe data");
  tmp_r2_index = (int *)POV_MALLOC(bcyl->number * sizeof(int), "temp lathe data");
  tmp_h1_index = (int *)POV_MALLOC(bcyl->number * sizeof(int), "temp lathe data");
  tmp_h2_index = (int *)POV_MALLOC(bcyl->number * sizeof(int), "temp lathe data");

  tmp_radius = (DBL *)POV_MALLOC(2 * bcyl->number * sizeof(DBL), "temp lathe data");
  tmp_height = (DBL *)POV_MALLOC(2 * bcyl->number * sizeof(DBL), "temp lathe data");

  /* Get different bounding radii and heights. */

  nr = 0;
  nh = 0;

  for (i = 0; i < bcyl->number; i++)
  {
    tmp_r1_index[i] = -1;
    tmp_r2_index[i] = -1;
    tmp_h1_index[i] = -1;
    tmp_h2_index[i] = -1;

    for (j = 0; j < nr; j++)
    {
      if (tmp_r1[i] == tmp_radius[j])
      {
        break;
      }
    }

    if (j == nr)
    {
      tmp_radius[nr++] = tmp_r1[i];
    }

    tmp_r1_index[i] = j;

    for (j = 0; j < nr; j++)
    {
      if (tmp_r2[i] == tmp_radius[j])
      {
        break;
      }
    }

    if (j == nr)
    {
      tmp_radius[nr++] = tmp_r2[i];
    }

    tmp_r2_index[i] = j;

    for (j = 0; j < nh; j++)
    {
      if (tmp_h1[i] == tmp_height[j])
      {
        break;
      }
    }

    if (j == nh)
    {
      tmp_height[nh++] = tmp_h1[i];
    }

    tmp_h1_index[i] = j;

    for (j = 0; j < nh; j++)
    {
      if (tmp_h2[i] == tmp_height[j])
      {
        break;
      }
    }

    if (j == nh)
    {
      tmp_height[nh++] = tmp_h2[i];
    }

    tmp_h2_index[i] = j;
  }

  /* Copy lists into the lathe. */

  bcyl->radius = (DBL *)POV_MALLOC(nr * sizeof(DBL), "bounding cylinder data");
  bcyl->height = (DBL *)POV_MALLOC(nh * sizeof(DBL), "bounding cylinder data");

  for (i = 0; i < nr; i++)
  {
    bcyl->radius[i] = Sqr(tmp_radius[i]);
  }

  for (i = 0; i < nh; i++)
  {
    bcyl->height[i] = tmp_height[i];
  }

  /* Assign height and radius indices. */

  bcyl->nradius = nr;
  bcyl->nheight = nh;

  for (i = 0; i < bcyl->number; i++)
  {
    bcyl->entry[i].r1 = tmp_r1_index[i];
    bcyl->entry[i].r2 = tmp_r2_index[i];
    bcyl->entry[i].h1 = tmp_h1_index[i];
    bcyl->entry[i].h2 = tmp_h2_index[i];
  }

/*
  fprintf(stderr, "number of different radii   = %d\n", nr);
  fprintf(stderr, "number of different heights = %d\n", nh);
*/

  /* Get rid of temp. memory. */

  POV_FREE(tmp_height);
  POV_FREE(tmp_radius);
  POV_FREE(tmp_h2_index);
  POV_FREE(tmp_h1_index);
  POV_FREE(tmp_r2_index);
  POV_FREE(tmp_r1_index);

  return(bcyl);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_BCyl
*
* INPUT
*
*   BCyl - bounding cylinder
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
*   Destroy a bounding cylinder.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

void Destroy_BCyl(BCYL *BCyl)
{
  POV_FREE(BCyl->entry);

  POV_FREE(BCyl->radius);

  POV_FREE(BCyl->height);

  POV_FREE(BCyl->rint);

  POV_FREE(BCyl->hint);

  POV_FREE(BCyl->intervals);

  POV_FREE(BCyl);
}



