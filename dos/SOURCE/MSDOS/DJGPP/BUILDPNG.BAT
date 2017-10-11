@echo off
cd ..\..\zlib
echo Don't worry about files not found messages
del *.bak
del *.obj
del *.o
del *.d
del *.lib
del *.a
del *.exe
del test
gmake -fmsdos\makefile.dj2
copy *.a ..\msdos\djgpp
cd ..\lpng101
gmake -fscripts\makefile.dj2
copy *.a ..\msdos\djgpp
echo Don't worry about files not found messages
del *.bak
del *.obj
del *.o
del *.d
del *.lib
del *.a
del *.exe
del test
del pngtest
cd ..\zlib
echo Don't worry about files not found messages
del *.bak
del *.obj
del *.o
del *.d
del *.lib
del *.a
del *.exe
del test
del example
del minigzip
cd ..\msdos\djgpp