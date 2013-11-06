# SYNOPSIS
#
#   AX_ARG_ENABLE(FEATURE, HELP-STRING)
#
# DESCRIPTION
#
#   Basic replacement for AC_ARG_ENABLE().
#   FEATURE must include the '--enable' or '--disable' prefix.
#
# LAST MODIFICATION
#
#   2009-01-09
#
# COPYLEFT
#
#   Copyright (c) 2006 Nicolas Calimet
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_ARG_ENABLE],
[
  AC_ARG_ENABLE(m4_bpatsubst([$1], [^--.*able-], []),
    [AS_HELP_STRING([$1], [$2])],
    [
      case "$enableval" in
        yes|no|"") ;;
        *)         AC_MSG_ERROR([incorrect value '$enableval' for $1]) ;;
      esac
    ]
  )
])
