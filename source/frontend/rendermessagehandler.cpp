//******************************************************************************
///
/// @file frontend/rendermessagehandler.cpp
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
#include "frontend/rendermessagehandler.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <sstream>

// POV-Ray header files (base module)
// POV-Ray header files (POVMS module)
//  (none at the moment)

// POV-Ray header files (frontend module)
#include "frontend/renderfrontend.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

RenderMessageHandler::RenderMessageHandler()
{
}

RenderMessageHandler::~RenderMessageHandler()
{
}

void RenderMessageHandler::HandleMessage(const SceneData& sd, const ViewData&, POVMSType ident, POVMS_Object& obj)
{
    if(ident != kPOVMsgIdent_Progress)
    {
        sd.console->flush();
        if (sd.streams[STATUS_STREAM] != nullptr)
            sd.streams[STATUS_STREAM]->flush();
    }

    switch(ident)
    {
        case kPOVMsgIdent_RenderOptions:
            Options(sd.console.get(), obj, sd.consoleoutput[RENDER_STREAM]);
            if (sd.streams[RENDER_STREAM].get() != nullptr)
                Message2Console::RenderOptions(obj, sd.streams[RENDER_STREAM].get());
            if (sd.streams[ALL_STREAM].get() != nullptr)
                Message2Console::RenderOptions(obj, sd.streams[ALL_STREAM].get());
            break;
        case kPOVMsgIdent_RenderStatistics:
            Statistics(sd.console.get(), obj, sd.consoleoutput[STATISTIC_STREAM]);
            if (sd.streams[STATISTIC_STREAM].get() != nullptr)
            {
                Message2Console::RenderStatistics(obj, sd.streams[STATISTIC_STREAM].get());
                Message2Console::RenderTime(obj, sd.streams[STATISTIC_STREAM].get());
            }
            if (sd.streams[ALL_STREAM].get() != nullptr)
            {
                Message2Console::RenderStatistics(obj, sd.streams[ALL_STREAM].get());
                Message2Console::RenderTime(obj, sd.streams[ALL_STREAM].get());
            }
            break;
        case kPOVMsgIdent_Progress:
            Progress(sd.console.get(), obj, sd.verbose);
//          if (sd.streams[ALL_STREAM].get() != nullptr)
//              Message2Console::Progress(obj, sd.streams[ALL_STREAM]);
            break;
        case kPOVMsgIdent_Warning:
            Warning(sd.console.get(), obj, sd.consoleoutput[WARNING_STREAM]);
            if (sd.streams[WARNING_STREAM].get() != nullptr)
                Message2Console::Warning(obj, sd.streams[WARNING_STREAM].get());
            if (sd.streams[ALL_STREAM].get() != nullptr)
                Message2Console::Warning(obj, sd.streams[ALL_STREAM].get());
            break;
        case kPOVMsgIdent_Error:
            Error(sd.console.get(), obj, sd.consoleoutput[WARNING_STREAM]);
            if (sd.streams[WARNING_STREAM].get() != nullptr)
                Message2Console::Error(obj, sd.streams[WARNING_STREAM].get());
            if (sd.streams[ALL_STREAM].get() != nullptr)
                Message2Console::Error(obj, sd.streams[ALL_STREAM].get());
            break;
        case kPOVMsgIdent_FatalError:
            FatalError(sd.console.get(), obj, sd.consoleoutput[FATAL_STREAM]);
            if (sd.streams[FATAL_STREAM].get() != nullptr)
                Message2Console::FatalError(obj, sd.streams[FATAL_STREAM].get());
            if (sd.streams[ALL_STREAM].get() != nullptr)
                Message2Console::FatalError(obj, sd.streams[ALL_STREAM].get());
            break;
    }
}

void RenderMessageHandler::Options(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
        Message2Console::RenderOptions(obj, console);
}

void RenderMessageHandler::Statistics(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
    {
        Message2Console::RenderStatistics(obj, console);
        Message2Console::RenderTime(obj, console);
    }
}

void RenderMessageHandler::Progress(Console *console, POVMS_Object& obj, bool verbose)
{
    std::ostringstream sstr;

    switch(obj.GetType(kPOVMSObjectClassID))
    {
        case kPOVObjectClass_PhotonProgress:
        {
            int cpc(obj.GetInt(kPOVAttrib_CurrentPhotonCount));

            sstr << Message2Console::GetProgressTime(obj, kPOVAttrib_RealTime)
                 << " Photons " << cpc << "    \r";
            break;
        }
        case kPOVObjectClass_RadiosityProgress:
        {
            int pt(obj.GetInt(kPOVAttrib_Pixels));
            int pc(obj.GetInt(kPOVAttrib_PixelsCompleted));
            int percent = 0;

            if(pt > 0)
                percent = (pc * 100) / pt;

            sstr << Message2Console::GetProgressTime(obj, kPOVAttrib_RealTime)
                 << " Radiosity pretrace completed " << pc << " of " << pt << " pixels (" << percent << "%)    \r";
            break;
        }
        case kPOVObjectClass_RenderProgress:
        {
            int pt(obj.GetInt(kPOVAttrib_Pixels));
            int pp(obj.GetInt(kPOVAttrib_PixelsPending));
            int pc(obj.GetInt(kPOVAttrib_PixelsCompleted));
            int percent = 0;

            if(pt > 0)
                percent = (pc * 100) / pt;

            sstr << Message2Console::GetProgressTime(obj, kPOVAttrib_RealTime)
                 << "Rendering completed " << pc << " of " << pt << " pixels (" << percent << "%) and " << pp << " pixels pending    \r";
            break;
        }
    }

    console->Output(sstr.str());
}

void RenderMessageHandler::Warning(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
        Message2Console::Warning(obj, console);
}

void RenderMessageHandler::Error(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
        Message2Console::Error(obj, console);
}

void RenderMessageHandler::FatalError(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
        Message2Console::FatalError(obj, console);
}

}
// end of namespace pov_frontend
