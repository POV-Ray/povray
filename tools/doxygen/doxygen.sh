#!/bin/sh

# PLANTUML_JAR_PATH='/usr/bin'
cd ../..
POV_VER=`cat unix/VERSION`
doxygen tools/doxygen/source-doc.cfg
cd tools/doxygen/source-doc/latex
make pdf
cd ../..
mkdir pdf
cp "source-doc/latex/refman.pdf" "source-doc/pdf/POV-Ray Developer Manual.pdf"
