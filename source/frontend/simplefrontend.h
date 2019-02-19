//******************************************************************************
///
/// @file frontend/simplefrontend.h
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

#ifndef POVRAY_FRONTEND_SIMPLEFRONTEND_H
#define POVRAY_FRONTEND_SIMPLEFRONTEND_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "frontend/configfrontend.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// Boost header files
#include <boost/function.hpp>

// POV-Ray header files (base module)
#include "base/povassert.h"
#include "base/stringutilities.h"
#include "base/image/image_fwd.h"

// POV-Ray header files (POVMS module)
//  (none at the moment)

// POV-Ray header files (frontend module)
#include "frontend/animationprocessing.h"
#include "frontend/renderfrontend.h"
#include "frontend/shelloutprocessing.h"

namespace pov_frontend
{

using namespace pov_base;

class ImageProcessing;
class AnimationProcessing;
class ShelloutProcessing;

enum State
{
    kUnknown,
    kReady,
    kStarting,
    kPreSceneShellout,
    kPreFrameShellout,
    kParsing,
    kPausedParsing,
    kRendering,
    kPausedRendering,
    kPostFrameShellout,
    kPostSceneShellout,
    kPostShelloutPause,
    kStopping,
    kStopped,
    kFailed,
    kDone
};

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
class SimpleFrontend final
{
    public:
        SimpleFrontend(POVMSContext ctx, POVMSAddress addr, POVMS_Object& msg,
                       boost::function<Console *()> cfn,
                       boost::function<Display *(unsigned int, unsigned int)> dfn,
                       POVMS_Object *result = nullptr, std::shared_ptr<Console> console = std::shared_ptr<Console>());
        ~SimpleFrontend();

        bool Start(POVMS_Object& opts, std::shared_ptr<Image> img = std::shared_ptr<Image>());
        bool Stop();
        bool Pause();
        bool Resume();

        State Process();

        State GetState() const;

        std::shared_ptr<Console> GetConsole();
        std::shared_ptr<Image> GetImage();
        std::shared_ptr<Display> GetDisplay();
    private:
        RenderFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH> renderFrontend;
        POVMSAddress backendAddress;
        State state;
        POVMS_Object options;
        RenderFrontendBase::SceneId sceneId;
        RenderFrontendBase::ViewId viewId;
        std::shared_ptr<ImageProcessing> imageProcessing;
        std::shared_ptr<AnimationProcessing> animationProcessing;
        std::shared_ptr<ShelloutProcessing> shelloutProcessing;
        boost::function<Console *()> createConsole;
        boost::function<Display *(unsigned int, unsigned int)> createDisplay;
};

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::SimpleFrontend(POVMSContext ctx, POVMSAddress addr, POVMS_Object& msg,
                                                                        boost::function<Console *()> cfn,
                                                                        boost::function<Display *(unsigned int, unsigned int)> dfn,
                                                                        POVMS_Object *result, std::shared_ptr<Console> console) :
    renderFrontend(ctx),
    backendAddress(addr),
    state(kReady),
    createConsole(cfn),
    createDisplay(dfn)
{
    renderFrontend.ConnectToBackend(backendAddress, msg, result, console);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::~SimpleFrontend()
{
    renderFrontend.DisconnectFromBackend(backendAddress);
    state = kUnknown;
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
bool SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::Start(POVMS_Object& opts, std::shared_ptr<Image> img)
{
    int width;
    int height;

    if(state != kReady)
        return false;

    animationProcessing.reset();

    POVMS_List declares;
    if(opts.Exist(kPOVAttrib_Declare) == true)
        opts.Get(kPOVAttrib_Declare, declares);

    POVMS_Object image_width(kPOVMSType_WildCard);
    image_width.SetString(kPOVAttrib_Identifier, "image_width");
    image_width.SetFloat(kPOVAttrib_Value, width = opts.TryGetInt(kPOVAttrib_Width, 160));
    declares.Append(image_width);

    POVMS_Object image_height(kPOVMSType_WildCard);
    image_height.SetString(kPOVAttrib_Identifier, "image_height");
    image_height.SetFloat(kPOVAttrib_Value, height = opts.TryGetInt(kPOVAttrib_Height, 120));
    declares.Append(image_height);

    opts.Set(kPOVAttrib_Declare, declares);

    if(opts.TryGetInt(kPOVAttrib_FinalFrame, 0) > 0)
        animationProcessing = std::shared_ptr<AnimationProcessing>(new AnimationProcessing(opts));

    options = opts;

    if(opts.TryGetBool(kPOVAttrib_OutputToFile, true))
    {
        if (img != nullptr)
            imageProcessing = std::shared_ptr<ImageProcessing>(new ImageProcessing(img));
        else
            imageProcessing = std::shared_ptr<ImageProcessing>(new ImageProcessing(options));
    }

    Path ip (opts.TryGetString(kPOVAttrib_InputFile, ""));
    shelloutProcessing.reset(new ShelloutProcessing(opts, UCS2toSysString(ip.GetFile()), width, height));

    state = kStarting;

    return true;
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
bool SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::Stop()
{
    switch(state)
    {
        case kStarting:
            state = kStopped;
            return true;

        case kPreSceneShellout:
        case kPostFrameShellout:
        case kPostSceneShellout:
        case kPreFrameShellout:
            // TODO: add support for stopping shellouts, then halting render
            throw POV_EXCEPTION(kCannotHandleRequestErr, "Shellout code not active yet (how did we get here?)");

        case kParsing:
            renderFrontend.StopParser(sceneId);
            state = kStopping;
            return true;
        case kRendering:
            renderFrontend.StopRender(viewId);
            state = kStopping;
            return true;
    }

    return false;
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
bool SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::Pause()
{
    switch(state)
    {
        case kPreSceneShellout:
        case kPreFrameShellout:
        case kPostFrameShellout:
            // TODO: if we are running a shellout, we should allow request of a pause, which will
            // take effect after the shellout returns (i.e. it won't pause the program being run
            // but will pause POV-Ray afterwards).
            throw POV_EXCEPTION(kCannotHandleRequestErr, "Shellout code not active yet (how did we get here?)");
            break;

        case kParsing:
            renderFrontend.PauseParser(sceneId);
            state = kPausedParsing;
            return true;

        case kRendering:
            renderFrontend.PauseRender(viewId);
            state = kPausedRendering;
            return true;
    }

    return false;
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
bool SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::Resume()
{
    switch(state)
    {
        case kPausedParsing:
            renderFrontend.ResumeParser(sceneId);
            state = kParsing;
            return true;
        case kPausedRendering:
            renderFrontend.ResumeRender(viewId);
            state = kRendering;
            return true;

        case kPreSceneShellout:
        case kPreFrameShellout:
        case kPostFrameShellout:
            // TODO: clear any pause that was requested while a shellout was running
            // (would not have been acted on yet)
            throw POV_EXCEPTION(kCannotHandleRequestErr, "Shellout code not active yet (how did we get here?)");
    }

    return false;
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
State SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::Process()
{
    switch(state)
    {
        case kReady:
            return kReady;
        case kStarting:
            try
            {
                if (animationProcessing != nullptr)
                {
                    options = animationProcessing->GetFrameRenderOptions();
                    if (imageProcessing != nullptr)
                        options.SetUCS2String(kPOVAttrib_OutputFile, imageProcessing->GetOutputFilename(options, animationProcessing->GetNominalFrameNumber(), animationProcessing->GetFrameNumberDigits()).c_str());
                }
                else
                    if (imageProcessing != nullptr)
                        options.SetUCS2String(kPOVAttrib_OutputFile, imageProcessing->GetOutputFilename(options, 0, 0).c_str());
            }
            catch(pov_base::Exception&)
            {
                state = kFailed;
                // TODO - output failure message
                return kFailed;
            }

            try { sceneId = renderFrontend.CreateScene(backendAddress, options, createConsole); }
            catch(pov_base::Exception&)
            {
                state = kFailed;
                // TODO - output failure message
                return kFailed;
            }

            try { renderFrontend.StartParser(sceneId, options); }
            catch(pov_base::Exception&)
            {
                state = kFailed;
                // TODO - output failure message
                return kFailed;
            }

            state = kParsing;

            return kParsing;

        case kPreSceneShellout:
        case kPostFrameShellout:
        case kPostSceneShellout:
        case kPreFrameShellout:
            // TODO: add support for shellouts
            throw POV_EXCEPTION(kCannotHandleRequestErr, "Shellout code not active yet (how did we get here?)");

        case kParsing:
            switch(renderFrontend.GetSceneState(sceneId))
            {
                case SceneData::Scene_Failed:
                    state = kStopped;
                    return kStopped;
                case SceneData::Scene_Stopping:
                    state = kStopping;
                    return kStopping;
                case SceneData::Scene_Ready:
                    try { viewId = renderFrontend.CreateView(sceneId, options, imageProcessing, createDisplay); }
                    catch(pov_base::Exception&)
                    {
                        state = kFailed;
                        // TODO - output failure message
                        return kFailed;
                    }

                    try { renderFrontend.StartRender(viewId, options); }
                    catch(pov_base::Exception& e)
                    {
                        if(e.codevalid() && (e.code() == kImageAlreadyRenderedErr)) // TODO FIXME - This can be done much simpler: Just do nothing!
                        {
                            // this is not a failure; continue has been requested and
                            // the file has already been rendered, so we skip it.
                            if ((animationProcessing != nullptr) && (animationProcessing->MoreFrames() == true))
                            {
                                animationProcessing->ComputeNextFrame();
                                state = kStarting;
                                return kStarting;
                            }
                            else
                            {
                                state = kDone;
                                return kDone;
                            }
                        }
                        state = kFailed;
                        // TODO - output failure message
                        return kFailed;
                    }

                    state = kRendering;

                    return kRendering;

                default:
                    return state;
            }
            POV_FRONTEND_ASSERT(false); // All cases of the preceding switch should return.

        case kRendering:
            switch(renderFrontend.GetViewState(viewId))
            {
                case ViewData::View_Failed:
                    state = kStopped;
                    return kStopped;
                case ViewData::View_Stopping:
                    state = kStopping;
                    return kStopping;
                case ViewData::View_Rendered:
                    if (imageProcessing != nullptr)
                    {
                        try
                        {
                            if (animationProcessing != nullptr)
                                imageProcessing->WriteImage(options, animationProcessing->GetNominalFrameNumber(), animationProcessing->GetFrameNumberDigits());
                            else
                                imageProcessing->WriteImage(options);
                        }
                        catch(...)
                        {
                            state = kFailed;
                            // TODO - output failure message
                            return kFailed;
                        }
                    }

                    if ((animationProcessing != nullptr) && (animationProcessing->MoreFrames() == true))
                    {
                        try { renderFrontend.CloseView(viewId); } catch(...) { } // Ignore any error here!
                        try { renderFrontend.CloseScene(sceneId); } catch(...) { } // Ignore any error here!
                        animationProcessing->ComputeNextFrame();
                        state = kStarting;
                        return kStarting;
                    }
                    else
                    {
                        state = kDone;
                        return kDone;
                    }

                default:
                    return state;
            }
            POV_FRONTEND_ASSERT(false); // All cases of the preceding switch should return.

        case kStopping:
            if(renderFrontend.GetSceneState(sceneId) == SceneData::Scene_Ready || renderFrontend.GetSceneState(sceneId) == SceneData::Scene_Failed)
            {
                state = kStopped;
                return kStopped;
            }
            else if(renderFrontend.GetViewState(viewId) == ViewData::View_Rendered)
            {
                state = kStopped;
                return kStopped;
            }
            return kRendering;
        case kStopped:
        case kFailed:
        case kDone:
            try { renderFrontend.CloseView(viewId); } catch(...) { } // Ignore any error here!
            try { renderFrontend.CloseScene(sceneId); } catch(...) { } // Ignore any error here!
            animationProcessing.reset();

            state = kReady;

            return kReady;

        default:
            return state;
    }
    POV_FRONTEND_ASSERT(false); // All cases of the preceding switch should return.
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
State SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::GetState() const
{
    return state;
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
std::shared_ptr<Console> SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::GetConsole()
{
    return renderFrontend.GetConsole(sceneId);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
std::shared_ptr<Image> SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::GetImage()
{
    return renderFrontend.GetImage(viewId);
}

template<class PARSER_MH, class FILE_MH, class RENDER_MH, class IMAGE_MH>
std::shared_ptr<Display> SimpleFrontend<PARSER_MH, FILE_MH, RENDER_MH, IMAGE_MH>::GetDisplay()
{
    return renderFrontend.GetDisplay(viewId);
}

}
// end of namespace pov_frontend

#endif // POVRAY_FRONTEND_SIMPLEFRONTEND_H
