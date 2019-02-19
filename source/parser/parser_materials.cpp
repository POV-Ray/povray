//******************************************************************************
///
/// @file parser/parser_materials.cpp
///
/// Implementations related to parsing of textures and atmospheric effects.
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
#include "parser/parser.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/fileutil.h"
#include "base/image/colourspace.h"
#include "base/image/image.h"
#include "base/path.h"
#include "base/stringutilities.h"

// POV-Ray header files (core module)
#include "core/lighting/lightgroup.h"
#include "core/material/blendmap.h"
#include "core/material/interior.h"
#include "core/material/media.h"
#include "core/material/noise.h"
#include "core/material/normal.h"
#include "core/material/pattern.h"
#include "core/material/pigment.h"
#include "core/material/texture.h"
#include "core/material/warp.h"
#include "core/math/matrix.h"
#include "core/scene/atmosphere.h"
#include "core/scene/object.h"
#include "core/scene/scenedata.h"
#include "core/scene/tracethreaddata.h"
#include "core/shape/isosurface.h"
#include "core/support/imageutil.h"

// POV-Ray header files (VM module)
#include "vm/fnpovfpu.h"

// POV-Ray header files (parser module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmsid.h" // TODO - only here for kPOVList_FileType_XXX

#ifdef SYS_IMAGE_HEADER
#include SYS_IMAGE_HEADER
#endif

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

using namespace pov;

using std::shared_ptr;
using std::vector;

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define ADD_TNORMAL if (Tnormal == nullptr) {if ((Default_Texture->Tnormal) != nullptr) \
    Tnormal = Copy_Tnormal ((Default_Texture->Tnormal)); else Tnormal = Create_Tnormal ();\
    Texture->Tnormal=Tnormal;};


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
*   Ronald Parker, changes by Thorsten Froehlich
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Make_Pattern_Image(ImageData *image, FUNCTION_PTR fn, int token)
{
    int i = 0;
    int j = 0;
    DBL val = 0;
    FunctionCode *f = mpFunctionVM->GetFunction(*fn);
    Vector3d point;

    image->iwidth  = image->width;
    image->iheight = image->height;
    if(token == FUNCT_ID_TOKEN)
    {
        image->data =Image::Create(image->iwidth, image->iheight, ImageDataType::Gray_Int16);

        point[Z] = 0;

        for(i = 0; i < image->iheight; i++)
        {
            point[Y] = ((DBL)i / (image->height - 1));

            for( j = 0; j < image->iwidth; j++ )
            {
                point[X] = ((DBL)j / (image->width - 1));

                fnVMContext->SetLocal(X, point[X]);
                fnVMContext->SetLocal(Y, point[Y]);
                fnVMContext->SetLocal(Z, point[Z]);

                image->data->SetGrayValue(j, i, float(POVFPU_Run(fnVMContext, *fn)));
            }
        }
    }
    else if((token == VECTFUNCT_ID_TOKEN) && (f->return_size == 5))
    {
        image->data = Image::Create(image->iwidth, image->iheight, ImageDataType::RGBA_Int16); // TODO - we should probably use an HDR format
        image->data->SetPremultiplied(false); // We're storing the data in non-premultiplied alpha format, as this preserves all the data we're getting from the function.

        point[Z] = 0;

        for(i = 0; i < image->iheight; i++)
        {
            point[Y] = ((DBL)i / (image->height - 1));

            for(j = 0; j < image->iwidth; j++)
            {
                point[X] = ((DBL)j / (image->width - 1));

                fnVMContext->SetLocal(X + f->return_size, point[X]);
                fnVMContext->SetLocal(Y + f->return_size, point[Y]);
                fnVMContext->SetLocal(Z + f->return_size, point[Z]);

                (void)POVFPU_Run(fnVMContext, *fn);

                image->data->SetRGBFTValue(j, i,
                                           float(fnVMContext->GetLocal(pRED)),
                                           float(fnVMContext->GetLocal(pGREEN)),
                                           float(fnVMContext->GetLocal(pBLUE)),
                                           float(fnVMContext->GetLocal(pFILTER)), // N.B. pFILTER component is currently ignored by the RGBA_Int16 SetRGBFTValue (matches v3.6 behavior)
                                           float(fnVMContext->GetLocal(pTRANSM)));
            }
        }
    }
    else
        Error("Unsupported function type in function image.");

    mpFunctionVM->DestroyFunction(fn);
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

ImageData *Parser::Parse_Image(int Legal, bool GammaCorrect)
{
    ImageData *image = nullptr;
    Vector3d Local_Vector;
    char *Name = nullptr;
    TokenId token_id;
    int filetype = NO_FILE;
    UCS2String ign;
    FUNCTION_PTR fnPtr;

    image = Create_Image();

    if(Legal & GRAD_FILE)
    {
        EXPECT_CAT
            CASE_VECTOR_UNGET
                VersionWarning(150, "Old style orientation vector or map type not supported. Ignoring value.");
                Parse_Vector(Local_Vector);
            END_CASE

            OTHERWISE
                UNGET
                EXIT
            END_CASE
        END_EXPECT
    }

    EXPECT_ONE
        CASE (FUNCTION_TOKEN)
            image->width = (SNGL)int(Parse_Float() + 0.5);
            Parse_Comma();
            image->height = (SNGL)int(Parse_Float() + 0.5);

            Get_Token();
            if (CurrentTrueTokenId() != LEFT_CURLY_TOKEN)
                Found_Instead_Error("Missing { after", "expression");
            Unget_Token();

            fnPtr = Parse_DeclareFunction(&token_id, nullptr, false);
            Make_Pattern_Image(image, fnPtr, token_id);
        END_CASE

        CASE (IFF_TOKEN)
            filetype = IFF_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (GIF_TOKEN)
            filetype = GIF_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (POT_TOKEN)
            filetype = POT_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (SYS_TOKEN)
            filetype = SYS_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (TGA_TOKEN)
            filetype = TGA_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (PNG_TOKEN)
            filetype = PNG_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (PGM_TOKEN)
            filetype = PGM_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (PPM_TOKEN)
            filetype = PPM_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (JPEG_TOKEN)
            filetype = JPEG_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (TIFF_TOKEN)
            mExperimentalFlags.tiff = true;
            filetype = TIFF_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (BMP_TOKEN)
            filetype = BMP_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (EXR_TOKEN)
            filetype = EXR_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE (HDR_TOKEN)
            filetype = HDR_FILE;
            Name = Parse_C_String(true);
        END_CASE

        CASE5 (STRING_LITERAL_TOKEN,CHR_TOKEN,SUBSTR_TOKEN,STR_TOKEN,VSTR_TOKEN)
        CASE5 (CONCAT_TOKEN,STRUPR_TOKEN,STRLWR_TOKEN,DATETIME_TOKEN,STRING_ID_TOKEN)
            {
                UNGET
                Name = Parse_C_String(true);
                UCS2String filename = SysToUCS2String(Name);
                UCS2String ext = GetFileExtension(Path(filename));

                if (ext.empty() == false)
                {
                    filetype = gFile_Type_To_Mask [InferFileTypeFromExt(ext)];
                    if (filetype == NO_FILE)
                    {
                        POV_FREE(Name);
                        Error("Could not determine valid image file type from extension.");
                    }
                }
                else
                {
                    switch (sceneData->defaultFileType)
                    {
                        case kPOVList_FileType_Targa:
                        case kPOVList_FileType_CompressedTarga:
                            filetype = TGA_FILE;
                            break;
                        case kPOVList_FileType_PNG:
                            filetype = PNG_FILE;
                            break;
                        case kPOVList_FileType_JPEG:
                            filetype = JPEG_FILE;
                            break;
                        case kPOVList_FileType_PPM:
                            filetype = PPM_FILE;
                            break;
                        case kPOVList_FileType_BMP:
                            filetype = BMP_FILE;
                            break;
                        case kPOVList_FileType_OpenEXR:
                            filetype = EXR_FILE;
                            break;
                        case kPOVList_FileType_RadianceHDR:
                            filetype = HDR_FILE;
                            break;
                        case kPOVList_FileType_System:
                            filetype = SYS_FILE;
                            break;
                        default:
                            POV_FREE(Name);
                            Error("Could not determine valid input file type from output type.");
                    }
                }
            }
        END_CASE

        OTHERWISE
            Expectation_Error("map file spec");
        END_CASE
    END_EXPECT

    if (Name != nullptr)
    {
        if(!(filetype & Legal))
            Error("File type not supported here.");

        UCS2String filename = SysToUCS2String(Name);
        ImageReadOptions options;

        switch (sceneData->gammaMode)
        {
            case kPOVList_GammaMode_None:
                options.gammacorrect = false;
                options.defaultGamma.reset();
                options.workingGamma.reset();
                break;
            case kPOVList_GammaMode_AssumedGamma36:
                if (GammaCorrect)
                {
                    options.gammacorrect = GammaCorrect;
                    options.defaultGamma = SimpleGammaCurvePtr(sceneData->workingGamma);
                    options.workingGamma = SimpleGammaCurvePtr(sceneData->workingGamma);
                }
                else
                {
                    options.gammacorrect = false;
                    options.defaultGamma.reset();
                    options.workingGamma.reset();
                }
                break;
            case kPOVList_GammaMode_AssumedGamma37:
            case kPOVList_GammaMode_AssumedGamma37Implied:
                if (GammaCorrect)
                {
                    options.gammacorrect = GammaCorrect;
                    options.defaultGamma.reset();
                    options.workingGamma = SimpleGammaCurvePtr(sceneData->workingGamma);
                }
                else
                {
                    options.gammacorrect = false;
                    options.defaultGamma.reset();
                    options.workingGamma.reset();
                }
                break;
            default:
                throw POV_EXCEPTION_STRING("Unknown gamma handling mode in Parse_Image()");
        }

        EXPECT
            CASE (GAMMA_TOKEN)
                // User wants to override decoding gamma for this particular file; let them have their will.
                options.gammacorrect = true;
                options.gammaOverride = true;
                options.defaultGamma = Parse_Gamma();
                if (GammaCorrect)
                    options.workingGamma = SimpleGammaCurvePtr(sceneData->workingGamma);
                else
                    options.workingGamma.reset();
            END_CASE
            CASE (PREMULTIPLIED_TOKEN)
                // User wants to override alpha premultiplication setting for this particular file; let them have their will.
                options.premultipliedOverride = true;
                options.premultiplied = ((int)Parse_Float() != 0);
            END_CASE
            OTHERWISE
                UNGET
                EXIT
            END_CASE
        END_EXPECT

        if (options.gammacorrect && !options.gammaOverride && (filetype == PNG_FILE) && (sceneData->EffectiveLanguageVersion() < 370))
        {
            PossibleError("PNG input image default gamma handling has changed for pre POV-Ray v3.7 scenes;\n"
                          "results may differ from original intention. See the documentation for more\n"
                          "details.");
        }

        if (GammaCorrect && !options.gammaOverride && ((filetype == PGM_FILE) || (filetype == PPM_FILE)))
        {
            // As of POV-Ray v3.8, our default gamma handling for Netpbm (PGM/PPM) input images adheres to the
            // official standard, which mandates data to be gamma-encoded using the ITU-R BT.709 transfer function.

            if (sceneData->EffectiveLanguageVersion() < 380)
            {
                // For legacy scenes we simulate the old behaviour, which was to perform no gamma correction at all.
                options.gammacorrect = false;
            }
            else
            {
                Warning("Input image gamma not specified for Netpbm (PGM/PPM) file; POV-Ray will default to the\n"
                        "official standard, but competing de-facto standards exist. To get rid of this warning,\n"
                        "explicitly specify \"gamma bt709\". If the results do not match your expectations, try\n"
                        "\"gamma srgb\" or \"gamma 1.0\".");
            }
        }

        if (!GammaCorrect)
        {
            // context typically implies that gamma correction is not desired (e.g. height_field)
            if (options.gammacorrect && !options.gammaOverride)
                Warning("Input image gamma not specified for height_field, bump_map or image_pattern;\n"
                        "no gamma adjustment performed on input image; results may differ from intention\n"
                        "in rare cases. See the documentation for details.\n"
                        "To get rid of this warning, explicitly specify \"gamma 1.0\".");
        }

        // beta-test feature
        // if we end up keeping vidcap, it will probably get its own filetype.
        // for now, avoid the need to change the SDL by using the sys file type with the below prefix
        if (filetype == SYS_FILE && strncmp(Name, ":vidcap:", 8) == 0)
        {
#ifdef POV_VIDCAP_IMPL
            image->VidCap = new POV_VIDCAP_IMPL();
            // note the second ':' gets passed since it's the option prefix
            // e.g. ":vidcap:source=/dev/video0:w=640:h=480:fps=5"
            image->data = image->VidCap->Init(Name + 7, options, true);
            mBetaFeatureFlags.videoCapture = true;
#else
            Error("Beta-test video capture feature not implemented on this platform.");
#endif
        }
        else
            image->data = Read_Image(filetype, filename.c_str(), options);

        if (!options.warnings.empty())
            for (vector<std::string>::iterator it = options.warnings.begin(); it != options.warnings.end(); it++)
                Warning("%s: %s", Name, it->c_str());

        POV_FREE(Name);
    }

    if (image->data == nullptr)
        Error("Cannot read image.");

    image->iwidth = image->data->GetWidth();
    image->iheight = image->data->GetHeight();
    image->width = (SNGL) image->iwidth;
    image->height = (SNGL) image->iheight;

    return image;
}


SimpleGammaCurvePtr Parser::Parse_Gamma (void)
{
    SimpleGammaCurvePtr gamma;
    EXPECT_ONE
        CASE (SRGB_TOKEN)
            gamma = SRGBGammaCurve::Get();
        END_CASE
        CASE (BT709_TOKEN)
            gamma = BT709GammaCurve::Get();
        END_CASE
        CASE (BT2020_TOKEN)
            gamma = BT2020GammaCurve::Get();
        END_CASE
        OTHERWISE
        {
            UNGET
            DBL value = Parse_Float();
            if (value <= 0.0)
                Error ("gamma value must be positive.");
            else if ((value < 1.0) && (sceneData->gammaMode != kPOVList_GammaMode_None))
                // (we're not warning about below-1.0-values in non-gamma-corrected scenes, as it is likely to make sense there)
                PossibleError("Suspicious value %g specified for gamma. Did you mean %g?\n", value, 1.0/value);
            gamma = PowerLawGammaCurve::GetByDecodingGamma(value);
        }
        END_CASE
    END_EXPECT
    return gamma;
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Image_Map (PIGMENT *Pigment)
{
    int reg;
    ImageData *image;
    Vector2d Repeat;

    Parse_Begin();

    image = Parse_Image (IMAGE_FILE, true);
    image->Use = USE_COLOUR; // was true [trf]

    image->AllTransmitLegacyMode = (sceneData->EffectiveLanguageVersion() < 380);

    EXPECT                   /* Look for image_attribs */
        CASE (ONCE_TOKEN)
            image->Once_Flag=true;
        END_CASE

        CASE (INTERPOLATE_TOKEN)
            image->Interpolation_Type = (int)Parse_Float();
            switch(image->Interpolation_Type)
            {
                case BILINEAR:
                case BICUBIC:
                case NORMALIZED_DIST:
                    break;

                default:
                    Error("Invalid interpolate value.");
                    break;
            }
        END_CASE

        CASE (MAP_TYPE_TOKEN)
            image->Map_Type = (int) Parse_Float ();
            switch(image->Map_Type)
            {
                case PLANAR_MAP:
                case SPHERICAL_MAP:
                case CYLINDRICAL_MAP:
                case TORUS_MAP:
                case ANGULAR_MAP:
                    break;

                default:
                    Error("Invalid map_type value.");
                    break;
            }
        END_CASE

        CASE (USE_COLOUR_TOKEN)
            image->Use = USE_COLOUR; // was true [trf]
        END_CASE

        CASE (USE_INDEX_TOKEN)
            image->Use = USE_NONE; // was false [trf]
        END_CASE

        CASE (REPEAT_TOKEN)
            Parse_UV_Vect (Repeat);
            if ((Repeat[0]<=0.0) || (Repeat[1]<=0.0))
                Error("Zero or Negative Image Repeat Vector.");
            image->width  =  (DBL)image->iwidth  * Repeat[0];
            image->height =  (DBL)image->iheight * Repeat[1];
        END_CASE

        CASE (OFFSET_TOKEN)
            Parse_UV_Vect (image->Offset);
            image->Offset[U] *= (DBL)-image->iwidth;
            image->Offset[V] *= (DBL)-image->iheight;
        END_CASE

        CASE (ALPHA_TOKEN)
            VersionWarning(155, "Keyword ALPHA discontinued. Use FILTER instead.");
            // FALLTHROUGH

        CASE (FILTER_TOKEN)
            EXPECT_ONE
                CASE (ALL_TOKEN)
                    {
                        DBL filter;
                        filter = Parse_Float();
                        image->AllFilter = filter;
                        if (image->data->IsIndexed())
                        {
                            if (image->data->HasFilterTransmit() == false)
                            {
                                vector<Image::RGBFTMapEntry> map;
                                image->data->GetColourMap (map);
                                image->data->SetColourMap (map);
                            }
                            for(reg = 0; reg < image->data->GetColourMapSize(); reg++)
                            {
                                float r, g, b, f, t;

                                image->data->GetRGBFTIndexedValue(reg, r, g, b, f, t);
                                image->data->SetRGBFTIndexedValue(reg, r, g, b, filter, t);
                            }
                        }
                    }
                END_CASE

                OTHERWISE
                    UNGET
                    reg = (int)(Parse_Float() + 0.01);
                    if (image->data->IsIndexed() == false)
                        Not_With ("filter","non color-mapped image");
                    if ((reg < 0) || (reg >= image->data->GetColourMapSize()))
                        Error ("FILTER color register value out of range.");

                    Parse_Comma();
                    {
                        float r, g, b, f, t;

                        if (image->data->HasFilterTransmit() == false)
                        {
                            vector<Image::RGBFTMapEntry> map;
                            image->data->GetColourMap (map);
                            image->data->SetColourMap (map);
                        }
                        image->data->GetRGBFTIndexedValue(reg, r, g, b, f, t);
                        image->data->SetRGBFTIndexedValue(reg, r, g, b, Parse_Float(), t);
                    }
                END_CASE

            END_EXPECT
            Pigment->Flags |= HAS_FILTER;
        END_CASE

        CASE (TRANSMIT_TOKEN)
            EXPECT_ONE
                CASE (ALL_TOKEN)
                    {
                        DBL transmit;
                        transmit = Parse_Float();
                        image->AllTransmit = transmit;
                        if (image->data->IsIndexed())
                        {
                            if (image->data->HasFilterTransmit() == false)
                            {
                                vector<Image::RGBFTMapEntry> map;
                                image->data->GetColourMap (map);
                                image->data->SetColourMap (map);
                            }
                            for(reg = 0; reg < image->data->GetColourMapSize(); reg++)
                            {
                                float r, g, b, f, t;

                                image->data->GetRGBFTIndexedValue(reg, r, g, b, f, t);
                                image->data->SetRGBFTIndexedValue(reg, r, g, b, f, transmit);
                            }
                        }
                    }
                END_CASE

                OTHERWISE
                    UNGET
                    reg = (int)(Parse_Float() + 0.01);
                    if (image->data->IsIndexed() == false)
                        Not_With ("transmit","non color-mapped image");
                    if ((reg < 0) || (reg >= image->data->GetColourMapSize()))
                        Error ("TRANSMIT color register value out of range.");

                    Parse_Comma();
                    {
                        float r, g, b, f, t;

                        if (image->data->HasFilterTransmit() == false)
                        {
                            vector<Image::RGBFTMapEntry> map;
                            image->data->GetColourMap (map);
                            image->data->SetColourMap (map);
                        }
                        image->data->GetRGBFTIndexedValue(reg, r, g, b, f, t);
                        image->data->SetRGBFTIndexedValue(reg, r, g, b, f, Parse_Float());
                    }
                END_CASE

            END_EXPECT
            Pigment->Flags |= HAS_FILTER;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if (ImagePatternImpl* pattern = dynamic_cast<ImagePatternImpl*>(Pigment->pattern.get()))
        pattern->pImage = image;
    else
        POV_PATTERN_ASSERT(false);
    Parse_End();
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*    Scott Manley Added use of repeat vector
*
******************************************************************************/

void Parser::Parse_Bump_Map (TNORMAL *Tnormal)
{
    ImageData *image;
    Vector2d Repeat;

    Parse_Begin();

    image = Parse_Image(IMAGE_FILE);
    image->Use = USE_COLOUR; // was true [trf]

    EXPECT
        CASE (ONCE_TOKEN)
            image->Once_Flag=true;
        END_CASE

        CASE (MAP_TYPE_TOKEN)
            image->Map_Type = (int) Parse_Float ();
        END_CASE

        CASE (INTERPOLATE_TOKEN)
            image->Interpolation_Type = (int)Parse_Float();
        END_CASE

        CASE (BUMP_SIZE_TOKEN)
            Tnormal->Amount = Parse_Float ();
        END_CASE

        CASE (USE_COLOUR_TOKEN)
            image->Use = USE_COLOUR; // was true [trf]
        END_CASE

        CASE (USE_INDEX_TOKEN)
            image->Use = USE_NONE; // was false [trf]
        END_CASE

        CASE (REPEAT_TOKEN)
            Parse_UV_Vect (Repeat);
            if ((Repeat[0]<=0.0) || (Repeat[1]<=0.0))
                Error("Zero or Negative Image Repeat Vector.");
            image->width  =  (DBL)image->iwidth  * Repeat[0];
            image->height =  (DBL)image->iheight * Repeat[1];
        END_CASE

        CASE (OFFSET_TOKEN)
            Parse_UV_Vect (image->Offset);
            image->Offset[U] *= (DBL)-image->iwidth;
            image->Offset[V] *= (DBL)-image->iheight;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if (ImagePatternImpl* pattern = dynamic_cast<ImagePatternImpl*>(Tnormal->pattern.get()))
        pattern->pImage = image;
    else
        POV_PATTERN_ASSERT(false);

    Parse_End();
}



//******************************************************************************

PatternPtr Parser::ParseDensityFilePattern()
{
    shared_ptr<DensityFilePattern> pattern(new DensityFilePattern());
    UCS2String dummy;
    pattern->densityFile = Create_Density_File();
    GET(DF3_TOKEN);
    pattern->densityFile->Data->Name = Parse_C_String(true);
    shared_ptr<IStream> dfile = Locate_File(SysToUCS2String(pattern->densityFile->Data->Name).c_str(), POV_File_Data_DF3, dummy, true);
    if (dfile == nullptr)
        Error("Cannot read media density file.");
    Read_Density_File(dfile.get(), pattern->densityFile);
    return pattern;
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
*   Nathan Kopp
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

PatternPtr Parser::ParseImagePattern()
{
    shared_ptr<ImagePattern> pattern(new ImagePattern());
    pattern->waveFrequency = 0.0;

    ImageDataPtr& image = pattern->pImage;
    Vector2d Repeat;

    Parse_Begin();

    image = Parse_Image(IMAGE_FILE);
    image->Use = USE_COLOUR; // was true [trf]

    EXPECT
        CASE (ONCE_TOKEN)
            image->Once_Flag=true;
        END_CASE

        CASE (MAP_TYPE_TOKEN)
            image->Map_Type = (int) Parse_Float ();
        END_CASE

        CASE (INTERPOLATE_TOKEN)
            image->Interpolation_Type = (int)Parse_Float();
        END_CASE

        CASE (USE_COLOUR_TOKEN)
            image->Use = USE_COLOUR;
        END_CASE

        CASE (USE_INDEX_TOKEN)
            image->Use = USE_INDEX;
        END_CASE

        CASE (USE_ALPHA_TOKEN)
            image->Use = USE_ALPHA;
        END_CASE

        CASE (REPEAT_TOKEN)
            Parse_UV_Vect (Repeat);
            if ((Repeat[0]<=0.0) || (Repeat[1]<=0.0))
                Error("Zero or Negative Image Repeat Vector.");
            image->width  =  (DBL)image->iwidth  * Repeat[0];
            image->height =  (DBL)image->iheight * Repeat[1];
        END_CASE

        CASE (OFFSET_TOKEN)
            Parse_UV_Vect (image->Offset);
            image->Offset[U] *= (DBL)-image->iwidth;
            image->Offset[V] *= (DBL)-image->iheight;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End();

    return pattern;
}

//******************************************************************************

PatternPtr Parser::ParseJuliaPattern()
{
    shared_ptr<JuliaPattern> pattern(new JuliaPattern());
    Parse_UV_Vect(pattern->juliaCoord);
    Parse_Comma();
    pattern->maxIterations = Parse_Int_With_Minimum(1, "fractal iterations limit");
    return pattern;
}

//******************************************************************************

PatternPtr Parser::ParseMagnetPattern()
{
    int i = (int)Parse_Float();
    shared_ptr<FractalPattern> pattern;
    EXPECT_ONE
        CASE (MANDEL_TOKEN)
            switch(i)
            {
                case 1:
                    pattern = shared_ptr<FractalPattern>(new Magnet1MPattern());
                    break;
                case 2:
                    pattern = shared_ptr<FractalPattern>(new Magnet2MPattern());
                    break;
                default:
                    Error("Invalid magnet-mandel pattern type found. Valid types are 1 and 2.");
                    break;
            }
        END_CASE
        CASE (JULIA_TOKEN)
            {
                shared_ptr<JuliaPattern> juliaPattern;
                switch(i)
                {
                    case 1:
                        juliaPattern = shared_ptr<JuliaPattern>(new Magnet1JPattern());
                        break;
                    case 2:
                        juliaPattern = shared_ptr<JuliaPattern>(new Magnet2JPattern());
                        break;
                    default:
                        Error("Invalid magnet-julia pattern type found. Valid types are 1 and 2.");
                        break;
                }
                Parse_UV_Vect(juliaPattern->juliaCoord);
                Parse_Comma();
                pattern = juliaPattern;
            }
        END_CASE
        OTHERWISE
            Error("Invalid magnet pattern found. Valid types are 'mandel' and 'julia'.");
        END_CASE
    END_EXPECT
    pattern->maxIterations = Parse_Int_With_Minimum(1, "fractal iterations limit");
    return pattern;
}

//******************************************************************************

PatternPtr Parser::ParseMandelPattern()
{
    shared_ptr<Mandel2Pattern> pattern(new Mandel2Pattern());
    pattern->maxIterations = Parse_Int_With_Minimum(1, "fractal iterations limit");
    return pattern;
}

//******************************************************************************

PatternPtr Parser::ParsePotentialPattern()
{
    shared_ptr<PotentialPattern> pattern(new PotentialPattern());

    Parse_Begin();

    vector<ObjectPtr> tempObjects;
    Parse_Bound_Clip(tempObjects, false);

    if(tempObjects.size() != 1)
        Error ("object or object identifier expected.");
    if (!(tempObjects[0]->Type & POTENTIAL_OBJECT))
        Error ("blob or isosurface object expected.");
    pattern->pObject = tempObjects[0];

    Parse_End();

    EXPECT
        CASE (THRESHOLD_TOKEN)
            pattern->subtractThreshold = Parse_Bool();
        END_CASE
        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    return pattern;
}

//******************************************************************************

PatternPtr Parser::ParseSlopePattern()
{
    DBL low, high;
    SNGL Tot_Len;
    int i;

    shared_ptr<SlopePattern> pattern(new SlopePattern());

    EXPECT_ONE_CAT
        /* simple syntax */
        CASE_EXPRESS_UNGET
            Parse_Vector (pattern->slopeDirection);
        END_CASE

        /* new syntax */
        CASE(LEFT_CURLY_TOKEN)
            UNGET
            Parse_Begin();

            if (AllowToken(POINT_AT_TOKEN))
                pattern->pointAt = true;

            /* parse the vector */
            Parse_Vector (pattern->slopeDirection);

            /* allow low slope, high slope */
            Parse_Comma();
            low  = Allow_Float(0.0);
            Parse_Comma();
            high = Allow_Float(0.0);

            if ((high != 0.0) && (high == low))
            {
                pattern->slopeModLow   = 0.0;
                pattern->slopeModWidth = 0.0;
                Warning("Zero gradient range, ignoring.");
            }
            else
            {
                pattern->slopeModLow   = low;
                pattern->slopeModWidth = high - low;
            }

            EXPECT
                CASE(ALTITUDE_TOKEN)
                    if (pattern->pointAt)
                    {
                        Error("Keyword 'altitude' cannot be used with 'point_at' in slope pattern.");
                    }
                    else
                    {
                        mExperimentalFlags.slopeAltitude = true; // this feature is experimental
                        Parse_Vector (pattern->altitudeDirection);

                        /* allow low alt, high alt */
                        Parse_Comma();
                        DBL low  = Allow_Float(0.0);
                        Parse_Comma();
                        DBL high = Allow_Float(0.0);

                        if ((high != 0.0) && (high == low))
                        {
                            pattern->altitudeModLow   = 0.0;
                            pattern->altitudeModWidth = 0.0;
                            Warning("Zero gradient range, ignoring.");
                        }
                        else
                        {
                            pattern->altitudeModLow   = low;
                            pattern->altitudeModWidth = high - low;
                        }
                    }
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT


            Parse_End();
        END_CASE

        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT

    if (pattern->pointAt)
    {
        pattern->slopeLen = 1.0;
        pattern->altitudeLen = 0.0;
    }
    else
    {
        pattern->slopeLen = pattern->slopeDirection.length();
        pattern->altitudeLen = pattern->altitudeDirection.length();

        if (pattern->slopeLen != 0.0) pattern->slopeDirection.normalize();
        if (pattern->altitudeLen != 0.0) pattern->altitudeDirection.normalize();

        Tot_Len = pattern->slopeLen + pattern->altitudeLen;

        if (Tot_Len != 0.0)
        {
            pattern->slopeLen /= Tot_Len;
            pattern->altitudeLen  = 1.0 - pattern->slopeLen;
        }
        else
        {
            Error ("Zero length for both slope and gradient.");
        }
    }

    /* pre-process some stuff (look for shortcuts) for speed rendering speed */
    pattern->slopeAxis = pattern->altitudeAxis = 0; /* mark unused */
    if (!pattern->pointAt)
    {
        for (i=X; i<=Z; i++)
        {
            if      (pattern->slopeDirection[i] ==  1.0) { pattern->slopeAxis =   i+1;  break; }
            else if (pattern->slopeDirection[i] == -1.0) { pattern->slopeAxis = -(i+1); break; }
        }
        for (i=X; i<=Z; i++)
        {
            if      (pattern->altitudeDirection[i] ==  1.0) { pattern->altitudeAxis =   i+1;  break; }
            else if (pattern->altitudeDirection[i] == -1.0) { pattern->altitudeAxis = -(i+1); break; }
        }
    }
    return pattern;
}

//******************************************************************************

template<typename PATTERN_T>
PatternPtr Parser::ParseSpiralPattern()
{
    shared_ptr<SpiralPattern> pattern(new PATTERN_T());
    pattern->arms = (short)Parse_Float ();
    return pattern;
}

//******************************************************************************

void Parser::VerifyPavementPattern(shared_ptr<PavementPattern> pattern)
{
    const int valid6[]={1,1,3,7,22};
    const int valid4[]={1,1,2,5,12,35};
    const int valid3[]={1,1,1,3,4,12};
    pattern->Interior %= 3;
    pattern->Exterior %= 3;
    switch(pattern->Side)
    {
        case 4:
            if ((!(pattern->Tile)) || (pattern->Tile>6))
            {
                Error("number_of_tiles of pavement must be between 1 and 6.");
            }
            else if ((!(pattern->Number)) || (pattern->Number > valid4[pattern->Tile - 1]))
            {
                Error("pattern value for pavement is out of range. max per tile:(1,1,2,5,12,35)");
            }
            pattern->Form %=4;
            break;

        case 6:
            if ((!(pattern->Tile)) || (pattern->Tile > 5))
            {
                Error("number_of_tiles of pavement must be between 1 and 5.");
            }
            else if ((!(pattern->Number)) || (pattern->Number > valid6[pattern->Tile - 1]))
            {
                Error("pattern value for pavement is out of range. max per tile:(1,1,3,7,22)");
            }
            pattern->Form %= 3;
            break;

        case 3:
            if ((!(pattern->Tile)) || (pattern->Tile > 6))
            {
                Error("number_of_tiles of pavement must be between 1 and 6.");
            }
            else if ((!(pattern->Number)) || (pattern->Number > valid3[pattern->Tile - 1]))
            {
                Error("pattern value for pavement is out of range. max per tile:(1,1,1,3,4,12)");
            }
            pattern->Form %= 3;
            break;

        default:
            Error("number_of_sides of pavement must be 3, 4 or 6.");
    }
}

//******************************************************************************

void Parser::VerifyTilingPattern(shared_ptr<TilingPattern> pattern)
{
    if ((pattern->tilingType < 1) || (pattern->tilingType > 27))
        Error("Tiling number must be between 1 and 27.");
}

//******************************************************************************

void Parser::VerifyPattern(PatternPtr basicPattern)
{
    if (shared_ptr<PavementPattern> pattern = std::dynamic_pointer_cast<PavementPattern>(basicPattern))
        VerifyPavementPattern(pattern);
    else if (shared_ptr<TilingPattern> pattern = std::dynamic_pointer_cast<TilingPattern>(basicPattern))
        VerifyTilingPattern(pattern);
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Pigment (PIGMENT **Pigment_Ptr)
{
    if (AllowToken(PIGMENT_ID_TOKEN))
    {
        Destroy_Pigment(*Pigment_Ptr);
        *Pigment_Ptr = Copy_Pigment(CurrentTokenDataPtr<PIGMENT*>());
    }

    Parse_Pattern<GenericPigmentBlendMap>(*Pigment_Ptr,kBlendMapType_Pigment);

    if (Not_In_Default && ((*Pigment_Ptr)->Type == NO_PATTERN))
    {
        VersionWarning(155, "Pigment type unspecified or not 1st item.");
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

template<typename MAP_T, typename PATTERN_T>
void Parser::Parse_Pattern (PATTERN_T *New, BlendMapTypeId TPat_Type)
{
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    ClassicTurbulence *Local_Turb;
    int i;
    TraceThreadData *Thread = GetParserDataPtr();
    ContinuousPattern* pContinuousPattern;

    EXPECT_ONE_CAT
        CASE (AGATE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new AgatePattern());
            Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());
        END_CASE

        CASE (BOZO_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new BozoPattern());
        END_CASE

        CASE (FUNCTION_TOKEN)
            {
                New->Type = GENERIC_PATTERN;
                shared_ptr<FunctionPattern> pattern(new FunctionPattern());
                pattern->pFn = new FunctionVM::CustomFunction(mpFunctionVM.get(), Parse_Function());
                New->pattern = pattern;
            }
        END_CASE

        CASE (USER_DEFINED_TOKEN)
            {
                if ((TPat_Type != kBlendMapType_Pigment) && (TPat_Type != kBlendMapType_Density))
                    Only_In("user_defined", "pigment or density");
                New->Type = COLOUR_PATTERN;
                shared_ptr<ColourFunctionPattern> pattern(new ColourFunctionPattern());
                Parse_Begin();
                Parse_FunctionOrContentList (pattern->pFn, 5, false);
                Parse_End();
                New->pattern = pattern;
            }
        END_CASE

        CASE(PIGMENT_PATTERN_TOKEN)
            {
                Parse_Begin();
                New->Type = GENERIC_PATTERN;
                shared_ptr<PigmentPattern> pattern(new PigmentPattern());
                pattern->pPigment = Create_Pigment();
                Parse_Pigment(&(pattern->pPigment));
                Post_Pigment(pattern->pPigment);
                Parse_End();
                New->pattern = pattern;
            }
        END_CASE

        CASE (GRANITE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new GranitePattern());
        END_CASE

        CASE (LEOPARD_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new LeopardPattern());
        END_CASE

        CASE (MARBLE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new MarblePattern());
        END_CASE

        CASE (MANDEL_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseMandelPattern();
        END_CASE

        CASE (JULIA_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseJuliaPattern();
        END_CASE

        CASE (MAGNET_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseMagnetPattern();
        END_CASE

        CASE (ONION_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new OnionPattern());
        END_CASE

        CASE (SPIRAL1_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseSpiralPattern<Spiral1Pattern>();
        END_CASE

        CASE (SPIRAL2_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseSpiralPattern<Spiral2Pattern>();
        END_CASE

        CASE (SPOTTED_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new SpottedPattern());
        END_CASE

        CASE (WOOD_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new WoodPattern());
        END_CASE

        CASE (GRADIENT_TOKEN)
            {
                New->Type = GENERIC_PATTERN;
                shared_ptr<GradientPattern> pattern(new GradientPattern());
                Parse_Vector (Local_Vector);
                Local_Vector.normalize();
                pattern->gradient = Local_Vector;
                New->pattern = pattern;
            }
        END_CASE

        CASE (RADIAL_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new RadialPattern());
        END_CASE

        CASE (CRACKLE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new CracklePattern());
        END_CASE

        CASE_COLOUR_UNGET
            if ((TPat_Type != kBlendMapType_Pigment) && (TPat_Type != kBlendMapType_Density))
                Only_In("color","pigment or density");
            New->Type = PLAIN_PATTERN;
            New->pattern = PatternPtr(new PlainPattern());
            Parse_Colour ((reinterpret_cast<PIGMENT *>(New))->colour);
        END_CASE

        CASE (UV_MAPPING_TOKEN)
            New->Type = UV_MAP_PATTERN;
            New->pattern = PatternPtr(new PlainPattern());
            New->Blend_Map = Parse_Item_Into_Blend_List<MAP_T>(TPat_Type);
        END_CASE

        CASE (CHECKER_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new CheckerPattern());
            New->Blend_Map = Parse_Blend_List<MAP_T>(2,New->pattern->GetDefaultBlendMap(),TPat_Type);
            if (TPat_Type == kBlendMapType_Normal)
                (reinterpret_cast<TNORMAL *>(New))->Delta = 0.02;
        END_CASE

        CASE (OBJECT_TOKEN)
            {
                Parse_Begin();
                vector<ObjectPtr> tempObjects;
                Parse_Bound_Clip(tempObjects, false);
                if(tempObjects.size() != 1)
                    Error ("object or object identifier expected.");
                New->Type = GENERIC_PATTERN;
                shared_ptr<ObjectPattern> pattern(new ObjectPattern());
                pattern->pObject = tempObjects[0];
                New->pattern = pattern;
                New->Blend_Map = Parse_Blend_List<MAP_T>(2,New->pattern->GetDefaultBlendMap(),TPat_Type);
                Parse_End();
            }
        END_CASE

        CASE (CELLS_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new CellsPattern());
        END_CASE

        CASE (BRICK_TOKEN)
            if (!dynamic_cast<BrickPattern*>(New->pattern.get()))
            {
                New->Type = GENERIC_PATTERN;
                New->pattern = PatternPtr(new BrickPattern());
            }
            New->Blend_Map = Parse_Blend_List<MAP_T>(2,New->pattern->GetDefaultBlendMap(),TPat_Type);
            if (TPat_Type == kBlendMapType_Normal)
                (reinterpret_cast<TNORMAL *>(New))->Delta = 0.02;
        END_CASE

        CASE (HEXAGON_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new HexagonPattern());
            New->Blend_Map = Parse_Blend_List<MAP_T>(3,New->pattern->GetDefaultBlendMap(),TPat_Type);
            if (TPat_Type == kBlendMapType_Normal)
                (reinterpret_cast<TNORMAL *>(New))->Delta = 0.02;
        END_CASE

        CASE (SQUARE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new SquarePattern());
            New->Blend_Map = Parse_Blend_List<MAP_T>(4,New->pattern->GetDefaultBlendMap(),TPat_Type);
            if (TPat_Type == kBlendMapType_Normal)
                (reinterpret_cast<TNORMAL *>(New))->Delta = 0.02;
        END_CASE

        CASE (TRIANGULAR_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new TriangularPattern());
            New->Blend_Map = Parse_Blend_List<MAP_T>(6,New->pattern->GetDefaultBlendMap(),TPat_Type);
            if (TPat_Type == kBlendMapType_Normal)
                (reinterpret_cast<TNORMAL *>(New))->Delta = 0.02;
        END_CASE

        // JN2007: Cubic pattern
        CASE (CUBIC_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new CubicPattern());
            New->Blend_Map = Parse_Blend_List<MAP_T>(6,New->pattern->GetDefaultBlendMap(),TPat_Type);
            if (TPat_Type == kBlendMapType_Normal)
                (reinterpret_cast<TNORMAL *>(New))->Delta = 0.02;
        END_CASE

        CASE (IMAGE_MAP_TOKEN)
            if (TPat_Type != kBlendMapType_Pigment)
                Only_In("image_map","pigment");
            New->Type = IMAGE_MAP_PATTERN;
            New->pattern = PatternPtr(new ColourImagePattern());
            Parse_Image_Map (reinterpret_cast<PIGMENT *>(New));
        END_CASE

        CASE (BUMP_MAP_TOKEN)
            {
                if (TPat_Type != kBlendMapType_Normal)
                    Only_In("bump_map","normal");
                New->Type = BITMAP_PATTERN;
                shared_ptr<ImagePattern> pattern(new ImagePattern());
                pattern->waveFrequency = 0.0;
                New->pattern = pattern;
                Parse_Bump_Map (reinterpret_cast<TNORMAL *>(New));
            }
        END_CASE

        CASE (WAVES_TOKEN)
            New->Type = WAVES_PATTERN;
            New->pattern = PatternPtr(new WavesPattern());
        END_CASE

        CASE (RIPPLES_TOKEN)
            New->Type = RIPPLES_PATTERN;
            New->pattern = PatternPtr(new RipplesPattern());
        END_CASE

        CASE (WRINKLES_TOKEN)
            New->Type = WRINKLES_PATTERN;
            New->pattern = PatternPtr(new WrinklesPattern());
        END_CASE

        CASE (BUMPS_TOKEN)
            New->Type = BUMPS_PATTERN;
            New->pattern = PatternPtr(new BumpsPattern());
        END_CASE

        CASE (DENTS_TOKEN)
            New->Type = DENTS_PATTERN;
            New->pattern = PatternPtr(new DentsPattern());
        END_CASE

        CASE (QUILTED_TOKEN)
            New->Type = QUILTED_PATTERN;
            New->pattern = PatternPtr(new QuiltedPattern());
        END_CASE

        CASE (FACETS_TOKEN)
            if (TPat_Type != kBlendMapType_Normal)
                Only_In("facets","normal");
            New->Type = FACETS_PATTERN;
            New->pattern = PatternPtr(new FacetsPattern());
        END_CASE

        CASE (AVERAGE_TOKEN)
            New->Type = AVERAGE_PATTERN;
            New->pattern = PatternPtr(new AveragePattern());
        END_CASE

        CASE (IMAGE_PATTERN_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseImagePattern();
        END_CASE

        CASE (PLANAR_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new PlanarPattern());
        END_CASE

        CASE (BOXED_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new BoxedPattern());
        END_CASE

        CASE (SPHERICAL_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new SphericalPattern());
        END_CASE

        CASE (CYLINDRICAL_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new CylindricalPattern());
        END_CASE

        CASE (DENSITY_FILE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseDensityFilePattern();
        END_CASE

        CASE (SLOPE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseSlopePattern();
        END_CASE

        CASE (AOI_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new AOIPattern());
        END_CASE

        CASE (PAVEMENT_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new PavementPattern());
        END_CASE

        CASE (TILING_TOKEN)
            {
                New->Type = GENERIC_PATTERN;
                shared_ptr<TilingPattern> pattern(new TilingPattern());
                pattern->tilingType = (unsigned char)Parse_Float();
                New->pattern = pattern;
            }
        END_CASE

        CASE (POTENTIAL_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParsePotentialPattern();
        END_CASE

        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT     /* Concludes pattern_body */

    if (TPat_Type == kBlendMapType_Normal)
    {
        Parse_Comma();
        (reinterpret_cast<TNORMAL *>(New))->Amount = Allow_Float ((reinterpret_cast<TNORMAL *>(New))->Amount );
    }

    EXPECT         /* Look for pattern_modifier */
        CASE (ACCURACY_TOKEN)
            if(TPat_Type != kBlendMapType_Normal)
            {
                Error("accuracy can only be used with normal patterns.");
            }
            (reinterpret_cast<TNORMAL *>(New))->Delta = Parse_Float();
        END_CASE

        CASE (SOLID_TOKEN)
            if (CracklePattern* pattern = dynamic_cast<CracklePattern*>(New->pattern.get()))
                pattern->crackleIsSolid = true;
            else
                Only_In("solid", "crackle");
        END_CASE

        CASE (EXTERIOR_TOKEN)
            if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
            {
                pattern->Exterior = (unsigned char)Parse_Float();
            }
            else if (FractalPattern* pattern = dynamic_cast<FractalPattern*>(New->pattern.get()))
            {
                pattern->exteriorType = (int)Parse_Float();
                if((pattern->exteriorType < 0) || (pattern->exteriorType > 8))
                    Error("Invalid fractal pattern exterior type. Valid types are 0 to 8.");
                Parse_Comma();
                pattern->exteriorFactor = Parse_Float();
            }
            else
            {
                Only_In("exterior", "mandel, julia, magnet or pavement");
            }
        END_CASE

        CASE (INTERIOR_TOKEN)
            if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
            {
                pattern->Interior = (unsigned char)Parse_Float();
            }
            else if (FractalPattern* pattern = dynamic_cast<FractalPattern*>(New->pattern.get()))
            {
                pattern->interiorType = (int)Parse_Float();
                if((pattern->interiorType < 0) || (pattern->interiorType > 6))
                    Error("Invalid fractal pattern interior type. Valid types are 0 to 6.");
                Parse_Comma();
                pattern->interiorFactor = Parse_Float();
            }
            else
            {
                Only_In("exterior", "mandel, julia, magnet or pavement");
            }
        END_CASE

        CASE (EXPONENT_TOKEN)
            if (JuliaPattern* oldPattern = dynamic_cast<JuliaPattern*>(New->pattern.get()))
            {
                i = (int)Parse_Float();
                switch(i)
                {
                    case 2:
                        New->pattern = PatternPtr(new JuliaPattern(*oldPattern));
                        break;
                    case 3:
                        New->pattern = PatternPtr(new Julia3Pattern(*oldPattern));
                        break;
                    case 4:
                        New->pattern = PatternPtr(new Julia4Pattern(*oldPattern));
                        break;
                    default:
                        if((i > 4) && (i <= kFractalMaxExponent))
                        {
                            shared_ptr<JuliaXPattern> pattern(new JuliaXPattern(*oldPattern));
                            pattern->fractalExponent = i;
                            New->pattern = pattern;
                        }
                        else
                        {
                            New->pattern = PatternPtr(new JuliaPattern(*oldPattern));
                            Warning("Invalid julia pattern exponent found. Supported exponents are 2 to %i.\n"
                                    "Using default exponent 2.", kFractalMaxExponent);
                        }
                        break;
                }
            }
            else if (MandelPattern* oldPattern = dynamic_cast<MandelPattern*>(New->pattern.get()))
            {
                i = (int)Parse_Float();
                switch(i)
                {
                    case 2:
                        New->pattern = PatternPtr(new Mandel2Pattern(*oldPattern));
                        break;
                    case 3:
                        New->pattern = PatternPtr(new Mandel3Pattern(*oldPattern));
                        break;
                    case 4:
                        New->pattern = PatternPtr(new Mandel4Pattern(*oldPattern));
                        break;
                    default:
                        if((i > 4) && (i <= kFractalMaxExponent))
                        {
                            shared_ptr<MandelXPattern> pattern(new MandelXPattern(*oldPattern));
                            pattern->fractalExponent = i;
                            New->pattern = pattern;
                        }
                        else
                        {
                            New->pattern = PatternPtr(new Mandel2Pattern(*oldPattern));
                            Warning("Invalid mandel pattern exponent found. Supported exponents are 2 to %i.\n"
                                    "Using default exponent 2.", kFractalMaxExponent);
                        }
                        break;
                }
            }
            else
                Only_In("exponent", "mandel or julia");
        END_CASE

        CASE (COORDS_TOKEN)
            if (FacetsPattern* pattern = dynamic_cast<FacetsPattern*>(New->pattern.get()))
                pattern->facetsCoords = Parse_Float();
            else
                Only_In("coords", "facets");
        END_CASE

        CASE (SIZE_TOKEN)
            if (FacetsPattern* pattern = dynamic_cast<FacetsPattern*>(New->pattern.get()))
                pattern->facetsSize = Parse_Float();
            else
                Only_In("size", "facets");
        END_CASE

        CASE (METRIC_TOKEN)
            if (FacetsPattern* pattern = dynamic_cast<FacetsPattern*>(New->pattern.get()))
            {
                Parse_Vector(Local_Vector);
                pattern->facetsMetric = Local_Vector[X];
            }
            else if (CracklePattern* pattern = dynamic_cast<CracklePattern*>(New->pattern.get()))
            {
                // Vector for backwards compatibility
                // the only component used was always X.
                Parse_Vector(Local_Vector);
                pattern->crackleMetric = Local_Vector[X];
            }
            else
                Only_In("metric", "facets or crackle");
        END_CASE

        CASE (FORM_TOKEN)
            if (CracklePattern* pattern = dynamic_cast<CracklePattern*>(New->pattern.get()))
                Parse_Vector(pattern->crackleForm);
            else if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
                pattern->Form = ((unsigned char)Parse_Float());
            else
                Only_In("form", "crackle or pavement");
        END_CASE

        CASE (OFFSET_TOKEN)
            if (CracklePattern* pattern = dynamic_cast<CracklePattern*>(New->pattern.get()))
                pattern->crackleOffset = Parse_Float();
            else
                Only_In("offset", "crackle");
        END_CASE

        CASE (TURBULENCE_TOKEN)
            Local_Turb=Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());
            Parse_Vector(Local_Turb->Turbulence);
        END_CASE

        CASE (COLOUR_MAP_TOKEN)
            if ((TPat_Type != kBlendMapType_Pigment) && (TPat_Type != kBlendMapType_Density))
            {
                Only_In("color_map","pigment");
            }
            if (New->Type == AVERAGE_PATTERN || !New->pattern->CanMap())
                Error("Cannot use color_map with this pattern type.");
            New->Blend_Map = Parse_Colour_Map<MAP_T> ();
        END_CASE

        CASE (PIGMENT_MAP_TOKEN)
            if (TPat_Type != kBlendMapType_Pigment)
            {
                Only_In("pigment_map","pigment");
            }
            if (!New->pattern->CanMap())
                Not_With ("pigment_map","this pigment type");
            New->Blend_Map = Parse_Blend_Map<MAP_T> (kBlendMapType_Pigment,New->Type);
        END_CASE

        CASE (DENSITY_MAP_TOKEN)
            if (TPat_Type != kBlendMapType_Density)
            {
                Only_In("density_map","density");
            }
            if (!New->pattern->CanMap())
                Not_With ("density_map","this density type");
            New->Blend_Map = Parse_Blend_Map<MAP_T> (kBlendMapType_Density,New->Type);
        END_CASE

        CASE (SLOPE_MAP_TOKEN)
            if (TPat_Type != kBlendMapType_Normal)
            {
                Only_In("slope_map","normal");
            }
            if (New->Type == AVERAGE_PATTERN || !New->pattern->CanMap())
                Not_With ("slope_map","this normal type");
            New->Blend_Map = Parse_Blend_Map<MAP_T> (kBlendMapType_Slope,New->Type);
        END_CASE

        CASE (NORMAL_MAP_TOKEN)
            if (TPat_Type != kBlendMapType_Normal)
            {
                Only_In("normal_map","normal");
            }
            if (New->Type == FACETS_PATTERN || !New->pattern->CanMap())
                Not_With ("normal_map","this normal type");
            New->Blend_Map = Parse_Blend_Map<MAP_T> (kBlendMapType_Normal,New->Type);
        END_CASE

        CASE (TEXTURE_MAP_TOKEN)
            if (TPat_Type != kBlendMapType_Texture)
            {
                Only_In("texture_map","texture");
            }
            if (!New->pattern->CanMap())
                Not_With ("texture_map","this pattern type");
            New->Blend_Map = Parse_Blend_Map<MAP_T> (kBlendMapType_Texture,New->Type);
        END_CASE

        CASE (QUICK_COLOUR_TOKEN)
            if (TPat_Type != kBlendMapType_Pigment)
            {
                Only_In("quick_color","pigment");
            }
            Parse_Colour ((reinterpret_cast<PIGMENT *>(New))->Quick_Colour);
        END_CASE

        CASE (CONTROL0_TOKEN)
            if (QuiltedPattern* pattern = dynamic_cast<QuiltedPattern*>(New->pattern.get()))
                pattern->Control0 = Parse_Float ();
            else
                Not_With ("control0","this pattern");
        END_CASE

        CASE (CONTROL1_TOKEN)
            if (QuiltedPattern* pattern = dynamic_cast<QuiltedPattern*>(New->pattern.get()))
                pattern->Control1 = Parse_Float ();
            else
                Not_With ("control1","this pattern");
        END_CASE

        CASE (OCTAVES_TOKEN)
            Local_Turb=Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());
            Local_Turb->Octaves = (int)Parse_Float();
            if(Local_Turb->Octaves < 1)
                Local_Turb->Octaves = 1;
            if(Local_Turb->Octaves > 10)  /* Avoid DOMAIN errors */
                Local_Turb->Octaves = 10;
        END_CASE

        CASE (OMEGA_TOKEN)
            Local_Turb=Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());
            Local_Turb->Omega = Parse_Float();
        END_CASE

        CASE (LAMBDA_TOKEN)
            Local_Turb=Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());
            Local_Turb->Lambda = Parse_Float();
        END_CASE

        CASE (FREQUENCY_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveFrequency = Parse_Float();
            else
            {
                Warning("frequency has no effect on discrete patterns");
                Parse_Float();
            }
        END_CASE

        CASE (RAMP_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveType = kWaveType_Ramp;
            else
                Warning("ramp_wave has no effect on discrete patterns");
        END_CASE

        CASE (TRIANGLE_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveType = kWaveType_Triangle;
            else
                Warning("triangle_wave has no effect on discrete patterns");
        END_CASE

        CASE (SINE_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveType = kWaveType_Sine;
            else
                Warning("sine_wave has no effect on discrete patterns");
        END_CASE

        CASE (SCALLOP_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveType = kWaveType_Scallop;
            else
                Warning("scallop_wave has no effect on discrete patterns");
        END_CASE

        CASE (CUBIC_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveType = kWaveType_Cubic;
            else
                Warning("cubic_wave has no effect on discrete patterns");
        END_CASE

        CASE (POLY_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
            {
                pContinuousPattern->waveType = kWaveType_Poly;
                pContinuousPattern->waveExponent  = Allow_Float(pContinuousPattern->waveExponent);
            }
            else
            {
                Warning("poly_wave has no effect on discrete patterns");
                Allow_Float(0.0);
            }
        END_CASE

        CASE (PHASE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->wavePhase = Parse_Float();
            else
            {
                Warning("phase has no effect on discrete patterns");
                Parse_Float();
            }
        END_CASE

        CASE (BUMP_SIZE_TOKEN)
            if (TPat_Type != kBlendMapType_Normal)
                Only_In ("bump_size","normal");
            (reinterpret_cast<TNORMAL *>(New))->Amount = Parse_Float ();
        END_CASE

        CASE (NOISE_GENERATOR_TOKEN)
        {
            int noise_generator;
            noise_generator = (int) Parse_Float();
            if (noise_generator < kNoiseGen_Min || noise_generator > kNoiseGen_Max)
                Error ("Value for noise_generator in global_settings must be between %i (inclusive) and %i (inclusive).", kNoiseGen_Min, kNoiseGen_Max);
            New->pattern->noiseGenerator = noise_generator;
        }
        END_CASE

        CASE (AGATE_TURB_TOKEN)
            if (AgatePattern* pattern = dynamic_cast<AgatePattern*>(New->pattern.get()))
            {
                pattern->agateTurbScale = Parse_Float();
                Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());   // agate needs Octaves, Lambda etc.
            }
            else
                Not_With ("agate_turb","non-agate");
        END_CASE

        CASE (BRICK_SIZE_TOKEN)
            if (BrickPattern* pattern = dynamic_cast<BrickPattern*>(New->pattern.get()))
                Parse_Vector(pattern->brickSize);
            else
                Not_With ("brick_size","non-brick");
        END_CASE

        CASE (MORTAR_TOKEN)
            if (BrickPattern* pattern = dynamic_cast<BrickPattern*>(New->pattern.get()))
                pattern->mortar = Parse_Float()-EPSILON*2.0;
            else
                Not_With ("mortar","non-brick");
        END_CASE

        CASE (INTERPOLATE_TOKEN)
            if (DensityFilePattern* pattern = dynamic_cast<DensityFilePattern*>(New->pattern.get()))
                pattern->densityFile->Interpolation = (int)Parse_Float();
            else
                Not_With ("interpolate","non-density_file");
        END_CASE

        CASE (NUMBER_OF_SIDES_TOKEN)
            if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
                pattern->Side = (unsigned char)Parse_Float();
            else
                Only_In("number_of_sides","pavement");
        END_CASE

        CASE (NUMBER_OF_TILES_TOKEN)
            if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
                pattern->Tile = (unsigned char)Parse_Float();
            else
                Only_In("number_of_tiles","pavement");
        END_CASE

        CASE (PATTERN_TOKEN)
            if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
                pattern->Number = (unsigned char)Parse_Float();
            else
                Only_In("pattern","pavement");
        END_CASE

        CASE (WARP_TOKEN)
            Parse_Warp(New->pattern->warps);
        END_CASE

        CASE (TRANSLATE_TOKEN)
            Parse_Vector (Local_Vector);
            Translate_Tpattern (New, Local_Vector);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (Local_Vector);
            Rotate_Tpattern (New, Local_Vector);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector (Local_Vector);
            Scale_Tpattern (New, Local_Vector);
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix(Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Transform_Tpattern (New, &Local_Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Transform_Tpattern (New, Parse_Transform(&Local_Trans));
        END_CASE

        CASE (NO_BUMP_SCALE_TOKEN)
            Set_Flag(New, DONT_SCALE_BUMPS_FLAG);
        END_CASE

        CASE (REPEAT_TOKEN)
            if (CracklePattern* pattern = dynamic_cast<CracklePattern*>(New->pattern.get()))
            {
                Parse_Vector(Local_Vector);
                pattern->repeat = IntVector3d(Local_Vector);
                if ((pattern->repeat.x() < 0) ||
                    (pattern->repeat.y() < 0) ||
                    (pattern->repeat.z() < 0))
                    Error("Repeat vector must be non-negative.");
            }
            else
                Only_In("repeat","crackle");
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if ((New->Type == AVERAGE_PATTERN) && (New->Blend_Map == nullptr))
    {
        Error("Average must have map.");
    }

    if ((TPat_Type == kBlendMapType_Texture) && (New->Type != PLAIN_PATTERN) &&
        (New->Blend_Map==nullptr))
    {
        Error("Patterned texture must have texture_map.");
    }

    VerifyPattern(New->pattern);

    if (!New->pattern->Precompute())
        Error("Unspecified parse error in pattern.");
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Tnormal (TNORMAL **Tnormal_Ptr)
{
    if (AllowToken(NORMAL_ID_TOKEN))
    {
        Destroy_Tnormal(*Tnormal_Ptr);
        *Tnormal_Ptr = Copy_Tnormal(CurrentTokenDataPtr<TNORMAL*>());
    }

    if (*Tnormal_Ptr == nullptr)
    {
        if (Default_Texture->Tnormal != nullptr)
        {
            *Tnormal_Ptr = Copy_Tnormal ((Default_Texture->Tnormal));
        }
        else
        {
            *Tnormal_Ptr = Create_Tnormal ();
        }
    }
    Parse_Pattern<GenericNormalBlendMap>(*Tnormal_Ptr,kBlendMapType_Normal);

    if ((*Tnormal_Ptr)->Type == NO_PATTERN)
        Error ("No normal type given.");
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Finish (FINISH **Finish_Ptr)
{
    TransColour Temp_Colour;
    FINISH *New;
    Vector3d Local_Vector;
    bool diffuseAdjust = false;
    bool phongAdjust = false;
    bool specularAdjust = false;
    bool ambientSet = false;
    bool emissionSet = false;

    Parse_Begin ();

    if (AllowToken(FINISH_ID_TOKEN))
    {
        if (*Finish_Ptr)
            delete *Finish_Ptr;
        *Finish_Ptr = Copy_Finish(CurrentTokenDataPtr<FINISH*>());
    }

    New = *Finish_Ptr;

    EXPECT        /* Look for zero or more finish_body */
        CASE (CONSERVE_ENERGY_TOKEN)
            New->Conserve_Energy = true;
        END_CASE

        CASE (AMBIENT_TOKEN)
            ambientSet = true;
            Parse_Colour(New->Ambient);
        END_CASE

        CASE (EMISSION_TOKEN)
            emissionSet = true;
            Parse_Colour(New->Emission);
        END_CASE

        CASE (BRILLIANCE_TOKEN)
            New->Brilliance = Parse_Float ();
#if POV_PARSER_EXPERIMENTAL_BRILLIANCE_OUT
            Parse_Comma();
            New->BrillianceOut = Allow_Float(1.0);
#endif
        END_CASE

        CASE (DIFFUSE_TOKEN)
            diffuseAdjust = AllowToken(ALBEDO_TOKEN);
            New->Diffuse = Parse_Float ();
            Parse_Comma();
            New->DiffuseBack = Allow_Float(0.0);
            if (New->DiffuseBack != 0.0)
                mExperimentalFlags.backsideIllumination = true;
        END_CASE

        CASE (REFLECTION_TOKEN)
        {
            bool found_second_color = false;
            EXPECT_ONE_CAT
                /* old syntax */
                CASE_EXPRESS_UNGET
                    Parse_Colour(New->Reflection_Max);
                    New->Reflection_Min = New->Reflection_Max;
                    New->Reflection_Falloff = 1;
                    New->Reflection_Fresnel = (New->Fresnel != 0.0);
                END_CASE

                /* new syntax */
                CASE(LEFT_CURLY_TOKEN)
                    UNGET
                    Parse_Begin();

                    /* parse the first colour */
                    Parse_Colour(New->Reflection_Min);
                    Parse_Comma();

                    /* look for a second color */
                    EXPECT_ONE_CAT
                        CASE_EXPRESS_UNGET
                            Parse_Colour(New->Reflection_Max);
                            found_second_color = true;
                        END_CASE

                        OTHERWISE
                            UNGET
                            /* by default, use reflection min = reflection max */
                            New->Reflection_Max = New->Reflection_Min;
                        END_CASE
                    END_EXPECT

                    /* look for a other options */
                    EXPECT
                        CASE(FRESNEL_TOKEN)
                            New->Reflection_Fresnel = ((int) Allow_Float(1.0) != 0);
                        END_CASE

                        CASE (FALLOFF_TOKEN)
                            New->Reflection_Falloff = Parse_Float();
                        END_CASE

                        CASE (EXPONENT_TOKEN)
                            New->Reflect_Exp = 1.0 / Parse_Float ();
                        END_CASE

                        CASE (METALLIC_TOKEN)
                            New->Reflect_Metallic = Allow_Float(1.0);
                        END_CASE

                        OTHERWISE
                            UNGET
                            EXIT
                        END_CASE
                    END_EXPECT

                    Parse_End();
                END_CASE

                OTHERWISE
                    UNGET
                END_CASE

            END_EXPECT
        }
        END_CASE

        CASE (REFLECTION_EXPONENT_TOKEN)
            New->Reflect_Exp = 1.0 / Parse_Float ();
        END_CASE

        CASE (PHONG_TOKEN)
            phongAdjust = AllowToken(ALBEDO_TOKEN);
            New->Phong = Parse_Float ();
        END_CASE

        CASE (PHONG_SIZE_TOKEN)
            New->Phong_Size = Parse_Float ();
        END_CASE

        CASE (SPECULAR_TOKEN)
            specularAdjust = AllowToken(ALBEDO_TOKEN);
            New->Specular = Parse_Float ();
        END_CASE

        CASE (ROUGHNESS_TOKEN)
            New->Roughness = Parse_Float ();
            if (New->Roughness != 0.0)
                New->Roughness = 1.0/New->Roughness; /* CEY 12/92 */
            else
                Warning("Zero roughness used.");
        END_CASE

        CASE (METALLIC_TOKEN)
            New->Metallic = 1.0;
            EXPECT_ONE_CAT
                CASE_FLOAT_UNGET
                    New->Metallic = Parse_Float();
                END_CASE

                OTHERWISE
                    UNGET
                END_CASE
            END_EXPECT
        END_CASE

        CASE(FRESNEL_TOKEN)
            New->Fresnel = Allow_Float(1.0);
            New->Reflection_Fresnel = (New->Fresnel != 0.0);
        END_CASE

        CASE (CRAND_TOKEN)
            New->Crand = Parse_Float();
        END_CASE

        CASE (IRID_TOKEN)                     /* DMF */
            Parse_Begin();
            New->Irid = Parse_Float();

            EXPECT
                CASE (THICKNESS_TOKEN)           /* DMF */
                    New->Irid_Film_Thickness = Parse_Float();
                END_CASE

                CASE (TURBULENCE_TOKEN)                /* DMF */
                    Parse_Vector(Local_Vector);
                    New->Irid_Turb = Local_Vector[X];
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            Parse_End();
        END_CASE

        CASE (IOR_TOKEN)
            New->Temp_IOR = Parse_Float();
            Warn_Compat(false, "Index of refraction value should be specified in 'interior{...}' statement.");
        END_CASE

        CASE (CAUSTICS_TOKEN)
            New->Temp_Caustics = Parse_Float();
            Warn_Compat(false, "Caustics value should be specified in 'interior{...}' statement.");
        END_CASE

        CASE (REFRACTION_TOKEN)
            New->Temp_Refract = Parse_Float();
            Warn_Compat(false, "Refraction value unnecessary to turn on refraction.\nTo attenuate, the fade_power and fade_distance keywords should be specified in 'interior{...}' statement.");
        END_CASE

        CASE (SUBSURFACE_TOKEN)
            mExperimentalFlags.subsurface = true;
            New->UseSubsurface = true;
            Parse_Begin();
            EXPECT
                CASE (ANISOTROPY_TOKEN)
                    Parse_Colour(New->SubsurfaceAnisotropy);
                END_CASE
                CASE (TRANSLUCENCY_TOKEN)
                    Parse_Colour(New->SubsurfaceTranslucency);
                END_CASE
                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            Parse_End();
        END_CASE

        CASE (USE_ALPHA_TOKEN)
            New->AlphaKnockout = ((int) Allow_Float(1.0) != 0);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE

    END_EXPECT    /* End of finish_body */

#if 0
    EXPECT        /* Look for finish_mods */

/*      CASE none implemented - if you implement one remember to remove the #if 0 and the #endif
        around this block of code
        END_CASE     */

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT    /* End of finish_mods */
#endif

    if ((sceneData->EffectiveLanguageVersion() >= 370) && ambientSet)
    {
        // As of v3.7, use of "ambient" to model glowing materials is deprecated, and "emission" should be used
        // instead.

        // We can only guess what the user is trying to achieve with the "ambient" keyword. Our heuristic is based on
        // the following assumptions:
        // - When used properly to model non-radiosity ambient illumination, "ambient" values are typically around 0.1.
        // - When misused to model glowing materials, "ambient" is often set to 1.0.
        // - If the user also specifies "emission", they probably know what they are doing.

        if ((New->Ambient.Greyscale() >= 0.3) && !emissionSet)
        {
            PossibleError("Suspiciously high 'ambient' value found. Are you trying to model a glowing material?"
                          " As of POV-Ray v3.7, 'ambient' is disabled when using radiosity, and its use to model glowing"
                          " materials is generally deprecated; use 'emission' for this purpose instead."
                          " If your intention is to model unusually high ambient illumination in a non-radiosity scene,"
                          " you can avoid this warning by explicitly specifying 'emission 0'.");
        }
    }

    // If requested by the user via the "albedo" keyword,
    // adjust diffuse, phong and/or specular intensity parameters
    // so that a user-specified value of 1.0 corresponds to a
    // backscattering of 100% of the incoming light
    if (diffuseAdjust)
    {
        New->BrillianceAdjust    = (New->Brilliance + 1.0) / 2.0;
        New->BrillianceAdjustRad = 1.0;
    }
    else
    {
        New->BrillianceAdjust    = 1.0;
        New->BrillianceAdjustRad = 2.0 / (New->Brilliance + 1.0);
    }
    if (phongAdjust)
        New->Phong *= (New->Phong_Size + 1.0) / 2.0;
    if (specularAdjust)
        New->Specular *= (New->Roughness + 2.0) / (4.0 * ( 2.0 - pow( 2.0, -New->Roughness / 2.0 ) ) );

    // For Fresnel reflectivity, if the user didn't specify a min & max,
    // then we will set the min to zero - otherwise, this setting would
    // have no effect.
    if ((New->Reflection_Fresnel) && (New->Reflection_Max - New->Reflection_Min).IsZero())
        New->Reflection_Min.Clear();

    Parse_End ();
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TEXTURE *Parser::Parse_Texture ()
{
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    TEXTURE *Texture;
    int Modified_Pnf;

    if (sceneData->EffectiveLanguageVersion() < 300)
    {
        return(Parse_Vers1_Texture());
    }

    Modified_Pnf = false;

    if (AllowToken(TEXTURE_ID_TOKEN))
    {
        Texture = Copy_Textures(CurrentTokenDataPtr<TEXTURE*>());
        Modified_Pnf = true;
    }
    else
        Texture = Copy_Textures(Default_Texture);

    /* If the texture identifier or the default texture was a PLAIN_PATTERN
       then allow its pigment, normal or finish to be overridden by
       pigment identifier, normal identifier and finish identifiers.
       This is a concession to backwards compatibility so that
       "texture{PIGMENT_IDENTIFIER}" etc. is legal even though it should
       be "texture{pigment{PIGMENT_IDENTIFIER}}"
    */

    /* Look for [pnf_texture] */
    if (Texture->Type == PLAIN_PATTERN)
    {
        EXPECT   /* Look for [pnf_ids] */
            CASE (PIGMENT_ID_TOKEN)
                Warn_State(PIGMENT_TOKEN);
                Destroy_Pigment(Texture->Pigment);
                Texture->Pigment = Copy_Pigment (CurrentTokenDataPtr<PIGMENT*>());
                Modified_Pnf = true;
            END_CASE

            CASE (NORMAL_ID_TOKEN)
                Warn_State(NORMAL_TOKEN);
                Destroy_Tnormal(Texture->Tnormal);
                Texture->Tnormal = Copy_Tnormal (CurrentTokenDataPtr<TNORMAL*>());
                Modified_Pnf = true;
            END_CASE

            CASE (FINISH_ID_TOKEN)
                Warn_State(FINISH_TOKEN);
                if (Texture->Finish)
                    delete Texture->Finish;
                Texture->Finish = Copy_Finish (CurrentTokenDataPtr<FINISH*>());
                Modified_Pnf = true;
            END_CASE

            OTHERWISE
                UNGET
                EXIT
            END_CASE
        END_EXPECT

    /* If the texture identifer or the default texture was a PLAIN_PATTERN
       then allow its pigment, normal or finish to be overridden by
       pigment, normal or finish statement.  Also allow transformations.
    */

        EXPECT   /* Modify previous pnf */
            CASE (PIGMENT_TOKEN)
                Parse_Begin ();
                Parse_Pigment ( &(Texture->Pigment) );
                Parse_End ();
                Modified_Pnf = true;
            END_CASE

            CASE (NORMAL_TOKEN)
                Parse_Begin ();
                Parse_Tnormal ( &(Texture->Tnormal) );
                Parse_End ();
                Modified_Pnf = true;
            END_CASE

            CASE (FINISH_TOKEN)
                Parse_Finish ( &(Texture->Finish) );
                Modified_Pnf = true;
            END_CASE

            CASE (TRANSLATE_TOKEN)
                Parse_Vector (Local_Vector);
                Compute_Translation_Transform(&Local_Trans, Local_Vector);
                Transform_Textures (Texture, &Local_Trans);
                Modified_Pnf = true;
            END_CASE

            CASE (ROTATE_TOKEN)
                Parse_Vector (Local_Vector);
                Compute_Rotation_Transform(&Local_Trans, Local_Vector);
                Transform_Textures (Texture, &Local_Trans);
                Modified_Pnf = true;
            END_CASE

            CASE (SCALE_TOKEN)
                Parse_Scale_Vector (Local_Vector);
                Compute_Scaling_Transform(&Local_Trans, Local_Vector);
                Transform_Textures (Texture, &Local_Trans);
                Modified_Pnf = true;
            END_CASE

            CASE (MATRIX_TOKEN)
                Parse_Matrix(Local_Matrix);
                Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
                Transform_Textures (Texture, &Local_Trans);
                Modified_Pnf = true;
            END_CASE

            CASE (TRANSFORM_TOKEN)
                Transform_Textures (Texture, Parse_Transform(&Local_Trans));
                Modified_Pnf = true;
            END_CASE

            CASE (NO_BUMP_SCALE_TOKEN)
                Set_Flag(Texture, DONT_SCALE_BUMPS_FLAG);
            END_CASE

            OTHERWISE
                UNGET
                EXIT
            END_CASE
        END_EXPECT
    }
    else
    {
        /* Here it is not a PLAIN_PATTERN texture and since default textures
           must be plain then this was a texture identifier that was a special
           texture.  Allow transforms.  The "if(!Modified_Pnf)..." below
           will always fail if we came here.  So we return after the
           transforms. */
        Parse_Texture_Transform(Texture);
    }

    /* If we've modified the default texture with a p,n, or f then this
       has to stay a PLAIN_PATTERN pnf texture.  We won't allow
       a texture_map or pattern.  Therefore quit now.
     */

    if (!Modified_Pnf)
    {
        /* At this point we've either got a texture statement that had
           no p, n or f.  Nor any texture identifier.  Its probably
           a patterned texture_map texture. */

        EXPECT_ONE
            CASE (TILES_TOKEN)
                Destroy_Textures (Texture);
                Texture = Parse_Tiles();
                if (Texture->Blend_Map->Blend_Map_Entries[1].Vals == nullptr)
                    Error("First texture missing from tiles");
                Parse_Texture_Transform(Texture);
            END_CASE

            CASE (MATERIAL_MAP_TOKEN)
                Destroy_Textures (Texture);
                Texture = Parse_Material_Map ();
                Parse_Texture_Transform(Texture);
            END_CASE

            OTHERWISE
                UNGET
                Destroy_Pigment(Texture->Pigment);
                Destroy_Tnormal(Texture->Tnormal);
                if (Texture->Finish)
                    delete Texture->Finish;
                Texture->Pigment = nullptr;
                Texture->Tnormal = nullptr;
                Texture->Finish  = nullptr;
                Parse_Pattern<TextureBlendMap>(Texture,kBlendMapType_Texture);
                /* if following is true, parsed "texture{}" so restore
                   default texture.
                 */
                if (Texture->Type <= PLAIN_PATTERN)
                {
                    Destroy_Textures(Texture);
                    Texture = Copy_Textures (Default_Texture);
                }
            END_CASE
        END_EXPECT
    }

    return (Texture);
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TEXTURE *Parser::Parse_Tiles()
{
    TEXTURE *Texture, *Local_Texture;

    Parse_Begin ();

    Texture = Create_Texture ();
    Destroy_Pigment(Texture->Pigment);
    Destroy_Tnormal(Texture->Tnormal);
    if (Texture->Finish)
        delete Texture->Finish;
    Texture->Pigment = nullptr;
    Texture->Tnormal = nullptr;
    Texture->Finish  = nullptr;
    Texture->Type = GENERIC_PATTERN;
    Texture->pattern = PatternPtr(new CheckerPattern());

    Texture->Blend_Map = Create_Blend_Map<TextureBlendMap> (kBlendMapType_Texture);
    Texture->Blend_Map->Blend_Map_Entries.resize(2);
    Texture->Blend_Map->Blend_Map_Entries[0].Vals = nullptr;
    Texture->Blend_Map->Blend_Map_Entries[0].value=0.0;
    Texture->Blend_Map->Blend_Map_Entries[1].Vals = nullptr;
    Texture->Blend_Map->Blend_Map_Entries[1].value=1.0;

    /* Note first tile is 1, 2nd tile is 0 to keep compatible with old tiles */

    EXPECT
        CASE (TEXTURE_TOKEN)
            Parse_Begin ();
            Local_Texture = Parse_Texture ();
            Link_Textures(&(Texture->Blend_Map->Blend_Map_Entries[1].Vals),Local_Texture);
            Parse_End ();
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    GET (TILE2_TOKEN);

    EXPECT
        CASE (TEXTURE_TOKEN)
            Parse_Begin ();
            Local_Texture = Parse_Texture ();
            Link_Textures(&(Texture->Blend_Map->Blend_Map_Entries[0].Vals),Local_Texture);
            Parse_End ();
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End ();

    return (Texture);
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TEXTURE *Parser::Parse_Material_Map()
{
    TEXTURE *Texture;
    Vector2d Repeat;

    Parse_Begin ();

    Texture = Create_Texture ();
    Destroy_Pigment(Texture->Pigment);
    Destroy_Tnormal(Texture->Tnormal);
    if (Texture->Finish)
        delete Texture->Finish;
    Texture->Pigment = nullptr;
    Texture->Tnormal = nullptr;
    Texture->Finish  = nullptr;
    Texture->Type = BITMAP_PATTERN;

    shared_ptr<ImagePattern> pattern(new ImagePattern()); // TODO REVIEW - other use cases set waveFrequency to 0.0
    ImageDataPtr& pImage = pattern->pImage;

    pImage = Parse_Image(MATERIAL_FILE);
    pImage->Use = USE_NONE; // was false [trf]

    EXPECT
        CASE (ONCE_TOKEN)
            pImage->Once_Flag=true;
        END_CASE

        CASE (INTERPOLATE_TOKEN)
            pImage->Interpolation_Type=(int)Parse_Float();
        END_CASE

        CASE (MAP_TYPE_TOKEN)
            pImage->Map_Type = (int) Parse_Float ();
        END_CASE

        CASE (REPEAT_TOKEN)
            Parse_UV_Vect (Repeat);
            if ((Repeat[0]<=0.0) || (Repeat[1]<=0.0))
                Error("Zero or Negative Image Repeat Vector.");
            pImage->width  =  (DBL)pImage->iwidth  * Repeat[0];
            pImage->height =  (DBL)pImage->iheight * Repeat[1];
        END_CASE

        CASE (OFFSET_TOKEN)
            Parse_UV_Vect (pImage->Offset);
            pImage->Offset[U] *= -(DBL)pImage->iwidth;
            pImage->Offset[V] *= -(DBL)pImage->iheight;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    GET (TEXTURE_TOKEN)                /* First material */
    Parse_Begin();
    Texture->Materials.push_back(Parse_Texture ());
    Parse_End();

    EXPECT                             /* Subsequent materials */
        CASE (TEXTURE_TOKEN)
            Parse_Begin();
            Texture->Materials.push_back(Parse_Texture ());
            Parse_End();
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End ();

    Texture->pattern = pattern;

    return(Texture);
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TEXTURE *Parser::Parse_Vers1_Texture ()
{
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    ClassicTurbulence *Local_Turb;
    TEXTURE *Texture;
    PIGMENT *Pigment;
    TNORMAL *Tnormal;
    FINISH *Finish;
    ContinuousPattern* pContinuousPattern;

    EXPECT_ONE                      /* Look for texture_body */
        CASE (TILES_TOKEN)
            Texture = Parse_Tiles();
            if (Texture->Blend_Map->Blend_Map_Entries[1].Vals == nullptr)
                Error("First texture missing from tiles");
        END_CASE

        CASE (MATERIAL_MAP_TOKEN)
            Texture = Parse_Material_Map ();
        END_CASE

        CASE (TEXTURE_ID_TOKEN)
            Texture = Copy_Textures(CurrentTokenDataPtr<TEXTURE*>());
        END_CASE

        OTHERWISE
            UNGET
            Texture = Copy_Textures (Default_Texture);
        END_CASE
    END_EXPECT

    /* Look for [pnf_texture] */
    if (Texture->Type == PLAIN_PATTERN)
        {
            EXPECT   /* Look for [pnf_ids] */
                CASE (PIGMENT_ID_TOKEN)
                    Destroy_Pigment(Texture->Pigment);
                    Texture->Pigment = Copy_Pigment (CurrentTokenDataPtr<PIGMENT*>());
                END_CASE

                CASE (NORMAL_ID_TOKEN)
                    Destroy_Tnormal(Texture->Tnormal);
                    Texture->Tnormal = Copy_Tnormal (CurrentTokenDataPtr<TNORMAL*>());
                END_CASE

                CASE (FINISH_ID_TOKEN)
                    if (Texture->Finish)
                        delete Texture->Finish;
                    Texture->Finish = Copy_Finish (CurrentTokenDataPtr<FINISH*>());
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT

            Pigment = Texture->Pigment;
            Tnormal = Texture->Tnormal;
            Finish  = Texture->Finish;

            EXPECT_CAT
                CASE (PIGMENT_TOKEN)
                    Parse_Begin ();
                    Parse_Pigment ( &(Texture->Pigment) );
                    Parse_End ();
                END_CASE

                CASE (NORMAL_TOKEN)
                    Parse_Begin ();
                    Parse_Tnormal ( &(Texture->Tnormal) );
                    Parse_End ();
                END_CASE

                CASE (FINISH_TOKEN)
                    Parse_Finish ( &(Texture->Finish) );
                END_CASE

/***********************************************************************
PIGMENT STUFF OUTSIDE PIGMENT{}
NOTE: Do not add new keywords to this section.  Use 1.0 syntax only.
***********************************************************************/
                CASE (AGATE_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new AgatePattern());
                    Check_Turb(Pigment->pattern->warps, Pigment->pattern->HasSpecialTurbulenceHandling()); // agate needs Octaves, Lambda etc., and handles the pattern itself
                END_CASE

                CASE (BOZO_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new BozoPattern());
                END_CASE

                CASE (GRANITE_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new GranitePattern());
                END_CASE

                CASE (LEOPARD_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new LeopardPattern());
                END_CASE

                CASE (MARBLE_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new MarblePattern());
                    // TODO REVIEW - proper syntax sets waveType
                END_CASE

                CASE (MANDEL_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = ParseMandelPattern();
                END_CASE

                CASE (ONION_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new OnionPattern());
                END_CASE

                CASE (SPOTTED_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new SpottedPattern());
                END_CASE

                CASE (WOOD_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new WoodPattern());
                END_CASE

                CASE (GRADIENT_TOKEN)
                    {
                        Warn_State(PIGMENT_TOKEN);
                        Pigment->Type = GENERIC_PATTERN;
                        shared_ptr<GradientPattern> pattern(new GradientPattern());
                        Parse_Vector (Local_Vector);
                        Local_Vector.normalize();
                        pattern->gradient = Local_Vector;
                        Pigment->pattern = pattern;
                    }
                END_CASE

                CASE_COLOUR_UNGET
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = PLAIN_PATTERN;
                    Pigment->pattern = PatternPtr(new PlainPattern());
                    Parse_Colour (Pigment->colour);
                END_CASE

                CASE (CHECKER_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new CheckerPattern());
                    Pigment->Blend_Map = Parse_Blend_List<ColourBlendMap>(2,Pigment->pattern->GetDefaultBlendMap(),kBlendMapType_Colour);
                END_CASE

                CASE (HEXAGON_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new HexagonPattern());
                    Pigment->Blend_Map = Parse_Blend_List<ColourBlendMap>(3,Pigment->pattern->GetDefaultBlendMap(),kBlendMapType_Colour);
                END_CASE

                CASE (SQUARE_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new SquarePattern());
                    Pigment->Blend_Map = Parse_Blend_List<ColourBlendMap>(4,Pigment->pattern->GetDefaultBlendMap(),kBlendMapType_Colour);
                END_CASE

                CASE (TRIANGULAR_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = GENERIC_PATTERN;
                    Pigment->pattern = PatternPtr(new TriangularPattern());
                    Pigment->Blend_Map = Parse_Blend_List<ColourBlendMap>(6,Pigment->pattern->GetDefaultBlendMap(),kBlendMapType_Colour);
                END_CASE

                CASE (IMAGE_MAP_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Pigment->Type = IMAGE_MAP_PATTERN;
                    Pigment->pattern = PatternPtr(new ColourImagePattern());
                    Parse_Image_Map (Pigment);
                END_CASE

                CASE (TURBULENCE_TOKEN)
                    Local_Turb=Check_Turb(Pigment->pattern->warps, Pigment->pattern->HasSpecialTurbulenceHandling());
                    Parse_Vector(Local_Turb->Turbulence);
                END_CASE

                CASE (COLOUR_MAP_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    if (!Pigment->pattern->CanMap())
                        VersionWarning(150, "Cannot use color map with this pigment type.");
                    Pigment->Blend_Map = Parse_Colour_Map<ColourBlendMap> ();
                END_CASE

                CASE (QUICK_COLOUR_TOKEN)
                    Warn_State(PIGMENT_TOKEN);
                    Parse_Colour (Pigment->Quick_Colour);
                END_CASE

                CASE (OCTAVES_TOKEN)
                    Local_Turb=Check_Turb(Pigment->pattern->warps, Pigment->pattern->HasSpecialTurbulenceHandling());
                    Local_Turb->Octaves = (int)Parse_Float();
                    if(Local_Turb->Octaves < 1)
                        Local_Turb->Octaves = 1;
                    if(Local_Turb->Octaves > 10)  /* Avoid DOMAIN errors */
                        Local_Turb->Octaves = 10;
                END_CASE

                CASE (OMEGA_TOKEN)
                    Local_Turb=Check_Turb(Pigment->pattern->warps, Pigment->pattern->HasSpecialTurbulenceHandling());
                    Local_Turb->Omega = Parse_Float();
                END_CASE

                CASE (LAMBDA_TOKEN)
                    Local_Turb=Check_Turb(Pigment->pattern->warps, Pigment->pattern->HasSpecialTurbulenceHandling());
                    Local_Turb->Lambda = Parse_Float();
                END_CASE

/***********************************************************************
TNORMAL STUFF OUTSIDE NORMAL{}
NOTE: Do not add new keywords to this section.  Use 1.0 syntax only.
***********************************************************************/
                CASE (BUMPS_TOKEN)
                    Warn_State(NORMAL_TOKEN);
                    ADD_TNORMAL
                    Tnormal->Type = BUMPS_PATTERN;
                    Tnormal->pattern = PatternPtr(new BumpsPattern());
                    Tnormal->Amount = Parse_Float ();
                END_CASE

                CASE (DENTS_TOKEN)
                    Warn_State(NORMAL_TOKEN);
                    ADD_TNORMAL
                    Tnormal->Type = DENTS_PATTERN;
                    Tnormal->pattern = PatternPtr(new DentsPattern());
                    Tnormal->Amount = Parse_Float ();
                END_CASE

                CASE (RIPPLES_TOKEN)
                    Warn_State(NORMAL_TOKEN);
                    ADD_TNORMAL
                    Tnormal->Type = RIPPLES_PATTERN;
                    Tnormal->pattern = PatternPtr(new RipplesPattern());
                    Tnormal->Amount = Parse_Float ();
                END_CASE

                CASE (WAVES_TOKEN)
                    Warn_State(NORMAL_TOKEN);
                    ADD_TNORMAL
                    Tnormal->Type = WAVES_PATTERN;
                    Tnormal->pattern = PatternPtr(new WavesPattern());
                    Tnormal->Amount = Parse_Float ();
                END_CASE

                CASE (WRINKLES_TOKEN)
                    Warn_State(NORMAL_TOKEN);
                    ADD_TNORMAL
                    Tnormal->Type = WRINKLES_PATTERN;
                    Tnormal->pattern = PatternPtr(new WrinklesPattern());
                    Tnormal->Amount = Parse_Float ();
                END_CASE

                CASE (BUMP_MAP_TOKEN)
                    {
                        Warn_State(NORMAL_TOKEN);
                        ADD_TNORMAL
                        Tnormal->Type = BITMAP_PATTERN;
                        shared_ptr<ImagePattern> pattern(new ImagePattern());
                        pattern->waveFrequency = 0.0;
                        Parse_Bump_Map (Tnormal);
                        Tnormal->pattern = pattern;
                    }
                END_CASE

                CASE (FREQUENCY_TOKEN)
                    Warn_State(NORMAL_TOKEN);
                    ADD_TNORMAL
                    pContinuousPattern = dynamic_cast<ContinuousPattern*>(Tnormal->pattern.get());
                    if (pContinuousPattern != nullptr)
                        pContinuousPattern->waveFrequency = Parse_Float();
                    else
                    {
                        if (sceneData->EffectiveLanguageVersion() >= 150)
                            VersionWarning(150, "Cannot use frequency with this normal.");
                        Parse_Float();
                    }
                END_CASE

                CASE (PHASE_TOKEN)
                    Warn_State(NORMAL_TOKEN);
                    ADD_TNORMAL
                    pContinuousPattern = dynamic_cast<ContinuousPattern*>(Tnormal->pattern.get());
                    if (pContinuousPattern != nullptr)
                        pContinuousPattern->wavePhase = Parse_Float();
                    else
                    {
                        if (sceneData->EffectiveLanguageVersion() >= 150)
                            VersionWarning(150, "Cannot use phase with this normal.");
                        Parse_Float();
                    }
                END_CASE


/***********************************************************************
FINISH STUFF OUTSIDE FINISH{}
NOTE: Do not add new keywords to this section.  Use 1.0 syntax only.
***********************************************************************/
                CASE (AMBIENT_TOKEN)
                    Warn_State(FINISH_TOKEN);
                    Finish->Ambient = MathColour(Parse_Float ());
                END_CASE

                CASE (BRILLIANCE_TOKEN)
                    Warn_State(FINISH_TOKEN);
                    Finish->Brilliance = Parse_Float ();
                END_CASE

                CASE (DIFFUSE_TOKEN)
                    Warn_State(FINISH_TOKEN);
                    Finish->Diffuse = Parse_Float ();
                END_CASE

                CASE (REFLECTION_TOKEN)
                    Warn_State(FINISH_TOKEN);
                    Finish->Reflection_Max = MathColour(Parse_Float ());
                    Finish->Reflection_Min = Finish->Reflection_Max;
                    Finish->Reflection_Falloff = 1;
                END_CASE

                CASE (PHONG_TOKEN)
                    Warn_State(FINISH_TOKEN);
                    Finish->Phong = Parse_Float ();
                END_CASE

                CASE (PHONG_SIZE_TOKEN)
                    Warn_State(FINISH_TOKEN);
                    Finish->Phong_Size = Parse_Float ();
                END_CASE

                CASE (SPECULAR_TOKEN)
                    Warn_State(FINISH_TOKEN);
                    Finish->Specular = Parse_Float ();
                END_CASE

                CASE (ROUGHNESS_TOKEN)
                    Warn_State(FINISH_TOKEN);
                    Finish->Roughness = Parse_Float ();
                    if (Finish->Roughness != 0.0)
                        Finish->Roughness = 1.0/Finish->Roughness; /* CEY 12/92 */
                    else
                        Warning("Zero roughness used.");
                END_CASE

                CASE (METALLIC_TOKEN)
                    Warn_State(FINISH_TOKEN);
                    Finish->Metallic = 1.0;
                END_CASE

                CASE (CRAND_TOKEN)
                    Warn_State(FINISH_TOKEN);
                    Finish->Crand = Parse_Float();
                END_CASE

                CASE_FLOAT_UNGET
                    Finish->Crand = Parse_Float();
                    VersionWarning(150, "Should use crand keyword in finish statement.");
                END_CASE

                CASE (IOR_TOKEN)
                    Warn_State(INTERIOR_TOKEN);
                    Finish->Temp_IOR = Parse_Float();
                    Warn_Compat(false, "Index of refraction value should be specified in 'interior{...}' statement.");
                END_CASE

                CASE (REFRACTION_TOKEN)
                    Warn_State(INTERIOR_TOKEN);
                    Finish->Temp_Refract = Parse_Float();
                    Warn_Compat(false, "Refraction value unnecessary to turn on refraction.\nTo attenuate, the fade_power and fade_distance keywords should be specified in 'interior{...}' statement.");
                END_CASE

                CASE (TRANSLATE_TOKEN)
                    Parse_Vector (Local_Vector);
                    Compute_Translation_Transform(&Local_Trans, Local_Vector);
                    Transform_Textures (Texture, &Local_Trans);
                END_CASE

                CASE (ROTATE_TOKEN)
                    Parse_Vector (Local_Vector);
                    Compute_Rotation_Transform(&Local_Trans, Local_Vector);
                    Transform_Textures (Texture, &Local_Trans);
                END_CASE

                CASE (SCALE_TOKEN)
                    Parse_Scale_Vector (Local_Vector);
                    Compute_Scaling_Transform(&Local_Trans, Local_Vector);
                    Transform_Textures (Texture, &Local_Trans);
                END_CASE

                CASE (MATRIX_TOKEN)
                    Parse_Matrix(Local_Matrix);
                    Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
                    Transform_Textures (Texture, &Local_Trans);
                END_CASE

                CASE (TRANSFORM_TOKEN)
                    Transform_Textures (Texture, Parse_Transform(&Local_Trans));
                END_CASE

                CASE (TEXTURE_ID_TOKEN)
                    Warning("Texture identifier overwriting previous values.");
                    Destroy_Textures(Texture);
                    Texture = Copy_Textures(CurrentTokenDataPtr<TEXTURE*>());
                    Pigment = Texture->Pigment;
                    Tnormal = Texture->Tnormal;
                    Finish  = Texture->Finish;
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE

/***********************************************************************/

            END_EXPECT

            if (Not_In_Default && (Texture->Pigment->Type == NO_PATTERN) &&
                (sceneData->EffectiveLanguageVersion() >= 150))
                Parse_Error(PIGMENT_ID_TOKEN);

        }

    Parse_Texture_Transform(Texture);

    return (Texture);
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Texture_Transform (TEXTURE *Texture)
{
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;

    EXPECT
        CASE (TRANSLATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Translation_Transform(&Local_Trans, Local_Vector);
            Transform_Textures (Texture, &Local_Trans);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Rotation_Transform(&Local_Trans, Local_Vector);
            Transform_Textures (Texture, &Local_Trans);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector (Local_Vector);
            Compute_Scaling_Transform(&Local_Trans, Local_Vector);
            Transform_Textures (Texture, &Local_Trans);
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix(Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Transform_Textures (Texture, &Local_Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Transform_Textures (Texture, Parse_Transform(&Local_Trans));
        END_CASE

        CASE (NO_BUMP_SCALE_TOKEN)
            Set_Flag(Texture, DONT_SCALE_BUMPS_FLAG);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT
}


/*****************************************************************************
*
* FUNCTION
*
*   Parse_Media
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

void Parser::Parse_Media(vector<Media>& medialist)
{
    Media IMediaObj;
    Media *IMedia = &IMediaObj;
    TRANSFORM Local_Trans;
    Vector3d Local_Vector;
    MATRIX Local_Matrix;

    Parse_Begin();

    if (AllowToken(MEDIA_ID_TOKEN))
        IMediaObj = CurrentTokenData<Media>();
    else
    {
        /* as of v3.5, the default media method is now 3 */
        if (sceneData->EffectiveLanguageVersion() >= 350)
        {
            IMedia->Intervals = 1;
            IMedia->Min_Samples = 10;
            IMedia->Max_Samples = 10;
            IMedia->Sample_Method = 3;
        }
    }

    EXPECT
        CASE (INTERVALS_TOKEN)
            if ((IMedia->Intervals = (int)Parse_Float()) < 1)
            {
                Error("At least one interval is needed in media.");
            }
        END_CASE

        CASE (SAMPLES_TOKEN)
            IMedia->Min_Samples = (int)Parse_Float();
            Parse_Comma();
            IMedia->Max_Samples = (int)Allow_Float(IMedia->Min_Samples);
            if (IMedia->Min_Samples < 1)
            {
                Error("At least one sample per interval is needed in media.");
            }
            if (IMedia->Max_Samples < IMedia->Min_Samples)
            {
                Error("Maximum number of samples per interval smaller than minimum number.");
            }
        END_CASE

        CASE (METHOD_TOKEN)
            IMedia->Sample_Method = (int)Parse_Float();
            if (IMedia->Sample_Method != 1 && IMedia->Sample_Method!= 2 && IMedia->Sample_Method!= 3)
            {
                Error("Sample method choices are 1, 2, or 3.");
            }
        END_CASE

        CASE (JITTER_TOKEN)
            IMedia->Jitter = Parse_Float();
        END_CASE

        CASE (AA_THRESHOLD_TOKEN)
            IMedia->AA_Threshold = Parse_Float();
            if(IMedia->AA_Threshold<=0)
            {
                Error("aa_threshold in media must be greater than zero.");
            }
        END_CASE

        CASE (AA_LEVEL_TOKEN)
            IMedia->AA_Level = (int)Parse_Float();
            if(IMedia->AA_Level<1)
                Error("aa_level in media must be at least one.");
        END_CASE

        CASE (COLLECT_TOKEN)
            IMedia->ignore_photons = !(Allow_Float(1.0) > 0.0);
        END_CASE

        CASE (ABSORPTION_TOKEN)
            Parse_Colour(IMedia->Absorption);
        END_CASE

        CASE (EMISSION_TOKEN)
            Parse_Colour(IMedia->Emission);
        END_CASE

        CASE (SCATTERING_TOKEN)
            Parse_Begin();
            IMedia->Type = (int)Parse_Float();
            if ((IMedia->Type < 1) || (IMedia->Type > SCATTERING_TYPES))
            {
                Warning("Unknown atmospheric scattering type.");
            }
            Parse_Comma();
            Parse_Colour(IMedia->Scattering);

            EXPECT
                CASE (ECCENTRICITY_TOKEN)
                    if (IMedia->Type != HENYEY_GREENSTEIN_SCATTERING)
                    {
                        Error("Eccentricity cannot be used with this scattering type.");
                    }
                    IMedia->Eccentricity = Parse_Float();
                END_CASE

                CASE (EXTINCTION_TOKEN)
                    IMedia->sc_ext = Parse_Float();
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT

            Parse_End();
        END_CASE

        CASE (CONFIDENCE_TOKEN)
            IMedia->Confidence = Parse_Float();
            if ((IMedia->Confidence < 0.0) || (IMedia->Confidence >= 1.0))
            {
                Error("Illegal confidence value in media.");
            }
        END_CASE

        CASE (VARIANCE_TOKEN)
            IMedia->Variance = Parse_Float();
        END_CASE

        CASE (RATIO_TOKEN)
            IMedia->Ratio = Parse_Float();
        END_CASE

        CASE (DENSITY_TOKEN)
            Parse_Begin();
            Parse_Media_Density_Pattern(IMedia->Density);
            Parse_End();
        END_CASE

        CASE (TRANSLATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Translation_Transform(&Local_Trans, Local_Vector);
            Transform_Density (IMedia->Density, &Local_Trans);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Rotation_Transform(&Local_Trans, Local_Vector);
            Transform_Density (IMedia->Density, &Local_Trans);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector (Local_Vector);
            Compute_Scaling_Transform(&Local_Trans, Local_Vector);
            Transform_Density (IMedia->Density, &Local_Trans);
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix(Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Transform_Density (IMedia->Density, &Local_Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Transform_Density (IMedia->Density, Parse_Transform(&Local_Trans));
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End();

    medialist.push_back(IMediaObj);
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Interior
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1997 : Creation.
*   Sep 1999 : Fade_Colour added - Edward Coffey
*
******************************************************************************/

void Parser::Parse_Interior(InteriorPtr& interior)
{
    Parse_Begin();

    if (AllowToken(INTERIOR_ID_TOKEN))
    {
        if (HaveCurrentTokenData())
            interior = InteriorPtr(new Interior(*CurrentTokenData<InteriorPtr>()));
        else
            interior = InteriorPtr(new Interior());
    }

    if(!interior)
        interior = InteriorPtr(new Interior());

    EXPECT
        CASE (IOR_TOKEN)
            interior->IOR = Parse_Float();
        END_CASE

        CASE (DISPERSION_TOKEN)
            interior->Dispersion = Parse_Float();
        END_CASE

        CASE (DISPERSION_SAMPLES_TOKEN)
            interior->Disp_NElems = Parse_Int_With_Minimum(DISPERSION_ELEMENTS_MIN, "dispersion_samples");
        END_CASE

        CASE (CAUSTICS_TOKEN)
            interior->Caustics = Parse_Float() * 45.0;
        END_CASE

        CASE (FADE_COLOUR_TOKEN)
            Parse_Colour(interior->Fade_Colour);
        END_CASE

        CASE (FADE_DISTANCE_TOKEN)
            interior->Fade_Distance = Parse_Float();
        END_CASE

        CASE (FADE_POWER_TOKEN)
            interior->Fade_Power = Parse_Float();
        END_CASE

        CASE (MEDIA_TOKEN)
            Parse_Media(interior->media);
        END_CASE

        CASE (REFRACTION_TOKEN)
            interior->Old_Refract = Parse_Float();
            Warn_Compat(false, "Refraction value unnecessary to turn on refraction.\nTo attenuate, the fade_power and fade_distance keywords should be specified in 'interior{...}' statement.");
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End();
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Media_Density_Pattern
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dez 1996 : Creation.
*
******************************************************************************/

void Parser::Parse_Media_Density_Pattern(PIGMENT** Density)
{
    if (AllowToken(DENSITY_ID_TOKEN))
        *Density = Copy_Pigment(CurrentTokenDataPtr<PIGMENT*>());
    else
        *Density = Create_Pigment();

    Parse_Pattern<GenericPigmentBlendMap>(*Density,kBlendMapType_Density);
}

void Parser::Parse_Media_Density_Pattern(vector<PIGMENT*>& Density)
{
    PIGMENT *New;

    Parse_Media_Density_Pattern(&New);

    Density.push_back(New);
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

FOG *Parser::Parse_Fog()
{
    Vector3d Vector;
    MATRIX Matrix;
    TRANSFORM Trans;
    FOG *Fog;

    Parse_Begin();

    if (AllowToken(FOG_ID_TOKEN))
        Fog = Copy_Fog(CurrentTokenDataPtr<FOG*>());
    else
        Fog = Create_Fog();

    EXPECT_CAT
        CASE_COLOUR_UNGET
            Parse_Colour(Fog->colour);
        END_CASE

        CASE (DISTANCE_TOKEN)
            Fog->Distance = Parse_Float();
        END_CASE

        CASE_FLOAT_UNGET
            VersionWarning(150, "Should use distance keyword.");
            Fog->Distance = Parse_Float();
        END_CASE

        CASE (FOG_TYPE_TOKEN)
            Fog->Type = (int)Parse_Float();
            if ((Fog->Type < ORIG_FOG) || (Fog->Type > FOG_TYPES))
            {
                Warning("Unknown fog type.");
            }
        END_CASE

        CASE (FOG_ALT_TOKEN)
            Fog->Alt = Parse_Float();
        END_CASE

        CASE (FOG_OFFSET_TOKEN)
            Fog->Offset = Parse_Float();
        END_CASE

        CASE (TURB_DEPTH_TOKEN)
            Fog->Turb_Depth = Parse_Float();
        END_CASE

        CASE (UP_TOKEN)
            Parse_Vector(Fog->Up);
        END_CASE

        CASE (TURBULENCE_TOKEN)
            if (Fog->Turb == nullptr)
                Fog->Turb = new TurbulenceWarp();
            Parse_Vector(Fog->Turb->Turbulence);
        END_CASE

        CASE (OCTAVES_TOKEN)
            if (Fog->Turb == nullptr)
                Fog->Turb = new TurbulenceWarp();
            Fog->Turb->Octaves = (int)Parse_Float();
            if(Fog->Turb->Octaves < 1)
                Fog->Turb->Octaves = 1;
            if(Fog->Turb->Octaves > 10)
                Fog->Turb->Octaves = 10;
        END_CASE

        CASE (OMEGA_TOKEN)
            if (Fog->Turb == nullptr)
                Fog->Turb = new TurbulenceWarp();
            Fog->Turb->Omega = Parse_Float();
        END_CASE

        CASE (LAMBDA_TOKEN)
            if (Fog->Turb == nullptr)
                Fog->Turb = new TurbulenceWarp();
            Fog->Turb->Lambda = Parse_Float();
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector(Vector);
            Compute_Rotation_Transform(&Trans, Vector);
            MTransDirection(Fog->Up, Fog->Up, &Trans);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Vector(Vector);
            Compute_Scaling_Transform(&Trans, Vector);
            MTransDirection(Fog->Up, Fog->Up, &Trans);
        END_CASE

        CASE (TRANSLATE_TOKEN)
            Parse_Vector(Vector);
            Warning("A fog's up vector can't be translated.");
/*
            Compute_Translation_Transform(&Trans, Vector);
            MTransDirection(Fog->Up, Fog->Up, &Trans);
*/
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix(Matrix);
            Compute_Matrix_Transform(&Trans, Matrix);
            MTransDirection(Fog->Up, Fog->Up, &Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            MTransDirection(Fog->Up, Fog->Up, Parse_Transform(&Trans));
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End ();

    /* Make sure the up vector is normalized. */

    Fog->Up.normalize();

    return(Fog);
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Rainbow
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jul 1994 : Creation.
*
*   Dec 1994 : Modified to work with multiple rainbows. [DB]
*
*   Apr 1995 : Added code for rainbow arcs. [DB]
*
******************************************************************************/

RAINBOW *Parser::Parse_Rainbow()
{
    int Angle1, Angle2;
    DBL d;
    RAINBOW *Rainbow;

    Angle1 = Angle2 = false;

    Parse_Begin();

    if (AllowToken(RAINBOW_ID_TOKEN))
        Rainbow = Copy_Rainbow(CurrentTokenDataPtr<RAINBOW*>());
    else
        Rainbow = Create_Rainbow();

    EXPECT
        CASE (ANGLE_TOKEN)
            Rainbow->Angle = Parse_Float();
        END_CASE

        CASE (DIRECTION_TOKEN)
            Parse_Vector(Rainbow->Antisolar_Vector);
        END_CASE

        CASE (COLOUR_MAP_TOKEN)
            {
                // TODO FIXME - what if the user specifies multiple color maps?
                Rainbow->Pigment = Create_Pigment();
                Rainbow->Pigment->Blend_Map = Parse_Colour_Map<ColourBlendMap>();
                Rainbow->Pigment->Type = GENERIC_PATTERN;
                shared_ptr<GradientPattern> pattern(new GradientPattern());
                pattern->gradient = Vector3d(1.0,0.0,0.0);
                Rainbow->Pigment->pattern = pattern;
            }
        END_CASE

        CASE (DISTANCE_TOKEN)
            Rainbow->Distance = Parse_Float();
        END_CASE

        CASE (JITTER_TOKEN)
            Rainbow->Jitter = Parse_Float();
        END_CASE

        CASE (WIDTH_TOKEN)
            Rainbow->Width = Parse_Float();
        END_CASE

        CASE (UP_TOKEN)
            Parse_Vector(Rainbow->Up_Vector);
        END_CASE

        CASE (FALLOFF_ANGLE_TOKEN)
            Angle1 = true;
            Rainbow->Falloff_Angle = Parse_Float();
            if ((Rainbow->Falloff_Angle < 0.0) || (Rainbow->Falloff_Angle > 360.0))
            {
                Error("Illegal falloff angle in rainbow (Use value from 0 to 360 degrees).");
            }
            Rainbow->Falloff_Angle *= M_PI_360;
        END_CASE

        CASE (ARC_ANGLE_TOKEN)
            Angle2 = true;
            Rainbow->Arc_Angle = Parse_Float();
            if ((Rainbow->Arc_Angle < 0.0) || (Rainbow->Arc_Angle > 360.0))
            {
                Error("Illegal arc angle in rainbow (Use value from 0 to 360 degrees).");
            }
            Rainbow->Arc_Angle *= M_PI_360;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End();

    /* Setup falloff angle. */

    if (Angle2 && !Angle1)
    {
        Rainbow->Falloff_Angle = Rainbow->Arc_Angle;
    }

    /* Test if arc angle is greater or equal to falloff angle. */

    if (Rainbow->Arc_Angle < Rainbow->Falloff_Angle)
    {
        Error("Arc angle is smaller than falloff angle in rainbow.");
    }

    /* Get falloff region width.*/

    Rainbow->Falloff_Width = Rainbow->Arc_Angle - Rainbow->Falloff_Angle;

    /* Check for illegal vectors. */

    d = Rainbow->Antisolar_Vector.lengthSqr();

    if (fabs(d) < EPSILON)
    {
        Error("Rainbow's direction vector is zero.");
    }

    d = Rainbow->Up_Vector.lengthSqr();

    if (fabs(d) < EPSILON)
    {
        Error("Rainbow's up vector is zero.");
    }

    Rainbow->Antisolar_Vector.normalize();
    Rainbow->Up_Vector.normalize();

    d = dot(Rainbow->Up_Vector, Rainbow->Antisolar_Vector);

    if (fabs(1.0 - fabs(d)) < EPSILON)
    {
        Error("Rainbow's up and direction vector are co-linear.");
    }

    /* Make sure that up and antisolar vector are perpendicular. */

    Rainbow->Right_Vector = cross(Rainbow->Up_Vector, Rainbow->Antisolar_Vector);

    Rainbow->Up_Vector = cross(Rainbow->Antisolar_Vector, Rainbow->Right_Vector);

    Rainbow->Up_Vector.normalize();
    Rainbow->Right_Vector.normalize();

    /* Adjust rainbow angle and width. */

    Rainbow->Angle -= 0.5 * Rainbow->Width;

    Rainbow->Angle *= M_PI_180;
    Rainbow->Width *= M_PI_180;

    return(Rainbow);
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Skysphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jul 1994 : Creation.
*
*   Dec 1994 : Modified to work with multiple skyspheres. [DB]
*
******************************************************************************/

SKYSPHERE *Parser::Parse_Skysphere()
{
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    SKYSPHERE *Skysphere;

    Parse_Begin();

    if (AllowToken(SKYSPHERE_ID_TOKEN))
        Skysphere = Copy_Skysphere(CurrentTokenDataPtr<SKYSPHERE*>());
    else
        Skysphere = Create_Skysphere();

    EXPECT
        CASE (PIGMENT_TOKEN)
            Skysphere->Pigments.push_back(Create_Pigment());
            Parse_Begin();
            Parse_Pigment(&(Skysphere->Pigments.back()));
            Parse_End();
        END_CASE

        CASE (EMISSION_TOKEN)
            Parse_Colour(Skysphere->Emission);
        END_CASE

        CASE (TRANSLATE_TOKEN)
            Parse_Vector (Local_Vector);
            Translate_Skysphere(Skysphere, Local_Vector);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (Local_Vector);
            Rotate_Skysphere(Skysphere, Local_Vector);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector (Local_Vector);
            Scale_Skysphere(Skysphere, Local_Vector);
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix(Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Transform_Skysphere(Skysphere, &Local_Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Transform_Skysphere(Skysphere, Parse_Transform(&Local_Trans));
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End();

    if (Skysphere->Pigments.empty())
    {
        Error("Empty sky_sphere statement.");
    }

    return(Skysphere);
}

/*****************************************************************************
*
* FUNCTION      : Check_BH_Parameters
*
* ARGUMENTS     : bh - pointer to Black_Hole
*
* AUTHOR        : CJC [7/95]
*
* DESCRIPTION   : Applies sanity checks to the parameters of a black hole.
*
* CHANGES
*
******************************************************************************/

void Parser::Check_BH_Parameters (BlackHoleWarp *bh)
{
    if (bh->Repeat == false) return;

    if (bh->Repeat_Vector [X] > 0.0)
    {
        if (bh->Center [X] < bh->Radius)
            bh->Center [X] = bh->Radius;
        if (bh->Repeat_Vector [X] < bh->Center [X] + bh->Radius + bh->Uncertainty_Vector [X])
        {
            bh->Repeat_Vector [X] = bh->Center [X] + bh->Radius + bh->Uncertainty_Vector [X];
            Warning("Black Hole repeat vector X too small; increased to %g", bh->Repeat_Vector [X]);
        }
        if (bh->Repeat_Vector [X] < EPSILON)
        {
            Warning("Black Hole repeat vector X is less than %f; ignored", (float) EPSILON);
            bh->Repeat_Vector [X] = 0.0;
        }
    }

    if (bh->Repeat_Vector [Y] > 0.0)
    {
        if (bh->Center [Y] < bh->Radius)
            bh->Center [Y] = bh->Radius;
        if (bh->Repeat_Vector [Y] < bh->Center [Y] + bh->Radius + bh->Uncertainty_Vector [Y])
        {
            bh->Repeat_Vector [Y] = bh->Center [Y] + bh->Radius + bh->Uncertainty_Vector [Y];
            Warning("Black Hole repeat vector Y too small; increased to %g", bh->Repeat_Vector [Y]);
        }
        if (bh->Repeat_Vector [Y] < EPSILON)
        {
            Warning("Black Hole repeat vector Y is less than %f; ignored", (float) EPSILON);
            bh->Repeat_Vector [Y] = 0.0;
        }
    }

    if (bh->Repeat_Vector [Z] > 0.0)
    {
        if (bh->Center [Z] < bh->Radius)
            bh->Center [Z] = bh->Radius;
        if (bh->Repeat_Vector [Z] < bh->Center [Z] + bh->Radius + bh->Uncertainty_Vector [Z])
        {
            bh->Repeat_Vector [Z] = bh->Center [Z] + bh->Radius + bh->Uncertainty_Vector [Z];
            Warning("Black Hole repeat vector Z too small; increased to %g", bh->Repeat_Vector [Z]);
        }
        if (bh->Repeat_Vector [Z] < EPSILON)
        {
            Warning("Black Hole repeat vector Z is less than %f; ignored", (float) EPSILON);
            bh->Repeat_Vector [Z] = 0.0;
        }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Check_Turb
*
* INPUT
*
*   Warps_Ptr : Address where the root warp of a warp list
*   is stored.
*
* OUTPUT
*
*   Warps_Ptr : If *Warps_Ptr is `nullptr`, a classic turb warp
*   is created and a pointer to it is stored
*
* RETURNS
*
*   A pointer to the first warp in the list, which is guarenteed to be a
*   classic turb.
*
* AUTHOR
*
*   CEY [2/95]
*
* DESCRIPTION   : This routine is called when a classic outside-the-warp
*  turbulence parameter is parsed.  One and only one classic turb may exist
*  in a warp chain.  If there is one, it must be the last.  This routine
*  traverses the warp chain and looks at the last link.  If it is not a
*  classic turb then it adds one to the end and returns a pointer to it.
*  If the chain is empty, it creates a single link chain consisting of a
*  classic turb link.  Future warp links get added ahead of the chain so
*  that any classic turb link is always last.
*
* CHANGES
*
******************************************************************************/

ClassicTurbulence *Parser::Check_Turb (WarpList& warps, bool patternHandlesTurbulence)
{
    ClassicTurbulence* turb = nullptr;
    if (!warps.empty())
        turb = dynamic_cast<ClassicTurbulence*>(warps.front());
    if (!turb)
    {
        turb = new ClassicTurbulence(patternHandlesTurbulence);
        warps.insert(warps.begin(), turb);
    }
    return turb;
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*    Talious 10/24/1998: Added Spherical/Cylindrical/Toroidal warps
*
******************************************************************************/

void Parser::Parse_Warp (WarpList& warps)
{
    GenericWarp *New = nullptr;
    TurbulenceWarp *Turb;
    RepeatWarp *Repeat;
    BlackHoleWarp *Black_Hole;
    Vector3d Local_Vector;
    CylindricalWarp *CylW;
    SphericalWarp *SphereW;
    ToroidalWarp *Toroidal;
    PlanarWarp *PlanarW;

    Parse_Begin();

    EXPECT_ONE
        CASE(TURBULENCE_TOKEN)
            New = Turb = new TurbulenceWarp();
            Parse_Vector(Turb->Turbulence);
            EXPECT
                CASE(OCTAVES_TOKEN)
                    Turb->Octaves = (int)Parse_Float();
                    if(Turb->Octaves < 1)
                        Turb->Octaves = 1;
                    if(Turb->Octaves > 10)  /* Avoid DOMAIN errors */
                        Turb->Octaves = 10;
                END_CASE

                CASE (OMEGA_TOKEN)
                    Turb->Omega = Parse_Float();
                END_CASE

                CASE (LAMBDA_TOKEN)
                    Turb->Lambda = Parse_Float();
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
        END_CASE

        CASE(REPEAT_TOKEN)
            New = Repeat = new RepeatWarp();
            Parse_Vector(Local_Vector);
            Repeat->Axis=-1;
            if (Local_Vector[X]!=0.0)
            {
                Repeat->Axis=X;
            }
            if (Local_Vector[Y]!=0.0)
            {
                if (Repeat->Axis < X)
                {
                    Repeat->Axis=Y;
                }
                else
                {
                    Error("Can only repeat along 1 axis.");
                }
            }
            if (Local_Vector[Z]!=0.0)
            {
                if (Repeat->Axis < X)
                {
                    Repeat->Axis=Z;
                }
                else
                {
                    Error("Can only repeat along 1 axis.");
                }
            }
            if (Repeat->Axis < X)
            {
                Error("No axis specified in repeat.");
            }
            Repeat->Width=Local_Vector[Repeat->Axis];

            EXPECT
                CASE(OFFSET_TOKEN)
                    Parse_Vector(Repeat->Offset);
                END_CASE

                CASE(FLIP_TOKEN)
                    Parse_Vector(Repeat->Flip);
                    if (Repeat->Flip[X]!=0.0) Repeat->Flip[X]=-1.0; else Repeat->Flip[X]=1.0;
                    if (Repeat->Flip[Y]!=0.0) Repeat->Flip[Y]=-1.0; else Repeat->Flip[Y]=1.0;
                    if (Repeat->Flip[Z]!=0.0) Repeat->Flip[Z]=-1.0; else Repeat->Flip[Z]=1.0;
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
        END_CASE

        CASE(BLACK_HOLE_TOKEN)
            New = Black_Hole = new BlackHoleWarp();
            Parse_Vector (Local_Vector);
            Black_Hole->Center = Local_Vector;
            Parse_Comma ();
            Black_Hole->Radius = Parse_Float ();
            Black_Hole->Radius_Squared = Black_Hole->Radius * Black_Hole->Radius;
            Black_Hole->Inverse_Radius = 1.0 / Black_Hole->Radius;
            Black_Hole->Strength = 1.0;
            Black_Hole->Power = 2.0;
            Black_Hole->Inverted = false;
            Black_Hole->Type = 0;

            EXPECT
                CASE(STRENGTH_TOKEN)
                    Black_Hole->Strength = Parse_Float ();
                END_CASE

                CASE(FALLOFF_TOKEN)
                    Black_Hole->Power = Parse_Float ();
                END_CASE

                CASE(INVERSE_TOKEN)
                    Black_Hole->Inverted = true;
                END_CASE

                CASE(TYPE_TOKEN)
                    Black_Hole->Type = (int) Parse_Float ();
                END_CASE

                CASE(REPEAT_TOKEN)
                    Parse_Vector (Black_Hole->Repeat_Vector);
                    Black_Hole->Repeat = true;
                    Check_BH_Parameters (Black_Hole);
                END_CASE

                CASE(TURBULENCE_TOKEN)
                    Parse_Vector (Black_Hole->Uncertainty_Vector);
                    Black_Hole->Uncertain = true;
                    Check_BH_Parameters (Black_Hole);
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
        END_CASE

        CASE(CYLINDRICAL_TOKEN)
            New = CylW = new CylindricalWarp();
            EXPECT
                CASE(ORIENTATION_TOKEN)
                    Parse_Vector (Local_Vector);
                    Local_Vector.normalize();
                    CylW->Orientation_Vector = Local_Vector;
                END_CASE

                CASE(DIST_EXP_TOKEN)
                    CylW->DistExp = Parse_Float ();
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
        END_CASE

        CASE(SPHERICAL_TOKEN)
            New = SphereW = new SphericalWarp();
            EXPECT
                CASE(ORIENTATION_TOKEN)
                    Parse_Vector (Local_Vector);
                    Local_Vector.normalize();
                    SphereW->Orientation_Vector = Local_Vector;
                END_CASE

                CASE(DIST_EXP_TOKEN)
                    SphereW->DistExp = Parse_Float ();
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
        END_CASE

        CASE(PLANAR_TOKEN)
            New = PlanarW = new PlanarWarp();
            if(Allow_Vector(Local_Vector))
            {
                Local_Vector.normalize();
                PlanarW->Orientation_Vector = Local_Vector;
                Parse_Comma();
                PlanarW->OffSet=Parse_Float();
            }
        END_CASE

        CASE(TOROIDAL_TOKEN)
            New = Toroidal = new ToroidalWarp();
            EXPECT
                CASE(ORIENTATION_TOKEN)
                    Parse_Vector (Local_Vector);
                    Local_Vector.normalize();
                    Toroidal->Orientation_Vector = Local_Vector;
                END_CASE

                CASE(DIST_EXP_TOKEN)
                    Toroidal->DistExp = Parse_Float ();
                END_CASE

                CASE(MAJOR_RADIUS_TOKEN)
                    Toroidal->MajorRadius = Parse_Float ();
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
        END_CASE

        // JN2007: Cubic warp
        CASE(CUBIC_TOKEN)
            New = new CubicWarp();
        END_CASE

        OTHERWISE
            Expectation_Error ("warp type");
        END_CASE
    END_EXPECT

    if (New == nullptr)
    {
        Error("Empty warp statement.");
    }

    warps.push_back(New);

    Parse_End();
}


void Parser::Parse_Material(MATERIAL *Material)
{
    MATERIAL *Temp;
    TEXTURE *Texture;
    TEXTURE * Int_Texture;
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;

    Parse_Begin();

    if (AllowToken(MATERIAL_ID_TOKEN))
    {
        Temp = CurrentTokenDataPtr<MATERIAL*>();
        Texture = Copy_Textures(Temp->Texture);
        Int_Texture = Copy_Textures(Temp->Interior_Texture);
        Link_Textures(&(Material->Texture), Texture);
        Link_Textures(&(Material->Interior_Texture), Int_Texture);
        if (Temp->interior != nullptr)
            Material->interior = InteriorPtr(new Interior(*(Temp->interior)));
        else
            Material->interior.reset();
    }

    EXPECT
        CASE (TEXTURE_TOKEN)
            Parse_Begin ();
            Texture = Parse_Texture ();
            Parse_End ();
            Link_Textures(&(Material->Texture),Texture);
        END_CASE

        CASE (INTERIOR_TEXTURE_TOKEN)
            Parse_Begin ();
            Int_Texture = Parse_Texture ();
            Parse_End ();
            Link_Textures(&(Material->Interior_Texture), Int_Texture);
        END_CASE

        CASE (INTERIOR_TOKEN)
            Parse_Interior(Material->interior);
        END_CASE

        CASE (TRANSLATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Translation_Transform(&Local_Trans, Local_Vector);
            Transform_Textures (Material->Texture, &Local_Trans);
            if (Material->interior!= nullptr)
                Material->interior->Transform(&Local_Trans);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Rotation_Transform(&Local_Trans, Local_Vector);
            Transform_Textures (Material->Texture, &Local_Trans);
            if (Material->interior!= nullptr)
                Material->interior->Transform(&Local_Trans);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector (Local_Vector);
            Compute_Scaling_Transform(&Local_Trans, Local_Vector);
            Transform_Textures (Material->Texture, &Local_Trans);
            if (Material->interior!= nullptr)
                Material->interior->Transform(&Local_Trans);
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix(Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Transform_Textures (Material->Texture, &Local_Trans);
            if (Material->interior!= nullptr)
                Material->interior->Transform(&Local_Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Parse_Transform(&Local_Trans);
            Transform_Textures (Material->Texture, &Local_Trans);
            if (Material->interior!= nullptr)
                Material->interior->Transform(&Local_Trans);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End();
}


/*****************************************************************************
 *
 * FUNCTION
 *
 *   Parse_PatternFunction
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
 *   -
 *
 * CHANGES
 *
 *   -
 *
 ******************************************************************************/

void Parser::Parse_PatternFunction(TPATTERN *New)
{
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    ClassicTurbulence *Local_Turb;
    int i;
    TraceThreadData *Thread = GetParserDataPtr();
    UCS2String ign;
    ContinuousPattern* pContinuousPattern;

    EXPECT_ONE
        CASE (AGATE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new AgatePattern());
            Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());
        END_CASE

        CASE (BOZO_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new BozoPattern());
        END_CASE

        CASE (FUNCTION_TOKEN)
            {
                New->Type = GENERIC_PATTERN;
                shared_ptr<FunctionPattern> pattern(new FunctionPattern());
                pattern->pFn = new FunctionVM::CustomFunction(mpFunctionVM.get(), Parse_Function());
                New->pattern = pattern;
            }
        END_CASE

        // USER_DEFINED_TOKEN is not accepted, as it produces a colour rather than a scalar.

        CASE(PIGMENT_PATTERN_TOKEN)
            {
                Parse_Begin();
                New->Type = GENERIC_PATTERN;
                shared_ptr<PigmentPattern> pattern(new PigmentPattern());
                pattern->pPigment = Create_Pigment();
                Parse_Pigment(&(pattern->pPigment));
                Post_Pigment(pattern->pPigment);
                Parse_End();
                New->pattern = pattern;
            }
        END_CASE

        CASE (GRANITE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new GranitePattern());
        END_CASE

        CASE (LEOPARD_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new LeopardPattern());
        END_CASE

        CASE (MARBLE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new MarblePattern());
        END_CASE

        CASE (MANDEL_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseMandelPattern();
        END_CASE

        CASE (JULIA_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseJuliaPattern();
        END_CASE

        CASE (MAGNET_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseMagnetPattern();
        END_CASE

        CASE (ONION_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new OnionPattern());
        END_CASE

        CASE (SPIRAL1_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseSpiralPattern<Spiral2Pattern>();
        END_CASE

        CASE (SPIRAL2_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseSpiralPattern<Spiral2Pattern>();
        END_CASE

        CASE (SPOTTED_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new SpottedPattern());
        END_CASE

        CASE (WOOD_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new WoodPattern());
        END_CASE

        CASE (GRADIENT_TOKEN)
            {
                New->Type = GENERIC_PATTERN;
                shared_ptr<GradientPattern> pattern(new GradientPattern());
                Parse_Vector (Local_Vector);
                Local_Vector.normalize();
                pattern->gradient = Local_Vector;
                New->pattern = pattern;
            }
        END_CASE

        CASE (RADIAL_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new RadialPattern());
        END_CASE

        CASE (CRACKLE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new CracklePattern());
        END_CASE

        // COLOUR is not accepted, as it produces a colour rather than a scalar.

        // UV_MAPPING_TOKEN is not accepted, as it requires UV mapping information, which can't be passed to a function.

        CASE (CHECKER_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new CheckerPattern());
            // New->Blend_Map not parsed in pattern functions.
        END_CASE

        CASE (OBJECT_TOKEN)
            {
                Parse_Begin();
                vector<ObjectPtr> tempObjects;
                Parse_Bound_Clip(tempObjects, false);
                if(tempObjects.size() != 1)
                    Error ("object or object identifier expected.");
                New->Type = GENERIC_PATTERN;
                shared_ptr<ObjectPattern> pattern(new ObjectPattern());
                pattern->pObject = tempObjects[0];
                New->pattern = pattern;
                // New->Blend_Map not parsed in pattern functions.
                Parse_End();
            }
        END_CASE

        CASE (CELLS_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new CellsPattern());
        END_CASE

        CASE (BRICK_TOKEN)
            if (!dynamic_cast<BrickPattern*>(New->pattern.get()))
            {
                New->Type = GENERIC_PATTERN;
                New->pattern = PatternPtr(new BrickPattern());
            }
            // New->Blend_Map not parsed in pattern functions.
        END_CASE

        CASE (HEXAGON_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new HexagonPattern());
            // New->Blend_Map not parsed in pattern functions.
        END_CASE

        // TODO VERIFY - SQUARE_TOKEN is not accepted, is that ok?

        // TODO VERIFY - TRIANGULAR_TOKEN is not accepted, is that ok?

        // JN2007: Cubic pattern
        CASE (CUBIC_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new CubicPattern());
            // New->Blend_Map not parsed in pattern functions.
        END_CASE

        // IMAGE_MAP_TOKEN is not accepted, as it produces a colour rather than a scalar.

        // BUMP_MAP_TOKEN is not accepted, as it makes no sense in a pattern function.

        CASE (WAVES_TOKEN)
            New->Type = WAVES_PATTERN;
            New->pattern = PatternPtr(new WavesPattern());
        END_CASE

        CASE (RIPPLES_TOKEN)
            New->Type = RIPPLES_PATTERN;
            New->pattern = PatternPtr(new RipplesPattern());
        END_CASE

        CASE (WRINKLES_TOKEN)
            New->Type = WRINKLES_PATTERN;
            New->pattern = PatternPtr(new WrinklesPattern());
        END_CASE

        CASE (BUMPS_TOKEN)
            New->Type = BUMPS_PATTERN;
            New->pattern = PatternPtr(new BumpsPattern());
        END_CASE

        CASE (DENTS_TOKEN)
            New->Type = DENTS_PATTERN;
            New->pattern = PatternPtr(new DentsPattern());
        END_CASE

        CASE (QUILTED_TOKEN)
            New->Type = QUILTED_PATTERN;
            New->pattern = PatternPtr(new QuiltedPattern());
        END_CASE

        // FACETS_TOKEN is not accepted, as it requires normal vector information, which can't be passed to a function

        // AVERAGE_TOKEN is not accepted, as it isn't really a pattern.

        CASE (IMAGE_PATTERN_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseImagePattern();
        END_CASE

        CASE (PLANAR_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new PlanarPattern());
        END_CASE

        CASE (BOXED_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new BoxedPattern());
        END_CASE

        CASE (SPHERICAL_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new SphericalPattern());
        END_CASE

        CASE (CYLINDRICAL_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new CylindricalPattern());
        END_CASE

        CASE (DENSITY_FILE_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParseDensityFilePattern();
        END_CASE

        // SLOPE_TOKEN is not accepted, as it requires normal vector information, which can't be passed to a function

        // AOI_TOKEN is not accepted, as it requires normal vector information, which can't be passed to a function

        CASE (PAVEMENT_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = PatternPtr(new PavementPattern());
        END_CASE

        CASE (TILING_TOKEN)
            {
                New->Type = GENERIC_PATTERN;
                shared_ptr<TilingPattern> pattern(new TilingPattern());
                pattern->tilingType = (unsigned char)Parse_Float();
                New->pattern = pattern;
            }
        END_CASE

        CASE (POTENTIAL_TOKEN)
            New->Type = GENERIC_PATTERN;
            New->pattern = ParsePotentialPattern();
        END_CASE

        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT

    EXPECT
        // ACCURACY_TOKEN is not accepted, as it is only useful for normal perturbations

        CASE (SOLID_TOKEN)
            if (CracklePattern* pattern = dynamic_cast<CracklePattern*>(New->pattern.get()))
                pattern->crackleIsSolid = true;
            else
                Only_In("solid", "crackle");
        END_CASE

        CASE (EXTERIOR_TOKEN)
            if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
            {
                pattern->Exterior = (unsigned char)Parse_Float();
            }
            else if (FractalPattern* pattern = dynamic_cast<FractalPattern*>(New->pattern.get()))
            {
                pattern->exteriorType = (int)Parse_Float();
                if((pattern->exteriorType < 0) || (pattern->exteriorType > 8))
                    Error("Invalid fractal pattern exterior type. Valid types are 0 to 8.");
                Parse_Comma();
                pattern->exteriorFactor = Parse_Float();
            }
            else
            {
                Only_In("exterior", "mandel, julia, magnet or pavement");
            }
        END_CASE

        CASE (INTERIOR_TOKEN)
            if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
            {
                pattern->Interior = (unsigned char)Parse_Float();
            }
            else if (FractalPattern* pattern = dynamic_cast<FractalPattern*>(New->pattern.get()))
            {
                pattern->interiorType = (int)Parse_Float();
                if((pattern->interiorType < 0) || (pattern->interiorType > 6))
                    Error("Invalid fractal pattern interior type. Valid types are 0 to 6.");
                Parse_Comma();
                pattern->interiorFactor = Parse_Float();
            }
            else
            {
                Only_In("exterior", "mandel, julia, magnet or pavement");
            }
        END_CASE

        CASE (EXPONENT_TOKEN)
            if (JuliaPattern* oldPattern = dynamic_cast<JuliaPattern*>(New->pattern.get()))
            {
                i = (int)Parse_Float();
                switch(i)
                {
                    case 2:
                        New->pattern = PatternPtr(new JuliaPattern(*oldPattern));
                        break;
                    case 3:
                        New->pattern = PatternPtr(new Julia3Pattern(*oldPattern));
                        break;
                    case 4:
                        New->pattern = PatternPtr(new Julia4Pattern(*oldPattern));
                        break;
                    default:
                        if((i > 4) && (i <= kFractalMaxExponent))
                        {
                            shared_ptr<JuliaXPattern> pattern(new JuliaXPattern(*oldPattern));
                            pattern->fractalExponent = i;
                            New->pattern = pattern;
                        }
                        else
                        {
                            New->pattern = PatternPtr(new JuliaPattern(*oldPattern));
                            Warning("Invalid julia pattern exponent found. Supported exponents are 2 to %i.\n"
                                    "Using default exponent 2.", kFractalMaxExponent);
                        }
                        break;
                }
            }
            else if (MandelPattern* oldPattern = dynamic_cast<MandelPattern*>(New->pattern.get()))
            {
                i = (int)Parse_Float();
                switch(i)
                {
                    case 2:
                        New->pattern = PatternPtr(new Mandel2Pattern(*oldPattern));
                        break;
                    case 3:
                        New->pattern = PatternPtr(new Mandel3Pattern(*oldPattern));
                        break;
                    case 4:
                        New->pattern = PatternPtr(new Mandel4Pattern(*oldPattern));
                        break;
                    default:
                        if((i > 4) && (i <= kFractalMaxExponent))
                        {
                            shared_ptr<MandelXPattern> pattern(new MandelXPattern(*oldPattern));
                            pattern->fractalExponent = i;
                            New->pattern = pattern;
                        }
                        else
                        {
                            New->pattern = PatternPtr(new Mandel2Pattern(*oldPattern));
                            Warning("Invalid mandel pattern exponent found. Supported exponents are 2 to %i.\n"
                                    "Using default exponent 2.", kFractalMaxExponent);
                        }
                        break;
                }
            }
            else
                Only_In("exponent", "mandel or julia");
        END_CASE

        CASE (COORDS_TOKEN)
            if (FacetsPattern* pattern = dynamic_cast<FacetsPattern*>(New->pattern.get()))
                pattern->facetsCoords = Parse_Float();
            else
                Only_In("coords", "facets");
        END_CASE

        CASE (SIZE_TOKEN)
            if (FacetsPattern* pattern = dynamic_cast<FacetsPattern*>(New->pattern.get()))
                pattern->facetsSize = Parse_Float();
            else
                Only_In("size", "facets");
        END_CASE

        CASE (METRIC_TOKEN)
            if (FacetsPattern* pattern = dynamic_cast<FacetsPattern*>(New->pattern.get()))
            {
                Parse_Vector(Local_Vector);
                pattern->facetsMetric = Local_Vector[X];
            }
            else if (CracklePattern* pattern = dynamic_cast<CracklePattern*>(New->pattern.get()))
            {
                // Vector for backwards compatibility
                // the only component used was always X.
                Parse_Vector(Local_Vector);
                pattern->crackleMetric = Local_Vector[X];
            }
            else
                Only_In("metric", "facets or crackle");
        END_CASE

        CASE (FORM_TOKEN)
            if (CracklePattern* pattern = dynamic_cast<CracklePattern*>(New->pattern.get()))
                Parse_Vector(pattern->crackleForm);
            else if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
                pattern->Form = ((unsigned char)Parse_Float());
            else
                Only_In("form", "crackle or pavement");
        END_CASE

        CASE (OFFSET_TOKEN)
            if (CracklePattern* pattern = dynamic_cast<CracklePattern*>(New->pattern.get()))
                pattern->crackleOffset = Parse_Float();
            else
                Only_In("offset", "crackle");
        END_CASE

        CASE (TURBULENCE_TOKEN)
            Local_Turb=Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());
            Parse_Vector(Local_Turb->Turbulence);
        END_CASE

        // COLOUR_MAP is not accepted, as pattern functions aren't mapped to anything.

        // PIGMENT_MAP is not accepted, as pattern functions aren't mapped to anything.

        // DENSITY_MAP is not accepted, as pattern functions aren't mapped to anything.

        // NORMAL_MAP is not accepted, as pattern functions aren't mapped to anything.

        // TEXTURE_MAP is not accepted, as pattern functions aren't mapped to anything.

        // QUICK_COLOUR is not accepted, as it makes no sense in a pattern function.

        CASE (CONTROL0_TOKEN)
            if (QuiltedPattern* pattern = dynamic_cast<QuiltedPattern*>(New->pattern.get()))
                pattern->Control0 = Parse_Float ();
            else
                Not_With ("control0","this pattern");
        END_CASE

        CASE (CONTROL1_TOKEN)
            if (QuiltedPattern* pattern = dynamic_cast<QuiltedPattern*>(New->pattern.get()))
                pattern->Control1 = Parse_Float ();
            else
                Not_With ("control1","this pattern");
        END_CASE

        CASE (OCTAVES_TOKEN)
            Local_Turb=Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());
            Local_Turb->Octaves = (int)Parse_Float();
            if(Local_Turb->Octaves < 1)
                Local_Turb->Octaves = 1;
            if(Local_Turb->Octaves > 10)  /* Avoid DOMAIN errors */
                Local_Turb->Octaves = 10;
        END_CASE

        CASE (OMEGA_TOKEN)
            Local_Turb=Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());
            Local_Turb->Omega = Parse_Float();
        END_CASE

        CASE (LAMBDA_TOKEN)
            Local_Turb=Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());
            Local_Turb->Lambda = Parse_Float();
        END_CASE

        CASE (FREQUENCY_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveFrequency = Parse_Float();
            else
            {
                Warning("frequency has no effect on discrete patterns");
                Parse_Float();
            }
        END_CASE

        CASE (RAMP_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveType = kWaveType_Ramp;
            else
                Warning("ramp_wave has no effect on discrete patterns");
        END_CASE

        CASE (TRIANGLE_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveType = kWaveType_Triangle;
            else
                Warning("triangle_wave has no effect on discrete patterns");
        END_CASE

        CASE (SINE_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveType = kWaveType_Sine;
            else
                Warning("sine_wave has no effect on discrete patterns");
        END_CASE

        CASE (SCALLOP_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveType = kWaveType_Scallop;
            else
                Warning("scallop_wave has no effect on discrete patterns");
        END_CASE

        CASE (CUBIC_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->waveType = kWaveType_Cubic;
            else
                Warning("cubic_wave has no effect on discrete patterns");
        END_CASE

        CASE (POLY_WAVE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
            {
                pContinuousPattern->waveType = kWaveType_Poly;
                pContinuousPattern->waveExponent  = Allow_Float(pContinuousPattern->waveExponent);
            }
            else
            {
                Warning("poly_wave has no effect on discrete patterns");
                Allow_Float(0.0);
            }
        END_CASE

        CASE (PHASE_TOKEN)
            pContinuousPattern = dynamic_cast<ContinuousPattern*>(New->pattern.get());
            if (pContinuousPattern != nullptr)
                pContinuousPattern->wavePhase = Parse_Float();
            else
            {
                Warning("phase has no effect on discrete patterns");
                Parse_Float();
            }
        END_CASE

        // BUMP_SIZE is not accepted, as it is only useful for normal perturbations

        CASE (NOISE_GENERATOR_TOKEN)
        {
            int noise_generator;
            noise_generator = (int) Parse_Float();
            if (noise_generator < kNoiseGen_Min || noise_generator > kNoiseGen_Max)
                Error ("Value for noise_generator in global_settings must be between %i (inclusive) and %i (inclusive).", kNoiseGen_Min, kNoiseGen_Max);
            New->pattern->noiseGenerator = noise_generator;
        }
        END_CASE

        CASE (AGATE_TURB_TOKEN)
            if (AgatePattern* pattern = dynamic_cast<AgatePattern*>(New->pattern.get()))
            {
                pattern->agateTurbScale = Parse_Float();
                Check_Turb(New->pattern->warps, New->pattern->HasSpecialTurbulenceHandling());   // agate needs Octaves, Lambda etc.
            }
            else
                Not_With ("agate_turb","non-agate");
        END_CASE

        CASE (BRICK_SIZE_TOKEN)
            if (BrickPattern* pattern = dynamic_cast<BrickPattern*>(New->pattern.get()))
                Parse_Vector(pattern->brickSize);
            else
                Not_With ("brick_size","non-brick");
        END_CASE

        CASE (MORTAR_TOKEN)
            if (BrickPattern* pattern = dynamic_cast<BrickPattern*>(New->pattern.get()))
                pattern->mortar = Parse_Float()-EPSILON*2.0;
            else
                Not_With ("mortar","non-brick");
        END_CASE

        CASE (INTERPOLATE_TOKEN)
            if (DensityFilePattern* pattern = dynamic_cast<DensityFilePattern*>(New->pattern.get()))
                pattern->densityFile->Interpolation = (int)Parse_Float();
            else
                Not_With ("interpolate","non-density_file");
        END_CASE

        CASE (NUMBER_OF_SIDES_TOKEN)
            if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
                pattern->Side = (unsigned char)Parse_Float();
            else
                Only_In("number_of_sides","pavement");
        END_CASE

        CASE (NUMBER_OF_TILES_TOKEN)
            if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
                pattern->Tile = (unsigned char)Parse_Float();
            else
                Only_In("number_of_tiles","pavement");
        END_CASE

        CASE (PATTERN_TOKEN)
            if (PavementPattern* pattern = dynamic_cast<PavementPattern*>(New->pattern.get()))
                pattern->Number = (unsigned char)Parse_Float();
            else
                Only_In("pattern","pavement");
        END_CASE

        CASE (WARP_TOKEN)
            Parse_Warp(New->pattern->warps);
        END_CASE

        CASE (TRANSLATE_TOKEN)
            Parse_Vector (Local_Vector);
            Translate_Tpattern (New, Local_Vector);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (Local_Vector);
            Rotate_Tpattern (New, Local_Vector);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector (Local_Vector);
            Scale_Tpattern (New, Local_Vector);
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix(Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Transform_Tpattern (New, &Local_Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Transform_Tpattern (New, Parse_Transform(&Local_Trans));
        END_CASE

        CASE (NO_BUMP_SCALE_TOKEN)
            Set_Flag(New, DONT_SCALE_BUMPS_FLAG);
        END_CASE

        CASE (REPEAT_TOKEN)
            if (CracklePattern* pattern = dynamic_cast<CracklePattern*>(New->pattern.get()))
            {
                Parse_Vector(Local_Vector);
                pattern->repeat = IntVector3d(Local_Vector);
                if ((pattern->repeat.x() < 0) ||
                    (pattern->repeat.y() < 0) ||
                    (pattern->repeat.z() < 0))
                    Error("Repeat vector must be non-negative.");
            }
            else
                Only_In("repeat","crackle");
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    VerifyPattern(New->pattern);

    if (!New->pattern->Precompute())
        Error("Unspecified parse error in pattern.");
}

}
// end of namespace pov_parser
