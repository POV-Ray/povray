#!/bin/sh
# Build the WebAssembly version of POV-Ray and assemble the web site.
#
# Requires the Emscripten SDK (https://emscripten.org/docs/getting_started/):
#
#   source /path/to/emsdk/emsdk_env.sh
#   web/build.sh
#
# The site is written to build-web/site/. Serve it with web/tools/serve.py
# (or any web server that sends the COOP/COEP headers; see web/README.md).
# Extra arguments are passed on to CMake, e.g. -DPOVWEB_BUILT_BY="Jane Doe".

set -e

root=$(cd "$(dirname "$0")/.." && pwd)
build="${POVWEB_BUILD_DIR:-$root/build-web}"

if ! command -v emcmake >/dev/null 2>&1; then
    echo "build.sh: emcmake not found; activate the Emscripten SDK first (source emsdk_env.sh)" >&2
    exit 1
fi

jobs=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)

emcmake cmake -S "$root/web" -B "$build" -DCMAKE_BUILD_TYPE=Release "$@"
cmake --build "$build" -j "$jobs"

echo
echo "Done. The site is in $build/site"
echo "Try it with:  python3 $root/web/tools/serve.py $build/site"
