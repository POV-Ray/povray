@ECHO OFF
Title POV-Ray on Windows auto build script

:: This script uses MSBuild to configure and build POV-Ray from the command line.
:: The primary benefit is not having to modify source files before building
:: as described in the official POV-Ray build documentation.
:: It is possible to build either the GUI or CUI project - see usage below.

:: This script is requires autobuild_defs.cmd
:: --
::  Trevor SANDY <trevor.sandy@gmail.com>
::  Last Update: April 17, 2017
::  Copyright (c) 2017 by Trevor SANDY
:: --
:: This script is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

:: It is expected that this script will reside in .\windows\vs2015

:: Variables
SET PLATFORM=unknown

:: Parse platform input flag
IF "%1"=="x86" (
	SET PLATFORM=Win32
	GOTO :BUILD
)
IF "%1"=="x86_64" (
	SET PLATFORM=x64
	GOTO :BUILD
)
IF [%1]==[] (
	SET PLATFORM=Win32
	GOTO :BUILD
)
:: If we get here display usage.
GOTO :USAGE

:BUILD
:: Initialize the Visual Studio command line development environment

:: Note you can change this line to your specific environment - I am using VS2017 here.
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
:: Set the auto-build POV-Ray defines
IF "%2"=="-cui" (
	:: Console project defines
	CALL autobuild_defs.cmd -c
) ELSE (
	:: GUI project defines
	CALL autobuild_defs.cmd
)

ECHO.
:: Display the defines set (as environment variables) for MSbuild
ECHO   BUILD_DEFINES........[%PovBuildDefs%]
ECHO.
ECHO -Building %PLATFORM% Platform...
ECHO.
IF "%2"=="-cui" (
:: Launch console project to build CUI
msbuild /m /p:Configuration=Release /p:Platform=%PLATFORM% console.vcxproj
) ELSE (
:: Launch povray solution build which defaults to the GUI project 
msbuild /m /p:Configuration=Release /p:Platform=%PLATFORM% povray.sln
)
GOTO :END

:USAGE
ECHO.
ECHO POV-Ray Windows auto build script.
ECHO.
ECHO Use the options below to select between Graphic User Interface (GUI)
ECHO or Console User Interface (CUI) build projects.
ECHO To run this scrip as is, you must have the following components:
ECHO     - Visual Studio 2017 (I'm using Community Edition here)
ECHO     - Git
ECHO     - Local POV-Ray git repository
ECHO However, you are free to reconfigue this script to use different components.
ECHO.
ECHO Use:
ECHO autobuild [x86 ^| x86_64] [-cui]
ECHO.
ECHO Build CUI 64bit, release project example:
ECHO autobuild x86_64 -cui
ECHO.
ECHO Flags:
ECHO  x86.......Platform flag - build 32bit architecture
ECHO  x86_64....Platform flag - build 64bit architecture
ECHO  -cui......Project flag - Build Console User Interface Project (must be preceded by a platform flag) 
ECHO.
ECHO If no flag is supplied, 32bit platform, GUI project built by default.
EXIT /b

:END
:: Done