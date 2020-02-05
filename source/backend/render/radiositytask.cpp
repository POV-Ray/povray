//******************************************************************************
///
/// @file backend/render/radiositytask.cpp
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
#include "backend/render/radiositytask.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>
#include <vector>

// POV-Ray header files (base module)
#include "base/timer.h"
#include "base/types.h"

// POV-Ray header files (core module)
#include "core/support/statistics.h"

// POV-Ray header files (POVMS module)
//  (none at the moment)

// POV-Ray header files (backend module)
#include "backend/scene/backendscenedata.h"
#include "backend/scene/view.h"
#include "backend/scene/viewthreaddata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using namespace pov_base;

using std::min;
using std::max;

RadiosityTask::RadiosityTask(ViewData *vd, DBL ptsz, DBL ptesz, unsigned int pts, unsigned int ptsc, unsigned int nt,
                             size_t seed) :
    RenderTask(vd, seed, "Radiosity", vd->GetViewId()),
    trace(vd->GetSceneData(), &vd->GetCamera(), GetViewDataPtr(), vd->GetSceneData()->parsedMaxTraceLevel, vd->GetSceneData()->parsedAdcBailout,
          vd->GetQualityFeatureFlags(), cooperate, media, radiosity, !vd->GetSceneData()->radiositySettings.vainPretrace),
    cooperate(*this),
    media(GetViewDataPtr(), &trace, &photonGatherer),
    radiosity(vd->GetSceneData(), GetViewDataPtr(),
              vd->GetSceneData()->radiositySettings, vd->GetRadiosityCache(), cooperate, false, vd->GetCamera().Location),
    photonGatherer(&vd->GetSceneData()->surfacePhotonMap, vd->GetSceneData()->photonSettings),
    pretraceStep(pts),
    pretraceStepCount(ptsc),
    pretraceStartSize(ptsz),
    pretraceEndSize(ptesz),
    pretraceCoverage(vd->GetSceneData()->radiositySettings.nearestCountAPT),
    nominalThreads(nt)
{
}

RadiosityTask::~RadiosityTask()
{
}

void RadiosityTask::Run()
{
    //RandomIntSequence rands(0, 2, 2047);
    //RandomIntSequence::Generator randgen(&rands);
    RandomDoubleSequence rands(-1.0, 1.0, 2047);
    RandomDoubleSequence::Generator randgen(&rands);

    DBL width = GetViewData()->GetWidth();
    DBL height = GetViewData()->GetHeight();

    POVRect rect;
    std::vector<Vector2d> pixelpositions;
    std::vector<RGBTColour> pixelcolors;
    unsigned int serial;

    float progressWeightTotal = (pow(4.0f, (float)pretraceStepCount) - 1.0) / 3.0; // equal to SUM[i=0,N-1](pow(4.0, i))

    ViewData::BlockInfo* pInfo;

    while(GetViewData()->GetNextRectangle(rect, serial, pInfo, nominalThreads) == true)
    {
        RadiosityBlockInfo* pBlockInfo = dynamic_cast<RadiosityBlockInfo*>(pInfo);
        if (!pBlockInfo)
        {
            if (pInfo)
            {
                delete pInfo;
                pInfo = nullptr;
            }
            pBlockInfo = new RadiosityBlockInfo();
        }

        unsigned int currentStep = pretraceStep + pBlockInfo->pass;
        double pretraceSize     = max(pretraceStartSize * pow(0.5f, (float)pBlockInfo->pass),     pretraceEndSize);
        double nextPretraceSize = max(pretraceStartSize * pow(0.5f, (float)pBlockInfo->pass + 1), pretraceEndSize);

        radiosity.BeforeTile((nominalThreads? serial % nominalThreads : 0), pretraceStep + pBlockInfo->pass);
        randgen.SetSeed((pretraceStep + pBlockInfo->pass) * 17 + serial * 13); // make sure our jitter is different (but reproducible) for each pass and tile

        unsigned int px = (rect.GetWidth()  + pretraceSize - 1) / pretraceSize;
        unsigned int py = (rect.GetHeight() + pretraceSize - 1) / pretraceSize;

        unsigned int nextPx = (rect.GetWidth()  + nextPretraceSize - 1) / nextPretraceSize;
        unsigned int nextPy = (rect.GetHeight() + nextPretraceSize - 1) / nextPretraceSize;

        double startX   = ceil((DBL(rect.left)  - 0.5) / pretraceSize) * pretraceSize;
        double startY   = ceil((DBL(rect.top)   - 0.5) / pretraceSize) * pretraceSize;
        double endX     = DBL(rect.right)       + 0.5;
        double endY     = DBL(rect.bottom)      + 0.5;

        // make sure we start with the pixel buffer cleared
        pixelpositions.clear();
        pixelpositions.reserve(px * py);
        pixelcolors.clear();
        pixelcolors.reserve(px * py);

        double jitter = min(1.0, pretraceSize / 2.0);
        double offset = (pretraceSize - 1.0) / 2.0;
        unsigned int subBlockCount = pBlockInfo->incompleteSubBlocks.size();

        int subBlockDivideX;
        int subBlockDivideY;

        if (pretraceCoverage != 0)
        {
            // for the next pass, subdivide further as long as that this leaves us with at least 4x4 pixels
            subBlockDivideX = max( 1, (int)floor((DBL)nextPx / (DBL)(pBlockInfo->subBlockCountX * 4)) );
            subBlockDivideY = max( 1, (int)floor((DBL)nextPy / (DBL)(pBlockInfo->subBlockCountY * 4)) );
        }
        else
        {
            // don't subdivide if we're not using adaptive pretrace
            subBlockDivideX = 1;
            subBlockDivideY = 1;
        }

        for (int sub = 0; sub < subBlockCount; sub ++)
        {

            radiosity.ResetTopLevelStats();
            int pixelCount = 0;

            int subX = pBlockInfo->incompleteSubBlocks.front().subBlockPosX;
            int subY = pBlockInfo->incompleteSubBlocks.front().subBlockPosY;

            DBL subStartX = rect.left + (DBL)rect.GetWidth()  * (DBL) subX / (DBL)(pBlockInfo->subBlockCountX);
            DBL subEndX   = subStartX + (DBL)rect.GetWidth()               / (DBL)(pBlockInfo->subBlockCountX);
            DBL subStartY = rect.top  + (DBL)rect.GetHeight() * (DBL) subY / (DBL)(pBlockInfo->subBlockCountY);
            DBL subEndY   = subStartY + (DBL)rect.GetHeight()              / (DBL)(pBlockInfo->subBlockCountY);

            for(DBL y = startY; y < subEndY; y += pretraceSize)
            {
                if (y < subStartY)
                    continue;

                for(DBL x = startX; x < subEndX; x += pretraceSize)
                {
                    if (x < subStartX)
                        continue;

                    RGBTColour col;

                    trace(x + offset + jitter * randgen(), y + offset + jitter * randgen(), width, height, col);

                    pixelpositions.push_back(Vector2d(x, y));
                    pixelcolors.push_back(col);
                    pixelCount ++;

                    Cooperate();
                }
            }

            long  queryCount;
            float reuse;
            radiosity.GetTopLevelStats(queryCount, reuse);

            bool again;
            if (pixelCount < 9)
                // shoot at least a certain number of rays
                // (NB: Subdivision strategy tries to ensure we get at least 4x4 pixels)
                again = true;
            else if (queryCount == 0)
                // stop if we don't seem to need any samples at all in this square (e.g. because it shows only background)
                again = false;
            else if ((pretraceCoverage != 0) && (reuse / (float)queryCount >= pretraceCoverage))
                // stop if the average number of re-usable samples reaches a certain threshold
                again = false;
            else
                // otherwise do another pass
                again = true;

            pBlockInfo->incompleteSubBlocks.pop_front();
            if (again)
            {
                for (int subDivY = 0; subDivY < subBlockDivideY; subDivY ++)
                    for (int subDivX = 0; subDivX < subBlockDivideX; subDivX ++)
                        pBlockInfo->incompleteSubBlocks.push_back(RadiositySubBlockInfo(subX * subBlockDivideX + subDivX, subY * subBlockDivideY + subDivY));
            }
        }
        GetViewDataPtr()->Stats()[Number_Of_Pixels] += pixelpositions.size();

        radiosity.AfterTile();

        float progressWeight = pow(4.0f, (float)pBlockInfo->pass) / progressWeightTotal;

        pBlockInfo->pass ++;

        if (pBlockInfo->pass < pretraceStepCount && pBlockInfo->incompleteSubBlocks.size() > 0)
        {
            // run another pass
            pBlockInfo->subBlockCountX *= subBlockDivideX;
            pBlockInfo->subBlockCountY *= subBlockDivideY;
            pBlockInfo->completion += progressWeight;
        }
        else
        {
            // no more passes please
            progressWeight = 1.0 - pBlockInfo->completion;
            delete pBlockInfo;
            pBlockInfo = nullptr;
        }

        GetViewDataPtr()->AfterTile();
        if(pixelpositions.size() > 0)
            GetViewData()->CompletedRectangle(rect, serial, pixelpositions, pixelcolors, int(ceil(pretraceSize)), false, false, progressWeight, pBlockInfo);
        else
            GetViewData()->CompletedRectangle(rect, serial, progressWeight, pBlockInfo);
    }
}

void RadiosityTask::Stopped()
{
    // nothing to do for now [trf]
}

void RadiosityTask::Finish()
{
    GetViewDataPtr()->timeType = TraceThreadData::kRadiosityTime;
    GetViewDataPtr()->realTime = ConsumedRealTime();
    GetViewDataPtr()->cpuTime = ConsumedCPUTime();
}

}
// end of namespace pov
