<!-- -----------------------------------------------------------------------------------------------
NOTE: THIS IS NOT A QUESTIONNAIRE, but rather a collection of building blocks to help you write a
good issue report. PLEASE DISCARD any portions you don't understand or deem irrelevant for your type
of report, and CHANGE OR ADD whatever you deem helpful.
------------------------------------------------------------------------------------------------ -->

### Summary

<!-- Briefly describe your issue here. -->

### Environment

<!-- Describe the environment you're using: -->
<!-- PLEASE DELETE ENTRIES if not applicable. -->
  - Source code Git tag: <!-- e.g. v3.7.0.0 -->
  - POV-Ray version: <!-- e.g. v3.7.1-beta.9+msvc14.win64 -->
  - Operating system: <!-- e.g. Windows 10, Ubuntu 14.04 -->
  - Hardware platform: <!-- e.g. x86, x86-64, ARM -->
  - Compiler: <!-- e.g. Visual Studio 2015 SP2, GNU g++ 5.3 -->
  - Regression from: <!-- known ok version, e.g. v3.6.2.msvc9.win64 -->


<!-- Build Problems Only ----------------------------------------------------------------------- -->
<!-- PLEASE DELETE THIS ENTIRE SECTION if reporting a non-build issue. -->

### Windows Build Settings

<!-- Describe the settings you were using to compile POV-Ray for Windows: -->
<!-- PLEASE DELETE THIS SUBSECTION for non-Windows builds. -->
  - Configuration: <!-- e.g. Debug, Release, Release-SSE2 -->
  - Platform: <!-- e.g. Win32, x64 -->
 
### Unix Build Command Sequence

<!-- Replace the following example with the actual command sequence you're using to build POV-Ray
for Unix: -->
<!-- PLEASE DELETE THIS SUBSECTION for non-Unix/Mac builds. -->
~~~
cd unix ; prebuild.sh ; cd ..
./configure COMPILED_BY="John Doe <john.doe@fnord.com>"
make check
sudo make install
~~~

### Unix Pre-Build Output

<!-- If you experience errors in `./configure`, or suspect the root cause to be in `prebuild.sh`,
copy the _complete_ output of `prebuild.sh` between the tilde lines (Otherwise, please strip this
subsection): -->
<!-- PLEASE DELETE THIS SUBSECTION for non-Unix/Mac builds or if not applicable. -->
~~~
~~~

### Unix Configure Output

<!-- Copy the complete output of `./configure` between the tilde lines: -->
<!-- PLEASE DELETE THIS SUBSECTION for non-Unix/Mac builds. -->
~~~
~~~

### Compiler/Linker Output

<!-- Copy any compiler/linker errors and other relevant messages between the tilde lines: -->
<!-- PLEASE DELETE THIS SUBSECTION for non-Unix/Mac builds. -->
~~~
~~~


<!-- Non-Build Problems Only ------------------------------------------------------------------- -->
<!-- PLEASE DELETE THIS ENTIRE SECTION if reporting a build issue. -->

### Steps to Reproduce

<!-- Describe the steps you took that led to the issue: -->
<!-- PLEASE DELETE OR ADD steps as applicable. -->
 1. <!-- First step -->
 2. <!-- Second step -->
 3. <!-- Third step -->

### Expected Behavior

<!-- Describe what you expected to happen. -->

### Actual Behavior

<!-- Describe what actually happened. -->


<!-- Render Problems Only ---------------------------------------------------------------------- -->
<!-- PLEASE DELETE THIS ENTIRE SECTION if reporting a non-render issue. -->

### Render Settings

<!-- Copy your INI options / command-line settings between the tilde lines: -->
<!-- PLEASE DELETE THIS SUBSECTION if not applicable. -->
~~~
~~~

### Scene

<!-- Copy a minimal sample scene between the tilde lines: -->
<!-- PLEASE DELETE THIS SUBSECTION if not applicable. -->
~~~
~~~

### Output

<!-- Copy the render output / message pane contents between the tilde lines: -->
<!-- PLEASE DELETE THIS SUBSECTION if not applicable. -->
~~~
~~~


<!-- All Problems ------------------------------------------------------------------------------ -->

### Workaround

<!-- If you have managed to work around the issue, describe that workaround here. -->
<!-- PLEASE DELETE THIS SUBSECTION if not applicable. -->

### Suggested Solution

<!-- If you have an idea how to solve the issue for good, describe it here. -->
<!-- PLEASE DELETE THIS SUBSECTION if not applicable. -->

<!-- -----------------------------------------------------------------------------------------------
NOTE: Please take a moment to PREVIEW YOUR REPORT before submitting it.
------------------------------------------------------------------------------------------------ -->
