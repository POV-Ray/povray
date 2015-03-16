@echo off

rem set PLANTUML_JAR_PATH=C:\Program Files\PlantUML
set /p POV_VER= < "../../unix/VERSION"

call :DOXYGEN
call :PDFLATEX
echo.
echo done.
goto :EOF

:DOXYGEN
doxygen.exe doxygen.cfg
if errorlevel 1 goto :DOXYGEN_FAIL
goto :EOF

:DOXYGEN_FAIL
echo *** FAILED TO GENERATE SOURCE DOCUMENTATION ***
pause
exit 1

:PDFLATEX
cd latex
call make.bat
cd ..
if not exist "latex\refman.pdf" goto :PDFLATEX_FAIL
if not exist pdf mkdir pdf
copy "latex\refman.pdf" "pdf\POV-Ray Developer's Manual.pdf"
goto :EOF

:PDFLATEX_FAIL
echo *** FAILED TO GENERATE PDF MANUAL ***
pause
goto :EOF
