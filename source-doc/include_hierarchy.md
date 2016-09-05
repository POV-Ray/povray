@page include_hierarchy  Include Hierarchy


Within the POV-Ray source code, the following rules apply for the include hierarchy of header files:


General Rules
=============

  - **Self-Containedness**: Header files should be self-contained, i.e. include all files they depend on themselves.

  - **Small Include Footprint**: Each file's include footprint should be kept small to simplify the include hierarchy:
      - Forward type declarations shall be used wherever this avoids having to include a header file.
      - Files already included in a unit header file shall _not_ be included again in the respective `.cpp` file (e.g. a
        file included in @ref core/colourspace.h shall not be included in @ref core/colourspace.cpp).


Pulling In Special Headers
==========================


Configuration Headers
---------------------

The files @ref backend/configbackend.h, @ref base/configbase.h, @ref core/configcore.h, @ref frontend/configfrontend.h,
@ref parser/configparser.h and @ref vm/configvm.h serve as module configuration headers, and the only POV-Ray
configuration headers to be pulled in directly by code comprising part of the respective module; they automatically
take care of pulling in any other relevant configuration headers, including platform-specific headers.

@note
    While the povms module has a configuration header following the same naming convention, the rules in this
    section do _not_ apply accordingly.

Each and every file comprising part of the _backend_, _base_, _core_, _frontend_, _parser_ or _vm_ module must
effectively pull in the respective module configuration header as the very first header file. This is achieved by the
following requirements:

  - Each non-header file comprising part of one of the aforementioned modules shall include its respective unit header
    file (the header file with the same base name but `.h` or `.hpp` extension) as the very first header file.

  - Each header file comprising part of one of the aforementioned modules shall include the respective module
    configuration header as the very first header file.

This allows to also apply the following rule without any drawbacks:

  - Each non-header file comprising part of one of the aforementioned modules shall _not_ include the respective module
    configuration header.

The intention of the latter rule is to keep graphical representations of the include hierarchy simple.


Last Header To Include
----------------------

Each non-header source file including any header files from the _backend_, _base_, _core_, _frontend_, _parser_ or _vm_
modules shall include @ref base/povdebug.h as the _very last_ include file.
