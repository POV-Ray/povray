#!/bin/sh
# ==============================================================================
# POV-Ray v3.8
# portfolio.sh - render the POV-Ray portfolio
# ==============================================================================
# written November 2003 by Christoph Hormann
# updated 2017-09-10 for POV-Ray v3.8 by Christoph Lipka
# This file is part of POV-Ray and subject to the POV-Ray licence
# see POVLEGAL.DOC for details
# ------------------------------------------------------------------------------
# calling conventions:
#
#   portfolio.sh [log] [-d scene_directory] [-o output_directory]
#
# output_directory: if specified all images are written to this directory
#                   if not specified the images are written into the scene 
#                   file directories, if these are not writable they are 
#                   written in the current directory.
# log:              log all text output of POV-Ray to a file (log.txt) 
# scene_directory:  if specified the portfolio scene in this directory are
#                   rendered, otherwise the scene directory is determined form
#                   the main povray ini file
#                   (usually /usr/local/share/povray-X.Y/scenes/portfolio,
#                   where X.Y represents the first two fields of the version
#                   number, e.g. for v3.8.1 this would be 3.8).
# ==============================================================================

# test mode
#SCENE_DIR=.

VERSION=`povray --generation`
VER_DIR=povray-$VERSION
DEFAULT_DIR=/usr/local
SYSCONFDIR=$DEFAULT_DIR/etc

install_dir()
{
  if [ -z "$POVINI" ] ; then
    test -f "$SYSCONFDIR/povray.ini" && POVINI="$SYSCONFDIR/povray.ini"
    test -f "$HOME/.povrayrc" && POVINI="$HOME/.povrayrc"
    test -f "$SYSCONFDIR/povray/$VERSION/povray.ini" && POVINI="$SYSCONFDIR/povray/$VERSION/povray.ini"
    test -f "$HOME/.povray/$VERSION/povray.ini" && POVINI="$HOME/.povray/$VERSION/povray.ini"
  fi

  if [ ! -z "$POVINI" ] ; then
    # this is not a completely failsafe method but it should work in most cases
    INSTALL_DIR=`grep -E -i "^library_path=.*share/$VER_DIR" "$POVINI" | head -n 1 | sed "s?[^=]*=\"*??;s?/share/$VER_DIR.*??"`
    echo "$INSTALL_DIR"
  fi
}

OPTIONS="$1 $2 $3 $4 $5"

case "$OPTIONS" in
  *log* | *LOG* | *Log* )
    DATE=`date`
    LOG_FILE="log.txt"
    echo "log file for POV-Ray v$VERSION sample scene render $DATE" > "$LOG_FILE"
    ;;
esac

test "$1" = "-d" && SCENE_DIR="$2"
test "$2" = "-d" && SCENE_DIR="$3"
test "$3" = "-d" && SCENE_DIR="$4"
test "$4" = "-d" && SCENE_DIR="$5"

if [ -z "$SCENE_DIR" ] ; then
  INSTALL_DIR="`install_dir`"
  if [ -z "$INSTALL_DIR" ] ; then
		echo "------------------------------------------------------"
    echo "  the sample scene render script could not determine"
    echo "  the location where POV-Ray is installed.  Make sure"
    echo "  POV-Ray v$VERSION has been correctly installed on this"
    echo "  computer.  If you continue the script will try to"
    echo "  the scenes from the current directory."
    echo ""
    read -p "Press CTRL-C to abort or any other key to continue " -n 1
		echo "------------------------------------------------------"

    SCENE_DIR=.
  else
    SCENE_DIR="$INSTALL_DIR/share/$VER_DIR/scenes/portfolio"
  fi
fi

if [ ! -d "$SCENE_DIR" ] ; then
	echo "------------------------------------------------------"
  echo "  Your POV-Ray installation seems to be defective"
  echo "  so this script does not work."
  echo "  Try reinstalling POV-Ray."
	echo "------------------------------------------------------"
  read
  exit
fi

if [ -d "$SCENE_DIR/portfolio" ] ; then
  SCENE_DIR="$SCENE_DIR/portfolio"
fi

test "$1" = "-o" && OUTPUT_DIR="$2"
test "$2" = "-o" && OUTPUT_DIR="$3"
test "$3" = "-o" && OUTPUT_DIR="$4"
test "$4" = "-o" && OUTPUT_DIR="$5"

if [ -z "$OUTPUT_DIR" ] ; then
  if [ -w "$SCENE_DIR" ] ; then
    OUTPUT_DIR="$SCENE_DIR"
  else
    OUTPUT_DIR=.
  fi
fi

if [ ! -d "$OUTPUT_DIR" ] ; then
  mkdir -p "$OUTPUT_DIR"
fi

if [ "$SCENE_DIR" != "$OUTPUT_DIR" ] ; then
  test -f "$SCENE_DIR/index.html" && cp -f "$SCENE_DIR/index.html" "$OUTPUT_DIR/"
  test -f "$SCENE_DIR/readme.txt" && cp -f "$SCENE_DIR/readme.txt" "$OUTPUT_DIR/"
fi

CURR_DIR=`pwd`

SCENE_DIR=`echo "$SCENE_DIR" | sed "s?^\.?$CURR_DIR?"`
SCENE_DIR=`echo "$SCENE_DIR" | sed "s?^\([^/]\)?$CURR_DIR/\1?"`

FILE_LIST=`find "$SCENE_DIR" -not -path "*__empty*" -name "*.ini" | sort`

cd "$OUTPUT_DIR"

#echo "$FILE_LIST"
#echo "-------"
#echo "$OUTPUT_DIR"
#echo "$SCENE_DIR"

if [ -z "$LOG_FILE" ] ; then
  echo "$FILE_LIST" | xargs -n 1 povray +L$SCENE_DIR
else
  echo "$FILE_LIST" | xargs -n 1 povray +L$SCENE_DIR 2>&1 | tee -a "$LOG_FILE"
fi

cd "$CURR_DIR"
