//******************************************************************************
///
/// @file frontend/imagemessagehandler.cpp
///
/// @todo   What's in here?
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
//*******************************************************************************

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "frontend/imagemessagehandler.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <vector>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/types.h"
#include "base/image/dither.h"
#include "base/image/encoding.h"
#include "base/image/image.h"

// POV-Ray header files (POVMS module)
//  (none at the moment)

// POV-Ray header files (frontend module)
#include "frontend/display.h"
#include "frontend/renderfrontend.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

using std::vector;

ImageMessageHandler::ImageMessageHandler()
{
}

ImageMessageHandler::~ImageMessageHandler()
{
}

void ImageMessageHandler::HandleMessage(const SceneData& sd, const ViewData& vd, POVMSType ident, POVMS_Object& msg)
{
    // only blocks relevant for final image will be stored in the buffer, others are just sent to the preview display
    bool final = msg.Exist(kPOVAttrib_PixelFinal);

    switch(ident)
    {
        case kPOVMsgIdent_PixelSet:
            DrawPixelSet(sd, vd, msg, final);
            break;
        case kPOVMsgIdent_PixelBlockSet:
            DrawPixelBlockSet(sd, vd, msg, final);
            break;
        case kPOVMsgIdent_PixelRowSet:
            DrawPixelRowSet(sd, vd, msg, final);
            break;
        case kPOVMsgIdent_RectangleFrameSet:
            DrawRectangleFrameSet(sd, vd, msg, final);
            break;
        case kPOVMsgIdent_FilledRectangleSet:
            DrawFilledRectangleSet(sd, vd, msg, final);
            break;
    }
}

void ImageMessageHandler::DrawPixelSet(const SceneData& sd, const ViewData& vd, POVMS_Object& msg, bool final)
{
    POVMS_Attribute pixelposattr;
    POVMS_Attribute pixelcolattr;
    unsigned int psize(msg.GetInt(kPOVAttrib_PixelSize));

    msg.Get(kPOVAttrib_PixelPositions, pixelposattr);
    msg.Get(kPOVAttrib_PixelColors, pixelcolattr);

    vector<POVMSInt> pixelpositions(pixelposattr.GetIntVector());
    vector<POVMSFloat> pixelcolors(pixelcolattr.GetFloatVector());

    if((pixelpositions.size() / 2) != (pixelcolors.size() / 5))
        throw POV_EXCEPTION(kInvalidDataSizeErr, "Number of pixel colors and pixel positions does not match!");

    for(int i = 0, ii = 0; (i < pixelcolors.size()) && (ii < pixelpositions.size()); i += 5, ii += 2)
    {
        RGBTColour col(pixelcolors[i], pixelcolors[i + 1], pixelcolors[i + 2], pixelcolors[i + 4]); // NB pixelcolors[i + 3] is an unused channel
        RGBTColour gcol(col);
        unsigned int x(pixelpositions[ii]);
        unsigned int y(pixelpositions[ii + 1]);
        Display::RGBA8 rgba;
        float dither = GetDitherOffset(x, y);

        if (vd.display != nullptr)
        {
            // TODO ALPHA - display may profit from receiving the data in its original, premultiplied form
            // Premultiplied alpha was good for the math, but the display expects non-premultiplied alpha, so fix this if possible.
            AlphaUnPremultiply(gcol);

            if (vd.greyscaleDisplay)
            {
                rgba.red  = IntEncode(vd.displayGamma, gcol.Greyscale(), 255, dither);
                rgba.green = rgba.red;
                rgba.blue  = rgba.red;
            }
            else
            {
                rgba.red   = IntEncode(vd.displayGamma, gcol.red(),   255, dither);
                rgba.green = IntEncode(vd.displayGamma, gcol.green(), 255, dither);
                rgba.blue  = IntEncode(vd.displayGamma, gcol.blue(),  255, dither);
            }
            rgba.alpha = IntEncode(gcol.alpha(), 255, dither);
        }

        if(psize == 1)
        {
            if (vd.display != nullptr)
                vd.display->DrawPixel(x, y, rgba);

            if (final && (vd.image != nullptr) && (x < vd.image->GetWidth()) && (y < vd.image->GetHeight()))
                vd.image->SetRGBTValue(x, y, col);
        }
        else
        {
            if (vd.display != nullptr)
                vd.display->DrawFilledRectangle(x, y, x + psize - 1, y + psize - 1, rgba);

            if (final && (vd.image != nullptr))
            {
                for(unsigned int py = 0; (py < psize) && (y + py < vd.image->GetHeight()); py++)
                {
                    for(unsigned int px = 0; (px < psize) && (x + px < vd.image->GetWidth()); px++)
                        vd.image->SetRGBTValue(x + px, y + py, col);
                }
            }
        }
    }

    if (final && (vd.imageBackup != nullptr))
    {
        msg.Write(*vd.imageBackup);
        vd.imageBackup->flush();
    }
}

void ImageMessageHandler::DrawPixelBlockSet(const SceneData& sd, const ViewData& vd, POVMS_Object& msg, bool final)
{
    POVRect rect(msg.GetInt(kPOVAttrib_Left), msg.GetInt(kPOVAttrib_Top), msg.GetInt(kPOVAttrib_Right), msg.GetInt(kPOVAttrib_Bottom));
    POVMS_Attribute pixelattr;
    vector<RGBTColour> cols;
    vector<Display::RGBA8> rgbas;
    unsigned int psize(msg.GetInt(kPOVAttrib_PixelSize));
    int i = 0;

    msg.Get(kPOVAttrib_PixelBlock, pixelattr);

    vector<POVMSFloat> pixelvector(pixelattr.GetFloatVector());

    cols.reserve(rect.GetArea());
    rgbas.reserve(rect.GetArea());

    for(i = 0; i < rect.GetArea() *  5; i += 5)
    {
        RGBTColour col(pixelvector[i], pixelvector[i + 1], pixelvector[i + 2], pixelvector[i + 4]); // NB pixelvector[i + 3] is an unused channel
        RGBTColour gcol(col);
        Display::RGBA8 rgba;
        unsigned int x(rect.left + (i/5) % rect.GetWidth());
        unsigned int y(rect.top  + (i/5) / rect.GetWidth());
        float dither = GetDitherOffset(x, y);

        if (vd.display != nullptr)
        {
            // TODO ALPHA - display may profit from receiving the data in its original, premultiplied form
            // Premultiplied alpha was good for the math, but the display expects non-premultiplied alpha, so fix this if possible.
            AlphaUnPremultiply(gcol);

            if (vd.greyscaleDisplay)
            {
                rgba.red    = IntEncode(vd.displayGamma, gcol.Greyscale(), 255, dither);
                rgba.green  = rgba.red;
                rgba.blue   = rgba.red;
            }
            else
            {
                rgba.red    = IntEncode(vd.displayGamma, gcol.red(),   255, dither);
                rgba.green  = IntEncode(vd.displayGamma, gcol.green(), 255, dither);
                rgba.blue   = IntEncode(vd.displayGamma, gcol.blue(),  255, dither);
            }
            rgba.alpha = IntEncode(gcol.alpha(), 255, dither);

            rgbas.push_back(rgba);
        }
        cols.push_back(col);
    }

    if (vd.display != nullptr)
    {
        if(psize == 1)
            vd.display->DrawPixelBlock(rect.left, rect.top, rect.right, rect.bottom, &rgbas[0]);
        else
        {
            for(unsigned int y = rect.top, i = 0; y <= rect.bottom; y += psize)
            {
                for(unsigned int x = rect.left; x <= rect.right; x += psize, i++)
                    vd.display->DrawFilledRectangle(x, y, x + psize - 1, y + psize - 1, rgbas[0]);
            }
        }
    }

    if (final && (vd.image != nullptr))
    {
        for(unsigned int y = rect.top, i = 0; y <= rect.bottom; y += psize)
        {
            for(unsigned int x = rect.left; x <= rect.right; x += psize, i++)
            {
                for(unsigned int py = 0; py < psize; py++)
                {
                    for(unsigned int px = 0; px < psize; px++)
                        vd.image->SetRGBTValue(x + px, y + py, cols[i]);
                }
            }
        }
    }

    if (final && (vd.imageBackup != nullptr))
    {
        msg.Write(*vd.imageBackup);
        vd.imageBackup->flush();
    }
}

void ImageMessageHandler::DrawPixelRowSet(const SceneData& sd, const ViewData& vd, POVMS_Object& msg, bool final)
{
}

void ImageMessageHandler::DrawRectangleFrameSet(const SceneData& sd, const ViewData& vd, POVMS_Object& msg, bool final)
{
}

void ImageMessageHandler::DrawFilledRectangleSet(const SceneData& sd, const ViewData& vd, POVMS_Object& msg, bool final)
{
}

}
// end of namespace pov_frontend
