//******************************************************************************
///
/// @file vm/fnintern.h
///
/// This module contains declarations for the built-in render-time functions.
///
/// This module is inspired by code by D. Skarda, T. Bily and R. Suzuki.
/// It includes functions based on code first introduced by many other
/// contributors.
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

#ifndef POVRAY_BACKEND_FNINTERN_H
#define POVRAY_BACKEND_FNINTERN_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "vm/configvm.h"

namespace pov
{

class FPUContext;

typedef struct
{
    DBL (*fn)(FPUContext *ctx, DBL *ptr, unsigned int fn);
    unsigned int parameter_cnt;
} Trap;

typedef struct
{
    void (*fn)(FPUContext *ctx, DBL *ptr, unsigned int fn, unsigned int sp);
    unsigned int parameter_cnt;
} TrapS;

extern const Trap POVFPU_TrapTable[];
extern const TrapS POVFPU_TrapSTable[];

extern const unsigned int POVFPU_TrapTableSize;
extern const unsigned int POVFPU_TrapSTableSize;

}

#endif // POVRAY_BACKEND_FNINTERN_H
