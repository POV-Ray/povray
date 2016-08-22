//******************************************************************************
///
/// @file core/scene/scenedata.cpp
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/scene/scenedata.h"

#include <sstream>

#include <boost/bind.hpp>

#include "base/version.h"

#include "core/material/pattern.h"
#include "core/scene/atmosphere.h"

#include "vm/fnpovfpu.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

SceneData::SceneData() :
    fog(NULL),
    rainbow(NULL),
    skysphere(NULL),
    functionContextFactory(new FunctionVM())
{
    atmosphereIOR = 1.0;
    atmosphereDispersion = 0.0;
    backgroundTrans  = FilterTransm(0.0, 1.0);
    ambientLight = ColourModelRGB::Whitepoint::Colour;
    generalWhitepoint = ColourModelRGB::Whitepoint::Colour;

    iridWavelengths = PseudoColour::DominantWavelengths;

    languageVersion = OFFICIAL_VERSION_NUMBER;
    languageVersionSet = false;
    languageVersionLate = false;
    warningLevel = 10; // all warnings
    stringEncoding = kStringEncoding_ASCII;
    noiseGenerator = kNoiseGen_RangeCorrected;
    explicitNoiseGenerator = false; // scene has not set the noise generator explicitly
    numberOfWaves = 10;
    parsedMaxTraceLevel = MAX_TRACE_LEVEL_DEFAULT;
    parsedAdcBailout = 1.0 / 255.0; // adc bailout sufficient for displays
    workingGamma.reset();
    workingGammaToSRGB.reset();
    inputFileGammaSet = false; // TODO remove for 3.7x
    inputFileGamma = SRGBGammaCurve::Get();

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
    boundingSlabs = NULL;

    splitUnions = false;
    removeBounds = true;

    tree = NULL;
}

SceneData::~SceneData()
{
    lightSources.clear();
    lightGroupLightSources.clear();
    Destroy_Skysphere(skysphere);
    while (fog != NULL)
    {
        FOG *next = fog->Next;
        Destroy_Fog(fog);
        fog = next;
    }
    while (rainbow != NULL)
    {
        RAINBOW *next = rainbow->Next;
        Destroy_Rainbow(rainbow);
        rainbow = next;
    }
    if(boundingSlabs != NULL)
        Destroy_BBox_Tree(boundingSlabs);
    for (vector<TrueTypeFont*>::iterator i = TTFonts.begin(); i != TTFonts.end(); ++i)
        delete *i;
    // TODO: perhaps ObjectBase::~ObjectBase would be a better place
    //       to handle cleanup of individual objects ?
    Destroy_Object(objects);
    delete functionContextFactory;

    if(tree != NULL)
        delete tree;
}

}
