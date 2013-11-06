# SYNOPSIS
#
#   AX_PROG_LD_STATIC
#
# DESCRIPTION
#
#   Try to determine the mode of static linking.  The macro guesses the proper
#   flag for static linking, caches the result, and updates LDFLAGS.
#
#   Code inspired from GNU libtool 1.4.2 (yes, I know this is a rather old one).
#
# LAST MODIFICATION
#
#   2006-06-04
#
# COPYLEFT
#
#   Copyright (c) 2006 Nicolas Calimet
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_PROG_LD_STATIC],
[
  AC_CACHE_VAL(
    [ax_cv_prog_ld_static],
    [
      AC_MSG_CHECKING([for linker static flag])
      if test x"$GCC" = x"yes"; then
        ax_cv_prog_ld_static="-static"
        case "$build_os" in
          aix*)
            ax_cv_prog_ld_static="$ax_cv_prog_ld_static -Wl,-lC"
            ;;
          irix*)  # [NC]
            ax_cv_prog_ld_static="$ax_cv_prog_ld_static -Wl,-Bstatic"
            ;;
        esac
      else
        case "$build_os" in
          aix3* | aix4* | aix5*)
            if text x"$build_cpu" = x"ia64"; then
              ax_cv_prog_ld_static="-Bstatic"
            else
              ax_cv_prog_ld_static="-bnso -bI:/lib/syscalls.exp"
            fi
            ;;
          hpux9* | hpux10* | hpux11*)
            ax_cv_prog_ld_static="-Wl,-a -Wl,archive"
            ;;
          irix5* | irix6*)
            ax_cv_prog_ld_static="-non_shared"
            ;;
          osf3* | osf4* | osf5*)
            ax_cv_prog_ld_static="-non_shared"
            ;;
          sco3.2v5*)
            ax_cv_prog_ld_static="-dn"
            ;;
          *)
            ax_cv_prog_ld_static="-Bstatic"
            ;;
        esac
      fi
      AC_MSG_RESULT([$ax_cv_prog_ld_static])

      # now check for working flag
      AC_MSG_CHECKING([for working '$ax_cv_prog_ld_static' flag])
      ax_prog_ld_static_save_ldflags="$LDFLAGS"
      LDFLAGS="$LDFLAGS $ax_cv_prog_ld_static"
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([], [])],
        [AC_MSG_RESULT([yes])],
        [
          AC_MSG_RESULT([no])
          AC_MSG_NOTICE([static linking does not work, revert to dynamic linking])
          ax_cv_prog_ld_static=""
          LDFLAGS="$ax_prog_ld_static_save_ldflags"
        ]
      )
    ]
  )
])
