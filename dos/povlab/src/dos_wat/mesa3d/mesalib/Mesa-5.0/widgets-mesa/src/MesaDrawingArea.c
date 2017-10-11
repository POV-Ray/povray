/* MesaDrawingArea.c -- Implementation file for the Mesa widget
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

   $Id: MesaDrawingArea.c,v 1.2 2000/11/23 16:36:36 brianp Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#ifdef __GLX_MOTIF
#include <GL/MesaMDrawingAreaP.h>
#else
#include <GL/MesaDrawingAreaP.h>
#endif
#ifndef GLwDebug
#define GLwDebug(w) 0
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
#endif


/* Resources. */

#define offset(_field)  XtOffsetOf(MesaDrawingAreaRec, mesaDrawingArea._field)
static XtResource resources[] =
{
  /* name, class, type, size,
     offset, default_type, default_addr */
  {GLwNximage, GLwCXImage, XtRBoolean, sizeof (Boolean),
   offset (ximage), XtRBoolean, (caddr_t) False},
  {GLwNshareLists, GLwCShareLists, XtRBoolean, sizeof (Boolean),
   offset (share_lists), XtRBoolean, (caddr_t) False},
  {GLwNshareListsWith, GLwCShareListsWith, XtRWidget, sizeof (Widget),
   offset (share_lists_with), XtRWidget, NULL},
};
#undef offset

/* Basic widget methods.  */

/* Initialize this widget class.  Currently we just clear the pointer
   to the widget we're sharing lists with.  */

static void
ClassInitialize (void)
{
  MesaListsRoot = NULL;
}

/* Initialize a widget instance.  */

static void
Initialize (Widget request, Widget new, ArgList args, Cardinal * num_args)
{
  LOG (new);
 ((MesaDrawingAreaWidget)new)->mesaDrawingArea.buffer = NULL;
}

static void
Realize (Widget w, XtValueMask *mask,
	 XSetWindowAttributes *attr)
{
  XMesaContext share_lists_with;
  
  LOG (w);

  /* Trying to outsmart ourselves with the cute looking
     `(XtSuperclass (w)->core_class.realize) (w, mask, attr);'
     is a BAD idea, because it will lead to infinite recursion if a
     subclass has the same idea ...  */

  (glwDrawingAreaClassRec.core_class.realize) (w, mask, attr);

  if (MesaShareLists (w) && (MesaListsRoot != NULL))
    {
      if (GLwDebug(w))
	fprintf (stderr, "sharing lists with %p...\n", MesaListsRoot);
      share_lists_with = MesaContext (MesaListsRoot);
    }
  else
    {
      if (GLwDebug(w))
	fprintf (stderr, "not sharing lists...\n");
      share_lists_with = NULL;
    }

  /* Attach a Mesa rendering context to the window */
  MesaVisual (w) =
    XMesaCreateVisual (XtDisplay (w),
		       ((MesaDrawingAreaWidget)w)->glwDrawingArea.visualInfo,
		       (GLboolean) MesaRGBA (w),
		       (MesaAlphaSize(w) > 0) ? GL_TRUE : GL_FALSE,
		       (GLboolean) MesaDoublebuffer (w),
                       GL_FALSE, /* stereo */
		       (GLboolean) MesaXImage (w),
		       MesaDepthSize(w),
		       MesaStencilSize(w),
		       MesaAccumRedSize(w),
		       MesaAccumGreenSize(w),
		       MesaAccumBlueSize(w),
		       MesaAccumAlphaSize(w),
                       (GLint) 0, /* num_samples */
                       (GLint) 0, /* level */
                       GLX_NONE_EXT /* visualCaveat */);
  if (!MesaVisual (w))
    {
      printf ("Couldn't create Mesa/X visual!\n");
      exit (1);
    }

  MesaContext (w) = XMesaCreateContext (MesaVisual (w), share_lists_with);
  if (!MesaContext (w))
    {
      printf ("Couldn't create Mesa/X context!\n");
      exit (1);
    }
  if (MesaShareLists (w) && (MesaListsRoot == NULL))
    {
      if (GLwDebug(w))
	fprintf (stderr, "installing %p as MesaListsRoot...\n", w);
      MesaListsRoot = w;
    }

#ifdef AVOID_MESABUFFER_INTERFACE
  glXMakeCurrent (XtDisplay (w), XtWindow (w), MesaContext (w));
#else
  MesaBuffer (w) = XMesaCreateWindowBuffer (MesaVisual (w), XtWindow (w));
  if (!MesaBuffer (w))
    {
      printf ("Couldn't create Mesa/X buffer!\n");
      exit (1);
    }

  XMesaMakeCurrent (MesaContext (w), MesaBuffer (w));
#endif
  if (GLwDebug(w))
    fprintf (stderr, "finished with Realize() of %p.\n", w);
}

static void
Destroy (Widget w)
{
  LOG (w);
#ifdef AVOID_MESABUFFER_INTERFACE
  XMesaDestroyContext (MesaContext (w));
  MesaContext (w) = NULL;
  XMesaDestroyVisual (MesaVisual (w));
  MesaVisual (w) = NULL;
#else
  XMesaDestroyBuffer (MesaBuffer (w));
  MesaBuffer (w) = NULL;
  XMesaDestroyContext (MesaContext (w));
  MesaContext (w) = NULL;
  XMesaDestroyVisual (MesaVisual (w));
  MesaVisual (w) = NULL;
#endif
}

/* Resizing the widget can be done by adjusting the viewport.
   However, we must make sure that the correct XMesaContext is
   active.  It is also just common courtesy for an event handler
   to restore the old XMesaContext afterwards.   */

static void
Resize (Widget w)
{
  LOG (w);

  /* Ignore while there is no Mesa context.  */
  if (XtIsRealized (w) && MesaContext (w))
    {
#ifdef AVOID_MESABUFFER_INTERFACE
      GLXDrawable drawable = glXGetCurrentDrawable ();
      GLXContext context = glXGetCurrentContext ();
      glXMakeCurrent (XtDisplay (w), XtWindow (w), MesaContext (w));
      glViewport (0, 0, w->core.width, w->core.height);
      glXMakeCurrent (XtDisplay (w), drawable, context);
#else
      XMesaBuffer buffer = XMesaGetCurrentBuffer ();
      XMesaContext context = XMesaGetCurrentContext ();
      XMesaMakeCurrent (MesaContext (w), MesaBuffer (w));
      glViewport (0, 0, w->core.width, w->core.height);
      XMesaMakeCurrent (context, buffer);
#endif
    }

  /* Call our superclass' Resize method.  */
  (glwDrawingAreaClassRec.core_class.resize) (w);
}


/* Now use all these methods in the widget class record.  */

MesaDrawingAreaClassRec mesaDrawingAreaClassRec =
{
  {
    /* superclass            */ (WidgetClass) &glwDrawingAreaClassRec,
#ifdef __GLX_MOTIF
    /* class_name            */ "MesaMDrawingArea",
#else
    /* class_name            */ "MesaDrawingArea",
#endif
    /* widget_size           */ sizeof (MesaDrawingAreaRec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ NULL,
    /* class_inited          */ FALSE,
    /* initialize            */ Initialize,
    /* initialize_hook       */ NULL,
    /* realize               */ Realize,
    /* actions               */ NULL,
    /* num_actions           */ 0,
    /* resources             */ resources,
    /* num_resources         */ XtNumber (resources),
    /* xrm_class             */ NULLQUARK,
    /* compress_motion       */ TRUE,
    /* compress_exposure     */ TRUE,
    /* compress_enterleave   */ TRUE,
    /* visible_interest      */ FALSE,
    /* destroy               */ Destroy,
    /* resize                */ (XtWidgetProc) Resize,
    /* expose                */ XtInheritExpose,
    /* set_values            */ NULL,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* accept_focus          */ NULL,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* tm_table              */ NULL,
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
    /* RCS_id                */
    "@(#) $Id: MesaDrawingArea.c,v 1.2 2000/11/23 16:36:36 brianp Exp $"
  }
};

WidgetClass mesaDrawingAreaWidgetClass
  = (WidgetClass) & mesaDrawingAreaClassRec;

/* The End. */

