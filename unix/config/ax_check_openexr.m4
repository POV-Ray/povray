# SYNOPSIS
#
#   AX_CHECK_OPENEXR(required_version)
#
# DESCRIPTION
#
#   Check whether the system has OpenEXR version required_version.
#
# LAST MODIFICATION
#
#   2007-11-08
#
# COPYLEFT
#
#   Copyright (c) 2006 Nicolas Calimet
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_CHECK_OPENEXR],
[
  AC_CHECK_PROGS([PKGCONFIG], [pkg-config])

  AC_MSG_CHECKING([for OpenEXR's pkg-config])
  ax_check_openexr=`pkg-config --print-errors OpenEXR 2>&1`
  if test x"$ax_check_openexr" != x""; then
    ax_check_openexr="not found"
    AC_MSG_RESULT([$ax_check_openexr])
  else
    AC_MSG_RESULT([yes])

    AC_MSG_CHECKING([for OpenEXR version >= $1])
    ax_check_openexr_version=`pkg-config --modversion OpenEXR 2>&1 | tail -1 | grep -v "No package"`
    if test x"$ax_check_openexr_version" != x""; then
      AX_COMPARE_VERSION([$ax_check_openexr_version], [ge], [$1], [ax_check_openexr="ok"], [ax_check_openexr="bad"])
      AC_MSG_RESULT([$ax_check_openexr_version, $ax_check_openexr])
    else
      ax_check_openexr="unknown"
      AC_MSG_RESULT([$ax_check_openexr])
    fi

    if test x"$ax_check_openexr" = x"ok"; then
      ax_check_openexr_cflags=`pkg-config --cflags OpenEXR 2> /dev/null`
      ax_check_openexr_libs=`pkg-config --libs OpenEXR 2> /dev/null`
      ax_check_openexr_save_cppflags="$CPPFLAGS"
      ax_check_openexr_save_libs="$LIBS"
      CPPFLAGS="$ax_check_openexr_cflags $CPPFLAGS"

      # FIXME: workaround for versions >= 1.4.0
      AX_COMPARE_VERSION([$ax_check_openexr_version], [ge], [1.4],
        [LIBS="$ax_check_openexr_libs -lIlmThread $LIBS"],
	[LIBS="$ax_check_openexr_libs $LIBS"]
      )

      # check include file
      AC_CHECK_HEADER(
        [OpenEXR/ImfCRgbaFile.h],
        [AC_CHECK_LIB([IlmImf], [ImfInputReadPixels], [], [ax_check_openexr="not found"])],
        [ax_check_openexr="no headers"]
      )
    fi

    if test x"$ax_check_openexr" != x"ok"; then
      CPPFLAGS="$ax_check_openexr_save_cppflags"
      LIBS="$ax_check_openexr_save_libs"
    fi
  fi
])
