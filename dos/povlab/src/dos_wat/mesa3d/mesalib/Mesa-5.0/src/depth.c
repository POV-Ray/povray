/* $Id: depth.c,v 1.31 2002/10/24 23:57:20 brianp Exp $ */

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


#include "glheader.h"
#include "imports.h"
#include "context.h"
#include "enums.h"
#include "depth.h"
#include "macros.h"
#include "mtypes.h"


/**********************************************************************/
/*****                          API Functions                     *****/
/**********************************************************************/



void
_mesa_ClearDepth( GLclampd depth )
{
   GET_CURRENT_CONTEXT(ctx);
   GLfloat tmp = (GLfloat) CLAMP( depth, 0.0, 1.0 );
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (ctx->Depth.Clear == tmp)
      return;

   FLUSH_VERTICES(ctx, _NEW_DEPTH);
   ctx->Depth.Clear = tmp;
   if (ctx->Driver.ClearDepth)
      (*ctx->Driver.ClearDepth)( ctx, ctx->Depth.Clear );
}



void
_mesa_DepthFunc( GLenum func )
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (MESA_VERBOSE & (VERBOSE_API|VERBOSE_TEXTURE))
      _mesa_debug(ctx, "glDepthFunc %s\n", _mesa_lookup_enum_by_nr(func));

   switch (func) {
   case GL_LESS:    /* (default) pass if incoming z < stored z */
   case GL_GEQUAL:
   case GL_LEQUAL:
   case GL_GREATER:
   case GL_NOTEQUAL:
   case GL_EQUAL:
   case GL_ALWAYS:
   case GL_NEVER:
      break;
   default:
      _mesa_error( ctx, GL_INVALID_ENUM, "glDepth.Func" );
      return;
   }

   if (ctx->Depth.Func == func)
      return;

   FLUSH_VERTICES(ctx, _NEW_DEPTH);
   ctx->Depth.Func = func;

   if (ctx->Driver.DepthFunc)
      ctx->Driver.DepthFunc( ctx, func );
}



void
_mesa_DepthMask( GLboolean flag )
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (MESA_VERBOSE & (VERBOSE_API|VERBOSE_TEXTURE))
      _mesa_debug(ctx, "glDepthMask %d\n", flag);

   /*
    * GL_TRUE indicates depth buffer writing is enabled (default)
    * GL_FALSE indicates depth buffer writing is disabled
    */
   if (ctx->Depth.Mask == flag)
      return;

   FLUSH_VERTICES(ctx, _NEW_DEPTH);
   ctx->Depth.Mask = flag;

   if (ctx->Driver.DepthMask)
      ctx->Driver.DepthMask( ctx, flag );
}
