/* ugl_span.c - UGL/Mesa span functions */

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
[write|read]_[mono|]_[rgb|rgba|ci8|ci32]_[span_pixels]_
[ARGB8888|ARGB4444|RGB888|RGB565|DITHER16|CI|WINDML]
*/

#include "glheader.h"
#include "context.h"
#include "drawpix.h"
#include "imports.h"
#include "state.h"
#include "depth.h"
#include "macros.h"
#include "mtypes.h"
#include "uglmesaP.h"
#include "extensions.h"

#include "swrast/s_context.h"

/*
 * ARGB888
 */

/*
 * Write a span of RGBA pixels to an ARGB8888 buffer
 */
 
static void write_rgba_span_ARGB8888(const GLcontext * ctx, 
				     GLuint n, GLint x, GLint y,
				     CONST GLubyte rgba[][4],
				     const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT32 *p = PIXELADDR4(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = PACK_ARGB8888(rgba[i][ACOMP], rgba[i][RCOMP],
				 rgba[i][GCOMP], rgba[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = PACK_ARGB8888(rgba[i][ACOMP], rgba[i][RCOMP],
			      rgba[i][GCOMP], rgba[i][BCOMP]);
      }
   }
}

/*
 * Write a span of RGB pixels to an ARGB8888 buffer.
 */

static void write_rgb_span_ARGB8888(const GLcontext * ctx,
				    GLuint n, GLint x, GLint y,
				    CONST GLubyte rgb[][3],
				    const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT32 *p = PIXELADDR4(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = PACK_ARGB8888(255, rgb[i][RCOMP],
				 rgb[i][GCOMP], rgb[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = PACK_ARGB8888(255, rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP]);
      }
   }
}

/*
 * Write a span of mono RGBA pixels to an ARGB8888 buffer.
 */

static void write_mono_rgba_span_ARGB8888(const GLcontext * ctx,
					  GLuint n, GLint x, GLint y,
					  const GLubyte color[4],
					  const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT32 *p = PIXELADDR4(umc->drawBuffer, x, y);
   const UGL_UINT32 c = PACK_ARGB8888(color[ACOMP], color[RCOMP],
				      color[GCOMP], color[BCOMP]);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = c;
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = c;
      }
   }
}

/*
 * Write an array of RGBA pixels to an ARGB8888 buffer
 */

static void write_rgba_pixels_ARGB8888(const GLcontext * ctx, GLuint n,
				       const GLint x[], const GLint y[],
				       CONST GLubyte rgba[][4],
				       const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    UGL_UINT32 *p = PIXELADDR4(umc->drawBuffer, x[i], y[i]);
	    *p = PACK_ARGB8888(rgba[i][ACOMP], rgba[i][RCOMP],
			       rgba[i][GCOMP], rgba[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 UGL_UINT32 *p = PIXELADDR4(umc->drawBuffer, x[i], y[i]);
	 *p = PACK_ARGB8888(rgba[i][ACOMP], rgba[i][RCOMP],
			    rgba[i][GCOMP], rgba[i][BCOMP]);
      }
   }
}

/*
 * Write an array of monocolor RGBA pixels to an ARGB8888 buffer
 */

static void write_mono_rgba_pixels_ARGB8888(const GLcontext * ctx, GLuint n,
					    const GLint x[], const GLint y[],
					    const GLubyte color[4],
					    const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const UGL_UINT32 c = PACK_ARGB8888(color[ACOMP], color[RCOMP],
				      color[GCOMP], color[BCOMP]);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    UGL_UINT32 *p = PIXELADDR4(umc->drawBuffer, x[i], y[i]);
	    *p = c;
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 UGL_UINT32 *p = PIXELADDR4(umc->drawBuffer, x[i], y[i]);
	 *p = c;
      }
   }
}

/*
 * Read a span of RGBA pixels from an ARGB8888 buffer
 */

static void read_rgba_span_ARGB8888(const GLcontext * ctx,
				    GLuint n, GLint x, GLint y,
				    GLubyte rgba[][4])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT32 *p = PIXELADDR4(umc->readBuffer, x, y);
   register GLuint i;

   for (i = 0; i < n; i++) {
      const UGL_UINT32 c = p[i];
      rgba[i][ACOMP] = (GLubyte) ((c >> 24) & 0xff);
      rgba[i][RCOMP] = (GLubyte) ((c >> 16) & 0xff); 
      rgba[i][GCOMP] = (GLubyte) ((c >> 8) & 0xff);
      rgba[i][BCOMP] = (GLubyte) (c & 0xff);
   }
}

/*
 * Read an array of RGBA pixels from an ARGB8888 buffer
 */

static void read_rgba_pixels_ARGB8888(const GLcontext * ctx, GLuint n,
				      const GLint x[], const GLint y[],
				      GLubyte rgba[][4], const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   for (i = 0; i < n; i++) {
      if (mask[i]) {
	 const UGL_UINT32 *p = PIXELADDR4(umc->readBuffer, x[i], y[i]);
	 const UGL_UINT32 c = *p;
	 rgba[i][ACOMP] = (GLubyte) ((c >> 24) & 0xff);
	 rgba[i][RCOMP] = (GLubyte) ((c >> 16) & 0xff);
	 rgba[i][GCOMP] = (GLubyte) ((c >> 8) & 0xff);
	 rgba[i][BCOMP] = (GLubyte) (c & 0xff);
      }
   }
}

/*
 * ARGB4444
 */

/*
 * Write a span of RGBA pixels to an ARGB4444 buffer
 */

static void write_rgba_span_ARGB4444(const GLcontext * ctx,
				     GLuint n, GLint x, GLint y,
				     CONST GLubyte rgba[][4],
				     const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = PACK_ARGB4444(rgba[i][ACOMP], rgba[i][RCOMP],
				 rgba[i][GCOMP], rgba[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = PACK_ARGB4444(rgba[i][ACOMP], rgba[i][RCOMP],
			      rgba[i][GCOMP], rgba[i][BCOMP]);
      }
   }
}

/*
 * Write a span of RGB pixels to an ARGB4444 buffer
 */

static void write_rgb_span_ARGB4444(const GLcontext * ctx,
				    GLuint n, GLint x, GLint y,
				    CONST GLubyte rgb[][3],
				    const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = PACK_ARGB4444(255, rgb[i][RCOMP], rgb[i][GCOMP],
				 rgb[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = PACK_ARGB4444(255, rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP]);
      }
   }
}

/* 
 * Write a span of  mono RGBA pixels to an ARGB4444 buffer
 */

static void write_mono_rgba_span_ARGB4444(const GLcontext * ctx,
					  GLuint n, GLint x, GLint y,
					  const GLubyte color[4],
					  const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x, y);
   const UGL_UINT16 c = PACK_ARGB4444(color[ACOMP], color[RCOMP],
				      color[GCOMP], color[BCOMP]);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = c;
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = c;
      }
   }
}

/*
 * Write an array of RGBA pixels to an ARGB4444 buffer
 */

static void write_rgba_pixels_ARGB4444(const GLcontext * ctx, GLuint n,
				       const GLint x[], const GLint y[],
				       CONST GLubyte rgba[][4],
				       const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x[i], y[i]);
	    *p = PACK_ARGB4444(rgba[i][ACOMP], rgba[i][RCOMP],
			       rgba[i][GCOMP], rgba[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x[i], y[i]);
	 *p = PACK_ARGB4444(rgba[i][ACOMP], rgba[i][RCOMP],
			    rgba[i][GCOMP], rgba[i][BCOMP]);
      }
   }
}


/*
 * Write an array of monocolor RGBA pixels to an ARGB4444 buffer
 */

static void write_mono_rgba_pixels_ARGB4444(const GLcontext * ctx, GLuint n,
					    const GLint x[], const GLint y[],
					    const GLubyte color[4],
					    const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x[i], y[i]);
	    *p = PACK_ARGB4444(color[ACOMP], color[RCOMP], color[GCOMP],
			       color[BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x[i], y[i]);
	 *p = PACK_ARGB4444(color[ACOMP], color[RCOMP],
			    color[GCOMP], color[BCOMP]);
      }
   }
}

/*
 * Read a span of RGBA pixels from an ARGB4444 buffer
 */

static void read_rgba_span_ARGB4444(const GLcontext * ctx,
				    GLuint n, GLint x, GLint y,
				    GLubyte rgba[][4])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT16 *p = PIXELADDR2(umc->readBuffer, x, y);
   register GLuint i;

   for (i = 0; i < n; i++) {
      const UGL_UINT16 c = p[i];
      rgba[i][ACOMP] = (GLubyte) ((c >> 8) & 0xf0);
      rgba[i][RCOMP] = (GLubyte) ((c >> 4) & 0xf0);
      rgba[i][GCOMP] = (GLubyte) (c & 0xf0);
      rgba[i][BCOMP] = (GLubyte) ((c << 4) & 0xff);
   }
}

/*
 * Read an array of RGBA pixels from an ARGB4444 buffer
 */

static void read_rgba_pixels_ARGB4444(const GLcontext * ctx, GLuint n,
				      const GLint x[], const GLint y[],
				      GLubyte rgba[][4], const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   for (i = 0; i < n; i++) {
      if (mask[i]) {
	 const UGL_UINT16 *p = PIXELADDR2(umc->readBuffer, x[i], y[i]);
	 const UGL_UINT16 c = *p;
	 rgba[i][ACOMP] = (GLubyte) ((c >> 8) & 0xf0);
	 rgba[i][RCOMP] = (GLubyte) ((c >> 4) & 0xf0);
	 rgba[i][GCOMP] = (GLubyte) (c & 0xf0);
	 rgba[i][BCOMP] = (GLubyte) ((c << 4) & 0xff);
      }
   }
}

/*
 * RGB888
 */

/*
 * Write a span of RGBA pixels to an RGB888 buffer
 */

static void write_rgba_span_RGB888(const GLcontext * ctx,
				   GLuint n, GLint x, GLint y,
				   CONST GLubyte rgba[][4],
				   const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   _RGB_T *p = PIXELADDR3(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++, p++) {
	 if (mask[i]) {
	    p->r = rgba[i][RCOMP];
	    p->g = rgba[i][GCOMP];
	    p->b = rgba[i][BCOMP];
	 }
      }
   }
   else {
      for (i = 0; i < n; i++, p++) {
	 p->r = rgba[i][RCOMP];
	 p->g = rgba[i][GCOMP];
	 p->b = rgba[i][BCOMP];
      }
   }
}

/*
 * Write a span of RGB pixels to an RGB888 buffer.
 */

static void write_rgb_span_RGB888(const GLcontext * ctx,
				  GLuint n, GLint x, GLint y,
				  CONST GLubyte rgb[][3],
				  const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   _RGB_T *p = PIXELADDR3(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++, p++) {
	 if (mask[i]) {
	    p->r = rgb[i][RCOMP];
	    p->g = rgb[i][GCOMP];
	    p->b = rgb[i][BCOMP];
	 }
      }
   }
   else {
      for (i = 0; i < n; i++, p++) {
	 p->r = rgb[i][RCOMP];
	 p->g = rgb[i][GCOMP];
	 p->b = rgb[i][BCOMP];
      }
   }
}

/*
 * Write a span of RGBA pixels to an RGB888 buffer
 */

static void write_mono_rgba_span_RGB888(const GLcontext * ctx,
					GLuint n, GLint x, GLint y,
					const GLubyte color[4],
					const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   _RGB_T *p = PIXELADDR3(umc->drawBuffer, x, y);
   _RGB_T c;
   register GLuint i;

   c.r = color[RCOMP];
   c.g = color[GCOMP];
   c.b = color[BCOMP];

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = c;
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = c;
      }
   }
}

/*
 * Write an array of RGBA pixels to an RGB888 buffer
 */

static void write_rgba_pixels_RGB888(const GLcontext * ctx, GLuint n,
				     const GLint x[], const GLint y[],
				     CONST GLubyte rgba[][4],
				     const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    _RGB_T *p = PIXELADDR3(umc->drawBuffer, x[i], y[i]);
	    p->r = rgba[i][RCOMP];
	    p->g = rgba[i][GCOMP];
	    p->b = rgba[i][BCOMP];
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 _RGB_T *p = PIXELADDR3(umc->drawBuffer, x[i], y[i]);
	 p->r = rgba[i][RCOMP];
	 p->g = rgba[i][GCOMP];
	 p->b = rgba[i][BCOMP];
      }
   }
}

/*
 * Write an array of mono RGBA pixels to an RGB888 buffer
 */

static void write_mono_rgba_pixels_RGB888(const GLcontext * ctx, GLuint n,
					  const GLint x[], const GLint y[],
					  const GLubyte color[4],
					  const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   _RGB_T c;
   register GLuint i;

   c.r = color[RCOMP];
   c.g = color[GCOMP];
   c.b = color[BCOMP];

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    _RGB_T *p = PIXELADDR3(umc->drawBuffer, x[i], y[i]);
	    *p = c;
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 _RGB_T *p = PIXELADDR3(umc->drawBuffer, x[i], y[i]);
	 *p = c;
      }
   }
}

/*
 * Read a span of RGBA pixels from an RGB888 buffer
 */

static void read_rgba_span_RGB888(const GLcontext * ctx, GLuint n,
				  GLint x, GLint y, GLubyte rgba[][4])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   _RGB_T *p = PIXELADDR3(umc->readBuffer, x, y);
   register GLuint i;

   for (i = 0; i < n; i++, p++) {
      rgba[i][RCOMP] = p->r;
      rgba[i][GCOMP] = p->g;
      rgba[i][BCOMP] = p->b;
      rgba[i][ACOMP] = 255;
   }
}

/*
 * Read an array of RGBA pixels from an RGB888 buffer
 */

static void read_rgba_pixels_RGB888(const GLcontext * ctx, GLuint n,
				    const GLint x[], const GLint y[],
				    GLubyte rgba[][4],
				    const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   for (i = 0; i < n; i++) {
      if (mask[i]) {
	 const _RGB_T *p = PIXELADDR3(umc->readBuffer, x[i], y[i]);
	 rgba[i][RCOMP] = p->r;
	 rgba[i][GCOMP] = p->g;
	 rgba[i][BCOMP] = p->b;
	 rgba[i][ACOMP] = 255;
      }
   }
}

/*
 * RGB565
 */

/*
 * Write a span of RGBA pixels to an RGB565 buffer
 */
static void write_rgba_span_RGB565(const GLcontext * ctx,
				   GLuint n, GLint x, GLint y,
				   CONST GLubyte rgba[][4],
				   const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = PACK_RGB565(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = PACK_RGB565(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]);
      }
   }
}

/*
 * Write a span of RGB pixels to an RGB565 buffer
 */

static void write_rgb_span_RGB565(const GLcontext * ctx,
				  GLuint n, GLint x, GLint y, 
				  CONST GLubyte rgb[][3],
				  const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = PACK_RGB565(rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = PACK_RGB565(rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP]);
      }
   }
}

/* Write a span of mono RGBA pixels to an RGB565 buffer */

static void write_mono_rgba_span_RGB565(const GLcontext * ctx,
					GLuint n, GLint x, GLint y,
					const GLubyte color[4],
					const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x, y);
   const UGL_UINT16 c = PACK_RGB565(color[RCOMP], color[GCOMP], color[BCOMP]);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = c;
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = c;
      }
   }
}

/*
 * Write an array of RGBA pixels to an RGB565 buffer
 */

static void write_rgba_pixels_RGB565(const GLcontext * ctx, GLuint n,
				     const GLint x[], const GLint y[],
				     CONST GLubyte rgba[][4],
				     const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer,
				       x[i], y[i]);
	    *p = PACK_RGB565(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer,
				    x[i], y[i]);
	 *p = PACK_RGB565(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]);
      }
   }
}

/*
 * Write an array of mono RGBA pixels to an RGB565 buffer
 */

static void write_mono_rgba_pixels_RGB565 (const GLcontext * ctx, GLuint n,
					   const GLint x[], const GLint y[],
					   const GLubyte color[4],
					   const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const UGL_UINT16 c = PACK_RGB565(color[RCOMP], color[GCOMP], color[BCOMP]);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x[i], y[i]);
	    *p = c;
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x[i], y[i]);
	 *p = c;
      }
   }
}

/*
 * Read a span of RGBA pixels from an RGB565 buffer
 */

static void read_rgba_span_RGB565(const GLcontext * ctx, GLuint n,
				  GLint x, GLint y, GLubyte rgba[][4])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const UGL_UINT16 *p = PIXELADDR2(umc->readBuffer, x, y);
   register GLuint i;

   for (i = 0; i < n; i++) {
      const UGL_UINT16 c = p[i];
      rgba[i][RCOMP] = (GLubyte) ((c >> 8) & 0xf8);
      rgba[i][GCOMP] = (GLubyte) ((c >> 3) & 0xfc);
      rgba[i][BCOMP] = (GLubyte) ((c << 3) & 0xff);
      rgba[i][ACOMP] = 255;
   }
}

/*
 * Read an array of RGBA pixels from an RGB565 buffer
 */

static void read_rgba_pixels_RGB565(const GLcontext * ctx, GLuint n,
				    const GLint x[], const GLint y[],
				    GLubyte rgba[][4],
				    const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   for (i = 0; i < n; i++) {
      if (mask[i]) {
	 const UGL_UINT16 *p = PIXELADDR2(umc->readBuffer, x[i], y[i]);
	 const UGL_UINT16 c = *p;
	 rgba[i][RCOMP] = (GLubyte) ((c >> 8) & 0xf8);
	 rgba[i][GCOMP] = (GLubyte) ((c >> 3) & 0xfc);
	 rgba[i][BCOMP] = (GLubyte) ((c << 3) & 0xff);
	 rgba[i][ACOMP] = 255;
      }
   }
}

/*
 * RGB 16bit DITHERING
 */

/*
 * Write a span of RGBA pixels to an dithered RGB buffer
 */
static void write_rgba_span_DITHER16(const GLcontext * ctx,
				     GLuint n, GLint x, GLint y,
				     CONST GLubyte rgba[][4],
				     const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++, x++) {
	 if (mask[i]) {
	    PACK_DITHER16(p[i], x, y,
			  rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++, x++) {
	 PACK_DITHER16(p[i], x, y,
		       rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]);
      }
   }
}

/*
 * Write a span of RGB pixels to an dithered RGB buffer
 */

static void write_rgb_span_DITHER16(const GLcontext * ctx,
				    GLuint n, GLint x, GLint y, 
				    CONST GLubyte rgb[][3],
				    const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x, y);
   register GLuint i;   

   if (mask) {
      for (i = 0; i < n; i++, x++) {
	 if (mask[i]) {
	    PACK_DITHER16(p[i], x, y,
			    rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++, x++) {
	 PACK_DITHER16(p[i], x, y,
			 rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP]);
      }
   }
}

/* Write a span of mono RGBA pixels to an dithered RGB buffer */

static void write_mono_rgba_span_DITHER16(const GLcontext * ctx,
					  GLuint n, GLint x, GLint y,
					  const GLubyte color[4],
					  const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x, y);
   const GLint r = color[RCOMP], g = color[GCOMP], b = color[BCOMP];
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++, x++) {
	 if (mask[i]) {
	    PACK_DITHER16(p[i], x, y, r, g, b);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++, x++) {
	 PACK_DITHER16(p[i], x, y, r, g, b);
      }
   }
}

/*
 * Write an array of RGBA pixels to an dithered RGB buffer
 */

static void write_rgba_pixels_DITHER16(const GLcontext * ctx, GLuint n,
				       const GLint x[], const GLint y[],
				       CONST GLubyte rgba[][4],
				       const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer,
				       x[i], y[i]);
	    PACK_DITHER16(*p, x[i], y[i], rgba[i][RCOMP],
			    rgba[i][GCOMP], rgba[i][BCOMP]);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer,
				    x[i], y[i]);
	 PACK_DITHER16(*p, x[i], y[i], rgba[i][RCOMP],
			 rgba[i][GCOMP], rgba[i][BCOMP]);
      }
   }
}

/*
 * Write an array of mono RGBA pixels to an dithered RGB buffer
 */

static void write_mono_rgba_pixels_DITHER16(const GLcontext * ctx,
					    GLuint n,
					    const GLint x[],
					    const GLint y[],
					    const GLubyte color[4],
					    const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const int r = color[RCOMP], g = color[GCOMP], b = color[BCOMP];
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x[i], y[i]);
	    PACK_DITHER16(*p, x[i], y[i], r, g, b);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 UGL_UINT16 *p = PIXELADDR2(umc->drawBuffer, x[i], y[i]);
	 PACK_DITHER16(*p, x[i], y[i], r, g, b);
      }
   }
}

/*
 * Read a span of RGBA pixels from an dithered RGB buffer
 */

static void read_rgba_span_DITHER16(const GLcontext * ctx, GLuint n,
				    GLint x, GLint y, GLubyte rgba[][4])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const UGL_UINT16 *p = PIXELADDR2(umc->readBuffer, x, y);
   register GLuint i;
   for (i = 0; i < n; i++) {
      const UGL_UINT16 c = p[i];
      rgba[i][RCOMP] = umc->pixelToR[(c & umc->rMask) >> umc->rShift];
      rgba[i][GCOMP] = umc->pixelToG[(c & umc->gMask) >> umc->gShift];
      rgba[i][BCOMP] = umc->pixelToB[(c & umc->bMask) >> umc->bShift];
      rgba[i][ACOMP] = 255;
   }
}

/*
 * Read an array of RGBA pixels from an dithered RGB buffer
 */

static void read_rgba_pixels_DITHER16(const GLcontext * ctx, GLuint n,
				      const GLint x[], const GLint y[],
				      GLubyte rgba[][4],
				      const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   for (i = 0; i < n; i++) {
      if (mask[i]) {
	 const UGL_UINT16 *p = PIXELADDR2(umc->readBuffer, x[i], y[i]);
	 const UGL_UINT16 c = *p;
	 rgba[i][RCOMP] = umc->pixelToR[(c & umc->rMask) >> umc->rShift];
	 rgba[i][GCOMP] = umc->pixelToG[(c & umc->gMask) >> umc->gShift];
	 rgba[i][BCOMP] = umc->pixelToB[(c & umc->bMask) >> umc->bShift];
	 rgba[i][ACOMP] = 255;
      }
   }
}

/*
 * INDEXED COLORS
 */

/*
 * Write a span of 32-bit CI pixels to a CI buffer
 */

static void write_ci32_span_CI(const GLcontext * ctx,
			       GLuint n, GLint x, GLint y,
			       const GLuint index[],
			       const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT8 *p = PIXELADDR1(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = (UGL_UINT8) index[i];
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = (UGL_UINT8) index[i];
      }
   }
}


/* 
 * Write a span of 8-bit CI pixels to a CI buffer
 */

static void write_ci8_span_CI(const GLcontext * ctx,
			      GLuint n, GLint x, GLint y,
			      const GLubyte index[],
			      const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT8 *p = PIXELADDR1(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = (UGL_UINT8) index[i];
	 }
      }
   }
   else {
      MEMCPY(p, index, n * sizeof(UGL_UINT8));
   }
}

/*
 * Write a span of mono 8-bit CI pixels to a CI buffer
 */

static void write_mono_ci32_span_CI(const GLcontext * ctx,
				    GLuint n, GLint x, GLint y,
				    GLuint colorIndex, const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT8 *p = PIXELADDR1(umc->drawBuffer, x, y);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    p[i] = (UGL_UINT8) colorIndex;
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 p[i] = (UGL_UINT8) colorIndex;
      }
   }
}

/*
 * Write an array of 32-bit CI pixels to a CI buffer
 */

static void write_ci32_pixels_CI(const GLcontext * ctx, GLuint n,
				 const GLint x[], const GLint y[],
				 const GLuint index[], const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    UGL_UINT8 *p = PIXELADDR1(umc->drawBuffer, x[i], y[i]);
	    *p = (UGL_UINT8) index[i];
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 UGL_UINT8 *p = PIXELADDR1(umc->drawBuffer, x[i], y[i]);
	 *p = (UGL_UINT8) index[i];
      }
   }
}

/*
 * Write an array of mono 32-bit CI pixels to a CI buffer
 */

static void write_mono_ci32_pixels_CI(const GLcontext * ctx, GLuint n,
				      const GLint x[], const GLint y[],
				      GLuint colorIndex, const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    UGL_UINT8 *p = PIXELADDR1(umc->drawBuffer, x[i], y[i]);
	    *p = (UGL_UINT8) colorIndex;
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 GLubyte *p = PIXELADDR1(umc->drawBuffer, x[i], y[i]);
	 *p = (UGL_UINT8) colorIndex;
      }
   }
}

/*
 * Read a span of 32-bit CI pixels from a CI buffer
 */

static void read_ci32_span_CI(const GLcontext * ctx,
			      GLuint n, const GLint x, const GLint y,
			      GLuint index[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_UINT8 *p = PIXELADDR1(umc->readBuffer, x, y);
   register GLuint i;
 
   for (i = 0; i < n; i++) {
      index[i] = (GLuint) p[i];
   }
}

/*
 * Read an array of 32-bit CI pixels from a CI buffer
 */

static void read_ci32_pixels_CI(const GLcontext * ctx, GLuint n,
				const GLint x[], const GLint y[],
				GLuint index[], const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   for (i = 0; i < n; i++) {
      if (mask[i]) {
	 const UGL_UINT8 *p = PIXELADDR1(umc->readBuffer, x[i], y[i]);
	 index[i] = (GLuint) * p;
      }
   }
}

/*
 * WINDML
 */

/*
 * Write a span of RGBA pixels with WindML
 */
static void write_rgba_span_WINDML(const GLcontext * ctx,
				   GLuint n, GLint x, GLint y,
				   CONST GLubyte rgba[][4],
				   const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register i;
   UGL_ARGB argb_color[n];
   UGL_COLOR ugl_color[n];

   y = FLIP(y);

   for (i = 0; i < n; i++) {
      argb_color[i] = UGL_MAKE_ARGB(rgba[i][ACOMP], rgba[i][RCOMP],
				    rgba[i][GCOMP], rgba[i][BCOMP]);
   }
   uglColorAlloc(umc->devId, argb_color, UGL_NULL, ugl_color, n);

   if (mask) {
      for (i = 0; i < n; i++, x++) {
	 if (mask[i]) {
	    uglPixelSet(umc->gc, x, y, ugl_color[i]);
	 }
      }
   } else {
      for (i=0; i < n; i++, x++) {
	 uglPixelSet(umc->gc, x, y, ugl_color[i]);
      }
   }
}

/*
 * Write a span of RGB pixels with WindML
 */

static void write_rgb_span_WINDML(const GLcontext * ctx,
				  GLuint n, GLint x, GLint y,
				  CONST GLubyte rgb[][3],
				  const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register i;
   UGL_RGB rgb_color[n];
   UGL_COLOR ugl_color[n];

   y = FLIP(y);

   for (i = 0; i < n; i++) {
      rgb_color[i] = UGL_MAKE_RGB(rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP]);
   }
   uglColorAlloc(umc->devId, rgb_color, UGL_NULL, ugl_color, n);
   
   if (mask) {
      for (i = 0; i < n; i++, x++) {
	 if (mask[i]) {
	    uglPixelSet(umc->gc, x, y, ugl_color[i]);
	 }
      }
   } else {
      for (i = 0; i < n; i++, x++) {
	 uglPixelSet(umc->gc, x, y, ugl_color[i]);
      }
   }
}

/*
 * Write a span of mono RGBA pixels to an WindML buffer.
 */

static void write_mono_rgba_span_WINDML(const GLcontext * ctx,
					GLuint n, GLint x, GLint y,
					const GLubyte color[4],
					const GLubyte mask[])
{
   register GLuint i;
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_COLOR ugl_color;
   UGL_ARGB argb_color = UGL_MAKE_ARGB(color[ACOMP],
				       color[RCOMP],
				       color[GCOMP],
				       color[BCOMP]);

   uglColorAlloc(umc->devId, &argb_color, UGL_NULL, &ugl_color, 1);
   
   y = FLIP(y);
   
   if (mask) {
      for (i = 0; i < n; i++, x++) {
	 if (mask[i]) {
	    uglPixelSet(umc->gc, x, y, ugl_color);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++, x++) {
	 uglPixelSet(umc->gc, x, y, ugl_color);
      }
   }
}

/*
 * Write an array of RGBA pixels with WindML
 */

static void write_rgba_pixels_WINDML(const GLcontext * ctx, GLuint n,
				     const GLint x[], const GLint y[],
				     CONST GLubyte rgba[][4],
				     const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register i;
   UGL_ARGB argb_color[n];
   UGL_COLOR ugl_color[n];

   for (i = 0; i < n; i++) {
      argb_color[i] = UGL_MAKE_ARGB(rgba[i][ACOMP], rgba[i][RCOMP],
				    rgba[i][GCOMP], rgba[i][BCOMP]);
   }
   uglColorAlloc(umc->devId, argb_color, UGL_NULL, ugl_color, n);
   
   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    uglPixelSet(umc->gc, x[i], FLIP(y[i]), ugl_color[i]);
	 }
      }
   } else {
      for (i = 0; i < n; i++) {
	 uglPixelSet(umc->gc, x[i], FLIP(y[i]), ugl_color[i]);
      }
   }
}

/*
 * Write an array of monocolor RGBA pixels with WindML
 */

static void write_mono_rgba_pixels_WINDML(const GLcontext * ctx, GLuint n,
					  const GLint x[], const GLint y[],
					  const GLubyte color[4],
					  const GLubyte mask[])
{
   register GLuint i;
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_COLOR ugl_color;
   UGL_ARGB argb_color = UGL_MAKE_ARGB(color[ACOMP],
				       color[RCOMP],
				       color[GCOMP],
				       color[BCOMP]);
   uglColorAlloc(umc->devId, &argb_color, UGL_NULL, &ugl_color, 1);

   if (mask) {
      for (i = 0; i < n; i++) {
	 if (mask[i]) {
	    uglPixelSet(umc->gc, x[i], FLIP(y[i]), ugl_color);
	 }
      }
   }
   else {
      for (i = 0; i < n; i++) {
	 uglPixelSet(umc->gc, x[i], FLIP(y[i]), ugl_color);
      }
   }
}

/*
 * Read a span of RGBA pixels with WindML
 */

static void read_rgba_span_WINDML(const GLcontext * ctx,
				  GLuint n, GLint x, GLint y,
				  GLubyte rgba[][4])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   uglPageDrawSet(umc->devId, umc->readBuffer->pageId);
   y = FLIP(y);
   
   for (i = 0; i < n; i++, x++) {
      UGL_COLOR ugl_color;
      UGL_ARGB argb_color;
      uglPixelGet(umc->gc, x, y, &ugl_color);
      uglColorConvert(umc->devId, &ugl_color, UGL_DEVICE_COLOR_32,
		      &argb_color, UGL_ARGB8888, 1);
      rgba[i][ACOMP] = UGL_ARGB_ALPHA(argb_color);
      rgba[i][RCOMP] = UGL_ARGB_RED(argb_color);
      rgba[i][GCOMP] = UGL_ARGB_GREEN(argb_color);
      rgba[i][BCOMP] = UGL_ARGB_BLUE(argb_color);
   }
}

/*
 * Read an array of RGBA pixels with WindML
 */

static void read_rgba_pixels_WINDML(const GLcontext * ctx, GLuint n,
				    const GLint x[], const GLint y[],
				    GLubyte rgba[][4], const GLubyte mask[])
{
   const UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint i;

   uglPageDrawSet(umc->devId, umc->readBuffer->pageId);
   
   for (i = 0; i < n; i++) {
      if (mask[i]) {
	 UGL_COLOR ugl_color;
	 UGL_ARGB argb_color;
	 uglPixelGet(umc->gc, x[i], FLIP(y[i]), &ugl_color);
	 uglColorConvert(umc->devId, &ugl_color, UGL_DEVICE_COLOR_32,
			 &argb_color, UGL_ARGB8888, 1);
	 rgba[i][ACOMP] = UGL_ARGB_ALPHA(argb_color);
	 rgba[i][RCOMP] = UGL_ARGB_RED(argb_color);
	 rgba[i][GCOMP] = UGL_ARGB_GREEN(argb_color);
	 rgba[i][BCOMP] = UGL_ARGB_BLUE(argb_color);
      }
   }
}

/*
 * uglmesa_update_span_funcs
 */

void uglmesa_update_span_funcs(GLcontext * ctx)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   struct swrast_device_driver *swdd = _swrast_GetDeviceDriverReference(ctx);

   ASSERT((void *) umc == (void *) ctx->DriverCtx);

   switch (umc->pixelFormat) {
   case UGL_MESA_ARGB8888:
      swdd->WriteRGBASpan = write_rgba_span_ARGB8888;
      swdd->WriteRGBSpan = write_rgb_span_ARGB8888;
      swdd->WriteMonoRGBASpan = write_mono_rgba_span_ARGB8888;
      swdd->WriteRGBAPixels = write_rgba_pixels_ARGB8888;
      swdd->WriteMonoRGBAPixels = write_mono_rgba_pixels_ARGB8888;
      swdd->ReadRGBASpan = read_rgba_span_ARGB8888;
      swdd->ReadRGBAPixels = read_rgba_pixels_ARGB8888;
      break;
   case UGL_MESA_ARGB4444:
      swdd->WriteRGBASpan = write_rgba_span_ARGB4444;
      swdd->WriteRGBSpan = write_rgb_span_ARGB4444;
      swdd->WriteMonoRGBASpan = write_mono_rgba_span_ARGB4444;
      swdd->WriteRGBAPixels = write_rgba_pixels_ARGB4444;
      swdd->WriteMonoRGBAPixels = write_mono_rgba_pixels_ARGB4444;
      swdd->ReadRGBASpan = read_rgba_span_ARGB4444;
      swdd->ReadRGBAPixels = read_rgba_pixels_ARGB4444;
      break;
   case UGL_MESA_RGB888:
      swdd->WriteRGBASpan = write_rgba_span_RGB888;
      swdd->WriteRGBSpan = write_rgb_span_RGB888;
      swdd->WriteMonoRGBASpan = write_mono_rgba_span_RGB888;
      swdd->WriteRGBAPixels = write_rgba_pixels_RGB888;
      swdd->WriteMonoRGBAPixels = write_mono_rgba_pixels_RGB888;
      swdd->ReadRGBASpan = read_rgba_span_RGB888;
      swdd->ReadRGBAPixels = read_rgba_pixels_RGB888;
      break;
   case UGL_MESA_RGB565:
      swdd->WriteRGBASpan = write_rgba_span_RGB565;
      swdd->WriteRGBSpan = write_rgb_span_RGB565;
      swdd->WriteMonoRGBASpan = write_mono_rgba_span_RGB565;
      swdd->WriteRGBAPixels = write_rgba_pixels_RGB565;
      swdd->WriteMonoRGBAPixels = write_mono_rgba_pixels_RGB565;
      swdd->ReadRGBASpan = read_rgba_span_RGB565;
      swdd->ReadRGBAPixels = read_rgba_pixels_RGB565;
   case UGL_MESA_DITHER16:
      swdd->WriteRGBASpan = write_rgba_span_DITHER16;
      swdd->WriteRGBSpan = write_rgb_span_DITHER16;
      swdd->WriteMonoRGBASpan = write_mono_rgba_span_DITHER16;
      swdd->WriteRGBAPixels = write_rgba_pixels_DITHER16;
      swdd->WriteMonoRGBAPixels = write_mono_rgba_pixels_DITHER16;
      swdd->ReadRGBASpan = read_rgba_span_DITHER16;
      swdd->ReadRGBAPixels = read_rgba_pixels_DITHER16;
      break;
   case UGL_MESA_CI:
      /* CI span/pixel functions */
      swdd->WriteCI32Span = write_ci32_span_CI;
      swdd->WriteCI8Span = write_ci8_span_CI;
      swdd->WriteMonoCISpan = write_mono_ci32_span_CI;
      swdd->WriteCI32Pixels = write_ci32_pixels_CI;
      swdd->WriteMonoCIPixels = write_mono_ci32_pixels_CI;
      swdd->ReadCI32Span = read_ci32_span_CI;
      swdd->ReadCI32Pixels = read_ci32_pixels_CI;
      break;
   case UGL_MESA_WINDML:
      swdd->WriteRGBASpan = write_rgba_span_WINDML;
      swdd->WriteRGBSpan = write_rgb_span_WINDML;
      swdd->WriteMonoRGBASpan = write_mono_rgba_span_WINDML;
      swdd->WriteRGBAPixels = write_rgba_pixels_WINDML;
      swdd->WriteMonoRGBAPixels = write_mono_rgba_pixels_WINDML;
      swdd->ReadRGBASpan = read_rgba_span_WINDML;
      swdd->ReadRGBAPixels = read_rgba_pixels_WINDML;
      break;
   }

   swdd->SetBuffer = uglmesa_set_buffer;
}
