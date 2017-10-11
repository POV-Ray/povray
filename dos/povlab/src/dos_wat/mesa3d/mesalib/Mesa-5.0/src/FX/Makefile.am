## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src $(FX_CFLAGS)

SUBDIRS = X86

if HAVE_FX
# Build a libtool convenience library.
noinst_LTLIBRARIES = libMesaFX.la
endif

include libMesaFX_la_SOURCES

#	fxapi.c \
#	fxdd.c \
#	fxddspan.c \
#	fxddtex.c \
#	fxdrv.h \
#	fxglidew.c \
#	fxglidew.h \
#	fxsetup.c \
#	fxtexman.c \
#	fxtris.c \
#	fxtritmp.h \
#	fxvb.c \
#	fxvbtmp.h \
#	fxwgl.c

libMesaFX_la_LIBADD = $(FX_LIBS)



include $(top_srcdir)/common_rules.make
