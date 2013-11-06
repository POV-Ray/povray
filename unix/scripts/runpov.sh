#!/bin/sh
#
# FILE : runpov.sh
#
# Run POV-Ray(tm) saving all the current settings into an INI file called
# rerun.ini.  Also see rerunpov.sh and the "Resuming Options" section in
# the User's Documentation.
# 
# From Mike Fleetwood

povray +GIrerun.ini ${@}
