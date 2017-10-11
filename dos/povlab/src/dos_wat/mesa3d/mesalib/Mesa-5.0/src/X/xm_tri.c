/* $Id: xm_tri.c,v 1.29 2002/10/30 20:24:47 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  4.1
 *
 * Copyright (C) 1999-2002  Brian Paul   All Rights Reserved.
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
 * This file contains "accelerated" triangle functions.  It should be
 * fairly easy to write new special-purpose triangle functions and hook
 * them into this module.
 */


#include "glxheader.h"
#include "depth.h"
#include "macros.h"
#include "imports.h"
#include "mmath.h"
#include "mtypes.h"
#include "xmesaP.h"

/* Internal swrast includes:
 */
#include "swrast/s_context.h"
#include "swrast/s_depth.h"
#include "swrast/s_triangle.h"



/**********************************************************************/
/***                   Triangle rendering                           ***/
/**********************************************************************/


/*
 * XImage, smooth, depth-buffered, PF_TRUECOLOR triangle.
 */
static void smooth_TRUECOLOR_z_triangle( GLcontext *ctx,
                                         const SWvertex *v0,
					 const SWvertex *v1,
					 const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1

#define RENDER_SPAN( span )					\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   GLuint i;							\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         unsigned long p;					\
         PACK_TRUECOLOR(p, FixedToInt(span.red),		\
            FixedToInt(span.green), FixedToInt(span.blue));	\
         XMesaPutPixel(img, x, y, p);				\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}



/*
 * XImage, smooth, depth-buffered, PF_8A8B8G8R triangle.
 */
static void smooth_8A8B8G8R_z_triangle( GLcontext *ctx,
					const SWvertex *v0,
					const SWvertex *v1,
					const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)

#define RENDER_SPAN( span )					\
   GLuint i;							\
   for (i = 0; i < span.end; i++) {				\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         pRow[i] = PACK_8B8G8R(FixedToInt(span.red),		\
            FixedToInt(span.green), FixedToInt(span.blue));	\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, PF_8R8G8B triangle.
 */
static void smooth_8R8G8B_z_triangle( GLcontext *ctx,
				      const SWvertex *v0,
				      const SWvertex *v1,
				      const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)

#define RENDER_SPAN( span )					\
   GLuint i;							\
   for (i = 0; i < span.end; i++) {				\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         pRow[i] = PACK_8R8G8B(FixedToInt(span.red),		\
            FixedToInt(span.green), FixedToInt(span.blue));	\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, PF_8R8G8B24 triangle.
 */
static void smooth_8R8G8B24_z_triangle( GLcontext *ctx,
                                        const SWvertex *v0,
					const SWvertex *v1,
					const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR3(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE bgr_t
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)

#define RENDER_SPAN( span ) 					\
   GLuint i;							\
   for (i = 0; i < span.end; i++) {				\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
	 PIXEL_TYPE *ptr = pRow + i;				\
         ptr->r = FixedToInt(span.red);				\
         ptr->g = FixedToInt(span.green);			\
         ptr->b = FixedToInt(span.blue);			\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, PF_TRUEDITHER triangle.
 */
static void smooth_TRUEDITHER_z_triangle( GLcontext *ctx,
					  const SWvertex *v0,
					  const SWvertex *v1,
					  const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         unsigned long p;					\
         PACK_TRUEDITHER(p, x, y, FixedToInt(span.red),		\
            FixedToInt(span.green), FixedToInt(span.blue));	\
         XMesaPutPixel(img, x, y, p);				\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, PF_5R6G5B triangle.
 */
static void smooth_5R6G5B_z_triangle( GLcontext *ctx,
                                      const SWvertex *v0,
				      const SWvertex *v1,
				      const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)

#define RENDER_SPAN( span )					\
   GLuint i;							\
   for (i = 0; i < span.end; i++) {				\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         pRow[i] = PACK_5R6G5B(FixedToInt(span.red),		\
            FixedToInt(span.green), FixedToInt(span.blue));	\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, PF_DITHER_5R6G5B triangle.
 */
static void smooth_DITHER_5R6G5B_z_triangle( GLcontext *ctx,
                                             const SWvertex *v0,
					     const SWvertex *v1,
					     const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         PACK_TRUEDITHER(pRow[i], x, y, FixedToInt(span.red),	\
            FixedToInt(span.green), FixedToInt(span.blue));	\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, 8-bit, PF_DITHER8 triangle.
 */
static void smooth_DITHER8_z_triangle( GLcontext *ctx,
                                       const SWvertex *v0,
				       const SWvertex *v1,
				       const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   XDITHER_SETUP(y);						\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         pRow[i] = (PIXEL_TYPE) XDITHER(x, FixedToInt(span.red),\
            FixedToInt(span.green), FixedToInt(span.blue) );	\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, PF_DITHER triangle.
 */
static void smooth_DITHER_z_triangle( GLcontext *ctx,
				      const SWvertex *v0,
				      const SWvertex *v1,
				      const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   XDITHER_SETUP(y);						\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         unsigned long p = XDITHER(x, FixedToInt(span.red),	\
            FixedToInt(span.green), FixedToInt(span.blue));	\
	 XMesaPutPixel(img, x, y, p);			       	\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, 8-bit PF_LOOKUP triangle.
 */
static void smooth_LOOKUP8_z_triangle( GLcontext *ctx,
				       const SWvertex *v0,
				       const SWvertex *v1,
				       const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)

#define RENDER_SPAN( span )					\
   GLuint i;							\
   LOOKUP_SETUP;						\
   for (i = 0; i < span.end; i++) {				\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         pRow[i] = LOOKUP(FixedToInt(span.red),			\
            FixedToInt(span.green), FixedToInt(span.blue));	\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}



/*
 * XImage, smooth, depth-buffered, 8-bit PF_HPCR triangle.
 */
static void smooth_HPCR_z_triangle( GLcontext *ctx,
				    const SWvertex *v0,
				    const SWvertex *v1,
				    const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         pRow[i] = DITHER_HPCR(x, y, FixedToInt(span.red),	\
            FixedToInt(span.green), FixedToInt(span.blue) );	\
         zRow[i] = z;						\
      }								\
      span.red += span.redStep;					\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_TRUECOLOR triangle.
 */
static void flat_TRUECOLOR_z_triangle( GLcontext *ctx,
                        	       const SWvertex *v0,
				       const SWvertex *v1,
				       const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define SETUP_CODE						\
   unsigned long pixel;						\
   PACK_TRUECOLOR(pixel, v2->color[0], v2->color[1], v2->color[2]);

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         XMesaPutPixel(img, x, y, pixel);			\
         zRow[i] = z;						\
      }								\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_8A8B8G8R triangle.
 */
static void flat_8A8B8G8R_z_triangle( GLcontext *ctx,
				      const SWvertex *v0,
				      const SWvertex *v1,
				      const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE					\
   unsigned long p = PACK_8B8G8R( v2->color[0],	\
		 v2->color[1], v2->color[2] );
#define RENDER_SPAN( span )				\
   GLuint i;						\
   for (i = 0; i < span.end; i++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);	\
      if (z < zRow[i]) {				\
	 pRow[i] = (PIXEL_TYPE) p;			\
         zRow[i] = z;					\
      }							\
      span.z += span.zStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_8R8G8B triangle.
 */
static void flat_8R8G8B_z_triangle( GLcontext *ctx,
				    const SWvertex *v0,
				    const SWvertex *v1,
				    const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE				\
   unsigned long p = PACK_8R8G8B( v2->color[0],	\
		 v2->color[1], v2->color[2] );
#define RENDER_SPAN( span )			\
   GLuint i;					\
   for (i = 0; i < span.end; i++) {		\
      DEPTH_TYPE z = FixedToDepth(span.z);	\
      if (z < zRow[i]) {			\
	 pRow[i] = (PIXEL_TYPE) p;		\
         zRow[i] = z;				\
      }						\
      span.z += span.zStep;			\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_8R8G8B24 triangle.
 */
static void flat_8R8G8B24_z_triangle( GLcontext *ctx,
				      const SWvertex *v0,
				      const SWvertex *v1,
				      const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = v2->color;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR3(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE bgr_t
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )				\
   GLuint i;						\
   for (i = 0; i < span.end; i++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);	\
      if (z < zRow[i]) {				\
	 PIXEL_TYPE *ptr = pRow + i;			\
         ptr->r = color[RCOMP];				\
         ptr->g = color[GCOMP];				\
         ptr->b = color[BCOMP];				\
         zRow[i] = z;					\
      }							\
      span.z += span.zStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_TRUEDITHER triangle.
 */
static void flat_TRUEDITHER_z_triangle( GLcontext *ctx,
					const SWvertex *v0,
					const SWvertex *v1,
					const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         unsigned long p;					\
         PACK_TRUEDITHER(p, x, y, v2->color[0],			\
            v2->color[1], v2->color[2]);			\
         XMesaPutPixel(img, x, y, p);				\
         zRow[i] = z;						\
      }								\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_5R6G5B triangle.
 */
static void flat_5R6G5B_z_triangle( GLcontext *ctx,
				    const SWvertex *v0,
				    const SWvertex *v1,
				    const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE					\
   unsigned long p = PACK_5R6G5B( v2->color[0],		\
            v2->color[1], v2->color[2] );
#define RENDER_SPAN( span )				\
   GLuint i;						\
   for (i = 0; i < span.end; i++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);	\
      if (z < zRow[i]) {				\
	 pRow[i] = (PIXEL_TYPE) p;			\
         zRow[i] = z;					\
      }							\
      span.z += span.zStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_DITHER_5R6G5B triangle.
 */
static void flat_DITHER_5R6G5B_z_triangle( GLcontext *ctx,
					   const SWvertex *v0,
					   const SWvertex *v1,
					   const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = v2->color;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
	 PACK_TRUEDITHER(pRow[i], x, y, color[RCOMP],		\
			 color[GCOMP], color[BCOMP]);		\
         zRow[i] = z;						\
      }								\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, 8-bit PF_DITHER triangle.
 */
static void flat_DITHER8_z_triangle( GLcontext *ctx,
				     const SWvertex *v0,
				     const SWvertex *v1,
				     const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE	\
   FLAT_DITHER_SETUP( v2->color[0], v2->color[1], v2->color[2] );

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   FLAT_DITHER_ROW_SETUP(FLIP(xmesa->xm_buffer, y));		\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
	 pRow[i] = (PIXEL_TYPE) FLAT_DITHER(x);			\
         zRow[i] = z;						\
      }								\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_DITHER triangle.
 */
static void flat_DITHER_z_triangle( GLcontext *ctx,
				    const SWvertex *v0,
				    const SWvertex *v1,
				    const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define SETUP_CODE	\
   FLAT_DITHER_SETUP( v2->color[0], v2->color[1], v2->color[2] );

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   FLAT_DITHER_ROW_SETUP(y);					\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
         unsigned long p = FLAT_DITHER(x);			\
	 XMesaPutPixel(img, x, y, p);				\
         zRow[i] = z;						\
      }								\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, 8-bit PF_HPCR triangle.
 */
static void flat_HPCR_z_triangle( GLcontext *ctx,
				  const SWvertex *v0,
				  const SWvertex *v1,
				  const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE		\
   GLubyte r = v2->color[0];	\
   GLubyte g = v2->color[1];	\
   GLubyte b = v2->color[2];
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);		\
      if (z < zRow[i]) {					\
	 pRow[i] = (PIXEL_TYPE) DITHER_HPCR(x, y, r, g, b);	\
         zRow[i] = z;						\
      }								\
      span.z += span.zStep;					\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, depth-buffered, 8-bit PF_LOOKUP triangle.
 */
static void flat_LOOKUP8_z_triangle( GLcontext *ctx,
				     const SWvertex *v0,
				     const SWvertex *v1,
				     const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE				\
   LOOKUP_SETUP;				\
   GLubyte r = v2->color[0];	\
   GLubyte g = v2->color[1];	\
   GLubyte b = v2->color[2];	\
   GLubyte p = LOOKUP(r,g,b);
#define RENDER_SPAN( span )				\
   GLuint i;						\
   for (i = 0; i < span.end; i++) {			\
      const DEPTH_TYPE z = FixedToDepth(span.z);	\
      if (z < zRow[i]) {				\
	 pRow[i] = p;					\
         zRow[i] = z;					\
      }							\
      span.z += span.zStep;				\
   }

#include "swrast/s_tritemp.h"
}



/*
 * XImage, smooth, NON-depth-buffered, PF_TRUECOLOR triangle.
 */
static void smooth_TRUECOLOR_triangle( GLcontext *ctx,
				       const SWvertex *v0,
				       const SWvertex *v1,
				       const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define INTERP_RGB 1
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      unsigned long p;						\
      PACK_TRUECOLOR(p, FixedToInt(span.red),			\
         FixedToInt(span.green), FixedToInt(span.blue));	\
      XMesaPutPixel(img, x, y, p);				\
      span.red += span.redStep;				\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_8A8B8G8R triangle.
 */
static void smooth_8A8B8G8R_triangle( GLcontext *ctx,
				      const SWvertex *v0,
				      const SWvertex *v1,
				      const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )					\
   GLuint i;							\
   for (i = 0; i < span.end; i++) {				\
      pRow[i] = PACK_8B8G8R(FixedToInt(span.red),		\
         FixedToInt(span.green), FixedToInt(span.blue) );	\
      span.red += span.redStep;				\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
   }								\

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_8R8G8B triangle.
 */
static void smooth_8R8G8B_triangle( GLcontext *ctx,
				    const SWvertex *v0,
				    const SWvertex *v1,
				    const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )					\
   GLuint i;							\
   for (i = 0; i < span.end; i++) {				\
      pRow[i] = PACK_8R8G8B(FixedToInt(span.red),		\
         FixedToInt(span.green), FixedToInt(span.blue) );	\
      span.red += span.redStep;				\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_8R8G8B triangle.
 */
static void smooth_8R8G8B24_triangle( GLcontext *ctx,
				      const SWvertex *v0,
				      const SWvertex *v1,
				      const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR3(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE bgr_t
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )				\
   GLuint i;						\
   PIXEL_TYPE *pixel = pRow;				\
   for (i = 0; i < span.end; i++, pixel++) {		\
      pixel->r = FixedToInt(span.red);			\
      pixel->g = FixedToInt(span.green);		\
      pixel->b = FixedToInt(span.blue);		\
      span.red += span.redStep;			\
      span.green += span.greenStep;			\
      span.blue += span.blueStep;			\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_TRUEDITHER triangle.
 */
static void smooth_TRUEDITHER_triangle( GLcontext *ctx,
					const SWvertex *v0,
					const SWvertex *v1,
					const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define INTERP_RGB 1
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      unsigned long p;						\
      PACK_TRUEDITHER(p, x, y, FixedToInt(span.red),		\
         FixedToInt(span.green), FixedToInt(span.blue));	\
      XMesaPutPixel(img, x, y, p );				\
      span.red += span.redStep;				\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_5R6G5B triangle.
 */
static void smooth_5R6G5B_triangle( GLcontext *ctx,
				    const SWvertex *v0,
				    const SWvertex *v1,
				    const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )					\
   GLuint i;							\
   for (i = 0; i < span.end; i++) {				\
      pRow[i] = (PIXEL_TYPE) PACK_5R6G5B(FixedToInt(span.red),	\
         FixedToInt(span.green), FixedToInt(span.blue));	\
      span.red += span.redStep;				\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_DITHER_5R6G5B triangle.
 */
static void smooth_DITHER_5R6G5B_triangle( GLcontext *ctx,
					   const SWvertex *v0,
					   const SWvertex *v1,
					   const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      PACK_TRUEDITHER(pRow[i], x, y, FixedToInt(span.red),	\
         FixedToInt(span.green), FixedToInt(span.blue));	\
      span.red += span.redStep;				\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, 8-bit PF_DITHER triangle.
 */
static void smooth_DITHER8_triangle( GLcontext *ctx,
				     const SWvertex *v0,
				     const SWvertex *v1,
				     const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   XDITHER_SETUP(y);						\
   for (i = 0; i < span.end; i++, x++) {			\
      pRow[i] = (PIXEL_TYPE) XDITHER(x, FixedToInt(span.red),	\
         FixedToInt(span.green), FixedToInt(span.blue) );	\
      span.red += span.redStep;				\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_DITHER triangle.
 */
static void smooth_DITHER_triangle( GLcontext *ctx,
				    const SWvertex *v0,
				    const SWvertex *v1,
				    const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define INTERP_RGB 1
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   XDITHER_SETUP(y);						\
   for (i = 0; i < span.end; i++, x++) {			\
      unsigned long p = XDITHER(x, FixedToInt(span.red),	\
         FixedToInt(span.green), FixedToInt(span.blue) );	\
      XMesaPutPixel(img, x, y, p);				\
      span.red += span.redStep;				\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, 8-bit PF_LOOKUP triangle.
 */
static void smooth_LOOKUP8_triangle( GLcontext *ctx,
				     const SWvertex *v0,
				     const SWvertex *v1,
				     const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )				\
   GLuint i;						\
   LOOKUP_SETUP;					\
   for (i = 0; i < span.end; i++) {			\
      pRow[i] = LOOKUP(FixedToInt(span.red),		\
         FixedToInt(span.green), FixedToInt(span.blue));\
      span.red += span.redStep;			\
      span.green += span.greenStep;			\
      span.blue += span.blueStep;			\
   }

#include "swrast/s_tritemp.h"
}



/*
 * XImage, smooth, NON-depth-buffered, 8-bit PF_HPCR triangle.
 */
static void smooth_HPCR_triangle( GLcontext *ctx,
				  const SWvertex *v0,
				  const SWvertex *v1,
				  const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      pRow[i] = DITHER_HPCR(x, y, FixedToInt(span.red),	\
         FixedToInt(span.green), FixedToInt(span.blue));	\
      span.red += span.redStep;				\
      span.green += span.greenStep;				\
      span.blue += span.blueStep;				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_TRUECOLOR triangle.
 */
static void flat_TRUECOLOR_triangle( GLcontext *ctx,
				     const SWvertex *v0,
				     const SWvertex *v1,
				     const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define SETUP_CODE						\
   unsigned long pixel;						\
   PACK_TRUECOLOR(pixel, v2->color[0], v2->color[1], v2->color[2]);

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      XMesaPutPixel(img, x, y, pixel);				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_8A8B8G8R triangle.
 */
static void flat_8A8B8G8R_triangle( GLcontext *ctx,
				    const SWvertex *v0,
				    const SWvertex *v1,
				    const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE					\
   unsigned long p = PACK_8B8G8R( v2->color[0],		\
		 v2->color[1], v2->color[2] );
#define RENDER_SPAN( span )			\
   GLuint i;					\
   for (i = 0; i < span.end; i++) {		\
      pRow[i] = (PIXEL_TYPE) p;			\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_8R8G8B triangle.
 */
static void flat_8R8G8B_triangle( GLcontext *ctx,
				  const SWvertex *v0,
				  const SWvertex *v1,
				  const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE				\
   unsigned long p = PACK_8R8G8B( v2->color[0],	\
		 v2->color[1], v2->color[2] );
#define RENDER_SPAN( span )			\
   GLuint i;					\
   for (i = 0; i < span.end; i++) {		\
      pRow[i] = (PIXEL_TYPE) p;			\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_8R8G8B24 triangle.
 */
static void flat_8R8G8B24_triangle( GLcontext *ctx,
				    const SWvertex *v0,
				    const SWvertex *v1,
				    const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = v2->color;
#define PIXEL_ADDRESS(X,Y) PIXELADDR3(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE bgr_t
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )				\
   GLuint i;						\
   PIXEL_TYPE *pixel = pRow;				\
   for (i = 0; i < span.end; i++, pixel++) {		\
      pixel->r = color[RCOMP];				\
      pixel->g = color[GCOMP];				\
      pixel->b = color[BCOMP];				\
   }

#include "swrast/s_tritemp.h"
}

/*
 * XImage, flat, NON-depth-buffered, PF_TRUEDITHER triangle.
 */
static void flat_TRUEDITHER_triangle( GLcontext *ctx,
				      const SWvertex *v0,
				      const SWvertex *v1,
				      const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      unsigned long p;						\
      PACK_TRUEDITHER(p, x, y, v2->color[0],			\
               v2->color[1], v2->color[2] );			\
      XMesaPutPixel(img, x, y, p);				\
   }

#include "swrast/s_tritemp.h"
}



/*
 * XImage, flat, NON-depth-buffered, PF_5R6G5B triangle.
 */
static void flat_5R6G5B_triangle( GLcontext *ctx,
				  const SWvertex *v0,
				  const SWvertex *v1,
				  const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE				\
   unsigned long p = PACK_5R6G5B( v2->color[0],	\
		 v2->color[1], v2->color[2] );
#define RENDER_SPAN( span )			\
   GLuint i;					\
   for (i = 0; i < span.end; i++) {		\
      pRow[i] = (PIXEL_TYPE) p;			\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_DITHER_5R6G5B triangle.
 */
static void flat_DITHER_5R6G5B_triangle( GLcontext *ctx,
					 const SWvertex *v0,
					 const SWvertex *v1,
					 const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = v2->color;
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      PACK_TRUEDITHER(pRow[i], x, y, color[RCOMP],		\
         color[GCOMP], color[BCOMP]);				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, 8-bit PF_DITHER triangle.
 */
static void flat_DITHER8_triangle( GLcontext *ctx,
				   const SWvertex *v0,
				   const SWvertex *v1,
				   const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE	\
   FLAT_DITHER_SETUP( v2->color[0], v2->color[1], v2->color[2] );

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   FLAT_DITHER_ROW_SETUP(FLIP(xmesa->xm_buffer, y));		\
   for (i = 0; i < span.end; i++, x++) {			\
      pRow[i] = (PIXEL_TYPE) FLAT_DITHER(x);			\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_DITHER triangle.
 */
static void flat_DITHER_triangle( GLcontext *ctx,
				  const SWvertex *v0,
				  const SWvertex *v1,
				  const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaImage *img = xmesa->xm_buffer->backimage;
#define SETUP_CODE	\
   FLAT_DITHER_SETUP( v2->color[0], v2->color[1], v2->color[2] );

#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   FLAT_DITHER_ROW_SETUP(y);					\
   for (i = 0; i < span.end; i++, x++) {			\
      unsigned long p = FLAT_DITHER(x);				\
      XMesaPutPixel(img, x, y, p );				\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, 8-bit PF_HPCR triangle.
 */
static void flat_HPCR_triangle( GLcontext *ctx,
				const SWvertex *v0,
				const SWvertex *v1,
				const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE		\
   GLubyte r = v2->color[0];	\
   GLubyte g = v2->color[1];	\
   GLubyte b = v2->color[2];
#define RENDER_SPAN( span )					\
   GLuint i;							\
   GLint x = span.x, y = FLIP(xmesa->xm_buffer, span.y);	\
   for (i = 0; i < span.end; i++, x++) {			\
      pRow[i] = (PIXEL_TYPE) DITHER_HPCR(x, y, r, g, b);	\
   }

#include "swrast/s_tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, 8-bit PF_LOOKUP triangle.
 */
static void flat_LOOKUP8_triangle( GLcontext *ctx,
				   const SWvertex *v0,
				   const SWvertex *v1,
				   const SWvertex *v2 )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(xmesa->xm_buffer,X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE				\
   LOOKUP_SETUP;				\
   GLubyte r = v2->color[0];			\
   GLubyte g = v2->color[1];			\
   GLubyte b = v2->color[2];			\
   GLubyte p = LOOKUP(r,g,b);
#define RENDER_SPAN( span )          		\
   GLuint i;					\
   for (i = 0; i < span.end; i++) {		\
      pRow[i] = (PIXEL_TYPE) p;			\
   }

#include "swrast/s_tritemp.h"
}


#ifdef DEBUG
extern void _xmesa_print_triangle_func( swrast_tri_func triFunc );
void _xmesa_print_triangle_func( swrast_tri_func triFunc )
{
   _mesa_printf("XMesa tri func = ");
   if (triFunc ==smooth_TRUECOLOR_z_triangle)
      _mesa_printf("smooth_TRUECOLOR_z_triangle\n");
   else if (triFunc ==smooth_8A8B8G8R_z_triangle)
      _mesa_printf("smooth_8A8B8G8R_z_triangle\n");
   else if (triFunc ==smooth_8R8G8B_z_triangle)
      _mesa_printf("smooth_8R8G8B_z_triangle\n");
   else if (triFunc ==smooth_8R8G8B24_z_triangle)
      _mesa_printf("smooth_8R8G8B24_z_triangle\n");
   else if (triFunc ==smooth_TRUEDITHER_z_triangle)
      _mesa_printf("smooth_TRUEDITHER_z_triangle\n");
   else if (triFunc ==smooth_5R6G5B_z_triangle)
      _mesa_printf("smooth_5R6G5B_z_triangle\n");
   else if (triFunc ==smooth_DITHER_5R6G5B_z_triangle)
      _mesa_printf("smooth_DITHER_5R6G5B_z_triangle\n");
   else if (triFunc ==smooth_HPCR_z_triangle)
      _mesa_printf("smooth_HPCR_z_triangle\n");
   else if (triFunc ==smooth_DITHER8_z_triangle)
      _mesa_printf("smooth_DITHER8_z_triangle\n");
   else if (triFunc ==smooth_LOOKUP8_z_triangle)
      _mesa_printf("smooth_LOOKUP8_z_triangle\n");
   else if (triFunc ==flat_TRUECOLOR_z_triangle)
      _mesa_printf("flat_TRUECOLOR_z_triangle\n");
   else if (triFunc ==flat_8A8B8G8R_z_triangle)
      _mesa_printf("flat_8A8B8G8R_z_triangle\n");
   else if (triFunc ==flat_8R8G8B_z_triangle)
      _mesa_printf("flat_8R8G8B_z_triangle\n");
   else if (triFunc ==flat_8R8G8B24_z_triangle)
      _mesa_printf("flat_8R8G8B24_z_triangle\n");
   else if (triFunc ==flat_TRUEDITHER_z_triangle)
      _mesa_printf("flat_TRUEDITHER_z_triangle\n");
   else if (triFunc ==flat_5R6G5B_z_triangle)
      _mesa_printf("flat_5R6G5B_z_triangle\n");
   else if (triFunc ==flat_DITHER_5R6G5B_z_triangle)
      _mesa_printf("flat_DITHER_5R6G5B_z_triangle\n");
   else if (triFunc ==flat_HPCR_z_triangle)
      _mesa_printf("flat_HPCR_z_triangle\n");
   else if (triFunc ==flat_DITHER8_z_triangle)
      _mesa_printf("flat_DITHER8_z_triangle\n");
   else if (triFunc ==flat_LOOKUP8_z_triangle)
      _mesa_printf("flat_LOOKUP8_z_triangle\n");
   else if (triFunc ==smooth_TRUECOLOR_triangle)
      _mesa_printf("smooth_TRUECOLOR_triangle\n");
   else if (triFunc ==smooth_8A8B8G8R_triangle)
      _mesa_printf("smooth_8A8B8G8R_triangle\n");
   else if (triFunc ==smooth_8R8G8B_triangle)
      _mesa_printf("smooth_8R8G8B_triangle\n");
   else if (triFunc ==smooth_8R8G8B24_triangle)
      _mesa_printf("smooth_8R8G8B24_triangle\n");
   else if (triFunc ==smooth_TRUEDITHER_triangle)
      _mesa_printf("smooth_TRUEDITHER_triangle\n");
   else if (triFunc ==smooth_5R6G5B_triangle)
      _mesa_printf("smooth_5R6G5B_triangle\n");
   else if (triFunc ==smooth_DITHER_5R6G5B_triangle)
      _mesa_printf("smooth_DITHER_5R6G5B_triangle\n");
   else if (triFunc ==smooth_HPCR_triangle)
      _mesa_printf("smooth_HPCR_triangle\n");
   else if (triFunc ==smooth_DITHER8_triangle)
      _mesa_printf("smooth_DITHER8_triangle\n");
   else if (triFunc ==smooth_LOOKUP8_triangle)
      _mesa_printf("smooth_LOOKUP8_triangle\n");
   else if (triFunc ==flat_TRUECOLOR_triangle)
      _mesa_printf("flat_TRUECOLOR_triangle\n");
   else if (triFunc ==flat_TRUEDITHER_triangle)
      _mesa_printf("flat_TRUEDITHER_triangle\n");
   else if (triFunc ==flat_8A8B8G8R_triangle)
      _mesa_printf("flat_8A8B8G8R_triangle\n");
   else if (triFunc ==flat_8R8G8B_triangle)
      _mesa_printf("flat_8R8G8B_triangle\n");
   else if (triFunc ==flat_8R8G8B24_triangle)
      _mesa_printf("flat_8R8G8B24_triangle\n");
   else if (triFunc ==flat_5R6G5B_triangle)
      _mesa_printf("flat_5R6G5B_triangle\n");
   else if (triFunc ==flat_DITHER_5R6G5B_triangle)
      _mesa_printf("flat_DITHER_5R6G5B_triangle\n");
   else if (triFunc ==flat_HPCR_triangle)
      _mesa_printf("flat_HPCR_triangle\n");
   else if (triFunc ==flat_DITHER8_triangle)
      _mesa_printf("flat_DITHER8_triangle\n");
   else if (triFunc ==flat_LOOKUP8_triangle)
      _mesa_printf("flat_LOOKUP8_triangle\n");
   else
      _mesa_printf("???\n");
}
#endif


#ifdef DEBUG

/* record the current triangle function name */
static const char *triFuncName = NULL;

#define USE(triFunc)                   \
do {                                   \
    triFuncName = #triFunc;            \
    return triFunc;                    \
} while (0)

#else

#define USE(triFunc)  return triFunc

#endif


static swrast_tri_func get_triangle_func( GLcontext *ctx )
{
   SWcontext *swrast = SWRAST_CONTEXT(ctx);
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   int depth = GET_VISUAL_DEPTH(xmesa->xm_visual);

   (void) kernel1;

#ifdef DEBUG
   triFuncName = NULL;
#endif

   if (ctx->RenderMode != GL_RENDER)  return (swrast_tri_func) NULL;
   if (ctx->Polygon.SmoothFlag)       return (swrast_tri_func) NULL;
   if (ctx->Texture._EnabledUnits)    return (swrast_tri_func) NULL;
   if (swrast->_RasterMask & MULTI_DRAW_BIT) return (swrast_tri_func) NULL;
   if (ctx->Polygon.CullFlag && 
       ctx->Polygon.CullFaceMode == GL_FRONT_AND_BACK)
                                        return (swrast_tri_func) NULL;

   if (xmesa->xm_buffer->buffer==XIMAGE) {
      if (   ctx->Light.ShadeModel==GL_SMOOTH
          && swrast->_RasterMask==DEPTH_BIT
          && ctx->Depth.Func==GL_LESS
          && ctx->Depth.Mask==GL_TRUE
          && ctx->Visual.depthBits == DEFAULT_SOFTWARE_DEPTH_BITS
          && ctx->Polygon.StippleFlag==GL_FALSE) {
         switch (xmesa->pixelformat) {
            case PF_TRUECOLOR:
	       USE(smooth_TRUECOLOR_z_triangle);
            case PF_8A8B8G8R:
               USE(smooth_8A8B8G8R_z_triangle);
            case PF_8R8G8B:
               USE(smooth_8R8G8B_z_triangle);
            case PF_8R8G8B24:
               USE(smooth_8R8G8B24_z_triangle);
            case PF_TRUEDITHER:
               USE(smooth_TRUEDITHER_z_triangle);
            case PF_5R6G5B:
               USE(smooth_5R6G5B_z_triangle);
            case PF_DITHER_5R6G5B:
               USE(smooth_DITHER_5R6G5B_z_triangle);
            case PF_HPCR:
	       USE(smooth_HPCR_z_triangle);
            case PF_DITHER:
               if (depth == 8)
                  USE(smooth_DITHER8_z_triangle);
               else
                  USE(smooth_DITHER_z_triangle);
               break;
            case PF_LOOKUP:
               if (depth == 8)
                  USE(smooth_LOOKUP8_z_triangle);
               else
                  return (swrast_tri_func) NULL;
            default:
               return (swrast_tri_func) NULL;
         }
      }
      if (   ctx->Light.ShadeModel==GL_FLAT
          && swrast->_RasterMask==DEPTH_BIT
          && ctx->Depth.Func==GL_LESS
          && ctx->Depth.Mask==GL_TRUE
          && ctx->Visual.depthBits == DEFAULT_SOFTWARE_DEPTH_BITS
          && ctx->Polygon.StippleFlag==GL_FALSE) {
         switch (xmesa->pixelformat) {
            case PF_TRUECOLOR:
	       USE(flat_TRUECOLOR_z_triangle);
            case PF_8A8B8G8R:
               USE(flat_8A8B8G8R_z_triangle);
            case PF_8R8G8B:
               USE(flat_8R8G8B_z_triangle);
            case PF_8R8G8B24:
               USE(flat_8R8G8B24_z_triangle);
            case PF_TRUEDITHER:
               USE(flat_TRUEDITHER_z_triangle);
            case PF_5R6G5B:
               USE(flat_5R6G5B_z_triangle);
            case PF_DITHER_5R6G5B:
               USE(flat_DITHER_5R6G5B_z_triangle);
            case PF_HPCR:
	       USE(flat_HPCR_z_triangle);
            case PF_DITHER:
               if (depth == 8)
                  USE(flat_DITHER8_z_triangle);
               else
                  USE(flat_DITHER_z_triangle);
               break;
            case PF_LOOKUP:
               if (depth == 8)
                  USE(flat_LOOKUP8_z_triangle);
               else
                  return (swrast_tri_func) NULL;
            default:
               return (swrast_tri_func) NULL;
         }
      }
      if (   swrast->_RasterMask==0   /* no depth test */
          && ctx->Light.ShadeModel==GL_SMOOTH
          && ctx->Polygon.StippleFlag==GL_FALSE) {
         switch (xmesa->pixelformat) {
            case PF_TRUECOLOR:
	       USE(smooth_TRUECOLOR_triangle);
            case PF_8A8B8G8R:
               USE(smooth_8A8B8G8R_triangle);
            case PF_8R8G8B:
               USE(smooth_8R8G8B_triangle);
            case PF_8R8G8B24:
               USE(smooth_8R8G8B24_triangle);
            case PF_TRUEDITHER:
               USE(smooth_TRUEDITHER_triangle);
            case PF_5R6G5B:
               USE(smooth_5R6G5B_triangle);
            case PF_DITHER_5R6G5B:
               USE(smooth_DITHER_5R6G5B_triangle);
            case PF_HPCR:
	       USE(smooth_HPCR_triangle);
            case PF_DITHER:
               if (depth == 8)
                  USE(smooth_DITHER8_triangle);
               else
                  USE(smooth_DITHER_triangle);
               break;
            case PF_LOOKUP:
               if (depth == 8)
                  USE(smooth_LOOKUP8_triangle);
               else
                  return (swrast_tri_func) NULL;
            default:
               return (swrast_tri_func) NULL;
         }
      }

      if (   swrast->_RasterMask==0   /* no depth test */
          && ctx->Light.ShadeModel==GL_FLAT
          && ctx->Polygon.StippleFlag==GL_FALSE) {
         switch (xmesa->pixelformat) {
            case PF_TRUECOLOR:
	       USE(flat_TRUECOLOR_triangle);
            case PF_TRUEDITHER:
	       USE(flat_TRUEDITHER_triangle);
            case PF_8A8B8G8R:
               USE(flat_8A8B8G8R_triangle);
            case PF_8R8G8B:
               USE(flat_8R8G8B_triangle);
            case PF_8R8G8B24:
               USE(flat_8R8G8B24_triangle);
            case PF_5R6G5B:
               USE(flat_5R6G5B_triangle);
            case PF_DITHER_5R6G5B:
               USE(flat_DITHER_5R6G5B_triangle);
            case PF_HPCR:
	       USE(flat_HPCR_triangle);
            case PF_DITHER:
               if (depth == 8)
                  USE(flat_DITHER8_triangle);
               else
                  USE(flat_DITHER_triangle);
               break;
            case PF_LOOKUP:
               if (depth == 8)
                  USE(flat_LOOKUP8_triangle);
               else
                  return (swrast_tri_func) NULL;
            default:
               return (swrast_tri_func) NULL;
         }
      }

      return (swrast_tri_func) NULL;
   }
   else {
      /* draw to pixmap */
      return (swrast_tri_func) NULL;
   }
}


/* Override for the swrast tri-selection function.  Try to use one
 * of our internal tri functions, otherwise fall back to the
 * standard swrast functions.
 */
void xmesa_choose_triangle( GLcontext *ctx )
{
   SWcontext *swrast = SWRAST_CONTEXT(ctx);

   if (!(swrast->Triangle = get_triangle_func( ctx )))
      _swrast_choose_triangle( ctx );
}

