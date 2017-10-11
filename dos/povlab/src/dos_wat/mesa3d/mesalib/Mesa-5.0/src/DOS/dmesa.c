/*
 * Mesa 3-D graphics library
 * Version:  4.1
 * 
 * Copyright (C) 1999  Brian Paul   All Rights Reserved.
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
 * DOS/DJGPP device driver v1.2 for Mesa 4.1
 *
 *  Copyright (C) 2002 - Borca Daniel
 *  Email : dborca@yahoo.com
 *  Web   : http://www.geocities.com/dborca
 */


#include "glheader.h"
#include "context.h"
#include "GL/dmesa.h"
#include "extensions.h"
#include "macros.h"
#include "matrix.h"
#include "mmath.h"
#include "texformat.h"
#include "texstore.h"
#include "array_cache/acache.h"
#include "swrast/s_context.h"
#include "swrast/s_depth.h"
#include "swrast/s_lines.h"
#include "swrast/s_triangle.h"
#include "swrast/s_trispan.h"
#include "swrast/swrast.h"
#include "swrast_setup/swrast_setup.h"
#include "tnl/tnl.h"
#include "tnl/t_context.h"
#include "tnl/t_pipeline.h"

#include "video.h"



/*
 * In C++ terms, this class derives from the GLvisual class.
 * Add system-specific fields to it.
 */
struct dmesa_visual {
   GLvisual *gl_visual;
   GLboolean db_flag;           /* double buffered? */
   GLboolean rgb_flag;          /* RGB mode? */
   GLuint depth;                /* bits per pixel (1, 8, 24, etc) */
};

/*
 * In C++ terms, this class derives from the GLframebuffer class.
 * Add system-specific fields to it.
 */
struct dmesa_buffer {
   GLframebuffer gl_buffer;     /* The depth, stencil, accum, etc buffers */
   void *the_window;            /* your window handle, etc */

   int xpos, ypos;              /* position */
   int width, height;           /* size in pixels */
   int bypp, stride, bytes;     /* bytes per pixel, in a line, then total */
};

/*
 * In C++ terms, this class derives from the GLcontext class.
 * Add system-specific fields to it.
 */
struct dmesa_context {
   GLcontext *gl_ctx;           /* the core library context */
   DMesaVisual visual;
   DMesaBuffer Buffer;
   GLuint ClearColor;
   /* etc... */
};



/****************************************************************************
 * Read/Write pixels
 ***************************************************************************/
#define FLIP(y)  (c->Buffer->height - (y) - 1)
#define FLIP2(y) (h - (y) - 1)

static void write_rgba_span (const GLcontext *ctx, GLuint n, GLint x, GLint y,
                             const GLubyte rgba[][4], const GLubyte mask[])
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint i, offset;

 offset = c->Buffer->width * FLIP(y) + x;
 if (mask) {
    /* draw some pixels */
    for (i=0; i<n; i++, offset++) {
        if (mask[i]) {
           vl_putpixel(b, offset, vl_mixrgba(rgba[i]));
        }
    }
 } else {
    /* draw all pixels */
    for (i=0; i<n; i++, offset++) {
        vl_putpixel(b, offset, vl_mixrgba(rgba[i]));
    }
 }
}



static void write_rgb_span (const GLcontext *ctx, GLuint n, GLint x, GLint y,
                            const GLubyte rgb[][3], const GLubyte mask[])
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint i, offset;

 offset = c->Buffer->width * FLIP(y) + x;
 if (mask) {
    /* draw some pixels */
    for (i=0; i<n; i++, offset++) {
        if (mask[i]) {
           vl_putpixel(b, offset, vl_mixrgb(rgb[i]));
        }
    }
 } else {
    /* draw all pixels */
    for (i=0; i<n; i++, offset++) {
        vl_putpixel(b, offset, vl_mixrgb(rgb[i]));
    }
 }
}



static void write_mono_rgba_span (const GLcontext *ctx,
                                  GLuint n, GLint x, GLint y,
                                  const GLchan color[4], const GLubyte mask[])
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint i, offset, rgba = vl_mixrgba(color);

 offset = c->Buffer->width * FLIP(y) + x;
 if (mask) {
    /* draw some pixels */
    for (i=0; i<n; i++, offset++) {
        if (mask[i]) {
           vl_putpixel(b, offset, rgba);
        }
    }
 } else {
    /* draw all pixels */
    for (i=0; i<n; i++, offset++) {
        vl_putpixel(b, offset, rgba);
    }
 }
}



static void read_rgba_span (const GLcontext *ctx, GLuint n, GLint x, GLint y,
                            GLubyte rgba[][4])
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint i, offset;

 offset = c->Buffer->width * FLIP(y) + x;
 /* read all pixels */
 for (i=0; i<n; i++, offset++) {
     vl_getrgba(b, offset, rgba[i]);
 }
}



static void write_rgba_pixels (const GLcontext *ctx,
                               GLuint n, const GLint x[], const GLint y[],
                               const GLubyte rgba[][4], const GLubyte mask[])
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint i, w = c->Buffer->width, h = c->Buffer->height;

 if (mask) {
    /* draw some pixels */
    for (i=0; i<n; i++) {
        if (mask[i]) {
           vl_putpixel(b, FLIP2(y[i])*w + x[i], vl_mixrgba(rgba[i]));
        }
    }
 } else {
    /* draw all pixels */
    for (i=0; i<n; i++) {
        vl_putpixel(b, FLIP2(y[i])*w + x[i], vl_mixrgba(rgba[i]));
    }
 }
}



static void write_mono_rgba_pixels (const GLcontext *ctx,
                                    GLuint n, const GLint x[], const GLint y[],
                                    const GLchan color[4], const GLubyte mask[])
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint i, w = c->Buffer->width, h = c->Buffer->height, rgba = vl_mixrgba(color);

 if (mask) {
    /* draw some pixels */
    for (i=0; i<n; i++) {
        if (mask[i]) {
           vl_putpixel(b, FLIP2(y[i])*w + x[i], rgba);
        }
    }
 } else {
    /* draw all pixels */
    for (i=0; i<n; i++) {
        vl_putpixel(b, FLIP2(y[i])*w + x[i], rgba);
    }
 }
}



static void read_rgba_pixels (const GLcontext *ctx,
                              GLuint n, const GLint x[], const GLint y[],
                              GLubyte rgba[][4], const GLubyte mask[])
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint i, w = c->Buffer->width, h = c->Buffer->height;

 if (mask) {
    /* read some pixels */
    for (i=0; i<n; i++) {
        if (mask[i]) {
           vl_getrgba(b, FLIP2(y[i])*w + x[i], rgba[i]);
        }
    }
 } else {
    /* read all pixels */
    for (i=0; i<n; i++) {
        vl_getrgba(b, FLIP2(y[i])*w + x[i], rgba[i]);
    }
 }
}



/****************************************************************************
 * Optimized triangle rendering
 ***************************************************************************/

/*
 * flat, NON-depth-buffered, triangle.
 */
static void tri_rgb_flat (GLcontext *ctx,
                          const SWvertex *v0,
                          const SWvertex *v1,
                          const SWvertex *v2)
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint w = c->Buffer->width, h = c->Buffer->height;

#define SETUP_CODE GLuint rgb = vl_mixrgb(v2->color);

#define RENDER_SPAN(span)					\
 GLuint i, offset = FLIP2(span.y)*w + span.x;			\
 for (i = 0; i < span.end; i++, offset++) {			\
     vl_putpixel(b, offset, rgb);				\
 }

#include "swrast/s_tritemp.h"
}



/*
 * flat, depth-buffered, triangle.
 */
static void tri_rgb_flat_z (GLcontext *ctx,
                            const SWvertex *v0,
                            const SWvertex *v1,
                            const SWvertex *v2)
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint w = c->Buffer->width, h = c->Buffer->height;

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define SETUP_CODE GLuint rgb = vl_mixrgb(v2->color);

#define RENDER_SPAN(span)					\
 GLuint i, offset = FLIP2(span.y)*w + span.x;			\
 for (i = 0; i < span.end; i++, offset++) {			\
     const DEPTH_TYPE z = FixedToDepth(span.z);			\
     if (z < zRow[i]) {						\
        vl_putpixel(b, offset, rgb);				\
        zRow[i] = z;						\
     }								\
     span.z += span.zStep;					\
 }

#include "swrast/s_tritemp.h"
}



/*
 * smooth, NON-depth-buffered, triangle.
 */
static void tri_rgb_smooth (GLcontext *ctx,
                            const SWvertex *v0,
                            const SWvertex *v1,
                            const SWvertex *v2)
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint w = c->Buffer->width, h = c->Buffer->height;

#define INTERP_RGB 1
#define RENDER_SPAN(span)					\
 GLuint i, offset = FLIP2(span.y)*w + span.x;			\
 for (i = 0; i < span.end; i++, offset++) {			\
     unsigned char rgb[3];					\
     rgb[0] = FixedToInt(span.red);				\
     rgb[1] = FixedToInt(span.green);				\
     rgb[2] = FixedToInt(span.blue);				\
     vl_putpixel(b, offset, vl_mixrgb(rgb));			\
     span.red += span.redStep;					\
     span.green += span.greenStep;				\
     span.blue += span.blueStep;				\
 }

#include "swrast/s_tritemp.h"
}



/*
 * smooth, depth-buffered, triangle.
 */
static void tri_rgb_smooth_z (GLcontext *ctx,
                              const SWvertex *v0,
                              const SWvertex *v1,
                              const SWvertex *v2)
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 void *b = c->Buffer->the_window;
 GLuint w = c->Buffer->width, h = c->Buffer->height;

#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1

#define RENDER_SPAN(span)					\
 GLuint i, offset = FLIP2(span.y)*w + span.x;			\
 for (i = 0; i < span.end; i++, offset++) {			\
     const DEPTH_TYPE z = FixedToDepth(span.z);			\
     if (z < zRow[i]) {						\
        unsigned char rgb[3];					\
        rgb[0] = FixedToInt(span.red);				\
        rgb[1] = FixedToInt(span.green);			\
        rgb[2] = FixedToInt(span.blue);				\
        vl_putpixel(b, offset, vl_mixrgb(rgb));			\
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
 * Analyze context state to see if we can provide a fast triangle function
 * Otherwise, return NULL.
 */
static swrast_tri_func dmesa_choose_tri_function (GLcontext *ctx)
{
 const SWcontext *swrast = SWRAST_CONTEXT(ctx);

 if (ctx->RenderMode != GL_RENDER)  return (swrast_tri_func) NULL;
 if (ctx->Polygon.SmoothFlag)       return (swrast_tri_func) NULL;
 if (ctx->Texture._EnabledUnits)    return (swrast_tri_func) NULL;

 if (ctx->Light.ShadeModel==GL_SMOOTH
     && swrast->_RasterMask==DEPTH_BIT
     && ctx->Depth.Func==GL_LESS
     && ctx->Depth.Mask==GL_TRUE
     && ctx->Visual.depthBits == DEFAULT_SOFTWARE_DEPTH_BITS
     && ctx->Polygon.StippleFlag==GL_FALSE) {
    return tri_rgb_smooth_z;
 }
 if (ctx->Light.ShadeModel==GL_FLAT
     && swrast->_RasterMask==DEPTH_BIT
     && ctx->Depth.Func==GL_LESS
     && ctx->Depth.Mask==GL_TRUE
     && ctx->Visual.depthBits == DEFAULT_SOFTWARE_DEPTH_BITS
     && ctx->Polygon.StippleFlag==GL_FALSE) {
    return tri_rgb_flat_z;
 }
 if (swrast->_RasterMask==0   /* no depth test */
     && ctx->Light.ShadeModel==GL_SMOOTH
     && ctx->Polygon.StippleFlag==GL_FALSE) {
    return tri_rgb_smooth;
 }
 if (swrast->_RasterMask==0   /* no depth test */
     && ctx->Light.ShadeModel==GL_FLAT
     && ctx->Polygon.StippleFlag==GL_FALSE) {
    return tri_rgb_flat;
 }

 return (swrast_tri_func)NULL;
}



/* Override for the swrast triangle-selection function.  Try to use one
 * of our internal line functions, otherwise fall back to the
 * standard swrast functions.
 */
static void dmesa_choose_tri (GLcontext *ctx)
{
 SWcontext *swrast = SWRAST_CONTEXT(ctx);

 if (!(swrast->Triangle=dmesa_choose_tri_function(ctx)))
    _swrast_choose_triangle(ctx);
}



/****************************************************************************
 * Miscellaneous device driver funcs
 ***************************************************************************/

static void clear_color (GLcontext *ctx, const GLfloat color[4])
{
 GLubyte col[4];
 DMesaContext c = (DMesaContext)ctx->DriverCtx;
 CLAMPED_FLOAT_TO_UBYTE(col[0], color[0]);
 CLAMPED_FLOAT_TO_UBYTE(col[1], color[1]);
 CLAMPED_FLOAT_TO_UBYTE(col[2], color[2]);
 CLAMPED_FLOAT_TO_UBYTE(col[3], color[3]);
 c->ClearColor = vl_mixrgba(col);
}



static void clear (GLcontext *ctx, GLbitfield mask, GLboolean all,
                   GLint x, GLint y, GLint width, GLint height)
{
 const DMesaContext c = (DMesaContext)ctx->DriverCtx;
 const GLuint *colorMask = (GLuint *)&ctx->Color.ColorMask;
 DMesaBuffer b = c->Buffer;

/*
 * Clear the specified region of the buffers indicated by 'mask'
 * using the clear color or index as specified by one of the two
 * functions above.
 * If all==GL_TRUE, clear whole buffer, else just clear region defined
 * by x,y,width,height
 */

 /* we can't handle color or index masking */
 if (*colorMask==0xffffffff) {
    if (mask & DD_BACK_LEFT_BIT) {
       if (all) {
          vl_clear(b->the_window, b->bytes, c->ClearColor);
       } else {
          vl_rect(b->the_window, x, y, width, height, c->ClearColor);
       }
       mask &= ~DD_BACK_LEFT_BIT;
    }
 }

 if (mask) {
    _swrast_Clear(ctx, mask, all, x, y, width, height);
 }
}



static void color_mask (GLcontext *ctx, GLboolean rmask, GLboolean gmask, GLboolean bmask, GLboolean amask)
{
 /*
  * XXX todo - Implements glColorMask()
  */
}



static void set_buffer (GLcontext *ctx, GLframebuffer *colorBuffer, GLuint bufferBit)
{
 /*
  * XXX todo - examine bufferBit and set read/write pointers
  */
}



static void enable (GLcontext *ctx, GLenum pname, GLboolean state)
{
 /*
  *  XXX todo -
  */
}



/*
 * Return the width and height of the current buffer.
 * If anything special has to been done when the buffer/window is
 * resized, do it now.
 */
static void get_buffer_size (GLframebuffer *buffer, GLuint *width, GLuint *height)
{
 DMesaBuffer b = (DMesaBuffer)buffer;

 *width  = b->width;
 *height = b->height;
}



static const GLubyte* get_string (GLcontext *ctx, GLenum name)
{
 switch (name) {
        case GL_RENDERER:
             return (const GLubyte *)"Mesa DJGPP\0port (c) Borca Daniel nov-2002";
        default:
             return NULL;
 }
}



static void finish (GLcontext *ctx)
{
 /*
  * XXX todo - OPTIONAL FUNCTION: implements glFinish if possible
  */
}



static void flush (GLcontext *ctx)
{
 /*
  * XXX todo - OPTIONAL FUNCTION: implements glFlush if possible
  */
}



/****************************************************************************
 * State
 ***************************************************************************/
#define DMESA_NEW_TRIANGLE (_NEW_POLYGON | \
                            _NEW_TEXTURE | \
                            _NEW_LIGHT | \
                            _NEW_DEPTH | \
                            _NEW_RENDERMODE | \
                            _SWRAST_NEW_RASTERMASK)

/* Extend the software rasterizer with our line and triangle
 * functions.
 */
static void dmesa_register_swrast_functions (GLcontext *ctx)
{
 SWcontext *swrast = SWRAST_CONTEXT(ctx);

 swrast->choose_triangle = dmesa_choose_tri;

 swrast->invalidate_triangle |= DMESA_NEW_TRIANGLE;
}



/* Setup pointers and other driver state that is constant for the life
 * of a context.
 */
static void dmesa_init_pointers (GLcontext *ctx)
{
 TNLcontext *tnl;
 struct swrast_device_driver *dd = _swrast_GetDeviceDriverReference(ctx);

 ctx->Driver.GetString = get_string;
 ctx->Driver.GetBufferSize = get_buffer_size;
 ctx->Driver.Flush = flush;
 ctx->Driver.Finish = finish;
    
 /* Software rasterizer pixel paths:
  */
 ctx->Driver.Accum = _swrast_Accum;
 ctx->Driver.Bitmap = _swrast_Bitmap;
 ctx->Driver.Clear = clear;
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

 ctx->Driver.CopyTexImage1D = _swrast_copy_teximage1d;
 ctx->Driver.CopyTexImage2D = _swrast_copy_teximage2d;
 ctx->Driver.CopyTexSubImage1D = _swrast_copy_texsubimage1d;
 ctx->Driver.CopyTexSubImage2D = _swrast_copy_texsubimage2d;
 ctx->Driver.CopyTexSubImage3D = _swrast_copy_texsubimage3d;

 ctx->Driver.CompressedTexImage1D = _mesa_store_compressed_teximage1d;
 ctx->Driver.CompressedTexImage2D = _mesa_store_compressed_teximage2d;
 ctx->Driver.CompressedTexImage3D = _mesa_store_compressed_teximage3d;
 ctx->Driver.CompressedTexSubImage1D = _mesa_store_compressed_texsubimage1d;
 ctx->Driver.CompressedTexSubImage2D = _mesa_store_compressed_texsubimage2d;
 ctx->Driver.CompressedTexSubImage3D = _mesa_store_compressed_texsubimage3d;

 /* Swrast hooks for imaging extensions:
  */
 ctx->Driver.CopyColorTable = _swrast_CopyColorTable;
 ctx->Driver.CopyColorSubTable = _swrast_CopyColorSubTable;
 ctx->Driver.CopyConvolutionFilter1D = _swrast_CopyConvolutionFilter1D;
 ctx->Driver.CopyConvolutionFilter2D = _swrast_CopyConvolutionFilter2D;

 /* Statechange callbacks:
  */
 ctx->Driver.ClearColor = clear_color;
 ctx->Driver.ColorMask = color_mask;
 ctx->Driver.Enable = enable;

 /* Initialize the TNL driver interface:
  */
 tnl = TNL_CONTEXT(ctx);
 tnl->Driver.RunPipeline = _tnl_run_pipeline;

 dd->SetBuffer = set_buffer;
   
 /* Install swsetup for tnl->Driver.Render.*:
  */
 _swsetup_Wakeup(ctx);
}



static void dmesa_update_state (GLcontext *ctx, GLuint new_state)
{
 struct swrast_device_driver *dd = _swrast_GetDeviceDriverReference(ctx);

 /* Propogate statechange information to swrast and swrast_setup
  * modules. The DMesa driver has no internal GL-dependent state.
  */
 _swrast_InvalidateState( ctx, new_state );
 _ac_InvalidateState( ctx, new_state );
 _tnl_InvalidateState( ctx, new_state );
 _swsetup_InvalidateState( ctx, new_state );

 /* RGB(A) span/pixel functions */
 dd->WriteRGBASpan = write_rgba_span;
 dd->WriteRGBSpan = write_rgb_span;
 dd->WriteMonoRGBASpan = write_mono_rgba_span;
 dd->WriteRGBAPixels = write_rgba_pixels;
 dd->WriteMonoRGBAPixels = write_mono_rgba_pixels;
 dd->ReadRGBASpan = read_rgba_span;
 dd->ReadRGBAPixels = read_rgba_pixels;
}



/****************************************************************************
 * DMesa Public API Functions
 ***************************************************************************/

/*
 * The exact arguments to this function will depend on your window system
 */
DMesaVisual DMesaCreateVisual (GLint width, GLint height, GLint colDepth,
                               GLboolean dbFlag, GLint depthSize,
                               GLint stencilSize,
                               GLint accumSize)
{
 DMesaVisual v;
 GLint redBits, greenBits, blueBits, alphaBits;

 char *var = getenv("DMESA_REFRESH");
 int refresh = (var != NULL) ? atoi(var) : 0;

 if (!dbFlag) {
    return NULL;
 }

 alphaBits = 0;
 switch (colDepth) {
        case 15:
             redBits = 5;
             greenBits = 5;
             blueBits = 5;
             break;
        case 16:
             redBits = 5;
             greenBits = 6;
             blueBits = 5;
             break;
        case 32:
             alphaBits = 8;
        case 24:
             redBits = 8;
             greenBits = 8;
             blueBits = 8;
             break;
        default:
             return NULL;
 }

 if (vl_video_init(width, height, colDepth, refresh) != 0) {
    return NULL;
 }

 if ((v=(DMesaVisual)calloc(1, sizeof(struct dmesa_visual))) != NULL) {
    /* Create core visual */
    v->gl_visual = _mesa_create_visual(colDepth>8,		/* rgb */
                                       dbFlag,
                                       GL_FALSE,		/* stereo */
                                       redBits,
                                       greenBits,
                                       blueBits,
                                       alphaBits,
                                       0,			/* indexBits */
                                       depthSize,
                                       stencilSize,
                                       accumSize,		/* accumRed */
                                       accumSize,		/* accumGreen */
                                       accumSize,		/* accumBlue */
                                       alphaBits?accumSize:0,	/* accumAlpha */
                                       1);			/* numSamples */

    v->depth = colDepth;
    v->db_flag = dbFlag;
 }

 return v;
}



void DMesaDestroyVisual (DMesaVisual v)
{
 vl_video_exit();
 _mesa_destroy_visual(v->gl_visual);
 free(v);
}



DMesaBuffer DMesaCreateBuffer (DMesaVisual visual,
                               GLint xpos, GLint ypos,
                               GLint width, GLint height)
{
 DMesaBuffer b;

 if ((b=(DMesaBuffer)calloc(1, sizeof(struct dmesa_buffer))) != NULL) {

    _mesa_initialize_framebuffer(&b->gl_buffer,
                                 visual->gl_visual,
                                 visual->gl_visual->depthBits > 0,
                                 visual->gl_visual->stencilBits > 0,
                                 visual->gl_visual->accumRedBits > 0,
                                 visual->gl_visual->alphaBits > 0);
    b->xpos = xpos;
    b->ypos = ypos;
    b->width = width;
    b->height = height;
    b->bypp = (visual->depth+7)/8;
 }

 return b;
}



void DMesaDestroyBuffer (DMesaBuffer b)
{
 free(b->the_window);
 _mesa_free_framebuffer_data(&b->gl_buffer);
 free(b);
}



DMesaContext DMesaCreateContext (DMesaVisual visual,
                                 DMesaContext share)
{
 DMesaContext c;
 GLboolean direct = GL_FALSE;

 if ((c=(DMesaContext)calloc(1, sizeof(struct dmesa_context))) != NULL) {
    c->gl_ctx = _mesa_create_context(visual->gl_visual,
                                     share ? share->gl_ctx : NULL,
                                     (void *)c, direct);

    _mesa_enable_sw_extensions(c->gl_ctx);
    _mesa_enable_1_3_extensions(c->gl_ctx);

    /* you probably have to do a bunch of other initializations here. */
    c->visual = visual;

    c->gl_ctx->Driver.UpdateState = dmesa_update_state;

    /* Initialize the software rasterizer and helper modules.
     */
    _swrast_CreateContext(c->gl_ctx);
    _ac_CreateContext(c->gl_ctx);
    _tnl_CreateContext(c->gl_ctx);
    _swsetup_CreateContext(c->gl_ctx);
    dmesa_register_swrast_functions(c->gl_ctx);
    dmesa_init_pointers(c->gl_ctx);
 }

 return c;
}



void DMesaDestroyContext (DMesaContext c)
{
 _mesa_destroy_context(c->gl_ctx);
 free(c);
}



GLboolean DMesaViewport (DMesaBuffer b,
                         GLint xpos, GLint ypos,
                         GLint width, GLint height)
{
 void *new_window;

 if ((new_window=vl_sync_buffer(b->the_window, xpos, ypos, width, height)) == NULL) {
    return GL_FALSE;
 } else {
    b->the_window = new_window;
    b->xpos = xpos;
    b->ypos = ypos;
    b->width = width;
    b->height = height;
    b->stride = width * b->bypp;
    b->bytes = b->stride * height;
    return GL_TRUE;
 }
}



/*
 * Make the specified context and buffer the current one.
 */
GLboolean DMesaMakeCurrent (DMesaContext c, DMesaBuffer b)
{
 if ((c != NULL) && (b != NULL)) {
    if (!DMesaViewport(b, b->xpos, b->ypos, b->width, b->height)) {
       return GL_FALSE;
    }

    c->Buffer = b;

    _mesa_make_current(c->gl_ctx, &b->gl_buffer);
    if (c->gl_ctx->Viewport.Width == 0) {
       /* initialize viewport to window size */
       _mesa_Viewport(0, 0, b->width, b->height);
    }
 } else {
    /* Detach */
    _mesa_make_current(NULL, NULL);
 }

 return GL_TRUE;
}



void DMesaSwapBuffers (DMesaBuffer b)
{
 /* copy/swap back buffer to front if applicable */
 GET_CURRENT_CONTEXT(ctx);
 _mesa_notifySwapBuffers(ctx);
 vl_flip(b->the_window, b->stride, b->height);
}
