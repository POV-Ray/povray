#!/bin/bash

# $1 - full output file name
# $2 - full source file name
# $3 - namespace
# $4 - identifier
# $5 - additional options

echo $1

filename="${1#../../source/}"
source="${2#../../}"

if [ "${filename##*.}" == "h" ]; then
  opts="-i"
else
  opts=""
fi

sed "reswrap-header.cpp" \
    -e 's|{{filename}}|'"$filename"'|g' \
    -e 's|{{source}}|'"$source"'|g' \
    -e 's|{{year}}|'`date +"%Y"`'|g' > "$1" || exit 2

reswrap $opts $5 -oa "$1" -n "$3" -e -s -r "$4" "$2" && exit 0

rm "$1"
exit 2
