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
        IOBase(unsigned int dir, unsigned int type);
        virtual ~IOBase();

        enum { none = 0, append = 1 };
        enum { input, output, io };
        enum { seek_set = SEEK_SET, seek_cur = SEEK_CUR, seek_end = SEEK_END };

        virtual bool open(const UCS2String& name, unsigned int Flags = 0);
        virtual bool close();
        virtual IOBase& read(void *buffer, size_t count);
        IOBase& write(const void *buffer, size_t count);
        virtual IOBase& seekg(POV_LONG pos, unsigned int whence = seek_set);

        inline unsigned int gettype() const { return(filetype); }
        inline unsigned int getdirection() const { return(direction); }
        inline bool eof() { return(fail ? true : feof(f) != 0); }
        virtual inline POV_LONG tellg() { return(f == NULL ? -1 : ftell(f)); }
        inline IOBase& clearstate() { if(f != NULL) fail = false; return *this; }
        inline const UCS2 *Name() const { return(filename.c_str()); }

        inline operator const void *() const { return(fail ? 0 :reinterpret_cast<const void *>(this)); }
        inline bool operator!() const { return(fail); }
    protected:
        bool fail;
        FILE *f;
        IOBase& flush();
        unsigned int filetype;
        unsigned int direction;
        UCS2String filename;
};

class IStream : public IOBase
{
    public:
        IStream(unsigned int type);
        virtual ~IStream();

        virtual inline int Read_Byte() { return(fail ? EOF : fgetc(f)); }
        int Read_Short();
        int Read_Int();
        inline IStream& Read_Byte(char& c) { c =(char) Read_Byte(); return *this; }
        inline IStream& Read_Byte(unsigned char& c) { c =(unsigned char) Read_Byte(); return *this; }
        /*
        inline IStream& Read_Short(short& n) { n =(short) Read_Short(); return *this; }
        inline IStream& Read_Short(unsigned short& n) { n =(unsigned short) Read_Short(); return *this; }
        inline IStream& Read_Int(int& n) { n = Read_Int(); return *this; }
        inline IStream& Read_Int(unsigned int& n) { n = Read_Int(); return *this; }
        */

        /*
        inline IStream& operator>>(int& n) { read(&n, sizeof(n)); return *this; }
        inline IStream& operator>>(short& n) { read(&n, sizeof(n)); return *this; }
        */
        inline IStream& operator>>(char& n) { read(&n, sizeof(n)); return *this; }
        /*
        inline IStream& operator>>(unsigned int& n) { read(&n, sizeof(n)); return *this; }
        inline IStream& operator>>(unsigned short& n) { read(&n, sizeof(n)); return *this; }
        */
        inline IStream& operator>>(unsigned char& n) { read(&n, sizeof(n)); return *this; }

        /// Step back in the stream by a single byte.
        /// @attention
        ///     Derived classes may rely on this function to be called at most once between consecutive read accesses.
        /// @attention
        ///     Derived classes may rely on the supplied parameter value to be identical to the last byte actually read.
        virtual IStream& UnRead_Byte(int c);

        virtual IStream& getline(char *s, size_t buflen);
        IStream& ignore(POV_LONG count) { seekg(count, seek_cur); return *this; }
};

/// Memory-backed pseudo input file.
///
/// This class provides read access to in-memory data in a manner compatible with regular input file
/// access.
///
/// This class is used to support in-built fonts and cached macros.
///
class IMemStream : public IStream
{
    public:
        IMemStream(unsigned int type, int fileId);
        virtual ~IMemStream();
        virtual int Read_Byte();
        virtual IStream& UnRead_Byte(int c);
        virtual IStream& getline(char *s, size_t buflen);
        virtual POV_LONG tellg();
        virtual IOBase& read(void *buffer, size_t count);
        virtual IOBase& seekg(POV_LONG pos, unsigned int whence = seek_set);

        /// Open the pseudo file.
        /// @note
        ///     This method is provided primarily for compatibility with the @ref IStream interface.
        ///     The pseudo file is always ready for reading; calling this method is purely optional,
        ///     and all it does is reset the pseudo file to the initial state. Any parameters
        ///     supplied are ignored.
        virtual bool open(const UCS2String&, unsigned int = 0);

        /// Close the pseudo file.
        /// @note
        ///     This method is provided for compatibility with the @ref IStream interface only.
        ///     The pseudo file is always ready for reading; calling this method is purely optional,
        ///     and all it does is reset the pseudo file to the initial state.
        virtual bool close();

    protected:

        size_t size;
        size_t pos;
        const unsigned char * start;
};

class OStream : public IOBase
{
    public:
        OStream(unsigned int type);
        ~OStream();

        void printf(const char *format, ...);

        inline OStream& Write_Byte(unsigned char data) { if(!fail) fail = fputc(data, f) != data; return *this; }
/*
        inline OStream& Write_Short(unsigned short data) { write(&data, sizeof(data)); return *this; }
        inline OStream& Write_Int(unsigned int data) { write(&data, sizeof(data)); return *this; }
*/
        inline OStream& flush(void) { IOBase::flush(); return *this; }

        inline OStream& operator<<(const char *s) { write(reinterpret_cast<const void *>(s), (size_t) strlen(s)); return *this; }
        inline OStream& operator<<(const unsigned char *s) { return operator<<(reinterpret_cast<const char *>(s)); }
        inline OStream& operator<<(char c) { return(Write_Byte(c)); }
        inline OStream& operator<<(unsigned char c) { return operator <<((char) c); }
/*
        inline OStream& operator<<(short n) { return(Write_Short(n)); }
        inline OStream& operator<<(unsigned short n) { return operator <<((short) n); }
        inline OStream& operator<<(int n) { return(Write_Int(n)); }
        inline OStream& operator<<(unsigned int n) { return operator <<((int) n); }
*/
};

IStream *NewIStream(const Path&, unsigned int);
OStream *NewOStream(const Path&, unsigned int, bool);

UCS2String GetFileExtension(const Path& p);
UCS2String GetFileName(const Path& p);

bool CheckIfFileExists(const Path& p);
POV_LONG GetFileLength(const Path& p);

}

#endif // POVRAY_BASE_FILEINPUTOUTPUT_H
