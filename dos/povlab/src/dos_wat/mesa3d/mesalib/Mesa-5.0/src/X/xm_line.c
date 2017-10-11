/* $Id: xm_line.c,v 1.20 2002/06/15 03:03:10 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.5
 *
 * Copyright (C) 1999-2000  Brian Paul   All Rights Reserved.
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
 * This file contains "accelerated" point, line, and triangle functions.
 * It should be fairly easy to write new special-purpose point, line or
 * triangle functions and hook them into this module.
 */


#include "glxheader.h"
#include "depth.h"
#include "macros.h"
#include "mmath.h"
#include "mtypes.h"
#include "xmesaP.h"

/* Internal swrast includes:
 */
#include "swrast/s_depth.h"
#include "swrast/s_points.h"
#include "swrast/s_lines.h"
#include "swrast/s_context.h"


/**********************************************************************/
/***                    Point rendering                             ***/
/**********************************************************************/


/*
 * Render an array of points into a pixmap, any pixel format.
 */
#if 000
/* XXX don't use this, it doesn't dither correctly */
static void draw_points_ANY_pixmap( GLcontext *ctx, const SWvertex *vert )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaDisplay *dpy = xmesa->xm_visual->display;
   XMesaDrawable buffer = xmesa->xm_buffer->buffer;
   XMesaGC gc = xmesa->xm_buffer->gc;

   if (xmesa->xm_visual->mesa_visual.RGBAflag) {
      register int x, y;
      const GLubyte *color = vert->color;
      unsigned long pixel = xmesa_color_to_pixel( xmesa,
						  color[0], color[1],
						  color[2], color[3],
						  xmesa->pixelformat);
      XMesaSetForeground( dpy, gc, pixel );
      x =                         (GLint) vert->win[0];
      y = FLIP( xmesa->xm_buffer, (GLint) vert->win[1] );
      XMesaDrawPoint( dpy, buffer, gc, x, y);
   }
   else {
      /* Color index mode */
      register int x, y;
      XMesaSetForeground( dpy, gc, vert->index );
      x =                         (GLint) vert->win[0];
      y = FLIP( xmesa->xm_buffer, (GLint) vert->win[1] );
      XMesaDrawPoint( dpy, buffer, gc, x, y);
   }
}
#endif


/* Override the swrast point-selection function.  Try to use one of
 * our internal point functions, otherwise fall back to the standard
 * swrast functions.
 */
void xmesa_choose_point( GLcontext *ctx )
{
#if 0
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   SWcontext *swrast = SWRAST_CONTEXT(ctx);

   if (ctx->RenderMode == GL_RENDER
       && ctx->Point.Size == 1.0F && !ctx->Point.SmoothFlag
       && swrast->_RasterMask == 0
       && !ctx->Texture._EnabledUnits
       && xmesa->xm_buffer->buffer != XIMAGE) {
      swrast->Point = draw_points_ANY_pixmap;
   }
   else {
      _swrast_choose_point( ctx );
   }
#else
   _swrast_choose_point( ctx );
#endif
}



/**********************************************************************/
/***                      Line rendering                            ***/
/**********************************************************************/


/*
 * Draw a flat-shaded, PF_TRUECOLOR line into an XImage.
 */
static void flat_TRUECOLOR_line( GLcontext *ctx,
                                 const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   XMesaImage *img = xmesa->xm_buffer->backimage;
   unsigned long pixel;
   PACK_TRUECOLOR( pixel, color[0], color[1], color[2] );

#define INTERP_XY 1
#define CLIP_HACK 1
#define PLOT(X,Y) XMesaPutPixel( img, X, FLIP(xmesa->xm_buffer, Y), pixel );

#include "swrast/s_linetemp.h"
}



/*
 * Draw a flat-shaded, PF_8A8B8G8R line into an XImage.
 */
static void flat_8A8B8G8R_line( GLcontext *ctx,
                                const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLuint pixel = PACK_8B8G8R( color[0], color[1], color[2] );

#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, PF_8R8G8B line into an XImage.
 */
static void flat_8R8G8B_line( GLcontext *ctx,
                              const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLuint pixel = PACK_8R8G8B( color[0], color[1], color[2] );

#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, PF_8R8G8B24 line into an XImage.
 */
static void flat_8R8G8B24_line( GLcontext *ctx,
                              const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;

#define PIXEL_TYPE bgr_t
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR3(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) {			\
      pixelPtr->r = color[RCOMP];	\
      pixelPtr->g = color[GCOMP];	\
      pixelPtr->b = color[BCOMP];	\
}

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, PF_5R6G5B line into an XImage.
 */
static void flat_5R6G5B_line( GLcontext *ctx,
                              const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLushort pixel = PACK_5R6G5B( color[0], color[1], color[2] );

#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, PF_DITHER_5R6G5B line into an XImage.
 */
static void flat_DITHER_5R6G5B_line( GLcontext *ctx,
                                     const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;

#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) PACK_TRUEDITHER( *pixelPtr, X, Y, color[0], color[1], color[2] );

#include "swrast/s_linetemp.h"
}



/*
 * Draw a flat-shaded, PF_DITHER 8-bit line into an XImage.
 */
static void flat_DITHER8_line( GLcontext *ctx,
                               const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLint r = color[0], g = color[1], b = color[2];
   DITHER_SETUP;

#define INTERP_XY 1
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = DITHER(X,Y,r,g,b);

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, PF_LOOKUP 8-bit line into an XImage.
 */
static void flat_LOOKUP8_line( GLcontext *ctx,
                               const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLubyte pixel;
   LOOKUP_SETUP;
   pixel = (GLubyte) LOOKUP( color[0], color[1], color[2] );

#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, PF_HPCR line into an XImage.
 */
static void flat_HPCR_line( GLcontext *ctx,
                            const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLint r = color[0], g = color[1], b = color[2];

#define INTERP_XY 1
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = (GLubyte) DITHER_HPCR(X,Y,r,g,b);

#include "swrast/s_linetemp.h"
}



/*
 * Draw a flat-shaded, Z-less, PF_TRUECOLOR line into an XImage.
 */
static void flat_TRUECOLOR_z_line( GLcontext *ctx,
                                   const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   XMesaImage *img = xmesa->xm_buffer->backimage;
   unsigned long pixel;
   PACK_TRUECOLOR( pixel, color[0], color[1], color[2] );

#define INTERP_XY 1
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define CLIP_HACK 1
#define PLOT(X,Y)							\
	if (Z < *zPtr) {						\
	   *zPtr = Z;							\
           XMesaPutPixel( img, X, FLIP(xmesa->xm_buffer, Y), pixel );	\
	}

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_8A8B8G8R line into an XImage.
 */
static void flat_8A8B8G8R_z_line( GLcontext *ctx,
                                  const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLuint pixel = PACK_8B8G8R( color[0], color[1], color[2] );

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		\
	if (Z < *zPtr) {	\
	   *zPtr = Z;		\
	   *pixelPtr = pixel;	\
	}

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_8R8G8B line into an XImage.
 */
static void flat_8R8G8B_z_line( GLcontext *ctx,
                                const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLuint pixel = PACK_8R8G8B( color[0], color[1], color[2] );

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		\
	if (Z < *zPtr) {	\
	   *zPtr = Z;		\
	   *pixelPtr = pixel;	\
	}

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_8R8G8B24 line into an XImage.
 */
static void flat_8R8G8B24_z_line( GLcontext *ctx,
                                    const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE bgr_t
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR3(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)			\
	if (Z < *zPtr) {		\
	   *zPtr = Z;			\
           pixelPtr->r = color[RCOMP];	\
           pixelPtr->g = color[GCOMP];	\
           pixelPtr->b = color[BCOMP];	\
	}

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_5R6G5B line into an XImage.
 */
static void flat_5R6G5B_z_line( GLcontext *ctx,
                                const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLushort pixel = PACK_5R6G5B( color[0], color[1], color[2] );

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		\
	if (Z < *zPtr) {	\
	   *zPtr = Z;		\
	   *pixelPtr = pixel;	\
	}
#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_DITHER_5R6G5B line into an XImage.
 */
static void flat_DITHER_5R6G5B_z_line( GLcontext *ctx,
                                       const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		\
	if (Z < *zPtr) {	\
	   *zPtr = Z;		\
	   PACK_TRUEDITHER(*pixelPtr, X, Y, color[0], color[1], color[2]); \
	}
#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_DITHER 8-bit line into an XImage.
 */
static void flat_DITHER8_z_line( GLcontext *ctx,
                                 const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLint r = color[0], g = color[1], b = color[2];
   DITHER_SETUP;

#define INTERP_XY 1
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)						\
	if (Z < *zPtr) {					\
	   *zPtr = Z;						\
	   *pixelPtr = (GLubyte) DITHER( X, Y, r, g, b);	\
	}
#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_LOOKUP 8-bit line into an XImage.
 */
static void flat_LOOKUP8_z_line( GLcontext *ctx,
                                 const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLubyte pixel;
   LOOKUP_SETUP;
   pixel = (GLubyte) LOOKUP( color[0], color[1], color[2] );

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		\
	if (Z < *zPtr) {	\
	   *zPtr = Z;		\
	   *pixelPtr = pixel;	\
	}

#include "swrast/s_linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_HPCR line into an XImage.
 */
static void flat_HPCR_z_line( GLcontext *ctx,
                              const SWvertex *vert0, const SWvertex *vert1 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = vert1->color;
   GLint r = color[0], g = color[1], b = color[2];

#define INTERP_XY 1
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)						\
	if (Z < *zPtr) {					\
	   *zPtr = Z;						\
	   *pixelPtr = (GLubyte) DITHER_HPCR( X, Y, r, g, b);	\
	}

#include "swrast/s_linetemp.h"
}


static swrast_line_func get_line_func( GLcontext *ctx )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   SWcontext *swrast = SWRAST_CONTEXT(ctx);
   int depth = GET_VISUAL_DEPTH(xmesa->xm_visual);

   (void) DitherValues;  /* silence unused var warning */
   (void) kernel1;  /* silence unused var warning */

   if (ctx->RenderMode != GL_RENDER)      return (swrast_line_func) NULL;
   if (ctx->Line.SmoothFlag)              return (swrast_line_func) NULL;
   if (ctx->Texture._EnabledUnits)        return (swrast_line_func) NULL;
   if (ctx->Light.ShadeModel != GL_FLAT)  return (swrast_line_func) NULL;
   if (ctx->Line.StippleFlag)             return (swrast_line_func) NULL;
   if (swrast->_RasterMask & MULTI_DRAW_BIT) return (swrast_line_func) NULL;

   if (xmesa->xm_buffer->buffer==XIMAGE
       && swrast->_RasterMask==DEPTH_BIT
       && ctx->Depth.Func==GL_LESS
       && ctx->Depth.Mask==GL_TRUE
       && ctx->Visual.depthBits == DEFAULT_SOFTWARE_DEPTH_BITS
       && ctx->Line.Width==1.0F) {
      switch (xmesa->pixelformat) {
         case PF_TRUECOLOR:
            return flat_TRUECOLOR_z_line;
         case PF_8A8B8G8R:
            return flat_8A8B8G8R_z_line;
         case PF_8R8G8B:
            return flat_8R8G8B_z_line;
         case PF_8R8G8B24:
            return flat_8R8G8B24_z_line;
         case PF_5R6G5B:
            return flat_5R6G5B_z_line;
         case PF_DITHER_5R6G5B:
            return flat_DITHER_5R6G5B_z_line;
         case PF_DITHER:
            return (depth==8) ? flat_DITHER8_z_line : (swrast_line_func) NULL;
         case PF_LOOKUP:
            return (depth==8) ? flat_LOOKUP8_z_line : (swrast_line_func) NULL;
         case PF_HPCR:
            return flat_HPCR_z_line;
         default:
            return (swrast_line_func)NULL;
      }
   }
   if (xmesa->xm_buffer->buffer==XIMAGE
       && swrast->_RasterMask==0
       && ctx->Line.Width==1.0F) {
      switch (xmesa->pixelformat) {
         case PF_TRUECOLOR:
            return flat_TRUECOLOR_line;
         case PF_8A8B8G8R:
            return flat_8A8B8G8R_line;
         case PF_8R8G8B:
            return flat_8R8G8B_line;
         case PF_8R8G8B24:
            return flat_8R8G8B24_line;
         case PF_5R6G5B:
            return flat_5R6G5B_line;
         case PF_DITHER_5R6G5B:
            return flat_DITHER_5R6G5B_line;
         case PF_DITHER:
            return (depth==8) ? flat_DITHER8_line : (swrast_line_func) NULL;
         case PF_LOOKUP:
            return (depth==8) ? flat_LOOKUP8_line : (swrast_line_func) NULL;
         case PF_HPCR:
            return flat_HPCR_line;
	 default:
	    return (swrast_line_func)NULL;
      }
   }

   return (swrast_line_func) NULL;
}

/* Override for the swrast line-selection function.  Try to use one
 * of our internal line functions, otherwise fall back to the
 * standard swrast functions.
 */
void xmesa_choose_line( GLcontext *ctx )
{
   SWcontext *swrast = SWRAST_CONTEXT(ctx);

   if (!(swrast->Line = get_line_func( ctx )))
      _swrast_choose_line( ctx );
}
