//******************************************************************************
///
/// @file base/textstreambuffer.h
///
/// Declarations related to buffered text file output.
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

#ifndef TEXTSTREAMBUFFER_H
#define TEXTSTREAMBUFFER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of standard C header files
#include <cctype>
#include <cstdarg>
#include <cstdio>

// must nuke these since everyone's favourite monopoly's cstdio still defines
// them for some reason (why not just use inlines like everyone else?)
#undef  getc
#undef  putc
#undef  getchar
#undef  putchar

namespace pov_base
{

class TextStreamBuffer
{
    public:
        TextStreamBuffer(size_t buffersize = 1024*8, unsigned int wrapwidth = 80);
        virtual ~TextStreamBuffer();

        void printf(const char *format, ...);
        void print(const char *str);
        void puts(const char *str);
        void putc(int chr);
        void printfile(const char *filename, POV_LONG offset, POV_LONG lines);
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

}

#endif
