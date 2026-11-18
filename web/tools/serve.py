#!/usr/bin/env python3
"""Serve the POV-Ray web build locally.

Sends the Cross-Origin-Opener-Policy / Cross-Origin-Embedder-Policy headers
that browsers require before they allow SharedArrayBuffer (and therefore
threads) in WebAssembly.

Usage:  python3 web/tools/serve.py [--port 8000] [directory]

The directory defaults to `build-web/site` in the repository root.
"""

import argparse
import functools
import http.server
import os
import sys


class Handler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {
        **http.server.SimpleHTTPRequestHandler.extensions_map,
        ".wasm": "application/wasm",
        ".js": "text/javascript",
        ".data": "application/octet-stream",
        ".pov": "text/plain",
        ".inc": "text/plain",
        ".ini": "text/plain",
    }

    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        self.send_header("Cross-Origin-Resource-Policy", "same-origin")
        self.send_header("Cache-Control", "no-cache")
        super().end_headers()


def main():
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("directory", nargs="?", default=os.path.join(repo_root, "build-web", "site"))
    parser.add_argument("--port", type=int, default=8000)
    parser.add_argument("--bind", default="127.0.0.1")
    args = parser.parse_args()

    if not os.path.isfile(os.path.join(args.directory, "povray.wasm")):
        print(f"serve.py: {args.directory} does not contain a build; run web/build.sh first", file=sys.stderr)
        sys.exit(1)

    handler = functools.partial(Handler, directory=args.directory)
    with http.server.ThreadingHTTPServer((args.bind, args.port), handler) as httpd:
        print(f"Serving {args.directory} at http://{args.bind}:{args.port}/", file=sys.stderr)
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            pass


if __name__ == "__main__":
    main()
