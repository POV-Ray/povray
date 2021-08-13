//******************************************************************************
///
/// @file base/threadsafe.cpp
///
/// Implementations related to thread-safe access to certain library functions.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/threadsafe.h"

// C++ variants of standard C header files
#include <clocale>

// Standard C++ header files
#include <mutex>

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

std::mutex gCtimeMutex;     /// Mutex for access to `<ctime>` thread-unsafe functions.
std::mutex gLocaleMutex;    /// Mutex for calls to `setlocale`.

void SafeGmtime(std::tm* result, const std::time_t* t)
{
    const std::lock_guard<std::mutex> lock(gCtimeMutex);
    *result = *std::gmtime(t);
}

void SafeLocaltime(std::tm* result, const std::time_t* t)
{
    const std::lock_guard<std::mutex> lock(gCtimeMutex);
    *result = *std::localtime(t);
}

void InitLocale()
{
    const std::lock_guard<std::mutex> lock(gLocaleMutex);
    // Standard "C" locale for all aspects, except as amended further below.
    // (NB: Since this is the standard C/C++ default, it shouldn't matter, but it also won't hurt.)
    std::setlocale(LC_ALL, "C");
    // User-preferred locale for date/time aspect.
    std::setlocale(LC_TIME, "");
}

} // end of namespace
