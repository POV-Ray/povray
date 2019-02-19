//******************************************************************************
///
/// @file platform/x86/cpuid.h
///
/// This file contains declarations related to probing the capabilities of the
/// CPU.
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

#ifndef POVRAY_CPUID_H
#define POVRAY_CPUID_H

#include "base/configbase.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <string>

class CPUInfo final
{
public:
    static bool SupportsSSE2();             ///< Test whether CPU and OS support SSE2.
    static bool SupportsAVX();              ///< Test whether CPU and OS support AVX.
    static bool SupportsAVX2();             ///< Test whether CPU and OS support AVX2.
    static bool SupportsFMA3();             ///< Test whether CPU and OS support FMA3.
    static bool SupportsFMA4();             ///< Test whether CPU and OS support FMA4.
    static bool IsIntel();                  ///< Test whether CPU is genuine Intel product.
    static bool IsAMD();                    ///< Test whether CPU is genuine AMD product.
    static bool IsVM();                     ///< Test whether CPU cannot be detected reliably due to running in a VM.
    static std::string GetFeatures();       ///< Query ASCII text string summarizing the features detected.
#if POV_CPUINFO_DEBUG
    static std::string GetDetails();        ///< Query ASCII text string detailing the raw CPUID (and related) information gathered.
#endif
private:
    struct Data;
    static const Data* gpData;
};

#endif // POVRAY_CPUID_H
