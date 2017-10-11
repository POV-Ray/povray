/* $Id: t_imm_eval.c,v 1.27 2002/10/29 20:29:02 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  4.1
 *
 * Copyright (C) 1999-2002  Brian Paul   All Rights Reserved.
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
 *    Brian Paul - vertex program updates
 */


#include "glheader.h"
#include "colormac.h"
#include "context.h"
#include "macros.h"
#include "imports.h"
#include "mmath.h"
#include "mtypes.h"
#include "math/m_eval.h"

#include "t_context.h"
#include "t_imm_debug.h"
#include "t_imm_eval.h"
#include "t_imm_exec.h"
#include "t_imm_fixup.h"
#include "t_imm_alloc.h"


static void eval_points1( GLfloat outcoord[][4],
			  GLfloat coord[][4],
			  const GLuint *flags,
			  GLfloat du, GLfloat u1 )
{
   GLuint i;
   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++)
      if (flags[i] & VERT_BITS_EVAL_ANY) {
	 outcoord[i][0] = coord[i][0];
	 outcoord[i][1] = coord[i][1];
	 if (flags[i] & VERT_BIT_EVAL_P1)
	    outcoord[i][0] = coord[i][0] * du + u1;
      }
}

static void eval_points2( GLfloat outcoord[][4],
			  GLfloat coord[][4],
			  const GLuint *flags,
			  GLfloat du, GLfloat u1,
			  GLfloat dv, GLfloat v1 )
{
   GLuint i;
   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++) {
      if (flags[i] & VERT_BITS_EVAL_ANY) {
	 outcoord[i][0] = coord[i][0];
	 outcoord[i][1] = coord[i][1];
	 if (flags[i] & VERT_BIT_EVAL_P2) {
	    outcoord[i][0] = coord[i][0] * du + u1;
	    outcoord[i][1] = coord[i][1] * dv + v1;
	 }
      }
   }
}

static const GLubyte dirty_flags[5] = {
   0,				/* not possible */
   VEC_DIRTY_0,
   VEC_DIRTY_1,
   VEC_DIRTY_2,
   VEC_DIRTY_3
};


static void eval1_4f( GLvector4f *dest,
		      GLfloat coord[][4],
		      const GLuint *flags,
		      GLuint dimension,
		      const struct gl_1d_map *map )
{
   const GLfloat u1 = map->u1;
   const GLfloat du = map->du;
   GLfloat (*to)[4] = dest->data;
   GLuint i;

   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++)
      if (flags[i] & (VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1)) {
	 GLfloat u = (coord[i][0] - u1) * du;
	 ASSIGN_4V(to[i], 0,0,0,1);
	 _math_horner_bezier_curve(map->Points, to[i], u,
				   dimension, map->Order);
      }

   dest->size = MAX2(dest->size, dimension);
   dest->flags |= dirty_flags[dimension];
}


/* as above, but dest is a gl_client_array */
static void eval1_4f_ca( struct gl_client_array *dest,
			 GLfloat coord[][4],
			 const GLuint *flags,
			 GLuint dimension,
			 const struct gl_1d_map *map )
{
   const GLfloat u1 = map->u1;
   const GLfloat du = map->du;
   GLfloat (*to)[4] = (GLfloat (*)[4])dest->Ptr;
   GLuint i;

   ASSERT(dest->Type == GL_FLOAT);
   ASSERT(dest->StrideB == 4 * sizeof(GLfloat));

   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++)
      if (flags[i] & (VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1)) {
	 GLfloat u = (coord[i][0] - u1) * du;
	 ASSIGN_4V(to[i], 0,0,0,1);
	 _math_horner_bezier_curve(map->Points, to[i], u,
				   dimension, map->Order);
      }

   dest->Size = MAX2(dest->Size, (GLint) dimension);
}


static void eval1_1ui( GLvector1ui *dest,
		       GLfloat coord[][4],
		       const GLuint *flags,
		       const struct gl_1d_map *map )
{
   const GLfloat u1 = map->u1;
   const GLfloat du = map->du;
   GLuint *to = dest->data;
   GLuint i;

   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++)
      if (flags[i] & (VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1)) {
	 GLfloat u = (coord[i][0] - u1) * du;
	 GLfloat tmp;
	 _math_horner_bezier_curve(map->Points, &tmp, u, 1, map->Order);
	 to[i] = (GLuint) (GLint) tmp;
      }

}

static void eval1_norm( GLvector4f *dest,
			GLfloat coord[][4],
			const GLuint *flags,
			const struct gl_1d_map *map )
{
   const GLfloat u1 = map->u1;
   const GLfloat du = map->du;
   GLfloat (*to)[4] = dest->data;
   GLuint i;

   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++)
      if (flags[i] & (VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1)) {
	 GLfloat u = (coord[i][0] - u1) * du;
	 _math_horner_bezier_curve(map->Points, to[i], u, 3, map->Order);
      }
}


static void eval2_obj_norm( GLvector4f *obj_ptr,
			    GLvector4f *norm_ptr,
			    GLfloat coord[][4],
			    GLuint *flags,
			    GLuint dimension,
			    const struct gl_2d_map *map )
{
   const GLfloat u1 = map->u1;
   const GLfloat du = map->du;
   const GLfloat v1 = map->v1;
   const GLfloat dv = map->dv;
   GLfloat (*obj)[4] = obj_ptr->data;
   GLfloat (*normal)[4] = norm_ptr->data;
   GLuint i;

   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++)
      if (flags[i] & (VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2)) {
	 GLfloat u = (coord[i][0] - u1) * du;
	 GLfloat v = (coord[i][1] - v1) * dv;
	 GLfloat du[4], dv[4];

	 ASSIGN_4V(obj[i], 0,0,0,1);
	 _math_de_casteljau_surf(map->Points, obj[i], du, dv, u, v, dimension,
				 map->Uorder, map->Vorder);

	 if (dimension == 4) {
	    du[0] = du[0]*obj[i][3] - du[3]*obj[i][0];
	    du[1] = du[1]*obj[i][3] - du[3]*obj[i][1];
	    du[2] = du[2]*obj[i][3] - du[3]*obj[i][2];
	 
	    dv[0] = dv[0]*obj[i][3] - dv[3]*obj[i][0];
	    dv[1] = dv[1]*obj[i][3] - dv[3]*obj[i][1];
	    dv[2] = dv[2]*obj[i][3] - dv[3]*obj[i][2];
	 }

	 CROSS3(normal[i], du, dv);
	 NORMALIZE_3FV(normal[i]);
      }

   obj_ptr->size = MAX2(obj_ptr->size, dimension);
   obj_ptr->flags |= dirty_flags[dimension];
}


static void eval2_4f( GLvector4f *dest,
		      GLfloat coord[][4],
		      const GLuint *flags,
		      GLuint dimension,
		      const struct gl_2d_map *map )
{
   const GLfloat u1 = map->u1;
   const GLfloat du = map->du;
   const GLfloat v1 = map->v1;
   const GLfloat dv = map->dv;
   GLfloat (*to)[4] = dest->data;
   GLuint i;

   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++)
      if (flags[i] & (VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2)) {
	 GLfloat u = (coord[i][0] - u1) * du;
	 GLfloat v = (coord[i][1] - v1) * dv;

	 _math_horner_bezier_surf(map->Points, to[i], u, v, dimension,
				  map->Uorder, map->Vorder);
      }

   dest->size = MAX2(dest->size, dimension);
   dest->flags |= dirty_flags[dimension];
}


/* as above, but dest is a gl_client_array */
static void eval2_4f_ca( struct gl_client_array *dest,
			 GLfloat coord[][4],
			 const GLuint *flags,
			 GLuint dimension,
			 const struct gl_2d_map *map )
{
   const GLfloat u1 = map->u1;
   const GLfloat du = map->du;
   const GLfloat v1 = map->v1;
   const GLfloat dv = map->dv;
   GLfloat (*to)[4] = (GLfloat (*)[4])dest->Ptr;
   GLuint i;

   ASSERT(dest->Type == GL_FLOAT);
   ASSERT(dest->StrideB == 4 * sizeof(GLfloat));

   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++)
      if (flags[i] & (VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2)) {
	 GLfloat u = (coord[i][0] - u1) * du;
	 GLfloat v = (coord[i][1] - v1) * dv;
	 _math_horner_bezier_surf(map->Points, to[i], u, v, dimension,
				  map->Uorder, map->Vorder);
      }

   dest->Size = MAX2(dest->Size, (GLint) dimension);
}


static void eval2_norm( GLvector4f *dest,
			GLfloat coord[][4],
			GLuint *flags,
			const struct gl_2d_map *map )
{
   const GLfloat u1 = map->u1;
   const GLfloat du = map->du;
   const GLfloat v1 = map->v1;
   const GLfloat dv = map->dv;
   GLfloat (*to)[4] = dest->data;
   GLuint i;

   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++) {
      if (flags[i] & (VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2)) {
	 GLfloat u = (coord[i][0] - u1) * du;
	 GLfloat v = (coord[i][1] - v1) * dv;
	 _math_horner_bezier_surf(map->Points, to[i], u, v, 3,
				  map->Uorder, map->Vorder);
      }
   }
}


static void eval2_1ui( GLvector1ui *dest,
		       GLfloat coord[][4],
		       const GLuint *flags,
		       const struct gl_2d_map *map )
{
   const GLfloat u1 = map->u1;
   const GLfloat du = map->du;
   const GLfloat v1 = map->v1;
   const GLfloat dv = map->dv;
   GLuint *to = dest->data;
   GLuint i;

   for (i = 0 ; !(flags[i] & VERT_BIT_END_VB) ; i++)
      if (flags[i] & (VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2)) {
	 GLfloat u = (coord[i][0] - u1) * du;
	 GLfloat v = (coord[i][1] - v1) * dv;
	 GLfloat tmp;
	 _math_horner_bezier_surf(map->Points, &tmp, u, v, 1,
				  map->Uorder, map->Vorder);

	 to[i] = (GLuint) (GLint) tmp;
      }
}


static void copy_4f( GLfloat to[][4], GLfloat from[][4], GLuint count )
{
   MEMCPY( to, from, count * sizeof(to[0]));
}

static void copy_4f_stride( GLfloat to[][4], const GLfloat *from, 
			    GLuint stride, GLuint count )
{
   if (stride == 4 * sizeof(GLfloat))
      MEMCPY( to, from, count * sizeof(to[0]));
   else {
      GLuint i;
      for (i = 0 ; i < count ; i++, STRIDE_F(from, stride))
	 COPY_4FV( to[i], from );
   }
}

static void copy_3f( GLfloat to[][4], GLfloat from[][4], GLuint count )
{
   GLuint i;
   for (i = 0 ; i < count ; i++) {
      COPY_3FV(to[i], from[i]);
   }
}


static void copy_1ui( GLuint to[], const GLuint from[], GLuint count )
{
   MEMCPY( to, from, (count) * sizeof(to[0]));
}



/* Translate eval enabled flags to VERT_* flags.
 */
static void update_eval( GLcontext *ctx )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   GLuint eval1 = 0, eval2 = 0;
   GLuint i;

   if (ctx->Eval.Map1Index)
      eval1 |= VERT_BIT_INDEX;

   if (ctx->Eval.Map2Index)
      eval2 |= VERT_BIT_INDEX;

   if (ctx->Eval.Map1Color4)
      eval1 |= VERT_BIT_COLOR0;

   if (ctx->Eval.Map2Color4)
      eval2 |= VERT_BIT_COLOR0;

   if (ctx->Eval.Map1Normal)
      eval1 |= VERT_BIT_NORMAL;

   if (ctx->Eval.Map2Normal)
      eval2 |= VERT_BIT_NORMAL;

   if (ctx->Eval.Map1TextureCoord4 ||
       ctx->Eval.Map1TextureCoord3 ||
       ctx->Eval.Map1TextureCoord2 ||
       ctx->Eval.Map1TextureCoord1)
      eval1 |= VERT_BIT_TEX0;

   if (ctx->Eval.Map2TextureCoord4 ||
       ctx->Eval.Map2TextureCoord3 ||
       ctx->Eval.Map2TextureCoord2 ||
       ctx->Eval.Map2TextureCoord1)
      eval2 |= VERT_BIT_TEX0;

   if (ctx->Eval.Map1Vertex4)
      eval1 |= VERT_BITS_OBJ_234;

   if (ctx->Eval.Map1Vertex3)
      eval1 |= VERT_BITS_OBJ_23;

   if (ctx->Eval.Map2Vertex4) {
      if (ctx->Eval.AutoNormal)
	 eval2 |= VERT_BITS_OBJ_234 | VERT_BIT_NORMAL;
      else
	 eval2 |= VERT_BITS_OBJ_234;
   }
   else if (ctx->Eval.Map2Vertex3) {
      if (ctx->Eval.AutoNormal)
	 eval2 |= VERT_BITS_OBJ_23 | VERT_BIT_NORMAL;
      else
	 eval2 |= VERT_BITS_OBJ_23;
   }

   tnl->eval.EvalMap1Flags = eval1;
   tnl->eval.EvalMap2Flags = eval2;

   /* GL_NV_vertex_program evaluators */
   eval1 = eval2 = 0;
   for (i = 0; i < VERT_ATTRIB_MAX; i++) {
      if (ctx->Eval.Map1Attrib[i])
         eval1 |= (1 << i);
      if (ctx->Eval.Map2Attrib[i])
         eval2 |= (1 << i);
   }
   tnl->eval.EvalMap1AttribFlags = eval1;
   tnl->eval.EvalMap2AttribFlags = eval2;

   tnl->eval.EvalNewState = 0;
}


/* This looks a lot like a pipeline stage, but for various reasons is
 * better handled outside the pipeline, and considered the final stage
 * of fixing up an immediate struct for execution.
 *
 * Really want to cache the results of this function in display lists,
 * at least for EvalMesh commands.
 */
void _tnl_eval_immediate( GLcontext *ctx, struct immediate *IM )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct vertex_arrays *tmp = &tnl->imm_inputs;
   struct immediate *store = tnl->eval.im;
   GLuint *flags = IM->Flag + IM->CopyStart;
   GLuint copycount;
   GLuint orflag = IM->OrFlag;
   GLuint any_eval1 = orflag & (VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1);
   GLuint any_eval2 = orflag & (VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2);
   GLuint req = 0;
   GLuint purge_flags = 0;
   GLfloat (*coord)[4] = IM->Attrib[VERT_ATTRIB_POS] + IM->CopyStart;

   if (IM->AndFlag & VERT_BITS_EVAL_ANY)
      copycount = IM->Start - IM->CopyStart; /* just copy copied vertices */
   else
      copycount = IM->Count - IM->CopyStart; /* copy all vertices */

   if (!store)
      store = tnl->eval.im = _tnl_alloc_immediate( ctx );

   if (tnl->eval.EvalNewState & _NEW_EVAL)
      update_eval( ctx );

   if (any_eval1) {
      req |= tnl->pipeline.inputs
         & (tnl->eval.EvalMap1Flags | tnl->eval.EvalMap1AttribFlags);

      if (!ctx->Eval.Map1Vertex4 && !ctx->Eval.Map1Vertex3 &&
          !ctx->Eval.Map1Attrib[0])
	 purge_flags = (VERT_BIT_EVAL_P1|VERT_BIT_EVAL_C1);

      if (orflag & VERT_BIT_EVAL_P1) {
	 eval_points1( store->Attrib[VERT_ATTRIB_POS] + IM->CopyStart, 
		       coord, flags,
		       ctx->Eval.MapGrid1du,
		       ctx->Eval.MapGrid1u1);
	 
	 coord = store->Attrib[VERT_ATTRIB_POS] + IM->CopyStart;
      }
   }

   if (any_eval2) {
      req |= tnl->pipeline.inputs
         & (tnl->eval.EvalMap2Flags | tnl->eval.EvalMap2AttribFlags);

      if (!ctx->Eval.Map2Vertex4 && !ctx->Eval.Map2Vertex3 &&
          !ctx->Eval.Map2Attrib[0])
	 purge_flags |= (VERT_BIT_EVAL_P2|VERT_BIT_EVAL_C2);

      if (orflag & VERT_BIT_EVAL_P2) {
	 eval_points2( store->Attrib[VERT_ATTRIB_POS] + IM->CopyStart, 
		       coord, flags,
		       ctx->Eval.MapGrid2du,
		       ctx->Eval.MapGrid2u1,
		       ctx->Eval.MapGrid2dv,
		       ctx->Eval.MapGrid2v1 );

	 coord = store->Attrib[VERT_ATTRIB_POS] + IM->CopyStart;
      }
   }

   /* Perform the evaluations on active data elements.
    */
   if (req & VERT_BIT_INDEX) {
      GLuint generated = 0;

      if (copycount)
	 copy_1ui( store->Index + IM->CopyStart, tmp->Index.data, copycount );

      tmp->Index.data = store->Index + IM->CopyStart;
      tmp->Index.start = store->Index + IM->CopyStart;

      if (ctx->Eval.Map1Index && any_eval1) {
	 eval1_1ui( &tmp->Index, coord, flags, &ctx->EvalMap.Map1Index );
	 generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
      }

      if (ctx->Eval.Map2Index && any_eval2) {
	 eval2_1ui( &tmp->Index, coord, flags, &ctx->EvalMap.Map2Index );
	 generated |= VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2;
      }
   }

   if (req & VERT_BIT_COLOR0) {
      GLuint generated = 0;

      if (copycount) 
	 copy_4f_stride( store->Attrib[VERT_ATTRIB_COLOR0] + IM->CopyStart, 
			 (GLfloat *)tmp->Color.Ptr, 
			 tmp->Color.StrideB,
			 copycount );

      tmp->Color.Ptr = store->Attrib[VERT_ATTRIB_COLOR0] + IM->CopyStart;
      tmp->Color.StrideB = 4 * sizeof(GLfloat);
      tmp->Color.Flags = 0;
      tnl->vb.importable_data &= ~VERT_BIT_COLOR0;

      if (ctx->VertexProgram.Enabled) {
         tmp->Attribs[VERT_ATTRIB_COLOR0].data =
            store->Attrib[VERT_ATTRIB_COLOR0] + IM->CopyStart;
         tmp->Attribs[VERT_ATTRIB_COLOR0].start =
            (GLfloat *) tmp->Attribs[VERT_ATTRIB_COLOR0].data;
         tmp->Attribs[VERT_ATTRIB_COLOR0].size = 0;
      }

      /* Vertex program maps have priority over conventional attribs */
      if (any_eval1) {
         if (ctx->VertexProgram.Enabled
             && ctx->Eval.Map1Attrib[VERT_ATTRIB_COLOR0]) {
            eval1_4f_ca( &tmp->Color, coord, flags, 4,
                         &ctx->EvalMap.Map1Attrib[VERT_ATTRIB_COLOR0] );
            generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
         }
         else if (ctx->Eval.Map1Color4) {
            eval1_4f_ca( &tmp->Color, coord, flags, 4,
                         &ctx->EvalMap.Map1Color4 );
            generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
         }
      }

      if (any_eval2) {
         if (ctx->VertexProgram.Enabled
             && ctx->Eval.Map2Attrib[VERT_ATTRIB_COLOR0]) {
            eval2_4f_ca( &tmp->Color, coord, flags, 4,
                         &ctx->EvalMap.Map2Attrib[VERT_ATTRIB_COLOR0] );
            generated |= VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2;
         }
         else if (ctx->Eval.Map2Color4) {
            eval2_4f_ca( &tmp->Color, coord, flags, 4,
                         &ctx->EvalMap.Map2Color4 );
            generated |= VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2;
         }
      }
   }

   if (req & VERT_BIT_TEX0) {
      GLuint generated = 0;

      if (copycount)
	 copy_4f( store->Attrib[VERT_ATTRIB_TEX0] + IM->CopyStart, 
		  tmp->TexCoord[0].data, copycount );
      else
	 tmp->TexCoord[0].size = 0;

      tmp->TexCoord[0].data = store->Attrib[VERT_ATTRIB_TEX0] + IM->CopyStart;
      tmp->TexCoord[0].start = (GLfloat *)tmp->TexCoord[0].data;

      if (ctx->VertexProgram.Enabled) {
         tmp->Attribs[VERT_ATTRIB_TEX0].data =
            store->Attrib[VERT_ATTRIB_TEX0] + IM->CopyStart;
         tmp->Attribs[VERT_ATTRIB_TEX0].start =
            (GLfloat *) tmp->Attribs[VERT_ATTRIB_TEX0].data;
         tmp->Attribs[VERT_ATTRIB_TEX0].size = 0;
      }

      /* Vertex program maps have priority over conventional attribs */
      if (any_eval1) {
         if (ctx->VertexProgram.Enabled
             && ctx->Eval.Map1Attrib[VERT_ATTRIB_TEX0]) {
	    eval1_4f( &tmp->TexCoord[0], coord, flags, 4,
		      &ctx->EvalMap.Map1Attrib[VERT_ATTRIB_TEX0] );
	    generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
         }
	 else if (ctx->Eval.Map1TextureCoord4) {
	    eval1_4f( &tmp->TexCoord[0], coord, flags, 4,
		      &ctx->EvalMap.Map1Texture4 );
	    generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
	 }
	 else if (ctx->Eval.Map1TextureCoord3) {
	    eval1_4f( &tmp->TexCoord[0], coord, flags, 3,
		      &ctx->EvalMap.Map1Texture3 );
	    generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
	 }
	 else if (ctx->Eval.Map1TextureCoord2) {
	    eval1_4f( &tmp->TexCoord[0], coord, flags, 2,
		      &ctx->EvalMap.Map1Texture2 );
	    generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
	 }
	 else if (ctx->Eval.Map1TextureCoord1) {
	    eval1_4f( &tmp->TexCoord[0], coord, flags, 1,
		      &ctx->EvalMap.Map1Texture1 );
	    generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
	 }
      }

      if (any_eval2) {
         if (ctx->VertexProgram.Enabled
             && ctx->Eval.Map2Attrib[VERT_ATTRIB_TEX0]) {
	    eval2_4f( &tmp->TexCoord[0], coord, flags, 4,
		      &ctx->EvalMap.Map2Attrib[VERT_ATTRIB_TEX0] );
	    generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
         }
	 else if (ctx->Eval.Map2TextureCoord4) {
	    eval2_4f( &tmp->TexCoord[0], coord, flags, 4,
		      &ctx->EvalMap.Map2Texture4 );
	    generated |= VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2;
	 }
	 else if (ctx->Eval.Map2TextureCoord3) {
	    eval2_4f( &tmp->TexCoord[0], coord, flags, 3,
		      &ctx->EvalMap.Map2Texture3 );
	    generated |= VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2;
	 }
	 else if (ctx->Eval.Map2TextureCoord2) {
	    eval2_4f( &tmp->TexCoord[0], coord, flags, 2,
		      &ctx->EvalMap.Map2Texture2 );
	    generated |= VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2;
	 }
	 else if (ctx->Eval.Map2TextureCoord1) {
	    eval2_4f( &tmp->TexCoord[0], coord, flags, 1,
		      &ctx->EvalMap.Map2Texture1 );
	    generated |= VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2;
	 }
      }
   }

   if (req & VERT_BIT_NORMAL) {
      GLuint generated = 0;

      if (copycount) {
	 copy_3f( store->Attrib[VERT_ATTRIB_NORMAL] + IM->CopyStart,
                  tmp->Normal.data, copycount );
      }

      tmp->Normal.data = store->Attrib[VERT_ATTRIB_NORMAL] + IM->CopyStart;
      tmp->Normal.start = (GLfloat *)tmp->Normal.data;

      if (ctx->VertexProgram.Enabled) {
         tmp->Attribs[VERT_ATTRIB_NORMAL].data =
            store->Attrib[VERT_ATTRIB_NORMAL] + IM->CopyStart;
         tmp->Attribs[VERT_ATTRIB_NORMAL].start =
            (GLfloat *) tmp->Attribs[VERT_ATTRIB_NORMAL].data;
         tmp->Attribs[VERT_ATTRIB_NORMAL].size = 0;
      }

      if (any_eval1) {
         if (ctx->VertexProgram.Enabled &&
             ctx->Eval.Map1Attrib[VERT_ATTRIB_NORMAL]) {
            eval1_norm( &tmp->Normal, coord, flags,
                        &ctx->EvalMap.Map1Attrib[VERT_ATTRIB_NORMAL] );
            generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
         }
         else if (ctx->Eval.Map1Normal) {
            eval1_norm( &tmp->Normal, coord, flags, &ctx->EvalMap.Map1Normal );
            generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
         }
      }

      if (any_eval2) {
         if (ctx->VertexProgram.Enabled &&
             ctx->Eval.Map2Attrib[VERT_ATTRIB_NORMAL]) {
            eval2_norm( &tmp->Normal, coord, flags,
                        &ctx->EvalMap.Map2Attrib[VERT_ATTRIB_NORMAL] );
            generated |= VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2;
         }
         else if (ctx->Eval.Map2Normal) {
            eval2_norm( &tmp->Normal, coord, flags, &ctx->EvalMap.Map2Normal );
            generated |= VERT_BIT_EVAL_C2|VERT_BIT_EVAL_P2;
         }
      }
   }

   /* In the AutoNormal case, the copy and assignment of tmp->NormalPtr
    * are done above.
    */
   if (req & VERT_BIT_POS) {
      if (copycount) {
	 /* This copy may already have occurred when eliminating
	  * glEvalPoint calls:
	  */
	 if (coord != store->Attrib[VERT_ATTRIB_POS] + IM->CopyStart) {
	    copy_4f( store->Attrib[VERT_ATTRIB_POS] + IM->CopyStart,
                     tmp->Obj.data, copycount );
         }
      }
      else {
	 tmp->Obj.size = 0;
      }

      tmp->Obj.data = store->Attrib[VERT_ATTRIB_POS] + IM->CopyStart;
      tmp->Obj.start = (GLfloat *) tmp->Obj.data;

#if 1
      /*tmp->Attribs[0].count = count;*/
      tmp->Attribs[0].data = store->Attrib[0] + IM->CopyStart;
      tmp->Attribs[0].start = (GLfloat *) tmp->Attribs[0].data;
      tmp->Attribs[0].size = 0;
#endif

      /* Note: Normal data is already prepared above.
       */

      if (any_eval1) {
         if (ctx->VertexProgram.Enabled &&
             ctx->Eval.Map1Attrib[VERT_ATTRIB_POS]) {
	    eval1_4f( &tmp->Obj, coord, flags, 4,
		      &ctx->EvalMap.Map1Attrib[VERT_ATTRIB_POS] );
         }
	 else if (ctx->Eval.Map1Vertex4) {
	    eval1_4f( &tmp->Obj, coord, flags, 4,
		      &ctx->EvalMap.Map1Vertex4 );
	 }
	 else if (ctx->Eval.Map1Vertex3) {
	    eval1_4f( &tmp->Obj, coord, flags, 3,
		      &ctx->EvalMap.Map1Vertex3 );
	 }
      }

      if (any_eval2) {
         if (ctx->VertexProgram.Enabled &&
             ctx->Eval.Map2Attrib[VERT_ATTRIB_POS]) {
	    if (ctx->Eval.AutoNormal && (req & VERT_BIT_NORMAL))
	       eval2_obj_norm( &tmp->Obj, &tmp->Normal, coord, flags, 4,
			       &ctx->EvalMap.Map2Attrib[VERT_ATTRIB_POS] );
	    else
	       eval2_4f( &tmp->Obj, coord, flags, 4,
			 &ctx->EvalMap.Map2Attrib[VERT_ATTRIB_POS] );
         }
	 else if (ctx->Eval.Map2Vertex4) {
	    if (ctx->Eval.AutoNormal && (req & VERT_BIT_NORMAL))
	       eval2_obj_norm( &tmp->Obj, &tmp->Normal, coord, flags, 4,
			       &ctx->EvalMap.Map2Vertex4 );
	    else
	       eval2_4f( &tmp->Obj, coord, flags, 4,
			 &ctx->EvalMap.Map2Vertex4 );
	 }
	 else if (ctx->Eval.Map2Vertex3) {
	    if (ctx->Eval.AutoNormal && (req & VERT_BIT_NORMAL))
	       eval2_obj_norm( &tmp->Obj, &tmp->Normal, coord, flags, 3,
			       &ctx->EvalMap.Map2Vertex3 );
	    else
	       eval2_4f( &tmp->Obj, coord, flags, 3,
			 &ctx->EvalMap.Map2Vertex3 );
	 }
      }
   }


   if (ctx->VertexProgram.Enabled) {
      /* We already evaluated position, normal, color and texture 0 above.
       * now evaluate any other generic attributes.
       */
      const GLuint skipBits = (VERT_BIT_POS |
                               VERT_BIT_NORMAL |
                               VERT_BIT_COLOR0 |
                               VERT_BIT_TEX0);
      GLuint generated = 0;
      GLuint attr;
      for (attr = 0; attr < VERT_ATTRIB_MAX; attr++) {
         if ((1 << attr) & req & ~skipBits) {
            if (any_eval1 && ctx->Eval.Map1Attrib[attr]) {
               /* evaluate 1-D vertex attrib map [i] */
               eval1_4f( &tmp->Attribs[attr], coord, flags, 4,
                         &ctx->EvalMap.Map1Attrib[attr] );
               generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
            }
            if (any_eval2 && ctx->Eval.Map2Attrib[attr]) {
               /* evaluate 2-D vertex attrib map [i] */
               eval2_4f( &tmp->Attribs[attr], coord, flags, 4,
                         &ctx->EvalMap.Map2Attrib[attr] );
               generated |= VERT_BIT_EVAL_C1|VERT_BIT_EVAL_P1;
            }
         }
      }
   }

   /* Calculate new IM->Elts, IM->Primitive, IM->PrimitiveLength for
    * the case where vertex maps are not enabled for some received
    * eval coordinates.  In this case those slots in the immediate
    * must be ignored.
    */
   if (purge_flags) {
      const GLuint vertex = VERT_BIT_POS|(VERT_BITS_EVAL_ANY & ~purge_flags);
      GLuint last_new_prim = 0;
      GLuint new_prim_length = 0;
      GLuint next_old_prim = 0;
      struct vertex_buffer *VB = &tnl->vb;
      const GLuint count = VB->Count;
      GLuint i, j;

      for (i = 0, j = 0 ; i < count ; i++) {
	 if (flags[i] & vertex) {
	    store->Elt[j++] = i;
	    new_prim_length++;
	 }
	 if (i == next_old_prim) {
	    next_old_prim += VB->PrimitiveLength[i];
	    VB->PrimitiveLength[last_new_prim] = new_prim_length;
	    VB->Primitive[j] = VB->Primitive[i];
	    last_new_prim = j;
	 }
      }
      
      VB->Elts = store->Elt;
      _tnl_get_purged_copy_verts( ctx, store );
   }

   /* Produce new flags array:
    */
   {
      const GLuint count = tnl->vb.Count + 1;
      GLuint i;

      copy_1ui( store->Flag, flags, count );
      tnl->vb.Flag = store->Flag;
      for (i = 0 ; i < count ; i++)
	 store->Flag[i] |= req;
      IM->Evaluated = req;	/* hack for copying. */
   }
}
