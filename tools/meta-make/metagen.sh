#!/bin/bash

# $1 - full output file name
# $2 - info
# $3 - generator tool command
# $4 - generator tool parameters
# $5 - include file extra parameters

echo $1

filename="${1#../../source/}"
info="$2"
generator="${3##* }"
generator="${generator##*/}"

if [ "${filename##*.}" == "h" ]; then
  opts="$5"
else
  opts=""
fi

sed "metagen-header.cpp" \
    -e 's|{{filename}}|'"$filename"'|g' \
    -e 's|{{info}}|'"$info"'|g' \
    -e 's|{{generator}}|'"$generator"'|g' \
    -e 's|{{year}}|'`date +"%Y"`'|g' > "$1" || exit 2

$3 $opts $4 >> "$1" && exit 0

rm "$1"
exit 2
