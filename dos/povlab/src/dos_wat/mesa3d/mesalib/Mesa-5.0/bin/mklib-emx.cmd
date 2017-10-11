/* mklib-emx.cmd */
/* REXX program to create DLLs for MESA 3.x   */
/* Keep this file in "DOS" text file format!! */

version = "19990515"

TRUE  = 1
FALSE = 0

/*
  Flags
*/

/* echo external commands being executed */
quiet = TRUE

/* get debuginfo about this utility */
DEBUG = FALSE

/* build static libraries
  (well, they're always built, but otherwise deleted ;-) */
BUILD_STATIC = TRUE


/* Here the code really starts */
Parse Arg param Major Minor Tiny Objects
say "mklib-emx version "version

if param = "" then
  do
  say "You shouldn't invoke mklib-emx directly."
  exit
  end

/* We need REXXUtil functions */
if RxFuncQuery('SysLoadFuncs') = 1 then
   do
   if RxFuncAdd('SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs') <> 0 then
      do
      say 'Sorry, your system seems to lack the REXX-Util-library !'
      say program' can not run on this system'
      exit
      end
   else
      call SysLoadFuncs
   end

/* Check for XFree installation. Shall we check for EMX, too ? */
env     = 'OS2ENVIRONMENT'
x11root = VALUE('X11ROOT',,env)
if x11root = "" then
  do
  say "XFree86/2 is not properly installed"
  exit
  end

/* Parse commandline arguments */
Parse Var param base '.' extension
_extension=Strip(Translate(extension))

/* We ignore silently the actual request which format
   the forwarder library should have. We create both
   a.out and OMF style ones */
DynaLibrary = Base".dll"
ImportLibrary_a = Base".a"
ImportLibrary_o = Base".lib"

if DEBUG then
  do
  say "DynaLibrary: "DynaLibrary
  Say "Major: "Major
  Say "Minor: "Minor
  Say "Tiny: "Tiny
  Say "Objects: "Objects
  end

/* Check for old .def file */
DEFFile = Base".def"
olddef = Stream(DEFFile, 'C', 'QUERY EXISTS')
if olddef <> "" then
  do
  tmpfile = SysTempFileName('.\mklib-emx.???')
  call ExecCmd("mv "olddef" "tmpfile)
  say "Renaming old "DEFFile" to "tmpfile
  end

/* Check for old library itself in ../lib-old/ */
/* Check also in %X1ROOT%\XFree86\lib\ ?? */
rc = SysFileTree("..\lib-old\"Base".dll", try_stem, "FO")
if try_stem.0 = 1 then
  do
  say "Old version of library found: "try_stem.1
  end

/* Write out header for the .def file */
call LineOut DEFFile ,,1
call LineOut DEFFile,  'LIBRARY "'Base'"'
call LineOut DEFFile,  'DESCRIPTION "X11R6 XFree86 'DLL' for OS/2 EMX 09c VERSION='major'.'minor'"'
call LineOut DEFFile,  'CODE'
call LineOut DEFFile,  '   PRELOAD'
call LineOut DEFFile,  'DATA'
call LineOut DEFFile,  '   MULTIPLE NONSHARED'
call LineOut DEFFile,  'STACKSIZE 65536'
if try_stem.0 = 1 then
  call LineOut DEFFile,  'OLD "'try_stem.1'"'
call LineOut DEFFile,  'EXPORTS'
call LineOut DEFFile 

/*
  Write out the export list for the DLL.
  Might also have to explicitly deal with compatibility if not
  solved otherwise.
*/
call CreateSymbolList

/* To ease the handling we build archives of the objects
   which might also serve as static libraries.
   In any case we build an OMF style library to
   ensure usage of link386 */
StaticLibrary_a = ""
StaticLibrary_o = ""
try = SubWord(objects, 1, 1)
try_ext= Translate(SubStr(try, LastPos(".", try)))
if try_ext=".OBJ" then
  do
  say "OMF objects"
  call ExecCmd("emxomfar rc "StaticLibrary_o" "objects)
  StaticLibrary_o = base"_s.lib"
  end
else if try_ext=".O" then
  do
  say "a.out objects found. Converting to OMF ..."
  StaticLibrary_a = base"_s.a"
  StaticLibrary_o = base"_s.lib"
  call ExecCmd("ar rc "StaticLibrary_a" "objects)
  call ExecCmd("emxomf -p 32 "StaticLibrary_a)
  end
else
  do
  say "What's that?!" /* shouldn't happen */
  exit
  end

/* 
  Put together all necessary flags for linking
*/
DLLFLAGS = "-s -Zomf -Zdll -Zmt -Zcrtdll -L"x11root"/XFree86/lib -Zlinker /EXEPACK:2 -Zlinker /NOO"
if DEBUG=TRUE then
  DLLFLAGS = DLLFLAGS" -v -Zlinker /I"

/* Add dependencies for the new DLLs */
if Translate(Base) = "MESAGL" then
  do
  DLLLibs = "-lX11"
  DLLGLLIBS = ""
  end
else if Translate(Base) = "MESAGLU" then
  do
  DLLLibs = ""
  DLLGLLIBS = "-L..\lib -lMesaGL"
  end
else if Translate(Base) = "GLUT" then
  do
  DLLLibs = "-lXmu -lXi -lX11"
  DLLGLLIBS = "-L..\lib -lMesaGL -lMesaGLU"
  end
/*
else if Translate(Base) = "GLW" then
  do
  DLLLibs = "-lXt -lX11"
  DLLGLLIBS = "-L..\lib -lMesaGL -lMesaGLU"
  end
*/
else
  do
  say "Unknown library: "Base
  exit
  end

/* Link DLL (using link386 implicitly) */
call ExecCmd("gcc -o "DynaLibrary" "DLLFLAGS" "StaticLibrary_o" "DEFFile" "DLLGLLIBS" "DLLLibs)

/* Link static import libraries */
call ExecCmd("emximp -o "ImportLibrary_a" "DEFFile)
call ExecCmd("emximp -o "ImportLibrary_o" "DEFFile)

/*
  Make sure everything ends up in /lib
*/
call ExecCmd("mv "DynaLibrary" ..\lib")
/* Don't move this one ! The Makefile will do this:
  call ExecCmd("mv "ImportLibrary_a" ..\lib")  */
call ExecCmd("mv "ImportLibrary_o" ..\lib")
call ExecCmd("mv "Base".def ..\lib")
if BUILD_STATIC=TRUE then
  do
  call ExecCmd("mv "StaticLibrary_o" ..\lib")
  if StaticLibrary_a <> "" then
    call ExecCmd("mv "StaticLibrary_a" ..\lib")
  end
else
  do
  call ExecCmd("rm "StaticLibrary_o)
  if StaticLibrary_a <> "" then
    call ExecCmd("rm "StaticLibrary_a)  
  end

/* Ok, done. */
exit

/* ************************ End of main program ************************ */

/* Small procedures */

CreateSymbolList: PROCEDURE Expose Objects DEFFile TRUE FALSE
Olist = Objects
call ExecCmd("emxexp "OList" >>"DEFFile)
return

ExecCmd: PROCEDURE Expose quiet TRUE FALSE
/* Execute a command properly and return it's return value */
Parse Arg cmdstring
ADDRESS CMD
if quiet=TRUE then
"@"cmdstring
else
cmdstring
ADDRESS
return rc
