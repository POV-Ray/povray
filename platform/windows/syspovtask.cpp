//******************************************************************************
///
/// @file platform/windows/syspovtask.cpp
///
/// Windows-specific partial implementation of the @ref Task class.
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

#include "syspovtask.h"

#include <windows.h>

// this must be the last file included
#include "base/povdebug.h"


#ifndef _CONSOLE

namespace povwin
{

// implemented in `windows/pvmem.cpp`
// TODO - Move the implementation into `platform/windows` somewhere.
void WinMemThreadStartup();
void WinMemThreadCleanup();

}
// end of namespace povwin

#endif

namespace vfePlatform
{

// TODO - Maybe move this somewhere else as well.
static unsigned int GetNumberofCPUs (void)
{
    SYSTEM_INFO         sysinfo;
    static unsigned int result = 0;

    // we cache the result, since this function is called on each thread startup
    // TODO - this isn't ideal on systems with hot-pluggable CPUs
    if (result != 0)
      return result;

    GetSystemInfo (&sysinfo) ;
    result = sysinfo.dwNumberOfProcessors;
    return result;
}

}
// end of namespace vfeplatform


namespace pov
{

using namespace vfePlatform;

//******************************************************************************


#if !POV_USE_DEFAULT_TASK_INITIALIZE

void Task::Initialize ()
{
    // NB This is not thread-safe, but we currently don't care.
    static volatile unsigned int count = 0;
    unsigned int numCPUs = GetNumberofCPUs();
    // TODO - if numCPUs > 64, we need to do more than this
    if (numCPUs > 1)
        SetThreadIdealProcessor (GetCurrentThread(), (count++) % numCPUs);
#ifndef _CONSOLE
    povwin::WinMemThreadStartup();
#endif
}

#endif // !POV_USE_DEFAULT_TASK_INITIALIZE

#if !POV_USE_DEFAULT_TASK_CLEANUP

void Task::Cleanup ()
{
#ifndef _CONSOLE
    povwin::WinMemThreadCleanup();
#endif
}

#endif // !POV_USE_DEFAULT_TASK_CLEANUP

//******************************************************************************

}
// end of namespace pov
