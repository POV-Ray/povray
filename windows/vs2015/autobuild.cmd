@ECHO OFF

Title POV-Ray on Windows auto build script

:: This script uses MSBuild to configure and build LPub3D-Trace from the command line.
:: The primary benefit is not having to modify source files before building
:: as described in the official POV-Ray build documentation.
:: It is possible to build either the GUI or CUI project - see usage below.

:: This script is requires autobuild_defs.cmd
:: --
::  Trevor SANDY <trevor.sandy@gmail.com>
::  Last Update: May 19, 2017
::  Copyright (c) 2017 by Trevor SANDY
:: --
:: This script is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

:: It is expected that this script will reside in .\windows\vs2015

:: Variables
SET PLATFORM=unknown
SET CONFIGURATION=unknown
SET PROJECT=unknown
SET CONSOLE=unknown 
SET VERBOSE=unknown

:: Check if invalid platform flag
IF NOT [%1]==[] (
	IF NOT "%1"=="-x86" (
		IF NOT "%1"=="x86_64" (
			IF NOT "%1"=="-help" GOTO :PLATFORM_ERROR
		)
	)
)
:: Parse platform input flag
IF [%1]==[] (
	SET PLATFORM=Win32
	GOTO :SET_CONFIGURATION
)
IF /I "%1"=="x86" (
	SET PLATFORM=Win32
	GOTO :SET_CONFIGURATION
)
IF /I "%1"=="x86_64" (
	SET PLATFORM=x64
	GOTO :SET_CONFIGURATION
)
IF /I "%1"=="-help" (
	GOTO :USAGE
)
:: If we get here display invalid command message.
GOTO :COMMAND_ERROR

:SET_CONFIGURATION
:: Check if invalid configuration flag
IF NOT [%2]==[] (
	IF NOT "%2"=="-rel" (
		IF NOT "%2"=="-avx" (
			IF NOT "%2"=="-sse2" GOTO :CONFIGURATION_ERROR
		)
	)
)
:: Parse configuration input flag
IF [%2]==[] (
	SET CONFIGURATION=Release
	GOTO :BUILD
)
:: Check if no extension release build
IF /I "%2"=="-rel" (
	SET CONFIGURATION=Release
	GOTO :BUILD
)
:: Check if x86_64 and AVX
IF "%PLATFORM%"=="x64" (	
	IF /I "%2"=="-avx" GOTO :SET_AVX
)
:: Check if x86 and SSE2
IF "%PLATFORM%"=="Win32" (	
	IF /I "%2"=="-sse2" GOTO :SET_SSE2
)
:: Check if bad platform and configuration flag combination
IF "%PLATFORM%"=="Win32" (
	IF /I "%2"=="-avx" GOTO :AVX_ERROR
)
IF "%PLATFORM%"=="x64" (	
	IF /I "%2"=="-sse2" GOTO :SSE2_ERROR
)
:: If we get here display invalid command message
GOTO :COMMAND_ERROR

:SET_AVX
:: AVX Configuration
SET CONFIGURATION=Release-AVX
GOTO :BUILD

:SET_SSE2
:: SSE2 Configuration
SET CONFIGURATION=Release-SSE2
GOTO :BUILD

:BUILD
:: Check if invalid console flag
IF NOT [%3]==[] (
	IF NOT "%3"=="-gui" (
		IF NOT "%3"=="-cui" GOTO :PROJECT_ERROR
	)
)
:: Build CUI or GUI project
IF /I "%3"=="-gui" (
	SET CONSOLE=0
	SET PROJECT=povray.sln
) ELSE (
	SET CONSOLE=1
	SET PROJECT=console.vcxproj
)
:: Check if invalid verbose flag
IF NOT [%4]==[] (
	IF NOT "%4"=="-verbose" GOTO :VERBOSE_ERROR
)
:: Enable verbose tracing (useful for debugging)
IF /I "%4"=="-verbose" (
	:: Check if CUI project
	IF NOT "%3"=="-cui" GOTO :VERBOSE_CUI_ERROR
	SET VERBOSE=1 
) ELSE (
	SET VERBOSE=0
)

:: Initialize the Visual Studio command line development environment
:: Note you can change this line to your specific environment - I am using VS2017 here.
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"

:: Display build project message
CALL :PROJECT_MESSAGE %CONSOLE%

:: Display verbosity message
CALL :VERBOSE_MESSAGE %VERBOSE%

:: Set the LPub3D-Trace auto-build pre-processor defines
CALL autobuild_defs.cmd

:: Display the defines set (as environment variables) for MSbuild
ECHO.
ECHO   BUILD_DEFINES.....[%PovBuildDefs%]
:: Display the build configuration and platform settings
ECHO.
ECHO -Building %CONFIGURATION% Configuration for %PLATFORM% Platform...
ECHO.

:: Launch msbuild
msbuild /m /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% %PROJECT%
ECHO.
GOTO :END

:PROJECT_MESSAGE
SET OPTION=Build Graphic User Interface (GUI) solution...
IF %1==1 SET OPTION=Build Console User Interface (CUI) project - Default...
ECHO. 
ECHO -%OPTION%
EXIT /b
	
:VERBOSE_MESSAGE
SET STATE=Verbose tracing is OFF - Default
IF %1==1 SET STATE=Verbose tracing is ON
ECHO. 
ECHO -%STATE%
EXIT /b

:PLATFORM_ERROR
ECHO.
ECHO -(FLAG ERROR) Platform or usage flag is invalid. Use x86 or x86_64. For usage help use -help.
GOTO :USAGE

:CONFIGURATION_ERROR
ECHO.
ECHO -(FLAG ERROR) Configuration flag is invalid. Use -rel, -avx or -sse2 with appropriate platform flag.
GOTO :USAGE

:AVX_ERROR
ECHO.
ECHO -(FLAG ERROR) AVX is not compatable with %PLATFORM% platform. Use -avx only with x86_64 flag.
GOTO :USAGE

:SSE2_ERROR
ECHO.
ECHO -(FLAG ERROR) SSE2 is not compatable with %PLATFORM% platform. Use -sse2 only with x86 flag.
GOTO :USAGE

:PROJECT_ERROR
ECHO.
ECHO -(FLAG ERROR) Project flag is invalid. Use -cui for Console UI or -gui for Graphic UI.
GOTO :USAGE

:VERBOSE_ERROR
ECHO.
ECHO -(FLAG ERROR) Output flag is invalid. Use -verbose.
GOTO :USAGE

:VERBOSE_CUI_ERROR
ECHO.
ECHO -(FLAG ERROR) Output flag can only be used with CUI project. Use -verbose only with -cui flag.
GOTO :USAGE

:COMMAND_ERROR
ECHO.
ECHO -(COMMAND ERROR) Invalid command string.
GOTO :USAGE

:USAGE
ECHO.
ECHO POV-Ray Windows auto build script.
ECHO.
ECHO Use the options below to select between Graphic User Interface (GUI)
ECHO or Console User Interface (CUI) build projects.
ECHO You can also select configuration Advanced Vector Extensions (AVX) 
ECHO for 64bit platforms or Streaming SIMD Extensions 2 (SSE2) for 32bit.
ECHO.
ECHO To run this scrip as is, you must have the following components:
ECHO     - Visual Studio 2017 (I'm using Community Edition here)
ECHO     - Git
ECHO     - Local POV-Ray git repository
ECHO However, you are free to reconfigue this script to use different components.
ECHO.
ECHO Usage:
ECHO autobuild [-help]
ECHO autobuild [x86 ] [-rel ^| sse2] [-cui [-verbose] ^| -gui]
ECHO autobuild [x86_64] [-rel ^| -avx ] [-cui [-verbose] ^| -gui]
ECHO.
ECHO Build 64bit, AVX-Release CUI project example:
ECHO autobuild x86_64 -avx
ECHO.
ECHO Build 64bit, Release, CUI project with verbose output example:
ECHO autobuild x86_64 -rel -cui -verbose
ECHO.
ECHO Build 32bit, Release GUI project example:
ECHO autobuild x86 -rel -gui
ECHO.
ECHO Build 32bit, SSE2-Release GUI project example:
ECHO autobuild x86 -sse2 -gui
ECHO.
ECHO Build 32bit, Release CUI project example:
ECHO autobuild
ECHO.
ECHO Flags:
ECHO  -help.....1.Useage flag - Display useage.
ECHO  x86.......1.Platform flag - [Default] Build 32bit architecture.
ECHO  x86_64....1.Platform flag - Build 64bit architecture.
EChO  -rel......2.Configuration flag - [Default] Release, no extensions (must be preceded by platform flag).
ECHO  -avx......2.Configuraiton flag - AVX-Release, use Advanced Vector Extensions (must be preceded by x86_64 flag).
ECHO  -sse2.....2.Configuration flag - SSE2-Release, use Streaming SIMD Extensions 2 (must be preceded by x86 flag).
ECHO  -cui......3.Project flag - [Default] Build Console User Interface (CUI) project (must be preceded by a configuration flag).
ECHO  -gui......3.Project flag - Build Graphic User Interface (GUI) project (must be preceded by a configuration flag).
ECHO  -verbose..4.Output flag - Display verbose output. Useful for debugging (must be preceded by -cui flag).
ECHO.
ECHO Flags are case sensitive, use lowere case.
ECHO.
ECHO If no flag is supplied, 32bit platform, Release Configuration, CUI project built by default.
EXIT /b

:END
:: Done