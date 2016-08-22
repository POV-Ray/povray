# Include Hierarchy {#include_hierarchy}

Within the POV-Ray source code, the following rules apply for the include hierarchy of header files:


@section self       Self-Containedness

Header files should be self-contained, i.e. include all files they depend on themselves.


@section small      Small Include Footprint

Each file's include footprint should be kept small to simplify include hierarchy:

  - Forward type declarations shall be used wherever this avoids having to include a header file.

  - Files already included in a unit header file shall _not_ be included again in the respective `.cpp` file (e.g. a
    file included in @ref core/colourspace.h shall not be included in @ref core/colourspace.cpp).


@section spec       Pulling In Special Headers

@subsection conf        Configuration Headers

The files @ref base/configbase.h, @ref core/configcore.h, @ref parser/configparser.h, @ref backend/configbackend.h and
@ref frontend/configfrontend.h serve as module configuration headers, and the only POV-Ray configuration headers to be
pulled in directly by code residing in the respective directory sub-tree; they automatically take care of pulling in any
other relevant configuration headers, including platform-specific headers.

@note   While the povms module has a configuration header following the same naming convention, the rules in this
        section do _not_ apply accordingly.

Each and every file within the @ref base/, @ref core/, @ref parser/, @ref backend/ or @ref frontend/ directory sub-trees
must effectively pull in the respective module configuration header as the very first header file. This is achieved by
the following requirements:

  - Each non-header file residing in the @ref base/, @ref core/, @ref parser/, @ref backend/ or @ref frontend/ directory
    sub-tree shall include its respective unit header file (the header file with the same base name but `.h` or `.hpp`
    extension) as the very first header file.

  - Each header file residing in the @ref base/, @ref core/, @ref parser/, @ref backend/ or @ref frontend/ directory
    sub-tree shall include the respective module configuration header as the very first header file.

This allows to also apply the following rule without any drawbacks:

  - Each non-header file residing in the @ref base/, @ref core/, @ref parser/, @ref backend/ or @ref frontend/ directory
    sub-tree, respectively, shall _not_ include the
    respective module configuration header.

The reason for this rule is to keep graphical representations of the include hierarchy simple.


@subsection debg        Last Header To Include

Each non-header source file residing in the @ref base/, @ref core/, @ref parser/, @ref backend/ or @ref frontend/
directory sub-tree, respectively, or including any of the headers therein, shall include @ref base/povdebug.h as the
very last include file.
