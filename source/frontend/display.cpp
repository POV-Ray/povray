//******************************************************************************
///
/// @file frontend/display.cpp
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "frontend/display.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
// POV-Ray header files (POVMS module)
// POV-Ray header files (frontend module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

Display::Display(unsigned int w, unsigned int h) :
    width(w),
    height(h)
{
    // nothing to do
}

Display::~Display()
{
    // nothing to do
}

unsigned int Display::GetWidth()
{
    return width;
}

unsigned int Display::GetHeight()
{
    return height;
}

void Display::DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
    for(unsigned int x = x1; x <= x2; x++)
    {
        DrawPixel(x, y1, colour);
        DrawPixel(x, y2, colour);
    }

    for(unsigned int y = y1; y <= y2; y++)
    {
        DrawPixel(x1, y, colour);
        DrawPixel(x2, y, colour);
    }
}

void Display::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
    for(unsigned int y = y1; y <= y2; y++)
        for(unsigned int x = x1; x <= x2; x++)
            DrawPixel(x, y, colour);
}

void Display::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
{
    for(unsigned int y = y1, i = 0; y <= y2; y++)
        for(unsigned int x = x1; x <= x2; x++, i++)
            DrawPixel(x, y, colour[i]);
}

void Display::Clear()
{
    RGBA8 colour;

    colour.red = colour.green = colour.blue = colour.alpha = 0;

    for(unsigned int y = 0; y < GetHeight(); y++)
        for(unsigned int x = 0; x < GetWidth(); x++)
            DrawPixel(x, y, colour);
}

}
// end of namespace pov_frontend
