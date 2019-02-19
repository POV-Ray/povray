//******************************************************************************
///
/// @file vm/configvm.h
///
/// This header file defines all types that can be configured by platform
/// specific code for VM use. It further allows insertion of platform specific
/// function prototypes making use of those types.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_VM_CONFIGVM_H
#define POVRAY_VM_CONFIGVM_H

// Pull in other compile-time config header files first
#include "core/configcore.h"
#include "syspovconfigvm.h"

//##############################################################################
///
/// @defgroup PovVMConfig VM Compile-Time Configuration
/// @ingroup PovVM
/// @ingroup PovConfig
///
/// @{

// Adjust to match floating-point parameter(s) of functions in math.h/cmath
#ifndef SYS_MATH_PARAM
    #define SYS_MATH_PARAM double
#endif

// Adjust to match floating-point return value of functions in math.h/cmath
#ifndef SYS_MATH_RETURN
    #define SYS_MATH_RETURN double
#endif

// Function that executes functions, the parameter is the function index
#ifndef POVFPU_Run
    #define POVFPU_Run(ctx, fn) POVFPU_RunDefault(ctx, fn)
#endif

// Adjust to add system specific handling of functions like just-in-time compilation
#ifndef SYS_FUNCTIONS
    // Note that if SYS_FUNCTIONS is 1, it will enable the field dblstack
    // in FPUContext_Struct and corresponding calculations in POVFPU_SetLocal
    // as well as POVFPU_NewContext.
    #define SYS_FUNCTIONS 0
#endif
#if SYS_FUNCTIONS == 0
    // Called after a function has been added, parameter is the function index
    #define SYS_ADD_FUNCTION(fe)
    // Called before a function is deleted, parameter is a pointer to the FunctionEntry_Struct
    #define SYS_DELETE_FUNCTION(fe)
    // Called inside POVFPU_Init after everything else has been inited
    #define SYS_INIT_FUNCTIONS()
    // Called inside POVFPU_Terminate before anything else is deleted
    #define SYS_TERM_FUNCTIONS()
    // Called inside POVFPU_Reset before anything else is reset
    #define SYS_RESET_FUNCTIONS()
    // Adjust to add system specific fields to FunctionEntry_Struct
    #define SYS_FUNCTION_ENTRY
#endif // SYS_FUNCTIONS


//******************************************************************************
///
/// @name Debug Settings.
///
/// The following settings enable or disable certain debugging aids, such as run-time sanity checks
/// or additional log output.
///
/// Unless noted otherwise, a non-zero integer will enable the respective debugging aids, while a
/// zero value will disable them.
///
/// It is recommended that system-specific configurations leave these settings undefined in release
/// builds, in which case they will default to @ref POV_DEBUG unless noted otherwise.
///
/// @{

/// @def POV_VM_DEBUG
/// Enable run-time sanity checks for the @ref PovVM.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_VM_DEBUG
    #define POV_VM_DEBUG POV_DEBUG
#endif

/// @}
///
//******************************************************************************
///
/// @name Non-Configurable Macros
///
/// The following macros are configured automatically at compile-time; they cannot be overridden by
/// system-specific configuration.
///
/// @{

#if POV_VM_DEBUG
    #define POV_VM_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_VM_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_VM_CONFIGVM_H
