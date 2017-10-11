/* $Id: polygon.h,v 1.4 2001/03/12 00:48:38 gareth Exp $ */

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


#ifndef POLYGON_H
#define POLYGON_H


#include "mtypes.h"


extern void
_mesa_CullFace( GLenum mode );

extern void
_mesa_FrontFace( GLenum mode );

extern void
_mesa_PolygonMode( GLenum face, GLenum mode );

extern void
_mesa_PolygonOffset( GLfloat factor, GLfloat units );

extern void
_mesa_PolygonOffsetEXT( GLfloat factor, GLfloat bias );

extern void
_mesa_PolygonStipple( const GLubyte *mask );

extern void
_mesa_GetPolygonStipple( GLubyte *mask );


#endif
