/* ugl_dd.c - UGL/Mesa Device Driver */

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

#include "glheader.h"
#include "uglmesaP.h"
#include <ugl/ugllog.h>
#include <ugl/driver/graphics/generic/udgen.h>
#include "context.h"
#include "extensions.h"
#include "macros.h"
#include "matrix.h"
#include "imports.h"
#include "mmath.h"
#include "mtypes.h"
#include "texstore.h"
#include "texformat.h"
#include "swrast/swrast.h"
#include "array_cache/acache.h"
#include "swrast_setup/swrast_setup.h"
#include "tnl/tnl.h"
#include "tnl/t_context.h"
#include "tnl/t_pipeline.h"

/*
 * Returns some informations about renderer and vendor
 */

static const GLubyte *get_string(GLcontext * ctx, GLenum name)
{
   (void) ctx;
   switch (name) {
      case GL_RENDERER:
	 return (const GLubyte *) "Mesa/WindML";
      case GL_VENDOR:
	 return (const GLubyte *) "Wind River Systems, Inc";
      default:
	 return NULL;
   }
}

/*
 * Returns width and height
 */

static void get_buffer_size(GLframebuffer *buffer, GLuint * width, GLuint * height)
{
   /*
    * XXX this isn't quite right.  We should get the size of the WindML
    * window associated with buffer.  This function should work whether or
    * not there is a current context.
    */
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   (*width) = umc->width;
   (*height) = umc->height;
}

/*
 * Set the FRONT or the BACK buffer for reading/reading.
 */
void uglmesa_set_buffer
(GLcontext * ctx, GLframebuffer * buffer, GLuint bufferBit)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   /* Only one GLframebuffer in my driver */

   if (umc->dbFlag) {
      if (bufferBit == BACK_LEFT_BIT) {
	 umc->readBuffer = (umc->visFirstPage) ?
	     umc->secondPage : umc->firstPage;
	 umc->drawBuffer = (umc->visFirstPage) ?
	     umc->secondPage : umc->firstPage;
      }
      else if (bufferBit == FRONT_LEFT_BIT) {
	 umc->readBuffer = (umc->visFirstPage) ?
	     umc->firstPage : umc->secondPage;
	 umc->drawBuffer = (umc->visFirstPage) ?
	     umc->firstPage : umc->secondPage;
      }
      if (umc->windMLFlag)
	 uglPageDrawSet(umc->devId, umc->drawBuffer->pageId);
   }
   /* else readBuffer is still unchanged (single buffer)
    * It doesn't support FRONT and BACK at the same time.
    */
}

/*
 * Set the clear color in color indexed mode 
 */

static void clear_index(GLcontext * ctx, GLuint index)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   umc->clearPixel = index;
}

/*
 * Set the clear color in RGB mode
 */

/* Are params between zero and one ? (color * 2^n) */
static void clear_color(GLcontext * ctx, const GLfloat color[4])
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_ARGB argb_color;
   GLubyte col[4];
   CLAMPED_FLOAT_TO_UBYTE(col[0], color[0]);
   CLAMPED_FLOAT_TO_UBYTE(col[1], color[1]);
   CLAMPED_FLOAT_TO_UBYTE(col[2], color[2]);
   CLAMPED_FLOAT_TO_UBYTE(col[3], color[3]);

   argb_color = UGL_MAKE_ARGB(col[3], col[0], col[1], col[2]);
   uglColorAlloc(umc->devId, &argb_color, UGL_NULL, &umc->clearPixel, 1);

   umc->clearColor[0] = col[0];
   umc->clearColor[1] = col[1];
   umc->clearColor[2] = col[2];
   umc->clearColor[3] = col[3];
}

/*
 * clear a 32bit buffer
 */

static void clear_32bit(UGL_MESA_PAGE *pPage,
			GLcontext * ctx, GLboolean all,
			GLint x, GLint y, GLint width, GLint height)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint pixel = (GLuint) umc->clearPixel;
   
   /* UGL_BIG_ENDIAN = swap bytes !! */
#ifdef UGL_BIG_ENDIAN
   pixel = lswapl(pixel);
#endif
   
   if (all) {
      /* Clear all the OpenGL surface */
      register GLint n = umc->width * umc->height;
      
      if (pixel == 0) {
	 if (umc->fullScreen) {
	    UGL_UINT32 *p = (UGL_UINT32 *)pPage->buffer;
	    MEMSET(p, pixel, umc->bufferSize);
	 }
	 else {
	    /* full window */
	    register int j;
	    for (j = 0; j < height; j++) {
	       register UGL_UINT32 *p = PIXELADDR4(pPage, 0, j);
	       MEMSET(p, pixel, umc->rowLength);
	    }	    
	 }
      } /* pixel == 0 */
      else {
	 if (umc->fullScreen) {
	    register UGL_UINT32 *p = (UGL_UINT32 *)pPage->buffer;
	    do {
	       *p++ = pixel;
	       n--;
	    }
	    while (n != 0);
	 }
	 else {
	    /* full window */
	    register int i,j;
	    for (j = 0; j < height; j++) {
	       register UGL_UINT32 *p = PIXELADDR4(pPage, 0, j);
	       for (i = 0; i < width; i++) {
		  *p++ = pixel;
	       }
	    }
	 }
      }
   } /* ALL */
   else {
      register int i, j;
      
      for (j = 0; j < height; j++) {
         register UGL_UINT32 *p = PIXELADDR4(pPage, x, y + j);
	 for (i = 0; i < width; i++) {
	    *p++ = pixel;
	 }
      }
   }
}

/*
 * clear a 24bit buffer with 8 bits by component
 */

static void clear_24bit (UGL_MESA_PAGE *pPage,
			 GLcontext * ctx, GLboolean all,
			 GLint x, GLint y, GLint width, GLint height)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLubyte r = umc->clearColor[0];
   const GLubyte g = umc->clearColor[1];
   const GLubyte b = umc->clearColor[2];
   register GLuint pixel;
 
   pixel = PACK_RGB888(r, g, b);

   if (all) {
      if (r == g && g == b) {
	 /* Same value for all three components (gray) */
	 const GLsizei h = umc->height;
	 GLint i;

	 if (umc->fullScreen) {
	    MEMSET(pPage->buffer, r, umc->bufferSize);
	 }
	 else {
	    for (i = 0; i < h; i++) {
	       _RGB_T *p = PIXELADDR3(pPage, 0, i);
	       MEMSET(p, r, umc->rowLength);
	    }
	 }
      }
      else {
	 /* the usual case */
	 const GLsizei w = umc->width;
	 const GLsizei h = umc->height;
	 register GLint i, j;

	 for (i = 0; i < h; i++) {
	    _RGB_T *p = PIXELADDR3(pPage, 0, i);
	    for (j = 0; j < w; j++) {
	       p->r = r;
	       p->g = g;
	       p->b = b;
	       p++;
	    }
	 }
      }
   } /* ALL */
   else {
      /* only clear subrectangle of color buffer */
      if (r == g && g == b) {
	 /* same value for all three component (gray) */
	 GLint j;
	 for (j = 0; j < height; j++) {
	    _RGB_T *p = PIXELADDR3(pPage, x, y + j);
	    MEMSET(p, r, 3 * width);
	 }
      }
      else {
	 /* non-gray clear color */
	 register GLint i, j;
	 for (j = 0; j < height; j++) {
	    _RGB_T *p = PIXELADDR3(pPage, x, y + j);
	    for (i = 0; i < width; i++) {
	       p->r = r;
	       p->g = g;
	       p->b = b;
	       p++;
	    }
	 }
      }
   }
}

/*
 * clear a 16bit buffer, works for ARGB4444 and RGB565
 */

static void clear_16bit(UGL_MESA_PAGE *pPage,
			GLcontext * ctx, GLboolean all,
			GLint x, GLint y, GLint width, GLint height)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   register GLuint pixel = (GLuint) umc->clearPixel;
   
#ifdef UGL_BIG_ENDIAN
   pixel = lswaps(pixel);
#endif

   if (all) {
      if (umc->fullScreen) {
         /* Erase all the screen */
	 if ((pixel & 0xff) == ((pixel >> 8) & 0xff)) {
         /* low and high bytes are equal so use memset() */
	    MEMSET(pPage->buffer, pixel & 0xff, umc->bufferSize);
	 }
	 else {
	    register GLuint n;
	    register UGL_UINT32 *p = (UGL_UINT32 *)pPage->buffer;

	    pixel = pixel | (pixel << 16);
	    
	    /* Four bytes at once */
	    n = umc->bufferSize / 4;
	    do {
	       *p++ = pixel;
	       n--;
	    }
	    while (n != 0);
	    
	    /* Two last ? */
	    if (umc->bufferSize & 0x2)
	       *(UGL_UINT16 *) p = pixel & 0xffff;
	 }
      }
      else {
	 /* Full window */
	 if ((pixel & 0xff) == ((pixel >> 8) & 0xff)) {
	    /* low and high bytes are equal so use memset() */
	    register GLint j;
	    for (j = 0; j < height; j++) {
	       UGL_UINT16 *p = PIXELADDR2(pPage, 0, j);
	       MEMSET(p, pixel & 0xff, umc->rowLength);
	    }
	 }
	 else {
	    register int i, j;
	    for (j = 0; j < height; j++) {
	       register UGL_UINT16 *p = PIXELADDR2(pPage, 0, j);
	       for (i = 0; i < width; i++) {
		  *p++ = pixel;
	       }
	    }
	 }
      }
   } /* ALL */
   else {
      /* Erase pixel by pixel */
      register int i, j;
      
      for (j = 0; j < height; j++) {
	 register UGL_UINT16 *p = PIXELADDR2(pPage, x, y + j);
	 for (i = 0; i < width; i++) {
	    *p++ = pixel;
	 }
      }
   }
}

/*
 * clear a 8 bit buffer, color indexed
 */

static void clear_8bit(UGL_MESA_PAGE *pPage,
		       GLcontext * ctx, GLboolean all,
		       GLint x, GLint y, GLint width, GLint height)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   
   if (all) {
      if (umc->fullScreen) {
	 MEMSET(pPage->buffer, (UGL_UINT8) umc->clearPixel, umc->bufferSize);
      }
      else {
	 register int i;
	 for (i = 0; i < height; i++) {
	    register UGL_UINT8 *p = PIXELADDR1(pPage, 0, i);
	    MEMSET(p, (UGL_UINT8) umc->clearPixel, umc->rowLength);
	 }
      }
   } /* ALL */
   else {
      register int i;
      for (i = 0; i < height; i++) {
	 register UGL_UINT8 *p = PIXELADDR1(pPage, x, y + i);
	 MEMSET(p, (UGL_UINT8) umc->clearPixel, width);
      }
   }
}

/*
 * clear a color buffer with WindML
 */

static void clear_windml(UGL_MESA_PAGE *pPage,
			 GLcontext * ctx, GLboolean all,
			 GLint x, GLint y, GLint width, GLint height)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   uglBackgroundColorSet(umc->gc, umc->clearPixel);
   uglForegroundColorSet(umc->gc, umc->clearPixel);
   uglLineStyleSet(umc->gc, UGL_LINE_STYLE_SOLID);
   uglLineWidthSet(umc->gc, 1);
   uglRectangle(umc->gc, x, y, x+width-1, x+height-1);
}

/*
 * Clear all buffers color, depth, stencil and accums.  Only the color
 * buffer is cleared (if it doesn't use a mask) in this file.
 */

static void clear(GLcontext * ctx,
		  GLbitfield mask, GLboolean all,
		  GLint x, GLint y, GLint width, GLint height)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   const GLuint *colorMask = (GLuint *) & ctx->Color.ColorMask;

   if (*colorMask == 0xffffffff && ctx->Color.IndexMask == 0xffffffff) {

      if (mask & DD_FRONT_LEFT_BIT) {
	 /* Is it only glDrawBuffer which change the mask ? */
	 /* set_buffer call may be useless (redundant) ? */
	 set_buffer(ctx, GL_FRONT_LEFT); 
	 (*umc->clearFunc) (umc->drawBuffer, ctx, all, x, y, width, height);
	 mask &= ~DD_FRONT_LEFT_BIT;
      }

      if (mask & DD_BACK_LEFT_BIT) {
	 set_buffer(ctx, GL_BACK_LEFT);
	 (*umc->clearFunc) (umc->drawBuffer, ctx, all, x, y, width, height);
	 mask &= ~DD_BACK_LEFT_BIT;
      }
   }

   if (mask)
      _swrast_Clear(ctx, mask, all, x, y, width, height);
}

/*
 * Enable/disable dithering
 */
static void enable(GLcontext *ctx, GLenum pname, GLboolean state)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   switch (pname) {
      case GL_DITHER:
	 if (state)
	    umc->pixelFormat = umc->dithered;
	 else
	    umc->pixelFormat = umc->undithered;
	 break;
      default:
	 ;  /* silence compiler warning */
   }
}

/*
 * Bind the right clear functions to the context
 */

void uglmesa_update_state(GLcontext * ctx, GLuint new_state)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   _swrast_InvalidateState(ctx, new_state);
   _swsetup_InvalidateState(ctx, new_state);
   _ac_InvalidateState(ctx, new_state);
   _tnl_InvalidateState(ctx, new_state);

   if (umc->windMLFlag)
      umc->clearFunc = clear_windml;
   else {
      switch (umc->bitsPerPixel) {
	 case 32:
	    umc->clearFunc = clear_32bit;
	    break;
	 case 24:
	    umc->clearFunc = clear_24bit;
	    break;
	 case 16:
	    umc->clearFunc = clear_16bit;
	    break;
	 case 8:
	    umc->clearFunc = clear_8bit;
	    break;
      }
   }

   uglmesa_update_span_funcs(ctx);
   uglmesa_update_swap_funcs(ctx);
}

void uglmesa_init_pointers(GLcontext * ctx)
{

   TNLcontext *tnl;

/*
 * XXX these function pointers could be initialized just once during
 * context creation since they don't depend on any state changes.
 */

   ctx->Driver.GetString = get_string;
   ctx->Driver.UpdateState = uglmesa_update_state;
   ctx->Driver.GetBufferSize = get_buffer_size;

   /* Software rasterizer pixel paths:
    */
   ctx->Driver.Accum = _swrast_Accum;
   ctx->Driver.Bitmap = _swrast_Bitmap;
   ctx->Driver.Clear = clear;
   
   /* Study behavior
    */
   ctx->Driver.ResizeBuffers = _swrast_alloc_buffers;
   ctx->Driver.CopyPixels = _swrast_CopyPixels;
   ctx->Driver.DrawPixels = _swrast_DrawPixels;
   ctx->Driver.ReadPixels = _swrast_ReadPixels;
   ctx->Driver.DrawBuffer = _swrast_DrawBuffer;

   /* Software texture functions:
    */
   ctx->Driver.ChooseTextureFormat = _mesa_choose_tex_format;
   ctx->Driver.TexImage1D = _mesa_store_teximage1d;
   ctx->Driver.TexImage2D = _mesa_store_teximage2d;
   ctx->Driver.TexImage3D = _mesa_store_teximage3d;
   ctx->Driver.TexSubImage1D = _mesa_store_texsubimage1d;
   ctx->Driver.TexSubImage2D = _mesa_store_texsubimage2d;
   ctx->Driver.TexSubImage3D = _mesa_store_texsubimage3d;
   ctx->Driver.TestProxyTexImage = _mesa_test_proxy_teximage;

   ctx->Driver.CompressedTexImage1D = _mesa_store_compressed_teximage1d;
   ctx->Driver.CompressedTexImage2D = _mesa_store_compressed_teximage2d;
   ctx->Driver.CompressedTexImage3D = _mesa_store_compressed_teximage3d;
   ctx->Driver.CompressedTexSubImage1D = _mesa_store_compressed_texsubimage1d;
   ctx->Driver.CompressedTexSubImage2D = _mesa_store_compressed_texsubimage2d;
   ctx->Driver.CompressedTexSubImage3D = _mesa_store_compressed_texsubimage3d;

   ctx->Driver.CopyTexImage1D = _swrast_copy_teximage1d;
   ctx->Driver.CopyTexImage2D = _swrast_copy_teximage2d;
   ctx->Driver.CopyTexSubImage1D = _swrast_copy_texsubimage1d;
   ctx->Driver.CopyTexSubImage2D = _swrast_copy_texsubimage2d;
   ctx->Driver.CopyTexSubImage3D = _swrast_copy_texsubimage3d;

   /* Swrast hooks for imaging extensions:
    */
   ctx->Driver.CopyColorTable = _swrast_CopyColorTable;
   ctx->Driver.CopyColorSubTable = _swrast_CopyColorSubTable;
   ctx->Driver.CopyConvolutionFilter1D = _swrast_CopyConvolutionFilter1D;
   ctx->Driver.CopyConvolutionFilter2D = _swrast_CopyConvolutionFilter2D;

   /* Statechange callbacks:
    */
   ctx->Driver.ClearIndex = clear_index;
   ctx->Driver.ClearColor = clear_color;
/*
  ctx->Driver.IndexMask = index_mask;
  ctx->Driver.ColorMask = color_mask;
*/
   /* Dithering */
   ctx->Driver.Enable = enable;
   
   /* Initialize the TNL driver interface:
    */
   tnl = TNL_CONTEXT(ctx);
   tnl->Driver.RunPipeline = _tnl_run_pipeline;
   
   /* Install swsetup for tnl->Driver.Render.*:
    */
   _swsetup_Wakeup(ctx);

}
