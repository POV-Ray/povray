//******************************************************************************
///
/// @file pov_mem.h
///
/// This module contains all defines, typedefs, and prototypes for
/// `pov_mem.cpp`.
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

#ifndef POV_MEM_H
#define POV_MEM_H

namespace pov
{

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
void mem_release (void);
void mem_release_all (void);
void *pov_malloc (size_t size, const char *file, int line, const char *msg);
void *pov_realloc (void *ptr, size_t size, const char *file, int line, const char *msg);
void pov_free (void *ptr, const char *file, int line);
char *pov_strdup (const char *s);
void *pov_memmove (void *dest, void *src, size_t length);

#if defined(MEM_STATS)
/* These are level 1 routines */
size_t mem_stats_current_mem_usage (void);
size_t mem_stats_largest_mem_usage (void);
size_t mem_stats_smallest_alloc (void);
size_t mem_stats_largest_alloc (void);
/* These are level 2 routines */
#if (MEM_STATS>=2)
const char* mem_stats_smallest_file (void);
int mem_stats_smallest_line (void);
const char* mem_stats_largest_file (void);
int mem_stats_largest_line (void);
long int mem_stats_total_allocs (void);
long int mem_stats_total_frees (void);
#endif
#endif

}

#endif /* MEM_H */
