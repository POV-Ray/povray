/****************************************************************************
*                   optout.h
*
*  This module contains all defines, typedefs, and prototypes for OPTOUT.C.
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


#ifndef OPTOUT_H
#define OPTOUT_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* These are used by OPTOUT.C and the machine specific modules */

#define POV_RAY_VERSION "3.1d"

#define DISTRIBUTION_MESSAGE_1 "This is an unofficial version compiled by:"
#define DISTRIBUTION_MESSAGE_2 "FILL IN NAME HERE........................."
#define DISTRIBUTION_MESSAGE_3 "The POV-Ray Team(tm) is not responsible for supporting this version."

/* Number of help pages (numbered 0 to MAX_HELP_PAGE). */

#define MAX_HELP_PAGE 7



/*****************************************************************************
* Global typedefs
******************************************************************************/




/*****************************************************************************
* Global variables
******************************************************************************/

/* These are available for GUI environments that may display them in a credits dialog */
extern char *Primary_Developers[];
extern char *Contributing_Authors[];


/*****************************************************************************
* Global functions
******************************************************************************/

void Print_Credits (void);
void Print_Options (void);
void Print_Stats (COUNTER *);
void Usage (int n, int f);
void Print_Help_Screens (void);
void Print_Authors (void);



#endif
