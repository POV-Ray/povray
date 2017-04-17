    /// POV-Ray Windows Console User Interface (CUI) build
	/// Trevor SANDY <trevor.sandy@gmail.com>
	/// April 16, 2017
	//////////////////////////////////////////////////////
	
	Updated Windows Console User Interface (CUI) POV-Ray build, including:
	- Port Unix CUI functionality to Windows project
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
	1. Open `windows\vs2015\povray.sln` in Visual Studio
	
	2. Set 'Windows Targets > CUI' as the start-up project
	
	3. Select the 'Generic POV-Ray > povbase' project
	
		3a. enable the definition of `_CONSOLE` in	`windows/povconfig/syspovconfig.h`
	
		3b. expand 'Backend Headers', then open the file `build.h` listed
		    within it. Please set `BUILT_BY` to your real name (and contact info)
	
		3c. Remove the `#error` directive after `BUILT_BY`
	
	4. Select the CUI branch and launch 'Build CUI' (Build Solution
	   will abend because Visual Studio will attempt to build the GUI branch also.)

	/// Building POV-Ray from the command line (VS2017 MSBuild)
	//////////////////////////////////////////////////////
	1. Launch `windows\vs2015\autobuild.cmd --info` from command prompt
	
	2. Execute autobuild.cmd with appropriate flags as desired.
	
	/// Updated files:
    /////////////////////////////////////////////////////
	1.  .gitignore				/
	2.  console.vcxproj			/windows/vs2015
	3.  console.vcxproj.filters	/windows/vs2015
	4.  openexr_eLut.vcxproj	/windows/vs2015
	5.  openexr_toFloat.vcxproj	/windows/vs2015
	6.  povray.sln				/windows/vs2015
	7.  syspovconfig.h			/windows/povconfig
	8.  vfeplatform.cpp			/vfe/win
	9.  vfeplatform.h 			/vfe/win
	10. winconsole.cpp			/vfe/win/console
	11. winoptions.cpp			/vfe/win/console   (New)
	12. winoptions.h`			/vfe/win/console   (New)
	13. CUI_README.txt			/windows           (New)
	14. autobuild.cmd           /windows/vs2015    (New)
	15. autobuild_defs.cmd      /windows/vs2015    (New)

	Note: Although I used VS2017 to develop the components described here.
	I do not believe there is any material difference between VS2017 and VS2015.