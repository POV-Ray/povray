# $Id: Makefile.am,v 1.8 2002/01/08 15:58:32 brianp Exp $

AUTOMAKE_OPTIONS = 

INCLUDES = -I../../include  -I../../../include

noinst_LTLIBRARIES = libNTess.la

# Choose one of STANDALONE, LIBRARYBUILD, and GLBUILD. They are debugging
# switches. grep LIBRARYBUILD `find .. -name '*.h'` for details.
CFLAGS += -DLIBRARYBUILD -DNDEBUG

include libNTess_la_SOURCES


include $(top_srcdir)/common_rules.make
