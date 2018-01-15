#!/bin/sh
# ==============================================================================
# POV-Ray v3.8
# allscene.sh - render all POV-Ray sample scenes
# ==============================================================================
# written November 2003 - January 2004 by Christoph Hormann
# updated 2017-09-10 for POV-Ray v3.8 by Christoph Lipka
# This file is part of POV-Ray and subject to the POV-Ray licence
# see POVLEGAL.DOC for details.
# ------------------------------------------------------------------------------
# calling conventions:
#
#   allscene.sh [log] [all] [-d scene_directory] [-o output_directory]
#
# output_directory: if specified all images are written to this directory
#                   if not specified the images are written into the scene 
#                   file directories, if these are not writable they are 
#                   written in the current directory.
# log:              log all text output of POV-Ray to a file (log.txt) 
# all:              also render scenes that don't contain any suggested 
#                   options for running
# scene_directory:  if specified the sample scene in this directory are rendered, 
#                   otherwise the scene directory is determined form the main 
#                   povray ini file (usually /usr/local/share/povray-X.Y/scenes,
#                   where X.Y represents the first two fields of the version
#                   number, e.g. for v3.8.1 this would be 3.8).
# html_file:        if specified a HTML file with links to the rendered 
#                   images is written.  If Imagemagick 'convert' is installed
#                   thumbnails for the images are generated as well.
# for the format of the render options in the scene file see render_scene.sh
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

OPTIONS="$1 $2 $3 $4 $5 $6"

case "$OPTIONS" in
  *log* | *LOG* | *Log* )
    DATE=`date`
    LOG_FILE="log.txt"
    echo "log file for POV-Ray v$VERSION sample scene render $DATE" > "$LOG_FILE"
    ;;
  *all* | *ALL* | *All* )
    RENDER_ALL=--all
    ;;
esac

test "$1" = "-d" && SCENE_DIR="$2"
test "$2" = "-d" && SCENE_DIR="$3"
test "$3" = "-d" && SCENE_DIR="$4"
test "$4" = "-d" && SCENE_DIR="$5" 
test "$5" = "-d" && SCENE_DIR="$6"

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
    SCENE_DIR="$INSTALL_DIR/share/$VER_DIR/scenes"
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

test "$1" = "-o" && OUTPUT_DIR="$2"
test "$2" = "-o" && OUTPUT_DIR="$3"
test "$3" = "-o" && OUTPUT_DIR="$4"
test "$4" = "-o" && OUTPUT_DIR="$5" 
test "$5" = "-o" && OUTPUT_DIR="$6"

if [ -z "$OUTPUT_DIR" ] ; then
  if [ ! -w "$SCENE_DIR" ] ; then
    OUTPUT_DIR=.
  fi
fi

if [ ! -d "$OUTPUT_DIR" ] ; then
  mkdir -p "$OUTPUT_DIR"
fi

SCRIPT_DIR=`dirname "$0"`

if [ -z "$RENDER_ALL" ] ; then
  FILE_LIST=`find "$SCENE_DIR" -not -path "*animations*" -not -path "*portfolio*" -name "*.pov" | sort -r |  xargs grep -l -E '^//[ ]+[-+]{1}[^ -]'`
else
  FILE_LIST=`find "$SCENE_DIR" -not -path "*animations*" -not -path "*portfolio*" -name "*.pov" | sort -r`
fi

if [ -z "$LOG_FILE" ] ; then
  echo "$FILE_LIST" | xargs -n 1 "$SCRIPT_DIR/render_scene.sh" "$OUTPUT_DIR" $RENDER_ALL
else
  echo "$FILE_LIST" | xargs -n 1 "$SCRIPT_DIR/render_scene.sh" "$OUTPUT_DIR" $RENDER_ALL 2>&1 | tee -a "$LOG_FILE"
fi

# -- HTML file --

HTML_FILE=

test "$1" = "-h" && HTML_FILE="$2"
test "$2" = "-h" && HTML_FILE="$3"
test "$3" = "-h" && HTML_FILE="$4"
test "$4" = "-h" && HTML_FILE="$5"
test "$5" = "-h" && HTML_FILE="$6"
test "$6" = "-h" && HTML_FILE="$7"

if [ ! -z "$HTML_FILE" ] ; then

CONVERT=`which convert`

FILE_LIST2=`echo "$FILE_LIST" | sort | xargs`

echo "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">
<html>

<head>
  <title>POV-Ray v$VERSION sample scenes - stills</title>
  <style type=\"text/css\">
  <!--
body
{
  font-family: Verdana, Arial, Helvetica, sans-serif ;
  text-align: justify ;
  background-color: #ffffff ;
}
h1 {        background-color: #fff0e8 ; border-color: #f0e0c0 ; color: #2040c0 }
h2 {        background-color: #ffe8d0 ; border-color: #f0d0c0 ; color: #2040c0 }
h3 {        background-color: #ffe4c0 ; border-color: #f0d0a0 ; color: #2040c0 }
h4 {        background-color: #ffdcc0 ; border-color: #f0d0b0 ; color: #2040c0 }
h5 {        background-color: #ffdaca ; border-color: #f0ccb0 ; color: #000000 }
.small { font-size:8pt; }
table { font-size:8pt; }
td { font-size:8pt; background-color: #f0f0ff ; }
th { background-color: #ffcc99 ; }
  -->
  </style>
</head>

<body>

<h2>POV-Ray v$VERSION sample scenes - stills</h2>

<p><em>written `date` by POV-Ray v$VERSION sample scenes render script (allscene.sh)</em></p>

<hr>

<h3>scenes from $SCENE_DIR:</h3>

<table>
  <tr>
    <th>Scene file</th>
    <th>Render</th>
  </tr>" > "$HTML_FILE"

for POV_FILE in $FILE_LIST2 ; do

  POV_NAME=`echo "$POV_FILE" | sed "s?$SCENE_DIR/??g"`
  FILE_BASE=`basename $POV_FILE .pov`

  echo "  <tr>
    <td><a href=\"file:${POV_FILE}\">${POV_NAME}</a></td>" >> "$HTML_FILE"
    if [ -f "$OUTPUT_DIR/$FILE_BASE.png" ] ; then
      if [ -z "$CONVERT" ] ; then
        echo "    <td><a href=\"file:$OUTPUT_DIR/$FILE_BASE.png\">$FILE_BASE.png</a></td>" >> "$HTML_FILE"
      else
        convert -sample 80x60 "$OUTPUT_DIR/$FILE_BASE.png" "$OUTPUT_DIR/${FILE_BASE}_a.png"
        echo "    <td><a href=\"file:$OUTPUT_DIR/$FILE_BASE.png\"><img src=\"file:$OUTPUT_DIR/${FILE_BASE}_a.png\" alt=\"$FILE_BASE.png\"></a></td>" >> "$HTML_FILE"
      fi
    else
      PNG_LIST=`find "$OUTPUT_DIR" -regex "${OUTPUT_DIR}/${FILE_BASE}[0123456789]+.png" | sort | xargs`
      PNG_LINKS=
      CNT=1
      for PNG_FILE in $PNG_LIST ; do
        PNG_BASE=`basename $PNG_FILE`
        PNG_LINKS="$PNG_LINKS <a href=\"file:$PNG_FILE\">[${CNT}]</a>"
        CNT=`expr $CNT + 1`
      done
      if [ -z "$PNG_LINKS" ] ; then
        echo "    <td>--</td>" >> "$HTML_FILE"
      else
        echo "    <td>$PNG_LINKS</td>" >> "$HTML_FILE"
      fi
    fi
  echo "  </tr>" >> "$HTML_FILE"

done

echo "</table>
</body>
</html>" >> "$HTML_FILE"

fi

test -f "FILEIO.TXT" && rm -f "FILEIO.TXT"
