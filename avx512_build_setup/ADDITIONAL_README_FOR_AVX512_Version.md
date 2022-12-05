Notes for AVX512 Windows build
==============================

The visual studio version was updated from vs2015 to vs2022 to enable support of AVX512 Version
1. Set the configuration in solution file to Release-AVX512 | x64
2. Select the 'Generic POV-Ray > povbase' project and expand 'Backend Headers', then open the
   file `build.h`(source/base/build.h) listed within it.In it replace with name
   and email of person who builds the code in `BUILT_BY` flag and comment the #error directive (line 129)
3. In syspovconfig.h(windows/povconfig/syspovconfig.h) uncomment the #define _CONSOLE. (line 56)
   The AVX512 version was developed with the console version.
   The GUI build has been skipped in the solution file.
   **Note:** (Presently with the updated code the GUI project is skipped for building,
   as the cmedit64.dll and povcmax64.dll from official windows distribution are
   incompatible with VS2022. The console version alone is available to build and test).
4. Build the solution file and in the vs2022/bin64 folder we can run the POVRAY examples with povconsole-avx512.exe.
```
         General command example - povconsole-avx512.exe +Ibenchmark.pov
         Single worker thread - povconsole-avx512.exe +WT1 benchmark.pov
         Output image - benchmark.png
```
5. Results with the AVX512 version has been attached in the same folder.

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

