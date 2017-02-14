//******************************************************************************
///
/// @file base/timer.h
///
/// Contains declarations of the @ref pov_base::Delay() function and the
/// @ref pov_base::Timer class.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
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

#if POV_USE_DEFAULT_TIMER || (POV_MULTITHREADED && POV_USE_DEFAULT_DELAY)
#include <boost/chrono.hpp>
#include <boost/chrono/process_cpu_clocks.hpp>
#include <boost/chrono/thread_clock.hpp>
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
///     This is a general implementation based on boost::thread + boost::chrono.  It should probably
///     be fine for most platforms.
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
///     This is a general implementation based on boost::chrono.  It should probably be fine for
///     most platforms.
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
        POV_LONG ElapsedProcessCPUTime() const;

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
        POV_LONG ElapsedThreadCPUTime() const;

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
        bool HasValidProcessCPUTime() const;

        /// Report whether per-thread measurement of CPU time is supported.
        ///
        /// This method reports whether the timer does indeed support per-thread measurement of CPU
        /// time consumption, or falls back to some other time measurement instead.
        ///
        /// @return     `true` if @ref ElapsedThreadCPUTime() does indeed report per-thread CPU time
        ///             consumption.
        ///
        bool HasValidThreadCPUTime() const;

    private:

        /// real time at last reset
#ifdef BOOST_CHRONO_HAS_CLOCK_STEADY
        boost::chrono::steady_clock::time_point mRealTimeStart;
#else
        boost::chrono::system_clock::time_point mRealTimeStart;
#endif

#ifdef BOOST_CHRONO_HAS_PROCESS_CLOCKS
        /// process CPU time at last reset
        boost::chrono::process_real_cpu_clock::time_point mProcessTimeStart;
#endif

#ifdef BOOST_CHRONO_HAS_THREAD_CLOCK
        /// thread time at last reset
        boost::chrono::thread_clock::time_point mThreadTimeStart;
#endif

};

#endif // POV_USE_DEFAULT_TIMER

/// @}
///
//##############################################################################

}

#endif // POVRAY_BASE_TIMER_H
