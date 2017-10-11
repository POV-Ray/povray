#ifdef MESA_TRACE

#include "glheader.h"
#include <time.h>  /* XXX move into glheader.h */
#include "tr_write.h"
#include "tr_context.h"

#define PATH_MAX 4098


static void trWriteFile( const void * buf, GLint recsize, GLint numrecs, const char * funcname ) {
	GLint recswritten;

	if( !(trCtx()->trDoPrint) ) {
		return;
	}

	recswritten = fwrite( buf, recsize, numrecs, trCtx()->logFP );
	if( recswritten != numrecs ) {
		fprintf( stderr, "Error writing to file in %s\n", funcname );
		fprintf( stderr, "Perhaps the disk is full?\n" );
		/* Should we abort ? */
		trCtx()->trDoPrint = GL_FALSE;
	}
}


static void (*trWrite)( const void *, GLint, GLint, const char *) = trWriteFile;


void trWriteCMD( GLint cmd ) {
	trWrite( (void*)&cmd, sizeof(GLint), 1, "trWriteCMD" );
}

void trWriteb( GLbyte b ) {
	trWrite( (void*)&b, sizeof(GLbyte), 1, "trWriteb" );
}

void trWrited( GLdouble d ) {
	trWrite( (void*)&d, sizeof(GLdouble), 1, "trWrited" );
}

void trWriteClampd( GLclampd cd ) {
	trWrite( (void*)&cd, sizeof(GLclampd), 1, "trWriteClampd" );
}

void trWritef( GLfloat f ) {
	trWrite( (void*)&f, sizeof(GLfloat), 1, "trWritef" );
}

void trWriteClampf( GLclampf cf ) {
	trWrite( (void*)&cf, sizeof(GLclampf), 1, "trWriteClampf" );
}

void trWritei( GLint i ) {
	trWrite( (void*)&i, sizeof(GLint), 1, "trWritei" );
}

void trWrites( GLshort s ) {
	trWrite( (void*)&s, sizeof(GLshort), 1, "trWrites" );
}

void trWriteub( GLubyte ub ) {
	trWrite( (void*)&ub, sizeof(GLubyte), 1, "trWriteub" );
}

void trWriteui( GLuint ui ) {
	trWrite( (void*)&ui, sizeof(GLuint), 1, "trWriteui" );
}

void trWriteus( GLushort us ) {
	trWrite( (void*)&us, sizeof(GLushort), 1, "trWriteus" );
}

void trWriteBool( GLboolean b ) {
	trWrite( (void*)&b, sizeof(GLboolean), 1, "trWriteBool" );
}

void trWriteBits( GLbitfield bf ) {
	trWrite( (void*)&bf, sizeof(GLbitfield), 1, "trWriteBits" );
}

void trWriteEnum( GLenum en ) {
	trWrite( (void*)&en, sizeof(GLenum), 1, "trWriteEnum" );
}

void trWriteSizei( GLsizei si ) {
	trWrite( (void*)&si, sizeof(GLsizei), 1, "trWriteSizei" );
}


void trWritePointer( const void * p ) {
	trWrite( (void*)&p, sizeof(void *), 1, "trWritePointer" );
}


void trWriteArrayb( GLsizei n, const GLbyte * b ) {
	trWrite( (void *)b, sizeof(GLbyte), n, "trWriteArrayb" );
}


void trWriteArrayub( GLsizei n, const GLubyte * ub ) {
	trWrite( (void *)ub, sizeof(GLubyte), n, "trWriteArrayub" );
}


void trWriteArrays( GLsizei n, const GLshort * s ) {
	trWrite( (void *)s, sizeof(GLshort), n, "trWriteArrays" );
}


void trWriteArrayus( GLsizei n, const GLushort * us ) {
	trWrite( (void *)us, sizeof(GLushort), n, "trWriteArrayus" );
}


void trWriteArrayi( GLsizei n, const GLint * i ) {
	trWrite( (void *)i, sizeof(GLint), n, "trWriteArrayi" );
}


void trWriteArrayui( GLsizei n, const GLuint * ui ) {
	trWrite( (void *)ui, sizeof(GLuint), n, "trWriteArrayui" );
}


void trWriteArrayBool( GLsizei n, const GLboolean * b ) {
	trWrite( (void *)b, sizeof(GLboolean), n, "trWriteArrayBool" );
}


void trWriteArrayf( GLsizei n, const GLfloat * f ) {
	trWrite( (void *)f, sizeof(GLfloat), n, "trWriteArrayf" );
}


void trWriteArrayd( GLsizei n, const GLdouble * d ) {
	trWrite( (void *)d, sizeof(GLdouble), n, "trWriteArrayd" );
}


void trWriteString( const char * str ) {
	GLuint length = strlen(str);
	trWriteui( length );
	trWrite( (void *)str, sizeof(GLubyte), length, "trWriteString" );
}


void trFileFlush( void ) {
  fflush( trCtx()->logFP );
}


static void trWriteTypeInformation( void ) {
	
	trWriteub( sizeof(GLint) );
	trWriteub( sizeof(GLshort) );
	trWriteub( sizeof(GLfloat) );
	trWriteub( sizeof(GLdouble) );
	trWriteub( sizeof(GLboolean) );
	trWriteub( sizeof(GLenum) );
	trWriteub( sizeof(GLsizei) );
	trWriteub( sizeof(GLbitfield) );
	trWriteub( sizeof(GLclampf) );
	trWriteub( sizeof(GLclampd) );
	trWriteub( sizeof(GLvoid *) );

}


static void trWriteHeader( void ) {
	struct tm *newtime;
	time_t aclock;
	char timestring[256];

	trWriteString( "0.0.1" );
	trWriteString( trCtx()->traceName );
	trWritei( trCtx()->framecounter );

	/* Always print this! */
	time( &aclock );
	newtime = localtime( &aclock );
	asctime( newtime );
	snprintf( timestring, sizeof(timestring), "Time: %s", asctime( newtime ) );
	trWriteString( timestring );
}


static void trWriteGLState( void ) {
	/*
	 * This function will write all the queryable GL state at
	 * the time the trace is started.
	 */
}


void trOpenLogFile( void ) {
	GLint numchars;
	char buffer[PATH_MAX];

	numchars = snprintf( buffer, sizeof(buffer), "%s-gl.log.%d",
	                     trCtx()->traceName, trCtx()->framecounter ); 
	if( numchars > -1 && numchars < sizeof(buffer) ) {
		trCtx()->logFP = fopen( buffer, "wb" );

		/* This information is needed before we can extract _anything_ */
		trWriteTypeInformation();
		trWriteHeader();
		trWriteGLState();

		(trCtx()->framecounter)++;
	} else if( numchars > -1 ) {
		fprintf( stderr, "buffer needs to be %d bytes long for logging to work.\n",
		         numchars + 1 );
		exit( -1 );
	} else {
		fprintf( stderr, "buffer needs to grow for logging to work.  Try %d bytes.\n",
		         sizeof(buffer) * 2 );
		exit( -1 );
	}
}


void trCloseLogFile( void ) {
	if( trCtx()->logFP != NULL ) {
		fclose(trCtx()->logFP);
	}

	trCtx()->logFP = NULL;
}


#else
extern void tr_write_dummy_func(void);
void tr_write_dummy_func(void)
{
}
#endif
