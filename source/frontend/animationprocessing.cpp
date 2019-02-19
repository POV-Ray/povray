//******************************************************************************
///
/// @file frontend/animationprocessing.cpp
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
#include "frontend/animationprocessing.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmsid.h"

// POV-Ray header files (frontend module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

AnimationProcessing::AnimationProcessing(POVMS_Object& options) :
    renderOptions(options)
{
    bool cyclic = renderOptions.TryGetBool(kPOVAttrib_CyclicAnimation, false);

    initialFrame = std::max(renderOptions.TryGetInt(kPOVAttrib_InitialFrame, 1), 0);
    initialClock = renderOptions.TryGetFloat(kPOVAttrib_InitialClock, 0.0);

    finalFrame = std::max(renderOptions.TryGetInt(kPOVAttrib_FinalFrame, 1), initialFrame);
    finalClock = renderOptions.TryGetFloat(kPOVAttrib_FinalClock, 1.0);

    frameStep = std::max(renderOptions.TryGetInt(kPOVAttrib_FrameStep, 1), 1);

    if(cyclic == true)
        finalFrame++;

    if(renderOptions.Exist(kPOVAttrib_SubsetStartFrame) == true)
    {
        POVMSFloat subsetStartPercent = std::max(renderOptions.GetFloat(kPOVAttrib_SubsetStartFrame), POVMSFloat(0.0));
        if((subsetStartPercent == 0.0) || (subsetStartPercent >= 1.0))
            subsetStartFrame = POVMSInt(subsetStartPercent);
        else
            subsetStartFrame = POVMSFloat((finalFrame - initialFrame + 1) * subsetStartPercent);
        if (subsetStartFrame < initialFrame)
            subsetStartFrame = initialFrame; // TODO: should we be issuing a warning or throwing an error here?
    }
    else
        subsetStartFrame = initialFrame;

    if(options.Exist(kPOVAttrib_SubsetEndFrame) == true)
    {
        POVMSFloat subsetEndPercent = std::max(renderOptions.GetFloat(kPOVAttrib_SubsetEndFrame), POVMSFloat(0.0));
        if((subsetEndPercent == 0.0) || (subsetEndPercent >= 1.0))
            subsetEndFrame = POVMSInt(subsetEndPercent);
        else
            subsetEndFrame = POVMSFloat((finalFrame - initialFrame + 1) * subsetEndPercent);
        if (subsetEndFrame < subsetStartFrame)
            subsetEndFrame = subsetStartFrame; // TODO: should we be issuing a warning or throwing an error here?
        if (subsetEndFrame > finalFrame)
            subsetEndFrame = finalFrame;
    }
    else
    {
        subsetEndFrame = finalFrame;
        if(cyclic == true)
            subsetEndFrame--;
    }

    fieldRenderFlag = renderOptions.TryGetBool(kPOVAttrib_FieldRender, false);
    oddFieldFlag = renderOptions.TryGetBool(kPOVAttrib_OddField, false);

    if (finalFrame == initialFrame)
    {
        // TODO - review the choice of `clockDelta` and `clockValue` for this case,
        // or make sure animation is never triggered in the first place when finalFrame and
        // initialFrame are set to the same value
        clockDelta = 1.0;
        runningFrameNumber = 1;
        nominalFrameNumber = subsetStartFrame;
        clockValue = initialClock;
    }
    else
    {
        clockDelta = double(finalClock - initialClock) / double(finalFrame - initialFrame);
        runningFrameNumber = 1;
        nominalFrameNumber = subsetStartFrame;
        clockValue = POVMSFloat(double(clockDelta * double(subsetStartFrame - initialFrame)) + double(initialClock));
    }

    frameNumberDigits = 1;
    for(POVMSInt i = finalFrame; i >= 10; i /= 10)
        frameNumberDigits++;
}

POVMS_Object AnimationProcessing::GetFrameRenderOptions()
{
    POVMS_Object opts(renderOptions);

    opts.SetFloat(kPOVAttrib_Clock, clockValue);

    // append to console files if not first frame (user can set this for first frame via command line to append all data to existing files, so don't set it to false)
    if(nominalFrameNumber > subsetStartFrame)
        opts.SetBool(kPOVAttrib_AppendConsoleFiles, true);

    POVMS_List declares;
    if(opts.Exist(kPOVAttrib_Declare) == true)
        opts.Get(kPOVAttrib_Declare, declares);

    POVMS_Object clock_delta(kPOVMSType_WildCard);
    clock_delta.SetString(kPOVAttrib_Identifier, "clock_delta");
    clock_delta.SetFloat(kPOVAttrib_Value, POVMSFloat(clockDelta));
    declares.Append(clock_delta);

    POVMS_Object final_clock(kPOVMSType_WildCard);
    final_clock.SetString(kPOVAttrib_Identifier, "final_clock");
    final_clock.SetFloat(kPOVAttrib_Value, finalClock);
    declares.Append(final_clock);

    POVMS_Object final_frame(kPOVMSType_WildCard);
    final_frame.SetString(kPOVAttrib_Identifier, "final_frame");
    final_frame.SetFloat(kPOVAttrib_Value, finalFrame);
    declares.Append(final_frame);

    POVMS_Object frame_number(kPOVMSType_WildCard);
    frame_number.SetString(kPOVAttrib_Identifier, "frame_number");
    frame_number.SetFloat(kPOVAttrib_Value, nominalFrameNumber);
    declares.Append(frame_number);

    POVMS_Object initial_clock(kPOVMSType_WildCard);
    initial_clock.SetString(kPOVAttrib_Identifier, "initial_clock");
    initial_clock.SetFloat(kPOVAttrib_Value, initialClock);
    declares.Append(initial_clock);

    POVMS_Object initial_frame(kPOVMSType_WildCard);
    initial_frame.SetString(kPOVAttrib_Identifier, "initial_frame");
    initial_frame.SetFloat(kPOVAttrib_Value, initialFrame);
    declares.Append(initial_frame);

    //frameStep not provided on purpose: it should remains invisible to the SDL
    //If a scene really needed it, it would be better tweaking the frame-range & clock

    opts.Set(kPOVAttrib_Declare, declares);

    return opts;
}

void AnimationProcessing::ComputeNextFrame()
{
    nominalFrameNumber+= frameStep;
    ++runningFrameNumber;
    clockValue = POVMSFloat(double(clockDelta * double(nominalFrameNumber - initialFrame)) + double(initialClock));
}

bool AnimationProcessing::MoreFrames()
{
    return (nominalFrameNumber+frameStep <= subsetEndFrame);
}

POVMSInt AnimationProcessing::GetNominalFrameNumber()
{
    return nominalFrameNumber;
}

POVMSInt AnimationProcessing::GetRunningFrameNumber()
{
    return runningFrameNumber;
}

POVMSInt AnimationProcessing::GetSubsetStartFrame()
{
    return subsetStartFrame;
}

POVMSInt AnimationProcessing::GetSubsetEndFrame()
{
    return subsetEndFrame;
}

POVMSInt AnimationProcessing::GetTotalFramesToRender()
{
    return (subsetEndFrame - subsetStartFrame) / frameStep + 1;
}

POVMSFloat AnimationProcessing::GetClockValue()
{
    return clockValue;
}

int AnimationProcessing::GetFrameNumberDigits()
{
    return frameNumberDigits;
}

}
// end of namespace pov_frontend
