@ECHO OFF

Title POV-Ray on Windows auto build script

:: This script uses MSBuild to configure and build POV-Ray from the command line.
:: The primary benefit is not having to modify source files before building
:: as described in the official POV-Ray build documentation.
:: It is possible to build either the GUI or CUI project - see usage below.

:: This script is requires autobuild_defs.cmd
:: --
::  Trevor SANDY <trevor.sandy@gmail.com>
::  Last Update: September 26, 2017
::  Copyright (c) 2017 by Trevor SANDY
:: --
:: This script is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

:: It is expected that this script will reside in .\windows\vs2015

:: Variables
SET VERSION_BASE=3.8

:: Static defaults
SET DEFAULT_PLATFORM=x64
SET PACKAGE=povconsole
SET DEBUG=0

:: Build checks settings - set according to your check requirements - do not add quotes
:: Check 01
::------------------------------------------
rem SET BUILD_CHK_POV_FILE=..\..\distribution\scenes\advanced\biscuit.pov
rem SET BUILD_CHK_MY_OUTPUT=..\..\distribution\scenes\advanced\biscuit
rem SET BUILD_CHK_MY_PARMS=-f +d +p +v +a0.3 +UA +A +w320 +h240
rem SET BUILD_CHK_MY_INCLUDES=

:: Check 03
::------------------------------------------
rem SET BUILD_CHK_MY_POV_FILE=tests\csi.ldr.pov
rem SET BUILD_CHK_MY_OUTPUT=tests\csi.ldr
rem SET BUILD_CHK_MY_PARMS=+d +a0.3 +UA +A +w2549 +h1650
rem SET BUILD_CHK_MY_INCLUDES=+L%USERPROFILE%\LDraw\lgeo\ar +L%USERPROFILE%\LDraw\lgeo\lg +L%USERPROFILE%\LDraw\lgeo\stl

:: Check 02
::------------------------------------------
SET BUILD_CHK_MY_POV_FILE=tests\space in dir name test\biscuit.pov
SET BUILD_CHK_MY_OUTPUT=tests\space in dir name test\biscuit space in file name test
SET BUILD_CHK_MY_PARMS=+d -p +a0.3 +UA +A +w320 +h240
SET BUILD_CHK_MY_INCLUDES=

:: Build check static settings - do not modify these.
SET BUILD_CHK_OUTPUT=%BUILD_CHK_MY_OUTPUT%
SET BUILD_CHK_POV_FILE=%BUILD_CHK_MY_POV_FILE%
SET BUILD_CHK_PARAMS=%BUILD_CHK_MY_PARMS%
SET BUILD_CHK_INCLUDE=+L"..\..\distribution\ini" +L"..\..\distribution\include" +L"..\..\distribution\scenes"
SET BUILD_CHK_INCLUDE=%BUILD_CHK_INCLUDE% %BUILD_CHK_MY_INCLUDES%

:: Visual Studio 'debug' comand line: +I"tests\space in dir name test\biscuit.pov" +O"tests\space in dir name test\biscuit space in file name test.png" +w320 +h240 +d -p +a0.3 +UA +A +L"..\..\distribution\ini" +L"..\..\distribution\include" +L"..\..\distribution\scenes"

SET CONFIGURATION=unknown
SET PLATFORM=unknown
SET PROJECT=unknown
SET CONSOLE=unknown
SET VERBOSE=unknown
SET REBUILD=unknown
SET CHECK=unknown

IF %DEBUG%==1 (
	SET d=d
	SET DEFAULT_CONFIGURATION=Debug
) ELSE (
  SET d=
  SET DEFAULT_CONFIGURATION=Release
)

:: Check if invalid platform flag
IF NOT [%1]==[] (
	IF NOT "%1"=="x86" (
		IF NOT "%1"=="x86_64" (
			IF NOT "%1"=="-allcui" (
				IF NOT "%1"=="-run" (
					IF NOT "%1"=="-rbld" (
						IF NOT "%1"=="-verbose" (
							IF NOT "%1"=="-help" GOTO :PLATFORM_ERROR
						)
					)
				)
			)
		)
	)
)
:: Parse platform input flag
IF [%1]==[] (
	SET PLATFORM=-allcui
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
IF /I "%1"=="-allcui" (
	SET PLATFORM=-allcui
	GOTO :SET_CONFIGURATION
)
IF /I "%1"=="-run" (
	GOTO :SET_CONFIGURATION
)
IF /I "%1"=="-rbld" (
	SET PLATFORM=-allcui
	GOTO :SET_CONFIGURATION
)
IF /I "%1"=="-verbose" (
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
		IF NOT "%2"=="-dbg" (
			IF NOT "%2"=="-avx" (
				IF NOT "%2"=="-chk" (
					IF NOT "%2"=="-run" (
						IF NOT "%2"=="-rbld" (
							IF NOT "%2"=="-sse2" GOTO :CONFIGURATION_ERROR
						)
					)
				)
			)
		)
	)
)
::  Set the default platform
IF "%PLATFORM%"=="unknown" (
	SET PLATFORM=%DEFAULT_PLATFORM%
)
:: Run a render check without building
IF /I "%1"=="-run" SET RUN_CHK=true
IF /I "%2"=="-run" SET RUN_CHK=true
IF /I "%RUN_CHK%"=="true" (
	SET CONFIGURATION=run render only
	CALL :CHECK_BUILD %PLATFORM%
	:: Finish
	EXIT /b
)
:: Perform verbose (debug) build
IF "%1"=="-verbose" (
	SET CHECK=1
	SET CONFIGURATION=%DEFAULT_CONFIGURATION%
	GOTO :BUILD
)
:: Parse configuration input flag
IF /I "%1"=="-rbld" SET REBUILD_CHK=true
IF /I "%2"=="-rbld" SET REBUILD_CHK=true
IF /I "%REBUILD_CHK%"=="true" (
	SET REBUILD=1
	SET CHECK=1
	SET CONFIGURATION=%DEFAULT_CONFIGURATION%
	GOTO :BUILD
)
:: Check if release build
IF /I "%2"=="-rel" (
	SET CONFIGURATION=Release
	GOTO :BUILD
)
:: Check if debug build
IF /I "%2"=="-dbg" (
	SET CONFIGURATION=Debug
	GOTO :BUILD
)
:: Build and run an image render check
IF /I "%2"=="-chk" (
	SET CHECK=1
	SET CONFIGURATION=%DEFAULT_CONFIGURATION%
	GOTO :BUILD
)
:: Parse configuration input flag
IF [%2]==[] (
	SET CHECK=1
	SET CONFIGURATION=%DEFAULT_CONFIGURATION%
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
:: Configure build parameters
SET DO_REBUILD=
SET BUILD_LBL=Building
IF %REBUILD%==1 (
	SET DO_REBUILD=/t:Rebuild
	SET BUILD_LBL=Rebuilding
)
:: Check if invalid console flag
IF NOT [%3]==[] (
	IF NOT "%3"=="-gui" (
		IF NOT "%3"=="-cui" GOTO :PROJECT_ERROR
	)
)
:: Build CUI or GUI project - CUI is default
:: Parse configuration input flag
IF [%3]==[] (
	SET CONSOLE=1
	SET PROJECT=console.vcxproj
)
IF /I "%3"=="-gui" (
	SET CONSOLE=0
	SET PROJECT=povray.sln
)
IF /I "%3"=="-cui" (
	SET CONSOLE=1
	SET PROJECT=console.vcxproj
)
:: Check if invalid verbose flag
IF NOT [%4]==[] (
	IF NOT "%4"=="-verbose" GOTO :VERBOSE_ERROR
)
:: Enable verbose tracing (useful for debugging)
IF /I "%1"=="-verbose" SET VERBOSE_CHK=true
IF /I "%4"=="-verbose" SET VERBOSE_CHK=true
IF "%CONFIGURATION%"=="Debug" SET VERBOSE_CHK=true
IF /I "%VERBOSE_CHK%"=="true" (
	:: Check if CUI or allCUI project build
	IF NOT %CONSOLE%==1 (
		IF NOT "%PLATFORM%"=="-allcui" GOTO :VERBOSE_CUI_ERROR
	)
	SET VERBOSE=1
) ELSE (
	SET VERBOSE=0
)
:: Initialize the Visual Studio command line development environment
:: Note you can change this line to your specific environment - I am using VS2017 here.
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
:: Set the LPub3D-Trace auto-build pre-processor defines
CALL autobuild_defs.cmd
:: Display the defines set (as environment variable 'PovBuildDefs') for MSbuild
ECHO.
ECHO   BUILD_DEFINES.....[%PovBuildDefs%]

:: Display build project message
CALL :PROJECT_MESSAGE %CONSOLE%

:: Display verbosity message
CALL :VERBOSE_MESSAGE %VERBOSE%

:: Console logging flags (see https://docs.microsoft.com/en-us/visualstudio/msbuild/msbuild-command-line-reference)
SET LOGGING=/clp:ErrorsOnly /nologo

:: Check if build all platforms
IF /I "%PLATFORM%"=="-allcui" (
	SET CONSOLE=1
	SET PROJECT=console.vcxproj
	SET CONFIGURATION=%DEFAULT_CONFIGURATION%
	GOTO :BUILD_ALL_CUI
)

:: Assemble command line
SET COMMAND_LINE=msbuild /m /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% %PROJECT% %LOGGING% %DO_REBUILD%
ECHO   BUILD_COMMAND.....[%COMMAND_LINE%]
:: Display the build configuration and platform settings
ECHO.
ECHO -%BUILD_LBL% %CONFIGURATION% Configuration for %PLATFORM% Platform...
ECHO.
:: Launch msbuild
%COMMAND_LINE%
:: Perform build check if specified
IF %CHECK%==1 CALL :CHECK_BUILD %PLATFORM%
:: Finish
EXIT /b

:BUILD_ALL_CUI
:: Display the build configuration and platform settings
ECHO.
ECHO -%BUILD_LBL% all CUI Platforms for %CONFIGURATION% Configuration...
:: Launch msbuild across all CUI platform builds
FOR %%P IN ( Win32, x64 ) DO (
	SETLOCAL ENABLEDELAYEDEXPANSION
	:: Assemble command line
	SET COMMAND_LINE=msbuild /m /p:Configuration=%CONFIGURATION% /p:Platform=%%P %PROJECT% %LOGGING% %DO_REBUILD%
	ECHO.
	ECHO --%BUILD_LBL% %%P Platform...
	ECHO.
	ECHO   BUILD_COMMAND.....[!COMMAND_LINE!]
	ECHO.
	:: Launch msbuild
	!COMMAND_LINE!
	:: Perform build check if specified
	IF %CHECK%==1 CALL :CHECK_BUILD %%P
	ENDLOCAL
)
:: Finish
EXIT /b


:PROJECT_MESSAGE
SET OPTION=%BUILD_LBL% Graphic User Interface (GUI) solution...
IF %1==1 SET OPTION=%BUILD_LBL% Console User Interface (CUI) project - Default...
ECHO.
ECHO -%OPTION%
EXIT /b

:VERBOSE_MESSAGE
SET STATE=Verbose (%CONFIGURATION%) tracing is OFF - Default
IF %1==1 SET STATE=Verbose (%CONFIGURATION%) tracing is ON
ECHO.
ECHO -%STATE%
EXIT /b

:CHECK_BUILD
IF %1==Win32 SET PL=32
IF %1==x64 SET PL=64
ECHO.
ECHO --Check %CONFIGURATION% Configuration, %PL%bit Platform...
SET BUILD_CHK_COMMAND=+I"%BUILD_CHK_POV_FILE%" +O"%BUILD_CHK_OUTPUT%.%PL%bit.png" %BUILD_CHK_PARAMS% %BUILD_CHK_INCLUDE%
ECHO   RUN_COMMAND.......[%PACKAGE%%PL%%d%.exe %BUILD_CHK_COMMAND%]
ECHO.
IF EXIST "%BUILD_CHK_OUTPUT%" (
	DEL /Q "%BUILD_CHK_OUTPUT%"
)
bin%PL%\%PACKAGE%%PL%%d%.exe %BUILD_CHK_COMMAND%
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
ECHO.
ECHO Help...
ECHO autobuild [ -help ]
ECHO.
ECHO First position flags...
ECHO autobuild [ x86 ^| x86_64 ^| -allcui ^| -run ^| -rbld ^| -verbose ^| -help]
ECHO.
ECHO All flags, 1st, 2nd, 3rd and 4th...
ECHO autobuild [ x86 ^| x86_64 ^| -allcui ^| -run ^| -rbld ^| -verbose ^| -help]
ECHO           [ -rel ^| -dgb ^| -chk ^| -run ^| -rbld ^| -avx ^| sse2]
ECHO           [ -cui ^| -gui]
ECHO           [ -verbose ]
ECHO.
ECHO Build 64bit, Release and perform build check
ECHO autobuild x86_64 -chk
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
ECHO ----------------------------------------------------------------
ECHO ^| Flag    ^| Pos ^| Type             ^| Description
ECHO ----------------------------------------------------------------
ECHO  -help......1.....Useage flag        [Difault=Off] Display useage.
ECHO  x86........1.....Platform flag      [Default=On ] Build 32bit architecture.
ECHO  x86_64.....1.....Platform flag      [Default=On ] Build 64bit architecture.
ECHO  -allcui....1.....Project flag       [Default=On ] Build and install 32bit, 64bit, CUI configurations.
ECHO  -run.......2,1...Project flag       [Default=Off] Run an image redering check - must be preceded by x86 or x86_64 flag.
ECHO  -rbld......2,1...Project flag       [Default=Off] Rebuild project - clane and rebuild all project components.
EChO  -rel.......2.....Configuration flag [Default=On ] Release build.
EChO  -dgb.......2.....Configuration flag [Default=Off] Debug build.
ECHO  -avx.......2.....Configuraiton flag [Default=Off] AVX-Release, use Advanced Vector Extensions (must be preceded by x86_64 flag).
ECHO  -sse2......2.....Configuration flag [Default=Off] SSE2-Release, use Streaming SIMD Extensions 2 (must be preceded by x86 flag).
ECHO  -chk.......2.....Project flag       [Default=On ] Build and run an image redering check.
ECHO  -cui.......3.....Project flag       [Default=On ] Build Console User Interface (CUI) project (must be preceded by a configuration flag).
ECHO  -gui.......3.....Project flag       [Default=Off] Build Graphic User Interface (GUI) project (must be preceded by a configuration flag).
ECHO  -verbose...4,1...Project flag       [Default=Off] Display verbose output. Useful for debugging (must be preceded by -cui flag).
ECHO.
ECHO Flags are case sensitive, use lowere case.
ECHO.
ECHO If no flag is supplied, 32bit platform, Release Configuration, CUI project built by default.
EXIT /b

:END
EXIT /b
:: Done
