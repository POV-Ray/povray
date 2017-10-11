/* $Id: t_vb_fog.c,v 1.19 2002/10/29 20:29:03 brianp Exp $ */

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
 */


#include "glheader.h"
#include "colormac.h"
#include "context.h"
#include "macros.h"
#include "imports.h"
#include "mmath.h"
#include "mtypes.h"

#include "math/m_xform.h"

#include "t_context.h"
#include "t_pipeline.h"


struct fog_stage_data {
   GLvector4f fogcoord;		/* has actual storage allocated */
   GLvector4f input;		/* points into VB->EyePtr Z values */
};

#define FOG_STAGE_DATA(stage) ((struct fog_stage_data *)stage->privatePtr)

#define FOG_EXP_TABLE_SIZE 256
#define FOG_MAX (10.0)
#define EXP_FOG_MAX .0006595
#define FOG_INCR (FOG_MAX/FOG_EXP_TABLE_SIZE)
static GLfloat exp_table[FOG_EXP_TABLE_SIZE];
static GLfloat inited = 0;

#if 1
#define NEG_EXP( result, narg )						\
do {									\
   GLfloat f = (GLfloat) (narg * (1.0/FOG_INCR));			\
   GLint k = (GLint) f;							\
   if (k > FOG_EXP_TABLE_SIZE-2) 					\
      result = (GLfloat) EXP_FOG_MAX;					\
   else									\
      result = exp_table[k] + (f-k)*(exp_table[k+1]-exp_table[k]);	\
} while (0)
#else
#define NEG_EXP( result, narg )					\
do {								\
   result = exp(-narg);						\
} while (0)
#endif


static void init_static_data( void )
{
   GLfloat f = 0.0F;
   GLint i = 0;
   for ( ; i < FOG_EXP_TABLE_SIZE ; i++, f += FOG_INCR) {
      exp_table[i] = (GLfloat) exp(-f);
   }
   inited = 1;
}


static void make_win_fog_coords( GLcontext *ctx, GLvector4f *out,
				 const GLvector4f *in )
{
   GLfloat end  = ctx->Fog.End;
   GLfloat *v = in->start;
   GLuint stride = in->stride;
   GLuint n = in->count;
   GLfloat (*data)[4] = out->data;
   GLfloat d;
   GLuint i;

   out->count = in->count;

   switch (ctx->Fog.Mode) {
   case GL_LINEAR:
      if (ctx->Fog.Start == ctx->Fog.End)
         d = 1.0F;
      else
         d = 1.0F / (ctx->Fog.End - ctx->Fog.Start);
      for ( i = 0 ; i < n ; i++, STRIDE_F(v, stride)) {
         GLfloat f = (end - ABSF(*v)) * d;
	 data[i][0] = CLAMP(f, 0.0F, 1.0F);
      }
      break;
   case GL_EXP:
      d = ctx->Fog.Density;
      for ( i = 0 ; i < n ; i++, STRIDE_F(v,stride))
         NEG_EXP( data[i][0], d * ABSF(*v) );
      break;
   case GL_EXP2:
      d = ctx->Fog.Density*ctx->Fog.Density;
      for ( i = 0 ; i < n ; i++, STRIDE_F(v, stride)) {
         GLfloat z = *v;
         NEG_EXP( data[i][0], d * z * z );
      }
      break;
   default:
      _mesa_problem(ctx, "Bad fog mode in make_fog_coord");
      return;
   }
}


static GLboolean run_fog_stage( GLcontext *ctx,
				struct gl_pipeline_stage *stage )
{
   struct vertex_buffer *VB = &TNL_CONTEXT(ctx)->vb;
   struct fog_stage_data *store = FOG_STAGE_DATA(stage);
   GLvector4f *input;

   if (stage->changed_inputs == 0)
      return GL_TRUE;

   if (ctx->Fog.FogCoordinateSource == GL_FRAGMENT_DEPTH_EXT) {
      /* fog computed from Z depth */
      /* source = VB->ObjPtr or VB->EyePtr coords */
      /* dest = VB->FogCoordPtr = fog stage private storage */
      VB->FogCoordPtr = &store->fogcoord;

      if (!ctx->_NeedEyeCoords) {
	 const GLfloat *m = ctx->ModelviewMatrixStack.Top->m;
	 GLfloat plane[4];

	 /* Use this to store calculated eye z values:
	  */
	 input = &store->fogcoord;

	 plane[0] = m[2];
	 plane[1] = m[6];
	 plane[2] = m[10];
	 plane[3] = m[14];

	 /* Full eye coords weren't required, just calculate the
	  * eye Z values.
	  */
	 _mesa_dotprod_tab[VB->ObjPtr->size]( (GLfloat *) input->data,
					      4 * sizeof(GLfloat),
					      VB->ObjPtr, plane );

	 input->count = VB->ObjPtr->count;
      }
      else {
	 input = &store->input;

	 if (VB->EyePtr->size < 2)
	    _mesa_vector4f_clean_elem( VB->EyePtr, VB->Count, 2 );

	 input->data = (GLfloat (*)[4]) &(VB->EyePtr->data[0][2]);
	 input->start = VB->EyePtr->start+2;
	 input->stride = VB->EyePtr->stride;
	 input->count = VB->EyePtr->count;
      }
   }
   else {
      /* use glFogCoord() coordinates */
      /* source = VB->FogCoordPtr */
      input = VB->FogCoordPtr;
      /* dest = fog stage private storage */
      VB->FogCoordPtr = &store->fogcoord;
   }

   make_win_fog_coords( ctx, VB->FogCoordPtr, input );
   return GL_TRUE;
}


static void check_fog_stage( GLcontext *ctx, struct gl_pipeline_stage *stage )
{
   stage->active = ctx->Fog.Enabled && !ctx->VertexProgram.Enabled;

   if (ctx->Fog.FogCoordinateSource == GL_FRAGMENT_DEPTH_EXT)
      stage->inputs = VERT_BIT_EYE;
   else
      stage->inputs = VERT_BIT_FOG;
}


/* Called the first time stage->run() is invoked.
 */
static GLboolean alloc_fog_data( GLcontext *ctx,
				 struct gl_pipeline_stage *stage )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct fog_stage_data *store;
   stage->privatePtr = MALLOC(sizeof(*store));
   store = FOG_STAGE_DATA(stage);
   if (!store)
      return GL_FALSE;

   _mesa_vector4f_alloc( &store->fogcoord, 0, tnl->vb.Size, 32 );
   _mesa_vector4f_init( &store->input, 0, 0 );

   if (!inited)
      init_static_data();

   /* Now run the stage.
    */
   stage->run = run_fog_stage;
   return stage->run( ctx, stage );
}


static void free_fog_data( struct gl_pipeline_stage *stage )
{
   struct fog_stage_data *store = FOG_STAGE_DATA(stage);
   if (store) {
      _mesa_vector4f_free( &store->fogcoord );
      FREE( store );
      stage->privatePtr = NULL;
   }
}


const struct gl_pipeline_stage _tnl_fog_coordinate_stage =
{
   "build fog coordinates",	/* name */
   _NEW_FOG,			/* check_state */
   _NEW_FOG,			/* run_state */
   GL_FALSE,			/* active? */
   0,				/* inputs */
   VERT_BIT_FOG,		/* outputs */
   0,				/* changed_inputs */
   NULL,			/* private_data */
   free_fog_data,		/* dtr */
   check_fog_stage,		/* check */
   alloc_fog_data		/* run -- initially set to init. */
};
