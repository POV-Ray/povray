## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = gnu no-dependencies

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src

if HAVE_OSMESA
lib_LTLIBRARIES = libOSMesa.la
endif

libOSMesa_la_SOURCES = osmesa.c
libOSMesa_la_LDFLAGS = -version-info @LIBMESA_VERSION_INFO@
libOSMesa_la_LIBADD = ../libGL.la


include $(top_srcdir)/common_rules.make
