#!/bin/bash

# =============================================================================

# Print header for single C++ source file.
# $1 - output file name
# $2 - input file name
function reswrap_header {
    sed "tools/meta-make/reswrap-header.cpp" \
      -e 's|{{filename}}|'"${1#source/}"'|g' \
      -e 's|{{source}}|'"${2#source/}"'|g'
}

# Create single C++ source file.
# $1 - output file name
# $2 - input file name
# $3 - C++ namespace
# $4 - C++ identifier
# $5 - additional reswrap options
function reswrap1 {
    reswrap_header "$1" "$2" > "$1"
    reswrap $5 -oa "$1" -n "$3" -e -s -r "$4" "$2"
}

# Create C++ source file pair (`.cpp`, `.h`).
# $1 - output base file name
# $2 - input file name
# $3 - C++ namespace
# $4 - C++ identifier
# $5 - additional reswrap options
function reswrap2 {
    reswrap1 "$1.cpp" "$2" "$3" "$4" "$5"
    reswrap1 "$1.h"   "$2" "$3" "$4" "$5 -i"
}

# Create C++ source file pair for a binary file.
# $1 - output base file name
# $2 - input file name
# $3 - C++ namespace
# $4 - C++ identifier
function reswrap_binary {
    reswrap2 "$@" "-z"
}

# Create C++ source file pair for a text file.
# $1 - output base file name
# $2 - input file name
# $3 - C++ namespace
# $4 - C++ identifier
function reswrap_text {
    reswrap2 "$@" "-m -ta -c 120"
}

# Create C++ source file pair for a TTF font file.
# $1 - font base file name
function reswrap_ttf {
    reswrap_binary "source/base/font/$1" "distribution/include/$1.ttf" "pov_base" "font_$1"
}

# Create C++ source file pair for a benchmark-related file.
# $1 - file type ("pov" or "ini")
# $2 - C++ identifier
function reswrap_benchmark {
    reswrap_text "source/backend/control/benchmark_$1" "distribution/scenes/advanced/benchmark/benchmark.$1" "pov" "$2"
}

# =============================================================================

# TrueType Fonts

reswrap_ttf "crystal"
reswrap_ttf "cyrvetic"
reswrap_ttf "povlogo"
reswrap_ttf "timrom"

# Benchmark Scene

reswrap_benchmark "pov" "Benchmark_File"
reswrap_benchmark "ini" "Benchmark_Options"
