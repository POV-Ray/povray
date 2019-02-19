##############################################################################
#
# configure.ac
#
# Process this file with autoconf to produce the configure script.
#
# ---------------------------------------------------------------------------
#
# Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
# Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
#
# POV-Ray is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# POV-Ray is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#---------------------------------------------------------------------------
#
# POV-Ray is based on the popular DKB raytracer version 2.12.
# DKBTrace was originally written by David K. Buck.
# DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
#
##############################################################################

# configure.ac for the source distribution of POV-Ray v3.8 for UNIX
# Report bugs via https://github.com/POV-Ray/povray/issues
#
# Based on configure.ac from POV-Ray v3.7
# Written by Nicolas Calimet <nicolas.calimet@povray.org>
# Based on configure.ac from POV-Ray v3.6

# Required autoconf version.
AC_PREREQ(2.68)

# Inits.

# The file VERSION must contain only the version string MAJOR.MINOR.REVISION(.PATCHLEVEL)(-PRERELEASE)
AC_INIT([POV-Ray], m4_normalize(m4_include([VERSION])), [unix-bugreports-2011@povray.org], [povray])

# VERSION_BASE is then defined as MAJOR.MINOR
VERSION_BASE=`echo $PACKAGE_VERSION | sed 's,\([[0-9]]*.[[0-9]]*\).*,\1,g'`
AC_SUBST([VERSION_BASE])
AC_DEFINE_UNQUOTED([VERSION_BASE], ["$VERSION_BASE"], [Base version number of package.])
AC_CONFIG_AUX_DIR([unix/config])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([unix/disp_text.cpp])

# Additional autoconf macros.
# Those required by other macros go first.
m4_include([unix/config/ax_require_defined.m4])
# The remainder we're sorting alphabetically.
m4_include([unix/config/ax_pthread.m4])
m4_include([unix/config/ax_arg_enable.m4])
m4_include([unix/config/ax_arg_with.m4])
m4_include([unix/config/ax_boost_base.m4])
m4_include([unix/config/ax_test_compiler_flags.m4])
m4_include([unix/config/ax_check_compile_flag.m4])
m4_include([unix/config/ax_check_lib.m4])
m4_include([unix/config/ax_check_libjpeg.m4])
m4_include([unix/config/ax_check_libsdl.m4])
m4_include([unix/config/ax_check_libtiff.m4])
m4_include([unix/config/ax_check_openexr.m4])
m4_include([unix/config/ax_compare_version.m4])
m4_include([unix/config/ax_compiler_vendor.m4])
m4_include([unix/config/ax_compiler_version.m4])
m4_include([unix/config/ax_cxx_compile_stdcxx.m4])
m4_include([unix/config/ax_fix_incorrect_path.m4])
m4_include([unix/config/ax_prog_ld_static.m4])
m4_include([unix/config/ax_x86_arch.m4])

# Required versions of the support libraries.
# Must be declared after AC_INIT.
required_libboost_version="1.38"
required_libz_version="1.2.1"
required_libpng_version="1.2.5"
required_libjpeg_version="6b"
required_libtiff_version="3.6.1"
required_libsdl_version="1.2"
required_openexr_version="1.2"

# Supported options.
# AX_ARG_ENABLE([--enable-avx2],             [enable AVX2 features and compiler optmization])
AX_ARG_ENABLE([--enable-watch-cursor],     [enable a watch cursor over the display while rendering (X Window only)])
AX_ARG_ENABLE([--enable-debug],            [enable compiler debugging mode])
AX_ARG_ENABLE([--enable-profile],          [enable program execution profiling])
AX_ARG_ENABLE([--enable-static],           [enable the linker to create static executable (no run-time dependancy to external libraries, larger binary)])
AX_ARG_ENABLE([--disable-optimiz],         [disable all compiler optimizations])
AX_ARG_ENABLE([--disable-optimiz-arch],    [disable architecture-specific compiler optimizations])
AX_ARG_ENABLE([--disable-pipe],            [disable usage of pipes during compilation (use temporary files instead)])
AX_ARG_ENABLE([--disable-strip],           [disable stripping all symbols from the object files (larger binary)])
AX_ARG_ENABLE([--disable-shared],          [disable linking with shared libraries (same as --enable-static)])
AX_ARG_ENABLE([--disable-io-restrictions], [disable POV-Ray's mechanism for control of I/O operations])

AX_ARG_WITH([--without-cygwin-dll], [],    [don't link with the Cygwin DLL (MinGW required)])
AX_ARG_WITH([--with-zlib],          [DIR], [use the ZLIB library (in directory DIR)])
AX_ARG_WITH([--with-libpng],        [DIR], [use the PNG library (in directory DIR)])
AX_ARG_WITH([--with-libjpeg],       [DIR], [use the JPEG library (in directory DIR)])
AX_ARG_WITH([--with-libtiff],       [DIR], [use the TIFF library (in directory DIR)])
AX_ARG_WITH([--with-libsdl],        [DIR], [use the SDL library (in directory DIR)])
AX_ARG_WITH([--with-libmkl],        [DIR], [use the Intel(R) Math Kernel Library (in directory DIR)])
AX_ARG_WITH([--with-openexr],       [DIR], [use the OpenEXR library (in directory DIR)]) 

AC_ARG_VAR([COMPILED_BY], [customize the "unofficial version" message])
AC_ARG_VAR([NON_REDISTRIBUTABLE_BUILD], [see the installation documentation])
AC_ARG_VAR([C99_COMPATIBLE_RADIOSITY], [adapt radiosity code to non-IEEE 754 floating point (see configcore.h for details)])


###############################################################################

# Remove povray.ini unconditionally.
rm -f ./povray.ini


# Instruction message.
if test x"$COMPILED_BY" = x""; then
  COMPILED_BY="$USER <no contact address>"
fi

pov_no_distrib="${NON_REDISTRIBUTABLE_BUILD:-no}"

# Don't use 'echo' (not saved into config.log / doesn't honor --silent).
# Don't use AC_MSG_NOTICE which prints things first.
AC_MSG_RESULT([
===============================================================================
Configure $PACKAGE_NAME version $PACKAGE_VERSION
===============================================================================

This is an unofficial version compiled by:
 $COMPILED_BY
The POV-Ray Team(tm) is not responsible for supporting this version.])

AC_DEFINE_UNQUOTED([BUILT_BY], ["$COMPILED_BY"], [Who compiled this binary.])


###############################################################################

AC_MSG_RESULT([
Environment
-----------])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.9 dist-bzip2 subdir-objects])
AM_MAINTAINER_MODE

AX_FIX_INCORRECT_PATH(C_INCLUDE_PATH, ".", [pov_warn_path="$pov_warn_path C_INCLUDE_PATH"])
AX_FIX_INCORRECT_PATH(CPLUS_INCLUDE_PATH, ".", [pov_warn_path="$pov_warn_path CPLUS_INCLUDE_PATH"])

AM_PROG_AR


###############################################################################

AC_MSG_RESULT([
Programs
--------])

# Check for the C compiler (used in some library checks).
# Init compiler flags to avoid defaults such as '-g -O2'.
CFLAGS="$CFLAGS"
AC_PROG_CC
AC_PROG_CPP

# Force checking essential (ANSI-compliant) headers.
AC_CHECK_HEADERS([stdlib.h])


# Check for the C++ compiler and version.
# Init compiler flags to avoid defaults such as '-g -O2'.
AC_LANG([C++])
CXXFLAGS="$CXXFLAGS"
AC_PROG_CXX
AC_MSG_CHECKING([whether the $CXX compiler works])
AC_LINK_IFELSE([AC_LANG_PROGRAM],
  [AC_MSG_RESULT([yes])],
  [
    AC_MSG_RESULT([no])
    AC_MSG_FAILURE([C++ compiler cannot create executables])
  ]
)
AC_PROG_CXXCPP
AX_CXX_COMPILE_STDCXX(11)

AX_COMPILER_VENDOR
AX_COMPILER_VERSION
pov_compiler=`echo $CXX | sed 's,.*/,,'`  # equivalent to `basename $CXX`
pov_compiler_vendor="$ax_cv_cxx_compiler_vendor"
pov_compiler_version="$pov_compiler"
if test "$ax_compiler_version"; then
  pov_compiler_version="$pov_compiler $ax_compiler_version"
fi

pov_built_for="$host"  # default
AC_DEFINE_UNQUOTED([BUILD_ARCH], ["$build"], [Build architecture.])
AC_DEFINE_UNQUOTED([COMPILER_VENDOR], ["$pov_compiler_vendor"], [Compiler vendor.])
AC_DEFINE_UNQUOTED([COMPILER_VERSION], ["$pov_compiler_version"], [Compiler version.])
AC_DEFINE_UNQUOTED([POV_COMPILER_INFO], ["$pov_compiler_version @ $build"], [Compiler version (processed).])


# Not necessary when AM_MAINTAINER_MODE is called above.
###AC_PROG_MAKE_SET

AC_PROG_RANLIB


AC_MSG_CHECKING([for stat format option])
echo "trying stat --format='%u,%g' ." >>config.log
if stat --format='%u,%g' . >>config.log 2>>config.log && test x`stat --format=Foo .` = x"Foo"; then
  stat_format="--format="
else
  echo "trying stat --f '%u,%g' ." >>config.log
  if stat -f '%u,%g' . >>config.log 2>>config.log && test x`stat -f Foo .` = x"Foo"; then
    stat_format="-f "
  else
    AC_MSG_FAILURE([stat command does not behave as required])
  fi
fi
AC_MSG_RESULT([$stat_format])


###############################################################################

AC_MSG_RESULT([
Libraries
---------])

# Link with or without the cygwin DLL.
AC_MSG_CHECKING([whether to link with cygwin DLL])
if test x"$with_cygwin_dll" = x"no"; then
  AC_MSG_RESULT([no])
  case "$build" in
    *cygwin*)
      pov_flags="-mno-cygwin"
      pov_cygwin_save_cxxflags="$CXXFLAGS"
      AX_TEST_COMPILER_FLAGS([$pov_flags],
        [
          AC_CHECK_LIB([mingw32], [main],
            [
              LDFLAGS="$LDFLAGS $pov_flags"
              # -D__CYGWIN is required for tiffio.h (incl. in tiff_pov.cpp)
              CPPFLAGS="$CPPFLAGS -D__CYGWIN $pov_flags" 
            ],
            [with_cygwin_dll="yes"]
          )
        ],
        [with_cygwin_dll="yes"]
      )
      if test x"$with_cygwin_dll" != x"no"; then
        AC_MSG_WARN([Cannot link without Cygwin DLL])
        CXXFLAGS="$pov_cygwin_save_cxxflags"
      fi
    ;;
  esac
else
  case "$build" in
    *cygwin*)
      AC_MSG_RESULT([yes])
    ;;
    *)
      AC_MSG_RESULT([no])
    ;;
  esac
fi

# Link with static library, update LDFLAGS.
if test x"$enable_static" = x"yes" && test x"$enable_shared" != x"yes"; then
  enable_shared="no"
fi
AC_MSG_CHECKING([whether to enable static linking])
if test x"$enable_shared" = x"no"; then
  AC_MSG_RESULT([yes])
  AX_PROG_LD_STATIC
else
  AC_MSG_RESULT([no])
fi

# Try to use the -v compiler flag for better debugging.
if test ! `$CXX -v < /dev/null 2> /dev/null`; then
  pov_lib_save_cxxflags="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -v"
  AC_LINK_IFELSE([AC_LANG_PROGRAM()])
  CXXFLAGS="$pov_lib_save_cxxflags"
fi

# Posix threads
AX_PTHREAD
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"  # append
LIBS="$LIBS $PTHREAD_CFLAGS $PTHREAD_LIBS"  # append

# Boost; required library
# the following macro stops with error when boost is not found
AX_BOOST_BASE([$required_libboost_version])

AC_DEFINE([USE_OFFICIAL_BOOST], [], [Use the official Boost libraries.])

# Intel Math Kernel library
pov_save_ldflags="$LDFLAGS"
test x"$with_libmkl" != x"" && test x"$with_libmkl" != x"no" && LDFLAGS="-L$with_libmkl $LDFLAGS"
AC_CHECK_LIB([mkl], [sin]) 
test x"$ac_cv_lib_mkl_sin" = x"no" && LDFLAGS="$pov_save_ldflags"

# libm
AC_CHECK_LIB([m], [sin])

# real-time clock
AC_CHECK_LIB([rt], [clock_gettime])

# libz; required library for distributable builds
AC_MSG_CHECKING([whether to use the ZLIB library])
if test x"$pov_no_distrib" = x"yes" && test x"$with_zlib" = x"no"; then
  AC_MSG_RESULT([no (NON DISTRIBUTABLE BUILD)])
  AC_MSG_WARN([all program features using the ZLIB library are disabled])
  AC_DEFINE([LIBZ_MISSING], [], [Don't use libz.])
else
  if test x"$with_zlib" = x"no"; then
    AC_MSG_ERROR([disabling support for ZLIB requires NON_REDISTRIBUTABLE_BUILD=yes])
  fi
  AC_MSG_RESULT([yes])
  AX_CHECK_LIB([z], [$required_libz_version], [z], [zlibVersion], [zlib.h], [zlibVersion()], [$with_zlib])
  if test x"$ax_check_lib" != x"ok"; then
    AC_MSG_ERROR([cannot find a suitable ZLIB library])
  else
    pov_libz=ok
  fi
fi

# libpng; required library for distributable builds
AC_MSG_CHECKING([whether to use the PNG library])
if test x"$pov_no_distrib" = x"yes" && test x"$with_libpng" = x"no"; then
  AC_MSG_RESULT([no (NON DISTRIBUTABLE BUILD)])
  AC_MSG_WARN([all program features using the PNG library are disabled])
  AC_DEFINE([LIBPNG_MISSING], [], [Don't use libpng.])
else
  if test x"$with_libpng" = x"no"; then
    AC_MSG_ERROR([disabling support for PNG requires NON_REDISTRIBUTABLE_BUILD=yes])
  fi
  AC_MSG_RESULT([yes])
  AX_CHECK_LIB([png], [$required_libpng_version], [png14 png png12 png], [png_get_libpng_ver], [png.h], [png_get_libpng_ver(NULL)], [$with_libpng])
  ### FIXME: do not allow for 1.4.x
	# This doesn't appear to be needed now that we're allowing 1.4 (jh)
  # AC_MSG_CHECKING([for libpng version < 1.4 (not supported at the moment!)])
  # AX_COMPARE_VERSION([$ax_check_lib_version], [ge], [1.4], [ax_check_lib="bad"], [ax_check_lib="ok"])
  # AC_MSG_RESULT([$ax_check_lib])
  ###
  if test x"$ax_check_lib" != x"ok"; then
    AC_MSG_ERROR([cannot find a suitable PNG library])
  else
    pov_libpng=ok
  fi
fi

# FIXME: what about cygwin?
# libjpeg; required library for distributable builds
AC_MSG_CHECKING([whether to use the JPEG library])
if test x"$pov_no_distrib" = x"yes" && test x"$with_libjpeg" = x"no"; then
  AC_MSG_RESULT([no (NON DISTRIBUTABLE BUILD)])
  AC_MSG_WARN([all program features using the JPEG library are disabled])
  AC_DEFINE([LIBJPEG_MISSING], [], [Don't use libjpeg.])
else
  if test x"$with_libjpeg" = x"no"; then
    AC_MSG_ERROR([disabling support for JPEG requires NON_REDISTRIBUTABLE_BUILD=yes])
  fi
  AC_MSG_RESULT([yes])
  AX_CHECK_LIBJPEG([$required_libjpeg_version], [$with_libjpeg])
  if test x"$ax_check_libjpeg" != x"ok"; then
    AC_MSG_ERROR([cannot find a suitable JPEG library])
  else
    pov_libjpeg=ok
  fi
fi

# libtiff; required library for distributable builds
AC_MSG_CHECKING([whether to use the TIFF library])
if test x"$pov_no_distrib" = x"yes" && test x"$with_libtiff" = x"no"; then
  AC_MSG_RESULT([no (NON DISTRIBUTABLE BUILD)])
  AC_MSG_WARN([all program features using the TIFF library are disabled])
  AC_DEFINE([LIBTIFF_MISSING], [], [Don't use libtiff.])
else
  if test x"$with_libtiff" = x"no"; then
    AC_MSG_ERROR([disabling support for TIFF requires NON_REDISTRIBUTABLE_BUILD=yes])
  fi
  AC_MSG_RESULT([yes])
  AX_CHECK_LIBTIFF([$required_libtiff_version], [$with_libtiff])
  if test x"$ax_check_libtiff" != x"ok"; then
    AC_MSG_ERROR([cannot find a suitable TIFF library])
  else
    pov_libtiff=ok
  fi
fi

# OpenEXR; optional library
AC_MSG_CHECKING([whether to use the OpenEXR library])
if test x"$with_openexr" = x"no"; then
  AC_MSG_RESULT([no])
else
  AC_MSG_RESULT([yes])
  AX_CHECK_OPENEXR([$required_openexr_version])
  if test x"$ax_check_openexr" = x"ok"; then
    pov_libopenexr=ok
  fi
fi
if test x"$pov_libopenexr" != x"ok"; then
  AC_MSG_WARN([all program features using the OpenEXR library are disabled])
  AC_DEFINE([OPENEXR_MISSING], [], [Don't use OpenEXR.])
fi

# Unix-specific display libraries.
if test x"$with_cygwin_dll" = x"no"; then

  # disable preview displays under Cygwin/MinGW
  AC_MSG_NOTICE([X Window display will be disabled])
  AC_MSG_NOTICE([SVGAlib display will be disabled])
  AC_DEFINE([X_DISPLAY_MISSING], [], [Don't use the X Window System.])
  pov_xwin_msg="disabled"

else 

  # libX11
  # first add default paths to help finding X11
  test x"$x_includes"  = x"NONE" && x_includes="/usr/include"
  test x"$x_libraries" = x"NONE" && x_libraries="/usr/lib"

  pov_xwin_msg="disabled"
  AC_PATH_XTRA
  if test x"$have_x" = x"yes"; then
    AC_CHECK_HEADER([X11/Xlib.h],
      [
        AC_CHECK_LIB([X11], [XFlush],
          [
            # X_CFLAGS should actually be X_CPPFLAGS since it only adds -Idir
            CPPFLAGS="$CPPFLAGS $X_CFLAGS"  # append
            LDFLAGS="$LDFLAGS $X_LIBS"      # append
            LIBS="$X_PRE_LIBS -lX11 $X_EXTRA_LIBS $LIBS"  # $PTHREAD_LIBS is already in $LIBS
            pov_xwin_msg="enabled"
          ],
          [
            AC_MSG_NOTICE([X Window display will be disabled])
            AC_DEFINE([X_DISPLAY_MISSING],[1])
          ],
          [$X_PRE_LIBS $X_EXTRA_LIBS $PTHREAD_LIBS]
        )
      ]
    )
  else
    AC_MSG_NOTICE([X Window display will be disabled])
  fi

  # color icon (libXpm) and X Window cursor
  if test x"$pov_xwin_msg" = x"enabled"; then
    AC_CHECK_HEADER([X11/xpm.h], [AC_CHECK_LIB([Xpm], [XpmCreatePixmapFromData])])
    AC_MSG_CHECKING([whether to enable the watch cursor])
    if test x"$enable_watch_cursor" = x"yes"; then
      AC_MSG_RESULT([yes])
      AC_CHECK_HEADER([X11/cursorfont.h],
        [
          AC_CHECK_FUNC([XCreateFontCursor],
            [AC_DEFINE([USE_CURSOR], [], [Use a watch cursor while rendering.])],
            [AC_MSG_NOTICE([watch cursor will be disabled])]
          )
        ]
      )
    else
      AC_MSG_RESULT([no])
    fi
  fi

#  AM_CONDITIONAL([BUILD_AVX2], [test x$enable_avx2 = xyes])
#  if test x"$enable_avx2" = x"yes"; then
#    AC_MSG_RESULT([yes])
#    AC_DEFINE([ENABLE_AVX2], [],[Enable AVX2 features and compiler optimization])
#
#    CXXFLAGS="$CXXFLAGS -mavx2 -mfma"  # append
#  else
#    AC_MSG_RESULT([no])
#  fi

  ### Note from Warp <warp@tag.povray.org> :
  # Using "-static" for static linking in Solaris causes a linkage error.
  # For some reason the X libraries in Solaris are not designed for static
  # linking. [...] So at least for Solaris the '-static' flag should
  # definitely be turned off by default.
  if test x"$ax_cv_prog_ld_static" != x"" && test x"$ac_cv_lib_X11_XFlush" = x"yes"; then
    case "$build" in
      sparc-*)
        AC_MSG_WARN([Using $ax_cv_prog_ld_static might cause a linkage error])
      ;;
    esac
  fi

  # libSDL
  if test x"$pov_xwin_msg" = x"enabled"; then
    AX_CHECK_LIBSDL([$required_libsdl_version])
    if test x"$ax_check_libsdl" = x"ok"; then
      pov_libsdl=ok
      pov_xwin_msg="$pov_xwin_msg (using SDL)"
    else
      AC_MSG_WARN([the preview display is disabled])
      pov_xwin_msg="disabled"  # FIXME
    fi
  fi

fi  # with_cygwin_dll


###############################################################################

AC_MSG_RESULT([
Language constructs and functions
---------------------------------])

# Compiling on the mingw32 platform (e.g. using MSYS) also requires
# the -D__CYGWIN flag for tiff_pov.cpp
case "$build" in
  *mingw32*)  CPPFLAGS="$CPPFLAGS -D__CYGWIN";;
esac


# Language constructs.

AC_HEADER_TIME
AC_CHECK_HEADERS([time.h limits.h sys/resource.h sys/time.h sys/wait.h unistd.h])

AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_STRUCT_TM

AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long int])	# check for 64 bit support
AC_CHECK_SIZEOF([size_t])	# check for 64 bit support
AC_CHECK_SIZEOF([float])


# Checks for functions.

# the test for memcmp does not work well with C++
AC_LANG_PUSH([C])
AC_FUNC_MEMCMP
AC_LANG_POP([C])

#AC_FUNC_STRFTIME  # FIXME
#AC_FUNC_VPRINTF   # FIXME

# getcwd or its equivalent with getenv <unistd.h>
AC_LANG_PUSH([C])
AC_CHECK_FUNCS([getcwd],
  [],
  [
    AC_MSG_CHECKING([for working getenv("PWD")])
    AC_RUN_IFELSE(
      [
        AC_LANG_SOURCE(
[#include <stdlib.h>
int main(void) { return (getenv("PWD") == NULL); }]
        )
      ],
      [AC_MSG_RESULT([yes])],
      [
        AC_MSG_RESULT([no])
        AC_MSG_NOTICE([I/O restriction code will be disabled])
        enable_io_restrictions="no"
      ],
      [
        AC_MSG_RESULT([cross-compiling])
        AC_MSG_NOTICE([I/O restriction code will be disabled])
        enable_io_restrictions="no"
      ]
    )
  ]
)
AC_LANG_POP([C])

# readlink <unistd.h>
if test x"$enable_io_restrictions" != x"no"; then
  AC_CHECK_FUNCS([readlink],
    [],
    [
      AC_MSG_NOTICE([I/O restriction code will be disabled])
      enable_io_restrictions="no"
    ]
  )
fi

# nanosleep <time.h>
AC_CHECK_FUNCS([nanosleep])

# usleep and related <unistd.h>
AC_CHECK_FUNCS([usleep])
AC_CHECK_TYPES([useconds_t])

# clock_gettime and related <time.h>
AC_CHECK_FUNCS([clock_gettime])
AC_CHECK_DECLS([CLOCK_MONOTONIC, CLOCK_REALTIME, CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID],
  [], [], [[#include <time.h>]]
)
AC_CHECK_TYPES([clockid_t], [], [], [[#include <time.h>]])

# getrusage and related <sys/resource.h>
AC_CHECK_FUNCS([getrusage])
AC_CHECK_DECLS([RUSAGE_SELF, RUSAGE_THREAD, RUSAGE_LWP],
  [], [], [[#include <sys/resource.h>]]
)

# gettimeofday <sys/time.h>
AC_CHECK_FUNCS([gettimeofday])

# (Not checking for `asinh` and friends anymore, as they are mandatory as of C++11. [CLi])

# support for non-IEEE compilers
# TODO FIXME - We should actually check for existence of `int32_t` rather than size of `int` now.
if test x"$C99_COMPATIBLE_RADIOSITY" != x"" || test x"$ac_cv_sizeof_int" != x"4" || test x"$ac_cv_sizeof_float" != x"4"; then
  # (Not checking for `copysign` anymore as we're no longer using it. [CLi])
  if test x"$C99_COMPATIBLE_RADIOSITY" = x""; then  # auto-detection
    # Simply choose `frexpf`/`ldexpf` variant; C++11 requires their existence,
    # and they provide maximum portability at good performance.
    AC_DEFINE([C99_COMPATIBLE_RADIOSITY], [6], [See source/core/configcore.h for details.])
  else  # check value of C99_COMPATIBLE_RADIOSITY
    # (Not checking function presence anymore, as they are all mandatory as of C++11. [CLi])
    case "$C99_COMPATIBLE_RADIOSITY" in
      1|2|3|4|5|6|7|8|9) ;;
      *) AC_MSG_ERROR([Unsupported C99_COMPATIBLE_RADIOSITY value]) ;;
    esac
    AC_DEFINE_UNQUOTED([C99_COMPATIBLE_RADIOSITY],
      [$C99_COMPATIBLE_RADIOSITY],
      [See source/core/configcore.h for details.]
    )
  fi
fi


###############################################################################

AC_MSG_RESULT([
Compiling
---------])

# Start with no flags at all, so back them up first.
pov_save_cxxflags="$CXXFLAGS"
CXXFLAGS=


# Use pipes for communication between compilation stages.
AC_MSG_CHECKING([whether to enable pipes for communications])
if test x"$enable_pipe" = x"no"; then
  AC_MSG_RESULT([no])
else
  AC_MSG_RESULT([yes])
  AX_TEST_COMPILER_FLAGS([-pipe])
fi


# Language.

# don't use -ansi since it can make compilation fail (e.g. on cygwin).
#AX_TEST_COMPILER_FLAGS([-ansi], [], [AX_TEST_COMPILER_FLAGS([-Aa])])

# -w would prevent all warning messages, even if -W... is given later on.
# we don't want to clutter the compiling with lots of warnings.
#AX_TEST_COMPILER_FLAGS([-W])
#AX_TEST_COMPILER_FLAGS([-Wall], [], [AX_TEST_COMPILER_FLAGS([-fullwarn])])
AX_TEST_COMPILER_FLAGS([-Wno-multichar])
AX_TEST_COMPILER_FLAGS([-Wno-write-strings])
if test x"$pov_compiler_vendor" = x"gnu"; then
  AX_TEST_COMPILER_FLAGS([-fno-enforce-eh-specs])
  AX_TEST_COMPILER_FLAGS([-Wno-non-template-friend])
fi
AX_TEST_COMPILER_FLAGS([-Wsuggest-override])


# I/O restrictions.
AC_MSG_CHECKING([whether to enable I/O restrictions])
if test x"$enable_io_restrictions" = x"no"; then
  AC_MSG_RESULT([no])
  AC_DEFINE([IO_RESTRICTIONS_DISABLED], [1], [I/O restrictions.])
else
  AC_MSG_RESULT([yes])
  AC_DEFINE([IO_RESTRICTIONS_DISABLED], [0], [I/O restrictions.])
fi


# Compile with debugging code.

AC_MSG_CHECKING([whether to enable debugging])

if test x"$enable_debug" = x"yes"; then
  AC_MSG_RESULT([yes])
  AX_TEST_COMPILER_FLAGS([-g], [], [AC_MSG_WARN([Cannot set debugging mode])])
  AC_DEFINE([UNIX_DEBUG], [], [Unix-specific debug messages.])
else
  AC_MSG_RESULT([no])
fi


# Compile for profiling.

AC_MSG_CHECKING([whether to enable profiling])

if test x"$enable_profile" = x"yes"; then
  AC_MSG_RESULT([yes])
  if test x"$ax_cv_prog_cxx_flags_g" = x""; then
    AX_TEST_COMPILER_FLAGS([-g], [], [AC_MSG_WARN([Cannot set debugging mode])])
  fi
  AX_TEST_COMPILER_FLAGS([-pg], [], [AC_MSG_WARN([Cannot set profiling mode])])
else
  AC_MSG_RESULT([no])
fi


# Strip object files.

AC_MSG_CHECKING([whether to enable stripping])

if test x"$enable_strip" = x"no"; then
  AC_MSG_RESULT([no])
else
  if test x"$enable_debug" = x"yes" || test x"$enable_profile" = x"yes"; then
    AC_MSG_RESULT([no, debugging/profiling mode is on])
  else
    AC_MSG_RESULT([yes])
    AX_TEST_COMPILER_FLAGS([-s], [], [AC_MSG_WARN([Cannot set stripping mode])])
  fi
fi


# Compile with optimizations.

AC_MSG_CHECKING([whether to enable optimizations])

if test x"$enable_optimiz" != x"no"; then

  AC_MSG_RESULT([yes])

  # don't try -fast (makes ICC fail when linking due to implied -ipo flag).
  AX_TEST_COMPILER_FLAGS([-O3], [], [AX_TEST_COMPILER_FLAGS([-O2], [], [AX_TEST_COMPILER_FLAGS([-O])])])

  # compiler-specific flags.
  case "$pov_compiler_vendor" in
    gnu)
      if test x"$ax_cv_prog_cxx_flags_O" = x"yes"; then  # -O was set
        for pov_flags in "-fexpensive-optimizations" "-finline-functions" "-foptimize-sibling-calls"
        do
          AX_TEST_COMPILER_FLAGS([$pov_flags])
        done
      elif test x"$ax_cv_prog_cxx_flags_O2" = x"yes"; then  # -O2 was set
        AX_TEST_COMPILER_FLAGS([-finline-functions])
      fi
      AX_TEST_COMPILER_FLAGS([-ffast-math])
      ;;

    intel)
      AX_TEST_COMPILER_FLAGS([-ip])
      ;;
  esac

  # Arch-specific tuning, alphabetically ordered for conveniency.
  # Perform checks only when not cross-compiling.
  AC_MSG_CHECKING([whether to enable architecture-specific optimizations])
  if test x"$cross_compiling" != x"no"; then
    AC_MSG_RESULT([no, cross-compiling])
    pov_built_for="$pov_built_for (cross-compiled)"
  elif test x"$enable_optimiz_arch" != x"no"; then
    AC_MSG_RESULT([yes])

    case "$build" in

      alphaev*6*)
        ### Suggested by Christopher Endsley <endsley@yokuts.csustan.edu>
        for pov_flags in "-mcpu=ev56" "-mno-soft-float"
        do
          AX_TEST_COMPILER_FLAGS([$pov_flags])
        done
        case "$CXXFLAGS" in
          *-mcpu=ev56*)  pov_built_for="$pov_built_for | DEC Alpha EV5 with BWX extensions" ;;
        esac
        ;;

      i?86-* | k?-* | *86*64*)
        case "$build" in
          i?86-* | k?-*)  AX_TEST_COMPILER_FLAGS([-malign-double]);;
        esac
        AX_TEST_COMPILER_FLAGS([-xHost],            # ICC >= 11.0
          [pov_built_for="$pov_built_for (using -xHost)"],
          [AX_TEST_COMPILER_FLAGS([-march=native],  # GCC >= 4.2
            [pov_built_for="$pov_built_for (using -march=native)"],
            [AC_MSG_CHECKING([processor type])
            AX_X86_ARCH
            AC_MSG_RESULT([$ax_x86_cpumodel_nofreq])
            AC_MSG_CHECKING([whether SSE instruction sets are supported])
            test x"$ax_x86_cpuflags_SSE_list" = x"" && ax_x86_cpuflags_SSE_list=no
            AC_MSG_RESULT([$ax_x86_cpuflags_SSE_list])
            if test x"$ax_x86_arch" != x"unknown"; then
              if test x"$pov_compiler_vendor" = x"intel"; then
                case "$ax_x86_arch" in
                  core2)
                    AX_TEST_COMPILER_FLAGS([-xT]) ;;
                  prescott|pentium4)  # Prescott, Northwood, Willamette
                    AX_TEST_COMPILER_FLAGS([-xP], [], [AX_TEST_COMPILER_FLAGS([-xN], [], [AX_TEST_COMPILER_FLAGS([-xW])])]) ;;
                  #pentium-m)  # Banias; option deprecated in ICC >= 10.0
                  #  AX_TEST_COMPILER_FLAGS([-xB]) ;;
                  pentium3)
                    AX_TEST_COMPILER_FLAGS([-xK]) ;;
                  pentium2)
                    AX_TEST_COMPILER_FLAGS([-march=pentiumii]) ;;
                esac
              else
                AX_TEST_COMPILER_FLAGS([-march=$ax_x86_arch],
                  [],
                  [
                    if test x"$ax_x86_arch_fallback" != x""; then
                      AX_TEST_COMPILER_FLAGS([-march=$ax_x86_arch_fallback], [ax_x86_arch=$ax_x86_arch_fallback], [ax_x86_arch=])
                    fi
                  ]
                )
                if test x"$pov_compiler_vendor" = x"gnu"; then
                  for sse_flag in $ax_x86_cpuflags_SSE_list
                  do
                    sse_flag=`echo $sse_flag | tr A-Z a-z`
                    AX_TEST_COMPILER_FLAGS([-m$sse_flag])
                  done
                  case "$ax_x86_cpuflags_SSE_list" in
                    *SSE2*|*SSE3*|*SSSE3*|*SSE4*)  AX_TEST_COMPILER_FLAGS([-mfpmath=sse]);;  # -march or -msse required
                  esac
                fi
              fi
            else
              ax_x86_arch=
            fi
            if test x"$ax_x86_arch" != x""; then
              pov_built_for="$pov_built_for | $ax_x86_cpumodel_nofreq"
            fi
          ])])
        ;;

      *irix*)
        AX_TEST_COMPILER_FLAGS([-mips4],
          [pov_built_for="$pov_built_for | MIPS 4"],
          [AX_TEST_COMPILER_FLAGS([-mips2], [pov_built_for="$pov_built_for | MIPS 2"])]
        )
        ;;

      powerpc*)
        ### Suggested by Thorsten Froehlich <thorsten@trf.de> :
        # Note that the different parameters for mcpu and mtune are intentional:
        # The PPC 7450 processor has a much longer pipeline and thus scheduling
        # for that pipeline always yields good results even on processors with
        # a shorter pipeline.  On the other hand, most PowerPC processors only
        # support the instructions also found in the PPC 750 processor (aka G3).
        # That is why it is selected in the mcpu parameter.  Using 7450 (aka G4+)
        # in mcpu would make the code incompatible with anything prior to the
        # PPC 7400 (aka G4).  To be precise, the vector instructions are new
        # in the four digit model number PowerPC processors (aka G4), but gcc
        # cannot (as of February 2003) use them automatically anyway! [trf]
        #
        # 2007-01-24 after gentoo patch for powerpc64 (http://bugs.gentoo.org/show_bug.cgi?id=118156)
        if "$build" != powerpc64* ; then
          for pov_flags in "-mpowerpc" "-mcpu=750 -mtune=7450"
          do
            AX_TEST_COMPILER_FLAGS([$pov_flags])
          done
          case "$CXXFLAGS" in
            *-mcpu=750*)  pov_built_for="$pov_built_for | PowerPC G3/G4" ;;
            *)            pov_built_for="$pov_built_for | PowerPC" ;;
          esac
        else
          pov_built_for="$pov_built_for | PowerPC G5"
        fi
        for pov_flags in "-mmultiple" "-mstring" "-mfused-madd"
        do
          AX_TEST_COMPILER_FLAGS([$pov_flags])
        done
        ;;

      sparc-*)
        # Commented out for now.
        #AX_TEST_COMPILER_FLAGS([-march=ultrasparc])
        #AX_TEST_COMPILER_FLAGS([-mvis])
        ;;

    esac

    AC_MSG_CHECKING([which architecture to optimize for])
    AC_MSG_RESULT([$pov_built_for])
  else
    AC_MSG_RESULT([no])  # no arch-specific optimizations
  fi

else
  AC_MSG_RESULT([no])  # no optimizations
fi

AC_DEFINE_UNQUOTED([BUILT_FOR], ["$pov_built_for"], [Host platform.])

case "$pov_built_for" in
  i?86-* | x86_64-*)
    pov_cpu='x86'
    AC_DEFINE([BUILD_X86], [], [Build for x86 or x86-64 architecture])
    AX_CHECK_COMPILE_FLAG([-mavx],  [pov_avx='-mavx'],   [pov_avx=''])
    AX_CHECK_COMPILE_FLAG([-mavx2], [pov_avx2='-mavx2'], [pov_avx2=''])
    AX_CHECK_COMPILE_FLAG([-mfma],  [pov_fma3='-mfma'],  [pov_fma3=''])
    AX_CHECK_COMPILE_FLAG([-mfma4], [pov_fma4='-mfma4'], [pov_fma4=''])
    ;;
  *)
    pov_cpu=''
    ;;
esac
AM_CONDITIONAL([BUILD_x86], [test x"$pov_cpu" = x"x86"])
AM_CONDITIONAL([BUILD_x86avx], [test x"$pov_avx" != x""])
AM_CONDITIONAL([BUILD_x86avxfma4], [test x"$pov_avx" != x"" -a x"$pov_fma4" != x"" ])
AM_CONDITIONAL([BUILD_x86avx2fma3], [test x"$pov_avx2" != x"" -a x"$pov_fma3" != x"" ])


# Add flags specified at the command line.
CXXFLAGS=`echo $CXXFLAGS $pov_save_cxxflags`
AC_DEFINE_UNQUOTED([CXXFLAGS], ["$CXXFLAGS"], [Compiler flags.])


###############################################################################

# NOTE: It is highly important that this block of tests is run AFTER compiler optimization settings have been chosen.

AC_MSG_RESULT([
Floating Point Features
-----------------------])

AC_CHECK_HEADERS([limits])

AC_MSG_CHECKING([whether NaNs are supported])
AC_RUN_IFELSE([
  AC_LANG_PROGRAM([[
    #ifdef HAVE_LIMITS
    #include <limits>
    #endif
  ]],[[
    if (!std::numeric_limits<double>::has_quiet_NaN) return 1;
    if (!std::numeric_limits<float>::has_quiet_NaN)  return 2;
  ]])
],[
  AC_MSG_RESULT([yes])

  AC_CHECK_HEADERS([cmath])

  AC_MSG_CHECKING([whether NaNs can be identified using std::isnan()])
  AC_RUN_IFELSE([
    AC_LANG_PROGRAM([[
      #ifdef HAVE_CMATH
      #include <cmath>
      #endif
      #ifdef HAVE_LIMITS
      #include <limits>
      #endif
    ]],[[
      volatile double d;
      volatile float f;
      d = std::numeric_limits<double>::quiet_NaN(); if (!std::isnan(d)) return 1;
      f = d;                                        if (!std::isnan(f)) return 2;
      f = std::numeric_limits<float>::quiet_NaN();  if (!std::isnan(f)) return 3;
      d = f;                                        if (!std::isnan(d)) return 4;
      d = 0.0; if (std::isnan(d)) return 5;
      f = 0.0; if (std::isnan(f)) return 6;
    ]])
  ],[
    AC_MSG_RESULT([yes])
    AC_DEFINE([HAVE_NAN],      [],[NaN floating point values are supported])
    AC_DEFINE([HAVE_STD_ISNAN],[],[NaNs can be identified using std::isnan()])
  ],[
    AC_MSG_RESULT([no])

    AC_MSG_CHECKING([whether NaNs can be identified using global isnan()])
    AC_RUN_IFELSE([
      AC_LANG_PROGRAM([[
        #ifdef HAVE_CMATH
        #include <cmath>
        #endif
        #ifdef HAVE_LIMITS
        #include <limits>
        #endif
      ]],[[
        volatile double d;
        volatile float f;
        d = std::numeric_limits<double>::quiet_NaN(); if (!isnan(d)) return 1;
        f = d;                                        if (!isnan(f)) return 2;
        f = std::numeric_limits<float>::quiet_NaN();  if (!isnan(f)) return 3;
        d = f;                                        if (!isnan(d)) return 4;
        d = 0.0; if (isnan(d)) return 5;
        f = 0.0; if (isnan(f)) return 6;
      ]])
    ],[
      AC_MSG_RESULT([yes])
      AC_DEFINE([HAVE_NAN],  [],[NaN floating point values are supported])
      AC_DEFINE([HAVE_ISNAN],[],[NaNs can be identified using global isnan()])
    ],[
      AC_MSG_RESULT([no])

      AC_MSG_CHECKING([whether NaNs can be identified by comparison to themselves])
      AC_RUN_IFELSE([
        AC_LANG_PROGRAM([[
          #ifdef HAVE_LIMITS
          #include <limits>
          #endif
          template<typename T> inline bool emulated_isnan(T x) { volatile T v = x; return (v != x); }
        ]],[[
          volatile double d;
          volatile float f;
          d = std::numeric_limits<double>::quiet_NaN(); if (!emulated_isnan(d)) return 1;
          f = d;                                        if (!emulated_isnan(f)) return 2;
          f = std::numeric_limits<float>::quiet_NaN();  if (!emulated_isnan(f)) return 3;
          d = f;                                        if (!emulated_isnan(d)) return 4;
          d = 0.0; if (emulated_isnan(d)) return 5;
          f = 0.0; if (emulated_isnan(f)) return 6;
        ]])
      ],[
        AC_MSG_RESULT([yes])
        AC_DEFINE([HAVE_NAN],           [],[NaN floating point values are supported])
        AC_DEFINE([HAVE_EMULATED_ISNAN],[],[NaNs can be identified by comparison to themselves])
      ],[
        # although double and float support NaN values, they are no use to us as we can't properly test for them,
        # so we're not setting HAVE_NAN.
        AC_MSG_RESULT([no])
      ],[
        AC_MSG_RESULT([cross-compiling])  # FIXME
      ])
    ],[
      AC_MSG_RESULT([cross-compiling])  # FIXME
    ])
  ],[
    AC_MSG_RESULT([cross-compiling])  # FIXME
  ])
],[
  AC_MSG_RESULT([no])
],[
  AC_MSG_RESULT([cross-compiling])  # FIXME
])

AC_MSG_CHECKING([whether infinite values are supported])
AC_RUN_IFELSE([
  AC_LANG_PROGRAM([[
    #ifdef HAVE_LIMITS
    #include <limits>
    #endif
  ]],[[
    if (!std::numeric_limits<double>::has_infinity) return 1;
    if (!std::numeric_limits<float>::has_infinity)  return 2;
  ]])
],[
  AC_MSG_RESULT([yes])

  AC_CHECK_HEADERS([cmath])

  AC_MSG_CHECKING([whether infinities can be identified using std::isinf()])
  AC_RUN_IFELSE([
    AC_LANG_PROGRAM([[
      #ifdef HAVE_CMATH
      #include <cmath>
      #endif
      #ifdef HAVE_LIMITS
      #include <limits>
      #endif
    ]],[[
      volatile double d;
      volatile float f;
      d = std::numeric_limits<double>::infinity();  if (!std::isinf(d)) return 1;
      f = d;                                        if (!std::isinf(f)) return 2;
      f = std::numeric_limits<float>::infinity();   if (!std::isinf(f)) return 3;
      d = f;                                        if (!std::isinf(d)) return 4;
      d = -std::numeric_limits<double>::infinity(); if (!std::isinf(d)) return 5;
      f = d;                                        if (!std::isinf(f)) return 6;
      f = -std::numeric_limits<float>::infinity();  if (!std::isinf(f)) return 7;
      d = f;                                        if (!std::isinf(d)) return 8;
      d = 0.0; if (std::isinf(d)) return 9;
      f = 0.0; if (std::isinf(f)) return 10;
    ]])
  ],[
    AC_MSG_RESULT([yes])
    AC_DEFINE([HAVE_INF],      [],[infinite floating point values are supported])
    AC_DEFINE([HAVE_STD_ISINF],[],[intinities can be identified using std::isinf()])
  ],[
    AC_MSG_RESULT([no])

    AC_MSG_CHECKING([whether infinities can be identified using global isinf()])
    AC_RUN_IFELSE([
      AC_LANG_PROGRAM([[
        #ifdef HAVE_CMATH
        #include <cmath>
        #endif
        #ifdef HAVE_LIMITS
        #include <limits>
        #endif
      ]],[[
        volatile double d;
        volatile float f;
        d = std::numeric_limits<double>::infinity();  if (!isinf(d)) return 1;
        f = d;                                        if (!isinf(f)) return 2;
        f = std::numeric_limits<float>::infinity();   if (!isinf(f)) return 3;
        d = f;                                        if (!isinf(d)) return 4;
        d = -std::numeric_limits<double>::infinity(); if (!isinf(d)) return 5;
        f = d;                                        if (!isinf(f)) return 6;
        f = -std::numeric_limits<float>::infinity();  if (!isinf(f)) return 7;
        d = f;                                        if (!isinf(d)) return 8;
        d = 0.0; if (isinf(d)) return 9;
        f = 0.0; if (isinf(f)) return 10;
      ]])
    ],[
      AC_MSG_RESULT([yes])
      AC_DEFINE([HAVE_INF],  [],[infinite floating point values are supported])
      AC_DEFINE([HAVE_ISINF],[],[intinities can be identified using global isinf()])
    ],[
      AC_MSG_RESULT([no])

      AC_MSG_CHECKING([whether infinities can be identified by comparison to the maximum value])
      AC_RUN_IFELSE([
        AC_LANG_PROGRAM([[
          #ifdef HAVE_CMATH
          #include <cmath>
          #endif
          #ifdef HAVE_LIMITS
          #include <limits>
          #endif
          template<typename T> inline bool emulated_isinf(T x) { volatile T v = std::numeric_limits<T>::max(); return std::fabs(x) > v; }
        ]],[[
          volatile double d;
          volatile float f;
          d = std::numeric_limits<double>::infinity();  if (!emulated_isinf(d)) return 1;
          f = d;                                        if (!emulated_isinf(f)) return 2;
          f = std::numeric_limits<float>::infinity();   if (!emulated_isinf(f)) return 3;
          d = f;                                        if (!emulated_isinf(d)) return 4;
          d = -std::numeric_limits<double>::infinity(); if (!emulated_isinf(d)) return 5;
          f = d;                                        if (!emulated_isinf(f)) return 6;
          f = -std::numeric_limits<float>::infinity();  if (!emulated_isinf(f)) return 7;
          d = f;                                        if (!emulated_isinf(d)) return 8;
          d = 0.0; if (emulated_isinf(d)) return 9;
          f = 0.0; if (emulated_isinf(f)) return 10;
        ]])
      ],[
        AC_MSG_RESULT([yes])
        AC_DEFINE([HAVE_INF],           [],[infinite floating point values are supported])
        AC_DEFINE([HAVE_EMULATED_ISINF],[],[infinities can be identified by comparison to the maximum value])
      ],[
        # although double and float support infinite values, they are no use to us as we can't properly test for them,
        # so we're not setting HAVE_INF.
        AC_MSG_RESULT([no])
      ],[
        AC_MSG_RESULT([cross-compiling])  # FIXME
      ])
    ],[
      AC_MSG_RESULT([cross-compiling])  # FIXME
    ])
  ],[
    AC_MSG_RESULT([cross-compiling])  # FIXME
  ])
],[
  AC_MSG_RESULT([no])
],[
  AC_MSG_RESULT([cross-compiling])  # FIXME
])


###############################################################################

AC_MSG_RESULT([
Makefiles
---------])

AC_CONFIG_FILES([\
  Makefile \
  source/Makefile \
  vfe/Makefile \
  platform/Makefile \
  unix/Makefile \
])


###############################################################################

if test x"$enable_io_restrictions" = x"no"; then
  pov_io_msg="disabled"
else
  pov_io_msg="enabled"
fi

# image format support
pov_img_fmt_builtin="gif tga iff ppm pgm hdr"

for format in png jpeg tiff openexr
do
  eval fmt=\$pov_lib$format
  if test x"$fmt" != x""; then
    pov_img_fmt_enabled="$pov_img_fmt_enabled $format"
  else
    pov_img_fmt_disabled="$pov_img_fmt_disabled $format"
  fi
done
pov_img_fmt_enabled=`echo $pov_img_fmt_enabled`
pov_img_fmt_disabled=`echo $pov_img_fmt_disabled`
test x"$pov_img_fmt_disabled" = x"" && pov_img_fmt_disabled="-"


AC_DEFINE_UNQUOTED([BUILTIN_IO_RESTRICTIONS], ["$pov_io_msg"], [I/O restrictions.])
AC_DEFINE_UNQUOTED([BUILTIN_XWIN_DISPLAY], ["$pov_xwin_msg"], [X Window display.])
AC_DEFINE_UNQUOTED([BUILTIN_IMG_FORMATS], ["$pov_img_fmt_builtin $pov_img_fmt_enabled"], [Supported image formats.])
AC_DEFINE_UNQUOTED([MISSING_IMG_FORMATS], ["$pov_img_fmt_disabled"], [Unsupported image formats.])

povowner=`stat ${stat_format}%u ${HOME}`
povgroup=`stat ${stat_format}%g ${HOME}`
AC_SUBST([povowner])
AC_SUBST([povgroup])

AC_OUTPUT


###############################################################################

# recursively expand the following variables for displaying
for i in bindir sysconfdir datadir mandir
do
  eval povtemp=\$$i
  while test x"$povtemp" != x"$oldpovtemp"
  do
    oldpovtemp=$povtemp
    eval povtemp=$povtemp
  done
  eval pov$i=$povtemp
done

AC_MSG_RESULT([
===============================================================================
$PACKAGE_NAME $PACKAGE_VERSION has been configured.
])

if test x"$pov_no_distrib" = x"yes"; then
  AC_MSG_WARN([the resulting binary is NOT REDISTRIBUTABLE.
])
fi

if test x"$pov_warn_path" != x""; then
  AC_MSG_WARN(
[configure has detected that the list of search paths
specified by the following environment variable(s)
 $pov_warn_path
erroneously contain the "." directory, which may cause a build failure.
In order to build POV-Ray successfully, the package has been configured
with all "." directory entries removed from the variables above.  You're
strongly advised to correct your configuration accordingly.
])
fi

AC_MSG_RESULT([Built-in features:
  I/O restrictions:          ${pov_io_msg}
  X Window display:          ${pov_xwin_msg}
  Supported image formats:   $pov_img_fmt_builtin $pov_img_fmt_enabled
  Unsupported image formats: $pov_img_fmt_disabled

Compilation settings:
  Build architecture:  $build 
  Built/Optimized for: $pov_built_for
  Compiler vendor:     $pov_compiler_vendor
  Compiler version:    $pov_compiler_version
  Compiler flags:      $CXXFLAGS
  Libraries:           $LIBS

Type 'make check' to build the program and run a test render.
Type 'make install' to install $PACKAGE_NAME on your system.

The $PACKAGE_NAME components will be installed in the following directories:
  Program (executable):       ${povbindir}
  System configuration files: ${povsysconfdir}/$PACKAGE/$VERSION_BASE
  User configuration files:   ${HOME}/.$PACKAGE/$VERSION_BASE
  Standard include files:     ${povdatadir}/$PACKAGE-$VERSION_BASE/include
  Standard INI files:         ${povdatadir}/$PACKAGE-$VERSION_BASE/ini
  Standard demo scene files:  ${povdatadir}/$PACKAGE-$VERSION_BASE/scenes
  Documentation (text, HTML): ${povdatadir}/doc/$PACKAGE-$VERSION_BASE
  Unix man page:              ${povmandir}
===============================================================================
])
