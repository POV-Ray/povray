/****************************************************************************
*                   optout.c
*
*  This module contains functions for credit, usage, options and stats.
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

#include <ctype.h>
#include <time.h>
#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "atmosph.h"
#include "bezier.h"
#include "blob.h"
#include "bbox.h"
#include "cones.h"
#include "csg.h"
#include "discs.h"
#include "fractal.h"
#include "hfield.h"
#include "lathe.h"
#include "lighting.h"
#include "mesh.h"
#include "polysolv.h"
#include "objects.h"
#include "parse.h"
#include "point.h"
#include "poly.h"
#include "polygon.h"
#include "octree.h"
#include "quadrics.h"
#include "pgm.h"
#include "ppm.h"
#include "prism.h"
#include "radiosit.h"
#include "render.h"
#include "sor.h"
#include "spheres.h"
#include "super.h"
#include "targa.h"
#include "texture.h"
#include "torus.h"
#include "triangle.h"
#include "truetype.h"
#include "userio.h"
#include "lbuffer.h"
#include "vbuffer.h"
#include "povray.h"
#include "optin.h"
#include "optout.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define NUMBER_LENGTH 19
#define OUTPUT_LENGTH 15

#define NUMBER_OF_AUTHORS_ACROSS  4


/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

static char numbers[64][20] =
{
"0000000000000000001",
"0000000000000000002",
"0000000000000000004",
"0000000000000000008",
"0000000000000000016",
"0000000000000000032",
"0000000000000000064",
"0000000000000000128",
"0000000000000000256",
"0000000000000000512",
"0000000000000001024",
"0000000000000002048",
"0000000000000004096",
"0000000000000008192",
"0000000000000016384",
"0000000000000032768",
"0000000000000065536",
"0000000000000131072",
"0000000000000262144",
"0000000000000524288",
"0000000000001048576",
"0000000000002097152",
"0000000000004194304",
"0000000000008388608",
"0000000000016777216",
"0000000000033554432",
"0000000000067108864",
"0000000000134217728",
"0000000000268435456",
"0000000000536870912",
"0000000001073741824",
"0000000002147483648",
"0000000004294967296",
"0000000008589934592",
"0000000017179869184",
"0000000034359738368",
"0000000068719476736",
"0000000137438953472",
"0000000274877906944",
"0000000549755813888",
"0000001099511627776",
"0000002199023255552",
"0000004398046511104",
"0000008796093022208",
"0000017592186044416",
"0000035184372088832",
"0000070368744177664",
"0000140737488355328",
"0000281474976710656",
"0000562949953421312",
"0001125899906842624",
"0002251799813685248",
"0004503599627370496",
"0009007199254740992",
"0018014398509481984",
"0036028797018963968",
"0072057594037927936",
"0144115188075855872",
"0288230376151711744",
"0576460752303423488",
"1152921504606846976",
"2305843009213693952",
"4611686018427387904",
"9223372036854775808"
};

static char s1[OUTPUT_LENGTH], s2[OUTPUT_LENGTH];

char *Primary_Developers[] =
{
  "Steve Anger",
  "Dieter Bayer",
  "Chris Cason",
  "Chris Dailey",
  "Andreas Dilger",
  "Steve Demlow",
  "Alexander Enzmann",
  "Dan Farmer",
  "Timothy Wegner",
  "Chris Young",
  NULL   /* NULL flags the end of the list */
};

char *Contributing_Authors[] =
{
  "Steve A. Bennett",
  "David K. Buck",
  "Aaron A. Collins",
  "Pascal Massimino",
  "Jim McElhiney",
  "Douglas Muir",
  "Bill Pulver",
  "Robert Skinner",
  "Zsolt Szalavari",
  "Scott Taylor",
  "Drew Wells",
  NULL   /* NULL flags the end of the list */
};


/*****************************************************************************
* Static functions
******************************************************************************/

static void rinfo_on (char *string, unsigned value);
static void add_numbers (char *result, char *c1, char *c2);
static void counter_to_string (COUNTER *counter, char *string, int len);
static void print_intersection_stats (char *text, COUNTER *tests, COUNTER *succeeded);



/*****************************************************************************
*
* FUNCTION
*
*   add_numbers
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
*   Add two decimal numbers stored in ASCII strings.
*
* CHANGES
*
*   Mar 1995 : Creation
*
******************************************************************************/

static void add_numbers(char *result, char  *c1, char  *c2)
{
  int i;
  char carry, x;

  carry = '0';

  for (i = NUMBER_LENGTH-1; i >= 0; i--)
  {
    x = c1[i] + c2[i] + carry - '0' - '0';

    if (x > '9')
    {
      carry = '1';

      result[i] = x - 10;
    }
    else
    {
      carry = '0';

      result[i] = x;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   counter_to_string
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
*   Convert a low/high precision counter into a decimal number.
*
* CHANGES
*
*   Mar 1995 : Creation
*
******************************************************************************/

static void counter_to_string(COUNTER *counter, char *string, int len)
{
  char n[NUMBER_LENGTH+1];
  int i, j;
  COUNTER c;

  c = *counter;

  for (i = 0; i < NUMBER_LENGTH; i++)
  {
    n[i] = '0';
  }

  n[NUMBER_LENGTH] = '\0';

#if COUNTER_RESOLUTION == HIGH_RESOLUTION

  for (i = 0; i < 32; i++)
  {
    if (c.low & 1)
    {
      add_numbers(n, n, numbers[i]);
    }

    c.low >>= 1;
  }

  for (i = 32; i < 64; i++)
  {
    if (c.high & 1)
    {
      add_numbers(n, n, numbers[i]);
    }

    c.high >>= 1;
  }

#else

  for (i = 0; i < 32; i++)
  {
    if (c & 1)
    {
      add_numbers(n, n, numbers[i]);
    }

    c >>= 1;
  }

#endif

  /* Replace leading zeros. */

  for (i = 0; i < NUMBER_LENGTH-1; i++)
  {
    if (n[i] == '0')
    {
      n[i] = ' ';
    }
    else
    {
      break;
    }
  }

  /* Copy number into result string. */

  if (i >= NUMBER_LENGTH-len)
  {
    for (j = 0; j < i-NUMBER_LENGTH+len-1; j++)
    {
      string[j] = ' ';
    }

    string[j] = '\0';

    string = strcat(string, &n[i]);
  }
  else
  {
    /* Print numbers that don't fit into output string in million units. */

    string = "";

    n[NUMBER_LENGTH-6] = 'm';
    n[NUMBER_LENGTH-5] = '\0';

    string = strcat(string, &n[NUMBER_LENGTH-len+1-6]);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   print_intersections_stats
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
*   -
*
* CHANGES
*
*   Mar 1995 : Creation
*
******************************************************************************/

static void print_intersection_stats(char *text, COUNTER *tests, COUNTER  *succeeded)
{
  DBL t, s, p;

  if (!Test_Zero_Counter(*tests))
  {
    t = DBL_Counter(*tests);
    s = DBL_Counter(*succeeded);

    p = 100.0 * s / t;

    counter_to_string(tests, s1, OUTPUT_LENGTH);
    counter_to_string(succeeded, s2, OUTPUT_LENGTH);

    Statistics("%-22s  %s  %s  %8.2f\n", text, s1, s2, p);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   rinfo_on
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

static void rinfo_on(char *string, unsigned value)
{
  if (value)
  {
    Render_Info("%s.On",(string));
  }
  else
  {
    Render_Info("%sOff",(string));
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Print_Credits
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

void Print_Credits()
{
  Banner ("Persistence of Vision(tm) Ray Tracer Version %s%s\n", POV_RAY_VERSION, COMPILER_VER);
  Banner ("  %s\n", DISTRIBUTION_MESSAGE_1);
  Banner ("  %s\n", DISTRIBUTION_MESSAGE_2);
  Banner ("  %s\n", DISTRIBUTION_MESSAGE_3);
  Banner ("Copyright 1999 POV-Ray Team(tm)\n");
}



/*****************************************************************************
*
* FUNCTION
*
*   Print_Help_Screens
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
*   Print all help screens. Use an interactive menu if GET_KEY exists.
*
* CHANGES
*
*   Apr 1995 : Creation.
*
******************************************************************************/

void Print_Help_Screens()
{
#ifdef GET_KEY_EXISTS
  char c;
  int n, x, ok;

  Usage(-1, FALSE);

  for (n = 0; ; )
  {
    Banner("\n");
    Banner("[ Press 0 for general help, 1 to %d for help screen. Press 'q' to quit. ]", MAX_HELP_PAGE);

    do
    {
      ok = FALSE;

      GET_KEY(x);

      c = (char)x;

      if ((c >= '0') && (c <= '0' + MAX_HELP_PAGE))
      {
        ok = TRUE;

        n = (int)c - (int)'0';
      }
      else
      {
        if ((c == 'q') || (c == 'Q'))
        {
          ok = TRUE;
        }
      }
    }
    while(!ok);

    Banner("\n");

    if ((c == 'q') || (c == 'Q'))
    {
      break;
    }

    Usage(n, FALSE);
  }

#else
  int n;

  for (n = -1; n <= MAX_HELP_PAGE; n++)
  {
    Usage(n, (n == MAX_HELP_PAGE));
  }
#endif
  Terminate_POV(0);
}



/*****************************************************************************
*
* FUNCTION
*
*   Usage
*
* INPUT
*
*   n - Number of usage screen
*   f - Flag to terminate
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
*   Print out usage messages.
*
* CHANGES
*
*   Dec 1994 : Changed to show options depending on parameter n. [DB]
*
*   Feb 1995 : Changed to terminate only if f != 0. [DB]
*
******************************************************************************/

void Usage(int n, int  f)
{
  switch (n)
  {
    /* Help screen. */

    case 0:

      Banner("\n");
      Banner("Usage: POVRAY [+/-]Option1 [+/-]Option2 ... (-h or -? for help)\n");
      Banner("\n");
      Banner("  Example: POVRAY scene.ini +Iscene.pov +Oscene.tga +W320 +H200\n");
      Banner("  Example: POVRAY +Iscene.pov +Oscene.tga +W160 +H200 +V -D +X\n");
      Banner("\n");
      Banner("The help screen is divided into several parts. To access one part\n");
      Banner("just enter the number of the screen after the -h or -? option.\n");
      Banner("\n");
      Banner("E.g. use -h5 to see the help screen about the tracing options.\n");
      Banner("\n");
      Banner("  Number  Part\n");
      Banner("    1     Parsing Options\n");
      Banner("    2     Output Options\n");
      Banner("    3     Output Options - display related\n");
      Banner("    4     Output Options - file related\n");
      Banner("    5     Tracing Options\n");
      Banner("    6     Animation Options\n");
      Banner("    7     Redirecting Options\n");

      break;

    /* Parsing options. */

    case 1:

      Banner("\n");
      Banner("Parsing options\n");
      Banner("\n");
      Banner("  I<name> = input file name\n");
      Banner("  L<name> = library path prefix\n");
      Banner("  MVn.n   = set compability to version n.n\n");
      Banner("  SU      = split bounded unions if children are finite\n");
      Banner("  UR      = remove unnecessary bounding objects\n");

      break;

    /* Output options. */

    case 2:

      Banner("\n");
      Banner("Output options\n");
      Banner("\n");
      Banner("  Hnnn    = image height\n");
      Banner("  Wnnn    = image width\n");
      Banner("\n");
      Banner("  SRnn    = start at row nn | SR0.nn start row at nn percent of screen\n");
      Banner("  ERnn    = end   at row nn | ER0.nn end   row at nn percent of screen\n");
      Banner("  SCnn    = start at col nn | SC0.nn start col at nn percent of screen\n");
      Banner("  ECnn    = end   at col nn | EC0.nn end   col at nn percent of screen\n");
      Banner("\n");
      Banner("  C       = continue aborted trace\n");
      Banner("  P       = pause before exit\n");
      Banner("  V       = verbose messages on\n");
      Banner("  Xnnn    = enable early exit by key hit (every nnn pixels)\n");

      break;

    case 3:

      Banner("\n");
      Banner("Output options - display related\n");
      Banner("\n");
      Banner("  Dxy     = display in format x, using palette option y\n");
      Banner("  SPnnn   = Mosaic Preview display, Start grid size = 2, 4, 8, 16, ...\n");
      Banner("  EPnnn   = Mosaic Preview display, End grid size   = 2, 4, 8, 16, ...\n");
      Banner("  UD      = draw vista rectangles\n");

      break;

    /* Output options - file related. */

    case 4:

      Banner("\n");
      Banner("Output options - file related\n");
      Banner("\n");
      Banner("  Bnnn    = Use nnn KB for output file buffer\n");
      Banner("  Fx      = write output file in format x\n");
      Banner("            FC  - Compressed Targa with 24 or 32 bpp\n");
      Banner("            FNn - PNG (n bits/color, n = 5 to 16, default is 8)\n");
      Banner("            FP  - PPM\n");
      Banner("            FS  - System specific\n");
      Banner("            FT  - Uncompressed Targa with 24 or 32 bpp\n");
      Banner("  O<name> = output file name\n");
#if PRECISION_TIMER_AVAILABLE
      Banner("\n");
      Banner("  HTx     = write CPU utilization histogram in format x\n");
      Banner("            HTC - Comma separated values (CSV - spreadsheet)\n");
      Banner("            HTN - PNG grayscale\n");
      Banner("            HTP - PPM heightfield\n");
      Banner("            HTS - System specific\n");
      Banner("            HTT - Uncompressed TGA heightfield\n");
      Banner("            HTX - No histogram output\n");
      Banner("  HN<name>= histogram filename\n");
      Banner("  HSx.y   = histogram grid number of x, y divisions\n");
#endif

      break;

    /* Tracing options. */

    case 5:

      Banner("\n");
      Banner("Tracing options\n");
      Banner("\n");
      Banner("  MBnnn   = use slabs if more than nnn objects\n");
      Banner("  Qn      = image quality (0 = rough, 9 = full, R = radiosity)\n");
      Banner("  QR      = enable radiosity calculations for ambient light\n");
      Banner("\n");
      Banner("  A0.n    = perform antialiasing\n");
      Banner("  AMn     = use non-adaptive (n=1) or adaptive (n=2) supersampling\n");
      Banner("  Jn.n    = set antialiasing-jitter amount\n");
      Banner("  Rn      = set antialiasing-depth (use n X n rays/pixel)\n");
      Banner("\n");
      Banner("  UL      = use light buffer\n");
      Banner("  UV      = use vista buffer\n");

      break;

    /* Animation options. */

    case 6:

      Banner("\n");
      Banner("Animation options\n");
      Banner("\n");
      Banner("  Kn.n      = set frame clock to n.n\n");
      Banner("  KFInnn    = Initial frame number\n");
      Banner("  KFFnnn    = Final frame number\n");
      Banner("  KInnn.nn  = Initial clock value\n");
      Banner("  KFnnn.nn  = Final clock value\n");
      Banner("  SFnn      = Start subset at frame nn\n");
      Banner("  SF0.nn    = Start subset nn percent into sequence\n");
      Banner("  EFnn      = End subset at frame nn\n");
      Banner("  EF0.n     = End subset nn percent into sequence\n");
      Banner("  KC        = Calculate clock value for cyclic animation\n");
      Banner("\n");
      Banner("  UF        = use field rendering\n");
      Banner("  UO        = use odd lines in odd frames\n");

      break;

    /* Redirecting options. */

    case 7:

      Banner("\n");
      Banner("Redirecting options\n");
      Banner("\n");
      Banner("  GI<name>= write all .INI parameters to file name\n");
      Banner("  Gx<name>= write stream x to console and/or file name\n");
      Banner("            GA - All streams (except status)\n");
      Banner("            GD - Debug stream\n");
      Banner("            GF - Fatal stream\n");
      Banner("            GR - Render stream\n");
      Banner("            GS - Statistics stream\n");
      Banner("            GW - Warning stream\n");

      break;

    /* Usage ... */

    default:

      Print_Authors();
  }

#if defined(WAIT_FOR_KEYPRESS_EXISTS) && !defined(GET_KEY_EXISTS)
  Banner("\n");
  Banner("[ Paused for keypress... ]");

  WAIT_FOR_KEYPRESS;

  Banner("\n");
#endif

  if (f)
  {
    Terminate_POV(0);
  }
}


void Print_Authors()
{
  int h, i, j;

  Banner("\n");
  Banner("POV-Ray is based on DKBTrace 2.12 by David K. Buck & Aaron A. Collins.\n");
  Banner("\n");
  Banner("Primary POV-Ray 3 Developers: (Alphabetically)\n");

  for (i = h = 0; Primary_Developers[h] != NULL; i++)
  {
    for (j = 0; (j < NUMBER_OF_AUTHORS_ACROSS) && (Primary_Developers[h] != NULL); j++)
    {
        Banner(" %-18s", Primary_Developers[h++]);
    }

    Banner("\n");
  }

  Banner("\n");
  Banner("Major Contributing Authors: (Alphabetically)\n");

  for (i = h = 0; Contributing_Authors[h] != NULL; i++)
  {
    for (j = 0; (j < NUMBER_OF_AUTHORS_ACROSS) && (Contributing_Authors[h] != NULL); j++)
    {
        Banner(" %-18s", Contributing_Authors[h++]);
    }

    Banner("\n");
  }

  Banner("\n");
  Banner("Other contributors listed in the documentation.\n");
}


/*****************************************************************************
*
* FUNCTION
*
*   Print_Options
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

void Print_Options()
{
  int i, j;

  /* Print parsing options. */

  Render_Info("Parsing Options\n");

  Render_Info("  Input file: %s", opts.Input_File_Name);
  Render_Info(" (compatible to version %.1f)\n", opts.Language_Version);

  rinfo_on   ("  Remove bounds.......", (opts.Options & REMOVE_BOUNDS));
  rinfo_on   ("  Split unions........", (opts.Options & SPLIT_UNION));
  Render_Info("\n");

  j = 17;

  Render_Info("  Library paths:");

  for (i = 0; i < opts.Library_Path_Index; i++)
  {
    j += strlen(opts.Library_Paths[i])+2;

    if (j > 77)
    {
      Render_Info("\n   ", j);

      j = strlen(opts.Library_Paths[i]) + 5;
    }

    Render_Info(" %s", opts.Library_Paths[i]);
  }

  Render_Info("\n");

  /* Print output options. */

  Render_Info("Output Options\n");

  Render_Info("  Image resolution %d by %d",
                                    Frame.Screen_Width, Frame.Screen_Height);
  Render_Info(" (rows %d to %d, columns %d to %d).\n",
                       opts.First_Line+1, opts.Last_Line,
                       opts.First_Column+1, opts.Last_Column);

  if (opts.Options & DISKWRITE)
  {
    Render_Info("  Output file: %s%s, ", opts.Output_Path, opts.Output_File_Name);

    if (toupper(opts.OutputFormat) == 'N' && opts.Options & HF_GRAY_16)
    {
       Render_Info("%d bpp ", opts.OutputQuality);
    }
    else
    {
      Render_Info("%d bpp ", ((opts.Options & OUTPUT_ALPHA) ? 4 : 3) * opts.OutputQuality);
    }

    switch (toupper(opts.OutputFormat))
    {
      case 'C': Render_Info("RLE Targa");       break;
      case 'N': Render_Info("PNG");             break;
      case 'P': Render_Info("PPM");             break;
      case 'S': Render_Info("(system format)"); break;
      case 'T': Render_Info("Targa");           break;
    }

    if (opts.Options & HF_GRAY_16)
    {
      if (toupper(opts.OutputFormat) == 'N')
      {
        Render_Info(" grayscale");
      }
      else if (opts.Options & HF_GRAY_16)
      {
        Render_Info(" POV heightfield");
      }
    }

    if (opts.Options & OUTPUT_ALPHA)
    {
      Render_Info(" with alpha");
    }

    if ((opts.Options & BUFFERED_OUTPUT) && (opts.File_Buffer_Size != 0))
    {
      Render_Info(", %d KByte buffer\n", opts.File_Buffer_Size/1024);
    }
    else
    {
      Render_Info("\n");
    }
  }
  else if (opts.Options & DISPLAY)
  {
    if (opts.histogram_on == TRUE)
    {
      Warning(0.0, "  Rendering to screen and histogram file only.\n");
    }
    else
    {
      Warning(0.0, "  Rendering to screen only. No file output.\n");
    }
  }
  else if (opts.histogram_on == TRUE)
  {
    Warning(0.0, "  Rendering to histogram file only.\n");
  }
  else
  {
    Warning(0.0, "  Rendering to nothing! No screen or file output.\n");
  }

  rinfo_on   ("  Graphic display.....", (opts.Options & DISPLAY));
  if (opts.Options & DISPLAY)
  {
    Render_Info("  (type: %c, palette: %c, gamma: %4.2g)",
               toupper(opts.DisplayFormat), toupper(opts.PaletteOption),
               opts.DisplayGamma);
  }
  Render_Info("\n");

  rinfo_on   ("  Mosaic preview......", (opts.Options & PREVIEW));
  if (opts.Options & PREVIEW)
  {
    Render_Info("  (pixel sizes %d to %d)",
               opts.PreviewGridSize_Start, opts.PreviewGridSize_End);
  }
  Render_Info("\n");

#if PRECISION_TIMER_AVAILABLE
  rinfo_on ("  CPU usage histogram.", opts.histogram_on);
  if (opts.histogram_on)
  {
    char *type;

    switch (opts.histogram_type)
    {
      case CSV:   type = "CSV"; break;
      case TARGA: type = "TGA"; break;
      case PNG:   type = "PNG"; break;
      case PPM:   type = "PPM"; break;
      case SYS:   type = "(system format)"; break;
      case NONE:                     /* Just to stop warning messages */
      default:    type = "None"; break;
    }

    Render_Info("  (name: %s type: %s, grid: %dx%d)",
             opts.Histogram_File_Name,type,opts.histogram_x,opts.histogram_y);
  }
  Render_Info("\n");
#endif /* PRECISION_TIMER_AVAILABLE */

  rinfo_on   ("  Continued trace.....", (opts.Options & CONTINUE_TRACE));
  rinfo_on   ("  Allow interruption..", (opts.Options & EXITENABLE));
  rinfo_on   ("  Pause when done.....", (opts.Options & PROMPTEXIT));
  Render_Info("\n");

  rinfo_on   ("  Verbose messages....", (opts.Options & VERBOSE));

  Render_Info("\n");

  /* Print tracing options. */

  Render_Info("Tracing Options\n");

  Render_Info("  Quality: %2d\n", opts.Quality);

  rinfo_on   ("  Bounding boxes......", opts.Use_Slabs);
  if (opts.Use_Slabs)
  {
    Render_Info("  Bounding threshold: %d ", opts.BBox_Threshold);
  }
  Render_Info("\n");

  rinfo_on("  Light Buffer........", (opts.Options & USE_LIGHT_BUFFER));
  rinfo_on("  Vista Buffer........", (opts.Options & USE_VISTA_BUFFER));
  if (opts.Options & USE_VISTA_BUFFER && opts.Options & DISPLAY)
  {
    rinfo_on("  Draw Vista Buffer...", (opts.Options & USE_VISTA_DRAW));
  }
  Render_Info("\n");

  rinfo_on   ("  Antialiasing........", (opts.Options & ANTIALIAS));
  if (opts.Options & ANTIALIAS)
  {
    Render_Info("  (Method %d, ", opts.Tracing_Method);
    Render_Info("Threshold %.3f, ", opts.Antialias_Threshold);
    Render_Info("Depth %ld, ", opts.AntialiasDepth);
    Render_Info("Jitter %.2f)", opts.JitterScale);
  }
  Render_Info("\n");

  rinfo_on   ("  Radiosity...........", (opts.Options & RADIOSITY));

  Render_Info("\n");

  /* Print animation options. */

  Render_Info("Animation Options\n");

  if (opts.FrameSeq.FrameType == FT_MULTIPLE_FRAME)
  {
    Render_Info("  Initial Frame..%8d", opts.FrameSeq.InitialFrame);
    Render_Info("  Final Frame....%8d\n", opts.FrameSeq.FinalFrame);
    Render_Info("  Initial Clock..%8.3f", opts.FrameSeq.InitialClock);
    Render_Info("  Final Clock....%8.3f\n", opts.FrameSeq.FinalClock);
    rinfo_on   ("  Cyclic Animation....", (opts.Options & CYCLIC_ANIMATION));
    rinfo_on   ("  Field render........", (opts.FrameSeq.Field_Render_Flag));
    rinfo_on   ("  Odd lines/frames....", (opts.FrameSeq.Odd_Field_Flag));
  }
  else
  {
    Render_Info("  Clock value....%8.3f", opts.FrameSeq.Clock_Value);
    Render_Info("  (Animation off)");
  }

  Render_Info("\n");

  /* Print redirecting options. */

  Render_Info("Redirecting Options\n");

  rinfo_on   ("  All Streams to console.........", Stream_Info[ALL_STREAM].do_console);
  if (Stream_Info[ALL_STREAM].name == NULL)
  {
    Render_Info("\n");
  }
  else
  {
    Render_Info("  and file %s\n", Stream_Info[ALL_STREAM].name);
  }

  rinfo_on   ("  Debug Stream to console........", Stream_Info[DEBUG_STREAM].do_console);
  if (Stream_Info[DEBUG_STREAM].name == NULL)
  {
    Render_Info("\n");
  }
  else
  {
    Render_Info("  and file %s\n", Stream_Info[DEBUG_STREAM].name);
  }

  rinfo_on   ("  Fatal Stream to console........", Stream_Info[FATAL_STREAM].do_console);
  if (Stream_Info[FATAL_STREAM].name == NULL)
  {
    Render_Info("\n");
  }
  else
  {
    Render_Info("  and file %s\n", Stream_Info[FATAL_STREAM].name);
  }

  rinfo_on   ("  Render Stream to console.......", Stream_Info[RENDER_STREAM].do_console);
  if (Stream_Info[RENDER_STREAM].name == NULL)
  {
    Render_Info("\n");
  }
  else
  {
    Render_Info("  and file %s\n", Stream_Info[RENDER_STREAM].name);
  }

  rinfo_on   ("  Statistics Stream to console...", Stream_Info[STATISTIC_STREAM].do_console);
  if (Stream_Info[STATISTIC_STREAM].name == NULL)
  {
    Render_Info("\n");
  }
  else
  {
    Render_Info("  and file %s\n", Stream_Info[STATISTIC_STREAM].name);
  }

  rinfo_on   ("  Warning Stream to console......", Stream_Info[WARNING_STREAM].do_console);
  if (Stream_Info[WARNING_STREAM].name == NULL)
  {
    Render_Info("\n");
  }
  else
  {
    Render_Info("  and file %s\n", Stream_Info[WARNING_STREAM].name);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Print_Stats
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

void Print_Stats(COUNTER *pstats)
{
  unsigned long hours, minutes;
  DBL seconds, taverage, ttotal;
  long Pixels_In_Image;

  Pixels_In_Image = (long)Frame.Screen_Width * (long)Frame.Screen_Height;

  Statistics ("\n%s Statistics", opts.Input_File_Name);

  if ( Pixels_In_Image > DBL_Counter(pstats[Number_Of_Pixels]) )
    Statistics (" (Partial Image Rendered)");

  Statistics (", Resolution %d x %d\n", Frame.Screen_Width, Frame.Screen_Height);

  Statistics ("----------------------------------------------------------------------------\n");

  Statistics ("Pixels: %15.0f   Samples: %15.0f   Smpls/Pxl: ",
              DBL_Counter(pstats[Number_Of_Pixels]),
              DBL_Counter(pstats[Number_Of_Samples]));

  if (!Test_Zero_Counter(pstats[Number_Of_Pixels]))
  {
    Statistics ("%.2f\n",
              DBL_Counter(pstats[Number_Of_Samples]) /
              DBL_Counter(pstats[Number_Of_Pixels]));
  }
  else
  {
    Statistics ("-\n");
  }

  counter_to_string(&pstats[Number_Of_Rays], s1, OUTPUT_LENGTH);
  counter_to_string(&pstats[ADC_Saves], s2, OUTPUT_LENGTH);

  Statistics ("Rays:    %s   Saved:    %s   Max Level: %d/%d\n",
              s1, s2, Highest_Trace_Level, Max_Trace_Level);

  Statistics ("----------------------------------------------------------------------------\n");

  Statistics ("Ray->Shape Intersection          Tests       Succeeded  Percentage\n");
  Statistics ("----------------------------------------------------------------------------\n");

  print_intersection_stats("Bezier Patch",          &pstats[Ray_Bicubic_Tests],
       &pstats[Ray_Bicubic_Tests_Succeeded]);

  print_intersection_stats("Blob",                  &pstats[Ray_Blob_Tests],
       &pstats[Ray_Blob_Tests_Succeeded]);
#ifdef BLOB_EXTRA_STATS
  print_intersection_stats("Blob Component",        &pstats[Blob_Element_Tests],
       &pstats[Blob_Element_Tests_Succeeded]);
  print_intersection_stats("Blob Bound",            &pstats[Blob_Bound_Tests],
       &pstats[Blob_Bound_Tests_Succeeded]);
#endif

  print_intersection_stats("Box",                   &pstats[Ray_Box_Tests],
       &pstats[Ray_Box_Tests_Succeeded]);
  print_intersection_stats("Cone/Cylinder",         &pstats[Ray_Cone_Tests],
       &pstats[Ray_Cone_Tests_Succeeded]);
  print_intersection_stats("CSG Intersection",      &pstats[Ray_CSG_Intersection_Tests],
       &pstats[Ray_CSG_Intersection_Tests_Succeeded]);
  print_intersection_stats("CSG Merge",             &pstats[Ray_CSG_Merge_Tests],
       &pstats[Ray_CSG_Merge_Tests_Succeeded]);
  print_intersection_stats("CSG Union",             &pstats[Ray_CSG_Union_Tests],
       &pstats[Ray_CSG_Union_Tests_Succeeded]);
  print_intersection_stats("Disc",                  &pstats[Ray_Disc_Tests],
       &pstats[Ray_Disc_Tests_Succeeded]);
  print_intersection_stats("Fractal",               &pstats[Ray_Fractal_Tests],
       &pstats[Ray_Fractal_Tests_Succeeded]);

  print_intersection_stats("Height Field",          &pstats[Ray_HField_Tests],
       &pstats[Ray_HField_Tests_Succeeded]);
#ifdef HFIELD_EXTRA_STATS
  print_intersection_stats("Height Field Box",      &pstats[Ray_HField_Box_Tests],
       &pstats[Ray_HField_Box_Tests_Succeeded]);
  print_intersection_stats("Height Field Triangle", &pstats[Ray_HField_Triangle_Tests],
       &pstats[Ray_HField_Triangle_Tests_Succeeded]);
  print_intersection_stats("Height Field Block",    &pstats[Ray_HField_Block_Tests],
       &pstats[Ray_HField_Block_Tests_Succeeded]);
  print_intersection_stats("Height Field Cell",     &pstats[Ray_HField_Cell_Tests],
       &pstats[Ray_HField_Cell_Tests_Succeeded]);
#endif

  print_intersection_stats("Lathe",                 &pstats[Ray_Lathe_Tests],
       &pstats[Ray_Lathe_Tests_Succeeded]);
#ifdef LATHE_EXTRA_STATS
  print_intersection_stats("Lathe Bound",           &pstats[Lathe_Bound_Tests],
       &pstats[Lathe_Bound_Tests_Succeeded]);
#endif

  print_intersection_stats("Mesh",                  &pstats[Ray_Mesh_Tests],
       &pstats[Ray_Mesh_Tests_Succeeded]);
  print_intersection_stats("Plane",                 &pstats[Ray_Plane_Tests],
       &pstats[Ray_Plane_Tests_Succeeded]);
  print_intersection_stats("Polygon",               &pstats[Ray_Polygon_Tests],
       &pstats[Ray_Polygon_Tests_Succeeded]);

  print_intersection_stats("Prism",                 &pstats[Ray_Prism_Tests],
       &pstats[Ray_Prism_Tests_Succeeded]);
#ifdef PRISM_EXTRA_STATS
  print_intersection_stats("Prism Bound",           &pstats[Prism_Bound_Tests],
       &pstats[Prism_Bound_Tests_Succeeded]);
#endif

  print_intersection_stats("Quadric",               &pstats[Ray_Quadric_Tests],
       &pstats[Ray_Quadric_Tests_Succeeded]);
  print_intersection_stats("Quartic/Poly",          &pstats[Ray_Poly_Tests],
       &pstats[Ray_Poly_Tests_Succeeded]);
  print_intersection_stats("Sphere",                &pstats[Ray_Sphere_Tests],
       &pstats[Ray_Sphere_Tests_Succeeded]);
  print_intersection_stats("Superellipsoid",        &pstats[Ray_Superellipsoid_Tests],
       &pstats[Ray_Superellipsoid_Tests_Succeeded]);

  print_intersection_stats("Surface of Revolution", &pstats[Ray_Sor_Tests],
       &pstats[Ray_Sor_Tests_Succeeded]);
#ifdef SOR_EXTRA_STATS
  print_intersection_stats("Surface of Rev. Bound", &pstats[Sor_Bound_Tests],
       &pstats[Sor_Bound_Tests_Succeeded]);
#endif

  print_intersection_stats("Torus",                 &pstats[Ray_Torus_Tests],
       &pstats[Ray_Torus_Tests_Succeeded]);
#ifdef TORUS_EXTRA_STATS
  print_intersection_stats("Torus Bound",           &pstats[Torus_Bound_Tests],
       &pstats[Torus_Bound_Tests_Succeeded]);
#endif

  print_intersection_stats("Triangle",              &pstats[Ray_Triangle_Tests],
       &pstats[Ray_Triangle_Tests_Succeeded]);
  print_intersection_stats("True Type Font",        &pstats[Ray_TTF_Tests],
       &pstats[Ray_TTF_Tests_Succeeded]);

  print_intersection_stats("Bounding Object", &pstats[Bounding_Region_Tests],
       &pstats[Bounding_Region_Tests_Succeeded]);
  print_intersection_stats("Clipping Object", &pstats[Clipping_Region_Tests],
       &pstats[Clipping_Region_Tests_Succeeded]);
  print_intersection_stats("Bounding Box",    &pstats[nChecked],
       &pstats[nEnqueued]);
  print_intersection_stats("Light Buffer",    &pstats[LBuffer_Tests],
       &pstats[LBuffer_Tests_Succeeded]);
  print_intersection_stats("Vista Buffer",    &pstats[VBuffer_Tests],
       &pstats[VBuffer_Tests_Succeeded]);

  Statistics ("----------------------------------------------------------------------------\n");

  if (!Test_Zero_Counter(pstats[Polynomials_Tested]))
  {
    counter_to_string(&pstats[Polynomials_Tested], s1, OUTPUT_LENGTH);
    counter_to_string(&pstats[Roots_Eliminated], s2, OUTPUT_LENGTH);

    Statistics ("Roots tested:       %s   eliminated:      %s\n", s1, s2);
  }

  counter_to_string(&pstats[Calls_To_Noise], s1, OUTPUT_LENGTH);
  counter_to_string(&pstats[Calls_To_DNoise], s2, OUTPUT_LENGTH);

  Statistics ("Calls to Noise:     %s   Calls to DNoise: %s\n", s1, s2);

  Statistics ("----------------------------------------------------------------------------\n");

  /* Print media samples. */

  if (!Test_Zero_Counter(pstats[Media_Intervals]))
  {
    counter_to_string(&pstats[Media_Intervals], s1, OUTPUT_LENGTH);
    counter_to_string(&pstats[Media_Samples], s2, OUTPUT_LENGTH);

    Statistics ("Media Intervals:    %s   Media Samples:   %s (%4.2f)\n", s1, s2,
      DBL_Counter(pstats[Media_Samples]) / DBL_Counter(pstats[Media_Intervals]));
  }

  if (!Test_Zero_Counter(pstats[Shadow_Ray_Tests]))
  {
    counter_to_string(&pstats[Shadow_Ray_Tests], s1, OUTPUT_LENGTH);
    counter_to_string(&pstats[Shadow_Rays_Succeeded], s2, OUTPUT_LENGTH);

    Statistics ("Shadow Ray Tests:   %s   Succeeded:       %s\n", s1, s2);
  }

  if (!Test_Zero_Counter(pstats[Reflected_Rays_Traced]))
  {
    counter_to_string(&pstats[Reflected_Rays_Traced], s1, OUTPUT_LENGTH);

    Statistics ("Reflected Rays:     %s", s1);

    if (!Test_Zero_Counter(pstats[Internal_Reflected_Rays_Traced]))
    {
      counter_to_string(&pstats[Internal_Reflected_Rays_Traced], s1, OUTPUT_LENGTH);

      Statistics ("   Total Internal:  %s", s1);
    }

    Statistics ("\n");
  }

  if (!Test_Zero_Counter(pstats[Refracted_Rays_Traced]))
  {
    counter_to_string(&pstats[Refracted_Rays_Traced], s1, OUTPUT_LENGTH);

    Statistics ("Refracted Rays:     %s\n", s1);
  }

  if (!Test_Zero_Counter(pstats[Transmitted_Rays_Traced]))
  {
    counter_to_string(&pstats[Transmitted_Rays_Traced], s1, OUTPUT_LENGTH);

    Statistics ("Transmitted Rays:   %s\n", s1);
  }

  if (!Test_Zero_Counter(pstats[Istack_overflows]))
  {
    counter_to_string(&pstats[Istack_overflows], s1, OUTPUT_LENGTH);

    Statistics ("I-Stack overflows:  %s\n", s1);
  }

  if ( ra_reuse_count || ra_gather_count )
  {
    Statistics ("----------------------------------------------------------------------------\n");
    Statistics ("Radiosity samples calculated:  %9ld (%.2f percent)\n", ra_gather_count,
                100.*(DBL) ra_gather_count / ((DBL)(ra_gather_count + ra_reuse_count)));
    /* Note:  don't try to put in a percent sign.  There is basically no way to do this
       which is completely portable using va_start */
    Statistics ("Radiosity samples reused:      %9ld\n", ra_reuse_count);
  }

#if defined(MEM_STATS)
  Statistics ("----------------------------------------------------------------------------\n");

  Long_To_Counter(mem_stats_smallest_alloc(), pstats[MemStat_Smallest_Alloc]);
  counter_to_string(&pstats[MemStat_Smallest_Alloc], s1, OUTPUT_LENGTH);

  Long_To_Counter(mem_stats_largest_alloc(), pstats[MemStat_Largest_Alloc]);
  counter_to_string(&pstats[MemStat_Largest_Alloc],  s2, OUTPUT_LENGTH);

#if (MEM_STATS==1)
  Statistics ("Smallest Alloc:     %s bytes   Largest:   %s\n", s1, s2);
#elif (MEM_STATS>=2)
  Statistics ("Smallest Alloc:     %s bytes @ %s:%d\n", s1, mem_stats_smallest_file(), mem_stats_smallest_line());
  Statistics ("Largest  Alloc:     %s bytes @ %s:%d\n", s2, mem_stats_largest_file(), mem_stats_largest_line());

  Long_To_Counter(mem_stats_total_allocs(), pstats[MemStat_Total_Allocs]);
  counter_to_string(&pstats[MemStat_Total_Allocs], s1, OUTPUT_LENGTH);
  Long_To_Counter(mem_stats_total_frees(), pstats[MemStat_Total_Frees]);
  counter_to_string(&pstats[MemStat_Total_Frees], s2, OUTPUT_LENGTH);
  Statistics ("Total Alloc calls:  %s         Free calls:%s\n", s1, s2);
#endif

  Long_To_Counter(mem_stats_largest_mem_usage(), pstats[MemStat_Largest_Mem_Usage]);
  counter_to_string(&pstats[MemStat_Largest_Mem_Usage], s1, OUTPUT_LENGTH);
  Statistics ("Peak memory used:   %s bytes\n", s1);

#endif

  Statistics ("----------------------------------------------------------------------------\n");

  /* Get time in case the trace was aborted. */

  if (trender == 0.0)
  {
    STOP_TIME

    trender = TIME_ELAPSED
  }

  if (opts.FrameSeq.FrameType == FT_MULTIPLE_FRAME)
  {
    if (tparse_total != 0.0)
    {
      taverage = tparse_total /
          (DBL)(opts.FrameSeq.FrameNumber - opts.FrameSeq.InitialFrame + 1);

      SPLIT_TIME(taverage,&hours,&minutes,&seconds);
      Statistics ("Time For Parse/Frame:  %3ld hours %2ld minutes %5.1f seconds (%ld seconds)\n",
          hours, minutes, seconds, (long)taverage);

      SPLIT_TIME(tparse_total,&hours,&minutes,&seconds);
      Statistics ("Time For Parse Total:  %3ld hours %2ld minutes %5.1f seconds (%ld seconds)\n",
          hours, minutes, seconds, (long)tparse_total);
    }

    if (trender_total != 0.0)
    {
      taverage = trender_total /
          (DBL)(opts.FrameSeq.FrameNumber - opts.FrameSeq.InitialFrame + 1);

      SPLIT_TIME(taverage,&hours,&minutes,&seconds);
      Statistics ("Time For Trace/Frame:  %3ld hours %2ld minutes %5.1f seconds (%ld seconds)\n",
          hours, minutes, seconds, (long)taverage);

      SPLIT_TIME(trender_total,&hours,&minutes,&seconds);
      Statistics ("Time For Trace Total:  %3ld hours %2ld minutes %5.1f seconds (%ld seconds)\n",
          hours, minutes, seconds, (long)trender_total);
    }

    ttotal = tparse_total + trender_total;

    if (ttotal != 0.0)
    {
      SPLIT_TIME(ttotal,&hours,&minutes,&seconds);

      Statistics ("          Total Time:  %3ld hours %2ld minutes %5.1f seconds (%ld seconds)\n",
          hours, minutes, seconds, (long)ttotal);
    }
  }
  else
  {
    if (tparse != 0.0)
    {
      SPLIT_TIME(tparse,&hours,&minutes,&seconds);

      Statistics ("Time For Parse:  %3ld hours %2ld minutes %5.1f seconds (%ld seconds)\n",
          hours, minutes, seconds, (long)tparse);
    }

    if (trender != 0.0)
    {
      SPLIT_TIME(trender,&hours,&minutes,&seconds);

      Statistics ("Time For Trace:  %3ld hours %2ld minutes %5.1f seconds (%ld seconds)\n",
          hours, minutes, seconds, (long)trender);
    }

    ttotal = tparse + trender;

    if (ttotal != 0.0)
    {
      SPLIT_TIME(ttotal,&hours,&minutes,&seconds);

      Statistics ("    Total Time:  %3ld hours %2ld minutes %5.1f seconds (%ld seconds)\n",
          hours, minutes, seconds, (long)ttotal);
    }
  }
}
