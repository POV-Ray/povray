//******************************************************************************
///
/// @file povms/povmsid.h
///
/// This module contains all defines, typedefs, and prototypes for the POVMS
/// interface.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVMSID_H
#define POVMSID_H

/*
 * The following instructions are an example of how to add a POVMS option
 * to POV-Ray, e.g. a new command-line switch or whatever. In this case we
 * are discussing the best way to add the Render_Block_Size option; you
 * should be able to extrapolate from this to whatever it is you intend doing.
 *
 * Take a look at frontend/processrenderoptions.cpp, i.e. Final_Frame in the
 * RenderOptions_INI_Table table. As you see, apart from a name, you need a
 * enum for POVMS. Those are in povms/povmsid.h . Please note that the Enums
 * follow a strict convention there to allow easily tracing back a four
 * character code to the variable. The block size is part of the view/rendered,
 * so it goes into the section marked with the comment "// options handled by
 * view/renderer". As you notice, options are groups by functions and separated
 * by a blank line. Thread specs are a new option, so a new group would be
 * appropriate (just add it at the end of that section).
 *
 * Now to the naming: The four characters are the initial characters of each
 * word in the variable. However, Sometimes that isn't enough, so the rule is
 * to use the initial characters except for the last word if there are less
 * than four characters so far, the second and third characters of the last
 * word are used as well, and those are lower case, for example:
 *
 * kPOVAttrib_DebugConsole -> Debug CONsole -> DCon
 *
 * However, sometimes this does not yield a valid difference, for example:
 *
 * kPOVAttrib_PreSceneCommand -> Pre Scene COmmand -> PSCo
 * kPOVAttrib_PostSceneCommand -> Post Scene COmmand -> PSCo
 *
 * So the rule then goes that the word that makes a difference is expanded, so
 * the "Post" and "Pre", which yields:
 *
 * kPOVAttrib_PreSceneCommand -> PRe Scene Command -> PrSC
 * kPOVAttrib_PostSceneCommand -> POst Scene Command -> PoSC
 *
 * However, there are exptions to this rule as well because the names on the
 * kPOVAttrib_xxx enums folow those in the INI file, which are not always
 * consistent. Hence, for example there would be:
 *
 * kPOVAttrib_SamplingMethod -> Sampling METhod -> SMet
 *
 * BUT, this is an anit-aliasing option, just without a prepended AnitAliasing
 * in the kPOVAttrib_xxx name (because it follows the option name). Thus, here
 * the rule is to implicitly prepend the AnitAliasing, yielding:
 *
 * kPOVAttrib_SamplingMethod -> (Anti Aliasing) Sampling Method -> AASM
 *
 * Then there is yet another rule for files: They should be written with an
 * implied "Name" at the end, i.e.:
 *
 * kPOVAttrib_DebugFile -> Debug File (NAme) -> DFNa
 *
 * The implied "Name" at the end is more for historic reasons, but as it is
 * used throughout, it should be kept as is ;-)
 *
 * So, now that you know how to define it, make sure you select a useful name,
 * i.e. "RenderBlockSize" in the INI option, so you would get "RBSi" for the
 * four character code and an easy to read INI file.
 *
 * After this, also decide if you want an command-line option. Look at the
 * RenderOptions_Cmd_Table table in frontend/processrenderoptions.cpp. Be
 * aware that the order has to be alphabethical in that table, and with the
 * shortest multicharacter option at the end. That is, for example the order
 * is "KFF", "KFI", "KF", "K", ***NOT*** "K", "KF", "KFF", "KFI". The reason
 * is really simple: The command-line parser picks the first match as that
 * makes it easier to implement (no look-ahead).
 *
 * For each of the two tables, the second column defines the POVMS name and the
 * third the POVMS type. There is the additional "kUseSpecialHandler" to
 * support special handling of options - that is, parsing more complex options
 * which commonly yield more than one POVMS attribute or different attributes.
 *
 * For the command-line argument table there is a fourth column. It provides
 * an optional binary POVMS attribute to set depending on the switch, i.e. "-"
 * sets it to "false" while a "+" and "/" set it to "true". I.e. the "UA"
 * command-line option just works via the +/- and doesn't have an argument.
 *
 * NOTE: The default is not stored in the table at all, that is completely
 * inside the core code. That is, if the option isn't specified, it just
 * doesn't have a value.
 *
 * The parsers will automatically handle POVMS types kPOVMSType_Float,
 * kPOVMSType_Int, kPOVMSType_CString and kPOVMSType_Bool and set POVMS
 * attributes accordingly.
 *
 * So once you added the entries to one or both of the tables, the INI and
 * command-line parser can handle the option.
 */

/*****************************************************************************
* NOTE: If you do not understand the code below, stay away! If your compiler
* warns you about the code below, your compiler warning configuration is
* incorrect. Turn off that warning! Do not, ever, mess with the code below to
* just please your compiler. Do not, ever, complain to the POV-Team about the
* code below! Just stay away from this code, please!!!
* RATIONALE: If you do understand the code below and are concerned about
* portability, please direct your attention to the file povms.cpp. There are
* several methods there that do determine the byteorder at runtime. Please
* refer to POVMSStream_Init, POVMSStream_ReadType and POVMSStream_WriteType
* in that file for details. If you have a failure of those functions to
* report, please include as many details about your platform and compiler
* as possible!
******************************************************************************/

// POV-Ray Object Classes
enum
{
    kPOVObjectClass_Rectangle           = 'Rect',
    kPOVObjectClass_ElapsedTime         = 'ETim',

    kPOVObjectClass_IsectStat           = 'ISta',
    kPOVObjectClass_SceneCamera         = 'SCam',

    kPOVObjectClass_ShellCommand        = 'SCmd',
    kPOVObjectClass_IniOptions          = 'IniO',
    kPOVObjectClass_FrontendOptions     = 'FOpt',

    kPOVObjectClass_AnimationOptions    = 'AOpt',
    kPOVObjectClass_OutputOptions       = 'OOpt',
    kPOVObjectClass_ParserOptions       = 'POpt',
    kPOVObjectClass_RenderOptions       = 'ROpt',
    kPOVObjectClass_ParserStatistics    = 'PSta',
    kPOVObjectClass_RenderStatistics    = 'RSta',

    kPOVObjectClass_PlatformData        = 'PlaD',
    kPOVObjectClass_ControlData         = 'CtrD',
    kPOVObjectClass_ResultData          = 'ResD',
    kPOVObjectClass_PixelData           = 'PixD',
    kPOVObjectClass_FileData            = 'FilD',

    kPOVObjectClass_ParserProgress      = 'ParP',
    kPOVObjectClass_BoundingProgress    = 'BouP',
    kPOVObjectClass_PhotonProgress      = 'PhoP',
    kPOVObjectClass_RadiosityProgress   = 'RadP',
    kPOVObjectClass_RenderProgress      = 'RenP',
};

// POV-Ray Message Classes
enum
{
    kPOVMsgClass_BackendControl      = 'BCtr',
    kPOVMsgClass_SceneControl        = 'SCtr',
    kPOVMsgClass_ViewControl         = 'VCtr',
    kPOVMsgClass_SceneOutput         = 'SOut',
    kPOVMsgClass_ViewOutput          = 'VOut',
    kPOVMsgClass_ViewImage           = 'VImg',
    kPOVMsgClass_FileAccess          = 'FAcc',
};

// POV-Ray Message Identifiers
enum
{
    // BackendControl
    kPOVMsgIdent_InitInfo            = 'Info',

    kPOVMsgIdent_CreateScene         = 'CreS',
    kPOVMsgIdent_CloseScene          = 'CloS',

    // SceneControl
    kPOVMsgIdent_CreateView          = 'CreV',
    kPOVMsgIdent_CloseView           = 'CloV',

    kPOVMsgIdent_StartParser         = 'StaP',
    kPOVMsgIdent_StopParser          = 'StpP',
    kPOVMsgIdent_PauseParser         = 'PauP',
    kPOVMsgIdent_ResumeParser        = 'ResP',

    // SceneOutput
    kPOVMsgIdent_ParserStatistics    = 'PSta',

    // ViewControl
    kPOVMsgIdent_StartRender         = 'StaR',
    kPOVMsgIdent_StopRender          = 'StpR',
    kPOVMsgIdent_PauseRender         = 'PauR',
    kPOVMsgIdent_ResumeRender        = 'ResR',

    // ViewOutput
    kPOVMsgIdent_RenderStatistics    = 'RSta',

    // ViewImage
    kPOVMsgIdent_PixelSet            = 'PxSe',
    kPOVMsgIdent_PixelBlockSet       = 'PxBS',
    kPOVMsgIdent_PixelRowSet         = 'RxRS',
    kPOVMsgIdent_RectangleFrameSet   = 'ReFS',
    kPOVMsgIdent_FilledRectangleSet  = 'FiRS',

    // SceneOutput, ViewOutput
    kPOVMsgIdent_Warning             = 'Warn',
    kPOVMsgIdent_Error               = 'ErrW',
    kPOVMsgIdent_FatalError          = 'ErrF',
    kPOVMsgIdent_Debug               = 'Dbug',

    kPOVMsgIdent_Progress            = 'Prog',

    // FileAccess
    kPOVMsgIdent_FindFile            = 'FinF',
    kPOVMsgIdent_ReadFile            = 'ReaF',
    kPOVMsgIdent_CreatedFile         = 'CreF',

    // all
    kPOVMsgIdent_Done                = 'Done',
    kPOVMsgIdent_Failed              = 'Fail',

    // shell command
    kPOVMsgIdent_CmdPreParse         = 'CPrP',
    kPOVMsgIdent_CmdPostParse        = 'CPoP',
    kPOVMsgIdent_CmdPreRender        = 'CPrR',
    kPOVMsgIdent_CmdPostRender       = 'CPoR',
    kPOVMsgIdent_CmdError            = 'CErr',
    kPOVMsgIdent_CmdAbort            = 'CAbo',

    // other
    kPOVMsgIdent_ParserOptions       = 'POpt',
    kPOVMsgIdent_RenderOptions       = 'ROpt',
};

// POV-Ray Message Attributes
enum
{
    kPOVAttrib_ErrorNumber           = 'ErrN',

    kPOVAttrib_SceneId               = 'ScId',
    kPOVAttrib_ViewId                = 'ViId',

    kPOVAttrib_PlatformData          = 'PlaD',
    kPOVAttrib_MaxRenderThreads      = 'MRTh',
    kPOVAttrib_SceneCamera           = 'SCam',

    // universal use
    kPOVAttrib_EnglishText           = 'ETxt',

    // FileAccess
    kPOVAttrib_ReadFile              = 'RFil',
    kPOVAttrib_LocalFile             = 'LFil',
    kPOVAttrib_FileURL               = 'FURL',
    kPOVAttrib_CreatedFile           = 'CFil',

    // backend init
    kPOVAttrib_CoreVersion           = 'Core',
    kPOVAttrib_CoreGeneration        = 'CorG',
    kPOVAttrib_PlatformName          = 'Plat',
    kPOVAttrib_Official              = 'Offi',
    kPOVAttrib_PrimaryDevs           = 'Prim',
    kPOVAttrib_AssistingDevs         = 'Asst',
    kPOVAttrib_ContributingDevs      = 'Cont',
    kPOVAttrib_ImageLibVersions      = 'ILVe',
    kPOVAttrib_Optimizations         = 'Opti',
    kPOVAttrib_CPUInfo               = 'CPUI',
    kPOVAttrib_CPUInfoDetails        = 'CPUD',

    // options handled by frontend
    kPOVAttrib_TestAbort             = 'TstA', // currently not supported by code
    kPOVAttrib_TestAbortCount        = 'TsAC', // currently not supported by code
    kPOVAttrib_PauseWhenDone         = 'PWDo',

    kPOVAttrib_ContinueTrace         = 'ConT',
    kPOVAttrib_BackupTrace           = 'BacT',

    kPOVAttrib_Verbose               = 'Verb',
    kPOVAttrib_DebugConsole          = 'DCon',
    kPOVAttrib_FatalConsole          = 'FCon',
    kPOVAttrib_RenderConsole         = 'RCon',
    kPOVAttrib_StatisticsConsole     = 'SCon',
    kPOVAttrib_WarningConsole        = 'WCon',
    kPOVAttrib_AllConsole            = 'ACon',
    kPOVAttrib_DebugFile             = 'DFNa',
    kPOVAttrib_FatalFile             = 'FFNa',
    kPOVAttrib_RenderFile            = 'RFNa',
    kPOVAttrib_StatisticsFile        = 'SFNa',
    kPOVAttrib_WarningFile           = 'WFNa',
    kPOVAttrib_AllFile               = 'AFNa',
    kPOVAttrib_AppendConsoleFiles    = 'ACFi',

    kPOVAttrib_Display               = 'Disp',
    kPOVAttrib_VideoMode             = 'VMod', // currently not supported by code
    kPOVAttrib_Palette               = 'Palt', // currently not supported by code
    kPOVAttrib_DisplayGamma          = 'DGam',
    kPOVAttrib_DisplayGammaType      = 'DGaT',
    kPOVAttrib_FileGamma             = 'FGam',
    kPOVAttrib_FileGammaType         = 'FGaT',
    kPOVAttrib_LegacyGammaMode       = 'LGaM',
    kPOVAttrib_WorkingGammaType      = 'WGaT',
    kPOVAttrib_WorkingGamma          = 'WGam',
    kPOVAttrib_ViewingGamma          = 'VGam',
    kPOVAttrib_DitherMethod          = 'DitM',
    kPOVAttrib_Dither                = 'Dith',

    kPOVAttrib_InitialFrame          = 'IFrm',
    kPOVAttrib_FinalFrame            = 'FFrm',
    kPOVAttrib_InitialClock          = 'IClk',
    kPOVAttrib_FinalClock            = 'FClk',
    kPOVAttrib_SubsetStartFrame      = 'SStF',
    kPOVAttrib_SubsetEndFrame        = 'SEnF',
    kPOVAttrib_CyclicAnimation       = 'CylA',
    kPOVAttrib_FieldRender           = 'FldR', // currently not supported by code
    kPOVAttrib_OddField              = 'OddF', // currently not supported by code
    kPOVAttrib_FrameStep             = 'FStp',

    kPOVAttrib_OutputToFile          = 'OToF',
    kPOVAttrib_OutputFileType        = 'OFTy',
    kPOVAttrib_OutputAlpha           = 'OAlp',
    kPOVAttrib_BitsPerColor          = 'BPCo',
    kPOVAttrib_GrayscaleOutput       = 'GOut',
    kPOVAttrib_OutputFile            = 'OFNa',
    kPOVAttrib_OutputPath            = 'OPat',
    kPOVAttrib_Compression           = 'OFCo',

    kPOVAttrib_HistogramFileType     = 'HFTy', // currently not supported by code
    kPOVAttrib_HistogramFile         = 'HFNa', // currently not supported by code
    kPOVAttrib_HistogramGridSizeX    = 'HGSX', // currently not supported by code
    kPOVAttrib_HistogramGridSizeY    = 'HGSY', // currently not supported by code

    kPOVAttrib_PreSceneCommand       = 'PrSC',
    kPOVAttrib_PreFrameCommand       = 'PrFC',
    kPOVAttrib_PostSceneCommand      = 'PoSc',
    kPOVAttrib_PostFrameCommand      = 'PoFC',
    kPOVAttrib_UserAbortCommand      = 'UAbC',
    kPOVAttrib_FatalErrorCommand     = 'FErC',
    kPOVAttrib_CommandString         = 'ComS',
    kPOVAttrib_ReturnAction          = 'RAct',

    kPOVAttrib_CreateIni             = 'CIni',
    kPOVAttrib_LibraryPath           = 'LibP',
    kPOVAttrib_IncludeIni            = 'IncI',

    // options handled by scene/parser
    kPOVAttrib_InputFile             = 'IFNa',
    kPOVAttrib_IncludeHeader         = 'IncH',

    kPOVAttrib_WarningLevel          = 'WLev',
    kPOVAttrib_Declare               = 'Decl',
    kPOVAttrib_Clock                 = 'Clck',
    kPOVAttrib_ClocklessAnimation    = 'Ckla',
    kPOVAttrib_RealTimeRaytracing    = 'RTRa',
    kPOVAttrib_Version               = 'Vers',

    // options handled by view/renderer
    kPOVAttrib_Height                = 'Heig',
    kPOVAttrib_Width                 = 'Widt',

    kPOVAttrib_Left                  = 'Left',
    kPOVAttrib_Top                   = 'Top ',
    kPOVAttrib_Right                 = 'Righ',
    kPOVAttrib_Bottom                = 'Bott',

    kPOVAttrib_Antialias             = 'Anti',
    kPOVAttrib_SamplingMethod        = 'AASM',
    kPOVAttrib_AntialiasThreshold    = 'AATh',
    kPOVAttrib_AntialiasConfidence   = 'AACo',
    kPOVAttrib_AntialiasDepth        = 'AADe',
    kPOVAttrib_Jitter                = 'AAJi',
    kPOVAttrib_JitterAmount          = 'AAJA',
    kPOVAttrib_AntialiasGamma        = 'AAGa',
    kPOVAttrib_AntialiasGammaType    = 'AAGT', // currently not supported by code
    kPOVAttrib_Quality               = 'Qual',
    kPOVAttrib_HighReproducibility   = 'HRep',
    kPOVAttrib_StochasticSeed        = 'Seed',

    kPOVAttrib_Bounding              = 'Boun',
    kPOVAttrib_BoundingMethod        = 'BdMe',
    kPOVAttrib_BoundingThreshold     = 'BdTh',
    kPOVAttrib_BSP_MaxDepth          = 'BspD',
    kPOVAttrib_BSP_ISectCost         = 'BspI',
    kPOVAttrib_BSP_BaseAccessCost    = 'BspB',
    kPOVAttrib_BSP_ChildAccessCost   = 'BspC',
    kPOVAttrib_BSP_MissChance        = 'BspM',
    kPOVAttrib_LightBuffer           = 'LBuf', // currently not supported by code
    kPOVAttrib_VistaBuffer           = 'VBuf', // currently not supported by code
    kPOVAttrib_RemoveBounds          = 'RmBd',
    kPOVAttrib_SplitUnions           = 'SplU',

    kPOVAttrib_CreateHistogram       = 'CHis', // currently not supported by code
    kPOVAttrib_DrawVistas            = 'DrVi', // currently not supported by code

    kPOVAttrib_PreviewStartSize      = 'PStS',
    kPOVAttrib_PreviewEndSize        = 'PEnS',

    kPOVAttrib_RadiosityFileName     = 'RaFN',
    kPOVAttrib_RadiosityFromFile     = 'RaFF',
    kPOVAttrib_RadiosityToFile       = 'RaTF',
    kPOVAttrib_RadiosityVainPretrace = 'RaVP',

    kPOVAttrib_RenderBlockSize       = 'RBSi',

    kPOVAttrib_MaxImageBufferMem     = 'MIBM', // [JG] for file backed image

    kPOVAttrib_CameraIndex           = 'CIdx',

    // time statistics generated by frontend
    kPOVAttrib_TotalTime             = 'TotT',
    kPOVAttrib_FrameTime             = 'FTim',
    kPOVAttrib_AnimationTime         = 'ATim',

    // time statistics generated by backend
    kPOVAttrib_ParseTime             = 'ParT',
    kPOVAttrib_BoundingTime          = 'BouT',
    kPOVAttrib_PhotonTime            = 'PhoT',
    kPOVAttrib_RadiosityTime         = 'RadT',
    kPOVAttrib_TraceTime             = 'TraT',

    // statistics generated by frontend
    kPOVAttrib_CurrentFrame          = 'CurF',
    kPOVAttrib_FrameCount            = 'FCnt',
    kPOVAttrib_AbsoluteCurFrame      = 'AbsF',
    kPOVAttrib_FirstClock            = 'FirC',
    kPOVAttrib_CurrentClock          = 'CurC',
    kPOVAttrib_LastClock             = 'LasC',

    // statistics generated by scene/parser
    kPOVAttrib_FiniteObjects         = 'FiOb',
    kPOVAttrib_InfiniteObjects       = 'InOb',
    kPOVAttrib_LightSources          = 'LiSo',
    kPOVAttrib_Cameras               = 'Cama',

    // statistics generated by scene/bounding
    kPOVAttrib_BSPNodes              = 'BNod',
    kPOVAttrib_BSPSplitNodes         = 'BSNo',
    kPOVAttrib_BSPObjectNodes        = 'BONo',
    kPOVAttrib_BSPEmptyNodes         = 'BENo',
    kPOVAttrib_BSPMaxObjects         = 'BMOb',
    kPOVAttrib_BSPAverageObjects     = 'BAOb',
    kPOVAttrib_BSPMaxDepth           = 'BMDe',
    kPOVAttrib_BSPAverageDepth       = 'BADe',
    kPOVAttrib_BSPAborts             = 'BAbo',
    kPOVAttrib_BSPAverageAborts      = 'BAAb',
    kPOVAttrib_BSPAverageAbortObjects = 'BAAO',

    // statistics generated by view/render (radiosity)
    kPOVAttrib_RadGatherCount        = 'RGCt',
    kPOVAttrib_RadUnsavedCount       = 'RUCo',
    kPOVAttrib_RadReuseCount         = 'RRCt',
    kPOVAttrib_RadRayCount           = 'RYCt',
    kPOVAttrib_RadTopLevelGatherCount= 'RGCT',
    kPOVAttrib_RadTopLevelReuseCount = 'RRCT',
    kPOVAttrib_RadTopLevelRayCount   = 'RYCT',
    kPOVAttrib_RadFinalGatherCount   = 'RGCF',
    kPOVAttrib_RadFinalReuseCount    = 'RRCF',
    kPOVAttrib_RadFinalRayCount      = 'RYCF',
    kPOVAttrib_RadOctreeNodes        = 'ROcN',
    kPOVAttrib_RadOctreeLookups      = 'ROcL',
    kPOVAttrib_RadOctreeAccepts0     = 'ROc0',
    kPOVAttrib_RadOctreeAccepts1     = 'ROc1',
    kPOVAttrib_RadOctreeAccepts2     = 'ROc2',
    kPOVAttrib_RadOctreeAccepts3     = 'ROc3',
    kPOVAttrib_RadOctreeAccepts4     = 'ROc4',
    kPOVAttrib_RadOctreeAccepts5     = 'ROc5',
    // [CLi] per-pass per-recursion sample count statistics
    // (Note: Do not change the IDs of any of these "just for fun"; at several places they are computed from the first one)
    kPOVAttrib_RadSamplesP1R0        = 'RS10',
    kPOVAttrib_RadSamplesP1R1        = 'RS11',
    kPOVAttrib_RadSamplesP1R2        = 'RS12',
    kPOVAttrib_RadSamplesP1R3        = 'RS13',
    kPOVAttrib_RadSamplesP1R4ff      = 'RS14',
    kPOVAttrib_RadSamplesP2R0        = 'RS20',
    kPOVAttrib_RadSamplesP2R1        = 'RS21',
    kPOVAttrib_RadSamplesP2R2        = 'RS22',
    kPOVAttrib_RadSamplesP2R3        = 'RS23',
    kPOVAttrib_RadSamplesP2R4ff      = 'RS24',
    kPOVAttrib_RadSamplesP3R0        = 'RS30',
    kPOVAttrib_RadSamplesP3R1        = 'RS31',
    kPOVAttrib_RadSamplesP3R2        = 'RS32',
    kPOVAttrib_RadSamplesP3R3        = 'RS33',
    kPOVAttrib_RadSamplesP3R4ff      = 'RS34',
    kPOVAttrib_RadSamplesP4R0        = 'RS40',
    kPOVAttrib_RadSamplesP4R1        = 'RS41',
    kPOVAttrib_RadSamplesP4R2        = 'RS42',
    kPOVAttrib_RadSamplesP4R3        = 'RS43',
    kPOVAttrib_RadSamplesP4R4ff      = 'RS44',
    kPOVAttrib_RadSamplesP5ffR0      = 'RS50',
    kPOVAttrib_RadSamplesP5ffR1      = 'RS51',
    kPOVAttrib_RadSamplesP5ffR2      = 'RS52',
    kPOVAttrib_RadSamplesP5ffR3      = 'RS53',
    kPOVAttrib_RadSamplesP5ffR4ff    = 'RS54',
    kPOVAttrib_RadSamplesFR0         = 'RSF0',
    kPOVAttrib_RadSamplesFR1         = 'RSF1',
    kPOVAttrib_RadSamplesFR2         = 'RSF2',
    kPOVAttrib_RadSamplesFR3         = 'RSF3',
    kPOVAttrib_RadSamplesFR4ff       = 'RSF4',
    kPOVAttrib_RadWeightR0           = 'RWt0',
    kPOVAttrib_RadWeightR1           = 'RWt1',
    kPOVAttrib_RadWeightR2           = 'RWt2',
    kPOVAttrib_RadWeightR3           = 'RWt3',
    kPOVAttrib_RadWeightR4ff         = 'RWt4',
    kPOVAttrib_RadQueryCountR0       = 'RQC0',
    kPOVAttrib_RadQueryCountR1       = 'RQC1',
    kPOVAttrib_RadQueryCountR2       = 'RQC2',
    kPOVAttrib_RadQueryCountR3       = 'RQC3',
    kPOVAttrib_RadQueryCountR4ff     = 'RQC4',

    // statistics generated by view/render (photons)
    kPOVAttrib_TotalPhotonCount      = 'TPCn',
    kPOVAttrib_ObjectPhotonCount     = 'OPCn',
    kPOVAttrib_MediaPhotonCount      = 'MPCn',
    kPOVAttrib_PhotonXSamples        = 'PXSa',
    kPOVAttrib_PhotonYSamples        = 'PYSa',
    kPOVAttrib_PhotonsShot           = 'PSho',
    kPOVAttrib_PhotonsStored         = 'PSto',
    kPOVAttrib_GlobalPhotonsStored   = 'GPSt',
    kPOVAttrib_MediaPhotonsStored    = 'MPSt',
    kPOVAttrib_PhotonsPriQInsert     = 'PPQI',
    kPOVAttrib_PhotonsPriQRemove     = 'PPQR',
    kPOVAttrib_GatherPerformedCnt    = 'GPCn',
    kPOVAttrib_GatherExpandedCnt     = 'GECn',

    // render progress and statistics generated by view/render (trace)
    kPOVAttrib_Pixels                = 'Pixe',
    kPOVAttrib_PixelSamples          = 'PixS',
    kPOVAttrib_SuperSampleCount      = 'SSCn',

    // statistics generated by view/render (all)
    kPOVAttrib_Rays                  = 'Rays',
    kPOVAttrib_RaysSaved             = 'RSav',

    kPOVAttrib_TraceLevel            = 'TLev',
    kPOVAttrib_MaxTraceLevel         = 'MaxL',

    kPOVAttrib_ShadowTest            = 'ShdT',
    kPOVAttrib_ShadowTestSuc         = 'ShdS',
    kPOVAttrib_ShadowCacheHits       = 'ShdC',

    kPOVAttrib_PolynomTest           = 'PnmT',
    kPOVAttrib_RootsEliminated       = 'REli',

    kPOVAttrib_CallsToNoise          = 'CTNo',
    kPOVAttrib_CallsToDNoise         = 'CTDN',

    kPOVAttrib_MediaSamples          = 'MeSa',
    kPOVAttrib_MediaIntervals        = 'MeIn',

    kPOVAttrib_ReflectedRays         = 'RflR',
    kPOVAttrib_InnerReflectedRays    = 'IReR',
    kPOVAttrib_RefractedRays         = 'RfrT',
    kPOVAttrib_TransmittedRays       = 'TraR',

    kPOVAttrib_IsoFindRoot           = 'IFRo',
    kPOVAttrib_FunctionVMCalls       = 'FVMC',
    kPOVAttrib_FunctionVMInstrEst    = 'FVMI',

    kPOVAttrib_CrackleCacheTest      = 'CrCT',
    kPOVAttrib_CrackleCacheTestSuc   = 'CrCS',

    kPOVAttrib_ObjectIStats          = 'OISt',
    kPOVAttrib_ISectsTests           = 'ITst',
    kPOVAttrib_ISectsSucceeded       = 'ISuc',

    kPOVAttrib_MinAlloc              = 'MinA',
    kPOVAttrib_MaxAlloc              = 'MaxA',
    kPOVAttrib_CallsToAlloc          = 'CTAl',
    kPOVAttrib_CallsToFree           = 'CTFr',
    kPOVAttrib_PeakMemoryUsage       = 'PMUs',

    // subject to elimination
    kPOVAttrib_BoundingQueues        = 'BQue',
    kPOVAttrib_BoundingQueueResets   = 'BQRs',
    kPOVAttrib_BoundingQueueResizes  = 'BQRz',
    kPOVAttrib_IStackOverflow        = 'IStO',
    kPOVAttrib_ObjectName            = 'ONam',
    kPOVAttrib_ObjectID              = 'OIde',

    // time statistics and progress reporting
    kPOVAttrib_RealTime              = 'ReaT',
    kPOVAttrib_CPUTime               = 'CPUT',
    kPOVAttrib_TimeSamples           = 'TSam',

    // parser progress
    kPOVAttrib_CurrentTokenCount     = 'CTCo',

    // bounding progress
    kPOVAttrib_CurrentNodeCount      = 'CNCo',

    // photon progress
    kPOVAttrib_CurrentPhotonCount    = 'CPCo',

    // render progress
    kPOVAttrib_PixelsPending         = 'PPen',
    kPOVAttrib_PixelsCompleted       = 'PCom',

    // render pixel data/control
    kPOVAttrib_PixelId               = 'PiId',  ///< (Int) ID of render block; only set if rendered to completion.
    kPOVAttrib_PixelSize             = 'PiSi',
    kPOVAttrib_PixelBlock            = 'PBlo',
    kPOVAttrib_PixelColors           = 'PCol',
    kPOVAttrib_PixelPositions        = 'PPos',
    kPOVAttrib_PixelSkipList         = 'PSLi',
    kPOVAttrib_PixelFinal            = 'PFin',  ///< (Void) Set if pixel data is relevant for final image.

    // scene/view error reporting and TBD
    kPOVAttrib_CurrentLine           = 'CurL',
    kPOVAttrib_LineCount             = 'LCnt',
    kPOVAttrib_AbsoluteCurrentLine   = 'AbsL',
    kPOVAttrib_FileName              = 'File',
    kPOVAttrib_State                 = 'Stat',
    kPOVAttrib_Warning               = 'Warn',
    kPOVAttrib_Line                  = 'Line',
    kPOVAttrib_Column                = 'Colu',
    kPOVAttrib_FilePosition          = 'FPos',
    kPOVAttrib_TokenName             = 'TokN',
    kPOVAttrib_Error                 = 'Erro',
    kPOVAttrib_INIFile               = 'IFil',
    kPOVAttrib_RenderOptions         = 'ROpt',
    kPOVAttrib_Identifier            = 'Iden',
    kPOVAttrib_Value                 = 'Valu',
    kPOVAttrib_ProgressStatus        = 'ProS',
    kPOVAttrib_MosaicPreviewSize     = 'MPSi',

    // Rendering order
    kPOVAttrib_RenderBlockStep       = 'RBSt',
    kPOVAttrib_RenderPattern         = 'RPat',

    // helpers
    kPOVAttrib_StartColumn           = kPOVAttrib_Left,
    kPOVAttrib_EndColumn             = kPOVAttrib_Right,
    kPOVAttrib_StartRow              = kPOVAttrib_Top,
    kPOVAttrib_EndRow                = kPOVAttrib_Bottom
};

enum
{
    kPOVList_FileType_Unknown,
    kPOVList_FileType_Targa,
    kPOVList_FileType_CompressedTarga,
    kPOVList_FileType_PNG,
    kPOVList_FileType_JPEG,
    kPOVList_FileType_PPM,
    kPOVList_FileType_BMP,
    kPOVList_FileType_OpenEXR,
    kPOVList_FileType_RadianceHDR,
    kPOVList_FileType_System,
    kPOVList_FileType_CSV, // used for histogram file
};

#endif // POVMSID_H
