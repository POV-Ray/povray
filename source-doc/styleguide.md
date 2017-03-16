@page styleguide  Coding Styleguide


C++ code can be written in a lot of different ways, and every code author has their personal preferences. However, in
a project like POV-Ray with multiple contributors, we feel that a consistent coding style will help a lot to find your
way around the code. Therefore, if you want to contribute to the POV-Ray development, we kindly ask you to follow the
guidelines presented here. Note that we find some of these rules important enough for our project that we may reject
non-compliant contributions.

The following covers the rules we deem sufficiently important for everyone to read; if you want more, see
@subpage styleguide2 for the boring stuff.


Role Models
===========

If, rather than read a prose styleguide for the POV-Ray project, you prefer to see examples, we suggest to look
primarily at the following files:

  - `source/backend/render/trace.h`
  - `source/backend/pattern/pattern.h` and `.../pattern.cpp`


Language Standard
=================

POV-Ray is being developed with portability high in mind. In practice and at present, this means:

  - Source code should adhere to the ISO-IEC 14882-2003 standard (aka C++03), as we currently do _not_ expect C++11
    (let alone C++14) to be readily available everywhere yet. Also, source code shold not rely on any compiler- or
    platform-specific behaviour except some essentials described in @ref compiler.
    @note
        Despite being standard-compliant, the following language constructs should _not_ be used, due to known
        limitations of popular build environments:
          - _Boolean Preprocessor Constants_: The expressions in `#``if` or `#``elif` preprocessor statements _must not_
            contain boolean constants (`true` or `false`) after macro expansion. The integer constants `0` and `1`,
            respectively, should be used instead. [MS Visual Studio 2013 and earlier]

  - As an exception to the above, the following C++11 constructs are fair game, as they has been supported by all major
    compiler brands for quite a while now and are thought to provide significant benefit:
      - `decltype(v)`, with `v` being a simple variable or class member (as opposed to a more complex expression)
    @note
        Other C++11 features may be added to the above list upon request if a reasonable case can be made in their
        favour. Note however that at present we will only consider features that have been supported by GCC as early as
        version 4.6, by Clang as early as version 4.2, and by MSVC as early as version 14 (Visual Studio 2015).

  - Source code should avoid potential conflicts with later standard extensions to the C++ language, most notably
    ISO-IEC DTR 19768 (aka TR1) and ISO-IEC 14882-2011 (aka C++11).

  - Use of smart pointers compatible with ISO-IEC DTR 19768 (aka TR1) is explicitly encouraged;
    however, instead of directly including the TR1 headers, `<boost/tr1/memory.hpp>` should be
    included for compatibility with environments that do not support TR1 out of the box.
    @todo
        Maybe we only want a subset of the smart pointers.

  - While POV-Ray does require boost, we want to keep dependence on it to a minimum. The following are currently
    considered fair game:
      - Flyweights.
      - Threads.
      - Datetime.
      - Intrusive Pointers.
      .
    @todo
        Make an inventory of what boost libraries we're actually using.


Code Formatting
===============

Even pure cosmetics can have a significant impact on how easily code can be read and understood.
In the POV-Ray project, we are using the following formatting style for new code, and are in the process of
transitioning existing code to these coventions:

  - Indentation of braces follows Allman style, with an indent of 4 spaces (no tabs), e.g.:

        while (x == y)
        {
            something();
            somethingelse();
        }
        finalthing();

  - Lines should be at most 120 characters long, to allow for sufficiently comfortable side-by-side viewing of code
    changes on a single display.

  - When breaking parameter lists or expressions, the next line is indented to wherever the broken parameter list or
    (sub-)expression starts, e.g.:

        void foo (int* pOneParam, int anotherParam, ...
                  int someParamThatDidntFit)
        {
            int i = pOneParam [anotherParam + ... +
                               someParamThatDidntFit];
            bar (i, pOneParam, anotherParam, ...
                 someParamThatDidntFit);
        }

  - For short comments, C++ single-line style (`// ...`) is preferred, with the `//` token
    indented to the same level as the commented code and followed by one blank. When using C-style
    block comments (`/* ... */`), both the opening and closing tags should be placed on individual
    lines and indented to the same level as the commented code, with the comment indented by another
    level and _not_ preceded by any decoration (leading `*` or some such); e.g.:

        /*
            Block comment.
        */
        int foo ()
        {
            // Single-line comment.
            return 0;
        }

  - Use of the `else if` idiom is strongly encouraged where it makes sense, e.g. the following is considered good style:

        if (x < foo)
        {
            something();
        }
        else if (x < bar)
        {
            somethingelse();
        }
        else
        {
            yetanotherthing();
        }

  - On the other hand, the following is considered poor style:

        if (x < foo)
        {
            if (y < bar)
            {
                something();
            }
        }
        else if (y < bar)
        {
            somethingelse();
        }


Naming Conventions
==================

Due to the age of the code, its C legacy, and the large number of contributors from early on, naming conventions in the
POV-Ray project have been anything but consistent in the past. For new code however, we try to stick to the following
scheme:

  - Macro names should use `UPPER_CASE_WITH_UNDERSCORES`.
  - Class and struct names should use `MixedCase`.
  - Type names should use `MixedCase`. Pointer types should have a `Ptr` suffix, i.e. `MixedCasePtr`. Pointers to
    immutable data should have a `Const` prefix and `Ptr` suffix, i.e. `ConstMixedCasePtr`.
  - Method names (both public and protected) should generally use `MixedCase` as well. However, methods that simply
    return a reference to a member variable may be named with `camelCase` instead.
  - Parameter and variable names should use `camelCase`.

Parameter and variable names might carry one or more additional prefixes. These should all be lowercase; e.g.
`mpCamelCase`:

  - Pointer names should have a `p` prefix. Pointers to pointers should have a `pp` prefix.
  - Non-const references should have a `r` prefix.
  - Arrays should have an `a` prefix.
  - Pointers to arrays should have a `pa` prefix; arrays of pointers should have an `ap` prefix.
  .
  - Protected or private member variable names should begin with a (possibly additional) `m` prefix.
  - Global variable names should begin with a (possibly additional) `g` prefix.
  - Constants should begin with a `k` prefix. (Does not apply to const parameters.)
  .

  - Excessively generic names, such as `Data`, should be avoided; if you've ever tried to figure out where such an
    entity is used in the project, you probably know why. Reasonable exceptions can be made for entities with a very
    limited visibility, such as private class members or local variables.
  - Name collisions with identifiers in an outer scope ("shadowing") should be avoided.


Include Files
=============

  - Header files should generally be included in the following order:
      - POV-Ray module configuration header file, if applicable (mandatory in any unit header file, i.e. any header
        file with a `.cpp` of the same name; e.g. `core/configbase.h` in `core/shape/sphere.h`).
      - POV-Ray unit header file, if applicable (mandatory in any `.cpp` file; e.g. `core/shape/sphere.h` in
        `core/shape/sphere.cpp`).
      - C++ variants of standard C header files.
      - Standard C++ header files.
      - Boost header files.
      - Other 3rd party library header files, grouped by library.
      - POV-Ray header files.
      - Debug header file (@ref base/povdebug.h), if applicable (mandatory in any `.cpp` file, _not_ allowed in any
        other file).
      .
    Within each group, alphabetical order should be preferred. (Note however that certain other ordering constraints
    might apply within platform-specific portions of the code.)

  - C++ source code should _not_ include C standard header files; include the corresponding C++ header files instead
    (e.g. `<cstdio>` instead of `<stdio.h>`).

  - Header files should be self-contained, i.e. include all files they depend on themselves. At the same time, their
    include footprint should be kept to a minimum to simplify include hierarchy; most notably, if all you need from
    another include file are type declarations, use forward declarations where possible instead of including the other
    file.


Smart Pointers
==============

Where ownership, i.e. responsibility for releasing dynamically allocated memory, is non-trivial, smart pointers should
be used. The choice of type depends on the context:

  - Regular pointers (`T*`) may be used where performance requirements are high and ownership is obvious. Examples might
    be temporary objects handled exclusively within one function, or private member data handled exclusively by the
    containing object.
  - Shared pointers (`shared_ptr<T>`) should be used where ownership is non-trivial, and code clarity is of higher
    importance than performance or memory consumption.
  - Intrusive pointers (`intrusive_ptr<T>`) should be used where ownership is complex, and performance or memory
    footprint are of higher importance than clarity of the referenced type's implementation.
  - Intrusive pointers should also be used to manage objects that need to create additional permanent references to
    themselves.

@note
    When judging complexity of ownership, do not forget the possibility of code terminating prematurely due to
    exceptions.

When smart pointers are used, some care still needs to be employed in passing around and storing references:

  - Return values and permanent references (e.g. object data members or global variables) _must_ be of the respective
    _smart_ pointer type.
  - Parameters or local variables of functions or methods that are guaranteed to _not_ cause additional permanent
    references to be created should use regular _reference_ type wherever possible, or _regular_ pointer type otherwise.
  - Parameters or local variables of Functions or methods that _can_ cause additional permanent references to be created
    _must_ use _smart_ pointer type when employing shared pointers, but _should_ use _regular_ pointer type when
    employing intrusive pointers.

@note
    When object ownership is shared between threads, additional considerations may apply. Such cases should be
    adequately documented.

When using smart pointers, instead of the type suffix `Ptr` the following suffixes should be used for clarity:

  - `SPtr` to denote a shared pointer type
  - `IPtr` to denote an intrusive pointer type
  - `TPtr` to denote a regular pointer type used as a temporary reference

@note
    When using `shared_ptr<>` or `intrusive_ptr<>`, do _not_ specify a namespace, and do _not_ include the header files
    you think these templates are defined in. It is entirely up to compile-time configuration header files to include
    the necessary headers and pull in the definitions to the global namespace, so that the code works equally well with
    e.g. the `shared_ptr` implementations from either Boost, TR1 or C++11.


Miscellaneous Coding Rules
==========================

  - **Unions**: They're evil. Use polymorphism instead, unless you have an exceptionally strong reason for it. Make sure
    to document that reason.

  - **Type Casting**: Except for primitive non-pointer types, only C++-style casts (e.g. `static_cast<T>(...)`) should
    be used, in order to avoid accidental removal of a `const` qualifier, conversion to an incompatible type, or similar
    pitfalls.

  - **Switch Fallthrough**: Intentional fall-through in a switch statement should generally be
    avoided. If used at all, it must be explicitly indicated with a `// FALLTHROUGH` comment.
    (Please use this exact spelling, to facilitate automatic code analysis.)

  - **For Loops**: Keep `for` loops simple; don't do anything that requires the use of the comma operator, and don't
    mess with the loop counter inside the loop.

  - **Const**: Make liberal use of the `const` qualifier where possible, especially in parameter declarations.

  - **Member Visibility**: Except in structs -- which should only be used for stateless plain-old-data aggregates --
    all member variables should be protected or private.

  - **Macros**: Avoid them wherever reasonably possible.

  - **Strings**: Do not use C-style strings, except as literals to initialize `std::string` objects.

  - **Memory Allocation**: When allocating dynamic memory, do not use C-style allocation (`malloc()`), but C++-style
    allocation using the `new` operator. Whenever responsibility for destruction of the allocated object is non-trivial,
    make use of smart pointers.

  - **Output Parameters**: When declaring a function that is to modify any of its parameters, prefer references over
    pointers.

  - **Locale**: Do not tamper with the C locale (i.e. do not call `setlocale` or any other function doing the same job),
    as plenty of code in POV-Ray relies on it remaining set to the default.

  - **Optimization**: Do not over-optimize your code. While we do want POV-Ray to be fast, we also want it to be
    reasonably maintainable, and modern compilers generally do a good enough job at automatic optimization that you can
    focus on keeping your code robust and easy to understand.


Code Documentation
==================

In order to help contributors find their way around the code, all relevant information -- from
software architecture to interfaces to implementation -- should be documented liberally, in a
standardized manner.


Implementation Documentation
----------------------------

The POV-Ray project follows the idea that, as far as implementation goes, the source code itself is
the most precise -- and therefore best -- documentation. However, wherever it is not immediately
obvious how the code works, or why this particular implementation was chosen, liberally pepper the
source code with comments.

@note
    Known bugs or limitations of an implementation should be placed in the interface
    documentation.


Interface Documentation
-----------------------

The interface provided by any source file should be documented in the respective header file, using
a format compatible with Doxygen 1.8.12. For consistency, please use JavaDoc-style comments (`/// ...`
or `/** ... */`; the general rules for comments apply) and JavaDoc-style tags (`@``foo`).
C++ single-line style comments should be preferred, as block comments may occasionally interfere with
Doxygen's Markdown support.

@note
    Platform-specific modules may mandate a different tag style, such as (hypothetically) DocXML
    style for the Windows GUI modules.


Other Documentation
-------------------

Any other information relevant for developers that is not specific to a particular source file
should be documented in Doxygen 1.8-compatible markdown files named `source-doc/foo.md`.


Notes and Warnings
------------------

Doxygen provides various tags to draw attention to a particular paragraph. To help you choose between these, use the
following guidelines:

  - **Remark**: Use the `@``remark` tag when documenting information that may be surprising but not really necessary to
    know.
  - **Note**: Use the `@``note` tag when documenting general information that the reader should be aware of.
  - **Warning**: Use the `@``warning` tag when documenting potential pitfalls.
  - **Attention**: Use the `@``attention` tag when documenting pitfalls that may come as a big surprise to the reader or
    have grave effects when not avoided.

However, always use the following instead if they are appropriate:

  - **To-Do**: Use the `@``todo` tag when documenting that something is still unfinished and needs more work.
  - **Deprecated**: Use the `@``deprecated` tag when documenting that somehting is only kept around for backward
    compatibility and should no longer be used in new code.


Images
------

Images should be used sparingly, as they are more difficult to keep up to date with source code
changes; where extended Doxygen features can do the trick, please use those instead (see below).

If they cannot be avoided, images accompanying the documentation of `foo.cpp` or `foo.h` should be
placed in a directory named `source-doc/foo/`. Same goes for images referenced from
`source-doc/foo.md`.


Doxygen Extensions
------------------

Some Doxygen syntax features require particular Doxygen configuration settings, or even external
tools. The following can be freely used:

  - **Automatic brief descriptions** (`JAVADOC_AUTOBRIEF=YES`): Please avoid using the `@``brief`
    tag.
  - **Markdown** (`MARKDOWN_SUPPORT=YES`): Please avoid tags wherever markdown is easier to read
    (e.g., use `_foo_` instead of `@``e foo` for emphasis).
  - **PlantUML** (<http://plantuml.sourceforge.net/>): Please use doxygen's `@``startuml` tag format for any UML
    sequence or state diagrams, both in source files as well as in markdown files; for now, please try to avoid other
    UML diagram types.
  - **Dot** (`HAVE_DOT=YES`; <http://www.graphviz.org/>): Please use doxygen's `@``dot` tag format for any other graphs,
    both in source files as well as in markdown files.
  - **LaTeX formulae**: Please use doxygen's `@``f$` / `@``f[` / `@``f{` tag format for any non-trivial formulae.


Legacy Coding Style
===================

Sometimes you may run across legacy code that violates the above rules. The following should give
you an idea of how to proceed:

  - If you're going to touch a file just for the sake of fixing legacy coding style, please go all
    the way and overhaul the entire file.
  - If you're going to touch a file for minor code changes, please adapt to the file's existing
    coding style, or go all the way and overhaul the entire file.
  - If you're going to touch a file for larger code changes, please overhaul the coding style of the
    entire respective section (e.g. function, class definition etc.)
