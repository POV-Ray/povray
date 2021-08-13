//******************************************************************************
///
/// @file base/threadsafe.h
///
/// Declarations related to thread-safe access to certain library functions.
///
/// The Functions in this file are essentially wrapper functions for a couple
/// of standard C/C++ functions that are, by specification, not thread-safe.
///
/// The functions are designed for portability and robustness rather than
/// performance, and should therefore be used sparingly.
///
/// @note
///     Thread-safety of these functions is subject to the conditions that the
///     underlying library functions are **only** invoked via these wrappers.
///     Race conditions may still occur with threads invoking the underlying
///     functions directly.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2021 Persistence of Vision Raytracer Pty. Ltd.
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
//------------------------------------------------------------------------------
// SPDX-License-Identifier: AGPL-3.0-or-later
//******************************************************************************

#ifndef POVRAY_BASE_THREADSAFE_H
#define POVRAY_BASE_THREADSAFE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of standard C header files
#include <ctime>


namespace pov_base
{

//##############################################################################
///
/// @addtogroup PovBaseThreadsafe
///
/// @{

/// Convert from `std::time_t` to `std::tm` using UTC representation.
/// This is a thread-safe wrapper for `std::gmtime()` as defined in the `<ctime>` header.
/// @attention
///     The thread-safety of this function hinges on being the only entry point to the wrapped
///     function.
void SafeGmtime(std::tm* result, const std::time_t* t);

/// Convert from `std::time_t` to `std::tm` using local time representation.
/// This is a thread-safe wrapper for `std::localtime()` as defined in the `<ctime>` header.
/// @attention
///     The thread-safety of this function hinges on being the only entry point to the wrapped
///     function.
void SafeLocaltime(std::tm* result, const std::time_t* t);

/// Initialize locale settings.
/// @note
///     Any POV-Ray application needs to call this function on startup to work properly.
///     This is usually done by the back-end module function @ref povray_init().
void InitLocale();

/// @}
///
//##############################################################################

} // end of namespace

#endif // POVRAY_BASE_THREADSAFE_H
