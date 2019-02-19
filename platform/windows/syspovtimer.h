//******************************************************************************
///
/// @file platform/windows/syspovtimer.h
///
/// Windows-specific declaration of the @ref pov_base::Delay() function and
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

#ifndef POVRAY_WINDOWS_SYSPOVTIMER_H
#define POVRAY_WINDOWS_SYSPOVTIMER_H

#include "base/configbase.h"

namespace pov_base
{

#if !POV_USE_DEFAULT_TIMER

class Timer final
{
    public:
        Timer();
        ~Timer();

        POV_LONG ElapsedRealTime() const;
        POV_LONG ElapsedThreadCPUTime() const;
        POV_LONG ElapsedProcessCPUTime() const;

        void Reset();

        bool HasValidThreadCPUTime() const;
        bool HasValidProcessCPUTime() const;

    private:

        POV_ULONG   mWallTimeStart;
        POV_ULONG   mThreadTimeStart;
        POV_ULONG   mProcessTimeStart;
        void*       mThreadHandle;
        bool        mCPUTimeSupported   : 1;

        POV_ULONG GetWallTime () const;
        POV_ULONG GetThreadTime () const;
        POV_ULONG GetProcessTime () const;
};

#endif // POV_USE_DEFAULT_TIMER

}
// end of namespace pov_base

#endif // POVRAY_WINDOWS_SYSPOVTIMER_H
