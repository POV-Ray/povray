//******************************************************************************
///
/// @file backend/vm/fncode.h
///
/// This module contains all defines, typedefs, and prototypes for `fncode.cpp`.
///
/// This module is inspired by code by D. Skarda, T. Bily and R. Suzuki.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef FNCODE_H
#define FNCODE_H

#include "backend/frame.h"
#include "base/textstream.h"
#include "parser/parser.h"


namespace pov
{

#ifndef DEBUG_FLOATFUNCTION
 #define DEBUG_FLOATFUNCTION 0
#endif

#define FN_INLINE_FLAG 1
#define FN_LOCAL_FLAG  2

#define MAX_K ((unsigned int)0x000fffff)
#define MAX_FN MAX_K

#define MAKE_INSTRUCTION(op, k) ((((k) << 12) & 0xfffff000) | ((op) & 0x00000fff))

#define GET_OP(w) ((w) & 0x00000fff)
#define GET_K(w) (((w) >> 12) & 0x000fffff)


typedef void *(*FNCODE_PRIVATE_COPY_METHOD)(void *);
typedef void (*FNCODE_PRIVATE_DESTROY_METHOD)(void *);

typedef unsigned int Instruction;

struct FunctionCode
{
    Instruction *program;
    unsigned int program_size;
    unsigned char return_size;
    unsigned char parameter_cnt;
    unsigned char localvar_cnt;
    unsigned int localvar_pos[MAX_PARAMETER_LIST];
    char *localvar[MAX_PARAMETER_LIST];
    char *parameter[MAX_PARAMETER_LIST];
    char *name;
    UCS2 *filename;
    pov_base::ITextStream::FilePos filepos;
    unsigned int flags;
    FNCODE_PRIVATE_COPY_METHOD private_copy_method;
    FNCODE_PRIVATE_DESTROY_METHOD private_destroy_method;
    void *private_data;
};

void FNCode_Copy(FunctionCode *, FunctionCode *);
void FNCode_Delete(FunctionCode *);

class FNCode
{
    public:
        FNCode(Parser *, FunctionCode *, bool, const char *);

        void Parameter();
        void Compile(Parser::ExprNode *);
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

        void compile_recursive(Parser::ExprNode *expr);
        void compile_member(char *name);
        void compile_call(Parser::ExprNode *expr, FUNCTION fn, int token, char *name);
        void compile_select(Parser::ExprNode *expr);
        void compile_seq_op(Parser::ExprNode *expr, unsigned int op, DBL neutral);
        void compile_float_function_call(Parser::ExprNode *expr, FUNCTION fn, char *name);
        void compile_vector_function_call(Parser::ExprNode *expr, FUNCTION fn, char *name);
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

#endif
