# Extract POV-Ray version information from `source/base/version.h`
#
# From PowerShell scripts, run as follows:
#
#   Set-ExecutionPolicy remotesigned -scope process -force
#   ./tools/windows/get-source-version.ps1 ./source/base/version.h
#
# From batch files, run as follows:
#
#   powershell -executionpolicy remotesigned -File ./tools/windows/get-source-version.ps1 ./source/base/version.h -bat version~.bat
#   call version~.bat
#   del version~.bat
#
# Both procedures will cause the following envionment variables to be set:
#
#   POV_RAY_COPYRIGHT       Copyright string
#   POV_RAY_GENERATION      First two fields of the version string (`X.Y`)
#   POV_RAY_FULL_VERSION    Full version string (`X.Y.Z`[`.P`][`-PRE`])
#   POV_RAY_PRERELEASE      Pre-release tag portion of the version string (`PRE`), or empty if not applicable [*]
#
# For backward compatibility with earlier versions of the script, the following (deprecated)
# environment variables will also be set:
#
#   POV_SOURCE_COPYRIGHT    same as POV_RAY_COPYRIGHT
#   POV_SOURCE_GENERATION   same as POV_RAY_GENERATION
#   POV_SOURCE_VERSION      same as POV_RAY_FULL_VERSION
#   POV_SOURCE_PRERELEASE   same as POV_RAY_PRERELEASE [*]
#
# [*In the batch version of the procedure, empty environment variables will be left undefined.]

param ([string]$version_h, [string]$bat)

function GetMacro ([string]$file, [string]$macro, [string]$pattern1, [string]$pattern2, [string]$pattern3) {
    $regexp = '^\s*#define\s+' + $macro + $pattern1 + '\s*$'
    $line = ( select-string -Path $file -Pattern $regexp | % { $_.Matches } | % { $_.Value } )
    $string = ( select-string -InputObject $line -Pattern $pattern2 | % { $_.Matches } | % { $_.Value } )
    $value = ( select-string -InputObject $string -Pattern $pattern3 | % { $_.Matches } | % { $_.Value } )
    return [string]$value
}

function GetNumericMacro ([string]$file, [string]$macro) {
    GetMacro $file $macro '\s+[0-9]+' '\s+[0-9]+' '[0-9]+'
}

function GetStringMacro ([string]$file, [string]$macro) {
    GetMacro $file $macro '\s*"[^"]+"' '"[^"]+"' '[^"]+'
}

$copyright  = GetStringMacro  $version_h 'POV_RAY_COPYRIGHT'

$major      = GetNumericMacro $version_h 'POV_RAY_MAJOR_VERSION_INT'
$minor      = GetNumericMacro $version_h 'POV_RAY_MINOR_VERSION_INT'
$revision   = GetNumericMacro $version_h 'POV_RAY_REVISION_INT'
$patchlevel = GetNumericMacro $version_h 'POV_RAY_PATCHLEVEL_INT'

$prerelease = GetStringMacro  $version_h 'POV_RAY_PRERELEASE'

$generation = $major + "." + $minor
if ([int]$patchlevel -eq 0) {
    $release = $generation + "." + $revision
} else {
    $release = $generation + "." + $revision + "." + $patchlevel
}
if ($prerelease) {
    $version = $release + '-' + $prerelease
} else {
    $version = $release
}

if ($bat) {

    $text =  ""

    $text += "set POV_RAY_COPYRIGHT="       + $copyright + "`n"
    $text += "set POV_RAY_GENERATION="      + $generation + "`n"
    $text += "set POV_RAY_FULL_VERSION="    + $version + "`n"
    $text += "set POV_RAY_PRERELEASE="      + $prerelease + "`n"

    $text += "set POV_SOURCE_COPYRIGHT="    + $copyright + "`n"
    $text += "set POV_SOURCE_GENERATION="   + $generation + "`n"
    $text += "set POV_SOURCE_VERSION="      + $version + "`n"
    $text += "set POV_SOURCE_PRERELEASE="   + $prerelease + "`n"

    Out-File -LiteralPath $bat -Encoding ASCII -InputObject $text -NoNewline

} else {

    $env:POV_RAY_COPYRIGHT      = $copyright
    $env:POV_RAY_GENERATION     = $generation
    $env:POV_RAY_FULL_VERSION   = $version
    $env:POV_RAY_PRERELEASE     = $prerelease

    $env:POV_SOURCE_COPYRIGHT   = $copyright
    $env:POV_SOURCE_GENERATION  = $generation
    $env:POV_SOURCE_VERSION     = $version
    $env:POV_SOURCE_PRERELEASE  = $prerelease

}

