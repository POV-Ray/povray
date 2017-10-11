#ifdef MESA_TRACE

#include "GL/gl.h"
#include "tr_context.h"
#include "tr_support.h"
#include "tr_write.h"


#if 0
static void trQueryConvolutionState( void ) {

	if( trCtx()->trConvolutionWidth == -1 ) {
		/* Query it */
		trWriteCMD( VAR_OOBBEGIN );
		trGetDispatch()->GetConvolutionParameteriv( GL_SEPARABLE_2D, GL_CONVOLUTION_WIDTH,
		                                          &(trCtx()->trConvolutionWidth) );
		trWriteCMD( VAR_OOBEND );
	}

	if( trCtx()->trConvolutionHeight == -1 ) {
		/* Query it */
		trWriteCMD( VAR_OOBBEGIN );
		trGetDispatch()->GetConvolutionParameteriv( GL_SEPARABLE_2D, GL_CONVOLUTION_HEIGHT,
		                                          &(trCtx()->trConvolutionHeight) );
		trWriteCMD( VAR_OOBEND );
	}
}
#endif


void trZeroGetterData( GLenum pname, GLsizei typesize, GLvoid * params ) {

   switch( pname ) {
      case GL_COLOR_MATRIX:
      case GL_MODELVIEW_MATRIX:
      case GL_PROJECTION_MATRIX:
      case GL_TEXTURE_MATRIX:
			memset( params, 0, typesize * 16 );
         break;

      case GL_ACCUM_CLEAR_VALUE:
      case GL_BLEND_COLOR:
      case GL_COLOR_CLEAR_VALUE:
      case GL_COLOR_WRITEMASK:
      case GL_CURRENT_COLOR:
      case GL_CURRENT_RASTER_COLOR:
      case GL_CURRENT_RASTER_POSITION:
      case GL_CURRENT_RASTER_TEXTURE_COORDS:
      case GL_CURRENT_TEXTURE_COORDS:
      case GL_LIGHT_MODEL_AMBIENT:
      case GL_MAP2_GRID_DOMAIN:
      case GL_SCISSOR_BOX:
      case GL_VIEWPORT:
			memset( params, 0, typesize * 4 );
         break;

      case GL_CURRENT_NORMAL:
			memset( params, 0, typesize * 3 );
         break;

      case GL_ALIASED_POINT_SIZE_RANGE:
      case GL_ALIASED_LINE_WIDTH_RANGE:
      case GL_DEPTH_RANGE:
      case GL_MAP1_GRID_DOMAIN:
      case GL_MAP2_GRID_SEGMENTS:
      case GL_MAX_VIEWPORT_DIMS:
      case GL_POLYGON_MODE:
      case GL_SMOOTH_LINE_WIDTH_RANGE:
      case GL_SMOOTH_POINT_SIZE_RANGE:
			memset( params, 0, typesize * 2 );
         break;

      case GL_ACCUM_ALPHA_BITS:
      case GL_ACCUM_BLUE_BITS:
      case GL_ACCUM_GREEN_BITS:
      case GL_ACCUM_RED_BITS:
      case GL_ACTIVE_TEXTURE_ARB:
      case GL_ALPHA_BIAS:
      case GL_ALPHA_BITS:
      case GL_ALPHA_SCALE:
      case GL_ALPHA_TEST:
      case GL_ALPHA_TEST_FUNC:
      case GL_ALPHA_TEST_REF:
      case GL_ATTRIB_STACK_DEPTH:
      case GL_AUTO_NORMAL:
      case GL_AUX_BUFFERS:
      case GL_BLEND:
      case GL_BLEND_SRC:
      case GL_BLUE_BIAS:
      case GL_BLUE_BITS:
      case GL_BLUE_SCALE:
      case GL_CLIENT_ACTIVE_TEXTURE_ARB:
      case GL_CLIENT_ATTRIB_STACK_DEPTH:
      case GL_CLIP_PLANE0:
      case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2:
      case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4:
      case GL_CLIP_PLANE5:
      case GL_COLOR_ARRAY:
      case GL_COLOR_ARRAY_SIZE:
      case GL_COLOR_ARRAY_STRIDE:
      case GL_COLOR_ARRAY_TYPE:
      case GL_COLOR_LOGIC_OP:
      case GL_COLOR_MATERIAL:
      case GL_COLOR_MATERIAL_FACE:
      case GL_COLOR_MATERIAL_PARAMETER:
      case GL_COLOR_MATRIX_STACK_DEPTH:
      case GL_COLOR_TABLE:
      case GL_CONVOLUTION_1D:
      case GL_CONVOLUTION_2D:
      case GL_CULL_FACE:
      case GL_CULL_FACE_MODE:
      case GL_CURRENT_INDEX:
      case GL_CURRENT_RASTER_DISTANCE:
      case GL_CURRENT_RASTER_INDEX:
      case GL_CURRENT_RASTER_POSITION_VALID:
      case GL_DEPTH_BIAS:
      case GL_DEPTH_CLEAR_VALUE:
      case GL_DEPTH_FUNC:
      case GL_DEPTH_SCALE:
      case GL_DEPTH_TEST:
      case GL_DEPTH_WRITEMASK:
      case GL_DITHER:
      case GL_DOUBLEBUFFER:
      case GL_DRAW_BUFFER:
      case GL_EDGE_FLAG:
      case GL_EDGE_FLAG_ARRAY:
      case GL_EDGE_FLAG_ARRAY_STRIDE:
      case GL_FEEDBACK_BUFFER_SIZE:
      case GL_FEEDBACK_BUFFER_TYPE:
      case GL_FOG:
      case GL_FOG_DENSITY:
      case GL_FOG_END:
      case GL_FOG_HINT:
      case GL_FOG_INDEX:
      case GL_FOG_MODE:
      case GL_FOG_START:
      case GL_FRONT_FACE:
      case GL_GREEN_BIAS:
      case GL_GREEN_BITS:
      case GL_GREEN_SCALE:
      case GL_HISTOGRAM:
      case GL_INDEX_ARRAY:
      case GL_INDEX_ARRAY_STRIDE:
      case GL_INDEX_ARRAY_TYPE:
      case GL_INDEX_BITS:
      case GL_INDEX_CLEAR_VALUE:
      case GL_INDEX_LOGIC_OP:
      case GL_INDEX_MODE:
      case GL_INDEX_OFFSET:
      case GL_INDEX_SHIFT:
      case GL_INDEX_WRITEMASK:
      case GL_LIGHT0:
      case GL_LIGHT1:
      case GL_LIGHT2:
      case GL_LIGHT3:
      case GL_LIGHT4:
      case GL_LIGHT5:
      case GL_LIGHT6:
      case GL_LIGHT7:
      case GL_LIGHTING:
      case GL_LIGHT_MODEL_COLOR_CONTROL:
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
      case GL_LIGHT_MODEL_TWO_SIDE:
      case GL_LINE_SMOOTH:
      case GL_LINE_SMOOTH_HINT:
      case GL_LINE_STIPPLE:
      case GL_LINE_STIPPLE_PATTERN:
      case GL_LINE_STIPPLE_REPEAT:
      case GL_LINE_WIDTH:
      case GL_LIST_BASE:
      case GL_LIST_INDEX:
      case GL_LIST_MODE:
      case GL_LOGIC_OP_MODE:
      case GL_MAP1_COLOR_4:
      case GL_MAP1_GRID_SEGMENTS:
      case GL_MAP1_INDEX:
      case GL_MAP1_NORMAL:
      case GL_MAP1_TEXTURE_COORD_1:
      case GL_MAP1_TEXTURE_COORD_2:
      case GL_MAP1_TEXTURE_COORD_3:
      case GL_MAP1_TEXTURE_COORD_4:
      case GL_MAP1_VERTEX_3:
      case GL_MAP1_VERTEX_4:
      case GL_MAP2_COLOR_4:
      case GL_MAP2_INDEX:
      case GL_MAP2_NORMAL:
      case GL_MAP2_TEXTURE_COORD_1:
      case GL_MAP2_TEXTURE_COORD_2:
      case GL_MAP2_TEXTURE_COORD_3:
      case GL_MAP2_TEXTURE_COORD_4:
      case GL_MAP2_VERTEX_3:
      case GL_MAP2_VERTEX_4:
      case GL_MAP_COLOR:
      case GL_MAP_STENCIL:
      case GL_MATRIX_MODE:
      case GL_MAX_3D_TEXTURE_SIZE:
      case GL_MAX_CLIENT_ATTRIB_STACK_DEPTH:
      case GL_MAX_ATTRIB_STACK_DEPTH:
      case GL_MAX_CLIP_PLANES:
      case GL_MAX_COLOR_MATRIX_STACK_DEPTH:
      case GL_MAX_ELEMENTS_VERTICES:
      case GL_MAX_EVAL_ORDER:
      case GL_MAX_LIGHTS:
      case GL_MAX_LIST_NESTING:
      case GL_MAX_MODELVIEW_STACK_DEPTH:
      case GL_MAX_NAME_STACK_DEPTH:
      case GL_MAX_PIXEL_MAP_TABLE:
      case GL_MAX_PROJECTION_STACK_DEPTH:
      case GL_MAX_TEXTURE_SIZE:
      case GL_MAX_TEXTURE_STACK_DEPTH:
      case GL_MAX_TEXTURE_UNITS_ARB:
      case GL_MINMAX:
      case GL_MODELVIEW_STACK_DEPTH:
      case GL_NAME_STACK_DEPTH:
      case GL_NORMAL_ARRAY:
      case GL_NORMAL_ARRAY_STRIDE:
      case GL_NORMAL_ARRAY_TYPE:
      case GL_NORMALIZE:
      case GL_PACK_ALIGNMENT:
      case GL_PACK_IMAGE_HEIGHT:
      case GL_PACK_LSB_FIRST:
      case GL_PACK_ROW_LENGTH:
      case GL_PACK_SKIP_IMAGES:
      case GL_PACK_SKIP_PIXELS:
      case GL_PACK_SKIP_ROWS:
      case GL_PACK_SWAP_BYTES:
      case GL_PERSPECTIVE_CORRECTION_HINT:
      case GL_PIXEL_MAP_A_TO_A_SIZE:
      case GL_PIXEL_MAP_B_TO_B_SIZE:
      case GL_PIXEL_MAP_G_TO_G_SIZE:
      case GL_PIXEL_MAP_I_TO_A_SIZE:
      case GL_PIXEL_MAP_I_TO_B_SIZE:
      case GL_PIXEL_MAP_I_TO_G_SIZE:
      case GL_PIXEL_MAP_I_TO_I_SIZE:
      case GL_PIXEL_MAP_I_TO_R_SIZE:
      case GL_PIXEL_MAP_R_TO_R_SIZE:
      case GL_PIXEL_MAP_S_TO_S_SIZE:
      case GL_POINT_SIZE:
      case GL_POINT_SMOOTH:
      case GL_POINT_SMOOTH_HINT:
      case GL_POLYGON_OFFSET_FACTOR:
      case GL_POLYGON_OFFSET_UNITS:
      case GL_POLYGON_OFFSET_FILL:
      case GL_POLYGON_OFFSET_LINE:
      case GL_POLYGON_OFFSET_POINT:
      case GL_POLYGON_SMOOTH:
      case GL_POLYGON_SMOOTH_HINT:
      case GL_POLYGON_STIPPLE:
      case GL_POST_COLOR_MATRIX_COLOR_TABLE:
      case GL_POST_COLOR_MATRIX_RED_BIAS:
      case GL_POST_COLOR_MATRIX_GREEN_BIAS:
      case GL_POST_COLOR_MATRIX_BLUE_BIAS:
      case GL_POST_COLOR_MATRIX_ALPHA_BIAS:
      case GL_POST_COLOR_MATRIX_RED_SCALE:
      case GL_POST_COLOR_MATRIX_GREEN_SCALE:
      case GL_POST_COLOR_MATRIX_BLUE_SCALE:
      case GL_POST_COLOR_MATRIX_ALPHA_SCALE:
      case GL_POST_CONVOLUTION_COLOR_TABLE:
      case GL_POST_CONVOLUTION_RED_BIAS:
      case GL_POST_CONVOLUTION_GREEN_BIAS:
      case GL_POST_CONVOLUTION_BLUE_BIAS:
      case GL_POST_CONVOLUTION_ALPHA_BIAS:
      case GL_POST_CONVOLUTION_RED_SCALE:
      case GL_POST_CONVOLUTION_GREEN_SCALE:
      case GL_POST_CONVOLUTION_BLUE_SCALE:
      case GL_POST_CONVOLUTION_ALPHA_SCALE:
      case GL_PROJECTION_STACK_DEPTH:
      case GL_READ_BUFFER:
      case GL_RED_BIAS:
      case GL_RED_BITS:
      case GL_RED_SCALE:
      case GL_RENDER_MODE:
      case GL_RESCALE_NORMAL:
      case GL_RGBA_MODE:
      case GL_SCISSOR_TEST:
      case GL_SELECTION_BUFFER_SIZE:
      case GL_SEPARABLE_2D:
      case GL_SHADE_MODEL:
      case GL_SMOOTH_LINE_WIDTH_GRANULARITY:
      case GL_SMOOTH_POINT_SIZE_GRANULARITY:
      case GL_STENCIL_BITS:
      case GL_STENCIL_CLEAR_VALUE:
      case GL_STENCIL_FAIL:
      case GL_STENCIL_FUNC:
      case GL_STENCIL_PASS_DEPTH_FAIL:
      case GL_STENCIL_PASS_DEPTH_PASS:
      case GL_STENCIL_REF:
      case GL_STENCIL_TEST:
      case GL_STENCIL_VALUE_MASK:
      case GL_STENCIL_WRITEMASK:
      case GL_STEREO:
      case GL_SUBPIXEL_BITS:
      case GL_TEXTURE_1D:
      case GL_TEXTURE_BINDING_1D:
      case GL_TEXTURE_2D:
      case GL_TEXTURE_BINDING_2D:
      case GL_TEXTURE_3D:
      case GL_TEXTURE_BINDING_3D:
      case GL_TEXTURE_COORD_ARRAY:
      case GL_TEXTURE_COORD_ARRAY_SIZE:
      case GL_TEXTURE_COORD_ARRAY_STRIDE:
      case GL_TEXTURE_COORD_ARRAY_TYPE:
      case GL_TEXTURE_GEN_Q:
      case GL_TEXTURE_GEN_R:
      case GL_TEXTURE_GEN_S:
      case GL_TEXTURE_GEN_T:
      case GL_TEXTURE_STACK_DEPTH:
      case GL_UNPACK_ALIGNMENT:
      case GL_UNPACK_IMAGE_HEIGHT:
      case GL_UNPACK_LSB_FIRST:
      case GL_UNPACK_ROW_LENGTH:
      case GL_UNPACK_SKIP_IMAGES:
      case GL_UNPACK_SKIP_PIXELS:
      case GL_UNPACK_SKIP_ROWS:
      case GL_UNPACK_SWAP_BYTES:
      case GL_VERTEX_ARRAY:
      case GL_VERTEX_ARRAY_SIZE:
      case GL_VERTEX_ARRAY_STRIDE:
      case GL_VERTEX_ARRAY_TYPE:
      case GL_ZOOM_X:
      case GL_ZOOM_Y:
			memset( params, 0, typesize );
         break;

      default:
         /* Bad enum.  What should we do? */
         break;
   }
}


void trPrintColorTableData( GLenum pname, GLenum type, GLvoid * params ) {
   if( !(trCtx()->doExec) ) {
		switch( pname ) {
			case GL_COLOR_TABLE_SCALE:
			case GL_COLOR_TABLE_BIAS:
            if( type == GL_FLOAT ) {
				   memset( params, 0, 4 * sizeof(GLfloat) );
            } else {
               memset( params, 0, 4 * sizeof(GLint) );
            }
				break;

			case GL_COLOR_TABLE_FORMAT:
			case GL_COLOR_TABLE_WIDTH:
			case GL_COLOR_TABLE_RED_SIZE:
			case GL_COLOR_TABLE_GREEN_SIZE:
			case GL_COLOR_TABLE_BLUE_SIZE:
			case GL_COLOR_TABLE_ALPHA_SIZE:
			case GL_COLOR_TABLE_LUMINANCE_SIZE:
			case GL_COLOR_TABLE_INTENSITY_SIZE:
            if( type == GL_FLOAT ) {
				   ((GLfloat *)params)[0] = 0.0;
            } else {
               ((GLint *)params)[0] = 0;
            }
				break;

			default:
				/* The 2nd pass should catch this */
				break;
		}
	}

	switch( pname ) {
		case GL_COLOR_TABLE_SCALE:
		case GL_COLOR_TABLE_BIAS:
         if( type == GL_FLOAT ) {
			   trWriteArrayf( 4, (GLfloat *)params );
         } else {
			   trWriteArrayi( 4, (GLint *)params );
         }
			break;

		case GL_COLOR_TABLE_FORMAT:
		case GL_COLOR_TABLE_WIDTH:
		case GL_COLOR_TABLE_RED_SIZE:
		case GL_COLOR_TABLE_GREEN_SIZE:
		case GL_COLOR_TABLE_BLUE_SIZE:
		case GL_COLOR_TABLE_ALPHA_SIZE:
		case GL_COLOR_TABLE_LUMINANCE_SIZE:
		case GL_COLOR_TABLE_INTENSITY_SIZE:
         if( type == GL_FLOAT ) {
			   trWritef( ((GLfloat *)params)[0] );
         } else {
			   trWritei( ((GLint *)params)[0] );
         }
			break;

		default:
			/* The 2nd pass should catch this */
			break;
	}
}


void trWriteTypeArray( GLenum type, GLsizei width, GLsizei pixelsize, GLint start, const GLvoid * ptr ) {

	switch( type ) {
		case GL_BYTE:
			{
				GLbyte * p = (GLbyte *)ptr + start * pixelsize;
				trWriteArrayb( width * pixelsize, p );
			}
			break;

		case GL_UNSIGNED_BYTE:
		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:
			{
				GLubyte * p = (GLubyte *)ptr + start * pixelsize;
				trWriteArrayub( width * pixelsize, p );
			}
			break;

		case GL_SHORT:
			{
				GLshort * p = (GLshort *)ptr + start * pixelsize;
				trWriteArrays( width * pixelsize, p );
			}
			break;

		case GL_UNSIGNED_SHORT:
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			{
				GLushort * p = (GLushort *)ptr + start * pixelsize;
				trWriteArrayus( width * pixelsize, p );
			}
			break;

		case GL_INT:
			{
				GLint * p = (GLint *)ptr + start * pixelsize;
				trWriteArrayi( width * pixelsize, p );
			}
			break;

		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			{
				GLuint * p = (GLuint *)ptr + start * pixelsize;
				trWriteArrayui( width * pixelsize, p );
			}
			break;

		case GL_FLOAT:
			{
				GLfloat * p = (GLfloat *)ptr + start * pixelsize;
				trWriteArrayf( width * pixelsize, p );
			}
			break;

		default:
			/* The 2nd pass should catch this. */
			break;
	}
}


static GLint trGetFormatSize( GLenum format ) {
	GLint pixelsize;

	switch( format ) {
		case GL_RED:
		case GL_GREEN:
		case GL_BLUE:
		case GL_ALPHA:
		case GL_LUMINANCE:
		case GL_INTENSITY:
		case GL_COLOR_INDEX:
		case GL_STENCIL_INDEX:
		case GL_DEPTH_COMPONENT:
			pixelsize = 1;
			break;
		case GL_LUMINANCE_ALPHA:
			pixelsize = 2;
			break;
		case GL_RGB:
		case GL_BGR:
			pixelsize = 3;
			break;
		case GL_RGBA:
		case GL_BGRA:
			pixelsize = 4;
			break;
		default:
			/* The 2nd pass should catch this. */
			pixelsize = 0;
			break;
	}

	return pixelsize;
}


GLint trGetPixelSize( GLenum format, GLenum type ) {
   GLint retval;
   GLint formatsize = trGetFormatSize( format );

   switch( type ) {
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
         retval = formatsize * sizeof(GLubyte);
         break;

		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:
         if( (type == GL_RGB) || (type == GL_BGR) ) {
            retval = sizeof(GLubyte);
         } else {
            retval = -1;
         }
         break;

		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
         retval = sizeof(GLushort);
         break;

		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
         if( (type == GL_RGB) || (type == GL_BGR) ) {
            retval = sizeof(GLushort);
         } else {
            retval = -1;
         }
         break;

		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
         if( (type == GL_RGBA) || (type == GL_BGRA) ) {
            retval = sizeof(GLushort);
         } else {
            retval = -1;
         }
         break;

		case GL_INT:
		case GL_UNSIGNED_INT:
         retval = sizeof(GLuint);
         break;

		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
         if( (type == GL_RGBA) || (type == GL_BGRA) ) {
            retval = sizeof(GLuint);
         } else {
            retval = -1;
         }
         break;

		case GL_FLOAT:
         retval = sizeof(GLfloat);
         break;

      default:
         retval = -1;
         break;
   }

   return retval;
}


#else
extern void tr_support_dummy_func(void);
void tr_support_dummy_func(void)
{
}
#endif
