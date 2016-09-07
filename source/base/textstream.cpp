//******************************************************************************
///
/// @file base/textstream.cpp
///
/// Implementations related to text file input and output.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "textstream.h"

// C++ variants of standard C header files
#include <cstdarg>
#include <cstdlib>

// Standard C++ header files
#include <algorithm>

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

ITextStream::ITextStream() :
    lineno(1)
{
}

ITextStream::~ITextStream()
{
}

IBufferedTextStream::IBufferedTextStream(const UCS2 *sname, unsigned int stype)
{
    if(sname == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);

    stream = NewIStream(sname, stype);
    if(stream == NULL)
        throw POV_EXCEPTION(kCannotOpenFileErr, string("Cannot open file '") + UCS2toASCIIString(sname) + "' for input.");

    filename = UCS2String(sname);
    bufferoffset = 0;
    maxbufferoffset = 0;
    filelength = 0;
    ungetbuffer = EOF;
    curpos = 0 ;

    stream->seekg(0, IOBase::seek_end);
    filelength = stream->tellg();
    stream->seekg(0, IOBase::seek_set);

    RefillBuffer();
}

IBufferedTextStream::IBufferedTextStream(const UCS2 *sname, IStream *sstream, POV_LONG initialLine)
{
    if(sname == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);
    if(sstream == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);

    stream = sstream;
    filename = UCS2String(sname);
    lineno = initialLine;
    bufferoffset = 0;
    maxbufferoffset = 0;
    filelength = 0;
    ungetbuffer = EOF;
    curpos = stream->tellg();

    stream->seekg(0, IOBase::seek_end);
    filelength = stream->tellg();
    stream->seekg(curpos, IOBase::seek_set);

    RefillBuffer();
}

IBufferedTextStream::~IBufferedTextStream()
{
    delete stream;
    stream = NULL;
}

int IBufferedTextStream::getchar()
{
    int chr = 0;

    if(ungetbuffer != EOF)
    {
        chr = ungetbuffer;
        ungetbuffer = EOF;
    }
    else
    {
        if(bufferoffset >= maxbufferoffset)
            chr = EOF;
        else
        {
            chr = buffer[bufferoffset];
            bufferoffset++;
        }
    }

    if(((chr == 10) || (chr == 13)) && (bufferoffset >= maxbufferoffset))
        RefillBuffer();

    if(chr == 10)
    {
        if(buffer[bufferoffset] == 13)
            bufferoffset++;
        chr = '\n';
        lineno++;
    }
    else if(chr == 13)
    {
        if(buffer[bufferoffset] == 10)
            bufferoffset++;
        chr = '\n';
        lineno++;
    }

    if(bufferoffset >= maxbufferoffset)
        RefillBuffer();

    return chr;
}

void IBufferedTextStream::ungetchar(int chr)
{
    ungetbuffer = chr;
    if(chr == '\n')
        lineno--;
}

bool IBufferedTextStream::eof() const
{
    if(ungetbuffer != EOF)
        return false;
    if(bufferoffset >= maxbufferoffset)
        // NB this relies on RefillBuffer being called by each read operation that exhausts the buffer
        return true;
    return stream->eof();
}

bool IBufferedTextStream::seekg(ITextStream::FilePos fp)
{
    bool result = true;

    if((fp.offset < curpos) && ((curpos - fp.offset) <= maxbufferoffset))
    {
        bufferoffset = maxbufferoffset - (curpos - fp.offset);
        lineno = fp.lineno;
        ungetbuffer = EOF;
    }
    else
    {
        result = (stream->seekg(fp.offset) != 0);

        if(result == true)
        {
            lineno = fp.lineno;

            bufferoffset = 0;
            maxbufferoffset = 0;
            ungetbuffer = EOF;
            curpos = fp.offset ;

            RefillBuffer();
        }
        else
            curpos = stream->tellg() ;
    }

    return result;
}

ITextStream::FilePos IBufferedTextStream::tellg() const
{
    FilePos fp;

    fp.lineno = lineno;
    fp.offset = curpos - (maxbufferoffset - bufferoffset);

    if(ungetbuffer != EOF)
        fp.offset--;

    return fp;
}

bool IBufferedTextStream::ReadRaw(unsigned char* buf, size_t size)
{
    unsigned char* p = buf;
    size_t remain = size;

    if (remain == 0)
        return true;

    // read from unget buffer first
    if (ungetbuffer != EOF)
    {
        *(p++) = (unsigned char)ungetbuffer;
        ungetbuffer = EOF;
        if (--remain == 0)
            return true;
    }

    // next read from the regular buffer
    if (bufferoffset < maxbufferoffset)
    {
        size_t copyFromBuffer = min(remain, size_t(maxbufferoffset - bufferoffset));
        memcpy(p, &(buffer[bufferoffset]), copyFromBuffer);
        remain -= copyFromBuffer;
        bufferoffset += copyFromBuffer;
        p += copyFromBuffer;
        if (remain == 0)
            return true;
    }

    // if all buffers are depleted, read directly from stream
    if (*stream)
    {
        if (stream->read(p, remain))
        {
            curpos += remain;
            return true;
        }
        else
            curpos = stream->tellg();
    }

    return false;
}

void IBufferedTextStream::RefillBuffer()
{
    if(bufferoffset < maxbufferoffset)
    {
        curpos -= (maxbufferoffset - bufferoffset);
        stream->seekg(curpos, IOBase::seek_set);
    }

    maxbufferoffset = min((POV_ULONG)ITEXTSTREAM_BUFFER_SIZE, filelength - curpos);
    bufferoffset = 0;

    stream->read(buffer, maxbufferoffset);
    if (*stream)
        curpos += maxbufferoffset ;
    else
        curpos = stream->tellg() ;
}

IMemTextStream::IMemTextStream(const UCS2 *formalName, unsigned char* data, size_t size, const FilePos& formalStart)
{
    if(formalName == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);
    if(data == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);

    buffer = data;
    filename = UCS2String(formalName);
    lineno = formalStart.lineno;
    bufferoffset = 0;
    maxbufferoffset = size;
    ungetbuffer = EOF;
    mFormalStart = formalStart.offset;
    fail = false;
}

IMemTextStream::~IMemTextStream()
{
}

int IMemTextStream::getchar()
{
    int chr = 0;

    if(ungetbuffer != EOF)
    {
        chr = ungetbuffer;
        ungetbuffer = EOF;
    }
    else
    {
        if(bufferoffset >= maxbufferoffset)
        {
            chr = EOF;
            fail = true;
        }
        else
        {
            chr = buffer[bufferoffset];
            bufferoffset++;
        }
    }

    if(chr == 10)
    {
        if((bufferoffset < maxbufferoffset) && (buffer[bufferoffset] == 13))
            bufferoffset++;
        chr = '\n';
        lineno++;
    }
    else if(chr == 13)
    {
        if((bufferoffset < maxbufferoffset) && (buffer[bufferoffset] == 10))
            bufferoffset++;
        chr = '\n';
        lineno++;
    }

    return chr;
}

void IMemTextStream::ungetchar(int chr)
{
    ungetbuffer = chr;
    if(chr == '\n')
        lineno--;
}

bool IMemTextStream::eof() const
{
    if(ungetbuffer != EOF)
        return false;
    return fail;
}

bool IMemTextStream::seekg(ITextStream::FilePos fp)
{
    fail = false;

    if((fp.offset < mFormalStart + maxbufferoffset) && (mFormalStart <= fp.offset))
    {
        bufferoffset = fp.offset - mFormalStart;
        lineno = fp.lineno;
        ungetbuffer = EOF;
    }
    else
    {
        fail = true;
    }

    return !fail;
}

ITextStream::FilePos IMemTextStream::tellg() const
{
    FilePos fp;

    fp.lineno = lineno;
    fp.offset = mFormalStart + bufferoffset;

    if(ungetbuffer != EOF)
        fp.offset--;

    return fp;
}

bool IMemTextStream::ReadRaw(unsigned char* buf, size_t size)
{
    if (fail)
        return false;

    unsigned char* p = buf;
    size_t remain = size;

    if (remain == 0)
        return true;

    // read from unget buffer first
    if (ungetbuffer != EOF)
    {
        *(p++) = (unsigned char)ungetbuffer;
        ungetbuffer = EOF;
        if (--remain == 0)
            return true;
    }

    // next read from the regular buffer
    if (bufferoffset < maxbufferoffset)
    {
        size_t copyFromBuffer = min(remain, size_t(maxbufferoffset - bufferoffset));
        memcpy(p, &(buffer[bufferoffset]), copyFromBuffer);
        remain -= copyFromBuffer;
        bufferoffset += copyFromBuffer;
        p += copyFromBuffer;
        if (remain == 0)
            return true;
    }

    return false;
}

OTextStream::OTextStream(const UCS2 *sname, unsigned int stype, bool append)
{
    if(sname == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);

    stream = NewOStream(sname, stype, append);
    if(stream == NULL)
        throw POV_EXCEPTION(kCannotOpenFileErr, string("Cannot open file '") + UCS2toASCIIString(sname) + "' for output.");

    filename = UCS2String(sname);
}

OTextStream::OTextStream(const UCS2 *sname, OStream *sstream)
{
    if(sname == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);
    if(sstream == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);

    stream = sstream;
    filename = UCS2String(sname);
}

OTextStream::~OTextStream()
{
    delete stream;
    stream = NULL;
}

void OTextStream::putchar(int chr)
{
#ifdef NEW_LINE_STRING
    if (chr == '\n')
    {
        for (char* c = NEW_LINE_STRING; c != '\0'; ++c)
            stream->Write_Byte(*c);
    }
#endif

    stream->Write_Byte((unsigned char)chr);
}

void OTextStream::putraw(int chr)
{
    stream->Write_Byte((unsigned char)chr);
}

void OTextStream::printf(const char *format, ...)
{
    va_list marker;
    char buffer[1024];

    va_start(marker, format);
    vsnprintf(buffer, 1023, format, marker);
    va_end(marker);

#ifdef NEW_LINE_STRING
    char *s1 = buffer ;
    char *s2 ;

    while ((s2 = strchr (s1, '\n')) != NULL)
    {
        *s2++ = '\0' ;
        stream->printf("%s" NEW_LINE_STRING, s1);
        s1 = s2 ;
    }
    if (*s1)
        stream->printf("%s", s1);
#else
    stream->printf("%s", buffer);
#endif
}

void OTextStream::flush()
{
    stream->flush();
}

}
