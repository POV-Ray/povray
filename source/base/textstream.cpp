//******************************************************************************
///
/// @file base/textstream.cpp
///
/// This module implements the classes handling text file input and output.
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

#include <cstdlib>
#include <cstdarg>
#include <algorithm>

// configbase.h must always be the first POV file included within base *.cpp files
#include "configbase.h"

#include "textstream.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

ITextStream::ITextStream(const UCS2 *sname, unsigned int stype)
{
    if(sname == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);

    stream = NewIStream(sname, stype);
    if(stream == NULL)
        throw POV_EXCEPTION(kCannotOpenFileErr, string("Cannot open file '") + UCS2toASCIIString(sname) + "' for input.");

    filename = UCS2String(sname);
    lineno = 1;
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

ITextStream::ITextStream(const UCS2 *sname, IStream *sstream)
{
    if(sname == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);
    if(sstream == NULL)
        throw POV_EXCEPTION_CODE(kParamErr);

    stream = sstream;
    filename = UCS2String(sname);
    lineno = 1;
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

ITextStream::~ITextStream()
{
    delete stream;
    stream = NULL;
}

int ITextStream::getchar()
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

void ITextStream::ungetchar(int chr)
{
    ungetbuffer = chr;
    if(chr == '\n')
        lineno--;
}

bool ITextStream::eof() const
{
    if(ungetbuffer != EOF)
        return false;
    if(bufferoffset >= maxbufferoffset)
        return true;
    return stream->eof();
}

bool ITextStream::seekg(ITextStream::FilePos fp)
{
    bool result = true;

    if((fp.offset < curpos) && ((curpos - fp.offset) < maxbufferoffset))
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

ITextStream::FilePos ITextStream::tellg() const
{
    FilePos fp;

    fp.lineno = lineno;
    fp.offset = curpos - (maxbufferoffset - bufferoffset);

    if(ungetbuffer != EOF)
        fp.offset--;

    return fp;
}

void ITextStream::RefillBuffer()
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
#ifdef TEXTSTREAM_CRLF
    if (chr == '\n')
        stream->Write_Byte('\r');
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

#ifdef TEXTSTREAM_CRLF
    char *s1 = buffer ;
    char *s2 ;

    while ((s2 = strchr (s1, '\n')) != NULL)
    {
        *s2++ = '\0' ;
        stream->printf("%s\r\n", s1);
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
