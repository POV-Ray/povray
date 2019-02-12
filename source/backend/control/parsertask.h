//******************************************************************************
///
/// @file backend/control/parsertask.h
///
/// Declarations related to the parser task.
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

#ifndef POVRAY_BACKEND_PARSERTASK_H
#define POVRAY_BACKEND_PARSERTASK_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// POV-Ray header files (base module)
// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (parser module)
#include "parser/parser_fwd.h"
#include "parser/parsertypes.h"

// POV-Ray header files (backend module)
#include "backend/support/task.h"

namespace pov
{

using namespace pov_base;
using namespace pov;

class ParserTask final : public SceneTask, public pov_parser::FileResolver, public pov_parser::ProgressReporter
{
public:

    ParserTask(std::shared_ptr<BackendSceneData> sd, const pov_parser::ParserOptions& opts);

    /// @name @ref SceneTask related.
    /// @{

    virtual void Run() override;
    virtual void Stopped() override;
    virtual void Finish() override;
    void SendFatalError(Exception& e);

    /// @}
    /// @name @ref pov_parser::FileResolver related.
    /// @{

    virtual UCS2String FindFile(UCS2String parsedFileName, unsigned int fileType) override;
    virtual IStream* ReadFile(const UCS2String& parsedFileName, const UCS2String& foundFileName, unsigned int fileType) override;
    virtual OStream* CreateFile(const UCS2String& parsedFileName, unsigned int fileType, bool append) override;

    /// @}
    /// @name @ref pov_parser::ProgressReporter related.
    /// @{

    static constexpr auto kMinProgressReportInterval = 1000;    /// Minimum delay between progress reports (in milliseconds).
    virtual void ReportProgress(POV_LONG tokenCount) override;

    /// @}

private:

    std::unique_ptr<pov_parser::Parser> mpParser;
    std::shared_ptr<BackendSceneData>   mpBackendSceneData;
    pov_parser::ParserOptions           mOptions;
    POV_LONG                            mLastProgressElapsedTime;
};

}
// end of namespace pov

#endif // POVRAY_BACKEND_PARSERTASK_H
