#!/usr/bin/env python3
"""Render every example scene with the web build, in headless Chromium.

Serves a built site (see web/build.sh), opens it in Chromium with Playwright,
and renders each scene listed in the site's `scenes.json` at a small size,
using the same render worker and job format as the page. Each render must
finish with POV-Ray reporting success. Scenes listed in `sweep-expected.txt`
are skipped, expected to fail, or rendered with one of their INI files.

The sweep fails if a scene fails unexpectedly, crashes the WebAssembly
module, or times out, and also if a scene expected to fail renders
successfully, so that the list of exceptions stays current.

Requires Playwright with Chromium:

    pip install playwright
    python -m playwright install chromium

Usage:  python3 web/tools/sweep.py [--site build-web/site] [--filter REGEX]
"""

import argparse
import asyncio
import functools
import http.server
import json
import os
import re
import sys
import threading
import time

from playwright.async_api import async_playwright

TOOLS_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.abspath(os.path.join(TOOLS_DIR, "..", ".."))
sys.path.insert(0, TOOLS_DIR)
from serve import Handler  # noqa: E402

SCENES_ROOT = "/povray/scenes"  # see app.js
HARNESS_PATH = "/__sweep__.html"

# Runs in the page. Each slot hosts one render worker (one POV-Ray instance);
# a slot's worker is replaced after a crash or a render that does not stop.
HARNESS_JS = r"""
window.sweep = (() => {
  const slots = [];
  const LOG_LINES = 40;

  function newWorker(slot) {
    slots[slot] = { worker: new Worker('render-worker.js'), handler: null };
    slots[slot].worker.onmessage = (e) => slots[slot].handler && slots[slot].handler(e.data);
    slots[slot].worker.onerror = (e) => {
      e.preventDefault();
      slots[slot].handler && slots[slot].handler({ type: 'fatal', message: e.message || 'worker error' });
    };
    return slots[slot];
  }

  function render(slot, request, timeoutMs) {
    const s = slots[slot] || newWorker(slot);
    const log = [];
    const started = performance.now();
    return new Promise((resolve) => {
      let timer = 0;
      let killTimer = 0;
      const finish = (result) => {
        clearTimeout(timer);
        clearTimeout(killTimer);
        s.handler = null;
        result.seconds = (performance.now() - started) / 1000;
        result.log = log.slice(-LOG_LINES);
        resolve(result);
      };
      const discard = () => { s.worker.terminate(); slots[slot] = null; };
      s.handler = (msg) => {
        if (msg.type === 'log') {
          log.push(msg.text);
        } else if (msg.type === 'done') {
          finish({ status: timer === 0 ? 'timeout' : (msg.code === 0 ? 'ok' : 'error'), code: msg.code });
        } else if (msg.type === 'fatal') {
          discard();
          finish({ status: 'crash', message: msg.message });
        }
      };
      timer = setTimeout(() => {
        timer = 0;
        s.worker.postMessage({ type: 'cancel' });
        killTimer = setTimeout(() => { discard(); finish({ status: 'timeout' }); }, 10000);
      }, timeoutMs);
      s.worker.postMessage(request);
    });
  }

  return { render };
})();
"""


def parse_expected(path):
    """Read `<scene> <expectation>` lines; `#` starts a comment."""
    expected = {}
    with open(path, encoding="utf-8") as f:
        for lineno, line in enumerate(f, 1):
            line = line.split("#", 1)[0].strip()
            if not line:
                continue
            fields = line.split()
            if len(fields) != 2 or not re.fullmatch(r"skip|error|ini=\S+", fields[1]):
                sys.exit(f"{path}:{lineno}: expected '<scene> skip|error|ini=<file>'")
            expected[fields[0]] = fields[1]
    return expected


class QuietHandler(Handler):
    def log_message(self, format, *args):
        pass


def start_server(site):
    handler = functools.partial(QuietHandler, directory=site)
    httpd = http.server.ThreadingHTTPServer(("127.0.0.1", 0), handler)
    threading.Thread(target=httpd.serve_forever, daemon=True).start()
    return httpd, f"http://127.0.0.1:{httpd.server_address[1]}"


def build_request(base, manifest, scene, ini, args):
    lazy_files = [{"path": f"{SCENES_ROOT}/{f['path']}", "url": f"{base}/scenes/{f['path']}"}
                  for f in manifest["files"]]
    render_args = []
    if ini:
        # The INI may define an animation; render only its first frame.
        render_args += [ini, "+SF1", "+EF1"]
    render_args += ["+I" + os.path.basename(scene["path"]), "+O/out/",
                    f"+W{args.width}", f"+H{args.height}", "-A", "+D", "-P",
                    f"+WT{args.threads}"]
    return {"type": "render", "cwd": f"{SCENES_ROOT}/{scene['dir']}",
            "files": [], "lazyFiles": lazy_files, "args": render_args}


async def sweep(args):
    with open(os.path.join(args.site, "scenes.json"), encoding="utf-8") as f:
        manifest = json.load(f)
    expected = parse_expected(args.expected)

    known = {s["path"] for s in manifest["scenes"]}
    problems = [f"{path}: listed in {os.path.basename(args.expected)} but not an example scene"
                for path in expected if path not in known]

    scenes = [s for s in manifest["scenes"] if re.search(args.filter, s["path"])]
    if not scenes:
        sys.exit(f"sweep.py: no example scene matches {args.filter!r}")
    jobs = [s for s in scenes if expected.get(s["path"]) != "skip"]
    skipped = len(scenes) - len(jobs)

    httpd, base = start_server(args.site)
    results = {}
    started = time.monotonic()
    async with async_playwright() as pw:
        browser = await pw.chromium.launch()
        page = await browser.new_page()
        page.on("pageerror", lambda e: print(f"page error: {e}", file=sys.stderr))
        await page.route(base + HARNESS_PATH, lambda route: route.fulfill(
            status=200, content_type="text/html", body="<!doctype html><title>sweep</title>",
            headers={"Cross-Origin-Opener-Policy": "same-origin",
                     "Cross-Origin-Embedder-Policy": "require-corp"}))
        await page.goto(base + HARNESS_PATH)
        if not await page.evaluate("crossOriginIsolated"):
            sys.exit("sweep.py: the harness page is not cross-origin isolated")
        await page.add_script_tag(content=HARNESS_JS)

        queue = list(jobs)
        done = 0

        async def run_slot(slot):
            nonlocal done
            while queue:
                scene = queue.pop(0)
                exp = expected.get(scene["path"], "ok")
                ini = exp[4:] if exp.startswith("ini=") else None
                request = build_request(base, manifest, scene, ini, args)
                result = await page.evaluate("([s, r, t]) => sweep.render(s, r, t)",
                                             [slot, request, args.timeout * 1000])
                results[scene["path"]] = result
                done += 1
                want = "error" if exp == "error" else "ok"
                mark = "" if result["status"] == want else "  <-- unexpected"
                print(f"[{done:3d}/{len(jobs)}] {result['status']:7s} {result['seconds']:6.1f}s  "
                      f"{scene['path']}{' (' + ini + ')' if ini else ''}{mark}", flush=True)

        await asyncio.gather(*(run_slot(i) for i in range(args.workers)))
        await browser.close()
    httpd.shutdown()

    for scene in jobs:
        path = scene["path"]
        result = results[path]
        exp = expected.get(path, "ok")
        want = "error" if exp == "error" else "ok"
        if result["status"] == want:
            continue
        if want == "error" and result["status"] == "ok":
            problems.append(f"{path}: rendered successfully; remove it from {os.path.basename(args.expected)}")
        else:
            lines = ([result["message"]] if result.get("message") else []) + result["log"]
            detail = "\n".join("    " + l for l in lines)
            problems.append(f"{path}: {result['status']} (expected {want})\n{detail}")

    counts = {}
    for r in results.values():
        counts[r["status"]] = counts.get(r["status"], 0) + 1
    summary = ", ".join(f"{n} {k}" for k, n in sorted(counts.items()))
    print(f"\n{len(jobs)} scenes rendered in {time.monotonic() - started:.0f}s ({summary}); {skipped} skipped")

    if args.report:
        with open(args.report, "w", encoding="utf-8") as f:
            json.dump(results, f, indent=1)

    for p in problems:
        first, _, rest = p.partition("\n")
        print(f"::error::{first}" if os.environ.get("GITHUB_ACTIONS") else f"FAIL {first}")
        if rest:
            print(rest)
    return 1 if problems else 0


def main():
    cpus = os.cpu_count() or 4
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--site", default=os.path.join(REPO_ROOT, "build-web", "site"))
    parser.add_argument("--expected", default=os.path.join(TOOLS_DIR, "sweep-expected.txt"))
    parser.add_argument("--filter", default="", help="only scenes whose path matches this regular expression")
    parser.add_argument("--width", type=int, default=80)
    parser.add_argument("--height", type=int, default=60)
    parser.add_argument("--timeout", type=int, default=300, help="seconds per scene")
    parser.add_argument("--workers", type=int, default=max(1, cpus // 4),
                        help="scenes rendered at the same time, each in its own POV-Ray instance")
    parser.add_argument("--threads", type=int, default=0,
                        help="render threads per scene (default: CPUs divided by workers)")
    parser.add_argument("--report", help="write the results as JSON to this file")
    args = parser.parse_args()
    if args.threads <= 0:
        args.threads = max(1, cpus // args.workers)

    if not os.path.isfile(os.path.join(args.site, "povray.wasm")):
        sys.exit(f"sweep.py: {args.site} does not contain a build; run web/build.sh first")
    sys.exit(asyncio.run(sweep(args)))


if __name__ == "__main__":
    main()
