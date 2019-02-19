//******************************************************************************
///
/// @file core/render/trace.h
///
/// Declarations related to the @ref pov::Trace class.
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

#ifndef POVRAY_CORE_TRACE_H
#define POVRAY_CORE_TRACE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <vector>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/coretypes.h"
#include "core/bounding/bsptree.h"
#include "core/math/randomsequence.h"
#include "core/render/ray.h"
#include "core/scene/atmosphere_fwd.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreRender Ray Tracing
/// @ingroup PovCore
///
/// @{

class PhotonGatherer;

struct NoSomethingFlagRayObjectCondition final  : public RayObjectCondition
{
    virtual bool operator()(const Ray& ray, ConstObjectPtr object, double) const override;
};

struct LitInterval final
{
    bool lit;
    double s0, s1, ds;
    size_t l0, l1;

    LitInterval() :
        lit(false), s0(0.0), s1(0.0), ds(0.0), l0(0), l1(0) { }
    LitInterval(bool nlit, double ns0, double ns1, size_t nl0, size_t nl1) :
        lit(nlit), s0(ns0), s1(ns1), ds(ns1 - ns0), l0(nl0), l1(nl1) { }
};

struct MediaInterval final
{
    bool lit;
    int samples;
    double s0, s1, ds;
    size_t l0, l1;
    MathColour od;
    MathColour te;
    MathColour te2;

    MediaInterval() :
        lit(false), samples(0), s0(0.0), s1(0.0), ds(0.0), l0(0), l1(0) { }
    MediaInterval(bool nlit, int nsamples, double ns0, double ns1, double nds, size_t nl0, size_t nl1) :
        lit(nlit), samples(nsamples), s0(ns0), s1(ns1), ds(nds), l0(nl0), l1(nl1) { }
    MediaInterval(bool nlit, int nsamples, double ns0, double ns1, double nds, size_t nl0, size_t nl1, const MathColour& nod, const MathColour& nte, const MathColour& nte2) :
        lit(nlit), samples(nsamples), s0(ns0), s1(ns1), ds(nds), l0(nl0), l1(nl1), od(nod), te(nte), te2(nte2) { }

    bool operator<(const MediaInterval& other) const { return (s0 < other.s0); }
};

struct LightSourceIntersectionEntry final
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

struct LightSourceEntry final
{
    double s0, s1;
    LightSource *light;

    LightSourceEntry() :
        s0(0.0), s1(0.0), light(nullptr) { }
    LightSourceEntry(LightSource *nlight) :
        s0(0.0), s1(0.0), light(nlight) { }
    LightSourceEntry(double ns0, double ns1, LightSource *nlight) :
        s0(ns0), s1(ns1), light(nlight) { }

    bool operator<(const LightSourceEntry& other) const { return (s0 < other.s0); }
};

// TODO: these sizes will need tweaking.
typedef PooledSimpleVector<Media *, MEDIA_VECTOR_SIZE> MediaVector;
typedef PooledSimpleVector<MediaInterval, MEDIA_INTERVAL_VECTOR_SIZE> MediaIntervalVector;
typedef PooledSimpleVector<LitInterval, LIT_INTERVAL_VECTOR_SIZE> LitIntervalVector;
typedef PooledSimpleVector<LightSourceIntersectionEntry, LIGHT_INTERSECTION_VECTOR_SIZE> LightSourceIntersectionVector;
typedef PooledSimpleVector<LightSourceEntry, LIGHTSOURCE_VECTOR_SIZE> LightSourceEntryVector;


struct TraceTicket final
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

        /// @todo This interface might also come in hand at other places,
        /// so we should pull it out of the @ref Trace class.
        class CooperateFunctor
        {
            public:
                virtual ~CooperateFunctor() {}
                virtual void operator()() { }
        };

        class MediaFunctor
        {
            public:
                virtual ~MediaFunctor() {}
                virtual void ComputeMedia(std::vector<Media>&, const Ray&, Intersection&, MathColour&, ColourChannel&) { }
                virtual void ComputeMedia(const RayInteriorVector&, const Ray&, Intersection&, MathColour&, ColourChannel&) { }
                virtual void ComputeMedia(MediaVector&, const Ray&, Intersection&, MathColour&, ColourChannel&) { }
        };

        class RadiosityFunctor
        {
            public:
                virtual ~RadiosityFunctor() {}
                virtual void ComputeAmbient(const Vector3d& ipoint, const Vector3d& raw_normal, const Vector3d& layer_normal, double brilliance, MathColour& ambient_colour, double weight, TraceTicket& ticket) { }
                virtual bool CheckRadiosityTraceLevel(const TraceTicket& ticket) { return false; }
        };

        /// @todo TraceThreadData already holds a reference to SceneData.
        Trace(std::shared_ptr<SceneData> sd, TraceThreadData *td, const QualityFlags& qf,
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
        virtual double TraceRay(Ray& ray, MathColour& colour, ColourChannel& transm, COLC weight, bool continuedRay, DBL maxDepth = 0.0);
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
        virtual double TraceRay(Ray& ray, MathColour& colour, COLC weight, bool continuedRay, DBL maxDepth = 0.0);
*/
        bool FindIntersection(Intersection& isect, const Ray& ray);
        bool FindIntersection(Intersection& isect, const Ray& ray, const RayObjectCondition& precondition, const RayObjectCondition& postcondition);
        bool FindIntersection(ObjectPtr object, Intersection& isect, const Ray& ray, double closest = HUGE_VAL);
        bool FindIntersection(ObjectPtr object, Intersection& isect, const Ray& ray, const RayObjectCondition& postcondition, double closest = HUGE_VAL);

        unsigned int GetHighestTraceLevel();

        bool TestShadow(const LightSource &light, double& depth, Ray& light_source_ray, const Vector3d& p, MathColour& colour); // TODO FIXME - this should not be exposed here

    protected: // TODO FIXME - should be private

        /// Structure used to cache reflection information for multi-layered textures.
        struct WNRX final
        {
            double weight;
            Vector3d normal;
            MathColour reflec;
            SNGL reflex;

            WNRX(DBL w, const Vector3d& n, const MathColour& r, SNGL x) :
                weight(w), normal(n), reflec(r), reflex(x) { }
        };

        typedef std::vector<const TEXTURE*> TextureVectorData;
        typedef RefPool<TextureVectorData> TextureVectorPool;
        typedef Ref<TextureVectorData, RefClearContainer<TextureVectorData>> TextureVector;

        typedef std::vector<WNRX> WNRXVectorData;
        typedef RefPool<WNRXVectorData> WNRXVectorPool;
        typedef Ref<WNRXVectorData, RefClearContainer<WNRXVectorData>> WNRXVector;

        /// Structure used to cache shadow test results for complex textures.
        struct LightColorCache final
        {
            bool        tested;
            MathColour  colour;
        };

        typedef std::vector<LightColorCache> LightColorCacheList;
        typedef std::vector<LightColorCacheList> LightColorCacheListList;

        /// List (well really vector) of lists of LightColorCaches.
        /// Each list is expected to have as many elements as there are global light sources.
        /// The number of lists should be at least that of max trace level.
        LightColorCacheListList lightColorCache;

        /// Current index into lightColorCaches.
        int lightColorCacheIndex;

        /// Scene data.
        std::shared_ptr<SceneData> sceneData;

        /// Maximum trace recursion level found.
        unsigned int maxFoundTraceLevel;
        /// Various quality-related flags.
        QualityFlags qualityFlags;

        /// Bounding slabs priority queue.
        BBoxPriorityQueue priorityQueue;
        /// BSP tree mailbox.
        BSPTree::Mailbox mailbox;
        /// Area light grid buffer.
        std::vector<MathColour> lightGrid;
        /// Fast stack pool.
        IStackPool stackPool;
        /// Fast texture list pool.
        TextureVectorPool texturePool;
        /// Fast WNRX list pool.
        WNRXVectorPool wnrxPool;
        /// Light source shadow cache for shadow tests of first trace level intersections.
        std::vector<ObjectPtr> lightSourceLevel1ShadowCache;
        /// Light source shadow cache for shadow tests of higher trace level intersections.
        std::vector<ObjectPtr> lightSourceOtherShadowCache;
        /// `crand` random number generator.
        unsigned int crandRandomNumberGenerator;
        /// Pseudo-random number sequence.
        RandomDoubleSequence randomNumbers;
        /// Pseudo-random number generator based on random number sequence.
        RandomDoubleSequence::Generator randomNumberGenerator;
        /// Sub-random uniform 3d points on sphere sequence.
        std::vector<SequentialVectorGeneratorPtr> ssltUniformDirectionGenerator;
        /// Sub-random uniform numbers sequence.
        std::vector<SequentialDoubleGeneratorPtr> ssltUniformNumberGenerator;
        /// Sub-random cos-weighted 3d points on hemisphere sequence.
        std::vector<SequentialVectorGeneratorPtr> ssltCosWeightedDirectionGenerator;
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
        /// @remark         The computed contribution is _added_ to the value passed in `colour` (does not apply to
        ///                 photon pass).
        ///
        /// @todo           Some input parameters are non-const references.
        ///
        /// @param[in]      isect           Intersection information.
        /// @param[in,out]  resultColour    Computed colour [in,out]; during photon pass: light colour [in].
        /// @param[in,out]  resultTransm    Computed transparency; not used during photon pass.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      weight          Importance of this computation.
        /// @param[in]      photonpass      Whether to deposit photons instead of computing a colour
        ///
        void ComputeTextureColour(Intersection& isect, MathColour& resultColour, ColourChannel& resultTransm, Ray& ray, COLC weight, bool photonpass);

        /// Compute the effective colour of an arbitrarily complex texture, or deposits photons.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `colour` (does not apply to
        ///                 photon pass).
        ///
        /// @remark         Computations do _not_ include media effects between the ray's origin and the point of
        ///                 intersection any longer.
        ///
        /// @todo           Some input parameters are non-const references or pointers.
        ///
        /// @param[in,out]  resultColour    Computed colour [out]; during photon pass: light colour [in].
        /// @param[out]     resultTransm    Computed transparency; not used during photon pass.
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
        void ComputeOneTextureColour(MathColour& resultColour, ColourChannel& resultTransm, const TEXTURE *texture, std::vector<const TEXTURE *>& warps,
                                     const Vector3d& ipoint, const Vector3d& rawnormal, Ray& ray, COLC weight,
                                     Intersection& isect, bool shadowflag, bool photonpass);

        /// Compute the effective colour of an averaged texture, or deposits photons.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `colour` (does not apply to
        ///                 photon pass).
        ///
        /// @remark         Computations do _not_ include media effects between the ray's origin and the point of
        ///                 intersection any longer.
        ///
        /// @todo           Some input parameters are non-const references or pointers.
        ///
        /// @param[in,out]  resultColour    Computed colour [out]; during photon pass: light colour [in].
        /// @param[out]     resultTransm    Computed transparency; not used during photon pass.
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
        void ComputeAverageTextureColours(MathColour& resultColour, ColourChannel& resultTransm, const TEXTURE *texture, std::vector<const TEXTURE *>& warps,
                                          const Vector3d& ipoint, const Vector3d& rawnormal, Ray& ray, COLC weight,
                                          Intersection& isect, bool shadowflag, bool photonpass);

        /// Compute the effective colour of a simple or layered texture.
        ///
        /// Computations include secondary rays.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `colour`.
        ///
        /// @remark         Computations do _not_ include media effects between the ray's origin and the point of
        ///                 intersection any longer.
        ///
        /// @remark         pov::PhotonTrace overrides this method to deposit photons instead.
        ///
        /// @todo           Some input parameters are non-const references or pointers.
        ///
        /// @param[in,out]  resultColour    Computed colour [out]; during photon pass: light colour [in].
        /// @param[out]     resultTransm    Computed transparency; not used during photon pass.
        /// @param[in]      texture         Texture.
        /// @param[in]      warps           Stack of warps to be applied.
        /// @param[in]      ipoint          Intersection point (possibly with earlier warps already applied).
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      weight          Importance of this computation.
        /// @param[in]      isect           Intersection information.
        ///
        virtual void ComputeLightedTexture(MathColour& resultColour, ColourChannel& resultTransm, const TEXTURE *texture, std::vector<const TEXTURE *>& warps,
                                           const Vector3d& ipoint, const Vector3d& rawnormal, Ray& ray, COLC weight,
                                           Intersection& isect);

        /// Compute the effective filtering effect of a simple or layered texture.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `colour`.
        ///
        /// @remark         Computations do _not_ include media effects between the ray's origin and the point of
        ///                 intersection any longer.
        ///
        /// @todo           Some input parameters are non-const references or pointers.
        ///
        /// @param[out]     filtercolour    Computed filter colour.
        /// @param[in]      texture         Texture.
        /// @param[in]      warps           Stack of warps to be applied.
        /// @param[in]      ipoint          Intersection point (possibly with earlier warps already applied).
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      isect           Intersection information.
        ///
        void ComputeShadowTexture(MathColour& filtercolour, const TEXTURE *texture, std::vector<const TEXTURE *>& warps,
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
        /// @param[in]      finish          Object's finish.
        /// @param[in]      ipoint          Intersection point.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[out]     colour          Computed colour.
        /// @param[in]      weight          Importance of this computation.
        ///
        void ComputeReflection(const FINISH* finish, const Vector3d& ipoint, Ray& ray, const Vector3d& normal,
                               const Vector3d& rawnormal, MathColour& colour, COLC weight);

        /// Compute the refraction contribution.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `colour`.
        ///
        /// @param[in]      finish          Object's finish.
        /// @param[in]      interior        Stack of currently effective interiors.
        /// @param[in]      ipoint          Intersection point.
        /// @param[in,out]  ray             Ray and associated information.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[out]     colour          Computed colour.
        /// @param[out]     transm          Computed transmittance.
        /// @param[in]      weight          Importance of this computation.
        /// @return                         `true` if total internal reflection _did_ occur.
        ///
        bool ComputeRefraction(const FINISH* finish, Interior *interior, const Vector3d& ipoint, Ray& ray,
                               const Vector3d& normal, const Vector3d& rawnormal, MathColour& colour, ColourChannel& transm, COLC weight);

        /// Compute the contribution of a single refracted ray.
        ///
        /// @remark         The computed contribution _overwrites_ any value passed in `colour`.
        ///
        /// @param[in]      finish          object's finish.
        /// @param[in]      ipoint          Intersection point.
        /// @param[in,out]  ray             Original ray and associated information.
        /// @param[in,out]  nray            Refracted ray [out] and associated information [in,out].
        /// @param[in]      ior             Relative index of refraction.
        /// @param[in]      n               Cosine of angle of incidence.
        /// @param[in]      normal          Effective (possibly pertubed) surface normal.
        /// @param[in]      rawnormal       Geometric (possibly smoothed) surface normal.
        /// @param[in]      localnormal     Effective surface normal, possibly flipped to match ray.
        /// @param[out]     colour          Computed colour.
        /// @param[out]     transm          Computed transmittance.
        /// @param[in]      weight          Importance of this computation.
        /// @return                         `true` if total internal reflection _did_ occur.
        ///
        bool TraceRefractionRay(const FINISH* finish, const Vector3d& ipoint, Ray& ray, Ray& nray, double ior, double n,
                                const Vector3d& normal, const Vector3d& rawnormal, const Vector3d& localnormal,
                                MathColour& colour, ColourChannel& transm, COLC weight);

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

        /// @todo The name is misleading, as it computes all contributions of classic lighting, including highlights.
        void ComputeDiffuseLight(const FINISH *finish, const Vector3d& ipoint, const  Ray& eye, const Vector3d& layer_normal, const MathColour& layer_pigment_colour,
                                 MathColour& colour, double attenuation, ObjectPtr object, double relativeIor);
        /// @todo The name is misleading, as it computes all contributions of classic lighting, including highlights.
        void ComputeOneDiffuseLight(const LightSource &lightsource, const Vector3d& reye, const FINISH *finish, const Vector3d& ipoint, const Ray& eye,
                                    const Vector3d& layer_normal, const MathColour& Layer_Pigment_Colour, MathColour& colour, double Attenuation, ConstObjectPtr Object, double relativeIor, int light_index = -1);
        /// @todo The name is misleading, as it computes all contributions of classic lighting, including highlights.
        void ComputeFullAreaDiffuseLight(const LightSource &lightsource, const Vector3d& reye, const FINISH *finish, const Vector3d& ipoint, const Ray& eye,
                                         const Vector3d& layer_normal, const MathColour& layer_pigment_colour, MathColour& colour, double attenuation,
                                         double lightsourcedepth, Ray& lightsourceray, const MathColour& lightcolour,
                                         ConstObjectPtr object, double relativeIor); // JN2007: Full area lighting

        /// Compute the direction, distance and unshadowed brightness of an unshadowed light source.
        ///
        /// Computations include spotlight falloff and distance-based attenuation.
        ///
        /// @param[in]      lightsource         Light source.
        /// @param[out]     lightsourcedepth    Distance to the light source.
        /// @param[in,out]  lightsourceray      Ray to the light source.
        /// @param[in]      ipoint              Intersection point.
        /// @param[out]     lightcolour         Effective brightness.
        /// @param[in]      forceAttenuate      `true` to immediately apply distance-based attenuation even for full
        ///                                     area lights.
        ///
        void ComputeOneLightRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray,
                                const Vector3d& ipoint, MathColour& lightcolour, bool forceAttenuate = false);

        void TraceShadowRay(const LightSource &light, double depth, Ray& lightsourceray, const Vector3d& point, MathColour& colour);
        void TracePointLightShadowRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray, MathColour& lightcolour);
        void TraceAreaLightShadowRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray,
                                     const Vector3d& ipoint, MathColour& lightcolour);
        void TraceAreaLightSubsetShadowRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray,
                                           const Vector3d& ipoint, MathColour& lightcolour, int u1, int  v1, int  u2, int  v2, int level, const Vector3d& axis1, const Vector3d& axis2);

        /// Compute the filtering effect of an object on incident light from a particular light source.
        ///
        /// Computations include any media effects between the ray's origin and the point of intersection.
        ///
        /// @todo           Some input parameters are non-const references.
        ///
        /// @param[in]      lightsource     Light source.
        /// @param[in]      isect           Intersection information.
        /// @param[in,out]  lightsourceray  Ray to the light source.
        /// @param[in,out]  colour          Computed effect on the incident light.
        ///
        void ComputeShadowColour(const LightSource &lightsource, Intersection& isect, Ray& lightsourceray,
                                 MathColour& colour);

        /// Compute the direction and distance of a single light source to a given intersection
        /// point.
        ///
        /// @remark         The `Origin` and `Direction` member of `lightsourceray` are updated, all other members are
        ///                 left unchanged; the distance is returned in a separate parameter. For cylindrical light
        ///                 sources, the values are set accordingly.
        ///
        /// @todo           The name is misleading, as it just computes direction and distance.
        ///
        /// @param[in]      lightsource         Light source.
        /// @param[out]     lightsourcedepth    Distance to the light source.
        /// @param[in,out]  lightsourceray      Ray to the light source.
        /// @param[in]      ipoint              Intersection point.
        /// @param[in]      jitter              Jitter to apply to the light source.
        ///
        void ComputeOneWhiteLightRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray,
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
        void ComputePhotonDiffuseLight(const FINISH *Finish, const Vector3d& IPoint, const Ray& Eye, const Vector3d& Layer_Normal, const Vector3d& Raw_Normal,
                                       const MathColour& Layer_Pigment_Colour, MathColour& colour, double Attenuation,
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

        /// Compute the diffuse contribution of a finish illuminated by light from a given direction.
        ///
        /// @remark         The computed contribution is _added_ to the value passed in `colour`.
        ///
        /// @param[in]      finish                  Finish.
        /// @param[in]      lightDirection          Direction of incoming light.
        /// @param[in]      eyeDirection            Direction from observer.
        /// @param[in]      layer_normal            Effective (possibly pertubed) surface normal.
        /// @param[in,out]  colour                  Effective surface colour.
        /// @param[in]      light_colour            Effective light colour.
        /// @param[in]      layer_pigment_colour    Nominal pigment colour.
        /// @param[in]      attenuation             Attenuation factor to account for partial transparency.
        /// @param[in]      backside                Whether to use backside instead of frontside diffuse brightness
        ///                                         factor.
        ///
        void ComputeDiffuseColour(const FINISH *finish, const Vector3d& lightDirection, const Vector3d& eyeDirection, const Vector3d& layer_normal,
                                  MathColour& colour, const MathColour& light_colour,
                                  const MathColour& layer_pigment_colour, double relativeIor, double attenuation, bool backside);

        /// Compute the iridescence contribution of a finish illuminated by light from a given direction.
        ///
        /// @remark         The computed contribution is _added_ to the value passed in `colour`.
        ///
        /// @param[in]      finish          Finish.
        /// @param[in]      lightDirection  Direction of incoming light.
        /// @param[in]      eyeDirection    Direction from observer.
        /// @param[in]      layer_normal    Effective (possibly pertubed) surface normal.
        /// @param[in]      ipoint          Intersection point (possibly with earlier warps already applied).
        /// @param[in,out]  colour          Effective surface colour.
        ///
        void ComputeIridColour(const FINISH *finish, const Vector3d& lightDirection, const Vector3d& eyeDirection,
                               const Vector3d& layer_normal, const Vector3d& ipoint, MathColour& colour);

        /// Compute the Phong highlight contribution of a finish illuminated by light from a given direction.
        ///
        /// Computation uses the classic Phong highlight model.
        ///
        /// @remark         The model used is _not_ energy-conserving.
        ///
        /// @remark         The computed contribution is _added_ to the value passed in `colour`.
        ///
        /// @param[in]      finish                  Finish.
        /// @param[in]      lightDirection          Direction of ray from light source.
        /// @param[in]      eyeDirection            Direction from observer.
        /// @param[in]      layer_normal            Effective (possibly pertubed) surface normal.
        /// @param[in,out]  colour                  Effective surface colour.
        /// @param[in]      light_colour            Effective light colour.
        /// @param[in]      layer_pigment_colour    Nominal pigment colour.
        /// @param[in]      fresnel                 Whether to apply fresnel-based attenuation.
        ///
        void ComputePhongColour(const FINISH *finish, const Vector3d& lightDirection, const Vector3d& eyeDirection,
                                const Vector3d& layer_normal, MathColour& colour, const MathColour& light_colour,
                                const MathColour& layer_pigment_colour, double relativeIor);

        /// Compute the specular highlight contribution of a finish illuminated by light from a given direction.
        ///
        /// Computation uses the Blinn-Phong highlight model
        ///
        /// @remark     The model used is _not_ energy-conserving.
        ///
        /// @remark     The computed contribution is _added_ to the value passed in `colour`.
        ///
        /// @param[in]      finish                  Finish.
        /// @param[in]      lightDirection          Direction of ray from light source.
        /// @param[in]      eyeDirection            Direction from observer.
        /// @param[in]      layer_normal            Effective (possibly pertubed) surface normal.
        /// @param[in,out]  colour                  Effective surface colour.
        /// @param[in]      light_colour            Effective light colour.
        /// @param[in]      layer_pigment_colour    Nominal pigment colour.
        /// @param[in]      fresnel                 Whether to apply fresnel-based attenuation.
        ///
        void ComputeSpecularColour(const FINISH *finish, const Vector3d& lightDirection, const Vector3d& eyeDirection,
                                   const Vector3d& layer_normal, MathColour& colour, const MathColour& light_colour,
                                   const MathColour& layer_pigment_colour, double relativeIor);

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
        void ComputeReflectivity(double& weight, MathColour& reflectivity, const MathColour& reflection_max,
                                 const MathColour& reflection_min, bool fresnel, double reflection_falloff,
                                 double cos_angle, double relativeIor);

        /// Compute metallic attenuation
        void ComputeMetallic(MathColour& colour, double metallic, const MathColour& metallicColour, double cosAngle);

        /// Compute fresnel-based reflectivity.
        void ComputeFresnel(MathColour& colour, const MathColour& rMax, const MathColour& rMin, double cos_angle, double relativeIor);

        /// Compute Fresnel reflectance term.
        ///
        /// This function computes the reflectance term _R_ of the Fresnel equations for the special
        /// case of a dielectric material and unpolarized light. The transmittance term _T_ can
        /// trivially be computed as _T=1-R_.
        ///
        /// @param[in]      cosTi           Cosine of angle between incident ray and surface normal.
        /// @param[in]      n               Relative refractive index of the material entered.
        ///
        static double FresnelR(double cosTi, double n);

        /// Compute Sky & Background Colour.
        ///
        /// @remark         The computed colour _overwrites_ any value passed in `colour` and `transm`.
        ///
        /// @param[in]      ray             Ray.
        /// @param[out]     colour          Computed sky/background colour.
        /// @param[out]     transm          Computed transmittance.
        ///
        void ComputeSky(const Ray& ray, MathColour& colour, ColourChannel& transm);

        void ComputeFog(const Ray& ray, const Intersection& isect, MathColour& colour, ColourChannel& transm);
        double ComputeConstantFogDepth(const Ray &ray, double depth, double width, const FOG *fog);
        double ComputeGroundFogDepth(const Ray& ray, double depth, double width, const FOG *fog);
        void ComputeRainbow(const Ray& ray, const Intersection& isect, MathColour& colour, ColourChannel& transm);

        /// Compute media effect on traversing light rays.
        ///
        /// @note           This computes two things:
        ///                   - media and fog attenuation of the shadow ray (optional)
        ///                   - entry/exit of interiors
        ///                   .
        ///                 In other words, you can't skip this whole thing, because the entry/exit is important.
        ///
        void ComputeShadowMedia(Ray& light_source_ray, Intersection& isect, MathColour& resultcolour,
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
        void ComputeDiffuseSamplePoint(const Vector3d& basePoint, Intersection& in, double& sampleArea, TraceTicket& ticket);
        void ComputeDiffuseContribution(const Intersection& out, const Vector3d& vOut, const Vector3d& pIn, const Vector3d& nIn, const Vector3d& vIn, double& sd, double sigma_prime_s, double sigma_a, double eta);
        void ComputeDiffuseContribution1(const LightSource& lightsource, const Intersection& out, const Vector3d& vOut, const Intersection& in, MathColour& Total_Colour, const PreciseMathColour& sigma_prime_s, const PreciseMathColour& sigma_a, double eta, double weight, TraceTicket& ticket);
        void ComputeDiffuseAmbientContribution1(const Intersection& out, const Vector3d& vOut, const Intersection& in, MathColour& Total_Colour, const PreciseMathColour& sigma_prime_s, const PreciseMathColour& sigma_a, double eta, double weight, TraceTicket& ticket);
        void ComputeOneSingleScatteringContribution(const LightSource& lightsource, const Intersection& out, double sigma_t_xo, double sigma_s, double s_prime_out, MathColour& Lo, double eta, const Vector3d& bend_point, double phi_out, double cos_out_prime, TraceTicket& ticket);
        void ComputeSingleScatteringContribution(const Intersection& out, double dist, double theta_out, double cos_out_prime, const Vector3d& refractedREye, double sigma_t_xo, double sigma_s, MathColour& Lo, double eta, TraceTicket& ticket);
        void ComputeSubsurfaceScattering (const FINISH *Finish, const MathColour& layer_pigment_colour, const Intersection& isect, Ray& Eye, const Vector3d& Layer_Normal, MathColour& colour, double Attenuation);
        bool SSLTComputeRefractedDirection(const Vector3d& v, const Vector3d& n, double eta, Vector3d& refracted);

    ///
    /// @}
    ///
    //*****************************************************************************
    ///

};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_TRACE_H
