/****************************************************************************
*                   optin.c
*
*  This module contains functions for ini-file/command line parsing, streams.
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

/****************************************************************************
*
*  This file contains the routines to implement an .INI file parser that can
*  parse options in the form "Variable=value" or traditional POV-Ray
*  command-line switches.  Values can come from POVRAYOPT, command-line,
*  .DEF or .INI files.
*
*  Written by CEY 4/94 based on existing code and INI code from CDW.
*
*  ---
*
* Modification by Thomas Willhalm, March 1999, used with permission.
*
*****************************************************************************/

#include <ctype.h>
#include <time.h>
#include "frame.h"
#include "povproto.h"
#include "bbox.h"
#include "lighting.h"
#include "mem.h"        /*POV_FREE*/
#include "octree.h"
#include "povray.h"
#include "optin.h"
#include "optout.h"
#include "parse.h"
#include "radiosit.h"
#include "render.h"
#include "tokenize.h"
#include "vlbuffer.h"
#include "ppm.h"
#include "targa.h"
#include "userio.h"
#include "png_pov.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/


/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

char *DefaultFile[] =
{
  "debug.out",
  "fatal.out",
  "render.out",
  "stats.out",
  "warning.out",
  "alltext.out"
};

int inflag, outflag;

/* Quality constants */

long Quality_Values[12]=
{
  QUALITY_0, QUALITY_1, QUALITY_2, QUALITY_3, QUALITY_4,
  QUALITY_5, QUALITY_6, QUALITY_7, QUALITY_8, QUALITY_9
};

/* Keywords for the ini-file parser. */

struct Reserved_Word_Struct Option_Variable [] =
{
  { ALL_CONSOLE_OP, "All_Console" },
  { ALL_FILE_OP, "All_File" },
  { ANTIALIAS_DEPTH_OP, "Antialias_Depth" },
  { ANTIALIAS_OP, "Antialias" },
  { ANTIALIAS_THRESH_OP, "Antialias_Threshold" },
  { BOUNDING_OP, "Bounding" },
  { BOUNDING_THRESH_OP, "Bounding_Threshold" },
  { BUFFERED_OUTPUT_OP,"Buffer_Output" },
  { BUF_SIZE_OP, "Buffer_Size" },
  { CLOCK_OP, "Clock" },

  { CONTINUE_OP, "Continue_Trace" },
  { CREATE_INI_OP, "Create_Ini" },
  { CYCLIC_ANIMATION_OP, "Cyclic_Animation" },
  { DEBUG_CONSOLE_OP, "Debug_Console" },
  { DEBUG_FILE_OP, "Debug_File" },
  { DISPLAY_OP, "Display" },
  { DISPLAY_GAMMA_OP, "Display_Gamma" },
  { DRAW_VISTAS_OP, "Draw_Vistas" },
  { END_COLUMN_OP, "End_Column" },
  { END_ROW_OP, "End_Row" },
  { FATAL_CONSOLE_OP, "Fatal_Console" },

  { FATAL_ERROR_CMD_OP, "Fatal_Error_Command" },
  { FATAL_ERROR_RET_OP, "Fatal_Error_Return" },
  { FATAL_FILE_OP, "Fatal_File" },
  { FIELD_RENDER_OP, "Field_Render" },
  { FILE_OUTPUT_OP, "Output_to_File" },
  { FILE_OUTPUT_TYPE_OP, "Output_File_Type" },
  { FINAL_CLOCK_OP, "Final_Clock" },
  { FINAL_FRAME_OP, "Final_Frame" },
  { HEIGHT_OP, "Height" },
  { HIST_NAME_OP, "Histogram_Name" },

  { HIST_SIZE_OP, "Histogram_Grid_Size" },
  { HIST_TYPE_OP, "Histogram_Type" },
  { INITIAL_CLOCK_OP, "Initial_Clock" },
  { INITIAL_FRAME_OP, "Initial_Frame" },
  { INPUT_FILE_NAME_OP, "Input_File_Name" },
  { JITTER_AMOUNT_OP, "Jitter_Amount" },
  { JITTER_OP, "Jitter" },
  { LIBRARY_PATH_OP, "Library_Path" },
  { LIGHT_BUFFER_OP, "Light_Buffer" },
  { ODD_FIELD_OP, "Odd_Field" },

  { OUTPUT_ALPHA_OP, "Output_Alpha" },
  { OUTPUT_FILE_NAME_OP, "Output_File_Name" },
  { PALETTE_OP, "Palette" },
  { PAUSE_WHEN_DONE_OP, "Pause_When_Done" },
  { POST_FRAME_CMD_OP, "Post_Frame_Command" },
  { POST_FRAME_RET_OP, "Post_Frame_Return" },
  { POST_SCENE_CMD_OP, "Post_Scene_Command" },
  { POST_SCENE_RET_OP, "Post_Scene_Return" },
  { PREVIEW_E_OP, "Preview_End_Size" },
  { PREVIEW_S_OP, "Preview_Start_Size" },

  { PRE_FRAME_CMD_OP, "Pre_Frame_Command" },
  { PRE_FRAME_RET_OP, "Pre_Frame_Return" },
  { PRE_SCENE_CMD_OP, "Pre_Scene_command" },
  { PRE_SCENE_RET_OP, "Pre_Scene_Return" },
  { QUALITY_OP, "Quality" },
  { RAD_SWITCH_OP, "Radiosity" },
  { REMOVE_BOUNDS_OP, "Remove_Bounds" },
  { RENDER_CONSOLE_OP, "Render_Console" },
  { RENDER_FILE_OP, "Render_File" },
  { SAMPLING_METHOD_OP, "Sampling_Method" },

  { SPLIT_UNIONS_OP, "Split_Unions" },
  { START_COLUMN_OP, "Start_Column" },
  { START_ROW_OP, "Start_Row" },
  { STATISTIC_CONSOLE_OP, "Statistic_Console" },
  { STATISTIC_FILE_OP, "Statistic_File" },
  { SUBSET_END_FRAME_OP, "Subset_End_Frame" },
  { SUBSET_START_FRAME_OP, "Subset_Start_Frame" },
  { TEST_ABORT_COUNT_OP, "Test_Abort_Count" },
  { TEST_ABORT_OP, "Test_Abort" },
  { USER_ABORT_CMD_OP, "User_Abort_Command" },

  { USER_ABORT_RET_OP, "User_Abort_Return" },
  { VERBOSE_OP, "Verbose" },
  { VERSION_OP, "Version" },
  { VIDEO_MODE_OP, "Video_Mode" },
  { VISTA_BUFFER_OP, "Vista_Buffer" },
  { WARNING_CONSOLE_OP, "Warning_Console" },
  { WARNING_FILE_OP, "Warning_File" },
  { WIDTH_OP, "Width" },

  { BITS_PER_COLOR_OP, "Bits_Per_Color" },
  { BITS_PER_COLOUR_OP, "Bits_Per_Colour" },
  { INCLUDE_INI_OP, "Include_Ini" }
};

static char temp_string[3]="\0\0";
static char ret_string[7]="IQUFSA";

/*****************************************************************************
* static functions
******************************************************************************/

static int matches ( char *v1, char *v2 );
static int istrue ( char *value );
static int isfalse ( char *value );

/*****************************************************************************
*
* FUNCTION
*
*   get_ini_value
*
* INPUT
*
*   op - the .ini option's index
*   libind - if op = LIBRARY_PATH_OP, the library's index
*   
* OUTPUT
*   
* RETURNS
*
*   char * pointing to a static string representation of the
*   option's value.
*   
* AUTHOR
*
*   SCD, 2/95
*   
* DESCRIPTION
*
*   Returns a static string representation of an option's value.
*
* CHANGES
*
*   -
*
******************************************************************************/

char *get_ini_value(int op, int  libind)
{
  static char value[128];

  value[0] = '\0';

  switch (op)
  {
    case BUF_SIZE_OP:
      sprintf(value,"%d", opts.File_Buffer_Size>>10);
      return(value);

    case BUFFERED_OUTPUT_OP:
      return (opts.Options & BUFFERED_OUTPUT ? "On" : "Off");

    case CONTINUE_OP:
      return (opts.Options & CONTINUE_TRACE ? "On" : "Off");

    case DISPLAY_OP:
      return (opts.Options & DISPLAY ? "On" : "Off");

    case VIDEO_MODE_OP:
      sprintf(value,"%c",opts.DisplayFormat);
      return(value);

    case PALETTE_OP:
      sprintf(value,"%c",opts.PaletteOption);
      return(value);

    case VERBOSE_OP:
      return (opts.Options & VERBOSE ? "On" : "Off");

    case WIDTH_OP:
      sprintf(value,"%d",Frame.Screen_Width);
      return(value);

    case HEIGHT_OP:
      sprintf(value,"%d",Frame.Screen_Height);
      return(value);

    case FILE_OUTPUT_OP:
      return (opts.Options & DISKWRITE ? "On" : "Off");

    case FILE_OUTPUT_TYPE_OP:
      sprintf(value,"%c",opts.OutputFormat);
      return(value);

    case PAUSE_WHEN_DONE_OP:
      return (opts.Options & PROMPTEXIT ? "On" : "Off");

    case INPUT_FILE_NAME_OP:
      return opts.Input_File_Name;

    case OUTPUT_FILE_NAME_OP:
      return opts.Output_File_Name;

    case ANTIALIAS_OP:
      return (opts.Options & ANTIALIAS ? "On" : "Off");

    case ANTIALIAS_THRESH_OP:
      sprintf(value,"%g",opts.Antialias_Threshold);
      return(value);

    case ANTIALIAS_DEPTH_OP:
      sprintf(value,"%ld",opts.AntialiasDepth);
      return(value);

    case JITTER_OP:
      return (opts.Options & JITTER ? "On" : "Off");

    case JITTER_AMOUNT_OP:
      sprintf(value,"%g",opts.JitterScale);
      return(value);

    case TEST_ABORT_OP:
      return (opts.Options & EXITENABLE ? "On" : "Off");

    case TEST_ABORT_COUNT_OP:
      sprintf(value,"%d",opts.Abort_Test_Counter);
      return(value);

    case LIBRARY_PATH_OP:
      return opts.Library_Paths[libind];

    case START_COLUMN_OP:
      if (opts.First_Column == -1)
        sprintf(value,"%g",opts.First_Column_Percent);
      else
        sprintf(value,"%d",opts.First_Column);
      return(value);

    case START_ROW_OP:
      if (opts.First_Line == -1)
        sprintf(value,"%g",opts.First_Line_Percent);
      else
        sprintf(value,"%d",opts.First_Line);
      return(value);

    case END_COLUMN_OP:
      if (opts.Last_Column == -1)
        sprintf(value,"%g",opts.Last_Column_Percent);
      else
        sprintf(value,"%d",opts.Last_Column);
      return(value);

    case END_ROW_OP:
      if (opts.Last_Line == -1)
        sprintf(value,"%g",opts.Last_Line_Percent);
      else
        sprintf(value,"%d",opts.Last_Line);
      return(value);

    case VERSION_OP:
      sprintf(value,"%g",opts.Language_Version);
      return(value);

    case BOUNDING_OP:
      return (opts.Use_Slabs ? "On" : "Off");

    case BOUNDING_THRESH_OP:
      sprintf(value,"%ld",opts.BBox_Threshold);
      return(value);

    case QUALITY_OP:
      sprintf(value,"%d",opts.Quality);
      return(value);

    case PREVIEW_S_OP:
      sprintf(value,"%d",opts.PreviewGridSize_Start);
      return value;

    case PREVIEW_E_OP:
      sprintf(value,"%d",opts.PreviewGridSize_End);
      return value;

    case CLOCK_OP:
      sprintf(value,"%g",opts.FrameSeq.Clock_Value);
      return value;

    case INITIAL_FRAME_OP:
      sprintf(value,"%d",opts.FrameSeq.InitialFrame);
      return value;

    case INITIAL_CLOCK_OP:
      sprintf(value,"%g",opts.FrameSeq.InitialClock);
      return value;

    case FINAL_FRAME_OP:
      sprintf(value,"%d",opts.FrameSeq.FinalFrame);
      return value;

    case FINAL_CLOCK_OP:
      sprintf(value,"%g",opts.FrameSeq.FinalClock);
      return value;

    case SUBSET_START_FRAME_OP:
      sprintf(value,"%d",opts.FrameSeq.SubsetStartFrame);
      return value;

    case SUBSET_END_FRAME_OP:
      sprintf(value,"%d",opts.FrameSeq.SubsetEndFrame);
      return value;

    case CREATE_INI_OP:
      return opts.Ini_Output_File_Name;

    case ALL_CONSOLE_OP:
      return (Stream_Info[ALL_STREAM].do_console ? "On" : "Off");

    case ALL_FILE_OP:
      return (Stream_Info[ALL_STREAM].name ? Stream_Info[ALL_STREAM].name : "");

    case DEBUG_CONSOLE_OP:
      return (Stream_Info[DEBUG_STREAM].do_console ? "On" : "Off");

    case DEBUG_FILE_OP:
      return (Stream_Info[DEBUG_STREAM].name ? Stream_Info[DEBUG_STREAM].name : "");

    case RENDER_CONSOLE_OP:
      return (Stream_Info[RENDER_STREAM].do_console ? "On" : "Off");

    case RENDER_FILE_OP:
      return (Stream_Info[RENDER_STREAM].name ? Stream_Info[RENDER_STREAM].name : "");

    case STATISTIC_CONSOLE_OP:
      return (Stream_Info[STATISTIC_STREAM].do_console ? "On" : "Off");

    case STATISTIC_FILE_OP:
      return (Stream_Info[STATISTIC_STREAM].name ? Stream_Info[STATISTIC_STREAM].name : "");

    case WARNING_CONSOLE_OP:
      return (Stream_Info[WARNING_STREAM].do_console ? "On" : "Off");

    case WARNING_FILE_OP:
      return (Stream_Info[WARNING_STREAM].name ? Stream_Info[WARNING_STREAM].name : "");

    case FATAL_CONSOLE_OP:
      return (Stream_Info[FATAL_STREAM].do_console ? "On" : "Off");

    case FATAL_FILE_OP:
      return (Stream_Info[FATAL_STREAM].name ? Stream_Info[FATAL_STREAM].name : "");

    case RAD_SWITCH_OP:
      return (opts.Options & RADIOSITY ? "On" : "Off");

    case HIST_SIZE_OP:
      sprintf (value, "%d.%d", opts.histogram_x, opts.histogram_y) ;
      return (value) ;

    case HIST_TYPE_OP:
      switch (opts.histogram_type)
      {
        case CSV :
          return ("C ; CSV") ;

        case SYS :
          return ("S ; SYS") ;

        case PPM :
          return ("P ; PPM") ;

        case TARGA :
          return ("T ; TARGA") ;

        case PNG :
          return ("N ; PNG") ;

        case NONE :
          return ("X ; NONE") ;

      }
      return ("X ; [UNKNOWN VALUE PASSED]") ;

    case HIST_NAME_OP:
      return (opts.Histogram_File_Name) ;

    case VISTA_BUFFER_OP:
      return ((opts.Options & USE_VISTA_BUFFER) ? "On" : "Off");

    case LIGHT_BUFFER_OP:
      return ((opts.Options & USE_LIGHT_BUFFER) ? "On" : "Off");

    case DRAW_VISTAS_OP:
      return ((opts.Options & USE_VISTA_DRAW) ? "On" : "Off");

    case SPLIT_UNIONS_OP:
      return ((opts.Options & SPLIT_UNION) ? "On" : "Off");

    case REMOVE_BOUNDS_OP:
      return ((opts.Options & REMOVE_BOUNDS) ? "On" : "Off");

    case CYCLIC_ANIMATION_OP:
      return ((opts.Options & CYCLIC_ANIMATION) ? "On" : "Off");

    case PRE_SCENE_CMD_OP:
      return opts.Shellouts[PRE_SCENE_SHL].Command;

    case PRE_FRAME_CMD_OP:
      return opts.Shellouts[PRE_FRAME_SHL].Command;

    case POST_FRAME_CMD_OP:
      return opts.Shellouts[POST_FRAME_SHL].Command;

    case POST_SCENE_CMD_OP:
      return opts.Shellouts[POST_SCENE_SHL].Command;

    case USER_ABORT_CMD_OP:
      return opts.Shellouts[USER_ABORT_SHL].Command;

    case FATAL_ERROR_CMD_OP:
      return opts.Shellouts[FATAL_SHL].Command;

    case PRE_SCENE_RET_OP:
      temp_string[0]=(opts.Shellouts[PRE_SCENE_SHL].Inverse)?'!':' ';
      temp_string[1]=ret_string[opts.Shellouts[PRE_SCENE_SHL].Ret];
      return temp_string;

    case PRE_FRAME_RET_OP:
      temp_string[0]=(opts.Shellouts[PRE_FRAME_SHL].Inverse)?'!':' ';
      temp_string[1]=ret_string[opts.Shellouts[PRE_FRAME_SHL].Ret];
      return temp_string;

    case POST_FRAME_RET_OP:
      temp_string[0]=(opts.Shellouts[POST_FRAME_SHL].Inverse)?'!':' ';
      temp_string[1]=ret_string[opts.Shellouts[POST_FRAME_SHL].Ret];
      return temp_string;

    case POST_SCENE_RET_OP:
      temp_string[0]=(opts.Shellouts[POST_SCENE_SHL].Inverse)?'!':' ';
      temp_string[1]=ret_string[opts.Shellouts[POST_SCENE_SHL].Ret];
      return temp_string;

    case USER_ABORT_RET_OP:
      temp_string[0]=(opts.Shellouts[USER_ABORT_SHL].Inverse)?'!':' ';
      temp_string[1]=ret_string[opts.Shellouts[USER_ABORT_SHL].Ret];
      return temp_string;

    case FATAL_ERROR_RET_OP:
      temp_string[0]=(opts.Shellouts[FATAL_SHL].Inverse)?'!':' ';
      temp_string[1]=ret_string[opts.Shellouts[FATAL_SHL].Ret];
      return temp_string;

    case OUTPUT_ALPHA_OP:
      return ((opts.Options & OUTPUT_ALPHA) ? "On" : "Off");

    case FIELD_RENDER_OP:
      return (opts.FrameSeq.Field_Render_Flag ? "On" : "Off");

    case ODD_FIELD_OP:
      return (opts.FrameSeq.Odd_Field_Flag ? "On" : "Off");
      
    case SAMPLING_METHOD_OP:
      sprintf(value,"%d",opts.Tracing_Method);
      return value;

    case BITS_PER_COLOR_OP:
    case BITS_PER_COLOUR_OP:
      sprintf(value,"%d",opts.OutputQuality);
      return value;

    case DISPLAY_GAMMA_OP:
      sprintf(value,"%g",opts.DisplayGamma);
      return value;

    case INCLUDE_INI_OP:
      value[0] = '\0';
      return value;

    default:
      Error("Unknown INI option in Write_INI.");
  }

  return(value);
}



/*****************************************************************************
*
* FUNCTION
*
*   parse_switch
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
*   Parses a traditional POV-Ray command-line switch that starts
*   with + or -.  Whenever it seemed feasible, calls process_variable
*   to perform the function rather than doing so itself.  Although this
*   requires another pass through a switch{case, case...}, it insures
*   that command-line switches and variable=value options get treated
*   identically.
*
* CHANGES
*
*   -
*
*   Sep 1994 : Added options for union splitting, vista/light buffer. [DB]
*   Jan 1995 : Added options for histogram grid. [CJC]
*   Feb 1995 : Added options for console/file redirection and .INI writing [SCD]
*
******************************************************************************/

void parse_switch (char *Option_String)
{
  int i;
  unsigned long Add_Option;
  unsigned long Option_Number;
  long longval;
  DBL floatval;

  if (*(Option_String++) == '-')
  {
    Add_Option = FALSE;
  }
  else
  {
    Add_Option = TRUE;
  }

  Option_Number = 0;

  switch (*Option_String)
  {
    case '?':

      if (Option_String[1] == '\0')
      {
        Usage(0, TRUE);
      }
      else
      {
        sscanf (&Option_String[1], "%d", &i);

        if ((i >= 0) && (i <= MAX_HELP_PAGE))
        {
          Usage(i, TRUE);
        }
        else
        {
          Usage(0, TRUE);
        }
      }

      break;

    case '@':

      Warning(0.0,"The +@ switch no longer supported. Use +GS.\n");

      break;

    case 'A':
    case 'a':

      switch (Option_String[1])
      {
        case 'm':
        case 'M':

          switch (Option_String[2])
          {
            case '1':

              opts.Tracing_Method = 1;

              break;

            case '2':

              opts.Tracing_Method = 2;

              break;

            default:

              Warning(0.0, "Unknown antialiasing method. Standard method used.\n");

              opts.Tracing_Method = 1;
          }

          break;

        default:

          Option_Number = ANTIALIAS;

          if (sscanf (&Option_String[1], DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
          {
            opts.Antialias_Threshold = floatval;
          }
      }

      break;

    case 'B':
    case 'b':

      process_variable(BUF_SIZE_OP, &Option_String[1]);

      if (opts.File_Buffer_Size > 0)
      {
        Option_Number = BUFFERED_OUTPUT;
      }

      break;

    case 'C':
    case 'c':

      Option_Number = CONTINUE_TRACE;

      break;

    case 'D':
    case 'd':

      Option_Number = DISPLAY;

      if (Option_String[1] != '\0')
      {
        opts.DisplayFormat = (char)toupper(Option_String[1]);
      }

      if (Option_String[1] != '\0' && Option_String[2] != '\0')
      {
        opts.PaletteOption = (char)toupper(Option_String[2]);
      }

      break;

    case 'E':
    case 'e':

      switch (Option_String[1])
      {
        case 'c':
        case 'C':

          process_variable(END_COLUMN_OP,&Option_String[2]);

          break;

        case 'f':
        case 'F':
          if(isdigit((int)Option_String[2])) /* tw */
            process_variable(SUBSET_END_FRAME_OP, &Option_String[2]);
          break;

        case 'r':
        case 'R':

          process_variable(END_ROW_OP,&Option_String[2]);

          break;

        case 'p': /* Mosaic Preview Grid Size - End */
        case 'P':

          process_variable(PREVIEW_E_OP,&Option_String[2]);

          break;

        default:

          process_variable(END_ROW_OP,&Option_String[1]);
      }

      break;

    case 'F':
    case 'f':

      Option_Number = DISKWRITE;
      
      if (Option_String[1] != '\0')
      {
        opts.OutputFormat = (char)tolower(Option_String[1]);
      }

      if (sscanf(&Option_String[2], "%d", &opts.OutputQuality) != 1)
      {
        opts.OutputQuality = 8;
      }

      break;

    /* Console/file redirection, .INI dump option - [SCD 2/95] */

    case 'G':
    case 'g':

      switch (Option_String[1])
      {
        case 'a':  /* All */
        case 'A':

          process_variable(ALL_CONSOLE_OP,Add_Option ? "On" : "Off");
          process_variable(ALL_FILE_OP,&Option_String[2]);

          break;

        case 'd':  /* DebugInfo */
        case 'D':

          process_variable(DEBUG_CONSOLE_OP,Add_Option ? "On" : "Off");
          process_variable(DEBUG_FILE_OP,&Option_String[2]);

          break;

        case 'f':  /* Fatal */
        case 'F':

          process_variable(FATAL_CONSOLE_OP,Add_Option ? "On" : "Off");
          process_variable(FATAL_FILE_OP,&Option_String[2]);

          break;

        case 'i':  /* Create .INI containing all settings */
        case 'I':

          process_variable(CREATE_INI_OP,&Option_String[2]);

          break;

        case 'r':  /* RenderInfo */
        case 'R':

          process_variable(RENDER_CONSOLE_OP,Add_Option ? "On" : "Off");
          process_variable(RENDER_FILE_OP,&Option_String[2]);

          break;

        case 's':  /* Statistics */
        case 'S':

          process_variable(STATISTIC_CONSOLE_OP,Add_Option ? "On" : "Off");
          process_variable(STATISTIC_FILE_OP,&Option_String[2]);

          break;

        case 'w':  /* Warning */
        case 'W':

          process_variable(WARNING_CONSOLE_OP,Add_Option ? "On" : "Off");
          process_variable(WARNING_FILE_OP,&Option_String[2]);

          break;

      }

      break;

    case 'H':
    case 'h':

      if (Help_Available)
      {
        if (Option_String[1] == '\0')
        {
          Usage(0, TRUE);
        }
        else
        {
          sscanf (&Option_String[1], "%d", &Frame.Screen_Height);

          if ((Frame.Screen_Height >= 0) &&  (Frame.Screen_Height <= MAX_HELP_PAGE))
          {
            Usage(Frame.Screen_Height, TRUE);
          }
        }
      }
      else
      {
        if (!isdigit ((int)Option_String [1])) /* tw */
        {
          switch (Option_String [1])
          {
            case 'n':  /* Histogram name */
            case 'N':
                 process_variable(HIST_NAME_OP,&Option_String[2]);
                 break ;

            case 's':  /* Histogram size */
            case 'S':
                 process_variable(HIST_SIZE_OP,&Option_String[2]);
                 break ;

            case 't':  /* Histogram type */
            case 'T':
                 process_variable(HIST_TYPE_OP,&Option_String[2]);
                 break ;
          }
        }
        else
        {
          sscanf (&Option_String[1], "%d", &Frame.Screen_Height);
        }
      }
      break;

    case 'I':
    case 'i':

      if (Option_String[1] == '\0')
      {
        inflag = TRUE;
      }
      else
      {
        process_variable(INPUT_FILE_NAME_OP, &Option_String[1]);
      }

      break;

    case 'J':
    case 'j':

      Option_Number = JITTER;
 
      if (sscanf (&Option_String[1], DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
      {
        opts.JitterScale = floatval;
      }

      if (opts.JitterScale <= 0.0)
      {
        Add_Option = FALSE;
      }

      break;

    case 'K':
    case 'k':

      /* Animation-type clock specification */
      switch(Option_String[1])
      {
        case 'c':
        case 'C':
          Option_Number = CYCLIC_ANIMATION;
          break;

        case 'i':
        case 'I':
          process_variable(INITIAL_CLOCK_OP, &Option_String[2]);
          break;

        case 'f':
        case 'F':
          /* Animation-type clock specification */
          switch(Option_String[2])
          {
            case 'i':
            case 'I':
              process_variable(INITIAL_FRAME_OP, &Option_String[3]);
              break;

            case 'f':
            case 'F':
              process_variable(FINAL_FRAME_OP, &Option_String[3]);
              break;
               
            default:
              process_variable(FINAL_CLOCK_OP, &Option_String[2]);
              break;
 
          }
          break;

        default:
          /* Standard clock specification */
          process_variable(CLOCK_OP,&Option_String[1]);
          break;
      }
      break;

    case 'L':
    case 'l':

      process_variable(LIBRARY_PATH_OP,&Option_String[1]);

      break;

    case 'M': /* Switch used so other max values can be inserted easily */
    case 'm':

      switch (Option_String[1])
      {
        case 's': /* Max Symbols */
        case 'S':

          Warning(0.0,"+MS or -MS switch no longer needed.\n");
          break;

        case 'v': /* Max Version */
        case 'V':

          sscanf (&Option_String[2], DBL_FORMAT_STRING, &opts.Language_Version);

          break;

        case 'b': /* Min Bounded */
        case 'B':

          if (sscanf (&Option_String[2], "%ld", &longval) != SCANF_EOF)
          {
            opts.BBox_Threshold=longval;
          }

          opts.Use_Slabs = Add_Option;

          break;

        default:

          break;
      }

      break;

    /*  "N" option flag is used by networking (multi-processor) options.

    case 'N':
    case 'n':

      break;

     */

    case 'O':
    case 'o':

      if (Option_String[1] == '\0')
      {
        outflag = TRUE;
      }
      else
      {
        process_variable(OUTPUT_FILE_NAME_OP, &Option_String[1]);
      }

      break;

    case 'P':
    case 'p':

      Option_Number = PROMPTEXIT;

      break;

    case 'Q':
    case 'q':

      switch(Option_String[1])
      {
        case 'r':
        case 'R':
          Option_Number = RADIOSITY;
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          process_variable(QUALITY_OP,&Option_String[1]);
          break;
        default:
          break;
      }
      break;

    case 'R':
    case 'r':

      process_variable(ANTIALIAS_DEPTH_OP,&Option_String[1]);

      break;

    case 'S':
    case 's':

      switch (Option_String[1])
      {
        case 'c':
        case 'C':

          process_variable(START_COLUMN_OP,&Option_String[2]);

          break;

        case 'r':
        case 'R':

          process_variable(START_ROW_OP,&Option_String[2]);

          break;

        case 'f':
        case 'F':
          process_variable(SUBSET_START_FRAME_OP, &Option_String[2]);
          break;

        case 'p': /* Mosaic Preview Grid Size - Start */
        case 'P':

          process_variable(PREVIEW_S_OP,&Option_String[2]);

          break;

        /* Split unions option. [DB 9/94] */

        case 'U':
        case 'u':

          Option_Number = SPLIT_UNION;

          break;

        default:

          process_variable(START_ROW_OP,&Option_String[1]);
      }

      break;

    /* Read vista/light buffer options. [DB 9/94] */

    case 'U':
    case 'u':

      switch (Option_String[1])
      {
        case 'l':
        case 'L':

          Option_Number = USE_LIGHT_BUFFER;

          break;

        case 'd':
        case 'D':

          Option_Number = USE_VISTA_DRAW;

          break;

        case 'r':
        case 'R':

           Option_Number = REMOVE_BOUNDS;

           break;

        case 'v':
        case 'V':

          Option_Number = USE_VISTA_BUFFER;

          break;

        case 'a':
        case 'A':

          Option_Number = OUTPUT_ALPHA;

          break;

        case 'f':
        case 'F':

          process_variable(FIELD_RENDER_OP, Add_Option ? "True" : "False");

          break;

        case 'o':
        case 'O':

          process_variable(ODD_FIELD_OP, Add_Option ? "True" : "False");

          break;
      }

      break;

    case 'V':
    case 'v':

      Option_Number = VERBOSE;
      break;

    case 'W':
    case 'w':

      sscanf (&Option_String[1], "%d", &Frame.Screen_Width);

      break;

    case 'X':
    case 'x':

      Option_Number = EXITENABLE;

      sscanf (&Option_String[1], "%d", &Abort_Test_Every);

      opts.Abort_Test_Counter = Abort_Test_Every;

      break;

    default:

      Warning (0.0,"Invalid option: %s.\n", --Option_String);
  }

  if (Option_Number != 0)
  {
    if (Add_Option)
    {
      opts.Options |= Option_Number;
    }
    else
    {
      opts.Options &= ~Option_Number;
    }
  }
}




/*****************************************************************************
*
* FUNCTION
*
*   process_variable
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
*   Given a token number representing an option variable and a string
*   that is the value to set, set one option.  If its just an on/off
*   switch that takes a boolean value then just set Option_Number and
*   break.  Otherwise process the value and return. 
*
* CHANGES
*
*   -
*
******************************************************************************/

void process_variable(TOKEN variable,char *value)
{
  int i;
  long longval;
  unsigned int Option_Number = 0;
  DBL floatval;

  switch (variable)
  {
    case BUF_SIZE_OP:
      if (sscanf (value, "%d", &opts.File_Buffer_Size) != SCANF_EOF)
      {
        opts.File_Buffer_Size *= 1024;

        if (opts.File_Buffer_Size > MAX_BUFSIZE)
          opts.File_Buffer_Size = MAX_BUFSIZE;

        /* If 0 then no buffer, other low values use system default MIN */
        if ((opts.File_Buffer_Size > 0) && (opts.File_Buffer_Size < BUFSIZ))
        {
          opts.File_Buffer_Size = BUFSIZ;
        }
        if (opts.File_Buffer_Size <= 0)
        {
          opts.Options &= ~BUFFERED_OUTPUT;
        }
      }
      return;

    case BUFFERED_OUTPUT_OP:
      Option_Number = BUFFERED_OUTPUT;
      break;

    case CONTINUE_OP:
      Option_Number = CONTINUE_TRACE;
      break;

    case DISPLAY_OP:
      Option_Number = DISPLAY;
      break;

    case VIDEO_MODE_OP:
      opts.DisplayFormat = (char)toupper(value[0]);
      return;

    case PALETTE_OP:
      opts.PaletteOption = (char)toupper(value[0]);
      return;

    case VERBOSE_OP:
      Option_Number = VERBOSE;
      break;

    case WIDTH_OP:
      Frame.Screen_Width = atoi(value);
      return;

    case HEIGHT_OP:
      Frame.Screen_Height = atoi(value);
      return;

    case FILE_OUTPUT_OP:
      Option_Number = DISKWRITE;
      break;

    case FILE_OUTPUT_TYPE_OP:
      opts.OutputFormat = (char)tolower(value[0]);
      return;

    case PAUSE_WHEN_DONE_OP:
      Option_Number = PROMPTEXIT;
      break;

    case INPUT_FILE_NAME_OP:
      if (!strcmp(value, "-") || !strcmp(value, "stdin"))
      {
        strcpy (opts.Input_File_Name, "stdin");
        opts.Options |= FROM_STDIN;
      }
      else
      {
        strncpy (opts.Input_File_Name, value, FILE_NAME_LENGTH);
      }
      return;

    case OUTPUT_FILE_NAME_OP:
      if (!strcmp(value, "-") || !strcmp(value, "stdout"))
      {
        strcpy (opts.Output_File_Name, "stdout");
        opts.Options |= TO_STDOUT;
      }
      else
      {
        strncpy (opts.Output_File_Name, value, FILE_NAME_LENGTH);
      }
      return;

    case ANTIALIAS_OP:
      Option_Number = ANTIALIAS;
      break;

    case ANTIALIAS_THRESH_OP:
      if (sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
        opts.Antialias_Threshold = floatval;
      return;

    case ANTIALIAS_DEPTH_OP:
      if (sscanf (value, "%ld", &longval) != SCANF_EOF)
        opts.AntialiasDepth = longval;
      if (opts.AntialiasDepth < 1)
        opts.AntialiasDepth = 1;
      if (opts.AntialiasDepth > 9)
        opts.AntialiasDepth = 9;
      return;

    case JITTER_OP:
      Option_Number = JITTER;
      break;

    case JITTER_AMOUNT_OP:
      if (sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
        opts.JitterScale = floatval;
      if (opts.JitterScale<=0.0)
        opts.Options &= ~JITTER;
      return;

    case TEST_ABORT_OP:
      Option_Number = EXITENABLE;
      break;

    case TEST_ABORT_COUNT_OP:
      sscanf (value, "%d", &Abort_Test_Every);
      opts.Abort_Test_Counter = Abort_Test_Every;
      break;

    case LIBRARY_PATH_OP:
      if (opts.Library_Path_Index >= MAX_LIBRARIES)
        Error ("Too many library directories specified.");
      for (i = 0; i < opts.Library_Path_Index; i++)
        if (strcmp(value,opts.Library_Paths[i])==0) return;
      opts.Library_Paths[opts.Library_Path_Index] = (char *)POV_MALLOC(strlen(value)+1,
                 "library paths");
      strcpy (opts.Library_Paths [opts.Library_Path_Index], value);
      opts.Library_Path_Index++;
      return;

    case START_COLUMN_OP:
      if (sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
      { /* tw */
        if(floatval > 0.0 && floatval < 1.0)
        {
          opts.First_Column = -1;
          opts.First_Column_Percent = floatval;
        }
        else
          opts.First_Column = ((int) floatval);
          /* The above used to have -1 but it messed up Write_INI_File.
           * Moved -1 fudge to fix_up_rendering_window 
           */
      } /* tw */
      return;

    case START_ROW_OP:
      if (sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
      { /* tw */
        if(floatval > 0.0 && floatval < 1.0)
        {
          opts.First_Line = -1;
          opts.First_Line_Percent = floatval;
        }
        else
          opts.First_Line = ((int) floatval);
          /* The above used to have -1 but it messed up Write_INI_File
          * Moved -1 fudge to fix_up_rendering_window 
          */
      } /* tw */
      return;

    case END_COLUMN_OP:
      { /* tw */
      if (sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
        if(floatval > 0.0 && floatval <= 1.0)
        {
          opts.Last_Column = -1;
          opts.Last_Column_Percent = floatval;
        }
        else
          opts.Last_Column = (int) floatval;
      } /* tw */
      return;

    case END_ROW_OP:
      { /* tw */ 
      if (sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
        if(floatval > 0.0 && floatval <= 1.0)
        {
          opts.Last_Line = -1;
          opts.Last_Line_Percent = floatval;
        }
        else
          opts.Last_Line = (int) floatval;
      } /* tw */
      return;

    case VERSION_OP:
      if (sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
        opts.Language_Version = floatval;
      return;

    case BOUNDING_OP:
      opts.Use_Slabs = istrue(value);
      return;

    case BOUNDING_THRESH_OP:
      opts.BBox_Threshold = atoi(value);
      return;

    case QUALITY_OP:
      opts.Quality = atoi(value);
      /* Emit a warning about the "radiosity" quality levels for
       * now.  We can get rid of this some time in the future.
       */
      if ((opts.Quality == 10) || (opts.Quality == 11))
      {
         Warning(0.0, "Quality settings 10 and 11 are no longer valid.\n"
                      "Use +QR if you need radiosity.\n");
         opts.Quality = 9;
      }
      else if ((opts.Quality < 0) || (opts.Quality > 9))
      {
         Error("Illegal Quality setting.");
      }
      opts.Quality_Flags = Quality_Values[opts.Quality];
      return;

    case CLOCK_OP:
      if(sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
      {
        opts.FrameSeq.Clock_Value = floatval;
      }
      return;

    case INITIAL_FRAME_OP:
      if(sscanf(value, "%ld", &longval)!=SCANF_EOF)
      {
        opts.FrameSeq.InitialFrame=longval;
      }
      return;

    case INITIAL_CLOCK_OP:
      if(sscanf(value, DBL_FORMAT_STRING, &floatval)!=SCANF_EOF)
      {
        opts.FrameSeq.InitialClock=floatval;
      }
      return;

    case FINAL_FRAME_OP:
      if(sscanf(value, "%ld", &longval)!=SCANF_EOF)
      {
        opts.FrameSeq.FinalFrame=longval;
      }
      return;

    case FINAL_CLOCK_OP:
      if(sscanf(value, DBL_FORMAT_STRING, &floatval)!=SCANF_EOF)
      {
        opts.FrameSeq.FinalClock=floatval;
      }
      return;

    case SUBSET_START_FRAME_OP:
      if (sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
      {
        if(floatval > 0.0 && floatval < 1.0)
          opts.FrameSeq.SubsetStartPercent=floatval;
        else
          opts.FrameSeq.SubsetStartFrame=(int)floatval;
      }
      return;

    case SUBSET_END_FRAME_OP:
      if (sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
      {
        if(floatval > 0.0 && floatval < 1.0)
          opts.FrameSeq.SubsetEndPercent=floatval;
        else
          opts.FrameSeq.SubsetEndFrame=(int)floatval;
      }
      return;

    case PREVIEW_S_OP:
      opts.PreviewGridSize_Start = atoi(value);
      return;

    case PREVIEW_E_OP:
      opts.PreviewGridSize_End = atoi(value);
      return;

    case CREATE_INI_OP:
      strcpy(opts.Ini_Output_File_Name,value);
      return;

    case ALL_CONSOLE_OP:
      Stream_Info[ALL_STREAM].do_console =
      Stream_Info[WARNING_STREAM].do_console =
      Stream_Info[STATISTIC_STREAM].do_console =
      Stream_Info[RENDER_STREAM].do_console = 
      Stream_Info[FATAL_STREAM].do_console = 
      Stream_Info[DEBUG_STREAM].do_console = istrue(value);
      return;

    case ALL_FILE_OP:
      Do_Stream_Option(ALL_STREAM,value);
      return;

    case DEBUG_CONSOLE_OP:
      Stream_Info[DEBUG_STREAM].do_console = istrue(value);
      return;

    case DEBUG_FILE_OP:
      Do_Stream_Option(DEBUG_STREAM,value);
      return;

    case FATAL_CONSOLE_OP:
      Stream_Info[FATAL_STREAM].do_console = istrue(value);
      return;

    case FATAL_FILE_OP:
      Do_Stream_Option(FATAL_STREAM,value);
      return;

    case RENDER_CONSOLE_OP:
      Stream_Info[RENDER_STREAM].do_console = istrue(value);
      return;

    case RENDER_FILE_OP:
      Do_Stream_Option(RENDER_STREAM,value);
      return;

    case STATISTIC_CONSOLE_OP:
      Stream_Info[STATISTIC_STREAM].do_console = istrue(value);
      return;

    case STATISTIC_FILE_OP:
      Do_Stream_Option(STATISTIC_STREAM,value);
      return;

    case WARNING_CONSOLE_OP:
      Stream_Info[WARNING_STREAM].do_console = istrue(value);
      return;

    case WARNING_FILE_OP:
      Do_Stream_Option(WARNING_STREAM,value);
      return;

    case RAD_SWITCH_OP:
      Option_Number = RADIOSITY;
      break;

    case HIST_SIZE_OP:
      if (sscanf (value, "%d.%d", &opts.histogram_x, &opts.histogram_y) == SCANF_EOF)
      {
        Warning (0.0, "Error occurred scanning histogram grid size '%s'.\n", value) ;
        opts.histogram_on = FALSE ;
      }
      break ;

    case HIST_TYPE_OP:
#if PRECISION_TIMER_AVAILABLE
    {
      char *def_ext = NULL;

      switch (*value)
      {
        case 'C' :
        case 'c' :
             opts.histogram_on = TRUE ;
             opts.histogram_type = CSV ;
             def_ext = ".csv";
             break ;
        case 'S' :
        case 's' :
             opts.histogram_on = TRUE ;
             opts.histogram_type = SYS ;
             Histogram_File_Handle = GET_SYS_FILE_HANDLE () ;
             def_ext = SYS_DEF_EXT;
             break ;
        case 'P' :
        case 'p' :
             opts.histogram_on = TRUE ;
             opts.histogram_type = PPM ;
             Histogram_File_Handle = Get_PPM_File_Handle () ;
             def_ext = ".ppm";
             break ;
        case 'T' :
        case 't' :
             opts.histogram_on = TRUE ;
             opts.histogram_type = TARGA ;
             Histogram_File_Handle = Get_Targa_File_Handle () ;
             def_ext = ".tga";
             break ;
        case 'n':
        case 'N':
             opts.histogram_on = TRUE ;
             opts.histogram_type = PNG ;
             Histogram_File_Handle = Get_Png_File_Handle () ;
             def_ext = ".png";
             break ;
        case 'x':
        case 'X':
             opts.histogram_on = FALSE ;
             break ;
        default :
             Warning (0.0, "Unknown histogram output type '%c'.\n", *value) ;
             opts.histogram_on = FALSE ;
             break ;
      }

      /* Process the histogram file name now, if it hasn't
       * yet been specified, and in case it isn't set later.
       */
      if (opts.histogram_on && opts.Histogram_File_Name[0] == '\0')
      {
        sprintf(opts.Histogram_File_Name, "histgram%s", def_ext);
      }
    }
#else  /* !PRECISION_TIMER_AVAILABLE */
      if (*value != 'x' && *value != 'X')
        Warning(0.0,"Histogram output unavailable in this compile of POV-Ray");
      opts.histogram_on = FALSE;
#endif /* PRECISION_TIMER_AVAILABLE */
      break ;

    case HIST_NAME_OP:
      if (opts.histogram_on && value[0] == '\0')
      {
        char *def_ext = NULL;

        switch (opts.histogram_type)
        {
          case CSV:   def_ext = ".csv"; break;
          case TARGA: def_ext = ".tga"; break;
          case PNG:   def_ext = ".png"; break;
          case PPM:   def_ext = ".ppm"; break;
          case SYS:   def_ext = SYS_DEF_EXT; break;
          case NONE:  def_ext = "";     break;  /* To quiet warnings */
        }

        sprintf(opts.Histogram_File_Name, "histgram%s", def_ext);
      }
      else
      {
        strncpy (opts.Histogram_File_Name, value, FILE_NAME_LENGTH);
      }
      break ;

    case VISTA_BUFFER_OP:
      Option_Number = USE_VISTA_BUFFER;
      break;

    case LIGHT_BUFFER_OP:
      Option_Number = USE_LIGHT_BUFFER;
      break;

    case DRAW_VISTAS_OP:
      Option_Number = USE_VISTA_DRAW;
      break;

    case SPLIT_UNIONS_OP:
      Option_Number = SPLIT_UNION;
      break;

    case REMOVE_BOUNDS_OP:
      Option_Number = REMOVE_BOUNDS;
      break;

    case CYCLIC_ANIMATION_OP:
      Option_Number = CYCLIC_ANIMATION;
      break;

    case PRE_SCENE_CMD_OP:
      strcpy(opts.Shellouts[PRE_SCENE_SHL].Command, value);
      break;

    case PRE_FRAME_CMD_OP:
      strcpy(opts.Shellouts[PRE_FRAME_SHL].Command, value);
      break;

    case POST_FRAME_CMD_OP:
      strcpy(opts.Shellouts[POST_FRAME_SHL].Command, value);
      break;

    case POST_SCENE_CMD_OP:
      strcpy(opts.Shellouts[POST_SCENE_SHL].Command, value);
      break;

    case USER_ABORT_CMD_OP:
      strcpy(opts.Shellouts[USER_ABORT_SHL].Command, value);
      break;

    case FATAL_ERROR_CMD_OP:
      strcpy(opts.Shellouts[FATAL_SHL].Command, value);
      break;

    case PRE_SCENE_RET_OP:
      Do_Return_Option(PRE_SCENE_SHL, value);
      break;

    case PRE_FRAME_RET_OP:
      Do_Return_Option(PRE_FRAME_SHL, value);
      break;

    case POST_FRAME_RET_OP:
      Do_Return_Option(POST_FRAME_SHL, value);
      break;

    case POST_SCENE_RET_OP:
      Do_Return_Option(POST_SCENE_SHL, value);
      break;

    case USER_ABORT_RET_OP:
      Do_Return_Option(USER_ABORT_SHL, value);
      break;

    case FATAL_ERROR_RET_OP:
      Do_Return_Option(FATAL_SHL, value);
      break;

    case OUTPUT_ALPHA_OP:
      Option_Number = OUTPUT_ALPHA;
      break;

    case FIELD_RENDER_OP:
      opts.FrameSeq.Field_Render_Flag = istrue(value);
      return;

    case ODD_FIELD_OP:
      opts.FrameSeq.Odd_Field_Flag = istrue(value);
      return;

    case SAMPLING_METHOD_OP:
      opts.Tracing_Method = atoi(value);
      return;

    case BITS_PER_COLOR_OP:
    case BITS_PER_COLOUR_OP:
      opts.OutputQuality = atoi(value);
      opts.OutputQuality = max(5,  opts.OutputQuality);
      opts.OutputQuality = min(16, opts.OutputQuality);
      return;

    case DISPLAY_GAMMA_OP:
      if (sscanf (value, DBL_FORMAT_STRING, &floatval) != SCANF_EOF)
      {
        if (floatval > 0.0)
          opts.DisplayGamma = floatval;
      }
      return;

    case INCLUDE_INI_OP:
      if (!parse_ini_file(value))
      {
        Error ("Could not open Include_Ini='%s'.\n", value);
      }
      return;

    default:
      Warning(0.0,"Unimplemented INI '%s'.\n",Option_Variable[variable].Token_Name);
      return;

  }

  if (Option_Number != 0)
  {
    if (istrue(value))
    {
      opts.Options |= Option_Number;
    }
    else
    {
      opts.Options &= ~Option_Number;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   istrue
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

static int matches(char *v1,char  *v2)
{
  int i=0;
  int ans=TRUE;
  
  while ((ans) && (v1[i] != '\0') && (v2[i] != '\0'))
  {
    ans = ans && (v1[i] == tolower(v2[i]));
    i++;
  }
  
  return(ans);
}

static int istrue(char *value)
{
   return (matches("on",value)  || matches("true",value) || 
           matches("yes",value) || matches("1",value));
}

static int isfalse(char *value)
{
   return (matches("off",value)  || matches("false",value) || 
           matches("no",value)   || matches("0",value));
}


/*****************************************************************************
*
* FUNCTION
*
*   Write_INI_File
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   SCD, 2/95
*   
* DESCRIPTION
*
*   Writes all options to an INI file with the current input filename's name
*   (by default), or using a specified filename.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Write_INI_File()
{
  int  op, i;
  char ini_name[FILE_NAME_LENGTH];
  FILE *ini_file;

  if (opts.Ini_Output_File_Name[0]=='\0')
  {
    return;
  }

  if (isfalse(opts.Ini_Output_File_Name))
  {
    return;
    }

  Status_Info("\nWriting INI file...");

  if (istrue(opts.Ini_Output_File_Name))
      {
    strcpy(ini_name,opts.Scene_Name);
    strcat(ini_name,".ini");
      }
      else
      {
    strcpy(ini_name,opts.Ini_Output_File_Name);
  }

  if ((ini_file = fopen(ini_name, WRITE_TXTFILE_STRING)) == NULL)
  {
    Warning (0.0,"Error opening .INI output file '%s' - no file written.\n",
                    ini_name);

    return;
  }

  for (op = 0; op < MAX_OPTION; op++)
  {
    if (op == LIBRARY_PATH_OP)
    {
      for (i = 0; i < opts.Library_Path_Index; i++)
      {
        fprintf(ini_file,"%s=%s%s",Option_Variable[op].Token_Name,
                   get_ini_value(op, i),NEW_LINE_STRING);
      }
    }
    /* So that we don't get both Bits_Per_Color and Bits_Per_Colour in
     * the INI file. */
    else if ((op != BITS_PER_COLOUR_OP) && (op != INCLUDE_INI_OP))
    {
      fprintf(ini_file,"%s=%s%s",Option_Variable[op].Token_Name,
                   get_ini_value(op, 0),NEW_LINE_STRING);
    }
  }

  fclose(ini_file);
}



/*****************************************************************************
*
* FUNCTION
*
*   parse_ini_file
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
*   Given a file name, open it, parse options from it, close it.
*   Return 1 if file found, 0 if not found.
*
* CHANGES
*
*   -
*
******************************************************************************/

int parse_ini_file(char *File_Name)
{
  char Option_Line[512];
  char INI_Name[FILE_NAME_LENGTH];
  char Desired_Section[FILE_NAME_LENGTH];
  char Current_Section[FILE_NAME_LENGTH];
  char *source, *dest;
  FILE *ini_file;
  int Matched, Never_Matched;

  Stage=STAGE_INI_FILE;
  
  /* File_Name can be of the for "FILE.INI[Section]" where everything
   * before the '[' is the actual name and "[Section]" is the title of
   * a section within that file that starts with the [Section] heading.  
   * Only the specified section of the INI file is processed.  If no
   * section is specified then only parts of the file without a section
   * header are processed.
   */
   
  /* Copy the file name part */
  source=File_Name;
  dest=INI_Name;
  while ((*source != '\0') && (*source != '['))
  {
    *(dest++) = *(source++);
  }
  *dest = '\0';

  /* Copy the section name part */
  dest = Desired_Section;
  while ((*source != '\0') && (*source != ']'))
  {
    *(dest++) = *(source++);
  }
  *dest = *source;
  *(++dest)='\0';

  if ((ini_file = Locate_File(INI_Name, READ_TXTFILE_STRING,".ini",".INI",NULL,FALSE)) == NULL)
  {
    return(FALSE);
  }
  
  *Current_Section='\0';
  
  Matched = (*Desired_Section == '\0');
  Never_Matched=TRUE;
  
  while (fgets(Option_Line, 512, ini_file) != NULL)
  {
    if (*Option_Line == '[')
    {
      source=Option_Line;
      dest=Current_Section;
      while ((*source != '\0') && (*source != ']'))
      {
        *(dest++) = *(source++);
      }
      *dest = *source;
      *(++dest)='\0';
      Matched = (pov_stricmp(Current_Section, Desired_Section) == 0);
      
    }
    else
    {    
       if (Matched)
       {
          parse_option_line(Option_Line);
          Never_Matched=FALSE;
       }
    }
  }

  if (Never_Matched)
  {
     Warning(0.0,"Never found section %s in file %s.\n",Desired_Section,INI_Name);
  }
  
  fclose(ini_file);

  return(TRUE);
}


/*****************************************************************************
*
* FUNCTION
*
*   parse_option_line
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
*   Given a string containing a line of text, split it into individual
*   switches or options and then pass them off to be parsed by parse_switch
*   or process_variable.  This routine is called by parse_ini_file,
*   by the main with argv[] and by READ_ENV_VAR_????
*
* CHANGES
*
*   Mar 1996 : Allow ';' in an option, if it is escaped  [AED]
*
******************************************************************************/

void parse_option_line(char *Option_Line)
{
  char *source, *dest;
  char Option_String[512];
  int i,Found;

  source =  Option_Line;

  while (TRUE)
  {
    /* skip leading white space */
    while (((int)*source > 0) && ((int)*source < 33))
    {
      source++;
    }

    /* Quit when finished or ignore if commented */
    if ((*source == '\0') || (*source == ';'))
    {
      return;
    }
    
    if ((*source == '=') || (*source == '#'))
    {
      Error("'=' or '#' must be preceded by a keyword.");
    }

    /* Copy everything that is not a space, an equals or a comment 
       into Option_String 
    */
    dest = Option_String;
    while ((isprint((int)*source)) && 
           (*source != ' ') && 
           (*source != '=') && 
           (*source != '#') && 
           (*source != ';'))
    {
      *(dest++) = *(source++);
    }
    *dest = '\0';

    /* At this point, nearly all options that start with a "-" are
     * options. However the syntax for the +I and the +O 
     * command-line switches, allows a space to appear between the 
     * switch and the name.  For example: "+I MYFILE.POV" is legal.  
     * Since "-" is a legal input and output filename for stdin or
     * stdout we must parse this before we try to read other options.
     * If we encounter a "-" without a following option immediately
     * after a +I or +O switch, it must mean that.  The flags
     * inflag and outflag indicate that there was a +I or +O switch
     * parsed just before this file name.
     */

    if (inflag)
    {
      inflag = FALSE;
      if (pov_stricmp(Option_String, "-") == 0 ||
          pov_stricmp(Option_String, "stdin") == 0)
      {
        strcpy (opts.Input_File_Name, "stdin");
        opts.Options |= FROM_STDIN;
        continue;
      }
      else if ((*Option_String != '+') && (*Option_String != '-'))
      {
        strncpy (opts.Input_File_Name, Option_String, FILE_NAME_LENGTH);
        continue;
      }
    }

    if (outflag)
    {
      outflag = FALSE;
      if (pov_stricmp(Option_String, "-") == 0 ||
          pov_stricmp(Option_String, "stdout") == 0)
      {
        strcpy (opts.Output_File_Name, "stdout");
        opts.Options |= TO_STDOUT;
        continue;
      }
      else if ((*Option_String != '+') && (*Option_String != '-'))
      {
        strncpy (opts.Output_File_Name, Option_String, FILE_NAME_LENGTH);
        continue;
      }
    }
    
    /* If its a +/- style switch then just do it */
    if ((*Option_String == '+') || (*Option_String == '-'))
    {
      parse_switch(Option_String);
      continue;
    }

    /* Now search the Option_Variables to see if we find a match */

    Found=-1;
    for (i = 0 ; i < MAX_OPTION; i++)
    {
      if (pov_stricmp(Option_Variable[i].Token_Name, Option_String) == 0)
      {
        Found=Option_Variable[i].Token_Number;
        break;
      }
    }
    
    if (Found < 0)
    {
      /* When an option string does not begin with a '+' or '-', and
       * is not in the list of valid keywords, then it is assumed to 
       * be a file name.  Any file names that appear at this point are
       * .INI/.DEF files to be parsed, or it is an error.
       */
       
      if (++Number_Of_Files > MAX_NESTED_INI)
      {
        Error ("Bad option syntax or too many nested .INI/.DEF files.");
      }

      if (!parse_ini_file(Option_String))
      {
        Error ("Bad option syntax or error opening .INI/.DEF file '%s'.\n", Option_String);
      }
      continue;
    }
    
    /* If we make it this far, then it must be an .INI-style setting with
     * the keyword already verified in "Found".  We now need to verify that
     * an equals sign follows.  
     */

    /* skip white space */
    while (((int)*source > 0) && ((int)*source < 33))
    {
      source++;
    }
    
    if ((*source != '=') && (*source != '#'))
    {
      Error("Missing '=' or '#' after %s in option.",Option_String);
    }
    
    source++;

    /* Now the entire rest of Option_Line up to but excluding a comment, 
     * becomes the variable part of the option. 
     */

    /* skip white space */
    while (((int)*source > 0) && ((int)*source < 33))
    {
      source++;
    }
    
    dest=source;
    
    /* Cut off comments and any unprintable characters */
    while (*source != '\0') 
    {
       /* If the comment character is escaped, pass it through */
       if ((*source == '\\') && (*(source + 1) == ';'))
       {
         *source = ';';
         source++;
         *source = ' ';
         source++;
       }
       else if ((*source == ';') || (! isprint((int)*source) ) )
       {
         *source = '\0';
       }
       else
       {
         source++;
       }
    }
    process_variable(Found, dest);
    return;
  }
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

void Do_Stream_Option (int i, char *value)
{
  if (value==NULL)
  {
    return;
  }

  if (*value == '\0')
  {
    return;
  }

  if (Stream_Info[i].name != NULL)
  {
    POV_FREE(Stream_Info[i].name);

    Stream_Info[i].name = NULL;
  }

  if (istrue(value))
  {
    Stream_Info[i].name = (char *)POV_MALLOC(strlen(DefaultFile[i])+1, "stream name");

    strcpy(Stream_Info[i].name, DefaultFile[i]);
  }
  else
  {
    if (isfalse(value))
    {
      return;
    }

    Stream_Info[i].name = (char *)POV_MALLOC(strlen(value)+1, "stream name");

    strcpy(Stream_Info[i].name, value);
 }
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

void Do_Return_Option (SHELLTYPE Type, char *value)
{
   char *s;
   
   SHELLDATA *Shell=&(opts.Shellouts[Type]);
   
   Shell->Inverse=FALSE;
   Shell->Ret=IGNORE_RET;
   
   if (value==NULL)
     return;
     
   if (*value=='\0')
     return;
     
   if ((*value=='-') || (*value=='!'))
   {
      Shell->Inverse=TRUE;
      value++;
   }

   if (*value=='\0')
     return;

   if ((s=strchr(ret_string,toupper(*value))) == NULL)
   {
     Warning(0.0,"Bad value in shellout return '%c'. Only '%s' are allowed.\n",*value,ret_string);
     Shell->Ret = IGNORE_RET;
   }
   else
   {
     Shell->Ret = (POV_SHELLOUT_CAST)(s-ret_string);
   }
}
