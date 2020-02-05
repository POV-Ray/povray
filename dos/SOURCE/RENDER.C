/****************************************************************************
*                   render.c
*
*  This module implements the main raytracing loop.
*
* 08/07/92 lsk    Changed the normal antialiasing function to use a loop 
*                 where the number of rays per pixel when antialiasing can 
*                 be specified.
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
******************************************************************************/

#include <time.h>
#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "bbox.h"
#include "chi2.h"
#include "colour.h"
#include "interior.h"
#include "lighting.h"
#include "normal.h"
#include "objects.h"
#include "octree.h"
#include "optout.h"
#include "povray.h"
#include "radiosit.h"
#include "ray.h"
#include "render.h"
#include "targa.h"
#include "texture.h"
#include "vbuffer.h"
#include "userio.h"


/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define rand2d(a, b) jitttab[(int)(hashTable[(int)(hashTable[(int)((a)&0xfff)]^(b))&0xfff])&0xff]

/* Grid stuff used by focal blur code. */

#define grid1size  4
#define hexgrid2size  7
#define hexgrid3size  19
#define hexgrid4size  37

/* Grid size (n x n) used while jittering focal blur sub-pixel position. */

#define SUB_PIXEL_GRID_SIZE 16

/*****************************************************************************
* Local typedefs
******************************************************************************/

typedef struct Pixel_Struct PIXEL;
typedef struct Vec2_Struct VEC2;

struct Vec2_Struct
{
  DBL x, y;
};

struct Pixel_Struct
{
  int active;
  COLOUR Colour;
};



/*****************************************************************************
* Local variables
******************************************************************************/

int Trace_Level;
int Max_Trace_Level = 5;
int Highest_Trace_Level; /* DMF */

/* ADC stuff by DMF. */

DBL ADC_Bailout = 1.0/255.0;

static COLOUR *Previous_Line, *Current_Line;
static char *Previous_Line_Antialiased_Flags, *Current_Line_Antialiased_Flags;

static RAY Camera_Ray;

static int SuperSampleCount, RadiosityCount;

static DBL maxclr;

/* Jitter values are taken from [-0.5*JitterScale, 0.5*JitterScale]. */

static DBL JitterScale;

/* Jitter ranges unsed during supersampling. */

static unsigned short JRanges[] = {1,1,1,1,3,2,5,3,7,4};

/* variables used by accumulate_histogram() for statistics on the render */

unsigned long           *histogram_grid;
unsigned long           max_histogram_value;
FILE_HANDLE             *Histogram_File_Handle;

/*
 * Focal blur stuff.
 */

/* Flag telling if focal blur is used. */

static int Focal_Blur_Is_Used;

/* Direction to focal plane. */

static DBL Focal_Distance;

/* Array of threshold for confidence test. */

static DBL *Sample_Threshold;

/* Array giving number of samples to take before next confidence test. */

static int *Current_Number_Of_Samples;

/* Array of sample locations. */

static VEC2 *Sample_Grid;

/* Maximum amount of jitter to use. */

static DBL Max_Jitter;

/* Vectors in the viewing plane. */

static VECTOR XPerp, YPerp;

/* 2*2 grid */

static VEC2 grid1[grid1size] =
{
  {-0.25,  0.25},
  { 0.25,  0.25},
  {-0.25, -0.25},
  { 0.25, -0.25}
};

static DBL hexjitter2 = 0.144338;
static int hexgrid2samples[2] = { 7, 0 };
static VEC2 hexgrid2 [hexgrid2size] =
{
  {-0.288675,  0.000000},
  { 0.000000,  0.000000},
  { 0.288675,  0.000000},
  {-0.144338,  0.250000},
  {-0.144338, -0.250000},
  { 0.144338,  0.250000},
  { 0.144338, -0.250000}
};

static DBL hexjitter3 = 0.096225;
static int hexgrid3samples[4] = { 7, 6, 6, 0 };
static VEC2 hexgrid3 [hexgrid3size] =
{
  {-0.192450,  0.333333},
  {-0.192450, -0.333333},
  { 0.192450,  0.333333},
  { 0.192450, -0.333333},
  { 0.384900,  0.000000},
  {-0.384900,  0.000000},
  { 0.000000,  0.000000},

  { 0.000000,  0.333333},
  { 0.000000, -0.333333},
  {-0.288675,  0.166667},
  {-0.288675, -0.166667},
  { 0.288675,  0.166667},
  { 0.288675, -0.166667},

  {-0.096225,  0.166667},
  {-0.096225, -0.166667},
  { 0.096225,  0.166667},
  { 0.096225, -0.166667},
  {-0.192450,  0.000000},
  { 0.192450,  0.000000}
};

static DBL hexjitter4 = 0.0721688;
static int hexgrid4samples[9] = { 7, 6, 6, 4, 4, 4, 4, 2, 0 };
static VEC2 hexgrid4 [hexgrid4size] =
{
  { 0.000000,  0.000000},
  {-0.216506,  0.375000},
  { 0.216506, -0.375000},
  {-0.216506, -0.375000},
  { 0.216506,  0.375000},
  {-0.433013,  0.000000},
  { 0.433013,  0.000000},

  {-0.144338,  0.250000},
  { 0.144338, -0.250000},
  {-0.144338, -0.250000},
  { 0.144338,  0.250000},
  {-0.288675,  0.000000},
  { 0.288675,  0.000000},

  {-0.072169,  0.125000},
  { 0.072169, -0.125000},
  {-0.072169, -0.125000},
  { 0.072169,  0.125000},
  {-0.144338,  0.000000},
  { 0.144338,  0.000000},

  {-0.360844,  0.125000},
  {-0.360844, -0.125000},
  { 0.360844,  0.125000},
  { 0.360844, -0.125000},

  {-0.288675,  0.250000},
  {-0.288675, -0.250000},
  { 0.288675,  0.250000},
  { 0.288675, -0.250000},

  {-0.072169,  0.375000},
  {-0.072169, -0.375000},
  { 0.072169,  0.375000},
  { 0.072169, -0.375000},

  {-0.216506,  0.125000},
  {-0.216506, -0.125000},
  { 0.216506,  0.125000},
  { 0.216506, -0.125000},

  { 0.000000,  0.250000},
  { 0.000000, -0.250000},
};

static float jitttab[256] = {
  -0.500000,0.005890,0.011749,-0.490234,0.023468,-0.470703,-0.480469,0.017609,
  0.046906,-0.447266,-0.441406,0.056671,-0.460938,0.044952,0.035187,-0.466797,
  0.093781,-0.400391,-0.394531,0.103546,-0.382813,0.123077,0.113312,-0.388672,
  -0.421875,0.084015,0.089874,-0.412109,0.070343,-0.423828,-0.433594,0.064484,
  0.187531,-0.306641,-0.300781,0.197296,-0.289063,0.216827,0.207062,-0.294922,
  -0.265625,0.240265,0.246124,-0.255859,0.226593,-0.267578,-0.277344,0.220734,
  -0.343750,0.162140,0.167999,-0.333984,0.179718,-0.314453,-0.324219,0.173859,
  0.140656,-0.353516,-0.347656,0.150421,-0.367188,0.138702,0.128937,-0.373047,
  0.375031,-0.119141,-0.113281,0.384796,-0.101563,0.404327,0.394562,-0.107422,
  -0.078125,0.427765,0.433624,-0.068359,0.414093,-0.080078,-0.089844,0.408234,
  -0.031250,0.474640,0.480499,-0.021484,0.492218,-0.001953,-0.011719,0.486359,
  0.453156,-0.041016,-0.035156,0.462921,-0.054688,0.451202,0.441437,-0.060547,
  -0.187500,0.318390,0.324249,-0.177734,0.335968,-0.158203,-0.167969,0.330109,
  0.359406,-0.134766,-0.128906,0.369171,-0.148438,0.357452,0.347687,-0.154297,
  0.281281,-0.212891,-0.207031,0.291046,-0.195313,0.310577,0.300812,-0.201172,
  -0.234375,0.271515,0.277374,-0.224609,0.257843,-0.236328,-0.246094,0.251984,
  -0.249969,0.255859,0.261719,-0.240204,0.273438,-0.220673,-0.230438,0.267578,
  0.296875,-0.197235,-0.191376,0.306641,-0.210907,0.294922,0.285156,-0.216766,
  0.343750,-0.150360,-0.144501,0.353516,-0.132782,0.373047,0.363281,-0.138641,
  -0.171844,0.333984,0.339844,-0.162079,0.320313,-0.173798,-0.183563,0.314453,
  0.437500,-0.056610,-0.050751,0.447266,-0.039032,0.466797,0.457031,-0.044891,
  -0.015594,0.490234,0.496094,-0.005829,0.476563,-0.017548,-0.027313,0.470703,
  -0.093719,0.412109,0.417969,-0.083954,0.429688,-0.064423,-0.074188,0.423828,
  0.390625,-0.103485,-0.097626,0.400391,-0.117157,0.388672,0.378906,-0.123016,
  0.125000,-0.369110,-0.363251,0.134766,-0.351532,0.154297,0.144531,-0.357391,
  -0.328094,0.177734,0.183594,-0.318329,0.164063,-0.330048,-0.339813,0.158203,
  -0.281219,0.224609,0.230469,-0.271454,0.242188,-0.251923,-0.261688,0.236328,
  0.203125,-0.290985,-0.285126,0.212891,-0.304657,0.201172,0.191406,-0.310516,
  -0.437469,0.068359,0.074219,-0.427704,0.085938,-0.408173,-0.417938,0.080078,
  0.109375,-0.384735,-0.378876,0.119141,-0.398407,0.107422,0.097656,-0.404266,
  0.031250,-0.462860,-0.457001,0.041016,-0.445282,0.060547,0.050781,-0.451141,
  -0.484344,0.021484,0.027344,-0.474579,0.007813,-0.486298,-0.496063,0.001953,
};

/* Precomputed ray container values. */

static int Primary_Ray_State_Tested;
static int Containing_Index;
static INTERIOR *Containing_Interiors[MAX_CONTAINING_OBJECTS];

/* Flag wether to compute camera constant that don't change during one frame. */

static int Precompute_Camera_Constants;
static DBL Camera_Aspect_Ratio;





/*****************************************************************************
* Local functions
******************************************************************************/

static void focal_blur (RAY *Ray, COLOUR Colour, DBL x, DBL y);
static void jitter_camera_ray (RAY *ray, int ray_number);
static void check_stats (int y, int doingMosaic, int pixWidth);
static void do_anti_aliasing (int x, int y, COLOUR Colour);
static void output_line (int y);
static int  create_ray (RAY *ray, DBL x, DBL y, int ray_number);
static void supersample (COLOUR result, int x, int y);
static void gamma_correct (COLOUR Colour);
static void extract_colors (COLOUR Colour, unsigned char *Red, unsigned char *Green, unsigned char *Blue, unsigned char *Alpha, DBL *grey);
static void trace_pixel (int x, int y, COLOUR Colour);
static void initialise_histogram (void) ;
static void accumulate_histogram (int x, int y, int on);
static void plot_pixel (int x, int y, COLOUR Colour);
static void jitter_pixel_position (int x, int y, DBL *Jitter_X, DBL *Jitter_Y);
static void trace_sub_pixel (int level, PIXEL **Block, int x, int y, int x1, int y1, int x2, int y2, int size, COLOUR Colour, int antialias);
static void trace_ray_with_offset (int x, int y, DBL dx, DBL dy, COLOUR Colour);
static void initialize_ray_container_state (RAY *Ray, int Compute);
static void initialize_ray_container_state_tree (RAY *Ray, BBOX_TREE *Node);



/*****************************************************************************
*
* FUNCTION
*
*   Initialize_Renderer
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
*   Setup renderer. Allocate memory for line storage.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Initialize_Renderer (void)
{
  char **Grid;
  int i, xi, yi, Grid_Size;
  int Standard_Sample_Grid_Size;
  size_t size;
  DBL x, y, len;
  DBL T1;
  VEC2 *Standard_Sample_Grid;

  maxclr = (DBL)(1 << Color_Bits) - 1.0;
  Radiosity_Trace_Level = 1;

  size = (Frame.Screen_Width + 1) * sizeof(COLOUR);

  Previous_Line = (COLOUR *)POV_MALLOC(size, "previous line buffer");
  Current_Line  = (COLOUR *)POV_MALLOC(size, "current line buffer");

  for (i = 0; i <= Frame.Screen_Width ; i++)
  {
    Make_ColourA(Previous_Line[i], 0.0, 0.0, 0.0, 0.0, 0.0);
    Make_ColourA(Current_Line[i],  0.0, 0.0, 0.0, 0.0, 0.0);
  }

  if (opts.Options & ANTIALIAS)
  {
    size = (Frame.Screen_Width + 1) * sizeof(char);

    Previous_Line_Antialiased_Flags = (char *)POV_MALLOC(size, "previous line flags");
    Current_Line_Antialiased_Flags  = (char *)POV_MALLOC(size, "current line flags");

    for (i = 0; i <= Frame.Screen_Width ; i++)
    {
      Previous_Line_Antialiased_Flags[i] = 0;
      Current_Line_Antialiased_Flags[i]  = 0;
    }
  }

  Assign_Vector(Camera_Ray.Initial, Frame.Camera->Location);

  if (opts.histogram_on)
  {
    initialise_histogram();
  }

  Focal_Blur_Is_Used = (Frame.Camera->Aperture != 0.0) && (Frame.Camera->Blur_Samples > 0);

  /* Init focal blur stuff. */

  Sample_Grid = NULL;

  Sample_Threshold = NULL;

  if (Focal_Blur_Is_Used)
  {
    /* Create list of thresholds for confidence test. */

    Sample_Threshold = (DBL *)POV_MALLOC(Frame.Camera->Blur_Samples*sizeof(DBL), "sample threshold list");

    if (Frame.Camera->Blur_Samples > 1)
    {
      T1 = Frame.Camera->Variance / chdtri((DBL)(Frame.Camera->Blur_Samples-1), Frame.Camera->Confidence);

      for (i = 0; i < Frame.Camera->Blur_Samples; i++)
      {
        Sample_Threshold[i] = T1 * chdtri((DBL)(i+1), Frame.Camera->Confidence);
      }
    }
    else
    {
      Sample_Threshold[0] = 0.0;
    }

    /* Create list of sample positions. */

    Sample_Grid = (VEC2 *)POV_MALLOC(Frame.Camera->Blur_Samples*sizeof(VEC2), "sample grid");

    /*
     * Choose sample list and the best standard grid to use.
     */

    /* Default is 4x4 standard grid. */

    Standard_Sample_Grid = &grid1[0];

    Standard_Sample_Grid_Size = 4;

    Current_Number_Of_Samples = NULL;

    /* Check for 7 samples hexgrid. */

    if (Frame.Camera->Blur_Samples >= hexgrid2size)
    {
      Standard_Sample_Grid = &hexgrid2[0];

      Standard_Sample_Grid_Size = hexgrid2size;

      Current_Number_Of_Samples = &hexgrid2samples[0];
    }

    /* Check for 19 samples hexgrid. */

    if (Frame.Camera->Blur_Samples >= hexgrid3size)
    {
      Standard_Sample_Grid = &hexgrid3[0];

      Standard_Sample_Grid_Size = hexgrid3size;

      Current_Number_Of_Samples = &hexgrid3samples[0];
    }

    /* Check for 37 samples hexgrid. */

    if (Frame.Camera->Blur_Samples >= hexgrid4size)
    {
      Standard_Sample_Grid = &hexgrid4[0];

      Standard_Sample_Grid_Size = hexgrid4size;

      Current_Number_Of_Samples = &hexgrid4samples[0];
    }

    /* Get max. jitter. */

    switch (Frame.Camera->Blur_Samples)
    {
      case hexgrid2size :

        Max_Jitter = hexjitter2;

        break;

      case hexgrid3size :

        Max_Jitter = hexjitter3;

        break;

      case hexgrid4size :

        Max_Jitter = hexjitter4;

        break;

      default:

        Max_Jitter = 1.0 / (2.0 * sqrt((DBL)Frame.Camera->Blur_Samples));
    }

    /* Copy standard grid to sample grid. */

    for (i = 0; i < min(Standard_Sample_Grid_Size, Frame.Camera->Blur_Samples); i++)
    {
      Sample_Grid[i] = Standard_Sample_Grid[i];
    }

    /* Choose remaining samples from a uniform grid to get "best" coverage. */

    if (Frame.Camera->Blur_Samples > Standard_Sample_Grid_Size)
    {
      /* Get sub-pixel grid size (I want it to be odd). */

      Grid_Size = (int)sqrt((DBL)Frame.Camera->Blur_Samples) + 1;

      if ((Grid_Size & 1) == 0)
      {
        Grid_Size++;
      }

      /* Allocate temporary grid. */

      Grid = (char **)POV_MALLOC(Grid_Size * sizeof(int *), "temporary sub-pixel grid");

      for (i = 0; i < Grid_Size; i++)
      {
        Grid[i] = (char *)POV_CALLOC((unsigned)Grid_Size, sizeof(int), "temporary sub-pixel grid");
      }

      /* Mark sub-pixels already covered. */

      for (i = 0; i < Standard_Sample_Grid_Size; i++)
      {
        xi = (int)((Sample_Grid[i].x + 0.5) * (DBL)Grid_Size);
        yi = (int)((Sample_Grid[i].y + 0.5) * (DBL)Grid_Size);

        Grid[yi][xi] = TRUE;
      }

      /* Distribute remaining samples. */

      for (i = Standard_Sample_Grid_Size; i < Frame.Camera->Blur_Samples; )
      {
        xi = POV_RAND() % Grid_Size;
        yi = POV_RAND() % Grid_Size;

        if (!Grid[yi][xi])
        {
          x = (DBL)(2 * xi + 1) / (DBL)(2 * Grid_Size) - 0.5;
          y = (DBL)(2 * yi + 1) / (DBL)(2 * Grid_Size) - 0.5;

          Sample_Grid[i].x = x;
          Sample_Grid[i].y = y;

          Grid[yi][xi] = TRUE;

          i++;
        }
      }

      /* Free temporary grid. */

      for (i = 0; i < Grid_Size; i++)
      {
        POV_FREE(Grid[i]);
      }

      POV_FREE(Grid);
    }

    /*
     * Calculate vectors perpendicular to the current ray
     * We're making a "+" (crosshair) on the film plane.
     */

    /* XPerp = vector perpendicular to y/z plane */

    VCross(XPerp, Frame.Camera->Up, Frame.Camera->Direction);

    VNormalize(XPerp, XPerp);

    /* YPerp = vector perpendicular to x/z plane */

    VCross(YPerp, Frame.Camera->Direction, XPerp);

    VNormalize(YPerp, YPerp);

    /* Get adjusted distance to focal plane. */

    VLength(len, Frame.Camera->Direction);

    Focal_Distance = Frame.Camera->Focal_Distance / len;
  }

  /* If a single frame is traced disable field rendering. */
/* Disabled 11/12/95 CEY
  if (opts.FrameSeq.FrameType == FT_SINGLE_FRAME)
  {
    opts.FrameSeq.Field_Render_Flag = FALSE;
  }
*/

  /* We have to precalculate all camera constants. */

  Precompute_Camera_Constants = TRUE;

  Primary_Ray_State_Tested = FALSE; 
}



/*****************************************************************************
*
* FUNCTION
*
*   Terminate_Renderer
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

void Terminate_Renderer()
{
  if (Previous_Line != NULL)
  {
    POV_FREE(Previous_Line);
    POV_FREE(Current_Line);

    Previous_Line = NULL;
    Current_Line  = NULL;
  }

  if (Previous_Line_Antialiased_Flags != NULL)
  {
    POV_FREE(Previous_Line_Antialiased_Flags);
    POV_FREE(Current_Line_Antialiased_Flags);

    Previous_Line_Antialiased_Flags = NULL;
    Current_Line_Antialiased_Flags  = NULL;
  }

  if (Focal_Blur_Is_Used)
  {
    if (Sample_Threshold != NULL)
    {
      POV_FREE(Sample_Threshold);

      Sample_Threshold = NULL;
    }

    if (Sample_Grid != NULL)
    {
      POV_FREE(Sample_Grid);

      Sample_Grid = NULL;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Read_Rendered_Part
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
*   Sep 1994 : Call extract_colors to get pixel's color. [DB]
*
******************************************************************************/

void Read_Rendered_Part(char *New_Fname)
{
  int rc, x, line_number = -1;
  unsigned char Red, Green, Blue, Alpha;
  DBL grey;

  maxclr = (DBL)(1 << Color_Bits) - 1.0;

  while ((rc = Read_Line(Output_File_Handle, Previous_Line, &line_number)) == 1)
  {
    if (opts.Options & DISPLAY)
    {
      for (x = 0; x < Frame.Screen_Width ; x++)
      {
        extract_colors(Previous_Line[x], &Red, &Green, &Blue, &Alpha, &grey);

        POV_DISPLAY_PLOT(x, line_number, Red, Green, Blue, Alpha);

        COOPERATE_1
      }
    }
  }

  opts.First_Line = line_number + 1;
  if(opts.First_Line < 1)
  {
    opts.First_Line = 1;
  }

  Close_File(Output_File_Handle);

  if (rc == 0)
  {
    if (Open_File(Output_File_Handle, New_Fname,
        &Frame.Screen_Width, &Frame.Screen_Height, opts.File_Buffer_Size,
        APPEND_MODE) != 1)
    {
      Error("Error opening output file.\n");
    }

    return;
  }

  Error("Error reading aborted data file.\n");
}



/*****************************************************************************
*
* FUNCTION
*
*   Check_User_Abort
*
* INPUT
*
*   Forced -- if FALSE, then check else force user abort
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
*   Exit with error if image not completed/user abort.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Check_User_Abort(int Forced)
{
  if (Forced)
  {
     Stop_Flag=TRUE;
  }
  else
  {
    if (--opts.Abort_Test_Counter <= 0)
    {
      opts.Abort_Test_Counter = Abort_Test_Every;

      TEST_ABORT
    }
  }

  if (Stop_Flag)
  {
    Render_Info("\nAborting render...\n");

    if ((opts.Options & DISPLAY) && Display_Started)
    {
      POV_DISPLAY_CLOSE
    }

    if (opts.Do_Stats)
    {
      PRINT_STATS(stats);
    }

    Error("User abort.\n");
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Start_Tracing_Mosaic_Preview
*
* INPUT
*
*   MosaicPixelSize - # of pixels square to start
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Eduard Schwan
*   
* DESCRIPTION
*
*   Trace the entire image, but instead of doing it pixel by pixel,
*   draw every 8th pixel, then every 4th, then every 2nd, then
*   every pixel. This shows the image as a quick chunky preview,
*   and the image gets more detailed as each pass redraws it in
*   increasing detail.  This is a simple subdivision preview method.
*   On the first pass, the entire screen is painted.  On each subsequent
*   pass, smaller squares are painted to fill in more detail.  Any
*   squares that were already calculated on previous passes are NOT
*   redrawn, making this mode just as fast as the non-preview rendering
*   mode (ignoring additional time used by rectangle screen painting.)
*
* CHANGES
*
*   Aug 1994 : Created from Start_Tracing, by Eduard Schwan
*   Dec 1994 : Updated for more regular scanning, by Eduard Schwan
*
******************************************************************************/

void Start_Tracing_Mosaic_Preview(int StartPixelSize, int  EndPixelSize)
{
  unsigned char Red, Green, Blue, Alpha;
  int x, y, x2, y2, PreviewStep, PixelSize, AlreadyPainted, PreviewPass;
  COLOUR Colour;
  DBL grey;

  /* PreviewStep tracks how many pixels to skip on each square painted */

  PreviewStep = StartPixelSize;

  /* do each pass of the image */
  /* increment pass counter and divide PixelSize & PreviewStep by 2 */

  for (PreviewPass = 1, PixelSize = StartPixelSize;
        PixelSize >= EndPixelSize;
        PreviewPass++, PixelSize >>= 1, PreviewStep >>= 1)
  {
    /* do each row */

    for (y = opts.First_Line; y < opts.Last_Line; y += PreviewStep)
    {
      /* show some status information */
      /* note, this fn should change to show new style for preview */

      check_stats(y, 1, PixelSize);

      /* do each column/pixel on a row */

      for (x = opts.First_Column; x < opts.Last_Column; x += PreviewStep)
      {
        Check_User_Abort(FALSE);

        /*
         * Skip any pixels we have done previously.  These would be any pixels
         * that are on a row AND column that is divisible by the previous
         * pass's step-size.  On the first pass, however, do ALL pixels, since
         * there is nothing to skip.
         */

        AlreadyPainted = FALSE;

        if (PreviewPass > 1)
        {
          if ( ((x-opts.First_Column) % (PreviewStep*2) == 0) &&
               ((y-opts.First_Line) % (PreviewStep*2) == 0) )
          {
            AlreadyPainted = TRUE;
          }
        }

        if (!AlreadyPainted)
        {
          /* OK, it is safe to draw this pixel */

          trace_pixel(x, y, Colour);

          extract_colors(Colour, &Red, &Green, &Blue, &Alpha, &grey);

          y2 = min(y + PixelSize - 1, opts.Last_Line-1);
          x2 = min(x + PixelSize - 1, opts.Last_Column-1);

          POV_DISPLAY_PLOT_RECT(x, y, x2, y2, Red, Green, Blue, Alpha);
        }
      }

      if (opts.Options & VERBOSE)
      {
        Status_Info("\r");
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Start_Tracing_Mosaic_Smooth
*
* INPUT
*
*   MosaicPixelSize - # of pixels square to start
*   
* OUTPUT
*
* RETURNS
*   
* AUTHOR
*
*   Eduard Schwan
*   
* DESCRIPTION
*
*   See comments for Start_Tracing_Mosaic_Preview.
*   Radiosity requires this pass, so it is not optional, and does not
*   require you to set Mosaic_Preview_Start and End.
*   This function does a smooth interpolation of pixel values, so as
*   a result, it re-renders the pixels done on previous passes, so it
*   is about 1/16th slower.  This function will take any tile size, not
*   just powers of two.  The smooth interpolation is conditional, since
*   on some platforms it is quite slow.  When you're doing radiosity,
*   life is slow anyway, so the smooth effect is very nice.
*   Most important difference is that this function does jittering, so
*   that the places at which radiosity tends to be calculated don't lie
*   on a grid and produce unpleasant grid artefacts.
*   This doesn't really have to be a separate function;  the two
*   could be merged.
*
* CHANGES
*
*   Apr 1995 : Created with first radiosity patches
*   Feb 1996 : Made the row buffers dynamically allocated [AED]
*
******************************************************************************/
void Start_Tracing_Mosaic_Smooth(int StartPixelSize, int  EndPixelSize)
{
  unsigned char Red, Green, Blue, Alpha;
  unsigned char *thisr = NULL, *thisg = NULL, *thisb = NULL, *thisa = NULL;
  unsigned char *upr = NULL, *upg = NULL, *upb = NULL, *upa = NULL;
  int Smooth_Preview = 0;
  int dx, dy, skip,
      tr, tg, tb, ta,
      lastr, lastg, lastb, lasta,
      ulr, urr, llr, lrr,
      ulg, urg, llg, lrg,
      ulb, urb, llb, lrb,
      ula, ura, lla, lra,
      lor, log, lob, loa,
      hir, hig, hib, hia,
      tx, ty, jitter_range, jitter_offset, offset_x, offset_y, first_pass,
      x, y, x2, y2;
  DBL grey, gather_grey;
  COLOUR Colour, avg_gather;

  lastr = lastg = lastb = lasta = 0;

  opts.Radiosity_Error_Bound *= opts.Radiosity_Low_Error_Factor;

  /* Initialize the accumulators which will allow us to set average amb Brightness */
  Make_Colour(Radiosity_Gather_Total, 0.0, 0.0, 0.0);
  Radiosity_Gather_Total_Count = 0;

  /* if radiosity is on, you MUST use preview pass to get reasonable results.
   * 8x8 is generally a good size to use if the user didn't specify anything.
   */
  if ( StartPixelSize == 1 )  StartPixelSize = EndPixelSize = 8;

  /* Prevent 2x2 or 1x1 passes - this code is very slow at 2x2 or less */
  if ( StartPixelSize < 4)  StartPixelSize = 4;
  if ( EndPixelSize   < 4)  EndPixelSize = 4;

  /* if there is no visible output, might as well just do one pass, it's faster.
   * The last pass is the one which determines the values which get put into
   * the radiosity tree, so just do the last (end) pass.
   */
  if ( !(opts.Options & DISPLAY))  StartPixelSize = EndPixelSize;

  /* Finally, end size must always be less than or equal to start size */
  if ( EndPixelSize > StartPixelSize ) EndPixelSize = StartPixelSize;


  skip = StartPixelSize;
  first_pass = TRUE;

  if (opts.Options & DISPLAY)
  {
    upr = (unsigned char *)POV_MALLOC(opts.Last_Column, "mosaic row buffer");
    upg = (unsigned char *)POV_MALLOC(opts.Last_Column, "mosaic row buffer");
    upb = (unsigned char *)POV_MALLOC(opts.Last_Column, "mosaic row buffer");
    upa = (unsigned char *)POV_MALLOC(opts.Last_Column, "mosaic row buffer");

    thisr = (unsigned char *)POV_MALLOC(opts.Last_Column, "mosaic row buffer");
    thisg = (unsigned char *)POV_MALLOC(opts.Last_Column, "mosaic row buffer");
    thisb = (unsigned char *)POV_MALLOC(opts.Last_Column, "mosaic row buffer");
    thisa = (unsigned char *)POV_MALLOC(opts.Last_Column, "mosaic row buffer");
  }

  while ((skip >= 4) && (skip >= EndPixelSize))
  {
    /* for each pass */

    jitter_range  = 3;
    jitter_offset = skip / 2 - 1;       /* add a very small amount of jitter */

    if ( skip <= 8 ) Smooth_Preview = 1;

    for (y = opts.First_Line; y < opts.Last_Line; y += skip)
    {
      check_stats(y, 1, skip);

      for (x = opts.First_Column; x < opts.Last_Column; x += skip)
      {
        Check_User_Abort(FALSE);

        offset_x = jitter_offset + (POV_RAND() % jitter_range);
        offset_y = jitter_offset + (POV_RAND() % jitter_range);

        trace_pixel(x + offset_x, y + offset_y, Colour);

        extract_colors(Colour, &Red, &Green, &Blue, &Alpha, &grey);

        Assign_Colour(Current_Line[x], Colour);
        POV_ASSIGN_PIXEL (x, y, Colour)

        if (opts.Options & DISPLAY)
        {
          if ( Smooth_Preview )
          {
            /* if smooth colour blending desired */

            if (y == opts.First_Line)
            {
              upr[x] = Red;
              upg[x] = Green;
              upb[x] = Blue;
              upa[x] = Alpha;
            }

            ulr = (x>opts.First_Column) ? upr[x-skip] : Red;
            urr = upr[x];

            llr = (x>opts.First_Column) ? lastr   : Red;
            lrr = Red;

            ulg = (x>opts.First_Column) ? upg[x-skip] : Green;
            urg = upg[x];

            llg = (x>opts.First_Column) ? lastg   : Green;
            lrg = Green;

            ulb = (x>opts.First_Column) ? upb[x-skip] : Blue;
            urb = upb[x];

            llb = (x>opts.First_Column) ? lastb   : Blue;
            lrb = Blue;

            ula = (x>opts.First_Column) ? upa[x-skip] : Alpha;
            ura = upa[x];

            lla = (x>opts.First_Column) ? lasta   : Alpha;
            lra = Alpha;

            for (ty = y, dy = 0; (dy < skip) && (ty < opts.Last_Line); dy++, ty++)
            {
              lor = (ulr * (skip-dy) + llr * dy) / skip;
              hir = (urr * (skip-dy) + lrr * dy) / skip;
              log = (ulg * (skip-dy) + llg * dy) / skip;
              hig = (urg * (skip-dy) + lrg * dy) / skip;
              lob = (ulb * (skip-dy) + llb * dy) / skip;
              hib = (urb * (skip-dy) + lrb * dy) / skip;
              loa = (ula * (skip-dy) + lla * dy) / skip;
              hia = (ura * (skip-dy) + lra * dy) / skip;

              for (tx = x, dx = 0; (dx < skip) && (tx < opts.Last_Column); dx++, tx++)
              {
                tr = (lor * (skip - dx) + (hir * dx)) / skip;
                tg = (log * (skip - dx) + (hig * dx)) / skip;
                tb = (lob * (skip - dx) + (hib * dx)) / skip;
                ta = (loa * (skip - dx) + (hia * dx)) / skip;

                POV_DISPLAY_PLOT (tx, y+dy, tr, tg, tb, ta);
              }
            }

            thisr[x] = Red;
            thisg[x] = Green;
            thisb[x] = Blue;
            thisa[x] = Alpha;

            lastr = Red;
            lastg = Green;
            lastb = Blue;
            lasta = Alpha;
          }
          else
          {
            y2 = min(y + skip - 1, opts.Last_Line - 1);
            x2 = min(x + skip - 1, opts.Last_Column - 1);

            POV_DISPLAY_PLOT_RECT(x, y, x2, y2, Red, Green, Blue, Alpha);
          }
        }
      } /* end loop for each block horizontally in a row of blocks */

      /* Swap the previous and current row buffers */
      if (opts.Options & DISPLAY)
      {
        unsigned char * temp;

        temp = upr;
        upr = thisr;
        thisr = temp;

        temp = upg;
        upg = thisg;
        thisg = temp;

        temp = upb;
        upb = thisb;
        thisb = temp;

        temp = upa;
        upa = thisa;
        thisa = temp;
      }

      if (opts.Options & VERBOSE)
      {
        if (opts.Options & RADIOSITY)
        {
          Status_Info("  %ld radiosity samples", ra_gather_count - RadiosityCount);
        }

        Status_Info(".    \r");
      }
    } /* end loop of rows of blocks */

    /*
     * This adjusts the overall brightness value so that the darkening
     * effect of the radiosity calculation is cancelled out.
     */

    if (first_pass)
    {
      /* Ensure that the average ambient value returned by compute_ambient() is about
       * the same as the average ambient value setting in the scene file
       */

      if ( Radiosity_Gather_Total_Count )
      {
        VInverseScale(avg_gather,  Radiosity_Gather_Total,  (DBL)Radiosity_Gather_Total_Count);
        gather_grey  = avg_gather[RED]  + avg_gather[GREEN]  + avg_gather[BLUE];
        if ( gather_grey > 0. )
        {
          opts.Radiosity_Brightness = 3. / gather_grey;
          if ( ot_fd != NULL)
          { 
            fprintf(ot_fd, "B%g\n", opts.Radiosity_Brightness);
          } 
        }
      }

      first_pass = 0;
    }

    skip /= 2;
  } /* end loop of different resolutions */

  /* Free our row buffers */
  if (opts.Options & DISPLAY)
  {
    POV_FREE(upr);      upr=NULL;  
    POV_FREE(upg);      upg=NULL;  
    POV_FREE(upb);      upb=NULL;  
    POV_FREE(upa);      upa=NULL;  
                                          
    POV_FREE(thisr);    thisr=NULL;
    POV_FREE(thisg);    thisg=NULL;
    POV_FREE(thisb);    thisb=NULL;
    POV_FREE(thisa);    thisa=NULL;
  }

  opts.Radiosity_Error_Bound /= opts.Radiosity_Low_Error_Factor;

  if (opts.Options & VERBOSE)
  {
    Status_Info("                                                                      \r");
  }
  
  /* Are we in the process of creating a radiosity cache file? */
  if ( ot_fd != NULL )
  {
    fprintf(ot_fd, "P\n");   /* code meaning that preview pass was completed */
  }
  opts.Radiosity_Preview_Done = 1;
}


/*****************************************************************************
*
* FUNCTION
*
*   Start_Non_Adaptive_Tracing
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
*   Trace pixels by shooting rays at the center of each pixel. If the
*   colors between a pixel and its left and/or upper neighbor differ
*   too much all or some of the pixel are supersampled using a fixed
*   number of rays.
*
* CHANGES
*
*   Aug 1994 : Modified to call common extract_colors() fn, Eduard Schwan
*
*   Sep 1994 : Added code for vista buffer. [DB]
*
*   Aug 1995 : Added Field Rendering for NTSC/PAL Animations, Jeff Bowermaster
*
*   Aug 1995 : Added code to jitter pixel location. [DB]
*
******************************************************************************/

void Start_Non_Adaptive_Tracing()
{
  int x, y;
  int antialias_line = TRUE;
  int skip_lines;
  int first_line;
  int skip_odd_lines;

  /* Set jitterscale. */

  JitterScale = opts.JitterScale / (DBL)opts.AntialiasDepth;

  /* Odd/even line tracing depends on the frame number. */

  skip_odd_lines = !((opts.FrameSeq.FrameNumber % 2) ^ opts.FrameSeq.Odd_Field_Flag);

  /* Field rendering starts on an odd or even line. */

  skip_lines = (opts.FrameSeq.Field_Render_Flag) && !(opts.Options & ANTIALIAS);

  /* Get first line number. */

  first_line = (opts.Options & ANTIALIAS)?opts.First_Line-1:opts.First_Line;

  /* Loop over all rows. */

  for (y = first_line; y < opts.Last_Line; y++)
  {
    /* Skip odd or even lines depending on the line number. */

    if ((skip_lines) && ((y % 2) == skip_odd_lines))
    {
      /* Write previous line again. */

      if ((opts.Options & DISKWRITE) && (y > opts.First_Line))
      {
        Write_Line(Output_File_Handle, Previous_Line, y);
      }
      POV_WRITE_LINE (Previous_Line, y)

      continue;
    }

    check_stats(y, 0, 1);

    /* Prune vista tree. */

    Prune_Vista_Tree(y);

    /* Precalculate whether to antialias a line. */

    if (opts.FrameSeq.Field_Render_Flag)
    {
      if (y >= opts.First_Line)
      {
        antialias_line = ((y % 2) ^ skip_odd_lines);
      }
      else
      {
        antialias_line = FALSE;
      }
    }

    /* Loop over all columns. */

    for (x = opts.First_Column; x < opts.Last_Column; x++)
    {
      /* Check for user abort. */

      Check_User_Abort(FALSE);

      /* Trace current pixel. */

/*
      Debug_Info("y = %3d, x = %3d\n", y, x);
*/

      trace_pixel(x, y, Current_Line[x]);

      /* Apply anti-aliasing. */

      if ((opts.Options & ANTIALIAS) && antialias_line)
      {
        do_anti_aliasing(x, y, Current_Line[x]);
      }

      /* Display pixel. */

      plot_pixel(x, y, Current_Line[x]);
    }

    /* Write current row to disk. */

    output_line(y);
  }

  /* Write last row to disk. */

  if (opts.Last_Line != opts.First_Line)
  {
    if (opts.Options & DISKWRITE)
    {
      Write_Line(Output_File_Handle, Previous_Line, opts.Last_Line - 1);
    }
    POV_WRITE_LINE (Previous_Line, opts.Last_Line - 1)
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Start_Adaptive_Tracing
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
*   Trace pixels by shooting rays at each corner of a pixel and subdividing
*   if the colors ate the pixel's corenr differ too much. The subdivision
*   is made recursively by further subdividing those sub-pixels whose colors
*   differ too much.
*
*   Note that is doesn't make any sense to skip line during field tracing
*   because samples are taken at the corners of the pixels and are shared
*   among adjacent lines/pixels. Thus leaving every second line doesn't
*   save anything. The subdivision is only done on the odd/even lines
*   depending on the lines to be traced.
*
* CHANGES
*
*   Jul 1995 : Creation
*
*   Aug 1995 : Added field rendering support. [DB]
*
******************************************************************************/

void Start_Adaptive_Tracing()
{
  int x, y, xx, xxx, yy, skip_odd_lines;
  int sub_pixel_size, antialias_line = TRUE;
  size_t size;
  COLOUR Colour;
  PIXEL *First_Row, *Last_Row, **Block, *TempRow, TempPixel;

  /* If no antialiasing is specified use non-adaptive sampling. */

  if (!(opts.Options & ANTIALIAS))
  {
    Start_Non_Adaptive_Tracing();

    return;
  }
  
  /* Init color. */

  Make_ColourA(Colour, 0.0, 0.0, 0.0, 0.0, 0.0);

  /* Odd/even line tracing depends on the frame number. */

  skip_odd_lines = !((opts.FrameSeq.FrameNumber % 2) ^ opts.FrameSeq.Odd_Field_Flag);

  /* Set sub-pixel size to 2**(AntialiasDepth) */

  sub_pixel_size = 1 << (opts.AntialiasDepth);

  /* Set jitterscale. */

  JitterScale = opts.JitterScale / (DBL)(sub_pixel_size+1);

  /* Allocate row arrays */

  size = (sub_pixel_size * Frame.Screen_Width + 1) * sizeof(PIXEL);

  First_Row = (PIXEL *)POV_MALLOC(size, "row buffer");
  Last_Row  = (PIXEL *)POV_MALLOC(size, "row buffer");

  /* Allocate block array */

  Block = (PIXEL **)POV_MALLOC((sub_pixel_size+1) * sizeof(PIXEL *), "block buffer");

  for (y = 0; y < sub_pixel_size + 1; y++)
  {
    Block[y] = (PIXEL *)POV_MALLOC((sub_pixel_size+1) * sizeof(PIXEL), "block buffer");
  }


  /**************************************************************************
   * Init row and block buffer
   **************************************************************************/

  for (x = 0; x < sub_pixel_size * Frame.Screen_Width + 1; x++)
  {
    First_Row[x].active = FALSE;
    Last_Row[x].active  = FALSE;

    Make_ColourA(First_Row[x].Colour, 0.0, 0.0, 0.0, 0.0, 0.0);
    Make_ColourA(Last_Row[x].Colour,  0.0, 0.0, 0.0, 0.0, 0.0);
  }

  for (y = 0; y < sub_pixel_size + 1; y++)
  {
    for (x = 0; x < sub_pixel_size + 1; x++)
    {
      Block[y][x].active = FALSE;

      Make_ColourA(Block[y][x].Colour, 0.0, 0.0, 0.0, 0.0, 0.0);
    }
  }


  /**************************************************************************
   * Trace all lines, i.e. the image
   **************************************************************************/

  for (y = opts.First_Line; y < opts.Last_Line; y++)
  {
    check_stats(y, 0, 1);

    if (opts.Options & USE_VISTA_BUFFER)
    {
      Prune_Vista_Tree(y);
    }

    /* Set last row inactive */

    for (xx = 0; xx < sub_pixel_size * Frame.Screen_Width + 1; xx++)
    {
      Last_Row[xx].active = FALSE;
    }

    /* Set first column inactive */

    for (yy = 0; yy < sub_pixel_size + 1; yy++)
    {
      Block[yy][0].active = FALSE;
    }

    /* Precalculate whether to antialias a line. */

    if (opts.FrameSeq.Field_Render_Flag)
    {
      antialias_line = (y % 2) ^ skip_odd_lines;
    }


    /************************************************************************
     * Trace all pixels on the current line
     ************************************************************************/

    for (x = opts.First_Column; x < opts.Last_Column; x++)
    {
      Check_User_Abort(FALSE);

      Increase_Counter(stats[Number_Of_Pixels]);

      /* Initialize current block */

      for (yy = 1; yy < sub_pixel_size + 1; yy++)
      {
        for (xx = 1; xx < sub_pixel_size + 1; xx++)
        {
          Block[yy][xx].active = FALSE;
        }
      }

      for (xxx = 0, xx = x * sub_pixel_size; xx < (x+1) * sub_pixel_size + 1; xxx++, xx++)
      {
        Block[0][xxx] = First_Row[xx];
      }

      /* Do histogram stuff. */

      if (opts.histogram_on)
      {
        accumulate_histogram(x, y, TRUE);
      }

      /* Trace pixel centered on (x, y) */

      trace_sub_pixel(1, Block, x, y, 0, 0, sub_pixel_size, sub_pixel_size, sub_pixel_size, Colour, antialias_line);

      /* Do histogram stuff. */

      if (opts.histogram_on)
      {
        accumulate_histogram(x, y, FALSE);
      }

      /* Store colour in current line */

      Assign_Colour(Current_Line[x], Colour);
      POV_ASSIGN_PIXEL (x, y, Colour)

      /* Display pixel */

      plot_pixel(x, y, Colour);

      /* Store current block in rows */

      for (xxx = 0, xx = x * sub_pixel_size; xx < (x+1) * sub_pixel_size + 1; xxx++, xx++)
      {
        First_Row[xx] = Block[0][xxx];
        Last_Row[xx]  = Block[sub_pixel_size][xxx];
      }

      /* Swap first and last block column */

      for (yy = 0; yy < sub_pixel_size + 1; yy++)
      {
        TempPixel                 = Block[yy][0];
        Block[yy][0]              = Block[yy][sub_pixel_size];
        Block[yy][sub_pixel_size] = TempPixel;
      }
    }

    output_line(y);

    /* Swap first and last row */

    TempRow   = Last_Row;
    Last_Row  = First_Row;
    First_Row = TempRow;
  }

  /* We've come to the end ... at last! */

  if (opts.Last_Line != opts.First_Line)
  {
    if (opts.Options & DISKWRITE)
    {
      Write_Line (Output_File_Handle, Previous_Line, opts.Last_Line - 1);
    }
    POV_WRITE_LINE (Previous_Line, opts.Last_Line - 1)
  }

  /* Free memory. */

  for (y = 0; y < sub_pixel_size + 1; y++)
  {
    POV_FREE(Block[y]);
  }

  POV_FREE(Block);
  POV_FREE(First_Row);
  POV_FREE(Last_Row);
}


/*****************************************************************************
*
* FUNCTION
*
*   Trace
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   distance to nearest intersection, or BOUND_HUGE on miss
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
*   Nov 1994 : Rearranged calls to Fog, Rainbow and Skyblend.
*              Added call to Atmosphere for atmospheric effects. [DB]
*   Jan 1995 : Set intersection depth to Max_Distance for infinite rays. [DB]
*   Mar 1995 : Added return value for radiosity work [JDM]
*   Jul 1995 : Added code to support alpha channel. [DB]
*
******************************************************************************/

DBL Trace(RAY *Ray, COLOUR Colour, DBL Weight)
{
  int i, Intersection_Found, all_hollow;
  OBJECT *Object;
  INTERSECTION Best_Intersection, New_Intersection;

  COOPERATE_0

  Increase_Counter(stats[Number_Of_Rays]);

  /* Transmittance has to be 1 to make alpha channel output to work. [DB] */

  Make_ColourA(Colour, 0.0, 0.0, 0.0, 0.0, 1.0);

  /* Check for max. trace level or ADC bailout. */

  if ((Trace_Level > Max_Trace_Level) || (Weight < ADC_Bailout))
  {
    if (Weight < ADC_Bailout)
    {
      Increase_Counter(stats[ADC_Saves]);
    }

    return (BOUND_HUGE);
  }

  /* Set highest level traced. */

  if (Trace_Level > Highest_Trace_Level)
  {
    Highest_Trace_Level = Trace_Level;
  }

  /* What objects does this ray intersect? */

  Intersection_Found = FALSE;

  Best_Intersection.Depth = BOUND_HUGE;

  if (!opts.Use_Slabs)
  {
    for (Object = Frame.Objects; Object != NULL; Object = Object -> Sibling)
    {
      if (Intersection(&New_Intersection, Object, Ray))
      {
        if (New_Intersection.Depth < Best_Intersection.Depth)
        {
          Best_Intersection = New_Intersection;

          Intersection_Found = TRUE;
        }
      }
    }
  }
  else
  {
    Intersection_Found = Intersect_BBox_Tree(Root_Object, Ray,
           &Best_Intersection, &Object);
  }

  /* Get color for this ray. */

  if (Intersection_Found)
  {
    /* Determine colour of object hit. */

    Determine_Apparent_Colour(&Best_Intersection, Colour, Ray, Weight);
  }
  else
  {
    /* Infinite ray, set intersecton distance. */

    Best_Intersection.Depth = Max_Distance;

    /* Apply infinite atmospheric effects. */

    Do_Infinite_Atmosphere(Ray, Colour);
  }

  /* Test if all contained objects are hollow. */

  all_hollow = TRUE;

  if (Ray->Index > -1)
  {
    for (i = 0; i <= Ray->Index; i++)
    {
      if (!Ray->Interiors[i]->hollow)
      {
        all_hollow = FALSE;

        break;
      }
    }
  }

  /* Apply finite atmospheric effects. */
  if (all_hollow && (opts.Quality_Flags & Q_VOLUME))
  {
    Do_Finite_Atmosphere(Ray, &Best_Intersection, Colour, FALSE);
  }

  return (Best_Intersection.Depth);
}



/*****************************************************************************
*
* FUNCTION
*
*   do_anti_aliasing
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
*   Aug 1995 : Modified to avoid unnecessary pixel output. [DB]
*
*   Aug 1995 : Modified to avoid supersampling of unused lines
*              when using field tracing. [DB]
*
******************************************************************************/

static void do_anti_aliasing(register int x, register int y, COLOUR Colour)
{
  char Antialias_Center_Flag = FALSE;

  Current_Line_Antialiased_Flags[x] = FALSE;

  /* Test difference to pixel left of current pixel. */

  if (x != 0)
  {
    if (Colour_Distance(Current_Line[x-1],Current_Line[x]) >= Frame.Antialias_Threshold)
    {
      Antialias_Center_Flag = TRUE;

      if (!(Current_Line_Antialiased_Flags[x-1]))
      {
        supersample(Current_Line[x-1], x-1, y);

        Current_Line_Antialiased_Flags[x-1] = TRUE;

        SuperSampleCount++;

        plot_pixel(x-1, y, Current_Line[x-1]);

        COOPERATE_1
      }
    }
  }

  /* Test difference to pixel above current pixel. */

  if ((y != opts.First_Line-1) && (!opts.FrameSeq.Field_Render_Flag))
  {
    if (Colour_Distance(Previous_Line[x],Current_Line[x]) >= Frame.Antialias_Threshold)
    {
      Antialias_Center_Flag = TRUE;

      if (!(Previous_Line_Antialiased_Flags[x]))
      {
        supersample(Previous_Line[x], x, y-1);

        Previous_Line_Antialiased_Flags[x] = TRUE;

        SuperSampleCount++;

        plot_pixel(x, y, Previous_Line[x]);

        COOPERATE_1
      }
    }
  }

  /* Supersample current pixel if necessary. */

  if (Antialias_Center_Flag)
  {
    supersample(Current_Line[x], x, y);

    Current_Line_Antialiased_Flags[x] = TRUE;

    Assign_Colour(Colour, Current_Line[x]);
    POV_ASSIGN_PIXEL (x, y, Colour)

    SuperSampleCount++;

    COOPERATE_1
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   supersample
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
*   Standard sampling in loop
*
* CHANGES
*
*   Aug 1995 : Modified to avoid resampling of center sub-pixel. [DB]
*
*   Sep 1995 : Weight of a primary ray has to be 1 regardless of its
*              contribution to a supersampled pixel! [DB]
*
******************************************************************************/

static void supersample(COLOUR result, int x, int  y)
{
  int i, j, samples;
  int JRange, JSteps;
  DBL JSize, JScale;
  DBL Jitter_X, Jitter_Y;
  DBL dx, dy;
  DBL save_radiosity_error_bound;
  COLOUR colour;

  /* Why are we here? */

  if (opts.AntialiasDepth <= 1)
  {
    return;
  }

  Increase_Counter(stats[Number_Of_Pixels_Supersampled]);

  /* Number of samples in pixel (used to scale resulting color). */

  samples = 1;

  /* Substantially reduces chances of doing new samples. */

  save_radiosity_error_bound = opts.Radiosity_Error_Bound;

  opts.Radiosity_Error_Bound *= 2.0;

  /* JSize is the size of the jitter scattering area */

  JSize = 1.0 / opts.AntialiasDepth;

  /*
   * JSteps is either 1 or 2 depending on whether the number of samples
   * is odd or even. This is because the loop need to either run through
   * or over 0.
   */

  JSteps = 2 - (opts.AntialiasDepth % 2);

  /*
   * JRange is the range that the loop will run through. I couldn't
   * come up with a function describing the values, so I used an array
   * for 2x2 up to 9x9.
   */

  JRange = JRanges[opts.AntialiasDepth];

  /*
   * JScale is the value with which the current sub-pixel indices
   * (i,j) have to be scaled to get the real sub-pixel positions.
   */

  JScale = JSize / (DBL)JSteps;

  /* Loop over all sub-pixels. */

  for (i = -JRange; i <= JRange; i += JSteps)
  {
    for (j = -JRange; j <= JRange; j += JSteps)
    {
      /* Skip center sub-pixel because we already traced it. */

      if ((i == 0) && (j == 0))
      {
        continue;
      }

      /* Increase number of samples. */

      samples++;

      /* Jitter grid location. */

      jitter_pixel_position(x, y, &Jitter_X, &Jitter_Y);

      /* Trace ray through current sub-pixel. */

      dx = Jitter_X + i * JScale;
      dy = Jitter_Y + j * JScale;

      if (create_ray(&Camera_Ray, (DBL)x+dx, (DBL)y+dy, 0))
      {
        Trace_Level = 1;

        Increase_Counter(stats[Number_Of_Samples]);

        if (opts.Options & USE_VISTA_BUFFER)
        {
          Trace_Primary_Ray(&Camera_Ray, colour, 1.0, x);
        }
        else
        {
          Trace(&Camera_Ray, colour, 1.0);
        }

        Clip_Colour(colour, colour);

        gamma_correct(colour);

        Add_Colour(result, result, colour);
      }
      else
      {
        Make_ColourA(colour, 0.0, 0.0, 0.0, 0.0, 1.0);
      }
    }
  }

  /* Average pixel's color. */

  Scale_Colour(result, result, 1.0 / (DBL)samples);

  opts.Radiosity_Error_Bound = save_radiosity_error_bound;
}


/*****************************************************************************
*
* FUNCTION
*
*   trace_sub_pixel
*
* INPUT
*
*   level     - current subdivision level
*   Block     - sub-pixel information of current pixel
*   x, y      - current pixel
*   x1, y1    - upper left corner of current sub-pixel
*   x3, y3    - lower right corner of current sub-pixel
*   size      - sub-pixel size
*   antialias - TRUE if antialiasing is allowed
*
* OUTPUT
*
*   Colour   - (sub-)pixels color
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Trace rays at the corners of the (sub-)pixel. If the colors differ
*   too much and the max. recursion level isn't reached yet subdivide
*   pixel into four sub-pixel and trace those.
*
* CHANGES
*
*   Jul 1995 : Creation.
*
******************************************************************************/

static void trace_sub_pixel(int level, PIXEL **Block, int x, int  y, int  x1, int  y1, int  x3, int  y3, int  size, COLOUR Colour, int antialias)
{
  int x2, y2;    /* Coordinates of center sub-pixel of current block.     */
  DBL dx1, dy1;  /* coord. of upper left corner relative to pixel coord.  */
  DBL dx3, dy3;  /* coord. of lower right corner relative to pixel coord. */
  COLOUR C1, C2, C3, C4;

  /* Get offsets for corner pixels. */

  dx1 = (DBL)(x1 - size / 2) / (DBL)size;
  dx3 = (DBL)(x3 - size / 2) / (DBL)size;

  dy1 = (DBL)(y1 - size / 2) / (DBL)size;
  dy3 = (DBL)(y3 - size / 2) / (DBL)size;

  /* Trace upper left corner pixel. */

  if (!Block[y1][x1].active)
  {
    trace_ray_with_offset(x, y, dx1, dy1, C1);

    Block[y1][x1].active = TRUE;

    Assign_Colour(Block[y1][x1].Colour, C1);
  }
  else
  {
    Assign_Colour(C1, Block[y1][x1].Colour);
  }

  /* Trace lower left corner pixel. */

  if (!Block[y3][x1].active)
  {
    trace_ray_with_offset(x, y, dx1, dy3, C2);

    Block[y3][x1].active = TRUE;

    Assign_Colour(Block[y3][x1].Colour, C2);
  }
  else
  {
    Assign_Colour(C2, Block[y3][x1].Colour);
  }

  /* Trace upper right corner pixel. */

  if (!Block[y1][x3].active)
  {
    trace_ray_with_offset(x, y, dx3, dy1, C3);

    Block[y1][x3].active = TRUE;

    Assign_Colour(Block[y1][x3].Colour, C3);
  }
    else
  {
    Assign_Colour(C3, Block[y1][x3].Colour);
  }

  /* Trace lower right corner pixel. */

  if (!Block[y3][x3].active)
  {
    trace_ray_with_offset(x, y, dx3, dy3, C4);

    Block[y3][x3].active = TRUE;

    Assign_Colour(Block[y3][x3].Colour, C4);
  }
  else
  {
    Assign_Colour(C4, Block[y3][x3].Colour);
  }

  /* Do we have to check for supersampling? */

  if (antialias && (level <= opts.AntialiasDepth))
  {
    /* Check if upper left sub-block should be supersampled. */

    if ((Colour_Distance(C1, C2) >= Frame.Antialias_Threshold) ||
        (Colour_Distance(C2, C4) >= Frame.Antialias_Threshold) ||
        (Colour_Distance(C3, C4) >= Frame.Antialias_Threshold) ||
        (Colour_Distance(C1, C3) >= Frame.Antialias_Threshold) ||
        (Colour_Distance(C1, C4) >= Frame.Antialias_Threshold) ||
        (Colour_Distance(C2, C3) >= Frame.Antialias_Threshold))
    {
      /* Get coordinates of center sub-pixel. */

      x2 = (x1 + x3) / 2;
      y2 = (y1 + y3) / 2;

      /* Trace the four sub-blocks. */

      trace_sub_pixel(level+1, Block, x, y, x1, y1, x2, y2, size, C1, antialias);
      trace_sub_pixel(level+1, Block, x, y, x1, y2, x2, y3, size, C2, antialias);
      trace_sub_pixel(level+1, Block, x, y, x2, y1, x3, y2, size, C3, antialias);
      trace_sub_pixel(level+1, Block, x, y, x2, y2, x3, y3, size, C4, antialias);

      if (level == 1)
      {
        SuperSampleCount++;
      }
    }
  }

  /* Average sub-block colors. */

  Colour[RED]    = 0.25 * (C1[RED]    + C2[RED]    + C3[RED]    + C4[RED]);
  Colour[GREEN]  = 0.25 * (C1[GREEN]  + C2[GREEN]  + C3[GREEN]  + C4[GREEN]);
  Colour[BLUE]   = 0.25 * (C1[BLUE]   + C2[BLUE]   + C3[BLUE]   + C4[BLUE]);
  Colour[TRANSM] = 0.25 * (C1[TRANSM] + C2[TRANSM] + C3[TRANSM] + C4[TRANSM]);
}


/*****************************************************************************
*
* FUNCTION
*
*   trace_ray_with_offset
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
*   Trace a ray through the pixel at (x,y) with an offset (dx,dy)
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Sep 1995 : Weight of a primary ray has to be 1 regardless of its
*              contribution to a supersampled pixel! [DB]
*
******************************************************************************/

static void trace_ray_with_offset(int x, int  y, DBL dx, DBL  dy, COLOUR Colour)
{
  DBL Jitter_X, Jitter_Y;

  if (Focal_Blur_Is_Used)
  {
    focal_blur(&Camera_Ray, Colour, (DBL)x, (DBL)y);
  }
  else
  {
    /* Jitter the ray */

    if (opts.Options & ANTIALIAS)
    {
      jitter_pixel_position(x, y, &Jitter_X, &Jitter_Y);
    }
    else
    {
      Jitter_X = Jitter_Y = 0.0;
    }

    if (create_ray (&Camera_Ray, (DBL)x+dx+Jitter_X, (DBL)y+dy+Jitter_Y, 0))
    {
      Trace_Level = 1;

      Increase_Counter(stats[Number_Of_Samples]);

      if (opts.Options & USE_VISTA_BUFFER)
      {
        Trace_Primary_Ray(&Camera_Ray, Colour, 1.0, x);
      }
      else
      {
        Trace(&Camera_Ray, Colour, 1.0);
      }

      Clip_Colour(Colour, Colour);

      gamma_correct(Colour);
    }
    else
    {
      Make_ColourA(Colour, 0.0, 0.0, 0.0, 0.0, 1.0);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   focal_blur
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
*   Calls create_ray(), which calls jitter_camera_ray() to apply the
*   correct amount of jitter to the ray.  This routine merely sends out
*   the correct number of rays and averages them into Colour.
*
* CHANGES
*
*   Jul 1995 : Added code to use a different sub-pixel location for
*              each sample. Added code to do a statistic confident
*              test to make early exists possible. [DB]
*
******************************************************************************/

static void focal_blur(RAY *Ray, COLOUR Colour, DBL x, DBL  y)
{
  int nr;     /* Number of current samples. */
  int level;  /* Index into number of samples list. */
  int max_s;  /* Number of samples to take before next confidence test. */
  int dxi, dyi;
  int i;
  DBL dx, dy, n;
  COLOUR C, V1, S1, S2;

  Make_ColourA(Colour, 0.0, 0.0, 0.0, 0.0, 0.0);

  Make_ColourA(V1, 0.0, 0.0, 0.0, 0.0, 0.0);

  Make_ColourA(S1, 0.0, 0.0, 0.0, 0.0, 0.0);

  Make_ColourA(S2, 0.0, 0.0, 0.0, 0.0, 0.0);

  nr = 0;

  level = 0;

  do
  {
    /* Trace number of rays given by the list Current_Number_Of_Samples[]. */

    max_s = 4;

    if (Current_Number_Of_Samples != NULL)
    {
      if (Current_Number_Of_Samples[level] > 0)
      {
        max_s = Current_Number_Of_Samples[level];

        level++;
      }
    }

    for (i = 0; (i < max_s) && (nr < Frame.Camera->Blur_Samples); i++)
    {
      /* Choose sub-pixel location. */

      dxi = POV_RAND() % SUB_PIXEL_GRID_SIZE;
      dyi = POV_RAND() % SUB_PIXEL_GRID_SIZE;

      dx = (DBL)(2 * dxi + 1) / (DBL)(2 * SUB_PIXEL_GRID_SIZE) - 0.5;
      dy = (DBL)(2 * dyi + 1) / (DBL)(2 * SUB_PIXEL_GRID_SIZE) - 0.5;

      /* Add jitter to sub-pixel location. */

      dx += (FRAND() - 0.5) / (DBL)(SUB_PIXEL_GRID_SIZE);
      dy += (FRAND() - 0.5) / (DBL)(SUB_PIXEL_GRID_SIZE);

      /* Create and trace ray. */

      if (create_ray(Ray, x+dx, y+dy, nr))
      {
        Trace_Level = 1;

        Increase_Counter(stats[Number_Of_Samples]);

        Trace(Ray, C, 1.0);

        Clip_Colour(C, C);

        Add_Colour(Colour, Colour, C);
      }
      else
      {
        Make_ColourA(C, 0.0, 0.0, 0.0, 0.0, 1.0);
      }

      /* Add color to color sum. */

      S1[RED]    += C[RED];
      S1[GREEN]  += C[GREEN];
      S1[BLUE]   += C[BLUE];
      S1[TRANSM] += C[TRANSM];

      /* Add color to squared color sum. */

      S2[RED]    += Sqr(C[RED]);
      S2[GREEN]  += Sqr(C[GREEN]);
      S2[BLUE]   += Sqr(C[BLUE]);
      S2[TRANSM] += Sqr(C[TRANSM]);

      nr++;
    }

    /* Get variance of samples. */

    n = (DBL)nr;

    V1[RED]    = (S2[RED]    / n - Sqr(S1[RED]    / n)) / n;
    V1[GREEN]  = (S2[GREEN]  / n - Sqr(S1[GREEN]  / n)) / n;
    V1[BLUE]   = (S2[BLUE]   / n - Sqr(S1[BLUE]   / n)) / n;
    V1[TRANSM] = (S2[TRANSM] / n - Sqr(S1[TRANSM] / n)) / n;

    /* Exit if samples are likely too be good enough. */

    if ((V1[RED]  < Sample_Threshold[nr-1]) && (V1[GREEN]  < Sample_Threshold[nr-1]) &&
        (V1[BLUE] < Sample_Threshold[nr-1]) && (V1[TRANSM] < Sample_Threshold[nr-1]))
    {
      break;
    }
  }
  while (nr < Frame.Camera->Blur_Samples);

  Scale_Colour(Colour, Colour, 1.0 / (DBL)nr);

  gamma_correct(Colour);
}



/*****************************************************************************
*
* FUNCTION
*
*   jitter_camera_ray
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
*   This routine will deflect eye rays only, since it relies on picking
*   up viewpoint information from the "Frame" variable.
*
*   A hexagonal jitter grid is used if number of blur rays is one of
*   7, 19, or 37, although this should probably be done differently.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void jitter_camera_ray(RAY *ray, int ray_number)
{
  DBL xjit, yjit, xlen, ylen, r;
  VECTOR temp_xperp, temp_yperp, deflection;

  r = Frame.Camera->Aperture * 0.5;

  xjit = Max_Jitter * ((FRAND() * 2.0) - 1.0);
  yjit = Max_Jitter * ((FRAND() * 2.0) - 1.0);

  xlen = r * (Sample_Grid[ray_number].x + xjit);
  ylen = r * (Sample_Grid[ray_number].y + yjit);

  /*
   * Deflect the position of the eye by the size of the aperture, and in
   * a direction perpendicular to the current direction of view.
   */

  VScale(temp_xperp, XPerp, xlen);
  VScale(temp_yperp, YPerp, ylen);

  VSub(deflection, temp_xperp, temp_yperp);

  VAdd(ray->Initial, Frame.Camera->Location, deflection);

  /*
   * Deflect the direction of the ray in the opposite direction we deflected
   * the eye position.  This makes sure that we are looking at the same place
   * when the distance from the eye is equal to "Focal_Distance".
   */

  VScale(ray->Direction, ray->Direction, Focal_Distance);
  VSub(ray->Direction, ray->Direction, deflection);

  VNormalize(ray->Direction, ray->Direction);
}



/*****************************************************************************
*
* FUNCTION
*
*   trace_pixel
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
*   Trace a primary ray regarding focal blur and vista buffer.
*   The color of the pixel is clipped and the number of pixels is increased.
*
* CHANGES
*
*   Sep 1994 : Extracted common code. [DB]
*   Jan 1995 : Added call to accumulate_histogram() - Chris Cason
*
******************************************************************************/

static void trace_pixel(int x, int  y, COLOUR Colour)
{
  Increase_Counter(stats[Number_Of_Pixels]);

  Trace_Level = 1;

  POV_PRE_PIXEL (x, y, Colour)

  COOPERATE_0

  /* Do histogram stuff. */
  if (opts.histogram_on)
  {
    accumulate_histogram(x, y, TRUE);
  }

  if (Focal_Blur_Is_Used)
  {
    /* Use focal blur tracing. */

    focal_blur(&Camera_Ray, Colour, (DBL)x, (DBL)y);
  }
  else
  {
    /* Create and trace ray. */

    if (create_ray(&Camera_Ray, (DBL)x, (DBL)y, 0))
    {
      Increase_Counter(stats[Number_Of_Samples]);

      if (opts.Options & USE_VISTA_BUFFER)
      {
        Trace_Primary_Ray(&Camera_Ray, Colour, 1.0, x);
      }
      else
      {
        Trace(&Camera_Ray, Colour, 1.0);
      }
    }
    else
    {
      Make_ColourA(Colour, 0.0, 0.0, 0.0, 0.0, 1.0);
    }
  }

  Clip_Colour(Colour, Colour);

  gamma_correct(Colour);

  /* Do histogram stuff. */
  if (opts.histogram_on)
  {
    accumulate_histogram(x, y, FALSE);
  }

  POV_POST_PIXEL (x, y, Colour)
}


/*****************************************************************************
*
* FUNCTION
*
*   create_ray
*
* INPUT
*
*   ray        - Primary ray for given screen point
*   x, y       - Coordinates of current pixel
*   ray_number - number of ray used by jitter_camera_ray()
*   
* OUTPUT
*
*   ray
*   
* RETURNS
*
*   int - TRUE, if a ray was created
*   
* AUTHOR
*
*   Dieter Bayer
*   Dan Farmer (focal blur, 'ultra wide angle camera')
*   
* DESCRIPTION
*
*   Create a primary ray depending on the camera model used.
*
*   Ideas for the camera models were taken from:
*
*     Geoff Wyvill and Craig McNaughton, "Optical Models",
*     CG International '90, Springer, 1990, p. 83-93
*
*     F. Kenton Musgrave, "A panoramic virtual screen for ray tracing",
*     Graphics Gems III, David Kirk (eds.), p. 288-294
*
* CHANGES
*
*   May 1994 : Creation.
*
*   Aug 1996 : Removed unnecessary width/height parameters. [DB]
*
******************************************************************************/

static int create_ray(RAY *Ray, DBL x, DBL  y, int ray_number)
{
  /* Just some shortcuts. */

#define FCD Frame.Camera->Direction
#define FCR Frame.Camera->Right
#define FCU Frame.Camera->Up
#define FCL Frame.Camera->Location

  DBL x0 = 0.0, y0 = 0.0;
  DBL cx, sx, cy, sy, ty, rad, phi, lx, ly;
  VECTOR V1;

  /* Create primary ray according to the camera used. */

  Assign_Vector(Ray->Initial, FCL);

  Initialize_Ray_Containers(Ray);

  switch (Frame.Camera->Type)
  {
    /*
     * Perspective projection (Pinhole camera; POV standard).
     */

    case PERSPECTIVE_CAMERA:

      /* Convert the x coordinate to be a DBL from -0.5 to 0.5. */

      x0 = x / (DBL)Frame.Screen_Width - 0.5;

      /* Convert the y coordinate to be a DBL from -0.5 to 0.5. */

      y0 = ((DBL)(Frame.Screen_Height - 1) - y) / (DBL)Frame.Screen_Height - 0.5;

      /* Create primary ray. */

      VLinComb3(Ray->Direction, 1.0, FCD, x0, FCR, y0, FCU);

      /* Do focal blurring (by Dan Farmer). */

      if (Focal_Blur_Is_Used)
      {
        jitter_camera_ray(Ray, ray_number);

        initialize_ray_container_state(Ray, TRUE);
      }
      else
      {
        initialize_ray_container_state(Ray, Precompute_Camera_Constants);

        Precompute_Camera_Constants = FALSE;
      }

    break;

    /*
     * Orthographic projection.
     */

    case ORTHOGRAPHIC_CAMERA:

      /* Convert the x coordinate to be a DBL from -0.5 to 0.5. */

      x0 = x / (DBL)Frame.Screen_Width - 0.5;

      /* Convert the y coordinate to be a DBL from -0.5 to 0.5. */

      y0 = ((DBL)(Frame.Screen_Height - 1) - y) / (DBL)Frame.Screen_Height - 0.5;

      /* Create primary ray. */

      Assign_Vector(Ray->Direction, FCD);

      VLinComb3(Ray->Initial, 1.0, FCL, x0, FCR, y0, FCU);

      initialize_ray_container_state(Ray, TRUE);

      break;

    /*
     * Fisheye camera.
     */

    case FISHEYE_CAMERA:

      /* Convert the x coordinate to be a DBL from -1.0 to 1.0. */

      x0 = 2.0 * x / (DBL)Frame.Screen_Width - 1.0;

      /* Convert the y coordinate to be a DBL from -1.0 to 1.0. */

      y0 = 2.0 * ((DBL)(Frame.Screen_Height - 1) - y) / (DBL)Frame.Screen_Height - 1.0;

      /* Get aspect ratio --> we want a circle (do this only once). */

      if (Precompute_Camera_Constants)
      {
        VLength(lx, FCR);
        VLength(ly, FCU);

        Camera_Aspect_Ratio = lx / ly;

        VNormalize(FCR, FCR);
        VNormalize(FCU, FCU);
        VNormalize(FCD, FCD);
      }

      /* Get polar coordinates. */

      x0 *= Camera_Aspect_Ratio;

      rad = sqrt(x0 * x0 + y0 * y0);

      /* If the pixel lies outside the unit circle no ray is traced. */

      if (rad > 1.0)
      {
        return(FALSE);
      }

      if (rad == 0.0)
      {
        phi = 0.0;
      }
      else
      {
        if (x0 < 0.0)
        {
          phi = M_PI - asin(y0 / rad);
        }
        else
        {
          phi = asin(y0 / rad);
        }
      }

      /* Get spherical coordinates. */

      x0 = phi;

      /* Set vertical angle to half viewing angle. */

      y0 = rad * Frame.Camera->Angle * M_PI_360;

      /* Create primary ray. */

      cx = cos(x0);  sx = sin(x0);
      cy = cos(y0);  sy = sin(y0);

      VLinComb3(Ray->Direction, cx * sy, FCR, sx * sy, FCU, cy, FCD);

      initialize_ray_container_state(Ray, Precompute_Camera_Constants);

      Precompute_Camera_Constants = FALSE;

      break;

    /*
     * Omnimax camera.
     */

    case OMNIMAX_CAMERA:

      /* Convert the x coordinate to be a DBL from -1.0 to 1.0. */

      x0 = 2.0 * x / (DBL)Frame.Screen_Width - 1.0;

      /* Convert the y coordinate to be a DBL from -1.0 to 1.0. */

      y0 = 2.0 * ((DBL)(Frame.Screen_Height - 1) - y) / (DBL)Frame.Screen_Height - 1.0;

      /* Get aspect ratio --> we want a circle (do this only once). */

      if (Precompute_Camera_Constants)
      {
        VLength(lx, FCR);
        VLength(ly, FCU);

        Camera_Aspect_Ratio = lx / ly;

        VNormalize(FCR, FCR);
        VNormalize(FCU, FCU);
        VNormalize(FCD, FCD);
      }

      /* Get polar coordinates. */

      x0 *= Camera_Aspect_Ratio;

      rad = sqrt(x0 * x0 + y0 * y0);

      /* If the pixel lies outside the unit circle no ray is traced. */

      if (rad > 1.0)
      {
        return(FALSE);
      }

      if (rad == 0.0)
      {
        phi = 0.0;
      }
      else
      {
        if (x0 < 0.0)
        {
          phi = M_PI - asin(y0 / rad);
        }
        else
        {
          phi = asin(y0 / rad);
        }
      }

      /* Get spherical coordinates. */

      x0 = phi;

      y0 = 1.411269 * rad - 0.09439 * rad * rad * rad + 0.25674 * rad * rad * rad * rad * rad;

      cx = cos(x0);  sx = sin(x0);
      cy = cos(y0);  sy = sin(y0);

      /* We can't see below 45 degrees under the projection axis. */

      if (sx * sy < tan(135.0 * M_PI_180) * cy)
      {
        return(FALSE);
      }

      /* Create primary ray. */

      VLinComb3(Ray->Direction, cx * sy, FCR, sx * sy, FCU, cy, FCD);

      initialize_ray_container_state(Ray, Precompute_Camera_Constants);

      Precompute_Camera_Constants = FALSE;

      break;

    /*
     * Panoramic camera from Graphic Gems III.
     */

    case PANORAMIC_CAMERA:

      /* Convert the x coordinate to be a DBL from 0.0 to 1.0. */

      x0 = x / (DBL)Frame.Screen_Width;

      /* Convert the y coordinate to be a DBL from -1.0 to 1.0. */

      y0 = 2.0 * ((DBL)(Frame.Screen_Height - 1) - y) / (DBL)Frame.Screen_Height - 1.0;

      /* Get cylindrical coordinates. */

      x0 = (1.0 - x0) * M_PI;

      y0 = M_PI_2 * y0;

      cx = cos(x0);
      sx = sin(x0);

      if (fabs(M_PI_2 - fabs(y0)) < EPSILON)
      {
        if (y0 > 0.0)
        {
          ty = BOUND_HUGE;
        }
        else
        {
          ty = - BOUND_HUGE;
        }
      }
      else
      {
        ty = tan(y0);
      }

      /* Create primary ray. */

      VLinComb3(Ray->Direction, cx, FCR, ty, FCU, sx, FCD);

      initialize_ray_container_state(Ray, Precompute_Camera_Constants);

      Precompute_Camera_Constants = FALSE;

      break;

    /*
     * Ultra wide angle camera written by Dan Farmer.
     */

    case ULTRA_WIDE_ANGLE_CAMERA:

      /* Convert the x coordinate to be a DBL from -0.5 to 0.5. */

      x0 = x / (DBL)Frame.Screen_Width - 0.5;

      /* Convert the y coordinate to be a DBL from -0.5 to 0.5. */

      y0 = ((DBL)(Frame.Screen_Height - 1) - y) / (DBL)Frame.Screen_Height - 0.5;

      /* Create primary ray. */

      x0 *= Frame.Camera->Angle / 180.0;

      y0 *= Frame.Camera->Angle / 180.0;

      cx = cos(x0);  sx = sin(x0);
      cy = cos(y0);  sy = sin(y0);

      VLinComb3(Ray->Direction, sx, FCR, sy, FCU, cx * cy, FCD);

      initialize_ray_container_state(Ray, Precompute_Camera_Constants);

      Precompute_Camera_Constants = FALSE;

      break;

    /*
     * Cylinder camera 1. Axis in "up" direction
     */

    case CYL_1_CAMERA:

      /* Convert the x coordinate to be a DBL from -0.5 to 0.5. */

      x0 = x / (DBL)Frame.Screen_Width - 0.5;

      /* Convert the y coordinate to be a DBL from -0.5 to 0.5. */

      y0 = ((DBL)(Frame.Screen_Height - 1) - y) / (DBL)Frame.Screen_Height - 0.5;

      /* Create primary ray. */

      x0 *= Frame.Camera->Angle * M_PI_180;

      cx = cos(x0);
      sx = sin(x0);

      VLinComb3(Ray->Direction, sx, FCR, y0, FCU, cx, FCD);

      initialize_ray_container_state(Ray, Precompute_Camera_Constants);

      Precompute_Camera_Constants = FALSE;

      break;

    /*
     * Cylinder camera 2. Axis in "right" direction
     */

    case CYL_2_CAMERA:

      /* Convert the x coordinate to be a DBL from -0.5 to 0.5. */

      x0 = x / (DBL)Frame.Screen_Width - 0.5;

      /* Convert the y coordinate to be a DBL from -0.5 to 0.5. */

      y0 = ((DBL)(Frame.Screen_Height - 1) - y) / (DBL)Frame.Screen_Height - 0.5;

      /* Create primary ray. */

      y0 *= Frame.Camera->Angle * M_PI_180;

      cy = cos(y0);
      sy = sin(y0);

      VLinComb3(Ray->Direction, x0, FCR, sy, FCU, cy, FCD);

      initialize_ray_container_state(Ray, Precompute_Camera_Constants);

      Precompute_Camera_Constants = FALSE;

      break;

    /*
     * Cylinder camera 3. Axis in "up" direction, orthogonal in "right"
     */

    case CYL_3_CAMERA:

      /* Convert the x coordinate to be a DBL from -0.5 to 0.5. */

      x0 = x / (DBL)Frame.Screen_Width - 0.5;

      /* Convert the y coordinate to be a DBL from -0.5 to 0.5. */

      y0 = ((DBL)(Frame.Screen_Height - 1) - y) / (DBL)Frame.Screen_Height - 0.5;

      /* Create primary ray. */

      x0 *= Frame.Camera->Angle * M_PI_180;

      cx = cos(x0);
      sx = sin(x0);

      VLinComb2(Ray->Direction, sx, FCR, cx, FCD);

      VLinComb2(Ray->Initial, 1.0, FCL, y0, FCU);

      initialize_ray_container_state(Ray, TRUE);

      break;

    /*
     * Cylinder camera 4. Axis in "right" direction, orthogonal in "up"
     */

    case CYL_4_CAMERA:

      /* Convert the x coordinate to be a DBL from -0.5 to 0.5. */

      x0 = x / (DBL)Frame.Screen_Width - 0.5;

      /* Convert the y coordinate to be a DBL from -0.5 to 0.5. */

      y0 = ((DBL)(Frame.Screen_Height - 1) - y) / (DBL)Frame.Screen_Height - 0.5;

      /* Create primary ray. */

      y0 *= Frame.Camera->Angle * M_PI_180;

      cy = cos(y0);
      sy = sin(y0);

      VLinComb2(Ray->Direction, sy, FCU, cy, FCD);

      VLinComb2(Ray->Initial, 1.0, FCL, x0, FCR);

      initialize_ray_container_state(Ray, TRUE);

      break;

    default:

    Error("Unknown camera type in create_ray().\n");
  }

  if (Frame.Camera->Tnormal != NULL)
  {
    VNormalize(Ray->Direction, Ray->Direction);

    Make_Vector(V1, x0, y0, 0.0);
    
    Perturb_Normal(Ray->Direction, Frame.Camera->Tnormal, V1);
  }
  
  VNormalize(Ray->Direction, Ray->Direction);
  
  return(TRUE);

#undef FCD
#undef FCR
#undef FCU
#undef FCL
}



/*****************************************************************************
*
* FUNCTION
*
*   gamma_correct
*
* INPUT
*
*   Colour (an array of DBL's)
*   
* OUTPUT
*
*   The colour array
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Adjust RGB values for overall brightness and gamma curve so that the
*   image appears a constant brightness regardless of the hardware that
*   is being used.  This can't be part of extract_colors, since
*   extract_colors is used multiple times in anti aliasing, and this has
*   to be called exactly once per pixel output.
*
*   If gamma correction is enabled, then file and display pixel values 
*   will be corrected for the current assumed_gamma, which has default
*   value 1.00, but can be set by a global_settings {assumed_gamma} in the
*   source file, and the DisplayGamma value, which can be set in the INI
*   file via Display_Gamma=n.n or defaults to DEFAULT_DISPLAY_GAMMA
*   (2.2 unless another value set in the system specific config.h files.)
*
*   If gamma correction is turned off (if no global_settings {assumed_gamma}
*   line occurrs in the scene file) then no gamma correction will be done 
*   on the pixel values.  If gamma correction is turned off, the 
*   DisplayGamma value is still relevant for PNG output files, since if 
*   gamma correction is disabled, it is presumed that the scene is 
*   "pre-corrected" for the current hardware, and a gAMA chunk with the 
*   current 1/DisplayGamma value will be output.
*
*   When DisplayGamma approximately equals assumed_gamma, it means that we
*   don't need to do any gamma correction since it is already the correct
*   brightness.  opts.GammaFactor is calculated once in tokenize.c to
*   be assumed_gamma/DisplayGamma to avoid re-calculation for each pixel.
*
* CHANGES
*
*   Apr 1995 :  Created function - Jim McElhiney
*   Oct 1995 :  Modified to do proper system gamma correction [AED]
*
******************************************************************************/

static void gamma_correct(COLOUR Colour)
{
  if (opts.Options & GAMMA_CORRECT)
  {
    Colour[RED]   = pow(Colour[RED],  opts.GammaFactor);
    Colour[GREEN] = pow(Colour[GREEN],opts.GammaFactor);
    Colour[BLUE]  = pow(Colour[BLUE], opts.GammaFactor);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   extract_colors
*
* INPUT
*
*   Colour, Red, Green, Blue, Alpha, grey
*   
* OUTPUT
*
*   Red, Green, Blue, Alpha, grey
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Create appropriate rgba values.
*
* CHANGES
*
*   Aug 1994 : Extracted common code from Start_Tracing - Eduard Schwan
*   Jun 1995 : Alpha channel support -CEY
*
******************************************************************************/

static void extract_colors(COLOUR Colour, unsigned char *Red, unsigned char  *Green, unsigned char  *Blue, unsigned char  *Alpha, DBL *grey)
{
  if (opts.PaletteOption == GREY)
  {
    *grey = Colour[RED] * 0.287 + Colour[GREEN] * 0.589 + Colour[BLUE] * 0.114;

    *Red = *Green = *Blue = (unsigned char)((*grey) * maxclr);
  }
  else
  {
    *Red   = (unsigned char)(Colour[RED]    * maxclr);
    *Green = (unsigned char)(Colour[GREEN]  * maxclr);
    *Blue  = (unsigned char)(Colour[BLUE]   * maxclr);
    *Alpha = (unsigned char)(Colour[TRANSM] * maxclr);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   plot_pixel
*
* INPUT
*
*   x, y   - pixel location
*   Colour - pixel color
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
*   Display a pixel on the screen.
*
* CHANGES
*
*   Aug 1995 : Creation. Extracted from common code.
*
******************************************************************************/

static void plot_pixel(int x, int  y, COLOUR Colour)
{
  unsigned char Red, Green, Blue, Alpha;
  DBL grey;

  if ((opts.Options & DISPLAY) && (y != opts.First_Line - 1))
  {
    extract_colors(Colour, &Red, &Green, &Blue, &Alpha, &grey);

    POV_DISPLAY_PLOT(x, y, Red, Green, Blue, Alpha);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   jitter_pixel_position
*
* INPUT
*
*   x, y     - pixel location
*   
* OUTPUT
*
*   Jitter_X - pseudo-random offset to x-coordinate
*   Jitter_Y - pseudo-random offset to y-coordinate
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Apply jitter to a pixel location.
*
* CHANGES
*
*   Aug 1995 : Creation. Extracted from common code.
*
******************************************************************************/

static void jitter_pixel_position(int x, int  y, DBL *Jitter_X, DBL  *Jitter_Y)
{
  static int Jitt_Offset = 10;

  if (opts.Options & JITTER)
  {
    *Jitter_X = rand2d(x + Jitt_Offset, y);

    Jitt_Offset++;

    *Jitter_Y = rand2d(x + Jitt_Offset, y);

    Jitt_Offset++;

    *Jitter_X *= JitterScale;
    *Jitter_Y *= JitterScale;
  }
  else
  {
    *Jitter_X = *Jitter_Y = 0.0;
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   output_line
*
* INPUT
*
*   y              - current line number
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

static void output_line(int y)
{
  COLOUR *Temp_Colour_Ptr;
  char *Temp_Char_Ptr;

  if (y > opts.First_Line)
  {
    if (opts.Options & DISKWRITE)
    {
      Write_Line(Output_File_Handle, Previous_Line, y - 1);
    }
    POV_WRITE_LINE (Previous_Line, y - 1)
  }

  if (opts.Options & VERBOSE)
  {
    if (opts.Options & ANTIALIAS)
    {
      if (opts.Options & RADIOSITY)
      {
        Status_Info(" supersampled %d, %ld radiosity.",
                    SuperSampleCount, ra_gather_count - RadiosityCount);
      }
      else
      {
        Status_Info(" supersampled %d times.", SuperSampleCount);
      }
    }
    else
    {
      if (opts.Options & RADIOSITY)
      {
        Status_Info("  %ld radiosity samples.", ra_gather_count - RadiosityCount);
      }
    }

    Status_Info("       \r");
  }

  Temp_Colour_Ptr = Previous_Line;
  Previous_Line   = Current_Line;
  Current_Line    = Temp_Colour_Ptr;

  Temp_Char_Ptr                   = Previous_Line_Antialiased_Flags;
  Previous_Line_Antialiased_Flags = Current_Line_Antialiased_Flags;
  Current_Line_Antialiased_Flags  = Temp_Char_Ptr;
}

/*****************************************************************************
*
* FUNCTION
*
*   initialise_histogram
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Chris Cason
*
* DESCRIPTION
*
* Initialise histogram collation if available, otherwise force histogram off.
*
* CHANGES
*
******************************************************************************/

static void initialise_histogram ()
{
#if PRECISION_TIMER_AVAILABLE
  if (opts.histogram_on)
  {
    PRECISION_TIMER_INIT
    histogram_grid = (unsigned long *)POV_CALLOC (opts.histogram_x * opts.histogram_y,
                                 sizeof(unsigned long), "histogram grid");
  }
#else
  opts.histogram_on = FALSE ;
#endif
}

/*****************************************************************************
*
* FUNCTION
*
*   destroy_histogram
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Andreas Dilger
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void destroy_histogram ()
{
#if PRECISION_TIMER_AVAILABLE
  if (Histogram_File_Handle != NULL)
  {
    Close_File(Histogram_File_Handle);
    POV_FREE(Histogram_File_Handle);
    Histogram_File_Handle = NULL;
  }

  if (histogram_grid != NULL)
  {
    POV_FREE (histogram_grid) ;
    histogram_grid = NULL;
  }
#endif
}

/*****************************************************************************
*
* FUNCTION
*
*   accumulate_histogram
*
* INPUT
*
*   int x, y - pixel position on screen, on - starting pixel
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Chris Cason
*   
* DESCRIPTION
*
* Accumulate statistics on the histogram grid given on the command line.
* The total amount of time that the program spends in tracing pixels in the
* particular grid section that the pixel lies in is eventually written to an
* output file.
*
* CHANGES
*
* Oct 1995:  Modified to return if PRECISION_TIMER_AVAILABLE == 0 [CJC]
*
******************************************************************************/

static void accumulate_histogram(int x, int  y, int  on)
{
#if PRECISION_TIMER_AVAILABLE
  int loc_x, loc_y;
  unsigned long  *p;

  if (y < 0)
  {
    return;
  }

  if (!on)
  {
    PRECISION_TIMER_STOP

    /* determine which grid section the pixel lies in */

    loc_x = x * opts.histogram_x / Frame.Screen_Width;
    loc_y = y * opts.histogram_y / Frame.Screen_Height;

    p = histogram_grid + (long) loc_y * opts.histogram_x + loc_x;

    *p += PRECISION_TIMER_COUNT;

    if (*p > max_histogram_value)
    {
      max_histogram_value = *p;
    }
  }
  else
  {
    PRECISION_TIMER_START
  }
#endif
}

/*****************************************************************************
*
* FUNCTION
*
*   write_histogram
*
* INPUT
*
*   filename - file to output the data to.
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Chris Cason
*   
* DESCRIPTION
*
* Processes and then writes the accumulated histogram statistics to a CSV
* (comma separated value) file. Programs such as Microsoft Excel can read CSV.
* Also accomodates image file output, such as Targa, for height field usage.
*
* CHANGES
*
* Sep 1995:  Added support for grayscale PNG histogram output [AED]
* Oct 1995:  Fixed TGA, PPM output for 0 - 1 range and use HF_GRAY_16 [AED]
* Oct 1995:  Modified to return if PRECISION_TIMER_AVAILABLE == 0 [CJC]
*
******************************************************************************/

void write_histogram(char *filename)
{
#if PRECISION_TIMER_AVAILABLE
  int x, y, x_step, y_step;
  unsigned long OptionsSave;
  int OutputQualitySave;
  unsigned long *p;
  FILE *f;
  COLOUR *line, *l;

  Status_Info ("\nWriting histogram file '%s'", filename);

  switch (opts.histogram_type)
  {
    case CSV :

      if ((f = fopen (filename, WRITE_TXTFILE_STRING)) == NULL)
      {
        Warning (0.0, "Cannot open file %s for histogram output.", filename);

        return;
      }

      for (y = 0, p = histogram_grid; y < opts.histogram_y ; y++)
      {
        for (x = 0; x < opts.histogram_x ; x++)
        {
             fprintf (f, "%s%010lu", x ? ", " : "", *p++);
        }

        fprintf (f, "\n");
      }

      fclose (f);

      break;

    case PPM :
    case SYS :
    case TARGA :
    case PNG:

      OptionsSave = opts.Options;
      OutputQualitySave = opts.OutputQuality;

      opts.Options = HF_GRAY_16;
      opts.OutputQuality = 12;

      Histogram_File_Handle->file_type = HIST_FTYPE;

      if (Open_File (Histogram_File_Handle, filename,
          &Frame.Screen_Width, &Frame.Screen_Height, 0, WRITE_MODE) != 1)
      {
        Warning (0.0, "Cannot open file %s for histogram output.", filename);
        return;
      }

      line = (COLOUR *)POV_CALLOC(Frame.Screen_Width, sizeof(COLOUR), "histogram buffer");

      y_step = Frame.Screen_Height / opts.histogram_y;
      x_step = Frame.Screen_Width / opts.histogram_x;

      for (y = 0; y < Frame.Screen_Height ; y++)
      {
        p = histogram_grid + (long) (y / y_step) * opts.histogram_x;

        for (x = 0, l = line; x < Frame.Screen_Width ; x++, l++)
        {
          (*l) [RED] =
          (*l) [GREEN] =
          (*l) [BLUE] = (COLC)*(p + x / x_step) / max_histogram_value;
        }
        Write_Line (Histogram_File_Handle, line, y);
      }

      POV_FREE (line);

      Close_File (Histogram_File_Handle);

      POV_FREE (Histogram_File_Handle);

      Histogram_File_Handle=NULL;

      opts.Options = OptionsSave;
      opts.OutputQuality = OutputQualitySave;

      break;

    case NONE:  /* Just to quiet warnings */
    default:
      break;
  }
#endif
}

/*****************************************************************************
*
* FUNCTION
*
*   check_stats
*
* INPUT
*   y = current row
*   doingMosaic = 1 if doing mosaic preview (show pixWidth)
*   pixWidth = size of pixel
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
*   6/17/95 esp Added display of mosaic pixWidth
*
******************************************************************************/

static void check_stats(register int y, register int doingMosaic, register int pixWidth)
{
  DBL time_dif;
  unsigned long hrs, mins;
  DBL secs;

  /* New verbose options CdW */

  if (opts.Options & VERBOSE)
  {
    STOP_TIME

    time_dif = TIME_ELAPSED

    if (time_dif > 0.0)
    {
      SPLIT_TIME(time_dif, &hrs, &mins, &secs);

      Status_Info("%3ld:%02ld:%02ld ", hrs, mins, (long)secs);
    }
    else
    {
      Status_Info("  -:--:-- ");
    }

    Status_Info("Rendering line %4d of %4d", y-opts.First_Line+1, opts.Last_Line-opts.First_Line);

    if (doingMosaic)
    {
      Status_Info(" at %dx%d  \b\b", pixWidth, pixWidth);
    }

    if (opts.Options & ANTIALIAS)
    {
      SuperSampleCount = 0;
    }
    else
    {
      Status_Info(". ");
    }

    RadiosityCount = ra_gather_count;
  }
}


/*****************************************************************************
*
* FUNCTION
*
*   initialize_ray_container_state
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
*   Initialize the containing lists of a primary ray by testing wether
*   the ray's starting point is inside any object. All objects that the
*   ray's origin is currently inside will be added to the containing
*   lists.
*
*   This is neccessary to make participating media work if the camera 
*   is inside the medium. It is also neccessary to make refraction work 
*   correctly if the camera is inside this object.
*
* CHANGES
*
*   Mar 1996 : Creation.
*
******************************************************************************/

static void initialize_ray_container_state(RAY *Ray, int Compute)
{
  int i, solid;
  OBJECT *Object;

  if (Compute)
  {
    Containing_Index = -1;

    if (!opts.Use_Slabs)
    {
      for (Object = Frame.Objects; Object != NULL; Object = Object -> Sibling)
      {
        if (Inside(Ray->Initial, Object) && (Object->Interior != NULL))
        {
          if ((++Containing_Index) >= MAX_CONTAINING_OBJECTS)
          {
             Error("Too many nested objects\n");
          }

          Containing_Interiors[Containing_Index] = Object->Interior;
        }
      }
    }
    else
    {
      initialize_ray_container_state_tree(Ray, Root_Object);
    }
  }

  for (i = 0; i <= Containing_Index; i++)
  {
    Ray->Interiors[i] = Containing_Interiors[i];
  }

  Ray->Index = Containing_Index;

  /* Test if we are in a hollow object (do this only once). */

  if (!Primary_Ray_State_Tested)
  {
    solid = FALSE;

    for (i = 0; i <= Containing_Index; i++)
    {
      if (!Containing_Interiors[i]->hollow)
      {
        solid = TRUE;

        break;
      }
    }

    if (solid)
    {
      Warning(0.0, "Camera is inside a non-hollow object.\nFog and participating media may not work as expected.\n");
    }

    Primary_Ray_State_Tested = TRUE;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   initialize_ray_container_state_tree
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
*   Step down the bounding box hierarchy and test for all node wether
*   the ray's origin is inside or not. If it's inside a node descend
*   further. If a leaf is reached and the ray's origin is inside the
*   leaf object insert the objects data into the ray's containing lists.
*
* CHANGES
*
*   Mar 1996 : Creation.
*
******************************************************************************/

static void initialize_ray_container_state_tree(RAY *Ray, BBOX_TREE *Node)
{
  int i;
  OBJECT *Object;

  /* Check current node. */

  if ((Ray->Initial[X] < (DBL)Node->BBox.Lower_Left[X]) ||
      (Ray->Initial[Y] < (DBL)Node->BBox.Lower_Left[Y]) ||
      (Ray->Initial[Z] < (DBL)Node->BBox.Lower_Left[Z]) ||
      (Ray->Initial[X] > (DBL)Node->BBox.Lower_Left[X] + (DBL)Node->BBox.Lengths[X]) ||
      (Ray->Initial[Y] > (DBL)Node->BBox.Lower_Left[Y] + (DBL)Node->BBox.Lengths[Y]) ||
      (Ray->Initial[Z] > (DBL)Node->BBox.Lower_Left[Z] + (DBL)Node->BBox.Lengths[Z]))
  {
    return;
  }

  if (Node->Entries)
  {
    /* This is a node containing leaves to be checked. */

    for (i = 0; i < Node->Entries; i++)
    {
      initialize_ray_container_state_tree(Ray, Node->Node[i]);
    }
  }
  else
  {
    /* This is a leaf so test contained object. */

    Object = (OBJECT *)Node->Node;

    if (Inside(Ray->Initial, Object) && (Object->Interior != NULL))
    {
      if ((++Containing_Index) >= MAX_CONTAINING_OBJECTS)
      {
        Error("Too many nested objects\n");
      }

      Containing_Interiors[Containing_Index] = Object->Interior;
    }
  }
}

