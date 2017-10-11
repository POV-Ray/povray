/* $Id: t_imm_api.c,v 1.35 2002/10/29 20:29:01 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  4.1
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



#include "glheader.h"
#include "context.h"
#include "dlist.h"
#include "enums.h"
#include "light.h"
#include "imports.h"
#include "state.h"
#include "colormac.h"
#include "macros.h"
#include "vtxfmt.h"

#include "t_context.h"
#include "t_imm_api.h"
#include "t_imm_elt.h"
#include "t_imm_exec.h"
#include "t_imm_dlist.h"


/* A cassette is full or flushed on a statechange.
 */
void _tnl_flush_immediate( GLcontext *ctx, struct immediate *IM )
{
   if (!ctx) {
      /* We were called by glVertex, glEvalCoord, glArrayElement, etc.
       * The current context is corresponds to the IM structure.
       */
      GET_CURRENT_CONTEXT(context);
      ctx = context;
   }

   if (MESA_VERBOSE & VERBOSE_IMMEDIATE)
      _mesa_debug(ctx, "_tnl_flush_immediate IM: %d compiling: %d\n",
                  IM->id, ctx->CompileFlag);

   if (IM->FlushElt == FLUSH_ELT_EAGER) {
      _tnl_translate_array_elts( ctx, IM, IM->LastPrimitive, IM->Count );
   }

   /* Mark the last primitive:
    */
   IM->PrimitiveLength[IM->LastPrimitive] = IM->Count - IM->LastPrimitive;
   IM->Primitive[IM->LastPrimitive] |= PRIM_LAST;

   if (ctx->CompileFlag)
      _tnl_compile_cassette( ctx, IM );
   else
      _tnl_execute_cassette( ctx, IM );
}


/* Hook for ctx->Driver.FlushVertices:
 */
void _tnl_flush_vertices( GLcontext *ctx, GLuint flags )
{
   struct immediate *IM = TNL_CURRENT_IM(ctx);

   if (MESA_VERBOSE & VERBOSE_IMMEDIATE)
      _mesa_debug(ctx,
                  "_tnl_flush_vertices flags %x IM(%d) %d..%d Flag[%d]: %x\n", 
                  flags, IM->id, IM->Start, IM->Count, IM->Start,
                  IM->Flag[IM->Start]);

   if (IM->Flag[IM->Start]) {
      if ((flags & FLUSH_UPDATE_CURRENT) || 
	  IM->Count > IM->Start ||
	  (IM->Flag[IM->Start] & (VERT_BIT_BEGIN | VERT_BIT_END))) {
	 _tnl_flush_immediate( ctx, IM );
      }
   }
}


void
_tnl_save_Begin( GLenum mode )
{
   GET_CURRENT_CONTEXT(ctx);
   struct immediate *IM = TNL_CURRENT_IM(ctx);
   GLuint inflags, state;

/*     _mesa_debug(ctx, "%s: before: %x\n", __FUNCTION__, IM->BeginState); */

   if (mode > GL_POLYGON) {
      _mesa_compile_error( ctx, GL_INVALID_ENUM, "_tnl_Begin" );
      return;
   }

   if (ctx->NewState)
      _mesa_update_state(ctx);

#if 000
   /* if only a very few slots left, might as well flush now
    */
   if (IM->Count > IMM_MAXDATA-8) {
      _tnl_flush_immediate( ctx, IM );
      IM = TNL_CURRENT_IM(ctx);
   }
#endif

   /* Check for and flush buffered vertices from internal operations.
    */
   if (IM->SavedBeginState) {
      _tnl_flush_immediate( ctx, IM );
      IM = TNL_CURRENT_IM(ctx);
      IM->BeginState = IM->SavedBeginState;
      IM->SavedBeginState = 0;
   }

   state = IM->BeginState;
   inflags = state & (VERT_BEGIN_0|VERT_BEGIN_1);
   state |= inflags << 2;	/* set error conditions */

   if (inflags != (VERT_BEGIN_0|VERT_BEGIN_1))
   {
      GLuint count = IM->Count;
      GLuint last = IM->LastPrimitive;

      state |= (VERT_BEGIN_0|VERT_BEGIN_1);
      IM->Flag[count] |= VERT_BIT_BEGIN;
      IM->Primitive[count] = mode | PRIM_BEGIN;
      IM->PrimitiveLength[IM->LastPrimitive] = count - IM->LastPrimitive;
      IM->LastPrimitive = count;

      /* Not quite right.  Need to use the fallback '_aa_ArrayElement'
       * when not known to be inside begin/end and arrays are
       * unlocked.  
       */
      if (IM->FlushElt == FLUSH_ELT_EAGER) {
	 _tnl_translate_array_elts( ctx, IM, last, count );
      }
   }

   ctx->Driver.NeedFlush |= FLUSH_STORED_VERTICES;
   IM->BeginState = state;

   /* Update save_primitive now.  Don't touch ExecPrimitive as this is
    * updated in the replay of this cassette if we are in
    * COMPILE_AND_EXECUTE mode.
    */
   if (ctx->Driver.CurrentSavePrimitive == PRIM_UNKNOWN)
      ctx->Driver.CurrentSavePrimitive = PRIM_INSIDE_UNKNOWN_PRIM;
   else if (ctx->Driver.CurrentSavePrimitive == PRIM_OUTSIDE_BEGIN_END)
      ctx->Driver.CurrentSavePrimitive = mode;
}


void
_tnl_Begin( GLenum mode )
{
   GET_CURRENT_CONTEXT(ctx);
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   ASSERT (!ctx->CompileFlag);

   if (mode > GL_POLYGON) {
      _mesa_error( ctx, GL_INVALID_ENUM, "_tnl_Begin(0x%x)", mode );
      return;
   }

   if (ctx->Driver.CurrentExecPrimitive != PRIM_OUTSIDE_BEGIN_END) {
      _mesa_error( ctx, GL_INVALID_OPERATION, "_tnl_Begin" );
      return;
   }

   if (ctx->NewState)
      _mesa_update_state(ctx);

   {
      struct immediate *IM = TNL_CURRENT_IM(ctx);
      GLuint count = IM->Count;
      GLuint last = IM->LastPrimitive;

      if (IM->Start == IM->Count &&
	  tnl->Driver.NotifyBegin &&
	  tnl->Driver.NotifyBegin( ctx, mode )) {
	 return;
      }

      assert( IM->SavedBeginState == 0 );
      assert( IM->BeginState == 0 );

      /* Not quite right.  Need to use the fallback '_aa_ArrayElement'
       * when not known to be inside begin/end and arrays are
       * unlocked.  
       */
      if (IM->FlushElt == FLUSH_ELT_EAGER) {
	 _tnl_translate_array_elts( ctx, IM, last, count );
      }

      IM->Flag[count] |= VERT_BIT_BEGIN;
      IM->Primitive[count] = mode | PRIM_BEGIN;
      IM->PrimitiveLength[last] = count - last;
      IM->LastPrimitive = count;
      IM->BeginState = (VERT_BEGIN_0|VERT_BEGIN_1);

/*        _mesa_debug(ctx, "%s: %x\n", __FUNCTION__, IM->BeginState);  */

      ctx->Driver.NeedFlush |= FLUSH_STORED_VERTICES;
      ctx->Driver.CurrentExecPrimitive = mode;
   }
}


/* Function which allows operations like 'glRectf' to decompose to a
 * begin/end object and vertices without worrying about what happens
 * with display lists.
 */
GLboolean
_tnl_hard_begin( GLcontext *ctx, GLenum p )
{
/*     _mesa_debug(ctx, "%s\n", __FUNCTION__); */

   if (!ctx->CompileFlag) {
      /* If not compiling, treat as a normal begin().
       */
/*        _mesa_debug(ctx, "%s: treating as glBegin\n", __FUNCTION__); */
      glBegin( p );
      return GL_TRUE;
   }
   else {
      /* Otherwise, need to do special processing to preserve the
       * condition that these vertices will only be replayed outside
       * future begin/end objects.
       */
      struct immediate *IM = TNL_CURRENT_IM(ctx);

      if (ctx->NewState)
	 _mesa_update_state(ctx);

      if (IM->Count > IMM_MAXDATA-8) {
	 _tnl_flush_immediate( ctx, IM );
	 IM = TNL_CURRENT_IM(ctx);
      }

      /* A lot depends on the degree to which the display list has
       * constrained the possible begin/end states at this point:
       */
      switch (IM->BeginState & (VERT_BEGIN_0|VERT_BEGIN_1)) {
      case VERT_BEGIN_0|VERT_BEGIN_1:
	 /* This is an immediate known to be inside a begin/end object.
	  */
	 ASSERT(ctx->Driver.CurrentSavePrimitive <= GL_POLYGON);
	 IM->BeginState |= (VERT_ERROR_1|VERT_ERROR_0);
	 return GL_FALSE;

      case VERT_BEGIN_0:
      case VERT_BEGIN_1:
	 /* This is a display-list immediate in an unknown begin/end
	  * state.  Assert it is empty and convert it to a 'hard' one.
	  */
	 ASSERT(IM->SavedBeginState == 0);
	 ASSERT(ctx->Driver.CurrentSavePrimitive == PRIM_UNKNOWN);

	 /* Push current beginstate, to be restored later.  Don't worry
	  * about raising errors.
	  */
	 IM->SavedBeginState = IM->BeginState;

	 /* FALLTHROUGH */

      case 0:
	 /* Unless we have fallen through, this is an immediate known to
	  * be outside begin/end objects.
	  */
	 ASSERT(ctx->Driver.CurrentSavePrimitive == PRIM_UNKNOWN ||
		ctx->Driver.CurrentSavePrimitive == PRIM_OUTSIDE_BEGIN_END);
	 ASSERT (IM->FlushElt != FLUSH_ELT_EAGER);

	 IM->BeginState |= VERT_BEGIN_0|VERT_BEGIN_1;
	 IM->Flag[IM->Count] |= VERT_BIT_BEGIN;
	 IM->Primitive[IM->Count] = p | PRIM_BEGIN;
	 IM->PrimitiveLength[IM->LastPrimitive] = IM->Count - IM->LastPrimitive;
	 IM->LastPrimitive = IM->Count;

	 /* This is necessary as this immediate will not be flushed in
	  * _tnl_end() -- we leave it active, hoping to pick up more
	  * vertices before the next state change.
	  */
	 ctx->Driver.NeedFlush |= FLUSH_STORED_VERTICES;
	 return GL_TRUE;

      default:
	 assert (0);
	 return GL_TRUE;
      }
   }
}






/* Both streams now outside begin/end.
 *
 * Leave SavedBeginState untouched -- attempt to gather several
 * rects/arrays together in a single immediate struct.
 */
void
_tnl_end( GLcontext *ctx )
{
   struct immediate *IM = TNL_CURRENT_IM(ctx);
   GLuint state = IM->BeginState;
   GLuint inflags = (~state) & (VERT_BEGIN_0|VERT_BEGIN_1);

   assert( ctx->Driver.NeedFlush & FLUSH_STORED_VERTICES );

   state |= inflags << 2;	/* errors */

   if (inflags != (VERT_BEGIN_0|VERT_BEGIN_1))
   {
      GLuint count = IM->Count;
      GLuint last = IM->LastPrimitive;

      state &= ~(VERT_BEGIN_0|VERT_BEGIN_1); /* update state */
      IM->Flag[count] |= VERT_BIT_END;
      IM->Primitive[last] |= PRIM_END;
      IM->PrimitiveLength[last] = count - last;
      IM->Primitive[count] = PRIM_OUTSIDE_BEGIN_END; /* removes PRIM_BEGIN 
						      * flag if length == 0
						      */
      IM->LastPrimitive = count;

      if (IM->FlushElt == FLUSH_ELT_EAGER) {
	 _tnl_translate_array_elts( ctx, IM, last, count );
      }
   }

   IM->BeginState = state;

   if (!ctx->CompileFlag) {
      if (ctx->Driver.CurrentExecPrimitive == PRIM_OUTSIDE_BEGIN_END) 
	 _mesa_error( ctx, GL_INVALID_OPERATION, "_tnl_End" );
      else
	 ctx->Driver.CurrentExecPrimitive = PRIM_OUTSIDE_BEGIN_END;
   }

   /* You can set this flag to get the old 'flush_vb on glEnd()'
    * behaviour.
    */
   if (MESA_DEBUG_FLAGS & DEBUG_ALWAYS_FLUSH)
      _tnl_flush_immediate( ctx, IM );
}

void
_tnl_End(void)
{
   GET_CURRENT_CONTEXT(ctx);

   _tnl_end( ctx );

   /* Need to keep save primitive uptodate in COMPILE and
    * COMPILE_AND_EXEC modes, need to keep exec primitive uptodate
    * otherwise.
    */
   if (ctx->CompileFlag)
      ctx->Driver.CurrentSavePrimitive = PRIM_OUTSIDE_BEGIN_END;
}


#define COLOR( r, g, b, a )					\
{								\
   GET_IMMEDIATE;						\
   GLuint count = IM->Count;					\
   GLfloat *color = IM->Attrib[VERT_ATTRIB_COLOR0][count];	\
   IM->Flag[count] |= VERT_BIT_COLOR0;				\
   color[0] = r;						\
   color[1] = g;						\
   color[2] = b;						\
   color[3] = a;						\
}

static void
_tnl_Color3f( GLfloat red, GLfloat green, GLfloat blue )
{
   COLOR( red, green, blue, 1.0 );
}

static void
_tnl_Color3ub( GLubyte red, GLubyte green, GLubyte blue )
{
   COLOR(UBYTE_TO_FLOAT(red),
         UBYTE_TO_FLOAT(green),
         UBYTE_TO_FLOAT(blue),
         1.0);
}

static void
_tnl_Color4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
   COLOR( red, green, blue, alpha );
}

static void
_tnl_Color4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
   COLOR(UBYTE_TO_FLOAT(red),
         UBYTE_TO_FLOAT(green),
         UBYTE_TO_FLOAT(blue),
         UBYTE_TO_FLOAT(alpha));
}

static void
_tnl_Color3fv( const GLfloat *v )
{
   COLOR( v[0], v[1], v[2], 1.0 );
}

static void
_tnl_Color3ubv( const GLubyte *v )
{
   COLOR(UBYTE_TO_FLOAT(v[0]),
         UBYTE_TO_FLOAT(v[1]),
         UBYTE_TO_FLOAT(v[2]),
         1.0 );
}

static void
_tnl_Color4fv( const GLfloat *v )
{
   COLOR( v[0], v[1], v[2], v[3] );
}

static void
_tnl_Color4ubv( const GLubyte *v)
{
   COLOR(UBYTE_TO_FLOAT(v[0]),
         UBYTE_TO_FLOAT(v[1]),
         UBYTE_TO_FLOAT(v[2]),
         UBYTE_TO_FLOAT(v[3]));
}




#define SECONDARY_COLOR( r, g, b )			\
{							\
   GLuint count;					\
   GET_IMMEDIATE;					\
   count = IM->Count;					\
   IM->Flag[count] |= VERT_BIT_COLOR1;			\
   IM->Attrib[VERT_ATTRIB_COLOR1][count][0] = r;	\
   IM->Attrib[VERT_ATTRIB_COLOR1][count][1] = g;	\
   IM->Attrib[VERT_ATTRIB_COLOR1][count][2] = b;	\
}

static void
_tnl_SecondaryColor3fEXT( GLfloat red, GLfloat green, GLfloat blue )
{
   SECONDARY_COLOR( red, green, blue );
}

static void
_tnl_SecondaryColor3ubEXT( GLubyte red, GLubyte green, GLubyte blue )
{
   SECONDARY_COLOR(UBYTE_TO_FLOAT(red),
                   UBYTE_TO_FLOAT(green),
                   UBYTE_TO_FLOAT(blue));
}

static void
_tnl_SecondaryColor3fvEXT( const GLfloat *v )
{
   SECONDARY_COLOR( v[0], v[1], v[2] );
}

static void
_tnl_SecondaryColor3ubvEXT( const GLubyte *v )
{
   SECONDARY_COLOR(UBYTE_TO_FLOAT(v[0]),
                   UBYTE_TO_FLOAT(v[1]),
                   UBYTE_TO_FLOAT(v[2]));
}


static void
_tnl_EdgeFlag( GLboolean flag )
{
   GLuint count;
   GET_IMMEDIATE;
   count = IM->Count;
   IM->EdgeFlag[count] = flag;
   IM->Flag[count] |= VERT_BIT_EDGEFLAG;
}


static void
_tnl_EdgeFlagv( const GLboolean *flag )
{
   GLuint count;
   GET_IMMEDIATE;
   count = IM->Count;
   IM->EdgeFlag[count] = *flag;
   IM->Flag[count] |= VERT_BIT_EDGEFLAG;
}


static void
_tnl_FogCoordfEXT( GLfloat f )
{
   GLuint count;
   GET_IMMEDIATE;
   count = IM->Count;
   IM->Attrib[VERT_ATTRIB_FOG][count][0] = f; /*FogCoord[count] = f;*/
   IM->Flag[count] |= VERT_BIT_FOG;
}

static void
_tnl_FogCoordfvEXT( const GLfloat *v )
{
   GLuint count;
   GET_IMMEDIATE;
   count = IM->Count;
   IM->Attrib[VERT_ATTRIB_FOG][count][0] = v[0]; /*FogCoord[count] = v[0];*/
   IM->Flag[count] |= VERT_BIT_FOG;
}


static void
_tnl_Indexi( GLint c )
{
   GLuint count;
   GET_IMMEDIATE;
   count = IM->Count;
   IM->Index[count] = c;
   IM->Flag[count] |= VERT_BIT_INDEX;
}


static void
_tnl_Indexiv( const GLint *c )
{
   GLuint count;
   GET_IMMEDIATE;
   count = IM->Count;
   IM->Index[count] = *c;
   IM->Flag[count] |= VERT_BIT_INDEX;
}


#define NORMAL( x, y, z )				\
{							\
   GLuint count;					\
   GLfloat *normal;					\
   GET_IMMEDIATE;					\
   count = IM->Count;					\
   IM->Flag[count] |= VERT_BIT_NORMAL;			\
   normal = IM->Attrib[VERT_ATTRIB_NORMAL][count];	\
   ASSIGN_3V(normal, x,y,z);				\
}

#if defined(USE_IEEE)
#define NORMALF( x, y, z )					\
{								\
   GLuint count;						\
   fi_type *normal;						\
   GET_IMMEDIATE;						\
   count = IM->Count;						\
   IM->Flag[count] |= VERT_BIT_NORMAL;				\
   normal = (fi_type *)IM->Attrib[VERT_ATTRIB_NORMAL][count];	\
   normal[0].i = ((fi_type *)&(x))->i;				\
   normal[1].i = ((fi_type *)&(y))->i;				\
   normal[2].i = ((fi_type *)&(z))->i;				\
}
#else
#define NORMALF NORMAL
#endif

static void
_tnl_Normal3f( GLfloat nx, GLfloat ny, GLfloat nz )
{
   NORMALF(nx, ny, nz);
}


static void
_tnl_Normal3fv( const GLfloat *v )
{
   NORMALF( v[0], v[1], v[2] );
/*     struct immediate *IM = (struct immediate *)(((GLcontext *) _glapi_Context)->swtnl_im); */
/*     IM->Flag[IM->Count] = VERT_NORM; */
}



#define TEXCOORD1(s)				\
{						\
   GLuint count;				\
   GLfloat *tc;					\
   GET_IMMEDIATE;				\
   count = IM->Count;				\
   IM->Flag[count] |= VERT_BIT_TEX0;		\
   tc = IM->Attrib[VERT_ATTRIB_TEX0][count];	\
   ASSIGN_4V(tc,s,0,0,1);			\
}

#define TEXCOORD2(s, t)				\
{						\
   GLuint count;				\
   GLfloat *tc;					\
   GET_IMMEDIATE;				\
   count = IM->Count;				\
   IM->Flag[count] |= VERT_BIT_TEX0;		\
   tc = IM->Attrib[VERT_ATTRIB_TEX0][count];	\
   ASSIGN_4V(tc, s, t, 0, 1);		        \
}

#define TEXCOORD3(s, t, u)			\
{						\
   GLuint count;				\
   GLfloat *tc;					\
   GET_IMMEDIATE;				\
   count = IM->Count;				\
   IM->Flag[count] |= VERT_BIT_TEX0;		\
   IM->TexSize |= TEX_0_SIZE_3;			\
   tc = IM->Attrib[VERT_ATTRIB_TEX0][count];	\
   ASSIGN_4V(tc, s, t, u, 1);			\
}

#define TEXCOORD4(s, t, u, v)			\
{						\
   GLuint count;				\
   GLfloat *tc;					\
   GET_IMMEDIATE;				\
   count = IM->Count;				\
   IM->Flag[count] |= VERT_BIT_TEX0;		\
   IM->TexSize |= TEX_0_SIZE_4;			\
   tc = IM->Attrib[VERT_ATTRIB_TEX0][count];	\
   ASSIGN_4V(tc, s, t, u, v);			\
}

#if defined(USE_IEEE)
#define TEXCOORD2F(s, t)				\
{							\
   GLuint count;					\
   fi_type *tc;						\
   GET_IMMEDIATE;					\
   count = IM->Count;					\
   IM->Flag[count] |= VERT_BIT_TEX0;			\
   tc = (fi_type *)IM->Attrib[VERT_ATTRIB_TEX0][count];	\
   tc[0].i = ((fi_type *)&(s))->i;			\
   tc[1].i = ((fi_type *)&(t))->i;			\
   tc[2].i = 0;						\
   tc[3].i = IEEE_ONE;					\
}
#else
#define TEXCOORD2F TEXCOORD2
#endif

static void
_tnl_TexCoord1f( GLfloat s )
{
   TEXCOORD1(s);
}


static void
_tnl_TexCoord2f( GLfloat s, GLfloat t )
{
   TEXCOORD2F(s, t);
}


static void
_tnl_TexCoord3f( GLfloat s, GLfloat t, GLfloat r )
{
   TEXCOORD3(s, t, r);
}

static void
_tnl_TexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
   TEXCOORD4(s, t, r, q)
}

static void
_tnl_TexCoord1fv( const GLfloat *v )
{
   TEXCOORD1(v[0]);
}

static void
_tnl_TexCoord2fv( const GLfloat *v )
{
   TEXCOORD2F(v[0], v[1]);
}

static void
_tnl_TexCoord3fv( const GLfloat *v )
{
   TEXCOORD3(v[0], v[1], v[2]);
}

static void
_tnl_TexCoord4fv( const GLfloat *v )
{
   TEXCOORD4(v[0], v[1], v[2], v[3]);
}



/* KW: Run into bad problems in vertex copying if we don't fully pad
 *     the incoming vertices.
 */
#define VERTEX2(IM, x,y)				\
{							\
   GLuint count = IM->Count++;				\
   GLfloat *dest = IM->Attrib[VERT_ATTRIB_POS][count];	\
   IM->Flag[count] |= VERT_BIT_POS;			\
   ASSIGN_4V(dest, x, y, 0, 1);				\
/*     ASSERT(IM->Flag[IM->Count]==0);		 */	\
   if (count == IMM_MAXDATA - 1)			\
      _tnl_flush_immediate( NULL, IM );			\
}

#define VERTEX3(IM,x,y,z)				\
{							\
   GLuint count = IM->Count++;				\
   GLfloat *dest = IM->Attrib[VERT_ATTRIB_POS][count];	\
   IM->Flag[count] |= VERT_BITS_OBJ_23;			\
   ASSIGN_4V(dest, x, y, z, 1);				\
/*     ASSERT(IM->Flag[IM->Count]==0); */		\
   if (count == IMM_MAXDATA - 1)			\
      _tnl_flush_immediate( NULL, IM );			\
}

#define VERTEX4(IM, x,y,z,w)				\
{							\
   GLuint count = IM->Count++;				\
   GLfloat *dest = IM->Attrib[VERT_ATTRIB_POS][count];	\
   IM->Flag[count] |= VERT_BITS_OBJ_234;		\
   ASSIGN_4V(dest, x, y, z, w);				\
   if (count == IMM_MAXDATA - 1)			\
      _tnl_flush_immediate( NULL, IM );			\
}

#if defined(USE_IEEE)
#define VERTEX2F(IM, x, y)						\
{									\
   GLuint count = IM->Count++;						\
   fi_type *dest = (fi_type *)IM->Attrib[VERT_ATTRIB_POS][count];	\
   IM->Flag[count] |= VERT_BIT_POS; 					\
   dest[0].i = ((fi_type *)&(x))->i;					\
   dest[1].i = ((fi_type *)&(y))->i;					\
   dest[2].i = 0;							\
   dest[3].i = IEEE_ONE;						\
/*     ASSERT(IM->Flag[IM->Count]==0); */				\
   if (count == IMM_MAXDATA - 1)					\
      _tnl_flush_immediate( NULL, IM );					\
}
#else
#define VERTEX2F VERTEX2
#endif

#if defined(USE_IEEE)
#define VERTEX3F(IM, x, y, z)						\
{									\
   GLuint count = IM->Count++;						\
   fi_type *dest = (fi_type *)IM->Attrib[VERT_ATTRIB_POS][count];	\
   IM->Flag[count] |= VERT_BITS_OBJ_23;					\
   dest[0].i = ((fi_type *)&(x))->i;					\
   dest[1].i = ((fi_type *)&(y))->i;					\
   dest[2].i = ((fi_type *)&(z))->i;					\
   dest[3].i = IEEE_ONE;						\
/*     ASSERT(IM->Flag[IM->Count]==0);	 */				\
   if (count == IMM_MAXDATA - 1)					\
      _tnl_flush_immediate( NULL, IM );					\
}
#else
#define VERTEX3F VERTEX3
#endif

#if defined(USE_IEEE)
#define VERTEX4F(IM, x, y, z, w)					\
{									\
   GLuint count = IM->Count++;						\
   fi_type *dest = (fi_type *)IM->Attrib[VERT_ATTRIB_POS][count];	\
   IM->Flag[count] |= VERT_BITS_OBJ_234;				\
   dest[0].i = ((fi_type *)&(x))->i;					\
   dest[1].i = ((fi_type *)&(y))->i;					\
   dest[2].i = ((fi_type *)&(z))->i;					\
   dest[3].i = ((fi_type *)&(w))->i;					\
   if (count == IMM_MAXDATA - 1)					\
      _tnl_flush_immediate( NULL, IM );					\
}
#else
#define VERTEX4F VERTEX4
#endif



static void
_tnl_Vertex2f( GLfloat x, GLfloat y )
{
   GET_IMMEDIATE;
   VERTEX2F( IM, x, y );
}

static void
_tnl_Vertex3f( GLfloat x, GLfloat y, GLfloat z )
{
   GET_IMMEDIATE;
   VERTEX3F( IM, x, y, z );
}
static void
_tnl_Vertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GET_IMMEDIATE;
   VERTEX4F( IM, x, y, z, w );
}

static void
_tnl_Vertex2fv( const GLfloat *v )
{
   GET_IMMEDIATE;
   VERTEX2F( IM, v[0], v[1] );
}

static void
_tnl_Vertex3fv( const GLfloat *v )
{
   GET_IMMEDIATE;
   VERTEX3F( IM, v[0], v[1], v[2] );
}

static void
_tnl_Vertex4fv( const GLfloat *v )
{
   GET_IMMEDIATE;
   VERTEX4F( IM, v[0], v[1], v[2], v[3] );
}




/*
 * GL_ARB_multitexture
 *
 * Note: the multitexture spec says that specifying an invalid target
 * has undefined results and does not have to generate an error.  Just
 * don't crash.  We no-op on invalid targets.
 */

#define MAX_TARGET (GL_TEXTURE0_ARB + MAX_TEXTURE_UNITS)

#define MULTI_TEXCOORD1(target, s)			\
{							\
   GET_IMMEDIATE;					\
   GLuint texunit = target - GL_TEXTURE0_ARB;		\
   if (texunit < IM->MaxTextureUnits) {			\
      GLuint count = IM->Count;				\
      GLfloat *tc = IM->Attrib[VERT_ATTRIB_TEX0 + texunit][count];	\
      ASSIGN_4V(tc, s, 0.0F, 0.0F, 1.0F);		\
      IM->Flag[count] |= VERT_BIT_TEX(texunit);		\
   }							\
}

#define MULTI_TEXCOORD2(target, s, t)			\
{							\
   GET_IMMEDIATE;					\
   GLuint texunit = target - GL_TEXTURE0_ARB;		\
   if (texunit < IM->MaxTextureUnits) {			\
      GLuint count = IM->Count;				\
      GLfloat *tc = IM->Attrib[VERT_ATTRIB_TEX0 + texunit][count];	\
      ASSIGN_4V(tc, s, t, 0.0F, 1.0F);			\
      IM->Flag[count] |= VERT_BIT_TEX(texunit);		\
   }							\
}

#define MULTI_TEXCOORD3(target, s, t, u)		\
{							\
   GET_IMMEDIATE;					\
   GLuint texunit = target - GL_TEXTURE0_ARB;		\
   if (texunit < IM->MaxTextureUnits) {			\
      GLuint count = IM->Count;				\
      GLfloat *tc = IM->Attrib[VERT_ATTRIB_TEX0 + texunit][count];	\
      ASSIGN_4V(tc, s, t, u, 1.0F);			\
      IM->Flag[count] |= VERT_BIT_TEX(texunit);		\
      IM->TexSize |= TEX_SIZE_3(texunit);		\
   }							\
}

#define MULTI_TEXCOORD4(target, s, t, u, v)		\
{							\
   GET_IMMEDIATE;					\
   GLuint texunit = target - GL_TEXTURE0_ARB;		\
   if (texunit < IM->MaxTextureUnits) {			\
      GLuint count = IM->Count;				\
      GLfloat *tc = IM->Attrib[VERT_ATTRIB_TEX0 + texunit][count];	\
      ASSIGN_4V(tc, s, t, u, v);			\
      IM->Flag[count] |= VERT_BIT_TEX(texunit);		\
      IM->TexSize |= TEX_SIZE_4(texunit);		\
   }							\
}

#if defined(USE_IEEE)
#define MULTI_TEXCOORD2F(target, s, t)				\
{								\
   GET_IMMEDIATE;						\
   GLuint texunit = target - GL_TEXTURE0_ARB;			\
   if (texunit < IM->MaxTextureUnits) {				\
      GLuint count = IM->Count;					\
      fi_type *tc = (fi_type *)IM->Attrib[VERT_ATTRIB_TEX0 + texunit][count];\
      IM->Flag[count] |= VERT_BIT_TEX(texunit);			\
      tc[0].i = ((fi_type *)&(s))->i;				\
      tc[1].i = ((fi_type *)&(t))->i;				\
      tc[2].i = 0;						\
      tc[3].i = IEEE_ONE;					\
   }								\
}
#else
#define MULTI_TEXCOORD2F MULTI_TEXCOORD2
#endif

static void
_tnl_MultiTexCoord1fARB(GLenum target, GLfloat s)
{
   MULTI_TEXCOORD1( target, s );
}

static void
_tnl_MultiTexCoord1fvARB(GLenum target, const GLfloat *v)
{
   MULTI_TEXCOORD1( target, v[0] );
}

static void
_tnl_MultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t)
{
   MULTI_TEXCOORD2F( target, s, t );
}

static void
_tnl_MultiTexCoord2fvARB(GLenum target, const GLfloat *v)
{
   MULTI_TEXCOORD2F( target, v[0], v[1] );
}

static void
_tnl_MultiTexCoord3fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
   MULTI_TEXCOORD3( target, s, t, r );
}

static void
_tnl_MultiTexCoord3fvARB(GLenum target, const GLfloat *v)
{
   MULTI_TEXCOORD3( target, v[0], v[1], v[2] );
}

static void
_tnl_MultiTexCoord4fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
   MULTI_TEXCOORD4( target, s, t, r, q );
}

static void
_tnl_MultiTexCoord4fvARB(GLenum target, const GLfloat *v)
{
   MULTI_TEXCOORD4( target, v[0], v[1], v[2], v[3] );
}



/* KW: Because the eval values don't become 'current', fixup will flow
 *     through these vertices, and then evaluation will write on top
 *     of the fixup results.
 *
 *     Note: using Obj to hold eval coord data.
 */
#define EVALCOORD1(IM, x)				\
{							\
   GLuint count = IM->Count++;				\
   GLfloat *dest = IM->Attrib[VERT_ATTRIB_POS][count];	\
   IM->Flag[count] |= VERT_BIT_EVAL_C1;			\
   ASSIGN_4V(dest, x, 0, 0, 1);				\
   if (count == IMM_MAXDATA-1)				\
      _tnl_flush_immediate( NULL, IM );			\
}

#define EVALCOORD2(IM, x, y)				\
{							\
   GLuint count = IM->Count++;				\
   GLfloat *dest = IM->Attrib[VERT_ATTRIB_POS][count];	\
   IM->Flag[count] |= VERT_BIT_EVAL_C2;			\
   ASSIGN_4V(dest, x, y, 0, 1);				\
   if (count == IMM_MAXDATA-1)				\
      _tnl_flush_immediate( NULL, IM );			\
}

#define EVALPOINT1(IM, x)				\
{							\
   GLuint count = IM->Count++;				\
   GLfloat *dest = IM->Attrib[VERT_ATTRIB_POS][count];	\
   IM->Flag[count] |= VERT_BIT_EVAL_P1;			\
   ASSIGN_4V(dest, x, 0, 0, 1);				\
   if (count == IMM_MAXDATA-1)				\
      _tnl_flush_immediate( NULL, IM );			\
}

#define EVALPOINT2(IM, x, y)				\
{							\
   GLuint count = IM->Count++;				\
   GLfloat *dest = IM->Attrib[VERT_ATTRIB_POS][count];	\
   IM->Flag[count] |= VERT_BIT_EVAL_P2;			\
   ASSIGN_4V(dest, x, y, 0, 1);				\
   if (count == IMM_MAXDATA-1)				\
      _tnl_flush_immediate( NULL, IM );			\
}

static void
_tnl_EvalCoord1f( GLfloat u )
{
   GET_IMMEDIATE;
   EVALCOORD1( IM, u );
}

static void
_tnl_EvalCoord1fv( const GLfloat *u )
{
   GET_IMMEDIATE;
   EVALCOORD1( IM, (GLfloat) *u );
}

static void
_tnl_EvalCoord2f( GLfloat u, GLfloat v )
{
   GET_IMMEDIATE;
   EVALCOORD2( IM, u, v );
}

static void
_tnl_EvalCoord2fv( const GLfloat *u )
{
   GET_IMMEDIATE;
   EVALCOORD2( IM, u[0], u[1] );
}


static void
_tnl_EvalPoint1( GLint i )
{
   GET_IMMEDIATE;
   EVALPOINT1( IM, (GLfloat) i );
}


static void
_tnl_EvalPoint2( GLint i, GLint j )
{
   GET_IMMEDIATE;
   EVALPOINT2( IM, (GLfloat) i, (GLfloat) j );
}


/* Need to use the default array-elt outside begin/end for strict
 * conformance.
 */
#define ARRAY_ELT( IM, i )			\
{						\
   GLuint count = IM->Count;			\
   IM->Elt[count] = i;				\
   IM->Flag[count] &= IM->ArrayEltFlags;	\
   IM->Flag[count] |= VERT_BIT_ELT;		\
   IM->FlushElt = IM->ArrayEltFlush;		\
   IM->Count += IM->ArrayEltIncr;		\
   if (IM->Count == IMM_MAXDATA)		\
      _tnl_flush_immediate( NULL, IM );		\
}


static void
_tnl_ArrayElement( GLint i )
{
   GET_IMMEDIATE;
   ARRAY_ELT( IM, i );
}


/* Internal functions.  These are safe to use providing either:
 *
 *    - It is determined that a display list is not being compiled, or
 *    if so that these commands won't be compiled into the list (see
 *    t_eval.c for an example).
 *
 *    - _tnl_hard_begin() is used instead of _tnl_[bB]egin, and tested
 *    for a GL_TRUE return value.  See _tnl_Rectf, below.
 */
void
_tnl_eval_coord1f( GLcontext *CC, GLfloat u )
{
   struct immediate *i = TNL_CURRENT_IM(CC);
   EVALCOORD1( i, u );
}

void
_tnl_eval_coord2f( GLcontext *CC, GLfloat u, GLfloat v )
{
   struct immediate *i = TNL_CURRENT_IM(CC);
   EVALCOORD2( i, u, v );
}




/*
 * NV_vertex_program
 */

static void
_tnl_VertexAttrib4fNV( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   if (index < 16) {
      GET_IMMEDIATE;
      const GLuint count = IM->Count;
      GLfloat *attrib = IM->Attrib[index][count];
      ASSIGN_4V(attrib, x, y, z, w);
      IM->Flag[count] |= (1 << index);
      if (index == 0) {
         IM->Count++;
         if (count == IMM_MAXDATA - 1)
            _tnl_flush_immediate( NULL, IM );
      }
   }
   else {
      GET_CURRENT_CONTEXT(ctx);
      _mesa_error(ctx, GL_INVALID_VALUE, "glVertexAttribNV(index > 15)");
   }
}   

static void
_tnl_VertexAttrib4fvNV( GLuint index, const GLfloat *v )
{
   if (index < 16) {
      GET_IMMEDIATE;
      const GLuint count = IM->Count;
      GLfloat *attrib = IM->Attrib[index][count];
      COPY_4V(attrib, v);
      IM->Flag[count] |= (1 << index);
      if (index == 0) {
         IM->Count++;
         if (count == IMM_MAXDATA - 1)
            _tnl_flush_immediate( NULL, IM );
      }
   }
   else {
      GET_CURRENT_CONTEXT(ctx);
      _mesa_error(ctx, GL_INVALID_VALUE, "glVertexAttribNV(index > 15)");
   }
}   


/* Execute a glRectf() function.  _tnl_hard_begin() ensures the check
 * on outside_begin_end is executed even in compiled lists.  These
 * vertices can now participate in the same immediate as regular ones,
 * even in most display lists.  
 */
static void
_tnl_Rectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
   GET_CURRENT_CONTEXT(ctx);

   if (_tnl_hard_begin( ctx, GL_QUADS )) {
      glVertex2f( x1, y1 );
      glVertex2f( x2, y1 );
      glVertex2f( x2, y2 );
      glVertex2f( x1, y2 );
      glEnd();
   }
}

static void
_tnl_Materialfv( GLenum face, GLenum pname, const GLfloat *params )
{
   GET_CURRENT_CONTEXT(ctx);
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct immediate *IM = TNL_CURRENT_IM(ctx);
   GLuint count = IM->Count;
   struct gl_material *mat;
   GLuint bitmask = _mesa_material_bitmask(ctx, face, pname, ~0, "Materialfv");

   if (bitmask == 0)
      return;
   
   if (MESA_VERBOSE & VERBOSE_API)
      _mesa_debug(ctx, "_tnl_Materialfv\n");

   if (tnl->IsolateMaterials &&
       !(IM->BeginState & VERT_BEGIN_1)) /* heuristic */
   {
      _tnl_flush_immediate( ctx, IM );      
      IM = TNL_CURRENT_IM(ctx);
      count = IM->Count;
   }

   if (!(IM->Flag[count] & VERT_BIT_MATERIAL)) {
      if (!IM->Material) {
	 IM->Material = (struct gl_material (*)[2])
            MALLOC( sizeof(struct gl_material) * IMM_SIZE * 2 );
	 IM->MaterialMask = (GLuint *) MALLOC( sizeof(GLuint) * IMM_SIZE );
	 IM->MaterialMask[IM->LastMaterial] = 0;
      }
      else if (IM->MaterialOrMask & ~bitmask) {
	 _mesa_copy_material_pairs( IM->Material[count],
				    IM->Material[IM->LastMaterial],
				    IM->MaterialOrMask & ~bitmask );
      }

      IM->Flag[count] |= VERT_BIT_MATERIAL;
      IM->MaterialMask[count] = 0;
      IM->MaterialAndMask &= IM->MaterialMask[IM->LastMaterial];
      IM->LastMaterial = count;
   }

   IM->MaterialOrMask |= bitmask;
   IM->MaterialMask[count] |= bitmask;
   mat = IM->Material[count];

   if (bitmask & FRONT_AMBIENT_BIT) {
      COPY_4FV( mat[0].Ambient, params );
   }
   if (bitmask & BACK_AMBIENT_BIT) {
      COPY_4FV( mat[1].Ambient, params );
   }
   if (bitmask & FRONT_DIFFUSE_BIT) {
      COPY_4FV( mat[0].Diffuse, params );
   }
   if (bitmask & BACK_DIFFUSE_BIT) {
      COPY_4FV( mat[1].Diffuse, params );
   }
   if (bitmask & FRONT_SPECULAR_BIT) {
      COPY_4FV( mat[0].Specular, params );
   }
   if (bitmask & BACK_SPECULAR_BIT) {
      COPY_4FV( mat[1].Specular, params );
   }
   if (bitmask & FRONT_EMISSION_BIT) {
      COPY_4FV( mat[0].Emission, params );
   }
   if (bitmask & BACK_EMISSION_BIT) {
      COPY_4FV( mat[1].Emission, params );
   }
   if (bitmask & FRONT_SHININESS_BIT) {
      GLfloat shininess = CLAMP( params[0], 0.0F, 128.0F );
      mat[0].Shininess = shininess;
   }
   if (bitmask & BACK_SHININESS_BIT) {
      GLfloat shininess = CLAMP( params[0], 0.0F, 128.0F );
      mat[1].Shininess = shininess;
   }
   if (bitmask & FRONT_INDEXES_BIT) {
      mat[0].AmbientIndex = params[0];
      mat[0].DiffuseIndex = params[1];
      mat[0].SpecularIndex = params[2];
   }
   if (bitmask & BACK_INDEXES_BIT) {
      mat[1].AmbientIndex = params[0];
      mat[1].DiffuseIndex = params[1];
      mat[1].SpecularIndex = params[2];
   }

   if (tnl->IsolateMaterials && 
       !(IM->BeginState & VERT_BEGIN_1)) /* heuristic */
   {
      _tnl_flush_immediate( ctx, IM );
   }
}

void _tnl_imm_vtxfmt_init( GLcontext *ctx )
{
   GLvertexformat *vfmt = &(TNL_CONTEXT(ctx)->vtxfmt);

   /* All begin/end operations are handled by this vertex format:
    */
   vfmt->ArrayElement = _tnl_ArrayElement;
   vfmt->Begin = _tnl_Begin;
   vfmt->Color3f = _tnl_Color3f;
   vfmt->Color3fv = _tnl_Color3fv;
   vfmt->Color3ub = _tnl_Color3ub;
   vfmt->Color3ubv = _tnl_Color3ubv;
   vfmt->Color4f = _tnl_Color4f;
   vfmt->Color4fv = _tnl_Color4fv;
   vfmt->Color4ub = _tnl_Color4ub;
   vfmt->Color4ubv = _tnl_Color4ubv;
   vfmt->EdgeFlag = _tnl_EdgeFlag;
   vfmt->EdgeFlagv = _tnl_EdgeFlagv;
   vfmt->End = _tnl_End;
   vfmt->EvalCoord1f = _tnl_EvalCoord1f;
   vfmt->EvalCoord1fv = _tnl_EvalCoord1fv;
   vfmt->EvalCoord2f = _tnl_EvalCoord2f;
   vfmt->EvalCoord2fv = _tnl_EvalCoord2fv;
   vfmt->EvalPoint1 = _tnl_EvalPoint1;
   vfmt->EvalPoint2 = _tnl_EvalPoint2;
   vfmt->FogCoordfEXT = _tnl_FogCoordfEXT;
   vfmt->FogCoordfvEXT = _tnl_FogCoordfvEXT;
   vfmt->Indexi = _tnl_Indexi;
   vfmt->Indexiv = _tnl_Indexiv;
   vfmt->Materialfv = _tnl_Materialfv;
   vfmt->MultiTexCoord1fARB = _tnl_MultiTexCoord1fARB;
   vfmt->MultiTexCoord1fvARB = _tnl_MultiTexCoord1fvARB;
   vfmt->MultiTexCoord2fARB = _tnl_MultiTexCoord2fARB;
   vfmt->MultiTexCoord2fvARB = _tnl_MultiTexCoord2fvARB;
   vfmt->MultiTexCoord3fARB = _tnl_MultiTexCoord3fARB;
   vfmt->MultiTexCoord3fvARB = _tnl_MultiTexCoord3fvARB;
   vfmt->MultiTexCoord4fARB = _tnl_MultiTexCoord4fARB;
   vfmt->MultiTexCoord4fvARB = _tnl_MultiTexCoord4fvARB;
   vfmt->Normal3f = _tnl_Normal3f;
   vfmt->Normal3fv = _tnl_Normal3fv;
   vfmt->SecondaryColor3fEXT = _tnl_SecondaryColor3fEXT;
   vfmt->SecondaryColor3fvEXT = _tnl_SecondaryColor3fvEXT;
   vfmt->SecondaryColor3ubEXT = _tnl_SecondaryColor3ubEXT;
   vfmt->SecondaryColor3ubvEXT = _tnl_SecondaryColor3ubvEXT;
   vfmt->TexCoord1f = _tnl_TexCoord1f;
   vfmt->TexCoord1fv = _tnl_TexCoord1fv;
   vfmt->TexCoord2f = _tnl_TexCoord2f;
   vfmt->TexCoord2fv = _tnl_TexCoord2fv;
   vfmt->TexCoord3f = _tnl_TexCoord3f;
   vfmt->TexCoord3fv = _tnl_TexCoord3fv;
   vfmt->TexCoord4f = _tnl_TexCoord4f;
   vfmt->TexCoord4fv = _tnl_TexCoord4fv;
   vfmt->Vertex2f = _tnl_Vertex2f;
   vfmt->Vertex2fv = _tnl_Vertex2fv;
   vfmt->Vertex3f = _tnl_Vertex3f;
   vfmt->Vertex3fv = _tnl_Vertex3fv;
   vfmt->Vertex4f = _tnl_Vertex4f;
   vfmt->Vertex4fv = _tnl_Vertex4fv;
   vfmt->VertexAttrib4fNV = _tnl_VertexAttrib4fNV;
   vfmt->VertexAttrib4fvNV = _tnl_VertexAttrib4fvNV;

   /* Outside begin/end functions (from t_varray.c, t_eval.c, ...):
    */
   vfmt->Rectf = _tnl_Rectf;

   /* Just use the core function:
    */
   vfmt->CallList = _mesa_CallList;

   vfmt->prefer_float_colors = GL_FALSE;
}
