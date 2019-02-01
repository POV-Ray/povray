//******************************************************************************
///
/// @file frontend/console.h
///
/// @todo   What's in here?
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

#ifndef POVRAY_FRONTEND_CONSOLE_H
#define POVRAY_FRONTEND_CONSOLE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "frontend/configfrontend.h"
#include "frontend/console_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <string>

// POV-Ray header files (base module)
#include "base/textstreambuffer.h"

// POV-Ray header files (POVMS module)
// POV-Ray header files (frontend module)
//  (none at the moment)

namespace pov_frontend
{

class Console : public pov_base::TextStreamBuffer
{
    public:
        Console(unsigned int wrapwidth = 80);
        virtual ~Console() override;

        virtual void Initialise() = 0;
        virtual void Output(const std::string&) = 0;
    private:
        virtual void lineoutput(const char *str, unsigned int chars) override;
};

}
// end of namespace pov_frontend

#endif // POVRAY_FRONTEND_CONSOLE_H
