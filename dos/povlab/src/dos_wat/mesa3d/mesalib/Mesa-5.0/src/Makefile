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
top_srcdir = ..

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

top_builddir = ..

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

# Build a libtool library for installation in libdir.
lib_LTLIBRARIES = libGL.la

sysconf_DATA = mesa.conf

EXTRA_DIST = tnl_dd mesa.conf descrip.mms vsnprintf.c

libGL_la_LDFLAGS = -version-info 5:500:4

libGL_la_SOURCES =  	accum.c 	accum.h 	api_arrayelt.c         api_arrayelt.h 	api_loopback.c 	api_loopback.h 	api_noop.c 	api_noop.h 	api_validate.c 	api_validate.h 	attrib.c 	attrib.h 	blend.c 	blend.h 	buffers.c 	buffers.h 	clip.c 	clip.h 	colormac.h 	colortab.c 	colortab.h 	config.h 	context.c 	context.h 	convolve.c 	convolve.h 	dd.h 	debug.c 	debug.h 	depth.c 	depth.h 	dispatch.c 	dlist.c 	dlist.h 	drawpix.c 	drawpix.h 	enable.c 	enable.h 	enums.c 	enums.h 	eval.c 	eval.h 	extensions.c 	extensions.h 	feedback.c 	feedback.h 	fog.c 	fog.h 	get.c 	get.h 	glapi.c 	glapi.h 	glapioffsets.h 	glapitable.h 	glapitemp.h 	glheader.h 	glthread.c 	glthread.h 	hash.c 	hash.h 	hint.c 	hint.h 	histogram.c 	histogram.h 	image.c 	image.h 	imports.c 	imports.h 	light.c 	light.h 	lines.c 	lines.h 	macros.h 	matrix.c 	matrix.h 	mmath.c 	mmath.h 	mtypes.h 	pixel.c 	pixel.h 	points.c 	points.h 	polygon.c 	polygon.h 	rastpos.c 	rastpos.h 	simple_list.h 	state.c 	state.h 	stencil.c 	stencil.h 	texcompress.c 	texcompress.h 	texformat.c 	texformat.h 	texformat_tmp.h 	teximage.c 	teximage.h 	texobj.c 	texobj.h 	texstate.c 	texstate.h 	texstore.c 	texstore.h 	texutil.c 	texutil.h 	varray.c 	varray.h 	vpexec.c 	vpexec.h 	vpparse.c 	vpparse.h 	vpstate.c 	vpstate.h 	vtxfmt.c 	vtxfmt.h


DRV_X86 = X86/libMesaX86.la
SUB_X86 = X86

#DRV_SPARC = SPARC/libMesaSPARC.la
#SUB_SPARC = SPARC

#DRV_FX = FX/libMesaFX.la
#SUB_FX = FX
#DRV_FX_X86 = FX/X86/libMesaFX_X86.la

DRV_GGI = GGI/libMesaGGI.la
SUB_GGI = GGI

DRV_OSMESA = OSmesa/libOSMesa.la
SUB_OSMESA = OSmesa

#DRV_SVGA = SVGA/libMesaSVGA.la
#SUB_SVGA = SVGA

SUB_X11 = X
DRV_X11 = X/libMesaX11.la

DRV_TRACE = Trace/libMesaTrace.la
SUB_TRACE = Trace

DIST_SUBDIRS = X86 SPARC math swrast swrast_setup Trace tnl FX GGI 	SVGA X array_cache OSmesa

SUBDIRS = math swrast swrast_setup tnl array_cache $(SUB_X86) $(SUB_SPARC) 	$(SUB_TRACE) $(SUB_FX) $(SUB_GGI) $(SUB_SVGA) $(SUB_X11)         . $(SUB_OSMESA)


libGL_la_LIBADD =  	math/libMesaMath.la 	swrast/libMesaSwrast.la 	swrast_setup/libMesaSwrast_setup.la 	tnl/libMesaTnl.la 	array_cache/libMesaAC.la 	$(DRV_X86) $(DRV_SPARC) $(DRV_TRACE) $(DRV_FX) $(DRV_FX_X86) $(DRV_GGI) 	$(DRV_SVGA) $(DRV_X11) $(THREAD_LIBS)

mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../conf.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(lib_LTLIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I..
CPPFLAGS = 
LDFLAGS = 
LIBS = 
X_CFLAGS =  -I/usr/X11R6/include
X_LIBS =  -L/usr/X11R6/lib
X_EXTRA_LIBS = 
X_PRE_LIBS =  -lSM -lICE
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####FX/libMesaFX.la \
####GGI/libMesaGGI.la \
####SVGA/libMesaSVGA.la \
####X/libMesaX11.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####X86/libMesaX86.la \
#####FX/libMesaFX.la \
#####FX/X86/libMesaFX_X86.la \
#####SVGA/libMesaSVGA.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###SPARC/libMesaSPARC.la \
###Trace/libMesaTrace.la \
###X/libMesaX11.la
#######libGL_la_DEPENDENCIES =  \
#######math/libMesaMath.la \
#######swrast/libMesaSwrast.la \
#######swrast_setup/libMesaSwrast_setup.la \
#######tnl/libMesaTnl.la \
#######array_cache/libMesaAC.la \
#######SPARC/libMesaSPARC.la \
#######FX/libMesaFX.la \
#######SVGA/libMesaSVGA.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SPARC/libMesaSPARC.la \
####Trace/libMesaTrace.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SPARC/libMesaSPARC.la \
####FX/libMesaFX.la \
####FX/X86/libMesaFX_X86.la \
####GGI/libMesaGGI.la \
####SVGA/libMesaSVGA.la \
####X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###SPARC/libMesaSPARC.la \
###Trace/libMesaTrace.la \
###GGI/libMesaGGI.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##Trace/libMesaTrace.la \
##X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##SPARC/libMesaSPARC.la \
##Trace/libMesaTrace.la \
##GGI/libMesaGGI.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###GGI/libMesaGGI.la \
###SVGA/libMesaSVGA.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####Trace/libMesaTrace.la \
####FX/libMesaFX.la \
####FX/X86/libMesaFX_X86.la \
####SVGA/libMesaSVGA.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SPARC/libMesaSPARC.la \
###X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####FX/libMesaFX.la \
####X/libMesaX11.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SPARC/libMesaSPARC.la \
#####Trace/libMesaTrace.la \
#####SVGA/libMesaSVGA.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##Trace/libMesaTrace.la \
##FX/libMesaFX.la \
##FX/X86/libMesaFX_X86.la \
##GGI/libMesaGGI.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###Trace/libMesaTrace.la \
###FX/libMesaFX.la \
###X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##SPARC/libMesaSPARC.la \
##Trace/libMesaTrace.la \
##X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####Trace/libMesaTrace.la \
####FX/libMesaFX.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####X86/libMesaX86.la \
#####SPARC/libMesaSPARC.la \
#####FX/libMesaFX.la \
#####FX/X86/libMesaFX_X86.la \
#####GGI/libMesaGGI.la \
#####SVGA/libMesaSVGA.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##SPARC/libMesaSPARC.la \
##Trace/libMesaTrace.la \
##GGI/libMesaGGI.la \
##SVGA/libMesaSVGA.la \
##X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SPARC/libMesaSPARC.la \
###Trace/libMesaTrace.la \
###GGI/libMesaGGI.la \
###SVGA/libMesaSVGA.la
#libGL_la_DEPENDENCIES =  \
#math/libMesaMath.la \
#swrast/libMesaSwrast.la \
#swrast_setup/libMesaSwrast_setup.la \
#tnl/libMesaTnl.la \
#array_cache/libMesaAC.la \
#X86/libMesaX86.la \
#Trace/libMesaTrace.la \
#GGI/libMesaGGI.la \
#SVGA/libMesaSVGA.la \
#X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SPARC/libMesaSPARC.la \
###FX/libMesaFX.la \
###FX/X86/libMesaFX_X86.la \
###GGI/libMesaGGI.la \
###X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SPARC/libMesaSPARC.la \
###Trace/libMesaTrace.la \
###FX/libMesaFX.la \
###FX/X86/libMesaFX_X86.la \
###GGI/libMesaGGI.la \
###SVGA/libMesaSVGA.la \
###X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##GGI/libMesaGGI.la \
##SVGA/libMesaSVGA.la \
##X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SPARC/libMesaSPARC.la \
####X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SPARC/libMesaSPARC.la \
####FX/libMesaFX.la \
####FX/X86/libMesaFX_X86.la \
####GGI/libMesaGGI.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###Trace/libMesaTrace.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SPARC/libMesaSPARC.la \
###Trace/libMesaTrace.la \
###FX/libMesaFX.la \
###FX/X86/libMesaFX_X86.la \
###GGI/libMesaGGI.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####Trace/libMesaTrace.la \
####FX/libMesaFX.la \
####GGI/libMesaGGI.la \
####SVGA/libMesaSVGA.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SPARC/libMesaSPARC.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SPARC/libMesaSPARC.la \
####Trace/libMesaTrace.la \
####FX/libMesaFX.la \
####X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SPARC/libMesaSPARC.la \
####GGI/libMesaGGI.la \
####SVGA/libMesaSVGA.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###FX/libMesaFX.la \
###FX/X86/libMesaFX_X86.la \
###GGI/libMesaGGI.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SPARC/libMesaSPARC.la \
####Trace/libMesaTrace.la \
####FX/libMesaFX.la \
####FX/X86/libMesaFX_X86.la \
####SVGA/libMesaSVGA.la \
####X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####FX/libMesaFX.la \
####FX/X86/libMesaFX_X86.la
#libGL_la_DEPENDENCIES =  \
#math/libMesaMath.la \
#swrast/libMesaSwrast.la \
#swrast_setup/libMesaSwrast_setup.la \
#tnl/libMesaTnl.la \
#array_cache/libMesaAC.la \
#X86/libMesaX86.la \
#Trace/libMesaTrace.la \
#GGI/libMesaGGI.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SPARC/libMesaSPARC.la \
#####SVGA/libMesaSVGA.la \
#####X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SPARC/libMesaSPARC.la \
####GGI/libMesaGGI.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##Trace/libMesaTrace.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SPARC/libMesaSPARC.la \
###Trace/libMesaTrace.la \
###FX/libMesaFX.la \
###FX/X86/libMesaFX_X86.la \
###X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SPARC/libMesaSPARC.la \
####GGI/libMesaGGI.la \
####SVGA/libMesaSVGA.la \
####X/libMesaX11.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####X86/libMesaX86.la \
#####SPARC/libMesaSPARC.la \
#####SVGA/libMesaSVGA.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##SPARC/libMesaSPARC.la \
##GGI/libMesaGGI.la \
##X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SPARC/libMesaSPARC.la \
####Trace/libMesaTrace.la \
####FX/libMesaFX.la \
####FX/X86/libMesaFX_X86.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SPARC/libMesaSPARC.la \
#####Trace/libMesaTrace.la \
#####FX/libMesaFX.la
######libGL_la_DEPENDENCIES =  \
######math/libMesaMath.la \
######swrast/libMesaSwrast.la \
######swrast_setup/libMesaSwrast_setup.la \
######tnl/libMesaTnl.la \
######array_cache/libMesaAC.la \
######SPARC/libMesaSPARC.la \
######FX/libMesaFX.la
######libGL_la_DEPENDENCIES =  \
######math/libMesaMath.la \
######swrast/libMesaSwrast.la \
######swrast_setup/libMesaSwrast_setup.la \
######tnl/libMesaTnl.la \
######array_cache/libMesaAC.la \
######SPARC/libMesaSPARC.la \
######SVGA/libMesaSVGA.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SPARC/libMesaSPARC.la \
####Trace/libMesaTrace.la \
####GGI/libMesaGGI.la \
####SVGA/libMesaSVGA.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SPARC/libMesaSPARC.la \
####Trace/libMesaTrace.la \
####SVGA/libMesaSVGA.la \
####X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##Trace/libMesaTrace.la \
##GGI/libMesaGGI.la \
##SVGA/libMesaSVGA.la \
##X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###FX/libMesaFX.la \
###FX/X86/libMesaFX_X86.la \
###GGI/libMesaGGI.la \
###SVGA/libMesaSVGA.la \
###X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####FX/libMesaFX.la \
####FX/X86/libMesaFX_X86.la \
####GGI/libMesaGGI.la \
####SVGA/libMesaSVGA.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##Trace/libMesaTrace.la \
##FX/libMesaFX.la \
##FX/X86/libMesaFX_X86.la \
##GGI/libMesaGGI.la \
##SVGA/libMesaSVGA.la \
##X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SPARC/libMesaSPARC.la \
###Trace/libMesaTrace.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###Trace/libMesaTrace.la \
###SVGA/libMesaSVGA.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####Trace/libMesaTrace.la \
#####FX/libMesaFX.la \
#####SVGA/libMesaSVGA.la
######libGL_la_DEPENDENCIES =  \
######math/libMesaMath.la \
######swrast/libMesaSwrast.la \
######swrast_setup/libMesaSwrast_setup.la \
######tnl/libMesaTnl.la \
######array_cache/libMesaAC.la \
######SPARC/libMesaSPARC.la \
######FX/libMesaFX.la \
######GGI/libMesaGGI.la \
######SVGA/libMesaSVGA.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###SPARC/libMesaSPARC.la \
###Trace/libMesaTrace.la \
###GGI/libMesaGGI.la \
###SVGA/libMesaSVGA.la \
###X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####FX/libMesaFX.la \
####GGI/libMesaGGI.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####Trace/libMesaTrace.la \
####FX/libMesaFX.la \
####SVGA/libMesaSVGA.la \
####X/libMesaX11.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SPARC/libMesaSPARC.la \
#####FX/libMesaFX.la \
#####GGI/libMesaGGI.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SPARC/libMesaSPARC.la \
#####GGI/libMesaGGI.la \
#####SVGA/libMesaSVGA.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###SPARC/libMesaSPARC.la \
###GGI/libMesaGGI.la \
###X/libMesaX11.la
#libGL_la_DEPENDENCIES =  \
#math/libMesaMath.la \
#swrast/libMesaSwrast.la \
#swrast_setup/libMesaSwrast_setup.la \
#tnl/libMesaTnl.la \
#array_cache/libMesaAC.la \
#X86/libMesaX86.la \
#Trace/libMesaTrace.la \
#X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SPARC/libMesaSPARC.la
#libGL_la_DEPENDENCIES =  \
#math/libMesaMath.la \
#swrast/libMesaSwrast.la \
#swrast_setup/libMesaSwrast_setup.la \
#tnl/libMesaTnl.la \
#array_cache/libMesaAC.la \
#X86/libMesaX86.la \
#Trace/libMesaTrace.la \
#FX/libMesaFX.la \
#FX/X86/libMesaFX_X86.la \
#GGI/libMesaGGI.la \
#X/libMesaX11.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####FX/libMesaFX.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####GGI/libMesaGGI.la \
####SVGA/libMesaSVGA.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##FX/libMesaFX.la \
##FX/X86/libMesaFX_X86.la \
##GGI/libMesaGGI.la \
##X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###SPARC/libMesaSPARC.la \
###Trace/libMesaTrace.la \
###FX/libMesaFX.la \
###GGI/libMesaGGI.la \
###X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SPARC/libMesaSPARC.la \
####Trace/libMesaTrace.la \
####FX/libMesaFX.la \
####GGI/libMesaGGI.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SPARC/libMesaSPARC.la \
###GGI/libMesaGGI.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SVGA/libMesaSVGA.la \
####X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###Trace/libMesaTrace.la \
###FX/libMesaFX.la \
###FX/X86/libMesaFX_X86.la \
###GGI/libMesaGGI.la \
###SVGA/libMesaSVGA.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SPARC/libMesaSPARC.la \
#####Trace/libMesaTrace.la \
#####FX/libMesaFX.la \
#####GGI/libMesaGGI.la \
#####SVGA/libMesaSVGA.la
######libGL_la_DEPENDENCIES =  \
######math/libMesaMath.la \
######swrast/libMesaSwrast.la \
######swrast_setup/libMesaSwrast_setup.la \
######tnl/libMesaTnl.la \
######array_cache/libMesaAC.la \
######SPARC/libMesaSPARC.la \
######FX/libMesaFX.la \
######SVGA/libMesaSVGA.la \
######X/libMesaX11.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####FX/libMesaFX.la \
#####SVGA/libMesaSVGA.la \
#####X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SVGA/libMesaSVGA.la \
###X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##Trace/libMesaTrace.la \
##GGI/libMesaGGI.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SPARC/libMesaSPARC.la \
####SVGA/libMesaSVGA.la \
####X/libMesaX11.la
######libGL_la_DEPENDENCIES =  \
######math/libMesaMath.la \
######swrast/libMesaSwrast.la \
######swrast_setup/libMesaSwrast_setup.la \
######tnl/libMesaTnl.la \
######array_cache/libMesaAC.la \
######X86/libMesaX86.la \
######SPARC/libMesaSPARC.la \
######FX/libMesaFX.la \
######FX/X86/libMesaFX_X86.la \
######SVGA/libMesaSVGA.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SPARC/libMesaSPARC.la \
#####FX/libMesaFX.la \
#####X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###Trace/libMesaTrace.la \
###FX/libMesaFX.la \
###FX/X86/libMesaFX_X86.la
libGL_la_DEPENDENCIES =  \
math/libMesaMath.la \
swrast/libMesaSwrast.la \
swrast_setup/libMesaSwrast_setup.la \
tnl/libMesaTnl.la \
array_cache/libMesaAC.la \
X86/libMesaX86.la \
Trace/libMesaTrace.la \
GGI/libMesaGGI.la \
X/libMesaX11.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####X86/libMesaX86.la \
#####SPARC/libMesaSPARC.la \
#####FX/libMesaFX.la \
#####FX/X86/libMesaFX_X86.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####X86/libMesaX86.la \
#####SPARC/libMesaSPARC.la \
#####FX/libMesaFX.la \
#####FX/X86/libMesaFX_X86.la \
#####SVGA/libMesaSVGA.la \
#####X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SVGA/libMesaSVGA.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SPARC/libMesaSPARC.la \
####Trace/libMesaTrace.la \
####FX/libMesaFX.la \
####FX/X86/libMesaFX_X86.la \
####GGI/libMesaGGI.la \
####SVGA/libMesaSVGA.la
#libGL_la_DEPENDENCIES =  \
#math/libMesaMath.la \
#swrast/libMesaSwrast.la \
#swrast_setup/libMesaSwrast_setup.la \
#tnl/libMesaTnl.la \
#array_cache/libMesaAC.la \
#X86/libMesaX86.la \
#GGI/libMesaGGI.la \
#X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SPARC/libMesaSPARC.la \
###Trace/libMesaTrace.la \
###SVGA/libMesaSVGA.la \
###X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###SPARC/libMesaSPARC.la \
###GGI/libMesaGGI.la \
###SVGA/libMesaSVGA.la \
###X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###Trace/libMesaTrace.la \
###GGI/libMesaGGI.la \
###SVGA/libMesaSVGA.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####FX/libMesaFX.la \
#####GGI/libMesaGGI.la \
#####SVGA/libMesaSVGA.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####FX/libMesaFX.la \
####FX/X86/libMesaFX_X86.la \
####SVGA/libMesaSVGA.la \
####X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###GGI/libMesaGGI.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###GGI/libMesaGGI.la \
###SVGA/libMesaSVGA.la \
###X/libMesaX11.la
#libGL_la_DEPENDENCIES =  \
#math/libMesaMath.la \
#swrast/libMesaSwrast.la \
#swrast_setup/libMesaSwrast_setup.la \
#tnl/libMesaTnl.la \
#array_cache/libMesaAC.la \
#Trace/libMesaTrace.la \
#GGI/libMesaGGI.la \
#X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##Trace/libMesaTrace.la \
##SVGA/libMesaSVGA.la \
##X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##SPARC/libMesaSPARC.la \
##Trace/libMesaTrace.la \
##GGI/libMesaGGI.la \
##X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###Trace/libMesaTrace.la \
###SVGA/libMesaSVGA.la \
###X/libMesaX11.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SVGA/libMesaSVGA.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SPARC/libMesaSPARC.la \
####FX/libMesaFX.la \
####FX/X86/libMesaFX_X86.la \
####X/libMesaX11.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####X86/libMesaX86.la \
#####SPARC/libMesaSPARC.la \
#####Trace/libMesaTrace.la \
#####FX/libMesaFX.la \
#####FX/X86/libMesaFX_X86.la \
#####SVGA/libMesaSVGA.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SPARC/libMesaSPARC.la \
#####Trace/libMesaTrace.la \
#####FX/libMesaFX.la \
#####SVGA/libMesaSVGA.la \
#####X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###FX/libMesaFX.la \
###GGI/libMesaGGI.la \
###X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####Trace/libMesaTrace.la \
####SVGA/libMesaSVGA.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###Trace/libMesaTrace.la \
###FX/libMesaFX.la \
###GGI/libMesaGGI.la \
###SVGA/libMesaSVGA.la \
###X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##Trace/libMesaTrace.la \
##FX/libMesaFX.la \
##FX/X86/libMesaFX_X86.la \
##X/libMesaX11.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###Trace/libMesaTrace.la \
###FX/libMesaFX.la \
###GGI/libMesaGGI.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####X86/libMesaX86.la \
####SPARC/libMesaSPARC.la \
####Trace/libMesaTrace.la \
####SVGA/libMesaSVGA.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##Trace/libMesaTrace.la \
##GGI/libMesaGGI.la \
##SVGA/libMesaSVGA.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###FX/libMesaFX.la \
###FX/X86/libMesaFX_X86.la \
###X/libMesaX11.la
######libGL_la_DEPENDENCIES =  \
######math/libMesaMath.la \
######swrast/libMesaSwrast.la \
######swrast_setup/libMesaSwrast_setup.la \
######tnl/libMesaTnl.la \
######array_cache/libMesaAC.la \
######SPARC/libMesaSPARC.la \
######Trace/libMesaTrace.la \
######FX/libMesaFX.la \
######SVGA/libMesaSVGA.la
######libGL_la_DEPENDENCIES =  \
######math/libMesaMath.la \
######swrast/libMesaSwrast.la \
######swrast_setup/libMesaSwrast_setup.la \
######tnl/libMesaTnl.la \
######array_cache/libMesaAC.la \
######FX/libMesaFX.la \
######SVGA/libMesaSVGA.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X86/libMesaX86.la \
###Trace/libMesaTrace.la \
###FX/libMesaFX.la \
###FX/X86/libMesaFX_X86.la \
###SVGA/libMesaSVGA.la \
###X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##SPARC/libMesaSPARC.la \
##Trace/libMesaTrace.la \
##FX/libMesaFX.la \
##FX/X86/libMesaFX_X86.la \
##GGI/libMesaGGI.la \
##X/libMesaX11.la
#####libGL_la_DEPENDENCIES =  \
#####math/libMesaMath.la \
#####swrast/libMesaSwrast.la \
#####swrast_setup/libMesaSwrast_setup.la \
#####tnl/libMesaTnl.la \
#####array_cache/libMesaAC.la \
#####SPARC/libMesaSPARC.la \
#####FX/libMesaFX.la \
#####GGI/libMesaGGI.la \
#####SVGA/libMesaSVGA.la \
#####X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##GGI/libMesaGGI.la \
##X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SPARC/libMesaSPARC.la \
####Trace/libMesaTrace.la \
####FX/libMesaFX.la \
####GGI/libMesaGGI.la \
####SVGA/libMesaSVGA.la \
####X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##X86/libMesaX86.la \
##GGI/libMesaGGI.la
###libGL_la_DEPENDENCIES =  \
###math/libMesaMath.la \
###swrast/libMesaSwrast.la \
###swrast_setup/libMesaSwrast_setup.la \
###tnl/libMesaTnl.la \
###array_cache/libMesaAC.la \
###X/libMesaX11.la
#libGL_la_DEPENDENCIES =  \
#math/libMesaMath.la \
#swrast/libMesaSwrast.la \
#swrast_setup/libMesaSwrast_setup.la \
#tnl/libMesaTnl.la \
#array_cache/libMesaAC.la \
#X86/libMesaX86.la \
#SPARC/libMesaSPARC.la \
#Trace/libMesaTrace.la \
#GGI/libMesaGGI.la \
#X/libMesaX11.la
####libGL_la_DEPENDENCIES =  \
####math/libMesaMath.la \
####swrast/libMesaSwrast.la \
####swrast_setup/libMesaSwrast_setup.la \
####tnl/libMesaTnl.la \
####array_cache/libMesaAC.la \
####SPARC/libMesaSPARC.la \
####FX/libMesaFX.la \
####GGI/libMesaGGI.la \
####X/libMesaX11.la
##libGL_la_DEPENDENCIES =  \
##math/libMesaMath.la \
##swrast/libMesaSwrast.la \
##swrast_setup/libMesaSwrast_setup.la \
##tnl/libMesaTnl.la \
##array_cache/libMesaAC.la \
##Trace/libMesaTrace.la \
##FX/libMesaFX.la \
##GGI/libMesaGGI.la \
##X/libMesaX11.la
libGL_la_OBJECTS =  accum.lo api_arrayelt.lo api_loopback.lo api_noop.lo \
api_validate.lo attrib.lo blend.lo buffers.lo clip.lo colortab.lo \
context.lo convolve.lo debug.lo depth.lo dispatch.lo dlist.lo \
drawpix.lo enable.lo enums.lo eval.lo extensions.lo feedback.lo fog.lo \
get.lo glapi.lo glthread.lo hash.lo hint.lo histogram.lo image.lo \
imports.lo light.lo lines.lo matrix.lo mmath.lo pixel.lo points.lo \
polygon.lo rastpos.lo state.lo stencil.lo texcompress.lo texformat.lo \
teximage.lo texobj.lo texstate.lo texstore.lo texutil.lo varray.lo \
vpexec.lo vpparse.lo vpstate.lo vtxfmt.lo
CFLAGS = -g -O2 -Wall -O0 -DDEBUG -DMESA_TRACE -ansi -pedantic -D_REENTRANT -DPTHREADS
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
DATA =  $(sysconf_DATA)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best
SOURCES = $(libGL_la_SOURCES)
OBJECTS = $(libGL_la_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .lo .o .obj .s
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) $(srcdir)/libGL_la_SOURCES $(top_srcdir)/common_rules.make
	cd $(top_srcdir) && $(AUTOMAKE) --gnu src/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-libLTLIBRARIES:

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)

distclean-libLTLIBRARIES:

maintainer-clean-libLTLIBRARIES:

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "$(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

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

libGL.la: $(libGL_la_OBJECTS) $(libGL_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libGL_la_LDFLAGS) $(libGL_la_OBJECTS) $(libGL_la_LIBADD) $(LIBS)

install-sysconfDATA: $(sysconf_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)
	@list='$(sysconf_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(sysconfdir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(sysconfdir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(sysconfdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(sysconfdir)/$$p; \
	  fi; fi; \
	done

uninstall-sysconfDATA:
	@$(NORMAL_UNINSTALL)
	list='$(sysconf_DATA)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(sysconfdir)/$$p; \
	done

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.



all-recursive install-data-recursive install-exec-recursive \
installdirs-recursive install-recursive uninstall-recursive  \
check-recursive installcheck-recursive info-recursive dvi-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"

mostlyclean-recursive clean-recursive distclean-recursive \
maintainer-clean-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	dot_seen=no; \
	rev=''; list='$(SUBDIRS)'; for subdir in $$list; do \
	  rev="$$subdir $$rev"; \
	  test "$$subdir" != "." || dot_seen=yes; \
	done; \
	test "$$dot_seen" = "no" && rev=". $$rev"; \
	target=`echo $@ | sed s/-recursive//`; \
	for subdir in $$rev; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
tags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) tags); \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS: tags-recursive $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
   if test "$$subdir" = .; then :; else \
	    test -f $$subdir/TAGS && tags="$$tags -i $$here/$$subdir/TAGS"; \
   fi; \
	done; \
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

subdir = src

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
	for subdir in $(DIST_SUBDIRS); do \
	  if test "$$subdir" = .; then :; else \
	    test -d $(distdir)/$$subdir \
	    || mkdir $(distdir)/$$subdir \
	    || exit 1; \
	    chmod 777 $(distdir)/$$subdir; \
	    (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) top_distdir=../$(top_distdir) distdir=../$(distdir)/$$subdir distdir) \
	      || exit 1; \
	  fi; \
	done
info-am:
info: info-recursive
dvi-am:
dvi: dvi-recursive
check-am: all-am
check: check-recursive
installcheck-am:
installcheck: installcheck-recursive
install-exec-am: install-libLTLIBRARIES install-sysconfDATA
install-exec: install-exec-recursive

install-data-am:
install-data: install-data-recursive

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-recursive
uninstall-am: uninstall-libLTLIBRARIES uninstall-sysconfDATA
uninstall: uninstall-recursive
all-am: Makefile $(LTLIBRARIES) $(DATA)
all-redirect: all-recursive
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs: installdirs-recursive
installdirs-am:
	$(mkinstalldirs)  $(DESTDIR)$(libdir) $(DESTDIR)$(sysconfdir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-libLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags \
		mostlyclean-generic

mostlyclean: mostlyclean-recursive

clean-am:  clean-libLTLIBRARIES clean-compile clean-libtool clean-tags \
		clean-generic mostlyclean-am

clean: clean-recursive

distclean-am:  distclean-libLTLIBRARIES distclean-compile \
		distclean-libtool distclean-tags distclean-generic \
		clean-am
	-rm -f libtool

distclean: distclean-recursive

maintainer-clean-am:  maintainer-clean-libLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-generic \
		distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-recursive

.PHONY: mostlyclean-libLTLIBRARIES distclean-libLTLIBRARIES \
clean-libLTLIBRARIES maintainer-clean-libLTLIBRARIES \
uninstall-libLTLIBRARIES install-libLTLIBRARIES mostlyclean-compile \
distclean-compile clean-compile maintainer-clean-compile \
mostlyclean-libtool distclean-libtool clean-libtool \
maintainer-clean-libtool uninstall-sysconfDATA install-sysconfDATA \
install-data-recursive uninstall-data-recursive install-exec-recursive \
uninstall-exec-recursive installdirs-recursive uninstalldirs-recursive \
all-recursive check-recursive installcheck-recursive info-recursive \
dvi-recursive mostlyclean-recursive distclean-recursive clean-recursive \
maintainer-clean-recursive tags tags-recursive mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags distdir info-am info \
dvi-am dvi check check-am installcheck-am installcheck install-exec-am \
install-exec install-data-am install-data install-am install \
uninstall-am uninstall all-redirect all-am all installdirs-am \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


strip:
	-if strip -V 2>&1 | grep "GNU" > /dev/null; then \
		strip --strip-unneeded .libs/*.so; \
		strip --strip-debug .libs/*.a; \
	fi

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
