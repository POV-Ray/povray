/* mcube.c -- Demo program for the Mesa widget
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

   $Id: mcube.c,v 1.2 2000/03/19 23:38:12 brianp Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#ifndef __GLX_MOTIF
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <GL/xmesa.h>
#include <GL/gl.h>
#include <GL/MesaDrawingArea.h>
#else /* __GLX_MOTIF */
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <GL/xmesa.h>
#include <GL/gl.h>
#include <GL/MesaMDrawingArea.h>
#define GLwMakeCurrent GLwMMakeCurrent
#endif /* __GLX_MOTIF */

static char *RCS_Id =
"@(#) $Id: mcube.c,v 1.2 2000/03/19 23:38:12 brianp Exp $";

static GLint Black, Red, Green, Blue;

void quit_function (Widget, XtPointer, XtPointer);

void
quit_function (Widget w, XtPointer closure, XtPointer call_data)
{
  exit (0);
}

static void
draw_cube (void)
{
  /* X faces */
  glIndexi (Red);
  glColor3f (1.0, 0.0, 0.0);
  glBegin (GL_POLYGON);
  {
    glVertex3f (1.0, 1.0, 1.0);
    glVertex3f (1.0, -1.0, 1.0);
    glVertex3f (1.0, -1.0, -1.0);
    glVertex3f (1.0, 1.0, -1.0);
  }
  glEnd ();

  glBegin (GL_POLYGON);
  {
    glVertex3f (-1.0, 1.0, 1.0);
    glVertex3f (-1.0, 1.0, -1.0);
    glVertex3f (-1.0, -1.0, -1.0);
    glVertex3f (-1.0, -1.0, 1.0);
  }
  glEnd ();

  /* Y faces */
  glIndexi (Green);
  glColor3f (0.0, 1.0, 0.0);
  glBegin (GL_POLYGON);
  {
    glVertex3f (1.0, 1.0, 1.0);
    glVertex3f (1.0, 1.0, -1.0);
    glVertex3f (-1.0, 1.0, -1.0);
    glVertex3f (-1.0, 1.0, 1.0);
  }
  glEnd ();

  glBegin (GL_POLYGON);
  {
    glVertex3f (1.0, -1.0, 1.0);
    glVertex3f (-1.0, -1.0, 1.0);
    glVertex3f (-1.0, -1.0, -1.0);
    glVertex3f (1.0, -1.0, -1.0);
  }
  glEnd ();

  /* Z faces */
  glIndexi (Blue);
  glColor3f (0.0, 0.0, 1.0);
  glBegin (GL_POLYGON);
  {
    glVertex3f (1.0, 1.0, 1.0);
    glVertex3f (-1.0, 1.0, 1.0);
    glVertex3f (-1.0, -1.0, 1.0);
    glVertex3f (1.0, -1.0, 1.0);
  }
  glEnd ();

  glBegin (GL_POLYGON);
  {
    glVertex3f (1.0, 1.0, -1.0);
    glVertex3f (1.0, -1.0, -1.0);
    glVertex3f (-1.0, -1.0, -1.0);
    glVertex3f (-1.0, 1.0, -1.0);
  }
  glEnd ();
}

GLfloat xrot, yrot, zrot;

static void
init_display (Widget w)
{
  xrot = yrot = zrot = 0.0;

  GLwMakeCurrent (w);

  glClearColor (0.0, 0.0, 0.0, 0.0);
  glClearIndex (Black);
  glClearDepth (10.0);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glFrustum (-1.0, 1.0, -1.0, 1.0, 1.0, 10.0);
  glTranslatef (0.0, 0.0, -3.0);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glCullFace (GL_BACK);
  glEnable (GL_CULL_FACE);

  glShadeModel (GL_FLAT);
}

static void
display (Widget w)
{
  GLwMakeCurrent (w);
  glClear (GL_COLOR_BUFFER_BIT);
  glPushMatrix ();
  {
    glRotatef (xrot, 1.0, 0.0, 0.0);
    glRotatef (yrot, 0.0, 1.0, 0.0);
    glRotatef (zrot, 0.0, 0.0, 1.0);
    draw_cube ();
  }
  glPopMatrix ();
  glFinish ();
  GLwDrawingAreaSwapBuffers (w);
  /* XMesaSwapBuffers (MesaContext (w)); */
      
  xrot += 1.0;
  yrot += 0.7;
  zrot -= 0.3;
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
      printf ("Couldn't allocate black!\n");
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

/* Just like the movies: do 24 frames per second. */
unsigned long delay = 1000/24;

static void first_frame (Widget);
static void next_frame (XtPointer, XtIntervalId *);

static void
first_frame (Widget w)
{
  XtAppAddTimeOut (XtWidgetToApplicationContext (w),
		   delay, next_frame, (XtPointer) w);
}

static void
next_frame (XtPointer client_data, XtIntervalId *id)
{
  Widget w = (Widget) client_data;
  first_frame (w);
  display (w);
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
  "*MesaDrawingArea.width: 100",
  "*MesaDrawingArea.height: 100",
  "*MesaDrawingArea.debug: false",
  "*MesaDrawingArea.rgba: true",
  "*MesaDrawingArea.installColormap: true",
  "*MesaDrawingArea.doublebuffer: true",
  "*MesaDrawingArea.ximage: false",
  "*mesa.width: 300",
  "*mesa.height: 300",
#else /* __GLX_MOTIF */
  "*MesaMDrawingArea.width: 100",
  "*MesaMDrawingArea.height: 100",
  "*MesaMDrawingArea.debug: false",
  "*MesaMDrawingArea.rgba: true",
  "*MesaMDrawingArea.installColormap: true",
  "*MesaMDrawingArea.doublebuffer: true",
  "*MesaMDrawingArea.ximage: false",
  "*mesa.width: 300",
  "*mesa.height: 300",
#endif /* __GLX_MOTIF */
  NULL
};

int
main (int argc, char *argv[])
{
  Widget top, frame, mesa, mesa1, quit;
  XtAppContext app_context;
  Boolean rgba, cmap_installed;

  XtSetLanguageProc (NULL, NULL, NULL);
  top = XtVaAppInitialize (&app_context, "Mcube",
			   options, XtNumber (options),
			   &argc, argv, fallback_resources,
			   NULL);

#ifndef __GLX_MOTIF
  frame = XtVaCreateManagedWidget ("frame", formWidgetClass,
				   top,
				   NULL);
  mesa = XtVaCreateManagedWidget ("mesa", mesaDrawingAreaWidgetClass,
				  frame,
				  GLwNshareLists, True,
				  NULL);
  mesa1 = XtVaCreateManagedWidget ("mesa1", mesaDrawingAreaWidgetClass,
				   frame,
				   GLwNshareLists, True,
				   XtNwidth, 100, XtNheight, 100,
				   XtNfromHoriz, mesa, XtNhorizDistance, 10,
				   NULL);
  quit = XtVaCreateManagedWidget ("quit", commandWidgetClass,
				  frame,
                                  XtNfromVert, mesa1, XtNvertDistance, 50,
                                  XtNfromHoriz, mesa, XtNhorizDistance, 10,
				  NULL);
  XtAddCallback (quit, XtNcallback, quit_function, NULL);
#else /* __GLX_MOTIF */
  frame = XtVaCreateManagedWidget ("frame", xmFormWidgetClass,
				   top,
				   /* This should not be necessary.  Is it
				      a bug in LessTif or in my understanding
				      of Motif?  */
				   XtNwidth, 430, XtNheight, 320,
				   NULL);
  mesa = XtVaCreateManagedWidget ("mesa", mesaMDrawingAreaWidgetClass,
				  frame,
				  GLwNshareLists, True,
				  XmNtopAttachment, XmATTACH_FORM,
				  XmNtopOffset, 10,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNleftOffset, 10,
				  XmNbottomAttachment,  XmATTACH_FORM,
				  XmNbottomOffset, 10,
				  NULL);
  mesa1 = XtVaCreateManagedWidget ("mesa1", mesaMDrawingAreaWidgetClass,
				   frame,
				   GLwNshareLists, True,
				   XtNwidth, 100, XtNheight, 100,
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNtopOffset, 10,
				   XmNleftAttachment, XmATTACH_WIDGET,
				   XmNleftOffset, 10,
				   XmNleftWidget, mesa,
				   XmNrightAttachment, XmATTACH_FORM,
				   XmNrightOffset, 10,
				   NULL);
  quit = XtVaCreateManagedWidget ("quit", xmPushButtonWidgetClass,
				  frame,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNrightOffset, 10,
				  XmNbottomAttachment, XmATTACH_FORM,
				  XmNbottomOffset, 10,
				  NULL);
  XtAddCallback (quit, XmNarmCallback, quit_function, NULL);
#endif /* __GLX_MOTIF */

  XtRealizeWidget (top);

  XtVaGetValues (mesa,
		 GLwNrgba, &rgba,
		 GLwNinstallColormap, &cmap_installed,
		 NULL);
  if (rgba)
    {
      Black = Red = Green = Blue = 0;

      if (cmap_installed)
	{
	  /* In RGBA mode, the Mesa widgets will have their own color map.
	     Adjust the colors of the other widgets so that--even if the rest
	     of the screen has wrong colors--all application widgets have the
	     right colors.  */
		     
	  translate_pixels (mesa, quit,
			    XtNbackground, XtNforeground, XtNborder, NULL);
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
      Black = alloc_color (top, cmap, 0x0000, 0x0000, 0x0000);
      Red   = alloc_color (top, cmap, 0xffff, 0x0000, 0x0000);
      Green = alloc_color (top, cmap, 0x0000, 0xffff, 0x0000);
      Blue  = alloc_color (top, cmap, 0x0000, 0x0000, 0xffff);
    }

  init_display (mesa);
  init_display (mesa1);

  first_frame (mesa);
  first_frame (mesa1);
  
  XtAppMainLoop (app_context);
  return (0);
}
