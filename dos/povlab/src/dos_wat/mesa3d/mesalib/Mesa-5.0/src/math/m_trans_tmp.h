/* $Id: m_trans_tmp.h,v 1.7 2002/01/30 16:52:02 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  4.0.2
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

/*
 * New (3.1) transformation code written by Keith Whitwell.
 */


/* KW: This file also included by tnl/trans_elt.c to build code
 *     specific to the implementation of array-elements in the
 *     tnl module.
 */


#ifdef DEST_4F
static void DEST_4F( GLfloat (*t)[4],
		     CONST void *ptr,
		     GLuint stride,
		     ARGS )
{
   const GLubyte *f = (GLubyte *) ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;

   (void) first;
   (void) start;
   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	 if (SZ >= 1) t[i][0] = TRX_4F(f, 0);
	 if (SZ >= 2) t[i][1] = TRX_4F(f, 1);
	 if (SZ >= 3) t[i][2] = TRX_4F(f, 2);
	 if (SZ == 4) t[i][3] = TRX_4F(f, 3); else t[i][3] = 1.0;
      }
   }
}
#endif


#ifdef DEST_3F
static void DEST_3F( GLfloat (*t)[3],
		     CONST void *ptr,
		     GLuint stride,
		     ARGS )
{
   const GLubyte *f = (GLubyte *) ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;
   (void) first;
   (void) start;
   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	 t[i][0] = TRX_3F(f, 0);
	 t[i][1] = TRX_3F(f, 1);
	 t[i][2] = TRX_3F(f, 2);
      }
   }
}
#endif

#ifdef DEST_1F
static void DEST_1F( GLfloat *t,
		     CONST void *ptr,
		     GLuint stride,
		     ARGS )
{
   const GLubyte *f = (GLubyte *) ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;
   (void) first;
   (void) start;
   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	 t[i] = TRX_1F(f, 0);
      }
   }
}
#endif

#ifdef DEST_4UB
static void DEST_4UB( GLubyte (*t)[4],
                      CONST void *ptr,
                      GLuint stride,
                      ARGS )
{
   const GLubyte *f = (GLubyte *) ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;
   (void) start;
   (void) first;
   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	 if (SZ >= 1) TRX_UB(t[i][0], f, 0);
	 if (SZ >= 2) TRX_UB(t[i][1], f, 1);
	 if (SZ >= 3) TRX_UB(t[i][2], f, 2);
	 if (SZ == 4) TRX_UB(t[i][3], f, 3); else t[i][3] = 255;
      }
   }
}
#endif


#ifdef DEST_4US
static void DEST_4US( GLushort (*t)[4],
                      CONST void *ptr,
                      GLuint stride,
                      ARGS )
{
   const GLubyte *f = (GLubyte *) ((GLubyte *) ptr + SRC_START * stride);
   const GLubyte *first = f;
   GLuint i;
   (void) start;
   (void) first;
   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	 if (SZ >= 1) TRX_US(t[i][0], f, 0);
	 if (SZ >= 2) TRX_US(t[i][1], f, 1);
	 if (SZ >= 3) TRX_US(t[i][2], f, 2);
	 if (SZ == 4) TRX_US(t[i][3], f, 3); else t[i][3] = 65535;
      }
   }
}
#endif


#ifdef DEST_1UB
static void DEST_1UB( GLubyte *t,
		      CONST void *ptr,
		      GLuint stride,
		      ARGS )
{
   const GLubyte *f = (GLubyte *) ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;
   (void) start;
   (void) first;
   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	  TRX_UB(t[i], f, 0);
      }
   }
}
#endif


#ifdef DEST_1UI
static void DEST_1UI( GLuint *t,
		      CONST void *ptr,
		      GLuint stride,
		      ARGS )
{
   const GLubyte *f = (GLubyte *) ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;
   (void) start;
   (void) first;

   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	 t[i] = TRX_UI(f, 0);
      }
   }
}
#endif


static void INIT(void)
{
#ifdef DEST_1UI
   ASSERT(SZ == 1);
   TAB(_1ui)[SRC_IDX] = DEST_1UI;
#endif
#ifdef DEST_1UB
   ASSERT(SZ == 1);
   TAB(_1ub)[SRC_IDX] = DEST_1UB;
#endif
#ifdef DEST_1F
   ASSERT(SZ == 1);
   TAB(_1f)[SRC_IDX] = DEST_1F;
#endif
#ifdef DEST_3F
   ASSERT(SZ == 3);
   TAB(_3f)[SRC_IDX] = DEST_3F;
#endif
#ifdef DEST_4UB
   TAB(_4ub)[SZ][SRC_IDX] = DEST_4UB;
#endif
#ifdef DEST_4US
   TAB(_4us)[SZ][SRC_IDX] = DEST_4US;
#endif
#ifdef DEST_4F
   TAB(_4f)[SZ][SRC_IDX] = DEST_4F;
#endif

}


#undef INIT
#undef DEST_1UI
#undef DEST_1UB
#undef DEST_4UB
#undef DEST_4US
#undef DEST_3F
#undef DEST_4F
#undef DEST_1F
#undef SZ
#undef TAG
