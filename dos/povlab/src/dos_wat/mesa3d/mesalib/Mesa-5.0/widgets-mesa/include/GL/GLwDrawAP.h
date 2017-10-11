/* GLwDrawingAreaP.h -- Mesa GL Widget for X11 Toolkit Programming
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

   $Id: GLwDrawAP.h,v 1.1.1.1 1999/08/19 00:55:42 jtg Exp $
 */

#ifndef _GLwDrawingAreaP_h
#define _GLwDrawingAreaP_h

#ifdef __GLX_MOTIF
#include <Xm/PrimitiveP.h>
#include <GL/GLwMDrawA.h>
#else
#include <GL/GLwDrawA.h>
#endif

#ifndef __GNUC__
#define __FUNCTION__ "???"
#endif

#ifdef DEBUG
#define GLwDebug(_widget) \
   (((GLwDrawingAreaWidget)_widget)->glwDrawingArea.debug)
#define LOG(w) \
   if (GLwDebug(w)) \
     fprintf (stderr, "%s():%s(%d) of %s (class %s, address %p)\n", \
	   __FUNCTION__, __FILE__, __LINE__, \
	   (w)->core.name, XtClass(w)->core_class.class_name, (w))
#else
#define LOG(w)
#endif

struct cmap_cache_entry
{
  Display *dpy;
  VisualID vid;
  Colormap cmap;
};

struct cmap_cache
{
  struct cmap_cache_entry *entries;
  int next_entry;
  int allocated_entries;
};

typedef struct _GLwDrawingAreaClassPart
  {
    caddr_t extension;
    struct cmap_cache colormaps;
  }
GLwDrawingAreaClassPart;

#ifdef __GLX_MOTIF

/* Motif class record */
typedef struct _GLwMDrawingAreaClassRec
  {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    GLwDrawingAreaClassPart glwDrawingArea_class;
  }
GLwMDrawingAreaClassRec;

extern GLwMDrawingAreaClassRec glwMDrawingAreaClassRec;

#else

/* Xt class record */
typedef struct _GLwDrawingAreaClassRec
  {
    CoreClassPart core_class;
    GLwDrawingAreaClassPart glwDrawingArea_class;
  }
GLwDrawingAreaClassRec;

extern GLwDrawingAreaClassRec glwDrawingAreaClassRec;

#endif

/************************************************************************
 *                      New Mesa Widget Resources                        *
 ************************************************************************/

typedef struct
  {
    int *attribList;		/* Attribute list for use in glXChooseVisual */
    XVisualInfo *visualInfo;	/* Chosen visual */
    Boolean installColormap;
    Boolean allocateBackground;
    Boolean allocateOtherColors;
    Boolean installBackground;
    Boolean rgba;		/* True color mode or index mode */
    Boolean doublebuffer;	/* Double buffering? */
    Boolean stereo;		/* Stereo mode? */
    XtCallbackList ginitCallback;
    XtCallbackList resizeCallback;
    XtCallbackList exposeCallback;
    XtCallbackList inputCallback;
    int bufferSize;		/* Smallest index buffer size (for index mode) */
    int level;			/* Overlay level */
    int auxBuffers;		/* Number of auxiliary buffers */
    int redSize;		/* Bits/channel red */
    int greenSize;		/* Bits/channel green */
    int blueSize;		/* Bits/channel blue */
    int alphaSize;		/* Bits/channel coverage */
    int depthSize;		/* Bits for Z-Buffer */
    int stencilSize;		/* Bits for stencil buffer */
    int accumRedSize;		/* Bits/channel red accu-buffer */
    int accumGreenSize;		/* Bits/channel green accu-buffer */
    int accumBlueSize;		/* Bits/channel blue accu-buffer */
    int accumAlphaSize;		/* Bits/channel coverage accu-buffer */

    Boolean debug;
    Boolean attribList_allocated;    /* Bookkeeping  */
    Boolean visualInfo_allocated;
  }
GLwDrawingAreaPart;


#ifdef __GLX_MOTIF

/* Motif-based widget */
typedef struct _GLwMDrawingAreaRec
  {
    CorePart core;
    XmPrimitivePart primitive;
    GLwDrawingAreaPart glwDrawingArea;
  }
GLwMDrawingAreaRec;

#else

/* Xt-based widget */
typedef struct _GLwDrawingAreaRec
  {
    CorePart core;
    GLwDrawingAreaPart glwDrawingArea;
  }
GLwDrawingAreaRec;

#endif

#endif /* _GLwDrawingAreaP_h */
