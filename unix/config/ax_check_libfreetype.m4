# SYNOPSIS
#
#   AX_CHECK_LIBFREETYPE(required_version, lib_dir)
#
# DESCRIPTION
#
#   Check whether the system has libfreetype version required_version.
#
# LAST MODIFICATION
#
#   2019-01-16
#
# COPYLEFT
#
#   Copyright (c) 2019 Christoph Lipka
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_CHECK_LIBFREETYPE],
[
  AC_CHECK_PROGS([PKGCONFIG], [pkg-config])

  ax_check_libfreetype_save_cppflags="$CPPFLAGS"
  ax_check_libfreetype_save_ldflags="$LDFLAGS"
  if test x"$2" != x""; then
    CPPFLAGS="-I$2/../include $CPPFLAGS"
    LDFLAGS="-L$2 $LDFLAGS"
  else
    AC_MSG_CHECKING([for FreeType's include path])
    ax_check_freetype_cflags=`pkg-config --cflags freetype2 2>&1`
    if test x"$test ax_check_freetype_cflags" != x""; then
      CPPFLAGS="$ax_check_freetype_cflags $CPPFLAGS"
      AC_MSG_RESULT([$ax_check_freetype_cflags])
    else
      AC_MSG_RESULT([not found])
    fi
  fi

  # check the library
  AC_SEARCH_LIBS(
    [FT_Init_FreeType],
    [freetype],
    [
      # check include file
      AC_CHECK_HEADER(
        [ft2build.h],
        [
          # check library version, update LIBS
          AC_MSG_CHECKING([for libfreetype version >= $1])
          AC_RUN_IFELSE(
            [
              AC_LANG_SOURCE(
[#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H
int main (void)
{
  FT_Library ftLib;
  FT_Init_FreeType(&ftLib);
  FT_Int ftMajor = 0, ftMinor = 0, ftPatch = 0;
  FT_Library_Version(ftLib, &ftMajor, &ftMinor, &ftPatch);
  FT_Done_FreeType(ftLib);
  fprintf (stderr, "%d.%d.%d\n", int(ftMajor), int(ftMinor), int(ftPatch));
  return 0;
}]
              )
            ],
            [
              ax_check_libfreetype_version=`eval $ac_try 2>&1`
              AX_COMPARE_VERSION([$ax_check_libfreetype_version], [ge], [$1], [ax_check_libfreetype="ok"], [ax_check_libfreetype="bad"])
              AC_MSG_RESULT([$ax_check_libfreetype_version, $ax_check_libfreetype])
            ],
            [
              ax_check_libfreetype="unknown"
              AC_MSG_RESULT([$ax_check_libfreetype])
            ],
            [AC_MSG_RESULT([cross-compiling, forced])]
          )  # AC_RUN_IFELSE
        ],
        [ax_check_libfreetype="no headers"]
      )  # AC_CHECK_HEADER
    ],
    [ax_check_libfreetype="not found"],
    []
  )  # AC_SEARCH_LIBS

  if test x"$ax_check_libfreetype" != x"ok"; then
    CPPFLAGS="$ax_check_libfreetype_save_cppflags"
    LDFLAGS="$ax_check_libfreetype_save_ldflags"
  fi
])
