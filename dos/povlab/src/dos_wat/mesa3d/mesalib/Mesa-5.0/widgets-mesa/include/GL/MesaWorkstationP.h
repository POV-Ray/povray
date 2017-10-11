/* MesaWorkstationP.h -- Private header file for the Mesa Workstation widget
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

   $Id: MesaWorkstationP.h,v 1.1.1.1 1999/08/19 00:55:42 jtg Exp $
 */

#ifndef _MesaWorkstationP_h
#define _MesaWorkstationP_h

#include <GL/MesaDrawingAreaP.h>
#include <GL/MesaWorkstation.h>

typedef enum { NOPROJ, PROJ_MATRIX, PROJ_LIST, FRUSTUM, ORTHO } projtype;

typedef	struct
{
  GLdouble left, right;
  GLdouble bottom, top;
  GLdouble near, far;
}
volume;
  
typedef struct
{
  projtype type;
  GLuint list;
  union
    {
      GLdouble m[16];
      volume vol;
    }
  u;
}
projection;

typedef enum { NOVIEW, VIEW_MATRIX, VIEW_LIST, LOOK_AT, POLAR } viewtype;

typedef	struct
{
  GLdouble eyex, eyey, eyez;
  GLdouble ctrx, ctry, ctrz;
  GLdouble upx, upy, upz;
}
look_at;
  
typedef	struct
{
  GLdouble r;
  GLdouble theta;
  GLdouble phi;
}
polar;

typedef struct
{
  viewtype type;
  GLuint list;
  union
    {
      GLdouble m[16];
      look_at look_at;
      polar polar;
    }
  u;
}
view;

typedef struct
  {
    char *RCS_Id;
  }
MesaWorkstationClassPart;

#ifdef __GLX_MOTIF
typedef struct _MesaMWorkstationClassRec
  {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    GLwDrawingAreaClassPart glwDrawingArea_class;
    MesaDrawingAreaClassPart mesaDrawingArea_class;
    MesaWorkstationClassPart mesaWorkstation_class;
  }
MesaMWorkstationClassRec;
extern MesaMWorkstationClassRec mesaMWorkstationClassRec;
#else
typedef struct _MesaWorkstationClassRec
  {
    CoreClassPart core_class;
    GLwDrawingAreaClassPart glwDrawingArea_class;
    MesaDrawingAreaClassPart mesaDrawingArea_class;
    MesaWorkstationClassPart mesaWorkstation_class;
  }
MesaWorkstationClassRec;
extern MesaWorkstationClassRec mesaWorkstationClassRec;
#endif

typedef struct
  {
    projection projection;
    view view;
    GLuint *objects;
    GLuint *next_object;
    size_t allocated_objects;
  }
MesaWorkstationPart;

#ifdef __GLX_MOTIF
typedef struct _MesaMWorkstationRec
  {
    CorePart core;
    XmPrimitivePart primitive;
    GLwDrawingAreaPart glwDrawingArea;
    MesaDrawingAreaPart mesaDrawingArea;
    MesaWorkstationPart mesaWorkstation;
  }
MesaMWorkstationRec;
#else
typedef struct _MesaWorkstationRec
  {
    CorePart core;
    GLwDrawingAreaPart glwDrawingArea;
    MesaDrawingAreaPart mesaDrawingArea;
    MesaWorkstationPart mesaWorkstation;
  }
MesaWorkstationRec;
#endif

#endif /* _MesaWorkstationP_h */
