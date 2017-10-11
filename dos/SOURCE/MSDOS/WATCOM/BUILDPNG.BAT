@echo off
cd ..\..\zlib
echo Don't worry about files not found messages
del *.bak
del *.obj
del *.o
del *.lib
del *.a
del *.exe
del *.err
del test
wmake -f msdos\makefile.wat
copy *.lib ..\msdos\watcom
cd ..\lpng101
wmake -f scripts\makefile.wat
copy *.lib ..\msdos\watcom
echo Don't worry about files not found messages
del *.bak
del *.obj
del *.o
del *.lib
del *.a
del *.exe
del *.err
del test
cd ..\zlib
echo Don't worry about files not found messages
del *.bak
del *.obj
del *.o
del *.lib
del *.a
del *.exe
del *.err
del test
cd ..\msdos\watcom