//******************************************************************************
///
/// @file platform/windows/syspovtimer.cpp
///
/// Windows-specific implementation of the @ref pov_base::Delay() function and
/// @ref pov_base::Timer class.
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

#include "syspovtimer.h"

#include <sys/timeb.h>
#include <sys/types.h>
#include <windows.h>

#include "base/povassert.h"
#include "base/types.h"

#include "osversioninfo.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

//******************************************************************************

#if POV_USE_PLATFORM_DELAY

// NOTE: Although we're currently not using this implementation, we may want to
// keep it around in case we find the default implementation wanting on Windows
// systems in general or some flavours in particular.
void Delay(unsigned int msec)
{
    Sleep (msec);
}

#endif // POV_USE_PLATFORM_DELAY

//******************************************************************************

#if !POV_USE_DEFAULT_TIMER

Timer::Timer () :
    // TODO - sources on the internet indicate that GetThreadTimes() and GetProcessTimes() have been
    //        around as early as NT 3.1. Is there a reason we're only making use of it in NT 4.0 and
    //        later Windows versions?
    mThreadHandle (nullptr),
    mCPUTimeSupported (WindowsVersionDetector().IsNTVersion (4,0))
{
    if (mCPUTimeSupported)
    {
        if (!DuplicateHandle (GetCurrentProcess (), GetCurrentThread (), GetCurrentProcess (),
                              &mThreadHandle, 0, TRUE, DUPLICATE_SAME_ACCESS))
        {
            POV_ASSERT (false);
            mThreadHandle = nullptr;
        }
    }
    Reset ();
}

Timer::~Timer ()
{
    if (mThreadHandle != nullptr)
        CloseHandle (mThreadHandle);
}

POV_ULONG Timer::GetWallTime () const
{
    struct timeb tb;
    ftime(&tb);
    return (static_cast<POV_ULONG>(tb.time) * 1000 + tb.millitm);
}

POV_ULONG Timer::GetThreadTime () const
{
    FILETIME    ct;
    FILETIME    et;
    __int64     kt;
    __int64     ut;
    BOOL        success;

    POV_ASSERT (mCPUTimeSupported);

    success = GetThreadTimes (mThreadHandle, &ct, &et,
                              reinterpret_cast<FILETIME *>(&kt),
                              reinterpret_cast<FILETIME *>(&ut));

    POV_ASSERT (success);
    if (!success)
        return 0;

    return ((kt + ut) / 10000);
}

POV_ULONG Timer::GetProcessTime () const
{
    FILETIME    ct;
    FILETIME    et;
    __int64     kt;
    __int64     ut;
    BOOL        success;

    POV_ASSERT (mCPUTimeSupported);

    success = GetProcessTimes (GetCurrentProcess (), &ct, &et,
                               reinterpret_cast<FILETIME *>(&kt),
                               reinterpret_cast<FILETIME *>(&ut));

    POV_ASSERT (success);
    if (!success)
        return 0;

    return ((kt + ut) / 10000);
}

POV_LONG Timer::ElapsedRealTime () const
{
    return GetWallTime () - mWallTimeStart;
}

POV_LONG Timer::ElapsedThreadCPUTime () const
{
    if (mCPUTimeSupported)
        return GetThreadTime () - mThreadTimeStart;
    else
        return GetWallTime () - mWallTimeStart;
}

POV_LONG Timer::ElapsedProcessCPUTime () const
{
    if (mCPUTimeSupported)
        return GetProcessTime () - mProcessTimeStart;
    else
        return GetWallTime () - mWallTimeStart;
}

void Timer::Reset ()
{
    mWallTimeStart = GetWallTime ();
    if (mCPUTimeSupported)
    {
        mThreadTimeStart = GetThreadTime ();
        mProcessTimeStart = GetProcessTime ();
    }
}

bool Timer::HasValidThreadCPUTime () const
{
    return mCPUTimeSupported;
}

bool Timer::HasValidProcessCPUTime () const
{
    return mCPUTimeSupported;
}

#endif // POV_USE_DEFAULT_TIMER

//******************************************************************************

}
// end of namespace pov_base
