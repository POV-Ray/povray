//******************************************************************************
///
/// @file platform/unix/syspovtimer.cpp
///
/// Unix-specific implementation of the @ref pov_base::Delay() function and
/// @ref pov_base::Timer class.
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

#include "syspovtimer.h"

#include <ctime>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "base/types.h"

// this must be the last file included
#include "base/povdebug.h"

#if !defined(HAVE_CLOCKID_T)
typedef int clockid_t;
#endif

#if !defined(HAVE_USECONDS_T)
typedef long useconds_t;
#endif

namespace pov_base
{

//******************************************************************************

#if !POV_USE_DEFAULT_DELAY

void Delay(unsigned int msec)
{
#if defined(HAVE_NANOSLEEP)
    timespec ts;
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (POV_ULONG) (1000000) * (msec % 1000);
    nanosleep(&ts, NULL);
#elif defined(HAVE_USLEEP)
    POV_ASSERT(msec < 1000); // On some systems, usleep() does not support sleeping for 1 second or more.
    usleep (msec * (useconds_t)1000);
#else
#error "Bad compile-time configuration."
#endif
}

#endif // !POV_USE_DEFAULT_DELAY

//******************************************************************************

#if !POV_USE_DEFAULT_TIMER

/// Attempt to get milliseconds time using `clock_gettime()`.
static inline bool ClockGettimeMillisec(POV_ULONG& result, clockid_t source)
{
#if defined(HAVE_CLOCK_GETTIME)
    struct timespec ts;
    bool success = (clock_gettime (source, &ts) == 0);
    if (success)
        result = static_cast<POV_ULONG>(ts.tv_sec)  *1000
               + static_cast<POV_ULONG>(ts.tv_nsec) /1000000;
    return success;
#else
    return false;
#endif
}

/// Attempt to get milliseconds elapsed CPU-time using `getrusage()`.
static inline bool GetrusageMillisec(POV_ULONG& result, int source)
{
#if defined(HAVE_GETRUSAGE)
    struct rusage ru;
    bool success = (getrusage(source, &ru) == 0);
    if (success)
        result = (static_cast<POV_ULONG>(ru.ru_utime.tv_sec)  + static_cast<POV_ULONG>(ru.ru_stime.tv_sec))  *1000
               + (static_cast<POV_ULONG>(ru.ru_utime.tv_usec) + static_cast<POV_ULONG>(ru.ru_stime.tv_usec)) /1000;
    return success;
#else
    return false;
#endif
}

/// Attempt to get milliseconds since the Epoch (1970-01-01) using `gettimeofday()`.
static inline bool GettimeofdayMillisec(POV_ULONG& result)
{
#if defined(HAVE_GETTIMEOFDAY)
    struct timeval tv;  // seconds + microseconds since
    bool success = (gettimeofday(&tv, NULL) == 0);
    if (success)
        result = static_cast<POV_ULONG>(tv.tv_sec)  *1000
               + static_cast<POV_ULONG>(tv.tv_usec) /1000;
    return success;
#else
    return false;
#endif
}

Timer::Timer () :
    mWallTimeUseClockGettimeMonotonic (false),
    mWallTimeUseClockGettimeRealtime (false),
    mWallTimeUseGettimeofday (false),
    mProcessTimeUseGetrusageSelf (false),
    mProcessTimeUseClockGettimeProcess (false),
    mProcessTimeUseFallback (false),
    mThreadTimeUseGetrusageThread (false),
    mThreadTimeUseGetrusageLwp (false),
    mThreadTimeUseClockGettimeThread (false),
    mThreadTimeUseFallback (false)
{
    // Figure out which timer source to use for wall clock time.
    bool haveWallTime = false;
#if defined(HAVE_DECL_CLOCK_MONOTONIC) && HAVE_DECL_CLOCK_MONOTONIC
    if (!haveWallTime)
        haveWallTime = mWallTimeUseClockGettimeMonotonic = ClockGettimeMillisec(mWallTimeStart, CLOCK_MONOTONIC);
#endif
    // we prefer CLOCK_MONOTONIC over CLOCK_REALTIME because the former will not be affected if someone adjusts the
    // system's real-time clock.
#if defined(HAVE_DECL_CLOCK_REALTIME) && HAVE_DECL_CLOCK_REALTIME
    if (!haveWallTime)
        haveWallTime = mWallTimeUseClockGettimeRealtime = ClockGettimeMillisec(mWallTimeStart, CLOCK_REALTIME);
#endif
    // TODO - Find out if there is some rationale behind the preference of clock_gettime() over
    //        gettimeofday(), and document it here.
    if (!haveWallTime)
        haveWallTime = mWallTimeUseGettimeofday = GettimeofdayMillisec(mWallTimeStart);
    // FIXME: add fallback, using ftime(), or time() + a counter for ms, or maybe boost::date_time
    if (!haveWallTime)
    {
        POV_ASSERT(false);
        mWallTimeStart = 0;
    }

    // Figure out which timer source to use for per-process CPU time.
    bool haveProcessTime = false;
#if defined(HAVE_DECL_RUSAGE_SELF) && HAVE_DECL_RUSAGE_SELF
    if (!haveProcessTime)
        haveProcessTime = mProcessTimeUseGetrusageSelf = GetrusageMillisec(mProcessTimeStart, RUSAGE_SELF);
#endif
    // We prefer getrusage() over clock_gettime() because the latter may be inaccurate on systems
    // with multiple physical processors.
#if defined(HAVE_DECL_CLOCK_PROCESS_CPUTIME_ID) && HAVE_DECL_CLOCK_PROCESS_CPUTIME_ID
    if (!haveProcessTime)
        haveProcessTime = mProcessTimeUseClockGettimeProcess = ClockGettimeMillisec(mProcessTimeStart, CLOCK_PROCESS_CPUTIME_ID);
#endif
    if (!haveProcessTime)
    {
        haveProcessTime = mProcessTimeUseFallback = haveWallTime;
        mProcessTimeStart = mWallTimeStart;
    }

    // Figure out which timer source to use for per-thread CPU time.
    bool haveThreadTime = false;
#if defined(HAVE_DECL_RUSAGE_THREAD) && HAVE_DECL_RUSAGE_THREAD
    if (!haveThreadTime)
        haveThreadTime = mThreadTimeUseGetrusageThread = GetrusageMillisec(mThreadTimeStart, RUSAGE_THREAD);
#elif defined(HAVE_DECL_RUSAGE_LWP) && HAVE_DECL_RUSAGE_LWP // should be alias of RUSAGE_THREAD on systems that support both
    if (!haveThreadTime)
        haveThreadTime = mThreadTimeUseGetrusageLwp = GetrusageMillisec(mThreadTimeStart, RUSAGE_LWP);
#endif
    // We prefer getrusage() over clock_gettime() because the latter may be inaccurate on systems
    // with multiple physical processors.
#if defined(HAVE_DECL_CLOCK_THREAD_CPUTIME_ID) && HAVE_DECL_CLOCK_THREAD_CPUTIME_ID
    if (!haveThreadTime)
        haveThreadTime = mThreadTimeUseClockGettimeThread = ClockGettimeMillisec(mThreadTimeStart, CLOCK_THREAD_CPUTIME_ID);
#endif
    if (!haveThreadTime)
    {
        haveThreadTime = mThreadTimeUseFallback = haveProcessTime;
        mThreadTimeStart = mProcessTimeStart;
    }
}

Timer::~Timer ()
{
    // nothing to do
}

POV_ULONG Timer::GetWallTime () const
{
    POV_ULONG result;
#if defined(HAVE_DECL_CLOCK_MONOTONIC) && HAVE_DECL_CLOCK_MONOTONIC
    if (mWallTimeUseClockGettimeMonotonic)
        return (ClockGettimeMillisec(result, CLOCK_MONOTONIC) ? result : 0);
#endif
#if defined(HAVE_DECL_CLOCK_REALTIME) && HAVE_DECL_CLOCK_REALTIME
    if (mWallTimeUseClockGettimeRealtime)
        return (ClockGettimeMillisec(result, CLOCK_REALTIME) ? result : 0);
#endif
    if (mWallTimeUseGettimeofday)
        return (GettimeofdayMillisec(result) ? result : 0);
    return 0;
}

POV_ULONG Timer::GetProcessTime () const
{
    POV_ULONG result;
#if defined(HAVE_DECL_RUSAGE_SELF) && HAVE_DECL_RUSAGE_SELF
    if (mProcessTimeUseGetrusageSelf)
        return (GetrusageMillisec(result, RUSAGE_SELF) ? result : 0);
#endif
#if defined(HAVE_DECL_CLOCK_PROCESS_CPUTIME_ID) && HAVE_DECL_CLOCK_PROCESS_CPUTIME_ID
    if (mProcessTimeUseClockGettimeProcess)
        return (ClockGettimeMillisec(result, CLOCK_PROCESS_CPUTIME_ID) ? result : 0);
#endif
    if (mProcessTimeUseFallback)
        return GetWallTime ();
    return 0;
}

POV_ULONG Timer::GetThreadTime () const
{
    POV_ULONG result;
#if defined(HAVE_DECL_RUSAGE_THREAD) && HAVE_DECL_RUSAGE_THREAD
    if (mThreadTimeUseGetrusageThread)
        return (GetrusageMillisec(result, RUSAGE_THREAD) ? result : 0);
#endif
#if defined(HAVE_DECL_RUSAGE_LWP) && HAVE_DECL_RUSAGE_LWP
    if (mThreadTimeUseGetrusageLwp)
        return (GetrusageMillisec(result, RUSAGE_LWP) ? result : 0);
#endif
#if defined(HAVE_DECL_CLOCK_THREAD_CPUTIME_ID) && HAVE_DECL_CLOCK_THREAD_CPUTIME_ID
    if (mThreadTimeUseClockGettimeThread)
        return (ClockGettimeMillisec(result, CLOCK_THREAD_CPUTIME_ID) ? result : 0);
#endif
    if (mThreadTimeUseFallback)
        return GetProcessTime ();
    return 0;
}

POV_LONG Timer::ElapsedRealTime () const
{
    return GetWallTime () - mWallTimeStart;
}

POV_LONG Timer::ElapsedProcessCPUTime () const
{
    return GetProcessTime () - mProcessTimeStart;
}

POV_LONG Timer::ElapsedThreadCPUTime () const
{
    return GetThreadTime () - mThreadTimeStart;
}

void Timer::Reset ()
{
    mWallTimeStart    = GetWallTime ();
    mProcessTimeStart = GetProcessTime ();
    mThreadTimeStart  = GetThreadTime ();
}

bool Timer::HasValidProcessCPUTime () const
{
    return !mProcessTimeUseFallback;
}

bool Timer::HasValidThreadCPUTime () const
{
    return !mThreadTimeUseFallback;
}

#endif // !POV_USE_DEFAULT_TIMER

//******************************************************************************

}
