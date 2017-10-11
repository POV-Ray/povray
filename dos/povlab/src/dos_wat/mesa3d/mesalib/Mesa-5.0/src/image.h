/* $Id: image.h,v 1.18 2002/03/13 04:34:32 brianp Exp $ */

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


#ifndef IMAGE_H
#define IMAGE_H


#include "mtypes.h"


extern const struct gl_pixelstore_attrib _mesa_native_packing;


extern void
_mesa_swap2( GLushort *p, GLuint n );

extern void
_mesa_swap4( GLuint *p, GLuint n );

extern GLint
_mesa_sizeof_type( GLenum type );

extern GLint
_mesa_sizeof_packed_type( GLenum type );

extern GLint
_mesa_components_in_format( GLenum format );

extern GLint
_mesa_bytes_per_pixel( GLenum format, GLenum type );

extern GLboolean
_mesa_is_legal_format_and_type( GLenum format, GLenum type );


extern GLvoid *
_mesa_image_address( const struct gl_pixelstore_attrib *packing,
                     const GLvoid *image, GLsizei width,
                     GLsizei height, GLenum format, GLenum type,
                     GLint img, GLint row, GLint column );


extern GLint
_mesa_image_row_stride( const struct gl_pixelstore_attrib *packing,
                        GLint width, GLenum format, GLenum type );


extern GLint
_mesa_image_image_stride( const struct gl_pixelstore_attrib *packing,
                          GLint width, GLint height,
                          GLenum format, GLenum type );

extern void
_mesa_unpack_polygon_stipple( const GLubyte *pattern, GLuint dest[32],
                              const struct gl_pixelstore_attrib *unpacking );


extern void
_mesa_pack_polygon_stipple( const GLuint pattern[32], GLubyte *dest,
                            const struct gl_pixelstore_attrib *packing );


extern GLvoid *
_mesa_unpack_bitmap( GLint width, GLint height, const GLubyte *pixels,
                     const struct gl_pixelstore_attrib *packing );

extern void
_mesa_pack_bitmap( GLint width, GLint height, const GLubyte *source,
                   GLubyte *dest, const struct gl_pixelstore_attrib *packing );


extern void
_mesa_pack_float_rgba_span( GLcontext *ctx,
                            GLuint n, CONST GLfloat rgba[][4],
                            GLenum dstFormat, GLenum dstType, GLvoid *dstAddr,
                            const struct gl_pixelstore_attrib *dstPacking,
                            GLuint transferOps );


extern void
_mesa_pack_rgba_span( GLcontext *ctx,
                      GLuint n, CONST GLchan rgba[][4],
                      GLenum dstFormat, GLenum dstType, GLvoid *dstAddr,
                      const struct gl_pixelstore_attrib *dstPacking,
                      GLuint transferOps );


extern void
_mesa_unpack_chan_color_span( GLcontext *ctx,
                              GLuint n, GLenum dstFormat, GLchan dest[],
                              GLenum srcFormat, GLenum srcType,
                              const GLvoid *source,
                              const struct gl_pixelstore_attrib *srcPacking,
                              GLuint transferOps );


extern void
_mesa_unpack_float_color_span( GLcontext *ctx,
                               GLuint n, GLenum dstFormat, GLfloat dest[],
                               GLenum srcFormat, GLenum srcType,
                               const GLvoid *source,
                               const struct gl_pixelstore_attrib *srcPacking,
                               GLuint transferOps, GLboolean clamp );


extern void
_mesa_unpack_index_span( const GLcontext *ctx, GLuint n,
                         GLenum dstType, GLvoid *dest,
                         GLenum srcType, const GLvoid *source,
                         const struct gl_pixelstore_attrib *srcPacking,
                         GLuint transferOps );


extern void
_mesa_pack_index_span( const GLcontext *ctx, GLuint n,
                       GLenum dstType, GLvoid *dest, const GLuint *source,
                       const struct gl_pixelstore_attrib *dstPacking,
                       GLuint transferOps );


extern void
_mesa_unpack_stencil_span( const GLcontext *ctx, GLuint n,
                           GLenum dstType, GLvoid *dest,
                           GLenum srcType, const GLvoid *source,
                           const struct gl_pixelstore_attrib *srcPacking,
                           GLuint transferOps );

extern void
_mesa_pack_stencil_span( const GLcontext *ctx, GLuint n,
                         GLenum dstType, GLvoid *dest, const GLstencil *source,
                         const struct gl_pixelstore_attrib *dstPacking );


extern void
_mesa_unpack_depth_span( const GLcontext *ctx, GLuint n, GLfloat *dest,
                         GLenum srcType, const GLvoid *source,
                         const struct gl_pixelstore_attrib *srcPacking );

extern void
_mesa_pack_depth_span( const GLcontext *ctx, GLuint n, GLvoid *dest,
                       GLenum dstType, const GLfloat *depthSpan,
                       const struct gl_pixelstore_attrib *dstPacking );


extern void *
_mesa_unpack_image( GLsizei width, GLsizei height, GLsizei depth,
                    GLenum format, GLenum type, const GLvoid *pixels,
                    const struct gl_pixelstore_attrib *unpack );


#endif
