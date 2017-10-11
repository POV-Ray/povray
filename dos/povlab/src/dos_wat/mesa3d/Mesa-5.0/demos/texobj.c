/* $Id: texobj.c,v 1.6 2002/01/04 21:05:57 brianp Exp $ */

/*
 * Example of using the 1.1 texture object functions.
 * Also, this demo utilizes Mesa's fast texture map path.
 *
 * Brian Paul   June 1996   This file is in the public domain.
 */

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "GL/glut.h"

static GLuint Window = 0;

static GLuint TexObj[2];
static GLfloat Angle = 0.0f;
static GLboolean UseObj = GL_FALSE;


#if defined(GL_VERSION_1_1) || defined(GL_VERSION_1_2)
#  define TEXTURE_OBJECT 1
#elif defined(GL_EXT_texture_object)
#  define TEXTURE_OBJECT 1
#  define glBindTexture(A,B)     glBindTextureEXT(A,B)
#  define glGenTextures(A,B)     glGenTexturesEXT(A,B)
#  define glDeleteTextures(A,B)  glDeleteTexturesEXT(A,B)
#endif




static void draw( void )
{
   glDepthFunc(GL_EQUAL);
   /*   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );*/
   glClear( GL_COLOR_BUFFER_BIT );

   glColor3f( 1.0, 1.0, 1.0 );

   /* draw first polygon */
   glPushMatrix();
   glTranslatef( -1.0, 0.0, 0.0 );
   glRotatef( Angle, 0.0, 0.0, 1.0 );
   if (UseObj) {
#ifdef TEXTURE_OBJECT
      glBindTexture( GL_TEXTURE_2D, TexObj[0] );
#endif
   }
   else {
      glCallList( TexObj[0] );
   }
   glBegin( GL_POLYGON );
   glTexCoord2f( 0.0, 0.0 );   glVertex2f( -1.0, -1.0 );
   glTexCoord2f( 1.0, 0.0 );   glVertex2f(  1.0, -1.0 );
   glTexCoord2f( 1.0, 1.0 );   glVertex2f(  1.0,  1.0 );
   glTexCoord2f( 0.0, 1.0 );   glVertex2f( -1.0,  1.0 );
   glEnd();
   glPopMatrix();

   /* draw second polygon */
   glPushMatrix();
   glTranslatef( 1.0, 0.0, 0.0 );
   glRotatef( Angle-90.0, 0.0, 1.0, 0.0 );
   if (UseObj) {
#ifdef TEXTURE_OBJECT
      glBindTexture( GL_TEXTURE_2D, TexObj[1] );
#endif
   }
   else {
      glCallList( TexObj[1] );
   }
   glBegin( GL_POLYGON );
   glTexCoord2f( 0.0, 0.0 );   glVertex2f( -1.0, -1.0 );
   glTexCoord2f( 1.0, 0.0 );   glVertex2f(  1.0, -1.0 );
   glTexCoord2f( 1.0, 1.0 );   glVertex2f(  1.0,  1.0 );
   glTexCoord2f( 0.0, 1.0 );   glVertex2f( -1.0,  1.0 );
   glEnd();
   glPopMatrix();

   glutSwapBuffers();
}



static void idle( void )
{
   Angle += 2.0;
   glutPostRedisplay();
}



/* change view Angle, exit upon ESC */
static void key(unsigned char k, int x, int y)
{
   (void) x;
   (void) y;
   switch (k) {
      case 27:
#ifdef TEXTURE_OBJECT
         glDeleteTextures( 2, TexObj );
#endif
         glutDestroyWindow(Window);
         exit(0);
   }
}



/* new window size or exposure */
static void reshape( int width, int height )
{
   glViewport(0, 0, (GLint)width, (GLint)height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   /*   glOrtho( -3.0, 3.0, -3.0, 3.0, -10.0, 10.0 );*/
   glFrustum( -2.0, 2.0, -2.0, 2.0, 6.0, 20.0 );
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef( 0.0, 0.0, -8.0 );
}


static void init( void )
{
   static int width=8, height=8;
   static GLubyte tex1[] = {
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 1, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 1, 1, 1, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 };

   static GLubyte tex2[] = {
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 2, 2, 0, 0, 0,
     0, 0, 2, 0, 0, 2, 0, 0,
     0, 0, 0, 0, 0, 2, 0, 0,
     0, 0, 0, 0, 2, 0, 0, 0,
     0, 0, 0, 2, 0, 0, 0, 0,
     0, 0, 2, 2, 2, 2, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 };

   GLubyte tex[64][3];
   GLint i, j;


   glDisable( GL_DITHER );

   /* Setup texturing */
   glEnable( GL_TEXTURE_2D );
   glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
   glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );


   /* generate texture object IDs */
   if (UseObj) {
#ifdef TEXTURE_OBJECT
      glGenTextures( 2, TexObj );
#endif
   }
   else {
      TexObj[0] = glGenLists(2);
      TexObj[1] = TexObj[0]+1;
   }

   /* setup first texture object */
   if (UseObj) {
#ifdef TEXTURE_OBJECT
      glBindTexture( GL_TEXTURE_2D, TexObj[0] );
      assert(glIsTexture(TexObj[0]));
#endif
   }
   else {
      glNewList( TexObj[0], GL_COMPILE );
   }
   /* red on white */
   for (i=0;i<height;i++) {
      for (j=0;j<width;j++) {
         int p = i*width+j;
         if (tex1[(height-i-1)*width+j]) {
            tex[p][0] = 255;   tex[p][1] = 0;     tex[p][2] = 0;
         }
         else {
            tex[p][0] = 255;   tex[p][1] = 255;   tex[p][2] = 255;
         }
      }
   }

   glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, tex );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
   if (!UseObj) {
      glEndList();
   }
   /* end of texture object */

   /* setup second texture object */
   if (UseObj) {
#ifdef TEXTURE_OBJECT
      glBindTexture( GL_TEXTURE_2D, TexObj[1] );
      assert(glIsTexture(TexObj[1]));
#endif
      assert(!glIsTexture(TexObj[1] + 999));
   }
   else {
      glNewList( TexObj[1], GL_COMPILE );
   }
   /* green on blue */
   for (i=0;i<height;i++) {
      for (j=0;j<width;j++) {
         int p = i*width+j;
         if (tex2[(height-i-1)*width+j]) {
            tex[p][0] = 0;     tex[p][1] = 255;   tex[p][2] = 0;
         }
         else {
            tex[p][0] = 0;     tex[p][1] = 0;     tex[p][2] = 255;
         }
      }
   }
   glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, tex );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
   if (!UseObj) {
      glEndList();
   }
   /* end texture object */

}



int main( int argc, char *argv[] )
{
   glutInit(&argc, argv);
   glutInitWindowPosition(0, 0);
   glutInitWindowSize(300, 300);
   glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );

   Window = glutCreateWindow("Texture Objects");
   if (!Window) {
      exit(1);
   }

   /* check that renderer has the GL_EXT_texture_object extension
    * or supports OpenGL 1.1
    */
#ifdef TEXTURE_OBJECT
   {
      char *exten = (char *) glGetString( GL_EXTENSIONS );
      char *version = (char *) glGetString( GL_VERSION );
      if (   strstr( exten, "GL_EXT_texture_object" )
          || strncmp( version, "1.1", 3 )==0
          || strncmp( version, "1.2", 3 )==0 ) {
         UseObj = GL_TRUE;
      }
   }
#endif

   init();

   glutReshapeFunc( reshape );
   glutKeyboardFunc( key );
   glutIdleFunc( idle );
   glutDisplayFunc( draw );
   glutMainLoop();
   return 0;
}
