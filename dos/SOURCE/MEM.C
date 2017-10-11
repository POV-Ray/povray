/****************************************************************************
*                mem.c
*
*  This module contains the code for our own memory allocation/deallocation,
*  providing memory tracing, statistics, and garbage collection options.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1999 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by email to team-coord@povray.org or visit us on the web at
*  http://www.povray.org. The latest version of POV-Ray may be found at this site.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

#include "frame.h"
#include "povproto.h"           /* Error() */

#include "mem.h"
#include "parse.h"              /* MAError() */
#include "povray.h"             /* stats[] global var */


/************************************************************************
* AUTHOR
*
*   Steve Anger:70714,3113
*
* DESCRIPTION
*
This module replaces the memory allocation calls malloc, calloc, realloc
and free with the macros POV_MALLOC, POV_CALLOC, POV_REALLOC, and POV_FREE.
These macros work the same as the standard C functions except that the
POV_xALLOC functions also take a message as the last parameter and
automatically call MAError(msg) if the allocation fails. That means that
instead of writing

  if ((New = malloc(sizeof(*New))) == NULL)
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

mem_mark()
  Starts a new memory pool. The next call to mem_release() will only release
  memory allocated after this call.

mem_release (int LogFile)
  Releases all unfree'd memory allocated since the last call to mem_mark().
  The LogFile parameter determines if it dumps the list of unfree'd memory to
  a file.

mem_release_all (int LogFile)
  Releases all unfree'd memory allocated since the program started running.

POV-Ray only uses the mem_release_all() function however mem_mark() and
mem_release() might be useful for implenting a leak-free animation loop.

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
#define MALLOC malloc
#endif

#ifndef CALLOC
#define CALLOC calloc
#endif

#ifndef REALLOC
#define REALLOC realloc
#endif

#ifndef FREE
#define FREE free
#endif


/****************************************************************************/
/* internal use                                                             */
/****************************************************************************/

/* if TRACE is on, the RECLAIM must also be on */
#if defined(MEM_TRACE) && !defined (MEM_RECLAIM)
#define MEM_RECLAIM
#endif

/* This is the filename created for memory leakage information */
#if defined(MEM_TRACE)
#define MEM_LOG_FNAME   "Memory.Log"
#endif

/* determine if we need to add a header to our memory records */
#if defined(MEM_TAG) || defined(MEM_RECLAIM) || defined(MEM_TRACE) || defined(MEM_STATS)
#define MEM_HEADER
#endif

#define MEMNODE struct mem_node

#if defined(MEM_HEADER)

struct mem_node
{

/* We have to do lots of testing here to make sure that the size of the 
 * mem_node struct is an even multiple of the sizeof(double) (usually 8
 * bytes, or we royally screw up some architectures.  Yuck!!!  To make
 * things easier, we have smaller groups of variables, and make them
 * work out to multiples of 8 bytes, rather than trying to do it for the
 * whole structure.  In most cases, only 4 bytes are wasted, as this is
 * mostly for debugging anyways.
 */

#if defined(MEM_TAG)
  long tag;
#if !defined(MEM_STATS) || defined(MEM_TRACE) 
  long junk1;
#endif /* !MEM_STATS */
#endif /* MEM_TAG */

#if defined(MEM_STATS) && !defined(MEM_TRACE) 
  size_t size;
#if !defined(MEM_TAG)
  long junk1;
#endif /* !MEM_TAG */
#endif /* MEM_STATS */

#if defined(MEM_RECLAIM)
  MEMNODE *prev;
  MEMNODE *next;
  long poolno;
#if defined(MEM_TRACE)
  char *file;
  long line;
  size_t size;
#else
  long junk2;
#endif /* MEM_TRACE */
#endif /* MEM_RECLAIM */

};
#endif /* MEM_HEADER */


#if defined(MEM_RECLAIM)
static int poolno = 0;
static MEMNODE *memlist = NULL;
#endif


static int leak_msg = FALSE;


#if defined(MEM_HEADER)
#define NODESIZE ((sizeof(MEMNODE)+3)/4)*4  /* Align memory on 4 byte boundary */
#else
#define NODESIZE 0
#endif


#if defined(MEM_RECLAIM)
static void add_node(MEMNODE * node);
static void remove_node(MEMNODE * node);
#endif


#if defined(MEM_TAG)
/* the tag value that marks our used memory */
#define MEMTAG_VALUE   0x4D546167L

static int mem_check_tag(MEMNODE * node);

#endif


#if defined(MEM_RECLAIM)
static long num_nodes;          /* keep track of valence of node list */
#endif /* MEM_RECLAIM */


#if defined(MEM_STATS)

typedef struct MemStats_Struct MEMSTATS;

struct MemStats_Struct
{
  size_t   smallest_alloc;    /* smallest # of bytes in one malloc() */
  size_t   largest_alloc;     /* largest # of bytes in one malloc() */
  size_t   current_mem_usage; /* current total # of bytes allocated */
  size_t   largest_mem_usage; /* peak total # of bytes allocated */
#if (MEM_STATS>=2)
  /* could add a running average size too, someday */
  long int total_allocs;      /* total # of alloc calls */
  long int total_frees;       /* total # of free calls */
  char    *smallest_file;     /* file name of largest alloc */
  int      smallest_line;     /* file line of largest alloc */
  char    *largest_file;      /* file name of largest alloc */
  int      largest_line;      /* file line of largest alloc */
#endif
};

/* keep track of memory allocation statistics */
static MEMSTATS mem_stats;

/* local prototypes */
static void mem_stats_init (void);
static void mem_stats_alloc (size_t nbytes, char *file, int line);
static void mem_stats_free (size_t nbytes);

#endif


/****************************************************************************/
void mem_init()
{
#if defined(MEM_RECLAIM)
  num_nodes = 0;
  poolno = 0;
  memlist = NULL;
#endif
#if defined(MEM_STATS)
  mem_stats_init();
#endif
  leak_msg = FALSE;
}


#if defined(MEM_TAG)
/****************************************************************************/
/* return TRUE if pointer is non-null and has a valid tag */
static int mem_check_tag(MEMNODE *node)
{
  int isOK = FALSE;

  if (node != NULL)
    if (node->tag == MEMTAG_VALUE)
      isOK = TRUE;
  return isOK;
}

#endif /* MEM_TAG */


/****************************************************************************/
void *pov_malloc(size_t size, char *file, int line, char *msg)
{
  void *block;
  size_t totalsize;
#if defined(MEM_HEADER)
  MEMNODE *node;
#endif

#if defined(MEM_HEADER)
  if (size == 0)
  {
    Error("Attempt to malloc zero size block (File: %s Line: %d).\n", file, line);
  }
#endif

  totalsize=size+NODESIZE; /* number of bytes allocated in OS */

  block = (void *)MALLOC(totalsize);

  if (block == NULL)
    MAError(msg, size);

#if defined(MEM_HEADER)
  node = (MEMNODE *) block;
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

#if defined(MEM_RECLAIM)
  add_node(node);
#endif

#if defined(MEM_STATS)
  mem_stats_alloc(totalsize, file, line);
#endif

  return (void *)((char *)block + NODESIZE);
}


/****************************************************************************/
void *pov_calloc(size_t nitems, size_t size, char *file, int line, char *msg)
{
  void *block;
  size_t actsize;
  size_t totalsize; /* number of bytes allocated in OS */
#if defined(MEM_HEADER)
  MEMNODE *node;
#endif

  actsize=nitems*size;
  totalsize=actsize+NODESIZE;

#if defined(MEM_HEADER)
  if (actsize == 0)
  {
    Error("Attempt to calloc zero size block (File: %s Line: %d).\n", file, line);
  }
#endif

  block = (void *)MALLOC(totalsize);

  if (block == NULL)
    MAError(msg, actsize);

  memset(block, 0, totalsize);

#if defined(MEM_HEADER)
  node = (MEMNODE *) block;
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

#if defined(MEM_RECLAIM)
  add_node(node);
#endif

#if defined(MEM_STATS)
  mem_stats_alloc(totalsize, file, line);
#endif

  return (void *)((char *)block + NODESIZE);
}


/****************************************************************************/
void *pov_realloc(void *ptr, size_t size, char *file, int line, char *msg)
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

  if (size == 0)
  {
    if (ptr)
      pov_free(ptr, file, line);
    return NULL;
  }
  else if (ptr == NULL)
    return pov_malloc(size, file, line, msg);

  block = (void *)((char *)ptr - NODESIZE);

#if defined(MEM_HEADER)
  node = (MEMNODE *) block;
#endif

#if defined(MEM_TAG)
  if (node->tag != MEMTAG_VALUE)
    Error("Attempt to realloc invalid block (File: %s Line: %d).\n", file, line);

  node->tag = ~node->tag;
#endif

#if defined(MEM_RECLAIM)
  prev = node->prev;
  next = node->next;
#endif

  block = (void *)REALLOC(block, NODESIZE + size);

  if (block == NULL)
    MAError(msg, size);

#if defined(MEM_STATS)
  /* REALLOC does an implied FREE... */
  oldsize = ((MEMNODE *)block)->size;
  mem_stats_free(oldsize);
  /* ...and an implied MALLOC... */
  mem_stats_alloc(NODESIZE + size, file, line);
#endif

#if defined(MEM_HEADER)
  node = (MEMNODE *) block;
#endif

#if defined(MEM_TAG)
  node->tag = MEMTAG_VALUE;
#endif

#if defined(MEM_TRACE) || defined(MEM_STATS)
  node->size = size + NODESIZE;
#endif
#if defined(MEM_TRACE)
  node->file = file;
  node->line = line;
#endif

#if defined(MEM_RECLAIM)
  if (prev == NULL)
    memlist = node;
  else
    prev->next = node;
  if (node->next != NULL)
    node->next->prev = node;
  if (next != NULL)
    next->prev = node;
#endif

  return (void *)((char *)block + NODESIZE);
}


/****************************************************************************/
void pov_free(void *ptr, char *file, int line)
{
  void *block;

#if defined(MEM_HEADER)
  MEMNODE *node;
#endif

  if (ptr == NULL)
    Error("Attempt to free NULL pointer (File: %s Line: %d).\n", file, line);

  block = (void *)((char *)ptr - NODESIZE);

#if defined(MEM_HEADER)
  node = (MEMNODE *) block;
#endif

#if defined(MEM_TAG)
  if (node->tag == ~MEMTAG_VALUE)
  {
    Warning(0.0, "Attempt to free already free'd block (File: %s Line: %d).\n", file, line);
    return;
  }
  else if (node->tag != MEMTAG_VALUE)
  {
    Warning(0.0, "Attempt to free invalid block (File: %s Line: %d).\n", file, line);
    return;
  }

#endif

#if defined(MEM_RECLAIM)
  remove_node(node);
#endif

#if defined(MEM_TAG)
  /* do this After remove_node, so remove_node can check validity of nodes */
  node->tag = ~node->tag;
#endif

#if defined(MEM_STATS)
  mem_stats_free(((MEMNODE*)block)->size);
#endif

  FREE(block);
}


/****************************************************************************/
/* Starts a new memory pool. The next mem_release() call will
   only release memory allocated after this call. */
void mem_mark()
{
#if defined(MEM_RECLAIM)
  poolno++;
#endif
}


/****************************************************************************/
/* Releases all unfree'd memory from current memory pool */
void mem_release(int LogFile)
{
#if defined(MEM_RECLAIM)
  FILE *f = NULL;
  MEMNODE *p, *tmp;
  size_t totsize;

  p = memlist;
  totsize = 0;

#if defined(MEM_TRACE)
  if (LogFile)
  {
    if (p != NULL && (p->poolno == poolno))
      f = fopen(MEM_LOG_FNAME, APPEND_TXTFILE_STRING);
  }
#endif /* MEM_TRACE */

  while (p != NULL && (p->poolno == poolno))
  {
#if defined(MEM_TRACE)

#if defined(MEM_TAG)
    if (!mem_check_tag(p))
      Debug_Info("mem_release(): Memory pointer corrupt!\n");
#endif /* MEM_TAG */

    totsize += (p->size-NODESIZE);
    if (LogFile)
    {
      if (!leak_msg)
      {
        Debug_Info("Memory leakage detected, see file '%s' for list\n",MEM_LOG_FNAME);
        leak_msg = TRUE;
      }

      if (f != NULL)
        fprintf(f, "File:%13s  Line:%4d  Size:%lu\n", p->file, p->line, (unsigned long)(p->size-NODESIZE));
    }
#endif /* MEM_TRACE */

#if defined(MEM_STATS)
    mem_stats_free(p->size);
#endif

    tmp = p;
    p = p->next;
    remove_node(tmp);
    FREE(tmp);
  }

  if (f != NULL)
    fclose(f);

  if (totsize > 0)
    Debug_Info("%lu bytes reclaimed (pool #%d)\n", totsize, poolno);

  if (poolno > 0)
    poolno--;

#if defined(MEM_STATS)
  /* reinitialize the stats structure for next time through */
  mem_stats_init();
#endif

#endif /* MEM_RECLAIM */
}


/****************************************************************************/
/* Released all unfree'd memory from all pools */
void mem_release_all(int LogFile)
{
#if defined(MEM_RECLAIM)
  FILE *f = NULL;
  MEMNODE *p, *tmp;
  size_t totsize;

  Status_Info("Reclaiming memory\n");

  p = memlist;
  totsize = 0;

#if defined(MEM_TRACE)
  if (LogFile)
  {
    if (p != NULL)
      f = fopen(MEM_LOG_FNAME, APPEND_TXTFILE_STRING);
  }
#endif

  while (p != NULL)
  {
#if defined(MEM_TRACE)

    #if defined(MEM_TAG)
    if (!mem_check_tag(p))
      Debug_Info("mem_release_all(): Memory pointer corrupt!\n");
    #endif /* MEM_TAG */

    totsize += (p->size-NODESIZE);
    if (LogFile)
    {
      if (!leak_msg)
      {
        Debug_Info("Memory leakage detected, see file '%s' for list\n",MEM_LOG_FNAME);
        leak_msg = TRUE;
      }

      if (f != NULL)
        fprintf(f, "File:%13s  Line:%4d  Size:%lu\n", p->file, p->line, (unsigned long)(p->size-NODESIZE));
    }
#endif

#if defined(MEM_STATS)
    /* This is after we have printed stats, and this may slow us down a little,      */
    /* so we may want to simply re-initialize the mem-stats at the end of this loop. */
    mem_stats_free(p->size);
#endif

    tmp = p;
    p = p->next;
    remove_node(tmp);
    FREE(tmp);
  }

  if (f != NULL)
    fclose(f);

  if (totsize > 0)
    Debug_Info("\n%lu bytes reclaimed\n", totsize);

  poolno = 0;
#endif

#if defined(MEM_STATS)
  /* reinitialize the stats structure for next time through */
  mem_stats_init();
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

  if (memlist == NULL)
  {
    memlist = node;
    node->poolno = poolno;
    node->prev = NULL;
    node->next = NULL;
    num_nodes = 0;
  }
  else
  {
    memlist->prev = node;
    node->poolno = poolno;
    node->prev = NULL;
    node->next = memlist;
    memlist = node;
  }
  num_nodes++;
}


/****************************************************************************/
/* Detatches a node from the 'allocated' list but doesn't free it */
static void remove_node(MEMNODE *node)
{

#if defined(MEM_TAG)
  if (!mem_check_tag(node))
    Debug_Info("remove_node(): Memory pointer corrupt!\n");
#endif /* MEM_TAG */

  num_nodes--;
  if (node->prev != NULL)
    node->prev->next = node->next;

  if (node->next != NULL)
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

}

#endif /* MEM_RECLAIM */


/****************************************************************************/
/* A strdup routine that uses POV_MALLOC                                    */
/****************************************************************************/
char *pov_strdup(char *s)
{
  char *New;
  
  New=(char *)POV_MALLOC(strlen(s)+1,s);
  strcpy(New,s);
  return (New);
}

/****************************************************************************/
/* A memmove routine for those systems that don't have one                  */
/****************************************************************************/

void *pov_memmove (void *dest, void  *src, size_t length)
{
  char *csrc =(char *)src;
  char *cdest=(char *)dest;
  
  if (csrc < cdest && csrc + length >= cdest)
  {
    size_t size = cdest - csrc;

    while (length > 0)
    {
      memcpy(cdest + length - size, csrc + length - size, size);

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
      memcpy(new_dest, csrc, length);

      new_dest += size;
      csrc += size;
      length -= size;

      if (length < size)
        size = length;
    }
  }
  else
  {
    memcpy(cdest, csrc, length);
  }

  return cdest;
}


/****************************************************************************/
/* Memory Statistics gathering routines                                     */
/****************************************************************************/

#if defined(MEM_STATS)

/****************************************************************************/
static void mem_stats_init()
{
  mem_stats.smallest_alloc    = 65535;  /* Must be an unsigned number */
  mem_stats.largest_alloc     = 0;
  mem_stats.current_mem_usage = 0;
  mem_stats.largest_mem_usage = 0;
#if (MEM_STATS>=2)
  mem_stats.total_allocs      = 0;
  mem_stats.total_frees       = 0;
  mem_stats.largest_file      = "none";
  mem_stats.largest_line      = -1;
  mem_stats.smallest_file     = "none";
  mem_stats.smallest_line     = -1;
#endif
}

/****************************************************************************/
/* update appropriate fields when an allocation takes place                 */
static void mem_stats_alloc(size_t nbytes, char *file, int line)
{
  /* update the fields */
  if ((mem_stats.smallest_alloc<0) || (nbytes<mem_stats.smallest_alloc))
  {
    mem_stats.smallest_alloc = nbytes;
#if (MEM_STATS>=2)
    mem_stats.smallest_file = file;
    mem_stats.smallest_line = line;
#endif
  }

  if (nbytes>mem_stats.largest_alloc)
  {
    mem_stats.largest_alloc = nbytes;
#if (MEM_STATS>=2)
    mem_stats.largest_file = file;
    mem_stats.largest_line = line;
#endif
  }

#if (MEM_STATS>=2)
  mem_stats.total_allocs++;
#endif

  mem_stats.current_mem_usage += nbytes;

  if (mem_stats.current_mem_usage>mem_stats.largest_mem_usage)
  {
    mem_stats.largest_mem_usage = mem_stats.current_mem_usage;
  }

}

/****************************************************************************/
/* update appropriate fields when a free takes place                        */
static void mem_stats_free(size_t nbytes)
{
  /* update the fields */
  mem_stats.current_mem_usage -= nbytes;
#if (MEM_STATS>=2)
  mem_stats.total_frees++;
#endif
}

/****************************************************************************/
/* Level 1                                                                  */

/****************************************************************************/
size_t mem_stats_smallest_alloc()
{
  return mem_stats.smallest_alloc;
}
/****************************************************************************/
size_t mem_stats_largest_alloc()
{
  return mem_stats.largest_alloc;
}
/****************************************************************************/
size_t mem_stats_current_mem_usage()
{
  return mem_stats.current_mem_usage;
}
/****************************************************************************/
size_t mem_stats_largest_mem_usage()
{
  return mem_stats.largest_mem_usage;
}

/****************************************************************************/
/* Level 2                                                                  */

#if (MEM_STATS>=2)

/****************************************************************************/
char *mem_stats_smallest_file()
{
  return mem_stats.smallest_file;
}
/****************************************************************************/
int mem_stats_smallest_line()
{
  return mem_stats.smallest_line;
}
/****************************************************************************/
char *mem_stats_largest_file()
{
  return mem_stats.largest_file;
}
/****************************************************************************/
int mem_stats_largest_line()
{
  return mem_stats.largest_line;
}
/****************************************************************************/
long int mem_stats_total_allocs()
{
  return mem_stats.total_allocs;
}
/****************************************************************************/
long int mem_stats_total_frees()
{
  return mem_stats.total_frees;
}

#endif

#endif /* MEM_STATS */


