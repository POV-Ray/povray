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
        int foo()
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

  - On the other hand, the following is considered **poor** style:

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
  - Type names should use `MixedCase`. Pointer types should have a `Ptr` suffix, i.e. `MixedCasePtr`. pointers to
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


Function and Method Signatures
==============================

@note
    For simplicity, the following section uses the term "function" as shorthand for "global function, static method or
    non-static method". Where only global functions are meant, this is explicitly stated.

  - Function parameters should be grouped in the following order:
     -# Output parameters, i.e. parameters set but never evaluated by the function.
     -# Input/Output parameters, i.e. parameters both evaluated and set by the function.
     -# Regular input parameters, i.e. parameters evaluated but never set by the function.
     -# Optional parameters, i.e. parameters that may be omitted entirely in a function call.
     .
    Within each group, ordering of the individual parameters should be consistent among related functions.

  - Function parameters should generally adhere to the following typing rules (with `TYPE` denoting the actual payload
    data type):
      - Mandatory output and input/output parameters should be of type `TYPE&`
      - Optional output and input/output parameters should be of type `TYPE*`.
      - Mandatory input parameters should be of type `TYPE` or `const TYPE&`, depending on whether `TYPE` is a basic
        type or compound type.
      - Complex mandatory input parameters should be of type `const TYPE&`.
      - Input parameters that are to undergo transfer of ownership (i.e. memory blocks allocated by the caller but to be
        released later by the function) should be of type `shared_ptr<TYPE>` or `shared_ptr<const TYPE>`.
      - Optional parameters should be of type `const TYPE*`. An exception are parameters of basic types for which a
        reasonable default can be provided.
      - In deviation from the above, any data residing in a memory block that is to undergo transfer of ownership (i.e.
        allocated by the caller but to be released by the function, or allocated by the function but to be released by
        the caller), or to which the non-owning party may retain a reference, should be passed as `shared_ptr<>` in case
        of input parameters, or `shared_ptr<>&` in case of output or input/output parameters.

  - Return values should generally adhere to the following typing rules:
      - Data of basic types should be returned as `TYPE`.
      - Data of pseudo-arithmetic types such as vectors or colours may be returned as `const TYPE&` if the function has
        access to a reasonably persistent instance of the data (e.g. as part of an object's state). Callers need to make
        sure that the data container's state remains unchanged for as long as they require access to the data. Ideally,
        callers should evaluate or copy the data immediately.
      - Designated class methods may return values of type `TYPE&` to provide access to protected data members. In such
        cases, callers need to make sure that the data container's state remains unchanged for as long as they require
        access to the data member.
      - Data residing in a memory block that is to undergo transfer of ownership (i.e. allocated by the function, but to
        be later released by the caller), or to which the caller may retain a reference, should be returned as
        `shared_ptr<TYPE>` or `shared_ptr<const TYPE>`.
      - In any other cases, data should be passed out of the function via an output parameter rather than by return
        value.

  - Functions reporting a success/failure status flag should always do so via a `bool` return value, using an output
    parameter to pass any actual result data (if applicable) rather than vice versa. A value of `true` should alwas
    indicate success.


Data Members
============

  - Data members should generally be grouped by type, rather than functionality. The main reason for this is to keep the
    memory footprint low on systems that have data alignment constraints for non-byte sized types. The members should be
    sorted by ascending alignment constraints (which usually corresponds to the size of the largest basic type involved)
    on typical target platforms:

     -# `long long int`. This type is at least 64 bits in size, but could be even larger, and will presumably be the
        largest integer type available (that we care about at any rate).
     -# @ref POV_LONG and @ref POV_ULONG. These are supposed to be at least 64 bits in size.
     -# `double` (and compounds thereof, most notably geometric vectors and high-precision colour types). On all current
        target platforms and all major contemporary architectures this conform to the IEEE 754 `binary64` type, and thus
        is 64 bits in size, but in theory it could be wider.
     -# @ref POV_INT64 and @ref POV_UINT64. These are supposed to be exactly 64 bits in size.
     -# Miscellaneous compound data types. Unless known otherwise, we expect these to have alignment constraints
        somehwere between pointers and IEEE 64-bit floats.
     -# Pointers, `ptrdiff_t` and `size_t`. The size of these types depends on the processor architecture, and on all
        current target platforms and all major contemporary architectures (those suitable to run POV-Ray at any rate)
        will be either 32 or 64 bits.
     -# `long int`. This type is guaranteed to be at least 32 bits in size, and on all current target platforms and all
        major contemporary systems will be no larger than the pointer size.
     -# `int` data. This type is guaranteed to be no larger than `long int`, and although it could be as small as 16
        bits, on all current target platforms and all major contemporary architectures suitable to run POV-Ray will be
        at least 32 bits in size. (Note that on some 64-bit platforms both `int` and `long int` may be 64 bits wide.)
     -# `float` (and compounds thereof, most notably colours and low-precision geometric vectors). On all current
        target platforms and all major contemporary architectures this conform to the IEEE 754 `binary32` type, and thus
        is 32 bits in size, but in theory it could be wider.
     -# @ref POV_INT32 and @ref POV_UINT32. These are supposed to be exactly 32 bits in size.
     -# `enum` types. While on some platforms enums may always be of the same size as `int` (which in the case of our
        current target platforms is 32 bits), other platforms may use just the right size to fit the maximum value.
     -# `short`. On all current target platforms and all major contemporary architectures this type is 16 bits in size,
        but in theory it could be wider.
     -# @ref POV_INT16 and @ref POV_UINT16. These are supposed to be exactly 16 bits in size.
     -# `char`. On all platforms, this is the smallest data type available, and at least 8 bits in size.
     -# @ref POV_INT8 and @ref POV_UINT8. These are supposed to be exactly 8 bits in size.
     -# `bool`. If used as a bit field, this type requires just a single bit per data field.

  - Boolean data should be stored in `bool:1` bit fields.


Choice of Data Types
====================

  - `long long int` should be avoided, as it may potentially be of excessive size for the job. Use @ref POV_LONG or
    @ref POV_ULONG instead, which are supposed to be at least 64 bits wide, but of the smallest type fitting that bill.

  - `long` should be avoided, as its actual size is particularly unreliable. If you need a 64-bit integer, use
    @ref POV_LONG or @ref POV_ULONG instead. If you need a 32-bit integer, use `int` instead, since we do not support
    platforms with a smaller `int` type (see @ref compiler). If you need a type holding some size or counter, consider
    using `size_t`.

  - Use `enum` types instead of `int` wherever possible.

  - Prefer `bool:1` bit fields over bit masks.

  - Always use `bool` to represent boolean ("yes/no", "on/off") values, **never** `int`. The "polarity" of the data
    should be chosen to be as straightforward as possible, e.g. use `bool enableFoo`, **not** `bool disableFoo`.


Include Files
=============

  - Header files should generally be included in the following order:
      - POV-Ray module configuration header file, if applicable (mandatory in any unit header file, i.e. any header
        file with a `.cpp` of the same name; e.g. @ref core/configcore.h in @ref core/colourspace.h).
      - POV-Ray unit header file, if applicable (mandatory in any `.cpp` file; e.g. @ref core/colourspace.h in
        @ref core/colourspace.cpp).
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

  - **Assignments in Conditions**: Avoid assignments in conditional statements.


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
