//******************************************************************************
///
/// @file core/coretypes.h
///
/// Essential types for the render core.
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

#ifndef POVRAY_CORE_CORETYPES_H
#define POVRAY_CORE_CORETYPES_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"
#include "core/core_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <stack>
#include <string>
#include <vector>

// Boost header files
#include <boost/intrusive_ptr.hpp>

// POV-Ray header files (base module)
#include "base/colour.h"
#include "base/messenger.h"
#include "base/povassert.h"
#include "base/stringtypes.h"

// POV-Ray header files (core module)
#include "core/math/vector.h"
#include "core/render/ray_fwd.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCore
///
/// @{

using namespace pov_base;

// from <cmath>; we don't want to always type the namespace for these.
using std::abs;
using std::acos;
using std::asin;
using std::atan;
using std::atan2;
using std::ceil;
using std::cos;
using std::cosh;
using std::exp;
using std::fabs;
using std::floor;
using std::fmod;
using std::frexp;
using std::ldexp;
using std::log;
using std::log10;
using std::modf;
using std::pow;
using std::sin;
using std::sinh;
using std::sqrt;
using std::tan;
using std::tanh;

// Simple Scalar Square
template<typename T>
inline T Sqr(T a)
{
    return a * a;
}

struct Transform_Struct;
using TRANSFORM = Transform_Struct; ///< @deprecated

/// @}
///
//##############################################################################
///
/// @addtogroup PovCoreMaterialPattern
///
/// @{

struct Pattern_Struct;
struct Texture_Struct;
struct Pigment_Struct;
struct Tnormal_Struct;
struct Finish_Struct;

using TPATTERN  = Pattern_Struct;   ///< @deprecated
using TEXTURE   = Texture_Struct;   ///< @deprecated
using PIGMENT   = Pigment_Struct;   ///< @deprecated
using TNORMAL   = Tnormal_Struct;   ///< @deprecated
using FINISH    = Finish_Struct;    ///< @deprecated

typedef TEXTURE* TexturePtr;

/// @}
///
//##############################################################################
///
/// @addtogroup PovCoreMaterialMedia
///
/// @{

class Media final
{
    public:
        int Type;
        int Intervals;
        int Min_Samples;
        int Max_Samples;
        unsigned Sample_Method : 8;
        bool is_constant : 1;
        bool use_absorption : 1;
        bool use_emission : 1;
        bool use_extinction : 1;
        bool use_scattering : 1;
        bool ignore_photons : 1;
        DBL Jitter;
        DBL Eccentricity;
        DBL sc_ext;
        MathColour Absorption;
        MathColour Emission;
        MathColour Extinction;
        MathColour Scattering;

        DBL Ratio;
        DBL Confidence;
        DBL Variance;
        DBL *Sample_Threshold;

        DBL AA_Threshold;
        int AA_Level;

        std::vector<PIGMENT*> Density;

        Media();
        Media(const Media&);
        ~Media();

        Media& operator=(const Media&);

        void Transform(const TRANSFORM *trans);

        void PostProcess();
};

/// @}
///
//##############################################################################
///
/// @addtogroup PovCoreMaterialInterior
///
/// @{

class SubsurfaceInterior;
class Interior final
{
    public:
        int  hollow, Disp_NElems;
        SNGL IOR, Dispersion;
        SNGL Caustics, Old_Refract;
        SNGL Fade_Distance, Fade_Power;
        MathColour Fade_Colour;
        std::vector<Media> media;
        std::shared_ptr<SubsurfaceInterior> subsurface;

        Interior();
        Interior(const Interior&);
        ~Interior();

        void Transform(const TRANSFORM *trans);

        void PostProcess();
    private:

        Interior& operator=(const Interior&) = delete;
};

typedef std::shared_ptr<Interior> InteriorPtr;
typedef std::shared_ptr<const Interior> ConstInteriorPtr;

/// @}
///
//##############################################################################
///
/// @addtogroup PovCoreMaterialPattern
///
/// @{

struct BasicPattern;

typedef std::shared_ptr<BasicPattern> PatternPtr;
typedef std::shared_ptr<const BasicPattern> ConstPatternPtr;


struct Pattern_Struct
{
    unsigned short Type;
    unsigned short Flags;
    PatternPtr pattern;
};

/// @}
///
//##############################################################################
///
/// @addtogroup PovCore
///
/// @{

struct Material_Struct final
{
    TEXTURE *Texture;
    TEXTURE *Interior_Texture;
    InteriorPtr interior;
};
using MATERIAL = Material_Struct; ///< @deprecated

template<typename T>
class RefPool final
{
    public:
        RefPool() { }
        ~RefPool() { for(typename std::vector<T*>::iterator i(pool.begin()); i != pool.end(); i++) delete *i; pool.clear(); }

        T *alloc() { if(pool.empty()) return new T(); T *ptr(pool.back()); pool.pop_back(); return ptr; }
        void release(T *ptr) { pool.push_back(ptr); }
    private:
        std::vector<T*> pool;

        RefPool(const RefPool&) = delete;
        RefPool& operator=(RefPool&) = delete;
};

template<typename T>
struct RefClearDefault final
{
    void operator()(T&) { }
};

template<typename T>
struct RefClearContainer final
{
    void operator()(T& p) { p.clear(); }
};

template<typename T, class C = RefClearDefault<T>>
class Ref final
{
    public:
        Ref(RefPool<T>& p) : pool(p), ptr(p.alloc()) { }
        ~Ref() { C c; c(*ptr); pool.release(ptr); }

        T& operator*() { return *ptr; }
        const T& operator*() const { return *ptr; }

        T *operator->() { return ptr; }
        const T *operator->() const { return ptr; }
    private:
        RefPool<T>& pool;
        T *ptr;

        Ref() = delete;
        Ref(const Ref&) = delete;
        Ref& operator=(Ref&) = delete;
};

class ObjectDebugHelper final
{
    public:
        int Index;
        bool IsCopy;
        std::string Tag;

        ObjectDebugHelper() { Index = ObjectIndex++; IsCopy = false; }
        ObjectDebugHelper& operator=(const ObjectDebugHelper& src) { Index = ObjectIndex++; IsCopy = true; Tag = src.Tag; return *this; }

        std::string& SimpleDesc (std::string& result);
    private:
        static int ObjectIndex;
        ObjectDebugHelper(const ObjectDebugHelper& src) { Index = ObjectIndex++; IsCopy = true; Tag = src.Tag; }
};

typedef unsigned short HF_VAL;

/// @}
///
//##############################################################################
///
/// @addtogroup PovCoreRender
///
/// @{

/// Ray-object intersection data.
///
/// This class holds various information on a ray-object intersection.
///
class Intersection final
{
    public:

        /// Distance from the intersecting ray's origin.
        DBL Depth;
        /// Point of the intersection in global coordinate space.
        Vector3d IPoint;
        /// Unperturbed surface normal at the intersection point.
        /// @attention This is not necessarily the true geometric surface normal, as it may include fake smoothing.
        /// @note This value is invalid if haveNormal is false.
        /// @todo We should have two distinct vectors: A true geometric one, and a separate one for faked smoothing.
        Vector3d INormal;
        /// Perturbed normal vector (set during texture evaluation).
        Vector3d PNormal;
        /// UV texture coordinate.
        Vector2d Iuv;
        /// Intersected object.
        ObjectPtr Object;
        /// Root-level parent CSG object for cutaway textures.
        ObjectPtr Csg;

        /// @name Object-Specific Auxiliary Data
        /// These members hold information specific to particular object types, typically generated during
        /// intersection testing, to be re-used later for normal and/or UV coordinate computation.
        /// @note These values may be invalid or meaningless depending on the type of object intersected.
        /// @{

        /// Point of the intersection in local coordinate space (used by Blob and SpindleTorus)
        /// @note This value is invalid in Blob if haveLocalIPoint is false.
        Vector3d LocalIPoint;
        /// Generic auxiliary float data #1 (used by Prism, Lathe)
        DBL d1;
        /// Generic auxiliary pointer data (used by Mesh)
        const void *Pointer;
        /// Generic auxiliary integer data #1 (used by Sor, Prism, Isosurface, Lathe, Cones, Boxes)
        int i1;
        /// Generic auxiliary integer data #2 (used by Sor, Prism, Isosurface)
        int i2;
        /// Flag to indicate whether INormal was computed during intersection testing (used by HField)
        /// @note Objects either always or never computing INormal during intersection testing don't use this flag.
        bool haveNormal : 1;
        /// Flag to indicate whether LocalIPoint has already been computed.
        bool haveLocalIPoint : 1;
        /// Generic auxiliary boolean data #1 (used by SpindleTorus)
        bool b1 : 1;

        /// @}

        Intersection() :
            Depth(BOUND_HUGE), Object(nullptr), Csg(nullptr),
            d1(0.0), Pointer(nullptr), i1(0), i2(0), haveNormal(false), haveLocalIPoint(false), b1(false)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o) :
            Depth(d), IPoint(v), Iuv(v), Object(o), Csg(nullptr),
            d1(0.0), Pointer(nullptr), i1(0), i2(0), haveNormal(false), haveLocalIPoint(false), b1(false)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector3d& n, ObjectPtr o) :
            Depth(d), IPoint(v), INormal(n), Iuv(v), Object(o), Csg(nullptr),
            d1(0.0), Pointer(nullptr), i1(0), i2(0), haveNormal(true), haveLocalIPoint(false), b1(false)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector2d& uv, ObjectPtr o) :
            Depth(d), IPoint(v), Iuv(uv), Object(o), Csg(nullptr),
            d1(0.0), Pointer(nullptr), i1(0), i2(0), haveNormal(false), haveLocalIPoint(false), b1(false)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector3d& n, const Vector2d& uv, ObjectPtr o) :
            Depth(d), IPoint(v), INormal(n), Iuv(uv), Object(o), Csg(nullptr),
            d1(0.0), Pointer(nullptr), i1(0), i2(0), haveNormal(true), haveLocalIPoint(false), b1(false)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, const void *a) :
            Depth(d), IPoint(v), Iuv(v), Object(o), Csg(nullptr),
            d1(0.0), Pointer(a), i1(0), i2(0), haveNormal(false), haveLocalIPoint(false), b1(false)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector2d& uv, ObjectPtr o, const void *a) :
            Depth(d), IPoint(v), Iuv(uv), Object(o), Csg(nullptr),
            d1(0.0), Pointer(a), i1(0), i2(0), haveNormal(false), haveLocalIPoint(false), b1(false)
        {}

        /// @todo Why does this not set Iuv=IPoint, as other constructors do?
        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a) :
            Depth(d), IPoint(v), Object(o), Csg(nullptr),
            d1(0.0), Pointer(nullptr), i1(a), i2(0), haveNormal(false), haveLocalIPoint(false), b1(false)
        {}

        /// @todo Why does this not set Iuv=IPoint, as other constructors do?
        Intersection(DBL d, const Vector3d& v, ObjectPtr o, DBL a) :
            Depth(d), IPoint(v), Object(o), Csg(nullptr),
            d1(a), Pointer(nullptr), i1(0), i2(0), haveNormal(false), haveLocalIPoint(false), b1(false)
        {}

        /// @todo Why does this not set Iuv=IPoint, as other constructors do?
        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a, int b) :
            Depth(d), IPoint(v), Object(o), Csg(nullptr),
            d1(0.0), Pointer(nullptr), i1(a), i2(b), haveNormal(false), haveLocalIPoint(false), b1(false)
        {}

        /// @todo Why does this not set Iuv=IPoint, as other constructors do?
        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a, DBL b) :
            Depth(d), IPoint(v), Object(o), Csg(nullptr),
            d1(b), Pointer(nullptr), i1(a), i2(0), haveNormal(false), haveLocalIPoint(false), b1(false)
        {}

        /// @todo Why does this not set Iuv=IPoint, as other constructors do?
        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a, int b, DBL c) :
            Depth(d), IPoint(v), Object(o), Csg(nullptr),
            d1(c), Pointer(nullptr), i1(a), i2(b), haveNormal(false), haveLocalIPoint(false), b1(false)
        {}

        /// @todo Why does this not set Iuv=IPoint, as other constructors do?
        Intersection(DBL d, const Vector3d& v, ObjectPtr o, const Vector3d& lv, bool a) :
            Depth(d), IPoint(v), Object(o), Csg(nullptr),
            LocalIPoint(lv), d1(0.0), Pointer(nullptr), i1(0), i2(0), haveNormal(false), haveLocalIPoint(true), b1(a)
        {}

        ~Intersection() { }
};

typedef std::stack<Intersection, std::vector<Intersection>> IStackData;
typedef RefPool<IStackData> IStackPool;
typedef Ref<IStackData> IStack;

struct BasicRay
{
    Vector3d Origin;
    Vector3d Direction;

    inline BasicRay() {}
    inline BasicRay(const BasicRay& obj) : Origin(obj.Origin), Direction(obj.Direction) {}
    inline BasicRay(const Vector3d& ov, const Vector3d& dv) : Origin(ov), Direction(dv) {}
    inline Vector3d Evaluate(double depth) const { return Origin + Direction * depth; }

    /// Make sure the ray's direction is normalized.
    /// @return     The length of the direction vector before normalization.
    inline DBL Normalize() { DBL len = Direction.length(); Direction /= len; return len; }

    inline const Vector3d& GetOrigin() const { return Origin; }
    inline const Vector3d& GetDirection() const { return Direction; }

    inline Vector3d& GetOrigin() { return Origin; }
    inline Vector3d& GetDirection() { return Direction; }
};

struct TraceTicket;

struct RayObjectCondition
{
    virtual ~RayObjectCondition() {}
    virtual bool operator()(const Ray& ray, ConstObjectPtr object, DBL data) const = 0;
};

struct TrueRayObjectCondition final : public RayObjectCondition
{
    virtual bool operator()(const Ray&, ConstObjectPtr, DBL) const override { return true; }
};

struct PointObjectCondition
{
    virtual ~PointObjectCondition() {}
    virtual bool operator()(const Vector3d& point, ConstObjectPtr object) const = 0;
};

struct TruePointObjectCondition final : public PointObjectCondition
{
    virtual bool operator()(const Vector3d&, ConstObjectPtr) const override { return true; }
};

/// @}
///
//##############################################################################
///
/// @addtogroup PovCore
///
/// @{

struct FrameSettings final
{
    DBL Clock_Value;      // May change between frames of an animation
    int FrameNumber;      // May change between frames of an animation

    int InitialFrame;
    DBL InitialClock;

    int FinalFrame;
    int FrameNumWidth;
    DBL FinalClock;

    int SubsetStartFrame;
    DBL SubsetStartPercent;
    int SubsetEndFrame;
    DBL SubsetEndPercent;

    bool Field_Render_Flag;
    bool Odd_Field_Flag;
};

/// @}
///
//##############################################################################
///
/// @addtogroup PovCoreMaterialPattern
///
/// @{

class Fractal;

class FractalRules
{
    public:
        virtual ~FractalRules() {}
        virtual void CalcNormal (Vector3d&, int, const Fractal *, DBL **) const = 0;
        virtual bool Iterate (const Vector3d&, const Fractal *, DBL **) const = 0;
        virtual bool Iterate (const Vector3d&, const Fractal *, const Vector3d&, DBL *, DBL **) const = 0;
        virtual bool Bound (const BasicRay&, const Fractal *, DBL *, DBL *) const = 0;
};

typedef std::shared_ptr<FractalRules> FractalRulesPtr;

struct QualityFlags final
{
    bool ambientOnly    : 1;
    bool quickColour    : 1;
    bool shadows        : 1;
    bool areaLights     : 1;
    bool refractions    : 1;
    bool reflections    : 1;
    bool normals        : 1;
    bool media          : 1;
    bool radiosity      : 1;
    bool photons        : 1;
    bool subsurface     : 1;

    explicit QualityFlags(int level) :
        ambientOnly (level <= 1),
        quickColour (level <= 5),
        shadows     (level >= 4),
        areaLights  (level >= 5),
        refractions (level >= 6),
        reflections (level >= 8),
        normals     (level >= 8),
        media       (level >= 9),
        radiosity   (level >= 9),
        photons     (level >= 9),
        subsurface  (level >= 9)
    {}
};

/// @}
///
//##############################################################################
///
/// @defgroup PovCoreFunction User-Defined Functions
/// @ingroup PovCore
///
/// @{

class GenericFunctionContext
{
    public:
        virtual ~GenericFunctionContext() {}
};

class GenericFunctionContextFactory
{
    public:
        GenericFunctionContextFactory() : mRefCounter(0) {}
        virtual ~GenericFunctionContextFactory() {}
        virtual GenericFunctionContextPtr CreateFunctionContext(TraceThreadData* pTd) = 0;

    private:
        mutable size_t mRefCounter;
        friend void intrusive_ptr_add_ref(GenericFunctionContextFactory* f);
        friend void intrusive_ptr_release(GenericFunctionContextFactory* f);
};

inline void intrusive_ptr_add_ref(GenericFunctionContextFactory* f) { ++f->mRefCounter; }
inline void intrusive_ptr_release(GenericFunctionContextFactory* f) { if (!(--f->mRefCounter)) delete f; }

typedef boost::intrusive_ptr<GenericFunctionContextFactory> GenericFunctionContextFactoryIPtr;
typedef GenericFunctionContextFactory*                      GenericFunctionContextFactoryTPtr;

struct SourcePosition
{
    POV_LONG    line;
    POV_LONG    column;
    POV_OFF_T   offset;
    SourcePosition() = default;
    SourcePosition(const SourcePosition&) = default;
    SourcePosition(POV_LONG l, POV_LONG c, POV_OFF_T o) : line(l), column(c), offset(o) {}
};

struct SourceInfo : MessageContext
{
    UCS2String      fileName;
    SourcePosition  position;
    SourceInfo() = default;
    SourceInfo(const MessageContext& o) : fileName(o.GetFileName()), position(o.GetLine(), o.GetColumn(), o.GetOffset()) {}
    SourceInfo(const UCS2String& fn, const SourcePosition& p) : fileName(fn), position(p) {}
    virtual UCS2String GetFileName() const override { return fileName; }
    virtual POV_LONG GetLine() const override { return position.line; }
    virtual POV_LONG GetColumn() const override { return position.column; }
    virtual POV_OFF_T GetOffset() const override { return position.offset; }
};

struct CustomFunctionSourceInfo : SourceInfo
{
    UTF8String  name;
    CustomFunctionSourceInfo() = default;
    CustomFunctionSourceInfo(const UTF8String& n, const MessageContext& o) : name(n), SourceInfo(o) {}
};

template<typename RETURN_T, typename ARG_T>
class GenericCustomFunction
{
public:
    virtual ~GenericCustomFunction() {}
    virtual GenericFunctionContextPtr AcquireContext(TraceThreadData* pThreadData) = 0;
    virtual void ReleaseContext(GenericFunctionContextPtr pContext) = 0;
    virtual void InitArguments(GenericFunctionContextPtr pContext) = 0;
    virtual void PushArgument(GenericFunctionContextPtr pContext, ARG_T arg) = 0;
    virtual RETURN_T Execute(GenericFunctionContextPtr pContext) = 0;
    virtual GenericCustomFunction* Clone() const = 0;
    virtual const CustomFunctionSourceInfo* GetSourceInfo() const { return nullptr; }
};

typedef GenericCustomFunction<double, double> GenericScalarFunction;
typedef GenericScalarFunction* GenericScalarFunctionPtr;

template<typename RETURN_T, typename ARG_T>
class GenericCustomFunctionInstance final
{
public:
    inline GenericCustomFunctionInstance(GenericCustomFunction<RETURN_T,ARG_T>* pFn, TraceThreadData* pThreadData) :
        mpFunction(pFn), mpContext(pFn->AcquireContext(pThreadData)), mReInit(true)
    {
        POV_ASSERT(mpFunction != nullptr);
        POV_ASSERT(mpContext  != nullptr);
    }

    inline ~GenericCustomFunctionInstance()
    {
        mpFunction->ReleaseContext(mpContext);
    }

    inline void PushArgument(ARG_T arg)
    {
        if (mReInit)
        {
            mpFunction->InitArguments(mpContext);
            mReInit = false;
        }
        mpFunction->PushArgument(mpContext, arg);
    }

    inline RETURN_T Evaluate()
    {
        RETURN_T result = mpFunction->Execute(mpContext);
        mReInit = true;
        return result;
    }

    template<typename T1>
    inline RETURN_T Evaluate(T1 arg1)
    {
        PushArgument(arg1);
        return Evaluate();
    }

    template<typename T1, typename T2>
    inline RETURN_T Evaluate(T1 arg1, T2 arg2)
    {
        PushArgument(arg1);
        PushArgument(arg2);
        return Evaluate();
    }

    template<typename T1, typename T2, typename T3>
    inline RETURN_T Evaluate(T1 arg1, T2 arg2, T3 arg3)
    {
        PushArgument(arg1);
        PushArgument(arg2);
        PushArgument(arg3);
        return Evaluate();
    }

    inline RETURN_T Evaluate(const Vector2d& argV)
    {
        return Evaluate(argV.u(), argV.v());
    }

    inline RETURN_T Evaluate(const Vector3d& argV)
    {
        return Evaluate(argV.x(), argV.y(), argV.z());
    }

protected:
    GenericCustomFunction<RETURN_T,ARG_T>*  mpFunction;
    GenericFunctionContextPtr               mpContext;
    bool                                    mReInit;

private:

    GenericCustomFunctionInstance() = delete;
};

typedef GenericCustomFunctionInstance<double, double> GenericScalarFunctionInstance;
typedef GenericScalarFunctionInstance* GenericScalarFunctionInstancePtr;

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_CORETYPES_H
