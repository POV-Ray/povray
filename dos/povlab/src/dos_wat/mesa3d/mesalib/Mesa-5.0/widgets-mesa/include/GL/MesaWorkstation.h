/* MesaWorkstation.h -- Public header file for the Mesa Workstation widget
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

   $Id: MesaWorkstation.h,v 1.1.1.1 1999/08/19 00:55:42 jtg Exp $
 */

#ifndef _MesaWorkstation_h
#define _MesaWorkstation_h

#include <GL/gl.h>
#include <GL/GLwDrawA.h>
#include <GL/MesaDrawingArea.h>

#ifdef __GLX_MOTIF
typedef struct _MesaMWorkstationClassRec *MesaMWorkstationWidgetClass;
typedef struct _MesaMWorkstationRec *MesaMWorkstationWidget;
extern WidgetClass mesaMWorkstationWidgetClass;
void GLwMBeginProjection (Widget w);
void GLwMEndProjection (void);
void GLwMPostProjectionList (Widget, GLuint);
void GLwMPostProjectionMatrix (Widget, GLdouble *m);
void GLwMPostCurrentProjection (Widget);
void GLwMUnpostProjection (Widget);
GLuint GLwMGetProjectionList (Widget w);
int GLwMGetProjectionMatrix (Widget w, GLdouble *m);
void GLwMSetFrustumProjection (Widget w, GLdouble left, GLdouble right,
			       GLdouble bottom, GLdouble top,
			       GLdouble near, GLdouble far);
void GLwMSetOrthoProjection (Widget w, GLdouble left, GLdouble right,
			     GLdouble bottom, GLdouble top,
			     GLdouble near, GLdouble far);
void GLwMBeginView (Widget w);
void GLwMEndView (void);
void GLwMPostViewList (Widget, GLuint);
void GLwMPostViewMatrix (Widget, GLdouble *m);
void GLwMPostCurrentView (Widget);
void GLwMUnpostView (Widget);
GLuint GLwMGetViewList (Widget w);
int GLwMGetViewMatrix (Widget w, GLdouble *m);
void GLwMSetPolarView (Widget w, GLdouble r, GLdouble theta, GLdouble phi);
void GLwMPostObject (Widget, GLuint);
void GLwMUnpostObject (Widget, GLuint);
void GLwMUnpostAllObjects (Widget);
void GLwMRedrawObjects (Widget);
void GLwMPostProjection (Widget, GLuint); /* obsolete */
GLuint GLwMGetProjection (Widget w); /* obsolete */
void GLwMPostView (Widget, GLuint); /* obsolete */
GLuint GLwMGetView (Widget w); /* obsolete */
#else /* not __GLX_MOTIF */
typedef struct _MesaWorkstationClassRec *MesaWorkstationWidgetClass;
typedef struct _MesaWorkstationRec *MesaWorkstationWidget;
extern WidgetClass mesaWorkstationWidgetClass;
void GLwBeginProjection (Widget w);
void GLwEndProjection (void);
void GLwPostProjectionList (Widget, GLuint);
void GLwPostProjectionMatrix (Widget, GLdouble *m);
void GLwPostCurrentProjection (Widget);
void GLwUnpostProjection (Widget);
GLuint GLwGetProjectionList (Widget w);
int GLwGetProjectionMatrix (Widget w, GLdouble *m);
void GLwSetFrustumProjection (Widget w, GLdouble left, GLdouble right,
			      GLdouble bottom, GLdouble top,
			      GLdouble near, GLdouble far);
void GLwSetOrthoProjection (Widget w, GLdouble left, GLdouble right,
			    GLdouble bottom, GLdouble top,
			    GLdouble near, GLdouble far);
void GLwBeginView (Widget w);
void GLwEndView (void);
void GLwPostViewList (Widget, GLuint);
void GLwPostViewMatrix (Widget, GLdouble *m);
void GLwPostCurrentView (Widget);
void GLwUnpostView (Widget);
GLuint GLwGetViewList (Widget w);
int GLwGetViewMatrix (Widget w, GLdouble *m);
void GLwSetPolarView (Widget w, GLdouble r, GLdouble theta, GLdouble phi);

void GLwPostObject (Widget, GLuint);
void GLwUnpostObject (Widget, GLuint);
void GLwUnpostAllObjects (Widget);
void GLwRedrawObjects (Widget);
void GLwPostProjection (Widget, GLuint); /* obsolete */
GLuint GLwGetProjection (Widget w); /* obsolete */
void GLwPostView (Widget, GLuint); /* obsolete */
GLuint GLwGetView (Widget w); /* obsolete */
#endif /* not __GLX_MOTIF */

#endif /* _MesaWorkstation_h */
