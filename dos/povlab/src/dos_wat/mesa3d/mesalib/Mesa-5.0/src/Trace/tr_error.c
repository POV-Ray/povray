#ifdef MESA_TRACE

#include "tr_context.h"
#include "tr_error.h"
#include "tr_write.h"


/* Have a Begin/End flag, skip checks if in-between. */


/**
 * Some GL implementations cache errors internally,
 *  thus we have to loop until we do not get
 *  any errors.
 */
void trError( void ) {
	int     sanity = 0;  /* Bail out on endless loops. */
	GLenum  err;

	if( !(trCtx()->check_errors) )
		return;

	while ( (err=trGetDispatch()->GetError())!=GL_NO_ERROR ) {
		trWriteEnum(err);
		sanity++;

		if (sanity > TR_MAX_QUEUED_ERRORS ) {
			/* Too many errors */
			return;
		}
	}
}


#else
extern void tr_error_dummy_func(void);
void tr_error_dummy_func(void)
{
}
#endif
