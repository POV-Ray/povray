/****************************************************************************
*                   povproto.h
*
*  This module defines the prototypes for all system-independent functions.
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

#ifndef POVPROTO_H
#define POVPROTO_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/



/*****************************************************************************
* Global typedefs
******************************************************************************/



/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

/* Prototypes for machine specific functions defined in "computer".c (ibm.c amiga.c unix.c etc.)*/
void display_finished (void);
void display_init (int width, int height);
void display_close (void);
void display_plot (int x, int y, unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha);
void display_plot_rect (int x1, int x2, int y1, int y2,
  unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha);

/* Prototypes for functions defined in mem.c */
#include "mem.h"

/* Prototypes for functions defined in userio.c */

int CDECL Banner (char *format,...);
int CDECL Warning (DBL level, char *format,...);
int CDECL Render_Info (char *format,...);
int CDECL Status_Info (char *format,...);
int CDECL Statistics (char *format,...);
int CDECL Error_Line (char *format,...);
int CDECL Error (char *format,...);
int CDECL Debug_Info (char *format, ...);

void Terminate_POV (int i);

#endif
