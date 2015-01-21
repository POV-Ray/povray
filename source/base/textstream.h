//******************************************************************************
///
/// @file base/textstream.h
///
/// This module contains all defines, typedefs, and prototypes for the C++
/// interface of `textstream.cpp`.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2014 Persistence of Vision Raytracer Pty. Ltd.
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
//*******************************************************************************

#ifndef TEXTSTREAM_H
#define TEXTSTREAM_H

#include <cstdio>

// must nuke these since everyone's favourite monopoly's cstdio still defines
// them for some reason (why not just use inlines like everyone else?)
#undef  getc
#undef  putc
#undef  getchar
#undef  putchar

#include "configbase.h"

#include "base/fileinputoutput.h"
#include "base/pov_err.h"
#include "base/stringutilities.h"
#include "base/types.h"

namespace pov_base
{

const int ITEXTSTREAM_BUFFER_SIZE = DEFAULT_ITEXTSTREAM_BUFFER_SIZE;

class ITextStream
{
    public:
        struct FilePos
        {
            POV_LONG offset;
            POV_LONG lineno;
        };

        ITextStream(const UCS2 *, unsigned int);
        ITextStream(const UCS2 *, IStream *);
        virtual ~ITextStream();

        int getchar();
        void ungetchar(int);

        bool eof() const;
        bool seekg(FilePos);
        FilePos tellg() const;

        POV_LONG line() const { return lineno; };

        const UCS2 *name() const { return filename.c_str(); };
    private:
        IStream *stream;
        unsigned char buffer[ITEXTSTREAM_BUFFER_SIZE];
        POV_ULONG bufferoffset;
        POV_ULONG maxbufferoffset;
        POV_ULONG filelength;
        POV_ULONG curpos;
        UCS2String filename;
        POV_LONG lineno;
        int ungetbuffer;

        void RefillBuffer();
};

class OTextStream
{
    public:
        OTextStream(const UCS2 *, unsigned int, bool append = false);
        OTextStream(const UCS2 *, OStream *);
        virtual ~OTextStream();

        void putchar(int);
        void putraw(int);
        void printf(const char *, ...);
    void flush();

        const UCS2 *name() const { return filename.c_str(); };
    private:
        OStream *stream;
        UCS2String filename;
};

}

#endif
