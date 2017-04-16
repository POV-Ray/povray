    /// POV-Ray Windows Console User Interface (CUI) build
	/// Trevor SANDY <trevor.sandy@gmail.com>
	/// April 13, 2017
	//////////////////////////////////////////////////////
	
	Port all Unix functionality to the Windows CUI build, including:
	- Options processor class
	- Benchmark, help and version options
	- Detailed console output_iterator 
	- Uses povray.conf just as Unix build 
	- Console signal management
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
	
	/// Updated files:
    /////////////////////////////////////////////////////
	1.  .gitignore				/
	2.  console.vcxproj			/windows/vs2015
	3.  console.vcxproj.filters /windows/vs2015 
	4.  povray.sln				/windows/vs2015 
	5.  syspovconfig.h			/windows/povconfig 
	6.  vfeplatform.cpp			/vfe/win
	7.  vfeplatform.h 			/vfe/win
	8.  winconsole.cpp			/vfe/win/console 
	9.  winoptions.cpp			/vfe/win/console   (New)
	10. winoptions.h`			/vfe/win/console   (New)
	11. CUI_README.txt			/windows