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
// to it is actually performed.
static inline void StoreHeaderField(std::uint32_t& field, std::uint32_t value)
{
  *static_cast<volatile std::uint32_t*>(&field) = value;
}

vfeDisplay::vfeDisplay(unsigned int w, unsigned int h, vfeSession* session, bool visible) :
  Display(w, h),
  m_Session(session),
  m_Buffer(nullptr),
  m_VisibleOnCreation(visible),
  m_PixelsWritten(0)
{
}

vfeDisplay::~vfeDisplay()
{
}

void vfeDisplay::Initialise()
{
  // Release anything left from an earlier call first.
  Clear();

  UCS2String SharedMemoryName = m_Session->GetOptions().GetOptions().TryGetUCS2String(kPOVAttrib_SharedMemory, "");

  // Do the size arithmetic in 64 bits: width * height * 4 overflows 32 bits
  // for images above 1 gigapixel. A size that does not fit the address space
  // of this build is refused below rather than wrapped.
  const std::uint_least64_t PixelCount = std::uint_least64_t(GetWidth()) * GetHeight();
  if (PixelCount > (std::numeric_limits<std::uint_least64_t>::max() - sizeof(vfeDisplayBufferHeader)) / sizeof(RGBA8))
    throw POV_EXCEPTION(kOutOfMemoryErr, "Render preview buffer size out of range");
  const std::uint_least64_t AllocSize = PixelCount * sizeof(RGBA8) + sizeof(vfeDisplayBufferHeader);

  if (SharedMemoryName.empty())
  {
    if (AllocSize > std::numeric_limits<std::size_t>::max())
      throw POV_EXCEPTION(kOutOfMemoryErr, "Render preview buffer too large for this build");
    m_HeapBuffer.assign(std::size_t(AllocSize), 0);
    m_Buffer = m_HeapBuffer.data();
  }
  else
  {
    std::unique_ptr<MappedFile> SharedBuffer(new MappedFile);
    if (!SharedBuffer->Open(SharedMemoryName, AllocSize))
      throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot use '" + UCS2toSysString(SharedMemoryName) + "' as shared display buffer: " + SharedBuffer->GetErrorText());
    m_SharedBuffer = std::move(SharedBuffer);
    m_Buffer = m_SharedBuffer->GetData();
  }

  // A version of 0 tells a reader that the header is being (re)written.
  vfeDisplayBufferHeader* Header = static_cast<vfeDisplayBufferHeader*>(m_Buffer);
  StoreHeaderField(Header->Version, 0);
  std::atomic_thread_fence(std::memory_order_release);
  StoreHeaderField(Header->Width, GetWidth());
  StoreHeaderField(Header->Height, GetHeight());
  StoreHeaderField(Header->PixelsWritten, 0);
  StoreHeaderField(Header->PixelsRead, 0);
  // The file may hold an earlier image; start from transparent black.
  std::memset(Header + 1, 0, std::size_t(AllocSize - sizeof(vfeDisplayBufferHeader)));
  m_PixelsWritten = 0;
  std::atomic_thread_fence(std::memory_order_release);
  StoreHeaderField(Header->Version, 1);
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
  if (m_Buffer == nullptr || x >= GetWidth() || y >= GetHeight())
    return;
  vfeDisplayBufferHeader* Header = static_cast<vfeDisplayBufferHeader*>(m_Buffer);
  RGBA8* Pixels = reinterpret_cast<RGBA8*>(Header + 1);
  Pixels[std::size_t(y) * GetWidth() + x] = colour;
  // Publish the pixel before the count that tells a reader about it.
  std::atomic_thread_fence(std::memory_order_release);
  StoreHeaderField(Header->PixelsWritten, ++m_PixelsWritten);
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
  // Unmapping keeps the file and its content for any reader.
  m_SharedBuffer.reset();
  m_HeapBuffer.clear();
  m_HeapBuffer.shrink_to_fit();
  m_Buffer = nullptr;
}

}
// end of namespace vfe
