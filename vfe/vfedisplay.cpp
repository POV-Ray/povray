//******************************************************************************
///
/// @file vfe/vfedisplay.cpp
///
/// This module contains a basic in-memory implementation of the Display class.
///
/// @author: Christopher J. Cason
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

#include "vfe.h"

// this must be the last file included
#include "syspovdebug.h"

////////////////////////////////////////////////////////////////////////////////////////
//
// class vfeDisplay
//
////////////////////////////////////////////////////////////////////////////////////////

namespace vfe
{

vfeDisplay::vfeDisplay(unsigned int w, unsigned int h, vfeSession* session, bool visible) :
  Display(w, h),
  m_Session(session),
  m_VisibleOnCreation(visible)
{
}

vfeDisplay::~vfeDisplay()
{
}

void vfeDisplay::Initialise()
{
  m_Pixels.clear();
  m_Pixels.resize(GetWidth() * GetHeight());
}

void vfeDisplay::Close()
{
}

void vfeDisplay::Show()
{
}

void vfeDisplay::Hide()
{
}

void vfeDisplay::DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
{
  assert (x < GetWidth() && y < GetHeight());
  m_Pixels[y * GetHeight() + x] = colour;
}

void vfeDisplay::DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
}

void vfeDisplay::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
}

void vfeDisplay::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
{
  for (int y = y1 ; y <= y2; y++)
    for (int x = x1; x <= x2; x++)
      DrawPixel (x, y, *colour++) ;
}

void vfeDisplay::Clear()
{
  m_Pixels.clear();
}

}
// end of namespace vfe
