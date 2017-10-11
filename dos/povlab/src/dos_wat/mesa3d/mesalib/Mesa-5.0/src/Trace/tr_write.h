/* This may look like C code, but it is really -*- C++ -*-  */
/* $Id: tr_write.h,v 1.1 2000/11/11 01:42:30 brianp Exp $ */

/*
 * DebugGL
 * Version:  1.0
 * 
 * Copyright (C) 1999-2000  Loki Entertainment
 * All Rights Reserved.
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

#ifndef __TR_WRITE_H
#define __TR_WRITE_H

#include "GL/gl.h"

void trWriteCMD( GLint opcode );
void trWriteb( GLbyte byte );
void trWriteub( GLubyte ub );
void trWrited( GLdouble d );
void trWritef( GLfloat f );
void trWritei( GLint i );
void trWriteui( GLuint ui );
void trWrites( GLshort s );
void trWriteus( GLushort us );
void trWriteBool( GLboolean b );
void trWriteBits( GLbitfield bf );
void trWriteEnum( GLenum en );
void trWriteSizei( GLsizei si );
void trWriteClampf( GLclampf cf );
void trWriteClampd( GLclampd cd );
void trWritePointer( const void * p );

void trWriteArrayb( GLsizei n, const GLbyte * b );
void trWriteArrayub( GLsizei n, const GLubyte * ub );
void trWriteArrays( GLsizei n, const GLshort * s );
void trWriteArrayus( GLsizei n, const GLushort * us );
void trWriteArrayi( GLsizei n, const GLint * i );
void trWriteArrayui( GLsizei n, const GLuint * ui );
void trWriteArrayBool( GLsizei n, const GLboolean * b );
void trWriteArrayf( GLsizei n, const GLfloat * f );
void trWriteArrayd( GLsizei n, const GLdouble * d );

void trWriteString( const char * str );

void trFileFlush( void );

void trOpenLogFile( void );
void trCloseLogFile( void );

#endif
