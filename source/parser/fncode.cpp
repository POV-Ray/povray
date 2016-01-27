//******************************************************************************
///
/// @file parser/fncode.cpp
///
/// Implementations related to the compilation of user-defined functions.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "parser/fncode.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#include "vm/fnintern.h"
#include "vm/fnpovfpu.h"

#include "parser/parser.h"

#include "backend/scene/backendscenedata.h"

// this must be the last header file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
*
* FUNCTION
*
*   FNCode::FNCode
*
* INPUT
*
* OUTPUT
*
*   f - function to init
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Prepares a new function.
*
* CHANGES
*
*   -
*
******************************************************************************/

FNCode::FNCode(Parser *pa, FunctionCode *f, bool is_local, const char *n)
{
    unsigned int i = 0;

#if (DEBUG_FLOATFUNCTION == 1)
    asm_input = NULL;
    asm_output = NULL;
    asm_error = NULL;
#endif

    max_program_size = 0;
    level = 0;
    max_stack_size = 0;
    stack_pointer = 0;
    parameter_stack_pointer = 0;

    parser = pa;
    functionVM = dynamic_cast<FunctionVM*>(parser->sceneData->functionContextFactory);

    function = f;

    function->program = NULL;
    function->program_size = 0;
    function->return_size = 0; // zero implies return in register r0
    function->parameter_cnt = 0;
    function->localvar_cnt = 0;
    for(i = 0; i < MAX_FUNCTION_PARAMETER_LIST; i++)
    {
        function->localvar_pos[i] = 0;
        function->localvar[i] = NULL;
        function->parameter[i] = NULL;
    }

    if(n != NULL)
        function->sourceInfo.name = POV_STRDUP(n);
    else
        function->sourceInfo.name = POV_STRDUP("");
    function->sourceInfo.filename = pa->UCS2_strdup(parser->Token.FileHandle->name());
    if(parser->Token.FileHandle != NULL)
        function->sourceInfo.filepos = parser->Token.FileHandle->tellg();
    else
    {
        function->sourceInfo.filepos.lineno = 0;
        function->sourceInfo.filepos.offset = 0;
    }
    function->flags = 0;
    function->private_copy_method = NULL;
    function->private_destroy_method = NULL;
    function->private_data = NULL;

    if(is_local == true)
        function->flags |= FN_LOCAL_FLAG;
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::Parameter
*
* INPUT
*
* OUTPUT
*
*   f - function containing the parameter list
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Parse the parameter list of a function declaration.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::Parameter()
{
    parser->Get_Token();
    if(parser->Token.Token_Id == LEFT_PAREN_TOKEN)
    {
        for(function->parameter_cnt = 0;
            ((parser->Token.Token_Id != RIGHT_PAREN_TOKEN) || (function->parameter_cnt == 0)) && (function->parameter_cnt < MAX_FUNCTION_PARAMETER_LIST);
            function->parameter_cnt++)
        {
            parser->Get_Token();

            if((parser->Token.Function_Id != IDENTIFIER_TOKEN) && (parser->Token.Function_Id != X_TOKEN) &&
               (parser->Token.Function_Id != Y_TOKEN) && (parser->Token.Function_Id != Z_TOKEN) &&
               (parser->Token.Function_Id != U_TOKEN) && (parser->Token.Function_Id != V_TOKEN))
                parser->Expectation_Error("parameter identifier");

            function->parameter[function->parameter_cnt] = POV_STRDUP(parser->Token.Token_String);

            parser->Parse_Comma();
        }

        parser->Get_Token();

        if(function->parameter_cnt == 0)
            parser->Error("At least one function parameter is required!");
    }
    else
        parser->Unget_Token();
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::Compile
*
* INPUT
*
*   expression - expression tree that will be compiled
*
* OUTPUT
*
*   f - function containing the compiled code
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Take an expression tree and compiles it into code understood by the
*   virtual machine.
*
*     R0        R1        R2        R3        R4        R5        R6        R7
*   right 1   right 2     x,        y,        z,      left 1    left 2    left 3
*                       param 1   param 2   param 3
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::Compile(ExprNode *expression)
{
    unsigned int gpos = 0;

    // allocate some program memory in advance
    max_program_size = 256;
    function->program_size = 0;
    function->program = reinterpret_cast<Instruction *>(POV_MALLOC(sizeof(Instruction) * max_program_size, "fn: program"));

#if (DEBUG_FLOATFUNCTION == 1)
    if(asm_input != NULL)
    {
        asm_error = NULL;
        if(assemble(asm_input) < 0)
        {
            POV_FREE(asm_input);
            asm_input = NULL;
            Error("Assembler Error: %s", asm_error);
        }
        else if(asm_error != NULL)
        {
            POV_FREE(asm_input);
            asm_input = NULL;
            Expectation_Error("valid function expression");
        }
        POV_FREE(asm_input);
        asm_input = NULL;
    }
    else // this is intentional [trf]
#endif

    if(expression->op == OP_TRAP)
    {
        // use either trap or traps, depending on the expected return size
        if(function->return_size > 0)
        {
            // make sure the internal function exists
            if(expression->trap >= POVFPU_TrapSTableSize)
                parser->Error("Function 'internal(%d)' does not exist.", (int)(expression->trap));

            // call internal function
            compile_instruction(OPCODE_TRAPS, 0, 0, expression->trap);

            // set the basic information
            function->parameter_cnt = POVFPU_TrapSTable[expression->trap].parameter_cnt;
        }
        else
        {
            // make sure the internal function exists
            if(expression->trap >= POVFPU_TrapTableSize)
                parser->Error("Function 'internal(%d)' does not exist.", (int)(expression->trap));

            // call internal function
            compile_instruction(OPCODE_TRAP, 0, 0, expression->trap);

            // set the basic information
            function->parameter_cnt = POVFPU_TrapTable[expression->trap].parameter_cnt;
        }
    }
    else
    {
        // fill in "grow max_stack_size" later (see below)
        gpos = compile_instruction(OPCODE_NOP, 0, 0, 0);

        // load the parameters x, y, z into registers and
        // assign stack locations to all other parameters
        compile_parameters();

        // prepare to compile the recursive expression
        // note that compile_parameters may have changed
        // function->parameter_cnt [trf]
        level = 0;
        parameter_stack_pointer = 0;
        max_stack_size = function->parameter_cnt;
        stack_pointer = function->parameter_cnt;

        // compile the expression
        compile_recursive(expression);

        // fill in "grow max_stack_size" now
        compile_instruction(gpos, OPCODE_GROW, 0, 0, max_stack_size);
    }

    // return from function
    compile_instruction(OPCODE_RTS, 0, 0, 0);

    // set optimal size of program memory
    function->program = reinterpret_cast<Instruction *>(POV_REALLOC(function->program, sizeof(Instruction) * function->program_size, "fn: program"));

#if (DEBUG_FLOATFUNCTION == 1)
    if(asm_output != NULL)
    {
        asm_error = NULL;
        if(disassemble(asm_output) < 0)
        {
            POV_FREE(asm_output);
            asm_output = NULL;
            Error("Disassembler Error: %s", asm_error);
        }
        else if(asm_error != NULL)
        {
            POV_FREE(asm_output);
            asm_output = NULL;
            Expectation_Error("valid function expression");
        }
        POV_FREE(asm_output);
        asm_output = NULL;
    }
#endif
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode_Copy
*
* INPUT
*
*   f - source function
*
* OUTPUT
*
*   fnew - destination function
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Copy a compiled function.
*
* CHANGES
*
*   -
*
******************************************************************************/
/*
void FNCode_Copy(FunctionCode *f, FunctionCode *fnew)
{
    int i;

    if(f->program != NULL)
    {
        fnew->program = reinterpret_cast<Instruction *>(POV_MALLOC(sizeof(Instruction) * f->program_size, "fn: program"));
        POV_MEMCPY(fnew->program, f->program, sizeof(Instruction) * f->program_size);
    }
    if(f->name != NULL)
    {
        fnew->name = reinterpret_cast<char *>(POV_MALLOC((UCS2_strlen(f->name) + 1) * sizeof(UCS2), "fn: name"));
        UCS2_strcpy(fnew->name, f->name);
    }
    if(f->filename != NULL)
    {
        fnew->filename = reinterpret_cast<char *>(POV_MALLOC(strlen(f->filename) + 1, "fn: scene file name"));
        strcpy(fnew->filename, f->filename);
    }

    fnew->program_size = f->program_size;
    fnew->parameter_cnt = f->parameter_cnt;
    for(i = 0; i < fnew->parameter_cnt; i++)
        fnew->parameter[i] = POV_STRDUP(f->parameter[i]);
    fnew->localvar_cnt = f->localvar_cnt;
    for(i = 0; i < fnew->localvar_cnt; i++)
        fnew->localvar[i] = POV_STRDUP(f->localvar[i]);
    fnew->filepos.lineno = f->filepos.lineno;
    fnew->filepos.offset = f->filepos.offset;
    fnew->flags = f->flags;

    fnew->private_copy_method = f->private_copy_method;
    fnew->private_destroy_method = f->private_destroy_method;
    if(f->private_data != NULL)
    {
        if(f->private_copy_method != NULL)
            fnew->private_data = f->private_copy_method(f->private_data);
        else
            fnew->private_data = NULL;
    }
}
*/

/*****************************************************************************
*
* FUNCTION
*
*   FNCode::SetFlag
*
* INPUT
*
*   flag - compiler flag field
*   str - flag field data
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Set compiler flag.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::SetFlag(unsigned int flag, char *str)
{
#if (DEBUG_FLOATFUNCTION == 1)
    if(flag == 1)
    {
        if(asm_input != NULL)
            POV_FREE(asm_input);
        asm_input = POV_STRDUP(str);
    }
    else if(flag == 2)
    {
        if(asm_output != NULL)
            POV_FREE(asm_output);
        asm_output = POV_STRDUP(str);
    }
#else
    // silence compiler warnings
    flag = 0;
    str = NULL;
#endif
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_recursive
*
* INPUT
*
*   expr - expression (sub-) tree to compile
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Compiles an expression (sub-) tree recursivly.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_recursive(ExprNode *expr)
{
    POV_ASSERT(expr != NULL);

    unsigned int local_k = 0;

    if(expr->op <= OP_LEFTMOST)
        local_k = compile_push_result();

    for(ExprNode *i = expr; i != NULL; i = i->next)
    {
        if(i->child != NULL)
        {
            if(i->child->op == OP_CONSTANT)
            {
                switch(i->op)
                {
                    case OP_ADD:
                        if(i->child->number != 0.0)
                            compile_instruction(OPCODE_ADDI, 0, 5, functionVM->AddConstant(i->child->number));
                        continue;
                    case OP_SUB:
                        if(i->child->number != 0.0)
                            compile_instruction(OPCODE_SUBI, 0, 5, functionVM->AddConstant(i->child->number));
                        continue;
                    case OP_MUL:
                        if(i->child->number != 1.0)
                            compile_instruction(OPCODE_MULI, 0, 5, functionVM->AddConstant(i->child->number));
                        continue;
                    case OP_DIV:
                        if(i->child->number == 0.0)
                            parser->Error("Division by zero.");
                        else if(i->child->number != 1.0)
                            compile_instruction(OPCODE_MULI, 0, 5, functionVM->AddConstant((DBL)1.0 / (i->child->number)));
                        continue;
                    case OP_POW:
                        if(i->child->number == 0.0)
                        {
                            compile_instruction(OPCODE_LOADI, 0, 5, functionVM->AddConstant(1.0));
                            parser->Warning("Zero power optimised to constant 1.0!");
                            continue;
                        }
                        else if(i->child->number == 2.0)
                        {
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            continue;
                        }
                        else if(i->child->number == 3.0)
                        {
                            compile_instruction(OPCODE_MOVE, 5, 0, 0);
                            compile_instruction(OPCODE_MUL, 0, 5, 0);
                            compile_instruction(OPCODE_MUL, 0, 5, 0);
                            continue;
                        }
                        else if(i->child->number == 4.0)
                        {
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            continue;
                        }
                        else if(i->child->number == 5.0)
                        {
                            compile_instruction(OPCODE_MOVE, 5, 0, 0);
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            compile_instruction(OPCODE_MUL, 0, 5, 0);
                            continue;
                        }
                        else if(i->child->number == 6.0)
                        {
                            compile_instruction(OPCODE_MOVE, 5, 0, 0);
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            compile_instruction(OPCODE_MUL, 0, 0, 0);
                            compile_instruction(OPCODE_MUL, 0, 5, 0);
                            continue;
                        }
                        else if(i->child->number == 7.0)
                        {
                            compile_instruction(OPCODE_MOVE, 5, 0, 0);
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            compile_instruction(OPCODE_MUL, 0, 5, 0);
                            compile_instruction(OPCODE_MUL, 0, 0, 0);
                            compile_instruction(OPCODE_MUL, 0, 5, 0);
                            continue;
                        }
                        else if(i->child->number == 8.0)
                        {
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            compile_instruction(OPCODE_MUL, 5, 5, 0);
                            continue;
                        }
                        break;
                }
            }

            if(i->op != OP_CALL)
                compile_recursive(i->child);
        }

        switch(i->op)
        {
            case OP_CONSTANT:
                compile_instruction(OPCODE_LOADI, 0, 0, functionVM->AddConstant(expr->number));
                break;
            case OP_VARIABLE:
                compile_variable(expr->variable);
                break;
            case OP_DOT:
                // do nothing
                break;
            case OP_MEMBER:
                compile_member(expr->variable);
                break;
            case OP_CALL:
                compile_call(i->child, i->call.fn, i->call.token, i->call.name);
                break;
            case OP_CMP_EQ:
                compile_instruction(OPCODE_CMP, 0, 5, 0);
                compile_instruction(OPCODE_SEQ, 0, 5, 0);
                break;
            case OP_CMP_NE:
                compile_instruction(OPCODE_CMP, 0, 5, 0);
                compile_instruction(OPCODE_SNE, 0, 5, 0);
                break;
            case OP_CMP_LT:
                compile_instruction(OPCODE_CMP, 0, 5, 0);
                compile_instruction(OPCODE_SLT, 0, 5, 0);
                break;
            case OP_CMP_LE:
                compile_instruction(OPCODE_CMP, 0, 5, 0);
                compile_instruction(OPCODE_SLE, 0, 5, 0);
                break;
            case OP_CMP_GT:
                compile_instruction(OPCODE_CMP, 0, 5, 0);
                compile_instruction(OPCODE_SGT, 0, 5, 0);
                break;
            case OP_CMP_GE:
                compile_instruction(OPCODE_CMP, 0, 5, 0);
                compile_instruction(OPCODE_SGE, 0, 5, 0);
                break;
            case OP_ADD:
                compile_instruction(OPCODE_ADD, 0, 5, 0);
                break;
            case OP_SUB:
                compile_instruction(OPCODE_SUB, 0, 5, 0);
                break;
            case OP_OR:
                compile_instruction(OPCODE_TNE, 0, 5, 0);
                compile_instruction(OPCODE_TNE, 0, 0, 0);
                compile_instruction(OPCODE_ADD, 0, 5, 0);
                compile_instruction(OPCODE_TNE, 0, 5, 0);
                break;
            case OP_MUL:
                compile_instruction(OPCODE_MUL, 0, 5, 0);
                break;
            case OP_DIV:
                compile_instruction(OPCODE_XEQ, 0, 0, 0);
                compile_instruction(OPCODE_DIV, 0, 5, 0);
                break;
            case OP_AND:
                compile_instruction(OPCODE_TNE, 0, 5, 0);
                compile_instruction(OPCODE_TNE, 0, 0, 0);
                compile_instruction(OPCODE_MUL, 0, 5, 0);
                break;
            case OP_POW:
                compile_instruction(OPCODE_XDZ, 0, 5, 0);
                compile_instruction(OPCODE_MOVE, 0, 1, 0);
                compile_instruction(OPCODE_MOVE, 5, 0, 0);
                compile_instruction(OPCODE_SYS2, 0, 0, TRAP_SYS2_POW);
                compile_instruction(OPCODE_MOVE, 0, 5, 0);
                break;
            case OP_NEG:
                compile_instruction(OPCODE_NEG, 0, 5, 0);
                break;
            case OP_NOT:
                // compile_instruction(OPCODE_NOT, 0, 5, 0);
                break;
            case OP_LEFTMOST:
                compile_instruction(OPCODE_MOVE, 0, 5, 0);
                break;
            default:
                break;
        }
    }

    if(expr->op <= OP_LEFTMOST)
    {
        compile_instruction(OPCODE_MOVE, 5, 0, 0);
        compile_pop_result(local_k);
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_member
*
* INPUT
*
*   expr - member expression
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Compiles a member access. Works for functions only!!!
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_member(char *name)
{
    unsigned int return_parameter_sp = 0;

    // determine position of return values on stack
    // (see compile_vector_function_call for stack layout)
    return_parameter_sp = (unsigned int)(((int)stack_pointer) + max(((int)(level)) - 2, 0) + min(((int)(level)) + 1, 3));

    // compile member access
    if(name[1] == '\0')
    {
        if((name[0] == 'x') || (name[0] == 'u'))
        {
            compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp);
            return;
        }
        else if((name[0] == 'y') || (name[0] == 'v'))
        {
            compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp + 1);
            return;
        }
        else if(name[0] == 'z')
        {
            compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp + 2);
            return;
        }
        else if(name[0] == 't')
        {
            compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp + 3);
            return;
        }
    }

    if(strcmp(name, "red") == 0)
        compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp);
    else if(strcmp(name, "green") == 0)
        compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp + 1);
    else if(strcmp(name, "blue") == 0)
        compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp + 2);
    else if(strcmp(name, "filter") == 0)
        compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp + 3);
    else if(strcmp(name, "transmit") == 0)
        compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp + 4);
    else if((strcmp(name, "gray") == 0) || (strcmp(name, "grey") == 0))
    {
        compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp);
        compile_instruction(OPCODE_MULI, 0, 5, functionVM->AddConstant(kRedIntensity));
        compile_instruction(OPCODE_LOAD, 1, 0, return_parameter_sp + 1);
        compile_instruction(OPCODE_MULI, 0, 0, functionVM->AddConstant(kGreenIntensity));
        compile_instruction(OPCODE_ADD, 0, 5, 0);
        compile_instruction(OPCODE_LOAD, 1, 0, return_parameter_sp + 2);
        compile_instruction(OPCODE_MULI, 0, 0, functionVM->AddConstant(kBlueIntensity));
        compile_instruction(OPCODE_ADD, 0, 5, 0);
    }
    else if(strcmp(name, "hf") == 0)
    {
        // for MegaPOV compatibility (not supported officially)
        compile_instruction(OPCODE_LOAD, 1, 5, return_parameter_sp);
        compile_instruction(OPCODE_LOAD, 1, 0, return_parameter_sp + 1);
        compile_instruction(OPCODE_MULI, 0, 0, functionVM->AddConstant(1.0 / 255.0));
        compile_instruction(OPCODE_ADD, 0, 5, 0);
        compile_instruction(OPCODE_MULI, 0, 5, functionVM->AddConstant(0.996093));
// TODO FIXME       mExperimentalFlags.functionHf = true;
    }
    else
        parser->Error("Invalid member access: Valid member names are x, y, z, t, u, v,\nred, green, blue, grey, filter and transmit.");
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_call
*
* INPUT
*
*   expr - list of function parameters (required)
*   fn - user defined function reference (optional)
*   token - token of the function (required)
*   name - name of the function (required)
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Compiles a function call.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_call(ExprNode *expr, FUNCTION fn, int token, char *name)
{
    unsigned int domain_check = 0;
    unsigned int domain_check_2nd = 0;
    unsigned int local_k = 0;
    unsigned int k = 0;
    int op_state = 1;

    if(expr == NULL)
        parser->Error("Invalid number of parameters: At least one parameter expected!");

    switch(token)
    {
        case SIN_TOKEN:
            k = TRAP_SYS1_SIN;
            break;
        case COS_TOKEN:
            k = TRAP_SYS1_COS;
            break;
        case TAN_TOKEN:
            k = TRAP_SYS1_TAN;
            break;
        case ASIN_TOKEN:
            k = TRAP_SYS1_ASIN;
            break;
        case ACOS_TOKEN:
            k = TRAP_SYS1_ACOS;
            break;
        case ATAN_TOKEN:
            k = TRAP_SYS1_ATAN;
            break;
        case SINH_TOKEN:
            k = TRAP_SYS1_SINH;
            break;
        case COSH_TOKEN:
            k = TRAP_SYS1_COSH;
            break;
        case TANH_TOKEN:
            k = TRAP_SYS1_TANH;
            break;
        case ASINH_TOKEN:
            k = TRAP_SYS1_ASINH;
            break;
        case ACOSH_TOKEN:
            k = TRAP_SYS1_ACOSH;
            break;
        case ATANH_TOKEN:
            k = TRAP_SYS1_ATANH;
            break;
        case ABS_TOKEN:
            op_state = 7;
            break;
        case RADIANS_TOKEN:
            op_state = 3;
            break;
        case DEGREES_TOKEN:
            op_state = 4;
            break;
        case DIV_TOKEN:
            k = TRAP_SYS2_DIV;
            domain_check_2nd = OPCODE_XEQ;
            op_state = 2;
            break;
        case INT_TOKEN:
            k = TRAP_SYS1_INT;
            break;
        case FLOOR_TOKEN:
            k = TRAP_SYS1_FLOOR;
            break;
        case CEIL_TOKEN:
            k = TRAP_SYS1_CEIL;
            break;
        case SQRT_TOKEN:
            k = TRAP_SYS1_SQRT;
            break;
        case EXP_TOKEN:
            k = TRAP_SYS1_EXP;
            break;
        case LN_TOKEN:
            k = TRAP_SYS1_LN;
            domain_check = OPCODE_XLE;
            break;
        case LOG_TOKEN:
            k = TRAP_SYS1_LOG;
            domain_check = OPCODE_XLE;
            break;
        case MIN_TOKEN:
            op_state = 5;
            break;
        case MAX_TOKEN:
            op_state = 6;
            break;
        case ATAN2_TOKEN:
            k = TRAP_SYS2_ATAN2;
            op_state = 2;
            break;
        case POW_TOKEN:
            k = TRAP_SYS2_POW;
            op_state = 11;
            break;
        case MOD_TOKEN:
            k = TRAP_SYS2_MOD;
            domain_check_2nd = OPCODE_XEQ;
            op_state = 2;
            break;
        case SELECT_TOKEN:
            op_state = 8;
            break;
        case SUM_TOKEN:
            op_state = 13;
            break;
        case PROD_TOKEN:
            op_state = 14;
            break;
        case SQR_TOKEN:
            op_state = 12;
            break;
        case FUNCT_ID_TOKEN:
            op_state = 9;
            break;
        case VECTFUNCT_ID_TOKEN:
            op_state = 10;
            break;
        default:
            parser->Expectation_Error("function identifier");
            break;
    }

    switch(op_state)
    {
        case 1: // one parameter
            compile_recursive(expr->child);
            if(domain_check != 0)
                compile_instruction(domain_check, 0, 0, 0);
            compile_instruction(OPCODE_SYS1, 0, 0, k);
            if(expr->next != NULL)
                parser->Error("Invalid number of parameters for '%s': Only one parameter expected!", name);
            break;
        case 2: // two parameters
            // first evaluate right parameter
            if(expr->next == NULL)
                parser->Error("Invalid number of parameters for '%s': Two parameters expected!", name);
            compile_recursive(expr->next->child);
            if(domain_check_2nd != 0)
                compile_instruction(domain_check_2nd, 0, 0, 0);
            // temporary storage of right parameter in r5
            local_k = compile_push_result();
            compile_instruction(OPCODE_MOVE, 0, 5, 0);
            // evaluate left parameter
            compile_recursive(expr->child);
            if(domain_check != 0)
                compile_instruction(domain_check, 0, 0, 0);
            // move right parameter from r5 to r1
            compile_instruction(OPCODE_MOVE, 5, 1, 0);
            compile_pop_result(local_k);
            // make sure there are not more than 2 parameters
            if(expr->next->next != NULL)
                parser->Error("Invalid number of parameters for '%s': Only two parameters expected!", name);
            compile_instruction(OPCODE_SYS2, 0, 0, k);
            break;
        case 3: // radians
            compile_recursive(expr->child);
            compile_instruction(OPCODE_MULI, 0, 0, functionVM->AddConstant(M_PI / 180.0));
            if(expr->next != NULL)
                parser->Error("Invalid number of parameters for '%s': Only one parameter expected!", name);
            break;
        case 4: // degrees
            compile_recursive(expr->child);
            compile_instruction(OPCODE_MULI, 0, 0, functionVM->AddConstant(180.0 / M_PI));
            if(expr->next != NULL)
                parser->Error("Invalid number of parameters for '%s': Only one parameter expected!", name);
            break;
        case 5: // min
            compile_recursive(expr->child);
            if(expr->next == NULL)
                parser->Error("Invalid number of parameters for '%s': At least two parameters expected!", name);
            // compare all parameters, searching for minimum
            for(expr = expr->next; expr != NULL; expr = expr->next)
            {
                // temporary storage of last minimum in r5
                local_k = compile_push_result();
                compile_instruction(OPCODE_MOVE, 0, 5, 0);
                compile_recursive(expr->child);
                // move last minimum from r5 to r1
                compile_instruction(OPCODE_MOVE, 5, 1, 0);
                compile_pop_result(local_k);
                // compare last minimum and current parameter
                compile_instruction(OPCODE_CMP, 0, 1, 0);
                compile_instruction(OPCODE_BGT, 0, 0, function->program_size + 2);
                compile_instruction(OPCODE_MOVE, 1, 0, 0);
            }
            break;
        case 6: // max
            compile_recursive(expr->child);
            if(expr->next == NULL)
                parser->Error("Invalid number of parameters for '%s': At least two parameters expected!", name);
            // compare all parameters, searching for maximum
            for(expr = expr->next; expr != NULL; expr = expr->next)
            {
                // temporary storage of last maximum in r5
                local_k = compile_push_result();
                compile_instruction(OPCODE_MOVE, 0, 5, 0);
                compile_recursive(expr->child);
                // move last maximum from r5 to r1
                compile_instruction(OPCODE_MOVE, 5, 1, 0);
                compile_pop_result(local_k);
                // compare last maximum and current parameter
                compile_instruction(OPCODE_CMP, 0, 1, 0);
                compile_instruction(OPCODE_BLT, 0, 0, function->program_size + 2);
                compile_instruction(OPCODE_MOVE, 1, 0, 0);
            }
            break;
        case 7: // abs
            compile_recursive(expr->child);
            compile_instruction(OPCODE_ABS, 0, 0, 0);
            if(expr->next != NULL)
                parser->Error("Invalid number of parameters for '%s': Only one parameter expected!", name);
            break;
        case 8: // select
            compile_select(expr);
            break;
        case 9: // user defined floating-point function
            compile_float_function_call(expr, fn, name);
            break;
        case 10: // user defined vector function
            compile_vector_function_call(expr, fn, name);
            break;
        case 11: // pow
            // first evaluate right parameter
            if(expr->next == NULL)
                parser->Error("Invalid number of parameters for '%s': Two parameters expected!", name);
            compile_recursive(expr->next->child);
            // temporary storage of right parameter in r5
            local_k = compile_push_result();
            compile_instruction(OPCODE_MOVE, 0, 5, 0);
            // evaluate left parameter
            compile_recursive(expr->child);
            // move right parameter from r5 to r1
            compile_instruction(OPCODE_MOVE, 5, 1, 0);
            compile_pop_result(local_k);
            // check domain error (if r0 and r1 are zero)
            compile_instruction(OPCODE_XDZ, 0, 1, 0);
            // make sure there are not more than 2 parameters
            if(expr->next->next != NULL)
                parser->Error("Invalid number of parameters for '%s': Only two parameters expected!", name);
            compile_instruction(OPCODE_SYS2, 0, 0, k);
            break;
        case 12: // sqr
            compile_recursive(expr->child);
            compile_instruction(OPCODE_MUL, 0, 0, 0);
            if(expr->next != NULL)
                parser->Error("Invalid number of parameters for '%s': Only one parameter expected!", name);
            break;
        case 13: // sum
            compile_seq_op(expr, OPCODE_ADD, 0.0);
            break;
        case 14: // prod
            compile_seq_op(expr, OPCODE_MUL, 1.0);
            break;
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_select
*
* INPUT
*
*   expr - list of function parameters
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Compiles a select function call.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_select(ExprNode *expr)
{
    unsigned int greater_pos;
    unsigned int less_pos;
    unsigned int equal_end;
    unsigned int greater_end;
    unsigned int all_end;
    bool have_fourth = false;

    if(expr->next == NULL) // second
        parser->Error("Invalid number of parameters: Three or four parameters expected!");
    if(expr->next->next == NULL) // third
        parser->Error("Invalid number of parameters: Three or four parameters expected!");
    if(expr->next->next->next != NULL) // fourth
    {
        if(expr->next->next->next->next != NULL) // fifth
            parser->Error("Invalid number of parameters: Only three or four parameters expected!");
        have_fourth = true;
    }

    // compile condition
    compile_recursive(expr->child);
    compile_instruction(OPCODE_CMPI, 0, 0, functionVM->AddConstant(0.0));
    greater_pos = compile_instruction(OPCODE_NOP, 0, 0, 0); // bgt
    if(have_fourth == true)
        less_pos = compile_instruction(OPCODE_NOP, 0, 0, 0); // blt
    // compile equal (four parameters) or equal or less than (three parameters) part
    compile_recursive(expr->next->next->child);
    equal_end = compile_instruction(OPCODE_NOP, 0, 0, 0); // jmp
    // compile greater than part
    compile_recursive(expr->next->child);
    // if there is a fourth argument, compile less than part
    if(have_fourth == true)
    {
        greater_end = compile_instruction(OPCODE_NOP, 0, 0, 0); // jmp
        compile_recursive(expr->next->next->next->child);
    }
    // get the position after the last compiled instruction
    all_end = function->program_size;

    // fix the branches
    compile_instruction(greater_pos, OPCODE_BLT, 0, 0, equal_end + 1);
    compile_instruction(equal_end, OPCODE_JMP, 0, 0, all_end);
    if(have_fourth == true)
    {
        compile_instruction(less_pos, OPCODE_BGT, 0, 0, greater_end + 1);
        compile_instruction(greater_end, OPCODE_JMP, 0, 0, all_end);
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_seq_op
*
* INPUT
*
*   expr - list of function parameters
*   op - operation to perfrom on value and result for each step
*   neutral - neutral element of the operation to apply
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Compiles a sum function call.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_seq_op(ExprNode *expr, unsigned int op, DBL neutral)
{
    unsigned int sum_k = 0;
    unsigned int local_k = 0;
    unsigned int max_k = 0;
    unsigned int begin_loop;
    unsigned int end_loop;
    unsigned int loop_condition;
    unsigned int old_level;
    unsigned int var_sp;
    unsigned int r5_content;

    if(expr->next == NULL) // second
        parser->Error("Invalid number of parameters: Four parameters expected!");
    if(expr->next->next == NULL) // third
        parser->Error("Invalid number of parameters: Four parameters expected!");
    if(expr->next->next->next == NULL) // fourth
        parser->Error("Invalid number of parameters: Four parameters expected!");
    if(expr->next->next->next->next != NULL) // fifth
        parser->Error("Invalid number of parameters: Only four parameters expected!");

    // create a local variable, the sum and its limit on the stack
    if(function->localvar_cnt >= MAX_FUNCTION_PARAMETER_LIST)
        parser->Error("Too many local variables!");

    // save r5 content
    r5_content = compile_push_result();

    // calculate variablestack pointer
    if (level >= 3)
        var_sp = stack_pointer + level - 3;
    else
        var_sp = stack_pointer;

    max_stack_size = (unsigned int)max((int)var_sp + 3, (int)max_stack_size);

    old_level = level;

    sum_k = var_sp;
    local_k = var_sp + 1;
    max_k = var_sp + 2;

    // compile_push_result (called by compile_recursive below)
    // should not overwrite the variables on the stack, hence:
    //
    // stack_pointer + level - 3 >= var_sp + 3

    level = var_sp + 3 - stack_pointer + 3;

    function->localvar_pos[function->localvar_cnt] = local_k;
    function->localvar[function->localvar_cnt] = expr->child->variable;
    if(expr->child->op != OP_VARIABLE)
        parser->Error("Local variable name expected!");
    function->localvar_cnt++;

    // clear result variable
    compile_instruction(OPCODE_LOADI, 1, 5, functionVM->AddConstant(neutral));
    compile_instruction(OPCODE_STORE, 1, 5, sum_k);
    // compile initial value expression and put it into the local variable
    compile_recursive(expr->next->child);
    compile_instruction(OPCODE_STORE, 1, 0, local_k);
    // compile final value expression and put it on the stack
    compile_recursive(expr->next->next->child);
    compile_instruction(OPCODE_STORE, 1, 0, max_k);

    // begin loop
    begin_loop = function->program_size;

        // load current value into r5
        compile_instruction(OPCODE_LOAD, 1, 5, local_k);
        // load final value into r0
        compile_instruction(OPCODE_LOAD, 1, 0, max_k);
        // check if the current value is less or equal to the final value
        compile_instruction(OPCODE_CMP, 0, 5, 0);
        loop_condition = compile_instruction(OPCODE_NOP, 0, 0, 0); // bgt

        // compile function to sum up
        compile_recursive(expr->next->next->next->child);
        // load result into r5
        compile_instruction(OPCODE_LOAD, 1, 5, sum_k);
        // add function result to sum
        compile_instruction(op, 0, 5, sum_k);
        // store sum
        compile_instruction(OPCODE_STORE, 1, 5, sum_k);

        // load current value into r5
        compile_instruction(OPCODE_LOAD, 1, 5, local_k);
        // increment current value
        compile_instruction(OPCODE_ADDI, 0, 5, functionVM->AddConstant(1.0));
        // store current value
        compile_instruction(OPCODE_STORE, 1, 5, local_k);

    // end loop
    compile_instruction(OPCODE_JMP, 0, 0, begin_loop);
    end_loop = function->program_size;

    // fix the branches
    compile_instruction(loop_condition, OPCODE_BGT, 0, 0, end_loop);

    // move sum into return register
    compile_instruction(OPCODE_LOAD, 1, 0, sum_k);

    // remove the local variable, the sum and its limit from the stack
    function->localvar_cnt--;
    function->localvar_pos[function->localvar_cnt] = 0;
    function->localvar[function->localvar_cnt] = NULL;

    level = old_level;
    // restore r5 content
    compile_pop_result(r5_content);
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_float_function_call
*
* INPUT
*
*   expr - list of function parameters
*   fn - user defined function reference
*   name - name of the function (required)
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Compiles a user-defined floating-point function call.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_float_function_call(ExprNode *expr, FUNCTION fn, char *name)
{
    FunctionCode *f = NULL;
    ExprNode *i = NULL;
    unsigned int cur_p = 0;
    unsigned int local_k = 0;
    unsigned int old_sp = 0;
    unsigned int old_parameter_sp = 0;
    unsigned int old_level = 0;
    unsigned int r567_sp = 0;
    unsigned int call_sp = 0;
    unsigned int call_parameter_sp = 0;

    // The VM stack format looks like this:
    //
    //           *
    //          ***
    //         *****
    //        *******
    //      Stack grows
    //     from lower to
    //    higher addresses
    //
    // +--------------------+
    // |      Temporary     |
    // |      Variables     |
    // +--------------------+ <= Temporary Variables Stack Pointer (call_sp)
    // |    Function Call   |
    // |     Parameters     |
    // +--------------------+ <= Call Stack Pointer (call_parameter_sp)
    // |      Registers     |    (will be new stack pointer for call)
    // |    r5, r6 and r7   |
    // +--------------------+ <= Register Buffer Stack Pointer (r567_sp)
    // |      Temporary     |
    // |      Variables     |
    // +--------------------+ <= Stack Pointer (stack_pointer)
    // |      Function      |
    // |   Local Variables  |
    // +--------------------+ <= (Currently there are no local variables!)
    // |      Function      |
    // |     Parameters     |
    // +--------------------+ <= Parameter Stack Pointer (parameter_stack_pointer)

    if(strcmp(name, function->sourceInfo.name) == 0)
    {
//      Warning("Recursive function call may have unexpected side effects or not\n"
//              "work as expected! Make sure the recursion is not infinite!!!");
        parser->PossibleError("Recursive function calls are not allowed!");
        // recursive calls may not increase the reference count [trf]
        f = function;
    }
    else
        f = functionVM->GetFunctionAndReference(fn);

    // calculate stack pointer to hold the registers
    r567_sp = (unsigned int)(((int)stack_pointer) + max(((int)(level)) - 2, 0));
    // calculate call parameter stack pointer
    call_parameter_sp = r567_sp + min(level + 1, 3);
    // copy result register r5 to the stack
    compile_instruction(OPCODE_STORE, 1, 5, r567_sp);
    // copy result register r6 to the stack
    if(level >= 1)
        compile_instruction(OPCODE_STORE, 1, 6, r567_sp + 1);
    // copy result register r7 to the stack
    if(level >= 2)
        compile_instruction(OPCODE_STORE, 1, 7, r567_sp + 2);

    // calculate call stack pointer
    call_sp = call_parameter_sp + f->parameter_cnt;
    // keep current level, stack pointer and function
    old_level = level;
    old_sp = stack_pointer;
    old_parameter_sp = parameter_stack_pointer;
    // set new stack pointer, reset level and change function
    level = 0;
    stack_pointer = call_sp;
    parameter_stack_pointer = call_parameter_sp;
    // make sure the max_stack_size is at least the current size
    max_stack_size = (unsigned int)max((int)stack_pointer, (int)max_stack_size);

    // determine all the parameters
    for(i = expr, cur_p = 0; i != NULL; i = i->next, cur_p++)
    {
        // compile the parameter expression
        compile_recursive(i->child);
        // copy the result as parameter to the stack
        compile_instruction(OPCODE_STORE, 1, 0, call_parameter_sp + cur_p);
    }
    // make sure the supplied number of parameters matches the required number
    if(cur_p != f->parameter_cnt)
        parser->Error("Invalid number of parameters: %d supplied, %d required!",
                      (int)(cur_p), (int)(f->parameter_cnt));

    // restore the old level, stack pointer and function
    level = old_level;
    stack_pointer = old_sp;
    parameter_stack_pointer = old_parameter_sp;

    // move the stack pointer so the parameters are at the stack base
    compile_instruction(OPCODE_PUSH, 0, 0, call_parameter_sp); // call_parameter_sp - parameter_stack_pointer
    // call the function and inline non-recursive calls to functions which are less than 16 instructions size
/*  if((f->program_size < 16) && (f != function))
    {
        POVFPU_RemoveFunction(fn);
        compile_inline(f);
    }
    else
*/      compile_instruction(OPCODE_CALL, 0, 0, fn);
    // move the stack pointer back
    compile_instruction(OPCODE_POP, 0, 0, call_parameter_sp); // call_parameter_sp - parameter_stack_pointer

    // copy result register r5 back from the stack
    compile_instruction(OPCODE_LOAD, 1, 5, r567_sp);
    // copy result register r6 back from the stack
    if(level >= 1)
        compile_instruction(OPCODE_LOAD, 1, 6, r567_sp + 1);
    // copy result register r7 back from the stack
    if(level >= 2)
        compile_instruction(OPCODE_LOAD, 1, 7, r567_sp + 2);

    // restore parameters x, y and z
    compile_parameters();
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_vector_function_call
*
* INPUT
*
*   expr - list of function parameters
*   fn - user defined function reference
*   name - name of the function (required)
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Compiles a user-defined vector function call.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_vector_function_call(ExprNode *expr, FUNCTION fn, char *name)
{
    FunctionCode *f = NULL;
    ExprNode *i = NULL;
    unsigned int cur_p = 0;
    unsigned int local_k = 0;
    unsigned int old_sp = 0;
    unsigned int old_parameter_sp = 0;
    unsigned int old_level = 0;
    unsigned int r567_sp = 0;
    unsigned int call_sp = 0;
    unsigned int call_parameter_sp = 0;

    // The VM stack format looks like this:
    //
    //           *
    //          ***
    //         *****
    //        *******
    //      Stack grows
    //     from lower to
    //    higher addresses
    //
    // +--------------------+
    // |      Temporary     |
    // |      Variables     |
    // +--------------------+ <= Temporary Variables Stack Pointer (call_sp)
    // |    Function Call   |
    // |     Parameters     |
    // +--------------------+ <= Call Stack Pointer (call_parameter_sp)
    // |      Function      |
    // |    Return Values   |
    // +--------------------+ <= Return Stack Pointer
    // |      Registers     |    (will be new stack pointer for call)
    // |    r5, r6 and r7   |
    // +--------------------+ <= Register Buffer Stack Pointer (r567_sp)
    // |      Temporary     |
    // |      Variables     |
    // +--------------------+ <= Stack Pointer (stack_pointer)
    // |      Function      |
    // |   Local Variables  |
    // +--------------------+ <= (Currently there are no local variables!)
    // |      Function      |
    // |     Parameters     |
    // +--------------------+ <= Parameter Stack Pointer (parameter_stack_pointer)

    if(strcmp(name, function->sourceInfo.name) == 0)
    {
//      Warning("Recursive function call may have unexpected side effects or not\n"
//              "work as expected! Make sure the recursion is not infinite!!!");
        parser->PossibleError("Recursive function calls are not allowed!");
        // recursive calls may not increase the reference count [trf]
        f = function;
    }
    else
        f = functionVM->GetFunctionAndReference(fn);

    // calculate stack pointer to hold the registers
    r567_sp = (unsigned int)(((int)stack_pointer) + max(((int)(level)) - 2, 0));
    // calculate call parameter stack pointer
    call_parameter_sp = r567_sp + f->return_size + min(level + 1, 3);
    // copy result register r5 to the stack
    compile_instruction(OPCODE_STORE, 1, 5, r567_sp);
    // copy result register r6 to the stack
    if(level >= 1)
        compile_instruction(OPCODE_STORE, 1, 6, r567_sp + 1);
    // copy result register r7 to the stack
    if(level >= 2)
        compile_instruction(OPCODE_STORE, 1, 7, r567_sp + 2);

    // calculate call stack pointer
    call_sp = call_parameter_sp + f->parameter_cnt;
    // keep current level, stack pointer and function
    old_level = level;
    old_sp = stack_pointer;
    old_parameter_sp = parameter_stack_pointer;
    // set new stack pointer, reset level and change function
    level = 0;
    stack_pointer = call_sp;
    parameter_stack_pointer = call_parameter_sp;
    // make sure the max_stack_size is at least the current size
    max_stack_size = (unsigned int)max((int)stack_pointer, (int)max_stack_size);

    // determine all the parameters
    for(i = expr, cur_p = 0; i != NULL; i = i->next, cur_p++)
    {
        // compile the parameter expression
        compile_recursive(i->child);
        // copy the result as parameter to the stack
        compile_instruction(OPCODE_STORE, 1, 0, call_parameter_sp + cur_p);
    }
    // make sure the supplied number of parameters matches the required number
    if(cur_p != f->parameter_cnt)
        parser->Error("Invalid number of parameters: %d supplied, %d required!",
                      (int)(cur_p), (int)(f->parameter_cnt));

    // restore the old level, stack pointer and function
    level = old_level;
    stack_pointer = old_sp;
    parameter_stack_pointer = old_parameter_sp;

    // move the stack pointer so the parameters are at the stack base
    compile_instruction(OPCODE_PUSH, 0, 0, call_parameter_sp - f->return_size); //  - parameter_stack_pointer
    // call the function
    // call the function and inline non-recursive calls to functions which are less than 16 instructions size
/*  if((f->program_size < 16) && (f != function))
    {
        POVFPU_RemoveFunction(fn);
        compile_inline(f);
    }
    else
*/      compile_instruction(OPCODE_CALL, 0, 0, fn);
    // move the stack pointer back
    compile_instruction(OPCODE_POP, 0, 0, call_parameter_sp - f->return_size); //  - parameter_stack_pointer

    // copy result register r5 back from the stack
    compile_instruction(OPCODE_LOAD, 1, 5, r567_sp);
    // copy result register r6 back from the stack
    if(level >= 1)
        compile_instruction(OPCODE_LOAD, 1, 6, r567_sp + 1);
    // copy result register r7 back from the stack
    if(level >= 2)
        compile_instruction(OPCODE_LOAD, 1, 7, r567_sp + 2);

    // restore parameters x, y and z
    compile_parameters();
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_inline
*
* INPUT
*
*   fn - user defined function reference
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Compiles a user-defined function as inline function.
*   Does not work yet! [trf]
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_inline(FunctionCode *f)
{
    unsigned int offset = function->program_size - 1;
    unsigned int op, k;
    unsigned int i = 0;

    for(i = 0; i < f->program_size - 1; i++)
    {
        op = GET_OP(f->program[i]);
        k = GET_K(f->program[i]);
        if(((op >= OPCODE_BEQ) && (op <= OPCODE_BGE)) || (op == OPCODE_JMP))
            compile_instruction(GET_OP(op), 0, 0, GET_K(k + offset));
        else
            compile_instruction(GET_OP(op), 0, 0, GET_K(k));
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_variable
*
* INPUT
*
*   name - variable name (required)
*   number - pointer to declared constant variable value (optional)
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Compiles a variable access.  Note that parameters will hide declared
*   constants.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_variable(char *name)
{
    unsigned int i = 0, found = MAX_K;

    // first, handle register parameters x,y,z,u and v
    if(name[1] == 0)
    {
        if((name[0] == 'x') || (name[0] == 'u'))
        {
            compile_instruction(OPCODE_MOVE, 2, 0, 0);
            return;
        }
        else if((name[0] == 'y') || (name[0] == 'v'))
        {
            compile_instruction(OPCODE_MOVE, 3, 0, 0);
            return;
        }
        else if(name[0] == 'z')
        {
            compile_instruction(OPCODE_MOVE, 4, 0, 0);
            return;
        }
    }

    // second, handle stack local variables
    for(i = 0; i < function->localvar_cnt; i++)
    {
        if(strcmp(name, function->localvar[i]) == 0)
            found = i;
    }
    if(found < function->localvar_cnt)
    {
        compile_instruction(OPCODE_LOAD, 1, 0, function->localvar_pos[found]);
        return;
    }

    // third, handle stack parameters
    for(i = 0; i < function->parameter_cnt; i++)
    {
        if(strcmp(name, function->parameter[i]) == 0)
        {
            compile_instruction(OPCODE_LOAD, 1, 0, i);
            return;
        }
    }

    parser->Expectation_Error("parameter identifier or floating-point constant identifier");
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_parameters
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Compiles the function call parameters. Load the parameters x,y,z into
*   registers and assign stack locations to all other parameters.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_parameters()
{
    unsigned int i = 0;
    bool had_x = false;
    bool had_y = false;
    bool had_z = false;

    // if it is a function with default parameters, add them
    if(function->parameter_cnt == 0)
    {
        function->parameter_cnt = 3;
        function->parameter[0] = POV_STRDUP("x");
        function->parameter[1] = POV_STRDUP("y");
        function->parameter[2] = POV_STRDUP("z");
    }

    for(i = 0; i < function->parameter_cnt; i++)
    {
        if((strcmp(function->parameter[i], "x") == 0) ||
           (strcmp(function->parameter[i], "u") == 0))
        {
            compile_instruction(OPCODE_LOAD, 1, 2, i);
            had_x = true;
        }
        else if((strcmp(function->parameter[i], "y") == 0) ||
                (strcmp(function->parameter[i], "v") == 0))
        {
            compile_instruction(OPCODE_LOAD, 1, 3, i);
            had_y = true;
        }
        else if(strcmp(function->parameter[i], "z") == 0)
        {
            compile_instruction(OPCODE_LOAD, 1, 4, i);
            had_z = true;
        }
    }

    if(had_x == false)
        compile_instruction(OPCODE_LOADI, 0, 2, functionVM->AddConstant(0.0));
    if(had_y == false)
        compile_instruction(OPCODE_LOADI, 0, 3, functionVM->AddConstant(0.0));
    if(had_z == false)
        compile_instruction(OPCODE_LOADI, 0, 4, functionVM->AddConstant(0.0));
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_push_result
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   unsigned int - local variable reference number (if any)
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Push the result register r5 on a stack or unused register.  This is
*   required to handle different operator precedences correctly.  The
*   function uses the global variable "level" to find the right storage
*   location. Also see description of partner function compile_pop_result.
*
* CHANGES
*
*   -
*
******************************************************************************/

unsigned int FNCode::compile_push_result()
{
    max_stack_size = (unsigned int)max(((int)(stack_pointer)) + ((int)(level)) - 2, ((int)(max_stack_size)));

    if(level == 1)
        compile_instruction(OPCODE_MOVE, 5, 6, 0);
    else if(level == 2)
        compile_instruction(OPCODE_MOVE, 5, 7, 0);
    else if(level >= 3)
        compile_instruction(OPCODE_STORE, 1, 5, stack_pointer + level - 3);

    level++;

    // NOTE: Always returns a valid result also there is not always one! [trf]
    return max(((int)(stack_pointer)) + ((int)(level)) - 4, 0);
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_pop_result
*
* INPUT
*
*   local_k - local variable reference number (if any)
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Pop the result register r5 from a stack or unused register.  This is
*   required to handle different operator precedences correctly.  The
*   function uses the global variable "level" to find the right storage
*   location. Also see description of partner function compile_push_result.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::compile_pop_result(unsigned int local_k)
{
    level--;

    if(level == 1)
        compile_instruction(OPCODE_MOVE, 6, 5, 0);
    else if(level == 2)
        compile_instruction(OPCODE_MOVE, 7, 5, 0);
    else if(level >= 3)
        compile_instruction(OPCODE_LOAD, 1, 5, local_k);
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_instruction
*
* INPUT
*
*   op - instruction opcode (required)
*   rs - source register (if any), set to 0 if not used
*   rd - destination register (if any), set to 0 if not used
*   k - optional parameter (if any), set to 0 if not used
*
* OUTPUT
*
* RETURNS
*
*   unsigned int - location of the instruction in the program memory
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Assemble an individual instruction and store it in the program memory.
*
* CHANGES
*
*   -
*
******************************************************************************/

unsigned int FNCode::compile_instruction(unsigned int op, unsigned int rs, unsigned int rd, unsigned int k)
{
    if(function->program_size >= max_program_size)
    {
        max_program_size = min(MAX_K, ((unsigned int)max_program_size) + 256);
        function->program = reinterpret_cast<Instruction *>(POV_REALLOC(function->program, sizeof(Instruction) * max_program_size, "fn: program"));
    }
/*  if(function->program_size > 0)
    {
        unsigned int i;

        // eliminate unnecessary move instruction
        //
        // ************************************************************
        //
        // IMPORTANT NOTE:
        // These optimisations do not take conditional branch
        // instructions in account!  This would have dangerous
        // side effects, but the current compiler does not
        // generate code for which these optimisations will
        // fail.  In the future the compiler will be extended
        // to use an intermediate code representation and more
        // advanced optimisation techniques, but this is a
        // project of its own and it would have made a release
        // of POV-Ray 3.5 next to impossible for at least
        // another year... [trf]
        //
        // ************************************************************
        //
        if(op == OPCODE_MOVE)
        {
            for(i = 0; i < function->program_size; i++)
            {
                // If there is a conditional branch instruction make sure it does not
                // cover the part that will be optimised away! [trf]
                if((function->program[i].op & OPCODE(13, 0, 0)) & 0x03C0 == OPCODE(13, 0, 0))
                {
                    if(function->program[i].k >= function->program_size)
                        break;
                }
            }

            // optimise only if the loop found no conditional branch
            if(i < function->program_size)
            {
                // case
                //   move rx, ry
                //   move ry, rx
                //  to
                //   move rx, ry
                // case
                //   move rx, ry
                //   move rx, ry
                //  to
                //   move rx, ry
                if((function->program[function->program_size - 1].op == (OPCODE_MOVE | (rd << 3) | rs)) ||
                   (function->program[function->program_size - 1].op == (OPCODE_MOVE | (rs << 3) | rd)))
                {
                    return function->program_size - 1;
                }
                // case
                //   move rx, ry
                //   move rz, ry
                //  to
                //   move rz, ry
                else if((function->program[function->program_size - 1].op & 0x03C7) == (OPCODE_MOVE | rd ))
                {
                    function->program[function->program_size - 1].op = OPCODE_MOVE | (rs << 3) | rd;
                    return function->program_size - 1;
                }
            }
        }
    }*/
    function->program[function->program_size] = MAKE_INSTRUCTION(op | (rs << 3) | rd, k);
    function->program_size++;
    if(function->program_size >= MAX_K)
        parser->Error("Function too complex!");

    return function->program_size - 1;
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::compile_instruction
*
* INPUT
*
*   pos - insert location of the instruction (required)
*   op - instruction opcode (required)
*   rs - source register (if any), set to 0 if not used
*   rd - destination register (if any), set to 0 if not used
*   k - optional parameter (if any), set to 0 if not used
*
* OUTPUT
*
* RETURNS
*
*   unsigned int - location of the instruction in the program memory
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Assemble an individual instruction and store it in the program memory at
*   the specified position.
*
* CHANGES
*
*   -
*
******************************************************************************/

unsigned int FNCode::compile_instruction(unsigned int pos, unsigned int op, unsigned int rs, unsigned int rd, unsigned int k)
{
    if(pos >= function->program_size)
        parser->Error("Internal function compiler failed in 'compile_instruction'.");
    function->program[pos] = MAKE_INSTRUCTION(op | (rs << 3) | rd, k);

    return pos;
}


#if (DEBUG_FLOATFUNCTION == 1)

/*****************************************************************************
*
* FUNCTION
*
*   FNCode::disassemble
*
* INPUT
*
*   filename - output file
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Disassembler for debugging of the virtual machine and the compiler.
*
* CHANGES
*
*   -
*
******************************************************************************/

int FNCode::disassemble(char *filename)
{
    FILE *f;
    int i;

    if(strcmp(filename, "stdout") == 0)
        f = stdout;
    else
    {
        f = fopen(filename, "w");
        if(f == NULL)
        {
            asm_error = "Cannot open disassembler output file.";
            return 1;
        }
    }

    for(i = 0; i < function->program_size; i++)
        disassemble_instruction(f, function->program[i]);

    if(f != stdout)
        fclose(f);

    return 0;
}


/*****************************************************************************
*
* FUNCTION
*
*   FNCode::disassemble_instruction
*
* INPUT
*
*   f - output file
*   i - instruction to disassemble
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Disassemble a single instruction.
*
* CHANGES
*
*   -
*
******************************************************************************/

void FNCode::disassemble_instruction(FILE *f, Instruction& i)
{
    int ii;
    unsigned int op;
    unsigned int program_op;
    unsigned int program_k;

    program_op = GET_OP(i);
    program_k = GET_K(i);

    for(ii = 0; POVFPU_Opcodes[ii].name != NULL; ii++)
    {
        op = POVFPU_Opcodes[ii].code;
        switch(POVFPU_Opcodes[ii].type)
        {
            case ITYPE_R:
                if(op == (program_op & 0x03C0))
                {
                    fprintf(f, "%-8s", POVFPU_Opcodes[ii].name);
                    fprintf(f, "R%d,R%d\n", (int)((program_op >> 3) & 7), (int)(program_op & 7));
                }
                break;
            case ITYPE_I:
                if(op == (program_op & 0x03F8))
                {
                    fprintf(f, "%-8s", POVFPU_Opcodes[ii].name);
                    fprintf(f, "%f,R%d   # const(%d)\n", (float)(POVFPU_Consts[program_k]),
                            (int)(program_op & 7), (int)program_k);
                }
                break;
            case ITYPE_S:
                if(op == (program_op & 0x03F8))
                {
                    fprintf(f, "%-8s", POVFPU_Opcodes[ii].name);
                    if((program_op & 0x03F8) == OPCODE_XDZ)
                        fprintf(f, "R0,");
                    fprintf(f, "R%d\n", (int)(program_op & 7));
                }
                break;
            case ITYPE_J:
                if(op == (program_op & 0x03FF))
                {
                    fprintf(f, "%-8s", POVFPU_Opcodes[ii].name);
                    fprintf(f, "%d\n", (int)program_k);
                }
                break;
            case ITYPE_X:
                if(op == (program_op & 0x03FF))
                {
                    fprintf(f, "%-8s", POVFPU_Opcodes[ii].name);
                    fprintf(f, "\n");
                }
                break;
            case ITYPE_M:
                if(op == (program_op & 0x03C0))
                {
                    fprintf(f, "%-8s", POVFPU_Opcodes[ii].name);
                    if((program_op & 0x03C0) == OPCODE_LOAD)
                    {
                        if((program_op & 8) == 0)
                            fprintf(f, "0(%d),R%d\n", (int)program_k, (int)(program_op & 7));
                        else
                            fprintf(f, "SP(%d),R%d\n", (int)program_k, (int)(program_op & 7));
                    }
                    else if((program_op & 0x03C0) == OPCODE_STORE)
                    {
                        if((program_op & 8) == 0)
                            fprintf(f, "R%d,0(%d)\n", (int)(program_op & 7), (int)program_k);
                        else
                            fprintf(f, "R%d,SP(%d)\n", (int)(program_op & 7), (int)program_k);
                    }
                }
                break;
        }
    }
}

#include "fnasm.cpp"

#endif

}
