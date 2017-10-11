AUTOMAKE_OPTION = @NO_DEPENDENCIES@

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src/GGI/include \
	-I$(top_srcdir)/src $(GGI_CFLAGS)


sublibdir = $(libdir)/ggi/mesa/default/

if HAVE_GGI_GENKGI
KGILIB = genkgi.la
else
KGILIB =
endif

if HAVE_GGI
sublib_LTLIBRARIES = stubs.la linear_8.la linear_15.la \
	linear_16.la linear_24.la linear_32.la $(KGILIB)
endif


genkgi_la_SOURCES = genkgi_visual.c genkgi_mode.c
genkgi_la_LDFLAGS = -module -no-undefined -avoid-verison \
	-export-symbols-regex "GGIMesa.*"

stubs_la_SOURCES = stubs.c
stubs_la_LDFLAGS = -module -no-undefined -avoid-verison \
	-export-symbols-regex "GGIMesa.*"

linear_8_la_SOURCES = linear_8.c
linear_8_la_LDFLAGS = -module -no-undefined -avoid-verison \
	-export-symbols-regex "GGIMesa.*"

linear_15_la_SOURCES = linear_15.c
linear_15_la_LDFLAGS = -module -no-undefined -avoid-verison \
	-export-symbols-regex "GGIMesa.*"

linear_16_la_SOURCES = linear_16.c
linear_16_la_LDFLAGS = -module -no-undefined -avoid-verison \
	-export-symbols-regex "GGIMesa.*"

linear_24_la_SOURCES = linear_24.c
linear_24_la_LDFLAGS = -module -no-undefined -avoid-verison \
	-export-symbols-regex "GGIMesa.*"

linear_32_la_SOURCES = linear_32.c
linear_32_la_LDFLAGS = -module -no-undefined -avoid-verison \
	-export-symbols-regex "GGIMesa.*"

genkgiconfdir = $(sysconfdir)/ggi/mesa/targets
genkgiconf_DATA = genkgi.conf

DISTCLEANFILES = genkgi.conf

EXTRA_DIST = genkgi.h linear.c


include $(top_srcdir)/common_rules.make
