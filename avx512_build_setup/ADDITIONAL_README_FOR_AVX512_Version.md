Notes for AVX512 Windows build
==============================

The visual studio version was updated from vs2015 to vs2022 to enable support of AVX512 Version
1. Set the configuration in solution file to Release-AVX512 | x64
2. Select the 'Generic POV-Ray > povbase' project and expand 'Backend Headers', then open the
   file `build.h`(source/base/build.h) listed within it.In it replace with name
   and email of person who builds the code in `BUILT_BY` flag and comment the #error directive (line 129)

Steps for console build:
1. In syspovconfig.h(windows/povconfig/syspovconfig.h) uncomment the #define _CONSOLE. (line 56)
2. Build the solution file and in the vs2022/bin64 folder we can run the POVRAY examples with povconsole64-avx512.exe.
```
         General command example - povconsole64-avx512.exe +Ibenchmark.pov
         Single worker thread - povconsole64-avx512.exe +WT1 benchmark.pov
         Output image - benchmark.png
```
Results with the AVX512 version has been attached in the same folder.

Notes for UNIX build
====================

Dependencies for unix build
```
   libboost-dev
   libboost-date-time-dev
   libboost-thread-dev
   libz-dev
   libpng-dev
   libjpeg-dev
   libtiff-dev
   libopenexr-dev
   pkg-config (if its already not there)
```

Steps :
Generating configure and building the code :
```
   % cd unix/
   % ./prebuild.sh
   % cd ../
   % ./configure COMPILED_BY="your name <email@address>"
   % make
```

To build with icpc :
```
   % source /opt/intel/oneapi/setvars.sh
   % cd unix/
   % ./prebuild.sh
   % cd ../
   % ./configure COMPILED_BY="your name <email@address>" CXX=icpc
   % make
```

Sample commands (inside the unix folder) :
```
         General command example - ./povray +Ibenchmark.pov
         Single worker thread - ./povray +WT1 benchmark.pov
         Output image - benchmark.png
```

