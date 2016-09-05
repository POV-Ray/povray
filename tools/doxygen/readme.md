To build the POV-Ray source documentation, you need the following pieces of software:

  - **Doxygen**: Version 1.8.9 or higher is recommended; earlier versions fail to generate proper latex output for PDF
    generation.
  - **Graphviz Toolkit**: Version 2.26.3 or above is recommended; version 2.38 has successfully been tested.
  - **Java**: Version 1.7.0 has successfully been tested; other versions may work just as fine.
  - **PlantUML**: Version 8018 has successfully been tested; other versions may work just as fine.
  - **LaTeX and GhostScript**: These are required for PDF generation only. On Windows we recommend MiKTeX 2.9 or higher,
    which includes GhostScript and has successfully been tested; other packages may work just as fine.

All of the above are presumed to be found in the command search path, with the exception of PlantUML which is presumed
to be named `plantuml.jar` and located in the directory specified by the environment variable PLANTUML_JAR_PATH.

To build the source documentation on Windows, run the `source-doc.bat` batch file in this directory.

For Unix users we currently provide only a minimalistic _untested_ shell script, `doxygen.sh`, to build the
source documentation.
