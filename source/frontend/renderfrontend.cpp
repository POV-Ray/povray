//******************************************************************************
///
/// @file frontend/renderfrontend.cpp
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
#include "frontend/renderfrontend.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/mathutil.h"
#include "base/platformbase.h"
#include "base/textstream.h"
#include "base/textstreambuffer.h"
#include "base/types.h"
#include "base/image/colourspace.h"
#include "base/image/dither.h"

// POV-Ray header files (POVMS module)
#include "povms/povmsid.h"

// POV-Ray header files (frontend module)
#include "frontend/console.h"
#include "frontend/processoptions.h"
#include "frontend/processrenderoptions.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

using std::min;
using std::max;

const int gStreamTypeUtilDataCount = 6;

static const POVMSType gStreamTypeUtilData[gStreamTypeUtilDataCount] =
{
    kPOVAttrib_DebugFile,
    kPOVAttrib_FatalFile,
    kPOVAttrib_RenderFile,
    kPOVAttrib_StatisticsFile,
    kPOVAttrib_WarningFile,
    kPOVAttrib_AllFile
};

static const char *gStreamDefaultFile[gStreamTypeUtilDataCount] =
{
    "debug.out",
    "fatal.out",
    "render.out",
    "stats.out",
    "warning.out",
    "alltext.out"
};

static const int gStreamNumber[gStreamTypeUtilDataCount] =
{
    DEBUG_STREAM,
    FATAL_STREAM,
    RENDER_STREAM,
    STATISTIC_STREAM,
    WARNING_STREAM,
    ALL_STREAM
};

namespace Message2Console
{

void InitInfo(POVMS_Object& cppmsg, TextStreamBuffer *tsb);

void FileMessage(TextStreamBuffer *tsb, int stream, POVMSObjectPtr msg);
const char *GetOptionSwitchString(POVMSObjectPtr msg, POVMSType key, bool defaultstate);

}
// end of namespace Message2Console

class FileTextStreamBuffer final : public TextStreamBuffer
{
    public:
        FileTextStreamBuffer(const UCS2 *filename, bool append) : stream(filename, POV_File_Text_Stream, append) { }
        virtual ~FileTextStreamBuffer() override { flush(); stream.flush(); }
    protected:
        virtual void lineoutput(const char *str, unsigned int chars) override { stream.printf("%s\n", std::string(str, chars).c_str()); stream.flush(); }
    private:
        OTextStream stream;
};

RenderFrontendBase::RenderFrontendBase(POVMSContext ctx) :
    POVMS_MessageReceiver(ctx),
    context(ctx)
{
    InstallFront(kPOVMsgClass_BackendControl, kPOVMsgIdent_Failed, this, &RenderFrontendBase::HandleMessage);

    InstallFront(kPOVMsgClass_SceneOutput, kPOVMsgIdent_ParserStatistics, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_SceneOutput, kPOVMsgIdent_Warning, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_SceneOutput, kPOVMsgIdent_Error, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_SceneOutput, kPOVMsgIdent_FatalError, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_SceneOutput, kPOVMsgIdent_Debug, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_SceneOutput, kPOVMsgIdent_Progress, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_SceneOutput, kPOVMsgIdent_Done, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_SceneOutput, kPOVMsgIdent_Failed, this, &RenderFrontendBase::HandleMessage);

    InstallFront(kPOVMsgClass_ViewOutput, kPOVMsgIdent_RenderStatistics, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewOutput, kPOVMsgIdent_Warning, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewOutput, kPOVMsgIdent_Error, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewOutput, kPOVMsgIdent_FatalError, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewOutput, kPOVMsgIdent_Debug, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewOutput, kPOVMsgIdent_Progress, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewOutput, kPOVMsgIdent_Done, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewOutput, kPOVMsgIdent_Failed, this, &RenderFrontendBase::HandleMessage);

    InstallFront(kPOVMsgClass_ViewImage, kPOVMsgIdent_PixelSet, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewImage, kPOVMsgIdent_PixelBlockSet, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewImage, kPOVMsgIdent_PixelRowSet, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewImage, kPOVMsgIdent_RectangleFrameSet, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_ViewImage, kPOVMsgIdent_FilledRectangleSet, this, &RenderFrontendBase::HandleMessage);

    InstallFront(kPOVMsgClass_FileAccess, kPOVMsgIdent_FindFile, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_FileAccess, kPOVMsgIdent_ReadFile, this, &RenderFrontendBase::HandleMessage);
    InstallFront(kPOVMsgClass_FileAccess, kPOVMsgIdent_CreatedFile, this, &RenderFrontendBase::HandleMessage);
}

RenderFrontendBase::~RenderFrontendBase()
{
    // nothing to do
}

void RenderFrontendBase::ConnectToBackend(POVMSAddress backendaddress, POVMS_Object& obj, POVMS_Object *resultobj, std::shared_ptr<Console>& console)
{
    POVMS_Message msg(obj, kPOVMsgClass_BackendControl, kPOVMsgIdent_InitInfo);
    POVMS_Message result(kPOVObjectClass_ResultData);

    msg.SetDestinationAddress(backendaddress);

    POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

    if(result.GetIdentifier() != kPOVMsgIdent_Done)
        throw POV_EXCEPTION_CODE(result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr));

    backendaddresses.insert(backendaddress);

    if(resultobj != nullptr)
        *resultobj = result;

    if(console != nullptr)
        Message2Console::InitInfo(result, console.get());
}

void RenderFrontendBase::DisconnectFromBackend(POVMSAddress backendaddress)
{
    POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_BackendControl, kPOVMsgIdent_Done);
    POVMS_Message result(kPOVObjectClass_ResultData);

    // Do not check here, the backend will check if the request to disconnect is valid! [trf]
    backendaddresses.erase(backendaddress);

    msg.SetDestinationAddress(backendaddress);

    POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

    if(result.GetIdentifier() != kPOVMsgIdent_Done)
        throw POV_EXCEPTION_CODE(result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr));
}

RenderFrontendBase::SceneId RenderFrontendBase::CreateScene(SceneData& shd, POVMSAddress backendaddress, POVMS_Object& obj)
{
    if(backendaddresses.find(backendaddress) == backendaddresses.end())
        throw POV_EXCEPTION_STRING("TODO"); // TODO FIXME

    POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_BackendControl, kPOVMsgIdent_CreateScene);
    POVMS_Message result(kPOVObjectClass_ResultData);

    msg.SetDestinationAddress(backendaddress);

    POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

    if(result.GetIdentifier() == kPOVMsgIdent_Done)
    {
        shd.scenepath = Path(obj.TryGetUCS2String(kPOVAttrib_InputFile, ""));
        shd.scenepath.SetFile("");

        shd.outputpath = Path(obj.TryGetUCS2String(kPOVAttrib_OutputPath, ""));
        // TODO FIXME BEGIN - this should not be needed, determine reason and fix [trf]
        if (shd.outputpath.Empty() == true)
        {
            shd.outputpath = Path(obj.TryGetUCS2String(kPOVAttrib_OutputFile, ""));
            shd.outputpath.SetFile("");
            if (shd.outputpath.Empty() == false)
                obj.SetUCS2String(kPOVAttrib_OutputPath, shd.outputpath().c_str());
        }
        // TODO FIXME END

        shd.verbose = obj.TryGetBool(kPOVAttrib_Verbose, true);

        for(size_t i = 0; i < MAX_STREAMS; i++)
        {
            shd.consoleoutput[i] = true;
            shd.streams[i].reset();
        }

        shd.consoleoutput[ALL_STREAM] = false;

        if(obj.Exist(kPOVAttrib_AllConsole))
        {
            bool b = obj.GetBool(kPOVAttrib_AllConsole);
            // NEVERE disable copyright banner BANNER_STREAM stream! [trf]
            shd.consoleoutput[DEBUG_STREAM] = b;
            shd.consoleoutput[FATAL_STREAM] = b;
            shd.consoleoutput[RENDER_STREAM] = b;
            shd.consoleoutput[STATISTIC_STREAM] = b;
            shd.consoleoutput[WARNING_STREAM] = b;
        }
        else
        {
            if(obj.Exist(kPOVAttrib_DebugConsole))
                shd.consoleoutput[DEBUG_STREAM] = obj.GetBool(kPOVAttrib_DebugConsole);
            if(obj.Exist(kPOVAttrib_FatalConsole))
                shd.consoleoutput[FATAL_STREAM] = obj.GetBool(kPOVAttrib_FatalConsole);
            if(obj.Exist(kPOVAttrib_RenderConsole))
                shd.consoleoutput[RENDER_STREAM] = obj.GetBool(kPOVAttrib_RenderConsole);
            if(obj.Exist(kPOVAttrib_StatisticsConsole))
                shd.consoleoutput[STATISTIC_STREAM] = obj.GetBool(kPOVAttrib_StatisticsConsole);
            if(obj.Exist(kPOVAttrib_WarningConsole))
                shd.consoleoutput[WARNING_STREAM] = obj.GetBool(kPOVAttrib_WarningConsole);
        }

        for(size_t i = 0; i < gStreamTypeUtilDataCount; i++)
        {
            if(obj.Exist(gStreamTypeUtilData[i]) == true)
            {
                shd.streamnames[gStreamNumber[i]] = obj.GetUCS2String(gStreamTypeUtilData[i]);
                if(ProcessOptions::IsTrue(UCS2toSysString(shd.streamnames[gStreamNumber[i]]).c_str()) == true)
                    shd.streamnames[gStreamNumber[i]] = SysToUCS2String(gStreamDefaultFile[i]);
            }
        }

        // append to the stream if this is a continued trace or animation.
        bool append = obj.TryGetBool(kPOVAttrib_ContinueTrace, false) || obj.TryGetBool(kPOVAttrib_AppendConsoleFiles, false);

        for(size_t i = 0; i < MAX_STREAMS; i++)
        {
            if(shd.streamnames[i].empty() == false)
            {
                Path tmp(shd.streamnames[i]);
#if 1
                // allow only pure file names
                if (tmp.HasVolume() || !tmp.GetFolder().empty())
                    throw POV_EXCEPTION(kParamErr, "Stream output files must reside in same directory as image output file.");
                Path path(shd.outputpath, tmp);
#elif 0
                // allow arbitrary relative file names, but no absolute file names
                if (tmp.HasVolume())
                    throw POV_EXCEPTION(kParamErr, "Stream output file names must be specified relative to the output file directory.");
                Path path(shd.outputpath, tmp);
#else
                // allow arbitrary file names
                Path path;
                if (tmp.HasVolume())
                    // file name is absolute
                    path = tmp;
                else
                    // file name is relative
                    path = Path(shd.outputpath, tmp);
#endif
                shd.streams[i].reset(new FileTextStreamBuffer(path().c_str(), append));
                if (append && i != DEBUG_STREAM)
                {
                    shd.streams[i]->puts("\n"
                                         "==============================================================================\n"
                                         "=                       Appending to stream output file                      =\n"
                                         "==============================================================================\n");
                }
            }
        }

        if(obj.Exist(kPOVAttrib_LibraryPath) == true)
        {
            POVMS_List lps;

            obj.Get(kPOVAttrib_LibraryPath, lps);
            for(int i = 1; i <= lps.GetListSize(); i++)
            {
                POVMS_Attribute lp;

                lps.GetNth(i, lp);
                UCS2String str = lp.GetUCS2String();
                if (str.empty() == true)
                    continue;
                if (!POV_IS_PATH_SEPARATOR(*str.rbegin()))
                    str += POV_PATH_SEPARATOR;
                shd.searchpaths.push_back(Path(str));
            }
        }

        return SceneId(backendaddress, result.GetInt(kPOVAttrib_SceneId));
    }
    else
        throw POV_EXCEPTION_CODE(result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr));
}

void RenderFrontendBase::CloseScene(SceneData& shd, SceneId sid)
{
    if(sid == Id())
        return;

    try
    {
        POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_BackendControl, kPOVMsgIdent_CloseScene);
        POVMS_Message result(kPOVObjectClass_ResultData);

        msg.SetDestinationAddress(sid.GetAddress());
        msg.SetInt(kPOVAttrib_SceneId, sid.GetIdentifier());

        POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

        if(result.GetIdentifier() != kPOVMsgIdent_Done)
            throw POV_EXCEPTION_CODE(result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr));
    }
    catch(pov_base::Exception&)
    {
        shd.state = SceneData::Scene_Invalid; // Cannot recover from error! - Current state of backend scene invalid! [trf]
        throw;
    }
}

void RenderFrontendBase::StartParser(SceneData& shd, SceneId sid, POVMS_Object& obj)
{
    if(shd.state != SceneData::Scene_Created)
        throw POV_EXCEPTION_CODE(kNotNowErr);

    POVMS_Message msg(obj, kPOVMsgClass_SceneControl, kPOVMsgIdent_StartParser);

    msg.SetDestinationAddress(sid.GetAddress());
    msg.SetInt(kPOVAttrib_SceneId, sid.GetIdentifier());

    POVMS_SendMessage(context, msg, nullptr, kPOVMSSendMode_NoReply);

    shd.state = SceneData::Scene_Parsing;
}

void RenderFrontendBase::PauseParser(SceneData& shd, SceneId sid)
{
    if(shd.state != SceneData::Scene_Parsing)
        throw POV_EXCEPTION_CODE(kNotNowErr);

    POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_SceneControl, kPOVMsgIdent_PauseParser);
    POVMS_Message result(kPOVObjectClass_ResultData);

    msg.SetDestinationAddress(sid.GetAddress());
    msg.SetInt(kPOVAttrib_SceneId, sid.GetIdentifier());

    POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

    if(result.GetIdentifier() != kPOVMsgIdent_Done)
    {
        int err = result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr);

        if((err != kNoErr) && (err != kNotNowErr))
            throw POV_EXCEPTION_CODE(err);
    }

    shd.state = SceneData::Scene_Paused;
}

void RenderFrontendBase::ResumeParser(SceneData& shd, SceneId sid)
{
    if(shd.state != SceneData::Scene_Paused)
        throw POV_EXCEPTION_CODE(kNotNowErr);

    POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_SceneControl, kPOVMsgIdent_ResumeParser);
    POVMS_Message result(kPOVObjectClass_ResultData);

    msg.SetDestinationAddress(sid.GetAddress());
    msg.SetInt(kPOVAttrib_SceneId, sid.GetIdentifier());

    POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

    if(result.GetIdentifier() != kPOVMsgIdent_Done)
    {
        int err = result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr);

        if((err != kNoErr) && (err != kNotNowErr))
            throw POV_EXCEPTION_CODE(err);
    }

    shd.state = SceneData::Scene_Parsing;
}

void RenderFrontendBase::StopParser(SceneData& shd, SceneId sid)
{
    if((shd.state != SceneData::Scene_Parsing) && (shd.state != SceneData::Scene_Paused))
        throw POV_EXCEPTION_CODE(kNotNowErr);

    POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_SceneControl, kPOVMsgIdent_StopParser);
    POVMS_Message result(kPOVObjectClass_ResultData);

    msg.SetDestinationAddress(sid.GetAddress());
    msg.SetInt(kPOVAttrib_SceneId, sid.GetIdentifier());

    shd.state = SceneData::Scene_Stopping;

    POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

    if(result.GetIdentifier() != kPOVMsgIdent_Done)
    {
        int err = result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr);

        if((err != kNoErr) && (err != kNotNowErr))
            throw POV_EXCEPTION_CODE(err);
    }
}

RenderFrontendBase::ViewId RenderFrontendBase::CreateView(SceneData& shd, ViewData& vhd, SceneId sid, POVMS_Object& obj)
{
    POVMS_Message msg(obj, kPOVMsgClass_SceneControl, kPOVMsgIdent_CreateView);
    POVMS_Message result(kPOVObjectClass_ResultData);

    msg.SetDestinationAddress(sid.GetAddress());
    msg.SetInt(kPOVAttrib_SceneId, sid.GetIdentifier());

    POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

    if(result.GetIdentifier() == kPOVMsgIdent_Done)
        return ViewId(sid.GetAddress(), result.GetInt(kPOVAttrib_ViewId));
    else
        throw POV_EXCEPTION_CODE(result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr));
}

void RenderFrontendBase::CloseView(ViewData& vhd, ViewId vid)
{
    if(vid == Id())
        return;

    try
    {
        POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_SceneControl, kPOVMsgIdent_CloseView);
        POVMS_Message result(kPOVObjectClass_ResultData);

        msg.SetDestinationAddress(vid.GetAddress());
        msg.SetInt(kPOVAttrib_ViewId, vid.GetIdentifier());

        POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

        if(result.GetIdentifier() != kPOVMsgIdent_Done)
            throw POV_EXCEPTION_CODE(result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr));
    }
    catch(pov_base::Exception&)
    {
        vhd.state = ViewData::View_Invalid; // Cannot recover from error! - Current state of backend view invalid! [trf]
        throw;
    }
}

void RenderFrontendBase::StartRender(ViewData& vhd, ViewId vid, POVMS_Object& obj)
{
    if((vhd.state != ViewData::View_Created) && (vhd.state != ViewData::View_Rendered))
        throw POV_EXCEPTION_CODE(kNotNowErr);

    POVMS_Message msg(obj, kPOVMsgClass_ViewControl, kPOVMsgIdent_StartRender);

    msg.SetDestinationAddress(vid.GetAddress());
    msg.SetInt(kPOVAttrib_ViewId, vid.GetIdentifier());

    POVMS_SendMessage(context, msg, nullptr, kPOVMSSendMode_NoReply);

    vhd.state = ViewData::View_Rendering;
}

void RenderFrontendBase::PauseRender(ViewData& vhd, ViewId vid)
{
    if(vhd.state != ViewData::View_Rendering)
        throw POV_EXCEPTION_CODE(kNotNowErr);

    POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_ViewControl, kPOVMsgIdent_PauseRender);
    POVMS_Message result(kPOVObjectClass_ResultData);

    msg.SetDestinationAddress(vid.GetAddress());
    msg.SetInt(kPOVAttrib_ViewId, vid.GetIdentifier());

    POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

    if(result.GetIdentifier() != kPOVMsgIdent_Done)
    {
        int err = result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr);

        if((err != kNoErr) && (err != kNotNowErr))
            throw POV_EXCEPTION_CODE(err);
    }

    vhd.state = ViewData::View_Paused;
}

void RenderFrontendBase::ResumeRender(ViewData& vhd, ViewId vid)
{
    if(vhd.state != ViewData::View_Paused)
        throw POV_EXCEPTION_CODE(kNotNowErr);

    POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_ViewControl, kPOVMsgIdent_ResumeRender);
    POVMS_Message result(kPOVObjectClass_ResultData);

    msg.SetDestinationAddress(vid.GetAddress());
    msg.SetInt(kPOVAttrib_ViewId, vid.GetIdentifier());

    POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

    if(result.GetIdentifier() != kPOVMsgIdent_Done)
    {
        int err = result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr);

        if((err != kNoErr) && (err != kNotNowErr))
            throw POV_EXCEPTION_CODE(err);
    }

    vhd.state = ViewData::View_Rendering;
}

void RenderFrontendBase::StopRender(ViewData& vhd, ViewId vid)
{
    if((vhd.state != ViewData::View_Rendering) && (vhd.state != ViewData::View_Paused))
        throw POV_EXCEPTION_CODE(kNotNowErr);

    POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_ViewControl, kPOVMsgIdent_StopRender);
    POVMS_Message result(kPOVObjectClass_ResultData);

    msg.SetDestinationAddress(vid.GetAddress());
    msg.SetInt(kPOVAttrib_ViewId, vid.GetIdentifier());

    vhd.state = ViewData::View_Stopping;

    POVMS_SendMessage(context, msg, &result, kPOVMSSendMode_WaitReply);

    if(result.GetIdentifier() != kPOVMsgIdent_Done)
    {
        int err = result.TryGetInt(kPOVAttrib_ErrorNumber, kNoErr);

        if((err != kNoErr) && (err != kNotNowErr))
            throw POV_EXCEPTION_CODE(err);
    }
}

void RenderFrontendBase::HandleMessage(POVMS_Message& msg, POVMS_Message& result, int)
{
    POVMSType ident = msg.GetIdentifier();

    switch(msg.GetClass())
    {
        case kPOVMsgClass_BackendControl:
            if(msg.Exist(kPOVAttrib_ErrorNumber) == true)
                OutputFatalError(msg.TryGetString(kPOVAttrib_EnglishText, "Unknown failure in backend!"), msg.GetInt(kPOVAttrib_ErrorNumber));
            else
                OutputFatalError(msg.TryGetString(kPOVAttrib_EnglishText, "Unknown failure in backend!"), 0);
            break;
        case kPOVMsgClass_SceneOutput:
            HandleParserMessage(SceneId(msg.GetSourceAddress(), msg.GetInt(kPOVAttrib_SceneId)), ident, msg);
            break;
        case kPOVMsgClass_ViewOutput:
            HandleRenderMessage(ViewId(msg.GetSourceAddress(), msg.GetInt(kPOVAttrib_ViewId)), ident, msg);
            break;
        case kPOVMsgClass_ViewImage:
            HandleImageMessage(ViewId(msg.GetSourceAddress(), msg.GetInt(kPOVAttrib_ViewId)), ident, msg);
            break;
        case kPOVMsgClass_FileAccess:
            HandleFileMessage(ViewId(msg.GetSourceAddress(), msg.GetInt(kPOVAttrib_SceneId)), ident, msg, result);
            break;
    }
}

void RenderFrontendBase::MakeBackupPath(POVMS_Object& ropts, ViewData& vd, const Path& outputpath)
{
    vd.imageBackupFile = outputpath;
    vd.imageBackupFile.SetFile((Path(ropts.TryGetUCS2String(kPOVAttrib_OutputFile, ""))).GetFile());
    if(vd.imageBackupFile.GetFile().empty() == true)
        vd.imageBackupFile.SetFile(ropts.TryGetUCS2String(kPOVAttrib_InputFile, "object.pov"));

    vd.imageBackupFile.SetFile(GetFileName(Path(vd.imageBackupFile.GetFile())) + u".pov-state");
}

void RenderFrontendBase::NewBackup(POVMS_Object& ropts, ViewData& vd, const Path& outputpath)
{
    vd.imageBackup.reset();

    MakeBackupPath(ropts, vd, outputpath);
    if (!pov_base::PlatformBase::GetInstance().AllowLocalFileAccess (vd.imageBackupFile(), POV_File_Data_Backup, true))
        throw POV_EXCEPTION(kCannotOpenFileErr, "Permission denied to create render state output file.");
    vd.imageBackup = std::shared_ptr<OStream>(new OStream(vd.imageBackupFile().c_str()));
    if(vd.imageBackup != nullptr)
    {
        Backup_File_Header hdr;

        if(!*vd.imageBackup)
            throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot create render state output file.");
        memcpy(hdr.sig, RENDER_STATE_SIG, sizeof(hdr.sig));
        memcpy(hdr.ver, RENDER_STATE_VER, sizeof(hdr.ver));
        if(vd.imageBackup->write(&hdr, sizeof(hdr)) == false)
            throw POV_EXCEPTION(kFileDataErr, "Cannot write header to render state output file.");
        vd.imageBackup->flush();

        // we remove the file now since if the render doesn't complete and the file
        // already exists, an attempt to do a continue later on will skip the render.
        UCS2String filename = ropts.TryGetUCS2String(kPOVAttrib_OutputFile, "");
        if(filename.length() > 0)
        {
            // we do this test even if the file doesn't exist as we need to write there
            // eventually anyhow. might as well test if before the render starts ...
            if(CheckIfFileExists(filename.c_str()))
                pov_base::Filesystem::DeleteFile(filename);
        }
    }
    else
        throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot create render state output file.");
}

void RenderFrontendBase::ContinueBackup(POVMS_Object& ropts, ViewData& vd, ViewId vid, POVMSInt& serial, std::vector<POVMSInt>& skip, const Path& outputpath)
{
    bool outputToFile = ropts.TryGetBool(kPOVAttrib_OutputToFile, true);

    // Note: due to the fact that tellg() only returns a 32-bit int on some platforms,
    // currently this code will only work properly with a state file that is < 4gb in
    // size, which works out to a render of roughly 16k*16k pixels.

    serial = 0;
    vd.imageBackup.reset();
    MakeBackupPath(ropts, vd, outputpath);

    std::unique_ptr<IStream> inbuffer(new IFileStream(vd.imageBackupFile().c_str()));

    size_t pos = sizeof(Backup_File_Header);

    if (inbuffer != nullptr)
    {
        Backup_File_Header hdr;

        if(*inbuffer)
        {
            // IOBase::eof() only is based on feof() and will only return
            // true if we have attempted to read past the end of the file.
            // therefore msg.Read() will throw an exception when we try to
            // read from the end of the file, which isn't harmful per se but
            // makes debugging more difficult since it is caught by the VC++
            // IDE, and we don't want to disable catching pov_base::Exception
            // since they are generally useful. therefore we explicitly check
            // for the end of the file.
            inbuffer->seekg (0, IOBase::seek_end);
            POV_OFF_T end = inbuffer->tellg();
            inbuffer->seekg (0, IOBase::seek_set);

            if (inbuffer->read (&hdr, sizeof (hdr)) == false)
                throw POV_EXCEPTION(kFileDataErr, "Cannot read header from render state file.");
            if (memcmp (hdr.sig, RENDER_STATE_SIG, sizeof (hdr.sig)) != 0)
                throw POV_EXCEPTION(kFileDataErr, "Render state file header appears to be invalid.");
            if (memcmp (hdr.ver, RENDER_STATE_VER, sizeof (hdr.ver)) != 0)
                throw POV_EXCEPTION(kFileDataErr, "Render state file was written by another version of POV-Ray.");

            while(pos < end && inbuffer->eof() == false)
            {
                POVMS_Message msg;

                try
                {
                    msg.Read(*(inbuffer.get()));

                    // do not render complete blocks again
                    if(msg.Exist(kPOVAttrib_PixelId) == true)
                    {
                        POVMSInt pid = msg.GetInt(kPOVAttrib_PixelId);

                        if(pid > serial)
                            skip.push_back(pid);
                        else
                            serial++;
                    }

                    HandleImageMessage(vid, msg.GetIdentifier(), msg);
                }
                catch(pov_base::Exception&)
                {
                    // ignore all problems, just assume file is broken from last message on
                    break;
                }
                pos = inbuffer->tellg();
            }
        }
        else
        {
            // file doesn't exist, we create it via NewBackup
            if (outputToFile == true)
                NewBackup(ropts, vd, outputpath);
            return;
        }
    }
    else
        throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot open state file from previous render.");

    // make sure the input file is closed since we're about to write to it
    inbuffer.reset();

    // if there isn't going to be an output file, we don't write to the state file
    if(outputToFile == true)
    {
        vd.imageBackup = std::shared_ptr<OStream>(new OStream(vd.imageBackupFile().c_str(), IOBase::append));
        if(vd.imageBackup != nullptr)
        {
            if(!*vd.imageBackup)
                throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot append to state output file.");

            vd.imageBackup->seekg(0, IOBase::seek_end);
            vd.imageBackup->seekg(min((POV_OFF_T)pos, vd.imageBackup->tellg()), IOBase::seek_set);
        }
        else
            throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot create state output file stream.");
    }
}

namespace Message2Console
{

void InitInfo(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POVMSObject msgobj(cppmsg());
    POVMSObjectPtr msg = &msgobj;
    const int NUMBER_OF_AUTHORS_ACROSS = 4;
    POVMSAttributeList attrlist;
    POVMSAttribute item;
    char charbuf[1024];
    int h, i, j;
    int cnt;
    int l;
    std::string generation;

    l = 1024;
    charbuf[0] = 0;
    if(POVMSUtil_GetString(msg, kPOVAttrib_CoreVersion, charbuf, &l) == kNoErr)
        tsb->printf("%s\n", charbuf);

    l = 1024;
    charbuf[0] = 0;
    if (POVMSUtil_GetString(msg, kPOVAttrib_CoreVersion, charbuf, &l) == kNoErr)
        generation = charbuf;

    l = 1024;
    charbuf[0] = 0;
    if(POVMSUtil_GetString(msg, kPOVAttrib_EnglishText, charbuf, &l) == kNoErr)
        tsb->printf("%s\n", charbuf);

    tsb->printf("\n");

    tsb->printf("Primary %s Architects/Developers: (Alphabetically)\n", generation.c_str());
    if(POVMSObject_Get(msg, &attrlist, kPOVAttrib_PrimaryDevs) == kNoErr)
    {
        cnt = 0;

        if(POVMSAttrList_Count(&attrlist, &cnt) == kNoErr)
        {
            for(i = 0, h = 1; h <= cnt; i++)
            {
                for(j = 0; (j < NUMBER_OF_AUTHORS_ACROSS) && (h <= cnt); j++, h++)
                {
                    if(POVMSAttrList_GetNth(&attrlist, h, &item) == kNoErr)
                    {
                        l = 1023;
                        charbuf[0] = 0;
                        if(POVMSAttr_Get(&item, kPOVMSType_CString, charbuf, &l) == kNoErr)
                            tsb->printf("  %-18s", charbuf);

                        (void)POVMSAttr_Delete(&item);
                    }
                }
                tsb->printf("\n");
            }
        }

        (void)POVMSAttrList_Delete(&attrlist);
    }

    tsb->printf("\n");
    tsb->printf("With Assistance From: (Alphabetically)\n");
    if(POVMSObject_Get(msg, &attrlist, kPOVAttrib_AssistingDevs) == kNoErr)
    {
        cnt = 0;

        if(POVMSAttrList_Count(&attrlist, &cnt) == kNoErr)
        {
            for(i = 0, h = 1; h <= cnt; i++)
            {
                for(j = 0; (j < NUMBER_OF_AUTHORS_ACROSS) && (h <= cnt); j++, h++)
                {
                    if(POVMSAttrList_GetNth(&attrlist, h, &item) == kNoErr)
                    {
                        l = 1023;
                        charbuf[0] = 0;
                        if(POVMSAttr_Get(&item, kPOVMSType_CString, charbuf, &l) == kNoErr)
                            tsb->printf("  %-18s", charbuf);

                        (void)POVMSAttr_Delete(&item);
                    }
                }
                tsb->printf("\n");
            }
        }

        (void)POVMSAttrList_Delete(&attrlist);
    }

    tsb->printf("\n");
    tsb->printf("Past Contributors: (Alphabetically)\n");
    if(POVMSObject_Get(msg, &attrlist, kPOVAttrib_ContributingDevs) == kNoErr)
    {
        cnt = 0;

        if(POVMSAttrList_Count(&attrlist, &cnt) == kNoErr)
        {
            for(i = 0, h = 1; h <= cnt; i++)
            {
                for(j = 0; (j < NUMBER_OF_AUTHORS_ACROSS) && (h <= cnt); j++, h++)
                {
                    if(POVMSAttrList_GetNth(&attrlist, h, &item) == kNoErr)
                    {
                        l = 1023;
                        charbuf[0] = 0;
                        if(POVMSAttr_Get(&item, kPOVMSType_CString, charbuf, &l) == kNoErr)
                            tsb->printf("  %-18s", charbuf);

                        (void)POVMSAttr_Delete(&item);
                    }
                }
                tsb->printf("\n");
            }
        }

        (void)POVMSAttrList_Delete(&attrlist);
    }

    tsb->printf("\n");
    tsb->printf("Other contributors are listed in the documentation.\n");

    if(POVMSObject_Get(msg, &attrlist, kPOVAttrib_ImageLibVersions) == kNoErr)
    {
        cnt = 0;

        if(POVMSAttrList_Count(&attrlist, &cnt) == kNoErr)
        {
            if(cnt > 0)
            {
                tsb->printf("\n");
                tsb->printf("Support libraries used by POV-Ray:\n");

                for(i = 1; i <= cnt; i++)
                {
                    if(POVMSAttrList_GetNth(&attrlist, i, &item) == kNoErr)
                    {
                        l = 1023;
                        charbuf[0] = 0;
                        if(POVMSAttr_Get(&item, kPOVMSType_CString, charbuf, &l) == kNoErr)
                            tsb->printf("  %s\n", charbuf);

                        (void)POVMSAttr_Delete(&item);
                    }
                }
            }
        }

        (void)POVMSAttrList_Delete(&attrlist);
    }

    l = 1024;
    charbuf[0] = 0;
    std::string cpuInfo;
    if (POVMSUtil_GetString(msg, kPOVAttrib_CPUInfo, charbuf, &l) == kNoErr)
        cpuInfo = charbuf;
#if POV_CPUINFO_DEBUG
    l = 1024;
    std::string cpuDetails;
    if (POVMSUtil_GetString(msg, kPOVAttrib_CPUInfoDetails, charbuf, &l) == kNoErr)
        cpuDetails = charbuf;
#endif

    if (POVMSObject_Get(msg, &attrlist, kPOVAttrib_Optimizations) == kNoErr)
    {
        cnt = 0;

        if (POVMSAttrList_Count(&attrlist, &cnt) == kNoErr)
        {
            if (cnt > 0)
            {
                tsb->printf("\n");
                tsb->printf("Dynamic optimizations:\n");

                if (!cpuInfo.empty())
                    tsb->printf("  CPU detected: %s\n", cpuInfo.c_str());
#if POV_CPUINFO_DEBUG
                if (!cpuDetails.empty())
                    tsb->printf("  CPU details: %s\n", cpuDetails.c_str());
#endif

                for (i = 1; i <= cnt; i++)
                {
                    if (POVMSAttrList_GetNth(&attrlist, i, &item) == kNoErr)
                    {
                        l = 1023;
                        charbuf[0] = 0;
                        if (POVMSAttr_Get(&item, kPOVMSType_CString, charbuf, &l) == kNoErr)
                            tsb->printf("  %s\n", charbuf);

                        (void)POVMSAttr_Delete(&item);
                    }
                }
            }
        }

        (void)POVMSAttrList_Delete(&attrlist);
    }

    POVMSObject_Delete(msg);
}

void ParserOptions(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POVMSObject msgobj(cppmsg());
    POVMSObjectPtr msg = &msgobj;
    POVMSAttribute attr;
    POVMSFloat f;
    UCS2 ucs2buf[1024];
    int l;

    tsb->printf("Parser Options\n");

    f = 0.0;
    l = sizeof (ucs2buf);
    ucs2buf[0] = 0;
    (void)POVMSUtil_GetUCS2String(msg, kPOVAttrib_InputFile, ucs2buf, &l);
    if(POVMSUtil_GetFloat(msg, kPOVAttrib_Version, &f) != kNoErr)
        tsb->printf("  Input file: %s\n", UCS2toSysString(ucs2buf).c_str());
    else
        tsb->printf("  Input file: %s (compatible to version %1.2f)\n", UCS2toSysString(ucs2buf).c_str(), (double)f);
    tsb->printf("  Remove bounds.......%s\n  Split unions........%s\n",
                  GetOptionSwitchString(msg, kPOVAttrib_RemoveBounds, true),
                  GetOptionSwitchString(msg, kPOVAttrib_SplitUnions, false));

    tsb->printf("  Library paths:\n");
    if(POVMSObject_Get(msg, &attr, kPOVAttrib_LibraryPath) == kNoErr)
    {
        int cnt = 0;

        if(POVMSAttrList_Count(&attr, &cnt) == kNoErr)
        {
            POVMSAttribute item;
            int ii;

            for(ii = 1; ii <= cnt; ii++)
            {
                if(POVMSAttrList_GetNth(&attr, ii, &item) == kNoErr)
                {
                    l = sizeof(ucs2buf);
                    ucs2buf[0] = 0;
                    (void)POVMSAttr_Get(&item, kPOVMSType_UCS2String, ucs2buf, &l);
                    tsb->printf("    %s\n", UCS2toSysString(ucs2buf).c_str());

                    (void)POVMSAttr_Delete(&item);
                }
            }
        }

        (void)POVMSAttr_Delete(&attr);
    }

    POVMSObject_Delete(msg);
    AnimationOptions(cppmsg,tsb);
}

void RenderOptions(POVMS_Object& obj, TextStreamBuffer *tsb)
{
    tsb->printf("----------------------------------------------------------------------------\n");
    tsb->printf("Render Options\n");

    tsb->printf("  Quality: %2d\n", clip(obj.TryGetInt(kPOVAttrib_Quality, 9), 0, 9));

    if(obj.TryGetBool (kPOVAttrib_Bounding, true))
        tsb->printf("  Bounding boxes.......On   Bounding threshold: %d\n",
                    clip<int>(obj.TryGetInt(kPOVAttrib_BoundingThreshold,DEFAULT_AUTO_BOUNDINGTHRESHOLD),1,SIGNED16_MAX));
    else
        tsb->printf("  Bounding boxes.......Off\n");

    /*
    tsb->printf("  Light Buffer........%s\n", GetOptionSwitchString(msg, kPOVAttrib_LightBuffer, true));
    tsb->printf("  Vista Buffer........%-3s", GetOptionSwitchString(msg, kPOVAttrib_VistaBuffer, true));
    b = false;
    (void)POVMSUtil_GetBool(msg, kPOVAttrib_VistaBuffer, &b);
    if(b == true)
        tsb->printf("  Draw Vista Buffer...%s", GetOptionSwitchString(msg, kPOVAttrib_DrawVistas, false));
    tsb->printf("\n");
*/

    if(obj.TryGetBool(kPOVAttrib_Antialias, false) == true)
    {
        int method = 0;
        if(obj.TryGetBool(kPOVAttrib_Antialias, false) == true)
            method = clip(obj.TryGetInt(kPOVAttrib_SamplingMethod, 1), 0, 3); // TODO FIXME - magic number in clip
        int depth = clip(obj.TryGetInt(kPOVAttrib_AntialiasDepth, 3), 1, 9); // TODO FIXME - magic number in clip
        float threshold = clip(obj.TryGetFloat(kPOVAttrib_AntialiasThreshold, 0.3f), 0.0f, 1.0f);
        float aagamma = obj.TryGetFloat(kPOVAttrib_AntialiasGamma, 2.5f);
        float jitter = 0.0f;
        if(obj.TryGetBool(kPOVAttrib_Jitter, true))
            jitter = clip(obj.TryGetFloat(kPOVAttrib_JitterAmount, 1.0f), 0.0f, 1.0f);
        if(jitter > 0.0f)
            tsb->printf("  Antialiasing.........On  (Method %d, Threshold %.3f, Depth %d, Jitter %.2f, Gamma %.2f)\n",
                           method, threshold, depth, jitter, aagamma);
        else
            tsb->printf("  Antialiasing.........On  (Method %d, Threshold %.3f, Depth %d, Jitter Off, Gamma %.2f)\n",
                           method, threshold, depth, aagamma);
    }
    else
        tsb->printf("  Antialiasing.........Off\n");
}

void OutputOptions(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POVMSObject msgobj(cppmsg());
    POVMSObjectPtr msg = &msgobj;
    POVMSInt i, i2;
    POVMSFloat f, f2, f3, f4;
    int startRow, startCol, endRow, endCol;
    POVMSBool b;
    UCS2 ucs2buf[1024];
    const char *t;
    int outputQuality = 8; // default bits per pixel channel // TODO FIXME: Default values shouldn't be hard-coded in here!
    int outputCompression;
    int l;
    int outputFormat = kPOVList_FileType_PNG; // TODO FIXME: Default values shouldn't be hard-coded in here!
    bool outputGrayscale = false;

    tsb->printf("Image Output Options\n");

    if (POVMSUtil_GetInt(msg, kPOVAttrib_Width, &i) != kNoErr)
        i = 160;
    if (POVMSUtil_GetInt(msg, kPOVAttrib_Height, &i2) != kNoErr)
        i2 = 120;
    if (POVMSUtil_GetFloat(msg, kPOVAttrib_StartRow, &f) != kNoErr)
        f = 0.0;
    if (POVMSUtil_GetFloat(msg, kPOVAttrib_EndRow, &f2) != kNoErr)
        f2 = (POVMSFloat)i2;
    if (POVMSUtil_GetFloat(msg, kPOVAttrib_StartColumn, &f3) != kNoErr)
        f3 = 0.0;
    if (POVMSUtil_GetFloat(msg, kPOVAttrib_EndColumn, &f4) != kNoErr)
        f4 = (POVMSFloat)i;

    // TODO FIXME - interpretation of the render rectangle (integer vs float) should be implemented elsewhere;
    // currently there is redundancy with View::StartRender(...) in view.cpp

    if((f3 >= 0.0) && (f3 < 1.0))
        startCol = int(DBL(i) * f3);
    else
        startCol = int(f3) - 1;

    if((f >= 0.0) && (f < 1.0))
        startRow = int(DBL(i2) * f);
    else
        startRow = int(f - 1);

    if((f4 >= 0.0) && (f4 <= 1.0))
        endCol = int(DBL(i) * f4) - 1;
    else
        endCol = int(f4 - 1);

    if((f2 >= 0.0) && (f2 <= 1.0))
        endRow = int(DBL(i2) * f2) - 1;
    else
        endRow = int(f2 - 1);

    tsb->printf("  Image resolution.....%u by %u (rows %d to %d, columns %d to %d).\n",
                (int)i, (int)i2, (int)(startRow+1), (int)(endRow+1), (int)(startCol+1), (int)(endCol+1));

    if(POVMSUtil_GetInt(msg, kPOVAttrib_OutputFileType, &i) == kNoErr)
        outputFormat = i;
    if(POVMSUtil_GetInt(msg, kPOVAttrib_BitsPerColor, &i) == kNoErr)
        outputQuality = i;
    if(POVMSUtil_GetBool(msg, kPOVAttrib_GrayscaleOutput, &b) == kNoErr)
        outputGrayscale = b;

    b = false;
    if(POVMSUtil_GetBool(msg, kPOVAttrib_OutputToFile, &b) != kNoErr || b == true) // TODO FIXME: Defaults (in this case b=true) shouldn't be hard-coded in here!
    {
        const char *al = "";

        l = 1023;
        ucs2buf[0] = 0;
        (void)POVMSUtil_GetUCS2String(msg, kPOVAttrib_OutputFile, ucs2buf, &l);

        if(outputFormat == kPOVList_FileType_JPEG)
        {
            outputQuality = 85; // Default from base/image/jpeg.cpp // TODO FIXME: Default values shouldn't be hard-coded in here!
            if(POVMSUtil_GetInt(msg, kPOVAttrib_Compression, &i) == kNoErr)
            {
                outputQuality = i>1?i:85; // Catching default 0 & 1 from base/image/jpeg.cpp // TODO FIXME: Default values shouldn't be hard-coded in here!
                outputQuality = max(0, outputQuality);
                outputQuality = min(100, outputQuality);
            }
            outputCompression = outputQuality;
            outputQuality = 8;
        }

        b = false;
        (void)POVMSUtil_GetBool(msg, kPOVAttrib_OutputAlpha, &b);
        if(b == true)
        {
            outputQuality *= 4;
            al = " with alpha";
        }
        else
            outputQuality *= 3;

        switch(outputFormat)
        {
            // TODO FIXME - for easier maintenance, this should probably be part of the FileTypeTable.
            case kPOVList_FileType_Targa:           t = "Targa";           break;
            case kPOVList_FileType_CompressedTarga: t = "RLE Targa";       break;
            case kPOVList_FileType_PNG:             t = "PNG";             break;
            case kPOVList_FileType_JPEG:            t = "JPEG";            break;
            case kPOVList_FileType_PPM:             t = "PPM";             break;
            case kPOVList_FileType_BMP:             t = "BMP";             break;
            case kPOVList_FileType_OpenEXR:         t = "EXR";             break;
            case kPOVList_FileType_RadianceHDR:     t = "HDR";             break;
            case kPOVList_FileType_System:          t = "(system format)"; break;
            default:                                t = "(none)";          break;
        }

        if(outputFormat == kPOVList_FileType_JPEG)
            tsb->printf("  Output file..........%s, %d bpp, quality %d%s%s %s\n", UCS2toSysString(ucs2buf).c_str(), outputQuality, outputCompression, "%", al, t);
        else if (outputGrayscale)
            tsb->printf("  Output file..........%s, grayscale%s %s\n", UCS2toSysString(ucs2buf).c_str(), al, t);
        else
            tsb->printf("  Output file..........%s, %d bpp%s %s\n", UCS2toSysString(ucs2buf).c_str(), outputQuality, al, t);

        if ((outputFormat != kPOVList_FileType_JPEG) && (outputFormat != kPOVList_FileType_OpenEXR))
        {
            b = false;
            (void)POVMSUtil_GetBool(msg, kPOVAttrib_Dither, &b);
            if (b)
            {
                i = int(DitherMethodId::kBlueNoise);
                (void)POVMSUtil_GetInt(msg, kPOVAttrib_DitherMethod, &i);
                t = ProcessRenderOptions::GetDitherMethodText(i);
                tsb->printf("  Dithering............%s\n", t);
            }
            else
                tsb->printf("  Dithering............Off\n");
        }
    }
    else
        tsb->printf("  Output file: Disabled\n");

    b = false;
    (void)POVMSUtil_GetBool(msg, kPOVAttrib_Display, &b);
    if(b == true)
    {
        i = DEFAULT_DISPLAY_GAMMA_TYPE;
        (void)POVMSUtil_GetInt(msg, kPOVAttrib_DisplayGammaType, &i);
        f = DEFAULT_DISPLAY_GAMMA;
        (void)POVMSUtil_GetFloat(msg, kPOVAttrib_DisplayGamma, &f);
        switch (i)
        {
            case kPOVList_GammaType_Neutral:
                tsb->printf("  Graphic display......On  (gamma: 1.0)\n");
                break;
            case kPOVList_GammaType_PowerLaw:
                tsb->printf("  Graphic display......On  (gamma: %g)\n", (float)f);
                break;
            default:
                t = ProcessRenderOptions::GetGammaTypeText(i);
                tsb->printf("  Graphic display......On  (gamma: %s)\n", t);
                break;
        }
    }
    else
        tsb->printf("  Graphic display......Off\n");

    i = 0;
    (void)POVMSUtil_GetInt(msg, kPOVAttrib_PreviewStartSize, &i);
    if(i > 1)
    {
        i2 = 0;
        (void)POVMSUtil_GetInt(msg, kPOVAttrib_PreviewEndSize, &i2);
        tsb->printf("  Mosaic preview.......On  (pixel sizes %d to %d)\n", (int)i, (int)i2);
    }
    else
        tsb->printf("  Mosaic preview.......Off\n");

    tsb->printf("  Continued trace.....%s\n", GetOptionSwitchString(msg, kPOVAttrib_ContinueTrace, false));

    tsb->printf("Information Output Options\n");

    tsb->printf("  All Streams to console.........%s", GetOptionSwitchString(msg, kPOVAttrib_AllConsole, true));
//  if (streamnames[ALL_STREAM] != nullptr)
//      tsb->printf("  and file %s\n", streamnames[ALL_STREAM]);
//  else
        tsb->printf("\n");

    tsb->printf("  Debug Stream to console........%s", GetOptionSwitchString(msg, kPOVAttrib_DebugConsole, true));
//  if (streamnames[DEBUG_STREAM] != nullptr)
//      tsb->printf("  and file %s\n", streamnames[DEBUG_STREAM]);
//  else
        tsb->printf("\n");

    tsb->printf("  Fatal Stream to console........%s", GetOptionSwitchString(msg, kPOVAttrib_FatalConsole, true));
//  if (streamnames[FATAL_STREAM] != nullptr)
//      tsb->printf("  and file %s\n", streamnames[FATAL_STREAM]);
//  else
        tsb->printf("\n");

    tsb->printf("  Render Stream to console.......%s", GetOptionSwitchString(msg, kPOVAttrib_RenderConsole, true));
//  if (streamnames[RENDER_STREAM] != nullptr)
//      tsb->printf("  and file %s\n", streamnames[RENDER_STREAM]);
//  else
        tsb->printf("\n");

    tsb->printf("  Statistics Stream to console...%s", GetOptionSwitchString(msg, kPOVAttrib_StatisticsConsole, true));
//  if (streamnames[STATISTIC_STREAM] != nullptr)
//      tsb->printf("  and file %s\n", streamnames[STATISTIC_STREAM]);
//  else
        tsb->printf("\n");

    tsb->printf("  Warning Stream to console......%s", GetOptionSwitchString(msg, kPOVAttrib_WarningConsole, true));
//  if (streamnames[WARNING_STREAM] != nullptr)
//      tsb->printf("  and file %s\n", streamnames[WARNING_STREAM]);
//  else
        tsb->printf("\n");

    POVMSObject_Delete(msg);
}

void AnimationOptions(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POVMSObject msgobj(cppmsg());
    POVMSObjectPtr msg = &msgobj;
    POVMSInt i, i2;
    POVMSFloat f, f2;

    i = 1;
    i2 = 1;
    f = 0.0;
    (void)POVMSUtil_GetInt(msg, kPOVAttrib_InitialFrame, &i);
    (void)POVMSUtil_GetInt(msg, kPOVAttrib_FinalFrame, &i2);
    (void)POVMSUtil_GetFloat(msg, kPOVAttrib_Clock, &f);
    if((i != 1) || (i2 != 1) || (i != i2) || (f != 0.0))
    {
        tsb->printf("Animation Options\n");
        tsb->printf("  Initial Frame: %8d  Final Frame: %8d\n", (int)i, (int)i2);
        i2 = 1; // POVMSUtil_GetInt might fail, default to 1
        (void)POVMSUtil_GetInt(msg, kPOVAttrib_FrameStep, &i2);
        i2=max(1,i2);
        tsb->printf("  Frame Step: %11d\n",i2);
        f = 0.0;
        f2 = 0.0;
        (void)POVMSUtil_GetFloat(msg, kPOVAttrib_InitialClock, &f);
        (void)POVMSUtil_GetFloat(msg, kPOVAttrib_FinalClock, &f2);
        tsb->printf("  Initial Clock: %8.3f  Final Clock: %8.3f\n", (float)f, (float)f2);
        tsb->printf("  Cyclic Animation....%s  Field render........%s  Odd lines/frames....%s",
                      GetOptionSwitchString(msg, kPOVAttrib_CyclicAnimation, false),
                      GetOptionSwitchString(msg, kPOVAttrib_FieldRender, false),
                      GetOptionSwitchString(msg, kPOVAttrib_OddField, false));
    }
    else
        tsb->printf("  Clock value: %8.3f  (Animation off)", (float)f);
    tsb->printf("\n");

    POVMSObject_Delete(msg);
}

void ParserStatistics(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POVMSLong ll = 0;
    int l = 0;
    int s = 0;
    int i = 0;

    tsb->printf("----------------------------------------------------------------------------\n");
    tsb->printf("Parser Statistics\n");
    tsb->printf("----------------------------------------------------------------------------\n");

    s = cppmsg.TryGetInt(kPOVAttrib_FiniteObjects, 0);
    i = cppmsg.TryGetInt(kPOVAttrib_InfiniteObjects, 0);
    l = cppmsg.TryGetInt(kPOVAttrib_LightSources, 0);

    tsb->printf("Finite Objects:   %10d\n", s);
    tsb->printf("Infinite Objects: %10d\n", i);
    tsb->printf("Light Sources:    %10d\n", l);
    tsb->printf("Total:            %10d\n", s + i + l);

    if(cppmsg.Exist(kPOVAttrib_BSPNodes) == true)
    {
        tsb->printf("----------------------------------------------------------------------------\n");
        tsb->printf("BSP Split Nodes:  %10d\n", cppmsg.TryGetInt(kPOVAttrib_BSPSplitNodes, 0));
        tsb->printf("BSP Object Nodes: %10d\n", cppmsg.TryGetInt(kPOVAttrib_BSPObjectNodes, 0));
        tsb->printf("BSP Empty Nodes:  %10d\n", cppmsg.TryGetInt(kPOVAttrib_BSPEmptyNodes, 0));
        tsb->printf("BSP Total Nodes:  %10d\n", cppmsg.TryGetInt(kPOVAttrib_BSPNodes, 0));
        tsb->printf("----------------------------------------------------------------------------\n");
        tsb->printf("BSP Objects/Node Average:       %8.2f          Maximum:      %10d\n",
                    cppmsg.TryGetFloat(kPOVAttrib_BSPAverageObjects, 0.0f), cppmsg.TryGetInt(kPOVAttrib_BSPMaxObjects, 0));
        tsb->printf("BSP Tree Depth Average:         %8.2f          Maximum:      %10d\n",
                    cppmsg.TryGetFloat(kPOVAttrib_BSPAverageDepth, 0.0f), cppmsg.TryGetInt(kPOVAttrib_BSPMaxDepth, 0));
        tsb->printf("BSP Max Depth Stopped Nodes:  %10d (%3.1f%%)   Objects/Node:   %8.2f\n",
                    cppmsg.TryGetInt(kPOVAttrib_BSPAborts, 0), cppmsg.TryGetFloat(kPOVAttrib_BSPAverageAborts, 0.0f) * 100.0f,
                    cppmsg.TryGetFloat(kPOVAttrib_BSPAverageAbortObjects, 0.0f));
    }

    tsb->printf("----------------------------------------------------------------------------\n");
}

void RenderStatistics(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POVMSObject msgobj(cppmsg());
    POVMSObjectPtr msg = &msgobj;
    POVMSAttribute attr;
    POVMSLong l, l2, l3;
    POV_LONG Pixels_In_Image;
    int i, i2;

    tsb->printf("----------------------------------------------------------------------------\n");

    (void)POVMSUtil_GetInt(msg, kPOVAttrib_Width, &i);
    (void)POVMSUtil_GetInt(msg, kPOVAttrib_Height, &i2);
    Pixels_In_Image = (POV_LONG)i * (POV_LONG)i2;

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_Pixels, &l);
    if(Pixels_In_Image > POVMSLongToCDouble(l))
        tsb->printf("Render Statistics (Partial Image Rendered)\n");
    else
        tsb->printf("Render Statistics\n");

    tsb->printf("Image Resolution %d x %d\n", i, i2);

    tsb->printf("----------------------------------------------------------------------------\n");

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_Pixels, &l);
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_PixelSamples, &l2);
    if(POVMSLongToCDouble(l) > 0.5)
        tsb->printf("Pixels:  %15.0f   Samples: %15.0f   Smpls/Pxl: %.2f\n",
                    POVMSLongToCDouble(l), POVMSLongToCDouble(l2), POVMSLongToCDouble(l2) / POVMSLongToCDouble(l));
    else
        tsb->printf("Pixels:  %15.0f   Samples: %15.0f   Smpls/Pxl: -\n",
                    POVMSLongToCDouble(l), POVMSLongToCDouble(l2));

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_Rays, &l);
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_RaysSaved, &l2);
    (void)POVMSUtil_GetInt(msg, kPOVAttrib_TraceLevel, &i);
    (void)POVMSUtil_GetInt(msg, kPOVAttrib_MaxTraceLevel, &i2);
    tsb->printf("Rays:    %15.0f   Saved:   %15.0f   Max Level: %d/%d\n",
                POVMSLongToCDouble(l), POVMSLongToCDouble(l2), i, i2);

    tsb->printf("----------------------------------------------------------------------------\n");
    tsb->printf("Ray->Shape Intersection          Tests       Succeeded  Percentage\n");
    tsb->printf("----------------------------------------------------------------------------\n");

    if(POVMSObject_Get(msg, &attr, kPOVAttrib_ObjectIStats) == kNoErr)
    {
        int cnt = 0;

        if(POVMSAttrList_Count(&attr, &cnt) == kNoErr)
        {
            POVMSObject obj;
            int ii, len;
            char str[40];

            for(ii = 1; ii <= cnt; ii++)
            {
                if(POVMSAttrList_GetNth(&attr, ii, &obj) == kNoErr)
                {
                    len = 40;
                    str[0] = 0;
                    (void)POVMSUtil_GetString(&obj, kPOVAttrib_ObjectName, str, &len);
                    (void)POVMSUtil_GetLong(&obj, kPOVAttrib_ISectsTests, &l);
                    (void)POVMSUtil_GetLong(&obj, kPOVAttrib_ISectsSucceeded, &l2);

                    if(POVMSLongToCDouble(l) > 0.5)
                    {
                        tsb->printf("%-22s  %14.0f  %14.0f  %8.2f\n", str,
                                      POVMSLongToCDouble(l), POVMSLongToCDouble(l2),
                                      100.0 * POVMSLongToCDouble(l2) / POVMSLongToCDouble(l));
                    }

                    (void)POVMSAttr_Delete(&obj);
                }
            }
        }

        (void)POVMSAttr_Delete(&attr);
    }

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_IsoFindRoot, &l);
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_FunctionVMCalls, &l2);
    if((POVMSLongToCDouble(l) > 0.5) || (POVMSLongToCDouble(l2) > 0.5))
    {
        tsb->printf("----------------------------------------------------------------------------\n");
        if(POVMSLongToCDouble(l) > 0.5)
            tsb->printf("Isosurface roots:   %15.0f\n", POVMSLongToCDouble(l));
        if(POVMSLongToCDouble(l2) > 0.5)
            tsb->printf("Function VM calls:  %15.0f\n", POVMSLongToCDouble(l2));
    }

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_CrackleCacheTest, &l);
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_CrackleCacheTestSuc, &l2);
    if((POVMSLongToCDouble(l) > 0.5) || (POVMSLongToCDouble(l2) > 0.5))
    {
        tsb->printf("----------------------------------------------------------------------------\n");
            if(POVMSLongToCDouble(l) > 0.5)
                tsb->printf("Crackle Cache Queries: %15.0f\n", POVMSLongToCDouble(l));
            if(POVMSLongToCDouble(l2) > 0.5)
                tsb->printf("Crackle Cache Hits:    %15.0f (%3.0f percent)\n", POVMSLongToCDouble(l2),
                            100.0 * POVMSLongToCDouble(l2) / POVMSLongToCDouble(l));
    }

    tsb->printf("----------------------------------------------------------------------------\n");

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_PolynomTest, &l);
    if(POVMSLongToCDouble(l) > 0.5)
    {
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RootsEliminated, &l2);
        tsb->printf("Roots tested:       %15.0f   eliminated:      %15.0f\n",
                      POVMSLongToCDouble(l), POVMSLongToCDouble(l2));
    }

    // TODO FIXME
    //(void)POVMSUtil_GetLong(msg, kPOVAttrib_CallsToNoise, &l);
    //(void)POVMSUtil_GetLong(msg, kPOVAttrib_CallsToDNoise, &l2);
    //tsb->printf("Calls to Noise:     %15.0f   Calls to DNoise: %15.0f\n",
    //              POVMSLongToCDouble(l), POVMSLongToCDouble(l2));
    // tsb->printf("----------------------------------------------------------------------------\n");

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_MediaIntervals, &l);
    if(POVMSLongToCDouble(l) > 0.5)
    {
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_MediaSamples, &l2);
        tsb->printf("Media Intervals:    %15.0f   Media Samples:   %15.0f (%4.2f)\n",
                      POVMSLongToCDouble(l), POVMSLongToCDouble(l2), POVMSLongToCDouble(l2) / POVMSLongToCDouble(l));
    }

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_ShadowTest, &l);
    if(POVMSLongToCDouble(l) > 0.5)
    {
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_ShadowTestSuc, &l2);

        tsb->printf("Shadow Ray Tests:   %15.0f   Succeeded:       %15.0f\n",
                      POVMSLongToCDouble(l), POVMSLongToCDouble(l2));

        (void)POVMSUtil_GetLong(msg, kPOVAttrib_ShadowCacheHits, &l);
        if(POVMSLongToCDouble(l) > 0.5)
            tsb->printf("Shadow Cache Hits:  %15.0f\n", POVMSLongToCDouble(l));
    }

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_ReflectedRays, &l);
    if(POVMSLongToCDouble(l) > 0.5)
    {
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_InnerReflectedRays, &l2);
        if(POVMSLongToCDouble(l2) > 0)
            tsb->printf("Reflected Rays:     %15.0f   Total Internal:  %15.0f\n",
                          POVMSLongToCDouble(l), POVMSLongToCDouble(l2));
        else
            tsb->printf("Reflected Rays:     %15.0f\n", POVMSLongToCDouble(l));
    }

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_RefractedRays, &l);
    if(POVMSLongToCDouble(l) > 0.5)
        tsb->printf("Refracted Rays:     %15.0f\n", POVMSLongToCDouble(l));

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_TransmittedRays, &l);
    if(POVMSLongToCDouble(l) > 0.5)
        tsb->printf("Transmitted Rays:   %15.0f\n", POVMSLongToCDouble(l));
/*
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_BoundingQueues, &l);
    if(POVMSLongToCDouble(l) > 0.5)
    {
        tsb->printf("Bounding Queues:    %15.0f\n", POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_BoundingQueueResets, &l);
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_BoundingQueueResizes, &l2);
        tsb->printf("Queue Resets:       %15.0f   Queue Resizes:   %15.0f\n",
                      POVMSLongToCDouble(l), POVMSLongToCDouble(l2));
    }
*/
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadGatherCount, &l);
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadReuseCount, &l2);
    if((POVMSLongToCDouble(l) > 0.5) || (POVMSLongToCDouble(l2) > 0.5))
    {
        tsb->printf("----------------------------------------------------------------------------\n");
        tsb->printf("Radiosity samples calculated:  %15.0f (%.2f %%)\n", POVMSLongToCDouble(l), 100.0 * POVMSLongToCDouble(l) / (POVMSLongToCDouble(l) + POVMSLongToCDouble(l2)));

        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadUnsavedCount, &l3);
        if(POVMSLongToCDouble(l3) > 0.5)
        {
            l -= l3;
            tsb->printf("  discarded due to low quality:%15.0f\n", POVMSLongToCDouble(l3));
            tsb->printf("  retained for re-use:         %15.0f\n", POVMSLongToCDouble(l));
        }

        tsb->printf("Radiosity samples reused:      %15.0f\n", POVMSLongToCDouble(l2));

        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadRayCount, &l3);
        if(POVMSLongToCDouble(l3) > 0.5)
        {
            tsb->printf("Radiosity sample rays shot:    %15.0f\n", POVMSLongToCDouble(l3));
        }

        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadOctreeNodes, &l2);
        if(POVMSLongToCDouble(l2) > 0.5)
        {
            tsb->printf("Radiosity octree nodes:        %15.0f\n", POVMSLongToCDouble(l2));
            tsb->printf("Radiosity octree samples/node: %15.2f\n", POVMSLongToCDouble(l) / POVMSLongToCDouble(l2));
        }

        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadOctreeLookups, &l);
        if(POVMSLongToCDouble(l) > 0.5)
            tsb->printf("Radiosity blocks examined:     %15.0f\n", POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadOctreeAccepts0, &l2);
        if(POVMSLongToCDouble(l2) > 0.5)
            tsb->printf("Radiosity blocks passed test 0:%15.0f (%.2f %%)\n", POVMSLongToCDouble(l2), 100.0 * POVMSLongToCDouble(l2) / POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadOctreeAccepts1, &l2);
        if(POVMSLongToCDouble(l2) > 0.5)
            tsb->printf("Radiosity blocks passed test 1:%15.0f (%.2f %%)\n", POVMSLongToCDouble(l2), 100.0 * POVMSLongToCDouble(l2) / POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadOctreeAccepts2, &l2);
        if(POVMSLongToCDouble(l2) > 0.5)
            tsb->printf("Radiosity blocks passed test 2:%15.0f (%.2f %%)\n", POVMSLongToCDouble(l2), 100.0 * POVMSLongToCDouble(l2) / POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadOctreeAccepts3, &l2);
        if(POVMSLongToCDouble(l2) > 0.5)
            tsb->printf("Radiosity blocks passed test 3:%15.0f (%.2f %%)\n", POVMSLongToCDouble(l2), 100.0 * POVMSLongToCDouble(l2) / POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadOctreeAccepts4, &l2);
        if(POVMSLongToCDouble(l2) > 0.5)
            tsb->printf("Radiosity blocks passed test 4:%15.0f (%.2f %%)\n", POVMSLongToCDouble(l2), 100.0 * POVMSLongToCDouble(l2) / POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadOctreeAccepts5, &l2);
        if(POVMSLongToCDouble(l2) > 0.5)
            tsb->printf("Radiosity blocks passed test 5:%15.0f (%.2f %%)\n", POVMSLongToCDouble(l2), 100.0 * POVMSLongToCDouble(l2) / POVMSLongToCDouble(l));
        l3 = l - l2;
        if(POVMSLongToCDouble(l3) > 0.5)
            tsb->printf("Radiosity blocks rejected:     %15.0f (%.2f %%)\n", POVMSLongToCDouble(l3), 100.0 * POVMSLongToCDouble(l3) / POVMSLongToCDouble(l));

        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadTopLevelGatherCount, &l);
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadTopLevelReuseCount, &l2);
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadTopLevelRayCount, &l3);
        tsb->printf("----------------------------------------------------------------------------\n");
        tsb->printf("Radiosity Depth 0 calculated:  %15.0f (%.2f %%)\n", POVMSLongToCDouble(l), 100.0 * POVMSLongToCDouble(l) / (POVMSLongToCDouble(l) + POVMSLongToCDouble(l2)));
        tsb->printf("Radiosity Depth 0 reused:      %15.0f\n", POVMSLongToCDouble(l2));
        tsb->printf("Radiosity Depth 0 rays shot:   %15.0f\n", POVMSLongToCDouble(l3));

        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadFinalGatherCount, &l);
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadFinalReuseCount, &l2);
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_RadFinalRayCount, &l3);
        tsb->printf("----------------------------------------------------------------------------\n");
        tsb->printf("Radiosity (final) calculated:  %15.0f (%.2f %%)\n", POVMSLongToCDouble(l), 100.0 * POVMSLongToCDouble(l) / (POVMSLongToCDouble(l) + POVMSLongToCDouble(l2)));
        tsb->printf("Radiosity (final) reused:      %15.0f\n", POVMSLongToCDouble(l2));
        tsb->printf("Radiosity (final) rays shot:   %15.0f\n", POVMSLongToCDouble(l3));

        POVMSLong samples[5][5];
        POVMSLong sampleSumPerRecursion[5];
        POVMSLong sampleSumPerPass[5];
        POVMSLong samplesFinal[5];
        POVMSLong sampleSumFinal = 0;
        POVMSLong sampleSum = 0;
        POVMSFloat sampleWeight[5];
        for (int recursion = 0; recursion < 5; recursion ++)
            sampleSumPerRecursion[recursion] = 0;
        for(int pass = 1; pass <= 5; pass ++)
        {
            sampleSumPerPass[pass-1] = 0;
            for (int recursion = 0; recursion < 5; recursion ++)
            {
                unsigned int id = kPOVAttrib_RadSamplesP1R0 + (pass-1)*256 + recursion;
                (void)POVMSUtil_GetLong(msg, id, &l);
                samples[pass-1][recursion] = l;
                sampleSumPerPass[pass-1] += l;
                sampleSumPerRecursion[recursion] += l;
                sampleSum += l;
            }
        }
        for (int recursion = 0; recursion < 5; recursion ++)
        {
            unsigned int id = kPOVAttrib_RadSamplesFR0 + recursion;
            (void)POVMSUtil_GetLong(msg, id, &l);
            samplesFinal[recursion] = l;
            sampleSumFinal += l;
            sampleSumPerRecursion[recursion] += l;
            sampleSum += l;
            id = kPOVAttrib_RadWeightR0 + recursion;
            (void)POVMSUtil_GetFloat(msg, id, &(sampleWeight[recursion]));
        }
        tsb->printf("----------------------------------------------------------------------------\n");
        tsb->printf("  Pass ");
        for (int recursion = 0; recursion < 5; recursion ++)
            if (POVMSLongToCDouble(sampleSumPerRecursion[recursion]) > 0.5)
                tsb->printf((recursion < 4? "    Depth %i" : "   Depth 4+") , recursion);
        tsb->printf("           Total\n");
        tsb->printf("----------------------------------------------------------------------------\n");
        for(int pass = 1; pass <= 5; pass ++)
        {
            if (sampleSumPerPass[pass-1] > 0)
            {
                tsb->printf("  %i%c   ", pass, (pass < 5 ? ' ' : '+'));
                for (int recursion = 0; recursion < 5; recursion ++)
                {
                    if (POVMSLongToCDouble(samples[pass-1][recursion]) > 0.5)
                        tsb->printf(" %10.0f", POVMSLongToCDouble(samples[pass-1][recursion]));
                    else if (POVMSLongToCDouble(sampleSumPerRecursion[recursion]) > 0.5)
                        tsb->printf("          -");
                }
                tsb->printf(" %15.0f\n", POVMSLongToCDouble(sampleSumPerPass[pass-1]));
            }
        }
        if (sampleSumFinal > 0)
        {
            tsb->printf("  Final");
            for (int recursion = 0; recursion < 5; recursion ++)
            {
                if (POVMSLongToCDouble(samplesFinal[recursion]) > 0.5)
                    tsb->printf(" %10.0f", POVMSLongToCDouble(samplesFinal[recursion]));
                else if (POVMSLongToCDouble(sampleSumPerRecursion[recursion]) > 0.5)
                    tsb->printf("          -");
            }
            tsb->printf(" %15.0f\n", POVMSLongToCDouble(sampleSumFinal));
        }
        tsb->printf("----------------------------------------------------------------------------\n");
        tsb->printf("  Total");
        for (int recursion = 0; recursion < 5; recursion ++)
        {
            if (POVMSLongToCDouble(sampleSumPerRecursion[recursion]) > 0.5)
                tsb->printf(" %10.0f", POVMSLongToCDouble(sampleSumPerRecursion[recursion]));
        }
        tsb->printf(" %15.0f\n", POVMSLongToCDouble(sampleSum));
        tsb->printf("  Weight");
        for (int recursion = 0; recursion < 5; recursion ++)
        {
            if (sampleWeight[recursion] > 1e-6)
                tsb->printf("%10.3f ", sampleWeight[recursion]);
            else if (POVMSLongToCDouble(sampleSumPerRecursion[recursion]) > 0.5)
                tsb->printf("         - ");
        }
        tsb->printf("\n");
    }

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_PhotonsShot, &l);
    if(POVMSLongToCDouble(l) > 0.5)
    {
        tsb->printf("----------------------------------------------------------------------------\n");
        tsb->printf("Number of photons shot: %15.0f\n", POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_PhotonsStored, &l);
        if(POVMSLongToCDouble(l) > 0.5)
            tsb->printf("Surface photons stored: %15.0f\n", POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_MediaPhotonsStored, &l);
        if(POVMSLongToCDouble(l) > 0.5)
            tsb->printf("Media photons stored:   %15.0f\n", POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_GlobalPhotonsStored, &l);
        if(POVMSLongToCDouble(l) > 0.5)
            tsb->printf("Global photons stored:  %15.0f\n", POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_PhotonsPriQInsert, &l);
        if(POVMSLongToCDouble(l) > 0.5)
            tsb->printf("Priority queue insert:  %15.0f\n", POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_PhotonsPriQRemove, &l);
        if(POVMSLongToCDouble(l) > 0.5)
            tsb->printf("Priority queue remove:  %15.0f\n", POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_GatherPerformedCnt, &l);
        if(POVMSLongToCDouble(l) > 0.5)
            tsb->printf("Gather function called: %15.0f\n", POVMSLongToCDouble(l));
        (void)POVMSUtil_GetLong(msg, kPOVAttrib_GatherExpandedCnt, &l);
        if(POVMSLongToCDouble(l) > 0.5)
            tsb->printf("Gather radius expanded: %15.0f\n", POVMSLongToCDouble(l));
    }

    tsb->printf("----------------------------------------------------------------------------\n");

    (void)POVMSUtil_GetLong(msg, kPOVAttrib_MinAlloc, &l);
    if (l > 0) // don't count allocs of 0 anyhow
        tsb->printf("Smallest Alloc:     %15.0f bytes\n", POVMSLongToCDouble(l));
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_MaxAlloc, &l);
    if (l > 0)
        tsb->printf("Largest  Alloc:     %15.0f bytes\n", POVMSLongToCDouble(l));

    l = 0;
    l2 = 0;
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_CallsToAlloc, &l);
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_CallsToFree, &l2);
    if(POVMSLongToCDouble(l) > 0.5)
        tsb->printf("Total Alloc calls:  %15.0f         Free calls:%15.0f\n", POVMSLongToCDouble(l), POVMSLongToCDouble(l2));

    l = 0;
    (void)POVMSUtil_GetLong(msg, kPOVAttrib_PeakMemoryUsage, &l);
    if(POVMSLongToCDouble(l) > 0.5)
        tsb->printf("Peak memory used:   %15.0f bytes\n", POVMSLongToCDouble(l));

    tsb->printf("----------------------------------------------------------------------------\n");

    POVMSObject_Delete(msg);
}

void Warning(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POVMSObject msgobj(cppmsg());
    POVMSObjectPtr msg = &msgobj;

    FileMessage(tsb, WARNING_STREAM, msg);

    POVMSObject_Delete(msg);
}

void Error(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POVMSObject msgobj(cppmsg());
    POVMSObjectPtr msg = &msgobj;

    FileMessage(tsb, WARNING_STREAM, msg);

    POVMSObject_Delete(msg);
}

void FatalError(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POVMSObject msgobj(cppmsg());
    POVMSObjectPtr msg = &msgobj;
    int ret = kNoErr;
    int l = 0;
    int s = 0;

    if(ret == kNoErr)
        FileMessage(tsb, FATAL_STREAM, msg);

    if(ret != kNoErr)
        throw POV_EXCEPTION_CODE(ret);

    POVMSObject_Delete(msg);
}

void FileMessage(TextStreamBuffer *tsb, int stream, POVMSObjectPtr msg)
{
    const int Num_Echo_Lines = 5; // TODO FIXME
    const int output_string_buffer_size = 1024; // TODO FIXME
    char output_string_buffer[output_string_buffer_size]; // TODO FIXME
    UCS2 filename[2048];

    POVMSLong ll = 0;
    int ret = kNoErr;
    int l = sizeof(filename);

    if(POVMSUtil_GetUCS2String(msg, kPOVAttrib_FileName, filename, &l) == kNoErr)
    {
        // TODO FIXME: we ought to support UCS2 string output.
        POVMSUCS2String fn(filename);
        std::string asciiFN(UCS2toSysString(fn));

        if((POVMSUtil_GetLong(msg, kPOVAttrib_Line, &ll) == kNoErr) && ((stream == WARNING_STREAM) || (stream == FATAL_STREAM)))
        {
            if ((asciiFN.empty() == false) && (ll > 0))
                tsb->printf("File: %s  Line: %ld\n", asciiFN.c_str(), ll);
        }
        if(((POVMSUtil_GetLong(msg, kPOVAttrib_FilePosition, &ll) == kNoErr) && (Num_Echo_Lines > 0)) && (stream == FATAL_STREAM))
        {
            tsb->printf("File Context (%d lines):\n", Num_Echo_Lines);
            tsb->printfile(asciiFN.c_str(), ll, -Num_Echo_Lines);
            tsb->printf("\n");
        }
    }

    l = output_string_buffer_size;
    output_string_buffer[0] = 0;
    ret = POVMSUtil_GetString(msg, kPOVAttrib_EnglishText, output_string_buffer, &l);
    if(ret == kNoErr)
        tsb->printf("%s\n", output_string_buffer);

    if(ret != kNoErr)
        throw POV_EXCEPTION_CODE(ret);
}

const char *GetOptionSwitchString(POVMSObjectPtr msg, POVMSType key, bool defaultstate)
{
    POVMSBool b = false;

    if(POVMSUtil_GetBool(msg, key, &b) != kNoErr)
        b = defaultstate;

    if(b == true)
        return ".On ";

    return ".Off";
}

void ParserTime(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POV_LONG i = 0;
    int msec = 0;
    int sec = 0;

    tsb->printf("Parser Time\n");

    POVMS_Object parseTime;
    POVMS_Object boundingTime;

    if(cppmsg.Exist(kPOVAttrib_ParseTime))
    {
        cppmsg.Get(kPOVAttrib_ParseTime, parseTime);
        i = parseTime.TryGetLong(kPOVAttrib_RealTime, 0);
        sec = int(i / (POV_LONG)(1000));
        msec = int(i % (POV_LONG)(1000));
        tsb->printf("  Parse Time:     %3d hours %2d minutes %2d seconds (%d.%03d seconds)\n", int(sec / 3600), int((sec / 60) % 60), int(sec % 60), sec, msec);
        if(parseTime.Exist(kPOVAttrib_CPUTime) == true)
        {
            i = parseTime.TryGetLong(kPOVAttrib_CPUTime, 0);
            sec = int(i / (POV_LONG)(1000));
            msec = int(i % (POV_LONG)(1000));
            tsb->printf("              using %d thread(s) with %d.%03d CPU-seconds total\n", int(parseTime.TryGetInt(kPOVAttrib_TimeSamples, 1)), sec, msec);
        }
        else
            tsb->printf("              using %d thread(s)\n", int(parseTime.TryGetInt(kPOVAttrib_TimeSamples, 1)));
    }
    else
        tsb->printf("  Parse Time:       No parsing\n");

    if(cppmsg.Exist(kPOVAttrib_BoundingTime))
    {
        cppmsg.Get(kPOVAttrib_BoundingTime, boundingTime);
        i = boundingTime.TryGetLong(kPOVAttrib_RealTime, 0);
        sec = int(i / (POV_LONG)(1000));
        msec = int(i % (POV_LONG)(1000));
        tsb->printf("  Bounding Time:  %3d hours %2d minutes %2d seconds (%d.%03d seconds)\n", int(sec / 3600), int((sec / 60) % 60), int(sec % 60), sec, msec);
        if(boundingTime.Exist(kPOVAttrib_CPUTime) == true)
        {
            i = boundingTime.TryGetLong(kPOVAttrib_CPUTime, 0);
            sec = int(i / (POV_LONG)(1000));
            msec = int(i % (POV_LONG)(1000));
            tsb->printf("              using %d thread(s) with %d.%03d CPU-seconds total\n", int(boundingTime.TryGetInt(kPOVAttrib_TimeSamples, 1)), sec, msec);
        }
        else
            tsb->printf("              using %d thread(s)\n", int(boundingTime.TryGetInt(kPOVAttrib_TimeSamples, 1)));
    }
    else
        tsb->printf("  Bounding Time:    No bounding\n");
}

void RenderTime(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    POV_LONG i = 0;
    int msec = 0;
    int sec = 0;

    tsb->printf("Render Time:\n");

    POVMS_Object photonTime;
    POVMS_Object radiosityTime;
    POVMS_Object renderTime;

    if(cppmsg.Exist(kPOVAttrib_PhotonTime))
    {
        cppmsg.Get(kPOVAttrib_PhotonTime, photonTime);
        i = photonTime.TryGetLong(kPOVAttrib_RealTime, 0);
        sec = int(i / (POV_LONG)(1000));
        msec = int(i % (POV_LONG)(1000));
        tsb->printf("  Photon Time:    %3d hours %2d minutes %2d seconds (%d.%03d seconds)\n", int(sec / 3600), int((sec / 60) % 60), int(sec % 60), sec, msec);
        if(photonTime.Exist(kPOVAttrib_CPUTime) == true)
        {
            i = photonTime.TryGetLong(kPOVAttrib_CPUTime, 0);
            sec = int(i / (POV_LONG)(1000));
            msec = int(i % (POV_LONG)(1000));
            tsb->printf("              using %d thread(s) with %d.%03d CPU-seconds total\n", int(photonTime.TryGetInt(kPOVAttrib_TimeSamples, 1)), sec, msec);
        }
        else
            tsb->printf("              using %d thread(s)\n", int(photonTime.TryGetInt(kPOVAttrib_TimeSamples, 1)));
    }
    else
        tsb->printf("  Photon Time:      No photons\n");

    if(cppmsg.Exist(kPOVAttrib_RadiosityTime))
    {
        cppmsg.Get(kPOVAttrib_RadiosityTime, radiosityTime);
        i = radiosityTime.TryGetLong(kPOVAttrib_RealTime, 0);
        sec = int(i / (POV_LONG)(1000));
        msec = int(i % (POV_LONG)(1000));
        tsb->printf("  Radiosity Time: %3d hours %2d minutes %2d seconds (%d.%03d seconds)\n", int(sec / 3600), int((sec / 60) % 60), int(sec % 60), sec, msec);
        if(radiosityTime.Exist(kPOVAttrib_CPUTime) == true)
        {
            i = radiosityTime.TryGetLong(kPOVAttrib_CPUTime, 0);
            sec = int(i / (POV_LONG)(1000));
            msec = int(i % (POV_LONG)(1000));
            tsb->printf("              using %d thread(s) with %d.%03d CPU-seconds total\n", int(radiosityTime.TryGetInt(kPOVAttrib_TimeSamples, 1)), sec, msec);
        }
        else
            tsb->printf("              using %d thread(s)\n", int(radiosityTime.TryGetInt(kPOVAttrib_TimeSamples, 1)));
    }
    else
        tsb->printf("  Radiosity Time:   No radiosity\n");

    if(cppmsg.Exist(kPOVAttrib_TraceTime))
    {
        cppmsg.Get(kPOVAttrib_TraceTime, renderTime);
        i = renderTime.TryGetLong(kPOVAttrib_RealTime, 0);
        sec = int(i / (POV_LONG)(1000));
        msec = int(i % (POV_LONG)(1000));
        tsb->printf("  Trace Time:     %3d hours %2d minutes %2d seconds (%d.%03d seconds)\n", int(sec / 3600), int((sec / 60) % 60), int(sec % 60), sec, msec);
        if(renderTime.Exist(kPOVAttrib_CPUTime) == true)
        {
            i = renderTime.TryGetLong(kPOVAttrib_CPUTime, 0);
            sec = int(i / (POV_LONG)(1000));
            msec = int(i % (POV_LONG)(1000));
            tsb->printf("              using %d thread(s) with %d.%03d CPU-seconds total\n", int(renderTime.TryGetInt(kPOVAttrib_TimeSamples, 1)), sec, msec);
        }
        else
            tsb->printf("              using %d thread(s)\n", int(renderTime.TryGetInt(kPOVAttrib_TimeSamples, 1)));
    }
    else
        tsb->printf("  Trace Time:       No trace\n");
}

void DebugInfo(POVMS_Object& cppmsg, TextStreamBuffer *tsb)
{
    std::string str = cppmsg.TryGetString(kPOVAttrib_EnglishText, "<Error retrieving debug output>");
    tsb->printf("%s\n", str.c_str());
}

std::string GetProgressTime(POVMS_Object& obj, POVMSType key)
{
    int sec = int(obj.TryGetLong(kPOVAttrib_RealTime, 0) / (POV_LONG)(1000));
    char buffer[32];

    sprintf(buffer, "%3d:%02d:%02d", int(sec / 3600), int((sec / 60) % 60), int(sec % 60));

    return std::string(buffer);
}

void RenderDone(TextStreamBuffer *tsb, POVMSObjectPtr msg)
{/*
    POVMSObject object;
    int ret = 0;
    int i = 0;

    ret = POVMSObject_Get(msg, &object, kPOVAttrib_AnimationTime);
    if(ret == kNoErr)
        tsb->printf("Total Scene Processing Times\n");

    if(ret == kNoErr)
        ret = POVMSUtil_GetInt(&object, kPOVAttrib_ParseTime, &i);
    if(ret == kNoErr)
        tsb->printf("  Parse Time:  %3d hours %2d minutes %2d seconds (%d seconds)\n", (int)(i / 3600), (int)((i / 60) % 60), (int)(i % 60), (int)i);

    if(ret == kNoErr)
        ret = POVMSUtil_GetInt(&object, kPOVAttrib_PhotonTime, &i);
    if(ret == kNoErr)
        tsb->printf("  Photon Time: %3d hours %2d minutes %2d seconds (%d seconds)\n", (int)(i / 3600), (int)((i / 60) % 60), (int)(i % 60), (int)i);

    if(ret == kNoErr)
        ret = POVMSUtil_GetInt(&object, kPOVAttrib_TraceTime, &i);
    if(ret == kNoErr)
        tsb->printf("  Render Time: %3d hours %2d minutes %2d seconds (%d seconds)\n", (int)(i / 3600), (int)((i / 60) % 60), (int)(i % 60), (int)i);

    if(ret == kNoErr)
        ret = POVMSUtil_GetInt(&object, kPOVAttrib_TotalTime, &i);
    if(ret == kNoErr)
        tsb->printf("  Total Time:  %3d hours %2d minutes %2d seconds (%d seconds)\n", (int)(i / 3600), (int)((i / 60) % 60), (int)(i % 60), (int)i);

    (void)POVMSObject_Delete(&object);

    if(ret != kNoErr)
        throw POV_EXCEPTION_CODE(ret);*/
}

void Progress(TextStreamBuffer *tsb, POVMSObjectPtr msg)
{/*
    POVMSLong ll = 0;
    POVMSBool b = false;
    int ret = kNoErr;
    int l = 0;
    int s = 0;

//  Flush(DEBUG_STREAM);

    ret = POVMSUtil_GetBool(msg, kPOVAttrib_ProgressStatus, &b);
    if(ret == kNoErr)
        ret = POVMSUtil_GetInt(msg, kPOVAttrib_TotalTime, &s);
    if(ret == kNoErr)
    {
        l = 80;

        if(b == false)
        {
            ret = POVMSUtil_GetString(msg, kPOVAttrib_EnglishText, status_string_buffer, &l);
            if(ret == kNoErr)
                tsb->printf("\n%3d:%02d:%02d %s", (int)(s / 3600), (int)((s / 60) % 60), (int)(s % 60), status_string_buffer);
        }
        else // if(opts.Options & VERBOSE) // Should this be part of verbose reporting only or not? I really don't know which way would be better... [trf]
        {
            (void)POVMSUtil_GetString(msg, kPOVAttrib_EnglishText, status_string_buffer, &l);
            tsb->printf("\r%3d:%02d:%02d %s", (int)(s / 3600), (int)((s / 60) % 60), (int)(s % 60), status_string_buffer);
        }

// FIXME        if(opts.Options & VERBOSE)
        {
            // animation frame progress
            if(POVMSUtil_GetInt(msg, kPOVAttrib_FrameCount, &l) == kNoErr)
            {
                if(POVMSUtil_GetInt(msg, kPOVAttrib_AbsoluteCurFrame, &s) == kNoErr)
                    tsb->printf(" %d of %d", s, l);
            }
            // parsing progress
            else if((POVMSUtil_GetLong(msg, kPOVAttrib_CurrentTokenCount, &ll) == kNoErr) && (ll > 0))
            {
                tsb->printf(" %ldK tokens", long(((POV_LONG)(ll))/1000));
            }
            // rendering progress
            else if(POVMSUtil_GetInt(msg, kPOVAttrib_CurrentLine, &l) == kNoErr)
            {
                if(POVMSUtil_GetInt(msg, kPOVAttrib_LineCount, &s) == kNoErr)
                    tsb->printf(" line %d of %d", l, s);
                if(POVMSUtil_GetInt(msg, kPOVAttrib_MosaicPreviewSize, &l) == kNoErr)
                    tsb->printf(" at %dx%d", l, l);
                if(POVMSUtil_GetInt(msg, kPOVAttrib_SuperSampleCount, &l) == kNoErr)
                    tsb->printf(", %d supersamples", l);
                if(POVMSUtil_GetInt(msg, kPOVAttrib_RadGatherCount, &l) == kNoErr)
                    tsb->printf(", %d rad. samples", l);
            }
            // photon progress
            else if(POVMSUtil_GetInt(msg, kPOVAttrib_TotalPhotonCount, &l) == kNoErr)
            {
                // sorting
                if(POVMSUtil_GetInt(msg, kPOVAttrib_CurrentPhotonCount, &s) == kNoErr)
                    tsb->printf(" %d of %d", s, l);
                // shooting
                else
                {
                    tsb->printf(" Photons %d", l);
                    l = 0;
                    (void)POVMSUtil_GetInt(msg, kPOVAttrib_PhotonXSamples, &l);
                    s = 0;
                    (void)POVMSUtil_GetInt(msg, kPOVAttrib_PhotonYSamples, &s);
                    tsb->printf(" (sampling %dx%d)", l, s);
                }
            }
        }
    }

    if(ret != kNoErr)
        throw POV_EXCEPTION_CODE(ret);*/
}

}
// end of namespace Message2Console

}
// end of namespace pov_frontend
