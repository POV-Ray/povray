/****************************************************************************
*                point.c
*
*  This module implements the point & spot light source primitive.
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
#include "vector.h"
#include "povproto.h"
#include "point.h"
#include "matrices.h"
#include "objects.h"
#include "povray.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static DBL cubic_spline ( DBL low,DBL high,DBL pos);
static int  All_Light_Source_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int  Inside_Light_Source (VECTOR point, OBJECT *Object);
static void Light_Source_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static void Translate_Light_Source (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Light_Source (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Light_Source (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Light_Source (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Light_Source (OBJECT *Object);
static LIGHT_SOURCE *Copy_Light_Source (OBJECT *Object);
static void Destroy_Light_Source (OBJECT *Object);

/*****************************************************************************
* Local variables
******************************************************************************/

static METHODS Light_Source_Methods =
{
  All_Light_Source_Intersections,
  Inside_Light_Source, Light_Source_Normal,
  (COPY_METHOD)Copy_Light_Source,
  Translate_Light_Source, Rotate_Light_Source,
  Scale_Light_Source, Transform_Light_Source, Invert_Light_Source,
  Destroy_Light_Source
};





/*****************************************************************************
*
* FUNCTION
*
*   All_Light_Source_Intersections
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

static int All_Light_Source_Intersections (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  if (((LIGHT_SOURCE *)Object)->Children != NULL)
  {
    if (Ray_In_Bound (Ray, ((LIGHT_SOURCE *)Object)->Children->Bound))
    {
      if (All_Intersections (((LIGHT_SOURCE *)Object)->Children, Ray, Depth_Stack))
      {
        return(TRUE);
      }
    }
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Light_Source
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

static int Inside_Light_Source (VECTOR IPoint, OBJECT *Object)
{
  if (((LIGHT_SOURCE *)Object)->Children != NULL)
  {
    if (Inside_Object (IPoint, ((LIGHT_SOURCE *)Object)->Children))
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
*   Light_Source_Normal
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

static void Light_Source_Normal (VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  if (((LIGHT_SOURCE *)Object)->Children != NULL)
  {
    Normal (Result, ((LIGHT_SOURCE *)Object)->Children,Inter);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Light_Source
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

static void Translate_Light_Source (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  LIGHT_SOURCE *Light = (LIGHT_SOURCE *)Object;

  VAddEq (Light->Center, Vector);
  VAddEq (Light->Points_At, Vector);

  if (Light->Children != NULL)
  {
    Translate_Object (Light->Children, Vector, Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Light_Source
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

static void Rotate_Light_Source (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Light_Source(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Light_Source
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

static void Scale_Light_Source (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  Transform_Light_Source(Object, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Light_Source
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

static void Transform_Light_Source (OBJECT *Object, TRANSFORM *Trans)
{
  DBL len;
  LIGHT_SOURCE *Light = (LIGHT_SOURCE *)Object;

  MTransPoint (Light->Center,    Light->Center,    Trans);
  MTransPoint (Light->Points_At, Light->Points_At, Trans);
  MTransPoint (Light->Axis1,     Light->Axis1,     Trans);
  MTransPoint (Light->Axis2,     Light->Axis2,     Trans);

  MTransDirection (Light->Direction, Light->Direction, Trans);

  /* Make sure direction has unit length. */

  VLength(len, Light->Direction);

  if (len > EPSILON)
  {
    VInverseScaleEq(Light->Direction, len);
  }

  if (Light->Children != NULL)
  {
    Transform_Object (Light->Children, Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Light_Source
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

static void Invert_Light_Source (OBJECT *Object)
{
  LIGHT_SOURCE *Light = (LIGHT_SOURCE *)Object;

  if (Light->Children != NULL)
  {
    Invert_Object (Light->Children);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Light_Source
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

LIGHT_SOURCE *Create_Light_Source ()
{
  int i;
  LIGHT_SOURCE *New;

  New = (LIGHT_SOURCE *)POV_MALLOC(sizeof (LIGHT_SOURCE), "light_source");

  INIT_OBJECT_FIELDS(New, LIGHT_OBJECT, &Light_Source_Methods)

  New->Children = NULL;

  Set_Flag(New, NO_SHADOW_FLAG);

  Make_Colour(New->Colour,    1.0, 1.0, 1.0);
  Make_Vector(New->Direction, 0.0, 0.0, 0.0);
  Make_Vector(New->Center,    0.0, 0.0, 0.0);
  Make_Vector(New->Points_At, 0.0, 0.0, 1.0);
  Make_Vector(New->Axis1,     0.0, 0.0, 1.0);
  Make_Vector(New->Axis2,     0.0, 1.0, 0.0);

  New->Coeff   = 10.0;
  New->Radius  = 0.35;
  New->Falloff = 0.35;

  New->Fade_Distance = 0.0;
  New->Fade_Power    = 0.0;

  New->Next_Light_Source    = NULL;
  New->Light_Grid           = NULL;
  New->Shadow_Cached_Object = NULL;

  New->Light_Type = POINT_SOURCE;

  New->Area_Light = FALSE;
  New->Jitter     = FALSE;
  New->Track      = FALSE;

  New->Area_Size1 = 0;
  New->Area_Size2 = 0;

  New->Adaptive_Level = 100;

  New->Media_Attenuation = FALSE;
  New->Media_Interaction = TRUE;

  for (i = 0; i < 6; i++)
  {
    New->Light_Buffer[i] = NULL;
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Light_Source
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

static LIGHT_SOURCE *Copy_Light_Source (OBJECT *Old)
{
  int i, j;
  LIGHT_SOURCE *New;
  LIGHT_SOURCE *Light = (LIGHT_SOURCE *)Old;

  New = Create_Light_Source();

  /* Copy light source. */

  *New = *(LIGHT_SOURCE *)Old;

  New->Next_Light_Source = NULL;

  New->Children = Copy_Object (((LIGHT_SOURCE *)Old)->Children);

  if (Light->Light_Grid != NULL)
  {
    New->Light_Grid = Create_Light_Grid(Light->Area_Size1, Light->Area_Size2);

    for (i = 0; i < Light->Area_Size1; i++)
    {
      for (j = 0; j < Light->Area_Size2; j++)
      {
        Assign_Colour(New->Light_Grid[i][j], Light->Light_Grid[i][j]);
      }
    }
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Light_Source
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

static void Destroy_Light_Source (OBJECT *Object)
{
  int i;
  LIGHT_SOURCE *Light = (LIGHT_SOURCE *)Object;

  if (Light->Light_Grid != NULL)
  {
    for (i = 0; i < Light->Area_Size1; i++)
    {
      POV_FREE(Light->Light_Grid[i]);
    }

    POV_FREE(Light->Light_Grid);
  }

  Destroy_Object(Light->Children);

  POV_FREE(Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Light_Grid
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

COLOUR **Create_Light_Grid (int Size1, int  Size2)
{
  int i;
  COLOUR **New;

  New = (COLOUR **)POV_MALLOC(Size1 * sizeof (COLOUR *), "area light");

  for (i = 0; i < Size1; i++)
  {
    New[i] = (COLOUR *)POV_MALLOC(Size2 * sizeof (COLOUR), "area light");
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   cubic_spline
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
*   Cubic spline that has tangents of slope 0 at x == low and at x == high.
*   For a given value "pos" between low and high the spline value is returned.
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL cubic_spline(DBL low, DBL  high, DBL  pos)
{
  /* Check to see if the position is within the proper boundaries. */

  if (pos < low)
  {
    return(0.0);
  }
  else
  {
    if (pos >= high)
    {
      return(1.0);
    }
  }

  /* This never happens. [DB] */

/*
  if (high == low)
  {
    return(0.0);
  }
*/

  /* Normalize to the interval [0...1]. */

  pos = (pos - low) / (high - low);

  /* See where it is on the cubic curve. */

  return(3 - 2 * pos) * pos * pos;
}



/*****************************************************************************
*
* FUNCTION
*
*   Attenuate_Light
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
*   Jan 1995 : Added attenuation due to atmospheric scattering and light
*              source distance. Added cylindrical light source. [DB]
*
******************************************************************************/

DBL Attenuate_Light (LIGHT_SOURCE *Light, RAY *Ray, DBL Distance)
{
  DBL len, k, costheta;
  DBL Attenuation = 1.0;
  VECTOR P, V1;

  /* If this is a spotlight then attenuate based on the incidence angle. */

  switch (Light->Light_Type)
  {
    case SPOT_SOURCE:

      VDot(costheta, Ray->Direction, Light->Direction);

      costheta *= -1.0;

      if (costheta > 0.0)
      {
        Attenuation = pow(costheta, Light->Coeff);

        /*
         * If there is a soft falloff region associated with the light then
         * do an interpolation of values between the hot center and the
         * direction at which light falls to nothing.
         */

        if (Light->Radius > 0.0)
        {
          Attenuation *= cubic_spline(Light->Falloff, Light->Radius, costheta);
        }
/*
        Debug_Info("Atten: %lg\n", Attenuation);
*/
      }
      else
      {
        Attenuation = 0.0;
      }

      break;

    case CYLINDER_SOURCE:

      VSub(V1, Ray->Initial, Light->Center);

      VDot(k, V1, Light->Direction);

      if (k > 0.0)
      {
        VLinComb2(P, 1.0, V1, -k, Light->Direction);

        VLength(len, P);

        if (len < Light->Falloff)
        {
          len = 1.0 - len / Light->Falloff;

          Attenuation = pow(len, Light->Coeff);

          if (Light->Radius > 0.0)
          {
            Attenuation *= cubic_spline(1.0 - Light->Radius / Light->Falloff, 1.0, len);
          }
        }
        else
        {
          Attenuation = 0.0;
        }
      }
      else
      {
        Attenuation = 0.0;
      }

      break;
  }

  if (Attenuation > 0.0)
  {
    /* Attenuate light due to light source distance. */

    if ((Light->Fade_Power > 0.0) && (fabs(Light->Fade_Distance) > EPSILON))
    {
      Attenuation *= 2.0 / (1.0 + pow(Distance / Light->Fade_Distance, Light->Fade_Power));
    }
  }

  return(Attenuation);
}
