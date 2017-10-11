## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = no-dependencies

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src $(SVGA_CFLAGS)

if HAVE_SVGA
# Build a libtool convenience library.
noinst_LTLIBRARIES = libMesaSVGA.la
endif

libMesaSVGA_la_SOURCES = \
	svgamesa.c svgapix.h \
	svgamesa8.c svgamesa8.h \
	svgamesa15.c svgamesa15.h \
	svgamesa16.c svgamesa16.h \
	svgamesa24.c svgamesa24.h \
	svgamesa32.c svgamesa32.h

libMesaSVGA_la_LIBADD = $(SVGA_LIBS)
include $(top_srcdir)/common_rules.make
