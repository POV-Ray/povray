/* $Id: s_pointtemp.h,v 1.21 2002/11/09 21:26:41 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  5.0
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
 * Point rendering template code.
 *
 * Set FLAGS = bitwise-OR of the following tokens:
 *
 *   RGBA = do rgba instead of color index
 *   SMOOTH = do antialiasing
 *   TEXTURE = do texture coords
 *   SPECULAR = do separate specular color
 *   LARGE = do points with diameter > 1 pixel
 *   ATTENUATE = compute point size attenuation
 *   SPRITE = GL_NV_point_sprite
 *
 * Notes: LARGE and ATTENUATE are exclusive of each other.
 *        TEXTURE requires RGBA
 *        SPECULAR requires TEXTURE
 */


/*
 * NOTES on antialiased point rasterization:
 *
 * Let d = distance of fragment center from vertex.
 * if d < rmin2 then
 *    fragment has 100% coverage
 * else if d > rmax2 then
 *    fragment has 0% coverage
 * else
 *    fragment has % coverage = (d - rmin2) / (rmax2 - rmin2)
 */



static void
NAME ( GLcontext *ctx, const SWvertex *vert )
{
#if FLAGS & (ATTENUATE | LARGE | SMOOTH | SPRITE)
   GLfloat size;
#endif
#if FLAGS & ATTENUATE
   GLfloat alphaAtten;
#endif
#if FLAGS & RGBA
   const GLchan red   = vert->color[0];
   const GLchan green = vert->color[1];
   const GLchan blue  = vert->color[2];
   const GLchan alpha = vert->color[3];
#endif
#if FLAGS & SPECULAR
   const GLchan specRed   = vert->specular[0];
   const GLchan specGreen = vert->specular[1];
   const GLchan specBlue  = vert->specular[2];
#endif
#if FLAGS & INDEX
   const GLuint colorIndex = vert->index;
#endif
#if FLAGS & TEXTURE
   GLfloat texcoord[MAX_TEXTURE_UNITS][4];
   GLuint u;
#endif
   SWcontext *swrast = SWRAST_CONTEXT(ctx);
   struct sw_span *span = &(swrast->PointSpan);

   /* Cull primitives with malformed coordinates.
    */
   {
      float tmp = vert->win[0] + vert->win[1];
      if (IS_INF_OR_NAN(tmp))
	 return;
   }

   /*
    * Span init
    */
   span->interpMask = SPAN_FOG;
   span->arrayMask = SPAN_XY | SPAN_Z;
   span->fog = vert->fog;
   span->fogStep = 0.0;
#if FLAGS & RGBA
   span->arrayMask |= SPAN_RGBA;
#endif
#if FLAGS & SPECULAR
   span->arrayMask |= SPAN_SPEC;
#endif
#if FLAGS & INDEX
   span->arrayMask |= SPAN_INDEX;
#endif
#if FLAGS & TEXTURE
   span->arrayMask |= SPAN_TEXTURE;
   for (u = 0; u < ctx->Const.MaxTextureUnits; u++) {
      if (ctx->Texture.Unit[u]._ReallyEnabled) {
         const GLfloat q = vert->texcoord[u][3];
         const GLfloat invQ = (q == 0.0F || q == 1.0F) ? 1.0F : (1.0F / q);
         texcoord[u][0] = vert->texcoord[u][0] * invQ;
         texcoord[u][1] = vert->texcoord[u][1] * invQ;
         texcoord[u][2] = vert->texcoord[u][2] * invQ;
         texcoord[u][3] = q;
      }
   }
#endif
#if FLAGS & SMOOTH
   span->arrayMask |= SPAN_COVERAGE;
#endif
#if FLAGS & SPRITE
   span->arrayMask |= SPAN_TEXTURE;
#endif

#if FLAGS & ATTENUATE
   if (vert->pointSize >= ctx->Point.Threshold) {
      size = MIN2(vert->pointSize, ctx->Point.MaxSize);
      alphaAtten = 1.0F;
   }
   else {
      GLfloat dsize = vert->pointSize / ctx->Point.Threshold;
      size = MAX2(ctx->Point.Threshold, ctx->Point.MinSize);
      alphaAtten = dsize * dsize;
   }
#elif FLAGS & (LARGE | SMOOTH | SPRITE)
   size = ctx->Point._Size;
#endif

#if FLAGS & (ATTENUATE | LARGE | SMOOTH | SPRITE)
   /*
    * Multi-pixel points
    */
   {{
      GLint x, y;
      const GLfloat radius = 0.5F * size;
      const GLint z = (GLint) (vert->win[2] + 0.5F);
      GLuint count;
#if FLAGS & SMOOTH
      const GLfloat rmin = radius - 0.7071F;  /* 0.7071 = sqrt(2)/2 */
      const GLfloat rmax = radius + 0.7071F;
      const GLfloat rmin2 = MAX2(0.0F, rmin * rmin);
      const GLfloat rmax2 = rmax * rmax;
      const GLfloat cscale = 1.0F / (rmax2 - rmin2);
      const GLint xmin = (GLint) (vert->win[0] - radius);
      const GLint xmax = (GLint) (vert->win[0] + radius);
      const GLint ymin = (GLint) (vert->win[1] - radius);
      const GLint ymax = (GLint) (vert->win[1] + radius);
#else
      /* non-smooth */
      GLint xmin, xmax, ymin, ymax;
      GLint iSize = (GLint) (size + 0.5F);
      GLint iRadius;
      iSize = MAX2(1, iSize);
      iRadius = iSize / 2;
      if (iSize & 1) {
         /* odd size */
         xmin = (GLint) (vert->win[0] - iRadius);
         xmax = (GLint) (vert->win[0] + iRadius);
         ymin = (GLint) (vert->win[1] - iRadius);
         ymax = (GLint) (vert->win[1] + iRadius);
      }
      else {
         /* even size */
         xmin = (GLint) vert->win[0] - iRadius + 1;
         xmax = xmin + iSize - 1;
         ymin = (GLint) vert->win[1] - iRadius + 1;
         ymax = ymin + iSize - 1;
      }
#endif /*SMOOTH*/

      /* check if we need to flush */
      if (span->end + (xmax-xmin+1) * (ymax-ymin+1) >= MAX_WIDTH ||
          (swrast->_RasterMask & (BLEND_BIT | LOGIC_OP_BIT | MASKING_BIT))) {
#if FLAGS & (TEXTURE | SPRITE)
         if (ctx->Texture._EnabledUnits)
            _mesa_write_texture_span(ctx, span);
         else
            _mesa_write_rgba_span(ctx, span);
#elif FLAGS & RGBA
         _mesa_write_rgba_span(ctx, span);
#else
         _mesa_write_index_span(ctx, span);
#endif
         span->end = 0;
      }

      /*
       * OK, generate fragments
       */
      count = span->end;
      (void) radius;
      for (y = ymin; y <= ymax; y++) {
         for (x = xmin; x <= xmax; x++) {
#if FLAGS & (SPRITE | TEXTURE)
            GLuint u;
#endif

#if FLAGS & RGBA
            span->array->rgba[count][RCOMP] = red;
            span->array->rgba[count][GCOMP] = green;
            span->array->rgba[count][BCOMP] = blue;
            span->array->rgba[count][ACOMP] = alpha;
#endif
#if FLAGS & SPECULAR
            span->array->spec[count][RCOMP] = specRed;
            span->array->spec[count][GCOMP] = specGreen;
            span->array->spec[count][BCOMP] = specBlue;
#endif
#if FLAGS & INDEX
            span->array->index[count] = colorIndex;
#endif
#if FLAGS & TEXTURE
            for (u = 0; u < ctx->Const.MaxTextureUnits; u++) {
               if (ctx->Texture.Unit[u]._ReallyEnabled) {
                  COPY_4V(span->array->texcoords[u][count], texcoord[u]);
               }
            }
#endif

#if FLAGS & SMOOTH
            /* compute coverage */
            {
               const GLfloat dx = x - vert->win[0] + 0.5F;
               const GLfloat dy = y - vert->win[1] + 0.5F;
               const GLfloat dist2 = dx * dx + dy * dy;
               if (dist2 < rmax2) {
                  if (dist2 >= rmin2) {
                     /* compute partial coverage */
                     span->array->coverage[count] = 1.0F - (dist2 - rmin2) * cscale;
#if FLAGS & INDEX
                     /* coverage in [0,15] */
                     span->array->coverage[count] *= 15.0;
#endif
                  }
                  else {
                     /* full coverage */
                     span->array->coverage[count] = 1.0F;
                  }

                  span->array->x[count] = x;
                  span->array->y[count] = y;
                  span->array->z[count] = z;

#if (FLAGS & ATTENUATE) && (FLAGS & RGBA)
                  span->array->rgba[count][ACOMP] = (GLchan) (alpha * alphaAtten);
#elif FLAGS & RGBA
                  span->array->rgba[count][ACOMP] = alpha;
#endif /*ATTENUATE*/
                  count++;
               } /*if*/
            }

#else /*SMOOTH*/

            /* not smooth (square points) */
            span->array->x[count] = x;
            span->array->y[count] = y;
            span->array->z[count] = z;

#if FLAGS & SPRITE
            for (u = 0; u < ctx->Const.MaxTextureUnits; u++) {
               if (ctx->Texture.Unit[u]._ReallyEnabled) {
                  if (ctx->Point.CoordReplace[u]) {
                     GLfloat s = 0.5F + (x + 0.5F - vert->win[0]) / size;
                     GLfloat t = 0.5F - (y + 0.5F - vert->win[1]) / size;
                     span->array->texcoords[u][count][0] = s;
                     span->array->texcoords[u][count][1] = t;
                     span->array->texcoords[u][count][3] = 1.0F;
                     if (ctx->Point.SpriteRMode == GL_ZERO)
                        span->array->texcoords[u][count][2] = 0.0F;
                     else if (ctx->Point.SpriteRMode == GL_S)
                        span->array->texcoords[u][count][2] = vert->texcoord[u][0];
                     else /* GL_R */
                        span->array->texcoords[u][count][2] = vert->texcoord[u][2];
                  }
                  else {
                     COPY_4V(span->array->texcoords[u][count], vert->texcoord[u]);
                  }
               }
            }
#endif /*SPRITE*/

            count++;  /* square point */

#endif /*SMOOTH*/

	 } /*for x*/
      } /*for y*/
      span->end = count;
   }}

#else /* LARGE | ATTENUATE | SMOOTH | SPRITE */

   /*
    * Single-pixel points
    */
   {{
      GLuint count;

      /* check if we need to flush */
      if (span->end >= MAX_WIDTH ||
          (swrast->_RasterMask & (BLEND_BIT | LOGIC_OP_BIT | MASKING_BIT))) {
#if FLAGS & (TEXTURE | SPRITE)
         if (ctx->Texture._EnabledUnits)
            _mesa_write_texture_span(ctx, span);
         else
            _mesa_write_rgba_span(ctx, span);
#elif FLAGS & RGBA
         _mesa_write_rgba_span(ctx, span);
#else
         _mesa_write_index_span(ctx, span);
#endif
         span->end = 0;
      }

      count = span->end;

#if FLAGS & RGBA
      span->array->rgba[count][RCOMP] = red;
      span->array->rgba[count][GCOMP] = green;
      span->array->rgba[count][BCOMP] = blue;
      span->array->rgba[count][ACOMP] = alpha;
#endif
#if FLAGS & SPECULAR
      span->array->spec[count][RCOMP] = specRed;
      span->array->spec[count][GCOMP] = specGreen;
      span->array->spec[count][BCOMP] = specBlue;
#endif
#if FLAGS & INDEX
      span->array->index[count] = colorIndex;
#endif
#if FLAGS & TEXTURE
      for (u = 0; u < ctx->Const.MaxTextureUnits; u++) {
         if (ctx->Texture.Unit[u]._ReallyEnabled) {
            COPY_4V(span->array->texcoords[u][count], texcoord[u]);
         }
      }
#endif

      span->array->x[count] = (GLint) vert->win[0];
      span->array->y[count] = (GLint) vert->win[1];
      span->array->z[count] = (GLint) (vert->win[2] + 0.5F);
      span->end = count + 1;
   }}

#endif /* LARGE || ATTENUATE || SMOOTH */

   ASSERT(span->end <= MAX_WIDTH);
}


#undef FLAGS
#undef NAME
