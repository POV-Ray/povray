/* $Id: s_stencil.h,v 1.9 2002/10/02 23:24:04 brianp Exp $ */

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


#ifndef S_STENCIL_H
#define S_STENCIL_H


#include "mtypes.h"
#include "swrast.h"



extern GLboolean
_mesa_stencil_and_ztest_span(GLcontext *ctx, struct sw_span *span);



extern void
_mesa_read_stencil_span( GLcontext *ctx, GLint n, GLint x, GLint y,
                         GLstencil stencil[] );


extern void
_mesa_write_stencil_span( GLcontext *ctx, GLint n, GLint x, GLint y,
                          const GLstencil stencil[] );


extern void
_mesa_alloc_stencil_buffer( GLframebuffer *buffer );


extern void
_mesa_clear_stencil_buffer( GLcontext *ctx );


#endif
