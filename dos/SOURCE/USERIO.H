/****************************************************************************
*                   userio.h
*
*  This module contains all defines, typedefs, and prototypes for userio.c
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


#ifndef USERIO_H
#define USERIO_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/




/*****************************************************************************
* Global typedefs
******************************************************************************/

enum
{
  DEBUG_STREAM = 0,
  FATAL_STREAM,
  RENDER_STREAM,
  STATISTIC_STREAM,
  WARNING_STREAM,
  ALL_STREAM,
  MAX_STREAMS  /* Must be last! */
};

typedef struct Stream_Struct
{
  FILE *handle;
  char *name;
  int do_console;
} STREAM_INFO;



/*****************************************************************************
* Global variables
******************************************************************************/

extern STREAM_INFO Stream_Info[MAX_STREAMS];


/*****************************************************************************
* Global functions
******************************************************************************/

void POV_Std_Banner (char *s);
void POV_Std_Warning (char *s);
void POV_Std_Render_Info (char *s);
void POV_Std_Status_Info (char *s);
void POV_Std_Fatal (char *s);
void POV_Std_Statistics (char *s);
void POV_Std_Debug_Info (char *s);
void POV_Std_Display_Init (int w, int h);
void POV_Std_Display_Close (void);
void POV_Std_Display_Finished (void);
void POV_Std_Display_Plot (int x, int y, unsigned int r, unsigned int g, unsigned int b, unsigned int a);
void POV_Std_Display_Plot_Rect (int x1, int y1, int x2, int y2, unsigned int r, unsigned int g, unsigned int b, unsigned int a);
void POV_Std_Display_Plot_Box (int x1,int y1,int x2,int y2, unsigned int r,unsigned int g,unsigned int b,unsigned int a);

void Init_Text_Streams (void);
void Open_Text_Streams (void);
void Destroy_Text_Streams (void);

#endif /* USERIO_H */
