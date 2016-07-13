//******************************************************************************
///
/// @file base/platformbase.h
///
/// Declarations related to the @ref pov_base::PlatformBase class.
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

#ifndef POVRAY_BASE_PLATFORMBASE_H
#define POVRAY_BASE_PLATFORMBASE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// Standard C++ header files
#include <string>

// POV-Ray base header files
#include "base/fileinputoutput.h"
#include "base/stringutilities.h"
#include "base/types.h"

namespace pov_base
{

#define POV_PLATFORM_BASE PlatformBase::GetPlatformBaseReference()

/// Abstract class defining an interface to platform-specific services.
class PlatformBase
{
    public:
        PlatformBase() { self = this; };
        virtual ~PlatformBase() { self = NULL; };

        virtual UCS2String GetTemporaryPath() = 0;
        virtual UCS2String CreateTemporaryFile() = 0;
        virtual void DeleteTemporaryFile(const UCS2String& filename) = 0;

        virtual bool ReadFileFromURL(OStream *file, const UCS2String& url, const UCS2String& referrer = UCS2String()) = 0;

        static PlatformBase& GetPlatformBaseReference() { return *self; };
    private:
        static PlatformBase *self;
};

}

#endif // POVRAY_BASE_PLATFORMBASE_H
