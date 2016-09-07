//******************************************************************************
///
/// @file platform/windows/syspovtask.cpp
///
/// Windows-specific partial implementation of the @ref Task class.
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

#endif

namespace vfePlatform
{

// implemented in `vfe/win/cpuinfo.cpp`
// TODO - Move the implementation into `platform/windows` somewhere.
extern bool GetCPUCount(unsigned int *TotAvailLogical, unsigned int *TotAvailCore, unsigned int *PhysicalNum);

// TODO - Maybe move this somewhere else as well.
static int GetNumberofCPUs (void)
{
    unsigned    logical;
    unsigned    cores;
    unsigned    physical;
    SYSTEM_INFO sysinfo;
    static int  result = -1;

    // we cache the result, since this function is called on each thread startup
    if (result != -1)
      return result;

    GetSystemInfo (&sysinfo) ;
    result = sysinfo.dwNumberOfProcessors;
    if (GetCPUCount(&logical, &cores, &physical))
        result = cores;
    return result;
}

}


namespace pov
{

using namespace vfePlatform;

//******************************************************************************


#if !POV_USE_DEFAULT_TASK_INITIALIZE

void Task::Initialize ()
{
    // NB This is not thread-safe, but we currently don't care.
    static volatile int count = 0;
    if (GetNumberofCPUs() > 1)
        SetThreadIdealProcessor (GetCurrentThread(), (count++ % GetNumberofCPUs()) + 1);
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
