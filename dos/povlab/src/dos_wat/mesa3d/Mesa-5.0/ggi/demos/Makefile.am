## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/

if HAVE_GGI
check_PROGRAMS = gears gears2 asc-view bigtest
endif

LDADD = $(top_builddir)/$(GLU_DIR)/libGLU.la \
	$(top_builddir)/src/libGL.la -lm -lggi -lgii -lgg -lglut



include $(top_srcdir)/common_rules.make
