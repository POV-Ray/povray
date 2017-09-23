# Extract POV-Ray version information from `source/base/version.h`
#
# From Unix shell scripts, run as follows:
#
#   eval `./tools/unix/get-source-version.sh ./source/base/version.h`
#
# This procedure will cause the following envionment variables to be set:
#
#   POV_RAY_COPYRIGHT       Copyright string
#   POV_RAY_GENERATION      First two fields of the version string (`X.Y`)
#   POV_RAY_FULL_VERSION    Full version string (`X.Y.Z`[`.P`][`-PRE`])
#   POV_RAY_PRERELEASE      Pre-release tag portion of the version string (`PRE`), or empty if not applicable

ExtractData() {

local version_h="$1"

GetMacro() {
  local file="$1"
  local macro="$2"
  local pattern="$3"
  # NB: The following regexp deliberately does not probe until end of line, to allow for CR/LF line endings.
  sed -n 's,^ *#define  *'"$macro""$pattern"',\1,p' "$file"
}

GetNumericMacro() {
  GetMacro "$1" "$2" '  *\([0-9][0-9]*\)'
}

GetStringMacro() {
  GetMacro "$1" "$2" ' *"\([^"]*\)"'
}

local copyright=`GetStringMacro "$version_h" POV_RAY_COPYRIGHT`

local major=`GetNumericMacro "$version_h" POV_RAY_MAJOR_VERSION_INT`
local minor=`GetNumericMacro "$version_h" POV_RAY_MINOR_VERSION_INT`
local revision=`GetNumericMacro "$version_h" POV_RAY_REVISION_INT`
local patchlevel=`GetNumericMacro "$version_h" POV_RAY_PATCHLEVEL_INT`

local prerelease=`GetStringMacro "$version_h" POV_RAY_PRERELEASE`

local generation="$major.$minor"
if test "$patchlevel" -eq 0 ; then
  local release="$generation.$revision"
else
  local release="$generation.$revision.$patchlevel"
fi
if test x"$prerelease" != x"" ; then
  local version="$release-$prerelease"
else
  local version="$release"
fi

cat << hereEOF
POV_RAY_COPYRIGHT="$copyright"
POV_RAY_GENERATION="$generation"
POV_RAY_FULL_VERSION="$version"
POV_RAY_PRERELEASE="$prerelease"
hereEOF

}

ExtractData "$@"