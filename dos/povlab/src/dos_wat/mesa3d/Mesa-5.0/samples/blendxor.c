/*
** blendxor.c - Demonstrates the use of the blend_logic_op
**    extension to draw hilights.  Using XOR to draw the same
**    image twice restores the background to its original value.
*/

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif
#define GL_GLEXT_LEGACY
#include <GL/glut.h>


GLenum doubleBuffer;
int dithering = 0;
GLint windW, windH;

static void Init(void)
{
    glDisable(GL_DITHER);
    glShadeModel(GL_FLAT);
}

static void Reshape(int width, int height)
{

    windW = (GLint)width;
    windH = (GLint)height;

    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 400, 0, 400);
    glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{

    switch (key) {
      case 27:
	exit(1);
      case 'd':
	dithering = !dithering;
	break;
      default:
	return;
    }

    glutPostRedisplay();
}

static void Draw(void)
{
    int i;

    glDisable(GL_BLEND);

    (dithering) ? glEnable(GL_DITHER) : glDisable(GL_DITHER);

    glClearColor(0.5, 0.6, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw background prims */
    glColor3f(0.1, 0.1, 1.0);
    glBegin(GL_TRIANGLES);
        glVertex2i(5, 5);
        glVertex2i(130, 50);
        glVertex2i(100,  300);
    glEnd();
    glColor3f(0.5, 0.2, 0.9);
    glBegin(GL_TRIANGLES);
        glVertex2i(200, 100);
        glVertex2i(330, 50);
        glVertex2i(340,  400);
    glEnd();

    glEnable(GL_BLEND);
    glBlendEquationEXT(GL_LOGIC_OP);
    glLogicOp(GL_XOR);

    /* Draw a set of rectangles across the window */
    glColor3f(0.9, 0.2, 0.8);
    for(i = 0; i < 400; i+=60) {
        glBegin(GL_POLYGON);
            glVertex2i(i, 100);
            glVertex2i(i+50, 100);
            glVertex2i(i+50, 200);
            glVertex2i(i, 200);
        glEnd();
    }
    glFlush();   /* Added by Brian Paul */
#ifndef _WIN32
    sleep(2);
#endif

    /* Redraw  the rectangles, which should erase them */
    for(i = 0; i < 400; i+=60) {
        glBegin(GL_POLYGON);
            glVertex2i(i, 100);
            glVertex2i(i+50, 100);
            glVertex2i(i+50, 200);
            glVertex2i(i, 200);
        glEnd();
    }
    glFlush();


    if (doubleBuffer) {
	glutSwapBuffers();
    }
}

static GLenum Args(int argc, char **argv)
{
    GLint i;

    doubleBuffer = GL_FALSE;

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-sb") == 0) {
	    doubleBuffer = GL_FALSE;
	} else if (strcmp(argv[i], "-db") == 0) {
	    doubleBuffer = GL_TRUE;
	} else {
	    printf("%s (Bad option).\n", argv[i]);
	    return GL_FALSE;
	}
    }
    return GL_TRUE;
}

int main(int argc, char **argv)
{
    GLenum type;
    char *s;
    char *extName = "GL_EXT_blend_logic_op";

    glutInit(&argc, argv);

    if (Args(argc, argv) == GL_FALSE) {
	exit(1);
    }

    glutInitWindowPosition(0, 0); glutInitWindowSize( 400, 400);

    type = GLUT_RGB;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);

    if (glutCreateWindow("Blend XOR") == GL_FALSE) {
	exit(1);
    }

    /* Make sure blend_logic_op extension is there. */
    s = (char *) glGetString(GL_EXTENSIONS);
    if (!s)
	exit(1);
    if (strstr(s,extName) == 0) {
	printf("Blend_logic_op extension is not present.\n");
	exit(1);
    }

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutDisplayFunc(Draw);
    glutMainLoop();
	return 0;
}
