/* $Id: cubemap.c,v 1.4 2002/10/25 17:20:26 brianp Exp $ */

/*
 * GL_ARB_texture_cube_map demo
 *
 * Brian Paul
 * May 2000
 *
 *
 * Copyright (C) 2000  Brian Paul   All Rights Reserved.
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


/*
 * This is a pretty minimalistic demo for now.  Eventually, use some
 * interesting cube map textures and 3D objects.
 * For now, we use 6 checkerboard "walls" and a sphere (good for
 * verification purposes).
 */


#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GL/glut.h"
#include "../util/readtex.c" /* a hack */


static GLfloat Xrot = 0, Yrot = 0;
static GLfloat EyeDist = 10;


static void draw_skybox( void )
{
   const GLfloat eps1 = 0.99;
   const GLfloat br = 20.0; /* box radius */

   glBegin(GL_QUADS);

   /* +X side */
   glTexCoord3f(1.0, -eps1, -eps1);  glVertex3f(br, -br, -br);
   glTexCoord3f(1.0, -eps1,  eps1);  glVertex3f(br, -br,  br);
   glTexCoord3f(1.0,  eps1,  eps1);  glVertex3f(br,  br,  br);
   glTexCoord3f(1.0,  eps1, -eps1);  glVertex3f(br,  br, -br);

   /* -X side */
   glTexCoord3f(-1.0,  eps1, -eps1);  glVertex3f(-br,  br, -br);
   glTexCoord3f(-1.0,  eps1,  eps1);  glVertex3f(-br,  br,  br);
   glTexCoord3f(-1.0, -eps1,  eps1);  glVertex3f(-br, -br,  br);
   glTexCoord3f(-1.0, -eps1, -eps1);  glVertex3f(-br, -br, -br);

   /* +Y side */
   glTexCoord3f(-eps1, 1.0, -eps1);  glVertex3f(-br,  br, -br);
   glTexCoord3f(-eps1, 1.0,  eps1);  glVertex3f(-br,  br,  br);
   glTexCoord3f( eps1, 1.0,  eps1);  glVertex3f( br,  br,  br);
   glTexCoord3f( eps1, 1.0, -eps1);  glVertex3f( br,  br, -br);

   /* -Y side */
   glTexCoord3f(-eps1, -1.0, -eps1);  glVertex3f(-br, -br, -br);
   glTexCoord3f(-eps1, -1.0,  eps1);  glVertex3f(-br, -br,  br);
   glTexCoord3f( eps1, -1.0,  eps1);  glVertex3f( br, -br,  br);
   glTexCoord3f( eps1, -1.0, -eps1);  glVertex3f( br, -br, -br);

   /* +Z side */
   glTexCoord3f( eps1, -eps1, 1.0);  glVertex3f( br, -br, br);
   glTexCoord3f(-eps1, -eps1, 1.0);  glVertex3f(-br, -br, br);
   glTexCoord3f(-eps1,  eps1, 1.0);  glVertex3f(-br,  br, br);
   glTexCoord3f( eps1,  eps1, 1.0);  glVertex3f( br,  br, br);

   /* -Z side */
   glTexCoord3f( eps1,  eps1, -1.0);  glVertex3f( br,  br, -br);
   glTexCoord3f(-eps1,  eps1, -1.0);  glVertex3f(-br,  br, -br);
   glTexCoord3f(-eps1, -eps1, -1.0);  glVertex3f(-br, -br, -br);
   glTexCoord3f( eps1, -eps1, -1.0);  glVertex3f( br, -br, -br);

   glEnd();
}


static void draw( void )
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix(); /*MODELVIEW*/
      glTranslatef( 0.0, 0.0, -EyeDist );

      /* skybox */
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_GEN_R);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
         glRotatef(Xrot, 1, 0, 0);
         glRotatef(Yrot, 0, 1, 0);
         draw_skybox();
      glPopMatrix();

      /* sphere */
      glMatrixMode(GL_TEXTURE);
      glLoadIdentity();
      glRotatef(-Yrot, 0, 1, 0);
      glRotatef(-Xrot, 1, 0, 0);

      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
      glEnable(GL_TEXTURE_GEN_R);
      glutSolidSphere(2.0, 20, 20);

      glLoadIdentity(); /* texture */

      glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glutSwapBuffers();
}


static void idle(void)
{
   GLfloat t = 0.05 * glutGet(GLUT_ELAPSED_TIME);
   Yrot = t;
   glutPostRedisplay();
}


static void set_mode(GLuint mode)
{
   if (mode == 0) {
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
      glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
      printf("GL_REFLECTION_MAP_ARB mode\n");
   }
   else if (mode == 1) {
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
      glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
      printf("GL_NORMAL_MAP_ARB mode\n");
   }
}


static void key(unsigned char k, int x, int y)
{
   static GLboolean anim = GL_TRUE;
   static GLuint mode = 0;
   (void) x;
   (void) y;
   switch (k) {
      case ' ':
         anim = !anim;
         if (anim)
            glutIdleFunc(idle);
         else
            glutIdleFunc(NULL);
         break;
      case 'm':
         mode = !mode;
         set_mode(mode);
         break;
      case 'z':
         EyeDist -= 0.5;
         if (EyeDist < 6.0)
            EyeDist = 6.0;
         break;
      case 'Z':
         EyeDist += 0.5;
         if (EyeDist > 90.0)
            EyeDist = 90;
         break;
      case 27:
         exit(0);
   }
   glutPostRedisplay();
}


static void specialkey(int key, int x, int y)
{
   GLfloat step = 5;
   (void) x;
   (void) y;
   switch (key) {
      case GLUT_KEY_UP:
         Xrot += step;
         break;
      case GLUT_KEY_DOWN:
         Xrot -= step;
         break;
      case GLUT_KEY_LEFT:
         Yrot -= step;
         break;
      case GLUT_KEY_RIGHT:
         Yrot += step;
         break;
   }
   glutPostRedisplay();
}


/* new window size or exposure */
static void reshape(int width, int height)
{
   GLfloat ar = (float) width / (float) height;
   glViewport(0, 0, (GLint)width, (GLint)height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum( -2.0*ar, 2.0*ar, -2.0, 2.0, 4.0, 100.0 );
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


static void init_checkers( void )
{
#define CUBE_TEX_SIZE 64
   GLubyte image[CUBE_TEX_SIZE][CUBE_TEX_SIZE][3];
   static const GLubyte colors[6][3] = {
      { 255,   0,   0 },
      {   0, 255, 255 },
      {   0, 255,   0 },
      { 255,   0, 255 },
      {   0,   0, 255 },
      { 255, 255,   0 }
   };
   static const GLenum targets[6] = {
      GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
   };

   GLint i, j, f;

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   /* make colored checkerboard cube faces */
   for (f = 0; f < 6; f++) {
      for (i = 0; i < CUBE_TEX_SIZE; i++) {
         for (j = 0; j < CUBE_TEX_SIZE; j++) {
            if ((i/4 + j/4) & 1) {
               image[i][j][0] = colors[f][0];
               image[i][j][1] = colors[f][1];
               image[i][j][2] = colors[f][2];
            }
            else {
               image[i][j][0] = 255;
               image[i][j][1] = 255;
               image[i][j][2] = 255;
            }
         }
      }

      glTexImage2D(targets[f], 0, GL_RGB, CUBE_TEX_SIZE, CUBE_TEX_SIZE, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, image);
   }
}


static void load(GLenum target, const char *filename,
                 GLboolean flipTB, GLboolean flipLR)
{
   GLint w, h;
   GLenum format;
   GLubyte *img = LoadRGBImage( filename, &w, &h, &format );
   if (!img) {
      printf("Error: couldn't load texture image %s\n", filename);
      exit(1);
   }
   assert(format == GL_RGB);

   /* <sigh> the way the texture cube mapping works, we have to flip
    * images to make things look right.
    */
   if (flipTB) {
      const int stride = 3 * w;
      GLubyte temp[3*1024];
      int i;
      for (i = 0; i < h / 2; i++) {
         memcpy(temp, img + i * stride, stride);
         memcpy(img + i * stride, img + (h - i - 1) * stride, stride);
         memcpy(img + (h - i - 1) * stride, temp, stride);
      }
   }
   if (flipLR) {
      const int stride = 3 * w;
      GLubyte temp[3];
      GLubyte *row;
      int i, j;
      for (i = 0; i < h; i++) {
         row = img + i * stride;
         for (j = 0; j < w / 2; j++) {
            int k = w - j - 1;
            temp[0] = row[j*3+0];
            temp[1] = row[j*3+1];
            temp[2] = row[j*3+2];
            row[j*3+0] = row[k*3+0];
            row[j*3+1] = row[k*3+1];
            row[j*3+2] = row[k*3+2];
            row[k*3+0] = temp[0];
            row[k*3+1] = temp[1];
            row[k*3+2] = temp[2];
         }
      }
   }

   gluBuild2DMipmaps(target, GL_RGB, w, h, format, GL_UNSIGNED_BYTE, img);
   free(img);
}


static void load_envmaps(void)
{
   load(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, "right.rgb", GL_TRUE, GL_FALSE);
   load(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, "left.rgb", GL_TRUE, GL_FALSE);
   load(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, "top.rgb", GL_FALSE, GL_TRUE);
   load(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, "bottom.rgb", GL_FALSE, GL_TRUE);
   load(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, "front.rgb", GL_TRUE, GL_FALSE);
   load(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, "back.rgb", GL_TRUE, GL_FALSE);
}


static void init( GLboolean useImageFiles )
{
   GLenum filter;

   /* check for extension */
   {
      char *exten = (char *) glGetString(GL_EXTENSIONS);
      if (!strstr(exten, "GL_ARB_texture_cube_map")) {
         printf("Sorry, this demo requires GL_ARB_texture_cube_map\n");
         exit(0);
      }
   }
   printf("GL_RENDERER: %s\n", (char *) glGetString(GL_RENDERER));

   if (useImageFiles) {
      load_envmaps();
      filter = GL_LINEAR;
   }
   else {
      init_checkers();
      filter = GL_NEAREST;
   }

   glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, filter);
   glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, filter);
   glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glEnable(GL_TEXTURE_CUBE_MAP_ARB);
   glEnable(GL_DEPTH_TEST);

   glClearColor(.3, .3, .3, 0);
   glColor3f( 1.0, 1.0, 1.0 );

   set_mode(0);
}


static void usage(void)
{
   printf("keys:\n");
   printf("  SPACE - toggle animation\n");
   printf("  CURSOR KEYS - rotation\n");
   printf("  m - toggle texgen reflection mode\n");
   printf("  z/Z - change viewing distance\n");
}


int main( int argc, char *argv[] )
{
   glutInitWindowPosition(0, 0);
   glutInitWindowSize(600, 500);
   glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
   glutCreateWindow("Texture Cube Maping");

   if (argc > 1 && strcmp(argv[1] , "-i") == 0)
      init( 1 );
   else
      init( 0 );
   glutReshapeFunc( reshape );
   glutKeyboardFunc( key );
   glutSpecialFunc( specialkey );
   glutIdleFunc( idle );
   glutDisplayFunc( draw );
   usage();
   glutMainLoop();
   return 0;
}
