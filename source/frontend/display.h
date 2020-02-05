//******************************************************************************
///
/// @file frontend/display.h
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

#ifndef POVRAY_FRONTEND_DISPLAY_H
#define POVRAY_FRONTEND_DISPLAY_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "frontend/configfrontend.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
// POV-Ray header files (POVMS module)
// POV-Ray header files (frontend module)
//  (none at the moment)

namespace pov_frontend
{

class Display
{
    public:
        struct RGBA8 final
        {
            unsigned char red, green, blue, alpha;
        };

        Display(unsigned int w, unsigned int h);
        virtual ~Display();

        virtual void Initialise() = 0;

        unsigned int GetWidth();
        unsigned int GetHeight();

        virtual void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour) = 0;

        virtual void DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
        virtual void DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);

        virtual void DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour);

        virtual void Clear();
    private:
        /// display width
        unsigned int width;
        /// display height
        unsigned int height;

        Display() = delete;
};

}
// end of namespace pov_frontend

#endif // POVRAY_FRONTEND_DISPLAY_H
