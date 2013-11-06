# SYNOPSIS
#
#   AX_COMPILER_VERSION
#
# DESCRIPTION
#
#   Try to determine the version of the current compiler.
#   Result is output in $ax_compiler_version
#
#
# LAST MODIFICATION
#
#   2009-01-16
#
# COPYLEFT
#
#   Copyright (c) 2009 Nicolas Calimet
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_COMPILER_VERSION],
[
  AC_REQUIRE([AC_PROG_EGREP])
  ax_compiler_version=

  # get current compiler; don't use $ac_compiler
  eval ax_compiler=`set X $ac_compile; echo $[2]`
  AC_MSG_CHECKING([for $ax_compiler version])

  filter1="$EGREP [[0-9]]"
  filter2="tr A-Z a-z | $EGREP version | sed 's,\(.*version[[[:space:]]]*\)\(.*\),\2,'"
  filter3="head -n 1 | sed 's,.*[[[:space:]]],,'"

  # check for various version options and remove leading/trailing blanks
  for ax_compiler_version_option_filter in -dumpversion:filter1 -v:filter2 -V:filter2 --version:filter3
  do
    if test x"$ax_compiler_version" = x""; then
      ax_compiler_version_option=`echo $ax_compiler_version_option_filter | cut -d: -f1`
      ax_compiler_version_filter=`echo $ax_compiler_version_option_filter | cut -d: -f2`
      eval ax_compiler_version_filter=\$$ax_compiler_version_filter
      ax_compiler_version_try="$ax_compiler $ax_compiler_version_option < /dev/null > conftest.out 2>&1"
      echo try option $ax_compiler_version_option with filter \"$ax_compiler_version_filter\" >&AS_MESSAGE_LOG_FD
      echo $ax_compiler_version_try >&AS_MESSAGE_LOG_FD
      eval $ax_compiler_version_try
      cat conftest.out >&AS_MESSAGE_LOG_FD
      ax_compiler_version=`eval cat conftest.out \| $ax_compiler_version_filter`
      ax_compiler_version=`echo $ax_compiler_version`
    fi
  done

  # print compiler version string
  if test x"$ax_compiler_version" = x""; then
    AC_MSG_RESULT([unkown])
  else
    AC_MSG_RESULT([$ax_compiler_version])
  fi

  rm -f conftest.out
])
