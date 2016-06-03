//******************************************************************************
///
/// @file base/timer.cpp
///
/// @todo   What's in here?
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

#if POV_TIMER_DEFAULT

TimerDefault::TimerDefault(bool tctime) :
    threadCPUTimeOnly(tctime)
{
    Reset();
}

TimerDefault::~TimerDefault()
{
}

POV_LONG TimerDefault::ElapsedRealTime() const
{
    boost::xtime t;
    boost::xtime_get(&t, POV_TIME_UTC);
    POV_LONG tt = (POV_LONG)(t.sec) * (POV_LONG)(1000000000) + (POV_LONG)(t.nsec);
    POV_LONG st = (POV_LONG)(realTimeStart.sec) * (POV_LONG)(1000000000) + (POV_LONG)(realTimeStart.nsec);
    return ((tt - st) / (POV_LONG)(1000000));
}

POV_LONG TimerDefault::ElapsedCPUTime() const
{
    boost::xtime t;
    boost::xtime_get(&t, POV_TIME_UTC);
    POV_LONG tt = (POV_LONG)(t.sec) * (POV_LONG)(1000000000) + (POV_LONG)(t.nsec);
    POV_LONG st = (POV_LONG)(cpuTimeStart.sec) * (POV_LONG)(1000000000) + (POV_LONG)(cpuTimeStart.nsec);
    return ((tt - st) / (POV_LONG)(1000000));
}

bool TimerDefault::HasValidCPUTime() const
{
    return false;
}

void TimerDefault::Reset()
{
    boost::xtime_get(&realTimeStart, POV_TIME_UTC);
    boost::xtime_get(&cpuTimeStart, POV_TIME_UTC);
}

#endif // POV_TIMER_DEFAULT

#if POV_MULTITHREADED && !defined(POV_DELAY_IMPLEMENTED)

void Delay(unsigned int msec)
{
    boost::xtime t;
    boost::xtime_get(&t, POV_TIME_UTC);
    POV_ULONG ns = (POV_ULONG)(t.sec) * (POV_ULONG)(1000000000) + (POV_ULONG)(t.nsec) + (POV_ULONG)(msec) * (POV_ULONG)(1000000);
    t.sec = (boost::xtime::xtime_sec_t)(ns / (POV_ULONG)(1000000000));
    t.nsec = (boost::xtime::xtime_nsec_t)(ns % (POV_ULONG)(1000000000));
    boost::thread::sleep(t);
}

#endif // POV_MULTITHREADED && !defined(POV_DELAY_IMPLEMENTED)

}
