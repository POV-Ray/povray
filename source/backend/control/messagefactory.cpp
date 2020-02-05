//******************************************************************************
///
/// @file backend/control/messagefactory.cpp
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
#include "backend/control/messagefactory.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/povassert.h"

// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"
#include "povms/povmsid.h"

// POV-Ray header files (backend module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

MessageFactory::MessageFactory(unsigned int wl, const char *sn, POVMSAddress saddr, POVMSAddress daddr, RenderBackend::SceneId sid, RenderBackend::ViewId vid) :
    GenericMessenger(wl, sn),
    sourceAddress(saddr),
    destinationAddress(daddr),
    sceneId(sid),
    viewId(vid)
{}

MessageFactory::~MessageFactory()
{}

void MessageFactory::SendMessage(MessageClass mc, WarningLevel level, const char *text,
                                 const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset)
{
    POVMSObject msg;
    unsigned int msgIdent;

    (void)POVMSObject_New(&msg, kPOVObjectClass_ControlData);

    if (!filename.empty())
        (void)POVMSUtil_SetUCS2String(&msg, kPOVAttrib_FileName, filename.c_str());
    if (line != -1)
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_Line, line);
    if (column != -1)
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_Column, column);
    if (offset != -1)
        (void)POVMSUtil_SetLong(&msg, kPOVAttrib_FilePosition, offset);

    (void)POVMSUtil_SetString(&msg, kPOVAttrib_EnglishText, text);

    switch(mc)
    {
        case kMessageClass_UserDebug:
            msgIdent = kPOVMsgIdent_Debug;
            break;

        case kMessageClass_InternalDebug:
            POV_ASSERT(false); // not yet implemented
            break;

        case kMessageClass_Info:
        case kMessageClass_Warning:
            msgIdent = kPOVMsgIdent_Warning;
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_Warning, level);
            break;

        case kMessageClass_PossibleError:
            msgIdent = kPOVMsgIdent_Error;
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_Error, 0);
            break;

        case kMessageClass_Error:
            msgIdent = kPOVMsgIdent_FatalError;
            (void)POVMSUtil_SetInt(&msg, kPOVAttrib_Error, 0);
            break;

        default:
            POV_ASSERT(false);
            break;
    }

    if(viewId != 0)
    {
        (void)POVMSUtil_SetInt(&msg, kPOVAttrib_ViewId, viewId);
        (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_ViewOutput, msgIdent);
    }
    else
    {
        (void)POVMSUtil_SetInt(&msg, kPOVAttrib_SceneId, sceneId);
        (void)POVMSMsg_SetupMessage(&msg, kPOVMsgClass_SceneOutput, msgIdent);
    }

    (void)POVMSMsg_SetSourceAddress(&msg, sourceAddress);
    (void)POVMSMsg_SetDestinationAddress(&msg, destinationAddress);

    (void)POVMS_Send(nullptr, &msg, nullptr, kPOVMSSendMode_NoReply);
}

}
// end of namespace pov
