#!/bin/sh

# PLANTUML_JAR_PATH='/usr/bin'
cd ../..
eval `tools/unix/get-source-version.sh source/base/version.h`
POV_VER="$POV_RAY_FULL_VERSION"
doxygen tools/doxygen/source-doc.cfg
cd tools/doxygen/source-doc/latex
make pdf
cd ../..
mkdir pdf
cp "source-doc/latex/refman.pdf" "source-doc/pdf/POV-Ray Developer Manual.pdf"
