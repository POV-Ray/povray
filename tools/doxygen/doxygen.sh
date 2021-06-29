#!/bin/bash

# Remember the directory we're currently working in.
olddir=`pwd`

# Make sure we're (initially) working in the directory this script resides in.
cd `dirname $0`

export PLANTUML_JAR_PATH=`pwd`/plantuml

# The next steps expect us to be working in the source tree root directory.
cd ../..

eval `tools/unix/get-source-version.sh source/base/version.h`
export POV_VER="$POV_RAY_FULL_VERSION"
if ! doxygen tools/doxygen/source-doc.cfg ; then
  cd "$olddir"
  exit 1
fi

# The step to generate the PDF version expects us to be working in the
# output directory of the LaTeX version
cd tools/doxygen/source-doc/latex

if ! make pdf ; then
  cd "$olddir"
  exit 1
fi

# We choose to do the final steps working in the directory this script resides in.
cd ../..

mkdir source-doc/pdf
cp "source-doc/latex/refman.pdf" "source-doc/pdf/povray-v$POV_RAY_FULL_VERSION-sourcedoc.pdf"

# Let's go back to whatever directory we were initially working in.
cd "$olddir"
