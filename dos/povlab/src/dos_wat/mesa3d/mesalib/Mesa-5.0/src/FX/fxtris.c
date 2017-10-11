/* $Id: fxtris.c,v 1.21 2002/10/29 20:28:57 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  4.0
 *
 * Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
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

/* Authors:
 *    Keith Whitwell <keith@tungstengraphics.com>
 */

#include "glheader.h"

#ifdef FX

#include "mtypes.h"
#include "macros.h"
#include "colormac.h"

#include "swrast/swrast.h"
#include "swrast_setup/swrast_setup.h"
#include "tnl/t_context.h"
#include "tnl/t_pipeline.h"

#include "fxdrv.h"


/*
 * Subpixel offsets to adjust Mesa's (true) window coordinates to
 * Glide coordinates.  We need these to ensure precise rasterization.
 * Otherwise, we'll fail a bunch of conformance tests.
 */
#define TRI_X_OFFSET    ( 0.0F)
#define TRI_Y_OFFSET    ( 0.0F)
#define LINE_X_OFFSET   ( 0.0F)
#define LINE_Y_OFFSET   ( 0.125F)
#define PNT_X_OFFSET    ( 0.375F)
#define PNT_Y_OFFSET    ( 0.375F)

static void fxRasterPrimitive( GLcontext *ctx, GLenum prim );
static void fxRenderPrimitive( GLcontext *ctx, GLenum prim );

/***********************************************************************
 *          Macros for t_dd_tritmp.h to draw basic primitives          *
 ***********************************************************************/

#define TRI( a, b, c )				\
do {						\
   if (DO_FALLBACK)				\
      fxMesa->draw_tri( fxMesa, a, b, c );	\
   else						\
      grDrawTriangle( a, b, c );	\
} while (0)					\

#define QUAD( a, b, c, d )			\
do {						\
   if (DO_FALLBACK) {				\
      fxMesa->draw_tri( fxMesa, a, b, d );	\
      fxMesa->draw_tri( fxMesa, b, c, d );	\
   } else {					\
      grDrawTriangle( a, b, d );	\
      grDrawTriangle( b, c, d );	\
   }						\
} while (0)

#define LINE( v0, v1 )				\
do {						\
   if (DO_FALLBACK)				\
      fxMesa->draw_line( fxMesa, v0, v1 );	\
   else {					\
      v0->x += LINE_X_OFFSET - TRI_X_OFFSET;	\
      v0->y += LINE_Y_OFFSET - TRI_Y_OFFSET;	\
      v1->x += LINE_X_OFFSET - TRI_X_OFFSET;	\
      v1->y += LINE_Y_OFFSET - TRI_Y_OFFSET;	\
      grDrawLine( v0, v1 );	\
      v0->x -= LINE_X_OFFSET - TRI_X_OFFSET;	\
      v0->y -= LINE_Y_OFFSET - TRI_Y_OFFSET;	\
      v1->x -= LINE_X_OFFSET - TRI_X_OFFSET;	\
      v1->y -= LINE_Y_OFFSET - TRI_Y_OFFSET;	\
   }						\
} while (0)

#define POINT( v0 )				\
do {						\
   if (DO_FALLBACK)				\
      fxMesa->draw_point( fxMesa, v0 );		\
   else {					\
      v0->x += PNT_X_OFFSET - TRI_X_OFFSET;	\
      v0->y += PNT_Y_OFFSET - TRI_Y_OFFSET;	\
      grDrawPoint( v0 );		\
      v0->x -= PNT_X_OFFSET - TRI_X_OFFSET;	\
      v0->y -= PNT_Y_OFFSET - TRI_Y_OFFSET;	\
   }						\
} while (0)


/***********************************************************************
 *              Fallback to swrast for basic primitives                *
 ***********************************************************************/

/* Build an SWvertex from a hardware vertex. 
 *
 * This code is hit only when a mix of accelerated and unaccelerated
 * primitives are being drawn, and only for the unaccelerated
 * primitives.  
 */
static void 
fx_translate_vertex( GLcontext *ctx, const GrVertex *src, SWvertex *dst)
{
   fxMesaContext fxMesa = FX_CONTEXT(ctx);
   GLuint ts0 = fxMesa->tmu_source[0];
   GLuint ts1 = fxMesa->tmu_source[1];
   GLfloat w = 1.0 / src->oow;

   dst->win[0] = src->x;
   dst->win[1] = src->y;
   dst->win[2] = src->ooz;
   dst->win[3] = src->oow;

   dst->color[0] = (GLubyte) src->r;
   dst->color[1] = (GLubyte) src->g;
   dst->color[2] = (GLubyte) src->b;
   dst->color[3] = (GLubyte) src->a;

   dst->texcoord[ts0][0] = fxMesa->inv_s0scale * src->tmuvtx[0].sow * w;
   dst->texcoord[ts0][1] = fxMesa->inv_t0scale * src->tmuvtx[0].tow * w;

   if (fxMesa->stw_hint_state & GR_STWHINT_W_DIFF_TMU0)
      dst->texcoord[ts0][3] = src->tmuvtx[0].oow * w;
   else
      dst->texcoord[ts0][3] = 1.0;

   if (fxMesa->SetupIndex & SETUP_TMU1) {
      dst->texcoord[ts1][0] = fxMesa->inv_s1scale * src->tmuvtx[1].sow * w;
      dst->texcoord[ts1][1] = fxMesa->inv_t1scale * src->tmuvtx[1].tow * w;

      if (fxMesa->stw_hint_state & GR_STWHINT_W_DIFF_TMU1)
	 dst->texcoord[ts1][3] = src->tmuvtx[1].oow * w;
      else
	 dst->texcoord[ts1][3] = 1.0;
   }

   dst->pointSize = ctx->Point._Size;
}


static void 
fx_fallback_tri( fxMesaContext fxMesa, 
		   GrVertex *v0, 
		   GrVertex *v1, 
		   GrVertex *v2 )
{
   GLcontext *ctx = fxMesa->glCtx;
   SWvertex v[3];

   fx_translate_vertex( ctx, v0, &v[0] );
   fx_translate_vertex( ctx, v1, &v[1] );
   fx_translate_vertex( ctx, v2, &v[2] );
   _swrast_Triangle( ctx, &v[0], &v[1], &v[2] );
}


static void 
fx_fallback_line( fxMesaContext fxMesa,
		    GrVertex *v0,
		    GrVertex *v1 )
{
   GLcontext *ctx = fxMesa->glCtx;
   SWvertex v[2];
   fx_translate_vertex( ctx, v0, &v[0] );
   fx_translate_vertex( ctx, v1, &v[1] );
   _swrast_Line( ctx, &v[0], &v[1] );
}


static void 
fx_fallback_point( fxMesaContext fxMesa, 
		     GrVertex *v0 )
{
   GLcontext *ctx = fxMesa->glCtx;
   SWvertex v[1];
   fx_translate_vertex( ctx, v0, &v[0] );
   _swrast_Point( ctx, &v[0] );
}

/***********************************************************************
 *                 Functions to draw basic primitives                  *
 ***********************************************************************/

static void fx_print_vertex( GLcontext *ctx, const GrVertex *v )
{
   fprintf(stderr, "vertex at %p\n", (void *) v);

   fprintf(stderr, "x %f y %f z %f oow %f\n", 
	   v->x, v->y, v->ooz, v->oow);
   fprintf(stderr, "r %f g %f b %f a %f\n", 
	   v->r,
	   v->g,
	   v->b,
	   v->a);
   
   fprintf(stderr, "\n");
}

#define DO_FALLBACK 0

/* Need to do clip loop at each triangle when mixing swrast and hw
 * rendering.  These functions are only used when mixed-mode rendering
 * is occurring.
 */
static void fx_draw_quad( fxMesaContext fxMesa,
			  GrVertex *v0,
			  GrVertex *v1,
			  GrVertex *v2,
			  GrVertex *v3 )
{
   QUAD( v0, v1, v2, v3 );
}

static void fx_draw_triangle( fxMesaContext fxMesa,
				GrVertex *v0,
				GrVertex *v1,
				GrVertex *v2 )
{
   TRI( v0, v1, v2 );
}

static void fx_draw_line( fxMesaContext fxMesa,
			    GrVertex *v0,
			    GrVertex *v1 )
{
   /* No support for wide lines (avoid wide/aa line fallback).
    */
   LINE(v0, v1);
}

static void fx_draw_point( fxMesaContext fxMesa,
			     GrVertex *v0 )
{
   /* No support for wide points.
    */
   POINT( v0 );
}

#undef DO_FALLBACK


#define FX_UNFILLED_BIT    0x1
#define FX_OFFSET_BIT	     0x2
#define FX_TWOSIDE_BIT     0x4
#define FX_FLAT_BIT        0x8
#define FX_FALLBACK_BIT    0x10
#define FX_MAX_TRIFUNC     0x20

static struct {
   points_func	        points;
   line_func		line;
   triangle_func	triangle;
   quad_func		quad;
} rast_tab[FX_MAX_TRIFUNC];

#define DO_FALLBACK (IND & FX_FALLBACK_BIT)
#define DO_OFFSET   (IND & FX_OFFSET_BIT)
#define DO_UNFILLED (IND & FX_UNFILLED_BIT)
#define DO_TWOSIDE  (IND & FX_TWOSIDE_BIT)
#define DO_FLAT     (IND & FX_FLAT_BIT)
#define DO_TRI       1
#define DO_QUAD      1
#define DO_LINE      1
#define DO_POINTS    1
#define DO_FULL_QUAD 1

#define HAVE_RGBA   1
#define HAVE_SPEC   0
#define HAVE_HW_FLATSHADE 0
#define HAVE_BACK_COLORS  0
#define VERTEX GrVertex
#define TAB rast_tab

#define DEPTH_SCALE 1.0
#define UNFILLED_TRI unfilled_tri
#define UNFILLED_QUAD unfilled_quad
#define VERT_X(_v) _v->x
#define VERT_Y(_v) _v->y
#define VERT_Z(_v) _v->ooz
#define AREA_IS_CCW( a ) (a < 0)
#define GET_VERTEX(e) (fxMesa->verts + e)

#define VERT_SET_RGBA( dst, f )                   \
do {						\
   dst->r = (GLfloat)f[0];	\
   dst->g = (GLfloat)f[1];	\
   dst->b = (GLfloat)f[2];	\
   dst->a = (GLfloat)f[3];	\
} while (0)

#define VERT_COPY_RGBA( v0, v1 ) 		\
do {						\
   v0->r = v1->r;				\
   v0->g = v1->g;				\
   v0->b = v1->b;				\
   v0->a = v1->a;				\
} while (0)

#define VERT_SAVE_RGBA( idx )  			\
do {						\
   color[idx][0] = v[idx]->r;			\
   color[idx][1] = v[idx]->g;			\
   color[idx][2] = v[idx]->b;			\
   color[idx][3] = v[idx]->a;			\
} while (0)


#define VERT_RESTORE_RGBA( idx )		\
do {						\
   v[idx]->r = color[idx][0];			\
   v[idx]->g = color[idx][1];			\
   v[idx]->b = color[idx][2];			\
   v[idx]->a = color[idx][3];			\
} while (0)


#define LOCAL_VARS(n)					\
   fxMesaContext fxMesa = FX_CONTEXT(ctx);		\
   GLfloat color[n][4];					\
   (void) color; 



/***********************************************************************
 *            Functions to draw basic unfilled primitives              *
 ***********************************************************************/

#define RASTERIZE(x) if (fxMesa->raster_primitive != x) \
                        fxRasterPrimitive( ctx, x )
#define RENDER_PRIMITIVE fxMesa->render_primitive
#define IND FX_FALLBACK_BIT
#define TAG(x) x
#include "tnl_dd/t_dd_unfilled.h"
#undef IND

/***********************************************************************
 *                 Functions to draw GL primitives                     *
 ***********************************************************************/

#define IND (0)
#define TAG(x) x
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_OFFSET_BIT)
#define TAG(x) x##_offset
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT)
#define TAG(x) x##_twoside
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_OFFSET_BIT)
#define TAG(x) x##_twoside_offset
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_UNFILLED_BIT)
#define TAG(x) x##_unfilled
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_OFFSET_BIT|FX_UNFILLED_BIT)
#define TAG(x) x##_offset_unfilled
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_UNFILLED_BIT)
#define TAG(x) x##_twoside_unfilled
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_OFFSET_BIT|FX_UNFILLED_BIT)
#define TAG(x) x##_twoside_offset_unfilled
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_FALLBACK_BIT)
#define TAG(x) x##_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_OFFSET_BIT|FX_FALLBACK_BIT)
#define TAG(x) x##_offset_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_FALLBACK_BIT)
#define TAG(x) x##_twoside_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_OFFSET_BIT|FX_FALLBACK_BIT)
#define TAG(x) x##_twoside_offset_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_UNFILLED_BIT|FX_FALLBACK_BIT)
#define TAG(x) x##_unfilled_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_OFFSET_BIT|FX_UNFILLED_BIT|FX_FALLBACK_BIT)
#define TAG(x) x##_offset_unfilled_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_UNFILLED_BIT|FX_FALLBACK_BIT)
#define TAG(x) x##_twoside_unfilled_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_OFFSET_BIT|FX_UNFILLED_BIT| \
	     FX_FALLBACK_BIT)
#define TAG(x) x##_twoside_offset_unfilled_fallback
#include "tnl_dd/t_dd_tritmp.h"


/* Fx doesn't support provoking-vertex flat-shading?
 */
#define IND (FX_FLAT_BIT)
#define TAG(x) x##_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_OFFSET_BIT|FX_FLAT_BIT)
#define TAG(x) x##_offset_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_FLAT_BIT)
#define TAG(x) x##_twoside_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_OFFSET_BIT|FX_FLAT_BIT)
#define TAG(x) x##_twoside_offset_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_UNFILLED_BIT|FX_FLAT_BIT)
#define TAG(x) x##_unfilled_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_OFFSET_BIT|FX_UNFILLED_BIT|FX_FLAT_BIT)
#define TAG(x) x##_offset_unfilled_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_UNFILLED_BIT|FX_FLAT_BIT)
#define TAG(x) x##_twoside_unfilled_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_OFFSET_BIT|FX_UNFILLED_BIT|FX_FLAT_BIT)
#define TAG(x) x##_twoside_offset_unfilled_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_FALLBACK_BIT|FX_FLAT_BIT)
#define TAG(x) x##_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_OFFSET_BIT|FX_FALLBACK_BIT|FX_FLAT_BIT)
#define TAG(x) x##_offset_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_FALLBACK_BIT|FX_FLAT_BIT)
#define TAG(x) x##_twoside_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_OFFSET_BIT|FX_FALLBACK_BIT|FX_FLAT_BIT)
#define TAG(x) x##_twoside_offset_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_UNFILLED_BIT|FX_FALLBACK_BIT|FX_FLAT_BIT)
#define TAG(x) x##_unfilled_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_OFFSET_BIT|FX_UNFILLED_BIT|FX_FALLBACK_BIT|FX_FLAT_BIT)
#define TAG(x) x##_offset_unfilled_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_UNFILLED_BIT|FX_FALLBACK_BIT|FX_FLAT_BIT)
#define TAG(x) x##_twoside_unfilled_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (FX_TWOSIDE_BIT|FX_OFFSET_BIT|FX_UNFILLED_BIT| \
	     FX_FALLBACK_BIT|FX_FLAT_BIT)
#define TAG(x) x##_twoside_offset_unfilled_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"


static void init_rast_tab( void )
{
   init();
   init_offset();
   init_twoside();
   init_twoside_offset();
   init_unfilled();
   init_offset_unfilled();
   init_twoside_unfilled();
   init_twoside_offset_unfilled();
   init_fallback();
   init_offset_fallback();
   init_twoside_fallback();
   init_twoside_offset_fallback();
   init_unfilled_fallback();
   init_offset_unfilled_fallback();
   init_twoside_unfilled_fallback();
   init_twoside_offset_unfilled_fallback();

   init_flat();
   init_offset_flat();
   init_twoside_flat();
   init_twoside_offset_flat();
   init_unfilled_flat();
   init_offset_unfilled_flat();
   init_twoside_unfilled_flat();
   init_twoside_offset_unfilled_flat();
   init_fallback_flat();
   init_offset_fallback_flat();
   init_twoside_fallback_flat();
   init_twoside_offset_fallback_flat();
   init_unfilled_fallback_flat();
   init_offset_unfilled_fallback_flat();
   init_twoside_unfilled_fallback_flat();
   init_twoside_offset_unfilled_fallback_flat();
}



/**********************************************************************/
/*            Render whole (indexed) begin/end objects                */
/**********************************************************************/


#define VERT(x) (vertptr + x)

#define RENDER_POINTS( start, count )		\
   for ( ; start < count ; start++)		\
      grDrawPoint( VERT(ELT(start)) );

#define RENDER_LINE( v0, v1 ) \
   grDrawLine( VERT(v0), VERT(v1) )

#define RENDER_TRI( v0, v1, v2 )  \
   grDrawTriangle( VERT(v0), VERT(v1), VERT(v2) )

#define RENDER_QUAD( v0, v1, v2, v3 ) \
   fx_draw_quad( fxMesa, VERT(v0), VERT(v1), VERT(v2), VERT(v3) )

#define INIT(x) fxRenderPrimitive( ctx, x )

#undef LOCAL_VARS
#define LOCAL_VARS						\
    fxMesaContext fxMesa = FX_CONTEXT(ctx);			\
    GrVertex *vertptr = fxMesa->verts;		\
    const GLuint * const elt = TNL_CONTEXT(ctx)->vb.Elts;	\
    (void) elt;

#define RESET_STIPPLE 
#define RESET_OCCLUSION 
#define PRESERVE_VB_DEFS

/* Elts, no clipping.
 */
#undef ELT
#undef TAG
#define TAG(x) fx_##x##_elts
#define ELT(x) elt[x]
#include "tnl_dd/t_dd_rendertmp.h"

/* Verts, no clipping.
 */
#undef ELT
#undef TAG
#define TAG(x) fx_##x##_verts
#define ELT(x) x
#include "tnl_dd/t_dd_rendertmp.h"



/**********************************************************************/
/*                   Render clipped primitives                        */
/**********************************************************************/



static void fxRenderClippedPoly( GLcontext *ctx, const GLuint *elts, 
				   GLuint n )
{
   fxMesaContext fxMesa = FX_CONTEXT(ctx);
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct vertex_buffer *VB = &tnl->vb;
   GLuint prim = fxMesa->render_primitive;

   /* Render the new vertices as an unclipped polygon. 
    */
   {
      GLuint *tmp = VB->Elts;
      VB->Elts = (GLuint *)elts;
      tnl->Driver.Render.PrimTabElts[GL_POLYGON]( ctx, 0, n, 
						  PRIM_BEGIN|PRIM_END );
      VB->Elts = tmp;
   }

   /* Restore the render primitive
    */
   if (prim != GL_POLYGON)
      tnl->Driver.Render.PrimitiveNotify( ctx, prim );
}


static void fxFastRenderClippedPoly( GLcontext *ctx, const GLuint *elts, 
				       GLuint n )
{
   fxMesaContext fxMesa = FX_CONTEXT( ctx );
   GrVertex *vertptr = fxMesa->verts;			
   const GrVertex *start = VERT(elts[0]);
   int i;
   for (i = 2 ; i < n ; i++) {
      grDrawTriangle( start, VERT(elts[i-1]), VERT(elts[i]) );
   }
}

/**********************************************************************/
/*                    Choose render functions                         */
/**********************************************************************/


#define POINT_FALLBACK (DD_POINT_SMOOTH)
#define LINE_FALLBACK (DD_LINE_STIPPLE)
#define TRI_FALLBACK (DD_TRI_SMOOTH | DD_TRI_STIPPLE)
#define ANY_FALLBACK_FLAGS (POINT_FALLBACK | LINE_FALLBACK | TRI_FALLBACK)
#define ANY_RASTER_FLAGS (DD_FLATSHADE | DD_TRI_LIGHT_TWOSIDE | DD_TRI_OFFSET \
			  | DD_TRI_UNFILLED)



void fxDDChooseRenderState(GLcontext *ctx)
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   fxMesaContext fxMesa = FX_CONTEXT(ctx);
   GLuint flags = ctx->_TriangleCaps;
   GLuint index = 0;

/*     fprintf(stderr, "%s\n", __FUNCTION__); */

   if (flags & (ANY_FALLBACK_FLAGS|ANY_RASTER_FLAGS)) {
      if (flags & ANY_RASTER_FLAGS) {
	 if (flags & DD_TRI_LIGHT_TWOSIDE)    index |= FX_TWOSIDE_BIT;
	 if (flags & DD_TRI_OFFSET)	      index |= FX_OFFSET_BIT;
	 if (flags & DD_TRI_UNFILLED)	      index |= FX_UNFILLED_BIT;
	 if (flags & DD_FLATSHADE)	      index |= FX_FLAT_BIT;
      }

      fxMesa->draw_point = fx_draw_point;
      fxMesa->draw_line = fx_draw_line;
      fxMesa->draw_tri = fx_draw_triangle;

      /* Hook in fallbacks for specific primitives.
       *
       *
       */
      if (flags & (POINT_FALLBACK|
		   LINE_FALLBACK|
		   TRI_FALLBACK))
      {
	 if (flags & POINT_FALLBACK)
	    fxMesa->draw_point = fx_fallback_point;

	 if (flags & LINE_FALLBACK)
	    fxMesa->draw_line = fx_fallback_line;

	 if (flags & TRI_FALLBACK) {
/*  	    fprintf(stderr, "tri fallback\n"); */
	    fxMesa->draw_tri = fx_fallback_tri;
	 }

	 index |= FX_FALLBACK_BIT;
      }
   }

/*     fprintf(stderr, "render index %x\n", index); */

   tnl->Driver.Render.Points = rast_tab[index].points;
   tnl->Driver.Render.Line = rast_tab[index].line;
   tnl->Driver.Render.ClippedLine = rast_tab[index].line;
   tnl->Driver.Render.Triangle = rast_tab[index].triangle;
   tnl->Driver.Render.Quad = rast_tab[index].quad;

   if (index == 0) {
      tnl->Driver.Render.PrimTabVerts = fx_render_tab_verts;
      tnl->Driver.Render.PrimTabElts = fx_render_tab_elts;
      tnl->Driver.Render.ClippedPolygon = fxFastRenderClippedPoly;

      tnl->Driver.Render.ClippedPolygon = fxRenderClippedPoly;

   } else {
      tnl->Driver.Render.PrimTabVerts = _tnl_render_tab_verts;
      tnl->Driver.Render.PrimTabElts = _tnl_render_tab_elts;
      tnl->Driver.Render.ClippedPolygon = fxRenderClippedPoly;
   }
}


/**********************************************************************/
/*                Runtime render state and callbacks                  */
/**********************************************************************/


static GLenum reduced_prim[GL_POLYGON+1] = {
   GL_POINTS,
   GL_LINES,
   GL_LINES,
   GL_LINES,
   GL_TRIANGLES,
   GL_TRIANGLES,
   GL_TRIANGLES,
   GL_TRIANGLES,
   GL_TRIANGLES,
   GL_TRIANGLES
};



/* Always called between RenderStart and RenderFinish --> We already
 * hold the lock.
 */
static void fxRasterPrimitive( GLcontext *ctx, GLenum prim )
{
   fxMesaContext fxMesa = FX_CONTEXT( ctx );

   fxMesa->raster_primitive = prim;
   if (prim == GL_TRIANGLES)
      grCullMode( fxMesa->cullMode );
   else
      grCullMode( GR_CULL_DISABLE );
}



/* Determine the rasterized primitive when not drawing unfilled 
 * polygons.
 */
static void fxRenderPrimitive( GLcontext *ctx, GLenum prim )
{
   fxMesaContext fxMesa = FX_CONTEXT(ctx);
   GLuint rprim = reduced_prim[prim];

   fxMesa->render_primitive = prim;

   if (rprim == GL_TRIANGLES && (ctx->_TriangleCaps & DD_TRI_UNFILLED))
      return;
       
   if (fxMesa->raster_primitive != rprim) {
      fxRasterPrimitive( ctx, rprim );
   }
}



/**********************************************************************/
/*               Manage total rasterization fallbacks                 */
/**********************************************************************/


void fxCheckIsInHardware( GLcontext *ctx )
{
   fxMesaContext fxMesa = (fxMesaContext) ctx->DriverCtx;
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   GLuint oldfallback = !fxMesa->is_in_hardware;
   GLuint newfallback;

   fxMesa->is_in_hardware = fx_check_IsInHardware( ctx );
   newfallback = !fxMesa->is_in_hardware;

   if (newfallback) {
      if (oldfallback == 0) {
/*  	 fprintf(stderr, "goint to fallback\n"); */
	 _swsetup_Wakeup( ctx );
      }
   }
   else {
      if (oldfallback) {
/*  	 fprintf(stderr, "leaving fallback\n"); */
	 _swrast_flush( ctx );
	 tnl->Driver.Render.Start = fxCheckTexSizes;
	 tnl->Driver.Render.Finish = _swrast_flush;
	 tnl->Driver.Render.PrimitiveNotify = fxRenderPrimitive;
	 tnl->Driver.Render.ClippedPolygon = _tnl_RenderClippedPolygon; 
	 tnl->Driver.Render.ClippedLine = _tnl_RenderClippedLine;
	 tnl->Driver.Render.PrimTabVerts = _tnl_render_tab_verts;
	 tnl->Driver.Render.PrimTabElts = _tnl_render_tab_elts;
	 tnl->Driver.Render.ResetLineStipple = _swrast_ResetLineStipple;
	 tnl->Driver.Render.BuildVertices = fxBuildVertices;
	 tnl->Driver.Render.Multipass = 0;
	 fxChooseVertexState(ctx);
	 fxDDChooseRenderState(ctx);
      }
   }
}

void fxDDInitTriFuncs( GLcontext *ctx )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   static int firsttime = 1;

   if (firsttime) {
      init_rast_tab();
      firsttime = 0;
   }

   tnl->Driver.Render.Start = fxCheckTexSizes;
   tnl->Driver.Render.Finish = _swrast_flush;
   tnl->Driver.Render.PrimitiveNotify = fxRenderPrimitive;
   tnl->Driver.Render.ClippedPolygon = _tnl_RenderClippedPolygon; 
   tnl->Driver.Render.ClippedLine = _tnl_RenderClippedLine;
   tnl->Driver.Render.PrimTabVerts = _tnl_render_tab_verts;
   tnl->Driver.Render.PrimTabElts = _tnl_render_tab_elts;
   tnl->Driver.Render.ResetLineStipple = _swrast_ResetLineStipple;
   tnl->Driver.Render.BuildVertices = fxBuildVertices;
   tnl->Driver.Render.Multipass = 0;
   
   (void) fx_print_vertex;
}


#else


/*
 * Need this to provide at least one external definition.
 */

extern int gl_fx_dummy_function_tris(void);
int
gl_fx_dummy_function_tris(void)
{
   return 0;
}

#endif /* FX */
