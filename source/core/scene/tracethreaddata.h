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

#ifndef POVRAY_CORE_TRACETHREADDATA_H
#define POVRAY_CORE_TRACETHREADDATA_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <stack>
#include <vector>

// POV-Ray header files (base module)
#include "base/types.h"
#include "base/colour.h"

// POV-Ray header files (core module)
#include "core/coretypes.h"
#include "core/bounding/boundingcylinder.h"
#include "core/math/randomsequence_fwd.h"
#include "core/math/vector.h"
#include "core/scene/scenedata_fwd.h"
#include "core/support/cracklecache_fwd.h"
#include "core/support/statistics_fwd.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCore
///
/// @{

using namespace pov_base;

class PhotonMap;
struct Blob_Interval_Struct;

/// Class holding parser thread specific data.
class TraceThreadData : public ThreadData
{
    public:

        /// Create thread local data.
        /// @param  sd      Scene data defining scene attributes.
        /// @param  seed    Seed for the stochastic random number generator;
        ///                 should be unique for each render.
        TraceThreadData(std::shared_ptr<SceneData> sd, size_t seed);

        /// Destructor.
        virtual ~TraceThreadData() override;

        /// Get the statistics.
        /// @return     Reference to statistic counters.
        RenderStatistics& Stats(void) { return *mpRenderStats; }

        DBL *Fractal_IStack[4];
        void **Blob_Queue;
        unsigned int Max_Blob_Queue_Size;
        DBL *Blob_Coefficients;
        Blob_Interval_Struct *Blob_Intervals;
        int Blob_Coefficient_Count;
        int Blob_Interval_Count;
        std::vector<BCYL_INT> BCyl_Intervals;
        std::vector<BCYL_INT> BCyl_RInt;
        std::vector<BCYL_INT> BCyl_HInt;
        IStackPool stackPool;
        std::vector<GenericFunctionContextPtr> functionContextPool;
        int Facets_Last_Seed;
        int Facets_CVC;
        Vector3d Facets_Cube[81];

        /// Common random number generator for all stochastic stuff
        SeedableDoubleGeneratorPtr stochasticRandomGenerator;
        size_t stochasticRandomSeedBase;

        // TODO FIXME - thread-local copy of lightsources. we need this
        // because various parts of the lighting code seem to make changes
        // to the lightsource object passed to them (this is not confined
        // just to the area light shadow code). This code ought to be fixed
        // to treat the lightsource as const, after which this can go away.
        std::vector<LightSource*> lightSources;

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

        CrackleCache* mpCrackleCache;

        // data for waves and ripples pattern
        unsigned int numberOfWaves;
        std::vector<double> waveFrequencies;
        std::vector<Vector3d> waveSources;

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

        inline std::shared_ptr<const SceneData> GetSceneData() const { return sceneData; }

    protected:
        /// scene data
        std::shared_ptr<SceneData> sceneData;
        /// render statistics
        RenderStatistics* mpRenderStats;

    private:

        TraceThreadData() = delete;
        TraceThreadData(const TraceThreadData&) = delete;
        TraceThreadData& operator=(const TraceThreadData&) = delete;

        /// current tile index (for crackle cache expiry)
        size_t progress_index;
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_TRACETHREADDATA_H
