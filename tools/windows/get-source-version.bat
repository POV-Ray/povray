@echo off

REM * Extract POV-Ray version information from `source/base/version.h`
REM *
REM * Run as follows:
REM *
REM *   call "tools/windows/get-source-version.bat" "source/base/version.h"
REM *
REM * This will cause the following envionment variables to be set:
REM *
REM *   POV_RAY_COPYRIGHT       Copyright string
REM *   POV_RAY_GENERATION      First two fields of the version string (`X.Y`)
REM *   POV_RAY_FULL_VERSION    Full version string (`X.Y.Z`[`.P`][`-PRE`])
REM *   POV_RAY_PRERELEASE      Pre-release tag portion of the version string (`PRE`), or undefined if not applicable
REM *
REM * For backward compatibility with earlier versions of the script, the following (deprecated)
REM * environment variables will also be set:
REM *
REM *   POV_SOURCE_COPYRIGHT    same as POV_RAY_COPYRIGHT
REM *   POV_SOURCE_GENERATION   same as POV_RAY_GENERATION
REM *   POV_SOURCE_VERSION      same as POV_RAY_FULL_VERSION
REM *   POV_SOURCE_PRERELEASE   same as POV_RAY_PRERELEASE

powershell -executionpolicy remotesigned -File "%~dp0get-source-version.ps1" "%~1" -bat version~.bat
call version~.bat
del version~.bat
