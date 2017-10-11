/* uglmesaP.h - UGL/Mesa private header */

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

/* includes */

#include "mtypes.h"
#include "GL/uglmesa.h"

/* defines */

/*
 * Pixel format
 */
#define UGL_MESA_ARGB8888          0x01
#define UGL_MESA_ARGB4444          0x02
#define UGL_MESA_RGB888            0x03
#define UGL_MESA_RGB565            0x04
#define UGL_MESA_CI                0x05
#define UGL_MESA_WINDML            0x06
/*
 * Dithering ARGB4444, RBG565
 */ 
#define UGL_MESA_DITHER16          0x10

/* Packing macros */

#define PACK_ARGB8888(A, R, G, B) (((A) << 24) | \
                                   ((R) << 16) | \
                                   ((G) << 8)  | \
                                    (B))

#define PACK_ARGB4444(A ,R, G, B) ((((A) & 0xf0) << 8) | \
                                   (((R) & 0xf0) << 4) | \
                                    ((G) & 0xf0)       | \
                                   (((B) & 0xf0) >> 4))

#define PACK_RGB888(R, G, B) (((R) << 16) | ((G) << 8) | (B))

#define PACK_RGB444(R, G, B) ((((R) & 0xf0) << 4) | \
                               ((G) & 0xf0)       | \
                              (((B) & 0xf0) >> 4))

#define PACK_RGB565(R, G, B) ((((R) & 0xf8) << 8) | \
                              (((G) & 0xfc) << 3) | \
                               ((B) >> 3))

#define PACK_DITHER16(PIXEL, X, Y, R, G, B)                     \
{								\
   int d = umc->kernel[((X)&3) | (((Y)&3)<<2)];	                \
   PIXEL = umc->rToPixel[(R)+d]			                \
         | umc->gToPixel[(G)+d]			                \
         | umc->bToPixel[(B)+d];			        \
}

#define FLIP(Y) (umc->height-1-(Y))
#define PIXELADDR4(P,X,Y)  ((UGL_UINT32 *)(P)->buffer +(P)->rowAddr[(Y)] + (X))
#define PIXELADDR3(P,X,Y)  ((_RGB_T *)(P)->buffer + (P)->rowAddr[(Y)] + (X))
#define PIXELADDR2(P,X,Y)  ((UGL_UINT16 *)(P)->buffer +(P)->rowAddr[(Y)] + (X))
#define PIXELADDR1(P,X,Y)  ((UGL_UINT8 *)(P)->buffer + (P)->rowAddr[(Y)] + (X))

/* Warning ! Works only after a call to GET_CURRENT_CONTEXT (speed up)*/

#define GET_UGL_MESA_CONTEXT(ctx)  ((UGL_MESA_CONTEXT) (ctx->DriverCtx))

/* typedefs */

typedef struct _rgb_t {
   UGL_UINT8 r;
   UGL_UINT8 g;
   UGL_UINT8 b;
} _RGB_T;

typedef struct uglMesaPage {
    GLubyte *buffer;            /* buffer address */
    UGL_PAGE_ID pageId;         /* WindML drawing buffers */
    GLuint *rowAddr;            /* first pixel offset in each image row */
} UGL_MESA_PAGE;

/*
 * This is the UGL/Mesa context struct.
 * Notice how it includes a GLcontext.  By doing this we're mimicking
 * C++ inheritance/derivation.
 * Later, we can cast a GLcontext pointer into an uglMesaContext pointer
 * or vice versa.
 */

struct uglMesaContext
{
   GLcontext *glCtx;		/* The core GL/Mesa context */
   GLvisual *glVisual;		/* Describes the buffers */
   GLframebuffer *glBuffer;	/* Depth, stencil, accum, etc buffers */
   UGL_MESA_PAGE *firstPage;	/* First page in video RAM */
   UGL_MESA_PAGE *secondPage;	/* Second page in video RAM */
   UGL_MESA_PAGE *drawBuffer;   /* the draw buffer (front or back buffer) */
   UGL_MESA_PAGE *readBuffer;	/* the read buffer (front or back buffer) */
   GLboolean visFirstPage;      /* First page is visible */
   GLsizei rowLength;		/* number of pixels per row */
   GLint bitsPerPixel;		/* pixel size */
   GLint bufferSize;		/* buffer size in bytes */
   GLsizei displayWidth;        /* Screen size */ 
   GLsizei displayHeight;       /* Screen size */ 
   GLsizei left, top;           /* Coordinates of superior pixel */ 
   GLsizei width, height;	/* Size of color buffer window */
   GLboolean fullScreen;        /* Fullscreen or window (windowing API) */
   GLboolean yUp;		/* Y increase upward (downward by default) */
   UGL_DEVICE_ID devId;		/* Display device Id */
   UGL_GC_ID gc;		/* UGL Graphic context */
   UGL_COLOR clearPixel;	/* Clear pixel index or RGB value */
   GLubyte clearColor[4];	/* Clear components RGBA */
   GLboolean dbFlag;		/* Double buffer */
   GLboolean dbSwFlag;		/* Software buffer allocation (DB) */
   GLboolean windMLFlag;        /* Use exclusively windML calls */ 
   GLenum pixelFormat;          /* UGL_MESA_ARGB8888, ... */
   GLint clutSize;		/* CLUT size */
   GLenum dithered;             /* pixelFormat dithering enabled */
   GLenum undithered;		/* pixelformat dithering disabled */
   GLubyte kernel[16];          /* Dither kernel */
   GLfloat rGamma, gGamma, bGamma; /* Gamma values, 1.0 is default */
   UGL_UINT32 rMask, gMask, bMask, aMask; /* Color component mask value */
   GLint rShift, gShift, bShift; /* Color component shift from LSB */
   UGL_UINT32 rToPixel[512];	 /* RGB to pixel conversion */
   UGL_UINT32 gToPixel[512];
   UGL_UINT32 bToPixel[512];
   GLubyte pixelToR[256];	 /* Pixel to RGB conversion */
   GLubyte pixelToG[256];
   GLubyte pixelToB[256];
   void (*clearFunc) (UGL_MESA_PAGE *pPage,
		      GLcontext * ctx, GLboolean all,
		      GLint x, GLint y,
		      GLint width, GLint height);  /* clear function */
   void (*swapFunc) (UGL_MESA_CONTEXT umc); /* swap buffers function */
};

extern void ugl_log(char *msg);

extern void uglmesa_update_state(GLcontext * ctx, GLuint newstate);
extern void uglmesa_init_pointers(GLcontext * ctx);
extern void uglmesa_update_span_funcs(GLcontext * ctx);
extern void uglmesa_update_swap_funcs(GLcontext * ctx);
extern void uglmesa_set_read_buffer(GLcontext * ctx,
				    GLframebuffer * buffer, GLenum mode);
extern void uglmesa_register_swrast_functions(GLcontext * ctx);
extern void uglmesa_choose_line(GLcontext * ctx);
extern void uglmesa_choose_triangle(GLcontext * ctx);
