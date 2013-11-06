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

AC_DEFUN([AX_CHECK_LIBSDL],
[
  AC_CHECK_PROGS([SDLCONFIG], [sdl-config])

  AC_MSG_CHECKING([for libSDL])
  ax_check_libsdl=`sdl-config 2>&1 | grep -v Usage`
  if test x"$ax_check_libsdl" != x""; then
    ax_check_libsdl="not found"
    AC_MSG_RESULT([$ax_check_libsdl])
  else
    AC_MSG_RESULT([yes])

    AC_MSG_CHECKING([for libSDL version >= $1])
    ax_check_libsdl_version=`sdl-config --version 2> /dev/null`
    if test x"$ax_check_libsdl_version" != x""; then
      AX_COMPARE_VERSION([$ax_check_libsdl_version], [ge], [$1], [ax_check_libsdl="ok"], [ax_check_libsdl="bad"])
      AC_MSG_RESULT([$ax_check_libsdl_version, $ax_check_libsdl])
    else
      ax_check_libsdl="unknown"
      AC_MSG_RESULT([$ax_check_libsdl])
    fi

    if test x"$ax_check_libsdl" = x"ok"; then
      ax_check_libsdl_cflags=`sdl-config --cflags 2> /dev/null`
      ax_check_libsdl_libs=`sdl-config --libs 2> /dev/null`
      ax_check_libsdl_save_cppflags="$CPPFLAGS"
      ax_check_libsdl_save_libs="$LIBS"
      CPPFLAGS="$ax_check_libsdl_cflags $CPPFLAGS"
      LIBS="$ax_check_libsdl_libs $LIBS"

      # check include file
      AC_CHECK_HEADER(
        [SDL/SDL.h],
        [AC_CHECK_LIB([SDL], [SDL_Quit], [], [ax_check_libsdl="not found"])],
        [ax_check_libsdl="no headers"]
      )
    fi

    if test x"$ax_check_libsdl" != x"ok"; then
      CPPFLAGS="$ax_check_libsdl_save_cppflags"
      LIBS="$ax_check_libsdl_save_libs"
    fi
  fi
])
