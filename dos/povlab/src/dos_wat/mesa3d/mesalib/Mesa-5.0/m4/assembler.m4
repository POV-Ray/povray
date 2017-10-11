# This file defines various checks for features of an x86 system's assembler.
# Note that these _only_ check whether _this_ systems assembler supports the
# feature, not that it will run on any particular processor.


# Try to run an assembler program
# Syntax similar to AC_TRY_COMPILE
AC_DEFUN(MESA_TRY_ASSEMBLE, [
  cat > conftest.S <<EOF
$1

$2
EOF
  save_ac_ext="$ac_ext"
  ac_ext=S
  if AC_TRY_EVAL(ac_compile); then
    $3;
  else
    $4;
  fi
  ac_ext="$save_ac_ext"
  rm -f conftest.S
])

# Check for a feature by trying to assemble some assembler code.
# $1: feature name
# $2: assembler code utilizing feature
# Examine cache variable mesa_cv_sys_as_$1 for result.
# Includes Mesa3D's src/X86/assyntax.h file, so only useful within Mesa3D.
AC_DEFUN(MESA_SYS_AS_FEATURE, [
  AC_CACHE_CHECK(whether the assembler supports $1,
                 mesa_cv_sys_as_$1,
                 # NOTE: Be sure to add the include directory for assyntax.h
                 # to CPPFLAGS before calling this macro!
                 MESA_TRY_ASSEMBLE([#include "assyntax.h"], $2,
                                   mesa_cv_sys_as_$1=yes,
                                   mesa_cv_sys_as_$1=no))
])


#
# Check for particular x86 extensions
#

# These macros are named MESA_SYS_AS_X where X is the name for the feature.
# They are wrappers to MESA_SYS_AS_FEATURE with a suitable assembler test code
# built-in. Therefore the semantics of a call to
#   MESA_SYS_AS_SOMEFEAT
# are equivalent to those of
#   MESA_SYS_AS_FEATURE(SomeFeat, asm-code-using-somefeat)

# Check whether the assembler supports the cpuid command
AC_DEFUN(MESA_SYS_AS_CPUID, [
  MESA_SYS_AS_FEATURE(cpuid, CPUID)
])

# Check whether the assembler supports the MMX command set
AC_DEFUN(MESA_SYS_AS_MMX, [
  MESA_SYS_AS_FEATURE(MMX, [MOVQ(MM4, MM5)])
])

# Check whether the assembler supports the SSE command set
AC_DEFUN(MESA_SYS_AS_SSE, [
  MESA_SYS_AS_FEATURE(SSE, [MULPS(XMM4 ,XMM0)])
])

# Check whether the assembler supports the 3DNow! commmand set
AC_DEFUN(MESA_SYS_AS_3DNOW, [
  MESA_SYS_AS_FEATURE(3DNow, [PFADD(MM0, MM2)])
])
