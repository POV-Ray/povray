/* MesaDrawingArea.h -- Public header file for the Mesa widget
   Copyright (C) 1995 Thorsten.Ohl @ Physik.TH-Darmstadt.de

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

   $Id: MesaDrawingArea.h,v 1.1.1.1 1999/08/19 00:55:42 jtg Exp $
 */

#ifndef _MesaDrawingArea_h
#define _MesaDrawingArea_h

#include <GL/GLwDrawA.h>

#define GLwNximage "ximage"
#define GLwCXImage "XImage"
#define GLwNshareLists "shareLists"
#define GLwCShareLists "ShareLists"
#define GLwNshareListsWith "shareListsWith"
#define GLwCShareListsWith "ShareListsWith"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif

#ifdef __GLX_MOTIF
typedef struct _MesaMDrawingAreaClassRec *MesaMDrawingAreaWidgetClass;
typedef struct _MesaMDrawingAreaRec *MesaMDrawingAreaWidget;
extern WidgetClass mesaMDrawingAreaWidgetClass;
void GLwMMakeCurrent (Widget w);
#else
typedef struct _MesaDrawingAreaClassRec *MesaDrawingAreaWidgetClass;
typedef struct _MesaDrawingAreaRec *MesaDrawingAreaWidget;
extern WidgetClass mesaDrawingAreaWidgetClass;
void GLwMakeCurrent (Widget w);
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _MesaDrawingArea_h */
