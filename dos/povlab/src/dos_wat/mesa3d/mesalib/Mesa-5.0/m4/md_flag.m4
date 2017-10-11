dnl $Id: md_flag.m4,v 1.1 2001/03/19 23:04:58 pesco Exp $

dnl Check for the compiler flag that updates dependencies (usually -MD), store
dnl that flag in variable MESA_MD_FLAG. A value of 'none' means that no
dnl appropriate flag could be found, i.e. we don't know how to do dependency
dnl tracking with the current compiler.
AC_DEFUN(MESA_SYS_CC_MD_FLAG,
  [AC_PROVIDE([$0])
   AC_REQUIRE([AC_CANONICAL_HOST])
   case "$host_os" in
     irix5* | irix6*)
       mesa_md_flag_choices="-MDupdate -MD"
       ;;
     *)
       mesa_md_flag_choices="-MD"
       ;;
   esac
   AC_CACHE_CHECK(which compiler flag updates dependencies,
     mesa_cv_sys_cc_md_flag,
     [mesa_cv_sys_cc_md_flag=none
      mesa_save_CFLAGS=$CFLAGS
      for mesa_flag in $mesa_md_flag_choices; do
        CFLAGS="-Wp,$mesa_flag,md_flag_test.d"
        AC_TRY_COMPILE(/*empty*/, /*empty*/,
          [if test -f md_flag_test.d; then
             mesa_cv_sys_cc_md_flag=$mesa_flag
             rm -f md_flag_test.d
             break
           fi])
      done
      CFLAGS=$mesa_save_CFLAGS])
   MESA_MD_FLAG=$mesa_cv_sys_cc_md_flag])
