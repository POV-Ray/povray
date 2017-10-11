/* $Id: blend.h,v 1.9 2001/06/18 17:26:08 brianp Exp $ */

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
 */


#ifndef BLEND_H
#define BLEND_H


#include "mtypes.h"


extern void
_mesa_BlendFunc( GLenum sfactor, GLenum dfactor );


extern void
_mesa_BlendFuncSeparateEXT( GLenum sfactorRGB, GLenum dfactorRGB,
                            GLenum sfactorA, GLenum dfactorA );


extern void
_mesa_BlendEquation( GLenum mode );


extern void
_mesa_BlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);


extern void
_mesa_AlphaFunc( GLenum func, GLclampf ref );


extern void
_mesa_LogicOp( GLenum opcode );


extern void
_mesa_IndexMask( GLuint mask );

extern void
_mesa_ColorMask( GLboolean red, GLboolean green,
                 GLboolean blue, GLboolean alpha );


#endif
