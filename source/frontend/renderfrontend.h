//******************************************************************************
///
/// @file frontend/renderfrontend.h
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

#ifndef POVRAY_FRONTEND_RENDERFRONTEND_H
#define POVRAY_FRONTEND_RENDERFRONTEND_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "frontend/configfrontend.h"
#include "frontend/renderfrontend_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

// Boost header files
#include <boost/function.hpp>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/filesystem.h"
#include "base/path.h"
#include "base/platformbase.h"
#include "base/stringutilities.h"
#include "base/textstreambuffer_fwd.h"
#include "base/types.h"
#include "base/image/colourspace.h"
#include "base/image/image.h"

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"
#include "povms/povmsid.h"

// POV-Ray header files (frontend module)
#include "frontend/console.h"
#include "frontend/imageprocessing.h"

namespace pov_frontend
{

using namespace pov_base;

class Display;

enum
{
    BANNER_STREAM = 0,
    STATUS_STREAM,
    DEBUG_STREAM,
    FATAL_STREAM,
    RENDER_STREAM,
    STATISTIC_STREAM,
    WARNING_STREAM,
    ALL_STREAM,
    MAX_STREAMS
};

struct SceneData final
{
    enum SceneState
    {
        Scene_Unknown,
        Scene_Created,
        Scene_Parsing,
        Scene_Paused,
        Scene_Stopping,
        Scene_Ready,
        Scene_Viewing,
        Scene_Closing,
        Scene_Failed,
        Scene_Invalid
    };

    SceneState state;

    mutable std::shared_ptr<Console> console;

    mutable std::list<POVMS_Object> readfiles;
    mutable std::list<POVMS_Object> createdfiles;

    Path scenepath;
    Path outputpath;

    std::list<Path> searchpaths;

    std::shared_ptr<TextStreamBuffer> streams[MAX_STREAMS];

    UCS2String streamnames[MAX_STREAMS];
    bool consoleoutput[MAX_STREAMS];

    bool verbose;

    struct final
    {
        int legacyGammaMode;
        // TODO FIXME - conversion from working gamma to linear should be moved to back-end
        GammaTypeId workingGammaType;
        float workingGamma;
    } backwardCompatibilityData;
};

struct ViewData final
{
    enum ViewState
    {
        View_Unknown,
        View_Created,
        View_Rendering,
        View_Paused,
        View_Stopping,
        View_Rendered,
        View_Closing,
        View_Failed,
        View_Invalid
    };

    ViewState state;

    mutable std::shared_ptr<Image> image;
    mutable std::shared_ptr<Display> display;
    mutable std::shared_ptr<OStream> imageBackup;
    GammaCurvePtr displayGamma;
    bool greyscaleDisplay;

    Path imageBackupFile;
};

namespace Message2Console
{

void ParserStatistics(POVMS_Object& cppmsg, TextStreamBuffer *tsb);
void RenderStatistics(POVMS_Object& cppmsg, TextStreamBuffer *tsb);

void ParserOptions(POVMS_Object& cppmsg, TextStreamBuffer *tsb);
void RenderOptions(POVMS_Object& cppmsg, TextStreamBuffer *tsb);
void OutputOptions(POVMS_Object& cppmsg, TextStreamBuffer *tsb);
void AnimationOptions(POVMS_Object& cppmsg, TextStreamBuffer *tsb);

void Warning(POVMS_Object& cppmsg, TextStreamBuffer *tsb);
void Error(POVMS_Object& cppmsg, TextStreamBuffer *tsb);
void FatalError(POVMS_Object& cppmsg, TextStreamBuffer *tsb);

void ParserTime(POVMS_Object& cppmsg, TextStreamBuffer *tsb);
void RenderTime(POVMS_Object& cppmsg, TextStreamBuffer *tsb);

void DebugInfo(POVMS_Object& cppmsg, TextStreamBuffer *tsb);

std::string GetProgressTime(POVMS_Object& obj, POVMSType key);

}
// end of namespace Message2Console

#define RENDER_STATE_SIG "POV-Ray Render State File\0\0"
#define RENDER_STATE_VER "0001"

struct Backup_File_Header final
{
    unsigned char sig[28];
    unsigned char ver[4];
    unsigned char reserved[480];
};

class RenderFrontendBase : public POVMS_MessageReceiver
{
        class Id final
        {
            public:
                Id() : address(POVMSInvalidAddress), identifier(0) { }
                Id(POVMSAddress a, POVMSInt i) : address(a), identifier(i) { }
                /* operator< needed by std::map<> and std::set<>
                 * total order is required
                 * [JG] evaluation's order of expression is (ab)used:
                 *   first rank order is address, if equals, second rank is id
                 */
                bool operator<(const Id& o) const { return ((address <= o.address) && (identifier < o.identifier)); }
                // operator== needed by explicit compare
                bool operator==(const Id& o) const { return ((address == o.address) && (identifier == o.identifier)); }
                // no need of != so far
                bool operator!=(const Id& o) const { return ((address != o.address) || (identifier != o.identifier)); }
                POVMSAddress GetAddress() const { return address; }
                POVMSInt GetIdentifier() const { return identifier; }
            private:
                POVMSAddress address;
                POVMSInt identifier;
        };
    public:
        typedef Id SceneId;
        typedef Id ViewId;

        RenderFrontendBase(POVMSContext);
        virtual ~RenderFrontendBase();

        void ConnectToBackend(POVMSAddress, POVMS_Object&, POVMS_Object *, std::shared_ptr<Console>&);
        void DisconnectFromBackend(POVMSAddress);

        virtual std::shared_ptr<Console> GetConsole(SceneId) = 0;
        virtual std::shared_ptr<Image> GetImage(ViewId) = 0;
        virtual std::shared_ptr<Display> GetDisplay(ViewId) = 0;
    protected:

        typedef std::set<POVMSAddress> BackendAddressSet;

        BackendAddressSet backendaddresses;
        POVMSContext context;

        SceneId CreateScene(SceneData&, POVMSAddress, POVMS_Object&);
        void CloseScene(SceneData&, SceneId);

        void StartParser(SceneData&, SceneId, POVMS_Object&);
        void PauseParser(SceneData&, SceneId);
        void ResumeParser(SceneData&, SceneId);
        void StopParser(SceneData&, SceneId);

        ViewId CreateView(SceneData&, ViewData&, SceneId, POVMS_Object&);
        void CloseView(ViewData&, ViewId);

        void StartRender(ViewData&, ViewId, POVMS_Object&);
        void PauseRender(ViewData&, ViewId);
        void ResumeRender(ViewData&, ViewId);
        void StopRender(ViewData&, ViewId);

        void HandleMessage(POVMS_Message& msg, POVMS_Message& result, int mode);

        virtual void HandleParserMessage(SceneId, POVMSType, POVMS_Object&) = 0;
        virtual void HandleFileMessage(SceneId, POVMSType, POVMS_Object&, POVMS_Object&) = 0;
        virtual void HandleRenderMessage(ViewId, POVMSType, POVMS_Object&) = 0;
        virtual void HandleImageMessage(ViewId, POVMSType, POVMS_Object&) = 0;

        virtual void OutputFatalError(const std::string&, int) = 0;

        void MakeBackupPath(POVMS_Object& ropts, ViewData& vd, const Path& outputpath);
        void NewBackup(POVMS_Object& ropts, ViewData& vd, const Path& outputpath);
        void ContinueBackup(POVMS_Object& ropts, ViewData& vd, ViewId vid, POVMSInt& serial, std::vector<POVMSInt>& skip, const Path& outputpath);
};

// TODO - Do we really need this to be a template?

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
class RenderFrontend : public RenderFrontendBase
{
        struct SceneHandler final
        {
            SceneData data;

            PARSER_MH parser;
            FILE_MH file;
        };

        struct ViewHandler final
        {
            ViewData data;

            RENDER_MH render;
            IMAGE_MH image;
        };
    public:
        RenderFrontend(POVMSContext ctx);
        virtual ~RenderFrontend() override;

        SceneId CreateScene(POVMSAddress backendaddress, POVMS_Object& obj, boost::function<Console *()> fn);
        void CloseScene(SceneId sid);

        SceneData::SceneState GetSceneState(SceneId sid);

        void StartParser(SceneId sid, POVMS_Object& obj);
        void PauseParser(SceneId sid);
        void ResumeParser(SceneId sid);
        void StopParser(SceneId sid);

        ViewId CreateView(SceneId sid, POVMS_Object& obj, std::shared_ptr<ImageProcessing>& imageProcessing, boost::function<Display *(unsigned int, unsigned int)> fn);
        void CloseView(ViewId vid);

        ViewData::ViewState GetViewState(ViewId vid);

        void StartRender(ViewId vid, POVMS_Object& obj);
        void PauseRender(ViewId vid);
        void ResumeRender(ViewId vid);
        void StopRender(ViewId vid);

        virtual std::shared_ptr<Console> GetConsole(SceneId sid) override;
        virtual std::shared_ptr<Image> GetImage(ViewId vid) override;
        virtual std::shared_ptr<Display> GetDisplay(ViewId vid) override;
    protected:
        virtual void HandleParserMessage(SceneId sid, POVMSType ident, POVMS_Object& msg) override;
        virtual void HandleFileMessage(SceneId sid, POVMSType ident, POVMS_Object& msg, POVMS_Object& result) override;
        virtual void HandleRenderMessage(ViewId vid, POVMSType ident, POVMS_Object& msg) override;
        virtual void HandleImageMessage(ViewId vid, POVMSType ident, POVMS_Object& msg) override;
        virtual void OutputFatalError(const std::string& msg, int err) override;
    private:

        typedef std::set<ViewId>                ViewIdSet;
        typedef std::map<SceneId, SceneHandler> SceneHandlerMap;
        typedef std::map<ViewId,  ViewHandler>  ViewHandlerMap;
        typedef std::map<SceneId, ViewIdSet>    Scene2ViewsMap;
        typedef std::map<ViewId,  SceneId>      View2SceneMap;

        SceneHandlerMap scenehandler;
        ViewHandlerMap  viewhandler;
        Scene2ViewsMap  scene2views;
        View2SceneMap   view2scene;

        void GetBackwardCompatibilityData(SceneData& sd, POVMS_Object& msg);
};

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::RenderFrontend(POVMSContext ctx) :
    RenderFrontendBase(ctx)
{
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::~RenderFrontend()
{
    // nothing to do
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
RenderFrontendBase::SceneId RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::CreateScene(POVMSAddress backendaddress, POVMS_Object& obj, boost::function<Console*()> fn)
{
    SceneHandler sh;
    SceneId sid;

    try
    {
        sh.data.state = SceneData::Scene_Invalid;

        sid = RenderFrontendBase::CreateScene(sh.data, backendaddress, obj);

        sh.data.console = std::shared_ptr<Console>(fn());

        scenehandler[sid] = sh;
        scene2views[sid] = ViewIdSet();

        scenehandler[sid].data.console->Initialise();

        scenehandler[sid].data.state = SceneData::Scene_Created;
    }
    catch(pov_base::Exception&)
    {
        RenderFrontendBase::CloseScene(sh.data, sid);

        scenehandler.erase(sid);
        scene2views.erase(sid);

        throw;
    }

    return sid;
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::CloseScene(SceneId sid)
{
    typename SceneHandlerMap::iterator shi(scenehandler.find(sid));
    if(shi != scenehandler.end())
    {
        SceneData& data = shi->second.data;
        if((data.state != SceneData::Scene_Created) && (data.state != SceneData::Scene_Ready) && (data.state != SceneData::Scene_Failed))
            throw POV_EXCEPTION_STRING("TODO"); // TODO FIXME

        data.state = SceneData::Scene_Closing;

        if(scene2views[sid].size() > 0)
            throw POV_EXCEPTION_STRING("TODO"); // TODO FIXME

        RenderFrontendBase::CloseScene(data, sid);

        scenehandler.erase(shi);
        scene2views.erase(sid);
    }
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
SceneData::SceneState RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::GetSceneState(SceneId sid)
{
    typename SceneHandlerMap::iterator shi(scenehandler.find(sid));
    if(shi != scenehandler.end())
        return shi->second.data.state;
    else
        return SceneData::Scene_Unknown;
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::StartParser(SceneId sid, POVMS_Object& obj)
{
    typename SceneHandlerMap::iterator shi(scenehandler.find(sid));
    if(shi != scenehandler.end())
    {
        RenderFrontendBase::StartParser(shi->second.data, sid, obj);
        HandleParserMessage(sid, kPOVMsgIdent_ParserOptions, obj);
    }
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::PauseParser(SceneId sid)
{
    typename SceneHandlerMap::iterator shi(scenehandler.find(sid));
    if(shi != scenehandler.end())
        RenderFrontendBase::PauseParser(shi->second.data, sid);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::ResumeParser(SceneId sid)
{
    typename SceneHandlerMap::iterator shi(scenehandler.find(sid));
    if(shi != scenehandler.end())
        RenderFrontendBase::ResumeParser(shi->second.data, sid);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::StopParser(SceneId sid)
{
    typename SceneHandlerMap::iterator shi(scenehandler.find(sid));
    if(shi != scenehandler.end())
        RenderFrontendBase::StopParser(shi->second.data, sid);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
RenderFrontendBase::ViewId RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::CreateView(SceneId sid, POVMS_Object& obj, std::shared_ptr<ImageProcessing>& imageProcessing, boost::function<Display *(unsigned int,unsigned int)> fn)
{
    typename SceneHandlerMap::iterator shi(scenehandler.find(sid));

    if(shi != scenehandler.end())
    {
        if(shi->second.data.state != SceneData::Scene_Ready)
            throw POV_EXCEPTION_STRING("TODO"); // TODO FIXME

        ViewHandler vh;
        ViewId vid;

        // NOTE: No method in this class should ever change options passed to
        // it as input. However, for backward compatibility and proper design,
        // this is the only way to do it at the moment.
        try
        {
            unsigned int width(obj.TryGetInt(kPOVAttrib_Width, 160));
            unsigned int height(obj.TryGetInt(kPOVAttrib_Height, 120));

            GammaCurvePtr gamma;

            // check if we need to deviate from the gamma handling mode as implied by the scene file
            switch (shi->second.data.backwardCompatibilityData.legacyGammaMode)
            {
                case kPOVList_GammaMode_None:
                    // The scene didn't explicitly set assumed_gamma, and it ended up with a #version of less than v3.7,
                    // which normally means no gamma correction is done whatsoever.

                    if(obj.TryGetFloat(kPOVAttrib_Version, 0.0f) >= 3.7f) // v3.7.0
                    {
                        // INI file or command line specify a version of v3.7 or greater, which we take to imply that
                        // output gamma handling should follow the v3.7 model.

                        shi->second.data.backwardCompatibilityData.legacyGammaMode = kPOVList_GammaMode_AssumedGamma37Implied;
                        shi->second.data.backwardCompatibilityData.workingGammaType = DEFAULT_WORKING_GAMMA_TYPE;
                        shi->second.data.backwardCompatibilityData.workingGamma = DEFAULT_WORKING_GAMMA;
                        shi->second.data.console->Output("------------------------------------------------------------------------------\n"
                                                         "Warning: A version of 3.7 or greater was specified in an INI file or on the\n"
                                                         "command-line, but the scene finished parsing with a #version of 3.6x or\n"
                                                         "earlier and without assumed_gamma set. Output gamma correction is being turned\n"
                                                         "on as per the v3.7 default using an assumed_gamma default of " DEFAULT_WORKING_GAMMA_TEXT ",\n"
                                                         "rather than left off (which was the v3.6.x and earlier default), because the INI\n"
                                                         "file or command-line specified version directive takes precedence.");
                    }
                    break;

                case kPOVList_GammaMode_AssumedGamma36:
                case kPOVList_GammaMode_AssumedGamma37:
                case kPOVList_GammaMode_AssumedGamma37Implied:
                    // The scene explicitly set assumed_gamma, and/or ended up with a #version of v3.7 or higher,
                    // which normally means output gamma handling should follow the v3.6 / v3.7 assumed_gamma model
                    // (which differs only in input image gamma handling, which is irrelevant here).
                    break;

                default:
                    throw POV_EXCEPTION_STRING("Unknown gamma handling mode in CreateView()");
            }

            GammaTypeId dispGammaType   = (GammaTypeId) obj.TryGetInt   (kPOVAttrib_DisplayGammaType, DEFAULT_DISPLAY_GAMMA_TYPE);
            float       dispGamma       =               obj.TryGetFloat (kPOVAttrib_DisplayGamma,     DEFAULT_DISPLAY_GAMMA);

            switch (shi->second.data.backwardCompatibilityData.legacyGammaMode)
            {
                case kPOVList_GammaMode_None:
                    // Turn gamma handling off for both display and file output
                    obj.SetInt      (kPOVAttrib_FileGammaType,      dispGammaType);
                    obj.SetFloat    (kPOVAttrib_FileGamma,          dispGamma);
                    obj.SetInt      (kPOVAttrib_WorkingGammaType,   dispGammaType);
                    obj.SetFloat    (kPOVAttrib_WorkingGamma,       dispGamma);
                    gamma = NeutralGammaCurve::Get();
                    break;

                case kPOVList_GammaMode_AssumedGamma36:
                case kPOVList_GammaMode_AssumedGamma37:
                case kPOVList_GammaMode_AssumedGamma37Implied:
                    // Adjust display gamma based on assumed_gamma and Display_Gamma
                    // (note that here we also pass the information we got from the back-end onward to the front-end)
                    obj.SetInt      (kPOVAttrib_WorkingGammaType,   shi->second.data.backwardCompatibilityData.workingGammaType);
                    obj.SetFloat    (kPOVAttrib_WorkingGamma,       shi->second.data.backwardCompatibilityData.workingGamma);
                    gamma = TranscodingGammaCurve::Get(GetGammaCurve(shi->second.data.backwardCompatibilityData.workingGammaType, shi->second.data.backwardCompatibilityData.workingGamma),
                                                       GetGammaCurve(dispGammaType, dispGamma));
                    break;

                default:
                    throw POV_EXCEPTION_STRING("Unknown gamma handling mode in CreateView()");
            }
            vh.data.displayGamma = gamma;

            vh.data.greyscaleDisplay = obj.TryGetBool(kPOVAttrib_GrayscaleOutput, false);

            vh.data.state = ViewData::View_Invalid;

            vid = RenderFrontendBase::CreateView(shi->second.data, vh.data, sid, obj);

            if(obj.TryGetBool(kPOVAttrib_OutputToFile, true))
            {
                if (imageProcessing == nullptr)
                    throw POV_EXCEPTION(kNullPointerErr, "Internal error: output to file is set, but no ImageProcessing object supplied");
                std::shared_ptr<Image> img(imageProcessing->GetImage());
                if (img != nullptr)
                {
                    if((img->GetWidth() != width) || (img->GetHeight() != height))
                        throw POV_EXCEPTION_STRING("Invalid partial rendered image. Image size does not match!");

                    vh.data.image = img;
                }
                else
                    vh.data.image = std::shared_ptr<Image>(Image::Create(width, height, ImageDataType::RGBFT_Float));
            }

            if(obj.TryGetBool(kPOVAttrib_Display, true) == true)
                vh.data.display = std::shared_ptr<Display>(fn(width, height));

            viewhandler[vid] = vh;
            view2scene[vid] = sid;
            scene2views[sid].insert(vid);

            if (viewhandler[vid].data.display != nullptr)
                viewhandler[vid].data.display->Initialise();

            shi->second.data.state = SceneData::Scene_Viewing;
            viewhandler[vid].data.state = ViewData::View_Created;
        }
        catch(pov_base::Exception&)
        {
            RenderFrontendBase::CloseView(vh.data, vid);

            scene2views[view2scene[vid]].erase(vid);
            viewhandler.erase(vid);
            view2scene.erase(vid);

            throw;
        }

        return vid;
    }
    else
        throw POV_EXCEPTION_STRING("TODO"); // FIXME TODO
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::CloseView(ViewId vid)
{
    typename ViewHandlerMap::iterator vhi(viewhandler.find(vid));

    if(vhi != viewhandler.end())
    {
        ViewData& data = vhi->second.data;
        if((data.state != ViewData::View_Created) && (data.state != ViewData::View_Rendered) && (data.state != ViewData::View_Failed))
            throw POV_EXCEPTION_STRING("TODO"); // TODO FIXME

        RenderFrontendBase::CloseView(data, vid);

        scene2views[view2scene[vid]].erase(vid);
        if(scene2views[view2scene[vid]].empty() == true)
        {
            typename SceneHandlerMap::iterator shi(scenehandler.find(view2scene[vid]));
            shi->second.data.state = SceneData::Scene_Ready;
        }

        viewhandler.erase(vhi);
        view2scene.erase(vid);
    }
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
ViewData::ViewState RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::GetViewState(ViewId vid)
{
    typename ViewHandlerMap::iterator vhi(viewhandler.find(vid));
    if(vhi != viewhandler.end())
        return vhi->second.data.state;
    else
        return ViewData::View_Unknown;
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::StartRender(ViewId vid, POVMS_Object& obj)
{
    bool continueOK = false;

    typename ViewHandlerMap::iterator vhi(viewhandler.find(vid));
    if(vhi != viewhandler.end())
    {
        Path outputpath(obj.TryGetUCS2String(kPOVAttrib_OutputPath, ""));
        UCS2String filename = obj.TryGetUCS2String(kPOVAttrib_OutputFile, "");
        std::string fn(UCS2toSysString(filename));
        bool to_stdout = fn == "-" || fn == "stdout" || fn == "stderr";

        if(obj.TryGetBool(kPOVAttrib_ContinueTrace, false) == true)
        {
            if (to_stdout)
                if(obj.TryGetBool(kPOVAttrib_BackupTrace, true))
                    throw POV_EXCEPTION(kCannotHandleRequestErr, "Cannot continue trace if output to STDOUT or STDERR is specified");

            // File_Length returns -1 if the file doesn't exist
            if ((filename.length() == 0) || (GetFileLength(filename.c_str()) <= 0))
            {
                if(obj.TryGetBool(kPOVAttrib_BackupTrace, true) == true)
                {
                    try
                    {
                        POVMSInt serial;
                        std::vector<POVMSInt> skip;

                        ContinueBackup(obj, vhi->second.data, vid, serial, skip, outputpath);

                        obj.SetInt(kPOVAttrib_PixelId, serial);
                        if(skip.empty() == false)
                            obj.SetIntVector(kPOVAttrib_PixelSkipList, skip);
                        continueOK = true;
                    }
                    catch(pov_base::Exception&)
                    {
                        vhi->second.data.imageBackup.reset();
                    }
                }
            }
            else
            {
                // TODO: we need to check to see if the image file is of an appropriate
                //       size and format. if so we must skip the entire render.
                //       this will more or less duplicate the pre-v3.7 behaviour, except
                //       that those versions would first load the image and then display
                //       it if +d was set. I am not sure if it's important to replicate
                //       this behaviour, however from a user's point of view it might be
                //       confusing if a continue render finishes with nothing displayed.
                //       so we may need to add this.

                // Note that kImageAlreadyRenderedErr isn't really an 'error' per se, but
                // it is a clean way of telling the frontend not to proceed with rendering.
                // (I thought of making it a non-fatal error but that implies rendering
                // should proceed).
                throw POV_EXCEPTION_CODE(kImageAlreadyRenderedErr);
            }
        }

        // Logic: if the continue attempt above failed, or was not attempted, we attempt
        //        to create a render state ('backup') file, but only if output to file
        //        is on and is not to stdout.
        if(continueOK == false)
        {
            if(to_stdout == false && obj.TryGetBool(kPOVAttrib_BackupTrace, obj.TryGetBool(kPOVAttrib_OutputToFile, true)) == true)
            {
                try
                {
                    NewBackup(obj, vhi->second.data, outputpath);
                }
                catch(pov_base::Exception&)
                {
                    vhi->second.data.imageBackup.reset ();
                    throw;
                }
            }
        }

        RenderFrontendBase::StartRender(vhi->second.data, vid, obj);
        HandleRenderMessage(vid, kPOVMsgIdent_RenderOptions, obj);
    }
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::PauseRender(ViewId vid)
{
    typename ViewHandlerMap::iterator vhi(viewhandler.find(vid));
    if(vhi != viewhandler.end())
        RenderFrontendBase::PauseRender(vhi->second.data, vid);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::ResumeRender(ViewId vid)
{
    typename ViewHandlerMap::iterator vhi(viewhandler.find(vid));
    if(vhi != viewhandler.end())
        RenderFrontendBase::ResumeRender(vhi->second.data, vid);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::StopRender(ViewId vid)
{
    typename ViewHandlerMap::iterator vhi(viewhandler.find(vid));
    if(vhi != viewhandler.end())
        RenderFrontendBase::StopRender(vhi->second.data, vid);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
std::shared_ptr<Console> RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::GetConsole(SceneId sid)
{
    typename SceneHandlerMap::iterator shi(scenehandler.find(sid));
    if(shi != scenehandler.end())
        return shi->second.data.console;
    else
        return std::shared_ptr<Console>();
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
std::shared_ptr<Image> RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::GetImage(ViewId vid)
{
    typename ViewHandlerMap::iterator vhi(viewhandler.find(vid));
    if(vhi != viewhandler.end())
        return vhi->second.data.image;
    else
        return std::shared_ptr<Image>();
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
std::shared_ptr<Display> RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::GetDisplay(ViewId vid)
{
    typename ViewHandlerMap::iterator vhi(viewhandler.find(vid));
    if(vhi != viewhandler.end())
        return vhi->second.data.display;
    else
        return std::shared_ptr<Display>();
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::HandleParserMessage(SceneId sid, POVMSType ident, POVMS_Object& msg)
{
    typename SceneHandlerMap::iterator shi(scenehandler.find(sid));
    if(shi != scenehandler.end())
    {
        if(ident == kPOVMsgIdent_Done)
        {
            GetBackwardCompatibilityData(shi->second.data, msg);
            shi->second.data.state = SceneData::Scene_Ready;
        }
        else if(ident == kPOVMsgIdent_Failed)
        {
            std::string str("Fatal error in parser: ");
            str += msg.TryGetString(kPOVAttrib_EnglishText, "Unknown failure!");
            shi->second.data.console->Output(str);
            shi->second.data.state = SceneData::Scene_Failed;
        }
        else
            shi->second.parser.HandleMessage(shi->second.data, ident, msg);
    }
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::HandleFileMessage(SceneId sid, POVMSType ident, POVMS_Object& msg, POVMS_Object& result)
{
    typename SceneHandlerMap::iterator shi(scenehandler.find(sid));
    if(shi != scenehandler.end())
        shi->second.file.HandleMessage(shi->second.data, ident, msg, result);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::HandleRenderMessage(ViewId vid, POVMSType ident, POVMS_Object& msg)
{
    typename ViewHandlerMap::iterator vhi(viewhandler.find(vid));
    if(vhi != viewhandler.end())
    {
        SceneData& sceneData(scenehandler[view2scene[vid]].data);

        if(ident == kPOVMsgIdent_Done)
        {
            vhi->second.data.state = ViewData::View_Rendered;

            // close the state file if it's open
            if (vhi->second.data.imageBackup != nullptr)
            {
                vhi->second.data.imageBackup.reset();
                pov_base::Filesystem::DeleteFile(vhi->second.data.imageBackupFile());
            }
        }
        else if(ident == kPOVMsgIdent_Failed)
        {
            std::string str("Fatal error in renderer: ");
            str += msg.TryGetString(kPOVAttrib_EnglishText, "Unknown failure!");
            sceneData.console->Output(str);
            vhi->second.data.state = ViewData::View_Failed;

            // close the state file if it's open
            if (vhi->second.data.imageBackup != nullptr)
                vhi->second.data.imageBackup.reset();
        }
        else
            vhi->second.render.HandleMessage(sceneData, vhi->second.data, ident, msg);
    }
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::HandleImageMessage(ViewId vid, POVMSType ident, POVMS_Object& msg)
{
    typename ViewHandlerMap::iterator vhi(viewhandler.find(vid));
    if(vhi != viewhandler.end())
        vhi->second.image.HandleMessage(scenehandler[view2scene[vid]].data, vhi->second.data, ident, msg);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::OutputFatalError(const std::string& msg, int err)
{
    // TODO FIXME  CONSOLE::OutputFatalError(msg, err);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
void RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::GetBackwardCompatibilityData(SceneData& sd, POVMS_Object& msg)
{
    sd.backwardCompatibilityData.legacyGammaMode = msg.TryGetInt(kPOVAttrib_LegacyGammaMode, kPOVList_GammaMode_None); // TODO FIXME - default shouldn't be hard-coded in here.
    sd.backwardCompatibilityData.workingGammaType = (GammaTypeId)msg.TryGetInt(kPOVAttrib_WorkingGammaType, DEFAULT_WORKING_GAMMA_TYPE);
    sd.backwardCompatibilityData.workingGamma = msg.TryGetFloat(kPOVAttrib_WorkingGamma, DEFAULT_WORKING_GAMMA);
}

namespace Message2TSB
{
    void InitInfo(TextStreamBuffer *, POVMSObjectPtr);
    void RenderOptions(TextStreamBuffer *, POVMSObjectPtr);
    void RenderStarted(TextStreamBuffer *, POVMSObjectPtr);
    void FrameStatistics(TextStreamBuffer *, POVMSObjectPtr);
    void ParseStatistics(TextStreamBuffer *, POVMSObjectPtr);
    void RenderStatistics(TextStreamBuffer *, POVMSObjectPtr);
    void RenderDone(TextStreamBuffer *, POVMSObjectPtr);
    void Progress(TextStreamBuffer *, POVMSObjectPtr);
    void Warning(TextStreamBuffer *, POVMSObjectPtr);
    void Error(TextStreamBuffer *, POVMSObjectPtr);
    void FatalError(TextStreamBuffer *, POVMSObjectPtr);
    void DebugInfo(TextStreamBuffer *, POVMSObjectPtr);
    void FileMessage(TextStreamBuffer *, int stream, POVMSObjectPtr);
    const char *GetOptionSwitchString(POVMSObjectPtr, POVMSType, bool defaultstate = false);
}
// end of namespace Message2TSB

}
// end of namespace pov_frontend

#endif // POVRAY_FRONTEND_RENDERFRONTEND_H
