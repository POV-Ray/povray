# SYNOPSIS
#
#   AX_CHECK_LIBTIFF(required_version, lib_dir)
#
# DESCRIPTION
#
#   Check whether the system has libtiff version required_version.
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

AC_DEFUN([AX_CHECK_LIBTIFF],
[
  ax_check_libtiff_save_cppflags="$CPPFLAGS"
  ax_check_libtiff_save_ldflags="$LDFLAGS"
  if test x"$2" != x""; then
    CPPFLAGS="-I$2/../include $CPPFLAGS"
    LDFLAGS="-L$2 $LDFLAGS"
  fi

  # check the library
  AC_SEARCH_LIBS(
    [TIFFGetVersion],
    [tiff],
    [
      # check include file
      AC_CHECK_HEADER(
        [tiffio.h],
        [
          # check library version, update LIBS
          AC_MSG_CHECKING([for libtiff version >= $1])
          AC_RUN_IFELSE(
            [
              AC_LANG_SOURCE(
[#include <stdio.h>
#include <string.h>
#include "tiffio.h"
int main (void)
{
  char version[[81]];
  char *p, *c;
  sprintf (version, "%.79s", TIFFGetVersion ());
  p = strstr (version, "Version ") + strlen ("Version ");
  if (! p)
    return 1;
  c = strchr (p, '\n');
  if (! c)
    return 1;
  *c = '\0';
  fprintf (stderr, "%s\n", p);
  return 0;
}]
              )
            ],
            [
              ax_check_libtiff_version=`eval $ac_try 2>&1`
              AX_COMPARE_VERSION([$ax_check_libtiff_version], [ge], [$1], [ax_check_libtiff="ok"], [ax_check_libtiff="bad"])
              AC_MSG_RESULT([$ax_check_libtiff_version, $ax_check_libtiff])
            ],
            [
              ax_check_libtiff="unknown"
              AC_MSG_RESULT([$ax_check_libtiff])
            ],
            [AC_MSG_RESULT([cross-compiling, forced])]
          )  # AC_RUN_IFELSE
        ],
        [ax_check_libtiff="no headers"]
      )  # AC_CHECK_HEADER
    ],
    [ax_check_libtiff="not found"],
    []
  )  # AC_SEARCH_LIBS

  if test x"$ax_check_libtiff" != x"ok"; then
    CPPFLAGS="$ax_check_libtiff_save_cppflags"
    LDFLAGS="$ax_check_libtiff_save_ldflags"
  fi
])
