//******************************************************************************
///
/// @file base/fileinputoutput.cpp
///
/// This module implements the classes handling file input and output.
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
//*******************************************************************************

#include <cstdlib>
#include <cstdarg>
#include <cstring>

#include <memory>

// configbase.h must always be the first POV file included within base *.cpp files
#include "base/configbase.h"
#include "base/fileinputoutput.h"

#include "base/stringutilities.h"
#include "base/platformbase.h"
#include "base/pov_err.h"

// All the builtin fonts must be declared here
#include "base/font/crystal.h"
#include "base/font/cyrvetic.h"
#include "base/font/povlogo.h"
#include "base/font/timrom.h"

// this must be the last file included
#include "base/povdebug.h"


namespace pov_base
{

IOBase::IOBase(unsigned int dir, unsigned int type)
{
    filetype = type;
    direction = dir;
    fail = true;
    f = NULL;
}

IOBase::~IOBase()
{
    close();
}

bool IOBase::open(const UCS2String& name, unsigned int Flags /* = 0 */)
{
    char mode[8];

    close();
    filename = name;

    if((Flags & append) == 0)
    {
        switch(direction)
        {
            case input:
                strcpy(mode, "r");
                break;
            case output:
                strcpy(mode, "w");
                break;
            case io:
                strcpy(mode, "w+");
                break;
            default:
                return false;
        }
    }
    else
    {
        // we cannot use append mode here, since "a" mode is totally incompatible with any
        // output file format that requires in-place updates(i.e. writing to any location
        // other than the end of the file). BMP files are in this category. In theory, "r+"
        // can do anything "a" can do(with appropriate use of seek()) so append mode should
        // not be needed.
        strcpy(mode, "r+");
    }

    strcat(mode, "b");

    f = NULL;
    if(pov_stricmp(UCS2toASCIIString(name).c_str(), "stdin") == 0)
    {
        if(direction != input ||(Flags & append) != 0)
            return false;
        f = stdin;
    }
    else if(pov_stricmp(UCS2toASCIIString(name).c_str(), "stdout") == 0)
    {
        if(direction != output ||(Flags & append) != 0)
            return false;
        f = stdout;
    }
    else if(pov_stricmp(UCS2toASCIIString(name).c_str(), "stderr") == 0)
    {
        if(direction != output ||(Flags & append) != 0)
            return false;
        f = stderr;
    }
    else
    {
        if((f = POV_UCS2_FOPEN(name, mode)) == NULL)
        {
            if((Flags & append) == 0)
                return false;

            // to maintain traditional POV +c(continue) mode compatibility, if
            // the open for append of an existing file fails, we allow a new file
            // to be created.
            mode [0] = 'w';
            if((f = POV_UCS2_FOPEN(name, mode)) == NULL)
                return false;
        }
    }
    fail = false;

    if((Flags & append) != 0)
    {
        if(!seekg(0, seek_end))
        {
            close();
            return false;
        }
    }

    return true;
}

bool IOBase::close(void)
{
    if(f != NULL)
    {
        if (f != stdout && f != stderr && f != stdin)
            fclose(f);
        f = NULL;
    }
    fail = true;
    return true;
}

IOBase& IOBase::flush(void)
{
    if(f != NULL)
        fflush(f);
    return *this;
}

IOBase& IOBase::read(void *buffer, size_t count)
{
    if(!fail && count > 0)
        fail = fread(buffer, count, 1, f) != 1;
    return *this;
}

IOBase& IOBase::write(const void *buffer, size_t count)
{
    if(!fail && count > 0)
        fail = fwrite(buffer, count, 1, f) != 1;
    return *this;
}

// Strictly speaking, this should -not- be called seekg, since 'seekg'(an iostreams
// term) applies only to an input stream, and therefore the use of this name here
// implies that only the input stream will be affected on streams opened for I/O
//(which is not the case with fseek, since fseek moves the pointer for output too).
// However, the macintosh code seems to need it to be called seekg, so it is ...
IOBase& IOBase::seekg(POV_LONG pos, unsigned int whence /* = seek_set */)
{
    if(!fail)
        fail = fseek(f, pos, whence) != 0;
    return *this;
}

IStream::IStream(const unsigned int stype) : IOBase(input, stype)
{
}

IStream::~IStream()
{
}

int IStream::Read_Short(void)
{
    short result;
    read(&result, sizeof(short));
    return result;
}

int IStream::Read_Int(void)
{
    int result;
    read(&result, sizeof(int));
    return result;
}

IStream& IStream::UnRead_Byte(int c)
{
    if(!fail)
        fail = ungetc(c, f) != c;
    return *this;
}

IStream& IStream::getline(char *s, size_t buflen)
{
    int chr = 0;

    if(feof(f) != 0)
        fail = true;

    if(!fail && buflen > 0)
    {
        while(buflen > 1)
        {
            chr = fgetc(f);
            if(chr == EOF)
                break;
            else if(chr == 10)
            {
                chr = fgetc(f);
                if(chr != 13)
                    ungetc(chr, f);
                break;
            }
            else if(chr == 13)
            {
                chr = fgetc(f);
                if(chr != 10)
                    ungetc(chr, f);
                break;
            }
            *s = chr;
            s++;
            buflen--;
        }
        *s = 0;
    }

    return *this;
}

/*
 * Default to povlogo.ttf (0)
 * 1 : TimeRoman (timrom.ttf), Serif
 * 2 : Cyrvetita (cyrvetic.ttf), Sans-Serif
 * 3 : Crystal (crystal.ttf), monospace sans serif
 *
 * To add a font, check first its license
 */
IMemStream::IMemStream(const int font_id):IStream(POV_File_Font_TTF)
{
    switch(font_id)
    {
        case 1:
            start = &font_timrom[0];
            size = sizeof(font_timrom);
            break;
        case 2:
            start = &font_cyrvetic[0];
            size = sizeof(font_cyrvetic);
            break;
        case 3:
            start = &font_crystal[0];
            size = sizeof(font_crystal);
            break;
        default:
            start = &font_povlogo[0];
            size = sizeof(font_povlogo);
            break;
    }
    pos = 0;
    fail= false;
}

IMemStream::~IMemStream()
{
// [jg] more to do here  (?)
}

OStream::OStream(const unsigned int stype) : IOBase(output, stype)
{
}

OStream::~OStream()
{
}

void OStream::printf(const char *format, ...)
{
    va_list marker;
    char buffer[1024];

    va_start(marker, format);
    vsnprintf(buffer, 1023, format, marker);
    va_end(marker);

    *this << buffer;
}

IStream *NewIStream(const Path& p, const unsigned int stype)
{
    std::auto_ptr<IStream> istreamptr(POV_PLATFORM_BASE.CreateIStream(stype));

    if(istreamptr.get() == NULL)
        return NULL;

    if (POV_ALLOW_FILE_READ(p().c_str(), stype) == false) // TODO FIXME - this is handled by the frontend, but that code isn't completely there yet [trf]
    {
        string str ("IO Restrictions prohibit read access to '") ;
        str += UCS2toASCIIString(p());
        str += "'";
        throw POV_EXCEPTION(kCannotOpenFileErr, str);
    }
    if(istreamptr->open(p().c_str()) == 0)
        return NULL;

    return istreamptr.release();
}

OStream *NewOStream(const Path& p, const unsigned int stype, const bool sappend)
{
    std::auto_ptr<OStream> ostreamptr(POV_PLATFORM_BASE.CreateOStream(stype));
    unsigned int Flags = IOBase::none;

    if(ostreamptr.get() == NULL)
        return NULL;

    if(sappend)
        Flags |= IOBase::append;

    if (POV_ALLOW_FILE_WRITE(p().c_str(), stype) == false) // TODO FIXME - this is handled by the frontend, but that code isn't completely there yet [trf]
    {
        string str ("IO Restrictions prohibit write access to '") ;
        str += UCS2toASCIIString(p());
        str += "'";
        throw POV_EXCEPTION(kCannotOpenFileErr, str);
    }
    if(ostreamptr->open(p().c_str(), Flags) == 0)
        return NULL;

    return ostreamptr.release();
}

UCS2String GetFileExtension(const Path& p)
{
    UCS2String::size_type pos = p.GetFile().find_last_of('.');

    if(pos != UCS2String::npos)
        return UCS2String(p.GetFile(), pos);

    return UCS2String();
}

UCS2String GetFileName(const Path& p)
{
    UCS2String::size_type pos = p.GetFile().find_last_of('.');

    if(pos != UCS2String::npos)
        return UCS2String(p.GetFile(), 0, pos);

    return p.GetFile();
}

bool CheckIfFileExists(const Path& p)
{
    FILE *tempf = POV_UCS2_FOPEN(p().c_str(), "r");

    if(tempf != NULL)
        fclose(tempf);
    else
        return false;

    return true;
}

POV_LONG GetFileLength(const Path& p)
{
    FILE *tempf = POV_UCS2_FOPEN(p().c_str(), "rb");
    POV_LONG result = -1;

    if(tempf != NULL)
    {
        fseek(tempf, 0, SEEK_END);
        result = ftell(tempf);
        fclose(tempf);
    }

    return result;
}

IOBase& IMemStream::read(void *buffer, size_t count)
{
    if ((!fail)&&(pos+count<= size))
    {
        memcpy(buffer,&start[pos],count);
        pos+= count;
    }
    else
    {
        fail = true;
    }
    return *this;
}

int IMemStream::Read_Byte()
{
    int v;
    if (fail)
    {
        v = EOF;
    }
    else
    {
        v = start[pos++];
        fail = !(pos<size);
    }
    return v;
}

IStream& IMemStream::UnRead_Byte(int c)
{
    pos--;
    fail = !(pos<size);
    return *this;
}

IStream& IMemStream::getline(char *s,size_t buflen)
{
    // Not needed for font
    return *this;
}

POV_LONG IMemStream::tellg()
{
  return pos;
}

IOBase& IMemStream::seekg(POV_LONG posi, unsigned int whence)
{
    switch(whence)
    {
        case seek_set:
            pos = posi;
            break;
        case seek_cur:
            pos += posi;
            break;
        case seek_end:
            pos = size - posi;
            break;
    }
    fail = !(pos<size);
    return *this;
}

bool IMemStream::open(const UCS2String &name, unsigned int Flags)
{
    // Not needed for font
    return true;
}

bool IMemStream::close()
{
    // Not needed for font
    return true;
}

}
