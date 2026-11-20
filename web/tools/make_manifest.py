#!/usr/bin/env python3
"""Copy the example scenes into the web site and write a manifest for them.

The manifest (`scenes.json`) lists every example scene (`*.pov`) along with
its category, an optional preview thumbnail and a short description pulled
from the scene's header comments. It also lists every file in the scenes
tree, so that the in-browser file system can expose them lazily: a scene's
include files, image maps, fonts, etc. are downloaded only when POV-Ray
opens them.
"""

import argparse
import json
import os
import re
import shutil
import sys

# Directories that are not scenes themselves.
SKIP_DIRS = {"previews"}
# The templates directory mostly holds snippets and thumbnails for the Windows
# editor's "Insert" menu; only its complete `.pov` scenes are exposed.
TEMPLATES_DIR = "templates"
# Files that are only useful for the Windows editor or offline browsing.
SKIP_SUFFIXES = {".htm", ".html", ".css", ".dox"}

DESC_RE = re.compile(r"^//\s*(?:Desc(?:ription)?)\s*:\s*(.+?)\s*$", re.IGNORECASE)
OPTS_RE = re.compile(r"^//\s*((?:[+-][a-zA-Z][^\s]*\s*)+)$")
INPUT_RE = re.compile(r"^\s*Input_File_Name\s*=\s*\"?([^\"\s;]+)", re.IGNORECASE | re.MULTILINE)


def ini_files_for(dirpath, filenames, scene_name):
    """INI files in the scene's directory that render this scene."""
    stem = os.path.splitext(scene_name)[0].lower()
    result = []
    for name in sorted(filenames):
        if not name.lower().endswith(".ini"):
            continue
        try:
            with open(os.path.join(dirpath, name), "r", encoding="latin-1") as f:
                text = f.read()
        except OSError:
            continue
        m = INPUT_RE.search(text)
        target = os.path.basename(m.group(1)).lower() if m else None
        if target in (scene_name.lower(), stem) or (target is None and os.path.splitext(name)[0].lower() == stem):
            result.append(name)
    return result


def read_header(path):
    """Extract a short description and suggested command-line options."""
    desc = None
    options = []
    try:
        with open(path, "r", encoding="latin-1") as f:
            for i, line in enumerate(f):
                if i > 60:
                    break
                line = line.rstrip()
                m = DESC_RE.match(line)
                if m and desc is None:
                    desc = m.group(1)
                m = OPTS_RE.match(line)
                if m:
                    opt = m.group(1).strip()
                    # Only keep obvious render options (sizes, antialiasing, frames).
                    if re.search(r"(^|\s)[+-][wWhHaAkK]", opt) and len(opt) < 80:
                        options.append(opt)
    except OSError:
        pass
    return desc, options


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--scenes", required=True, help="distribution/scenes directory")
    parser.add_argument("--out", required=True, help="site output directory")
    args = parser.parse_args()

    src_root = os.path.abspath(args.scenes)
    out_root = os.path.join(os.path.abspath(args.out), "scenes")

    files = []
    scenes = []

    for dirpath, dirnames, filenames in os.walk(src_root):
        rel_dir = os.path.relpath(dirpath, src_root)
        rel_dir = "" if rel_dir == "." else rel_dir.replace(os.sep, "/")
        if rel_dir == "":
            dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        dirnames.sort()
        in_templates = rel_dir.split("/")[0] == TEMPLATES_DIR
        for name in sorted(filenames):
            if os.path.splitext(name)[1].lower() in SKIP_SUFFIXES:
                continue
            if in_templates and not name.lower().endswith(".pov"):
                continue
            src = os.path.join(dirpath, name)
            rel = f"{rel_dir}/{name}" if rel_dir else name
            size = os.path.getsize(src)
            files.append({"path": rel, "size": size})

            dst = os.path.join(out_root, rel)
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            if not os.path.exists(dst) or os.path.getmtime(dst) < os.path.getmtime(src):
                shutil.copy2(src, dst)

            if name.lower().endswith(".pov") and rel_dir:
                stem = os.path.splitext(name)[0]
                if in_templates:
                    preview_rel = f"{rel_dir}/{stem}.jpg"
                else:
                    preview_rel = f"previews/{rel_dir}/{stem}.jpg"
                preview_src = os.path.join(src_root, preview_rel)
                preview = None
                if os.path.exists(preview_src):
                    preview = preview_rel
                    dst = os.path.join(out_root, preview_rel)
                    os.makedirs(os.path.dirname(dst), exist_ok=True)
                    if not os.path.exists(dst) or os.path.getmtime(dst) < os.path.getmtime(preview_src):
                        shutil.copy2(preview_src, dst)
                desc, options = read_header(src)
                scenes.append({
                    "path": rel,
                    "name": stem,
                    "category": rel_dir.split("/")[0],
                    "dir": rel_dir,
                    "preview": preview,
                    "description": desc,
                    "options": options,
                    "inis": ini_files_for(dirpath, filenames, name),
                })

    manifest = {"scenes": scenes, "files": files}
    with open(os.path.join(os.path.abspath(args.out), "scenes.json"), "w", encoding="utf-8") as f:
        json.dump(manifest, f, separators=(",", ":"))

    print(f"make_manifest: {len(scenes)} scenes, {len(files)} files", file=sys.stderr)


if __name__ == "__main__":
    main()
