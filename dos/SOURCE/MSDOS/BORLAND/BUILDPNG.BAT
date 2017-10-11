@echo off
cd ..\..\zlib
echo Don't worry about files not found messages
del *.bak
del *.obj
del *.o
del *.lib
del *.a
del *.exe
del test
make -fmsdos\makefile.b32
copy *.lib ..\msdos\borland
cd ..\lpng101
make -fscripts\makefile.b32
copy *.lib ..\msdos\borland
echo Don't worry about files not found messages
del *.bak
del *.obj
del *.o
del *.lib
del *.a
del *.exe
del test
cd ..\zlib
echo Don't worry about files not found messages
del *.bak
del *.obj
del *.o
del *.lib
del *.a
del *.exe
del test
cd ..\msdos\borland