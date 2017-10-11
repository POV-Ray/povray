/* $Id: t_imm_api.h,v 1.5 2002/04/09 16:56:52 keithw Exp $ */

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
 */


#ifndef _T_VTXFMT_H
#define _T_VTXFMT_H

#include "mtypes.h"
#include "t_context.h"


extern void _tnl_save_Begin( GLenum mode );
extern void _tnl_Begin( GLenum mode );

extern void _tnl_Begin( GLenum mode );

extern void _tnl_End(void);


/* TNL-private internal functions for building higher-level operations:
 */
extern GLboolean _tnl_hard_begin( GLcontext *ctx, GLenum p );
extern void _tnl_end( GLcontext *ctx );
extern void _tnl_vertex2f( GLcontext *ctx, GLfloat x, GLfloat y );
extern void _tnl_eval_coord1f( GLcontext *CC, GLfloat u );
extern void _tnl_eval_coord2f( GLcontext *CC, GLfloat u, GLfloat v );
extern void _tnl_array_element( GLcontext *CC, GLint i );

/* Initialize our part of the vtxfmt struct:
 */
extern void _tnl_imm_vtxfmt_init( GLcontext *ctx );


#endif
