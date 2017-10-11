## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = gnu no-dependencies

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src


# Build a libtool library for installation in libdir.
lib_LTLIBRARIES = libGL.la

sysconf_DATA = mesa.conf

EXTRA_DIST = tnl_dd mesa.conf descrip.mms vsnprintf.c

libGL_la_LDFLAGS = -version-info @LIBGL_VERSION_INFO@

include libGL_la_SOURCES

if HAVE_X86
DRV_X86 = X86/libMesaX86.la
SUB_X86 = X86
endif

if HAVE_SPARC
DRV_SPARC = SPARC/libMesaSPARC.la
SUB_SPARC = SPARC
endif

if HAVE_FX
DRV_FX = FX/libMesaFX.la
SUB_FX = FX
if HAVE_X86
DRV_FX_X86 = FX/X86/libMesaFX_X86.la
endif
endif

if HAVE_GGI
DRV_GGI = GGI/libMesaGGI.la
SUB_GGI = GGI
endif

if HAVE_OSMESA
DRV_OSMESA = OSmesa/libOSMesa.la
SUB_OSMESA = OSmesa
endif

if HAVE_SVGA
DRV_SVGA = SVGA/libMesaSVGA.la
SUB_SVGA = SVGA
endif

if HAVE_X11
SUB_X11 = X
DRV_X11 = X/libMesaX11.la
endif

if MESA_TRACE
DRV_TRACE = Trace/libMesaTrace.la
SUB_TRACE = Trace
endif


DIST_SUBDIRS = X86 SPARC math swrast swrast_setup Trace tnl FX GGI \
	SVGA X array_cache OSmesa
SUBDIRS = math swrast swrast_setup tnl array_cache $(SUB_X86) $(SUB_SPARC) \
	$(SUB_TRACE) $(SUB_FX) $(SUB_GGI) $(SUB_SVGA) $(SUB_X11) \
        . $(SUB_OSMESA)


libGL_la_LIBADD = \
	math/libMesaMath.la \
	swrast/libMesaSwrast.la \
	swrast_setup/libMesaSwrast_setup.la \
	tnl/libMesaTnl.la \
	array_cache/libMesaAC.la \
	$(DRV_X86) $(DRV_SPARC) $(DRV_TRACE) $(DRV_FX) $(DRV_FX_X86) $(DRV_GGI) \
	$(DRV_SVGA) $(DRV_X11) $(THREAD_LIBS)

strip:
	-if strip -V 2>&1 | grep "GNU" > /dev/null; then \
		strip --strip-unneeded .libs/*.so; \
		strip --strip-debug .libs/*.a; \
	fi

include $(top_srcdir)/common_rules.make
