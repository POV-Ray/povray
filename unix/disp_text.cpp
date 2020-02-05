//******************************************************************************
///
/// @file unix/disp_text.cpp
///
/// Template for a text mode render display system.
///
/// @author Christoph Hormann <chris_hormann@gmx.de>
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
//*******************************************************************************

#include "disp_text.h"

// this must be the last file included
#include "syspovdebug.h"


namespace pov_frontend
{
    using namespace vfe;
    using namespace vfePlatform;

    const UnixOptionsProcessor::Option_Info UnixTextDisplay::Options[] =
    {
        // command line/povray.conf/environment options of this display mode can be added here
        // section name, option name, default, has_param, command line parameter, environment variable name, help text
        UnixOptionsProcessor::Option_Info("", "", "", false, "", "", "") // has to be last
    };

    bool UnixTextDisplay::Register(vfeUnixSession *session)
    {
        session->GetUnixOptions()->Register(Options);
        // text mode display should always work
        return true;
    }

    void UnixTextDisplay::DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
    {
        //fprintf(stderr, "DrawPixel(%d,%d)\n", x, y);
    }
}
// end of namespace pov_frontend
