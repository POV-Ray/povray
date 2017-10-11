/* ugl_line.c - UGL/Mesa Line */

/* Copyright (C) 2001 by Wind River Systems, Inc */

/*
 * Mesa 3-D graphics library
 * Version:  3.5
 *
 * The MIT License
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
 * THE AUTHORS OR COPYRIGHT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
 * Authors:
 * Stephane Raimbault <stephane.raimbault@windriver.com> 
 */

/*
[smooth|flat]_[z|]_[triangle|line]_
[ARGB8888|ARGB4444|RGB888|RGB565|DITHER16|WINDML]
*/

#include "glheader.h"
#include "uglmesaP.h"
#include "depth.h"
#include "macros.h"
#include "mmath.h"
#include "mtypes.h"

#include "swrast/s_depth.h"
#include "swrast/s_points.h"
#include "swrast/s_lines.h"
#include "swrast/s_triangle.h"
#include "swrast/s_context.h"

/*
 * Draw a flat-shaded ARGB8888 line
 */
static void flat_line_ARGB8888(GLcontext * ctx,
			       const SWvertex * vert0,
			       const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;
   UGL_UINT32 pixel = PACK_RGB888(color[0], color[1], color[2]);
   
#define PIXEL_TYPE UGL_UINT32
#define BYTES_PER_ROW (umc->rowLength)
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(umc->drawBuffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded ARGB8888 line
 */
static void flat_line_ARGB4444(GLcontext * ctx,
			       const SWvertex * vert0,
			       const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;
   UGL_UINT16 pixel = PACK_RGB444(color[0], color[1], color[2]);
   
#define PIXEL_TYPE UGL_UINT16
#define BYTES_PER_ROW (umc->rowLength)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(umc->drawBuffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded RGB888 line
 */
static void flat_line_RGB888(GLcontext * ctx,
			     const SWvertex * vert0,
			     const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;

#define PIXEL_TYPE _RGB_T
#define BYTES_PER_ROW (umc->rowLength)
#define PIXEL_ADDRESS(X,Y) PIXELADDR3(umc->drawBuffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) {            \
   pixelPtr->r = color[RCOMP]; \
   pixelPtr->g = color[GCOMP]; \
   pixelPtr->b = color[BCOMP]; \
}

#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded RGB565 line
 */
static void flat_line_RGB565(GLcontext * ctx,
			     const SWvertex * vert0,
			     const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;
   UGL_UINT16 pixel = PACK_RGB565(color[0], color[1], color[2]);
   
#define PIXEL_TYPE UGL_UINT16
#define BYTES_PER_ROW (umc->rowLength)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(umc->drawBuffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded dithered RGB line
 */
static void flat_line_DITHER16(GLcontext * ctx,
			       const SWvertex * vert0,
			       const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;
   
#define PIXEL_TYPE UGL_UINT16
#define BYTES_PER_ROW (umc->rowLength)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(umc->drawBuffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) PACK_DITHER16(*pixelPtr, X, Y, color[0], color[1], color[2]);

#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded WindML line
 */
static void flat_line_WINDML(GLcontext * ctx,
			     const SWvertex * vert0,
			     const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;
   UGL_RGB rgb_color = UGL_MAKE_RGB(color[0], color[1], color[2]);
   UGL_COLOR ugl_color;
   uglColorAlloc(umc->devId, &rgb_color, UGL_NULL, &ugl_color, 1);

#define INTERP_XY 1
#define CLIP_HACK 1
#define PLOT(X,Y) uglPixelSet(umc->gc, X, FLIP(Y), ugl_color);

#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded, Z-less, ARGB8888 line
 */
static void flat_z_line_ARGB8888(GLcontext * ctx, 
				 const SWvertex * vert0,
				 const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;
   UGL_UINT32 pixel = PACK_RGB888(color[0], color[1], color[2]);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE UGL_UINT32
#define BYTES_PER_ROW (umc->rowLength)
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(umc->drawBuffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)	 \
   if (Z < *zPtr) {	 \
      *zPtr = Z;	 \
      *pixelPtr = pixel; \
   }

#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded, Z-less, ARGB4444 line
 */
static void flat_z_line_ARGB4444(GLcontext * ctx,
				 const SWvertex * vert0,
				 const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;
   UGL_UINT16 pixel = PACK_RGB444(color[0], color[1], color[2]);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE UGL_UINT16
#define BYTES_PER_ROW (umc->rowLength)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(umc->drawBuffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)	 \
   if (Z < *zPtr) {	 \
      *zPtr = Z;	 \
      *pixelPtr = pixel; \
   }
#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded, Z-less, RGB888 line
 */
static void flat_z_line_RGB888(GLcontext * ctx, 
			       const SWvertex * vert0, 
			       const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;
   _RGB_T pixel;
   pixel.r = color[0];
   pixel.g = color[1];
   pixel.b = color[2];

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE _RGB_T
#define BYTES_PER_ROW (umc->rowLength)
#define PIXEL_ADDRESS(X,Y) PIXELADDR3(umc->drawBuffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		  \
   if (Z < *zPtr) {	          \
      *zPtr = Z;		  \
      pixelPtr->r = color[RCOMP]; \
      pixelPtr->g = color[GCOMP]; \
      pixelPtr->b = color[GCOMP]; \
   }
#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded, Z-less, RGB565 line
 */
static void flat_z_line_RGB565(GLcontext * ctx, 
			       const SWvertex * vert0, 
			       const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;
   UGL_UINT16 pixel = PACK_RGB565(color[0], color[1], color[2]);
   
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE UGL_UINT16
#define BYTES_PER_ROW (umc->rowLength)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(umc->drawBuffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)	 \
   if (Z < *zPtr) {	 \
      *zPtr = Z;	 \
      *pixelPtr = pixel; \
   }
#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded, Z-less, dithered RGB line
 */
static void flat_z_line_DITHER16(GLcontext * ctx, 
				 const SWvertex * vert0, 
				 const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_TYPE UGL_UINT16
#define BYTES_PER_ROW (umc->rowLength)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(umc->drawBuffer,X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		     \
   if (Z < *zPtr) {	             \
      *zPtr = Z;	             \
      PACK_DITHER16(*pixelPtr, X, Y, \
      color[0], color[1], color[2]); \
   }
#include "swrast/s_linetemp.h"
}

/*
 * Draw a flat-shaded, Z-less, WindML line
 */
static void flat_z_line_WINDML(GLcontext * ctx, 
			       const SWvertex * vert0, 
			       const SWvertex * vert1)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = vert1->color;
   UGL_RGB rgb_color = UGL_MAKE_RGB(color[0], color[1], color[2]);
   UGL_COLOR ugl_color;
   uglColorAlloc(umc->devId, &rgb_color, UGL_NULL, &ugl_color, 1);
   
#define INTERP_XY 1
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define CLIP_HACK 1
#define PLOT(X,Y)	                           \
   if (Z < *zPtr) {	                           \
      *zPtr = Z;	                           \
      uglPixelSet(umc->gc, X, FLIP(Y), ugl_color); \
   }
#include "swrast/s_linetemp.h"
}

static swrast_line_func get_line_func(GLcontext * ctx)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   SWcontext *swrast = SWRAST_CONTEXT(ctx);
   
   if (ctx->RenderMode != GL_RENDER)
      return (swrast_line_func) NULL;
   if (ctx->Line.SmoothFlag)
      return (swrast_line_func) NULL;
   if (ctx->Texture._EnabledUnits)
      return (swrast_line_func) NULL;
   if (ctx->Light.ShadeModel != GL_FLAT)
      return (swrast_line_func) NULL;
   if (ctx->Line.StippleFlag)
      return (swrast_line_func) NULL;
   if (ctx->swrast->_Rastermask & MULTI_DRAW_BIT)
      return (swrast_tri_func) NULL;
   
   if (swrast->_RasterMask == DEPTH_BIT
       && ctx->Depth.Func == GL_LESS
       && ctx->Depth.Mask == GL_TRUE
       && ctx->Visual.depthBits == DEFAULT_SOFTWARE_DEPTH_BITS
       && ctx->Line.Width == 1.0F) {
      switch (umc->pixelFormat) {
      case UGL_MESA_ARGB8888:
	 return flat_z_line_ARGB8888;
      case UGL_MESA_ARGB4444:
	 return flat_z_line_ARGB4444;
      case UGL_MESA_RGB888:
	 return flat_z_line_RGB888;
      case UGL_MESA_RGB565:
	 return flat_z_line_RGB565;
      case UGL_MESA_DITHER16:
	 return flat_z_line_DITHER16;
      case UGL_MESA_WINDML:
	 return flat_z_line_WINDML;
      default:
	 return (swrast_line_func) NULL;
      }
   }

   
   if (swrast->_RasterMask == 0 && ctx->Line.Width == 1.0F) {
      switch (umc->pixelFormat) {
      case UGL_MESA_ARGB8888:
	 return flat_line_ARGB8888;
      case UGL_MESA_ARGB4444:
	 return flat_line_ARGB4444;
      case UGL_MESA_RGB888:
	 return flat_line_RGB888;
      case UGL_MESA_RGB565:
	 return flat_line_RGB565;
      case UGL_MESA_DITHER16:
	 return flat_line_DITHER16;
      case UGL_MESA_WINDML:
	 return flat_line_WINDML;
      default:
	 return (swrast_line_func) NULL;
      }
   }
   
   return (swrast_line_func) NULL;
}

void uglmesa_choose_line(GLcontext * ctx)
{
   SWcontext *swrast = SWRAST_CONTEXT(ctx);

   if (!(swrast->Line = get_line_func(ctx)))
      _swrast_choose_line(ctx);
}

#define UGLMESA_NEW_POINT  (_NEW_POINT | \
                          _NEW_RENDERMODE | \
                          _SWRAST_NEW_RASTERMASK)

#define UGLMESA_NEW_LINE   (_NEW_LINE | \
                          _NEW_TEXTURE | \
                          _NEW_LIGHT | \
                          _NEW_DEPTH | \
                          _NEW_RENDERMODE | \
                          _SWRAST_NEW_RASTERMASK)

#define UGLMESA_NEW_TRIANGLE (_NEW_POLYGON | \
                            _NEW_TEXTURE | \
                            _NEW_LIGHT | \
                            _NEW_DEPTH | \
                            _NEW_RENDERMODE | \
                            _SWRAST_NEW_RASTERMASK)

void uglmesa_register_swrast_functions(GLcontext * ctx)
{
   SWcontext *swrast = SWRAST_CONTEXT(ctx);
   
   swrast->choose_point = _swrast_choose_point;
   swrast->choose_line = uglmesa_choose_line;
   swrast->choose_triangle = uglmesa_choose_triangle;

   swrast->invalidate_point |= UGLMESA_NEW_POINT;
   swrast->invalidate_line |= UGLMESA_NEW_LINE;
   swrast->invalidate_triangle |= UGLMESA_NEW_TRIANGLE;
}
