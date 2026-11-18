POV-Ray for the Web
===================

This directory contains a WebAssembly build of POV-Ray and a small browser
front-end for it. POV-Ray itself is compiled unchanged from `source/`, using
the Unix platform layer (`platform/unix`, `vfe/unix`) and the libraries
bundled in `libraries/` (zlib, libpng, libjpeg).

Features:

- Edit scenes in the browser, render them with all CPU cores, and watch a
  live preview while rendering.
- Open scene files from local disk: one or more files, a whole folder, or by
  drag and drop. Include files, image maps, fonts, etc. opened alongside the
  scene are available to it.
- Save scenes back to disk (in place, where the browser supports the File
  System Access API), save the rendered image, and download any other files
  the scene writes (animation frames, `#fopen` output).
- Browse and render all example scenes from `distribution/scenes`. Files that
  an example needs from its directory are downloaded only when POV-Ray opens
  them.

Building
--------

The build needs the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
(the CI workflow uses 6.0.12), CMake 3.16 or later, and Python 3. Activate
the SDK, then run the build script from anywhere:

    source /path/to/emsdk/emsdk_env.sh
    web/build.sh

The site is assembled in `build-web/site/`. Extra arguments are passed to
CMake as options:

- `-DPOVWEB_BUILT_BY="Your Name"` sets the "compiled by" name that POV-Ray
  reports in its version information.
- `-DPOVWEB_SOURCE_URL=...` sets the repository that the page footer links
  to (see Deploying).

`build.sh` runs these two commands, which also work on their own once the
SDK is active:

    emcmake cmake -S web -B build-web -DCMAKE_BUILD_TYPE=Release
    cmake --build build-web -j

Running
-------

POV-Ray uses threads. In the browser, threads need `SharedArrayBuffer`, which
is available only when the page is *cross-origin isolated*, that is, served
with:

    Cross-Origin-Opener-Policy: same-origin
    Cross-Origin-Embedder-Policy: require-corp

`web/tools/serve.py` serves `build-web/site/` with these headers. It takes
another directory as an argument, and `--port` to change the port:

    python3 web/tools/serve.py          # http://127.0.0.1:8000/

On hosts that cannot set headers (e.g. GitHub Pages), the page registers
`coi-serviceworker.js` (part of the site, from `shell/`), which adds them and
reloads the page once. This requires HTTPS (or `localhost`).

Deploying
---------

The site in `build-web/site/` is static and can be served by any web
server that sends the headers above, or over HTTPS by one that cannot (see
Running).

The page footer names the POV-Ray version and links to the licence and to
the source code at the git commit the site was built from. The source link
points to `https://github.com/POV-Ray/povray` by default; when deploying a
build from another repository, pass
`-DPOVWEB_SOURCE_URL=https://github.com/<owner>/<repository>` to
`web/build.sh`. If the working tree had uncommitted changes, the footer says
so.

Static hosts cache each file separately, so right after a deployment a
browser could combine a new `povray.js` with a cached `povray.wasm`. To
prevent that, the build stamps a hash of the site into `index.html`
(`web/tools/stamp_version.py`), and the page appends it to the URLs of its
code and data files. Example scenes are not versioned.

How it works
------------

Paths in this table are relative to `web/`.

| Path | Purpose |
| --- | --- |
| `CMakeLists.txt` | Emscripten build of POV-Ray and the bundled libraries; assembles the site. |
| `config/config.h` | Replaces the autoconf-generated `config.h` of the Unix build. |
| `etc/` | `povray.ini` / `povray.conf`, packaged into the virtual file system at `/povray/etc`. |
| `src/webconsole.cpp` | Entry point; takes the place of `vfe/unix/unixconsole.cpp`. |
| `shell/` | The web page (`index.html`, `app.js`, `style.css`), the render worker (`render-worker.js`) and `coi-serviceworker.js`. |
| `tools/make_manifest.py` | Copies the example scenes into the site and writes `scenes.json`. |
| `tools/stamp_version.py` | Writes `index.html` with the build version stamped into asset URLs and the footer filled in. |
| `tools/serve.py` | Local web server with the required headers. |

- The page starts one long-lived worker (`render-worker.js`) that hosts one
  instance of the WebAssembly module. POV-Ray's `main()` runs on a pthread
  (`-sPROXY_TO_PTHREAD`) and waits in a loop for render jobs. For each
  render, the worker passes it the command-line arguments from the page's
  options and the scene files, and gets the output files back when the
  render ends.
- The worker reuses one instance for all renders: browsers do not reliably
  reclaim the large shared memory that each instance reserves, so creating
  one per render eventually fails with "could not allocate memory". The
  worker is replaced only if POV-Ray crashes or a render does not stop when
  asked.
- Standard include files and INI files are preloaded into the virtual file
  system from a data package (`povray.data`). Example scenes are mounted as
  lazy files under `/povray/scenes`, which are fetched when first read;
  files opened from disk are written to `/work`. Render output goes to
  `/out`.
- The preview is drawn from an RGBA framebuffer in shared WebAssembly memory,
  which the worker polls and forwards to the page.

Limitations
-----------

- TIFF and OpenEXR images are not supported (the libraries are not bundled).
- Shell-outs (`Pre_Scene_Command` etc.) are not available.
- Rendering is slower than a native build.
- Memory is limited to 4 GB.
