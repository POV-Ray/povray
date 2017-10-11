#ifndef TR_CONTEXT_H
#define TR_CONTEXT_H

#include "glheader.h"
#include "glapitable.h"

#define TR_MAX_QUEUED_ERRORS 8


/**
 * State Variables.
 * To make this thread-safe we'd maintain such
 *  a set per thread/context.
 */
typedef struct t_t{

	GLboolean                traceEnabled;

	/* The current log file pointer. */
	FILE *                   logFP;      

	char *                   traceName;

	/* What frame we're currently on. */
	GLint                    framecounter;

	/* Not yet used.  This field will control which attributes should be logged. */
	GLbitfield               traceAttribLogBits;

	GLbitfield               traceEnableLogBits;

	/* Used to short-circuit writing to the file, if the disk is full */
	GLboolean                trDoPrint;

	/* Used to toggle whether the gl command should
	 * actually be executed.  For now this will always be true.
	 */
	GLboolean                doExec;
  
	/* Verify GL_NO_ERROR wherever possible. */
	GLboolean                check_errors;

	/* Loop over all GL errors.
	 * TODO: We have to make sure the application gets 
	 *  the exact same errors despite our checking.
	 *  That basically means we have to queue all
	 *  errors from our internal checks.
	 */
	GLint                    head_errors;
	GLint                    tail_errors;

	GLenum                   cached_errors[TR_MAX_QUEUED_ERRORS];

	/* Not yet used.  The 1st of many state flags used to
	 * track the current state of the GL state machine.
	 */
	GLboolean                betweenBeginEnd;

#if 0
	GLboolean                doAsserts;
	GLboolean                clientStateValid;
#endif

} trace_context_t;


extern void trInitContext( trace_context_t * tr_context );

extern trace_context_t*  trCtx( void );

extern struct _glapi_table* trGetDispatch( void );

extern void trSetTraceDispatch( void );

extern void trSetOriginalDispatch( void );

extern GLboolean trDoErrorCheck( void );


#endif
