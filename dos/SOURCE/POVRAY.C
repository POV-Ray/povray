/****************************************************************************
*                povray.c
*
*  This module contains the entry routine for the raytracer and the code to
*  parse the parameters on the command line.
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

#include <ctype.h>
#include <time.h>     /* BP */
#include "frame.h"    /* common to ALL modules in this program */
#include "povproto.h"
#include "bezier.h"
#include "blob.h"
#include "bbox.h"
#include "cones.h"
#include "csg.h"
#include "discs.h"
#include "express.h"
#include "fractal.h"
#include "hfield.h"
#include "lathe.h"
#include "lighting.h"
#include "mem.h"
#include "mesh.h"
#include "polysolv.h"
#include "objects.h"
#include "octree.h"
#include "parse.h"
#include "pigment.h"
#include "point.h"
#include "poly.h"
#include "polygon.h"
#include "povray.h"
#include "optin.h"
#include "optout.h"
#include "quadrics.h"
#include "pgm.h"
#include "png_pov.h"
#include "ppm.h"
#include "prism.h"
#include "radiosit.h"
#include "render.h"
#include "sor.h"
#include "spheres.h"
#include "super.h"
#include "targa.h"
#include "texture.h"
#include "tokenize.h"
#include "torus.h"
#include "triangle.h"
#include "truetype.h"
#include "userio.h"     /*Error,Warning,Init_Text_Streams*/
#include "lbuffer.h"
#include "vbuffer.h"


/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Flags for the variable store. */

#define STORE   1
#define RESTORE 2



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

/* The frame. */

FRAME Frame;

/* Options and display stuff. */

char Color_Bits;

int Display_Started;

int Abort_Test_Every;

int Experimental_Flag;

/* Current stage of the program. */

int Stage;

/* Flag if -h option will show help screens. */

int Help_Available;

/* File and parsing stuff. */

Opts opts;
COUNTER stats[MaxStat];
COUNTER totalstats[MaxStat];

int Num_Echo_Lines;      /* May make user setable later - CEY*/
int Echo_Line_Length;    /* May make user setable later - CEY*/

int Number_Of_Files;

FILE *stat_file;
FILE_HANDLE *Output_File_Handle;

char Actual_Output_Name[FILE_NAME_LENGTH];

/* Timing stuff .*/

time_t tstart, tstop;
DBL tparse, trender, tparse_total, trender_total;

/* Variable used by vector macros. */

DBL VTemp;
volatile int Stop_Flag;

/* Flag if close_all() was already called. */
static int closed_flag;

int pre_init_flag=0;

char *Option_String_Ptr;      
DBL Clock_Delta;

/*****************************************************************************
* Static functions
******************************************************************************/

static void init_vars (void);
static void destroy_libraries (void);
static void fix_up_rendering_window (void);
static void fix_up_animation_values (void);
static void fix_up_scene_name (void);
static void set_output_file_handle (void);
static void setup_output_file_name (void);
static void open_output_file (void);
static void FrameRender (void);
static void init_statistics (COUNTER *);
static void sum_statistics (COUNTER *, COUNTER *);
static void variable_store (int Flag);
static int Has_Extension (char *name);
static unsigned closest_power_of_2 (unsigned theNumber);
static void init_shellouts (void);
static void destroy_shellouts (void);


/*****************************************************************************
*
* FUNCTION
*
*   main
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


#ifdef NOCMDLINE    /* a main() by any other name... */
#ifdef ALTMAIN
  MAIN_RETURN_TYPE alt_main()
#else
  MAIN_RETURN_TYPE main()
#endif
#else
#ifdef ALTMAIN
  MAIN_RETURN_TYPE alt_main(int argc, char **argv)
#else
  MAIN_RETURN_TYPE main(int argc, char **argv)
#endif
#endif            /* ...would be a lot less hassle!! :-) AAC */
{
  register int i;
  int Diff_Frame;
  DBL Diff_Clock;
  SHELLRET Pre_Scene_Result, Frame_Result;

  /* Attention all ALTMAIN people! See comments attached to this function*/
  pre_init_povray();

  /* Startup povray. */
  Stage = STAGE_STARTUP;
  STARTUP_POVRAY

  /* Print banner and credit info. */
  Stage = STAGE_BANNER;
  PRINT_CREDITS
  PRINT_OTHER_CREDITS

#ifndef NOCMDLINE
  /* Print help screens. */

  if (argc == 1)
  {
    Print_Help_Screens();
  }
#endif

  /* Initialize variables. */
  init_vars();

  Stage = STAGE_ENVIRONMENT;

  READ_ENV_VAR

  Stage = STAGE_INI_FILE;

  /* Read parameters from POVRAY.INI */
  PROCESS_POVRAY_INI

#ifndef NOCMDLINE
  /* Parse the command line parameters */

  Stage = STAGE_COMMAND_LINE;

  Help_Available = (argc == 2);

  for (i = 1 ; i < argc ; i++ )
  {
    parse_option_line(argv[i]);
  }
#endif

  /* Strip path and extension off input name to create scene name */
  fix_up_scene_name ();
  
  /* Redirect text streams [SCD 2/95] */
  Open_Text_Streams();

  /* Write .INI file [SCD 2/95] */
  Write_INI_File();
  
  ALT_WRITE_INI_FILE

  /* Make sure clock is okay, validate animation parameters */
  fix_up_animation_values();

  /* Fix-up rendering window values if necessary. */
  fix_up_rendering_window();
  
  /* Set output file handle for options screen. */
  set_output_file_handle();

  /* Print options used. */
  Print_Options();
  
  /* BEGIN SECTION */
  /* VARIOUS INITIALIZATION THAT ONLY NEEDS TO BE DONE 1/EXECUTION */

  /* Set up noise-tables. */
  Initialize_Noise();
  
  Diff_Clock = opts.FrameSeq.FinalClock - opts.FrameSeq.InitialClock;

  if (opts.Options & CYCLIC_ANIMATION)
  {
    Diff_Frame = opts.FrameSeq.FinalFrame - opts.FrameSeq.InitialFrame + 1;
  }
  else
  {
    Diff_Frame = opts.FrameSeq.FinalFrame - opts.FrameSeq.InitialFrame;
  }

  Clock_Delta = ((Diff_Frame==0)?0:Diff_Clock/Diff_Frame);

  /* END SECTION */

  /* Execute the first shell-out command */
  Pre_Scene_Result=(POV_SHELLOUT_CAST)POV_SHELLOUT(PRE_SCENE_SHL);

  /* Loop over each frame */
  
  if (Pre_Scene_Result != ALL_SKIP_RET)
  {
     if (Pre_Scene_Result != SKIP_ONCE_RET)
     {
       for (opts.FrameSeq.FrameNumber = opts.FrameSeq.InitialFrame,
            opts.FrameSeq.Clock_Value = opts.FrameSeq.InitialClock;

            opts.FrameSeq.FrameNumber <= opts.FrameSeq.FinalFrame;

            opts.FrameSeq.FrameNumber++,
            opts.FrameSeq.Clock_Value += Clock_Delta)
       {
         setup_output_file_name();

         /* Execute a shell-out command before tracing */

         Frame_Result=(POV_SHELLOUT_CAST)POV_SHELLOUT(PRE_FRAME_SHL);
           
         if (Frame_Result == ALL_SKIP_RET)
         {
           break;
         }

         if (Frame_Result != SKIP_ONCE_RET)
         {
           FrameRender();

           /* Execute a shell-out command after tracing */

           Frame_Result = (POV_SHELLOUT_CAST)POV_SHELLOUT(POST_FRAME_SHL);
           
           if ((Frame_Result==SKIP_ONCE_RET) || (Frame_Result==ALL_SKIP_RET))
           {
             break;
           }
         }
       }

       /* Print total stats ... */
 
       if(opts.FrameSeq.FrameType==FT_MULTIPLE_FRAME)
       {
         Statistics("\nTotal Statistics");
    
         opts.FrameSeq.FrameNumber--;

         PRINT_STATS(totalstats);
    
         opts.FrameSeq.FrameNumber++;
       }
     }

     /* Execute the final shell-out command */

     POV_SHELLOUT(POST_SCENE_SHL);
  }

  /* And finish. */

  Terminate_POV(0);

  MAIN_RETURN_STATEMENT
} /* main */


/*****************************************************************************
*
* FUNCTION
*
*   FrameRender
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
*   Do all that is necessary for rendering a single frame, including parsing
*
* CHANGES
*
*   Feb 1996: Make sure we are displaying when doing a mosaic preview [AED]
*
******************************************************************************/

static void FrameRender()
{
  unsigned long hours, minutes;
  DBL seconds, t_total;

  /* Store start time for parse. */
  START_TIME

  /* Parse the scene file. */
  Status_Info("\n\nParsing...");
  
  opts.Do_Stats=FALSE;

  Init_Random_Generators();

  Parse();

  Destroy_Random_Generators();

  opts.Do_Stats=TRUE;

  if (opts.Options & RADIOSITY)
  {
     Experimental_Flag |= EF_RADIOS;
  }

  if (Experimental_Flag)
  {
    Warning(0.0,"Warning: This rendering uses the following experimental features:\n");
    if (Experimental_Flag & EF_RADIOS) 
    {
       Warning(0.0," radiosity");
    }
    Warning(0.0,".\nThe design and implementation of these features is likely to\n");
    Warning(0.0,"change in future versions of POV-Ray.  Full backward compatibility\n");
    Warning(0.0,"with the current implementation is NOT guaranteed.\n");
  }
  
  Experimental_Flag=0;

  /* Switch off standard anti-aliasing. */

  if ((Frame.Camera->Aperture != 0.0) && (Frame.Camera->Blur_Samples > 0))
  {
    opts.Options &= ~ANTIALIAS;

    Warning(0.0, "Focal blur is used. Standard antialiasing is switched off.\n");
  }

  /* Create the bounding box hierarchy. */

  Stage = STAGE_SLAB_BUILDING;

  if (opts.Use_Slabs)
  {
    Status_Info("\nCreating bounding slabs.");
  }

  /* Init module specific stuff. */
  Initialize_Atmosphere_Code();
  Initialize_BBox_Code();
  Initialize_Lighting_Code();
  Initialize_Mesh_Code();
  Initialize_VLBuffer_Code();
  Initialize_Radiosity_Code();

  /* Always call this to print number of objects. */
  Build_Bounding_Slabs(&Root_Object);

  /* Create the vista buffer. */
  Build_Vista_Buffer();

  /* Create the light buffers. */
  Build_Light_Buffers();

  /* Create blob queue. */
  Init_Blob_Queue();

  /* Save variable values. */
  variable_store(STORE);

  /* Open output file and if we are continuing an interrupted trace,
   * read in the previous file settings and any data there.  This has to
   * be done before any image-size related allocations, since the settings
   * in a resumed file take precedence over that specified by the user. [AED]
   */
  open_output_file();

  /* Start the display. */
  if (opts.Options & DISPLAY)
  {
    Status_Info ("\nDisplaying...");

    POV_DISPLAY_INIT(Frame.Screen_Width, Frame.Screen_Height);

    Display_Started = TRUE;

    /* Display vista tree. */
    Draw_Vista_Buffer();
  }

  /* Get things ready for ray tracing (misc init, mem alloc) */
  Initialize_Renderer();

  /* This had to be taken out of open_output_file() because we don't have
   * the final image size until the output file has been opened, so we can't
   * initialize the display until we know this, which in turn means we can't
   * read the rendered part before the display is initialized. [AED]
   */
  if ((opts.Options & DISKWRITE) && (opts.Options & CONTINUE_TRACE))
  {
    Read_Rendered_Part(Actual_Output_Name);

    if (opts.Last_Line > Frame.Screen_Height)
      opts.Last_Line = Frame.Screen_Height;

    if (opts.Last_Column > Frame.Screen_Width)
      opts.Last_Column = Frame.Screen_Width;

  }

  /* Get parsing time. */
  STOP_TIME
  tparse = TIME_ELAPSED

  /* Get total parsing time. */
  tparse_total += tparse;

  /* Store start time for trace. */
  START_TIME

  if(opts.FrameSeq.FrameType==FT_MULTIPLE_FRAME)
  {
    t_total=tparse_total+trender_total;
    SPLIT_TIME(t_total,&hours,&minutes,&seconds);
    Render_Info("\n %02ld:%02ld:%02.0f so far, ",hours,minutes,seconds);
    Render_Info("Rendering frame %d, going to %d.",
        opts.FrameSeq.FrameNumber, opts.FrameSeq.FinalFrame);
  }
  /* Start tracing. */
  Stage = STAGE_RENDERING;

  POV_PRE_RENDER

  Status_Info ("\nRendering...\r");

  /* Macro for setting up any special FP options */
  CONFIG_MATH

  /* Ok, go for it - trace the picture. */

  /* If radiosity preview has been done, we are continuing a trace, so it
   * is important NOT to do the preview, even if the user requests it, as it
   * will cause discontinuities in radiosity shading by (probably) calculating
   * a few more radiosity values.
   */
  if ( !opts.Radiosity_Preview_Done )
  {
    if ( opts.Options & RADIOSITY )
    {
      /* Note that radiosity REQUIRES a mosaic preview prior to main scan */

      Start_Tracing_Mosaic_Smooth(opts.PreviewGridSize_Start, opts.PreviewGridSize_End);
    }
    else
    {
      if (opts.Options & PREVIEW && opts.Options & DISPLAY)
      {
        Start_Tracing_Mosaic_Preview(opts.PreviewGridSize_Start, opts.PreviewGridSize_End);
      }
    }
  }

  switch (opts.Tracing_Method)
  {
    case 2 :

      Start_Adaptive_Tracing();

      break;

    case 1 :
    default:

      Start_Non_Adaptive_Tracing();
  }

  /* We're done. */

  /* Record time so well spent before file close so it can be in comments  */
  STOP_TIME
  trender = TIME_ELAPSED

  /* Close out our file */
  if (Output_File_Handle)
  {
     Close_File(Output_File_Handle);
  }

  Stage = STAGE_SHUTDOWN;

  POV_PRE_SHUTDOWN

  /* DESTROY lots of stuff */
  Deinitialize_Atmosphere_Code();
  Deinitialize_BBox_Code();
  Deinitialize_Lighting_Code();
  Deinitialize_Mesh_Code();
  Deinitialize_VLBuffer_Code();
  Deinitialize_Radiosity_Code();
  Destroy_Blob_Queue();
  Destroy_Light_Buffers();
  Destroy_Vista_Buffer();
  Destroy_Bounding_Slabs();
  Destroy_Frame();
  Terminate_Renderer();
  FreeFontInfo();
  Free_Iteration_Stack();

  POV_POST_SHUTDOWN

  /* Get total render time. */
  trender_total += trender;

  POV_DISPLAY_FINISHED

  if ((opts.Options & DISPLAY) && Display_Started)
  {
    POV_DISPLAY_CLOSE

    Display_Started = FALSE;
  }

  if (opts.histogram_on)
    write_histogram (opts.Histogram_File_Name) ;

  Status_Info("\nDone Tracing");

  /* Print stats ... */
  PRINT_STATS(stats);

  if(opts.FrameSeq.FrameType==FT_MULTIPLE_FRAME)
  {
    /* Add them up */
    sum_statistics(totalstats, stats);

    /* ... and then clear them for the next frame */
    init_statistics(stats);
  }

  /* Restore variable values. */
  variable_store(RESTORE);

}



/*****************************************************************************
*
* FUNCTION
*
*   fix_up_rendering_window
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
*   Fix wrong window and mosaic preview values.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void fix_up_rendering_window()
{
  int temp;
  
  if (opts.First_Column_Percent > 0.0)
    opts.First_Column = (int) (Frame.Screen_Width * opts.First_Column_Percent);

  if (opts.First_Line_Percent > 0.0)
    opts.First_Line = (int) (Frame.Screen_Height * opts.First_Line_Percent);

  /* The decrements are a fudge factor that used to be in OPTIN.C
   * but it messed up Write_INI_File so its moved here.
   */

  if (opts.First_Column <= 0)
    opts.First_Column = 0;
  else
    opts.First_Column--;

  if (opts.First_Line <= 0)
    opts.First_Line = 0;
  else
    opts.First_Line--;
  
  if ((opts.Last_Column == -1) && (opts.Last_Column_Percent <= 1.0))
    opts.Last_Column = (int) (Frame.Screen_Width * opts.Last_Column_Percent);

  if ((opts.Last_Line == -1) && (opts.Last_Line_Percent <= 1.0))
    opts.Last_Line = (int) (Frame.Screen_Height * opts.Last_Line_Percent);

  if (opts.Last_Line == -1)
    opts.Last_Line = Frame.Screen_Height;

  if (opts.Last_Column == -1)
    opts.Last_Column = Frame.Screen_Width;

  if (opts.Last_Column < 0 || opts.Last_Column > Frame.Screen_Width)
    opts.Last_Column = Frame.Screen_Width;

  if (opts.Last_Line > Frame.Screen_Height)
    opts.Last_Line = Frame.Screen_Height;

  /* Fix up Mosaic Preview values */
  opts.PreviewGridSize_Start=max(1,opts.PreviewGridSize_Start);
  opts.PreviewGridSize_End=max(1,opts.PreviewGridSize_End);

  if ((temp=closest_power_of_2((unsigned)opts.PreviewGridSize_Start))!=opts.PreviewGridSize_Start)
  {
     Warning(0.0,"Preview_Start_Size must be a power of 2. Changing to %d.\n",temp);
     opts.PreviewGridSize_Start=temp;
  }

  if ((temp=closest_power_of_2((unsigned)opts.PreviewGridSize_End))!=opts.PreviewGridSize_End)
  {
     Warning(0.0,"Preview_End_Size must be a power of 2. Changing to %d.\n",temp);
     opts.PreviewGridSize_End=temp;
  }

  /* End must be less than or equal to start */
  if (opts.PreviewGridSize_End > opts.PreviewGridSize_Start)
    opts.PreviewGridSize_End = opts.PreviewGridSize_Start;
    
  if (opts.PreviewGridSize_Start > 1)
  {
     opts.PreviewGridSize_End=max(opts.PreviewGridSize_End,2);
     opts.Options |= PREVIEW;
  }
  else
  {
     opts.Options &= ~PREVIEW;
  }

  /* Set histogram size here so it is available for Print_Options, and
   * make sure that it has an integer number of pixels/bucket. */
  if (opts.histogram_on)
  {
    if (opts.histogram_x == 0 || opts.histogram_x > Frame.Screen_Width)
      opts.histogram_x = Frame.Screen_Width;
    else if (opts.histogram_x < Frame.Screen_Width)
      opts.histogram_x = Frame.Screen_Width / ((Frame.Screen_Width +
                         opts.histogram_x - 1) / opts.histogram_x);

    if (opts.histogram_y == 0 || opts.histogram_y > Frame.Screen_Height)
      opts.histogram_y = Frame.Screen_Height;
    else if (opts.histogram_y < Frame.Screen_Height)
      opts.histogram_y = Frame.Screen_Height / ((Frame.Screen_Height +
                         opts.histogram_y - 1) /opts.histogram_y);
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   fix_up_animation_values
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
*   Validate animation parameters, compute subset values
*
* CHANGES
*
*   -
*
******************************************************************************/
static void fix_up_animation_values()
{
  float ClockDiff;
  int FrameDiff;
  int FrameIncr;
  float ClockPerFrameIncr;
  int NumFrames;

  if (opts.FrameSeq.FinalFrame != -1)
  {
    opts.FrameSeq.FrameType = FT_MULTIPLE_FRAME;

    if (opts.FrameSeq.Clock_Value != 0.0)
    {
       Warning(0.0,"Attempted to set single clock value in multi frame\nanimation. Clock value overridden.\n");
    }
  }
  else
  {
    if (opts.FrameSeq.Clock_Value != 0.0)
    {
       opts.FrameSeq.FrameType = FT_SINGLE_FRAME;
    }
  }

  if (opts.FrameSeq.FrameType == FT_SINGLE_FRAME)
  {
    /*
     * These are dummy values that will work for single_frame,
     * even in an animation loop.
     */

    opts.FrameSeq.InitialFrame = 0;
    opts.FrameSeq.FinalFrame   = 0;
    opts.FrameSeq.InitialClock = opts.FrameSeq.Clock_Value;
    opts.FrameSeq.FinalClock   = 0.0;
  }
  else
  {
    /* FrameType==FT_MULTIPLE_FRAME */

    if(opts.FrameSeq.InitialFrame == -1)
    {
      opts.FrameSeq.InitialFrame = 1;
    }

    if (opts.FrameSeq.FinalFrame < opts.FrameSeq.InitialFrame)
    {
      Error("Final frame %d is less than Start Frame %d.\n",
            opts.FrameSeq.FinalFrame, opts.FrameSeq.InitialFrame);
    }

    ClockDiff = opts.FrameSeq.FinalClock-opts.FrameSeq.InitialClock;

    if (opts.Options & CYCLIC_ANIMATION)
    {
      FrameDiff = opts.FrameSeq.FinalFrame-opts.FrameSeq.InitialFrame+1;
    }
    else
    {
      FrameDiff = opts.FrameSeq.FinalFrame-opts.FrameSeq.InitialFrame;
    }

    ClockPerFrameIncr = (FrameDiff == 0) ? 0 : (ClockDiff/FrameDiff);

    /* Calculate width, which is an integer log10 */

    NumFrames = opts.FrameSeq.FinalFrame;

    opts.FrameSeq.FrameNumWidth = 1;

    while (NumFrames >= 10)
    {
      opts.FrameSeq.FrameNumWidth++;

      NumFrames = NumFrames / 10;
    }

    if (opts.FrameSeq.FrameNumWidth > POV_NAME_MAX-1)
    {
      Error("Can't render %d frames requiring %d chars with %d width filename.\n",
          opts.FrameSeq.FinalFrame - opts.FrameSeq.InitialFrame + 1,
          opts.FrameSeq.FrameNumWidth, POV_NAME_MAX);
    }

    /* STARTING FRAME SUBSET */

    if (opts.FrameSeq.SubsetStartPercent != DBL_VALUE_UNSET)
    {
      FrameIncr = FrameDiff * opts.FrameSeq.SubsetStartPercent + 0.5; /* w/rounding */

      opts.FrameSeq.SubsetStartFrame = opts.FrameSeq.InitialFrame + FrameIncr;
    }

    if (opts.FrameSeq.SubsetStartFrame != INT_VALUE_UNSET)
    {
      NumFrames = opts.FrameSeq.SubsetStartFrame - opts.FrameSeq.InitialFrame;

      opts.FrameSeq.InitialFrame = opts.FrameSeq.SubsetStartFrame;
      opts.FrameSeq.InitialClock = opts.FrameSeq.InitialClock + NumFrames * ClockPerFrameIncr;
    }

    /* ENDING FRAME SUBSET */

    if (opts.FrameSeq.SubsetEndPercent != DBL_VALUE_UNSET)
    {
      /*
       * By this time, we have possibly lost InitialFrame, so we calculate
       * it via FinalFrame-FrameDiff
       */

      FrameIncr = FrameDiff * opts.FrameSeq.SubsetEndPercent + 0.5; /* w/rounding */

      opts.FrameSeq.SubsetEndFrame = (opts.FrameSeq.FinalFrame - FrameDiff) + FrameIncr;
    }

    if (opts.FrameSeq.SubsetEndFrame != INT_VALUE_UNSET)
    {
      NumFrames = opts.FrameSeq.FinalFrame - opts.FrameSeq.SubsetEndFrame;

      opts.FrameSeq.FinalFrame = opts.FrameSeq.SubsetEndFrame;
      opts.FrameSeq.FinalClock = opts.FrameSeq.FinalClock - NumFrames * ClockPerFrameIncr;
    }

    /*
     * Now that we have everything calculated, we check FinalFrame
     * and InitialFrame one more time, in case the subsets messed them up
     */

    if (opts.FrameSeq.FinalFrame < opts.FrameSeq.InitialFrame)
    {
      Error("Final frame %d is less than Start Frame %d\ndue to bad subset specification.\n",
            opts.FrameSeq.FinalFrame, opts.FrameSeq.InitialFrame);
    }
  }

  /* Needed for pre-render shellout fixup */

  opts.FrameSeq.FrameNumber = opts.FrameSeq.InitialFrame;
  opts.FrameSeq.Clock_Value = opts.FrameSeq.InitialClock;
}

/*****************************************************************************
*
* FUNCTION
*
*   fix_up_scene_name
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
*   Strip path and extention of input file to create scene name
*
* CHANGES
*
******************************************************************************/

static void fix_up_scene_name()
{
  int i, l;
  char temp[FILE_NAME_LENGTH];
  
  if ((l=strlen(opts.Input_File_Name)-1)<1)
  {
     strcpy(opts.Scene_Name,opts.Input_File_Name);
     return;
  }

  strcpy(temp,opts.Input_File_Name);
  for (i=l;i>0;i--)
  {
     if (temp[i]==FILENAME_SEPARATOR)
     {
        break;
     }
     if (temp[i]=='.')
     {
        temp[i]=0;
        break;
     }
  }

  i=strlen(temp)-1;
  
  while ((i>0) && (temp[i]!=FILENAME_SEPARATOR))
    i--;
  if (temp[i]==FILENAME_SEPARATOR)
    i++;
  strcpy(opts.Scene_Name,&(temp[i]));
}

/*****************************************************************************
*
* FUNCTION
*
*   set_output_file_handle
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
*   Set the output file handle according to the file type used.
*
* CHANGES
*
* Oct 95 - Removed test where the output file handle was only set if
*          output_to_file was TRUE. The output file handle structure
*          contains a pointer to read line, which is used by the continue
*          trace option. If you tried a continue trace with file output
*          manually turned OFF, then a GPF would occur due to a call to a
*          NULL function pointer.
*
******************************************************************************/

static void set_output_file_handle()
{
  char *def_ext = NULL;
  char temp[FILE_NAME_LENGTH];

    switch (opts.OutputFormat)
    {
      case '\0':
      case 's' :
      case 'S' : Output_File_Handle = GET_SYS_FILE_HANDLE(); def_ext=SYS_DEF_EXT; break;

      case 't' :
      case 'T' :
      case 'c' :
      case 'C' : Output_File_Handle = Get_Targa_File_Handle(); def_ext=".tga"; break;

      case 'p' :
      case 'P' : Output_File_Handle = Get_PPM_File_Handle(); def_ext=".ppm"; break;

      case 'n' :
      case 'N' : Output_File_Handle = Get_Png_File_Handle(); def_ext=".png"; break;
      
      case 'd' :
      case 'D' : Error ("Dump format no longer supported.\n"); break;
      case 'r' :
      case 'R' : Error ("Raw format no longer supported.\n"); break;

      default  : Error ("Unrecognized output file format %c.\n",
                        opts.OutputFormat);
    }

    Output_File_Handle->file_type = IMAGE_FTYPE;

    strcpy(temp,opts.Output_File_Name);
    
    POV_SPLIT_PATH(temp,opts.Output_Path,opts.Output_File_Name);

    if (opts.Output_File_Name[0] == '\0')
    {
      sprintf(opts.Output_File_Name, "%s%s",opts.Scene_Name,def_ext);
    }
    else if (!(opts.Options & TO_STDOUT))
    {
       if (!Has_Extension(opts.Output_File_Name))
       {
         strcat(opts.Output_File_Name, def_ext);
       }
    }
    
    strcpy(opts.Output_Numbered_Name,opts.Output_File_Name);
}

/*****************************************************************************
*
* FUNCTION
*
*   setup_output_file_name
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
*   Determine the file name for this frame.  For an animation, the frame
*   number is inserted into the file name.
*
* CHANGES
*
*   Jan-97  [esp]  Added conditional after getcwd, because Metrowerks getcwd
*                  function appends a path separator on output.
*
******************************************************************************/
static void setup_output_file_name()
{
  char number_string[10];
  char separator_string[2] = {FILENAME_SEPARATOR, 0} ;
  char *plast_period;
  int available_characters;
  int ilast_period;
  int fname_chars;

  /* This will create the real name for the file */
  if(opts.FrameSeq.FrameType!=FT_MULTIPLE_FRAME ||
     opts.Options & TO_STDOUT)
  {
    strcpy(opts.Output_Numbered_Name,opts.Output_File_Name);
  }
  else
  {
    /*
     * This is the maximum number of characters that can be used of the
     * original filename.  This will ensure that enough space is available
     * for the frame number in the filename
     */

    available_characters = POV_NAME_MAX-opts.FrameSeq.FrameNumWidth;

    plast_period = strrchr(opts.Output_File_Name, '.');

    if (plast_period == NULL)
    {
      Error("Illegal file name %s -- no extension.\n", opts.Output_File_Name);
    }

    ilast_period = plast_period - opts.Output_File_Name;

    fname_chars = ilast_period;

    if (fname_chars > available_characters)
    {
      /* Only give the warning once */

      if (opts.FrameSeq.FrameNumber == opts.FrameSeq.InitialFrame)
      {
        Warning(0.0, "Need to cut the output filename by %d characters.\n",
                ilast_period - available_characters);
      }

      fname_chars = available_characters;
    }

    /* Perform actual generation of filename */

    strncpy(opts.Output_Numbered_Name, opts.Output_File_Name, (unsigned)fname_chars);

    /* strncpy doesn't terminate if strlen(opts.Output_File_Name)<fname_chars */

    opts.Output_Numbered_Name[fname_chars]='\0';

    sprintf(number_string, "%0*d", opts.FrameSeq.FrameNumWidth, opts.FrameSeq.FrameNumber);

    strcat(opts.Output_Numbered_Name, number_string);

    strcat(opts.Output_Numbered_Name, &opts.Output_File_Name[ilast_period]);
  }

  if (strlen (opts.Output_Path) == 0)
  {
    getcwd (opts.Output_Path, sizeof (opts.Output_Path) - 1) ;
    /* on some systems (MacOS) getcwd adds the path separator on the end */
    /* so only add it if it isn't already there...  [esp]                */
    if (opts.Output_Path[strlen(opts.Output_Path)-1] != FILENAME_SEPARATOR)
        strcat (opts.Output_Path, separator_string) ;
  }
  strncpy (Actual_Output_Name,opts.Output_Path, sizeof (Actual_Output_Name));
  strncat (Actual_Output_Name,opts.Output_Numbered_Name, sizeof (Actual_Output_Name));
/*
Debug_Info("P='%s',O='%s',A='%s',N='%s'\n",opts.Output_Path,
  opts.Output_Numbered_Name, Actual_Output_Name,opts.Output_Numbered_Name);
*/
}


/*****************************************************************************
*
* FUNCTION
*
*   open_output_file
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
*   Open file and read in previous image if continued trace is on.
*
*   GOTCHA : This saves a POINTER to the file name, so the file
*            name must exist over the entire life/use of the file
*
* CHANGES
*
*   -
*
******************************************************************************/

static void open_output_file()
{
  int Buffer_Size;
  
  if (opts.Options & DISKWRITE)
  {
    Stage = STAGE_FILE_INIT;
    
    if (opts.Options & BUFFERED_OUTPUT)
    {
       Buffer_Size=opts.File_Buffer_Size;
    }
    else
    {
       Buffer_Size=0;
    }

    if (opts.Options & CONTINUE_TRACE)
    {
      Stage = STAGE_CONTINUING;

      if (Open_File(Output_File_Handle, Actual_Output_Name,
            &Frame.Screen_Width, &Frame.Screen_Height, Buffer_Size,
            READ_MODE) != 1)
      {
        Close_File(Output_File_Handle);

        Warning (0.0,"Error opening continue trace output file.\n");

        Warning (0.0,"Opening new output file %s.\n",Actual_Output_Name);

        /* Turn off continue trace */

        opts.Options &= ~CONTINUE_TRACE;

        if (Open_File(Output_File_Handle, Actual_Output_Name,
              &Frame.Screen_Width, &Frame.Screen_Height, Buffer_Size,
              WRITE_MODE) != 1)
        {
          Error ("Error opening output file.");
        }
      }
    }
    else
    {
      if (Open_File(Output_File_Handle, Actual_Output_Name,
            &Frame.Screen_Width, &Frame.Screen_Height, Buffer_Size,
            WRITE_MODE) != 1)
      {
        Error ("Error opening output file.");
      }
    }
  }
}


/*****************************************************************************
*
* FUNCTION
*
*   init_vars
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
*   Initialize all global variables.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void init_vars()
{
  Stage=STAGE_INIT;
  opts.Abort_Test_Counter = Abort_Test_Every ;
  Abort_Test_Every = 1;
  opts.AntialiasDepth = 3;
  opts.Antialias_Threshold = 0.3;
  opts.BBox_Threshold = 25;
  Color_Bits = 8;
  opts.DisplayFormat = '0';
  Display_Started = FALSE;
  opts.File_Buffer_Size = 0;
  opts.First_Column = 0;
  opts.First_Column_Percent = 0.0;
  opts.First_Line = 0;
  opts.First_Line_Percent = 0.0;
  Frame.Screen_Height = 100;
  Frame.Screen_Width  = 100;
  Root_Object = NULL;
  free_istack = NULL;
  opts.JitterScale = 1.0;
  opts.Language_Version = 3.1;
  opts.Last_Column = -1;
  opts.Last_Column_Percent = 1.0;
  opts.Last_Line = -1;
  opts.Last_Line_Percent = 1.0;
  opts.PreviewGridSize_Start = 1;
  opts.PreviewGridSize_End   = 1;
  opts.Library_Paths[0] = NULL;
  opts.Library_Path_Index = 0;
  Max_Intersections = 64; /*128*/
  Number_Of_Files = 0;
  Number_of_istacks = 0;

  opts.Options = USE_VISTA_BUFFER + USE_LIGHT_BUFFER + JITTER +
                 DISKWRITE + REMOVE_BOUNDS;
  opts.OutputFormat = DEFAULT_OUTPUT_FORMAT;
  opts.OutputQuality = 8;
  Output_File_Handle = NULL;
  opts.Output_Numbered_Name[0]='\0';
  opts.Output_File_Name[0]='\0';
  opts.Output_Path[0]='\0';
  opts.PaletteOption = '3';
  opts.Quality = 9;
  opts.Quality_Flags = QUALITY_9;
  opts.DisplayGamma = DEFAULT_DISPLAY_GAMMA;

  /* 
   * If DisplayGamma == 2.2, then GammaFactor == .45, which is what we want.
   */
  opts.GammaFactor = DEFAULT_ASSUMED_GAMMA/opts.DisplayGamma;

  opts.FrameSeq.FrameType = FT_SINGLE_FRAME;
  opts.FrameSeq.Clock_Value = 0.0;
  opts.FrameSeq.InitialFrame = 1;
  opts.FrameSeq.InitialClock = 0.0;
  opts.FrameSeq.FinalFrame = INT_VALUE_UNSET;
  opts.FrameSeq.FrameNumWidth = 0;
  opts.FrameSeq.FinalClock = 1.0;
  opts.FrameSeq.SubsetStartFrame = INT_VALUE_UNSET;
  opts.FrameSeq.SubsetStartPercent = DBL_VALUE_UNSET;
  opts.FrameSeq.SubsetEndFrame = INT_VALUE_UNSET;
  opts.FrameSeq.SubsetEndPercent = DBL_VALUE_UNSET;
  opts.FrameSeq.Field_Render_Flag = FALSE;
  opts.FrameSeq.Odd_Field_Flag = FALSE;

  opts.Radiosity_Brightness = 3.3;
  opts.Radiosity_Count = 100;
  opts.Radiosity_Dist_Max = 0.;   /* default calculated in Radiosity_Initialize */
  opts.Radiosity_Error_Bound = .4;
  opts.Radiosity_Gray = .5;       /* degree to which gathered light is grayed */
  opts.Radiosity_Low_Error_Factor = .8;
  opts.Radiosity_Min_Reuse = .015;
  opts.Radiosity_Nearest_Count = 6;
  opts.Radiosity_Recursion_Limit = 1;
  opts.Radiosity_Quality = 6;     /* Q-flag value for light gathering */
  opts.Radiosity_File_ReadOnContinue = 1;
  opts.Radiosity_File_SaveWhileRendering = 1;
  opts.Radiosity_File_AlwaysReadAtStart = 0;
  opts.Radiosity_File_KeepOnAbort = 1;
  opts.Radiosity_File_KeepAlways = 0;


  init_statistics(stats);
  init_statistics(totalstats);

  strcpy (opts.Input_File_Name, "OBJECT.POV");
  opts.Scene_Name[0]='\0';
  opts.Ini_Output_File_Name[0]='\0';
  opts.Use_Slabs=TRUE;
  Num_Echo_Lines = 5;   /* May make user setable later - CEY*/
  Echo_Line_Length = 180;   /* May make user setable later - CEY*/

  closed_flag = FALSE;
  Stop_Flag = FALSE;

  trender = trender_total = 0.0;
  tparse  = tparse_total  = 0.0;

  histogram_grid = NULL ;
  opts.histogram_on = FALSE ;
  opts.histogram_type = NONE ;
  opts.Histogram_File_Name[0] = '\0';
  Histogram_File_Handle = NULL ;
  /*
   * Note that late initialization of the histogram_x and histogram_y
   * variables is done in fix_up_rendering_window, if they aren't specified
   * on the command line.  This is because they are based on the image
   * dimensions, and we can't be certain that we have this info at the
   * time we parse the histogram options in optin.c. [AED]
   */
  opts.histogram_x = opts.histogram_y = 0 ;
  max_histogram_value = 0 ;

  opts.Tracing_Method = 1;
  Experimental_Flag = 0;
  Make_Pigment_Entries();
}


/*****************************************************************************
*
* FUNCTION
*
*   init_statistics
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
*   Initialize statistics to 0
*
* CHANGES
*
*   -
*
******************************************************************************/

static void init_statistics(COUNTER *pstats)
{
  int i;

  for(i=0; i<MaxStat; i++)
    Init_Counter(pstats[i]);
}

/*****************************************************************************
*
* FUNCTION
*
*   sum_statistics
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
*   Add current statistics to total statistics
*
* CHANGES
*
*   -
*
******************************************************************************/

static void sum_statistics(COUNTER *ptotalstats, COUNTER *pstats)
{
  int i;
  COUNTER tmp;

  for(i=0; i<MaxStat; i++)
  {
    Add_Counter(tmp,pstats[i],ptotalstats[i]);
    ptotalstats[i]=tmp;
  }
}


/*****************************************************************************
*
* FUNCTION
*
*   variable_store
*
* INPUT
*
*   flag - flag telling wether to store or restore variables.
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
*   Store or restore variables whose value has to be the same for all
*   frames of an animation and who are changed during every frame.
*
* CHANGES
*
*   May 1995 : Creation.
*
******************************************************************************/

static void variable_store(int Flag)
{
  static int STORE_First_Line;

  switch (Flag)
  {
    case STORE:

      STORE_First_Line = opts.First_Line;

      break;

    case RESTORE:

      opts.First_Line = STORE_First_Line;

      break;

    default:

      Error("Unknown flag in variable_store().\n");
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   destroy_libraries
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
*   Free library path memory.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void destroy_libraries()
{
  int i;

  for (i = 0; i < opts.Library_Path_Index; i++)
  {
    POV_FREE(opts.Library_Paths[i]);
    
    opts.Library_Paths[i] = NULL;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   close_all
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
*   Close all the stuff that has been opened and free all allocated memory.
*
* CHANGES
*
*   -
*
******************************************************************************/

void close_all()
{
  /* Only close things once */

  if (closed_flag)
  {
    return;
  }

  if (Output_File_Handle != NULL)
  {
    Close_File(Output_File_Handle);
    
    POV_FREE(Output_File_Handle);
    
    Output_File_Handle = NULL;
  }

  destroy_shellouts();
  destroy_libraries();
  Destroy_Text_Streams();
  Free_Noise_Tables();
  Terminate_Renderer();
  Destroy_Bounding_Slabs();
  Destroy_Blob_Queue();
  Destroy_Vista_Buffer();
  Destroy_Light_Buffers();
  Destroy_Random_Generators();
  Deinitialize_Radiosity_Code();
  Free_Iteration_Stack();
  destroy_histogram();
  Deinitialize_Atmosphere_Code();
  Deinitialize_BBox_Code();
  Deinitialize_Lighting_Code();
  Deinitialize_Mesh_Code();
  Deinitialize_VLBuffer_Code();
  Destroy_Frame();
  Destroy_IStacks();
  FreeFontInfo();

  if ((opts.Options & DISPLAY) && Display_Started)
  {
    POV_DISPLAY_CLOSE
  }

  closed_flag = TRUE;
}



/*****************************************************************************
*
* FUNCTION
*
*   POV_Std_Split_Time
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
*   Split time into hours, minutes and seconds.
*
* CHANGES
*
*   -
*
******************************************************************************/

void POV_Std_Split_Time(DBL time_dif, unsigned long *hrs, unsigned long *mins, DBL *secs)
{
  *hrs = (unsigned long)(time_dif / 3600.0);

  *mins = (unsigned long)((time_dif - (DBL)(*hrs * 3600)) / 60.0);

  *secs = time_dif - (DBL)(*hrs * 3600 + *mins * 60);
}




/*****************************************************************************
*
* FUNCTION
*
*   pov_stricmp
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
*   Since the stricmp function isn't available on all systems, we've
*   provided a simplified version of it here.
*
* CHANGES
*
*   -
*
******************************************************************************/

int pov_stricmp (char *s1, char  *s2)
{
  char c1, c2;

  while ((*s1 != '\0') && (*s2 != '\0'))
  {
    c1 = *s1++;
    c2 = *s2++;

    c1 = (char)toupper(c1);
    c2 = (char)toupper(c2);

    if (c1 < c2)
    {
      return(-1);
    }

    if (c1 > c2)
    {
      return(1);
    }
  }

  if (*s1 == '\0')
  {
    if (*s2 == '\0')
    {
      return(0);
    }
    else
    {
      return(-1);
    }
  }
  else
  {
    return(1);
  }
}


/*****************************************************************************
*
* FUNCTION
*
*   Locate_File
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
*   Find a file in the search path.
*
* CHANGES
*
*   Apr 1996: Don't add trailing FILENAME_SEPARATOR if we are immediately
*             following DRIVE_SEPARATOR because of Amiga probs.  [AED]
*
******************************************************************************/

FILE *Locate_File (char *filename, char  *mode, char  *ext1, char  *ext2, char  *buffer, int err_flag)
{
  int i,l1,l2;
  char pathname[FILE_NAME_LENGTH];
  char file0[FILE_NAME_LENGTH];
  char file1[FILE_NAME_LENGTH];
  char file2[FILE_NAME_LENGTH];
  FILE *f;

  if (Has_Extension(filename))
  {
     l1=l2=0;
  }
  else
  {
     if ((l1 = strlen(ext1)) > 0)
     {
        strcpy(file1, filename);
        strcat(file1, ext1);
     }

     if ((l2 = strlen(ext2)) > 0)
     {
        strcpy(file2, filename);
        strcat(file2, ext2);
     }
  }

  /* Check the current directory first. */
  if (l1)
  {
     if ((f = fopen(file1, mode)) != NULL)
     {
       POV_GET_FULL_PATH(f,file1,buffer);
       return(f);
     }
  }
  if (l2)
  {
     if ((f = fopen(file2, mode)) != NULL)
     {
       POV_GET_FULL_PATH(f,file2,buffer);
       return(f);
     }
  }
  if ((f = fopen(filename, mode)) != NULL)
  {
     POV_GET_FULL_PATH(f,filename,buffer);
     return(f);
  }

  for (i = 0; i < opts.Library_Path_Index; i++)
  {
    strcpy(file0, opts.Library_Paths[i]);
    file0[strlen(file0)+1] = '\0';
    if (file0[strlen(file0) - 1] != DRIVE_SEPARATOR)
      file0[strlen(file0)] = FILENAME_SEPARATOR;

    if (l1)
    {
       strcpy(pathname, file0);
       strcat(pathname, file1);
       if ((f = fopen(pathname, mode)) != NULL)
       {
          POV_GET_FULL_PATH(f,pathname,buffer);
          return(f);
       }
    }

    if (l2)
    {
       strcpy(pathname, file0);
       strcat(pathname, file2);
       if ((f = fopen(pathname, mode)) != NULL)
       {
          POV_GET_FULL_PATH(f,pathname,buffer);
          return(f);
       }
    }
    strcpy(pathname, file0);
    strcat(pathname, filename);
    if ((f = fopen(pathname, mode)) != NULL)
    {
      POV_GET_FULL_PATH(f,pathname,buffer);
      return(f);
    }
  }
 
  if (err_flag)
  {
    if (l1)
    {
      Error_Line("Could not find file '%s%s'\n",filename,ext1);
    }
    else
    {
      Error_Line("Could not find file '%s'\n",filename);
    }
  }
  
  return(NULL);
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

static int Has_Extension (char *name)
{
   char *p;

   if (name!=NULL)
   {
     p=strrchr(name, '.');

     if (p!=NULL)
     {
        if ((strlen(name)-(p-name))<=4)
        {
           return (TRUE);
        }
     }
   }
   return (FALSE);
}


/*****************************************************************************
*
* FUNCTION
*
*   pov_shellout
*
* INPUT
*
*   template_command - the template command string to execute
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
*   Execute the command line described by the string being passed in
*
* CHANGES
*
*   -
*
******************************************************************************/

SHELLRET pov_shellout (SHELLTYPE Type)
{
  char real_command[POV_MAX_CMD_LENGTH];
  int i, j, l = 0;
  int length;
  SHELLRET Return_Code;
  char *s = NULL;
  char *template_command;


  if ( opts.Shellouts == NULL ) return(IGNORE_RET);

  template_command=opts.Shellouts[Type].Command;

  if ((length = strlen(template_command)) == 0)
  {
    return(IGNORE_RET);
  }

  switch(Type)
  {
    case PRE_SCENE_SHL:  s="pre-scene";   break;
    case PRE_FRAME_SHL:  s="pre-frame";   break;
    case POST_FRAME_SHL: s="post-frame";  break;
    case POST_SCENE_SHL: s="post-scene";  break;
    case USER_ABORT_SHL: s="user about";  break;
    case FATAL_SHL:      s="fatal error"; break;
    case MAX_SHL: /* To remove warnings*/ break;
  }

  Status_Info("\nPerforming %s shell-out command",s);

  /* First, find the real command */

  for (i = 0, j = 0; i < length; )
  {
    if (template_command[i] == '%')
    {
      switch (toupper(template_command[i+1]))
      {
         case 'O':

          strncpy(&real_command[j], opts.Output_Numbered_Name, 
               (unsigned)(l=strlen(opts.Output_Numbered_Name)));

          break;

         case 'P':

          strncpy(&real_command[j], opts.Output_Path,(unsigned)(l=strlen(opts.Output_Path)));

          break;

         case 'S':

          strncpy(&real_command[j], opts.Scene_Name, (unsigned)(l=strlen(opts.Scene_Name)));

          break;

         case 'N':

          sprintf(&real_command[j],"%d",opts.FrameSeq.FrameNumber);
          l = strlen(&real_command[j]);

          break;

         case 'K':

          sprintf(&real_command[j],"%f",opts.FrameSeq.Clock_Value);
          l = strlen(&real_command[j]);

          break;

         case 'H':

          sprintf(&real_command[j],"%d",Frame.Screen_Height);
          l = strlen(&real_command[j]);

          break;

         case 'W':

          sprintf(&real_command[j],"%d",Frame.Screen_Width);
          l = strlen(&real_command[j]);

          break;

         case '%':

          real_command[j]='%';

          l=1;

          break;
       }

       j+=l;

       i+=2; /* we used 2 characters of template_command */
    }
    else
    {
      real_command[j++]=template_command[i++];
    }
  }

  real_command[j]='\0';

  Return_Code=(POV_SHELLOUT_CAST)POV_SYSTEM(real_command);

  if (opts.Shellouts[Type].Inverse)
  {
    Return_Code=(POV_SHELLOUT_CAST)(!((int)Return_Code));
  }

  if (Return_Code)
  {
    if (Type < USER_ABORT_SHL)
    {
      switch(opts.Shellouts[Type].Ret)
      {
        case FATAL_RET:

          Error("Fatal error returned from shellout command.");

          break;

        case USER_RET:

          Check_User_Abort(TRUE); /* the TRUE forces user abort */

          break;

        case QUIT_RET:

          Terminate_POV(0);

          break;

        case IGNORE_RET:
        case SKIP_ONCE_RET:
        case ALL_SKIP_RET: /* Added to remove warnings */
          break;
      }
    }

    return(opts.Shellouts[Type].Ret);
  }

  return(IGNORE_RET);
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

static void init_shellouts()
{
  int i;

  opts.Shellouts=(SHELLDATA *)POV_MALLOC(sizeof(SHELLDATA)*MAX_SHL,"shellout data");

  for (i=0; i < MAX_SHL; i++)
  {
    opts.Shellouts[i].Ret=IGNORE_RET;
    opts.Shellouts[i].Inverse=FALSE;
    opts.Shellouts[i].Command[0]='\0';
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

static void destroy_shellouts()
{
  if (opts.Shellouts != NULL)
  {
    POV_FREE(opts.Shellouts);
  }

  opts.Shellouts=NULL;
}


/*****************************************************************************
*
* FUNCTION
*
*   closest_power_of_2
*
* INPUT
*
*   theNumber - the value to determine closest power of 2 for.
*
* OUTPUT
*
* RETURNS
*
*   The closest power of two is returned, or zero if the
*   argument is less than or equal to zero.
*
* AUTHOR
*
*   Eduard Schwan
*
* DESCRIPTION
*
*   Decription: Find the highest positive power of 2 that is
*   less than or equal to the number passed.
*
*   Input  Output
*   -----  ------
*     0      0
*     1      1
*     2      2
*     3      2
*     8      8
*     9      8
*
* CHANGES
*
*   Aug 1994 : Created by Eduard.
*
******************************************************************************/

static unsigned closest_power_of_2(unsigned theNumber)
{
  int PowerOf2Counter;

  /* do not handle zero or negative numbers for now */

  if (theNumber <= 0)
  {
    return(0);
  }

  /* count the number in question down as we count up a power of 2 */

  PowerOf2Counter = 1;

  while (theNumber > 1)
  {
    /* move our power of 2 counter bit up... */

    PowerOf2Counter <<= 1;

    /* and reduce our test number by a factor of 2 two */

    theNumber >>= 1;
  }

  return(PowerOf2Counter);
}

/*****************************************************************************
*
* FUNCTION
*
*   pre_init_povray
*
* INPUT -- none
*
* OUTPUT
*
* RETURNS
*
* AUTHOR -- CEY
*
* DESCRIPTION
*
*   This routine does essential initialization that is required before any
*   POV_MALLOC-like routines may be called and before any text streams
*   may be used.
*   
*   If you are using alt_main and need access to any part of the generic code
*   before alt_main is called, you MUST call this routine first!  Also note
*   that it is safe to call it twice.  If you don't call it, alt_main will.
*   It won't hurt if you both do it.
*   
*   NOTE: Terminate_POV de-initializes these features.  Therefore you may
*   need to call it again between sucessive calls to alt_main.  If you call
*   pre_init_povray but for some reason you abort and don't call alt_main,
*   then you should call Terminate_POV to clean up.
*
* CHANGES
*   Nov 1995 : Created by CEY
*
******************************************************************************/

void pre_init_povray()
{
  if (pre_init_flag==1234)
  {
    return;
  }
      
  /* Initialize memory. */
  POV_MEM_INIT();

  /* Initialize streams. In USERIO.C */
  Init_Text_Streams();

  init_shellouts();

  pre_init_tokenizer ();
  
  pre_init_flag=1234;
}

void POV_Split_Path(char *s,char *p,char *f)
{
char *l;

  strcpy(p,s);

  if ((l=strrchr(p,FILENAME_SEPARATOR))==NULL)
  {
     if ((l=strrchr(p,DRIVE_SEPARATOR))==NULL)
     {
        strcpy(f,s);
        p[0]='\0';
        return;
     }
  }
  
  l++;
  strcpy(f,l);
  *l='\0';

}
