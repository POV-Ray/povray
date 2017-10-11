## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = gnu no-dependencies

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src $(X_CFLAGS) $(FX_CFLAGS)

if HAVE_X11
# Build a libtool convenience library.
noinst_LTLIBRARIES = libMesaX11.la
endif

libMesaX11_la_SOURCES = \
	glxapi.c \
	glxapi.h \
	glxheader.h \
	fakeglx.c \
	xfonts.c \
	xfonts.h \
	xm_api.c \
	xm_dd.c \
	xm_line.c \
	xm_span.c \
	xm_tri.c \
	xmesaP.h

libMesaX11_la_LIBADD = $(X_LIBS) $(X_PRE_LIBS) $(X_LIBADD)


include $(top_srcdir)/common_rules.make
