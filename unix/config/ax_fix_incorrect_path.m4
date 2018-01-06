# SYNOPSIS
#
#   AX_FIX_INCORRECT_PATH(ENVVAR, PATH, [ACTION-IF-FOUND], [ACTION-IF-MISSING])
#
# DESCRIPTION
#
#   Check whether the environment variable contains a given path, warn
#   and remove it.
#
# LAST MODIFICATION
#
#   2018-01-05
#
# COPYLEFT
#
#   Copyright (c) 2009 Nicolas Calimet
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_FIX_INCORRECT_PATH],
[
  AC_SUBST([$1])

  # process paths containing dots and create regexp
  ax_fix_incorrect_path_regexp=":`echo $2 | sed 's,\.,\\\\.,g'`:"
  echo ax_fix_incorrect_path_regexp = $ax_fix_incorrect_path_regexp >&AS_MESSAGE_LOG_FD

  # initial and processed variable values
  eval "ax_fix_incorrect_path_old=\$$1"
  ax_fix_incorrect_path_new=`echo :$ax_fix_incorrect_path_old: | sed s,$ax_fix_incorrect_path_regexp,:,g | sed s,^:,, | sed s,:$,,`
  echo ax_fix_incorrect_path_old    = $ax_fix_incorrect_path_old >&AS_MESSAGE_LOG_FD
  echo ax_fix_incorrect_path_new    = $ax_fix_incorrect_path_new >&AS_MESSAGE_LOG_FD

  AC_MSG_CHECKING([whether \$$1 contains the $2 path])
  if test x"$ax_fix_incorrect_path_new" != x"$ax_fix_incorrect_path_old"; then
    AC_MSG_RESULT([yes])
    AC_MSG_WARN([\$$1 is incorrectly set with the $2 path])
    eval $1=$ax_fix_incorrect_path_new
    ifelse([$3],[],[:],[$3])
  else
    AC_MSG_RESULT([no])
    ifelse([$4],[],[:],[$4])
  fi
])
