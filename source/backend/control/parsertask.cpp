//******************************************************************************
///
/// @file backend/control/parsertask.cpp
///
/// This module implements the parser task in a multi-threaded architecture.
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
#include "backend/control/parsertask.h"

// C++ variants of C standard header files
// C++ standard header files

// Boost header files
#include <boost/bind.hpp>

// POV-Ray header files (base module)
#include "base/types.h"

// POV-Ray header files (core module)
#include "core/scene/tracethreaddata.h"

// POV-Ray header files (parser module)
#include "parser/parser.h"

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"
#include "povms/povmsid.h"

// POV-Ray header files (backend module)
#include "backend/control/messagefactory.h"
#include "backend/scene/backendscenedata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using namespace pov_parser;

ParserTask::ParserTask(std::shared_ptr<BackendSceneData> sd, const ParserOptions& opts) :
    SceneTask(new TraceThreadData(sd, opts.randomSeed), boost::bind(&ParserTask::SendFatalError, this, _1), "Parse", sd),
    mpParser(nullptr),
    mpBackendSceneData(sd),
    mOptions(opts),
    mLastProgressElapsedTime(0)
{}

void ParserTask::Run()
{
    mpParser.reset(new Parser(mpBackendSceneData, mOptions, *mpMessageFactory, *this, *this, *reinterpret_cast<TraceThreadData *>(GetDataPtr())));
    mpParser->Run();
}

void ParserTask::Stopped()
{
    mpParser.reset();
    RenderBackend::SendSceneFailedResult(mpBackendSceneData->sceneId, kUserAbortErr, mpBackendSceneData->frontendAddress);
}

void ParserTask::Finish()
{
    if (mpParser != nullptr)
    {
        mpParser->GetParserDataPtr()->timeType  = TraceThreadData::kParseTime;
        mpParser->GetParserDataPtr()->realTime  = ConsumedRealTime();
        mpParser->GetParserDataPtr()->cpuTime   = ConsumedCPUTime();
        mpParser->Finish();
        mpParser.reset();
    }
}

void ParserTask::SendFatalError(Exception& e)
{
    if (mpParser != nullptr)
        mpParser->SendFatalError(e);
}

UCS2String ParserTask::FindFile(UCS2String parsedFileName, unsigned int fileType)
{
    return mpBackendSceneData->FindFile(GetPOVMSContext(), parsedFileName, fileType);
}

IStream* ParserTask::ReadFile(const UCS2String& parsedFileName, const UCS2String& foundFileName, unsigned int fileType)
{
    return mpBackendSceneData->ReadFile(GetPOVMSContext(), parsedFileName, foundFileName, fileType);
}

OStream* ParserTask::CreateFile(const UCS2String& parsedFileName, unsigned int fileType, bool append)
{
    return mpBackendSceneData->CreateFile(GetPOVMSContext(), parsedFileName, fileType, append);
}

void ParserTask::ReportProgress(POV_LONG tokenCount)
{
    POV_LONG elapsedTime = ElapsedRealTime();
    if ((elapsedTime - mLastProgressElapsedTime) > kMinProgressReportInterval)
    {
        POVMS_Object obj(kPOVObjectClass_ParserProgress);
        obj.SetLong(kPOVAttrib_RealTime, elapsedTime);
        obj.SetLong(kPOVAttrib_CurrentTokenCount, tokenCount);
        RenderBackend::SendSceneOutput(mpBackendSceneData->sceneId, mpBackendSceneData->frontendAddress, kPOVMsgIdent_Progress, obj);
        Cooperate();
        mLastProgressElapsedTime = ElapsedRealTime();
    }
}

}
// end of namespace pov
