/****************************************************************************
*                   mem.h
*
*  This module contains all defines, typedefs, and prototypes for mem.c
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


#ifndef POV_MEM_H
#define POV_MEM_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/



/*****************************************************************************
* Global typedefs
******************************************************************************/



/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

void mem_init (void);
void mem_mark (void);
void mem_release (int LogFile);
void mem_release_all (int LogFile);
void *pov_malloc (size_t size, char *file, int line, char *msg);
void *pov_calloc (size_t nitems, size_t size, char *file, int line, char *msg);
void *pov_realloc (void *ptr, size_t size, char *file, int line, char *msg);
void pov_free (void *ptr, char *file, int line);
char *pov_strdup (char *s);
void *pov_memmove (void *dest, void *src, size_t length);

#if defined(MEM_STATS)
/* These are level 1 routines */
size_t mem_stats_current_mem_usage (void);
size_t mem_stats_largest_mem_usage (void);
size_t mem_stats_smallest_alloc (void);
size_t mem_stats_largest_alloc (void);
/* These are level 2 routines */
#if (MEM_STATS>=2)
char* mem_stats_smallest_file (void);
int mem_stats_smallest_line (void);
char* mem_stats_largest_file (void);
int mem_stats_largest_line (void);
long int mem_stats_total_allocs (void);
long int mem_stats_total_frees (void);
#endif
#endif


#endif /* MEM_H */
