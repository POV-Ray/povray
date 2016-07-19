//******************************************************************************
///
/// @file base/fileinputoutput.h
///
/// Declarations related to basic file input and output.
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

#ifndef POVRAY_BASE_FILEINPUTOUTPUT_H
#define POVRAY_BASE_FILEINPUTOUTPUT_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of standard C header files
#include <cstring>

// POV-Ray base header files
#include "base/path.h"
#include "base/stringutilities.h"
#include "base/types.h"

namespace pov_base
{

// NOTE: Remember to modify gPOV_File_Extensions in fileinputoutput.cpp!
enum
{
    POV_File_Unknown = 0,
    POV_File_Image_Targa,
    POV_File_Image_PNG,
    POV_File_Image_PPM,
    POV_File_Image_PGM,
    POV_File_Image_GIF,
    POV_File_Image_IFF,
    POV_File_Image_JPEG,
    POV_File_Image_TIFF,
    POV_File_Image_BMP,
    POV_File_Image_EXR,
    POV_File_Image_HDR,
    POV_File_Image_System,
    POV_File_Text_POV,
    POV_File_Text_INC,
    POV_File_Text_Macro = POV_File_Text_INC,
    POV_File_Text_INI,
    POV_File_Text_CSV,
    POV_File_Text_Stream,
    POV_File_Text_User,
    POV_File_Data_DF3,
    POV_File_Data_RCA,
    POV_File_Data_LOG,
    POV_File_Data_Backup,
    POV_File_Font_TTF,
    POV_File_Count
};

class IOBase
{
    public:
        IOBase();
        IOBase(const UCS2String& name);
        virtual ~IOBase();

        enum { none = 0, append = 1 };
        enum { seek_set = SEEK_SET, seek_cur = SEEK_CUR, seek_end = SEEK_END };

        virtual bool seekg(POV_LONG pos, unsigned int whence = seek_set) = 0;

        virtual bool eof() const = 0;
        virtual POV_LONG tellg() const = 0;
        virtual bool clearstate() = 0;
        inline const UCS2 *Name() const { return(filename.c_str()); }

        inline operator const void *() const { return(fail ? 0 :reinterpret_cast<const void *>(this)); }
        inline bool operator!() const { return(fail); }

    protected:
        bool fail;
        UCS2String filename;
};

class IStream : public IOBase
{
    public:
        IStream();
        IStream(const UCS2String& name);
        virtual ~IStream();

        virtual bool read(void *buffer, size_t count) = 0;

        virtual int Read_Byte() = 0;
        inline bool Read_Byte(char& c) { c =(char) Read_Byte(); return !fail; }
        inline bool Read_Byte(unsigned char& c) { c =(unsigned char) Read_Byte(); return !fail; }

        /// Step back in the stream by a single byte.
        /// @attention
        ///     Derived classes may rely on this function to be called at most once between consecutive read accesses.
        /// @attention
        ///     Derived classes may rely on the supplied parameter value to be identical to the last byte actually read.
        virtual bool UnRead_Byte(int c) = 0;

        virtual bool getline(char *s, size_t buflen) = 0;
        inline bool ignore(POV_LONG count) { return seekg(count, seek_cur); }
};

/// File-backed input stream.
///
/// This class provides file read access.
///
class IFileStream : public IStream
{
    public:
        IFileStream(const UCS2String& name);
        virtual ~IFileStream();

        virtual bool eof() const { return(fail ? true : feof(f) != 0); }
        virtual bool seekg(POV_LONG pos, unsigned int whence = seek_set);
        virtual POV_LONG tellg() const { return(f == NULL ? -1 : ftell(f)); }
        virtual bool clearstate() { if(f != NULL) fail = false; return !fail; }

        virtual bool read(void *buffer, size_t count);
        virtual bool getline(char *s, size_t buflen);
        virtual int Read_Byte();

        virtual bool UnRead_Byte(int c);

    protected:
        FILE *f;
};

/// Memory-backed input stream.
///
/// This class provides read access to in-memory data in a manner compatible with regular input file
/// access.
///
/// This class is used to support in-built fonts and cached macros.
///
class IMemStream : public IStream
{
    public:
        IMemStream(const unsigned char* data, size_t size, const char* formalName, POV_LONG formalStart = 0);
        IMemStream(const unsigned char* data, size_t size, const UCS2String& formalName, POV_LONG formalStart = 0);
        virtual ~IMemStream();

        virtual int Read_Byte();
        virtual bool UnRead_Byte(int c);
        virtual bool getline(char *s, size_t buflen);
        virtual POV_LONG tellg() const;
        virtual bool read(void *buffer, size_t count);
        virtual bool seekg(POV_LONG pos, unsigned int whence = seek_set);
        virtual bool clearstate() { fail = false; return true; }

        virtual bool eof() const { return fail; }

    protected:

        size_t size;
        size_t pos;
        POV_LONG formalStart;
        const unsigned char * start;
        int mUngetBuffer;
};

class OStream : public IOBase
{
    public:
        OStream(const UCS2String& name, unsigned int Flags = 0);
        ~OStream();

        virtual bool seekg(POV_LONG pos, unsigned int whence = seek_set);
        virtual POV_LONG tellg() const { return(f == NULL ? -1 : ftell(f)); }
        inline bool clearstate() { if(f != NULL) fail = false; return !fail; }
        virtual bool eof() const { return(fail ? true : feof(f) != 0); }

        bool write(const void *buffer, size_t count);
        void printf(const char *format, ...);
        inline bool Write_Byte(unsigned char data) { if(!fail) fail = fputc(data, f) != data; return !fail; }

        OStream& flush();

    protected:
        FILE *f;
};

IStream *NewIStream(const Path&, unsigned int);
OStream *NewOStream(const Path&, unsigned int, bool);

UCS2String GetFileExtension(const Path& p);
UCS2String GetFileName(const Path& p);

bool CheckIfFileExists(const Path& p);
POV_LONG GetFileLength(const Path& p);

}

#endif // POVRAY_BASE_FILEINPUTOUTPUT_H
