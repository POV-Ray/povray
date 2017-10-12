//******************************************************************************
///
/// @file core/scene/tracethreaddata.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_CORE_TRACETHREADDATA_H
#define POVRAY_CORE_TRACETHREADDATA_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include <vector>
#include <stack>

#include "base/types.h"

#include "core/coretypes.h"
#include "core/bounding/boundingcylinder.h"
#include "core/bounding/bsptree.h"
#include "core/material/pattern.h"
#include "core/shape/mesh.h"
#include "core/support/statistics.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCore
///
/// @{

using namespace pov_base;

class SceneData;
struct ISO_ThreadData;

class PhotonMap;
struct Blob_Interval_Struct;

/// Class holding parser thread specific data.
class TraceThreadData : public ThreadData
{
        friend class Scene;
        friend class Trace;
        friend class View; // TODO FIXME - needed only to access TraceThreadData for CheckCameraHollowObject()

    public:

        /// Create thread local data.
        /// @param  sd  Scene data defining scene attributes.
        TraceThreadData(shared_ptr<SceneData> sd);

        /// Destructor.
        ~TraceThreadData();

        /// Get the statistics.
        /// @return     Reference to statistic counters.
        RenderStatistics& Stats(void) { return renderStats; }

        DBL *Fractal_IStack[4];
        BBoxPriorityQueue Mesh_Queue;
        void **Blob_Queue;
        unsigned int Max_Blob_Queue_Size;
        DBL *Blob_Coefficients;
        Blob_Interval_Struct *Blob_Intervals;
        int Blob_Coefficient_Count;
        int Blob_Interval_Count;
        ISO_ThreadData *isosurfaceData;     ///< @todo We may want to move this data block to the isosurface code as a local variable.
        vector<BCYL_INT> BCyl_Intervals;
        vector<BCYL_INT> BCyl_RInt;
        vector<BCYL_INT> BCyl_HInt;
        IStackPool stackPool;
        vector<GenericFunctionContextPtr> functionContextPool;
        int Facets_Last_Seed;
        int Facets_CVC;
        Vector3d Facets_Cube[81];

        // TODO FIXME - thread-local copy of lightsources. we need this
        // because various parts of the lighting code seem to make changes
        // to the lightsource object passed to them (this is not confined
        // just to the area light shadow code). This code ought to be fixed
        // to treat the lightsource as const, after which this can go away.
        vector<LightSource *> lightSources;

        // all of these are for photons
        // most of them should be refactored into parameters, return values, or other objects
        LightSource *photonSourceLight;
        ObjectPtr photonTargetObject;
        bool litObjectIgnoresPhotons;
        MathColour GFilCol;
        int hitObject;    // did we hit the target object? (for autostop)
        DBL photonSpread; // photon spread (in radians)
        DBL photonDepth;  // total distance from light to intersection
        int passThruThis;           // is this a pass-through object encountered before the target?
        int passThruPrev;           // was the previous object a pass-through object encountered before the target?
        bool Light_Is_Global;       // is the current light global? (not part of a light_group?)
        PhotonMap* surfacePhotonMap;
        PhotonMap* mediaPhotonMap;

        CrackleCache mCrackleCache;

        // data for waves and ripples pattern
        unsigned int numberOfWaves;
        vector<double> waveFrequencies;
        vector<Vector3d> waveSources;

        /// Called after a rectangle is finished.
        /// Used for crackle cache expiry.
        void AfterTile();

        /// Used by the crackle pattern to indicate age of cache entries.
        /// @return     The index of the current rectangle rendered.
        inline size_t ProgressIndex() const { return progress_index; }

        enum TimeType
        {
            kUnknownTime,
            kParseTime,
            kBoundingTime,
            kPhotonTime,
            kRadiosityTime,
            kRenderTime,
            kMaxTimeType
        };

        TimeType timeType;
        POV_LONG cpuTime;
        POV_LONG realTime;
        QualityFlags qualityFlags; // TODO FIXME - remove again

        inline shared_ptr<const SceneData> GetSceneData() const { return sceneData; }

    protected:
        /// scene data
        shared_ptr<SceneData> sceneData;
        /// render statistics
        RenderStatistics renderStats;

    private:
        /// not available
        TraceThreadData();

        /// not available
        TraceThreadData(const TraceThreadData&);

        /// not available
        TraceThreadData& operator=(const TraceThreadData&);

        /// current number of Tiles to expire crackle cache entries after
        size_t CrCache_MaxAge;
        /// current tile index (for crackle cache expiry)
        size_t progress_index;
};

/// @}
///
//##############################################################################

}

#endif // POVRAY_CORE_TRACETHREADDATA_H
