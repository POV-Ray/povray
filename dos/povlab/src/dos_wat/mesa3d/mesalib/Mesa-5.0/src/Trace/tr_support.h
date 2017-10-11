#ifndef TR_SUPPORT_H
#define TR_SUPPORT_H


extern void trQueryConvolutionState( void );

extern void trZeroGetterData( GLenum pname, GLsizei typesize, GLvoid * params );

extern void trPrintColorTableData( GLenum pname, GLenum type, GLvoid * params );

extern void trWriteTypeArray( GLenum type, GLsizei width, GLsizei pixelsize, GLint start, const GLvoid * ptr );

extern GLint trGetPixelSize( GLenum format, GLenum type );


#endif
