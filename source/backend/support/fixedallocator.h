//******************************************************************************
///
/// @file backend/support/fixedallocator.h
///
/// Simple allocator using a fixed-size pool.
///
/// @author Christopher J. Cason.
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

#if !defined __FIXED_ALLOCATOR_H__
#define __FIXED_ALLOCATOR_H__

#include <cassert>

namespace pov
{
    template <typename T, int MaxElements>
    class FixedAllocator
    {
    public:
        typedef T               value_type ;
        typedef unsigned short  size_type ;
        typedef ptrdiff_t       difference_type;
        typedef T&              reference ;
        typedef const T&        const_reference ;
        typedef T               *pointer ;
        typedef const T         *const_pointer ;

    private:
        size_type byteCount (unsigned char nItems) { return ((size_type) nItems * sizeof (T)) ; }
        typedef struct
        {
            unsigned char nItems ;
            unsigned char allocated ;
        } BlockHeader ;

        void defrag (void)
        {
            unsigned char current = m_FirstFreeBlock ;
            while (current < MaxElements)
            {
                unsigned char next = current + m_Headers [current].nItems ;
                if (next >= MaxElements)
                    break ;
                if (m_Headers [current].allocated || m_Headers [next].allocated)
                {
                    current = next ;
                    continue ;
                }
                m_Headers [current].nItems += m_Headers [next].nItems ;
                m_Headers [next].nItems = 0 ;
            }
        }

        unsigned char allocateBlocks (unsigned char nBlocks, unsigned char current)
        {
            assert (nBlocks > 0) ;
            while (current < MaxElements)
            {
                unsigned char next = current + m_Headers [current].nItems ;
                if (m_Headers [current].allocated == false && m_Headers [current].nItems >= nBlocks)
                {
                    if (current + nBlocks < MaxElements && m_Headers [current].nItems > nBlocks)
                    {
                        m_Headers [current + nBlocks].nItems = m_Headers [current].nItems - nBlocks ;
                        m_Headers [current + nBlocks].allocated = false ;
                    }
                    m_Headers [current].nItems = nBlocks ;
                    m_Headers [current].allocated = true ;
                    if (m_FirstFreeBlock == current)
                        m_FirstFreeBlock = current + nBlocks ;
                    return (current) ;
                }
                current = next ;
            }
            return (0xff) ;
        }

    public:
        // construct an allocator for the supplied new type.
        template <typename N>
        struct rebind { typedef FixedAllocator<N, MaxElements> other ; } ;

        size_type max_size() const { return (MaxElements) ; }
        pointer address (reference what) const { return (&what) ; }
        const_pointer address (const_reference what) const { return (&what) ; }
        FixedAllocator ()
        {
            if (MaxElements > 127)
                throw POV_EXCEPTION_CODE(kInternalLimitErr, "Internal limit exceeded in fixedallocator.h") ;
            m_FirstFreeBlock = 0 ;
            m_Headers [0].allocated = false ;
            m_Headers [0].nItems = MaxElements ;
        }
        ~FixedAllocator () { }
        void construct (pointer where, T const& what) { new (where) T (what) ; }
        void destroy (pointer what) { what->~T() ; }

        pointer allocate (size_type nItems, std::allocator<void>::const_pointer hint = 0)
        {
            if ((reinterpret_cast<unsigned char *>(hint) >= m_Data) && (reinterpret_cast<unsigned char *>(hint) < m_Data + sizeof (T) * MaxElements))
            {
                unsigned char slot = (reinterpret_cast<unsigned char *>(hint) - m_Data) / sizeof (T) ;
                if ((slot = allocateBlocks (nItems, slot)) != 0xff)
                    return ((pointer) (m_Data + byteCount (slot))) ;
            }
            unsigned char slot = allocateBlocks (nItems, m_FirstFreeBlock) ;
            if (slot == 0xff)
            {
                defrag () ;
                if ((slot = allocateBlocks (nItems, m_FirstFreeBlock)) == 0xff)
                    throw POV_EXCEPTION_CODE(kInternalLimitErr, "Internal limit exceeded in fixedallocator.h") ;
            }
            return ((pointer) (m_Data + byteCount (slot))) ;
        }

        void deallocate (pointer what, unsigned char nItems)
        {
            if ((reinterpret_cast<unsigned char *>(what) >= m_Data) && (reinterpret_cast<unsigned char *>(what) < m_Data + sizeof (T) * MaxElements))
            {
                unsigned char slot = (reinterpret_cast<unsigned char *>(what) - m_Data) / sizeof (T) ;
                if (m_Headers [slot].allocated && m_Headers [slot].nItems == nItems)
                {
                    m_Headers [slot].allocated = false ;
                    if (m_FirstFreeBlock > slot)
                    {
                        m_FirstFreeBlock = slot ;
                        assert (m_Headers [slot].nItems > 0) ;
                    }
                    unsigned char next = slot + nItems ;
                    while (next < MaxElements && m_Headers [next].allocated == false)
                    {
                        m_Headers [slot].nItems += m_Headers [next].nItems ;
                        m_Headers [next].nItems = 0 ;
                        next = slot + m_Headers [slot].nItems ;
                    }
                }
            }
        }

    private:
        unsigned char m_FirstFreeBlock ;
        unsigned char m_Data [sizeof (T) * MaxElements] ;
        BlockHeader m_Headers [MaxElements] ;
    } ;

    template <typename T>
    class MallocAllocator
    {
    public:
        typedef T               value_type ;
        typedef size_t          size_type ;
        typedef ptrdiff_t       difference_type;
        typedef T&              reference ;
        typedef const T&        const_reference ;
        typedef T               *pointer ;
        typedef const T         *const_pointer ;

    public:
        // construct an allocator for the supplied new type.
        template <typename N>
        struct rebind { typedef MallocAllocator<N> other ; } ;

        size_type max_size() const { return (0xffffffff) ; }
        pointer address (reference what) const { return (&what) ; }
        const_pointer address (const_reference what) const { return (&what) ; }
        MallocAllocator () { }
        ~MallocAllocator () { }
        void construct (pointer where, T const& what) { new (where) T (what) ; }
        void destroy (pointer what) { what->~T() ; }

        pointer allocate (size_type nItems, std::allocator<void>::const_pointer hint = 0)
        {
            return ((pointer) malloc (sizeof (T) * nItems)) ;
        }

        void deallocate (pointer what, size_type nItems)
        {
            free (what) ;
        }
    } ;
} // end namespace pov

#endif

