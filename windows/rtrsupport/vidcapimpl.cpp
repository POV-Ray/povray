//******************************************************************************
///
/// @file windows/rtrsupport/vidcapimpl.cpp
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

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#include <windows.h>
#include <tchar.h>

#include "pvengine.h"

#ifdef RTR_SUPPORT

#include "rtrsupport.h"
#include "syspovimage.h"
#include "rtrvidcap.h"

// this must be the last file included
#include "syspovdebug.h"

namespace pov
{

using namespace povwin;
using namespace pov_frontend;

// this code has been stubbed out: need to re-implement

VideoCaptureImpl::VideoCaptureImpl() : m_Capture(NULL)
{
}

VideoCaptureImpl::~VideoCaptureImpl()
{
}

Image *VideoCaptureImpl::Init(const char *params, ImageReadOptions& options, bool doubleBuffer)
{
  return NULL;
}

void VideoCaptureImpl::UpdateImage()
{
}

bool VideoCaptureImpl::WaitFrame(int count, unsigned int timeout)
{
  return false;
}

}
// end of namespace pov

#endif
