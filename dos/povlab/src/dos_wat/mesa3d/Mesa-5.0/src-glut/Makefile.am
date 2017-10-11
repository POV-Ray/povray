## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = gnu no-dependencies

INCLUDES = -I$(top_srcdir)/include $(X_CFLAGS) $(GLUT_CFLAGS)

# Build a libtool library for installation in libdir.
lib_LTLIBRARIES = libglut.la
EXTRA_PROGRAMS = capturexfont

libglut_la_LDFLAGS = -version-info @LIBGLUT_VERSION_INFO@ -L../@GLU_DIR@
libglut_la_LIBADD = -lGLU
libglut_la_SOURCES = \
	glutwin32.h \
	win32_glx.h \
	win32_x11.h \
	glut_8x13.c \
	glut_9x15.c \
	glut_bitmap.c \
	glut_bwidth.c \
	glut_cindex.c \
	glut_cmap.c \
	glut_cursor.c \
	glut_dials.c \
	glut_dstr.c \
	glut_event.c \
	glut_ext.c \
	glut_fbc.c \
	glut_fullscrn.c \
	glut_gamemode.c \
	glut_get.c \
	glut_glxext.c \
	glut_hel10.c \
	glut_hel12.c \
	glut_hel18.c \
	glut_init.c \
	glut_input.c \
	glut_joy.c \
	glut_key.c \
	glut_keyctrl.c \
	glut_keyup.c \
	glut_menu.c \
	glut_menu2.c \
	glut_mesa.c \
	glut_modifier.c \
	glut_mroman.c \
	glut_overlay.c \
	glut_roman.c \
	glut_shapes.c \
	glut_space.c \
	glut_stroke.c \
	glut_swap.c \
	glut_swidth.c \
	glut_tablet.c \
	glut_teapot.c \
	glut_tr10.c \
	glut_tr24.c \
	glut_util.c \
	glut_vidresize.c \
	glut_warp.c \
	glut_win.c \
	glut_winmisc.c \
	glutbitmap.h \
	glutint.h \
	glutstroke.h \
	layerutil.c \
	layerutil.h \
	stroke.h

libglut_la_LIBADD = $(X_LIBS) $(X_PRE_LIBS) $(X_LIBADD)
capturexfont_LDFLAGS = $(X_LIBS) $(X_PRE_LIBS) $(X_LIBADD)

EXTRA_DIST = descript.mms

strip:
	-if strip -V 2>&1 | grep "GNU" > /dev/null; then \
		strip --strip-unneeded .libs/*.so; \
		strip --strip-debug .libs/*.a; \
	fi

include $(top_srcdir)/common_rules.make
