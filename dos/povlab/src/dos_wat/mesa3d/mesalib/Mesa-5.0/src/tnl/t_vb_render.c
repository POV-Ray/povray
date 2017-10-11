/* $Id: t_vb_render.c,v 1.33 2002/10/29 20:29:04 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  4.0.1
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
 *
 * Authors:
 *    Keith Whitwell <keith@tungstengraphics.com>
 */


/*
 * Render whole vertex buffers, including projection of vertices from
 * clip space and clipping of primitives.
 *
 * This file makes calls to project vertices and to the point, line
 * and triangle rasterizers via the function pointers:
 *
 *    context->Driver.Render.*
 *
 */


#include "glheader.h"
#include "context.h"
#include "enums.h"
#include "macros.h"
#include "imports.h"
#include "mtypes.h"
#include "mmath.h"

#include "math/m_matrix.h"
#include "math/m_xform.h"

#include "t_pipeline.h"



/**********************************************************************/
/*                        Clip single primitives                      */
/**********************************************************************/


#if defined(USE_IEEE)
#define NEGATIVE(x) (GET_FLOAT_BITS(x) & (1<<31))
#define DIFFERENT_SIGNS(x,y) ((GET_FLOAT_BITS(x) ^ GET_FLOAT_BITS(y)) & (1<<31))
#else
#define NEGATIVE(x) (x < 0)
#define DIFFERENT_SIGNS(x,y) (x * y <= 0 && x - y != 0)
/* Could just use (x*y<0) except for the flatshading requirements.
 * Maybe there's a better way?
 */
#endif


#define W(i) coord[i][3]
#define Z(i) coord[i][2]
#define Y(i) coord[i][1]
#define X(i) coord[i][0]
#define SIZE 4
#define TAG(x) x##_4
#include "t_vb_cliptmp.h"



/**********************************************************************/
/*              Clip and render whole begin/end objects               */
/**********************************************************************/

#define NEED_EDGEFLAG_SETUP (ctx->_TriangleCaps & DD_TRI_UNFILLED)
#define EDGEFLAG_GET(idx) VB->EdgeFlag[idx]
#define EDGEFLAG_SET(idx, val) VB->EdgeFlag[idx] = val


/* Vertices, with the possibility of clipping.
 */
#define RENDER_POINTS( start, count ) \
   tnl->Driver.Render.Points( ctx, start, count )

#define RENDER_LINE( v1, v2 )			\
do {						\
   GLubyte c1 = mask[v1], c2 = mask[v2];	\
   GLubyte ormask = c1|c2;			\
   if (!ormask)					\
      LineFunc( ctx, v1, v2 );			\
   else if (!(c1 & c2 & 0x3f))			\
      clip_line_4( ctx, v1, v2, ormask );	\
} while (0)

#define RENDER_TRI( v1, v2, v3 )			\
do {							\
   GLubyte c1 = mask[v1], c2 = mask[v2], c3 = mask[v3];	\
   GLubyte ormask = c1|c2|c3;				\
   if (!ormask)						\
      TriangleFunc( ctx, v1, v2, v3 );			\
   else if (!(c1 & c2 & c3 & 0x3f)) 			\
      clip_tri_4( ctx, v1, v2, v3, ormask );    	\
} while (0)

#define RENDER_QUAD( v1, v2, v3, v4 )			\
do {							\
   GLubyte c1 = mask[v1], c2 = mask[v2];		\
   GLubyte c3 = mask[v3], c4 = mask[v4];		\
   GLubyte ormask = c1|c2|c3|c4;			\
   if (!ormask)						\
      QuadFunc( ctx, v1, v2, v3, v4 );			\
   else if (!(c1 & c2 & c3 & c4 & 0x3f)) 		\
      clip_quad_4( ctx, v1, v2, v3, v4, ormask );	\
} while (0)


#define LOCAL_VARS						\
   TNLcontext *tnl = TNL_CONTEXT(ctx);				\
   struct vertex_buffer *VB = &tnl->vb;				\
   const GLuint * const elt = VB->Elts;				\
   const GLubyte *mask = VB->ClipMask;				\
   const GLuint sz = VB->ClipPtr->size;				\
   const line_func LineFunc = tnl->Driver.Render.Line;		\
   const triangle_func TriangleFunc = tnl->Driver.Render.Triangle;	\
   const quad_func QuadFunc = tnl->Driver.Render.Quad;		\
   const GLboolean stipple = ctx->Line.StippleFlag;		\
   (void) (LineFunc && TriangleFunc && QuadFunc);		\
   (void) elt; (void) mask; (void) sz; (void) stipple;

#define TAG(x) clip_##x##_verts
#define INIT(x) tnl->Driver.Render.PrimitiveNotify( ctx, x )
#define RESET_STIPPLE if (stipple) tnl->Driver.Render.ResetLineStipple( ctx )
#define RESET_OCCLUSION ctx->OcclusionResult = GL_TRUE
#define PRESERVE_VB_DEFS
#include "t_vb_rendertmp.h"



/* Elts, with the possibility of clipping.
 */
#undef ELT
#undef TAG
#define ELT(x) elt[x]
#define TAG(x) clip_##x##_elts
#include "t_vb_rendertmp.h"

/* TODO: do this for all primitives, verts and elts:
 */
static void clip_elt_triangles( GLcontext *ctx,
				GLuint start,
				GLuint count,
				GLuint flags )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   render_func render_tris = tnl->Driver.Render.PrimTabElts[GL_TRIANGLES];
   struct vertex_buffer *VB = &tnl->vb;
   const GLuint * const elt = VB->Elts;
   GLubyte *mask = VB->ClipMask;
   GLuint last = count-2;
   GLuint j;
   (void) flags;

   tnl->Driver.Render.PrimitiveNotify( ctx, GL_TRIANGLES );

   for (j=start; j < last; j+=3 ) {
      GLubyte c1 = mask[elt[j]];
      GLubyte c2 = mask[elt[j+1]];
      GLubyte c3 = mask[elt[j+2]];
      GLubyte ormask = c1|c2|c3;
      if (ormask) {
	 if (start < j)
	    render_tris( ctx, start, j, 0 );
	 if (!(c1&c2&c3&0x3f))
	    clip_tri_4( ctx, elt[j], elt[j+1], elt[j+2], ormask );
	 start = j+3;
      }
   }

   if (start < j)
      render_tris( ctx, start, j, 0 );
}

/**********************************************************************/
/*                  Render whole begin/end objects                    */
/**********************************************************************/

#define NEED_EDGEFLAG_SETUP (ctx->_TriangleCaps & DD_TRI_UNFILLED)
#define EDGEFLAG_GET(idx) VB->EdgeFlag[idx]
#define EDGEFLAG_SET(idx, val) VB->EdgeFlag[idx] = val


/* Vertices, no clipping.
 */
#define RENDER_POINTS( start, count ) \
   tnl->Driver.Render.Points( ctx, start, count )

#define RENDER_LINE( v1, v2 ) \
   LineFunc( ctx, v1, v2 )

#define RENDER_TRI( v1, v2, v3 ) \
   TriangleFunc( ctx, v1, v2, v3 )

#define RENDER_QUAD( v1, v2, v3, v4 ) \
   QuadFunc( ctx, v1, v2, v3, v4 )

#define TAG(x) _tnl_##x##_verts

#define LOCAL_VARS						\
   TNLcontext *tnl = TNL_CONTEXT(ctx);				\
   struct vertex_buffer *VB = &tnl->vb;				\
   const GLuint * const elt = VB->Elts;				\
   const line_func LineFunc = tnl->Driver.Render.Line;		\
   const triangle_func TriangleFunc = tnl->Driver.Render.Triangle;	\
   const quad_func QuadFunc = tnl->Driver.Render.Quad;		\
   const GLboolean stipple = ctx->Line.StippleFlag;		\
   (void) (LineFunc && TriangleFunc && QuadFunc);		\
   (void) elt; (void) stipple

#define RESET_STIPPLE if (stipple) tnl->Driver.Render.ResetLineStipple( ctx )
#define RESET_OCCLUSION ctx->OcclusionResult = GL_TRUE
#define INIT(x) tnl->Driver.Render.PrimitiveNotify( ctx, x )
#define RENDER_TAB_QUALIFIER
#define PRESERVE_VB_DEFS
#include "t_vb_rendertmp.h"


/* Elts, no clipping.
 */
#undef ELT
#define TAG(x) _tnl_##x##_elts
#define ELT(x) elt[x]
#include "t_vb_rendertmp.h"


/**********************************************************************/
/*              Helper functions for drivers                  */
/**********************************************************************/

void _tnl_RenderClippedPolygon( GLcontext *ctx, const GLuint *elts, GLuint n )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct vertex_buffer *VB = &tnl->vb;
   GLuint *tmp = VB->Elts;

   VB->Elts = (GLuint *)elts;
   tnl->Driver.Render.PrimTabElts[GL_POLYGON]( ctx, 0, n, PRIM_BEGIN|PRIM_END );
   VB->Elts = tmp;
}

void _tnl_RenderClippedLine( GLcontext *ctx, GLuint ii, GLuint jj )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   tnl->Driver.Render.Line( ctx, ii, jj );
}



/**********************************************************************/
/*              Clip and render whole vertex buffers                  */
/**********************************************************************/


static GLboolean run_render( GLcontext *ctx,
			     struct gl_pipeline_stage *stage )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct vertex_buffer *VB = &tnl->vb;
   GLuint new_inputs = stage->changed_inputs;
   render_func *tab;
   GLint pass = 0;

   /* Allow the drivers to lock before projected verts are built so
    * that window coordinates are guarenteed not to change before
    * rendering.
    */
   ASSERT(tnl->Driver.Render.Start);

   tnl->Driver.Render.Start( ctx );

   ASSERT(tnl->Driver.Render.BuildVertices);
   ASSERT(tnl->Driver.Render.PrimitiveNotify);
   ASSERT(tnl->Driver.Render.Points);
   ASSERT(tnl->Driver.Render.Line);
   ASSERT(tnl->Driver.Render.Triangle);
   ASSERT(tnl->Driver.Render.Quad);
   ASSERT(tnl->Driver.Render.ResetLineStipple);
   ASSERT(tnl->Driver.Render.Interp);
   ASSERT(tnl->Driver.Render.CopyPV);
   ASSERT(tnl->Driver.Render.ClippedLine);
   ASSERT(tnl->Driver.Render.ClippedPolygon);
   ASSERT(tnl->Driver.Render.Finish);

   tnl->Driver.Render.BuildVertices( ctx, 0, VB->Count, new_inputs );

   if (VB->ClipOrMask) {
      tab = VB->Elts ? clip_render_tab_elts : clip_render_tab_verts;
      clip_render_tab_elts[GL_TRIANGLES] = clip_elt_triangles;
   }
   else {
      tab = (VB->Elts ? 
	     tnl->Driver.Render.PrimTabElts : 
	     tnl->Driver.Render.PrimTabVerts);
   }

   do
   {
      GLuint i, length, flags = 0;
      for (i = VB->FirstPrimitive ; !(flags & PRIM_LAST) ; i += length)
      {
	 flags = VB->Primitive[i];
	 length= VB->PrimitiveLength[i];
	 ASSERT(length || (flags & PRIM_LAST));
	 ASSERT((flags & PRIM_MODE_MASK) <= GL_POLYGON+1);

	 if (MESA_VERBOSE & VERBOSE_PRIMS)
	    _mesa_debug(NULL, "MESA prim %s %d..%d\n", 
		    _mesa_lookup_enum_by_nr(flags & PRIM_MODE_MASK), 
		    i, i+length);

	 if (length)
	    tab[flags & PRIM_MODE_MASK]( ctx, i, i + length, flags );
      }
   } while (tnl->Driver.Render.Multipass &&
	    tnl->Driver.Render.Multipass( ctx, ++pass ));


   tnl->Driver.Render.Finish( ctx );
/*     _swrast_flush(ctx); */
/*     usleep(1000000); */
   return GL_FALSE;		/* finished the pipe */
}


/**********************************************************************/
/*                          Render pipeline stage                     */
/**********************************************************************/



/* Quite a bit of work involved in finding out the inputs for the
 * render stage.
 */
static void check_render( GLcontext *ctx, struct gl_pipeline_stage *stage )
{
   GLuint inputs = VERT_BIT_CLIP;
   GLuint i;

   if (ctx->Visual.rgbMode) {
      inputs |= VERT_BIT_COLOR0;

      if (ctx->_TriangleCaps & DD_SEPARATE_SPECULAR)
	 inputs |= VERT_BIT_COLOR1;

      if (ctx->Texture._EnabledUnits) {
	 for (i = 0 ; i < ctx->Const.MaxTextureUnits ; i++) {
	    if (ctx->Texture.Unit[i]._ReallyEnabled)
	       inputs |= VERT_BIT_TEX(i);
	 }
      }
   }
   else {
      inputs |= VERT_BIT_INDEX;
   }

   if (ctx->Point._Attenuated)
      inputs |= VERT_BIT_POINT_SIZE;

   /* How do drivers turn this off?
    */
   if (ctx->Fog.Enabled)
      inputs |= VERT_BIT_FOG;

   if (ctx->_TriangleCaps & DD_TRI_UNFILLED)
      inputs |= VERT_BIT_EDGEFLAG;

   if (ctx->RenderMode==GL_FEEDBACK)
      inputs |= VERT_BITS_TEX_ANY;

   stage->inputs = inputs;
}




static void dtr( struct gl_pipeline_stage *stage )
{
}


const struct gl_pipeline_stage _tnl_render_stage =
{
   "render",			/* name */
   (_NEW_BUFFERS |
    _DD_NEW_SEPARATE_SPECULAR |
    _DD_NEW_FLATSHADE |
    _NEW_TEXTURE|
    _NEW_LIGHT|
    _NEW_POINT|
    _NEW_FOG|
    _DD_NEW_TRI_UNFILLED |
    _NEW_RENDERMODE),		/* re-check (new inputs, interp function) */
   0,				/* re-run (always runs) */
   GL_TRUE,			/* active? */
   0,				/* inputs (set in check_render) */
   0,				/* outputs */
   0,				/* changed_inputs */
   NULL,			/* private data */
   dtr,				/* destructor */
   check_render,		/* check */
   run_render			/* run */
};
