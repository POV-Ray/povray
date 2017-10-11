## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = gnu no-dependencies

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src 

noinst_LTLIBRARIES = libMesaSwrast_setup.la

libMesaSwrast_setup_la_SOURCES = \
ss_context.c \
ss_context.h \
ss_triangle.c \
ss_triangle.h \
ss_vb.c \
ss_vb.h \
swrast_setup.h

# libMesaSwrast_setup_la_LDFLAGS = -static


include $(top_srcdir)/common_rules.make
