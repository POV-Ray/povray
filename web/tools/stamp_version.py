#!/usr/bin/env python3
"""Write the site's index.html with the build version and source details filled in.

Static hosts cache files independently (GitHub Pages: 10 minutes), so right
after a deployment a browser could combine a new `povray.js` with a cached,
old `povray.wasm`. To avoid that, every `@POVWEB_VERSION@` placeholder in
the index.html template is replaced by a hash of the site's code and data
files; the page appends it as a query string to everything it loads.

The page footer links to the source code: the repository given by
`--source-url`, at the checked-out commit. When the source tree is not a git
checkout, the link goes to the repository itself and says "unknown commit".
"""

import argparse
import hashlib
import html
import os
import re
import subprocess
import sys

# Files whose content defines a build. Example scenes are excluded: they
# change rarely and are fetched on demand.
VERSIONED_FILES = [
    "povray.js", "povray.wasm", "povray.data",
    "app.js", "render-worker.js", "style.css", "scenes.json",
]


def git(repo, *args):
    return subprocess.run(["git", "-C", repo, *args], check=True,
                          capture_output=True, text=True).stdout.strip()


def git_revision(repo):
    """The checked-out commit and whether the working tree differs from it, or (None, False)."""
    try:
        # Without a .git of its own, git would report an enclosing checkout.
        if os.path.realpath(git(repo, "rev-parse", "--show-toplevel")) != os.path.realpath(repo):
            return None, False
        sha = git(repo, "rev-parse", "HEAD")
        dirty = git(repo, "status", "--porcelain") != ""
    except (OSError, subprocess.CalledProcessError):
        return None, False
    return sha, dirty


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--template", required=True, help="index.html template")
    parser.add_argument("--site", required=True, help="site output directory")
    parser.add_argument("--repo", required=True, help="root of the POV-Ray source tree")
    parser.add_argument("--source-url", required=True, help="URL of the source repository")
    args = parser.parse_args()

    digest = hashlib.sha256()
    for name in VERSIONED_FILES:
        with open(os.path.join(args.site, name), "rb") as f:
            digest.update(name.encode() + b"\0")
            digest.update(f.read())

    source_url = args.source_url.rstrip("/")
    sha, dirty = git_revision(args.repo)
    if sha:
        source_label = f"Source code ({sha[:8]}{', modified' if dirty else ''})"
        source_url += "/tree/" + sha
    else:
        source_label = "Source code (unknown commit)"

    values = {
        "VERSION": digest.hexdigest()[:12],
        "SOURCE_URL": source_url,
        "SOURCE_LABEL": source_label,
    }

    with open(args.template, "r", encoding="utf-8") as f:
        page = f.read()
    for key, value in values.items():
        page = page.replace(f"@POVWEB_{key}@", html.escape(value))
    leftover = re.search(r"@POVWEB_[A-Z_]+@", page)
    if leftover:
        sys.exit(f"stamp_version.py: unknown placeholder {leftover.group(0)} in {args.template}")
    with open(os.path.join(args.site, "index.html"), "w", encoding="utf-8") as f:
        f.write(page)


if __name__ == "__main__":
    main()
