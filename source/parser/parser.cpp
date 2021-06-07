//******************************************************************************
///
/// @file parser/parser.cpp
///
/// This module implements a parser for the scene description files.
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
#include <cctype>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/fileutil.h"
#include "base/path.h"
#include "base/povassert.h"
#include "base/stringutilities.h"
#include "base/types.h"
#include "base/image/colourspace.h"
#include "base/image/image.h"

// POV-Ray header files (core module)
#include "core/bounding/boundingcylinder.h"
#include "core/bounding/boundingsphere.h"
#include "core/lighting/lightgroup.h"
#include "core/lighting/lightsource.h"
#include "core/lighting/photons.h"
#include "core/lighting/radiosity.h"
#include "core/lighting/subsurface.h"
#include "core/material/blendmap.h"
#include "core/material/interior.h"
#include "core/material/noise.h"
#include "core/material/normal.h"
#include "core/material/pattern.h"
#include "core/material/pigment.h"
#include "core/material/texture.h"
#include "core/math/matrix.h"
#include "core/math/polynomialsolver.h"
#include "core/math/spline.h"
#include "core/math/vector.h"
#include "core/scene/atmosphere.h"
#include "core/scene/object.h"
#include "core/scene/scenedata.h"
#include "core/scene/tracethreaddata.h"
#include "core/shape/bezier.h"
#include "core/shape/blob.h"
#include "core/shape/box.h"
#include "core/shape/cone.h"
#include "core/shape/csg.h"
#include "core/shape/disc.h"
#include "core/shape/fractal.h"
#include "core/shape/heightfield.h"
#include "core/shape/isosurface.h"
#include "core/shape/lathe.h"
#include "core/shape/lemon.h"
#include "core/shape/mesh.h"
#include "core/shape/ovus.h"
#include "core/shape/parametric.h"
#include "core/shape/plane.h"
#include "core/shape/polynomial.h"
#include "core/shape/polygon.h"
#include "core/shape/prism.h"
#include "core/shape/quadric.h"
#include "core/shape/sor.h"
#include "core/shape/sphere.h"
#include "core/shape/spheresweep.h"
#include "core/shape/superellipsoid.h"
#include "core/shape/torus.h"
#include "core/shape/triangle.h"
#include "core/shape/truetype.h"
#include "core/support/imageutil.h"
#include "core/support/octree.h"

// POV-Ray header files (VM module)
#include "vm/fnpovfpu.h"

// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

using namespace pov;

using std::min;
using std::max;
using std::shared_ptr;
using std::vector;

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Volume that is considered to be infinite. [DB 9/94] */

const DBL INFINITE_VOLUME = BOUND_HUGE;


//******************************************************************************

Parser::Parser(shared_ptr<SceneData> sd, const Options& opts,
               GenericMessenger& mf, FileResolver& fr, ProgressReporter& pr, TraceThreadData& td) :
    sceneData(sd),
    clockValue(opts.clock),
    useClock(opts.useClock),
    mMessageFactory(mf),
    mFileResolver(fr),
    mProgressReporter(pr),
    mThreadData(td),
    Debug_Message_Buffer(mf),
    mpFunctionVM(new FunctionVM),
    fnVMContext(new FPUContext(mpFunctionVM.get(), GetParserDataPtr())),
    Destroying_Frame(false),
    mTokenCount(0),
    mTokensSinceLastProgressReport(0),
    next_rand(nullptr)
{
    std::tm tmY2K;
    // Field       = Value - Base
    tmY2K.tm_year  =  2000 - 1900;
    tmY2K.tm_mon   =     1 -    1;
    tmY2K.tm_mday  =     1 -    0;
    tmY2K.tm_hour  =     0;
    tmY2K.tm_min   =     0;
    tmY2K.tm_sec   =     0;
    tmY2K.tm_isdst = false;
    // `std::mktime()` doesn't need `tmY2K.tm_wday` nor `tmY2K.yday` to be set.
    mY2K = std::chrono::system_clock::from_time_t(std::mktime(&tmY2K));

    pre_init_tokenizer();
    if (sceneData->realTimeRaytracing)
        mBetaFeatureFlags.realTimeRaytracing = true;

    sceneData->functionContextFactory = mpFunctionVM;
}

Parser::~Parser()
{
    // NB: We need to keep fnVMContext around until all functions have been destroyed.
    delete fnVMContext;
}

/* Parse the file. */
void Parser::Run()
{
    SourceInfo errorInfo(UCS2String(POV_FILENAME_BUFFER_CHARS, u'\0'), // Pre-claim some memory, so we can handle an out-of-memory error.
                         SourcePosition(-1,-1,-1));

    // Outer try/catch block to handle out-of-memory conditions
    // occurring during regular error handling.
    try
    {
        // Main "try/catch" block to handle most error conditions.
        try
        {
            Init_Random_Generators();

            Initialize_Tokenizer();

            Default_Texture = Create_Texture ();
            Default_Texture->Pigment = Create_Pigment();
            Default_Texture->Tnormal = nullptr;
            Default_Texture->Finish  = Create_Finish();

            defaultsVersion = DefaultsVersion::kLegacy;
            defaultsModified = false;

            // Initialize various defaults depending on language version as per command line / INI settings.
            InitDefaults(sceneData->EffectiveLanguageVersion());

            Not_In_Default = true;
            Ok_To_Declare = true;
            LValue_Ok = false;
            parsingVersionDirective = false;

            Frame_Init ();

            for(SceneData::DeclaredVariablesMap::const_iterator i(sceneData->declaredVariables.begin()); i != sceneData->declaredVariables.end(); i++)
            {
                if(i->second.length() > 0)
                {
                    SYM_ENTRY *Temp_Entry = nullptr;

                    if(i->second[0] == '\"')
                    {
                        std::string tmp(i->second, 1, i->second.length() - 2);
                        Temp_Entry = mSymbolStack.GetGlobalTable()->Add_Symbol(i->first, STRING_ID_TOKEN);
                        Temp_Entry->Data = String_Literal_To_UCS2(tmp);
                    }
                    else
                    {
                        Temp_Entry = mSymbolStack.GetGlobalTable()->Add_Symbol(i->first, FLOAT_ID_TOKEN);
                        Temp_Entry->Data = Create_Float();
                        *(reinterpret_cast<DBL *>(Temp_Entry->Data)) = std::atof(i->second.c_str());
                    }
                }
            }

            IncludeHeader(sceneData->headerFile);

            Parse_Frame();

            // post process atmospheric media
            for (vector<Media>::iterator i(sceneData->atmosphere.begin()); i != sceneData->atmosphere.end(); i++)
                i->PostProcess();

            // post process global light sources
            for (size_t i = 0; i < sceneData->lightSources.size(); i++)
            {
                sceneData->lightSources[i]->index = i;
                sceneData->lightSources[i]->lightGroupLight = false;
            }

            // post process local light sources
            for (size_t i = 0; i < sceneData->lightGroupLightSources.size(); i++)
            {
                sceneData->lightGroupLightSources[i]->index = i;
                sceneData->lightGroupLightSources[i]->lightGroupLight = true;
            }
        }
        // Make sure any exceptional situations are reported as a parse error (pov_base::Exception)
        // (both to the user interface and "upward" to the calling code)
        catch (const IncompleteCommentException& e)
        {
            Error(e, "Unterminated block comment in input file.");
        }
        catch (const IncompleteStringLiteralException& e)
        {
            Error(e, "Unterminated string literal in input file.");
        }
        catch (const InvalidEncodingException& e)
        {
            if (e.details == nullptr)
                Error(e, "Invalid byte sequence in %s input file.", e.encodingName);
            else
                Error(e, "Invalid byte sequence in %s input file (%s).", e.encodingName, e.details);
        }
        catch (const InvalidCharacterException& e)
        {
            if (e.offendingCharacter <= 0x100u)
                Error(e, "Illegal character %02x in input file.", int(e.offendingCharacter));
            else
                Error(e, "Illegal character U+%04x in input file.", int(e.offendingCharacter));
        }
        catch (const InvalidEscapeSequenceException& e)
        {
            Error(e, "Illegal escape sequence '%s' in string literal.", e.offendingText.c_str());
        }
        catch (pov_base::Exception& e)
        {
            // Error was detected by the parser, and already reported when first thrown
            throw(e);
        }
        catch (std::exception& e)
        {
            // Some other exceptional situation occurred in a library or some such and couldn't be handled gracefully;
            // handle it now by failing with a corresponding parse error
            Error(e.what());
        }
    }
    catch(std::bad_alloc&)
    {
        try
        {
            if (HaveCurrentFile())
            {
                // take a (local) copy of error location prior to freeing token data
                // NB error_filename has been pre-allocated for strings up to POV_FILENAME_BUFFER_CHARS
                errorInfo.fileName = CurrentFileName();
                errorInfo.position = CurrentFilePosition();
            }

            // free up some memory before proceeding with error notification.
            Terminate_Tokenizer();
            Destroy_Textures(Default_Texture);
            Default_Texture = nullptr;
            Destroy_Random_Generators();

            if (errorInfo.position.line != -1)
                mMessageFactory.ErrorAt(POV_EXCEPTION_CODE(kOutOfMemoryErr), errorInfo, "Out of memory.");
            else
                Error("Out of memory.");
        }
        catch (std::bad_alloc&)
        {
            // ran out of memory during processing of bad_alloc above ...
            // not much we can do except return
            return;
        }
        catch(...)
        {
            // other exceptions are OK to re-throw here (in particular, Error/ErrorAt will throw one)
            throw;
        }
    }

    // validate scene contents
    if(sceneData->objects.empty())
        Warning("No objects in scene.");

    Cleanup();

    // Check for experimental features
    char str[512] = "";

    vector<std::string> featureList;
    std::string featureString;

    if(mExperimentalFlags.backsideIllumination) featureList.push_back("backside illumination");
    if(mExperimentalFlags.functionHf)           featureList.push_back("function '.hf'");
    if(mExperimentalFlags.meshCamera)           featureList.push_back("mesh camera");
    if(mExperimentalFlags.objImport)            featureList.push_back("wavefront obj import");
    if(mExperimentalFlags.slopeAltitude)        featureList.push_back("slope pattern altitude");
    if(mExperimentalFlags.spline)               featureList.push_back("spline");
    if(mExperimentalFlags.subsurface)           featureList.push_back("subsurface light transport");
    if(mExperimentalFlags.tiff)                 featureList.push_back("TIFF image support");
    if(mExperimentalFlags.userDefinedCamera)    featureList.push_back("user-defined camera");

    for (vector<std::string>::iterator i = featureList.begin(); i != featureList.end(); ++i)
    {
        if (!featureString.empty())
            featureString += ", ";
        featureString += *i;
    }

    if (!featureString.empty())
        Warning("This rendering uses the following experimental feature(s): %s.\n"
                "The design and implementation of these features is likely to change in future\n"
                "versions of POV-Ray. Backward compatibility with the current implementation is\n"
                "not guaranteed.",
                featureString.c_str());

    // Check for beta features
    featureList.clear();
    featureString.clear();

    if(mBetaFeatureFlags.videoCapture)          featureList.push_back("video capture");
    if(mBetaFeatureFlags.realTimeRaytracing)    featureList.push_back("real-time raytracing render loop");

    for (vector<std::string>::iterator i = featureList.begin(); i != featureList.end(); ++i)
    {
        if (!featureString.empty())
            featureString += ", ";
        featureString += *i;
    }

    if (!featureString.empty())
        Warning("This rendering uses the following beta-test feature(s): %s.\n"
                "The implementation of these features is likely to change or be completely\n"
                "removed in subsequent beta-test versions of POV-Ray. There is no guarantee\n"
                "that they will be available in the next full release version.\n",
                featureString.c_str());

    if ((sceneData->bspMaxDepth != 0) ||
        (sceneData->bspObjectIsectCost != 0.0f) || (sceneData->bspBaseAccessCost != 0.0f) ||
        (sceneData->bspChildAccessCost != 0.0f) || (sceneData->bspMissChance != 0.0f))
    {
        Warning("You have overridden a default BSP tree cost constant. Note that these "
                "INI settings may be removed or changed without notice in future versions.\n");
    }

    // TODO FIXME - review whole if-statement and line after it below [trf]
    // we set this before resetting languageVersion since there's nothing to
    // be gained from disabling the defaulting of the noise generator to
    // something other than compatibility mode.
    if (sceneData->explicitNoiseGenerator == false)
        sceneData->noiseGenerator = (sceneData->EffectiveLanguageVersion() < 350 ?
                                     kNoiseGen_Original : kNoiseGen_RangeCorrected);

    if((sceneData->gammaMode != kPOVList_GammaMode_AssumedGamma36) && (sceneData->gammaMode != kPOVList_GammaMode_AssumedGamma37))
    {
        if (sceneData->EffectiveLanguageVersion() < 370)
        {
            sceneData->gammaMode = kPOVList_GammaMode_None;
            sceneData->workingGamma.reset();
            sceneData->workingGammaToSRGB.reset();
            Warning("assumed_gamma not specified, so gamma_correction is turned off for compatibility\n"
                    "with this pre POV-Ray v3.7 scene. See the documentation for more details.");
        }
        else
        {
            sceneData->gammaMode = kPOVList_GammaMode_AssumedGamma37Implied;
            sceneData->workingGamma = GetGammaCurve(DEFAULT_WORKING_GAMMA_TYPE, DEFAULT_WORKING_GAMMA);
            sceneData->workingGammaToSRGB = TranscodingGammaCurve::Get(sceneData->workingGamma, SRGBGammaCurve::Get());
            PossibleError("assumed_gamma not specified in this POV-Ray v3.7 or later scene. Future\n"
                          "versions of POV-Ray may consider this a fatal error. To avoid this\n"
                          "warning, explicitly specify 'assumed_gamma " DEFAULT_WORKING_GAMMA_TEXT "' in the global_settings\n"
                          "section. See the documentation for more details.");
        }
    }

    if(sceneData->EffectiveLanguageVersion() < 350)
    {
        Warning("The scene finished parsing with a language version set to v3.1 or earlier. Full\n"
                "backward compatibility with scenes requiring support for bugs in POV-Ray\n"
                "version v3.1 or earlier is not guaranteed. Please use POV-Ray v3.5 or earlier if\n"
                "your scene depends on rendering defects caused by these bugs.");

        sceneData->languageVersion = 350;
    }

    if(sceneData->languageVersionLate)
    {
        Warning("This scene had other declarations preceding the first #version directive.\n"
                "Please be aware that as of POV-Ray v3.7, unless already specified via an INI\n"
                "option, a #version is expected as the first declaration in a scene file. If\n"
                "this is not done, POV-Ray may apply compatibility settings to some features\n"
                "that are intended to make pre-v3.7 scenes render as designed. You are strongly\n"
                "encouraged to add a #version statement to the scene to make your intent clear.\n"
                "Future versions of POV-Ray may make the presence of a #version mandatory.");
    }
    else if(sceneData->languageVersionSet == false)
    {
        Warning("This scene did not contain a #version directive. Please be aware that as of\n"
                "POV-Ray v3.7, unless already specified via an INI option, a #version is\n"
                "expected as the first declaration in a scene file. POV-Ray may apply settings\n"
                "to some features that are intended to maintain compatibility with pre-v3.7\n"
                "scenes. You are strongly encouraged to add a #version statement to the scene\n"
                "to make your intent clear. Future versions of POV-Ray may make the presence of\n"
                "a #version statement mandatory.");
    }

    sceneData->parsedMaxTraceLevel = Max_Trace_Level;

    if (sceneData->clocklessAnimation == true)
    {
        if (sceneData->cameras.size() < 2)
        {
            Warning("Need at least two cameras for a clockless animation loop - treating\nscene as single frame.");
            sceneData->clocklessAnimation = false;
        }
        else
        {
            Warning("Clockless animation: total of %d cameras will be used.", sceneData->cameras.size());
            sceneData->parsedCamera = sceneData->cameras[0];
        }
    }

    if (sceneData->radiositySettings.pretraceEnd > sceneData->radiositySettings.pretraceStart)
        Error("Radiosity pretrace end must be smaller than or equal to pretrace start.");

    if (sceneData->radiositySettings.minimumReuse > sceneData->radiositySettings.maximumReuse * 0.5)
    {
        if (!sceneData->radiositySettings.minimumReuseSet)
        {
            sceneData->radiositySettings.minimumReuse = sceneData->radiositySettings.maximumReuse * 0.5;
            Warning("Radiosity maximum_reuse should be significantly larger than minimum_reuse.\n"
                    "Decreasing minimum_reuse to %lf instead of the default.",
                    sceneData->radiositySettings.minimumReuse);
        }
        else if (!sceneData->radiositySettings.maximumReuseSet)
        {
            sceneData->radiositySettings.maximumReuse = sceneData->radiositySettings.minimumReuse * 2.0;
            Warning("Radiosity maximum_reuse should be significantly larger than minimum_reuse.\n"
                    "Increasing maximum_reuse to %lf instead of the default.",
                    sceneData->radiositySettings.maximumReuse);
        }
        else if (sceneData->radiositySettings.minimumReuse >= sceneData->radiositySettings.maximumReuse)
        {
            Error("Radiosity maximum_reuse must be larger than minimum_reuse.\n");
        }
        else
        {
            Warning("Radiosity maximum_reuse should be significantly larger than minimum_reuse.\n");
        }
    }
}

void Parser::Cleanup()
{
    // TODO FIXME - cleanup [trf]
    Terminate_Tokenizer();

    Destroy_Textures(Default_Texture);
    Default_Texture = nullptr;

    Destroy_Random_Generators();
}

void Parser::Finish()
{
    Cleanup();
}

//******************************************************************************

/* Set up the fields in the frame to default values. */
void Parser::Frame_Init()
{
    Destroying_Frame = false;
    sceneData->parsedCamera = Default_Camera;
    sceneData->lightSources.clear();
    sceneData->atmosphereIOR = 1.0;
    sceneData->atmosphereDispersion = 1.0;
    // TODO FIXME Frame.Antialias_Threshold = opts.Antialias_Threshold;

    /* Init atmospheric stuff. [DB 12/94] */

    sceneData->fog = nullptr;
    sceneData->rainbow = nullptr;
    sceneData->skysphere = nullptr;
}

//******************************************************************************

void Parser::InitDefaults(int version)
{
    // Initialize defaults depending on version:
    // As of v3.8...
    //   - pigment defaults to `rgb <1,1,1>`
    //   - `ambient` defaults to 0.0.
    //   - Camera `right` length defaults to output image aspect ratio.
    // Prior to that...
    //   - pigment defaulted to `rgb <0,0,0>`
    //   - `ambient` defaulted to 0.1.
    //   - Camera `right` length defaulted to 1.33.

    unsigned short pigmentType;
    MathColour pigmentColour;
    double ambientLevel;
    double rightLength;
    DefaultsVersion newDefaults;

    if (version >= 380)
        newDefaults = DefaultsVersion::k380;
    else
        newDefaults = DefaultsVersion::kLegacy;

    if (newDefaults == defaultsVersion)
        // nothing to change
        return;

    if (defaultsModified)
    {
        // Don't override defaults if they've already been modified by the user.
        Warning("Scene language version changed after a 'default' statement. "
                "The changes in defaults normally associated with the language "
                "version change are not applied.");
        return;
    }

    switch (newDefaults)
    {
        case DefaultsVersion::k380:
            pigmentType = PLAIN_PATTERN;
            pigmentColour = MathColour(1.0);
            ambientLevel = 0.0;
            rightLength = sceneData->aspectRatio;
            break;

        case DefaultsVersion::kLegacy:
            pigmentType = NO_PATTERN;
            pigmentColour = MathColour(0.0);
            ambientLevel = 0.1;
            rightLength = 1.33;
            break;

        default:
            POV_PARSER_ASSERT(false);
            break;
    }

    Default_Texture->Pigment->Type = pigmentType;
    Default_Texture->Pigment->colour = TransColour(pigmentColour, 0.0, 0.0);
    Default_Texture->Finish->Ambient = MathColour(ambientLevel);
    Default_Camera.Right = Vector3d(rightLength, 0.0, 0.0);

    defaultsVersion = newDefaults;
}

//******************************************************************************

void Parser::Destroy_Frame()
{
    FOG *Fog, *Next_Fog;
    RAINBOW *Rainbow, *Next_Rainbow;

    // This is necessary as a user who hits CANCEL during any IO performed
    // by this routine (e.g. Destroy_Object(), which can complain about
    // isosurface max_gradient), will cause this routine to be entered again
    // before the relevent data member has been set to `nullptr` (this is able
    // to happen since cancel will invoke a longjmp on most platforms).
    // This causes the currently-executing segment to be destroyed twice,
    // which is a Bad Thing(tm). [CJC 11/01]
    if (Destroying_Frame)
        return;
    Destroying_Frame = true;

    /* Destroy fogs. [DB 12/94] */

    for (Fog = sceneData->fog; Fog != nullptr;)
    {
        Next_Fog = Fog->Next;

        Destroy_Fog(Fog);

        Fog = Next_Fog;
    }

    sceneData->fog = nullptr;

    /* Destroy rainbows. [DB 12/94] */

    for (Rainbow = sceneData->rainbow; Rainbow != nullptr;)
    {
        Next_Rainbow = Rainbow->Next;

        Destroy_Rainbow(Rainbow);

        Rainbow = Next_Rainbow;
    }

    sceneData->rainbow = nullptr;

    /* Destroy skysphere. [DB 12/94] */
    Destroy_Skysphere(sceneData->skysphere);
    sceneData->skysphere = nullptr;

    if(!sceneData->objects.empty())
    {
        Destroy_Object(sceneData->objects);
        sceneData->objects.clear();
        sceneData->lightSources.clear();
    }

    if(!sceneData->lightGroupLightSources.empty())
    {
        for(vector<LightSource *>::iterator i = sceneData->lightGroupLightSources.begin(); i != sceneData->lightGroupLightSources.end(); i++)
            Destroy_Object(*i);
        sceneData->lightGroupLightSources.clear();
    }
}

//******************************************************************************

bool Parser::Parse_Begin (TokenId tokenId, bool mandatory)
{
    Get_Token();

    if(CurrentTrueTokenId() == tokenId)
    {
        maBraceStack.emplace_back(mToken);
        return true;
    }
    else
    {
        if (mandatory)
            Expectation_Error(Get_Token_String(tokenId));
        else
            Unget_Token();

        return false;
    }
}

//******************************************************************************

void Parser::Parse_End(TokenId openTokenId, TokenId expectTokenId)
{
    Get_Token();

    if(CurrentTrueTokenId() == expectTokenId)
    {
        POV_PARSER_ASSERT(!maBraceStack.empty());
        POV_PARSER_ASSERT(openTokenId == maBraceStack.back().openToken);

        if (!maIncludeStack.empty() && (maBraceStack.size() <= maIncludeStack.back().braceStackSize))
        {
            BraceStackEntry& braceStackEntry = maBraceStack.back();
            // Include file has closed more braces/parentheses/etc. than it has opened.
            Warning("Unbalanced %s in include file", Get_Token_String(CurrentTrueTokenId()));
        }
        maBraceStack.pop_back();

        return;
    }

    ErrorInfo(maBraceStack.back(), "Unmatched %s", Get_Token_String(maBraceStack.back().openToken));
    Error("No matching %s, %s found instead", Get_Token_String(expectTokenId), Get_Token_String(CurrentTrueTokenId()));
}

//******************************************************************************

ObjectPtr Parser::Parse_Bicubic_Patch ()
{
    BicubicPatch *Object;
    int i, j;

    Parse_Begin ();

    Object = reinterpret_cast<BicubicPatch *>(Parse_Object_Id());
    if (Object != nullptr)
    {
        return (reinterpret_cast<ObjectPtr>(Object));
    }

    Object = new BicubicPatch();

    EXPECT_CAT
        CASE_FLOAT_UNGET
            VersionWarning(150, "Should use keywords for bicubic parameters.");
            Object->Patch_Type = (int)Parse_Float();
            if (Object->Patch_Type == 2 ||
                Object->Patch_Type == 3)
            {
                Object->Flatness_Value = Parse_Float();
            }
            else
            {
                Object->Flatness_Value = 0.1;
            }
            Object->U_Steps = (int)Parse_Float();
            Object->V_Steps = (int)Parse_Float();
            EXIT
        END_CASE

        CASE (TYPE_TOKEN)
            Object->Patch_Type = (int)Parse_Float();
        END_CASE

        CASE (FLATNESS_TOKEN)
            Object->Flatness_Value = Parse_Float();
        END_CASE

        CASE (V_STEPS_TOKEN)
            Object->V_Steps = (int)Parse_Float();
        END_CASE

        CASE (U_STEPS_TOKEN)
            Object->U_Steps = (int)Parse_Float();
        END_CASE

        CASE (ACCURACY_TOKEN)
            Object->accuracy = Parse_Float();
        END_CASE

        CASE(UV_VECTORS_TOKEN)
            /* Store 4 ST coords for quadrilateral  */
            Parse_UV_Vect(Object->ST[0]);  Parse_Comma();
            Parse_UV_Vect(Object->ST[1]);  Parse_Comma();
            Parse_UV_Vect(Object->ST[2]);  Parse_Comma();
            Parse_UV_Vect(Object->ST[3]);

            EXIT
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if (Object->Patch_Type > 1)
    {
        Object->Patch_Type = 1;
        Warning("Patch type no longer supported. Using type 1.");
    }

    if (Object->Patch_Type < 0)
    {
        Error("Undefined bicubic patch type.");
    }

    Parse_Comma();

    for (i=0;i<4;i++)
    {
        for (j=0;j<4;j++)
        {
            Parse_Vector(Object->Control_Points[i][j]);
            Parse_Comma();
        }
    }

    Object->Precompute_Patch_Values(); /* interpolated mesh coords */

    Object->Compute_BBox();

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}


/*****************************************************************************
*
* FUNCTION
*
*   Parse_Blob
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
******************************************************************************/

ObjectPtr Parser::Parse_Blob()
{
    int npoints;
    DBL threshold;
    Vector3d Axis, Base, Apex;
    Blob *Object;
    Blob_List_Struct *blob_components, *blob_component;

    Parse_Begin();

    Object = reinterpret_cast<Blob *>(Parse_Object_Id());
    if (Object != nullptr)
    {
        return (reinterpret_cast<ObjectPtr>(Object));
    }

    Object = new Blob();

    blob_components = nullptr;

    npoints = 0;

    threshold = 1.0;

    EXPECT
        CASE (THRESHOLD_TOKEN)
            threshold = Parse_Float();
        END_CASE

        /*************************************************************************
         * Read sperical component (old syntax).
         *************************************************************************/

        CASE (COMPONENT_TOKEN)
            blob_component = Object->Create_Blob_List_Element();

            blob_component->elem.Type = BLOB_SPHERE;

            blob_component->elem.c[2] = Parse_Float();

            Parse_Comma();

            blob_component->elem.rad2 = Parse_Float();

            Parse_Comma();

            blob_component->elem.rad2 = Sqr(blob_component->elem.rad2);

            Parse_Vector(blob_component->elem.O);

            /* Next component. */

            blob_component->next = blob_components;

            blob_components = blob_component;

            npoints++;
        END_CASE

        /*************************************************************************
         * Read sperical component (new syntax).
         *************************************************************************/

        CASE (SPHERE_TOKEN)
            blob_component = Object->Create_Blob_List_Element();

            blob_component->elem.Type = BLOB_SPHERE;

            Parse_Begin();

            Parse_Vector(blob_component->elem.O);

            Parse_Comma();

            blob_component->elem.rad2 = Parse_Float();

            blob_component->elem.rad2 = Sqr(blob_component->elem.rad2);

            Parse_Comma();

            ALLOW(STRENGTH_TOKEN)

            blob_component->elem.c[2] = Parse_Float();

            Parse_Blob_Element_Mods(&blob_component->elem);

            /* Next component. */

            blob_component->next = blob_components;

            blob_components = blob_component;

            npoints++;
        END_CASE

        /*************************************************************************
         * Read cylindrical component.
         *************************************************************************/

        CASE (CYLINDER_TOKEN)
            blob_component = Object->Create_Blob_List_Element();

            blob_component->elem.Type = BLOB_CYLINDER;

            blob_component->elem.Trans = Create_Transform();

            Parse_Begin();

            Parse_Vector(Base);

            Parse_Comma();

            Parse_Vector(Apex);

            Parse_Comma();

            blob_component->elem.rad2 = Parse_Float();

            blob_component->elem.rad2 = Sqr(blob_component->elem.rad2);

            Parse_Comma();

            ALLOW(STRENGTH_TOKEN)

            blob_component->elem.c[2] = Parse_Float();

            /* Calculate cylinder's coordinate system. */

            Axis = Apex - Base;

            blob_component->elem.len = Axis.length();

            if (blob_component->elem.len < EPSILON)
            {
                Error("Degenerate cylindrical component in blob.");
            }

            Axis /= blob_component->elem.len;

            Compute_Coordinate_Transform(blob_component->elem.Trans, Base, Axis, 1.0, 1.0);

            Parse_Blob_Element_Mods(&blob_component->elem);

            /* Next component. */

            blob_component->next = blob_components;

            blob_components = blob_component;

            npoints++;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Object->Create_Blob_Element_Texture_List(blob_components, npoints);

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    /* The blob's texture has to be processed before Make_Blob() is called. */

    Post_Textures(Object->Texture);

    /* Finally, process the information */

    int components = Object->Make_Blob(threshold, blob_components, npoints, GetParserDataPtr());
    if (components > sceneData->Max_Blob_Components)
        sceneData->Max_Blob_Components = components;

    return (reinterpret_cast<ObjectPtr>(Object));
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Blob_Element_Mods
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
*   Sep 1994 : Creation.
*
******************************************************************************/

void Parser::Parse_Blob_Element_Mods(Blob_Element *Element)
{
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    TEXTURE *Local_Texture;

    EXPECT
        CASE (TRANSLATE_TOKEN)
            Parse_Vector (Local_Vector);
            Blob::Translate_Blob_Element (Element, Local_Vector);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (Local_Vector);
            Blob::Rotate_Blob_Element (Element, Local_Vector);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector (Local_Vector);
            Blob::Scale_Blob_Element (Element, Local_Vector);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Blob::Transform_Blob_Element (Element, Parse_Transform(&Local_Trans));
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix (Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Blob::Transform_Blob_Element (Element, &Local_Trans);
        END_CASE

        CASE (TEXTURE_TOKEN)
            Parse_Begin ();
            Local_Texture = Parse_Texture();
            Parse_End ();
            Link_Textures(&Element->Texture, Local_Texture);
        END_CASE

        CASE3 (PIGMENT_TOKEN, NORMAL_TOKEN, FINISH_TOKEN)
            if (Element->Texture == nullptr)
            {
                Element->Texture = Copy_Textures(Default_Texture);
            }
            else
            {
                if (Element->Texture->Type != PLAIN_PATTERN)
                {
                    Link_Textures(&Element->Texture, Copy_Textures(Default_Texture));
                }
            }
            UNGET
            EXPECT
                CASE (PIGMENT_TOKEN)
                    Parse_Begin ();
                    Parse_Pigment(&Element->Texture->Pigment);
                    Parse_End ();
                END_CASE

                CASE (NORMAL_TOKEN)
                    Parse_Begin ();
                    Parse_Tnormal(&Element->Texture->Tnormal);
                    Parse_End ();
                END_CASE

                CASE (FINISH_TOKEN)
                    Parse_Finish(&Element->Texture->Finish);
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End();

    /* Postprocess to make sure that HAS_FILTER will be set correctly. */

    Post_Textures(Element->Texture);
}

//******************************************************************************

ObjectPtr Parser::Parse_Box ()
{
    Box *Object;
    DBL temp;

    Parse_Begin ();

    Object = reinterpret_cast<Box *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new Box();

    Parse_Vector(Object->bounds[0]);     Parse_Comma();
    Parse_Vector(Object->bounds[1]);

    if (Object->bounds[0][X] > Object->bounds[1][X]) {
        temp = Object->bounds[0][X];
        Object->bounds[0][X] = Object->bounds[1][X];
        Object->bounds[1][X] = temp;
    }
    if (Object->bounds[0][Y] > Object->bounds[1][Y]) {
        temp = Object->bounds[0][Y];
        Object->bounds[0][Y] = Object->bounds[1][Y];
        Object->bounds[1][Y] = temp;
    }
    if (Object->bounds[0][Z] > Object->bounds[1][Z]) {
        temp = Object->bounds[0][Z];
        Object->bounds[0][Z] = Object->bounds[1][Z];
        Object->bounds[1][Z] = temp;
    }

    Object->Compute_BBox();

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}

void Parser::Parse_Mesh_Camera (Camera& Cam)
{
    Cam.Type = MESH_CAMERA;
    Parse_Begin ();

    // Get the rays per pixel
    Cam.Rays_Per_Pixel = (unsigned int) Parse_Float();
    if (Cam.Rays_Per_Pixel == 0)
        Error("Rays per pixel may not be 0");

    // Now get the distribution type.
    Cam.Face_Distribution_Method = (unsigned int) Parse_Float();
    if (Cam.Face_Distribution_Method > 3)
        Error("Unrecognized distribution method");
    if (Cam.Face_Distribution_Method == 2 || Cam.Face_Distribution_Method == 3)
        if (Cam.Rays_Per_Pixel != 1)
            Error("Rays per pixel must be 1 for distribution method #2 or #3");

    // Now the optional maximum ray length
    Cam.Max_Ray_Distance = Allow_Float(0.0);

    EXPECT
        CASE2 (MESH_TOKEN, MESH2_TOKEN)
            Cam.Meshes.push_back(Parse_Mesh());
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    ALLOW(SMOOTH_TOKEN);
    Cam.Smooth = (CurrentTrueTokenId() == SMOOTH_TOKEN);
    if (Cam.Smooth && Cam.Face_Distribution_Method != 3)
        Error("Smooth can only be used with distribution method #3");

    Parse_End ();

    if (Cam.Meshes.size() == 0)
        Expectation_Error("mesh or mesh2");
    if ((Cam.Face_Distribution_Method) == 0)
        if (Cam.Rays_Per_Pixel > Cam.Meshes.size())
            Error("Rays per pixel must be <= number of meshes for distribution method #0");

    unsigned int faces = 0;
    for (std::vector<ObjectPtr>::const_iterator it = Cam.Meshes.begin(); it != Cam.Meshes.end(); it++)
        Cam.Mesh_Index.push_back(faces += static_cast<const Mesh *>(*it)->Data->Number_Of_Triangles);

    if (Cam.Face_Distribution_Method == 3)
    {
        if (Cam.Meshes.size() > 1)
            Warning("Additional meshes after the first are ignored for distribution method #3");

        // build a 10 row and 10 column index relating faces to UV co-ordinates. each face is represented
        // by a single bit at each node of the index. to determine which faces cover a given pair of
        // co-ordinates, we just need to AND the two columns together.
        const Mesh *mesh = static_cast<const Mesh *>(Cam.Meshes[0]);
        unsigned int size = (mesh->Data->Number_Of_Triangles + 31) / 32;
        for (int i = 0; i < 10; i++) {
            Cam.U_Xref[i].resize(size);
            Cam.V_Xref[i].resize(size);
        }

        Mesh_Triangle_Struct *tr(mesh->Data->Triangles);
        for (int i = 0, idx = 0, bit = 1; i < mesh->Data->Number_Of_Triangles; i++, tr++)
        {
            int P1u(mesh->Data->UVCoords[tr->UV1][U] * 10);
            int P2u(mesh->Data->UVCoords[tr->UV2][U] * 10);
            int P3u(mesh->Data->UVCoords[tr->UV3][U] * 10);
            int P1v(mesh->Data->UVCoords[tr->UV1][V] * 10);
            int P2v(mesh->Data->UVCoords[tr->UV2][V] * 10);
            int P3v(mesh->Data->UVCoords[tr->UV3][V] * 10);

            int minU = min(min(P1u, min(P2u, P3u)), 9);
            int minV = min(min(P1v, min(P2v, P3v)), 9);
            int maxU = min(max(P1u, max(P2u, P3u)), 9);
            int maxV = min(max(P1v, max(P2v, P3v)), 9);

            for (int u = minU; u <= maxU; u++)
                Cam.U_Xref[u][idx] |= bit;
            for (int v = minV; v <= maxV; v++)
                Cam.V_Xref[v][idx] |= bit;

            if ((bit <<= 1) == 0)
            {
                idx++;
                bit = 1;
            }
        }
    }
}

void Parser::Parse_User_Defined_Camera (Camera& Cam)
{
    Cam.Type = USER_DEFINED_CAMERA;

    EXPECT
        CASE (LOCATION_TOKEN)
            if (Parse_Begin(false))
            {
                Cam.Location = Vector3d(0.0);
                Parse_FunctionOrContentList(Cam.Location_Fn, 3);
                Parse_End();
            }
            else
            {
                for(unsigned int i = 0; i < 3; ++i)
                {
                    if (Cam.Location_Fn[i] != nullptr)
                    {
                        delete Cam.Location_Fn[i];
                        Cam.Location_Fn[i] = nullptr;
                    }
                }
                Parse_Vector(Cam.Location);
            }
        END_CASE

        CASE (DIRECTION_TOKEN)
            if (Parse_Begin(false))
            {
                Cam.Direction = Vector3d(0.0);
                Parse_FunctionOrContentList(Cam.Direction_Fn, 3);
                Parse_End();
            }
            else
            {
                for(unsigned int i = 0; i < 3; ++i)
                {
                    if (Cam.Direction_Fn[i] != nullptr)
                    {
                        delete Cam.Direction_Fn[i];
                        Cam.Direction_Fn[i] = nullptr;
                    }
                }
                Parse_Vector(Cam.Direction);
            }
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT
}

//******************************************************************************

void Parser::Parse_Camera (Camera& Cam)
{
    int i;
    DBL Direction_Length = 1.0, Up_Length, Right_Length, Handedness;
    DBL k1, k2, k3;
    Vector3d tempv;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    bool only_mods = false;

    Parse_Begin ();

    if (AllowToken(CAMERA_ID_TOKEN))
    {
        Cam = CurrentTokenData<Camera>();
        if (sceneData->EffectiveLanguageVersion() >= 350)
            only_mods = true;
    }

    Camera& New = Cam;

    if ((sceneData->EffectiveLanguageVersion() >= 350) && (only_mods == true))
    {
        // keep a copy and clear it because this is a copy of a camera
        // and this will prevent that transforms are applied twice [trf]
        TRANSFORM Backup_Trans = *New.Trans;
        Destroy_Transform(New.Trans);
        New.Trans = Create_Transform();

        EXPECT
            CASE (TRANSLATE_TOKEN)
                Parse_Vector (tempv);
                Compute_Translation_Transform(&Local_Trans, tempv);
                Compose_Transforms(New.Trans, &Local_Trans);
            END_CASE

            CASE (ROTATE_TOKEN)
                Parse_Vector (tempv);
                Compute_Rotation_Transform(&Local_Trans, tempv);
                Compose_Transforms(New.Trans, &Local_Trans);
            END_CASE

            CASE (SCALE_TOKEN)
                Parse_Scale_Vector(tempv);
                Compute_Scaling_Transform(&Local_Trans, tempv);
                Compose_Transforms(New.Trans, &Local_Trans);
            END_CASE

            CASE (TRANSFORM_TOKEN)
                Parse_Transform(&Local_Trans);
                Compose_Transforms(New.Trans, &Local_Trans);
            END_CASE

            CASE (MATRIX_TOKEN)
                Parse_Matrix(Local_Matrix);
                Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
                Compose_Transforms(New.Trans, &Local_Trans);
            END_CASE

            OTHERWISE
                UNGET
                EXIT
            END_CASE
        END_EXPECT

        // apply camera transformations
        New.Transform(New.Trans);
        Compose_Transforms(&Backup_Trans, New.Trans);
    }
    else if (sceneData->EffectiveLanguageVersion() >= 350)
    {

        /*
         * The camera statement in version v3.5 is a tiny bit more restrictive
         * than in previous versions (Note: Backward compatibility is available
         * with the version switch!).  It will always apply camera modifiers in
         * the same order, regardless of the order in which they appeared in the
         * camera statement.  The order is as follows:
         *
         * right
         * direction
         * angle (depends on right, changes direction-length)
         * up
         * sky
         * location
         * look_at (depends on location, right, direction, up, sky, changes right, up, direction)
         * focal_point (depends on location)
         *
         * VERIFY: Is there a need to modify look_at to consider angle, right, up and/or direction??? [trf]
         * VERIFY: Is there a need to modify angle to consider direction??? [trf]
         */

        bool had_angle = false, had_up = false, had_right = false;
        Vector3d old_look_at, old_up, old_right, old_focal_point;
        DBL old_angle;

        old_look_at     = New.Look_At;
        New.Look_At     = Vector3d(HUGE_VAL);
        old_up          = New.Up;
        New.Up          = Vector3d(HUGE_VAL);
        old_right       = New.Right;
        New.Right       = Vector3d(HUGE_VAL);
        old_focal_point = New.Focal_Point;
        New.Focal_Point = Vector3d(HUGE_VAL);
        old_angle       = New.Angle;
        New.Angle       = HUGE_VAL;

        EXPECT
            CASE (PERSPECTIVE_TOKEN)
                New.Type = PERSPECTIVE_CAMERA;
            END_CASE

            CASE (ORTHOGRAPHIC_TOKEN)
                New.Type = ORTHOGRAPHIC_CAMERA;
            END_CASE

            CASE (FISHEYE_TOKEN)
                New.Type = FISHEYE_CAMERA;
            END_CASE

            CASE (ULTRA_WIDE_ANGLE_TOKEN)
                New.Type = ULTRA_WIDE_ANGLE_CAMERA;
            END_CASE

            CASE (OMNIMAX_TOKEN)
                New.Type = OMNIMAX_CAMERA;
            END_CASE

            CASE (PANORAMIC_TOKEN)
                New.Type = PANORAMIC_CAMERA;
            END_CASE

            CASE (SPHERICAL_TOKEN)
                New.Type = SPHERICAL_CAMERA;
            END_CASE

            CASE (CYLINDER_TOKEN)
                i = (int)Parse_Float();
                switch (i)
                {
                    case 1: New.Type = CYL_1_CAMERA; break;
                    case 2: New.Type = CYL_2_CAMERA; break;
                    case 3: New.Type = CYL_3_CAMERA; break;
                    case 4: New.Type = CYL_4_CAMERA; break;
                    default: Error("Invalid cylinder camera type, valid are types 1 to 4."); break;
                }
            END_CASE

            CASE (MESH_CAMERA_TOKEN)
                mExperimentalFlags.meshCamera = true;
                Parse_Mesh_Camera(New);
            END_CASE

            CASE (USER_DEFINED_TOKEN)
                mExperimentalFlags.userDefinedCamera = true;
                Parse_User_Defined_Camera(New);
            END_CASE

            OTHERWISE
                UNGET
                EXIT
            END_CASE
        END_EXPECT

        switch(New.Type)
        {
            case PERSPECTIVE_CAMERA:
                EXPECT
                    CASE (ANGLE_TOKEN)
                        New.Angle = Parse_Float();
                        if (New.Angle < 0.0)
                            Error("Negative viewing angle.");
                    END_CASE

                    CASE5(ORTHOGRAPHIC_TOKEN, FISHEYE_TOKEN, ULTRA_WIDE_ANGLE_TOKEN, OMNIMAX_TOKEN, PANORAMIC_TOKEN)
                    CASE4(SPHERICAL_TOKEN, CYLINDER_TOKEN, MESH_CAMERA_TOKEN, USER_DEFINED_TOKEN)
                        Expectation_Error("perspective camera modifier");
                    END_CASE

                    OTHERWISE
                        UNGET
                        if(Parse_Camera_Mods(New) == false)
                            EXIT
                    END_CASE
                END_EXPECT
                break;

            case ORTHOGRAPHIC_CAMERA:
                EXPECT
                    CASE (ANGLE_TOKEN)
                        New.Angle = Allow_Float(0.0);
                        if (New.Angle < 0.0)
                            Error("Negative viewing angle.");
                    END_CASE

                    CASE5(PERSPECTIVE_TOKEN, FISHEYE_TOKEN, ULTRA_WIDE_ANGLE_TOKEN, OMNIMAX_TOKEN, PANORAMIC_TOKEN)
                    CASE4(SPHERICAL_TOKEN, CYLINDER_TOKEN, MESH_CAMERA_TOKEN, USER_DEFINED_TOKEN)
                        Expectation_Error("orthographic camera modifier");
                    END_CASE

                    OTHERWISE
                        UNGET
                        if(Parse_Camera_Mods(New) == false)
                            EXIT
                    END_CASE
                END_EXPECT
                break;

            case FISHEYE_CAMERA:
                EXPECT
                    CASE (ANGLE_TOKEN)
                        New.Angle = Parse_Float();
                        if (New.Angle < 0.0)
                            Error("Negative viewing angle.");
                    END_CASE

                    CASE5(PERSPECTIVE_TOKEN, ORTHOGRAPHIC_TOKEN, ULTRA_WIDE_ANGLE_TOKEN, OMNIMAX_TOKEN, PANORAMIC_TOKEN)
                    CASE4(SPHERICAL_TOKEN, CYLINDER_TOKEN, MESH_CAMERA_TOKEN, USER_DEFINED_TOKEN)
                        Expectation_Error("fisheye camera modifier");
                    END_CASE

                    OTHERWISE
                        UNGET
                        if(Parse_Camera_Mods(New) == false)
                            EXIT
                    END_CASE
                END_EXPECT
                break;

            case ULTRA_WIDE_ANGLE_CAMERA:
                EXPECT
                    CASE (ANGLE_TOKEN)
                        New.Angle = Parse_Float();
                        if (New.Angle < 0.0)
                            Error("Negative viewing angle.");
                    END_CASE

                    CASE5(PERSPECTIVE_TOKEN, ORTHOGRAPHIC_TOKEN, FISHEYE_TOKEN, OMNIMAX_TOKEN, PANORAMIC_TOKEN)
                    CASE4(SPHERICAL_TOKEN, CYLINDER_TOKEN, MESH_CAMERA_TOKEN, USER_DEFINED_TOKEN)
                        Expectation_Error("ultra_wide_angle camera modifier");
                    END_CASE

                    OTHERWISE
                        UNGET
                        if(Parse_Camera_Mods(New) == false)
                            EXIT
                    END_CASE
                END_EXPECT
                break;

            case OMNIMAX_CAMERA:
                EXPECT
                    CASE (ANGLE_TOKEN)
                        New.Angle = Parse_Float();
                        if (New.Angle < 0.0)
                            Error("Negative viewing angle.");
                    END_CASE

                    CASE5(PERSPECTIVE_TOKEN, ORTHOGRAPHIC_TOKEN, FISHEYE_TOKEN, ULTRA_WIDE_ANGLE_TOKEN, PANORAMIC_TOKEN)
                    CASE4(SPHERICAL_TOKEN, CYLINDER_TOKEN, MESH_CAMERA_TOKEN, USER_DEFINED_TOKEN)
                        Expectation_Error("omnimax camera modifier");
                    END_CASE

                    OTHERWISE
                        UNGET
                        if(Parse_Camera_Mods(New) == false)
                            EXIT
                    END_CASE
                END_EXPECT
                break;

            case PANORAMIC_CAMERA:
                EXPECT
                    CASE (ANGLE_TOKEN)
                        New.Angle = Parse_Float();
                        if (New.Angle < 0.0)
                            Error("Negative viewing angle.");
                    END_CASE

                    CASE5(PERSPECTIVE_TOKEN, ORTHOGRAPHIC_TOKEN, FISHEYE_TOKEN, ULTRA_WIDE_ANGLE_TOKEN, OMNIMAX_TOKEN)
                    CASE4(SPHERICAL_TOKEN, CYLINDER_TOKEN, MESH_CAMERA_TOKEN, USER_DEFINED_TOKEN)
                        Expectation_Error("panoramic camera modifier");
                    END_CASE

                    OTHERWISE
                        UNGET
                        if(Parse_Camera_Mods(New) == false)
                            EXIT
                    END_CASE
                END_EXPECT
                break;

            case CYL_1_CAMERA:
            case CYL_2_CAMERA:
            case CYL_3_CAMERA:
            case CYL_4_CAMERA:
                EXPECT
                    CASE (ANGLE_TOKEN)
                        New.Angle = Parse_Float();
                        if (New.Angle < 0.0)
                            Error("Negative viewing angle.");
                    END_CASE

                    CASE6(PERSPECTIVE_TOKEN, ORTHOGRAPHIC_TOKEN, FISHEYE_TOKEN, ULTRA_WIDE_ANGLE_TOKEN, OMNIMAX_TOKEN, PANORAMIC_TOKEN)
                    CASE3(SPHERICAL_TOKEN, MESH_CAMERA_TOKEN, USER_DEFINED_TOKEN)
                        Expectation_Error("cylinder camera modifier");
                    END_CASE

                    OTHERWISE
                        UNGET
                        if(Parse_Camera_Mods(New) == false)
                            EXIT
                    END_CASE
                END_EXPECT
                break;

            case SPHERICAL_CAMERA:
                EXPECT
                    CASE (ANGLE_TOKEN)
                        New.H_Angle = Parse_Float();
                        if (New.H_Angle < 0.0)
                            Error("Negative horizontal angle not allowed.");
                        Parse_Comma();
                        New.V_Angle = Allow_Float(New.H_Angle * 0.5);
                        if (New.V_Angle < 0.0)
                            Error("Negative vertical angle not allowed.");
                    END_CASE

                    CASE6(PERSPECTIVE_TOKEN, ORTHOGRAPHIC_TOKEN, FISHEYE_TOKEN, ULTRA_WIDE_ANGLE_TOKEN, OMNIMAX_TOKEN, PANORAMIC_TOKEN)
                    CASE3(CYLINDER_TOKEN, MESH_CAMERA_TOKEN, USER_DEFINED_TOKEN)
                        Expectation_Error("spherical camera modifier");
                    END_CASE

                    OTHERWISE
                        UNGET
                        if(Parse_Camera_Mods(New) == false)
                            EXIT
                    END_CASE
                END_EXPECT
                break;

            case MESH_CAMERA:
                EXPECT
                    CASE6(PERSPECTIVE_TOKEN, ORTHOGRAPHIC_TOKEN, FISHEYE_TOKEN, ULTRA_WIDE_ANGLE_TOKEN, OMNIMAX_TOKEN, PANORAMIC_TOKEN)
                    CASE3(SPHERICAL_TOKEN, CYLINDER_TOKEN, USER_DEFINED_TOKEN)
                        Expectation_Error("mesh camera modifier");
                    END_CASE

                    OTHERWISE
                        UNGET
                        if(Parse_Camera_Mods(New) == false)
                            EXIT
                    END_CASE
                END_EXPECT
                break;

            case USER_DEFINED_CAMERA:
                EXPECT
                    CASE (ANGLE_TOKEN)
                    CASE6(PERSPECTIVE_TOKEN, ORTHOGRAPHIC_TOKEN, FISHEYE_TOKEN, ULTRA_WIDE_ANGLE_TOKEN, OMNIMAX_TOKEN, PANORAMIC_TOKEN)
                    CASE3(SPHERICAL_TOKEN, CYLINDER_TOKEN, MESH_CAMERA_TOKEN)
                        Expectation_Error("user-defined camera modifier");
                    END_CASE

                    OTHERWISE
                        UNGET
                        if(Parse_Camera_Mods(New) == false)
                            EXIT
                    END_CASE
                END_EXPECT
                break;
        }

        // handle "up"
        if (New.Up[X] == HUGE_VAL)
        {
            New.Up = old_up; // restore default up
        }
        else
            had_up = true;

        // handle "right"
        if (New.Right[X] == HUGE_VAL)
        {
            New.Right = old_right; // restore default right
        }
        else
            had_right = true;

        // apply "angle"
        if (New.Angle != HUGE_VAL)
        {
            if ((New.Type == PERSPECTIVE_CAMERA) || (New.Type == ORTHOGRAPHIC_CAMERA))
            {
                if (New.Angle >= 180.0)
                    Error("Viewing angle has to be smaller than 180 degrees.");

                if (New.Angle > 0.0)
                {
                    New.Direction.normalize();
                    Right_Length = New.Right.length();
                    Direction_Length = Right_Length / tan(New.Angle * M_PI_360)/2.0;
                    New.Direction *= Direction_Length;
                }
            }

            had_angle = true;
        }
        else
            New.Angle = old_angle; // restore default angle

        // apply "look_at"
        if (New.Look_At[X] != HUGE_VAL)
        {
            Direction_Length = New.Direction.length();
            Up_Length        = New.Up.length();
            Right_Length     = New.Right.length();
            tempv            = cross(New.Up, New.Direction);
            Handedness       = dot(tempv, New.Right);

            New.Direction    = New.Look_At - New.Location;

            // Check for zero length direction vector.
            if (New.Direction.lengthSqr() < EPSILON)
                Error("Camera location and look_at point must be different.");

            New.Direction.normalize();

            // Save Right vector
            tempv = New.Right;

            New.Right = cross(New.Sky, New.Direction);

            // Avoid DOMAIN error (from Terry Kanakis)
            if((fabs(New.Right[X]) < EPSILON) &&
               (fabs(New.Right[Y]) < EPSILON) &&
               (fabs(New.Right[Z]) < EPSILON))
            {
                Warning("Camera location to look_at direction and sky direction should be different.\n"
                        "Using default/supplied right vector instead.");

                // Restore Right vector
                New.Right = tempv;
            }

            New.Right.normalize();
            New.Up = cross(New.Direction, New.Right);
            New.Direction *= Direction_Length;

            if (Handedness > 0.0)
            {
                New.Right *= Right_Length;
            }
            else
            {
                New.Right *= -Right_Length;
            }

            New.Up *= Up_Length;
        }
        else
            New.Look_At = old_look_at; // restore default look_at

        // apply "orthographic"
        if (New.Type == ORTHOGRAPHIC_CAMERA)
        {
            // only if neither up nor right have been specified
            // or if angle has been specified regardless if up or right have been specified
            if (((had_up == false) && (had_right == false)) || (had_angle == true))
            {
                // resize right and up vector to get the same image
                // area as we get with the perspective camera
                tempv = New.Look_At - New.Location;
                k1 = tempv.length();
                k2 = New.Direction.length();
                if ((k1 > EPSILON) && (k2 > EPSILON))
                {
                    New.Right *= (k1 / k2);
                    New.Up    *= (k1 / k2);
                }
            }
        }

        // apply "focal_point"
        if (New.Focal_Point[X] != HUGE_VAL)
        {
            tempv = New.Focal_Point - New.Location;
            New.Focal_Distance = tempv.length();
        }
        else
            New.Focal_Point = old_focal_point; // restore default focal_point

        // apply camera transformations
        New.Transform(New.Trans);
    }
    else // old style syntax [mesh camera and user-defined camera not supported]
    {
        EXPECT
            CASE (PERSPECTIVE_TOKEN)
                New.Type = PERSPECTIVE_CAMERA;
            END_CASE

            CASE (ORTHOGRAPHIC_TOKEN)
                New.Type = ORTHOGRAPHIC_CAMERA;
                // resize right and up vector to get the same image
                // area as we get with the perspective camera
                tempv = New.Look_At - New.Location;
                k1 = tempv.length();
                k2 = New.Direction.length();
                if ((k1 > EPSILON) && (k2 > EPSILON))
                {
                    New.Right *= (k1 / k2);
                    New.Up    *= (k1 / k2);
                }
            END_CASE

            CASE (FISHEYE_TOKEN)
                New.Type = FISHEYE_CAMERA;
            END_CASE

            CASE (ULTRA_WIDE_ANGLE_TOKEN)
                New.Type = ULTRA_WIDE_ANGLE_CAMERA;
            END_CASE

            CASE (OMNIMAX_TOKEN)
                New.Type = OMNIMAX_CAMERA;
            END_CASE

            CASE (PANORAMIC_TOKEN)
                New.Type = PANORAMIC_CAMERA;
            END_CASE

            CASE2 (MESH_CAMERA_TOKEN, USER_DEFINED_TOKEN)
                Error("This camera type not supported for language version < v3.5");
            END_CASE

            CASE (CYLINDER_TOKEN)
                i = (int)Parse_Float();
                switch (i)
                {
                    case 1: New.Type = CYL_1_CAMERA; break;
                    case 2: New.Type = CYL_2_CAMERA; break;
                    case 3: New.Type = CYL_3_CAMERA; break;
                    case 4: New.Type = CYL_4_CAMERA; break;
                }
            END_CASE

            CASE (ANGLE_TOKEN)
                New.Angle = Parse_Float();

                if (New.Angle < 0.0)
                    Error("Negative viewing angle.");

                if (New.Type == PERSPECTIVE_CAMERA)
                {
                    if (New.Angle >= 180.0)
                        Error("Viewing angle has to be smaller than 180 degrees.");

                    New.Direction.normalize();
                    Right_Length = New.Right.length();
                    Direction_Length = Right_Length / tan(New.Angle * M_PI_360)/2.0;
                    New.Direction *= Direction_Length;
                }
            END_CASE

            CASE (NORMAL_TOKEN)
                Parse_Begin ();
                Parse_Tnormal(&(New.Tnormal));
                Parse_End ();
            END_CASE

            CASE (LOCATION_TOKEN)
                Parse_Vector(New.Location);
            END_CASE

            CASE (DIRECTION_TOKEN)
                Parse_Vector(New.Direction);
            END_CASE

            CASE (UP_TOKEN)
                Parse_Vector(New.Up);
            END_CASE

            CASE (RIGHT_TOKEN)
                Parse_Vector(New.Right);
            END_CASE

            CASE (SKY_TOKEN)
                Parse_Vector(New.Sky);
            END_CASE

            CASE (LOOK_AT_TOKEN)
                Direction_Length = New.Direction.length();
                Up_Length        = New.Up.length();
                Right_Length     = New.Right.length();
                tempv            = cross(New.Up, New.Direction);
                Handedness       = dot(tempv, New.Right);

                Parse_Vector (New.Look_At);
                New.Direction = New.Look_At - New.Location;

                // Check for zero length direction vector.
                if (New.Direction.lengthSqr() < EPSILON)
                    Error("Camera location and look_at point must be different.");

                New.Direction.normalize();

                // Save Right vector
                tempv = New.Right;

                New.Right = cross(New.Sky, New.Direction);

                // Avoid DOMAIN error (from Terry Kanakis)
                if((fabs(New.Right[X]) < EPSILON) &&
                   (fabs(New.Right[Y]) < EPSILON) &&
                   (fabs(New.Right[Z]) < EPSILON))
                {
                    // Restore Right vector
                    New.Right = tempv;
                }

                New.Right.normalize();
                New.Up = cross(New.Direction, New.Right);
                New.Direction *= Direction_Length;

                if (Handedness > 0.0)
                {
                    New.Right *= Right_Length;
                }
                else
                {
                    New.Right *= -Right_Length;
                }

                New.Up *= Up_Length;
            END_CASE

            CASE (TRANSLATE_TOKEN)
                Parse_Vector (tempv);
                New.Translate (tempv);
            END_CASE

            CASE (ROTATE_TOKEN)
                Parse_Vector (tempv);
                New.Rotate (tempv);
            END_CASE

            CASE (SCALE_TOKEN)
                Parse_Scale_Vector (tempv);
                New.Scale (tempv);
            END_CASE

            CASE (TRANSFORM_TOKEN)
                New.Transform(Parse_Transform(&Local_Trans));
            END_CASE

            CASE (MATRIX_TOKEN)
                Parse_Matrix (Local_Matrix);
                Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
                New.Transform(&Local_Trans);
            END_CASE

            CASE (BLUR_SAMPLES_TOKEN)
                New.Blur_Samples = Parse_Float();
                if (New.Blur_Samples <= 0)
                    Error("Illegal number of focal blur samples.");
            END_CASE

            CASE (CONFIDENCE_TOKEN)
                k1 = Parse_Float();
                if ((k1 > 0.0) && (k1 < 1.0))
                    New.Confidence = k1;
                else
                    Warning("Illegal confidence value. Default is used.");
            END_CASE

            CASE (VARIANCE_TOKEN)
                k1 = Parse_Float();
                if ((k1 >= 0.0) && (k1 <= 1.0))
                    New.Variance = k1;
                else
                    Warning("Illegal variance value. Default is used.");
            END_CASE

            CASE (APERTURE_TOKEN)
                New.Aperture = Parse_Float();
            END_CASE

            CASE (FOCAL_POINT_TOKEN)
                Parse_Vector(tempv);
                New.Focal_Point = tempv;
                tempv = New.Focal_Point - New.Location;
                New.Focal_Distance = tempv.length();
            END_CASE

            OTHERWISE
                UNGET
                EXIT
            END_CASE
        END_EXPECT
    }

    Parse_End ();

    // Make sure the focal distance hasn't been explicitly given
    if (New.Focal_Distance < 0.0)
        New.Focal_Distance = Direction_Length;
    if (New.Focal_Distance == 0.0)
        New.Focal_Distance = 1.0;

    // Print a warning message if vectors are not perpendicular. [DB 10/94]
    k1 = dot(New.Right, New.Up);
    k2 = dot(New.Right, New.Direction);
    k3 = dot(New.Up, New.Direction);

    if ((fabs(k1) > EPSILON) || (fabs(k2) > EPSILON) || (fabs(k3) > EPSILON))
    {
        Warning("Camera vectors are not perpendicular.\n"
                "Making look_at the last statement may help.");
    }
}

bool Parser::Parse_Camera_Mods(Camera& New)
{
    TRANSFORM Local_Trans;
    PIGMENT *Local_Pigment;
    MATRIX Local_Matrix;
    Vector3d tempv;
    DBL k1;

    EXPECT_ONE
        CASE (TRANSLATE_TOKEN)
            Parse_Vector (tempv);
            Compute_Translation_Transform(&Local_Trans, tempv);
            Compose_Transforms(New.Trans, &Local_Trans);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (tempv);
            Compute_Rotation_Transform(&Local_Trans, tempv);
            Compose_Transforms(New.Trans, &Local_Trans);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector(tempv);
            Compute_Scaling_Transform(&Local_Trans, tempv);
            Compose_Transforms(New.Trans, &Local_Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Parse_Transform(&Local_Trans);
            Compose_Transforms(New.Trans, &Local_Trans);
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix(Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Compose_Transforms(New.Trans, &Local_Trans);
        END_CASE

        CASE (NORMAL_TOKEN)
            Parse_Begin();
            Parse_Tnormal(&(New.Tnormal));
            Parse_End();
        END_CASE

        CASE (LOOK_AT_TOKEN)
            Parse_Vector(New.Look_At);
        END_CASE

        CASE (LOCATION_TOKEN)
            Parse_Vector(New.Location);
        END_CASE

        CASE (DIRECTION_TOKEN)
            Parse_Vector(New.Direction);
        END_CASE

        CASE (UP_TOKEN)
            Parse_Vector(New.Up);
        END_CASE

        CASE (RIGHT_TOKEN)
            Parse_Vector(New.Right);
        END_CASE

        CASE (SKY_TOKEN)
            Parse_Vector(New.Sky);
        END_CASE

        CASE (BLUR_SAMPLES_TOKEN)
            New.Blur_Samples_Min = Parse_Float();
            if (New.Blur_Samples_Min <= 0)
                Error("Illegal number of focal blur samples.");
            Parse_Comma();
            New.Blur_Samples = (int)Allow_Float(0.0);
            if (New.Blur_Samples == 0.0)
            {
                // oops, user specified no minimum blur samples
                New.Blur_Samples = New.Blur_Samples_Min;
                New.Blur_Samples_Min = 0;
            }
            else if (New.Blur_Samples_Min > New.Blur_Samples)
                Error("Focal blur samples minimum must not be larger than maximum.");
        END_CASE

        CASE (CONFIDENCE_TOKEN)
            k1 = Parse_Float();
            if ((k1 > 0.0) && (k1 < 1.0))
                New.Confidence = k1;
            else
                Warning("Illegal confidence value. Default is used.");
        END_CASE

        CASE (VARIANCE_TOKEN)
            k1 = Parse_Float();
            if ((k1 >= 0.0) && (k1 <= 1.0))
                New.Variance = k1;
            else
                Warning("Illegal variance value. Default is used.");
        END_CASE

        CASE (APERTURE_TOKEN)
            New.Aperture = Parse_Float();
        END_CASE

        CASE (FOCAL_POINT_TOKEN)
            Parse_Vector(New.Focal_Point);
        END_CASE

        CASE (BOKEH_TOKEN)
            Parse_Begin();

            EXPECT
                CASE (PIGMENT_TOKEN)
                    Local_Pigment = Copy_Pigment(Default_Texture->Pigment);
                    Parse_Begin();
                    Parse_Pigment(&Local_Pigment);
                    Parse_End();
                    Destroy_Pigment(New.Bokeh);
                    New.Bokeh = Local_Pigment;
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
            return false;
        END_CASE
    END_EXPECT

    return true;
}

//******************************************************************************

ObjectPtr Parser::Parse_CSG(int CSG_Type)
{
    CSG *Object;
    ObjectPtr Local;
    int Object_Count = 0;
    int Light_Source_Union = true;

    Parse_Begin();

    Object = reinterpret_cast<CSG *>(Parse_Object_Id());
    if(Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    if(CSG_Type & CSG_UNION_TYPE)
        Object = new CSGUnion();
    else if(CSG_Type & CSG_MERGE_TYPE)
        Object = new CSGMerge();
    else
        Object = new CSGIntersection((CSG_Type & CSG_DIFFERENCE_TYPE) != 0);

    while ((Local = Parse_Object()) != nullptr)
    {
        if((CSG_Type & CSG_INTERSECTION_TYPE) && (Local->Type & PATCH_OBJECT))
            Warning("Patch objects not allowed in intersection.");
        Object_Count++;

        if((CSG_Type & CSG_DIFFERENCE_TYPE) && (Object_Count > 1))
            // warning: Local->Invert will change the pointer if Object is CSG
            Local = Local->Invert();
        Object->Type |= (Local->Type & CHILDREN_FLAGS);
        if(!(Local->Type & LIGHT_SOURCE_OBJECT))
            Light_Source_Union = false;
        Local->Type |= IS_CHILD_OBJECT;
        Link(Local, Object->children);
    }

    if(Light_Source_Union)
        Object->Type |= LT_SRC_UNION_OBJECT;

    if(Object_Count < 2)
        VersionWarning(150, "Should have at least 2 objects in csg.");

    Object->Compute_BBox();

    // if the invert flag is in the object mods, the returned pointer will be
    // different than the passed one, though the object will still be an instance
    // of a CSG. we use dynamic_cast here to aid debugging since the overhead is small.
    Object = dynamic_cast<CSG *>(Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object)));
    POV_PARSER_ASSERT(Object != nullptr);

    if(CSG_Type & CSG_DIFFERENCE_TYPE)
        Object->Type |= CSG_DIFFERENCE_OBJECT;

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Cone ()
{
    Cone *Object;

    Parse_Begin ();

    Object = reinterpret_cast<Cone *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new Cone();

    Parse_Vector(Object->apex);  Parse_Comma ();
    Object->apex_radius = Parse_Float();  Parse_Comma ();

    Parse_Vector(Object->base);  Parse_Comma ();
    Object->base_radius = Parse_Float();

    if (AllowToken(OPEN_TOKEN))
        Clear_Flag(Object, CLOSED_FLAG);

    /* Compute run-time values for the cone */
    Object->Compute_Cone_Data();

    Object->Compute_BBox();

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Cylinder ()
{
    Cone *Object;

    Parse_Begin ();

    Object = reinterpret_cast<Cone *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new Cone();
    Object->Cylinder();

    Parse_Vector(Object->apex);  Parse_Comma();
    Parse_Vector(Object->base);  Parse_Comma();
    Object->apex_radius = Parse_Float();
    Object->base_radius = Object->apex_radius;

    if (AllowToken(OPEN_TOKEN))
        Clear_Flag(Object, CLOSED_FLAG);

    Object->Compute_Cylinder_Data();

    Object->Compute_BBox();

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Disc ()
{
    Disc *Object;
    DBL tmpf;

    Parse_Begin();

    Object = reinterpret_cast<Disc *>(Parse_Object_Id());
    if(Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new Disc();

    Parse_Vector(Object->center); Parse_Comma ();
    Parse_Vector(Object->normal); Parse_Comma ();
    Object->normal.normalize();

    tmpf = Parse_Float(); Parse_Comma ();
    Object->oradius2 = tmpf * tmpf;

    EXPECT_ONE_CAT
        CASE_FLOAT_UNGET
            tmpf = Parse_Float();
            Object->iradius2 = tmpf * tmpf;
        END_CASE

        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT

    /* Calculate info needed for ray-disc intersections */
    tmpf = dot(Object->center, Object->normal);
    Object->d = -tmpf;

    Object->Compute_Disc();

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_HField ()
{
    Vector3d Local_Vector;
    DBL Temp_Water_Level;
    HField *Object;
    ImageData *image;

    Parse_Begin ();

    Object = reinterpret_cast<HField *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new HField();

    image = Parse_Image (HF_FILE);
    image->Use = USE_NONE;

    Object->bounding_corner1 = Vector3d(0.0, 0.0, 0.0);
    Object->bounding_corner2 = Vector3d(image->width - 1.0, 65536.0, image->height - 1.0);

    Local_Vector = Vector3d(1.0) / Object->bounding_corner2;

    Compute_Scaling_Transform(Object->Trans, Local_Vector);

    EXPECT
        CASE (WATER_LEVEL_TOKEN)
            Temp_Water_Level = Parse_Float();
            if (sceneData->EffectiveLanguageVersion() < 200)
                Temp_Water_Level /=256.0;
            (reinterpret_cast<HField *>(Object))->bounding_corner1[Y] = 65536.0 * Temp_Water_Level;
        END_CASE

        CASE (SMOOTH_TOKEN)
            Set_Flag(Object, SMOOTHED_FLAG);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    Object->Compute_HField(image);

    Object->Compute_BBox();

    Destroy_Image(image);

    return (reinterpret_cast<ObjectPtr>(Object));
}



/*****************************************************************************
*
* FUNCTION    Parse_Isosurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR        R. Suzuki
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

ObjectPtr Parser::Parse_Isosurface()
{
    IsoSurface *Object;

    Parse_Begin();

    Object = reinterpret_cast<IsoSurface *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new IsoSurface();

    GET(FUNCTION_TOKEN);

    Object->Function = new FunctionVM::CustomFunction(fnVMContext->functionvm.get(), Parse_Function());

    EXPECT
        CASE(CONTAINED_BY_TOKEN)
            ParseContainedBy(Object->container, Object);
        END_CASE

        CASE(THRESHOLD_TOKEN)
            Object->threshold = Parse_Float();
        END_CASE

        CASE(ACCURACY_TOKEN)
            Object->accuracy = Parse_Float();
        END_CASE

        CASE(MAX_GRADIENT_TOKEN)
            Object->max_gradient = Parse_Float();
        END_CASE

        CASE(MAX_TRACE_TOKEN)
            Object->max_trace = Parse_Int_With_Range (1, ISOSURFACE_MAXTRACE, "isosurface max_trace");
        END_CASE

        CASE(EVALUATE_TOKEN)
            Object->eval = true;
            Object->eval_param[0] = Parse_Float();
            Parse_Comma();
            Object->eval_param[1] = Parse_Float();
            Parse_Comma();
            Object->eval_param[2] = Parse_Float();
        END_CASE

        CASE(OPEN_TOKEN)
            Object->closed = false;
        END_CASE

        CASE(ALL_INTERSECTIONS_TOKEN)
            Object->max_trace = ISOSURFACE_MAXTRACE;
        END_CASE

        CASE (POLARITY_TOKEN)
            Object->positivePolarity = (Parse_Float() > 0);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if (Object->accuracy <= 0.0)
    {
        Warning("Isosurface 'accuracy' is not positive. Using 0.001 (default).");
        Object->accuracy = 0.001;
    }
    if (Object->max_gradient <= 0.0)
    {
        Warning("Isosurface 'max_gradient' is not positive. Using 1.1 (default).");
        Object->max_gradient = 1.1;
    }

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

void Parser::ParseContainedBy(shared_ptr<pov::ContainedByShape>& container, ObjectPtr obj)
{
    DBL temp;

    Parse_Begin();

    EXPECT_ONE
        CASE(BOX_TOKEN)
            {
                shared_ptr<ContainedByBox> box(new ContainedByBox());

                Parse_Begin();

                Parse_Vector(box->corner1);
                Parse_Comma();
                Parse_Vector(box->corner2);

                Parse_End();

                if (box->corner1.x() > box->corner2.x())
                {
                    temp = box->corner1.x();
                    box->corner1.x() = box->corner2.x();
                    box->corner2.x() = temp;
                }
                if (box->corner1.y() > box->corner2.y())
                {
                    temp = box->corner1.y();
                    box->corner1.y() = box->corner2.y();
                    box->corner2.y() = temp;
                }
                if (box->corner1.z() > box->corner2.z())
                {
                    temp = box->corner1.z();
                    box->corner1.z() = box->corner2.z();
                    box->corner2.z() = temp;
                }

                container = box;

                // TODO REVIEW - where is the bounding box computed when obj->Trans is `nullptr`?
                if (obj->Trans != nullptr)
                    obj->Compute_BBox();
            }
        END_CASE

        CASE(SPHERE_TOKEN)
            {
                shared_ptr<ContainedBySphere> sphere(new ContainedBySphere());

                Parse_Begin();

                Parse_Vector(sphere->center);
                Parse_Comma();
                sphere->radius = Parse_Float();

                Parse_End();

                Make_BBox(obj->BBox,
                          sphere->center.x() - sphere->radius,
                          sphere->center.y() - sphere->radius,
                          sphere->center.z() - sphere->radius,
                          2.0 * sphere->radius,
                          2.0 * sphere->radius,
                          2.0 * sphere->radius);

                container = sphere;

                if (obj->Trans != nullptr)
                    obj->Compute_BBox();
            }
        END_CASE

        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT
    Parse_End();
}

/*****************************************************************************
*
* FUNCTION
*
*   Parse_Julia_Fractal
*
* INPUT None
*
* OUTPUT Fractal Object structure filled
*
* RETURNS
*
*   ObjectPtr  -
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Adopted to version v3.0. [DB]
*   Sept 1995 : Total rewrite for new syntax [TW]
*
******************************************************************************/

ObjectPtr Parser::Parse_Julia_Fractal ()
{
    Fractal *Object;
    DBL P;

    Parse_Begin();

    Object = reinterpret_cast<Fractal *>(Parse_Object_Id());
    if (Object != nullptr)
        return(reinterpret_cast<ObjectPtr>(Object));

    Object = new Fractal();

    Parse_Vector4D(Object->Julia_Parm);

    EXPECT

        CASE(MAX_ITERATION_TOKEN)
            Object->Num_Iterations = (int)floor(Parse_Float());

            if (Object->Num_Iterations <= 0)
            {
                Object->Num_Iterations = 1;
            }
        END_CASE

        CASE(SLICE_TOKEN)
            Parse_Vector4D(Object->Slice);
            Parse_Comma();
            Object->SliceDist = Parse_Float();

            /* normalize slice vector */
            V4D_Dot(P,Object->Slice, Object->Slice);
            if (fabs(P) < EPSILON)
            {
                Error("Slice vector is zero.");
            }
            if (fabs(Object->Slice[T]) < EPSILON)
            {
                Error("Slice t component is zero.");
            }
            P = sqrt(P);
            V4D_InverseScaleEq(Object->Slice, P);

        END_CASE

        CASE(PRECISION_TOKEN)
            P = Parse_Float();
            if ( P < 1.0 )
            {
                P = 1.0;
            }
            Object->Precision = 1.0 / P;
        END_CASE

        CASE(EXP_TOKEN)
            Object->Sub_Type = EXP_STYPE;
        END_CASE

        CASE(LN_TOKEN)
            Object->Sub_Type = LN_STYPE;
        END_CASE

        CASE(SIN_TOKEN)
            Object->Sub_Type = SIN_STYPE;
        END_CASE

        CASE(ASIN_TOKEN)
            Object->Sub_Type = ASIN_STYPE;
        END_CASE

        CASE(COS_TOKEN)
            Object->Sub_Type = COS_STYPE;
        END_CASE

        CASE(ACOS_TOKEN)
            Object->Sub_Type = ACOS_STYPE;
        END_CASE

        CASE(TAN_TOKEN)
            Object->Sub_Type = TAN_STYPE;
        END_CASE

        CASE(ATAN_TOKEN)
            Object->Sub_Type = ATAN_STYPE;
        END_CASE

        CASE(COSH_TOKEN)
            Object->Sub_Type = COSH_STYPE;
        END_CASE

        CASE(SINH_TOKEN)
            Object->Sub_Type = SINH_STYPE;
        END_CASE

        CASE(TANH_TOKEN)
            Object->Sub_Type = TANH_STYPE;
        END_CASE

        CASE(ATANH_TOKEN)
            Object->Sub_Type = ATANH_STYPE;
        END_CASE

        CASE(ACOSH_TOKEN)
            Object->Sub_Type = ACOSH_STYPE;
        END_CASE

        CASE(ASINH_TOKEN)
            Object->Sub_Type = ASINH_STYPE;
        END_CASE

        CASE(SQR_TOKEN)
            Object->Sub_Type = SQR_STYPE;
        END_CASE

        CASE(PWR_TOKEN)
            Object->Sub_Type = PWR_STYPE;
            Parse_Float_Param2(&Object->exponent.x,&Object->exponent.y);
        END_CASE

        CASE(CUBE_TOKEN)
            Object->Sub_Type = CUBE_STYPE;
        END_CASE

        CASE(RECIPROCAL_TOKEN)
            Object->Sub_Type = RECIPROCAL_STYPE;
        END_CASE

        CASE(HYPERCOMPLEX_TOKEN)
            Object->Algebra = HYPERCOMPLEX_TYPE;
        END_CASE

        CASE(QUATERNION_TOKEN)
            Object->Algebra = QUATERNION_TYPE;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE

    END_EXPECT

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    int num_iterations = Object->SetUp_Fractal();
    if (num_iterations > sceneData->Fractal_Iteration_Stack_Length)
    {
        sceneData->Fractal_Iteration_Stack_Length = num_iterations;
        TraceThreadData *td = GetParserDataPtr();
        Fractal::Allocate_Iteration_Stack(td->Fractal_IStack, sceneData->Fractal_Iteration_Stack_Length);
    }

    return(reinterpret_cast<ObjectPtr>(Object));
}
/*****************************************************************************
*
* FUNCTION
*
*   Parse_Lathe
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   ObjectPtr  -
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Read a lathe primitive.
*
* CHANGES
*
*   Jun 1994 : Creation.
*
******************************************************************************/

ObjectPtr Parser::Parse_Lathe()
{
    bool AlreadyWarned;
    int i;
    Lathe *Object;
    Vector2d *Points;

    Parse_Begin();

    Object = reinterpret_cast<Lathe *>(Parse_Object_Id());
    if(Object != nullptr)
        return(reinterpret_cast<ObjectPtr>(Object));

    Object = new Lathe();

    /* Determine kind of spline used and aspect ratio. */

    EXPECT
        CASE(LINEAR_SPLINE_TOKEN)
            Object->Spline_Type = LINEAR_SPLINE;
        END_CASE

        CASE(QUADRATIC_SPLINE_TOKEN)
            Object->Spline_Type = QUADRATIC_SPLINE;
        END_CASE

        CASE(CUBIC_SPLINE_TOKEN)
            Object->Spline_Type = CUBIC_SPLINE;
        END_CASE

        CASE(BEZIER_SPLINE_TOKEN)
            Object->Spline_Type = BEZIER_SPLINE;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    /* Get number of points. */

    Object->Number = (int)Parse_Float();

    switch (Object->Spline_Type)
    {
        case LINEAR_SPLINE :

            if (Object->Number < 2)
            {
                Error("Lathe with linear splines must have at least two points.");
            }

            break;

        case QUADRATIC_SPLINE :

            if (Object->Number < 3)
            {
                Error("Lathe with quadratic splines must have at least three points.");
            }

            break;

        case CUBIC_SPLINE :

            if (Object->Number < 4)
            {
                Error("Prism with cubic splines must have at least four points.");
            }

            break;

        case BEZIER_SPLINE :

            if ((Object->Number & 3) != 0)
            {
                Error("Lathe with Bezier splines must have four points per segment.");
            }

            break;
    }

    /* Get temporary points describing the rotated curve. */

    Points = reinterpret_cast<Vector2d *>(POV_MALLOC(Object->Number*sizeof(Vector2d), "temporary lathe points"));

    /* Read points (x : radius; y : height; z : not used). */

    Parse_Comma();

    AlreadyWarned = false;
    for (i = 0; i < Object->Number; i++)
    {
        Parse_UV_Vect(Points[i]);

        switch (Object->Spline_Type)
        {
           case LINEAR_SPLINE :

              if (Points[i][X] < 0.0)
              {
                 if ((sceneData->EffectiveLanguageVersion() < 380) && ((i == 0) || (i == Object->Number - 1)))
                     Warning("Lathe with linear spline has a first or last point with an x value < 0.0.\n"
                             "Leads to artifacts, and would be considered an error in v3.8 and later scenes.");
                 else
                     Error("Lathe with linear spline has a point with an x value < 0.0.");
              }

              break;

           case QUADRATIC_SPLINE :

              if ((i > 0) && (Points[i][X] < 0.0))
              {
                 if ((sceneData->EffectiveLanguageVersion() < 380) && (i == Object->Number - 1))
                     Warning("Lathe with quadratic spline has last point with an x value < 0.0.\n"
                             "Leads to artifacts, and would be considered an error in v3.8 and later scenes.");
                 else
                     Error("Lathe with quadratic spline has a point with an x value < 0.0.");
              }

              break;

           case CUBIC_SPLINE :

              if ((i > 0) && (i < Object->Number - 1) && (Points[i][X] < 0.0))
              {
                 Error("Lathe with cubic spline has a point with an x value < 0.0.");
              }

              break;

           case BEZIER_SPLINE :

              if (((i%4 == 0) || (i%4 == 3)) && (Points[i][X] < 0.0))
              {
                 if ((sceneData->EffectiveLanguageVersion() < 380) && ((i == 0) || (i == Object->Number - 1)))
                     Warning("Lathe with Bezier spline has a first or last point with an x value < 0.0.\n"
                             "Leads to artifacts, and would be considered an error in v3.8 and later scenes.");
                 else
                     Error("Lathe with Bezier spline has a point with an x value < 0.0.");
              }
              else if (!AlreadyWarned && (i%4 != 0) && (i%4 != 3) && (Points[i][X] < 0.0))
              {
                 AlreadyWarned = true;
                 Warning("Lathe with negative Bezier spline control point potentially\n"
                         "an issue for normal calculation and shape bounding.");
              }

              break;
        }

        // NB we allow for a trailing comma at the end of the list,
        // to facilitate auto-generation of lists.
        Parse_Comma();
    }

    /* Compute spline segments. */

    Object->Compute_Lathe(Points, GetParserDataPtr()->Stats());

    /* Compute bounding box. */

    Object->Compute_BBox();

    /* Parse object's modifiers. */

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    /* Destroy temporary points. */

    POV_FREE(Points);

    if (Object->Spline->BCyl->number > sceneData->Max_Bounding_Cylinders)
    {
        TraceThreadData *td = GetParserDataPtr();
        sceneData->Max_Bounding_Cylinders = Object->Spline->BCyl->number;
        td->BCyl_Intervals.reserve(4*sceneData->Max_Bounding_Cylinders);
        td->BCyl_RInt.reserve(2*sceneData->Max_Bounding_Cylinders);
        td->BCyl_HInt.reserve(2*sceneData->Max_Bounding_Cylinders);
    }

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Lemon ()
{
    Lemon *Object;
    ObjectPtr ptr;

    Parse_Begin ();

    Object = reinterpret_cast<Lemon *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new Lemon();

    Parse_Vector(Object->apex);  Parse_Comma ();
    Object->apex_radius = Parse_Float();  Parse_Comma ();

    Parse_Vector(Object->base);  Parse_Comma ();
    Object->base_radius = Parse_Float();

    Parse_Comma ();

    Object->inner_radius = Parse_Float();

    if ((Object->apex_radius < 0)||(Object->base_radius < 0)||(Object->inner_radius < 0))
    {
        Error("All radii must be positive");
    }

    if (AllowToken(OPEN_TOKEN))
        Clear_Flag(Object, CLOSED_FLAG);

    /* Compute run-time values for the lemon */
    Object->Compute_Lemon_Data(mMessageFactory, CurrentTokenMessageContext());

    Object->Compute_BBox();
    ptr = reinterpret_cast<ObjectPtr>(Object);

    Parse_Object_Mods(ptr);

    return (ptr);
}


/*****************************************************************************
*
* FUNCTION
*
*   Parse_Light_Group
*
* INPUT
*
*   -
*
* OUTPUT
*
* RETURNS
*
*   Light group object
*
* AUTHOR
*
*   Thorsten Froehlich [trf]
*
* DESCRIPTION
*
*   Parse light_group object
*
* CHANGES
*
*   Jun 2000 : Creation.
*
******************************************************************************/

ObjectPtr Parser::Parse_Light_Group()
{
    CSG *Object;
    ObjectPtr Local;
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;

    Parse_Begin();

    Object = new CSGUnion();

    Object->Type |= LIGHT_GROUP_OBJECT;
    Set_Flag(Object, NO_GLOBAL_LIGHTS_FLAG);

    while ((Local = Parse_Object()) != nullptr)
    {
        // prevent light sources from being added to Frame.Light_Sources
        if((Local->Type & LIGHT_SOURCE_OBJECT) == LIGHT_SOURCE_OBJECT)
            Local->Type |= LIGHT_GROUP_LIGHT_OBJECT;
        Local->Type |= IS_CHILD_OBJECT;
        Link(Local, Object->children);
    }

    Promote_Local_Lights(Object); // in core/lighting/lightgroup.cpp [trf]

    Object->Compute_BBox();

    // Note: We cannot use Parse_Object_Mods here because
    // it would allow all kinds of modifiers. However,
    // changing it to not allow those would slow it down,
    // so the bits of code needed are just duplicated
    // here. [trf]
    // Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    EXPECT
        CASE (TRANSLATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Translation_Transform(&Local_Trans, Local_Vector);
            Translate_Object (reinterpret_cast<ObjectPtr>(Object), Local_Vector, &Local_Trans);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Rotation_Transform(&Local_Trans, Local_Vector);
            Rotate_Object (reinterpret_cast<ObjectPtr>(Object), Local_Vector, &Local_Trans);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector (Local_Vector);
            Compute_Scaling_Transform(&Local_Trans, Local_Vector);
            Scale_Object (reinterpret_cast<ObjectPtr>(Object), Local_Vector, &Local_Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Transform_Object (reinterpret_cast<ObjectPtr>(Object), Parse_Transform(&Local_Trans));
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix (Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Transform_Object (reinterpret_cast<ObjectPtr>(Object), &Local_Trans);
        END_CASE

        CASE (GLOBAL_LIGHTS_TOKEN)
            Bool_Flag (Object, NO_GLOBAL_LIGHTS_FLAG, !(Allow_Float(1.0) > 0.5));
            // TODO FIXME -- shouldn't we set NO_GLOBAL_LIGHTS_SET_FLAG here?
        END_CASE

        CASE(PHOTONS_TOKEN)
            Parse_Begin();
            EXPECT
                CASE(TARGET_TOKEN)
                    Object->Ph_Density = Allow_Float(1.0);
                    if (Object->Ph_Density > 0)
                    {
                        Set_Flag(Object,PH_TARGET_FLAG);
                        CheckPassThru(reinterpret_cast<ObjectPtr>(Object), PH_TARGET_FLAG);
                    }
                    else
                    {
                        Clear_Flag(Object, PH_TARGET_FLAG);
                    }
                END_CASE

                CASE(REFRACTION_TOKEN)
                    if((int)Parse_Float())
                    {
                        Set_Flag(Object, PH_RFR_ON_FLAG);
                        Clear_Flag(Object, PH_RFR_OFF_FLAG);
                        CheckPassThru(reinterpret_cast<ObjectPtr>(Object), PH_RFR_ON_FLAG);
                    }
                    else
                    {
                        Clear_Flag(Object, PH_RFR_ON_FLAG);
                        Set_Flag(Object, PH_RFR_OFF_FLAG);
                    }
                END_CASE

                CASE(REFLECTION_TOKEN)
                    if((int)Parse_Float())
                    {
                        Set_Flag(Object, PH_RFL_ON_FLAG);
                        Clear_Flag(Object, PH_RFL_OFF_FLAG);
                    }
                    else
                    {
                        Clear_Flag(Object, PH_RFL_ON_FLAG);
                        Set_Flag(Object, PH_RFL_OFF_FLAG);
                    }
                END_CASE

                CASE(PASS_THROUGH_TOKEN)
                    if((int)Allow_Float(1.0))
                    {
                        Set_Flag(Object, PH_PASSTHRU_FLAG);
                        CheckPassThru(reinterpret_cast<ObjectPtr>(Object), PH_PASSTHRU_FLAG);
                    }
                    else
                    {
                        Clear_Flag(Object, PH_PASSTHRU_FLAG);
                    }
                END_CASE

                CASE(COLLECT_TOKEN)
                    Bool_Flag (Object, PH_IGNORE_PHOTONS_FLAG, !(Allow_Float(1.0) > 0.0));
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
            EXIT
        END_CASE
    END_EXPECT

    Set_CSG_Children_Flag(Object, Test_Flag(Object, NO_GLOBAL_LIGHTS_FLAG),
                          NO_GLOBAL_LIGHTS_FLAG, NO_GLOBAL_LIGHTS_SET_FLAG);

    Parse_End();

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Light_Source ()
{
    DBL Len;
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    LightSource *Object;
    /* NK ---- */

    Parse_Begin ();

    Object = reinterpret_cast<LightSource *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new LightSource ();

    Parse_Vector(Object->Center);

    Parse_Comma();

    Parse_Colour(Object->colour);

    EXPECT
        /* NK phmap */
        CASE (COLOUR_MAP_TOKEN)
            // TODO - apparently this undocumented syntax was once intended to do something related to dispersion,
            //        but is currently dysfunctional, doing nothing except provide an undocumented means of averaging
            //        different colours. Can we safely drop it?
            Warning("Undocumented syntax ignored (colour_map in light_source);"
                    " future versions of POV-Ray may drop support for it entirely.");
            (void)Parse_Colour_Map<ColourBlendMap> ();
        END_CASE

        CASE(PHOTONS_TOKEN)
            Parse_Begin();
            EXPECT
#ifdef GLOBAL_PHOTONS
                // TODO -- if we ever revive this, we need to choose a different keyword for this
                CASE(GLOBAL_TOKEN)
                    Object->Ph_Density = Allow_Float(1.0);
                    if (Object->Ph_Density > 0)
                    {
                        Set_Flag(Object, PH_TARGET_FLAG);
                        /*CheckPassThru(Object, PH_TARGET_FLAG);*/
                    }
                    else
                    {
                        Clear_Flag(Object, PH_TARGET_FLAG);
                    }
                END_CASE
#endif

                CASE(REFRACTION_TOKEN)
                    if((int)Parse_Float())
                    {
                        Set_Flag(Object, PH_RFR_ON_FLAG);
                        Clear_Flag(Object, PH_RFR_OFF_FLAG);
                    }
                    else
                    {
                        Clear_Flag(Object, PH_RFR_ON_FLAG);
                        Set_Flag(Object, PH_RFR_OFF_FLAG);
                    }
                END_CASE

                CASE(REFLECTION_TOKEN)
                    if((int)Parse_Float())
                    {
                        Set_Flag(Object, PH_RFL_ON_FLAG);
                        Clear_Flag(Object, PH_RFL_OFF_FLAG);
                    }
                    else
                    {
                        Clear_Flag(Object, PH_RFL_ON_FLAG);
                        Set_Flag(Object, PH_RFL_OFF_FLAG);
                    }
                END_CASE

                CASE (AREA_LIGHT_TOKEN)
                    Object->Photon_Area_Light = true;
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            Parse_End();
        END_CASE

        CASE (LOOKS_LIKE_TOKEN)
            if (!Object->children.empty())
                Error("Only one looks_like allowed per light_source.");
            Parse_Begin ();
            Object->Type &= ~(int)PATCH_OBJECT;
            Object->children.push_back(Parse_Object());
            if (Object->children.empty() || (Object->children[0] == nullptr))
                Expectation_Error("object");
            Compute_Translation_Transform(&Local_Trans, Object->Center);
            Translate_Object(Object->children[0], Object->Center, &Local_Trans);
            Object->children[0] = Parse_Object_Mods (Object->children[0]);
            Set_Flag(Object->children[0], NO_SHADOW_FLAG);
            Set_Flag(Object, NO_SHADOW_FLAG);
            Object->Type |= (Object->children[0]->Type & CHILDREN_FLAGS);
            Set_Flag(Object, PH_PASSTHRU_FLAG);
        END_CASE

        CASE (PROJECTED_THROUGH_TOKEN)
            if (Object->Projected_Through_Object != nullptr)
                Error("Only one projected through allowed per light_source.");
            Parse_Begin ();
            Object->Type &= ~(int)PATCH_OBJECT;
            if ((Object->Projected_Through_Object = Parse_Object ()) == nullptr)
                Expectation_Error ("object");
            Object->Projected_Through_Object = Parse_Object_Mods (Object->Projected_Through_Object);
            Set_Flag(Object, NO_SHADOW_FLAG);
            Set_Flag(Object, PH_PASSTHRU_FLAG);
        END_CASE

        CASE (SHADOWLESS_TOKEN)
            Object->Light_Type = FILL_LIGHT_SOURCE;
        END_CASE

        CASE (PARALLEL_TOKEN)
            Object->Parallel= true;
        END_CASE

        CASE (SPOTLIGHT_TOKEN)
            Object->Light_Type = SPOT_SOURCE;
            Object->Radius = cos(30 * M_PI_180);
            Object->Falloff = cos(45 * M_PI_180);
            Object->Coeff = 0;
        END_CASE

        CASE (CYLINDER_TOKEN)
            Object->Light_Type = CYLINDER_SOURCE;
            Object->Radius = 0.75;
            Object->Falloff = 1;
            Object->Coeff = 0;
            Object->Parallel = true;
        END_CASE

        CASE (POINT_AT_TOKEN)
            if ((Object->Light_Type == SPOT_SOURCE) || (Object->Light_Type == CYLINDER_SOURCE) ||
                Object->Parallel)
            {
                Parse_Vector(Object->Points_At);
            }
            else
            {
                Not_With ("point_at","standard light source");
            }
        END_CASE

        CASE (TIGHTNESS_TOKEN)
            if ((Object->Light_Type == SPOT_SOURCE) || (Object->Light_Type == CYLINDER_SOURCE))
                Object->Coeff = Parse_Float();
            else
                Not_With ("tightness","standard light source");
        END_CASE

        CASE (RADIUS_TOKEN)
            if ((Object->Light_Type == SPOT_SOURCE) || (Object->Light_Type == CYLINDER_SOURCE))
            {
                Object->Radius = Parse_Float();
                if (Object->Light_Type == SPOT_SOURCE)
                {
                    Object->Radius  = cos(Object->Radius * M_PI_180);
                }
            }
            else
                Not_With ("radius","standard light source");
        END_CASE

        CASE (FALLOFF_TOKEN)
            if ((Object->Light_Type == SPOT_SOURCE) || (Object->Light_Type == CYLINDER_SOURCE))
            {
                Object->Falloff = Parse_Float();
                if (Object->Light_Type == SPOT_SOURCE)
                {
                    Object->Falloff = cos(Object->Falloff * M_PI_180);
                }
            }
            else
                Not_With ("falloff","standard light source");
        END_CASE

        CASE (FADE_DISTANCE_TOKEN)
            Object->Fade_Distance = Parse_Float();
        END_CASE

        CASE (FADE_POWER_TOKEN)
            Object->Fade_Power = Parse_Float();
        END_CASE

        CASE (AREA_LIGHT_TOKEN)
            Object->Area_Light = true;
            Parse_Vector (Object->Axis1); Parse_Comma ();
            Parse_Vector (Object->Axis2); Parse_Comma ();
            Object->Area_Size1 = (int)Parse_Float();
            if (Object->Area_Size1 == 0)
                Error("Area size must be greater than zero.");
            Parse_Comma ();
            Object->Area_Size2 = (int)Parse_Float();
            if (Object->Area_Size2 == 0)
                Error("Area size must be greater than zero.");
        END_CASE

        CASE (JITTER_TOKEN)
            Object->Jitter = true;
        END_CASE

        /* Orient area lights to the point [ENB 9/97] */
        CASE (ORIENT_TOKEN)
            Object->Orient = true;
            if (!(Object->Area_Light))
            {
                Warning("Orient only affects area_light");
            }
        END_CASE

        /* Circular area lights [ENB 9/97] */
        CASE (CIRCULAR_TOKEN)
            Object->Circular = true;
            if (!(Object->Area_Light))
            {
                Warning("Circular only affects area_light");
            }
        END_CASE

        // JN2007: Full area lighting:
        CASE (AREA_ILLUMINATION_TOKEN)
            Object->Use_Full_Area_Lighting = Allow_Float(1.0) > 0.0;
            if (!(Object->Area_Light))
            {
                Warning("Area_illumination only affects area_light");
            }
        END_CASE

        CASE (ADAPTIVE_TOKEN)
            Object->Adaptive_Level = (int)Parse_Float();
        END_CASE

        CASE (MEDIA_ATTENUATION_TOKEN)
            Object->Media_Attenuation = Allow_Float(1.0) > 0.0;
        END_CASE

        CASE (MEDIA_INTERACTION_TOKEN)
            Object->Media_Interaction = Allow_Float(1.0) > 0.0;
        END_CASE

        CASE (TRANSLATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Translation_Transform(&Local_Trans, Local_Vector);
            Translate_Object (reinterpret_cast<ObjectPtr>(Object), Local_Vector, &Local_Trans);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Rotation_Transform(&Local_Trans, Local_Vector);
            Rotate_Object (reinterpret_cast<ObjectPtr>(Object), Local_Vector, &Local_Trans);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector (Local_Vector);
            Compute_Scaling_Transform(&Local_Trans, Local_Vector);
            Scale_Object (reinterpret_cast<ObjectPtr>(Object), Local_Vector, &Local_Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Transform_Object (reinterpret_cast<ObjectPtr>(Object), Parse_Transform(&Local_Trans));
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix (Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Transform_Object (reinterpret_cast<ObjectPtr>(Object), &Local_Trans);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if ((Object->Fade_Power != 0) && (fabs(Object->Fade_Distance) < EPSILON) && (sceneData->EffectiveLanguageVersion() < 380))
    {
        Warning("fade_power with fade_distance 0 is not supported in legacy (pre-v3.8) scenes; fade_power is ignored.");
        Object->Fade_Power    = 0;
        Object->Fade_Distance = 0;
    }

    Parse_End ();


    Object->Direction = Object->Points_At - Object->Center;

    Len = Object->Direction.length();

    if (Len > EPSILON)
    {
        Object->Direction /= Len;
    }

    /* Make sure that circular light sources are larger than 1 by x [ENB 9/97] */
    if (Object->Circular)
    {
        if ((Object->Area_Size1 <= 1) || (Object->Area_Size2 <= 1))
        {
            Error("Circular area light must have more than 1 point per axis");
        }
    }

    return (reinterpret_cast<ObjectPtr>(Object));
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Mesh
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   OBJECT
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Read a triangle mesh.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

ObjectPtr Parser::Parse_Mesh()
{
    Mesh *Object;

    Parse_Begin();

    Object = reinterpret_cast<Mesh *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    /* Create object. */

    Object = new Mesh();

#if POV_PARSER_EXPERIMENTAL_OBJ_IMPORT

    EXPECT_ONE

        CASE4 (VERTEX_VECTORS_TOKEN, NORMAL_VECTORS_TOKEN, UV_VECTORS_TOKEN, TEXTURE_LIST_TOKEN)
        // the following are currently not expected by mesh2 right away, but that may change in the future
        CASE3 (FACE_INDICES_TOKEN, UV_INDICES_TOKEN, NORMAL_INDICES_TOKEN)
            UNGET
            Parse_Mesh2 (Object);
        END_CASE

        CASE (OBJ_TOKEN)
            mExperimentalFlags.objImport = true;
            Parse_Obj (Object);
        END_CASE

        OTHERWISE
            UNGET
            Parse_Mesh1 (Object);
        END_CASE

    END_EXPECT

#else

    Parse_Mesh1 (Object);

#endif

    // Create bounding box.

    Object->Compute_BBox();

    // Parse object modifiers.

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    // Create bounding box tree.

    Object->Build_Mesh_BBox_Tree();

    return Object;
}

void Parser::Parse_Mesh1 (Mesh* Object)
{
    /* NK 1998 - added all sorts of uv variables*/
    int i;
    int number_of_normals, number_of_textures, number_of_triangles, number_of_vertices, number_of_uvcoords;
    int max_normals, max_textures, max_triangles, max_vertices, max_uvcoords;
    DBL l1, l2, l3;
    Vector3d D1, D2, P1, P2, P3, N1, N2, N3, N;
    Vector2d UV1, UV2, UV3;
    MeshVector *Normals, *Vertices;
    TEXTURE **Textures;
    MeshUVVector *UVCoords;
    MESH_TRIANGLE *Triangles;
    bool fully_textured=true;
    /* NK 1998 */
    Vector3d Inside_Vect;
    TEXTURE *t2, *t3;
    bool foundZeroNormal=false;

    Inside_Vect = Vector3d(0.0, 0.0, 0.0);

    /* Allocate temporary normals, textures, triangles and vertices. */

    max_normals = 256;

    max_vertices = 256;

    max_textures = 16;

    max_triangles = 256;

    Normals = reinterpret_cast<MeshVector *>(POV_MALLOC(max_normals*sizeof(MeshVector), "temporary triangle mesh data"));

    Textures = reinterpret_cast<TEXTURE **>(POV_MALLOC(max_textures*sizeof(TEXTURE *), "temporary triangle mesh data"));

    Triangles = reinterpret_cast<MESH_TRIANGLE *>(POV_MALLOC(max_triangles*sizeof(MESH_TRIANGLE), "temporary triangle mesh data"));

    Vertices = reinterpret_cast<MeshVector *>(POV_MALLOC(max_vertices*sizeof(MeshVector), "temporary triangle mesh data"));

    /* Read raw triangle file. */

    number_of_normals = 0;

    number_of_textures = 0;

    number_of_triangles = 0;

    number_of_vertices = 0;

    max_uvcoords = 256;
    UVCoords = reinterpret_cast<MeshUVVector *>(POV_MALLOC(max_uvcoords*sizeof(MeshUVVector), "temporary triangle mesh data"));
    number_of_uvcoords = 0;

    /* Create hash tables. */

    Object->Create_Mesh_Hash_Tables();

    EXPECT
        CASE(TRIANGLE_TOKEN)
            Parse_Begin();

            Parse_Vector(P1);  Parse_Comma();
            Parse_Vector(P2);  Parse_Comma();
            Parse_Vector(P3);

            if (!Object->Degenerate(P1, P2, P3))
            {
                if (number_of_triangles >= max_triangles)
                {
                    if (max_triangles >= std::numeric_limits<int>::max()/2)
                    {
                        Error("Too many triangles in triangle mesh.");
                    }

                    max_triangles *= 2;

                    Triangles = reinterpret_cast<MESH_TRIANGLE *>(POV_REALLOC(Triangles, max_triangles*sizeof(MESH_TRIANGLE), "triangle triangle mesh data"));
                }

                /* Init triangle. */

                Object->Init_Mesh_Triangle(&Triangles[number_of_triangles]);

                Triangles[number_of_triangles].P1 = Object->Mesh_Hash_Vertex(&number_of_vertices, &max_vertices, &Vertices, P1);
                Triangles[number_of_triangles].P2 = Object->Mesh_Hash_Vertex(&number_of_vertices, &max_vertices, &Vertices, P2);
                Triangles[number_of_triangles].P3 = Object->Mesh_Hash_Vertex(&number_of_vertices, &max_vertices, &Vertices, P3);

                /* NK 1998 */
                (void)Parse_Three_UVCoords(UV1,UV2,UV3);
                Triangles[number_of_triangles].UV1 = Object->Mesh_Hash_UV(&number_of_uvcoords, &max_uvcoords, &UVCoords, UV1);
                Triangles[number_of_triangles].UV2 = Object->Mesh_Hash_UV(&number_of_uvcoords, &max_uvcoords, &UVCoords, UV2);
                Triangles[number_of_triangles].UV3 = Object->Mesh_Hash_UV(&number_of_uvcoords, &max_uvcoords, &UVCoords, UV3);
                /* NK ---- */

                /* NK */
                /* read possibly three instead of only one texture */
                /* read these before compute!!! */
                t2 = t3 = nullptr;
                Triangles[number_of_triangles].Texture = Object->Mesh_Hash_Texture(&number_of_textures, &max_textures, &Textures, Parse_Mesh_Texture(&t2,&t3));
                if (t2) Triangles[number_of_triangles].Texture2 = Object->Mesh_Hash_Texture(&number_of_textures, &max_textures, &Textures, t2);
                if (t3) Triangles[number_of_triangles].Texture3 = Object->Mesh_Hash_Texture(&number_of_textures, &max_textures, &Textures, t3);
                if (t2 || t3) Triangles[number_of_triangles].ThreeTex = true;

                Object->Compute_Mesh_Triangle(&Triangles[number_of_triangles], false, P1, P2, P3, N);

                Triangles[number_of_triangles].Normal_Ind = Object->Mesh_Hash_Normal(&number_of_normals, &max_normals, &Normals, N);

                if(Triangles[number_of_triangles].Texture < 0)
                    fully_textured = false;

                number_of_triangles++;
            }
            /* NK degenerate fix */
            else
            {
                /* parse the uv and texture info - even though we'll just throw it
                   away.  why?  if not we get a parse error - we should just ignore the
                   degenerate triangle */
                t2 = t3 = nullptr;
                (void)Parse_Three_UVCoords(UV1,UV2,UV3);
                Parse_Mesh_Texture(&t2,&t3);
            }

            Parse_End();
        END_CASE

        CASE(SMOOTH_TRIANGLE_TOKEN)
            Parse_Begin();

            Parse_Vector(P1);  Parse_Comma();
            Parse_Vector(N1);  Parse_Comma();
            if(fabs(N1[X])<EPSILON && fabs(N1[Y])<EPSILON && fabs(N1[Z])<EPSILON)
            {
                N1[X] = 1.0;  // make it nonzero
                if(!foundZeroNormal)
                    Warning("Normal vector in mesh cannot be zero - changing it to <1,0,0>.");
                foundZeroNormal = true;
            }

            Parse_Vector(P2);  Parse_Comma();
            Parse_Vector(N2);  Parse_Comma();
            if(fabs(N2[X])<EPSILON && fabs(N2[Y])<EPSILON && fabs(N2[Z])<EPSILON)
            {
                N2[X] = 1.0;  // make it nonzero
                if(!foundZeroNormal)
                    Warning("Normal vector in mesh cannot be zero - changing it to <1,0,0>.");
                foundZeroNormal = true;
            }

            Parse_Vector(P3);  Parse_Comma();
            Parse_Vector(N3);
            if(fabs(N3[X])<EPSILON && fabs(N3[Y])<EPSILON && fabs(N3[Z])<EPSILON)
            {
                N3[X] = 1.0;  // make it nonzero
                if(!foundZeroNormal)
                    Warning("Normal vector in mesh cannot be zero - changing it to <1,0,0>.");
                foundZeroNormal = true;
            }

            l1 = N1.length();
            l2 = N2.length();
            l3 = N3.length();

            if ((l1 != 0.0) && (l2 != 0.0) && (l3 != 0.0) && (!Object->Degenerate(P1, P2, P3)))
            {
                if (number_of_triangles >= max_triangles)
                {
                    if (max_triangles >= std::numeric_limits<int>::max()/2)
                        Error("Too many triangles in triangle mesh.");

                    max_triangles *= 2;

                    Triangles = reinterpret_cast<MESH_TRIANGLE *>(POV_REALLOC(Triangles, max_triangles*sizeof(MESH_TRIANGLE), "triangle triangle mesh data"));
                }

                N1 /= l1;
                N2 /= l2;
                N3 /= l3;

                /* Init triangle. */

                Object->Init_Mesh_Triangle(&Triangles[number_of_triangles]);

                Triangles[number_of_triangles].P1 = Object->Mesh_Hash_Vertex(&number_of_vertices, &max_vertices, &Vertices, P1);
                Triangles[number_of_triangles].P2 = Object->Mesh_Hash_Vertex(&number_of_vertices, &max_vertices, &Vertices, P2);
                Triangles[number_of_triangles].P3 = Object->Mesh_Hash_Vertex(&number_of_vertices, &max_vertices, &Vertices, P3);

                /* Check for equal normals. */

                D1 = N1 - N2;
                D2 = N1 - N3;

                l1 = D1.lengthSqr();
                l2 = D2.lengthSqr();

                /* NK 1998 */
                (void)Parse_Three_UVCoords(UV1,UV2,UV3);
                Triangles[number_of_triangles].UV1 = Object->Mesh_Hash_UV(&number_of_uvcoords, &max_uvcoords, &UVCoords, UV1);
                Triangles[number_of_triangles].UV2 = Object->Mesh_Hash_UV(&number_of_uvcoords, &max_uvcoords, &UVCoords, UV2);
                Triangles[number_of_triangles].UV3 = Object->Mesh_Hash_UV(&number_of_uvcoords, &max_uvcoords, &UVCoords, UV3);

                /* read possibly three instead of only one texture */
                /* read these before compute!!! */
                t2 = t3 = nullptr;
                Triangles[number_of_triangles].Texture = Object->Mesh_Hash_Texture(&number_of_textures, &max_textures, &Textures, Parse_Mesh_Texture(&t2,&t3));
                if (t2) Triangles[number_of_triangles].Texture2 = Object->Mesh_Hash_Texture(&number_of_textures, &max_textures, &Textures, t2);
                if (t3) Triangles[number_of_triangles].Texture3 = Object->Mesh_Hash_Texture(&number_of_textures, &max_textures, &Textures, t3);
                if (t2 || t3) Triangles[number_of_triangles].ThreeTex = true;

                if ((fabs(l1) > EPSILON) || (fabs(l2) > EPSILON))
                {
                    /* Smooth triangle. */

                    Triangles[number_of_triangles].N1 = Object->Mesh_Hash_Normal(&number_of_normals, &max_normals, &Normals, N1);
                    Triangles[number_of_triangles].N2 = Object->Mesh_Hash_Normal(&number_of_normals, &max_normals, &Normals, N2);
                    Triangles[number_of_triangles].N3 = Object->Mesh_Hash_Normal(&number_of_normals, &max_normals, &Normals, N3);

                    Object->Compute_Mesh_Triangle(&Triangles[number_of_triangles], true, P1, P2, P3, N);
                }
                else
                {
                    /* Flat triangle. */

                    Object->Compute_Mesh_Triangle(&Triangles[number_of_triangles], false, P1, P2, P3, N);
                }

                Triangles[number_of_triangles].Normal_Ind = Object->Mesh_Hash_Normal(&number_of_normals, &max_normals, &Normals, N);

                if (Triangles[number_of_triangles].Texture < 0)
                {
                    fully_textured = false;
                }

                number_of_triangles++;
            }
            /* NK degenerate fix */
            else
            {
                /* parse the uv and texture info - even though we'll just throw it
                   away.  why?  if not we get a parse error - we should just ignore the
                   degenerate triangle */
                t2 = t3 = nullptr;
                (void)Parse_Three_UVCoords(UV1,UV2,UV3);
                Parse_Mesh_Texture(&t2,&t3);
            }

            Parse_End();
        END_CASE

        CASE(INSIDE_VECTOR_TOKEN)
            Parse_Vector(Inside_Vect);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    /* Destroy hash tables. */

    Object->Destroy_Mesh_Hash_Tables();

    /* If there are no triangles something went wrong. */

    if (number_of_triangles == 0)
    {
        Error("No triangles in triangle mesh.");
    }

    /* Init triangle mesh data. */

    Object->Data = reinterpret_cast<MESH_DATA *>(POV_MALLOC(sizeof(MESH_DATA), "triangle mesh data"));


    Object->Data->References = 1;

    Object->Data->Tree = nullptr;
    /* NK 1998 */

    if( (fabs(Inside_Vect[X]) < EPSILON) &&  (fabs(Inside_Vect[Y]) < EPSILON) &&  (fabs(Inside_Vect[Z]) < EPSILON))
    {
        Object->has_inside_vector=false;
        Object->Type |= PATCH_OBJECT;
    }
    else
    {
        Object->Data->Inside_Vect = Inside_Vect.normalized();
        Object->has_inside_vector=true;
        Object->Type &= ~PATCH_OBJECT;
    }

    Object->Data->Normals   = nullptr;

    /* [LSK] Removed "Data->" */
    Object->Textures  = nullptr;

    Object->Data->Triangles = nullptr;
    Object->Data->Vertices  = nullptr;

    /* Allocate memory for normals, textures, triangles and vertices. */

    Object->Number_Of_Textures = number_of_textures;

    Object->Data->Number_Of_Normals = number_of_normals;

    Object->Data->Number_Of_Triangles = number_of_triangles;

    Object->Data->Number_Of_Vertices = number_of_vertices;

    Object->Data->Normals = reinterpret_cast<MeshVector *>(POV_MALLOC(number_of_normals*sizeof(MeshVector), "triangle mesh data"));

    if (number_of_textures)
    {
        Set_Flag(Object, MULTITEXTURE_FLAG);

        /* [LSK] Removed "Data->" */
        Object->Textures = reinterpret_cast<TEXTURE **>(POV_MALLOC(number_of_textures*sizeof(TEXTURE *), "triangle mesh data"));
    }

    Object->Data->Triangles = reinterpret_cast<MESH_TRIANGLE *>(POV_MALLOC(number_of_triangles*sizeof(MESH_TRIANGLE), "triangle mesh data"));

    Object->Data->Vertices = reinterpret_cast<MeshVector *>(POV_MALLOC(number_of_vertices*sizeof(MeshVector), "triangle mesh data"));

    /* Copy normals, textures, triangles and vertices into mesh. */

    for (i = 0; i < number_of_normals; i++)
    {
        Object->Data->Normals[i] = Normals[i];
    }

    for (i = 0; i < number_of_textures; i++)
    {
        /* [LSK] Removed "Data->" */
        Object->Textures[i] = Copy_Textures(Textures[i]);
        Post_Textures(Object->Textures[i]);

        /* now free the texture, in order to decrement the reference count */
        Destroy_Textures(Textures[i]);
    }

    if (fully_textured)
    {
        Object->Type |= TEXTURED_OBJECT;
    }

    for (i = 0; i < number_of_triangles; i++)
    {
        Object->Data->Triangles[i] = Triangles[i];
    }

    for (i = 0; i < number_of_vertices; i++)
    {
        Object->Data->Vertices[i] = Vertices[i];
    }

    /* NK 1998 */
    /* do the four steps above, but for UV coordinates*/
    Object->Data->UVCoords  = nullptr;
    Object->Data->Number_Of_UVCoords = number_of_uvcoords;
    Object->Data->UVCoords = reinterpret_cast<MeshUVVector *>(POV_MALLOC(number_of_uvcoords*sizeof(MeshUVVector), "triangle mesh data"));
    for (i = 0; i < number_of_uvcoords; i++)
    {
        Object->Data->UVCoords[i] = UVCoords[i];
    }
    POV_FREE(UVCoords);
    /* NK ---- */

    /* Free temporary memory. */

    POV_FREE(Normals);
    POV_FREE(Textures);
    POV_FREE(Triangles);
    POV_FREE(Vertices);

/*
    Render_Info("Mesh: %ld bytes: %ld vertices, %ld normals, %ld textures, %ld triangles, %ld uv-coords\n",
        Object->Data->Number_Of_Normals*sizeof(MeshVector)+
        Object->Number_Of_Textures*sizeof(TEXTURE *)+
        Object->Data->Number_Of_Triangles*sizeof(MESH_TRIANGLE)+
        Object->Data->Number_Of_Vertices*sizeof(MeshVector)+
        Object->Data->Number_Of_UVCoords*sizeof(MeshUVVector),
        Object->Data->Number_Of_Vertices,
        Object->Data->Number_Of_Normals,
        Object->Number_Of_Textures,
        Object->Data->Number_Of_Triangles,
        Object->Data->Number_Of_UVCoords);
*/
}

/*****************************************************************************
*
* FUNCTION
*
*   Parse_Mesh2
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   OBJECT
*
* AUTHOR
*
*   Nathan Kopp
*
* DESCRIPTION
*
*   Read a triangle mesh - syntax version 2.
*
* CHANGES
*
*   Feb 1998 : Creation.
*
******************************************************************************/

ObjectPtr Parser::Parse_Mesh2()
{
    Mesh *Object;

    Parse_Begin();

    Object = reinterpret_cast<Mesh *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    /* Create object. */

    Object = new Mesh();

    Parse_Mesh2 (Object);

    // Create bounding box.

    Object->Compute_BBox();

    // Parse object modifiers.

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    // Create bounding box tree.

    Object->Build_Mesh_BBox_Tree();

    return Object;
}

void Parser::Parse_Mesh2 (Mesh* Object)
{
    int i;
    int number_of_normals, number_of_textures, number_of_triangles, number_of_vertices, number_of_uvcoords;
    int number_of_normal_indices;
    int a,b,c;
    int n1, n2, n3;
    bool found_normal_indices = false;
    bool found_uv_indices = false;
    bool fully_textured = true;
    bool foundZeroNormal = false;

    DBL l1, l2;
    Vector3d D1, D2, P1, P2, P3, N1, N;
    Vector3d Inside_Vect;

    Vector2d UV1;
    MeshVector *Normals = nullptr;
    MeshVector *Vertices = nullptr;
    TEXTURE **Textures = nullptr;
    MeshUVVector *UVCoords = nullptr;
    MESH_TRIANGLE *Triangles;

    Inside_Vect = Vector3d(0.0, 0.0, 0.0);

    /* normals, uvcoords, and textures are optional */
    number_of_vertices = 0;
    number_of_uvcoords = 0;
    number_of_textures = 0;
    number_of_normals = 0;
    number_of_normal_indices = 0;


    /* -----------------  Get the Normals & UV Vectors & Textures ------------ */
    EXPECT
        /* -------------------  Get the Vertices ------------------- */
        CASE(VERTEX_VECTORS_TOKEN)
            if (number_of_vertices>0)
            {
                Warning("Duplicate vertex_vectors block; ignoring previous block.");
                POV_FREE(Vertices);
            }

            Parse_Begin();

            number_of_vertices = (int)Parse_Float(); Parse_Comma();

            if (number_of_vertices<=0)
                Error("No vertices in triangle mesh.");

            /* allocate memory for vertices */
            Vertices = reinterpret_cast<MeshVector *>(POV_MALLOC(number_of_vertices*sizeof(MeshVector), "triangle mesh data"));

            for(i=0; i<number_of_vertices; i++)
            {
                Parse_Vector(P1); Parse_Comma();
                Vertices[i] = MeshVector(P1);
            }
            Parse_End();
        END_CASE

        CASE(NORMAL_VECTORS_TOKEN)
            if (number_of_normals>0)
            {
                Warning("Duplicate normal_vectors block; ignoring previous block.");
                POV_FREE(Normals);
            }

            Parse_Begin();
            number_of_normals = (int)Parse_Float(); Parse_Comma();

            if (number_of_normals>0)
            {
                Normals = reinterpret_cast<MeshVector *>(POV_MALLOC(number_of_normals*sizeof(MeshVector), "triangle mesh data"));

                /* leave space in the array for the raw triangle normals */
                for(i=0; i<number_of_normals; i++)
                {
                    Parse_Vector(N1); Parse_Comma();
                    if(fabs(N1[X])<EPSILON && fabs(N1[Y])<EPSILON && fabs(N1[Z])<EPSILON)
                    {
                        N1[X] = 1.0;  // make it nonzero
                        if(!foundZeroNormal)
                            Warning("Normal vector in mesh2 cannot be zero - changing it to <1,0,0>.");
                        foundZeroNormal = true;
                    }
                    N1.normalize();
                    Normals[i] = MeshVector(N1);
                }
            }

            Parse_End();
        END_CASE

        CASE(UV_VECTORS_TOKEN)
            if (number_of_uvcoords>0)
            {
                Warning("Duplicate uv_vectors block; ignoring previous block.");
                POV_FREE(UVCoords);
            }

            Parse_Begin();
            number_of_uvcoords = (int)Parse_Float(); Parse_Comma();

            if (number_of_uvcoords>0)
            {
                UVCoords = reinterpret_cast<MeshUVVector *>(POV_MALLOC(number_of_uvcoords*sizeof(MeshUVVector), "triangle mesh data"));

                for(i=0; i<number_of_uvcoords; i++)
                {
                    Parse_UV_Vect(UV1); Parse_Comma();
                    UVCoords[i] = MeshUVVector(UV1);
                }
            }

            Parse_End();
        END_CASE

        /*OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    EXPECT*/
        CASE(TEXTURE_LIST_TOKEN)
            Parse_Begin();

            number_of_textures = (int)Parse_Float();  Parse_Comma();

            if (number_of_textures>0)
            {
                Textures = reinterpret_cast<TEXTURE **>(POV_MALLOC(number_of_textures*sizeof(TEXTURE *), "triangle mesh data"));

                for(i=0; i<number_of_textures; i++)
                {
                    /*
                    GET(TEXTURE_ID_TOKEN)
                    Textures[i] = Copy_Texture_Pointer(CurrentTokenDataPtr<TEXTURE*>());
                    */
                    GET(TEXTURE_TOKEN);
                    Parse_Begin();
                    Textures[i] = Parse_Texture();
                    Post_Textures(Textures[i]);
                    Parse_End();
                    Parse_Comma();
                }
            }

            Parse_End();
            EXIT
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE

    END_EXPECT

    if (number_of_vertices == 0)
        Error("Vertex vectors not found in mesh2");

    /* first make sure we at least have one UV coordinate */
    if (number_of_uvcoords == 0)
    {
        number_of_uvcoords = 1;
        UVCoords = reinterpret_cast<MeshUVVector *>(POV_MALLOC(number_of_uvcoords*sizeof(MeshUVVector), "triangle mesh data"));
        UVCoords[0][U] = 0;
        UVCoords[0][V] = 0;
    }

    /* -------------------  Get the Faces ------------------- */
    GET(FACE_INDICES_TOKEN)
            Parse_Begin();

    /* number faces is mandatory, so we ask how many there are */
    number_of_triangles = Parse_Float(); Parse_Comma();

    if (number_of_triangles == 0)
    {
        Error("No triangles in triangle mesh.");
    }

    /* allocate memory for triangles */
    Triangles = reinterpret_cast<MESH_TRIANGLE *>(POV_MALLOC(number_of_triangles*sizeof(MESH_TRIANGLE), "triangle mesh data"));

    /* start reading triangles */

    for(i=0; i<number_of_triangles; i++)
    {
        /* read in the indices vector */
        Parse_Vector(P1); Parse_Comma();

        /* convert the vector to integers */
        a = (int)P1[X];
        b = (int)P1[Y];
        c = (int)P1[Z];

        /* a--;b--;c--; use this to start external stuff at 1 */
        if ( a<0 || b<0 || c<0 ||
             a>=number_of_vertices || b>=number_of_vertices ||
             c>=number_of_vertices)
        {
            Error("Mesh face index out of range.");
        }

        /* Init triangle. */
        Object->Init_Mesh_Triangle(&Triangles[i]);

        /* assign the vertices */
        Triangles[i].P1 = a;
        Triangles[i].P2 = b;
        Triangles[i].P3 = c;

        /* look for a texture index */
        EXPECT_ONE_CAT
            CASE_FLOAT_UNGET
                Triangles[i].Texture = Parse_Float(); Parse_Comma();
                if (Triangles[i].Texture >= number_of_textures ||
                    Triangles[i].Texture < 0)
                    Error("Texture index out of range in mesh2.");
            END_CASE

            OTHERWISE
                Triangles[i].Texture = -1;
                fully_textured = false;
                UNGET
            END_CASE
        END_EXPECT
        /* look for a texture index */
        EXPECT_ONE_CAT
            CASE_FLOAT_UNGET
                Triangles[i].Texture2 = Parse_Float(); Parse_Comma();
                if (Triangles[i].Texture2 >= number_of_textures ||
                    Triangles[i].Texture2 < 0)
                    Error("Texture index out of range in mesh2.");
                Triangles[i].ThreeTex = true;
            END_CASE
            OTHERWISE
                Triangles[i].Texture2 = -1;
                UNGET
            END_CASE
        END_EXPECT
        /* look for a texture index */
        EXPECT_ONE_CAT
            CASE_FLOAT_UNGET
                Triangles[i].Texture3 = Parse_Float(); Parse_Comma();
                if (Triangles[i].Texture3 >= number_of_textures ||
                    Triangles[i].Texture3 < 0)
                    Error("Texture index out of range in mesh2.");
                Triangles[i].ThreeTex = true;
            END_CASE
            OTHERWISE
                Triangles[i].Texture3 = -1;
                UNGET
            END_CASE
        END_EXPECT

    }

    Parse_End();

    /* now we get the uv_indices & normal_indices in either order */

    EXPECT
        CASE(UV_INDICES_TOKEN)
            if (found_uv_indices)
            {
                Error("Only one uv_indices section is allowed in mesh2");
            }
            found_uv_indices = true;
            Parse_Begin();

            if (Parse_Float() != number_of_triangles)
                Error("Number of uv indices must equal number of faces.");
            Parse_Comma();

            for (i=0; i<number_of_triangles; i++)
            {
                /* read in the indices vector */
                Parse_Vector(P1); Parse_Comma();

                /* convert the vector to integers */
                a = (int)P1[X];
                b = (int)P1[Y];
                c = (int)P1[Z];

                /* a--;b--;c--; use this to start external stuff at 1 */
                if ( a<0 || b<0 || c<0 ||
                     a>=number_of_uvcoords || b>=number_of_uvcoords ||
                     c>=number_of_uvcoords)
                {
                    Error("Mesh UV index out of range.");
                }

                /* assign the uv coordinate */
                Triangles[i].UV1 = a;
                Triangles[i].UV2 = b;
                Triangles[i].UV3 = c;
            }
            Parse_End();
            /*EXIT*/
        END_CASE

    /*
        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    EXPECT
    */
        CASE(NORMAL_INDICES_TOKEN)
            if (found_normal_indices)
            {
                Error("Only one normal_indices section is allowed in mesh2");
            }
            found_normal_indices = true;
            Parse_Begin();

            /*
            Change - if fewer normals than triangles, then no problem - the
            rest will be flat triangles.

            if (Parse_Float() != number_of_triangles)
                Error("Number of normal indices must equal number of faces.");
            */
            number_of_normal_indices = Parse_Float();
            if (number_of_normal_indices > number_of_triangles)
                Error("Number of normal indices cannot be more than the number of faces.");

            Parse_Comma();

            for (i=0; i<number_of_normal_indices; i++)
            {
                /* read in the indices vector */
                Parse_Vector(P1); Parse_Comma();

                /* convert the vector to integers */
                a = (int)P1[X];
                b = (int)P1[Y];
                c = (int)P1[Z];

                /* a--;b--;c--; use this to start external stuff at 1 */
                if ( a<0 || b<0 ||
                     c<0 ||
                     a>=number_of_normals || b>=number_of_normals ||
                     c>=number_of_normals)
                {
                    Error("Mesh normal index out of range.");
                }

                /* assign the uv coordinate */
                Triangles[i].N1 = a;
                Triangles[i].N2 = b;
                Triangles[i].N3 = c;
            }
            Parse_End();
            /*EXIT*/
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    /* ----------------------------------------------------- */
    /* ----------------------------------------------------- */

    EXPECT
        CASE(INSIDE_VECTOR_TOKEN)
            Parse_Vector(Inside_Vect);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if (fully_textured)
        Object->Type |= TEXTURED_OBJECT;

    if (!found_uv_indices)
    {
        if (number_of_uvcoords==number_of_vertices)
        {
            for (i=0; i<number_of_triangles; i++)
            {
                Triangles[i].UV1 = Triangles[i].P1;
                Triangles[i].UV2 = Triangles[i].P2;
                Triangles[i].UV3 = Triangles[i].P3;
            }
        }
        else if (number_of_uvcoords==1)
        {
            for (i=0; i<number_of_triangles; i++)
            {
                Triangles[i].UV1 = 0;
                Triangles[i].UV2 = 0;
                Triangles[i].UV3 = 0;
            }
        }
        else
        {
            Error("Missing uv_indicies section in mesh2.");
        }
    }

    if (!found_normal_indices)
    {
        if (number_of_normals==number_of_vertices)
        {
            /* If number of normals matches number of vertices, then assume
               that the normal_indices are the same as the triangle indices
               (left out for file size reasons).
               So, we pretend that we read in some normal_indices
            */
            number_of_normal_indices = number_of_triangles;

            for (i=0; i<number_of_triangles; i++)
            {
                Triangles[i].N1 = Triangles[i].P1;
                Triangles[i].N2 = Triangles[i].P2;
                Triangles[i].N3 = Triangles[i].P3;
            }
        }
        else if (number_of_normals)
        {
            Error("Missing normal_indicies section in mesh2.");
        }
    }

    /* ---------------- Compute Triangle Normals ---------------- */

    /* reallocate the normals stuff */
    if (!number_of_normals)
        Normals = reinterpret_cast<MeshVector *>(POV_MALLOC(number_of_triangles*sizeof(MeshVector), "triangle mesh data"));
    else
        Normals = reinterpret_cast<MeshVector *>(POV_REALLOC(Normals, (number_of_normals+number_of_triangles)*sizeof(MeshVector), "triangle mesh data"));

    for (i=0; i<number_of_triangles; i++)
    {
        a = (int) Triangles[i].P1;
        b = (int) Triangles[i].P2;
        c = (int) Triangles[i].P3;
        n1 = (int) Triangles[i].N1;
        n2 = (int) Triangles[i].N2;
        n3 = (int) Triangles[i].N3;

        P1 = Vector3d(Vertices[a]);
        P2 = Vector3d(Vertices[b]);
        P3 = Vector3d(Vertices[c]);

        Triangles[i].Smooth = false;

        /* compute the normal (check for smoothness) */
        /* if number_of_normal_indices > 0, then the first triangles
           are smooth and the rest are flat */
        if (i<number_of_normal_indices)
        {
            /* Check for equal normals. */
            D1 = Vector3d(Normals[n1]) - Vector3d(Normals[n2]);
            D2 = Vector3d(Normals[n1]) - Vector3d(Normals[n3]);

            l1 = D1.lengthSqr();
            l2 = D2.lengthSqr();

            if ((fabs(l1) > EPSILON) || (fabs(l2) > EPSILON))
            {
                /* Smooth triangle. */
                Object->Compute_Mesh_Triangle(&Triangles[i], true, P1, P2, P3, N);
                Triangles[i].Smooth = true;
            }
            else
            {
                /* Flat triangle. */
                Object->Compute_Mesh_Triangle(&Triangles[i], false, P1, P2, P3, N);
            }
        }
        else
        {
            /* Flat triangle. */
            Object->Compute_Mesh_Triangle(&Triangles[i], false, P1, P2, P3, N);
        }

        /* assign the triangle normal that we just computed */
        Triangles[i].Normal_Ind = i+number_of_normals;
        Normals[i+number_of_normals] = MeshVector(N);
    }

    /* now remember how many normals we really have */
    number_of_normals += number_of_triangles;

    /* ----------------------------------------------------- */

    /* Init triangle mesh data. */
    Object->Data = reinterpret_cast<MESH_DATA *>(POV_MALLOC(sizeof(MESH_DATA), "triangle mesh data"));
    Object->Data->References = 1;
    Object->Data->Tree = nullptr;
    /* NK 1998 */
    /*YS* 31/12/1999 */

    if( (fabs(Inside_Vect[X]) < EPSILON) &&  (fabs(Inside_Vect[Y]) < EPSILON) &&  (fabs(Inside_Vect[Z]) < EPSILON))
    {
        Object->has_inside_vector=false;
        Object->Type |= PATCH_OBJECT;
    }
    else
    {
        Object->Data->Inside_Vect = Inside_Vect.normalized();
        Object->has_inside_vector=true;
        Object->Type &= ~PATCH_OBJECT;
    }
    /*YS*/

    /* copy pointers to normals, triangles, textures, and vertices. */
    Object->Data->Normals   = Normals;
    Object->Data->Triangles = Triangles;
    Object->Data->Vertices  = Vertices;
    Object->Data->UVCoords  = UVCoords;
    /* [LSK] Removed "Data->" */
    Object->Textures  = Textures;

    /* copy number of for normals, textures, triangles and vertices. */
    Object->Data->Number_Of_Normals = number_of_normals;
    Object->Data->Number_Of_Triangles = number_of_triangles;
    Object->Data->Number_Of_Vertices = number_of_vertices;
    Object->Data->Number_Of_UVCoords  = number_of_uvcoords;
    Object->Number_Of_Textures = number_of_textures;

    if (number_of_textures)
    {
        Set_Flag(Object, MULTITEXTURE_FLAG);
    }

/*
    Render_Info("Mesh2: %ld bytes: %ld vertices, %ld normals, %ld textures, %ld triangles, %ld uv-coords\n",
        Object->Data->Number_Of_Normals*sizeof(MeshVector)+
        Object->Number_Of_Textures*sizeof(TEXTURE *)+
        Object->Data->Number_Of_Triangles*sizeof(MESH_TRIANGLE)+
        Object->Data->Number_Of_Vertices*sizeof(MeshVector)+
        Object->Data->Number_Of_UVCoords*sizeof(MeshUVVector),
        Object->Data->Number_Of_Vertices,
        Object->Data->Number_Of_Normals,
        Object->Number_Of_Textures,
        Object->Data->Number_Of_Triangles,
        Object->Data->Number_Of_UVCoords);
*/
}


/*****************************************************************************
*
* FUNCTION
*
*   Parse_Mesh_Texture
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   OBJECT
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Read an individual triangle mesh texture.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

TEXTURE *Parser::Parse_Mesh_Texture (TEXTURE **t2, TEXTURE **t3)
{
    TEXTURE *Texture;

    Texture = nullptr;

    EXPECT
        CASE(TEXTURE_TOKEN)
            Parse_Begin();

            GET(TEXTURE_ID_TOKEN);

            Texture = CurrentTokenDataPtr<TEXTURE*>();

            Parse_End();
        END_CASE

        /* NK */
        CASE(TEXTURE_LIST_TOKEN)
            Parse_Begin();

            GET(TEXTURE_ID_TOKEN);
            Texture = CurrentTokenDataPtr<TEXTURE*>();

            Parse_Comma();

            GET(TEXTURE_ID_TOKEN);
            *t2 = CurrentTokenDataPtr<TEXTURE*>();

            Parse_Comma();

            GET(TEXTURE_ID_TOKEN);
            *t3 = CurrentTokenDataPtr<TEXTURE*>();

            Parse_End();
            EXIT
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    return(Texture);
}


/*****************************************************************************
*
* FUNCTION
*
*   Parse_Ovus
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   OBJECT
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jul 2010 : Creation.
*   Jul 2016 : extension with distance & radius
*
******************************************************************************/

ObjectPtr Parser::Parse_Ovus()
{
    Ovus *Object;

    Parse_Begin();

    Object = reinterpret_cast<Ovus *>(Parse_Object_Id());
    if (Object != nullptr)
    {
        return(reinterpret_cast<ObjectPtr>(Object));
    }

    Object = new Ovus();


    Object->BottomRadius = Parse_Float(); /* Bottom radius */
    Parse_Comma();
    Object->TopRadius = Parse_Float(); /* Top radius */
    // default values, for backward compatibility
    Object->VerticalSpherePosition = Object->BottomRadius;
    Object->ConnectingRadius = 2.0 * std::max(Object->BottomRadius, Object->TopRadius);
    EXPECT
        CASE(RADIUS_TOKEN)
            Object->ConnectingRadius = Parse_Float();
        END_CASE
        CASE(DISTANCE_TOKEN)
            Object->VerticalSpherePosition = Parse_Float();
        END_CASE
        CASE(PRECISION_TOKEN)
            Object->RootTolerance = Parse_Float();
        END_CASE
        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    /*
     ** Pre-compute the important values
     */
    if ((Object->TopRadius < 0)||(Object->BottomRadius <= 0))
    {
        Error("Both Ovus radii must be positive, and bottom radius must be strictly positive");
    }
    if (Object->VerticalSpherePosition < Object->BottomRadius)
    {
        Error("distance of Ovus must be greater or equal to bottom radius");
        // in theory, it would be possible to allow VerticalSpherePosition + TopRadius >= BottomRadius
        // (with VerticalSpherePosition > 0)
        // but the computation of BottomVertical & TopVertical would need more work, as the current formula
        // use a simplification based on VerticalSpherePosition >= BottomRadius
    }
    if ( Object->TopRadius + Object->BottomRadius + Object->VerticalSpherePosition > 2*Object->ConnectingRadius)
    {
        Error("Connecting radius of Ovus is too small. Should be at least half the sum of the three other distances. sphere or lemon object could also be considered.");
    }
    /*
     *  b : BottomRadius
     *  r : ConnectingRadius
     *  t : TopRadius
     *  v : VerticalSpherePosition
     *
     * solve (x^2+y^2)=(r-b)^2, (x^2+(y-v)^2)=(r-t)^2, v>=b, t>=0, b>0, 2r>=(b+t+v) for x,y
     *  intersection of two circles:
     *   from the origin (center of bottom sphere), connecting radius - bottom radius
     *   from the center of top sphere, connecting radius - top radius
     *
     * x = +/- 1/2 sqrt( - (b^2-2bt+t^2-v^2)(b^2-4br+2bt+4r^2-4rt+t^2-v^2)/v^2)
     * remaining code expect x >= 0
     */

    Object->HorizontalPosition = sqrt(
            -(Sqr(Object->BottomRadius)-2.0*Object->BottomRadius*Object->TopRadius+Sqr(Object->TopRadius)-Sqr(Object->VerticalSpherePosition))
            *(Sqr(Object->BottomRadius)-4.0*Object->BottomRadius*Object->ConnectingRadius+2.0*Object->BottomRadius*Object->TopRadius+4.0*Sqr(Object->ConnectingRadius)-4.0*Object->ConnectingRadius*Object->TopRadius+Sqr(Object->TopRadius) -Sqr(Object->VerticalSpherePosition))
            )
        /(Object->VerticalSpherePosition*2.0);

    /*
     * for t=b+v and r> (b^2-t^2-v^2)/(2(b-t))
     *
     * y = sqrt( r^2-2rt+t^2-x^2)
     *
     *
     * for t>b+v and r = (b+v+t)/2
     * for b<t<b+v and r> (b^2-t^2-v^2)/(2(b-t))
     * for t=b+v and r> (b^2-t^2-v^2)/(2(b-t))
     *
     * y = sqrt( r^2-2rt+t^2-x^2) +v
     *
     *
     * else
     *
     * y = v- sqrt( r^2-2rt+t^2-x^2)
     */
    if ((Object->TopRadius == (Object->BottomRadius+Object->VerticalSpherePosition))&&(Object->ConnectingRadius > ((Sqr(Object->BottomRadius)-Sqr(Object->TopRadius)-Sqr(Object->VerticalSpherePosition))/(2.0*Object->VerticalSpherePosition))))
    {
        Object->VerticalPosition = sqrt(Sqr(Object->ConnectingRadius-Object->TopRadius)-Sqr(Object->HorizontalPosition));
    }
    else if
        (
         (( Object->TopRadius > (Object->BottomRadius + Object->VerticalSpherePosition ) ) && ( Object->ConnectingRadius == ((Object->BottomRadius+Object->TopRadius+Object->VerticalSpherePosition)/2.0)))
         ||((Object->BottomRadius < Object->TopRadius) && (Object->TopRadius < (Object->BottomRadius+Object->VerticalSpherePosition))&&(Object->ConnectingRadius > ((Sqr(Object->BottomRadius)-Sqr(Object->TopRadius)-Sqr(Object->VerticalSpherePosition))/(2.0*Object->VerticalSpherePosition))))
         ||( (Object->TopRadius == (Object->BottomRadius+Object->VerticalSpherePosition))&&(Object->ConnectingRadius > ((Sqr(Object->BottomRadius)-Sqr(Object->TopRadius)-Sqr(Object->VerticalSpherePosition))/(2.0*Object->VerticalSpherePosition))))
        )
    {
        Object->VerticalPosition = Object->VerticalSpherePosition+sqrt(Sqr(Object->ConnectingRadius-Object->TopRadius)-Sqr(Object->HorizontalPosition));
    }
    else
    {
        Object->VerticalPosition = Object->VerticalSpherePosition-sqrt(Sqr(Object->ConnectingRadius-Object->TopRadius)-Sqr(Object->HorizontalPosition));
    }
    // if not a degenerated sphere, HorizontalPosition is a square root, test only against 0, as negative is not possible
    if (Object->HorizontalPosition != 0.0)
    {
        Object->BottomVertical = -Object->VerticalPosition*Object->BottomRadius/(Object->ConnectingRadius-Object->BottomRadius);
        Object->TopVertical = -( Object->VerticalPosition-Object->VerticalSpherePosition)* Object->TopRadius / (Object->ConnectingRadius - Object->TopRadius)  + Object->VerticalSpherePosition;
    }
    else
    { // replace the ovus with the top sphere, keeping the parsing of option compatible with ovus (sturm)
        Object->VerticalSpherePosition = Object->VerticalPosition;
        Object->TopRadius = Object->ConnectingRadius;
        Object->TopVertical = Object->VerticalSpherePosition-Object->ConnectingRadius;
        Object->BottomVertical = -Object->ConnectingRadius;// low enough
        Object->BottomRadius = 0.0;
        Object->ConnectingRadius = 0.0;
        Warning("Ovus is reduced to a sphere, consider using a real sphere instead");
    }

    Object->Compute_BBox();
    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));
    return (reinterpret_cast<ObjectPtr>(Object));
}

/*****************************************************************************
*
* FUNCTION
*
*   Parse_Parametric
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

ObjectPtr Parser::Parse_Parametric(void)
{
    Parametric  *Object;
    DBL         temp;
    char        PrecompFlag = 0;
    int         PrecompDepth = 1;
    Vector2d    tempUV;

    Parse_Begin();

    Object = reinterpret_cast<Parametric *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new Parametric();

    Parse_FunctionOrContentList(Object->Function, 3);

    Parse_UV_Vect(tempUV);
    Object->umin = tempUV[U];
    Object->vmin = tempUV[V];
    Parse_Comma();

    Parse_UV_Vect(tempUV);
    Object->umax = tempUV[U];
    Object->vmax = tempUV[V];

    if(Object->umin>Object->umax)
    {
        temp = Object->umin;
        Object->umin = Object->umax;
        Object->umax = temp;
    }
    if(Object->vmin>Object->vmax)
    {
        temp = Object->vmin;
        Object->vmin = Object->vmax;
        Object->vmax = temp;
    }

    EXPECT
        CASE(ACCURACY_TOKEN)
            Object->accuracy= Parse_Float();
        END_CASE

        CASE(MAX_GRADIENT_TOKEN)
            Object->max_gradient = Parse_Float();
        END_CASE

        CASE(PRECOMPUTE_TOKEN)
            PrecompDepth= Parse_Float();
            Parse_Comma();

            if (AllowToken(X_TOKEN))
                PrecompFlag |= 1;

            Parse_Comma();

            if (AllowToken(Y_TOKEN))
                PrecompFlag |= 2;

            Parse_Comma();

            if (AllowToken(Z_TOKEN))
                PrecompFlag |= 4;
        END_CASE

        CASE(CONTAINED_BY_TOKEN)
            ParseContainedBy(Object->container, Object);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    if(PrecompFlag != 0)
        Object->Precompute_Parametric_Values(PrecompFlag, PrecompDepth, GetParserDataPtr());

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Plane ()
{
    DBL len;
    Plane *Object;

    Parse_Begin ();

    Object = reinterpret_cast<Plane *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new Plane();

    Parse_Vector(Object->Normal_Vector);   Parse_Comma();
    len = Object->Normal_Vector.length();
    if (len < EPSILON)
    {
        Error("Degenerate plane normal.");
    }
    Object->Normal_Vector /= len;
    Object->Distance = -Parse_Float();

    Object->Compute_BBox();

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Poly (int order)
{
    Poly *Object;

    Parse_Begin ();

    Object = reinterpret_cast<Poly *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    if (order == 0)
    {
        order = (int)Parse_Float();      Parse_Comma();
        if (order < 2 || order > MAX_ORDER)
            Error("Order of poly is out of range.");
    }

    Object = new Poly(order);

    Parse_Coeffs(Object->Order, &(Object->Coeffs[0]));

    Object->Compute_BBox();

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Polynom ()
{
    Poly *Object;
    unsigned int x,y,z;
    int order;
    DBL value;

    Parse_Begin ();

    Object = reinterpret_cast<Poly *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    order = (int)Parse_Float();      Parse_Comma();
    if (order < 2 || order > MAX_ORDER)
    {
        Error("Order of polynom is out of range.");
    }

    Object = new Poly(order);

    EXPECT
        CASE (XYZ_TOKEN)
            Parse_Paren_Begin();
            x = (unsigned int)Parse_Float();
            Parse_Comma();
            y = (unsigned int)Parse_Float();
            Parse_Comma();
            z = (unsigned int)Parse_Float();
            Parse_Paren_End();
            GET (COLON_TOKEN);
            value = Parse_Float();
            if (!Object->Set_Coeff(x,y,z,value))
            {
                Error("Coefficient of polynom is out of range.");
            }
            Parse_Comma();
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Object->Compute_BBox();

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}

/*****************************************************************************
*
* FUNCTION
*
*   Parse_Polygon
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   ObjectPtr  -
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
*   May 1994 : Creation.
*
*   Oct 1994 : Modified to use new polygon data structure. [DB]
*
******************************************************************************/

ObjectPtr Parser::Parse_Polygon()
{
    int i, closed = false;
    int Number;
    Polygon *Object;
    Vector3d *Points;
    Vector3d P;

    Parse_Begin();

    Object = reinterpret_cast<Polygon *>(Parse_Object_Id());
    if (Object != nullptr)
    {
        return(reinterpret_cast<ObjectPtr>(Object));
    }

    Object = new Polygon();

    Number = (int)Parse_Float();

    if (Number < 3)
    {
        Error("Polygon needs at least three points.");
    }

    Points = reinterpret_cast<Vector3d *>(POV_MALLOC((Number+1)*sizeof(Vector3d), "temporary polygon points"));

    Parse_Comma();

    for (i = 0; i < Number; i++)
    {
        Parse_Vector(Points[i]);

        // NB we allow for a trailing comma at the end of the list,
        // to facilitate auto-generation of lists.
        Parse_Comma();
    }

    /* Check for closed polygons. */

    P = Points[0];

    for (i = 1; i < Number; i++)
    {
        closed = false;

        if ((fabs(P[X] - Points[i][X]) < EPSILON) &&
            (fabs(P[Y] - Points[i][Y]) < EPSILON) &&
            (fabs(P[Z] - Points[i][Z]) < EPSILON))
        {
            // force almost-identical vertices to be /exactly/ identical,
            // to make processing easier later
            Points[i] = P;

            i++;

            if (i < Number)
            {
                P = Points[i];
            }

            closed = true;
        }
    }

    if (!closed)
    {
        Warning("Polygon not closed. Closing it.");

        Points[Number] = P;

        Number++;
    }

    Object->Compute_Polygon(Number, Points);

    POV_FREE (Points);

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    return(reinterpret_cast<ObjectPtr>(Object));
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Prism
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   ObjectPtr  -
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
*   May 1994 : Creation.
*
******************************************************************************/

ObjectPtr Parser::Parse_Prism()
{
    int i, closed = false;
    DBL h;
    int loopStart = 0;

    Prism *Object;
    Vector2d *Points;
    Vector2d P;

    Parse_Begin();

    Object = reinterpret_cast<Prism *>(Parse_Object_Id());
    if (Object != nullptr)
    {
        return(reinterpret_cast<ObjectPtr>(Object));
    }

    Object = new Prism();

    /*
     * Determine kind of spline used (linear, quadratic, cubic)
     * and type of sweeping (linear, conic).
     */

    EXPECT
        CASE(LINEAR_SPLINE_TOKEN)
            Object->Spline_Type = LINEAR_SPLINE;
        END_CASE

        CASE(QUADRATIC_SPLINE_TOKEN)
            Object->Spline_Type = QUADRATIC_SPLINE;
        END_CASE

        CASE(CUBIC_SPLINE_TOKEN)
            Object->Spline_Type = CUBIC_SPLINE;
        END_CASE

        CASE(BEZIER_SPLINE_TOKEN)
            Object->Spline_Type = BEZIER_SPLINE;
        END_CASE

        CASE(LINEAR_SWEEP_TOKEN)
            Object->Sweep_Type = LINEAR_SWEEP;
        END_CASE

        CASE(CONIC_SWEEP_TOKEN)
            Object->Sweep_Type = CONIC_SWEEP;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    /* Read prism heights. */

    Object->Height1 = Parse_Float(); Parse_Comma();
    Object->Height2 = Parse_Float(); Parse_Comma();

    if (Object->Height1 > Object->Height2)
    {
        h = Object->Height1;
        Object->Height1 = Object->Height2;
        Object->Height2 = h;
    }

    /* Get number of points = number of segments. */

    Object->Number = (int)Parse_Float();

    switch (Object->Spline_Type)
    {
        case LINEAR_SPLINE :

            if (Object->Number < 3)
            {
                Error("Prism with linear splines must have at least three points.");
            }

            break;

        case QUADRATIC_SPLINE :

            if (Object->Number < 5)
            {
                Error("Prism with quadratic splines must have at least five points.");
            }

            break;

        case CUBIC_SPLINE :

            if (Object->Number < 6)
            {
                Error("Prism with cubic splines must have at least six points.");
            }

            break;

        case BEZIER_SPLINE :

            if ((Object->Number & 3) != 0)
            {
                Error("Prism with Bezier splines must have four points per segment.");
            }

            break;
    }

    /* Allocate Object->Number points for the prism. */

    Points = reinterpret_cast<Vector2d *>(POV_MALLOC((Object->Number+1) * sizeof(Vector2d), "temporary prism points"));

    /* Read points (x, y : coordinate of 2d point; z : not used). */

    Parse_Comma();

    for (i = 0; i < Object->Number; i++)
    {
        Parse_UV_Vect(Points[i]);

        // NB we allow for a trailing comma at the end of the list,
        // to facilitate auto-generation of lists.
        Parse_Comma();
    }

    /* Closed or not closed that's the question. */

    if (AllowToken(OPEN_TOKEN))
        Clear_Flag(Object, CLOSED_FLAG);

    /* Check for closed prism. */

    if ((Object->Spline_Type == LINEAR_SPLINE) ||
        (Object->Spline_Type == QUADRATIC_SPLINE) ||
        (Object->Spline_Type == CUBIC_SPLINE))
    {
        switch (Object->Spline_Type)
        {
            case LINEAR_SPLINE :

                i = 1;

                P = Points[0];

                break;

            case QUADRATIC_SPLINE :
            case CUBIC_SPLINE :

                i = 2;

                P = Points[1];

                break;
        }

        for (; i < Object->Number; i++)
        {
            closed = false;

            if ((fabs(P[X] - Points[i][X]) < EPSILON) &&
                (fabs(P[Y] - Points[i][Y]) < EPSILON))
            {
                switch (Object->Spline_Type)
                {
                    case LINEAR_SPLINE :

                        i++;

                        if (i < Object->Number)
                        {
                            P = Points[i];
                        }

                        break;

                    case QUADRATIC_SPLINE :

                        i += 2;

                        if (i < Object->Number)
                        {
                            P = Points[i];
                        }

                        break;

                    case CUBIC_SPLINE :

                        i += 3;

                        if (i < Object->Number)
                        {
                            P = Points[i];
                        }

                        break;
                }

                closed = true;
            }
        }
    }
    else
    {
        closed = true;

        loopStart = 0;

        for (i = 4; i < Object->Number; i += 4)
        {
            if ((fabs(Points[i][X] - Points[i-1][X]) > EPSILON) ||
                (fabs(Points[i][Y] - Points[i-1][Y]) > EPSILON))
            {
                //. this is a different point.  Check if we have a loop.
                if ((fabs(Points[i-1][X] - Points[loopStart][X]) > EPSILON) ||
                    (fabs(Points[i-1][Y] - Points[loopStart][Y]) > EPSILON))
                {
                    closed = false;
                    break;
                }

                loopStart = i;
            }
        }
        if ((fabs(Points[Object->Number-1][X] - Points[loopStart][X]) > EPSILON) ||
            (fabs(Points[Object->Number-1][Y] - Points[loopStart][Y]) > EPSILON))
        {
            closed = false;
        }
    }

    if (!closed)
    {
        if (Object->Spline_Type == LINEAR_SPLINE)
        {
            Points[Object->Number] = P;

            Object->Number++;

            Warning("Linear prism not closed. Closing it.");
        }
        else
        {
            Set_Flag(Object, DEGENERATE_FLAG);

            Warning("Prism not closed. Ignoring it.");
        }
    }

    /* Compute spline segments. */

    Object->Compute_Prism(Points, GetParserDataPtr()->Stats());

    /* Compute bounding box. */

    Object->Compute_BBox();

    /* Parse object's modifiers. */

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    /* Destroy temporary points. */

    POV_FREE (Points);

    return(reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Quadric ()
{
    Vector3d Min, Max;
    Quadric *Object;

    Parse_Begin ();

    Object = reinterpret_cast<Quadric *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new Quadric();

    Parse_Vector(Object->Square_Terms);     Parse_Comma();
    Parse_Vector(Object->Mixed_Terms);      Parse_Comma();
    Parse_Vector(Object->Terms);            Parse_Comma();
    Object->Constant = Parse_Float();

    Min = Vector3d(-BOUND_HUGE);
    Max = Vector3d(BOUND_HUGE);

    Object->Compute_BBox(Min, Max);

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Smooth_Triangle ()
{
    SmoothTriangle *Object;
    short degen;
    DBL vlen;

    degen=false;

    Parse_Begin ();

    Object = reinterpret_cast<SmoothTriangle *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new SmoothTriangle();

    Parse_Vector (Object->P1);    Parse_Comma();
    Parse_Vector (Object->N1);    Parse_Comma();

    vlen = Object->N1.length();

    if (vlen == 0.0)
        degen=true;
    else
        Object->N1 /= vlen;

    Parse_Vector (Object->P2);    Parse_Comma();
    Parse_Vector (Object->N2);    Parse_Comma();

    vlen = Object->N2.length();

    if(vlen == 0.0)
        degen=true;
    else
        Object->N2 /= vlen;

    Parse_Vector (Object->P3);    Parse_Comma();
    Parse_Vector (Object->N3);

    vlen = Object->N3.length();

    if(vlen == 0.0)
        degen=true;
    else
        Object->N3.normalize();

    if(!degen)
        degen = !Object->Compute_Triangle();

    if(degen)
        Warning("Degenerate triangle. Please remove.");

    Object->Compute_BBox();

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Sor
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   ObjectPtr  -
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Read a surface of revolution primitive.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

ObjectPtr Parser::Parse_Sor()
{
    int i;
    Sor *Object;
    Vector2d *Points;

    Parse_Begin();

    Object = reinterpret_cast<Sor *>(Parse_Object_Id());
    if (Object != nullptr)
    {
        return(reinterpret_cast<ObjectPtr>(Object));
    }

    Object = new Sor();

    /* Get number of points. */

    Object->Number = (int)Parse_Float();

    if (Object->Number <4)
    {
        Error("Surface of revolution must have at least four points.");
    }

    /* Get temporary points describing the rotated curve. */

    Points = reinterpret_cast<Vector2d *>(POV_MALLOC(Object->Number*sizeof(Vector2d), "temporary surface of revolution points"));

    /* Read points (x : radius; y : height; z : not used). */

    Parse_Comma();

    for (i = 0; i < Object->Number; i++)
    {
        Parse_UV_Vect(Points[i]);

        if ((Points[i][X] < 0.0) ||
            ((i > 1 ) && (i < Object->Number - 1) && (Points[i][Y] <= Points[i-1][Y])))
        {
            Error("Incorrect point in surface of revolution.");
        }

        // NB we allow for a trailing comma at the end of the list,
        // to facilitate auto-generation of lists.
        Parse_Comma();
    }

    /* Closed or not closed that's the question. */

    if (AllowToken(OPEN_TOKEN))
        Clear_Flag(Object, CLOSED_FLAG);

    /* There are Number-3 segments! */

    Object->Number -= 3;

    /* Compute spline segments. */

    Object->Compute_Sor(Points, GetParserDataPtr()->Stats());

    /* Compute bounding box. */

    Object->Compute_BBox();

    /* Parse object's modifiers. */

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    /* Destroy temporary points. */

    POV_FREE (Points);

    if (Object->Spline->BCyl->number > sceneData->Max_Bounding_Cylinders)
    {
        TraceThreadData *td = GetParserDataPtr();
        sceneData->Max_Bounding_Cylinders = Object->Spline->BCyl->number;
        td->BCyl_Intervals.reserve(4*sceneData->Max_Bounding_Cylinders);
        td->BCyl_RInt.reserve(2*sceneData->Max_Bounding_Cylinders);
        td->BCyl_HInt.reserve(2*sceneData->Max_Bounding_Cylinders);
    }

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Sphere()
{
    Sphere *Object;

    Parse_Begin();

    Object = reinterpret_cast<Sphere *>(Parse_Object_Id());
    if (Object != nullptr)
    {
        return (reinterpret_cast<ObjectPtr>(Object));
    }

    Object = new Sphere();

    Parse_Vector(Object->Center);

    Parse_Comma();

    Object->Radius = Parse_Float();

    Object->Compute_BBox();

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    return(reinterpret_cast<ObjectPtr>(Object));
}



/*****************************************************************************
*
* FUNCTION
*
*       Parse_Sphere_Sweep
*
* INPUT
*
*   -
*
* OUTPUT
*
*   -
*
* RETURNS
*
*   Object
*
* AUTHOR
*
*       Jochen Lippert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

ObjectPtr Parser::Parse_Sphere_Sweep()
{
    SphereSweep *Object;
    int i;

    Parse_Begin();

    Object = reinterpret_cast<SphereSweep *>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    Object = new SphereSweep();

    /* Get type of interpolation */
    EXPECT_ONE
        CASE(LINEAR_SPLINE_TOKEN)
            Object->Interpolation = LINEAR_SPHERE_SWEEP;
        END_CASE
        CASE(CUBIC_SPLINE_TOKEN)
            Object->Interpolation = CATMULL_ROM_SPLINE_SPHERE_SWEEP;
        END_CASE
        CASE(B_SPLINE_TOKEN)
            Object->Interpolation = B_SPLINE_SPHERE_SWEEP;
        END_CASE
        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT

    if (Object->Interpolation == -1)
    {
            Error("Invalid type of interpolation.");
    }

    Parse_Comma();

    /* Get number of modeling spheres */
    Object->Num_Modeling_Spheres = (int)Parse_Float();

    if ((Object->Num_Modeling_Spheres < 2) || (Object->Interpolation != LINEAR_SPHERE_SWEEP && Object->Num_Modeling_Spheres < 4))
        Error("Too few modeling spheres for interpolation type.");

    Object->Modeling_Sphere =
        reinterpret_cast<SPHSWEEP_SPH *>(POV_MALLOC(Object->Num_Modeling_Spheres * sizeof(SPHSWEEP_SPH),
        "sphere sweep modeling spheres"));

    Parse_Comma();

    for (i = 0; i < Object->Num_Modeling_Spheres; i++)
    {
        Parse_Vector(Object->Modeling_Sphere[i].Center);
        Parse_Comma();
        Object->Modeling_Sphere[i].Radius = Parse_Float();

        // NB we allow for a trailing comma at the end of the list,
        // to facilitate auto-generation of lists.
        Parse_Comma();
    }

    if (AllowToken(TOLERANCE_TOKEN))
        Object->Depth_Tolerance = Parse_Float();

    Object->Compute();

    Object->Compute_BBox();

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Superellipsoid
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   ObjectPtr  -
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Read a superellipsoid primitive.
*
* CHANGES
*
*   Oct 1994 : Creation.
*
******************************************************************************/

ObjectPtr Parser::Parse_Superellipsoid()
{
    Vector2d V1;
    Superellipsoid *Object;

    Parse_Begin();

    Object = reinterpret_cast<Superellipsoid *>(Parse_Object_Id());
    if (Object != nullptr)
    {
        return(reinterpret_cast<ObjectPtr>(Object));
    }

    Object = new Superellipsoid();

    Parse_UV_Vect(V1);

    /* The x component is e, the y component is n. */

    Object->Power[X] = 2.0  / V1[X];
    Object->Power[Y] = V1[X] / V1[Y];
    Object->Power[Z] = 2.0  / V1[Y];

    /* Compute bounding box. */

    Object->Compute_BBox();

    /* Parse object's modifiers. */

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    return(reinterpret_cast<ObjectPtr>(Object));
}


/*****************************************************************************
*
* FUNCTION
*
*   Parse_Torus
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   OBJECT
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
******************************************************************************/

ObjectPtr Parser::Parse_Torus()
{
    Torus *Object;
    DBL majorRadius, minorRadius;
    bool invert = false;
    SpindleTorus::SpindleMode spindleMode = SpindleTorus::UnionSpindle;
    bool spindleModeSet = false;

    Parse_Begin();

    Object = reinterpret_cast<Torus *>(Parse_Object_Id());
    if (Object != nullptr)
    {
        return(reinterpret_cast<ObjectPtr>(Object));
    }

    /* Read in the two radii. */

    majorRadius = Parse_Float(); /* Big radius */
    if (majorRadius < 0)
    {
        Warning("Negative torus major radius has the effect of inverting the object; if this is intentional, use the 'inverse' keyword instead.");
        majorRadius = -majorRadius;
        invert = !invert;
    }

    Parse_Comma();

    minorRadius = Parse_Float(); /* Little radius */
    if (minorRadius < 0)
    {
        Warning("Negative torus minor radius has the effect of inverting the object; if this is intentional, use the 'inverse' keyword instead.");
        minorRadius = -minorRadius;
        invert = !invert;
    }

    bool spindleTorus = (majorRadius < minorRadius);

    EXPECT_ONE
        CASE(DIFFERENCE_TOKEN)
            spindleMode = SpindleTorus::DifferenceSpindle;
        END_CASE
        CASE(INTERSECTION_TOKEN)
            spindleMode = SpindleTorus::IntersectionSpindle;
            if (!spindleTorus)
            {
                PossibleError("Intersection spindle mode specified on non-spindle torus.");
                spindleTorus = true;
            }
        END_CASE
        CASE(MERGE_TOKEN)
            spindleMode = SpindleTorus::MergeSpindle;
        END_CASE
        CASE(UNION_TOKEN)
            spindleMode = SpindleTorus::UnionSpindle;
        END_CASE
        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT

    if (spindleTorus)
    {
        SpindleTorus* temp = new SpindleTorus();
        temp->mSpindleMode = spindleMode;
        Object = temp;
    }
    else
    {
        Object = new Torus();
    }

    Object->MajorRadius = majorRadius;
    Object->MinorRadius = minorRadius;

    Object->Compute_BBox();

    Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    if (invert)
        Object = reinterpret_cast<Torus *>(Object->Invert());

    return (reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_Triangle()
{
    Triangle *Object;

    Parse_Begin();

    Object = reinterpret_cast<Triangle *>(Parse_Object_Id());
    if (Object != nullptr)
    {
        return(reinterpret_cast<ObjectPtr>(Object));
    }

    Object = new Triangle();

    Parse_Vector(Object->P1);    Parse_Comma();
    Parse_Vector(Object->P2);    Parse_Comma();
    Parse_Vector(Object->P3);

    /* Note that Compute_Triangle also computes the bounding box. */

    if(!Object->Compute_Triangle())
        Warning("Degenerate triangle. Please remove.");

    Parse_Object_Mods(reinterpret_cast<ObjectPtr>(Object));

    return(reinterpret_cast<ObjectPtr>(Object));
}

//******************************************************************************

ObjectPtr Parser::Parse_TrueType ()
{
    ObjectPtr Object;
    char *filename = nullptr;
    UCS2 *text_string;
    DBL depth;
    Vector3d offset;
    int builtin_font = 0;
    TRANSFORM Local_Trans;
    POV_UINT32 cmap;
    CharsetID charset;
    LegacyCharset legacyCharset;

#if 0
    if((sceneData->EffectiveLanguageVersion() < 350) && ((sceneData->legacyCharset == LegacyCharset::kUnspecified) ||
                                                         (sceneData->legacyCharset == LegacyCharset::kASCII)))
    {
        PossibleError("Text may not be displayed as expected.\n"
                      "Please refer to the user manual regarding changes\n"
                      "in POV-Ray v3.5 and later.");
    }
#endif

    if (sceneData->EffectiveLanguageVersion() < 380)
    {
        if (sceneData->legacyCharset == LegacyCharset::kUnspecified)
            legacyCharset = LegacyCharset::kASCII;
        else
            legacyCharset = sceneData->legacyCharset;

        PossibleError("Text may not be displayed as expected.\n"
                      "Please refer to the user manual regarding changes "
                      "in POV-Ray v3.8 and later.");
    }
    else
        legacyCharset = LegacyCharset::kUnspecified;

    Parse_Begin ();

    Object = reinterpret_cast<ObjectPtr>(Parse_Object_Id());
    if (Object != nullptr)
        return (reinterpret_cast<ObjectPtr>(Object));

    EXPECT_ONE
        CASE(TTF_TOKEN)
            filename = Parse_C_String(true);
        END_CASE
        CASE(INTERNAL_TOKEN)
            builtin_font = (int)Parse_Float();
        END_CASE
        OTHERWISE
            Expectation_Error ("ttf or internal");
        END_CASE
    END_EXPECT

    cmap = TrueTypeFont::kAnyCMAP;
    charset = CharsetID::kUndefined;
    if (AllowToken(CMAP_TOKEN))
    {
        Warning("Text primitive 'cmap' extension is experimental and may be "
                "subject to future changes.");
        Parse_Begin();
        cmap =  POV_UINT16(Parse_Float()) << 16;
        Parse_Comma();
        cmap += POV_UINT16(Parse_Float());
        charset = CharsetID::kUCS4;
        if (AllowToken(CHARSET_TOKEN))
        {
            charset = CharsetID(POV_UINT16(Parse_Float()));
            if (Charset::Get(charset) == nullptr)
                Error("Unknown character set %i", int(charset));
        }
        Parse_End();
    }

    /*** Object = Create_TTF(); */
    Parse_Comma();

    /* Parse the text string to be rendered */
    text_string = Parse_String();
    Parse_Comma();

    /* Get the extrusion depth */
    depth = Parse_Float(); Parse_Comma ();

    /* Get the offset vector */
    Parse_Vector(offset);

    /* Open the font file */
    TrueTypeFont* font = OpenFontFile(filename, builtin_font, cmap, charset, legacyCharset);

    /* Process all this good info */
    Object = new CSGUnion();
    TrueType::ProcessNewTTF(reinterpret_cast<CSG *>(Object), font, text_string, depth, offset);
    if (filename)
    {
        /* Free up the filename  */
        POV_FREE (filename);
    }

    /* Free up the text string memory */
    POV_FREE (text_string);

    /**** Compute_TTF_BBox(Object); */
    Object->Compute_BBox();

    /* This tiny rotation should fix cracks in text that lies along an axis */
    offset = Vector3d(0.001, 0.001, 0.001); // TODO - try to find a different solution to this hack
    Compute_Rotation_Transform(&Local_Trans, offset);
    Rotate_Object (reinterpret_cast<ObjectPtr>(Object), offset, &Local_Trans);

    /* Get any rotate/translate or texturing stuff */
    Object = Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));

    return (reinterpret_cast<ObjectPtr>(Object));
}


/*****************************************************************************
*
* FUNCTION
*
*   OpenFontFile
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Added support for builtin fonts - Oct 2012 [JG]
*
******************************************************************************/
TrueTypeFont *Parser::OpenFontFile(const char *asciifn, const int font_id, POV_UINT32 cmap, CharsetID charset,
                                   LegacyCharset legacyCharset)
{
    TrueTypeFont *font = nullptr;
    UCS2String ign;
    UCS2String formalFilename;

    if (asciifn != nullptr)
    {
        formalFilename = SysToUCS2String(asciifn);

        // First look to see if we have already opened this font (with the same character mapping).

        for (vector<TrueTypeFont*>::iterator iFont = sceneData->TTFonts.begin(); iFont != sceneData->TTFonts.end(); ++iFont)
        {
            if ((formalFilename == (*iFont)->filename) && (cmap == (*iFont)->cmap) && (charset == (*iFont)->charset) &&
                (legacyCharset == (*iFont)->legacyCharset))
            {
                font = *iFont;
                break;
            }
        }
    }
    if (font != nullptr)
    {
        if (font->file == nullptr)
        {
            /* We have a match, use the previous information */
            font->file = Locate_File(font->filename,POV_File_Font_TTF,ign,true);
            if (font->file == nullptr)
            {
                throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot open font file.");
            }
        }
        else
        {
            #ifdef TTF_DEBUG
            Debug_Info("Using cached font info for %s\n", font->filename.c_str());
            #endif
        }
    }
    else
    {
        /*
         * We haven't looked at this font before, let's allocate a holder for the
         * information and set some defaults
         */

        shared_ptr<IStream> file;

        if (asciifn)
        {
            if ((file = Locate_File(formalFilename,POV_File_Font_TTF,ign,true)) == nullptr)
            {
                throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot open font file.");
            }
        }
        else
        {
            file = shared_ptr<IStream>(Internal_Font_File(font_id));
        }

        font = new TrueTypeFont(formalFilename, file, cmap, charset, legacyCharset);

        sceneData->TTFonts.push_back(font);
    }

    return font;
}

//******************************************************************************

ObjectPtr Parser::Parse_Object ()
{
    ObjectPtr Object = nullptr;

    EXPECT_ONE

        CASE (ISOSURFACE_TOKEN)
            Object = Parse_Isosurface ();
        END_CASE

        CASE (PARAMETRIC_TOKEN)
            Object = Parse_Parametric ();
        END_CASE

        CASE (JULIA_FRACTAL_TOKEN)
            Object = Parse_Julia_Fractal ();
        END_CASE

        CASE (SPHERE_TOKEN)
            Object = Parse_Sphere ();
        END_CASE

        CASE (SPHERE_SWEEP_TOKEN)
            Object = Parse_Sphere_Sweep ();
        END_CASE

        CASE (PLANE_TOKEN)
            Object = Parse_Plane ();
        END_CASE

        CASE (CONE_TOKEN)
            Object = Parse_Cone ();
        END_CASE

        CASE (CYLINDER_TOKEN)
            Object = Parse_Cylinder ();
        END_CASE

        CASE (DISC_TOKEN)
            Object = Parse_Disc ();
        END_CASE

        CASE (QUADRIC_TOKEN)
            Object = Parse_Quadric ();
        END_CASE

        CASE (CUBIC_TOKEN)
            Object = Parse_Poly (3);
        END_CASE

        CASE (QUARTIC_TOKEN)
            Object = Parse_Poly (4);
        END_CASE

        CASE (POLY_TOKEN)
            Object = Parse_Poly (0);
        END_CASE

        CASE (POLYNOMIAL_TOKEN)
            Object = Parse_Polynom();
        END_CASE

        CASE (OVUS_TOKEN)
            Object = Parse_Ovus();
        END_CASE

        CASE (TORUS_TOKEN)
            Object = Parse_Torus ();
        END_CASE

        /* Parse lathe primitive. [DB 8/94] */

        CASE (LATHE_TOKEN)
            Object = Parse_Lathe();
        END_CASE

        CASE (LEMON_TOKEN)
            Object = Parse_Lemon();
        END_CASE

        /* Parse polygon primitive. [DB 8/94] */

        CASE (POLYGON_TOKEN)
            Object = Parse_Polygon();
        END_CASE

        /* Parse prism primitive. [DB 8/94] */

        CASE (PRISM_TOKEN)
            Object = Parse_Prism();
        END_CASE

        /* Parse surface of revolution primitive. [DB 8/94] */

        CASE (SOR_TOKEN)
            Object = Parse_Sor();
        END_CASE

        /* Parse superellipsoid primitive. [DB 11/94] */

        CASE (SUPERELLIPSOID_TOKEN)
            Object = Parse_Superellipsoid();
        END_CASE

        /* Parse triangle mesh primitive. [DB 2/95] */

        CASE (MESH_TOKEN)
            Object = Parse_Mesh();
        END_CASE

        /* NK 1998 Parse triangle mesh primitive - syntax version 2. */
        CASE (MESH2_TOKEN)
            Object = Parse_Mesh2();
        END_CASE
        /* NK ---- */

        CASE (TEXT_TOKEN)
            Object = Parse_TrueType ();
        END_CASE

        CASE (OBJECT_ID_TOKEN)
            Object = Copy_Object(CurrentTokenDataPtr<ObjectPtr>());
        END_CASE

        CASE (UNION_TOKEN)
            Object = Parse_CSG (CSG_UNION_TYPE);
        END_CASE

        CASE (LIGHT_GROUP_TOKEN)
            Object = Parse_Light_Group ();
        END_CASE

        CASE (COMPOSITE_TOKEN)
            VersionWarning(150, "Use union instead of composite.");
            Object = Parse_CSG (CSG_UNION_TYPE);
        END_CASE

        CASE (MERGE_TOKEN)
            Object = Parse_CSG (CSG_MERGE_TYPE);
        END_CASE

        CASE (INTERSECTION_TOKEN)
            Object = Parse_CSG (CSG_INTERSECTION_TYPE);
        END_CASE

        CASE (DIFFERENCE_TOKEN)
            Object = Parse_CSG (CSG_DIFFERENCE_TYPE+CSG_INTERSECTION_TYPE);
        END_CASE

        CASE (BICUBIC_PATCH_TOKEN)
            Object = Parse_Bicubic_Patch ();
        END_CASE

        CASE (TRIANGLE_TOKEN)
            Object = Parse_Triangle ();
        END_CASE

        CASE (SMOOTH_TRIANGLE_TOKEN)
            Object = Parse_Smooth_Triangle ();
        END_CASE

        CASE (HEIGHT_FIELD_TOKEN)
            Object = Parse_HField ();
        END_CASE

        CASE (BOX_TOKEN)
            Object = Parse_Box ();
        END_CASE

        CASE (BLOB_TOKEN)
            Object = Parse_Blob ();
        END_CASE

        CASE (LIGHT_SOURCE_TOKEN)
            Object = Parse_Light_Source ();
        END_CASE

        CASE (OBJECT_TOKEN)
            Parse_Begin ();
            Object = Parse_Object ();
            if (!Object)
                Expectation_Error ("object");
            Object = Parse_Object_Mods (reinterpret_cast<ObjectPtr>(Object));
        END_CASE

        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT

    if (Object && !Object->Precompute())
        PossibleError("Inconsistent object parameters.");

    return Object;
}

//******************************************************************************

void Parser::Parse_Default ()
{
    TEXTURE *Local_Texture;
    PIGMENT *Local_Pigment;
    TNORMAL *Local_Tnormal;
    FINISH  *Local_Finish;

    Not_In_Default = false;
    Parse_Begin();

    defaultsModified = true;

    EXPECT
        CASE (TEXTURE_TOKEN)
            Local_Texture = Default_Texture;
            Parse_Begin ();
            Default_Texture = Parse_Texture();
            Parse_End ();
            if (Default_Texture->Type != PLAIN_PATTERN)
                Error("Default texture cannot be material map or tiles.");
            if (Default_Texture->Next != nullptr)
                Error("Default texture cannot be layered.");
            Destroy_Textures(Local_Texture);
        END_CASE

        CASE (PIGMENT_TOKEN)
            Local_Pigment = Copy_Pigment((Default_Texture->Pigment));
            Parse_Begin ();
            Parse_Pigment (&Local_Pigment);
            Parse_End ();
            Destroy_Pigment(Default_Texture->Pigment);
            Default_Texture->Pigment = Local_Pigment;
        END_CASE

        CASE (NORMAL_TOKEN)
            Local_Tnormal = Copy_Tnormal((Default_Texture->Tnormal));
            Parse_Begin ();
            Parse_Tnormal (&Local_Tnormal);
            Parse_End ();
            Destroy_Tnormal(Default_Texture->Tnormal);
            Default_Texture->Tnormal = Local_Tnormal;
        END_CASE

        CASE (FINISH_TOKEN)
            Local_Finish = Copy_Finish((Default_Texture->Finish));
            Parse_Finish (&Local_Finish);
            if (Default_Texture->Finish)
                delete Default_Texture->Finish;
            Default_Texture->Finish = Local_Finish;
        END_CASE

        CASE (RADIOSITY_TOKEN)
            Parse_Begin ();
            EXPECT
                CASE (IMPORTANCE_TOKEN)
                    sceneData->radiositySettings.defaultImportance = Parse_Float ();
                    if ( (sceneData->radiositySettings.defaultImportance <= 0.0) ||
                         (sceneData->radiositySettings.defaultImportance >  1.0) )
                        Error("Radiosity importance must be greater than 0.0 and at most 1.0.");
                END_CASE
                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            Parse_End ();
        END_CASE

        CASE (CAMERA_TOKEN)
            Parse_Camera (Default_Camera);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End();

    Not_In_Default = true;
}

//******************************************************************************

void Parser::Parse_Frame ()
{
    ObjectPtr Object;
    RAINBOW  *Local_Rainbow;
    FOG  *Local_Fog;
    SKYSPHERE  *Local_Skysphere;
    bool had_camera = false;

    EXPECT
        CASE (RAINBOW_TOKEN)
            Local_Rainbow = Parse_Rainbow();
            Local_Rainbow->Next = sceneData->rainbow;
            sceneData->rainbow = Local_Rainbow;
        END_CASE

        CASE (SKYSPHERE_TOKEN)
            Local_Skysphere = Parse_Skysphere();
            if (sceneData->skysphere != nullptr)
            {
                Warning("Only one sky-sphere allowed (last one will be used).");
                Destroy_Skysphere(sceneData->skysphere);
            }
            sceneData->skysphere = Local_Skysphere;
            for (vector<PIGMENT*>::iterator i = Local_Skysphere->Pigments.begin(); i != Local_Skysphere->Pigments.end(); ++ i)
            {
                Post_Pigment(*i);
            }
        END_CASE

        CASE (FOG_TOKEN)
            Local_Fog = Parse_Fog();
            Local_Fog->Next = sceneData->fog;
            sceneData->fog = Local_Fog;
        END_CASE

        CASE (MEDIA_TOKEN)
            Parse_Media(sceneData->atmosphere);
        END_CASE

        CASE (BACKGROUND_TOKEN)
            Parse_Begin();
            Parse_Colour (sceneData->backgroundColour);
            if (sceneData->EffectiveLanguageVersion() < 370)
            {
                if (sceneData->outputAlpha)
                    sceneData->backgroundColour.SetFT(0.0f, 1.0f);
                else
                    sceneData->backgroundColour.SetFT(0.0f, 0.0f);
            }
            else
            {
                if (!sceneData->outputAlpha)
                {
                    // if we're not outputting an alpha channel, precompose the scene background against a black "background behind the background"
                    sceneData->backgroundColour.colour() *= sceneData->backgroundColour.Opacity();
                    sceneData->backgroundColour.SetFT(0.0f, 0.0f);
                }
            }
            Parse_End();
        END_CASE

        CASE (CAMERA_TOKEN)
            if (sceneData->EffectiveLanguageVersion() >= 350)
            {
                if (sceneData->clocklessAnimation == false)
                {
                    if (had_camera == true)
                        Warning("More than one camera in scene. Ignoring previous camera(s).");
                }
                had_camera = true;
                sceneData->parsedCamera = Default_Camera;
            }

            Parse_Camera(sceneData->parsedCamera);
            if (sceneData->clocklessAnimation == true)
                sceneData->cameras.push_back(sceneData->parsedCamera);
        END_CASE

        CASE (DECLARE_TOKEN)
            VersionWarning(295,"Should have '#' before 'declare'.");
            POV_EXPERIMENTAL_ASSERT(IsOkToDeclare());
            Parse_Declare(false, false);
        END_CASE

        CASE (INCLUDE_TOKEN)
            VersionWarning(295,"Should have '#' before 'include'.");
            POV_EXPERIMENTAL_ASSERT(IsOkToDeclare());
            Open_Include();
        END_CASE

        CASE (VERSION_TOKEN)
            VersionWarning(295,"Should have '#' before 'version'.");
            POV_EXPERIMENTAL_ASSERT(IsOkToDeclare());
            Parse_Version();
        END_CASE

        CASE (MAX_TRACE_LEVEL_TOKEN)
            if (sceneData->EffectiveLanguageVersion() >= 350)
            {
                PossibleError("'max_trace_level' should be in global_settings block.\n"
                                "Future versions may not support 'max_trace_level' outside global_settings.");
            }
            Global_Setting_Warn();
            Max_Trace_Level = (int)Parse_Float();
            Max_Trace_Level = max(1, Max_Trace_Level);
            Had_Max_Trace_Level = true;
            if(Max_Trace_Level > MAX_TRACE_LEVEL_LIMIT)
            {
                Warning("Maximum max_trace_level is %d but %d was specified.\n"
                            "Going to use max_trace_level %d.",
                            MAX_TRACE_LEVEL_LIMIT, Max_Trace_Level, MAX_TRACE_LEVEL_LIMIT);
                Max_Trace_Level = MAX_TRACE_LEVEL_LIMIT;
            }
        END_CASE

        CASE (MAX_INTERSECTIONS_TOKEN)
            Parse_Float();
            VersionWarning(370, "'max_intersections' is no longer needed and has no effect in POV-Ray v3.7 or later.");
        END_CASE

        CASE (DEFAULT_TOKEN)
            Parse_Default();
        END_CASE

        CASE (END_OF_FILE_TOKEN)
            EXIT
        END_CASE

        CASE (GLOBAL_SETTINGS_TOKEN)
            Parse_Global_Settings();
        END_CASE

        OTHERWISE
            UNGET
            Object = Parse_Object();
            if (Object == nullptr)
                Expectation_Error ("object or directive");
            Post_Process (Object, nullptr);
            Link_To_Frame (Object);
        END_CASE
    END_EXPECT
}

//******************************************************************************

void Parser::Parse_Global_Settings()
{
    Parse_Begin();
    EXPECT
        CASE (IRID_WAVELENGTH_TOKEN)
            Parse_Wavelengths (sceneData->iridWavelengths);
        END_CASE

        CASE (CHARSET_TOKEN)
            Warning("Encountered 'charset' global setting. As of POV-Ray v3.8, this mechanism "
                    "of specifying character encoding is no longer supported, and future versions "
                    "may treat the presence of the setting as an error.");
            EXPECT_ONE
                CASE (ASCII_TOKEN)
                    sceneData->legacyCharset = LegacyCharset::kASCII;
                    if ((sceneData->EffectiveLanguageVersion() >= 350) &&
                        (sceneData->EffectiveLanguageVersion() <  380))
                    {
                        Warning("Encountered 'charset ascii' in a v3.5 or later legacy scene. "
                                "Behaviour with respect to non-ASCII characters (either encoded as "
                                "extended ASCII or using '\\uXXXX' notation) differs from previous "
                                "versions.");
                    }
                END_CASE
                CASE (UTF8_TOKEN)
                    sceneData->legacyCharset = LegacyCharset::kUTF8;
                END_CASE
                CASE (SYS_TOKEN)
                    sceneData->legacyCharset = LegacyCharset::kSystem;
                    Warning("Encountered 'charset sys'. Behaviour with respect to non-ASCII "
                            "characters (either encoded as extended ASCII or using '\\uXXXX' "
                            "notation) differs from previous versions.");
                END_CASE
                OTHERWISE
                    Expectation_Error ("charset type");
                END_CASE
            END_EXPECT
        END_CASE

        CASE (ASSUMED_GAMMA_TOKEN)
        {
            switch (sceneData->gammaMode)
            {
                case kPOVList_GammaMode_AssumedGamma36:
                case kPOVList_GammaMode_AssumedGamma37:
                    // they have explicitly set assumed_gamma, so we
                    // don't do any of the compatibility checks we normally do. issue a
                    // warning and continue on our way.
                    Warning("New instance of assumed_gamma ignored");
                    Parse_Gamma();
                    break;
                default:
                    if (sceneData->EffectiveLanguageVersion() < 370)
                        sceneData->gammaMode = kPOVList_GammaMode_AssumedGamma36;
                    else
                        sceneData->gammaMode = kPOVList_GammaMode_AssumedGamma37;
                    sceneData->workingGamma = Parse_Gamma();
                    sceneData->workingGammaToSRGB = TranscodingGammaCurve::Get(sceneData->workingGamma, SRGBGammaCurve::Get());
                    break;
            }
        }
        END_CASE

        CASE (MAX_TRACE_LEVEL_TOKEN)
        {
            int Trace_Level = (int)Parse_Float();
            Max_Trace_Level = max(1, Trace_Level);
            Had_Max_Trace_Level = true;
            if(Max_Trace_Level > MAX_TRACE_LEVEL_LIMIT)
            {
                Warning("Maximum max_trace_level is %d but %d was specified.\n"
                        "Going to use max_trace_level %d.",
                        MAX_TRACE_LEVEL_LIMIT, Max_Trace_Level, MAX_TRACE_LEVEL_LIMIT);
                Max_Trace_Level = MAX_TRACE_LEVEL_LIMIT;
            }
        }
        END_CASE

        CASE (ADC_BAILOUT_TOKEN)
            sceneData->parsedAdcBailout = Parse_Float ();
        END_CASE

        CASE (NUMBER_OF_WAVES_TOKEN)
            {
                int numberOfWaves = (int) Parse_Float ();
                if(numberOfWaves <=0)
                {
                    Warning("Illegal Value: number_of_waves must be greater than 0.\nChanged to 1.");
                    numberOfWaves = 1;
                }
                sceneData->numberOfWaves = numberOfWaves;
            }
        END_CASE

        CASE (MAX_INTERSECTIONS_TOKEN)
            Parse_Float();
            VersionWarning(370, "'max_intersections' is no longer needed and has no effect in POV-Ray v3.7 or later.");
        END_CASE

        CASE (NOISE_GENERATOR_TOKEN)
            sceneData->noiseGenerator = (int) Parse_Float();
            if (sceneData->noiseGenerator < kNoiseGen_Min || sceneData->noiseGenerator > kNoiseGen_Max)
                Error ("Value for noise_generator in global_settings must be between %i (inclusive) and %i (inclusive).", kNoiseGen_Min, kNoiseGen_Max);
            sceneData->explicitNoiseGenerator = true;
        END_CASE

        CASE (AMBIENT_LIGHT_TOKEN)
            Parse_Colour (sceneData->ambientLight);
        END_CASE

        CASE (PHOTONS_TOKEN)
            // TODO FIXME PHOTONS
            sceneData->photonSettings.minGatherCount = 20;
            sceneData->photonSettings.maxGatherCount = 100;
            sceneData->photonSettings.adcBailout = -1;  // use the normal adc bailout
            sceneData->photonSettings.Max_Trace_Level = -1; // use the normal max_trace_level

            sceneData->photonSettings.jitter = 0.4;
            sceneData->photonSettings.autoStopPercent = 0.5;

            sceneData->photonSettings.expandTolerance = 0.2;
            sceneData->photonSettings.minExpandCount = 35;

            sceneData->photonSettings.fileName.clear();
            sceneData->photonSettings.loadFile = false;

            sceneData->photonSettings.surfaceSeparation = 1.0;
            sceneData->photonSettings.globalSeparation = 1.0;

            sceneData->photonSettings.maxMediaSteps = 0;  // disable media photons by default
            sceneData->photonSettings.mediaSpacingFactor = 1.0;

            //  sceneData->photonSettings.photonReflectionBlur = false; // off by default

            sceneData->photonSettings.surfaceCount = 0;
            //  sceneData->photonSettings.globalCount = 0;

            sceneData->surfacePhotonMap.minGatherRad = -1;

            Parse_Begin();
            EXPECT
                CASE(RADIUS_TOKEN)
                    sceneData->surfacePhotonMap.minGatherRad = Allow_Float(-1.0);
                    Parse_Comma();
                    sceneData->surfacePhotonMap.minGatherRadMult = Allow_Float(1.0);
                    Parse_Comma();
                    sceneData->mediaPhotonMap.minGatherRad = Allow_Float(-1.0);
                    Parse_Comma();
                    sceneData->mediaPhotonMap.minGatherRadMult = Allow_Float(1.0);
                END_CASE

                CASE(SPACING_TOKEN)
                    sceneData->photonSettings.surfaceSeparation = Parse_Float();
                END_CASE

#ifdef GLOBAL_PHOTONS
                // TODO -- if we ever revive this, we need to choose a different keyword for this
                CASE(GLOBAL_TOKEN)
                    sceneData->photonSettings.globalCount = (int)Parse_Float();
                END_CASE
#endif

                CASE (EXPAND_THRESHOLDS_TOKEN)
                    sceneData->photonSettings.expandTolerance = Parse_Float(); Parse_Comma();
                    sceneData->photonSettings.minExpandCount = Parse_Float();
                    if (sceneData->photonSettings.expandTolerance < 0.0)
                    {
                        VersionWarning(100,"The first parameter of expand_thresholds must be greater than or equal to 0.\nSetting it to 0 now.");
                        sceneData->photonSettings.expandTolerance = 0.0;
                    }
                    if (sceneData->photonSettings.minExpandCount < 0)
                    {
                        VersionWarning(100,"The second parameter of expand_thresholds must be greater than or equal to 0.\nSetting it to 0 now.");
                        sceneData->photonSettings.minExpandCount = 0;
                    }
                END_CASE

                CASE (GATHER_TOKEN)
                    sceneData->photonSettings.minGatherCount = (int)Parse_Float();
                    Parse_Comma();
                    sceneData->photonSettings.maxGatherCount = (int)Parse_Float();
                END_CASE

                CASE (JITTER_TOKEN)
                    sceneData->photonSettings.jitter = Parse_Float();
                END_CASE

                CASE (COUNT_TOKEN)
                    sceneData->photonSettings.surfaceCount = (int)Parse_Float();
                END_CASE

                CASE (AUTOSTOP_TOKEN)
                    sceneData->photonSettings.autoStopPercent = Parse_Float();
                END_CASE

                CASE (ADC_BAILOUT_TOKEN)
                    sceneData->photonSettings.adcBailout = Parse_Float ();
                END_CASE

                CASE (MAX_TRACE_LEVEL_TOKEN)
                    sceneData->photonSettings.Max_Trace_Level = Parse_Float();
                END_CASE

                CASE(LOAD_FILE_TOKEN)
                    if (!sceneData->photonSettings.fileName.empty())
                    {
                        if(sceneData->photonSettings.loadFile)
                            VersionWarning(100,"Filename already given, using new name");
                        else
                            VersionWarning(100,"Cannot both load and save photon map. Now switching to load mode.");
                    }
                    sceneData->photonSettings.fileName = Parse_SysString(true);
                    sceneData->photonSettings.loadFile = true;
                END_CASE

                CASE(SAVE_FILE_TOKEN)
                    if (!sceneData->photonSettings.fileName.empty())
                    {
                        if(!sceneData->photonSettings.loadFile)
                            VersionWarning(100,"Filename already given, using new name");
                        else
                            VersionWarning(100,"Cannot both load and save photon map. Now switching to save mode.");
                    }
                    sceneData->photonSettings.fileName = Parse_SysString(true);
                    sceneData->photonSettings.loadFile = false;
                END_CASE

                CASE(MEDIA_TOKEN)
                    sceneData->photonSettings.maxMediaSteps = (int)Parse_Float(); Parse_Comma();
                    if (sceneData->photonSettings.maxMediaSteps<0)
                        Error("max media steps must be non-negative.");

                    sceneData->photonSettings.mediaSpacingFactor = Allow_Float(1.0);
                    if (sceneData->photonSettings.mediaSpacingFactor <= 0.0)
                        Error("media spacing factor must be greater than zero.");
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT

            // max_gather_count = 0  means no photon maps
            if (sceneData->photonSettings.maxGatherCount > 0)
                sceneData->photonSettings.photonsEnabled = true;
            else
                sceneData->photonSettings.photonsEnabled = false;

            if(sceneData->photonSettings.photonsEnabled)
            {
                // check for range errors
                if (sceneData->photonSettings.minGatherCount < 0)
                    Error("min_gather_count cannot be negative.");
                if (sceneData->photonSettings.maxGatherCount < 0)
                    Error("max_gather_count cannot be negative.");
                if (sceneData->photonSettings.minGatherCount > sceneData->photonSettings.maxGatherCount)
                    Error("min_gather_count must be less than max_gather_count.");
            }

            Parse_End();

            if(sceneData->photonSettings.photonsEnabled == false)
            {
                sceneData->photonSettings.photonsEnabled = false;
                Warning("A photons{}-block has been found but photons remain disabled because\n"
                        "the output quality is set to 8 or less.");
            }
        END_CASE


        CASE (RADIOSITY_TOKEN)
            // enable radiosity only if the user includes a "radiosity" token
            if((sceneData->radiositySettings.radiosityEnabled == false) && (sceneData->EffectiveLanguageVersion() < 350))
            {
                Warning("In POV-Ray v3.5 and later a radiosity{}-block will automatically\n"
                        "turn on radiosity if the output quality is set to 9 or higher.\n"
                        "Read the documentation to find out more about radiosity changes!");
            }
            sceneData->radiositySettings.radiosityEnabled = true;

            Parse_Begin();
            EXPECT
                CASE(LOAD_FILE_TOKEN)
                    PossibleError("In POV-Ray v3.7 and later 'load_file' has to be specified as\n"
                                  "an option on the command-line, in an INI file or in a dialog\n"
                                  "(on some platforms). The 'load_file' setting here will be ignored!\n"
                                  "Read the documentation to find out more about radiosity changes!");
                    {
                        char *tstr = Parse_C_String(true);
                        POV_FREE(tstr);
                    }
                END_CASE

                CASE(SAVE_FILE_TOKEN)
                    PossibleError("In POV-Ray v3.7 and later 'save_file' has to be specified as\n"
                                  "an option on the command-line, in an INI file or in a dialog\n"
                                  "(on some platforms). The 'save_file' setting here will be ignored!\n"
                                  "Read the documentation to find out more about radiosity changes!");
                    {
                        char *tstr = Parse_C_String(true);
                        POV_FREE(tstr);
                    }
                END_CASE

                CASE(ALWAYS_SAMPLE_TOKEN)
                    sceneData->radiositySettings.alwaysSample = ((int)Parse_Float() != 0);
                END_CASE

                CASE (PRETRACE_START_TOKEN)
                    sceneData->radiositySettings.pretraceStart = Parse_Float();
                    if ((sceneData->radiositySettings.pretraceStart <= 0.0) ||
                        (sceneData->radiositySettings.pretraceStart >  1.0))
                    {
                        Error("Radiosity pretrace start must be greater than 0 and no higher than 1.");
                    }
                END_CASE

                CASE (PRETRACE_END_TOKEN)
                    sceneData->radiositySettings.pretraceEnd = Parse_Float();
                    if ((sceneData->radiositySettings.pretraceEnd <= 0.0) ||
                        (sceneData->radiositySettings.pretraceEnd >  1.0))
                    {
                        Error("Radiosity pretrace end must be greater than 0 and no higher than 1.");
                    }
                END_CASE

                CASE (BRIGHTNESS_TOKEN)
                    if ((sceneData->radiositySettings.brightness = Parse_Float()) <= 0.0)
                    {
                        Error("Radiosity brightness must be a positive number.");
                    }
                END_CASE

                CASE (COUNT_TOKEN)
                    if (( sceneData->radiositySettings.count = (int)Parse_Float()) <= 0)
                    {
                        Error("Radiosity count must be a positive number.");
                    }
                    Parse_Comma();
                    sceneData->radiositySettings.directionPoolSize = (int)Allow_Float(max((long)RADIOSITY_MAX_SAMPLE_DIRECTIONS,sceneData->radiositySettings.count));
                    if (sceneData->radiositySettings.directionPoolSize < sceneData->radiositySettings.count)
                    {
                        Error("Radiosity count can not be more than direction pool size (count 2nd value).");
                    }
                END_CASE

                CASE (ERROR_BOUND_TOKEN)
                    if (( sceneData->radiositySettings.errorBound = Parse_Float()) <= 0.0)
                    {
                        Error("Radiosity error bound must be a positive number.");
                    }
                END_CASE

                CASE (GRAY_THRESHOLD_TOKEN)
                    sceneData->radiositySettings.grayThreshold = Parse_Float();
                    if (( sceneData->radiositySettings.grayThreshold < 0.0) || ( sceneData->radiositySettings.grayThreshold > 1.0))
                    {
                        Error("Radiosity gray threshold must be from 0.0 to 1.0.");
                    }
                END_CASE

                CASE (LOW_ERROR_FACTOR_TOKEN)
                    if (( sceneData->radiositySettings.lowErrorFactor = Parse_Float()) <= 0.0)
                    {
                        Error("Radiosity low error factor must be a positive number.");
                    }
                END_CASE

                CASE (MAXIMUM_REUSE_TOKEN)
                    sceneData->radiositySettings.maximumReuseSet = true;
                    if (( sceneData->radiositySettings.maximumReuse = Parse_Float()) <= 0.0)
                    {
                        Error("Radiosity maximum reuse must be a positive number.");
                    }
                END_CASE

                CASE (MINIMUM_REUSE_TOKEN)
                    sceneData->radiositySettings.minimumReuseSet = true;
                    if (( sceneData->radiositySettings.minimumReuse = Parse_Float()) < 0.0)
                    {
                        Error("Radiosity minimum reuse can not be a negative number.");
                    }
                END_CASE

                CASE (NEAREST_COUNT_TOKEN)
                    sceneData->radiositySettings.nearestCount = (int)Parse_Float();
                    if (( sceneData->radiositySettings.nearestCount < 1) ||
                        ( sceneData->radiositySettings.nearestCount > RadiosityFunction::MAX_NEAREST_COUNT))
                    {
                        Error("Radiosity nearest count must be a value from 1 to %d.", RadiosityFunction::MAX_NEAREST_COUNT);
                    }
                    Parse_Comma();
                    sceneData->radiositySettings.nearestCountAPT = (int)Allow_Float(0.0);
                    if (( sceneData->radiositySettings.nearestCountAPT < 0) ||
                        ( sceneData->radiositySettings.nearestCountAPT >= sceneData->radiositySettings.nearestCount))
                    {
                        Error("Radiosity nearest count for adaptive pretrace must be non-negative and smaller than general nearest count.");
                    }
                END_CASE

                CASE (RECURSION_LIMIT_TOKEN)
                    sceneData->radiositySettings.recursionLimit = (int)Parse_Float();
                    if ((sceneData->radiositySettings.recursionLimit < 1) || (sceneData->radiositySettings.recursionLimit > RadiosityFunction::DEPTH_MAX))
                    {
                        Error("Radiosity recursion limit must be in the range 1 to %d.", RadiosityFunction::DEPTH_MAX);
                    }
                END_CASE

                CASE (MAX_SAMPLE_TOKEN)
                    sceneData->radiositySettings.maxSample = Parse_Float();
                END_CASE

                CASE (ADC_BAILOUT_TOKEN)
                    if (( sceneData->radiositySettings.adcBailout = Parse_Float()) <= 0)
                    {
                        Error("ADC Bailout must be a positive number.");
                    }
                END_CASE

                CASE (NORMAL_TOKEN)
                    sceneData->radiositySettings.normal = ((int)Parse_Float() != 0);
                END_CASE

                CASE (MEDIA_TOKEN)
                    sceneData->radiositySettings.media = ((int)Parse_Float() != 0);
                END_CASE

                CASE (SUBSURFACE_TOKEN)
                    sceneData->radiositySettings.subsurface = ((int)Parse_Float() != 0);
                END_CASE

                CASE (BRILLIANCE_TOKEN)
                    sceneData->radiositySettings.brilliance = ((int)Parse_Float() != 0);
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            Parse_End();
        END_CASE
        CASE (HF_GRAY_16_TOKEN)
            Allow_Float(1.0);
            PossibleError("In POV-Ray v3.7 and later 'hf_gray_16' has to be specified as\n"
                          "an option on the command-line (e.g. '+FNg'), in an INI file\n"
                          "(e.g. 'Grayscale_Output=true'), or (on some platforms) in a dialog.\n"
                          "The 'hf_gray_16' setting here is ignored. See the documentation\n"
                          "for more details.");
        END_CASE

        CASE (MM_PER_UNIT_TOKEN)
            sceneData->mmPerUnit = Parse_Float ();
        END_CASE

        CASE (SUBSURFACE_TOKEN)
            sceneData->useSubsurface = true;
            Parse_Begin();
            EXPECT
                CASE(SAMPLES_TOKEN)
                    sceneData->subsurfaceSamplesDiffuse = (int)Parse_Float(); Parse_Comma();
                    sceneData->subsurfaceSamplesSingle = (int)Parse_Float();
                END_CASE

                CASE (RADIOSITY_TOKEN)
                    sceneData->subsurfaceUseRadiosity = ((int)Parse_Float() != 0);
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
            EXIT
        END_CASE
    END_EXPECT
    Parse_End();
}

//******************************************************************************

// be aware that this method may change the address of Object
// (this will only happen if Object is CSG and the invert_object keyword is parsed)
ObjectPtr Parser::Parse_Object_Mods (ObjectPtr Object)
{
    DBL V1, V2;
    Vector3d Min, Max;
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    BoundingBox BBox;
    TEXTURE *Local_Texture;
    TEXTURE *Local_Int_Texture;
    MATERIAL Local_Material;
    TransColour Local_Colour;
    char *s;

    EXPECT_CAT
        CASE(UV_MAPPING_TOKEN)
            /* if no texture than allow uv_mapping
               otherwise, warn user */
            if (Object->Texture == nullptr)
            {
                Set_Flag(Object, UV_FLAG);
            }
            else
            {
                Error ("uv_mapping must be specified before texture.");
            }
        END_CASE

        CASE(SPLIT_UNION_TOKEN)
            if (dynamic_cast<CSGUnion *>(Object) == nullptr) // FIXME
                Error("split_union found in non-union object.\n");

            (reinterpret_cast<CSG *>(Object))->do_split = (int)Parse_Float();
        END_CASE

        CASE(PHOTONS_TOKEN)
            Parse_Begin();
            EXPECT
                CASE(TARGET_TOKEN)
                    Object->Ph_Density = Allow_Float(1.0);
                    if (Object->Ph_Density > 0)
                    {
                        Set_Flag(Object,PH_TARGET_FLAG);
                        CheckPassThru(Object, PH_TARGET_FLAG);
                    }
                    else
                    {
                        Clear_Flag(Object, PH_TARGET_FLAG);
                    }
                END_CASE

                CASE(REFRACTION_TOKEN)
                    if((int)Parse_Float())
                    {
                        Set_Flag(Object, PH_RFR_ON_FLAG);
                        Clear_Flag(Object, PH_RFR_OFF_FLAG);
                        CheckPassThru(Object, PH_RFR_ON_FLAG);
                    }
                    else
                    {
                        Clear_Flag(Object, PH_RFR_ON_FLAG);
                        Set_Flag(Object, PH_RFR_OFF_FLAG);
                    }
                END_CASE

                CASE(REFLECTION_TOKEN)
                    if((int)Parse_Float())
                    {
                        Set_Flag(Object, PH_RFL_ON_FLAG);
                        Clear_Flag(Object, PH_RFL_OFF_FLAG);
                    }
                    else
                    {
                        Clear_Flag(Object, PH_RFL_ON_FLAG);
                        Set_Flag(Object, PH_RFL_OFF_FLAG);
                    }
                END_CASE

                CASE(PASS_THROUGH_TOKEN)
                    if((int)Allow_Float(1.0))
                    {
                        Set_Flag(Object, PH_PASSTHRU_FLAG);
                        CheckPassThru(Object, PH_PASSTHRU_FLAG);
                    }
                    else
                    {
                        Clear_Flag(Object, PH_PASSTHRU_FLAG);
                    }
                END_CASE

                CASE(COLLECT_TOKEN)
                    Bool_Flag (Object, PH_IGNORE_PHOTONS_FLAG, !(Allow_Float(1.0) > 0.0));
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            Parse_End();

        END_CASE

        CASE(CUTAWAY_TEXTURES_TOKEN)
            if (dynamic_cast<CSGIntersection *>(Object) == nullptr) // FIXME
                Error("cutaway_textures can only be used with intersection and difference.");
            Set_Flag(Object, CUTAWAY_TEXTURES_FLAG);
        END_CASE

        CASE_COLOUR_UNGET
            Parse_Colour (Local_Colour);
            if (sceneData->EffectiveLanguageVersion() < 150)
            {
                if (Object->Texture != nullptr)
                {
                    if (Object->Texture->Type == PLAIN_PATTERN)
                    {
                        Object->Texture->Pigment->Quick_Colour = Local_Colour;
                        END_CASE
                    }
                }
            }
            Warning("Quick color belongs in texture. Color ignored.");
        END_CASE

        CASE (TRANSLATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Translation_Transform(&Local_Trans, Local_Vector);
            Translate_Object (Object, Local_Vector, &Local_Trans);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector (Local_Vector);
            Compute_Rotation_Transform(&Local_Trans, Local_Vector);
            Rotate_Object (Object, Local_Vector, &Local_Trans);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector (Local_Vector);
            Compute_Scaling_Transform(&Local_Trans, Local_Vector);
            Scale_Object (Object, Local_Vector, &Local_Trans);
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Transform_Object(Object, Parse_Transform(&Local_Trans));
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix (Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Transform_Object (Object, &Local_Trans);
        END_CASE

        CASE (BOUNDED_BY_TOKEN)
            Parse_Begin ();
            if(!Object->Bound.empty())
                if(Object->Clip == Object->Bound)
                    Error ("Cannot add bounds after linking bounds and clips.");

            if (AllowToken(CLIPPED_BY_TOKEN))
            {
                if (!Object->Bound.empty())
                    Error("Cannot link clips with previous bounds.");
                Object->Bound = Object->Clip;
            }
            else
                Parse_Bound_Clip(Object->Bound);

            Parse_End ();
        END_CASE

        CASE (CLIPPED_BY_TOKEN)
            Parse_Begin ();
            if(!Object->Clip.empty())
                if(Object->Clip == Object->Bound)
                    Error ("Cannot add clips after linking bounds and clips.");

            if (AllowToken(BOUNDED_BY_TOKEN))
            {
                if (!Object->Clip.empty())
                    Error("Cannot link bounds with previous clips.");
                Object->Clip = Object->Bound;
            }
            else
            {
                Parse_Bound_Clip(Object->Clip);

                /* Compute quadric bounding box before transformations. [DB 8/94] */

                if (dynamic_cast<Quadric *>(Object) != nullptr)
                {
                    Min = Vector3d(-BOUND_HUGE);
                    Max = Vector3d(BOUND_HUGE);

                    (dynamic_cast<Quadric *>(Object))->Compute_BBox(Min, Max);
                }
            }

            Parse_End ();
        END_CASE

        CASE (TEXTURE_TOKEN)
            Object->Type |= TEXTURED_OBJECT;
            Parse_Begin ();
            Local_Texture = Parse_Texture ();
            Parse_End ();
            Link_Textures(&(Object->Texture), Local_Texture);
        END_CASE

        CASE (INTERIOR_TEXTURE_TOKEN)
            Object->Type |= TEXTURED_OBJECT;
            Parse_Begin ();
            Local_Int_Texture = Parse_Texture ();
            Parse_End ();
            Link_Textures(&(Object->Interior_Texture), Local_Int_Texture);
        END_CASE

        CASE (INTERIOR_TOKEN)
            Parse_Interior(Object->interior);
        END_CASE

        CASE (MATERIAL_TOKEN)
            Local_Material.Texture  = Object->Texture;
            Local_Material.Interior_Texture  = Object->Interior_Texture;
            Local_Material.interior = Object->interior;
            Parse_Material(&Local_Material);
            Object->Texture  = Local_Material.Texture;
            if ( Object->Texture )
            {
                Object->Type |= TEXTURED_OBJECT;
            }
            Object->Interior_Texture  = Local_Material.Interior_Texture;
            Object->interior = Local_Material.interior;
        END_CASE

        CASE3 (PIGMENT_TOKEN, NORMAL_TOKEN, FINISH_TOKEN)
            Object->Type |= TEXTURED_OBJECT;
            if (Object->Texture == nullptr)
                Object->Texture = Copy_Textures(Default_Texture);
            else
                if (Object->Texture->Type != PLAIN_PATTERN)
                    Link_Textures(&(Object->Texture), Copy_Textures(Default_Texture));
            UNGET
            EXPECT
                CASE (PIGMENT_TOKEN)
                    Parse_Begin ();
                    Parse_Pigment ( &(Object->Texture->Pigment) );
                    Parse_End ();
                END_CASE

                CASE (NORMAL_TOKEN)
                    Parse_Begin ();
                    Parse_Tnormal ( &(Object->Texture->Tnormal) );
                    Parse_End ();
                END_CASE

                CASE (FINISH_TOKEN)
                    Parse_Finish ( &(Object->Texture->Finish) );
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
        END_CASE

        CASE (INVERSE_TOKEN)
            if (Object->Type & PATCH_OBJECT)
                Warning("Cannot invert a patch object.");

            // warning: Object->Invert will change the pointer if Object is CSG
            Object = Object->Invert();
        END_CASE

        CASE (STURM_TOKEN)
            if (!(Object->Type & STURM_OK_OBJECT))
                Not_With ("sturm","this object");
            Bool_Flag (Object, STURM_FLAG, (Allow_Float(1.0) > 0.0));
        END_CASE

        /* Object-Ray Options
           Do not intersect with camera rays [ENB 9/97] */
        CASE (NO_IMAGE_TOKEN)
            Bool_Flag (Object, NO_IMAGE_FLAG, (Allow_Float(1.0) > 0.0));
        END_CASE

        /* Object-Ray Options
           Do not intersect with reflection rays [ENB 9/97] */
        CASE (NO_REFLECTION_TOKEN)
            Bool_Flag (Object, NO_REFLECTION_FLAG, (Allow_Float(1.0) > 0.0));
        END_CASE

        /* Object-Ray Options
           Do not intersect with radiosity rays [CLi 5/09] */
        CASE (NO_RADIOSITY_TOKEN)
            Bool_Flag (Object, NO_RADIOSITY_FLAG, (Allow_Float(1.0) > 0.0));
        END_CASE

        CASE (NO_SHADOW_TOKEN)
            Set_Flag(Object, NO_SHADOW_FLAG);
        END_CASE

        CASE (LIGHT_SOURCE_TOKEN)
            Error("Light source must be defined using new syntax.");
        END_CASE

        CASE(HIERARCHY_TOKEN)
            if (!(Object->Type & HIERARCHY_OK_OBJECT))
                Not_With ("hierarchy", "this object");
            Bool_Flag (Object, HIERARCHY_FLAG, (Allow_Float(1.0) > 0.0));
        END_CASE

        CASE(HOLLOW_TOKEN)
            Bool_Flag (Object, HOLLOW_FLAG, (Allow_Float(1.0) > 0.0));
            Set_Flag (Object, HOLLOW_SET_FLAG);
            if ((dynamic_cast<CSGIntersection *>(Object) != nullptr) ||
                (dynamic_cast<CSGMerge *>(Object) != nullptr) ||
                (dynamic_cast<CSGUnion *>(Object) != nullptr))
            {
                Set_CSG_Children_Flag(Object, Test_Flag(Object, HOLLOW_FLAG), HOLLOW_FLAG, HOLLOW_SET_FLAG);
            }
        END_CASE

        CASE(DOUBLE_ILLUMINATE_TOKEN)
            Bool_Flag (Object, DOUBLE_ILLUMINATE_FLAG, (Allow_Float(1.0) > 0.0));
            if ((dynamic_cast<CSGIntersection *>(Object) != nullptr) ||
                (dynamic_cast<CSGMerge *>(Object) != nullptr) ||
                (dynamic_cast<CSGUnion *>(Object) != nullptr))
            {
                Set_CSG_Tree_Flag(Object, DOUBLE_ILLUMINATE_FLAG,Test_Flag(Object, DOUBLE_ILLUMINATE_FLAG));
            }
        END_CASE

        CASE(RADIOSITY_TOKEN)
#if 0
            Bool_Flag (Object, IGNORE_RADIOSITY_FLAG, !(Allow_Float(1.0) > 0.0));
#else
            Parse_Begin ();
            EXPECT
                CASE (IMPORTANCE_TOKEN)
                    Object->RadiosityImportance = Parse_Float ();
                    Object->RadiosityImportanceSet = true;
                    if ( (Object->RadiosityImportance <= 0.0) ||
                         (Object->RadiosityImportance >  1.0) )
                        Error("Radiosity importance must be greater than 0.0 and at most 1.0.");
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            Parse_End ();
#endif
        END_CASE

        CASE(DEBUG_TAG_TOKEN)
            s = Parse_C_String ();
#ifdef OBJECT_DEBUG_HELPER
            Object->Debug.Tag = s;
#endif
            POV_FREE (s);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    /*
     * Assign bounding objects' bounding box to object
     * if object's bounding box is larger. [DB 9/94]
     */

    if(!Object->Bound.empty())
    {
        /* Get bounding objects bounding box. */

        Min = Vector3d(-BOUND_HUGE);
        Max = Vector3d(BOUND_HUGE);

        for(vector<ObjectPtr>::iterator Sib = Object->Bound.begin(); Sib != Object->Bound.end(); Sib++)
        {
            if(!Test_Flag((*Sib), INVERTED_FLAG))
            {
                Min[X] = max(Min[X], (DBL)((*Sib)->BBox.lowerLeft[X]));
                Min[Y] = max(Min[Y], (DBL)((*Sib)->BBox.lowerLeft[Y]));
                Min[Z] = max(Min[Z], (DBL)((*Sib)->BBox.lowerLeft[Z]));
                Max[X] = min(Max[X], (DBL)((*Sib)->BBox.lowerLeft[X] + (*Sib)->BBox.size[X]));
                Max[Y] = min(Max[Y], (DBL)((*Sib)->BBox.lowerLeft[Y] + (*Sib)->BBox.size[Y]));
                Max[Z] = min(Max[Z], (DBL)((*Sib)->BBox.lowerLeft[Z] + (*Sib)->BBox.size[Z]));
            }
        }

        Make_BBox_from_min_max(BBox, Min, Max);

        /* Get bounding boxes' volumes. */

        // TODO - Area is probably a better measure to decide which box is better.
        // TODO - Doesn't this mechanism prevent users from reliably overriding broken default boxes?
        BOUNDS_VOLUME(V1, BBox);
        BOUNDS_VOLUME(V2, Object->BBox);

        if (V1 < V2)
        {
            Object->BBox = BBox;
        }
    }

    /*
     * Assign clipping objects' bounding box to object
     * if object's bounding box is larger. [DB 9/94]
     */

    if(!Object->Clip.empty())
    {
        /* Get clipping objects bounding box. */

        Min = Vector3d(-BOUND_HUGE);
        Max = Vector3d(BOUND_HUGE);

        for(vector<ObjectPtr>::iterator Sib = Object->Clip.begin(); Sib != Object->Clip.end(); Sib++)
        {
            if(!Test_Flag((*Sib), INVERTED_FLAG))
            {
                Min[X] = max(Min[X], (DBL)((*Sib)->BBox.lowerLeft[X]));
                Min[Y] = max(Min[Y], (DBL)((*Sib)->BBox.lowerLeft[Y]));
                Min[Z] = max(Min[Z], (DBL)((*Sib)->BBox.lowerLeft[Z]));
                Max[X] = min(Max[X], (DBL)((*Sib)->BBox.lowerLeft[X] + (*Sib)->BBox.size[X]));
                Max[Y] = min(Max[Y], (DBL)((*Sib)->BBox.lowerLeft[Y] + (*Sib)->BBox.size[Y]));
                Max[Z] = min(Max[Z], (DBL)((*Sib)->BBox.lowerLeft[Z] + (*Sib)->BBox.size[Z]));
            }
        }

        Make_BBox_from_min_max(BBox, Min, Max);

        /* Get bounding boxes' volumes. */

        // TODO - Area is probably a better measure to decide which box is better.
        BOUNDS_VOLUME(V1, BBox);
        BOUNDS_VOLUME(V2, Object->BBox);

        if (V1 < V2)
        {
            Object->BBox = BBox;
        }
    }

    if ((Object->Texture == nullptr) && (Object->Interior_Texture != nullptr))
        Error("Interior texture requires an exterior texture.");

    Parse_End ();

    return Object;
}

//******************************************************************************

void Parser::Parse_Matrix(MATRIX Matrix)
{
    int i, j;

    Parse_Angle_Begin();

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 3; j++)
        {
            Matrix[i][j] = Parse_Float();
            Parse_Comma();
        }

        Matrix[i][3] = (i != 3 ? 0.0 : 1.0);
    }

    Parse_Angle_End();

    /* Check to see that we aren't scaling any dimension by zero */
    for (i = 0; i < 3; i++)
    {
        if (fabs(Matrix[0][i]) < EPSILON && fabs(Matrix[1][i]) < EPSILON &&
            fabs(Matrix[2][i]) < EPSILON)
        {
            Warning("Illegal matrix column: Scale by 0.0. Changed to 1.0.");
            Matrix[i][i] = 1.0;
        }
    }
}

//******************************************************************************

TRANSFORM *Parser::Parse_Transform(TRANSFORM *Trans)
{
    Get_Token();
    if(CurrentTrueTokenId() == TRANSFORM_ID_TOKEN)
    {
        /* using old "transform TRANS_IDENT" syntax */
        if (Trans == nullptr)
            Trans=Create_Transform();
        else
        {
            MIdentity (Trans->matrix);
            MIdentity (Trans->inverse);
        }
        Compose_Transforms(Trans, CurrentTokenDataPtr<TRANSFORM*>());
    }
    else
    {
        /* using new "transform {TRANS}" syntax */
        Unget_Token();
        Trans = Parse_Transform_Block(Trans);
    }
    return Trans;
}

//******************************************************************************

TRANSFORM *Parser::Parse_Transform_Block(TRANSFORM *New)
{
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    Vector3d Local_Vector;
    bool isInverse = false;

    Parse_Begin();
    if (New == nullptr)
        New = Create_Transform();
    else
    {
        MIdentity (New->matrix);
        MIdentity (New->inverse);
    }

    EXPECT
        CASE(INVERSE_TOKEN)
            isInverse = true;
        END_CASE

        CASE(TRANSFORM_ID_TOKEN)
            Compose_Transforms(New, CurrentTokenDataPtr<TRANSFORM*>());
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Compose_Transforms(New, Parse_Transform(&Local_Trans));
        END_CASE

        CASE (TRANSLATE_TOKEN)
            Parse_Vector(Local_Vector);
            Compute_Translation_Transform(&Local_Trans, Local_Vector);
            Compose_Transforms(New, &Local_Trans);
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector(Local_Vector);
            Compute_Rotation_Transform(&Local_Trans, Local_Vector);
            Compose_Transforms(New, &Local_Trans);
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector(Local_Vector);
            Compute_Scaling_Transform(&Local_Trans, Local_Vector);
            Compose_Transforms(New, &Local_Trans);
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix(Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            Compose_Transforms(New, &Local_Trans);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End();

    if(isInverse == true)
    {
        MInvers(New->matrix, New->matrix);
        MInvers(New->inverse, New->inverse);
    }

    return (New);
}

//******************************************************************************

void Parser::Parse_Bound_Clip(vector<ObjectPtr>& dest, bool notexture)
{
    Vector3d Local_Vector;
    MATRIX Local_Matrix;
    TRANSFORM Local_Trans;
    ObjectPtr Current;
    vector<ObjectPtr> objects;

    while ((Current = Parse_Object()) != nullptr)
    {
        if((notexture == true) && (Current->Type & (TEXTURED_OBJECT+PATCH_OBJECT)))
            Error ("Illegal texture or patch in clip, bound, object or potential pattern.");
        objects.push_back(Current);
    }

    EXPECT
        CASE (TRANSLATE_TOKEN)
            Parse_Vector(Local_Vector);
            Compute_Translation_Transform(&Local_Trans, Local_Vector);
            for(vector<ObjectPtr>::iterator i = objects.begin(); i != objects.end(); i++)
            {
                Translate_Object(*i, Local_Vector, &Local_Trans);
            }
        END_CASE

        CASE (ROTATE_TOKEN)
            Parse_Vector(Local_Vector);
            Compute_Rotation_Transform(&Local_Trans, Local_Vector);
            for(vector<ObjectPtr>::iterator i = objects.begin(); i != objects.end(); i++)
            {
                Rotate_Object(*i, Local_Vector, &Local_Trans);
            }
        END_CASE

        CASE (SCALE_TOKEN)
            Parse_Scale_Vector(Local_Vector);
            Compute_Scaling_Transform(&Local_Trans, Local_Vector);
            for(vector<ObjectPtr>::iterator i = objects.begin(); i != objects.end(); i++)
            {
                Scale_Object(*i, Local_Vector, &Local_Trans);
            }
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Parse_Transform(&Local_Trans);

            for(vector<ObjectPtr>::iterator i = objects.begin(); i != objects.end(); i++)
            {
                Transform_Object(*i, &Local_Trans);
            }
        END_CASE

        CASE (MATRIX_TOKEN)
            Parse_Matrix(Local_Matrix);
            Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
            for(vector<ObjectPtr>::iterator i = objects.begin(); i != objects.end(); i++)
            {
                Transform_Object(*i, &Local_Trans);
            }
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if(objects.empty())
        Expectation_Error("object");

    dest.insert(dest.end(), objects.begin(), objects.end());
}


/*****************************************************************************
*
* FUNCTION
*
*   Parse_Three_UVCoords
*
* INPUT
*
* OUTPUT
*
*   UV1..UV3 are the uv
*
* RETURNS
*
*   1 for successful read, 0 if UV_VECTORS_TOKEN not found
*
* AUTHOR
*
*   Nathan Kopp
*
* DESCRIPTION
*
*   Look for UV_VECTORS_TOKEN and then read in three UV coordinates
*
******************************************************************************/

bool Parser::Parse_Three_UVCoords(Vector2d& UV1, Vector2d& UV2, Vector2d& UV3)
{
    if (AllowToken(UV_VECTORS_TOKEN))
    {
        Parse_UV_Vect(UV1);  Parse_Comma();
        Parse_UV_Vect(UV2);  Parse_Comma();
        Parse_UV_Vect(UV3);
        return true;
    }
    else
    {
        UV1[0] = UV1[1] = 0.0;
        UV2[0] = UV2[1] = 0.0;
        UV3[0] = UV3[1] = 0.0;
        return false;
    }
}

//******************************************************************************

bool Parser::Parse_Comma (void)
{
    Get_Token();
    if (CurrentTrueTokenId() != COMMA_TOKEN)
    {
        UNGET
        return false;
    }
    else
        return true;
}

//******************************************************************************

bool Parser::AllowToken(TokenId tokenId)
{
    Get_Token();
    bool tokenMatches = (CurrentTrueTokenId() == tokenId);
    if (!tokenMatches)
        Unget_Token();
    return tokenMatches;
}

//******************************************************************************

bool Parser::Peek_Token (TokenId tokenId)
{
    Get_Token();
    bool tokenMatches = (CurrentTrueTokenId() == tokenId);
    Unget_Token();
    return tokenMatches;
}

//******************************************************************************

void Parser::Parse_Semi_Colon (bool force_semicolon)
{
    Get_Token();
    if (CurrentTrueTokenId() != SEMI_COLON_TOKEN)
    {
        UNGET
        if ((sceneData->EffectiveLanguageVersion() >= 350) && (force_semicolon == true))
        {
            Error("All #declares of float, vector, and color require semi-colon ';' at end if the\n"
                  "language version is set to v3.5 or higher.\n"
                  "Either add the semi-colon or set the language version to v3.1 or lower.");
        }
        else if (sceneData->EffectiveLanguageVersion() >= 310)
        {
            PossibleError("All #version and #declares of float, vector, and color require semi-colon ';' at end.");
        }
    }
}

//******************************************************************************

void Parser::Parse_Coeffs(int order, DBL *Coeffs)
{
    int i;

    Parse_Angle_Begin();

    Coeffs[0] = Parse_Float();
    for (i = 1; i < term_counts(order); i++)
    {
        Parse_Comma();
        Coeffs[i] = Parse_Float();
    }

    Parse_Angle_End();
}

//******************************************************************************

ObjectPtr Parser::Parse_Object_Id ()
{
    ObjectPtr Object = nullptr;

    if (AllowToken(OBJECT_ID_TOKEN))
    {
        Warn_State(OBJECT_TOKEN);
        Object = Copy_Object(CurrentTokenDataPtr<ObjectPtr>());
        Object = Parse_Object_Mods(Object);
    }

    return (Object);
}

//******************************************************************************

void Parser::Parse_Declare(bool is_local, bool after_hash)
{
    vector<LValue> lvalues;
    bool deprecated = false;
    bool deprecated_once = false;
    TokenId Previous;
    int Local_Index;
    SYM_ENTRY *Temp_Entry;
    bool allow_redefine = true;
    UCS2 *deprecation_message;
    bool tupleDeclare = false;
    bool lvectorDeclare = false;
    bool larrayDeclare = false;
    TokenId* numberPtr = nullptr;
    void** dataPtr = nullptr;
    bool optional = false;

    POV_EXPERIMENTAL_ASSERT(IsOkToDeclare());
    SetOkToDeclare(false);

    if ((sceneData->EffectiveLanguageVersion() >= 350) && (after_hash == false))
    {
        PossibleError("'declare' should be changed to '#declare'.\n"
                      "Future versions may not support 'declare' and may require '#declare'.");
    }

    if (is_local)
    {
        Local_Index = mSymbolStack.GetLocalTableIndex();
    }
    else
    {
        Local_Index = mSymbolStack.GetGlobalTableIndex();
    }

    LValue_Ok = true;

    EXPECT_ONE
        CASE (LEFT_PAREN_TOKEN)
            UNGET
            tupleDeclare = true;
        END_CASE
        CASE (LEFT_ANGLE_TOKEN)
            UNGET
            lvectorDeclare = true;
        END_CASE
        CASE (LEFT_CURLY_TOKEN)
            UNGET
            larrayDeclare = true;
        END_CASE
        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT

    if (tupleDeclare)
    {
        Parse_Paren_Begin();
    }
    else if (lvectorDeclare)
    {
        Parse_Angle_Begin();
    }
    else if (larrayDeclare)
    {
        Parse_Begin();
    }

    for (bool more = true; more; /* body-controlled loop */)
    {
        deprecated = false;
        deprecated_once = false;
        numberPtr = nullptr;
        dataPtr = nullptr;
        optional = false;

        EXPECT
            CASE (DEPRECATED_TOKEN)
                deprecated = true;
                ALLOW(ONCE_TOKEN);
                if (CurrentTrueTokenId() == ONCE_TOKEN)
                    deprecated_once = true;
                deprecation_message = Parse_String(false, false);
            END_CASE

            CASE (OPTIONAL_TOKEN)
                optional = true;
            END_CASE

            OTHERWISE
                UNGET
                EXIT
            END_CASE
        END_EXPECT

        Previous = NOT_A_TOKEN;
        Temp_Entry = nullptr;

        EXPECT_ONE
            CASE (IDENTIFIER_TOKEN)
                POV_PARSER_ASSERT(!CurrentTokenIsHomogenousArrayElement());
                allow_redefine = true; // should actually be irrelevant downstream, thanks to Previous==IDENTIFIER_TOKEN
                if (CurrentTokenIsArrayElement())
                {
                    numberPtr = mToken.NumberPtr;
                    dataPtr   = mToken.DataPtr;
                    Previous  = CurrentTrueTokenId();
                }
                else
                {
                    if (CurrentTokenIsDictionaryElement())
                    {
                        if (is_local && !mSymbolStack.IsLocalTableIndex(mToken.context))
                            Error("Cannot use '#local' to assign a non-local array or dictionary element.");
                        Temp_Entry = mToken.table->Add_Symbol (CurrentTokenText(), IDENTIFIER_TOKEN);
                    }
                    else
                        Temp_Entry = mSymbolStack.Add_Symbol (Local_Index, CurrentTokenText(), IDENTIFIER_TOKEN);
                    numberPtr = &(Temp_Entry->Token_Number);
                    dataPtr   = &(Temp_Entry->Data);
                    Previous  = CurrentTrueTokenId();
                    if (deprecated)
                    {
                        Temp_Entry->deprecated = true;;
                        if (deprecated_once)
                            Temp_Entry->deprecatedOnce = true;
                        if (deprecation_message != nullptr)
                        {
                            UCS2String str(deprecation_message);
                            POV_FREE(deprecation_message);
                            Temp_Entry->Deprecation_Message = POV_STRDUP(UCS2toSysString(str).c_str());
                        }
                        else
                        {
                            char str[256];
                            sprintf(str, "Identifier '%.128s' was declared deprecated.", CurrentTokenText().c_str());
                            Temp_Entry->Deprecation_Message = POV_STRDUP(str);
                        }
                    }
                }
            END_CASE

            CASE3 (FILE_ID_TOKEN, MACRO_ID_TOKEN, PARAMETER_ID_TOKEN)
                // TODO - We should allow assignment if `is_local` is set and the identifier is non-local.
                Parse_Error(IDENTIFIER_TOKEN);
            END_CASE

            CASE2 (FUNCT_ID_TOKEN, VECTFUNCT_ID_TOKEN)
                // Issue an error, _except_ when assigning to a still-empty element of a function array.
                // TODO - We should allow assignment if `is_local` is set and the identifier is non-local.
                if ((!mToken.is_array_elem) || (*(mToken.DataPtr) != nullptr))
                    Error("Redeclaring functions is not allowed - #undef the function first!");
                // FALLTHROUGH

            // These are also used in Parse_Directive UNDEF_TOKEN section, Parse_Macro, and and Parse_For_Param,
            // and all these functions should accept exactly the same identifiers! [trf]
            CASE4 (NORMAL_ID_TOKEN, FINISH_ID_TOKEN, TEXTURE_ID_TOKEN, OBJECT_ID_TOKEN)
            CASE4 (COLOUR_MAP_ID_TOKEN, TRANSFORM_ID_TOKEN, CAMERA_ID_TOKEN, PIGMENT_ID_TOKEN)
            CASE4 (SLOPE_MAP_ID_TOKEN, NORMAL_MAP_ID_TOKEN, TEXTURE_MAP_ID_TOKEN, COLOUR_ID_TOKEN)
            CASE4 (PIGMENT_MAP_ID_TOKEN, MEDIA_ID_TOKEN, STRING_ID_TOKEN, INTERIOR_ID_TOKEN)
            CASE4 (DENSITY_MAP_ID_TOKEN, ARRAY_ID_TOKEN, DENSITY_ID_TOKEN, UV_ID_TOKEN)
            CASE4 (VECTOR_4D_ID_TOKEN, RAINBOW_ID_TOKEN, FOG_ID_TOKEN, SKYSPHERE_ID_TOKEN)
            CASE3 (MATERIAL_ID_TOKEN, SPLINE_ID_TOKEN, DICTIONARY_ID_TOKEN)
            CASE2 (VECTOR_ID_TOKEN, FLOAT_ID_TOKEN)
                if (is_local && !mSymbolStack.IsLocalTableIndex(mToken.context))
                {
                    if (CurrentTokenIsContainerElement())
                        Error ("Cannot use '#local' to assign a non-local array or dictionary element.");
                    allow_redefine = true; // should actually be irrelevant downstream, thanks to Previous==IDENTIFIER_TOKEN
                    Temp_Entry = mSymbolStack.Add_Symbol (Local_Index, CurrentTokenText(), IDENTIFIER_TOKEN);
                    numberPtr = &(Temp_Entry->Token_Number);
                    dataPtr   = &(Temp_Entry->Data);
                    Previous  = IDENTIFIER_TOKEN;
                }
                else
                {
                    allow_redefine = !CurrentTokenIsHomogenousArrayElement();
                    numberPtr = mToken.NumberPtr;
                    dataPtr   = mToken.DataPtr;
                    Previous  = CurrentTrueTokenId();
                }
            END_CASE

            CASE (EMPTY_ARRAY_TOKEN)
                POV_PARSER_ASSERT(mToken.is_array_elem);
                allow_redefine = true; // should actually be irrelevant downstream, thanks to Previous==EMPTY_ARRAY_TOKEN
                numberPtr = mToken.NumberPtr;
                dataPtr   = mToken.DataPtr;
                Previous  = CurrentTrueTokenId();
            END_CASE

            CASE4 (COMMA_TOKEN, RIGHT_PAREN_TOKEN, RIGHT_ANGLE_TOKEN, RIGHT_CURLY_TOKEN)
                if (tupleDeclare || lvectorDeclare || larrayDeclare)
                {
                    // when using tuple-style declare, it is legal to omit individual identifiers,
                    // in which case we evaluate the corresponding expression element but ignore
                    // the resulting value.
                    // We do this by assigning the resulting value to a dummy symbol entry.
                    allow_redefine = true; // should actually be irrelevant downstream, thanks to Previous=IDENTIFIER_TOKEN
                    Temp_Entry = SymbolTable::Create_Entry ("", DUMMY_SYMBOL_TOKEN);
                    numberPtr = &(Temp_Entry->Token_Number);
                    dataPtr = &(Temp_Entry->Data);
                    optional = true;
                    Previous = IDENTIFIER_TOKEN;
                    UNGET
                    END_CASE
                }
                // fall through

            OTHERWISE
                Parse_Error(IDENTIFIER_TOKEN);
            END_CASE
        END_EXPECT

        POV_PARSER_ASSERT((numberPtr != nullptr) || (mToken.NumberPtr == nullptr));
        POV_PARSER_ASSERT((dataPtr != nullptr) || (mToken.DataPtr == nullptr));

        LValue lvalue;
        lvalue.numberPtr = numberPtr;
        lvalue.dataPtr = dataPtr;
        lvalue.symEntry = Temp_Entry;
        lvalue.previous = Previous;
        lvalue.allowRedefine = allow_redefine;
        lvalue.optional = optional;
        lvalues.push_back(lvalue);

        if (lvectorDeclare && (lvalues.size() >= 5))
            more = false;
        else if (tupleDeclare || lvectorDeclare || larrayDeclare)
            more = AllowToken(COMMA_TOKEN);
        else
            more = false;
    }

    if (tupleDeclare)
    {
        Parse_Paren_End();
    }
    else if (lvectorDeclare)
    {
        Parse_Angle_End();
    }
    else if (larrayDeclare)
    {
        Parse_End();
    }

    LValue_Ok = false;

    GET (EQUALS_TOKEN)
    SetOkToDeclare(true);

    if (lvectorDeclare)
    {
        EXPRESS expr;
        int terms = 5;
        Parse_Express(expr, &terms);
        Promote_Express(expr,&terms,lvalues.size());
        for (int i = 0; i < lvalues.size(); ++i)
        {
            numberPtr = lvalues[i].numberPtr;
            dataPtr = lvalues[i].dataPtr;
            Previous = lvalues[i].previous;
            Temp_Entry = lvalues[i].symEntry;
            allow_redefine = lvalues[i].allowRedefine;

            *numberPtr = FLOAT_ID_TOKEN;
            Test_Redefine(Previous, numberPtr, *dataPtr, allow_redefine);
            *dataPtr = reinterpret_cast<void *>(Create_Float());
            *(reinterpret_cast<DBL *>(*dataPtr)) = expr[i];
        }
    }
    else if (larrayDeclare)
    {
        SYM_ENTRY *rvalue = SymbolTable::Create_Entry ("", DUMMY_SYMBOL_TOKEN);
        if (!Parse_RValue (IDENTIFIER_TOKEN, &(rvalue->Token_Number), &(rvalue->Data), nullptr, false, false, true, true, false, MAX_NUMBER_OF_TABLES) ||
            (rvalue->Token_Number != ARRAY_ID_TOKEN))
            Expectation_Error("array RValue");
        POV_ARRAY *a = reinterpret_cast<POV_ARRAY *>(rvalue->Data);
        if (a->maxDim != 0)
            Error ("cannot bulk-assign from multi-dimensional array");
        if (lvalues.size() > a->Sizes[0])
            Error ("array size mismatch");
        if (a->DataPtrs.empty())
            Error ("cannot assign from uninitialized array");

        for (int i = 0; i < lvalues.size(); ++i)
        {
            if (!a->HasElement(i))
                Error ("cannot assign from partially uninitialized array");

            numberPtr = lvalues[i].numberPtr;
            dataPtr = lvalues[i].dataPtr;
            Previous = lvalues[i].previous;
            Temp_Entry = lvalues[i].symEntry;
            allow_redefine = lvalues[i].allowRedefine;

            *numberPtr = a->ElementType(i);
            Test_Redefine(Previous, numberPtr, *dataPtr, allow_redefine);
            *dataPtr = SymbolTable::Copy_Identifier(a->DataPtrs[i], a->ElementType(i));
        }

        SymbolTable::Destroy_Entry (rvalue);
    }
    else
    {
        if (tupleDeclare)
        {
            Parse_Paren_Begin();
        }
        for (int i = 0; i < lvalues.size(); ++i)
        {
            numberPtr = lvalues[i].numberPtr;
            dataPtr = lvalues[i].dataPtr;
            Previous = lvalues[i].previous;
            Temp_Entry = lvalues[i].symEntry;
            allow_redefine = lvalues[i].allowRedefine;
            optional = lvalues[i].optional;

            if (i > 0)
            {
                GET (COMMA_TOKEN)
            }
            bool finalParameter = (i == lvalues.size()-1);
            if (!Parse_RValue (Previous, numberPtr, dataPtr, Temp_Entry, false, !tupleDeclare, is_local, allow_redefine, true, MAX_NUMBER_OF_TABLES))
            {
                EXPECT_ONE
                    CASE (IDENTIFIER_TOKEN)
                        // an uninitialized identifier was passed
                        if (!optional)
                            Error("Cannot pass uninitialized identifier to non-optional LValue.");
                    END_CASE

                    CASE (RIGHT_PAREN_TOKEN)
                        if (!finalParameter)
                            // the parameter list was closed prematurely
                            Error("Expected %d RValues but only %d found.",lvalues.size(),i);
                        // the parameter was left empty
                        if (!optional)
                            Error("Cannot omit RValue for non-optional LValue.");
                        UNGET
                    END_CASE

                    CASE (COMMA_TOKEN)
                        // the parameter was left empty
                        if (!optional)
                            Error("Cannot omit RValue for non-optional LValue.");
                        UNGET
                    END_CASE

                    OTHERWISE
                        Expectation_Error("RValue to declare");
                    END_CASE
                END_EXPECT
            }
        }
        if (tupleDeclare)
        {
            Parse_Paren_End();
        }
    }

    // discard any dummy symbol entries we may have created as stand-in for omitted identifiers
    // in tuple-style declarations
    for (vector<LValue>::iterator i = lvalues.begin(); i != lvalues.end(); ++i)
    {
        if ((i->symEntry != nullptr) && (i->symEntry->Token_Number == DUMMY_SYMBOL_TOKEN))
            SymbolTable::Destroy_Entry (i->symEntry);
    }

    if ( after_hash )
    {
        POV_EXPERIMENTAL_ASSERT(IsOkToDeclare());
        SetOkToDeclare(false);
        ALLOW(SEMI_COLON_TOKEN);
        SetOkToDeclare(true);
    }
}

bool Parser::PassParameterByReference (int callingContext)
{
    if (mToken.is_dictionary_elem)
    {
        return true;
    }
    else
    {
        return (mToken.context <= callingContext);
    }
}

bool Parser::Parse_RValue (TokenId Previous, TokenId *NumberPtr, void **DataPtr, SYM_ENTRY *sym, bool ParFlag, bool SemiFlag, bool is_local, bool allow_redefine, bool allowUndefined, int old_table_index)
{
    EXPRESS Local_Express;
    RGBFTColour *Local_Colour;
    PIGMENT *Local_Pigment;
    TNORMAL *Local_Tnormal;
    FINISH *Local_Finish;
    TEXTURE *Local_Texture, *Temp_Texture;
    TRANSFORM *Local_Trans;
    ObjectPtr Local_Object;
    Camera *Local_Camera;
    vector<Media> Local_Media;
    PIGMENT *Local_Density;
    InteriorPtr* Local_Interior;
    MATERIAL *Local_Material;
    void *Temp_Data;
    POV_PARAM *New_Par;
    bool Found=true;
    int Temp_Count=3000000; // TODO FIXME - magic value!
    bool oldOkToDeclare = IsOkToDeclare();
    int Terms;
    bool function_identifier;
    bool callable_identifier;
    bool had_callable_identifier;
    SYM_ENTRY* symbol_entry;
    SymbolTable* symbol_entry_table;

    bool oldParseOptionalRVaue = parseOptionalRValue;
    parseOptionalRValue = allowUndefined;

    EXPECT_ONE_CAT
        CASE4 (NORMAL_ID_TOKEN, FINISH_ID_TOKEN, TEXTURE_ID_TOKEN, OBJECT_ID_TOKEN)
        CASE4 (COLOUR_MAP_ID_TOKEN, TRANSFORM_ID_TOKEN, CAMERA_ID_TOKEN, PIGMENT_ID_TOKEN)
        CASE4 (SLOPE_MAP_ID_TOKEN,NORMAL_MAP_ID_TOKEN,TEXTURE_MAP_ID_TOKEN,ARRAY_ID_TOKEN)
        CASE4 (PIGMENT_MAP_ID_TOKEN, MEDIA_ID_TOKEN,INTERIOR_ID_TOKEN,DENSITY_ID_TOKEN)
        CASE4 (DENSITY_MAP_ID_TOKEN, RAINBOW_ID_TOKEN, FOG_ID_TOKEN, SKYSPHERE_ID_TOKEN)
        CASE3 (MATERIAL_ID_TOKEN, STRING_ID_TOKEN, DICTIONARY_ID_TOKEN)
            if ((ParFlag) && PassParameterByReference (old_table_index))
            {
                // pass by reference
                New_Par            = reinterpret_cast<POV_PARAM *>(POV_MALLOC(sizeof(POV_PARAM),"parameter"));
                New_Par->NumberPtr = mToken.NumberPtr;
                New_Par->DataPtr   = mToken.DataPtr;
                *NumberPtr = PARAMETER_ID_TOKEN;
                *DataPtr   = reinterpret_cast<void *>(New_Par);
            }
            else
            {
                // pass by value
                Temp_Data  = SymbolTable::Copy_Identifier(*mToken.DataPtr, *mToken.NumberPtr);
                *NumberPtr = *mToken.NumberPtr;
                Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
                *DataPtr   = Temp_Data;
            }
        END_CASE

        CASE (IDENTIFIER_TOKEN)
            if (allowUndefined)
            {
                Found = false;
                UNGET
            }
            else
            {
                Error("Cannot assign uninitialized identifier.");
            }
        END_CASE

        CASE_COLOUR_RAW // no UNGET here
            if ((CurrentTrueTokenId() != COLOUR_ID_TOKEN) || (sceneData->EffectiveLanguageVersion() < 350))
            {
                UNGET
                Local_Colour  = Create_Colour();
                SetOkToDeclare(false);
                Parse_Colour (*Local_Colour);
                if (SemiFlag)
                {
                    Parse_Semi_Colon(true);
                }
                *NumberPtr    = COLOUR_ID_TOKEN;
                Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
                *DataPtr      = reinterpret_cast<void *>(Local_Colour);
                END_CASE
            }
            // intentional to allow color dot expressions as macro parameters if #version is 3.5 or higher [trf]
            // FALLTHROUGH
        FALLTHROUGH_CASE

        CASE_VECTOR_UNGET
            // It seems very few people understand what is going on here, so let me try to
            // explain it. All comments below [1] are mine [2] and they are based on how I think it
            // works and understand it. As I didn't write most of the code I cannot really
            // tell for sure, so if anybody finds incorrect comments please let me know!
            // BTW, when saying #declare it always implies "or #local" :-)

            // [1] That's most certainly no longer true. [CLi]
            // [2] Who is "me" anyway? [CLi]

            // determine the type of the first identifier
            function_identifier = (CurrentTrueTokenId() == FUNCT_ID_TOKEN) ||
                                  (CurrentTrueTokenId() == VECTFUNCT_ID_TOKEN);
            callable_identifier = function_identifier ||
                                  (CurrentTrueTokenId() == SPLINE_ID_TOKEN);

            // don't allow #declares from here
            SetOkToDeclare(false);

            // if what follows could be a function/spline call or
            // is a macro parameter taking a float, vector or ids
            // of a float, vector or color then count the tokens
            // found between now and the time when the function
            // Parse_Unknown_Vector returns
            if (callable_identifier || (ParFlag && ((CurrentTrueTokenId()==FLOAT_ID_TOKEN)     ||
                                                    (CurrentTrueTokenId()==VECTOR_ID_TOKEN)    ||
                                                    (CurrentTrueTokenId()==VECTOR_4D_ID_TOKEN) ||
                                                    (CurrentTrueTokenId()==UV_ID_TOKEN)        ||
                                                    (CurrentTrueTokenId()==COLOUR_ID_TOKEN))))
            {
                Temp_Count = mTokenCount;
            }

            // assume no callable identifier (that is a function or spline identifier) has been found
            had_callable_identifier = false;

            // [CLi] If we're dealing with a local symbol, Parse_Unknown_Vector may cause it to drop out of scope,
            // so we claim dibs on it until we're done.
            // TODO - this is a bit hackish; ideally, if the Token is a symbol we should have it store the SYM_ENTRY pointer,
            // so we don't need to look it up again via name.
            if ((CurrentTrueTokenId() == FUNCT_ID_TOKEN)     || (CurrentTrueTokenId() == VECTFUNCT_ID_TOKEN) ||
                (CurrentTrueTokenId() == SPLINE_ID_TOKEN)    || (CurrentTrueTokenId() == UV_ID_TOKEN)        ||
                (CurrentTrueTokenId() == VECTOR_4D_ID_TOKEN) || (CurrentTrueTokenId() == COLOUR_ID_TOKEN))
            {
                symbol_entry = mToken.table->Find_Symbol (CurrentTokenText().c_str());
                if (symbol_entry)
                {
                    symbol_entry_table = mToken.table;
                    SymbolTable::Acquire_Entry_Reference(symbol_entry);
                }
            }
            else
            {
                symbol_entry = nullptr;
            }

            // parse the expression and determine if it was a callable identifier
            Terms = Parse_Unknown_Vector (Local_Express, true, &had_callable_identifier);

            // if in a #declare force a semicolon at the end
            // (but don't "eat" it yet, `Parse_Declare` will take care of that)
            if (SemiFlag)
            {
                Parse_Semi_Colon(true);
                UNGET
            }

            // get the number of tokens found
            Temp_Count = mTokenCount - Temp_Count;

            if ((Temp_Count != 1) && had_callable_identifier)
                // no tokens have been found or a function call had no parameters in parenthesis
                Error("Identifier expected, incomplete function call or spline call found instead.");

            if ((Temp_Count == 1) && PassParameterByReference (old_table_index))
            {
                // only one identifier token has been found so pass it by reference
                // It is important that functions are passed by value and not by reference! [trf]
                if(!ParFlag || function_identifier)
                {
                    // pass by value
                    Temp_Data  = SymbolTable::Copy_Identifier(*mToken.DataPtr, *mToken.NumberPtr);
                    *NumberPtr = *mToken.NumberPtr;
                    Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
                    *DataPtr   = Temp_Data;
                }
                else
                {
                    // pass by reference
                    New_Par            = reinterpret_cast<POV_PARAM *>(POV_MALLOC(sizeof(POV_PARAM),"parameter"));
                    New_Par->NumberPtr = mToken.NumberPtr;
                    New_Par->DataPtr   = mToken.DataPtr;

                    *NumberPtr = PARAMETER_ID_TOKEN;
                    *DataPtr   = reinterpret_cast<void *>(New_Par);
                }
            }
            else // an expression has been found, so create a new identifier
            {
                switch(Terms)
                {
                    case 1:
                        *NumberPtr = FLOAT_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
                        *DataPtr   = reinterpret_cast<void *>(Create_Float());
                        *(reinterpret_cast<DBL *>(*DataPtr))  = Local_Express[X];
                        break;

                    case 2:
                        *NumberPtr = UV_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
                        *DataPtr   = reinterpret_cast<void *>(new Vector2d(Local_Express));
                        break;

                    case 3:
                        *NumberPtr = VECTOR_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
                        *DataPtr   = reinterpret_cast<void *>(new Vector3d(Local_Express));
                        break;

                    case 4:
                        *NumberPtr = VECTOR_4D_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
                        *DataPtr   = reinterpret_cast<void *>(Create_Vector_4D());
                        Assign_Vector_4D(reinterpret_cast<DBL *>(*DataPtr), Local_Express);
                        break;

                    case 5:
                        *NumberPtr    = COLOUR_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
                        *DataPtr      = reinterpret_cast<void *>(Create_Colour());
                        (*reinterpret_cast<RGBFTColour *>(*DataPtr)).Set(Local_Express, 5);
                        break;
                }
            }
            if (symbol_entry)
                SymbolTable::Release_Entry_Reference (symbol_entry);
        END_CASE

        CASE (PIGMENT_TOKEN)
            Local_Pigment = Copy_Pigment(Default_Texture->Pigment);
            Parse_Begin ();
            Parse_Pigment (&Local_Pigment);
            Parse_End ();
            *NumberPtr = PIGMENT_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = reinterpret_cast<void *>(Local_Pigment);
        END_CASE

        CASE (NORMAL_TOKEN)
            Local_Tnormal = Copy_Tnormal(Default_Texture->Tnormal);
            Parse_Begin ();
            Parse_Tnormal (&Local_Tnormal);
            Parse_End ();
            *NumberPtr = NORMAL_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = reinterpret_cast<void *>(Local_Tnormal);
        END_CASE

        CASE (FINISH_TOKEN)
            Local_Finish = Copy_Finish(Default_Texture->Finish);
            Parse_Finish (&Local_Finish);
            *NumberPtr = FINISH_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = reinterpret_cast<void *>(Local_Finish);
        END_CASE

        CASE (CAMERA_TOKEN)
            Local_Camera = new Camera(Default_Camera);
            Parse_Camera (*Local_Camera);
            *NumberPtr = CAMERA_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = reinterpret_cast<void *>(Local_Camera);
        END_CASE

        CASE (TEXTURE_TOKEN)
            Parse_Begin ();
            Local_Texture = Parse_Texture ();
            Parse_End ();
            Temp_Texture = nullptr;
            Link_Textures(&Temp_Texture, Local_Texture);
            SetOkToDeclare(false);
            EXPECT
                CASE (TEXTURE_TOKEN)
                    Parse_Begin ();
                    Local_Texture = Parse_Texture ();
                    Parse_End ();
                    Link_Textures(&Temp_Texture, Local_Texture);
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT

            *NumberPtr    = TEXTURE_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr      = reinterpret_cast<void *>(Temp_Texture);
        END_CASE

        CASE (COLOUR_MAP_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(new ColourBlendMapPtr(Parse_Colour_Map<ColourBlendMap> ()));
            *NumberPtr = COLOUR_MAP_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (PIGMENT_MAP_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(new PigmentBlendMapPtr(Parse_Blend_Map<PigmentBlendMap> (kBlendMapType_Pigment,NO_PATTERN)));
            *NumberPtr = PIGMENT_MAP_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (SPLINE_TOKEN)
            mExperimentalFlags.spline = true;
            Parse_Begin();
            Temp_Data  = reinterpret_cast<void *>(Parse_Spline());
            Parse_End();
            *NumberPtr = SPLINE_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (DENSITY_MAP_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(new PigmentBlendMapPtr(Parse_Blend_Map<PigmentBlendMap> (kBlendMapType_Density,NO_PATTERN)));
            *NumberPtr = DENSITY_MAP_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (SLOPE_MAP_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(new SlopeBlendMapPtr(Parse_Blend_Map<SlopeBlendMap> (kBlendMapType_Slope,NO_PATTERN)));
            *NumberPtr = SLOPE_MAP_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (TEXTURE_MAP_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(new TextureBlendMapPtr(Parse_Blend_Map<TextureBlendMap> (kBlendMapType_Texture,NO_PATTERN)));
            *NumberPtr = TEXTURE_MAP_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (NORMAL_MAP_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(new NormalBlendMapPtr(Parse_Blend_Map<NormalBlendMap> (kBlendMapType_Normal,NO_PATTERN)));
            *NumberPtr = NORMAL_MAP_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (RAINBOW_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(Parse_Rainbow());
            *NumberPtr = RAINBOW_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (FOG_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(Parse_Fog());
            *NumberPtr = FOG_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (MEDIA_TOKEN)
            Parse_Media(Local_Media);
            Temp_Data  = reinterpret_cast<void *>(new Media(Local_Media.front()));
            *NumberPtr = MEDIA_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (DENSITY_TOKEN)
            Local_Density = nullptr;
            Parse_Begin ();
            Parse_Media_Density_Pattern (&Local_Density);
            Parse_End ();
            *NumberPtr = DENSITY_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = reinterpret_cast<void *>(Local_Density);
        END_CASE

        CASE (INTERIOR_TOKEN)
            Local_Interior = new InteriorPtr;
            Parse_Interior(*Local_Interior);
            Temp_Data  = reinterpret_cast<void *>(Local_Interior);
            *NumberPtr = INTERIOR_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (MATERIAL_TOKEN)
            Local_Material = Create_Material();
            Parse_Material(Local_Material);
            Temp_Data  = reinterpret_cast<void *>(Local_Material);
            *NumberPtr = MATERIAL_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (SKYSPHERE_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(Parse_Skysphere());
            *NumberPtr = SKYSPHERE_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (FUNCTION_TOKEN)
            // Do NOT allow to redefine functions! [trf]
            //   #declare foo = function(x) { x }
            //   #declare foo = function(x) { foo(x) } // Error!
            // Reason: Code like this would be unreadable but possible. Is it
            // a recursive function or not? - It is not recursive because the
            // foo in the second line refers to the first function, which is
            // not logical. Further, recursion is not supported in current POV-Ray
            // anyway. However, allowing such code now would cause problems
            // implementing recursive functions in future versions!
            if (sym != nullptr)
                Temp_Data  = reinterpret_cast<void *>(new AssignableFunction(
                    Parse_DeclareFunction(NumberPtr, sym->name.c_str(), is_local),
                    mpFunctionVM));
            else
                Temp_Data  = reinterpret_cast<void *>(new AssignableFunction(
                    Parse_DeclareFunction(NumberPtr, nullptr, is_local),
                    mpFunctionVM));
            Test_Redefine(Previous, NumberPtr, *DataPtr, false);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (TRANSFORM_TOKEN)
            Local_Trans = Parse_Transform ();
            *NumberPtr  = TRANSFORM_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr    = reinterpret_cast<void *>(Local_Trans);
        END_CASE

        CASE5 (STRING_LITERAL_TOKEN,CHR_TOKEN,SUBSTR_TOKEN,STR_TOKEN,VSTR_TOKEN)
        CASE4 (CONCAT_TOKEN,STRUPR_TOKEN,STRLWR_TOKEN,DATETIME_TOKEN)
            UNGET
            Temp_Data  = Parse_String();
            *NumberPtr = STRING_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (ARRAY_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(Parse_Array_Declare());
            *NumberPtr = ARRAY_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        CASE (DICTIONARY_TOKEN)
            Temp_Data  = reinterpret_cast<void *>(Parse_Dictionary_Declare());
            *NumberPtr = DICTIONARY_ID_TOKEN;
            Test_Redefine (Previous,NumberPtr,*DataPtr, allow_redefine);
            *DataPtr   = Temp_Data;
        END_CASE

        OTHERWISE
            UNGET
            Local_Object = Parse_Object ();
            Found = (Local_Object != nullptr);
            if (Found)
            {
                *NumberPtr   = OBJECT_ID_TOKEN;
                Test_Redefine(Previous,NumberPtr,*DataPtr, allow_redefine);
                *DataPtr     = reinterpret_cast<void *>(Local_Object);
            }
        END_CASE

    END_EXPECT

    SetOkToDeclare(oldOkToDeclare);
    parseOptionalRValue = oldParseOptionalRVaue;
    return(Found);
}

//******************************************************************************

void Parser::Link(ObjectPtr New_Object, vector<ObjectPtr>& Object_List_Root)
{
    Object_List_Root.push_back(New_Object);
}

//******************************************************************************

void Parser::Link_Textures (TEXTURE **Old_Textures, TEXTURE *New_Textures)
{
    TEXTURE *Layer;

    if (New_Textures == nullptr)
        return;

    if (*Old_Textures != nullptr)
    {
        if ((*Old_Textures)->Type != PLAIN_PATTERN)
        {
            Error("Cannot layer over a patterned texture.");
        }
    }
    for (Layer = New_Textures; Layer->Next != nullptr; Layer = Layer->Next)
    {
        /* NK layers - 1999 June 10 - for backwards compatiblity with layered textures */
        if(sceneData->EffectiveLanguageVersion() <= 310)
            Convert_Filter_To_Transmit(Layer->Pigment);
    }

    /* NK layers - 1999 Nov 16 - for backwards compatiblity with layered textures */
    if ((sceneData->EffectiveLanguageVersion() <= 310) && (*Old_Textures != nullptr))
        Convert_Filter_To_Transmit(Layer->Pigment);

    Layer->Next = *Old_Textures;
    *Old_Textures = New_Textures;

    if ((New_Textures->Type != PLAIN_PATTERN) && (New_Textures->Next != nullptr))
    {
        Error("Cannot layer a patterned texture over another.");
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
*    Changed macro ALLOW_REDEFINE to parameter which defaults to true [trf]
*
******************************************************************************/

void Parser::Test_Redefine(TokenId Previous, TokenId *NumberPtr, void *Data, bool allow_redefine)
{
    if ((Previous == IDENTIFIER_TOKEN) || (Previous == EMPTY_ARRAY_TOKEN))
    {
        return;
    }
    /* NK 1998 - allow user to redefine all identifiers! */
    if( allow_redefine)
    {
        SymbolTable::Destroy_Ident_Data(Data, Previous);
    }
    else
    {
        if (Previous == *NumberPtr)
        {
            SymbolTable::Destroy_Ident_Data(Data,*NumberPtr);
        }
        else
        {
            const char *oldt, *newt;

            oldt = Get_Token_String (Previous);
            newt = Get_Token_String (*NumberPtr);
            *NumberPtr = Previous;

            Error ("Attempted to redefine %s as %s.", oldt, newt);
        }
    }
}

//******************************************************************************

void Parser::Parse_Error(TokenId Token_Id)
{
    Expectation_Error(Get_Token_String(Token_Id));
}

//******************************************************************************

void Parser::Found_Instead_Error(const char *exstr, const char *extokstr)
{
    const char *found;

    switch (CurrentCategorizedTokenId())
    {
        case IDENTIFIER_TOKEN:
            Error("%s '%s', undeclared identifier '%s' found instead", exstr, extokstr, CurrentTokenText().c_str());
            break;
        case VECTOR_TOKEN_CATEGORY:
            found = Get_Token_String(CurrentTrueTokenId());
            Error("%s '%s', vector function '%s' found instead", exstr, extokstr, found);
            break;
        case FLOAT_TOKEN_CATEGORY:
            found = Get_Token_String(CurrentTrueTokenId());
            Error("%s '%s', float function '%s' found instead", exstr, extokstr, found);
            break;
        case COLOUR_TOKEN_CATEGORY:
            found = Get_Token_String(CurrentTrueTokenId());
            Error("%s '%s', color keyword '%s' found instead", exstr, extokstr, found);
            break;
        default:
            found = Get_Token_String(CurrentTrueTokenId());
            Error("%s '%s', %s found instead", exstr, extokstr, found);
    }
}

//******************************************************************************

void Parser::Warn_State(TokenId Type)
{
    char *str;

    if(sceneData->EffectiveLanguageVersion() >= 150)
        return;

    str = reinterpret_cast<char *>(POV_MALLOC(160, "global setting warning string"));

    strcpy(str, "Found '");
    strcat(str, Get_Token_String (CurrentTrueTokenId()));
    strcat(str, "' that should be in '");
    strcat(str, Get_Token_String (Type));
    strcat(str, "' statement.");
    Warning(str);
    POV_FREE(str);
}

//******************************************************************************

void Parser::MAError (const char *, long)
{
    throw std::bad_alloc();
}

//******************************************************************************

void Parser::Post_Process (ObjectPtr Object, ObjectPtr Parent)
{
    DBL Volume;
    FINISH *Finish;

    if (Object == nullptr)
    {
        return;
    }

    if (Object->Type & LT_SRC_UNION_OBJECT)
    {
        for (vector<ObjectPtr>::iterator Sib = (reinterpret_cast<CSG *>(Object))->children.begin(); Sib != (reinterpret_cast<CSG *>(Object))->children.end(); Sib++)
        {
            Post_Process(*Sib, Object);
        }
        return;
    }

    // Promote texture etc. from parent to children.

    if (Parent != nullptr)
    {
        if (Object->Texture == nullptr)
        {
            Object->Texture = Copy_Texture_Pointer(Parent->Texture);
            // NK 1998 copy uv_mapping flag if and only if we copy the texture
            if (Test_Flag(Parent, UV_FLAG))
                Set_Flag(Object, UV_FLAG);
        }
        if (Object->Interior_Texture == nullptr)
        {
            Object->Interior_Texture = Copy_Texture_Pointer(Parent->Interior_Texture);
            if(Test_Flag(Parent, UV_FLAG))
                Set_Flag(Object, UV_FLAG);
        }
        if (Object->interior == nullptr)
        {
            // TODO - may need to copy the interior, as we may need to modify a few of its fields.
            Object->interior = Parent->interior;
        }

        if (Test_Flag(Parent, NO_REFLECTION_FLAG))
        {
            Set_Flag(Object, NO_REFLECTION_FLAG);
        }
        if (Test_Flag(Parent, NO_RADIOSITY_FLAG))
        {
            Set_Flag(Object, NO_RADIOSITY_FLAG);
        }
        if (Test_Flag(Parent, NO_IMAGE_FLAG))
        {
            Set_Flag(Object, NO_IMAGE_FLAG);
        }
        if (Test_Flag(Parent, NO_SHADOW_FLAG))
        {
            Set_Flag(Object, NO_SHADOW_FLAG);
        }
        if (Test_Flag(Parent, CUTAWAY_TEXTURES_FLAG))
        {
            Set_Flag(Object, CUTAWAY_TEXTURES_FLAG);
        }

        // NK phmap
        // promote photon mapping flags to child
        if (Test_Flag(Parent, PH_TARGET_FLAG))
        {
            Set_Flag(Object, PH_TARGET_FLAG);
            Object->Ph_Density = Parent->Ph_Density;
            CheckPassThru(Object, PH_TARGET_FLAG);
        }

        // promote object-specific radiosity settings to child
        if (!Object->RadiosityImportanceSet)
        {
            if (Parent->RadiosityImportanceSet)
                Object->RadiosityImportance = Parent->RadiosityImportance;
            else
                Object->RadiosityImportance = sceneData->radiositySettings.defaultImportance;
            Object->RadiosityImportanceSet = true;
        }

        if(Test_Flag(Parent, PH_PASSTHRU_FLAG))
        {
            Set_Flag(Object, PH_PASSTHRU_FLAG);
            CheckPassThru(Object, PH_PASSTHRU_FLAG);
        }

        if (Test_Flag(Parent, PH_RFL_ON_FLAG))
        {
            Set_Flag(Object, PH_RFL_ON_FLAG);
            Clear_Flag(Object, PH_RFL_OFF_FLAG);
        }
        else if (Test_Flag(Parent, PH_RFL_OFF_FLAG))
        {
            Set_Flag(Object, PH_RFL_OFF_FLAG);
            Clear_Flag(Object, PH_RFL_ON_FLAG);
        }

        if (Test_Flag(Parent, PH_RFR_ON_FLAG))
        {
            Set_Flag(Object, PH_RFR_ON_FLAG);
            Clear_Flag(Object, PH_RFR_OFF_FLAG);
            CheckPassThru(Object, PH_RFR_ON_FLAG);
        }
        else if (Test_Flag(Parent, PH_RFR_OFF_FLAG))
        {
            Set_Flag(Object, PH_RFR_OFF_FLAG);
            Clear_Flag(Object, PH_RFR_ON_FLAG);
        }

        if(Test_Flag(Parent, PH_IGNORE_PHOTONS_FLAG))
        {
            Set_Flag(Object, PH_IGNORE_PHOTONS_FLAG);
        }
    }

    if (Object->interior != nullptr)
        Object->interior->PostProcess();

    if ((Object->Texture == nullptr) &&
        !(Object->Type & TEXTURED_OBJECT) &&
        !(Object->Type & LIGHT_SOURCE_OBJECT))
    {
        if (Parent)
        {
            if ((dynamic_cast<CSGIntersection *>(Parent) == nullptr) ||
                !Test_Flag(Parent, CUTAWAY_TEXTURES_FLAG))
            {
                Object->Texture = Copy_Textures(Default_Texture);
            }
        }
        else
            Object->Texture = Copy_Textures(Default_Texture);
    }

    if(!(Object->Type & LIGHT_GROUP_OBJECT) &&
       !(Object->Type & LIGHT_GROUP_LIGHT_OBJECT))
    {
        Post_Textures(Object->Texture);  //moved cey 6/97

        if (Object->Interior_Texture)
        {
            Post_Textures(Object->Interior_Texture);
        }
    }

    if(Object->Type & LIGHT_SOURCE_OBJECT)
    {
        DBL len1,len2;
        LightSource *Light = reinterpret_cast<LightSource *>(Object);


        // check some properties of the orient light sources
        if (Light->Orient)
        {
            if(!Light->Circular)
            {
                Light->Circular = true;
                Warning("Orient can only be used with circular area lights. This area light is now circular.");
            }

            len1 = Light->Axis1.length();
            len2 = Light->Axis2.length();

            if(fabs(len1-len2)>EPSILON)
            {
                Warning("When using orient, the two axes of the area light must be of equal length.\nOnly the length of the first axis will be used.");

                // the equalization is actually done in the lighting code, since only the length of
                // Axis1 will be used

            }

            if(Light->Area_Size1 != Light->Area_Size2)
            {
                Warning("When using orient, the two sample sizes for the area light should be equal.");
            }
        }

        // Make sure that circular light sources are larger than 1 by x [ENB 9/97]
        if (Light->Circular)
        {
            if ((Light->Area_Size1 <= 1) || (Light->Area_Size2 <= 1))
            {
                Warning("Circular area lights must have more than one sample along each axis.");
                Light->Circular = false;
            }
        }
    }

    if (Object->Type & LIGHT_SOURCE_OBJECT)
    {
        // post-process the light source
        if ((reinterpret_cast<LightSource *>(Object))->Projected_Through_Object != nullptr)
        {
            if ((reinterpret_cast<LightSource *>(Object))->Projected_Through_Object->interior != nullptr)
            {
                (reinterpret_cast<LightSource *>(Object))->Projected_Through_Object->interior.reset();
                Warning("Projected through objects can not have interior, interior removed.");
            }
            if ((reinterpret_cast<LightSource *>(Object))->Projected_Through_Object->Texture != nullptr)
            {
                Destroy_Textures((reinterpret_cast<LightSource *>(Object))->Projected_Through_Object->Texture);
                (reinterpret_cast<LightSource *>(Object))->Projected_Through_Object->Texture = nullptr;
                Warning("Projected through objects can not have texture, texture removed.");
            }
        }

        // only global light sources are in Frame.Light_Sources list [trf]
        if(!(Object->Type & LIGHT_GROUP_LIGHT_OBJECT))
            // add this light to the frame's list of global light sources
            sceneData->lightSources.push_back(reinterpret_cast<LightSource *>(Object));
        else
            // Put it into the frame's list of light-group lights
            sceneData->lightGroupLightSources.push_back(reinterpret_cast<LightSource *>(Object));
    }
    else
    {
        // post-process the object

        // If there is no interior create one.

        if (Object->interior == nullptr)
        {
            Object->interior = InteriorPtr(new Interior());
        }

        // Promote hollow flag to interior.

        Object->interior->hollow = (Test_Flag(Object, HOLLOW_FLAG) != false);

        // Promote finish's IOR to interior IOR.

        if (Object->Texture != nullptr)
        {
            if (Object->Texture->Type == PLAIN_PATTERN)
            {
                Finish = Object->Texture->Finish;
                if (Finish != nullptr)
                {
                    if (Finish->Temp_IOR >= 0.0)
                    {
                        Object->interior->IOR = Finish->Temp_IOR;
                        Object->interior->Dispersion = Finish->Temp_Dispersion;
                    }
                    if (Finish->Temp_Caustics >= 0.0)
                    {
                        Object->interior->Caustics = Finish->Temp_Caustics;
                    }

                    Object->interior->Old_Refract = Finish->Temp_Refract;
                }
            }
        }

        // If there is no IOR specified use the atmopshere ior.

        if (Object->interior->IOR == 0.0)
        {
            Object->interior->IOR = sceneData->atmosphereIOR;
            Object->interior->Dispersion = sceneData->atmosphereDispersion;
        }

        // If object has subsurface light transport enabled, precompute some necessary information
        /* if(!Object->Texture->Finish->SubsurfaceTranslucency.IsZero()) */
        if (sceneData->useSubsurface)
        {
            Object->interior->subsurface = shared_ptr<SubsurfaceInterior>(new SubsurfaceInterior(Object->interior->IOR));
        }
    }

    if (Object->Type & IS_COMPOUND_OBJECT)
    {
        for (vector<ObjectPtr>::iterator Sib = (reinterpret_cast<CSG *>(Object))->children.begin(); Sib != (reinterpret_cast<CSG *>(Object))->children.end(); Sib++)
        {
            Post_Process(*Sib, Object);
        }
    }

    // Test whether the object is finite or infinite. [DB 9/94]
    // CJC TODO FIXME: see if this can be improved, and/or if it is appropriate for all bounding systems

    BOUNDS_VOLUME(Volume, Object->BBox);

    if (Volume > INFINITE_VOLUME)
    {
        Set_Flag(Object, INFINITE_FLAG);
    }

    // Test if the object is opaque or not. [DB 8/94]

    if (Object->IsOpaque())
        Set_Flag(Object, OPAQUE_FLAG);
}

/*****************************************************************************
*
* FUNCTION
*
*   Link_To_Frame
*
* INPUT
*
*   Object - Pointer to object
*
* OUTPUT
*
*   Object
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
*   Sep 1994 : Added optional splitting of bounded unions if children are
*              finite. Added removing of unnecessary bounding. [DB]
*
******************************************************************************/

void Parser::Link_To_Frame(ObjectPtr Object)
{
    if (Object == nullptr)
        return;

    /* Remove bounding object if object is cheap to intersect. [DB 8/94]  */

    if ((Object->Bound.empty() == false) && (sceneData->removeBounds == true))
    {
        if ((dynamic_cast<CSGUnion *>(Object) == nullptr)        && // FIXME
            (dynamic_cast<CSGIntersection *>(Object) == nullptr) && // FIXME
            (dynamic_cast<CSGMerge *>(Object) == nullptr)        && // FIXME
            (dynamic_cast<Poly *>(Object) == nullptr)            && // FIXME
            (dynamic_cast<TrueType *>(Object) == nullptr)        && // FIXME
            ((dynamic_cast<Quadric *>(Object) == nullptr) || (dynamic_cast<Quadric *>(Object)->Automatic_Bounds)))
        {
            /* Destroy only, if bounding object is not used as clipping object. */

            if(Object->Bound != Object->Clip)
                Destroy_Object(Object->Bound);
            Object->Bound.clear();
            Warning("Unnecessary bounding object removed.");
        }
    }

    /*
     * [CJC 8/01]
     *
     * if all children of a union have the no_shadow flag, then the union should
     * have it as well.
     */
    if ((dynamic_cast<CSGUnion *>(Object) != nullptr) && (dynamic_cast<CSGMerge *>(Object) == nullptr))
    {
        vector<ObjectPtr>::iterator This_Sib = (dynamic_cast<CSG *>(Object))->children.begin();
        while (This_Sib != (dynamic_cast<CSG *>(Object))->children.end())
        {
            if ((dynamic_cast<LightSource *>(*This_Sib) == nullptr) && !Test_Flag ((*This_Sib), NO_SHADOW_FLAG)) // FIXME
                break;
            This_Sib++;
        }
        if(This_Sib == (dynamic_cast<CSG *>(Object))->children.end())
            Set_Flag(Object, NO_SHADOW_FLAG);
    }

    // Link the object to the frame if it's not a CSG union object,
    // if it's clipped or if bounding slabs aren't used.
    // TODO FIXME - check if bound is used
    if ((Object->Clip.empty() == false) ||
        (dynamic_cast<CSGUnion *>(Object) == nullptr) ||
        (dynamic_cast<CSGMerge *>(Object) != nullptr))
    {
        Link(Object, sceneData->objects);
        return;
    }

    /*
     * [DB 8/94]
     *
     * The object is a CSG union object. It will be split if all siblings are
     * finite, i.e. the volume of the bounding box doesn't exceed a threshold.
     */

    /* NK phmap - added code so union is not split up if it is
                  flagged for hi-density photon mapping...
              maybe we SHOULD split it anyways... do speed tests later */
    if((reinterpret_cast<CSGUnion *>(Object))->do_split == false)
    {
        Link(Object, sceneData->objects);
        return;
    }

    if(!Object->Bound.empty())
    {
        /* Test if all children are finite. */
        bool finite = true;
        DBL Volume;
        for(vector<ObjectPtr>::iterator This_Sib = (reinterpret_cast<CSG *>(Object))->children.begin(); This_Sib != (reinterpret_cast<CSG *>(Object))->children.end(); This_Sib++)
        {
            BOUNDS_VOLUME(Volume, (*This_Sib)->BBox);
            if (Volume > BOUND_HUGE)
            {
                finite = false;
                break;
            }
        }

        /*
         * If the union has infinite children or splitting is not used and
         * the object is not a light group link the union to the frame.
         */

        if (((finite == false) || (sceneData->splitUnions == false)) && ((Object->Type & LIGHT_GROUP_OBJECT) != LIGHT_GROUP_OBJECT))
        {
            if (finite)
                Warning("CSG union unnecessarily bounded.");
            Link(Object, sceneData->objects);
            return;
        }

        Warning("Bounded CSG union split.");
    }

    // Link all children of a union to the frame.
    for(vector<ObjectPtr>::iterator This_Sib = (reinterpret_cast<CSG *>(Object))->children.begin(); This_Sib != (reinterpret_cast<CSG *>(Object))->children.end(); This_Sib++)
    {
        // Child is no longer inside a CSG object.
        (*This_Sib)->Type &= ~IS_CHILD_OBJECT;
        Link_To_Frame(*This_Sib);
    }

    (reinterpret_cast<CSG *>(Object))->children.clear();
    Destroy_Object(Object);
}

//******************************************************************************

void Parser::Only_In(const  char *s1, const char *s2)
{
    Error("Keyword '%s' can only be used in a %s statement.",s1,s2);
}

//******************************************************************************

void Parser::Not_With(const char *s1, const char *s2)
{
    Error("Keyword '%s' cannot be used with %s.",s1,s2);
}

void Parser::Warn_Compat(bool definite, const char *syn)
{
    char isNotText[] = "is not";
    char mayNotText[] = "may not be";
    char *text;

    if (definite)
    {
        text = isNotText;
    }
    else
    {
        text = mayNotText;
    }

    Warning("%s\n"
            "  Use of this syntax %s backwards compatible with earlier versions of POV-Ray.\n"
            "  The #version directive or +MV switch will not help.",
            syn, text);
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
*   Mar 1996 : Add line number info to warning message  [AED]
*
******************************************************************************/

void Parser::Global_Setting_Warn()
{
    if (sceneData->EffectiveLanguageVersion() >= 300)
        PossibleError("'%s' should be in 'global_settings{...}' statement.", CurrentTokenText().c_str());
}

/*****************************************************************************
*
* FUNCTION
*
*  Set_CSG_Children_Hollow
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

void Parser::Set_CSG_Children_Flag(ObjectPtr Object, unsigned int f, unsigned int  flag, unsigned int  set_flag)
{
    for(vector<ObjectPtr>::iterator Sib = (reinterpret_cast<CSG *>(Object))->children.begin(); Sib != (reinterpret_cast<CSG *>(Object))->children.end(); Sib++)
    {
        ObjectPtr p = *Sib;
        if(!Test_Flag (p, set_flag))
        {
            if ((dynamic_cast<CSGUnion *> (p) != nullptr) || // FIXME
                (dynamic_cast<CSGIntersection *> (p) != nullptr) || // FIXME
                (dynamic_cast<CSGMerge *> (p) != nullptr)) // FIXME
            {
                Set_CSG_Children_Flag(p, f, flag, set_flag);
            }
            else
            {
                p->Flags = (p->Flags & (~flag)) | f;
            }
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*  Set_CSG_Tree_Flag
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

void Parser::Set_CSG_Tree_Flag(ObjectPtr Object, unsigned int f, int val)
{
    for(vector<ObjectPtr>::iterator Sib = (reinterpret_cast<CSG *>(Object))->children.begin(); Sib != (reinterpret_cast<CSG *>(Object))->children.end(); Sib++)
    {
        ObjectPtr p = *Sib;
        if ((dynamic_cast<CSGUnion *>(p) != nullptr) || // FIXME
            (dynamic_cast<CSGIntersection *>(p) != nullptr) || // FIXME
            (dynamic_cast<CSGMerge *>(p) != nullptr)) // FIXME
        {
            Set_CSG_Tree_Flag(p, f, val);
        }
        Bool_Flag (p, f, val);
    }
}

//******************************************************************************

/* NK layers - 1999 June 10 - for backwards compatibility with layered textures */
void Parser::Convert_Filter_To_Transmit(PIGMENT *Pigment)
{
    if (!Pigment)
        return;

    switch (Pigment->Type)
    {
        case PLAIN_PATTERN:
            Pigment->colour.SetFT(0.0, 1.0 - Pigment->colour.Opacity());
            break;

        default:
            Convert_Filter_To_Transmit(Pigment->Blend_Map.get());
            break;
    }
}

// NK layers - 1999 July 10 - for backwards compatibility with layered textures
void Parser::Convert_Filter_To_Transmit(GenericPigmentBlendMap *pBlendMap)
{
    if (!pBlendMap)
        return;

    if (PigmentBlendMap* pPBlendMap = dynamic_cast<PigmentBlendMap*>(pBlendMap))
    {
        POV_BLEND_MAP_ASSERT((pPBlendMap->Type == kBlendMapType_Pigment) ||
                             (pPBlendMap->Type == kBlendMapType_Density));
        for (PigmentBlendMap::Vector::iterator i = pPBlendMap->Blend_Map_Entries.begin(); i != pPBlendMap->Blend_Map_Entries.end(); i++)
        {
            Convert_Filter_To_Transmit(i->Vals);
        }
    }
    else if (ColourBlendMap* pCBlendMap = dynamic_cast<ColourBlendMap*>(pBlendMap))
    {
        for (ColourBlendMap::Vector::iterator i = pCBlendMap->Blend_Map_Entries.begin(); i != pCBlendMap->Blend_Map_Entries.end(); i++)
        {
            i->Vals.SetFT(0.0, 1.0 - i->Vals.Opacity());
        }
    }
    else
        POV_BLEND_MAP_ASSERT(false);
}

//******************************************************************************

void Parser::Expectation_Error(const char *s)
{
    Found_Instead_Error("Expected", s);
}

void Parser::SendFatalError(Exception& e)
{
    // if the front-end has been told about this exception already, we don't tell it again
    if (e.frontendnotified(true))
        return;

    PossibleError("%s", e.what());
}

void Parser::Warning(const char *format,...)
{
    va_list marker;
    char localvsbuffer[1024];

    va_start(marker, format);
    std::vsnprintf(localvsbuffer, sizeof(localvsbuffer), format, marker);
    va_end(marker);

    Warning(kWarningGeneral, localvsbuffer);
}

void Parser::Warning(const MessageContext& loc, const char *format, ...)
{
    va_list marker;
    char localvsbuffer[1024];

    va_start(marker, format);
    std::vsnprintf(localvsbuffer, sizeof(localvsbuffer), format, marker);
    va_end(marker);

    Warning(kWarningGeneral, loc, localvsbuffer);
}

void Parser::Warning(WarningLevel level, const char *format,...)
{
    POV_PARSER_ASSERT(level >= kWarningGeneral);

    va_list marker;
    char localvsbuffer[1024];

    va_start(marker, format);
    std::vsnprintf(localvsbuffer, sizeof(localvsbuffer), format, marker);
    va_end(marker);

    if (HaveCurrentMessageContext())
        mMessageFactory.WarningAt(level, CurrentMessageContext(), "%s", localvsbuffer);
    else
        mMessageFactory.Warning(level, "%s", localvsbuffer);
}

void Parser::Warning(WarningLevel level, const MessageContext& loc, const char *format, ...)
{
    POV_PARSER_ASSERT(level >= kWarningGeneral);

    va_list marker;
    char localvsbuffer[1024];

    va_start(marker, format);
    std::vsnprintf(localvsbuffer, sizeof(localvsbuffer), format, marker);
    va_end(marker);

    mMessageFactory.WarningAt(level, loc, "%s", localvsbuffer);
}

void Parser::VersionWarning(unsigned int sinceVersion, const char *format,...)
{
    if(sceneData->EffectiveLanguageVersion() >= sinceVersion)
    {
        va_list marker;
        char localvsbuffer[1024];

        va_start(marker, format);
        std::vsnprintf(localvsbuffer, sizeof(localvsbuffer), format, marker);
        va_end(marker);

        Warning(kWarningLanguage, localvsbuffer);
    }
}

void Parser::PossibleError(const char *format,...)
{
    va_list marker;
    char localvsbuffer[1024];

    va_start(marker, format);
    std::vsnprintf(localvsbuffer, sizeof(localvsbuffer), format, marker);
    va_end(marker);

    if (HaveCurrentMessageContext())
        mMessageFactory.PossibleErrorAt(CurrentMessageContext(), "%s", localvsbuffer);
    else
        mMessageFactory.PossibleError("%s", localvsbuffer);
}

void Parser::Error(const char *format,...)
{
#if POV_BOMB_ON_ERROR
    POV_ASSERT_HARD(false);
#endif

    va_list marker;
    char localvsbuffer[1024];

    va_start(marker, format);
    std::vsnprintf(localvsbuffer, sizeof(localvsbuffer), format, marker);
    va_end(marker);

    if (HaveCurrentMessageContext())
        mMessageFactory.ErrorAt(POV_EXCEPTION(kParseErr, localvsbuffer), CurrentMessageContext(), "%s", localvsbuffer);
    else
        mMessageFactory.Error(POV_EXCEPTION(kParseErr, localvsbuffer), "%s", localvsbuffer);
}

void Parser::Error(const MessageContext& loc, const char *format, ...)
{
#if POV_BOMB_ON_ERROR
    POV_ASSERT_HARD(false);
#endif

    va_list marker;
    char localvsbuffer[1024];

    va_start(marker, format);
    std::vsnprintf(localvsbuffer, sizeof(localvsbuffer), format, marker);
    va_end(marker);

    mMessageFactory.ErrorAt(POV_EXCEPTION(kParseErr, localvsbuffer), loc, "%s", localvsbuffer);
}

void Parser::ErrorInfo(const MessageContext& loc, const char *format,...)
{
    va_list marker;
    char localvsbuffer[1024];

    va_start(marker, format);
    std::vsnprintf(localvsbuffer, sizeof(localvsbuffer), format, marker);
    va_end(marker);

    mMessageFactory.PossibleErrorAt(loc, "%s", localvsbuffer);
}

int Parser::Debug_Info(const char *format,...)
{
    va_list marker;
    char localvsbuffer[1024];

    va_start(marker, format);
    std::vsnprintf(localvsbuffer, sizeof(localvsbuffer), format, marker);
    va_end(marker);

    Debug_Message_Buffer.printf("%s", localvsbuffer);

    return 0;
}

void Parser::FlushDebugMessageBuffer()
{
    Debug_Message_Buffer.flush();
}

Parser::DebugTextStreamBuffer::DebugTextStreamBuffer(GenericMessenger& m) :
    TextStreamBuffer (1024*8, 160),
    mMessenger(m)
{
    // do nothing
}

Parser::DebugTextStreamBuffer::~DebugTextStreamBuffer()
{
    // do nothing
}

void Parser::DebugTextStreamBuffer::lineoutput(const char *str, unsigned int chars)
{
    char buffer[256];

    buffer[0] = 0;
    strncat(buffer, str, min((unsigned int)255, chars));

    mMessenger.UserDebug(buffer);
}

void Parser::DebugTextStreamBuffer::directoutput(const char *, unsigned int)
{
    // do nothing
}

/*****************************************************************************

 FUNCTION

   CheckPassThru()

   Checks to make sure that pass-through, high-density, and refraction
   are not simultaneously selected.  If all three are turned on, we need
   to choose an appropriate one to turn off.

  Preconditions:
    'o' is an initialized object
    'flag' is PH_PASSTHRU_FLAG, PH_TARGET_FLAG, or PH_RFR_ON_FLAG
         (this is which flag was set most recently)

  Postconditions:
    One of these flags in 'o' is turned off, since they cannot all be turned on.

******************************************************************************/

void Parser::CheckPassThru(ObjectPtr o, int flag)
{
    if( Test_Flag(o, PH_PASSTHRU_FLAG) &&
        Test_Flag(o, PH_TARGET_FLAG) &&
        !Test_Flag(o, PH_RFR_OFF_FLAG) )
    {
        switch (flag)
        {
            case PH_PASSTHRU_FLAG:
                Warning("Cannot use pass_through with refraction & target.\nTurning off refraction.");
                Set_Flag(o, PH_RFR_OFF_FLAG);
                Clear_Flag(o, PH_RFR_ON_FLAG);
                break;

            case PH_TARGET_FLAG:
                if(Test_Flag(o, PH_RFR_ON_FLAG))
                {
                    Warning("Cannot use pass_through with refraction & target.\nTurning off pass_through.");
                    Clear_Flag(o,PH_PASSTHRU_FLAG);
                }
                else
                {
                    Warning("Cannot use pass_through with refraction & target.\nTurning off refraction.");
                    Set_Flag(o, PH_RFR_OFF_FLAG);
                    Clear_Flag(o, PH_RFR_ON_FLAG);
                }
                break;

            case PH_RFR_ON_FLAG:
                Warning("Cannot use pass_through with refraction & target.\nTurning off pass_through.");
                Clear_Flag(o, PH_PASSTHRU_FLAG);
                break;
        }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Locate_File
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
*   Find a file in the search path.
*
* CHANGES
*
*   Apr 1996: Don't add trailing FILENAME_SEPARATOR if we are immediately
*             following DRIVE_SEPARATOR because of Amiga probs.  [AED]
*
******************************************************************************/

shared_ptr<IStream> Parser::Locate_File(const UCS2String& filename, unsigned int stype, UCS2String& buffer, bool err_flag)
{
    UCS2String fn(filename);
    UCS2String foundfile(mFileResolver.FindFile(fn, stype));

    if(foundfile.empty() == true)
    {
        if(err_flag == true)
            PossibleError("Cannot find file '%s', even after trying to append file type extension.", UCS2toSysString(fn).c_str());

        return nullptr;
    }

    if(fn.find('.') == UCS2String::npos)
    {
        // the passed-in filename didn't have an extension, but a file has been found,
        // which means one of the appended extensions worked. we need to work out which
        // one and append it to the original filename so we can store it in the cache
        // (since it's that name that the cache search routine looks for).
        UCS2String ext = GetFileExtension(Path(foundfile));
        if (ext.size() != 0)
            fn += ext;
    }

    // ReadFile will store both fn and foundfile in the cache for next time round
    shared_ptr<IStream> result(mFileResolver.ReadFile(fn, foundfile.c_str(), stype));

    if ((result == nullptr) && (err_flag == true))
        PossibleError("Cannot open file '%s'.", UCS2toSysString(foundfile).c_str());

    buffer = foundfile;

    return result;
}
/* TODO FIXME - code above should not be there, this is how it should work but this has bugs [trf]
shared_ptr<IStream> Parser::Locate_File(const UCS2String& filename, unsigned int stype, UCS2String& buffer, bool err_flag)
{
    UCS2String foundfile(mFileResolver.FindFile(filename, stype));

    if(foundfile.empty() == true)
    {
        if(err_flag == true)
            PossibleError("Cannot find file '%s', even after trying to append file type extension.", UCS2toSysString(filename).c_str());

        return nullptr;
    }

    shared_ptr<IStream> result(mFileResolver.ReadFile(foundfile.c_str(), stype));

    if ((result == nullptr) && (err_flag == true))
        PossibleError("Cannot open file '%s'.", UCS2toSysString(foundfile).c_str());

    buffer = foundfile;

    return result;
}
*/

//******************************************************************************

OStream *Parser::CreateFile(const UCS2String& filename, unsigned int stype, bool append)
{
    return mFileResolver.CreateFile(filename, stype, append);
}

//******************************************************************************

Image *Parser::Read_Image(int filetype, const UCS2 *filename, const ImageReadOptions& options)
{
    unsigned int stype;
    Image::ImageFileType type;
    UCS2String ign;

    switch(filetype)
    {
        case GIF_FILE:
            stype = POV_File_Image_GIF;
            type = Image::GIF;
            break;
        case POT_FILE:
            stype = POV_File_Image_GIF;
            type = Image::POT;
            break;
        case SYS_FILE:
            stype = POV_File_Image_System;
            type = Image::SYS;
            break;
        case IFF_FILE:
            stype = POV_File_Image_IFF;
            type = Image::IFF;
            break;
        case TGA_FILE:
            stype = POV_File_Image_Targa;
            type = Image::TGA;
            break;
        case PGM_FILE:
            stype = POV_File_Image_PGM;
            type = Image::PGM;
            break;
        case PPM_FILE:
            stype = POV_File_Image_PPM;
            type = Image::PPM;
            break;
        case PNG_FILE:
            stype = POV_File_Image_PNG;
            type = Image::PNG;
            break;
        case JPEG_FILE:
            stype = POV_File_Image_JPEG;
            type = Image::JPEG;
            break;
        case TIFF_FILE:
            stype = POV_File_Image_TIFF;
            type = Image::TIFF;
            break;
        case BMP_FILE:
            stype = POV_File_Image_BMP;
            type = Image::BMP;
            break;
        case EXR_FILE:
            stype = POV_File_Image_EXR;
            type = Image::EXR;
            break;
        case HDR_FILE:
            stype = POV_File_Image_HDR;
            type = Image::HDR;
            break;
        default:
            throw POV_EXCEPTION(kDataTypeErr, "Unknown file type.");
    }

    shared_ptr<IStream> file = Locate_File(filename, stype, ign, true);

    if (file == nullptr)
        throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot find image file.");

    return Image::Read(type, file.get(), options);
}

//******************************************************************************

RGBFTColour *Parser::Create_Colour ()
{
    return new RGBFTColour();
}

//******************************************************************************

bool Parser::POV_ARRAY::IsInitialized() const
{
    POV_PARSER_ASSERT(resizable || !DataPtrs.empty());
    return !DataPtrs.empty();
}

bool Parser::POV_ARRAY::HasElement(size_t i) const
{
    return ((i < DataPtrs.size()) && (DataPtrs[i] != nullptr));
}

const TokenId& Parser::POV_ARRAY::ElementType(size_t i) const
{
    POV_PARSER_ASSERT(resizable || (i < DataPtrs.size()));
    return (mixedType ? Types[i] : Type_);
}

TokenId& Parser::POV_ARRAY::ElementType(size_t i)
{
    POV_PARSER_ASSERT(resizable || (i < DataPtrs.size()));
    return (mixedType ? Types[i] : Type_);
}

size_t Parser::POV_ARRAY::GetLinearSize() const
{
    POV_PARSER_ASSERT(!resizable || ((maxDim == 0) && (Sizes[0] == DataPtrs.size())));
    POV_PARSER_ASSERT(!mixedType || (Types.size() == DataPtrs.size()));
    return DataPtrs.size();
}

void Parser::POV_ARRAY::Grow()
{
    POV_PARSER_ASSERT(resizable && (maxDim == 0));
    ++Sizes[0];
    DataPtrs.push_back(nullptr);
    POV_PARSER_ASSERT(DataPtrs.size() == Sizes[0]);
    if (mixedType)
    {
        Types.push_back(IDENTIFIER_TOKEN);
        POV_PARSER_ASSERT(Types.size() == Sizes[0]);
    }
}

void Parser::POV_ARRAY::GrowBy(size_t delta)
{
    POV_PARSER_ASSERT(resizable && (maxDim == 0));
    Sizes[0] += delta;
    DataPtrs.insert(DataPtrs.end(), delta, nullptr);
    if (mixedType)
    {
        Types.insert(Types.end(), delta, IDENTIFIER_TOKEN);
        POV_PARSER_ASSERT(Types.size() == Sizes[0]);
    }
}

void Parser::POV_ARRAY::GrowTo(size_t delta)
{
    POV_PARSER_ASSERT(resizable && (maxDim == 0));
    POV_PARSER_ASSERT(delta > Sizes[0]);
    GrowBy(delta - Sizes[0]);
}

void Parser::POV_ARRAY::Shrink()
{
    POV_PARSER_ASSERT(resizable && (maxDim == 0));
    POV_PARSER_ASSERT(Sizes[0] > 0);
    --Sizes[0];
    POV_PARSER_ASSERT(DataPtrs.back() == nullptr);
    DataPtrs.pop_back();
    POV_PARSER_ASSERT(DataPtrs.size() == Sizes[0]);
    if (mixedType)
    {
        POV_PARSER_ASSERT(Types.back() == IDENTIFIER_TOKEN);
        Types.pop_back();
        POV_PARSER_ASSERT(Types.size() == Sizes[0]);
    }
}

Parser::POV_ARRAY::POV_ARRAY(const POV_ARRAY& obj)
{
    maxDim = obj.maxDim;
    Type_ = obj.Type_;
    resizable = obj.resizable;
    mixedType = obj.mixedType;
    for (int i = 0; i < POV_ARRAY::kMaxDimensions; ++i)
    {
        Sizes[i] = obj.Sizes[i];
        Mags[i] = obj.Mags[i];
    }
    DataPtrs.resize(obj.DataPtrs.size());
    for (int i = 0; i < obj.DataPtrs.size(); i++)
        DataPtrs[i] = SymbolTable::Copy_Identifier(obj.DataPtrs[i], obj.ElementType(i));
    Types = obj.Types;
}

Parser::POV_ARRAY::~POV_ARRAY()
{
    for (int i = 0; i < this->DataPtrs.size(); ++i)
        SymbolTable::Destroy_Ident_Data(this->DataPtrs[i], this->ElementType(i));
}

Parser::POV_ARRAY* Parser::POV_ARRAY::Clone() const
{
    return new POV_ARRAY(*this);
}

//******************************************************************************

Parser::AssignableFunction::AssignableFunction(const AssignableFunction& obj) :
    fn(obj.vm->CopyFunction(obj.fn)),
    vm(obj.vm)
{}

Parser::AssignableFunction::AssignableFunction(FUNCTION_PTR fn, boost::intrusive_ptr<FunctionVM> vm) :
    fn(fn),
    vm(vm)
{}

Parser::AssignableFunction::~AssignableFunction()
{
    vm->DestroyFunction(fn);
}

//******************************************************************************

UCS2String Parser::BraceStackEntry::GetFileName() const
{
    return file->Name();
}

//******************************************************************************

}
// end of namespace pov_parser
