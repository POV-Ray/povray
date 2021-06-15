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
# From GitHub Workflow scripts, run as a dedicated step as follows:
#
#   - shell: pwsh
#     run: ./tools/windows/get-source-version.ps1 ./source/base/version.h -github_env $env:GITHUB_ENV
#
# All procedures will cause the following envionment variables to be set:
#
#   POV_RAY_COPYRIGHT       Copyright string
#   POV_RAY_GENERATION      First two fields of the version string (`X.Y`)
#   POV_RAY_FULL_VERSION    Full version string (`X.Y.Z`[`.P`][`-PRE`])
#   POV_RAY_PRERELEASE      Pre-release tag portion of the version string (`PRE`), or empty if not applicable [*]
#   POV_RAY_HOST_VERSION    First two fields of the "host" version string (`V.W`), or empty if not applicable [*]
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

param ([string]$version_h, [string]$bat, [string]$github_env)

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

$copyright      = GetStringMacro  $version_h 'POV_RAY_COPYRIGHT'

$major          = GetNumericMacro $version_h 'POV_RAY_MAJOR_VERSION_INT'
$minor          = GetNumericMacro $version_h 'POV_RAY_MINOR_VERSION_INT'
$revision       = GetNumericMacro $version_h 'POV_RAY_REVISION_INT'
$patchlevel     = GetNumericMacro $version_h 'POV_RAY_PATCHLEVEL_INT'

$prerelease     = GetStringMacro  $version_h 'POV_RAY_PRERELEASE'
$hostversion    = GetStringMacro  $version_h 'POV_RAY_HOST_VERSION'

$generation = $major + '.' + $minor
if ([int]$patchlevel -eq 0) {
    $release = $generation + '.' + $revision
} else {
    $release = $generation + '.' + $revision + '.' + $patchlevel
}
if ($prerelease) {
    $version = $release + '-' + $prerelease
} else {
    $version = $release
}

if ($bat) {

    function SetVariable ([string]$name, [string]$value) {
        ('set ' + $name + '=' + $value) | Out-File -FilePath $bat -Encoding ASCII -Append
    }

} elseif ($github_env) {

    function SetVariable ([string]$name, [string]$value) {
        ($name + '=' + $value) | Out-File -FilePath $github_env -Encoding UTF8 -Append
    }

} else {

    function SetVariable ([string]$name, [string]$value) {
        $expr = '$env:' + $name + ' = "' + $value + '"'
        Invoke-Expression $expr
    }

}

SetVariable 'POV_RAY_COPYRIGHT'     $copyright
SetVariable 'POV_RAY_GENERATION'    $generation
SetVariable 'POV_RAY_FULL_VERSION'  $version
SetVariable 'POV_RAY_PRERELEASE'    $prerelease
SetVariable 'POV_RAY_HOST_VERSION'  $hostversion

SetVariable 'POV_SOURCE_COPYRIGHT'  $copyright
SetVariable 'POV_SOURCE_GENERATION' $generation
SetVariable 'POV_SOURCE_VERSION'    $version
SetVariable 'POV_SOURCE_PRERELEASE' $prerelease
