/* $Id: wincopy.c,v 1.1 1999/11/25 17:41:51 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.3
 * 
 * Copyright (C) 1999  Brian Paul   All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/*
 * This program opens two GLX windows, renders into one and uses
 * glCopyPixels to copy the image from the first window into the
 * second by means of the GLX 1.3 function glxMakeContextCurrent().
 * This function works just like the glXMakeCurrentReadSGI() function
 * in the GLX_SGI_make_current_read extension.
 */



#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#ifdef GLX_VERSION_1_3


static Display *Dpy;
static int ScrNum;
static GLXContext Context;
static Window Win[2];  /* Win[0] = source,  Win[1] = dest */
static GLint Width[2], Height[2];

static GLfloat Angle = 0.0;



static Window
CreateWindow(Display *dpy, int scrnum, XVisualInfo *visinfo,
             int xpos, int ypos, int width, int height,
             const char *name)
{
   Window win;
   XSetWindowAttributes attr;
   unsigned long mask;
   Window root;

   root = RootWindow(dpy, scrnum);

   /* window attributes */
   attr.background_pixel = 0;
   attr.border_pixel = 0;
   attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
   attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

   win = XCreateWindow(dpy, root, xpos, ypos, width, height,
		        0, visinfo->depth, InputOutput,
		        visinfo->visual, mask, &attr);
   if (win) {
      XSizeHints sizehints;
      sizehints.x = xpos;
      sizehints.y = ypos;
      sizehints.width  = width;
      sizehints.height = height;
      sizehints.flags = USSize | USPosition;
      XSetNormalHints(dpy, win, &sizehints);
      XSetStandardProperties(dpy, win, name, name,
                              None, (char **)NULL, 0, &sizehints);

      XMapWindow(dpy, win);
   }
   return win;
}


static void
Redraw(void)
{
   /* make the first window the current one */
   if (!glXMakeContextCurrent(Dpy, Win[0], Win[0], Context)) {
      printf("glXMakeContextCurrent failed in Redraw()\n");
      return;
   }

   Angle += 1.0;

   glViewport(0, 0, Width[0], Height[0]);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);

   glShadeModel(GL_FLAT);
   glClearColor(0.5, 0.5, 0.5, 1.0);
   glClear(GL_COLOR_BUFFER_BIT);

   /* draw blue quad */
   glColor3f(0.3, 0.3, 1.0);
   glPushMatrix();
   glRotatef(Angle, 0, 0, 1);
   glBegin(GL_POLYGON);
   glVertex2f(-0.5, -0.25);
   glVertex2f( 0.5, -0.25);
   glVertex2f( 0.5, 0.25);
   glVertex2f(-0.5, 0.25);
   glEnd();
   glPopMatrix();

   glXSwapBuffers(Dpy, Win[0]);


   /* copy image from window 0 to window 1 */
   if (!glXMakeContextCurrent(Dpy, Win[1], Win[0], Context)) {
      printf("glXMakeContextCurrent failed in Redraw()\n");
      return;
   }

   /* raster pos setup */
   glViewport(0, 0, Width[1], Height[1]);
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(-1, 1, -1, 1, -1, 1);
   glRasterPos2f(-1, -1);

   /* copy the image between windows */
   glDrawBuffer(GL_FRONT);
   glCopyPixels(0, 0, Width[0], Height[0], GL_COLOR);
   glDrawBuffer(GL_BACK);

   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
}



static void
Resize(Window win, unsigned int width, unsigned int height)
{
   int i;
   if (win == Win[0]) {
      i = 0;
   }
   else {
      i = 1;
   }
   Width[i] = width;
   Height[i] = height;
   if (!glXMakeCurrent(Dpy, Win[i], Context)) {
      printf("glXMakeCurrent failed in Resize()\n");
      return;
   }
}



static void
EventLoop(void)
{
   XEvent event;
   while (1) {
      if (XPending(Dpy) > 0) {
         XNextEvent( Dpy, &event );
         switch (event.type) {
            case Expose:
               Redraw();
               break;
            case ConfigureNotify:
               Resize(event.xany.window, event.xconfigure.width, event.xconfigure.height);
               break;
            case KeyPress:
               return;
            default:
               /*no-op*/ ;
         }
      }
      else {
         /* animate */
         Redraw();
      }
   }
}


static void
Init(void)
{
   XVisualInfo *visinfo;
   int attrib[] = { GLX_RGBA,
		    GLX_RED_SIZE, 1,
		    GLX_GREEN_SIZE, 1,
		    GLX_BLUE_SIZE, 1,
		    GLX_DOUBLEBUFFER,
		    None };

   Dpy = XOpenDisplay(NULL);
   if (!Dpy) {
      printf("Couldn't open default display!\n");
      exit(1);
   }

   ScrNum = DefaultScreen(Dpy);

   visinfo = glXChooseVisual(Dpy, ScrNum, attrib);
   if (!visinfo) {
      printf("Unable to find RGB, double-buffered visual\n");
      exit(1);
   }

   Context = glXCreateContext(Dpy, visinfo, NULL, True);
   if (!Context) {
      printf("Couldn't create GLX context\n");
      exit(1);
   }


   Win[0] = CreateWindow(Dpy, ScrNum, visinfo,
                         0, 0, 300, 300, "source window");

   Win[1] = CreateWindow(Dpy, ScrNum, visinfo,
                         350, 0, 300, 300, "dest window");

}


int
main(int argc, char *argv[])
{
   Init();
   EventLoop();
   return 0;
}


#else


int
main(int argc, char *argv[])
{
   printf("This program requires GLX 1.3!\n");
   return 0;
}


#endif /* GLX_VERSION_1_3 */
