/****************************************************************************
*                   msdosvid.c
*
*  This module implements the VGA video display code for msdos 32-bit
*  protected mode versions of POV-Ray under various compilers.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file. If
*  POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's Graphics Developer's
*  Forum.  The latest version of POV-Ray may be found there as well.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

/* 

This module contains video output routines for MS-DOS.

It was extensively re-written in September '94 by Chris Young to include
Kendall Bennett's PMODE.LIB which greatly simplified the protected mode
interface on various compilers.  It was based on Kendall's SVGAKIT, work
by Chris Cason and earlier versions of the POV-Ray video code.  During this 
rewrite, most support for real mode and 16 bit protected mode was dropped.  
This necessitated dropping support for the TIGA video board.  

Then in June '94 it was updated again by Chris Young to integrate
fully new VESA material by Kendall Bennett to upgrade to VESA 2.0. What 
follows in the comment block are notes on the implentation and 
acknowledgments to those who have contributed to the development of this 
module even though their contributions may have been lost along the way.

The original IBM VGA color output routines for MS/DOS were written 
by Aaron A. Collins.  It used the hsv palette option '0'.

Further SVGA, MVGA mods by Aaron A. Collins:
SVGA, MVGA assembler routines originally by John Bridges.
VESA assembler routines from FRACTINT, by The Stone Soup Group
AT&T VDC600 SVGA mods to DKB Trace 2.01 module IBM.C by John Gooding

Parts of this module were based upon the collective wisdom of the VGAKIT34 
package, with support for all of the SVGA types known at that time.  
It was written by John Bridges, CIS:73307,606, GENIE:J.BRIDGES.  It was 
originally coded in IBM 80x86 assembler, and since POV-Ray is a completely 
"C"-based package, it was converted into "C".  

The VESA VGA mode were originally adapted from FRACTINT but have been highly
modified since then.

Other POV-Ray Enhancements:

  (S)VGA B&W 64 Greyscale code courtesy of Grant Hess 6/21/91
  16/24-bit HGWS/TIGA/TRUECOLOR code courtesy of Jay S. Curtis 11/15/90 but
    was removed because it was not 32-bit protected mode.

  VGA "332" palette routines courtesy of Alexander Enzmann 7/20/91

  Additional support for VESA and Tseng Labs 256 and 32768 color modes by
  Charles Marslett  (CIS: 75300,1636).  4/28/91

  ATI VGA Wonder XL Support for 32,768 color mode and all other SVGA modes
  Also image centering, outline box, and forces SVGA mode for small images.  
  Minor bug fixes and cleanups as well by Randy Antler.  9/11/91

  Extensive work to combine IBMPRO.C and IBM.C into 1 file by B.Pulver 10/20/91

  Corrected defects in the centering and high color support for VESA modes.
  Extended Doug's dithering code to other high color modes and eliminated
  a bad validity check in the 24-bit code high color cards.  
                                      -- Charles Marslett, 1992.

  Added support for Diamond Speedstar 24X including HiColor display in 
  640x480 and 800x600 resolutions . Fixed bug for displaying images 
  larger than screen resolutions. Added bank switch checking inside pixels 
  for truecolor modes.  -- Lutz Kretzschmar,  11/05/92

  Reworked find_go32() to make the VESA routines work properly with DJ's 
  port of IBMGCC
   - Aaron A. Collins, 2/2/93.

  Completed Intel Code Builder code for VESA 1.2 support. 
  Added: Set palette to P_332 if Hi/True color VESA init fails.
         Doubled dithering matrixes to support 2048Xxxxx images.
         Added code to disable dithering @ rez over 2048Xxxxx.
         Added message warning user of failure if no_valid_mode is called
         during VESA initialization attempt. - B.Pulver 1993

  Found and fixed consistency bug in find_go32(). 
  Added DEBUG_VIDEO diagnostics - Aaron A. Collins, 8/2/1993

  Updated Watcom VESA support for DPMI. -- Carl Peterson 1/94

************************************************************************/

/************************
* Various defines that affect debugging
*************************/
/*#define DEBUG_VIDEO*/ /* Uncomment to display Video initialization info during program startup. */
/*#define DEBUG_VESA*/ /* Uncomment to display VESA Video info. */

/************************
* Include file common to all compilers
*************************/
#include <dos.h>        /* MS-DOS specific - for int86() REGS struct, etc. */
#include <stdarg.h>
#include <time.h>
#include "frame.h"
#include "vesavbe.h"
#include "povproto.h"
#include "povray.h"
#include "msdosall.h"
#include <io.h>       /* for low-level open() funct, etc. */
#include <fcntl.h>    /* for low-level open() modes O_RDWR, O_BINARY, etc. */

/************************
* Compiler specific setups & defines. 
*************************/

/************************
* BORLAND C++ Version 4.02 and later with DOS Power Pack
*************************/
#if defined(__BORLANDC__) || defined(__TURBOC__)
  extern unsigned _stklen = 12288; /* fairly large stack for HEAVY recursion */
  #define CLD __emit__(0xFC)
  #define _far
  #ifdef __DPMI32__
   #define DPMI32       /* essential for the PMODE include file   */
  #endif
  #include "pmode.h"
  #undef outp         /* bug in BC++ inline.  Use functions instead */
  #undef outpw
#endif

/************************
* WATCOM Version ?
*************************/
#ifdef __WATCOMC__
  #include <float.h>
  #include "pmode.h"
  unsigned short __8087cw = IC_AFFINE | RC_NEAR | PC_64  | 0x007F;
  #define CLD clear_direction()
  void clear_direction(void);
  #pragma aux clear_direction = 0xFC parm [] modify nomemory;
#endif

/************************
* Zortech and Symantech Version ?
*************************/
#ifdef __ZTC__
  #include <math.h>
  #include <int.h>

  #ifndef DOS386
    #ifndef DOS16RM
      unsigned _stack = 12288;
    #endif
  #endif

  #define CLD

  #ifdef DOS386
    extern unsigned short _x386_zero_base_selector;
    #define _disable()
    #define _enable()
    #define _cdecl
    #define PM_int86  int86_real
    #define PM_int86x int86x_real
  #else
    #ifdef DOS16RM
      void *D16SegAbsolute(long absadr, short size);
      void D16SegCancel(void *segptr);
      #define _disable()
      #define _enable()
      unsigned char *display_base;
    #else
      #define _disable() int_off()
      #define _enable()  int_on()
    #endif
  #endif

#endif

/*****************
*
*  NOTE: Many DJGCC/PMODE defines moved to MSDOSALL.H
*
*****************/

#ifdef __GO32__
/************************
* DJGCC port of Gnu's GCC Version ?
*************************/
  #include <errno.h>
  #include <ctype.h>
  #include "pmode.h"

  #define _cdecl
  #define _far
  #define CLD

#endif



/************************
* Special matherr routines for Watcom & GCC only
*************************/
#ifdef __WATCOMC__
  int matherr(struct exception *);     /* Math error traps. */
#else
  int _cdecl matherr(struct exception *);
#endif


/************************
* Defines and macros used throughout
*************************/

/* POV-Ray's own mode numbers for supported VGA adapter types 1 - 9, 
   A - Z.  0 is for auto-detect. 
   
Not all the hi-res modes of all the various SVGA adapters, have been 
implemented.  Most just use 640x480.  If you select a trace size bigger than 
the program and/or card can handle, it is dynamically scaled to fit the 
available resolution, so you'll be able to see a rough approximation of 
an 800x600 trace even on any generic VGA card at 320x200 resolution.  
For users with a regular non-Super VGA card, it includes "MODE13x" or MVGA 
(modified VGA) mode (some call it "tweaked", or call it "Simulated SVGA"), 
which gives 360x480 on any reasonably register-compatible plain vanilla VGA 
card.  This mode gives a good simulated 640 by 480 screen resolution 
using 360x480.  
*/
#define BASIC_VGA       1               /* 1 - Tested: AAC */
#define MODE13x         2               /* 2 - Tested: AAC */
#define TSENG3          3               /* 3 - Tested: William Minus */
#define TSENG4          4               /* 4 - Tested: William Minus */
#define VDC600          5               /* 5 - Tested: John Gooding */
#define OAKTECH         6               /* 6 - Untested */
#define VIDEO7          7               /* 7 - Untested */
#define CIRRUS          8               /* 8 - Tested: AAC */
#define PARADISE        9               /* 9 - Tested: John Degner */
#define AHEADA          17              /* A - Untested */
#define AHEADB          18              /* B - Untested */
#define CHIPSTECH       19              /* C - Untested */
#define ATIVGA          20              /* D - Tested: William Earl */
#define EVEREX          21              /* E - Tested: A+B problem - Larry Minton */
#define TRIDENT         22              /* F - Tested: A problem - Alexander Enzmann */
#define VESA            23              /* G - Tested: Charles Marslett/AAC/BP */
#define ATIXL           24              /* H - Tested: Randy Antler */
#define PARADISE24X     25              /* I - Tested: Lutz Kretzschmar */
/* J - N -- 5 more reserved SVGA adapter types */

#define MISCOUT         0x3c2           /* VGA chip msic output reg. addr */
#define SEQUENCER       0x3c4           /* VGA chip sequencer register addr */
#define CRTC            0x3d4           /* VGA chip crt controller reg addr */

/************************
* Constants and tables used throughout
*************************/
static char *vga_names[] =
    {                                   /* POV-Ray command line option: */
    "",                                 /* '0' is autodetect */
    "Standard VGA",                     /*  1  */
    "Simulated SVGA",                   /*  2  */
    "Tseng Labs 3000 SVGA",             /*  3  */
    "Tseng Labs 4000 SVGA",             /*  4  */
    "AT&T VDC600 SVGA",                 /*  5  */
    "Oak Technologies SVGA",            /*  6  */
    "Video 7 SVGA",                     /*  7  */
    "Video 7 Vega (Cirrus) VGA",        /*  8  */
    "Paradise SVGA",                    /*  9  */
    "",                                 /* misc ASCII */
    "",
    "",
    "", /* reserved */
    "",
    "",
    "",
    "Ahead Systems Ver. A SVGA",        /*  A  */
    "Ahead Systems Ver. B SVGA",        /*  B  */
    "Chips & Technologies SVGA",        /*  C  */
    "ATI SVGA",                         /*  D  */
    "Everex SVGA",                      /*  E  */
    "Trident SVGA",                     /*  F  */
    "VESA Standard SVGA",               /*  G  */
    "ATI VGA Wonder XL 32K Color SVGA", /*  H  */
    "Diamond SpeedStar 24X SVGA",       /*  I  */
    "",
    "", /* spare SVGAs */
    "",
    "",
    "",
    "Hercules GWS/TIGA 16-bit",          /*  O   Here on is reserved for non-SVGA cards */
    "Hercules GWS/TIGA 24-bit",          /*  P   TIGA is unsupported by the 32 bit compilers */
    "TrueColor 640 x 480",               /*  Q  */
    "TrueColor 800 x 600",               /*  R  */
    "TrueColor 1024 x 768",              /*  S  */
    };


static unsigned int vptbl[] =          /* CRTC register values for MODE13x */
    {
    0x6b00,     /* horz total */
    0x5901,     /* horz displayed */
    0x5a02,     /* start horz blanking */
    0x8e03U,    /* end horz blanking */
    0x5e04,     /* start h sync */
    0x8a05U,    /* end h sync */
    0x0d06,     /* vertical total */
    0x3e07,     /* overflow */
    0x4009,     /* cell height */
    0xea10U,    /* v sync start */
    0xac11U,    /* v sync end and protect cr0-cr7 */
    0xdf12U,    /* vertical displayed */
    0x2d13,     /* offset */
    0x0014,     /* turn off dword mode */
    0xe715U,    /* v blank start */
    0x0616,     /* v blank end */
    0xe317U     /* turn on byte mode */
};

/* Flags for the Capabilities field */

#define	vbe8BitDAC		0x0001		/* DAC width is switchable to 8 bit	*/
#define	vbeNonVGA		0x0002		/* Controller is non-VGA			*/
#define	vbeBlankRAMDAC	0x0004		/* Programmed DAC with blank bit	*/

#define	VBE20_EXT_SIG				0xFBADFBAD

/* Flags for combining with video modes during mode set */

#define	vbeDontClear	0x8000		/* Dont clear display memory		*/
#define	vbeLinearBuffer	0x4000		/* Enable linear framebuffer mode	*/

/* Flags for the mode attributes returned by VBE_getModeInfo. If
 * vbeMdNonBanked is set to 1 and vbeMdLinear is also set to 1, then only
 * the linear framebuffer mode is available.
 */

#define vbeMdAvailable	0x0001		/* Video mode is available			*/
#define	vbeMdTTYOutput	0x0004		/* TTY BIOS output is supported		*/
#define	vbeMdColorMode	0x0008		/* Mode is a color video mode		*/
#define	vbeMdGraphMode	0x0010		/* Mode is a graphics mode			*/
#define	vbeMdNonVGA		0x0020		/* Mode is not VGA compatible		*/
#define	vbeMdNonBanked	0x0040		/* Banked mode is not supported		*/
#define	vbeMdLinear		0x0080		/* Linear mode supported			*/

/* Flags for save/restore state calls */

#define	vbeStHardware	0x0001		/* Save the hardware state			*/
#define	vbeStBIOS		0x0002		/* Save the BIOS state				*/
#define	vbeStDAC		0x0004		/* Save the DAC state				*/
#define	vbeStSVGA		0x0008		/* Save the SuperVGA state			*/
#define	vbeStAll		0x000F		/* Save all states					*/

/************************
* General Variables 
*************************/
static int whichvga;                  /* POV-Ray's own mode number */
static int vga_512K;                  /* Flag for whether or not >= 512K VGA mem */
/*static int BitsPerPixel;*/                       /* Number of bits per pixel */
static unsigned int map_code;         /* Default map code is 0 */
static unsigned long gran;            /* SVGA granule size (64K by default) */
static unsigned short vesamode;       /* Current VESA BIOS supported mode */

/************************
* Prototypes for local functions
*************************/
static int AutodetectVGA(void);
static void palette_init(void);
static void hsv_to_rgb(DBL, DBL, DBL, unsigned *, unsigned *, unsigned *);
static void rgb_to_hsv(unsigned, unsigned, unsigned, DBL *, DBL *, DBL *);
static int Select_VESA_Mode(void);
static int Paradise_Chkbank(unsigned int, unsigned int);
static void atiplot(int x, int y, int Red, int Green, int Blue);
static void NonVesaNewBank(int NewBank);
static bool SetGraphicsMode(int vesamode);
static void VBE_setBankA(int bank);
static void VBE_setBankAB(int bank);
static void SetPaletteRegister(int Val,uchar Red, uchar Green, uchar Blue);
static int SelectVBEMode(int width,int height);
PRIVATE void ExitVBEBuf(void);
PRIVATE int InitVBE(void);

/*--------------------------- Global Variables -----------------------------*/

#define MAXMODES    128                /* Maximum modes available in list      */

PRIVATE int     VBEVersion;         /* VBE Version Number                    */
PRIVATE int     XRes, YRes;         /* Resolution of video mode used          */
PRIVATE int     BytesPerLine;       /* Logical CRT scanline length            */
PRIVATE int     BitsPerPixel;       /* Color depth for graphics mode        */
PRIVATE int     CurBank;            /* Current read/write bank                */
PRIVATE int     BankShift;          /* Bank granularity adjust factor         */
PRIVATE uchar   *VideoMem;            /* Near pointer to video memory            */
PRIVATE ushort  ModeList[MAXMODES];    /* List of valid VBE modes                */
PRIVATE int     RedPos, RedAdjust;    /* Masks for building TrueColor pixels    */
PRIVATE int     GreenPos, GreenAdjust;
PRIVATE int     BluePos, BlueAdjust;
PRIVATE uchar   RedMask,GreenMask,BlueMask;
PRIVATE uchar   RedDitherMask,GreenDitherMask,BlueDitherMask;
PRIVATE int     WinLeft;            /* Display output window coordinates    */
PRIVATE int     WinTop;
PRIVATE int     WinRight;
PRIVATE int     WinBottom;
PRIVATE int     ImageHeight;        /* Rendered image height                */
PRIVATE int     ImageWidth;            /* Rendered image width                    */
PRIVATE DBL     HeightScale;          /* Fudge factors for SVGA scaling         */
PRIVATE DBL     WidthScale;
PUBLIC  bool    In_Graph_Mode;        /* Graphics mode supresses text display    */
PRIVATE bool    Dithered;            /* True if dithering is on                */
PRIVATE void    (*SetBankPtr)(int bank);/* Pointer to bank switch function    */

/* Dithering error arrays for resolutions up to 2048x??? resolutions. We
 * could dynamically allocate these arrays, but most users wont be
 * previewing huge images anyway.
 */ 
PRIVATE short    r_err[2][2050];
PRIVATE short    g_err[2][2050];
PRIVATE short    b_err[2][2050];

/* The following globals are used by the VESAVBE.C module */

PUBLIC    uint _ASMAPI VESABuf_len = 1024;/* Length of VESABuf                */
PUBLIC    uint _ASMAPI VESABuf_sel = 0;    /* Selector for VESABuf             */
PUBLIC    uint _ASMAPI VESABuf_off;        /* Offset for VESABuf               */
PUBLIC    uint _ASMAPI VESABuf_rseg;        /* Real mode segment of VESABuf     */
PUBLIC    uint _ASMAPI VESABuf_roff;        /* Real mode offset of VESABuf      */

/*-------------------------- Private Functions -----------------------------*/

/* Macro to convert from 8 bit RGB tuples to the framebuffer pixek format */

#define    RGB_COLOR(R,G,B)                                        \
    ((ulong)((R >> RedAdjust) & RedMask) << RedPos)    |            \
    ((ulong)((G >> GreenAdjust) & GreenMask) << GreenPos) |        \
    ((ulong)((B >> BlueAdjust) & BlueMask) << BluePos)            \

void SV_nop(void) {}


/************************
*        Name: MSDOS_Display_Init
*   Called by: Generic part of POV-Ray.
* Description: 
*   Uses board type specified by DisplayFormat or attempts to autodetect
*   board type.  Uses selected opts.PaletteOption to select palette.
*   Chooses the lowest resoltion supported video mode based on
*   the rendered image size passed to it.  
*   Initializes the display and puts it in graphics mode.
************************/
void MSDOS_Display_Init(int width, int height)
  {
   RMREGS inr, outr;
   unsigned long u;
   int i;
   time_t l, lt;
   int show_display_type = FALSE;
   
   ImageHeight = height;      /* requested screen height and width */
   ImageWidth = width;

   MSDOS_Save_Text();

   if (opts.DisplayFormat == '?')
   {
      opts.DisplayFormat = '0';
      show_display_type = TRUE;
   }

   if (opts.DisplayFormat != '0')                 /* if not 0, some display type specified */
   {
      whichvga = (int)(opts.DisplayFormat - '0'); /* de-ASCII-fy selection */
   }
   else 
   {
      whichvga = AutodetectVGA();
      if (show_display_type) 
      {   /* If display format is ? */
         Render_Info("Display detected: (%c) %s Adapter", whichvga + '0', vga_names[whichvga]);
         Render_Info(", with %s 512K RAM\n", vga_512K ? ">=" : "<");
         lt = l = time(&l);
         while (time(&l) < lt + 5);   /* display detected VGA type for 5 secs */
      }
      if (!vga_512K)
      {                           /* not enough RAM for 640 x 480? */
         whichvga = MODE13x;      /* then try for next best mode... */
      }
   }
   
   if ((whichvga != BASIC_VGA) && (whichvga != VESA))
   {
     Warning(0.0,"Future versions of POV-Ray may no longer support mode '%c'='%s'\n",
                  whichvga+'0',vga_names[whichvga]);
     Warning(0.0,"Only VESA VBE standard and generic VGA will be supported.\n");
   }

   if (whichvga == CIRRUS) /* Register Compatible VGA? */
   {
      whichvga = MODE13x; /* MODE13x if > 320x200, else... */
   }

   if ((whichvga <= MODE13x) && 
       ((opts.PaletteOption == HICOLOR) || (opts.PaletteOption == FULLCOLOR)) )
   {
      opts.PaletteOption = P_332;          
   }

   if (ImageHeight <= 200 && ImageWidth <= 320 &&
       opts.PaletteOption != HICOLOR && opts.PaletteOption != FULLCOLOR)
   {
      whichvga = BASIC_VGA;
   }

   if (whichvga == TSENG4 && opts.PaletteOption == HICOLOR) 
   {
      inr.x.ax = 0x10F1;          /* check and see if it's true... */
      PM_int86(0x10, &inr, &inr);
      if ((int) inr.h.al != 0x10 || (int)inr.h.bl == 0) 
         Error("High Color Palette Option Unavailable\n");
   }

   switch (whichvga) 
   {
      case MODE13x:
        inr.x.ax = 0x0013;   /* Setup to VGA 360x480x256 (mode 13X) */
        XRes = 360;    /* Fake 640 mode actually is 360 */
        BytesPerLine = 90;
        break;
      case VDC600:
        inr.x.ax = 0x005E;   /* Setup to VGA 640x400x256 (mode 5EH) */
        YRes = 400;   /* This is the only SVGA card w/400 Lines */
        break;
      case OAKTECH:
        inr.x.ax = 0x0053;   /* Setup to VGA 640x480x256 most SVGAs */
        break;
      case AHEADA:
      case AHEADB:
        inr.x.ax = 0x0061;
        break;
      case EVEREX:
        inr.x.ax = 0x0070;   /* BIOS Mode 0x16 for EV-678? */
        inr.h.bl = 0x30;
        break;
      case ATIVGA:
        if (ImageWidth <= 1024 && ImageHeight <= 768) 
        {
           inr.x.ax = 0x0064;
           XRes = 1024;
           YRes = 768;
           BytesPerLine = 1024;
        }
        if (ImageWidth <= 800 && ImageHeight <= 600) 
        {
           inr.x.ax = 0x0063;
           XRes = 800;
           YRes = 600;
           BytesPerLine = 800;
        }
        if (ImageWidth <= 640 && ImageHeight <= 480)
        {
           inr.x.ax = 0x0062;
           XRes = 640;
           YRes = 480;
           BytesPerLine = 640;
        }
        break;
      case ATIXL:
        inr.x.ax = 0x0072;
        XRes = 640;
        YRes = 480;
        BytesPerLine = 640;
        break;
      case TRIDENT:
        inr.x.ax = 0x005d;
        break;
      case VIDEO7:
        inr.x.ax = 0x6f05;
        inr.h.bl = 0x67;
        break;
      case CHIPSTECH:
        if (ImageHeight <= 400) 
        {
           inr.x.ax = 0x0078;
           YRes = 400;
        }
        else
        {
          inr.x.ax = 0x0079;
        }
        break;
      case PARADISE:
        inr.x.ax = 0x005f;
        break;
      case PARADISE24X:
        if (opts.PaletteOption == HICOLOR)
        {
           if (ImageHeight <= 480 && ImageWidth <= 640) 
           {
              inr.x.ax = 0x0062;
              XRes = 640;
              YRes = 480;
              BytesPerLine = 640;
           }
           else 
           {                     /* 800 by 600 and beyond */
              inr.x.ax = 0x0063;
              XRes = 800;
              YRes = 600;
              BytesPerLine = 800;
           }
           BitsPerPixel = 15;
           RedDitherMask = GreenDitherMask = BlueDitherMask = 0xF8;
           RedAdjust = GreenAdjust = BlueAdjust = 3;
           RedMask   = GreenMask   = BlueMask   =0x001f;
           RedPos =10; GreenPos=5;    BluePos=0;
           BytesPerLine <<=1;
        }
        else                            /* Truecolor mode. Force 640x480 */
        {
           inr.x.ax = 0x0072;
           XRes = 640;
           YRes = 480;
           BytesPerLine = 1920;           /* 3 bytes per pixel */
           BitsPerPixel = 24;
           RedAdjust = GreenAdjust = BlueAdjust = 0;
           RedMask   = GreenMask   = BlueMask   =0x00ff;
           RedPos=0; GreenPos=8; BluePos=16;
        }
        vga_512K = TRUE;                /* Always has 1MB */
        break;
      case TSENG3:
      case TSENG4:
        if (ImageHeight <= 200 && ImageWidth <= 320) 
        {
           inr.x.ax = 0x0013;         /* setup to VGA 320x200 for 32K mode  */
           XRes = 320;          /* allow scaling to run at 320x200 */
           YRes = 200;
           BytesPerLine = 320;
        }
        else 
        {
          if (ImageHeight <= 350 && ImageWidth <= 640) 
          {
             inr.x.ax = 0x002D;
             YRes = 350;
          }
          else 
          {
             if (ImageHeight <= 400 && ImageWidth <= 640 && whichvga == TSENG4) 
             {
                inr.x.ax = 0x0078;
                YRes = 400;
             }
             else 
             {
                if (ImageHeight <= 480 && ImageWidth <= 640) 
                {
                 inr.x.ax = 0x002E;
                }
                else 
                {                          /* 800 by 600 and beyond */
                 inr.x.ax = 0x0030;
                 XRes = 800;
                 YRes = 600;
                 BytesPerLine = 800;
                }
             }
           }
        }
        if ((ImageHeight > 600 || ImageWidth > 800) && whichvga == TSENG4) 
        {
           if (opts.PaletteOption == HICOLOR) 
           {
              inr.x.ax = 0x0030;      /* Limit to 800x600 in HiColor mode */
              XRes = 800;
              YRes = 600;
              BytesPerLine = 800;
           }
           else 
           {
              inr.x.ax = 0x0038;
              XRes = 1024;
              YRes = 768;
              BytesPerLine = 1024;
           }
        }
        if (opts.PaletteOption == HICOLOR && whichvga == TSENG4) 
        {
           if ((int)inr.h.al == 0x78)     /* if it was mode 78 */
             inr.x.bx = 0x2F;             /* make it 2F... */
           else
             inr.x.bx = (unsigned int)inr.h.al;
           inr.x.ax = 0x10F0;
           BitsPerPixel = 15;
           RedDitherMask = GreenDitherMask = BlueDitherMask = 0xF8;
           RedAdjust = GreenAdjust = BlueAdjust = 3;
           RedMask   = GreenMask   = BlueMask   =0x001f;
           RedPos =10; GreenPos=5;   BluePos=0;
           BytesPerLine <<=1;
        }
        break;

      case VESA:
        vesamode = 0x13;
        /* InitVBE confirms presence of VBE and fills modelist and other
           stuff like OEM strings etc. Does not yet set mode. */
        if (InitVBE() >= 0x100)
        {
           vesamode = SelectVBEMode(ImageWidth,ImageHeight);
           vga_512K = TRUE;
        }
    
        if (vesamode == 0x13)
        {
           Warning (0.0,"Could not find appropriate VESA mode.  Using generic VGA.");
           goto no_valid_mode;
        }
        break;
        
      default:                /* BASIC_VGA */
      no_valid_mode:
        if ((opts.PaletteOption == HICOLOR) || (opts.PaletteOption == FULLCOLOR))
        {
           opts.PaletteOption=P_332;
        }
        whichvga = BASIC_VGA;
        inr.x.ax = 0x0013;  /* setup to VGA 320x200x256 (mode 13H) */
        XRes = 320;   /* allow scaling to run at 320x200 */
        YRes = 200;
        BytesPerLine = 320;
        BitsPerPixel = 8;
        break;
     }

#ifdef DEBUG_VIDEO
   Render_Info("Display detected: (%c) %s Adapter", whichvga + '0', vga_names[whichvga]);
   Render_Info(", with %s 512K RAM", vga_512K ? ">=" : "<");
   Render_Info("\n   ax=%04X, bx=%04X, cx=%04X\n", inr.x.ax, inr.x.bx, inr.x.cx);
   Render_Info("   %dx%d, yincr=%d\n", XRes, YRes, BytesPerLine);
   Render_Info("   masks(R,G,B) =%d,%d,%d\n", RedDitherMask, GreenDitherMask, BlueDitherMask);
   lt = l = time(&l);
   while (time(&l) < lt + 5)                          /* display detected VGA type for 5 secs */
     ;
#endif
  
   if (whichvga==VESA)
   {
      if (!SetGraphicsMode(vesamode)) 
      {
         Warning (0.0,"Could not set graphics mode.  Display is turned off.");
         return;
      }
   }
   else
   {
      PM_int86(0x10, &inr, &outr);       /* do the BIOS video mode sel. call */
      VideoMem = (unsigned char *)PM_mapPhysicalAddr(0xA0000L,0xFFFF);

      if (whichvga == MODE13x) 
      {      /* Tweak VGA registers to get higher res! */
         outpw(SEQUENCER, 0x0604);   /* disable chain 4 */
         outpw(SEQUENCER, 0x0F02);   /* allow writes to all planes */
         for (u = 0; u < 43200L; u++) /* clear the whole screen */
         {
            VideoMem[u]=(uchar)0;
         }
         outpw(SEQUENCER, 0x0100);   /* synchronous reset */
         outp(MISCOUT, 0xE7);        /* use 28 mhz dot clock */
         outpw(SEQUENCER, 0x0300);   /* restart sequencer */
         outp(CRTC, 0x11);           /* ctrl register 11, please */
         outp(CRTC+1, inp(CRTC+1) & 0x7f); /* write-prot cr0-7 */

         for (i = 0; i < 17; i++)    /* write CRTC register array */
         {
            outpw(CRTC, vptbl[i]);
         }
      }

      if (whichvga != ATIXL && BitsPerPixel==8)
      {
         palette_init();  /* if we get here it has a normal 256 color palette DAC */
      }
   
      if (whichvga == CHIPSTECH) 
      {                          /* (not sure why this is necessary) */
         outpw(0x46E8, 0x001E);    /* put chip in setup mode */
         outpw(0x103, 0x0080);     /* enable extended registers */
         outpw(0x46E8, 0x000E);    /* take chip out of setup mode */
         outp(0x3D6, 0x10);
      }
   }

   In_Graph_Mode=TRUE;

   /* Set up scaling factors for the current graphics mode */
   if (ImageWidth > XRes) 
   {
      /* Image is larger than graphics resolution in width */
      WinLeft = 0;
      WinRight = XRes;
      WidthScale = ((DBL)XRes / ImageWidth);
   }
   else 
   {
      WinLeft = (XRes - ImageWidth) / 2;
      WinRight = WinLeft + ImageWidth;
   }

   if (ImageHeight > YRes) 
   {
      /* Image is larger than graphics resolution in height */
      WinTop = 0;
      WinBottom = YRes;
      HeightScale = ((DBL)YRes / ImageHeight);
   }
   else 
   {
      WinTop = (YRes - ImageHeight) / 2;
      WinBottom = WinTop + ImageHeight;
   }


   /* Set up dithering if color depth less than 24 bit */
   if ((Dithered = ((BitsPerPixel <= 16) && (ImageWidth <= 2048))) != FALSE) 
   {
      memset(r_err,sizeof(r_err),0);
      memset(g_err,sizeof(g_err),0);
      memset(b_err,sizeof(b_err),0);
   }

   /* Draw a box around the output window */
   box(0,0,ImageWidth-1,ImageHeight-1,255,255,255);

   /* Draw a box around the viewport, if defined */
   if(opts.First_Column > 1 || opts.Last_Column < ImageWidth ||
      opts.First_Line > 1 || opts.Last_Line < ImageHeight)
   {
     box (opts.First_Column, opts.First_Line, opts.Last_Column-1, opts.Last_Line-1,0,255,0);
   }
   return;
  }

/************************
*        Name: AutodetectVGA
*   Called by: MSDOS_Display_Init
* Description:
*   Attempts to autodetect vga type.
*   Returns POV-Ray mode number or BASIC_VGA if unable to detect.
************************/
static int AutodetectVGA()
  {
   unsigned char tmp_byte;
   unsigned int crc_word, tmp_word;
   unsigned int retval;
   RMREGS inr, outr;
   unsigned selector;
   unsigned offset;

/*************
* Test for VESA compatible
**************/
   if (InitVBE() >= 0x100)
   {
     vga_512K = TRUE;
     return(VESA);
   }

/*************
* Test for ATI Wonder
**************/
   PM_mapRealPointer (&selector, &offset, 0xC000, 0x0040);
  
   if ((PM_getByte(selector,offset  )== '3') &&
       (PM_getByte(selector,offset+1)== '1'))
     {
      _disable();                     /* Disable system interrupts */
      outp(0x1CE, 0xBB);
      if (inp(0x1CD) & 0x20)
        vga_512K = TRUE;
      _enable();                      /* Re-enable system interrupts */
      return (ATIVGA);
     }

/*************
* Test for Everex &| Trident 
* 
* There have been reports of a problem with the EVEREX autodetect returning
* TRIDENT.  In fact EVEREX uses a TRIDENT chip set, but apparently there
* is some difference in operation.  There are cryptic diagnostic messages
* such as T0000, etc. printed as a result of the autodetection routines
* to help track down why the error is happening.  If you are experiencing
* problems with EVEREX or TRIDENT, make note of the letter-4 digit code you
* are given.
**************/
   inr.x.ax = 0x7000;
   inr.x.bx = 0;
   CLD;
   PM_int86(0x10, &inr, &outr);
   if (outr.h.al == 0x70)
     {
      if (outr.h.ch & 0xC0)
        vga_512K = TRUE;
      outr.x.dx &= 0xFFF0;
      if (outr.x.dx == 0x6780)
        {
         Render_Info("\nT6780\n");
         return (TRIDENT);
        }
      if (outr.x.dx == 0x2360)
        {
         Render_Info("\nT2360\n");
         return (TRIDENT);
        }
      if (outr.x.dx == 0x6730)        /* EVGA? (No BIOS Page Fn.) */
        {
         Render_Info("\nE6730\n");
         return (EVEREX);
        }
      Render_Info("\nE0000\n");
      return (EVEREX);        /* Newer board with fetchable bankswitch */
     }

/*************
* Test for Trident
**************/
   outp(0x3C4, 0x0B);
   tmp_byte = (unsigned char) inp(0x3C5);
   if ((tmp_byte > 1) && (tmp_byte < 0x10))
     {
      vga_512K = TRUE;
      Render_Info("\nT0000\n");
      return (TRIDENT);
     }

/*************
* Test for Cirrus
**************/
   outp(0x3D4, 0x0C);                          /* assume 3Dx addressing, scrn A start addr hi */
   crc_word = (unsigned int) inp(0x3D5) << 8;  /* save the crc */
   outp(0x3D5, 0);                             /* clear the crc */
   outp(0x3D4, 0x1F);                          /* Eagle ID register */
   tmp_byte = (unsigned char) inp(0x3D5);      /* nybble swap "register" */
   tmp_word = (((tmp_byte & 0x0F) << 4) | ((tmp_byte & 0xf0) >> 4)) << 8;
   outpw(0x3C4, tmp_word | 0x06);              /* disable extensions */
   if (!inp(0x3C5))
     {
      tmp_word = (unsigned int) tmp_byte << 8;
      outpw(0x3C4, tmp_word | 0x06);          /* re-enable extensions */
      if (inp(0x3C5) == 1)
        {
         outpw(0x3D5, crc_word | 0x0c);       /* restore the crc */
         return(CIRRUS);
        }
     }
   outpw(0x3D5, crc_word | 0x0c);       /* restore the crc */

/*************
* Test for Video 7, Note - Vega VGA (Cirrus) will pass this 
*  test so always test Cirrus 1st 
**************/
   inr.x.ax = 0x6F00;
   inr.x.bx = 0;     
   CLD;
   PM_int86(0x10, &inr, &outr);

   if (outr.h.bh == 'V' && outr.h.bl == '7')
     {
      inr.x.ax = 0x6F07;
      CLD;
      PM_int86(0x10, &inr, &outr);
      if ((outr.h.ah & 0x7F) > 1)
        vga_512K = TRUE;
      return (VIDEO7);
     }

/*************
* Test for Paradise &| AT&T VDC600
* The VDC600 is detected as a PARADISE because it uses the PARADISE chip set.  
* The code looks for what is believed to be the model number in the BIOS ROM 
* of the VDC600 to differentiate between the two.  
**************/
   outp(0x3CE, 9);
   if (!inp(0x3CF))
     {
      outpw(0x3CE, 0x050F);         /* Turn off write protect on regs */
      if (Paradise_Chkbank(0,1))             /* if bank 0 and 1 same not para. */
        {                           /* FALSE == banks same... (C) */
         if (Paradise_Chkbank(0, 64))        /* if bank 0 and 64 same only 256K */
           vga_512K = TRUE;
           retval = PARADISE;
           PM_mapRealPointer (&selector, &offset, 0xC000, 0x0039);
           if ((PM_getByte(selector,offset  )== '1') &&
               (PM_getByte(selector,offset+1)== '6'))   /* AT&T p/n 003116 */
             retval = VDC600;       /* a real Paradise is p/n 003145 */
           return (retval);
         }
     }
/*************
* Test for Chips & Tech 
**************/
   inr.x.ax = 0x5F00;
   inr.x.bx = 0;
   CLD;
   PM_int86(0x10, &inr, &outr);
   if (outr.h.al == 0x5F)
     {
      if (outr.h.bh >= 1)
        vga_512K = TRUE;
      return (CHIPSTECH);
     }

/*************
* Test for Tseng 4000 or 3000 Chip 
**************/
   outp(0x3D4, 0x33);
   tmp_word = (unsigned int) inp(0x3D5) << 8;
   outpw(0x3D4, 0x0A33);
   outp(0x3D4, 0x33);
   retval = BASIC_VGA;
   if ((inp(0x3D5) & 0x0F) == 0x0A)
     {
      outpw(0x3D4, 0x0533);
      outp(0x3D4, 0x33);
      if ((inp(0x3D5) & 0x0F) == 0x05)
        {
         retval = TSENG4;
         outpw(0x3D4, tmp_word | 0x33);
         outp(0x3D4, 0x37);
         if ((inp(0x3D5) & 0x0A) == 0x0A)
           vga_512K = TRUE;
         outp(0x3BF, 0x03);          /* Enable access to extended regs */
         outp(0x3D8, 0xA0);
         outp(0x3D8, 0x29);          /* Enable mapping register access */
         outp(0x3D8, 0xA0);
        }
     }
   tmp_byte = (unsigned char) inp(0x3CD);      /* save bank switch reg */
   outp(0x3CD, 0xAA);                          /* test register w/ 0xAA */
   if (inp(0x3CD) == 0xAA)
     {
      outp(0x3CD, 0x55);                      /* test register w/ 0x55 */
      if (inp(0x3CD) == 0x55)
        {
         outp(0x3CD, tmp_byte);              /* restore bank switch reg */
         if (retval != TSENG4)               /* yep, it's a Tseng... */
           retval = TSENG3;
         vga_512K = TRUE;
         return (retval);
        }
     }

/*************
* Test for Ahead A or B chipsets 
**************/
   outpw(0x3CE, 0x200F);
   tmp_byte = (unsigned char) inp(0x3CF);
   if (tmp_byte == 0x21)
     {
      vga_512K = TRUE;              /* Assume all Ahead's have 512K... */
      return (AHEADB);
     }
   if (tmp_byte == 0x20)
     {
      vga_512K = TRUE;
      return (AHEADA);
     }

/*************
* Test for Oak Tech OTI-067 
**************/
   if ((inp(0x3DE) & 0xE0) == 0x60)
     {
      outp(0x3DE, 0x0D);
      if (inp(0x3DF) & 0x80)
        vga_512K = TRUE;
      return(OAKTECH);
     }
   return (BASIC_VGA);            /* Return 1 if Unknown/BASIC_VGA */
  }

/************************
*        Name: Paradise_Chkbank(bank0, bank1)
*   Called by: MSDOS_Display_Init
*    Calls to: 
* Description: 
*   Paradise specific routine to check if 2 banks occupy the same physical RAM
*   Returns TRUE if banks are different RAM
************************/
static int Paradise_Chkbank(unsigned int bank0, unsigned int bank1)
  {
   static unsigned int value = 0x1234;
   unsigned int temp;
   unsigned int oldval0, oldval1;
   unsigned selector;
   unsigned offset;
    
   /* Point out into display RAM */
   PM_mapRealPointer (&selector, &offset, 0xB800, 0x0000);

   outp(0x3CE, 9);
   outp(0x3CF, bank0);       /* save prior video data and write test values */
   oldval0 = PM_getWord(selector, offset);
   PM_setWord(selector, offset, (oldval0 ^ value));
   temp = PM_getWord(selector, offset);
   if (temp != (oldval0 ^ value)) 
     {
      PM_setWord(selector, offset, oldval0);
      return FALSE;          /* No RAM there at all -- can't be 512K */
     }

   outp(0x3CE, 9);
   outp(0x3CF, bank1);      /* save prior video data and write test values */
   oldval1 = PM_getWord(selector, offset);
   PM_setWord(selector, offset, (oldval1 ^ value));
   temp = PM_getWord(selector, offset);
   if (temp != (oldval1 ^ value)) 
     {
      PM_setWord(selector, offset, oldval1);
      return FALSE;          /* No RAM there at all -- can't be 512K */
     }

   if (temp != oldval0) 
     {
      PM_setWord(selector,offset,oldval1);
      outp(0x3CE, 9);
      outp(0x3CF, bank0);
      PM_setWord(selector,offset,oldval0);
      return TRUE;                    /* pages cannot be the same RAM */
     }

   outp(0x3CE, 9);
   outp(0x3CF, bank0);
   temp = PM_getWord(selector, offset);
   outp(0x3CE, 9);
   outp(0x3CF, bank1);
   PM_setWord(selector,offset,oldval1);
   outp(0x3CE, 9);
   outp(0x3CF, bank0);
   PM_setWord(selector,offset,oldval0);

   if (temp == oldval0)
     return FALSE;                 /* pages are the same RAM */
   else
     return TRUE;                  /* independent values, so not same RAM */
  }

/************************
*        Name: MSDOS_Display_Finished
*   Called by: Generic part of POV-Ray.
*    Calls to: 
* Description: 
*   On IBM this routine does very little.  It just implements the
*   pause-on-exit feature.  The real stuff happens in MSDOS_Display_Close.
************************/
void MSDOS_Display_Finished ()
{
  if (opts.Options & PROMPTEXIT)
  {
     if ((opts.FrameSeq.FrameNumber >= opts.FrameSeq.FinalFrame) ||
         (Stop_Flag==TRUE))
     {
        fprintf(stderr,"\007\007");    /* long beep */
        fflush(stderr);
        if (In_Graph_Mode)
        {
           while(!kbhit())             /* wait for key hit */
             ;
           if (!getch())               /* get another if ext. scancode */
             getch();
        }
     }
   }
}

/************************
*        Name: MSDOS_Display_Close
*   Called by: Generic part of POV-Ray.
*    Calls to:
* Description:
*   Shuts down graphics mode and returns to 80x25 text mode.
************************/
void MSDOS_Display_Close()
  {
   RMREGS regs;

   /* If we're not in Graphics mode, don't do the switch */
   if (In_Graph_Mode)
   {
     regs.x.ax = 0x0003;
     PM_int86(0x10, &regs, &regs);

     MSDOS_Restore_Text();
   }

   In_Graph_Mode=FALSE;
   return;
}

/************************
*        Name: MSDOS_Display_Plot
*   Called by: Generic part of POV-Ray.
*    Calls to:
* Description:
*   Plots an R,G,B pixel to location x,y
************************/
void MSDOS_Display_Plot (int x, int y, unsigned char Red, unsigned char Green, unsigned char Blue)

  {
   unsigned long color;
   int bank;
   unsigned int svga_word, offset;
   unsigned long address;
   uchar   *memPtr;
   DBL h, s, v;
   int i, r, g, b, re, ge, be, ri, gi, bi;

   if (x == opts.First_Column)              /* first pixel on this line? */
   {
      if (Dithered)
      {
         for(i=0;(unsigned short)i < ImageWidth+2;i++)
         {
            r_err[(y+1)&1][i+WinLeft] = 0;
            g_err[(y+1)&1][i+WinLeft] = 0;
            b_err[(y+1)&1][i+WinLeft] = 0;
         }
      }
   }

   y += WinTop;
   x += WinLeft;

   /* Scale pixels down to current resolution */
   if (ImageHeight > YRes) 
   {
      y = (int)(y * HeightScale);
   }
   if (ImageWidth > XRes) 
   {
      x = (int)(x * WidthScale);
   }

   if (whichvga == ATIXL)
   {
      atiplot(x, y, (int)Red, (int)Green, (int)Blue);
      return;
   }
 
   /* Plot the pixel to the framebuffer */
   switch (BitsPerPixel) 
   {
      case 32:
        /* 32 bit TrueColor modes */
        address = y * BytesPerLine + x*4;
        memPtr = VideoMem + (address & 0xFFFF);
        bank = (address >> 16);
        if (bank != CurBank)
        {
           SetBankPtr(CurBank = bank);
        }
        *((ulong*)memPtr) = RGB_COLOR(Red,Green,Blue);
        break;
      case 24:
        /* 24 bit TrueColor modes */
        address = y * BytesPerLine + x*3;
        offset = (address & 0xFFFF);
        memPtr = VideoMem + offset;
        bank = (address >> 16);
        if (bank != CurBank)
        {
           SetBankPtr(CurBank = bank);
        }
        /* Note that we use the RGB_COLOR macro which converts the
         * pixel into the framebuffer format, and then we write that
         * to the display.
         */
        color = RGB_COLOR(Red,Green,Blue);
        if (offset >= 0xFFFD) 
        {
           /* Pixel crosses a bank boundary */
           *memPtr = color;
           if (++offset > 0xFFFF) 
           {
              SetBankPtr(++CurBank);
              memPtr = VideoMem;
              offset = 0;
           }
           else
           {
              memPtr++;
           }
           *memPtr = (unsigned)color >> 8;
           if (++offset > 0xFFFF) 
           {
              SetBankPtr(++CurBank);
              memPtr = VideoMem;
              offset = 0;
           }
           else
           {
              memPtr++;
           }
           *memPtr = (unsigned)color >> 16;
        }
        else 
        {
           /* Standard pixel plotting */
           *memPtr = (uchar)color;
           *(memPtr+1) = (uchar)(color >> 8);
           *(memPtr+2) = (uchar)(color >> 16);
        }
        break;
      
      case 15:
      case 16:
        address = y * BytesPerLine + x*2;
        memPtr = VideoMem + (address & 0xFFFF);
        bank = (address >> 16);
        if (bank != CurBank)
        {
           SetBankPtr(CurBank = bank);
        }

        if (Dithered)
        {  
           /* Add previous error terms. */
           r = ri = (int)Red   + (int)r_err[y&1][x+1]/16;
           g = gi = (int)Green + (int)g_err[y&1][x+1]/16;
           b = bi = (int)Blue  + (int)b_err[y&1][x+1]/16;
        }
        else
        {
           ri = (int)Red;
           gi = (int)Green;
           bi = (int)Blue;
        }

        if(ri > 255) ri = 255;  /* This clamping is a good idea */
        if(gi > 255) gi = 255;  /* even if no dithering is done.*/
        if(bi > 255) bi = 255;
        if(ri < 0) ri = 0;
        if(gi < 0) gi = 0;
        if(bi < 0) bi = 0;

        /* Set the pixel */
        *((ushort*)memPtr) = (ushort)RGB_COLOR(ri,gi,bi);

        if (Dithered)  /* Now propigate errors into dither array */
        {
           re = r - (ri & RedDitherMask);
           ge = g - (gi & GreenDitherMask);
           be = b - (bi & BlueDitherMask);
           r_err[y&1][x+2]     += (signed char)(7*re);
           r_err[y&1][x]       += (signed char)re;
           r_err[(y+1)&1][x+1] += (signed char)(5*re);
           r_err[(y+1)&1][x+2] += (signed char)(3*re);
           g_err[y&1][x+2]     += (signed char)(7*ge);
           g_err[y&1][x]       += (signed char)ge;
           g_err[(y+1)&1][x+1] += (signed char)(5*ge);
           g_err[(y+1)&1][x+2] += (signed char)(3*ge);
           b_err[y&1][x+2]     += (signed char)(7*be);
           b_err[y&1][x]       += (signed char)be;
           b_err[(y+1)&1][x+1] += (signed char)(5*be);
           b_err[(y+1)&1][x+2] += (signed char)(3*be);
        }
        break;
      case 8:
        switch (opts.PaletteOption)
        {
           case GREY:
             /* RGB are already set the same, so really, any color will do... */
             color = (unsigned char)(Green >> 2);
             break;
           case P_332:
             if (Dithered)
             {
                r = (int)Red   + (int)r_err[y&1][x+1];
                g = (int)Green + (int)g_err[y&1][x+1];
                b = (int)Blue  + (int)b_err[y&1][x+1];

                ri = (r+18)/36;
                gi = (g+18)/36;
                bi = (b+42)/84;
 
                if (ri > 7) ri=7;
                if (gi > 7) gi=7;
                if (bi > 3) bi=3;
                if (ri < 0) ri=0;
                if (gi < 0) gi=0;
                if (bi < 0) bi=0;

                color = (unsigned char)(((ri<<5) + (gi<<2) + bi));

                re = r - ((255 * ri) / 7);
                ge = g - ((255 * gi) / 7);
                be = b - ((255 * bi) / 3);

                r_err[y&1][x+2] += (signed char)((7*re)/16);
                r_err[y&1][x] += (signed char)(re/16);
                r_err[(y+1)&1][x+1] += (signed char)((5*re)/16);
                r_err[(y+1)&1][x+2] += (signed char)((3*re)/16);
                g_err[y&1][x+2] += (signed char)((7*ge)/16);
                g_err[y&1][x] += (signed char)(ge/16);
                g_err[(y+1)&1][x+1] += (signed char)((5*ge)/16);
                g_err[(y+1)&1][x+2] += (signed char)((3*ge)/16);
                b_err[y&1][x+2] += (signed char)((7*be)/16);
                b_err[y&1][x] += (signed char)(be/16);
                b_err[(y+1)&1][x+1] += (signed char)((5*be)/16);
                b_err[(y+1)&1][x+2] += (signed char)((3*be)/16);
             }
             else
             {
                color = ((Red & 0xE0) | ((Green & 0xE0) >> 3) | ((Blue & 0xC0) >> 6));
             }
             break;
           default:
             /* Translate RGB value to nearest of 256 palette Colors (by HSV) */
             rgb_to_hsv((unsigned)Red,(unsigned)Green,(unsigned)Blue, &h, &s, &v);
             if (s < 0.20) 
             {           /* black or white if no saturation of color... */
                if (v < 0.25)
                {
                   color = 0;        /* black */
                }
                else 
                {
                   if (v > 0.8)
                   {
                      color = 64;       /* white */
                   }
                   else 
                   {
                      if (v > 0.5)
                      {
                         color = 192;      /* lite grey */
                      }
                      else
                      {
                         color = 128;      /* dark grey */
                      }
                   }
                }
             }
             else 
             {
                color = (unsigned char) (64.0 * ((DBL)(h)) / 360.0);
 
                if (!color)
                   color = 1;        /* avoid black, white or grey */
                if (color > 63)
                   color = 63;       /* avoid same */
                if (v > 0.50)
                   color |= 0x80;    /* colors 128-255 for high inten. */
                if (s > 0.50)         /* more than half saturated? */
                   color |= 0x40;    /* color range 64-128 or 192-255 */
             }
        }
        switch (whichvga)    /* decide on (S)VGA bank switching scheme to use */
        {
           case BASIC_VGA:        /* none */
             address = BytesPerLine * y + x;
             break;

           case MODE13x:          /* faked */
             svga_word = 1 << (x & 3);       /* form bit plane mask */
             svga_word = (svga_word << 8) | 2;
             outpw(SEQUENCER, svga_word);    /* tweak the sequencer */
             address = BytesPerLine * y + (x >> 2);
             break;

           default:               /* actual bank switch for all SVGA cards */
             address=((unsigned long)BytesPerLine) * y + x;
             bank = (address >> 16);
             if (bank != CurBank)
             {
                SetBankPtr(CurBank = bank);
             }
             break;
        }
        memPtr = VideoMem + (address & 0xFFFF);
        *memPtr = (uchar)color;
   }
}

/************************
*        Name: NonVesaNewBank
*   Called by: MSDOS_Display_Plot
*    Calls to: 
* Description: Sets VGA bank to NewBank
************************/
static void NonVesaNewBank(int NewBank)
{
   register unsigned char tmp_byte, tmp_byte1;
   register unsigned int tmp_word;
   RMREGS regs;
   static unsigned char xlateT3[] = {0x40, 0x49, 0x52, 0x5B, 0x64, 0x6D,
            0x76, 0x7F};
   static unsigned char xlateT4[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
            0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

   switch (whichvga)
   {
      case VDC600:                    /* AT&T VDC 600 */
        tmp_byte = (unsigned char) (NewBank << 4); /* was >> 12... */
        outpw(0x03CE,0x050F);
        outp(0x03CE,0x09);
        outp(0x03CF, tmp_byte);
        break;
      case OAKTECH:                   /* Oak Technology OTI-067 */
        tmp_byte = (unsigned char)(NewBank & 0x0F);
        outp(0x3DF, (tmp_byte << 4) | tmp_byte);
        break;
      case AHEADA:                    /* Ahead Systems Ver A */
        outpw(0x3CE, 0x200F);       /* enable extended registers */
        tmp_byte = (unsigned char)(inp(0x3CC) & 0xDF);  /* bit 0 */
        if (NewBank & 1)
          tmp_byte |= 0x20;
        outp(0x3C2, tmp_byte);
        outp(0x3CF, 0);                /* bits 1, 2, 3 */
        tmp_word = (unsigned int)((NewBank >> 1 )|(inp(0x3D0) & 0xF8));
        outpw(0x3CF, tmp_word << 8);
        break;
      case AHEADB:                    /* Ahead Systems Ver B */
        outpw(0x3CE, 0x200F);       /* enable extended registers */
        tmp_word = (unsigned int)((NewBank << 4) | NewBank);
        outpw(0x3CF, (tmp_word << 8) | 0x000D);
        break;
      case EVEREX:                    /* Everex SVGA's */
        outp(0x3C4, 8);
        if (NewBank & 1)
          tmp_word = (unsigned int)(inp(0x3C5) | 0x80);
        else 
          tmp_word = (unsigned int)(inp(0x3C5) & 0x7F);
        outpw(0x3C4, (tmp_word << 8) | 0x0008);
        tmp_byte = (unsigned char)(inp(0x3CC) & 0xDF);
        if (!(NewBank & 2))
          tmp_byte |= 0x20;
        outp(0x3C2, tmp_byte);
        break;
      case ATIVGA:                    /* ATI VGA Wonder (and the XL) */
        outp(0x1CE, 0xB2);
        tmp_word = (unsigned int)((NewBank << 1) | (inp(0x1CF) & 0xE1));
        outpw(0x1CE, (tmp_word << 8) | 0x00B2);
        break;
      case TRIDENT:
        outp(0x3CE, 6);             /* set page size to 64K */
        tmp_word = (unsigned int)(inp(0x3CF) | 4) << 8;
        outpw(0x3CE, tmp_word | 0x0006);
        outp(0x3C4, 0x0b);          /* switch to BPS mode */
        inp(0x3C5);                 /* dummy read?? */
        tmp_word = (unsigned int)(NewBank ^ 2) << 8;
        outpw(0x3C4, tmp_word | 0x000E);
        break;
      case VIDEO7:                    /* Video-7 VRAM, FastRAM SVGA cards */
        tmp_byte1 = tmp_byte = (unsigned char)(NewBank & 0x0F);
        outpw(0x3C4, 0xEA06);
        tmp_word = (unsigned int)(tmp_byte & 1) << 8;
        outpw(0x3C4, tmp_word | 0x00F9);
        tmp_byte &= 0x0C;
        tmp_word = (unsigned int)((tmp_byte >> 2) | tmp_byte) << 8;
        outpw(0x3C4, tmp_word | 0x00F6);
        tmp_word |= (inp(0x3C5) & 0xF0) << 8;
        outpw(0x3C4, tmp_word | 0x00F6);
        tmp_byte = (unsigned char)((tmp_byte1 << 4) & 0x20);
        outp(0x3C2, (inp(0x3CC) & 0xDF) | tmp_byte);
        break;
      case CHIPSTECH:                 /* Chips & Technology VGA Chip Set */
        outp(0x3D7, NewBank << 2); /* this is all that's necessary?? */
        break;
      case PARADISE24X:               /* Diamond Speedstar 24X  */
      case PARADISE:                  /* Paradise, Professional, Plus */
        outpw(0x3CE, 0x050F);       /* turn off VGA reg. write protect */
        tmp_word = (unsigned int)(NewBank << 4) << 8;
        outpw(0x3CE, tmp_word | 0x0009);
        break;
      case TSENG3:                    /* Tseng 3000 - Orchid, STB, etc. */
        outp(0x3CD, xlateT3[NewBank & 0x07]);
        break;
      case TSENG4:                    /* Tseng 4000 - Orchid PD+, etc. */
        outp(0x3CD, xlateT4[NewBank & 0x0F]);
        break;
      case VESA:                      /* VESA standard mode bank switch */
        regs.x.ax = 0x4F05;         /* VESA BIOS bank switch call */
        regs.x.dx = NewBank;
        regs.x.bx = map_code;       /* Map code 0 or 1? */
        PM_int86(0x10, &regs, &regs);  /* Do the video BIOS interrupt */
   }
   return;
}

static void SetPaletteRegister(int Val,uchar Red, uchar Green, uchar Blue)
/****************************************************************************
*
* Function:        SetPaletteRegister
* Parameters:    val        - Palette index to set
*                Red        - Red value for palette index
*                Green    - Green value for palette index
*                Blue    - Blue value for palette index
*
* Description:  Sets a VGA palette register entry on VBE 1.2 or lower
*                devices.
*
****************************************************************************/
{
    RMREGS    regs;

    regs.x.ax = 0x1010;             /* Set one palette register function     */
    regs.x.bx = Val;                /* the palette register to set (color#)    */
    regs.h.dh = (Red & 0x3F);        /* set the gun values (6 bits ea.)        */
    regs.h.ch = (Green & 0x3F);
    regs.h.cl = (Blue & 0x3F);
    PM_int86(0x10, &regs, &regs);
}

/************************
*        Name: hsv_to_rgb
*   Called by: 
*    Calls to: 
* Description: 
*  Conversion from Hue, Saturation, Value to Red, Green, and Blue and back.
   From "Computer Graphics", Donald Hearn & M. Pauline Baker, p. 304 
   This will deliver approximate colorings using HSV values for the selection.
   The palette map is divided into 4 parts - upper and lower half generated
   with full and half "value" (intensity), respectively.  These halves are
   further halved by full and half saturation values of each range (pastels).
   There are three constant colors, black, white, and grey.  They are used
   when the saturation is low enough that the hue becomes undefined, and which
   one is selected is based on a simple range map of "value".  Usage of the
   palette is accomplished by converting the requested color RGB into an HSV
   value.  If the saturation is too low (< .25) then black, white or grey is
   selected.  If there is enough saturation to consider looking at the hue,
   then the hue range of 1-63 is scaled into one of the 4 palette quadrants
   based on its "value" and "saturation" characteristics.
************************/
static void hsv_to_rgb(DBL hue, DBL s, DBL v, unsigned *r, unsigned *g, unsigned *b)
   /* hue (0.0-360.0) s and v from 0.0-1.0) */
   /* values from 0 to 63 */
   {
   register DBL i, f, p1, p2, p3;
   register DBL xh;
   register DBL nr, ng, nb;     /* rgb values of 0.0 - 1.0 */

   if (hue == 360.0)
      hue = 0.0;                /* (THIS LOOKS BACKWARDS BUT OK) */

   xh = hue / 60.0;             /* convert hue to be in 0,6     */
   i = floor(xh);               /* i = greatest integer <= h    */
   f = xh - i;                  /* f = fractional part of h     */
   p1 = v * (1 - s);
   p2 = v * (1 - (s * f));
   p3 = v * (1 - (s * (1 - f)));

   switch ((int) i)
      {
      case 0:
         nr = v;
         ng = p3;
         nb = p1;
         break;
      case 1:
         nr = p2;
         ng = v;
         nb = p1;
         break;
      case 2:
         nr = p1;
         ng = v;
         nb = p3;
         break;
      case 3:
         nr = p1;
         ng = p2;
         nb = v;
         break;
      case 4:
         nr = p3;
         ng = p1;
         nb = v;
         break;
      case 5:
         nr = v;
         ng = p1;
         nb = p2;
         break;
      default:
         nr = ng = nb = 0;
        }

   *r = (unsigned)(nr * 63.0); /* Normalize the values to 63 */
   *g = (unsigned)(ng * 63.0);
   *b = (unsigned)(nb * 63.0);

   return;
   }


/************************
*        Name: rgb_to_hsv
*   Called by: 
*    Calls to: 
* Description: 
*   
************************/
static void rgb_to_hsv(unsigned r, unsigned g, unsigned b, DBL *h, DBL *s, DBL *v)
   {
   register DBL m, r1, g1, b1;
   register DBL nr, ng, nb;             /* rgb values of 0.0 - 1.0 */
   register DBL nh = 0.0, ns, nv;       /* hsv local values */

   nr = (DBL) r / 255.0;
   ng = (DBL) g / 255.0;
   nb = (DBL) b / 255.0;

   nv = max (nr, max (ng, nb));
   m = min (nr, min (ng, nb));

   if (nv != 0.0)                /* if no value, it's black! */
      ns = (nv - m) / nv;
   else
      ns = 0.0;                  /* black = no colour saturation */

   if (ns == 0.0)                /* hue undefined if no saturation */
   {
      *h = 0.0;                  /* return black level (?) */
      *s = 0.0;
      *v = nv;
      return;
   }

   r1 = (nv - nr) / (nv - m);    /* distance of color from red   */
   g1 = (nv - ng) / (nv - m);    /* distance of color from green */
   b1 = (nv - nb) / (nv - m);    /* distance of color from blue  */

   if (nv == nr)
   {
      if (m == ng)
         nh = 5. + b1;
      else
         nh = 1. - g1;
   }

   if (nv == ng)
      {
      if (m == nb)
         nh = 1. + r1;
      else
         nh = 3. - b1;
      }

   if (nv == nb)
      {
      if (m == nr)
         nh = 3. + g1;
      else
         nh = 5. - r1;
      }

   *h = nh * 60.0;        /* return h converted to degrees */
   *s = ns;
   *v = nv;
   return;
   }

/************************
*        Name: box
*   Called by: 
*    Calls to: 
* Description: Draw a thin box in graphics mode
************************/
void box(int x1, int y1, int x2, int y2, unsigned int r, unsigned int g, unsigned int b)
{
   int i, d;
   
   d=Dithered;
   Dithered=FALSE;
   for (i = x1; i <= x2; i++)
   {
        MSDOS_Display_Plot(i, y1, r,g,b);
        MSDOS_Display_Plot(i, y2, r,g,b);
   }
   for (i = y1; i <= y2; i++)
   {
        MSDOS_Display_Plot(x1, i, r,g,b);
        MSDOS_Display_Plot(x2, i, r,g,b);
   }
   Dithered=d;
   return;
}

/************************
*        Name: MSDOS_Display_Plot_Rect
*   Called by: Generic part of POV-Ray
*    Calls to: MSDOS_display_plot
* Description: Plots rectangular area with the same pixel.
*               Used for mosaic preview.
************************/
void MSDOS_Display_Plot_Rect  (int x1, int y1, int x2, int y2, unsigned char Red, unsigned char Green, unsigned char Blue)

{
   int i,j, d;
   
   d=Dithered;
   Dithered=FALSE;   
   for (j=y1; j<=y2; j++)
     for (i=x1; i<=x2; i++)
       MSDOS_Display_Plot(i,j,Red,Green,Blue);
   Dithered=d;
}
      
static int SelectVBEMode(int width,int height)
/****************************************************************************
*
* Function:     SelectVBEMode
* Parameters:    width    - Width of the image being rendered
*                height    - Height of the image being rendered
* Returns:      VBE mode number to use, or mode 13h if none found.
*
* Description:  Searches for an appropriate VBE mode number given the
*                current resolution of the specified image. The color depth
*                is forced from the command line, and we always look for
*                the highest possible mode from the list.
*
****************************************************************************/
{
   VBE_modeInfo    modeInfo;
   int                i,bits;
   int                bestMode = 0x13;
   int                bestXRes = 0,bestYRes = 0;
   DBL aspect;

   if (opts.PaletteOption == FULLCOLOR)
   {
      bits = 32;
   }
   else 
   {
      if (opts.PaletteOption == HICOLOR)
      {
         bits = 16;
      }
      else
      {
         bits = 8;
      }
   }

   for (i = 0; ModeList[i] != 0xFFFF; i++) 
   {
#ifdef DEBUG_VESA
Debug_Info("\nChecking VESA ModeList[%d]= %#X\n",i,ModeList[i]);
#endif

      /* Find mode info for mode */
      if (!VBE_getModeInfo(ModeList[i],&modeInfo))
         continue;

#ifdef DEBUG_VESA
Debug_Info("Our bpp=%d but VESA_mi.BPP=%d ",bits,(int)modeInfo.BitsPerPixel);
#endif
         
      /* Check if we have a match in color resolution */
      if (modeInfo.BitsPerPixel == bits ||
         (modeInfo.BitsPerPixel == 24 && bits == 32) ||
         (modeInfo.BitsPerPixel == 15 && bits == 16)) 
      {
#ifdef DEBUG_VESA
Debug_Info("which is a match\n");
Debug_Info("We want to match a resolution of  W=%d H=%d\n",ImageWidth,ImageHeight);
Debug_Info("The best we have found so far was W=%d H=%d in mode %#X\n",bestXRes,bestYRes,bestMode);
Debug_Info("We're now going to try out values W=%d H=%d in mode %#X\n",
           modeInfo.XResolution,modeInfo.YResolution,ModeList[i]) ;
#endif
         /* Filter out non 1:1 aspect ratio modes except for 320x200 modes. */
         aspect = (DBL)modeInfo.XResolution * 0.75 / modeInfo.YResolution;
         if (modeInfo.XResolution != 320 && 
             modeInfo.YResolution != 200 && 
             aspect != 1.0) 
         {
#ifdef DEBUG_VESA
Debug_Info("Rejected mode because aspect ratio was %lf\n",aspect);
#endif
            continue;
         }
         /* If the best mode so far is smaller than the rendering size
          * and if this mode is bigger than the best mode then make
          * this the best mode so far.
          */
         if ((bestYRes < height || bestXRes < width) &&
             modeInfo.XResolution >= bestXRes &&
             modeInfo.YResolution >= bestYRes)
         {
            bestMode = ModeList[i];
            bestXRes = modeInfo.XResolution;
            bestYRes = modeInfo.YResolution;
#ifdef DEBUG_VESA
Debug_Info("New best mode now W=%d H=%d in mode %#X\n",bestXRes,bestYRes,bestMode);
#endif
         }
         else
         {
            /* Here the best mode so far is big enough.  If this mode
               is smaller than the best so far and this mode is still
               big enough then make it the best so far. */
            if (modeInfo.XResolution <= bestXRes && 
                modeInfo.YResolution <= bestYRes && 
                modeInfo.XResolution >= width    && 
                modeInfo.YResolution >= height) 
            {
               bestMode = ModeList[i];
               bestXRes = modeInfo.XResolution;
               bestYRes = modeInfo.YResolution;
#ifdef DEBUG_VESA
Debug_Info("New best mode now W=%d H=%d in mode %#X\n",bestXRes,bestYRes,bestMode);
#endif
            }
         }
      }
   }
#ifdef DEBUG_VESA
Debug_Info("The chosen mode is W=%d H=%d B=%d in mode %#X\n",bestXRes,bestYRes,bits,bestMode);
#endif
    return bestMode;
}

/************************
*        Name: MSDOS_Video_Init_Vars
*   Called by: 
*    Calls to: 
* Description: Initalize variables used in IBM.C
*   
************************/
void MSDOS_Video_Init_Vars(void)
{
   XRes  = 640;
   YRes = 480;
   BytesPerLine  = 640;
   WinLeft = 0;
   WinTop = 0;
   whichvga = BASIC_VGA;
   vga_512K = FALSE;
   BitsPerPixel = 8;
   CurBank = 255;
   map_code = 0;
   gran = 65536L;
   HeightScale   = 1.0;
   WidthScale    = 1.0;
   In_Graph_Mode   = FALSE;
   Dithered        = TRUE;
   SetBankPtr = NonVesaNewBank;
}

void MSDOS_Startup(void)
{
  MSDOS_Video_Init_Vars();
#ifdef FANCY_TEXT
  MSDOS_Text_Init_Vars();
#endif
}

/************************
*        Name: 
*   Called by: 
*    Calls to: 
* Description: Fill VGA 256 color palette with colors.
************************/
static void palette_init()
  {
   register unsigned m;
   unsigned r, g, b;
   register DBL hue, sat, val;

   if (opts.PaletteOption == GREY)       /* B/W Video Mod */
     {
      for (m = 1; m < 64; m++)     /* for the 1st 64 colors... */
        SetPaletteRegister (m, m, m, m); /* set m to rgb value */
      for (m = 64; m < 256; m++)     /* for the remaining, at full value */
        SetPaletteRegister (m, 63, 63, 63);
      return;
     }
   if (opts.PaletteOption == P_332) /* 332 Video Mod */
     {
      for (r=0;r<8;r++)
        for (g=0;g<8;g++)
          for (b=0;b<4;b++) 
            {
             m = (r * 32 + g * 4 + b);
             SetPaletteRegister (m, r * 9, g * 9, b * 21);
            }
      return;
     }

    /* otherwise, use the classic HSV palette scheme */

    SetPaletteRegister (0, 0, 0, 0);   /* make palette register 0 black */
    SetPaletteRegister (64, 63, 63, 63);   /* make palette register 64 white */
    SetPaletteRegister (128, 31, 31, 31);  /* make register 128 dark grey */
    SetPaletteRegister (192, 48, 48, 48);  /* make register 192 lite grey */

    for (m = 1; m < 64; m++)                /* for the 1st 64 colors... */
      {
       sat = 0.5;                          /* start with the saturation and intensity low */
       val = 0.5;
       hue = 360.0 * ((DBL)(m)) / 64.0;    /* normalize to 360 */
       hsv_to_rgb (hue, sat, val, &r, &g, &b);
       SetPaletteRegister (m, r, g, b);  /* set m to rgb value */

       sat = 1.0;                          /* high saturation and half intensity (shades) */
       val = 0.50;
       hue = 360.0 * ((DBL)(m)) / 64.0;    /* normalize to 360 */
       hsv_to_rgb (hue, sat, val, &r, &g, &b);
       SetPaletteRegister (m + 64, r, g, b);  /* set m + 64 */

       sat = 0.5;                          /* half saturation and high intensity (pastels) */
       val = 1.0;
       hue = 360.0 * ((DBL)(m)) / 64.0;    /* normalize to 360 */
       hsv_to_rgb (hue, sat, val, &r, &g, &b);
       SetPaletteRegister (m + 128, r, g, b); /* set m + 128 */

       sat = 1.0;                          /* normal full HSV set at full intensity */
       val = 1.0;
       hue = 360.0 * ((DBL)(m)) / 64.0;    /* normalize to 360 */
       hsv_to_rgb (hue, sat, val, &r, &g, &b);
       SetPaletteRegister (m + 192, r, g, b); /* set m + 192 */
      }
    return;
   }

/************************
*        Name: atiplot
*   Called by:
*    Calls to:
* Description: ATI VGA Wonder XL 32K color display plot
************************/
static void atiplot(int x, int y, int r, int g, int b)
  {
   char plane;
   unsigned offset, tmp_word;
   unsigned long address;
   uchar   *memPtr;

   r/=8;
   g/=8;
   b/=8;
   address = y * 1280L + x * 2L;
   offset = (unsigned)(address % 65536L);
   memPtr = VideoMem + offset;
   plane = (char)(address / 65536L);
   _disable();

   /* NOTE: this used to be the atibank() funct. - AAC */
   outp(0x1CE, 0xB2);                      
   tmp_word = (unsigned int)((plane << 1) | (inp(0x1CF) & 0xE1));
   outpw(0x1CE, (tmp_word << 8) | 0x00B2);
   
   *((ushort*)memPtr) = (ushort) (r * 1024 + g * 32 + b);
   _enable();
   return;
}


#if !__STDC__
#if !defined (__ZTC__) && !defined(__BORLANDC__) && !defined(__TURBOC__)
 /* BCC has srand but doesn't set __STDC__ same as Zortech */

/* ANSI Standard psuedo-random number generator */
#ifndef __cplusplus
int rand(void);
void srand(int);
static unsigned long int next = 1;

int rand()
   {
   next = next * 1103515245L + 12345L;
   return ((int) (next / 0x10000L) & 0x7FFF);
   }

void srand(seed)
   int seed;
   {
   next = (unsigned long int)seed;
   }

#endif
#endif
#endif


/* Math Error exception struct format:
        int type;               - exception type - see below
        char _far *name;        - name of function where error occured
        long double arg1;       - first argument to function
        long double arg2;       - second argument (if any) to function
        long double retval;     - value to be returned by function
*/

#ifndef __cplusplus
#ifdef __WATCOMC__
#define EDOM    7       /* MSC is 33 */
#define ERANGE  8       /* MSC is 34 */
int matherr(e)
   struct exception *e;
#else
int _cdecl matherr(e)
   struct exception *e;
#endif
   {
      /* Since we are just making pictures, not keeping nuclear power under
         control - it really isn't important if there is a minor math problem.
         This routine traps and ignores them.  Note: the most common one is
         a DOMAIN error coming out of "acos". */
    /*
    switch (e->type) 
    {
         case DOMAIN   : Debug_Info("DOMAIN error in '%s'\n", e->name); break;
         case SING     : Debug_Info("SING   error in '%s'\n", e->name); break;
         case OVERFLOW : Debug_Info("OVERFLOW error in '%s'\n", e->name); break;
         case UNDERFLOW: Debug_Info("UNDERFLOW error in '%s'\n", e->name); break;
         case TLOSS    : Debug_Info("TLOSS error in '%s'\n", e->name); break;
         case PLOSS    : Debug_Info("PLOSS error in '%s'\n", e->name); break;
#ifdef EDOM
         case EDOM     : Debug_Info("EDOM error in '%s'\n", e->name); break;
#endif
#ifdef ERANGE
         case ERANGE   : Debug_Info("ERANGE error in '%s'\n", e->name); break;
#endif
         default       : Debug_Info("Unknown math error in '%s'\n",e->name);break;
         }
    */
   return (1);  /* Indicate the math error was corrected... */
   }
#endif

PRIVATE int InitVBE(void)
/****************************************************************************
*
* Function:     InitVBE
* Returns:      VBE version number for the SuperVGA (0 if no SuperVGA).
*
* Description:  Performs initialisation for VBE based devices.
*
****************************************************************************/
{
    VBE_vgaInfo        vgaInfo;
    int                i;
    ushort           *p;

    /* Detect the install VBE driver and version */
    if ((VBEVersion = VBE_detect(&vgaInfo)) == 0)
        return 0;

    /* Copy relevent information from the mode block into our globals.
     * Note that the video mode list _may_ be built in the information
     * block that we have passed, so we _must_ copy this from here
     * into our our storage if we want to continue to use it.
     */
    for (i = 0, p = vgaInfo.VideoModePtr; *p != 0xFFFF; p++)
        ModeList[i++] = *p;
    ModeList[i] = -1;

    /* Return the detected VBE version */
    return VBEVersion;
}

static bool SetGraphicsMode(int vesamode)
/****************************************************************************
*
* Function:     SetGraphicsMode
* Parameters:    vesamode    - VBE mode number to initialize
*
* Description:  Initializes the specified graphics mode.
*
****************************************************************************/
{
    VBE_modeInfo    modeInfo;

    if (vesamode < 0x100 && vesamode != 0x13)
        return false;
    if (!VBE_setVideoMode(vesamode))
        return false;

    /* Initialise global variables for current video mode dimensions    */
    if (vesamode == 0x13) 
    {
       /* Special case for VGA mode 13h */
       XRes = 320;
       YRes = 200;
       BytesPerLine = 320;
       BitsPerPixel = 8;
       BankShift = 0;
#ifdef DEBUG_VESA
Debug_Info("VBE but using mode 13h\n");
#endif
    }
    else 
    {
       /* Get graphics mode information */
       VBE_getModeInfo(vesamode,&modeInfo);
       XRes = modeInfo.XResolution-1;
       YRes = modeInfo.YResolution-1;
       BytesPerLine = modeInfo.BytesPerScanLine;
       BitsPerPixel = modeInfo.BitsPerPixel;
       BankShift = 0;
       while ((64 >> BankShift) != modeInfo.WinGranularity)
           BankShift++;
#ifdef DEBUG_VESA
Debug_Info("Mode=%04X, W=%d H=%d BPL=%d BPP=%d BS=%02X\n",
vesamode,XRes,YRes,BytesPerLine,BitsPerPixel,BankShift);
#endif
    }
    CurBank = -1;

    /* Emulate RGB modes using a 3 3 2 palette arrangement by default */
    RedMask   = 0x7;    RedPos = 5;     RedAdjust = 5;
    GreenMask = 0x7;    GreenPos = 2;   GreenAdjust = 5;
    BlueMask  = 0x3;    BluePos = 0;    BlueAdjust = 6;

    if (vesamode != 0x13 && modeInfo.MemoryModel == vbeMemRGB) 
    {
       /* Save direct color info mask positions etc */
       RedMask = (0xFF >> (RedAdjust = 8 - modeInfo.RedMaskSize));
       RedPos = modeInfo.RedFieldPosition;
       GreenMask = (0xFF >> (GreenAdjust = 8 - modeInfo.GreenMaskSize));
       GreenPos = modeInfo.GreenFieldPosition;
       BlueMask = (0xFF >> (BlueAdjust = 8 - modeInfo.BlueMaskSize));
       BluePos = modeInfo.BlueFieldPosition;
#ifdef DEBUG_VESA
Debug_Info("MemMod = vbeMemRGB\n");
#endif
    }

    /* Create dither error masks */
    RedDitherMask = (RedMask << RedAdjust);
    GreenDitherMask = (GreenMask << GreenAdjust);
    BlueDitherMask = (BlueMask << BlueAdjust);

#ifdef DEBUG_VESA
Debug_Info("RM=%04X RP=%04X RD=%04X\n",RedMask,RedPos,RedDitherMask);
Debug_Info("GM=%04X GP=%04X GD=%04X\n",GreenMask,GreenPos,GreenDitherMask);
Debug_Info("BM=%04X BP=%04X BD=%04X\n",BlueMask,BluePos,BlueDitherMask);
#endif
    /* Set up a pointer to the appopriate bank switching code to use */
    if (vesamode == 0x13) 
    {
       SetBankPtr = (void (* ) (int))SV_nop;
       VideoMem = (unsigned char *)PM_mapPhysicalAddr(0xA0000L,0xFFFF);
    }
    else 
    {
       if ((modeInfo.WinAAttributes & 0x7) != 0x7)
       {
          SetBankPtr = VBE_setBankAB;
#ifdef DEBUG_VESA
Debug_Info("VBE_setBankAB\n");
#endif
       }
       else
       {
          SetBankPtr = VBE_setBankA;
#ifdef DEBUG_VESA
Debug_Info("VBE_setBankA\n");
#endif
       }
       VideoMem = (unsigned char *)VBE_getBankedPointer(&modeInfo);
    }

#ifdef DEBUG_VESA
Debug_Info("VideoMem %012X \n",VideoMem);
#endif
    if (BitsPerPixel == 8) 
    {
       palette_init();
    }

    return true;
}

static void VBE_setBankA(int bank)
/****************************************************************************
*
* Function:     VBE_setBankA
* Parameters:    int    bank
*
* Description:  Sets the read/write bank on systems that have a single read
*                and write bank.
*
****************************************************************************/
{
    RMREGS    regs;

    bank <<= BankShift;             /* Adjust to window granularity     */
    regs.x.ax = 0x4F05;
    regs.x.bx = 0;
    regs.x.dx = bank;
    PM_int86(0x10, &regs, &regs);
}

static void VBE_setBankAB(int bank)
/****************************************************************************
*
* Function:     VBE_setBankA
* Parameters:    int    bank
*
* Description:  Sets the read/write bank on systems that have dual read
*                and write banks. This requires two calls to the VBE driver
*                and is slower than the single bank version.
*
****************************************************************************/
{
    RMREGS    regs;

    bank <<= BankShift;             /* Adjust to window granularity     */
    regs.x.ax = 0x4F05;
    regs.x.bx = 0;
    regs.x.dx = bank;
    PM_int86(0x10, &regs, &regs);
    regs.x.ax = 0x4F05;
    regs.x.bx = 1;
    regs.x.dx = bank;
    PM_int86(0x10, &regs, &regs);
}

void _PUBAPI VBE_initRMBuf(void)
/****************************************************************************
*
* Function:        VBE_initRMBuf
*
* Description:    Initialises the VBE transfer buffer in real mode memory.
*                This routine is called by the VESAVBE.C module every time
*                it needs to use the transfer buffer, so we simply allocate
*                it once and then return.
*
****************************************************************************/
{
    if (!VESABuf_sel) {
        /* Allocate a global buffer for communicating with the VESA VBE */
        if (!PM_allocRealSeg(VESABuf_len, &VESABuf_sel, &VESABuf_off,
                &VESABuf_rseg, &VESABuf_roff))
            exit(1);
        atexit(ExitVBEBuf);
        }
}

PRIVATE void ExitVBEBuf(void)
{
    PM_freeRealSeg(VESABuf_sel,VESABuf_off);
}

