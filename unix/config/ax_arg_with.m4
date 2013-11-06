# SYNOPSIS
#
#   AX_ARG_WITH(FEATURE, OPTIONAL_VALUE_DESCRIPTION, HELP-STRING)
#
# DESCRIPTION
#
#   Basic replacement for AC_ARG_WITH().
#   FEATURE must include the '--with' or '--without' prefix.
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

AC_DEFUN([AX_ARG_WITH],
[
  AC_ARG_WITH(m4_bpatsubst([$1], [^--with[^-]*-], []),
    [ifelse(len($2), 0, AS_HELP_STRING([$1], [$3]), AS_HELP_STRING([$1@<:@=$2@:>@], [$3]))])
])
