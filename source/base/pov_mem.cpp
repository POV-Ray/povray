//******************************************************************************
///
/// @file base/pov_mem.cpp
///
/// Implementations for memory handling.
///
/// This unit contains the code for our own memory allocation/deallocation,
/// providing memory tracing, statistics, and garbage collection options.
///
/// @deprecated
///     Since new code should use C++-style memory management using the `new`
///     and `delete` operators, and legacy code should be overhauled accordingly,
///     this unit will eventually be removed.
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
#include "base/pov_mem.h"

// C++ variants of C standard header files
#include <cstdlib>
#include <cstring>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

using std::size_t;

/************************************************************************
* AUTHOR
*
*   Steve Anger:70714,3113
*
* DESCRIPTION
*
This module replaces the memory allocation calls malloc, realloc
and free with the macros POV_MALLOC, POV_REALLOC, and POV_FREE.
These macros work the same as the standard C functions except that the
POV_xALLOC functions also take a message as the last parameter and
automatically call MAError(msg) if the allocation fails. That means that
instead of writing

    if ((New = malloc(sizeof(*New))) == nullptr)
    {
        MAError ("new object");
    }

you'd just use

    New = POV_MALLOC (sizeof(*New), "new object");

This also expands the function of the macros to include error checking and
memory tracking.

The following macros need to be defined in config.h, depending of what
features the compile needs:

#define MEM_TAG     - Enables memory tag debugging
--------------------------------------------------
Memory tag debugging adds a 32-bit identifier to the beginning of each
allocated memory block and erases it after the block has been free'd. This
lets POV_FREE verify that the block it's freeing is valid and issue an
error message if it isn't. Makes it easy to find those nasty double free's
which usually corrupt the heap.

#define MEM_RECLAIM - Enables garbage collection
------------------------------------------------
Garbage collection maintains a list of all currently allocated memory so
that it can be free'd when the program exits. Normally POV-Ray will free all
of its memory on its own, however abnormal exits such as parser errors or
user aborts bypass the destructors. There are four functions which control
the garbage collection:

mem_init()
  Initializes global variables used by the garbage collection routines.
  This function should be called once before any memory allocation functions
  are called.

mem_release_all ()
  Releases all unfree'd memory allocated since the program started running.

#define MEM_TRACE   - Enables garbage collection and memory tracing
-------------------------------------------------------------------
Memory tracing stores the file name and line number for ever POV_xALLOC
call and dumps a list of unfree'd blocks when POV-Ray terminates.

#define MEM_STATS 1 - enables tracking of memory statistics
-------------------------------------------------------------------
Memory statistics enables routines that will track overall memory usage.
After all memory allocation/deallocation has taken place, and before you
re-initialize everything with another mem_init() call, you can call some
accessor routines to determine how memory was used.  Setting MEM_STATS
to 1 only tracks peak memory usage.  Setting it to 2 additionally tracks
number of calls to malloc/free and some other statistics.
*
* CHANGES
*
*   Aug 1995 : Steve Anger - Creation.
*   Apr 1996 : Eduard Schwan - Added MEM_STATS code
*   Jul 1996 : Andreas Dilger - Force mem_header to align on double boundary
**************************************************************************/


/****************************************************************************/
/* Allow user definable replacements for memory functions                   */
/****************************************************************************/
#ifndef MALLOC
    #define MALLOC std::malloc
#endif

#ifndef REALLOC
    #define REALLOC std::realloc
#endif

#ifndef FREE
    #define FREE std::free
#endif


/****************************************************************************/
/* internal use                                                             */
/****************************************************************************/

// if PREFILL or GUARD on, STATS must also be on
#if (defined(MEM_PREFILL) || defined (MEM_GUARD)) && !defined (MEM_STATS)
    #define MEM_STATS
#endif

// if TRACE is on, the RECLAIM must also be on
#if defined(MEM_TRACE) && !defined (MEM_RECLAIM)
    #define MEM_RECLAIM
#endif

// This is the filename created for memory leakage information
#if defined(MEM_TRACE)
    #define MEM_LOG_FNAME   "Memory.Log"
#endif

// determine if we need to add a header to our memory records
#if defined(MEM_TAG) || defined(MEM_RECLAIM) || defined(MEM_TRACE) || defined(MEM_STATS)
    #define MEM_HEADER
#endif

#ifdef MEM_HEADER

    #define MEMNODE struct mem_node

    #ifndef MEM_HEADER_ALIGNMENT
        #define MEM_HEADER_ALIGNMENT sizeof(double)
    #endif

    struct mem_node final
    {
        #ifdef MEM_TAG
            int tag;
        #endif

        #ifdef MEM_STATS
            size_t size;
        #endif

        #ifdef MEM_RECLAIM
            MEMNODE *prev;
            MEMNODE *next;
            int poolno;

            #ifdef MEM_TRACE
                const char *file;
                int line;
            #endif

        #endif

    };

#endif /* MEM_HEADER */


#if defined(MEM_RECLAIM)
    static int poolno = 0; // GLOBAL VARIABLE
    static MEMNODE *memlist = nullptr; // GLOBAL VARIABLE
#endif

#if !defined(MEM_PREFILL_STRING)
    #define MEM_PREFILL_STRING "POVR"
#endif

#if !defined(MEM_CLEAR_STRING)
    #define MEM_CLEAR_STRING "CLEA"
#endif

#if !defined(MEM_GUARD_STRING)
    #define MEM_GUARD_STRING "GURD"
#endif

#if defined(MEM_GUARD)
    static char *mem_guard_string = MEM_GUARD_STRING;
    static size_t mem_guard_string_len = 0;
    #if !defined(MEM_GUARD_SIZE)
        #define MEM_GUARD_SIZE MEM_HEADER_ALIGNMENT
    #endif
#else
    #define MEM_GUARD_SIZE 0
#endif

#if defined(MEM_PREFILL)
    static char *mem_prefill_string = MEM_PREFILL_STRING; // GLOBAL VARIABLE
    static size_t mem_prefill_string_len = 0; // GLOBAL VARIABLE
    static char *mem_clear_string = MEM_CLEAR_STRING; // GLOBAL VARIABLE
    static size_t mem_clear_string_len = 0; // GLOBAL VARIABLE
#endif

static int leak_msg = false; // GLOBAL VARIABLE


#ifdef MEM_HEADER
    const int NODESIZE = (((sizeof(MEMNODE) + (MEM_HEADER_ALIGNMENT - 1)) / MEM_HEADER_ALIGNMENT) * MEM_HEADER_ALIGNMENT);
#else
    const int NODESIZE = 0;
#endif


#if defined(MEM_RECLAIM)
    static void add_node(MEMNODE * node);
    static void remove_node(MEMNODE * node);
#endif


#if defined(MEM_TAG)
    // the tag value that marks our used memory
    #define MEMTAG_VALUE   0x4D546167L

    static int mem_check_tag(MEMNODE * node);
#endif


#if defined(MEM_RECLAIM)
static long num_nodes;          /* keep track of valence of node list */ // GLOBAL VARIABLE
#endif /* MEM_RECLAIM */


/****************************************************************************/
void mem_init()
{
#if defined(MEM_RECLAIM)
    num_nodes = 0;
    poolno = 0;
    memlist = nullptr;
#endif
#if defined(MEM_GUARD)
    mem_guard_string_len = std::strlen(mem_guard_string);
#endif
#if defined(MEM_PREFILL)
    mem_prefill_string_len = std::strlen(mem_prefill_string);
    mem_clear_string_len = std::strlen(mem_clear_string);
#endif
    leak_msg = false;
}

#if defined(MEM_TAG)
/****************************************************************************/
/* return true if pointer is non-null and has a valid tag */
static int mem_check_tag(MEMNODE *node)
{
    int isOK = false;

    if (node != nullptr)
        if (node->tag == MEMTAG_VALUE)
            isOK = true;
    return isOK;
}

#endif /* MEM_TAG */


/****************************************************************************/
void *pov_malloc(size_t size, const char *file, int line, const char *msg)
{
    void *block;
    size_t totalsize;
#if defined(MEM_HEADER)
    MEMNODE *node;
#endif

#if defined(MEM_PREFILL) || defined(MEM_GUARD)
    char *memptr;
    size_t i;
#endif

#if defined(MEM_HEADER)
    if (size == 0)
    {
// TODO MESSAGE     Error("Attempt to malloc zero size block (File: %s Line: %d).", file, line);
    }
#endif

    totalsize = size + NODESIZE + (MEM_GUARD_SIZE * 2); /* number of bytes allocated in OS */

    block = reinterpret_cast<void *>(MALLOC(totalsize));

    if (block == nullptr)
        throw std::bad_alloc();; // TODO FIXME !!! // Parser::MAError(msg, (int)size);

#if defined(MEM_HEADER)
    node = reinterpret_cast<MEMNODE *>(block);
#endif

#if defined(MEM_TAG)
    node->tag = MEMTAG_VALUE;
#endif

#if defined(MEM_TRACE) || defined(MEM_STATS)
    node->size = totalsize;
#endif
#if defined(MEM_TRACE)
    node->file = file;
    node->line = line;
#endif

#if defined(MEM_PREFILL)
    memptr = reinterpret_cast<char *>(block) + NODESIZE + MEM_GUARD_SIZE;
    for(i = 0; i < size; i++)
        memptr[i] = mem_prefill_string[i % mem_prefill_string_len];
#endif

#if defined(MEM_GUARD)
    memptr = reinterpret_cast<char *>(block) + NODESIZE;
    for(i = 0; i < MEM_GUARD_SIZE; i++)
        memptr[i] = mem_guard_string[i % mem_guard_string_len];
    memptr = reinterpret_cast<char *>(block) + (reinterpret_cast<MEMNODE *>(block))->size - MEM_GUARD_SIZE;
    for(i = 0; i < MEM_GUARD_SIZE; i++)
        memptr[i] = mem_guard_string[i % mem_guard_string_len];
#endif

#if defined(MEM_RECLAIM)
    add_node(node);
#endif

    return reinterpret_cast<void *>(reinterpret_cast<char *>(block) + NODESIZE + MEM_GUARD_SIZE);
}


/****************************************************************************/
void *pov_realloc(void *ptr, size_t size, const char *file, int line, const char *msg)
{
    void *block;
#if defined(MEM_STATS)
    size_t oldsize;
#endif

#if defined(MEM_HEADER)
    MEMNODE *node;
#endif

#if defined(MEM_RECLAIM)
    MEMNODE *prev;
    MEMNODE *next;
#endif

#if defined(MEM_PREFILL) || defined(MEM_GUARD)
    char *memptr;
    size_t i;
#endif

    if (size == 0)
    {
        if (ptr)
            pov_free(ptr, file, line);
        return nullptr;
    }
    else if (ptr == nullptr)
        return pov_malloc(size, file, line, msg);

    block = reinterpret_cast<void *>(reinterpret_cast<char *>(ptr) - NODESIZE - MEM_GUARD_SIZE);

#if defined(MEM_GUARD)
    memptr = reinterpret_cast<char *>(block) + NODESIZE;
    for(i = 0; i < MEM_GUARD_SIZE; i++)
    {
        if(memptr[i] != mem_guard_string[i % mem_guard_string_len])
        {
            Warning(kWarningGeneral, "Damaged start guard detected in resized block (File: %s Line: %d).", file, line);
            break;
        }
    }
    memptr = reinterpret_cast<char *>(block) + (reinterpret_cast<MEMNODE *>(block))->size - MEM_GUARD_SIZE;
    for(i = 0; i < MEM_GUARD_SIZE; i++)
    {
        if(memptr[i] != mem_guard_string[i % mem_guard_string_len])
        {
            Warning(kWarningGeneral, "Damaged end guard detected in resized block (File: %s Line: %d).", file, line);
            break;
        }
    }
#endif

#if defined(MEM_HEADER)
    node = reinterpret_cast<MEMNODE *>(block);
#endif

#if defined(MEM_TAG)
    if (node->tag != MEMTAG_VALUE)
        Error("Attempt to realloc invalid block (File: %s Line: %d).", file, line);

    node->tag = ~node->tag;
#endif

#if defined(MEM_RECLAIM)
    prev = node->prev;
    next = node->next;
#endif

#if defined(MEM_STATS)
    oldsize = (reinterpret_cast<MEMNODE *>(block))->size;
#endif

#if defined(MEM_PREFILL)
    memptr = reinterpret_cast<char *>(block) + NODESIZE + MEM_GUARD_SIZE;
    for(i = size; i < oldsize - NODESIZE - (MEM_GUARD_SIZE * 2); i++)
        memptr[i] = mem_clear_string[i % mem_clear_string_len];
#endif

    block = reinterpret_cast<void *>(REALLOC(block, NODESIZE + (MEM_GUARD_SIZE * 2) + size));

    if (block == nullptr)
        throw std::bad_alloc(); // TODO FIXME !!! // Parser::MAError(msg, (int)size);

#if defined(MEM_PREFILL)
    memptr = reinterpret_cast<char *>(block) + NODESIZE + MEM_GUARD_SIZE;
    for(i = oldsize - NODESIZE - (MEM_GUARD_SIZE * 2); i < size; i++)
        memptr[i] = mem_prefill_string[i % mem_prefill_string_len];
#endif

#if defined(MEM_HEADER)
    node = reinterpret_cast<MEMNODE *>(block);
#endif

#if defined(MEM_TAG)
    node->tag = MEMTAG_VALUE;
#endif

#if defined(MEM_TRACE) || defined(MEM_STATS)
    node->size = size + NODESIZE + (MEM_GUARD_SIZE * 2);
#endif
#if defined(MEM_TRACE)
    node->file = file;
    node->line = line;
#endif

#if defined(MEM_GUARD)
    memptr = reinterpret_cast<char *>(block) + NODESIZE;
    for(i = 0; i < MEM_GUARD_SIZE; i++)
        memptr[i] = mem_guard_string[i % mem_guard_string_len];
    memptr = reinterpret_cast<char *>(block) + (reinterpret_cast<MEMNODE *>(block))->size - MEM_GUARD_SIZE;
    for(i = 0; i < MEM_GUARD_SIZE; i++)
        memptr[i] = mem_guard_string[i % mem_guard_string_len];
#endif

#if defined(MEM_RECLAIM)
    if (prev == nullptr)
        memlist = node;
    else
        prev->next = node;
    if (node->next != nullptr)
        node->next->prev = node;
    if (next != nullptr)
        next->prev = node;
#endif

    return reinterpret_cast<void *>(reinterpret_cast<char *>(block) + NODESIZE + MEM_GUARD_SIZE);
}


/****************************************************************************/
void pov_free(void *ptr, const char *file, int line)
{
    void *block;

#if defined(MEM_HEADER)
    MEMNODE *node;
#endif

#if defined(MEM_PREFILL) || defined(MEM_GUARD)
    char *memptr;
    size_t size;
    size_t i;
#endif

    if (ptr == nullptr)
        throw pov_base::Exception(nullptr, file, (unsigned int)line, "Attempt to free NULL pointer.");

    block = reinterpret_cast<void *>(reinterpret_cast<char *>(ptr) - NODESIZE - MEM_GUARD_SIZE);

#if defined(MEM_HEADER)
    node = reinterpret_cast<MEMNODE *>(block);
#endif

#if defined(MEM_TAG)
    if (node->tag == ~MEMTAG_VALUE)
    {
        Warning(kWarningGeneral, "Attempt to free already free'd block (File: %s Line: %d).", file, line);
        return;
    }
    else if (node->tag != MEMTAG_VALUE)
    {
        Warning(kWarningGeneral, "Attempt to free invalid block (File: %s Line: %d).", file, line);
        return;
    }

#endif

#if defined(MEM_GUARD)
    memptr = reinterpret_cast<char *>(block) + NODESIZE;
    for(i = 0; i < MEM_GUARD_SIZE; i++)
    {
        if(memptr[i] != mem_guard_string[i % mem_guard_string_len])
        {
            Warning(kWarningGeneral, "Damaged start guard detected in free'd block (File: %s Line: %d).", file, line);
            break;
        }
    }
    memptr = reinterpret_cast<char *>(block) + (reinterpret_cast<MEMNODE *>(block))->size - MEM_GUARD_SIZE;
    for(i = 0; i < MEM_GUARD_SIZE; i++)
    {
        if(memptr[i] != mem_guard_string[i % mem_guard_string_len])
        {
            Warning(kWarningGeneral, "Damaged end guard detected in free'd block (File: %s Line: %d).", file, line);
            break;
        }
    }
#endif

#if defined(MEM_RECLAIM)
    remove_node(node);
#endif

#if defined(MEM_TAG)

    /* do this After remove_node, so remove_node can check validity of nodes */
    node->tag = ~node->tag;
#endif

#if defined(MEM_PREFILL)
    size = (reinterpret_cast<MEMNODE *>(block))->size;
    memptr = reinterpret_cast<char *>(block) + NODESIZE + MEM_GUARD_SIZE;
    for(i = 0; i < size - NODESIZE - (MEM_GUARD_SIZE * 2); i++)
        memptr[i] = mem_clear_string[i % mem_clear_string_len];
#endif

    FREE(block);
}


/****************************************************************************/
/* Released all unfree'd memory from all pools */
void mem_release_all()
{
#if defined(MEM_RECLAIM)
    OStream *f = nullptr;
    MEMNODE *p, *tmp;
    size_t totsize;

//  Send_Progress("Reclaiming memory", PROGRESS_RECLAIMING_MEMORY);

    p = memlist;
    totsize = 0;

#if defined(MEM_TRACE)
    if (p != nullptr)
        f = New_OStream(MEM_LOG_FNAME, POV_File_Data_LOG, true);
#endif

    while (p != nullptr)
    {
#if defined(MEM_TRACE)

        #if defined(MEM_TAG)
        if (!mem_check_tag(p))
            Debug_Info("mem_release_all(): Memory pointer corrupt!\n");
        #endif /* MEM_TAG */

        totsize += (p->size - NODESIZE - (MEM_GUARD_SIZE * 2));
        if (!leak_msg)
        {
            Debug_Info("Memory leakage detected, see file '%s' for list\n",MEM_LOG_FNAME);
            leak_msg = true;
        }

        if (f != nullptr)
            f->printf("File:%13s  Line:%4d  Size:%lu\n", p->file, p->line, (unsigned long)(p->size - NODESIZE - (MEM_GUARD_SIZE * 2)));
#endif

        tmp = p;
        p = p->next;
        remove_node(tmp);
        FREE(tmp);
    }

    if (f != nullptr)
        delete f;

//  if (totsize > 0)
//      Debug_Info("\n%lu bytes reclaimed\n", totsize);

    poolno = 0;
    memlist = nullptr;
#endif
}


/****************************************************************************/
#if defined(MEM_RECLAIM)
/* Adds a new node to the 'allocated' list */
static void add_node(MEMNODE *node)
{

#if defined(MEM_TAG)
    if (!mem_check_tag(node))
        Debug_Info("add_node(): Memory pointer corrupt!\n");
#endif /* MEM_TAG */

    if (memlist == nullptr)
    {
        memlist = node;
        node->poolno = poolno;
        node->prev = nullptr;
        node->next = nullptr;
        num_nodes = 0;
    }
    else
    {
        memlist->prev = node;
        node->poolno = poolno;
        node->prev = nullptr;
        node->next = memlist;
        memlist = node;
    }
    num_nodes++;
}


/****************************************************************************/
/* Detaches a node from the 'allocated' list but doesn't free it */
static void remove_node(MEMNODE *node)
{

#if defined(MEM_TAG)
    if (!mem_check_tag(node))
        Debug_Info("remove_node(): Memory pointer corrupt!\n");
#endif /* MEM_TAG */

    num_nodes--;
    if (node->prev != nullptr)
        node->prev->next = node->next;

    if (node->next != nullptr)
        node->next->prev = node->prev;

    if (memlist == node)
    {
#if defined(MEM_TAG)
        /* check node->next if it is non-null, to insure it is safe to assign. */
        /* if it is null, it is safe since it is the last in the list. */
        if (node->next)
            if (!mem_check_tag(node->next))
                Debug_Info("remove_node(): memlist pointer corrupt!\n");
#endif /* MEM_TAG */

        memlist = node->next;
    }

    node->prev = nullptr;
    node->next = nullptr;
}

#endif /* MEM_RECLAIM */


/****************************************************************************/
/* A strdup routine that uses POV_MALLOC                                    */
/****************************************************************************/
char *pov_strdup(const char *s)
{
    char *New;

    New=reinterpret_cast<char *>(POV_MALLOC(std::strlen(s)+1,s));
    std::strcpy(New,s);
    return (New);
}

/****************************************************************************/
/* A memmove routine for those systems that don't have one                  */
/****************************************************************************/

void *pov_memmove (void *dest, const void  *src, size_t length)
{
    const char *csrc = reinterpret_cast<const char *>(src);
    char *cdest = reinterpret_cast<char *>(dest);

    if (csrc < cdest && csrc + length >= cdest)
    {
        size_t size = cdest - csrc;

        while (length > 0)
        {
            std::memcpy(cdest + length - size, csrc + length - size, size);

            length -= size;

            if (length < size)
                size = length;
        }
    }
    /* I'm not sure if this is needed, but my docs on memcpy say the regions
     * can't overlap, so theoretically we need to special case this.  If you
     * don't think it's necessary, you can just comment this part out.
     */
    else if (cdest < csrc && cdest + length >= csrc)
    {
        char *new_dest = cdest;
        size_t size = csrc - cdest;

        while (length > 0)
        {
            std::memcpy(new_dest, csrc, length);

            new_dest += size;
            csrc += size;
            length -= size;

            if (length < size)
                size = length;
        }
    }
    else
    {
        std::memcpy(cdest, csrc, length);
    }

    return cdest;
}

}
// end of namespace pov_base
