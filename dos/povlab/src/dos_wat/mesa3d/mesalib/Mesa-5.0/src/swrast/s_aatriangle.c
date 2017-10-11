/* $Id: s_aatriangle.c,v 1.26 2002/10/24 23:57:24 brianp Exp $ */

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


/*
 * Antialiased Triangle rasterizers
 */


#include "glheader.h"
#include "macros.h"
#include "imports.h"
#include "mmath.h"
#include "s_aatriangle.h"
#include "s_context.h"
#include "s_span.h"


/*
 * Compute coefficients of a plane using the X,Y coords of the v0, v1, v2
 * vertices and the given Z values.
 * A point (x,y,z) lies on plane iff a*x+b*y+c*z+d = 0.
 */
static INLINE void
compute_plane(const GLfloat v0[], const GLfloat v1[], const GLfloat v2[],
              GLfloat z0, GLfloat z1, GLfloat z2, GLfloat plane[4])
{
   const GLfloat px = v1[0] - v0[0];
   const GLfloat py = v1[1] - v0[1];
   const GLfloat pz = z1 - z0;

   const GLfloat qx = v2[0] - v0[0];
   const GLfloat qy = v2[1] - v0[1];
   const GLfloat qz = z2 - z0;

   /* Crossproduct "(a,b,c):= dv1 x dv2" is orthogonal to plane. */
   const GLfloat a = py * qz - pz * qy;
   const GLfloat b = pz * qx - px * qz;
   const GLfloat c = px * qy - py * qx;
   /* Point on the plane = "r*(a,b,c) + w", with fixed "r" depending
      on the distance of plane from origin and arbitrary "w" parallel
      to the plane. */
   /* The scalar product "(r*(a,b,c)+w)*(a,b,c)" is "r*(a^2+b^2+c^2)",
      which is equal to "-d" below. */
   const GLfloat d = -(a * v0[0] + b * v0[1] + c * z0);

   plane[0] = a;
   plane[1] = b;
   plane[2] = c;
   plane[3] = d;
}


/*
 * Compute coefficients of a plane with a constant Z value.
 */
static INLINE void
constant_plane(GLfloat value, GLfloat plane[4])
{
   plane[0] = 0.0;
   plane[1] = 0.0;
   plane[2] = -1.0;
   plane[3] = value;
}

#define CONSTANT_PLANE(VALUE, PLANE)	\
do {					\
   PLANE[0] = 0.0F;			\
   PLANE[1] = 0.0F;			\
   PLANE[2] = -1.0F;			\
   PLANE[3] = VALUE;			\
} while (0)



/*
 * Solve plane equation for Z at (X,Y).
 */
static INLINE GLfloat
solve_plane(GLfloat x, GLfloat y, const GLfloat plane[4])
{
   ASSERT(plane[2] != 0.0F);
   return (plane[3] + plane[0] * x + plane[1] * y) / -plane[2];
}


#define SOLVE_PLANE(X, Y, PLANE) \
   ((PLANE[3] + PLANE[0] * (X) + PLANE[1] * (Y)) / -PLANE[2])


/*
 * Return 1 / solve_plane().
 */
static INLINE GLfloat
solve_plane_recip(GLfloat x, GLfloat y, const GLfloat plane[4])
{
   const GLfloat denom = plane[3] + plane[0] * x + plane[1] * y;
   if (denom == 0.0F)
      return 0.0F;
   else
      return -plane[2] / denom;
}


/*
 * Solve plane and return clamped GLchan value.
 */
static INLINE GLchan
solve_plane_chan(GLfloat x, GLfloat y, const GLfloat plane[4])
{
   GLfloat z = (plane[3] + plane[0] * x + plane[1] * y) / -plane[2] + 0.5F;
   if (z < 0.0F)
      return 0;
   else if (z > CHAN_MAXF)
      return (GLchan) CHAN_MAXF;
   return (GLchan) (GLint) z;
}



/*
 * Compute how much (area) of the given pixel is inside the triangle.
 * Vertices MUST be specified in counter-clockwise order.
 * Return:  coverage in [0, 1].
 */
static GLfloat
compute_coveragef(const GLfloat v0[3], const GLfloat v1[3],
                  const GLfloat v2[3], GLint winx, GLint winy)
{
   /* Given a position [0,3]x[0,3] return the sub-pixel sample position.
    * Contributed by Ray Tice.
    *
    * Jitter sample positions -
    * - average should be .5 in x & y for each column
    * - each of the 16 rows and columns should be used once
    * - the rectangle formed by the first four points
    *   should contain the other points
    * - the distrubition should be fairly even in any given direction
    *
    * The pattern drawn below isn't optimal, but it's better than a regular
    * grid.  In the drawing, the center of each subpixel is surrounded by
    * four dots.  The "x" marks the jittered position relative to the
    * subpixel center.
    */
#define POS(a, b) (0.5+a*4+b)/16
   static const GLfloat samples[16][2] = {
      /* start with the four corners */
      { POS(0, 2), POS(0, 0) },
      { POS(3, 3), POS(0, 2) },
      { POS(0, 0), POS(3, 1) },
      { POS(3, 1), POS(3, 3) },
      /* continue with interior samples */
      { POS(1, 1), POS(0, 1) },
      { POS(2, 0), POS(0, 3) },
      { POS(0, 3), POS(1, 3) },
      { POS(1, 2), POS(1, 0) },
      { POS(2, 3), POS(1, 2) },
      { POS(3, 2), POS(1, 1) },
      { POS(0, 1), POS(2, 2) },
      { POS(1, 0), POS(2, 1) },
      { POS(2, 1), POS(2, 3) },
      { POS(3, 0), POS(2, 0) },
      { POS(1, 3), POS(3, 0) },
      { POS(2, 2), POS(3, 2) }
   };

   const GLfloat x = (GLfloat) winx;
   const GLfloat y = (GLfloat) winy;
   const GLfloat dx0 = v1[0] - v0[0];
   const GLfloat dy0 = v1[1] - v0[1];
   const GLfloat dx1 = v2[0] - v1[0];
   const GLfloat dy1 = v2[1] - v1[1];
   const GLfloat dx2 = v0[0] - v2[0];
   const GLfloat dy2 = v0[1] - v2[1];
   GLint stop = 4, i;
   GLfloat insideCount = 16.0F;

#ifdef DEBUG
   {
      const GLfloat area = dx0 * dy1 - dx1 * dy0;
      ASSERT(area >= 0.0);
   }
#endif

   for (i = 0; i < stop; i++) {
      const GLfloat sx = x + samples[i][0];
      const GLfloat sy = y + samples[i][1];
      const GLfloat fx0 = sx - v0[0];
      const GLfloat fy0 = sy - v0[1];
      const GLfloat fx1 = sx - v1[0];
      const GLfloat fy1 = sy - v1[1];
      const GLfloat fx2 = sx - v2[0];
      const GLfloat fy2 = sy - v2[1];
      /* cross product determines if sample is inside or outside each edge */
      GLfloat cross0 = (dx0 * fy0 - dy0 * fx0);
      GLfloat cross1 = (dx1 * fy1 - dy1 * fx1);
      GLfloat cross2 = (dx2 * fy2 - dy2 * fx2);
      /* Check if the sample is exactly on an edge.  If so, let cross be a
       * positive or negative value depending on the direction of the edge.
       */
      if (cross0 == 0.0F)
         cross0 = dx0 + dy0;
      if (cross1 == 0.0F)
         cross1 = dx1 + dy1;
      if (cross2 == 0.0F)
         cross2 = dx2 + dy2;
      if (cross0 < 0.0F || cross1 < 0.0F || cross2 < 0.0F) {
         /* point is outside triangle */
         insideCount -= 1.0F;
         stop = 16;
      }
   }
   if (stop == 4)
      return 1.0F;
   else
      return insideCount * (1.0F / 16.0F);
}



/*
 * Compute how much (area) of the given pixel is inside the triangle.
 * Vertices MUST be specified in counter-clockwise order.
 * Return:  coverage in [0, 15].
 */
static GLint
compute_coveragei(const GLfloat v0[3], const GLfloat v1[3],
                  const GLfloat v2[3], GLint winx, GLint winy)
{
   /* NOTE: 15 samples instead of 16. */
   static const GLfloat samples[15][2] = {
      /* start with the four corners */
      { POS(0, 2), POS(0, 0) },
      { POS(3, 3), POS(0, 2) },
      { POS(0, 0), POS(3, 1) },
      { POS(3, 1), POS(3, 3) },
      /* continue with interior samples */
      { POS(1, 1), POS(0, 1) },
      { POS(2, 0), POS(0, 3) },
      { POS(0, 3), POS(1, 3) },
      { POS(1, 2), POS(1, 0) },
      { POS(2, 3), POS(1, 2) },
      { POS(3, 2), POS(1, 1) },
      { POS(0, 1), POS(2, 2) },
      { POS(1, 0), POS(2, 1) },
      { POS(2, 1), POS(2, 3) },
      { POS(3, 0), POS(2, 0) },
      { POS(1, 3), POS(3, 0) }
   };
   const GLfloat x = (GLfloat) winx;
   const GLfloat y = (GLfloat) winy;
   const GLfloat dx0 = v1[0] - v0[0];
   const GLfloat dy0 = v1[1] - v0[1];
   const GLfloat dx1 = v2[0] - v1[0];
   const GLfloat dy1 = v2[1] - v1[1];
   const GLfloat dx2 = v0[0] - v2[0];
   const GLfloat dy2 = v0[1] - v2[1];
   GLint stop = 4, i;
   GLint insideCount = 15;

#ifdef DEBUG
   {
      const GLfloat area = dx0 * dy1 - dx1 * dy0;
      ASSERT(area >= 0.0);
   }
#endif

   for (i = 0; i < stop; i++) {
      const GLfloat sx = x + samples[i][0];
      const GLfloat sy = y + samples[i][1];
      const GLfloat fx0 = sx - v0[0];
      const GLfloat fy0 = sy - v0[1];
      const GLfloat fx1 = sx - v1[0];
      const GLfloat fy1 = sy - v1[1];
      const GLfloat fx2 = sx - v2[0];
      const GLfloat fy2 = sy - v2[1];
      /* cross product determines if sample is inside or outside each edge */
      GLfloat cross0 = (dx0 * fy0 - dy0 * fx0);
      GLfloat cross1 = (dx1 * fy1 - dy1 * fx1);
      GLfloat cross2 = (dx2 * fy2 - dy2 * fx2);
      /* Check if the sample is exactly on an edge.  If so, let cross be a
       * positive or negative value depending on the direction of the edge.
       */
      if (cross0 == 0.0F)
         cross0 = dx0 + dy0;
      if (cross1 == 0.0F)
         cross1 = dx1 + dy1;
      if (cross2 == 0.0F)
         cross2 = dx2 + dy2;
      if (cross0 < 0.0F || cross1 < 0.0F || cross2 < 0.0F) {
         /* point is outside triangle */
         insideCount--;
         stop = 15;
      }
   }
   if (stop == 4)
      return 15;
   else
      return insideCount;
}



static void
rgba_aa_tri(GLcontext *ctx,
	    const SWvertex *v0,
	    const SWvertex *v1,
	    const SWvertex *v2)
{
#define DO_Z
#define DO_FOG
#define DO_RGBA
#include "s_aatritemp.h"
}


static void
index_aa_tri(GLcontext *ctx,
	     const SWvertex *v0,
	     const SWvertex *v1,
	     const SWvertex *v2)
{
#define DO_Z
#define DO_FOG
#define DO_INDEX
#include "s_aatritemp.h"
}


/*
 * Compute mipmap level of detail.
 * XXX we should really include the R coordinate in this computation
 * in order to do 3-D texture mipmapping.
 */
static INLINE GLfloat
compute_lambda(const GLfloat sPlane[4], const GLfloat tPlane[4],
               const GLfloat qPlane[4], GLfloat cx, GLfloat cy,
               GLfloat invQ, GLfloat texWidth, GLfloat texHeight)
{
   const GLfloat s = solve_plane(cx, cy, sPlane);
   const GLfloat t = solve_plane(cx, cy, tPlane);
   const GLfloat invQ_x1 = solve_plane_recip(cx+1.0F, cy, qPlane);
   const GLfloat invQ_y1 = solve_plane_recip(cx, cy+1.0F, qPlane);
   const GLfloat s_x1 = s - sPlane[0] / sPlane[2];
   const GLfloat s_y1 = s - sPlane[1] / sPlane[2];
   const GLfloat t_x1 = t - tPlane[0] / tPlane[2];
   const GLfloat t_y1 = t - tPlane[1] / tPlane[2];
   GLfloat dsdx = s_x1 * invQ_x1 - s * invQ;
   GLfloat dsdy = s_y1 * invQ_y1 - s * invQ;
   GLfloat dtdx = t_x1 * invQ_x1 - t * invQ;
   GLfloat dtdy = t_y1 * invQ_y1 - t * invQ;
   GLfloat maxU, maxV, rho, lambda;
   dsdx = FABSF(dsdx);
   dsdy = FABSF(dsdy);
   dtdx = FABSF(dtdx);
   dtdy = FABSF(dtdy);
   maxU = MAX2(dsdx, dsdy) * texWidth;
   maxV = MAX2(dtdx, dtdy) * texHeight;
   rho = MAX2(maxU, maxV);
   lambda = LOG2(rho);
   return lambda;
}


static void
tex_aa_tri(GLcontext *ctx,
	   const SWvertex *v0,
	   const SWvertex *v1,
	   const SWvertex *v2)
{
#define DO_Z
#define DO_FOG
#define DO_RGBA
#define DO_TEX
#include "s_aatritemp.h"
}


static void
spec_tex_aa_tri(GLcontext *ctx,
		const SWvertex *v0,
		const SWvertex *v1,
		const SWvertex *v2)
{
#define DO_Z
#define DO_FOG
#define DO_RGBA
#define DO_TEX
#define DO_SPEC
#include "s_aatritemp.h"
}


static void
multitex_aa_tri(GLcontext *ctx,
		const SWvertex *v0,
		const SWvertex *v1,
		const SWvertex *v2)
{
#define DO_Z
#define DO_FOG
#define DO_RGBA
#define DO_MULTITEX
#include "s_aatritemp.h"
}

static void
spec_multitex_aa_tri(GLcontext *ctx,
		     const SWvertex *v0,
		     const SWvertex *v1,
		     const SWvertex *v2)
{
#define DO_Z
#define DO_FOG
#define DO_RGBA
#define DO_MULTITEX
#define DO_SPEC
#include "s_aatritemp.h"
}


/*
 * Examine GL state and set swrast->Triangle to an
 * appropriate antialiased triangle rasterizer function.
 */
void
_mesa_set_aa_triangle_function(GLcontext *ctx)
{
   ASSERT(ctx->Polygon.SmoothFlag);

   if (ctx->Texture._EnabledUnits != 0) {
      if (ctx->_TriangleCaps & DD_SEPARATE_SPECULAR) {
         if (ctx->Texture._EnabledUnits > 1) {
            SWRAST_CONTEXT(ctx)->Triangle = spec_multitex_aa_tri;
         }
         else {
            SWRAST_CONTEXT(ctx)->Triangle = spec_tex_aa_tri;
         }
      }
      else {
         if (ctx->Texture._EnabledUnits > 1) {
            SWRAST_CONTEXT(ctx)->Triangle = multitex_aa_tri;
         }
         else {
            SWRAST_CONTEXT(ctx)->Triangle = tex_aa_tri;
         }
      }
   }
   else if (ctx->Visual.rgbMode) {
      SWRAST_CONTEXT(ctx)->Triangle = rgba_aa_tri;
   }
   else {
      SWRAST_CONTEXT(ctx)->Triangle = index_aa_tri;
   }

   ASSERT(SWRAST_CONTEXT(ctx)->Triangle);
}
