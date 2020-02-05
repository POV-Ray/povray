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

#ifndef POVRAY_BASE_TIMER_H
#define POVRAY_BASE_TIMER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <chrono>

// NOTE:
// Another file is included at the end of this file.

// POV-Ray header files (base module)
//  (none at the moment)

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBaseTimer Time Handling
/// @ingroup PovBase
///
/// @{

/// Wait for the specified time.
///
/// This function puts the current thread into idle mode for the specified time,
/// give or take.
///
/// The default implementation is based on `std::this_thread::sleep_for`, which
/// should be good enough on most platforms. However, platforms can still
/// provide their own implementations, by setting @ref POV_USE_PLATFORM_DELAY to
/// non-zero and providing their own definition.
///
/// @note
///     For low values (e.g. below 10 ms, but depending on implementation and
///     platform) this function may effectively be a no-op.
///
/// @param[in]  msec    Time to wait in milliseconds (0..999).
///
void Delay(unsigned int msec);

/// Default Millisecond-precision wall clock timer.
///
/// This class provides facilities to measure the elapsed wall clock time
/// since the object was created or last reset.
///
/// @note
///     This class should not be used directly. Instead, it is intended as a
///     building block for implementations of the @ref Timer class.
///
/// @impl
///     The current implementation is based on `std::chrono::steady_clock`.
///
class DefaultRealTimer final
{
public:

    /// Create and start a new timer.
    DefaultRealTimer();

    /// Report elapsed wall-clock time.
    ///
    /// This method reports the actual time in milliseconds that has elapsed
    /// since the timer's creation or last call to @ref Reset().
    ///
    /// @return     Elapsed real time in milliseconds.
    ///
    POV_LONG ElapsedTime() const;

    /// Reset the timer.
    void Reset();

private:

    /// Point in time of construction or last reset.
    std::chrono::steady_clock::time_point mRealTimeStart;
};

#if POV_USE_DEFAULT_TIMER

/// Millisecond-precision timer.
///
/// This class provides facilities to measure the elapsed wall clock time, CPU
/// time used by the current process, and CPU time used by the current thread,
/// since the object was created or last reset.
///
/// It is intended that platforms provide their own implementations, by setting
/// @ref POV_USE_DEFAULT_TIMER to non-zero, providing their own declaration in
/// `syspovtimer.h`, and providing their own definition.
///
/// @note
///     The default implementation is provided only as a last-ditch resort for platforms that
///     cannot provide a better implementation. It can neither guarantee millisecond precision, nor
///     does it support measurement of CPU time.
///
/// @impl
///     Note that to measure per-process CPU time we're not resorting to `clock()` as a default
///     implementation, as depending on the platform it may incorrectly report elapsed wall-clock
///     time (sources on the internet report this for Solaris, and it is a well-documented fact for
///     Windows), and/or may be limited to timespans in the order of an hour (any system with a
///     32-bit `clock_t` and a standard `CLOCKS_PER_SEC` of 1,000,000).
///
class Timer final
{
    public:

        /// Create and start a new timer.
        ///
        Timer() = default;

        /// Destroy the timer.
        ///
        ~Timer() = default;

        /// Report elapsed wall-clock time.
        ///
        /// This method reports the actual time in milliseconds that has elapsed since the timer's
        /// creation or last call to @ref Reset().
        ///
        /// @return     Elapsed real time in milliseconds.
        ///
        inline POV_LONG ElapsedRealTime() const { return mRealTimer.ElapsedTime(); }

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
        inline POV_LONG ElapsedProcessCPUTime() const { return mRealTimer.ElapsedTime(); }

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
        inline POV_LONG ElapsedThreadCPUTime() const { return mRealTimer.ElapsedTime(); }

        /// Reset the timer.
        ///
        inline void Reset() { mRealTimer.Reset(); }

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

        /// Point in time of construction or last reset.
        DefaultRealTimer mRealTimer;
};

#endif // POV_USE_DEFAULT_TIMER

/// @}
///
//##############################################################################

}
// end of namespace pov_base

// Need to include this last because it may require definitions from this file.
#if !POV_USE_DEFAULT_TIMER
#include "syspovtimer.h"
#endif

#endif // POVRAY_BASE_TIMER_H
