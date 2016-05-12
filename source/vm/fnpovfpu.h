//******************************************************************************
///
/// @file vm/fnpovfpu.h
///
/// This module contains declarations for the virtual machine executing
/// render-time functions.
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

#ifndef POVRAY_VM_FNPOVFPU_H
#define POVRAY_VM_FNPOVFPU_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "vm/configvm.h"

#include <set>
#include <vector>

#include "base/textstream.h"

#include "core/coretypes.h"

namespace pov
{

class FunctionVM;

#define MAX_CALL_STACK_SIZE 1024
#define INITIAL_DBL_STACK_SIZE 256

#define MAX_K ((unsigned int)0x000fffff)

enum
{
    ITYPE_R = 0,
    ITYPE_I,
    ITYPE_S,
    ITYPE_J,
    ITYPE_X,
    ITYPE_M
};

struct Opcode
{
    const char *name;
    unsigned int code;
    int type;
};

enum
{
    TRAP_SYS1_SIN = 0,
    TRAP_SYS1_COS,
    TRAP_SYS1_TAN,
    TRAP_SYS1_ASIN,
    TRAP_SYS1_ACOS,
    TRAP_SYS1_ATAN,
    TRAP_SYS1_SINH,
    TRAP_SYS1_COSH,
    TRAP_SYS1_TANH,
    TRAP_SYS1_ASINH,
    TRAP_SYS1_ACOSH,
    TRAP_SYS1_ATANH,
    TRAP_SYS1_FLOOR,
    TRAP_SYS1_CEIL,
    TRAP_SYS1_SQRT,
    TRAP_SYS1_EXP,
    TRAP_SYS1_LN,
    TRAP_SYS1_LOG,
    TRAP_SYS1_INT
};

enum
{
    TRAP_SYS2_POW = 0,
    TRAP_SYS2_ATAN2,
    TRAP_SYS2_MOD,
    TRAP_SYS2_DIV
};

typedef SYS_MATH_RETURN (*Sys1)(SYS_MATH_PARAM r0);
typedef SYS_MATH_RETURN (*Sys2)(SYS_MATH_PARAM r0,SYS_MATH_PARAM r1);

typedef unsigned int Instruction;

const int MAX_FUNCTION_PARAMETER_LIST = 56;

typedef void *(*FNCODE_PRIVATE_COPY_METHOD)(void *);
typedef void (*FNCODE_PRIVATE_DESTROY_METHOD)(void *);

struct FunctionCode
{
    Instruction *program;
    unsigned int program_size;
    unsigned char return_size;
    unsigned char parameter_cnt;
    unsigned char localvar_cnt;
    unsigned int localvar_pos[MAX_FUNCTION_PARAMETER_LIST];
    char *localvar[MAX_FUNCTION_PARAMETER_LIST];
    char *parameter[MAX_FUNCTION_PARAMETER_LIST];
    FunctionSourceInfo sourceInfo;
    unsigned int flags;
    FNCODE_PRIVATE_COPY_METHOD private_copy_method;
    FNCODE_PRIVATE_DESTROY_METHOD private_destroy_method;
    void *private_data;
};

typedef unsigned int FUNCTION;
typedef FUNCTION * FUNCTION_PTR;

// WARNING: Do not change this structure without notice!!!
// Platform specific code may depend on the exact layout and size! [trf]
struct FunctionEntry
{
    union {
        FunctionCode fn;            // valid if reference_count != 0
        FUNCTION next_unreferenced; // valid if reference_count == 0
    };
    unsigned int reference_count;
    SYS_FUNCTION_ENTRY
};

// WARNING: Do not change this structure without notice!!!
// Platform specific code may depend on the exact layout and size! [trf]
struct StackFrame
{
    unsigned int pc;
    FUNCTION fn;
};

// WARNING: Do not change this structure without notice!!!
// Platform specific code may depend on the exact layout and size! [trf]
class FPUContext : public GenericFunctionContext
{
    public:
        FPUContext(FunctionVM* pVm, TraceThreadData* pThreadData);
        virtual ~FPUContext();

        StackFrame *pstackbase;
        DBL *dblstackbase;
        unsigned int maxdblstacksize;
        FunctionVM *functionvm;
        TraceThreadData *threaddata;
        #if (SYS_FUNCTIONS == 1)
        DBL *dblstack;
        #endif
        int nextArgument;

        void SetLocal(unsigned int k, DBL v);
        DBL GetLocal(unsigned int k);
};


#define OPCODE(i,s,d) ((i << 6) | (s << 3) | d)

#define OPCODE_ADD    OPCODE(0,0,0)
#define OPCODE_SUB    OPCODE(1,0,0)
#define OPCODE_MUL    OPCODE(2,0,0)
#define OPCODE_DIV    OPCODE(3,0,0)
#define OPCODE_MOD    OPCODE(4,0,0)
#define OPCODE_MOVE   OPCODE(5,0,0)
#define OPCODE_CMP    OPCODE(6,0,0)
#define OPCODE_NEG    OPCODE(7,0,0)
#define OPCODE_ABS    OPCODE(8,0,0)
#define OPCODE_ADDI   OPCODE(9,0,0)
#define OPCODE_SUBI   OPCODE(9,1,0)
#define OPCODE_MULI   OPCODE(9,2,0)
#define OPCODE_DIVI   OPCODE(9,3,0)
#define OPCODE_MODI   OPCODE(9,4,0)
#define OPCODE_LOADI  OPCODE(9,5,0)
#define OPCODE_CMPI   OPCODE(9,6,0)
#define OPCODE_SEQ    OPCODE(10,0,0)
#define OPCODE_SNE    OPCODE(10,1,0)
#define OPCODE_SLT    OPCODE(10,2,0)
#define OPCODE_SLE    OPCODE(10,3,0)
#define OPCODE_SGT    OPCODE(10,4,0)
#define OPCODE_SGE    OPCODE(10,5,0)
#define OPCODE_TEQ    OPCODE(10,6,0)
#define OPCODE_TNE    OPCODE(10,7,0)
#define OPCODE_LOAD   OPCODE(11,0,0)
#define OPCODE_STORE  OPCODE(12,0,0)
#define OPCODE_BEQ    OPCODE(13,0,0)
#define OPCODE_BNE    OPCODE(13,1,0)
#define OPCODE_BLT    OPCODE(13,2,0)
#define OPCODE_BLE    OPCODE(13,3,0)
#define OPCODE_BGT    OPCODE(13,4,0)
#define OPCODE_BGE    OPCODE(13,5,0)
#define OPCODE_XEQ    OPCODE(14,0,0)
#define OPCODE_XNE    OPCODE(14,1,0)
#define OPCODE_XLT    OPCODE(14,2,0)
#define OPCODE_XLE    OPCODE(14,3,0)
#define OPCODE_XGT    OPCODE(14,4,0)
#define OPCODE_XGE    OPCODE(14,5,0)
#define OPCODE_XDZ    OPCODE(14,6,0)
// #define OPCODE_XINF   OPCODE(14,7,0)
#define OPCODE_JSR    OPCODE(15,0,0)
#define OPCODE_JMP    OPCODE(15,0,1)
#define OPCODE_RTS    OPCODE(15,0,2)
#define OPCODE_CALL   OPCODE(15,0,3)
#define OPCODE_SYS1   OPCODE(15,0,4)
#define OPCODE_SYS2   OPCODE(15,0,5)
#define OPCODE_TRAP   OPCODE(15,0,6)
#define OPCODE_TRAPS  OPCODE(15,0,7)
#define OPCODE_GROW   OPCODE(15,1,0)
#define OPCODE_PUSH   OPCODE(15,1,1)
#define OPCODE_POP    OPCODE(15,1,2)
#define OPCODE_DEBUG  OPCODE(15,1,5)
#define OPCODE_NOP    OPCODE(15,3,7)

#define MAKE_INSTRUCTION(op, k) ((((k) << 12) & 0xfffff000) | ((op) & 0x00000fff))

#define GET_OP(w) ((w) & 0x00000fff)
#define GET_K(w) (((w) >> 12) & 0x000fffff)

extern const Opcode POVFPU_Opcodes[];

extern const Sys1 POVFPU_Sys1Table[];
extern const Sys2 POVFPU_Sys2Table[];

extern const unsigned int POVFPU_Sys1TableSize;
extern const unsigned int POVFPU_Sys2TableSize;

void POVFPU_Exception(FPUContext *context, FUNCTION fn, const char *msg = NULL);
DBL POVFPU_RunDefault(FPUContext *context, FUNCTION k);

void FNCode_Delete(FunctionCode *);

class FunctionVM : public GenericFunctionContextFactory
{
        friend void POVFPU_Exception(FPUContext *, FUNCTION, const char *);
        friend DBL POVFPU_RunDefault(FPUContext *, FUNCTION);
    public:

        class CustomFunction : public GenericScalarFunction
        {
            public:
                CustomFunction(FunctionVM* pVm, FUNCTION_PTR pFn);
                virtual ~CustomFunction();
                virtual GenericFunctionContextPtr AcquireContext(TraceThreadData* pThreadData);
                virtual void ReleaseContext(GenericFunctionContextPtr pContext);
                virtual void InitArguments(GenericFunctionContextPtr pContext);
                virtual void PushArgument(GenericFunctionContextPtr pContext, DBL arg);
                virtual DBL Execute(GenericFunctionContextPtr pContext);
                virtual GenericScalarFunctionPtr Clone() const;
                virtual const FunctionSourceInfo* GetSourceInfo() const;
            protected:
                FunctionVM *mpVm;
                FUNCTION_PTR mpFn;
        };

        FunctionVM();
        virtual ~FunctionVM();

        void Reset();

        void SetGlobal(unsigned int k, DBL v);
        DBL GetGlobal(unsigned int k);

        FunctionCode *GetFunction(FUNCTION k);
        FunctionCode *GetFunctionAndReference(FUNCTION k);

        unsigned int AddConstant(DBL v);

        FUNCTION AddFunction(FunctionCode *f);
        void RemoveFunction(FUNCTION fn);

        FUNCTION_PTR CopyFunction(FUNCTION_PTR pK);
        void DestroyFunction(FUNCTION_PTR pK);

        virtual GenericFunctionContextPtr CreateFunctionContext(TraceThreadData* pTd);

    private:

        vector<FunctionEntry> functions;
        FUNCTION nextUnreferenced;
        vector<DBL> globals;
        vector<DBL> consts;
};

}

#endif // POVRAY_VM_FNPOVFPU_H
