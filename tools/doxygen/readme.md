Building the POV-Ray Developer's Manual
=======================================

We're maintaining the bulk of the documentation of POV-Ray's inner workings in the form of
comments embedded in the source files themselves, adhering to certain formalisms that allow us
to use Doxygen (along with a few auxiliary tools) to compile that source documentation into a
Developer's Manual.

To generate the POV-Ray Developer's Manual, you need the following pieces of software:

  - **Doxygen**: Version 1.8.17 has successfully been tested; later versions may work just as fine.
  - **Graphviz Toolkit**: Version 2.26.3 or higher is recommended; version 2.38 has successfully been tested.
  - **Java**: Version 1.7.0 has successfully been tested; other versions may work just as fine.
  - **LaTeX and GhostScript**: These are only required for PDF generation. On Windows we recommend MiKTeX 2.9 or higher,
    which includes GhostScript and has successfully been tested; other packages may work just as fine.

All of the above are presumed to be found in the command search path.

The following pieces of software are also required, but included in POV-Ray's source tree for convenience,
and expected to reside there as provided:

  - **PlantUML**: Currently (2021-06-29), we're providing version 1.2021.8 (GPL 3 edition); other
    versions may work just as fine. (Note that we need the JAR file version, not the executable
    provided by some Linux distributions.)

To build the source documentation on Windows, run the `tools/doxygen/source-doc.bat` batch file.

To build the source documentation on Unix, run the `tools/doxygen/doxygen.sh` shell script.
