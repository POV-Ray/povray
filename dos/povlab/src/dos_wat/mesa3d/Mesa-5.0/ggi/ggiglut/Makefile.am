INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src

if HAVE_GGI
  lib_LTLIBRARIES = libglut.la
  libglut_la_SOURCES = ggiglut.c debug.h
  libglut_la_LDFLAGS = -version-info $(LIBGLUT_VERSION)
endif

ggiincludedir = $(includedir)/ggi
include $(top_srcdir)/common_rules.make
