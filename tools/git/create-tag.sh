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
# Extract version information from source.

if [ -f source/base/version.h ]
then
  versionfile=source/base/version.h
else
  versionfile=source/backend/povray.h
fi

ver_povray_h_3=`sed -n 's/#define [ ]*OFFICIAL_VERSION_STRING [ ]*"\([0-9]*\.[0-9]*\.[0-9]*\)"/\1/p' "$versionfile"`
ver_povray_h=`sed -n 's/#define [ ]*OFFICIAL_VERSION_STRING [ ]*"\([0-9.]*\)"/\1/p' "$versionfile"`
ver_povray_h_int=`sed -n 's/#define [ ]*OFFICIAL_VERSION_NUMBER [ ]*\([0-9]*\)/\1/p' "$versionfile"`
ver_povray_h_hex=`sed -n 's/#define [ ]*OFFICIAL_VERSION_NUMBER_HEX [ ]*\(0x[0]*[0-9]*\)/\1/p' "$versionfile"`
ver_version_h_major=`sed -n 's/#define [ ]*POV_RAY_MAJOR_VERSION_INT [ ]*\([0-9]*\)/\1/p' "$versionfile"`
ver_version_h_minor=`sed -n 's/#define [ ]*POV_RAY_MINOR_VERSION_INT [ ]*\([0-9]*\)/\1/p' "$versionfile"`
ver_version_h_revision=`sed -n 's/#define [ ]*POV_RAY_REVISION_INT [ ]*\([0-9]*\)/\1/p' "$versionfile"`
ver_version_h_patchlevel=`sed -n 's/#define [ ]*POV_RAY_PATCHLEVEL_INT [ ]*\([0-9]*\)/\1/p' "$versionfile"`
ver_version_h_prerelease=`sed -n 's/^.*#define [ ]*POV_RAY_PRERELEASE [ ]*"\([^"]*\)".*$/\1/p' "$versionfile"`

if [ x"$ver_povray_h" != x"" ]
then
  ver_version_h_3="$ver_povray_h_3"
  ver_version_h="$ver_povray_h"
else
  ver_version_h_3="$ver_version_h_major.$ver_version_h_minor.$ver_version_h_revision"
  if [ $ver_version_h_patchlevel -gt 0 ]
  then
    ver_version_h="$ver_version_h_3.$ver_version_h_patchlevel"
  else
    ver_version_h="$ver_version_h_3"
  fi
fi

if [ x"$ver_version_h_prerelease" != x"" ]
then
  ver_version_h="$ver_version_h-$ver_version_h_prerelease"
fi

if [ x"$ver_version_h_prerelease" = x"" ]
then
  echo "Identified as final release '$ver_version_h'."
  tag="v$ver_version_h"
else
  echo "Identified as pre-release '$ver_version_h'."
  if ! echo $ver_version_h_prerelease | grep -e '[0-9]$' >/dev/null
  then
    echo "Pre-release looks suspiciously non-unique; please tag manually!"
    exit 1
  fi
  tag="v$ver_version_h"
fi

echo "Tagging as '$tag'."

git tag "$tag"

########################################################################################################################
#
# Done.

exit 0
