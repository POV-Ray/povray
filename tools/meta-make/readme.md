A few POV-Ray "source" files are actually generated themselves from other
sources, and are included in the official repository only because setting up a
portable build process for them is deemed too much effort.

To re-create those files, you will need an Ubuntu machine with the following
packages installed (or a compatible environment):

  - `bash`
  - `libfox-1.6-dev` (we expect other versions to work as well)

Once you have those packages installed, invoke the following command from the
root directory of the POV-Ray source package:

    tools/meta-make/meta-make.sh

This will re-create the following files:

| Generated File                                    | Generated From                                            |
|:--------------------------------------------------|:----------------------------------------------------------|
| `source/base/font/crystal.cpp`/`.h`               | `distribution/include/crystal.ttf`                        |
| `source/base/font/cyrvetic.cpp`/`.h`              | `distribution/include/cyrvetic.ttf`                       |
| `source/base/font/povlogo.cpp`/`.h`               | `distribution/include/povlogo.ttf`                        |
| `source/base/font/timrom.cpp`/`.h`                | `distribution/include/timrom.ttf`                         |
| `source/backend/control/benchmark_pov.cpp`/`.h`   | `distribution/scenes/advanced/benchmark/benchmark.pov`    |
| `source/backend/control/benchmark_ini.cpp`/`.h`   | `distribution/scenes/advanced/benchmark/benchmark.ini`    |
