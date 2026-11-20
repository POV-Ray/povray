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
  reports in its version information. The default is "unknown".
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

The page footer says that the site is an unofficial development build,
shows POV-Ray's version string once POV-Ray has loaded, and links to the
licence and to the source code at the git commit the site was built from.
The source link points to `https://github.com/POV-Ray/povray` by default;
when deploying a build from another repository, pass
`-DPOVWEB_SOURCE_URL=https://github.com/<owner>/<repository>` to
`web/build.sh`. If the working tree had uncommitted or untracked files, the
footer says "modified". If the source tree is not a git checkout (for
example, a source archive), the link goes to the repository itself and the
footer says "unknown commit".

Static hosts cache each file separately, so right after a deployment a
browser could combine a new `povray.js` with a cached `povray.wasm`. To
prevent that, the build stamps a hash of the site into `index.html`
(`web/tools/stamp_version.py`), and the page appends it to the URLs of its
code and data files. Example scenes are not versioned.

Testing
-------

`web/tools/sweep.py` renders every example scene from `distribution/scenes`
at 80x60 in headless Chromium, using the same render worker as the page, and
fails if a scene does not render. It serves the built site in
`build-web/site/` itself, so build first. It needs Playwright, installed
for the same Python:

    python3 -m pip install playwright
    python3 -m playwright install chromium
    python3 web/tools/sweep.py                    # all scenes
    python3 web/tools/sweep.py --filter '^advanced/'

`web/tools/sweep-expected.txt` lists the scenes that the sweep skips,
expects to fail, or renders with their INI file; the file's header
describes the format. The sweep also fails if a scene listed as failing
renders successfully, so the list stays current. In addition, the sweep
renders the first three frames of `distribution/scenes/animations/clockd`
and checks that the render writes an image for each frame.

The workflow `.github/workflows/web_build.yml` runs on every push and pull
request that changes files outside `doc/` and `source-doc/`. It builds the
site, uploads it as the `povray-web-site` artifact, and runs the sweep.

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
| `tools/sweep.py`, `tools/sweep-expected.txt` | Renders all example scenes in headless Chromium; see Testing. |
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
  which the worker polls and forwards to the page. Frames of an animation
  that have the same size share one framebuffer, and each frame paints over
  the previous one unless "Clear frames" in the toolbar is checked.

Render worker protocol
----------------------

Other pages can render with POV-Ray by starting `render-worker.js` as a web
worker and exchanging the messages below with it. The worker loads
`povray.js`, `povray.wasm` and `povray.data` from its own directory, so it
needs the built site beside it, served from the same origin as the page.
The page must be cross-origin isolated (see Running).

    const worker = new Worker('render-worker.js');
    worker.onmessage = (e) => console.log(e.data);
    worker.postMessage({ type: 'render', cwd: '/work',
                         files: [{ path: '/work/scene.pov', data: 'sphere { 0, 1 pigment { rgb 1 } }' }],
                         args: ['+Iscene.pov', '+O/out/', '+W320', '+H240'] });

Each message is an object with a `type` field. File paths are absolute
paths in POV-Ray's virtual file system. The standard include files are in
`/povray/include`.

Messages to the worker:

| `type` | Fields | Meaning |
| --- | --- | --- |
| `init` | | Load POV-Ray without rendering. Optional; `render` also loads it. |
| `render` | `args`, `cwd`, `files`, `lazyFiles`, `clearBetweenFrames` | Start a render. |
| `cancel` | | Stop the current render. |

The fields of `render`:

- `args`: POV-Ray command-line options, one string per option. They apply
  after the settings in `/povray/etc/povray.ini` (from `web/etc/`), which
  turn on the display, and so `preview` messages, and select PNG output.
  Pass `+O/out/` to get the rendered image back in `done`.
- `cwd`: the directory to render in; relative paths in `args` and in the
  scene resolve against it. It is created if it does not exist. The
  directories `/out` and `/work` are emptied before the render's `files`
  are written.
- `files` (optional): `{path, data}` objects, written before the render and
  deleted after it. `data` is a string or an `ArrayBuffer`.
- `lazyFiles` (optional): `{path, url}` objects. Each file is downloaded from
  `url` (an absolute URL) when POV-Ray first reads it, and stays available to
  later renders.
- `clearBetweenFrames` (optional): if true, the preview is cleared before each
  animation frame.

The worker runs one render at a time. It sends `ready` once, after the
first `init` or `render`. A render that starts sends `started`, then any
number of `log`, `status`, `state`, `frame` and `preview` messages, then
`done`. If POV-Ray crashes, `fatal` is sent in place of `done`.

Messages from the worker:

| `type` | Fields | Meaning |
| --- | --- | --- |
| `ready` | `protocol`, `povrayVersion` | POV-Ray has loaded. `protocol` is the version of this protocol (currently 1); `povrayVersion` is POV-Ray's version string. |
| `started` | | The render has started. |
| `log` | `stream`, `text` | One line of POV-Ray's output; `stream` is `stdout` or `stderr`. |
| `status` | `text` | POV-Ray's current status line, e.g. render progress. |
| `state` | `state` | `parsing` or `rendering`. |
| `frame` | `current`, `total` | An animation frame has started. `current` counts from 1 up to `total`, the number of frames being rendered. |
| `preview` | `width`, `height`, `pixels` | The preview image: an `ArrayBuffer` of RGBA bytes, top row first. Sent while the image changes, at most once every 100 ms. |
| `done` | `code`, `outputs`, `memory`, `refused` | The render has ended. `code` is 0 on success, 1 on error and 2 if cancelled. `outputs` lists `{name, data}` objects (`data` is an `ArrayBuffer`): the files in `/out`, then the files the render wrote in `cwd`. `memory` is the size of the WebAssembly memory in bytes. `refused` is true if the render did not start, because another render was running or its files could not be written; in that case the running render is not affected. |
| `fatal` | `message` | POV-Ray crashed or could not load. Terminate the worker and start a new one. |

Limitations
-----------

- TIFF and OpenEXR images are not supported (the libraries are not bundled).
- Shell-outs (`Pre_Scene_Command` etc.) are not available.
- Rendering is slower than a native build.
- Memory is limited to 4 GB.
