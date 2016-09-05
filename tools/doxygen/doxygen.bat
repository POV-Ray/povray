@echo off

if "%~1" == "" goto :PARAMETER_ERROR

rem set PLANTUML_JAR_PATH=C:\Program Files\PlantUML
set /p POV_VER= < "../../unix/VERSION"

call :JAVA_SYMLINK_FIX

pushd "../.."
doxygen.exe "tools/doxygen/%~1.cfg"
if errorlevel 1 goto :DOXYGEN_FAIL
popd

call :PDFLATEX "%~1" "%~2"
call :HTMLHELP "%~1"

echo.
echo done.
goto :EOF

:PARAMETER_ERROR
echo This batch file is no longer intended to be run directly.
echo To generate source documentation, run source-doc.bat instead.
pause
goto :EOF

:DOXYGEN_FAIL
popd
echo *** FAILED TO GENERATE SOURCE DOCUMENTATION ***
pause
goto :EOF


rem ------------------------------------------------------------------------------------------------

:PDFLATEX
if "%~2" == "" goto :EOF
call "%~1\latex\make.bat"
if not exist "%~1\latex\refman.pdf" goto :PDFLATEX_FAIL
if not exist "%~dp2" mkdir "%~dp2"
copy "%~1\latex\refman.pdf" "%~2"
goto :EOF

:PDFLATEX_FAIL
echo *** FAILED TO GENERATE PDF MANUAL ***
pause
goto :EOF


rem ------------------------------------------------------------------------------------------------

:HTMLHELP
if not exist "%~1\html\index.hhp" goto :EOF
call :CLEANUP_FILE "%~1\html\*.chm"
hhc.exe "%~1\html\index.hhp"
if not exist "%~1\html\*.chm" goto :HTMLHELP_FAIL
goto :EOF

:HTMLHELP_FAIL
echo.
echo *** FAILED TO GENERATE HTML HELP FILE ***
pause
goto :EOF


rem ------------------------------------------------------------------------------------------------

:CLEANUP_FILE
if exist "%~1" del "%~1"
goto :EOF


rem ------------------------------------------------------------------------------------------------

rem   Current (2015) java version istallers don't include the true location of the java executable
rem   (.exe) files in the path variable, and instead include a standard directory
rem   (%PROGRAMDATA%\Oracle\Java\javapath), which in turn contains symbolic links to the actual executables.
rem   However, some recent (2015) Windows security updates have blocked certain ways to execute symbolic links;
rem   unfortunately, Doxygen uses exactly one such blocked way to execute java in order to run PlantUML.
rem   The following code works around this by looking up the directory the symlink points to, and prepending that
rem   directory to the path variable.

:JAVA_SYMLINK_FIX

rem set JAVA_EXE to whatever java.exe is found via the path
call :FIND_IN_PATH JAVA_EXE java.exe
rem set TRUE_JAVA_EXE to the actual file java.exe points to (if it is a symlink), or an empty string otherwise
call :FIND_SYMLINK TRUE_JAVA_EXE "%JAVA_EXE%"
rem if the java.exe found via the path is not a symlink, we're done
if "%TRUE_JAVA_EXE%" == "" goto :EOF
echo "%JAVA_EXE%" is actually a symlink pointing to "%TRUE_JAVA_EXE%"
rem set JAVA_EXE_DIR to the directory in which the actual java.exe resides
call :SET_DIR JAVA_EXE_DIR "%TRUE_JAVA_EXE%"
rem prepend JAVA_EXE_DIR to the path variable
set PATH=%JAVA_EXE_DIR%;%PATH%
goto :EOF

:FIND_IN_PATH
set %1=%~dp$PATH:2%2
goto :EOF

:FIND_SYMLINK
rem Sets the environment variable named by %1 to the file pointed to by %2 if that's a symlink.
rem Sets the environment variable to an empty string otherwise.
set %1=
for /F "usebackq tokens=2 delims=[]" %%i in (`dir "%~2" /N`) do set %1=%%i
goto :EOF

:SET_DIR
rem sets the environment variable named by %1 to the directory of the file named by %2.
set %1=%~dp2
goto :EOF

rem ************************************************************************************************
rem End of File.
