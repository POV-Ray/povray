/* GLwDrawingArea.c -- Mesa GL Widget for X11 Toolkit Programming
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

   $Id: GLwDrawingArea.c,v 1.2 2000/03/19 23:40:50 brianp Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#ifdef __GLX_MOTIF
#include <Xm/XmP.h>
#include <Xm/PrimitiveP.h>
#include <GL/GLwMDrawAP.h>
#else
#include <X11/CoreP.h>
#include <GL/GLwDrawAP.h>
#endif
#include <X11/Xatom.h>
#ifdef HAVE_XMU
#include <X11/Xmu/StdCmap.h>  /* for XmuLookupStandardColormap */
#endif

#ifdef __GLX_MOTIF
#define GLwDrawingAreaWidget      GLwMDrawingAreaWidget
#define GLwDrawingAreaClassRec    GLwMDrawingAreaClassRec
#define GLwDrawingAreaRec         GLwMDrawingAreaRec
#define glwDrawingAreaClassRec    glwMDrawingAreaClassRec
#define glwDrawingAreaWidgetClass glwMDrawingAreaWidgetClass
#endif

/* Actions */

static void glwInput (Widget w, XEvent * event, String * params, Cardinal * numParams);


/* Methods */

static void ClassInitialize (void);
static void Initialize (GLwDrawingAreaWidget req, GLwDrawingAreaWidget new,
			ArgList args, Cardinal * num_args);
static void Realize (Widget w, Mask * valueMask, XSetWindowAttributes * attributes);
static void Redraw (GLwDrawingAreaWidget w, XEvent * event, Region region);
static void Resize (GLwDrawingAreaWidget glw);
static void Destroy (GLwDrawingAreaWidget glw);


/* Translations */

static char defaultTranslations[] =
"<KeyDown>:   glwInput() \n\
   <KeyUp>:     glwInput() \n\
   <BtnDown>:   glwInput() \n\
   <BtnUp>:     glwInput() \n\
   <BtnMotion>: glwInput() ";


static XtActionsRec actions[] =
{
  {"glwInput", (XtActionProc) glwInput},	/* key or mouse input */
};

static XtResource resources[] =
{
#undef offset
#define offset(_field) XtOffset (GLwDrawingAreaWidget, glwDrawingArea._field)
  {GLwNattribList, GLwCAttribList, XtRPointer, sizeof (int *),
   offset (attribList), XtRImmediate, (caddr_t) NULL},
  {GLwNvisualInfo, GLwCVisualInfo, GLwRVisualInfo, sizeof (XVisualInfo *),
   offset (visualInfo), XtRImmediate, (caddr_t) NULL},
  {GLwNinstallColormap, GLwCInstallColormap, XtRBoolean, sizeof (Boolean),
   offset (installColormap), XtRImmediate, (caddr_t) True},
  {GLwNallocateBackground, GLwNallocateOtherColors, XtRBoolean, sizeof (Boolean),
   offset (allocateBackground), XtRImmediate, (caddr_t) False},
  {GLwNallocateOtherColors, GLwCAllocateColors, XtRBoolean, sizeof (Boolean),
   offset (allocateOtherColors), XtRImmediate, (caddr_t) False},
  {GLwNinstallBackground, GLwCInstallBackground, XtRBoolean, sizeof (Boolean),
   offset (installBackground), XtRImmediate, (caddr_t) True},
  {GLwNginitCallback, GLwCCallback, XtRCallback, sizeof (XtCallbackList),
   offset (ginitCallback), XtRImmediate, (XtPointer) NULL},
  {GLwNinputCallback, GLwCCallback, XtRCallback, sizeof (XtCallbackList),
   offset (inputCallback), XtRImmediate, (XtPointer) NULL},
  {GLwNresizeCallback, GLwCCallback, XtRCallback, sizeof (XtCallbackList),
   offset (resizeCallback), XtRImmediate, (XtPointer) NULL},
  {GLwNexposeCallback, GLwCCallback, XtRCallback, sizeof (XtCallbackList),
   offset (exposeCallback), XtRImmediate, (XtPointer) NULL},
  {GLwNbufferSize, GLwCBufferSize, XtRInt, sizeof (int),
   offset (bufferSize), XtRImmediate, (caddr_t) 0},
  {GLwNlevel, GLwCLevel, XtRInt, sizeof (int),
   offset (level), XtRImmediate, (caddr_t) 0},
  {GLwNrgba, GLwCRgba, XtRBoolean, sizeof (Boolean),
   offset (rgba), XtRImmediate, (caddr_t) False},
  {GLwNdoublebuffer, GLwCDoublebuffer, XtRBoolean, sizeof (Boolean),
   offset (doublebuffer), XtRImmediate, (caddr_t) False},
  {GLwNstereo, GLwCStereo, XtRBoolean, sizeof (Boolean),
   offset (stereo), XtRImmediate, (caddr_t) False},
  {GLwNauxBuffers, GLwCAuxBuffers, XtRInt, sizeof (int),
   offset (auxBuffers), XtRImmediate, (caddr_t) 0},
  {GLwNredSize, GLwCColorSize, XtRInt, sizeof (int),
   offset (redSize), XtRImmediate, (caddr_t) 1},
  {GLwNgreenSize, GLwCColorSize, XtRInt, sizeof (int),
   offset (greenSize), XtRImmediate, (caddr_t) 1},
  {GLwNblueSize, GLwCColorSize, XtRInt, sizeof (int),
   offset (blueSize), XtRImmediate, (caddr_t) 1},
  {GLwNalphaSize, GLwCAlphaSize, XtRInt, sizeof (int),
   offset (alphaSize), XtRImmediate, (caddr_t) 0},
  {GLwNdepthSize, GLwCDepthSize, XtRInt, sizeof (int),
   offset (depthSize), XtRImmediate, (caddr_t) 0},
  {GLwNstencilSize, GLwCStencilSize, XtRInt, sizeof (int),
   offset (stencilSize), XtRImmediate, (caddr_t) 0},
  {GLwNaccumRedSize, GLwCAccumColorSize, XtRInt, sizeof (int),
   offset (accumRedSize), XtRImmediate, (caddr_t) 0},
  {GLwNaccumGreenSize, GLwCAccumColorSize, XtRInt, sizeof (int),
   offset (accumGreenSize), XtRImmediate, (caddr_t) 0},
  {GLwNaccumBlueSize, GLwCAccumColorSize, XtRInt, sizeof (int),
   offset (accumBlueSize), XtRImmediate, (caddr_t) 0},
  {GLwNaccumAlphaSize, GLwCAccumAlphaSize, XtRInt, sizeof (int),
   offset (accumAlphaSize), XtRImmediate, (caddr_t) 0},
#ifdef __GLX_MOTIF
  {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof (Boolean),
   XtOffset (GLwDrawingAreaWidget, primitive.traversal_on),
   XmRImmediate, (XtPointer) FALSE},
  {XmNhighlightOnEnter, XmCHighlightOnEnter, XmRBoolean, sizeof (Boolean),
   XtOffset (GLwDrawingAreaWidget, primitive.highlight_on_enter),
   XmRImmediate, (XtPointer) FALSE},
  {XmNhighlightThickness, XmCHighlightThickness,
   XmRHorizontalDimension, sizeof (Dimension),
   XtOffset (GLwDrawingAreaWidget, primitive.highlight_thickness),
   XmRImmediate, (XtPointer) 0},
#endif
  {GLwNdebug, GLwCDebug, XtRBoolean, sizeof (Boolean),
   offset (debug), XtRImmediate, (caddr_t) False},
#undef offset
};


GLwDrawingAreaClassRec glwDrawingAreaClassRec =
{
  {				/* Core fields */
#ifdef __GLX_MOTIF
    /* superclass               */ (WidgetClass) & xmPrimitiveClassRec,
    /* class_name               */ "GLwMDrawingArea",
#else
    /* superclass               */ (WidgetClass) & coreClassRec,
    /* class_name               */ "GLwDrawingArea",
#endif
    /* widget_size              */ sizeof (GLwDrawingAreaRec),
    /* class_initialize         */ ClassInitialize,
    /* class_part_initialize    */ NULL,
    /* class_inited             */ FALSE,
    /* initialize               */ (XtInitProc) Initialize,
    /* initialize_hook          */ NULL,
    /* realize                  */ Realize,
    /* actions                  */ actions,
    /* num_actions              */ XtNumber (actions),
    /* resources                */ resources,
    /* num_resources            */ XtNumber (resources),
    /* xrm_class                */ NULLQUARK,
    /* compress_motion          */ TRUE,
    /* compress_exposure        */ TRUE,
    /* compress_enterleave      */ TRUE,
    /* visible_interest         */ TRUE,
    /* destroy                  */ (XtWidgetProc) Destroy,
    /* resize                   */ (XtWidgetProc) Resize,
    /* expose                   */ (XtExposeProc) Redraw,
    /* set_values               */ NULL,
    /* set_values_hook          */ NULL,
    /* set_values_almost        */ XtInheritSetValuesAlmost,
    /* get_values_hook          */ NULL,
    /* accept_focus             */ NULL,
    /* version                  */ XtVersion,
    /* callback_private         */ NULL,
    /* tm_table                 */ defaultTranslations,
    /* query_geometry           */ XtInheritQueryGeometry,
    /* display_accelerator      */ XtInheritDisplayAccelerator,
    /* extension                */ NULL
  },
#ifdef __GLX_MOTIF
  {				/* XmPrimitive fields */
    /* border_highlight         */ (XtWidgetProc) _XtInherit,
    /* border_unhighlight       */ (XtWidgetProc) _XtInherit,
    /* translations             */ XtInheritTranslations,
    /* arm_and_activate         */ NULL,
    /* get_resources            */ NULL,
    /* num get_resources        */ 0,
    /* extension                */ NULL,
  }
#endif /* __GLX_MOTIF */
};

WidgetClass glwDrawingAreaWidgetClass = (WidgetClass) & glwDrawingAreaClassRec;


static void
error (Widget w, char *string)
{
  char buf[256];
  sprintf (buf, "%s: %s: %s\n",
	   XtClass(w)->core_class.class_name, w->core.name, string);
  XtAppError (XtWidgetToApplicationContext (w), buf);
}

static void
warning (Widget w, char *string)
{
  char buf[256];
  sprintf (buf, "%s: %s: %s\n",
	   XtClass(w)->core_class.class_name, w->core.name, string);
  XtAppWarning (XtWidgetToApplicationContext (w), buf);
}

static Widget
toplevel_widget (Widget w)
{
  Widget p;
  for (p = XtParent (w); !XtIsShell (p); p = XtParent (p))
    if (p == NULL)
      {
	p = w;
	error (w, "can't find shell widget");
      }
  return p;
}

/* Create attribute list for passing to glxChooseVisual()
   from the ressources.  */

#define ATTRIBLIST_SIZE 30
static void
generate_attrib_list (GLwDrawingAreaPart *p)
{
  int i = 0;
  p->attribList = (int *) XtCalloc (ATTRIBLIST_SIZE, sizeof (int));
  p->attribList_allocated = True;

  p->attribList[i++] = GLX_BUFFER_SIZE;
  p->attribList[i++] = p->bufferSize;
  p->attribList[i++] = GLX_LEVEL;
  p->attribList[i++] = p->level;
  if (p->rgba)
    p->attribList[i++] = GLX_RGBA;
  if (p->doublebuffer)
    p->attribList[i++] = GLX_DOUBLEBUFFER;
  if (p->stereo)
    p->attribList[i++] = GLX_STEREO;
  p->attribList[i++] = GLX_AUX_BUFFERS;
  p->attribList[i++] = p->auxBuffers;
  p->attribList[i++] = GLX_RED_SIZE;
  p->attribList[i++] = p->redSize;
  p->attribList[i++] = GLX_GREEN_SIZE;
  p->attribList[i++] = p->greenSize;
  p->attribList[i++] = GLX_BLUE_SIZE;
  p->attribList[i++] = p->blueSize;
  p->attribList[i++] = GLX_ALPHA_SIZE;
  p->attribList[i++] = p->alphaSize;
  p->attribList[i++] = GLX_DEPTH_SIZE;
  if (p->depthSize == 1)
     p->depthSize = 16;
  p->attribList[i++] = p->depthSize;
  p->attribList[i++] = GLX_STENCIL_SIZE;
  p->attribList[i++] = p->stencilSize;
  p->attribList[i++] = GLX_ACCUM_RED_SIZE;
  p->attribList[i++] = p->accumRedSize;
  p->attribList[i++] = GLX_ACCUM_GREEN_SIZE;
  p->attribList[i++] = p->accumGreenSize;
  p->attribList[i++] = GLX_ACCUM_BLUE_SIZE;
  p->attribList[i++] = p->accumBlueSize;
  p->attribList[i++] = GLX_ACCUM_ALPHA_SIZE;
  p->attribList[i++] = p->accumAlphaSize;
  p->attribList[i++] = None;
}

#ifdef XmuLookupStandardColormap_OK
static Colormap
get_standard_rgb_map (Widget w, XVisualInfo *vi)
{
  Colormap cmap = None;

#ifdef HAVE_XMU
  Display *dpy = XtDisplay (w);
  int scr = XScreenNumberOfScreen (XtScreen (w));
  
  if (XmuLookupStandardColormap (dpy, vi->screen, vi->visualid, vi->depth,
				 XA_RGB_DEFAULT_MAP, False, True))
    {
      XStandardColormap *std_cmaps;
      int i, num_cmaps;
      if (XGetRGBColormaps (dpy, RootWindow (dpy, scr),
			    &std_cmaps, &num_cmaps, XA_RGB_DEFAULT_MAP))
	{
	  for (i = 0; i < num_cmaps; i++)
	    if (std_cmaps[i].visualid == vi->visualid)
	      {
		cmap = std_cmaps[i].colormap;
		break;
	      }
	  XFree (std_cmaps);
	}
    }
#endif /* HAVE_XMU */

  return cmap;
}
#endif /* XmuLookupStandardColormap_OK */

#define glw_cmaps glwDrawingAreaClassRec.glwDrawingArea_class.colormaps

static Colormap
lookup_colormap (Widget w, XVisualInfo *vi, int alloc)
{
  Display *dpy;
  int scr;
  Colormap cmap;
  int i;

  LOG (w);

  dpy = XtDisplay (w);
  scr = XScreenNumberOfScreen (XtScreen (w));
  cmap = None;

#ifdef DEBUG
  if (GLwDebug(w))
    fprintf (stderr,
	     "looking up colormap for visual id #%ld on display #%p ... ",
	     vi->visualid, dpy);
#endif

  assert (glw_cmaps.entries);
  
  for (i = 0; i < glw_cmaps.next_entry; i++)
    if ((glw_cmaps.entries[i].dpy == dpy)
	&& (glw_cmaps.entries[i].vid == vi->visualid))
      {
#ifdef DEBUG
	if (GLwDebug(w))
	  fprintf (stderr, "found #%ld\n", glw_cmaps.entries[i].cmap);
#endif
	return glw_cmaps.entries[i].cmap;
      }

#ifdef XmuLookupStandardColormap_OK
  /* Commented out as of version 1.2.8 since Sun-SGI display causes a
     weird problem.  Probably a bug in Sun's XmuLookupStandardColormap().  */

  if ((vi->class == TrueColor)
      || (vi->class == DirectColor))
    cmap = get_standard_rgb_map (w, vi);

  /* I had put this code (which is inspired by glut) in for a reason because
     I had experienced problems with 8 bit TrueColor visuals.  Now I can't
     reproduce these problem anymore.  I'll leave it in as inactive code, in
     case somebody can reproduce the old problems.  */
#endif

  /* Code to look for HP Color Recovery Atom and colormap contributed by
     Jean-Luc Daems (jld@star.be) on Feb 23, 1996.   */
  if (cmap == None) {
     Atom hp_cr_maps = XInternAtom(dpy,"_HP_RGB_SMOOTH_MAP_LIST",True) ;
     if (hp_cr_maps) {
        XStandardColormap* colmaps = 0;
        int nrColmaps = 0;
        int i;
        XGetRGBColormaps( dpy, RootWindow(dpy, scr),
                          &colmaps, &nrColmaps, hp_cr_maps );
        for (i=0; i<nrColmaps; i++) {
           if (colmaps[i].visualid == vi->visual->visualid) {
              cmap = colmaps[i].colormap;
              break;
           }
        }
     }
  }
  if (cmap == None)
    cmap = XCreateColormap (dpy, RootWindow (dpy, scr), vi->visual, AllocNone);

  if (!cmap)
      error (w, "can't get a colormap");

  if (glw_cmaps.next_entry >= glw_cmaps.allocated_entries)
    {
      glw_cmaps.allocated_entries += 10;
      glw_cmaps.entries = (struct cmap_cache_entry *)
	XtRealloc ((char *) glw_cmaps.entries,
		   glw_cmaps.allocated_entries
		   * sizeof (struct cmap_cache_entry));
    }
  
  i = glw_cmaps.next_entry++;
  glw_cmaps.entries[i].dpy = dpy;
  glw_cmaps.entries[i].vid = vi->visualid;
  glw_cmaps.entries[i].cmap = cmap;

#ifdef DEBUG
  if (GLwDebug(w))
    fprintf (stderr, "allocated new #%ld\n", cmap);
#endif
  return cmap;
}

/* Inform the window manager that the widget W's window needs a
   special colormap by setting the WM_COLORMAP_WINDOWS property
   on the top level shell.

   FIXME: we should use XGetWMColormapWindows () to check for
          other windows and add our window to this list instead
	  of overwriting. */

static void
post_colormap (Widget w)
{
  Widget top = toplevel_widget (w);
  Window wlist[2];
  wlist[0] = XtWindow(w);
  wlist[1] = XtWindow(top);
  if (!XSetWMColormapWindows (XtDisplay(top), XtWindow(top), wlist, 2))
    warning (w, "can't post colormap");
}



/* Widget methods.  */

/* Initialize this widget class.  Currently we just allocate the first
   chunk of memory for the colormap cache.  */

static void
ClassInitialize (void)
{
  /* two's a crowd ...  (for most applications)  */
  glw_cmaps.allocated_entries = 2;
  glw_cmaps.next_entry = 0;
  glw_cmaps.entries = (struct cmap_cache_entry *)
    XtMalloc (glw_cmaps.allocated_entries * sizeof (struct cmap_cache_entry));
}

/* Initialize a widget instance.
   Here's where we do the colormap dance. */

static void 
Initialize (GLwDrawingAreaWidget req,
	    GLwDrawingAreaWidget new,
	    ArgList args, Cardinal *num_args)
{
  Display *dpy;
  int scr;
  XVisualInfo *vi;

  LOG (new);
  
  dpy = XtDisplay (new);
  scr = XScreenNumberOfScreen (XtScreen (new));

  if (req->core.width == 0)
    new->core.width = 100;
  if (req->core.height == 0)
    new->core.width = 100;

  new->glwDrawingArea.attribList_allocated = False;
  new->glwDrawingArea.visualInfo_allocated = False;

  vi = new->glwDrawingArea.visualInfo;

  /* If there's no visual in the resources, we have to select our own,
     based on the attribute list or the other resources.  */

  if (vi == NULL)
    {
      /* If the list of attribues is not defined in the resources, we
	 have to build one from the other resources.  */
      if (new->glwDrawingArea.attribList == NULL)
	generate_attrib_list (&new->glwDrawingArea);

      /* Select the visual and remember it.  */
      vi = glXChooseVisual (dpy, scr, new->glwDrawingArea.attribList);
      if (!vi)
	error ((Widget) new, "can't get a visual");
      new->glwDrawingArea.visualInfo = vi;
      new->glwDrawingArea.visualInfo_allocated = True;
    }

  new->core.depth = vi->depth;

  /* If we're rendering in the default visual in color-index mode,
     we continue to use the default map.  This is the most economical
     approach.  In RGBA mode we try to use a standard RGB map for
     TrueColor and DirectColor visuals.  If this is not available
     and for all other visuals, we allocate a fresh colormap. */

  if (!new->glwDrawingArea.rgba
      && (vi->visualid == XVisualIDFromVisual (DefaultVisual (dpy, scr))))
    new->core.colormap = DefaultColormap (dpy, scr);
  else
    new->core.colormap = lookup_colormap ((Widget) new, vi, AllocNone);
}

/* Realize a widget.
   That's trivial, we just create the window with the appropriate
   visual and call the callback functions.  */

static void 
Realize (Widget w, Mask *mask, XSetWindowAttributes *attr)
{
  GLwDrawingAreaWidget glw;
  Display *dpy;
  int scr;
  GLwDrawingAreaCallbackStruct cb;

  LOG (w);

  glw = (GLwDrawingAreaWidget) w;
  dpy = XtDisplay (w);
  scr = XScreenNumberOfScreen (XtScreen (w));

  /* Create the window.  */

  attr->colormap = w->core.colormap;
  attr->event_mask
    = KeyPressMask
    | KeyReleaseMask
    | ButtonPressMask
    | ButtonReleaseMask
    | EnterWindowMask
    | LeaveWindowMask
    | PointerMotionMask
    | ButtonMotionMask
    | ExposureMask
    | StructureNotifyMask;
  attr->border_pixel = BlackPixel (dpy, scr);
  attr->background_pixel = BlackPixel (dpy, scr);
  attr->backing_store = NotUseful;
  *mask = CWColormap
        | CWEventMask
	| CWBorderPixel
	| CWBackPixel
	| CWBackingStore;
  XtCreateWindow (w, (unsigned int) InputOutput,
		  glw->glwDrawingArea.visualInfo->visual,
		  *mask, attr);

  /* Install the colormap if requested.  */
  if (glw->glwDrawingArea.installColormap)
    post_colormap (w);

  /* Invoke callback to initialize.  */
  cb.reason = GLwCR_GINIT;
  cb.event = NULL;
  cb.width = glw->core.width;
  cb.height = glw->core.height;
  XtCallCallbackList ((Widget) glw, glw->glwDrawingArea.ginitCallback, &cb);
}

/* Invoke expose callbacks to redraw screen */

static void 
Redraw (GLwDrawingAreaWidget w, XEvent * event, Region region)
{
  GLwDrawingAreaCallbackStruct cb;
  LOG (w);
  /* Ignore while not yet realized.  */
  if (!XtIsRealized ((Widget) w))
    return;
  cb.reason = GLwCR_EXPOSE;
  cb.event = event;
  cb.width = w->core.width;
  cb.height = w->core.height;
  XtCallCallbackList ((Widget) w, w->glwDrawingArea.exposeCallback, &cb);
}

/* Invoke resize callbacks */

static void 
Resize (GLwDrawingAreaWidget glw)
{
  GLwDrawingAreaCallbackStruct cb;
  LOG (glw);
  /* Ignore while not yet realized.  */
  if (!XtIsRealized ((Widget) glw))
    return;
  cb.reason = GLwCR_RESIZE;
  cb.event = NULL;
  cb.width = glw->core.width;
  cb.height = glw->core.height;
  XtCallCallbackList ((Widget) glw, glw->glwDrawingArea.resizeCallback, &cb);
}

/* Window destroy handling.

   FIXME: shouldn't we remove the WM_COLORMAP_WINDOWS property
          from the toplevel shell?  */

static void 
Destroy (GLwDrawingAreaWidget glw)
{
  LOG (glw);
  XtRemoveAllCallbacks ((Widget) glw, GLwNinputCallback);
  XtRemoveAllCallbacks ((Widget) glw, GLwNresizeCallback);
  XtRemoveAllCallbacks ((Widget) glw, GLwNexposeCallback);
  if (glw->glwDrawingArea.attribList_allocated)
    {
      XtFree ((char *)glw->glwDrawingArea.attribList);
      glw->glwDrawingArea.attribList_allocated = False;
    }
  if (glw->glwDrawingArea.visualInfo_allocated)
    {
      XtFree ((char *)glw->glwDrawingArea.visualInfo);
      glw->glwDrawingArea.visualInfo_allocated = False;
    }
}

/* Action routine for keyboard and mouse events */

static void 
glwInput (Widget w, XEvent * event, String * params, Cardinal * numParams)
{
  GLwDrawingAreaCallbackStruct cb;
  GLwDrawingAreaWidget glw = (GLwDrawingAreaWidget) w;
  LOG (w);
  cb.reason = GLwCR_INPUT;
  cb.event = event;
  cb.width = glw->core.width;
  cb.height = glw->core.height;
  XtCallCallbackList ((Widget) glw, glw->glwDrawingArea.inputCallback, &cb);
}

/* The End.  */
