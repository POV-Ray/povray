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

#include <algorithm>
#include <atomic>
#include <cstring>
#include <limits>

#include "base/filesystem.h"
#include "base/pov_err.h"
#include "base/stringutilities.h"

// this must be the last file included
#include "syspovdebug.h"

////////////////////////////////////////////////////////////////////////////////////////
//
// class vfeDisplay
//
////////////////////////////////////////////////////////////////////////////////////////

namespace vfe
{

using pov_base::Filesystem::MappedFile;

struct vfeDisplayBufferHeader
{
  std::uint32_t Version;
  std::uint32_t Width;
  std::uint32_t Height;
  std::uint32_t PixelsWritten;
  std::uint32_t PixelsRead;
};

static_assert(sizeof(vfeDisplayBufferHeader) == 20, "shared display buffer header must be 20 bytes");
static_assert(sizeof(Display::RGBA8) == 4, "shared display buffer pixels must be 4 bytes");

// Another process may read the header at any time, so make sure every store
// to it is actually performed. The release fences placed before such stores
// order them after the pixel stores; strictly, the C++ memory model pairs a
// fence only with atomic stores, so on weakly ordered hardware (ARM) the
// ordering seen by the other process rests on the fence instruction itself.
static inline void StoreHeaderField(std::uint32_t& field, std::uint32_t value)
{
  *static_cast<volatile std::uint32_t*>(&field) = value;
}

// Clip a rectangle to the display; false if nothing is left.
static inline bool ClipRectangle(unsigned int width, unsigned int height, unsigned int x1, unsigned int y1, unsigned int& x2, unsigned int& y2)
{
  if (x1 >= width || y1 >= height || x2 < x1 || y2 < y1)
    return false;
  x2 = std::min(x2, width - 1);
  y2 = std::min(y2, height - 1);
  return true;
}

vfeDisplay::vfeDisplay(unsigned int w, unsigned int h, vfeSession* session, bool visible) :
  Display(w, h),
  m_Session(session),
  m_VisibleOnCreation(visible),
  m_SharedPixelsWritten(0)
{
}

vfeDisplay::~vfeDisplay()
{
}

void vfeDisplay::Initialise()
{
  InitialiseSharedBuffer();
  // With a shared buffer the pixels live there; otherwise keep them in memory.
  m_Pixels.clear();
  if (m_SharedBuffer == nullptr)
    m_Pixels.resize(size_t(GetWidth()) * GetHeight());
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
  if (m_SharedBuffer != nullptr)
  {
    SharedBufferDrawPixel(x, y, colour);
    return;
  }
  const size_t i = size_t(y) * GetWidth() + x;
  if (x < GetWidth() && i < m_Pixels.size())
    m_Pixels[i] = colour;
}

void vfeDisplay::DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
}

void vfeDisplay::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
  SharedBufferDrawFilledRectangle(x1, y1, x2, y2, colour);
}

void vfeDisplay::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
{
  for (int y = y1 ; y <= y2; y++)
    for (int x = x1; x <= x2; x++)
      DrawPixel (x, y, *colour++) ;
}

void vfeDisplay::Clear()
{
  SharedBufferClear();
  m_Pixels.clear();
}

//////////////////////////////////////////////////////////////////////////////
// Shared display buffer (`+SM`)

void vfeDisplay::InitialiseSharedBuffer()
{
  // Release anything left from an earlier call first.
  CloseSharedBuffer();

  UCS2String fileName = m_Session->GetOptions().GetOptions().TryGetUCS2String(kPOVAttrib_SharedMemory, "");
  if (fileName.empty())
    return;

  // Do the size arithmetic in 64 bits: width * height * 4 overflows 32 bits
  // for images above 1 gigapixel. A size that does not fit the address space
  // of this build is refused by MappedFile::Open() rather than wrapped.
  const std::uint_least64_t pixelCount = std::uint_least64_t(GetWidth()) * GetHeight();
  if (pixelCount > (std::numeric_limits<std::uint_least64_t>::max() - sizeof(vfeDisplayBufferHeader)) / sizeof(RGBA8))
    throw POV_EXCEPTION(kOutOfMemoryErr, "Shared display buffer size out of range");
  const std::uint_least64_t size = pixelCount * sizeof(RGBA8) + sizeof(vfeDisplayBufferHeader);

  // VirtualFrontEnd::Start() has checked the name against the I/O restrictions
  // already, but check again right before the file is opened, as is done for
  // the output image, since the file system may have changed while parsing.
  if (!m_Session->TestAccessAllowed(fileName, true))
    throw POV_EXCEPTION(kCannotOpenFileErr, "IO Restrictions prohibit write access to '" + UCS2toSysString(fileName) + "'");

  std::unique_ptr<MappedFile> file(new MappedFile);
  if (!file->Open(fileName, size))
    throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot use '" + UCS2toSysString(fileName) + "' as shared display buffer: " + file->GetErrorText());
  m_SharedBuffer = std::move(file);

  // A version of 0 tells a reader that the header is being (re)written.
  vfeDisplayBufferHeader* header = static_cast<vfeDisplayBufferHeader*>(m_SharedBuffer->GetData());
  StoreHeaderField(header->Version, 0);
  std::atomic_thread_fence(std::memory_order_release);
  StoreHeaderField(header->Width, GetWidth());
  StoreHeaderField(header->Height, GetHeight());
  StoreHeaderField(header->PixelsWritten, 0);
  StoreHeaderField(header->PixelsRead, 0);
  // The file may hold an earlier image; start from transparent black.
  std::memset(header + 1, 0, size_t(size - sizeof(vfeDisplayBufferHeader)));
  m_SharedPixelsWritten = 0;
  std::atomic_thread_fence(std::memory_order_release);
  StoreHeaderField(header->Version, 1);
}

void vfeDisplay::CloseSharedBuffer()
{
  // Unmapping keeps the file and its content for any reader.
  m_SharedBuffer.reset();
}

void vfeDisplay::SharedBufferAdvance(std::uint_least64_t count)
{
  // The counter wraps modulo 2^32, as it always has.
  m_SharedPixelsWritten = std::uint32_t(m_SharedPixelsWritten + count);
  // Publish the pixels before the count that tells a reader about them.
  std::atomic_thread_fence(std::memory_order_release);
  vfeDisplayBufferHeader* header = static_cast<vfeDisplayBufferHeader*>(m_SharedBuffer->GetData());
  StoreHeaderField(header->PixelsWritten, m_SharedPixelsWritten);
}

void vfeDisplay::SharedBufferDrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
{
  if (m_SharedBuffer == nullptr || x >= GetWidth() || y >= GetHeight())
    return;
  RGBA8* pixels = reinterpret_cast<RGBA8*>(static_cast<vfeDisplayBufferHeader*>(m_SharedBuffer->GetData()) + 1);
  pixels[size_t(y) * GetWidth() + x] = colour;
  SharedBufferAdvance(1);
}

void vfeDisplay::SharedBufferDrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
{
  if (m_SharedBuffer == nullptr || x2 < x1)
    return;
  const size_t blockWidth = size_t(x2 - x1) + 1;
  unsigned int cx2 = x2, cy2 = y2;
  if (!ClipRectangle(GetWidth(), GetHeight(), x1, y1, cx2, cy2))
    return;
  RGBA8* pixels = reinterpret_cast<RGBA8*>(static_cast<vfeDisplayBufferHeader*>(m_SharedBuffer->GetData()) + 1);
  const size_t rowLength = size_t(cx2 - x1) + 1;
  for (unsigned int y = y1; y <= cy2; y++)
    std::memcpy(&pixels[size_t(y) * GetWidth() + x1], &colour[size_t(y - y1) * blockWidth], rowLength * sizeof(RGBA8));
  SharedBufferAdvance(std::uint_least64_t(rowLength) * (cy2 - y1 + 1));
}

void vfeDisplay::SharedBufferDrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
  if (m_SharedBuffer == nullptr || !ClipRectangle(GetWidth(), GetHeight(), x1, y1, x2, y2))
    return;
  RGBA8* pixels = reinterpret_cast<RGBA8*>(static_cast<vfeDisplayBufferHeader*>(m_SharedBuffer->GetData()) + 1);
  for (unsigned int y = y1; y <= y2; y++)
    std::fill_n(&pixels[size_t(y) * GetWidth() + x1], size_t(x2 - x1) + 1, colour);
  // Rectangles are previews (mosaic preview, radiosity pretrace) that the final
  // pixels will overwrite, so they do not count: PixelsWritten reaches
  // Width * Height when the frame is complete, which is what readers test for.
}

void vfeDisplay::SharedBufferClear()
{
  if (m_SharedBuffer == nullptr)
    return;
  RGBA8* pixels = reinterpret_cast<RGBA8*>(static_cast<vfeDisplayBufferHeader*>(m_SharedBuffer->GetData()) + 1);
  const std::uint_least64_t pixelCount = std::uint_least64_t(GetWidth()) * GetHeight();
  std::memset(pixels, 0, size_t(pixelCount) * sizeof(RGBA8));
  // Clearing stores no rendered pixels, so PixelsWritten does not change.
}

}
// end of namespace vfe
