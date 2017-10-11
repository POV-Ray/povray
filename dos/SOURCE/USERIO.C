/****************************************************************************
*                userio.c
*
*  This module contains I/O routines.
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

#include <stdarg.h>
#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "parse.h"
#include "povray.h"
#include "tokenize.h"
#include "userio.h"

STREAM_INFO Stream_Info[MAX_STREAMS];
static char vsbuffer[1000];
static void PrintToStream (int stream, char *s);

/****************************************************************************/
/* Prints s to the stream's console or file destination, as specified by type */

static void PrintToStream(int stream, char *s)
{
  if (Stream_Info[stream].handle != NULL)
  {
    fprintf(Stream_Info[stream].handle, s);
    fflush(Stream_Info[stream].handle);
  }

  if (Stream_Info[ALL_STREAM].handle != NULL)
  {
    fprintf(Stream_Info[ALL_STREAM].handle, s);
    fflush(Stream_Info[ALL_STREAM].handle);
  }
}

/****************************************************************************/
/* Use this routine to display opening banner & copyright info */
int CDECL Banner(char *format,...)
{
  va_list marker;

  va_start(marker, format);
  vsprintf(vsbuffer, format, marker);
  va_end(marker);
  
  POV_BANNER(vsbuffer);
  
  return (0);
}

/****************************************************************************/

/*
 * Use this routine to display non-fatal warning message if
 * opts.Language_Version is greater than level parameter.
 */
int CDECL Warning(DBL level, char *format,...)
{
  va_list marker;

  va_start(marker, format);
  vsprintf(vsbuffer, format, marker);
  va_end(marker);
  
  if (level >= opts.Language_Version)
    return (0);
  
  PrintToStream(WARNING_STREAM, vsbuffer);
  
  if (Stream_Info[WARNING_STREAM].do_console)
  {
    POV_WARNING(vsbuffer);
  }
  
  return (0);
}

/****************************************************************************/
/* Use this routine to display debug information. */
int CDECL Debug_Info(char *format,...)
{
  va_list marker;

  va_start(marker, format);
  vsprintf(vsbuffer, format, marker);
  va_end(marker);
  
  PrintToStream(DEBUG_STREAM, vsbuffer);
  
  if (Stream_Info[DEBUG_STREAM].do_console)
  {
    POV_DEBUG_INFO(vsbuffer);
  }
  
  return (0);
}

/****************************************************************************/

/*
 * Use this routine to display general information messages about the current
 * rendering if that information applies to the entire render.  Items such as
 * "Options in effect" or when animation is added it would display frame number
 * or clock value. In a windowed environment this info might stay static on the
 * screen during the whole session. Status messages such as "Parsing..." or
 * "Building slabs, please wait" etc should use Status_Info below.
 */
int CDECL Render_Info(char *format,...)
{
  va_list marker;

  va_start(marker, format);
  vsprintf(vsbuffer, format, marker);
  va_end(marker);
  
  PrintToStream(RENDER_STREAM, vsbuffer);
  
  if (Stream_Info[RENDER_STREAM].do_console)
  {
    POV_RENDER_INFO(vsbuffer);
  }
  
  return (0);
}

/****************************************************************************/

/*
 * Use this routine to display information messages such as "Parsing..." or
 * "Building slabs, please wait" etc   Windowed environments might implement
 * one or two status lines at the bottom of the screen.
 */
int CDECL Status_Info(char *format,...)
{
  va_list marker;

  va_start(marker, format);
  vsprintf(vsbuffer, format, marker);
  va_end(marker);
  
  POV_STATUS_INFO(vsbuffer);
  
  return (0);
}

/****************************************************************************/

/*
 * This routine is used by various specialized fatal error functions to display
 * a message to the fatal error reporting device. It does not terminate
 * rendering.  The function "Error" below prints a message and does the actual
 * termination.  Use "Error" for most purposes.
 */

int CDECL Error_Line(char *format,...)
{
  va_list marker;

  va_start(marker, format);
  vsprintf(vsbuffer, format, marker);
  va_end(marker);
  
  PrintToStream(FATAL_STREAM, vsbuffer);
  
  if (Stream_Info[FATAL_STREAM].do_console)
  {
    POV_FATAL(vsbuffer);
  }
  
  return (0);
}

/****************************************************************************/

/*
 * This not only prints a fatal error message, it first calls Where_Error and
 * it calls Terminate_POV to do the actual termination.
 */

int CDECL Error(char *format,...)
{
  va_list marker;
  
  if (Stop_Flag)
  {
    if (POV_SHELLOUT(USER_ABORT_SHL) != FATAL_RET)
    {
      POV_STATUS_INFO("\nUser abort.\n");

      UICB_USER_ABORT

      Terminate_Tokenizer(); /* Closes scene file */

      Terminate_POV(2);
    }
    else
    {
      Error_Line("\nFatal error in User_Abort_Command.\n");
    }
  }

  switch (Stage)
  {
    case STAGE_STARTUP:
      Error_Line("\nStartup error.\n");
      break;

    case STAGE_BANNER:
      Error_Line("\n Banner error.\n");
      break;

    case STAGE_INIT:
      Error_Line("\nInit error.\n");
      break;

    case STAGE_ENVIRONMENT:
      Error_Line("\nEnvironment error.\n");
      break;

    case STAGE_COMMAND_LINE:
      Error_Line("\nCommand line error.\n");
      break;

    case STAGE_FILE_INIT:
      Error_Line("\nFile init error.\n");
      break;

    case STAGE_PARSING:
    case STAGE_INCLUDE_ERR:
      Where_Error();
      break;

    case STAGE_CONTINUING:
      Error_Line("\nContinue trace error.\n");
      break;

    case STAGE_RENDERING:
      Error_Line("\nRendering error.\n");
      break;

    case STAGE_SHUTDOWN:
      Error_Line("\nShutdown error.\n");
      break;

    case STAGE_INI_FILE:
      Error_Line("\nINI file error.\n");
      break;

    case STAGE_CLEANUP_PARSE:
      Error_Line("\nCleanup parse error.\n");
      break;

    case STAGE_SLAB_BUILDING:
      Error_Line("\nSlab building error.\n");
      break;

    case STAGE_TOKEN_INIT:
      Error_Line("\nScene file parser initialization error.\n");
      break;

    case STAGE_FOUND_INSTEAD:
      break;

    default:
      Error_Line("\nUnkown error %d.\n", Stage);
      break;
  }

  va_start(marker, format);
  vsprintf(vsbuffer, format, marker);
  va_end(marker);
  
  PrintToStream(FATAL_STREAM, vsbuffer);
  
  if (Stream_Info[FATAL_STREAM].do_console)
  {
    POV_FATAL(vsbuffer);
  }

  /* This could be just an "if" but we may add special messages later */

  switch (Stage)
  {
    case STAGE_INCLUDE_ERR:
      Error_Line("Check that the file is in a directory specifed with a +L switch\n");
      Error_Line("or 'Library_Path=' .INI item.  Standard include files are in the\n");
      Error_Line("include directory or folder. Please read your documentation carefully.\n");
  }

  UICB_PARSE_ERROR

  Terminate_Tokenizer(); /* Closes scene file */

  POV_SHELLOUT(FATAL_SHL);

  Terminate_POV(1);
  
  return (0);
}

/****************************************************************************/
/* Use this routine to display final rendering statistics */
int CDECL Statistics(char *format,...)
{
  va_list marker;

  va_start(marker, format);
  vsprintf(vsbuffer, format, marker);
  va_end(marker);
  
  PrintToStream(STATISTIC_STREAM, vsbuffer);
  
  if (Stream_Info[STATISTIC_STREAM].do_console)
  {
    POV_STATISTICS(vsbuffer);
  }
  
  return (0);
}

/****************************************************************************/
/* Initialization for streams structure */

void Init_Text_Streams()
{
  int i;
  
  for (i = 0; i < MAX_STREAMS; i++)
  {
    Stream_Info[i].handle = NULL;
    Stream_Info[i].name = NULL;
    Stream_Info[i].do_console = TRUE;
  }
}

/****************************************************************************/
/* Opens stream text output files if necessary. */

void Open_Text_Streams()
{
  int i;

  for (i = 0; i < MAX_STREAMS; i++)
  {
    if (Stream_Info[i].name != NULL)
    {
      if (opts.Options & CONTINUE_TRACE)
      {
        if ((Stream_Info[i].handle =
             fopen(Stream_Info[i].name, APPEND_TXTFILE_STRING)) == NULL)
        {
          Warning(0.0, "Couldn't append stream to file %s.\n", Stream_Info[i].name);
        }
      }
      else
      {
        if ((Stream_Info[i].handle =
             fopen(Stream_Info[i].name, WRITE_TXTFILE_STRING)) == NULL)
        {
          Warning(0.0, "Couldn't write stream to file %s.\n", Stream_Info[i].name);
        }
      }
    }
  }
}

void Destroy_Text_Streams()
{
  int i;

  for (i = 0; i < MAX_STREAMS; i++)
  {
    if (Stream_Info[i].name)
    {
      POV_FREE(Stream_Info[i].name);

      Stream_Info[i].name = NULL;
    }
  }
}


/****************************************************************************/
void POV_Std_Banner(char *s)
{
  fprintf(stderr, s);
  fflush(stderr);
}

/****************************************************************************/
void POV_Std_Warning(char *s)
{
  fprintf(stderr, s);
  fflush(stderr);
}

/****************************************************************************/
void POV_Std_Status_Info(char *s)
{
  fprintf(stderr, s);
  fflush(stderr);
}

/****************************************************************************/
void POV_Std_Render_Info(char *s)
{
  fprintf(stderr, s);
  fflush(stderr);
}

/****************************************************************************/
void POV_Std_Debug_Info(char *s)
{
  fprintf(stderr, s);
  fflush(stderr);
}

/****************************************************************************/
void POV_Std_Fatal(char *s)
{
  fprintf(stderr, s);
  fflush(stderr);
}

/****************************************************************************/
void POV_Std_Statistics(char *s)
{
  fprintf(stderr, s);
  fflush(stderr);
}

/****************************************************************************/
void Terminate_POV(int i)
{
  close_all();
  
  POV_MEM_RELEASE_ALL(i == 0);

  pre_init_flag=0;
  
  FINISH_POVRAY(i); /* Must call exit(i) or somehow stop */
}

/***************************************************************************
 *
 * Dummy display routines for non-graphic Unix ports
 *
 **************************************************************************/

static DBL Display_Width_Scale, Display_Height_Scale;
static int Prev_X, Prev_Y;

/****************************************************************************/
void POV_Std_Display_Init(int w, int  h)
{
  Display_Width_Scale = 78.0 / (DBL)w;
  Display_Height_Scale = 24.0 / (DBL)h;
  Prev_X = 0;
  Prev_Y = 0;
  fprintf(stderr, "\n");
}

/****************************************************************************/
void POV_Std_Display_Finished()
{
  char s[3];

  fprintf(stderr, "\007\007");
  if (opts.Options & PROMPTEXIT)
  {
    fgets(s, 2, stdin);
  }
}

/****************************************************************************/
void POV_Std_Display_Close()
{
  fprintf(stderr, "\n");
}

/****************************************************************************/
void POV_Std_Display_Plot(int x, int y, unsigned int r, unsigned int g, unsigned int b, unsigned int a)
{
  int sx = (int)(Display_Width_Scale * ((DBL)x));
  int sy = (int)(Display_Height_Scale * ((DBL)y));
  char I;
  unsigned char G[6] = { ' ', '.', 'o', '*', '@', 'M' };

  if (sy > Prev_Y)
  {
    Prev_Y++;
    
    fprintf(stderr, "\n");
    
    Prev_X = 0;
  }
  
  if (sx > Prev_X)
  {
    I = (int)(((DBL)r * 1.80 + (DBL)g * 3.54 + (DBL)b * 0.66) / 256.0);

    fprintf(stderr, "%c", G[(int)I]);
    
    Prev_X++;
  }
}

/****************************************************************************/
void POV_Std_Display_Plot_Rect(int x1, int y1, int x2, int y2, unsigned int r, unsigned int g, unsigned int b, unsigned int a)
{
  POV_Std_Display_Plot(x1, y1, r, g, b, a);
}

/*****************************************************************************
*
* FUNCTION
*
*   POV_Std_Display_Plot_Box
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Chris Young
*   
* DESCRIPTION
*
*   Generic box drawing routine which may be overriden in POV_DRAW_BOX
*   by a platform specific routine.
*
* CHANGES
*
*   Nov 1995 : Creation.
*
******************************************************************************/
void POV_Std_Display_Plot_Box(int x1, int y1, int x2, int y2, unsigned int r, unsigned int g, unsigned int b, unsigned int a)
{
     int x,y;
   
     for (x = x1; x <= x2; x++)
     {
       POV_DISPLAY_PLOT(x, y1, r, g, b, a);
       POV_DISPLAY_PLOT(x, y2, r, g, b, a);
     }

     for (y = y1; y <= y2; y++)
     {
       POV_DISPLAY_PLOT(x1, y, r, g, b, a);
       POV_DISPLAY_PLOT(x2, y, r, g, b, a);
     }
  }

