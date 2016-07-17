# Compiler Requirements {#compiler}


General Presumptions {#compiler_exot}
====================

While POV-Ray is being developed with portability high in mind, the C++ standard allows for some degrees of freedom that
we consider too exotic to cater to. We therefore presume all compilers and runtime environments to adhere to the
following additional restrictions:

  - **Char Size**: The `char` data type is currently presumed to be exactly 8 bits wide.

  - **Pointer Size**: All pointers are presumed to be of the same width.

  - **Character Encoding**: The compiler is presumed to accept ASCII-encoded source files; the _source character set_,
    as well as the _basic execution character set_ and its encoding, are presumed to be ASCII-compatible.

  - **Include File Names**: The compiler is presumed to accept relative file names, with forward slash (`/`) as path
    separator, in include directives.

  - **Integer Division**: Integer division is presumed to round towards zero. This also implies that the remainder (as
    computed by the modulus operator `%`) is negative (or zero) if the dividend and divisor have different sign.


POVMS Additional Restrictions {#compiler_povms}
=============================

To support network rendering (a feature not yet implemented but planned) in a heterogenous network, the POVMS interface
imposes the following additional restriction on the compiler:

  - **Multicharacter Literals**: The internal representation of a 4-character _multicharacter literal_ is presumed to
    be identical to the first 4 bytes of a corresponding string literal (e.g. `'abcd'` is presumed to have the same
    binary representation as `"abcd"` would have without the terminating _null_ character).
