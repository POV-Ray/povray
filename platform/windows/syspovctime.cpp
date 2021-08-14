//******************************************************************************
///
/// @file platform/windows/syspovctime.cpp
///
/// Windows-specific implementation of `<ctime>`-related functions.
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

#include "syspovctime.h"

#include "base/pov_err.h"
#include "base/types.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

std::tm* safe_gmtime(const std::time_t* ts, std::tm* buf)
{
    auto err = gmtime_s(buf, ts);
    if (err != 0)
        throw POV_EXCEPTION_CODE(kParamErr);
    return buf;
}

std::tm* safe_localtime(const std::time_t* ts, std::tm* buf)
{
    auto err = localtime_s(buf, ts);
    if (err != 0)
        throw POV_EXCEPTION_CODE(kParamErr);
    return buf;
}

std::time_t mktime_utc(std::tm* t)
{
    return _mkgmtime(t);
}

} // end of namespace pov_base
