/****************************************************************************
*                txttest.c
*
*  This module implements "fill-in-the-blank" pre-programmed texture 
*  functions for easy modification and testing. Create new textures here.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1998 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that patterns may experiment
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
#include "povray.h"    /* [DB 9/94] */
#include "txttest.h"   /* [DB 9/94] */
#include "pattern.h"   /* [CY 10/94] */



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



/*
 * Test new textures in the routines that follow.
 */

/*****************************************************************************
*
* FUNCTION
*
*   pattern1
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
*   The pattern routines take an x,y,z point on an object and a pointer to
*   the object's texture description and return the color at that point
*   Similar routines are granite, agate, marble. See txtcolor.c for examples.
*
* CHANGES
*
******************************************************************************/

DBL pattern1 (VECTOR EPoint, TPATTERN *TPat)
{
  DBL value;
  /* YOUR NAME HERE */

  TPat=TPat;

  value = Noise(EPoint);

  return(value);

}



/*****************************************************************************
*
* FUNCTION
*
*   pattern2
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
*   The pattern routines take an x,y,z point on an object and a pointer to
*   the object's texture description and return the color at that point
*   Similar routines are granite, agate, marble. See txtcolor.c for examples.
*
* CHANGES
*
******************************************************************************/

DBL pattern2 (VECTOR EPoint, TPATTERN *TPat)
{
  DBL value;
  /* YOUR NAME HERE */
  TPat=TPat;

  value = Noise(EPoint);

  return(value);

}




/*****************************************************************************
*
* FUNCTION
*
*   pattern3
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
*   The pattern routines take an x,y,z point on an object and a pointer to
*   the object's texture description and return the color at that point
*   Similar routines are granite, agate, marble. See txtcolor.c for examples.
*
* CHANGES
*
******************************************************************************/

DBL pattern3 (VECTOR EPoint, TPATTERN *TPat)
{
  DBL value;
  /* YOUR NAME HERE */
  TPat=TPat;

  value = Noise(EPoint);

  return(value);

}



/*****************************************************************************
*
* FUNCTION
*
*   bumpy1
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
*   The bumpy routines take a point on an object,  a pointer to the
*   object's texture description and the surface normal at that point and
*   return a peturb surface normal to create the illusion that the surface
*   has been displaced.
*
*   Similar routines are ripples, dents, bumps. See txtbump.c for examples.
*
* CHANGES
*
******************************************************************************/

void bumpy1 (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal)
{
  /* YOUR NAME HERE */
  EPoint=EPoint;

  Tnormal = Tnormal;

  Assign_Vector(normal, normal);
}



/*****************************************************************************
*
* FUNCTION
*
*   bumpy2
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
*   The bumpy routines take a point on an object,  a pointer to the
*   object's texture description and the surface normal at that point and
*   return a peturb surface normal to create the illusion that the surface
*   has been displaced.
*
*   Similar routines are ripples, dents, bumps. See txtbump.c for examples.
*
* CHANGES
*
******************************************************************************/

void bumpy2 (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal)
{
  /* YOUR NAME HERE */
  EPoint=EPoint;

  Tnormal = Tnormal;

  Assign_Vector(normal, normal);
}



/*****************************************************************************
*
* FUNCTION
*
*   bumpy3
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
*   The bumpy routines take a point on an object,  a pointer to the
*   object's texture description and the surface normal at that point and
*   return a peturb surface normal to create the illusion that the surface
*   has been displaced.
*
*   Similar routines are ripples, dents, bumps. See txtbump.c for examples.
*
* CHANGES
*
******************************************************************************/

void bumpy3 (VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal)
{
  /* YOUR NAME HERE */
  EPoint=EPoint;

  Tnormal = Tnormal;

  Assign_Vector(normal, normal);
}
