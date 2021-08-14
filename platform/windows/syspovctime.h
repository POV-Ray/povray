//******************************************************************************
///
/// @file platform/windows/syspovctime.h
///
/// Windows-specific declaration of `<ctime>`-related functions.
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

#ifndef POVRAY_WINDOWS_SYSPOVCTIME_H
#define POVRAY_WINDOWS_SYSPOVCTIME_H

#include "base/configbase.h"

#include <ctime>

namespace pov_base
{

/// Thread-Safe Replacement for `std::gmtime()`.
/// This function shall do the same job as `std::gmtime()`, except that it
/// shall use the supplied buffer to store the result, rather than a
/// static one.
std::tm* safe_gmtime(const std::time_t* ts, std::tm* buf);

/// Thread-Safe Replacement for `std::localtime()`.
/// This function shall do the same job as `std::gmtime()`, except that it
/// shall use the supplied buffer to store the result, rather than a
/// static one.
std::tm* safe_localtime(const std::time_t* ts, std::tm* buf);

/// UTC-Based Equivalent of `std::mktime()`.
/// This function shall do the same job as `std::mktime()`, except that it
/// shall interpret the input data as UTC.
std::time_t mktime_utc(std::tm* t);

} // end of namespace pov_base

#endif // POVRAY_WINDOWS_SYSPOVCTIME_H
