/****************************************************************************
*                   povray.h
*
*  This module contains all defines, typedefs, and prototypes for POVRAY.C.
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


#ifndef POVRAY_H
#define POVRAY_H

#include <time.h>

#include "atmosph.h"
#include "camera.h"
#include "media.h"
#include "point.h"
#include "render.h"


/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define MAX_LIBRARIES 25

#define STAGE_STARTUP         0  /* set in POVRAY.C */
#define STAGE_BANNER          1  /* set in POVRAY.C */
#define STAGE_INIT            2  /* set in POVRAY.C */
#define STAGE_ENVIRONMENT     3  /* set in POVRAY.C */
#define STAGE_COMMAND_LINE    4  /* set in POVRAY.C */
#define STAGE_FILE_INIT       5  /* set in POVRAY.C */
#define STAGE_PARSING         7  /* set in PARSE.C */
#define STAGE_CONTINUING      8  /* set in POVRAY.C */
#define STAGE_RENDERING       9  /* set in POVRAY.C */
#define STAGE_SHUTDOWN       10  /* set in POVRAY.C */
#define STAGE_INI_FILE       11  /* set in POVRAY.C */
#define STAGE_CLEANUP_PARSE  12  /* set in PARSE.C */
#define STAGE_SLAB_BUILDING  13  /* set in POVRAY.C */
#define STAGE_TOKEN_INIT     14  /* set in TOKENIZE.C */
#define STAGE_INCLUDE_ERR    15  /* set in TOKENIZE.C */
#define STAGE_FOUND_INSTEAD  16  /* set in TOKENIZE.C */

#define DISPLAY           0x000001L
#define VERBOSE           0x000002L
#define DISKWRITE         0x000004L
#define PROMPTEXIT        0x000008L
#define ANTIALIAS         0x000010L
#define RGBSEPARATE       0x000020L
#define EXITENABLE        0x000040L
#define CONTINUE_TRACE    0x000080L
#define BUFFERED_OUTPUT   0x000100L
#define JITTER            0x000200L
#define PREVIEW           0x000400L
#define SPLIT_UNION       0x000800L
#define USE_VISTA_BUFFER  0x001000L
#define USE_LIGHT_BUFFER  0x002000L
#define USE_VISTA_DRAW    0x004000L
#define REMOVE_BOUNDS     0x008000L
#define CYCLIC_ANIMATION  0x010000L
#define OUTPUT_ALPHA      0x020000L
#define HF_GRAY_16        0x040000L
#define GAMMA_CORRECT     0x080000L
#define RADIOSITY         0x100000L
#define FROM_STDIN        0x200000L
#define TO_STDOUT         0x400000L

#define Q_FULL_AMBIENT 0x000001L
#define Q_QUICKC       0x000002L
#define Q_SHADOW       0x000004L
#define Q_AREA_LIGHT   0x000008L
#define Q_REFRACT      0x000010L
#define Q_REFLECT      0x000020L
#define Q_NORMAL       0x000040L
#define Q_VOLUME       0x000080L

#define EF_RADIOS  1

#define QUALITY_0  Q_QUICKC+Q_FULL_AMBIENT
#define QUALITY_1  QUALITY_0
#define QUALITY_2  QUALITY_1-Q_FULL_AMBIENT
#define QUALITY_3  QUALITY_2
#define QUALITY_4  QUALITY_3+Q_SHADOW
#define QUALITY_5  QUALITY_4+Q_AREA_LIGHT
#define QUALITY_6  QUALITY_5-Q_QUICKC+Q_REFRACT
#define QUALITY_7  QUALITY_6
#define QUALITY_8  QUALITY_7+Q_REFLECT+Q_NORMAL
#define QUALITY_9  QUALITY_8+Q_VOLUME


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Frame_Struct FRAME;

struct Frame_Struct
{
  CAMERA *Camera;
  int Screen_Height, Screen_Width; /* OPTIONS */
  int Number_Of_Light_Sources;
  LIGHT_SOURCE *Light_Sources;
  OBJECT *Objects;
  DBL Atmosphere_IOR, Antialias_Threshold;
  COLOUR Background_Colour;
  COLOUR Ambient_Light;
  COLOUR Irid_Wavelengths;
  IMEDIA *Atmosphere;
  FOG *Fog;
  RAINBOW *Rainbow;
  SKYSPHERE *Skysphere;
};

typedef enum STATS
{
  /* Computations are performed on these three */
  Number_Of_Pixels = 0,
  Number_Of_Pixels_Supersampled,
  Number_Of_Samples,
  Number_Of_Rays,
  Calls_To_DNoise,
  Calls_To_Noise,
  ADC_Saves,

  /* objects */
  Bounding_Region_Tests,
  Bounding_Region_Tests_Succeeded,
  Clipping_Region_Tests,
  Clipping_Region_Tests_Succeeded,
  Istack_overflows,

  Ray_Bicubic_Tests,
  Ray_Bicubic_Tests_Succeeded,
  Ray_Blob_Tests,
  Ray_Blob_Tests_Succeeded,
  Blob_Element_Tests,
  Blob_Element_Tests_Succeeded,
  Blob_Bound_Tests,
  Blob_Bound_Tests_Succeeded,
  Ray_Box_Tests,
  Ray_Box_Tests_Succeeded,
  Ray_Cone_Tests,
  Ray_Cone_Tests_Succeeded,
  Ray_Disc_Tests,
  Ray_Disc_Tests_Succeeded,
  Ray_HField_Tests,
  Ray_HField_Tests_Succeeded,
  Ray_HField_Box_Tests,
  Ray_HField_Box_Tests_Succeeded,
  Ray_HField_Triangle_Tests,
  Ray_HField_Triangle_Tests_Succeeded,
  Ray_HField_Block_Tests,
  Ray_HField_Block_Tests_Succeeded,
  Ray_HField_Cell_Tests,
  Ray_HField_Cell_Tests_Succeeded,
  Ray_Fractal_Tests,
  Ray_Fractal_Tests_Succeeded,
  Ray_Lathe_Tests,
  Ray_Lathe_Tests_Succeeded,
  Lathe_Bound_Tests,
  Lathe_Bound_Tests_Succeeded,
  Ray_Mesh_Tests,
  Ray_Mesh_Tests_Succeeded,
  Ray_Plane_Tests,
  Ray_Plane_Tests_Succeeded,
  Ray_Poly_Tests,
  Ray_Poly_Tests_Succeeded,
  Ray_Polygon_Tests,
  Ray_Polygon_Tests_Succeeded,
  Ray_Prism_Tests,
  Ray_Prism_Tests_Succeeded,
  Prism_Bound_Tests,
  Prism_Bound_Tests_Succeeded,
  Ray_Quadric_Tests,
  Ray_Quadric_Tests_Succeeded,
  Ray_Sor_Tests,
  Ray_Sor_Tests_Succeeded,
  Sor_Bound_Tests,
  Sor_Bound_Tests_Succeeded,
  Ray_Sphere_Tests,
  Ray_Sphere_Tests_Succeeded,
  Ray_Torus_Tests,
  Ray_Torus_Tests_Succeeded,
  Torus_Bound_Tests,
  Torus_Bound_Tests_Succeeded,
  Ray_CSG_Intersection_Tests,
  Ray_CSG_Intersection_Tests_Succeeded,
  Ray_CSG_Merge_Tests,
  Ray_CSG_Merge_Tests_Succeeded,
  Ray_CSG_Union_Tests,
  Ray_CSG_Union_Tests_Succeeded,
  Ray_Triangle_Tests,
  Ray_Triangle_Tests_Succeeded,
  Ray_TTF_Tests,
  Ray_TTF_Tests_Succeeded,
  Ray_Superellipsoid_Tests,
  Ray_Superellipsoid_Tests_Succeeded,
  Media_Samples,
  Media_Intervals,

  Reflected_Rays_Traced,
  Refracted_Rays_Traced,
  Transmitted_Rays_Traced,
  Internal_Reflected_Rays_Traced,
  Shadow_Cache_Hits,
  Shadow_Rays_Succeeded,
  Shadow_Ray_Tests,

  nChecked,
  nEnqueued,
  totalQueues,
  totalQueueResets,
  totalQueueResizes,
  Polynomials_Tested,
  Roots_Eliminated,
  VBuffer_Tests,
  VBuffer_Tests_Succeeded,
  LBuffer_Tests,
  LBuffer_Tests_Succeeded,

#if defined(MEM_STATS)
  MemStat_Smallest_Alloc,
  MemStat_Largest_Alloc,
  MemStat_Largest_Mem_Usage,
#if (MEM_STATS>=2)
  MemStat_Total_Allocs,
  MemStat_Total_Frees,
#endif
#endif

  /* Must be the last */
  MaxStat

} Stats;

typedef enum shelltype
{
   PRE_SCENE_SHL = 0,
   PRE_FRAME_SHL,
   POST_FRAME_SHL,
   POST_SCENE_SHL,
   USER_ABORT_SHL,
   FATAL_SHL,
   MAX_SHL /* Must be last */
} SHELLTYPE;

typedef enum shellret
{
  IGNORE_RET = 0,
  QUIT_RET,
  USER_RET,
  FATAL_RET,
  SKIP_ONCE_RET,
  ALL_SKIP_RET
} SHELLRET;

#ifdef __cplusplus
#undef POV_SHELLOUT_CAST
#define POV_SHELLOUT_CAST SHELLRET
#else
#define POV_SHELLOUT_CAST int
#endif

typedef struct shelldata
{
   SHELLRET Ret;
   int Inverse;
   char Command[POV_MAX_CMD_LENGTH];
} SHELLDATA;

typedef struct OPTIONS_STRUCT
{
  int File_Buffer_Size;
  unsigned long Options;
  char DisplayFormat;
  char PaletteOption;

  char OutputFormat;
  int OutputQuality;
  char Input_File_Name[FILE_NAME_LENGTH];
  char Output_File_Name[FILE_NAME_LENGTH];
  char Output_Path[FILE_NAME_LENGTH];
  char Output_Numbered_Name[FILE_NAME_LENGTH];
  char Scene_Name[FILE_NAME_LENGTH];
  DBL DisplayGamma;
  DBL GammaFactor;

  unsigned long Quality_Flags;

  long AntialiasDepth;
  DBL Antialias_Threshold;

  DBL JitterScale;

  int Abort_Test_Counter;

  char *Library_Paths[MAX_LIBRARIES];
  int Library_Path_Index;

  int First_Column, Last_Column;
  DBL First_Column_Percent, Last_Column_Percent;

  int First_Line, Last_Line;
  DBL First_Line_Percent, Last_Line_Percent;

  /* Parse */
  DBL Language_Version;

  unsigned Use_Slabs;
  long BBox_Threshold;

  int Quality;

  int PreviewGridSize_Start;   /* Mosaic Preview - Initial pixel grid size */
  int PreviewGridSize_End;     /* Mosaic Preview - Ending pixel grid size */

  FRAMESEQ FrameSeq;

  /* Should STREAM PATHS go somewhere here? */

  DBL Radiosity_Brightness;
  long Radiosity_Count;
  DBL Radiosity_Dist_Max;
  DBL Radiosity_Error_Bound;
  DBL Radiosity_Gray;  /* degree to which gathered light is grayed */
  DBL Radiosity_Low_Error_Factor;
  DBL Radiosity_Min_Reuse;
  long Radiosity_Nearest_Count;
  int Radiosity_Recursion_Limit;
  long Radiosity_Quality;  /* Q-flag value for light gathering */
  int Radiosity_File_ReadOnContinue;
  int Radiosity_File_SaveWhileRendering;
  int Radiosity_File_AlwaysReadAtStart;
  int Radiosity_File_KeepOnAbort;
  int Radiosity_File_KeepAlways;
  int Radiosity_Preview_Done;  /* used in cache file processing */


  int histogram_x, histogram_y, histogram_on;
  Histogram_Types histogram_type;
  char Histogram_File_Name[FILE_NAME_LENGTH];

  SHELLDATA *Shellouts;

  char Ini_Output_File_Name[FILE_NAME_LENGTH];

  int Tracing_Method;
  int Do_Stats;
} Opts;

/*****************************************************************************
* Global variables
******************************************************************************/
extern FRAME Frame;

extern COUNTER stats[MaxStat];
extern COUNTER totalstats[MaxStat];

extern time_t tstart, tstop;
extern DBL tparse, trender, tparse_total, trender_total;

extern char Color_Bits;

extern int Number_Of_Files;
extern Opts opts;

extern FILE *stat_file;
extern FILE_HANDLE *Output_File_Handle;


extern int Help_Available;

extern int Abort_Test_Every;
extern int Display_Started;
extern int Stage;
extern volatile int Stop_Flag;
extern int pre_init_flag;
extern int Experimental_Flag;

extern int Num_Echo_Lines, Echo_Line_Length;
extern char *Option_String_Ptr;      
extern DBL Clock_Delta;

/*****************************************************************************
* Global functions
******************************************************************************/

#ifdef NOCMDLINE
# ifdef ALTMAIN
  MAIN_RETURN_TYPE alt_main (void);
# else
  MAIN_RETURN_TYPE main (void);
# endif
#else
# ifdef ALTMAIN
  MAIN_RETURN_TYPE alt_main (int argc, char ** argv);
# else
  MAIN_RETURN_TYPE main (int argc, char ** argv);
# endif
#endif

int pov_stricmp (char *s1,char *s2);
void close_all (void);
void POV_Std_Split_Time (DBL time_dif,unsigned long *hrs,unsigned long *mins,DBL *secs);
FILE *Locate_File (char *filename, char *mode, char *ext1, char *ext2, char *buffer, int err_flag);

SHELLRET pov_shellout (SHELLTYPE Type);
void pre_init_povray (void);

void POV_Split_Path (char *s, char *p, char *f);

#endif
