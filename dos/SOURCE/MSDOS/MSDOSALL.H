/****************************************************************************
*                   msdosall.h
*
*  This file contains headers used by MSDOSxxx.C but not by the generic
*  parts of POV-Ray.
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

#ifndef MSDOSALL_H
#define MSDOSALL_H

#include "console.h"


void MSDOS_Video_Init_Vars(void);
void MSDOS_Text_Init_Vars(void);
void MSDOS_Save_Text(void);
void MSDOS_Restore_Text(void);
#ifdef __cplusplus
extern PUBLIC  bool    In_Graph_Mode;        /* Graphics mode supresses text display    */
#else
extern int In_Graph_Mode;
#endif

#ifdef __GO32__
  #undef  outpw
  #undef  outp
  #undef  inp

  #define outpw( portid,v )  outportw( portid,v )
  #define outp( portid,v )  outportb( portid,v )
  #define inp( portid ) inportb( portid )

  #define getch() getkey()
#endif



#endif
