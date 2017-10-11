/* ugl_tri.c - UGL/Mesa Triangle */

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
[smooth|flat]_[z|]_[triangle|line]_[ARGB8888|ARGB4444|RGB888|RGB565|DITHER16]
*/

#include "glheader.h"
#include "uglmesaP.h"
#include "depth.h"
#include "macros.h"
#include "mmath.h"
#include "mtypes.h"

#include "swrast/s_context.h"
#include "swrast/s_depth.h"
#include "swrast/s_triangle.h"
#include "swrast/s_trispan.h"

/*
 * smooth, depth-buffered, ARGB8888 triangle
 */
static void smooth_z_triangle_ARGB8888(GLcontext * ctx,
				       const SWvertex * v0,
				       const SWvertex * v1,
				       const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   UGL_UINT32* pRow = PIXELADDR4 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      const DEPTH_TYPE z = FixedToDepth (span.z);		     \
      if (z < zRow[i]) {                                             \
          pRow[i] = PACK_RGB888 (FixedToInt(span.red),               \
                                 FixedToInt(span.green),             \
				 FixedToInt(span.blue));	     \
          zRow[i] = z;					             \
      }							             \
      span.red += span.redStep;				             \
      span.green += span.greenStep;				     \
      span.blue += span.blueStep;				     \
      span.z += span.zStep;					     \
   }

#include "swrast/s_tritemp.h"
}

/*
 * smooth, depth-buffered, ARGB4444 triangle
 */
static void smooth_z_triangle_ARGB4444(GLcontext * ctx,
				       const SWvertex * v0,
				       const SWvertex * v1,
				       const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      const DEPTH_TYPE z = FixedToDepth(span.z);		     \
      if (z < zRow[i]) {                                             \
         pRow[i] = PACK_RGB444 (FixedToInt(span.red),                \
                                FixedToInt(span.green),              \
				FixedToInt(span.blue));	             \
         zRow[i] = z;					             \
      }							             \
      span.red += span.redStep;				             \
      span.green += span.greenStep;				     \
      span.blue += span.blueStep;				     \
      span.z += span.zStep;					     \
   }

#include "swrast/s_tritemp.h"
}

/*
 * smooth, depth-buffered, RGB888 triangle
 */
static void smooth_z_triangle_RGB888(GLcontext * ctx,
				     const SWvertex * v0,
				     const SWvertex * v1,
				     const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   _RGB_T* pRow = PIXELADDR3 (umc->drawBuffer,span.x,span.y);        \
   for (i = 0; i < span.end; i++) {                                \
      const DEPTH_TYPE z = FixedToDepth(span.z);		     \
      if (z < zRow[i]) {                                             \
	 _RGB_T *ptr = pRow + i;                                     \
         ptr->r = FixedToInt(span.red);		                     \
         ptr->g = FixedToInt(span.green);                            \
         ptr->b = FixedToInt(span.blue);	                     \
         zRow[i] = z;					             \
      }							             \
      span.red += span.redStep;				             \
      span.green += span.greenStep;				     \
      span.blue += span.blueStep;				     \
      span.z += span.zStep;					     \
   }

#include "swrast/s_tritemp.h"
}

/*
 * smooth, depth-buffered, RGB565 triangle
 */
static void smooth_z_triangle_RGB565(GLcontext * ctx,
				     const SWvertex * v0,
				     const SWvertex * v1,
				     const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      const DEPTH_TYPE z = FixedToDepth(span.z);                     \
      if (z < zRow[i]) {                                             \
         pRow[i] = PACK_RGB565 (FixedToInt(span.red),                \
                                FixedToInt(span.green),              \
       			        FixedToInt(span.blue));	             \
         zRow[i] = z;                                                \
      }                                                              \
      span.red += span.redStep;				             \
      span.green += span.greenStep;				     \
      span.blue += span.blueStep;				     \
      span.z += span.zStep;					     \
   }

#include "swrast/s_tritemp.h"
}

/*
 * smooth, depth-buffered, dithered RGB triangle
 */
static void smooth_z_triangle_DITHER16(GLcontext * ctx,
				       const SWvertex * v0,
				       const SWvertex * v1,
				       const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   GLint x = span.x, y = span.y;                                     \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++, x++) {                           \
      const DEPTH_TYPE z = FixedToDepth(span.z);                     \
      if (z < zRow[i]) {                                             \
         PACK_DITHER16(pRow[i], x, y, FixedToInt(span.red),          \
		       FixedToInt(span.green),                       \
		       FixedToInt(span.blue));	                     \
         zRow[i] = z;                                                \
      }                                                              \
      span.red += span.redStep;				             \
      span.green += span.greenStep;				     \
      span.blue += span.blueStep;				     \
      span.z += span.zStep;					     \
   }

#include "swrast/s_tritemp.h"
}

/*
 * smooth, depth-buffered, WindML triangle
 */
static void smooth_z_triangle_WINDML(GLcontext * ctx,
				     const SWvertex * v0,
				     const SWvertex * v1,
				     const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   GLint x = span.x, y = FLIP(span.y);                               \
   UGL_RGB rgb_color[span.end];                                    \
   UGL_COLOR ugl_color[span.end];                                  \
   for (i = 0; i < span.end; i++) {                                \
      rgb_color[i] = UGL_MAKE_RGB (FixedToInt(span.red),             \
                                   FixedToInt(span.green),           \
				   FixedToInt(span.blue));           \
      span.red += span.redStep;				             \
      span.green += span.greenStep;				     \
      span.blue += span.blueStep;				     \
   }                                                                 \
   uglColorAlloc(umc->devId, rgb_color, UGL_NULL, ugl_color,         \
		 span.end);                                        \
   for (i = 0; i < span.end; i++, x++) {                           \
      const DEPTH_TYPE z = FixedToDepth(span.z);                     \
      if (z < zRow[i]) {                                             \
         uglPixelSet(umc->gc, x, y, ugl_color[i]);                   \
         zRow[i] = z;                                                \
      }                                                              \
      span.z += span.zStep;					     \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, depth-buffered, ARGB8888 triangle.
 */
static void flat_z_triangle_ARGB8888(GLcontext * ctx,
				     const SWvertex * v0,
				     const SWvertex * v1,
				     const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define SETUP_CODE					             \
   unsigned long p = PACK_RGB888 (v2->color[0],	v2->color[1],        \
				  v2->color[2]);
#define RENDER_SPAN(span) 				             \
   GLuint i;						             \
   UGL_UINT32* pRow = PIXELADDR4 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      const DEPTH_TYPE z = FixedToDepth(span.z);	             \
      if (z < zRow[i]) {                                             \
	 pRow[i] = p;			                             \
         zRow[i] = z;					             \
      }							             \
   span.z += span.zStep;				             \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, depth-buffered, ARGB4444 triangle.
 */
static void flat_z_triangle_ARGB4444(GLcontext * ctx,
				     const SWvertex * v0,
				     const SWvertex * v1,
				     const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define SETUP_CODE					             \
   unsigned long p = PACK_RGB444 (v2->color[0], v2->color[1],        \
                                  v2->color[2]);
#define RENDER_SPAN(span)				             \
   GLuint i;						             \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      const DEPTH_TYPE z = FixedToDepth(span.z);	             \
      if (z < zRow[i]) {                                             \
	 pRow[i] = p;			                             \
         zRow[i] = z;					             \
      }							             \
      span.z += span.zStep;				             \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, depth-buffered, RGB888 triangle.
 */
static void flat_z_triangle_RGB888(GLcontext * ctx,
				   const SWvertex * v0,
				   const SWvertex * v1,
				   const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = v2->color;
   
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define RENDER_SPAN(span)				             \
   GLuint i;						             \
   _RGB_T* pRow = PIXELADDR3 (umc->drawBuffer,span.x,span.y);        \
   for (i = 0; i < span.end; i++) {                                \
      const DEPTH_TYPE z = FixedToDepth(span.z);	             \
      if (z < zRow[i]) {                                             \
	 _RGB_T *ptr = pRow + i;			             \
         ptr->r = color[RCOMP];				             \
         ptr->g = color[GCOMP];				             \
         ptr->b = color[BCOMP];				             \
         zRow[i] = z;					             \
      }							             \
      span.z += span.zStep;				             \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, depth-buffered, RGB565 triangle.
 */
static void flat_z_triangle_RGB565(GLcontext * ctx,
				   const SWvertex * v0,
				   const SWvertex * v1,
				   const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define SETUP_CODE					             \
   unsigned long p = PACK_RGB565 (v2->color[0], v2->color[1],        \
				  v2->color[2]);
#define RENDER_SPAN(span)		                             \
   GLuint i;						             \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      const DEPTH_TYPE z = FixedToDepth(span.z);	             \
      if (z < zRow[i]) {                                             \
	 pRow[i] = p;			                             \
         zRow[i] = z;					             \
      }							             \
      span.z += span.zStep;				             \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, depth-buffered, dithered RGB triangle.
 */
static void flat_z_triangle_DITHER16(GLcontext * ctx,
				     const SWvertex * v0,
				     const SWvertex * v1,
				     const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = v2->color;

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define RENDER_SPAN(span)		                             \
   GLuint i;						             \
   GLint x = span.x, y = span.y;                                     \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++, x++) {                           \
      const DEPTH_TYPE z = FixedToDepth(span.z);	             \
      if (z < zRow[i]) {                                             \
	 PACK_DITHER16(pRow[i], x, y, color[0], color[1],            \
		       color[2]);			             \
         zRow[i] = z;					             \
      }							             \
      span.z += span.zStep;				             \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, depth-buffered, WindML triangle.
 */
static void flat_z_triangle_WINDML(GLcontext * ctx,
				   const SWvertex * v0,
				   const SWvertex * v1,
				   const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = v2->color;
   
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define SETUP_CODE                                                   \
   UGL_RGB rgb_color;                                                \
   UGL_COLOR ugl_color;                                              \
   rgb_color = UGL_MAKE_RGB (color[0], color[1], color[2]);          \
   uglColorAlloc(umc->devId, &rgb_color, UGL_NULL, &ugl_color, 1);
#define RENDER_SPAN(span)		                             \
   GLuint i;						             \
   GLint x = span.x, y = FLIP(span.y);                               \
   for (i = 0; i < span.end; i++, x++) {                           \
      const DEPTH_TYPE z = FixedToDepth(span.z);	             \
      if (z < zRow[i]) {                                             \
         uglPixelSet(umc->gc, x, y, ugl_color);                      \
         zRow[i] = z;					             \
      }							             \
      span.z += span.zStep;				             \
   }

#include "swrast/s_tritemp.h"
}   
   
/*
 * smooth, NON-depth-buffered, ARGB8888 triangle.
 */
static void smooth_triangle_ARGB8888(GLcontext * ctx,
				     const SWvertex * v0,
				     const SWvertex * v1,
				     const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_RGB 1
#define RENDER_SPAN(span)					     \
   GLuint i;							     \
   UGL_UINT32* pRow = PIXELADDR4 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      pRow[i] = PACK_RGB888 (FixedToInt(span.red),		     \
                             FixedToInt(span.green),                 \
			     FixedToInt(span.blue));	             \
      span.red += span.redStep;				             \
      span.green += span.greenStep;				     \
      span.blue += span.blueStep;				     \
   }

#include "swrast/s_tritemp.h"
}

/*
 * smooth, NON-depth-buffered, ARGB4444 triangle.
 */
static void smooth_triangle_ARGB4444(GLcontext * ctx,
				     const SWvertex * v0,
				     const SWvertex * v1,
				     const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_RGB 1
#define RENDER_SPAN(span)					    \
   GLuint i;							    \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);   \
   for (i = 0; i < span.end; i++) {                               \
      pRow[i] = PACK_RGB444 (FixedToInt(span.red),                  \
                             FixedToInt(span.green),                \
			     FixedToInt(span.blue));	            \
      span.red += span.redStep;					    \
      span.green += span.greenStep;				    \
      span.blue += span.blueStep;				    \
   }

#include "swrast/s_tritemp.h"
}

/*
 * smooth, NON-depth-buffered, RGB888 triangle.
 */
static void smooth_triangle_RGB888(GLcontext * ctx,
				   const SWvertex * v0,
				   const SWvertex * v1,
				   const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_RGB 1
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   _RGB_T *pixel = PIXELADDR3 (umc->drawBuffer,span.x,span.y);       \
   for (i = 0; i < span.end; i++, pixel++) {                       \
      pixel->r = FixedToInt(span.red);			             \
      pixel->g = FixedToInt(span.green);		             \
      pixel->b = FixedToInt(span.blue);			             \
      span.red += span.redStep;				             \
      span.green += span.greenStep;                                  \
      span.blue += span.blueStep;        		             \
   }

#include "swrast/s_tritemp.h"
}

/*
 * smooth, NON-depth-buffered, RGB565 triangle.
 */
static void smooth_triangle_RGB565(GLcontext * ctx,
				   const SWvertex * v0,
				   const SWvertex * v1,
				   const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_RGB 1
#define RENDER_SPAN(span)					     \
   GLuint i;                                                         \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      pRow[i] = PACK_RGB565 (FixedToInt(span.red),                   \
                             FixedToInt(span.green),                 \
			     FixedToInt(span.blue));                 \
      span.red += span.redStep;				             \
      span.green += span.greenStep;				     \
      span.blue += span.blueStep;                                    \
   }

#include "swrast/s_tritemp.h"
}

/*
 * smooth, NON-depth-buffered, dithered RGB triangle.
 */
static void smooth_triangle_DITHER16(GLcontext * ctx,
				     const SWvertex * v0,
				     const SWvertex * v1,
				     const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define INTERP_RGB 1
#define RENDER_SPAN(span)					     \
   GLuint i;                                                         \
   GLint x = span.x, y = span.y;                                     \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++, x++) {                           \
      PACK_DITHER16(pRow[i], x, y, FixedToInt(span.red),             \
                    FixedToInt(span.green), FixedToInt(span.blue));  \
      span.red += span.redStep;				             \
      span.green += span.greenStep;				     \
      span.blue += span.blueStep;                                    \
   }

#include "swrast/s_tritemp.h"
}

/*
 * smooth, NON-depth-buffered, WindML triangle.
 */
static void smooth_triangle_WINDML(GLcontext * ctx,
				   const SWvertex * v0,
				   const SWvertex * v1,
				   const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   
#define INTERP_RGB 1
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   GLint x = span.x, y = FLIP(span.y);                               \
   UGL_RGB rgb_color[span.end];                                    \
   UGL_COLOR ugl_color[span.end];                                  \
   for (i = 0; i < span.end; i++) {                                \
      rgb_color[i] = UGL_MAKE_RGB (FixedToInt(span.red),             \
                                   FixedToInt(span.green),           \
				   FixedToInt(span.blue));	     \
      span.red += span.redStep;				             \
      span.green += span.greenStep;				     \
      span.blue += span.blueStep;				     \
   }                                                                 \
   uglColorAlloc(umc->devId, rgb_color, UGL_NULL, ugl_color,         \
		 span.end);                                        \
   for (i=0; i < span.end; i++, x++) {                             \
      uglPixelSet(umc->gc, x, y, ugl_color[i]);                      \
   }   
   
#include "swrast/s_tritemp.h"      
}

/*
 * flat, NON-depth-buffered, ARGB8888 triangle.
 */
static void flat_triangle_ARGB8888(GLcontext * ctx,
				   const SWvertex * v0,
				   const SWvertex * v1,
				   const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define SETUP_CODE					             \
   unsigned long p = PACK_RGB888 (v2->color[0], v2->color[1],        \
                                  v2->color[2]);
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   UGL_UINT32* pRow = PIXELADDR4 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      pRow[i] = p;                                                   \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, NON-depth-buffered, ARGB4444 triangle.
 */
static void flat_triangle_ARGB4444(GLcontext * ctx,
				   const SWvertex * v0,
				   const SWvertex * v1,
				   const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define SETUP_CODE					             \
   unsigned long p = PACK_RGB444 (v2->color[0], v2->color[1],        \
                                  v2->color[2]);
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      pRow[i] = p;                                                   \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, NON-depth-buffered, RGB888 triangle.
 */
static void flat_triangle_RGB888(GLcontext * ctx,
				 const SWvertex * v0,
				 const SWvertex * v1,
				 const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = v2->color;
   
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   _RGB_T *pixel = PIXELADDR3 (umc->drawBuffer,span.x,span.y);       \
   for (i = 0; i < span.end; i++, pixel++) {                       \
      pixel->r = color[RCOMP];				             \
      pixel->g = color[GCOMP];				             \
      pixel->b = color[BCOMP];				             \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, NON-depth-buffered, RGB565 triangle.
 */
static void flat_triangle_RGB565(GLcontext * ctx,
				 const SWvertex * v0,
				 const SWvertex * v1,
				 const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define SETUP_CODE					             \
   unsigned long p = PACK_RGB565 (v2->color[0], v2->color[1],        \
                                  v2->color[2]);
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++) {                                \
      pRow[i] = p;                                                   \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, NON-depth-buffered, dithered RGB triangle.
 */
static void flat_triangle_DITHER16(GLcontext * ctx,
				   const SWvertex * v0,
				   const SWvertex * v1,
				   const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte *color = v2->color;
   
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   GLint x = span.x, y = span.y;                                     \
   UGL_UINT16* pRow = PIXELADDR2 (umc->drawBuffer,span.x,span.y);    \
   for (i = 0; i < span.end; i++, x++) {                           \
      PACK_DITHER16(pRow[i], x, y, color[0], color[1], color[2]);    \
   }

#include "swrast/s_tritemp.h"
}

/*
 * flat, NON-depth-buffered, WindML triangle.
 */
static void flat_triangle_WINDML(GLcontext * ctx,
				 const SWvertex * v0,
				 const SWvertex * v1,
				 const SWvertex * v2)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

#define SETUP_CODE                                                   \
   UGL_RGB rgb_color;                                                \
   UGL_COLOR ugl_color;                                              \
   rgb_color = UGL_MAKE_RGB (v2->color[0], v2->color[1],             \
			     v2->color[2]);                          \
   uglColorAlloc(umc->devId, &rgb_color, UGL_NULL, &ugl_color, 1);
#define RENDER_SPAN(span)                                            \
   GLuint i;                                                         \
   GLint x = span.x, y = FLIP(span.y);                               \
   for (i = 0; i < span.end; i++, x++) {                           \
      uglPixelSet(umc->gc, x, y, ugl_color);                         \
   }       

#include "swrast/s_tritemp.h"
}

static swrast_tri_func get_triangle_func(GLcontext * ctx)
{
   SWcontext *swrast = SWRAST_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   if (ctx->RenderMode != GL_RENDER)
      return (swrast_tri_func) NULL;
   if (ctx->Polygon.SmoothFlag)
      return (swrast_tri_func) NULL;
   if (ctx->Texture._EnabledUnits)
      return (swrast_tri_func) NULL;
   if (ctx->swrast->_Rastermask & MULTI_DRAW_BIT)
      return (swrast_tri_func) NULL;

   if (ctx->Light.ShadeModel == GL_SMOOTH
       && swrast->_RasterMask == DEPTH_BIT
       && ctx->Depth.Func == GL_LESS
       && ctx->Depth.Mask == GL_TRUE
       && ctx->Visual.depthBits == DEFAULT_SOFTWARE_DEPTH_BITS
       && ctx->Polygon.StippleFlag == GL_FALSE) {
      switch (umc->pixelFormat) {
	 case UGL_MESA_ARGB8888:
	    return smooth_z_triangle_ARGB8888;
	 case UGL_MESA_ARGB4444:
	    return smooth_z_triangle_ARGB4444;
	 case UGL_MESA_RGB888:
	    return smooth_z_triangle_RGB888;
	 case UGL_MESA_RGB565:
	    return smooth_z_triangle_RGB565;
	 case UGL_MESA_DITHER16:
	    return smooth_z_triangle_DITHER16;
	 case UGL_MESA_WINDML:
	    return smooth_z_triangle_WINDML;
	 default:
	    return (swrast_tri_func) NULL;
      }
   }

   if (ctx->Light.ShadeModel == GL_FLAT
       && swrast->_RasterMask == DEPTH_BIT
       && ctx->Depth.Func == GL_LESS
       && ctx->Depth.Mask == GL_TRUE
       && ctx->Visual.depthBits == DEFAULT_SOFTWARE_DEPTH_BITS
       && ctx->Polygon.StippleFlag == GL_FALSE) {
      switch (umc->pixelFormat) {
	 case UGL_MESA_ARGB8888:
	    return flat_z_triangle_ARGB8888;
	 case UGL_MESA_ARGB4444:
	    return flat_z_triangle_ARGB4444;
	 case UGL_MESA_RGB888:
	    return flat_z_triangle_RGB888;
	 case UGL_MESA_RGB565:
	    return flat_z_triangle_RGB565;
	 case UGL_MESA_DITHER16:
	    return flat_z_triangle_DITHER16;
	 case UGL_MESA_WINDML:
	    return flat_z_triangle_WINDML;
	 default:
	    return (swrast_tri_func) NULL;
      }
   }

   if (swrast->_RasterMask == 0	/* no depth test */
       && ctx->Light.ShadeModel == GL_SMOOTH
       && ctx->Polygon.StippleFlag == GL_FALSE) {
      switch (umc->pixelFormat) {
	 case UGL_MESA_ARGB8888:
	    return smooth_triangle_ARGB8888;
	 case UGL_MESA_ARGB4444:
	    return smooth_triangle_ARGB4444;
	 case UGL_MESA_RGB888:
	    return smooth_triangle_RGB888;
	 case UGL_MESA_RGB565:
	    return smooth_triangle_RGB565;
	 case UGL_MESA_DITHER16:
	    return smooth_triangle_DITHER16;
	 case UGL_MESA_WINDML:
	    return smooth_triangle_WINDML;
	 default:
	    return (swrast_tri_func) NULL;
      }
   }

   if (swrast->_RasterMask == 0	/* no depth test */
       && ctx->Light.ShadeModel == GL_FLAT
       && ctx->Polygon.StippleFlag == GL_FALSE) {
      switch (umc->pixelFormat) {
	 case UGL_MESA_ARGB8888:
	    return flat_triangle_ARGB8888;
	 case UGL_MESA_ARGB4444:
	    return flat_triangle_ARGB4444;
	 case UGL_MESA_RGB888:
	    return flat_triangle_RGB888;
	 case UGL_MESA_RGB565:
	    return flat_triangle_RGB565;
	 case UGL_MESA_DITHER16:  
	    return flat_triangle_DITHER16;
	 case UGL_MESA_WINDML:
	    return flat_triangle_WINDML;
	 default:
	    return (swrast_tri_func) NULL;
      }
   }

   return (swrast_tri_func) NULL;
}


/* Override for the swrast tri-selection function.  Try to use one
 * of our internal tri functions, otherwise fall back to the
 * standard swrast functions.
 */
void uglmesa_choose_triangle(GLcontext * ctx)
{
   SWcontext *swrast = SWRAST_CONTEXT(ctx);

   if (!(swrast->Triangle = get_triangle_func(ctx)))
      _swrast_choose_triangle(ctx);
}
