//******************************************************************************
///
/// @file base/platformbase.cpp
///
/// Implementations related to the @ref pov_base::PlatformBase class.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/platformbase.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/filesystem.h"
#include "base/povassert.h"
#include "base/stringutilities.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

//******************************************************************************

PlatformBase::PlatformBase()
{
    POV_ASSERT(self == nullptr);
    self = this;
};

PlatformBase::~PlatformBase()
{
    self = nullptr;
};

PlatformBase& PlatformBase::GetInstance()
{
    POV_ASSERT(self != nullptr);
    return *self;
};

/// Platform specific function interface self reference pointer
PlatformBase *PlatformBase::self = nullptr;

//******************************************************************************

DefaultPlatformBase::DefaultPlatformBase()
{
}

DefaultPlatformBase::~DefaultPlatformBase()
{
}

UCS2String DefaultPlatformBase::GetTemporaryPath()
{
    return u"/tmp/";
}

UCS2String DefaultPlatformBase::CreateTemporaryFile()
{
    static int cnt = 0;
    char buffer[32];

    cnt++;
    sprintf(buffer, "/tmp/pov%08x.dat", cnt);

    FILE *f = fopen(buffer, "wb");
    if (f != nullptr)
        fclose(f);

    return ASCIItoUCS2String(buffer);
}

void DefaultPlatformBase::DeleteTemporaryFile(const UCS2String& filename)
{
    (void)pov_base::Filesystem::DeleteFile(filename);
}

bool DefaultPlatformBase::ReadFileFromURL(OStream *, const UCS2String&, const UCS2String&)
{
    return false;
}

FILE* DefaultPlatformBase::OpenLocalFile(const UCS2String& name, const char *mode)
{
    return fopen(UCS2toSysString(name).c_str(), mode);
}

bool DefaultPlatformBase::AllowLocalFileAccess(const UCS2String& name, const unsigned int fileType, bool write)
{
    return true;
}

//******************************************************************************

}
// end of namespace pov_base
