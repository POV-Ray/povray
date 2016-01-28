//******************************************************************************
///
/// @file backend/control/messagefactory.cpp
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#include <boost/thread.hpp>
#include <boost/bind.hpp>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/control/messagefactory.h"

#include "povms/povmscpp.h"
#include "povms/povmsid.h"

#include "base/pov_err.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

MessageFactory::MessageFactory(unsigned int wl, const char *sn, POVMSAddress saddr, POVMSAddress daddr, RenderBackend::SceneId sid, RenderBackend::ViewId vid) :
    warningLevel(wl),
    stageName(sn),
    sourceAddress(saddr),
    destinationAddress(daddr),
    sceneId(sid),
    viewId(vid)
{}

MessageFactory::~MessageFactory()
{}

void MessageFactory::CoreMessage(CoreMessageClass mc, const char *format, ...)
{
    va_list marker;
    POVMSObject msg;
    char localvsbuffer[1024];
    WarningLevel level;
    unsigned int msgClassId;

    switch (mc)
    {
        case kCoreMessageClass_Debug:
            sprintf(localvsbuffer, "%s Debug Info: ", stageName);
            msgClassId = kPOVMsgIdent_Warning;
            level = kWarningGeneral;
            break;
        case kCoreMessageClass_Info:
            sprintf(localvsbuffer, "%s Info: ", stageName);
            msgClassId = kPOVMsgIdent_Warning;
            level = kWarningGeneral;
            break;
        case kCoreMessageClass_Warning:
            if(warningLevel < kWarningGeneral)
                return;
            sprintf(localvsbuffer, "%s Warning: ", stageName);
            msgClassId = kPOVMsgIdent_Warning;
            level = kWarningGeneral;
            break;
        default:
            break;
    }

    if ((msgClassId != kPOVMsgIdent_Warning) || (warningLevel >= level))
    {
        va_start(marker, format);
        vsnprintf(localvsbuffer + strlen(localvsbuffer), 1023 - strlen(localvsbuffer), format, marker);
        va_end(marker);

        CleanupString(localvsbuffer);

        (void)POVMSObject_New(&msg, kPOVObjectClass_ControlData);
        (void)POVMSUtil_SetString(&msg, kPOVAttrib_EnglishText, localvsbuffer);
        if (msgClassId == kPOVMsgIdent_Warning)
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_Warning, level);

        if(viewId != 0)
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_ViewId, viewId);
        else
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_SceneId, sceneId);

        if(viewId != 0)
            (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_ViewOutput, msgClassId);
        else
            (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_SceneOutput, msgClassId);

        (void)POVMSMsg_SetSourceAddress(&msg, sourceAddress);
        (void)POVMSMsg_SetDestinationAddress(&msg, destinationAddress);

        (void)POVMS_Send(NULL, &msg, NULL, kPOVMSSendMode_NoReply);
    }
}

void MessageFactory::CoreMessageAt(CoreMessageClass mc, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...)
{
    va_list marker;
    POVMSObject msg;
    char localvsbuffer[1024];
    WarningLevel level;
    unsigned int msgClassId;

    switch (mc)
    {
        case kCoreMessageClass_Debug:
            sprintf(localvsbuffer, "%s Debug Info: ", stageName);
            msgClassId = kPOVMsgIdent_Warning;
            level = kWarningGeneral;
            break;
        case kCoreMessageClass_Info:
            sprintf(localvsbuffer, "%s Info: ", stageName);
            msgClassId = kPOVMsgIdent_Warning;
            level = kWarningGeneral;
            break;
        case kCoreMessageClass_Warning:
            if(warningLevel < kWarningGeneral)
                return;
            sprintf(localvsbuffer, "%s Warning: ", stageName);
            msgClassId = kPOVMsgIdent_Warning;
            level = kWarningGeneral;
            break;
        default:
            break;
    }

    if ((msgClassId != kPOVMsgIdent_Warning) || (warningLevel >= level))
    {
        va_start(marker, format);
        vsnprintf(localvsbuffer + strlen(localvsbuffer), 1023 - strlen(localvsbuffer), format, marker);
        va_end(marker);

        CleanupString(localvsbuffer);

        (void)POVMSObject_New(&msg, kPOVObjectClass_ControlData);
        (void)POVMSUtil_SetUCS2String(&msg, kPOVAttrib_FileName, filename);
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_Line, line);
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_Column, column);
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_FilePosition, offset);

        (void)POVMSUtil_SetString(&msg, kPOVAttrib_EnglishText, localvsbuffer);
        if (msgClassId == kPOVMsgIdent_Warning)
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_Warning, level);

        if(viewId != 0)
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_ViewId, viewId);
        else
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_SceneId, sceneId);

        if(viewId != 0)
            (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_ViewOutput, msgClassId);
        else
            (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_SceneOutput, msgClassId);

        (void)POVMSMsg_SetSourceAddress(&msg, sourceAddress);
        (void)POVMSMsg_SetDestinationAddress(&msg, destinationAddress);

        (void)POVMS_Send(NULL, &msg, NULL, kPOVMSSendMode_NoReply);
    }
}

void MessageFactory::Warning(WarningLevel level, const char *format,...)
{
    if(warningLevel >= level)
    {
        va_list marker;
        POVMSObject msg;
        char localvsbuffer[1024];

        sprintf(localvsbuffer, "%s Warning: ", stageName);

        va_start(marker, format);
        vsnprintf(localvsbuffer + strlen(localvsbuffer), 1023 - strlen(localvsbuffer), format, marker);
        va_end(marker);

        CleanupString(localvsbuffer);

        (void)POVMSObject_New(&msg, kPOVObjectClass_ControlData);
        (void)POVMSUtil_SetString(&msg, kPOVAttrib_EnglishText, localvsbuffer);
        (void)POVMSUtil_SetInt(&msg, kPOVAttrib_Warning, level);

        if(viewId != 0)
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_ViewId, viewId);
        else
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_SceneId, sceneId);

        if(viewId != 0)
            (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_ViewOutput, kPOVMsgIdent_Warning);
        else
            (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_SceneOutput, kPOVMsgIdent_Warning);

        (void)POVMSMsg_SetSourceAddress(&msg, sourceAddress);
        (void)POVMSMsg_SetDestinationAddress(&msg, destinationAddress);

        (void)POVMS_Send(NULL, &msg, NULL, kPOVMSSendMode_NoReply);
    }
}

void MessageFactory::WarningAt(WarningLevel level, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...)
{
    if(warningLevel >= level)
    {
        va_list marker;
        POVMSObject msg;
        char localvsbuffer[1024];

        sprintf(localvsbuffer, "%s Warning: ", stageName);

        va_start(marker, format);
        vsnprintf(localvsbuffer + strlen(localvsbuffer), 1023 - strlen(localvsbuffer), format, marker);
        va_end(marker);

        CleanupString(localvsbuffer);

        (void)POVMSObject_New(&msg, kPOVObjectClass_ControlData);
        (void)POVMSUtil_SetUCS2String(&msg, kPOVAttrib_FileName, filename);
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_Line, line);
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_Column, column);
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_FilePosition, offset);

        (void)POVMSUtil_SetString(&msg, kPOVAttrib_EnglishText, localvsbuffer);
        (void)POVMSUtil_SetInt(&msg, kPOVAttrib_Warning, level);

        if(viewId != 0)
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_ViewId, viewId);
        else
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_SceneId, sceneId);

        if(viewId != 0)
            (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_ViewOutput, kPOVMsgIdent_Warning);
        else
            (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_SceneOutput, kPOVMsgIdent_Warning);

        (void)POVMSMsg_SetSourceAddress(&msg, sourceAddress);
        (void)POVMSMsg_SetDestinationAddress(&msg, destinationAddress);

        (void)POVMS_Send(NULL, &msg, NULL, kPOVMSSendMode_NoReply);
    }
}

void MessageFactory::PossibleError(const char *format,...)
{
    va_list marker;
    POVMSObject msg;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "Possible %s Error: ", stageName);

    va_start(marker, format);
    vsnprintf(localvsbuffer + strlen(localvsbuffer), 1023 - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    if(warningLevel == 0)
        return;

    (void)POVMSObject_New(&msg, kPOVObjectClass_ControlData);
    (void)POVMSUtil_SetString(&msg, kPOVAttrib_EnglishText, localvsbuffer);
    (void)POVMSUtil_SetInt(&msg, kPOVAttrib_Error, 0);

    if(viewId != 0)
        (void)POVMSUtil_SetInt(&msg, kPOVAttrib_ViewId, viewId);
    else
        (void)POVMSUtil_SetInt(&msg, kPOVAttrib_SceneId, sceneId);

    if(viewId != 0)
        (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_ViewOutput, kPOVMsgIdent_Error);
    else
        (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_SceneOutput, kPOVMsgIdent_Error);

    (void)POVMSMsg_SetSourceAddress(&msg, sourceAddress);
    (void)POVMSMsg_SetDestinationAddress(&msg, destinationAddress);

    (void)POVMS_Send(NULL, &msg, NULL, kPOVMSSendMode_NoReply);
}

void MessageFactory::PossibleErrorAt(const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...)
{
    va_list marker;
    POVMSObject msg;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "Possible %s Error: ", stageName);

    va_start(marker, format);
    vsnprintf(localvsbuffer + strlen(localvsbuffer), 1023 - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    if(warningLevel == 0)
        return;

    (void)POVMSObject_New(&msg, kPOVObjectClass_ControlData);
    (void)POVMSUtil_SetUCS2String(&msg, kPOVAttrib_FileName, filename);
    (void)POVMSUtil_SetLong(&msg, kPOVAttrib_Line, line);
    (void)POVMSUtil_SetLong(&msg, kPOVAttrib_Column, column);
    (void)POVMSUtil_SetLong(&msg, kPOVAttrib_FilePosition, offset);

    (void)POVMSUtil_SetString(&msg, kPOVAttrib_EnglishText, localvsbuffer);
    (void)POVMSUtil_SetInt(&msg, kPOVAttrib_Error, 0);

    if(viewId != 0)
        (void)POVMSUtil_SetInt(&msg, kPOVAttrib_ViewId, viewId);
    else
        (void)POVMSUtil_SetInt(&msg, kPOVAttrib_SceneId, sceneId);

    if(viewId != 0)
        (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_ViewOutput, kPOVMsgIdent_Error);
    else
        (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_SceneOutput, kPOVMsgIdent_Error);

    (void)POVMSMsg_SetSourceAddress(&msg, sourceAddress);
    (void)POVMSMsg_SetDestinationAddress(&msg, destinationAddress);

    (void)POVMS_Send(NULL, &msg, NULL, kPOVMSSendMode_NoReply);
}

// filename defaults to NULL, and line, column, and offset default to -1
std::string MessageFactory::SendError(const char *format, va_list arglist, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset)
{
    POVMSObject msg;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "%s Error: ", stageName);
    vsnprintf(localvsbuffer + strlen(localvsbuffer), 1023 - strlen(localvsbuffer), format, arglist);
    CleanupString(localvsbuffer);

    (void)POVMSObject_New(&msg, kPOVObjectClass_ControlData);
    if (filename != NULL)
        (void)POVMSUtil_SetUCS2String(&msg, kPOVAttrib_FileName, filename);
    if (line != -1)
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_Line, line);
    if (column != -1)
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_Column, column);
    if (offset != -1)
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_FilePosition, offset);
    (void)POVMSUtil_SetString(&msg, kPOVAttrib_EnglishText, localvsbuffer);
    (void)POVMSUtil_SetInt(&msg, kPOVAttrib_Error, 0);

    if(viewId != 0)
    {
        (void)POVMSUtil_SetInt(&msg, kPOVAttrib_ViewId, viewId);
        (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_ViewOutput, kPOVMsgIdent_FatalError);
    }
    else
    {
        (void)POVMSUtil_SetInt(&msg, kPOVAttrib_SceneId, sceneId);
        (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_SceneOutput, kPOVMsgIdent_FatalError);
    }

    (void)POVMSMsg_SetSourceAddress(&msg, sourceAddress);
    (void)POVMSMsg_SetDestinationAddress(&msg, destinationAddress);

    (void)POVMS_Send(NULL, &msg, NULL, kPOVMSSendMode_NoReply);

    return std::string(localvsbuffer);
}

void MessageFactory::Error(const char *format, ...)
{
    va_list marker;

    va_start(marker, format);
    std::string text = SendError(format, marker);
    va_end(marker);

    // Terminate by throwing an exception with the notification flag already set
    pov_base::Exception ex(__FUNCTION__, __FILE__, __LINE__, text);
    ex.frontendnotified(true);
    throw ex;
}

void MessageFactory::Error(const Exception& ex, const char *format, ...)
{
    va_list marker;

    // if the front-end has been told about this exception already, we don't want to tell it again
    // (we presume that the text given by format and its parameters relate to that exception)
    // this form of frontendnotified() doesn't change the state of ex
    if (ex.frontendnotified())
        throw ex;

    va_start(marker, format);
    SendError(format, marker);
    va_end(marker);

    // now take a copy of ex and set its notification flag, then throw that.
    pov_base::Exception local_ex(ex);
    local_ex.frontendnotified(true);
    throw local_ex;
}

void MessageFactory::Error(Exception& ex, const char *format, ...)
{
    va_list marker;

    // if the front-end has been told about this exception already, we don't want to tell it again
    // (we presume that the text given by format and its parameters relate to that exception)
    // this form of frontendnotified() sets the notified state of ex
    if (ex.frontendnotified(true))
        throw ex;

    va_start(marker, format);
    SendError(format, marker);
    va_end(marker);

    // Terminate
    throw ex;
}

void MessageFactory::ErrorAt(const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...)
{
    va_list marker;

    va_start(marker, format);
    std::string text = SendError(format, marker, filename, line, column, offset);
    va_end(marker);

    // Terminate by throwing an exception with the notification flag already set
    pov_base::Exception ex(__FUNCTION__, __FILE__, __LINE__, text);
    ex.frontendnotified(true);

    throw ex;
}

void MessageFactory::ErrorAt(const Exception& ex, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...)
{
    va_list marker;

    if (ex.frontendnotified())
        throw ex;

    va_start(marker, format);
    SendError(format, marker, filename, line, column, offset);
    va_end(marker);

    pov_base::Exception local_ex(ex);
    local_ex.frontendnotified(true);
    throw local_ex;
}

void MessageFactory::ErrorAt(Exception& ex, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...)
{
    va_list marker;

    if (ex.frontendnotified(true))
        throw ex;

    va_start(marker, format);
    SendError(format, marker, filename, line, column, offset);
    va_end(marker);

    throw ex;
}

void MessageFactory::CleanupString(char *str)
{
    while(*str != 0)
    {
        if(*str == '\n')
            *str = ' ';
        str++;
    }
}

}
