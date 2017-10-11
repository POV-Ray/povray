/****************************************************************************
*                   media.h
*
*  This module contains all defines, typedefs, and prototypes for MEDIA.C.
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


#ifndef MEDIA_H
#define MEDIA_H

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Scattering types. */

#define ISOTROPIC_SCATTERING            1
#define MIE_HAZY_SCATTERING             2
#define MIE_MURKY_SCATTERING            3
#define RAYLEIGH_SCATTERING             4
#define HENYEY_GREENSTEIN_SCATTERING    5
#define SCATTERING_TYPES                5

/*****************************************************************************
* Global typedefs
******************************************************************************/



/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

void Simulate_Media (IMEDIA **, RAY *, INTERSECTION *, COLOUR , int );

void Init_Media (IMEDIA *);

IMEDIA *Create_Media (void);
IMEDIA *Copy_Media (IMEDIA *);
void Destroy_Media (IMEDIA *);
void Transform_Media (IMEDIA *IMedia, TRANSFORM *Trans);
void Transform_Density (PIGMENT *Density, TRANSFORM *Trans);
void Post_Media (IMEDIA *);

#endif
