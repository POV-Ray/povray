AUTOMAKE_OPTIONS = 

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src/GGI/include \
	-I$(top_srcdir)/src $(GGI_CFLAGS)

if HAVE_GGI_FBDEV
sublibdir = $(libdir)/ggi/mesa/display/
sublib_LTLIBRARIES = fbdev.la
endif

fbdev_la_SOURCES = fbdev_visual.c fbdev_mode.c
fbdev_la_LDFLAGS = -module -no-undefined -avoid-version

fbdevconfdir = $(sysconfdir)/ggi/mesa/targets
fbdevconf_DATA = fbdev.conf

DISTCLEANFILES = fbdev.conf


include $(top_srcdir)/common_rules.make
