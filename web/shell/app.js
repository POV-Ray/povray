// POV-Ray Web front-end.
//
// Manages the scene "project" (one or more files edited in the browser),
// loading/saving files from local disk, the example scene browser, and
// running renders in a dedicated worker (render-worker.js).

'use strict';

(() => {
  const $ = (id) => document.getElementById(id);

  // ---------------------------------------------------------------------------
  // Constants
  // ---------------------------------------------------------------------------

  const TEXT_EXTENSIONS = new Set([
    'pov', 'inc', 'ini', 'mcr', 'mac', 'txt', 'csv', 'par', 'log',
  ]);
  const RENDERABLE_EXTENSIONS = new Set(['pov', 'ini']);
  const SCENES_ROOT = '/povray/scenes';
  const LOCAL_ROOT = '/work';
  const STORAGE_KEY = 'povray-web-state-v1';
  const MAX_LOG_LINES = 5000;
  // Build version, appended to the URLs of versioned files so that a browser
  // does not combine them from different deployments (see web/tools/stamp_version.py).
  const VERSION_META = document.querySelector('meta[name="povweb-version"]');
  const VERSION_QUERY = VERSION_META && !VERSION_META.content.startsWith('@')
    ? '?v=' + encodeURIComponent(VERSION_META.content) : '';

  const NEW_SCENE = `// Persistence of Vision Ray Tracer scene description file
#version 3.8;
global_settings { assumed_gamma 1.0 }

#include "colors.inc"
#include "textures.inc"

camera {
  location <0, 2, -5>
  look_at  <0, 0.75, 0>
  angle 45
}

light_source { <-5, 10, -10> color White }
light_source { <8, 6, -4> color rgb 0.4 shadowless }

sky_sphere {
  pigment { gradient y color_map { [0 color rgb <0.9, 0.95, 1>] [1 color rgb <0.3, 0.5, 0.9>] } }
}

plane {
  y, 0
  pigment { checker color rgb 0.9 color rgb 0.2 scale 0.5 }
  finish { reflection 0.1 }
}

sphere {
  <0, 1, 0>, 1
  texture {
    pigment { color rgb <1, 0.35, 0.1> }
    finish { phong 0.8 phong_size 60 reflection 0.15 }
  }
}
`;

  // ---------------------------------------------------------------------------
  // State
  // ---------------------------------------------------------------------------

  /**
   * A project is the set of files a render can see.
   *   kind:   'local'   - files opened from disk or created here, rendered in /work
   *           'example' - an example scene; its directory is available lazily
   *   files:  Map<name, {name, text?, data?, handle?, dirty}>
   */
  let project = null;
  let activeName = null;      // file shown in the editor
  let mainName = null;        // file rendered by "Render"
  let manifest = null;        // scenes.json
  let manifestPromise = null;

  let job = null;             // current render
  let outputs = [];           // [{name, blob, url}]
  let selectedOutput = -1;
  let hasPreview = false;     // canvas holds a (possibly partial) render

  // ---------------------------------------------------------------------------
  // Utilities
  // ---------------------------------------------------------------------------

  const ext = (name) => {
    const i = name.lastIndexOf('.');
    return i < 0 ? '' : name.slice(i + 1).toLowerCase();
  };
  const stem = (name) => {
    const base = name.slice(name.lastIndexOf('/') + 1);
    const i = base.lastIndexOf('.');
    return i <= 0 ? base : base.slice(0, i);
  };
  const isText = (name) => TEXT_EXTENSIONS.has(ext(name));
  const isRenderable = (name) => RENDERABLE_EXTENSIONS.has(ext(name));

  function mimeFor(name) {
    switch (ext(name)) {
      case 'png': return 'image/png';
      case 'jpg': case 'jpeg': return 'image/jpeg';
      case 'gif': return 'image/gif';
      case 'bmp': return 'image/bmp';
      case 'exr': return 'image/x-exr';
      case 'hdr': return 'image/vnd.radiance';
      case 'tga': return 'image/x-tga';
      case 'ppm': return 'image/x-portable-pixmap';
      case 'pgm': return 'image/x-portable-graymap';
      default: return isText(name) ? 'text/plain' : 'application/octet-stream';
    }
  }

  function formatDuration(ms) {
    const s = Math.floor(ms / 1000);
    const h = Math.floor(s / 3600);
    const m = Math.floor((s % 3600) / 60);
    const sec = s % 60;
    const tenths = Math.floor((ms % 1000) / 100);
    return (h ? `${h}:${String(m).padStart(2, '0')}` : `${m}`) + `:${String(sec).padStart(2, '0')}.${tenths}`;
  }

  function splitArgs(text) {
    const args = [];
    const re = /"([^"]*)"|'([^']*)'|(\S+)/g;
    let m;
    while ((m = re.exec(text))) args.push(m[1] ?? m[2] ?? m[3]);
    return args;
  }

  function storageGet() {
    try { return JSON.parse(localStorage.getItem(STORAGE_KEY) || 'null'); } catch (e) { return null; }
  }
  // Returns the error if the value could not be stored (quota or storage disabled).
  function storageSet(value) {
    try {
      localStorage.setItem(STORAGE_KEY, JSON.stringify(value));
      return null;
    } catch (e) {
      return e;
    }
  }

  // ---------------------------------------------------------------------------
  // Message log
  // ---------------------------------------------------------------------------

  const logEl = $('log');
  let logLines = 0;
  let pendingLog = [];
  let logFlushScheduled = false;
  const ERROR_LOCATION = /File '([^']+)' line (\d+)/;

  function log(text, cls) {
    pendingLog.push([text, cls]);
    if (!logFlushScheduled) {
      logFlushScheduled = true;
      requestAnimationFrame(flushLog);
    }
  }

  function flushLog() {
    logFlushScheduled = false;
    const atBottom = logEl.scrollTop + logEl.clientHeight >= logEl.scrollHeight - 8;
    const frag = document.createDocumentFragment();
    for (const [text, cls] of pendingLog) {
      const line = document.createElement('div');
      if (cls) line.className = cls;
      const m = ERROR_LOCATION.exec(text);
      if (m) {
        const a = document.createElement('a');
        a.href = '#';
        a.textContent = text;
        a.title = 'Show this line in the editor';
        a.addEventListener('click', (e) => { e.preventDefault(); gotoLine(m[1], Number(m[2])); });
        line.appendChild(a);
        if (/error/i.test(text)) line.classList.add('error');
        else if (/warning/i.test(text)) line.classList.add('warning');
      } else {
        line.textContent = text;
      }
      frag.appendChild(line);
      logLines++;
    }
    pendingLog = [];
    logEl.appendChild(frag);
    while (logLines > MAX_LOG_LINES) {
      logEl.removeChild(logEl.firstChild);
      logLines--;
    }
    if (atBottom) logEl.scrollTop = logEl.scrollHeight;
  }

  function clearLog() {
    logEl.textContent = '';
    logLines = 0;
  }

  // ---------------------------------------------------------------------------
  // Editor
  // ---------------------------------------------------------------------------

  const editor = $('editor');
  const gutter = $('gutter');
  let gutterLines = 0;

  function updateGutter() {
    const lines = editor.value.split('\n').length;
    if (lines !== gutterLines) {
      let s = '';
      for (let i = 1; i <= lines; i++) s += i + '\n';
      gutter.textContent = s;
      gutterLines = lines;
    }
    gutter.scrollTop = editor.scrollTop;
  }

  editor.addEventListener('scroll', () => { gutter.scrollTop = editor.scrollTop; });
  editor.addEventListener('input', () => {
    const file = project && project.files.get(activeName);
    if (file && file.text !== undefined) {
      file.text = editor.value;
      if (!file.dirty) {
        file.dirty = true;
        renderTabs();
      }
      scheduleSave();
    }
    updateGutter();
  });

  // Tab indents. After Escape, the next Tab or Shift+Tab moves the focus out
  // of the editor instead. Escape in the editor does not stop a render.
  let tabMovesFocus = false;
  editor.addEventListener('blur', () => { tabMovesFocus = false; });
  editor.addEventListener('keydown', (e) => {
    if (e.key === 'Escape') {
      e.stopPropagation();
      tabMovesFocus = true;
      return;
    }
    if (e.key === 'Tab' && tabMovesFocus) {
      tabMovesFocus = false;
      return;
    }
    if (e.key !== 'Shift') tabMovesFocus = false;
    if (e.key === 'Tab' && !e.ctrlKey && !e.metaKey && !e.altKey) {
      e.preventDefault();
      const start = editor.selectionStart;
      const end = editor.selectionEnd;
      if (start === end && !e.shiftKey) {
        document.execCommand('insertText', false, '  ');
      } else {
        // Indent / outdent the selected lines.
        const value = editor.value;
        const lineStart = value.lastIndexOf('\n', start - 1) + 1;
        const block = value.slice(lineStart, end);
        const changed = e.shiftKey
          ? block.replace(/^( {1,2}|\t)/gm, '')
          : block.replace(/^/gm, '  ');
        editor.setSelectionRange(lineStart, end);
        document.execCommand('insertText', false, changed);
        editor.setSelectionRange(lineStart, lineStart + changed.length);
      }
    }
  });

  function gotoLine(fileName, line) {
    if (!project) return;
    const base = fileName.slice(fileName.lastIndexOf('/') + 1);
    let target = null;
    for (const name of project.files.keys()) {
      if (name === fileName || name.slice(name.lastIndexOf('/') + 1) === base) { target = name; break; }
    }
    if (!target) {
      log(`(${fileName} is not open in the editor)`, 'note');
      return;
    }
    activate(target);
    const lines = editor.value.split('\n');
    const n = Math.max(1, Math.min(line, lines.length));
    let start = 0;
    for (let i = 0; i < n - 1; i++) start += lines[i].length + 1;
    const end = start + lines[n - 1].length;
    editor.focus();
    editor.setSelectionRange(start, end);
    const lineHeight = parseFloat(getComputedStyle(editor).lineHeight) || 18;
    editor.scrollTop = Math.max(0, (n - 5) * lineHeight);
  }

  // ---------------------------------------------------------------------------
  // Project & tabs
  // ---------------------------------------------------------------------------

  const tabsEl = $('tabs');
  const assetsEl = $('assets');

  function setProject(p, active) {
    project = p;
    const names = [...p.files.keys()];
    const firstRenderable = names.find((n) => ext(n) === 'pov') || names.find(isRenderable);
    mainName = p.main || firstRenderable || null;
    activate(active || mainName || names.find((n) => isText(n)) || null);
    renderAssets();
    updateTitle();
    scheduleSave();
  }

  function activate(name) {
    activeName = name;
    const file = name && project.files.get(name);
    if (file && file.text !== undefined) {
      editor.value = file.text;
      editor.disabled = false;
      if (isRenderable(name)) mainName = name;
    } else {
      editor.value = '';
      editor.disabled = true;
    }
    editor.scrollTop = 0;
    gutterLines = 0;
    updateGutter();
    renderTabs();
    updateTitle();
  }

  function renderTabs() {
    tabsEl.textContent = '';
    if (!project) return;
    for (const [name, file] of project.files) {
      if (file.text === undefined) continue;
      const tab = document.createElement('div');
      tab.className = 'tab' + (name === activeName ? ' active' : '');
      tab.setAttribute('role', 'tab');
      tab.setAttribute('aria-selected', name === activeName ? 'true' : 'false');
      tab.title = name + (name === mainName ? ' (rendered by Render)' : '');

      const label = document.createElement('span');
      label.className = 'tab-label';
      label.textContent = (name === mainName ? '▶ ' : '') + name + (file.dirty ? ' •' : '');
      tab.appendChild(label);

      if (project.files.size > 1) {
        const close = document.createElement('button');
        close.className = 'tab-close';
        close.textContent = '×';
        close.title = 'Close ' + name;
        close.addEventListener('click', (e) => {
          e.stopPropagation();
          closeFile(name);
        });
        tab.appendChild(close);
      }
      tab.addEventListener('click', () => activate(name));
      tabsEl.appendChild(tab);
    }
    const add = document.createElement('button');
    add.className = 'tab-add';
    add.textContent = '+';
    add.title = 'Add files to this project (scene, include files, image maps, fonts…)';
    add.addEventListener('click', () => openFiles({ add: true }));
    tabsEl.appendChild(add);
    const active = tabsEl.querySelector('.tab.active');
    if (active) active.scrollIntoView({ block: 'nearest', inline: 'nearest' });
  }

  function renderAssets() {
    assetsEl.textContent = '';
    const binaries = project ? [...project.files.values()].filter((f) => f.text === undefined) : [];
    assetsEl.hidden = binaries.length === 0;
    if (!binaries.length) return;
    const label = document.createElement('span');
    label.className = 'assets-label';
    label.textContent = 'Assets:';
    assetsEl.appendChild(label);
    for (const file of binaries) {
      const chip = document.createElement('span');
      chip.className = 'chip';
      chip.textContent = `${file.name} (${Math.ceil(file.data.byteLength / 1024)} KB)`;
      const remove = document.createElement('button');
      remove.textContent = '×';
      remove.title = 'Remove ' + file.name;
      remove.addEventListener('click', () => closeFile(file.name));
      chip.appendChild(remove);
      assetsEl.appendChild(chip);
    }
  }

  function closeFile(name) {
    const file = project.files.get(name);
    if (file && file.dirty && !confirm(`Discard unsaved changes to ${name}?`)) return;
    project.files.delete(name);
    if (mainName === name) mainName = null;
    if (activeName === name) {
      const next = [...project.files.keys()].find((n) => project.files.get(n).text !== undefined) || null;
      activate(next);
    } else {
      renderTabs();
    }
    if (!mainName) mainName = [...project.files.keys()].find(isRenderable) || null;
    renderTabs();
    renderAssets();
    updateTitle();
    scheduleSave();
  }

  function updateTitle() {
    const name = activeName || 'POV-Ray Web';
    document.title = (project && activeName ? `${name} – ` : '') + 'POV-Ray Web';
    $('btn-render').title = mainName ? `Render ${mainName} (Ctrl+Enter)` : 'Render (Ctrl+Enter)';
    $('btn-save').disabled = !activeName || !project.files.get(activeName) || project.files.get(activeName).text === undefined;
    $('btn-save-as').disabled = $('btn-save').disabled;
    renderIniSelect();
  }

  function hasUnsavedChanges() {
    return !!project && [...project.files.values()].some((f) => f.dirty);
  }

  function newProject() {
    if (hasUnsavedChanges() && !confirm('Discard unsaved changes?')) return;
    const files = new Map();
    files.set('scene.pov', { name: 'scene.pov', text: NEW_SCENE, dirty: false });
    setProject({ kind: 'local', files });
    history.replaceState(null, '', location.pathname);
  }

  // ---------------------------------------------------------------------------
  // INI selection (example scenes with animation / batch INI files)
  // ---------------------------------------------------------------------------

  function renderIniSelect() {
    const wrap = $('ini-wrap');
    const select = $('opt-ini');
    const inis = project && project.kind === 'example' && project.example ? project.example.inis : [];
    wrap.hidden = !inis || inis.length === 0;
    const current = select.value;
    select.textContent = '';
    const none = document.createElement('option');
    none.value = '';
    none.textContent = 'none';
    select.appendChild(none);
    for (const ini of inis || []) {
      const opt = document.createElement('option');
      opt.value = ini;
      opt.textContent = ini;
      select.appendChild(opt);
    }
    if ((inis || []).includes(current)) select.value = current;
  }

  // ---------------------------------------------------------------------------
  // Persistence (browser storage; convenience only)
  // ---------------------------------------------------------------------------

  let saveTimer = 0;
  let storageWarned = false;

  function scheduleSave() {
    clearTimeout(saveTimer);
    saveTimer = setTimeout(saveState, 500);
  }

  function saveState() {
    const state = {
      options: {
        width: $('opt-width').value,
        height: $('opt-height').value,
        aa: $('opt-aa').checked,
        clearFrames: $('opt-clear-frames').checked,
        threads: $('opt-threads').value,
        extra: $('opt-extra').value,
      },
    };
    if (project) {
      const files = [];
      for (const f of project.files.values()) {
        if (f.text !== undefined) files.push({ name: f.name, text: f.text, dirty: !!f.dirty });
      }
      state.project = {
        kind: project.kind,
        example: project.example ? project.example.path : null,
        files,
        active: activeName,
        main: mainName,
      };
    }
    const error = storageSet(state);
    if (error && !storageWarned) {
      log('Could not keep your edits in the browser\'s storage (' + (error.message || error) + '). ' +
          'They will be lost when this page is closed; save them to disk.', 'error');
    }
    storageWarned = !!error;
  }

  // ---------------------------------------------------------------------------
  // Opening files from disk
  // ---------------------------------------------------------------------------

  const fileInput = $('file-input');
  let fileInputResolve = null;

  fileInput.addEventListener('change', () => {
    const files = [...fileInput.files].map((file) => ({ file, handle: null, name: file.webkitRelativePath || file.name }));
    fileInput.value = '';
    if (fileInputResolve) fileInputResolve(files);
    fileInputResolve = null;
  });

  function pickWithInput(directory) {
    return new Promise((resolve) => {
      fileInputResolve = resolve;
      fileInput.webkitdirectory = !!directory;
      fileInput.click();
    });
  }

  async function readDirectoryHandle(dirHandle, prefix, out) {
    for await (const entry of dirHandle.values()) {
      const path = prefix ? `${prefix}/${entry.name}` : entry.name;
      if (entry.kind === 'file') {
        out.push({ file: await entry.getFile(), handle: entry, name: path });
      } else if (entry.kind === 'directory' && !entry.name.startsWith('.')) {
        await readDirectoryHandle(entry, path, out);
      }
    }
  }

  async function openFiles({ add = false, directory = false } = {}) {
    let picked;
    try {
      if (directory && window.showDirectoryPicker) {
        const dir = await window.showDirectoryPicker({ id: 'povray-scenes' });
        picked = [];
        await readDirectoryHandle(dir, '', picked);
      } else if (!directory && window.showOpenFilePicker) {
        const handles = await window.showOpenFilePicker({ id: 'povray-scenes', multiple: true });
        picked = await Promise.all(handles.map(async (h) => ({ file: await h.getFile(), handle: h, name: h.name })));
      } else {
        picked = await pickWithInput(directory);
      }
    } catch (e) {
      if (e.name !== 'AbortError') log('Could not open files: ' + e.message, 'error');
      return;
    }
    if (!picked || !picked.length) return;
    // Strip a common top-level folder from directory picks.
    if (directory) {
      const first = picked[0].name.split('/')[0];
      if (picked.every((p) => p.name.startsWith(first + '/'))) {
        for (const p of picked) p.name = p.name.slice(first.length + 1);
      }
    }
    await loadPickedFiles(picked, add);
  }

  async function loadPickedFiles(picked, add) {
    if (!add && hasUnsavedChanges() && !confirm('Discard unsaved changes?')) return;
    const files = add && project && project.kind === 'local' ? project.files : new Map();
    if (add && project && project.kind === 'example') {
      // Adding files to an example turns it into a local project that keeps the scene text.
      for (const f of project.files.values()) files.set(f.name, f);
    }
    let firstText = null;
    for (const { file, handle, name } of picked) {
      const entry = { name, handle, dirty: false };
      if (isText(name)) {
        entry.text = await file.text();
        if (!firstText || (ext(name) === 'pov' && ext(firstText) !== 'pov')) firstText = name;
      } else {
        entry.data = await file.arrayBuffer();
      }
      files.set(name, entry);
    }
    const main = [...files.keys()].find((n) => ext(n) === 'pov' && picked.some((p) => p.name === n));
    setProject({ kind: 'local', files, main }, firstText || undefined);
    history.replaceState(null, '', location.pathname);
    log(`Opened ${picked.length} file${picked.length === 1 ? '' : 's'}.`, 'note');
  }

  // Drag & drop.
  const dropOverlay = $('drop-overlay');
  let dragDepth = 0;
  window.addEventListener('dragenter', (e) => {
    if (!e.dataTransfer || ![...e.dataTransfer.types].includes('Files')) return;
    dragDepth++;
    dropOverlay.hidden = false;
  });
  window.addEventListener('dragleave', () => {
    dragDepth = Math.max(0, dragDepth - 1);
    if (!dragDepth) dropOverlay.hidden = true;
  });
  window.addEventListener('dragover', (e) => e.preventDefault());
  window.addEventListener('drop', async (e) => {
    e.preventDefault();
    dragDepth = 0;
    dropOverlay.hidden = true;
    const items = [...(e.dataTransfer.items || [])];
    const picked = [];
    // Prefer handles (so that Save writes back to the original file).
    if (items.length && items[0].getAsFileSystemHandle) {
      for (const item of items) {
        if (item.kind !== 'file') continue;
        const handle = await item.getAsFileSystemHandle();
        if (!handle) continue;
        if (handle.kind === 'directory') await readDirectoryHandle(handle, '', picked);
        else picked.push({ file: await handle.getFile(), handle, name: handle.name });
      }
    } else {
      for (const file of e.dataTransfer.files) picked.push({ file, handle: null, name: file.name });
    }
    if (picked.length) await loadPickedFiles(picked, !!project && project.kind === 'local' && e.shiftKey);
  });

  // ---------------------------------------------------------------------------
  // Saving files to disk
  // ---------------------------------------------------------------------------

  function download(blob, name) {
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = name;
    document.body.appendChild(a);
    a.click();
    a.remove();
    setTimeout(() => URL.revokeObjectURL(url), 10000);
  }

  async function writeHandle(handle, blob) {
    if (handle.requestPermission) {
      const perm = await handle.requestPermission({ mode: 'readwrite' });
      if (perm !== 'granted') throw new Error('permission denied');
    }
    const writable = await handle.createWritable();
    await writable.write(blob);
    await writable.close();
  }

  /** Save a blob, asking for a location when possible. Returns the handle used (if any). */
  async function saveBlob(blob, suggestedName, types) {
    if (window.showSaveFilePicker) {
      let handle;
      try {
        handle = await window.showSaveFilePicker({ id: 'povray-save', suggestedName, types });
      } catch (e) {
        if (e.name === 'AbortError') return null;
        throw e;
      }
      await writeHandle(handle, blob);
      return handle;
    }
    download(blob, suggestedName);
    return null;
  }

  async function saveActive(saveAs) {
    const file = project && project.files.get(activeName);
    if (!file || file.text === undefined) return;
    const blob = new Blob([file.text], { type: 'text/plain' });
    try {
      if (!saveAs && file.handle && file.handle.createWritable) {
        await writeHandle(file.handle, blob);
      } else {
        const baseName = file.name.slice(file.name.lastIndexOf('/') + 1);
        const handle = await saveBlob(blob, baseName, [{
          description: 'POV-Ray files',
          accept: { 'text/plain': ['.pov', '.inc', '.ini', '.txt', '.mcr'] },
        }]);
        if (handle === null && window.showSaveFilePicker) return;  // cancelled
        if (handle) file.handle = handle;
      }
      file.dirty = false;
      renderTabs();
      scheduleSave();
      log(`Saved ${file.name}.`, 'note');
    } catch (e) {
      log(`Could not save ${file.name}: ${e.message}`, 'error');
    }
  }

  async function saveImage() {
    try {
      let blob;
      let name;
      if (selectedOutput >= 0 && outputs[selectedOutput]) {
        ({ blob, name } = outputs[selectedOutput]);
      } else {
        blob = await new Promise((resolve) => canvas.toBlob(resolve, 'image/png'));
        name = (mainName ? stem(mainName) : 'render') + '.png';
      }
      const type = blob.type || 'application/octet-stream';
      await saveBlob(blob, name, [{ description: 'Image', accept: { [type]: ['.' + ext(name)] } }]);
    } catch (e) {
      log('Could not save the image: ' + e.message, 'error');
    }
  }

  // ---------------------------------------------------------------------------
  // Example scenes
  // ---------------------------------------------------------------------------

  const examplesDialog = $('examples-dialog');
  const examplesGrid = $('examples-grid');
  const examplesSearch = $('examples-search');
  const examplesCategory = $('examples-category');

  function loadManifest() {
    if (!manifestPromise) {
      manifestPromise = fetch('scenes.json' + VERSION_QUERY)
        .then((r) => {
          if (!r.ok) throw new Error(`HTTP ${r.status}`);
          return r.json();
        })
        .then((m) => {
          manifest = m;
          const categories = [...new Set(m.scenes.map((s) => s.category))].sort();
          examplesCategory.textContent = '';
          const all = document.createElement('option');
          all.value = '';
          all.textContent = `All categories (${m.scenes.length})`;
          examplesCategory.appendChild(all);
          for (const c of categories) {
            const opt = document.createElement('option');
            opt.value = c;
            opt.textContent = `${c} (${m.scenes.filter((s) => s.category === c).length})`;
            examplesCategory.appendChild(opt);
          }
          return m;
        })
        .catch((e) => {
          manifestPromise = null;
          throw e;
        });
    }
    return manifestPromise;
  }

  async function showExamples() {
    examplesDialog.showModal();
    try {
      await loadManifest();
    } catch (e) {
      examplesGrid.textContent = 'Could not load the scene list: ' + e.message;
      return;
    }
    filterExamples();
    examplesSearch.focus();
  }

  function filterExamples() {
    if (!manifest) return;
    const q = examplesSearch.value.trim().toLowerCase();
    const cat = examplesCategory.value;
    examplesGrid.textContent = '';
    const frag = document.createDocumentFragment();
    let count = 0;
    for (const scene of manifest.scenes) {
      if (cat && scene.category !== cat) continue;
      if (q && !(scene.path.toLowerCase().includes(q) || (scene.description || '').toLowerCase().includes(q))) continue;
      count++;
      const card = document.createElement('button');
      card.type = 'button';
      card.className = 'example';
      card.title = scene.path + (scene.description ? '\n' + scene.description : '');
      const thumb = document.createElement('div');
      thumb.className = 'thumb';
      if (scene.preview) {
        const img = document.createElement('img');
        img.loading = 'lazy';
        img.alt = '';
        img.src = 'scenes/' + scene.preview;
        thumb.appendChild(img);
      } else {
        thumb.classList.add('none');
        thumb.textContent = '.pov';
      }
      const name = document.createElement('div');
      name.className = 'name';
      name.textContent = scene.name;
      const dir = document.createElement('div');
      dir.className = 'dir';
      dir.textContent = scene.dir;
      card.append(thumb, name, dir);
      card.addEventListener('click', () => {
        examplesDialog.close();
        loadExample(scene.path);
      });
      frag.appendChild(card);
    }
    if (!count) {
      const empty = document.createElement('p');
      empty.className = 'empty';
      empty.textContent = 'No example scenes match.';
      frag.appendChild(empty);
    }
    examplesGrid.appendChild(frag);
  }

  async function loadExample(path, savedText) {
    if (savedText === undefined && hasUnsavedChanges() && !confirm('Discard unsaved changes?')) return;
    let scene;
    try {
      await loadManifest();
      scene = manifest.scenes.find((s) => s.path === path);
      if (!scene) throw new Error('unknown example ' + path);
      let text = savedText;
      if (text === undefined) {
        const r = await fetch('scenes/' + path);
        if (!r.ok) throw new Error(`HTTP ${r.status}`);
        text = await r.text();
      }
      const name = path.slice(path.lastIndexOf('/') + 1);
      const files = new Map();
      files.set(name, { name, text, dirty: savedText !== undefined && savedText !== null });
      setProject({ kind: 'example', example: scene, files, main: name });
      history.replaceState(null, '', '#example=' + encodeURIComponent(path));
    } catch (e) {
      log(`Could not load example ${path}: ${e.message}`, 'error');
      return;
    }
    if (savedText === undefined) {
      applySuggestedOptions(scene);
      log(`Loaded example ${path}${scene.description ? ' – ' + scene.description : ''}`, 'note');
      if (scene.options.length) log(`Suggested options: ${scene.options.join('  |  ')}`, 'note');
    }
  }

  function applySuggestedOptions(scene) {
    const opt = scene.options.find((o) => /[+-]w\d+/i.test(o) && /[+-]h\d+/i.test(o));
    if (!opt) return;
    const w = /[+-]w(\d+)/i.exec(opt);
    const h = /[+-]h(\d+)/i.exec(opt);
    if (w && h) {
      $('opt-width').value = w[1];
      $('opt-height').value = h[1];
    }
  }

  examplesSearch.addEventListener('input', filterExamples);
  examplesCategory.addEventListener('change', filterExamples);

  // ---------------------------------------------------------------------------
  // Rendering
  // ---------------------------------------------------------------------------

  const canvas = $('canvas');
  const ctx = canvas.getContext('2d');
  const stateEl = $('state');
  const statusEl = $('status');
  const elapsedEl = $('elapsed');
  const outputsEl = $('outputs');

  function setState(text, cls) {
    stateEl.textContent = text;
    stateEl.className = 'state' + (cls ? ' ' + cls : '');
  }

  function setRunning(running) {
    $('btn-render').disabled = running;
    $('btn-cancel').disabled = !running;
    document.body.classList.toggle('rendering', running);
  }

  function clearOutputs() {
    for (const o of outputs) URL.revokeObjectURL(o.url);
    outputs = [];
    selectedOutput = -1;
    outputsEl.textContent = '';
    outputsEl.hidden = true;
    $('btn-save-image').disabled = true;
  }

  function showPreview(width, height, pixels) {
    if (canvas.width !== width || canvas.height !== height) {
      canvas.width = width;
      canvas.height = height;
    }
    ctx.putImageData(new ImageData(new Uint8ClampedArray(pixels), width, height), 0, 0);
    $('placeholder').hidden = true;
    hasPreview = true;
  }

  const isImageOutput = (o) => /^image\//.test(o.blob.type);
  const isViewableOutput = (o) => /^image\/(png|jpeg|gif|bmp)$/.test(o.blob.type);

  async function showOutput(index) {
    const o = outputs[index];
    if (!o) return;
    if (!isImageOutput(o)) {
      download(o.blob, o.name);
      return;
    }
    selectedOutput = index;
    for (const el of outputsEl.querySelectorAll('.output')) el.classList.toggle('selected', Number(el.dataset.index) === index);
    if (!isViewableOutput(o)) return;
    try {
      const bitmap = await createImageBitmap(o.blob);
      canvas.width = bitmap.width;
      canvas.height = bitmap.height;
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      ctx.drawImage(bitmap, 0, 0);
      $('placeholder').hidden = true;
    } catch (e) { /* not decodable by the browser; keep the preview */ }
  }

  function renderOutputs() {
    outputsEl.textContent = '';
    outputsEl.hidden = outputs.length === 0;
    if (!outputs.length) return;
    const label = document.createElement('span');
    label.className = 'outputs-label';
    label.textContent = outputs.length === 1 ? 'Output:' : `Output (${outputs.length} files):`;
    outputsEl.appendChild(label);
    outputs.forEach((o, i) => {
      const item = document.createElement('span');
      item.className = 'output';
      item.dataset.index = String(i);
      const view = document.createElement('button');
      view.className = 'link';
      view.textContent = o.name;
      view.title = isViewableOutput(o) ? 'Show in the preview' : isImageOutput(o) ? 'Select for "Save image"' : 'Download';
      view.addEventListener('click', () => showOutput(i));
      const dl = document.createElement('a');
      dl.href = o.url;
      dl.download = o.name;
      dl.textContent = '⤓';
      dl.title = 'Download ' + o.name;
      item.append(view, dl);
      outputsEl.appendChild(item);
    });
  }

  function buildRenderRequest() {
    const isExample = project.kind === 'example';
    const cwd = isExample ? `${SCENES_ROOT}/${project.example.dir}` : LOCAL_ROOT;

    const files = [];
    for (const f of project.files.values()) {
      const path = `${cwd}/${f.name}`;
      if (f.text !== undefined) files.push({ path, data: f.text });
      else files.push({ path, data: f.data.slice(0) });
    }

    const lazyFiles = [];
    if (isExample && manifest) {
      for (const f of manifest.files) {
        lazyFiles.push({ path: `${SCENES_ROOT}/${f.path}`, url: new URL('scenes/' + f.path, location.href).href });
      }
    }

    const args = [];
    const ini = isExample ? $('opt-ini').value : '';
    if (ini) args.push(ini);
    if (ext(mainName) === 'ini') args.push(mainName);
    else args.push('+I' + mainName);
    args.push('+O/out/');

    const w = parseInt($('opt-width').value, 10);
    const h = parseInt($('opt-height').value, 10);
    if (w > 0) args.push('+W' + w);
    if (h > 0) args.push('+H' + h);
    args.push($('opt-aa').checked ? '+A0.3' : '-A');
    const threads = parseInt($('opt-threads').value, 10);
    if (threads > 0) args.push('+WT' + threads);
    args.push('+D', '-P');
    args.push(...splitArgs($('opt-extra').value));

    return { type: 'render', cwd, files, lazyFiles, args, clearBetweenFrames: $('opt-clear-frames').checked };
  }

  async function startRender() {
    if (job) return;
    if (!project || !mainName) {
      log('Nothing to render: open a .pov file or pick an example scene first.', 'error');
      return;
    }
    if (!window.crossOriginIsolated) {
      log('This page is not cross-origin isolated, so WebAssembly threads are unavailable. ' +
          'Serve it with COOP/COEP headers (see web/README.md) or reload so the service worker can enable them.', 'error');
      return;
    }
    if (project.kind === 'example') {
      try { await loadManifest(); } catch (e) { log('Could not load the scene list: ' + e.message, 'error'); return; }
    }

    const request = buildRenderRequest();
    clearOutputs();
    log('');
    log(`$ povray ${request.args.join(' ')}`, 'command');

    const started = performance.now();
    const eng = getEngine();
    job = { engine: eng, started, cancelled: false, killTimer: 0, tick: 0 };
    setRunning(true);
    setState(eng.ready ? 'Starting…' : 'Loading POV-Ray…', 'busy');
    statusEl.textContent = '';
    job.tick = setInterval(() => { elapsedEl.textContent = formatDuration(performance.now() - started); }, 200);
    eng.worker.postMessage(request);
  }

  // ---------------------------------------------------------------------------
  // Render engine: one long-lived worker hosting POV-Ray, reused for every
  // render (see render-worker.js for why). It is only replaced when it has
  // crashed or a render had to be killed.
  // ---------------------------------------------------------------------------

  let engine = null;

  function getEngine() {
    if (engine) return engine;
    const eng = { worker: new Worker('render-worker.js' + VERSION_QUERY), ready: false };
    eng.worker.onmessage = (e) => {
      if (engine !== eng) return;  // a discarded engine still winding down
      if (e.data.type === 'ready') eng.ready = true;
      onWorkerMessage(e.data);
    };
    eng.worker.onerror = (e) => {
      if (engine !== eng) return;
      e.preventDefault();
      log('POV-Ray crashed: ' + (e.message || 'unknown error'), 'error');
      discardEngine();
      endJob('Failed', 'error');
    };
    engine = eng;
    return eng;
  }

  function discardEngine() {
    if (!engine) return;
    engine.worker.terminate();
    engine = null;
  }

  function warmUpEngine() {
    if (window.crossOriginIsolated && !engine) getEngine().worker.postMessage({ type: 'init' });
  }

  function onWorkerMessage(msg) {
    switch (msg.type) {
      case 'started':
        setState('Starting…', 'busy');
        break;
      case 'log':
        log(msg.text, msg.stream === 'stderr' ? null : 'stdout');
        break;
      case 'status':
        statusEl.textContent = msg.text;
        break;
      case 'state':
        setState(msg.state === 'parsing' ? 'Parsing…' : msg.state === 'rendering' ? 'Rendering…' : msg.state, 'busy');
        break;
      case 'frame':
        setState(`Frame ${msg.current} of ${msg.total}`, 'busy');
        break;
      case 'preview':
        showPreview(msg.width, msg.height, msg.pixels);
        break;
      case 'ready':
        $('povray-version').textContent = 'POV-Ray ' + msg.povrayVersion;
        $('povray-version').hidden = false;
        if (!job) setState('Ready');
        break;
      case 'fatal':
        log(msg.message, 'error');
        discardEngine();
        endJob('Failed', 'error');
        break;
      case 'done': {
        for (const o of msg.outputs) {
          const blob = new Blob([o.data], { type: mimeFor(o.name) });
          outputs.push({ name: o.name, blob, url: URL.createObjectURL(blob) });
        }
        renderOutputs();
        const lastImage = outputs.map(isImageOutput).lastIndexOf(true);
        if (lastImage >= 0) showOutput(lastImage);
        const ok = msg.code === 0;
        const cancelled = job && job.cancelled;
        endJob(cancelled ? 'Stopped' : ok ? 'Done' : 'Failed', ok && !cancelled ? 'ok' : 'error');
        break;
      }
      default:
        break;
    }
  }

  function endJob(text, cls) {
    if (!job) return;
    clearInterval(job.tick);
    clearTimeout(job.killTimer);
    elapsedEl.textContent = formatDuration(performance.now() - job.started);
    job = null;
    setRunning(false);
    setState(text, cls);
    $('btn-save-image').disabled = !(hasPreview || outputs.some(isImageOutput));
  }

  function cancelRender() {
    if (!job) return;
    job.cancelled = true;
    setState('Stopping…', 'busy');
    job.engine.worker.postMessage({ type: 'cancel' });
    // If POV-Ray does not wind down promptly (e.g. stuck in a long parse),
    // kill it; the next render starts a fresh engine.
    job.killTimer = setTimeout(() => {
      log('Render did not stop in time; POV-Ray was restarted.', 'error');
      discardEngine();
      endJob('Stopped', 'error');
      warmUpEngine();
    }, 5000);
  }

  // ---------------------------------------------------------------------------
  // Layout: resizable split between editor and output
  // ---------------------------------------------------------------------------

  const splitter = $('splitter');
  splitter.addEventListener('pointerdown', (e) => {
    splitter.setPointerCapture(e.pointerId);
    const workspace = document.querySelector('.workspace');
    const rect = workspace.getBoundingClientRect();
    const vertical = getComputedStyle(workspace).flexDirection === 'column';
    const move = (ev) => {
      const frac = vertical ? (ev.clientY - rect.top) / rect.height : (ev.clientX - rect.left) / rect.width;
      workspace.style.setProperty('--split', `${Math.min(85, Math.max(15, frac * 100))}%`);
    };
    const up = () => {
      splitter.removeEventListener('pointermove', move);
      splitter.removeEventListener('pointerup', up);
    };
    splitter.addEventListener('pointermove', move);
    splitter.addEventListener('pointerup', up);
  });

  // ---------------------------------------------------------------------------
  // Wiring
  // ---------------------------------------------------------------------------

  $('btn-new').addEventListener('click', newProject);
  $('btn-examples').addEventListener('click', showExamples);
  $('btn-open').addEventListener('click', () => openFiles());
  $('btn-open-folder').addEventListener('click', () => openFiles({ directory: true }));
  $('btn-save').addEventListener('click', () => saveActive(false));
  $('btn-save-as').addEventListener('click', () => saveActive(true));
  $('btn-render').addEventListener('click', startRender);
  $('btn-cancel').addEventListener('click', cancelRender);
  $('btn-save-image').addEventListener('click', saveImage);
  $('btn-clear-log').addEventListener('click', clearLog);
  for (const id of ['opt-width', 'opt-height', 'opt-aa', 'opt-clear-frames', 'opt-threads', 'opt-extra']) {
    $(id).addEventListener('change', scheduleSave);
  }

  window.addEventListener('keydown', (e) => {
    const mod = e.ctrlKey || e.metaKey;
    if (mod && e.key === 'Enter') {
      e.preventDefault();
      startRender();
    } else if (mod && (e.key === 's' || e.key === 'S')) {
      e.preventDefault();
      saveActive(e.shiftKey);
    } else if (mod && (e.key === 'o' || e.key === 'O')) {
      e.preventDefault();
      openFiles();
    } else if (e.key === 'Escape' && job) {
      cancelRender();
    }
  });

  window.addEventListener('beforeunload', (e) => {
    saveState();
    if (job || [...(project ? project.files.values() : [])].some((f) => f.dirty && f.handle)) {
      e.preventDefault();
      e.returnValue = '';
    }
  });

  // ---------------------------------------------------------------------------
  // Start-up
  // ---------------------------------------------------------------------------

  function checkEnvironment() {
    if (window.crossOriginIsolated) return;
    const banner = document.createElement('div');
    banner.className = 'banner';
    banner.textContent = window.isSecureContext && 'serviceWorker' in navigator
      ? 'Enabling WebAssembly threads… if this message stays, reload the page.'
      : 'This page needs to be served over HTTPS (or from localhost) with cross-origin isolation to run POV-Ray. See web/README.md.';
    document.body.prepend(banner);
  }

  async function init() {
    checkEnvironment();
    const saved = storageGet();
    if (saved && saved.options) {
      const o = saved.options;
      if (o.width) $('opt-width').value = o.width;
      if (o.height) $('opt-height').value = o.height;
      if (typeof o.aa === 'boolean') $('opt-aa').checked = o.aa;
      if (typeof o.clearFrames === 'boolean') $('opt-clear-frames').checked = o.clearFrames;
      if (o.threads !== undefined) $('opt-threads').value = o.threads;
      if (o.extra !== undefined) $('opt-extra').value = o.extra;
    }

    const hash = new URLSearchParams(location.hash.slice(1));
    const examplePath = hash.get('example');
    if (examplePath) {
      const sp = saved && saved.project;
      const savedText = sp && sp.kind === 'example' && sp.example === examplePath && sp.files[0] && sp.files[0].dirty
        ? sp.files[0].text : undefined;
      await loadExample(examplePath, savedText);
      if (project) return;
    }

    const sp = saved && saved.project;
    if (sp && sp.kind === 'example' && sp.example && sp.files.length) {
      await loadExample(sp.example, sp.files[0].dirty ? sp.files[0].text : undefined);
      if (project) return;
    }
    if (sp && sp.kind === 'local' && sp.files && sp.files.length) {
      const files = new Map();
      for (const f of sp.files) files.set(f.name, { name: f.name, text: f.text, dirty: f.dirty });
      setProject({ kind: 'local', files, main: sp.main }, sp.active);
      return;
    }
    const files = new Map();
    files.set('scene.pov', { name: 'scene.pov', text: NEW_SCENE, dirty: false });
    setProject({ kind: 'local', files });
  }

  window.addEventListener('hashchange', () => {
    const path = new URLSearchParams(location.hash.slice(1)).get('example');
    if (path && !(project && project.kind === 'example' && project.example.path === path)) loadExample(path);
  });

  init();
  warmUpEngine();
  loadManifest().catch(() => { /* reported when used */ });
})();
