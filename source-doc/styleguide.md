Coding Styleguide
=================

@todo   Flesh out non-cosmetic coding rules.


Language Standard
-----------------

POV-Ray has been written, and should be continued to be written, with portability high in mind.

  - Source code should be written in C++ adhering to the ISO-IEC 14882-2003 standard (aka C++03).

  - Source code should avoid potential conflicts with later standard extensions to the C++ language,
    such as ISO-IEC DTR 19768 (aka TR1) and ISO-IEC 14882-2011 (aka C++11).

  - Use of smart pointers compatible with ISO-IEC DTR 19768 (aka TR1) is explicitly encouraged;
    however, instead of directly including the TR1 headers, `<boost/tr1/memory.hpp>` should be
    included for compatibility with environments that do not support TR1 out of the box.

    @todo   Maybe we only want a subset of the smart pointers.

  - While POV-Ray does require boost, only a small subset of the libraries should actually be used;
    the following are fair game:
      - Flyweights.
      - Threads.

    @todo   Make an inventory of what boost libraries we're actually using.


Cosmetics
---------

Even pure cosmetics can have a significant impact on how easy code can be read and understood.
While there are many competing schools of thought out there with different ideas in this respect,
the primary concern should not be _what_ code formatting style is used, but that it is used
_consistently_ across all source files. Therefore, when contributing for the POV-Ray project, please
set aside your personal preferences, and adhere to the following established rules:

### Code Formatting

  - Indentation should follow Allman style, with an indent of 4 spaces (no tabs), e.g.:

        while (x == y)
        {
            something();
            somethingelse();
        }
        finalthing();

  - Lines should be at most 100 characters long.
  - Comments should use C++ single-line style (`//`) without exceptions.

### Naming Conventions

@todo   Agree upon official naming conventions.

Due to the age of the code, its C legacy, and the large number of contributors from early on, naming
conventions in the POV-Ray project are anything but consistent.

The module `trace.cpp`, which constitutes the very heart of the POV-Ray render engine and might
therefore be considered authoritative to some degree, uses the following conventions:

  - Macro names use `UPPER_CASE_WITH_UNDERSCORES`.
  - Class and struct names use `MixedCase`.
  - Method names (both public and protected) use `MixedCase`.
  - Member variable names use `camelCase`.
  - Parameter names vary wildly between `lowercase`, `lower_case_with_underscores`, `camelCase` and,
    in a few instances, `Mixed_Case_With_Underscores`.
  - Local variable names vary wildly between `lowercase`, `lower_case_with_underscores`, `camelCase`
    and, in a few instances, `camel_Case_With_Underscores`.

Until official conventions have been agreed upon, please adapt to the code around you for now.


Include Files
-------------

  - Header files should be included in the following order:
      - Standard C header files.
      - Standard C++ header files.
      - Boost header files.
      - Other 3rd party library header files, grouped by library.
      - POV-Ray header files.
      .
    Within each group, alphabetical order should be preferred. (Note however that certain other
    ordering constraints might apply for the POV-Ray header files.)

  - In C++ source code, standard C header files should be included by their proper C++ names, e.g.
    `<cstdio>` instead of `<stdio.h>`.

  - Header files should be self-contained, i.e. include all files they depend on.


Code Documentation
------------------

In order to help contributors find their way around the code, all relevant information -- from
software architecture to interfaces to implementation -- should be documented liberally, in a
standardized manner.

### Implementation Documentation

The POV-Ray project follows the idea that, as far as implementation goes, the source code itself is
the most precise -- and therefore best -- documentation. However, wherever it is not immediately
obvious how the code works, or why this particular implementation was chosen, liberally pepper the
source code with comments.

@note   Known bugs or limitations of an implementation should be placed in the interface
        documentation.

### Interface Documentation

The interface provided by any source file should be documented in the respective header file, using
a format compatible with Doxygen 1.8. For consistency, please use single-line JavaDoc-style comments
(`///`) and JavaDoc-style tags (`@``foo`).

@note   Platform-specific modules may mandate a different tag style, such as (hypothetically) DocXML
        style for the Windows GUI modules.

### Other Documentation

Any other information relevant for developers that is not specific to a particular source file
should be documented in Doxygen 1.8-compatible markdown files named `source-doc/*.md`.

### Doxygen Extensions

Some Doxygen syntax features require particular Doxygen configuration settings, or even external
tools. The following can be freely used:

  - Automatic brief descriptions (`JAVADOC_AUTOBRIEF=YES`). Please avoid using the `@``brief` tag.
  - Markdown (`MARKDOWN_SUPPORT= YES`). Please avoid tags wherever markdown is easier to read (e.g.,
    use `_foo_` instead of `@``e foo` for emphasis).
  - Mscgen (<http://www.mcternan.me.uk/mscgen/>). Please use doxygen's `@``msc` tag format for any
    message sequence charts, both in source files as well as in markdown files.
  - LaTeX formulae.


Dealing with Legacy Coding Style
--------------------------------

Sometimes you may run across legacy code that violates the above rules. The following should give
you an idea of how to proceed:

  - If you're going to touch a file just for the sake of fixing legacy coding style, please go all
    the way and overhaul the entire file.
  - If you're going to touch a file for minor code changes, please adapt to the file's existing
    coding style, or go all the way and overhaul the entire file.
  - If you're going to touch a file for larger code changes, please overhaul the coding style of the
    entire respective section (e.g. function, class definition etc.)