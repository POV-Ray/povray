/* ed.c -- Event Display (demo program for the MesaWS widget)
   Copyright (C) 1995 Thorsten.Ohl @ Physik.TH-Darmstadt.de

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

   $Id: ed.c,v 1.2 2000/03/19 23:38:12 brianp Exp $
 */

/* This is a simple event display as a demonstration of the MesaWS widget.
   It reads a stream of simple event records from standard input (see the
   file `events' for a example).

   It is a slow and crude hack.  */
  
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
#ifndef __GLX_MOTIF
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <GL/xmesa.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/MesaDrawingArea.h>
#include <GL/MesaWorkstation.h>
#else /* __GLX_MOTIF */
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <GL/xmesa.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/MesaMDrawingArea.h>
#include <GL/MesaMWorkstation.h>
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
"@(#) $Id: ed.c,v 1.2 2000/03/19 23:38:12 brianp Exp $";

static GLint Black, White, Red, Green, Blue, Yellow;
Widget mesa, mesax, mesay, mesaz;

static void quit_function (Widget, XtPointer, XtPointer);
static void next_function (Widget, XtPointer, XtPointer);
static void next_event (void);
static void update_angles (float, float);
static void update_number (int);

static void
Quit (Widget w, XEvent *event, String *argv, Cardinal *argc)
{
  quit_function (NULL, NULL, NULL);
}

void
Next (Widget w, XEvent *event, String *argv, Cardinal *argc)
{
  next_function (NULL, NULL, NULL);
}

static XtActionsRec actions[] =
{
  {"Quit", Quit },
  {"Next", Next },
};

void
quit_function (Widget w, XtPointer closure, XtPointer call_data)
{
  exit (0);
}

void
next_function (Widget w, XtPointer closure, XtPointer call_data)
{
  next_event ();
  GLwRedrawObjects (mesa);
  GLwRedrawObjects (mesax);
  GLwRedrawObjects (mesay);
  GLwRedrawObjects (mesaz);
}

#define SET_POLAR(_p, _r, _theta, _phi) \
    (_p)[0] = (_r) * sin (_theta) * cos (_phi), \
    (_p)[1] = (_r) * sin (_theta) * sin (_phi), \
    (_p)[2] = (_r) * cos (_theta)
static void
draw_barrel_segment (float bottom_radius, float top_radius,
		     float front_angle, float back_angle,
		     float half_angle)
{
  int i;
  GLdouble top[4][3], bottom[4][3], *front[4], *back[4];

  SET_POLAR (top[0], top_radius, front_angle, half_angle);
  SET_POLAR (top[1], top_radius, front_angle, -half_angle);
  SET_POLAR (top[2], top_radius, back_angle, -half_angle);
  SET_POLAR (top[3], top_radius, back_angle, half_angle);

  SET_POLAR (bottom[0], bottom_radius, front_angle, half_angle);
  SET_POLAR (bottom[1], bottom_radius, front_angle, -half_angle);
  SET_POLAR (bottom[2], bottom_radius, back_angle, -half_angle);
  SET_POLAR (bottom[3], bottom_radius, back_angle, half_angle);

  front[0] = top[0];
  front[1] = top[1];
  front[2] = bottom[1];
  front[3] = bottom[0];

  back[0] = top[3];
  back[1] = top[2];
  back[2] = bottom[2];
  back[3] = bottom[3];

  glBegin (GL_LINE_LOOP);
    for (i = 0; i <= 3; i++)
      glVertex3dv (top[i]);
  glEnd ();
  
  glBegin (GL_LINE_LOOP);
    for (i = 0; i <= 3; i++)
      glVertex3dv (bottom[i]);
  glEnd ();
  
  glBegin (GL_LINE_LOOP);
    for (i = 0; i <= 3; i++)
      glVertex3dv (front[i]);
  glEnd ();
  
  glBegin (GL_LINE_LOOP);
    for (i = 0; i <= 3; i++)
      glVertex3dv (back[i]);
  glEnd ();
}

static GLuint particles;
static GLuint greek, latin, desc, desc_number, desc_angles;

static void
next_event (void)
{
  char buffer[100];
  enum
    {
      SKIP, READ, DONE
    }
  state;
  int event = 0;
  
  glNewList (particles, GL_COMPILE);
  glBegin (GL_LINES);
  {
    state = SKIP;
    while (state != DONE)
      {
	int id;
	float px, py, pz;
	if (fgets (buffer, 100, stdin) == NULL)
	  state = DONE;
	switch (state)
	  {
	  case SKIP:
	    if (sscanf (buffer, "BEGIN %d", &event) == 1)
	      state = READ;
	    break;
	  case READ:
	    if (sscanf (buffer, "%d %g %g %g",
			&id, &px, &py, &pz) == 4)
	      {
		if (id == 22)
		  {
		    /* Photons */
		    glIndexi (Red);
		    glColor3f (1.0, 0.0, 0.0);
		  }
		else if ((11 == abs (id))
			 || (13 == abs (id))
			 || (15 == abs (id)))
		  {
		    /* Charged leptons */
		    glIndexi (Green);
		    glColor3f (0.0, 1.0, 0.0);
		  }
		else if ((12 == abs (id))
			 || (14 == abs (id))
			 || (16 == abs (id)))
		  {
		    /* Neutral leptons */
		    glIndexi (Yellow);
		    glColor3f (1.0, 1.0, 0.0);
		  }
		else if ((abs (id) <= 6) || (100 <= abs (id)))
		  {
		    /* Quarks/Hadrons */
		    glIndexi (Blue);
		    glColor3f (0.0, 0.0, 1.0);
		  }
		else
		  {
		    /* ??? */
		    glIndexi (White);
		    glColor3f (1.0, 1.0, 1.0);
		  }
		glVertex3f (0.0, 0.0, 0.0);
		glVertex3f (px, py, pz);
	      }
	    else if (strncmp ("END", buffer, 3) == 0)
	      state = DONE;
	    break;
	  case DONE:
	    /* never happens */
	    break;
	  }
      }
  }
  glEnd ();
  glEndList ();

  update_number (event);
}

static void
update_angles (float theta, float phi)
{
  char msg[64];
  sprintf (msg, "f=%5.2fp, q=%5.2fp", phi/M_PI, theta/M_PI);
  glNewList (desc_angles, GL_COMPILE);
    glCallLists (strlen (msg), GL_BYTE, msg);
  glEndList ();
}  

static void
update_number (int event)
{
  char msg[32];
  sprintf (msg, "ADLO/TH, #%d", event);
  glNewList (desc_number, GL_COMPILE);
    glCallLists (strlen (msg), GL_BYTE, msg);
  glEndList ();
}  

static void
setup_context (Widget w)
{
  GLwMakeCurrent (w);
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glClearIndex (Black);
  glShadeModel (GL_FLAT);
}

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

static Widget
create_label (Widget parent, char *name, char *text)
{
#ifndef __GLX_MOTIF
  return XtVaCreateManagedWidget (name, labelWidgetClass, parent,
				  XtNlabel, text, NULL);
#else /* __GLX_MOTIF */
  XmString label = XmStringCreateLtoR (text, XmFONTLIST_DEFAULT_TAG);
  return XtVaCreateManagedWidget (name, xmLabelWidgetClass, parent,
				  XmNlabelString, label, NULL);
  XmStringFree (label);
#endif /* __GLX_MOTIF */
}

static Widget
create_ok (Widget parent, char *name, XtCallbackProc cb)
{
  Widget ok;
#ifndef __GLX_MOTIF
  ok = XtVaCreateManagedWidget (name, commandWidgetClass, parent,
				XtNlabel, "OK", NULL);
  XtAddCallback (ok, XtNcallback, cb, NULL);
#else /* __GLX_MOTIF */
  XmString label = XmStringCreate ("OK", XmFONTLIST_DEFAULT_TAG);
  ok = XtVaCreateManagedWidget (name, xmPushButtonWidgetClass, parent,
				XmNlabelString, label, NULL);
  XmStringFree (label);
  XtAddCallback (ok, XmNarmCallback, cb, NULL);
#endif /* __GLX_MOTIF */
  return ok;
}

static Widget
create_command (Widget parent, char *name, XtCallbackProc cb)
{
  Widget ok;
#ifndef __GLX_MOTIF
  ok = XtVaCreateManagedWidget (name, commandWidgetClass, parent, NULL);
  XtAddCallback (ok, XtNcallback, cb, NULL);
#else /* __GLX_MOTIF */
  ok = XtVaCreateManagedWidget (name, xmPushButtonWidgetClass, parent, NULL);
  XtAddCallback (ok, XmNarmCallback, cb, NULL);
#endif /* __GLX_MOTIF */
  return ok;
}

static Widget
create_popup (Widget parent, char *name)
{
#ifndef __GLX_MOTIF
  return XtVaCreatePopupShell (name, transientShellWidgetClass, parent, NULL);
#else /* __GLX_MOTIF */
  /* ??? */
  return XtVaCreatePopupShell (name, transientShellWidgetClass, parent, NULL);
#endif /* __GLX_MOTIF */
}

static Widget
create_box (Widget parent, char *name)
{
#ifndef __GLX_MOTIF
  return XtVaCreateManagedWidget (name, boxWidgetClass, parent,
				  XtNorientation, XtorientVertical, NULL);
#else /* __GLX_MOTIF */
  return XtVaCreateManagedWidget (name, xmRowColumnWidgetClass, parent,
				  XmNorientation, XmVERTICAL, NULL);
#endif /* __GLX_MOTIF */
}

   
Widget about_shell, help_shell;

char about_message[] = "\
This is ed, a simple particle physics event display\n\
application and demo for the MesaWorkstation widget.\n\
\n\
Copyright (C) 1995 Thorsten.Ohl @ Physik.TH-Darmstadt.de\n\
\n\
This is free software; see the source for copying conditions.\n\
There is NO warranty; not even for MERCHANTABILITY\n\
or FITNESS FOR A PARTICULAR PURPOSE.";

void
popup_about (Widget w, XtPointer closure, XtPointer call_data)
{
  XtPopup (about_shell, XtGrabNonexclusive);
}

void
popdown_about (Widget w, XtPointer closure, XtPointer call_data)
{
  XtPopdown (about_shell);
}


char help_message[] = "\
Sorry, no online help available yet.\n\
\n\
See the manpage or the source of the MesaWorkstation widget\n\
for the available keystroke translations and their actions.\n\
For a start, use the cursor keys to rotate the object.\n\
\n\
The color code of the particles is as follows:\n\
\n\
  red:    photons\n\
  green:  charged leptons (electrons, muons, taus)\n\
  yellow: neutrinos\n\
  blue:   hadrons (incl. quarks)\n\
  while:  unidentified\n";

void
popup_help (Widget w, XtPointer closure, XtPointer call_data)
{
  XtPopup (help_shell, XtGrabNonexclusive);
}

void
popdown_help (Widget w, XtPointer closure, XtPointer call_data)
{
  XtPopdown (help_shell);
}


static XrmOptionDescRec options[] =
{
  { "-debug", "*debug", XrmoptionNoArg, "True" },
  { "-rgba", "*rgba", XrmoptionNoArg, "True" },
  { "-doublebuffer", "*doublebuffer", XrmoptionNoArg, "True" },
  { "-ximage", "*ximage", XrmoptionNoArg, "True" },
};

static String fallback_resources[] =
{
#ifndef __GLX_MOTIF
  "*MesaWorkstation.debug: false",
  "*MesaWorkstation.rgba: true",
  "*MesaWorkstation.installColormap: true",
  "*MesaWorkstation.doublebuffer: true",
  "*MesaWorkstation.ximage: false",
  "*MesaWorkstation.translations: #augment \
   <Key>q: Quit()\n\
   <Key>Return: Next()\n\
   <Key>space: Next()",
#else /* __GLX_MOTIF */
  "*MesaMWorkstation.debug: false",
  "*MesaMWorkstation.rgba: true",
  "*MesaMWorkstation.installColormap: true",
  "*MesaMWorkstation.doublebuffer: true",
  "*MesaMWorkstation.ximage: false",
  "*MesaMWorkstation.translations: #augment \
   <Key>q: Quit()\n\
   <Key>Return: Next()\n\
   <Key>space: Next()",
#endif /* __GLX_MOTIF */
  "*help.label: Help",
  "*about.label: About ed",
  "*quit.label: Exit",
  "*next.label: Next event",
  "*mesa.width: 400",
  "*mesa.height: 400",
  "*mesax.width: 100",
  "*mesax.height: 100",
  "*mesay.width: 100",
  "*mesay.height: 100",
  "*mesaz.width: 100",
  "*mesaz.height: 100",
  NULL
};

int
main (int argc, char *argv[])
{
  Widget top, frame, commands, next, quit;
  Widget about, about_box, about_text, about_id, about_ok;
  Widget help, help_box, help_text, help_ok;
  XtAppContext app_context;
  Boolean rgba, cmap_installed;
  
  GLuint barrel, object;

  XtSetLanguageProc (NULL, NULL, NULL);
  top = XtVaAppInitialize (&app_context, "Ed",
			   options, XtNumber (options),
			   &argc, argv, fallback_resources,
			   NULL);

#ifndef __GLX_MOTIF
  frame = XtVaCreateManagedWidget ("frame", formWidgetClass,
				   top,
				   NULL);
  commands = XtVaCreateManagedWidget ("commands", boxWidgetClass,
				      frame,
				      XtNhSpace, 10,
				      XtNorientation, XtorientHorizontal,
				      NULL);
#else /* __GLX_MOTIF */
  frame = XtVaCreateManagedWidget ("frame", xmFormWidgetClass,
				   top,
				   /* This should not be necessary.  Is it
				      a bug in LessTif or in my understanding
				      of Motif?  */
				   XtNwidth, 420, XtNheight, 600,
 				   NULL);
  commands = XtVaCreateManagedWidget ("commands", xmRowColumnWidgetClass,
				      frame,
				      XmNorientation, XmHORIZONTAL,
				      XmNtopAttachment, XmATTACH_FORM,
				      XmNtopOffset, 10,
				      XmNrightAttachment, XmATTACH_FORM,
				      XmNrightOffset, 10,
				      NULL);
#endif /* __GLX_MOTIF */

  help = create_command (commands, "help", popup_help);
  help_shell = create_popup (top, "help_shell");
  help_box = create_box (help_shell, "help_box");
  help_text = create_label (help_box, "help_text", help_message);
  help_ok = create_ok (help_box, "help_ok", popdown_help);

  about = create_command (commands, "about", popup_about);
  about_shell = create_popup (top, "about_shell");
  about_box = create_box (about_shell, "about_box");
  about_text = create_label (about_box, "about_text", about_message);
  about_id = create_label (about_box, "about_id", RCS_Id);
  about_ok = create_ok (about_box, "about_ok", popdown_about);

  next = create_command (commands, "next", next_function);
  quit = create_command (commands, "quit", quit_function);

#ifndef __GLX_MOTIF
  mesa = XtVaCreateManagedWidget ("mesa", mesaWorkstationWidgetClass,
				  frame,
				  GLwNshareLists, True,
				  XtNfromVert, commands, XtNvertDistance, 10,
				  NULL);
  mesax = XtVaCreateManagedWidget ("mesax", mesaWorkstationWidgetClass,
				   frame,
				   GLwNshareLists, True,
				   XtNfromVert, mesa, XtNvertDistance, 10,
				   NULL);
  mesay = XtVaCreateManagedWidget ("mesay", mesaWorkstationWidgetClass,
				   frame,
				   GLwNshareLists, True,
				   XtNfromVert, mesa, XtNvertDistance, 10,
				   XtNfromHoriz, mesax, XtNhorizDistance, 50,
				   NULL);
  mesaz = XtVaCreateManagedWidget ("mesaz", mesaWorkstationWidgetClass,
				   frame,
				   GLwNshareLists, True,
				   XtNfromVert, mesa, XtNvertDistance, 10,
				   XtNfromHoriz, mesay, XtNhorizDistance, 50,
				   NULL);
#else /* __GLX_MOTIF */
  mesa = XtVaCreateManagedWidget ("mesa", mesaMWorkstationWidgetClass,
				  frame,
				  GLwNshareLists, True,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopOffset, 10,
				  XmNtopWidget, commands,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNleftOffset, 10,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNrightOffset, 10,
				  NULL);
  mesax = XtVaCreateManagedWidget ("mesax", mesaMWorkstationWidgetClass,
				   frame,
				   GLwNshareLists, True,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopOffset, 10,
				   XmNtopWidget, mesa,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNleftOffset, 10,
				   XmNbottomAttachment, XmATTACH_FORM,
				   XmNbottomOffset, 10,
				   NULL);
  mesay = XtVaCreateManagedWidget ("mesay", mesaMWorkstationWidgetClass,
				   frame,
				   GLwNshareLists, True,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopOffset, 10,
				   XmNtopWidget, mesa,
				   XmNleftAttachment, XmATTACH_WIDGET,
				   XmNleftOffset, 50,
				   XmNleftWidget, mesax,
				   XmNbottomAttachment, XmATTACH_FORM,
				   XmNbottomOffset, 10,
				   NULL);
  mesaz = XtVaCreateManagedWidget ("mesaz", mesaMWorkstationWidgetClass,
				   frame,
				   GLwNshareLists, True,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopOffset, 10,
				   XmNtopWidget, mesa,
				   XmNleftAttachment, XmATTACH_WIDGET,
				   XmNleftOffset, 50,
				   XmNleftWidget, mesay,
				   XmNbottomAttachment, XmATTACH_FORM,
				   XmNbottomOffset, 10,
				   XmNrightAttachment, XmATTACH_FORM,
				   XmNrightOffset, 10,
				   NULL);
#endif /* __GLX_MOTIF */

  XtAppAddActions (app_context, actions, XtNumber (actions));
  
  XtRealizeWidget (top);

  XtVaGetValues (mesa,
		 GLwNrgba, &rgba,
		 GLwNinstallColormap, &cmap_installed,
		 NULL);
  if (rgba)
    {
      Black = White = Red = Green = Blue = Yellow = 0;

      if (cmap_installed)
	{
	  /* In RGBA mode, the Mesa widgets will have their own color map.
	     Adjust the colors of the other widgets so that--even if the rest
	     of the screen has wrong colors--all application widgets have the
	     right colors.  */
		     
	  translate_pixels (mesa, quit,
			    XtNbackground, XtNforeground, XtNborder, NULL);
	  translate_pixels (mesa, next,
			    XtNbackground, XtNforeground, XtNborder, NULL);
	  translate_pixels (mesa, help,
			    XtNbackground, XtNforeground, XtNborder, NULL);
	  translate_pixels (mesa, about,
			    XtNbackground, XtNforeground, XtNborder, NULL);
	  translate_pixels (mesa, commands, XtNbackground, XtNborder, NULL);
	  translate_pixels (mesa, frame, XtNbackground, XtNborder, NULL);
	  
	  /* Finally warp the pointer into the mesa widget, to make sure that
	     the user sees the right colors at the beginning.  */

	  XWarpPointer (XtDisplay (mesa), None, XtWindow (mesa), 0, 0, 0, 0, 0, 0);
	}
    }
  else
    {
      /* Allocate a few colors for use in color index mode.  */

      Colormap cmap;
      cmap = DefaultColormap (XtDisplay (top), DefaultScreen (XtDisplay (top)));
      Black  = alloc_color (top, cmap, 0x0000, 0x0000, 0x0000);
      White  = alloc_color (top, cmap, 0xffff, 0xffff, 0xffff);
      Red    = alloc_color (top, cmap, 0xffff, 0x0000, 0x0000);
      Green  = alloc_color (top, cmap, 0x0000, 0xffff, 0x0000);
      Blue   = alloc_color (top, cmap, 0x0000, 0x0000, 0xffff);
      Yellow = alloc_color (top, cmap, 0xffff, 0xffff, 0x0000);
    }

  setup_context (mesa);
  setup_context (mesax);
  setup_context (mesay);
  setup_context (mesaz);

  GLwSetFrustumProjection (mesa, -1.0, 1.0, -1.0, 1.0, 1.0, 10.0);
  GLwSetPolarView (mesa, 3, M_PI/2, 0.0);

  GLwBeginProjection (mesax);
  glOrtho (-1.5, 1.5, -1.5, 1.5, 1.0, 10.0);
  GLwEndProjection ();
  GLwPostProjection (mesay, GLwGetProjectionList (mesax));
  GLwPostProjection (mesaz, GLwGetProjectionList (mesax));

  GLwBeginView (mesax);
  gluLookAt (3.0, 0.0, 0.0,  0.0, 0.0, 0.0,  0.0, 1.0, 0.0);
  GLwEndView ();

  GLwBeginView (mesay);
  gluLookAt (0.0, 3.0, 0.0,  0.0, 0.0, 0.0,  0.0, 0.0,-1.0);
  GLwEndView ();

  GLwBeginView (mesaz);
  gluLookAt (0.0, 0.0, 3.0,  0.0, 0.0, 0.0,  0.0, 1.0, 0.0);
  GLwEndView ();

  barrel = glGenLists (1);
  glNewList (barrel, GL_COMPILE);
  {
    draw_barrel_segment (0.9, 1.0,
			 0.3 * M_PI, 0.7 * M_PI,
			 0.1 * M_PI);
  }
  glEndList ();

  particles = glGenLists (1);
  glNewList (particles, GL_COMPILE);
  glEndList ();

  object = glGenLists (1);
  glNewList (object, GL_COMPILE);
  {
    float frac;
    glClear (GL_COLOR_BUFFER_BIT);
    glIndexi (White);
    glColor3f (1.0, 1.0, 1.0);
    for (frac = 0; frac < 1.0; frac += 0.125)
      {
	glPushMatrix ();
	  glRotatef (frac * 360, 0.0, 0.0, 1.0);
	  glCallList (barrel);
	glPopMatrix ();
      }
    glPushMatrix ();
      glCallList (particles);
    glPopMatrix ();
  }
  glEndList ();

#if 0
  greek = glGenLists (128);
  glXUseXFont (XLoadFont (XtDisplay (top), "*-symbol-*-240-*"),
	       0, 128, greek);
#endif
  latin = glGenLists (128);
  glXUseXFont (XLoadFont (XtDisplay (top), "*-times-*-240-*"),
	       0, 128, latin);

  desc_number = glGenLists (1);
  update_number (0);
  desc_angles = glGenLists (1);
  update_angles (0.0, 0.0);

  desc = glGenLists (1);
  glNewList (desc, GL_COMPILE);
  {
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (0, 400, 0, 400, -1.0, 1.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glIndexi (White);
    glColor3f (1.0, 1.0, 1.0);
    glPushAttrib (GL_LIST_BIT);
      glRasterPos2i (10.0, 10.0);
      glListBase (latin);
      glCallList (desc_number);
#if 0
      glRasterPos2i (10.0, 380.0);
      glListBase (greek);
      glCallList (desc_angles);
#endif
    glPopAttrib ();
  }
  glEndList ();
  
  GLwPostObject (mesa, object);
  GLwPostObject (mesax, object);
  GLwPostObject (mesay, object);
  GLwPostObject (mesaz, object);
  GLwPostObject (mesa, desc);

  XtAppMainLoop (app_context);
  return (0);
}

