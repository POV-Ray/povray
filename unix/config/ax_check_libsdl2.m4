# SYNOPSIS
#
#   AX_CHECK_LIBSDL(required_version)
#
# DESCRIPTION
#
#   Check whether the system has libSDL version required_version.
#
# LAST MODIFICATION
#
#   2007-11-08
#
# COPYLEFT
#
#   Copyright (c) 2007 Nicolas Calimet
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_CHECK_LIBSDL2],
[
  AC_CHECK_PROGS([SDL2CONFIG], [sdl2-config])

  AC_MSG_CHECKING([for libSDL2])
  ax_check_libsdl2=`sdl2-config 2>&1 | grep -v Usage`
  if test x"$ax_check_libsdl2" != x""; then
    ax_check_libsdl2="not found"
    AC_MSG_RESULT([$ax_check_libsdl2])
  else
    AC_MSG_RESULT([yes])

    AC_MSG_CHECKING([for libSDL2 version >= $1])
    ax_check_libsdl2_version=`sdl2-config --version 2> /dev/null`
    if test x"$ax_check_libsdl2_version" != x""; then
      AX_COMPARE_VERSION([$ax_check_libsdl2_version], [ge], [$1], [ax_check_libsdl2="ok"], [ax_check_libsdl2="bad"])
      AC_MSG_RESULT([$ax_check_libsdl2_version, $ax_check_libsdl2])
    else
      ax_check_libsdl2="unknown"
      AC_MSG_RESULT([$ax_check_libsdl2])
    fi

    if test x"$ax_check_libsdl2" = x"ok"; then
      ax_check_libsdl2_cflags=`sdl2-config --cflags 2> /dev/null`
      ax_check_libsdl2_libs=`sdl2-config --libs 2> /dev/null`
      ax_check_libsdl2_save_cppflags="$CPPFLAGS"
      ax_check_libsdl2_save_libs="$LIBS"
      CPPFLAGS="$ax_check_libsdl2_cflags $CPPFLAGS"
      LIBS="$ax_check_libsdl2_libs $LIBS"

      # check include file
      AC_CHECK_HEADER(
        [SDL2/SDL.h],
        [AC_CHECK_LIB([SDL2], [SDL_Quit], [], [ax_check_libsdl2="not found"])],
        [ax_check_libsdl2="no headers"]
      )
    fi

    if test x"$ax_check_libsdl2" != x"ok"; then
      CPPFLAGS="$ax_check_libsdl2_save_cppflags"
      LIBS="$ax_check_libsdl2_save_libs"
    fi
  fi
])
