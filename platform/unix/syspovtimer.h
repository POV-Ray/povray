//******************************************************************************
///
/// @file platform/unix/syspovtimer.h
///
/// Unix-specific declaration of the @ref pov_base::Delay() function and
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

#ifndef POVRAY_UNIX_SYSPOVTIMER_H
#define POVRAY_UNIX_SYSPOVTIMER_H

#include "base/configbase.h"

#include "base/timer.h"

namespace pov_base
{

#if !POV_USE_DEFAULT_TIMER

// NOTE: Although we're currently not using platform-specific implementations
// of the wall clock timer, we may want to keep them around in case we find the
// default implementation wanting on particular flavours of Unix.
#define POVUNIX_USE_DEFAULT_REAL_TIMER 1

class DefaultRealTimer;

/// Millisecond-precision timer.
///
/// This is the Unix-specific implementation of the millisecond-precision timer required by POV-Ray.
///
/// @impl
///     Note that to measure per-process CPU time we're not using `clock()` as a fallback
///     implementation, as depending on the platform it may incorrectly report elapsed wall-clock
///     time (sources on the internet report this for Solaris), and/or may be limited to timespans
///     in the order of an hour (any system with a 32-bit `clock_t` and a standard `CLOCKS_PER_SEC`
///     of 1,000,000).
///
class Timer final
{
    public:

        Timer();
        ~Timer() = default;

#if POVUNIX_USE_DEFAULT_REAL_TIMER
        inline POV_LONG ElapsedRealTime() const { return mRealTimer.ElapsedTime(); }
#else
        POV_LONG ElapsedRealTime() const;
#endif
        POV_LONG ElapsedProcessCPUTime() const;
        POV_LONG ElapsedThreadCPUTime() const;

        void Reset();

        bool HasValidProcessCPUTime() const;
        bool HasValidThreadCPUTime() const;

    private:

#if POVUNIX_USE_DEFAULT_REAL_TIMER
        DefaultRealTimer mRealTimer;
#else
        POV_ULONG mWallTimeStart;
#endif
        POV_ULONG mProcessTimeStart;
        POV_ULONG mThreadTimeStart;

#if !POVUNIX_USE_DEFAULT_REAL_TIMER
        bool mWallTimeUseClockGettimeMonotonic  : 1;    ///< Whether we'll measure elapsed wall-clock time using `clock_gettime(CLOCK_MONOTONIC)`.
        bool mWallTimeUseClockGettimeRealtime   : 1;    ///< Whether we'll measure elapsed wall-clock time using `clock_gettime(CLOCK_REALTIME)`.
        bool mWallTimeUseGettimeofday           : 1;    ///< Whether we'll measure elapsed wall-clock time using `gettimeofday()`.
#endif

        bool mProcessTimeUseGetrusageSelf       : 1;    ///< Whether we'll measure per-process CPU time using `getrusage(RUSAGE_SELF)`.
        bool mProcessTimeUseClockGettimeProcess : 1;    ///< Whether we'll measure per-process CPU time using `clock_gettime(CLOCK_PROCESS_CPUTIME_ID)`.
        bool mProcessTimeUseFallback            : 1;    ///< Whether we'll fall back to wall-clock time instead of per-process CPU time.

        bool mThreadTimeUseGetrusageThread      : 1;    ///< Whether we'll measure per-thread CPU time using `getrusage(RUSAGE_THREAD)`.
        bool mThreadTimeUseGetrusageLwp         : 1;    ///< Whether we'll measure per-thread CPU time using `getrusage(RUSAGE_LWP)`.
        bool mThreadTimeUseClockGettimeThread   : 1;    ///< Whether we'll measure per-thread CPU time `clock_gettime(CLOCK_THREAD_CPUTIME_ID)`.
        bool mThreadTimeUseFallback             : 1;    ///< Whether we'll fall back to per-process CPU time (or wall-clock time) instead of per-thread CPU time.

#if !POVUNIX_USE_DEFAULT_REAL_TIMER
        POV_ULONG GetWallTime() const;
#endif
        POV_ULONG GetThreadTime() const;
        POV_ULONG GetProcessTime() const;
};

#endif // !POV_USE_DEFAULT_TIMER

}
// end of namespace pov_base

#endif // POVRAY_UNIX_SYSPOVTIMER_H
