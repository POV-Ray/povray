## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = no-dependencies

INCLUDES = -I$(top_srcdir)/include

# Build a libtool library for installation in libdir.
# Only do this here if not using the SI GLU.
lib_LTLIBRARIES = libGLU.la
libGLU_la_LDFLAGS = -version-info @LIBGLU_VERSION_INFO@ -L../src
libGLU_la_LIBADD = -lGL
libGLU_la_SOURCES = \
	all.h \
	glu.c \
	gluP.h \
	mipmap.c \
	nurbs.c \
	nurbs.h \
	nurbscrv.c \
	nurbssrf.c \
	nurbsutl.c \
	polytest.c \
	project.c \
	quadric.c \
	tess.c \
	tess.h \
	tesselat.c

EXTRA_DIST = descrip.mms

strip:
	-if strip -V 2>&1  | grep "GNU" > /dev/null; then \
		strip --strip-unneeded .libs/*.so; \
		strip --strip-debug .libs/*.a; \
	fi

include $(top_srcdir)/common_rules.make
