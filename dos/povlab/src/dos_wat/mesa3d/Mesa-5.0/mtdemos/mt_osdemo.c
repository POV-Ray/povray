/* 
 * mt_osdemo.c 
 *
 * Demo of off-screen Mesa rendering with multithreading support .
 * 
 * IMPORTANT NOTE: The code in this file is in alpha stage. It is used
 * as an example how multithreaded programming with Mesa could look
 * like. YOU MUST HAVE COMPILED THE MESA WITH POSIX THREADS SUPPORT TO
 * COMPILE AND RUN THIS DEMO! - poliwoda@volumegraphics.com
 *
 * The original "osdemo" by Brian Paul can be found in "../demos"
 *
 * See Mesa/include/GL/osmesa.h for documentation of the OSMesa functions.
 *
 * If you want to render BIG images you'll probably have to increase
 * MAX_WIDTH and MAX_HEIGHT in src/config.h.
 *
 * This program is in the public domain.
 *
 * Brian Paul
 *
 * modified for multithread usage by Christop Poliwoda (poliwoda@volumegraphics.com) 
 * PPM output provided by Joerg Schmalzl.
 * ASCII PPM output added by Brian Paul.
 *
 */

#ifndef THREADS
#ifndef PTHREADS
#error This code compiles and runs with POSIX threads support only. \
Compile the Mesa and this demo as threaded version.
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include "GL/osmesa.h"
#include "GL/glut.h"

#include <pthread.h>            /* POSIX thread interface */


#define WIDTH 512
#define HEIGHT 512
#define REPETITIONS 4
#define MAXTHREADS 4

void * buffers[MAXTHREADS];	/* these are the different result buffers */
OSMesaContext ctx[MAXTHREADS];	/* and these the used contexts */



/*
 * renders an image with 3 teapots, colorized in red, green, blue 
 */
static void render_image( void ) {
   GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
   GLfloat red_mat[]   = { 1.0, 0.2, 0.2, 1.0 };
   GLfloat green_mat[] = { 0.2, 1.0, 0.2, 1.0 };
   GLfloat blue_mat[]  = { 0.2, 0.2, 1.0, 1.0 };

   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-2.5, 2.5, -2.5, 2.5, -10.0, 10.0);
   glMatrixMode(GL_MODELVIEW);

   glPushMatrix();
   glRotatef(20.0, 1.0, 0.0, 0.0);

   glPushMatrix();
   glTranslatef(-0.75, 0.5, 0.0); 
   glRotatef(90.0, 1.0, 0.0, 0.0);
   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red_mat );
   glutSolidTeapot(1.5);
   glPopMatrix();
   glPopMatrix();

   glPushMatrix();
   glRotatef(20.0, 1.0, 0.0, 0.0);

   glPushMatrix();
   glTranslatef(-0.75, -0.5, 0.0); 
   glRotatef(270.0, 1.0, 0.0, 0.0);
   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green_mat );
   glutSolidTeapot(1.0);
   glPopMatrix();
   glPopMatrix();

   glPushMatrix();
   glRotatef(20.0, 1.0, 0.0, 0.0);

   glPushMatrix();
   glTranslatef(0.75, 0.0, -1.0); 
   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue_mat );
   glutSolidTeapot(1.0);
   glPopMatrix();
   glPopMatrix();

   glFinish();
}


/* 
 * Do rendering for a thread.
 */
static void * thread_function(void * threadNr) {

  int i = (int)threadNr;

  OSMesaMakeCurrent(ctx[i], buffers[i], GL_UNSIGNED_BYTE, WIDTH, HEIGHT);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  render_image();

  return NULL;
}



int main( int argc, char *argv[] ) {
  pthread_t threads[MAXTHREADS]; 
  int i, k;
  int p, x, y;
  int t0, t1;
  FILE *f;
  GLubyte *ptr;

#ifdef __sun
  thr_setconcurrency(MAXTHREADS);
#endif

  for (i=0; i<MAXTHREADS; i++) {
    /* create the OS Mesa contexts and result buffers */
    ctx[i] = OSMesaCreateContext( GL_RGBA, NULL );
    buffers[i] = malloc(WIDTH * HEIGHT * 4);
  }

  /* to compare rendering time, we do a single threaded rendering pass */
  printf("Main thread rendering %d %dx%d-images (no threads)...\n", 
         MAXTHREADS*REPETITIONS, WIDTH, HEIGHT); 
  fflush(stdout);

  t0 = glutGet(GLUT_ELAPSED_TIME);
  for (k=0; k<REPETITIONS; k++) {
    for (i=0; i<MAXTHREADS; i++) {
      thread_function((void*)i); 
    }
  }
  t1 = glutGet(GLUT_ELAPSED_TIME);
  printf("Single thread runtime: %6.2f seconds\n\n", (t1 - t0) * 0.001);


  /* now we run the multithreaded code */
  printf("%d threads rendering %d %dx%d-images (multithreaded)...\n", 
         MAXTHREADS, MAXTHREADS*REPETITIONS, WIDTH, HEIGHT); 
  fflush(stdout);

  t0 = glutGet(GLUT_ELAPSED_TIME);
  for (k=0; k<REPETITIONS; k++) {
    for (i=0; i<MAXTHREADS; i++) {
      pthread_create(&threads[i], NULL, thread_function, (void*)i);
    }
    for (i=0; i<MAXTHREADS; i++) {
      pthread_join(threads[i], NULL);  /* wait for threads to finish */
    }
  }
  t1 = glutGet(GLUT_ELAPSED_TIME);
  printf("Multithreaded runtime: %6.2f seconds\n\n", (t1 - t0) * 0.001);

  for (i=0; i<MAXTHREADS; i++) {
    /* write PPM (binary) file */
    if ((argc>i+1) &&
	(f = fopen( argv[i+1], "w" ))) {
      printf("write buffer #%d to file %s\n", i, argv[i+1]);
      ptr = (GLubyte *) buffers[i];
      fprintf(f,"P6\n");
      fprintf(f,"# ppm-file created by %s\n",  argv[0]);
      fprintf(f,"%i %i\n", WIDTH,HEIGHT);
      fprintf(f,"255\n");
      fclose(f);
      f = fopen( argv[i+1], "ab" );  /* reopen in binary append mode */
      for (y=HEIGHT-1; y>=0; y--) {
	for (x=0; x<WIDTH; x++) {
	  p = (y*WIDTH + x) * 4;
	  fputc(ptr[p], f);   /* write red */
	  fputc(ptr[p+1], f); /* write green */
	  fputc(ptr[p+2], f); /* write blue */
	}
      }
      fclose(f);
    }
    /* destroy OS Mesa contexts and result buffers */
    OSMesaDestroyContext(ctx[i]);
    free(buffers[i]);
  }

  printf("\nall done.\n");

  return 0;
}
