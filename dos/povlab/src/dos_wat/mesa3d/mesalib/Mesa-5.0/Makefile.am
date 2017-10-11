## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign no-dependencies

SUB_DEMOS = @demo_dirs@

if HAVE_GLUT
sel_sub_demos = $(SUB_DEMOS)
else
sel_sub_demos =
endif

if NEED_GLUT
sel_sub_glut = @glut_dir@
else
sel_sub_glut =
endif

if USE_SI_GLU
sel_sub_glu = si-glu
else
sel_sub_glu = src-glu
endif

exec:
	@for subdir in $(sel_sub_demos); do \
	  echo "Making exec in $$subdir"; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) exec); \
	done;

DIST_SUBDIRS = include src si-glu src-glu @glut_dir@ $(SUB_DEMOS) util
SUBDIRS = include src $(sel_sub_glu) $(sel_sub_glut) $(sel_sub_demos) util

EXTRA_DIST = \
	3Dfx \
	BeOS \
	OpenStep \
	WIN32 \
	docs \
	ggi \
	images \
	m4 \
	macos \
	mtdemos \
	src-glut.beos \
	src-glut.dja \
	tests \
	vms \
	widgets-mesa \
	widgets-sgi \
	bootstrap \
	MESADOS.BAT \
	MESAFX.BAT \
	Make-config \
	Makefile.DJ \
	Makefile.X11 \
	Makefile.dja \
	Makefile.win \
	Mingw32.bat \
	Names.win \
	STARTUP.MK \
	common.dja \
	descrip.mms \
	fixam \
	mesaw32.BAT \
	mesawin32.mak \
	mms-config \
	win32-openstep.sh \
	xlib.opt

strip:
	@for subdir in src src-glu si-glu $(sel_sub_glut); do \
	  echo "Making strip in $$subdir"; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) strip); \
	done;

dist-hook:
	rm -rf `find $(distdir) -name 'CVS' -type d`

include $(top_srcdir)/common_rules.make
