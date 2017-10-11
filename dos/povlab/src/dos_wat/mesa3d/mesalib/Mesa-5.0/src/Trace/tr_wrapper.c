#ifdef MESA_TRACE

#include "glheader.h"
#include "glapitable.h"
#include "tr_write.h"
#include "tr_error.h"
#include "tr_context.h"
#include "tr_commands.h"
#include "tr_support.h"
#include "tr_wrapper.h"


#ifdef GLAPI
#undef GLAPI
#endif
#define GLAPI static

#ifdef GLAPIENTRY
#undef GLAPIENTRY
#endif
#define GLAPIENTRY


GLAPI void GLAPIENTRY trAccum( GLenum op, GLfloat value ) {
	trWriteCMD( CMD_ACCUM );
	trWriteEnum( op );
	trWritef( value );

	if( trCtx()->doExec ) {
		trGetDispatch()->Accum( op, value );
		trError();
	}
}


GLAPI void GLAPIENTRY trActiveTextureARB( GLenum texture) {
	trWriteCMD( CMD_ACTIVETEXTUREARB );
	trWriteEnum( texture );

	if( trCtx()->doExec ) {
		trGetDispatch()->ActiveTextureARB( texture );
		trError();
	}
}


GLAPI void GLAPIENTRY trAlphaFunc( GLenum func, GLclampf ref ) {
	trWriteCMD( CMD_ALPHAFUNC );
	trWriteEnum( func );
	trWriteClampf( ref );

	if( trCtx()->doExec ) {
		trGetDispatch()->AlphaFunc( func, ref );
		trError();
	}
}


GLAPI GLboolean GLAPIENTRY trAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences ) {
	GLboolean retval;

	trWriteCMD( CMD_ARETEXTURESRESIDENT );
	trWriteSizei( n );
	trWritePointer( (void *)textures );
	trFileFlush();
	trWriteArrayui( n, textures );
	trWritePointer( (void *)residences  );
	trFileFlush();

	if( trCtx()->doExec ) {
		retval = trGetDispatch()->AreTexturesResident( n, textures, residences  );
		trError();
	} else {
		memset( residences, 0, n * sizeof(GLboolean) );
		retval = GL_NO_ERROR;
	}

	trWriteBool( retval );
	trWriteArrayBool( n, residences );

	return retval;
}


GLAPI GLboolean GLAPIENTRY trAreTexturesResidentEXT( GLsizei n, const GLuint *textures, GLboolean *residences ) {
	GLboolean retval;

	trWriteCMD( CMD_ARETEXTURESRESIDENTEXT );
	trWriteSizei( n );
	trWritePointer( (void *)textures );
	trFileFlush();
	trWriteArrayui( n, textures );
	trWritePointer( (void *)residences  );
	trFileFlush();

	if( trCtx()->doExec ) {
		retval = trGetDispatch()->AreTexturesResidentEXT( n, textures, residences  );
		trError();
	} else {
		memset( residences, 0, n * sizeof(GLboolean) );
		retval = GL_NO_ERROR;
	}

	trWriteBool( retval );
	trWriteArrayBool( n, residences );

	return retval;
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trArrayElementEXT( GLint i ) {
	trWriteCMD( CMD_ARRAYELEMENTEXT );
	trWritei( i );

	if( trCtx()->doExec ) {
		trGetDispatch()->ArrayElementEXT( i );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trArrayElement( GLint i ) {
	trace_context_t * tctx;

	trWriteCMD( CMD_ARRAYELEMENT );
	trWritei( i );

	tctx = trCtx();

	if( tctx->doExec ) {
		trGetDispatch()->ArrayElement( i );
		trError();
	}
}


GLAPI void GLAPIENTRY trBegin( GLenum mode ) {
	trWriteCMD( CMD_BEGIN );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->Begin( mode );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trBindTextureEXT( GLenum target, GLuint texture ) {
	trWriteCMD( CMD_BINDTEXTUREEXT );
	trWriteEnum( target );
	trWriteui( texture );

	if( trCtx()->doExec ) {
		trGetDispatch()->BindTextureEXT( target, texture );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trBindTexture( GLenum target, GLuint texture ) {
	trWriteCMD( CMD_BINDTEXTURE );
	trWriteEnum( target );
	trWriteui( texture );

	if( trCtx()->doExec ) {
		trGetDispatch()->BindTexture( target, texture );
		trError();
	}
}


GLAPI void GLAPIENTRY trBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap ) {
	trWriteCMD( CMD_BITMAP );
	trWriteSizei( width );
	trWriteSizei( height );
	trWritef( xorig );
	trWritef( yorig );
	trWritef( xmove );
	trWritef( ymove );
	trWritePointer( (void *)bitmap  );
	trFileFlush();
	trWriteArrayub( width * height / 8, bitmap );

	if( trCtx()->doExec ) {
		trGetDispatch()->Bitmap( width, height, xorig, yorig, xmove, ymove, bitmap  );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trBlendColorEXT( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
	trWriteCMD( CMD_BLENDCOLOREXT );
	trWriteClampf( red );
	trWriteClampf( green );
	trWriteClampf( blue );
	trWriteClampf( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->BlendColorEXT( red, green, blue, alpha );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
	trWriteCMD( CMD_BLENDCOLOR );
	trWriteClampf( red );
	trWriteClampf( green );
	trWriteClampf( blue );
	trWriteClampf( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->BlendColor( red, green, blue, alpha );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trBlendEquationEXT( GLenum mode ) {
	trWriteCMD( CMD_BLENDEQUATIONEXT );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->BlendEquationEXT( mode );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trBlendEquation( GLenum mode ) {
	trWriteCMD( CMD_BLENDEQUATION );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->BlendEquation( mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trBlendFunc( GLenum sfactor, GLenum dfactor ) {
	trWriteCMD( CMD_BLENDFUNC );
	trWriteEnum( sfactor );
	trWriteEnum( dfactor );

	if( trCtx()->doExec ) {
		trGetDispatch()->BlendFunc( sfactor, dfactor );
		trError();
	}
}


GLAPI void GLAPIENTRY trCallList( GLuint list ) {
	trWriteCMD( CMD_CALLLIST );
	trWriteui( list );

	if( trCtx()->doExec ) {
		trSetOriginalDispatch();
		trGetDispatch()->CallList( list );
		trSetTraceDispatch();
		trError();
	}
}


GLAPI void GLAPIENTRY trCallLists( GLsizei n, GLenum type, const GLvoid *lists ) {
	trWriteCMD( CMD_CALLLISTS );
	trWriteSizei( n );
	trWriteEnum( type );
	trWritePointer( (void *)lists  );
	trFileFlush();

	switch( type ) {
		case GL_2_BYTES:
			trWriteTypeArray( GL_UNSIGNED_BYTE, n, 2, 0, lists );
			break;
		case GL_3_BYTES:
			trWriteTypeArray( GL_UNSIGNED_BYTE, n, 3, 0, lists );
			break;
		case GL_4_BYTES:
			trWriteTypeArray( GL_UNSIGNED_BYTE, n, 4, 0, lists );
			break;
		default:
			trWriteTypeArray( type, n, 1, 0, lists );
			break;
	}

	if( trCtx()->doExec ) {
		trSetOriginalDispatch();
		trGetDispatch()->CallLists( n, type, lists  );
		trSetTraceDispatch();
		trError();
	}
}


GLAPI void GLAPIENTRY trClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
	trWriteCMD( CMD_CLEARACCUM );
	trWritef( red );
	trWritef( green );
	trWritef( blue );
	trWritef( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->ClearAccum( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
	trWriteCMD( CMD_CLEARCOLOR );
	trWriteClampf( red );
	trWriteClampf( green );
	trWriteClampf( blue );
	trWriteClampf( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->ClearColor( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trClearDepth( GLclampd depth ) {
	trWriteCMD( CMD_CLEARDEPTH );
	trWriteClampd( depth );

	if( trCtx()->doExec ) {
		trGetDispatch()->ClearDepth( depth );
		trError();
	}
}


GLAPI void GLAPIENTRY trClear( GLbitfield mask ) {
	trWriteCMD( CMD_CLEAR );
	trWriteBits( mask );

	if( trCtx()->doExec ) {
		trGetDispatch()->Clear( mask );
		trError();
	}
}


GLAPI void GLAPIENTRY trClearIndex( GLfloat c ) {
	trWriteCMD( CMD_CLEARINDEX );
	trWritef( c );

	if( trCtx()->doExec ) {
		trGetDispatch()->ClearIndex( c );
		trError();
	}
}


GLAPI void GLAPIENTRY trClearStencil( GLint s ) {
	trWriteCMD( CMD_CLEARSTENCIL );
	trWritei( s );

	if( trCtx()->doExec ) {
		trGetDispatch()->ClearStencil( s );
		trError();
	}
}


GLAPI void GLAPIENTRY trClientActiveTextureARB( GLenum texture) {
	trWriteCMD( CMD_CLIENTACTIVETEXTUREARB );
	trWriteEnum( texture );

	if( trCtx()->doExec ) {
		trGetDispatch()->ClientActiveTextureARB( texture );
		trError();
	}
}


GLAPI void GLAPIENTRY trClipPlane( GLenum plane, const GLdouble *equation ) {
	trWriteCMD( CMD_CLIPPLANE );
	trWriteEnum( plane );
	trWritePointer( (void *)equation  );
	trFileFlush();
	trWriteArrayd( 4, equation );

	if( trCtx()->doExec ) {
		trGetDispatch()->ClipPlane( plane, equation  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3b( GLbyte red, GLbyte green, GLbyte blue ) {
	trWriteCMD( CMD_COLOR3B );
	trWriteb( red );
	trWriteb( green );
	trWriteb( blue );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3b( red, green, blue );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3bv( const GLbyte *v ) {
	trWriteCMD( CMD_COLOR3BV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayb( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3bv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3d( GLdouble red, GLdouble green, GLdouble blue ) {
	trWriteCMD( CMD_COLOR3D );
	trWrited( red );
	trWrited( green );
	trWrited( blue );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3d( red, green, blue );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3dv( const GLdouble *v ) {
	trWriteCMD( CMD_COLOR3DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3f( GLfloat red, GLfloat green, GLfloat blue ) {
	trWriteCMD( CMD_COLOR3F );
	trWritef( red );
	trWritef( green );
	trWritef( blue );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3f( red, green, blue );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3fv( const GLfloat *v ) {
	trWriteCMD( CMD_COLOR3FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3i( GLint red, GLint green, GLint blue ) {
	trWriteCMD( CMD_COLOR3I );
	trWritei( red );
	trWritei( green );
	trWritei( blue );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3i( red, green, blue );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3iv( const GLint *v ) {
	trWriteCMD( CMD_COLOR3IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3s( GLshort red, GLshort green, GLshort blue ) {
	trWriteCMD( CMD_COLOR3S );
	trWrites( red );
	trWrites( green );
	trWrites( blue );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3s( red, green, blue );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3sv( const GLshort *v ) {
	trWriteCMD( CMD_COLOR3SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3ub( GLubyte red, GLubyte green, GLubyte blue ) {
	trWriteCMD( CMD_COLOR3UB );
	trWriteub( red );
	trWriteub( green );
	trWriteub( blue );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3ub( red, green, blue );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3ubv( const GLubyte *v ) {
	trWriteCMD( CMD_COLOR3UBV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayub( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3ubv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3ui( GLuint red, GLuint green, GLuint blue ) {
	trWriteCMD( CMD_COLOR3UI );
	trWriteui( red );
	trWriteui( green );
	trWriteui( blue );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3ui( red, green, blue );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3uiv( const GLuint *v ) {
	trWriteCMD( CMD_COLOR3UIV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayui( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3uiv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3us( GLushort red, GLushort green, GLushort blue ) {
	trWriteCMD( CMD_COLOR3US );
	trWriteus( red );
	trWriteus( green );
	trWriteus( blue );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3us( red, green, blue );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor3usv( const GLushort *v ) {
	trWriteCMD( CMD_COLOR3USV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayus( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color3usv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha ) {
	trWriteCMD( CMD_COLOR4B );
	trWriteb( red );
	trWriteb( green );
	trWriteb( blue );
	trWriteb( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4b( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4bv( const GLbyte *v ) {
	trWriteCMD( CMD_COLOR4BV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayb( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4bv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha ) {
	trWriteCMD( CMD_COLOR4D );
	trWrited( red );
	trWrited( green );
	trWrited( blue );
	trWrited( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4d( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4dv( const GLdouble *v ) {
	trWriteCMD( CMD_COLOR4DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
	trWriteCMD( CMD_COLOR4F );
	trWritef( red );
	trWritef( green );
	trWritef( blue );
	trWritef( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4f( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4fv( const GLfloat *v ) {
	trWriteCMD( CMD_COLOR4FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4i( GLint red, GLint green, GLint blue, GLint alpha ) {
	trWriteCMD( CMD_COLOR4I );
	trWritei( red );
	trWritei( green );
	trWritei( blue );
	trWritei( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4i( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4iv( const GLint *v ) {
	trWriteCMD( CMD_COLOR4IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha ) {
	trWriteCMD( CMD_COLOR4S );
	trWrites( red );
	trWrites( green );
	trWrites( blue );
	trWrites( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4s( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4sv( const GLshort *v ) {
	trWriteCMD( CMD_COLOR4SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha ) {
	trWriteCMD( CMD_COLOR4UB );
	trWriteub( red );
	trWriteub( green );
	trWriteub( blue );
	trWriteub( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4ub( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4ubv( const GLubyte *v ) {
	trWriteCMD( CMD_COLOR4UBV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayub( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4ubv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha ) {
	trWriteCMD( CMD_COLOR4UI );
	trWriteui( red );
	trWriteui( green );
	trWriteui( blue );
	trWriteui( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4ui( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4uiv( const GLuint *v ) {
	trWriteCMD( CMD_COLOR4UIV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayui( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4uiv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha ) {
	trWriteCMD( CMD_COLOR4US );
	trWriteus( red );
	trWriteus( green );
	trWriteus( blue );
	trWriteus( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4us( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trColor4usv( const GLushort *v ) {
	trWriteCMD( CMD_COLOR4USV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayus( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Color4usv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ) {
	trWriteCMD( CMD_COLORMASK );
	trWriteBool( red );
	trWriteBool( green );
	trWriteBool( blue );
	trWriteBool( alpha );

	if( trCtx()->doExec ) {
		trGetDispatch()->ColorMask( red, green, blue, alpha );
		trError();
	}
}


GLAPI void GLAPIENTRY trColorMaterial( GLenum face, GLenum mode ) {
	trWriteCMD( CMD_COLORMATERIAL );
	trWriteEnum( face );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->ColorMaterial( face, mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trColorPointerEXT( GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr ) { /* TODO */
	trWriteCMD( CMD_COLORPOINTEREXT );
	trWritei( size );
	trWriteEnum( type );
	trWriteSizei( stride );
	trWriteSizei( count );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->ColorPointerEXT( size, type, stride, count, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
	trace_context_t * tctx;

	trWriteCMD( CMD_COLORPOINTER );
	trWritei( size );
	trWriteEnum( type );
	trWriteSizei( stride );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	if( tctx->doExec ) {
		trGetDispatch()->ColorPointer( size, type, stride, ptr  );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trColorSubTableEXT( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data ) {
	GLint pixelsize;

	trWriteCMD( CMD_COLORSUBTABLEEXT );
	trWriteEnum( target );
	trWriteSizei( start );
	trWriteSizei( count );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)data  );
	trFileFlush();

	pixelsize = trGetPixelSize( format, type );
	trWriteTypeArray( type, count, pixelsize, start, data );

	if( trCtx()->doExec ) {
		trGetDispatch()->ColorSubTableEXT( target, start, count, format, type, data  );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trColorSubTable( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data ) {
	GLint pixelsize;

	trWriteCMD( CMD_COLORSUBTABLE );
	trWriteEnum( target );
	trWriteSizei( start );
	trWriteSizei( count );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)data  );
	trFileFlush();

	pixelsize = trGetPixelSize( format, type );
	trWriteTypeArray( type, count, pixelsize, start, data );

	if( trCtx()->doExec ) {
		trGetDispatch()->ColorSubTable( target, start, count, format, type, data  );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trColorTableEXT( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table ) {
	GLint pixelsize;

	trWriteCMD( CMD_COLORTABLEEXT );
	trWriteEnum( target );
	trWriteEnum( internalformat );
	trWriteSizei( width );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)table  );
	trFileFlush();

	pixelsize = trGetPixelSize( format, type );
	trWriteTypeArray( type, width, pixelsize, 0, table );

	if( trCtx()->doExec ) {
		trGetDispatch()->ColorTableEXT( target, internalformat, width, format, type, table  );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trColorTable( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table ) { /* TODO */
	GLint pixelsize;

	trWriteCMD( CMD_COLORTABLE );
	trWriteEnum( target );
	trWriteEnum( internalformat );
	trWriteSizei( width );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)table  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->ColorTable( target, internalformat, width, format, type, table  );
		trError();
	}
}


GLAPI void GLAPIENTRY trColorTableParameterfv( GLenum target, GLenum pname, const GLfloat *params) {
	trWriteCMD( CMD_COLORTABLEPARAMETERFV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params );
	trFileFlush();
	trWriteArrayf( 4, params );

	if( trCtx()->doExec ) {
		trGetDispatch()->ColorTableParameterfv( target, pname, params );
		trError();
	}
}


GLAPI void GLAPIENTRY trColorTableParameteriv( GLenum target, GLenum pname, const GLint *params) {
	trWriteCMD( CMD_COLORTABLEPARAMETERIV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params );
	trFileFlush();
	trWriteArrayi( 4, params );

	if( trCtx()->doExec ) {
		trGetDispatch()->ColorTableParameteriv( target, pname, params );
		trError();
	}
}


GLAPI void GLAPIENTRY trConvolutionFilter1D( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image ) {
	GLint pixelsize;

	trWriteCMD( CMD_CONVOLUTIONFILTER1D );
	trWriteEnum( target );
	trWriteEnum( internalformat );
	trWriteSizei( width );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)image  );
	trFileFlush();

	pixelsize = trGetPixelSize( format, type );
	trWriteTypeArray( type, width, pixelsize, 0, image );

	if( trCtx()->doExec ) {
		trGetDispatch()->ConvolutionFilter1D( target, internalformat, width, format, type, image  );
		trError();
	}
}


GLAPI void GLAPIENTRY trConvolutionFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image ) {
	GLint i;
	GLint pixelsize;

	trWriteCMD( CMD_CONVOLUTIONFILTER2D );
	trWriteEnum( target );
	trWriteEnum( internalformat );
	trWriteSizei( width );
	trWriteSizei( height );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)image  );
	trFileFlush();

	/* p76 of GL spec says this isn't subject to unpack_row_width... */
	pixelsize = trGetPixelSize( format, type );
	for( i = 0; i < height; i++ ) {
		trWriteTypeArray( type, width, pixelsize, i * width, image );
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->ConvolutionFilter2D( target, internalformat, width, height, format, type, image  );
		trError();
	}
}


GLAPI void GLAPIENTRY trConvolutionParameterf( GLenum target, GLenum pname, GLfloat params ) {
	trWriteCMD( CMD_CONVOLUTIONPARAMETERF );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritef( params );

	if( trCtx()->doExec ) {
		trGetDispatch()->ConvolutionParameterf( target, pname, params );
		trError();
	}
}


GLAPI void GLAPIENTRY trConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
	trWriteCMD( CMD_CONVOLUTIONPARAMETERFV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_CONVOLUTION_BORDER_MODE:
			trWritef( params[0] );
			break;
		case GL_CONVOLUTION_BORDER_COLOR:
			trWriteArrayf( 4, params );
			break;
		default:
			/* The 2nd pass should catch this */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->ConvolutionParameterfv( target, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trConvolutionParameteri( GLenum target, GLenum pname, GLint params ) {
	trWriteCMD( CMD_CONVOLUTIONPARAMETERI );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritei( params );

	if( trCtx()->doExec ) {
		trGetDispatch()->ConvolutionParameteri( target, pname, params );
		trError();
	}
}


GLAPI void GLAPIENTRY trConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params ) {
	trWriteCMD( CMD_CONVOLUTIONPARAMETERIV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_CONVOLUTION_BORDER_MODE:
			trWritei( params[0] );
			break;
		case GL_CONVOLUTION_BORDER_COLOR:
			trWriteArrayi( 4, params );
			break;
		default:
			/* The 2nd pass should catch this */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->ConvolutionParameteriv( target, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trCopyColorSubTable( GLenum target, GLsizei start, GLint x, GLint y, GLsizei width ) {
	trWriteCMD( CMD_COPYCOLORSUBTABLE );
	trWriteEnum( target );
	trWriteSizei( start );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyColorSubTable( target, start, x, y, width );
		trError();
	}
}


GLAPI void GLAPIENTRY trCopyColorTable( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ) {
	trWriteCMD( CMD_COPYCOLORTABLE );
	trWriteEnum( target );
	trWriteEnum( internalformat );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyColorTable( target, internalformat, x, y, width );
		trError();
	}
}


GLAPI void GLAPIENTRY trCopyConvolutionFilter1D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ) {
	trWriteCMD( CMD_COPYCONVOLUTIONFILTER1D );
	trWriteEnum( target );
	trWriteEnum( internalformat );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyConvolutionFilter1D( target, internalformat, x, y, width );
		trError();
	}
}


GLAPI void GLAPIENTRY trCopyConvolutionFilter2D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height) {
	trWriteCMD( CMD_COPYCONVOLUTIONFILTER2D );
	trWriteEnum( target );
	trWriteEnum( internalformat );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );
	trWriteSizei( height );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyConvolutionFilter2D( target, internalformat, x, y, width, height );
		trError();
	}
}


GLAPI void GLAPIENTRY trCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type ) {
	trWriteCMD( CMD_COPYPIXELS );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );
	trWriteSizei( height );
	trWriteEnum( type );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyPixels( x, y, width, height, type );
		trError();
	}
}


GLAPI void GLAPIENTRY trCopyTexImage1D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border ) {
	trWriteCMD( CMD_COPYTEXIMAGE1D );
	trWriteEnum( target );
	trWritei( level );
	trWriteEnum( internalformat );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );
	trWritei( border );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyTexImage1D( target, level, internalformat, x, y, width, border );
		trError();
	}
}


GLAPI void GLAPIENTRY trCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border ) {
	trWriteCMD( CMD_COPYTEXIMAGE2D );
	trWriteEnum( target );
	trWritei( level );
	trWriteEnum( internalformat );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );
	trWriteSizei( height );
	trWritei( border );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyTexImage2D( target, level, internalformat, x, y, width, height, border );
		trError();
	}
}


GLAPI void GLAPIENTRY trCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width ) {
	trWriteCMD( CMD_COPYTEXSUBIMAGE1D );
	trWriteEnum( target );
	trWritei( level );
	trWritei( xoffset );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyTexSubImage1D( target, level, xoffset, x, y, width );
		trError();
	}
}


GLAPI void GLAPIENTRY trCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
	trWriteCMD( CMD_COPYTEXSUBIMAGE2D );
	trWriteEnum( target );
	trWritei( level );
	trWritei( xoffset );
	trWritei( yoffset );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );
	trWriteSizei( height );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trCopyTexSubImage3DEXT( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
	trWriteCMD( CMD_COPYTEXSUBIMAGE3DEXT );
	trWriteEnum( target );
	trWritei( level );
	trWritei( xoffset );
	trWritei( yoffset );
	trWritei( zoffset );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );
	trWriteSizei( height );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyTexSubImage3DEXT( target, level, xoffset, yoffset, zoffset, x, y, width, height );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trCopyTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
	trWriteCMD( CMD_COPYTEXSUBIMAGE3D );
	trWriteEnum( target );
	trWritei( level );
	trWritei( xoffset );
	trWritei( yoffset );
	trWritei( zoffset );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );
	trWriteSizei( height );

	if( trCtx()->doExec ) {
		trGetDispatch()->CopyTexSubImage3D( target, level, xoffset, yoffset, zoffset, x, y, width, height );
		trError();
	}
}


GLAPI void GLAPIENTRY trCullFace( GLenum mode ) {
	trWriteCMD( CMD_CULLFACE );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->CullFace( mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trDeleteLists( GLuint list, GLsizei range ) {
	trWriteCMD( CMD_DELETELISTS );
	trWriteui( list );
	trWriteSizei( range );

	if( trCtx()->doExec ) {
		trGetDispatch()->DeleteLists( list, range );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trDeleteTexturesEXT( GLsizei n, const GLuint *textures) {
	trWriteCMD( CMD_DELETETEXTURESEXT );
	trWriteSizei( n );
	trWritePointer( (void *)textures );
	trFileFlush();
	trWriteArrayui( n, textures );

	if( trCtx()->doExec ) {
		trGetDispatch()->DeleteTexturesEXT( n, textures );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trDeleteTextures( GLsizei n, const GLuint *textures) {
	trWriteCMD( CMD_DELETETEXTURES );
	trWriteSizei( n );
	trWritePointer( (void *)textures );
	trFileFlush();
	trWriteArrayui( n, textures );

	if( trCtx()->doExec ) {
		trGetDispatch()->DeleteTextures( n, textures );
		trError();
	}
}


GLAPI void GLAPIENTRY trDepthFunc( GLenum func ) {
	trWriteCMD( CMD_DEPTHFUNC );
	trWriteEnum( func );

	if( trCtx()->doExec ) {
		trGetDispatch()->DepthFunc( func );
		trError();
	}
}


GLAPI void GLAPIENTRY trDepthMask( GLboolean flag ) {
	trWriteCMD( CMD_DEPTHMASK );
	trWriteBool( flag );

	if( trCtx()->doExec ) {
		trGetDispatch()->DepthMask( flag );
		trError();
	}
}


GLAPI void GLAPIENTRY trDepthRange( GLclampd near_val, GLclampd far_val ) {
	trWriteCMD( CMD_DEPTHRANGE );
	trWriteClampd( near_val );
	trWriteClampd( far_val );

	if( trCtx()->doExec ) {
		trGetDispatch()->DepthRange( near_val, far_val );
		trError();
	}
}


GLAPI void GLAPIENTRY trDisableClientState( GLenum cap ) {
	trace_context_t * tctx;

	trWriteCMD( CMD_DISABLECLIENTSTATE );
	trWriteEnum( cap );

	tctx = trCtx();

	if( tctx->doExec ) {
		trGetDispatch()->DisableClientState( cap );
		trError();
	}
}


GLAPI void GLAPIENTRY trDisable( GLenum cap ) {
	trWriteCMD( CMD_DISABLE );
	trWriteEnum( cap );

	if( trCtx()->doExec ) {
		trGetDispatch()->Disable( cap );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trDrawArraysEXT( GLenum mode, GLint first, GLsizei count ) {
	trWriteCMD( CMD_DRAWARRAYSEXT );
	trWriteEnum( mode );
	trWritei( first );
	trWriteSizei( count );

	if( trCtx()->doExec ) {
		trGetDispatch()->DrawArraysEXT( mode, first, count );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trDrawArrays( GLenum mode, GLint first, GLsizei count ) {

	trWriteCMD( CMD_DRAWARRAYS );
	trWriteEnum( mode );
	trWritei( first );
	trWriteSizei( count );

	if( trCtx()->doExec ) {
		trGetDispatch()->DrawArrays( mode, first, count );
		trError();
	}
}


GLAPI void GLAPIENTRY trDrawBuffer( GLenum mode ) {
	trWriteCMD( CMD_DRAWBUFFER );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->DrawBuffer( mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices ) {
	trace_context_t * tctx;

	trWriteCMD( CMD_DRAWELEMENTS );
	trWriteEnum( mode );
	trWriteSizei( count );
	trWriteEnum( type );
	trWritePointer( (void *)indices  );
	trFileFlush();
	/* Why isn't the indices a GLint * ? */
	trWriteArrayi( count, (GLint *)indices );

	tctx = trCtx();

	if( tctx->doExec ) {
		trGetDispatch()->DrawElements( mode, count, type, indices  );
		trError();
	}
}


GLAPI void GLAPIENTRY trDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ) { /* TODO */
	GLint pixelsize;

	trWriteCMD( CMD_DRAWPIXELS );
	trWriteSizei( width );
	trWriteSizei( height );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->DrawPixels( width, height, format, type, pixels  );
		trError();
	}
}


GLAPI void GLAPIENTRY trDrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices ) {
	trace_context_t * tctx;

	trWriteCMD( CMD_DRAWRANGEELEMENTS );
	trWriteEnum( mode );
	trWriteui( start );
	trWriteui( end );
	trWriteSizei( count );
	trWriteEnum( type );
	trWritePointer( (void *)indices  );
	trFileFlush();
	switch( type ) {
		case GL_UNSIGNED_BYTE:
			trWriteArrayub( count, (GLubyte *)indices );
			break;
		case GL_UNSIGNED_SHORT:
			trWriteArrayus( count, (GLushort *)indices );
			break;
		case GL_UNSIGNED_INT:
			trWriteArrayui( count, (GLuint *)indices );
			break;
		default:
			/* The 2nd pass should catch this. */
			break;
	}

	tctx = trCtx();

	if( trCtx()->doExec ) {
		trGetDispatch()->DrawRangeElements( mode, start, end, count, type, indices  );
		trError();
	}
}


GLAPI void GLAPIENTRY trEdgeFlag( GLboolean flag ) {
	trWriteCMD( CMD_EDGEFLAG );
	trWriteBool( flag );

	if( trCtx()->doExec ) {
		trGetDispatch()->EdgeFlag( flag );
		trError();
	}
}


GLAPI void GLAPIENTRY trEdgeFlagPointerEXT( GLsizei stride, GLsizei count, const GLboolean *ptr ) { /* TODO */
	trWriteCMD( CMD_EDGEFLAGPOINTEREXT );
	trWriteSizei( stride );
	trWriteSizei( count );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->EdgeFlagPointerEXT( stride, count, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trEdgeFlagPointer( GLsizei stride, const GLvoid *ptr ) { /* TODO */
	trace_context_t * tctx;

	trWriteCMD( CMD_EDGEFLAGPOINTER );
	trWriteSizei( stride );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	tctx = trCtx();

	if( tctx->doExec ) {
		trGetDispatch()->EdgeFlagPointer( stride, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trEdgeFlagv( const GLboolean *flag ) {
	trWriteCMD( CMD_EDGEFLAGV );
	trWritePointer( (void *)flag  );
	trFileFlush();
	trWriteBool( flag[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->EdgeFlagv( flag  );
		trError();
	}
}


GLAPI void GLAPIENTRY trEnableClientState( GLenum cap ) {
	trace_context_t * tctx;

	trWriteCMD( CMD_ENABLECLIENTSTATE );
	trWriteEnum( cap );

	tctx = trCtx();

	if( tctx->doExec ) {
		trGetDispatch()->EnableClientState( cap );
		trError();
	}
}


GLAPI void GLAPIENTRY trEnable( GLenum cap ) {
	trWriteCMD( CMD_ENABLE );
	trWriteEnum( cap );

	if( trCtx()->doExec ) {
		trGetDispatch()->Enable( cap );
		trError();
	}
}


GLAPI void GLAPIENTRY trEndList( void ) {
	trWriteCMD( CMD_ENDLIST );

	if( trCtx()->doExec ) {
		trGetDispatch()->EndList(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trEnd( void ) {
	trWriteCMD( CMD_END );

	if( trCtx()->doExec ) {
		trGetDispatch()->End(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalCoord1d( GLdouble u ) {
	trWriteCMD( CMD_EVALCOORD1D );
	trWrited( u );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalCoord1d( u );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalCoord1dv( const GLdouble *u ) {
	trWriteCMD( CMD_EVALCOORD1DV );
	trWritePointer( (void *)u  );
	trFileFlush();
	trWrited( u[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalCoord1dv( u  );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalCoord1f( GLfloat u ) {
	trWriteCMD( CMD_EVALCOORD1F );
	trWritef( u );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalCoord1f( u );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalCoord1fv( const GLfloat *u ) {
	trWriteCMD( CMD_EVALCOORD1FV );
	trWritePointer( (void *)u  );
	trFileFlush();
	trWritef( u[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalCoord1fv( u  );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalCoord2d( GLdouble u, GLdouble v ) {
	trWriteCMD( CMD_EVALCOORD2D );
	trWrited( u );
	trWrited( v );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalCoord2d( u, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalCoord2dv( const GLdouble *u ) {
	trWriteCMD( CMD_EVALCOORD2DV );
	trWritePointer( (void *)u  );
	trFileFlush();
	trWriteArrayd( 2, u );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalCoord2dv( u  );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalCoord2f( GLfloat u, GLfloat v ) {
	trWriteCMD( CMD_EVALCOORD2F );
	trWritef( u );
	trWritef( v );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalCoord2f( u, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalCoord2fv( const GLfloat *u ) {
	trWriteCMD( CMD_EVALCOORD2FV );
	trWritePointer( (void *)u  );
	trFileFlush();
	trWriteArrayf( 2, u );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalCoord2fv( u  );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalMesh1( GLenum mode, GLint i1, GLint i2 ) {
	trWriteCMD( CMD_EVALMESH1 );
	trWriteEnum( mode );
	trWritei( i1 );
	trWritei( i2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalMesh1( mode, i1, i2 );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 ) {
	trWriteCMD( CMD_EVALMESH2 );
	trWriteEnum( mode );
	trWritei( i1 );
	trWritei( i2 );
	trWritei( j1 );
	trWritei( j2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalMesh2( mode, i1, i2, j1, j2 );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalPoint1( GLint i ) {
	trWriteCMD( CMD_EVALPOINT1 );
	trWritei( i );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalPoint1( i );
		trError();
	}
}


GLAPI void GLAPIENTRY trEvalPoint2( GLint i, GLint j ) {
	trWriteCMD( CMD_EVALPOINT2 );
	trWritei( i );
	trWritei( j );

	if( trCtx()->doExec ) {
		trGetDispatch()->EvalPoint2( i, j );
		trError();
	}
}


GLAPI void GLAPIENTRY trFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer ) {
	trWriteCMD( CMD_FEEDBACKBUFFER );
	trWriteSizei( size );
	trWriteEnum( type );
	trWritePointer( (void *)buffer  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->FeedbackBuffer( size, type, buffer  );
		trError();
	}
}


GLAPI void GLAPIENTRY trFinish( void ) {
	trWriteCMD( CMD_FINISH );

	if( trCtx()->doExec ) {
		trGetDispatch()->Finish(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trFlush( void ) {
	trWriteCMD( CMD_FLUSH );

	if( trCtx()->doExec ) {
		trGetDispatch()->Flush(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trFogf( GLenum pname, GLfloat param ) {
	trWriteCMD( CMD_FOGF );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->Fogf( pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trFogfv( GLenum pname, const GLfloat *params ) {
	trWriteCMD( CMD_FOGFV );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_FOG_MODE:
		case GL_FOG_DENSITY:
		case GL_FOG_START:	
		case GL_FOG_END:
		case GL_FOG_INDEX:
			trWritef( params[0] );
			break;

		case GL_FOG_COLOR:
			trWriteArrayf( 4, params );
			break;

		default:
			/* The 2nd pass should catch this */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->Fogfv( pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trFogi( GLenum pname, GLint param ) {
	trWriteCMD( CMD_FOGI );
	trWriteEnum( pname );
	trWritei( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->Fogi( pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trFogiv( GLenum pname, const GLint *params ) {
	trWriteCMD( CMD_FOGIV );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_FOG_MODE:
		case GL_FOG_DENSITY:
		case GL_FOG_START:	
		case GL_FOG_END:
		case GL_FOG_INDEX:
			trWritei( params[0] );
			break;

		case GL_FOG_COLOR:
			trWriteArrayi( 4, params );
			break;

		default:
			/* The 2nd pass should catch this */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->Fogiv( pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trFrontFace( GLenum mode ) {
	trWriteCMD( CMD_FRONTFACE );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->FrontFace( mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
	trWriteCMD( CMD_FRUSTUM );
	trWrited( left );
	trWrited( right );
	trWrited( bottom );
	trWrited( top );
	trWrited( near_val );
	trWrited( far_val );

	if( trCtx()->doExec ) {
		trGetDispatch()->Frustum( left, right, bottom, top, near_val, far_val );
		trError();
	}
}


GLAPI GLuint GLAPIENTRY trGenLists( GLsizei range ) {
	GLuint retval;

	trWriteCMD( CMD_GENLISTS );
	trWriteSizei( range );

	if( trCtx()->doExec ) {
		retval = trGetDispatch()->GenLists( range );
		trError();
	} else {
		retval = 0;
	}

	trWriteui( retval );
	return retval;
}


GLAPI void GLAPIENTRY trGenTexturesEXT( GLsizei n, GLuint *textures ) {
	trWriteCMD( CMD_GENTEXTURESEXT );
	trWriteSizei( n );
	trWritePointer( (void *)textures  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GenTexturesEXT( n, textures  );
		trError();
	}

	if( !(trCtx()->doExec) ) {
		memset( textures, 0, n * sizeof(GLuint) );
	}

	trWriteArrayui( n, textures );
}


GLAPI void GLAPIENTRY trGenTextures( GLsizei n, GLuint *textures ) {
	trWriteCMD( CMD_GENTEXTURES );
	trWriteSizei( n );
	trWritePointer( (void *)textures  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GenTextures( n, textures  );
		trError();
	}

	if( !(trCtx()->doExec) ) {
		memset( textures, 0, n * sizeof(GLuint) );
	}

	trWriteArrayui( n, textures );
}


GLAPI void GLAPIENTRY trGetBooleanv( GLenum pname, GLboolean *params ) { /* TODO */
	trWriteCMD( CMD_GETBOOLEANV );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

   switch( pname ) {
      case GL_COLOR_MATRIX:
      case GL_MODELVIEW_MATRIX:
      case GL_PROJECTION_MATRIX:
      case GL_TEXTURE_MATRIX:
         trWriteArrayBool( 16, params );
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
         trWriteArrayBool( 4, params );
         break;

      case GL_CURRENT_NORMAL:
         trWriteArrayBool( 3, params );
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
         trWriteArrayBool( 2, params );
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
         trWriteBool( params[0] );
         break;

      default:
         /* Bad enum.  What should we do? */
         break;
   }

	if( trCtx()->doExec ) {
		trGetDispatch()->GetBooleanv( pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetClipPlane( GLenum plane, GLdouble *equation ) {
	trWriteCMD( CMD_GETCLIPPLANE );
	trWriteEnum( plane );
	trWritePointer( (void *)equation  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetClipPlane( plane, equation  );
		trError();
	}

	if( !(trCtx()->doExec) ) {
		memset( equation, 0, sizeof(GLdouble) );
	}

	trWriteArrayd( 4, equation );
}


GLAPI void GLAPIENTRY trGetColorTableEXT( GLenum target, GLenum format, GLenum type, GLvoid *table ) { /* TODO */

	trWriteCMD( CMD_GETCOLORTABLEEXT );
	trWriteEnum( target );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)table  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetColorTableEXT( target, format, type, table  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table ) { /* TODO */
	trWriteCMD( CMD_GETCOLORTABLE );
	trWriteEnum( target );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)table  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetColorTable( target, format, type, table  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetColorTableParameterfvEXT( GLenum target, GLenum pname, GLfloat *params ) {
	trWriteCMD( CMD_GETCOLORTABLEPARAMETERFVEXT );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetColorTableParameterfvEXT( target, pname, params  );
		trError();
	} else {
		if( pname == GL_COLOR_TABLE_BIAS || pname == GL_COLOR_TABLE_SCALE ) {
			memset( params, 0, sizeof(GLfloat) * 4 );
		} else {
			params[0] = 0.0;
		}
	}

	if( pname == GL_COLOR_TABLE_BIAS || pname == GL_COLOR_TABLE_SCALE ) {
		trWriteArrayf( 4, params );
	} else {
		trWritef( params[0] );
	}
}


GLAPI void GLAPIENTRY trGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	trWriteCMD( CMD_GETCOLORTABLEPARAMETERFV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetColorTableParameterfv( target, pname, params  );
		trError();
	} else {
		if( pname == GL_COLOR_TABLE_BIAS || pname == GL_COLOR_TABLE_SCALE ) {
			memset( params, 0, sizeof(GLfloat) * 4 );
		} else {
			params[0] = 0.0;
		}
	}

	if( pname == GL_COLOR_TABLE_BIAS || pname == GL_COLOR_TABLE_SCALE ) {
		trWriteArrayf( 4, params );
	} else {
		trWritef( params[0] );
	}
}


GLAPI void GLAPIENTRY trGetColorTableParameterivEXT( GLenum target, GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETCOLORTABLEPARAMETERIVEXT );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetColorTableParameterivEXT( target, pname, params  );
		trError();
	} else {
		if( pname == GL_COLOR_TABLE_BIAS || pname == GL_COLOR_TABLE_SCALE ) {
			memset( params, 0, sizeof(GLint) * 4 );
		} else {
			params[0] = 0;
		}
	}

	if( pname == GL_COLOR_TABLE_BIAS || pname == GL_COLOR_TABLE_SCALE ) {
		trWriteArrayi( 4, params );
	} else {
		trWritei( params[0] );
	}
}


GLAPI void GLAPIENTRY trGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETCOLORTABLEPARAMETERIV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetColorTableParameteriv( target, pname, params  );
		trError();
	} else {
		if( pname == GL_COLOR_TABLE_BIAS || pname == GL_COLOR_TABLE_SCALE ) {
			memset( params, 0, sizeof(GLint) * 4 );
		} else {
			params[0] = 0;
		}
	}

	if( pname == GL_COLOR_TABLE_BIAS || pname == GL_COLOR_TABLE_SCALE ) {
		trWriteArrayi( 4, params );
	} else {
		trWritei( params[0] );
	}
}


GLAPI void GLAPIENTRY trGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image ) {
	trace_context_t * tctx;

	trWriteCMD( CMD_GETCONVOLUTIONFILTER );
	trWriteEnum( target );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)image  );
	trFileFlush();

	if( tctx->doExec ) {
		trGetDispatch()->GetConvolutionFilter( target, format, type, image  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	trWriteCMD( CMD_GETCONVOLUTIONPARAMETERFV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetConvolutionParameterfv( target, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_CONVOLUTION_FILTER_SCALE:
			case GL_CONVOLUTION_FILTER_BIAS:
				memset( params, 0, 4 * sizeof(GLfloat) );
				break;
			case GL_CONVOLUTION_BORDER_MODE:
			case GL_CONVOLUTION_BORDER_COLOR:
			case GL_CONVOLUTION_FORMAT:
			case GL_CONVOLUTION_WIDTH:
			case GL_CONVOLUTION_HEIGHT:
			case GL_MAX_CONVOLUTION_WIDTH:
			case GL_MAX_CONVOLUTION_HEIGHT:
				params[0] = 0;
				break;
			default:
				/* The 2nd pass should catch this */
				break;
		}
	}

	switch( pname ) {
		case GL_CONVOLUTION_FILTER_SCALE:
		case GL_CONVOLUTION_FILTER_BIAS:
			trWriteArrayf( 4, params );
			break;
		case GL_CONVOLUTION_BORDER_MODE:
		case GL_CONVOLUTION_BORDER_COLOR:
		case GL_CONVOLUTION_FORMAT:
		case GL_CONVOLUTION_WIDTH:
		case GL_CONVOLUTION_HEIGHT:
		case GL_MAX_CONVOLUTION_WIDTH:
		case GL_MAX_CONVOLUTION_HEIGHT:
			trWritef( params[0] );
			break;
		default:
			/* The 2nd pass should catch this */
			break;
	}
}


GLAPI void GLAPIENTRY trGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETCONVOLUTIONPARAMETERIV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetConvolutionParameteriv( target, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_CONVOLUTION_FILTER_SCALE:
			case GL_CONVOLUTION_FILTER_BIAS:
				memset( params, 0, 4 * sizeof(GLint) );
				break;
			case GL_CONVOLUTION_BORDER_MODE:
			case GL_CONVOLUTION_BORDER_COLOR:
			case GL_CONVOLUTION_FORMAT:
			case GL_CONVOLUTION_WIDTH:
			case GL_CONVOLUTION_HEIGHT:
			case GL_MAX_CONVOLUTION_WIDTH:
			case GL_MAX_CONVOLUTION_HEIGHT:
				params[0] = 0;
				break;
			default:
				/* The 2nd pass should catch this */
				break;
		}
	}

	switch( pname ) {
		case GL_CONVOLUTION_FILTER_SCALE:
		case GL_CONVOLUTION_FILTER_BIAS:
			trWriteArrayi( 4, params );
			break;
		case GL_CONVOLUTION_BORDER_MODE:
		case GL_CONVOLUTION_BORDER_COLOR:
		case GL_CONVOLUTION_FORMAT:
		case GL_CONVOLUTION_WIDTH:
		case GL_CONVOLUTION_HEIGHT:
		case GL_MAX_CONVOLUTION_WIDTH:
		case GL_MAX_CONVOLUTION_HEIGHT:
			trWritei( params[0] );
			break;
		default:
			/* The 2nd pass should catch this */
			break;
	}
}


GLAPI void GLAPIENTRY trGetDoublev( GLenum pname, GLdouble *params ) {
	trWriteCMD( CMD_GETDOUBLEV );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetDoublev( pname, params  );
		trError();
	} else {
		trZeroGetterData( pname, sizeof(GLdouble), params );
   }

   switch( pname ) {
      case GL_COLOR_MATRIX:
      case GL_MODELVIEW_MATRIX:
      case GL_PROJECTION_MATRIX:
      case GL_TEXTURE_MATRIX:
         trWriteArrayd( 16, params );
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
         trWriteArrayd( 4, params );
         break;

      case GL_CURRENT_NORMAL:
         trWriteArrayd( 3, params );
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
         trWriteArrayd( 2, params );
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
         trWrited( params[0] );
         break;

      default:
         /* Bad enum.  What should we do? */
         break;
   }

}


GLAPI GLenum GLAPIENTRY trGetError( void ) {
	GLenum retval;
	trWriteCMD( CMD_GETERROR );

	if( trCtx()->doExec ) {
		retval = trGetDispatch()->GetError(  );
		trError();
	} else {
		retval = GL_NO_ERROR;
	}

	trWriteEnum( retval );
	return retval;
}


GLAPI void GLAPIENTRY trGetFloatv( GLenum pname, GLfloat *params ) {
	trWriteCMD( CMD_GETFLOATV );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetFloatv( pname, params  );
		trError();
	} else {
		trZeroGetterData( pname, sizeof(GLfloat), params );
   }

   switch( pname ) {
      case GL_COLOR_MATRIX:
      case GL_MODELVIEW_MATRIX:
      case GL_PROJECTION_MATRIX:
      case GL_TEXTURE_MATRIX:
         trWriteArrayf( 16, params );
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
         trWriteArrayf( 4, params );
         break;

      case GL_CURRENT_NORMAL:
         trWriteArrayf( 3, params );
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
         trWriteArrayf( 2, params );
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
         trWritef( params[0] );
         break;

      default:
         /* Bad enum.  What should we do? */
         break;
   }
}


GLAPI void GLAPIENTRY trGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values ) { /* TODO */
	trWriteCMD( CMD_GETHISTOGRAM );
	trWriteEnum( target );
	trWriteBool( reset );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)values  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetHistogram( target, reset, format, type, values  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	trWriteCMD( CMD_GETHISTOGRAMPARAMETERFV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetHistogramParameterfv( target, pname, params  );
		trError();
	} else {
		params[0] = 0;
	}

	trWritef( params[0] );
}


GLAPI void GLAPIENTRY trGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETHISTOGRAMPARAMETERIV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetHistogramParameteriv( target, pname, params  );
		trError();
	} else {
		params[0] = 0;
	}

	trWritei( params[0] );
}


GLAPI void GLAPIENTRY trGetIntegerv( GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETINTEGERV );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetIntegerv( pname, params  );
		trError();
	} else {
		trZeroGetterData( pname, sizeof(GLint), params );
   }

   switch( pname ) {
      case GL_COLOR_MATRIX:
      case GL_MODELVIEW_MATRIX:
      case GL_PROJECTION_MATRIX:
      case GL_TEXTURE_MATRIX:
         trWriteArrayi( 16, params );
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
         trWriteArrayi( 4, params );
         break;

      case GL_CURRENT_NORMAL:
         trWriteArrayi( 3, params );
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
         trWriteArrayi( 2, params );
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
         trWritei( params[0] );
         break;

      default:
         /* Bad enum.  What should we do? */
         break;
   }
}


GLAPI void GLAPIENTRY trGetLightfv( GLenum light, GLenum pname, GLfloat *params ) {
	trWriteCMD( CMD_GETLIGHTFV );
	trWriteEnum( light );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetLightfv( light, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_AMBIENT:
			case GL_DIFFUSE:
			case GL_SPECULAR:
			case GL_POSITION:
				memset( params, 0, 4 * sizeof(GLfloat) );
				break;
			case GL_SPOT_DIRECTION:
				memset( params, 0, 3 * sizeof(GLfloat) );
				break;
			case GL_SPOT_EXPONENT:
			case GL_SPOT_CUTOFF:
			case GL_CONSTANT_ATTENUATION:
			case GL_LINEAR_ATTENUATION:
			case GL_QUADRATIC_ATTENUATION:	
				params[0] = 0;
				break;
			default:
				/* The 2nd pass should catch this */
				break;
		}
	}
	switch( pname ) {
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_SPECULAR:
		case GL_POSITION:
			trWriteArrayf( 4, params );
			break;
		case GL_SPOT_DIRECTION:
			trWriteArrayf( 3, params );
			break;
		case GL_SPOT_EXPONENT:
		case GL_SPOT_CUTOFF:
		case GL_CONSTANT_ATTENUATION:
		case GL_LINEAR_ATTENUATION:
		case GL_QUADRATIC_ATTENUATION:	
			trWritef( params[0] );
			break;
		default:
			/* The 2nd pass should catch this */
			break;
	}
}


GLAPI void GLAPIENTRY trGetLightiv( GLenum light, GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETLIGHTIV );
	trWriteEnum( light );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetLightiv( light, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_AMBIENT:
			case GL_DIFFUSE:
			case GL_SPECULAR:
			case GL_POSITION:
				memset( params, 0, 4 * sizeof(GLint) );
				break;
			case GL_SPOT_DIRECTION:
				memset( params, 0, 3 * sizeof(GLint) );
				break;
			case GL_SPOT_EXPONENT:
			case GL_SPOT_CUTOFF:
			case GL_CONSTANT_ATTENUATION:
			case GL_LINEAR_ATTENUATION:
			case GL_QUADRATIC_ATTENUATION:	
				params[0] = 0;
				break;
			default:
				/* The 2nd pass should catch this */
				break;
		}
	}
	switch( pname ) {
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_SPECULAR:
		case GL_POSITION:
			trWriteArrayi( 4, params );
			break;
		case GL_SPOT_DIRECTION:
			trWriteArrayi( 3, params );
			break;
		case GL_SPOT_EXPONENT:
		case GL_SPOT_CUTOFF:
		case GL_CONSTANT_ATTENUATION:
		case GL_LINEAR_ATTENUATION:
		case GL_QUADRATIC_ATTENUATION:	
			trWritei( params[0] );
			break;
		default:
			/* The 2nd pass should catch this */
			break;
	}
}


GLAPI void GLAPIENTRY trGetMapdv( GLenum target, GLenum query, GLdouble *v ) { /* TODO */
	trWriteCMD( CMD_GETMAPDV );
	trWriteEnum( target );
	trWriteEnum( query );
	trWritePointer( (void *)v  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetMapdv( target, query, v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetMapfv( GLenum target, GLenum query, GLfloat *v ) { /* TODO */
	trWriteCMD( CMD_GETMAPFV );
	trWriteEnum( target );
	trWriteEnum( query );
	trWritePointer( (void *)v  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetMapfv( target, query, v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetMapiv( GLenum target, GLenum query, GLint *v ) { /* TODO */
	trWriteCMD( CMD_GETMAPIV );
	trWriteEnum( target );
	trWriteEnum( query );
	trWritePointer( (void *)v  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetMapiv( target, query, v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetMaterialfv( GLenum face, GLenum pname, GLfloat *params ) {
	trWriteCMD( CMD_GETMATERIALFV );
	trWriteEnum( face );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetMaterialfv( face, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_AMBIENT:
			case GL_DIFFUSE:
			case GL_SPECULAR:
			case GL_EMISSION:
				memset( params, 0, 4 * sizeof(GLfloat) );
				break;
			case GL_COLOR_INDEXES:
				memset( params, 0, 3 * sizeof(GLfloat) );
				break;
			case GL_SHININESS:
				params[0] = 0;
				break;
			default:
				/* The 2nd pass will pick this up. */
				break;
		}
	}
	switch( pname ) {
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_SPECULAR:
		case GL_EMISSION:
			trWriteArrayf( 4, params );
			break;
		case GL_COLOR_INDEXES:
			trWriteArrayf( 3, params );
			break;
		case GL_SHININESS:
			trWritef( params[0] );
			break;
		default:
			/* The 2nd pass will pick this up. */
			break;
	}
}


GLAPI void GLAPIENTRY trGetMaterialiv( GLenum face, GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETMATERIALIV );
	trWriteEnum( face );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetMaterialiv( face, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_AMBIENT:
			case GL_DIFFUSE:
			case GL_SPECULAR:
			case GL_EMISSION:
				memset( params, 0, 4 * sizeof(GLint) );
				break;
			case GL_COLOR_INDEXES:
				memset( params, 0, 3 * sizeof(GLint) );
				break;
			case GL_SHININESS:
				params[0] = 0;
				break;
			default:
				/* The 2nd pass will pick this up. */
				break;
		}
	}
	switch( pname ) {
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_SPECULAR:
		case GL_EMISSION:
			trWriteArrayi( 4, params );
			break;
		case GL_COLOR_INDEXES:
			trWriteArrayi( 3, params );
			break;
		case GL_SHININESS:
			trWritei( params[0] );
			break;
		default:
			/* The 2nd pass will pick this up. */
			break;
	}
}


GLAPI void GLAPIENTRY trGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values ) {
	GLint pixelsize;

	trWriteCMD( CMD_GETMINMAX );
	trWriteEnum( target );
	trWriteBool( reset );
	trWriteEnum( format );
	trWriteEnum( types );
	trWritePointer( (void *)values  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetMinmax( target, reset, format, types, values  );
		trError();
	} else {
		switch( types ) {
			case GL_BYTE:
				((GLbyte *)values)[0] = 0;
				((GLbyte *)values)[1] = 0;
				break;

			case GL_UNSIGNED_BYTE:
			case GL_UNSIGNED_BYTE_3_3_2:
			case GL_UNSIGNED_BYTE_2_3_3_REV:
				((GLubyte *)values)[0] = 0;
				((GLubyte *)values)[1] = 0;
				break;

			case GL_SHORT:
				((GLshort *)values)[0] = 0;
				((GLshort *)values)[1] = 0;
				break;

			case GL_UNSIGNED_SHORT:
			case GL_UNSIGNED_SHORT_5_6_5:
			case GL_UNSIGNED_SHORT_5_6_5_REV:
			case GL_UNSIGNED_SHORT_4_4_4_4:
			case GL_UNSIGNED_SHORT_4_4_4_4_REV:
			case GL_UNSIGNED_SHORT_5_5_5_1:
			case GL_UNSIGNED_SHORT_1_5_5_5_REV:
				((GLshort *)values)[0] = 0;
				((GLshort *)values)[1] = 0;
				break;

			case GL_INT:
				((GLint *)values)[0] = 0;
				((GLint *)values)[1] = 0;
				break;

			case GL_UNSIGNED_INT:
			case GL_UNSIGNED_INT_8_8_8_8:
			case GL_UNSIGNED_INT_8_8_8_8_REV:
			case GL_UNSIGNED_INT_10_10_10_2:
			case GL_UNSIGNED_INT_2_10_10_10_REV:
				((GLuint *)values)[0] = 0;
				((GLuint *)values)[1] = 0;
				break;

			case GL_FLOAT:
				((GLfloat *)values)[0] = 0.0;
				((GLfloat *)values)[1] = 0.0;
				break;

			default:
				/* The 2nd pass should catch this. */
				break;
		}
	}

	pixelsize = trGetPixelSize( format, types );
	trWriteTypeArray( types, 2, pixelsize, 0, values );
}


GLAPI void GLAPIENTRY trGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params ) { /* TODO */
	trWriteCMD( CMD_GETMINMAXPARAMETERFV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetMinmaxParameterfv( target, pname, params  );
		trError();
	} else {
      params[0] = 0.0;
   }

   trWritef( params[0] );
}


GLAPI void GLAPIENTRY trGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params ) { /* TODO */
	trWriteCMD( CMD_GETMINMAXPARAMETERIV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetMinmaxParameteriv( target, pname, params  );
		trError();
	} else {
      params[0] = 0;
   }

   trWritei( params[0] );
}


GLAPI void GLAPIENTRY trGetPixelMapfv( GLenum map, GLfloat *values ) { /* TODO */
	trWriteCMD( CMD_GETPIXELMAPFV );
	trWriteEnum( map );
	trWritePointer( (void *)values  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetPixelMapfv( map, values  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetPixelMapuiv( GLenum map, GLuint *values ) { /* TODO */
	trWriteCMD( CMD_GETPIXELMAPUIV );
	trWriteEnum( map );
	trWritePointer( (void *)values  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetPixelMapuiv( map, values  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetPixelMapusv( GLenum map, GLushort *values ) { /* TODO */
	trWriteCMD( CMD_GETPIXELMAPUSV );
	trWriteEnum( map );
	trWritePointer( (void *)values  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetPixelMapusv( map, values  );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trGetPointervEXT( GLenum pname, void **params ) {
	trWriteCMD( CMD_GETPOINTERVEXT );
	trWriteEnum( pname );

	if( trCtx()->doExec ) {
		trGetDispatch()->GetPointervEXT( pname, params );
		trError();
	} else {
		*params = NULL;
	}
	trWritePointer( (void *)(*params) );
}
#endif


GLAPI void GLAPIENTRY trGetPointerv( GLenum pname, void **params ) {
	trWriteCMD( CMD_GETPOINTERV );
	trWriteEnum( pname );

	if( trCtx()->doExec ) {
		trGetDispatch()->GetPointerv( pname, params );
		trError();
	} else {
		*params = NULL;
	}
	trWritePointer( (void *)(*params) );
}


GLAPI void GLAPIENTRY trGetPolygonStipple( GLubyte *mask ) {
	GLint i;

	trWriteCMD( CMD_GETPOLYGONSTIPPLE );
	trWritePointer( (void *)mask  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetPolygonStipple( mask  );
		trError();
	} else {
		for( i = 0; i < 8 * 8; i++ ) {
			mask[i] = 0x0;
		}
	}
	for( i = 0; i < 8 * 8; i++ ) {
		trWriteub( mask[i] );
	}
}


GLAPI void GLAPIENTRY trGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span ) {

	trWriteCMD( CMD_GETSEPARABLEFILTER );
	trWriteEnum( target );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)row );
	trFileFlush();
	trWritePointer( (void *)column );
	trFileFlush();
	trWritePointer( (void *)span  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetSeparableFilter( target, format, type, row, column, span  );
		trError();
	}
}


GLAPI const GLubyte* GLAPIENTRY trGetString( GLenum name ) {
	const GLubyte * tmpstring;

	trWriteCMD( CMD_GETSTRING );
	trWriteEnum( name );

	if( trCtx()->doExec ) {
		tmpstring = trGetDispatch()->GetString( name );
		trError();
	} else {
		tmpstring = NULL;
	}

	trWriteString( (char *)tmpstring );
	return tmpstring;
}


GLAPI void GLAPIENTRY trGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params ) {
	trWriteCMD( CMD_GETTEXENVFV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetTexEnvfv( target, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_TEXTURE_ENV_MODE:
				params[0] = 0.0;
				break;
			case GL_TEXTURE_ENV_COLOR:
				memset( params, 0, 4 * sizeof(GLfloat) );
				break;
			default:
				/* The 2nd pass should pick this up. */
				break;
		}
	}
	switch( pname ) {
		case GL_TEXTURE_ENV_MODE:
			trWritef( params[0] );
			break;
		case GL_TEXTURE_ENV_COLOR:
			trWriteArrayf( 4, params );
			break;
		default:
			/* The 2nd pass should pick this up. */
			break;
	}
}


GLAPI void GLAPIENTRY trGetTexEnviv( GLenum target, GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETTEXENVIV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetTexEnviv( target, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_TEXTURE_ENV_MODE:
				params[0] = 0;
				break;
			case GL_TEXTURE_ENV_COLOR:
				memset( params, 0, 4 * sizeof(GLint) );
				break;
			default:
				/* The 2nd pass should pick this up. */
				break;
		}
	}
	switch( pname ) {
		case GL_TEXTURE_ENV_MODE:
			trWritei( params[0] );
			break;
		case GL_TEXTURE_ENV_COLOR:
			trWriteArrayi( 4, params );
			break;
		default:
			/* The 2nd pass should pick this up. */
			break;
	}
}


GLAPI void GLAPIENTRY trGetTexGendv( GLenum coord, GLenum pname, GLdouble *params ) {
	trWriteCMD( CMD_GETTEXGENDV );
	trWriteEnum( coord );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetTexGendv( coord, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_TEXTURE_GEN_MODE:
				params[0] = 0.0;
				break;
			case GL_OBJECT_PLANE:
			case GL_EYE_PLANE:
				memset( params, 0, 4 * sizeof(GLdouble) );
				break;
			default:
				/* The 2nd pass should pick this up. */
				break;
		}
	}
	switch( pname ) {
		case GL_TEXTURE_GEN_MODE:
			trWrited( params[0] );
			break;
		case GL_OBJECT_PLANE:
		case GL_EYE_PLANE:
			trWriteArrayd( 4, params );
			break;
		default:
			/* The 2nd pass should pick this up. */
			break;
	}
}


GLAPI void GLAPIENTRY trGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params ) {
	trWriteCMD( CMD_GETTEXGENFV );
	trWriteEnum( coord );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetTexGenfv( coord, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_TEXTURE_GEN_MODE:
				params[0] = 0.0;
				break;
			case GL_OBJECT_PLANE:
			case GL_EYE_PLANE:
				memset( params, 0, 4 * sizeof(GLfloat) );
				break;
			default:
				/* The 2nd pass should pick this up. */
				break;
		}
	}
	switch( pname ) {
		case GL_TEXTURE_GEN_MODE:
			trWritef( params[0] );
			break;
		case GL_OBJECT_PLANE:
		case GL_EYE_PLANE:
			trWriteArrayf( 4, params );
			break;
		default:
			/* The 2nd pass should pick this up. */
			break;
	}
}


GLAPI void GLAPIENTRY trGetTexGeniv( GLenum coord, GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETTEXGENIV );
	trWriteEnum( coord );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetTexGeniv( coord, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_TEXTURE_GEN_MODE:
				params[0] = 0;
				break;
			case GL_OBJECT_PLANE:
			case GL_EYE_PLANE:
				memset( params, 0, 4 * sizeof(GLint) );
				break;
			default:
				/* The 2nd pass should pick this up. */
				break;
		}
	}
	switch( pname ) {
		case GL_TEXTURE_GEN_MODE:
			trWritei( params[0] );
			break;
		case GL_OBJECT_PLANE:
		case GL_EYE_PLANE:
			trWriteArrayi( 4, params );
			break;
		default:
			/* The 2nd pass should pick this up. */
			break;
	}
}


GLAPI void GLAPIENTRY trGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels ) { /* TODO */

	trWriteCMD( CMD_GETTEXIMAGE );
	trWriteEnum( target );
	trWritei( level );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetTexImage( target, level, format, type, pixels  );
		trError();
	}
}


GLAPI void GLAPIENTRY trGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params ) {
	trWriteCMD( CMD_GETTEXLEVELPARAMETERFV );
	trWriteEnum( target );
	trWritei( level );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetTexLevelParameterfv( target, level, pname, params  );
		trError();
	} else {
		params[0] = 0.0;
	}
	trWritef( params[0] );
}


GLAPI void GLAPIENTRY trGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETTEXLEVELPARAMETERIV );
	trWriteEnum( target );
	trWritei( level );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetTexLevelParameteriv( target, level, pname, params  );
		trError();
	} else {
		params[0] = 0;
	}
	trWritei( params[0] );
}


GLAPI void GLAPIENTRY trGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params) {
	trWriteCMD( CMD_GETTEXPARAMETERFV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetTexParameterfv( target, pname, params );
		trError();
	} else {
		switch( pname ) {
			case GL_TEXTURE_MAG_FILTER:
			case GL_TEXTURE_MIN_FILTER:
			case GL_TEXTURE_MIN_LOD:
			case GL_TEXTURE_MAX_LOD:
			case GL_TEXTURE_BASE_LEVEL:
			case GL_TEXTURE_MAX_LEVEL:
			case GL_TEXTURE_WRAP_S:
			case GL_TEXTURE_WRAP_T:
			case GL_TEXTURE_WRAP_R:
			case GL_TEXTURE_PRIORITY:
			case GL_TEXTURE_RESIDENT:
				params[0] = 0.0;
				break;
			case GL_TEXTURE_BORDER_COLOR:
				memset( params, 0, 4 * sizeof(GLfloat) );
			default:
				/* The 2nd pass should pick this up. */
				break;
		}
	}
	switch( pname ) {
		case GL_TEXTURE_MAG_FILTER:
		case GL_TEXTURE_MIN_FILTER:
		case GL_TEXTURE_MIN_LOD:
		case GL_TEXTURE_MAX_LOD:
		case GL_TEXTURE_BASE_LEVEL:
		case GL_TEXTURE_MAX_LEVEL:
		case GL_TEXTURE_WRAP_S:
		case GL_TEXTURE_WRAP_T:
		case GL_TEXTURE_WRAP_R:
		case GL_TEXTURE_PRIORITY:
		case GL_TEXTURE_RESIDENT:
			trWritef( params[0] );
			break;
		case GL_TEXTURE_BORDER_COLOR:
			trWriteArrayf( 4, params );
			break;
		default:
			/* The 2nd pass should pick this up. */
			break;
	}
}


GLAPI void GLAPIENTRY trGetTexParameteriv( GLenum target, GLenum pname, GLint *params ) {
	trWriteCMD( CMD_GETTEXPARAMETERIV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->GetTexParameteriv( target, pname, params  );
		trError();
	} else {
		switch( pname ) {
			case GL_TEXTURE_MAG_FILTER:
			case GL_TEXTURE_MIN_FILTER:
			case GL_TEXTURE_MIN_LOD:
			case GL_TEXTURE_MAX_LOD:
			case GL_TEXTURE_BASE_LEVEL:
			case GL_TEXTURE_MAX_LEVEL:
			case GL_TEXTURE_WRAP_S:
			case GL_TEXTURE_WRAP_T:
			case GL_TEXTURE_WRAP_R:
			case GL_TEXTURE_PRIORITY:
			case GL_TEXTURE_RESIDENT:
				params[0] = 0;
				break;
			case GL_TEXTURE_BORDER_COLOR:
				memset( params, 0, 4 * sizeof(GLint) );
			default:
				/* The 2nd pass should pick this up. */
				break;
		}
	}
	switch( pname ) {
		case GL_TEXTURE_MAG_FILTER:
		case GL_TEXTURE_MIN_FILTER:
		case GL_TEXTURE_MIN_LOD:
		case GL_TEXTURE_MAX_LOD:
		case GL_TEXTURE_BASE_LEVEL:
		case GL_TEXTURE_MAX_LEVEL:
		case GL_TEXTURE_WRAP_S:
		case GL_TEXTURE_WRAP_T:
		case GL_TEXTURE_WRAP_R:
		case GL_TEXTURE_PRIORITY:
		case GL_TEXTURE_RESIDENT:
			trWritei( params[0] );
			break;
		case GL_TEXTURE_BORDER_COLOR:
			trWriteArrayi( 4, params );
			break;
		default:
			/* The 2nd pass should pick this up. */
			break;
	}
}


GLAPI void GLAPIENTRY trHint( GLenum target, GLenum mode ) {
	trWriteCMD( CMD_HINT );
	trWriteEnum( target );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->Hint( target, mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trHistogram( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink ) {
	trWriteCMD( CMD_HISTOGRAM );
	trWriteEnum( target );
	trWriteSizei( width );
	trWriteEnum( internalformat );
	trWriteBool( sink );

	if( trCtx()->doExec ) {
		trGetDispatch()->Histogram( target, width, internalformat, sink );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexd( GLdouble c ) {
	trWriteCMD( CMD_INDEXD );
	trWrited( c );

	if( trCtx()->doExec ) {
		trGetDispatch()->Indexd( c );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexdv( const GLdouble *c ) {
	trWriteCMD( CMD_INDEXDV );
	trWritePointer( (void *)c  );
	trFileFlush();
	trWrited( c[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->Indexdv( c  );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexf( GLfloat c ) {
	trWriteCMD( CMD_INDEXF );
	trWritef( c );

	if( trCtx()->doExec ) {
		trGetDispatch()->Indexf( c );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexfv( const GLfloat *c ) {
	trWriteCMD( CMD_INDEXFV );
	trWritePointer( (void *)c  );
	trFileFlush();
	trWritef( c[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->Indexfv( c  );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexi( GLint c ) {
	trWriteCMD( CMD_INDEXI );
	trWritei( c );

	if( trCtx()->doExec ) {
		trGetDispatch()->Indexi( c );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexiv( const GLint *c ) {
	trWriteCMD( CMD_INDEXIV );
	trWritePointer( (void *)c  );
	trFileFlush();
	trWritei( c[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->Indexiv( c  );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexMask( GLuint mask ) {
	trWriteCMD( CMD_INDEXMASK );
	trWriteui( mask );

	if( trCtx()->doExec ) {
		trGetDispatch()->IndexMask( mask );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexPointerEXT( GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr ) { /* TODO */
	trWriteCMD( CMD_INDEXPOINTEREXT );
	trWriteEnum( type );
	trWriteSizei( stride );
	trWriteSizei( count );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->IndexPointerEXT( type, stride, count, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexPointer( GLenum type, GLsizei stride, const GLvoid *ptr ) { /* TODO */
	trace_context_t * tctx;

	trWriteCMD( CMD_INDEXPOINTER );
	trWriteEnum( type );
	trWriteSizei( stride );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	tctx = trCtx();

	if( tctx->doExec ) {
		trGetDispatch()->IndexPointer( type, stride, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexs( GLshort c ) {
	trWriteCMD( CMD_INDEXS );
	trWrites( c );

	if( trCtx()->doExec ) {
		trGetDispatch()->Indexs( c );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexsv( const GLshort *c ) {
	trWriteCMD( CMD_INDEXSV );
	trWritePointer( (void *)c  );
	trFileFlush();
	trWrites( c[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->Indexsv( c  );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexub( GLubyte c ) {
	trWriteCMD( CMD_INDEXUB );
	trWriteub( c );

	if( trCtx()->doExec ) {
		trGetDispatch()->Indexub( c );
		trError();
	}
}


GLAPI void GLAPIENTRY trIndexubv( const GLubyte *c ) {
	trWriteCMD( CMD_INDEXUBV );
	trWritePointer( (void *)c  );
	trFileFlush();
	trWriteub( c[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->Indexubv( c  );
		trError();
	}
}


GLAPI void GLAPIENTRY trInitNames( void ) {
	trWriteCMD( CMD_INITNAMES );

	if( trCtx()->doExec ) {
		trGetDispatch()->InitNames(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer ) { /* TODO */
	trace_context_t * tctx = trCtx();

	trWriteCMD( CMD_INTERLEAVEDARRAYS );
	trWriteEnum( format );
	trWriteSizei( stride );
	trWritePointer( (void *)pointer  );
	trFileFlush();

	if( tctx->doExec ) {
		trSetOriginalDispatch();
		trGetDispatch()->InterleavedArrays( format, stride, pointer  );
		trSetTraceDispatch();
		trError();
	}
}


GLAPI GLboolean GLAPIENTRY trIsEnabled( GLenum cap ) {
	GLboolean retval;

	trWriteCMD( CMD_ISENABLED );
	trWriteEnum( cap );

	if( trCtx()->doExec ) {
		retval = trGetDispatch()->IsEnabled( cap );
		trError();
	} else {
		retval = GL_FALSE;
	}
	trWriteBool( retval );
	return retval;
}


GLAPI GLboolean GLAPIENTRY trIsList( GLuint list ) {
	GLboolean retval;

	trWriteCMD( CMD_ISLIST );
	trWriteui( list );

	if( trCtx()->doExec ) {
		retval = trGetDispatch()->IsList( list );
		trError();
	} else {
		retval = GL_FALSE;
	}
	trWriteBool( retval );
	return retval;
}


GLAPI GLboolean GLAPIENTRY trIsTexture( GLuint texture ) {
	GLboolean retval;

	trWriteCMD( CMD_ISTEXTURE );
	trWriteui( texture );

	if( trCtx()->doExec ) {
		retval = trGetDispatch()->IsTexture( texture );
		trError();
	} else {
		retval = GL_FALSE;
	}
	trWriteBool( retval );
	return retval;
}


GLAPI GLboolean GLAPIENTRY trIsTextureEXT( GLuint texture ) {
	GLboolean retval;

	trWriteCMD( CMD_ISTEXTUREEXT );
	trWriteui( texture );

	if( trCtx()->doExec ) {
		retval = trGetDispatch()->IsTextureEXT( texture );
		trError();
	} else {
		retval = GL_FALSE;
	}
	trWriteBool( retval );
	return retval;
}


GLAPI void GLAPIENTRY trLightf( GLenum light, GLenum pname, GLfloat param ) {
	trWriteCMD( CMD_LIGHTF );
	trWriteEnum( light );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->Lightf( light, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trLightfv( GLenum light, GLenum pname, const GLfloat *params ) {
	trWriteCMD( CMD_LIGHTFV );
	trWriteEnum( light );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_SPECULAR:
		case GL_POSITION:
			trWriteArrayf( 4, params );
			break;
		case GL_SPOT_DIRECTION:
			trWriteArrayf( 3, params );
			break;
		case GL_SPOT_EXPONENT:
		case GL_SPOT_CUTOFF:
		case GL_CONSTANT_ATTENUATION:
		case GL_LINEAR_ATTENUATION:
		case GL_QUADRATIC_ATTENUATION:
			trWritef( params[0] );
			break;
		default:
			/* The 2nd pass should pick this up. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->Lightfv( light, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trLighti( GLenum light, GLenum pname, GLint param ) {
	trWriteCMD( CMD_LIGHTI );
	trWriteEnum( light );
	trWriteEnum( pname );
	trWritei( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->Lighti( light, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trLightiv( GLenum light, GLenum pname, const GLint *params ) {
	trWriteCMD( CMD_LIGHTIV );
	trWriteEnum( light );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_SPECULAR:
		case GL_POSITION:
			trWriteArrayi( 4, params );
			break;
		case GL_SPOT_DIRECTION:
			trWriteArrayi( 3, params );
			break;
		case GL_SPOT_EXPONENT:
		case GL_SPOT_CUTOFF:
		case GL_CONSTANT_ATTENUATION:
		case GL_LINEAR_ATTENUATION:
		case GL_QUADRATIC_ATTENUATION:
			trWritei( params[0] );
			break;
		default:
			/* The 2nd pass should pick this up. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->Lightiv( light, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trLightModelf( GLenum pname, GLfloat param ) {
	trWriteCMD( CMD_LIGHTMODELF );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->LightModelf( pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trLightModelfv( GLenum pname, const GLfloat *params ) {
	trWriteCMD( CMD_LIGHTMODELFV );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_LIGHT_MODEL_AMBIENT:
			trWriteArrayf( 4, params );
			break;
		case GL_LIGHT_MODEL_COLOR_CONTROL:
		case GL_LIGHT_MODEL_LOCAL_VIEWER:
		case GL_LIGHT_MODEL_TWO_SIDE:
			trWritef( params[0] );
			break;
		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->LightModelfv( pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trLightModeli( GLenum pname, GLint param ) {
	trWriteCMD( CMD_LIGHTMODELI );
	trWriteEnum( pname );
	trWritei( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->LightModeli( pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trLightModeliv( GLenum pname, const GLint *params ) {
	trWriteCMD( CMD_LIGHTMODELIV );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_LIGHT_MODEL_AMBIENT:
			trWriteArrayi( 4, params );
			break;
		case GL_LIGHT_MODEL_COLOR_CONTROL:
		case GL_LIGHT_MODEL_LOCAL_VIEWER:
		case GL_LIGHT_MODEL_TWO_SIDE:
			trWritei( params[0] );
			break;
		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->LightModeliv( pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trLineStipple( GLint factor, GLushort pattern ) {
	trWriteCMD( CMD_LINESTIPPLE );
	trWritei( factor );
	trWriteus( pattern );

	if( trCtx()->doExec ) {
		trGetDispatch()->LineStipple( factor, pattern );
		trError();
	}
}


GLAPI void GLAPIENTRY trLineWidth( GLfloat width ) {
	trWriteCMD( CMD_LINEWIDTH );
	trWritef( width );

	if( trCtx()->doExec ) {
		trGetDispatch()->LineWidth( width );
		trError();
	}
}


GLAPI void GLAPIENTRY trListBase( GLuint base ) {
	trWriteCMD( CMD_LISTBASE );
	trWriteui( base );

	if( trCtx()->doExec ) {
		trGetDispatch()->ListBase( base );
		trError();
	}
}


GLAPI void GLAPIENTRY trLoadIdentity( void ) {
	trWriteCMD( CMD_LOADIDENTITY );

	if( trCtx()->doExec ) {
		trGetDispatch()->LoadIdentity(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trLoadMatrixd( const GLdouble *m ) {
	trWriteCMD( CMD_LOADMATRIXD );
	trWritePointer( (void *)m  );
	trFileFlush();
	trWriteArrayd( 16, m );

	if( trCtx()->doExec ) {
		trGetDispatch()->LoadMatrixd( m  );
		trError();
	}
}


GLAPI void GLAPIENTRY trLoadMatrixf( const GLfloat *m ) {
	trWriteCMD( CMD_LOADMATRIXF );
	trWritePointer( (void *)m  );
	trFileFlush();
	trWriteArrayf( 16, m );

	if( trCtx()->doExec ) {
		trGetDispatch()->LoadMatrixf( m  );
		trError();
	}
}


GLAPI void GLAPIENTRY trLoadName( GLuint name ) {
	trWriteCMD( CMD_LOADNAME );
	trWriteui( name );

	if( trCtx()->doExec ) {
		trGetDispatch()->LoadName( name );
		trError();
	}
}


GLAPI void GLAPIENTRY trLockArraysEXT( GLint first, GLsizei count ) {
	trWriteCMD( CMD_LOCKARRAYSEXT );
	trWritei( first );
	trWriteSizei( count );

	if( trCtx()->doExec ) {
		trGetDispatch()->LockArraysEXT( first, count );
		trError();
	}
}


GLAPI void GLAPIENTRY trLogicOp( GLenum opcode ) {
	trWriteCMD( CMD_LOGICOP );
	trWriteEnum( opcode );

	if( trCtx()->doExec ) {
		trGetDispatch()->LogicOp( opcode );
		trError();
	}
}


GLAPI void GLAPIENTRY trMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points ) {
	GLint i;

	trWriteCMD( CMD_MAP1D );
	trWriteEnum( target );
	trWrited( u1 );
	trWrited( u2 );
	trWritei( stride );
	trWritei( order );
	trWritePointer( (void *)points  );
	trFileFlush();

	switch( target ) {
		case GL_MAP1_INDEX:
		case GL_MAP1_TEXTURE_COORD_1:
			for( i = 0; i < stride * order; i += stride ) {
				trWrited( points[i] );
			}
			break;
		case GL_MAP1_TEXTURE_COORD_2:
			for( i = 0; i < stride * order; i += stride ) {
				trWrited( points[i] );
				trWrited( points[i + 1] );
			}
			break;
		case GL_MAP1_VERTEX_3:
		case GL_MAP1_NORMAL:
		case GL_MAP1_TEXTURE_COORD_3:
			for( i = 0; i < stride * order; i += stride ) {
				trWrited( points[i] );
				trWrited( points[i + 1] );
				trWrited( points[i + 2] );
			}
			break;
		case GL_MAP1_VERTEX_4:
		case GL_MAP1_TEXTURE_COORD_4:
			for( i = 0; i < stride * order; i += stride ) {
				trWrited( points[i] );
				trWrited( points[i + 1] );
				trWrited( points[i + 2] );
				trWrited( points[i + 3] );
			}
			break;
		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->Map1d( target, u1, u2, stride, order, points  );
		trError();
	}
}


GLAPI void GLAPIENTRY trMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points ) {
	GLint i;

	trWriteCMD( CMD_MAP1F );
	trWriteEnum( target );
	trWritef( u1 );
	trWritef( u2 );
	trWritei( stride );
	trWritei( order );
	trWritePointer( (void *)points  );
	trFileFlush();

	switch( target ) {
		case GL_MAP1_INDEX:
		case GL_MAP1_TEXTURE_COORD_1:
			for( i = 0; i < stride * order; i += stride ) {
				trWritef( points[i] );
			}
			break;
		case GL_MAP1_TEXTURE_COORD_2:
			for( i = 0; i < stride * order; i += stride ) {
				trWritef( points[i] );
				trWritef( points[i + 1] );
			}
			break;
		case GL_MAP1_VERTEX_3:
		case GL_MAP1_NORMAL:
		case GL_MAP1_TEXTURE_COORD_3:
			for( i = 0; i < stride * order; i += stride ) {
				trWritef( points[i] );
				trWritef( points[i + 1] );
				trWritef( points[i + 2] );
			}
			break;
		case GL_MAP1_VERTEX_4:
		case GL_MAP1_TEXTURE_COORD_4:
			for( i = 0; i < stride * order; i += stride ) {
				trWritef( points[i] );
				trWritef( points[i + 1] );
				trWritef( points[i + 2] );
				trWritef( points[i + 3] );
			}
			break;
		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->Map1f( target, u1, u2, stride, order, points  );
		trError();
	}
}


GLAPI void GLAPIENTRY trMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points ) {
	GLint i;
	GLint j;

	trWriteCMD( CMD_MAP2D );
	trWriteEnum( target );
	trWrited( u1 );
	trWrited( u2 );
	trWritei( ustride );
	trWritei( uorder );
	trWrited( v1 );
	trWrited( v2 );
	trWritei( vstride );
	trWritei( vorder );
	trWritePointer( (void *)points  );
	trFileFlush();

	switch( target ) {
		case GL_MAP1_INDEX:
		case GL_MAP1_TEXTURE_COORD_1:
			for( j = 0; j < vstride * vorder; j += vstride ) {
				for( i = 0; i < ustride * uorder; i += ustride ) {
					trWrited( points[i + j] );
				}
			}
			break;
		case GL_MAP1_TEXTURE_COORD_2:
			for( j = 0; j < vstride * vorder; j += vstride ) {
				for( i = 0; i < ustride * uorder; i += ustride ) {
					trWrited( points[i + j] );
					trWrited( points[i + j + 1] );
				}
			}
			break;
		case GL_MAP1_VERTEX_3:
		case GL_MAP1_NORMAL:
		case GL_MAP1_TEXTURE_COORD_3:
			for( j = 0; j < vstride * vorder; j += vstride ) {
				for( i = 0; i < ustride * uorder; i += ustride ) {
					trWrited( points[i + j] );
					trWrited( points[i + j + 1] );
					trWrited( points[i + j + 2] );
				}
			}
			break;
		case GL_MAP1_VERTEX_4:
		case GL_MAP1_TEXTURE_COORD_4:
			for( j = 0; j < vstride * vorder; j += vstride ) {
				for( i = 0; i < ustride * uorder; i += ustride ) {
					trWrited( points[i + j] );
					trWrited( points[i + j + 1] );
					trWrited( points[i + j + 2] );
					trWrited( points[i + j + 3] );
				}
			}
			break;
		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->Map2d( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points  );
		trError();
	}
}


GLAPI void GLAPIENTRY trMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points ) {
	GLint i;
	GLint j;

	trWriteCMD( CMD_MAP2F );
	trWriteEnum( target );
	trWritef( u1 );
	trWritef( u2 );
	trWritei( ustride );
	trWritei( uorder );
	trWritef( v1 );
	trWritef( v2 );
	trWritei( vstride );
	trWritei( vorder );
	trWritePointer( (void *)points  );
	trFileFlush();

	switch( target ) {
		case GL_MAP1_INDEX:
		case GL_MAP1_TEXTURE_COORD_1:
			for( j = 0; j < vstride * vorder; j += vstride ) {
				for( i = 0; i < ustride * uorder; i += ustride ) {
					trWritef( points[i + j] );
				}
			}
			break;
		case GL_MAP1_TEXTURE_COORD_2:
			for( j = 0; j < vstride * vorder; j += vstride ) {
				for( i = 0; i < ustride * uorder; i += ustride ) {
					trWritef( points[i + j] );
					trWritef( points[i + j + 1] );
				}
			}
			break;
		case GL_MAP1_VERTEX_3:
		case GL_MAP1_NORMAL:
		case GL_MAP1_TEXTURE_COORD_3:
			for( j = 0; j < vstride * vorder; j += vstride ) {
				for( i = 0; i < ustride * uorder; i += ustride ) {
					trWritef( points[i + j] );
					trWritef( points[i + j + 1] );
					trWritef( points[i + j + 2] );
				}
			}
			break;
		case GL_MAP1_VERTEX_4:
		case GL_MAP1_TEXTURE_COORD_4:
			for( j = 0; j < vstride * vorder; j += vstride ) {
				for( i = 0; i < ustride * uorder; i += ustride ) {
					trWritef( points[i + j] );
					trWritef( points[i + j + 1] );
					trWritef( points[i + j + 2] );
					trWritef( points[i + j + 3] );
				}
			}
			break;
		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->Map2f( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points  );
		trError();
	}
}


GLAPI void GLAPIENTRY trMapGrid1d( GLint un, GLdouble u1, GLdouble u2 ) {
	trWriteCMD( CMD_MAPGRID1D );
	trWritei( un );
	trWrited( u1 );
	trWrited( u2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->MapGrid1d( un, u1, u2 );
		trError();
	}
}


GLAPI void GLAPIENTRY trMapGrid1f( GLint un, GLfloat u1, GLfloat u2 ) {
	trWriteCMD( CMD_MAPGRID1F );
	trWritei( un );
	trWritef( u1 );
	trWritef( u2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->MapGrid1f( un, u1, u2 );
		trError();
	}
}


GLAPI void GLAPIENTRY trMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 ) {
	trWriteCMD( CMD_MAPGRID2D );
	trWritei( un );
	trWrited( u1 );
	trWrited( u2 );
	trWritei( vn );
	trWrited( v1 );
	trWrited( v2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->MapGrid2d( un, u1, u2, vn, v1, v2 );
		trError();
	}
}


GLAPI void GLAPIENTRY trMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 ) {
	trWriteCMD( CMD_MAPGRID2F );
	trWritei( un );
	trWritef( u1 );
	trWritef( u2 );
	trWritei( vn );
	trWritef( v1 );
	trWritef( v2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->MapGrid2f( un, u1, u2, vn, v1, v2 );
		trError();
	}
}


GLAPI void GLAPIENTRY trMaterialf( GLenum face, GLenum pname, GLfloat param ) {
	trWriteCMD( CMD_MATERIALF );
	trWriteEnum( face );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->Materialf( face, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trMaterialfv( GLenum face, GLenum pname, const GLfloat *params ) {
	trWriteCMD( CMD_MATERIALFV );
	trWriteEnum( face );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_AMBIENT_AND_DIFFUSE:
		case GL_SPECULAR:
		case GL_EMISSION:
			trWriteArrayf( 4, params );
			break;
		case GL_SHININESS:
			trWritef( params[0] );
			break;
		case GL_COLOR_INDEXES:
			trWriteArrayf( 3, params );
			break;
		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->Materialfv( face, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trMateriali( GLenum face, GLenum pname, GLint param ) {
	trWriteCMD( CMD_MATERIALI );
	trWriteEnum( face );
	trWriteEnum( pname );
	trWritei( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->Materiali( face, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trMaterialiv( GLenum face, GLenum pname, const GLint *params ) {
	trWriteCMD( CMD_MATERIALIV );
	trWriteEnum( face );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_AMBIENT_AND_DIFFUSE:
		case GL_SPECULAR:
		case GL_EMISSION:
			trWriteArrayi( 4, params );
			break;
		case GL_SHININESS:
			trWritei( params[0] );
			break;
		case GL_COLOR_INDEXES:
			trWriteArrayi( 3, params );
			break;
		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->Materialiv( face, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trMatrixMode( GLenum mode ) {
	trWriteCMD( CMD_MATRIXMODE );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->MatrixMode( mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trMinmax( GLenum target, GLenum internalformat, GLboolean sink ) {
	trWriteCMD( CMD_MINMAX );
	trWriteEnum( target );
	trWriteEnum( internalformat );
	trWriteBool( sink );

	if( trCtx()->doExec ) {
		trGetDispatch()->Minmax( target, internalformat, sink );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord1dARB( GLenum target, GLdouble s) {
	trWriteCMD( CMD_MULTITEXCOORD1DARB );
	trWriteEnum( target );
	trWrited( s );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord1dARB( target, s );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord1dvARB( GLenum target, const GLdouble *v) {
	trWriteCMD( CMD_MULTITEXCOORD1DVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWrited( v[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord1dvARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord1fARB( GLenum target, GLfloat s) {
	trWriteCMD( CMD_MULTITEXCOORD1FARB );
	trWriteEnum( target );
	trWritef( s );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord1fARB( target, s );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord1fvARB( GLenum target, const GLfloat *v) {
	trWriteCMD( CMD_MULTITEXCOORD1FVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWritef( v[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord1fvARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord1iARB( GLenum target, GLint s) {
	trWriteCMD( CMD_MULTITEXCOORD1IARB );
	trWriteEnum( target );
	trWritei( s );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord1iARB( target, s );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord1ivARB( GLenum target, const GLint *v) {
	trWriteCMD( CMD_MULTITEXCOORD1IVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWritei( v[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord1ivARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord1sARB( GLenum target, GLshort s) {
	trWriteCMD( CMD_MULTITEXCOORD1SARB );
	trWriteEnum( target );
	trWrites( s );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord1sARB( target, s );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord1svARB( GLenum target, const GLshort *v) {
	trWriteCMD( CMD_MULTITEXCOORD1SVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWrites( v[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord1svARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord2dARB( GLenum target, GLdouble s, GLdouble t) {
	trWriteCMD( CMD_MULTITEXCOORD2DARB );
	trWriteEnum( target );
	trWrited( s );
	trWrited( t );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord2dARB( target, s, t );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord2dvARB( GLenum target, const GLdouble *v) {
	trWriteCMD( CMD_MULTITEXCOORD2DVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrayd( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord2dvARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord2fARB( GLenum target, GLfloat s, GLfloat t) {
	trWriteCMD( CMD_MULTITEXCOORD2FARB );
	trWriteEnum( target );
	trWritef( s );
	trWritef( t );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord2fARB( target, s, t );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord2fvARB( GLenum target, const GLfloat *v) {
	trWriteCMD( CMD_MULTITEXCOORD2FVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrayf( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord2fvARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord2iARB( GLenum target, GLint s, GLint t) {
	trWriteCMD( CMD_MULTITEXCOORD2IARB );
	trWriteEnum( target );
	trWritei( s );
	trWritei( t );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord2iARB( target, s, t );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord2ivARB( GLenum target, const GLint *v) {
	trWriteCMD( CMD_MULTITEXCOORD2IVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrayi( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord2ivARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord2sARB( GLenum target, GLshort s, GLshort t) {
	trWriteCMD( CMD_MULTITEXCOORD2SARB );
	trWriteEnum( target );
	trWrites( s );
	trWrites( t );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord2sARB( target, s, t );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord2svARB( GLenum target, const GLshort *v) {
	trWriteCMD( CMD_MULTITEXCOORD2SVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrays( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord2svARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord3dARB( GLenum target, GLdouble s, GLdouble t, GLdouble r) {
	trWriteCMD( CMD_MULTITEXCOORD3DARB );
	trWriteEnum( target );
	trWrited( s );
	trWrited( t );
	trWrited( r );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord3dARB( target, s, t, r );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord3dvARB( GLenum target, const GLdouble *v) {
	trWriteCMD( CMD_MULTITEXCOORD3DVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrayd( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord3dvARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord3fARB( GLenum target, GLfloat s, GLfloat t, GLfloat r) {
	trWriteCMD( CMD_MULTITEXCOORD3FARB );
	trWriteEnum( target );
	trWritef( s );
	trWritef( t );
	trWritef( r );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord3fARB( target, s, t, r );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord3fvARB( GLenum target, const GLfloat *v) {
	trWriteCMD( CMD_MULTITEXCOORD3FVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrayf( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord3fvARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord3iARB( GLenum target, GLint s, GLint t, GLint r) {
	trWriteCMD( CMD_MULTITEXCOORD3IARB );
	trWriteEnum( target );
	trWritei( s );
	trWritei( t );
	trWritei( r );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord3iARB( target, s, t, r );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord3ivARB( GLenum target, const GLint *v) {
	trWriteCMD( CMD_MULTITEXCOORD3IVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrayi( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord3ivARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord3sARB( GLenum target, GLshort s, GLshort t, GLshort r) {
	trWriteCMD( CMD_MULTITEXCOORD3SARB );
	trWriteEnum( target );
	trWrites( s );
	trWrites( t );
	trWrites( r );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord3sARB( target, s, t, r );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord3svARB( GLenum target, const GLshort *v) {
	trWriteCMD( CMD_MULTITEXCOORD3SVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrays( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord3svARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord4dARB( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q) {
	trWriteCMD( CMD_MULTITEXCOORD4DARB );
	trWriteEnum( target );
	trWrited( s );
	trWrited( t );
	trWrited( r );
	trWrited( q );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord4dARB( target, s, t, r, q );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord4dvARB( GLenum target, const GLdouble *v) {
	trWriteCMD( CMD_MULTITEXCOORD4DVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrayd( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord4dvARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord4fARB( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) {
	trWriteCMD( CMD_MULTITEXCOORD4FARB );
	trWriteEnum( target );
	trWritef( s );
	trWritef( t );
	trWritef( r );
	trWritef( q );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord4fARB( target, s, t, r, q );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord4fvARB( GLenum target, const GLfloat *v) {
	trWriteCMD( CMD_MULTITEXCOORD4FVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrayf( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord4fvARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord4iARB( GLenum target, GLint s, GLint t, GLint r, GLint q) {
	trWriteCMD( CMD_MULTITEXCOORD4IARB );
	trWriteEnum( target );
	trWritei( s );
	trWritei( t );
	trWritei( r );
	trWritei( q );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord4iARB( target, s, t, r, q );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord4ivARB( GLenum target, const GLint *v) {
	trWriteCMD( CMD_MULTITEXCOORD4IVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrayi( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord4ivARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord4sARB( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q) {
	trWriteCMD( CMD_MULTITEXCOORD4SARB );
	trWriteEnum( target );
	trWrites( s );
	trWrites( t );
	trWrites( r );
	trWrites( q );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord4sARB( target, s, t, r, q );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultiTexCoord4svARB( GLenum target, const GLshort *v) {
	trWriteCMD( CMD_MULTITEXCOORD4SVARB );
	trWriteEnum( target );
	trWritePointer( (void *)v );
	trFileFlush();
	trWriteArrays( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultiTexCoord4svARB( target, v );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultMatrixd( const GLdouble *m ) {
	trWriteCMD( CMD_MULTMATRIXD );
	trWritePointer( (void *)m  );
	trFileFlush();
	trWriteArrayd( 16, m );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultMatrixd( m  );
		trError();
	}
}


GLAPI void GLAPIENTRY trMultMatrixf( const GLfloat *m ) {
	trWriteCMD( CMD_MULTMATRIXF );
	trWritePointer( (void *)m  );
	trFileFlush();
	trWriteArrayf( 16, m );

	if( trCtx()->doExec ) {
		trGetDispatch()->MultMatrixf( m  );
		trError();
	}
}


GLAPI void GLAPIENTRY trNewList( GLuint list, GLenum mode ) {
	trWriteCMD( CMD_NEWLIST );
	trWriteui( list );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->NewList( list, mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormal3b( GLbyte nx, GLbyte ny, GLbyte nz ) {
	trWriteCMD( CMD_NORMAL3B );
	trWriteb( nx );
	trWriteb( ny );
	trWriteb( nz );

	if( trCtx()->doExec ) {
		trGetDispatch()->Normal3b( nx, ny, nz );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormal3bv( const GLbyte *v ) {
	trWriteCMD( CMD_NORMAL3BV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayb( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Normal3bv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormal3d( GLdouble nx, GLdouble ny, GLdouble nz ) {
	trWriteCMD( CMD_NORMAL3D );
	trWrited( nx );
	trWrited( ny );
	trWrited( nz );

	if( trCtx()->doExec ) {
		trGetDispatch()->Normal3d( nx, ny, nz );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormal3dv( const GLdouble *v ) {
	trWriteCMD( CMD_NORMAL3DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Normal3dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormal3f( GLfloat nx, GLfloat ny, GLfloat nz ) {
	trWriteCMD( CMD_NORMAL3F );
	trWritef( nx );
	trWritef( ny );
	trWritef( nz );

	if( trCtx()->doExec ) {
		trGetDispatch()->Normal3f( nx, ny, nz );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormal3fv( const GLfloat *v ) {
	trWriteCMD( CMD_NORMAL3FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Normal3fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormal3i( GLint nx, GLint ny, GLint nz ) {
	trWriteCMD( CMD_NORMAL3I );
	trWritei( nx );
	trWritei( ny );
	trWritei( nz );

	if( trCtx()->doExec ) {
		trGetDispatch()->Normal3i( nx, ny, nz );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormal3iv( const GLint *v ) {
	trWriteCMD( CMD_NORMAL3IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Normal3iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormal3s( GLshort nx, GLshort ny, GLshort nz ) {
	trWriteCMD( CMD_NORMAL3S );
	trWrites( nx );
	trWrites( ny );
	trWrites( nz );

	if( trCtx()->doExec ) {
		trGetDispatch()->Normal3s( nx, ny, nz );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormal3sv( const GLshort *v ) {
	trWriteCMD( CMD_NORMAL3SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Normal3sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormalPointerEXT( GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr ) { /* TODO */
	trWriteCMD( CMD_NORMALPOINTEREXT );
	trWriteEnum( type );
	trWriteSizei( stride );
	trWriteSizei( count );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->NormalPointerEXT( type, stride, count, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr ) { /* TODO */
	trWriteCMD( CMD_NORMALPOINTER );
	trWriteEnum( type );
	trWriteSizei( stride );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->NormalPointer( type, stride, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
	trWriteCMD( CMD_ORTHO );
	trWrited( left );
	trWrited( right );
	trWrited( bottom );
	trWrited( top );
	trWrited( near_val );
	trWrited( far_val );

	if( trCtx()->doExec ) {
		trGetDispatch()->Ortho( left, right, bottom, top, near_val, far_val );
		trError();
	}
}


GLAPI void GLAPIENTRY trPassThrough( GLfloat token ) {
	trWriteCMD( CMD_PASSTHROUGH );
	trWritef( token );

	if( trCtx()->doExec ) {
		trGetDispatch()->PassThrough( token );
		trError();
	}
}


GLAPI void GLAPIENTRY trPixelMapfv( GLenum map, GLint mapsize, const GLfloat *values ) {
	trWriteCMD( CMD_PIXELMAPFV );
	trWriteEnum( map );
	trWritei( mapsize );
	trWritePointer( (void *)values  );
	trFileFlush();
	trWriteArrayf( mapsize, values );

	if( trCtx()->doExec ) {
		trGetDispatch()->PixelMapfv( map, mapsize, values  );
		trError();
	}
}


GLAPI void GLAPIENTRY trPixelMapuiv( GLenum map, GLint mapsize, const GLuint *values ) {
	trWriteCMD( CMD_PIXELMAPUIV );
	trWriteEnum( map );
	trWritei( mapsize );
	trWritePointer( (void *)values  );
	trFileFlush();
	trWriteArrayui( mapsize, values );

	if( trCtx()->doExec ) {
		trGetDispatch()->PixelMapuiv( map, mapsize, values  );
		trError();
	}
}


GLAPI void GLAPIENTRY trPixelMapusv( GLenum map, GLint mapsize, const GLushort *values ) {
	trWriteCMD( CMD_PIXELMAPUSV );
	trWriteEnum( map );
	trWritei( mapsize );
	trWritePointer( (void *)values  );
	trFileFlush();
	trWriteArrayus( mapsize, values );

	if( trCtx()->doExec ) {
		trGetDispatch()->PixelMapusv( map, mapsize, values  );
		trError();
	}
}


GLAPI void GLAPIENTRY trPixelStoref( GLenum pname, GLfloat param ) {
	trWriteCMD( CMD_PIXELSTOREF );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->PixelStoref( pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trPixelStorei( GLenum pname, GLint param ) {
	trWriteCMD( CMD_PIXELSTOREI );
	trWriteEnum( pname );
	trWritei( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->PixelStorei( pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trPixelTransferf( GLenum pname, GLfloat param ) {
	trWriteCMD( CMD_PIXELTRANSFERF );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->PixelTransferf( pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trPixelTransferi( GLenum pname, GLint param ) {
	trWriteCMD( CMD_PIXELTRANSFERI );
	trWriteEnum( pname );
	trWritei( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->PixelTransferi( pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trPixelZoom( GLfloat xfactor, GLfloat yfactor ) {
	trWriteCMD( CMD_PIXELZOOM );
	trWritef( xfactor );
	trWritef( yfactor );

	if( trCtx()->doExec ) {
		trGetDispatch()->PixelZoom( xfactor, yfactor );
		trError();
	}
}


GLAPI void GLAPIENTRY trPointParameterfEXT( GLenum pname, GLfloat param ) {
	trWriteCMD( CMD_POINTPARAMETERFEXT );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->PointParameterfEXT( pname, param );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trPointParameterfSGIS( GLenum pname, GLfloat param) {
	trWriteCMD( CMD_POINTPARAMETERFSGIS );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->PointParameterfSGIS( pname, param );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trPointParameterfvEXT( GLenum pname, const GLfloat *params ) {
	trWriteCMD( CMD_POINTPARAMETERFVEXT );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_POINT_SIZE_MIN_EXT:
		case GL_POINT_SIZE_MAX_EXT:
		case GL_POINT_FADE_THRESHOLD_SIZE_EXT:
			trWritef( params[0] );
			break;

		case GL_DISTANCE_ATTENUATION_EXT:
			trWriteArrayf( 3, params );
			break;

		default:
			/* The 2nd pass should handle this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->PointParameterfvEXT( pname, params  );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trPointParameterfvSGIS( GLenum pname, const GLfloat *params) { /* TODO */
	trWriteCMD( CMD_POINTPARAMETERFVSGIS );
	trWriteEnum( pname );
	trWritePointer( (void *)params );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->PointParameterfvSGIS( pname, params );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trPointSize( GLfloat size ) {
	trWriteCMD( CMD_POINTSIZE );
	trWritef( size );

	if( trCtx()->doExec ) {
		trGetDispatch()->PointSize( size );
		trError();
	}
}


GLAPI void GLAPIENTRY trPolygonMode( GLenum face, GLenum mode ) {
	trWriteCMD( CMD_POLYGONMODE );
	trWriteEnum( face );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->PolygonMode( face, mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trPolygonOffsetEXT( GLfloat factor, GLfloat bias ) {
	trWriteCMD( CMD_POLYGONOFFSETEXT );
	trWritef( factor );
	trWritef( bias );

	if( trCtx()->doExec ) {
		trGetDispatch()->PolygonOffsetEXT( factor, bias );
		trError();
	}
}


GLAPI void GLAPIENTRY trPolygonOffset( GLfloat factor, GLfloat units ) {
	trWriteCMD( CMD_POLYGONOFFSET );
	trWritef( factor );
	trWritef( units );

	if( trCtx()->doExec ) {
		trGetDispatch()->PolygonOffset( factor, units );
		trError();
	}
}


GLAPI void GLAPIENTRY trPolygonStipple( const GLubyte *mask ) {
	trWriteCMD( CMD_POLYGONSTIPPLE );
	trWritePointer( (void *)mask  );
	trFileFlush();
	trWriteArrayub( 64, mask );

	if( trCtx()->doExec ) {
		trGetDispatch()->PolygonStipple( mask  );
		trError();
	}
}


GLAPI void GLAPIENTRY trPopAttrib( void ) {
	trWriteCMD( CMD_POPATTRIB );

	if( trCtx()->doExec ) {
		trGetDispatch()->PopAttrib(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trPopClientAttrib( void ) {
	trWriteCMD( CMD_POPCLIENTATTRIB );

	if( trCtx()->doExec ) {
		trGetDispatch()->PopClientAttrib(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trPopMatrix( void ) {
	trWriteCMD( CMD_POPMATRIX );

	if( trCtx()->doExec ) {
		trGetDispatch()->PopMatrix(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trPopName( void ) {
	trWriteCMD( CMD_POPNAME );

	if( trCtx()->doExec ) {
		trGetDispatch()->PopName(  );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trPrioritizeTexturesEXT( GLsizei n, const GLuint *textures, const GLclampf *priorities ) {
	trWriteCMD( CMD_PRIORITIZETEXTURESEXT );
	trWriteSizei( n );
	trWritePointer( (void *)textures );
	trFileFlush();
	trWriteArrayui( n, textures );

	trWritePointer( (void *)priorities  );
	trFileFlush();
	/* FIXME!!! */
	trWriteArrayf( n, priorities );

	if( trCtx()->doExec ) {
		trGetDispatch()->PrioritizeTexturesEXT( n, textures, priorities  );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities ) {
	trWriteCMD( CMD_PRIORITIZETEXTURES );
	trWriteSizei( n );
	trWritePointer( (void *)textures );
	trFileFlush();
	trWriteArrayui( n, textures );

	trWritePointer( (void *)priorities  );
	trFileFlush();
	/* FIXME!!! */
	trWriteArrayf( n, priorities );

	if( trCtx()->doExec ) {
		trGetDispatch()->PrioritizeTextures( n, textures, priorities  );
		trError();
	}
}


GLAPI void GLAPIENTRY trPushAttrib( GLbitfield mask ) {
	trWriteCMD( CMD_PUSHATTRIB );
	trWriteBits( mask );

	if( trCtx()->doExec ) {
		trGetDispatch()->PushAttrib( mask );
		trError();
	}
}


GLAPI void GLAPIENTRY trPushClientAttrib( GLbitfield mask ) {
	trWriteCMD( CMD_PUSHCLIENTATTRIB );
	trWriteBits( mask );

	if( trCtx()->doExec ) {
		trGetDispatch()->PushClientAttrib( mask );
		trError();
	}
}


GLAPI void GLAPIENTRY trPushMatrix( void ) {
	trWriteCMD( CMD_PUSHMATRIX );

	if( trCtx()->doExec ) {
		trGetDispatch()->PushMatrix(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trPushName( GLuint name ) {
	trWriteCMD( CMD_PUSHNAME );
	trWriteui( name );

	if( trCtx()->doExec ) {
		trGetDispatch()->PushName( name );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos2d( GLdouble x, GLdouble y ) {
	trWriteCMD( CMD_RASTERPOS2D );
	trWrited( x );
	trWrited( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos2d( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos2dv( const GLdouble *v ) {
	trWriteCMD( CMD_RASTERPOS2DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos2dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos2f( GLfloat x, GLfloat y ) {
	trWriteCMD( CMD_RASTERPOS2F );
	trWritef( x );
	trWritef( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos2f( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos2fv( const GLfloat *v ) {
	trWriteCMD( CMD_RASTERPOS2FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos2fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos2i( GLint x, GLint y ) {
	trWriteCMD( CMD_RASTERPOS2I );
	trWritei( x );
	trWritei( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos2i( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos2iv( const GLint *v ) {
	trWriteCMD( CMD_RASTERPOS2IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos2iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos2s( GLshort x, GLshort y ) {
	trWriteCMD( CMD_RASTERPOS2S );
	trWrites( x );
	trWrites( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos2s( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos2sv( const GLshort *v ) {
	trWriteCMD( CMD_RASTERPOS2SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos2sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos3d( GLdouble x, GLdouble y, GLdouble z ) {
	trWriteCMD( CMD_RASTERPOS3D );
	trWrited( x );
	trWrited( y );
	trWrited( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos3d( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos3dv( const GLdouble *v ) {
	trWriteCMD( CMD_RASTERPOS3DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos3dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos3f( GLfloat x, GLfloat y, GLfloat z ) {
	trWriteCMD( CMD_RASTERPOS3F );
	trWritef( x );
	trWritef( y );
	trWritef( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos3f( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos3fv( const GLfloat *v ) {
	trWriteCMD( CMD_RASTERPOS3FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos3fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos3i( GLint x, GLint y, GLint z ) {
	trWriteCMD( CMD_RASTERPOS3I );
	trWritei( x );
	trWritei( y );
	trWritei( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos3i( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos3iv( const GLint *v ) {
	trWriteCMD( CMD_RASTERPOS3IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos3iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos3s( GLshort x, GLshort y, GLshort z ) {
	trWriteCMD( CMD_RASTERPOS3S );
	trWrites( x );
	trWrites( y );
	trWrites( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos3s( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos3sv( const GLshort *v ) {
	trWriteCMD( CMD_RASTERPOS3SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos3sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ) {
	trWriteCMD( CMD_RASTERPOS4D );
	trWrited( x );
	trWrited( y );
	trWrited( z );
	trWrited( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos4d( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos4dv( const GLdouble *v ) {
	trWriteCMD( CMD_RASTERPOS4DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos4dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
	trWriteCMD( CMD_RASTERPOS4F );
	trWritef( x );
	trWritef( y );
	trWritef( z );
	trWritef( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos4f( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos4fv( const GLfloat *v ) {
	trWriteCMD( CMD_RASTERPOS4FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos4fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos4i( GLint x, GLint y, GLint z, GLint w ) {
	trWriteCMD( CMD_RASTERPOS4I );
	trWritei( x );
	trWritei( y );
	trWritei( z );
	trWritei( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos4i( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos4iv( const GLint *v ) {
	trWriteCMD( CMD_RASTERPOS4IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos4iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w ) {
	trWriteCMD( CMD_RASTERPOS4S );
	trWrites( x );
	trWrites( y );
	trWrites( z );
	trWrites( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos4s( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trRasterPos4sv( const GLshort *v ) {
	trWriteCMD( CMD_RASTERPOS4SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->RasterPos4sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trReadBuffer( GLenum mode ) {
	trWriteCMD( CMD_READBUFFER );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->ReadBuffer( mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels ) {
	GLint pixelsize;

	trWriteCMD( CMD_READPIXELS );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );
	trWriteSizei( height );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels  );
	trFileFlush();

   pixelsize = trGetPixelSize( format, type );
	if( trCtx()->doExec ) {
		trGetDispatch()->ReadPixels( x, y, width, height, format, type, pixels  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 ) {
	trWriteCMD( CMD_RECTD );
	trWrited( x1 );
	trWrited( y1 );
	trWrited( x2 );
	trWrited( y2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->Rectd( x1, y1, x2, y2 );
		trError();
	}
}


GLAPI void GLAPIENTRY trRectdv( const GLdouble *v1, const GLdouble *v2 ) {
	trWriteCMD( CMD_RECTDV );
	trWritePointer( (void *)v1 );
	trFileFlush();
	trWriteArrayd( 2, v1 );

	trWritePointer( (void *)v2  );
	trFileFlush();
	trWriteArrayd( 2, v2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->Rectdv( v1, v2  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) {
	trWriteCMD( CMD_RECTF );
	trWritef( x1 );
	trWritef( y1 );
	trWritef( x2 );
	trWritef( y2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->Rectf( x1, y1, x2, y2 );
		trError();
	}
}


GLAPI void GLAPIENTRY trRectfv( const GLfloat *v1, const GLfloat *v2 ) {
	trWriteCMD( CMD_RECTFV );
	trWritePointer( (void *)v1 );
	trFileFlush();
	trWriteArrayf( 2, v1 );

	trWritePointer( (void *)v2  );
	trFileFlush();
	trWriteArrayf( 2, v2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->Rectfv( v1, v2  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRecti( GLint x1, GLint y1, GLint x2, GLint y2 ) {
	trWriteCMD( CMD_RECTI );
	trWritei( x1 );
	trWritei( y1 );
	trWritei( x2 );
	trWritei( y2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->Recti( x1, y1, x2, y2 );
		trError();
	}
}


GLAPI void GLAPIENTRY trRectiv( const GLint *v1, const GLint *v2 ) {
	trWriteCMD( CMD_RECTIV );
	trWritePointer( (void *)v1 );
	trFileFlush();
	trWriteArrayi( 2, v1 );

	trWritePointer( (void *)v2  );
	trFileFlush();
	trWriteArrayi( 2, v2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->Rectiv( v1, v2  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 ) {
	trWriteCMD( CMD_RECTS );
	trWrites( x1 );
	trWrites( y1 );
	trWrites( x2 );
	trWrites( y2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->Rects( x1, y1, x2, y2 );
		trError();
	}
}


GLAPI void GLAPIENTRY trRectsv( const GLshort *v1, const GLshort *v2 ) {
	trWriteCMD( CMD_RECTSV );
	trWritePointer( (void *)v1 );
	trFileFlush();
	trWriteArrays( 2, v1 );

	trWritePointer( (void *)v2  );
	trFileFlush();
	trWriteArrays( 2, v2 );

	if( trCtx()->doExec ) {
		trGetDispatch()->Rectsv( v1, v2  );
		trError();
	}
}


GLAPI GLint GLAPIENTRY trRenderMode( GLenum mode ) {
	GLint retval;

	trWriteCMD( CMD_RENDERMODE );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		retval = trGetDispatch()->RenderMode( mode );
		trError();
	} else {
		retval = 0;
	}

	trWritei( retval );
	return retval;
}


GLAPI void GLAPIENTRY trResetHistogram( GLenum target ) {
	trWriteCMD( CMD_RESETHISTOGRAM );
	trWriteEnum( target );

	if( trCtx()->doExec ) {
		trGetDispatch()->ResetHistogram( target );
		trError();
	}
}


GLAPI void GLAPIENTRY trResetMinmax( GLenum target ) {
	trWriteCMD( CMD_RESETMINMAX );
	trWriteEnum( target );

	if( trCtx()->doExec ) {
		trGetDispatch()->ResetMinmax( target );
		trError();
	}
}


GLAPI void GLAPIENTRY trResizeBuffersMESA( void ) {
	trWriteCMD( CMD_RESIZEBUFFERSMESA );

	if( trCtx()->doExec ) {
		trGetDispatch()->ResizeBuffersMESA(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z ) {
	trWriteCMD( CMD_ROTATED );
	trWrited( angle );
	trWrited( x );
	trWrited( y );
	trWrited( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->Rotated( angle, x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ) {
	trWriteCMD( CMD_ROTATEF );
	trWritef( angle );
	trWritef( x );
	trWritef( y );
	trWritef( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->Rotatef( angle, x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trScaled( GLdouble x, GLdouble y, GLdouble z ) {
	trWriteCMD( CMD_SCALED );
	trWrited( x );
	trWrited( y );
	trWrited( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->Scaled( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trScalef( GLfloat x, GLfloat y, GLfloat z ) {
	trWriteCMD( CMD_SCALEF );
	trWritef( x );
	trWritef( y );
	trWritef( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->Scalef( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trScissor( GLint x, GLint y, GLsizei width, GLsizei height) {
	trWriteCMD( CMD_SCISSOR );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );
	trWriteSizei( height );

	if( trCtx()->doExec ) {
		trGetDispatch()->Scissor( x, y, width, height );
		trError();
	}
}


GLAPI void GLAPIENTRY trSelectBuffer( GLsizei size, GLuint *buffer ) {
	trWriteCMD( CMD_SELECTBUFFER );
	trWriteSizei( size );
	trWritePointer( (void *)buffer  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->SelectBuffer( size, buffer  );
		trError();
	}
}


GLAPI void GLAPIENTRY trSeparableFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column ) {
	GLint pixelsize;

	trWriteCMD( CMD_SEPARABLEFILTER2D );
	trWriteEnum( target );
	trWriteEnum( internalformat );
	trWriteSizei( width );
	trWriteSizei( height );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)row );
	trFileFlush();
	trWritePointer( (void *)column  );
	trFileFlush();

	pixelsize = trGetPixelSize( format, type );
	trWriteTypeArray( type, width, pixelsize, 0, row );
	trWriteTypeArray( type, height, pixelsize, 0, column );

	if( trCtx()->doExec ) {
		trGetDispatch()->SeparableFilter2D( target, internalformat, width, height, format, type, row, column  );
		trError();
	}
}


GLAPI void GLAPIENTRY trShadeModel( GLenum mode ) {
	trWriteCMD( CMD_SHADEMODEL );
	trWriteEnum( mode );

	if( trCtx()->doExec ) {
		trGetDispatch()->ShadeModel( mode );
		trError();
	}
}


GLAPI void GLAPIENTRY trStencilFunc( GLenum func, GLint ref, GLuint mask ) {
	trWriteCMD( CMD_STENCILFUNC );
	trWriteEnum( func );
	trWritei( ref );
	trWriteui( mask );

	if( trCtx()->doExec ) {
		trGetDispatch()->StencilFunc( func, ref, mask );
		trError();
	}
}


GLAPI void GLAPIENTRY trStencilMask( GLuint mask ) {
	trWriteCMD( CMD_STENCILMASK );
	trWriteui( mask );

	if( trCtx()->doExec ) {
		trGetDispatch()->StencilMask( mask );
		trError();
	}
}


GLAPI void GLAPIENTRY trStencilOp( GLenum fail, GLenum zfail, GLenum zpass ) {
	trWriteCMD( CMD_STENCILOP );
	trWriteEnum( fail );
	trWriteEnum( zfail );
	trWriteEnum( zpass );

	if( trCtx()->doExec ) {
		trGetDispatch()->StencilOp( fail, zfail, zpass );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord1d( GLdouble s ) {
	trWriteCMD( CMD_TEXCOORD1D );
	trWrited( s );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord1d( s );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord1dv( const GLdouble *v ) {
	trWriteCMD( CMD_TEXCOORD1DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWrited( v[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord1dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord1f( GLfloat s ) {
	trWriteCMD( CMD_TEXCOORD1F );
	trWritef( s );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord1f( s );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord1fv( const GLfloat *v ) {
	trWriteCMD( CMD_TEXCOORD1FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWritef( v[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord1fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord1i( GLint s ) {
	trWriteCMD( CMD_TEXCOORD1I );
	trWritei( s );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord1i( s );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord1iv( const GLint *v ) {
	trWriteCMD( CMD_TEXCOORD1IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWritei( v[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord1iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord1s( GLshort s ) {
	trWriteCMD( CMD_TEXCOORD1S );
	trWrites( s );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord1s( s );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord1sv( const GLshort *v ) {
	trWriteCMD( CMD_TEXCOORD1SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWrites( v[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord1sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord2d( GLdouble s, GLdouble t ) {
	trWriteCMD( CMD_TEXCOORD2D );
	trWrited( s );
	trWrited( t );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord2d( s, t );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord2dv( const GLdouble *v ) {
	trWriteCMD( CMD_TEXCOORD2DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord2dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord2f( GLfloat s, GLfloat t ) {
	trWriteCMD( CMD_TEXCOORD2F );
	trWritef( s );
	trWritef( t );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord2f( s, t );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord2fv( const GLfloat *v ) {
	trWriteCMD( CMD_TEXCOORD2FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord2fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord2i( GLint s, GLint t ) {
	trWriteCMD( CMD_TEXCOORD2I );
	trWritei( s );
	trWritei( t );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord2i( s, t );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord2iv( const GLint *v ) {
	trWriteCMD( CMD_TEXCOORD2IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord2iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord2s( GLshort s, GLshort t ) {
	trWriteCMD( CMD_TEXCOORD2S );
	trWrites( s );
	trWrites( t );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord2s( s, t );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord2sv( const GLshort *v ) {
	trWriteCMD( CMD_TEXCOORD2SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord2sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord3d( GLdouble s, GLdouble t, GLdouble r ) {
	trWriteCMD( CMD_TEXCOORD3D );
	trWrited( s );
	trWrited( t );
	trWrited( r );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord3d( s, t, r );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord3dv( const GLdouble *v ) {
	trWriteCMD( CMD_TEXCOORD3DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord3dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord3f( GLfloat s, GLfloat t, GLfloat r ) {
	trWriteCMD( CMD_TEXCOORD3F );
	trWritef( s );
	trWritef( t );
	trWritef( r );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord3f( s, t, r );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord3fv( const GLfloat *v ) {
	trWriteCMD( CMD_TEXCOORD3FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord3fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord3i( GLint s, GLint t, GLint r ) {
	trWriteCMD( CMD_TEXCOORD3I );
	trWritei( s );
	trWritei( t );
	trWritei( r );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord3i( s, t, r );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord3iv( const GLint *v ) {
	trWriteCMD( CMD_TEXCOORD3IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord3iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord3s( GLshort s, GLshort t, GLshort r ) {
	trWriteCMD( CMD_TEXCOORD3S );
	trWrites( s );
	trWrites( t );
	trWrites( r );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord3s( s, t, r );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord3sv( const GLshort *v ) {
	trWriteCMD( CMD_TEXCOORD3SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord3sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q ) {
	trWriteCMD( CMD_TEXCOORD4D );
	trWrited( s );
	trWrited( t );
	trWrited( r );
	trWrited( q );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord4d( s, t, r, q );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord4dv( const GLdouble *v ) {
	trWriteCMD( CMD_TEXCOORD4DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord4dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
	trWriteCMD( CMD_TEXCOORD4F );
	trWritef( s );
	trWritef( t );
	trWritef( r );
	trWritef( q );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord4f( s, t, r, q );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord4fv( const GLfloat *v ) {
	trWriteCMD( CMD_TEXCOORD4FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord4fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord4i( GLint s, GLint t, GLint r, GLint q ) {
	trWriteCMD( CMD_TEXCOORD4I );
	trWritei( s );
	trWritei( t );
	trWritei( r );
	trWritei( q );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord4i( s, t, r, q );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord4iv( const GLint *v ) {
	trWriteCMD( CMD_TEXCOORD4IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord4iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q ) {
	trWriteCMD( CMD_TEXCOORD4S );
	trWrites( s );
	trWrites( t );
	trWrites( r );
	trWrites( q );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord4s( s, t, r, q );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoord4sv( const GLshort *v ) {
	trWriteCMD( CMD_TEXCOORD4SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoord4sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoordPointerEXT( GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr ) { /* TODO */
	trWriteCMD( CMD_TEXCOORDPOINTEREXT );
	trWritei( size );
	trWriteEnum( type );
	trWriteSizei( stride );
	trWriteSizei( count );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->TexCoordPointerEXT( size, type, stride, count, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) { /* TODO */
	trace_context_t * tctx;

	trWriteCMD( CMD_TEXCOORDPOINTER );
	trWritei( size );
	trWriteEnum( type );
	trWriteSizei( stride );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	if( tctx->doExec ) {
		trGetDispatch()->TexCoordPointer( size, type, stride, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexEnvf( GLenum target, GLenum pname, GLfloat param ) {
	trWriteCMD( CMD_TEXENVF );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexEnvf( target, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexEnvfv( GLenum target, GLenum pname, const GLfloat *params ) {
	trWriteCMD( CMD_TEXENVFV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_TEXTURE_ENV_MODE:
			trWritef( params[0] );
			break;
		case GL_TEXTURE_ENV_COLOR:
			trWriteArrayf( 4, params );
			break;
		default:
			/* The 2nd pass should catch this */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->TexEnvfv( target, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexEnvi( GLenum target, GLenum pname, GLint param ) {
	trWriteCMD( CMD_TEXENVI );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritei( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexEnvi( target, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexEnviv( GLenum target, GLenum pname, const GLint *params ) {
	trWriteCMD( CMD_TEXENVIV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_TEXTURE_ENV_MODE:
			trWritei( params[0] );
			break;
		case GL_TEXTURE_ENV_COLOR:
			trWriteArrayi( 4, params );
			break;
		default:
			/* The 2nd pass should catch this */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->TexEnviv( target, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexGend( GLenum coord, GLenum pname, GLdouble param ) {
	trWriteCMD( CMD_TEXGEND );
	trWriteEnum( coord );
	trWriteEnum( pname );
	trWrited( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexGend( coord, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexGendv( GLenum coord, GLenum pname, const GLdouble *params ) {
	trWriteCMD( CMD_TEXGENDV );
	trWriteEnum( coord );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_TEXTURE_GEN_MODE:
			trWrited( params[0] );
			break;

		case GL_OBJECT_PLANE:
		case GL_EYE_PLANE:
			trWriteArrayd( 4, params );
			break;

		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->TexGendv( coord, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexGenf( GLenum coord, GLenum pname, GLfloat param ) {
	trWriteCMD( CMD_TEXGENF );
	trWriteEnum( coord );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexGenf( coord, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexGenfv( GLenum coord, GLenum pname, const GLfloat *params ) {
	trWriteCMD( CMD_TEXGENFV );
	trWriteEnum( coord );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_TEXTURE_GEN_MODE:
			trWritef( params[0] );
			break;

		case GL_OBJECT_PLANE:
		case GL_EYE_PLANE:
			trWriteArrayf( 4, params );
			break;

		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->TexGenfv( coord, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexGeni( GLenum coord, GLenum pname, GLint param ) {
	trWriteCMD( CMD_TEXGENI );
	trWriteEnum( coord );
	trWriteEnum( pname );
	trWritei( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexGeni( coord, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexGeniv( GLenum coord, GLenum pname, const GLint *params ) {
	trWriteCMD( CMD_TEXGENIV );
	trWriteEnum( coord );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();

	switch( pname ) {
		case GL_TEXTURE_GEN_MODE:
			trWritei( params[0] );
			break;

		case GL_OBJECT_PLANE:
		case GL_EYE_PLANE:
			trWriteArrayi( 4, params );
			break;

		default:
			/* The 2nd pass should catch this. */
			break;
	}

	if( trCtx()->doExec ) {
		trGetDispatch()->TexGeniv( coord, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexImage1D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) { /* TODO */
   GLint pixelsize;

	trWriteCMD( CMD_TEXIMAGE1D );
	trWriteEnum( target );
	trWritei( level );
	trWritei( internalFormat );
	trWriteSizei( width );
	trWritei( border );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->TexImage1D( target, level, internalFormat, width, border, format, type, pixels  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) { /* TODO */
   GLint pixelsize;

	trWriteCMD( CMD_TEXIMAGE2D );
	trWriteEnum( target );
	trWritei( level );
	trWritei( internalFormat );
	trWriteSizei( width );
	trWriteSizei( height );
	trWritei( border );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->TexImage2D( target, level, internalFormat, width, height, border, format, type, pixels  );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trTexImage3DEXT( GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) { /* TODO */

	trWriteCMD( CMD_TEXIMAGE3DEXT );
	trWriteEnum( target );
	trWritei( level );
	trWriteEnum( internalFormat );
	trWriteSizei( width );
	trWriteSizei( height );
	trWriteSizei( depth );
	trWritei( border );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels  );
	trFileFlush();

   /* Pixels isn't touched if target is GL_PROXY_TEXTURE_3D */
   if( target != GL_PROXY_TEXTURE_3D ) {
      pixelsize = trGetPixelSize( format, type );
      trWritePixelArray( GL_FALSE, type, width, height, depth, pixelsize, pixels );
   }

	if( trCtx()->doExec ) {
		trGetDispatch()->TexImage3DEXT( target, level, internalFormat, width, height, depth, border, format, type, pixels  );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trTexImage3D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) { /* TODO */
   GLint pixelsize;

	trWriteCMD( CMD_TEXIMAGE3D );
	trWriteEnum( target );
	trWritei( level );
	trWritei( internalFormat );
	trWriteSizei( width );
	trWriteSizei( height );
	trWriteSizei( depth );
	trWritei( border );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->TexImage3D( target, level, internalFormat, width, height, depth, border, format, type, pixels  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexParameterf( GLenum target, GLenum pname, GLfloat param ) {
	trWriteCMD( CMD_TEXPARAMETERF );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritef( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexParameterf( target, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
	trWriteCMD( CMD_TEXPARAMETERFV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();
  	trWritef( params[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexParameterfv( target, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexParameteri( GLenum target, GLenum pname, GLint param ) {
	trWriteCMD( CMD_TEXPARAMETERI );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritei( param );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexParameteri( target, pname, param );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexParameteriv( GLenum target, GLenum pname, const GLint *params ) {
	trWriteCMD( CMD_TEXPARAMETERIV );
	trWriteEnum( target );
	trWriteEnum( pname );
	trWritePointer( (void *)params  );
	trFileFlush();
  	trWritei( params[0] );

	if( trCtx()->doExec ) {
		trGetDispatch()->TexParameteriv( target, pname, params  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels ) { /* TODO */
   GLint pixelsize;

	trWriteCMD( CMD_TEXSUBIMAGE1D );
	trWriteEnum( target );
	trWritei( level );
	trWritei( xoffset );
	trWriteSizei( width );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->TexSubImage1D( target, level, xoffset, width, format, type, pixels  );
		trError();
	}
}


GLAPI void GLAPIENTRY trTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ) { /* TODO */
   GLint pixelsize;

	trWriteCMD( CMD_TEXSUBIMAGE2D );
	trWriteEnum( target );
	trWritei( level );
	trWritei( xoffset );
	trWritei( yoffset );
	trWriteSizei( width );
	trWriteSizei( height );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->TexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels  );
		trError();
	}
}


#if 0
// Not in MESAs dispatch table
GLAPI void GLAPIENTRY trTexSubImage3DEXT( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) { /* TODO */
   GLint pixelsize;

	trWriteCMD( CMD_TEXSUBIMAGE3DEXT );
	trWriteEnum( target );
	trWritei( level );
	trWritei( xoffset );
	trWritei( yoffset );
	trWritei( zoffset );
	trWriteSizei( width );
	trWriteSizei( height );
	trWriteSizei( depth );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels );
	trFileFlush();

   /* Pixels isn't touched if target is GL_PROXY_TEXTURE_3D */
   if( target != GL_PROXY_TEXTURE_3D ) {
      pixelsize = trGetPixelSize( format, type );
      trWritePixelArray( GL_FALSE, type, width, height, depth, pixelsize, pixels );
   }

	if( trCtx()->doExec ) {
		trGetDispatch()->TexSubImage3DEXT( target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels );
		trError();
	}
}
#endif


GLAPI void GLAPIENTRY trTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) { /* TODO */
   GLint pixelsize;

	trWriteCMD( CMD_TEXSUBIMAGE3D );
	trWriteEnum( target );
	trWritei( level );
	trWritei( xoffset );
	trWritei( yoffset );
	trWritei( zoffset );
	trWriteSizei( width );
	trWriteSizei( height );
	trWriteSizei( depth );
	trWriteEnum( format );
	trWriteEnum( type );
	trWritePointer( (void *)pixels );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->TexSubImage3D( target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels );
		trError();
	}
}


GLAPI void GLAPIENTRY trTranslated( GLdouble x, GLdouble y, GLdouble z ) {
	trWriteCMD( CMD_TRANSLATED );
	trWrited( x );
	trWrited( y );
	trWrited( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->Translated( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trTranslatef( GLfloat x, GLfloat y, GLfloat z ) {
	trWriteCMD( CMD_TRANSLATEF );
	trWritef( x );
	trWritef( y );
	trWritef( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->Translatef( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trUnlockArraysEXT( void ) {
	trWriteCMD( CMD_UNLOCKARRAYSEXT );

	if( trCtx()->doExec ) {
		trGetDispatch()->UnlockArraysEXT(  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex2d( GLdouble x, GLdouble y ) {
	trWriteCMD( CMD_VERTEX2D );
	trWrited( x );
	trWrited( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex2d( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex2dv( const GLdouble *v ) {
	trWriteCMD( CMD_VERTEX2DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex2dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex2f( GLfloat x, GLfloat y ) {
	trWriteCMD( CMD_VERTEX2F );
	trWritef( x );
	trWritef( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex2f( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex2fv( const GLfloat *v ) {
	trWriteCMD( CMD_VERTEX2FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex2fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex2i( GLint x, GLint y ) {
	trWriteCMD( CMD_VERTEX2I );
	trWritei( x );
	trWritei( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex2i( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex2iv( const GLint *v ) {
	trWriteCMD( CMD_VERTEX2IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex2iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex2s( GLshort x, GLshort y ) {
	trWriteCMD( CMD_VERTEX2S );
	trWrites( x );
	trWrites( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex2s( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex2sv( const GLshort *v ) {
	trWriteCMD( CMD_VERTEX2SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 2, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex2sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex3d( GLdouble x, GLdouble y, GLdouble z ) {
	trWriteCMD( CMD_VERTEX3D );
	trWrited( x );
	trWrited( y );
	trWrited( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex3d( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex3dv( const GLdouble *v ) {
	trWriteCMD( CMD_VERTEX3DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex3dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex3f( GLfloat x, GLfloat y, GLfloat z ) {
	trWriteCMD( CMD_VERTEX3F );
	trWritef( x );
	trWritef( y );
	trWritef( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex3f( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex3fv( const GLfloat *v ) {
	trWriteCMD( CMD_VERTEX3FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex3fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex3i( GLint x, GLint y, GLint z ) {
	trWriteCMD( CMD_VERTEX3I );
	trWritei( x );
	trWritei( y );
	trWritei( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex3i( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex3iv( const GLint *v ) {
	trWriteCMD( CMD_VERTEX3IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex3iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex3s( GLshort x, GLshort y, GLshort z ) {
	trWriteCMD( CMD_VERTEX3S );
	trWrites( x );
	trWrites( y );
	trWrites( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex3s( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex3sv( const GLshort *v ) {
	trWriteCMD( CMD_VERTEX3SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 3, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex3sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ) {
	trWriteCMD( CMD_VERTEX4D );
	trWrited( x );
	trWrited( y );
	trWrited( z );
	trWrited( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex4d( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex4dv( const GLdouble *v ) {
	trWriteCMD( CMD_VERTEX4DV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayd( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex4dv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
	trWriteCMD( CMD_VERTEX4F );
	trWritef( x );
	trWritef( y );
	trWritef( z );
	trWritef( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex4f( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex4fv( const GLfloat *v ) {
	trWriteCMD( CMD_VERTEX4FV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayf( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex4fv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex4i( GLint x, GLint y, GLint z, GLint w ) {
	trWriteCMD( CMD_VERTEX4I );
	trWritei( x );
	trWritei( y );
	trWritei( z );
	trWritei( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex4i( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex4iv( const GLint *v ) {
	trWriteCMD( CMD_VERTEX4IV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrayi( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex4iv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex4s( GLshort x, GLshort y, GLshort z, GLshort w ) {
	trWriteCMD( CMD_VERTEX4S );
	trWrites( x );
	trWrites( y );
	trWrites( z );
	trWrites( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex4s( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertex4sv( const GLshort *v ) {
	trWriteCMD( CMD_VERTEX4SV );
	trWritePointer( (void *)v  );
	trFileFlush();
	trWriteArrays( 4, v );

	if( trCtx()->doExec ) {
		trGetDispatch()->Vertex4sv( v  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertexPointerEXT( GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr ) { /* TODO */
	trWriteCMD( CMD_VERTEXPOINTEREXT );
	trWritei( size );
	trWriteEnum( type );
	trWriteSizei( stride );
	trWriteSizei( count );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->VertexPointerEXT( size, type, stride, count, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) { /* TODO */
	trWriteCMD( CMD_VERTEXPOINTER );
	trWritei( size );
	trWriteEnum( type );
	trWriteSizei( stride );
	trWritePointer( (void *)ptr  );
	trFileFlush();

	if( trCtx()->doExec ) {
		trGetDispatch()->VertexPointer( size, type, stride, ptr  );
		trError();
	}
}


GLAPI void GLAPIENTRY trViewport( GLint x, GLint y, GLsizei width, GLsizei height ) {
	trWriteCMD( CMD_VIEWPORT );
	trWritei( x );
	trWritei( y );
	trWriteSizei( width );
	trWriteSizei( height );

	if( trCtx()->doExec ) {
		trGetDispatch()->Viewport( x, y, width, height );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos2dMESA( GLdouble x, GLdouble y ) {
	trWriteCMD( CMD_WINDOWPOS2DMESA );
	trWrited( x );
	trWrited( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos2dMESA( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos2dvMESA( const GLdouble *p ) {
	trWriteCMD( CMD_WINDOWPOS2DVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrayd( 2, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos2dvMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos2fMESA( GLfloat x, GLfloat y ) {
	trWriteCMD( CMD_WINDOWPOS2FMESA );
	trWritef( x );
	trWritef( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos2fMESA( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos2fvMESA( const GLfloat *p ) {
	trWriteCMD( CMD_WINDOWPOS2FVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrayf( 2, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos2fvMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos2iMESA( GLint x, GLint y ) {
	trWriteCMD( CMD_WINDOWPOS2IMESA );
	trWritei( x );
	trWritei( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos2iMESA( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos2ivMESA( const GLint *p ) {
	trWriteCMD( CMD_WINDOWPOS2IVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrayi( 2, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos2ivMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos2sMESA( GLshort x, GLshort y ) {
	trWriteCMD( CMD_WINDOWPOS2SMESA );
	trWrites( x );
	trWrites( y );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos2sMESA( x, y );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos2svMESA( const GLshort *p ) {
	trWriteCMD( CMD_WINDOWPOS2SVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrays( 2, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos2svMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z ) {
	trWriteCMD( CMD_WINDOWPOS3DMESA );
	trWrited( x );
	trWrited( y );
	trWrited( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos3dMESA( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos3dvMESA( const GLdouble *p ) {
	trWriteCMD( CMD_WINDOWPOS3DVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrayd( 3, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos3dvMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z ) {
	trWriteCMD( CMD_WINDOWPOS3FMESA );
	trWritef( x );
	trWritef( y );
	trWritef( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos3fMESA( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos3fvMESA( const GLfloat *p ) {
	trWriteCMD( CMD_WINDOWPOS3FVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrayf( 3, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos3fvMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos3iMESA( GLint x, GLint y, GLint z ) {
	trWriteCMD( CMD_WINDOWPOS3IMESA );
	trWritei( x );
	trWritei( y );
	trWritei( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos3iMESA( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos3ivMESA( const GLint *p ) {
	trWriteCMD( CMD_WINDOWPOS3IVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrayi( 3, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos3ivMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos3sMESA( GLshort x, GLshort y, GLshort z ) {
	trWriteCMD( CMD_WINDOWPOS3SMESA );
	trWrites( x );
	trWrites( y );
	trWrites( z );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos3sMESA( x, y, z );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos3svMESA( const GLshort *p ) {
	trWriteCMD( CMD_WINDOWPOS3SVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrays( 3, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos3svMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w) {
	trWriteCMD( CMD_WINDOWPOS4DMESA );
	trWrited( x );
	trWrited( y );
	trWrited( z );
	trWrited( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos4dMESA( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos4dvMESA( const GLdouble *p ) {
	trWriteCMD( CMD_WINDOWPOS4DVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrayd( 4, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos4dvMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
	trWriteCMD( CMD_WINDOWPOS4FMESA );
	trWritef( x );
	trWritef( y );
	trWritef( z );
	trWritef( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos4fMESA( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos4fvMESA( const GLfloat *p ) {
	trWriteCMD( CMD_WINDOWPOS4FVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrayf( 4, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos4fvMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w ) {
	trWriteCMD( CMD_WINDOWPOS4IMESA );
	trWritei( x );
	trWritei( y );
	trWritei( z );
	trWritei( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos4iMESA( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos4ivMESA( const GLint *p ) {
	trWriteCMD( CMD_WINDOWPOS4IVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrayi( 4, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos4ivMESA( p  );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w ) {
	trWriteCMD( CMD_WINDOWPOS4SMESA );
	trWrites( x );
	trWrites( y );
	trWrites( z );
	trWrites( w );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos4sMESA( x, y, z, w );
		trError();
	}
}


GLAPI void GLAPIENTRY trWindowPos4svMESA( const GLshort *p ) {
	trWriteCMD( CMD_WINDOWPOS4SVMESA );
	trWritePointer( (void *)p  );
	trFileFlush();
	trWriteArrays( 4, p );

	if( trCtx()->doExec ) {
		trGetDispatch()->WindowPos4svMESA( p  );
		trError();
	}
}


void trInitDispatch( struct _glapi_table* dispatch ) {
	
	/* assert(dispatch); */
	if (!dispatch)
		return;

	memset(dispatch,0,sizeof(struct _glapi_table));

	dispatch->NewList =                      trNewList;                    /* 0 */
	dispatch->EndList =                      trEndList;                    /* 1 */
	dispatch->CallList =                     trCallList;                   /* 2 */
	dispatch->CallLists =                    trCallLists;                  /* 3 */
	dispatch->DeleteLists =                  trDeleteLists;                /* 4 */
	dispatch->GenLists =                     trGenLists;                   /* 5 */
	dispatch->ListBase =                     trListBase;                   /* 6 */
	dispatch->Begin =                        trBegin;                      /* 7 */
	dispatch->Bitmap =                       trBitmap;                     /* 8 */
	dispatch->Color3b =                      trColor3b;                    /* 9 */
	dispatch->Color3bv =                     trColor3bv;                  /* 10 */
	dispatch->Color3d =                      trColor3d;                   /* 11 */
	dispatch->Color3dv =                     trColor3dv;                  /* 12 */
	dispatch->Color3f =                      trColor3f;                   /* 13 */
	dispatch->Color3fv =                     trColor3fv;                  /* 14 */
	dispatch->Color3i =                      trColor3i;                   /* 15 */
	dispatch->Color3iv =                     trColor3iv;                  /* 16 */
	dispatch->Color3s =                      trColor3s;                   /* 17 */
	dispatch->Color3sv =                     trColor3sv;                  /* 18 */
	dispatch->Color3ub =                     trColor3ub;                  /* 19 */
	dispatch->Color3ubv =                    trColor3ubv;                 /* 20 */
	dispatch->Color3ui =                     trColor3ui;                  /* 21 */
	dispatch->Color3uiv =                    trColor3uiv;                 /* 22 */
	dispatch->Color3us =                     trColor3us;                  /* 23 */
	dispatch->Color3usv =                    trColor3usv;                 /* 24 */
	dispatch->Color4b =                      trColor4b;                   /* 25 */
	dispatch->Color4bv =                     trColor4bv;                  /* 26 */
	dispatch->Color4d =                      trColor4d;                   /* 27 */
	dispatch->Color4dv =                     trColor4dv;                  /* 28 */
	dispatch->Color4f =                      trColor4f;                   /* 29 */
	dispatch->Color4fv =                     trColor4fv;                  /* 30 */
	dispatch->Color4i =                      trColor4i;                   /* 31 */
	dispatch->Color4iv =                     trColor4iv;                  /* 32 */
	dispatch->Color4s =                      trColor4s;                   /* 33 */
	dispatch->Color4sv =                     trColor4sv;                  /* 34 */
	dispatch->Color4ub =                     trColor4ub;                  /* 35 */
	dispatch->Color4ubv =                    trColor4ubv;                 /* 36 */
	dispatch->Color4ui =                     trColor4ui;                  /* 37 */
	dispatch->Color4uiv =                    trColor4uiv;                 /* 38 */
	dispatch->Color4us =                     trColor4us;                  /* 39 */
	dispatch->Color4usv =                    trColor4usv;                 /* 40 */
	dispatch->EdgeFlag =                     trEdgeFlag;                  /* 41 */
	dispatch->EdgeFlagv =                    trEdgeFlagv;                 /* 42 */
	dispatch->End =                          trEnd;                       /* 43 */
	dispatch->Indexd =                       trIndexd;                    /* 44 */
	dispatch->Indexdv =                      trIndexdv;                   /* 45 */
	dispatch->Indexf =                       trIndexf;                    /* 46 */
	dispatch->Indexfv =                      trIndexfv;                   /* 47 */
	dispatch->Indexi =                       trIndexi;                    /* 48 */
	dispatch->Indexiv =                      trIndexiv;                   /* 49 */
	dispatch->Indexs =                       trIndexs;                    /* 50 */
	dispatch->Indexsv =                      trIndexsv;                   /* 51 */
	dispatch->Normal3b =                     trNormal3b;                  /* 52 */
	dispatch->Normal3bv =                    trNormal3bv;                 /* 53 */
	dispatch->Normal3d =                     trNormal3d;                  /* 54 */
	dispatch->Normal3dv =                    trNormal3dv;                 /* 55 */
	dispatch->Normal3f =                     trNormal3f;                  /* 56 */
	dispatch->Normal3fv =                    trNormal3fv;                 /* 57 */
	dispatch->Normal3i =                     trNormal3i;                  /* 58 */
	dispatch->Normal3iv =                    trNormal3iv;                 /* 59 */
	dispatch->Normal3s =                     trNormal3s;                  /* 60 */
	dispatch->Normal3sv =                    trNormal3sv;                 /* 61 */
	dispatch->RasterPos2d =                  trRasterPos2d;               /* 62 */
	dispatch->RasterPos2dv =                 trRasterPos2dv;              /* 63 */
	dispatch->RasterPos2f =                  trRasterPos2f;               /* 64 */
	dispatch->RasterPos2fv =                 trRasterPos2fv;              /* 65 */
	dispatch->RasterPos2i =                  trRasterPos2i;               /* 66 */
	dispatch->RasterPos2iv =                 trRasterPos2iv;              /* 67 */
	dispatch->RasterPos2s =                  trRasterPos2s;               /* 68 */
	dispatch->RasterPos2sv =                 trRasterPos2sv;              /* 69 */
	dispatch->RasterPos3d =                  trRasterPos3d;               /* 70 */
	dispatch->RasterPos3dv =                 trRasterPos3dv;              /* 71 */
	dispatch->RasterPos3f =                  trRasterPos3f;               /* 72 */
	dispatch->RasterPos3fv =                 trRasterPos3fv;              /* 73 */
	dispatch->RasterPos3i =                  trRasterPos3i;               /* 74 */
	dispatch->RasterPos3iv =                 trRasterPos3iv;              /* 75 */
	dispatch->RasterPos3s =                  trRasterPos3s;               /* 76 */
	dispatch->RasterPos3sv =                 trRasterPos3sv;              /* 77 */
	dispatch->RasterPos4d =                  trRasterPos4d;               /* 78 */
	dispatch->RasterPos4dv =                 trRasterPos4dv;              /* 79 */
	dispatch->RasterPos4f =                  trRasterPos4f;               /* 80 */
	dispatch->RasterPos4fv =                 trRasterPos4fv;              /* 81 */
	dispatch->RasterPos4i =                  trRasterPos4i;               /* 82 */
	dispatch->RasterPos4iv =                 trRasterPos4iv;              /* 83 */
	dispatch->RasterPos4s =                  trRasterPos4s;               /* 84 */
	dispatch->RasterPos4sv =                 trRasterPos4sv;              /* 85 */
	dispatch->Rectd =                        trRectd;                     /* 86 */
	dispatch->Rectdv =                       trRectdv;                    /* 87 */
	dispatch->Rectf =                        trRectf;                     /* 88 */
	dispatch->Rectfv =                       trRectfv;                    /* 89 */
	dispatch->Recti =                        trRecti;                     /* 90 */
	dispatch->Rectiv =                       trRectiv;                    /* 91 */
	dispatch->Rects =                        trRects;                     /* 92 */
	dispatch->Rectsv =                       trRectsv;                    /* 93 */
	dispatch->TexCoord1d =                   trTexCoord1d;                /* 94 */
	dispatch->TexCoord1dv =                  trTexCoord1dv;               /* 95 */
	dispatch->TexCoord1f =                   trTexCoord1f;                /* 96 */
	dispatch->TexCoord1fv =                  trTexCoord1fv;               /* 97 */
	dispatch->TexCoord1i =                   trTexCoord1i;                /* 98 */
	dispatch->TexCoord1iv =                  trTexCoord1iv;               /* 99 */
	dispatch->TexCoord1s =                   trTexCoord1s;               /* 100 */
	dispatch->TexCoord1sv =                  trTexCoord1sv;              /* 101 */
	dispatch->TexCoord2d =                   trTexCoord2d;               /* 102 */
	dispatch->TexCoord2dv =                  trTexCoord2dv;              /* 103 */
	dispatch->TexCoord2f =                   trTexCoord2f;               /* 104 */
	dispatch->TexCoord2fv =                  trTexCoord2fv;              /* 105 */
	dispatch->TexCoord2i =                   trTexCoord2i;               /* 106 */
	dispatch->TexCoord2iv =                  trTexCoord2iv;              /* 107 */
	dispatch->TexCoord2s =                   trTexCoord2s;               /* 108 */
	dispatch->TexCoord2sv =                  trTexCoord2sv;              /* 109 */
	dispatch->TexCoord3d =                   trTexCoord3d;               /* 110 */
	dispatch->TexCoord3dv =                  trTexCoord3dv;              /* 111 */
	dispatch->TexCoord3f =                   trTexCoord3f;               /* 112 */
	dispatch->TexCoord3fv =                  trTexCoord3fv;              /* 113 */
	dispatch->TexCoord3i =                   trTexCoord3i;               /* 114 */
	dispatch->TexCoord3iv =                  trTexCoord3iv;              /* 115 */
	dispatch->TexCoord3s =                   trTexCoord3s;               /* 116 */
	dispatch->TexCoord3sv =                  trTexCoord3sv;              /* 117 */
	dispatch->TexCoord4d =                   trTexCoord4d;               /* 118 */
	dispatch->TexCoord4dv =                  trTexCoord4dv;              /* 119 */
	dispatch->TexCoord4f =                   trTexCoord4f;               /* 120 */
	dispatch->TexCoord4fv =                  trTexCoord4fv;              /* 121 */
	dispatch->TexCoord4i =                   trTexCoord4i;               /* 122 */
	dispatch->TexCoord4iv =                  trTexCoord4iv;              /* 123 */
	dispatch->TexCoord4s =                   trTexCoord4s;               /* 124 */
	dispatch->TexCoord4sv =                  trTexCoord4sv;              /* 125 */
	dispatch->Vertex2d =                     trVertex2d;                 /* 126 */
	dispatch->Vertex2dv =                    trVertex2dv;                /* 127 */
	dispatch->Vertex2f =                     trVertex2f;                 /* 128 */
	dispatch->Vertex2fv =                    trVertex2fv;                /* 129 */
	dispatch->Vertex2i =                     trVertex2i;                 /* 130 */
	dispatch->Vertex2iv =                    trVertex2iv;                /* 131 */
	dispatch->Vertex2s =                     trVertex2s;                 /* 132 */
	dispatch->Vertex2sv =                    trVertex2sv;                /* 133 */
	dispatch->Vertex3d =                     trVertex3d;                 /* 134 */
	dispatch->Vertex3dv =                    trVertex3dv;                /* 135 */
	dispatch->Vertex3f =                     trVertex3f;                 /* 136 */
	dispatch->Vertex3fv =                    trVertex3fv;                /* 137 */
	dispatch->Vertex3i =                     trVertex3i;                 /* 138 */
	dispatch->Vertex3iv =                    trVertex3iv;                /* 139 */
	dispatch->Vertex3s =                     trVertex3s;                 /* 140 */
	dispatch->Vertex3sv =                    trVertex3sv;                /* 141 */
	dispatch->Vertex4d =                     trVertex4d;                 /* 142 */
	dispatch->Vertex4dv =                    trVertex4dv;                /* 143 */
	dispatch->Vertex4f =                     trVertex4f;                 /* 144 */
	dispatch->Vertex4fv =                    trVertex4fv;                /* 145 */
	dispatch->Vertex4i =                     trVertex4i;                 /* 146 */
	dispatch->Vertex4iv =                    trVertex4iv;                /* 147 */
	dispatch->Vertex4s =                     trVertex4s;                 /* 148 */
	dispatch->Vertex4sv =                    trVertex4sv;                /* 149 */
	dispatch->ClipPlane =                    trClipPlane;                /* 150 */
	dispatch->ColorMaterial =                trColorMaterial;            /* 151 */
	dispatch->CullFace =                     trCullFace;                 /* 152 */
	dispatch->Fogf =                         trFogf;                     /* 153 */
	dispatch->Fogfv =                        trFogfv;                    /* 154 */
	dispatch->Fogi =                         trFogi;                     /* 155 */
	dispatch->Fogiv =                        trFogiv;                    /* 156 */
	dispatch->FrontFace =                    trFrontFace;                /* 157 */
	dispatch->Hint =                         trHint;                     /* 158 */
	dispatch->Lightf =                       trLightf;                   /* 159 */
	dispatch->Lightfv =                      trLightfv;                  /* 160 */
	dispatch->Lighti =                       trLighti;                   /* 161 */
	dispatch->Lightiv =                      trLightiv;                  /* 162 */
	dispatch->LightModelf =                  trLightModelf;              /* 163 */
	dispatch->LightModelfv =                 trLightModelfv;             /* 164 */
	dispatch->LightModeli =                  trLightModeli;              /* 165 */
	dispatch->LightModeliv =                 trLightModeliv;             /* 166 */
	dispatch->LineStipple =                  trLineStipple;              /* 167 */
	dispatch->LineWidth =                    trLineWidth;                /* 168 */
	dispatch->Materialf =                    trMaterialf;                /* 169 */
	dispatch->Materialfv =                   trMaterialfv;               /* 170 */
	dispatch->Materiali =                    trMateriali;                /* 171 */
	dispatch->Materialiv =                   trMaterialiv;               /* 172 */
	dispatch->PointSize =                    trPointSize;                /* 173 */
	dispatch->PolygonMode =                  trPolygonMode;              /* 174 */
	dispatch->PolygonStipple =               trPolygonStipple;           /* 175 */
	dispatch->Scissor =                      trScissor;                  /* 176 */
	dispatch->ShadeModel =                   trShadeModel;               /* 177 */
	dispatch->TexParameterf =                trTexParameterf;            /* 178 */
	dispatch->TexParameterfv =               trTexParameterfv;           /* 179 */
	dispatch->TexParameteri =                trTexParameteri;            /* 180 */
	dispatch->TexParameteriv =               trTexParameteriv;           /* 181 */
	dispatch->TexImage1D =                   trTexImage1D;               /* 182 */
	dispatch->TexImage2D =                   trTexImage2D;               /* 183 */
	dispatch->TexEnvf =                      trTexEnvf;                  /* 184 */
	dispatch->TexEnvfv =                     trTexEnvfv;                 /* 185 */
	dispatch->TexEnvi =                      trTexEnvi;                  /* 186 */
	dispatch->TexEnviv =                     trTexEnviv;                 /* 187 */
	dispatch->TexGend =                      trTexGend;                  /* 188 */
	dispatch->TexGendv =                     trTexGendv;                 /* 189 */
	dispatch->TexGenf =                      trTexGenf;                  /* 190 */
	dispatch->TexGenfv =                     trTexGenfv;                 /* 191 */
	dispatch->TexGeni =                      trTexGeni;                  /* 192 */
	dispatch->TexGeniv =                     trTexGeniv;                 /* 193 */
	dispatch->FeedbackBuffer =               trFeedbackBuffer;           /* 194 */
	dispatch->SelectBuffer =                 trSelectBuffer;             /* 195 */
	dispatch->RenderMode =                   trRenderMode;               /* 196 */
	dispatch->InitNames =                    trInitNames;                /* 197 */
	dispatch->LoadName =                     trLoadName;                 /* 198 */
	dispatch->PassThrough =                  trPassThrough;              /* 199 */
	dispatch->PopName =                      trPopName;                  /* 200 */
	dispatch->PushName =                     trPushName;                 /* 201 */
	dispatch->DrawBuffer =                   trDrawBuffer;               /* 202 */
	dispatch->Clear =                        trClear;                    /* 203 */
	dispatch->ClearAccum =                   trClearAccum;               /* 204 */
	dispatch->ClearIndex =                   trClearIndex;               /* 205 */
	dispatch->ClearColor =                   trClearColor;               /* 206 */
	dispatch->ClearStencil =                 trClearStencil;             /* 207 */
	dispatch->ClearDepth =                   trClearDepth;               /* 208 */
	dispatch->StencilMask =                  trStencilMask;              /* 209 */
	dispatch->ColorMask =                    trColorMask;                /* 210 */
	dispatch->DepthMask =                    trDepthMask;                /* 211 */
	dispatch->IndexMask =                    trIndexMask;                /* 212 */
	dispatch->Accum =                        trAccum;                    /* 213 */
	dispatch->Disable =                      trDisable;                  /* 214 */
	dispatch->Enable =                       trEnable;                   /* 215 */
	dispatch->Finish =                       trFinish;                   /* 216 */
	dispatch->Flush =                        trFlush;                    /* 217 */
	dispatch->PopAttrib =                    trPopAttrib;                /* 218 */
	dispatch->PushAttrib =                   trPushAttrib;               /* 219 */
	dispatch->Map1d =                        trMap1d;                    /* 220 */
	dispatch->Map1f =                        trMap1f;                    /* 221 */
	dispatch->Map2d =                        trMap2d;                    /* 222 */
	dispatch->Map2f =                        trMap2f;                    /* 223 */
	dispatch->MapGrid1d =                    trMapGrid1d;                /* 224 */
	dispatch->MapGrid1f =                    trMapGrid1f;                /* 225 */
	dispatch->MapGrid2d =                    trMapGrid2d;                /* 226 */
	dispatch->MapGrid2f =                    trMapGrid2f;                /* 227 */
	dispatch->EvalCoord1d =                  trEvalCoord1d;              /* 228 */
	dispatch->EvalCoord1dv =                 trEvalCoord1dv;             /* 229 */
	dispatch->EvalCoord1f =                  trEvalCoord1f;              /* 230 */
	dispatch->EvalCoord1fv =                 trEvalCoord1fv;             /* 231 */
	dispatch->EvalCoord2d =                  trEvalCoord2d;              /* 232 */
	dispatch->EvalCoord2dv =                 trEvalCoord2dv;             /* 233 */
	dispatch->EvalCoord2f =                  trEvalCoord2f;              /* 234 */
	dispatch->EvalCoord2fv =                 trEvalCoord2fv;             /* 235 */
	dispatch->EvalMesh1 =                    trEvalMesh1;                /* 236 */
	dispatch->EvalPoint1 =                   trEvalPoint1;               /* 237 */
	dispatch->EvalMesh2 =                    trEvalMesh2;                /* 238 */
	dispatch->EvalPoint2 =                   trEvalPoint2;               /* 239 */
	dispatch->AlphaFunc =                    trAlphaFunc;                /* 240 */
	dispatch->BlendFunc =                    trBlendFunc;                /* 241 */
	dispatch->LogicOp =                      trLogicOp;                  /* 242 */
	dispatch->StencilFunc =                  trStencilFunc;              /* 243 */
	dispatch->StencilOp =                    trStencilOp;                /* 244 */
	dispatch->DepthFunc =                    trDepthFunc;                /* 245 */
	dispatch->PixelZoom =                    trPixelZoom;                /* 246 */
	dispatch->PixelTransferf =               trPixelTransferf;           /* 247 */
	dispatch->PixelTransferi =               trPixelTransferi;           /* 248 */
	dispatch->PixelStoref =                  trPixelStoref;              /* 249 */
	dispatch->PixelStorei =                  trPixelStorei;              /* 250 */
	dispatch->PixelMapfv =                   trPixelMapfv;               /* 251 */
	dispatch->PixelMapuiv =                  trPixelMapuiv;              /* 252 */
	dispatch->PixelMapusv =                  trPixelMapusv;              /* 253 */
	dispatch->ReadBuffer =                   trReadBuffer;               /* 254 */
	dispatch->CopyPixels =                   trCopyPixels;               /* 255 */
	dispatch->ReadPixels =                   trReadPixels;               /* 256 */
	dispatch->DrawPixels =                   trDrawPixels;               /* 257 */
	dispatch->GetBooleanv =                  trGetBooleanv;              /* 258 */
	dispatch->GetClipPlane =                 trGetClipPlane;             /* 259 */
	dispatch->GetDoublev =                   trGetDoublev;               /* 260 */
	dispatch->GetError =                     trGetError;                 /* 261 */
	dispatch->GetFloatv =                    trGetFloatv;                /* 262 */
	dispatch->GetIntegerv =                  trGetIntegerv;              /* 263 */
	dispatch->GetLightfv =                   trGetLightfv;               /* 264 */
	dispatch->GetLightiv =                   trGetLightiv;               /* 265 */
	dispatch->GetMapdv =                     trGetMapdv;                 /* 266 */
	dispatch->GetMapfv =                     trGetMapfv;                 /* 267 */
	dispatch->GetMapiv =                     trGetMapiv;                 /* 268 */
	dispatch->GetMaterialfv =                trGetMaterialfv;            /* 269 */
	dispatch->GetMaterialiv =                trGetMaterialiv;            /* 270 */
	dispatch->GetPixelMapfv =                trGetPixelMapfv;            /* 271 */
	dispatch->GetPixelMapuiv =               trGetPixelMapuiv;           /* 272 */
	dispatch->GetPixelMapusv =               trGetPixelMapusv;           /* 273 */
	dispatch->GetPolygonStipple =            trGetPolygonStipple;        /* 274 */
	dispatch->GetString =                    trGetString;                /* 275 */
	dispatch->GetTexEnvfv =                  trGetTexEnvfv;              /* 276 */
	dispatch->GetTexEnviv =                  trGetTexEnviv;              /* 277 */
	dispatch->GetTexGendv =                  trGetTexGendv;              /* 278 */
	dispatch->GetTexGenfv =                  trGetTexGenfv;              /* 279 */
	dispatch->GetTexGeniv =                  trGetTexGeniv;              /* 280 */
	dispatch->GetTexImage =                  trGetTexImage;              /* 281 */
	dispatch->GetTexParameterfv =            trGetTexParameterfv;        /* 282 */
	dispatch->GetTexParameteriv =            trGetTexParameteriv;        /* 283 */
	dispatch->GetTexLevelParameterfv =       trGetTexLevelParameterfv;   /* 284 */
	dispatch->GetTexLevelParameteriv =       trGetTexLevelParameteriv;   /* 285 */
	dispatch->IsEnabled =                    trIsEnabled;                /* 286 */
	dispatch->IsList =                       trIsList;                   /* 287 */
	dispatch->DepthRange =                   trDepthRange;               /* 288 */
	dispatch->Frustum =                      trFrustum;                  /* 289 */
	dispatch->LoadIdentity =                 trLoadIdentity;             /* 290 */
	dispatch->LoadMatrixf =                  trLoadMatrixf;              /* 291 */
	dispatch->LoadMatrixd =                  trLoadMatrixd;              /* 292 */
	dispatch->MatrixMode =                   trMatrixMode;               /* 293 */
	dispatch->MultMatrixf =                  trMultMatrixf;              /* 294 */
	dispatch->MultMatrixd =                  trMultMatrixd;              /* 295 */
	dispatch->Ortho =                        trOrtho;                    /* 296 */
	dispatch->PopMatrix =                    trPopMatrix;                /* 297 */
	dispatch->PushMatrix =                   trPushMatrix;               /* 298 */
	dispatch->Rotated =                      trRotated;                  /* 299 */
	dispatch->Rotatef =                      trRotatef;                  /* 300 */
	dispatch->Scaled =                       trScaled;                   /* 301 */
	dispatch->Scalef =                       trScalef;                   /* 302 */
	dispatch->Translated =                   trTranslated;               /* 303 */
	dispatch->Translatef =                   trTranslatef;               /* 304 */
	dispatch->Viewport =                     trViewport;                 /* 305 */
	dispatch->ArrayElement =                 trArrayElement;             /* 306 */
	dispatch->BindTexture =                  trBindTexture;              /* 307 */
	dispatch->ColorPointer =                 trColorPointer;             /* 308 */
	dispatch->DisableClientState =           trDisableClientState;       /* 309 */
	dispatch->DrawArrays =                   trDrawArrays;               /* 310 */
	dispatch->DrawElements =                 trDrawElements;             /* 311 */
	dispatch->EdgeFlagPointer =              trEdgeFlagPointer;          /* 312 */
	dispatch->EnableClientState =            trEnableClientState;        /* 313 */
	dispatch->IndexPointer =                 trIndexPointer;             /* 314 */
	dispatch->Indexub =                      trIndexub;                  /* 315 */
	dispatch->Indexubv =                     trIndexubv;                 /* 316 */
	dispatch->InterleavedArrays =            trInterleavedArrays;        /* 317 */
	dispatch->NormalPointer =                trNormalPointer;            /* 318 */
	dispatch->PolygonOffset =                trPolygonOffset;            /* 319 */
	dispatch->TexCoordPointer =              trTexCoordPointer;          /* 320 */
	dispatch->VertexPointer =                trVertexPointer;            /* 321 */
	dispatch->AreTexturesResident =          trAreTexturesResident;      /* 322 */
	dispatch->CopyTexImage1D =               trCopyTexImage1D;           /* 323 */
	dispatch->CopyTexImage2D =               trCopyTexImage2D;           /* 324 */
	dispatch->CopyTexSubImage1D =            trCopyTexSubImage1D;        /* 325 */
	dispatch->CopyTexSubImage2D =            trCopyTexSubImage2D;        /* 326 */
	dispatch->DeleteTextures =               trDeleteTextures;           /* 327 */
	dispatch->GenTextures =                  trGenTextures;              /* 328 */
	dispatch->GetPointerv =                  trGetPointerv;              /* 329 */
	dispatch->IsTexture =                    trIsTexture;                /* 330 */
	dispatch->PrioritizeTextures =           trPrioritizeTextures;       /* 331 */
	dispatch->TexSubImage1D =                trTexSubImage1D;            /* 332 */
	dispatch->TexSubImage2D =                trTexSubImage2D;            /* 333 */
	dispatch->PopClientAttrib =              trPopClientAttrib;          /* 334 */
	dispatch->PushClientAttrib =             trPushClientAttrib;         /* 335 */
#if 1
	dispatch->BlendColor =                   trBlendColor;               /* 336 */
	dispatch->BlendEquation =                trBlendEquation;            /* 337 */
	dispatch->DrawRangeElements =            trDrawRangeElements;        /* 338 */
	dispatch->ColorTable =                   trColorTable;               /* 339 */
	dispatch->ColorTableParameterfv =        trColorTableParameterfv;    /* 340 */
	dispatch->ColorTableParameteriv =        trColorTableParameteriv;    /* 341 */
	dispatch->CopyColorTable =               trCopyColorTable;           /* 342 */
	dispatch->GetColorTable =                trGetColorTable;            /* 343 */
	dispatch->GetColorTableParameterfv =     trGetColorTableParameterfv; /* 344 */
	dispatch->GetColorTableParameteriv =     trGetColorTableParameteriv; /* 345 */
	dispatch->ColorSubTable =                trColorSubTable;            /* 346 */
	dispatch->CopyColorSubTable =            trCopyColorSubTable;        /* 347 */
	dispatch->ConvolutionFilter1D =          trConvolutionFilter1D;      /* 348 */
	dispatch->ConvolutionFilter2D =          trConvolutionFilter2D;      /* 349 */
	dispatch->ConvolutionParameterf =        trConvolutionParameterf;    /* 350 */
	dispatch->ConvolutionParameterfv =       trConvolutionParameterfv;   /* 351 */
	dispatch->ConvolutionParameteri =        trConvolutionParameteri;    /* 352 */
	dispatch->ConvolutionParameteriv =       trConvolutionParameteriv;   /* 353 */
	dispatch->CopyConvolutionFilter1D =      trCopyConvolutionFilter1D;  /* 354 */
	dispatch->CopyConvolutionFilter2D =      trCopyConvolutionFilter2D;  /* 355 */
	dispatch->GetConvolutionFilter =         trGetConvolutionFilter;     /* 356 */
	dispatch->GetConvolutionParameterfv =    trGetConvolutionParameterfv;/* 357 */
	dispatch->GetConvolutionParameteriv =    trGetConvolutionParameteriv;/* 358 */
	dispatch->GetSeparableFilter =           trGetSeparableFilter;       /* 359 */
	dispatch->SeparableFilter2D =            trSeparableFilter2D;        /* 360 */
	dispatch->GetHistogram =                 trGetHistogram;             /* 361 */
	dispatch->GetHistogramParameterfv =      trGetHistogramParameterfv;  /* 362 */
	dispatch->GetHistogramParameteriv =      trGetHistogramParameteriv;  /* 363 */
	dispatch->GetMinmax =                    trGetMinmax;                /* 364 */
	dispatch->GetMinmaxParameterfv =         trGetMinmaxParameterfv;     /* 365 */
	dispatch->GetMinmaxParameteriv =         trGetMinmaxParameteriv;     /* 366 */
#endif
#if 0
	dispatch->Histogram =                    trHistogram                 /* 367 */
	dispatch->Minmax =                       trMinmax                    /* 368 */
	dispatch->ResetHistogram =               trResetHistogram            /* 369 */
	dispatch->ResetMinmax =                  trResetMinmax               /* 370 */
	dispatch->TexImage3D =                   trTexImage3D                /* 371 */
	dispatch->TexSubImage3D =                trTexSubImage3D             /* 372 */
	dispatch->CopyTexSubImage3D =            trCopyTexSubImage3D         /* 373 */
	dispatch->ActiveTextureARB =             trActiveTextureARB          /* 374 */
	dispatch->ClientActiveTextureARB =       trClientActiveTextureARB    /* 375 */
	dispatch->MultiTexCoord1dARB =           trMultiTexCoord1dARB        /* 376 */
	dispatch->MultiTexCoord1dvARB =          trMultiTexCoord1dvARB       /* 377 */
	dispatch->MultiTexCoord1fARB =           trMultiTexCoord1fARB        /* 378 */
	dispatch->MultiTexCoord1fvARB =          trMultiTexCoord1fvARB       /* 379 */
	dispatch->MultiTexCoord1iARB =           trMultiTexCoord1iARB        /* 380 */
	dispatch->MultiTexCoord1ivARB =          trMultiTexCoord1ivARB       /* 381 */
	dispatch->MultiTexCoord1sARB =           trMultiTexCoord1sARB        /* 382 */
	dispatch->MultiTexCoord1svARB =          trMultiTexCoord1svARB       /* 383 */
	dispatch->MultiTexCoord2dARB =           trMultiTexCoord2dARB        /* 384 */
	dispatch->MultiTexCoord2dvARB =          trMultiTexCoord2dvARB       /* 385 */
	dispatch->MultiTexCoord2fARB =           trMultiTexCoord2fARB        /* 386 */
	dispatch->MultiTexCoord2fvARB =          trMultiTexCoord2fvARB       /* 387 */
	dispatch->MultiTexCoord2iARB =           trMultiTexCoord2iARB        /* 388 */
	dispatch->MultiTexCoord2ivARB =          trMultiTexCoord2ivARB       /* 389 */
	dispatch->MultiTexCoord2sARB =           trMultiTexCoord2sARB        /* 390 */
	dispatch->MultiTexCoord2svARB =          trMultiTexCoord2svARB       /* 391 */
	dispatch->MultiTexCoord3dARB =           trMultiTexCoord3dARB        /* 392 */
	dispatch->MultiTexCoord3dvARB =          trMultiTexCoord3dvARB       /* 393 */
	dispatch->MultiTexCoord3fARB =           trMultiTexCoord3fARB        /* 394 */
	dispatch->MultiTexCoord3fvARB =          trMultiTexCoord3fvARB       /* 395 */
	dispatch->MultiTexCoord3iARB =           trMultiTexCoord3iARB        /* 396 */
	dispatch->MultiTexCoord3ivARB =          trMultiTexCoord3ivARB       /* 397 */
	dispatch->MultiTexCoord3sARB =           trMultiTexCoord3sARB        /* 398 */
	dispatch->MultiTexCoord3svARB =          trMultiTexCoord3svARB       /* 399 */
	dispatch->MultiTexCoord4dARB =           trMultiTexCoord4dARB        /* 400 */
	dispatch->MultiTexCoord4dvARB =          trMultiTexCoord4dvARB       /* 401 */
	dispatch->MultiTexCoord4fARB =           trMultiTexCoord4fARB        /* 402 */
	dispatch->MultiTexCoord4fvARB =          trMultiTexCoord4fvARB       /* 403 */
	dispatch->MultiTexCoord4iARB =           trMultiTexCoord4iARB        /* 404 */
	dispatch->MultiTexCoord4ivARB =          trMultiTexCoord4ivARB       /* 405 */
	dispatch->MultiTexCoord4sARB =           trMultiTexCoord4sARB        /* 406 */
	dispatch->MultiTexCoord4svARB =          trMultiTexCoord4svARB       /* 407 */
	dispatch->LoadTransposeMatrixfARB =      trLoadTransposeMatrixfARB   /* 408 */
	dispatch->LoadTransposeMatrixdARB =      trLoadTransposeMatrixdARB   /* 409 */
	dispatch->MultTransposeMatrixfARB =      trMultTransposeMatrixfARB   /* 410 */
	dispatch->MultTransposeMatrixdARB =      trMultTransposeMatrixdARB   /* 411 */
	dispatch->SampleCoverageARB =            trSampleCoverageARB         /* 412 */
	dispatch->SamplePassARB =                trSamplePassARB             /* 413 */
	dispatch->PolygonOffsetEXT =             trPolygonOffsetEXT          /* 414 */
	dispatch->GetTexFilterFuncSGIS =         trGetTexFilterFuncSGIS      /* 415 */
	dispatch->TexFilterFuncSGIS =            trTexFilterFuncSGIS         /* 416 */
	dispatch->GetHistogramEXT =              trGetHistogramEXT           /* 417 */
	dispatch->GetHistogramParameterfvEXT =   trGetHistogramParameterfvEXT /* 418 */
	dispatch->GetHistogramParameterivEXT =   trGetHistogramParameterivEXT /* 419 */
	dispatch->GetMinmaxEXT =                 trGetMinmaxEXT              /* 420 */
	dispatch->GetMinmaxParameterfvEXT =      trGetMinmaxParameterfvEXT   /* 421 */
	dispatch->GetMinmaxParameterivEXT =      trGetMinmaxParameterivEXT   /* 422 */
	dispatch->GetConvolutionFilterEXT =      trGetConvolutionFilterEXT   /* 423 */
	dispatch->GetConvolutionParameterfvEXT = trGetConvolutionParameterfvEXT /* 424 */
	dispatch->GetConvolutionParameterivEXT = trGetConvolutionParameterivEXT /* 425 */
	dispatch->GetSeparableFilterEXT =        trGetSeparableFilterEXT     /* 426 */
	dispatch->GetColorTableSGI =             trGetColorTableSGI          /* 427 */
	dispatch->GetColorTableParameterfvSGI =  trGetColorTableParameterfvSGI /* 428 */
	dispatch->GetColorTableParameterivSGI =  trGetColorTableParameterivSGI /* 429 */
	dispatch->PixelTexGenSGIX =              trPixelTexGenSGIX           /* 430 */
	dispatch->PixelTexGenParameteriSGIS =    trPixelTexGenParameteriSGIS /* 431 */
	dispatch->PixelTexGenParameterivSGIS =   trPixelTexGenParameterivSGIS /* 432 */
	dispatch->PixelTexGenParameterfSGIS =    trPixelTexGenParameterfSGIS /* 433 */
	dispatch->PixelTexGenParameterfvSGIS =   trPixelTexGenParameterfvSGIS /* 434 */
	dispatch->GetPixelTexGenParameterivSGIS = trGetPixelTexGenParameterivSGIS /* 435 */
	dispatch->GetPixelTexGenParameterfvSGIS = trGetPixelTexGenParameterfvSGIS /* 436 */
	dispatch->TexImage4DSGIS =               trTexImage4DSGIS            /* 437 */
	dispatch->TexSubImage4DSGIS =            trTexSubImage4DSGIS         /* 438 */
	dispatch->AreTexturesResidentEXT =       trAreTexturesResidentEXT    /* 439 */
	dispatch->GenTexturesEXT =               trGenTexturesEXT            /* 440 */
	dispatch->IsTextureEXT =                 trIsTextureEXT              /* 441 */
	dispatch->DetailTexFuncSGIS =            trDetailTexFuncSGIS         /* 442 */
	dispatch->GetDetailTexFuncSGIS =         trGetDetailTexFuncSGIS      /* 443 */
	dispatch->SharpenTexFuncSGIS =           trSharpenTexFuncSGIS        /* 444 */
	dispatch->GetSharpenTexFuncSGIS =        trGetSharpenTexFuncSGIS     /* 445 */
	dispatch->SampleMaskSGIS =               trSampleMaskSGIS            /* 446 */
	dispatch->SamplePatternSGIS =            trSamplePatternSGIS         /* 447 */
	dispatch->ColorPointerEXT =              trColorPointerEXT           /* 448 */
	dispatch->EdgeFlagPointerEXT =           trEdgeFlagPointerEXT        /* 449 */
	dispatch->IndexPointerEXT =              trIndexPointerEXT           /* 450 */
	dispatch->NormalPointerEXT =             trNormalPointerEXT          /* 451 */
	dispatch->TexCoordPointerEXT =           trTexCoordPointerEXT        /* 452 */
	dispatch->VertexPointerEXT =             trVertexPointerEXT          /* 453 */
	dispatch->SpriteParameterfSGIX =         trSpriteParameterfSGIX      /* 454 */
	dispatch->SpriteParameterfvSGIX =        trSpriteParameterfvSGIX     /* 455 */
	dispatch->SpriteParameteriSGIX =         trSpriteParameteriSGIX      /* 456 */
	dispatch->SpriteParameterivSGIX =        trSpriteParameterivSGIX     /* 457 */
	dispatch->PointParameterfEXT =           trPointParameterfEXT        /* 458 */
	dispatch->PointParameterfvEXT =          trPointParameterfvEXT       /* 459 */
	dispatch->GetInstrumentsSGIX =           trGetInstrumentsSGIX        /* 460 */
	dispatch->InstrumentsBufferSGIX =        trInstrumentsBufferSGIX     /* 461 */
	dispatch->PollInstrumentsSGIX =          trPollInstrumentsSGIX       /* 462 */
	dispatch->ReadInstrumentsSGIX =          trReadInstrumentsSGIX       /* 463 */
	dispatch->StartInstrumentsSGIX =         trStartInstrumentsSGIX      /* 464 */
	dispatch->StopInstrumentsSGIX =          trStopInstrumentsSGIX       /* 465 */
	dispatch->FrameZoomSGIX =                trFrameZoomSGIX             /* 466 */
	dispatch->TagSampleBufferSGIX =          trTagSampleBufferSGIX       /* 467 */
	dispatch->ReferencePlaneSGIX =           trReferencePlaneSGIX        /* 468 */
	dispatch->FlushRasterSGIX =              trFlushRasterSGIX           /* 469 */
	dispatch->GetListParameterfvSGIX =       trGetListParameterfvSGIX    /* 470 */
	dispatch->GetListParameterivSGIX =       trGetListParameterivSGIX    /* 471 */
	dispatch->ListParameterfSGIX =           trListParameterfSGIX        /* 472 */
	dispatch->ListParameterfvSGIX =          trListParameterfvSGIX       /* 473 */
	dispatch->ListParameteriSGIX =           trListParameteriSGIX        /* 474 */
	dispatch->ListParameterivSGIX =          trListParameterivSGIX       /* 475 */
	dispatch->FragmentColorMaterialSGIX =    trFragmentColorMaterialSGIX /* 476 */
	dispatch->FragmentLightfSGIX =           trFragmentLightfSGIX        /* 477 */
	dispatch->FragmentLightfvSGIX =          trFragmentLightfvSGIX       /* 478 */
	dispatch->FragmentLightiSGIX =           trFragmentLightiSGIX        /* 479 */
	dispatch->FragmentLightivSGIX =          trFragmentLightivSGIX       /* 480 */
	dispatch->FragmentLightModelfSGIX =      trFragmentLightModelfSGIX   /* 481 */
	dispatch->FragmentLightModelfvSGIX =     trFragmentLightModelfvSGIX  /* 482 */
	dispatch->FragmentLightModeliSGIX =      trFragmentLightModeliSGIX   /* 483 */
	dispatch->FragmentLightModelivSGIX =     trFragmentLightModelivSGIX  /* 484 */
	dispatch->FragmentMaterialfSGIX =        trFragmentMaterialfSGIX     /* 485 */
	dispatch->FragmentMaterialfvSGIX =       trFragmentMaterialfvSGIX    /* 486 */
	dispatch->FragmentMaterialiSGIX =        trFragmentMaterialiSGIX     /* 487 */
	dispatch->FragmentMaterialivSGIX =       trFragmentMaterialivSGIX    /* 488 */
	dispatch->GetFragmentLightfvSGIX =       trGetFragmentLightfvSGIX    /* 489 */
	dispatch->GetFragmentLightivSGIX =       trGetFragmentLightivSGIX    /* 490 */
	dispatch->GetFragmentMaterialfvSGIX =    trGetFragmentMaterialfvSGIX /* 491 */
	dispatch->GetFragmentMaterialivSGIX =    trGetFragmentMaterialivSGIX /* 492 */
	dispatch->LightEnviSGIX =                trLightEnviSGIX             /* 493 */
	dispatch->VertexWeightfEXT =             trVertexWeightfEXT          /* 494 */
	dispatch->VertexWeightfvEXT =            trVertexWeightfvEXT         /* 495 */
	dispatch->VertexWeightPointerEXT =       trVertexWeightPointerEXT    /* 496 */
	dispatch->FlushVertexArrayRangeNV =      trFlushVertexArrayRangeNV   /* 497 */
	dispatch->VertexArrayRangeNV =           trVertexArrayRangeNV        /* 498 */
	dispatch->CombinerParameterfvNV =        trCombinerParameterfvNV     /* 499 */
	dispatch->CombinerParameterfNV =         trCombinerParameterfNV      /* 500 */
	dispatch->CombinerParameterivNV =        trCombinerParameterivNV     /* 501 */
	dispatch->CombinerParameteriNV =         trCombinerParameteriNV      /* 502 */
	dispatch->CombinerInputNV =              trCombinerInputNV           /* 503 */
	dispatch->CombinerOutputNV =             trCombinerOutputNV          /* 504 */
	dispatch->FinalCombinerInputNV =         trFinalCombinerInputNV      /* 505 */
	dispatch->GetCombinerInputParameterfvNV = trGetCombinerInputParameterfvNV /* 506 */
	dispatch->GetCombinerInputParameterivNV = trGetCombinerInputParameterivNV /* 507 */
	dispatch->GetCombinerOutputParameterfvNV = trGetCombinerOutputParameterfvNV /* 508 */
	dispatch->GetCombinerOutputParameterivNV = trGetCombinerOutputParameterivNV /* 509 */
	dispatch->GetFinalCombinerInputParameterfvNV = trGetFinalCombinerInputParameterfvNV /* 510 */
	dispatch->GetFinalCombinerInputParameterivNV = trGetFinalCombinerInputParameterivNV /* 511 */
	dispatch->ResizeBuffersMESA =            trResizeBuffersMESA         /* 512 */
	dispatch->WindowPos2dMESA =              trWindowPos2dMESA           /* 513 */
	dispatch->WindowPos2dvMESA =             trWindowPos2dvMESA          /* 514 */
	dispatch->WindowPos2fMESA =              trWindowPos2fMESA           /* 515 */
	dispatch->WindowPos2fvMESA =             trWindowPos2fvMESA          /* 516 */
	dispatch->WindowPos2iMESA =              trWindowPos2iMESA           /* 517 */
	dispatch->WindowPos2ivMESA =             trWindowPos2ivMESA          /* 518 */
	dispatch->WindowPos2sMESA =              trWindowPos2sMESA           /* 519 */
	dispatch->WindowPos2svMESA =             trWindowPos2svMESA          /* 520 */
	dispatch->WindowPos3dMESA =              trWindowPos3dMESA           /* 521 */
	dispatch->WindowPos3dvMESA =             trWindowPos3dvMESA          /* 522 */
	dispatch->WindowPos3fMESA =              trWindowPos3fMESA           /* 523 */
	dispatch->WindowPos3fvMESA =             trWindowPos3fvMESA          /* 524 */
	dispatch->WindowPos3iMESA =              trWindowPos3iMESA           /* 525 */
	dispatch->WindowPos3ivMESA =             trWindowPos3ivMESA          /* 526 */
	dispatch->WindowPos3sMESA =              trWindowPos3sMESA           /* 527 */
	dispatch->WindowPos3svMESA =             trWindowPos3svMESA          /* 528 */
	dispatch->WindowPos4dMESA =              trWindowPos4dMESA           /* 529 */
	dispatch->WindowPos4dvMESA =             trWindowPos4dvMESA          /* 530 */
	dispatch->WindowPos4fMESA =              trWindowPos4fMESA           /* 531 */
	dispatch->WindowPos4fvMESA =             trWindowPos4fvMESA          /* 532 */
	dispatch->WindowPos4iMESA =              trWindowPos4iMESA           /* 533 */
	dispatch->WindowPos4ivMESA =             trWindowPos4ivMESA          /* 534 */
	dispatch->WindowPos4sMESA =              trWindowPos4sMESA           /* 535 */
	dispatch->WindowPos4svMESA =             trWindowPos4svMESA          /* 536 */
	dispatch->BlendFuncSeparateEXT =         trBlendFuncSeparateEXT      /* 537 */
	dispatch->IndexMaterialEXT =             trIndexMaterialEXT          /* 538 */
	dispatch->IndexFuncEXT =                 trIndexFuncEXT              /* 539 */
	dispatch->LockArraysEXT =                trLockArraysEXT             /* 540 */
	dispatch->UnlockArraysEXT =              trUnlockArraysEXT           /* 541 */
	dispatch->CullParameterdvEXT =           trCullParameterdvEXT        /* 542 */
	dispatch->CullParameterfvEXT =           trCullParameterfvEXT        /* 543 */
	dispatch->HintPGI =                      trHintPGI                   /* 544 */
	dispatch->FogCoordfEXT =                 trFogCoordfEXT              /* 545 */
	dispatch->FogCoordfvEXT =                trFogCoordfvEXT             /* 546 */
	dispatch->FogCoorddEXT =                 trFogCoorddEXT              /* 547 */
	dispatch->FogCoorddvEXT =                trFogCoorddvEXT             /* 548 */
	dispatch->FogCoordPointerEXT =           trFogCoordPointerEXT        /* 549 */
	dispatch->GetColorTableEXT =             trGetColorTableEXT          /* 550 */
	dispatch->GetColorTableParameterivEXT =  trGetColorTableParameterivEXT /* 551 */
	dispatch->GetColorTableParameterfvEXT =  trGetColorTableParameterfvEXT /* 552 */
#endif
}


#else
extern void tr_wrapper_dummy_func(void);
void tr_wrapper_dummy_func(void)
{
}
#endif
