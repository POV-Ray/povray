/* $Id: texcompress.h,v 1.2 2002/10/18 17:41:45 brianp Exp $ */

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

#ifndef TEXCOMPRESS_H
#define TEXCOMPRESS_H

#include "mtypes.h"


extern GLuint
_mesa_get_compressed_formats( GLcontext *ctx, GLint *formats );

extern GLuint
_mesa_compressed_texture_size( GLcontext *ctx,
                               GLsizei width, GLsizei height, GLsizei depth,
                               GLenum format );

extern GLint
_mesa_compressed_row_stride(GLenum format, GLsizei width);


extern GLubyte *
_mesa_compressed_image_address(GLint col, GLint row, GLint img,
                               GLenum format,
                               GLsizei width, const GLubyte *image);


extern void
_mesa_compress_teximage( GLcontext *ctx, GLsizei width, GLsizei height,
                         GLenum srcFormat, const GLchan *source,
                         GLint srcRowStride,
                         const struct gl_texture_format *dstFormat,
                         GLubyte *dest, GLint dstRowStride );

#endif /* TEXCOMPRESS_H */
