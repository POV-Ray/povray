//******************************************************************************
///
/// @file windows/pvmem.cpp
///
/// This module implements windows-specific memory handling routines.
///
/// @author Christopher J. Cason
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#include <windows.h>
#include <crtdbg.h>
#include <errno.h>
#include <assert.h>

#include "pvengine.h"

#ifdef _DEBUG
  #define EXTRA_VALIDATION        1
#else
  #define EXTRA_VALIDATION        0
#endif

#include "pvmem.h"

// this must be the last file included
#include "syspovdebug.h"

extern "C"
{
  int __cdecl WinMainCRTStartup();
}

#undef new
#undef delete

using namespace pov_base;

namespace povwin
{

bool          TrackMem;
__int64       PeakMem;

static WinHeap *gHeap = NULL;
WinHeap *WinMemBlock::m_Heap = NULL;
static WinMemStats gMemStats;
__declspec(thread) static WinMemStats *lMemStats = NULL;

#if POV_MEM_STATS == 0 && WIN_MEM_TRACKING == 0

int __cdecl POVWINStartup()
{
  return WinMainCRTStartup();
}

void *win_malloc(size_t size)
{
  void *result = malloc(size);
  if (result == NULL)
    throw std::bad_alloc();
  return result;
}

void *win_malloc(size_t size, const void *data, int line)
{
  void *result = malloc(size);
  if (result == NULL)
    throw std::bad_alloc();
  return result;
}

void *win_realloc(void *p, size_t size)
{
  void *result = realloc(p, size);
  if (result == NULL)
    throw std::bad_alloc();
  return result;
}

void *win_realloc(void *p, size_t size, const void *data, int line)
{
  void *result = realloc(p, size);
  if (result == NULL)
    throw std::bad_alloc();
  return result;
}

void win_free(void *p)
{
  free(p);
}

void win_free(void *p, const void *data, int line)
{
  free(p);
}

char *win_strdup(const char *s)
{
  char *result = _strdup(s);
  if (result == NULL)
    throw std::bad_alloc();
  return result;
}

char *win_strdup(const char *s, const void *data, int line)
{
  char *result = _strdup(s);
  if (result == NULL)
    throw std::bad_alloc();
  return result;
}

void WinMemStage(bool BeginRender, void *cookie)
{
  if (BeginRender)
  {
    PeakMem = 0;
    TrackMem = true;
  }
  else
    TrackMem = false;
}

bool WinMemReport(bool global, uint64& allocs, uint64& frees, int64& current, uint64& peak, uint64 &smallest, uint64 &largest)
{
  if (!global)
    return false;
  allocs = frees = current = smallest = largest = 0;
  peak = PeakMem;
  return true;
}

void WinMemThreadStartup()
{
}

void WinMemThreadCleanup()
{
}

#else

// This function is the first called when POVWIN starts (even before the CRT startup code).
// Therefore we can't count on very much being set up for us.
int __cdecl POVWINStartup()
{
  // note that WinHeap has its own operator new.
  gHeap = new WinHeap;
  if (gHeap == NULL || gHeap->Validate() == false)
  {
    // not safe to call MessageBox here due to some other threads getting fired up
    // MessageBox(GetDesktopWindow(), "Failed to set up private heap - POV-Ray will now exit", "POV-Ray for Windows", MB_OK | MB_ICONERROR);
    return 1;
  }
  WinMemBlock::SetAllocator(gHeap);

  // we never delete gHeap since WinMainCRTStartup() often exits via means other than a return.
  return WinMainCRTStartup();
}

void WinMemThreadStartup()
{
  lMemStats = new WinMemStats();
}

void WinMemThreadCleanup()
{
  delete lMemStats;
}

bool WinMemReport(bool global, uint64& allocs, uint64& frees, int64& current, uint64& peak, uint64 &smallest, uint64 &largest)
{
  HeapLock lock(gHeap);

  allocs = frees = current = peak = smallest = largest = 0;
  if (global)
  {
    gMemStats.Report(allocs, frees, current, peak, smallest, largest);
    return true;
  }
  if (lMemStats == NULL)
    return false;
  lMemStats->Report(allocs, frees, current, peak, smallest, largest);
  return true;
}

void WinMemStage(bool BeginRender, void *cookie)
{
  HeapLock lock(gHeap);

  if (BeginRender)
    gMemStats.Clear();
}

#ifdef _DEBUG

void *win_malloc(size_t size, const void *data, int line)
{
  HeapLock lock(gHeap);

  // note that WinMemBlock has its own operator new
  WinMemBlock *block = new (size) WinMemBlock(data, line);
  if (block == NULL)
    throw std::bad_alloc();
  gMemStats.RecordAlloc(block->Size(size));
  return block->GetMem();
}

void *win_realloc(void *p, size_t newSize, const void *data, int line)
{
  if (p == NULL)
    return win_malloc(newSize, data, line);

  if (newSize != 0)
  {
    HeapLock lock(gHeap);
    size_t oldSize;
    WinMemBlock *block = WinMemBlock::GetBlock(p);
    if ((block == NULL) || ((oldSize = block->Size()) == (size_t) -1))
      throw POV_EXCEPTION(kCannotHandleDataErr, "Bad memory block in realloc()");
    if ((block = block->Realloc(newSize, data, line)) == NULL)
      throw std::bad_alloc();
    gMemStats.RecordFree(oldSize);
    gMemStats.RecordAlloc(block->Size(newSize));
    return block->GetMem();
  }
  else
  {
    win_free(p, data, line);
    return NULL;
  }
}

void win_free(void *p, const void *data, int line)
{
  if (p == NULL)
    return;
  HeapLock lock(gHeap);
  size_t size;
  WinMemBlock *block = WinMemBlock::GetBlock(p);
  if ((block == NULL) || ((size = block->Size()) == (size_t) -1))
    throw POV_EXCEPTION(kCannotHandleDataErr, "Bad memory block in free()");
  gMemStats.RecordFree(size);
  // note that WinMemBlock has a private delete operator
  delete block;
}

char *win_strdup(const char *s, const void *data, int line)
{
  char *p = (char *) win_malloc(strlen(s) + 1, data, line);
  strcpy(p, s);
  return p;
}

#else // _DEBUG

// release-mode functions

void *win_malloc(size_t size)
{
  HeapLock    lock(gHeap);

  void *p = gHeap->Alloc(size);
  if (p == NULL)
    throw std::bad_alloc();
  return p;
}

void *win_realloc(void *p, size_t size)
{
  if (p == NULL)
    return win_malloc(size);
  if (size == 0)
  {
    win_free(p);
    return NULL;
  }
  HeapLock lock(gHeap);
  size_t currentSize = gHeap->BlockSize(p);
  if (currentSize == (size_t) -1)
    throw POV_EXCEPTION(kCannotHandleDataErr, "Bad memory block in realloc()");
  void *newBlock = gHeap->ReAlloc(p, size);
  if (newBlock == NULL)
  {
    // this can happen with the low-fragmentation heap, even if memory is available
    // as long as it's a shrink, we return the original pointer
    if (size < currentSize)
      return p;
    throw std::bad_alloc();
  }
  gMemStats.RecordFree(currentSize);
  gMemStats.RecordAlloc(size);
  return newBlock;
}

void win_free(void *p)
{
  if (p == NULL)
    return;
  HeapLock lock(gHeap);
  size_t len = gHeap->BlockSize(p);
  if (len == (size_t) -1)
    throw POV_EXCEPTION(kCannotHandleDataErr, "Bad memory block in free()");
  gMemStats.RecordFree(len);
  if (!gHeap->Free(p))
    throw POV_EXCEPTION(kCannotHandleDataErr, "Bad memory block in free()");
}

char *win_strdup(const char *s)
{
  size_t len = strlen(s) + 1;
  char *p = (char *) win_malloc(len);
  memcpy(p, s, len);
  return p;
}

#endif // else portion of '#ifdef _DEBUG'

#endif // else portion of '#if POV_MEM_STATS == 0 && WIN_MEM_TRACKING == 0'
} // end of namespace povwin

#if WIN_MEM_TRACKING

using namespace povwin;

#ifdef _DEBUG
  #define DEBUGPARAMS   , data, line
#else
  #define DEBUGPARAMS
#endif

void *operator new (size_t size, const char *func, const void *data, int line)
{
  void *res;

  if ((res = win_malloc(size DEBUGPARAMS)) != NULL)
    return res;
  throw std::bad_alloc () ;
}

void *operator new[] (size_t size, const char *func, const void *data, int line)
{
  void *res;

  if ((res = win_malloc(size DEBUGPARAMS)) != NULL)
    return res;
  throw std::bad_alloc () ;
}

void operator delete (void *p, const char *func, const void *data, int line)
{
  win_free(p DEBUGPARAMS);
}

void operator delete[] (void *p, const char *func, const void *data, int line)
{
  win_free(p DEBUGPARAMS);
}

#ifdef _DEBUG
  #undef DEBUGPARAMS
  #define DEBUGPARAMS   , ReturnAddress(), -1
#endif

void *operator new (size_t size)
{
  void *res;

  if ((res = win_malloc(size DEBUGPARAMS)) != NULL)
    return res;
  throw std::bad_alloc () ;
}

void *operator new[] (size_t size)
{
  void *res;

  if ((res = win_malloc(size DEBUGPARAMS)) != NULL)
    return res;
  throw std::bad_alloc () ;
}

void operator delete (void *p) throw ()
{
  win_free(p DEBUGPARAMS);
}

void operator delete[] (void *p) throw ()
{
  win_free(p DEBUGPARAMS);
}

#endif // #if WIN_MEM_TRACKING
