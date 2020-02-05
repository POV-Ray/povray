/****************************************************************************
*                   media.c
*
*  This module contains all functions for participating media.
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
#include "chi2.h"
#include "colour.h"
#include "povray.h"
#include "texture.h"
#include "pigment.h"
#include "objects.h"
#include "lighting.h"
#include "matrices.h"
#include "media.h"
#include "pattern.h"
#include "povray.h"
#include "point.h"
#include "texture.h"
#include "ray.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/*****************************************************************************
* Local typedefs
******************************************************************************/

typedef struct Light_List_Struct LIGHT_LIST;
typedef struct Media_Interval_Struct MEDIA_INTERVAL;
typedef struct Lit_Interval_Struct LIT_INTERVAL;

struct Lit_Interval_Struct
{
  int lit;
  DBL s0, s1, ds;
};

struct Media_Interval_Struct
{
  int lit;
  int samples;
  DBL s0, s1, ds;
  COLOUR od;
  COLOUR te;
  COLOUR te2;
};

struct Light_List_Struct
{
  int active;
  DBL s0, s1;
  LIGHT_SOURCE *Light;
};



/*****************************************************************************
* Local variables
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static void sample_media (LIGHT_LIST *, RAY *, IMEDIA **, MEDIA_INTERVAL *, int );
static void get_light_list (LIGHT_LIST *, RAY *, INTERSECTION *);
static void get_lit_interval (int *, LIT_INTERVAL *, int, LIGHT_LIST *, INTERSECTION *);
static int set_up_sampling_intervals (MEDIA_INTERVAL *,
  int, LIT_INTERVAL *, IMEDIA *);

static int intersect_spotlight (RAY *Ray, LIGHT_SOURCE *Light, DBL *d1, DBL *d2);
static int intersect_cylinderlight (RAY *Ray, LIGHT_SOURCE *Light, DBL *d1, DBL *d2);

static int CDECL compdoubles (CONST void *in_a, CONST void *in_b);

static void evaluate_density_pattern (IMEDIA *, VECTOR , COLOUR);


/*****************************************************************************
*
* FUNCTION
*
*   Simulate_Media
*
* INPUT
*
*   Ray       - Current ray, start point P0
*   Inter     - Current intersection, end point P1
*   Colour    - Color emitted at P1 towards P0
*   light_ray - TRUE if we are looking at a light source ray
*
* OUTPUT
*
*   Colour    - Color arriving at the end point
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Simulate participating media using volume sampling.
*
*   The effects of participating media on the light emitted at P1
*   towards P0 are determined using Monte Carlo integration.
*
*   The effects include: emission, absoprtion and scattering.
*
*   Currently one global medium with constant coefficients is implemented.
*
*   Ideas for the atmospheric scattering were taken from:
*
*     - M. Inakage, "An Illumination Model for Atmospheric Environments", ..
*
*     - Nishita, T., Miyawaki, Y. and Nakamae, E., "A Shading Model for
*       Atmospheric Scattering Considering Luminous Intensity Distribution
*       of Light Sources", Computer Graphics, 21, 4, 1987, 303-310
*
* CHANGES
*
*   Nov 1994 : Creation.
*
*   Jan 1995 : Added support of cylindrical light sources. [DB]
*
*   Jun 1995 : Added code for alpha channel support. [DB]
*
******************************************************************************/

void Simulate_Media(IMEDIA **Media_List, RAY *Ray, INTERSECTION *Inter, COLOUR Colour, int light_ray)
{
  int i, j, intervals, use_extinction;
  int lit_interval_entries;
  DBL n;
  COLOUR Od, Te, Va;
  LIGHT_LIST *Light_List = NULL;
  LIT_INTERVAL *Lit_Interval;
  IMEDIA *IMedia, **Tmp, *Local;
  MEDIA_INTERVAL *Media_Interval, *curr;

  /* Why are we here? */

  if ((Media_List == NULL) || (Media_List[0] == NULL))
  {
    return;
  }

  /* Find media with the largest number of intervals. */

  intervals = 0;

  use_extinction = FALSE;

  IMedia = Media_List[0];

  for (Tmp = Media_List; (*Tmp) != NULL; Tmp++)
  {
    for (Local = *Tmp; Local != NULL; Local = Local->Next_Media)
    {
      if (Local->Intervals > intervals)
      {
        intervals = Local->Intervals;

        IMedia = Local;
      }

      use_extinction |= Local->use_extinction;
    }
  }

  /* If this is a light ray and no extinction is used we can return. */

  if ((light_ray) && (!use_extinction))
  {
    return;
  }

  /*
   * Prepare the Monte Carlo integration along the ray from P0 to P1.
   */

  if (light_ray || (Frame.Number_Of_Light_Sources==0))
  {
    Lit_Interval = (LIT_INTERVAL *)POV_MALLOC(sizeof(LIT_INTERVAL), "lit interval");

    lit_interval_entries = 1;

    Lit_Interval[0].lit = FALSE;

    Lit_Interval[0].s0 = 0.0;
    Lit_Interval[0].s1 =
    Lit_Interval[0].ds = Inter->Depth;
  }
  else
  {
    /* Get light list. */

    Light_List = (LIGHT_LIST *)POV_MALLOC(Frame.Number_Of_Light_Sources*sizeof(LIGHT_LIST), "light list");

    get_light_list(Light_List, Ray, Inter);

    /* Get lit intervals. */

    Lit_Interval = (LIT_INTERVAL *)POV_MALLOC((2*Frame.Number_Of_Light_Sources+1)*sizeof(LIT_INTERVAL), "lit interval");

    get_lit_interval(&lit_interval_entries, Lit_Interval, Frame.Number_Of_Light_Sources, Light_List, Inter);
  }

  /* Set up sampling intervals. */

  Media_Interval = (MEDIA_INTERVAL *)POV_MALLOC(IMedia->Intervals*sizeof(MEDIA_INTERVAL), "media intervals");

  intervals = set_up_sampling_intervals(Media_Interval, lit_interval_entries, Lit_Interval, IMedia);

  /* Sample all intervals. */

  for (i = 0; i < intervals; i++)
  {
    /* Sample current interval. */

    Increase_Counter(stats[Media_Intervals]);

    for (j = 0; j < IMedia->Min_Samples; j++)
    {
      sample_media(Light_List, Ray, Media_List, &Media_Interval[i], light_ray);
    }
  }

  /* Cast additional samples if necessary. */

  if ((!light_ray) && (IMedia->Max_Samples > IMedia->Min_Samples))
  {
    curr = &Media_Interval[0];

    for (i = 0; i < intervals; i++)
    {
      if (curr->samples < IMedia->Max_Samples)
      {
        /* Get variance of samples. */

        n = (DBL)curr->samples;

        Va[0] = (curr->te2[0] / n - Sqr(curr->te[0] / n)) / n;
        Va[1] = (curr->te2[1] / n - Sqr(curr->te[1] / n)) / n;
        Va[2] = (curr->te2[2] / n - Sqr(curr->te[2] / n)) / n;

        /* Take additional samples until variance is small enough. */

        while ((Va[0] >= IMedia->Sample_Threshold[curr->samples-1]) ||
               (Va[1] >= IMedia->Sample_Threshold[curr->samples-1]) ||
               (Va[2] >= IMedia->Sample_Threshold[curr->samples-1]))
        {
          /* Sample current interval again. */

          sample_media(Light_List, Ray, Media_List, curr, light_ray);

          /* Have we reached maximum number of samples. */

          if (curr->samples > IMedia->Max_Samples)
          {
            break;
          }

          /* Get variance of samples. */

          n = (DBL)curr->samples;

          Va[0] = (curr->te2[0] / n - Sqr(curr->te[0] / n)) / n;
          Va[1] = (curr->te2[1] / n - Sqr(curr->te[1] / n)) / n;
          Va[2] = (curr->te2[2] / n - Sqr(curr->te[2] / n)) / n;
        }
      }

      curr++;
    }
  }

  /* Sum the influences of all intervals. */

  Make_Colour(Od, 0.0, 0.0, 0.0);
  Make_Colour(Te, 0.0, 0.0, 0.0);

  curr = &Media_Interval[0];

  for (i = 0; i < intervals; i++)
  {
    /* Add total emission. */

    Te[0] += curr->te[0] / (DBL)curr->samples * exp(-Od[0]);
    Te[1] += curr->te[1] / (DBL)curr->samples * exp(-Od[1]);
    Te[2] += curr->te[2] / (DBL)curr->samples * exp(-Od[2]);

    /* Add optical depth of current interval. */

    Od[0] += curr->od[0] / (DBL)curr->samples;
    Od[1] += curr->od[1] / (DBL)curr->samples;
    Od[2] += curr->od[2] / (DBL)curr->samples;

    curr++;
  }

  /* Add contribution estimated for the participating media. */

  Colour[0] = Colour[0] * exp(-Od[0]) + Te[0];
  Colour[1] = Colour[1] * exp(-Od[1]) + Te[1];
  Colour[2] = Colour[2] * exp(-Od[2]) + Te[2];

  if (!(light_ray || (Frame.Number_Of_Light_Sources==0)))
  {
    POV_FREE(Light_List);
  }

  POV_FREE(Lit_Interval);

  POV_FREE(Media_Interval);
}



/*****************************************************************************
*
* FUNCTION
*
*   sample_media
*
* INPUT
*
*   dist  - distance of current sample
*   Ray   - pointer to ray
*   IMedia - pointer to media to use
*
* OUTPUT
*
*   Col          - color of current sample
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the color of the current media sample.
*
* CHANGES
*
*   Nov 1994 : Creation.
*
******************************************************************************/

static void sample_media(LIGHT_LIST *Light_List, RAY *Ray, IMEDIA **Media_List, MEDIA_INTERVAL *Interval, int light_ray)
{
  int i, n, use_scattering;
  DBL alpha, d0, d1, len, k, g, g2;
  VECTOR P, H;
  COLOUR C0, Light_Colour, Te;
  COLOUR Em, Ex, Sc;
  RAY Light_Ray;
  IMEDIA **Tmp, *Local;

  Increase_Counter(stats[Media_Samples]);

  /* Set up sampling location. */

  d0 = Interval->ds * FRAND();

  d1 = Interval->s0 + d0;

  VEvaluateRay(H, Ray->Initial, d1, Ray->Direction);

  /* Get coefficients in current sample location. */

  Make_Colour(Em, 0.0, 0.0, 0.0);
  Make_Colour(Ex, 0.0, 0.0, 0.0);
  Make_Colour(Sc, 0.0, 0.0, 0.0);

  use_scattering = FALSE;

  for (Tmp = Media_List; (*Tmp) != NULL; Tmp++)
  {
    for (Local = *Tmp; Local != NULL; Local = Local->Next_Media)
    {
      Assign_Vector(P, H);

      evaluate_density_pattern(Local, P, C0);

      Ex[0] += C0[0] * Local->Extinction[0];
      Ex[1] += C0[1] * Local->Extinction[1];
      Ex[2] += C0[2] * Local->Extinction[2];

      if (!light_ray)
      {
        Em[0] += C0[0] * Local->Emission[0];
        Em[1] += C0[1] * Local->Emission[1];
        Em[2] += C0[2] * Local->Emission[2];

        Sc[0] += C0[0] * Local->Scattering[0];
        Sc[1] += C0[1] * Local->Scattering[1];
        Sc[2] += C0[2] * Local->Scattering[2];
      }

      use_scattering |= Local->use_scattering;
    }
  }

  /* Get estimate for the total optical depth of the current interval. */

  Interval->od[0] += Ex[0] * Interval->ds;
  Interval->od[1] += Ex[1] * Interval->ds;
  Interval->od[2] += Ex[2] * Interval->ds;

  /* Get estimate for the total emission of the current interval. */

  Te[0] = Em[0];
  Te[1] = Em[1];
  Te[2] = Em[2];

  if ((!light_ray) && (use_scattering) && (Interval->lit))
  {
    /* Process all light sources. */

    for (i = 0; i < Frame.Number_Of_Light_Sources; i++)
    {
      /* Use light only if active and within it's boundaries. */

      if ((Light_List[i].active) && (d1 >= Light_List[i].s0) && (d1 <= Light_List[i].s1))
      {
        if (!(Test_Shadow(Light_List[i].Light, &len, &Light_Ray, Ray, P, Light_Colour)))
        {
          /* Get attenuation due to scattering. */

          k = 0.0;

          for (n = 0, Tmp = Media_List; (*Tmp) != NULL; n++, Tmp++)
          {
            for (Local = *Tmp; Local != NULL; Local = Local->Next_Media)
            {
              switch (Local->Type)
              {
                case RAYLEIGH_SCATTERING:

                  VDot(alpha, Light_Ray.Direction, Ray->Direction);

                  k += 0.799372013 * (1.0 + Sqr(alpha));

                  break;

                case MIE_HAZY_SCATTERING:

                  VDot(alpha, Light_Ray.Direction, Ray->Direction);

                  k += 0.576655375 * (1.0 + 9.0 * pow(0.5 * (1.0 + alpha), 8.0));

                  break;

                case MIE_MURKY_SCATTERING:

                  VDot(alpha, Light_Ray.Direction, Ray->Direction);

                  k += 0.495714547 * (1.0 + 50.0 * pow(0.5 * (1.0 + alpha), 32.0));

                  break;

                case HENYEY_GREENSTEIN_SCATTERING:

                  VDot(alpha, Light_Ray.Direction, Ray->Direction);

                  g = Local->Eccentricity;

                  g2 = Sqr(g);

                  k += (1.0 - g2) / pow(1.0 + g2 - 2.0 * g * alpha, 1.5);

                  break;

                case ISOTROPIC_SCATTERING:
                default:

                  k += 1.0;

                break;
              }
            }
          }

          k /= (DBL)n;

          Te[0] += k * Sc[0] * Light_Colour[0];
          Te[1] += k * Sc[1] * Light_Colour[1];
          Te[2] += k * Sc[2] * Light_Colour[2];
        }
      }
    }
  }

  Te[0] *= Interval->ds * exp(-Ex[0] * d0);
  Te[1] *= Interval->ds * exp(-Ex[1] * d0);
  Te[2] *= Interval->ds * exp(-Ex[2] * d0);

  /* Add emission. */

  Interval->te[0] += Te[0];
  Interval->te[1] += Te[1];
  Interval->te[2] += Te[2];

  Interval->te2[0] += Sqr(Te[0]);
  Interval->te2[1] += Sqr(Te[1]);
  Interval->te2[2] += Sqr(Te[2]);

  Interval->samples++;
}



/*****************************************************************************
*
* FUNCTION
*
*   evaluate_density_pattern
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
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

static void evaluate_density_pattern(IMEDIA *IMedia, VECTOR P, COLOUR C)
{
  COLOUR Local_Color;
  PIGMENT *Temp=IMedia->Density;
  
  Make_Colour (C, 1.0, 1.0, 1.0);

  while (Temp != NULL)
  {
    Make_Colour (Local_Color, 0.0, 0.0, 0.0);
    
    Compute_Pigment (Local_Color, Temp, P);

    C[0] *= Local_Color[0];
    C[1] *= Local_Color[1];
    C[2] *= Local_Color[2];

    Temp=(PIGMENT *)Temp->Next;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   get_light_list
*
* INPUT
*
*   Light_List - array containing light source information
*   dist       - distance of current sample
*   Ray        - pointer to ray
*   IMedia      - pointer to media to use
*
* OUTPUT
*
*   Col        - color of current sample
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
* CHANGES
*
*   Nov 1994 : Creation.
*
******************************************************************************/

static void get_light_list(LIGHT_LIST *Light_List, RAY *Ray, INTERSECTION *Inter)
{
  int i, insert;
  DBL t1, t2;
  LIGHT_SOURCE *Light;

  /* Get depths for all light sources and disconnected sampling intervals. */

  t1 = t2 = 0.0;

  for (i = 0, Light = Frame.Light_Sources; Light != NULL; Light = Light->Next_Light_Source, i++)
  {
    /* Init interval. */

    Light_List[i].active = FALSE;
    Light_List[i].s0     = 0.0;
    Light_List[i].s1     = Max_Distance;
    Light_List[i].Light  = NULL;

    insert = FALSE;

    Light_List[i].Light = Light;

    if (!Light->Media_Interaction)
    {
      continue;
    }

    switch (Light->Light_Type)
    {
      case CYLINDER_SOURCE:

        if (intersect_cylinderlight(Ray, Light, &t1, &t2))
        {
          if ((t1 < Inter->Depth) && (t2 > Small_Tolerance))
          {
            insert = TRUE;
          }
        }

        break;

      case POINT_SOURCE:

        t1 = 0.0;
        t2 = Inter->Depth;

        insert = TRUE;

        break;

      case SPOT_SOURCE:

        if (intersect_spotlight(Ray, Light, &t1, &t2))
        {
          if ((t1 < Inter->Depth) && (t2 > Small_Tolerance))
          {
            insert = TRUE;
          }
        }

        break;
    }

    /* Insert distances into sampling interval list. */

    if (insert)
    {
      /* Insert light source intersections into light list. */

      t1 = max(t1, 0.0);
      t2 = min(t2, Inter->Depth);

      Light_List[i].active = TRUE;
      Light_List[i].s0 = t1;
      Light_List[i].s1 = t2;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   get_lit_interval
*
* INPUT
*
*   Light_List - array containing light source information
*   dist       - distance of current sample
*   Ray        - pointer to ray
*   IMedia      - pointer to media to use
*
* OUTPUT
*
*   Col        - color of current sample
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
* CHANGES
*
*   Nov 1994 : Creation.
*
******************************************************************************/

static void get_lit_interval(int *number, LIT_INTERVAL *Lit_Interval, int entries, LIGHT_LIST *Light_List, INTERSECTION *Inter)
{
  int a, i, n;
  DBL *s0, *s1;
  LIT_INTERVAL *curr, *prev;

  s0 = (DBL *)POV_MALLOC(Frame.Number_Of_Light_Sources*sizeof(DBL), "temp data");
  s1 = (DBL *)POV_MALLOC(Frame.Number_Of_Light_Sources*sizeof(DBL), "temp data");

  for (i = a = 0; i < entries; i++)
  {
    if (Light_List[i].active)
    {
      s0[a] = Light_List[i].s0;
      s1[a] = Light_List[i].s1;

      a++;
    }
  }

  n = 0;

  curr = &Lit_Interval[0];

  if (a)
  {
    QSORT((void *)(&s0[0]), (unsigned long)a, sizeof(DBL), compdoubles);
    QSORT((void *)(&s1[0]), (unsigned long)a, sizeof(DBL), compdoubles);

    if (s0[0] > 0.0)
    {
      curr->lit = FALSE;

      curr->s0 = 0.0;
      curr->s1 = s0[0];

      curr++;

      n++;
    }

    curr->lit = TRUE;

    curr->s0 = s0[0];
    curr->s1 = s1[0];

    prev = curr;

    curr++;

    n++;

    for (i = 1; i < a; i++)
    {
      if (s0[i] > prev->s1)
      {
        curr->lit = FALSE;

        curr->s0 = prev->s1;
        curr->s1 = s0[i];

        prev++;
        curr++;

        n++;

        curr->lit = TRUE;

        curr->s0 = s0[i];
        curr->s1 = s1[i];

        prev++;
        curr++;

        n++;
      }
      else
      {
        if (s1[i] > prev->s1)
        {
          prev->s1 = s1[i];
        }
      }
    }

    if (prev->s1 < Inter->Depth)
    {
      curr->lit = FALSE;

      curr->s0 = prev->s1;
      curr->s1 = Inter->Depth;

      curr++;

      n++;
    }
  }
  else
  {
    curr->lit = FALSE;

    curr->s0 = 0.0;
    curr->s1 = Inter->Depth;

    curr++;

    n++;
  }

  curr = &Lit_Interval[0];

  for (i = 0; i < n; i++)
  {
    curr->ds = curr->s1 - curr->s0;

    curr++;
  }

  POV_FREE(s0);
  POV_FREE(s1);

  *number = n;
}



/*****************************************************************************
*
* FUNCTION
*
*   set_up_sampling_intervals
*
* INPUT
*
*   interval - array containing media intervals
*   number   - number of lit intervals
*   list     - array of lit intervals
*   media    - media to use
*
* OUTPUT
*
*   interval - array containing media intervals
*
* RETURNS
*
*   int      - number of media intervals created
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Distribute samples along an interval according to the maximum
*   number of samples and the ratio of samples in lit and unlit
*   areas as given by the participating media.
*
* CHANGES
*
*   Nov 1994 : Creation.
*
******************************************************************************/

static int set_up_sampling_intervals(MEDIA_INTERVAL *interval, int number, LIT_INTERVAL *list, IMEDIA *media)
{
  int i, j, n, r, remaining, intervals;
  DBL delta, sum, weight;
  MEDIA_INTERVAL *curr;
  LIT_INTERVAL *entry;

  /* Set up sampling intervals. */

  intervals = media->Intervals;

  /* Use one interval if no lit intervals and constant media. */

  if ((number == 0) && (media->is_constant))
  {
    intervals = 1;

    delta = list[0].ds;

    curr = interval;

    curr->lit = TRUE;

    curr->samples = 0;

    curr->s0 = list[0].s0;
    curr->s1 = list[0].s0 + delta;
    curr->ds = delta;

    Make_Colour(curr->od,  0.0, 0.0, 0.0);
    Make_Colour(curr->te,  0.0, 0.0, 0.0);
    Make_Colour(curr->te2, 0.0, 0.0, 0.0);

    return(intervals);
  }

  /* Choose intervals. */

  if (number == 1)
  {
    /* Use uniform intervals. */

    delta = list[0].ds / (DBL)intervals;

    curr = interval;

    for (i = 0; i < intervals; i++)
    {
      curr->lit = TRUE;

      curr->samples = 0;

      curr->s0 = list[0].s0 + delta * (DBL)i;
      curr->s1 = list[0].s0 + delta * (DBL)(i + 1);
      curr->ds = delta;

      Make_Colour(curr->od,  0.0, 0.0, 0.0);
      Make_Colour(curr->te,  0.0, 0.0, 0.0);
      Make_Colour(curr->te2, 0.0, 0.0, 0.0);

      curr++;
    }
  }
  else
  {
    /* Choose intervals according to the specified ratio. */

    if (number > intervals)
    {
      Error("Too few sampling intervals.\n");
    }

    sum = 0.0;

    entry = list;

    for (i = 0; i < number; i++)
    {
/*
      sum += ((entry->lit) ? (0.9) : (0.1)) * entry->ds;
*/
      sum += ((entry->lit) ? (media->Ratio) : (1.0 - media->Ratio));

      entry++;
    }

    remaining = intervals;

    curr = interval;

    entry = list;

    for (i = 0; i < number; i++)
    {
/*
      weight = ((entry->lit) ? (0.9) : (0.1)) * entry->ds;
*/
      weight = ((entry->lit) ? (media->Ratio) : (1.0 - media->Ratio));

      n = (int)(weight / sum * (DBL)intervals) + 1;

      r = remaining - number + i + 1;

      if (n > r)
      {
        n = r;
      }

      delta = entry->ds / (DBL)n;

      for (j = 0; j < n; j++)
      {
        curr->lit = entry->lit;

        curr->samples = 0;

        curr->s0 = entry->s0 + delta * (DBL)j;
        curr->s1 = entry->s0 + delta * (DBL)(j + 1);
        curr->ds = delta;

        Make_Colour(curr->od,  0.0, 0.0, 0.0);
        Make_Colour(curr->te,  0.0, 0.0, 0.0);
        Make_Colour(curr->te2, 0.0, 0.0, 0.0);

        curr++;
      }

      remaining -= n;

      entry++;
    }
  }

  return(intervals);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_spotlight
*
* INPUT
*
*   Ray    - current ray
*   Light  - current light source
*
* OUTPUT
*
*   d1, d2 - intersection depths
*
* RETURNS
*
*   int - TRUE, if hit
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Intersect a ray with the light cone of a spotlight.
*
* CHANGES
*
*   Nov 1994 : Creation.
*
******************************************************************************/

static int intersect_spotlight(RAY *Ray, LIGHT_SOURCE *Light, DBL *d1, DBL  *d2)
{
  int viewpoint_is_in_cone;
  DBL a, b, c, d, m, l, l1, l2, t, t1, t2, k1, k2, k3, k4;
  VECTOR V1;

  /* Get cone's slope. Note that cos(falloff) is stored in Falloff! */

  a = acos(Light->Falloff);

  /* This only works for a < 180 degrees! */

  m = tan(a);

  m = 1.0 + Sqr(m);

  VSub(V1, Ray->Initial, Light->Center);

  VDot(k1, Ray->Direction, Light->Direction);

  VDot(k2, V1, Light->Direction);

  VLength(l, V1);

  if (l > EPSILON)
  {
    viewpoint_is_in_cone = (k2 / l >= Light->Falloff);
  }
  else
  {
    viewpoint_is_in_cone = FALSE;
  }

  if ((k1 <= 0.0) && (k2 < 0.0))
  {
    return (FALSE);
  }

  VDot(k3, V1, Ray->Direction);

  VDot(k4, V1, V1);

  a = 1.0 - Sqr(k1) * m;

  b = k3 - k1 * k2 * m;

  c = k4 - Sqr(k2) * m;

  if (a != 0.0)
  {
    d = Sqr(b) - a * c;

    if (d > EPSILON)
    {
      d = sqrt(d);

      t1 = (-b + d) / a;
      t2 = (-b - d) / a;

      if (t1 > t2)
      {
        t = t1; t1 = t2; t2 = t;
      }

      l1 = k2 + t1 * k1;
      l2 = k2 + t2 * k1;

      if ((l1 <= 0.0) && (l2 <= 0.0))
      {
        return (FALSE);
      }

      if ((l1 <= 0.0) || (l2 <= 0.0))
      {
        if (l1 <= 0.0)
        {
          if (viewpoint_is_in_cone)
          {
            t1 = 0.0;
            t2 = (t2 > 0.0) ? (t2) : (Max_Distance);
          }
          else
          {
            t1 = t2;
            t2 = Max_Distance;
          }
        }
        else
        {
          if (viewpoint_is_in_cone)
          {
            t2 = t1;
            t1 = 0.0;
          }
          else
          {
            t2 = Max_Distance;
          }
        }
      }

      *d1 = t1;
      *d2 = t2;

      return (TRUE);
    }
    else
    {
      if (d > -EPSILON)
      {
        if (viewpoint_is_in_cone)
        {
          *d1 = 0.0;
          *d2 = -b / a;
        }
        else
        {
          *d1 = -b / a;
          *d2 = Max_Distance;
        }

        return(TRUE);
      }
    }
  }
  else
  {
    if (viewpoint_is_in_cone)
    {
      *d1 = 0.0;
      *d2 = -c/b;

      return(TRUE);
    }
  }

  return (FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_cylinderlight
*
* INPUT
*
*   Ray    - current ray
*   Light  - current light source
*
* OUTPUT
*
*   d1, d2 - intersection depths
*
* RETURNS
*
*   int - TRUE, if hit
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Intersect a ray with the light cylinder of a cylinderlight.
*
* CHANGES
*
*   Jan 1995 : Creation.
*
******************************************************************************/

static int intersect_cylinderlight(RAY *Ray, LIGHT_SOURCE *Light, DBL *d1, DBL  *d2)
{
  DBL a, b, c, d, l1, l2, t, t1, t2, k1, k2, k3, k4;
  VECTOR V1;

  VSub(V1, Ray->Initial, Light->Center);

  VDot(k1, Ray->Direction, Light->Direction);

  VDot(k2, V1, Light->Direction);

  if ((k1 <= 0.0) && (k2 < 0.0))
  {
    return (FALSE);
  }

  a = 1.0 - Sqr(k1);

  if (a != 0.0)
  {
    VDot(k3, V1, Ray->Direction);

    VDot(k4, V1, V1);

    b = k3 - k1 * k2;

    c = k4 - Sqr(k2) - Sqr(Light->Falloff);

    d = Sqr(b) - a * c;

    if (d > EPSILON)
    {
      d = sqrt(d);

      t1 = (-b + d) / a;
      t2 = (-b - d) / a;

      if (t1 > t2)
      {
        t = t1; t1 = t2; t2 = t;
      }

      l1 = k2 + t1 * k1;
      l2 = k2 + t2 * k1;

      if ((l1 <= 0.0) && (l2 <= 0.0))
      {
        return (FALSE);
      }

      if ((l1 <= 0.0) || (l2 <= 0.0))
      {
        if (l1 <= 0.0)
        {
          t1 = 0.0;
        }
        else
        {
          t2 = (Max_Distance - k2) / k1;
        }
      }

      *d1 = t1;
      *d2 = t2;

      return (TRUE);
    }
  }

  return (FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Post_Media
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
*   Initialize media.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

void Post_Media(IMEDIA *IMedia)
{
  int i;
  DBL t;

  if (IMedia == NULL)
  {
    return;
  }
  
  /* Get extinction coefficient. */

  CLinComb2(IMedia->Extinction, 1.0, IMedia->Absorption, IMedia->sc_ext, IMedia->Scattering);

  /* Determine used effects. */

  IMedia->use_absorption = (IMedia->Absorption[0] != 0.0) ||
                          (IMedia->Absorption[1] != 0.0) ||
                          (IMedia->Absorption[2] != 0.0);

  IMedia->use_emission = (IMedia->Emission[0] != 0.0) ||
                        (IMedia->Emission[1] != 0.0) ||
                        (IMedia->Emission[2] != 0.0);

  IMedia->use_scattering = (IMedia->Scattering[0] != 0.0) ||
                          (IMedia->Scattering[1] != 0.0) ||
                          (IMedia->Scattering[2] != 0.0);

  IMedia->use_extinction = IMedia->use_absorption || IMedia->use_scattering;

  IMedia->is_constant = (IMedia->Density == NULL);

  /* Init sample threshold array. */

  if (IMedia->Sample_Threshold != NULL)
  {
    POV_FREE(IMedia->Sample_Threshold);
  }

  /* Create list of thresholds for confidence test. */

  IMedia->Sample_Threshold = (DBL *)POV_MALLOC(IMedia->Max_Samples*sizeof(DBL), "sample threshold list");

  if (IMedia->Max_Samples > 1)
  {
    t = chdtri((DBL)(IMedia->Max_Samples-1), IMedia->Confidence);

    if (t > 0.0)
    {
      t = IMedia->Variance / t;
    }
    else
    {
      t = IMedia->Variance * EPSILON;
    }

    for (i = 0; i < IMedia->Max_Samples; i++)
    {
      IMedia->Sample_Threshold[i] = t * chdtri((DBL)(i+1), IMedia->Confidence);

/*
      fprintf(stderr, "threshold for n = %3d: %f\n", i+1, IMedia->Sample_Threshold[i]);
*/
    }
  }
  else
  {
    IMedia->Sample_Threshold[0] = 0.0;
  }

  if (IMedia->Density != NULL)
  {
    Post_Pigment(IMedia->Density);
  }
  
  Post_Media(IMedia->Next_Media);  
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Media
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   IMEDIA * - created media
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a media.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

IMEDIA *Create_Media()
{
  IMEDIA *New;

  New = (IMEDIA *)POV_MALLOC(sizeof(IMEDIA), "media");

  New->Type = ISOTROPIC_SCATTERING;

  New->Intervals    = 10;
  New->Min_Samples  = 1;
  New->Max_Samples  = 1;
  New->Eccentricity = 0.0;

  Make_Colour(New->Absorption, 0.0, 0.0, 0.0);
  Make_Colour(New->Emission,   0.0, 0.0, 0.0);
  Make_Colour(New->Extinction, 0.0, 0.0, 0.0);
  Make_Colour(New->Scattering, 0.0, 0.0, 0.0);

  New->use_absorption = FALSE;
  New->use_emission   = FALSE;
  New->use_extinction = FALSE;
  New->use_scattering = FALSE;

  New->is_constant = FALSE;

  New->sc_ext     = 1.0;
  New->Ratio      = 0.9;
  New->Confidence = 0.9;
  New->Variance   = 1.0 / 128.0;

  New->Sample_Threshold = NULL;

  New->Density = NULL;

  New->Next_Media = NULL;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Media
*
* INPUT
*
*   Old - media to copy
*
* OUTPUT
*
* RETURNS
*
*   IMEDIA * - new media
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy an media.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

IMEDIA *Copy_Media(IMEDIA *Old)
{
  int i;
  IMEDIA *New, *First, *Previous, *Local_Media;

  Previous = First = NULL;

  if (Old != NULL)
  {
    for (Local_Media = Old; Local_Media != NULL; Local_Media = Local_Media->Next_Media)
    {
      New = Create_Media();

      *New = *Local_Media;

      if (Local_Media->Sample_Threshold != NULL)
      {
        if (New->Intervals > 0)
        {
          New->Sample_Threshold = (DBL *)POV_MALLOC(New->Intervals*sizeof(DBL), "sample threshold list");

          for (i = 0; i < New->Intervals; i++)
          {
            New->Sample_Threshold[i] =  Local_Media->Sample_Threshold[i];
          }
        }
      }

      New->Density = Copy_Pigment(Local_Media->Density);

      if (First == NULL)
      {
        First = New;
      }

      if (Previous != NULL)
      {
        Previous->Next_Media = New;
      }

      Previous = New;
    }
  }

  return(First);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Media
*
* INPUT
*
*   IMedia - media to destroy
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
*   Destroy an media.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Destroy_Media(IMEDIA *IMedia)
{
  IMEDIA *Local_Media, *Temp;

  if (IMedia != NULL)
  {
    Local_Media = IMedia;

    while (Local_Media != NULL)
    {
      if (Local_Media->Sample_Threshold != NULL)
      {
        POV_FREE(Local_Media->Sample_Threshold);
      }

      /* Note Destroy_Pigment also handles Density->Next */
      Destroy_Pigment(Local_Media->Density);

      Temp = Local_Media->Next_Media;

      POV_FREE(Local_Media);

      Local_Media = Temp;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   compdoubles
*
* INPUT
*
*   in_a, in_b - Elements to compare
*
* OUTPUT
*
* RETURNS
*
*   int - result of comparison
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
*   Dec 1996 : Creation.
*
******************************************************************************/

static int CDECL compdoubles(CONST void *in_a, CONST void *in_b)
{
  DBL *a, *b;

  a = (DBL *)in_a;
  b = (DBL *)in_b;

  if (*a < *b)
  {
    return (-1);
  }
  else
  {
    if (*a == *b)
    {
      return (0);
    }
    else
    {
      return (1);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Media
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
*   Transform media.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

void Transform_Media(IMEDIA *IMedia, TRANSFORM *Trans)
{
  IMEDIA *Temp;

  if (IMedia != NULL)
  {
    for (Temp = IMedia; Temp != NULL; Temp = Temp->Next_Media)
    {
      Transform_Density(Temp->Density, Trans);
    }
  }
}

void Transform_Density(PIGMENT *Density, TRANSFORM *Trans)
{
  TPATTERN *Temp = (TPATTERN *)Density;
  
  while (Temp != NULL)
  {
    Transform_Tpattern(Temp, Trans);
    Temp = Temp->Next;
  }
}

