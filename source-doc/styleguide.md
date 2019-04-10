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

  - Source code should adhere to the ISO/IEC 14882:2011 standard (aka C++11), as we currently do _not_ expect C++14
    to be readily available everywhere yet. Also, source code shold not rely on any compiler- or
    platform-specific behaviour except some essentials described in @ref compiler.
    @note
        Despite being standard-compliant, the following language constructs should _not_ be used, due to known
        limitations of popular build environments:
          - _Boolean Preprocessor Constants_: The expressions in `#``if` or `#``elif` preprocessor statements _must not_
            contain boolean constants (`true` or `false`) after macro expansion. The integer constants `0` and `1`,
            respectively, should be used instead. [MS Visual Studio 2013 and earlier]
          - _Large Integer Literals_: Integer literals specified using decimal notation should not
            exceed a magnitude of 2147483647, unless carrying a `u` and/or `ll` suffix.
            [MS Visual Studio 2015]
          - _Static Thread-Local Template Members_: Template classes _must not_ have thread-local
            static member variables. See the @ref PooledSimpleVector template class for a viable
            workaround. [XCode 8 and later, apparently still unresolved as of 2018-04-08]
          - _Math Library Float Functions_: The `f`-suffixed variants of `<cmath>` library functions
            (e.g. `std::fabsf(float)`) _must not_ be used. The float overloads of the non-suffixed
            variants (e.g. `std::fabs(float)`) should be used instead. [GCC, all versions as of
            2019-02-11].

  - Source code should avoid potential conflicts with later standard extensions to the C++ language,
    most notably ISO/IEC 14882:2014 (aka C++14), ISO/IEC 14882:2017 (aka C++17)  and the upcoming
    successor (aka C++20).

  - While POV-Ray does require boost, we want to keep dependency on it to a minimum. The following are currently
    considered fair game:
      - Flyweights.
      - DateTime **except** features that may require linking with the lib (mostly conversions to/from string).
      - SmartPtr intrusive pointers.
      .
    The following boost features are explicitly **not** allowed:
      - DateTime conversions to/from string (they may require linking on some platforms).
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
    changes on a single display. New lines of code should be limited to 100 characters, to provide
    additional spare space for later minor modifications.

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
  - As an exception, underscores can be used to separate components of a name representing
    distinct concepts if this improves legibility; for example we're currently making use of such
    patterns for enums, where the first component often serves as a kind of namespace to avoid
    name collisions with other enums (note however that this particular problem is now better dealt
    with by using C++11 scoped enumerations, i.e. `enum class`).

Parameter and variable names might carry one or more additional prefixes. These should all be lowercase; e.g.
`mpCamelCase`:

  - Pointer names should have a `p` prefix. Pointers to pointers should have a `pp` prefix.
  - Non-const references should have a `r` prefix.
  - Arrays should have an `a` prefix.
  - Pointers to arrays should have a `pa` prefix; arrays of pointers should have an `ap` prefix.
  .
  - Protected or private member variable names should begin with a (possibly additional) `m` prefix.
  - Global variable names should begin with a (possibly additional) `g` prefix.
  - Global variables with thread-local storage should begin with a (possibly additional) `gt`
  - prefix.
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

To test whether a smart pointer is (non-)null, do not unnecessarily clutter your code by invoking
`.get()`, but do explicitly compare with `nullptr` (rather than rely on automatic conversion to
`bool`) to make it obvious that you're dealing with pointer data. Never compare smart pointers
with `NULL`, as that would lead to compile errors on some (perfectly C++11 compliant) compilers.


Classes
=======

  - All classes should be declared `final` unless specifically intended otherwise.
  - All destructors of polymorphic classes should be declared `virtual`.
  - All overriding methods (including destructors) should be declared both `virtual` and `override`.


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
    mess with the loop counter inside the loop. Do make use of C++11 ranged for loops (using the
    `for (auto&& item : container)` idiom) wherever applicable.

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

  - **Booleans**: Do not use `int` types or integer literals when dealing with boolean (yes/no,
    on/off) information, as this gives the false impression that you're dealing with numbers.
    Use the `bool` type and the `true` and `false` constants instead. (Exception: In preprocessor
    conditional statements, `1` and `0` _must_ be used in lieu of `true` and `false`.)

  - **Boolean Tests**: When testing whether a boolean expression is true or false, simply use the
    boolean expression as-is, or its negation (using the unary `!` operator). If you think you need
    to explicitly compare a variable or function result with `false` for clarity, chances are your
    variable or function name is poorly chosen. Comparing with `true` is right out, as it may cause
    unexpected results if the expression is not a genuine `bool`.

  - **Null Pointer Constant**: Do not use `NULL`, as it is not guaranteed to expand to a genuine
    pointer expression. Use the C++11 `nullptr` constant instead.

  - **Null Pointer Tests**: To test whether a pointer expression is null, explicitly compare the
    expression with `nullptr` to make it obvious that you're dealing with pointer data. 

  - **Zero Tests**: To test whether an integer value is null, explicitly compare the expression
    with `0` to make it obvious that you're dealing with integer data.

  - **Object Validity Tests**: To test whether a particular object is in a "good" state
    (e.g. a stream is open and ready to read), explicitly invoke a corresponding member function
    to make it obvious what you're doing, rather than making use of implicit conversions to `bool`
    or the unary `!` operator. (Exceptions can be made for cases where invoking a method returns
    a reference to the object itself, and using the function result as if it was a `bool`
    has the effect of testing whether the method invocation was successful.)


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

Sometimes you may run across legacy code that violates the above rules. The following guidelines should give
you an idea of how to proceed:

  - If you're going to touch a file just for the sake of fixing legacy coding style, please go all
    the way and overhaul the entire file.
  - If you're going to touch a file for minor code changes, please adapt to the file's existing
    coding style (unless the file's style is a hodgepodge already, in which case please lean towards
    the above rules), or go all the way and overhaul the entire file.
  - If you're going to touch a file for larger code changes, please overhaul the coding style of the
    entire respective section (e.g. function, class definition etc.)
