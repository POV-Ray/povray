//******************************************************************************
///
/// @file base/povassert.h
///
/// Declarations related to run-time sanity checks.
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

#ifndef POVRAY_BASE_POVASSERT_H
#define POVRAY_BASE_POVASSERT_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of C standard header files
#include <cassert>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"

namespace pov_base
{

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

/// A macro wrapping a sequence of statements into a single one.
///
/// This macro is intended to be used in the definition of other macros that should behave
/// syntactically like a single statement, while evaluating to something that would not normally
/// behave that way.
///
/// Example:
///
///     #declare FOO(x) SINGLE_STATEMENT( char buf[128]; foo(buf,x); )
///     ...
///     if (some_cond)
///         FOO(a);
///     else
///         ...
///
#define SINGLE_STATEMENT( block ) do { block } while (false)

/// A macro that does nothing.
///
/// This macro is intended to be used in the definition of other macros that should behave
/// syntactically like a single statement, while evaluating to a no-operation.
///
/// Example:
///
///     #declare MY_ASSERT(x) NO_OP
///     ...
///     if (some_cond)
///         MY_ASSERT(some_test);
///     else
///         ...
///
#define NO_OP SINGLE_STATEMENT(;)

/// A macro that tests an expression and, if it evaluates false, throws an exception to allow the
/// application to fail gracefully.
///
#define POV_ASSERT_SOFT(expr) SINGLE_STATEMENT( if(!(expr)) throw POV_EXCEPTION_CODE(kUncategorizedError); )

/// A macro that tests an expression and, if it evaluates false, causes a hard crash to generate a
/// core dump or break to a debugger.
///
#define POV_ASSERT_HARD(expr) assert(expr)

/// A macro that does nothing, but is mapped to standard `assert()` during static code analysis.
///
#ifdef STATIC_CODE_ANALYSIS
    #define POV_ASSERT_DISABLE(expr) assert(expr)
#else
    #define POV_ASSERT_DISABLE(expr) NO_OP
#endif


/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_POVASSERT_H
