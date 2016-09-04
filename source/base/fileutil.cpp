//******************************************************************************
///
/// @file base/fileutil.cpp
///
/// Implementations related to file handling.
///
/// @todo   Describe more precisely.
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
#include "base/fileutil.h"

// C++ variants of standard C header files
#include <cctype>
#include <cstdarg>

// POV-Ray base header files
#include "base/path.h"

// All the builtin fonts must be declared here
#include "base/font/crystal.h"
#include "base/font/cyrvetic.h"
#include "base/font/povlogo.h"
#include "base/font/timrom.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

/*
// not used right now
typedef struct
{
    bool read_local;
    bool read_global;
    bool write_local;
    bool write_global;
} POV_File_Restrictions;

POV_File_Restrictions gPOV_File_Restrictions[POV_File_Count] =
{
    { false, false, false, false }, // POV_File_Unknown
    { true,  true,  false, false }, // POV_File_Image_Targa
    { true,  true,  false, false }, // POV_File_Image_PNG
    { true,  true,  false, false }, // POV_File_Image_PPM
    { true,  true,  false, false }, // POV_File_Image_PGM
    { true,  true,  false, false }, // POV_File_Image_GIF
    { true,  true,  false, false }, // POV_File_Image_IFF
    { true,  true,  false, false }, // POV_File_Image_JPEG
    { true,  true,  false, false }, // POV_File_Image_TIFF
    { true,  true,  false, false }, // POV_File_Image_System
    { true,  false, false, false }, // POV_File_Text_POV
    { true,  false, false, false }, // POV_File_Text_INC
    { true,  false, false, false }, // POV_File_Text_INI
    { true,  true,  false, false }, // POV_File_Text_CSV
    { true,  false, false, false }, // POV_File_Text_Stream
    { true,  true,  false, false }, // POV_File_Text_User
    { true,  true,  true,  false }, // POV_File_Data_DF3
    { true,  true,  true,  true  }, // POV_File_Data_RCA
    { true,  true,  true,  true  }, // POV_File_Data_LOG
    { true,  false, true,  false }  // POV_File_Font_TTF
};
*/

#ifndef POV_IS1
    #define POV_IS1 ""
#endif

#ifndef POV_IS2
    #define POV_IS2 ""
#endif

#ifndef POV_IS3
    #define POV_IS3 ""
#endif

#ifndef POV_IS4
    #define POV_IS4 ""
#endif

/// @todo   merge with @ref gFile_Type_To_Mask
POV_File_Extensions gPOV_File_Extensions[POV_File_Count] =
{
    {{ "",      "",      "",      ""      }}, // POV_File_Unknown
    {{ ".tga",  ".TGA",  "",      ""      }}, // POV_File_Image_Targa
    {{ ".png",  ".PNG",  "",      ""      }}, // POV_File_Image_PNG
    {{ ".ppm",  ".PPM",  "",      ""      }}, // POV_File_Image_PPM
    {{ ".pgm",  ".PGM",  "",      ""      }}, // POV_File_Image_PGM
    {{ ".gif",  ".GIF",  "",      ""      }}, // POV_File_Image_GIF
    {{ ".iff",  ".IFF",  ".lbm",  ".LBM"  }}, // POV_File_Image_IFF
    {{ ".jpg",  ".JPG",  ".jpeg", ".JPEG" }}, // POV_File_Image_JPEG
    {{ ".tif",  ".TIF",  ".tiff", ".TIFF" }}, // POV_File_Image_TIFF
    {{ ".bmp",  ".BMP",  "",      ""      }}, // POV_File_Image_BMP
    {{ ".exr",  ".EXR",  "",      ""      }}, // POV_File_Image_EXR
    {{ ".hdr",  ".HDR",  "",      ""      }}, // POV_File_Image_HDR
    {{ POV_IS1, POV_IS2, POV_IS3, POV_IS4 }}, // POV_File_Image_System
    {{ ".pov",  ".POV",  "",      ""      }}, // POV_File_Text_POV
    {{ ".inc",  ".INC",  "",      ""      }}, // POV_File_Text_INC
    {{ ".ini",  ".INI",  "",      ""      }}, // POV_File_Text_INI
    {{ ".csv",  ".CSV",  "",      ""      }}, // POV_File_Text_CSV
    {{ ".txt",  ".TXT",  "",      ""      }}, // POV_File_Text_Stream
    {{ "",      "",      "",      ""      }}, // POV_File_Text_User
    {{ ".df3",  ".DF3",  "",      ""      }}, // POV_File_Data_DF3
    {{ ".rca",  ".RCA",  "",      ""      }}, // POV_File_Data_RCA
    {{ ".log",  ".LOG",  "",      ""      }}, // POV_File_Data_LOG
    {{ ".bak",  ".BAK",  "",      ""      }}, // POV_File_Data_Backup
    {{ ".ttf",  ".TTF",  "",      ""      }}  // POV_File_Font_TTF
};

/// @todo   merge with @ref gPOV_File_Extensions
const int gFile_Type_To_Mask [POV_File_Count] =
{
    NO_FILE,   // POV_File_Unknown
    TGA_FILE,  // POV_File_Image_Targa
    PNG_FILE,  // POV_File_Image_PNG
    PPM_FILE,  // POV_File_Image_PPM
    PGM_FILE,  // POV_File_Image_PGM
    GIF_FILE,  // POV_File_Image_GIF
    IFF_FILE,  // POV_File_Image_IFF
    JPEG_FILE, // POV_File_Image_JPEG
    TIFF_FILE, // POV_File_Image_TIFF
    BMP_FILE,  // POV_File_Image_BMP
    EXR_FILE,  // POV_File_Image_EXR
    HDR_FILE,  // POV_File_Image_HDR
    SYS_FILE,  // POV_File_Image_System
    NO_FILE,   // POV_File_Text_POV
    NO_FILE,   // POV_File_Text_INC
    NO_FILE,   // POV_File_Text_INI
    NO_FILE,   // POV_File_Text_CSV
    NO_FILE,   // POV_File_Text_Stream
    NO_FILE,   // POV_File_Text_User
    NO_FILE,   // POV_File_Data_DF3
    NO_FILE,   // POV_File_Data_RCA
    NO_FILE,   // POV_File_Data_LOG
    NO_FILE,   // POV_File_Data_Backup
    NO_FILE    // POV_File_Font_TTF
};

int InferFileTypeFromExt(const UCS2String& ext)
{
    // TODO FIXME - better compare in the string domain

    string str = UCS2toASCIIString(ext);

    for (int i = 0; i < POV_File_Count; i++)
    {
        // TODO - checking whether extension matches is required at other places, too
        for (int j = 0; j < POV_FILE_EXTENSIONS_PER_TYPE; j ++)
        {
            if ( (strlen(gPOV_File_Extensions[i].ext[j]) > 0) &&
                 (pov_stricmp (gPOV_File_Extensions[i].ext[j], str.c_str()) == 0) )
            {
                return (i);
            }
        }
    }
    return NO_FILE;
}

/*****************************************************************************
*
* FUNCTION
*
*   Internal_Font_File
*
* INPUT
*   The numeric identifier of a built-in file
*
* OUTPUT
*
* RETURNS
*   An IStream for usage by the TTF analyser
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Provide a compiled font as a Stream for reading
*
* CHANGES
*
*   Creation. Oct 2012 [JG]
*
******************************************************************************/

/*
 * Default to povlogo.ttf (0)
 * 1 : TimeRoman (timrom.ttf), Serif
 * 2 : Cyrvetita (cyrvetic.ttf), Sans-Serif
 * 3 : Crystal (crystal.ttf), monospace sans serif
 *
 * To add a font, check first its license
 */
IMemStream *Internal_Font_File(int font_id)
{
    switch(font_id)
    {
        case 1:     return new IMemStream(&font_timrom[0],   sizeof(font_timrom),   "timrom.ttf");
        case 2:     return new IMemStream(&font_cyrvetic[0], sizeof(font_cyrvetic), "cyrvetic.ttf");
        case 3:     return new IMemStream(&font_crystal[0],  sizeof(font_crystal),  "crystal.ttf");
        default:    return new IMemStream(&font_povlogo[0],  sizeof(font_povlogo),  "povlogo.ttf");
    }
}

}
