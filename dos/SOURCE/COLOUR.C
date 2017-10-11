/****************************************************************************
*                   colour.c
*
*  This module implements routines to manipulate colours.
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
#include "colour.h"
#include "pigment.h"
#include "normal.h"
#include "texture.h"


/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/


/*****************************************************************************
* Static functions
******************************************************************************/



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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

COLOUR *Create_Colour ()
{
  COLOUR *New;

  New = (COLOUR *)POV_MALLOC(sizeof (COLOUR), "color");

  Make_Colour (*New, 0.0, 0.0, 0.0);

  return (New);
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

COLOUR *Copy_Colour (COLOUR Old)
{
  COLOUR *New;

  if (Old != NULL)
  {
    New = Create_Colour ();

    Assign_Colour(*New,Old);
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
*   Aug 1995 : Use POV_CALLOC to initialize entries. [DB]
*
******************************************************************************/

BLEND_MAP_ENTRY *Create_BMap_Entries (int Map_Size)
{
  BLEND_MAP_ENTRY *New;

  New = (BLEND_MAP_ENTRY *)POV_CALLOC((size_t)Map_Size, sizeof (BLEND_MAP_ENTRY), "blend map entry");

  return (New);
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
*
* CHANGES
*
******************************************************************************/

BLEND_MAP_ENTRY *Copy_BMap_Entries (BLEND_MAP_ENTRY *Old, int Map_Size, int  Type)
{
  int i;
  BLEND_MAP_ENTRY *New;

  if (Old != NULL)
  {
    New = Create_BMap_Entries (Map_Size);

    for (i = 0; i < Map_Size; i++)
    {
      switch (Type)
      {
        case PIGMENT_TYPE:

          New[i].Vals.Pigment = Copy_Pigment(Old[i].Vals.Pigment);

          break;

        case NORMAL_TYPE:

          New[i].Vals.Tnormal = Copy_Tnormal(Old[i].Vals.Tnormal);

          break;

        case TEXTURE_TYPE:

          New[i].Vals.Texture = Copy_Textures(Old[i].Vals.Texture);

          break;

        case COLOUR_TYPE:
        case SLOPE_TYPE:

          New[i] = Old[i];

          break;
      }
    }
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
*   Create_Blend_Map
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

BLEND_MAP *Create_Blend_Map ()
{
  BLEND_MAP *New;

  New = (BLEND_MAP *)POV_MALLOC(sizeof (BLEND_MAP), "blend map");

  New->Users = 1;

  New->Number_Of_Entries = 0;

  New->Type = COLOUR_TYPE;

  New->Blend_Map_Entries = NULL;

  New->Transparency_Flag = FALSE;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Blend_Map
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

BLEND_MAP *Copy_Blend_Map (BLEND_MAP *Old)
{
  BLEND_MAP *New;

  New = Old;

  /* 
   * Do not increase the users field if it is negative.
   *
   * A negative users field incicates a reference to a static
   * or global memory area in the data segment, not on the heap!
   * Thus it must not be deleted later.
   */

  if ((New != NULL) && (New->Users >= 0))
  {
    New->Users++;
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Colour_Distance
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

DBL Colour_Distance (COLOUR colour1, COLOUR  colour2)
{
  return (fabs(colour1[RED]   - colour2[RED]) +
          fabs(colour1[GREEN] - colour2[GREEN]) +
          fabs(colour1[BLUE]  - colour2[BLUE]));
}



/*****************************************************************************
*
* FUNCTION
*
*   Add_Colour
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

void Add_Colour (COLOUR result, COLOUR  colour1, COLOUR  colour2)
{
  result[RED]    = colour1[RED]    + colour2[RED];
  result[GREEN]  = colour1[GREEN]  + colour2[GREEN];
  result[BLUE]   = colour1[BLUE]   + colour2[BLUE];
  result[FILTER] = colour1[FILTER] + colour2[FILTER];
  result[TRANSM] = colour1[TRANSM] + colour2[TRANSM];
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Colour
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

void Scale_Colour (COLOUR result, COLOUR  colour, DBL factor)
{
  result[RED]    = colour[RED]    * factor;
  result[GREEN]  = colour[GREEN]  * factor;
  result[BLUE]   = colour[BLUE]   * factor;
  result[FILTER] = colour[FILTER] * factor;
  result[TRANSM] = colour[TRANSM] * factor;
}



/*****************************************************************************
*
* FUNCTION
*
*   Clip_Colour
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

void Clip_Colour (COLOUR result, COLOUR  colour)
{
  if (colour[RED] > 1.0)
  {
    result[RED] = 1.0;
  }
  else
  {
    if (colour[RED] < 0.0)
    {
      result[RED] = 0.0;
    }
    else
    {
      result[RED] = colour[RED];
    }
  }

  if (colour[GREEN] > 1.0)
  {
    result[GREEN] = 1.0;
  }
  else
  {
    if (colour[GREEN] < 0.0)
    {
      result[GREEN] = 0.0;
    }
    else
    {
      result[GREEN] = colour[GREEN];
    }
  }

  if (colour[BLUE] > 1.0)
  {
    result[BLUE] = 1.0;
  }
  else
  {
    if (colour[BLUE] < 0.0)
    {
      result[BLUE] = 0.0;
    }
    else
    {
      result[BLUE] = colour[BLUE];
    }
  }

  if (colour[FILTER] > 1.0)
  {
    result[FILTER] = 1.0;
  }
  else
  {
    if (colour[FILTER] < 0.0)
    {
      result[FILTER] = 0.0;
    }
    else
    {
      result[FILTER] = colour[FILTER];
    }
  }

  if (colour[TRANSM] > 1.0)
  {
    result[TRANSM] = 1.0;
  }
  else
  {
    if (colour[TRANSM] < 0.0)
    {
      result[TRANSM] = 0.0;
    }
    else
    {
      result[TRANSM] = colour[TRANSM];
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Blend_Map
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

void Destroy_Blend_Map (BLEND_MAP *BMap)
{
  int i;
  
  if (BMap != NULL)
  {
    if (--(BMap->Users) == 0)
    {
      for (i = 0; i < BMap->Number_Of_Entries; i++)
      {
        switch (BMap->Type)
        {
           case PIGMENT_TYPE:
           case DENSITY_TYPE:
             Destroy_Pigment(BMap->Blend_Map_Entries[i].Vals.Pigment);
             break;

           case NORMAL_TYPE:
             Destroy_Tnormal(BMap->Blend_Map_Entries[i].Vals.Tnormal);
             break;

           case TEXTURE_TYPE:
             Destroy_Textures(BMap->Blend_Map_Entries[i].Vals.Texture);
        }
      }

      POV_FREE (BMap->Blend_Map_Entries);

      POV_FREE (BMap);
    }
  }
}

