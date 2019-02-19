//******************************************************************************
///
/// @file core/support/imageutil.h
///
/// Declarations related to mapped textures like image map, bump map and
/// material map.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_CORE_IMAGEUTIL_H
#define POVRAY_CORE_IMAGEUTIL_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/image/image_fwd.h"

// POV-Ray header files (core module)
#include "core/coretypes.h"
#include "core/material/pattern_fwd.h"
#include "core/math/vector.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreSupportImageUtil Image Handling Utilities
/// @ingroup PovCore
///
/// @{

using namespace pov_base;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

// Image/Bump Map projection types.

enum
{
    PLANAR_MAP       = 0,
    SPHERICAL_MAP    = 1,
    CYLINDRICAL_MAP  = 2,
    PARABOLIC_MAP    = 3,
    HYPERBOLIC_MAP   = 4,
    TORUS_MAP        = 5,
    PIRIFORM_MAP     = 6,
    ANGULAR_MAP      = 7
};

// Bit map interpolation types.

enum
{
    NO_INTERPOLATION = 0,
    NEAREST_NEIGHBOR = 1, // currently not supported; would be essentially the same as NO_INTERPOLATION
    BILINEAR         = 2,
    BICUBIC          = 3,
    NORMALIZED_DIST  = 4
};

enum
{
    USE_NONE         = 0,
    USE_COLOUR       = 1,
    USE_INDEX        = 2,
    USE_ALPHA        = 3
};

class ImageData final
{
    public:
        int References; // Keeps track of number of pointers to this structure
        int Map_Type;
        int Interpolation_Type;
        bool Once_Flag : 1;
        bool AllTransmitLegacyMode : 1;
        char Use;
        Vector3d Gradient;
        int iwidth, iheight;
        SNGL width, height;
        Vector2d Offset;
        COLC AllFilter, AllTransmit;
        void *Object;
        Image *data;

// it would have been a lot cleaner if POV_VIDCAP_IMPL was a subclass of pov::Image,
// since we could just assign it to data above and the following would not be needed.
// however at this point pov::Image doesn't allow the default constructor to be used,
// and we don't know our capture size or data type until after we talk to the hardware.
#ifdef POV_VIDCAP_IMPL
        POV_VIDCAP_IMPL *VidCap;
#endif

        ImageData();
        ~ImageData();
};

typedef ImageData *ImageDataPtr;

DBL image_pattern(const Vector3d& EPoint, const ImagePattern* pPattern); // TODO - move to pattern.cpp
TEXTURE *material_map(const Vector3d& IPoint, const TEXTURE *Texture);
void bump_map(const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal);
void image_colour_at(const ImageData *image, DBL xcoor, DBL ycoor, RGBFTColour& colour, int *index); // TODO ALPHA - caller should decide whether to prefer premultiplied or non-premultiplied alpha
void image_colour_at(const ImageData *image, DBL xcoor, DBL ycoor, RGBFTColour& colour, int *index, bool premul);
HF_VAL image_height_at(const ImageData *image, int x, int y);
bool is_image_opaque(const ImageData *image);
int map_pos(const Vector3d& EPoint, const ImageData* pImage, DBL *xcoor, DBL *ycoor);
ImageData *Copy_Image(ImageData *old);
ImageData *Create_Image(void);
void Destroy_Image(ImageData *image);

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_IMAGEUTIL_H
