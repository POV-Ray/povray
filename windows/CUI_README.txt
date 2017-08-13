	/// POV-Ray Windows Console User Interface (CUI) build
	/// Trevor SANDY <trevor.sandy@gmail.com>
	/// April 17, 2017
	//////////////////////////////////////////////////////
	
	Updated Windows Console User Interface (CUI) POV-Ray build, including:
	- Port Unix CUI functionality to Windows project
	- SDL2 image display window (Using SDL2 v2.0.5)
	- Options processor class
	- Benchmark, help and version options
	- Detailed console output_iterator 
	- Uses povray.conf just as Unix build 
	- Console signal management
	- GUI and CUI AppVeyor CI
	- Build POV-Ray CUI and GUI projects from the command line
	- Additional features...
	
	/// Building the Console User Interface (VS2017 GUI)
	//////////////////////////////////////////////////////
	See README.md for comprehensive details on building POV-Ray. 
	
	1. Open `windows\vs2015\povray.sln` in Visual Studio
	
	2. Set 'Windows Targets > CUI' as the start-up project
	
	3. Select the 'Generic POV-Ray > povbase' project
	
		3a. enable the definition of `_CONSOLE` in	`windows/povconfig/syspovconfig.h`
	
		3b. expand 'Backend Headers', then open the file `build.h` listed
		    within it. Please set `BUILT_BY` to your real name (and contact info)
	
		3c. Remove the `#error` directive after `BUILT_BY`
	
	4. Select the CUI branch and launch 'Build CUI' (Using 'Build Solution'
	   will abend because Visual Studio will attempt to build the GUI branch also.)
	
	/// Building POV-Ray from the command line (VS2017 MSBuild)
	//////////////////////////////////////////////////////
	See README.md for comprehensive details on building POV-Ray.
	
	This autobuild.cmd script uses MSBuild to configure and build POV-Ray from the command line.
	The primary benefit is not having to modify source files before building
	as described in the official POV-Ray build documentation when building from Visual Studio GUI.
	Additionally, it is possible to build either the CUI or GUI project.
	
	1. Launch `windows\vs2015\autobuild.cmd -info` from command prompt to see usage info.
	
	2. Execute autobuild.cmd with appropriate flags as desired.
	
	/// Build success (VS2017 GUI and MSBuild)
	//////////////////////////////////////////////////////
    If all goes well, you should end up with the POV-Ray for Windows
    executable. All 32-bit binaries should end up in
    `windows\vs2015\bin32`, and the 64-bit ones are in
    `windows\vs2015\bin64`. 
	
	/// File locations
    /////////////////////////////////////////////////////
	All Files
	
    The Windows Console User Interface build uses a file location 
	architecture similar to that of the Unix build. The default 
	locations for the povray conf, INI, scene, and include files are:
	
	- System Location:  C:\ProgramData\POV-Ray\Version[-release]\
	- User Location:    %USERPROFILE%\Documents\POV-Ray\Version[-release]\
	
	There is no default location for the povray binary itself. 
	At this moment, the default	locations are fixed (hard-coded) only.
	However all locations, except that for povray.conf, can be defined
	in the povray.conf file and; therefore, can be placed wherever
	you like as long as their path is defined in povray.conf
	
	INI Files

	POV-Ray allows the use of INI files to store common configuration
	settings, such as the output format, image size, and library paths.
	Upon startup, POV-Ray Console User Interface will use the environment
	variable POVINI to determine custom configuration information if
	that environment variable is set.  Otherwise, it will look for the 
	file "povray.ini" in the current directory.  If neither of these are
	set, POV-Ray will try to read the user "povray.ini" file (located under
	{User Location}\ini) or, otherwise, the system-level "povray.ini" (by 
	default in {User Location}\ini).	

	CONF File
	
	POV-Ray CUI build include the I/O Restriction feature as an attempt
	to at least partially protect a machine running the program to perform
	forbidden file operation and/or run external programs.  I/O Restriction
	settings are specified in a "povray.conf" configuration file.  There are
	two configuration levels within POV-Ray CUI: a system and a user-
	level configuration.  The system-level povray.conf file (by default in
	{System Location}) is intended for system administrators to set up minimal
	restrictions for the system on which POV-Ray will run. The user povray.conf
	file (under {User Location}) allows further restrictions to be set. For
	obvious security reasons, the user's settings can only be more (or equally)
	restrictive than the system-level settings. The administrator must take
	responsibility to secure the system location as appropriate.
	
	Here are the conf file options (cut and paste to create your povray.conf file):

	;                     PERSISTENCE OF VISION RAY TRACER
	;
	;                           POV-Ray VERSION 3.7
	;                            POVRAY.CONF FILE
	;                      FOR I/O RESTRICTIONS SETTINGS
	;	
	; The general form of the conf file option is:
	;
	; [Section]
	; setting
	;
	; Note: characters after a semi-colon are treated as a comment.
	;	
	; [File I/O Security] determines whether POV-Ray will be allowed to perform
	; read-write operations on files.  Specify one of the 3 following values:
	; - "none" means that there are no restrictions other than those enforced
	;   by the file system, i.e. normal file and directory permissions.
	; - "read-only" means that files may be read without restriction.
	; - "restricted" means that files access is subject to restrictions as
	;   specified in the rest of this file. See the other variables for details.

	[File I/O Security]
	;none       ; all read and write operations on files are allowed.
	;read-only  ; uses the "read+write" directories for writing (see below).
	restricted  ; uses _only_ "read" and "read+write" directories for file I/O.
	
	; [Shellout Security] determines whether POV-Ray will be allowed to call
	; scripts (e.g. Post_Frame_Command) as specified in the documentation.
	; Specify one of the 2 following values:
	; - "allowed" means that shellout will work as specified in the documentation.
	; - "forbidden" means that shellout will be disabled.

	[Shellout Security]
	;allowed
	forbidden
	
	; [Permitted Paths] specifies a list of directories for which reading or
	; reading + writing is permitted (in those directories and optionally
	; in their descendants).  Any entry of the directory list is specified on
	; a single line.  These paths are only used when the file I/O security
	; is enabled (i.e. "read-only" or "restricted").
	;
	; The list entries must be formatted as following:
	;   read = directory	     ; read-only directory
	;   read* = directory        ; read-only directory including its descendants
	;   read+write = directory   ; read/write directory
	;   read+write* = directory  ; read/write directory including its descendants
	; where directory is a string (to be quoted or doubly-quoted if it contains
	; space characters; see the commented example below).  Any number of spaces
	; can be placed before and after the equal sign.  Read-only and read/write
	; entries can be specified in any order.
	;
	; Both relative and absolute paths are possible (which makes "." particularly
	; useful for defining the current working directory).  The POV-Ray install
	; directory is designated as the {System Location}) and 
	; can be specified with "%INSTALLDIR%".  You should not specify
	; "%INSTALLDIR%" in read/write directory paths.  The user home (%USERPROFILE%)
	; directory can be specified with "%HOME%".
	;
	; Note that since user-level restrictions are at least as strict as system-
	; level restrictions, any paths specified in the system-wide povray.conf
	; will also need to be specified in the user povray.conf file.
	
	[Permitted Paths]
	; You can set permitted paths to control where POV-Ray can access content.
	; To enable remove the preceding ';'.

	; This example shows how to qualify path names containing space(s):
	; read = "C:\this\directory\contains space characters"
	
	; You can use %HOME% and/or %INSTALLDIR% as the origin to explicitly define content paths:
	
	; %HOME% is hard-coded to the path returned by the %USERPROFILE% environment variable.
	; read* = "%HOME%\Documents\POV-Ray\3.7\include"
	; read* = "%HOME%\Documents\POV-Ray\3.7\scenes"
	; read* = "%HOME%\Documents\POV-Ray\3.7\ini"	
	; read+write* = "HOME%\..\..\tmp"
	
	; %INSTALLDIR% is hard-coded to: C:\ProgramData\POV-Ray\LPub3D-Trace
	; read* = "%INSTALLDIR%\include"
	; read* = "%INSTALLDIR%\scenes"	
	; read* = "%INSTALLDIR%\ini"
	; read+write* = "%INSTALLDIR%\..\..\..\tmp"
	
	; You can also use your working directory path as the origin.
	
	; 1. Map POV-Ray library paths at 'C:\Users\<Joe Blow>\Documents\POV-Ray\3.7' from desktop
	; working directory, where a model is being rendered, at 'C:\Users\<Joe Blow>\Desktop\Models\FooModel'.	
	; 2. Working directory read and write access - to write rendered images.
	; read* = "..\..\..\..\..\Documents\POV-Ray\3.7\include"
	; read* = "..\..\..\..\..\Documents\POV-Ray\3.7\scenes"
	; read* = "..\..\..\..\..\Documents\POV-Ray\3.7\ini"
	; read+write* = .
	
	; End povray conf file

	Here are the INI file options for the conf file above (cut and paste to create your povray.ini file(s)):

	; Specify path to search for any files not found in current directory.
	; For example: Library_Path="C:\Program Files\POV-Ray for Windows\include"
	; There may be some entries already here; if there are they were
	; probably added by the install process or whoever set up the
	; software for you. At the least you can expect an entry that
	; points to the standard POV-Ray include files directory; on
	; some operating systems there may also be one which points to
	; the system's fonts directory.
	;
	; Note that some platforms (e.g. Windows, unless this feature is
	; turned off via the configuration file) will automatically append
	; standard locations like those mentioned above to the library
	; path list after reading this file, so in those cases you don't
	; necessarily have to have anything at all here.
	;
	
	System INI:
	; Library_Path="C:\ProgramData\POV-Ray\3.7\scenes"
	; Library_Path="C:\ProgramData\POV-Ray\3.7\include"
	; Library_Path="C:\ProgramData\POV-Ray\3.7\ini"
	
	User INI
	; Library_Path="C:\Users\<Joe Blow>\Documents\POV-Ray\3.7\scenes"
	; Library_Path="C:\Users\<Joe Blow>\Documents\POV-Ray\3.7\include"
	; Library_Path="C:\Users\<Joe Blow>\Documents\POV-Ray\3.7\ini"
	
	/// Updated files
    /////////////////////////////////////////////////////
	1.  .gitignore.............../
	2.  appveyor.yml............./
	3.  console.vcxproj........../windows/vs2015
	4.  console.vcxproj.filters../windows/vs2015
	5.  vfewin.vcxproj.........../windows/vs2015
	6.  vfewin.vcxproj.filters.../windows/vs2015
	7.  openexr_eLut.vcxproj...../windows/vs2015
	8.  openexr_toFloat.vcxproj../windows/vs2015
	9.  povray.sln.............../windows/vs2015
	10. syspovconfig.h.........../windows/povconfig
	11. vfesession.h............./vfe
	12. vfeplatform.cpp........../vfe/win
	13. vfeplatform.h............/vfe/win
	14. disp.h.................../windows...........(New) 
	15. disp_sdl.cpp............./windows...........(New) 
	16. disp_sdl.h.............../windows...........(New) 
	17. disp_text.h............../windows...........(New) 
	18. disp_text.cpp............/windows...........(New) 
	19. winconsole.cpp.........../vfe/win/console
	20. winoptions.cpp.........../vfe/win/console...(New)
	21. winoptions.h............./vfe/win/console...(New)
	22. CUI_README.txt.........../windows...........(New)
	23. autobuild.cmd............/windows/vs2015....(New)
	24. autobuild_defs.cmd......./windows/vs2015....(New)
	25. SDL2.vcxproj............./windows/vs2015....(New)
	26. SDL2_vcxproj.filters...../windows/vs2015....(New)
	27. SDL2Main.vcxproj........./windows/vs2015....(New)
	
	Note: Although I used VS2017 to develop the components described here.
	I do not believe there is any material difference between VS2017 and VS2015
	so you can substitute VS2017 for 2015.
	
	Please send any comments or corrections to Trevor SANDY <trevor.sandy@gmail.com>