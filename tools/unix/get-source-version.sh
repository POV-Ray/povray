#!/bin/sh

# Extract POV-Ray version information from `source/base/version.h`
#
# From Unix shell scripts, run as follows:
#
#   eval `./tools/unix/get-source-version.sh ./source/base/version.h`
#
# From GitHub Workflow scripts, run as a dedicated step as follows:
#
#   - shell: bash
#     run: ./tools/unix/get-source-version.sh ./source/base/version.h -github_env $GITHUB_ENV
#
# All procedures will cause the following envionment variables to be set:
#
#   POV_RAY_COPYRIGHT       Copyright string
#   POV_RAY_GENERATION      First two fields of the version string (`X.Y`)
#   POV_RAY_FULL_VERSION    Full version string (`X.Y.Z`[`.P`][`-PRE`])
#   POV_RAY_PRERELEASE      Pre-release tag portion of the version string (`PRE`), or empty if not applicable
#   POV_RAY_HOST_VERSION    First two fields of the "host" version string (`V.W`), or empty if not applicable

version_h="$1"
format="$2"
outfile="$3"

GetMacro() {
  file="$1"
  macro="$2"
  pattern="$3"
  # NB: The following regexp deliberately does not probe until end of line, to allow for CR/LF line endings.
  sed -n 's,^ *#define  *'"$macro""$pattern"',\1,p' "$file"
}

GetNumericMacro() {
  GetMacro "$1" "$2" '  *\([0-9][0-9]*\)'
}

GetStringMacro() {
  GetMacro "$1" "$2" ' *"\([^"]*\)"'
}

copyright=`GetStringMacro "$version_h" POV_RAY_COPYRIGHT`

major=`GetNumericMacro "$version_h" POV_RAY_MAJOR_VERSION_INT`
minor=`GetNumericMacro "$version_h" POV_RAY_MINOR_VERSION_INT`
revision=`GetNumericMacro "$version_h" POV_RAY_REVISION_INT`
patchlevel=`GetNumericMacro "$version_h" POV_RAY_PATCHLEVEL_INT`

prerelease=`GetStringMacro "$version_h" POV_RAY_PRERELEASE`
hostversion=`GetStringMacro "$version_h" POV_RAY_HOST_VERSION`

generation="$major.$minor"
if test "$patchlevel" -eq 0 ; then
  release="$generation.$revision"
else
  release="$generation.$revision.$patchlevel"
fi
if test x"$prerelease" != x"" ; then
  version="$release-$prerelease"
else
  version="$release"
fi

case "$format" in

  '')
    SetVariable() {
      echo "$1='$2' ;"
    }
    ;;

  -github_env)
    SetVariable() {
      echo "$1=$2" >> "$outfile"
    }
    ;;

esac

SetVariable POV_RAY_COPYRIGHT    "$copyright"
SetVariable POV_RAY_GENERATION   "$generation"
SetVariable POV_RAY_FULL_VERSION "$version"
SetVariable POV_RAY_PRERELEASE   "$prerelease"
SetVariable POV_RAY_HOST_VERSION "$hostversion"
