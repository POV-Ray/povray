@echo off

REM *** Prepare Clean Slate **************************************************

rmdir /s /q "output"
mkdir "output"

REM *** Create TOC and Index *************************************************

echo Preparing TOC and index...

REM prepare a list of HTML files (required by the perl script)
dir /b /o:n "input\index.htm*" > filelist.txt"
dir /b /o:n "input\?1*.htm*" >> filelist.txt"
dir /b /o:n "input\?2*.htm*" >> filelist.txt"
dir /b /o:n "input\?3*.htm*" >> filelist.txt"

makedocs.pl

echo.

REM *** Copy Auxiliary Files *************************************************

echo Copying auxiliary files...

xcopy "include" "output"
REM copy "input\povray37.css" "output"
xcopy /i /s "input\images" "output\images"

echo.

REM *** Create Compressed Help File ******************************************

echo Creating compressed help file...

hhc.exe "output\povray37.hhp"

echo.

REM *** Copy To Final Location ***********************************************

echo Copying result...

copy "output\povray37.chm" "..\..\..\distribution\platform-specific\windows\help\povray37.chm"

echo.

REM **************************************************************************

pause
