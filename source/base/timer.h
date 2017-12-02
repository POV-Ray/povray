//******************************************************************************
///
/// @file base/timer.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_BASE_TIMER_H
#define POVRAY_BASE_TIMER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

#if POV_USE_DEFAULT_TIMER
#include <boost/thread/xtime.hpp>
#endif

#if !POV_USE_DEFAULT_TIMER || (POV_MULTITHREADED && !POV_USE_DEFAULT_DELAY)
#include "syspovtimer.h"
#endif

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBaseTimer Time Handling
/// @ingroup PovBase
///
/// @{

#if POV_MULTITHREADED && POV_USE_DEFAULT_DELAY

/// Wait for the specified time.
///
/// This function puts the current thread into idle mode for the specified time.
///
/// @note
///     This is a default implementation, provided only as a last-ditch resort for platforms that
///     cannot provide a better implementation.
///
/// @todo
///     The current implementation is based on boost::xtime, which has been deprecated since
///     boost 1.34.
///
/// @attention
///     Due to possible limitations of platform-specific implementations, this function may only be
///     called to wait for less than 1 second.
///
/// @attention
///     Due to possible limitations of platform-specific implementations, callers must not rely on
///     the duration to be exact, or even anywhere close. Most notably, the function may return
///     prematurely in case the thread receives a signal.
///
/// @param[in]  msec    Time to wait in milliseconds (0..999).
///
void Delay(unsigned int msec);

#endif // POV_MULTITHREADED && POV_USE_DEFAULT_DELAY

#if POV_USE_DEFAULT_TIMER

/// Millisecond-precision timer.
///
/// @note
///     This is a default implementation, provided only as a last-ditch resort for platforms that
///     cannot provide a better implementation. It can neither guarantee millisecond precision, nor
///     does it support measurement of CPU time.
///
/// @todo
///     This implementation is based on boost::xtime, which has been deprecated since 1.34.
///
/// @impl
///     Note that to measure per-process CPU time we're not resorting to `clock()` as a default
///     implementation, as depending on the platform it may incorrectly report elapsed wall-clock
///     time (sources on the internet report this for Solaris, and it is a well-documented fact for
///     Windows), and/or may be limited to timespans in the order of an hour (any system with a
///     32-bit `clock_t` and a standard `CLOCKS_PER_SEC` of 1,000,000).
///
class Timer
{
    public:

        /// Create and start a new timer.
        ///
        Timer();

        /// Destroy the timer.
        ///
        ~Timer();

        /// Report elapsed wall-clock time.
        ///
        /// This method reports the actual time in milliseconds that has elapsed since the timer's
        /// creation or last call to @ref Reset().
        ///
        /// @return     Elapsed real time in milliseconds.
        ///
        POV_LONG ElapsedRealTime() const;

        /// Report CPU time consumed by current process.
        ///
        /// This method reports the CPU time in milliseconds that the current process has consumed
        /// since the timer's creation or last call to @ref Reset().
        ///
        /// @note
        ///     Timer implementations that do not support per-process measurement of CPU time
        ///     consumption should fall back to reporting wall-clock time.
        ///
        /// @return     Elapsed CPU time in milliseconds.
        ///
        inline POV_LONG ElapsedProcessCPUTime() const { return ElapsedRealTime(); }

        /// Report CPU time consumed by current thread.
        ///
        /// This method reports the CPU time in milliseconds that the current thread has consumed
        /// since the timer's creation or last call to @ref Reset().
        ///
        /// @note
        ///     If the timer was created or reset from a different thread, the result is undefined.
        ///
        /// @note
        ///     Timer implementations that do not support per-thread measurement of CPU time
        ///     consumption should fall back to reporting CPU time consumed by the process if
        ///     supported, or elapsed wall-clock time otherwise.
        ///
        /// @return     Elapsed CPU time in milliseconds.
        ///
        inline POV_LONG ElapsedThreadCPUTime() const { return ElapsedProcessCPUTime(); }

        /// Reset the timer.
        ///
        void Reset();

        /// Report whether per-process measurement of CPU time is supported.
        ///
        /// This method reports whether the timer does indeed support per-process measurement of CPU
        /// time consumption, or falls back to some other time measurement instead.
        ///
        /// @return     `true` if @ref ElapsedProcessCPUTime() does indeed report per-thread CPU time
        ///             consumption.
        ///
        inline bool HasValidProcessCPUTime() const { return false; }

        /// Report whether per-thread measurement of CPU time is supported.
        ///
        /// This method reports whether the timer does indeed support per-thread measurement of CPU
        /// time consumption, or falls back to some other time measurement instead.
        ///
        /// @return     `true` if @ref ElapsedThreadCPUTime() does indeed report per-thread CPU time
        ///             consumption.
        ///
        inline bool HasValidThreadCPUTime() const { return false; }

    private:

        /// real time at last reset
        boost::xtime mRealTimeStart;
};

#endif // POV_USE_DEFAULT_TIMER

/// @}
///
//##############################################################################

}

#endif // POVRAY_BASE_TIMER_H
