/* GLwDrawingAreaMakeCurrent.c -- Mesa GL Widget for X11 Toolkit Programming
   Copyright (C) 1995, 1996 by
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

   $Id: GLwDrawingAreaMakeCurrent.c,v 1.1.1.1 1999/08/19 00:55:43 jtg Exp $
 */

#include <X11/Intrinsic.h>
#include <GL/glx.h>

/* Set current drawing context */

void 
GLwDrawingAreaMakeCurrent (Widget w, GLXContext ctx)
{
  glXMakeCurrent (XtDisplay (w), XtWindow(w), ctx);
}

/* The End.  */
