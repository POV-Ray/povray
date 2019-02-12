//******************************************************************************
///
/// @file core/lighting/radiosity.h
///
/// Declarations related to radiosity.
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

#ifndef POVRAY_CORE_RADIOSITY_H
#define POVRAY_CORE_RADIOSITY_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <mutex>
#include <vector>

// POV-Ray header files (base module)
#include "base/fileinputoutput_fwd.h"
#include "base/path_fwd.h"

// POV-Ray header files (core module)
#include "core/lighting/photons.h" // TODO FIXME - make PhotonGatherer class visible only as a pointer
#include "core/material/media.h"   // TODO FIXME - make MediaFunction class visible only as a pointer
#include "core/math/randcosweighted.h"
#include "core/render/trace.h"     // TODO FIXME - make Trace class visible only as a pointer
#include "core/support/octree_fwd.h"
#include "core/support/statistics.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreLightingRadiosity Radiosity
/// @ingroup PovCore
///
/// @{

struct ot_block_struct;
struct ot_node_struct;
struct ot_id_struct;

#define RADIOSITY_CACHE_EXTENSION ".rca"

static const unsigned int RADIOSITY_MAX_SAMPLE_DIRECTIONS    = kRandCosWeightedCount;
// to get some more pseudo-randomness and make use of the full range of all the precomputed sample directions,
// we start each sample direction sequence at a different index than the previous one; 663 has some nice properties
// for this:
// - it is fairly large stride, only giving "overlap" of consecutive samples at high sample counts
// - it has no divisors in common with 1600 (kRandCosWeightedCount), so that any consecutive 1600 samples will start at
//   a different index
// - it gives the highest possible number of "secondary strides", those being -274, 115, -44, -17, -7 and 3

// settings as effective for a particular bounce depth during a particular trace step
struct RadiosityRecursionSettings final
{
    // true "tweakables"
    unsigned int    raysPerSample;          // number of sample rays to shoot per sample
    unsigned int    reuseCount;             // number of samples required for re-use
    double          errorBoundFactor;       // factor governing spacing of samples in general
    double          minReuseFactor;         // factor governing minimum spacing of samples in creases
    double          maxReuseFactor;         // factor governing maximum spacing of samples in open areas
    double          octreeOverfillFactor;   // factor governing octree lookup performance
    unsigned int    traceLevel;             // base trace level to use for secondary rays
    double          weight;                 // base weight to use for secondary rays

    // precomputed values
    double          maxErrorBound;          // maximum error bound to be expected for sample lookup
    double          octreeAddressFactor;    // effective radius factor for filing samples in the octree
};

// settings as specified in the scene file;
// naming conventions are as per the respective scene file parameter
class SceneRadiositySettings final
{
    public:

        // primary settings from the scene file

        bool    radiosityEnabled;

        double  brightness;
        long    count;
        long    directionPoolSize;
        double  errorBound;
        double  grayThreshold;
        double  lowErrorFactor;
        double  minimumReuse;
        bool    minimumReuseSet;
        double  maximumReuse;
        bool    maximumReuseSet;
        long    nearestCount;
        long    nearestCountAPT;
        int     recursionLimit;
        double  maxSample;
        double  adcBailout;
        bool    normal;
        bool    media;
        double  pretraceStart;
        double  pretraceEnd;
        bool    alwaysSample;
        bool    vainPretrace;               // whether to use full quality during pretrace even where it doesn't matter, to give the user a nice show
        float   defaultImportance;
        bool    subsurface;                 // whether to use subsurface scattering for radiosity sampling rays
        bool    brilliance;                 // whether to respect brilliance in radiosity computations

        SceneRadiositySettings() {
            radiosityEnabled    = false;
            brightness          = 1.0;
            count               = 35;
            directionPoolSize   = RADIOSITY_MAX_SAMPLE_DIRECTIONS;
            errorBound          = 1.8;
            grayThreshold       = 0.0;
            lowErrorFactor      = 0.5;
            minimumReuse        = 0.015;
            minimumReuseSet     = false;
            maximumReuse        = 0.2;
            maximumReuseSet     = false;
            nearestCount        = 5;        // TODO FIXME - let's get rid of this completely
            nearestCountAPT     = 0;        // second nearest_count parameter, governing adaptive pretrace
            recursionLimit      = 2;
            maxSample           = -1.0;     // default max brightness allows any
            adcBailout          = 0.01;
            normal              = false;
            media               = false;
            pretraceStart       = 0.08;
            pretraceEnd         = 0.04;
            alwaysSample        = false;
            vainPretrace        = false;
            defaultImportance   = 1.0;
            subsurface          = false;
            brilliance          = false;
        }

        RadiosityRecursionSettings* GetRecursionSettings (bool final) const;
};

class RadiosityCache final
{

    public:

        class BlockPool final
        {
            friend class RadiosityCache;
            public:
                BlockPool();
                ~BlockPool();
            protected:
                ot_block_struct* NewBlock();
                void Save(OStream *fd);
            private:
                struct PoolUnit;
                PoolUnit *head;                 // newest pool unit
                PoolUnit *savedHead;            // newest block that has been saved completely
                unsigned int nextFreeBlock;     // next free block (in *head)
                unsigned int nextUnsavedBlock;  // next unsaved block (in *savedHead predecessor)
        };

        int firstRadiosityPass;

        long ra_reuse_count;
        long ra_gather_count;

        MathColour Gather_Total;
        long Gather_Total_Count;

        #ifdef RADSTATS
            extern long ot_blockcount;
            long ot_seenodecount;
            long ot_seeblockcount;
            long ot_doblockcount;
            long ot_dotokcount;
            long ot_lastcount;
            long ot_lowerrorcount;
        #endif

        RadiosityCache(const SceneRadiositySettings& radset);
        ~RadiosityCache();

        bool Load(const Path& inputFile);
        void InitAutosave(const Path& outputFile, bool append);

        DBL FindReusableBlock(RenderStatistics& stats, DBL errorbound, const Vector3d& ipoint, const Vector3d& snormal, DBL brilliance, MathColour& illuminance, int recursionDepth, int pretraceStep, int tileId);
        BlockPool* AcquireBlockPool();
        void AddBlock(BlockPool* pool, RenderStatistics* stats, const Vector3d& Point, const Vector3d& S_Normal, DBL brilliance, const Vector3d& To_Nearest_Surface,
                      const MathColour& dx, const MathColour& dy, const MathColour& dz, const MathColour& Illuminance,
                      DBL Harmonic_Mean_Distance, DBL Nearest_Distance, DBL Quality, int Bounce_Depth, int pretraceStep, int tileId);
        void ReleaseBlockPool(BlockPool* pool);

    private:

        struct Octree final
        {
            ot_node_struct *root;
#if POV_MULTITHREADED
            std::mutex treeMutex;   // lock this when adding nodes to the tree
            std::mutex blockMutex;  // lock this when adding blocks to any node of the tree
#endif

            Octree() : root(nullptr) {}
        };

        std::vector<BlockPool*> blockPools;  // block pools ready to be re-used
#if POV_MULTITHREADED
        std::mutex blockPoolsMutex;   // lock this when accessing blockPools
#endif

        Octree octree;

        OStream *ot_fd;
#if POV_MULTITHREADED
        std::mutex fileMutex;         // lock this when accessing ot_fd
#endif

        RadiosityRecursionSettings* recursionSettings; // dynamically allocated array; use recursion depth as index

        void InsertBlock(ot_node_struct* node, ot_block_struct *block);
        ot_node_struct *GetNode(RenderStatistics* stats, const ot_id_struct& id);

        static bool AverageNearBlock(ot_block_struct *block, void *void_info);
};

class RadiosityFunction final : public Trace::RadiosityFunctor
{
    public:

        static const unsigned int PRETRACE_INVALID  = kOctreePassInvalid;
        static const unsigned int PRETRACE_FIRST    = kOctreePassFirst;
        static const unsigned int PRETRACE_MAX      = kOctreePassMax;
        static const unsigned int FINAL_TRACE       = kOctreePassFinal;
        static const unsigned int DEPTH_MAX         = (kOctreeDepthMax < 20 ? kOctreeDepthMax : 20);
        static const unsigned int MAX_NEAREST_COUNT = 20;

        // initializes radiosity module from:
        //      sd      - pointer to the scene data
        //      td      - pointer to the thread-specific data
        //      rs      - the radiosity settings as parsed from the scene file
        //      rc      - the radiosity cache to retrieve previously computed samples from, and store newly computed samples in
        //      cf      - the cooperate functor (whatever that is - some thing that handles inter-thread communication?)
        //      ft      - whether this is the final trace (i.e. not a radiosity pretrace step)
        //      camera  - position of the camera
        RadiosityFunction(std::shared_ptr<SceneData> sd, TraceThreadData *td,
                          const SceneRadiositySettings& rs, RadiosityCache& rc, Trace::CooperateFunctor& cf, bool ft, const Vector3d& camera);
        virtual ~RadiosityFunction() override;

        // looks up the ambient value for a certain point
        //      ipoint          - point on the surface
        //      raw_normal      - the geometry raw normal at this point
        //      layer_normal    - texture-perturbed normal
        //      brilliance      - brilliance
        //      ambient_colour  - (output) the ambient color at this point
        //      weight          - the base "weight" of the traced ray (used to compare against ADC bailout)
        virtual void ComputeAmbient(const Vector3d& ipoint, const Vector3d& raw_normal, const Vector3d& layer_normal, DBL brilliance, MathColour& ambient_colour, DBL weight, TraceTicket& ticket) override;

        // checks whether the specified recursion depth is still within the configured limits
        virtual bool CheckRadiosityTraceLevel(const TraceTicket& ticket) override;

        // retrieves top level statistics information to drive pretrace re-iteration
        virtual void GetTopLevelStats(long& queryCount, float& reuse);
        virtual void ResetTopLevelStats();
        virtual void BeforeTile(int id, unsigned int pts = FINAL_TRACE);
        virtual void AfterTile();

    private:

        class SampleDirectionGenerator final
        {
            public:
                /// constructor
                SampleDirectionGenerator();
                /// Called before each tile
                void Reset(unsigned int samplePoolCount);
                /// Called before each sample
                void InitSequence(unsigned int& sample_count, const Vector3d& raw_normal, const Vector3d& layer_normal, bool use_raw_normal, DBL brilliance);
                /// Called to get the next sampling ray direction
                bool GetDirection(Vector3d& direction);
            protected:
                /// number of remaining directions to try
                size_t remainingDirections;
                /// whether we're using the raw surface normal instead of the pertubed normal
                bool rawNormalMode;
                /// the raw surface normal // TODO FIXME - for smooth triangles etc. this *should* be *really* raw, but it isn't!
                Vector3d rawNormal;
                /// the surface brilliance for which to generate sample rays
                DBL brilliance;
                /// direction we'll map the precomputed sample directions' X axis to
                Vector3d frameX;
                /// direction we'll map the precomputed sample directions' Y axis to (the effective normal vector)
                Vector3d frameY;
                /// direction we'll map the precomputed sample directions' Z axis to
                Vector3d frameZ;
                /// Generator for sampling directions
                SequentialVectorGeneratorPtr sampleDirections;
        };

        // structure to store precomputed effective parameters for each recursion depth
        struct RecursionParameters final
        {
            SampleDirectionGenerator directionGenerator;    // sample generator for this recursion depth
            IntStatsIndex statsId;                          // statistics id for per-pass per-recursion statistics
            IntStatsIndex queryCountStatsId;                // statistics id for per-recursion statistics
            FPStatsIndex weightStatsId;                     // statistics id for per-recursion statistics
        };

        // The modules that do the actual computing
        // (we use our own instances for the sake of thread-safety)

        Trace trace;                        // does the main raytracing
        MediaFunction media;                // computes media effects
        PhotonGatherer photonGatherer;      // computes photon-based illumination

        // Local data

        TraceThreadData *threadData;
        RadiosityCache& radiosityCache;     // this is where we retrieve previously computed samples from, and store newly computed samples in
        RadiosityCache::BlockPool* cacheBlockPool;
        DBL errorBound;                     // the error_bound setting
        bool isFinalTrace;
        unsigned int pretraceStep;
        Vector3d cameraPosition;
        const SceneRadiositySettings&       settings;
        const RadiosityRecursionSettings*   recursionSettings;      // dynamically allocated array; use recursion depth as index
        RecursionParameters*                recursionParameters;    // dynamically allocated array; use recursion depth as index
        long topLevelQueryCount;
        float topLevelReuse;
        int tileId;

        double GatherLight(const Vector3d& IPoint, const Vector3d& Raw_Normal, const Vector3d& LayNormal, DBL brilliance, MathColour& Illuminance, TraceTicket& ticket);
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_RADIOSITY_H
