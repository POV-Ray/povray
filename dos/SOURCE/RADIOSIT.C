/****************************************************************************
*                   radiosit.c
*
*  This module contains all radiosity calculation functions.
*
*  This file was written by Jim McElhiney.
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

/************************************************************************
*  Radiosity calculation routies.
*
*  (This does not work the way that most radiosity programs do, but it accomplishes
*  the diffuse interreflection integral the hard way and produces similar results. It
*  is called radiosity here to avoid confusion with ambient and diffuse, which
*  already have well established meanings within POV).
*  Inspired by the paper "A Ray Tracing Solution for Diffuse Interreflection"
*  by Ward, Rubinstein, and Clear, in Siggraph '88 proceedings.
*
*  Basic Idea:  Never use a constant ambient term.  Instead,
*     - For first pixel, cast a whole bunch of rays in different directions
*       from the object intersection point to see what the diffuse illumination
*       really is.  Save this value, after estimating its
*       degree of reusability.  (Method 1)
*     - For second and subsequent pixels,
*         - If there are one or more nearby values already computed,
*           average them and use the result (Method 2), else
*         - Use method 1.
*
*  Implemented by and (c) 1994-6 Jim McElhiney, mcelhiney@acm.org or 71201,1326
*  All standard POV distribution rights granted.  All other rights reserved.
*************************************************************************/

#include "string.h"
#include "frame.h"
#include "lighting.h"
#include "vector.h"
#include "povray.h"
#include "optin.h"
#include "povproto.h"
#include "render.h"
#include "texture.h"
#include "octree.h"
#include "radiosit.h"
#include "ray.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define RAD_GRADIENT 1
#define SAW_METHOD 1
/* #define SIGMOID_METHOD 1 */
/* #define SHOW_SAMPLE_SPOTS 1 */   /* try this!  bright spots at sample pts */

/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/


long ra_reuse_count = 0;
long ra_gather_count = 0;

int Radiosity_Trace_Level = 1;

COLOUR Radiosity_Gather_Total;
long Radiosity_Gather_Total_Count;
COLOUR Radiosity_Setting_Total;
long Radiosity_Setting_Total_Count;

#ifdef RADSTATS
extern long ot_blockcount;
long ot_seenodecount = 0;
long ot_seeblockcount = 0;
long ot_doblockcount = 0;
long ot_dotokcount = 0;
long ot_lastcount = 0;
long ot_lowerrorcount = 0;
#endif


OT_NODE *ot_root = NULL;

/* This (and all other changing globals) should really be in an execution
 * context structure passed down the execution call tree as a parameter to
 * each function.  This would allow for a multiprocessor/multithreaded version.
 */
FILE *ot_fd = NULL;


/*****************************************************************************
* Static functions
******************************************************************************/

static long ra_reuse (VECTOR IPoint, VECTOR Normal, COLOUR Illuminance);
static long ra_average_near (OT_BLOCK *block, void *void_info);
static void ra_gather (VECTOR IPoint, VECTOR Normal, COLOUR Illuminance, DBL Weight);
static void VUnpack (VECTOR dest_vec, BYTE_XYZ *pack);



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Ambient
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Main entry point for calculated diffuse illumination
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/
/* the colour to be calculated */
/* maximum possible contribution to pixel colour */
int Compute_Ambient(VECTOR IPoint, VECTOR  S_Normal, COLOUR Ambient_Colour, DBL Weight)
{
  int retval, reuse;
  DBL grey, save_bound;

  save_bound = opts.Radiosity_Error_Bound;
  if ( Weight < .25 )
  {
    opts.Radiosity_Error_Bound += (.25 - Weight);
  }
  reuse = ra_reuse(IPoint, S_Normal, Ambient_Colour);
  opts.Radiosity_Error_Bound = save_bound;


  if ( reuse )
  {
    ra_reuse_count++;
    retval = 0;
  }
  else
  {
    ra_gather(IPoint, S_Normal, Ambient_Colour, Weight);

    ra_gather_count++;  /* keep a running count */

    retval = 1;
  }


  if ( Radiosity_Trace_Level == 1 )
  {
 
    grey = (Ambient_Colour[RED] + Ambient_Colour[GREEN] + Ambient_Colour[BLUE]) / 3.;

    /* note grey spelling:  american options structure with worldbeat calculations! */
    Ambient_Colour[RED]   = opts.Radiosity_Gray * grey + Ambient_Colour[RED]   * (1.-opts.Radiosity_Gray);
    Ambient_Colour[GREEN] = opts.Radiosity_Gray * grey + Ambient_Colour[GREEN] * (1.-opts.Radiosity_Gray);
    Ambient_Colour[BLUE]  = opts.Radiosity_Gray * grey + Ambient_Colour[BLUE]  * (1.-opts.Radiosity_Gray);

    /* Scale up by current brightness factor prior to return */
    VScale(Ambient_Colour, Ambient_Colour, opts.Radiosity_Brightness);
  }

  return(retval);
}



/*****************************************************************************
*
* FUNCTION
*
*   ra_reuse
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Returns whether or not there were some prestored values close enough to
*   reuse.
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

static long ra_reuse/* return value */(VECTOR IPoint, VECTOR  S_Normal, COLOUR Illuminance)
{
  long i;
  WT_AVG gather;

  if (ot_root != NULL)
  {
    Make_Colour(gather.Weights_Times_Illuminances, 0.0, 0.0, 0.0);

    gather.Weights = 0.0;

    Assign_Vector(gather.P, IPoint);
    Assign_Vector(gather.N, S_Normal);

    gather.Weights_Count = 0;
    gather.Good_Count = 0;
    gather.Close_Count = 0;
    gather.Current_Error_Bound = opts.Radiosity_Error_Bound;

    for (i = 1; i < Radiosity_Trace_Level; i++)
    {
      gather.Current_Error_Bound *= 1.4;
    }

    /*
     * Go through the tree calculating a weighted average of all of the
     * usable points near this one
     */

    ot_dist_traverse(ot_root, IPoint, Radiosity_Trace_Level,
                     ra_average_near, (void *)&gather);

    /* Did we get any nearby points we could reuse? */

    if (gather.Good_Count > 0)
    {
      Make_Colour(gather.Weights_Times_Illuminances, 0.0, 0.0, 0.0);

      gather.Weights = 0.0;

      for (i = 0; i < gather.Close_Count; i++)
      {
        VAddEq(gather.Weights_Times_Illuminances, gather.Weight_Times_Illuminance[i]);

        gather.Weights += gather.Weight[i];
      }

      VInverseScale(Illuminance, gather.Weights_Times_Illuminances, gather.Weights);
    }
  }
  else
  {
    gather.Good_Count = 0;      /* No tree, so no reused values */
  }

  return(gather.Good_Count);
}



/*****************************************************************************
*
* FUNCTION
*
*   ra_average_near
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*   
* DESCRIPTION
*
*   Tree traversal function used by ra_reuse()
*   Calculate the weight of this cached value, taking into account how far
*   it is from our test point, and the difference in surface normal angles.
*
*   Given a node with an old cached value, check to see if it is reusable, and
*   aggregate its info into the weighted average being built during the tree
*   traversal. block contains Point, Normal, Illuminance,
*   Harmonic_Mean_Distance
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

static long ra_average_near(OT_BLOCK *block, void *void_info)
{
  long ind, i;
  WT_AVG *info = (WT_AVG *) void_info;
  VECTOR half, delta, delta_unit;
  COLOUR tc, prediction;
  DBL ri, error_reuse, dir_diff, in_front, dist, weight, square_dist, dr, dg, db;
  DBL error_reuse_rotate, error_reuse_translate, inverse_dist, cos_diff_from_nearest;
  DBL quickcheck_rad;


#ifdef RADSTATS
  ot_doblockcount++;
#endif

  VSub(delta, info->P, block->Point);   /* a = b - c, which is test p minus old pt */

  square_dist = VSumSqr(delta);

  quickcheck_rad = (DBL)block->Harmonic_Mean_Distance * info->Current_Error_Bound;

  /* first we do a tuning test--this func gets called a LOT */
  if (square_dist < quickcheck_rad * quickcheck_rad)
  {

    dist = sqrt(square_dist);
    ri = (DBL)block->Harmonic_Mean_Distance;


    if ( dist > .000001 )
    {
      inverse_dist = 1./dist;
      VScale(delta_unit, delta, inverse_dist);  /* this is a normalization */

      /* This block reduces the radius of influence when it points near the nearest
         surface found during sampling. */
      VDot( cos_diff_from_nearest, block->To_Nearest_Surface, delta_unit);
      if ( cos_diff_from_nearest > 0. )
      {
        ri = cos_diff_from_nearest * (DBL)block->Nearest_Distance +
             (1.-cos_diff_from_nearest) * ri;
      }
    }

    if (dist < ri * info->Current_Error_Bound)
    {
      VDot(dir_diff, info->N, block->S_Normal);

      /* NB error_reuse varies from 0 to 3.82 (1+ 2 root 2) */

      error_reuse_translate = dist / ri;

      error_reuse_rotate = 2.0 * sqrt(fabs(1.0 - dir_diff));

      error_reuse = error_reuse_translate + error_reuse_rotate;

      /* is this old point within a reasonable error distance? */

      if (error_reuse < info->Current_Error_Bound)
      {
#ifdef RADSTATS
        ot_lowerrorcount++;
#endif

        if (dist > 0.000001)
        {
          /*
           * Make sure that the old point is not in front of this point, the
           * old surface might shadow this point and make the result
           * meaningless
           */
          VHalf(half, info->N, block->S_Normal);
          VNormalizeEq(half);          /* needed so check can be to constant */
          VDot(in_front, delta_unit, half);
        }
        else
        {
          in_front = 1.0;
        }

        /*
         * Theory:        eliminate the use of old points well in front of our
         * new point we are calculating, but not ones which are just a little
         * tiny bit in front.  This (usually) avoids eliminating points on the
         * same surface by accident.
         */

        if (in_front > (-0.05))
        {
#ifdef RADSTATS
          ot_dotokcount++;
#endif

#ifdef SIGMOID_METHOD
          weight = error_reuse / info->Current_Error_Bound;  /* 0 < t < 1 */
          weight = (cos(weight * M_PI) + 1.0) * 0.5;         /* 0 < w < 1 */
#endif
#ifdef SAW_METHOD
          weight = 1.0 - (error_reuse / info->Current_Error_Bound); /* 0 < t < 1 */
#endif

          if ( weight > 0.001 )
          { /* avoid floating point oddities near zero */

            /* This is the block where we use the gradient to improve the prediction */
            dr = delta[X] * block->drdx + delta[Y] * block->drdy + delta[Z] * block->drdz;
            dg = delta[X] * block->dgdx + delta[Y] * block->dgdy + delta[Z] * block->dgdz;
            db = delta[X] * block->dbdx + delta[Y] * block->dbdy + delta[Z] * block->dbdz;
#ifndef RAD_GRADIENT
            dr = dg = db = 0.;
#endif
#if 0
            /* Ensure that the total change in colour is a reasonable magnitude */
            if ( dr > .1 ) dr = .1;  else if ( dr < -.1 ) dr = -.1;
            if ( dg > .1 ) dg = .1;  else if ( dg < -.1 ) dg = -.1;
            if ( db > .1 ) db = .1;  else if ( db < -.1 ) db = -.1;
#endif
            prediction[RED]   = block->Illuminance[RED]   + dr;
            prediction[RED]   = min(max(prediction[RED],  0.), 1.);
            prediction[GREEN] = block->Illuminance[GREEN] + dg;
            prediction[GREEN] = min(max(prediction[GREEN],0.), 1.);
            prediction[BLUE]  = block->Illuminance[BLUE]  + db;
            prediction[BLUE]  = min(max(prediction[BLUE], 0.), 1.);

#ifdef SHOW_SAMPLE_SPOTS
            if ( dist < opts.Radiosity_Dist_Max * .015 ) {
              prediction[RED] = prediction[GREEN] = prediction[BLUE] = 3.;
            }
#endif

            /* The predicted colour is an extrapolation based on the old value */
            VScale(tc, prediction, weight);

            VAddEq(info->Weights_Times_Illuminances, tc);

            info->Weights += weight;

            info->Weights_Count++;

            info->Good_Count++;

            ind = -1;

            if (info->Close_Count < opts.Radiosity_Nearest_Count)
            {
              ind = info->Close_Count++;
            }
            else
            {
              for (i = 0; i < info->Close_Count; i++)
              {
                if (dist < info->Distance[i])
                {
                  ind = i;

                  i = opts.Radiosity_Nearest_Count;
                }
              }
            }

            if (ind != -1)
            {
              info->Distance[ind] = dist;

              info->Weight[ind] = weight;

              VScale(info->Weight_Times_Illuminance[ind], prediction, weight);

            }
          }
        }
      }
    }
  }

  return(1);
}



/*****************************************************************************
*
* FUNCTION
*
*   ra_gather
*
* INPUT
*   IPoint - a point at which the illumination is needed
*   S_Normal - the surface normal (not perturbed by the current layer) at that point
*   Illuminance - a place to put the return result
*   Weight - the weight of this point in final output, to drive ADC_Bailout
*   
* OUTPUT
*   The average colour of light of objects visible from the specified point.
*   The colour is returned in the Illuminance parameter.
*
*   
* RETURNS
*   
* AUTHOUR
*
*   Jim McElhiney
*   
* DESCRIPTION
*    Gather up the incident light and average it.
*    Return the results in Illuminance, and also cache them for later.
*    Note that last parameter is similar to weight parameter used
*    to control ADC_Bailout as a parameter to Trace(), but it also
*    takes into account that this subsystem calculates only ambient
*    values.  Therefore, coming in at the top level, the value might
*    be 0.3 if the first object hit had an ambient of 0.3, whereas
*    Trace() would have been passed a parameter of 1.0 (since it
*    calculates the whole pixel value).
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

static void ra_gather(VECTOR IPoint, VECTOR  S_Normal, COLOUR Illuminance, DBL Weight)
{
  extern FRAME Frame;
  long i, hit, Current_Radiosity_Count;
  unsigned long Save_Quality_Flags, Save_Options;
  VECTOR random_vec, direction, n2, n3, up, min_dist_vec;
  int save_Max_Trace_Level;
  DBL Inverse_Distance_Sum, depth, mean_dist, weight, save_min_reuse,
      drdxs, dgdxs, dbdxs, drdys, dgdys, dbdys, drdzs, dgdzs, dbdzs,
      depth_weight_for_this_gradient, dxsquared, dysquared, dzsquared,
      constant_term, deemed_depth, min_dist, reuse_dist_min, to_eye,
      sum_of_inverse_dist, sum_of_dist, average_dist, gradient_count;
  COLOUR Colour_Sums, Temp_Colour;
  RAY New_Ray;
  OT_BLOCK *block;
  OT_ID id;

  /*
   * A bit of theory: The goal is to create a set of "random" direction rays
   * so that the probability of close-to-normal versus close-to-tangent rolls
   * off in a cos-theta curve, where theta is the deviation from normal.
   * That is, lots of rays close to normal, and very few close to tangent.
   * You also want to have all of the rays be evenly spread, no matter how
   * many you want to use.  The lookup array has an array of points carefully
   * chosen to meet all of these criteria.
  */


  /* The number of rays to trace varies with our recursion depth */

  Current_Radiosity_Count = opts.Radiosity_Count;
  save_min_reuse = opts.Radiosity_Min_Reuse;
  for ( i=1; i<Radiosity_Trace_Level; i++ )
  {
    Current_Radiosity_Count /= 3;
    opts.Radiosity_Min_Reuse *= 2.;
  }

  /* Save some global stuff which we have to change for now */

  save_Max_Trace_Level = Max_Trace_Level;

 
  /* Since we'll be calculating averages, zero the accumulators */

  Make_Colour(Colour_Sums, 0., 0., 0.);
  Inverse_Distance_Sum = 0.;


  min_dist = BOUND_HUGE;

  if ( fabs(fabs(S_Normal[Z])- 1.) < .1 ) {
    /* too close to vertical for comfort, so use cross product with horizon */
    up[X] = 0.; up[Y] = 1.; up[Z] = 0.;
  }
  else
  {
    up[X] = 0.; up[Y] = 0.; up[Z] = 1.;
  }

  VCross(n2, S_Normal, up);  VNormalizeEq(n2);
  VCross(n3, S_Normal, n2);  VNormalizeEq(n3);


  /* Note that this max() forces at least one ray to be shot.
    Otherwise, the loop does nothing, since every call to 
    Trace() just bails out immediately! */
  weight = max(ADC_Bailout, Weight/(DBL)Current_Radiosity_Count);

  /* Initialized the accumulators for the integrals which will be come the rad gradient */
  drdxs = dgdxs = dbdxs = drdys = dgdys = dbdys = drdzs = dgdzs = dbdzs = 0.;
  sum_of_inverse_dist = sum_of_dist = gradient_count = 0.;


  for (i = hit = 0; i < Current_Radiosity_Count; i++)
  {
    /* pick a random direction with the right statistical skew */

    VUnpack(random_vec, &rad_samples[i]);

    if ( fabs(S_Normal[Z] - 1.) < .001 )         /* pretty well straight Z, folks */
    {
      /* we are within 1/20 degree of pointing in the Z axis. */
      /* use all vectors as is--they're precomputed this way */
      Assign_Vector(direction, random_vec);
    }
    else
    {
      direction[X] = n2[X]*random_vec[X] + n3[X]*random_vec[Y] + S_Normal[X]*random_vec[Z];
      direction[Y] = n2[Y]*random_vec[X] + n3[Y]*random_vec[Y] + S_Normal[Y]*random_vec[Z];
      direction[Z] = n2[Z]*random_vec[X] + n3[Z]*random_vec[Y] + S_Normal[Z]*random_vec[Z];
    }


    /* Build a ray pointing in the chosen direction */
    Make_Colour(Temp_Colour, 0.0, 0.0, 0.0);
    Initialize_Ray_Containers(&New_Ray);
    Assign_Vector(New_Ray.Initial, IPoint);
    Assign_Vector(New_Ray.Direction, direction);

    /* save some flags that must be set to a different value during the trace() */
    Save_Quality_Flags = opts.Quality_Flags;
    Save_Options = opts.Options;
    opts.Radiosity_Quality = 6;

#ifdef SAFE_BUT_SLOW
    opts.Quality_Flags = Quality_Values[opts.Radiosity_Quality];
#else
    /* Set up a custom quality level, like Q=5, without area lights, with radiosity */
    opts.Quality_Flags = Q_SHADOW;
    opts.Options &= ~USE_LIGHT_BUFFER;
#endif

    /* Go down in recursion, trace the result, and come back up */

    Trace_Level++;
    Radiosity_Trace_Level++;
    depth = Trace(&New_Ray, Temp_Colour, weight);
    Radiosity_Trace_Level--;
    Trace_Level--;


    /* Add into illumination gradient integrals */

    deemed_depth = depth;
    if (deemed_depth < opts.Radiosity_Dist_Max * 10.)
    {
      depth_weight_for_this_gradient = 1. / deemed_depth;
      sum_of_inverse_dist += 1. / deemed_depth;
      sum_of_dist += deemed_depth;
      gradient_count++;

      dxsquared = direction[X] * direction[X]; if (direction[X] < 0.) dxsquared = -dxsquared;
      dysquared = direction[Y] * direction[Y]; if (direction[Y] < 0.) dysquared = -dysquared;
      dzsquared = direction[Z] * direction[Z]; if (direction[Z] < 0.) dzsquared = -dzsquared;
      drdxs += dxsquared * Temp_Colour[RED]   * depth_weight_for_this_gradient;
      dgdxs += dxsquared * Temp_Colour[GREEN] * depth_weight_for_this_gradient;
      dbdxs += dxsquared * Temp_Colour[BLUE]  * depth_weight_for_this_gradient;
      drdys += dysquared * Temp_Colour[RED]   * depth_weight_for_this_gradient;
      dgdys += dysquared * Temp_Colour[GREEN] * depth_weight_for_this_gradient;
      dbdys += dysquared * Temp_Colour[BLUE]  * depth_weight_for_this_gradient;
      drdzs += dzsquared * Temp_Colour[RED]   * depth_weight_for_this_gradient;
      dgdzs += dzsquared * Temp_Colour[GREEN] * depth_weight_for_this_gradient;
      dbdzs += dzsquared * Temp_Colour[BLUE]  * depth_weight_for_this_gradient;
    }


    if (depth > opts.Radiosity_Dist_Max)
    {
      depth = opts.Radiosity_Dist_Max;
    }
    else
    {
#ifdef RADSTATS
      hit++;
#endif
    }

    if (depth < min_dist)
    {
      min_dist = depth;
      Assign_Vector(min_dist_vec, direction);
    }

    opts.Quality_Flags = Save_Quality_Flags;
    opts.Options = Save_Options;

    /* Add into total illumination integral */

    VAddEq(Colour_Sums, Temp_Colour);

    Inverse_Distance_Sum += 1.0 / depth;
  } /* end ray sampling loop */


  /*
   * Use the accumulated values to calculate the averages needed. The sphere
   * of influence of this primary-method sample point is based on the
   * harmonic mean distance to the points encountered. (An harmonic mean is
   * the inverse of the mean of the inverses).
   */

  mean_dist = 1.0 / (Inverse_Distance_Sum / (DBL) Current_Radiosity_Count);

  VInverseScale(Illuminance, Colour_Sums, (DBL) Current_Radiosity_Count);

  /* Keep a running total of the final Illuminances we calculated */
  if ( Radiosity_Trace_Level == 1.) {
    VAddEq(Radiosity_Gather_Total, Illuminance);
    Radiosity_Gather_Total_Count++;
  }


  /* We want to cached this block for later reuse.  But,
   * if ground units not big enough, meaning that the value has very
   * limited reuse potential, forget it.
   */

  if (mean_dist > (opts.Radiosity_Dist_Max * 0.0001))
  {

    /*
     * Theory:  we don't want to calculate a primary method ray loop at every
     * point along the inside edges, so a minimum effectivity is practical.
     * It is expressed as a fraction of the distance to the eyepoint.  1/2%
     * is a good number.  This enhancement was Greg Ward's idea, but the use
     * of % units is my idea.  [JDM]
     */

    VDist(to_eye, Frame.Camera->Location, IPoint);
    reuse_dist_min = to_eye * opts.Radiosity_Min_Reuse;
    if (mean_dist < reuse_dist_min)
    {
      mean_dist = reuse_dist_min;
    }


    /* figure out the block id */
    ot_index_sphere(IPoint, mean_dist * opts.Radiosity_Error_Bound, &id);


#ifdef RADSTATS
    ot_blockcount++;
#endif

    /* After end of ray loop, we've decided that this point is worth storing */
    /* Allocate a block, and fill it with values for reuse in cacheing later */
    block = (OT_BLOCK *)POV_MALLOC(sizeof(OT_BLOCK), "octree block");
    memset(block, 0, sizeof(OT_BLOCK));

    /* beta */
    if ( gradient_count > 10.)
    {
      average_dist = sum_of_dist / gradient_count;
      constant_term = 1.00 / (sum_of_inverse_dist * average_dist );

      block->drdx = (float)(drdxs * constant_term);
      block->dgdx = (float)(dgdxs * constant_term);
      block->dbdx = (float)(dbdxs * constant_term);
      block->drdy = (float)(drdys * constant_term);
      block->dgdy = (float)(dgdys * constant_term);
      block->dbdy = (float)(dbdys * constant_term);
      block->drdz = (float)(drdzs * constant_term);
      block->dgdz = (float)(dgdzs * constant_term);
      block->dbdz = (float)(dbdzs * constant_term);
    }


    /* Fill up the values in the octree (ot_) cache block */

    Assign_Vector(block->Illuminance, Illuminance);
    Assign_Vector(block->To_Nearest_Surface, min_dist_vec);
    block->Harmonic_Mean_Distance = (float)mean_dist;
    block->Nearest_Distance = (float)min_dist;
    block->Bounce_Depth = (short)Radiosity_Trace_Level;
    Assign_Vector(block->Point, IPoint);
    Assign_Vector(block->S_Normal, S_Normal);
    block->next = NULL;

    /* store the info block in the oct tree */
    ot_ins(&ot_root, block, &id);

    /* In case the rendering is suspended, save the cache tree values to a file */
    if ( opts.Radiosity_File_SaveWhileRendering && (ot_fd != NULL) ) {
      ot_write_block(block, ot_fd);
    }
  }

  /* Put things back where they were in recursion depth */
  Max_Trace_Level = save_Max_Trace_Level;
  opts.Radiosity_Min_Reuse = save_min_reuse;

}


/*****************************************************************************
*
* FUNCTION  VUnpack()  -  Unpacks "pack_vec" into "dest_vec" and normalizes it.
*
* INPUT
*
* OUTPUT
*
* RETURNS   Nothing
*
* AUTHOUR   Jim McElhiney
*
* DESCRIPTION
*
*  The precomputed radiosity rays are packed into a lookup array with one byte
*  for each of dx, dy, and dz.  dx and dy are scaled from the range (-1. to 1.),
*  and dz is scaled from the range (0. to 1.), and both are stored in the range
*  0 to 255.
*
*  The reason for this function is that it saves a bit of memory.  There are 2000
*  entries in the table, and packing them saves 21 bytes each, or 42KB.
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/
static void VUnpack(VECTOR dest_vec, BYTE_XYZ * pack_vec)
{
  dest_vec[X] = ((double)pack_vec->x * (1./ 255.))*2.-1.;
  dest_vec[Y] = ((double)pack_vec->y * (1./ 255.))*2.-1.;
  dest_vec[Z] = ((double)pack_vec->z * (1./ 255.));

  VNormalizeEq(dest_vec);   /* already good to about 1%, but we can do better */
}


/*****************************************************************************
*
* FUNCTION  Initialize_Radiosity_Code
*
* INPUT     Nothing.
*
* OUTPUT    Sets various global states used by radiosity.  Notably,
*           ot_fd - the file identifier of the file used to save radiosity values
*
* RETURNS   1 for Success, 0 for failure  (e.g., could not open cache file)
*
* AUTHOUR   Jim McElhiney
*
* DESCRIPTION
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/
long
Initialize_Radiosity_Code()
{
  long retval, used_existing_file;
  FILE *fd;
  char *modes, rad_cache_filename[256];

  retval = 1;                   /* assume the best */

  if ( opts.Options & RADIOSITY )
  {
    opts.Radiosity_Preview_Done = 0;

    ra_gather_count  = 0;
    ra_reuse_count   = 0;


    if ( opts.Radiosity_Dist_Max == 0. )
    {
      /* User hasn't picked a radiosity dist max, so pick one automatically. */
      VDist(opts.Radiosity_Dist_Max, Frame.Camera->Location,
                                     Frame.Camera->Look_At);
      opts.Radiosity_Dist_Max *= 0.2;
    }

#ifdef RADSTATS
    ot_seenodecount  = 0;
    ot_seeblockcount = 0;
    ot_doblockcount  = 0;
    ot_dotokcount    = 0;
    ot_lowerrorcount = 0;
    ot_lastcount     = 0;
#endif

    if ( ot_fd != NULL )    /* if already open for some unknown reason, close it */
    {
      fclose(ot_fd);
      ot_fd = 0;
    }

    /* build the file name for the radiosity cache file */
    strcpy(rad_cache_filename, opts.Scene_Name);
    strcat(rad_cache_filename, RADIOSITY_CACHE_EXTENSION);

    used_existing_file = 0;
    if ( ((opts.Options & CONTINUE_TRACE) && opts.Radiosity_File_ReadOnContinue)  ||
         opts.Radiosity_File_AlwaysReadAtStart )
    {
      fd = fopen(rad_cache_filename, READ_BINFILE_STRING);   /* "myname.rca" */
      if ( fd != NULL) {
        used_existing_file = ot_read_file(fd);
        retval &= used_existing_file;
        fclose(fd);
      }
    }
    else
    {
      DELETE_FILE(rad_cache_filename);  /* default case, force a clean start */
    }


    if ( opts.Radiosity_File_SaveWhileRendering )
    {
      /* If we are writing a file, but not using what's there, we truncate,
         since we conclude that what is there is bad.
         But, if we are also using what's there, then it must be good, so
         we just append to it.
      */
      modes = used_existing_file ? APPEND_BINFILE_STRING : WRITE_BINFILE_STRING;
      ot_fd = fopen(rad_cache_filename, modes);
      retval &= (ot_fd != NULL);
    }
  }

  return retval;
}


/*****************************************************************************
*
* FUNCTION  Deinitialize_Radiosity_Code()
*
* INPUT     Nothing.
*
* OUTPUT    Sets various global states used by radiosity.  Notably,
*           ot_fd - the file identifier of the file used to save radiosity values
*
* RETURNS   1 for total success, 0 otherwise (e.g., could not save cache tree)
*
* AUTHOUR   Jim McElhiney
*
* DESCRIPTION
*   Wrap up and free any radiosity-specific features.
*   Note that this function is safe to call even if radiosity was not on.
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/
long
Deinitialize_Radiosity_Code()
{
  long retval;
  char rad_cache_filename[256];
  FILE *fd;

  retval = 1;                    /* assume the best */

  if ( opts.Options & RADIOSITY )
  {
  /* if the global file identifier is set, close it */
  if ( ot_fd != NULL ) {
    fclose(ot_fd);
    ot_fd = NULL;
  }


  /* build the file name for the radiosity cache file */
  strcpy(rad_cache_filename, opts.Scene_Name);
  strcat(rad_cache_filename, RADIOSITY_CACHE_EXTENSION);


  /* If user has not asked us to save the radiosity cache file, delete it */
  if ( opts.Radiosity_File_SaveWhileRendering  &&
      !(opts.Radiosity_File_KeepAlways || (Stop_Flag && opts.Radiosity_File_KeepOnAbort) ) )
  {
    DELETE_FILE(rad_cache_filename);
  }

  /* after-the-fact version.  This is an alternative to putting a call to 
     ot_write_node after the call to ot_ins in ra_gather().
     The on-the-fly version (all of the code which uses ot_fd) is superior
     in that you will get partial results if you restart your rendering
     with a different resolution or camera angle.  This version is superior
     in that your rendering goes a lot quicker.
  */

  if (!(opts.Radiosity_File_KeepAlways || (Stop_Flag && opts.Radiosity_File_KeepOnAbort)) &&  
      !opts.Radiosity_File_SaveWhileRendering && ot_root != NULL )
  {
    fd = fopen(rad_cache_filename, WRITE_BINFILE_STRING);

    if ( fd != NULL ) {
      retval &= ot_save_tree(ot_root, fd);

      fclose(fd);
    }
    else
    {
      retval = 0;
    }
  }


  /* Note that multiframe animations should call this free function if they have
     moving objects and want correct results.
     They should NOT call this function if they have no moving objects (like
     fly-throughs) and want speed
  */
  if ( ot_root != NULL ) {
    retval &= ot_free_tree(&ot_root);   /* this zeroes the root pointer */
  }

  }
  return retval;
}
