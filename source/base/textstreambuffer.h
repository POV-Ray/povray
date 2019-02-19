//******************************************************************************
///
/// @file base/textstreambuffer.h
///
/// Declarations related to buffered text file output.
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

#ifndef POVRAY_BASE_TEXTSTREAMBUFFER_H
#define POVRAY_BASE_TEXTSTREAMBUFFER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"
#include "base/textstreambuffer_fwd.h"

// C++ variants of C standard header files
#include <cctype>
#include <cstdio>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

namespace pov_base
{

//##############################################################################
///
/// @addtogroup PovBaseTextstream
///
/// @{

class TextStreamBuffer
{
    public:
        TextStreamBuffer(size_t buffersize = 1024*8, unsigned int wrapwidth = 80);
        virtual ~TextStreamBuffer();

        void printf(const char *format, ...);
        void print(const char *str);
        void puts(const char *str);
        void putc(int chr);
        void printfile(const char *filename, POV_OFF_T offset, POV_LONG lines);
        void printfile(FILE *file, POV_LONG lines);
        void flush();
    protected:
        virtual void lineoutput(const char *str, unsigned int chars);
        virtual void directoutput(const char *str, unsigned int chars);
        virtual void rawoutput(const char *str, unsigned int chars);
    private:
        char *buffer;
        size_t boffset;
        size_t bsize;
        unsigned int wrap;
        POV_LONG curline;

        void lineflush();
        void directflush(const char *str, unsigned int chars);
};

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_TEXTSTREAMBUFFER_H
