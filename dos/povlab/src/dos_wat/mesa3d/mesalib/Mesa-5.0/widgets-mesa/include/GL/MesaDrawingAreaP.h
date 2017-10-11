/* MesaDrawingAreaP.h -- Private header file for the Mesa widget
   Copyright (C) 1995, 1996 Thorsten.Ohl @ Physik.TH-Darmstadt.de

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

   $Id: MesaDrawingAreaP.h,v 1.4 2002/01/24 02:57:02 brianp Exp $
 */

#ifndef _MesaDrawingAreaP_h
#define _MesaDrawingAreaP_h

#include <GL/GLwDrawAP.h>
#include <GL/MesaDrawingArea.h>
#include <GL/xmesa.h>

typedef struct
  {
    char *RCS_Id;
    Widget lists_root;
  }
MesaDrawingAreaClassPart;

#ifdef __GLX_MOTIF
typedef struct _MesaMDrawingAreaClassRec
  {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    GLwDrawingAreaClassPart glwDrawingArea_class;
    MesaDrawingAreaClassPart mesaDrawingArea_class;
  }
MesaMDrawingAreaClassRec;
extern MesaMDrawingAreaClassRec mesaMDrawingAreaClassRec;
#else
typedef struct _MesaDrawingAreaClassRec
  {
    CoreClassPart core_class;
    GLwDrawingAreaClassPart glwDrawingArea_class;
    MesaDrawingAreaClassPart mesaDrawingArea_class;
  }
MesaDrawingAreaClassRec;
extern MesaDrawingAreaClassRec mesaDrawingAreaClassRec;
#endif

typedef struct
  {
    Boolean ximage;
    XMesaVisual visual;
    XMesaContext context;
    XMesaBuffer buffer;
    Boolean share_lists;
    Widget share_lists_with;
  }
MesaDrawingAreaPart;

#ifdef __GLX_MOTIF
typedef struct _MesaMDrawingAreaRec
  {
    CorePart core;
    XmPrimitivePart primitive;
    GLwDrawingAreaPart glwDrawingArea;
    MesaDrawingAreaPart mesaDrawingArea;
  }
MesaMDrawingAreaRec;
#else
typedef struct _MesaDrawingAreaRec
  {
    CorePart core;
    GLwDrawingAreaPart glwDrawingArea;
    MesaDrawingAreaPart mesaDrawingArea;
  }
MesaDrawingAreaRec;
#endif

#ifdef __GLX_MOTIF
#define __DRAWINGAREAWIDGET MesaMDrawingAreaWidget
#define __DRAWINGAREACLASS  
#else
#define __DRAWINGAREAWIDGET MesaDrawingAreaWidget
#define __DRAWINGAREACLASS  
#endif


#define MesaRGBA(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->glwDrawingArea.rgba)
#define MesaDoublebuffer(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->glwDrawingArea.doublebuffer)
#define MesaVisual(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->mesaDrawingArea.visual)
#define MesaContext(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->mesaDrawingArea.context)
#define MesaBuffer(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->mesaDrawingArea.buffer)
#define MesaXImage(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->mesaDrawingArea.ximage)
#define MesaShareLists(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->mesaDrawingArea.share_lists)
#define MesaShareListsWith(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->mesaDrawingArea.share_lists_with)

#define MesaAlphaSize(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->glwDrawingArea.alphaSize)
#define MesaDepthSize(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->glwDrawingArea.depthSize)
#define MesaStencilSize(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->glwDrawingArea.stencilSize)
#define MesaAccumRedSize(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->glwDrawingArea.accumRedSize)
#define MesaAccumGreenSize(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->glwDrawingArea.accumGreenSize)
#define MesaAccumBlueSize(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->glwDrawingArea.accumBlueSize)
#define MesaAccumAlphaSize(_widget) \
   (((__DRAWINGAREAWIDGET)_widget)->glwDrawingArea.accumAlphaSize)

#ifdef __GLX_MOTIF
#define MesaListsRoot \
   (mesaMDrawingAreaClassRec.mesaDrawingArea_class.lists_root)
#else
#define MesaListsRoot \
   (mesaDrawingAreaClassRec.mesaDrawingArea_class.lists_root)
#endif

/*#define AVOID_MESABUFFER_INTERFACE 1*/

#endif /* _MesaDrawingAreaP_h */
