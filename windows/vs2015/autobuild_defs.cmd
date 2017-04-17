@ECHO OFF
Title POV-Ray auto build defines
::
:: This script sets the POV-Ray preprocessor defines
:: needed to build the solution/project.
::
:: This script is intended to be called from autobuild.cmd
:: --
::  Trevor SANDY <trevor.sandy@gmail.com>
::  Last Update: April 15, 2017
::  Copyright (c) 2017 by Trevor Sandy
:: --
:: This script is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

:: It is expected that this script will reside in .\windows\vs2015
:: Variables
SET DEV_ENV=unknown
SET GIT_SHA=unknown
SET VERSION=unknown
SET RELEASE=unknown
SET CONSOLE=unknown
SET VERFILE="..\..\source\base\version.h"

:: Check if CUI build flag passed in
IF /I "%1"=="-c" GOTO :CUI ELSE GOTO :GUI

:GUI
:: Building GUI project so set Console variable to empty
ECHO.
ECHO -Automatically building GUI project (Default)...
SET CONSOLE=
GOTO :PROCESS_DEFS

:CUI
:: Building CUI project, set Console variable
ECHO. 
ECHO -Automatically building CUI project...
SET CONSOLE=BUILD
GOTO :PROCESS_DEFS 

:PROCESS_DEFS
:: Get some source details to populate the required defines
:: These are not fixed. You can change as you like
FOR /F "tokens=3*" %%i IN ('FINDSTR /c:"#define OFFICIAL_VERSION_STRING" %VERFILE%') DO SET VERSION=%%i
FOR /F "tokens=3*" %%i IN ('FINDSTR /c:"#define POV_RAY_PRERELEASE" %VERFILE%') DO SET RELEASE=%%i
FOR /F "tokens=* USEBACKQ" %%i IN (`git rev-parse --short HEAD`) DO SET GIT_SHA=%%i
FOR /F "tokens=* USEBACKQ" %%i IN (`msbuild -nologo -version`) DO SET DEV_ENV=%%i

:: Remove quotes and trailing space
CALL :CLEAN VERSION %VERSION%
CALL :CLEAN RELEASE %RELEASE%

:: POV-Ray documentation would like you to use "YOUR NAME (YOUR EMAIL)" here.
SET BUILT_BY="Autobuild using MSBuild %DEV_ENV%"
:: Here I use the official version and git sha. You can change if you're not building from a local git repository.
SET BUILD_ID="%VERSION%(Rev: %GIT_SHA%)"

:: Set project build defines - configured to build GUI project at this stage 
SET PovBuildDefs=POV_RAY_IS_AUTOBUILD=1;POV_RAY_BUILD_ID=%BUILD_ID%;BUILT_BY=%BUILT_BY%;
:: Check if console variable is not empty and if not, append console define to project build defines
IF NOT [%CONSOLE%]==[] SET PovBuildDefs=%PovBuildDefs%_CONSOLE=1;

:: Display the define attributes to visually confirm all is well.
ECHO.
ECHO -Build Defines:
ECHO.
ECHO   VERSION...........[%VERSION%]
ECHO   RELEASE...........[%RELEASE%]
ECHO   GIT_SHA...........[%GIT_SHA%]
ECHO   DEV_ENV...........[%DEV_ENV%]
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