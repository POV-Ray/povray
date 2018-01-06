#!/bin/sh
# ==============================================================================
# POV-Ray v3.8
# render_scene.sh - render a scene with options given in a scene file comment
# ==============================================================================
# written November 2003 by Christoph Hormann
# This file is part of POV-Ray and subject to the POV-Ray licence
# see POVLEGAL.DOC for details
# ------------------------------------------------------------------------------
# calling conventions:
#
#   render_scene.sh [output_directory] [--all] scene_file
#
# output_directory: directory where to write the rendered image 
# scene_file: scene to render
#
# the render options have to be given as a '//' comment within the first
# 50 lines of the file.  There must be a space after the '//' and the first 
# option has to start with '-' or '+'.  If option '--all' is given scenes
# with no options given are rendered, otherwise not.
# ==============================================================================

# --- specify additional render options here ---
POV_OPTIONS=""

if [ ! -z "$1" ] ; then

  if [ ! -z "$2" ] ; then
    case $1 in
      --all )
        RENDER_ALL=--all
        if [ ! -z "$3" ] ; then
          OUTPUT_DIR="$2"
          RENDER_NAME="$3"
        else
          OUTPUT_DIR=`dirname $2`
          RENDER_NAME="$2"
        fi
        ;;
      *)
        OUTPUT_DIR="$1"
        case $2 in
          --all )
            RENDER_ALL=--all
            RENDER_NAME="$3"
            ;;
          *)
            RENDER_NAME="$2"
            ;;
        esac        
        ;;
    esac
  else
    OUTPUT_DIR=`dirname $1`
    RENDER_NAME="$1"
  fi

  head -n 50 "$RENDER_NAME" | grep -E '^//[ ]+[-+]{1}[^ -]' > /dev/null && POV_FILE="$RENDER_NAME"

  SCENE_DIR=`dirname $RENDER_NAME`
  FILE_BASE=`basename $RENDER_NAME .pov`

  if [ ! -z "$POV_FILE" ] ; then
    # -- use first option line --
    OPTIONS=`head -n 50 "$POV_FILE" | grep -E '^//[ ]+[-+]{1}[^ -]' | head -n 1 | sed "s?^//[ ]*??"`
    # -- use last option line --
    #OPTIONS=`head -n 50 "$POV_FILE" | grep -E '^//[ ]+[-+]{1}[^ -]' | tail -n 1 | sed "s?^//[ ]*??"`

    echo "==========================================================================="
    echo "$POV_FILE: $OPTIONS"
    echo "==========================================================================="
    
    povray +L$SCENE_DIR +L$OUTPUT_DIR -i$POV_FILE -o$OUTPUT_DIR/ $OPTIONS $POV_OPTIONS -p
    #echo "povray +L$SCENE_DIR +L$OUTPUT_DIR -i$POV_FILE -o$OUTPUT_DIR/ $OPTIONS $POV_OPTIONS -p"

  else
    if [ ! -z "$RENDER_ALL" ] ; then

      echo "==========================================================================="
      echo "$RENDER_NAME"
      echo "==========================================================================="

      echo "########################"
      povray +L$SCENE_DIR +L$OUTPUT_DIR -i$POV_FILE -o$OUTPUT_DIR/ $POV_OPTIONS -p
      #echo "povray +L$SCENE_DIR +L$OUTPUT_DIR -i$RENDER_NAME -o$OUTPUT_DIR/ $POV_OPTIONS -p"

    fi
  fi

fi
