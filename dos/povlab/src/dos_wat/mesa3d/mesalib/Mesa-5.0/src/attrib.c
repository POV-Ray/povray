/* $Id: attrib.c,v 1.75 2002/10/24 23:57:19 brianp Exp $ */

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

#include "glheader.h"
#include "imports.h"
#include "accum.h"
#include "attrib.h"
#include "blend.h"
#include "buffers.h"
#include "colormac.h"
#include "context.h"
#include "depth.h"
#include "enable.h"
#include "enums.h"
#include "fog.h"
#include "hint.h"
#include "light.h"
#include "lines.h"
#include "matrix.h"
#include "points.h"
#include "polygon.h"
#include "simple_list.h"
#include "stencil.h"
#include "texobj.h"
#include "texstate.h"
#include "mtypes.h"
#include "math/m_xform.h"


/*
 * Allocate a new attribute state node.  These nodes have a
 * "kind" value and a pointer to a struct of state data.
 */
static struct gl_attrib_node *
new_attrib_node( GLbitfield kind )
{
   struct gl_attrib_node *an = MALLOC_STRUCT(gl_attrib_node);
   if (an) {
      an->kind = kind;
   }
   return an;
}


void
_mesa_PushAttrib(GLbitfield mask)
{
   struct gl_attrib_node *newnode;
   struct gl_attrib_node *head;

   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (MESA_VERBOSE & VERBOSE_API)
      _mesa_debug(ctx, "glPushAttrib %x\n", (int) mask);

   if (ctx->AttribStackDepth >= MAX_ATTRIB_STACK_DEPTH) {
      _mesa_error( ctx, GL_STACK_OVERFLOW, "glPushAttrib" );
      return;
   }

   /* Build linked list of attribute nodes which save all attribute */
   /* groups specified by the mask. */
   head = NULL;

   if (mask & GL_ACCUM_BUFFER_BIT) {
      struct gl_accum_attrib *attr;
      attr = MALLOC_STRUCT( gl_accum_attrib );
      MEMCPY( attr, &ctx->Accum, sizeof(struct gl_accum_attrib) );
      newnode = new_attrib_node( GL_ACCUM_BUFFER_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_COLOR_BUFFER_BIT) {
      struct gl_colorbuffer_attrib *attr;
      attr = MALLOC_STRUCT( gl_colorbuffer_attrib );
      MEMCPY( attr, &ctx->Color, sizeof(struct gl_colorbuffer_attrib) );
      newnode = new_attrib_node( GL_COLOR_BUFFER_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_CURRENT_BIT) {
      struct gl_current_attrib *attr;
      FLUSH_CURRENT( ctx, 0 );
      attr = MALLOC_STRUCT( gl_current_attrib );
      MEMCPY( attr, &ctx->Current, sizeof(struct gl_current_attrib) );
      newnode = new_attrib_node( GL_CURRENT_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_DEPTH_BUFFER_BIT) {
      struct gl_depthbuffer_attrib *attr;
      attr = MALLOC_STRUCT( gl_depthbuffer_attrib );
      MEMCPY( attr, &ctx->Depth, sizeof(struct gl_depthbuffer_attrib) );
      newnode = new_attrib_node( GL_DEPTH_BUFFER_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_ENABLE_BIT) {
      struct gl_enable_attrib *attr;
      GLuint i;
      attr = MALLOC_STRUCT( gl_enable_attrib );
      /* Copy enable flags from all other attributes into the enable struct. */
      attr->AlphaTest = ctx->Color.AlphaEnabled;
      attr->AutoNormal = ctx->Eval.AutoNormal;
      attr->Blend = ctx->Color.BlendEnabled;
      attr->ClipPlanes = ctx->Transform.ClipPlanesEnabled;
      attr->ColorMaterial = ctx->Light.ColorMaterialEnabled;
      attr->Convolution1D = ctx->Pixel.Convolution1DEnabled;
      attr->Convolution2D = ctx->Pixel.Convolution2DEnabled;
      attr->Separable2D = ctx->Pixel.Separable2DEnabled;
      attr->CullFace = ctx->Polygon.CullFlag;
      attr->DepthTest = ctx->Depth.Test;
      attr->Dither = ctx->Color.DitherFlag;
      attr->Fog = ctx->Fog.Enabled;
      for (i=0;i<MAX_LIGHTS;i++) {
         attr->Light[i] = ctx->Light.Light[i].Enabled;
      }
      attr->Lighting = ctx->Light.Enabled;
      attr->LineSmooth = ctx->Line.SmoothFlag;
      attr->LineStipple = ctx->Line.StippleFlag;
      attr->Histogram = ctx->Pixel.HistogramEnabled;
      attr->MinMax = ctx->Pixel.MinMaxEnabled;
      attr->IndexLogicOp = ctx->Color.IndexLogicOpEnabled;
      attr->ColorLogicOp = ctx->Color.ColorLogicOpEnabled;
      attr->Map1Color4 = ctx->Eval.Map1Color4;
      attr->Map1Index = ctx->Eval.Map1Index;
      attr->Map1Normal = ctx->Eval.Map1Normal;
      attr->Map1TextureCoord1 = ctx->Eval.Map1TextureCoord1;
      attr->Map1TextureCoord2 = ctx->Eval.Map1TextureCoord2;
      attr->Map1TextureCoord3 = ctx->Eval.Map1TextureCoord3;
      attr->Map1TextureCoord4 = ctx->Eval.Map1TextureCoord4;
      attr->Map1Vertex3 = ctx->Eval.Map1Vertex3;
      attr->Map1Vertex4 = ctx->Eval.Map1Vertex4;
      MEMCPY(attr->Map1Attrib, ctx->Eval.Map1Attrib, sizeof(ctx->Eval.Map1Attrib));
      attr->Map2Color4 = ctx->Eval.Map2Color4;
      attr->Map2Index = ctx->Eval.Map2Index;
      attr->Map2Normal = ctx->Eval.Map2Normal;
      attr->Map2TextureCoord1 = ctx->Eval.Map2TextureCoord1;
      attr->Map2TextureCoord2 = ctx->Eval.Map2TextureCoord2;
      attr->Map2TextureCoord3 = ctx->Eval.Map2TextureCoord3;
      attr->Map2TextureCoord4 = ctx->Eval.Map2TextureCoord4;
      attr->Map2Vertex3 = ctx->Eval.Map2Vertex3;
      attr->Map2Vertex4 = ctx->Eval.Map2Vertex4;
      MEMCPY(attr->Map2Attrib, ctx->Eval.Map2Attrib, sizeof(ctx->Eval.Map2Attrib));
      attr->Normalize = ctx->Transform.Normalize;
      attr->RasterPositionUnclipped = ctx->Transform.RasterPositionUnclipped;
      attr->PixelTexture = ctx->Pixel.PixelTextureEnabled;
      attr->PointSmooth = ctx->Point.SmoothFlag;
      attr->PointSprite = ctx->Point.PointSprite;
      attr->PolygonOffsetPoint = ctx->Polygon.OffsetPoint;
      attr->PolygonOffsetLine = ctx->Polygon.OffsetLine;
      attr->PolygonOffsetFill = ctx->Polygon.OffsetFill;
      attr->PolygonSmooth = ctx->Polygon.SmoothFlag;
      attr->PolygonStipple = ctx->Polygon.StippleFlag;
      attr->RescaleNormals = ctx->Transform.RescaleNormals;
      attr->Scissor = ctx->Scissor.Enabled;
      attr->Stencil = ctx->Stencil.Enabled;
      attr->MultisampleEnabled = ctx->Multisample.Enabled;
      attr->SampleAlphaToCoverage = ctx->Multisample.SampleAlphaToCoverage;
      attr->SampleAlphaToOne = ctx->Multisample.SampleAlphaToOne;
      attr->SampleCoverage = ctx->Multisample.SampleCoverage;
      attr->SampleCoverageInvert = ctx->Multisample.SampleCoverageInvert;
      for (i=0; i<MAX_TEXTURE_UNITS; i++) {
         attr->Texture[i] = ctx->Texture.Unit[i].Enabled;
         attr->TexGen[i] = ctx->Texture.Unit[i].TexGenEnabled;
      }
      /* GL_NV_vertex_program */
      attr->VertexProgram = ctx->VertexProgram.Enabled;
      attr->VertexProgramPointSize = ctx->VertexProgram.PointSizeEnabled;
      attr->VertexProgramTwoSide = ctx->VertexProgram.TwoSideEnabled;
      newnode = new_attrib_node( GL_ENABLE_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_EVAL_BIT) {
      struct gl_eval_attrib *attr;
      attr = MALLOC_STRUCT( gl_eval_attrib );
      MEMCPY( attr, &ctx->Eval, sizeof(struct gl_eval_attrib) );
      newnode = new_attrib_node( GL_EVAL_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_FOG_BIT) {
      struct gl_fog_attrib *attr;
      attr = MALLOC_STRUCT( gl_fog_attrib );
      MEMCPY( attr, &ctx->Fog, sizeof(struct gl_fog_attrib) );
      newnode = new_attrib_node( GL_FOG_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_HINT_BIT) {
      struct gl_hint_attrib *attr;
      attr = MALLOC_STRUCT( gl_hint_attrib );
      MEMCPY( attr, &ctx->Hint, sizeof(struct gl_hint_attrib) );
      newnode = new_attrib_node( GL_HINT_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_LIGHTING_BIT) {
      struct gl_light_attrib *attr;
      FLUSH_CURRENT(ctx, 0);	/* flush material changes */
      attr = MALLOC_STRUCT( gl_light_attrib );
      MEMCPY( attr, &ctx->Light, sizeof(struct gl_light_attrib) );
      newnode = new_attrib_node( GL_LIGHTING_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_LINE_BIT) {
      struct gl_line_attrib *attr;
      attr = MALLOC_STRUCT( gl_line_attrib );
      MEMCPY( attr, &ctx->Line, sizeof(struct gl_line_attrib) );
      newnode = new_attrib_node( GL_LINE_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_LIST_BIT) {
      struct gl_list_attrib *attr;
      attr = MALLOC_STRUCT( gl_list_attrib );
      MEMCPY( attr, &ctx->List, sizeof(struct gl_list_attrib) );
      newnode = new_attrib_node( GL_LIST_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_PIXEL_MODE_BIT) {
      struct gl_pixel_attrib *attr;
      attr = MALLOC_STRUCT( gl_pixel_attrib );
      MEMCPY( attr, &ctx->Pixel, sizeof(struct gl_pixel_attrib) );
      newnode = new_attrib_node( GL_PIXEL_MODE_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_POINT_BIT) {
      struct gl_point_attrib *attr;
      attr = MALLOC_STRUCT( gl_point_attrib );
      MEMCPY( attr, &ctx->Point, sizeof(struct gl_point_attrib) );
      newnode = new_attrib_node( GL_POINT_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_POLYGON_BIT) {
      struct gl_polygon_attrib *attr;
      attr = MALLOC_STRUCT( gl_polygon_attrib );
      MEMCPY( attr, &ctx->Polygon, sizeof(struct gl_polygon_attrib) );
      newnode = new_attrib_node( GL_POLYGON_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_POLYGON_STIPPLE_BIT) {
      GLuint *stipple;
      stipple = (GLuint *) MALLOC( 32*sizeof(GLuint) );
      MEMCPY( stipple, ctx->PolygonStipple, 32*sizeof(GLuint) );
      newnode = new_attrib_node( GL_POLYGON_STIPPLE_BIT );
      newnode->data = stipple;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_SCISSOR_BIT) {
      struct gl_scissor_attrib *attr;
      attr = MALLOC_STRUCT( gl_scissor_attrib );
      MEMCPY( attr, &ctx->Scissor, sizeof(struct gl_scissor_attrib) );
      newnode = new_attrib_node( GL_SCISSOR_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_STENCIL_BUFFER_BIT) {
      struct gl_stencil_attrib *attr;
      attr = MALLOC_STRUCT( gl_stencil_attrib );
      MEMCPY( attr, &ctx->Stencil, sizeof(struct gl_stencil_attrib) );
      newnode = new_attrib_node( GL_STENCIL_BUFFER_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_TEXTURE_BIT) {
      struct gl_texture_attrib *attr;
      GLuint u;
      /* Bump the texture object reference counts so that they don't
       * inadvertantly get deleted.
       */
      for (u = 0; u < ctx->Const.MaxTextureUnits; u++) {
	 ctx->Texture.Unit[u].Current1D->RefCount++;
	 ctx->Texture.Unit[u].Current2D->RefCount++;
	 ctx->Texture.Unit[u].Current3D->RefCount++;
	 ctx->Texture.Unit[u].CurrentCubeMap->RefCount++;
	 ctx->Texture.Unit[u].CurrentRect->RefCount++;
      }
      attr = MALLOC_STRUCT( gl_texture_attrib );
      MEMCPY( attr, &ctx->Texture, sizeof(struct gl_texture_attrib) );
      /* copy state of the currently bound texture objects */
      for (u = 0; u < ctx->Const.MaxTextureUnits; u++) {
         _mesa_copy_texture_object(&attr->Unit[u].Saved1D,
                                   attr->Unit[u].Current1D);
         _mesa_copy_texture_object(&attr->Unit[u].Saved2D,
                                   attr->Unit[u].Current2D);
         _mesa_copy_texture_object(&attr->Unit[u].Saved3D,
                                   attr->Unit[u].Current3D);
         _mesa_copy_texture_object(&attr->Unit[u].SavedCubeMap,
                                   attr->Unit[u].CurrentCubeMap);
         _mesa_copy_texture_object(&attr->Unit[u].SavedRect,
                                   attr->Unit[u].CurrentRect);
      }
      newnode = new_attrib_node( GL_TEXTURE_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_TRANSFORM_BIT) {
      struct gl_transform_attrib *attr;
      attr = MALLOC_STRUCT( gl_transform_attrib );
      MEMCPY( attr, &ctx->Transform, sizeof(struct gl_transform_attrib) );
      newnode = new_attrib_node( GL_TRANSFORM_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   if (mask & GL_VIEWPORT_BIT) {
      struct gl_viewport_attrib *attr;
      attr = MALLOC_STRUCT( gl_viewport_attrib );
      MEMCPY( attr, &ctx->Viewport, sizeof(struct gl_viewport_attrib) );
      newnode = new_attrib_node( GL_VIEWPORT_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   /* GL_ARB_multisample */
   if (mask & GL_MULTISAMPLE_BIT_ARB) {
      struct gl_multisample_attrib *attr;
      attr = MALLOC_STRUCT( gl_multisample_attrib );
      MEMCPY( attr, &ctx->Multisample, sizeof(struct gl_multisample_attrib) );
      newnode = new_attrib_node( GL_MULTISAMPLE_BIT_ARB );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   ctx->AttribStack[ctx->AttribStackDepth] = head;
   ctx->AttribStackDepth++;
}



static void
pop_enable_group(GLcontext *ctx, const struct gl_enable_attrib *enable)
{
   GLuint i;

#define TEST_AND_UPDATE(VALUE, NEWVALUE, ENUM)		\
	if ((VALUE) != (NEWVALUE)) {			\
	   _mesa_set_enable( ctx, ENUM, (NEWVALUE) );	\
	}

   TEST_AND_UPDATE(ctx->Color.AlphaEnabled, enable->AlphaTest, GL_ALPHA_TEST);
   TEST_AND_UPDATE(ctx->Color.BlendEnabled, enable->Blend, GL_BLEND);

   for (i=0;i<MAX_CLIP_PLANES;i++) {
      const GLuint mask = 1 << i;
      if ((ctx->Transform.ClipPlanesEnabled & mask) != (enable->ClipPlanes & mask))
	  _mesa_set_enable(ctx, (GLenum) (GL_CLIP_PLANE0 + i),
			   (GLboolean) ((enable->ClipPlanes & mask) ? GL_TRUE : GL_FALSE));
   }

   TEST_AND_UPDATE(ctx->Light.ColorMaterialEnabled, enable->ColorMaterial,
                   GL_COLOR_MATERIAL);
   TEST_AND_UPDATE(ctx->Polygon.CullFlag, enable->CullFace, GL_CULL_FACE);
   TEST_AND_UPDATE(ctx->Depth.Test, enable->DepthTest, GL_DEPTH_TEST);
   TEST_AND_UPDATE(ctx->Color.DitherFlag, enable->Dither, GL_DITHER);
   TEST_AND_UPDATE(ctx->Pixel.Convolution1DEnabled, enable->Convolution1D,
                   GL_CONVOLUTION_1D);
   TEST_AND_UPDATE(ctx->Pixel.Convolution2DEnabled, enable->Convolution2D,
                   GL_CONVOLUTION_2D);
   TEST_AND_UPDATE(ctx->Pixel.Separable2DEnabled, enable->Separable2D,
                   GL_SEPARABLE_2D);
   TEST_AND_UPDATE(ctx->Fog.Enabled, enable->Fog, GL_FOG);
   TEST_AND_UPDATE(ctx->Light.Enabled, enable->Lighting, GL_LIGHTING);
   TEST_AND_UPDATE(ctx->Line.SmoothFlag, enable->LineSmooth, GL_LINE_SMOOTH);
   TEST_AND_UPDATE(ctx->Line.StippleFlag, enable->LineStipple,
                   GL_LINE_STIPPLE);
   TEST_AND_UPDATE(ctx->Color.IndexLogicOpEnabled, enable->IndexLogicOp,
                   GL_INDEX_LOGIC_OP);
   TEST_AND_UPDATE(ctx->Color.ColorLogicOpEnabled, enable->ColorLogicOp,
                   GL_COLOR_LOGIC_OP);

   TEST_AND_UPDATE(ctx->Eval.Map1Color4, enable->Map1Color4, GL_MAP1_COLOR_4);
   TEST_AND_UPDATE(ctx->Eval.Map1Index, enable->Map1Index, GL_MAP1_INDEX);
   TEST_AND_UPDATE(ctx->Eval.Map1Normal, enable->Map1Normal, GL_MAP1_NORMAL);
   TEST_AND_UPDATE(ctx->Eval.Map1TextureCoord1, enable->Map1TextureCoord1,
                   GL_MAP1_TEXTURE_COORD_1);
   TEST_AND_UPDATE(ctx->Eval.Map1TextureCoord2, enable->Map1TextureCoord2,
                   GL_MAP1_TEXTURE_COORD_2);
   TEST_AND_UPDATE(ctx->Eval.Map1TextureCoord3, enable->Map1TextureCoord3,
                   GL_MAP1_TEXTURE_COORD_3);
   TEST_AND_UPDATE(ctx->Eval.Map1TextureCoord4, enable->Map1TextureCoord4,
                   GL_MAP1_TEXTURE_COORD_4);
   TEST_AND_UPDATE(ctx->Eval.Map1Vertex3, enable->Map1Vertex3,
                   GL_MAP1_VERTEX_3);
   TEST_AND_UPDATE(ctx->Eval.Map1Vertex4, enable->Map1Vertex4,
                   GL_MAP1_VERTEX_4);
   for (i = 0; i < 16; i++) {
      TEST_AND_UPDATE(ctx->Eval.Map1Attrib[i], enable->Map1Attrib[i],
                      GL_MAP1_VERTEX_ATTRIB0_4_NV + i);
   }

   TEST_AND_UPDATE(ctx->Eval.Map2Color4, enable->Map2Color4, GL_MAP2_COLOR_4);
   TEST_AND_UPDATE(ctx->Eval.Map2Index, enable->Map2Index, GL_MAP2_INDEX);
   TEST_AND_UPDATE(ctx->Eval.Map2Normal, enable->Map2Normal, GL_MAP2_NORMAL);
   TEST_AND_UPDATE(ctx->Eval.Map2TextureCoord1, enable->Map2TextureCoord1,
                   GL_MAP2_TEXTURE_COORD_1);
   TEST_AND_UPDATE(ctx->Eval.Map2TextureCoord2, enable->Map2TextureCoord2,
                   GL_MAP2_TEXTURE_COORD_2);
   TEST_AND_UPDATE(ctx->Eval.Map2TextureCoord3, enable->Map2TextureCoord3,
                   GL_MAP2_TEXTURE_COORD_3);
   TEST_AND_UPDATE(ctx->Eval.Map2TextureCoord4, enable->Map2TextureCoord4,
                   GL_MAP2_TEXTURE_COORD_4);
   TEST_AND_UPDATE(ctx->Eval.Map2Vertex3, enable->Map2Vertex3,
                   GL_MAP2_VERTEX_3);
   TEST_AND_UPDATE(ctx->Eval.Map2Vertex4, enable->Map2Vertex4,
                   GL_MAP2_VERTEX_4);
   for (i = 0; i < 16; i++) {
      TEST_AND_UPDATE(ctx->Eval.Map2Attrib[i], enable->Map2Attrib[i],
                      GL_MAP2_VERTEX_ATTRIB0_4_NV + i);
   }

   TEST_AND_UPDATE(ctx->Eval.AutoNormal, enable->AutoNormal, GL_AUTO_NORMAL);
   TEST_AND_UPDATE(ctx->Transform.Normalize, enable->Normalize, GL_NORMALIZE);
   TEST_AND_UPDATE(ctx->Transform.RescaleNormals, enable->RescaleNormals,
                   GL_RESCALE_NORMAL_EXT);
   TEST_AND_UPDATE(ctx->Transform.RasterPositionUnclipped,
                   enable->RasterPositionUnclipped,
                   GL_RASTER_POSITION_UNCLIPPED_IBM);
   TEST_AND_UPDATE(ctx->Pixel.PixelTextureEnabled, enable->PixelTexture,
                   GL_POINT_SMOOTH);
   TEST_AND_UPDATE(ctx->Point.SmoothFlag, enable->PointSmooth,
                   GL_POINT_SMOOTH);
   if (ctx->Extensions.NV_point_sprite) {
      TEST_AND_UPDATE(ctx->Point.PointSprite, enable->PointSprite,
                      GL_POINT_SPRITE_NV);
   }
   TEST_AND_UPDATE(ctx->Polygon.OffsetPoint, enable->PolygonOffsetPoint,
                   GL_POLYGON_OFFSET_POINT);
   TEST_AND_UPDATE(ctx->Polygon.OffsetLine, enable->PolygonOffsetLine,
                   GL_POLYGON_OFFSET_LINE);
   TEST_AND_UPDATE(ctx->Polygon.OffsetFill, enable->PolygonOffsetFill,
                   GL_POLYGON_OFFSET_FILL);
   TEST_AND_UPDATE(ctx->Polygon.SmoothFlag, enable->PolygonSmooth,
                   GL_POLYGON_SMOOTH);
   TEST_AND_UPDATE(ctx->Polygon.StippleFlag, enable->PolygonStipple,
                   GL_POLYGON_STIPPLE);
   TEST_AND_UPDATE(ctx->Scissor.Enabled, enable->Scissor, GL_SCISSOR_TEST);
   TEST_AND_UPDATE(ctx->Stencil.Enabled, enable->Stencil, GL_STENCIL_TEST);
   /* XXX two-sided stencil */
   TEST_AND_UPDATE(ctx->Multisample.Enabled, enable->MultisampleEnabled,
                   GL_MULTISAMPLE_ARB);
   TEST_AND_UPDATE(ctx->Multisample.SampleAlphaToCoverage,
                   enable->SampleAlphaToCoverage,
                   GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
   TEST_AND_UPDATE(ctx->Multisample.SampleAlphaToOne,
                   enable->SampleAlphaToOne,
                   GL_SAMPLE_ALPHA_TO_ONE_ARB);
   TEST_AND_UPDATE(ctx->Multisample.SampleCoverage,
                   enable->SampleCoverage,
                   GL_SAMPLE_COVERAGE_ARB);
   TEST_AND_UPDATE(ctx->Multisample.SampleCoverageInvert,
                   enable->SampleCoverageInvert,
                   GL_SAMPLE_COVERAGE_INVERT_ARB);
   /* GL_NV_vertex_program */
   TEST_AND_UPDATE(ctx->VertexProgram.Enabled,
                   enable->VertexProgram,
                   GL_VERTEX_PROGRAM_NV);
   TEST_AND_UPDATE(ctx->VertexProgram.PointSizeEnabled,
                   enable->VertexProgramPointSize,
                   GL_VERTEX_PROGRAM_POINT_SIZE_NV);
   TEST_AND_UPDATE(ctx->VertexProgram.TwoSideEnabled,
                   enable->VertexProgramTwoSide,
                   GL_VERTEX_PROGRAM_TWO_SIDE_NV);

#undef TEST_AND_UPDATE

   /* texture unit enables */
   for (i = 0; i < ctx->Const.MaxTextureUnits; i++) {
      if (ctx->Texture.Unit[i].Enabled != enable->Texture[i]) {
         ctx->Texture.Unit[i].Enabled = enable->Texture[i];
         if (ctx->Driver.Enable) {
            if (ctx->Driver.ActiveTexture) {
               (*ctx->Driver.ActiveTexture)(ctx, i);
            }
            (*ctx->Driver.Enable)( ctx, GL_TEXTURE_1D,
                             (GLboolean) (enable->Texture[i] & TEXTURE_1D_BIT) );
            (*ctx->Driver.Enable)( ctx, GL_TEXTURE_2D,
                             (GLboolean) (enable->Texture[i] & TEXTURE_2D_BIT) );
            (*ctx->Driver.Enable)( ctx, GL_TEXTURE_3D,
                             (GLboolean) (enable->Texture[i] & TEXTURE_3D_BIT) );
            if (ctx->Extensions.ARB_texture_cube_map)
               (*ctx->Driver.Enable)( ctx, GL_TEXTURE_CUBE_MAP_ARB,
                          (GLboolean) (enable->Texture[i] & TEXTURE_CUBE_BIT) );
            if (ctx->Extensions.NV_texture_rectangle)
               (*ctx->Driver.Enable)( ctx, GL_TEXTURE_RECTANGLE_NV,
                          (GLboolean) (enable->Texture[i] & TEXTURE_RECT_BIT) );
         }
      }

      if (ctx->Texture.Unit[i].TexGenEnabled != enable->TexGen[i]) {
         ctx->Texture.Unit[i].TexGenEnabled = enable->TexGen[i];
         if (ctx->Driver.Enable) {
            if (ctx->Driver.ActiveTexture) {
               (*ctx->Driver.ActiveTexture)(ctx, i);
            }
            if (enable->TexGen[i] & S_BIT)
               (*ctx->Driver.Enable)( ctx, GL_TEXTURE_GEN_S, GL_TRUE);
            else
               (*ctx->Driver.Enable)( ctx, GL_TEXTURE_GEN_S, GL_FALSE);
            if (enable->TexGen[i] & T_BIT)
               (*ctx->Driver.Enable)( ctx, GL_TEXTURE_GEN_T, GL_TRUE);
            else
               (*ctx->Driver.Enable)( ctx, GL_TEXTURE_GEN_T, GL_FALSE);
            if (enable->TexGen[i] & R_BIT)
               (*ctx->Driver.Enable)( ctx, GL_TEXTURE_GEN_R, GL_TRUE);
            else
               (*ctx->Driver.Enable)( ctx, GL_TEXTURE_GEN_R, GL_FALSE);
            if (enable->TexGen[i] & Q_BIT)
               (*ctx->Driver.Enable)( ctx, GL_TEXTURE_GEN_Q, GL_TRUE);
            else
               (*ctx->Driver.Enable)( ctx, GL_TEXTURE_GEN_Q, GL_FALSE);
         }
      }
   }

   if (ctx->Driver.ActiveTexture) {
      (*ctx->Driver.ActiveTexture)(ctx, ctx->Texture.CurrentUnit);
   }
}


static void
pop_texture_group(GLcontext *ctx, const struct gl_texture_attrib *texAttrib)
{
   GLuint u;

   for (u = 0; u < ctx->Const.MaxTextureUnits; u++) {
      const struct gl_texture_unit *unit = &texAttrib->Unit[u];
      GLuint i;

      _mesa_ActiveTextureARB(GL_TEXTURE0_ARB + u);
      _mesa_set_enable(ctx, GL_TEXTURE_1D,
              (GLboolean) (unit->Enabled & TEXTURE_1D_BIT ? GL_TRUE : GL_FALSE));
      _mesa_set_enable(ctx, GL_TEXTURE_2D,
              (GLboolean) (unit->Enabled & TEXTURE_2D_BIT ? GL_TRUE : GL_FALSE));
      _mesa_set_enable(ctx, GL_TEXTURE_3D,
              (GLboolean) (unit->Enabled & TEXTURE_3D_BIT ? GL_TRUE : GL_FALSE));
      if (ctx->Extensions.ARB_texture_cube_map) {
         _mesa_set_enable(ctx, GL_TEXTURE_CUBE_MAP_ARB,
             (GLboolean) (unit->Enabled & TEXTURE_CUBE_BIT ? GL_TRUE : GL_FALSE));
      }
      if (ctx->Extensions.NV_texture_rectangle) {
         _mesa_set_enable(ctx, GL_TEXTURE_RECTANGLE_NV,
             (GLboolean) (unit->Enabled & TEXTURE_RECT_BIT ? GL_TRUE : GL_FALSE));
      }
      _mesa_TexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, unit->EnvMode);
      _mesa_TexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, unit->EnvColor);
      _mesa_TexGeni(GL_S, GL_TEXTURE_GEN_MODE, unit->GenModeS);
      _mesa_TexGeni(GL_T, GL_TEXTURE_GEN_MODE, unit->GenModeT);
      _mesa_TexGeni(GL_R, GL_TEXTURE_GEN_MODE, unit->GenModeR);
      _mesa_TexGeni(GL_Q, GL_TEXTURE_GEN_MODE, unit->GenModeQ);
      _mesa_TexGenfv(GL_S, GL_OBJECT_PLANE, unit->ObjectPlaneS);
      _mesa_TexGenfv(GL_T, GL_OBJECT_PLANE, unit->ObjectPlaneT);
      _mesa_TexGenfv(GL_R, GL_OBJECT_PLANE, unit->ObjectPlaneR);
      _mesa_TexGenfv(GL_Q, GL_OBJECT_PLANE, unit->ObjectPlaneQ);
      _mesa_TexGenfv(GL_S, GL_EYE_PLANE, unit->EyePlaneS);
      _mesa_TexGenfv(GL_T, GL_EYE_PLANE, unit->EyePlaneT);
      _mesa_TexGenfv(GL_R, GL_EYE_PLANE, unit->EyePlaneR);
      _mesa_TexGenfv(GL_Q, GL_EYE_PLANE, unit->EyePlaneQ);
      if (ctx->Extensions.EXT_texture_lod_bias) {
         _mesa_TexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT,
                       GL_TEXTURE_LOD_BIAS_EXT, unit->LodBias);
      }
      if (ctx->Extensions.EXT_texture_env_combine ||
          ctx->Extensions.ARB_texture_env_combine) {
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,
                       unit->CombineModeRGB);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT,
                       unit->CombineModeA);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT,
                       unit->CombineSourceRGB[0]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT,
                       unit->CombineSourceRGB[1]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT,
                       unit->CombineSourceRGB[2]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT,
                       unit->CombineSourceA[0]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT,
                       unit->CombineSourceA[1]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT,
                       unit->CombineSourceA[2]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT,
                       unit->CombineOperandRGB[0]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT,
                       unit->CombineOperandRGB[1]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT,
                       unit->CombineOperandRGB[2]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT,
                       unit->CombineOperandA[0]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT,
                       unit->CombineOperandA[1]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_EXT,
                       unit->CombineOperandA[2]);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT,
                       1 << unit->CombineScaleShiftRGB);
         _mesa_TexEnvi(GL_TEXTURE_ENV, GL_ALPHA_SCALE,
                       1 << unit->CombineScaleShiftA);
      }

      /* Restore texture object state */
      for (i = 0; i < NUM_TEXTURE_TARGETS; i++) {
         GLenum target = 0;
         const struct gl_texture_object *obj = NULL;
         GLfloat bordColor[4];

         switch (i) {
         case 0:
            target = GL_TEXTURE_1D;
            obj = &unit->Saved1D;
            break;
         case 1:
            target = GL_TEXTURE_2D;
            obj = &unit->Saved2D;
            break;
         case 2:
            target = GL_TEXTURE_3D;
            obj = &unit->Saved3D;
            break;
         case 3:
            if (!ctx->Extensions.ARB_texture_cube_map)
               continue;
            target = GL_TEXTURE_CUBE_MAP_ARB;
            obj = &unit->SavedCubeMap;
            break;
         case 4:
            if (!ctx->Extensions.NV_texture_rectangle)
               continue;
            target = GL_TEXTURE_RECTANGLE_NV;
            obj = &unit->SavedRect;
            break;
         default:
            ; /* silence warnings */
         }

         _mesa_BindTexture(target, obj->Name);

         bordColor[0] = CHAN_TO_FLOAT(obj->BorderColor[0]);
         bordColor[1] = CHAN_TO_FLOAT(obj->BorderColor[1]);
         bordColor[2] = CHAN_TO_FLOAT(obj->BorderColor[2]);
         bordColor[3] = CHAN_TO_FLOAT(obj->BorderColor[3]);

         _mesa_TexParameterf(target, GL_TEXTURE_PRIORITY, obj->Priority);
         _mesa_TexParameterfv(target, GL_TEXTURE_BORDER_COLOR, bordColor);
         _mesa_TexParameteri(target, GL_TEXTURE_WRAP_S, obj->WrapS);
         _mesa_TexParameteri(target, GL_TEXTURE_WRAP_T, obj->WrapT);
         _mesa_TexParameteri(target, GL_TEXTURE_WRAP_R, obj->WrapR);
         _mesa_TexParameteri(target, GL_TEXTURE_MIN_FILTER, obj->MinFilter);
         _mesa_TexParameteri(target, GL_TEXTURE_MAG_FILTER, obj->MagFilter);
         _mesa_TexParameterf(target, GL_TEXTURE_MIN_LOD, obj->MinLod);
         _mesa_TexParameterf(target, GL_TEXTURE_MAX_LOD, obj->MaxLod);
         _mesa_TexParameteri(target, GL_TEXTURE_BASE_LEVEL, obj->BaseLevel);
         _mesa_TexParameteri(target, GL_TEXTURE_MAX_LEVEL, obj->MaxLevel);
         if (ctx->Extensions.EXT_texture_filter_anisotropic) {
            _mesa_TexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                                obj->MaxAnisotropy);
         }
         if (ctx->Extensions.SGIX_shadow) {
            _mesa_TexParameteri(target, GL_TEXTURE_COMPARE_SGIX,
                                obj->CompareFlag);
            _mesa_TexParameteri(target, GL_TEXTURE_COMPARE_OPERATOR_SGIX,
                                obj->CompareOperator);
         }
         if (ctx->Extensions.SGIX_shadow_ambient) {
            _mesa_TexParameterf(target, GL_SHADOW_AMBIENT_SGIX,
                                obj->ShadowAmbient);
         }

      }
   }
   _mesa_ActiveTextureARB(GL_TEXTURE0_ARB
                          + texAttrib->CurrentUnit);

   /* "un-bump" the texture object reference counts.  We did that so they
    * wouldn't inadvertantly get deleted while they were still referenced
    * inside the attribute state stack.
    */
   for (u = 0; u < ctx->Const.MaxTextureUnits; u++) {
      ctx->Texture.Unit[u].Current1D->RefCount--;
      ctx->Texture.Unit[u].Current2D->RefCount--;
      ctx->Texture.Unit[u].Current3D->RefCount--;
      ctx->Texture.Unit[u].CurrentCubeMap->RefCount--;
      ctx->Texture.Unit[u].CurrentRect->RefCount--;
   }
}


/*
 * This function is kind of long just because we have to call a lot
 * of device driver functions to update device driver state.
 *
 * XXX As it is now, most of the pop-code calls immediate-mode Mesa functions
 * in order to restore GL state.  This isn't terribly efficient but it
 * ensures that dirty flags and any derived state gets updated correctly.
 * We could at least check if the value to restore equals the current value
 * and then skip the Mesa call.
 */
void
_mesa_PopAttrib(void)
{
   struct gl_attrib_node *attr, *next;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx);

   if (ctx->AttribStackDepth == 0) {
      _mesa_error( ctx, GL_STACK_UNDERFLOW, "glPopAttrib" );
      return;
   }

   ctx->AttribStackDepth--;
   attr = ctx->AttribStack[ctx->AttribStackDepth];

   while (attr) {

      if (MESA_VERBOSE & VERBOSE_API) {
         _mesa_debug(ctx, "glPopAttrib %s\n",
                     _mesa_lookup_enum_by_nr(attr->kind));
      }

      switch (attr->kind) {
         case GL_ACCUM_BUFFER_BIT:
            {
               const struct gl_accum_attrib *accum;
               accum = (const struct gl_accum_attrib *) attr->data;
               _mesa_ClearAccum(accum->ClearColor[0],
                                accum->ClearColor[1],
                                accum->ClearColor[2],
                                accum->ClearColor[3]);
            }
            break;
         case GL_COLOR_BUFFER_BIT:
            {
               const struct gl_colorbuffer_attrib *color;
               color = (const struct gl_colorbuffer_attrib *) attr->data;
               _mesa_ClearIndex((GLfloat) color->ClearIndex);
               _mesa_ClearColor(color->ClearColor[0],
                                color->ClearColor[1],
                                color->ClearColor[2],
                                color->ClearColor[3]);
               _mesa_IndexMask(color->IndexMask);
               _mesa_ColorMask((GLboolean) (color->ColorMask[0] != 0),
                               (GLboolean) (color->ColorMask[1] != 0),
                               (GLboolean) (color->ColorMask[2] != 0),
                               (GLboolean) (color->ColorMask[3] != 0));
               _mesa_DrawBuffer(color->DrawBuffer);
               _mesa_set_enable(ctx, GL_ALPHA_TEST, color->AlphaEnabled);
               _mesa_AlphaFunc(color->AlphaFunc, color->AlphaRef);
               _mesa_set_enable(ctx, GL_BLEND, color->BlendEnabled);
               _mesa_BlendFuncSeparateEXT(color->BlendSrcRGB,
                                          color->BlendDstRGB,
                                          color->BlendSrcA,
                                          color->BlendDstA);
               _mesa_BlendEquation(color->BlendEquation);
               _mesa_BlendColor(color->BlendColor[0],
                                color->BlendColor[1],
                                color->BlendColor[2],
                                color->BlendColor[3]);
               _mesa_LogicOp(color->LogicOp);
               _mesa_set_enable(ctx, GL_COLOR_LOGIC_OP,
                                color->ColorLogicOpEnabled);
               _mesa_set_enable(ctx, GL_INDEX_LOGIC_OP,
                                color->IndexLogicOpEnabled);
               _mesa_set_enable(ctx, GL_DITHER, color->DitherFlag);
            }
            break;
         case GL_CURRENT_BIT:
	    FLUSH_CURRENT( ctx, 0 );
            MEMCPY( &ctx->Current, attr->data,
		    sizeof(struct gl_current_attrib) );
            break;
         case GL_DEPTH_BUFFER_BIT:
            {
               const struct gl_depthbuffer_attrib *depth;
               depth = (const struct gl_depthbuffer_attrib *) attr->data;
               _mesa_DepthFunc(depth->Func);
               _mesa_ClearDepth(depth->Clear);
               _mesa_set_enable(ctx, GL_DEPTH_TEST, depth->Test);
               _mesa_DepthMask(depth->Mask);
               if (ctx->Extensions.HP_occlusion_test)
                  _mesa_set_enable(ctx, GL_OCCLUSION_TEST_HP,
                                   depth->OcclusionTest);
            }
            break;
         case GL_ENABLE_BIT:
            {
               const struct gl_enable_attrib *enable;
               enable = (const struct gl_enable_attrib *) attr->data;
               pop_enable_group(ctx, enable);
	       ctx->NewState |= _NEW_ALL;
            }
            break;
         case GL_EVAL_BIT:
            MEMCPY( &ctx->Eval, attr->data, sizeof(struct gl_eval_attrib) );
	    ctx->NewState |= _NEW_EVAL;
            break;
         case GL_FOG_BIT:
            {
               const struct gl_fog_attrib *fog;
               fog = (const struct gl_fog_attrib *) attr->data;
               _mesa_set_enable(ctx, GL_FOG, fog->Enabled);
               _mesa_Fogfv(GL_FOG_COLOR, fog->Color);
               _mesa_Fogf(GL_FOG_DENSITY, fog->Density);
               _mesa_Fogf(GL_FOG_START, fog->Start);
               _mesa_Fogf(GL_FOG_END, fog->End);
               _mesa_Fogf(GL_FOG_INDEX, fog->Index);
               _mesa_Fogi(GL_FOG_MODE, fog->Mode);
            }
            break;
         case GL_HINT_BIT:
            {
               const struct gl_hint_attrib *hint;
               hint = (const struct gl_hint_attrib *) attr->data;
               _mesa_Hint(GL_PERSPECTIVE_CORRECTION_HINT,
                          hint->PerspectiveCorrection );
               _mesa_Hint(GL_POINT_SMOOTH_HINT, hint->PointSmooth);
               _mesa_Hint(GL_LINE_SMOOTH_HINT, hint->LineSmooth);
               _mesa_Hint(GL_POLYGON_SMOOTH_HINT, hint->PolygonSmooth);
               _mesa_Hint(GL_FOG_HINT, hint->Fog);
               _mesa_Hint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT,
                          hint->ClipVolumeClipping);
               if (ctx->Extensions.ARB_texture_compression)
                  _mesa_Hint(GL_TEXTURE_COMPRESSION_HINT_ARB,
                             hint->TextureCompression);
            }
            break;
         case GL_LIGHTING_BIT:
            {
               GLuint i;
               const struct gl_light_attrib *light;
               light = (const struct gl_light_attrib *) attr->data;
               /* lighting enable */
               _mesa_set_enable(ctx, GL_LIGHTING, light->Enabled);
               /* per-light state */

	       if (ctx->ModelviewMatrixStack.Top->flags & MAT_DIRTY_INVERSE) 
		  _math_matrix_analyse( ctx->ModelviewMatrixStack.Top );
	       
               for (i = 0; i < MAX_LIGHTS; i++) {
                  GLenum lgt = (GLenum) (GL_LIGHT0 + i);
		  const struct gl_light *l = &light->Light[i];
		  GLfloat tmp[4];
                  _mesa_set_enable(ctx, lgt, l->Enabled);
		  _mesa_Lightfv( lgt, GL_AMBIENT, l->Ambient );
		  _mesa_Lightfv( lgt, GL_DIFFUSE, l->Diffuse );
		  _mesa_Lightfv( lgt, GL_SPECULAR, l->Specular );
		  TRANSFORM_POINT( tmp, ctx->ModelviewMatrixStack.Top->inv, l->EyePosition );
		  _mesa_Lightfv( lgt, GL_POSITION, tmp );
		  TRANSFORM_POINT( tmp, ctx->ModelviewMatrixStack.Top->m, l->EyeDirection );
		  _mesa_Lightfv( lgt, GL_SPOT_DIRECTION, tmp );
		  _mesa_Lightfv( lgt, GL_SPOT_EXPONENT, &l->SpotExponent );
		  _mesa_Lightfv( lgt, GL_SPOT_CUTOFF, &l->SpotCutoff );
		  _mesa_Lightfv( lgt, GL_CONSTANT_ATTENUATION, 
				 &l->ConstantAttenuation );
		  _mesa_Lightfv( lgt, GL_LINEAR_ATTENUATION, 
				 &l->LinearAttenuation );
		  _mesa_Lightfv( lgt, GL_QUADRATIC_ATTENUATION, 
				 &l->QuadraticAttenuation );
               }
               /* light model */
               _mesa_LightModelfv(GL_LIGHT_MODEL_AMBIENT,
                                  light->Model.Ambient);
               _mesa_LightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER,
                                 (GLfloat) light->Model.LocalViewer);
               _mesa_LightModelf(GL_LIGHT_MODEL_TWO_SIDE,
                                 (GLfloat) light->Model.TwoSide);
               _mesa_LightModelf(GL_LIGHT_MODEL_COLOR_CONTROL,
                                 (GLfloat) light->Model.ColorControl);
               /* materials */
               MEMCPY(ctx->Light.Material, light->Material,
                      2 * sizeof(struct gl_material));
               /* shade model */
               _mesa_ShadeModel(light->ShadeModel);
               /* color material */
               _mesa_ColorMaterial(light->ColorMaterialFace,
                                   light->ColorMaterialMode);
               _mesa_set_enable(ctx, GL_COLOR_MATERIAL,
                                light->ColorMaterialEnabled);
            }
            break;
         case GL_LINE_BIT:
            {
               const struct gl_line_attrib *line;
               line = (const struct gl_line_attrib *) attr->data;
               _mesa_set_enable(ctx, GL_LINE_SMOOTH, line->SmoothFlag);
               _mesa_set_enable(ctx, GL_LINE_STIPPLE, line->StippleFlag);
               _mesa_LineStipple(line->StippleFactor, line->StipplePattern);
               _mesa_LineWidth(line->Width);
            }
            break;
         case GL_LIST_BIT:
            MEMCPY( &ctx->List, attr->data, sizeof(struct gl_list_attrib) );
            break;
         case GL_PIXEL_MODE_BIT:
            MEMCPY( &ctx->Pixel, attr->data, sizeof(struct gl_pixel_attrib) );
	    ctx->NewState |= _NEW_PIXEL;
            break;
         case GL_POINT_BIT:
            {
               const struct gl_point_attrib *point;
               point = (const struct gl_point_attrib *) attr->data;
               _mesa_PointSize(point->Size);
               _mesa_set_enable(ctx, GL_POINT_SMOOTH, point->SmoothFlag);
               if (ctx->Extensions.EXT_point_parameters) {
                  _mesa_PointParameterfvEXT(GL_DISTANCE_ATTENUATION_EXT,
                                            point->Params);
                  _mesa_PointParameterfEXT(GL_POINT_SIZE_MIN_EXT,
                                           point->MinSize);
                  _mesa_PointParameterfEXT(GL_POINT_SIZE_MAX_EXT,
                                           point->MaxSize);
                  _mesa_PointParameterfEXT(GL_POINT_FADE_THRESHOLD_SIZE_EXT,
                                           point->Threshold);
               }
               if (ctx->Extensions.NV_point_sprite) {
                  GLuint u;
                  for (u = 0; u < ctx->Const.MaxTextureUnits; u++) {
                     _mesa_TexEnvi(GL_POINT_SPRITE_NV, GL_COORD_REPLACE_NV,
                                   (GLint) point->CoordReplace[u]);
                  }
                  _mesa_set_enable(ctx, GL_POINT_SPRITE_NV,point->PointSprite);
                  _mesa_PointParameteriNV(GL_POINT_SPRITE_R_MODE_NV,
                                          ctx->Point.SpriteRMode);
               }
            }
            break;
         case GL_POLYGON_BIT:
            {
               const struct gl_polygon_attrib *polygon;
               polygon = (const struct gl_polygon_attrib *) attr->data;
               _mesa_CullFace(polygon->CullFaceMode);
               _mesa_FrontFace(polygon->FrontFace);
               _mesa_PolygonMode(GL_FRONT, polygon->FrontMode);
               _mesa_PolygonMode(GL_BACK, polygon->BackMode);
               _mesa_PolygonOffset(polygon->OffsetFactor,
                                   polygon->OffsetUnits);
               _mesa_set_enable(ctx, GL_POLYGON_SMOOTH, polygon->SmoothFlag);
               _mesa_set_enable(ctx, GL_POLYGON_STIPPLE, polygon->StippleFlag);
               _mesa_set_enable(ctx, GL_CULL_FACE, polygon->CullFlag);
               _mesa_set_enable(ctx, GL_POLYGON_OFFSET_POINT,
                                polygon->OffsetPoint);
               _mesa_set_enable(ctx, GL_POLYGON_OFFSET_LINE,
                                polygon->OffsetLine);
               _mesa_set_enable(ctx, GL_POLYGON_OFFSET_FILL,
                                polygon->OffsetFill);
            }
            break;
	 case GL_POLYGON_STIPPLE_BIT:
	    MEMCPY( ctx->PolygonStipple, attr->data, 32*sizeof(GLuint) );
	    ctx->NewState |= _NEW_POLYGONSTIPPLE;
	    if (ctx->Driver.PolygonStipple)
	       ctx->Driver.PolygonStipple( ctx, (const GLubyte *) attr->data );
	    break;
         case GL_SCISSOR_BIT:
            {
               const struct gl_scissor_attrib *scissor;
               scissor = (const struct gl_scissor_attrib *) attr->data;
               _mesa_Scissor(scissor->X, scissor->Y,
                             scissor->Width, scissor->Height);
               _mesa_set_enable(ctx, GL_SCISSOR_TEST, scissor->Enabled);
            }
            break;
         case GL_STENCIL_BUFFER_BIT:
            {
               const GLint face = 0; /* XXX stencil two side */
               const struct gl_stencil_attrib *stencil;
               stencil = (const struct gl_stencil_attrib *) attr->data;
               _mesa_set_enable(ctx, GL_STENCIL_TEST, stencil->Enabled);
               _mesa_ClearStencil(stencil->Clear);
               _mesa_StencilFunc(stencil->Function[face], stencil->Ref[face],
                                 stencil->ValueMask[face]);
               _mesa_StencilMask(stencil->WriteMask[face]);
               _mesa_StencilOp(stencil->FailFunc[face],
                               stencil->ZFailFunc[face],
                               stencil->ZPassFunc[face]);
            }
            break;
         case GL_TRANSFORM_BIT:
            {
               GLuint i;
               const struct gl_transform_attrib *xform;
               xform = (const struct gl_transform_attrib *) attr->data;
               _mesa_MatrixMode(xform->MatrixMode);

               if (ctx->ProjectionMatrixStack.Top->flags & MAT_DIRTY)
                  _math_matrix_analyse( ctx->ProjectionMatrixStack.Top );

               /* restore clip planes */
               for (i = 0; i < MAX_CLIP_PLANES; i++) {
                  const GLuint mask = 1 << 1;
                  const GLfloat *eyePlane = xform->EyeUserPlane[i];
                  COPY_4V(ctx->Transform.EyeUserPlane[i], eyePlane);
                  if (xform->ClipPlanesEnabled & mask) {
                     _mesa_set_enable(ctx, GL_CLIP_PLANE0 + i, GL_TRUE);
                  }
                  else {
                     _mesa_set_enable(ctx, GL_CLIP_PLANE0 + i, GL_FALSE);
                  }
                  if (ctx->Driver.ClipPlane)
                     ctx->Driver.ClipPlane( ctx, GL_CLIP_PLANE0 + i, eyePlane );
               }

               /* normalize/rescale */
               if (xform->Normalize != ctx->Transform.Normalize)
                  _mesa_set_enable(ctx, GL_NORMALIZE,ctx->Transform.Normalize);
               if (xform->RescaleNormals != ctx->Transform.RescaleNormals)
                  _mesa_set_enable(ctx, GL_RESCALE_NORMAL_EXT,
                                   ctx->Transform.RescaleNormals);
            }
            break;
         case GL_TEXTURE_BIT:
            /* Take care of texture object reference counters */
            {
               const struct gl_texture_attrib *texture;
               texture = (const struct gl_texture_attrib *) attr->data;
               pop_texture_group(ctx, texture);
	       ctx->NewState |= _NEW_TEXTURE;
            }
            break;
         case GL_VIEWPORT_BIT:
            {
               const struct gl_viewport_attrib *vp;
               vp = (const struct gl_viewport_attrib *) attr->data;
               _mesa_Viewport(vp->X, vp->Y, vp->Width, vp->Height);
               _mesa_DepthRange(vp->Near, vp->Far);
            }
            break;
         case GL_MULTISAMPLE_BIT_ARB:
            {
               const struct gl_multisample_attrib *ms;
               ms = (const struct gl_multisample_attrib *) attr->data;
               _mesa_SampleCoverageARB(ms->SampleCoverageValue,
                                       ms->SampleCoverageInvert);
            }
            break;

         default:
            _mesa_problem( ctx, "Bad attrib flag in PopAttrib");
            break;
      }

      next = attr->next;
      FREE( attr->data );
      FREE( attr );
      attr = next;
   }
}


#define GL_CLIENT_PACK_BIT (1<<20)
#define GL_CLIENT_UNPACK_BIT (1<<21)


void
_mesa_PushClientAttrib(GLbitfield mask)
{
   struct gl_attrib_node *newnode;
   struct gl_attrib_node *head;

   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (ctx->ClientAttribStackDepth >= MAX_CLIENT_ATTRIB_STACK_DEPTH) {
      _mesa_error( ctx, GL_STACK_OVERFLOW, "glPushClientAttrib" );
      return;
   }

   /* Build linked list of attribute nodes which save all attribute */
   /* groups specified by the mask. */
   head = NULL;

   if (mask & GL_CLIENT_PIXEL_STORE_BIT) {
      struct gl_pixelstore_attrib *attr;
      /* packing attribs */
      attr = MALLOC_STRUCT( gl_pixelstore_attrib );
      MEMCPY( attr, &ctx->Pack, sizeof(struct gl_pixelstore_attrib) );
      newnode = new_attrib_node( GL_CLIENT_PACK_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
      /* unpacking attribs */
      attr = MALLOC_STRUCT( gl_pixelstore_attrib );
      MEMCPY( attr, &ctx->Unpack, sizeof(struct gl_pixelstore_attrib) );
      newnode = new_attrib_node( GL_CLIENT_UNPACK_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }
   if (mask & GL_CLIENT_VERTEX_ARRAY_BIT) {
      struct gl_array_attrib *attr;
      attr = MALLOC_STRUCT( gl_array_attrib );
      MEMCPY( attr, &ctx->Array, sizeof(struct gl_array_attrib) );
      newnode = new_attrib_node( GL_CLIENT_VERTEX_ARRAY_BIT );
      newnode->data = attr;
      newnode->next = head;
      head = newnode;
   }

   ctx->ClientAttribStack[ctx->ClientAttribStackDepth] = head;
   ctx->ClientAttribStackDepth++;
}




void
_mesa_PopClientAttrib(void)
{
   struct gl_attrib_node *attr, *next;

   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx);

   if (ctx->ClientAttribStackDepth == 0) {
      _mesa_error( ctx, GL_STACK_UNDERFLOW, "glPopClientAttrib" );
      return;
   }

   ctx->ClientAttribStackDepth--;
   attr = ctx->ClientAttribStack[ctx->ClientAttribStackDepth];

   while (attr) {
      switch (attr->kind) {
         case GL_CLIENT_PACK_BIT:
            MEMCPY( &ctx->Pack, attr->data,
                    sizeof(struct gl_pixelstore_attrib) );
	    ctx->NewState |= _NEW_PACKUNPACK;
            break;
         case GL_CLIENT_UNPACK_BIT:
            MEMCPY( &ctx->Unpack, attr->data,
                    sizeof(struct gl_pixelstore_attrib) );
	    ctx->NewState |= _NEW_PACKUNPACK;
            break;
         case GL_CLIENT_VERTEX_ARRAY_BIT:
            MEMCPY( &ctx->Array, attr->data,
		    sizeof(struct gl_array_attrib) );
	    ctx->NewState |= _NEW_ARRAY;
            break;
         default:
            _mesa_problem( ctx, "Bad attrib flag in PopClientAttrib");
            break;
      }

      next = attr->next;
      FREE( attr->data );
      FREE( attr );
      attr = next;
   }
}
