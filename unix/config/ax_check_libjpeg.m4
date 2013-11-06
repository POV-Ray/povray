# SYNOPSIS
#
#   AX_CHECK_LIBJPEG(required_version, lib_dir)
#
# DESCRIPTION
#
#   Check whether the system has libjpeg version required_version.
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

AC_DEFUN([AX_CHECK_LIBJPEG],
[
  ax_check_libjpeg_save_cppflags="$CPPFLAGS"
  ax_check_libjpeg_save_ldflags="$LDFLAGS"
  if test x"$2" != x""; then
    CPPFLAGS="-I$2/../include $CPPFLAGS"
    LDFLAGS="-L$2 $LDFLAGS"
  fi

  # required numeric version (e.g. 6b -> 62)
  ax_check_libjpeg_version_num=`echo $1 | tr [[a-i]] [[1-9]]`

  # check the library
  AC_SEARCH_LIBS(
    [jpeg_std_error],
    [jpeg],
    [
      # check include file
      AC_CHECK_HEADER(
        [jpeglib.h],
        [
          # check library version, update LIBS
	  if test x"$1" != x"$ax_check_libjpeg_version_num"; then
            AC_MSG_CHECKING([for libjpeg version >= $1 ($ax_check_libjpeg_version_num)])
          else
            AC_MSG_CHECKING([for libjpeg version >= $1])
          fi
          AC_RUN_IFELSE(
            [
              AC_LANG_SOURCE(
[#include <stdio.h>
#include "jpeglib.h"
int main (void)
{
  fprintf (stderr, "%d\n", JPEG_LIB_VERSION);
  return 0;
}]
              )
            ],
            [
              ax_check_libjpeg_version=`eval $ac_try 2>&1`
              AX_COMPARE_VERSION([$ax_check_libjpeg_version], [ge], [$ax_check_libjpeg_version_num], [ax_check_libjpeg="ok"], [ax_check_libjpeg="bad"])
              AC_MSG_RESULT([$ax_check_libjpeg_version, $ax_check_libjpeg])
            ],
            [
              ax_check_libjpeg="unknown"
              AC_MSG_RESULT([$ax_check_libjpeg])
            ],
            [AC_MSG_RESULT([cross-compiling, forced])]
          )  # AC_RUN_IFELSE
        ],
        [ax_check_libjpeg="no headers"]
      )  # AC_CHECK_HEADER
    ],
    [ax_check_libjpeg="not found"],
    []
  )  # AC_SEARCH_LIBS

  if test x"$ax_check_libjpeg" != x"ok"; then
    CPPFLAGS="$ax_check_libjpeg_save_cppflags"
    LDFLAGS="$ax_check_libjpeg_save_ldflags"
  fi
])
