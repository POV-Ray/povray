#!/bin/sh

# Run this script to tag the current revision (intended to trigger a development build).

tab=`echo -e '\t'`
nl='
'

########################################################################################################################
#
# Workarounds for quirks of particular runtime environments.

# A single sed expression to fix line endings, passed to sed via the '-e' option as the last expression to process.
# By default, this is a no-op expression.
sed_newline_fix=''

# A function to fix quirky behaviour of 'sed -i', called with a list of all files previously processed by 'sed -i'.
# By default, this is a no-op function.
sed_i_fix() { : ; }

case "$(uname -s)" in

   Darwin)
     echo 'Running on Mac OS X'
     # no known quirks to work around
     ;;

   Linux)
     echo 'Running on Linux'
     # no known quirks to work around
     ;;

   CYGWIN*|MINGW32*|MSYS*)
     echo 'Running on MS Windows'
     # 'sed' output has LF line endings, but we want CR+LF for consistency.
     sed_newline_fix='s/$/\r/g'
     # 'sed -i' sets the edited file to read-only.
     sed_i_fix() { chmod u+w "$@" ; }
     ;;

   *)
     echo 'Running on unidentified OS' 
     # no known quirks to work around
     ;;

esac

########################################################################################################################
#
# Update prerelease id (trailing numeric portion).
# We're setting the prerelease id to the number of minutes since 2000-01-01 00:00.

if [ -f source/base/version.h ]
then
  versionfile=source/base/version.h
else
  versionfile=source/backend/povray.h
fi

ver_povray_h=`sed -n 's/#define [ ]*OFFICIAL_VERSION_STRING [ ]*"\([0-9]*\.[0-9]*\.[0-9]*\)"/\1/p' "$versionfile"`
ver_povray_h_int=`sed -n 's/#define [ ]*OFFICIAL_VERSION_NUMBER [ ]*\([0-9]*\)/\1/p' "$versionfile"`
ver_povray_h_hex=`sed -n 's/#define [ ]*OFFICIAL_VERSION_NUMBER_HEX [ ]*\(0x[0]*[0-9]*\)/\1/p' "$versionfile"`
prerelease_povray_h=`sed -n 's/^.*#define [ ]*POV_RAY_PRERELEASE [ ]*"\([^"]*\)".*$/\1/p' "$versionfile"`

if [ x"$prerelease_povray_h" = x"" ]
then
  echo "Identified as final release '$ver_povray_h'."
  tag="v$ver_povray_h"
else
  echo "Identified as pre-release '$ver_povray_h-$prerelease_povray_h'."
  if ! echo $prerelease_povray_h | grep -e '[0-9][0-9]$' >/dev/null
  then
    echo "Pre-release looks suspiciously non-unique; please tag manually!"
    exit 1
  fi
  tag="v$ver_povray_h-$prerelease_povray_h"
fi

echo "Tagging as '$tag'."

git tag "$tag"

########################################################################################################################
#
# Done.

exit 0
