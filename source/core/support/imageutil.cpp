//******************************************************************************
///
/// @file core/support/imageutil.cpp
///
/// Implementation of mapped textures like image map, bump map and material map.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/support/imageutil.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"
#include "base/povassert.h"
#include "base/image/encoding.h"
#include "base/image/image.h"

// POV-Ray header files (core module)
#include "core/colour/spectral.h"
#include "core/material/normal.h"
#include "core/material/pattern.h"
#include "core/material/texture.h"

#ifdef SYS_IMAGE_HEADER
#include SYS_IMAGE_HEADER
#endif

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

const DBL DIV_1_BY_65535 = 1.0 / 65535.0;
const DBL DIV_1_BY_255 = 1.0 / 255.0;



/*****************************************************************************
* Static functions
******************************************************************************/

static int cylindrical_image_map(const Vector3d& EPoint, const ImageData *image, DBL *u, DBL *v);
static int torus_image_map(const Vector3d& EPoint, const ImageData *image, DBL *u, DBL *v);
static int spherical_image_map(const Vector3d& EPoint, const ImageData *image, DBL *u, DBL *v);
static int planar_image_map(const Vector3d& EPoint, const ImageData *image, DBL *u, DBL *v);
static int angular_image_map(const Vector3d& EPoint, const ImageData *image, DBL *u, DBL  *v);
static void no_interpolation(const ImageData *image, DBL xcoor, DBL ycoor, RGBFTColour& colour, int *index, bool premul);
static void bilinear(DBL *factors, DBL x, DBL y);
static void norm_dist(DBL *factors, DBL x, DBL y);
static void cubic(DBL *factors, DBL x);
static void Interp(const ImageData *image, DBL xcoor, DBL ycoor, RGBFTColour& colour, int *index, bool premul);
static void InterpolateBicubic(const ImageData *image, DBL xcoor, DBL ycoor, RGBFTColour& colour, int *index, bool premul);

/*
 * 2-D to 3-D Procedural Texture Mapping of a Bitmapped Image onto an Object:
 *
 * A. Simplistic (planar) method of image projection devised by DKB and AAC:
 *
 * 1. Transform texture in 3-D space if requested. 2. Determine local object 2-d
 * coords from 3-d coords by <X Y Z> triple. 3. Return pixel color value at
 * that position on the 2-d plane of "Image". 3. Map colour value in Image
 * [0..255] to a more normal colour range [0..1].
 *
 * B. Specialized shape projection variations by Alexander Enzmann:
 *
 * 1. Cylindrical mapping 2. Spherical mapping 3. Torus mapping
 */

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Very different stuff than the other routines here. This routine takes an
*   intersection point and a texture and returns a new texture based on the
*   index/color of that point in an image/materials map. CdW 7/91
*
* CHANGES
*
******************************************************************************/

TEXTURE *material_map(const Vector3d& EPoint, const TEXTURE *Texture)
{
    int reg_number = -1;
    int Material_Number;
    DBL xcoor = 0.0, ycoor = 0.0;
    RGBFTColour colour;

    /*
     * Now we have transformed x, y, z we use image mapping routine to determine
     * texture index.
     */

    // NB: Can't use `static_cast` here due to multi-inheritance in the pattern class hierarchy.
    const ImagePatternImpl *pattern = dynamic_cast<ImagePatternImpl*>(Texture->pattern.get());
    POV_PATTERN_ASSERT(pattern);

    if(map_pos(EPoint, pattern->pImage, &xcoor, &ycoor))
        Material_Number = 0;
    else
    {
        image_colour_at(pattern->pImage, xcoor, ycoor, colour, &reg_number); // TODO ALPHA - we should decide whether we prefer premultiplied or non-premultiplied alpha

        if(reg_number == -1)
            Material_Number = (int)(colour.red() * 255.0);
        else
            Material_Number = reg_number;
    }

    if(Material_Number > Texture->Materials.size())
        Material_Number %= Texture->Materials.size();

    return Texture->Materials[Material_Number % Texture->Materials.size()];
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void bump_map(const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal)
{
    DBL xcoor = 0.0, ycoor = 0.0;
    int index = -1, index2 = -1, index3 = -1;
    RGBFTColour colour1, colour2, colour3;
    Vector3d p1, p2, p3;
    Vector3d bump_normal;
    Vector3d xprime, yprime, zprime;
    DBL Length;
    DBL Amount = Tnormal->Amount;
    const ImageData *image;

    // NB: Can't use `static_cast` here due to multi-inheritance in the pattern class hierarchy.
    const ImagePatternImpl *pattern = dynamic_cast<ImagePatternImpl*>(Tnormal->pattern.get());
    POV_PATTERN_ASSERT(pattern);

    image = pattern->pImage;

    // going to have to change this
    // need to know if bump point is off of image for all 3 points

    if(map_pos(EPoint, image, &xcoor, &ycoor))
        return;
    else
        image_colour_at(image, xcoor, ycoor, colour1, &index); // TODO ALPHA - we should decide whether we prefer premultiplied or non-premultiplied alpha

    xcoor--;
    ycoor++;

    if(xcoor < 0.0)
        xcoor += (DBL)image->iwidth;
    else if(xcoor >= image->iwidth)
        xcoor -= (DBL)image->iwidth;

    if(ycoor < 0.0)
        ycoor += (DBL)image->iheight;
    else if(ycoor >= (DBL)image->iheight)
        ycoor -= (DBL)image->iheight;

    image_colour_at(image, xcoor, ycoor, colour2, &index2); // TODO ALPHA - we should decide whether we prefer premultiplied or non-premultiplied alpha

    xcoor += 2.0;

    if(xcoor < 0.0)
        xcoor += (DBL)image->iwidth;
    else if(xcoor >= image->iwidth)
        xcoor -= (DBL)image->iwidth;

    image_colour_at(image, xcoor, ycoor, colour3, &index3); // TODO ALPHA - we should decide whether we prefer premultiplied or non-premultiplied alpha

    if(image->Use || (index == -1) || (index2 == -1) || (index3 == -1))
    {
        p1[X] = 0;
        p1[Y] = Amount * colour1.Greyscale();
        p1[Z] = 0;

        p2[X] = -1;
        p2[Y] = Amount * colour2.Greyscale();
        p2[Z] = 1;

        p3[X] = 1;
        p3[Y] = Amount * colour3.Greyscale();
        p3[Z] = 1;
    }
    else
    {
        p1[X] = 0;
        p1[Y] = Amount * index;
        p1[Z] = 0;

        p2[X] = -1;
        p2[Y] = Amount * index2;
        p2[Z] = 1;

        p3[X] = 1;
        p3[Y] = Amount * index3;
        p3[Z] = 1;
    }

    // we have points 1,2,3 for a triangle now we need the surface normal for it

    xprime = p1 - p2;
    yprime = p3 - p2;
    bump_normal = cross(yprime, xprime).normalized();

    yprime = normal;
    xprime = cross(yprime, Vector3d(0.0, 1.0, 0.0));
    Length = xprime.length();

    if(Length < EPSILON)
    {
        if(fabs(normal[Y] - 1.0) < EPSILON)
        {
            yprime = Vector3d(0.0, 1.0, 0.0);
            xprime = Vector3d(1.0, 0.0, 0.0);
            Length = 1.0;
        }
        else
        {
            yprime = Vector3d(0.0,-1.0, 0.0);
            xprime = Vector3d(1.0, 0.0, 0.0);
            Length = 1.0;
        }
    }

    xprime /= Length;
    zprime = cross(xprime, yprime).normalized();
    xprime *= bump_normal[X];
    yprime *= bump_normal[Y];
    zprime *= bump_normal[Z];
    normal = xprime + yprime - zprime;
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR    Nathan Kopp
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL image_pattern(const Vector3d& EPoint, const ImagePattern* pPattern)
{
    DBL xcoor = 0.0, ycoor = 0.0;
    int index = -1;
    RGBFTColour colour;
    const ImageData *image = pPattern->pImage;
    DBL Value;

    colour.Clear();

    // going to have to change this
    // need to know if bump point is off of image for all 3 points

    if(map_pos(EPoint, pPattern->pImage, &xcoor, &ycoor))
        return 0.0;
    else
        image_colour_at(image, xcoor, ycoor, colour, &index); // TODO ALPHA - we should decide whether we prefer premultiplied or non-premultiplied alpha

    if((index == -1) || image->Use)
    {
        if(image->Use == USE_ALPHA)
        {
            // use alpha channel or red channel
            if(image->data->HasTransparency() == true)
                Value = colour.transm();
            else
                Value = colour.red();   // otherwise, just use the red channel
        }
        else
            // use grey-scaled version of the color
            Value = colour.Greyscale();
    }
    else
        Value = index / 255.0;

    if(Value < 0)
        Value = 0;
    else if(Value > 1.0)
        Value = 1.0;

    return Value;
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void image_colour_at(const ImageData *image, DBL xcoor, DBL ycoor, RGBFTColour& colour, int *index)
{
    // TODO ALPHA - caller should decide whether to prefer premultiplied or non-premultiplied alpha

    // As the caller isn't sure whether to prefer premultiplied or non-premultiplied alpha,
    // we'll just give him what the original image source provided.
    image_colour_at(image, xcoor, ycoor, colour, index, image->data->IsPremultiplied());
}

void image_colour_at(const ImageData *image, DBL xcoor, DBL ycoor, RGBFTColour& colour, int *index, bool premul)
{
    *index = -1;

    bool doProperTransmitAll = image->data->HasTransparency() &&
                               !image->AllTransmitLegacyMode &&
                               !image->data->IsIndexed() && ( (image->AllTransmit != 0.0) || (image->AllFilter != 0.0) );

    // If either source or destination uses premultiplied alpha, make sure interpolation is done in premultiplied space
    // as it makes the mathematical operations cleaner -- unless the alpha channel is modulated by "transmit all" or
    // "filter all", in which case we prefer non-premultiplied space.
    bool getPremul = doProperTransmitAll ? (premul && image->data->IsPremultiplied()) :
                                           (premul || image->data->IsPremultiplied());

    switch(image->Interpolation_Type)
    {
        case NO_INTERPOLATION:
            no_interpolation(image, xcoor, ycoor, colour, index, getPremul);
            break;
        case BICUBIC:
            InterpolateBicubic(image, xcoor, ycoor, colour, index, getPremul);
            break;
        default:
            Interp(image, xcoor, ycoor, colour, index, getPremul);
            break;
    }
    bool havePremul = getPremul;

    if (!premul && havePremul)
    {
        // We fetched premultiplied data, but caller expects it non-premultiplied, so we need to fix that.
        // As "transmit/filter all" handling also works best on non-premultiplied data, we're doing this now.
        AlphaUnPremultiply(colour.rgb(), colour.FTtoA());
        havePremul = false;
    }

    if (doProperTransmitAll)
    {
        COLC imageAlpha = colour.FTtoA();

        if (imageAlpha != 0.0)  // No need to apply "filter/transmit all" if the image is fully transparent here anyway.
        {


            colour.transm() += image->AllTransmit * imageAlpha;
            colour.filter() += image->AllFilter   * imageAlpha;

            if (havePremul)
            {
                // We have premultiplied data here, and the caller expects it to stay that way (otherwise we'd already
                // have converted to non-premultiplied by now), so we need to fix the premultiplication of the colour
                // according to our modifications to the transparency components.
                COLC alphaCorrection = colour.FTtoA() / imageAlpha;
                AlphaPremultiply(colour.rgb(), alphaCorrection);
            }
        }
    }

    if (premul && !havePremul)
    {
        // We have non-premultiplied data here, but caller expects it premultiplied, so we need to fix that
        // As "transmit/filter all" handling works best on non-premultiplied data, we haven't done this earlier.
        AlphaPremultiply(colour.rgb(), colour.FTtoA());
    }
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

HF_VAL image_height_at(const ImageData *image, int x, int y)
{
    // TODO ALPHA - handling of premultiplied vs. non-premultiplied alpha needs to be considered

    RGBColour colour;

    // for 8-bit indexed images, use the index (scaled to match short int range)
    if (image->data->IsIndexed())
        return ((HF_VAL)image->data->GetIndexedValue(x, y) * 256);
    // TODO FIXME - should be *257 to get a max value of 255*257 = 65535
    /* [JG-2013]: do not change 256 for 257, due to backward compatibility with all versions up to v3.6
     * it's a shame to not being able to cover the full range when using indexed image, but
     * it was like that for a very very long time (since the introduction of height field in povray)
     */

    // for greyscale images, use the float greyscale value (scaled to match short int range)
    if (image->data->IsGrayscale())
        return ((HF_VAL) (image->data->GetGrayValue(x, y) * 65535.0f));

    image->data->GetRGBValue(x, y, colour); // TODO - what about alpha premultiplication?

    // for images with high bit depth (>8 bit per color channel), use the float greyscale value (scaled to match short int range)
    if (image->data->GetMaxIntValue() > 255)
        return ((HF_VAL) (colour.Greyscale() * 65535.0f));

    // for images with low bit depth (<=8 bit per color channel), compose from red (high byte) and green (low byte) channel.
    return ((HF_VAL) ((colour.red() * 256.0 + colour.green()) * 255.0));
    // [JG-2013] : the high byte / low byte is (r *255) * 256 + (g*255) , which is factored as (r*256+g)*255 (was issue flyspray #308)
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

bool is_image_opaque(const ImageData *image)
{
    return image->data->IsOpaque();
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Map a point (x, y, z) on a cylinder of radius 1, height 1, that has its axis
*   of symmetry along the y-axis to the square [0,1]x[0,1].
*
* CHANGES
*
******************************************************************************/

static int cylindrical_image_map(const Vector3d& EPoint, const ImageData *image, DBL *u, DBL  *v)
{
    DBL len, theta;
    DBL x = EPoint[X];
    DBL y = EPoint[Y];
    DBL z = EPoint[Z];

    if((image->Once_Flag) && ((y < 0.0) || (y > 1.0)))
        return 0;

    *v = fmod(y * image->height, (DBL)image->height);

    // Make sure this vector is on the unit sphere.

    len = sqrt(x * x + y * y + z * z);

    if (len == 0.0)
    {
        return 0;
    }
    else
    {
        x /= len;
        z /= len;
    }

    // Determine its angle from the point (1, 0, 0) in the x-z plane.

    len = sqrt(x * x + z * z);

    if(len == 0.0)
        return 0;
    else
    {
        if(z == 0.0)
        {
            if(x > 0)
                theta = 0.0;
            else
                theta = M_PI;
        }
        else
        {
            theta = acos(x / len);

            if(z < 0.0)
                theta = TWO_M_PI - theta;
        }

        theta /= TWO_M_PI;  // This will be from 0 to 1
    }

    *u = (theta * image->width);

    return 1;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Map a point (x, y, z) on a torus  to a 2-d image.
*
* CHANGES
*
******************************************************************************/

static int torus_image_map(const Vector3d& EPoint, const ImageData *image, DBL *u, DBL  *v)
{
    DBL len, phi, theta;
    DBL r0;
    DBL x = EPoint[X];
    DBL y = EPoint[Y];
    DBL z = EPoint[Z];

    r0 = image->Gradient[X];

    // Determine its angle from the x-axis.

    len = sqrt(x * x + z * z);

    if(len == 0.0)
        return 0;
    else if(z == 0.0)
    {
        if(x > 0)
            theta = 0.0;
        else
            theta = M_PI;
    }
    else
    {
        theta = acos(x / len);

        if(z < 0.0)
            theta = TWO_M_PI - theta;
    }

    theta = 0.0 - theta;

    // Now rotate about the y-axis to get the point (x, y, z) into the x-y plane.

    x = len - r0;

    len = sqrt(x * x + y * y);

    phi = acos(-x / len);

    if(y > 0.0)
        phi = TWO_M_PI - phi;

    // Determine the parametric coordinates.

    theta /= TWO_M_PI;

    phi /= TWO_M_PI;

    *u = (-theta * image->width);

    *v = (phi * image->height);

    return 1;
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Map a point (x, y, z) on a sphere of radius 1 to a 2-d image.
*   This is stuff from MegaPov for doing P. Debevec mirrorball mapping
*
* CHANGES
*
******************************************************************************/

/// @author Denis Bodor
static int angular_image_map(const Vector3d& EPoint, const ImageData *image, DBL *u, DBL  *v)
{
    DBL len, r;
    DBL x = EPoint[X];
    DBL y = EPoint[Y];
    DBL z = EPoint[Z];

    /* Make sure this vector is on the unit sphere. */
    len = sqrt(x * x + y * y + z * z);
    if (len == 0.0)
    {
        return 0;
    }
    else
    {
        x /= len;
        y /= len;
        z /= len;
    }

    if ( (x==0) && (y==0) )
        r=0;
    else
        r=(1/M_PI)*acos(z)/sqrt(x*x+y*y);

    *u = (x*r+1)/2 * image->width;
    *v = (y*r+1)/2 * image->height;

    return 1;
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Map a point (x, y, z) on a sphere of radius 1 to a 2-d image. (Or is it the
*   other way around?)
*
* CHANGES
*
******************************************************************************/

static int spherical_image_map(const Vector3d& EPoint, const ImageData *image, DBL *u, DBL *v)
{
    DBL len, phi, theta;
    DBL x = EPoint[X];
    DBL y = EPoint[Y];
    DBL z = EPoint[Z];

    // Make sure this vector is on the unit sphere.

    len = sqrt(x * x + y * y + z * z);

    if(len == 0.0)
        return 0;
    else
    {
        x /= len;
        y /= len;
        z /= len;
    }

    // Determine its angle from the x-z plane.

    phi = 0.5 + asin(y) / M_PI; // This will be from 0 to 1


    // Determine its angle from the point (1, 0, 0) in the x-z plane.

    len = sqrt(x * x + z * z);

    if(len == 0.0)
        // This point is at one of the poles. Any value of xcoord will be ok...
        theta = 0;
    else
    {
        if(z == 0.0)
        {
            if(x > 0)
                theta = 0.0;
            else
                theta = M_PI;
        }
        else
        {
            theta = acos(x / len);

            if(z < 0.0)
                theta = TWO_M_PI - theta;
        }

        theta /= TWO_M_PI;  // This will be from 0 to 1
    }

    *u = (theta * image->width);
    *v = (phi * image->height);

    return 1;
}

/*
 * 2-D to 3-D Procedural Texture Mapping of a Bitmapped Image onto an Object:
 *
 * Simplistic planar method of object image projection devised by DKB and AAC.
 *
 * 1. Transform texture in 3-D space if requested. 2. Determine local object 2-d
 * coords from 3-d coords by <X Y Z> triple. 3. Return pixel color value at
 * that position on the 2-d plane of "Image". 3. Map colour value in Image
 * [0..255] to a more normal colour range [0..1].
 */



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Return 0 if there is no color at this point (i.e. invisible), return 1 if a
*   good mapping is found.
*
* CHANGES
*
******************************************************************************/

static int planar_image_map(const Vector3d& EPoint, const ImageData *image, DBL *u, DBL  *v)
{
    DBL x = EPoint[X];
    DBL y = EPoint[Y];
    DBL z = EPoint[Z];

    if(image->Gradient[X] != 0.0)
    {
        if((image->Once_Flag) && ((x < 0.0) || (x > 1.0)))
            return 0;

        if(image->Gradient[X] > 0)
            *u = fmod(x * image->width, (DBL) image->width);
        else
            *v = fmod(x * image->height, (DBL) image->height);
    }

    if(image->Gradient[Y] != 0.0)
    {
        if((image->Once_Flag) && ((y < 0.0) || (y > 1.0)))
            return 0;

        if(image->Gradient[Y] > 0)
            *u = fmod(y * image->width, (DBL) image->width);
        else
            *v = fmod(y * image->height, (DBL) image->height);
    }

    if(image->Gradient[Z] != 0.0)
    {
        if((image->Once_Flag) && ((z < 0.0) || (z > 1.0)))
            return 0;

        if(image->Gradient[Z] > 0)
            *u = fmod(z * image->width, (DBL) image->width);
        else
            *v = fmod(z * image->height, (DBL) image->height);
    }

    return 1;
}


/*****************************************************************************
*
* FUNCTION
*
*   map_pos
*
* INPUT
*
*   EPoint   -- 3-D point at which function is evaluated
*   TPattern -- Pattern containing various parameters
*
* OUTPUT
*
*   xcoor, ycoor -- 2-D result
*
* RETURNS
*
*   Map returns 1 if point off of map 0 if on map
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Maps a 3-D point to a 2-D point depending upon map type
*
* CHANGES
*
******************************************************************************/

int map_pos(const Vector3d& EPoint, const ImageData* image, DBL *xcoor, DBL *ycoor)
{
    // Determine which mapper to use.

    switch(image->Map_Type)
    {
        case PLANAR_MAP:
            if(!planar_image_map(EPoint, image, xcoor, ycoor))
                return (1);
            break;
        case SPHERICAL_MAP:
            if(!spherical_image_map(EPoint, image, xcoor, ycoor))
                return (1);
            break;
        case CYLINDRICAL_MAP:
            if(!cylindrical_image_map(EPoint, image, xcoor, ycoor))
                return (1);
            break;
        case TORUS_MAP:
            if(!torus_image_map(EPoint, image, xcoor, ycoor))
                return (1);
            break;
        case ANGULAR_MAP:
            if(!angular_image_map(EPoint, image, xcoor, ycoor))
                return (1);
            break;
        default:
            if(!planar_image_map(EPoint, image, xcoor, ycoor))
                return (1);
            break;
    }

    // Now make sure the point is on the image
    // and apply integer repeats and offsets
    *xcoor += image->Offset[U] + EPSILON;
    *ycoor += image->Offset[V] + EPSILON;

    if(image->Once_Flag)
    {
        if((*xcoor >= image->iwidth) || (*ycoor >= image->iheight) || (*xcoor < 0.0) || (*ycoor <0.0))
            return (1);
    }

    *xcoor = wrap( *xcoor, (DBL)(image->iwidth));
    *ycoor = wrap(-*ycoor, (DBL)(image->iheight)); // (Compensate for y coordinates on the images being upside down)

    return (0);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void no_interpolation(const ImageData *image, DBL xcoor, DBL ycoor, RGBFTColour& colour, int *index, bool premul)
{
    int iycoor, ixcoor;

    if(image->Once_Flag)
    {
        // image is to be seen only once, so when taking samples for interpolation
        // coordinates should not wrap around; instead, take the samples from the nearest
        // pixel right at the edge of the image

        if(xcoor < 0.0)
            ixcoor = 0;
        else if(xcoor >= (DBL)image->iwidth)
            ixcoor = image->iwidth - 1;
        else
            ixcoor = (int)xcoor;

        if(ycoor < 0.0)
            iycoor = 0;
        else if(ycoor >= (DBL)image->iheight)
            iycoor = image->iheight - 1;
        else
            iycoor = (int)ycoor;
    }
    else
    {
        // image is to be repeated, so when taking samples for interpolation
        // have coordinates wrap around

        ixcoor = (int)wrap(xcoor, (DBL)image->iwidth);
        iycoor = (int)wrap(ycoor, (DBL)image->iheight);
    }

    image->data->GetRGBFTValue(ixcoor, iycoor, colour, premul);

    if(image->data->IsIndexed() == false)
    {
        *index = -1;

        if (image->AllTransmitLegacyMode)
        {
            // Legacy versions applied "transmit/filter all" before interpolation,
            // and with little respect to an image's inherent alpha information.
            colour.transm() += image->AllTransmit;
            colour.filter() += image->AllFilter;
        }
    }
    else
        *index = image->data->GetIndexedValue(ixcoor, iycoor);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

// Interpolate color and filter values when mapping

static void Interp(const ImageData *image, DBL xcoor, DBL ycoor, RGBFTColour& colour, int *index, bool premul)
{
    int iycoor, ixcoor, i;
    int Corners_Index[4];
    RGBFTColour Corner_Colour[4];
    DBL Corner_Factors[4];

    xcoor += 0.5;
    ycoor += 0.5;

    iycoor = (int)ycoor;
    ixcoor = (int)xcoor;

    no_interpolation(image, (DBL)ixcoor,     (DBL)iycoor,     Corner_Colour[0], &Corners_Index[0], premul);
    no_interpolation(image, (DBL)ixcoor - 1, (DBL)iycoor,     Corner_Colour[1], &Corners_Index[1], premul);
    no_interpolation(image, (DBL)ixcoor,     (DBL)iycoor - 1, Corner_Colour[2], &Corners_Index[2], premul);
    no_interpolation(image, (DBL)ixcoor - 1, (DBL)iycoor - 1, Corner_Colour[3], &Corners_Index[3], premul);

    if(image->Interpolation_Type == BILINEAR)
        bilinear(Corner_Factors, xcoor, ycoor);
    else if(image->Interpolation_Type == NORMALIZED_DIST)
        norm_dist(Corner_Factors, xcoor, ycoor);
    else
        POV_ASSERT(false);

    // We're using double precision for the colors here to avoid higher-than-1.0 results due to rounding errors,
    // which would otherwise lead to stray dot artifacts when clamped to [0..1] range for a color_map or similar.
    // (Note that strictly speaking we don't avoid such rounding errors, but rather make them small enough that
    // subsequent rounding to single precision will take care of them.)
    PreciseRGBFTColour temp_colour;
    DBL temp_index = 0;
    for (i = 0; i < 4; i ++)
    {
        temp_colour += PreciseRGBFTColour(Corner_Colour[i]) * Corner_Factors[i];
        temp_index  += Corners_Index[i]                     * Corner_Factors[i];
    }
    colour = RGBFTColour(temp_colour);
    *index = (int)temp_index;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

// Interpolate color and filter values when mapping

static void cubic(DBL *factors, DBL x)
{
    DBL p = x-(int)x;
    DBL q = 1-p;
    factors[0] = -0.5 * p * q*q;
    factors[1] =  0.5 * q * ( q * (3*p + 1) + 1 );
    factors[2] =  0.5 * p * ( p * (3*q + 1) + 1 );
    factors[3] = -0.5 * q * p*p;
}

static void InterpolateBicubic(const ImageData *image, DBL xcoor, DBL  ycoor, RGBFTColour& colour, int *index, bool premul)
{
    int iycoor, ixcoor;
    int cornerIndex;
    RGBFTColour cornerColour;
    DBL factor;
    DBL factorsX[4];
    DBL factorsY[4];

    xcoor += 0.5;
    ycoor += 0.5;

    iycoor = (int)ycoor;
    ixcoor = (int)xcoor;

    cubic(factorsX, xcoor);
    cubic(factorsY, ycoor);

    // We're using double precision for the colors here to avoid higher-than-1.0 results due to rounding errors,
    // which would otherwise lead to stray dot artifacts when clamped to [0..1] range for a color_map or similar.
    // (Note that strictly speaking we don't avoid such rounding errors, but rather make them small enough that
    // subsequent rounding to single precision will take care of them.)
    // (Note that bicubic interpolation may still give values outside the range [0..1] at high-contrast edges;
    // this is an inherent property of this interpolation method, and is therefore accepted here.)
    PreciseRGBFTColour tempColour;
    DBL tempIndex = 0;
    for (int i = 0; i < 4; i ++)
    {
        for (int j = 0; j < 4; j ++)
        {
            cornerColour.Clear();
            no_interpolation(image, (DBL)ixcoor + i-2, (DBL)iycoor + j-2, cornerColour, &cornerIndex, premul);
            factor = factorsX[i] * factorsY[j];
            tempColour += PreciseRGBFTColour(cornerColour) * factor;
            tempIndex  += cornerIndex                      * factor;
        }
    }
    colour = RGBFTColour(tempColour);
    *index = (int)tempIndex;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

// These interpolation techniques are taken from an article by
// Girish T. Hagan in the C Programmer's Journal V 9 No. 8
// They were adapted for POV-Ray by CdW

static void bilinear(DBL *factors, DBL x, DBL y)
{
    DBL p, q;

    p = x - (int)x;
    q = y - (int)y;

    factors[0] =    p  *    q;
    factors[1] = (1-p) *    q;
    factors[2] =    p  * (1-q);
    factors[3] = (1-p) * (1-q);
}




/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

#define PYTHAGOREAN_SQ(a,b)  ( (a)*(a) + (b)*(b) )

static void norm_dist(DBL *factors, DBL x, DBL y)
{
    int i;

    DBL p, q;
    DBL wts[4];
    DBL sum_inv_wts = 0.0;
    DBL sum_I = 0.0;

    p = x - (int)x;
    q = y - (int)y;

    wts[0] = 1.0 / PYTHAGOREAN_SQ(1-p, 1-q);
    wts[1] = 1.0 / PYTHAGOREAN_SQ(  p, 1-q);
    wts[2] = 1.0 / PYTHAGOREAN_SQ(1-p,   q);
    wts[3] = 1.0 / PYTHAGOREAN_SQ(  p,   q);

    for(i = 0; i < 4; i++)
        sum_inv_wts += wts[i];

    for(i = 0; i < 4; i++)
        factors[i] = (wts[i] / sum_inv_wts);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Image
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Scott Manley Added repeat vector initialisation
*
******************************************************************************/

ImageData *Create_Image()
{
    return new ImageData;
}

ImageData::ImageData() :
    References(1),
    Map_Type(PLANAR_MAP),
    Interpolation_Type(NO_INTERPOLATION),
    Once_Flag(false),
    AllTransmitLegacyMode(false),
    Use(USE_NONE),
    Gradient(1.0,-1.0,0.0),
    iwidth(0), iheight(0),
    width(0.0), height(0.0),
    Offset(0.0, 0.0),
    AllFilter(0.0), AllTransmit(0.0),
    Object(nullptr),
    data(nullptr)
#ifdef POV_VIDCAP_IMPL
    // beta-test feature
    ,VidCap(nullptr)
#endif
{}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Image
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

ImageData *Copy_Image(ImageData *Old)
{
    if (Old != nullptr)
        Old->References++;

    return (Old);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Image
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Destroy_Image(ImageData *image)
{
    if ((image == nullptr) || (--(image->References) > 0))
        return;

    delete image;
}

ImageData::~ImageData()
{
#ifdef POV_VIDCAP_IMPL
    // beta-test feature
    if (VidCap != nullptr)
        delete VidCap;
#endif

    if (data != nullptr)
        delete data;
}

}
// end of namespace pov
