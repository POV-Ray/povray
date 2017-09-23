@echo off

REM * Extract POV-Ray version information from `source/base/version.h`
REM *
REM * Run as follows:
REM *
REM *   call "tools/windows/get-source-version.bat" "source/base/version.h"
REM *
REM * This will cause the followng envionment variables to be set:
REM *
REM *   POV_SOURCE_GENERATION   First two fields of the version string (`X.Y`)
REM *   POV_SOURCE_VERSION      Full version string (`X.Y.Z`[`.P`][`-PRE`])
REM *   POV_SOURCE_PRERELEASE   Pre-release tag portion of the version string (`PRE`), or undefined if not applicable

powershell -executionpolicy remotesigned -File "%~dp0get-source-version.ps1" "%~1" -bat version~.bat
call version~.bat
del version~.bat
