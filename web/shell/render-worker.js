// POV-Ray render worker.
//
// Hosts one long-lived instance of the Emscripten module (`povray.js` /
// `povray.wasm`) and runs any number of render jobs on it. POV-Ray's `main()`
// runs on a pthread (PROXY_TO_PTHREAD) and waits for jobs (see
// web/src/webconsole.cpp), so this worker's own event loop stays free to
// service file-system calls and to poll the preview framebuffer.
//
// The instance is reused because creating a fresh one per render leaks: each
// instance reserves a large shared WebAssembly memory, which browsers do not
// reliably reclaim when the worker is terminated.
//
// Emscripten starts its pthread workers from this same script (it uses
// `self.location` as the script URL), so a worker named "em-pthread" only
// needs to load the module; `povray.js` bootstraps itself in that case.

'use strict';

// The page passes the build version as a query string (see app.js); pass it
// on to the files loaded from here.
const VERSION_QUERY = self.location.search;

importScripts('povray.js' + VERSION_QUERY);

if (self.name !== 'em-pthread') {
  const JOB_ARGS_FILE = '/povweb/job.args';  // see webconsole.cpp
  const OUT_DIR = '/out';
  const LOCAL_ROOT = '/work';

  let povray = null;          // Emscripten module instance
  let startup = null;         // promise resolved once POV-Ray is ready for jobs
  let job = null;             // current job: {cwd, written, finished}
  let previewTimer = 0;
  let lastSerial = -1;
  const lazyFiles = new Map(); // path -> url of example files mounted lazily

  const post = (msg, transfer) => self.postMessage(msg, transfer || []);

  self.onmessage = (event) => {
    const msg = event.data;
    if (msg.type === 'init') {
      start().catch(reportFatal);
    } else if (msg.type === 'render') {
      start().then(() => runJob(msg)).catch(reportFatal);
    } else if (msg.type === 'cancel') {
      if (povray && job) povray._povweb_cancel();
    }
  };

  function reportFatal(err) {
    post({ type: 'fatal', message: String((err && err.stack) || err) });
  }

  // Lines prefixed with \x01 are structured events from webconsole.cpp.
  function handleStdout(line) {
    if (line.charCodeAt(0) === 1) {
      const space = line.indexOf(' ');
      const kind = line.slice(1, space < 0 ? undefined : space);
      const rest = space < 0 ? '' : line.slice(space + 1);
      if (kind === 'STATUS') post({ type: 'status', text: rest });
      else if (kind === 'STATE') post({ type: 'state', state: rest });
      else if (kind === 'FRAME') {
        const [current, total] = rest.split(' ').map(Number);
        post({ type: 'frame', current, total });
      }
      return;
    }
    post({ type: 'log', stream: 'stdout', text: line });
  }

  function start() {
    if (startup) return startup;
    const base = new URL('.', self.location.href).href;
    startup = new Promise((resolve, reject) => {
      createPovray({
        print: handleStdout,
        printErr: (text) => post({ type: 'log', stream: 'stderr', text }),
        locateFile: (path) => base + path + VERSION_QUERY,
        povwebReady: (ok) => (ok ? resolve() : reject(new Error('POV-Ray failed to initialize'))),
        povwebJobDone: (code) => finishJob(code),
        onAbort: (what) => reject(new Error('POV-Ray aborted: ' + what)),
        preRun: [(Module) => {
          const FS = Module.FS;
          // An (empty) user configuration file keeps POV-Ray from warning about it.
          writeFile(FS, '/home/web_user/.povray/3.8/povray.conf', '');
          mkdirTree(FS, OUT_DIR);
          mkdirTree(FS, LOCAL_ROOT);
          mkdirTree(FS, JOB_ARGS_FILE.slice(0, JOB_ARGS_FILE.lastIndexOf('/')));
        }],
      }).then((instance) => {
        povray = instance;
        // Starts the job loop on its own thread; returns immediately.
        povray.callMain([]);
      }, reject);
    });
    startup.then(() => post({ type: 'ready' }), () => {});
    return startup;
  }

  // ---------------------------------------------------------------------------
  // File system helpers
  // ---------------------------------------------------------------------------

  function mkdirTree(FS, path) {
    let cur = '';
    for (const part of path.split('/')) {
      if (!part) continue;
      cur += '/' + part;
      try { FS.mkdir(cur); } catch (e) { /* exists */ }
    }
  }

  function writeFile(FS, path, data) {
    mkdirTree(FS, path.slice(0, path.lastIndexOf('/')) || '/');
    try { FS.unlink(path); } catch (e) { /* did not exist (or was a lazy file) */ }
    FS.writeFile(path, typeof data === 'string' ? data : new Uint8Array(data));
  }

  function mountLazy(FS, path, url) {
    const slash = path.lastIndexOf('/');
    const dir = path.slice(0, slash);
    mkdirTree(FS, dir);
    FS.createLazyFile(dir, path.slice(slash + 1), url, true, false);
  }

  /** Delete the contents of a directory that holds no lazy files. */
  function clearDir(FS, dir) {
    let names = [];
    try { names = FS.readdir(dir); } catch (e) { return; }
    for (const name of names) {
      if (name === '.' || name === '..') continue;
      const path = dir + '/' + name;
      if (FS.isDir(FS.lstat(path).mode)) {
        clearDir(FS, path);
        FS.rmdir(path);
      } else {
        FS.unlink(path);
      }
    }
  }

  // Gather regular files under `dir` (recursively), skipping the inputs.
  function collectFiles(FS, dir, prefix, out) {
    let names = [];
    try { names = FS.readdir(dir); } catch (e) { return out; }
    for (const name of names.sort()) {
      // Skip POV-Ray's internal continue-trace state file.
      if (name === '.' || name === '..' || name.endsWith('.pov-state')) continue;
      const path = dir + '/' + name;
      // Never stat inputs: stat'ing a lazy file would download it.
      if (lazyFiles.has(path) || job.written.has(path)) continue;
      const mode = FS.stat(path).mode;
      if (FS.isDir(mode)) {
        collectFiles(FS, path, prefix + name + '/', out);
      } else if (FS.isFile(mode)) {
        out.push({ name: prefix + name, path, data: FS.readFile(path).buffer });
      }
    }
    return out;
  }

  // ---------------------------------------------------------------------------
  // Jobs
  // ---------------------------------------------------------------------------

  function runJob(msg) {
    if (job) {
      post({ type: 'log', stream: 'stderr', text: 'A render is already running.' });
      return;
    }
    const FS = povray.FS;
    FS.chdir('/');

    // Example scenes, fetched on demand when POV-Ray opens them.
    for (const file of msg.lazyFiles || []) {
      if (lazyFiles.has(file.path)) continue;
      try { mountLazy(FS, file.path, file.url); } catch (e) { continue; }
      lazyFiles.set(file.path, file.url);
    }

    clearDir(FS, OUT_DIR);
    clearDir(FS, LOCAL_ROOT);

    job = { cwd: msg.cwd, written: new Set(), finished: false };

    // Files of the current project (edited scene text, user-supplied assets).
    for (const file of msg.files || []) {
      writeFile(FS, file.path, file.data);
      job.written.add(file.path);
    }

    mkdirTree(FS, msg.cwd);
    FS.chdir(msg.cwd);

    FS.writeFile(JOB_ARGS_FILE, msg.args.join('\n') + '\n');
    lastSerial = -1;
    post({ type: 'started' });
    previewTimer = setInterval(pollPreview, 100);
    povray._povweb_submit();
  }

  function finishJob(code) {
    if (!job || job.finished) return;
    job.finished = true;
    clearInterval(previewTimer);
    pollPreview(true);

    const FS = povray.FS;
    const outputs = [];
    try {
      // Rendered images, then any files the scene itself wrote (#fopen, etc.).
      collectFiles(FS, OUT_DIR, '', outputs);
      collectFiles(FS, job.cwd, '', outputs);
    } catch (e) {
      post({ type: 'log', stream: 'stderr', text: 'Could not read output files: ' + e });
    }

    // Put the file system back the way it was for the next job.
    try {
      FS.chdir('/');
      for (const o of outputs) {
        try { FS.unlink(o.path); } catch (e) { /* already gone */ }
      }
      for (const path of job.written) {
        try { FS.unlink(path); } catch (e) { /* already gone */ }
        // Restore example files that the job replaced with edited versions.
        if (lazyFiles.has(path)) mountLazy(FS, path, lazyFiles.get(path));
      }
    } catch (e) {
      post({ type: 'log', stream: 'stderr', text: 'Could not clean up after the render: ' + e });
    }
    job = null;

    const files = outputs.map((o) => ({ name: o.name, data: o.data }));
    const memory = povray.wasmMemory.buffer.byteLength;  // for diagnostics
    post({ type: 'done', code, outputs: files, memory }, files.map((o) => o.data));
  }

  function pollPreview(force) {
    if (!povray) return;
    const serial = povray._povweb_frame_serial();
    if (!force && serial === lastSerial) return;
    const ptr = povray._povweb_framebuffer();
    const width = povray._povweb_width();
    const height = povray._povweb_height();
    if (!ptr || !width || !height) return;
    lastSerial = serial;
    // Copy out of the shared heap; ImageData cannot wrap shared memory.
    const pixels = new Uint8ClampedArray(povray.wasmMemory.buffer, ptr, width * height * 4).slice();
    post({ type: 'preview', width, height, pixels: pixels.buffer }, [pixels.buffer]);
  }
}
