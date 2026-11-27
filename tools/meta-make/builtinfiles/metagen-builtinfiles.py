#!/usr/bin/env python3
"""Create the table of built-in standard include files as C++ source.

Every file in the source directory (the standard include files, colour maps,
images and fonts of `distribution/include`), except hidden files, editor backup
files (names starting with `.` or `#`, or ending in `~`) and the names listed in
EXCLUDE, is stored deflated with zlib, as a
C-style array of bytes, followed by a table naming each file with its size.
POV-Ray reads a file from this table when a scene names it and it is not found
on disk; `source/base/builtinfiles.h` declares the table.

The output depends only on the contents of the files, their names and the
version of zlib used: names are sorted, and no dates or paths are written.
Different zlib versions may deflate differently, so the output is not expected
to match byte for byte when generated elsewhere; any version inflates to the
same files.
"""

import getopt
import os
import sys
import zlib

# Files of the source directory that are NOT to be built in, by exact name.
# To drop a file from the program, add its name here and re-generate.
EXCLUDE = [
]

# Extensions of the kinds of file in the source directory. Any other file stops
# the generator, so that nothing is built in by accident; to build in a new kind
# of file, add its extension here.
EXTENSIONS = [
    ".cat", ".df3", ".inc", ".map", ".png", ".pot", ".ttf",
]

# zlib compression level; 9 gives the smallest output.
LEVEL = 9

BYTES_PER_LINE = 16


def printerr(s):
    sys.stderr.write("%s\n" % s)


def UsageError():
    printerr("metagen-builtinfiles.py")
    printerr("    Create the table of built-in standard include files as C++ source.")
    printerr("Usage:")
    printerr("    python3 metagen-builtinfiles.py [options] <dir>")
    printerr("Parameters:")
    printerr("    <dir>:      Directory holding the files to build in.")
    printerr("Output options:")
    printerr("    -n<STRING>  Wraps the definitions in a namespace.")
    sys.exit(2)


def Error(s):
    printerr(s)
    sys.exit(2)


def IsStrayFile(name):
    """Whether a name is that of a hidden or editor backup file, never a distribution file."""
    return name.startswith(".") or name.startswith("#") or name.endswith("~")


def FilesOf(src):
    names = []
    skipped = []
    for name in os.listdir(src):
        if not os.path.isfile(os.path.join(src, name)):
            continue
        if name in EXCLUDE:
            continue
        if IsStrayFile(name):
            skipped.append(name)
            continue
        if os.path.splitext(name)[1] not in EXTENSIONS:
            Error("File %r is not of a kind built in (%s): add it to EXCLUDE, or its extension to EXTENSIONS." %
                  (name, " ".join(EXTENSIONS)))
        try:
            name.encode("ascii")
        except UnicodeError:
            Error("File name %r is not plain ASCII." % name)
        if ('"' in name) or ('\\' in name):
            Error("File name %r cannot be written as a C string." % name)
        names.append(name)
    missing = [n for n in EXCLUDE if not os.path.isfile(os.path.join(src, n))]
    if missing:
        Error("Excluded file(s) not found in %s: %s" % (src, ", ".join(missing)))
    # sort by byte value, so the table can be searched with strcmp()
    names.sort(key=lambda n: n.encode("ascii"))
    # say what is built in, so that a change in the number of files is seen
    for name in sorted(skipped):
        printerr("metagen-builtinfiles.py: skipping %s (hidden or backup file)" % name)
    printerr("metagen-builtinfiles.py: building in %d files of %s (%d excluded)" % (len(names), src, len(EXCLUDE)))
    for name in names:
        with open(os.path.join(src, name), "rb") as f:
            yield name, f.read()


def Generate(src, namespace):
    out = []
    table = []
    totalRaw = 0
    totalDeflated = 0

    if namespace:
        out.append("namespace %s {" % namespace)
        out.append("")
    out.append("namespace {")
    out.append("")

    for index, (name, raw) in enumerate(FilesOf(src)):
        data = zlib.compress(raw, LEVEL)
        ident = "kBuiltinData%02d" % index
        totalRaw += len(raw)
        totalDeflated += len(data)
        out.append("// %s: %d bytes, %d deflated" % (name, len(raw), len(data)))
        out.append("const unsigned char %s[%d] = {" % (ident, len(data)))
        for i in range(0, len(data), BYTES_PER_LINE):
            out.append("  " + "".join("0x%02x," % b for b in bytearray(data[i:i + BYTES_PER_LINE])))
        out.append("};")
        out.append("")
        table.append('    { "%s", %d, %d, %s },' % (name, len(raw), len(data), ident))

    out.append("} // anonymous namespace")
    out.append("")
    out.append("/// Built-in files, sorted by name (%d files, %d bytes, %d deflated)." % (len(table), totalRaw, totalDeflated))
    out.append("extern const BuiltinFileEntry kBuiltinFiles[] = {")
    out.extend(table)
    out.append("};")
    out.append("")
    out.append("extern const size_t kBuiltinFileCount = %d;" % len(table))
    out.append("")
    if namespace:
        out.append("}")
        out.append("")
    return "\n".join(out)


try:
    opts, args = getopt.getopt(sys.argv[1:], "n:")
except getopt.GetoptError:
    UsageError()

namespace = ""
for o, a in opts:
    if o == "-n":
        namespace = a

if len(args) != 1:
    UsageError()

sys.stdout.write('#include "base/builtinfiles.h"\n\n')
sys.stdout.write(Generate(args[0], namespace))
