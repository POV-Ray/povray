/* uglmesa.c - UGL/Mesa */

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


/* - index/color mask
 *
 * MEMORY MANAGEMENT:
 * - src/imports.h -> it allows to redefine memory management functions by UGL.
 * Add #ifdef UGL like for SUNOS or BSD
 * - include/internal/glcore.h allow to redefine the OS dependencies in the
 * structure __GLimports
 *
 * CONVENTIONS:
 * ------------
 * Mesa coding style
 * - public API: uglMesa...
 * - extern internal: uglmesa_name_func
 * - internal: name_func
 */

/*
 * includes
 */

#include "glheader.h"
#include "GL/uglmesa.h"
#include "uglmesaP.h"
#include <taskVarLib.h>
#include <ugl/driver/graphics/generic/udgen.h>
#include <ugl/ugllog.h>
#include "context.h"
#include "extensions.h"
#include "imports.h"
#include "macros.h"
#include "matrix.h"
#include "imports.h"
#include "mmath.h"
#include "mtypes.h"
#include "swrast/swrast.h"
#include "array_cache/acache.h"
#include "swrast_setup/swrast_setup.h"
#include "tnl/tnl.h"

/**********************************************************************/
/*****                   Misc Private Functions                   *****/
/**********************************************************************/

/*
 * Return number of bits set in n.
 */
static int bit_count(unsigned long n)
{
   int bits;
   for (bits=0; n>0; n=n>>1) {
      if (n&1) {
         bits++;
      }
   }
   return bits;
}

/*
 * Apply gamma correction to an intensity value in [0..max].  Return the
 * new intensity value.
 */
static GLint gamma_adjust(GLfloat gamma, GLint value, GLint max)
{
   if (gamma == 1.0) {
      return value;
   }
   else {
      double x = (double) value / (double) max;
      return IROUND_POS((GLfloat) max * pow(x, 1.0F/gamma));
   }
}

/*
 * Software allocation in double buffer mode
 */
static void alloc_sw_page(UGL_MESA_CONTEXT umc)
{
   FREE(umc->secondPage->buffer);
   
   /* back buffer allocation */
   umc->secondPage->buffer = (GLubyte *) MALLOC(umc->bufferSize);
  
   if (!umc->secondPage->buffer) {
      /* Error */	 
      uglGcDestroy(umc->gc);
	 
      _swsetup_DestroyContext(umc->glCtx);
      _tnl_DestroyContext(umc->glCtx);
      _ac_DestroyContext(umc->glCtx);
      _swrast_DestroyContext(umc->glCtx);
	 
      _mesa_destroy_visual(umc->glVisual);
      _mesa_destroy_framebuffer(umc->glBuffer);
      _mesa_destroy_context(umc->glCtx);
      
      FREE(umc->firstPage->rowAddr);
      FREE(umc->firstPage);
      
      if (umc->dbSwFlag) {
	 FREE(umc->secondPage->buffer);
	 FREE(umc->secondPage->rowAddr);
	 FREE(umc->secondPage);
      }
      
      FREE(umc);

      uglLog(UGL_ERR_TYPE_FATAL,
	     "Not enough RAM for software double buffer mode",
	     0, 0, 0, 0, 0);
   }
   
   /* OpenGL conformance ?!? */
   if (umc->drawBuffer != umc->firstPage)
      umc->drawBuffer = umc->secondPage;
   if (umc->readBuffer != umc->firstPage)
      umc->readBuffer = umc->secondPage;
}

/**********************************************************************/
/*****                    Public Functions                        *****/
/**********************************************************************/

UGL_MESA_CONTEXT uglMesaCreateNewContext(GLenum mode,
					 UGL_MESA_CONTEXT share_list)
{
   return uglMesaCreateNewContextExt(mode, DEFAULT_SOFTWARE_DEPTH_BITS,
				     0, 0, 0, 0, 0, share_list);
}

UGL_MESA_CONTEXT uglMesaCreateNewContextExt(GLenum mode,
					    GLint depth_bits,
					    GLint stencil_bits,
					    GLint accum_red_bits,
					    GLint accum_green_bits,
					    GLint accum_blue_bits,
					    GLint accum_alpha_bits,
					    UGL_MESA_CONTEXT share_list)
{
   UGL_MESA_CONTEXT umc;	/* UGL/Mesa context */
   UGL_MODE_INFO mode_info;	/* display mode information */
   UGL_FB_INFO fb_info;		/* framebuffer information */

   int i;
   GLint index_bits, red_bits, green_bits, blue_bits, alpha_bits, max_bits;
   UGL_UINT32 red_mask, green_mask, blue_mask;
   GLboolean rgb_mode = GL_FALSE;
   GLboolean sw_alpha = GL_FALSE;
   GLboolean db_mode = mode & (~UGL_MESA_WINDML_EXCLUSIVE);
   GLubyte tmp_kernel[16] = {
       15*16,  7*16, 13*16,  5*16,
       3*16,  11*16,  1*16,  9*16,
       12*16,  4*16, 14*16,  6*16,
       0*16,   8*16,  2*16, 10*16,
   };

   /* UGL_MESA_CONTEXT allocation */
   umc = (UGL_MESA_CONTEXT) MALLOC_STRUCT(uglMesaContext);
   
   /* Get device Id */
   if (uglDriverFind(UGL_DISPLAY_TYPE, 0,
		     (UGL_UINT32 *) & umc->devId) == UGL_STATUS_ERROR) {
      FREE(umc);
      uglLog(UGL_ERROR_NO_GRAPHICS,
	     "uglMesaCreateNewContext: Error uglDriverFind()", 0, 0, 0, 0, 0);
      return NULL;
   }

   /* Obtain informations on the display */
   uglInfo(umc->devId, UGL_MODE_INFO_REQ, &mode_info);

   /* Get some infos about the framebuffer */ 
   uglInfo(umc->devId, UGL_FB_INFO_REQ, &fb_info);
   
   /* Initialization to zero */
   index_bits = red_bits = green_bits = blue_bits = alpha_bits = 0;
   umc->rMask = umc->gMask = umc->bMask = umc->aMask = 0;
   umc->clutSize = 0;
   umc->rGamma = umc->gGamma = umc->bGamma = 1.0;
   
   /* Analyze color mode */
   if (mode_info.colorModel == UGL_DIRECT) {
      switch (mode_info.colorFormat) {
	 case UGL_ARGB8888:
	    umc->aMask = 0xFF000000;
	    umc->rMask = 0x00FF0000;
	    umc->gMask = 0x0000FF00;
	    umc->bMask = 0x000000FF;
	    rgb_mode = GL_TRUE;
	    sw_alpha = GL_FALSE;
	    umc->dithered = umc->undithered = UGL_MESA_ARGB8888;
	    break;
	 case UGL_ARGB4444:
	    umc->aMask = 0xF000;
	    umc->rMask = 0x0F00;
	    umc->gMask = 0x00F0;
	    umc->bMask = 0x000F;
	    rgb_mode = GL_TRUE;
	    sw_alpha = GL_FALSE;
	    umc->dithered = UGL_MESA_DITHER16;
	    umc->undithered = UGL_MESA_ARGB4444;
	    break;
	 case UGL_RGB888:
	    umc->rMask = 0xFF0000;
	    umc->gMask = 0x00FF00;
	    umc->bMask = 0x0000FF;
	    umc->aMask = 0x0;
	    rgb_mode = GL_TRUE;
	    sw_alpha = GL_TRUE;
	    umc->dithered = umc->undithered = UGL_MESA_RGB888;
	    break;
	 case UGL_RGB565:
	    umc->rMask = 0xF800;
	    umc->gMask = 0x07E0;
	    umc->bMask = 0x001F;
	    umc->aMask = 0x0;
	    rgb_mode = GL_TRUE;
	    sw_alpha = GL_FALSE;
	    umc->dithered = UGL_MESA_DITHER16;
	    umc->undithered = UGL_MESA_RGB565;
	    break;
	 case UGL_DEVICE_COLOR:
	    FREE(umc);
	    uglLog(UGL_ERR_TYPE_FATAL,
		   "UGL_DEVICE_COLOR: Not supported", 0, 0, 0, 0, 0);
	    return NULL;
	    break;
	 case UGL_CLUT_COLOR:
	    FREE(umc);
	    uglLog(UGL_ERR_TYPE_FATAL,
		   "UGL_CLUT_COLOR: Not supported", 0, 0, 0, 0, 0);
	    return NULL;
	    break;
	 case UGL_YUV422:
	    FREE(umc);
	    uglLog(UGL_ERR_TYPE_FATAL,
		   "UGL_YUV422: Not supported", 0, 0, 0, 0, 0);
	    return NULL;
	    break;
      }	/* END switch(umc->colorFormat) */

      /* Compute red multiplier (mask) and bit shift */
      umc->rShift = 0;
      red_mask = umc->rMask;
      while ((red_mask & 1)==0) {
	 umc->rShift++;
	 red_mask = red_mask >> 1;
      }
      red_bits = bit_count(red_mask);
      
      /* Compute green multiplier (mask) and bit shift */
      umc->gShift = 0;
      green_mask = umc->gMask;
      while ((green_mask & 1)==0) {
	 umc->gShift++;
	 green_mask = green_mask >> 1;
      }
      green_bits = bit_count(green_mask);
      
      /* Compute blue multiplier (mask) and bit shift */
      umc->bShift = 0;
      blue_mask = umc->bMask;
      while ((blue_mask & 1)==0) {
	 umc->bShift++;
	 blue_mask = blue_mask >> 1;
      }
      blue_bits = bit_count(blue_mask);

      alpha_bits = bit_count(umc->aMask);

      umc->bitsPerPixel = red_bits + green_bits + blue_bits + alpha_bits;

      /* convert pixel components in [0,_mask] to RGB values in [0,255] */
      for (i=0; i<=red_mask; i++)
	 umc->pixelToR[i] = (unsigned char) ((i * 255) / red_mask);
      for (i=0; i<=green_mask; i++)
	 umc->pixelToG[i] = (unsigned char) ((i * 255) / green_mask);
      for (i=0; i<=blue_mask; i++)
	 umc->pixelToB[i] = (unsigned char) ((i * 255) / blue_mask);
	 
      for (i=0;i<256;i++) {
	 GLint r = gamma_adjust(umc->rGamma,   i, 255);
	 GLint g = gamma_adjust(umc->gGamma, i, 255);
	 GLint b = gamma_adjust(umc->bGamma,  i, 255);
	 umc->rToPixel[i] = (r >> (8-red_bits)) << umc->rShift;
	 umc->gToPixel[i] = (g >> (8-green_bits)) << umc->gShift;
	 umc->bToPixel[i] = (b >> (8-blue_bits)) << umc->bShift;
      }
      /* overflow protection */
      for (i=256;i<512;i++) {
	 umc->rToPixel[i] = umc->rToPixel[255];
	 umc->gToPixel[i] = umc->gToPixel[255];
	 umc->bToPixel[i] = umc->bToPixel[255];
      }
	 
      /* setup dithering kernel */
      max_bits = red_bits;
      if (green_bits > max_bits)  max_bits = green_bits;
      if (blue_bits > max_bits)  max_bits = blue_bits;
      for (i=0;i<16;i++) {
	 umc->kernel[i] = tmp_kernel[i] >> max_bits;
      }
   }
   else {
      /* COLOR INDEXED */
      umc->dithered = umc->undithered = UGL_MESA_CI;     
      /* UGL_INDEX_MASK = 0xFF ! */
      index_bits = (GLint) (mode_info.colorModel & UGL_INDEX_MASK);
      umc->bitsPerPixel = index_bits;
      rgb_mode = GL_FALSE;
      sw_alpha = GL_FALSE;
      /* CLUT allocation if necessary */
      umc->clutSize = mode_info.clutSize;
   }

   umc->windMLFlag = mode & UGL_MESA_WINDML_EXCLUSIVE;
   if (umc->windMLFlag)
      umc->dithered = umc->undithered = UGL_MESA_WINDML;
   
   umc->pixelFormat = umc->dithered;

   /*
    * Compute component-to-pixel lookup tables and dithering kernel
    */

   /* Create a UGL graphics context */

   if ((umc->gc = uglGcCreate(umc->devId)) == UGL_NULL) {
      FREE(umc);
      uglLog(UGL_ERROR_NO_GRAPHICS,
	     "UGLMesaInit: Error uglGcCreate()", 0, 0, 0, 0, 0);
      return NULL;
   }

   /* Set to default values */
   umc->dbSwFlag = GL_FALSE;
   /* Get the first page address */
   umc->firstPage = (UGL_MESA_PAGE *) MALLOC_STRUCT(uglMesaPage);
   umc->firstPage->rowAddr = NULL;
   umc->firstPage->buffer = mode_info.fbAddress;
   /* The user sees the first page */
   umc->visFirstPage = GL_TRUE;
   /* for FREE(secondPage) */
   umc->secondPage = NULL;
   umc->displayWidth = fb_info.width;
   umc->displayHeight = fb_info.height;
   
   if (!(db_mode & UGL_MESA_SINGLE)) {
      /* The user want a double buffer HARDWARE or SOFTWARE */
      umc->dbFlag = GL_TRUE;
      umc->secondPage = (UGL_MESA_PAGE *) MALLOC_STRUCT(uglMesaPage);
      umc->secondPage->rowAddr = NULL;

      if (fb_info.flags & UGL_FB_PAGING_ENABLED
	  && db_mode & UGL_MESA_DOUBLE_HARDWARE) {
	 /* Hardware double buffer */
	 
	 umc->firstPage->pageId = UGL_PAGE_ZERO_ID;
	 umc->secondPage->pageId = uglPageCreate(umc->devId);
	 
	 /* Get the second page address */
	 umc->secondPage->buffer = ((UGL_GEN_DDB *)
				    ((umc->secondPage->pageId)->pDdb))->image;

	 /* Draw in the back buffer at the beginning */
	 if (umc->windMLFlag)
	    uglPageDrawSet(umc->devId, umc->secondPage->pageId);
      }
      else {
	 /* Software double buffer */
	 umc->dbSwFlag = GL_TRUE;
	 umc->secondPage->buffer = NULL;
	 
	 /* Double buffer mode unavailable and fallback to software
            double buffer is impossible in windML mode */
	 if (db_mode & UGL_MESA_DOUBLE_HARDWARE
	     || umc->windMLFlag) {
	    uglGcDestroy(umc->gc);
	    FREE(umc->firstPage);
	    FREE(umc->secondPage);
	    FREE(umc);
	    uglLog(UGL_ERR_TYPE_FATAL,
		   "Harware double buffer unavailable", 0, 0, 0, 0, 0);
	    return NULL;
	 }
      }
   }
   else { /* Single buffer mode */
      umc->dbFlag = GL_FALSE;
   }
   
   /* Create a Mesa Visual from our parameters */ 
   umc->glVisual = _mesa_create_visual(rgb_mode,
				       umc->dbFlag,
				       GL_FALSE, /* stereo */
				       red_bits,
				       green_bits,
				       blue_bits,
				       alpha_bits,
				       index_bits,
				       depth_bits,
				       stencil_bits,
				       accum_red_bits,
				       accum_green_bits,
				       accum_blue_bits,
				       accum_alpha_bits,
				       1);	/* num samples */

   if (!umc->glVisual) {
      if (umc->dbFlag && !umc->dbSwFlag) {
	 uglPageDrawSet(umc->devId, umc->firstPage->pageId);
	 uglPageVisibleSet(umc->devId, umc->firstPage->pageId);
	 uglPageDestroy(umc->devId, umc->secondPage->pageId);
      }
      uglGcDestroy(umc->gc);
      FREE(umc->firstPage);
      FREE(umc->secondPage);
      FREE(umc);
      return NULL;
   }

   /* Use a share list or not */
   umc->glCtx = _mesa_create_context(umc->glVisual,
				     share_list ?
				     share_list->glCtx : (GLcontext *) NULL,
				     (void *) umc, GL_FALSE);

   if (!umc->glCtx) {
      _mesa_destroy_visual(umc->glVisual);
      if (umc->dbFlag && !umc->dbSwFlag) {
	 uglPageDrawSet(umc->devId, umc->firstPage->pageId);
	 uglPageVisibleSet(umc->devId, umc->firstPage->pageId);
	 uglPageDestroy(umc->devId, umc->secondPage->pageId);
      }
      uglGcDestroy(umc->gc);
      FREE(umc->firstPage);
      FREE(umc->secondPage);
      FREE(umc);
      uglLog(UGL_ERR_TYPE_FATAL,
	     "uglMesaCreateNewContextExt: Error _mesa_create_context",
	     0, 0, 0, 0, 0);
      return NULL;
   }

   _mesa_enable_sw_extensions(umc->glCtx);
   _mesa_enable_1_3_extensions(umc->glCtx);
   
   /* Only for depth, stencil, accum and alpha buffers */
   umc->glBuffer = _mesa_create_framebuffer(umc->glVisual,
					    umc->glVisual->depthBits > 0,
					    umc->glVisual->stencilBits > 0,
					    umc->glVisual->accumRedBits > 0,
					    sw_alpha);


   if (!umc->glBuffer) {
      _mesa_destroy_visual(umc->glVisual);
      _mesa_destroy_context(umc->glCtx);
      if (umc->dbFlag && !umc->dbSwFlag) {
	 uglPageDrawSet(umc->devId, umc->firstPage->pageId);
	 uglPageVisibleSet(umc->devId, umc->firstPage->pageId);
	 uglPageDestroy(umc->devId, umc->secondPage->pageId);
      }
      uglGcDestroy(umc->gc);
      FREE(umc->firstPage);
      FREE(umc->secondPage);
      FREE(umc);

      uglLog(UGL_ERR_TYPE_FATAL,
	     "uglMesaCreateNewContextExt: _mesa_create_framebuffer",
	     0, 0, 0, 0, 0);
      return NULL;
   }

   /* Set to default values before uglMesaMakeCurrentContext call */
   umc->left = 0;
   umc->top = 0;
   umc->width = 0;
   umc->height = 0;
   umc->rowLength = 0;
      
   umc->glCtx->Driver.UpdateState = uglmesa_update_state;

   /* Initialize the software rasterizer and helper modules */
   _swrast_CreateContext(umc->glCtx);
   _ac_CreateContext(umc->glCtx);
   _tnl_CreateContext(umc->glCtx);
   _swsetup_CreateContext(umc->glCtx);

   uglmesa_register_swrast_functions(umc->glCtx);

   uglmesa_init_pointers(umc->glCtx);

   return umc;
}

/*
 * Recompute the values of the context's rowaddr array.
 * These values are useful to draw a color pixel on the screen, avoid
 * a multiplication.
 */
static void compute_row_addresses(UGL_MESA_CONTEXT umc)
{
   GLuint i;

   if (umc->windMLFlag) {
      uglViewPortSet(umc->gc, umc->left, umc->top,
		     umc->left+umc->width, umc->top+umc->height);
      return;
   }

   FREE(umc->firstPage->rowAddr);
   umc->firstPage->rowAddr = (GLuint *) MALLOC(umc->height*sizeof(GLuint));

   if (umc->dbSwFlag) {
      FREE(umc->secondPage->rowAddr);
      umc->secondPage->rowAddr = (GLuint *) MALLOC(umc->height*sizeof(GLuint));

      /* FIRST PAGE - rowAddr */
      if (umc->yUp) {
	 for (i = 0; i < umc->height; i++)
	    umc->firstPage->rowAddr[i] = ((umc->top + umc->height - 1) - i) * 
		umc->displayWidth + umc->left;
      }
      else {
	 for (i = 0; i < umc->height; i++)
	    umc->firstPage->rowAddr[i] = (i + umc->top) * umc->displayWidth +
		umc->left;
      }

      /* SECOND PAGE - rowAddr */
      if (umc->yUp) {
	 for (i = 0; i < umc->height; i++)
	    umc->secondPage->rowAddr[i] = ((umc->height - 1) - i) * umc->width;
      }
      else {
	 for (i = 0; i < umc->height; i++)
	    umc->secondPage->rowAddr[i] = i * umc->width;
      }
   }
   else {
      /* if Y==0 is the bottom line of window and increases upward,
	 yUp is true */
      if (umc->yUp) {
	 for (i = 0; i < umc->height; i++)
	    umc->firstPage->rowAddr[i] = ((umc->top + umc->height - 1) - i) * 
		umc->displayWidth + umc->left;
      }
      else {
	 for (i = 0; i < umc->height; i++)
	    umc->firstPage->rowAddr[i] = (i + umc->top) * umc->displayWidth
		+ umc->left;
      }
      /* In single or double buffer hw */
      umc->secondPage->rowAddr = umc->firstPage->rowAddr;
   }
}

GLboolean uglMesaMakeCurrentContext(UGL_MESA_CONTEXT umc,
				    GLsizei left, GLsizei top,
				    GLsizei width, GLsizei height)
{
   GLboolean retValue = GL_TRUE;
   
   if (!umc) {
      return GL_FALSE;
   }

   /* Bind the drawing functions */
   uglmesa_update_state(umc->glCtx, 0);

   /* Multiple WindML/Mesa context */
   taskVarAdd(0, (int *)&_glapi_Context);
   
   /* Bind the ancillaries buffer to UGL/Mesa context */
   _mesa_make_current(umc->glCtx, umc->glBuffer);

   /* Check values */
   if (left == 0 && width == UGL_MESA_FULLSCREEN_WIDTH &&
       top == 0 && height == UGL_MESA_FULLSCREEN_HEIGHT) {
      /* The first are unusefull ! It's just more clear */
      umc->left = 0;
      umc->top = 0;
      umc->width = umc->displayWidth;
      umc->height = umc->displayHeight;
      umc->fullScreen = GL_TRUE;
   } else {
      if (width == UGL_MESA_FULLSCREEN_WIDTH)
	 width = umc->displayWidth;
      if (height == UGL_MESA_FULLSCREEN_HEIGHT)
	 height = umc->displayHeight;
      
      if (0 <= left && 0 < width && left+width <= umc->displayWidth &&
	  0 <= top && 0 < height && top+height <= umc->displayHeight) {
	 umc->left = left;
	 umc->top = top;
	 umc->width = width;
	 umc->height = height;
	 umc->fullScreen = GL_FALSE;
      }
      else { /* ERROR */
	 uglLog(UGL_ERR_TYPE_WARN,
		"uglMesaMakeCurrentContext: left:%i top:%i"
		" right:%i bottom:%i\n",
		left, top, left+width, top+height, 0);
	 /* The first are unusefull ! It's just more clear */
	 umc->left = 0;
	 umc->top = 0;
	 umc->width =  umc->displayWidth;
	 umc->height = umc->displayHeight;
	 umc->fullScreen = GL_TRUE;
	 retValue = GL_FALSE;
      }
   }

   umc->bufferSize = (umc->width * umc->height * umc->bitsPerPixel) / 8;
   /* Compute row length in bytes */
   umc->rowLength = (GLsizei) ((umc->width * umc->bitsPerPixel) / 8);
   /* ((UGL_GEN_DDB *)pDriver->ugi.pPageZero->pDdb)->stride; */

   if (umc->dbFlag) {
      if (umc->dbSwFlag) {
	 alloc_sw_page(umc);
      }
      umc->drawBuffer = umc->secondPage;
      umc->readBuffer = umc->secondPage;
   }
   else {
      umc->drawBuffer = umc->firstPage;
      umc->readBuffer = umc->firstPage;
   }
   
   /* Y coordinates increases downward */
   umc->yUp = GL_TRUE;
   compute_row_addresses(umc);

   /* Default value of clear pixel */
   umc->clearPixel = 0;

   /* Initialize viewport if necessary, identical to glViewPort */

   if (umc->glCtx->Viewport.Width == 0) {
      /* initialize viewport and scissor box to buffer size */
      _mesa_Viewport(0, 0, umc->width, umc->height);
      umc->glCtx->Scissor.Width = umc->width;
      umc->glCtx->Scissor.Height = umc->height;
   }

   /* uglBatchStart(umc->gc); */

   return (retValue);
}

GLboolean uglMesaMoveWindow (GLsizei dx, GLsizei dy)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   /* Check values */
   if (0 <= (umc->left + dx) &&
       (umc->left+umc->width+dx) < umc->displayWidth &&
       0 <= (umc->top + dy) &&
       (umc->top+umc->height+dy) < umc->displayHeight &&
       (dx != 0 || dy != 0)) {
      umc->left += dx;
      umc->top += dy;
   } 
   else
      return GL_FALSE;

   if (umc->left == 0 && umc->top == 0
       && umc->width == umc->displayWidth
       && umc->height == umc->displayHeight)
      umc->fullScreen = GL_TRUE;
   else
      umc->fullScreen = GL_FALSE;
   
   compute_row_addresses(umc);

   return GL_TRUE;
}

GLboolean uglMesaMoveToWindow (GLsizei left, GLsizei top)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   /* Check values */
   if (0 <= left && (left+umc->width) < umc->displayWidth &&
       0 <= top  && (top+umc->height) < umc->displayHeight &&
       (umc->left != left || umc->top != top)) {
      umc->left = left;
      umc->top = top;
   } 
   else
      return GL_FALSE;

   if (umc->left == 0 && umc->top == 0
       && umc->width == umc->displayWidth
       && umc->height == umc->displayHeight)
      umc->fullScreen = GL_TRUE;
   else
      umc->fullScreen = GL_FALSE;
     
   compute_row_addresses(umc);

   return GL_TRUE;
}

GLboolean uglMesaResizeWindow (GLsizei dw, GLsizei dh)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   /* Check values */
   if (0 < (umc->width + dw) &&
       (umc->left+umc->width+dw) < umc->displayWidth &&
       0 < (umc->height + dh) &&
       (umc->top+umc->height+dh) < umc->displayHeight &&
       (dw != 0 || dh !=0)) {
      umc->width += dw;
      umc->height += dh;
   } 
   else
      return GL_FALSE;

   umc->bufferSize = (umc->width * umc->height * umc->bitsPerPixel) / 8;
   /* Compute row length in bytes */
   umc->rowLength = (GLsizei) ((umc->width * umc->bitsPerPixel) / 8);

   if (umc->dbSwFlag) {
      alloc_sw_page(umc);
   }
   
   if (umc->left == 0 && umc->top == 0
       && umc->width == umc->displayWidth
       && umc->height == umc->displayHeight)
      umc->fullScreen = GL_TRUE;
   else
      umc->fullScreen = GL_FALSE;
   
   compute_row_addresses(umc);

   _mesa_Viewport(0, 0, umc->width, umc->height);
   umc->glCtx->Scissor.Width = umc->width;
   umc->glCtx->Scissor.Height = umc->height;
   
   return GL_TRUE;
}

GLboolean uglMesaResizeToWindow (GLsizei width, GLsizei height)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   /* Zero is not allowed and reserved for fullscreen mode */
   if (width == UGL_MESA_FULLSCREEN_WIDTH)
      width = umc->displayWidth;
   if (height == UGL_MESA_FULLSCREEN_HEIGHT)
      height = umc->displayHeight;

   /* Check values */
   if (0 < width && (umc->left+width) <= umc->displayWidth &&
       0 < height  && (umc->top+height) <= umc->displayHeight &&
       (umc->height != height || umc->width != width)) {
      umc->width = width;
      umc->height = height;
   } 
   else
      return GL_FALSE;

   umc->bufferSize = (umc->width * umc->height * umc->bitsPerPixel) / 8;
   /* Compute row length in bytes */
   umc->rowLength = (GLsizei) ((umc->width * umc->bitsPerPixel) / 8);

   if (umc->dbSwFlag) {
      alloc_sw_page(umc);
   }
   
   if (umc->left == 0 && umc->top == 0
       && umc->width == umc->displayWidth
       && umc->height == umc->displayHeight)
      umc->fullScreen = GL_TRUE;
   else
      umc->fullScreen = GL_FALSE;
      
   compute_row_addresses(umc);

   _mesa_Viewport(0, 0, umc->width, umc->height);
   umc->glCtx->Scissor.Width = umc->width;
   umc->glCtx->Scissor.Height = umc->height;
   
   return GL_TRUE;
}

void uglMesaDestroyContext(void)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx); 

   if (ctx) {
      /* Select the front buffer */
      if (umc->dbFlag && !umc->dbSwFlag) {
	 uglPageDrawSet(umc->devId, umc->firstPage->pageId);
	 uglPageVisibleSet(umc->devId, umc->firstPage->pageId);
	 uglPageDestroy(umc->devId, umc->secondPage->pageId);
      }

      /* Destroy UGL stuff before mesa stuff */
/*      uglBatchEnd(umc->gc); */
      uglGcDestroy(umc->gc);

      _swsetup_DestroyContext(umc->glCtx);
      _tnl_DestroyContext(umc->glCtx);
      _ac_DestroyContext(umc->glCtx);
      _swrast_DestroyContext(umc->glCtx);
      
      _mesa_destroy_framebuffer(umc->glBuffer);
      _mesa_destroy_context(umc->glCtx);
      _mesa_destroy_visual(umc->glVisual);

      taskVarDelete(0, (int *)&_glapi_Context);
      
      FREE(umc->firstPage->rowAddr);
      FREE(umc->firstPage);

      if (umc->dbSwFlag) {
	 FREE(umc->secondPage->buffer);
	 FREE(umc->secondPage->rowAddr);
      }

      FREE(umc->secondPage);
      FREE(umc);
   }
}

UGL_MESA_CONTEXT uglMesaGetCurrentContext(void)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   return umc;
}

static void swap_buffer_single(UGL_MESA_CONTEXT umc)
{
   /* Do nothing */
}

static void swap_buffer_double_sw(UGL_MESA_CONTEXT umc)
{
   GLuint y;

   /*
    * If we're swapping the buffer associated with the current context
    * we have to flush any pending rendering commands first.
    */
   _mesa_notifySwapBuffers(umc->glCtx);

   /* Copy software */
   if (umc->fullScreen)
      memcpy(umc->firstPage->buffer, umc->secondPage->buffer, umc->bufferSize);
   else {
      switch (umc->bitsPerPixel) {
	 case 32:
	    for (y=0; y<umc->height; y++)
	       memcpy(PIXELADDR4(umc->firstPage, 0 ,y),
		      PIXELADDR4(umc->secondPage, 0 ,y),
		      umc->rowLength);
	 case 24:
	    for (y=0; y<umc->height; y++)
	       memcpy(PIXELADDR3(umc->firstPage, 0 ,y),
		      PIXELADDR3(umc->secondPage, 0 ,y),
		      umc->rowLength);
	    break;
	 case 16:
	    for (y=0; y<umc->height; y++)
	       memcpy(PIXELADDR2(umc->firstPage, 0 ,y),
		      PIXELADDR2(umc->secondPage, 0 ,y),
		      umc->rowLength);
	    break;
	 case 8:
	    for (y=0; y<umc->height; y++)
	       memcpy(PIXELADDR1(umc->firstPage, 0 ,y),
		      PIXELADDR1(umc->secondPage, 0 ,y),
		      umc->rowLength);
      }
   }
}

static void swap_buffer_double_hw(UGL_MESA_CONTEXT umc)
{
   /*
    * If we're swapping the buffer associated with the current context
    * we have to flush any pending rendering commands first.
    */
   _mesa_notifySwapBuffers(umc->glCtx);

   /* Shows back buffer and swaps */
   if (umc->visFirstPage) {
      uglPageVisibleSet(umc->devId, umc->secondPage->pageId);
      umc->visFirstPage = GL_FALSE;
   }
   else {
      uglPageVisibleSet(umc->devId, umc->firstPage->pageId);
      umc->visFirstPage = GL_TRUE;
   }
   
   /* swaps drawBuffer */
   if (umc->drawBuffer == umc->firstPage)
      umc->drawBuffer = umc->secondPage;
   else
      umc->drawBuffer = umc->firstPage;

   /* swaps readBuffer */
   if (umc->readBuffer == umc->firstPage)
      umc->readBuffer = umc->secondPage;
   else
      umc->readBuffer = umc->firstPage;
}

static void swap_buffer_double_windml(UGL_MESA_CONTEXT umc)
{
   /*
    * If we're swapping the buffer associated with the current context
    * we have to flush any pending rendering commands first.
    */
   _mesa_notifySwapBuffers(umc->glCtx);

   /* Shows back buffer and swaps */
   if (umc->visFirstPage) {
      uglPageVisibleSet(umc->devId, umc->secondPage->pageId);
      umc->visFirstPage = GL_FALSE;
   }
   else {
      uglPageVisibleSet(umc->devId, umc->firstPage->pageId);
      umc->visFirstPage = GL_TRUE;
   }
   
   /* swaps drawBuffer */
   if (umc->drawBuffer == umc->firstPage) {
      umc->drawBuffer = umc->secondPage;
      uglPageDrawSet(umc->devId, umc->drawBuffer->pageId);
   }
   else {
      umc->drawBuffer = umc->firstPage;
      uglPageDrawSet(umc->devId, umc->drawBuffer->pageId);
   }

   /* swaps readBuffer */
   if (umc->readBuffer == umc->firstPage)
      umc->readBuffer = umc->secondPage;
   else
      umc->readBuffer = umc->firstPage;
}

/*
 * Update swap buffers functions
 */
void uglmesa_update_swap_funcs(GLcontext * ctx)
{
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   
   if (umc->dbFlag) {
      /* Software or hardware buffer */
      if (umc->windMLFlag)
	 umc->swapFunc = swap_buffer_double_windml;
      else
	 umc->swapFunc = (umc->dbSwFlag) ?
	     swap_buffer_double_sw : swap_buffer_double_hw;
   }
   else {
      umc->swapFunc = swap_buffer_single;
   }
}

void uglMesaSwapBuffers(void)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   
   (*umc->swapFunc)(umc); 
}

void uglMesaPixelStore(GLint pname, GLint value)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   switch (pname) {
      case UGL_MESA_ROW_LENGTH:
	 if (value < 0) {
	    _mesa_error(ctx, GL_INVALID_VALUE, "uglMesaPixelStore(value)");
	    return;
	 }
	 umc->rowLength = value;
	 break;
      case UGL_MESA_Y_UP:
	 umc->yUp = (value) ? GL_TRUE: GL_FALSE;
	 break;
      default:
	 _mesa_error(ctx, GL_INVALID_ENUM, "uglMesaPixelStore(pname)");
	 return;
   }

   compute_row_addresses(umc);
}

void uglMesaGetIntegerv(GLint pname, GLint * value)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   
   switch (pname) {
      case UGL_MESA_LEFT_X:
	 *value = umc->left;
	 break;
      case UGL_MESA_TOP_Y:
	 *value = umc->top;
	 break;
      case UGL_MESA_WIDTH:
	 *value = umc->width;
	 break;
      case UGL_MESA_HEIGHT:
	 *value = umc->height;
	 break;
      case UGL_MESA_DISPLAY_WIDTH:
	 *value = umc->displayWidth;
	 break;
      case UGL_MESA_DISPLAY_HEIGHT:
	 *value = umc->displayHeight;
	 break;
      case UGL_MESA_ROW_LENGTH:
	 *value = umc->rowLength;
	 break;
      case UGL_MESA_RGB:
	 *value = (umc->pixelFormat == UGL_MESA_CI) ?  GL_FALSE : GL_TRUE;
	 break;
      case UGL_MESA_COLOR_INDEXED:
	 *value = (umc->pixelFormat == UGL_MESA_CI) ?  GL_TRUE : GL_FALSE;
	 break;
      case UGL_MESA_DOUBLE_BUFFER:
	 *value = umc->dbFlag;
	 break;
      case UGL_MESA_SINGLE_BUFFER:
	 *value = !umc->dbFlag;
	 break;
      case UGL_MESA_DOUBLE_BUFFER_SOFTWARE:
	 *value = umc->dbSwFlag;
	 break;
      case UGL_MESA_DOUBLE_BUFFER_HARDWARE:
	 *value = (umc->dbFlag) ? !umc->dbSwFlag : GL_FALSE;
	 break;
      default:
	 _mesa_error(ctx, GL_INVALID_ENUM, "uglMesaGetIntergerv(pname)");
   }
}

GLboolean uglMesaGetColorBuffer(GLint * width, GLint * height,
				GLint * format, void **buffer)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);

   *width = umc->width;
   *height = umc->height;
   *format = umc->pixelFormat;
   *buffer = umc->drawBuffer->buffer;

   return GL_TRUE;
}

GLboolean uglMesaGetDepthBuffer(GLint * width, GLint * height,
				GLint * bytesPerValue, void **buffer)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   
   if ((!umc->glBuffer) || (!umc->glBuffer->DepthBuffer)) {
      *width = 0;
      *height = 0;
      *bytesPerValue = 0;
      *buffer = 0;
      return GL_FALSE;
   }
   else {
      *width = umc->glBuffer->Width;
      *height = umc->glBuffer->Height;

      if (umc->glVisual->depthBits <= 16)
	 *bytesPerValue = sizeof(GLushort);
      else
	 *bytesPerValue = sizeof(GLuint);

      *buffer = umc->glBuffer->DepthBuffer;
      return GL_TRUE;
   }
}

GLboolean uglMesaSetColor(GLubyte index, GLfloat red,
			  GLfloat green, GLfloat blue)
{
   GET_CURRENT_CONTEXT(ctx);
   UGL_MESA_CONTEXT umc = GET_UGL_MESA_CONTEXT(ctx);
   UGL_RGB rgbColor;
   UGL_COLOR uglColor;

   GLubyte r, g, b;

   r=g=b=0;
   
   if (umc->pixelFormat == UGL_MESA_CI) {
      /* A ubyte is always greater than zero */

      if (umc->clutSize < index)
	 return GL_FALSE;

      if (0.0 < red && red < 1.0)
	 r = (GLubyte) (red * 255.0);
      else
	 return GL_FALSE;

      if (0.0 < green && green < 1.0)
	 r = (GLubyte) (green * 255.0);
      else
	 return GL_FALSE;

      if (0.0 < blue && blue < 1.0)
	 r = (GLubyte) (blue * 255.0);
      else
	 return GL_FALSE;

      rgbColor = UGL_MAKE_RGB(r, g, b);
      
      if (uglColorAlloc(umc->devId, &rgbColor, (UGL_ORD *)&index,
			&uglColor, 1) != UGL_STATUS_OK)
	 return GL_FALSE;
   }

   return GL_TRUE;
}
