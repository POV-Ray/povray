#ifdef MESA_TRACE
#include "glheader.h"
#include "glapi.h"
#include "context.h" /* for _mesa_error */
#include "mtypes.h"
#include "tr_context.h"
#include "tr_write.h"


void glEnableTraceMESA( GLbitfield mask )
{
	trace_context_t * tctx = trCtx();

	tctx->traceEnableLogBits = mask;
	tctx->traceEnabled       = GL_TRUE;
}


void glDisableTraceMESA( GLbitfield mask )
{
	/* Reset traceEnableLogBits ? */
	trCtx()->traceEnabled = GL_FALSE;
}


void glNewTraceMESA( GLbitfield logbits, const GLubyte * traceName )
{
	char * newname;
	GLint     length;
	GLcontext * ctx;
	const char * defaultName = "traceGL";

	ctx = (GLcontext *)_glapi_get_context();

	assert(ctx);
	assert(ctx->TraceCtx);
	assert(ctx->TraceDispatch);
	if( !ctx ||                                      /* Do we even have a context ? */
		(ctx->TraceCtx->betweenBeginEnd == GL_TRUE)   || /* Are we currently between glBegin and glEnd ? */
	   (ctx->TraceDispatch == _glapi_get_override_dispatch(1)) ) {  /* Has a trace already started ? */
		_mesa_error(ctx, GL_INVALID_OPERATION, __FUNCTION__ );
		return;
	}

	/* FIXME!!! When do we free tracename after the app is finished? */
	if( ctx->TraceCtx->traceName ) {
		free( ctx->TraceCtx->traceName );
	}

	length = strlen((char *)traceName) + 1;
	if( length != 1 ) {
		newname = (char *)malloc( length );
		strncpy( (char *)newname, (char *)traceName, length );
	} else {
		length = strlen( defaultName );
		newname = (char *)malloc( length );
		strncpy( (char *)newname, defaultName, length );
	}
	ctx->TraceCtx->traceName          = newname;
	ctx->TraceCtx->traceAttribLogBits = logbits;

	trOpenLogFile();
	trSetTraceDispatch();
}


void glEndTraceMESA(void)
{
	GLcontext * ctx;

	ctx = (GLcontext *)_glapi_get_context();
	assert(ctx);
	assert(ctx->TraceCtx);
	assert(ctx->TraceDispatch);

        /* Do we even have a context ? */
        /* Are we currently between glBegin and glEnd ? */
        /* Are we sure the current dispatch _is_ the TraceDispatch ? */
	if (!ctx ||
            (ctx->TraceCtx->betweenBeginEnd == GL_TRUE)  ||
	    (ctx->TraceDispatch != _glapi_get_override_dispatch(1)) ) {
		_mesa_error(ctx, GL_INVALID_OPERATION, __FUNCTION__ );
		return;
	}

#if 0
	/* Always dump the max indices */
// But not yet...
	trWriteCMD( VAR_COLORPOINTER );
	trWritei( ctx->TraceCtx->trColorPtrState.maxIndex );
	trWriteCMD( VAR_EDGEFLAGPOINTER );
	trWritei( ctx->TraceCtx->trEdgeFlagPtrState.maxIndex );
	trWriteCMD( VAR_INDEXPOINTER );
	trWritei( ctx->TraceCtx->trIndexPtrState.maxIndex );
	trWriteCMD( VAR_NORMALPOINTER );
	trWritei( ctx->TraceCtx->trNormalPtrState.maxIndex );
	trWriteCMD( VAR_TEXCOORDPOINTER );
	trWritei( ctx->TraceCtx->trTexCoordPtrState.maxIndex );
	trWriteCMD( VAR_VERTEXPOINTER );
	trWritei( ctx->TraceCtx->trVertexPtrState.maxIndex );
#endif

	trCloseLogFile();
	trSetOriginalDispatch();
}


void glTraceAssertAttribMESA( GLbitfield attribMask )
{
#warning TraceAssertAttrib not implemented
}


void glTraceCommentMESA( const GLubyte * comment )
{
	trWriteString( (char *)comment );
}


void glTraceTextureMESA( GLuint name, const GLubyte* comment )
{
#warning TraceTexture not implemented
}

void glTraceListMESA( GLuint name, const GLubyte* comment )
{
#warning TraceList not implemented
}


void glTracePointerMESA( GLvoid* pointer, const GLubyte* comment )
{
#warning TracePointer not implemented
}


void glTracePointerRangeMESA( const GLvoid* first, const GLvoid* last, const GLubyte* comment )
{
#warning TracePointerRange not implemented
}

#else
extern void tr_control_dummy_func(void);
void tr_control_dummy_func(void)
{
}
#endif
