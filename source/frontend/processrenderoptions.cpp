//******************************************************************************
///
/// @file frontend/processrenderoptions.cpp
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
//******************************************************************************

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "frontend/processrenderoptions.h"

// C++ variants of C standard header files
#include <cstdlib>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/fileutil.h"
#include "base/platformbase.h"
#include "base/stringutilities.h"
#include "base/image/colourspace.h"
#include "base/image/dither.h"

// POV-Ray header files (core module)
#include "base/fileinputoutput.h"
#include "base/path.h"
#include "base/textstream.h"

// POV-Ray header files (POVMS module)
#include "povms/povmsid.h"

// POV-Ray header files (frontend module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

using namespace pov_base;

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define kUseSpecialHandler   kPOVMSType_WildCard
#define kNoParameter         kPOVMSType_Null

/*****************************************************************************
* Local variables
******************************************************************************/

#ifdef SYS_HAS_GRAYSCALE
#define SYS_GRAYSCALE_FLAG true
#else // SYS_HAS_GRAYSCALE
#define SYS_GRAYSCALE_FLAG false
#endif // SYS_HAS_GRAYSCALE

#ifdef SYS_HAS_ALPHA
#define SYS_ALPHA_FLAG true
#else // SYS_HAS_ALPHA
#define SYS_ALPHA_FLAG false
#endif // SYS_HAS_ALPHA

/*
   Keyword table for the INI-file parser.
   The parser converts the INI-file options into a POVMS object using
   the specifications provided in this table. The first element is the
   INI-file keyword, the second element is the POVMS object attribute
   key, the third is the attribute type. Entries with a POVMS attribute
   key of 0 are obsolete options that will generate a warning that the
   option no longer is supported and will generate an error in a later
   (unspecified) version of POV.
*/
struct ProcessOptions::INI_Parser_Table RenderOptions_INI_Table[] =
{
    { "All_Console",         kPOVAttrib_AllConsole,         kPOVMSType_Bool },
    { "All_File",            kPOVAttrib_AllFile,            kPOVMSType_UCS2String },
    { "Antialias",           kPOVAttrib_Antialias,          kPOVMSType_Bool },
    { "Antialias_Confidence",kPOVAttrib_AntialiasConfidence,kPOVMSType_Float },
    { "Antialias_Depth",     kPOVAttrib_AntialiasDepth,     kPOVMSType_Int },
    { "Antialias_Gamma",     kPOVAttrib_AntialiasGamma,     kPOVMSType_Float },
    { "Antialias_Threshold", kPOVAttrib_AntialiasThreshold, kPOVMSType_Float },
    { "Append_File",         kPOVAttrib_AppendConsoleFiles, kPOVMSType_Bool },

    { "Bits_Per_Color",      kPOVAttrib_BitsPerColor,       kPOVMSType_Int,         kINIOptFlag_SuppressWrite },
    { "Bits_Per_Colour",     kPOVAttrib_BitsPerColor,       kPOVMSType_Int },
    { "Bounding",            kPOVAttrib_Bounding,           kPOVMSType_Bool },
    { "Bounding_Method",     kPOVAttrib_BoundingMethod,     kPOVMSType_Int },
    { "Bounding_Threshold",  kPOVAttrib_BoundingThreshold,  kPOVMSType_Int },
    { "BSP_BaseAccessCost",  kPOVAttrib_BSP_BaseAccessCost, kPOVMSType_Float },
    { "BSP_ChildAccessCost", kPOVAttrib_BSP_ChildAccessCost,kPOVMSType_Float },
    { "BSP_ISectCost",       kPOVAttrib_BSP_ISectCost,      kPOVMSType_Float },
    { "BSP_MaxDepth",        kPOVAttrib_BSP_MaxDepth,       kPOVMSType_Int },
    { "BSP_MissChance",      kPOVAttrib_BSP_MissChance,     kPOVMSType_Float },
    { "Buffer_Output",       0,                             0 },
    { "Buffer_Size",         0,                             0 },

    { "Clock",               kPOVAttrib_Clock,              kPOVMSType_Float },
    { "Clockless_Animation", kPOVAttrib_ClocklessAnimation, kPOVMSType_Bool },
    { "Compression",         kPOVAttrib_Compression,        kPOVMSType_Int },
    { "Continue_Trace",      kPOVAttrib_ContinueTrace,      kPOVMSType_Bool },
    { "Create_Continue_Trace_Log", kPOVAttrib_BackupTrace,  kPOVMSType_Bool },
    { "Create_Histogram",    0,                             0 },
    { "Create_Ini",          kPOVAttrib_CreateIni,          kPOVMSType_UCS2String },
    { "Cyclic_Animation",    kPOVAttrib_CyclicAnimation,    kPOVMSType_Bool },

    { "Debug_Console",       kPOVAttrib_DebugConsole,       kPOVMSType_Bool },
    { "Debug_File",          kPOVAttrib_DebugFile,          kPOVMSType_UCS2String },
    { "Declare",             kPOVAttrib_Declare,            kUseSpecialHandler },
    { "Display",             kPOVAttrib_Display,            kPOVMSType_Bool },
    { "Display_Gamma",       kPOVAttrib_DisplayGamma,       kUseSpecialHandler },
    { "Dither",              kPOVAttrib_Dither,             kPOVMSType_Bool },
    { "Dither_Method",       kPOVAttrib_DitherMethod,       kUseSpecialHandler },
    { "Draw_Vistas",         kPOVAttrib_DrawVistas,         kPOVMSType_Bool },

    { "End_Column",          kPOVAttrib_Right,              kPOVMSType_Float },
    { "End_Row",             kPOVAttrib_Bottom,             kPOVMSType_Float },

    { "Fatal_Console",       kPOVAttrib_FatalConsole,       kPOVMSType_Bool },
    { "Fatal_Error_Command", kPOVAttrib_FatalErrorCommand,  kUseSpecialHandler },
    { "Fatal_Error_Return",  kPOVAttrib_FatalErrorCommand,  kUseSpecialHandler },
    { "Fatal_File",          kPOVAttrib_FatalFile,          kPOVMSType_UCS2String },
    { "Field_Render",        kPOVAttrib_FieldRender,        kPOVMSType_Bool },
    { "File_Gamma",          kPOVAttrib_FileGamma,          kUseSpecialHandler },
    { "Final_Clock",         kPOVAttrib_FinalClock,         kPOVMSType_Float },
    { "Final_Frame",         kPOVAttrib_FinalFrame,         kPOVMSType_Int },
    { "Frame_Step",          kPOVAttrib_FrameStep,          kPOVMSType_Int },

    { "Grayscale_Output",    kPOVAttrib_GrayscaleOutput,    kPOVMSType_Bool },
    { "Greyscale_Output",    kPOVAttrib_GrayscaleOutput,    kPOVMSType_Bool,        kINIOptFlag_SuppressWrite },

    { "Height",              kPOVAttrib_Height,             kPOVMSType_Int },
    { "High_Reproducibility",kPOVAttrib_HighReproducibility,kPOVMSType_Bool },
    { "Histogram_Name",      0,                             0 },
    { "Histogram_Grid_Size", 0,                             0 },
    { "Histogram_Type",      0,                             0 },

    { "Initial_Clock",       kPOVAttrib_InitialClock,       kPOVMSType_Float },
    { "Initial_Frame",       kPOVAttrib_InitialFrame,       kPOVMSType_Int },
    { "Input_File_Name",     kPOVAttrib_InputFile,          kPOVMSType_UCS2String },
    { "Include_Header",      kPOVAttrib_IncludeHeader,      kPOVMSType_UCS2String },
    { "Include_Ini",         kPOVAttrib_IncludeIni,         kUseSpecialHandler },

    { "Jitter_Amount",       kPOVAttrib_JitterAmount,       kPOVMSType_Float },
    { "Jitter",              kPOVAttrib_Jitter,             kPOVMSType_Bool },

    { "Library_Path",        kPOVAttrib_LibraryPath,        kUseSpecialHandler },
    { "Light_Buffer",        kPOVAttrib_LightBuffer,        kPOVMSType_Bool },

    { "Max_Image_Buffer_Memory", kPOVAttrib_MaxImageBufferMem, kPOVMSType_Int },

    { "Odd_Field",           kPOVAttrib_OddField,           kPOVMSType_Bool },
    { "Output_Alpha",        kPOVAttrib_OutputAlpha,        kPOVMSType_Bool },
    { "Output_File_Name",    kPOVAttrib_OutputFile,         kPOVMSType_UCS2String },
    { "Output_File_Type",    kPOVAttrib_OutputFileType,     kUseSpecialHandler },
    { "Output_To_File",      kPOVAttrib_OutputToFile,       kPOVMSType_Bool },

    { "Palette",             kPOVAttrib_Palette,            kUseSpecialHandler },
    { "Pause_When_Done",     kPOVAttrib_PauseWhenDone,      kPOVMSType_Bool },
    { "Post_Frame_Command",  kPOVAttrib_PostFrameCommand,   kUseSpecialHandler },
    { "Post_Frame_Return",   kPOVAttrib_PostFrameCommand,   kUseSpecialHandler },
    { "Post_Scene_Command",  kPOVAttrib_PostSceneCommand,   kUseSpecialHandler },
    { "Post_Scene_Return",   kPOVAttrib_PostSceneCommand,   kUseSpecialHandler },
    { "Preview_End_Size",    kPOVAttrib_PreviewEndSize,     kPOVMSType_Int },
    { "Preview_Start_Size",  kPOVAttrib_PreviewStartSize,   kPOVMSType_Int },
    { "Pre_Frame_Command",   kPOVAttrib_PreFrameCommand,    kUseSpecialHandler },
    { "Pre_Frame_Return",    kPOVAttrib_PreFrameCommand,    kUseSpecialHandler },
    { "Pre_Scene_Command",   kPOVAttrib_PreSceneCommand,    kUseSpecialHandler },
    { "Pre_Scene_Return",    kPOVAttrib_PreSceneCommand,    kUseSpecialHandler },

    { "Quality",             kPOVAttrib_Quality,            kPOVMSType_Int },

    { "Radiosity_File_Name", kPOVAttrib_RadiosityFileName,  kPOVMSType_UCS2String },
    { "Radiosity_From_File", kPOVAttrib_RadiosityFromFile,  kPOVMSType_Bool },
    { "Radiosity_To_File",   kPOVAttrib_RadiosityToFile,    kPOVMSType_Bool },
    { "Radiosity_Vain_Pretrace", kPOVAttrib_RadiosityVainPretrace, kPOVMSType_Bool },
    { "Real_Time_Raytracing",kPOVAttrib_RealTimeRaytracing, kPOVMSType_Bool },
    { "Remove_Bounds",       kPOVAttrib_RemoveBounds,       kPOVMSType_Bool },
    { "Render_Block_Size",   kPOVAttrib_RenderBlockSize,    kPOVMSType_Int },
    { "Render_Block_Step",   kPOVAttrib_RenderBlockStep,    kPOVMSType_Int },
    { "Render_Console",      kPOVAttrib_RenderConsole,      kPOVMSType_Bool },
    { "Render_File",         kPOVAttrib_RenderFile,         kPOVMSType_UCS2String },
    { "Render_Pattern",      kPOVAttrib_RenderPattern,      kPOVMSType_Int },

    { "Sampling_Method",     kPOVAttrib_SamplingMethod,     kPOVMSType_Int },
    { "Split_Unions",        kPOVAttrib_SplitUnions,        kPOVMSType_Bool },
    { "Start_Column",        kPOVAttrib_Left,               kPOVMSType_Float },
    { "Start_Row",           kPOVAttrib_Top,                kPOVMSType_Float },
    { "Statistic_Console",   kPOVAttrib_StatisticsConsole,  kPOVMSType_Bool },
    { "Statistic_File",      kPOVAttrib_StatisticsFile,     kPOVMSType_UCS2String },
    { "Stochastic_Seed",     kPOVAttrib_StochasticSeed,     kPOVMSType_Int },
    { "Subset_End_Frame",    kPOVAttrib_SubsetEndFrame,     kPOVMSType_Float },
    { "Subset_Start_Frame",  kPOVAttrib_SubsetStartFrame,   kPOVMSType_Float },

    { "Test_Abort_Count",    kPOVAttrib_TestAbortCount,     kPOVMSType_Int },
    { "Test_Abort",          kPOVAttrib_TestAbort,          kPOVMSType_Bool },

    { "User_Abort_Command",  kPOVAttrib_UserAbortCommand,   kUseSpecialHandler },
    { "User_Abort_Return",   kPOVAttrib_UserAbortCommand,   kUseSpecialHandler },

    { "Verbose",             kPOVAttrib_Verbose,            kPOVMSType_Bool },
    { "Version",             kPOVAttrib_Version,            kPOVMSType_Float },
    { "Video_Mode",          kPOVAttrib_VideoMode,          kUseSpecialHandler },
    { "Vista_Buffer",        kPOVAttrib_VistaBuffer,        kPOVMSType_Bool },

    { "Warning_Console",     kPOVAttrib_WarningConsole,     kPOVMSType_Bool },
    { "Warning_File",        kPOVAttrib_WarningFile,        kPOVMSType_UCS2String },
    { "Warning_Level",       kPOVAttrib_WarningLevel,       kPOVMSType_Int },
    { "Width",               kPOVAttrib_Width,              kPOVMSType_Int },
    { "Work_Threads",        kPOVAttrib_MaxRenderThreads,   kPOVMSType_Int },

    { nullptr, 0, 0 }
};

/*
   Keyword table for the command line parser.
   The parser converts the command line options into a POVMS object using
   the specifications provided in this table. The first element is the
   command keyword, the second element is the POVMS object attribute key
   of the parameter, the third is the attribute type and the last specifies
   if the +/- switch is used as boolean parameter if an attribute key is
   provided.
*/

/// @attention
///     In this table, options **must** be sorted long strings first; e.g., `AM` must be placed
///     before `A`.
///
struct ProcessOptions::Cmd_Parser_Table RenderOptions_Cmd_Table[] =
{
    //       Parameter setting              Parameter type          Boolean setting

    { "AC",  kPOVAttrib_AntialiasConfidence,kPOVMSType_Float,       kNoParameter },
    { "AG",  kPOVAttrib_AntialiasGamma,     kPOVMSType_Float,       kNoParameter },
    { "AM",  kPOVAttrib_SamplingMethod,     kPOVMSType_Int,         kNoParameter },
    { "A",   kPOVAttrib_AntialiasThreshold, kPOVMSType_Float,       kPOVAttrib_Antialias,           kCmdOptFlag_Optional },

    { "BM",  kPOVAttrib_BoundingMethod,     kPOVMSType_Int,         kNoParameter },
    { "BS",  kPOVAttrib_RenderBlockSize,    kPOVMSType_Int,         kNoParameter },
    { "B",   kNoParameter,                  kNoParameter,           kPOVAttrib_Bounding },

    { "CC",  kNoParameter,                  kNoParameter,           kPOVAttrib_BackupTrace },
    { "C",   kNoParameter,                  kNoParameter,           kPOVAttrib_ContinueTrace },

    { "D",   kPOVAttrib_Display,            kUseSpecialHandler,     kPOVAttrib_Display,             kCmdOptFlag_Optional },

    { "EC",  kPOVAttrib_Right,              kPOVMSType_Float,       kNoParameter },
    { "EF0", kPOVAttrib_SubsetEndFrame,     kPOVMSType_Float,       kNoParameter },
    { "EF",  kPOVAttrib_SubsetEndFrame,     kPOVMSType_Int,         kNoParameter },
    { "EP",  kPOVAttrib_PreviewEndSize,     kPOVMSType_Int,         kNoParameter },
    { "ER",  kPOVAttrib_Bottom,             kPOVMSType_Float,       kNoParameter },

    { "F",   kPOVAttrib_OutputFileType,     kUseSpecialHandler,     kPOVAttrib_OutputToFile,        kCmdOptFlag_Optional },

    { "GA",  kPOVAttrib_AllFile,            kPOVMSType_UCS2String,  kPOVAttrib_AllConsole,          kCmdOptFlag_Optional },
    { "GD",  kPOVAttrib_DebugFile,          kPOVMSType_UCS2String,  kPOVAttrib_DebugConsole,        kCmdOptFlag_Optional },
    { "GF",  kPOVAttrib_FatalFile,          kPOVMSType_UCS2String,  kPOVAttrib_FatalConsole,        kCmdOptFlag_Optional },
    { "GI",  kPOVAttrib_CreateIni,          kPOVMSType_UCS2String,  kNoParameter },
    { "GP",  kNoParameter,                  kNoParameter,           kPOVAttrib_AppendConsoleFiles },
    { "GR",  kPOVAttrib_RenderFile,         kPOVMSType_UCS2String,  kPOVAttrib_RenderConsole,       kCmdOptFlag_Optional },
    { "GS",  kPOVAttrib_StatisticsFile,     kPOVMSType_UCS2String,  kPOVAttrib_StatisticsConsole,   kCmdOptFlag_Optional },
    { "GW",  kPOVAttrib_WarningFile,        kPOVMSType_UCS2String,  kPOVAttrib_WarningConsole,      kCmdOptFlag_Optional },

    { "HI",  kPOVAttrib_IncludeHeader,      kPOVMSType_UCS2String,  kNoParameter },
    { "HR",  kNoParameter,                  kNoParameter,           kPOVAttrib_HighReproducibility },
    { "H",   kPOVAttrib_Height,             kPOVMSType_Int,         kNoParameter },

    { "I",   kPOVAttrib_InputFile,          kPOVMSType_UCS2String,  kNoParameter },

    { "J",   kPOVAttrib_JitterAmount,       kPOVMSType_Float,       kPOVAttrib_Jitter,              kCmdOptFlag_Optional },

    { "KC",  kNoParameter,                  kNoParameter,           kPOVAttrib_CyclicAnimation },
    { "KI",  kPOVAttrib_InitialClock,       kPOVMSType_Float,       kNoParameter },
    { "KFF", kPOVAttrib_FinalFrame,         kPOVMSType_Int,         kNoParameter },
    { "KFI", kPOVAttrib_InitialFrame,       kPOVMSType_Int,         kNoParameter },
    { "KF",  kPOVAttrib_FinalClock,         kPOVMSType_Float,       kNoParameter },
    { "KLA", kNoParameter,                  kNoParameter,           kPOVAttrib_ClocklessAnimation },
    { "K",   kPOVAttrib_Clock,              kPOVMSType_Float,       kNoParameter },

    { "L",   kPOVAttrib_LibraryPath,        kUseSpecialHandler,     kNoParameter },

    { "MB",  kPOVAttrib_BoundingThreshold,  kPOVMSType_Int,         kPOVAttrib_Bounding,            kCmdOptFlag_Optional },
    { "MI",  kPOVAttrib_MaxImageBufferMem,  kPOVMSType_Int,         kNoParameter },
    { "MV",  kPOVAttrib_Version,            kPOVMSType_Float,       kNoParameter },

    { "O",   kPOVAttrib_OutputFile,         kPOVMSType_UCS2String,  kNoParameter },

    { "P",   kNoParameter,                  kNoParameter,           kPOVAttrib_PauseWhenDone },

    { "Q",   kPOVAttrib_Quality,            kPOVMSType_Int,         kNoParameter },

    { "RFI", kNoParameter,                  kNoParameter,           kPOVAttrib_RadiosityFromFile },
    { "RFO", kNoParameter,                  kNoParameter,           kPOVAttrib_RadiosityToFile },
    { "RF",  kPOVAttrib_RadiosityFileName,  kPOVMSType_UCS2String,  kNoParameter },
    { "RS",  kPOVAttrib_RenderBlockStep,    kPOVMSType_Int,         kNoParameter },
    { "RP",  kPOVAttrib_RenderPattern,      kPOVMSType_Int,         kNoParameter },
    { "RTR", kNoParameter,                  kNoParameter,           kPOVAttrib_RealTimeRaytracing },
    { "RVP", kNoParameter,                  kNoParameter,           kPOVAttrib_RadiosityVainPretrace },
    { "R",   kPOVAttrib_AntialiasDepth,     kPOVMSType_Int,         kNoParameter },

    { "SC",  kPOVAttrib_Left,               kPOVMSType_Float,       kNoParameter },
    { "SF0", kPOVAttrib_SubsetStartFrame,   kPOVMSType_Float,       kNoParameter },
    { "SF",  kPOVAttrib_SubsetStartFrame,   kPOVMSType_Int,         kNoParameter },
    { "SP",  kPOVAttrib_PreviewStartSize,   kPOVMSType_Int,         kNoParameter },
    { "SR",  kPOVAttrib_Top,                kPOVMSType_Float,       kNoParameter },
    { "SS",  kPOVAttrib_StochasticSeed,     kPOVMSType_Int,         kNoParameter },
    { "STP", kPOVAttrib_FrameStep,          kPOVMSType_Int,         kNoParameter },
    { "SU",  kNoParameter,                  kNoParameter,           kPOVAttrib_SplitUnions },

    { "TH",  kPOVAttrib_DitherMethod,       kUseSpecialHandler,     kPOVAttrib_Dither,              kCmdOptFlag_Optional },

    { "UA",  kNoParameter,                  kNoParameter,           kPOVAttrib_OutputAlpha },
    { "UD",  kNoParameter,                  kNoParameter,           kPOVAttrib_DrawVistas },
    { "UF",  kNoParameter,                  kNoParameter,           kPOVAttrib_FieldRender },
    { "UL",  kNoParameter,                  kNoParameter,           kPOVAttrib_LightBuffer },
    { "UO",  kNoParameter,                  kNoParameter,           kPOVAttrib_OddField },
    { "UR",  kNoParameter,                  kNoParameter,           kPOVAttrib_RemoveBounds },
    { "UV",  kNoParameter,                  kNoParameter,           kPOVAttrib_VistaBuffer },

    { "V",   kNoParameter,                  kNoParameter,           kPOVAttrib_Verbose },

    { "WL",  kPOVAttrib_WarningLevel,       kPOVMSType_Int,         kNoParameter },
    { "WT",  kPOVAttrib_MaxRenderThreads,   kPOVMSType_Int,         kNoParameter },
    { "W",   kPOVAttrib_Width,              kPOVMSType_Int,         kNoParameter },

    { "X",   kPOVAttrib_TestAbortCount,     kUseSpecialHandler,     kPOVAttrib_TestAbort,           kCmdOptFlag_Optional },

    { nullptr }
};

extern struct ProcessRenderOptions::Parameter_Code_Table DitherMethodTable[];

ProcessRenderOptions::ProcessRenderOptions() : ProcessOptions(RenderOptions_INI_Table, RenderOptions_Cmd_Table)
{
}

ProcessRenderOptions::~ProcessRenderOptions()
{
}

int ProcessRenderOptions::ReadSpecialOptionHandler(INI_Parser_Table *option, char *param, POVMSObjectPtr obj)
{
    POVMSAttributeList list;
    POVMSAttribute attr;
    POVMSObject decobj;
    POVMSObject cmdobj;
    POVMSType typeKey;
    double floatval = 0.0;
    int intval = 0;
    int intval2 = 0;
    int err = kNoErr;

    switch(option->key)
    {
        case kPOVAttrib_Palette:
        case kPOVAttrib_VideoMode:

            while(isspace(*param))
                param++;
            err = POVMSUtil_SetInt(obj, option->key, tolower(*param));
            break;

        case kPOVAttrib_DitherMethod:

            while(isspace(*param))
                param++;
            err = ParseParameterCode(DitherMethodTable, param, &intval);
            if (err == kNoErr)
                err = POVMSUtil_SetInt(obj, option->key, intval);
            else
                ParseError("Unrecognized dither method '%s'.", param);
            break;

        case kPOVAttrib_OutputFileType:

            while(isspace(*param))
                param++;
            err = ParseFileType(*param, option->key, &intval);
            if (err == kNoErr)
                err = POVMSUtil_SetInt(obj, option->key, intval);
            break;

        case kPOVAttrib_IncludeIni:
        case kPOVAttrib_LibraryPath:

            // parse INI file (recursive)
            if(option->key == kPOVAttrib_IncludeIni)
                err = ParseFile(param, obj);

            // create list if it isn't there
            if(err == kNoErr)
            {
                if(POVMSObject_Exist(obj, option->key) == kFalseErr)
                    err = POVMSAttrList_New(&list);
                else if(POVMSObject_Exist(obj, option->key) != kNoErr)
                    err = kObjectAccessErr;
                else
                    err = POVMSObject_Get(obj, &list, option->key);
            }

            // add path or file to list
            if(err == kNoErr)
                err = POVMSAttr_New(&attr);
            if(err == kNoErr)
            {
                err = POVMSAttr_SetUTF8String(&attr, kPOVMSType_UCS2String, param);
                if(err == kNoErr)
                    err = POVMSAttrList_Append(&list, &attr);
                else
                    err = POVMSAttr_Delete(&attr);
            }
            if(err == kNoErr)
                err = POVMSObject_Set(obj, &list, option->key);
            break;

        case kPOVAttrib_Declare:

            // create list if it isn't there
            if(POVMSObject_Exist(obj, option->key) == kFalseErr)
                err = POVMSAttrList_New(&list);
            else if(POVMSObject_Exist(obj, option->key) != kNoErr)
                err = kObjectAccessErr;
            else
                err = POVMSObject_Get(obj, &list, option->key);

            // add value to list
            if(err == kNoErr)
                err = POVMSObject_New(&decobj, kPOVMSType_WildCard);
            if(err == kNoErr)
            {
                char *ptr = nullptr;

                err = POVMSUtil_SetString(&decobj, kPOVAttrib_Identifier, strtok(param, "="));
                if(err == kNoErr)
                {
                    ptr = strtok(nullptr, "");
                    if (ptr == nullptr)
                        err = kParseErr;
                }
                if(err == kNoErr)
                {
                    if (strchr(ptr, '"') != nullptr)
                    {
                        ptr = strchr(ptr, '"') + 1;
                        strtok(ptr, "\"");
                        err = POVMSUtil_SetString(&decobj, kPOVAttrib_Value, ptr);
                    }
                    else
                        err = POVMSUtil_SetFloat(&decobj, kPOVAttrib_Value, std::atof(ptr));
                }
                if(err == kNoErr)
                    err = POVMSAttrList_Append(&list, &decobj);
                else
                    err = POVMSObject_Delete(&decobj);
            }
            if(err == kNoErr)
                err = POVMSObject_Set(obj, &list, option->key);
            break;

        case kPOVAttrib_FatalErrorCommand:
        case kPOVAttrib_PostFrameCommand:
        case kPOVAttrib_PostSceneCommand:
        case kPOVAttrib_PreFrameCommand:
        case kPOVAttrib_PreSceneCommand:
        case kPOVAttrib_UserAbortCommand:

            if(POVMSObject_Exist(obj, option->key) == kNoErr)
                err = POVMSObject_Get(obj, &cmdobj, option->key);
            else
                err = POVMSObject_New(&cmdobj, kPOVMSType_WildCard);
            if(toupper(*(option->keyword + strlen(option->keyword) - 1)) == 'D')
            {
                if(err == kNoErr)
                    err = POVMSUtil_SetString(&cmdobj, kPOVAttrib_CommandString, param);
            }
            else
            {
                if(err == kNoErr)
                {
                    int i = 0;

                    if((*param == '-') || (*param == '!'))
                        i = toupper(*(param + 1)); // use upper-case to indicate negation of process result
                    else
                        i = tolower(*param); // lower-case for normal interpretation of process result
                    err = POVMSUtil_SetInt(&cmdobj, kPOVAttrib_ReturnAction, i);
                }
            }
            if(err == kNoErr)
                err = POVMSObject_Set(obj, &cmdobj, option->key);
            break;

        case kPOVAttrib_AntialiasGamma:
        case kPOVAttrib_DisplayGamma:
        case kPOVAttrib_FileGamma:

            switch (option->key)
            {
                case kPOVAttrib_AntialiasGamma:  typeKey = kPOVAttrib_AntialiasGammaType;    break;
                case kPOVAttrib_DisplayGamma:    typeKey = kPOVAttrib_DisplayGammaType;      break;
                case kPOVAttrib_FileGamma:       typeKey = kPOVAttrib_FileGammaType;         break;
            }
            floatval = std::atof(param);
            if (floatval == 1.0)
                intval = kPOVList_GammaType_Neutral;
            else if (floatval > 0)
                intval = kPOVList_GammaType_PowerLaw;
            else
                err = ParseGammaType(param, &intval);

            if (err == kNoErr)
                err = POVMSUtil_SetFloat(obj, option->key, fabs(floatval));
            if (err == kNoErr)
                err = POVMSUtil_SetInt(obj, typeKey, intval);
            break;
    }

    return err;
}

int ProcessRenderOptions::ReadSpecialSwitchHandler(Cmd_Parser_Table *option, char *param, POVMSObjectPtr obj, bool)
{
    POVMSAttributeList list;
    POVMSAttribute attr;
    int intval = 0;
    int intval2 = 0;
    int err = 0;
    char chr = 0;
    char file_type;
    bool has16BitGrayscale;

    switch(option->key)
    {
        case kPOVAttrib_Display:

            if(param[0] != '\0')
            {
                err = POVMSUtil_SetInt(obj, kPOVAttrib_VideoMode, (int)toupper(param[0]));
                if((param[1] != '\0') && (err == 0))
                    err = POVMSUtil_SetInt(obj, kPOVAttrib_Palette, (int)toupper(param[1]));
            }
            break;

        case kPOVAttrib_DitherMethod:

            err = ParseParameterCode(DitherMethodTable, param, &intval);
            if (err == kNoErr)
                err = POVMSUtil_SetInt(obj, option->key, intval);
            else
                ParseError("Unrecognized dither method '%s'.", param);
            break;

        case kPOVAttrib_OutputFileType:

            err = ParseFileType(*param, option->key, &intval, &has16BitGrayscale);
            if (err == kNoErr)
            {
                err = POVMSUtil_SetInt(obj, option->key, intval);
                file_type = *param++;
            }
            if ((err == kNoErr) && (tolower(*param) == 'g'))
            {
                if(!has16BitGrayscale)
                {
                    ParseError("Grayscale not currently supported with output file format '%c'.", file_type);
                    err = kParamErr;
                }
                else
                {
                    err = POVMSUtil_SetBool(obj, kPOVAttrib_GrayscaleOutput, true);
                    ++param;
                }
            }
            if ((err == kNoErr) && (*param > ' '))
            {
                if (isdigit(*param) != 0)
                {
                    if (sscanf(param, "%d%n", &intval, &intval2) == 1)
                    {
                        if ((err = POVMSUtil_SetInt(obj, kPOVAttrib_BitsPerColor, intval)) == kNoErr)
                        {
                            param += intval2;
                            if (*param > ' ')
                            {
                                ParseError("Unexpected '%s' following bits per color in +F%c option.", param, file_type);
                                err = kParamErr;
                            }
                        }
                    }
                    else
                    {
                        ParseError("Invalid bits per color '%s' found in +F%c option.", param, file_type);
                        err = kParamErr;
                    }
                }
                else
                {
                    ParseError("Invalid modifier '%s' following +F%c option.", param, file_type);
                    err = kParamErr;
                }
            }
            break;

        case kPOVAttrib_LibraryPath:

            // create list if it isn't there
            if(POVMSObject_Exist(obj, option->key) == kFalseErr)
                err = POVMSAttrList_New(&list);
            else if(POVMSObject_Exist(obj, option->key) != kNoErr)
                err = kObjectAccessErr;
            else
                err = POVMSObject_Get(obj, &list, option->key);

            // add path or file to list
            if(err == kNoErr)
                err = POVMSAttr_New(&attr);
            if(err == kNoErr)
            {
                err = POVMSAttr_SetUTF8String(&attr, kPOVMSType_UCS2String, param);
                if(err == kNoErr)
                    err = POVMSAttrList_Append(&list, &attr);
                else
                    err = POVMSAttr_Delete(&attr);
            }
            if(err == kNoErr)
                err = POVMSObject_Set(obj, &list, option->key);
            break;

        case kPOVAttrib_TestAbortCount:

            if((*param) == 0)
                break;
            if(sscanf(param, "%d", &intval) == 1)
                err = POVMSUtil_SetInt(obj, option->key, intval);
            else
            {
                ParseError("No or integer parameter expected for switch '%s', found '%s'.", option->command, param);
                err = kParamErr;
            }
            break;
    }

    return err;
}

int ProcessRenderOptions::WriteSpecialOptionHandler(INI_Parser_Table *option, POVMSObjectPtr obj, OTextStream *file)
{
    POVMSAttributeList list;
    POVMSObject decobj;
    POVMSObject cmdobj;
    POVMSFloat floatval;
    POVMSInt intval;
    int err = 0;
    int l;
    int i,imax;
    POVMSAttribute item;
    char *bufptr;
    char chr;

    switch(option->key)
    {
        case kPOVAttrib_Palette:
        case kPOVAttrib_VideoMode:

            if(POVMSUtil_GetInt(obj, option->key, &intval) == 0)
            {
                chr = intval;
                if(chr > 32)
                    file->printf("%s=%c\n", option->keyword, chr);
            }
            break;

        case kPOVAttrib_OutputFileType:

            if(POVMSUtil_GetInt(obj, option->key, &intval) == 0)
            {
                chr = UnparseFileType(intval);
                if(chr > 32)
                    file->printf("%s=%c\n", option->keyword, chr);
            }
            break;

        case kPOVAttrib_IncludeIni:

            break;

        case kPOVAttrib_Declare:

            err = POVMSObject_Get(obj, &list, option->key);
            if(err != 0)
                break;

            l = 0;
            err = POVMSAttrList_Count(&list, &l);
            if(err != 0)
                break;
            if(l == 0)
                break;

            imax = l;
            for(i = 1; i <= imax; i++)
            {
                err = POVMSAttrList_GetNth(&list, i, &decobj);
                if(err == 0)
                    err = POVMSObject_Get(&decobj, &item, kPOVAttrib_Identifier);
                if(err == 0)
                {
                    l = 0;
                    err = POVMSAttr_Size(&item, &l);
                    if(l > 0)
                    {
                        bufptr = new char[l];
                        bufptr[0] = 0;
                        if((POVMSUtil_GetFloat(&decobj, kPOVAttrib_Value, &floatval) == 0) &&
                           (POVMSAttr_Get(&item, kPOVMSType_CString, bufptr, &l) == 0))
                            file->printf("%s=%s=%g\n", option->keyword, bufptr, (float)floatval);
                        delete[] bufptr;
                    }
                    (void)POVMSAttr_Delete(&item);
                }
            }
            break;

        case kPOVAttrib_LibraryPath:

            err = POVMSObject_Get(obj, &list, option->key);
            if(err != 0)
                break;

            l = 0;
            err = POVMSAttrList_Count(&list, &l);
            if(err != 0)
                break;
            if(l == 0)
                break;

            imax = l;
            for(i = 1; i <= imax; i++)
            {
                err = POVMSAttrList_GetNth(&list, i, &item);
                if(err == 0)
                {
                    l = 0;
                    err = POVMSAttr_Size(&item, &l);
                    if(l > 0)
                    {
                        UTF8String buf;
                        if(POVMSAttr_GetUTF8String(&item, kPOVMSType_UCS2String, buf) == 0)
                            file->printf("%s=\"%s\"\n", option->keyword, buf.c_str());
                    }
                    (void)POVMSAttr_Delete(&item);
                }
            }
            break;

        case kPOVAttrib_FatalErrorCommand:
        case kPOVAttrib_PostFrameCommand:
        case kPOVAttrib_PostSceneCommand:
        case kPOVAttrib_PreFrameCommand:
        case kPOVAttrib_PreSceneCommand:
        case kPOVAttrib_UserAbortCommand:

            err = POVMSObject_Get(obj, &cmdobj, option->key);
            if(err != 0)
                break;

            err = POVMSObject_Get(&cmdobj, &item, kPOVAttrib_CommandString);
            if(err == 0)
            {
                if(toupper(*(option->keyword + strlen(option->keyword) - 1)) == 'D')
                {
                    l = 0;
                    err = POVMSAttr_Size(&item, &l);
                    if(l > 0)
                    {
                        bufptr = new char[l];
                        bufptr[0] = 0;
                        if(POVMSAttr_Get(&item, kPOVMSType_CString, bufptr, &l) == 0)
                            file->printf("%s=%s\n", option->keyword, bufptr);
                        delete[] bufptr;
                    }
                }
                else
                {
                    if(POVMSUtil_GetInt(&cmdobj, kPOVAttrib_ReturnAction, &intval) == 0)
                    {
                        if(intval < 0)
                        {
                            chr = -intval;
                            file->printf("%s=!%c\n", option->keyword, chr);
                        }
                        else
                        {
                            chr = intval;
                            file->printf("%s=%c\n", option->keyword, chr);
                        }
                    }
                }
            }
            if(err == 0)
                err = POVMSObject_Delete(&cmdobj);
            break;
    }

    return err;
}

int ProcessRenderOptions::ProcessUnknownString(char *str, POVMSObjectPtr obj)
{
    POVMSAttributeList list;
    POVMSAttribute attr;
    int state = 0; // INI file
    int err = kNoErr;

    if (str == nullptr)
    {
        ParseError("Expected filename, nothing was found.");
        return kParamErr;
    }

    // add filename or path

    // see if it is a POV file
    if(state == 0)
    {
        char *ptr = strrchr(str, '.');
        if (ptr != nullptr)
        {
            if(pov_stricmp(ptr, ".pov") == 0)
                state = 1; // POV file
        }
    }

    // see if it is a path
    if(state == 0)
    {
        if(strlen(str) > 0)
        {
            if(POV_IS_PATH_SEPARATOR(str[strlen(str) - 1]))
                state = 2; // library path
        }
    }

    switch(state)
    {
        // INI file
        case 0:
            // parse INI file (recursive)
            err = ParseFile(str, obj);
            if(err == kNoErr)
            {
                // create list if it isn't there
                if(POVMSObject_Exist(obj, kPOVAttrib_IncludeIni) == kFalseErr)
                    err = POVMSAttrList_New(&list);
                else if(POVMSObject_Exist(obj, kPOVAttrib_IncludeIni) != kNoErr)
                    err = kObjectAccessErr;
                else
                    err = POVMSObject_Get(obj, &list, kPOVAttrib_IncludeIni);
            }

            // add INI file to list
            if(err == kNoErr)
                err = POVMSAttr_New(&attr);
            if(err == kNoErr)
            {
                err = POVMSAttr_SetUTF8String(&attr, kPOVMSType_UCS2String, str);
                if(err == kNoErr)
                    err = POVMSAttrList_Append(&list, &attr);
                else
                    err = POVMSAttr_Delete(&attr);
            }
            if(err == kNoErr)
                err = POVMSObject_Set(obj, &list, kPOVAttrib_IncludeIni);
            break;
        // POV file
        case 1:
            // set POV file
            err = POVMSUtil_SetUTF8String(obj, kPOVAttrib_InputFile, str);
            break;
        // library path
        case 2:
            // create list if it isn't there
            if(POVMSObject_Exist(obj, kPOVAttrib_LibraryPath) == kFalseErr)
                err = POVMSAttrList_New(&list);
            else if(POVMSObject_Exist(obj, kPOVAttrib_LibraryPath) != kNoErr)
                err = kObjectAccessErr;
            else
                err = POVMSObject_Get(obj, &list, kPOVAttrib_LibraryPath);

            // add library path to list
            if(err == kNoErr)
                err = POVMSAttr_New(&attr);
            if(err == kNoErr)
            {
                err = POVMSAttr_SetUTF8String(&attr, kPOVMSType_UCS2String, str);
                if(err == kNoErr)
                    err = POVMSAttrList_Append(&list, &attr);
                else
                    err = POVMSAttr_Delete(&attr);
            }
            if(err == kNoErr)
                err = POVMSObject_Set(obj, &list, kPOVAttrib_LibraryPath);
            break;
    }

    return err;
}

ITextStream *ProcessRenderOptions::OpenFileForRead(const char *filename, POVMSObjectPtr obj)
{
    return OpenINIFileStream(filename, pov_base::POV_File_Text_INI, obj);
}

OTextStream *ProcessRenderOptions::OpenFileForWrite(const char *filename, POVMSObjectPtr)
{
    return new OTextStream(SysToUCS2String(filename).c_str(), pov_base::POV_File_Text_INI);
}

ITextStream *ProcessRenderOptions::OpenINIFileStream(const char *filename, unsigned int stype, POVMSObjectPtr obj) // TODO FIXME - Use new Path class!
{
    // TODO FIXME - we should join forces with SceneData::FindFile()
    // TODO FIXME - use proper C++ strings instead of C character arrays

    int i,ii,l[POV_FILE_EXTENSIONS_PER_TYPE];
    UTF8String filepath;
    UTF8String libpath;
    UTF8String file_x[POV_FILE_EXTENSIONS_PER_TYPE];
    int cnt = 0;
    int ll;
    POVMSAttribute attr, item;
    const char *xstr = strrchr(filename, '.');
    bool hasextension = ((xstr != nullptr) && (strlen(xstr) <= 4)); // TODO FIXME - we shouldn't rely on extensions being at most 1+3 chars long

    // TODO - the following statement may need reviewing; before it was changed from a macro to a PlatformBase call,
    //        it carried a comment "TODO FIXME - Remove dependency on this macro!!! [trf]".
    if (!PlatformBase::GetInstance().AllowLocalFileAccess (SysToUCS2String(filename), stype, false))
        return nullptr;

    for(i = 0; i < POV_FILE_EXTENSIONS_PER_TYPE; i++)
    {
        if((l[i] = strlen(pov_base::gPOV_File_Extensions[stype].ext[i])) > 0)
        {
            file_x[i] = filename;
            file_x[i] += pov_base::gPOV_File_Extensions[stype].ext[i];
        }
    }

    // Check the current directory first

    if((hasextension == true) && (CheckIfFileExists(filename) == true))
    {
        return new IBufferedTextStream(SysToUCS2String(filename).c_str(), stype);
    }

    for(i = 0; i < POV_FILE_EXTENSIONS_PER_TYPE; i++)
    {
        if(l[i])
        {
            if(CheckIfFileExists(file_x[i]) == true)
            {
                return new IBufferedTextStream(UTF8toUCS2String(file_x[i]).c_str(), stype);
            }
        }
    }

    if(POVMSObject_Get(obj, &attr, kPOVAttrib_LibraryPath) != 0)
        return nullptr;

    if(POVMSAttrList_Count(&attr, &cnt) != 0)
    {
        (void)POVMSAttrList_Delete(&attr);
        return nullptr;
    }

    for (i = 1; i <= cnt; i++)
    {
        (void)POVMSAttr_New(&item);
        if(POVMSAttrList_GetNth(&attr, i, &item) != 0)
            continue;
        ll = 0;
        if(POVMSAttr_Size(&item, &ll) != 0)
        {
            (void)POVMSAttr_Delete(&item);
            continue;
        }
        if(ll <= 0)
        {
            (void)POVMSAttr_Delete(&item);
            continue;
        }
        if(POVMSAttr_GetUTF8String(&item, kPOVMSType_UCS2String, libpath) != 0)
        {
            (void)POVMSAttr_Delete(&item);
            continue;
        }
        (void)POVMSAttr_Delete(&item);

        libpath.push_back(POV_PATH_SEPARATOR);

        filepath = libpath;
        filepath += filename;
        if((hasextension == true) && (CheckIfFileExists(filepath) == true))
        {
            (void)POVMSAttrList_Delete(&attr);
            return new IBufferedTextStream(UTF8toUCS2String(filepath).c_str(), stype);
        }

        for(ii = 0; ii < POV_FILE_EXTENSIONS_PER_TYPE; ii++)
        {
            if(l[ii])
            {
                filepath = libpath;
                filepath += file_x[ii];
                if(CheckIfFileExists(filepath.c_str()) == true)
                {
                    (void)POVMSAttrList_Delete(&attr);
                    return new IBufferedTextStream(UTF8toUCS2String(filepath).c_str(), stype);
                }
            }
        }
    }

    (void)POVMSAttrList_Delete(&attr);

    if(l[0])
        ParseError("Could not find file '%s%s'", filename, pov_base::gPOV_File_Extensions[stype].ext[0]);
    else
        ParseError("Could not find file '%s'", filename);

    return nullptr;
}

// TODO - the following code might need reviewing, according to trf

/* Supported output file types */
struct ProcessRenderOptions::Output_FileType_Table FileTypeTable[] =
{
    // attribute-specific file types (must go first)
    // code, attribute,                     internalId,                         has16BitGrayscale   hasAlpha
    // { 'C',  kPOVAttrib_HistogramFileType,   kPOVList_FileType_CSV,              false,              false },

    // generic file types
    // code, attribute,                     internalId,                         has16BitGrayscale   hasAlpha
    { 'T',  0,                              kPOVList_FileType_Targa,            false,              true },
    { 'C',  0,                              kPOVList_FileType_CompressedTarga,  false,              true },
    { 'N',  0,                              kPOVList_FileType_PNG,              true,               true },
    { 'J',  0,                              kPOVList_FileType_JPEG,             false,              false },
    { 'P',  0,                              kPOVList_FileType_PPM,              true,               false },
    { 'B',  0,                              kPOVList_FileType_BMP,              false,              false /*[1]*/ },
    { 'E',  0,                              kPOVList_FileType_OpenEXR,          false /*[2]*/,      true },
    { 'H',  0,                              kPOVList_FileType_RadianceHDR,      false,              false },
#ifdef POV_SYS_IMAGE_TYPE
    { 'S',  0,                              kPOVList_FileType_System,           SYS_GRAYSCALE_FLAG, SYS_ALPHA_FLAG },
#endif // POV_SYS_IMAGE_TYPE

    //  [1] Alpha support for BMP uses an unofficial extension to the BMP file format, which is not recognized by
    //      most image processing software.

    //  [2] While OpenEXR does support greyscale output at >8 bits, the variants currently supported by POV-Ray
    //      use 16-bit floating-point values with 10 bit mantissa, which might be insufficient for various purposes
    //      such as height fields.

    // end-of-list marker
    { '\0', 0, 0, false }
};

/* Supported special gamma types */
struct ProcessRenderOptions::Parameter_Code_Table GammaTypeTable[] =
{

    // code,        internalId,
    { "BT709",      kPOVList_GammaType_BT709,           "ITU-R BT.709 transfer function" },
    { "BT2020",     kPOVList_GammaType_BT2020,          "ITU-R BT.2020 transfer function" },
    { "SRGB",       kPOVList_GammaType_SRGB,            "sRGB transfer function" },

    // end-of-list marker
    { nullptr,      0,                                  "(unknown)" }
};

/* Supported dither types */
struct ProcessRenderOptions::Parameter_Code_Table DitherMethodTable[] =
{

    // code,    internalId,
    { "AT",     int(DitherMethodId::kAtkinson),         "Atkinson error diffusion" },
    { "B2",     int(DitherMethodId::kBayer2x2),         "2x2 Bayer pattern" },
    { "B3",     int(DitherMethodId::kBayer3x3),         "3x3 Bayer pattern" },
    { "B4",     int(DitherMethodId::kBayer4x4),         "4x4 Bayer pattern" },
    { "BK",     int(DitherMethodId::kBurkes),           "Burkes error diffusion" },
    { "BNX",    int(DitherMethodId::kBlueNoiseX),       "Blue noise pattern (inverted for Red/Blue)" },
    { "BN",     int(DitherMethodId::kBlueNoise),        "Blue noise pattern" },
    { "D1",     int(DitherMethodId::kDiffusion1D),      "Simple 1-D error diffusion" },
    { "D2",     int(DitherMethodId::kSierraLite),       "Simple 2-D error diffusion (Sierra Lite)" },
    { "FS",     int(DitherMethodId::kFloydSteinberg),   "Floyd-Steinberg error diffusion" },
    { "JN",     int(DitherMethodId::kJarvisJudiceNinke),"Jarvis-Judice-Ninke error diffusion" },
    { "S2",     int(DitherMethodId::kSierra2),          "Two-row Sierra error diffusion" },
    { "S3",     int(DitherMethodId::kSierra3),          "Sierra error diffusion" },
    { "ST",     int(DitherMethodId::kStucki),           "Stucki error diffusion" },

    // end-of-list marker
    { nullptr,  0,                                      "(unknown)" }
};

int ProcessRenderOptions::ParseFileType(char code, POVMSType attribute, int* pInternalId, bool* pHas16BitGreyscale)
{
    *pInternalId = kPOVList_FileType_Unknown;
    int err = kNoErr;
    for (int i = 0; FileTypeTable[i].code != '\0'; i ++)
    {
        if ( (toupper(code) == FileTypeTable[i].code) &&
             ((FileTypeTable[i].attribute == 0) || (FileTypeTable[i].attribute == attribute )) )
        {
            if (pHas16BitGreyscale != nullptr)
                *pHas16BitGreyscale = FileTypeTable[i].has16BitGrayscale;
            *pInternalId = FileTypeTable[i].internalId;
            break;
        }
    }
#ifdef OPENEXR_MISSING
    if (*pInternalId == kPOVList_FileType_OpenEXR)
    {
        ParseError(
"This unofficial POV-Ray binary was built without support for the OpenEXR \
file format.  You must either use an official POV-Ray binary or recompile \
the POV-Ray sources on a system providing you with the OpenEXR library \
to make use of this facility.  Alternatively, you may use any of the \
following built-in formats: HDR."
        );
        err = kParamErr;
    }
#endif // OPENEXR_MISSING
    if (*pInternalId == kPOVList_FileType_Unknown)
    {
        ParseError("Unrecognized output file format %c.", code);
        err = kParamErr;
    }
    return err;
}

char ProcessRenderOptions::UnparseFileType(int fileType)
{
    for (int i = 0; FileTypeTable[i].code != '\0'; i ++)
        if (fileType == FileTypeTable[i].internalId)
            return FileTypeTable[i].code;
    return '\0';
}

int ProcessRenderOptions::ParseGammaType(char* code, int* pInternalId)
{
    *pInternalId = kPOVList_GammaType_Unknown;
    int err = ParseParameterCode(GammaTypeTable, code, pInternalId);
    if (err == kParamErr)
        ParseError("Unrecognized gamma setting '%s'.", code);
    return err;
}

const char* ProcessRenderOptions::UnparseGammaType(int gammaType)
{
    return UnparseParameterCode(GammaTypeTable, gammaType);
}

const char* ProcessRenderOptions::GetGammaTypeText(int gammaType)
{
    return GetParameterCodeText(GammaTypeTable, gammaType);
}

const char* ProcessRenderOptions::GetDitherMethodText(int ditherMethod)
{
    return GetParameterCodeText(DitherMethodTable, ditherMethod);
}

int ProcessRenderOptions::ParseParameterCode(const ProcessRenderOptions::Parameter_Code_Table* codeTable, char* code, int* pInternalId)
{
    for (int i = 0; code[i] != '\0'; i ++)
        code[i] = toupper(code[i]);
    for (int i = 0; codeTable[i].code != nullptr; i ++)
    {
        if ( strcmp(code, codeTable[i].code) == 0 )
        {
            *pInternalId = codeTable[i].internalId;
            return kNoErr;
        }
    }
    return kParamErr;
}

const char* ProcessRenderOptions::UnparseParameterCode(const ProcessRenderOptions::Parameter_Code_Table* codeTable, int internalId)
{
    for (int i = 0; codeTable[i].code != nullptr; i ++)
        if (internalId == codeTable[i].internalId)
            return codeTable[i].code;
    return nullptr;
}

const char* ProcessRenderOptions::GetParameterCodeText(const ProcessRenderOptions::Parameter_Code_Table* codeTable, int internalId)
{
    int i;
    for (i = 0; codeTable[i].code != nullptr; i++)
        if (internalId == codeTable[i].internalId)
            break;
    return codeTable[i].text;
}

}
// end of namespace pov_frontend
