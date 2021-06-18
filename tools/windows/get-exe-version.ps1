# Extract formal version information from Windows executable
#
# From PowerShell scripts, run as follows:
#
#   Set-ExecutionPolicy remotesigned -scope process -force
#   ./tools/windows/get-exe-version.ps1 ./windows/vs2015/bin64/pvengine64.exe
#
# From batch files, run as follows:
#
#   powershell -executionpolicy remotesigned -File ./tools/windows/get-exe-version.ps1 ./windows/vs2015/bin64/pvengine64.exe -bat version~.bat
#   call version~.bat
#   del version~.bat
#
# From GitHub Workflow scripts, run as a dedicated step as follows:
#
#   - shell: pwsh
#     run: ./tools/windows/get-exe-version.ps1 ./windows/vs2015/bin64/pvengine64.exe -github_env $env:GITHUB_ENV
#
# All procedures will cause the following envionment variables to be set
# according to the formal version info embedded in the executable:
#
#   BINARY_COMPANYNAME          CompanyName
#   BINARY_FILEDESCRIPTION      FileDescription
#   BINARY_FILEVERSION          FileVersion
#   BINARY_INTERNALNAME         InternalName
#   BINARY_LEGALCOPYRIGHT       LegalCopyright
#   BINARY_LEGALTRADEMARKS      LegalTrademarks
#   BINARY_ORIGINALFILENAME     OriginalFilename
#   BINARY_PRODUCTNAME          ProductName
#   BINARY_PRODUCTVERSION       ProductVersion
#   BINARY_PRODUCTMAJORPART     ProductMajorPart
#   BINARY_PRODUCTMINORPART     ProductMinorPart
#   BINARY_PRODUCTBUILDPART     ProductBuildPart
#   BINARY_PRODUCTPRIVATEPART   ProductPrivatePart
#
# [*In the batch version of the procedure, empty environment variables will be left undefined.]

param ([string]$binary, [string]$bat, [string]$github_env)

$version_info = (get-item $binary).VersionInfo

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

SetVariable 'BINARY_COMPANYNAME'        $version_info.CompanyName
SetVariable 'BINARY_FILEDESCRIPTION'    $version_info.FileDescription
SetVariable 'BINARY_FILEVERSION'        $version_info.FileVersion
SetVariable 'BINARY_INTERNALNAME'       $version_info.InternalName
SetVariable 'BINARY_LEGALCOPYRIGHT'     $version_info.LegalCopyright
SetVariable 'BINARY_LEGALTRADEMARKS'    $version_info.LegalTrademarks
SetVariable 'BINARY_ORIGINALFILENAME'   $version_info.OriginalFilename
SetVariable 'BINARY_PRODUCTNAME'        $version_info.ProductName
SetVariable 'BINARY_PRODUCTVERSION'     $version_info.ProductVersion

SetVariable 'BINARY_PRODUCTMAJORPART'   $version_info.ProductMajorPart
SetVariable 'BINARY_PRODUCTMINORPART'   $version_info.ProductMinorPart
SetVariable 'BINARY_PRODUCTBUILDPART'   $version_info.ProductBuildPart
SetVariable 'BINARY_PRODUCTPRIVATEPART' $version_info.ProductPrivatePart
