###################################################################################################
#
# setup.nsi
#
# This file contains the NSIS setup script for the Windows version of POV-Ray.
#
# Compiling this file requires the Nullsoft Scriptable Install System. It has only been tested with
# NSIS v2.46.
#
# Before compiling please read "Customization Notes" below. Once you have set up your distribution
# tree and updated the variable definitions in this file, you may execute "makensis setup.nsi" to
# build the output file.
#
###################################################################################################
#
# Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
# Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
# 
# POV-Ray is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# POV-Ray is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###################################################################################################
#
# Customization Notes:
#
# Please read these instructions in order to customize this install script for your use. Doing so
# involves two main steps: firstly, setting up a install tree containing the files in a particular
# layout, and secondly changing various variables, such as adding your name to the 'Company' field,
# choosing the default destination (please do not use the same default as official POV-Ray), and so
# on. Once you have done all of this you should comment-out the '!error' directive below.
#
# You must set INSTROOT to point at a tree containing a set of files in the layout expected by this 
# script. Basically the tree as presented here is almost identical to what will end up on the target
# system's drive, divided into two sections: core files (usable by everyone) and user-specific files
# (need to be installed for each user). This looks something like the following:
#
#   ./agpl-3.0.txt
#   ./changes.txt
#   ./revision.txt
#   ./core
#     ./core/bin
#     ./core/help
#     ./core/sounds
#     ./core/tiles
#   ./user
#     ./user/include
#     ./user/ini
#     ./user/Insert Menu
#     ./user/scenes
#
# Note in particular that within the ./core/bin directory you must re-name any files that contain
# 'POV-Ray' to contain ${MYABBREV} as defined below; e.g. using the example 'Laser-Ray' abbreviation
# "POV-Ray.Scene.ico" would become "Laser-Ray.Scene.ico". Additionally any files prefixed with
# "pvengine" (e.g. all default EXE's) must have that prefix replaced with ${MYEXEPREFIX}.
#
# Additionally some files not part of the build are added to ./core/bin allow for supporting dumps;
# these are dbghelp.dll and submitminidump.exe. The former is available from Microsoft and may be
# omitted if you do not wish to support writing minidumps if the program crashes, and the latter
# (which uploads minidumps to the POV-Ray bug tracker) is only part of official distributions and
# should never be part of a custom install.
#
# Apart from the above extra DLL's, the only other files needed in ./core/bin are the icons used for
# shortcuts; you may copy these from distribution/platform-specific/windows/icons/ (but be sure to
# re-name them as discussed above).
#
# NB: The path 'distribution/' refers to the directory of that name in the standard POV-Ray source
# tree (on the same level as 'libraries', 'source', 'vfe', 'unix', etc). 
#
#   ./agpl-3.0.txt       copied from distribution/
#   ./changes.txt        copied from the top-level of the source tree
#   ./revision.txt       copied from the top-level of the source tree
#
#   ./core/help/         copied from distribution/platform-specific/windows/help/
#   ./core/sounds/       copied from distribution/platform-specific/windows/sounds/
#   ./core/tiles/        copied from distribution/platform-specific/windows/tiles/
#
# It is safe to omit ./core/sounds/ and ./core/tiles/ if you wish. in particular the tiles are
# rarely used nowadays (defaulting to being off - they are a feature from the mid 1990's).
#
#   ./user/include/      copied from distribution/include/
#   ./user/ini/          copied from distribution/ini/ and distribution/platform-specific/windows/ini/
#   ./user/Insert Menu/  copied from distribution/platform-specific/windows/Insert Menu/
#   ./user/scenes/       copied from distribution/scenes/
#
# Please keep in mind that the compiled executables will look in a particular location in the registry
# for the install path (so that they can find the include and INI files, for example). Generally it
# can work this out automatically but nevertheless it is best to ensure that the registry path compiled
# in is the same as the registry path constructed by this installer. The path is constructed as follows:
#
#   HKCU\SOFTWARE\${MYCOMPANY}\${MYPRODUCT}\${VERSIONSTR}
#
# e.g. given the below defaults, "HKCU\Software\Acme Space Blasters Inc.\Laser-Ray Pro\v1.1". POVWIN
# declares its registry path macros in PVEDIT.H, so look there to see what it is using.
#
# NOTE: If your version acts differently from the official POV-Ray sources from which it is derived
# you are strongly urged to change MYSCENEEXT below to something other than 'pov' (and re-name the
# sample scenes accordingly). Failing to do this could result in your version taking over the .POV
# file association on another users system (presuming that user has already installed POV-Ray). If
# your version is not a 1:1 direct replacement for the official version this may not be desirable.
# In addition, if that user ever installs an official distribution of POV-Ray after having installed
# your renderer, the opposite will happen: POV-Ray will take over the association (reasonable given
# .POV files have been the default extension for POV-Ray for more than 20 years).
#
###################################################################################################
# READ THE ABOVE THEN MODIFY THE FOLLOWING VARIABLES TO SUIT YOUR INSTALLATION.
!error "READ INSTRUCTIONS IN SETUP.NSI TO AVOID THIS ERROR"
!define INSTROOT     "c:\read\setup.nsi\and\change\this"
!define MYCOMPANY    "Acme Space Blasters Inc."
!define MYPRODUCT    "Laser-Ray Pro"
!define MYABBREV     "Laser-Ray"
!define MYVER        "1.1"
!define MYSUBVER     "0.0"
!define MYURL        "http://acme-space-blasters.tycho.mars/"
!define MYCOPYRIGHT  "Copyright(c) 2050-2051 Acme Space Blasters. Inc."
!define MYEXEPREFIX  "lasray"
!define MYSCENEEXT   "pov"
!define MYHELPFILE   "povray.chm"
###################################################################################################

RequestExecutionLevel highest
SetCompressor /SOLID lzma
Name "${MYPRODUCT}"
!define VERSIONSTR "v${MYVER}"
!define BINDEST "${MYCOMPANY}\${MYPRODUCT}\${VERSIONSTR}"
!define DOCDEST "$DOCUMENTS\${MYCOMPANY}\${MYPRODUCT}\${VERSIONSTR}"
!define REGKEY "SOFTWARE\${MYCOMPANY}\${MYPRODUCT}\${VERSIONSTR}"
!define UNINSTALL_REG_SUFFIX "${VERSIONSTR}"
!define URL ${MYURL}

# MUI Symbol Definitions
!define MUI_ICON "${INSTROOT}\Core\Bin\${MYABBREV}.ico"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKCU
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${REGKEY}\Components"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${MYPRODUCT} ${VERSIONSTR}"
!define MUI_UNICON "${INSTROOT}\Core\Bin\${MYABBREV}.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

!define MUI_FINISHPAGE_SHOWREADME "$DocDir\changes.txt"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Display change list."

# Included files
!include Sections.nsh
!include MUI2.nsh
!include x64.nsh
!include FileFunc.nsh

!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of $(^NameDA) ${VERSIONSTR}.$\r$\n$\r$\nPress Next to continue."
!define MUI_UNCONFIRMPAGE_TEXT_TOP "The following $(^NameDA) installation will be uninstalled.$\r$\n$\r$\nNOTE: The scenes, include and INI files associated with the install will not be removed."

# Variables
Var StartMenuGroup
Var TargetEXE
Var BinDir
Var DocDir
var Dialog

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE ${INSTROOT}\agpl-3.0.txt
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE BinDirectoryLeave
!define MUI_DIRECTORYPAGE_VARIABLE $BinDir
!define MUI_DIRECTORYPAGE_TEXT_TOP "Setup will install the core $(^NameDA) files in the following location. Click Browse to select a different folder."
!insertmacro MUI_PAGE_DIRECTORY
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE DocDirectoryLeave
!define MUI_DIRECTORYPAGE_VARIABLE $DocDir
!define MUI_DIRECTORYPAGE_TEXT_TOP "Setup will install the $(^NameDA) include and sample scene files in the following location. Click Browse to select a different folder."
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
Page custom ReadyToInstall
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile "${MYPRODUCT}.${VERSIONSTR}-setup.exe"
InstallDir $BinDir
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion ${MYVER}.${MYSUBVER}
VIAddVersionKey ProductName "${MYPRODUCT}"
VIAddVersionKey ProductVersion "${MYVER}"
VIAddVersionKey CompanyName "${MYCOMPANY}"
VIAddVersionKey CompanyWebsite "${MYURL}"
VIAddVersionKey FileVersion "${MYVER}"
VIAddVersionKey FileDescription "${MYPRODUCT} ${MYVER}"
VIAddVersionKey LegalCopyright "${MYCOPYRIGHT}"
InstallDirRegKey HKCU "${REGKEY}\Windows" Home
ShowUninstDetails show

# Installer sections
Section -Main SEC0000
    SetShellVarContext current

    # remove the registry key, if present
    DeleteRegKey HKCU "${REGKEY}"

    SetOutPath "$BinDir"
    SetOverwrite on
    File ${INSTROOT}\agpl-3.0.txt
    File ${INSTROOT}\revision.txt
    File ${INSTROOT}\changes.txt
    File /r ${INSTROOT}\Core\*
    WriteRegStr HKCU "${REGKEY}\Components" Main 1
SectionEnd

Section "User Files" SEC0001
    SetOutPath "$DocDir"
    SetOverwrite on
    File ${INSTROOT}\agpl-3.0.txt
    File ${INSTROOT}\revision.txt
    File ${INSTROOT}\changes.txt
    File /r ${INSTROOT}\User\*
    WriteRegStr HKCU "${REGKEY}\Components" "User Files" 1
SectionEnd

Section -post SEC0002
    WriteRegStr HKCU "${REGKEY}\Windows" Home "$BinDir"
    WriteRegStr HKCU "${REGKEY}\Windows" DocPath "$DocDir"
    WriteRegDWORD HKCU "${REGKEY}\Windows" FreshInstall 1
    SetOutPath "$BinDir"
    WriteUninstaller $BinDir\${MYEXEPREFIX}-${MYVER}-uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath "$SMPROGRAMS\$StartMenuGroup"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\${MYPRODUCT}.lnk" "$BinDir\bin\$TargetEXE"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\License.lnk" "$BinDir\agpl-3.0.txt"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Documentation.lnk" "$BinDir\help\${MYHELPFILE}"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Release Notes.lnk" "$DocDir\changes.txt"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Change List.lnk" "$DocDir\revision.txt"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Sample Scenes.lnk" "$DocDir\Scenes"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Scene Previews.lnk" "$DocDir\Scenes\index.htm"
    CreateShortcut "$Desktop\${MYABBREV} ${VERSIONSTR}.lnk" "$BinDir\bin\$TargetEXE"
    CreateShortcut "$Desktop\${MYABBREV} ${VERSIONSTR} Examples.lnk" "$DocDir\Scenes"
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name) ${UNINSTALL_REG_SUFFIX}" DisplayName "$(^Name) ${VERSIONSTR}"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name) ${UNINSTALL_REG_SUFFIX}" DisplayVersion "${MYVER}"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name) ${UNINSTALL_REG_SUFFIX}" Publisher "${MYCOMPANY}"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name) ${UNINSTALL_REG_SUFFIX}" URLInfoAbout "${MYURL}"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name) ${UNINSTALL_REG_SUFFIX}" DisplayIcon "$BinDir\${MYEXEPREFIX}-${MYVER}-uninstall.exe"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name) ${UNINSTALL_REG_SUFFIX}" UninstallString "$BinDir\${MYEXEPREFIX}-${MYVER}-uninstall.exe"
    WriteRegDWORD HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name) ${UNINSTALL_REG_SUFFIX}" NoModify 1
    WriteRegDWORD HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name) ${UNINSTALL_REG_SUFFIX}" NoRepair 1

    # set up our file association, backing up previous values if present
    ClearErrors
    ReadRegStr $R0 HKCR ".${MYSCENEEXT}" "backup"
    IfErrors +1 +3
    ReadRegStr $R0 HKCR ".${MYSCENEEXT}" ""
    WriteRegStr HKCR ".${MYSCENEEXT}" "backup" $R0
    WriteRegStr HKCR ".${MYSCENEEXT}" "" "${MYABBREV}.Scene"

    # we don't save the perceived type as it should always be text
    WriteRegStr HKCR ".${MYSCENEEXT}" "PerceivedType" "text"

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene" "backup"
    IfErrors +1 +3
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene" ""
    WriteRegStr HKCR "${MYABBREV}.Scene" "backup" $R0
    WriteRegStr HKCR "${MYABBREV}.Scene" "" "${MYABBREV} scene source file"

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\open" "backup"
    IfErrors +1 +3
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\open" ""
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\open" "backup" $R0
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\open" "" "Edit in ${MYABBREV} ${MYVER}"

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\edit" "backup"
    IfErrors +1 +3
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\edit" ""
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\edit" "backup" $R0
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\edit" "" "Render with ${MYABBREV} ${MYVER}"

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\DefaultIcon" "backup"
    IfErrors +1 +3
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\DefaultIcon" ""
    WriteRegStr HKCR "${MYABBREV}.Scene\DefaultIcon" "backup" $R0
    WriteRegStr HKCR "${MYABBREV}.Scene\DefaultIcon" "" "$BinDir\bin\${MYABBREV}.Scene-XP.ico"

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\open\command" "backup"
    IfErrors +1 +3
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\open\command" ""
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\open\command" "backup" $R0
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\open\command" "" '"$BinDir\bin\$TargetEXE" /edit "%1"'

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\edit\command" "backup"
    IfErrors +1 +3
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\edit\command" ""
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\edit\command" "backup" $R0
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\edit\command" "" '"$BinDir\bin\$TargetEXE" /render "%1"'

    System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKCU "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

Function isEmptyDir
  # Stack ->                    # Stack: <directory>
  Exch $0                       # Stack: $0
  Push $1                       # Stack: $1, $0
  FindFirst $0 $1 "$0\*.*"
  strcmp $1 "." 0 _notempty
    FindNext $0 $1
    strcmp $1 ".." 0 _notempty
      ClearErrors
      FindNext $0 $1
      IfErrors 0 _notempty
        FindClose $0
        Pop $1                  # Stack: $0
        StrCpy $0 1
        Exch $0                 # Stack: 1 (true)
        goto _end
     _notempty:
       FindClose $0
       ClearErrors
       Pop $1                   # Stack: $0
       StrCpy $0 0
       Exch $0                  # Stack: 0 (false)
  _end:
FunctionEnd

Function BinDirectoryLeave
  IfFileExists "$BinDir\*.*" 0 exit
  push $BinDir
  Call isEmptyDir
  Pop $0
  StrCmp $0 0 0 exit
    MessageBox MB_YESNO|MB_ICONEXCLAMATION "The destination folder is not empty.$\r$\nWould you like to use it anyway?" IDYES exit
    Abort
exit:
FunctionEnd

Function DocDirectoryLeave
  IfFileExists "$DocDir\*.*" 0 exit
  push $DocDir
  Call isEmptyDir
  Pop $0
  StrCmp $0 0 0 exit
    MessageBox MB_YESNO|MB_ICONEXCLAMATION "The destination folder is not empty.$\r$\nWould you like to use it anyway?" IDYES exit
    Abort
exit:
FunctionEnd

Function ReadyToInstall
  !insertmacro MUI_HEADER_TEXT "Ready to Install" "Press Install to begin the installation."
  nsDialogs::Create 1018
  Pop $Dialog
  nsDialogs::Show
FunctionEnd

# Uninstaller sections
Section /o "-un.User Files" UNSEC0001
    DeleteRegValue HKCU "${REGKEY}\Components" "User Files"
SectionEnd

Section /o -un.Main UNSEC0000
    RmDir /r $INSTDIR\bin
    RmDir /r $INSTDIR\help
    RmDir /r $INSTDIR\sounds
    RmDir /r $INSTDIR\tiles
    Delete $INSTDIR\changes.txt
    Delete $INSTDIR\revision.txt
    Delete $INSTDIR\agpl-3.0.txt
    RmDir $INSTDIR
    DeleteRegValue HKCU "${REGKEY}\Components" Main
SectionEnd

Section -un.post UNSEC0002
    DeleteRegKey HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name) ${UNINSTALL_REG_SUFFIX}"
    DeleteRegKey HKCU "${REGKEY}"

    # if our backup value exists, write it to the default value
    ClearErrors
    ReadRegStr $R0 HKCR ".${MYSCENEEXT}" "backup"
    IfErrors +3
    WriteRegStr HKCR ".${MYSCENEEXT}" "" $R0
    DeleteRegValue HKCR ".${MYSCENEEXT}" "backup"
    
    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene" "backup"
    IfErrors +3
    WriteRegStr HKCR "${MYABBREV}.Scene" "" $R0
    DeleteRegValue HKCR "${MYABBREV}.Scene" "backup"

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\open" "backup"
    IfErrors +3
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\open" "" $R0
    DeleteRegValue HKCR "${MYABBREV}.Scene\shell\open" "backup"

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\edit" "backup"
    IfErrors +3
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\edit" "" $R0
    DeleteRegValue HKCR "${MYABBREV}.Scene\shell\edit" "backup"

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\DefaultIcon" "backup"
    IfErrors +3
    WriteRegStr HKCR "${MYABBREV}.Scene\DefaultIcon" "" $R0
    DeleteRegValue HKCR "${MYABBREV}.Scene\DefaultIcon" "backup"

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\open\command" "backup"
    IfErrors +3
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\open\command" "" $R0
    DeleteRegValue HKCR "${MYABBREV}.Scene\shell\open\command" "backup"

    ClearErrors
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\edit\command" "backup"
    IfErrors +3
    WriteRegStr HKCR "${MYABBREV}.Scene\shell\edit\command" "" $R0
    DeleteRegValue HKCR "${MYABBREV}.Scene\shell\edit\command" "backup"

    # if the default value is empty or does not exist, delete the key
    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\edit\command" ""
    StrCmp $R0 "" +1 +2
    DeleteRegKey HKCR "${MYABBREV}.Scene\shell\edit\command"

    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\open\command" ""
    StrCmp $R0 "" +1 +2
    DeleteRegKey HKCR "${MYABBREV}.Scene\shell\open\command"

    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\edit" ""
    StrCmp $R0 "" +1 +2
    DeleteRegKey /ifempty HKCR "${MYABBREV}.Scene\shell\edit"

    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\shell\open" ""
    StrCmp $R0 "" +1 +2
    DeleteRegKey /ifempty HKCR "${MYABBREV}.Scene\shell\open"

    ReadRegStr $R0 HKCR "${MYABBREV}.Scene\DefaultIcon" ""
    StrCmp $R0 "" +1 +2
    DeleteRegKey HKCR "${MYABBREV}.Scene\DefaultIcon"

    DeleteRegKey /ifempty HKCR "${MYABBREV}.Scene\shell"
    DeleteRegKey /ifempty HKCR "${MYABBREV}.Scene"

    ReadRegStr $R0 HKCR ".${MYSCENEEXT}" ""
    StrCmp $R0 "" +1 +2
    DeleteRegKey HKCR ".${MYSCENEEXT}"

    Delete "$SMPROGRAMS\$StartMenuGroup\${MYPRODUCT}.lnk"
    Delete "$SMPROGRAMS\$StartMenuGroup\License.lnk"
    Delete "$SMPROGRAMS\$StartMenuGroup\Documentation.lnk"
    Delete "$SMPROGRAMS\$StartMenuGroup\Release Notes.lnk"
    Delete "$SMPROGRAMS\$StartMenuGroup\Change List.lnk"
    Delete "$SMPROGRAMS\$StartMenuGroup\Sample Scenes.lnk"
    Delete "$SMPROGRAMS\$StartMenuGroup\Scene Previews.lnk"
    Delete "$Desktop\${MYABBREV} ${VERSIONSTR}.lnk"
    Delete "$Desktop\${MYABBREV} ${VERSIONSTR} Examples.lnk"
    Delete $INSTDIR\${MYEXEPREFIX}-${MYVER}-uninstall.exe
    RmDir $SMPROGRAMS\$StartMenuGroup
    RmDir $INSTDIR
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir

    ${If} ${RunningX64}
        SetRegView 64
    ${EndIf}

    ReadRegStr $R0 HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name) ${UNINSTALL_REG_SUFFIX}" UninstallString
    StrCmp $R0 "" Proceed
    MessageBox MB_YESNOCANCEL|MB_ICONEXCLAMATION "A copy of $(^Name) is currently installed. Remove it before proceeding?" IDNO Proceed IDYES UninstallRC
    Abort

UninstallRC:
    ClearErrors
    ${GetParent} $R0 $R1
    ExecWait '$R0 _?=$R1'
    Delete $R0
    RmDir $R1

Proceed:
    ${If} ${RunningX64}
        StrCpy $INSTDIR "$PROGRAMFILES64\${BINDEST}"
        StrCpy $TargetEXE "${MYEXEPREFIX}64.exe"
    ${Else}
        StrCpy $INSTDIR "$PROGRAMFILES\${BINDEST}"
        System::Call kernel32::IsProcessorFeaturePresent(i10)i.r0
        ${If} $0 != 0
            StrCpy $TargetEXE "${MYEXEPREFIX}32-sse2.exe"
        ${Else}
            StrCpy $TargetEXE "${MYEXEPREFIX}32.exe"
        ${EndIf}
    ${EndIf}
    StrCpy $BinDir "$INSTDIR"
    StrCpy $DocDir "${DOCDEST}"
FunctionEnd

# Uninstaller functions
Function un.onInit
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
    !insertmacro SELECT_UNSECTION "User Files" ${UNSEC0001}
    ${If} ${RunningX64}
        SetRegView 64
    ${EndIf}
FunctionEnd


