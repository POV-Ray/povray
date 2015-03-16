#!/bin/sh

# PLANTUML_JAR_PATH='/usr/bin'
POV_VER=`cat ../../unix/VERSION`
doxygen doxygen.cfg
cd latex
make pdf
cd ..
mkdir pdf
cp "latex/refman.pdf" "pdf/POV-Ray Developer's Manual.pdf"
