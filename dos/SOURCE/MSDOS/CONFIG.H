/****************************************************************************
*                   source\msdos\config.h
*
*  This file contains the machine specific defines for a 32-bit protected
*  mode msdos compile under several different compilers. 
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
* Rev. 6/7/94
*****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

/* Comment out the defines below if you are not using the fancy text 
 * routines in MSDOSTXT.C or video in MSDOSVID.C
 */
#define FANCY_TEXT 
#define FANCY_VIDEO

/*****************
 *
 * Note: These compiler specific defines should only contain items
 * that are likely to affect the generic parts of POV-Ray.  Any video
 * specific stuff belongs in MSDOSVID.C.  Any text specific stuff belongs
 * in MSDOSTXT.C.  Any items common to video and text belongs in MSDOSALL.H.
 ****************/

#ifdef __WATCOMC__                        /* Watcom C/C++ C32 */
  #include <direct.h>
  #include <unistd.h>
  #include <float.h>
  #include "ztimer.h"
  #ifdef __CAUSEWAY__
    #define COMPILER_VER ".msdos.wat-cwa"
  #else
    #define COMPILER_VER ".msdos.wat-d4g"
  #endif
  #define PRECISION_TIMER_AVAILABLE           1
  #define PRECISION_TIMER_INIT                ZTimerInit () ;
  #define PRECISION_TIMER_START               LZTimerOn() ;
  #define PRECISION_TIMER_STOP                LZTimerOff() ;
  #define PRECISION_TIMER_COUNT               LZTimerCount() ;
  #define FIX_WATCOM_BUG                      if (l<0) Fix_Watcom_Bug(temp4);
  void Fix_Watcom_Bug(char *s);
#endif


#ifdef __ZTC__                             /* Zortech 3.1 & Symantec 6.x */  
  #define QSORT_FUNCT_PARM const void *
  #define COMPILER_VER ".msdos.ztc"
  #ifdef __SC__
    #undef COMPILER_VER
    #define COMPILER_VER ".msdos.sym"
  #endif
  #ifdef __LARGE__
    #undef COMPILER_VER
    #define COMPILER_VER ".msdos.ztc"
    #ifdef __SC__
      #undef COMPILER_VER
      #define COMPILER_VER ".msdos.symc"
    #endif
  #endif
#endif

#ifdef __GO32__
 void main(int, char **);
 double fmod(double, double);
 #define COMPILER_VER ".msdos.gcc"
 #define SCANF_EOF 0
#endif

#ifdef __BORLANDC__                       /* Borland C/C++ */       
 #ifdef __cplusplus
    #define bool unsigned char
 #endif
 #define COMPILER_VER ".msdos.bcc"
 #define IFF_SWITCH_CAST (long)
 #define QSORT_FUNCT_PARM const void _FAR *
 #define QSORT_FUNCT_RET  int _Cdecl
 #include <direct.h>
 #include "ztimer.h"
 #define PRECISION_TIMER_AVAILABLE           1
 #define PRECISION_TIMER_INIT                ZTimerInit () ;
 #define PRECISION_TIMER_START               LZTimerOn() ;
 #define PRECISION_TIMER_STOP                LZTimerOff() ;
 #define PRECISION_TIMER_COUNT               LZTimerCount() ;
 #define CDECL _cdecl
 #undef  outpw
 #undef  outp
 #undef  inp
 /* Needed for ZLIB & LIBPNG */
 #define MSDOS
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <conio.h>  

/* Un comment at most one of these options
#define MEM_TAG     - Enables memory tag debugging
#define MEM_TRACE   
*/
#define MEM_RECLAIM - Enables garbage collection
#define MEM_STATS 1	

#define FILENAME_SEPARATOR '\\'

/*#define EPSILON 1.0e-5*/

#define DEFAULT_OUTPUT_FORMAT   't'
#define NEW_LINE_STRING "\r\n"

/* defines for machine-specific PaletteOption settings */
#define NORMAL  '0'
#define GREY    'G'
#define HSV     '0'
#define P_332   '3'
#define HICOLOR 'H'             /* 16 bits/pixel "high" color */
#define FULLCOLOR 'T'           /* 24 bits/pixel "true" color */

#ifndef COMPILER_VER	/* if we haven't figured this out yet... */
#define COMPILER_VER ".unsupported"
#endif

#define RENAME_FILE(orig,new) rename(orig,new)

void MSDOS_Startup(void);
int MSDOS_System(char *s);
void MSDOS_Process_Povray_Ini(char *s);
void MSDOS_Process_Env(void);
void MSDOS_Other_Credits(void);

#define STARTUP_POVRAY MSDOS_Startup();
#define POV_SYSTEM(s) MSDOS_System(s);
#define PROCESS_POVRAY_INI MSDOS_Process_Povray_Ini(argv[0]);
#define READ_ENV_VAR MSDOS_Process_Env();
#define PRINT_OTHER_CREDITS if (argc>1) {Print_Authors(); MSDOS_Other_Credits(); }
#define CONST const

#ifdef FANCY_TEXT

 void MSDOS_Banner(char *s);
 void MSDOS_Warning(char *s);
 void MSDOS_Render_Info(char *s);
 void MSDOS_Status_Info(char *s);
 void MSDOS_Debug_Info(char *s);
 void MSDOS_Fatal(char *s);
 void MSDOS_Statistics(char *s);
 void MSDOS_Finish(int n);
 void MSDOS_Wait_Key(void);
 void MSDOS_Test_Abort(void);
 int MSDOS_getch(void);

 #define POV_BANNER(s) MSDOS_Banner(s);
 #define POV_WARNING(s) MSDOS_Warning(s);
 #define POV_RENDER_INFO(s) MSDOS_Render_Info(s);
 #define POV_STATUS_INFO(s) MSDOS_Status_Info(s);
 #define POV_DEBUG_INFO(s) MSDOS_Debug_Info(s);
 #define POV_FATAL(s) MSDOS_Fatal(s);
 #define POV_STATISTICS(s) MSDOS_Statistics(s);
 #define FINISH_POVRAY(n) MSDOS_Finish(n);

 #define WAIT_FOR_KEYPRESS MSDOS_Wait_Key();
 #define GET_KEY(x) { (x) = MSDOS_getch(); }
 #define TEST_ABORT MSDOS_Test_Abort();

#else

 #define WAIT_FOR_KEYPRESS getch();
 #define GET_KEY(x) { (x) = getch(); }
 #define TEST_ABORT if (opts.Options & EXITENABLE) { if (kbhit()) { Stop_Flag = TRUE; getch(); }}
 #define FINISH_POVRAY(n) exit(n);

#endif

#ifdef FANCY_VIDEO

 void MSDOS_Display_Init (int w, int h);
 void MSDOS_Display_Finished (void);
 void MSDOS_Display_Close (void);
 void MSDOS_Display_Plot (int x, int y, unsigned char r, 
  unsigned char g, unsigned char b);
 void MSDOS_Display_Plot_Rect (int x1, int x2, int y1, int y2, 
  unsigned char r, unsigned char g, unsigned char b);
 void box (int x1, int x2, int y1, int y2, 
  unsigned int r, unsigned int g, unsigned int b);
 
 #define POV_DISPLAY_INIT(w,h) MSDOS_Display_Init ((w), (h));
 #define POV_DISPLAY_FINISHED MSDOS_Display_Finished();
 #define POV_DISPLAY_CLOSE MSDOS_Display_Close();
 #define POV_DISPLAY_PLOT(x,y,r,g,b,a) MSDOS_Display_Plot((x),(y),(r),(g),(b));
 #define POV_DISPLAY_PLOT_RECT(x1,x2,y1,y2,r,g,b,a) MSDOS_Display_Plot_Rect((x1),(x2),(y1),(y2),(r),(g),(b));
 #define POV_DISPLAY_PLOT_BOX(x1,x2,y1,y2,r,g,b,a) box((x1),(x2),(y1),(y2),(r),(g),(b));

#endif

#endif
