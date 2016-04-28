//******************************************************************************
///
/// @file frontend/animationprocessing.h
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

#ifndef POVRAY_FRONTEND_ANIMATIONPROCESSING_H
#define POVRAY_FRONTEND_ANIMATIONPROCESSING_H

#include <string>

#include "povms/povmscpp.h"
#include "povms/povmsid.h"

#include "base/types.h"

namespace pov_frontend
{

using namespace pov_base;

class AnimationProcessing
{
    public:
        AnimationProcessing(POVMS_Object& options);

        POVMS_Object GetFrameRenderOptions();
        void ComputeNextFrame();
        bool MoreFrames();

        /// Get nominal frame number.
        POVMSInt GetNominalFrameNumber();

        /// Get running frame number.
        POVMSInt GetRunningFrameNumber();

        /// Get subset start frame number.
        POVMSInt GetSubsetStartFrame();

        /// Get subset end frame number.
        POVMSInt GetSubsetEndFrame();

        /// Get the total number of frames to actually render.
        POVMSInt GetTotalFramesToRender();

        POVMSFloat GetClockValue();
        int GetFrameNumberDigits();
    private:
        POVMS_Object renderOptions;

        POVMSInt initialFrame;
        POVMSFloat initialClock;

        POVMSInt finalFrame;
        POVMSFloat finalClock;

        POVMSInt subsetStartFrame;
        POVMSInt subsetEndFrame;

        POVMSBool fieldRenderFlag;
        POVMSBool oddFieldFlag;

        POVMSFloat clockValue;
        POVMSInt runningFrameNumber;
        POVMSInt nominalFrameNumber;

        POVMSInt frameStep;

        double clockDelta;
        int frameNumberDigits;
};

}

#endif // POVRAY_FRONTEND_ANIMATIONPROCESSING_H
