//******************************************************************************
///
/// @file core/support/simplevector.h
///
/// Specialized replacements for `std::vector` optimized for speed.
///
/// Very simple, basic vector-like classes containing just enough functionality
/// for their intended uses within POV. Flexibility is sacrificed for
/// performance as these classes will typically be used in places where they may
/// constructed and destroyed many millions of times per render. (For example,
/// mediasky.pov rendered at only 160x120 with no AA results in over 16 million
/// instances of construction of a FixedSimpleVector).
///
/// These classes were added after extensive profiling pointed to a number of
/// instances of our use of std::vector causing slowdowns, particularly when
/// multiple threads were in use (due to locks in the RTL memory management used
/// to prevent heap corruption). Experiments with non-heap-based allocators
/// (e.g. refpools or thread-local storage) did improve the situation somewhat
/// but weren't enough, hence this file. At the time of writing we get about a
/// 10% improvement as compared to the old code.
///
/// @note   Be aware that these classes do NOT run destructors on contained
///         objects. This is intentional as we currently do not store any
///         objects in them that require this functionality.
///
/// @author Christopher J. Cason.
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

#ifndef POVRAY_CORE_SIMPLEVECTOR_H
#define POVRAY_CORE_SIMPLEVECTOR_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
#include <vector>

// C++ standard header files
#include <stdexcept>

// POV-Ray header files (base module)
#include "base/pov_err.h"

// POV-Ray header files (core module)
#include "core/coretypes.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreSupportSimplevector Fast Data Containers
/// @ingroup PovCore
///
/// @{

////////////////////////////////////////////////////////////////////////////
// Works like std::vector in some ways, but very limited and not at all as
// flexible. Does not implement all the methods of std::vector (just what
// is needed in POV). Has different allocation behaviour, which will probably
// need to be tweaked over time for best performance.
//
// Be aware that this class does NOT run destructors on contained objects.
// This is intentional as we currently do not store any objects in it that
// require this functionality. // TODO FIXME
////////////////////////////////////////////////////////////////////////////
template<class ContainerType, class Allocator = std::allocator<ContainerType>>
class SimpleVector final
{
public:
    typedef SimpleVector<ContainerType> MyType;
    typedef size_t size_type;
    typedef size_t difference_type;
    typedef ContainerType *pointer;
    typedef ContainerType& reference;
    typedef ContainerType value_type;
    typedef const ContainerType *const_pointer;
    typedef const ContainerType& const_reference;
    typedef const ContainerType *const_iterator;
    typedef ContainerType *iterator;
    typedef Allocator allocator_type;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    SimpleVector()
    {
        m_First = m_Last = m_End = nullptr;
    }

    SimpleVector(size_type nItems, const ContainerType& InitialVal)
    {
        m_First = m_Last = m_End = nullptr;
        if (nItems)
            allocate (nItems, InitialVal);
    }

    SimpleVector(const MyType& RHS)
    {
        if (RHS.m_First != RHS.m_Last)
        {
            allocate (RHS.capacity());
            for (pointer p = RHS.m_First ; p != RHS.m_Last ; )
                *m_Last++ = *p++;
        }
        else
            m_First = m_Last = m_End = nullptr;
    }

    ~SimpleVector()
    {
        // we don't call destructors, even if they exist
        if (m_First != nullptr)
            deallocate ();
    }

    MyType& operator=(const MyType& RHS)
    {
        if (RHS.size() > capacity())
        {
            if (m_First != nullptr)
                deallocate ();
            allocate (RHS.size());
        }
        m_Last = m_First;
        for (pointer p = RHS.m_First ; p != RHS.m_Last ; )
            *m_Last++ = *p++;
        return (*this);
    }

    size_type capacity() const
    {
        return (m_End - m_First);
    }

    iterator begin()
    {
        return (m_First);
    }

    const_iterator begin() const
    {
        return (m_First);
    }

    const_iterator cbegin() const
    {
        return (m_First);
    }

    iterator end()
    {
        return (m_Last);
    }

    const_iterator end() const
    {
        return (m_Last);
    }

    const_iterator cend() const
    {
        return (m_Last);
    }

    reverse_iterator rbegin()
    {
        return (reverse_iterator (m_Last));
    }

    const_reverse_iterator rbegin() const
    {
        return (const_reverse_iterator (m_Last));
    }

    const_reverse_iterator crbegin() const
    {
        return (const_reverse_iterator (m_Last));
    }

    reverse_iterator rend()
    {
        return (reverse_iterator (m_First));
    }

    const_reverse_iterator rend() const
    {
        return (const_reverse_iterator (m_First));
    }

    const_reverse_iterator crend() const
    {
        return (const_reverse_iterator (m_First));
    }

    size_type size() const
    {
        return (m_Last - m_First);
    }

    size_type max_size() const
    {
        return (alloc.max_size ());
    }

    bool empty() const
    {
        return (m_First == m_Last);
    }

    const_reference at(size_type Index) const
    {
        if (Index > size())
            throw std::out_of_range ("index out of range in SimpleVector::at");
        return (m_First [Index]);
    }

    reference at(size_type Index)
    {
        if (Index > size())
            throw std::out_of_range ("index out of range in SimpleVector::at");
        return (m_First [Index]);
    }

    const_reference operator[](size_type Index) const
    {
        return (m_First [Index]);
    }

    reference operator[](size_type Index)
    {
        return (m_First [Index]);
    }

    reference front()
    {
        return (*m_First);
    }

    const_reference front() const
    {
        return (*m_First);
    }

    reference back()
    {
        return (*(m_Last - 1));
    }

    const_reference back() const
    {
        return (*(m_Last - 1));
    }

    void push_back(const ContainerType& NewVal)
    {
        if (m_Last < m_End)
        {
            *m_Last++ = NewVal;
            return;
        }
        insert(m_Last, NewVal);
    }

    void pop_back()
    {
        if (m_Last > m_First)
            --m_Last;
    }

    iterator insert(iterator Where, const ContainerType& NewVal)
    {
        size_type Index = 0;

        if (m_Last > m_First)
            Index = Where - m_First;

        if (m_Last == m_End)
        {
            size_type c = size() + 1;
            size_type n = capacity();
            size_type nc = n * 2;
            if (nc < 8)
                nc = 8;
            pointer p = alloc.allocate (nc);
            p [Index] = NewVal;
            for (size_type i = 0 ; i < Index ; i++)
                p [i] = m_First [i];
            for (size_type i = Index + 1 ; i < c ; i++)
                p [i] = m_First [i];
            if (m_First != nullptr)
                alloc.deallocate (m_First, n);
            m_First = p;
            m_End = m_First + nc;
            m_Last = m_First + c;
            return (m_First + Index);
        }

        if (Index == size())
        {
            *m_Last = NewVal;
            return (m_Last++);
        }

        for (size_type i = size() ; i > Index ; i--)
            m_First [i] = m_First [i - 1];
        m_Last++;
        m_First [Index] = NewVal;
        return (m_First + Index);
    }

    iterator erase(iterator What)
    {
        size_type Index = What - begin();

        if (What == m_Last - 1)
            return (m_Last--);

        for (pointer p1 = What, p2 = What + 1 ; p2 < m_Last ; )
            *p1++ = *p2++;
        m_Last--;
        return (++What);
    }

    void clear()
    {
        m_Last = m_First;
    }

    void reserve(size_type nItems)
    {
        size_type c = capacity();
        if (c < nItems)
        {
            pointer p = alloc.allocate(nItems);
            size_type n = size();
            for (size_type i = 0; i < n; ++i)
                p[i] = m_First[i];
            if (m_First != nullptr)
                alloc.deallocate(m_First, c);
            m_First = p;
            m_End = m_First + nItems;
            m_Last = m_First + c;
        }
    }

private:
    void allocate (size_type nItems)
    {
        m_Last = m_First = alloc.allocate (nItems);
        m_End = m_First + nItems;
    }

    void allocate (size_type nItems, const ContainerType& InitialVal)
    {
        m_Last = m_First = alloc.allocate (nItems);
        m_End = m_First + nItems;
        while (nItems--)
            *m_Last++ = InitialVal;
    }

    void deallocate ()
    {
        alloc.deallocate (m_First, m_End - m_First);
    }

    allocator_type alloc;
    pointer m_First;
    pointer m_End;
    pointer m_Last;
};

////////////////////////////////////////////////////////////////////////////
// This template class requires a maximum size (ElementCount) and maintains
// its storage internally (typically therefore this will end up on the stack
// rather than being allocated upon request). The up side to this behaviour
// is that no time is spent obtaining memory from a pool, or for that matter
// copying data if a reallocation is needed. The down side is that firstly
// it cannot expand beyond the maximum size specified, and secondly that
// binary copies of the object take longer because they contain the entire
// storage (even if no entries are allocated).
//
// Be aware that this class does NOT run destructors on contained objects.
// This is intentional as we currently do not store any objects in it that
// require this functionality.
////////////////////////////////////////////////////////////////////////////
template<class ContainerType, int ElementCount>
class FixedSimpleVector final
{
public:
    typedef FixedSimpleVector<ContainerType, ElementCount> MyType;
    typedef size_t size_type;
    typedef size_t difference_type;
    typedef ContainerType *pointer;
    typedef ContainerType& reference;
    typedef ContainerType value_type;
    typedef const ContainerType *const_pointer;
    typedef const ContainerType& const_reference;
    typedef const ContainerType *const_iterator;
    typedef ContainerType *iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    FixedSimpleVector() :
        m_Last ((pointer) m_Data),
        m_End (pointer (m_Data) + ElementCount)
    {
    }

    FixedSimpleVector(size_type nItems, const ContainerType& InitialVal) :
        m_Last ((pointer) m_Data),
        m_End (pointer (m_Data) + ElementCount)
    {
        if (nItems > ElementCount)
            throw POV_EXCEPTION(pov_base::kInternalLimitErr, "Internal limit exceeded in FixedSimpleVector");
        while (nItems--)
            *m_Last++ = InitialVal;
    }

    FixedSimpleVector(const MyType& RHS) :
        m_Last ((pointer) m_Data),
        m_End (pointer (m_Data) + ElementCount)
    {
        for (pointer p = pointer (RHS.m_Data) ; p != RHS.m_Last ; )
            *m_Last++ = *p++;
    }

    ~FixedSimpleVector()
    {
        // we don't call destructors, even if they exist
    }

    MyType& operator=(const MyType& RHS)
    {
        if (RHS.size() > ElementCount)
            throw POV_EXCEPTION(pov_base::kInternalLimitErr, "Internal limit exceeded in FixedSimpleVector");
        m_Last = pointer (m_Data);
        for (pointer p = pointer (RHS.m_Data) ; p != RHS.m_Last ; )
            *m_Last++ = *p++;
        return (*this);
    }

    size_type capacity() const
    {
        return (ElementCount);
    }

    iterator begin()
    {
        return (reinterpret_cast<pointer>(&(m_Data[0])));
    }

    const_iterator begin() const
    {
        return (reinterpret_cast<const_pointer>(&(m_Data[0])));
    }

    iterator end()
    {
        return (m_Last);
    }

    const_iterator end() const
    {
        return (m_Last);
    }

    reverse_iterator rbegin()
    {
        return (reverse_iterator (m_Last));
    }

    const_reverse_iterator rbegin() const
    {
        return (const_reverse_iterator (m_Last));
    }

    reverse_iterator rend()
    {
        return (reverse_iterator (pointer (m_Data)));
    }

    const_reverse_iterator rend() const
    {
        return (const_reverse_iterator (pointer (m_Data)));
    }

    size_type size() const
    {
        return (m_Last - const_pointer (m_Data));
    }

    size_type max_size() const
    {
        return (ElementCount);
    }

    bool empty() const
    {
        return (const_pointer (m_Data) == m_Last);
    }

    const_reference at(size_type Index) const
    {
        if (Index > size())
            throw std::out_of_range ("index out of range in FixedSimpleVector::at");
        return (pointer (m_Data) [Index]);
    }

    reference at(size_type Index)
    {
        if (Index > size())
            throw std::out_of_range ("index out of range in FixedSimpleVector::at");
        return (pointer (m_Data) [Index]);
    }

    const_reference operator[](size_type Index) const
    {
        return (pointer (m_Data) [Index]);
    }

    reference operator[](size_type Index)
    {
        return (pointer (m_Data) [Index]);
    }

    reference front()
    {
        return (*pointer (m_Data));
    }

    const_reference front() const
    {
        return (*pointer (m_Data));
    }

    reference back()
    {
        return (*(m_Last - 1));
    }

    const_reference back() const
    {
        return (*(m_Last - 1));
    }

    void push_back(const ContainerType& NewVal)
    {
        if (m_Last == m_End)
            throw POV_EXCEPTION(pov_base::kInternalLimitErr, "Internal limit exceeded in FixedSimpleVector");
        *m_Last++ = NewVal;
    }

    void pop_back()
    {
        if (m_Last > pointer (m_Data))
            --m_Last;
    }

    iterator insert(iterator Where, const ContainerType& NewVal)
    {
        if (m_Last == m_End)
            throw POV_EXCEPTION(pov_base::kInternalLimitErr, "Internal limit exceeded in FixedSimpleVector");
        for (pointer p1 = Where, p2 = Where + 1 ; p1 < m_Last ; )
            *p2++ = *p1++;
        m_Last++;
        *Where = NewVal;
        return (Where);
    }

    iterator erase(iterator Where)
    {
        if (Where == m_End)
            throw POV_EXCEPTION(pov_base::kInternalLimitErr, "Attempt to erase past end of vector");
        if (Where == m_Last - 1)
            return (m_Last--);
        for (pointer p1 = Where, p2 = Where + 1 ; p2 < m_Last ; )
            *p1++ = *p2++;
        m_Last--;
        return (++Where);
    }

    void clear()
    {
        m_Last = pointer (m_Data);
    }

private:
    unsigned char m_Data [sizeof (value_type) * ElementCount];
    const pointer m_End;
    pointer m_Last;
};

//******************************************************************************

/// Helper class for @ref PooledSimpleVector
///
/// @attention
///     During thread cleanup, vectors handed out by one instance of this class
///     (via the @ref alloc() method) may happen to be returned to a different
///     instance (via the @ref release() method). The implementation must handle
///     such cases gracefully.
///
template<class VECTOR_T>
class VectorPool final
{
public:

    VectorPool(size_t sizeHint = 0, size_t initialPoolSize = POV_VECTOR_POOL_SIZE) :
        mPool(),
        mSizeHint(sizeHint)
    {
        if (initialPoolSize != 0)
            mPool.reserve(initialPoolSize);
    }

    ~VectorPool()
    {
        for (auto&& p : mPool)
            delete p;
    }

    VectorPool(const VectorPool&) = delete;
    VectorPool& operator=(VectorPool&) = delete;

    VECTOR_T *alloc()
    {
        VECTOR_T* p;
        if (mPool.empty())
        {
            p = new VECTOR_T();
        }
        else
        {
            p = mPool.back();
            mPool.pop_back();
        }
        p->clear();
        if (mSizeHint != 0)
            p->reserve(mSizeHint);
        return p;
    }

    void release(VECTOR_T* p)
    {
        mSizeHint = p->capacity();
        mPool.push_back(p);
    }

private:

    std::vector<VECTOR_T*> mPool;
    size_t mSizeHint;
};

//******************************************************************************

/// Wrapper for simple vector allocated using a thread-local object pool.
///
/// @attention
///     This class does __not__ run destructors on contained objects.
///     This is intentional as we currently do not store any objects in it that
///     require this functionality.
///
template<typename ELEMENT_T, size_t SIZE_HINT>
class PooledSimpleVector final
{
public:

    typedef POV_SIMPLE_VECTOR<ELEMENT_T>                VectorType;
    typedef VectorPool<VectorType>                      PoolType;

    typedef typename VectorType::value_type             value_type;
    typedef typename VectorType::allocator_type         allocator_type;
    typedef typename VectorType::size_type              size_type;
    typedef typename VectorType::difference_type        difference_type;
    typedef typename VectorType::reference              reference;
    typedef typename VectorType::const_reference        const_reference;
    typedef typename VectorType::pointer                pointer;
    typedef typename VectorType::const_pointer          const_pointer;
    typedef typename VectorType::iterator               iterator;
    typedef typename VectorType::const_iterator         const_iterator;
    typedef typename VectorType::reverse_iterator       reverse_iterator;
    typedef typename VectorType::const_reverse_iterator const_reverse_iterator;

    PooledSimpleVector() :
        mpVector(GetPool().alloc())
    {}

    PooledSimpleVector(const PooledSimpleVector& o) :
        mpVector(GetPool().alloc())
    {
        *mpVector = *o.mpVector;
    }

    ~PooledSimpleVector()
    {
        GetPool().release(mpVector);
    }

    inline PooledSimpleVector& operator=(const PooledSimpleVector& o)
    {
        *mpVector = *o.mpVector;
    }

    // assign() not supported
    // get_allocator() not supported

    inline reference at(size_type i)                        { return mpVector->at(i); }
    inline const_reference at(size_type i) const            { return mpVector->at(i); }
    inline reference operator[](size_type i)                { return (*mpVector)[i]; }
    inline const_reference operator[](size_type i) const    { return (*mpVector)[i]; }
    inline reference front()                                { return mpVector->front(); }
    inline const_reference front() const                    { return mpVector->front(); }
    inline reference back()                                 { return mpVector->back(); }
    inline const_reference back() const                     { return mpVector->back(); }
    // data() not supported

    inline iterator begin()                                 { return mpVector->begin(); }
    inline const_iterator begin() const                     { return mpVector->begin(); }
    inline const_iterator cbegin() const                    { return mpVector->cbegin(); }
    inline iterator end()                                   { return mpVector->end(); }
    inline const_iterator end() const                       { return mpVector->end(); }
    inline const_iterator cend() const                      { return mpVector->cend(); }
    inline reverse_iterator rbegin()                        { return mpVector->rbegin(); }
    inline const_reverse_iterator rbegin() const            { return mpVector->rbegin(); }
    inline const_reverse_iterator crbegin() const           { return mpVector->crbegin(); }
    inline reverse_iterator rend()                          { return mpVector->rend(); }
    inline const_reverse_iterator rend() const              { return mpVector->rend(); }
    inline const_reverse_iterator crend() const             { return mpVector->crend(); }

    inline bool empty() const                               { return mpVector->empty(); }
    inline size_type size() const                           { return mpVector->size(); }
    inline size_type max_size() const                       { return mpVector->max_size(); }
    inline void reserve(size_type n)                        { mpVector->reserve(n); }
    inline size_type capacity() const                       { return mpVector->capacity(); }

    inline void clear()                                     { mpVector->clear(); }
    inline iterator insert(iterator i, const_reference v)   { return mpVector->insert(i, v); }
    // other overloads of insert() not supported
    // emplace() not supported
    inline iterator erase(iterator i)                       { return mpVector->erase(i); }
    // other overloads of erase() not supported
    inline void push_back(const_reference v)                { mpVector->push_back(v); }
    // other overloads of push_back() not supported
    // emplace_back() not supported
    inline void pop_back()                                  { mpVector->pop_back(); }
    // resize() not supported
    // swap() not supported

private:

    /// Get thread-local vector pool instance.
    ///
    /// @note
    ///     This construct is deliberate; while it would seem natural to use a straightforward
    ///     `static thread_local` class member variable instead, in template classes that seems to
    ///     lead to runtime errors with certain compilers (seen e.g. with XCode 9.3).
    ///
    static inline PoolType& GetPool()
    {
        static thread_local PoolType pool(SIZE_HINT);
        return pool;
    }

    VectorType* mpVector;
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_SIMPLEVECTOR_H
