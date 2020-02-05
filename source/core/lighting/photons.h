//******************************************************************************
///
/// @file core/lighting/photons.h
///
/// Declarations related to Photon Mapping.
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

#ifndef POVRAY_CORE_PHOTONS_H
#define POVRAY_CORE_PHOTONS_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"
#include "core/lighting/photons_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <string>
#include <vector>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/material/media.h"
#include "core/render/trace.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreLightingPhotons Photon Mapping
/// @ingroup PovCore
///
/// @{

using namespace pov_base;

#define MEDIA_INTERACTION 1

/* ------------------------------------------------------ */
class ScenePhotonSettings final
{
    public:
        ScenePhotonSettings()
        {
            photonsEnabled = false;

            surfaceSeparation = 0.1;
            globalSeparation = 0.1;
            surfaceCount = 0;

            expandTolerance = 0;
            minExpandCount = 0;
            minGatherCount = 10;
            maxGatherCount = 100;

            // trace settings for the photon pre-trace
            Max_Trace_Level = 10;
            adcBailout = 0.001;
            jitter = 0.25;

            // see the POV documentation for info about autostop
            autoStopPercent = 1.0;  // disabled by default

            // these are used for saving or loading the photon map
            // note: save and load are mutually exclusive - there are three states: save, load, neither;
            // save is indicated by a non-empty fileName with loadFile set to false.
            loadFile = false;

            #ifdef GLOBAL_PHOTONS
            // ---------- global photon map ----------
            int globalCount = 0;  // disabled by default
            #endif

            // settings for media photons
            mediaSpacingFactor = 0;
            maxMediaSteps = 0;  // disabled by default

            // reflection blur settings
            // to be used with the reflection blur patch
            //photonReflectionBlur = false;
        }

        bool photonsEnabled;

        // photon separation (a.k.a. spacing)
        DBL surfaceSeparation;
        DBL globalSeparation;
        // ...or photon count
        int surfaceCount;

        // settings for the adaptive search
        // see POV documentation for an explanation
        DBL expandTolerance;
        int minExpandCount;
        int minGatherCount;
        int maxGatherCount;

        // trace settings for the photon pre-trace
        int Max_Trace_Level;
        DBL adcBailout;
        DBL jitter;

        // see the POV documentation for info about autostop
        DBL autoStopPercent;

        // these are used for saving or loading the photon map
        // note: save and load are mutually exclusive - there are three states: save, load, neither;
        // save is indicated by a non-empty fileName with loadFile set to false.
        std::string fileName;
        bool loadFile;

        #ifdef GLOBAL_PHOTONS
        // ---------- global photon map ----------
        int globalPhotonsToShoot;      // number of global photons to shoot
        DBL globalGatherRad;           // minimum gather radius
        int globalCount;
        #endif

        // settings for media photons
        DBL mediaSpacingFactor;
        int maxMediaSteps;

        // reflection blur settings
        // to be used with the reflection blur patch
        //bool photonReflectionBlur;
};

/* ------------------------------------------------------ */
/* photon */
/* ------------------------------------------------------ */

typedef float PhotonScalar;
typedef GenericVector3d<PhotonScalar> PhotonVector3d;

struct Photon final
{
    void init(unsigned char _info)
    {
        this->info = _info;
    }

    PhotonVector3d Loc;     /* location */
    PhotonColour colour;    /* color & intensity (flux) */
    unsigned char info;     /* info byte for kd-tree */
    signed char theta, phi; /* incoming direction */
};

/* ------------------------------------------------------ */
/* photon map */
/* ------------------------------------------------------ */

class PhotonMap final
{

    private:

        class PhotonBlock;

    public:

        std::vector<PhotonBlock*> mBlockList;

        int numPhotons;         /* total number of photons used */
        DBL minGatherRad;       /* minimum gather radius */
        DBL minGatherRadMult;   /* minimum gather radius multiplier (for speed adjustments) */
        DBL gatherRadStep;      /* step size for gather expansion */
        int gatherNumSteps;     /* maximum times to perform 'gather' */

        PhotonMap();
        ~PhotonMap();

        void swapPhotons(int a, int b);
        void insertSort(int start, int end, int d);
        void quickSortRec(int left, int right, int d);
        void halfSortRec(int left, int right, int d, int mid);
        void sortAndSubdivide(int start, int end, int /*sorted*/);
        void buildTree();

        void setGatherOptions(ScenePhotonSettings& photonSettings, bool mediaMap);

        Photon* AllocatePhoton();

        void mergeMap(PhotonMap* map);

        Photon& GetPhoton(unsigned int photonId);
        const Photon& GetPhoton(unsigned int photonId) const;

    protected:

        /* ------------------------------------------------------ */
        /*
          Photon array mapping function

          This converts a one-dimensional index into a two-dimensional address
          in the photon array.

          Photon memory looks like this:

            # -> **********
            # -> **********  <- blocks of photons
            # -> **********
            # -> /
            # -> /
            :
            ^
            |
            Base array.

          The base array (which is dynamically resized as needed) contains pointers
          to blocks of photons.  Blocks of photons (of fixed size of a power of two)
          are allocated as needed.

          This mapping function converts a one-dimensional index and into a two-
          dimensional address consisting of a block index and an offset within
          that block.

          Note that, while this data structure allows easy allocation of photons,
          it does not easily permit deallocation of photons.  Once photons are placed
          into the photon map, they are not removed.
        */

        static unsigned int GetBlockId(unsigned int photonId);
        static unsigned int GetIndexInBlock(unsigned int photonId);

        Photon& GetPhoton(unsigned int blockId, unsigned int indexInBlock);
        const Photon& GetPhoton(unsigned int blockId, unsigned int indexInBlock) const;
};


typedef Photon* PhotonPtr;


/* ------------------------------------------------------ */
/* photon gatherer */
/* ------------------------------------------------------ */
class GatheredPhotons final
{
    public:
        // priority queue arrays
        Photon** photonGatherList;  // photons
        DBL *photonDistances;       // priorities
        int numFound;  // number of photons found

        void swapWith(GatheredPhotons& theOther);

        GatheredPhotons(int maxGatherCount);
        ~GatheredPhotons();
};

class PhotonGatherer final
{
    public:
        ScenePhotonSettings& photonSettings;
        PhotonMap *map;

        DBL size_sq_s;   // search radius squared
        DBL Size_s;      // search radius (static)
        DBL sqrt_dmax_s, dmax_s;      // dynamic search radius... current maximum
        int TargetNum_s; // how many to gather
        const Vector3d *pt_s;       // point around which we are gathering
        const Vector3d *norm_s;     // surface normal
        DBL flattenFactor; // amount to flatten the sphere to make it
                           // an ellipsoid when gathering photons
                           // zero = no flatten, one = regular
        bool gathered;
        DBL alreadyGatheredRadius;

        GatheredPhotons gatheredPhotons;

        PhotonGatherer(PhotonMap *map, ScenePhotonSettings& photonSettings);

        void gatherPhotonsRec(int start, int end);
        int gatherPhotons(const Vector3d* pt, DBL Size, DBL *r, const Vector3d* norm, bool flatten);
        DBL gatherPhotonsAdaptive(const Vector3d* pt, const Vector3d* norm, bool flatten);

        void PQInsert(Photon *photon, DBL d);
        void FullPQInsert(Photon *photon, DBL d);
};

class PhotonMediaFunction final : public MediaFunction
{
    public:
        PhotonMediaFunction(std::shared_ptr<SceneData> sd, TraceThreadData *td, Trace *t, PhotonGatherer *pg);

        void ComputeMediaAndDepositPhotons(MediaVector& medias, const Ray& ray, const Intersection& isect, MathColour& colour);
    protected:
        void DepositMediaPhotons(MathColour& colour, MediaVector& medias, LightSourceEntryVector& lights, MediaIntervalVector& mediaintervals,
                                 const Ray& ray, int minsamples, bool ignore_photons, bool use_scattering, bool all_constant_and_light_ray);
    private:
        std::shared_ptr<SceneData> sceneData;

        void addMediaPhoton(const Vector3d& Point, const Vector3d& Origin, const MathColour& LightCol, DBL depthDiff);
};

class PhotonTrace final : public Trace
{
    public:
        PhotonTrace(std::shared_ptr<SceneData> sd, TraceThreadData *td, const QualityFlags& qf, Trace::CooperateFunctor& cf);
        virtual ~PhotonTrace() override;

        virtual DBL TraceRay(Ray& ray, MathColour& colour, ColourChannel&, COLC weight, bool continuedRay, DBL maxDepth = 0.0) override;
    protected:
        virtual void ComputeLightedTexture(MathColour& LightCol, ColourChannel&, const TEXTURE *Texture, std::vector<const TEXTURE *>& warps, const Vector3d& ipoint, const Vector3d& rawnormal, Ray& ray, COLC weight, Intersection& isect) override;
        bool ComputeRefractionForPhotons(const FINISH* finish, Interior *interior, const Vector3d& ipoint, Ray& ray, const Vector3d& normal, const Vector3d& rawnormal, MathColour& colour, COLC weight);
        bool TraceRefractionRayForPhotons(const FINISH* finish, const Vector3d& ipoint, Ray& ray, Ray& nray, DBL ior, DBL n, const Vector3d& normal, const Vector3d& rawnormal, const Vector3d& localnormal, MathColour& colour, COLC weight);
    private:
        PhotonMediaFunction mediaPhotons;
        RadiosityFunctor noRadiosity;

        void addSurfacePhoton(const Vector3d& Point, const Vector3d& Origin, const MathColour& LightCol);
};

/* ------------------------------------------------------ */
/* photon map builder */
/* ------------------------------------------------------ */

class ShootingDirection final
{
    public:
        ShootingDirection(LightSource* light, ObjectPtr target):light(light),target(target) {}

        LightSource* light;
        ObjectPtr target;
        Vector3d up, left, ctr, toctr; /* vectors to determine direction of shot */
        DBL dist;                      /* distance from light to center of bounding sphere */
        DBL rad;                       /* radius of bounding sphere */

        void compute();
        void recomputeForAreaLight(Ray& ray, int area_x, int area_y);
    };


class LightTargetCombo final
{
    public:
        LightTargetCombo(LightSource *light, ObjectPtr target):light(light),target(target),shootingDirection(light,target) {}
        LightSource *light;
        ObjectPtr target;
        int estimate;
        DBL mintheta;
        DBL maxtheta;
        DBL dtheta;
        DBL photonSpread;
        ShootingDirection shootingDirection;

        int computeMergedFlags();
        void computeAnglesAndDeltas(std::shared_ptr<SceneData> sceneData);
};


class PhotonShootingUnit final
{
    public:
        PhotonShootingUnit(LightSource* light, ObjectPtr target):lightAndObject(light,target) {}
        LightTargetCombo lightAndObject;
};




class SinCosOptimizations final
{
    public:
        // speed optimization data - sin/cos stored in two arrays
        // these are only created if photon mapping is used
        // TODO move these to their own class
        // these are thread safe - used many times but not modified after initialization
        DBL *cosTheta;
        DBL *sinTheta;
        SinCosOptimizations();
        ~SinCosOptimizations();
};

extern SinCosOptimizations sinCosData;

/* ------------------------------------------------------ */
/* global functions */
/* for documentation of these functions, see photons.c */
/* ------------------------------------------------------ */
void ChooseRay(BasicRay &NewRay, const Vector3d& Normal, const Vector3d& Raw_Normal, int WhichRay);

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_PHOTONS_H
