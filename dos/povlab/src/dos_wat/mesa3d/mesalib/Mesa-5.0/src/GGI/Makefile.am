## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 

SUBDIRS = default display include

ggimesaconfdatadir = ${sysconfdir}/ggi
ggimesaconfdata_DATA = ggimesa.conf

INCLUDES += -I$(top_srcdir)/include -I$(top_srcdir)/src/GGI/include \
	-I$(top_srcdir)/src $(GGI_CFLAGS)

DEFS += -DGGIMESACONFFILE=\"$(ggimesaconfdatadir)/ggimesa.conf\"

if HAVE_GGI
# Build a libtool convenience library.
noinst_LTLIBRARIES = libMesaGGI.la
endif

libMesaGGI_la_SOURCES = ggimesa.c
libMesaGGI_la_LIBADD = $(GGI_LIBS)

DISTCLEANFILES = ggimesa.conf


include $(top_srcdir)/common_rules.make
