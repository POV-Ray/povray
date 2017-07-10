@page directories   Source Directory Tree


POV-Ray's primary source files are generally organized as follows:

  - `distribution`: Accompanying files to be distributed as-is.
      - `platform-specific`: Files specific to a given POV-Ray _incarnation_.
  - `doc`: User manual in HTML format.
  - `libraries`: Copies of well-defined versions of 3rd party libraries.
  - `platform`: Platform-specific code.
      - `<hw-name>`: Code specific to a given hardware platform family.
      - `<os-name>`: Code specific to a given operating system family.
  - `source`: Platform-neutral code.
      - base: Code shared between multiple modules.
      - `<module-name>`: Code specific to a given module.
  - `source-doc`: General documentation of the POV-Ray source code.
  - `tests`: Unit tests.
  - `tools`: Optional development tools.
  - `vfe`: Code specific to the _Virtual Front-End_ module.
  - `<os-name>`: Code specific to a given POV-Ray _incarnation_.
      - `povconfig`: Compile-time configuration of the platform-neutral code.

@todo
    The location of the `vfe` directory is inconsistent with that of the other modules. We should either move it inside
    the `source` directory, or set up a new directory specifically for optional modules. In either case, the
    _incarnation_-specific subdirectories should be moved inside the respective general _incarnation_-specific
    directories.

@todo
    Traditionally, the `doc` directory was used as the primary source for the user manual. Now that the Wiki has taken
    over this role, the `doc` directory should probably be moved over to `distribution/platform-specific`.
