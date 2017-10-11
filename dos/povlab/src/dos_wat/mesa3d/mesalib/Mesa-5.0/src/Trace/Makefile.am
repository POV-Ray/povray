## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src

if MESA_TRACE
noinst_LTLIBRARIES = libMesaTrace.la
endif

libMesaTrace_la_SOURCES = \
	tr_attrib.c \
	tr_context.c \
	tr_control.c \
	tr_error.c \
	tr_support.c \
	tr_wrapper.c \
	tr_write.c


include $(top_srcdir)/common_rules.make
