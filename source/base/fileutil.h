//******************************************************************************
///
/// @file base/fileutil.h
///
/// Declarations related to file handling.
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

#ifndef POVRAY_BASE_FILEUTIL_H
#define POVRAY_BASE_FILEUTIL_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// POV-Ray base header files
#include "base/fileinputoutput.h"
#include "base/stringutilities.h"

namespace pov_base
{

class IMemStream;
class IStream;

// Legal image file attributes.

#define NO_FILE         0x00000000
#define GIF_FILE        0x00000001
#define POT_FILE        0x00000002
#define SYS_FILE        0x00000004
#define IFF_FILE        0x00000008
#define TGA_FILE        0x00000010
#define GRAD_FILE       0x00000020
#define PGM_FILE        0x00000040
#define PPM_FILE        0x00000080
#define PNG_FILE        0x00000100
#define JPEG_FILE       0x00000200
#define TIFF_FILE       0x00000400
#define BMP_FILE        0x00000800
#define EXR_FILE        0x00001000
#define HDR_FILE        0x00002000

#define POV_FILE_EXTENSIONS_PER_TYPE 4

struct POV_File_Extensions
{
    const char *ext[POV_FILE_EXTENSIONS_PER_TYPE];
};

extern POV_File_Extensions gPOV_File_Extensions[];
extern const int gFile_Type_To_Mask[];

int InferFileTypeFromExt(const pov_base::UCS2String& ext);
IMemStream *Internal_Font_File(int font_id);

}

#endif // POVRAY_BASE_FILEUTIL_H
