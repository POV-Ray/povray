/* MesaWorkstation.c -- Implementation file for the Mesa Workstation widget
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

   $Id: MesaWorkstation.c,v 1.1.1.1 1999/08/19 00:55:43 jtg Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <GL/glu.h>
#include <GL/xmesa.h>
#ifdef __GLX_MOTIF
#include <GL/MesaMWorkstationP.h>
#else
#include <GL/MesaWorkstationP.h>
#endif

#if STDC_HEADERS
# include <string.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr (), *strrchr ();
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
# endif
# ifndef HAVE_MEMMOVE
#  define memmove(d, s, n) bcopy ((s), (d), (n))
# endif
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __GLX_MOTIF
#define GLwDrawingAreaWidget       GLwMDrawingAreaWidget
#define GLwDrawingAreaClassRec     GLwMDrawingAreaClassRec
#define GLwDrawingAreaRec          GLwMDrawingAreaRec
#define glwDrawingAreaClassRec     glwMDrawingAreaClassRec
#define glwDrawingAreaWidgetClass  glwMDrawingAreaWidgetClass
#define MesaDrawingAreaWidget      MesaMDrawingAreaWidget
#define MesaDrawingAreaClassRec    MesaMDrawingAreaClassRec
#define MesaDrawingAreaRec         MesaMDrawingAreaRec
#define mesaDrawingAreaClassRec    mesaMDrawingAreaClassRec
#define mesaDrawingAreaWidgetClass mesaMDrawingAreaWidgetClass
#define MesaWorkstationWidget      MesaMWorkstationWidget
#define MesaWorkstationClassRec    MesaMWorkstationClassRec
#define MesaWorkstationRec         MesaMWorkstationRec
#define mesaWorkstationClassRec    mesaMWorkstationClassRec
#define mesaWorkstationWidgetClass mesaMWorkstationWidgetClass
#define GLwPostObject		   GLwMPostObject
#define GLwUnpostObject		   GLwMUnpostObject
#define GLwUnpostAllObjects	   GLwMUnpostAllObjects
#define GLwBeginView		   GLwMBeginView
#define GLwEndView		   GLwMEndView
#define GLwPostViewList		   GLwMPostViewList
#define GLwPostViewMatrix	   GLwMPostViewMatrix
#define GLwPostCurrentView	   GLwMPostCurrentView
#define GLwUnpostView		   GLwMUnpostView
#define GLwSetPolarView		   GLwMSetPolarView
#define GLwGetViewList		   GLwMGetViewList
#define GLwGetViewMatrix	   GLwMGetViewMatrix
#define GLwBeginProjection	   GLwMBeginProjection
#define GLwEndProjection	   GLwMEndProjection
#define GLwPostProjectionList	   GLwMPostProjectionList
#define GLwPostProjectionMatrix	   GLwMPostProjectionMatrix
#define GLwPostCurrentProjection   GLwMPostCurrentProjection
#define GLwUnpostProjection	   GLwMUnpostProjection
#define GLwSetFrustumProjection	   GLwMSetFrustumProjection
#define GLwSetOrthoProjection	   GLwMSetOrthoProjection
#define GLwGetProjectionList	   GLwMGetProjectionList
#define GLwGetProjectionMatrix	   GLwMGetProjectionMatrix
#define GLwRedrawObjects	   GLwMRedrawObjects
#define GLwPostView		   GLwMPostView
#define GLwGetView		   GLwMGetView
#define GLwPostProjection	   GLwMPostProjection
#define GLwGetProjection	   GLwMGetProjection
#define GLwMakeCurrent             GLwMMakeCurrent
#endif

#define MesaProjection(_widget) \
   (((MesaWorkstationWidget)_widget)->mesaWorkstation.projection)
#define MesaView(_widget) \
   (((MesaWorkstationWidget)_widget)->mesaWorkstation.view)
#define MesaNextObject(_widget) \
   (((MesaWorkstationWidget)_widget)->mesaWorkstation.next_object)
#define MesaAllocatedObjects(_widget) \
   (((MesaWorkstationWidget)_widget)->mesaWorkstation.allocated_objects)
#define MesaObjects(_widget) \
   (((MesaWorkstationWidget)_widget)->mesaWorkstation.objects)


/* Private utility functions. */


/* Resources. */


/* Actions and their translations. */

static void
Projection (Widget w, XEvent *event, String *argv, Cardinal *argc)
{
  LOG (w);

  if (*argc == 0)
    return;

  if ((MesaProjection(w).type != FRUSTUM)
      && (MesaProjection(w).type != ORTHO))
    return;

  switch (*argv[0])
    {
    case 'p':
      MesaProjection(w).type = FRUSTUM;
      break;
    case 'o':
      MesaProjection(w).type = ORTHO;
      break;
    case 'l':
      MesaProjection(w).u.vol.left *= 1.1;
      break;
    case 'L':
      MesaProjection(w).u.vol.left /= 1.1;
      break;
    case 'r':
      MesaProjection(w).u.vol.right *= 1.1;
      break;
    case 'R':
      MesaProjection(w).u.vol.right /= 1.1;
      break;
    case 'b':
      MesaProjection(w).u.vol.bottom *= 1.1;
      break;
    case 'B':
      MesaProjection(w).u.vol.bottom /= 1.1;
      break;
    case 't':
      MesaProjection(w).u.vol.top *= 1.1;
      break;
    case 'T':
      MesaProjection(w).u.vol.top /= 1.1;
      break;
    case 'n':
      MesaProjection(w).u.vol.near *= 1.1;
      break;
    case 'N':
      MesaProjection(w).u.vol.near /= 1.1;
      break;
    case 'f':
      MesaProjection(w).u.vol.far *= 1.1;
      break;
    case 'F':
      MesaProjection(w).u.vol.far /= 1.1;
      break;
    case 'a':
      MesaProjection(w).u.vol.left *= 1.1;
      MesaProjection(w).u.vol.right *= 1.1;
      MesaProjection(w).u.vol.bottom *= 1.1;
      MesaProjection(w).u.vol.top *= 1.1;
      break;
    case 'A':
      MesaProjection(w).u.vol.left /= 1.1;
      MesaProjection(w).u.vol.right /= 1.1;
      MesaProjection(w).u.vol.bottom /= 1.1;
      MesaProjection(w).u.vol.top /= 1.1;
      break;
    }

  GLwRedrawObjects (w);
}

static void
Move (Widget w, XEvent *event, String *argv, Cardinal *argc)
{
  double scale = 0.01;
  LOG (w);

  if (*argc == 0)
    return;

  if (MesaView(w).type != POLAR)
    return;

  if (*argc >= 2)
    scale *= atof (argv[1]);

  switch (*argv[0])
    {
    case '+':
      MesaView(w).u.polar.r /= 1.1;
      break;
    case '-':
      MesaView(w).u.polar.r *= 1.1;
      break;
    case 'l':
      MesaView(w).u.polar.phi += scale * M_PI;
      break;
    case 'r':
      MesaView(w).u.polar.phi -= scale * M_PI;
      break;
    case 'u':
      MesaView(w).u.polar.theta -= scale * M_PI;
      break;
    case 'd':
      MesaView(w).u.polar.theta += scale * M_PI;
      break;
    }

  GLwRedrawObjects (w);
}

static XtActionsRec actions[] =
{
  /* {name, procedure} */
  {"Move", Move },
  {"Projection", Projection },
};

static char translations[] =
#ifdef __GLX_MOTIF
"~Shift<Key>osfLeft: Move(l)\n\
Shift<Key>osfLeft: Move(l,10)\n\
~Shift<Key>osfRight: Move(r)\n\
Shift<Key>osfRight: Move(r,10)\n\
~Shift<Key>osfUp: Move(u)\n\
Shift<Key>osfUp: Move(u,10)\n\
~Shift<Key>osfDown: Move(d)\n\
Shift<Key>osfDown: Move(d,10)\n\
~Shift<Key>Left: Move(l)\n\
Shift<Key>Left: Move(l,10)\n\
~Shift<Key>Right: Move(r)\n\
Shift<Key>Right: Move(r,10)\n\
~Shift<Key>Up: Move(u)\n\
Shift<Key>Up: Move(u,10)\n\
~Shift<Key>Down: Move(d)\n\
Shift<Key>Down: Move(d,10)\n\
<Key>plus: Move(+)\n\
<Key>minus: Move(-)\n\
~Shift<Key>l: Projection(l)\n\
Shift<Key>l: Projection(L)\n\
~Shift<Key>r: Projection(r)\n\
Shift<Key>r: Projection(R)\n\
~Shift<Key>b: Projection(b)\n\
Shift<Key>b: Projection(B)\n\
~Shift<Key>t: Projection(t)\n\
Shift<Key>t: Projection(T)\n\
~Shift<Key>n: Projection(n)\n\
Shift<Key>n: Projection(N)\n\
~Shift<Key>f: Projection(f)\n\
Shift<Key>f: Projection(F)\n\
~Shift<Key>a: Projection(a)\n\
Shift<Key>a: Projection(A)\n\
<Key>p: Projection(p)\n\
<Key>o: Projection(o)\n";
#else /* __GLX_MOTIF */
"~Shift<Key>Left: Move(l)\n\
Shift<Key>Left: Move(l,10)\n\
~Shift<Key>Right: Move(r)\n\
Shift<Key>Right: Move(r,10)\n\
~Shift<Key>Up: Move(u)\n\
Shift<Key>Up: Move(u,10)\n\
~Shift<Key>Down: Move(d)\n\
Shift<Key>Down: Move(d,10)\n\
<Key>plus: Move(+)\n\
<Key>minus: Move(-)\n\
~Shift<Key>l: Projection(l)\n\
Shift<Key>l: Projection(L)\n\
~Shift<Key>r: Projection(r)\n\
Shift<Key>r: Projection(R)\n\
~Shift<Key>b: Projection(b)\n\
Shift<Key>b: Projection(B)\n\
~Shift<Key>t: Projection(t)\n\
Shift<Key>t: Projection(T)\n\
~Shift<Key>n: Projection(n)\n\
Shift<Key>n: Projection(N)\n\
~Shift<Key>f: Projection(f)\n\
Shift<Key>f: Projection(F)\n\
~Shift<Key>a: Projection(a)\n\
Shift<Key>a: Projection(A)\n\
<Key>p: Projection(p)\n\
<Key>o: Projection(o)\n";
#endif /* __GLX_MOTIF */


/* Basic widget methods.  */

#define OBJECT_CHUNK_SIZE 10

static void
Initialize (Widget request, Widget new, ArgList args, Cardinal * num_args)
{
  LOG (new);
  MesaAllocatedObjects (new) = OBJECT_CHUNK_SIZE;
  MesaObjects (new) = (GLuint *)
    XtCalloc (MesaAllocatedObjects (new), sizeof (GLuint));
  MesaNextObject (new) = MesaObjects (new);
  MesaView(new).type = NOVIEW;
  MesaView(new).list = 0;
  MesaProjection(new).type = NOPROJ;
  MesaProjection(new).list = 0;
}

#if 0
static void
Realize (Widget w, XtValueMask * mask,
	 XSetWindowAttributes * attr)
{
  LOG (w);
  (XtSuperclass (w)->core_class.realize) (w, mask, attr);
}
#endif

static void
Destroy (Widget w)
{
  LOG (w);
  XtFree ((char *) MesaObjects (w));
}

static void
Redisplay (Widget w, XEvent *event, Region region)
{
  LOG (w);
  if (event->xexpose.count == 0)	/* last Expose event */
    GLwRedrawObjects (w);
}


/* Now use all these methods in the widget class record.  */

MesaWorkstationClassRec mesaWorkstationClassRec =
{
  {
    /* superclass            */ (WidgetClass) &mesaDrawingAreaClassRec,
#ifdef __GLX_MOTIF
    /* class_name            */ "MesaMWorkstation",
#else
    /* class_name            */ "MesaWorkstation",
#endif
    /* widget_size           */ sizeof (MesaWorkstationRec),
    /* class_initialize      */ NULL,
    /* class_part_initialize */ NULL,
    /* class_inited          */ FALSE,
    /* initialize            */ Initialize,
    /* initialize_hook       */ NULL,
    /* realize               */ XtInheritRealize,
    /* actions               */ actions,
    /* num_actions           */ XtNumber (actions),
    /* resources             */ NULL,
    /* num_resources         */ 0,
    /* xrm_class             */ NULLQUARK,
    /* compress_motion       */ TRUE,
    /* compress_exposure     */ TRUE,
    /* compress_enterleave   */ TRUE,
    /* visible_interest      */ FALSE,
    /* destroy               */ Destroy,
    /* resize                */ XtInheritResize,
    /* expose                */ Redisplay,
    /* set_values            */ NULL,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* accept_focus          */ NULL,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* tm_table              */ translations,
    /* query_geometry        */ XtInheritQueryGeometry,
    /* display_accelerator   */ XtInheritDisplayAccelerator,
    /* extension             */ NULL
  },
#ifdef __GLX_MOTIF
  { /* XmPrimitive fields */
    /* border_highlight      */ (XtWidgetProc) _XtInherit,
    /* border_unhighlight    */ (XtWidgetProc) _XtInherit,
    /* translations          */ XtInheritTranslations,
    /* arm_and_activate      */ NULL,
    /* get_resources         */ NULL,
    /* num get_resources     */ 0,
    /* extension             */ NULL,
  },
#endif /* __GLX_MOTIF */
  { /* MesaDrawingArea fields*/
    /* RCS_id                */ NULL
  },
  { /* MesaWorkstation fields*/
    /* RCS_id                */
    "@(#) $Id: MesaWorkstation.c,v 1.1.1.1 1999/08/19 00:55:43 jtg Exp $"
  }
};
WidgetClass mesaWorkstationWidgetClass
  = (WidgetClass) & mesaWorkstationClassRec;


/* More private utility functions. */

static int
is_workstation (Widget w)
{
  if (XtClass (w) != mesaWorkstationWidgetClass)
    {
      XtAppError (XtWidgetToApplicationContext (w),
#ifndef __GLX_MOTIF
		  "Not a Mesa Workstation!");
#else
		  "Not a Mesa/Motif Workstation!");
#endif
      return 0;
    }
  else
    return 1;
}

static void
mesa_frustum (volume v)
{
  glFrustum (v.left, v.right, v.bottom, v.top, v.near, v.far);
}

static void
mesa_ortho (volume v)
{
  glOrtho (v.left, v.right, v.bottom, v.top, v.near, v.far);
}

static void
mesa_look_at (look_at l)
{
  gluLookAt (l.eyex, l.eyex, l.eyez,
	     l.ctrx, l.ctrx, l.ctrz,
	     l.upx, l.upx, l.upz);
}


static void
mesa_polar (polar p)
{
  GLdouble r, sin_th, cos_th, sin_phi, cos_phi, u_sin_th, u_cos_th;
  r = p.r;
  sin_th = sin (p.theta);
  cos_th = cos (p.theta);
  sin_phi = sin (p.phi);
  cos_phi = cos (p.phi);
  u_sin_th = cos_th;
  u_cos_th = -sin_th;
  gluLookAt (r*sin_th*cos_phi, r*cos_th, r*sin_th*sin_phi,
	     0.0, 0.0, 0.0,
	     u_sin_th*cos_phi, u_cos_th, u_sin_th*sin_phi);
}


/* Exported utilility functions.  */
void
GLwPostObject (Widget w, GLuint object)
{
  if (is_workstation (w))
    {
      if (MesaNextObject (w) > MesaObjects (w) + MesaAllocatedObjects (w))
	{
	  MesaAllocatedObjects (w) += OBJECT_CHUNK_SIZE;
	  MesaObjects (w) = (GLuint *)
	    XtRealloc ((char *) MesaObjects (w),
		       sizeof (GLuint) * MesaAllocatedObjects (w));
	}
      *(MesaNextObject(w)++) = object;
    }
}

void
GLwUnpostObject (Widget w, GLuint object)
{
  if (is_workstation (w))
    {
      GLuint *p;
      for (p = MesaObjects (w); p < MesaNextObject (w); p++)
	if (*p == object)
	  {
	    memmove (p, p + 1,
		     sizeof (GLuint) * (MesaNextObject (w) - p - 1));
	    MesaNextObject(w)--;
	    break;
	  }
    }
}

void
GLwUnpostAllObjects (Widget w)
{
  if (is_workstation (w))
    MesaNextObject (w) = MesaObjects (w);
}

void
GLwBeginView (Widget w)
{
  if (is_workstation (w))
    {
      MesaView(w).type = VIEW_LIST;
      if (!glIsList (MesaView(w).list))
	MesaView(w).list = glGenLists (1);
      glNewList (MesaView(w).list, GL_COMPILE);
    }
}

void
GLwEndView (void)
{
  glEndList ();
}

void
GLwPostViewList (Widget w, GLuint view)
{
  if (is_workstation (w))
    {
      MesaView(w).type = VIEW_LIST;
      MesaView(w).list = view;
    }
}

void
GLwPostViewMatrix (Widget w, GLdouble *m)
{
  if (is_workstation (w))
    {
      MesaView(w).type = VIEW_MATRIX;
      memcpy (MesaView(w).u.m, m, sizeof (MesaView(w).u.m));
    }
}

void
GLwPostCurrentView (Widget w)
{
  if (is_workstation (w))
    {
      MesaView(w).type = VIEW_MATRIX;
      glGetDoublev (GL_MODELVIEW_MATRIX, MesaView(w).u.m);
    }
}

void
GLwUnpostView (Widget w)
{
  if (is_workstation (w))
    MesaView(w).type = NOVIEW;
}

void
GLwSetPolarView (Widget w, GLdouble r, GLdouble theta, GLdouble phi)
{
  if (is_workstation (w))
    {
      MesaView(w).type = POLAR;
      MesaView(w).u.polar.r = r;
      MesaView(w).u.polar.theta = theta;
      MesaView(w).u.polar.phi = phi;
    }
}

GLuint
GLwGetViewList (Widget w)
{
  if (is_workstation (w))
    {
      if (MesaView(w).type == VIEW_LIST)
	return MesaView(w).list;
    }
  return 0;
}

int
GLwGetViewMatrix (Widget w, GLdouble *m)
{
  if (is_workstation (w))
    {
      if (MesaView(w).type == VIEW_MATRIX)
	memcpy (m, MesaView(w).u.m, sizeof (MesaView(w).u.m));
      return 1;
    }
  return 0;
}

void
GLwBeginProjection (Widget w)
{
  if (is_workstation (w))
    {
      MesaProjection(w).type = PROJ_LIST;
      if (!glIsList (MesaProjection(w).list))
	MesaProjection(w).list = glGenLists (1);
      glNewList (MesaProjection(w).list, GL_COMPILE);
    }
}

void
GLwEndProjection (void)
{
  glEndList ();
}

void
GLwPostProjectionList (Widget w, GLuint proj)
{
  if (is_workstation (w))
    {
      MesaProjection(w).type = PROJ_LIST;
      MesaProjection(w).list = proj;
    }
}

void
GLwPostProjectionMatrix (Widget w, GLdouble *m)
{
  if (is_workstation (w))
    {
      MesaProjection(w).type = PROJ_MATRIX;
      memcpy (MesaProjection(w).u.m, m, sizeof (MesaProjection(w).u.m));
    }
}

void
GLwPostCurrentProjection (Widget w)
{
  if (is_workstation (w))
    {
      MesaProjection(w).type = PROJ_MATRIX;
      glGetDoublev (GL_PROJECTION_MATRIX, MesaProjection(w).u.m);
    }
}

void
GLwUnpostProjection (Widget w)
{
  if (is_workstation (w))
    MesaProjection(w).type = NOPROJ;
}

void
GLwSetFrustumProjection (Widget w,
			 GLdouble left, GLdouble right,
			 GLdouble bottom, GLdouble top,
			 GLdouble near, GLdouble far)
{
  if (is_workstation (w))
    {
      MesaProjection(w).type = FRUSTUM;
      MesaProjection(w).u.vol.left = left;
      MesaProjection(w).u.vol.right = right;
      MesaProjection(w).u.vol.bottom = bottom;
      MesaProjection(w).u.vol.top = top;
      MesaProjection(w).u.vol.near = near;
      MesaProjection(w).u.vol.far = far;
    }
}

void
GLwSetOrthoProjection (Widget w,
			 GLdouble left, GLdouble right,
			 GLdouble bottom, GLdouble top,
			 GLdouble near, GLdouble far)
{
  if (is_workstation (w))
    {
      MesaProjection(w).type = ORTHO;
      MesaProjection(w).u.vol.left = left;
      MesaProjection(w).u.vol.right = right;
      MesaProjection(w).u.vol.bottom = bottom;
      MesaProjection(w).u.vol.top = top;
      MesaProjection(w).u.vol.near = near;
      MesaProjection(w).u.vol.far = far;
    }
}

GLuint
GLwGetProjectionList (Widget w)
{
  if (is_workstation (w))
    {
      if (MesaProjection(w).type == PROJ_LIST)
	return MesaProjection(w).list;
    }
  return 0;
}

int
GLwGetProjectionMatrix (Widget w, GLdouble *m)
{
  if (is_workstation (w))
    {
      if (MesaProjection(w).type == PROJ_MATRIX)
	memcpy (m, MesaProjection(w).u.m, sizeof (MesaProjection(w).u.m));
      return 1;
    }
  return 0;
}

void
GLwRedrawObjects (Widget w)
{
  if (is_workstation (w))
    {
      XMesaBuffer buffer;
      XMesaContext context;

      GLwMakeCurrent(w);
      buffer = XMesaGetCurrentBuffer();
      context = XMesaGetCurrentContext();

      XMesaMakeCurrent (MesaContext (w), MesaBuffer (w));
      if (MesaProjection(w).type != NOPROJ)
	{
	  glMatrixMode (GL_PROJECTION);
	  glPushMatrix ();
	  glLoadIdentity ();
	}
      switch (MesaProjection(w).type)
	{
	case PROJ_MATRIX:
	  glLoadMatrixd (MesaProjection(w).u.m);
	  break;
	case PROJ_LIST:
	  if (!glIsList (MesaProjection(w).list))
	    XtAppWarning (XtWidgetToApplicationContext (w),
			  "Invalid projection list!");
	  else
	    glCallList (MesaProjection(w).list);
	  break;
	case FRUSTUM:
	  mesa_frustum (MesaProjection(w).u.vol);
	  break;
	case ORTHO:
	  mesa_ortho (MesaProjection(w).u.vol);
	  break;
	case NOPROJ:
	  break;
	}
      glMatrixMode (GL_MODELVIEW);
      glPushMatrix ();
      {
	GLuint *obj;
	glLoadIdentity ();
	switch (MesaView(w).type)
	  {
	  case PROJ_MATRIX:
	    glLoadMatrixd (MesaView(w).u.m);
	    break;
	  case PROJ_LIST:
	    if (!glIsList (MesaView(w).list))
	      XtAppWarning (XtWidgetToApplicationContext (w),
			    "Invalid view list!");
	    else
	      glCallList (MesaView(w).list);
	    break;
	  case LOOK_AT:
	    mesa_look_at (MesaView(w).u.look_at);
	    break;
	  case POLAR:
	    mesa_polar (MesaView(w).u.polar);
	    break;
	  case NOVIEW:
	    break;
	  }
	for (obj = MesaObjects (w); obj < MesaNextObject (w); obj++)
	  {
	    if (!glIsList (*obj))
	      XtAppWarning (XtWidgetToApplicationContext (w),
			    "Invalid object list!");
	    else
	      {
		glPushMatrix ();
		  glCallList (*obj);
		glPopMatrix ();
	      }
	  }	     
      }
      glPopMatrix ();
      if (MesaProjection(w).type != NOVIEW)
	{
	  glMatrixMode (GL_PROJECTION);
	  glPopMatrix ();
	  glMatrixMode (GL_MODELVIEW);
	}
      glFinish ();
      XMesaSwapBuffers (buffer);
      XMesaMakeCurrent (context, buffer);
    }
}

/* obsolete aliases: */

void
GLwPostView (Widget w, GLuint view)
{
  GLwPostViewList (w, view);
}

GLuint
GLwGetView (Widget w)
{
  return GLwGetViewList (w);
}

void
GLwPostProjection (Widget w, GLuint proj)
{
  GLwPostProjectionList (w, proj);
}

GLuint
GLwGetProjection (Widget w)
{
  return GLwGetProjectionList (w);
}

/* The End. */

