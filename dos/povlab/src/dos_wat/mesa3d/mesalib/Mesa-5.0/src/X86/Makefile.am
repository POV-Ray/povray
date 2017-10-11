## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS =

x86_files = \
	matypes.h \
	assyntax.h \
	clip_args.h \
	common_x86.c \
	common_x86_asm.S \
	common_x86_asm.h \
	common_x86_features.h \
	common_x86_macros.h \
	glapi_x86.S \
	x86.c \
	x86.h \
	x86_cliptest.S \
	x86_xform2.S \
	x86_xform3.S \
	x86_xform4.S \
	xform_args.h

x3dnow_files = \
	3dnow.c \
	3dnow.h \
	3dnow_normal.S \
	3dnow_xform1.S \
	3dnow_xform2.S \
	3dnow_xform3.S \
	3dnow_xform4.S

sse_files = \
	sse.c \
	sse.h \
	sse_normal.S \
	sse_xform1.S \
	sse_xform2.S \
	sse_xform3.S \
	sse_xform4.S

mmx_files = mmx.h mmx_blend.S

if HAVE_3DNOW
x3dnow_sources = $(x3dnow_files)
CFLAGS_3DNOW = -DUSE_3DNOW_ASM
endif

if HAVE_SSE
sse_sources = $(sse_files)
CFLAGS_SSE = -DUSE_SSE_ASM
endif

if HAVE_MMX
mmx_sources = $(mmx_files)
CFLAGS_MMX = -DUSE_MMX_ASM
endif

if HAVE_X86
# Build a libtool convenience library.
noinst_LTLIBRARIES = libMesaX86.la
CFLAGS_X86 = -DUSE_X86_ASM
endif
x86_sources = $(x86_files)

# matypes stuff
noinst_PROGRAMS = gen_matypes
gen_matypes_SOURCES = gen_matypes.c

# RM 20020614:
# for this to work with Cygwin NT 1.3.10, you need to specify the extension explicitly
# cf URL http://www.gnu.org/manual/automake-1.5/html_node/automake_43.html
# matypes.h: gen_matypes ../mtypes.h
matypes.h: gen_matypes$(EXEEXT) ../mtypes.h
	./gen_matypes > matypes.h
BUILT_SOURCES = matypes.h

libMesaX86_la_SOURCES = $(x86_sources) $(mmx_sources) $(x3dnow_sources) $(sse_sources)

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src $(CFLAGS_X86) $(CFLAGS_MMX) $(CFLAGS_3DNOW) $(CFLAGS_SSE)

EXTRA_DIST = $(x3dnow_files) $(mmx_files) $(sse_files)


include $(top_srcdir)/common_rules.make
