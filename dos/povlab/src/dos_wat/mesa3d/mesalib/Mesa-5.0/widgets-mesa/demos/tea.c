/* tea.c -- demo program for the MesaWS widget
   Copyright (C) 1995 Thorsten.Ohl @ Physik.TH-Darmstadt.de

   Parts Copyright (c) Mark J. Kilgard, 1994.  See below.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

   $Id: tea.c,v 1.1.1.1 1999/08/19 00:55:42 jtg Exp $

 */

/* No 3D graphics package is complete without a teapot demo.
   Use the cursor keys (and other MesaWorkstationWidget translations)
   to move the teapot around.

   FIXME: toggling the radiobuttons sends two expose events, which is
          a nuisance on affortable hardware ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Toggle.h>
#include <GL/xmesa.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/MesaWorkstation.h>

#ifdef __GLX_MOTIF
#include <GL/MesaMWorkstation.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#define GLwMakeCurrent GLwMMakeCurrent
#define GLwPostObject		   GLwMPostObject
#define GLwBeginView		   GLwMBeginView
#define GLwEndView		   GLwMEndView
#define GLwSetPolarView		   GLwMSetPolarView
#define GLwBeginProjection	   GLwMBeginProjection
#define GLwEndProjection	   GLwMEndProjection
#define GLwSetFrustumProjection	   GLwMSetFrustumProjection
#define GLwGetProjectionList	   GLwMGetProjectionList
#define GLwRedrawObjects	   GLwMRedrawObjects
#define GLwPostProjection	   GLwMPostProjection
#endif /* __GLX_MOTIF */

static char *RCS_Id =
"@(#) $Id: tea.c,v 1.1.1.1 1999/08/19 00:55:42 jtg Exp $";

void glutSolidTeapot (GLdouble scale);
void glutWireTeapot (GLdouble scale);

static GLint
alloc_color (Widget w, Colormap cmap, int red, int green, int blue)
{ 
  XColor xcolor;
  xcolor.red = red;
  xcolor.green = green;
  xcolor.blue = blue;
  xcolor.flags = DoRed | DoGreen | DoBlue;
  if (!XAllocColor (XtDisplay (w), cmap, &xcolor))
    {
      printf ("Couldn't allocate color!\n");
      exit (1);
    }
  return xcolor.pixel;
}

/* This is rather inefficient, but we don't mind for the moment,
   because it works.  */

static void
translate_pixels (Widget to, Widget from, ...)
{
  va_list ap;
  char *name;
  Colormap from_cmap, to_cmap;
  XColor xcolor;

  XtVaGetValues (from, XtNcolormap, &from_cmap, NULL);
  XtVaGetValues (to, XtNcolormap, &to_cmap, NULL);

  va_start (ap, from);
  for (name = va_arg (ap, char *); name != NULL; name = va_arg (ap, char *))
    {
      XtVaGetValues (from, name, &xcolor.pixel, NULL);
      XQueryColor (XtDisplay (from), from_cmap, &xcolor);
      if (!XAllocColor (XtDisplay (to), to_cmap, &xcolor))
	XtAppWarning (XtWidgetToApplicationContext (to),
		      "Couldn't allocate color!\n");
      else
	XtVaSetValues (from, name, xcolor.pixel, NULL);
    }
  va_end (ap);
}
#if 0
static Widget
create_command (Widget parent, char *name, XtCallbackProc cb)
{
  Widget ok;
  ok = XtVaCreateManagedWidget (name, commandWidgetClass, parent, NULL);
  XtAddCallback (ok, XtNcallback, cb, NULL);
  return ok;
}
#endif
GLuint light, material;
Widget mesa;

void
setup_light (void)
{
  GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};

  /* light_position is NOT default value */
  GLfloat light_position[] = {1.0, 0.0, 0.0, 0.0};
  GLfloat global_ambient[] = {0.75, 0.75, 0.75, 1.0};

  glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv (GL_LIGHT0, GL_POSITION, light_position);

  glLightModelfv (GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

static XtTranslations toggle_translations;
static char toggle_translation_string[] =
  "<Btn1Down>,<Btn1Up>: set() notify()";

enum light_source { FIX_OBJ, FIX_OBS };
  
static void
light_cb (Widget w, XtPointer client_data, XtPointer junk)
{
  switch ((enum light_source) client_data)
    {
    case FIX_OBJ:
      /* Fixed wrt object: perform the light setup in the the LIGHT list.  */
      glNewList (light, GL_COMPILE);
        setup_light ();
      glEndList ();
      break;
    case FIX_OBS:
      /* Fixed wrt observer: nuke the LIGHT list and setup the
	 light in the default coordinate system.  */
      glNewList (light, GL_COMPILE);
      glEndList ();
      glPushMatrix ();
        glLoadIdentity ();
	setup_light ();
      glPopMatrix ();
      break;
    }
  GLwRedrawObjects (mesa);
}

enum material_mode { MAT_GOLD, MAT_RUBY, MAT_EMERALD };
  
static void
material_cb (Widget w, XtPointer client_data, XtPointer junk)
{
  GLfloat gold_ambient[] = {0.24725, 0.1995, 0.0745, 1.0};
  GLfloat gold_diffuse[] = {0.75164, 0.60648, 0.22648, 1.0};
  GLfloat gold_specular[] = {0.628281, 0.555802, 0.366065, 1.0};
  GLfloat gold_shine = 0.4;
  
  GLfloat ruby_ambient[] = {0.1745, 0.01175, 0.01175, 1.0};
  GLfloat ruby_diffuse[] = {0.61424, 0.04136, 0.04136, 1.0};
  GLfloat ruby_specular[] = {0.727811, 0.626959, 0.626959, 1.0};
  GLfloat ruby_shine = 0.6;
  
  GLfloat emerald_ambient[] = {0.0215, 0.1745, 0.0215, 1.0};
  GLfloat emerald_diffuse[] = {0.07568, 0.61424, 0.07568, 1.0};
  GLfloat emerald_specular[] = {0.633, 0.727811, 0.633, 1.0};
  GLfloat emerald_shine = 0.6;
  
  glNewList (material, GL_COMPILE);
  {
    switch ((enum material_mode) client_data)
      {
      case MAT_GOLD:
	glMaterialfv (GL_FRONT, GL_AMBIENT, gold_ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, gold_diffuse);
	glMaterialfv (GL_FRONT, GL_SPECULAR, gold_specular);
	glMaterialf (GL_FRONT, GL_SHININESS, gold_shine*128.0);
	break;
      case MAT_RUBY:
	glMaterialfv (GL_FRONT, GL_AMBIENT, ruby_ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, ruby_diffuse);
	glMaterialfv (GL_FRONT, GL_SPECULAR, ruby_specular);
	glMaterialf (GL_FRONT, GL_SHININESS, ruby_shine*128.0);
	break;
      case MAT_EMERALD:
	glMaterialfv (GL_FRONT, GL_AMBIENT, emerald_ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, emerald_diffuse);
	glMaterialfv (GL_FRONT, GL_SPECULAR, emerald_specular);
	glMaterialf (GL_FRONT, GL_SHININESS, emerald_shine*128.0);
	break;
      }
  }
  glEndList ();
  GLwRedrawObjects (mesa);
}

void
quit_function (Widget w, XtPointer closure, XtPointer call_data)
{
  exit (0);
}

static String fallback_resources[] =
{
#ifndef __GLX_MOTIF
  "*MesaWorkstation.debug: false",
  "*MesaWorkstation.installColormap: true",
  "*MesaWorkstation.ximage: true",
  "*MesaWorkstation.doublebuffer: false",
  "*MesaWorkstation.depthSize: 1",
#else /* __GLX_MOTIF */
  "*MesaMWorkstation.debug: false",
  "*MesaMWorkstation.installColormap: true",
  "*MesaMWorkstation.ximage: true",
  "*MesaMWorkstation.doublebuffer: false",
#endif /* __GLX_MOTIF */
  "*quit.label: Exit",
  "*mesa.width: 400",
  "*mesa.height: 400",
  NULL
};

int
main (int argc, char *argv[])
{
  Widget top, frame, quit;
  Widget fix_obj, fix_obs;
  Widget gold, ruby, emerald;
#ifdef __GLX_MOTIF
  Widget rb1, rb2;
#endif
  XtAppContext app_context;
  Boolean cmap_installed;
  GLuint teapot;

  XtSetLanguageProc (NULL, NULL, NULL);
  top = XtVaAppInitialize (&app_context, "Tea", NULL, 0,
			   &argc, argv, fallback_resources, NULL);

#ifdef __GLX_MOTIF
  frame = XtVaCreateManagedWidget ("frame", xmFormWidgetClass,
				   top,
				   NULL);

  mesa = XtVaCreateManagedWidget ("mesa", mesaMWorkstationWidgetClass,
				  frame,
				  GLwNrgba, True,
				  XmNtopAttachment, XmATTACH_FORM,
				  XmNtopOffset, 10,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNleftOffset, 10,
				  XmNrightAttachment, XmATTACH_NONE,
				  XmNrightOffset, 10,
				  XmNbottomAttachment, XmATTACH_FORM,
				  NULL);
  rb1 = XtVaCreateManagedWidget ("rb1", xmRowColumnWidgetClass,
				 frame,
				 XmNradioBehavior, True,
				 XmNtopAttachment, XmATTACH_FORM,
				 XmNtopOffset, 10,
				 XmNleftAttachment, XmATTACH_WIDGET,
				 XmNleftOffset, 10,
				 XmNleftWidget, mesa,
				 XmNrightAttachment, XmATTACH_NONE,
				 XmNbottomAttachment, XmATTACH_NONE,
				 NULL);
  fix_obj = XtVaCreateManagedWidget ("fix_obj", xmToggleButtonWidgetClass,
				     rb1,
				     XtNstate, False,
				     XmNtopAttachment, XmATTACH_FORM,
				     XmNtopOffset, 0,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNleftOffset, 0,
				     XmNrightAttachment, XmATTACH_FORM,
				     XmNrightOffset, 0,
				     XmNbottomAttachment, XmATTACH_FORM,
				     XmNbottomOffset, 0,
				     NULL);
  fix_obs = XtVaCreateManagedWidget ("fix_obs", xmToggleButtonWidgetClass,
				     rb1,
				     XtNstate, True,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNtopOffset, 10,
				     XmNtopWidget, fix_obj,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNleftOffset, 0,
				     XmNrightAttachment, XmATTACH_FORM,
				     XmNrightOffset, 0,
				     XmNbottomAttachment, XmATTACH_FORM,
				     XmNbottomOffset, 0,
				     NULL);
#else /* __GLX_MOTIF */
  frame = XtVaCreateManagedWidget ("frame", formWidgetClass,
				   top,
				   NULL);

  mesa = XtVaCreateManagedWidget ("mesa", mesaWorkstationWidgetClass,
				  frame,
				  GLwNrgba, True,
				  NULL);
  fix_obj = XtVaCreateManagedWidget ("fix_obj", toggleWidgetClass,
				     frame,
				     XtNlabel, "light fixed wrt object",
				     XtNstate, False,
				     XtNfromHoriz, mesa, XtNhorizDistance, 10,
				     NULL);
  fix_obs = XtVaCreateManagedWidget ("fix_obs", toggleWidgetClass,
				     frame,
				     XtNlabel, "light fixed wrt observer",
				     XtNstate, True,
				     XtNradioGroup, fix_obj,
				     XtNfromVert, fix_obj, XtNvertDistance, 0,
				     XtNfromHoriz, mesa, XtNhorizDistance, 10,
				     NULL);
#endif /* __GLX_MOTIF */
#ifdef __GLX_MOTIF
  XtAddCallback (fix_obj, XmNarmCallback, light_cb, (XtPointer) FIX_OBJ);
  XtAddCallback (fix_obs, XmNarmCallback, light_cb, (XtPointer) FIX_OBS);
#else /* __GLX_MOTIF */
  toggle_translations = XtParseTranslationTable (toggle_translation_string);
  XtOverrideTranslations (fix_obj, toggle_translations);
  XtOverrideTranslations (fix_obs, toggle_translations);
  XtAddCallback (fix_obj, XtNcallback, light_cb, (XtPointer) FIX_OBJ);
  XtAddCallback (fix_obs, XtNcallback, light_cb, (XtPointer) FIX_OBS);
#endif /* __GLX_MOTIF */

#ifdef __GLX_MOTIF
  rb2 = XtVaCreateManagedWidget ("rb2", xmRowColumnWidgetClass,
				 frame,
				 XmNradioBehavior, True,
				 XmNtopAttachment, XmATTACH_WIDGET,
				 XmNtopOffset, 20,
				 XmNtopWidget, rb1,
				 XmNleftAttachment, XmATTACH_WIDGET,
				 XmNleftOffset, 10,
				 XmNleftWidget, mesa,
				 XmNrightAttachment, XmATTACH_NONE,
				 XmNbottomAttachment, XmATTACH_NONE,
				 NULL);
  gold = XtVaCreateManagedWidget ("gold", xmToggleButtonWidgetClass,
				  rb2,
				  XtNstate, True,
				  XmNtopAttachment, XmATTACH_FORM,
				  XmNtopOffset, 0,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNleftOffset, 0,
				  XmNrightAttachment, XmATTACH_NONE,
				  XmNbottomAttachment, XmATTACH_NONE,
				  NULL);
  ruby = XtVaCreateManagedWidget ("ruby", xmToggleButtonWidgetClass,
				  rb2,
				  XtNstate, False,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopOffset, 0,
				  XmNtopWidget, gold,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNleftOffset, 0,
				  XmNrightAttachment, XmATTACH_NONE,
				  XmNbottomAttachment, XmATTACH_NONE,
				  NULL);
  emerald = XtVaCreateManagedWidget ("emerald", xmToggleButtonWidgetClass,
				     rb2,
				     XtNstate, False,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNtopOffset, 0,
				     XmNtopWidget, ruby,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNleftOffset, 0,
				     XmNrightAttachment, XmATTACH_NONE,
				     XmNbottomAttachment, XmATTACH_NONE,
				     NULL);
#else /* __GLX_MOTIF */
  gold = XtVaCreateManagedWidget ("gold", toggleWidgetClass,
				  frame,
				  XtNlabel, "gold",
				  XtNstate, True,
				  XtNfromVert, fix_obs, XtNvertDistance, 20,
				  XtNfromHoriz, mesa, XtNhorizDistance, 10,
				  NULL);
  ruby = XtVaCreateManagedWidget ("ruby", toggleWidgetClass,
				  frame,
				  XtNlabel, "ruby",
				  XtNstate, False,
				  XtNradioGroup, gold,
				  XtNfromVert, gold, XtNvertDistance, 0,
				  XtNfromHoriz, mesa, XtNhorizDistance, 10,
				  NULL);
  emerald = XtVaCreateManagedWidget ("emerald", toggleWidgetClass,
				     frame,
				     XtNlabel, "emerald",
				     XtNstate, False,
				     XtNradioGroup, gold,
				     XtNfromVert, ruby, XtNvertDistance, 0,
				     XtNfromHoriz, mesa, XtNhorizDistance, 10,
				     NULL);
#endif /* __GLX_MOTIF */

#ifdef __GLX_MOTIF
  XtAddCallback (gold, XmNarmCallback, material_cb, (XtPointer) MAT_GOLD);
  XtAddCallback (ruby, XmNarmCallback, material_cb, (XtPointer) MAT_RUBY);
  XtAddCallback (emerald, XmNarmCallback, material_cb, (XtPointer) MAT_EMERALD);
#else /* __GLX_MOTIF */
  XtOverrideTranslations (gold, toggle_translations);
  XtOverrideTranslations (ruby, toggle_translations);
  XtOverrideTranslations (emerald, toggle_translations);
  XtAddCallback (gold, XtNcallback, material_cb, (XtPointer) MAT_GOLD);
  XtAddCallback (ruby, XtNcallback, material_cb, (XtPointer) MAT_RUBY);
  XtAddCallback (emerald, XtNcallback, material_cb, (XtPointer) MAT_EMERALD);
#endif /* __GLX_MOTIF */

#ifdef __GLX_MOTIF
  quit = XtVaCreateManagedWidget ("quit", xmPushButtonWidgetClass,
				  frame,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, rb2,
				  XmNtopOffset, 50,
				  XmNleftAttachment, XmATTACH_WIDGET,
				  XmNleftOffset, 10,
				  XmNleftWidget, mesa,
				  XmNrightAttachment, XmATTACH_NONE,
				  XmNrightOffset, 10,
				  XmNbottomAttachment, XmATTACH_NONE,
				  NULL);
  XtAddCallback (quit, XmNarmCallback, quit_function, NULL);
#else /* __GLX_MOTIF */
  quit = XtVaCreateManagedWidget ("quit", commandWidgetClass,
				  frame,
				  XtNfromVert, emerald, XtNvertDistance, 50,
				  XtNfromHoriz, mesa, XtNhorizDistance, 10,
				  NULL);
  XtAddCallback (quit, XtNcallback, quit_function, NULL);
#endif /* __GLX_MOTIF */

  XtRealizeWidget (top);

  XtVaGetValues (mesa, GLwNinstallColormap, &cmap_installed, NULL);
  if (cmap_installed)
    {
      translate_pixels (mesa, quit,
			XtNbackground, XtNforeground, XtNborder, NULL);
      translate_pixels (mesa, fix_obj,
			XtNbackground, XtNforeground, XtNborder, NULL);
      translate_pixels (mesa, fix_obs,
			XtNbackground, XtNforeground, XtNborder, NULL);
      translate_pixels (mesa, gold,
			XtNbackground, XtNforeground, XtNborder, NULL);
      translate_pixels (mesa, ruby,
			XtNbackground, XtNforeground, XtNborder, NULL);
      translate_pixels (mesa, emerald,
			XtNbackground, XtNforeground, XtNborder, NULL);
      translate_pixels (mesa, frame, XtNbackground, XtNborder, NULL);
      XWarpPointer (XtDisplay (mesa), None, XtWindow (mesa),
		    0, 0, 0, 0, 0, 0);
    }

  GLwMakeCurrent (mesa);

  glFrontFace (GL_CW);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  glEnable (GL_AUTO_NORMAL);
  glEnable (GL_NORMALIZE);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);

  GLwSetFrustumProjection (mesa, -1.0, 1.0, -1.0, 1.0, 1.0, 10.0);
  GLwSetPolarView (mesa, 3.0, 2*M_PI/3, -M_PI/3);

  teapot = glGenLists (1);
  glNewList (teapot, GL_COMPILE);
  {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSolidTeapot (1.0);
  }
  glEndList ();

  light = glGenLists (1);
  glNewList (light, GL_COMPILE);
  glEndList ();

  material = glGenLists (1);
  glNewList (material, GL_COMPILE);
  glEndList ();

  material_cb (NULL, (XtPointer) MAT_GOLD, NULL);
  light_cb (NULL, (XtPointer) FIX_OBS, NULL);

  GLwPostObject (mesa, light);
  GLwPostObject (mesa, material);
  GLwPostObject (mesa, teapot);

  XtAppMainLoop (app_context);
  return (0);
}


/* Copyright (c) Mark J. Kilgard, 1994. */

/**
(c) Copyright 1993, Silicon Graphics, Inc.

ALL RIGHTS RESERVED

Permission to use, copy, modify, and distribute this software
for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that
both the copyright notice and this permission notice appear in
supporting documentation, and that the name of Silicon
Graphics, Inc. not be used in advertising or publicity
pertaining to distribution of the software without specific,
written prior permission.

THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU
"AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR
OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  IN NO
EVENT SHALL SILICON GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE
ELSE FOR ANY DIRECT, SPECIAL, INCIDENTAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER,
INCLUDING WITHOUT LIMITATION, LOSS OF PROFIT, LOSS OF USE,
SAVINGS OR REVENUE, OR THE CLAIMS OF THIRD PARTIES, WHETHER OR
NOT SILICON GRAPHICS, INC.  HAS BEEN ADVISED OF THE POSSIBILITY
OF SUCH LOSS, HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
ARISING OUT OF OR IN CONNECTION WITH THE POSSESSION, USE OR
PERFORMANCE OF THIS SOFTWARE.

US Government Users Restricted Rights

Use, duplication, or disclosure by the Government is subject to
restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
(c)(1)(ii) of the Rights in Technical Data and Computer
Software clause at DFARS 252.227-7013 and/or in similar or
successor clauses in the FAR or the DOD or NASA FAR
Supplement.  Unpublished-- rights reserved under the copyright
laws of the United States.  Contractor/manufacturer is Silicon
Graphics, Inc., 2011 N.  Shoreline Blvd., Mountain View, CA
94039-7311.

OpenGL(TM) is a trademark of Silicon Graphics, Inc.
*/

#include <GL/gl.h>
/* #include <GL/glut.h> */

/* Rim, body, lid, and bottom data must be reflected in x
   and y; handle and spout data across the y axis only.  */

long patchdata[][16] =
{
    /* rim */
  {102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15},
    /* body */
  {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27},
  {24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36,
    37, 38, 39, 40},
    /* lid */
  {96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101,
    101, 0, 1, 2, 3,},
  {0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112,
    113, 114, 115, 116, 117},
    /* bottom */
  {118, 118, 118, 118, 124, 122, 119, 121, 123, 126,
    125, 120, 40, 39, 38, 37},
    /* handle */
  {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
    53, 54, 55, 56},
  {53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
    28, 65, 66, 67},
    /* spout */
  {68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83},
  {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
    92, 93, 94, 95}
};
/* *INDENT-OFF* */

float cpdata[][3] =
{
    {0.2, 0, 2.7}, {0.2, -0.112, 2.7}, {0.112, -0.2, 2.7}, {0,
    -0.2, 2.7}, {1.3375, 0, 2.53125}, {1.3375, -0.749, 2.53125},
    {0.749, -1.3375, 2.53125}, {0, -1.3375, 2.53125}, {1.4375,
    0, 2.53125}, {1.4375, -0.805, 2.53125}, {0.805, -1.4375,
    2.53125}, {0, -1.4375, 2.53125}, {1.5, 0, 2.4}, {1.5, -0.84,
    2.4}, {0.84, -1.5, 2.4}, {0, -1.5, 2.4}, {1.75, 0, 1.875},
    {1.75, -0.98, 1.875}, {0.98, -1.75, 1.875}, {0, -1.75,
    1.875}, {2, 0, 1.35}, {2, -1.12, 1.35}, {1.12, -2, 1.35},
    {0, -2, 1.35}, {2, 0, 0.9}, {2, -1.12, 0.9}, {1.12, -2,
    0.9}, {0, -2, 0.9}, {-2, 0, 0.9}, {2, 0, 0.45}, {2, -1.12,
    0.45}, {1.12, -2, 0.45}, {0, -2, 0.45}, {1.5, 0, 0.225},
    {1.5, -0.84, 0.225}, {0.84, -1.5, 0.225}, {0, -1.5, 0.225},
    {1.5, 0, 0.15}, {1.5, -0.84, 0.15}, {0.84, -1.5, 0.15}, {0,
    -1.5, 0.15}, {-1.6, 0, 2.025}, {-1.6, -0.3, 2.025}, {-1.5,
    -0.3, 2.25}, {-1.5, 0, 2.25}, {-2.3, 0, 2.025}, {-2.3, -0.3,
    2.025}, {-2.5, -0.3, 2.25}, {-2.5, 0, 2.25}, {-2.7, 0,
    2.025}, {-2.7, -0.3, 2.025}, {-3, -0.3, 2.25}, {-3, 0,
    2.25}, {-2.7, 0, 1.8}, {-2.7, -0.3, 1.8}, {-3, -0.3, 1.8},
    {-3, 0, 1.8}, {-2.7, 0, 1.575}, {-2.7, -0.3, 1.575}, {-3,
    -0.3, 1.35}, {-3, 0, 1.35}, {-2.5, 0, 1.125}, {-2.5, -0.3,
    1.125}, {-2.65, -0.3, 0.9375}, {-2.65, 0, 0.9375}, {-2,
    -0.3, 0.9}, {-1.9, -0.3, 0.6}, {-1.9, 0, 0.6}, {1.7, 0,
    1.425}, {1.7, -0.66, 1.425}, {1.7, -0.66, 0.6}, {1.7, 0,
    0.6}, {2.6, 0, 1.425}, {2.6, -0.66, 1.425}, {3.1, -0.66,
    0.825}, {3.1, 0, 0.825}, {2.3, 0, 2.1}, {2.3, -0.25, 2.1},
    {2.4, -0.25, 2.025}, {2.4, 0, 2.025}, {2.7, 0, 2.4}, {2.7,
    -0.25, 2.4}, {3.3, -0.25, 2.4}, {3.3, 0, 2.4}, {2.8, 0,
    2.475}, {2.8, -0.25, 2.475}, {3.525, -0.25, 2.49375},
    {3.525, 0, 2.49375}, {2.9, 0, 2.475}, {2.9, -0.15, 2.475},
    {3.45, -0.15, 2.5125}, {3.45, 0, 2.5125}, {2.8, 0, 2.4},
    {2.8, -0.15, 2.4}, {3.2, -0.15, 2.4}, {3.2, 0, 2.4}, {0, 0,
    3.15}, {0.8, 0, 3.15}, {0.8, -0.45, 3.15}, {0.45, -0.8,
    3.15}, {0, -0.8, 3.15}, {0, 0, 2.85}, {1.4, 0, 2.4}, {1.4,
    -0.784, 2.4}, {0.784, -1.4, 2.4}, {0, -1.4, 2.4}, {0.4, 0,
    2.55}, {0.4, -0.224, 2.55}, {0.224, -0.4, 2.55}, {0, -0.4,
    2.55}, {1.3, 0, 2.55}, {1.3, -0.728, 2.55}, {0.728, -1.3,
    2.55}, {0, -1.3, 2.55}, {1.3, 0, 2.4}, {1.3, -0.728, 2.4},
    {0.728, -1.3, 2.4}, {0, -1.3, 2.4}, {0, 0, 0}, {1.425,
    -0.798, 0}, {1.5, 0, 0.075}, {1.425, 0, 0}, {0.798, -1.425,
    0}, {0, -1.5, 0.075}, {0, -1.425, 0}, {1.5, -0.84, 0.075},
    {0.84, -1.5, 0.075}
};

static float tex[2][2][2] =
{
  { {0, 0},
    {1, 0}},
  { {0, 1},
    {1, 1}}
};

/* *INDENT-ON* */

static void
teapot(GLint grid, GLdouble scale, GLenum type)
{
  float p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];
  long i, j, k, l;

  glPushAttrib(GL_ENABLE_BIT | GL_EVAL_BIT);
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_MAP2_VERTEX_3);
  glEnable(GL_MAP2_TEXTURE_COORD_2);
  glPushMatrix();
  /* glRotatef(270.0, 1.0, 0.0, 0.0); */
  /* Rotate it upwards in the standard MesaWorkstation
     coordinate system.  */
  glRotatef(90.0, 0.0, 1.0, 0.0);
  glRotatef(90.0, 1.0, 0.0, 0.0);
  glScalef(0.5 * scale, 0.5 * scale, 0.5 * scale);
  glTranslatef(0.0, 0.0, -1.5);
  for (i = 0; i < 10; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 4; k++) {
        for (l = 0; l < 3; l++) {
          p[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
          q[j][k][l] = cpdata[patchdata[i][j * 4 + (3 - k)]][l];
          if (l == 1)
            q[j][k][l] *= -1.0;
          if (i < 6) {
            r[j][k][l] =
              cpdata[patchdata[i][j * 4 + (3 - k)]][l];
            if (l == 0)
              r[j][k][l] *= -1.0;
            s[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
            if (l == 0)
              s[j][k][l] *= -1.0;
            if (l == 1)
              s[j][k][l] *= -1.0;
          }
        }
      }
    }
    glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2,
      &tex[0][0][0]);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
      &p[0][0][0]);
    glMapGrid2f(grid, 0.0, 1.0, grid, 0.0, 1.0);
    glEvalMesh2(type, 0, grid, 0, grid);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
      &q[0][0][0]);
    glEvalMesh2(type, 0, grid, 0, grid);
    if (i < 6) {
      glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
        &r[0][0][0]);
      glEvalMesh2(type, 0, grid, 0, grid);
      glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
        &s[0][0][0]);
      glEvalMesh2(type, 0, grid, 0, grid);
    }
  }
  glPopMatrix();
  glPopAttrib();
}

/* CENTRY */
void
glutSolidTeapot(GLdouble scale)
{
  teapot(14, scale, GL_FILL);
}

void
glutWireTeapot(GLdouble scale)
{
  teapot(10, scale, GL_LINE);
}
/* ENDCENTRY */
