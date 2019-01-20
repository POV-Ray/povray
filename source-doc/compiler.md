@page compiler  Compiler Requirements


Compiling POV-Ray requires a compiler compatible with the C++11 language standard as defined by
ISO/IEC 14882:2011. However, the following additional requirements apply.


Language Extensions
===================

Compiling POV-Ray will require support for the following language extensions beyond C++11:

  - (none).


Exotic Environments
===================

The following properties are categorized as "implementation-defined" (meaning that they may vary
across compilers), but we currently consider any deviations from the most common behaviour too
exotic to cater to:

  - **Char Size**: The `char` data type is currently presumed to be exactly 8 bits wide.

  - **Pointer Size**: All pointers are presumed to be of the same width.

  - **Character Encoding**: The compiler is presumed to accept ASCII-encoded source files; the _source character set_,
    as well as the _basic execution character set_ and its encoding, are presumed to be ASCII-compatible.

  - **Include File Names**: The compiler is presumed to accept relative file names, with forward slash (`/`) as path
    separator, in include directives.

  - **Integer Division**: Integer division is presumed to round towards zero. This also implies that the remainder (as
    computed by the modulus operator `%`) is negative (or zero) if the dividend and divisor have different signs.

Failure to meet the above restrictions will result in undefined behaviour.

Further restrictions may apply, but shall prompt a compile-time error if not satisfied.


Incompatibility Creep
=====================

The following limitations have also crept in over time, due to contemporary implementations rarely
deviating from them. New code is strongly discouraged from relying on them, but some legacy code may
still rely on them:

  - **Int Size**: Some existing code may currently presume the `int` data type to be exactly
    32 bits wide.

  - **Signed Integers**: Some existing code may currently presume signed integers to be stored
    in two's complement format.


POVMS Additional Restrictions
=============================

To support network rendering (a feature not yet implemented but planned) in a heterogenous network, the POVMS interface
imposes the following additional restriction on the compiler:

  - **Multicharacter Literals**: The internal representation of a 4-character _multicharacter literal_ is presumed to
    be identical to the first 4 bytes of a corresponding string literal (e.g. `'abcd'` is presumed to have the same
    binary representation as `"abcd"` would have without the terminating _null_ character).
    @todo
        Verify that this restriction does indeed exist, or whether mechanisms are already in place
        to detect and work around differing behaviour.
