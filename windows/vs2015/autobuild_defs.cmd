@ECHO OFF
Title LPub3D-Trace on Windows auto build script

:: This script sets the LPub3D-Trace preprocessor defines
:: needed to build the solution/project.

:: This script is intended to be called from autobuild.cmd
:: --
::  Trevor SANDY <trevor.sandy@gmail.com>
::  Last Update: September 23, 2017
::  Copyright (c) 2017 by Trevor SANDY
:: --
:: This script is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

:: It is expected that this script will reside in .\windows\vs2015

:: Variables
SET DEV_ENV=unknown
SET GIT_SHA=unknown
SET VERSION_MAJ=unknown
SET VERSION_MIN=unknown
SET RELEASE=unknown
SET VERSION_H="..\..\source\base\version.h"

:: Get some source details to populate the required defines
:: These are not fixed. You can change as you like
FOR /F "tokens=3*" %%i IN ('FINDSTR /c:"#define POV_RAY_MAJOR_VERSION_INT" %VERSION_H%') DO SET VERSION_MAJ=%%i
FOR /F "tokens=3*" %%i IN ('FINDSTR /c:"#define POV_RAY_MINOR_VERSION_INT" %VERSION_H%') DO SET VERSION_MIN=%%i
FOR /F "tokens=3*" %%i IN ('FINDSTR /c:"#define POV_RAY_PRERELEASE" %VERSION_H%') DO SET RELEASE=%%i
FOR /F "tokens=* USEBACKQ" %%i IN (`git rev-parse --short HEAD`) DO SET GIT_SHA=%%i
FOR /F "tokens=* USEBACKQ" %%i IN (`msbuild -nologo -version`) DO SET DEV_ENV=%%i

:: Remove quotes and trailing space
CALL :CLEAN VERSION_MAJ %VERSION_MAJ%
CALL :CLEAN VERSION_MIN %VERSION_MIN%
CALL :CLEAN RELEASE %RELEASE%

:: Build version number
SET VERSION_BASE="%VERSION_MAJ%.%VERSION_MIN%"
:: POV-Ray documentation would like you to use "YOUR NAME (YOUR EMAIL)" here.
SET BUILT_BY="Autobuild using MSBuild v%DEV_ENV%"
:: Here I use the git sha. You can change if you're not building from a local git repository.
SET BUILD_ID="%GIT_SHA%"

:: Set project build defines - configured to build GUI project at this stage
SET PovBuildDefs=POV_RAY_IS_AUTOBUILD=1;VERSION_BASE=%VERSION_BASE%;POV_RAY_BUILD_ID=%BUILD_ID%;BUILT_BY=%BUILT_BY%;
:: If console variable is not empty append console define to project build defines
IF %CONSOLE%==1 SET PovBuildDefs=%PovBuildDefs%_CONSOLE=1;
:: If verbose variable is not empty append tracing define to project build defines
IF %VERBOSE%==1 SET PovBuildDefs=%PovBuildDefs%WIN_DEBUG=1;

:: Display the define attributes to visually confirm all is well.
ECHO.
ECHO -Build Parameters:
ECHO.
ECHO   VERSION_MAJ.......[%VERSION_MAJ%]
ECHO   VERSION_MIN.......[%VERSION_MIN%]
ECHO   RELEASE...........[%RELEASE%]
ECHO   GIT_SHA...........[%GIT_SHA%]
ECHO   DEV_ENV...........[%DEV_ENV%]
ECHO   VERSION_BASE......[%VERSION_BASE%]
ECHO   BUILD_ID..........[%BUILD_ID%]
ECHO   BUILT_BY..........[%BUILT_BY%]
GOTO :END

:CLEAN
:: A little routine to remove quotes and trailing space
SETLOCAL ENABLEDELAYEDEXPANSION
SET INPUT=%*
SET INPUT=%INPUT:"=%
FOR /F "tokens=1*" %%a IN ("!INPUT!") DO ENDLOCAL & SET %1=%%b
EXIT /b

:END
:: Done
