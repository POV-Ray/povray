# SYNOPSIS
#
#   AX_X86_ARCH
#
# DESCRIPTION
#
#   Try to determine the current x86 or x86-64 architecture by reading
#   the /proc filesystem or `dmesg' output.  The architecture name
#   follows the gcc naming convention used for e.g. the -march flag.
#
#   The macro sets the following variables:
#     ax_x86_arch                detected architecture or "unknown"
#     ax_x86_arch_fallback       fallback architecture (e.g. "i686" for "pentium4")
#     ax_x86_cpuinfo             cpuinfo file that is being used
#     ax_x86_cpumodel_orig       original string for the CPU model
#     ax_x86_cpumodel_nofreq     same but without clock speed info
#     ax_x86_cpumodel            processed string for the CPU model
#     ax_x86_vendorid            vendor ID string
#     ax_x86_cpuflags            all supported cpu flags
#     ax_x86_cpuflags_SSE_list   space-seperated list of SSE* supported, e.g. "SSE2 SSE4.1"
#
# LAST MODIFICATION
#
#   2009-01-16
#
# COPYLEFT
#
#   Copyright (c) 2006 Nicolas Calimet
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_X86_ARCH],
[
  # head is used to handle SMP/HT systems
  # check whether 'head -n 1' is supported; otherwise use older 'head -1'
  if test x"`head -n 1 /dev/null 2>&1`" = x""; then
    head_1='head -n 1'
  else
    head_1='head -1'
  fi

  # get machine info
  if test -r /proc/cpuinfo ; then  # Linux
    ax_x86_cpuinfo=/proc/cpuinfo
  elif test -r /compat/linux/proc/cpuinfo ; then  # FreeBSD linprocfs
    ax_x86_cpuinfo=/compat/linux/proc/cpuinfo
  elif test -r /emul/linux/proc ; then  # NetBSD linprocfs
    ax_x86_cpuinfo=/emul/linux/proc
  else
    case "`uname -s 2>/dev/null || echo nothing`" in
      NetBSD)
        ax_x86_cpumodel_orig=`grep cpu0: /var/run/dmesg.boot | $head_1 | cut -d: -f2 | cut -d, -f1`
        ax_x86_cpuflags=`grep 'cpu0: features' /var/run/dmesg.boot | cut -d'<' -f2 | tr 'A-Z,>\n' 'a-z   '`
        ax_x86_vendorid="unknown"  # not reported as such
      ;;
      FreeBSD)
        ax_x86_cpumodel_orig=`grep CPU /var/run/dmesg.boot | $head_1 | cut -d: -f2`
        ax_x86_cpuflags=`grep Features /var/run/dmesg.boot | $head_1 | cut -d'<' -f2 | tr 'A-Z,>' 'a-z  '`
        ax_x86_vendorid=`grep Origin /var/run/dmesg.boot | $head_1 | cut -d'"' -f2`
      ;;
    esac
  fi

  if test x"$ax_x86_cpuinfo" != x""; then
    ax_x86_cpumodel_orig=`cat $ax_x86_cpuinfo | grep 'model name' | $head_1 | cut -d: -f2`
    ax_x86_cpumodel_orig=`echo $ax_x86_cpumodel_orig`  # remove leading/trailing blanks
    ax_x86_cpuflags=`cat $ax_x86_cpuinfo | grep flags | $head_1 | cut -d: -f2`
    ax_x86_cpuflags=`echo $ax_x86_cpuflags`  # remove leading/trailing blanks
    ax_x86_vendorid=`cat $ax_x86_cpuinfo | grep vendor_id | $head_1 | cut -d: -f2`
    ax_x86_vendorid=`echo $ax_x86_vendorid`  # remove leading/trailing blanks
  fi

  # remove clockspeed info
  ax_x86_cpumodel_nofreq=`echo $ax_x86_cpumodel_orig | sed -e 's,Processor.*$,,' -e 's,CPU.*$,,' -e 's,[[.[:alnum:]]]*[[hH]]z,,g' -e 's,[[[:digit:]]]\++,,g'`
  ax_x86_cpumodel_nofreq=`echo $ax_x86_cpumodel_nofreq`  # remove leading/trailing blanks

  # convert to lowercase, remove "mobile" and "(tm)"
  ax_x86_cpumodel=`echo $ax_x86_cpumodel_nofreq | tr A-Z a-z`
  ax_x86_cpumodel=${ax_x86_cpumodel:-`uname -p 2> /dev/null`}
  ax_x86_cpumodel=`echo $ax_x86_cpumodel | sed -e 's,mobile,,g' -e 's,(tm),,g'`

  # log values
  echo ax_x86_cpuinfo           = $ax_x86_cpuinfo         >&AS_MESSAGE_LOG_FD
  echo ax_x86_cpumodel_orig     = $ax_x86_cpumodel_orig   >&AS_MESSAGE_LOG_FD
  echo ax_x86_cpumodel_nofreq   = $ax_x86_cpumodel_nofreq >&AS_MESSAGE_LOG_FD
  echo ax_x86_cpumodel          = $ax_x86_cpumodel        >&AS_MESSAGE_LOG_FD
  echo ax_x86_vendorid          = $ax_x86_vendorid        >&AS_MESSAGE_LOG_FD
  echo ax_x86_cpuflags          = $ax_x86_cpuflags        >&AS_MESSAGE_LOG_FD

  # list of supported SSE instruction set(s)
  ax_x86_cpuflags_SSE_list=
  for flag in $ax_x86_cpuflags
  do
    case "$flag" in
      sse)            ax_x86_cpuflags_SSE_list="$ax_x86_cpuflags_SSE_list SSE";;
      sse2)           ax_x86_cpuflags_SSE_list="$ax_x86_cpuflags_SSE_list SSE2";;
      sse3|pni)       ax_x86_cpuflags_SSE_list="$ax_x86_cpuflags_SSE_list SSE3";;   # prescott new instructions
      ssse3|tni|mni)  ax_x86_cpuflags_SSE_list="$ax_x86_cpuflags_SSE_list SSSE3";;  # tejas|merom new instructions
      sse4a)          ax_x86_cpuflags_SSE_list="$ax_x86_cpuflags_SSE_list SSE4a";;  # barcelona
      sse4_1)         ax_x86_cpuflags_SSE_list="$ax_x86_cpuflags_SSE_list SSE4.1";; # penryn
      sse4_2)         ax_x86_cpuflags_SSE_list="$ax_x86_cpuflags_SSE_list SSE4.2";; # nehalem (core i7)
    esac
  done
  ax_x86_cpuflags_SSE_list=`echo $ax_x86_cpuflags_SSE_list`
  echo ax_x86_cpuflags_SSE_list = $ax_x86_cpuflags_SSE_list >&AS_MESSAGE_LOG_FD

  # processor architecture
  if test x"$ax_x86_arch" = x""; then
    case "$build" in
      i386-*)    ;;  # *BSD always says i386; handled below
      i486-*)    ax_x86_arch="i486";;
      i586-*)    ax_x86_arch="pentium";;
      i686-*)    ;;  # handled below
      k6-*)      ;;  # handled below
      k7-*)      ;;  # handled below
      k8-*)      ;;  # handled below
      x86_64-*)  ;;  # handled below
      *)         ax_x86_arch="unknown";;
    esac
  fi

  # processor model
  if test x"$ax_x86_arch" = x""; then
    case "$ax_x86_cpumodel" in
      # Intel
      *xeon*)          ax_x86_arch="pentium4";;    # FIXME: Core2-based xeon
      *intel*core*i7*) ax_x86_arch="core2";;       # FIXME
      *intel*core2*)   ax_x86_arch="core2";;       # use "intel" for consistency with the test below
      *intel*core*)    ax_x86_arch="pentium4";;    # use "intel" to avoid catching e.g. "athlon 64 x2 dual core"
      *pentium*4*)     ax_x86_arch="pentium4";;
      *pentium*mmx*)   ax_x86_arch="pentium";;     # catch this first
      *pentium*iii*)   ax_x86_arch="pentium3";;    # catch pentium III Mxxx before Pentium-M
      *pentium*ii*)    ax_x86_arch="pentium2";;
      *pentium*m*)     ax_x86_arch="pentium-m";;
      *pentium*pro*)   ax_x86_arch="pentiumpro";;
      *pentium*)       ax_x86_arch="pentium";;
      *celeron*)       ax_x86_arch="pentium3";;    # assume all celerons belong here (but see below)

      # AMD; flags are for GCC only but see the ICC tweaks below
      *phenom*)       ax_x86_arch="barcelona";;
      *opteron*)      ax_x86_arch="k8";;
      *turion*)       ax_x86_arch="k8";;
      *athlon*64*)    ax_x86_arch="k8";;
      *athlon*xp*)    ax_x86_arch="athlon-xp";;
      *athlon*mp*)    ax_x86_arch="athlon-mp";;
      *athlon*)       ax_x86_arch="athlon";;
      *sempron*64*)   ax_x86_arch="k8";;
      *sempron*)      ax_x86_arch="athlon-xp";;   # assume all semprons belong here (but see below)
      *duron*)        ax_x86_arch="athlon";;
      *k6*)           ax_x86_arch="k6";;
      *k7*)           ax_x86_arch="k7";;
      *k8*)           ax_x86_arch="k8";;
      *x86_64*)       ax_x86_arch="k8";;

      # others; don't support true i386
      *)              ax_x86_arch="i686";;
    esac  # ax_x86_cpumodel

    # celerons with SSE2/SSE3
    if test x"$ax_x86_arch" = x"pentium3"; then
      case "$ax_x86_cpuflags_SSE_list" in
        *SSE3*)  ax_x86_arch="prescott";;
        *SSE2*)  ax_x86_arch="pentium4";;
      esac
    fi

    # semprons with SSE3
    if test x"$ax_x86_arch" = x"athlon-xp"; then
      case "$ax_x86_cpuflags_SSE_list" in
        *SSE3*)  ax_x86_arch="k8";;
      esac
    fi

    # fallback
    case "$ax_x86_arch" in
      k6|pentium)               ax_x86_arch_fallback="i586";;
      core*|pentium*|prescott)  ax_x86_arch_fallback="i686";;
      k7|k8|athlon*)            ax_x86_arch_fallback="i686";;
      *)                        ax_x86_arch_fallback="i386";;
    esac
  fi  # processor model

  echo ax_x86_arch              = $ax_x86_arch          >&AS_MESSAGE_LOG_FD
  echo ax_x86_arch_fallback     = $ax_x86_arch_fallback >&AS_MESSAGE_LOG_FD
])
