//******************************************************************************
///
/// @file base/pov_mem.h
///
/// Declarations for memory handling.
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
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_BASE_POV_MEM_H
#define POVRAY_BASE_POV_MEM_H

#include "base/configbase.h"

namespace pov_base
{

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

void mem_init (void);
void mem_release_all (void);
void *pov_malloc (size_t size, const char *file, int line, const char *msg);
void *pov_realloc (void *ptr, size_t size, const char *file, int line, const char *msg);
void pov_free (void *ptr, const char *file, int line);
char *pov_strdup (const char *s);
void *pov_memmove (void *dest, void *src, size_t length);

/// @}
///
//##############################################################################

}

#endif // POVRAY_BASE_POV_MEM_H
