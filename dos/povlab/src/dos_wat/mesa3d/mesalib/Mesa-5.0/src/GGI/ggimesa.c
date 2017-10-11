/* GGI-Driver for MESA
 * 
 * Copyright (C) 1997-1998  Uwe Maurer  -  uwe_maurer@t-online.de 
 *                    2002  Filip Spacek
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ---------------------------------------------------------------------
 * This code was derived from the following source of information:
 *
 * svgamesa.c and ddsample.c by Brian Paul
 * 
 */

#ifdef HAVE_CONFIG_H
#include "conf.h"
#endif

#include <ggi/mesa/ggimesa_int.h>
#include <ggi/mesa/debug.h>
#include "extensions.h"
#include "colormac.h"
#include "imports.h"
#include "matrix.h"
#include "swrast/swrast.h"
#include "swrast_setup/swrast_setup.h"
#include "tnl/tnl.h"
#include "tnl/t_context.h"
#include "tnl/t_pipeline.h"
#include "array_cache/acache.h"
#include "texformat.h"
#include "texstore.h"

ggi_extid ggiMesaID = -1;
static int _ggimesaLibIsUp = 0;
static void *_ggimesaConfigHandle;

static char ggimesaconffile[] = GGIMESACONFFILE;

int _ggimesaDebugSync = 0;
uint32 _ggimesaDebugState = 0;

static void gl_ggiUpdateState(GLcontext *ctx, GLuint new_state);
static int changed(ggi_visual_t vis, int whatchanged);


static int _ggi_error(void)
{
	GGIMESADPRINT_CORE("_ggi_error() called\n");
	
	return -1;
}

static void gl_ggiGetSize(GLframebuffer *fb, GLuint *width, GLuint *height)
{
	/* FIXME: this is a hack to work around the new interface */
	GLcontext *ctx;
	ggi_mesa_context_t ggi_ctx;
	ctx = _mesa_get_current_context();
	ggi_ctx = (ggi_mesa_context_t)ctx->DriverCtx;
	
	GGIMESADPRINT_CORE("gl_ggiGetSize() called\n");
	
	*width = LIBGGI_MODE(ggi_ctx->ggi_visual)->visible.x; 
	*height = LIBGGI_MODE(ggi_ctx->ggi_visual)->visible.y;
	printf("returning %d, %d\n", *width, *height);
}

static void gl_ggiSetIndex(GLcontext *ctx, GLuint ci)
{
	ggi_mesa_context_t ggi_ctx = (ggi_mesa_context_t)ctx->DriverCtx;
	
	GGIMESADPRINT_CORE("gl_ggiSetIndex() called\n");
	
	ggiSetGCForeground(ggi_ctx->ggi_visual, ci);
	ggi_ctx->color = (ggi_pixel)ci;
}

static void gl_ggiSetClearIndex(GLcontext *ctx, GLuint ci)
{	
	ggi_mesa_context_t ggi_ctx = (ggi_mesa_context_t)ctx->DriverCtx;
	
	GGIMESADPRINT_CORE("gl_ggiSetClearIndex() called\n");
	
	ggiSetGCForeground(ggi_ctx->ggi_visual, ci);
	ggi_ctx->clearcolor = (ggi_pixel)ci;
}

static void gl_ggiSetClearColor(GLcontext *ctx, const GLfloat color[4])
{
	ggi_mesa_context_t ggi_ctx = (ggi_mesa_context_t)ctx->DriverCtx;
	ggi_color rgb;
	ggi_pixel col;
	GLubyte byteColor[3];

	GGIMESADPRINT_CORE("gl_ggiSetClearColor() called\n");
	
	CLAMPED_FLOAT_TO_UBYTE(byteColor[0], color[0]);
	CLAMPED_FLOAT_TO_UBYTE(byteColor[1], color[1]);
	CLAMPED_FLOAT_TO_UBYTE(byteColor[2], color[2]);

	rgb.r = (uint16)byteColor[0] << SHIFT;
	rgb.g = (uint16)byteColor[1] << SHIFT;
	rgb.b = (uint16)byteColor[2] << SHIFT;
	col = ggiMapColor(ggi_ctx->ggi_visual, &rgb);
	ggiSetGCForeground(ggi_ctx->ggi_visual, col);
	ggi_ctx->clearcolor = col;
}

static void gl_ggiClear(GLcontext *ctx, GLbitfield mask, GLboolean all,
			GLint x, GLint y, GLint width, GLint height)
{
	ggi_mesa_context_t ggi_ctx = (ggi_mesa_context_t)ctx->DriverCtx;
	
	GGIMESADPRINT_CORE("gl_ggiClear() called\n");

	if (mask & (DD_FRONT_LEFT_BIT | DD_BACK_LEFT_BIT)) 
	{
		ggiSetGCForeground(ggi_ctx->ggi_visual, ggi_ctx->clearcolor);

		if (all)
		{
			int w, h;
			w = LIBGGI_MODE(ggi_ctx->ggi_visual)->visible.x;
			h = LIBGGI_MODE(ggi_ctx->ggi_visual)->visible.y;
			ggiDrawBox(ggi_ctx->ggi_visual, 0, 0, w, h);
		}
		else
		{
			ggiDrawBox(ggi_ctx->ggi_visual, x, y, //FLIP(y),
				   width, height);
		}
		ggiSetGCForeground(ggi_ctx->ggi_visual, ggi_ctx->color);

		mask &= ~(DD_FRONT_LEFT_BIT | DD_BACK_LEFT_BIT);
	}
	_swrast_Clear(ctx, mask, all, x, y, width, height);
	
}


/* Set the buffer used for reading */
/* XXX support for separate read/draw buffers hasn't been tested */
static GLboolean gl_ggiSetBuffer(GLcontext *ctx, GLframebuffer *buffer, GLuint bufferBit)
{
	ggi_mesa_context_t ggi_ctx = (ggi_mesa_context_t)ctx->DriverCtx;
	
	printf("set read %d\n", bufferBit);
	GGIMESADPRINT_CORE("gl_ggiSetBuffer() called\n");

	if (bufferBit == FRONT_LEFT_BIT) 
	{
		ggiSetReadFrame(ggi_ctx->ggi_visual,
				ggiGetDisplayFrame(ggi_ctx->ggi_visual));
		ggiSetWriteFrame(ggi_ctx->ggi_visual,
				 ggiGetDisplayFrame(ggi_ctx->ggi_visual));
		return GL_TRUE;
	}
	else if (bufferBit == BACK_LEFT_BIT)
	{
		ggiSetReadFrame(ggi_ctx->ggi_visual,
				ggiGetDisplayFrame(ggi_ctx->ggi_visual)?0 : 1);
		ggiSetWriteFrame(ggi_ctx->ggi_visual,
				 ggiGetDisplayFrame(ggi_ctx->ggi_visual)?0 : 1);
		return GL_TRUE;
	}
	else
		return GL_FALSE;
}


static const GLubyte * gl_ggiGetString(GLcontext *ctx, GLenum name)
{
	GGIMESADPRINT_CORE("gl_ggiGetString() called\n");
	
	if (name == GL_RENDERER)
        	return (GLubyte *) "Mesa GGI";
	else
		return NULL;
}

static void gl_ggiFlush(GLcontext *ctx)
{
	ggi_mesa_context_t ggi_ctx = (ggi_mesa_context_t)ctx->DriverCtx;

	GGIMESADPRINT_CORE("gl_ggiFlush() called\n");
	
	ggiFlush(ggi_ctx->ggi_visual);
}

static void gl_ggiIndexMask(GLcontext *ctx, GLuint mask)
{
	GGIMESADPRINT_CORE("gl_ggiIndexMask() called\n");
}

static void gl_ggiColorMask(GLcontext *ctx, GLboolean rmask, GLboolean gmask,
			    GLboolean bmask, GLboolean amask)
{
	GGIMESADPRINT_CORE("gl_ggiColorMask() called\n");
}

static void gl_ggiEnable(GLcontext *ctx, GLenum pname, GLboolean state)
{
	GGIMESADPRINT_CORE("gl_ggiEnable() called\n");
}

static void gl_ggiSetupPointers(GLcontext *ctx)
{
	TNLcontext *tnl;
  
	GGIMESADPRINT_CORE("gl_ggiSetupPointers() called\n");

	/* General information */
	ctx->Driver.GetString = gl_ggiGetString;
	ctx->Driver.GetBufferSize = gl_ggiGetSize;
	ctx->Driver.Finish = gl_ggiFlush;
	ctx->Driver.Flush = gl_ggiFlush;
	
	/* Software rasterizer pixel paths */
	ctx->Driver.Accum = _swrast_Accum;
	ctx->Driver.Bitmap = _swrast_Bitmap;
	ctx->Driver.Clear = gl_ggiClear;
	ctx->Driver.ResizeBuffers = _swrast_alloc_buffers;
	ctx->Driver.CopyPixels = _swrast_CopyPixels;
	ctx->Driver.DrawPixels = _swrast_DrawPixels;
	ctx->Driver.ReadPixels = _swrast_ReadPixels;
        ctx->Driver.DrawBuffer = _swrast_DrawBuffer;

	/* Software texturing */
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

	/* Imaging extensions */
	ctx->Driver.CopyColorTable = _swrast_CopyColorTable;
	ctx->Driver.CopyColorSubTable = _swrast_CopyColorSubTable;
	ctx->Driver.CopyConvolutionFilter1D = _swrast_CopyConvolutionFilter1D;
	ctx->Driver.CopyConvolutionFilter2D = _swrast_CopyConvolutionFilter2D;
	
	/* State change callbacks */
	ctx->Driver.ClearIndex = gl_ggiSetClearIndex; 
	ctx->Driver.ClearColor = gl_ggiSetClearColor;
	ctx->Driver.IndexMask = gl_ggiIndexMask;
	ctx->Driver.ColorMask = gl_ggiColorMask;
	ctx->Driver.Enable = gl_ggiEnable;
	
	ctx->Driver.UpdateState = gl_ggiUpdateState;

	/* Initialize TNL driver interface */
	tnl = TNL_CONTEXT(ctx);
	tnl->Driver.RunPipeline = _tnl_run_pipeline;
	
	/* Install setup for tnl */
	_swsetup_Wakeup(ctx);
}

static void get_mode_info(ggi_visual_t vis, int *r, int *g, int *b,
			  GLboolean *rgb, GLboolean *db, int *ci)
{
	int i;
	
	*r = 0;
	*g = 0;
	*b = 0;

	for(i = 0; i < sizeof(ggi_pixel)*8; ++i){
		int mask = 1 << i;
		if(LIBGGI_PIXFMT(vis)->red_mask & mask)
			++(*r);
		if(LIBGGI_PIXFMT(vis)->green_mask & mask)
			++(*g);
		if(LIBGGI_PIXFMT(vis)->blue_mask & mask)
			++(*b);
	}

	*rgb = GT_SCHEME(LIBGGI_MODE(vis)->graphtype) == GT_TRUECOLOR;
	*db = LIBGGI_MODE(vis)->frames > 1;
	*ci = GT_SIZE(LIBGGI_MODE(vis)->graphtype);

	printf("rgb (%d, %d, %d) db %d, rgb %d ci %d\n",*r,*g,*b,*db,*rgb,*ci);
}
	
int ggiMesaInit()
{
	int err;
	char *str;
	
	GGIMESADPRINT_CORE("ggiMesaInit() called\n");
	
        str = getenv("GGIMESA_DEBUG");
	if (str != NULL) {
		_ggimesaDebugState = atoi(str);
		GGIMESADPRINT_CORE("Debugging=%d\n", _ggimesaDebugState);
	}
	
	str = getenv("GGIMESA_DEBUGSYNC");
	if (str != NULL) {
		_ggimesaDebugSync = 1;
	}
	
	GGIMESADPRINT_CORE("ggiMesaInit()\n");
	
	_ggimesaLibIsUp++;
	if (_ggimesaLibIsUp > 1)
		return 0; /* Initialize only at first call */
	
	err = ggLoadConfig(ggimesaconffile, &_ggimesaConfigHandle);
	if (err != GGI_OK)
	{
		GGIMESADPRINT_CORE("GGIMesa: Couldn't open %s\n",
				   ggimesaconffile);
		_ggimesaLibIsUp--;
		return err;
	}
	
	ggiMesaID = ggiExtensionRegister("GGIMesa",
					 sizeof(struct ggi_mesa_ext), changed);
	
	if (ggiMesaID < 0)
	{
		GGIMESADPRINT_CORE("GGIMesa: failed to register as extension\n");
		_ggimesaLibIsUp--;
		ggFreeConfig(_ggimesaConfigHandle);
		return ggiMesaID;
	}
	
	return 0;
}

int ggiMesaExit(void)
{
	int rc;
	
	GGIMESADPRINT_CORE("ggiMesaExit() called\n");
	
	if (!_ggimesaLibIsUp)
		return -1;
	
	if (_ggimesaLibIsUp > 1)
	{
		/* Exit only at last call */
		_ggimesaLibIsUp--;
		return 0;
	}
	
	rc = ggiExtensionUnregister(ggiMesaID);
	ggFreeConfig(_ggimesaConfigHandle);
	
	_ggimesaLibIsUp = 0;
	
	return rc;
}

int ggiMesaAttach(ggi_visual_t vis)
{
	int rc;
	
	GGIMESADPRINT_CORE("ggiMesaAttach() called\n");
	
	rc = ggiExtensionAttach(vis, ggiMesaID);
	if (rc == 0)
	{
		int r, g, b, ci;
		GLboolean rgb, db;
		GLvisual *gl_visual;
		GLframebuffer *gl_fb;
		
		/* We are creating the primary instance */
		memset(LIBGGI_MESAEXT(vis), 0, sizeof(struct ggi_mesa_ext));
		LIBGGI_MESAEXT(vis)->update_state = (void *)_ggi_error;
		LIBGGI_MESAEXT(vis)->setup_driver = (void *)_ggi_error;

		/* Initialize default mesa visual */
		get_mode_info(vis, &r, &g, &b, &rgb, &db, &ci);
		gl_visual = &(LIBGGI_MESAEXT(vis)->mesa_visual.gl_visual);
		_mesa_initialize_visual(gl_visual,
					rgb, db, 0 /* No stereo */,
					r, g, b, 0 /* No alpha */, ci,
					0 /* No depth */, 0 /* No stencil */,
					0, 0, 0, 0 /* No accum */, 0);
		
		/* Now fake an "API change" so the right libs get loaded */
		changed(vis, GGI_CHG_APILIST);
	}
	
	return rc;
}

int ggiMesaDetach(ggi_visual_t vis)
{
	GGIMESADPRINT_CORE("ggiMesaDetach() called\n");
	
	return ggiExtensionDetach(vis, ggiMesaID);
}
 
int ggiMesaExtendVisual(ggi_visual_t vis, GLboolean alpha_flag,
			GLboolean stereo_flag, GLint depth_size,
			GLint stencil_size, GLint accum_red_size,
			GLint accum_green_size, GLint accum_blue_size,
			GLint accum_alpha_size, GLint num_samples)
{
        GLvisual *gl_vis = &(LIBGGI_MESAEXT(vis)->mesa_visual.gl_visual);
	int r, g, b, ci;
	GLboolean db, rgb;

	get_mode_info(vis, &r, &g, &b, &rgb, &db, &ci);
       
	/* Initialize the visual with the provided information */	
	_mesa_initialize_visual(&(LIBGGI_MESAEXT(vis)->mesa_visual.gl_visual),
				rgb, db, stereo_flag,
				r, g, b, 0 /* FIXME */, ci,
				depth_size, stencil_size,
				accum_red_size, accum_green_size,
				accum_blue_size, accum_alpha_size, 0);

	/* Now fake an "API change" so the right libs get loaded. After all,
	   extending the visual by all these new buffers could be considered
	   a "mode change" which requires an "API change".
	 */
	changed(vis, GGI_CHG_APILIST);
	
	return 0;
}


ggi_mesa_context_t ggiMesaCreateContext(ggi_visual_t vis)
{
	ggi_mesa_context_t ctx;
	int err;
	ggi_color pal[256];
	int i;

	GGIMESADPRINT_CORE("ggiMesaCreateContext() called\n");
	
	ctx = (ggi_mesa_context_t)malloc(sizeof(struct ggi_mesa_context));
	if (!ctx) 
		return NULL;
	
	ctx->ggi_visual = vis;
	ctx->color = 0;

	ctx->gl_ctx =
	  _mesa_create_context(&(LIBGGI_MESAEXT(vis)->mesa_visual.gl_visual),
			       NULL, (void *) ctx, GL_FALSE);
	if (!ctx->gl_ctx)
		goto free_context;
	
        _mesa_enable_sw_extensions(ctx->gl_ctx);
	
	_swrast_CreateContext(ctx->gl_ctx);
	_ac_CreateContext(ctx->gl_ctx);
	_tnl_CreateContext(ctx->gl_ctx);
	_swsetup_CreateContext(ctx->gl_ctx);
	
	gl_ggiSetupPointers(ctx->gl_ctx);

	/* Make sure that an appropriate sublib has been loaded */
	if (!LIBGGI_MESAEXT(ctx->ggi_visual)->setup_driver){
		GGIMESADPRINT_CORE("setup_driver==NULL!\n");
		GGIMESADPRINT_CORE("Please check your config files!\n");
		goto free_context;
	}

	/* Set up the sublib driver */
	err = LIBGGI_MESAEXT(ctx->ggi_visual)->setup_driver(ctx);
	if (err){
		GGIMESADPRINT_CORE("setup_driver failed (err = %d)", err);
		goto free_gl_context;
	}

	return ctx;
	
free_gl_context:
	_mesa_destroy_context(ctx->gl_ctx);
free_context:
	free(ctx);
	
	return NULL;
}

void ggiMesaDestroyContext(ggi_mesa_context_t ctx)
{
	GGIMESADPRINT_CORE("ggiMesaDestroyContext() called\n");
	
	if(!ctx)
		return;

	_mesa_destroy_context(ctx->gl_ctx);
	free(ctx);
}

void ggiMesaMakeCurrent(ggi_mesa_context_t ctx, ggi_visual_t vis)
{
	GGIMESADPRINT_CORE("ggiMesaMakeCurrent(ctx = %p) called\n", ctx);

	/* FIXME: clean up where are ggi_vis */
	if (ctx->ggi_visual != vis) {
		GGIMESADPRINT_CORE("Cannot migrate GL contexts\n");
		return;
	}
	
	_mesa_make_current(ctx->gl_ctx, &LIBGGI_MESAEXT(vis)->mesa_buffer);
	
	if (ctx->gl_ctx->Viewport.Width == 0)
	{
		_mesa_Viewport(0, 0,
			       LIBGGI_MODE(vis)->visible.x,
			       LIBGGI_MODE(vis)->visible.y);
		ctx->gl_ctx->Scissor.Width = LIBGGI_MODE(vis)->visible.x;
		ctx->gl_ctx->Scissor.Height = LIBGGI_MODE(vis)->visible.y;
	}
}


/*
 * Swap front/back buffers for current context if double buffered.
 */
void ggiMesaSwapBuffers(void)
{
	GLcontext *ctx;
	ggi_mesa_context_t ggi_ctx;
	ctx = _mesa_get_current_context();
	ggi_ctx = (ggi_mesa_context_t)ctx->DriverCtx;
	
	GGIMESADPRINT_CORE("ggiMesaSwapBuffers() called\n");	
	
	_mesa_notifySwapBuffers(ctx);
	gl_ggiFlush(ctx);

	ggiSetDisplayFrame(ggi_ctx->ggi_visual,
			   !ggiGetDisplayFrame(ggi_ctx->ggi_visual));
	ggiSetWriteFrame(ggi_ctx->ggi_visual,
			 !ggiGetWriteFrame(ggi_ctx->ggi_visual));
	ggiSetReadFrame(ggi_ctx->ggi_visual,
			 !ggiGetReadFrame(ggi_ctx->ggi_visual));

	GGIMESADPRINT_CORE("swap disp: %d, write %d\n",
			   ggiGetDisplayFrame(ggi_ctx->ggi_visual),
			   ggiGetWriteFrame(ggi_ctx->ggi_visual));
}

static void gl_ggiUpdateState(GLcontext *ctx, GLuint new_state)
{
	ggi_mesa_context_t ggi_ctx = (ggi_mesa_context_t)ctx->DriverCtx;
	
	GGIMESADPRINT_CORE("gl_ggiUpdateState() called\n");
		
	/* Propogate statechange information to swrast and swrast_setup
	 * modules.  The GGI driver has no internal GL-dependent state.
	 */
	_swrast_InvalidateState(ctx, new_state);
	_swsetup_InvalidateState(ctx, new_state);
	_tnl_InvalidateState(ctx, new_state);
	
	if (!LIBGGI_MESAEXT(ggi_ctx->ggi_visual)->update_state) {
		GGIMESADPRINT_CORE("update_state == NULL!\n");
		GGIMESADPRINT_CORE("Please check your config files!\n");
		ggiPanic("");
	}

	LIBGGI_MESAEXT(ggi_ctx->ggi_visual)->update_state(ggi_ctx);
}

static int changed(ggi_visual_t vis, int whatchanged)
{
	GLcontext *ctx;
	ctx = _mesa_get_current_context();

	GGIMESADPRINT_CORE("changed() called\n");
		
	switch (whatchanged)
	{
	case GGI_CHG_APILIST:
	{
		char api[256];
		char args[256];
		int i;
		const char *fname;
		ggi_dlhandle *lib;
		GLvisual *gl_vis=&(LIBGGI_MESAEXT(vis)->mesa_visual.gl_visual);
		GLframebuffer *gl_fb = &(LIBGGI_MESAEXT(vis)->mesa_buffer);
		
		/* Initialize the framebuffer to provide all necessary
		   buffers in software. The target libraries that are loaded
		   next are free to modify this according to their
		   capabilities. 
		 */
		 /* FIXME: if the target changes capabilities we'll leak 
		    swrast's memory !!! Need to deallocate first */
		_mesa_initialize_framebuffer(gl_fb, gl_vis,
					 gl_vis->depthBits > 0,
					 gl_vis->stencilBits > 0,
					 gl_vis->accumRedBits > 0,
					 gl_vis->alphaBits > 0);

		for (i = 0; ggiGetAPI(vis, i, api, args) == 0; i++)
		{
			strcat(api, "-mesa");
			fname = ggMatchConfig(_ggimesaConfigHandle, api, NULL);
			if (fname == NULL)
			{
				/* No special implementation for this sublib */
				continue;
			}
			lib = ggiExtensionLoadDL(vis, fname, args, NULL,
						 GGI_SYMNAME_PREFIX);
		}

		/* The targets have cleared everything they can do from 
		   the framebuffer structure so we provide the rest in sw
		 */
		_swrast_alloc_buffers(gl_fb);
		
		break;
	} 
	}
	return 0;
}

