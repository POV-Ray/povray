/* $Id: t_vb_texgen.c,v 1.15 2002/10/29 20:29:04 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.5
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
 *    Brian Paul
 *    Keith Whitwell <keith@tungstengraphics.com>
 */


#include "glheader.h"
#include "colormac.h"
#include "context.h"
#include "macros.h"
#include "mmath.h"
#include "imports.h"
#include "mtypes.h"

#include "math/m_xform.h"

#include "t_context.h"
#include "t_pipeline.h"


/***********************************************************************
 * Automatic texture coordinate generation (texgen) code.
 */


struct texgen_stage_data;

typedef void (*texgen_func)( GLcontext *ctx,
			     struct texgen_stage_data *store,
			     GLuint unit);


struct texgen_stage_data {

   /* Per-texunit derived state.
    */
   GLuint TexgenSize[MAX_TEXTURE_UNITS];
   GLuint TexgenHoles[MAX_TEXTURE_UNITS];
   texgen_func TexgenFunc[MAX_TEXTURE_UNITS];

   /* Temporary values used in texgen.
    */
   GLfloat (*tmp_f)[3];
   GLfloat *tmp_m;

   /* Buffered outputs of the stage.
    */
   GLvector4f texcoord[MAX_TEXTURE_UNITS];
};


#define TEXGEN_STAGE_DATA(stage) ((struct texgen_stage_data *)stage->privatePtr)



static GLuint all_bits[5] = {
   0,
   VEC_SIZE_1,
   VEC_SIZE_2,
   VEC_SIZE_3,
   VEC_SIZE_4,
};

#define VEC_SIZE_FLAGS (VEC_SIZE_1|VEC_SIZE_2|VEC_SIZE_3|VEC_SIZE_4)

#define TEXGEN_NEED_M            (TEXGEN_SPHERE_MAP)
#define TEXGEN_NEED_F            (TEXGEN_SPHERE_MAP        | \
				  TEXGEN_REFLECTION_MAP_NV)



static void build_m3( GLfloat f[][3], GLfloat m[],
		      const GLvector4f *normal,
		      const GLvector4f *eye )
{
   GLuint stride = eye->stride;
   GLfloat *coord = (GLfloat *)eye->start;
   GLuint count = eye->count;
   const GLfloat *norm = normal->start;
   GLuint i;

   for (i=0;i<count;i++,STRIDE_F(coord,stride),STRIDE_F(norm,normal->stride)) {
      GLfloat u[3], two_nu, fx, fy, fz;
      COPY_3V( u, coord );
      NORMALIZE_3FV( u );
      two_nu = 2.0F * DOT3(norm,u);
      fx = f[i][0] = u[0] - norm[0] * two_nu;
      fy = f[i][1] = u[1] - norm[1] * two_nu;
      fz = f[i][2] = u[2] - norm[2] * two_nu;
      m[i] = fx * fx + fy * fy + (fz + 1.0F) * (fz + 1.0F);
      if (m[i] != 0.0F) {
	 m[i] = 0.5F / (GLfloat) GL_SQRT(m[i]);
      }
   }
}



static void build_m2( GLfloat f[][3], GLfloat m[],
		      const GLvector4f *normal,
		      const GLvector4f *eye )
{
   GLuint stride = eye->stride;
   GLfloat *coord = eye->start;
   GLuint count = eye->count;

   GLfloat *norm = normal->start;
   GLuint i;

   for (i=0;i<count;i++,STRIDE_F(coord,stride),STRIDE_F(norm,normal->stride)) {
      GLfloat u[3], two_nu, fx, fy, fz;
      COPY_2V( u, coord );
      u[2] = 0;
      NORMALIZE_3FV( u );
      two_nu = 2.0F * DOT3(norm,u);
      fx = f[i][0] = u[0] - norm[0] * two_nu;
      fy = f[i][1] = u[1] - norm[1] * two_nu;
      fz = f[i][2] = u[2] - norm[2] * two_nu;
      m[i] = fx * fx + fy * fy + (fz + 1.0F) * (fz + 1.0F);
      if (m[i] != 0.0F) {
	 m[i] = 0.5F / (GLfloat) GL_SQRT(m[i]);
      }
   }
}



typedef void (*build_m_func)( GLfloat f[][3],
			      GLfloat m[],
			      const GLvector4f *normal,
			      const GLvector4f *eye );


static build_m_func build_m_tab[5] = {
   0,
   0,
   build_m2,
   build_m3,
   build_m3
};


/* This is unusual in that we respect the stride of the output vector
 * (f).  This allows us to pass in either a texcoord vector4f, or a
 * temporary vector3f.
 */
static void build_f3( GLfloat *f,
		      GLuint fstride,
		      const GLvector4f *normal,
		      const GLvector4f *eye )
{
   GLuint stride = eye->stride;
   GLfloat *coord = eye->start;
   GLuint count = eye->count;

   GLfloat *norm = normal->start;
   GLuint i;

   for (i=0;i<count;i++) {
      GLfloat u[3], two_nu;
      COPY_3V( u, coord );
      NORMALIZE_3FV( u );
      two_nu = 2.0F * DOT3(norm,u);
      f[0] = u[0] - norm[0] * two_nu;
      f[1] = u[1] - norm[1] * two_nu;
      f[2] = u[2] - norm[2] * two_nu;
      STRIDE_F(coord,stride);
      STRIDE_F(f,fstride);
      STRIDE_F(norm, normal->stride);
   }
}


static void build_f2( GLfloat *f,
		      GLuint fstride,
		      const GLvector4f *normal,
		      const GLvector4f *eye )
{
   GLuint stride = eye->stride;
   GLfloat *coord = eye->start;
   GLuint count = eye->count;
   GLfloat *norm = normal->start;
   GLuint i;

   for (i=0;i<count;i++) {

      GLfloat u[3], two_nu;
      COPY_2V( u, coord );
      u[2] = 0;
      NORMALIZE_3FV( u );
      two_nu = 2.0F * DOT3(norm,u);
      f[0] = u[0] - norm[0] * two_nu;
      f[1] = u[1] - norm[1] * two_nu;
      f[2] = u[2] - norm[2] * two_nu;

      STRIDE_F(coord,stride);
      STRIDE_F(f,fstride);
      STRIDE_F(norm, normal->stride);
   }
}

typedef void (*build_f_func)( GLfloat *f,
			      GLuint fstride,
			      const GLvector4f *normal_vec,
			      const GLvector4f *eye );



/* Just treat 4-vectors as 3-vectors.
 */
static build_f_func build_f_tab[5] = {
   0,
   0,
   build_f2,
   build_f3,
   build_f3
};


/* Special case texgen functions.
 */
static void texgen_reflection_map_nv( GLcontext *ctx,
				      struct texgen_stage_data *store,
				      GLuint unit )
{
   struct vertex_buffer *VB = &TNL_CONTEXT(ctx)->vb;
   GLvector4f *in = VB->TexCoordPtr[unit];
   GLvector4f *out = &store->texcoord[unit];

   build_f_tab[VB->EyePtr->size]( out->start,
				  out->stride,
				  VB->NormalPtr,
				  VB->EyePtr );

   if (in) {
      out->flags |= (in->flags & VEC_SIZE_FLAGS) | VEC_SIZE_3;
      out->count = in->count;
      out->size = MAX2(in->size, 3);
      if (in->size == 4)
	 _mesa_copy_tab[0x8]( out, in );
   }
   else {
      out->flags |= VEC_SIZE_3;
      out->size = 3;
      out->count = in->count;
   }

}



static void texgen_normal_map_nv( GLcontext *ctx,
				  struct texgen_stage_data *store,
				  GLuint unit )
{
   struct vertex_buffer *VB = &TNL_CONTEXT(ctx)->vb;
   GLvector4f *in = VB->TexCoordPtr[unit];
   GLvector4f *out = &store->texcoord[unit];
   GLvector4f *normal = VB->NormalPtr;
   GLfloat (*texcoord)[4] = (GLfloat (*)[4])out->start;
   GLuint count = VB->Count;
   GLuint i;
   const GLfloat *norm = normal->start;

   for (i=0;i<count;i++, STRIDE_F(norm, normal->stride)) {
      texcoord[i][0] = norm[0];
      texcoord[i][1] = norm[1];
      texcoord[i][2] = norm[2];
   }


   if (in) {
      out->flags |= (in->flags & VEC_SIZE_FLAGS) | VEC_SIZE_3;
      out->count = in->count;
      out->size = MAX2(in->size, 3);
      if (in->size == 4)
	 _mesa_copy_tab[0x8]( out, in );
   }
   else {
      out->flags |= VEC_SIZE_3;
      out->size = 3;
      out->count = in->count;
   }
}


static void texgen_sphere_map( GLcontext *ctx,
			       struct texgen_stage_data *store,
			       GLuint unit )
{
   struct vertex_buffer *VB = &TNL_CONTEXT(ctx)->vb;
   GLvector4f *in = VB->TexCoordPtr[unit];
   GLvector4f *out = &store->texcoord[unit];
   GLfloat (*texcoord)[4] = (GLfloat (*)[4]) out->start;
   GLuint count = VB->Count;
   GLuint i;
   GLfloat (*f)[3] = store->tmp_f;
   GLfloat *m = store->tmp_m;

/*     _mesa_debug(NULL, "%s normstride %d eyestride %d\n",  */
/*  	   __FUNCTION__, VB->NormalPtr->stride, */
/*  	   VB->EyePtr->stride); */

   (build_m_tab[VB->EyePtr->size])( store->tmp_f,
				    store->tmp_m,
				    VB->NormalPtr,
				    VB->EyePtr );

   for (i=0;i<count;i++) {
      texcoord[i][0] = f[i][0] * m[i] + 0.5F;
      texcoord[i][1] = f[i][1] * m[i] + 0.5F;
   }

   if (in) {
      out->size = MAX2(in->size,2);
      out->count = in->count;
      out->flags |= (in->flags & VEC_SIZE_FLAGS) | VEC_SIZE_2;
      if (in->size > 2)
	 _mesa_copy_tab[all_bits[in->size] & ~0x3]( out, in );
   } else {
      out->size = 2;
      out->flags |= VEC_SIZE_2;
      out->count = in->count;
   }
}



static void texgen( GLcontext *ctx,
		    struct texgen_stage_data *store,
		    GLuint unit )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct vertex_buffer *VB = &tnl->vb;
   GLvector4f *in = VB->TexCoordPtr[unit];
   GLvector4f *out = &store->texcoord[unit];
   struct gl_texture_unit *texUnit = &ctx->Texture.Unit[unit];
   const GLvector4f *obj = VB->ObjPtr;
   const GLvector4f *eye = VB->EyePtr;
   const GLvector4f *normal = VB->NormalPtr;
   GLfloat (*texcoord)[4] = (GLfloat (*)[4])out->data;
   GLfloat *indata;
   GLuint count = VB->Count;
   GLfloat (*f)[3] = store->tmp_f;
   GLfloat *m = store->tmp_m;
	 GLuint holes = 0;


   if (texUnit->_GenFlags & TEXGEN_NEED_M) {
      build_m_tab[in->size]( store->tmp_f, store->tmp_m, normal, eye );
   } else if (texUnit->_GenFlags & TEXGEN_NEED_F) {
      build_f_tab[in->size]( (GLfloat *)store->tmp_f, 3, normal, eye );
   }

   if (!in) {
      ASSERT(0);
      in = out;
      in->count = VB->Count;

      out->size = store->TexgenSize[unit];
      out->flags |= texUnit->TexGenEnabled;
      out->count = VB->Count;
      holes = store->TexgenHoles[unit];
   }
   else {
      GLuint copy = (all_bits[in->size] & ~texUnit->TexGenEnabled);
      if (copy)
	 _mesa_copy_tab[copy]( out, in );

      out->size = MAX2(in->size, store->TexgenSize[unit]);
      out->flags |= (in->flags & VEC_SIZE_FLAGS) | texUnit->TexGenEnabled;
      out->count = in->count;

      holes = ~all_bits[in->size] & store->TexgenHoles[unit];
   }

   if (holes) {
      if (holes & VEC_DIRTY_2) _mesa_vector4f_clean_elem(out, count, 2);
      if (holes & VEC_DIRTY_1) _mesa_vector4f_clean_elem(out, count, 1);
      if (holes & VEC_DIRTY_0) _mesa_vector4f_clean_elem(out, count, 0);
   }

   if (texUnit->TexGenEnabled & S_BIT) {
      GLuint i;
      switch (texUnit->GenModeS) {
      case GL_OBJECT_LINEAR:
	 _mesa_dotprod_tab[obj->size]( (GLfloat *)out->data,
				       sizeof(out->data[0]), obj,
				       texUnit->ObjectPlaneS );
	 break;
      case GL_EYE_LINEAR:
	 _mesa_dotprod_tab[eye->size]( (GLfloat *)out->data,
				       sizeof(out->data[0]), eye,
				       texUnit->EyePlaneS );
	 break;
      case GL_SPHERE_MAP:
	 for (indata=in->start,i=0 ; i<count ;i++, STRIDE_F(indata,in->stride))
	    texcoord[i][0] = indata[0] * m[i] + 0.5F;
	 break;
      case GL_REFLECTION_MAP_NV:
	 for (i=0;i<count;i++)
	     texcoord[i][0] = f[i][0];
	 break;
      case GL_NORMAL_MAP_NV: {
	 const GLfloat *norm = normal->start;
	 for (i=0;i<count;i++, STRIDE_F(norm, normal->stride)) {
	     texcoord[i][0] = norm[0];
	 }
	 break;
      }
      default:
	 _mesa_problem(ctx, "Bad S texgen");
      }
   }

   if (texUnit->TexGenEnabled & T_BIT) {
      GLuint i;
      switch (texUnit->GenModeT) {
      case GL_OBJECT_LINEAR:
	 _mesa_dotprod_tab[obj->size]( &(out->data[0][1]),
				       sizeof(out->data[0]), obj,
				       texUnit->ObjectPlaneT );
	 break;
      case GL_EYE_LINEAR:
	 _mesa_dotprod_tab[eye->size]( &(out->data[0][1]),
				       sizeof(out->data[0]), eye,
				       texUnit->EyePlaneT );
	 break;
      case GL_SPHERE_MAP:
	 for (indata=in->start,i=0; i<count ;i++,STRIDE_F(indata,in->stride))
	     texcoord[i][1] = indata[1] * m[i] + 0.5F;
	 break;
      case GL_REFLECTION_MAP_NV:
	 for (i=0;i<count;i++)
	     texcoord[i][0] = f[i][0];
	 break;
      case GL_NORMAL_MAP_NV: {
	 const GLfloat *norm = normal->start;
	 for (i=0;i<count;i++, STRIDE_F(norm, normal->stride)) {
	     texcoord[i][1] = norm[1];
	 }
	 break;
      }
      default:
	 _mesa_problem(ctx, "Bad T texgen");
      }
   }

   if (texUnit->TexGenEnabled & R_BIT) {
      GLuint i;
      switch (texUnit->GenModeR) {
      case GL_OBJECT_LINEAR:
	 _mesa_dotprod_tab[obj->size]( &(out->data[0][2]),
				       sizeof(out->data[0]), obj,
				       texUnit->ObjectPlaneR );
	 break;
      case GL_EYE_LINEAR:
	 _mesa_dotprod_tab[eye->size]( &(out->data[0][2]),
				       sizeof(out->data[0]), eye,
				       texUnit->EyePlaneR );
	 break;
      case GL_REFLECTION_MAP_NV:
	 for (i=0;i<count;i++)
	     texcoord[i][2] = f[i][2];
	 break;
      case GL_NORMAL_MAP_NV: {
	 const GLfloat *norm = normal->start;
	 for (i=0;i<count;i++,STRIDE_F(norm, normal->stride)) {
	     texcoord[i][2] = norm[2];
	 }
	 break;
      }
      default:
	 _mesa_problem(ctx, "Bad R texgen");
      }
   }

   if (texUnit->TexGenEnabled & Q_BIT) {
      switch (texUnit->GenModeQ) {
      case GL_OBJECT_LINEAR:
	 _mesa_dotprod_tab[obj->size]( &(out->data[0][3]),
				       sizeof(out->data[0]), obj,
				       texUnit->ObjectPlaneQ );
	 break;
      case GL_EYE_LINEAR:
	 _mesa_dotprod_tab[eye->size]( &(out->data[0][3]),
				       sizeof(out->data[0]), eye,
				       texUnit->EyePlaneQ );
	 break;
      default:
	 _mesa_problem(ctx, "Bad Q texgen");
      }
   }
}



static GLboolean run_texgen_stage( GLcontext *ctx,
				   struct gl_pipeline_stage *stage )
{
   struct vertex_buffer *VB = &TNL_CONTEXT(ctx)->vb;
   struct texgen_stage_data *store = TEXGEN_STAGE_DATA( stage );
   GLuint i;

   for (i = 0 ; i < ctx->Const.MaxTextureUnits ; i++)
      if (ctx->Texture._TexGenEnabled & ENABLE_TEXGEN(i)) {
	 if (stage->changed_inputs & (VERT_BIT_EYE | VERT_BIT_NORMAL | VERT_BIT_TEX(i)))
	    store->TexgenFunc[i]( ctx, store, i );

	 VB->TexCoordPtr[i] = &store->texcoord[i];
      }

   return GL_TRUE;
}




static GLboolean run_validate_texgen_stage( GLcontext *ctx,
					    struct gl_pipeline_stage *stage )
{
   struct texgen_stage_data *store = TEXGEN_STAGE_DATA(stage);
   GLuint i;

   for (i = 0 ; i < ctx->Const.MaxTextureUnits ; i++) {
      struct gl_texture_unit *texUnit = &ctx->Texture.Unit[i];

      if (texUnit->TexGenEnabled) {
	 GLuint sz;

	 if (texUnit->TexGenEnabled & R_BIT)
	    sz = 4;
	 else if (texUnit->TexGenEnabled & Q_BIT)
	    sz = 3;
	 else if (texUnit->TexGenEnabled & T_BIT)
	    sz = 2;
	 else
	    sz = 1;

	 store->TexgenSize[i] = sz;
	 store->TexgenHoles[i] = (all_bits[sz] & ~texUnit->TexGenEnabled);
	 store->TexgenFunc[i] = texgen;

	 if (texUnit->TexGenEnabled == (S_BIT|T_BIT|R_BIT)) {
	    if (texUnit->_GenFlags == TEXGEN_REFLECTION_MAP_NV) {
	       store->TexgenFunc[i] = texgen_reflection_map_nv;
	    }
	    else if (texUnit->_GenFlags == TEXGEN_NORMAL_MAP_NV) {
	       store->TexgenFunc[i] = texgen_normal_map_nv;
	    }
	 }
	 else if (texUnit->TexGenEnabled == (S_BIT|T_BIT) &&
		  texUnit->_GenFlags == TEXGEN_SPHERE_MAP) {
	    store->TexgenFunc[i] = texgen_sphere_map;
	 }
      }
   }

   stage->run = run_texgen_stage;
   return stage->run( ctx, stage );
}


static void check_texgen( GLcontext *ctx, struct gl_pipeline_stage *stage )
{
   GLuint i;
   stage->active = 0;

   if (ctx->Texture._TexGenEnabled && !ctx->VertexProgram.Enabled) {
      GLuint inputs = 0;
      GLuint outputs = 0;

      if (ctx->Texture._GenFlags & TEXGEN_OBJ_LINEAR)
	 inputs |= VERT_BIT_POS;

      if (ctx->Texture._GenFlags & TEXGEN_NEED_EYE_COORD)
	 inputs |= VERT_BIT_EYE;

      if (ctx->Texture._GenFlags & TEXGEN_NEED_NORMALS)
	 inputs |= VERT_BIT_NORMAL;

      for (i = 0 ; i < ctx->Const.MaxTextureUnits ; i++)
	 if (ctx->Texture._TexGenEnabled & ENABLE_TEXGEN(i))
	 {
	    outputs |= VERT_BIT_TEX(i);

	    /* Need the original input in case it contains a Q coord:
	     * (sigh)
	     */
	    inputs |= VERT_BIT_TEX(i);

	    /* Something for Feedback? */
	 }

      if (stage->privatePtr)
	 stage->run = run_validate_texgen_stage;
      stage->active = 1;
      stage->inputs = inputs;
      stage->outputs = outputs;
   }
}




/* Called the first time stage->run() is invoked.
 */
static GLboolean alloc_texgen_data( GLcontext *ctx,
				    struct gl_pipeline_stage *stage )
{
   struct vertex_buffer *VB = &TNL_CONTEXT(ctx)->vb;
   struct texgen_stage_data *store;
   GLuint i;

   stage->privatePtr = CALLOC(sizeof(*store));
   store = TEXGEN_STAGE_DATA(stage);
   if (!store)
      return GL_FALSE;

   for (i = 0 ; i < ctx->Const.MaxTextureUnits ; i++)
      _mesa_vector4f_alloc( &store->texcoord[i], 0, VB->Size, 32 );

   store->tmp_f = (GLfloat (*)[3]) MALLOC(VB->Size * sizeof(GLfloat) * 3);
   store->tmp_m = (GLfloat *) MALLOC(VB->Size * sizeof(GLfloat));

   /* Now validate and run the stage.
    */
   stage->run = run_validate_texgen_stage;
   return stage->run( ctx, stage );
}


static void free_texgen_data( struct gl_pipeline_stage *stage )

{
   struct texgen_stage_data *store = TEXGEN_STAGE_DATA(stage);
   GLuint i;

   if (store) {
      for (i = 0 ; i < MAX_TEXTURE_UNITS ; i++)
	 if (store->texcoord[i].data)
	    _mesa_vector4f_free( &store->texcoord[i] );


      if (store->tmp_f) FREE( store->tmp_f );
      if (store->tmp_m) FREE( store->tmp_m );
      FREE( store );
      stage->privatePtr = NULL;
   }
}



const struct gl_pipeline_stage _tnl_texgen_stage =
{
   "texgen",			/* name */
   _NEW_TEXTURE,		/* when to call check() */
   _NEW_TEXTURE,		/* when to invalidate stored data */
   GL_FALSE,			/* active? */
   0,				/* inputs */
   0,				/* outputs */
   0,				/* changed_inputs */
   NULL,			/* private data */
   free_texgen_data,		/* destructor */
   check_texgen,		/* check */
   alloc_texgen_data		/* run -- initially set to alloc data */
};
