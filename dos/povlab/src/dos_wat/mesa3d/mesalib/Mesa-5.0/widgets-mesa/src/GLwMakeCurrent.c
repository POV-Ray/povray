/* GLwMakeCurrent.c -- Implementation file for the Mesa widget
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

   $Id: GLwMakeCurrent.c,v 1.1.1.1 1999/08/19 00:55:43 jtg Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <GL/MesaDrawingAreaP.h>

void
GLwMakeCurrent (Widget w)
{
  if (XtIsSubclass (w, mesaDrawingAreaWidgetClass))
#ifdef AVOID_MESABUFFER_INTERFACE
    glXMakeCurrent (XtDisplay (w), XtWindow (w), MesaContext (w));
#else
    XMesaMakeCurrent (MesaContext (w), MesaBuffer (w));
#endif
  else
    XtAppError (XtWidgetToApplicationContext (w),
		"Not a MesaDrawingArea widget!");
}

/* The End. */

