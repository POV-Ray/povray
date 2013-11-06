#!/bin/sh
#
# FILE : rerunpov.sh
#
# Re-run POV-Ray(tm) using all the previously saved setting from an INI file
# called rerun.ini.  Also see runpov.sh and the "Resuming Options" section in
# the User's Documentation.
#
# From Mike Fleetwood

povray rerun.ini ${@}
