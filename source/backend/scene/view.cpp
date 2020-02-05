//******************************************************************************
///
/// @file backend/scene/view.cpp
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
#include "backend/scene/view.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>
#include <chrono>

// Boost header files
#include <boost/bind.hpp>
#include <boost/math/common_factor.hpp>

// POV-Ray header files (base module)
#include "base/path.h"
#include "base/povassert.h"
#include "base/timer.h"
#include "base/image/colourspace.h"

// POV-Ray header files (core module)
#include "core/lighting/photons.h"
#include "core/lighting/radiosity.h"
#include "core/math/matrix.h"
#include "core/support/octree.h"

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"
#include "povms/povmsid.h"

// POV-Ray header files (backend module)
#include "backend/control/messagefactory.h"
#include "backend/control/renderbackend.h"
#include "backend/lighting/photonestimationtask.h"
#include "backend/lighting/photonshootingstrategy.h"
#include "backend/lighting/photonshootingtask.h"
#include "backend/lighting/photonsortingtask.h"
#include "backend/lighting/photonstrategytask.h"
#include "backend/render/radiositytask.h"
#include "backend/render/tracetask.h"
#include "backend/scene/backendscenedata.h"
#include "backend/scene/viewthreaddata.h"

// this must be the last file included
#include "base/povdebug.h"

#define DEFAULT_BLOCK_SIZE 32

namespace pov
{

using std::min;
using std::max;
using std::shared_ptr;
using std::vector;

/// Round up to a power of two.
inline unsigned int MakePowerOfTwo(unsigned int i)
{
    unsigned int ii = 0;

    for(i >>= 1; i != 0; i >>= 1, ii++) { }

    return 1 << ii;
}

ViewData::ViewData(shared_ptr<BackendSceneData> sd) :
    nextBlock(0),
    completedFirstPass(false),
    highestTraceLevel(0),
    width(160),
    height(120),
    blockWidth(10),
    blockHeight(8),
    blockSize(DEFAULT_BLOCK_SIZE),
    realTimeRaytracing(false),
    rtrData(nullptr),
    renderArea(0, 0, 159, 119),
    radiosityCache(sd->radiositySettings),
    sceneData(sd),
    qualityFlags(9)
{
}

ViewData::~ViewData()
{
    if (rtrData != nullptr)
        delete rtrData;
}

void ViewData::getBlockXY(const unsigned int nb, unsigned int &x, unsigned int &y)
{
    unsigned long neo_nb = nb; /* must be larger, if possible */
    unsigned long sz = blockWidth*blockHeight;

    if (renderBlockStep>1)
    {
        /* clock arithmetic: if renderBlockStep & sz are prime (gcd == 1), then
        **  q := (n * renderBlockStep)% sz , for n in [0,sz[,
        ** q is also in [0,sz[
        ** and it's a bijection (every n provides a different q)
        **
        ** if they are not pseudo prime, you're stuck!
        */
        neo_nb *= renderBlockStep;
        neo_nb %= sz;
    }
    switch(renderPattern)
    {
         case 1:
             x = neo_nb / blockHeight;
             y = neo_nb % blockHeight;
             break;
         case 2:
             x = neo_nb % blockWidth;
             y = neo_nb / blockWidth;
             if (x & 1)
             {
                 x = blockWidth-1-x/2;
             }
             else
             {
                 x /= 2;
             }
             if (y & 1)
             {
                 y = blockHeight-1-y/2;
             }
             else
             {
                 y /= 2;
             }
             break;
         case 3:
             x = (sz-1-neo_nb) % blockWidth;
             y = (sz-1-neo_nb) / blockWidth;
             if (x & 1)
             {
                 x = blockWidth-1-x/2;
             }
             else
             {
                 x /= 2;
             }
             if (y & 1)
             {
                 y = blockHeight-1-y/2;
             }
             else
             {
                 y /= 2;
             }
             break;
         case 4:
             x = neo_nb / blockHeight;
             y = neo_nb % blockHeight;
             if (x & 1)
             {
                 x = blockWidth-1-x/2;
             }
             else
             {
                 x /= 2;
             }
             if (y & 1)
             {
                 y = blockHeight-1-y/2;
             }
             else
             {
                 y /= 2;
             }
             break;
         case 5:
             x = (sz-1-neo_nb) / blockHeight;
             y = (sz-1-neo_nb) % blockHeight;
             if (x & 1)
             {
                 x = blockWidth-1-x/2;
             }
             else
             {
                 x /= 2;
             }
             if (y & 1)
             {
                 y = blockHeight-1-y/2;
             }
             else
             {
                 y /= 2;
             }
             break;
         default:
             x = neo_nb % blockWidth;
             y = neo_nb / blockWidth;
             break;
     }/* all values are covered */
}

bool ViewData::GetNextRectangle(POVRect& rect, unsigned int& serial)
{
    std::lock_guard<std::mutex> lock(nextBlockMutex);

    while(true)
    {
        if(nextBlock >= (blockWidth * blockHeight))
            return false;

        unsigned int tempNextBlock = nextBlock; // TODO FIXME - works around nasty gcc (found using 4.0.1) bug failing to honor casting away of volatile on pass by value on template argument lookup [trf]

        if((blockSkipList.empty() == true) || (blockSkipList.find((unsigned int)tempNextBlock) == blockSkipList.end()))
            break;

        blockSkipList.erase((unsigned int)tempNextBlock);
        nextBlock++;
    }

    unsigned int blockX;
    unsigned int blockY;
    getBlockXY(nextBlock,blockX,blockY);

    rect.left = renderArea.left + (blockX * blockSize);
    rect.right = min(renderArea.left + ((blockX + 1) * blockSize) - 1, renderArea.right);
    rect.top = renderArea.top + (blockY * blockSize);
    rect.bottom = min(renderArea.top + ((blockY + 1) * blockSize) - 1, renderArea.bottom);

    pixelsPending += rect.GetArea();

    serial = nextBlock;
    nextBlock++;

    blockBusyList.insert(serial);

    return true;
}

bool ViewData::GetNextRectangle(POVRect& rect, unsigned int& serial, BlockInfo*& blockInfo, unsigned int stride)
{
    std::lock_guard<std::mutex> lock(nextBlockMutex);

    BlockIdSet newPostponedList;

    if (stride != 0)
    {
        unsigned int oldNextBlock = nextBlock;

        bool usePostponed = false;
        for(BlockIdSet::iterator i = blockPostponedList.begin(); i != blockPostponedList.end(); i ++)
        {
            usePostponed = true;
            for(BlockIdSet::iterator busy = blockBusyList.begin(); busy != blockBusyList.end(); busy ++)
            {
                if((*i >= *busy) && ((*i - *busy) % stride == 0))
                {
                    usePostponed = false;
                    break;
                }
            }
            if (usePostponed)
            {
                serial = *i;
                blockPostponedList.erase(i);
                break;
            }
        }

        if(!usePostponed)
        {
            while(true)
            {
                if(nextBlock >= (blockWidth * blockHeight))
                {
                    nextBlock = oldNextBlock;
                    return false;
                }

                unsigned int tempNextBlock = nextBlock; // TODO FIXME - works around nasty gcc (found using 4.0.1) bug failing to honor casting away of volatile on pass by value on template argument lookup [trf]

                if((blockSkipList.empty() == true) || (blockSkipList.find((unsigned int)tempNextBlock) == blockSkipList.end()))
                {
                    bool avoid = false;
                    for(BlockIdSet::iterator busy = blockBusyList.begin(); busy != blockBusyList.end(); busy ++)
                    {
                        if((tempNextBlock >= *busy) && ((tempNextBlock - *busy) % stride == 0))
                        {
                            avoid = true;
                            break;
                        }
                    }

                    if(avoid)
                    {
                        newPostponedList.insert(tempNextBlock);
                    }
                    else
                    {
                        serial = tempNextBlock;
                        nextBlock++;
                        break;
                    }
                }
                else
                {
                    // blockSkipList.erase((unsigned int)tempNextBlock);
                }

                nextBlock++;
            }
        }
    }
    else
    {
        unsigned int oldNextBlock = nextBlock;

        while(true)
        {
            if(nextBlock >= (blockWidth * blockHeight))
            {
                nextBlock = 0;
                completedFirstPass = true;
            }

            unsigned int tempNextBlock = nextBlock; // TODO FIXME - works around nasty gcc (found using 4.0.1) bug failing to honor casting away of volatile on pass by value on template argument lookup [trf]

            if(!completedFirstPass || blockInfoList[tempNextBlock])
            {
                if((blockSkipList.empty() == true) || (blockSkipList.find((unsigned int)tempNextBlock) == blockSkipList.end()))
                {
                    if((blockBusyList.empty() == true) || (blockBusyList.find((unsigned int)tempNextBlock) == blockBusyList.end()))
                    {
                        serial = tempNextBlock;
                        nextBlock++;
                        break;
                    }
                }
                else
                {
                    // blockSkipList.erase((unsigned int)tempNextBlock);
                }
            }

            nextBlock++;

            if (oldNextBlock == nextBlock)
                return false;
        }
    }

    unsigned int blockX ;
    unsigned int blockY ;
    getBlockXY(serial,blockX,blockY);

    rect.left = renderArea.left + (blockX * blockSize);
    rect.right = min(renderArea.left + ((blockX + 1) * blockSize) - 1, renderArea.right);
    rect.top = renderArea.top + (blockY * blockSize);
    rect.bottom = min(renderArea.top + ((blockY + 1) * blockSize) - 1, renderArea.bottom);

    pixelsPending += rect.GetArea();

    blockBusyList.insert(serial);
    for(BlockIdSet::iterator i = newPostponedList.begin(); i != newPostponedList.end(); i ++)
        blockPostponedList.insert(*i);

    blockInfo = blockInfoList[serial];

    return true;
}

void ViewData::CompletedRectangle(const POVRect& rect, unsigned int serial, const vector<RGBTColour>& pixels, unsigned int size, bool relevant, bool complete, float completion, BlockInfo* blockInfo)
{
    if (realTimeRaytracing == true)
    {
        POV_RTR_ASSERT(pixels.size() == rect.GetArea());
        int y = rect.top - 1;
        int x = rect.right;
        int offset;
        for(vector<RGBTColour>::const_iterator i(pixels.begin()); i != pixels.end(); i++)
        {
            if (++x > rect.right)
                offset = (++y * width + (x = rect.left)) * 5;
            POV_RTR_ASSERT(rtrData->rtrPixels.size() >= offset + 5);
            rtrData->rtrPixels[offset++] = i->red();
            rtrData->rtrPixels[offset++] = i->green();
            rtrData->rtrPixels[offset++] = i->blue();
            rtrData->rtrPixels[offset++] = 0.0; // unused component
            rtrData->rtrPixels[offset++] = i->transm();
        }
    }
    else
    {
        try
        {
            POVMS_Message pixelblockmsg(kPOVObjectClass_PixelData, kPOVMsgClass_ViewImage, kPOVMsgIdent_PixelBlockSet);
            vector<POVMSFloat> pixelvector;

            pixelvector.reserve(pixels.size() * 5);

            for(vector<RGBTColour>::const_iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                pixelvector.push_back(i->red());
                pixelvector.push_back(i->green());
                pixelvector.push_back(i->blue());
                pixelvector.push_back(0.0); // unused component
                pixelvector.push_back(i->transm());
            }

            POVMS_Attribute pixelattr(pixelvector);

            pixelblockmsg.Set(kPOVAttrib_PixelBlock, pixelattr);
            if (relevant)
                pixelblockmsg.SetVoid(kPOVAttrib_PixelFinal);
            if (complete)
                // only completely rendered blocks get a block id
                // (used by continue-trace to identify blocks that do not need to be rendered again)
                pixelblockmsg.SetInt(kPOVAttrib_PixelId, serial);
            pixelblockmsg.SetInt(kPOVAttrib_PixelSize, size);
            pixelblockmsg.SetInt(kPOVAttrib_Left, rect.left);
            pixelblockmsg.SetInt(kPOVAttrib_Top, rect.top);
            pixelblockmsg.SetInt(kPOVAttrib_Right, rect.right);
            pixelblockmsg.SetInt(kPOVAttrib_Bottom, rect.bottom);

            pixelblockmsg.SetInt(kPOVAttrib_ViewId, viewId);
            pixelblockmsg.SetSourceAddress(sceneData->backendAddress);
            pixelblockmsg.SetDestinationAddress(sceneData->frontendAddress);

            POVMS_SendMessage(pixelblockmsg);
        }
        catch(pov_base::Exception&)
        {
            // TODO - work out what we should do here. until then, just re-raise the exception.
            throw;
        }
    }

    // update render progress information
    CompletedRectangle(rect, serial, completion, blockInfo);
}

void ViewData::CompletedRectangle(const POVRect& rect, unsigned int serial, const vector<Vector2d>& positions, const vector<RGBTColour>& colors, unsigned int size, bool relevant, bool complete, float completion, BlockInfo* blockInfo)
{
    try
    {
        if(positions.size() != colors.size())
            throw POV_EXCEPTION(kInvalidDataSizeErr, "Number of pixel colors and pixel positions does not match!");

        POVMS_Message pixelblockmsg(kPOVObjectClass_PixelData, kPOVMsgClass_ViewImage, kPOVMsgIdent_PixelSet);
        vector<POVMSInt> positionvector;
        vector<POVMSFloat> colorvector;

        positionvector.reserve(positions.size() * 2);
        colorvector.reserve(colors.size() * 5);

        for(vector<Vector2d>::const_iterator i(positions.begin()); i != positions.end(); i++)
        {
            positionvector.push_back(POVMSInt(i->x()));
            positionvector.push_back(POVMSInt(i->y()));
        }

        for(vector<RGBTColour>::const_iterator i(colors.begin()); i != colors.end(); i++)
        {
            colorvector.push_back(i->red());
            colorvector.push_back(i->green());
            colorvector.push_back(i->blue());
            colorvector.push_back(0.0); // unused component
            colorvector.push_back(i->transm());
        }

        POVMS_Attribute pixelposattr(positionvector);
        POVMS_Attribute pixelcolattr(colorvector);

        pixelblockmsg.Set(kPOVAttrib_PixelPositions, pixelposattr);
        pixelblockmsg.Set(kPOVAttrib_PixelColors, pixelcolattr);
        if (relevant)
            pixelblockmsg.SetVoid(kPOVAttrib_PixelFinal);
        if (complete)
            // only completely rendered blocks get a block id
            // (used by continue-trace to identify blocks that do not need to be rendered again)
            pixelblockmsg.SetInt(kPOVAttrib_PixelId, serial);
        pixelblockmsg.SetInt(kPOVAttrib_PixelSize, size);

        pixelblockmsg.SetInt(kPOVAttrib_ViewId, viewId);
        pixelblockmsg.SetSourceAddress(sceneData->backendAddress);
        pixelblockmsg.SetDestinationAddress(sceneData->frontendAddress);

        POVMS_SendMessage(pixelblockmsg);
    }
    catch(pov_base::Exception&)
    {
        // TODO - work out what we should do here. until then, just re-raise the exception.
        throw;
    }

    // update render progress information
    CompletedRectangle(rect, serial, completion, blockInfo);
}

void ViewData::CompletedRectangle(const POVRect& rect, unsigned int serial, float completion, BlockInfo* blockInfo)
{
    {
        std::lock_guard<std::mutex> lock(nextBlockMutex);
        blockBusyList.erase(serial);
        blockInfoList[serial] = blockInfo;
    }

    if (realTimeRaytracing == true)
    {
        // nothing to do
    }
    else
    {
        try
        {
            pixelsCompleted += (int)(rect.GetArea() * completion);

            POVMS_Object obj(kPOVObjectClass_RenderProgress);
            // TODO obj.SetLong(kPOVAttrib_RealTime, ElapsedRealTime());
            obj.SetInt(kPOVAttrib_Pixels, renderArea.GetArea());
            obj.SetInt(kPOVAttrib_PixelsPending, pixelsPending - pixelsCompleted + rect.GetArea());
            obj.SetInt(kPOVAttrib_PixelsCompleted, pixelsCompleted);
            RenderBackend::SendViewOutput(viewId, sceneData->frontendAddress, kPOVMsgIdent_Progress, obj);
        }

        catch(pov_base::Exception&)
        {
            // TODO - work out what we should do here. until then, just re-raise the exception.
            throw;
        }
    }
}

void ViewData::SetNextRectangle(const BlockIdSet& bsl, unsigned int fs)
{
    blockSkipList = bsl;
    blockBusyList.clear(); // safety catch; shouldn't be necessary
    blockPostponedList.clear(); // safety catch; shouldn't be necessary
    nextBlock = fs;
    completedFirstPass = false; // TODO
    pixelsCompleted = 0; // TODO
}

void ViewData::SetHighestTraceLevel(unsigned int htl)
{
    std::lock_guard<std::mutex> lock(setDataMutex);

    highestTraceLevel = max(highestTraceLevel, htl);
}

const QualityFlags& ViewData::GetQualityFeatureFlags() const
{
    return qualityFlags;
}

RadiosityCache& ViewData::GetRadiosityCache()
{
    return radiosityCache;
}

View::View(shared_ptr<BackendSceneData> sd, unsigned int width, unsigned int height, RenderBackend::ViewId vid) :
    viewData(sd),
    stopRequsted(false),
    mailbox(0),
    renderControlThread(nullptr)
{
    viewData.viewId = vid;
    viewData.width = width;
    viewData.height = height;

    POV_MEM_STATS_RENDER_BEGIN();

    if(sd->boundingMethod == 2)
        mailbox = BSPTree::Mailbox(sd->numberOfFiniteObjects);
}

View::~View()
{
    stopRequsted = true; // NOTE: Order is important here, set this before stopping the queue!
    renderTasks.Stop();

    if (renderControlThread != nullptr)
        renderControlThread->join();
    delete renderControlThread;

    for(vector<ViewThreadData *>::iterator i(viewThreadData.begin()); i != viewThreadData.end(); i++)
        delete (*i);
    viewThreadData.clear();

    // ok to call this more than once (it could be called from the stats method too)
    POV_MEM_STATS_RENDER_END();
}

bool View::CheckCameraHollowObject(const Vector3d& point, const BBOX_TREE *node)
{
    // TODO FIXME - duplicate code - remove again!!!

    // Check current node.
    if((node->Infinite == false) && (Inside_BBox(point, node->BBox) == false))
        return false;

    if(node->Entries)
    {
        // This is a node containing leaves to be checked.
        for(int i = 0; i < node->Entries; i++)
            if(CheckCameraHollowObject(point, node->Node[i]))
                return true;
    }
    else
    {
        size_t seed = 0; // TODO
        // This is a leaf so test contained object.
        TraceThreadData threadData(viewData.GetSceneData(), seed); // TODO: avoid the need to construct threadData
        ObjectPtr object = reinterpret_cast<ObjectPtr>(node->Node);
        if ((object->interior != nullptr) && (object->Inside(point, &threadData)))
            return true;
    }

    return false;
}

bool View::CheckCameraHollowObject(const Vector3d& point)
{
    size_t seed = 0; // TODO
    shared_ptr<BackendSceneData>& sd = viewData.GetSceneData();

    if(sd->boundingMethod == 2)
    {
        HasInteriorPointObjectCondition precond;
        TruePointObjectCondition postcond;
        TraceThreadData threadData(sd, seed); // TODO: avoid the need to construct threadData
        BSPInsideCondFunctor ifn(point, sd->objects, &threadData, precond, postcond);

        mailbox.clear();
        if ((*sd->tree)(point, ifn, mailbox, true))
            return true;

        // test infinite objects
        for(vector<ObjectPtr>::iterator object = sd->objects.begin() + sd->numberOfFiniteObjects; object != sd->objects.end(); object++)
            if (((*object)->interior != nullptr) && Inside_BBox(point, (*object)->BBox) && (*object)->Inside(point, &threadData))
                return true;
    }
    else if ((sd->boundingMethod == 0) || (sd->boundingSlabs == nullptr))
    {
        TraceThreadData threadData(sd, seed); // TODO: avoid the need to construct threadData
        for(vector<ObjectPtr>::const_iterator object = viewData.GetSceneData()->objects.begin(); object != viewData.GetSceneData()->objects.end(); object++)
            if ((*object)->interior != nullptr)
                if((*object)->Inside(point, &threadData))
                    return true;
    }
    else
    {
        return CheckCameraHollowObject(point, sd->boundingSlabs);
    }

    return false;
}

void View::StartRender(POVMS_Object& renderOptions)
{
    unsigned int tracingmethod = 0;
    DBL jitterscale = 1.0;
    bool jitter = false;
    DBL aathreshold = 0.3;
    DBL aaconfidence = 0.9;
    unsigned int aadepth = 3;
    DBL aaGammaValue = 1.0;
    GammaCurvePtr aaGammaCurve;
    unsigned int previewstartsize = 0;
    unsigned int previewendsize = 0;
    unsigned int nextblock = 0;
    bool highReproducibility = false;
    size_t seed = 0;
    shared_ptr<ViewData::BlockIdSet> blockskiplist(new ViewData::BlockIdSet());

    if (renderControlThread == nullptr)
        renderControlThread = new std::thread(boost::bind(&View::RenderControlThread, this));

    viewData.qualityFlags = QualityFlags(clip(renderOptions.TryGetInt(kPOVAttrib_Quality, 9), 0, 9));

    if(renderOptions.TryGetBool(kPOVAttrib_Antialias, false) == true)
        tracingmethod = clip(renderOptions.TryGetInt(kPOVAttrib_SamplingMethod, 1), 0, 3); // TODO FIXME - magic number in clip

    aadepth = clip((unsigned int)renderOptions.TryGetInt(kPOVAttrib_AntialiasDepth, 3), 1u, 9u);
    aathreshold = clip(renderOptions.TryGetFloat(kPOVAttrib_AntialiasThreshold, 0.3f), 0.0f, 1.0f);
    aaconfidence = clip(renderOptions.TryGetFloat(kPOVAttrib_AntialiasConfidence, 0.9f), 0.0f, 1.0f);
    if(renderOptions.TryGetBool(kPOVAttrib_Jitter, true) == true)
        jitterscale = clip(renderOptions.TryGetFloat(kPOVAttrib_JitterAmount, 1.0f), 0.0f, 1.0f);
    else
        jitterscale = 0.0f;
    aaGammaValue = renderOptions.TryGetFloat(kPOVAttrib_AntialiasGamma, 2.5f);
    if (aaGammaValue > 0)
        aaGammaCurve = PowerLawGammaCurve::GetByDecodingGamma(aaGammaValue);
    if (viewData.GetSceneData()->workingGamma)
        aaGammaCurve = TranscodingGammaCurve::Get(viewData.GetSceneData()->workingGamma, aaGammaCurve);

    previewstartsize = MakePowerOfTwo(clip((unsigned int)renderOptions.TryGetInt(kPOVAttrib_PreviewStartSize, 1), 1u, 64u));
    previewendsize = MakePowerOfTwo(clip((unsigned int)renderOptions.TryGetInt(kPOVAttrib_PreviewEndSize, 1), 1u, previewstartsize));
    if((previewendsize == 2) && (tracingmethod == 0)) // optimisation to render all pixels only once
        previewendsize = 1;

    highReproducibility = renderOptions.TryGetBool(kPOVAttrib_HighReproducibility, false);

    seed = renderOptions.TryGetInt(kPOVAttrib_StochasticSeed, 0);
    if (seed == 0)
        // The following expression returns the number of _ticks_ elapsed since
        // the system clock's _epoch_, where a _tick_ is the platform-dependent
        // shortest time interval the system clock can measure, and _epoch_ is a
        // platform-dependent point in time (usually 1970-01-01 00:00:00).
        seed = std::chrono::system_clock::now().time_since_epoch().count();

    // TODO FIXME - [CLi] handle loading, storing (and later optionally deleting) of radiosity cache file for trace abort & continue feature
    // TODO FIXME - [CLi] if high reproducibility is a demand, timing of writing samples to disk is an issue regarding abort & continue
    bool loadRadiosityCache = renderOptions.TryGetBool(kPOVAttrib_RadiosityFromFile, false);
    bool saveRadiosityCache = renderOptions.TryGetBool(kPOVAttrib_RadiosityToFile, false);
    if (loadRadiosityCache || saveRadiosityCache)
    {
        // TODO FIXME - [CLi] I guess the radiosity file name needs more attention than this; probably a frontend job
        Path radiosityFile = Path(renderOptions.TryGetUCS2String(kPOVAttrib_RadiosityFileName, "object.rca"));
        if(loadRadiosityCache)
            loadRadiosityCache = viewData.radiosityCache.Load(radiosityFile);
        if(saveRadiosityCache)
            viewData.radiosityCache.InitAutosave(radiosityFile, loadRadiosityCache); // if we loaded the file, add to existing data
    }

    viewData.GetSceneData()->radiositySettings.vainPretrace = renderOptions.TryGetBool(kPOVAttrib_RadiosityVainPretrace, true);


     // TODO FIXME - all below is not implemented properly and not threadsafe [trf]


    viewData.GetSceneData()->radiositySettings.radiosityEnabled = viewData.GetSceneData()->radiositySettings.radiosityEnabled && viewData.qualityFlags.radiosity;
    viewData.GetSceneData()->photonSettings.photonsEnabled = viewData.GetSceneData()->photonSettings.photonsEnabled && (viewData.qualityFlags.photons);
    viewData.GetSceneData()->useSubsurface = viewData.GetSceneData()->useSubsurface && (viewData.qualityFlags.subsurface);

    if(viewData.GetSceneData()->photonSettings.Max_Trace_Level < 0)
        viewData.GetSceneData()->photonSettings.Max_Trace_Level = viewData.GetSceneData()->parsedMaxTraceLevel;

    if(viewData.GetSceneData()->photonSettings.adcBailout < 0)
        viewData.GetSceneData()->photonSettings.adcBailout = viewData.GetSceneData()->parsedAdcBailout;


    // TODO FIXME - end of todo fixme stuff above


    DBL raleft = renderOptions.TryGetFloat(kPOVAttrib_Left, 1.0f);
    DBL ratop = renderOptions.TryGetFloat(kPOVAttrib_Top, 1.0f);
    DBL raright = renderOptions.TryGetFloat(kPOVAttrib_Right, POVMSFloat(viewData.width));
    DBL rabottom = renderOptions.TryGetFloat(kPOVAttrib_Bottom, POVMSFloat(viewData.height));

    if((raleft >= 0.0) && (raleft < 1.0))
        viewData.renderArea.left = int(DBL(viewData.GetWidth()) * raleft);
    else
        viewData.renderArea.left = int(raleft) - 1;

    if((ratop >= 0.0) && (ratop < 1.0))
        viewData.renderArea.top = int(DBL(viewData.GetHeight()) * ratop);
    else
        viewData.renderArea.top = int(ratop - 1);

    if((raright >= 0.0) && (raright <= 1.0))
        viewData.renderArea.right = int(DBL(viewData.GetWidth()) * raright) - 1;
    else
        viewData.renderArea.right = int(raright - 1);

    if((rabottom >= 0.0) && (rabottom <= 1.0))
        viewData.renderArea.bottom = int(DBL(viewData.GetHeight()) * rabottom) - 1;
    else
        viewData.renderArea.bottom = int(rabottom - 1);

    // TODO FIXME - need to check this in front end, then change this code to set the values to default
    if ((viewData.renderArea.left >= viewData.GetWidth()) || (viewData.renderArea.top >= viewData.GetHeight()))
        throw POV_EXCEPTION(kParamErr, "Invalid start column or row");
    if ((viewData.renderArea.right >= viewData.GetWidth()) || (viewData.renderArea.bottom >= viewData.GetHeight()))
        throw POV_EXCEPTION(kParamErr, "Invalid end column or row");

    viewData.blockSize = renderOptions.TryGetInt(kPOVAttrib_RenderBlockSize, 32);
    if(viewData.blockSize < 4)
        viewData.blockSize = 4;
    if(viewData.blockSize > max(viewData.renderArea.GetWidth(), viewData.renderArea.GetHeight()))
        viewData.blockSize = max(viewData.renderArea.GetWidth(), viewData.renderArea.GetHeight());
    viewData.blockWidth = ((viewData.renderArea.GetWidth() + viewData.blockSize - 1) / viewData.blockSize);
    viewData.blockHeight = ((viewData.renderArea.GetHeight() + viewData.blockSize - 1) / viewData.blockSize);

    viewData.renderPattern = (unsigned int)renderOptions.TryGetInt(kPOVAttrib_RenderPattern,0);
    viewData.renderBlockStep = (unsigned int)renderOptions.TryGetInt(kPOVAttrib_RenderBlockStep,0);
    if (viewData.renderBlockStep > 0)
    {
        // check that the value is prime with blockWidth * blockHeight,
        // if not reduces it until it does (1 is ok... but boring)
        // when renderBlockStep is positive, the factor (renderBlockStep)
        // must be prime with the modulus (blockWidth * blockHeight),
        // (otherwise, the full range [0.. W*H-1] won't be covered)
        //
        // reduce renderBlockStep to [0;blockWidth * blockHeight-1] range
        // (it's not worth keeping a bigger value)
        //
        viewData.renderBlockStep %= (viewData.blockWidth*viewData.blockHeight);
        if (!viewData.renderBlockStep)
            viewData.renderBlockStep = (viewData.blockWidth*viewData.blockHeight);
        while(boost::math::gcd((long)viewData.renderBlockStep,(long)(viewData.blockWidth*viewData.blockHeight))>1)
            viewData.renderBlockStep--;
    }

    viewData.blockInfoList.resize(viewData.blockWidth * viewData.blockHeight);

    viewData.pixelsPending = 0;
    viewData.pixelsCompleted = 0;

    // continue trace
    nextblock = renderOptions.TryGetInt(kPOVAttrib_PixelId, 0);

    if(renderOptions.Exist(kPOVAttrib_PixelSkipList) == true)
    {
        vector<POVMSInt> psl(renderOptions.GetIntVector(kPOVAttrib_PixelSkipList));

        for(vector<POVMSInt>::iterator i(psl.begin()); i != psl.end(); i++)
            blockskiplist->insert(*i);
    }

    viewData.SetNextRectangle(*blockskiplist, nextblock);

    // render thread count
    int maxRenderThreads = renderOptions.TryGetInt(kPOVAttrib_MaxRenderThreads, 1);

    viewData.realTimeRaytracing = renderOptions.TryGetBool(kPOVAttrib_RealTimeRaytracing, false); // TODO - experimental code
    if (viewData.realTimeRaytracing)
        viewData.rtrData = new RTRData(viewData, maxRenderThreads);

    // camera changes without parsing
    if(renderOptions.Exist(kPOVAttrib_SceneCamera) == false)
        viewData.camera = viewData.GetSceneData()->parsedCamera;
    else // INCOMPLETE EXPERIMENTAL [trf]
    {
        POVMS_Object camera;

        renderOptions.Get(kPOVAttrib_SceneCamera, camera);

        // TODO FIXME - clear by setting scene's camera, but not sure if this is the way to go in the long run [trf]
        viewData.camera = viewData.GetSceneData()->parsedCamera;

        bool had_location = false;
        bool had_direction = false;
        bool had_up = false;
        bool had_right = false;
        bool had_sky = false;
        bool had_look_at = false;

        if(camera.Exist('cloc') == true)
        {
            std::vector<POVMSFloat> pv = camera.GetFloatVector('cloc');
            viewData.camera.Location = Vector3d(pv[X], pv[Y], pv[Z]);
            had_location = true;
        }

        if(camera.Exist('cdir') == true)
        {
            std::vector<POVMSFloat> pv = camera.GetFloatVector('cdir');
            viewData.camera.Direction = Vector3d(pv[X], pv[Y], pv[Z]);
            had_direction = true;
        }

        if(camera.Exist('cup ') == true)
        {
            std::vector<POVMSFloat> pv = camera.GetFloatVector('cup ');
            viewData.camera.Up = Vector3d(pv[X], pv[Y], pv[Z]);
            had_up = true;
        }

        if(camera.Exist('crig') == true)
        {
            std::vector<POVMSFloat> pv = camera.GetFloatVector('crig');
            viewData.camera.Right = Vector3d(pv[X], pv[Y], pv[Z]);
            had_right = true;
        }

        if(camera.Exist('csky') == true)
        {
            std::vector<POVMSFloat> pv = camera.GetFloatVector('csky');
            viewData.camera.Sky = Vector3d(pv[X], pv[Y], pv[Z]);
            had_sky = true;
        }

        if(camera.Exist('clat') == true)
        {
            std::vector<POVMSFloat> pv = camera.GetFloatVector('clat');
            viewData.camera.Look_At = Vector3d(pv[X], pv[Y], pv[Z]);
            had_look_at = true;
        }

        // apply "look_at"
        if(had_look_at == true)
        {
            DBL Direction_Length = 1.0, Up_Length, Right_Length, Handedness;
            Vector3d tempv;

            Direction_Length = viewData.camera.Direction.length();
            Up_Length        = viewData.camera.Up.length();
            Right_Length     = viewData.camera.Right.length();
            tempv            = cross(viewData.camera.Up, viewData.camera.Direction);
            Handedness       = dot(tempv, viewData.camera.Right);

            viewData.camera.Direction = viewData.camera.Look_At - viewData.camera.Location;

            // Check for zero length direction vector.
            if(viewData.camera.Direction.lengthSqr() < EPSILON)
                ; // Error("Camera location and look_at point must be different.");

            viewData.camera.Direction.normalize();

            // Save Right vector
            tempv = viewData.camera.Right;

            viewData.camera.Right = cross(viewData.camera.Sky, viewData.camera.Direction);

            // Avoid DOMAIN error (from Terry Kanakis)
            if((fabs(viewData.camera.Right[X]) < EPSILON) &&
               (fabs(viewData.camera.Right[Y]) < EPSILON) &&
               (fabs(viewData.camera.Right[Z]) < EPSILON))
            {
                // Warning("Camera location to look_at direction and sky direction should be different.\n"
                //         "Using default/supplied right vector instead.");

                // Restore Right vector
                viewData.camera.Right = tempv;
            }

            viewData.camera.Right.normalize();
            viewData.camera.Up = cross(viewData.camera.Direction, viewData.camera.Right);
            viewData.camera.Direction *= Direction_Length;

            if (Handedness > 0.0)
            {
                viewData.camera.Right *= Right_Length;
            }
            else
            {
                viewData.camera.Right *= -Right_Length;
            }

            viewData.camera.Up *= Up_Length;
        }
    }

    if(CheckCameraHollowObject(viewData.camera.Location))
    {
        // TODO FIXME
        // Warning("Camera is inside a non-hollow object. Fog and participating media may not work as expected.");
    }

    // check for preview end size
    if((previewendsize > 1) && (tracingmethod == 0))
    {
        // TODO FIXME
        // Warning("In POV-Ray v3.7 and later it is recommended to set the mosaic preview end size to one for\n"
        //         "maximum performance when rendering without anti-aliasing.");
    }

    // do photons
    /*
    if(viewData.GetSceneData()->photonSettings.photonsEnabled)
    {
        viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new PhotonTask(&viewData))));

        // wait for photons to finish
        renderTasks.AppendSync();
    }
    */
    if(viewData.GetSceneData()->photonSettings.photonsEnabled)
    {
        if (!viewData.GetSceneData()->photonSettings.fileName.empty() && viewData.GetSceneData()->photonSettings.loadFile)
        {
            vector<PhotonMap*> surfaceMaps;
            vector<PhotonMap*> mediaMaps;

            // when we pass a null parameter for the "strategy" (last parameter),
            // then this will LOAD the photon map
            viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new PhotonSortingTask(
                &viewData, surfaceMaps, mediaMaps, nullptr, seed
                ))));
            // wait for photons to finish
            renderTasks.AppendSync();
        }
        else
        {
            PhotonShootingStrategy* strategy = new PhotonShootingStrategy();

            viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new PhotonEstimationTask(
                &viewData, seed
                ))));
            // wait for photons to finish
            renderTasks.AppendSync();

            viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new PhotonStrategyTask(
                &viewData, strategy, seed
                ))));
            // wait for photons to finish
            renderTasks.AppendSync();

            vector<PhotonMap*> surfaceMaps;
            vector<PhotonMap*> mediaMaps;

            for(int i = 0; i < maxRenderThreads; i++)
            {
                PhotonShootingTask* task = new PhotonShootingTask(&viewData, strategy, seed);
                surfaceMaps.push_back(task->getSurfacePhotonMap());
                mediaMaps.push_back(task->getMediaPhotonMap());
                viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(task)));
            }
            // wait for photons to finish
            renderTasks.AppendSync();

            // this merges the maps, sorts, computes gather options, and then cleans up memory
            viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new PhotonSortingTask(
                &viewData, surfaceMaps, mediaMaps, strategy, seed
                ))));
            // wait for photons to finish
            renderTasks.AppendSync();

        }
    }

    // do radiosity pretrace
    if(viewData.GetSceneData()->radiositySettings.radiosityEnabled)
    {
        // TODO load radiosity data (if applicable)?

        DBL maxWidthHeight  = DBL(max(viewData.GetWidth(), viewData.GetHeight()));
        DBL startSize       = maxWidthHeight * viewData.GetSceneData()->radiositySettings.pretraceStart;
        DBL endSize         = maxWidthHeight * viewData.GetSceneData()->radiositySettings.pretraceEnd;
        if (endSize < 1.0)
        {
            endSize = 1.0;
            // Warning("Radiosity pretrace end too low for selected resolution. Pretrace will be\n"
            //         "stopped early, corresponding to a value of %lf.\n"
            //         "To avoid this warning, increase pretrace_end.", endSize / maxWidthHeight);
        }
        int steps = (int)floor(log(startSize/endSize)/log(2.0) + (1.0 - EPSILON)) + 1;
        if (steps > RadiosityFunction::PRETRACE_MAX - RadiosityFunction::PRETRACE_FIRST - 1)
        {
            steps = RadiosityFunction::PRETRACE_MAX - RadiosityFunction::PRETRACE_FIRST - 1;
            startSize = endSize * pow(2.0, (double)steps);
            // Warning("Too many radiosity pretrace steps. Pretrace will be started late,\n"
            //         "corresponding to a value of %lf", startSize);
            //         "To avoid this warning, decrease pretrace_start or increase pretrace_end.", endSize / maxWidthHeight);
        }

        if (highReproducibility)
        {
            int nominalThreads = 1;
            int actualThreads;
            DBL stepSize = startSize;
            DBL actualSize;
            for(int step = RadiosityFunction::PRETRACE_FIRST; step < RadiosityFunction::PRETRACE_FIRST + steps; step ++)
            {
                actualThreads = min(nominalThreads, maxRenderThreads);
                actualSize = max(stepSize, endSize);

                // do render one pretrace step with current pretrace size
                for(int i = 0; i < actualThreads; i++)
                    viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new RadiosityTask(
                        &viewData, actualSize, actualSize, step, 1, nominalThreads, seed
                        ))));

                // wait for previous pretrace step to finish
                renderTasks.AppendSync();

                // reset block size counter and block skip list for next pretrace step
                renderTasks.AppendFunction(boost::bind(&View::SetNextRectangle, this, _1, blockskiplist, nextblock));

                // wait for block size counter and block skip list reset to finish
                renderTasks.AppendSync();

                stepSize *= 0.5;
                nominalThreads *= 2;
            }
        }
        else if (steps > 0)
        {
            // do render all pretrace steps
            for(int i = 0; i < maxRenderThreads; i++)
                viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new RadiosityTask(
                    &viewData, startSize, endSize, RadiosityFunction::PRETRACE_FIRST, steps, 0, seed
                    ))));

            // wait for pretrace to finish
            renderTasks.AppendSync();

            // reset block size counter and block skip list for main render
            renderTasks.AppendFunction(boost::bind(&View::SetNextRectangle, this, _1, blockskiplist, nextblock));

            // wait for block size counter and block skip list reset to finish
            renderTasks.AppendSync();
        }

        // TODO store radiosity data (if applicable)?
    }

    // do render with mosaic preview
    if(previewstartsize > 1)
    {
        // If the mosaic preview goes all the way down to single-pixel size and no anti-aliasing is required,
        // we don't need a dedicated final render pass.
        bool previewIsFinalPass = (previewendsize == 1) && (tracingmethod == 0);

        // do render with mosaic preview start size
        for(int i = 0; i < maxRenderThreads; i++)
            viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new TraceTask(
                &viewData, 0, jitterscale, aathreshold, aaconfidence, aadepth, aaGammaCurve,
                previewstartsize, false, previewIsFinalPass, highReproducibility, seed
                ))));

        for(unsigned int step = (previewstartsize >> 1); step >= previewendsize; step >>= 1)
        {
            // wait for previous mosaic preview step to finish
            renderTasks.AppendSync();

            // reset block size counter and block skip list
            renderTasks.AppendFunction(boost::bind(&View::SetNextRectangle, this, _1, blockskiplist, nextblock));

            // wait for block size counter and block skip list reset to finish
            renderTasks.AppendSync();

            // do render with current mosaic preview size
            for(int i = 0; i < maxRenderThreads; i++)
                viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new TraceTask(
                    &viewData, 0, jitterscale, aathreshold, aaconfidence, aadepth, aaGammaCurve,
                    step, true, previewIsFinalPass, highReproducibility, seed
                    ))));
        }

        if (!previewIsFinalPass)
        {
            // wait for previous mosaic preview step to finish
            renderTasks.AppendSync();

            // reset block size counter and block skip list
            renderTasks.AppendFunction(boost::bind(&View::SetNextRectangle, this, _1, blockskiplist, nextblock));

            // wait for block size counter and block skip list reset to finish
            renderTasks.AppendSync();

            for(int i = 0; i < maxRenderThreads; i++)
                viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new TraceTask(
                    &viewData, tracingmethod, jitterscale, aathreshold, aaconfidence, aadepth, aaGammaCurve,
                    0, false, true, highReproducibility, seed
                    ))));
        }
    }
    // do render without mosaic preview
    else
    {
        for(int i = 0; i < maxRenderThreads; i++)
            viewThreadData.push_back(dynamic_cast<ViewThreadData *>(renderTasks.AppendTask(new TraceTask(
                &viewData, tracingmethod, jitterscale, aathreshold, aaconfidence, aadepth, aaGammaCurve,
                0, false, true, highReproducibility, seed
                ))));
    }

    // wait for render to finish
    renderTasks.AppendSync();

    // send shutdown messages
    renderTasks.AppendFunction(boost::bind(&View::DispatchShutdownMessages, this, _1));

    // wait for shutdown messages to be sent
    renderTasks.AppendSync();

    // send statistics
    renderTasks.AppendFunction(boost::bind(&View::SendStatistics, this, _1));

    // send done message
    POVMS_Message doneMessage(kPOVObjectClass_ResultData, kPOVMsgClass_ViewOutput, kPOVMsgIdent_Done);
    doneMessage.SetInt(kPOVAttrib_ViewId, viewData.viewId);
    doneMessage.SetSourceAddress(viewData.sceneData->backendAddress);
    doneMessage.SetDestinationAddress(viewData.sceneData->frontendAddress);
    renderTasks.AppendMessage(doneMessage);
}

void View::StopRender()
{
    renderTasks.Stop();

    RenderBackend::SendViewFailedResult(viewData.viewId, kUserAbortErr, viewData.sceneData->frontendAddress);
}

void View::GetStatistics(POVMS_Object& renderStats)
{
    RenderStatistics    stats;

    for(vector<ViewThreadData *>::iterator i(viewThreadData.begin()); i != viewThreadData.end(); i++)
        stats += (*i)->Stats();

    // object intersection stats
    POVMS_List isectStats;

    for (size_t index = 0; intersection_stats[index].infotext != nullptr; index++)
    {
        POVMS_Object isectStat(kPOVObjectClass_IsectStat);

        isectStat.SetString(kPOVAttrib_ObjectName, intersection_stats[index].infotext);
        isectStat.SetInt(kPOVAttrib_ObjectID, intersection_stats[index].povms_id);
        isectStat.SetLong(kPOVAttrib_ISectsTests, stats[intersection_stats[index].stat_test_id]);
        isectStat.SetLong(kPOVAttrib_ISectsSucceeded, stats[intersection_stats[index].stat_suc_id]);

        isectStats.Append(isectStat);
    }

    renderStats.Set(kPOVAttrib_ObjectIStats, isectStats);

    // general stats
    renderStats.SetInt(kPOVAttrib_Height, viewData.GetHeight());
    renderStats.SetInt(kPOVAttrib_Width, viewData.GetWidth());

    // basic tracing stats
    renderStats.SetLong(kPOVAttrib_Pixels, stats[Number_Of_Pixels]);
    renderStats.SetLong(kPOVAttrib_PixelSamples, stats[Number_Of_Samples]);
    renderStats.SetLong(kPOVAttrib_Rays, stats[Number_Of_Rays]);
    renderStats.SetLong(kPOVAttrib_RaysSaved, stats[ADC_Saves]);

    // detailed tracing stats
    renderStats.SetLong(kPOVAttrib_ShadowTest, stats[Shadow_Ray_Tests]);
    renderStats.SetLong(kPOVAttrib_ShadowTestSuc, stats[Shadow_Rays_Succeeded]);
    renderStats.SetLong(kPOVAttrib_ShadowCacheHits, stats[Shadow_Cache_Hits]);
    renderStats.SetLong(kPOVAttrib_MediaSamples, stats[Media_Samples]);
    renderStats.SetLong(kPOVAttrib_MediaIntervals, stats[Media_Intervals]);
    renderStats.SetLong(kPOVAttrib_ReflectedRays, stats[Reflected_Rays_Traced]);
    renderStats.SetLong(kPOVAttrib_InnerReflectedRays, stats[Internal_Reflected_Rays_Traced]);
    renderStats.SetLong(kPOVAttrib_RefractedRays, stats[Refracted_Rays_Traced]);
    renderStats.SetLong(kPOVAttrib_TransmittedRays, stats[Transmitted_Rays_Traced]);

    // other tracing-related stats
    renderStats.SetLong(kPOVAttrib_IsoFindRoot, stats[Ray_IsoSurface_Find_Root]);
    renderStats.SetLong(kPOVAttrib_FunctionVMCalls, stats[Ray_Function_VM_Calls]);
    renderStats.SetLong(kPOVAttrib_FunctionVMInstrEst, stats[Ray_Function_VM_Instruction_Est]);
    renderStats.SetLong(kPOVAttrib_PolynomTest, stats[Polynomials_Tested]);
    renderStats.SetLong(kPOVAttrib_RootsEliminated, stats[Roots_Eliminated]);
    renderStats.SetLong(kPOVAttrib_CallsToNoise, stats[Calls_To_Noise]);
    renderStats.SetLong(kPOVAttrib_CallsToDNoise, stats[Calls_To_DNoise]);

    renderStats.SetLong(kPOVAttrib_CrackleCacheTest, stats[CrackleCache_Tests]);
    renderStats.SetLong(kPOVAttrib_CrackleCacheTestSuc, stats[CrackleCache_Tests_Succeeded]);

    POV_LONG current;
    POV_ULONG allocs(0), frees(0), peak(0), smallest(0), largest(0);
    POV_MEM_STATS_RENDER_END();
    if (POV_GLOBAL_MEM_STATS(allocs, frees, current, peak, smallest, largest))
    {
        if (allocs)
            renderStats.SetLong(kPOVAttrib_CallsToAlloc, allocs);
        if (frees)
            renderStats.SetLong(kPOVAttrib_CallsToFree, frees);
        if (peak)
            renderStats.SetLong(kPOVAttrib_PeakMemoryUsage, peak);
        if (smallest)
            renderStats.SetLong(kPOVAttrib_MinAlloc, smallest);
        if (largest)
            renderStats.SetLong(kPOVAttrib_MaxAlloc, largest);
    }

    renderStats.SetInt(kPOVAttrib_TraceLevel, viewData.highestTraceLevel);
    renderStats.SetInt(kPOVAttrib_MaxTraceLevel, viewData.sceneData->parsedMaxTraceLevel);

    renderStats.SetLong(kPOVAttrib_RadGatherCount, stats[Radiosity_GatherCount]);
    renderStats.SetLong(kPOVAttrib_RadUnsavedCount, stats[Radiosity_UnsavedCount]);
    renderStats.SetLong(kPOVAttrib_RadReuseCount, stats[Radiosity_ReuseCount]);
    renderStats.SetLong(kPOVAttrib_RadRayCount, stats[Radiosity_RayCount]);
    renderStats.SetLong(kPOVAttrib_RadTopLevelGatherCount, stats[Radiosity_TopLevel_GatherCount]);
    renderStats.SetLong(kPOVAttrib_RadTopLevelReuseCount, stats[Radiosity_TopLevel_ReuseCount]);
    renderStats.SetLong(kPOVAttrib_RadTopLevelRayCount, stats[Radiosity_TopLevel_RayCount]);
    renderStats.SetLong(kPOVAttrib_RadFinalGatherCount, stats[Radiosity_Final_GatherCount]);
    renderStats.SetLong(kPOVAttrib_RadFinalReuseCount, stats[Radiosity_Final_ReuseCount]);
    renderStats.SetLong(kPOVAttrib_RadFinalRayCount, stats[Radiosity_Final_RayCount]);
    renderStats.SetLong(kPOVAttrib_RadOctreeNodes, stats[Radiosity_OctreeNodes]);
    renderStats.SetLong(kPOVAttrib_RadOctreeLookups, stats[Radiosity_OctreeLookups]);
    renderStats.SetLong(kPOVAttrib_RadOctreeAccepts0, stats[Radiosity_OctreeAccepts0]);
    renderStats.SetLong(kPOVAttrib_RadOctreeAccepts1, stats[Radiosity_OctreeAccepts1]);
    renderStats.SetLong(kPOVAttrib_RadOctreeAccepts2, stats[Radiosity_OctreeAccepts2]);
    renderStats.SetLong(kPOVAttrib_RadOctreeAccepts3, stats[Radiosity_OctreeAccepts3]);
    renderStats.SetLong(kPOVAttrib_RadOctreeAccepts4, stats[Radiosity_OctreeAccepts4]);
    renderStats.SetLong(kPOVAttrib_RadOctreeAccepts5, stats[Radiosity_OctreeAccepts5]);

    for (int recursion = 0; recursion < 5; recursion ++)
    {
        long id;
        long queryCount = stats[IntStatsIndex(Radiosity_QueryCount_R0 + recursion)];
        id = kPOVAttrib_RadQueryCountR0 + recursion;
        renderStats.SetLong(id, queryCount);
        for(int pass = 1; pass <= 5; pass ++)
        {
            id = kPOVAttrib_RadSamplesP1R0 + (pass-1)*256 + recursion;
            renderStats.SetLong(id, stats[IntStatsIndex(Radiosity_SamplesTaken_PTS1_R0 + (pass-1)*5 + recursion)]);
        }
        id = kPOVAttrib_RadSamplesFR0 + recursion;
        renderStats.SetLong(id, stats[IntStatsIndex(Radiosity_SamplesTaken_Final_R0 + recursion)]);
        id = kPOVAttrib_RadWeightR0 + recursion;
        if (queryCount > 0)
            renderStats.SetFloat(id, stats[FPStatsIndex(Radiosity_Weight_R0 + recursion)] / (double)queryCount);
        else
            renderStats.SetFloat(id, 0.0);
    }

    // photon stats // TODO FIXME - move to photon pass? [trf]
    renderStats.SetLong(kPOVAttrib_PhotonsShot, stats[Number_Of_Photons_Shot]);
    renderStats.SetLong(kPOVAttrib_PhotonsStored, stats[Number_Of_Photons_Stored]);
    renderStats.SetLong(kPOVAttrib_GlobalPhotonsStored, stats[Number_Of_Global_Photons_Stored]);
    renderStats.SetLong(kPOVAttrib_MediaPhotonsStored, stats[Number_Of_Media_Photons_Stored]);
    renderStats.SetLong(kPOVAttrib_PhotonsPriQInsert, stats[Priority_Queue_Add]);
    renderStats.SetLong(kPOVAttrib_PhotonsPriQRemove, stats[Priority_Queue_Remove]);
    renderStats.SetLong(kPOVAttrib_GatherPerformedCnt, stats[Gather_Performed_Count]);
    renderStats.SetLong(kPOVAttrib_GatherExpandedCnt, stats[Gather_Expanded_Count]);

    struct TimeData final
    {
        POV_LONG cpuTime;
        POV_LONG realTime;
        size_t samples;

        TimeData() : cpuTime(0), realTime(0), samples(0) { }
    };

    TimeData timeData[TraceThreadData::kMaxTimeType];

    for(vector<ViewThreadData *>::iterator i(viewThreadData.begin()); i != viewThreadData.end(); i++)
    {
        timeData[(*i)->timeType].realTime = max(timeData[(*i)->timeType].realTime, (*i)->realTime);
        timeData[(*i)->timeType].cpuTime += (*i)->cpuTime;
        timeData[(*i)->timeType].samples++;
    }

    for(size_t i = TraceThreadData::kUnknownTime; i < TraceThreadData::kMaxTimeType; i++)
    {
        if(timeData[i].samples > 0)
        {
            POVMS_Object elapsedTime(kPOVObjectClass_ElapsedTime);

            elapsedTime.SetLong(kPOVAttrib_RealTime, timeData[i].realTime);
            elapsedTime.SetLong(kPOVAttrib_CPUTime, timeData[i].cpuTime);
            elapsedTime.SetInt(kPOVAttrib_TimeSamples, (int) timeData[i].samples);

            switch(i)
            {
                case TraceThreadData::kPhotonTime:
                    renderStats.Set(kPOVAttrib_PhotonTime, elapsedTime);
                    break;
                case TraceThreadData::kRadiosityTime:
                    renderStats.Set(kPOVAttrib_RadiosityTime, elapsedTime);
                    break;
                case TraceThreadData::kRenderTime:
                    renderStats.Set(kPOVAttrib_TraceTime, elapsedTime);
                    break;
            }
        }
    }
}

void View::PauseRender()
{
    renderTasks.Pause();
}

void View::ResumeRender()
{
    renderTasks.Resume();
}

bool View::IsRendering()
{
    return renderTasks.IsRunning();
}

bool View::IsPaused()
{
    return renderTasks.IsPaused();
}

bool View::Failed()
{
    return renderTasks.Failed();
}

void View::DispatchShutdownMessages(TaskQueue&)
{
    MessageFactory messageFactory(viewData.GetSceneData()->warningLevel, "Shutdown",
                                  viewData.sceneData->backendAddress, viewData.sceneData->frontendAddress,
                                  viewData.sceneData->sceneId, viewData.viewId);

    for (vector<ObjectPtr>::iterator it = viewData.sceneData->objects.begin(); it != viewData.sceneData->objects.end(); it++)
        (*it)->DispatchShutdownMessages(messageFactory);
}

void View::SendStatistics(TaskQueue&)
{
    POVMS_Message renderStats(kPOVObjectClass_RenderStatistics, kPOVMsgClass_ViewOutput, kPOVMsgIdent_RenderStatistics);

    GetStatistics(renderStats);

    renderStats.SetInt(kPOVAttrib_ViewId, viewData.viewId);
    renderStats.SetSourceAddress(viewData.sceneData->backendAddress);
    renderStats.SetDestinationAddress(viewData.sceneData->frontendAddress);

    POVMS_SendMessage(renderStats);

    for(vector<ViewThreadData *>::iterator i(viewThreadData.begin()); i != viewThreadData.end(); i++)
        delete (*i);
    viewThreadData.clear();
}

void View::SetNextRectangle(TaskQueue&, shared_ptr<ViewData::BlockIdSet> bsl, unsigned int fs)
{
    viewData.SetNextRectangle(*bsl, fs);
}

void View::RenderControlThread()
{
    bool sentFailedResult = false;

    while(stopRequsted == false)
    {
        while((renderTasks.Process() == true) && (stopRequsted == false)) { }

        if((renderTasks.IsDone() == true) && (renderTasks.Failed() == true) && (sentFailedResult == false))
        {
            RenderBackend::SendViewFailedResult(viewData.viewId, renderTasks.FailureCode(kUncategorizedError), viewData.sceneData->frontendAddress);
            sentFailedResult = true;
        }

        if(stopRequsted == false)
        {
            std::this_thread::yield();
            Delay(50);
        }
    }
}

RTRData::RTRData(ViewData& v, int mrt) :
    viewData(v),
    numRTRframes(0),
    numRenderThreads(mrt),
    numRenderThreadsCompleted(0),
    numPixelsCompleted(0)
{
    width = viewData.GetWidth();
    height = viewData.GetHeight();
    rtrPixels.resize(width * height * 5);
}

RTRData::~RTRData()
{
    event.notify_all();
};

// TODO: it will be more efficient using atomic operators on numRenderThreadsCompleted.
// as it stands, we only use counterMutex to avoid a race condition that exists (or
// if it's not that, then for some reason notify_all() isn't releasing all of the
// waiting threads on win32). currently the below code is slightly sub-optimal in
// that we only get about 95% CPU utilization, as compared to the 99-100% we were
// getting with the code as of rev #110 (see change #4275). however for now we put
// up with a few percent idle CPU time in order to avoid said race condition (which
// lead to all threads ending up waiting on the condition).
const Camera *RTRData::CompletedFrame()
{
    std::unique_lock<std::mutex> lock (counterMutex);

    vector<Camera>& cameras = viewData.GetSceneData()->cameras;
    bool ca = viewData.GetSceneData()->clocklessAnimation;

    if(true) // yes I know it's not needed, but I prefer this over headless code blocks
    {
        // test >= in case of weirdness due to the timed wait we use with the std::condition_variable
        if (++numRenderThreadsCompleted >= numRenderThreads)
        {
            viewData.SetNextRectangle(ViewData::BlockIdSet(), 0);
            try
            {
                POVMS_Message pixelblockmsg(kPOVObjectClass_PixelData, kPOVMsgClass_ViewImage, kPOVMsgIdent_PixelBlockSet);
                POVMS_Attribute pixelattr(rtrPixels);

                // we can release the other threads now.
                numRenderThreadsCompleted = 0;
                numRTRframes++;

                event.notify_all();

                pixelblockmsg.Set(kPOVAttrib_PixelBlock, pixelattr);
                pixelblockmsg.SetInt(kPOVAttrib_PixelSize, 1);
                pixelblockmsg.SetInt(kPOVAttrib_Left, 0);
                pixelblockmsg.SetInt(kPOVAttrib_Top, 0);
                pixelblockmsg.SetInt(kPOVAttrib_Right, width - 1);
                pixelblockmsg.SetInt(kPOVAttrib_Bottom, height - 1);

                pixelblockmsg.SetInt(kPOVAttrib_ViewId, viewData.GetViewId());
                pixelblockmsg.SetSourceAddress(viewData.GetSceneData()->backendAddress);
                pixelblockmsg.SetDestinationAddress(viewData.GetSceneData()->frontendAddress);

                POVMS_SendMessage(pixelblockmsg);

                numPixelsCompleted += width * height;
                POVMS_Object obj(kPOVObjectClass_RenderProgress);
                obj.SetInt(kPOVAttrib_Pixels, width * height);
                obj.SetInt(kPOVAttrib_PixelsPending, 0);
                obj.SetInt(kPOVAttrib_PixelsCompleted, numPixelsCompleted);
                RenderBackend::SendViewOutput(viewData.GetViewId(), viewData.GetSceneData()->frontendAddress, kPOVMsgIdent_Progress, obj);

                return (ca ? &cameras[numRTRframes % cameras.size()] : nullptr);
            }
            catch(pov_base::Exception&)
            {
                // TODO - work out what we should do here. until then, just re-raise the exception.
                // oh yeah, might want to release any waiting threads first ...
                event.notify_all();
                throw;
            }
        }
    }

    // this will cause us to wait until the other threads are done.
    // we use a timed lock so that we eventually pick up a render cancel request.
    // if we do exit as a result of a timeout, and there is not a cancel pending,
    // things could get out of whack.
    if (event.wait_for(lock, std::chrono::seconds(3)) == std::cv_status::timeout)
        numRenderThreadsCompleted--;

    return (ca ? &cameras[numRTRframes % cameras.size()] : nullptr);
}

}
// end of namespace pov
