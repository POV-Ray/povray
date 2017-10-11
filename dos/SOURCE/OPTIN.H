/****************************************************************************
*                   optin.h
*
*  This module contains all defines, typedefs, and prototypes for OPTIN.C.
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


#ifndef OPTIN_H
#define OPTIN_H

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Max. number of nested ini-files. */

#define MAX_NESTED_INI 10

/*****************************************************************************
* Global typedefs
******************************************************************************/

/* Defines for the ini-parser. */

typedef enum ini_op
{
  ALL_CONSOLE_OP=0,
  ALL_FILE_OP,
  ANTIALIAS_DEPTH_OP,
  ANTIALIAS_OP,
  ANTIALIAS_THRESH_OP,
  BOUNDING_OP,
  BOUNDING_THRESH_OP,
  BUFFERED_OUTPUT_OP,
  BUF_SIZE_OP,

  CLOCK_OP,
  CONTINUE_OP,
  CREATE_INI_OP,
  CYCLIC_ANIMATION_OP,
  DEBUG_CONSOLE_OP,
  DEBUG_FILE_OP,
  DISPLAY_OP,
  DISPLAY_GAMMA_OP,
  DRAW_VISTAS_OP,
  END_COLUMN_OP,
  END_ROW_OP,

  FATAL_CONSOLE_OP,
  FATAL_ERROR_CMD_OP,
  FATAL_ERROR_RET_OP,
  FATAL_FILE_OP,
  FIELD_RENDER_OP,
  FILE_OUTPUT_OP,
  FILE_OUTPUT_TYPE_OP,
  FINAL_CLOCK_OP,
  FINAL_FRAME_OP,
  HEIGHT_OP,

  HIST_NAME_OP,
  HIST_SIZE_OP,
  HIST_TYPE_OP,
  INITIAL_CLOCK_OP,
  INITIAL_FRAME_OP,
  INPUT_FILE_NAME_OP,
  JITTER_AMOUNT_OP,
  JITTER_OP,
  LIBRARY_PATH_OP,
  LIGHT_BUFFER_OP,

  ODD_FIELD_OP,
  OUTPUT_ALPHA_OP,
  OUTPUT_FILE_NAME_OP,
  PALETTE_OP,
  PAUSE_WHEN_DONE_OP,
  POST_FRAME_CMD_OP,
  POST_FRAME_RET_OP,
  POST_SCENE_CMD_OP,
  POST_SCENE_RET_OP,
  PREVIEW_E_OP,

  PREVIEW_S_OP,
  PRE_FRAME_CMD_OP,
  PRE_FRAME_RET_OP,
  PRE_SCENE_CMD_OP,
  PRE_SCENE_RET_OP,
  QUALITY_OP,
  RAD_SWITCH_OP,
  REMOVE_BOUNDS_OP,
  RENDER_CONSOLE_OP,
  RENDER_FILE_OP,

  SAMPLING_METHOD_OP,
  SPLIT_UNIONS_OP,
  START_COLUMN_OP,
  START_ROW_OP,
  STATISTIC_CONSOLE_OP,
  STATISTIC_FILE_OP,
  SUBSET_END_FRAME_OP,
  SUBSET_START_FRAME_OP,
  TEST_ABORT_COUNT_OP,
  TEST_ABORT_OP,

  USER_ABORT_CMD_OP,
  USER_ABORT_RET_OP,
  VERBOSE_OP,
  VERSION_OP,
  VIDEO_MODE_OP,
  VISTA_BUFFER_OP,
  WARNING_CONSOLE_OP,
  WARNING_FILE_OP,
  WIDTH_OP,

  BITS_PER_COLOR_OP,
  BITS_PER_COLOUR_OP,
  INCLUDE_INI_OP,

  MAX_OPTION
} INI_OP;

/*****************************************************************************
* Global variables
******************************************************************************/

extern long Quality_Values[];
extern struct Reserved_Word_Struct Option_Variable [] ;

/*****************************************************************************
* Global functions
******************************************************************************/

char *get_ini_value (int op, int libind);
void parse_switch (char *Option_String);
void process_variable (TOKEN variable,char *value);
void Do_Stream_Option (int i, char *value);
void Do_Return_Option (SHELLTYPE Type, char *value);
int parse_ini_file (char *File_Name);
void parse_option_line (char *Option_Line);
void Write_INI_File (void);

#endif
