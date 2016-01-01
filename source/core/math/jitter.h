//******************************************************************************
///
/// @file core/math/jitter.h
///
/// @todo   What's in here?
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

#ifndef POVRAY_CORE_JITTER_H
#define POVRAY_CORE_JITTER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

namespace pov
{

#ifdef DYNAMIC_HASHTABLE
extern unsigned short *hashTable; // GLOBAL VARIABLE
#else
extern ALIGN16 unsigned short hashTable[]; // GLOBAL VARIABLE
#endif

extern const float JitterTable[]; // GLOBAL VARIABLE

inline DBL Jitter2d(int x, int y)
{
    return JitterTable[int(hashTable[int(hashTable[int(x & 0xfff)] ^ y) & 0xfff]) & 0xff];
}

inline void Jitter2d(int x, int y, DBL& jx, DBL& jy)
{
    jx = JitterTable[int(hashTable[int(hashTable[int((x + 1021) & 0xfff)] ^ (y + 1019)) & 0xfff]) & 0xff]; // Note that 1019 and 1021 are prime! [trf]
    jy = JitterTable[int(hashTable[int(hashTable[int((x + 1019) & 0xfff)] ^ (y + 1021)) & 0xfff]) & 0xff]; // Note that 1019 and 1021 are prime! [trf]
}

inline DBL Jitter2d(DBL x, DBL y)
{
    return JitterTable[int(hashTable[int(hashTable[(int(x * 1021.0) & 0xfff)] ^ int(y * 1021.0)) & 0xfff]) & 0xff]; // Note that 1021 is prime! [trf]
}

inline void Jitter2d(DBL x, DBL y, DBL& jx, DBL& jy)
{
    jx = JitterTable[int(hashTable[int(hashTable[(int(x * 1021.0) & 0xfff)] ^ int(y * 1019.0)) & 0xfff]) & 0xff]; // Note that 1019 and 1021 are prime! [trf]
    jy = JitterTable[int(hashTable[int(hashTable[(int(x * 1019.0) & 0xfff)] ^ int(y * 1021.0)) & 0xfff]) & 0xff]; // Note that 1019 and 1021 are prime! [trf]
}

}

#endif // POVRAY_CORE_JITTER_H
