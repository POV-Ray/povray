/****************************************************************************
*                ray.c
*
*  This module implements the code pertaining to rays.
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
#include "povray.h"
#include "interior.h"
#include "ray.h"
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
*   Initialize_Ray_Containers
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

void Initialize_Ray_Containers(RAY *Ray)
{
  Ray->Index = - 1;
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Ray_Containers
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

void Copy_Ray_Containers(RAY *Dest_Ray, RAY  *Source_Ray)
{
  register int i;

  if ((Dest_Ray->Index = Source_Ray->Index) >= MAX_CONTAINING_OBJECTS)
  {
    Error("ERROR - Containing Index too high.\n");
  }

  for (i = 0 ; i <= Source_Ray->Index; i++)
  {
    Dest_Ray->Interiors[i] = Source_Ray->Interiors[i];
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Ray_Enter
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
*   Oct 1995 : Fixed bug with IOR assignment (only valid for plain textures) [DB]
*
******************************************************************************/

void Ray_Enter(RAY *Ray, INTERIOR *interior)
{
  int index;

  if ((index = ++(Ray->Index)) >= MAX_CONTAINING_OBJECTS)
  {
    Error("Too many nested refracting objects.");
  }

  Ray->Interiors[index] = interior;
}



/*****************************************************************************
*
* FUNCTION
*
*   Ray_Exit
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
*   Remove given entry from given ray's container.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Ray_Exit(RAY *Ray, int nr)
{
  int i;

  for (i = nr; i < Ray->Index; i++)
  {
    Ray->Interiors[i] = Ray->Interiors[i+1];
  }

  if (--(Ray->Index) < - 1)
  {
    Error("Too many exits from refractions.");
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Interior_In_Ray_Container
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
*   Test if a given interior is in the container of a given ray.
*
* CHANGES
*
*   Mar 1996 : Creation.
*
******************************************************************************/

int Interior_In_Ray_Container(RAY *ray, INTERIOR *interior)
{
  int i, found = -1;

  if (ray->Index > -1)
  {
    for (i = 0; i <= ray->Index; i++)
    {
      if (interior == ray->Interiors[i])
      {
        found = i;

        break;
      }
    }
  }

  return(found);
}



