//******************************************************************************
///
/// @file backend/control/renderbackend.cpp
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
#include "backend/control/renderbackend.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"

// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"
#include "povms/povmsid.h"

// POV-Ray header files (backend module)
#include "backend/povray.h"
#include "backend/control/scene.h"
#include "backend/scene/view.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using namespace pov_base;

POVMSContext RenderBackend::context = nullptr;

RenderBackend::RenderBackend(POVMSContext ctx,  bool (*val)(POVMSAddress)) :
    POVMS_MessageReceiver(ctx),
    validateFrontendAddress(val),
    scenecounter(0),
    viewcounter(0)
{
    context = ctx;

    InstallFront(kPOVMsgClass_BackendControl, kPOVMsgIdent_CreateScene, this, &RenderBackend::CreateScene);
    InstallFront(kPOVMsgClass_BackendControl, kPOVMsgIdent_CloseScene, this, &RenderBackend::CloseScene);

    InstallFront(kPOVMsgClass_SceneControl, kPOVMsgIdent_CreateView, this, &RenderBackend::CreateView);
    InstallFront(kPOVMsgClass_SceneControl, kPOVMsgIdent_CloseView, this, &RenderBackend::CloseView);

    InstallFront(kPOVMsgClass_SceneControl, kPOVMsgIdent_StartParser, this, &RenderBackend::StartParser);
    InstallFront(kPOVMsgClass_SceneControl, kPOVMsgIdent_StopParser, this, &RenderBackend::StopParser);
    InstallFront(kPOVMsgClass_SceneControl, kPOVMsgIdent_PauseParser, this, &RenderBackend::PauseParser);
    InstallFront(kPOVMsgClass_SceneControl, kPOVMsgIdent_ResumeParser, this, &RenderBackend::ResumeParser);

    InstallFront(kPOVMsgClass_ViewControl, kPOVMsgIdent_StartRender, this, &RenderBackend::StartRender);
    InstallFront(kPOVMsgClass_ViewControl, kPOVMsgIdent_StopRender, this, &RenderBackend::StopRender);
    InstallFront(kPOVMsgClass_ViewControl, kPOVMsgIdent_PauseRender, this, &RenderBackend::PauseRender);
    InstallFront(kPOVMsgClass_ViewControl, kPOVMsgIdent_ResumeRender, this, &RenderBackend::ResumeRender);

    InstallFront(kPOVMsgClass_FileAccess, kPOVMsgIdent_ReadFile, this, &RenderBackend::ReadFile);
}

RenderBackend::~RenderBackend()
{
}

void RenderBackend::SendSceneOutput(SceneId sid, POVMSAddress addr, POVMSType ident, POVMS_Object& obj)
{
    POVMS_Message msg(obj, kPOVMsgClass_SceneOutput, ident);

    msg.SetInt(kPOVAttrib_SceneId, sid);
    msg.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, msg, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendViewOutput(ViewId vid, POVMSAddress addr, POVMSType ident, POVMS_Object& obj)
{
    POVMS_Message msg(obj, kPOVMsgClass_ViewOutput, ident);

    msg.SetInt(kPOVAttrib_ViewId, vid);
    msg.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, msg, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendFindFile(POVMSContext ctx, SceneId sid, POVMSAddress addr, const std::vector<POVMSUCS2String>& filenames, POVMSUCS2String& filename)
{
    POVMS_Message msg(kPOVObjectClass_FileData, kPOVMsgClass_FileAccess, kPOVMsgIdent_FindFile);
    POVMS_Message result(kPOVObjectClass_FileData, kPOVMsgClass_FileAccess, kPOVMsgIdent_FindFile);
    POVMS_List files;

    for(std::vector<POVMSUCS2String>::const_iterator i(filenames.begin()); i != filenames.end(); i++)
    {
        POVMS_Attribute attr(i->c_str());
        files.Append(attr);
    }

    msg.Set(kPOVAttrib_ReadFile, files);

    msg.SetInt(kPOVAttrib_SceneId, sid);
    msg.SetDestinationAddress(addr);

    POVMS_SendMessage(ctx, msg, &result, kPOVMSSendMode_WaitReply);

    filename = result.TryGetUCS2String(kPOVAttrib_ReadFile, "");
}

void RenderBackend::SendReadFile(POVMSContext ctx, SceneId sid, POVMSAddress addr, const POVMSUCS2String& filename, POVMSUCS2String& localfile, POVMSUCS2String& fileurl)
{
    POVMS_Message msg(kPOVObjectClass_FileData, kPOVMsgClass_FileAccess, kPOVMsgIdent_ReadFile);
    POVMS_Message result(kPOVObjectClass_FileData, kPOVMsgClass_FileAccess, kPOVMsgIdent_ReadFile);

    msg.SetUCS2String(kPOVAttrib_ReadFile, filename.c_str());

    msg.SetInt(kPOVAttrib_SceneId, sid);
    msg.SetDestinationAddress(addr);

    POVMS_SendMessage(ctx, msg, &result, kPOVMSSendMode_WaitReply);

    localfile = result.TryGetUCS2String(kPOVAttrib_LocalFile, "");
    fileurl = result.TryGetUCS2String(kPOVAttrib_FileURL, "");
}

void RenderBackend::SendCreatedFile(POVMSContext ctx, SceneId sid, POVMSAddress addr, const POVMSUCS2String& filename)
{
    POVMS_Message msg(kPOVObjectClass_FileData, kPOVMsgClass_FileAccess, kPOVMsgIdent_CreatedFile);

    msg.SetUCS2String(kPOVAttrib_CreatedFile, filename.c_str());

    msg.SetInt(kPOVAttrib_SceneId, sid);
    msg.SetDestinationAddress(addr);

    POVMS_SendMessage(ctx, msg, nullptr, kPOVMSSendMode_NoReply);
}

void RenderBackend::SendSuccessResult(POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_BackendControl, kPOVMsgIdent_Done);

    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendFailedResult(int error, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_BackendControl, kPOVMsgIdent_Failed);

    result.SetInt(kPOVAttrib_ErrorNumber, error);
    result.SetString(kPOVAttrib_EnglishText, pov_base::Exception::lookup_code(error).c_str());
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendFailedResult(const pov_base::Exception& e, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_BackendControl, kPOVMsgIdent_Failed);

    result.SetInt(kPOVAttrib_ErrorNumber, e.code(kCannotHandleRequestErr));
    // pov_base::Exception(...) does a code->string lookup if a string isn't supplied
    result.SetString(kPOVAttrib_EnglishText, e.what());
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendFailedResult(const char *str, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_BackendControl, kPOVMsgIdent_Failed);

    result.SetString(kPOVAttrib_EnglishText, str);
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendSceneSuccessResult(SceneId sid, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_SceneOutput, kPOVMsgIdent_Done);

    result.SetInt(kPOVAttrib_SceneId, sid);
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendSceneFailedResult(SceneId sid, int error, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_SceneOutput, kPOVMsgIdent_Failed);

    result.SetInt(kPOVAttrib_SceneId, sid);
    result.SetInt(kPOVAttrib_ErrorNumber, error);
    result.SetString(kPOVAttrib_EnglishText, pov_base::Exception::lookup_code(error).c_str());
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendSceneFailedResult(SceneId sid, const pov_base::Exception& e, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_SceneOutput, kPOVMsgIdent_Failed);

    result.SetInt(kPOVAttrib_SceneId, sid);
    result.SetInt(kPOVAttrib_ErrorNumber, e.code(kCannotHandleRequestErr));
    // pov_base::Exception(...) does a code->string lookup if a string isn't supplied
    result.SetString(kPOVAttrib_EnglishText, e.what());
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendSceneFailedResult(SceneId sid, const char *str, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_SceneOutput, kPOVMsgIdent_Failed);

    result.SetInt(kPOVAttrib_SceneId, sid);
    result.SetString(kPOVAttrib_EnglishText, str);
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendViewSuccessResult(ViewId vid, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_ViewOutput, kPOVMsgIdent_Done);

    result.SetInt(kPOVAttrib_ViewId, vid);
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendViewFailedResult(ViewId vid, int error, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_ViewOutput, kPOVMsgIdent_Failed);

    result.SetInt(kPOVAttrib_ViewId, vid);
    result.SetInt(kPOVAttrib_ErrorNumber, error);
    result.SetString(kPOVAttrib_EnglishText, pov_base::Exception::lookup_code(error).c_str());
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendViewFailedResult(ViewId vid, const pov_base::Exception& e, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_ViewOutput, kPOVMsgIdent_Failed);

    result.SetInt(kPOVAttrib_ViewId, vid);
    result.SetInt(kPOVAttrib_ErrorNumber, e.code(kCannotHandleRequestErr));
    // pov_base::Exception(...) does a code->string lookup if a string isn't supplied
    result.SetString(kPOVAttrib_EnglishText, e.what());
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::SendViewFailedResult(ViewId vid, const char *str, POVMSAddress addr)
{
    POVMS_Message result(kPOVObjectClass_ResultData, kPOVMsgClass_ViewOutput, kPOVMsgIdent_Failed);

    result.SetInt(kPOVAttrib_ViewId, vid);
    result.SetString(kPOVAttrib_EnglishText, str);
    result.SetDestinationAddress(addr);

    POVMS_SendMessage(RenderBackend::context, result, nullptr, kPOVMSSendMode_NoReply); // POVMS context provide for source address access only!
}

void RenderBackend::CreateScene(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    try
    {
        POVMSAddress backendAddress = POVMSInvalidAddress;
        int err = POVMS_GetContextAddress(context, &backendAddress);

        if(err != kNoErr)
            throw POV_EXCEPTION_CODE (err);

        std::shared_ptr<Scene> scene(new Scene(backendAddress, msg.GetSourceAddress(), scenecounter + 1));

        scenecounter++;

        POVMS_Message newresult(result, result.GetClass(), kPOVMsgIdent_Done);
        result = newresult;
        result.SetInt(kPOVAttrib_SceneId, scenecounter);

        scenes[scenecounter] = scene;
        try
        {
            scene2views[scenecounter] = ViewIdSet();
        }
        catch(std::exception&)
        {
            scenes.erase(scenecounter);
            throw;
        }
    }
    catch(pov_base::Exception& e)
    {
        MakeFailedResult(e, result);
    }
    catch(std::runtime_error& e)
    {
        MakeFailedResult(e.what(), result);
    }
    catch(std::bad_alloc&)
    {
        MakeFailedResult(kOutOfMemoryErr, result);
    }
}

void RenderBackend::CloseScene(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    try
    {
        SceneId sid = msg.GetInt(kPOVAttrib_SceneId);

        SceneMap::iterator i(scenes.find(sid));

        if(i == scenes.end())
            throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

        if(scene2views[sid].size() > 0)
            throw POV_EXCEPTION_CODE(kNotNowErr);

        try { scenes.erase(sid); } catch(pov_base::Exception&) { }
        try { scene2views.erase(sid); } catch(pov_base::Exception&) { }

        MakeDoneResult(result);
    }
    catch(pov_base::Exception& e)
    {
        MakeFailedResult(e, result);
    }
    catch(std::runtime_error& e)
    {
        MakeFailedResult(e.what(), result);
    }
    catch(std::bad_alloc&)
    {
        MakeFailedResult(kOutOfMemoryErr, result);
    }
}

void RenderBackend::CreateView(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    try
    {
        SceneId sid = msg.GetInt(kPOVAttrib_SceneId);

        SceneMap::iterator i(scenes.find(sid));

        if(i == scenes.end())
            throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

        std::shared_ptr<View> view(i->second->NewView(msg.TryGetInt(kPOVAttrib_Width, 160), msg.TryGetInt(kPOVAttrib_Height, 120), viewcounter + 1));

        viewcounter++;

        POVMS_Message newresult(result, result.GetClass(), kPOVMsgIdent_Done);
        result = newresult;
        result.SetInt(kPOVAttrib_ViewId, viewcounter);

        views[viewcounter] = view;
        try { view2scene[viewcounter] = sid; } catch(std::exception&) { views.erase(viewcounter); throw; }
        try { scene2views[sid].insert(viewcounter); } catch(std::exception&) { views.erase(viewcounter); view2scene.erase(viewcounter); throw; }
    }
    catch(pov_base::Exception& e)
    {
        MakeFailedResult(e, result);
    }
    catch(std::runtime_error& e)
    {
        MakeFailedResult(e.what(), result);
    }
    catch(std::bad_alloc&)
    {
        MakeFailedResult(kOutOfMemoryErr, result);
    }
}

void RenderBackend::CloseView(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    try
    {
        ViewId vid = msg.GetInt(kPOVAttrib_ViewId);

        ViewMap::iterator i(views.find(vid));

        if(i == views.end())
            throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

        try { views.erase(vid); } catch(pov_base::Exception&) { }
        try { scene2views[view2scene[vid]].erase(vid); } catch(pov_base::Exception&) { }
        try { view2scene.erase(vid); } catch(pov_base::Exception&) { }

        MakeDoneResult(result);
    }
    catch(pov_base::Exception& e)
    {
        MakeFailedResult(e, result);
    }
    catch(std::runtime_error& e)
    {
        MakeFailedResult(e.what(), result);
    }
    catch(std::bad_alloc&)
    {
        MakeFailedResult(kOutOfMemoryErr, result);
    }
}

void RenderBackend::StartParser(POVMS_Message& msg, POVMS_Message&, int)
{
    try
    {
        SceneId sid = msg.GetInt(kPOVAttrib_SceneId);

        try
        {
            if(validateFrontendAddress(msg.GetSourceAddress()) == false)
                throw POV_EXCEPTION_CODE(kAuthorisationErr);

            SceneMap::iterator i(scenes.find(sid));

            if(i == scenes.end())
                throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

            i->second->StartParser(msg);
        }
        catch(pov_base::Exception& e)
        {
            SendSceneFailedResult(sid, e, msg.GetSourceAddress());
        }
    }
    catch(pov_base::Exception& e)
    {
        SendFailedResult(e, msg.GetSourceAddress());
    }
    catch(std::runtime_error& e)
    {
        SendFailedResult(e.what(), msg.GetSourceAddress());
    }
    catch(std::bad_alloc&)
    {
        SendFailedResult(kOutOfMemoryErr, msg.GetSourceAddress());
    }
}

void RenderBackend::StopParser(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    try
    {
        SceneId sid = msg.GetInt(kPOVAttrib_SceneId);

        SceneMap::iterator i(scenes.find(sid));

        if(i == scenes.end())
            throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

        if((i->second->IsParsing() == false) && (i->second->IsPaused() == false))
            throw POV_EXCEPTION_CODE(kNotNowErr);

        i->second->StopParser();

        MakeDoneResult(result);
    }
    catch(pov_base::Exception& e)
    {
        MakeFailedResult(e, result);
    }
    catch(std::runtime_error& e)
    {
        MakeFailedResult(e.what(), result);
    }
    catch(std::bad_alloc&)
    {
        MakeFailedResult(kOutOfMemoryErr, result);
    }
}

void RenderBackend::PauseParser(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    try
    {
        SceneId sid = msg.GetInt(kPOVAttrib_SceneId);

        SceneMap::iterator i(scenes.find(sid));

        if(i == scenes.end())
            throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

        if((i->second->IsParsing() == false) && (i->second->IsPaused() == false))
            throw POV_EXCEPTION_CODE(kNotNowErr);

        i->second->PauseParser();

        MakeDoneResult(result);
    }
    catch(pov_base::Exception& e)
    {
        MakeFailedResult(e, result);
    }
    catch(std::runtime_error& e)
    {
        MakeFailedResult(e.what(), result);
    }
    catch(std::bad_alloc&)
    {
        MakeFailedResult(kOutOfMemoryErr, result);
    }
}

void RenderBackend::ResumeParser(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    try
    {
        SceneId sid = msg.GetInt(kPOVAttrib_SceneId);

        SceneMap::iterator i(scenes.find(sid));

        if(i == scenes.end())
            throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

        if((i->second->IsParsing() == false) && (i->second->IsPaused() == false))
            throw POV_EXCEPTION_CODE(kNotNowErr);

        i->second->ResumeParser();

        MakeDoneResult(result);
    }
    catch(pov_base::Exception& e)
    {
        MakeFailedResult(e, result);
    }
    catch(std::runtime_error& e)
    {
        MakeFailedResult(e.what(), result);
    }
    catch(std::bad_alloc&)
    {
        MakeFailedResult(kOutOfMemoryErr, result);
    }
}

void RenderBackend::StartRender(POVMS_Message& msg, POVMS_Message&, int)
{
    try
    {
        ViewId vid = msg.GetInt(kPOVAttrib_ViewId);

        try
        {
            if(validateFrontendAddress(msg.GetSourceAddress()) == false)
                throw POV_EXCEPTION_CODE(kAuthorisationErr);

            ViewMap::iterator i(views.find(vid));

            if(i == views.end())
                throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

            if(i->second->IsRendering() == true)
                throw POV_EXCEPTION_CODE(kNotNowErr);

            i->second->StartRender(msg);
        }
        catch(pov_base::Exception& e)
        {
            SendViewFailedResult(vid, e, msg.GetSourceAddress());
        }
    }
    catch(pov_base::Exception& e)
    {
        SendFailedResult(e, msg.GetSourceAddress());
    }
    catch(std::runtime_error& e)
    {
        SendFailedResult(e.what(), msg.GetSourceAddress());
    }
    catch(std::bad_alloc&)
    {
        SendFailedResult(kOutOfMemoryErr, msg.GetSourceAddress());
    }
}

void RenderBackend::StopRender(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    try
    {
        ViewId vid = msg.GetInt(kPOVAttrib_ViewId);

        ViewMap::iterator i(views.find(vid));

        if(i == views.end())
            throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

        if((i->second->IsRendering() == false) && (i->second->IsPaused() == false))
            throw POV_EXCEPTION_CODE(kNotNowErr);

        i->second->StopRender();

        MakeDoneResult(result);
    }
    catch(pov_base::Exception& e)
    {
        MakeFailedResult(e, result);
    }
    catch(std::runtime_error& e)
    {
        MakeFailedResult(e.what(), result);
    }
    catch(std::bad_alloc&)
    {
        MakeFailedResult(kOutOfMemoryErr, result);
    }
}

void RenderBackend::PauseRender(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    try
    {
        ViewId vid = msg.GetInt(kPOVAttrib_ViewId);

        ViewMap::iterator i(views.find(vid));

        if(i == views.end())
            throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

        if((i->second->IsRendering() == false) && (i->second->IsPaused() == false))
            throw POV_EXCEPTION_CODE(kNotNowErr);

        i->second->PauseRender();

        MakeDoneResult(result);
    }
    catch(pov_base::Exception& e)
    {
        MakeFailedResult(e, result);
    }
    catch(std::runtime_error& e)
    {
        MakeFailedResult(e.what(), result);
    }
    catch(std::bad_alloc&)
    {
        MakeFailedResult(kOutOfMemoryErr, result);
    }
}

void RenderBackend::ResumeRender(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    try
    {
        ViewId vid = msg.GetInt(kPOVAttrib_ViewId);

        ViewMap::iterator i(views.find(vid));

        if(i == views.end())
            throw POV_EXCEPTION_CODE(kInvalidIdentifierErr);

        if((i->second->IsRendering() == false) && (i->second->IsPaused() == false))
            throw POV_EXCEPTION_CODE(kNotNowErr);

        i->second->ResumeRender();

        MakeDoneResult(result);
    }
    catch(pov_base::Exception& e)
    {
        MakeFailedResult(e, result);
    }
    catch(std::runtime_error& e)
    {
        MakeFailedResult(e.what(), result);
    }
    catch(std::bad_alloc&)
    {
        MakeFailedResult(kOutOfMemoryErr, result);
    }
}

void RenderBackend::ReadFile(POVMS_Message& msg, POVMS_Message& result, int)
{
    if(ValidateFrontendAddress(msg.GetSourceAddress(), result) == false)
        return;

    MakeFailedResult(kCannotHandleRequestErr, result);
}

bool RenderBackend::ValidateFrontendAddress(POVMSAddress addr, POVMS_Message& result)
{
    if(validateFrontendAddress(addr) == false)
    {
        MakeFailedResult(kAuthorisationErr, result);

        return false;
    }
    else
        return true;
}

void RenderBackend::MakeFailedResult(int error, POVMS_Message& result)
{
    POVMS_Message newmsg(result, result.GetClass(), kPOVMsgIdent_Failed);
    result = newmsg;
    result.SetInt(kPOVAttrib_ErrorNumber, error);
    result.SetString(kPOVAttrib_EnglishText, pov_base::Exception::lookup_code(error).c_str());
}

void RenderBackend::MakeFailedResult(const pov_base::Exception& e, POVMS_Message& result)
{
    POVMS_Message newmsg(result, result.GetClass(), kPOVMsgIdent_Failed);
    result = newmsg;
    result.SetInt(kPOVAttrib_ErrorNumber, e.code(kCannotHandleRequestErr));
    // pov_base::Exception(...) does a code->string lookup if a string isn't supplied
    result.SetString(kPOVAttrib_EnglishText, e.what());
}

void RenderBackend::MakeFailedResult(const char *str, POVMS_Message& result)
{
    POVMS_Message newmsg(result, result.GetClass(), kPOVMsgIdent_Failed);
    result = newmsg;
    result.SetString(kPOVAttrib_EnglishText, str);
}

void RenderBackend::MakeDoneResult(POVMS_Message& result)
{
    POVMS_Message newmsg(result, result.GetClass(), kPOVMsgIdent_Done);
    result = newmsg;
}

}
// end of namespace pov
