# SYNOPSIS
#
#   AX_CHECK_COMPILER_FLAGS(FLAGS, [ACTION-IF-WORKS], [ACTION-IF-FAILS])
#
# DESCRIPTION
#
#   Check whether the current compiler supports a given set of flags,
#   cache the result, and update xxFLAGS.
#
#   Note: in principle I'd better use AC_COMPILE_IFELSE([AC_LANG_PROGRAM()])
#   but I don't remember whether it works well when using multiple flags.
#   I find it also safer to inspect stderr using a regular expression.
#   For safety, the compiler is checked with and without the tested flags
#   and the corresponding standard error outputs are compared.
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

AC_DEFUN([AX_TEST_COMPILER_FLAGS],
[
  AC_PREREQ(2.59)
  AC_REQUIRE([AC_PROG_EGREP])

  # Create a unique cache-id name (multiple flags are handled).
  ax_test_compiler_flags_var=ax_cv_test_[]_AC_LANG_ABBREV[]_flags`echo $1 | sed 's,[[^a-zA-Z0-9]],_,g'`

  # Create the extended regular expression to handle multiple flags.
  # For instance, "-first-flag -second-flag -X" gives the regexp:
  #   "\-f|irst-flag|\-s|econd-flag|\-X|X"
  #
  # FreeBSD and Darwin seem to have a problem with the \+ sed construct
  # (but apparently not with the \{x,y\} one).  For safety, I prefer to
  # use  [[:space:]][[:space:]]*  for  [[:space:]]\+
  ax_test_compiler_flags_regexp=`echo $1 | sed 's,\(-.\)\([[^[:space:]]]*\),\\\\\1|\2,g; s,[[[:space:]]][[[:space:]]]*,|,g; s,\(.\)||,\1|\1|,g; s,\(.\)|$,\1|\1,'`

  # get current compiler; don't use $ac_compiler
  eval ax_test_compiler=`set X $ac_compile; echo $[2]`

  AC_MSG_CHECKING([whether $ax_test_compiler accepts $1])
  AC_CACHE_VAL(
    [$ax_test_compiler_flags_var],
    [
      # Create a conftest file.
      AC_LANG_CONFTEST([AC_LANG_PROGRAM()])

      # Compile and inspect standard error for given flags.
      echo "$ax_test_compiler $_AC_LANG_PREFIX[]FLAGS -c conftest.$ac_ext 2> conftest.err0" >&AS_MESSAGE_LOG_FD
            $ax_test_compiler $_AC_LANG_PREFIX[]FLAGS -c conftest.$ac_ext 2> conftest.err0  >&AS_MESSAGE_LOG_FD
      echo "cat conftest.err0" >&AS_MESSAGE_LOG_FD
            cat conftest.err0  >&AS_MESSAGE_LOG_FD
      echo "$ax_test_compiler $_AC_LANG_PREFIX[]FLAGS $1 -c conftest.$ac_ext 2> conftest.err" >&AS_MESSAGE_LOG_FD
            $ax_test_compiler $_AC_LANG_PREFIX[]FLAGS $1 -c conftest.$ac_ext 2> conftest.err  >&AS_MESSAGE_LOG_FD
      echo "cat conftest.err" >&AS_MESSAGE_LOG_FD
            cat conftest.err  >&AS_MESSAGE_LOG_FD

      echo $EGREP "$ax_test_compiler_flags_regexp" conftest.err >&AS_MESSAGE_LOG_FD
      ax_test_compiler_flags_err=`$EGREP "$ax_test_compiler_flags_regexp" conftest.err 2>&1`
      echo $ax_test_compiler_flags_err >&AS_MESSAGE_LOG_FD

      echo diff conftest.err0 conftest.err >&AS_MESSAGE_LOG_FD
      ax_test_compiler_flags_diff=`diff conftest.err0 conftest.err 2>&1`
      echo $ax_test_compiler_flags_diff >&AS_MESSAGE_LOG_FD

      eval "$ax_test_compiler_flags_var=no"
      if test "$ax_test_compiler_flags_err" = x"" || test x"$ax_test_compiler_flags_diff" = x""; then
        # compile, link, and run the test program.
        echo "$ax_test_compiler $_AC_LANG_PREFIX[]FLAGS $1 conftest.$ac_ext -o conftest$ac_exeext 2> conftest.err" >&AS_MESSAGE_LOG_FD
              $ax_test_compiler $_AC_LANG_PREFIX[]FLAGS $1 conftest.$ac_ext -o conftest$ac_exeext 2> conftest.err  >&AS_MESSAGE_LOG_FD
        echo "cat conftest.err" >&AS_MESSAGE_LOG_FD
              cat conftest.err  >&AS_MESSAGE_LOG_FD
        echo "./conftest$ac_exeext 2> conftest.err >&2" >&AS_MESSAGE_LOG_FD
              ./conftest$ac_exeext 2> conftest.err >&2
        ax_test_compiler_flags_run=$?
        echo "cat conftest.err" >&AS_MESSAGE_LOG_FD
              cat conftest.err  >&AS_MESSAGE_LOG_FD

        # success
        if test x"`cat conftest.err`" = x"" && test x"$ax_test_compiler_flags_run" = x"0"; then
          eval "$ax_test_compiler_flags_var=yes"
        fi
      fi

      rm -f conftest.$ac_ext conftest.$ac_objext conftest.err conftest.err0 conftest$ac_exeext
    ]
  )
  eval "ax_test_compiler_flags_value=\$$ax_test_compiler_flags_var"
  AC_MSG_RESULT([$ax_test_compiler_flags_value])

  if test x"$ax_test_compiler_flags_value" = x"yes"; then
    _AC_LANG_PREFIX[]FLAGS="$_AC_LANG_PREFIX[]FLAGS $1"
    ifelse([$2],[],[:],[$2])
  else
    ifelse([$3],[],[:],[$3])
  fi
])
