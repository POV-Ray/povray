#ifdef MESA_TRACE

#include "glheader.h"
#include "glapi.h"
#include "glapitable.h"
#include "context.h"
#include "tr_context.h"


/* Full precision on floats/double, else human readable. */
#define  TR_FULL_PRECISION  0x000000001


void trInitContext( trace_context_t * tr_context )
{
	int i;

	if (!tr_context)
		return;

	tr_context->traceEnabled = GL_FALSE;
	tr_context->logFP = stdout;      
	tr_context->traceName = NULL;

	tr_context->traceAttribLogBits = GL_ALL_ATTRIB_BITS;
	tr_context->traceEnableLogBits = GL_TRACE_ALL_BITS_MESA;

	tr_context->betweenBeginEnd = GL_FALSE;

	tr_context->framecounter = 0;

	tr_context->trDoPrint = GL_TRUE;
	tr_context->doExec = GL_TRUE;
	tr_context->check_errors = GL_TRUE;

	tr_context->head_errors = 0;
	tr_context->tail_errors = 0;

	for( i = 0; i < TR_MAX_QUEUED_ERRORS; i++ ) {
		tr_context->cached_errors[i] = GL_NO_ERROR;
	}

#if 0
	tr_context->doAsserts    = GL_TRUE;
	tr_context->clientStateValid = GL_FALSE;
#endif
}


/**
 * Get the current context.
 */
trace_context_t* trCtx() {
	GLcontext * ctx;
	ctx = (GLcontext *)_glapi_get_context();

	assert(ctx);
	assert(ctx->TraceCtx);
	if( (!ctx) || !(ctx->TraceCtx) ) {
		_mesa_error(ctx, GL_INVALID_OPERATION, __FUNCTION__ );
		return NULL;
	}

	return ctx->TraceCtx;
}


/**
 * Get the current, real dispatch table pointer.
 */
struct _glapi_table* trGetDispatch() {
	return _glapi_get_dispatch();
}


void trSetTraceDispatch( void ) {
	GLcontext * ctx;
	ctx = (GLcontext *)_glapi_get_context();

	assert( ctx );
	assert( ctx->TraceCtx );
	assert( ctx->TraceDispatch );

	ctx->TraceCtx->traceEnabled = GL_TRUE;

        /* XXX save returned value */
	(void) _glapi_begin_dispatch_override(ctx->TraceDispatch);
}


void trSetOriginalDispatch( void ) {
	GLcontext * ctx;
	ctx = (GLcontext *)_glapi_get_context();

	assert( ctx );
	assert( ctx->TraceCtx );
	assert( ctx->TraceDispatch );

	ctx->TraceCtx->traceEnabled = GL_FALSE;

        /* XXX pass value we got from _glapi_begin_dispatch_override() */
	_glapi_end_dispatch_override(1);
}


/**
 * Is error checking enabled?
 */
GLboolean trDoErrorCheck() {
  return trCtx()->check_errors;
}

#else
extern void tr_context_dummy_func(void);
void tr_context_dummy_func(void)
{
}
#endif
