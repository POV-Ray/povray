/****************************************************************************
*                normal.c
*
*  This module implements solid texturing functions that perturb the surface
*  normal to create a bumpy effect. 
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

/*
 * Some texture ideas garnered from SIGGRAPH '85 Volume 19 Number 3,
 * "An Image Synthesizer" By Ken Perlin.
 *
 * Further Ideas Garnered from "The RenderMan Companion" (Addison Wesley)
 */

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "texture.h"
#include "image.h"
#include "matrices.h"
#include "normal.h"
#include "povray.h"
#include "txttest.h"
#include "pigment.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local constants
******************************************************************************/

static CONST
VECTOR Pyramid_Vect [4]= {{ 0.942809041,-0.333333333, 0.0},
                          {-0.471404521,-0.333333333, 0.816496581},
                          {-0.471404521,-0.333333333,-0.816496581},
                          { 0.0        , 1.0        , 0.0}};


/*****************************************************************************
* Static functions
******************************************************************************/

static void ripples (VECTOR EPoint, TNORMAL *Tnormal, VECTOR Vector);
static void waves (VECTOR EPoint, TNORMAL *Tnormal, VECTOR Vector);
static void bumps (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal);
static void dents (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal);
static void wrinkles (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal);
static void quilted (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal);
static DBL Hermite_Cubic (DBL T1,UV_VECT UV1,UV_VECT UV2);
static DBL Do_Slope_Map (DBL value, BLEND_MAP *Blend_Map);
static void Do_Average_Normals (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal);


/*****************************************************************************
*
* FUNCTION
*
*   ripples
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

static void ripples (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal)
{
  register unsigned int i;
  register DBL length, scalar, index;
  VECTOR point;

  for (i = 0 ; i < Number_Of_Waves ; i++)
  {
    VSub (point, EPoint, Wave_Sources[i]);
    VLength (length, point);

    if (length == 0.0)
      length = 1.0;

    index = length * Tnormal->Frequency + Tnormal->Phase;

    scalar = cycloidal(index) * Tnormal ->Amount;

    VAddScaledEq(normal, scalar / (length * (DBL)Number_Of_Waves), point);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   waves
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

static void waves (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal)
{
  register unsigned int i;
  register DBL length, scalar, index, sinValue ;
  VECTOR point;

  for (i = 0 ; i < Number_Of_Waves ; i++)
  {
    VSub (point, EPoint, Wave_Sources[i]);

    VLength (length, point);

    if (length == 0.0)
    {
      length = 1.0;
    }

    index = length * Tnormal->Frequency * frequency[i] + Tnormal->Phase;

    sinValue = cycloidal(index);

    scalar = sinValue * Tnormal->Amount / frequency[i];

    VAddScaledEq(normal, scalar / (length * (DBL)Number_Of_Waves), point);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   bumps
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

static void bumps (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal)
{
  VECTOR bump_turb;

  /* Get normal displacement value. */

  DNoise (bump_turb, EPoint);

  /* Displace "normal". */

  VAddScaledEq(normal, Tnormal->Amount, bump_turb);
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
*   POV-Ray Team
*
* DESCRIPTION
*   Dents is similar to bumps, but uses noise() to control the amount of
*   dnoise() perturbation of the object normal...
*
* CHANGES
*
******************************************************************************/

static void dents (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal)
{
  DBL noise;
  VECTOR stucco_turb;

  noise = Noise (EPoint);

  noise = noise * noise * noise * Tnormal->Amount;

  /* Get normal displacement value. */

  DNoise(stucco_turb, EPoint);

  /* Displace "normal". */

  VAddScaledEq(normal, noise, stucco_turb);
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
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Wrinkles - This is my implementation of the dented() routine, using
*   a surface iterative fractal derived from DTurbulence.
*
*   This is a 3-D version (thanks to DNoise()...) of the usual version
*   using the singular Noise()...
*
*   Seems to look a lot like wrinkles, however... (hmmm)
*
*   Idea garnered from the April 89 Byte Graphics Supplement on RenderMan,
*   refined from "The RenderMan Companion, by Steve Upstill of Pixar,
*   (C) 1990 Addison-Wesley.
*
* CHANGES
*
******************************************************************************/

static void wrinkles (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal)
{
  register int i;
  register DBL scale = 1.0;
  VECTOR result, value, value2;

  Make_Vector(result, 0.0, 0.0, 0.0);

  for (i = 0; i < 10; scale *= 2.0, i++)
  {
    VScale(value2,EPoint,scale);
    DNoise(value, value2);

    result[X] += fabs(value[X] / scale);
    result[Y] += fabs(value[Y] / scale);
    result[Z] += fabs(value[Z] / scale);
  }

  /* Displace "normal". */

  VAddScaledEq(normal, Tnormal->Amount, result);
}


/*****************************************************************************
*
* FUNCTION
*
*   quilted
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dan Farmer '94
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void quilted (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal)
{
  VECTOR value;
  DBL t;

  value[X] = EPoint[X]-FLOOR(EPoint[X])-0.5;
  value[Y] = EPoint[Y]-FLOOR(EPoint[Y])-0.5;
  value[Z] = EPoint[Z]-FLOOR(EPoint[Z])-0.5;

  t = sqrt(value[X]*value[X]+value[Y]*value[Y]+value[Z]*value[Z]);

  t = quilt_cubic(t, Tnormal->Vals.Quilted.Control0, Tnormal->Vals.Quilted.Control1);

  value[X] *= t;
  value[Y] *= t;
  value[Z] *= t;

  VAddScaledEq (normal, Tnormal->Amount,value);
}

/*****************************************************************************
*
* FUNCTION
*
*   Create_Tnormal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*
*   pointer to the created Tnormal
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION   : Allocate memory for new Tnormal and initialize it to
*                 system default values.
*
* CHANGES
*
******************************************************************************/


TNORMAL *Create_Tnormal ()
{
  TNORMAL *New;

  New = (TNORMAL *)POV_MALLOC(sizeof(TNORMAL), "normal");

  Init_TPat_Fields((TPATTERN *)New);

  New->Amount = 0.5;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Tnormal
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

TNORMAL *Copy_Tnormal (TNORMAL *Old)
{
  TNORMAL *New;

  if (Old != NULL)
  {
    New = Create_Tnormal();

    Copy_TPat_Fields ((TPATTERN *)New, (TPATTERN *)Old);

    New->Amount = Old->Amount;
  }
  else
  {
    New = NULL;
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Tnormal
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

void Destroy_Tnormal(TNORMAL *Tnormal)
{
  if (Tnormal != NULL)
  {
    Destroy_TPat_Fields ((TPATTERN *)Tnormal);

    POV_FREE(Tnormal);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Post_Tnormal
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

void Post_Tnormal (TNORMAL *Tnormal)
{
  int i;
  BLEND_MAP *Map;

  if (Tnormal != NULL)
  {
    if (Tnormal->Flags & POST_DONE)
    {
      return;
    }

    if (Tnormal->Type == NO_PATTERN)
    {
      Error("No normal type given.");
    }

    Tnormal->Flags |= POST_DONE;

    if ((Map = Tnormal->Blend_Map) != NULL)
    {
      for (i = 0; i < Map->Number_Of_Entries; i++)
      {
        switch (Map->Type)
        {
          case PIGMENT_TYPE:

            Post_Pigment(Map->Blend_Map_Entries[i].Vals.Pigment);

            break;

          case NORMAL_TYPE:

            Post_Tnormal(Map->Blend_Map_Entries[i].Vals.Tnormal);

            break;

          case TEXTURE_TYPE:

            Post_Textures(Map->Blend_Map_Entries[i].Vals.Texture);

            break;

          case SLOPE_TYPE:
          case COLOUR_TYPE:
          case PATTERN_TYPE:

            break;

          default:

            Error("Unknown pattern type in Post_Tnormal.");
        }
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Perturb_Normal
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

#define DELTA 0.02

void Perturb_Normal(VECTOR Layer_Normal, TNORMAL *Tnormal, VECTOR  EPoint)
{
  VECTOR TPoint,P1;
  DBL value1,value2,Amount;
  int i;
  BLEND_MAP *Blend_Map;
  BLEND_MAP_ENTRY *Prev, *Cur;
  
  if (Tnormal==NULL)
  {
    return;
  }

  /* If normal_map present, use it and return */

  if ((Blend_Map=Tnormal->Blend_Map) != NULL)
  {
    if ((Blend_Map->Type == NORMAL_TYPE) && (Tnormal->Type != AVERAGE_PATTERN))
    {
      value1 = Evaluate_TPat((TPATTERN *)Tnormal,EPoint);

      Search_Blend_Map (value1,Blend_Map,&Prev,&Cur);
      
      Assign_Vector(P1,Layer_Normal);

      Warp_EPoint (TPoint, EPoint, (TPATTERN *)Tnormal);
      Perturb_Normal(Layer_Normal,Cur->Vals.Tnormal,TPoint);

      if (Prev != Cur)
      {
        Perturb_Normal(P1,Prev->Vals.Tnormal,TPoint);

        value2 = (value1-Prev->value)/(Cur->value-Prev->value);
        value1 = 1.0-value2;

        VLinComb2(Layer_Normal,value1,P1,value2,Layer_Normal)
      }

      VNormalizeEq(Layer_Normal);

      return;
    }
  }
  
  /* No normal_map. */

  if (Tnormal->Type <= LAST_NORM_ONLY_PATTERN)
  {
    Warp_EPoint (TPoint, EPoint, (TPATTERN *)Tnormal);
    switch (Tnormal->Type)
      {
       case BITMAP_PATTERN: bump_map (TPoint, Tnormal, Layer_Normal); break;
       case BUMPS_PATTERN:  bumps (TPoint, Tnormal, Layer_Normal);    break;
       case DENTS_PATTERN:  dents (TPoint, Tnormal, Layer_Normal);    break;
       case RIPPLES_PATTERN:ripples (TPoint, Tnormal, Layer_Normal);  break;
       case WAVES_PATTERN:  waves (TPoint, Tnormal, Layer_Normal);    break;
       case WRINKLES_PATTERN:wrinkles (TPoint, Tnormal, Layer_Normal);break;
       case QUILTED_PATTERN:quilted (TPoint, Tnormal, Layer_Normal);  break;
       case AVERAGE_PATTERN: Do_Average_Normals (TPoint, Tnormal, Layer_Normal);  break;
       default:
         Error("Normal pattern not yet implemented.");
      }
  }
  else
  {
    Amount=Tnormal->Amount * -5.0; /*fudge factor*/
    
/* Note, even though DELTA and Pyramid_Vect are constants, we may later
   make DELTA a user-defined parameter.  Good optimising compilers
   should merge the constants anyway. */
   
    for(i=0; i<=3; i++)
    {
      VAddScaled(P1,EPoint,DELTA,Pyramid_Vect[i]);
      value1 = Do_Slope_Map(Evaluate_TPat((TPATTERN *)Tnormal,P1),Blend_Map);
      VAddScaledEq(Layer_Normal,value1*Amount,Pyramid_Vect[i]);
    }

  }

  VNormalizeEq(Layer_Normal);
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

static DBL Do_Slope_Map (DBL value,BLEND_MAP *Blend_Map)
{
  DBL Result;
  BLEND_MAP_ENTRY *Prev, *Cur;

  if (Blend_Map == NULL)
  {
    return(value);
  }

  Search_Blend_Map (value,Blend_Map,&Prev,&Cur);

  if (Prev == Cur)
  {
     return(Cur->Vals.Point_Slope[0]);
  }

  Result = (value-Prev->value)/(Cur->value-Prev->value);

  return(Hermite_Cubic(Result,Prev->Vals.Point_Slope,Cur->Vals.Point_Slope));
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

#define S1 UV1[1]
#define S2 UV2[1]
#define P1 UV1[0]
#define P2 UV2[0]

static DBL Hermite_Cubic(DBL T1,UV_VECT UV1,UV_VECT UV2)
{
  DBL TT=T1*T1;
  DBL TTT=TT*T1;
  DBL rv;        /* simplified equation for poor Symantec */

  rv  = TTT*(S1+S2+2.0*(P1-P2));
  rv += -TT*(2.0*S1+S2+3.0*(P1-P2));
  rv += T1*S1 +P1;

  return (rv);
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

static void Do_Average_Normals (VECTOR EPoint,TNORMAL *Tnormal,VECTOR normal)
{
   int i;
   BLEND_MAP *Map = Tnormal->Blend_Map;
   SNGL Value;
   SNGL Total = 0.0;
   VECTOR V1,V2;
   
   Make_Vector (V1, 0.0, 0.0, 0.0);

   for (i = 0; i < Map->Number_Of_Entries; i++)
   {
     Value = Map->Blend_Map_Entries[i].value;
     
     Assign_Vector(V2,normal);

     Perturb_Normal(V2,Map->Blend_Map_Entries[i].Vals.Tnormal,EPoint);
     
     VAddScaledEq(V1,Value,V2);

     Total += Value;
   }

   VInverseScale(normal,V1,Total);
}
