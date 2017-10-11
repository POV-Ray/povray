/* REXX Program to make WPS object for POVLAB program */

/* Clearscreen */
'cls'

/* Intro */
say ""
say "Welcome to POVLAB's OS/2 setup program."
say ""
say "POVLAB is a DOS based modelling program"
say "for the Persistance of Vision Raytracer."
say ""
say "This install routine will create a WPS"
say "icon on the desktop with the required"
say "settings...."

/* Required system fuctions */
Call rxfuncadd sysloadfuncs, rexxutil, sysloadfuncs
Call sysloadfuncs

/* Get path */
fullpath=Directory()
if right(fullpath, 1) \= "\" then do
    fullpath = fullpath || "\"
end

/* Ask if "fullpath" is where POVLAB is located */
say ""
say "Is" fullpath
reply = GetYesNo("the directory where you unzipped POVLAB")

/* If no then leave */
if \reply then do
    say ""
    say "Please run the install program from a fullscreen DOS"
    say "prompt and then rerun this program from an OS/2 shell"
    say "within the POVLAB directory."
    say ""
    say "Exiting setup....."
    exit
    end

/* Otherwise explain the procedure */
say ""
say "POVLAB's setup program will now create a POVLAB icon on"
say "the Desktop, complete with associations....."
call SysSleep 3

/* Check for POVLAB executable */
drop res.
call SysFileTree fullpath || "POVLAB.EXE", res, "OF"

/* Not found */
if res.1 = "RES.1" then do
    say ""
    say "Can't find the POVLAB executable in the specified directory."
    say "Please install POVLAB, switch to the install directory and"
    say "rerun this setup program."
    say ""
    say "Exiting setup....."
    exit
end

/* Found */
else do

    /* Copy "povlabic.os2" to "povlab.ico" */
    'echo off'
    'copy povlabic.os2 povlab.ico'
    'cls'

    /* Setup associations and create WPS object */
    assoc = '*.SCN'
    Call SysCreateObject 'WPProgram','POVLAB','<WP_DESKTOP>','CCVIEW=NO;EXENAME='||fullpath||'\POVLAB.EXE;PROGTYPE=VDM;STARTUPDIR='||fullpath||';SET COM_HOLD=OFF;SET COM_SELECT=NONE;SET DPMI_DOS_API=ENABLED;SET DPMI_MEMORY_LIMIT=8;ASSOCFILTER='||assoc,'R'

    /* Announce we're finished */
    say ""
    say "Desktop icon for POVLAB successfully created."
    say ""

    /* Small warning */
    say "NOTE:  As of yet, POVLAB is unable to spawn the DOS version of"
    say "       POV-Ray directly under OS/2."
    say "       To render a given scene, select -->POV-Ray (.pov) from"
    say "       the File menu and render manually from a DOS shell."
    say ""
    say "       When the OS/2 version of POV-Ray 3.0 is released, this"
    say "       Setup will be updated."
    say ""
    say "Enjoy POVLAB!!!!"

end
exit

/* Execution should have terminated by this point */

/* Subroutine "GetYesNo" */
GetYesNo: procedure
ok = 0

/* Loop until a "Y" or "N" is entered */
do until ok
   call charout, arg(1) || "? "
   pull reply
   reply = left(reply, 1)
   ok = (reply = "Y") | (reply = "N")
   if \ok then do
      say
      say 'Please enter "Y" or "N".'
      end
   end

/* Return 1 for "Yes", 0 for "No" */
return (reply = "Y")

