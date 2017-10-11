## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 

INCLUDES = -I$(top_srcdir)/include $(GLUT_CFLAGS)

EXTRA_PROGRAMS = glstate
glstate_SOURCES = glstate.c glstate.h

LDADD = $(GLUT_LIBS) $(top_builddir)/$(GLU_DIR)/libGLU.la \
	$(top_builddir)/src/libGL.la -lm

EXTRA_DIST = \
	dumpstate.c \
	errcheck.c \
	glutskel.c \
	idproj.c \
	imagesgi.h \
	mwmborder.c \
	readtex.c \
	readtex.h \
	showbuffer.c \
	showbuffer.h \
	winpos.c


include $(top_srcdir)/common_rules.make
