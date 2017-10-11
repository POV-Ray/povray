/* $Id: t_array_api.c,v 1.28 2002/10/24 23:57:25 brianp Exp $ */

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
 */

/**
 * \file vpexec.c
 * \brief Vertex array API functions (glDrawArrays, etc)
 * \author Keith Whitwell
 */

#include "glheader.h"
#include "api_validate.h"
#include "context.h"
#include "imports.h"
#include "macros.h"
#include "mmath.h"
#include "mtypes.h"
#include "state.h"

#include "array_cache/acache.h"

#include "t_array_api.h"
#include "t_array_import.h"
#include "t_imm_api.h"
#include "t_imm_exec.h"
#include "t_context.h"
#include "t_pipeline.h"

static void fallback_drawarrays( GLcontext *ctx, GLenum mode, GLint start,
				 GLsizei count )
{
   if (_tnl_hard_begin( ctx, mode )) {
      GLint i;
      for (i = start; i < count; i++) 
	 glArrayElement( i );
      glEnd();
   }
}


static void fallback_drawelements( GLcontext *ctx, GLenum mode, GLsizei count,
				   const GLuint *indices)
{
   if (_tnl_hard_begin(ctx, mode)) {
      GLint i;
      for (i = 0 ; i < count ; i++)
	 glArrayElement( indices[i] );
      glEnd();
   }
}


static void _tnl_draw_range_elements( GLcontext *ctx, GLenum mode,
				      GLuint start, GLuint end,
				      GLsizei count, const GLuint *indices )

{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   FLUSH_CURRENT( ctx, 0 );
   
   /*  _mesa_debug(ctx, "%s\n", __FUNCTION__); */
   if (tnl->pipeline.build_state_changes)
      _tnl_validate_pipeline( ctx );

   _tnl_vb_bind_arrays( ctx, start, end );

   tnl->vb.FirstPrimitive = 0;
   tnl->vb.Primitive[0] = mode | PRIM_BEGIN | PRIM_END | PRIM_LAST;
   tnl->vb.PrimitiveLength[0] = count;
   tnl->vb.Elts = (GLuint *)indices;

   if (ctx->Array.LockCount)
      tnl->Driver.RunPipeline( ctx );
   else {
      /* Note that arrays may have changed before/after execution.
       */
      tnl->pipeline.run_input_changes |= ctx->Array._Enabled;
      tnl->Driver.RunPipeline( ctx );
      tnl->pipeline.run_input_changes |= ctx->Array._Enabled;
   }
}



/**
 * Called via the GL API dispatcher.
 */
void
_tnl_DrawArrays(GLenum mode, GLint start, GLsizei count)
{
   GET_CURRENT_CONTEXT(ctx);
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct vertex_buffer *VB = &tnl->vb;
   GLuint thresh = (ctx->Driver.NeedFlush & FLUSH_STORED_VERTICES) ? 30 : 10;
   
   if (MESA_VERBOSE & VERBOSE_API)
      _mesa_debug(NULL, "_tnl_DrawArrays %d %d\n", start, count); 
   
   /* Check arguments, etc.
    */
   if (!_mesa_validate_DrawArrays( ctx, mode, start, count ))
      return;

   if (tnl->pipeline.build_state_changes)
      _tnl_validate_pipeline( ctx );

   if (ctx->CompileFlag) {
      fallback_drawarrays( ctx, mode, start, start + count );
   }    
   else if (!ctx->Array.LockCount && (GLuint) count < thresh) {
      /* Small primitives: attempt to share a vb (at the expense of
       * using the immediate interface).
      */
      fallback_drawarrays( ctx, mode, start, start + count );
   } 
   else if (ctx->Array.LockCount && 
	    count < (GLint) ctx->Const.MaxArrayLockSize) {
      
      /* Locked primitives which can fit in a single vertex buffer:
       */
      FLUSH_CURRENT( ctx, 0 );

      if (start < (GLint) ctx->Array.LockFirst)
	 start = ctx->Array.LockFirst;
      if (start + count > (GLint) ctx->Array.LockCount)
	 count = ctx->Array.LockCount - start;
      
      /* Locked drawarrays.  Reuse any previously transformed data.
       */
      _tnl_vb_bind_arrays( ctx, ctx->Array.LockFirst, ctx->Array.LockCount );
      VB->FirstPrimitive = start;
      VB->Primitive[start] = mode | PRIM_BEGIN | PRIM_END | PRIM_LAST;
      VB->PrimitiveLength[start] = count;
      tnl->Driver.RunPipeline( ctx );
   } 
   else {
      int bufsz = 256;		/* Use a small buffer for cache goodness */
      int j, nr;
      int minimum, modulo, skip;

      /* Large primitives requiring decomposition to multiple vertex
       * buffers:
       */
      switch (mode) {
      case GL_POINTS:
	 minimum = 0;
	 modulo = 1;
	 skip = 0;
      case GL_LINES:
	 minimum = 1;
	 modulo = 2;
	 skip = 1;
      case GL_LINE_STRIP:
	 minimum = 1;
	 modulo = 1;
	 skip = 0;
	 break;
      case GL_TRIANGLES:
	 minimum = 2;
	 modulo = 3;
	 skip = 2;
	 break;
      case GL_TRIANGLE_STRIP:
	 minimum = 2;
	 modulo = 1;
	 skip = 0;
	 break;
      case GL_QUADS:
	 minimum = 3;
	 modulo = 4;
	 skip = 3;
	 break;
      case GL_QUAD_STRIP:
	 minimum = 3;
	 modulo = 2;
	 skip = 0;
	 break;
      case GL_LINE_LOOP:
      case GL_TRIANGLE_FAN:
      case GL_POLYGON:
      default:
	 /* Primitives requiring a copied vertex (fan-like primitives)
	  * must use the slow path if they cannot fit in a single
	  * vertex buffer.  
	  */
	 if (count < (GLint) ctx->Const.MaxArrayLockSize) {
	    bufsz = ctx->Const.MaxArrayLockSize;
	    minimum = 0;
	    modulo = 1;
	    skip = 0;
	 }
	 else {
	    fallback_drawarrays( ctx, mode, start, start + count );
	    return;
	 }
      }

      FLUSH_CURRENT( ctx, 0 );

      bufsz -= bufsz % modulo;
      bufsz -= minimum;
      count += start;

      for (j = start + minimum ; j < count ; j += nr + skip ) {

	 nr = MIN2( bufsz, count - j );

	 _tnl_vb_bind_arrays( ctx, j - minimum, j + nr );

	 VB->FirstPrimitive = 0;
	 VB->Primitive[0] = mode | PRIM_BEGIN | PRIM_END | PRIM_LAST;
	 VB->PrimitiveLength[0] = nr + minimum;
	 tnl->pipeline.run_input_changes |= ctx->Array._Enabled;
	 tnl->Driver.RunPipeline( ctx );
	 tnl->pipeline.run_input_changes |= ctx->Array._Enabled;
      }
   }
}


/**
 * Called via the GL API dispatcher.
 */
void
_tnl_DrawRangeElements(GLenum mode,
		       GLuint start, GLuint end,
		       GLsizei count, GLenum type, const GLvoid *indices)
{
   GET_CURRENT_CONTEXT(ctx);
   GLuint *ui_indices;

   if (MESA_VERBOSE & VERBOSE_API)
      _mesa_debug(NULL, "_tnl_DrawRangeElements %d %d %d\n", start, end, count); 

   /* Check arguments, etc.
    */
   if (!_mesa_validate_DrawRangeElements( ctx, mode, start, end, count,
                                          type, indices ))
      return;

   ui_indices = (GLuint *)_ac_import_elements( ctx, GL_UNSIGNED_INT,
					       count, type, indices );


   if (ctx->CompileFlag) {
      /* Can't do anything when compiling:
       */
      fallback_drawelements( ctx, mode, count, ui_indices );
   }
   else if (ctx->Array.LockCount) {
      /* Are the arrays already locked?  If so we currently have to look
       * at the whole locked range.
       */
      if (start >= ctx->Array.LockFirst && end <= ctx->Array.LockCount)
	 _tnl_draw_range_elements( ctx, mode,
				   ctx->Array.LockFirst,
				   ctx->Array.LockCount,
				   count, ui_indices );
      else {
	 /* The spec says referencing elements outside the locked
	  * range is undefined.  I'm going to make it a noop this time
	  * round, maybe come up with something beter before 3.6.
	  *
	  * May be able to get away with just setting LockCount==0,
	  * though this raises the problems of dependent state.  May
	  * have to call glUnlockArrays() directly?
	  *
	  * Or scan the list and replace bad indices?
	  */
	 _mesa_problem( ctx,
		     "DrawRangeElements references "
		     "elements outside locked range.");
      }
   }
   else if (end + 1 - start < ctx->Const.MaxArrayLockSize) {
      /* The arrays aren't locked but we can still fit them inside a
       * single vertexbuffer.
       */
      _tnl_draw_range_elements( ctx, mode, start, end + 1, count, ui_indices );
   } else {
      /* Range is too big to optimize:
       */
      fallback_drawelements( ctx, mode, count, ui_indices );
   }
}



/**
 * Called via the GL API dispatcher.
 */
void
_tnl_DrawElements(GLenum mode, GLsizei count, GLenum type,
		  const GLvoid *indices)
{
   GET_CURRENT_CONTEXT(ctx);
   GLuint *ui_indices;

   if (MESA_VERBOSE & VERBOSE_API)
      _mesa_debug(NULL, "_tnl_DrawElements %d\n", count); 

   /* Check arguments, etc.
    */
   if (!_mesa_validate_DrawElements( ctx, mode, count, type, indices ))
      return;

   ui_indices = (GLuint *)_ac_import_elements( ctx, GL_UNSIGNED_INT,
					       count, type, indices );

   if (ctx->CompileFlag) {
      /* Can't do anything when compiling:
       */
      fallback_drawelements( ctx, mode, count, ui_indices );
   }
   else if (ctx->Array.LockCount) {
      _tnl_draw_range_elements( ctx, mode,
				ctx->Array.LockFirst,
				ctx->Array.LockCount,
				count, ui_indices );
   }
   else {
      /* Scan the index list and see if we can use the locked path anyway.
       */
      GLuint max_elt = 0;
      GLint i;

      for (i = 0 ; i < count ; i++)
	 if (ui_indices[i] > max_elt)
            max_elt = ui_indices[i];

      if (max_elt < ctx->Const.MaxArrayLockSize && /* can we use it? */
	  max_elt < (GLuint) count) 	           /* do we want to use it? */
	 _tnl_draw_range_elements( ctx, mode, 0, max_elt+1, count, ui_indices );
      else
	 fallback_drawelements( ctx, mode, count, ui_indices );
   }
}


/**
 * Initialize context's vertex array fields.  Called during T 'n L context
 * creation.
 */
void _tnl_array_init( GLcontext *ctx )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct vertex_arrays *tmp = &tnl->array_inputs;
   GLvertexformat *vfmt = &(TNL_CONTEXT(ctx)->vtxfmt);
   GLuint i;

   vfmt->DrawArrays = _tnl_DrawArrays;
   vfmt->DrawElements = _tnl_DrawElements;
   vfmt->DrawRangeElements = _tnl_DrawRangeElements;

   /* Setup vector pointers that will be used to bind arrays to VB's.
    */
   _mesa_vector4f_init( &tmp->Obj, 0, 0 );
   _mesa_vector4f_init( &tmp->Normal, 0, 0 );   
   _mesa_vector4f_init( &tmp->FogCoord, 0, 0 );
   _mesa_vector1ui_init( &tmp->Index, 0, 0 );
   _mesa_vector1ub_init( &tmp->EdgeFlag, 0, 0 );

   for (i = 0; i < ctx->Const.MaxTextureUnits; i++)
      _mesa_vector4f_init( &tmp->TexCoord[i], 0, 0);

   tnl->tmp_primitive = (GLuint *)MALLOC(sizeof(GLuint)*tnl->vb.Size);
   tnl->tmp_primitive_length = (GLuint *)MALLOC(sizeof(GLuint)*tnl->vb.Size);
}


/**
 * Destroy the context's vertex array stuff.
 * Called during T 'n L context destruction.
 */
void _tnl_array_destroy( GLcontext *ctx )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   if (tnl->tmp_primitive_length) FREE(tnl->tmp_primitive_length);
   if (tnl->tmp_primitive) FREE(tnl->tmp_primitive);
}
