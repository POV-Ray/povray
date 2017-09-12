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
#   POV_SOURCE_GENERATION   First two fields of the version string (`X.Y`)
#   POV_SOURCE_VERSION      Full version string (`X.Y.Z`[`.P`][`-PRE`])
#   POV_SOURCE_PRERELEASE   Pre-release tag portion of the version string (`PRE`), or empty[*] if not applicable
#
# [*In the batch version of the procedure, empty environment variables will be left undefined.]

param ([string]$version_h, [string]$bat)

function GetNumericMacro ([string]$file, [string]$macro) {
    $regexp = '^\s*#define\s+' + $macro + '\s+[0-9]+\s*$'
    $line = ( select-string -Path $file -Pattern $regexp | % { $_.Matches } | % { $_.Value } )
    $string = ( select-string -InputObject $line -Pattern '\s+[0-9]+' | % { $_.Matches } | % { $_.Value } )
    $value = ( select-string -InputObject $string -Pattern '[0-9]+' | % { $_.Matches } | % { $_.Value } )
    return [string]$value
}

function GetStringMacro ([string]$file, [string]$macro) {
    $regexp = '^\s*#define\s+' + $macro + '\s*"[^"]+"\s*$'
    $line = ( select-string -Path $file -Pattern $regexp | % { $_.Matches } | % { $_.Value } )
    $string = ( select-string -InputObject $line -Pattern '"[^"]+"' | % { $_.Matches } | % { $_.Value } )
    $value = ( select-string -InputObject $string -Pattern '[^"]+' | % { $_.Matches } | % { $_.Value } )
    return [string]$value
}

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

$env:POV_SOURCE_GENERATION = $generation
$env:POV_SOURCE_VERSION    = $version
$env:POV_SOURCE_PRERELEASE = $prerelease

if ($bat) {
    $text =  "set POV_SOURCE_GENERATION=" + $env:POV_SOURCE_GENERATION + "`n"
    $text += "set POV_SOURCE_VERSION="    + $env:POV_SOURCE_VERSION + "`n"
    $text += "set POV_SOURCE_PRERELEASE=" + $env:POV_SOURCE_PRERELEASE + "`n"
    Out-File -LiteralPath $bat -Encoding ASCII -InputObject $text -NoNewline
}
