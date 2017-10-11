## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = gnu no-dependencies

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src

noinst_LTLIBRARIES = libMesaMath.la

libMesaMath_la_SOURCES = \
        m_debug_clip.c \
	m_debug_norm.c \
	m_debug_util.h \
	m_debug_xform.c \
	m_debug.h \
	m_eval.c \
	m_eval.h \
	m_matrix.c \
	m_matrix.h \
	m_translate.c \
	m_translate.h \
	m_trans_tmp.h \
	m_vector.c \
	m_vector.h \
	m_xform.c \
	m_xform.h \
	mathmod.h


include $(top_srcdir)/common_rules.make
