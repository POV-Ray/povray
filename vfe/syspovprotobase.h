//******************************************************************************
///
/// @file vfe/syspovprotobase.h
///
/// Provides definitions that are used by both the windows and core code.
///
/// @author Christopher J. Cason
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef __SYSPROTO_H__
#define __SYSPROTO_H__

#include <cstddef>
#include <vector>
// FIXME #include <xmemory>

#include "povms/povmscpp.h"

namespace pov_base
{

void vfeSysThreadStartup();
void vfeSysThreadCleanup();
bool vfeParsePathString (const POVMSUCS2String& path, POVMSUCS2String& volume, vector<POVMSUCS2String>& components, POVMSUCS2String& filename);

class vfeTimer
{
    public:
        vfeTimer(bool CPUTimeIsThreadOnly = false);
        ~vfeTimer();

        POV_LONG ElapsedRealTime(void) const ;
        POV_LONG ElapsedCPUTime(void) const;
        void Reset(void);
        bool HasValidCPUTime() const;

    private:
        POV_ULONG GetWallTime (void) const ;
        POV_ULONG GetCPUTime (void) const ;

        POV_ULONG m_WallTimeStart ;
        POV_ULONG m_CPUTimeStart ;
        bool m_ThreadTimeOnly ;
        void *m_ThreadHandle ;
        bool m_IsNT ;
};

}

#endif // __SYSPROTO_H__
