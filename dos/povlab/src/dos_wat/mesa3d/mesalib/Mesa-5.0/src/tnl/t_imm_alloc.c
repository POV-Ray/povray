/* $Id: t_imm_alloc.c,v 1.17 2002/10/29 20:29:01 brianp Exp $ */

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
#include "imports.h"
#include "mtypes.h"

#include "t_imm_alloc.h"


static int id = 0;  /* give each struct immediate a unique ID number */

static struct immediate *real_alloc_immediate( GLcontext *ctx )
{
   struct immediate *IM = ALIGN_CALLOC_STRUCT( immediate, 32 );

   if (!IM)
      return 0;

/*     memset(IM, 0, sizeof(*IM)); */

   IM->id = id++;
   IM->ref_count = 0;
   IM->FlushElt = 0;
   IM->LastPrimitive = IMM_MAX_COPIED_VERTS;
   IM->Count = IMM_MAX_COPIED_VERTS;
   IM->Start = IMM_MAX_COPIED_VERTS;
   IM->Material = 0;
   IM->MaterialMask = 0;
   IM->MaxTextureUnits = ctx->Const.MaxTextureUnits;
   IM->TexSize = 0;
   IM->NormalLengthPtr = 0;

   IM->CopyTexSize = 0;
   IM->CopyStart = IM->Start;

   return IM;
}


static void real_free_immediate( struct immediate *IM )
{
   static int freed = 0;

   if (IM->Material) {
      FREE( IM->Material );
      FREE( IM->MaterialMask );
      IM->Material = 0;
      IM->MaterialMask = 0;
   }

   if (IM->NormalLengthPtr)
      ALIGN_FREE( IM->NormalLengthPtr );

   ALIGN_FREE( IM );
   freed++;
/*     printf("outstanding %d\n", id - freed);    */
}


/* Cache a single allocated immediate struct.
 */
struct immediate *_tnl_alloc_immediate( GLcontext *ctx )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct immediate *tmp = tnl->freed_immediate;
   
   if (tmp) {
      tnl->freed_immediate = 0;
      return tmp;
   }
   else
      return real_alloc_immediate( ctx );
}

/* May be called after tnl is destroyed.
 */
void _tnl_free_immediate( GLcontext *ctx, struct immediate *IM )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);

   ASSERT(IM->ref_count == 0);

   if (IM->NormalLengthPtr) {
      ALIGN_FREE(IM->NormalLengthPtr);
      IM->NormalLengthPtr = NULL;
   }

   if (!tnl) {
      real_free_immediate( IM );
   } 
   else {
      if (tnl->freed_immediate)
	 real_free_immediate( tnl->freed_immediate );
      
      tnl->freed_immediate = IM;
   }
}
