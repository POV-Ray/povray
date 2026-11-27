@page tools-metamake    Meta Build Process

A few POV-Ray "source" files are actually generated themselves from other
sources, and are included in the official repository only because setting up a
portable build process for them is deemed too much effort.


Prerequisites
=============

To re-create the files in question, you will need an Ubuntu machine with the following
packages installed (or a compatible environment):

  - `bash`
  - `libfox-1.6-dev` (we expect other versions to work as well)
  - `python`, `python-numpy`, `python-scipy`, `python-matplotlib`, `python-pypng`
  - `python3` (for the built-in include files; its standard `zlib` module is all it needs)


Procedure
=========


From the root of the POV-Ray source package, change to the `tools/meta-make`
directory, and invoke the following command:

    make

This will re-create all missing or outdated generated files.

To re-create all converted files unconditionally, use:

    make clean all

Note that some files may take a while to re-create.


Output
======

The following files will be re-created:

| Generated File                                    | Generated From                                            |
|:--------------------------------------------------|:----------------------------------------------------------|
| `source/base/data/bluenoise*.cpp`/`.h`            | `tools/meta-make/bluenoise/metagen-bluenoise.py`          |
| `source/base/builtinfiles_data.cpp`               | all files in `distribution/include`, by `tools/meta-make/builtinfiles/metagen-builtinfiles.py` |
| `source/base/font/crystal.cpp`/`.h`               | `distribution/include/crystal.ttf`                        |
| `source/base/font/cyrvetic.cpp`/`.h`              | `distribution/include/cyrvetic.ttf`                       |
| `source/base/font/povlogo.cpp`/`.h`               | `distribution/include/povlogo.ttf`                        |
| `source/base/font/timrom.cpp`/`.h`                | `distribution/include/timrom.ttf`                         |
| `source/backend/control/benchmark_pov.cpp`/`.h`   | `distribution/scenes/advanced/benchmark/benchmark.pov`    |
| `source/backend/control/benchmark_ini.cpp`/`.h`   | `distribution/scenes/advanced/benchmark/benchmark.ini`    |

The built-in include files are stored deflated, so `builtinfiles_data.cpp` also
depends on the version of zlib that Python uses: a different version may produce
different bytes for the same files, all of which inflate to the same contents.
Like every file `metagen.sh` writes, it also carries the current year in its
copyright line, so re-creating it in a later year changes that line too. Hidden
files and editor backup files in `distribution/include` are skipped, any other
file whose extension is not listed in `EXTENSIONS` in `metagen-builtinfiles.py`
stops the generator, and the generator says on stderr how many files it built in.
Leaving out a `.inc` file also leaves its name out of the test that makes a render
use the files on disk when a library path holds a standard include file.
To leave a file of `distribution/include` out of the program, add its name to the
`EXCLUDE` list in `metagen-builtinfiles.py` and re-create the file.
