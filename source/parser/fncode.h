//******************************************************************************
///
/// @file parser/fncode.h
///
/// Declarations related to the compilation of user-defined functions.
///
/// This module is inspired by code by D. Skarda, T. Bily and R. Suzuki.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#ifndef POVRAY_PARSER_FNCODE_H
#define POVRAY_PARSER_FNCODE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "parser/configparser.h"

#include "vm/fnpovfpu.h"

#include "parser/reservedwords.h"

namespace pov
{

#ifndef DEBUG_FLOATFUNCTION
    #define DEBUG_FLOATFUNCTION 0
#endif

#define FN_INLINE_FLAG 1
#define FN_LOCAL_FLAG  2

// Caution: The compiler depends on the order of these constants!
enum
{
    OP_NONE = 0,    // 0

    OP_CMP_EQ,      // 1
    OP_CMP_NE,      // 2
    OP_CMP_LT,      // 3
    OP_CMP_LE,      // 4
    OP_CMP_GT,      // 5
    OP_CMP_GE,      // 6
    OP_ADD,         // 7
    OP_SUB,         // 8
    OP_OR,          // 9
    OP_MUL,         // 10
    OP_DIV,         // 11
    OP_AND,         // 12
    OP_POW,         // 13
    OP_NEG,         // 14
    OP_NOT,         // 15

    OP_LEFTMOST,    // 16
    OP_FIRST,       // 17

    OP_CONSTANT,    // 18
    OP_VARIABLE,    // 19
    OP_DOT,         // 20
    OP_MEMBER,      // 21
    OP_CALL,        // 22
    OP_TRAP         // 23
};

class Parser;
class FunctionVM;

void FNCode_Copy(FunctionCode *, FunctionCode *);

struct ExprNode
{
    ExprNode *parent;
    ExprNode *child;
    ExprNode *prev;
    ExprNode *next;
    int stage;
    int op;
    union
    {
        char *variable;
        struct
        {
            char *name;
            TOKEN token;
            FUNCTION fn;
        } call;
        unsigned int trap;
        DBL number;
    };
};

class FNCode
{
    public:
        FNCode(Parser *, FunctionCode *, bool, const char *);

        void Parameter();
        void Compile(ExprNode *);
        void SetFlag(unsigned int, char *);
    private:
        FunctionCode *function;
        Parser *parser;
        FunctionVM *functionVM;

        unsigned int max_program_size;
        unsigned int max_stack_size;
        unsigned int stack_pointer;
        unsigned int parameter_stack_pointer;
        int level;

        #if (DEBUG_FLOATFUNCTION == 1)

        char *asm_input;
        char *asm_output;
        char *asm_error;

        #endif

        FNCode();
        FNCode(FNCode&);

        void compile_recursive(ExprNode *expr);
        void compile_member(char *name);
        void compile_call(ExprNode *expr, FUNCTION fn, int token, char *name);
        void compile_select(ExprNode *expr);
        void compile_seq_op(ExprNode *expr, unsigned int op, DBL neutral);
        void compile_float_function_call(ExprNode *expr, FUNCTION fn, char *name);
        void compile_vector_function_call(ExprNode *expr, FUNCTION fn, char *name);
        void compile_inline(FunctionCode *function);
        void compile_variable(char *name);
        void compile_parameters();
        unsigned int compile_push_result();
        void compile_pop_result(unsigned int local_k);
        unsigned int compile_instruction(unsigned int, unsigned int, unsigned int, unsigned int);
        unsigned int compile_instruction(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

        #if (DEBUG_FLOATFUNCTION == 1)

        int assemble(char *filename);
        int disassemble(char *filename);
        void disassemble_instruction(FILE *f, Instruction& i);

        unsigned int parse_instruction(FILE *f);
        unsigned int parse_reg(FILE *f);
        DBL parse_float(FILE *f);
        unsigned int parse_integer(FILE *f);
        unsigned int parse_move(FILE *f, unsigned int& r, unsigned int& k);
        void parse_comma(FILE *f);
        bool parse_comment(FILE *f);
        void skip_space(FILE *f);
        void skip_newline(FILE *f);

        #endif
};

}

#endif // POVRAY_PARSER_FNCODE_H
