//******************************************************************************
///
/// @file core/scene/scenedata.cpp
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
#include "core/scene/scenedata.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <sstream>

// POV-Ray header files (base module)
#include "base/types.h"
#include "base/version_info.h"
#include "base/image/colourspace.h"

// POV-Ray header files (core module)
#include "core/material/noise.h"
#include "core/material/pattern.h"
#include "core/scene/atmosphere.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

SceneData::SceneData() :
    fog(nullptr),
    rainbow(nullptr),
    skysphere(nullptr),
    functionContextFactory()
{
    atmosphereIOR = 1.0;
    atmosphereDispersion = 0.0;
    backgroundColour = ToTransColour(RGBFTColour(0.0, 0.0, 0.0, 0.0, 1.0));
    ambientLight = MathColour(1.0);

    iridWavelengths = MathColour::DefaultWavelengths();

    languageVersion = POV_RAY_VERSION_INT;
    languageVersionSet = false;
    languageVersionLate = false;
    warningLevel = 10; // all warnings
    legacyCharset = LegacyCharset::kUnspecified;
    noiseGenerator = kNoiseGen_RangeCorrected;
    explicitNoiseGenerator = false; // scene has not set the noise generator explicitly
    boundingMethod = 0;
    numberOfWaves = 10;
    parsedMaxTraceLevel = MAX_TRACE_LEVEL_DEFAULT;
    parsedAdcBailout = 1.0 / 255.0; // adc bailout sufficient for displays
    workingGamma.reset();
    workingGammaToSRGB.reset();
    inputFileGamma = SRGBGammaCurve::Get();
    gammaMode = kPOVList_GammaMode_None; // default setting for v3.6.2, which in turn is the default for the language

    mmPerUnit = 10;
    useSubsurface = false;
    subsurfaceSamplesDiffuse = 50;
    subsurfaceSamplesSingle = 50;
    subsurfaceUseRadiosity = false;

    bspMaxDepth = 0;
    bspObjectIsectCost = bspBaseAccessCost = bspChildAccessCost = bspMissChance = 0.0f;

    Fractal_Iteration_Stack_Length = 0;
    Max_Blob_Components = 1000; // TODO FIXME - this gets set in the parser but allocated *before* that in the scene data, and if it is 0 here, a malloc may fail there because the memory requested is zero [trf]
    Max_Bounding_Cylinders = 100; // TODO FIXME - see note for Max_Blob_Components
    boundingSlabs = nullptr;

    splitUnions = false;
    removeBounds = true;

    tree = nullptr;
}

SceneData::~SceneData()
{
    lightSources.clear();
    lightGroupLightSources.clear();
    Destroy_Skysphere(skysphere);
    while (fog != nullptr)
    {
        FOG *next = fog->Next;
        Destroy_Fog(fog);
        fog = next;
    }
    while (rainbow != nullptr)
    {
        RAINBOW *next = rainbow->Next;
        Destroy_Rainbow(rainbow);
        rainbow = next;
    }
    if (boundingSlabs != nullptr)
        Destroy_BBox_Tree(boundingSlabs);
    for (std::vector<TrueTypeFont*>::iterator i = TTFonts.begin(); i != TTFonts.end(); ++i)
        delete *i;
    // TODO: perhaps ObjectBase::~ObjectBase would be a better place
    //       to handle cleanup of individual objects ?
    Destroy_Object(objects);

    if (tree != nullptr)
        delete tree;
}

}
// end of namespace pov
