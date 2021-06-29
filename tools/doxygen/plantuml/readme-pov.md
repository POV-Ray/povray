PlantUML
========

We're using PlantUML as a minor part of our process to generate a Developer's
Manual. The PlantUML software is included in the POV-Ray source tree for
technical reasons only, to facilitate automated generation of said manual:
We need the Java archive (jar file) variant, while Linux distributions (e.g.
Ubuntu) will typically provide only a package containing some GUI variant;
and while an official site for downloading the jar file does exist, it is
hosted by SourceForge, and designed specifically to make automated download a
major pain.

To find out what version of PlantUML we're currently including, run:

~~~
java -jar plantuml.jar -version
~~~

To view the licensing terms and conditions of the PlantUML version we're
currently including, run:

~~~
java -jar plantuml.jar -license
~~~
