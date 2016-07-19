//******************************************************************************
///
/// @file base/textstream.h
///
/// Declarations related to text file input and output.
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

#ifndef TEXTSTREAM_H
#define TEXTSTREAM_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of standard C header files
#include <cstdio>

// must nuke these since everyone's favourite monopoly's cstdio still defines
// them for some reason (why not just use inlines like everyone else?)
#undef  getc
#undef  putc
#undef  getchar
#undef  putchar

// POV-Ray base header files
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

        ITextStream();
        virtual ~ITextStream();

        virtual int getchar() = 0;
        virtual void ungetchar(int) = 0;

        virtual bool eof() const = 0;
        virtual bool seekg(FilePos) = 0;
        virtual FilePos tellg() const = 0;

        virtual bool ReadRaw(unsigned char* buf, size_t size) = 0;

        /// Formal current line number of the file, e.g. to be displayed in error messages.
        POV_LONG line() const { return lineno; };

        /// Formal name of the file, e.g. to be displayed in error messages.
        virtual const UCS2 *name() const = 0;
    protected:
        POV_LONG lineno;
};

class IBufferedTextStream : public ITextStream
{
    public:
        IBufferedTextStream(const UCS2 *, unsigned int);

        /// @param[in]  formalName  Name by which the file is known to the user.
        /// @param[in]  byteStream  Underlying byte-oriented stream to read from.
        /// @param[in]  initialLine First line number as known to the user.
        IBufferedTextStream(const UCS2 *formalName, IStream *byteStream, POV_LONG initialLine = 1);

        virtual ~IBufferedTextStream();

        virtual int getchar();
        virtual void ungetchar(int);

        virtual bool eof() const;
        virtual bool seekg(FilePos);
        virtual FilePos tellg() const;

        virtual bool ReadRaw(unsigned char* buf, size_t size);

        /// Formal name of the file, e.g. to be displayed in error messages.
        virtual const UCS2 *name() const { return filename.c_str(); };
    private:
        IStream *stream;
        unsigned char buffer[ITEXTSTREAM_BUFFER_SIZE];
        POV_ULONG bufferoffset;
        POV_ULONG maxbufferoffset;
        POV_ULONG filelength;
        POV_ULONG curpos;
        UCS2String filename;
        int ungetbuffer;

        void RefillBuffer();
};

class IMemTextStream : public ITextStream
{
    public:
        /// @param[in]  formalName  Name by which the file is known to the user.
        /// @param[in]  data        Underlying memory buffer to read from.
        /// @param[in]  size        Size of underlying memory buffer.
        /// @param[in]  formalStart File position of buffer start as known to the user.
        IMemTextStream(const UCS2 *formalName, unsigned char* data, size_t size, const FilePos& formalStart);

        virtual ~IMemTextStream();

        virtual int getchar();
        virtual void ungetchar(int);

        virtual bool eof() const;
        virtual bool seekg(FilePos);
        virtual FilePos tellg() const;

        virtual bool ReadRaw(unsigned char* buf, size_t size);

        /// Formal name of the file, e.g. to be displayed in error messages.
        virtual const UCS2 *name() const { return filename.c_str(); };
    private:
        unsigned char* buffer;
        size_t bufferoffset;
        size_t maxbufferoffset;
        POV_ULONG mFormalStart;
        UCS2String filename;
        int ungetbuffer;
        bool fail;
};

class OTextStream
{
    public:
        OTextStream(const UCS2 *, unsigned int, bool append = false);

        /// @param[in]  formalName  Name by which the file is known to the user.
        /// @param[in]  byteStream  Underlying byte-oriented stream to write to.
        OTextStream(const UCS2 *formalName, OStream *);

        virtual ~OTextStream();

        void putchar(int);
        void putraw(int);
        void printf(const char *, ...);
    void flush();

        /// Formal name of the file, e.g. to be displayed in error messages.
        const UCS2 *name() const { return filename.c_str(); };
    private:
        OStream *stream;
        UCS2String filename;
};

}

#endif
