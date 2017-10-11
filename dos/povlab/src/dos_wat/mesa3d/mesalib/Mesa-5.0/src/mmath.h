/* $Id: mmath.h,v 1.63 2002/11/06 13:39:23 joukj Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  5.0
 *
 * Copyright (C) 1999-2002 Brian Paul   All Rights Reserved.
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
 * Faster arithmetic functions.  If the FAST_MATH preprocessor symbol is
 * defined on the command line (-DFAST_MATH) then we'll use some (hopefully)
 * faster functions for sqrt(), etc.
 */


#ifndef MMATH_H
#define MMATH_H


#include "glheader.h"
#include "imports.h"
/* Do not reference mtypes.h from this file.
 */

/*
 * Set the x86 FPU control word to guarentee only 32 bits of presision
 * are stored in registers.  Allowing the FPU to store more introduces
 * differences between situations where numbers are pulled out of memory
 * vs. situations where the compiler is able to optimize register usage.
 *
 * In the worst case, we force the compiler to use a memory access to
 * truncate the float, by specifying the 'volatile' keyword.
 */
#if defined(__GNUC__) && defined(__i386__)

/* Hardware default: All exceptions masked, extended double precision,
 * round to nearest.  IEEE compliant.
 */
#define DEFAULT_X86_FPU		0x037f

/* All exceptions masked, single precision, round to nearest.
 */
#define FAST_X86_FPU		0x003f

/* Set it up how we want it.  The fldcw instruction will cause any
 * pending FP exceptions to be raised prior to entering the block, and
 * we clear any pending exceptions before exiting the block.  Hence, asm
 * code has free reign over the FPU while in the fast math block.
 */
#if defined(NO_FAST_MATH)
#define START_FAST_MATH(x)						\
do {									\
   static GLuint mask = DEFAULT_X86_FPU;				\
   __asm__ ( "fnstcw %0" : "=m" (*&(x)) );				\
   __asm__ ( "fldcw %0" : : "m" (mask) );				\
} while (0)
#else
#define START_FAST_MATH(x)						\
do {									\
   static GLuint mask = FAST_X86_FPU;					\
   __asm__ ( "fnstcw %0" : "=m" (*&(x)) );				\
   __asm__ ( "fldcw %0" : : "m" (mask) );				\
} while (0)
#endif

/* Put it back how the application had it, and clear any exceptions that
 * may have occurred in the FAST_MATH block.
 */
#define END_FAST_MATH(x)						\
do {									\
   __asm__ ( "fnclex ; fldcw %0" : : "m" (*&(x)) );			\
} while (0)

#define HAVE_FAST_MATH

#elif defined(__WATCOMC__) && !defined(NO_FAST_MATH)

/* This is the watcom specific inline assembly version of setcw and getcw */

void START_FAST_MATH2(unsigned short *x);
#pragma aux START_FAST_MATH2 =          \
    "fstcw   word ptr [esi]"            \
    "or      word ptr [esi], 0x3f"      \
    "fldcw   word ptr [esi]"            \
    parm [esi]                          \
    modify exact [];

void END_FAST_MATH2(unsigned short *x);
#pragma aux END_FAST_MATH2 =            \
    "fldcw   word ptr [esi]"            \
    parm [esi]                          \
    modify exact [];

#define START_FAST_MATH(x)  START_FAST_MATH2(& x)
#define END_FAST_MATH(x)  END_FAST_MATH2(& x)

/*
__inline START_FAST_MATH(unsigned short x)
    {
    _asm {
        fstcw   ax
        mov     x , ax
        or      ax, 0x3f
        fldcw   ax
        }
    }

__inline END_FAST_MATH(unsigned short x)
    {
    _asm {
        fldcw   x
        }
    }
*/
#define HAVE_FAST_MATH

#else
#define START_FAST_MATH(x) x = 0
#define END_FAST_MATH(x)   (void)(x)

/* The mac float really is a float, with the same precision as a
 * single precision 387 float.
 */
#if defined(macintosh) || defined(__powerpc__)
#define HAVE_FAST_MATH
#endif

#endif



/*
 * Square root
 */

extern float gl_sqrt(float x);

#ifdef FAST_MATH
#if defined(__WATCOMC__) && defined(USE_X86_ASM)
#  define GL_SQRT(X)  asm_sqrt(X)
#else
#  define GL_SQRT(X)  gl_sqrt(X)
#endif
#else
#  define GL_SQRT(X)  sqrt(X)
#endif


/*
 * Normalize a 3-element vector to unit length.
 */
#define NORMALIZE_3FV( V )			\
do {						\
   GLfloat len = (GLfloat) LEN_SQUARED_3FV(V);	\
   if (len) {					\
      len = (GLfloat) (1.0 / GL_SQRT(len));	\
      (V)[0] = (GLfloat) ((V)[0] * len);	\
      (V)[1] = (GLfloat) ((V)[1] * len);	\
      (V)[2] = (GLfloat) ((V)[2] * len);	\
   }						\
} while(0)

#define LEN_3FV( V ) (GL_SQRT((V)[0]*(V)[0]+(V)[1]*(V)[1]+(V)[2]*(V)[2]))
#define LEN_2FV( V ) (GL_SQRT((V)[0]*(V)[0]+(V)[1]*(V)[1]))

#define LEN_SQUARED_3FV( V ) ((V)[0]*(V)[0]+(V)[1]*(V)[1]+(V)[2]*(V)[2])
#define LEN_SQUARED_2FV( V ) ((V)[0]*(V)[0]+(V)[1]*(V)[1])


/*
 * Single precision ceiling, floor, and absolute value functions
 */
#if defined(__sparc__) /* XXX this probably isn't the ideal test */
#define CEILF(x)   ceil(x)
#define FLOORF(x)  floor(x)
#define FABSF(x)   fabs(x)
#elif defined(__WIN32__)
#define CEILF(x)   ((GLfloat)ceil(x))
#define FLOORF(x)  ((GLfloat)floor(x))
#define FABSF(x)   ((GLfloat)fabs(x))
#else
#define CEILF(x)   ceilf(x)
#define FLOORF(x)  floorf(x)
#define FABSF(x)   fabsf(x)
#endif


#if defined(__i386__) || defined(__sparc__) || defined(__s390x__) || \
    defined(__powerpc__) || \
    ( defined(__alpha__) && ( defined(__IEEE_FLOAT) || !defined(VMS) ) )
#define USE_IEEE
#endif



#define GET_FLOAT_BITS(x) ((fi_type *) &(x))->i

/*
 * Float -> Int conversions (rounding, floor, ceiling)
 */

#if defined(USE_SPARC_ASM) && defined(__GNUC__) && defined(__sparc__)

static INLINE int iround(float f)
{
       int r;
       __asm__ ("fstoi %1, %0" : "=f" (r) : "f" (f));
       return r;
}

#define IROUND(x)  iround(x)

#elif defined(USE_X86_ASM) && defined(__GNUC__) && defined(__i386__)


static INLINE int iround(float f)
{
   int r;
   __asm__ ("fistpl %0" : "=m" (r) : "t" (f) : "st");
   return r;
}

#define IROUND(x)  iround(x)

/*
 * IEEE floor for computers that round to nearest or even.
 * 'f' must be between -4194304 and 4194303.
 * This floor operation is done by "(iround(f + .5) + iround(f - .5)) >> 1",
 * but uses some IEEE specific tricks for better speed.
 * Contributed by Josh Vanderhoof
 */
static INLINE int ifloor(float f)
{
   int ai, bi;
   double af, bf;
   af = (3 << 22) + 0.5 + (double)f;
   bf = (3 << 22) + 0.5 - (double)f;
   /* GCC generates an extra fstp/fld without this. */
   __asm__ ("fstps %0" : "=m" (ai) : "t" (af) : "st");
   __asm__ ("fstps %0" : "=m" (bi) : "t" (bf) : "st");
   return (ai - bi) >> 1;
}

#define IFLOOR(x)  ifloor(x)

/*
 * IEEE ceil for computers that round to nearest or even.
 * 'f' must be between -4194304 and 4194303.
 * This ceil operation is done by "(iround(f + .5) + iround(f - .5) + 1) >> 1",
 * but uses some IEEE specific tricks for better speed.
 * Contributed by Josh Vanderhoof
 */
static INLINE int iceil(float f)
{
   int ai, bi;
   double af, bf;
   af = (3 << 22) + 0.5 + (double)f;
   bf = (3 << 22) + 0.5 - (double)f;
   /* GCC generates an extra fstp/fld without this. */
   __asm__ ("fstps %0" : "=m" (ai) : "t" (af) : "st");
   __asm__ ("fstps %0" : "=m" (bi) : "t" (bf) : "st");
   return (ai - bi + 1) >> 1;
}

#define ICEIL(x)  iceil(x)


#elif defined(USE_X86_ASM) && defined(__MSC__) && defined(__WIN32__)


static INLINE int iround(float f)
{
   int r;
   _asm {
	 fld f
	 fistp r
	}
   return r;
}

#define IROUND(x)  iround(x)


#elif defined(USE_X86_ASM) && defined(__WATCOMC__)


long iround(float f);
#pragma aux iround =                        \
	"push   eax"                        \
	"fistp  dword ptr [esp]"            \
	"pop    eax"                        \
	parm [8087]                         \
	value [eax]                         \
	modify exact [eax];

#define IROUND(x)  iround(x)

float asm_sqrt (float x);
#pragma aux asm_sqrt =                      \
	"fsqrt"                             \
	parm [8087]                         \
	value [8087]                        \
	modify exact [];


#endif /* assembly/optimized IROUND, IROUND_POS, IFLOOR, ICEIL macros */


/* default IROUND macro */
#ifndef IROUND
#define IROUND(f)  ((int) (((f) >= 0.0F) ? ((f) + 0.5F) : ((f) - 0.5F)))
#endif


/* default IROUND_POS macro */
#ifndef IROUND_POS
#ifdef DEBUG
#define IROUND_POS(f) (ASSERT((f) >= 0.0F), IROUND(f))
#else
#define IROUND_POS(f) (IROUND(f))
#endif
#endif /* IROUND_POS */


/* default IFLOOR macro */
#ifndef IFLOOR
static INLINE int ifloor(float f)
{
#ifdef USE_IEEE
   int ai, bi;
   double af, bf;
   union { int i; float f; } u;

   af = (3 << 22) + 0.5 + (double)f;
   bf = (3 << 22) + 0.5 - (double)f;
   u.f = af; ai = u.i;
   u.f = bf; bi = u.i;
   return (ai - bi) >> 1;
#else
   int i = IROUND(f);
   return (i > f) ? i - 1 : i;
#endif
}
#define IFLOOR(x)  ifloor(x)
#endif /* IFLOOR */


/* default ICEIL macro */
#ifndef ICEIL
static INLINE int iceil(float f)
{
#ifdef USE_IEEE
   int ai, bi;
   double af, bf;
   union { int i; float f; } u;
   af = (3 << 22) + 0.5 + (double)f;
   bf = (3 << 22) + 0.5 - (double)f;
   u.f = af; ai = u.i;
   u.f = bf; bi = u.i;
   return (ai - bi + 1) >> 1;
#else
   int i = IROUND(f);
   return (i < f) ? i + 1 : i;
#endif
}
#define ICEIL(x)  iceil(x)
#endif /* ICEIL */



/*
 * Convert unclamped or clamped ([0,1]) floats to ubytes for color
 * conversion only.  These functions round to the nearest int.
 */
#define IEEE_ONE 0x3f800000
#define IEEE_0996 0x3f7f0000	/* 0.996 or something??? used in macro
                                   below only */

#if defined(USE_IEEE) && !defined(DEBUG)

/*
 * This function/macro is sensitive to precision.  Test carefully
 * if you change it.
 */
#define UNCLAMPED_FLOAT_TO_UBYTE(ub, f)					\
        do {								\
           union { GLfloat r; GLuint i; } __tmp;			\
           __tmp.r = (f);						\
           ub = ((__tmp.i >= IEEE_0996)					\
               ? ((GLint)__tmp.i < 0) ? (GLubyte)0 : (GLubyte)255	\
               : (__tmp.r = __tmp.r*(255.0F/256.0F) + 32768.0F,		\
                  (GLubyte)__tmp.i));					\
        } while (0)

#define CLAMPED_FLOAT_TO_UBYTE(ub, f) \
        UNCLAMPED_FLOAT_TO_UBYTE(ub, f)

#define COPY_FLOAT( dst, src )					\
	((fi_type *) &(dst))->i = ((fi_type *) &(src))->i

#else /* USE_IEEE */

#define UNCLAMPED_FLOAT_TO_UBYTE(ub, f) \
	ub = ((GLubyte) IROUND(CLAMP((f), 0.0F, 1.0F) * 255.0F))

#define CLAMPED_FLOAT_TO_UBYTE(ub, f) \
	ub = ((GLubyte) IROUND((f) * 255.0F))

#define COPY_FLOAT( dst, src )		(dst) = (src)

#endif /* USE_IEEE */



/*
 * Integer / float conversion for colors, normals, etc.
 */

/* Convert GLubyte in [0,255] to GLfloat in [0.0,1.0] */
extern float _mesa_ubyte_to_float_color_tab[256];
#define UBYTE_TO_FLOAT(u) _mesa_ubyte_to_float_color_tab[(unsigned int)(u)]

/* Convert GLfloat in [0.0,1.0] to GLubyte in [0,255] */
#define FLOAT_TO_UBYTE(X)	((GLubyte) (GLint) ((X) * 255.0F))


/* Convert GLbyte in [-128,127] to GLfloat in [-1.0,1.0] */
#define BYTE_TO_FLOAT(B)	((2.0F * (B) + 1.0F) * (1.0F/255.0F))

/* Convert GLfloat in [-1.0,1.0] to GLbyte in [-128,127] */
#define FLOAT_TO_BYTE(X)	( (((GLint) (255.0F * (X))) - 1) / 2 )


/* Convert GLushort in [0,65536] to GLfloat in [0.0,1.0] */
#define USHORT_TO_FLOAT(S)	((GLfloat) (S) * (1.0F / 65535.0F))

/* Convert GLfloat in [0.0,1.0] to GLushort in [0,65536] */
#define FLOAT_TO_USHORT(X)	((GLushort) (GLint) ((X) * 65535.0F))


/* Convert GLshort in [-32768,32767] to GLfloat in [-1.0,1.0] */
#define SHORT_TO_FLOAT(S)	((2.0F * (S) + 1.0F) * (1.0F/65535.0F))

/* Convert GLfloat in [0.0,1.0] to GLshort in [-32768,32767] */
#define FLOAT_TO_SHORT(X)	( (((GLint) (65535.0F * (X))) - 1) / 2 )


/* Convert GLuint in [0,4294967295] to GLfloat in [0.0,1.0] */
#define UINT_TO_FLOAT(U)	((GLfloat) (U) * (1.0F / 4294967295.0F))

/* Convert GLfloat in [0.0,1.0] to GLuint in [0,4294967295] */
#define FLOAT_TO_UINT(X)	((GLuint) ((X) * 4294967295.0))


/* Convert GLint in [-2147483648,2147483647] to GLfloat in [-1.0,1.0] */
#define INT_TO_FLOAT(I)		((2.0F * (I) + 1.0F) * (1.0F/4294967294.0F))

/* Convert GLfloat in [-1.0,1.0] to GLint in [-2147483648,2147483647] */
/* causes overflow:
#define FLOAT_TO_INT(X)		( (((GLint) (4294967294.0F * (X))) - 1) / 2 )
*/
/* a close approximation: */
#define FLOAT_TO_INT(X)		( (GLint) (2147483647.0 * (X)) )


#define BYTE_TO_UBYTE(b)   ((GLubyte) ((b) < 0 ? 0 : (GLubyte) (b)))
#define SHORT_TO_UBYTE(s)  ((GLubyte) ((s) < 0 ? 0 : (GLubyte) ((s) >> 7)))
#define USHORT_TO_UBYTE(s) ((GLubyte) ((s) >> 8))
#define INT_TO_UBYTE(i)    ((GLubyte) ((i) < 0 ? 0 : (GLubyte) ((i) >> 23)))
#define UINT_TO_UBYTE(i)   ((GLubyte) ((i) >> 24))


#define BYTE_TO_USHORT(b)  ((b) < 0 ? 0 : ((GLushort) (((b) * 65535) / 255)))
#define UBYTE_TO_USHORT(b) (((GLushort) (b) << 8) | (GLushort) (b))
#define SHORT_TO_USHORT(s) ((s) < 0 ? 0 : ((GLushort) (((s) * 65535 / 32767))))
#define INT_TO_USHORT(i)   ((i) < 0 ? 0 : ((GLushort) ((i) >> 15)))
#define UINT_TO_USHORT(i)  ((i) < 0 ? 0 : ((GLushort) ((i) >> 16)))
#define UNCLAMPED_FLOAT_TO_USHORT(us, f)  \
        us = ( (GLushort) IROUND( CLAMP((f), 0.0, 1.0) * 65535.0F) )
#define CLAMPED_FLOAT_TO_USHORT(us, f)  \
        us = ( (GLushort) IROUND( (f) * 65535.0F) )



/*
 * Linear interpolation
 * NOTE:  OUT argument is evaluated twice!
 * NOTE:  Be wary of using *coord++ as an argument to any of these macros!
 */
#define LINTERP(T, OUT, IN)	((OUT) + (T) * ((IN) - (OUT)))

/* Can do better with integer math:
 */
#define INTERP_UB( t, dstub, outub, inub )	\
do {						\
   GLfloat inf = UBYTE_TO_FLOAT( inub );	\
   GLfloat outf = UBYTE_TO_FLOAT( outub );	\
   GLfloat dstf = LINTERP( t, outf, inf );	\
   UNCLAMPED_FLOAT_TO_UBYTE( dstub, dstf );	\
} while (0)

#define INTERP_CHAN( t, dstc, outc, inc )	\
do {						\
   GLfloat inf = CHAN_TO_FLOAT( inc );		\
   GLfloat outf = CHAN_TO_FLOAT( outc );	\
   GLfloat dstf = LINTERP( t, outf, inf );	\
   UNCLAMPED_FLOAT_TO_CHAN( dstc, dstf );	\
} while (0)

#define INTERP_UI( t, dstui, outui, inui )	\
   dstui = (GLuint) (GLint) LINTERP( (t), (GLfloat) (outui), (GLfloat) (inui) )

#define INTERP_F( t, dstf, outf, inf )		\
   dstf = LINTERP( t, outf, inf )

#define INTERP_4F( t, dst, out, in )		\
do {						\
   dst[0] = LINTERP( (t), (out)[0], (in)[0] );	\
   dst[1] = LINTERP( (t), (out)[1], (in)[1] );	\
   dst[2] = LINTERP( (t), (out)[2], (in)[2] );	\
   dst[3] = LINTERP( (t), (out)[3], (in)[3] );	\
} while (0)

#define INTERP_3F( t, dst, out, in )		\
do {						\
   dst[0] = LINTERP( (t), (out)[0], (in)[0] );	\
   dst[1] = LINTERP( (t), (out)[1], (in)[1] );	\
   dst[2] = LINTERP( (t), (out)[2], (in)[2] );	\
} while (0)

#define INTERP_4CHAN( t, dst, out, in )			\
do {							\
   INTERP_CHAN( (t), (dst)[0], (out)[0], (in)[0] );	\
   INTERP_CHAN( (t), (dst)[1], (out)[1], (in)[1] );	\
   INTERP_CHAN( (t), (dst)[2], (out)[2], (in)[2] );	\
   INTERP_CHAN( (t), (dst)[3], (out)[3], (in)[3] );	\
} while (0)

#define INTERP_3CHAN( t, dst, out, in )			\
do {							\
   INTERP_CHAN( (t), (dst)[0], (out)[0], (in)[0] );	\
   INTERP_CHAN( (t), (dst)[1], (out)[1], (in)[1] );	\
   INTERP_CHAN( (t), (dst)[2], (out)[2], (in)[2] );	\
} while (0)

#define INTERP_SZ( t, vec, to, out, in, sz )				\
do {									\
   switch (sz) {							\
   case 4: vec[to][3] = LINTERP( (t), (vec)[out][3], (vec)[in][3] );	\
   case 3: vec[to][2] = LINTERP( (t), (vec)[out][2], (vec)[in][2] );	\
   case 2: vec[to][1] = LINTERP( (t), (vec)[out][1], (vec)[in][1] );	\
   case 1: vec[to][0] = LINTERP( (t), (vec)[out][0], (vec)[in][0] );	\
   }									\
} while(0)


/*
 * Fixed point arithmetic macros
 */
#ifdef FIXED_14
#define FIXED_ONE       0x00004000
#define FIXED_HALF      0x00002000
#define FIXED_FRAC_MASK 0x00003FFF
#define FIXED_SCALE     16384.0f
#define FIXED_SHIFT     14
#else
#define FIXED_ONE       0x00000800
#define FIXED_HALF      0x00000400
#define FIXED_FRAC_MASK 0x000007FF
#define FIXED_SCALE     2048.0f
#define FIXED_SHIFT     11
#endif
#define FIXED_INT_MASK  (~FIXED_FRAC_MASK)
#define FIXED_EPSILON   1
#define FloatToFixed(X) (IROUND((X) * FIXED_SCALE))
#define IntToFixed(I)   ((I) << FIXED_SHIFT)
#define FixedToInt(X)   ((X) >> FIXED_SHIFT)
#define FixedToUns(X)   (((unsigned int)(X)) >> FIXED_SHIFT)
#define FixedCeil(X)    (((X) + FIXED_ONE - FIXED_EPSILON) & FIXED_INT_MASK)
#define FixedFloor(X)   ((X) & FIXED_INT_MASK)
#define FixedToFloat(X) ((X) * (1.0F / FIXED_SCALE))
#define PosFloatToFixed(X)      FloatToFixed(X)
#define SignedFloatToFixed(X)   FloatToFixed(X)

/* Returns TRUE for x == Inf or x == NaN. */
#ifdef USE_IEEE
static INLINE int IS_INF_OR_NAN( float x )
{
   union {float f; int i;} tmp;
   tmp.f = x;
   return !(int)((unsigned int)((tmp.i & 0x7fffffff)-0x7f800000) >> 31);
}
#elif defined(isfinite)
#define IS_INF_OR_NAN(x)        (!isfinite(x))
#elif defined(finite)
#define IS_INF_OR_NAN(x)        (!finite(x))
#elif __VMS
#define IS_INF_OR_NAN(x)        (!finite(x))
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define IS_INF_OR_NAN(x)        (!isfinite(x))
#else
#define IS_INF_OR_NAN(x)        (!finite(x))
#endif 


/*
 * Return log_base_2(x).
 */
#ifdef USE_IEEE

#if 0
/* This is pretty fast, but not accurate enough (only 2 fractional bits).
 * Based on code from http://www.stereopsis.com/log2.html
 */
static INLINE GLfloat LOG2(GLfloat x)
{
   const GLfloat y = x * x * x * x;
   const GLuint ix = *((GLuint *) &y);
   const GLuint exp = (ix >> 23) & 0xFF;
   const GLint log2 = ((GLint) exp) - 127;
   return (GLfloat) log2 * (1.0 / 4.0);  /* 4, because of x^4 above */
}
#endif

/* Pretty fast, and accurate.
 * Based on code from http://www.flipcode.com/totd/
 */
static INLINE GLfloat LOG2(GLfloat val)
{
   GLint *exp_ptr = (GLint *) &val;
   GLint x = *exp_ptr;
   const GLint log_2 = ((x >> 23) & 255) - 128;
   x &= ~(255 << 23);
   x += 127 << 23;
   *exp_ptr = x;
   val = ((-1.0f/3) * val + 2) * val - 2.0f/3;
   return val + log_2;
}

#else /* USE_IEEE */

/* Slow, portable solution.
 * NOTE: log_base_2(x) = log(x) / log(2)
 * NOTE: 1.442695 = 1/log(2).
 */
#define LOG2(x)  ((GLfloat) (log(x) * 1.442695F))

#endif /* USE_IEEE */


extern void
_mesa_init_math(void);


extern GLuint
_mesa_bitcount(GLuint n);


#endif
