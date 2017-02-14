//******************************************************************************
///
/// @file base/timer.cpp
///
/// Contains implementations of the @ref pov_base::Delay() function and the
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/timer.h"

// Boost header files
#if POV_MULTITHREADED
#include <boost/thread.hpp>
#endif

// POV-Ray base header files
#include "base/types.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

#if POV_MULTITHREADED && POV_USE_DEFAULT_DELAY

void Delay(unsigned int msec)
{
    boost::this_thread::sleep_for(boost::chrono::milliseconds(msec));
}

#endif // POV_MULTITHREADED && POV_USE_DEFAULT_DELAY

#if POV_USE_DEFAULT_TIMER

Timer::Timer()
{
    Reset();
}

Timer::~Timer()
{
}

POV_LONG Timer::ElapsedRealTime() const
{
#ifdef BOOST_CHRONO_HAS_CLOCK_STEADY
    return static_cast<POV_LONG>(boost::chrono::duration_cast<boost::chrono::milliseconds>
                                 (boost::chrono::steady_clock::now() - mRealTimeStart).count());
#else
    return static_cast<POV_LONG>(boost::chrono::duration_cast<boost::chrono::milliseconds>
                                 (boost::chrono::system_clock::now() - mRealTimeStart).count());
#endif
}

POV_LONG Timer::ElapsedProcessCPUTime() const
{
#ifdef BOOST_CHRONO_HAS_PROCESS_CLOCKS
    return static_cast<POV_LONG>(boost::chrono::duration_cast<boost::chrono::milliseconds>
                                 (boost::chrono::process_real_cpu_clock::now() - mProcessTimeStart).count());
#else
    return ElapsedRealTime();
#endif
}

POV_LONG Timer::ElapsedThreadCPUTime() const
{
#ifdef BOOST_CHRONO_HAS_THREAD_CLOCK
    return static_cast<POV_LONG>(boost::chrono::duration_cast<boost::chrono::milliseconds>
                                 (boost::chrono::thread_clock::now() - mThreadTimeStart).count());
#else
    return ElapsedProcessCPUTime();
#endif
}


bool Timer::HasValidProcessCPUTime() const
{
#ifdef BOOST_CHRONO_HAS_PROCESS_CLOCKS
    return true;
#else
    return false;
#endif
}

bool Timer::HasValidThreadCPUTime() const
{
#ifdef BOOST_CHRONO_HAS_THREAD_CLOCK
    return true;
#else
    return false;
#endif
}

void Timer::Reset()
{
#ifdef BOOST_CHRONO_HAS_CLOCK_STEADY
    mRealTimeStart = boost::chrono::steady_clock::now();
#else
    mRealTimeStart = boost::chrono::system_clock::now();
#endif

#ifdef BOOST_CHRONO_HAS_PROCESS_CLOCKS
    mProcessTimeStart = boost::chrono::process_real_cpu_clock::now();
#endif

#ifdef BOOST_CHRONO_HAS_THREAD_CLOCK
    mThreadTimeStart = boost::chrono::thread_clock::now();
#endif
}

#endif // POV_USE_DEFAULT_TIMER

}
