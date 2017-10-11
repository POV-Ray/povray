/* GLwDrawA.h -- Mesa GL Widget for X11 Toolkit Programming
   Copyright (C) 1995 by
     Jeroen van der Zijp <jvz@cyberia.cfdrc.com>
     Thorsten Ohl <Thorsten.Ohl@Physik.TH-Darmstadt.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   $Id: GLwDrawA.h,v 1.1.1.1 1999/08/19 00:55:42 jtg Exp $
 */

#ifndef _GLwDrawingArea_h
#define _GLwDrawingArea_h

#include <GL/gl.h>
#include <GL/glx.h>


/*
   New resources available in this widget:

   Resource name       Resource class     Resource type   Initial Value
   =============       ==============     =============   =============
   attribList          AttribList         int*            NULL
   visualInfo          VisualInfo         VisualInfo      NULL
   installColormap     InstallColormap    Boolean         TRUE
   allocateBackground  AllocateColors     Boolean         FALSE
   allocateOtherColors AllocateColors     Boolean         FALSE
   installBackground   InstallBackground  Boolean         TRUE
   ginitCallback       Callback           Pointer         NULL
   exposeCallback      Callback           Pointer         NULL
   inputCallback       Callback           Pointer         NULL
   resizeCallback      Callback           Pointer         NULL
   rgba                Rgba               Boolean         FALSE
   doublebuffer        Doublebuffer       Boolean         FALSE
   stereo              Stereo             Boolean         FALSE
   bufferSize          BufferSize         int             0
   level               Level              int             0
   auxBuffers          AuxBuffers         int             0
   redSize             ColorSize          int             1
   greenSize           ColorSize          int             1
   blueSize            ColorSize          int             1
   alphaSize           AlphaSize          int             0
   depthSize           DepthSize          int             0
   stencilSize         StencilSize        int             0
   accumRedSize        AccumColorSize     int             0
   accumGreenSize      AccumColorSize     int             0
   accumBlueSize       AccumColorSize     int             0
   accumAlphaSize      AccumAlphaSize     int             0

 */

/* Resource names */
#define GLwNdebug		"debug"
#define GLwNattribList          "attribList"
#define GLwNvisualInfo          "visualInfo"
#define GLwNinstallColormap     "installColormap"
#define GLwNallocateBackground  "allocateBackground"
#define GLwNallocateOtherColors "allocateOtherColors"
#define GLwNinstallBackground   "installBackground"
#define GLwNexposeCallback      "exposeCallback"
#define GLwNginitCallback       "ginitCallback"
#define GLwNresizeCallback      "resizeCallback"
#define GLwNinputCallback       "inputCallback"
#define GLwNbufferSize          "bufferSize"
#define GLwNlevel               "level"
#define GLwNrgba                "rgba"
#define GLwNdoublebuffer        "doublebuffer"
#define GLwNstereo              "stereo"
#define GLwNauxBuffers          "auxBuffers"
#define GLwNredSize             "redSize"
#define GLwNgreenSize           "greenSize"
#define GLwNblueSize            "blueSize"
#define GLwNalphaSize           "alphaSize"
#define GLwNdepthSize           "depthSize"
#define GLwNstencilSize         "stencilSize"
#define GLwNaccumRedSize        "accumRedSize"
#define GLwNaccumGreenSize      "accumGreenSize"
#define GLwNaccumBlueSize       "accumBlueSize"
#define GLwNaccumAlphaSize      "accumAlphaSize"

/* Resource classes */
#define GLwCDebug		"Debug"

#define GLwCAttribList          "AttribList"
#define GLwCVisualInfo          "VisualInfo"

#define GLwCInstallColormap     "InstallColormap"
#define GLwCAllocateColors      "AllocateColors"
#define GLwCInstallBackground   "InstallBackground"

#define GLwCCallback            "Callback"

#define GLwCBufferSize          "BufferSize"
#define GLwCLevel               "Level"
#define GLwCRgba                "Rgba"
#define GLwCDoublebuffer        "Doublebuffer"
#define GLwCStereo              "Stereo"
#define GLwCAuxBuffers          "AuxBuffers"
#define GLwCColorSize           "ColorSize"
#define GLwCAlphaSize           "AlphaSize"
#define GLwCDepthSize           "DepthSize"
#define GLwCStencilSize         "StencilSize"
#define GLwCAccumColorSize      "AccumColorSize"
#define GLwCAccumAlphaSize      "AccumAlphaSize"

/* Resource type for VisualInfo */
#define GLwRVisualInfo          "VisualInfo"


#ifdef __GLX_MOTIF

/* Motif-based widget */
typedef struct _GLwMDrawingAreaClassRec *GLwMDrawingAreaWidgetClass;
typedef struct _GLwMDrawingAreaRec *GLwMDrawingAreaWidget;

extern WidgetClass glwMDrawingAreaWidgetClass;

#else

/* Xt-based widget */
typedef struct _GLwDrawingAreaClassRec *GLwDrawingAreaWidgetClass;
typedef struct _GLwDrawingAreaRec *GLwDrawingAreaWidget;

extern WidgetClass glwDrawingAreaWidgetClass;

#endif

/* Callback Reasons */
#ifdef __GLX_MOTIF
#define GLwCR_EXPOSE XmCR_EXPOSE
#define GLwCR_RESIZE XmCR_RESIZE
#define GLwCR_INPUT  XmCR_INPUT
#else
#define GLwCR_EXPOSE 38
#define GLwCR_RESIZE 39
#define GLwCR_INPUT  40
#endif
#define GLwCR_GINIT  32135

/************************************************************************
 *                Callback Structure for Mesa Widget                     *
 ************************************************************************/

typedef struct
  {
    int reason;
    XEvent *event;
    Dimension width, height;
  }
GLwDrawingAreaCallbackStruct;


/************************************************************************
 *                       Mesa Widget Functions                           *
 ************************************************************************/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif

/* front ends to glXMakeCurrent and glXSwapBuffers */
#ifdef _NO_PROTO
  extern void GLwDrawingAreaMakeCurrent ();
  extern void GLwDrawingAreaSwapBuffers ();
#else
  extern void GLwDrawingAreaMakeCurrent (Widget w, GLXContext ctx);
  extern void GLwDrawingAreaSwapBuffers (Widget w);
#endif

#ifdef __GLX_MOTIF
#ifdef _NO_PROTO
  extern Widget GLwCreateMDrawingArea ();
#else
  extern Widget GLwCreateMDrawingArea (Widget parent, char *name, ArgList arglist, Cardinal argcount);
#endif
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _GLwDrawingArea_h */

