# Makefile.in generated automatically by automake 1.4-p5 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



SHELL = /bin/sh

srcdir = .
top_srcdir = ../..

prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/Mesa
pkglibdir = $(libdir)/Mesa
pkgincludedir = $(includedir)/Mesa

top_builddir = ../..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = @host_alias@
host_triplet = i686-pc-linux-gnu
AS = @AS@
CC = gcc
CPP = gcc -E
CXX = g++
DLLEXT = @DLLEXT@
DLLTOOL = @DLLTOOL@
ECHO = echo
EXEEXT = 
FX_CFLAGS = 
FX_LIBS = 
GGI_CFLAGS = 
GGI_LIBS =  -lggi -lgii -lgg
GLUT_CFLAGS =  -DNDEBUG
GLUT_LIBS = $(top_builddir)/src-glut/libglut.la
GLU_DIR = si-glu
LIBGLUT_VERSION_INFO = 10:1:7
LIBGLU_VERSION_INFO = 2:500:1
LIBGL_VERSION_INFO = 5:500:4
LIBMESA_VERSION_INFO = 4:500:0
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
MAINT = #
MAKEINFO = makeinfo
MESA_MD_FLAG = -MD
OBJDUMP = @OBJDUMP@
OBJEXT = o
PACKAGE = Mesa
RANLIB = ranlib
SI_GLU_VERSION_INFO = 4:500:3
STRIP = strip
SVGA_CFLAGS = 
SVGA_LIBS = 
THREAD_LIBS =  -lpthread
VERSION = 5.0
X_LIBADD =  -lXmu -lXext -lXi -lX11
demo_dirs = book demos samples xdemos
extra_includes = @extra_includes@
extra_libraries = @extra_libraries@
ggi_confdir = NONE/etc
ggi_libdir = NONE/lib
glut_dir = src-glut
use_si_glu = yes

AUTOMAKE_OPTIONS = gnu no-dependencies

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src

noinst_LTLIBRARIES = libMesaMath.la

libMesaMath_la_SOURCES =          m_debug_clip.c 	m_debug_norm.c 	m_debug_util.h 	m_debug_xform.c 	m_debug.h 	m_eval.c 	m_eval.h 	m_matrix.c 	m_matrix.h 	m_translate.c 	m_translate.h 	m_trans_tmp.h 	m_vector.c 	m_vector.h 	m_xform.c 	m_xform.h 	mathmod.h

mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../conf.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(noinst_LTLIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I../..
CPPFLAGS = 
LDFLAGS = 
LIBS = 
X_CFLAGS =  -I/usr/X11R6/include
X_LIBS =  -L/usr/X11R6/lib
X_EXTRA_LIBS = 
X_PRE_LIBS =  -lSM -lICE
libMesaMath_la_LDFLAGS = 
libMesaMath_la_LIBADD = 
libMesaMath_la_OBJECTS =  m_debug_clip.lo m_debug_norm.lo \
m_debug_xform.lo m_eval.lo m_matrix.lo m_translate.lo m_vector.lo \
m_xform.lo
CFLAGS = -g -O2 -Wall -O0 -DDEBUG -DMESA_TRACE -ansi -pedantic -D_REENTRANT -DPTHREADS
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best
SOURCES = $(libMesaMath_la_SOURCES)
OBJECTS = $(libMesaMath_la_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .lo .o .obj .s
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) $(top_srcdir)/common_rules.make
	cd $(top_srcdir) && $(AUTOMAKE) --gnu src/math/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-noinstLTLIBRARIES:

clean-noinstLTLIBRARIES:
	-test -z "$(noinst_LTLIBRARIES)" || rm -f $(noinst_LTLIBRARIES)

distclean-noinstLTLIBRARIES:

maintainer-clean-noinstLTLIBRARIES:

.c.o:
	$(COMPILE) -c $<

# FIXME: We should only use cygpath when building on Windows,
# and only if it is available.
.c.obj:
	$(COMPILE) -c `cygpath -w $<`

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core
	-rm -f *.$(OBJEXT)

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

.c.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.s.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.S.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:

maintainer-clean-libtool:

libMesaMath.la: $(libMesaMath_la_OBJECTS) $(libMesaMath_la_DEPENDENCIES)
	$(LINK)  $(libMesaMath_la_LDFLAGS) $(libMesaMath_la_OBJECTS) $(libMesaMath_la_LIBADD) $(LIBS)

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = src/math

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am:
install-exec: install-exec-am

install-data-am:
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am:
uninstall: uninstall-am
all-am: Makefile $(LTLIBRARIES)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-noinstLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags \
		mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-noinstLTLIBRARIES clean-compile clean-libtool \
		clean-tags clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-noinstLTLIBRARIES distclean-compile \
		distclean-libtool distclean-tags distclean-generic \
		clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-noinstLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-generic \
		distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-noinstLTLIBRARIES distclean-noinstLTLIBRARIES \
clean-noinstLTLIBRARIES maintainer-clean-noinstLTLIBRARIES \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile mostlyclean-libtool distclean-libtool \
clean-libtool maintainer-clean-libtool tags mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags distdir info-am info \
dvi-am dvi check check-am installcheck-am installcheck install-exec-am \
install-exec install-data-am install-data install-am install \
uninstall-am uninstall all-redirect all-am all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


#%.o: %.c
#	@echo '$(COMPILE) -c $<'; \
#	$(COMPILE) -Wp,$(MESA_MD_FLAG),.deps/$(*F).pp -c $<
#	@-cp .deps/$(*F).pp .deps/$(*F).P; \
#	tr ' ' '\012' < .deps/$(*F).pp \
#	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
#	    >> .deps/$(*F).P; \
#	rm .deps/$(*F).pp
#%.lo: %.c
#	@echo '$(LTCOMPILE) -c $<'; \
#	$(LTCOMPILE) -Wp,$(MESA_MD_FLAG),.deps/$(*F).pp -c $<
#	@-sed -e 's/^\([^:]*\)\.o[      ]*:/\1.lo \1.o :/' \
#	  < .deps/$(*F).pp > .deps/$(*F).P; \
#	tr ' ' '\012' < .deps/$(*F).pp \
#	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
#	    >> .deps/$(*F).P; \
#	rm -f .deps/$(*F).pp
#%.o: %.cc
#	@echo '$(CXXCOMPILE) -c $<'; \
#	$(CXXCOMPILE) -Wp,$(MESA_MD_FLAG),.deps/$(*F).pp -c $<
#	@-cp .deps/$(*F).pp .deps/$(*F).P; \
#	tr ' ' '\012' < .deps/$(*F).pp \
#	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
#	    >> .deps/$(*F).P; \
#	rm .deps/$(*F).pp
#%.lo: %.cc
#	@echo '$(LTCXXCOMPILE) -c $<'; \
#	$(LTCXXCOMPILE) -Wp,$(MESA_MD_FLAG),.deps/$(*F).pp -c $<
#	@-sed -e 's/^\([^:]*\)\.o[      ]*:/\1.lo \1.o :/' \
#	  < .deps/$(*F).pp > .deps/$(*F).P; \
#	tr ' ' '\012' < .deps/$(*F).pp \
#	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
#	    >> .deps/$(*F).P; \
#	rm -f .deps/$(*F).pp

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
