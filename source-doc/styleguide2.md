@page styleguide2  Nitpicker's Coding Styleguide


To avoid boring you to death with coding rules, we decided to not include the following in our main coding styleguide
for one or more of the following reasons:

  - They deal with special cases.
  - They can, in our view, be taken for granted.
  - They are of lesser importance to us.
  - They can be easily checked for, and con-compliant code can easily be fixed.

We don't really expect you to actually read the following; if you have the stamina though, and want to learn more about
good coding practices, be our guest. These rules are heavily inspired by the Motor Industry Software Reliability
Association's publication "MISRA C++:2008 -- Guidelines for the use of the C++ language in critical systems", an
industry-wide accepted standard for writing robust C++ code. (However, if you're reading the source code of this
documentation page, be aware that the HTML comments citing individual MISRA rules are included for reference only, and
that our rules may differ substantially.)


@note
    Entries in parentheses are placeholders for concerns we haven't addressed yet. Most of these
    correspond to rules from the MISRA C++ 2008 guidelines.

@todo
    Flesh out the placeholders.

  - **Unused or useless code**: There should generally be no unreachable, orphaned, dead or
    pointless code, nor any unused variables or types. The following are valid exceptions:
      - Some feature needs to be disabled for a hotfix, and is intended to be re-enabled as soon as
        a proper fix has been implemented.
      - Helper code is already being developed (or being retained from dropped features) in
        anticipation of a feature that will use it.
      .
    In any case, the rationale for dragging along such unused code should be well documented in the
    source code.
    <!-- MISRA C++ 2008 **required** rules 0-1-1...0-1-6, 0-1-8...0-1-12, 14-7-1 -->

  - **Unused return values**: Whenever the return value of a function is ignored, it must be
    explicitly cast to `(void)` to indicate that this is intentional.
    <!-- MISRA C++ 2008 **required** rule 0-1-7 -->

  - **Unions**: Never use unions. Any deviation from this rule needs an exceptionally strong reason
    for it.
    <!-- MISRA C++ 2008 **required** rules 0-2-1, 9-5-1 -->

  - **Run-Time Checks**: When writing code, give thought to possible run-time errors, such as
    arithmetic errors (numeric overflow, division by zero, limits of floating-point precision,
    etc.), failed memory allocations, or bogus parameter values. Most notably, if a called function
    may indicate an error, make sure to test for that error indication and handle it appropriately.
    <!-- MISRA C++ 2008 document rule 0-3-1 (c), **required** rule 0-3-2 -->

    <!-- MISRA C++ 2008 document rule 0-4-1 (Use of scaled-integer or fixed-point arithmetic shall be documented)
         IGNORED: We're not using any of these... or are we? -->
    <!-- MISRA C++ 2008 document rule 0-4-2 (Use of floating-point arithmetic shall be documented)
         IGNORED: We're using floating-points virtually everywhere, and can't do without for obvious reasons. -->
    <!-- MISRA C++ 2008 document rule 0-4-3 (Floating-point implementations shall comply with a defined floating-point
         standard)
         IGNORED: This depends on the user's choice of compiler, and is therefore beyond the scope of any
         platform-independent project. -->

  - **Language Standard**: Source code should be written in C++ adhering to the ISO/IEC 14882:2011 standard (aka C++11).
    Compiler-specific language extensions may only be used in the dedicated headers intended to _hide_ compiler-specific
    implementation details.
    <!-- MISRA C++ 2008 **required** rule 1-0-1 -->

    <!-- MISRA C++ 2008 document rule 1-0-2 (Multiple compilers shall only be used if they have a common, defined
         interface)
         IGNORED: This depends on the user's choice of compiler, and is therefore beyond the scope of any
         platform-independent project. -->

  - **Negative Integer Division**: Source code should avoid division or modulus operations on
    negative integers, or should not rely on a particular rounding mode (towards negative infinity
    or towards zero) in such cases.
    <!-- MISRA C++ 2008 document rule 1-0-3 -->

  - **Character Encoding**: Source files should be plain vanilla ASCII, and should not use _universal character name
    notation_ (`\uXXXX` or `\UXXXXXXXX`). String literals should not encode any non-ASCII character by means of escape
    sequences.
    <!-- MISRA C++ 2008 document rule 2-2-1 -->

  - **Tri- and Digraphs**: Neither tri- nor digraphs should be used in the source code. Multiple
    consecutive question marks (`??`) in string literals should be escaped (`?\?`) to prevent the
    compiler from interpreting them as trigraphs.
    <!-- MISRA C++ 2008 **required** rule 2-3-1, advisory rule 2-5-1 -->

  - **Comments**: Do not nest block quotes; as a matter of fact, do not use `/*` in any comment
    (neither block nor single-line style).
    <!-- MISRA C++ 2008 **required** rule 2-7-1 -->
  - Do not use comments (neither block nor single-line style) to disable code.
    <!-- MISRA C++ 2008 **required** rule 2-7-2, advisory rule 2-7-3 -->

  - **Identifier Names**: We expect any sane developer to be using a fixed-pitch font with reasonably
    discernable characters, including (but not limited to) the notorious lowercase letter `l` and
    digit `1`. However, try to avoid cryptic identifiers with a high percentage of such problematic
    characters. Well, actually, better avoid _any_ cryptic identifiers.
    <!-- MISRA C++ 2008 **required** rule 2-10-1 -->
  - Avoid name collisions with identifiers in an outer scope ("shadowing").
    <!-- MISRA C++ 2008 **required** rule 2-10-2 -->

  .
  - (a typedef name shall be a unique identifier) <!-- MISRA C++ 2008 **required** rule 2-10-3 -->
  - (a class, union or enum name shall be a unique identifier) <!-- MISRA C++ 2008 **required** rule 2-10-4 -->
  - (the identifier name of a non-member object or function with static storage duration shall not be reused) <!-- MISRA C++ 2008 advisory rule 2-10-5 -->
  - (if an identifier refers to a type, it shall not also refer to an object or a function in the same scope) <!-- MISRA C++ 2008 **required** rule 2-10-6 -->
  .

  - **Escape Sequences**: Do not use any non-standard escape sequences.
    <!-- MISRA C++ 2008 **required** rule 2-13-1 -->

  - **Octal Constants**: Do not use octal constants or octal escape sequences.
    <!-- MISRA C++ 2008 **required** rule 2-13-2 -->

  - **Hexadecimal Constants**: When using hexadecimal constants, always explicitly specify them as
    signed or unsigned by append either a `u` or `s` suffix.
    <!-- MISRA C++ 2008 **required** rule 2-13-3 -->

  .
  - (Lower case literal suffix) <!-- MISRA C++ 2008 **required** rule 2-13-4 -->
  .

  - **Wide String Literals**: Do not mix narrow and wide string literals.
    <!-- MISRA C++ 2008 **required** rule 2-13-5 -->

  .
  - (object/function definitions in header files) <!-- MISRA C++ 2008 **required** rule 3-1-1 -->
  - (function not declared at file scope) <!-- MISRA C++ 2008 **required** rule 3-1-2 -->
  - (zero-dimensioned array) <!-- MISRA C++ 2008 **required** rule 3-1-3 -->

  - (all declarations of an object or function shall have compatible types) <!-- MISRA C++ 2008 **required** rule 3-2-1 -->
  - (the One Definition Rule shall not be violated) <!-- MISRA C++ 2008 **required** rule 3-2-2 -->
  - (a type, object or function that is used in multiple translation units shall be declared in one and only one file.) <!-- MISRA C++ 2008 **required** rule 3-2-3 -->
  - (an identifier with external linkage shall have exactly one definition) <!-- MISRA C++ 2008 **required** rule 3-2-4 -->

  - (objects or functions with external linkage shall be declared in a header file) <!-- MISRA C++ 2008 **required** rule 3-3-1 -->
  - (if a function has internal linkage then all re-declarations shall include the static storage class specifier) <!-- MISRA C++ 2008 **required** rule 3-3-2 -->

  - (an identifier declared to be an object or type shall be defined in a block that minimizes its visibility) <!-- MISRA C++ 2008 **required** rule 3-4-1 -->

  - (The types used for an object, a function return type, or a function parameter shall be token-for-token identical in all declarations and re-declarations) <!-- MISRA C++ 2008 **required** rule 3-9-1 -->
  - (typedefs that indicate size and signedness should be used in place of the basic numerical types) <!-- MISRA C++ 2008 advisory rule 3-9-2 -->
  - (the underlying bit representations of floating-point values shall not be used) <!-- MISRA C++ 2008 **required** rule 3-9-3 -->

  - (Expressions with type bool shall not be used as operands to built-in operators other than the assignment operator =,
    the logical operators &&, ||, !, the equality operators == and !=, the unary & operator, and the conditional operator) <!-- MISRA C++ 2008 **required** rule 4-5-1 -->
  - (Expressions with type enum shall not be used as operands to built-in operators other than the subscript operator [],
    the assignment operator =, the equality operators == and !=, the unary & operator, and the relational operators <, <=, >, >=) <!-- MISRA C++ 2008 **required** rule 4-5-2 -->
  - (expressions with type (plain) char and wchar_t shall not be used as operands to built-in operators other than the
    assignment operator =, the equality operators == and !=, and the unary & operator) <!-- MISRA C++ 2008 **required** rule 4-5-3 -->

  - (NULL shall not be used as an integer value) <!-- MISRA C++ 2008 **required** rule 4-10-1 -->
  - (Literal zero (0) shall not be used as the null-pointer-constant) <!-- MISRA C++ 2008 **required** rule 4-10-2 -->

  - (The value of an expression shall be the same under any order of evaluation that the standard permits) <!-- MISRA C++ 2008 **required** rule 5-0-1 -->
  - (Limited precedence should be placed on C++ operator precedence rules in expressions) <!-- MISRA C++ 2008 advisory rule 5-0-2 -->
  - (A cvalue expression shall not be implicitly converted to a different underlying type) <!-- MISRA C++ 2008 **required** rule 5-0-3 -->
  - (An implicit integral conversion shall not change the signedness of the underlying type) <!-- MISRA C++ 2008 **required** rule 5-0-4 -->
  - (There shall be no implicit floating-integral conversions) <!-- MISRA C++ 2008 **required** rule 5-0-5 -->
  - (An implicit integral or floating-point conversion shall not reduce the size of the underlying type) <!-- MISRA C++ 2008 **required** rule 5-0-6 -->
  - (There shall be no explicit floating-integral conversions of a cvalue expression) <!-- MISRA C++ 2008 **required** rule 5-0-7 -->
  - (An explicit integral or floating-point conversion shall not increase the size of the underlying type of a cvalue expression) <!-- MISRA C++ 2008 **required** rule 5-0-8 -->
  - (An explicit integral conversion shall not change the signedness of the underlying type of a cvalue expression) <!-- MISRA C++ 2008 **required** rule 5-0-9 -->
  - (If the bitwise operators '~' and '<<' are applied to an operand with an underlying type of signed char or unsigned short,
    the result shall be immediately cast to the underlying type of the operand) <!-- MISRA C++ 2008 **required** rule 5-0-10 -->
  - (The plain char type shall only be used for the storage and use of character values) <!-- MISRA C++ 2008 **required** rule 5-0-11 -->
  - (signed char and unsigned char type shall only be used for the storage and use of numeric values) <!-- MISRA C++ 2008 **required** rule 5-0-12 -->
  - (the condition of an if-statement or iteration-statement shall have type bool) <!-- MISRA C++ 2008 **required** rule 5-0-13 -->
  - (the first operand of a conditional-operator shall have type bool) <!-- MISRA C++ 2008 **required** rule 5-0-14 -->
  - (array indexing shall be the only form of pointer arithmetic) <!-- MISRA C++ 2008 **required** rule 5-0-15 -->
  - (out-of-bounds pointer) <!-- MISRA C++ 2008 **required** rule 5-0-16 -->
  - (subtraction between pointers shall only be applied to pointers that address elements of the same array) <!-- MISRA C++ 2008 **required** rule 5-0-17 -->
  - (>, >=, <, <= shall not be applied to objects of pointer type, except where they point to the same array) <!-- MISRA C++ 2008 **required** rule 5-0-18 -->
  - (The declaration of objects shall contain no more than two levels of indirection) <!-- MISRA C++ 2008 **required** rule 5-0-19 -->
  - (Non-constant operands to a binary bitwise operator shall have the same underlying type) <!-- MISRA C++ 2008 **required** rule 5-0-20 -->
  - (Bitwise operators shall only be applied to operands of unsigned underlying type) <!-- MISRA C++ 2008 **required** rule 5-0-21 -->

  - ((Operands of logical operators && or || need to be parenthesized properly, except in a sequence of all-identical operators)) <!-- MISRA C++ 2008 **required** rule 5-2-1 -->
  - (A pointer to a virtual base class shall only be cast to a pointer to a derived class by means of dynamic_cast) <!-- MISRA C++ 2008 **required** rule 5-2-2 -->
  - (Casts from a base class to a derived class should not be performed on polymorphic types) <!-- MISRA C++ 2008 advisory rule 5-2-3 -->
  - (C-style casts (other than void casts) and functional notation casts (other than explicit constructor calls) shall not be used) <!-- MISRA C++ 2008 **required** rule 5-2-4 -->
  - (A cast shall not remove any const or volatile qualification from the type of a pointer or reference) <!-- MISRA C++ 2008 **required** rule 5-2-5 -->
  - (a cast shall not convert a pointer to a function to any other pointer type) <!-- MISRA C++ 2008 **required** rule 5-2-6 -->
  - (an object with pointer type shall not be converted to an unrelated type) <!-- MISRA C++ 2008 **required** rule 5-2-7 -->
  - (an object with integral type or pointer to void type shall not be converted to an object with pointer type) <!-- MISRA C++ 2008 advisory rule 5-2-8 -->
  - (a cast should not convert a pointer type to an integral type) <!-- MISRA C++ 2008 advisory rule 5-2-9 -->
  - (the increment (++) and decrement (--) operators should not be mixed with other operators in an expression) <!-- MISRA C++ 2008 advisory rule 5-2-10 -->
  - (the comma operator, && operator and the || operator shall not be overloaded) <!-- MISRA C++ 2008 **required** rule 5-2-11 -->
  - (An identifier with array type passed as a function argument shall not decay into a pointer) <!-- MISRA C++ 2008 **required** rule 5-2-12 -->

  - (Each of the ! operator, the logical && or the logical || operators shall have type bool) <!-- MISRA C++ 2008 **required** rule 5-3-1 -->
  - (The unary minus operator shall not be applied to an expression whose underlying type is unsigned) <!-- MISRA C++ 2008 **required** rule 5-3-2 -->
  - (The unary & operator shall not be overloaded) <!-- MISRA C++ 2008 **required** rule 5-3-3 -->
  .

  - **Sizeof Operator**: The parameter to the sizeof operator should always be a variable, and
    nothing but a variable.
    <!-- MISRA C++ 2008 **required** rule 5-3-4 -->

  .
  - (The right hand operand of a shift operator shall lie between zero and none less than the width in bits of the underlying type of the left operand) <!-- MISRA C++ 2008 **required** rule 5-8-1 -->

  - (The right hand operand of a logical && or || operator shall not contain side effects) <!-- MISRA C++ 2008 **required** rule 5-14-1 -->

  - (The semantic equivalence between a binary operator and its assignment operator form shall be preserved) <!-- MISRA C++ 2008 **required** rule 5-17-1 -->
  .

  - **Comma Operator**: Do not use the comma operator.
    <!-- MISRA C++ 2008 **required** rule 5-18-1 -->

  .
  - (Evaluation of constant unsigned integer expressions should not lead to wrap-around) <!-- MISRA C++ 2008 advisory rule 5-19-1 -->

  - (Assignment operators shall not be used in sub-expressions) <!-- MISRA C++ 2008 **required** rule 6-2-1 -->
  - (Floating-point expressions shall not be tested for equality or inequality) <!-- MISRA C++ 2008 **required** rule 6-2-2 -->
  - (a null statement shall only occur on a line by itself) <!-- MISRA C++ 2008 **required** rule 6-2-3 -->

  - (The body of a switch, while, do...while or for statement shall be a compound statement) <!-- MISRA C++ 2008 **required** rule 6-3-1 -->

  - (an if() shall be followed by a compound statement. an else shall be followed by either a compound statement, or another if) <!-- MISRA C++ 2008 **required** rule 6-4-1 -->
  - (an if...else if construct shall be terminated with an else clause) <!-- MISRA C++ 2008 **required** rule 6-4-2 -->
  - (a switch statement shall be _well-formed_ (no labels, jumps or declarations outside compound statements)) <!-- MISRA C++ 2008 **required** rule 6-4-3 -->
  - (a switch-label shall only be used when the most closely-enclosing compound statement is the body of a switch statement) <!-- MISRA C++ 2008 **required** rule 6-4-4 -->
  .

  - **Switch Fallthrough**: Intentional fall-through in a switch statement should generally be
    avoided. If used at all, it must be explicitly indicated with a `// FALLTHROUGH` comment.
    (Please use this exact spelling, to facilitate automatic code analysis.)
    <!-- MISRA C++ 2008 **required** rule 6-4-5 -->

  - **Switch Default**: Each switch statement should have a default as its last block. If it is not
    empty, it should end with a `break` statement.
    <!-- MISRA C++ 2008 **required** rule 6-4-6 -->

  .
  - (the condition of a switch statement shall not have bool type) <!-- MISRA C++ 2008 **required** rule 6-4-7 -->
  - (every switch statement shall have at least one case-clause) <!-- MISRA C++ 2008 **required** rule 6-4-8 -->

  - (a for loop shall contain a single loop-counter which shall not have floating type) <!-- MISRA C++ 2008 **required** rule 6-5-1 -->
  - (if loop-counter is not modified by -- or ++, then, within condition, the loop-counter shall only be used as an operand to <=, <, > or >=) <!-- MISRA C++ 2008 **required** rule 6-5-2 -->
  - (the loop-counter shall not be modified within condition or statement) <!-- MISRA C++ 2008 **required** rule 6-5-3 -->
  - (the loop-counter shall be modified by one of: --, ++, -=const, +=const) <!-- MISRA C++ 2008 **required** rule 6-5-4 -->
  - (other variables used for early loop termination shall not be modified within condition or expression) <!-- MISRA C++ 2008 **required** rule 6-5-5 -->
  - (other variables used for early loop termination shall have type bool) <!-- MISRA C++ 2008 **required** rule 6-5-6 -->
  .

  - **Goto Statements**: They are evil. Do not use them.
    <!-- MISRA C++ 2008 **required** rules 6-6-1, 6-6-2 -->

  .
  - (The continue statement shall only be used within a _well-formed_ for loop (i.e. that satisifies rules 6-5-1 to 6-5-6) <!-- MISRA C++ 2008 **required** rule 6-6-3 -->
  - (For any iteration statement there shall be no more than one break or goto statement used for loop termination) <!-- MISRA C++ 2008 **required** rule 6-6-4 -->
  - (A function shall have a single point of exit at the end of the function) <!-- MISRA C++ 2008 **required** rule 6-6-5 -->

  - (A variable which is not modified shall be const qualified) <!-- MISRA C++ 2008 **required** rule 7-1-1 -->
  - (A pointer or reference parameter in a function shall be declared as pointer to const or reference to const if the corresponding object is not modified) <!-- MISRA C++ 2008 **required** rule 7-1-2 -->

  - (An expression with enum underlying type shall only have values corresponding to the enumerators of the enumeration) <!-- MISRA C++ 2008 **required** rule 7-2-1 -->

  - (The global namespace shall only contain main, namespace declarations and extern "C" declarations) <!-- MISRA C++ 2008 **required** rule 7-3-1 -->
  - (The identifier main shall not be used for a function other than the global function main) <!-- MISRA C++ 2008 **required** rule 7-3-2 -->
  - (There shall be no unnamed namespaces in header files) <!-- MISRA C++ 2008 **required** rule 7-3-3 -->
  - (Using-directives shall not be used) <!-- MISRA C++ 2008 **required** rule 7-3-4 -->
  - (Multiple declarations for an identifier in the same namespace shall not straddle a using-declaration for that identifier) <!-- MISRA C++ 2008 **required** rule 7-3-5 -->
  - (Using-directives/declarations (excluding class scope or function scope using-declarations) shall not be used in header file) <!-- MISRA C++ 2008 **required** rule 7-3-6 -->

  - (All usage of assembler shall be documented) <!-- MISRA C++ 2008 document rule 7-4-1 -->
  - (Assembler instructions shall only be introduced using the asm directive) <!-- MISRA C++ 2008 **required** rule 7-4-2 -->
  - (Assembly language shall be encapsulated and isolated) <!-- MISRA C++ 2008 **required** rule 7-4-3 -->

  - (a function shall not return a reference or pointer to an automatic variable (i.e. non-static local or non-reference parameter)) <!-- MISRA C++ 2008 **required** rule 7-5-1 -->
  - (the address of an object with automatic storage shall not be assigned to another object that may persist after the first object has ceased to exist) <!-- MISRA C++ 2008 **required** rule 7-5-2 -->
  - (a function shall not return a reference or a pointer to a parameter that is passed by reference or const reference) <!-- MISRA C++ 2008 **required** rule 7-5-3 -->
  - (Functions shall not call themselves, either directly or indirectly) <!-- MISRA C++ 2008 advisory rule 7-5-4 -->

  - (An init-declarator-list or a member-declarator-list shall consist of a single init-declarator or member-declarator respectively) <!-- MISRA C++ 2008 **required** rule 8-0-1 -->

  - (Parameters in an overriding virtual function shall either use the same default arguments as the function they override,
    or else shall not specify any default parameters) <!-- MISRA C++ 2008 **required** rule 8-3-1 -->

  - (Functions shall not be defined using the ellipsis notation) <!-- MISRA C++ 2008 **required** rule 8-4-1 -->
  - (The identifiers used for the parameters in a re-declaration of a function shall be identical to those in the declaration) <!-- MISRA C++ 2008 **required** rule 8-4-2 -->
  - (All exit paths from a function with non-void return type shall have an explicit return statement with an expression) <!-- MISRA C++ 2008 **required** rule 8-4-3 -->
  - (A function identifier shall either be used to call the function or it shall be preceded wby &) <!-- MISRA C++ 2008 **required** rule 8-4-4 -->

  - (All variables shall have a defined value before they are used) <!-- MISRA C++ 2008 **required** rule 8-5-1 -->
  - (Braces shall be used to indicate and match the structure in the non-zero initialization of arrays and structures) <!-- MISRA C++ 2008 **required** rule 8-5-2 -->
  - (In enumerator lists, the = construct shall not be used to explicitly initialize members other than the first, unless all items are explicitly initialized) <!-- MISRA C++ 2008 **required** rule 8-5-3 -->

  - (const member functions shall not return non-const pointers or reference to class-data) <!-- MISRA C++ 2008 **required** rule 9-3-1 -->
  - (member functions shall not return non-const handles to class-data) <!-- MISRA C++ 2008 **required** rule 9-3-2 -->
  - (If a member function can be made static then it shall be made static, otherwise if it can be made const then it shall be made const) <!-- MISRA C++ 2008 **required** rule 9-3-3 -->

  - (When the absolute positioning of bits representing a bit-field is required, then the behaviour and packing of bit-fields shall be documented) <!-- MISRA C++ 2008 document rule 9-6-1 -->
  - (bit-fields shall be either bool type or an explicitly unsigned or signed integral type) <!-- MISRA C++ 2008 **required** rule 9-6-2 -->
  - (bit fields shall not have enum type) <!-- MISRA C++ 2008 **required** rule 9-6-3 -->
  - (named bit-fields with signed integer type shall have a length of more than one bit) <!-- MISRA C++ 2008 **required** rule 9-6-4 -->

  - (Classes should not be derived from virtual bases (i.e., `class B: public virtual A`)) <!-- MISRA C++ 2008 advisory rule 10-1-1 -->
  - (A base class shall only be declared virtual (i.e., `class B: public virtual A`) if it is used in a diamond hierarchy.) <!-- MISRA C++ 2008 **required** rule 10-1-2 -->
  - (An accessible base class shall not be both virtual (i.e., `class B: public virtual A`) and non-virtual in the same hierarchy) <!-- MISRA C++ 2008 **required** rule 10-1-3 -->

  - (All accessible entity names within a multiple inheritance hierarchy should be unique) <!-- MISRA C++ 2008 advisory rule 10-2-1 -->

  - (There shall be no more than one definition of each virtual function on each path through the inheritance hierarchy) <!-- MISRA C++ 2008 **required** rule 10-3-1 -->
  - (Each overriding virtual function shall be declared with the 'virtual' keyword) <!-- MISRA C++ 2008 **required** rule 10-3-2 -->
  - (A virtual function shall only be overridden by a pure virtual function if it is itself declared as pure virtual) <!-- MISRA C++ 2008 **required** rule 10-3-3 -->

  - (Member data in non-POD class types shall be private) <!-- MISRA C++ 2008 **required** rule 11-0-1 -->

  - (An object's dynamic type shall not be used from the body of its constructor or destructor (i.e. no virtual function calls, no use of typeid or dynamic_cast)) <!-- MISRA C++ 2008 **required** rule 12-1-1 -->
  - (All constructors of a class should explicitly call a constructor of all of its immediate base classes and all virtual base classes) <!-- MISRA C++ 2008 advisory rule 12-1-2 -->
  - (All constructors that are callable with a single argument of fundamental type shall be declared `explicit`) <!-- MISRA C++ 2008 **required** rule 12-1-3 -->

  - (A copy constructor shall only initialize its base classes and the non-static members of the class of which it is a member) <!-- MISRA C++ 2008 **required** rule 12-8-1 -->
  - (The copy assignment operator shall be declared protective or private in an abstract class) <!-- MISRA C++ 2008 **required** rule 12-8-2 -->

  - [!] (A non-member generic function shall only be declared in a template that is not an associated namespace) <!-- MISRA C++ 2008 **required** rule 14-5-1 -->
  - [!] (A copy constructor shall be declared when there is a template constructor with a single parameter that is a generic parameter) <!-- MISRA C++ 2008 **required** rule 14-5-2 -->
  - [!] (A copy assignment operator shall be declared when there is a template assignment operator with a parameter that is a generic parameter) <!-- MISRA C++ 2008 **required** rule 14-5-3 -->

  - [!] (In a class template with a dependent base, any name that may be found in that dependent base shall be referred to using a qualified-id or `this->`) <!-- MISRA C++ 2008 **required** rule 14-6-1 -->
  - [!] (The function chosen by overload resolution shall resolve to a function declared previously in the translation unit) <!-- MISRA C++ 2008 **required** rule 14-6-2 -->

  - (For any given template specialization, an explicit instantiation of the template with the template-arguments used in the
    specialization shall not render the program ill-formed) <!-- MISRA C++ 2008 **required** rule 14-7-2 -->
  - (all partial and explicit specializations for a template shall be declared in the same file as the declaration of their primary template) <!-- MISRA C++ 2008 **required** rule 14-7-3 -->

  - (Overloaded function templates shall not be explicitly specialized) <!-- MISRA C++ 2008 **required** rule 14-8-1 -->
  - (The viable function set for a function call should either contain no function specializations, or only contain function specializations) <!-- MISRA C++ 2008 advisory rule 14-8-2 -->

  - (Exceptions shall only be used for error handling) <!-- MISRA C++ 2008 document rule 15-0-1 -->
  - (An exception object should not have pointer type) <!-- MISRA C++ 2008 advisory rule 15-0-2 -->
  - (Control shall not be transferred into a try or catch block using a goto or a switch statement) <!-- MISRA C++ 2008 **required** rule 15-0-3 -->

  - (The assignment-expression of a throw statement shall not itself cause an exception to be thrown) <!-- MISRA C++ 2008 **required** rule 15-1-1 -->
  - (`NULL` shall not be thrown explicit) <!-- MISRA C++ 2008 **required** rule 15-1-2 -->
  - (An empty throw shall only be used in the compound-statement of a catch handler) <!-- MISRA C++ 2008 **required** rule 15-1-3 -->

  - (Exceptions shall be raised only after start-up and before termination of the program (i.e., exceptions shall not be thrown in static constructors/destructors)) <!-- MISRA C++ 2008 **required** rule 15-3-1 -->
  - (There should be at least one exception handler to catch all otherwise unhandled exceptions) <!-- MISRA C++ 2008 advisory rule 15-3-2 -->
  - (Handlers of a function-try-block implementation of a class constructor or destructor shall not reference any non-static members from this class or its bases) <!-- MISRA C++ 2008 **required** rule 15-3-3 -->
  - (Each exception explicitly thrown in the code shall have a handler of compatible type in all call paths that could lead to that point) <!-- MISRA C++ 2008 **required** rule 15-3-4 -->
  - (A class type exception shall always be caught by reference) <!-- MISRA C++ 2008 **required** rule 15-3-5 -->
  - (Where multiple handlers are provided in a single try-catch statement or function-try-block for a derived class and some or all of its bases,
    the handlers shall be ordered most-derived to base class) <!-- MISRA C++ 2008 **required** rule 15-3-6 -->
  - (Where multiple handlers are provided in a single try-catch statement or function-try-block, any ellipsis handler shall occur last) <!-- MISRA C++ 2008 **required** rule 15-3-7 -->

  - (If a function is declared with an exception specification, then all declarations of the same function (in other translation units) shall be declared with the same set of type-ids) <!-- MISRA C++ 2008 **required** rule 15-4-1 -->

  - (A destructor shall not exit with an exception) <!-- MISRA C++ 2008 **required** rule 15-5-1 -->
  - (Where a function's declaration includes an exception specification, the function shall only be capable of throwing exceptions of the indicated types) <!-- MISRA C++ 2008 **required** rule 15-5-2 -->
  - (The terminate() function shall not be called implicitly (see earlier rules)) <!-- MISRA C++ 2008 **required** rule 15-5-3 -->

  - (`#``include` directives in a file shall only be preceded by other preprocessor directives or comments) <!-- MISRA C++ 2008 **required** rule 16-0-1 -->
  - (Macros shall only be defined or undefined in the global namespace) <!-- MISRA C++ 2008 **required** rule 16-0-2 -->
  - (`#``undef` shall not be used) <!-- MISRA C++ 2008 **required** rule 16-0-3 -->
  - (function-like macros shall not be defined) <!-- MISRA C++ 2008 **required** rule 16-0-4 -->
  - (Arguments to a function-like macro shall not contain tokens that look like preprocessing directives) <!-- MISRA C++ 2008 **required** rule 16-0-5 -->
  - (in the definition of a function-like macro, each instance of a parameter shall be enclosed in parentheses, unless it is used as the operand of `#` or `##`) <!-- MISRA C++ 2008 **required** rule 16-0-6 -->
  - (undefined macro identifiers shall not be used in `#``if` or `#``elif` preprocessor directives, except as operands to the `defined` operator) <!-- MISRA C++ 2008 **required** rule 16-0-7 -->
  - (if the `#` token appears as the first token on a line, then it shall be immediately followed by a preprocessing token) <!-- MISRA C++ 2008 **required** rule 16-0-8 -->

  - (The `defined` operator shall only be used in the form `defined ( identifier )` or `defined identifier`) <!-- MISRA C++ 2008 **required** rule 16-1-1 -->
  - (All `#``else`, `#``elif` and `#``endif` preprocessor directives shall reside in the same file as the `#``if` or `#``ifdef` directive to which they are related) <!-- MISRA C++ 2008 **required** rule 16-1-2 -->

  - (The preprocessor shall only be used for file inclusion and include guards) <!-- MISRA C++ 2008 **required** rule 16-2-1 -->
  - (macros shall only be used for include guards, type qualifiers, or storage class specifiers) <!-- MISRA C++ 2008 **required** rule 16-2-2 -->
  - (include guards shall be provided) <!-- MISRA C++ 2008 **required** rule 16-2-3 -->
  - (the ', ", /* or // character sequences shall not occur in a header file name) <!-- MISRA C++ 2008 **required** rule 16-2-4 -->
  - (the \ character should not occur in a header file name) <!-- MISRA C++ 2008 advisory rule 16-2-5 -->
  - (the `#``include` directive shall be followed by either a <filename> or "filename" sequence) <!-- MISRA C++ 2008 **required** rule 16-2-6 -->

  - (There shall be at most one occurrence of the `#` or `##` operators in a single macro definition) <!-- MISRA C++ 2008 **required** rule 16-3-1 -->
  - (The `#` and `##` operators should not be used) <!-- MISRA C++ 2008 advisory rule 16-3-2 -->

  - (All uses of the `#``pragma` directive shall be documented) <!-- MISRA C++ 2008 document rule 16-6-1 -->

  - (Reserved identifiers, macros and functions in the standard library shall not be defined, redefined or undefined) <!-- MISRA C++ 2008 **required** rule 17-0-1 -->
  - (The names of standard library macros and objects shall not be reused) <!-- MISRA C++ 2008 **required** rule 17-0-2 -->
  - (The names of standard library functions shall not be overridden) <!-- MISRA C++ 2008 **required** rule 17-0-3 -->
  .

    <!-- MISRA C++ 2008 **required** rule 17-0-4 (All library code shall conform to MISRA C++)
         IGNORED: We're using MISRA C++ only as a guideline for our own code, presuming that authors of well-established
         3rd party libraries take their own reasonable measures to assure good code quality. -->

  - **longjmp**: The `setjmp` macro and `longjmp` function are evil. Do not use them.
    <!-- MISRA C++ 2008 **required** rule 17-0-5 -->

  - **Standard Libraries**: Do not use the C libraries (e.g. `<stdio.h>`); use the corresponding C++
    libraries instead (e.g. `<cstdio>`).
    <!-- MISRA C++ 2008 **required** rule 18-0-1 -->
  - Do not use `atof`, `atoi`, or `atol`.
    <!-- MISRA C++ 2008 **required** rule 18-0-2 -->

  .
  - (The library functions abort, exit, getenv and system from <cstdlib> shall not be used) <!-- MISRA C++ 2008 **required** rule 18-0-3 -->
  - (The time handling functions of <ctime> shall not be used) <!-- MISRA C++ 2008 **required** rule 18-0-4 -->
  .

  - **Strings**: Do not use C-style strings, except as literals to initialize `std::string` objects.
    <!-- MISRA C++ 2008 MISRA C++ **required** rule 18-0-5 -->

  .
  - (The macro offsetof shall not be used) <!-- MISRA C++ 2008 **required** rule 18-2-1 -->
  .

    <!-- MISRA C++ 2008 **required** rule 18-4-1 (Dynamic heap memory allocation shall not be used)
         IGNORED: We're making heavy use of dynamic heap memory allocation and can't reasonably do without. -->

  .
  - (The signal handling facilities of <csignal> shall not be used) <!-- MISRA C++ 2008 **required** rule 18-7-1 -->

  - (The error indicator errno shall not be used) <!-- MISRA C++ 2008 **required** rule 19-3-1 -->

  - (The stream input/output library <cstdio> shall not be used) <!-- MISRA C++ 2008 **required** rule 27-0-1 -->
  .
