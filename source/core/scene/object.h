//******************************************************************************
///
/// @file core/scene/object.h
///
/// Declarations related to geometric shapes.
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

#ifndef POVRAY_CORE_OBJECT_H
#define POVRAY_CORE_OBJECT_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <vector>

// POV-Ray header files (base module)
#include "base/messenger_fwd.h"

// POV-Ray header files (core module)
#include "core/bounding/boundingbox.h"
#include "core/material/texture.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreShape Geometric Shapes
/// @ingroup PovCore
///
/// @{

//******************************************************************************
///
/// @name Object Flags
///
/// The flag field is used to store all possible flags that are
/// used for objects (up to 32).
///
/// The flages are manipulated using the following macros:
///
///     Set_Flag    (Object, Flag) : set    specified Flag in Object
///     Clear_Flag  (Object, Flag) : clear  specified Flag in Object
///     Invert_Flag (Object, Flag) : invert specified Flag in Object
///     Test_Flag   (Object, Flag) : test   specified Flag in Object
///
///     Copy_Flag   (Object1, Object2, Flag) : Set the Flag in Object1 to the
///                                            value of the Flag in Object2.
///     Bool_Flag   (Object, Flag, Bool)     : if(Bool) Set flag else Clear flag
///
/// Object is a pointer to the object.
/// Flag is the number of the flag to test.
///
/// @{

#define NO_SHADOW_FLAG            0x00000001L ///< Object doesn't cast shadows.
#define CLOSED_FLAG               0x00000002L ///< Object is closed.
#define INVERTED_FLAG             0x00000004L ///< Object is inverted.
#define SMOOTHED_FLAG             0x00000008L ///< Object is smoothed.
#define CYLINDER_FLAG             0x00000010L ///< Object is a cylinder.
#define DEGENERATE_FLAG           0x00000020L ///< Object is degenerate.
#define STURM_FLAG                0x00000040L ///< Object should use sturmian root solver.
#define OPAQUE_FLAG               0x00000080L ///< Object is opaque.
#define MULTITEXTURE_FLAG         0x00000100L ///< Object is multi-textured primitive.
#define INFINITE_FLAG             0x00000200L ///< Object is infinite.
#define HIERARCHY_FLAG            0x00000400L ///< Object can have a bounding hierarchy.
#define HOLLOW_FLAG               0x00000800L ///< Object is hollow (atmosphere inside).
#define HOLLOW_SET_FLAG           0x00001000L ///< Hollow explicitly set in scene file.
#define UV_FLAG                   0x00002000L ///< Object uses UV mapping. Set if `uv_mapping` is specified on an object itself.
#define DOUBLE_ILLUMINATE_FLAG    0x00004000L ///< Illuminate both sides of the surface.
#define NO_IMAGE_FLAG             0x00008000L ///< Object doesn't catch camera rays.
#define NO_REFLECTION_FLAG        0x00010000L ///< Object doesn't catch reflection rays.
#define NO_GLOBAL_LIGHTS_FLAG     0x00020000L ///< Object doesn't receive light from global lights.
#define NO_GLOBAL_LIGHTS_SET_FLAG 0x00040000L ///< Object doesn't receive light from global lights explicitly set in scene file. @bug This seems to be currently broken.
/* Photon-related flags */
#define PH_TARGET_FLAG            0x00080000L ///< Object is a photons target.
#define PH_PASSTHRU_FLAG          0x00100000L ///< Object is pass through object (i.e. it may let photons pass on their way to the target).
#define PH_RFL_ON_FLAG            0x00200000L ///< Object explicitly reflects photons.
#define PH_RFL_OFF_FLAG           0x00400000L ///< Object explicitly does not reflect photons.
#define PH_RFR_ON_FLAG            0x00800000L ///< Object explicitly refracts photons.
#define PH_RFR_OFF_FLAG           0x01000000L ///< Object explicitly does not refract photons.
#define PH_IGNORE_PHOTONS_FLAG    0x02000000L ///< Object does not collect photons.
#define IGNORE_RADIOSITY_FLAG     0x04000000L ///< Object doesn't receive ambient light from radiosity.
#define NO_RADIOSITY_FLAG         0x08000000L ///< Object doesn't catch radiosity rays (i.e. is invisible to radiosity).
#define CUTAWAY_TEXTURES_FLAG     0x10000000L ///< Object (or any of its parents) has cutaway_textures set.

#define Set_Flag(Object, Flag)     \
    { (Object)->Flags |=  (Flag); }

#define Clear_Flag(Object, Flag)   \
    { (Object)->Flags &= ~(Flag); }

#define Invert_Flag(Object, Flag)  \
    { (Object)->Flags ^=  (Flag); }

#define Test_Flag(Object, Flag)    \
    ( (Object)->Flags & (Flag))

#define Copy_Flag(Object1, Object2, Flag) \
    { (Object1)->Flags = (((Object1)->Flags) & (~Flag)) | \
                         (((Object2)->Flags) &  (Flag)); }

#define Bool_Flag(Object, Flag, Bool) \
    { if(Bool){ (Object)->Flags |=  (Flag); } else { (Object)->Flags &= ~(Flag); }}

/// @}
///
//******************************************************************************
///
/// @name Object Type Flags
///
/// The object type encodes various properties in a bit field.
///
/// @{

#define BASIC_OBJECT                0x0000u ///< Object has no special properties.
#define PATCH_OBJECT                0x0001u ///< Object has no inside, no inverse.
#define TEXTURED_OBJECT             0x0002u ///< Object has texture, possibly in children.
#define IS_COMPOUND_OBJECT          0x0004u ///< Object has children field.
#define STURM_OK_OBJECT             0x0008u ///< Object accepts the `sturm` parameter.
// 0x0010u currently not used
#define LIGHT_SOURCE_OBJECT         0x0020u ///< Object is to be linked in frame.light_sources.
// 0x0040u currently not used
// 0x0080u currently not used
#define IS_CHILD_OBJECT             0x0100u ///< Object is inside a compound.
#define HIERARCHY_OK_OBJECT         0x0200u ///< Object accepts the `hiararchy` parameter.
#define LT_SRC_UNION_OBJECT         0x0400u ///< Object is union of light source objects only.
#define LIGHT_GROUP_OBJECT          0x0800u ///< Object is light_group union object.
#define LIGHT_GROUP_LIGHT_OBJECT    0x1000u ///< Object is light in light_group object.
#define CSG_DIFFERENCE_OBJECT       0x2000u ///< Object is CSG difference object.
#define IS_CSG_OBJECT               0x4000u ///< Object is a CSG and not some other compound object.
#define POTENTIAL_OBJECT            0x8000u ///< Object has an intrinsic potential field associated.

#define CHILDREN_FLAGS (PATCH_OBJECT+TEXTURED_OBJECT)  ///< Reverse inherited flags.

/// @}
///
//******************************************************************************

/// Abstract base class for all geometric objects.
class ObjectBase
{
    public:

        int Type; // TODO - make obsolete
        TEXTURE *Texture;
        TEXTURE *Interior_Texture;
        InteriorPtr interior;
        std::vector<ObjectPtr> Bound;
        std::vector<ObjectPtr> Clip;
        std::vector<LightSource*> LLights;  ///< Used for light groups.
        BoundingBox BBox;
        TRANSFORM *Trans;
        SNGL Ph_Density;
        double RadiosityImportance;
        bool RadiosityImportanceSet;
        unsigned int Flags;

#ifdef OBJECT_DEBUG_HELPER
        ObjectDebugHelper Debug;
#endif

        /// Construct object from scratch.
        ObjectBase(int t) :
            Type(t),
            Texture(nullptr), Interior_Texture(nullptr), interior(), Trans(nullptr),
            Ph_Density(0), RadiosityImportance(0.0), RadiosityImportanceSet(false), Flags(0)
        {
            Make_BBox(BBox, -BOUND_HUGE/2.0, -BOUND_HUGE/2.0, -BOUND_HUGE/2.0, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
        }

        /// Construct object as copy of existing one.
        ///
        /// @param[in]  t           Object type.
        /// @param[in]  o           Object to copy data from.
        /// @param[in]  transplant  Whether to move data rather than copy it.
        ///
        ObjectBase(int t, ObjectBase& o, bool transplant) :
            Type(t),
            Texture(o.Texture), Interior_Texture(o.Interior_Texture), interior(o.interior), Trans(o.Trans),
            Ph_Density(o.Ph_Density), RadiosityImportance(o.RadiosityImportance),
            RadiosityImportanceSet(o.RadiosityImportanceSet), Flags(o.Flags),
            Bound(o.Bound), Clip(o.Clip), LLights(o.LLights), BBox(o.BBox)
        {
            if (transplant)
            {
                o.Texture = nullptr;
                o.Interior_Texture = nullptr;
                o.interior.reset();
                o.Trans = nullptr;
                o.Bound.clear();
                o.Clip.clear();
                o.LLights.clear();
            }
        }
        virtual ~ObjectBase();

        virtual ObjectPtr Copy() = 0;

        /// Test the object parameters and precompute derived values.
        ///
        /// @return True if object parameters are within reasonable limits.
        ///
        virtual bool Precompute() { return true; }

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) = 0; // could be "const", if it wasn't for isosurface max_gradient estimation stuff
        virtual double GetPotential (const Vector3d&, bool subtractThreshold, TraceThreadData *) const;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const = 0;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const = 0;
        virtual void UVCoord(Vector2d&, const Intersection *) const;
        virtual void Translate(const Vector3d&, const TRANSFORM *) = 0;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) = 0;
        virtual void Scale(const Vector3d&, const TRANSFORM *) = 0;
        virtual void Transform(const TRANSFORM *) = 0;

        /// Invert the object.
        ///
        /// @attention  This method may return a newly constructed object and destroy itself.
        ///
        virtual ObjectPtr Invert();

        virtual void Compute_BBox() = 0;
        virtual void Determine_Textures(Intersection *, bool, WeightedTextureVector&, TraceThreadData *Thread); // could be "(const Intersection*...) const" if it wasn't for blob specials

        /// Checks whether a given ray intersects the object's bounding box.
        /// Primitives with low-cost intersection tests may override this to always return true
        virtual bool Intersect_BBox(BBoxDirection, const BBoxVector3d&, const BBoxVector3d&, BBoxScalar = HUGE_VAL) const;

        /// Optional post-render message dispatcher.
        ///
        /// This method will be called upon completion of rendering a view. This is the appropriate
        /// place to send messages that would traditionally have been sent at the end of a render or
        /// at object destruction - e.g. IsoSurface max_gradient warnings. (object destruction isn't
        /// the place to do that anymore since a scene may persist between views).
        ///
        virtual void DispatchShutdownMessages(GenericMessenger& messenger) {};

        /// Test texture for opacity.
        ///
        /// This method will be called by the parser as part of object post-processing,
        /// to test whether the object's material is guaranteed to be fully opaque.
        ///
        /// The default implementation reports the object as opaque if if has a texture that is
        /// guaranteed to be opaque (as determined by @ref Test_Opacity()), and it has either no
        /// explicit interior texture or that texture is also guaranteed to be opaque.
        ///
        /// Primitives with innate textures (such as blob or mesh) must override this method, and
        /// return false if any of their innate textures is potentially non-opaque.
        ///
        virtual bool IsOpaque() const;

    protected:

        explicit ObjectBase(const ObjectBase&) { }
};

/// Convenience class to derive patch objects from.
class NonsolidObject : public ObjectBase
{
    public:
        NonsolidObject(int t) : ObjectBase(t) {}
        virtual ObjectPtr Invert() override;
};

/// Abstract base class for compound geometric objects.
///
/// @note   Some special compound objects (such as mesh or blob) do _not_ derive from this class.
///
class CompoundObject : public ObjectBase
{
    public:
        CompoundObject(int t) : ObjectBase(t) {}
        CompoundObject(int t, CompoundObject& o, bool transplant) : ObjectBase(t, o, transplant), children(o.children) { if (transplant) o.children.clear(); }
        std::vector<ObjectPtr> children;
        virtual ObjectPtr Invert() override;
};


/// Light source.
/// @ingroup PovCoreLightingLightsource
class LightSource final : public CompoundObject
{
    public:
        size_t index;
        MathColour colour;
        Vector3d Direction, Center, Points_At, Axis1, Axis2;
        DBL Coeff, Radius, Falloff;
        DBL Fade_Distance, Fade_Power;
        int Area_Size1, Area_Size2;
        int Adaptive_Level;
        ObjectPtr Projected_Through_Object;

        unsigned Light_Type : 8;
        bool Area_Light : 1;
        bool Use_Full_Area_Lighting : 1; // JN2007: Full area lighting
        bool Jitter : 1;
        bool Orient : 1;
        bool Circular : 1;
        bool Parallel : 1;
        bool Photon_Area_Light : 1;
        bool Media_Attenuation : 1;
        bool Media_Interaction : 1;
        bool lightGroupLight : 1;

        LightSource();
        virtual ~LightSource() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void UVCoord(Vector2d&, const Intersection *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override {}
};


/// Generic abstract class used for containing inherently infinite objects (isosurface, parametric).
struct ContainedByShape
{
    virtual ~ContainedByShape() {}

    virtual void ComputeBBox(BoundingBox& rBox) const = 0;
    virtual bool Intersect(const Ray& ray, const TRANSFORM* pTrans, DBL& rDepth1, DBL& rDepth2, int& rSide1, int& sSide2) const = 0;
    virtual bool Inside(const Vector3d& IPoint) const = 0;
    virtual void Normal(const Vector3d& IPoint, const TRANSFORM* pTrans, int side, Vector3d& rNormal) const = 0;
    virtual ContainedByShape* Copy() const = 0;
};

/// Class used for containing inherently infinite objects (isosurface, parametric) in a box.
struct ContainedByBox final : public ContainedByShape
{
    Vector3d corner1;
    Vector3d corner2;

    ContainedByBox() : corner1(-1,-1,-1), corner2(1,1,1) {}

    virtual void ComputeBBox(BoundingBox& rBox) const override;
    virtual bool Intersect(const Ray& ray, const TRANSFORM* pTrans, DBL& rDepth1, DBL& rDepth2, int& rSide1, int& sSide2) const override;
    virtual bool Inside(const Vector3d& IPoint) const override;
    virtual void Normal(const Vector3d& IPoint, const TRANSFORM* pTrans, int side, Vector3d& rNormal) const override;
    virtual ContainedByShape* Copy() const override;
};

/// Class used for containing inherently infinite objects (isosurface, parametric) in a sphere.
struct ContainedBySphere final : public ContainedByShape
{
    Vector3d center;
    DBL radius;

    ContainedBySphere() : center(0,0,0), radius(1) {}

    virtual void ComputeBBox(BoundingBox& rBox) const override;
    virtual bool Intersect(const Ray& ray, const TRANSFORM* pTrans, DBL& rDepth1, DBL& rDepth2, int& rSide1, int& sSide2) const override;
    virtual bool Inside(const Vector3d& IPoint) const override;
    virtual void Normal(const Vector3d& IPoint, const TRANSFORM* pTrans, int side, Vector3d& rNormal) const override;
    virtual ContainedByShape* Copy() const override;
};

bool Find_Intersection(Intersection *Ray_Intersection, ObjectPtr Object, const Ray& ray, TraceThreadData *Thread);
bool Find_Intersection(Intersection *Ray_Intersection, ObjectPtr Object, const Ray& ray, const RayObjectCondition& postcondition, TraceThreadData *Thread);
bool Find_Intersection(Intersection *isect, ObjectPtr object, const Ray& ray, BBoxDirection variant, const BBoxVector3d& origin, const BBoxVector3d& invdir, TraceThreadData *ThreadData);
bool Find_Intersection(Intersection *isect, ObjectPtr object, const Ray& ray, BBoxDirection variant, const BBoxVector3d& origin, const BBoxVector3d& invdir, const RayObjectCondition& postcondition, TraceThreadData *ThreadData);
bool Ray_In_Bound(const Ray& ray, const std::vector<ObjectPtr>& Bounding_Object, TraceThreadData *Thread);
bool Point_In_Clip(const Vector3d& IPoint, const std::vector<ObjectPtr>& Clip, TraceThreadData *Thread);
ObjectPtr Copy_Object(ObjectPtr Old);
std::vector<ObjectPtr> Copy_Objects(std::vector<ObjectPtr>& Src);
void Translate_Object(ObjectPtr Object, const Vector3d& Vector, const TRANSFORM *Trans);
void Rotate_Object(ObjectPtr Object, const Vector3d& Vector, const TRANSFORM *Trans);
void Scale_Object(ObjectPtr Object, const Vector3d& Vector, const TRANSFORM *Trans);
void Transform_Object(ObjectPtr Object, const TRANSFORM *Trans);
bool Inside_Object(const Vector3d& IPoint, ObjectPtr Object, TraceThreadData *Thread);
void Destroy_Object(std::vector<ObjectPtr>& Object);
void Destroy_Object(ObjectPtr Object);
void Destroy_Single_Object(ObjectPtr *ObjectPtr);

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_OBJECT_H
