
/**************************************************************************

Copyright 1998-1999 Precision Insight, Inc., Cedar Park, Texas.
All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sub license, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice (including the
next paragraph) shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************************************************************/

/*
 * Authors:
 *   Kevin E. Martin <kevin@precisioninsight.com>
 *
 * $Header: /cvsroot/mesa3d/Mesa/include/GL/xmesa_x.h,v 1.2 2000/02/25 20:31:11 brianp Exp $
 */

#ifndef _XMESA_X_H_
#define _XMESA_X_H_

typedef Display      XMesaDisplay;
typedef Pixmap       XMesaPixmap;
typedef Colormap     XMesaColormap;
typedef Drawable     XMesaDrawable;
typedef Window       XMesaWindow;
typedef GC           XMesaGC;
typedef XVisualInfo *XMesaVisualInfo;
typedef XImage       XMesaImage;
typedef XPoint       XMesaPoint;
typedef XColor       XMesaColor;

#define XMesaDestroyImage      XDestroyImage

#define XMesaPutPixel          XPutPixel
#define XMesaGetPixel          XGetPixel

#define XMesaSetForeground     XSetForeground
#define XMesaSetBackground     XSetBackground
#define XMesaSetPlaneMask      XSetPlaneMask
#define XMesaSetFunction       XSetFunction
#define XMesaSetDashes         XSetDashes
#define XMesaSetLineAttributes XSetLineAttributes
#define XMesaSetFillStyle      XSetFillStyle
#define XMesaSetTile           XSetTile
#define XMesaSetStipple        XSetStipple

#define XMesaDrawPoint         XDrawPoint
#define XMesaDrawPoints        XDrawPoints
#define XMesaDrawLine          XDrawLine
#define XMesaFillRectangle     XFillRectangle
#define XMesaPutImage          XPutImage
#define XMesaCopyArea          XCopyArea
#define XMesaFillPolygon       XFillPolygon

#define XMesaCreatePixmap      XCreatePixmap
#define XMesaFreePixmap        XFreePixmap
#define XMesaFreeGC            XFreeGC

#define GET_COLORMAP_SIZE(__v)  __v->visinfo->colormap_size
#define GET_REDMASK(__v)        __v->visinfo->red_mask
#define GET_GREENMASK(__v)      __v->visinfo->green_mask
#define GET_BLUEMASK(__v)       __v->visinfo->blue_mask
#if defined(__cplusplus) || defined(c_plusplus)
#define GET_VISUAL_CLASS(__v)   __v->visinfo->c_class
#else
#define GET_VISUAL_CLASS(__v)   __v->visinfo->class
#endif
#define GET_VISUAL_DEPTH(__v)   __v->visinfo->depth
#define GET_BLACK_PIXEL(__v)    BlackPixel(__v->display, __v->visinfo->screen)
#define CHECK_BYTE_ORDER(__v)   host_byte_order()==ImageByteOrder(__v->display)
#define CHECK_FOR_HPCR(__v)     XInternAtom(__v->display, "_HP_RGB_SMOOTH_MAP_LIST", True)

#endif
