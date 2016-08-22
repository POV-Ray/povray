//******************************************************************************
///
/// @file base/fileinputoutput.cpp
///
/// Implementations related to basic file input and output.
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
#include "base/fileinputoutput.h"

// C++ variants of standard C header files
#include <cstdarg>
#include <cstdlib>

// Standard C++ header files
#include <memory>

// POV-Ray base header files
#include "base/platformbase.h"
#include "base/pov_err.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

IOBase::IOBase() : filename(), fail(true)
{
}

IOBase::IOBase(const UCS2String& name) : filename(name), fail(true)
{
}

IOBase::~IOBase()
{
}

IStream::IStream() : IOBase()
{
}

IStream::IStream(const UCS2String& name) : IOBase(name)
{
}

IStream::~IStream()
{
}

IFileStream::IFileStream(const UCS2String& name) : IStream(name), f(NULL)
{
    if(pov_stricmp(UCS2toASCIIString(name).c_str(), "stdin") == 0)
    {
        f = stdin;
    }
    else if((pov_stricmp(UCS2toASCIIString(name).c_str(), "stdout") == 0) ||
            (pov_stricmp(UCS2toASCIIString(name).c_str(), "stderr") == 0))
    {
        f = NULL;
    }
    else
    {
        f = POV_UCS2_FOPEN(name, "rb");
    }
    fail = (f == NULL);
}

IFileStream::~IFileStream()
{
    if(f != NULL)
    {
        if (f != stdout && f != stderr && f != stdin)
            fclose(f);
    }
}

bool IFileStream::UnRead_Byte(int c)
{
    if(!fail)
        fail = ungetc(c, f) != c;
    return !fail;
}

// Strictly speaking, this should -not- be called seekg, since 'seekg'(an iostreams
// term) applies only to an input stream, and therefore the use of this name here
// implies that only the input stream will be affected on streams opened for I/O
// (which is not the case with fseek, since fseek moves the pointer for output too).
// However, the macintosh code seems to need it to be called seekg, so it is ...
bool IFileStream::seekg(POV_LONG pos, unsigned int whence)
{
    if(!fail)
        fail = fseek(f, pos, whence) != 0;
    return !fail;
}

bool IFileStream::read(void *buffer, size_t count)
{
    if(!fail && count > 0)
        fail = fread(buffer, count, 1, f) != 1;
    return !fail;
}

int IFileStream::Read_Byte()
{
    return(fail ? EOF : fgetc(f));
}

bool IFileStream::getline(char *s, size_t buflen)
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

    return !fail;
}

IMemStream::IMemStream(const unsigned char* data, size_t size, const char* formalName, POV_LONG formalStart) :
    IStream(ASCIItoUCS2String(formalName)), size(size), pos(0), start(data), formalStart(formalStart)
{
    fail = false;
}

IMemStream::IMemStream(const unsigned char* data, size_t size, const UCS2String& formalName, POV_LONG formalStart) :
    IStream(formalName), size(size), pos(0), start(data), formalStart(formalStart)
{
    fail = false;
}

IMemStream::~IMemStream()
{
}

OStream::OStream(const UCS2String& name, unsigned int Flags) : IOBase(name), f(NULL)
{
    const char* mode;

    if((Flags & append) == 0)
    {
        mode = "wb";
    }
    else
    {
        // we cannot use append mode here, since "a" mode is totally incompatible with any
        // output file format that requires in-place updates(i.e. writing to any location
        // other than the end of the file). BMP files are in this category. In theory, "r+"
        // can do anything "a" can do(with appropriate use of seek()) so append mode should
        // not be needed.
        mode = "r+b";
    }

    if(pov_stricmp(UCS2toASCIIString(name).c_str(), "stdin") == 0)
    {
        f = NULL;
    }
    else if(pov_stricmp(UCS2toASCIIString(name).c_str(), "stdout") == 0)
    {
        if((Flags & append) != 0)
            f = NULL;
        else
            f = stdout;
    }
    else if(pov_stricmp(UCS2toASCIIString(name).c_str(), "stderr") == 0)
    {
        if((Flags & append) != 0)
            f = NULL;
        else
            f = stdout;
    }
    else
    {
        f = POV_UCS2_FOPEN(name, mode);
        if (f == NULL)
        {
            if((Flags & append) == 0)
                f = NULL;
            else
            {
                // to maintain traditional POV +c(continue) mode compatibility, if
                // the open for append of an existing file fails, we allow a new file
                // to be created.
                mode = "wb";
                f = POV_UCS2_FOPEN(name, mode);
            }
        }

        if (f != NULL)
        {
            if((Flags & append) != 0)
            {
                if(!seekg(0, seek_end))
                {
                    fclose(f);
                    f = NULL;
                }
            }
        }
    }

    fail = (f == NULL);
}

OStream::~OStream()
{
    if(f != NULL)
    {
        if (f != stdout && f != stderr && f != stdin)
            fclose(f);
    }
}

OStream& OStream::flush()
{
    if(f != NULL)
        fflush(f);
    return *this;
}

// Strictly speaking, this should -not- be called seekg, since 'seekg'(an iostreams
// term) applies only to an input stream, and therefore the use of this name here
// implies that only the input stream will be affected on streams opened for I/O
// (which is not the case with fseek, since fseek moves the pointer for output too).
// However, the macintosh code seems to need it to be called seekg, so it is ...
bool OStream::seekg(POV_LONG pos, unsigned int whence /* = seek_set */)
{
    if(!fail)
        fail = fseek(f, pos, whence) != 0;
    return !fail;
}

bool OStream::write(const void *buffer, size_t count)
{
    if(!fail && count > 0)
        fail = fwrite(buffer, count, 1, f) != 1;
    return !fail;
}

void OStream::printf(const char *format, ...)
{
    va_list marker;
    char buffer[1024];

    va_start(marker, format);
    vsnprintf(buffer, 1023, format, marker);
    va_end(marker);

    write(reinterpret_cast<const void *>(buffer), strlen(buffer));
}

IStream *NewIStream(const Path& p, unsigned int stype)
{
    if (POV_ALLOW_FILE_READ(p().c_str(), stype) == false) // TODO FIXME - this is handled by the frontend, but that code isn't completely there yet [trf]
    {
        string str ("IO Restrictions prohibit read access to '") ;
        str += UCS2toASCIIString(p());
        str += "'";
        throw POV_EXCEPTION(kCannotOpenFileErr, str);
    }

    return new IFileStream(p().c_str());
}

OStream *NewOStream(const Path& p, unsigned int stype, bool sappend)
{
    unsigned int Flags = IOBase::none;

    if(sappend)
        Flags |= IOBase::append;

    if (POV_ALLOW_FILE_WRITE(p().c_str(), stype) == false) // TODO FIXME - this is handled by the frontend, but that code isn't completely there yet [trf]
    {
        string str ("IO Restrictions prohibit write access to '") ;
        str += UCS2toASCIIString(p());
        str += "'";
        throw POV_EXCEPTION(kCannotOpenFileErr, str);
    }

    return new OStream(p().c_str(), Flags);
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

bool IMemStream::read(void *buffer, size_t maxCount)
{
    size_t count = 0;

    if (fail)
        return false;

    unsigned char* p = reinterpret_cast<unsigned char*>(buffer);

    if (maxCount == 0)
        return true;

    // read from unget buffer first
    if (mUngetBuffer != EOF)
    {
        *(p++) = (unsigned char)mUngetBuffer;
        mUngetBuffer = EOF;
        if (++count == maxCount)
            return true;
    }

    size_t copyFromBuffer = min(maxCount-count, size-pos);
    memcpy(p, &(start[pos]), copyFromBuffer);
    count += copyFromBuffer;
    pos += copyFromBuffer;
    if (count == maxCount)
        return true;

    fail = true;
    return false;
}

int IMemStream::Read_Byte()
{
    int v;
    if (fail)
    {
        v = EOF;
    }
    else if (mUngetBuffer != EOF)
    {
        v = mUngetBuffer;
        mUngetBuffer = EOF;
    }
    else
    {
        if (pos < size)
            v = start[pos++];
        else
            fail = true;
    }
    return v;
}

bool IMemStream::UnRead_Byte(int c)
{
    if (fail)
        return false;

    mUngetBuffer = c;

    return true;
}

bool IMemStream::getline(char *s,size_t buflen)
{
    // Not needed for inbuilt fonts or scene file caching
    throw POV_EXCEPTION_CODE(kParamErr);
    return !fail;
}

POV_LONG IMemStream::tellg() const
{
    size_t physicalPos = pos;
    if (mUngetBuffer != EOF)
        ++physicalPos;
    return formalStart + physicalPos;
}

bool IMemStream::seekg(POV_LONG posi, unsigned int whence)
{
    if(!fail)
    {
        // Any seek operation renders the unget buffer's content obsolete.
        mUngetBuffer = EOF;

        switch(whence)
        {
            case seek_set:
                if (posi < formalStart)
                    fail = true;
                else if (posi - formalStart <= size)
                    pos = posi - formalStart;
                else
                    fail = true;
                break;
            case seek_cur:
                if ((posi <= size) && (pos <= size-posi))
                    pos += posi;
                else
                    fail = true;
                break;
            case seek_end:
                if (posi <= size)
                    pos = size - posi;
                else
                    fail = true;
                break;
            default:
                POV_ASSERT(false);
                break;
        }
    }
    return !fail;
}

}
