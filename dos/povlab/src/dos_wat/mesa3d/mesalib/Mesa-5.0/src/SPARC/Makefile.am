## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS =

sparc_files = \
	sparc_matrix.h \
	sparc.h \
	sparc.c \
	clip.S \
	xform.S \
	norm.S \
	glapi_sparc.S

if HAVE_SPARC
# Build a libtool convenience library.
noinst_LTLIBRARIES = libMesaSPARC.la
sparc_sources = $(sparc_files)
CFLAGS_SPARC = -DUSE_SPARC_ASM
endif

libMesaSPARC_la_SOURCES = $(sparc_sources)

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src $(CFLAGS_SPARC)

EXTRA_DIST = 

include $(top_srcdir)/common_rules.make
