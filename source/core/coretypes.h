//******************************************************************************
///
/// @file core/coretypes.h
///
/// Essential types for the render core.
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

#ifndef POVRAY_CORE_CORETYPES_H
#define POVRAY_CORE_CORETYPES_H

#include "core/configcore.h"
#include "base/textstream.h"

namespace pov
{

class TraceThreadData;

class GenericFunctionContext
{
    public:
        virtual ~GenericFunctionContext() {}
};

typedef GenericFunctionContext* GenericFunctionContextPtr;

struct FunctionSourceInfo
{
    char* name;
    UCS2* filename;
    pov_base::ITextStream::FilePos filepos;
};

template<typename RETURN_T, typename ARG_T>
class GenericCustomFunction
{
    public:
        virtual ~GenericCustomFunction() {}
        virtual GenericFunctionContextPtr NewContext(TraceThreadData* pThreadData) = 0;
        virtual void InitArguments(GenericFunctionContextPtr pContext) = 0;
        virtual void PushArgument(GenericFunctionContextPtr pContext, ARG_T arg) = 0;
        virtual RETURN_T Execute(GenericFunctionContextPtr pContext) = 0;
        virtual GenericCustomFunction* Clone() const = 0;
        virtual const FunctionSourceInfo* GetSourceInfo() const { return NULL; }
};

typedef GenericCustomFunction<double, double> GenericScalarFunction;
typedef GenericScalarFunction* GenericScalarFunctionPtr;

}

#endif // POVRAY_CORE_CORETYPES_H
