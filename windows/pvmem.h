//******************************************************************************
///
/// @file windows/pvmem.h
///
/// Definitions for windows-specific memory handling routines.
///
/// @author Christopher J. Cason
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

#ifndef POVRAY_WINDOWS_PVMEM_H
#define POVRAY_WINDOWS_PVMEM_H

#undef new
#undef delete

namespace povwin
{

class WinMemStats
{
  public:
    WinMemStats() { Clear(); }
    ~WinMemStats() {}

    void Clear() { currentAllocated = peakAllocated = callsToAlloc = callsToFree = smallestAlloc = largestAlloc = 0; }
    void * operator new(size_t count) { return (WinMemStats *) HeapAlloc(GetProcessHeap(), 0, count); }
    void operator delete (void *p) { HeapFree(GetProcessHeap(), 0, p); }

    void RecordAlloc(size_t count)
    {
      callsToAlloc++;
      currentAllocated += count;
      if (currentAllocated > 0)
        if (peakAllocated < currentAllocated)
          peakAllocated = currentAllocated;
      if (count > 1)
        if ((smallestAlloc == 0) || (count < smallestAlloc))
          smallestAlloc = count;
      if (count > largestAlloc)
        largestAlloc = count;
    }

    void RecordFree(size_t count)
    {
      callsToFree++;
      currentAllocated -= count;
    }

    void InterlockedRecordAlloc(size_t count)
    {
    }

    void InterlockedRecordFree(size_t count)
    {
    }

    void Report(uint64& allocs, uint64& frees, int64& current, uint64& peak, uint64 &smallest, uint64 &largest)
    {
      allocs = callsToAlloc;
      frees = callsToFree;
      current = currentAllocated;
      peak = peakAllocated;
      smallest = smallestAlloc;
      largest = largestAlloc;
    }

    WinMemStats& operator=(const WinMemStats& rhs)
    {
      // not interlocked, should only be used when thread is not performing memory allocations
      currentAllocated = rhs.currentAllocated;
      peakAllocated = rhs.peakAllocated;
      callsToAlloc = rhs.callsToAlloc;
      callsToFree = rhs.callsToFree;
      smallestAlloc = rhs.smallestAlloc;
      largestAlloc = rhs.largestAlloc;
      return *this;
    }

    WinMemStats operator+(const WinMemStats& rhs) const
    {
      // not interlocked, should only be used when thread is not performing memory allocations
      WinMemStats result = *this;
      result.currentAllocated += rhs.currentAllocated;
      result.callsToAlloc += rhs.callsToAlloc;
      result.callsToFree += rhs.callsToFree;
      result.peakAllocated = 0;
      result.smallestAlloc = 0;
      result.largestAlloc = 0;
      return result;
    }

    WinMemStats& operator+=(const WinMemStats& rhs)
    {
      // not interlocked, should only be used when threads are not performing memory allocations
      currentAllocated += rhs.currentAllocated;
      callsToAlloc += rhs.callsToAlloc;
      callsToFree += rhs.callsToFree;
      peakAllocated = 0;
      smallestAlloc = 0;
      largestAlloc = 0;
      return *this;
    }

  private:
    volatile int64 currentAllocated;
    volatile uint64 peakAllocated;
    volatile uint64 callsToAlloc;
    volatile uint64 callsToFree;
    volatile uint64 smallestAlloc;
    volatile uint64 largestAlloc;
};

typedef WinMemStats PovMemStats;

class WinHeap
{
  public:
    WinHeap()
    {
      InitializeCriticalSectionAndSpinCount(&m_CritSec, 4000);
      m_LowFragHeap = false;

      SYSTEM_INFO si;
      GetSystemInfo(&si);

      // allocate heap
      m_Heap = HeapCreate (HEAP_NO_SERIALIZE, si.dwPageSize, 0);
      if (m_Heap == NULL)
        return;

#if 0
      // enable low-fragmentation heap. NB this only works on XP or later.
      // TODO: make sure this isn't an unresolved symbol in W2K or earlier.
      ULONG val = 2;
      HeapSetInformation(m_Heap, HeapCompatibilityInformation, &val, sizeof(val));
      if (HeapQueryInformation(m_Heap, HeapCompatibilityInformation, &val, sizeof(val), NULL))
        if (val == 2)
          m_LowFragHeap = true;
#endif
    }
    ~WinHeap()
    {
      if (m_Heap != NULL)
      {
        Lock();
        HeapDestroy(m_Heap);
        Unlock();
      }
      DeleteCriticalSection(&m_CritSec);
    }

    void * operator new(size_t count) { return (WinHeap *) HeapAlloc(GetProcessHeap(), 0, count); }
    bool Validate(const void *p = NULL) const { return m_Heap != NULL && HeapValidate(m_Heap, 0, p); }
    void *Alloc(size_t count) { return HeapAlloc(m_Heap, 0, count ? count : 1); }
    bool Free(void *p) { return HeapFree(m_Heap, 0, p); }
    void *ReAlloc(void *p, size_t count) { return HeapReAlloc(m_Heap, 0, p, count); }
    void Compact() { HeapCompact(m_Heap, 0); }
    void Lock() { EnterCriticalSection(&m_CritSec); }
    void Unlock() { LeaveCriticalSection(&m_CritSec); }
    size_t BlockSize(const void *p) const { return HeapSize(m_Heap, 0, p); }

    void *LockedAlloc(size_t count)
    {
      Lock();
      void *p = HeapAlloc(m_Heap, 0, count ? count : 1);
      Unlock();
      return p;
    }

    void *LockedReAlloc(void *p, size_t count)
    {
      Lock();
      p = HeapReAlloc(m_Heap, 0, p, count);
      Unlock();
      return p;
    }

    bool LockedFree(void *p)
    {
      Lock();
      bool result = HeapFree(m_Heap, 0, p);
      Unlock();
      return result;
    }

    bool LockedValidate(const void *p = NULL)
    {
      if (m_Heap == NULL)
        return false;
      Lock();
      bool result = HeapValidate(m_Heap, 0, p);
      Unlock();
      return result;
    }

  private:
    bool m_LowFragHeap;
    HANDLE m_Heap;
    CRITICAL_SECTION m_CritSec;
};

class HeapLock
{
  public:
    HeapLock(WinHeap *wh) : m_WinHeap(wh) { m_WinHeap->Lock(); }
    ~HeapLock() { m_WinHeap->Unlock(); }
  private:
    WinHeap *m_WinHeap;
};

class WinMemBlock
{
public:
  __declspec(nothrow) WinMemBlock(const void *data, int line)
  {
    m_Sanity = 'PM';
    m_Line = line;
    m_Data = data;
#   if EXTRA_VALIDATION
      m_Hash = ptrdiff_t(m_Sanity) ^ ptrdiff_t(m_Line) ^ ptrdiff_t(m_Data) ^ ptrdiff_t(m_Length) ^ ptrdiff_t(this);
      memcpy((char *) this + m_Length - sizeof(m_Hash), &m_Hash, sizeof(m_Hash));
#   endif
  }

  // must be called with the lock held
  void __declspec(nothrow) * operator new (size_t size, size_t len)
  {
#   if EXTRA_VALIDATION
      WinMemBlock *block = (WinMemBlock *) m_Heap->Alloc(len + size + BlockPadding);
      if (block == NULL)
        return NULL;
      block->m_Length = len + size + BlockPadding;
      return block;
#   else
      return m_Heap->Alloc(len + size);
#   endif
  }

  // must be called with the lock held
  void __declspec(nothrow) operator delete (void *p)
  {
    m_Heap->Free(p);
  }

  bool Validate(bool full = EXTRA_VALIDATION) const
  {
    if (m_Sanity != 'PM')
      return false;
#   if EXTRA_VALIDATION
      ptrdiff_t hash = ptrdiff_t(m_Sanity) ^ ptrdiff_t(m_Line) ^ ptrdiff_t(m_Data) ^ ptrdiff_t(m_Length) ^ ptrdiff_t(this);
      if (m_Hash != hash || memcmp(&m_Hash, (char *) this + m_Length - sizeof(m_Hash), sizeof(m_Hash)) != 0)
        return false;
#   endif
    if (!full)
      return true;
    if (m_Heap->Validate(this) == false)
      return false;
#   if EXTRA_VALIDATION
      if (m_Heap->BlockSize(this) < m_Length)
        return false;
#   endif
    return true;
  }

  size_t Size() const
  {
    size_t len = m_Heap->BlockSize(this);
    if (len == (size_t) -1 || len < sizeof(WinMemBlock) + BlockPadding)
      return (size_t) -1;
#   if EXTRA_VALIDATION
      if (len < m_Length)
        return (size_t) -1;
#   endif
    return len;
  }

  size_t Size(size_t size) const
  {
    return size + sizeof(WinMemBlock) + BlockPadding;
  }

  void *GetMem()
  {
    return this + 1;
  }

  WinMemBlock *Realloc(size_t size, const void *data, int line)
  {
    WinMemBlock *newBlock = (WinMemBlock *) m_Heap->ReAlloc(this, size + sizeof(WinMemBlock) + BlockPadding);
    if (newBlock == NULL)
      return NULL;
    // note that *this is no longer valid here!
    newBlock->m_Line = line;
    newBlock->m_Data = data;
#   if EXTRA_VALIDATION
      newBlock->m_Length = size + sizeof(WinMemBlock) + BlockPadding;
      newBlock->m_Hash = ptrdiff_t(newBlock->m_Sanity) ^ ptrdiff_t(newBlock->m_Line) ^ ptrdiff_t(newBlock->m_Data) ^
                         ptrdiff_t(newBlock->m_Length) ^ ptrdiff_t(newBlock);
      memcpy((char *) newBlock + newBlock->m_Length - sizeof(m_Hash), &newBlock->m_Hash, sizeof(m_Hash));
#   endif
    return newBlock;
  }

  static WinMemBlock *GetBlock(void *p, bool full = EXTRA_VALIDATION)
  {
    WinMemBlock *block = (WinMemBlock *) p - 1;
    if (!block->Validate(full))
      return NULL;
    return block;
  }

  static void SetAllocator(WinHeap *alloc) { m_Heap = alloc; }

private:
  unsigned short m_Sanity;
  short m_Line;
  const void *m_Data;
  static WinHeap *m_Heap;

# if EXTRA_VALIDATION
    size_t m_Length;
    ptrdiff_t m_Hash;
    static const size_t BlockPadding = sizeof(ptrdiff_t);
# else
    static const size_t BlockPadding = 0;
# endif

  WinMemBlock() {} // not available
  void *operator new (size_t len) {} // not available
};

}

#endif // POVRAY_WINDOWS_PVMEM_H
