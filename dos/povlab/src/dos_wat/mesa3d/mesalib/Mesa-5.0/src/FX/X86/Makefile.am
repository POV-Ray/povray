## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src $(FX_CFLAGS)

x3dnow_files = \
	fx_3dnow_fasttmp.h \
	fx_3dnow_fastpath.S


if HAVE_3DNOW
x3dnow_sources = $(x3dnow_files)
endif

if HAVE_X86
fx_3dnow_fastpath.lo: fx_3dnow_fastpath.S fx_3dnow_fasttmp.h

noinst_LTLIBRARIES = libMesaFX_X86.la
endif

libMesaFX_X86_la_SOURCES =  $(x3dnow_sources)
libMesaFX_X86_la_LDFLAGS = -static


include $(top_srcdir)/common_rules.make
