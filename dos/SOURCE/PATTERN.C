/**************************************************************************
*                pattern.c
*
*  This module implements texturing functions that return a value to be
*  used in a pigment or normal.
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
* Modifications by Hans-Detlev Fink, January 1999, used with permission
* Modifications by Thomas Willhalm, March 1999, used with permission
*
*****************************************************************************/

/*
 * Some texture ideas garnered from SIGGRAPH '85 Volume 19 Number 3,
 * "An Image Synthesizer" By Ken Perlin.
 * Further Ideas Garnered from "The RenderMan Companion" (Addison Wesley).
 */

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "matrices.h"
#include "pattern.h"
#include "povray.h"
#include "texture.h"
#include "image.h"
#include "txttest.h"
#include "colour.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/


/*****************************************************************************
* Static functions
******************************************************************************/

static DBL agate (VECTOR EPoint, TPATTERN *TPat);
static DBL brick (VECTOR EPoint, TPATTERN *TPat);
static DBL checker (VECTOR EPoint);
static DBL crackle (VECTOR EPoint);
static DBL gradient (VECTOR EPoint, TPATTERN *TPat);
static DBL granite (VECTOR EPoint);
static DBL leopard (VECTOR EPoint);
static DBL mandel (VECTOR EPoint, TPATTERN *TPat);
static DBL marble (VECTOR EPoint, TPATTERN *TPat);
static DBL onion (VECTOR EPoint);
static DBL radial (VECTOR EPoint);
static DBL spiral1 (VECTOR EPoint, TPATTERN *TPat);
static DBL spiral2 (VECTOR EPoint, TPATTERN *TPat);
static DBL wood (VECTOR EPoint, TPATTERN *TPat);
static DBL hexagon (VECTOR EPoint);
static DBL planar_pattern (VECTOR EPoint);
static DBL spherical (VECTOR EPoint);
static DBL boxed (VECTOR EPoint);
static DBL cylindrical (VECTOR EPoint);
static DBL density_file (VECTOR EPoint, TPATTERN *TPat);

static long PickInCube (VECTOR tv, VECTOR p1);

static DBL ripples_pigm (VECTOR EPoint, TPATTERN *TPat);
static DBL waves_pigm  (VECTOR EPoint, TPATTERN *TPat);
static DBL dents_pigm  (VECTOR EPoint);
static DBL wrinkles_pigm (VECTOR EPoint);
static DBL quilted_pigm (VECTOR EPoint, TPATTERN *TPat);
static TURB *Search_For_Turb (WARP *Warps);
/* static TURB *Copy_Turb (TURB *Old);   Unused function [AED] */
static unsigned short readushort(FILE *infile);


/*****************************************************************************
*
* FUNCTION
*
*   agate
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994    : adapted from agate pigment by [CY]
*
******************************************************************************/

static DBL agate (VECTOR EPoint, TPATTERN *TPat)
{
  register DBL noise, turb_val;
  TURB* Turb;

  Turb=Search_For_Turb(TPat->Warps);

  turb_val = TPat->Vals.Agate_Turb_Scale * Turbulence(EPoint,Turb);

  noise = 0.5 * (cycloidal(1.3 * turb_val + 1.1 * EPoint[Z]) + 1.0);

  if (noise < 0.0)
  {
    noise = 0.0;
  }
  else
  {
    noise = min(1.0, noise);
    noise = pow(noise, 0.77);
  }

  return(noise);
}


/*****************************************************************************
*
* FUNCTION
*
*   brick
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value exactly 0.0 or 1.0
*   
* AUTHOR
*
*   Dan Farmer
*   
* DESCRIPTION
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL brick (VECTOR EPoint, TPATTERN *TPat)
{
  int ibrickx, ibricky, ibrickz;
  DBL brickheight, brickwidth, brickdepth;
  DBL brickmortar, mortarheight, mortarwidth, mortardepth;
  DBL brickx, bricky, brickz;
  DBL x, y, z, fudgit;

  fudgit=Small_Tolerance+TPat->Vals.Brick.Mortar;

  x =  EPoint[X]+fudgit;
  y =  EPoint[Y]+fudgit;
  z =  EPoint[Z]+fudgit;

  brickwidth  = TPat->Vals.Brick.Size[X];
  brickheight = TPat->Vals.Brick.Size[Y];
  brickdepth  = TPat->Vals.Brick.Size[Z];
  brickmortar = (DBL)TPat->Vals.Brick.Mortar;

  mortarwidth  = brickmortar / brickwidth;
  mortarheight = brickmortar / brickheight;
  mortardepth  = brickmortar / brickdepth;

  /* 1) Check mortar layers in the X-Z plane (ie: top view) */

  bricky = y / brickheight;
  ibricky = (int) bricky;
  bricky -= (DBL) ibricky;

  if (bricky < 0.0)
  {
    bricky += 1.0;
  }

  if (bricky <= mortarheight)
  {
    return(0.0);
  }

  bricky = (y / brickheight) * 0.5;
  ibricky = (int) bricky;
  bricky -= (DBL) ibricky;

  if (bricky < 0.0)
  {
    bricky += 1.0;
  }


  /* 2) Check ODD mortar layers in the Y-Z plane (ends) */

  brickx = (x / brickwidth);
  ibrickx = (int) brickx;
  brickx -= (DBL) ibrickx;

  if (brickx < 0.0)
  {
    brickx += 1.0;
  }

  if ((brickx <= mortarwidth) && (bricky <= 0.5))
  {
    return(0.0);
  }

  /* 3) Check EVEN mortar layers in the Y-Z plane (ends) */

  brickx = (x / brickwidth) + 0.5;
  ibrickx = (int) brickx;
  brickx -= (DBL) ibrickx;

  if (brickx < 0.0)
  {
    brickx += 1.0;
  }

  if ((brickx <= mortarwidth) && (bricky > 0.5))
  {
    return(0.0);
  }

  /* 4) Check ODD mortar layers in the Y-X plane (facing) */

  brickz = (z / brickdepth);
  ibrickz = (int) brickz;
  brickz -= (DBL) ibrickz;

  if (brickz < 0.0)
  {
    brickz += 1.0;
  }

  if ((brickz <= mortardepth) && (bricky > 0.5))
  {
    return(0.0);
  }

  /* 5) Check EVEN mortar layers in the X-Y plane (facing) */

  brickz = (z / brickdepth) + 0.5;
  ibrickz = (int) brickz;
  brickz -= (DBL) ibrickz;

  if (brickz < 0.0)
  {
    brickz += 1.0;
  }

  if ((brickz <= mortardepth) && (bricky <= 0.5))
  {
    return(0.0);
  }

  /* If we've gotten this far, color me brick. */

  return(1.0);
}


/*****************************************************************************
*
* FUNCTION
*
*   checker
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0 or 1.0
*
* AUTHOR
*
*   POV-Team
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL checker (VECTOR EPoint)
{
  int value;

  value = (int)(floor(EPoint[X]+Small_Tolerance) +
                floor(EPoint[Y]+Small_Tolerance) +
                floor(EPoint[Z]+Small_Tolerance));

  if (value & 1)
  {
    return (1.0);
  }
  else
  {
    return (0.0);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   crackle
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   "crackle":
*
*   New colour function by Jim McElhiney,
*     CompuServe 71201,1326, aka mcelhiney@acm.org
*
*   Large scale, without turbulence, makes a pretty good stone wall.
*   Small scale, without turbulence, makes a pretty good crackle ceramic glaze.
*   Highly turbulent (with moderate displacement) makes a good marble, solving
*   the problem of apparent parallel layers in Perlin's method.
*   2 octaves of full-displacement turbulence make a great "drizzled paint"
*   pattern, like a 1950's counter top.
*   Rule of thumb:  put a single colour transition near 0 in your colour map.
*
*   Mathematically, the set crackle(p)=0 is a 3D Voronoi diagram of a field of
*   semirandom points, and crackle(p)>0 is distance from set along shortest path.
*   (A Voronoi diagram is the locus of points equidistant from their 2 nearest
*   neighbours from a set of disjoint points, like the membranes in suds are
*   to the centres of the bubbles).
*
*   All "crackle" specific source code and examples are in the public domain.
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL crackle (VECTOR EPoint)
{
  int    i;
  long   thisseed;
  DBL    sum, minsum, minsum2, tf;
  VECTOR sv, tv, dv, t1, add;

  static int cvc;
  static long lastseed = 0x80000000;
  static VECTOR cv[81];

  Assign_Vector(tv,EPoint);

  /*
   * Check to see if the input point is in the same unit cube as the last
   * call to this function, to use cache of cubelets for speed.
   */

  thisseed = PickInCube(tv, t1);

  if (thisseed != lastseed)
  {
    /*
     * No, not same unit cube.  Calculate the random points for this new
     * cube and its 80 neighbours which differ in any axis by 1 or 2.
     * Why distance of 2?  If there is 1 point in each cube, located
     * randomly, it is possible for the closest random point to be in the
     * cube 2 over, or the one two over and one up.  It is NOT possible
     * for it to be two over and two up.  Picture a 3x3x3 cube with 9 more
     * cubes glued onto each face.
     */

    /* Now store a points for this cube and each of the 80 neighbour cubes. */

    cvc = 0;

    for (add[X] = -2.0; add[X] < 2.5; add[X] +=1.0)
    {
      for (add[Y] = -2.0; add[Y] < 2.5; add[Y] += 1.0)
      {
        for (add[Z] = -2.0; add[Z] < 2.5; add[Z] += 1.0)
        {
          /* For each cubelet in a 5x5 cube. */

          if ((fabs(add[X])>1.5)+(fabs(add[Y])>1.5)+(fabs(add[Z])>1.5) <= 1.0)
          {
            /* Yes, it's within a 3d knight move away. */

            VAdd(sv, tv, add);

            PickInCube(sv, t1);

            cv[cvc][X] = t1[X];
            cv[cvc][Y] = t1[Y];
            cv[cvc][Z] = t1[Z];
            cvc++;
          }
        }
      }
    }

    lastseed = thisseed;
  }

  /*
   * Find the 2 points with the 2 shortest distances from the input point.
   * Loop invariant:  minsum is shortest dist, minsum2 is 2nd shortest
   */

  /* Set up the loop so the invariant is true:  minsum <= minsum2 */

  VSub(dv, cv[0], tv);  minsum  = VSumSqr(dv);
  VSub(dv, cv[1], tv);  minsum2 = VSumSqr(dv);

  if (minsum2 < minsum)
  {
    tf = minsum; minsum = minsum2; minsum2 = tf;
  }

  /* Loop for the 81 cubelets to find closest and 2nd closest. */

  for (i = 2; i < cvc; i++)
  {
    VSub(dv, cv[i], tv);

    sum = VSumSqr(dv);

    if (sum < minsum)
    {
      minsum2 = minsum;
      minsum = sum;
    }
    else
    {
      if (sum < minsum2)
      {
        minsum2 = sum;
      }
    }
  }

  /* Crackle value is absolute value of diff in dist to closest 2 points. */

  tf = sqrt(minsum2) - sqrt(minsum);      /* minsum is known <= minsum2 */

  /*
   * Note that the theoretical range of this function is 0 to root 3.
   * In practice, it rarely exceeds 0.9, and only very rarely 1.0
   */

  return min(tf, 1.);
}


/*****************************************************************************
*
* FUNCTION
*
*   gradient
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Gradient Pattern - gradient based on the fractional values of
*   x, y or z, based on whether or not the given directional vector is
*   a 1.0 or a 0.0.
*   The basic concept of this is from DBW Render, but Dave Wecker's
*   only supports simple Y axis gradients.
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL gradient (VECTOR EPoint, TPATTERN *TPat)
{
  register int i;
  register DBL temp;
  DBL value = 0.0;

  for (i=X; i<=Z; i++)
  {
    if (TPat->Vals.Gradient[i] != 0.0)
    {
      temp = fabs(EPoint[i]);

      value += fmod(temp,1.0);
    }
  }

  /* Clamp to 1.0. */

  value = ((value > 1.0) ? fmod(value, 1.0) : value);

  return(value);
}



/*****************************************************************************
*
* FUNCTION
*
*   granite
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Granite - kind of a union of the "spotted" and the "dented" textures,
*   using a 1/f fractal noise function for color values. Typically used
*   with small scaling values. Should work with colour maps for pink granite.
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL granite (VECTOR EPoint)
{
  register int i;
  register DBL temp, noise = 0.0, freq = 1.0;
  VECTOR tv1,tv2;

  VScale(tv1,EPoint,4.0);

  for (i = 0; i < 6 ; freq *= 2.0, i++)
  {
    VScale(tv2,tv1,freq);
    temp = 0.5 - Noise (tv2);

    temp = fabs(temp);

    noise += temp / freq;
  }

  return(noise);
}



/*****************************************************************************
*
* FUNCTION
*
*   leopard
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Scott Taylor
*
* DESCRIPTION
*
* CHANGES
*   Jul 1991 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL leopard (VECTOR EPoint)
{
  register DBL value, temp1, temp2, temp3;

  /* This form didn't work with Zortech 386 compiler */
  /* value = Sqr((sin(x)+sin(y)+sin(z))/3); */
  /* So we break it down. */

  temp1 = sin(EPoint[X]);
  temp2 = sin(EPoint[Y]);
  temp3 = sin(EPoint[Z]);

  value = Sqr((temp1 + temp2 + temp3) / 3.0);

  return(value);
}



/*****************************************************************************
*
* FUNCTION
*
*   mandel
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   submitted by user, name lost (sorry)
*
* DESCRIPTION
* The mandel pattern computes the standard Mandelbrot fractal pattern and
* projects it onto the X-Y plane.  It uses the X and Y coordinates to compute
* the Mandelbrot set.
*
* CHANGES
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL mandel (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, x, y;

  a = x = EPoint[X]; a2 = Sqr(a);
  b = y = EPoint[Y]; b2 = Sqr(b);

  it_max = TPat->Vals.Iterations;

  for (col = 0; col < it_max; col++)
  {
    b  = 2.0 * a * b + y;
    a  = a2 - b2 + x;

    a2 = Sqr(a);
    b2 = Sqr(b);

    if ((a2 + b2) > 4.0)
    {
      break;
    }
  }

  cf = (DBL)col / (DBL)it_max;

  return(cf);
}



/*****************************************************************************
*
* FUNCTION
*
*   marble
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL marble (VECTOR EPoint, TPATTERN *TPat)
{
  register DBL turb_val;
  TURB *Turb;

  if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
  {
    turb_val = Turb->Turbulence[X] * Turbulence(EPoint,Turb);
  }
  else
  {
    turb_val = 0.0;
  }

  return(EPoint[X] + turb_val);
}



/*****************************************************************************
*
* FUNCTION
*
*   onion
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Scott Taylor
*
* DESCRIPTION
*
* CHANGES
*   Jul 1991 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL onion (VECTOR EPoint)
{
  /* The variable noise is not used as noise in this function */

  register DBL noise;

/*
   This ramp goes 0-1,1-0,0-1,1-0...

   noise = (fmod(sqrt(Sqr(x)+Sqr(y)+Sqr(z)),2.0)-1.0);

   if (noise<0.0) {noise = 0.0-noise;}
*/

  /* This ramp goes 0-1, 0-1, 0-1, 0-1 ... */

  noise = (fmod(sqrt(Sqr(EPoint[X])+Sqr(EPoint[Y])+Sqr(EPoint[Z])), 1.0));

  return(noise);
}



/*****************************************************************************
*
* FUNCTION
*
*   radial
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Chris Young -- new in vers 2.0
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL radial (VECTOR EPoint)
{
  register DBL value;

  if ((fabs(EPoint[X])<0.001) && (fabs(EPoint[Z])<0.001))
  {
    value = 0.25;
  }
  else
  {
    value = 0.25 + (atan2(EPoint[X],EPoint[Z]) + M_PI) / TWO_M_PI;
  }

  return(value);
}



/*****************************************************************************
*
* FUNCTION
*
*   spiral1
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*   Spiral whirles around z-axis.
*   The number of "arms" is defined in the TPat.
*
* CHANGES
*   Aug 1994 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL spiral1(VECTOR EPoint, TPATTERN *TPat)
{
  DBL rad, phi, turb_val;
  DBL x = EPoint[X];
  DBL y = EPoint[Y];
  DBL z = EPoint[Z];
  TURB *Turb;

  if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
  {
    turb_val = Turb->Turbulence[X] * Turbulence(EPoint,Turb);
  }
  else
  {
    turb_val = 0.0;
  }

  /* Get distance from z-axis. */

  rad = sqrt(x * x + y * y);

  /* Get angle in x,y-plane (0...2 PI). */

  if (rad == 0.0)
  {
    phi = 0.0;
  }
  else
  {
    if (x < 0.0)
    {
      phi = 3.0 * M_PI_2 - asin(y / rad);
    }
    else
    {
      phi = M_PI_2 + asin(y / rad);
    }
  }

  return(z + rad + (DBL)TPat->Vals.Arms * phi / TWO_M_PI + turb_val);
}



/*****************************************************************************
*
* FUNCTION
*
*   spiral2
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*   Spiral whirles around z-axis.
*   The number of "arms" is defined in the TPat.
*
* CHANGES
*   Aug 1994 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/


static DBL spiral2(VECTOR EPoint, TPATTERN *TPat)
{
  DBL rad, phi, turb_val;
  DBL x = EPoint[X];
  DBL y = EPoint[Y];
  DBL z = EPoint[Z];
  TURB *Turb;

  if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
  {
    turb_val = Turb->Turbulence[X] * Turbulence(EPoint,Turb);
  }
  else
  {
    turb_val = 0.0;
  }

  /* Get distance from z-axis. */

  rad = sqrt(x * x + y * y);

  /* Get angle in x,y-plane (0...2 PI) */

  if (rad == 0.0)
  {
    phi = 0.0;
  }
  else
  {
    if (x < 0.0)
    {
      phi = 3.0 * M_PI_2 - asin(y / rad);
    }
    else
    {
      phi = M_PI_2 + asin(y / rad);
    }
  }

  turb_val = Triangle_Wave(z + rad + (DBL)TPat->Vals.Arms * phi / TWO_M_PI +
                           turb_val);

  return(Triangle_Wave(rad) + turb_val);
}



/*****************************************************************************
*
* FUNCTION
*
*   wood
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/


static DBL wood (VECTOR EPoint, TPATTERN *TPat)
{
  register DBL length;
  VECTOR WoodTurbulence;
  VECTOR point;
  DBL x=EPoint[X];
  DBL y=EPoint[Y];
  TURB *Turb;

  if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
  {
    DTurbulence (WoodTurbulence, EPoint,Turb);
    point[X] = cycloidal((x + WoodTurbulence[X]) * Turb->Turbulence[X]);
    point[Y] = cycloidal((y + WoodTurbulence[Y]) * Turb->Turbulence[Y]);
  }
  else
  {
    point[X] = 0.0;
    point[Y] = 0.0;
  }
  point[Z] = 0.0;

  point[X] += x;
  point[Y] += y;

  /* point[Z] += z; Deleted per David Buck --  BP 7/91 */

  VLength (length, point);

  return(length);
}


/*****************************************************************************
*
* FUNCTION
*
*   hexagon
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0, 1.0 or 2.0
*
* AUTHOR
*
*   Ernest MacDougal Campbell III
*   
* DESCRIPTION
*
*   TriHex pattern -- Ernest MacDougal Campbell III (EMC3) 11/23/92
*
*   Creates a hexagon pattern in the XZ plane.
*
*   This algorithm is hard to explain.  First it scales the point to make
*   a few of the later calculations easier, then maps some points to be
*   closer to the Origin.  A small area in the first quadrant is subdivided
*   into a 6 x 6 grid.  The position of the point mapped into that grid
*   determines its color.  For some points, just the grid location is enough,
*   but for others, we have to calculate which half of the block it's in
*   (this is where the atan2() function comes in handy).
*
* CHANGES
*   Nov 1992 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

#define xfactor 0.5;         /* each triangle is split in half for the grid */
#define zfactor 0.866025404; /* sqrt(3)/2 -- Height of an equilateral triangle */

static DBL hexagon (VECTOR EPoint)
{
  int xm, zm;
  int brkindx;
  DBL xs, zs, xl, zl, value = 0.0;
  DBL x=EPoint[X];
  DBL z=EPoint[Z];


  /* Keep all numbers positive.  Also, if z is negative, map it in such a
   * way as to avoid mirroring across the x-axis.  The value 5.196152424
   * is (sqrt(3)/2) * 6 (because the grid is 6 blocks high)
   */

  x = fabs(x);

  /* Avoid mirroring across x-axis. */

  z = z < 0.0 ? 5.196152424 - fabs(z) : z;

  /* Scale point to make calcs easier. */

  xs = x/xfactor;
  zs = z/zfactor;

  /* Map points into the 6 x 6 grid where the basic formula works. */

  xs -= floor(xs/6.0) * 6.0;
  zs -= floor(zs/6.0) * 6.0;

  /* Get a block in the 6 x 6 grid. */

  xm = (int) FLOOR(xs) % 6;
  zm = (int) FLOOR(zs) % 6;

  switch (xm)
  {
    /* These are easy cases: Color depends only on xm and zm. */

    case 0:
    case 5:

      switch (zm)
      {
        case 0:
        case 5: value = 0; break;

        case 1:
        case 2: value = 1; break;

        case 3:
        case 4: value = 2; break;
      }

      break;

    case 2:
    case 3:

      switch (zm)
      {
        case 0:
        case 1: value = 2; break;

        case 2:
        case 3: value = 0; break;

        case 4:
        case 5: value = 1; break;
      }

      break;

    /* These cases are harder.  These blocks are divided diagonally
     * by the angled edges of the hexagons.  Some slope positive, and
     * others negative.  We flip the x value of the negatively sloped
     * pieces.  Then we check to see if the point in question falls
     * in the upper or lower half of the block.  That info, plus the
     * z status of the block determines the color.
     */

    case 1:
    case 4:

      /* Map the point into the block at the origin. */

      xl = xs-xm;
      zl = zs-zm;

      /* These blocks have negative slopes so we flip it horizontally. */

      if (((xm + zm) % 2) == 1)
      {
        xl = 1.0 - xl;
      }

      /* Avoid a divide-by-zero error. */

      if (xl == 0.0)
      {
        xl = 0.0001;
      }

      /* Is the angle less-than or greater-than 45 degrees? */

      brkindx = (zl / xl) < 1.0;

      /* was...
       * brkindx = (atan2(zl,xl) < (45 * M_PI_180));
       * ...but because of the mapping, it's easier and cheaper,
       * CPU-wise, to just use a good ol' slope.
       */

      switch (brkindx)
      {
        case TRUE:

          switch (zm)
          {
            case 0:
            case 3: value = 0; break;

            case 2:
            case 5: value = 1; break;

            case 1:
            case 4: value = 2; break;
          }

          break;

        case FALSE:

          switch (zm)
          {
            case 0:
            case 3: value = 2; break;

            case 2:
            case 5: value = 0; break;

            case 1:
            case 4: value = 1; break;
          }

          break;
      }
  }

  value = fmod(value, 3.0);

  return(value);
}

/* In addition to clipping the value to 
   lie between 0.0 to 1.0, it also fudges 1.0-value.
 */

#define CLIP_DENSITY(r) if((r)<0.0){(r)=1.0;}else{if((r)>1.0){(r)=0.0;}else{(r)=1.0-(r);}}

static DBL planar_pattern (VECTOR EPoint)
{
  register DBL value;

  value = fabs(EPoint[Y]);
  CLIP_DENSITY(value);

  return(value);
}

static DBL spherical (VECTOR EPoint)
{
  register DBL value;

  VLength(value, EPoint);
  CLIP_DENSITY(value);

  return(value);
}

static DBL boxed (VECTOR EPoint)
{
  register DBL value;

  value = max(fabs(EPoint[X]), max(fabs(EPoint[Y]), fabs(EPoint[Z])));
  CLIP_DENSITY(value);

  return(value);
}

static DBL cylindrical (VECTOR EPoint)
{
  register DBL value;

  value = sqrt(Sqr(EPoint[X]) + Sqr(EPoint[Z]));
  CLIP_DENSITY(value);

  return(value);
}



/*****************************************************************************
*
* FUNCTION
*
*   PickInCube(tv, p1)
*
* INPUT
*
*   ?
*
* OUTPUT
*   
* RETURNS
*
*   long integer hash function used, to speed up cacheing.
*   
* AUTHOR
*
*   Jim McElhiney
*   
* DESCRIPTION
*
*   A subroutine to go with crackle.
*
*   Pick a random point in the same unit-sized cube as tv, in a
*   predictable way, so that when called again with another point in
*   the same unit cube, p1 is picked to be the same.
*
* CHANGES
*
******************************************************************************/

static long PickInCube(VECTOR tv, VECTOR  p1)
{
  int seed, temp;
  VECTOR flo;

  /*
   * This uses floor() not FLOOR, so it will not be a mirror
   * image about zero in the range -1.0 to 1.0. The viewer
   * won't see an artefact around the origin.
   */

  flo[X] = floor(tv[X] - EPSILON);
  flo[Y] = floor(tv[Y] - EPSILON);
  flo[Z] = floor(tv[Z] - EPSILON);

  seed = Hash3d((int)flo[X], (int)flo[Y], (int)flo[Z]);

  temp = POV_GET_OLD_RAND(); /* save current seed */
  
  POV_SRAND(seed);

  p1[X] = flo[X] + FRAND();
  p1[Y] = flo[Y] + FRAND();
  p1[Z] = flo[Z] + FRAND();

  POV_SRAND(temp);  /* restore */

  return((long)seed);
}



/*****************************************************************************
*
* FUNCTION
*
*   Evaluate_Pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*   
* OUTPUT
*   
* RETURNS
*
*   DBL result usual 0.0 to 1.0 but may be 2.0 in hexagon
*   
* AUTHOR
*
*   adapted from Add_Pigment by Chris Young
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Evaluate_TPat (TPATTERN *TPat, VECTOR EPoint)
{
  DBL value = 0.0;
  VECTOR TPoint;

  Warp_EPoint (TPoint, EPoint, TPat);

  switch (TPat->Type)
  {
    case AGATE_PATTERN:    value = agate    (TPoint, TPat);   break;

    case BOZO_PATTERN:
    case SPOTTED_PATTERN:
    case BUMPS_PATTERN:    value = Noise    (TPoint);         break;

    case BRICK_PATTERN:    value = brick    (TPoint, TPat);   break;
    case CHECKER_PATTERN:  value = checker  (TPoint);         break;
    case CRACKLE_PATTERN:  value = crackle  (TPoint);         break;
    case GRADIENT_PATTERN: value = gradient (TPoint, TPat);   break;
    case GRANITE_PATTERN:  value = granite  (TPoint);         break;
    case HEXAGON_PATTERN:  value = hexagon  (TPoint);         break;
    case LEOPARD_PATTERN:  value = leopard  (TPoint);         break;
    case MANDEL_PATTERN:   value = mandel   (TPoint, TPat);   break;
    case MARBLE_PATTERN:   value = marble   (TPoint, TPat);   break;
    case ONION_PATTERN:    value = onion    (TPoint);         break;
    case RADIAL_PATTERN:   value = radial   (TPoint);         break;
    case SPIRAL1_PATTERN:  value = spiral1  (TPoint, TPat);   break;
    case SPIRAL2_PATTERN:  value = spiral2  (TPoint, TPat);   break;
    case WOOD_PATTERN:     value = wood     (TPoint, TPat);   break;

    case WAVES_PATTERN:    value = waves_pigm    (TPoint, TPat);   break;
    case RIPPLES_PATTERN:  value = ripples_pigm  (TPoint, TPat);   break;
    case WRINKLES_PATTERN: value = wrinkles_pigm (TPoint);   break;
    case DENTS_PATTERN:    value = dents_pigm    (TPoint);   break;
    case QUILTED_PATTERN:  value = quilted_pigm  (TPoint, TPat);   break;

    case PLANAR_PATTERN:      value = planar_pattern (TPoint);      break;
    case BOXED_PATTERN:       value = boxed          (TPoint);      break;
    case SPHERICAL_PATTERN:   value = spherical      (TPoint);      break;
    case CYLINDRICAL_PATTERN: value = cylindrical    (TPoint);      break;
    case DENSITY_FILE_PATTERN:value = density_file (TPoint, TPat);  break;


    default: Error("Problem in Evaluate_TPat.");
  }

  if (TPat->Frequency !=0.0)
  {
    value = fmod(value * TPat->Frequency + TPat->Phase, 1.00001);
  }

  /* allow negative Frequency */

  if (value < 0.0)
  {
    value -= floor(value);
  }

  switch (TPat->Wave_Type)
  {
    case RAMP_WAVE:
      break;

    case SINE_WAVE:
      value = (1.0+cycloidal(value))*0.5;
      break;

    case TRIANGLE_WAVE:
      value = Triangle_Wave(value);
      break;

    case SCALLOP_WAVE:
      value = fabs(cycloidal(value*0.5));
      break;

    case CUBIC_WAVE:
      value = Sqr(value)*((-2.0 * value) + 3.0);
      break;

    case POLY_WAVE:
      value = pow(value, TPat->Exponent);
      break;

    default: Error("Unknown Wave Type %d.",TPat->Wave_Type);
   }

  return(value);
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Init_TPat_Fields (TPATTERN *Tpat)
{
  Tpat->Type       = NO_PATTERN;
  Tpat->Wave_Type  = RAMP_WAVE;
  Tpat->Flags      = NO_FLAGS;
  Tpat->References = 1;
  Tpat->Exponent   = 1.0;
  Tpat->Frequency  = 1.0;
  Tpat->Phase      = 0.0;
  Tpat->Warps      = NULL;
  Tpat->Next       = NULL;
  Tpat->Blend_Map  = NULL;
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Copy_TPat_Fields (TPATTERN *New, TPATTERN  *Old)
{
  *New = *Old;
  
  /* Copy warp chain */
  New->Warps = Copy_Warps(Old->Warps);

  New->Blend_Map = Copy_Blend_Map(Old->Blend_Map);

  /* Note, cannot copy Old->Next because we don't know what kind of
     thing this is.  It must be copied by Copy_Pigment, Copy_Tnormal etc.
  */

  if (Old->Type == BITMAP_PATTERN)
  {
     New->Vals.Image = Copy_Image(Old->Vals.Image);
  }
  if (Old->Type == DENSITY_FILE_PATTERN)
  {
     New->Vals.Density_File = Copy_Density_File(Old->Vals.Density_File);
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Destroy_TPat_Fields(TPATTERN *Tpat)
{
  Destroy_Warps(Tpat->Warps);
  Destroy_Blend_Map(Tpat->Blend_Map);
  /* Note, cannot destroy Tpat->Next nor pattern itself because we don't
     know what kind of thing this is.  It must be destroied by Destroy_Pigment, etc.
  */

  if (Tpat->Type == BITMAP_PATTERN)
  {
     Destroy_Image(Tpat->Vals.Image);
  }

  if (Tpat->Type == DENSITY_FILE_PATTERN)
  {
     Destroy_Density_File(Tpat->Vals.Density_File);
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TURB *Create_Turb()
{
  TURB *New;

  New = (TURB *)POV_MALLOC(sizeof(TURB),"turbulence struct");

  Make_Vector(New->Turbulence, 0.0, 0.0, 0.0);

  New->Octaves = 6;
  New->Omega = 0.5;
  New->Lambda = 2.0;

  return(New);
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

#if 0   /* Unused function [AED] */
static TURB *Copy_Turb(TURB *Old)
{
  TURB *New;

  if (Old != NULL)
  {
    New = Create_Turb();

    *New = *Old;
  }
  else
  {
    New=NULL;
  }

  return(New);
}
#endif


/*****************************************************************************
*
* FUNCTION
*
*   Translate_Tpattern
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
* CHANGES
*
******************************************************************************/

void Translate_Tpattern(TPATTERN *Tpattern,VECTOR Vector)
{
  TRANSFORM Trans;

  if (Tpattern != NULL)
  {
    Compute_Translation_Transform (&Trans, Vector);

    Transform_Tpattern (Tpattern, &Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Tpattern
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
* CHANGES
*
******************************************************************************/

void Rotate_Tpattern(TPATTERN *Tpattern,VECTOR Vector)
{
  TRANSFORM Trans;

  if (Tpattern != NULL)
  {
    Compute_Rotation_Transform (&Trans, Vector);

    Transform_Tpattern (Tpattern, &Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Tpattern
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
* CHANGES
*
******************************************************************************/

void Scale_Tpattern(TPATTERN *Tpattern,VECTOR Vector)
{
  TRANSFORM Trans;

  if (Tpattern != NULL)
  {
    Compute_Scaling_Transform (&Trans, Vector);

    Transform_Tpattern (Tpattern, &Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Tpattern
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
* CHANGES
*
******************************************************************************/

void Transform_Tpattern(TPATTERN *Tpattern,TRANSFORM *Trans)
{
  WARP *Temp;

  if (Tpattern != NULL)
  {
    if (Tpattern->Warps == NULL)
    {
      Tpattern->Warps=Create_Warp(TRANSFORM_WARP);
    }
    else
    {
      if (Tpattern->Warps->Warp_Type != TRANSFORM_WARP)
      {
        Temp=Tpattern->Warps;

        Tpattern->Warps=Create_Warp(TRANSFORM_WARP);

        Tpattern->Warps->Next_Warp=Temp;
      }
    }

    Compose_Transforms (&( ((TRANS *)(Tpattern->Warps))->Trans), Trans);
  }
}


/*****************************************************************************
*
* FUNCTION
*
*   ripples_pigm
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/

static DBL ripples_pigm (VECTOR EPoint, TPATTERN *TPat)
{
  register unsigned int i;
  register DBL length, index;
  DBL scalar =0.0;
  VECTOR point;

  for (i = 0 ; i < Number_Of_Waves ; i++)
  {
    VSub (point, EPoint, Wave_Sources[i]);
    VLength (length, point);

    if (length == 0.0)
      length = 1.0;

    index = length * TPat->Frequency + TPat->Phase;

    scalar += cycloidal(index);
  }

  scalar = 0.5*(1.0+(scalar / (DBL)Number_Of_Waves));

  return(scalar);
}


/*****************************************************************************
*
* FUNCTION
*
*   waves_pigm
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/

static DBL waves_pigm (VECTOR EPoint, TPATTERN *TPat)
{
  register unsigned int i;
  register DBL length, index;
  DBL scalar = 0.0;
  VECTOR point;

  for (i = 0 ; i < Number_Of_Waves ; i++)
  {
    VSub (point, EPoint, Wave_Sources[i]);
    VLength (length, point);

    if (length == 0.0)
    {
      length = 1.0;
    }

    index = length * TPat->Frequency * frequency[i] + TPat->Phase;

    scalar += cycloidal(index)/frequency[i];
  }

  scalar = 0.2*(2.5+(scalar / (DBL)Number_Of_Waves));

  return(scalar);
}


/*****************************************************************************
*
* FUNCTION
*
*   dents_pigm
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/

static DBL dents_pigm (VECTOR EPoint)
{
  DBL noise;

  noise = Noise (EPoint);

  return(noise * noise * noise);
}



/*****************************************************************************
*
* FUNCTION
*
*   wrinkles_pigm
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/


static DBL wrinkles_pigm (VECTOR EPoint)
{
  register int i;
  DBL lambda = 2.0;
  DBL omega = 0.5;
  DBL value;
  VECTOR temp;

  value = Noise(EPoint);

  for (i = 1; i < 10; i++)
  {
    VScale(temp,EPoint,lambda);

    value += omega * Noise(temp);

    lambda *= 2.0;

    omega *= 0.5;
  }

  return(value/2.0);
}



/*****************************************************************************
*
* FUNCTION
*
*   quilted_pigm
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dan Farmer & Chris Young
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static DBL quilted_pigm (VECTOR EPoint, TPATTERN *TPat)
{
  VECTOR value;
  DBL t;

  value[X] = EPoint[X]-FLOOR(EPoint[X])-0.5;
  value[Y] = EPoint[Y]-FLOOR(EPoint[Y])-0.5;
  value[Z] = EPoint[Z]-FLOOR(EPoint[Z])-0.5;

  t = sqrt(value[X]*value[X]+value[Y]*value[Y]+value[Z]*value[Z]);

  t = quilt_cubic(t, TPat->Vals.Quilted.Control0, TPat->Vals.Quilted.Control1);

  value[X] *= t;
  value[Y] *= t;
  value[Z] *= t;

  return((fabs(value[X])+fabs(value[Y])+fabs(value[Z]))/3.0);
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

#define INV_SQRT_3_4 1.154700538
DBL quilt_cubic(DBL t,DBL p1,DBL p2)
{
 DBL it=(1-t);
 DBL itsqrd=it*it;
/* DBL itcubed=it*itsqrd; */
 DBL tsqrd=t*t;
 DBL tcubed=t*tsqrd;
 DBL val;

/* Originally coded as...

 val= (DBL)(itcubed*n1+(tcubed)*n2+3*t*(itsqrd)*p1+3*(tsqrd)*(it)*p2);

 re-written by CEY to optimise because n1=0 n2=1 always.
 
*/

 val = (tcubed + 3.0*t*itsqrd*p1 + 3.0*tsqrd*it*p2) * INV_SQRT_3_4;
 
 return(val);
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Search_Blend_Map (DBL value,BLEND_MAP *Blend_Map,BLEND_MAP_ENTRY **Prev,BLEND_MAP_ENTRY  **Cur)
{
  BLEND_MAP_ENTRY *P, *C;
  int Max_Ent=Blend_Map->Number_Of_Entries-1;

  /* if greater than last, use last. */

  if (value >= Blend_Map->Blend_Map_Entries[Max_Ent].value)
  {
    P = C = &(Blend_Map->Blend_Map_Entries[Max_Ent]);
  }
  else
  {
    P = C = &(Blend_Map->Blend_Map_Entries[0]);

    while (value > C->value)
    {
      P = C++;
    }
  }

  if (value == C->value)
  {
    P = C;
  }

  *Prev = P;
  *Cur  = C;
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static TURB *Search_For_Turb(WARP *Warps)
{
  WARP* Temp=Warps;

  if (Temp!=NULL)
  {
    while (Temp->Next_Warp != NULL)
    {
      Temp=Temp->Next_Warp;
    }

    if (Temp->Warp_Type != CLASSIC_TURB_WARP)
    {
       Temp=NULL;
    }
  }

  return ((TURB *)Temp);
}


/*****************************************************************************
*
* FUNCTION
*
*   density_file
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

static DBL density_file(VECTOR EPoint, TPATTERN *TPat)
{
  int x, y, z;
  int x1, y1, z1;
  int x2, y2, z2;
  DBL xx, yy, zz;
  DBL xi, yi, zi;
  DBL f111, f112, f121, f122, f211, f212, f221, f222;
  DBL density = 0.0;
  DENSITY_FILE_DATA *Data;

  if ((TPat->Vals.Density_File != NULL) &&
      ((Data = TPat->Vals.Density_File->Data) != NULL))
  {
    if ((EPoint[X] >= 0.0) && (EPoint[X] < 1.0) &&
        (EPoint[Y] >= 0.0) && (EPoint[Y] < 1.0) &&
        (EPoint[Z] >= 0.0) && (EPoint[Z] < 1.0))
    {
      switch (TPat->Vals.Density_File->Interpolation)
      {
        case NO_INTERPOLATION:

          x = (int)(EPoint[X] * (DBL)Data->Sx);
          y = (int)(EPoint[Y] * (DBL)Data->Sy);
          z = (int)(EPoint[Z] * (DBL)Data->Sz);

          if ((x < 0) || (x >= Data->Sx) ||
              (y < 0) || (y >= Data->Sy) ||
              (z < 0) || (z >= Data->Sz))
          {
            density = 0.0;
          }
          else
          {
            density = (DBL)Data->Density[z][y][x] / 255.0;
          }

          break;

        case TRILINEAR_INTERPOLATION:

          xx = EPoint[X] * (DBL)(Data->Sx - 1);
          yy = EPoint[Y] * (DBL)(Data->Sy - 1);
          zz = EPoint[Z] * (DBL)(Data->Sz - 1);

          x1 = (int)xx;
          y1 = (int)yy;
          z1 = (int)zz;

          x2 = x1 + 1;
          y2 = y1 + 1;
          z2 = z1 + 1;

          xx -= floor(xx);
          yy -= floor(yy);
          zz -= floor(zz);

          xi = 1.0 - xx;
          yi = 1.0 - yy;
          zi = 1.0 - zz;

          f111 = (DBL)Data->Density[z1][y1][x1] / 255.0;
          f112 = (DBL)Data->Density[z1][y1][x2] / 255.0;
          f121 = (DBL)Data->Density[z1][y2][x1] / 255.0;
          f122 = (DBL)Data->Density[z1][y2][x2] / 255.0;
          f211 = (DBL)Data->Density[z2][y1][x1] / 255.0;
          f212 = (DBL)Data->Density[z2][y1][x2] / 255.0;
          f221 = (DBL)Data->Density[z2][y2][x1] / 255.0;
          f222 = (DBL)Data->Density[z2][y2][x2] / 255.0;

          density = f111 * zi * yi * xi +
                    f112 * zi * yi * xx +
                    f121 * zi * yy * xi +
                    f122 * zi * yy * xx +
                    f211 * zz * yi * xi +
                    f212 * zz * yi * xx +
                    f221 * zz * yy * xi +
                    f222 * zz * yy * xx;

          break;
      }
    }
    else
    {
      density = 0.0;
    }

/*
    fprintf(stderr, "x = %3d, y = %3d, z = %3d, density = %5.4f\n", x, y, z, density);
*/
  }
  return(density);
}


/*****************************************************************************
*
* FUNCTION
*
*   Create_Density_File
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
*   Create a density file structure.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

DENSITY_FILE *Create_Density_File()
{
  DENSITY_FILE *New;

  New = (DENSITY_FILE *)POV_MALLOC(sizeof(DENSITY_FILE), "density file");

  New->Interpolation = NO_INTERPOLATION;

  New->Data = (DENSITY_FILE_DATA *)POV_MALLOC(sizeof(DENSITY_FILE_DATA), "density file data");

  New->Data->References = 1;

  New->Data->Name = NULL;

  New->Data->Sx =
  New->Data->Sy =
  New->Data->Sz = 0;

  New->Data->Density = NULL;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Density_File
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
*   Copy a density file structure.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

DENSITY_FILE *Copy_Density_File(DENSITY_FILE *Old)
{
  DENSITY_FILE *New;

  if (Old != NULL)
  {
    New = (DENSITY_FILE *)POV_MALLOC(sizeof(DENSITY_FILE), "density file");

    *New = *Old;

    New->Data->References++;
  }
  else          /* tw */
    New = NULL; /* tw */
    
  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Density_File
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
*   Destroy a density file structure.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

void Destroy_Density_File(DENSITY_FILE *Density_File)
{
  int y, z;

  if (Density_File != NULL)
  {
    if ((--(Density_File->Data->References)) == 0)
    {
      POV_FREE(Density_File->Data->Name);

      for (z = 0; z < Density_File->Data->Sz; z++)
      {
        for (y = 0; y < Density_File->Data->Sy; y++)
        {
          POV_FREE(Density_File->Data->Density[z][y]);
        }

        POV_FREE(Density_File->Data->Density[z]);
      }
      POV_FREE(Density_File->Data->Density);
      POV_FREE(Density_File->Data);
    }

    POV_FREE(Density_File);
  }
}


void Read_Density_File(DENSITY_FILE *df)
{
  unsigned char ***map;
  int y, z, sx, sy, sz;
  FILE *file;

  if (df == NULL)
  {
     return;
  }
  
  /* Allocate and read density file. */

  if ((df != NULL) && (df->Data->Name != NULL))
  {
    if ((file = Locate_File(df->Data->Name, READ_BINFILE_STRING, ".df3", ".DF3",NULL,TRUE)) == NULL)
    {
      Error("Cannot read media density file.\n");
    }
    
    sx = df->Data->Sx = readushort(file);
    sy = df->Data->Sy = readushort(file);
    sz = df->Data->Sz = readushort(file);

    map = (unsigned char ***)POV_MALLOC(sz*sizeof(unsigned char **), "media density file data");

    for (z = 0; z < sz; z++)
    {
      map[z] = (unsigned char **)POV_MALLOC(sy*sizeof(unsigned char *), "media density file data");

      for (y = 0; y < sy; y++)
      {
        map[z][y] = (unsigned char *)POV_MALLOC(sx*sizeof(unsigned char), "media density file data");

        fread(map[z][y], sizeof(unsigned char), (size_t)sx, file);
      }
    }

    df->Data->Density = map;

    if (file != NULL)		/* -hdf99- */
    {
      fclose(file);
    }

  }
}

static unsigned short readushort(FILE *infile)
{
  int i0, i1 = 0; /* To quiet warnings */

  if ((i0  = fgetc(infile)) == EOF || (i1  = fgetc(infile)) == EOF)
  {
    Error("Error reading density_file\n");
  }

  return (unsigned short)((((unsigned short)i0) << 8) | ((unsigned short)i1));
}

