# SYNOPSIS
#
#   AX_CHECK_LIB(lib, required_version, search_libs, check_function, header, version_function, lib_dir)
#
# DESCRIPTION
#
#   Check whether a function is found in a set of libraries and compare
#   the library version to the required one.
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

AC_DEFUN([AX_CHECK_LIB],
[
  ax_check_lib_save_cppflags="$CPPFLAGS"
  ax_check_lib_save_ldflags="$LDFLAGS"
  if test x"$7" != x""; then
    CPPFLAGS="-I$7/../include $CPPFLAGS"
    LDFLAGS="-L$7 $LDFLAGS"
  fi

  # check the library
  AC_SEARCH_LIBS(
    [$4],
    [$3],
    [
      # check include file
      AC_CHECK_HEADER(
        [$5],
        [
          # check library version, update LIBS
          AC_MSG_CHECKING([for lib$1 version >= $2])
          AC_RUN_IFELSE(
            [
              AC_LANG_SOURCE(
[#include <stdio.h>
#include <string.h>
#include "$5"
int main (void)
{
  const char *version = $6;
  fprintf (stderr, "%s\n", version);
  return 0;
}]
              )
            ],
            [
              ax_check_lib_version=`eval $ac_try 2>&1`
              AX_COMPARE_VERSION([$ax_check_lib_version], [ge], [$2], [ax_check_lib="ok"], [ax_check_lib="bad"])
              AC_MSG_RESULT([$ax_check_lib_version, $ax_check_lib])
            ],
            [
              ax_check_lib="unknown"
              AC_MSG_RESULT([$ax_check_lib])
            ],
            [AC_MSG_RESULT([cross-compiling, forced])]
          )  # AC_RUN_IFELSE
        ],
        [ax_check_lib="no headers"]
      )  # AC_CHECK_HEADER
    ],
    [ax_check_lib="not found"],
    []
  )  # AC_SEARCH_LIBS

  if test x"$ax_check_lib" != x"ok"; then
    CPPFLAGS="$ax_check_lib_save_cppflags"
    LDFLAGS="$ax_check_lib_save_ldflags"
  fi
])
