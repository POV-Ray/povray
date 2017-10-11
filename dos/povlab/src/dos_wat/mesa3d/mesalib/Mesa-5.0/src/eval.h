/* $Id: eval.h,v 1.6 2001/03/12 00:48:37 gareth Exp $ */

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


#ifndef EVAL_H
#define EVAL_H


#include "mtypes.h"


extern void _mesa_init_eval( void );


extern GLuint _mesa_evaluator_components( GLenum target );


extern void gl_free_control_points( GLcontext *ctx,
                                    GLenum target, GLfloat *data );


extern GLfloat *_mesa_copy_map_points1f( GLenum target,
                                      GLint ustride, GLint uorder,
                                      const GLfloat *points );

extern GLfloat *_mesa_copy_map_points1d( GLenum target,
                                      GLint ustride, GLint uorder,
                                      const GLdouble *points );

extern GLfloat *_mesa_copy_map_points2f( GLenum target,
                                      GLint ustride, GLint uorder,
                                      GLint vstride, GLint vorder,
                                      const GLfloat *points );

extern GLfloat *_mesa_copy_map_points2d(GLenum target,
                                     GLint ustride, GLint uorder,
                                     GLint vstride, GLint vorder,
                                     const GLdouble *points );



extern void
_mesa_Map1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride,
             GLint order, const GLfloat *points );

extern void
_mesa_Map2f( GLenum target,
             GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
             GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
             const GLfloat *points );

extern void
_mesa_Map1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride,
             GLint order, const GLdouble *points );

extern void
_mesa_Map2d( GLenum target,
             GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
             GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
             const GLdouble *points );

extern void
_mesa_MapGrid1f( GLint un, GLfloat u1, GLfloat u2 );

extern void
_mesa_MapGrid1d( GLint un, GLdouble u1, GLdouble u2 );

extern void
_mesa_MapGrid2f( GLint un, GLfloat u1, GLfloat u2,
                 GLint vn, GLfloat v1, GLfloat v2 );

extern void
_mesa_MapGrid2d( GLint un, GLdouble u1, GLdouble u2,
                 GLint vn, GLdouble v1, GLdouble v2 );

extern void
_mesa_GetMapdv( GLenum target, GLenum query, GLdouble *v );

extern void
_mesa_GetMapfv( GLenum target, GLenum query, GLfloat *v );

extern void
_mesa_GetMapiv( GLenum target, GLenum query, GLint *v );


#endif
