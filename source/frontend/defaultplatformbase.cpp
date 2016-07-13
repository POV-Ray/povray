//******************************************************************************
///
/// @file frontend/defaultplatformbase.cpp
///
/// This module contains the C++ DefaultPlatformBase class.
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

// configfrontend.h must always be the first POV file included within frontend *.cpp files
#include "frontend/configfrontend.h"
#include "frontend/defaultplatformbase.h"

#include "povms/povmscpp.h"

#include "base/pov_err.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

DefaultPlatformBase::DefaultPlatformBase()
{
}

DefaultPlatformBase::~DefaultPlatformBase()
{
}

UCS2String DefaultPlatformBase::GetTemporaryPath()
{
    return ASCIItoUCS2String("/tmp/");
}

UCS2String DefaultPlatformBase::CreateTemporaryFile()
{
    static int cnt = 0;
    char buffer[32];

    cnt++;
    sprintf(buffer, "/tmp/pov%08x.dat", cnt);

    FILE *f = fopen(buffer, "wb");
    if(f != NULL)
        fclose(f);

    return UCS2String(ASCIItoUCS2String(buffer));
}

void DefaultPlatformBase::DeleteTemporaryFile(const UCS2String& filename)
{
    remove(UCS2toASCIIString(filename).c_str());
}

bool DefaultPlatformBase::ReadFileFromURL(OStream *, const UCS2String&, const UCS2String&)
{
    return false;
}


}
