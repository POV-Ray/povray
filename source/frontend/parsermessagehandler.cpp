//******************************************************************************
///
/// @file frontend/parsermessagehandler.cpp
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
#include "frontend/parsermessagehandler.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <sstream>
#include <string>

// POV-Ray header files (base module)
// POV-Ray header files (POVMS module)
//  (none at the moment)

// POV-Ray header files (frontend module)
#include "frontend/renderfrontend.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

ParserMessageHandler::ParserMessageHandler()
{
}

ParserMessageHandler::~ParserMessageHandler()
{
}

void ParserMessageHandler::HandleMessage(const SceneData& sd, POVMSType ident, POVMS_Object& obj)
{
    if(ident != kPOVMsgIdent_Progress)
    {
        sd.console->flush();
        if (sd.streams[STATUS_STREAM] != nullptr)
            sd.streams[STATUS_STREAM]->flush();
    }

    if(ident != kPOVMsgIdent_Debug)
    {
        sd.console->flush();
        if (sd.streams[DEBUG_STREAM] != nullptr)
            sd.streams[DEBUG_STREAM]->flush();
    }

    switch(ident)
    {
        case kPOVMsgIdent_ParserOptions:
            Options(sd.console.get(), obj, sd.consoleoutput[RENDER_STREAM]);
            if (sd.streams[RENDER_STREAM].get() != nullptr)
            {
                Message2Console::ParserOptions(obj, sd.streams[RENDER_STREAM].get());
                Message2Console::OutputOptions(obj, sd.streams[RENDER_STREAM].get());
            }
            if (sd.streams[ALL_STREAM].get() != nullptr)
            {
                Message2Console::ParserOptions(obj, sd.streams[ALL_STREAM].get());
                Message2Console::OutputOptions(obj, sd.streams[ALL_STREAM].get());
            }
            break;
        case kPOVMsgIdent_ParserStatistics:
            Statistics(sd.console.get(), obj, sd.consoleoutput[STATISTIC_STREAM]);
            if (sd.streams[STATISTIC_STREAM].get() != nullptr)
            {
                Message2Console::ParserStatistics(obj, sd.streams[STATISTIC_STREAM].get());
                Message2Console::ParserTime(obj, sd.streams[STATISTIC_STREAM].get());
            }
            if (sd.streams[ALL_STREAM].get() != nullptr)
            {
                Message2Console::ParserStatistics(obj, sd.streams[ALL_STREAM].get());
                Message2Console::ParserTime(obj, sd.streams[ALL_STREAM].get());
            }
            break;
        case kPOVMsgIdent_Progress:
            Progress(sd.console.get(), obj, sd.verbose);
//          if (sd.streams[ALL_STREAM].get() != nullptr)
//              Message2Console::Progress(obj, sd.streams[ALL_STREAM].get());
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
        case kPOVMsgIdent_Debug:
            DebugInfo(sd.console.get(), obj, sd.consoleoutput[DEBUG_STREAM]);
            if (sd.streams[DEBUG_STREAM].get() != nullptr)
                Message2Console::DebugInfo(obj, sd.streams[DEBUG_STREAM].get());
            if (sd.streams[ALL_STREAM].get() != nullptr)
                Message2Console::DebugInfo(obj, sd.streams[ALL_STREAM].get());
            break;
    }
}

void ParserMessageHandler::Options(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
    {
        Message2Console::ParserOptions(obj, console);
        Message2Console::OutputOptions(obj, console);
    }
}

void ParserMessageHandler::Statistics(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
    {
        Message2Console::ParserStatistics(obj, console);
        Message2Console::ParserTime(obj, console);
    }
}

void ParserMessageHandler::Progress(Console *console, POVMS_Object& obj, bool verbose)
{
    std::ostringstream sstr;

    switch(obj.GetType(kPOVMSObjectClassID))
    {
        case kPOVObjectClass_ParserProgress:
        {
            sstr << Message2Console::GetProgressTime(obj, kPOVAttrib_RealTime)
                 << " Parsing " << (obj.GetLong(kPOVAttrib_CurrentTokenCount) / (POVMSLong)(1000)) << "K tokens\r";
            break;
        }
        case kPOVObjectClass_BoundingProgress:
        {
            sstr << Message2Console::GetProgressTime(obj, kPOVAttrib_RealTime)
                 << " Bounding " << (obj.GetLong(kPOVAttrib_CurrentNodeCount) / (POVMSLong)(1000)) << "K nodes\r";
            break;
        }
    }

    console->Output(sstr.str());
}

void ParserMessageHandler::Warning(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
        Message2Console::Warning(obj, console);
}

void ParserMessageHandler::Error(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
        Message2Console::Error(obj, console);
}

void ParserMessageHandler::FatalError(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
        Message2Console::FatalError(obj, console);
}

void ParserMessageHandler::DebugInfo(Console *console, POVMS_Object& obj, bool conout)
{
    if(conout == true)
    {
        // TODO FIXME HACK
        std::string str(obj.GetString(kPOVAttrib_EnglishText));
        console->Output(str);
    }
}

}
// end of namespace pov_frontend
