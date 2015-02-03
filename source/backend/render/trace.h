//******************************************************************************
///
/// @file backend/render/trace.h
///
/// Declarations related to the `pov::Trace` class.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_BACKEND_TRACE_H
#define POVRAY_BACKEND_TRACE_H

#include <vector>

#include <boost/thread.hpp>

#include "backend/frame.h"
#include "backend/render/ray.h"
#include "backend/support/bsptree.h"
#include "backend/support/randomsequences.h"

namespace pov
{

typedef struct Fog_Struct FOG;
class PhotonGatherer;
class SceneData;
class Task;
class ViewData;

struct NoSomethingFlagRayObjectCondition : public RayObjectCondition
{
    virtual bool operator()(const Ray& ray, ConstObjectPtr object, double) const;
};

struct LitInterval
{
    bool lit;
    double s0, s1, ds;
    size_t l0, l1;

    LitInterval() :
        lit(false), s0(0.0), s1(0.0), ds(0.0), l0(0), l1(0) { }
    LitInterval(bool nlit, double ns0, double ns1, size_t nl0, size_t nl1) :
        lit(nlit), s0(ns0), s1(ns1), ds(ns1 - ns0), l0(nl0), l1(nl1) { }
};

struct MediaInterval
{
    bool lit;
    int samples;
    double s0, s1, ds;
    size_t l0, l1;
    AttenuatingColour od;
    LightColour te;
    PseudoColour te2;

    MediaInterval() :
        lit(false), samples(0), s0(0.0), s1(0.0), ds(0.0), l0(0), l1(0) { }
    MediaInterval(bool nlit, int nsamples, double ns0, double ns1, double nds, size_t nl0, size_t nl1) :
        lit(nlit), samples(nsamples), s0(ns0), s1(ns1), ds(nds), l0(nl0), l1(nl1) { }
    MediaInterval(bool nlit, int nsamples, double ns0, double ns1, double nds, size_t nl0, size_t nl1, const AttenuatingColour& nod, const LightColour& nte, const PseudoColour& nte2) :
        lit(nlit), samples(nsamples), s0(ns0), s1(ns1), ds(nds), l0(nl0), l1(nl1), od(nod), te(nte), te2(nte2) { }

    bool operator<(const MediaInterval& other) const { return (s0 < other.s0); }
};

struct LightSourceIntersectionEntry
{
    double s;
    size_t l;
    bool lit;

    LightSourceIntersectionEntry() :
        s(0.0), l(0), lit(false) { }
    LightSourceIntersectionEntry(double ns, size_t nl, bool nlit) :
        s(ns), l(nl), lit(nlit) { }

    bool operator<(const LightSourceIntersectionEntry& other) const { return (s < other.s); }
};

struct LightSourceEntry
{
    double s0, s1;
    LightSource *light;

    LightSourceEntry() :
        s0(0.0), s1(0.0), light(NULL) { }
    LightSourceEntry(LightSource *nlight) :
        s0(0.0), s1(0.0), light(nlight) { }
    LightSourceEntry(double ns0, double ns1, LightSource *nlight) :
        s0(ns0), s1(ns1), light(nlight) { }

    bool operator<(const LightSourceEntry& other) const { return (s0 < other.s0); }
};

// TODO: these sizes will need tweaking.
typedef FixedSimpleVector<Media *, MEDIA_VECTOR_SIZE> MediaVector; // TODO FIXME - cannot allow this to be fixed size [trf]
typedef FixedSimpleVector<MediaInterval, MEDIA_INTERVAL_VECTOR_SIZE> MediaIntervalVector; // TODO FIXME - cannot allow this to be fixed size [trf]
typedef FixedSimpleVector<LitInterval, LIT_INTERVAL_VECTOR_SIZE> LitIntervalVector; // TODO FIXME - cannot allow this to be fixed size [trf]
typedef FixedSimpleVector<LightSourceIntersectionEntry, LIGHT_INTERSECTION_VECTOR_SIZE> LightSourceIntersectionVector; // TODO FIXME - cannot allow this to be fixed size [trf]
typedef FixedSimpleVector<LightSourceEntry, LIGHTSOURCE_VECTOR_SIZE> LightSourceEntryVector; // TODO FIXME - cannot allow this to be fixed size [trf]


struct TraceTicket
{
    /// trace recursion level
    unsigned int traceLevel;

    /// maximum trace recursion level allowed
    unsigned int maxAllowedTraceLevel;

    /// maximum trace recursion level found
    unsigned int maxFoundTraceLevel;

    /// adc bailout
    double adcBailout;

    /// whether background should be rendered all transparent
    bool alphaBackground;

    /// something the radiosity algorithm needs
    unsigned int radiosityRecursionDepth;

    /// something the radiosity algorithm needs
    float radiosityImportanceQueried;
    /// something the radiosity algorithm needs
    float radiosityImportanceFound;
    /// set by radiosity code according to the sample quality encountered (1.0 is ideal, 0.0 really sucks)
    float radiosityQuality;

    /// something the subsurface scattering algorithm needs
    unsigned int subsurfaceRecursionDepth;

    TraceTicket(unsigned int mtl, double adcb, bool ab = true, unsigned int rrd = 0, unsigned int ssrd = 0,
                float riq = -1.0, float rq = 1.0):
        traceLevel(0), maxAllowedTraceLevel(mtl), maxFoundTraceLevel(0), adcBailout(adcb), alphaBackground(ab),
        radiosityRecursionDepth(rrd), subsurfaceRecursionDepth(ssrd), radiosityImportanceQueried(riq),
        radiosityImportanceFound(-1.0), radiosityQuality(rq)
    {}
};


/// Ray tracing and shading engine.
///
/// This class provides the fundamental functionality to trace rays and determine the effective colour of an object.
///
class Trace
{
    public:

        class CooperateFunctor
        {
            public:
                virtual void operator()() { }
        };

        class MediaFunctor
        {
            public:
                virtual void ComputeMedia(LightColour&, ColourChannel&, vector<Media>&, const Ray&, Intersection&) { }
                virtual void ComputeMedia(LightColour&, ColourChannel&, const RayInteriorVector&, const Ray&, Intersection&) { }
                virtual void ComputeMedia(LightColour&, ColourChannel&, MediaVector&, const Ray&, Intersection&) { }
        };

        class RadiosityFunctor
        {
            public:
                virtual void ComputeAmbient(LightColour& ambient_colour, const Vector3d& ipoint, const Vector3d& raw_normal, const Vector3d& layer_normal, double brilliance, double weight, TraceTicket& ticket) { }
                virtual bool CheckRadiosityTraceLevel(const TraceTicket& ticket) { return false; }
        };

        Trace(shared_ptr<SceneData> sd, TraceThreadData *td, const QualityFlags& qf,
              CooperateFunctor& cf, MediaFunctor& mf, RadiosityFunctor& af);

        virtual ~Trace();

        /// Trace a ray.
        ///
        /// Call this if transmittance matters.
        ///
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[out]     colour          Computed colour.
        /// @param[out]     transm          Computed transmittance.
        /// @param[in]      weight          Importance of this computation.
        /// @param[in]      continuedRay    Set to true when tracing a ray after it went through some surface
        ///                                 without a change in direction; this governs trace level handling.
        /// @param[in]      maxDepth        Objects at or beyond this distance won't be hit by the ray (ignored if
        ///                                 < EPSILON).
        /// @return                         The distance to the nearest object hit.
        ///
        virtual double TraceRay(Ray& ray, LightColour& colour, ColourChannel& transm, COLC weight, bool continuedRay, DBL maxDepth = 0.0);
/*
        /// Trace a ray.
        ///
        /// Call this if transmittance will be ignored anyway.
        ///
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[out]     colour          Computed colour.
        /// @param[in]      weight          Importance of this computation.
        /// @param[in]      continuedRay    Set to true when tracing a ray after it went through some surface
        ///                                 without a change in direction; this governs trace level handling.
        /// @param[in]      maxDepth        Objects at or beyond this distance won't be hit by the ray (ignored if
        ///                                 < EPSILON).
        /// @return                         The distance to the nearest object hit.
        ///
        virtual double TraceRay(Ray& ray, LightColour& colour, COLC weight, bool continuedRay, DBL maxDepth = 0.0);
*/
        bool FindIntersection(Intersection& isect, const Ray& ray);
        bool FindIntersection(Intersection& isect, const Ray& ray, const RayObjectCondition& precondition, const RayObjectCondition& postcondition);
        bool FindIntersection(ObjectPtr object, Intersection& isect, const Ray& ray, double closest = HUGE_VAL);
        bool FindIntersection(ObjectPtr object, Intersection& isect, const Ray& ray, const RayObjectCondition& postcondition, double closest = HUGE_VAL);

        unsigned int GetHighestTraceLevel();

        bool TestShadow(LightColour& colour, double& depth, const LightSource &light, Ray& light_source_ray, const Vector3d& p); // TODO FIXME - this should not be exposed here

    protected: // TODO FIXME - should be private

        /// Structure used to cache reflection information for multi-layered textures.
        struct WNRX
        {
            double weight;
            Vector3d normal;
            AttenuatingColour reflec;
            SNGL reflex;

            WNRX(DBL w, const Vector3d& n, const AttenuatingColour& r, SNGL x) :
                weight(w), normal(n), reflec(r), reflex(x) { }
        };

        typedef vector<const TEXTURE *> TextureVectorData;
        typedef RefPool<TextureVectorData> TextureVectorPool;
        typedef Ref<TextureVectorData, RefClearContainer<TextureVectorData> > TextureVector;

        typedef vector<WNRX> WNRXVectorData;
        typedef RefPool<WNRXVectorData> WNRXVectorPool;
        typedef Ref<WNRXVectorData, RefClearContainer<WNRXVectorData> > WNRXVector;

        /// Structure used to cache shadow test results for complex textures.
        struct LightColorCache
        {
            bool        tested;
            LightColour colour;
        };

        typedef vector<LightColorCache> LightColorCacheList;
        typedef vector<LightColorCacheList> LightColorCacheListList;

        /// List (well really vector) of lists of LightColorCaches.
        /// Each list is expected to have as many elements as there are global light sources.
        /// The number of lists should be at least that of max trace level.
        LightColorCacheListList lightColorCache;

        /// Current index into lightColorCaches.
        int lightColorCacheIndex;

        /// Scene data.
        shared_ptr<SceneData> sceneData;

        /// Maximum trace recursion level found.
        unsigned int maxFoundTraceLevel;
        /// Various quality-related flags.
        QualityFlags qualityFlags;

        /// Bounding slabs priority queue.
        BBoxPriorityQueue priorityQueue;
        /// BSP tree mailbox.
        BSPTree::Mailbox mailbox;
        /// Area light grid buffer.
        vector<LightColour> lightGrid;
        /// Fast stack pool.
        IStackPool stackPool;
        /// Fast texture list pool.
        TextureVectorPool texturePool;
        /// Fast WNRX list pool.
        WNRXVectorPool wnrxPool;
        /// Light source shadow cache for shadow tests of first trace level intersections.
        vector<ObjectPtr> lightSourceLevel1ShadowCache;
        /// Light source shadow cache for shadow tests of higher trace level intersections.
        vector<ObjectPtr> lightSourceOtherShadowCache;
        /// `crand` random number generator.
        unsigned int crandRandomNumberGenerator;
        /// Pseudo-random number sequence.
        RandomDoubleSequence randomNumbers;
        /// Pseudo-random number generator based on random number sequence.
        RandomDoubleSequence::Generator randomNumberGenerator;
        /// Sub-random uniform 3d points on sphere sequence.
        vector<SequentialVectorGeneratorPtr> ssltUniformDirectionGenerator;
        /// Sub-random uniform numbers sequence.
        vector<SequentialDoubleGeneratorPtr> ssltUniformNumberGenerator;
        /// Sub-random cos-weighted 3d points on hemisphere sequence.
        vector<SequentialVectorGeneratorPtr> ssltCosWeightedDirectionGenerator;
        /// Thread data.
        TraceThreadData *threadData;

        CooperateFunctor& cooperate;
        MediaFunctor& media;
        RadiosityFunctor& radiosity;

    ///
    //*****************************************************************************
    ///
    /// @name Texture Computations
    ///
    /// The following methods compute the effective colour of a given, possibly complex, texture.
    ///
    /// @{
    ///

        /// Compute the effective contribution of an intersection point as seen from the ray's origin, or deposits
        /// photons.
        ///
        /// Computations include any media effects between the ray's origin and the point of intersection.
        ///
        /// @remark         The computed contribution is _added_ to the values passed in `resultColour` and
        ///                 `resultTransm`.
        ///
        /// @todo           Some input parameters are non-const references.
        ///
        /// @param[in,out]  resultColour    Computed colour; undefined during photon pass.
        /// @param[in,out]  resultTransm    Computed transparency; not used during photon pass.
        /// @param[in]      lightColour     Light colour; only used during photon pass or shadow computations.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      weight          Importance of this computation.
        /// @param[in]      isect           Intersection information.
        /// @param[in]      photonpass      Whether to deposit photons instead of computing a colour
        ///
        void ComputeTextureColour(LightColour& resultColour, ColourChannel& resultTransm, const LightColour& lightColour, Ray& ray, COLC weight, Intersection& isect, bool photonpass);

        /// Compute the effective colour of an arbitrarily complex texture, or deposits photons.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `resultColour`.
        ///
        /// @remark         Computations do _not_ include media effects between the ray's origin and the point of
        ///                 intersection any longer.
        ///
        /// @todo           Some input parameters are non-const references or pointers.
        ///
        /// @param[out]     resultColour    Computed colour; undefined during photon pass.
        /// @param[out]     resultTransm    Computed transparency; not used during photon pass.
        /// @param[in]      lightColour     Light colour; only used during photon pass or shadow computations.
        /// @param[in]      texture         Texture.
        /// @param[in]      warps           Stack of warps to be applied.
        /// @param[in]      ipoint          Intersection point (possibly with earlier warps already applied).
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      weight          Importance of this computation.
        /// @param[in]      isect           Intersection information.
        /// @param[in]      shadowflag      Whether to perform only computations necessary for shadow testing.
        /// @param[in]      photonpass      Whether to deposit photons instead of computing a colour.
        ///
        void ComputeOneTextureColour(LightColour& resultColour, ColourChannel& resultTransm, const LightColour& lightColour, const TEXTURE *texture, vector<const TEXTURE *>& warps,
                                     const Vector3d& ipoint, const Vector3d& rawnormal, Ray& ray, COLC weight,
                                     Intersection& isect, bool shadowflag, bool photonpass);

        /// Compute the effective colour of an averaged texture, or deposits photons.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `resultColour`.
        ///
        /// @remark         Computations do _not_ include media effects between the ray's origin and the point of
        ///                 intersection any longer.
        ///
        /// @todo           Some input parameters are non-const references or pointers.
        ///
        /// @param[out]     resultColour    Computed colour; undefined during photon pass.
        /// @param[out]     resultTransm    Computed transparency; not used during photon pass.
        /// @param[in]      lightColour     Light colour; only used during photon pass or shadow computations.
        /// @param[in]      texture         Texture.
        /// @param[in]      warps           Stack of warps to be applied.
        /// @param[in]      ipoint          Intersection point (possibly with earlier warps already applied).
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      weight          Importance of this computation.
        /// @param[in]      isect           Intersection information.
        /// @param[in]      shadowflag      Whether to perform only computations necessary for shadow testing.
        /// @param[in]      photonpass      Whether to deposit photons instead of computing a colour.
        ///
        void ComputeAverageTextureColours(LightColour& resultColour, ColourChannel& resultTransm, const LightColour& lightColour, const TEXTURE *texture, vector<const TEXTURE *>& warps,
                                          const Vector3d& ipoint, const Vector3d& rawnormal, Ray& ray, COLC weight,
                                          Intersection& isect, bool shadowflag, bool photonpass);

        /// Compute the effective colour of a simple or layered texture.
        ///
        /// Computations include secondary rays.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `resultColour`.
        ///
        /// @remark         Computations do _not_ include media effects between the ray's origin and the point of
        ///                 intersection any longer.
        ///
        /// @remark         pov::PhotonTrace overrides this method to deposit photons instead.
        ///
        /// @todo           Some input parameters are non-const references or pointers.
        ///
        /// @param[out]     resultColour    Computed colour; undefined during photon pass.
        /// @param[out]     resultTransm    Computed transparency; not used during photon pass.
        /// @param[in]      lightColour     Light colour; only used during photon pass.
        /// @param[in]      texture         Texture.
        /// @param[in]      warps           Stack of warps to be applied.
        /// @param[in]      ipoint          Intersection point (possibly with earlier warps already applied).
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      weight          Importance of this computation.
        /// @param[in]      isect           Intersection information.
        ///
        virtual void ComputeLightedTexture(LightColour& resultColour, ColourChannel& resultTransm, const LightColour& lightColour, const TEXTURE *texture, vector<const TEXTURE *>& warps,
                                           const Vector3d& ipoint, const Vector3d& rawnormal, Ray& ray, COLC weight,
                                           Intersection& isect);

        /// Compute the effective filtering effect of a simple or layered texture.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `resultColour`.
        ///
        /// @remark         Computations do _not_ include media effects between the ray's origin and the point of
        ///                 intersection any longer.
        ///
        /// @todo           Some input parameters are non-const references or pointers.
        ///
        /// @param[out]     resultColour    Computed colour.
        /// @param[in]      lightColour     Light colour.
        /// @param[in]      texture         Texture.
        /// @param[in]      warps           Stack of warps to be applied.
        /// @param[in]      ipoint          Intersection point (possibly with earlier warps already applied).
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      isect           Intersection information.
        ///
        void ComputeShadowTexture(LightColour& resultColour, const LightColour& lightColour, const TEXTURE *texture, vector<const TEXTURE *>& warps,
                                  const Vector3d& ipoint, const Vector3d& rawnormal, const Ray& ray,
                                  Intersection& isect);

    ///
    /// @}
    ///
    //*****************************************************************************
    ///
    /// @name Reflection and Refraction Computations
    ///
    /// The following methods compute the contribution of secondary (reflected and refracted) rays.
    ///
    /// @{
    ///

        /// Compute the refraction contribution.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `colour`.
        ///
        /// @param[out]     colour          Computed colour.
        /// @param[in]      finish          Object's finish.
        /// @param[in]      ipoint          Intersection point.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[in]      weight          Importance of this computation.
        ///
        void ComputeReflection(LightColour& colour, const FINISH* finish, const Vector3d& ipoint, Ray& ray, const Vector3d& normal,
                               const Vector3d& rawnormal, COLC weight);

        /// Compute the refraction contribution.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `colour`.
        ///
        /// @param[out]     colour          Computed colour.
        /// @param[out]     transm          Computed transmittance.
        /// @param[in]      finish          Object's finish.
        /// @param[in]      interior        Stack of currently effective interiors.
        /// @param[in]      ipoint          Intersection point.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[in]      weight          Importance of this computation.
        /// @return                         `true` if total internal reflection _did_ occur.
        ///
        bool ComputeRefraction(LightColour& colour, ColourChannel& transm, const FINISH* finish, Interior *interior, const Vector3d& ipoint, Ray& ray,
                               const Vector3d& normal, const Vector3d& rawnormal, COLC weight);

        /// Compute the contribution of a single refracted ray.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `colour`.
        ///
        /// @param[out]     colour          Computed colour.
        /// @param[out]     transm          Computed transmittance.
        /// @param[in]      finish          object's finish.
        /// @param[in]      ipoint          Intersection point.
        /// @param[in,out]  ray             Original ray and associated information.
        /// @param[in,out]  nray            Refracted ray [out] and associated information [in,out].
        /// @param[in]      ior             Relative index of refraction.
        /// @param[in]      n               Cosine of angle of incidence.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[in]      localnormal     Effective surface normal, possibly flipped to match ray.
        /// @param[in]      weight          Importance of this computation.
        /// @return                         `true` if total internal reflection _did_ occur.
        ///
        bool TraceRefractionRay(LightColour& colour, ColourChannel& transm, const FINISH* finish, const Vector3d& ipoint, Ray& ray, Ray& nray, double ior, double n,
                                const Vector3d& normal, const Vector3d& rawnormal, const Vector3d& localnormal,
                                COLC weight);

    ///
    /// @}
    ///
    //*****************************************************************************
    ///
    /// @name Classic Light Source Computations
    ///
    /// The following methods compute the (additional) contribution of classic lighting.
    ///
    /// @{
    ///

        /// Compute the classic-illumination contribution of a finish for all light sources combined.
        ///
        /// @todo The name is misleading, as it computes all contributions of classic lighting, including highlights.
        ///
        /// @remark         The computed contribution is _added_ to the value passed in `colour`.
        ///
        /// @param[in,out]  colour          Effective surface colour.
        /// @param[in]      finish          Finish.
        /// @param[in]      ipoint          Intersection point.
        /// @param[in]      eyeRay          Ray from observer.
        /// @param[in]      layerNormal     Effective (possibly pertubed) surface normal.
        /// @param[in]      layerColour     Nominal pigment colour.
        /// @param[in]      attenuation     Attenuation factor to account for partial transparency.
        /// @param[in]      object          The object to compute for.
        ///
        void ComputeDiffuseLight(LightColour& colour, const FINISH *finish, const Vector3d& ipoint, const  Ray& eyeRay, const Vector3d& layerNormal, const AttenuatingColour& layerColour,
                                 double attenuation, ObjectPtr object, double relativeIor);

        /// Compute the classic-illumination contribution of a finish for a single light source.
        ///
        /// @todo The name is misleading, as it computes all contributions of classic lighting, including highlights.
        ///
        /// @remark         The computed contribution is _added_ to the value passed in `colour`.
        ///
        /// @param[in,out]  colour          Effective surface colour.
        /// @param[in]      lightSource     Light source.
        /// @param[in]      finish          Finish.
        /// @param[in]      ipoint          Intersection point.
        /// @param[in]      eyeRay          Ray from observer.
        /// @param[in]      layerNormal     Effective (possibly pertubed) surface normal.
        /// @param[in]      layerColour     Nominal pigment colour.
        /// @param[in]      attenuation     Attenuation factor to account for partial transparency.
        /// @param[in]      object          The object to compute for.
        ///
        void ComputeOneDiffuseLight(LightColour& colour, const LightSource &lightsource, const FINISH *finish, const Vector3d& ipoint, const Ray& eyeRay,
                                    const Vector3d& layerNormal, const AttenuatingColour& pigmentColour, double attenuation, ConstObjectPtr Object, double relativeIor, int light_index = -1);

        /// @todo The name is misleading, as it computes all contributions of classic lighting, including highlights.
        void ComputeFullAreaDiffuseLight(LightColour& colour, const LightColour& lightcolour, const LightSource &lightsource, const FINISH *finish, const Vector3d& ipoint, const Ray& eye,
                                         const Vector3d& layer_normal, const AttenuatingColour& layer_pigment_colour, double attenuation,
                                         double lightsourcedepth, Ray& lightsourceray,
                                         ConstObjectPtr object, double relativeIor); // JN2007: Full area lighting

        /// Compute the direction, distance and unshadowed brightness of an unshadowed light source.
        ///
        /// Computations include spotlight falloff and distance-based attenuation.
        ///
        /// @param[out]     lightColour         Effective brightness.
        /// @param[out]     lightSourceDepth    Distance to the light source.
        /// @param[in]      lightSource         Light source.
        /// @param[in,out]  lightSourceRay      Ray to the light source.
        /// @param[in]      ipoint              Intersection point.
        /// @param[in]      forceAttenuate      `true` to immediately apply distance-based attenuation even for full
        ///                                     area lights.
        ///
        void ComputeOneLightRay(LightColour& lightColour, double& lightSourceDepth, const LightSource &lightSource, Ray& lightSourceRay,
                                const Vector3d& ipoint, bool forceAttenuate = false);

        /// Compute the filtering effect of the scene on incident light from a particular light ray.
        ///
        /// @param[in,out]  colour              Incident light.
        /// @param[in]      lightSource         Light source.
        /// @param[in]      lightSourceDepth    Distance to the light source.
        /// @param[in,out]  lightSourceRay      Ray to the light source.
        /// @param[in]      ipoint              Intersection point.
        ///
        void TraceShadowRay(LightColour& colour, const LightSource &lightSource, double lightSourceDepth, Ray& lightSourceRay, const Vector3d& ipoint);

        /// @param[in,out]  colour              Incident light.
        /// @param[in]      lightSource         Light source.
        /// @param[in]      lightSourceDepth    Distance to the light source.
        /// @param[in,out]  lightSourceRay      Ray to the light source.
        ///
        void TracePointLightShadowRay(LightColour& colour, const LightSource &lightSource, double lightSourceDepth, Ray& lightSourceRay);

        /// @param[in,out]  colour              Incident light.
        /// @param[in]      lightSource         Light source.
        /// @param[in]      lightSourceDepth    Distance to the light source.
        /// @param[in,out]  lightSourceRay      Ray to the light source.
        ///
        void TraceAreaLightShadowRay(LightColour& colour, const LightSource &lightSource, double lightSourceDepth, Ray& lightSourceRay,
                                     const Vector3d& ipoint);

        /// @param[in,out]  colour              Incident light.
        /// @param[in]      lightSource         Light source.
        /// @param[in]      lightSourceDepth    Distance to the light source.
        /// @param[in,out]  lightSourceRay      Ray to the light source.
        /// @param[in]      ipoint              Intersection point.
        ///
        void TraceAreaLightSubsetShadowRay(LightColour& colour, const LightSource &lightSource, double lightSourceDepth, Ray& lightSourceRay,
                                           const Vector3d& ipoint, int u1, int  v1, int  u2, int  v2, int level, const Vector3d& axis1, const Vector3d& axis2);

        /// Compute the filtering effect of an object on incident light from a particular light source.
        ///
        /// Computations include any media effects between the ray's origin and the point of intersection.
        ///
        /// @todo           Some input parameters are non-const references.
        ///
        /// @param[in,out]  colour          Incident light.
        /// @param[in]      lightsource     Light source.
        /// @param[in]      isect           Intersection information.
        /// @param[in,out]  lightsourceray  Ray to the light source.
        ///
        void ComputeShadowColour(LightColour& colour, const LightSource &lightsource, Intersection& isect, Ray& lightsourceray);

        /// Compute the direction and distance of a single light source to a given intersection
        /// point.
        ///
        /// @remark         The `Origin` and `Direction` member of `lightsourceray` are updated, all other members are
        ///                 left unchanged; the distance is returned in a separate parameter. For cylindrical light
        ///                 sources, the values are set accordingly.
        ///
        /// @todo           The name is misleading, as it just computes direction and distance.
        ///
        /// @param[out]     lightsourcedepth    Distance to the light source.
        /// @param[in,out]  lightsourceray      Ray to the light source.
        /// @param[in]      lightsource         Light source.
        /// @param[in]      ipoint              Intersection point.
        /// @param[in]      jitter              Jitter to apply to the light source.
        ///
        void ComputeOneWhiteLightRay(BasicRay& lightsourceray, double& lightsourcedepth, const LightSource &lightsource,
                                     const Vector3d& ipoint, const Vector3d& jitter = Vector3d());

    ///
    /// @}
    ///
    //*****************************************************************************
    ///
    /// @name Photon Light Source Computations
    ///
    /// The following methods compute the (additional) contribution of photon-based lighting.
    ///
    /// @{
    ///

        /// @todo The name is misleading, as it computes all contributions of classic lighting, including highlights.
        void ComputePhotonDiffuseLight(LightColour& colour, const FINISH *Finish, const Vector3d& IPoint, const Ray& Eye, const Vector3d& Layer_Normal, const Vector3d& Raw_Normal,
                                       const AttenuatingColour& Layer_Pigment_Colour, double Attenuation,
                                       ConstObjectPtr Object, double relativeIor, PhotonGatherer& renderer);

    ///
    /// @}
    ///
    //*****************************************************************************
    ///
    /// @name Material Finish Computations
    ///
    /// The following methods compute the contribution of a finish illuminated by light from a given direction.
    ///
    /// @{
    ///

        /// Compute the classic contribution of a finish illuminated by light from a given direction.
        ///
        /// @remark         The computed contribution is _added_ to the value passed in `colour`.
        ///
        /// @param[in,out]  colour          Effective contribution.
        /// @param[in]      lightColour     Effective light colour.
        /// @param[in]      pigmentColour   Nominal pigment colour.
        /// @param[in]      fromEye         Direction from observer.
        /// @param[in]      toLight         Direction to light source.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      finish          Finish.
        /// @param[in]      ipoint          Intersection point (possibly with earlier warps already applied).
        /// @param[in]      attenuation     Attenuation factor to account for partial transparency.
        /// @param[in]      backside        Whether to use backside instead of frontside diffuse brightness factor.
        /// @param[in]      doHighlights    Whether to compute highlights. Set to `false` e.g. for fill lights and
        ///                                 during radiosity sampling.
        ///
        void ComputeClassicColour(LightColour& colour, const LightColour& lightColour, const AttenuatingColour& pigmentColour,
                                  const Vector3d& fromEye, const Vector3d& toLight, const Vector3d& normal,
                                  const FINISH *finish, const Vector3d& ipoint, double relativeIor, double attenuation, bool backside, bool doHighlights);

        /// Compute the diffuse contribution of a finish illuminated by light from a given direction.
        ///
        /// @remark         The computed contribution is _added_ to the value passed in `colour`.
        ///
        /// @param[in,out]  colour          Effective contribution.
        /// @param[in]      lightColour     Effective light colour.
        /// @param[in]      pigmentColour   Nominal pigment colour.
        /// @param[in]      fromEye         Direction from observer.
        /// @param[in]      toLight         Direction to light source.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      finish          Finish.
        /// @param[in]      attenuation     Attenuation factor to account for partial transparency.
        /// @param[in]      backside        Whether to use backside instead of frontside diffuse brightness factor.
        ///
        void ComputeDiffuseColour(LightColour& colour, const LightColour& lightColour, const AttenuatingColour& pigmentColour,
                                  const Vector3d& fromEye, const Vector3d& toLight, const Vector3d& normal,
                                  const FINISH *finish, double relativeIor, double attenuation, bool backside);

        /// Compute the Phong highlight contribution of a finish illuminated by light from a given direction.
        ///
        /// Computation uses the classic Phong highlight model.
        ///
        /// @remark         The model used is _not_ energy-conserving.
        ///
        /// @remark         The computed contribution is _added_ to the value passed in `colour`.
        ///
        /// @param[in,out]  colour          Effective contribution.
        /// @param[in]      lightColour     Effective light colour.
        /// @param[in]      pigmentColour   Nominal pigment colour.
        /// @param[in]      fromEye         Direction from observer.
        /// @param[in]      toLight         Direction to light source.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      finish          Finish.
        ///
        void ComputePhongColour(LightColour& colour, const LightColour& lightColour, const AttenuatingColour& pigmentColour,
                                const Vector3d& fromEye, const Vector3d& toLight, const Vector3d& normal,
                                const FINISH *finish, double relativeIor);

        /// Compute the specular highlight contribution of a finish illuminated by light from a given direction.
        ///
        /// Computation uses the Blinn-Phong highlight model
        ///
        /// @remark     The model used is _not_ energy-conserving.
        ///
        /// @remark     The computed contribution is _added_ to the value passed in `colour`.
        ///
        /// @param[in,out]  colour          Effective contribution.
        /// @param[in]      lightColour     Effective light colour.
        /// @param[in]      pigmentColour   Nominal pigment colour.
        /// @param[in]      fromEye         Direction from observer.
        /// @param[in]      toLight         Direction to light source.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      finish          Finish.
        ///
        void ComputeSpecularColour(LightColour& colour, const LightColour& lightColour, const AttenuatingColour& pigmentColour,
                                   const Vector3d& fromEye, const Vector3d& toLight, const Vector3d& normal,
                                   const FINISH *finish, double relativeIor);

        /// Compute the iridescence contribution of a finish illuminated by light from a given direction.
        ///
        /// @remark         The computed contribution _overwrites_ the value passed in `colour`.
        ///
        /// @param[in,out]  colour          Effective contribution.
        /// @param[in]      fromEye         Direction from observer.
        /// @param[in]      toLight         Direction to light source.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      finish          Finish.
        /// @param[in]      ipoint          Intersection point (possibly with earlier warps already applied).
        ///
        void ComputeIridColour(LightColour& colour,
                               const Vector3d& fromEye, const Vector3d& toLight, const Vector3d& normal,
                               const FINISH *finish, const Vector3d& ipoint);

    ///
    /// @}
    ///
    //*****************************************************************************
    ///

        /// Compute relative index of refraction.
        void ComputeRelativeIOR(const Ray& ray, const Interior* interior, double& ior);

        /// Compute Reflectivity.
        ///
        /// @remark         In Fresnel mode, light is presumed to be unpolarized on average, using
        ///                 @f$ R = \frac{1}{2} \left( R_s + R_p \right) @f$.
        ///
        void ComputeReflectivity(double& weight, AttenuatingColour& reflectivity, const AttenuatingColour& reflection_max,
                                 const AttenuatingColour& reflection_min, bool fresnel, double reflection_falloff,
                                 double cos_angle, double relativeIor);

        /// Compute metallic attenuation
        void ComputeMetallic(AttenuatingColour& colour, double metallic, const AttenuatingColour& metallicColour, double cosAngle);

        /// Compute fresnel-based reflectivity.
        void ComputeFresnel(AttenuatingColour& colour, const AttenuatingColour& rMax, const AttenuatingColour& rMin, double cos_angle, double relativeIor);

        /// Compute Sky & Background Colour.
        ///
        /// @remark         The computed colour _overwrites_ any value passed in `colour` and `transm`.
        ///
        /// @param[out]     colour          Computed sky/background colour.
        /// @param[out]     transm          Computed transmittance.
        /// @param[in]      ray             Ray.
        ///
        void ComputeSky(LightColour& colour, ColourChannel& transm, const Ray& ray);

        void ComputeFog(LightColour& colour, ColourChannel& transm, const Ray& ray, const Intersection& isect);
        double ComputeConstantFogDepth(const Ray &ray, double depth, double width, const FOG *fog);
        double ComputeGroundFogDepth(const Ray& ray, double depth, double width, const FOG *fog);
        void ComputeRainbow(LightColour& colour, ColourChannel& transm, const Ray& ray, const Intersection& isect);

        /// Compute media effect on traversing light rays.
        ///
        /// @note           This computes two things:
        ///                   - media and fog attenuation of the shadow ray (optional)
        ///                   - entry/exit of interiors
        ///                   .
        ///                 In other words, you can't skip this whole thing, because the entry/exit is important.
        ///
        void ComputeShadowMedia(LightColour& resultcolour, Ray& light_source_ray, Intersection& isect,
                                bool media_attenuation_and_interaction);

        /// Test whether an object is part of (or identical to) a given other object.
        ///
        /// @todo           The name is misleading, as the object to test against (`parent`) does not necessarily have
        ///                 to be a CSG compound object, but can actually be of any type. In that case, the function
        ///                 serves to test for identity.
        ///
        /// @param[in]      object          The object to test.
        /// @param[in]      parent          The object to test against.
        /// @return                         True if `object` is part of, or identical to, `parent`.
        ///
        bool IsObjectInCSG(ConstObjectPtr object, ConstObjectPtr parent);


    ///
    //*****************************************************************************
    ///
    /// @name Subsurface Light Transport
    ///
    /// The following methods implement the BSSRDF approximation as outlined by Jensen et al.
    ///
    /// @{
    ///

        double ComputeFt(double phi, double eta);
        void ComputeSurfaceTangents(const Vector3d& normal, Vector3d& u, Vector3d& v);
        void ComputeSSLTNormal (Intersection& Ray_Intersection);
        bool IsSameSSLTObject(ConstObjectPtr obj1, ConstObjectPtr obj2);
        void ComputeDiffuseSampleBase(Vector3d& basePoint, const Intersection& out, const Vector3d& vOut, double avgFreeDist, TraceTicket& ticket);
        void ComputeDiffuseSamplePoint(Intersection& in, double& sampleArea, const Vector3d& basePoint, TraceTicket& ticket);
        bool SSLTComputeRefractedDirection(const Vector3d& v, const Vector3d& n, double eta, Vector3d& refracted);
        void ComputeDiffuseContribution(double& sd, const Intersection& out, const Vector3d& vOut, const Vector3d& pIn, const Vector3d& nIn, const Vector3d& vIn, double sigma_prime_s, double sigma_a, double eta);
        void ComputeDiffuseContribution1(LightColour& Total_Colour, const LightSource& lightsource, const Intersection& out, const Vector3d& vOut, const Intersection& in, const PrecisePseudoColour& sigma_prime_s, const PrecisePseudoColour& sigma_a, double eta, double weight, TraceTicket& ticket);
        void ComputeDiffuseAmbientContribution1(LightColour& Total_Colour, const Intersection& out, const Vector3d& vOut, const Intersection& in, const PrecisePseudoColour& sigma_prime_s, const PrecisePseudoColour& sigma_a, double eta, double weight, TraceTicket& ticket);
        void ComputeOneSingleScatteringContribution(LightColour& Lo, const LightSource& lightsource, const Intersection& out, double sigma_t_xo, double sigma_s, double s_prime_out, double eta, const Vector3d& bend_point, double phi_out, double cos_out_prime, TraceTicket& ticket);
        void ComputeSingleScatteringContribution(LightColour& Lo, const Intersection& out, double dist, double theta_out, double cos_out_prime, const Vector3d& refractedREye, double sigma_t_xo, double sigma_s, double eta, TraceTicket& ticket);
        void ComputeSubsurfaceScattering(LightColour& colour, const FINISH *Finish, const AttenuatingColour& layer_pigment_colour, const Intersection& isect, Ray& Eye, const Vector3d& Layer_Normal, double Attenuation);

    ///
    /// @}
    ///
    //*****************************************************************************
    ///

};

}

#endif // POVRAY_BACKEND_TRACE_H
