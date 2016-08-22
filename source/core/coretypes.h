//******************************************************************************
///
/// @file core/coretypes.h
///
/// Essential types for the render core.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#include "core/configcore.h"

#include <stack>

#include "base/colour.h"
#include "base/types.h"
#include "base/textstream.h"

#include "core/math/vector.h"

namespace pov
{

using namespace pov_base;

// from <algorithm>; we don't want to always type the namespace for these.
using std::min;
using std::max;

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

/// @relates CoreMessenger
enum CoreMessageClass
{
    kCoreMessageClass_Debug,    ///< Diagnostic information to help in POV-Ray development.
    kCoreMessageClass_Info,     ///< Information that is no reason for alarm.
    kCoreMessageClass_Warning,  ///< Information about a potentially undesired and/or unexpected situation.
    // we don't have a value for errors, because those are signalled via exceptions.
};

/// Abstract class representing an object that can pass on messages from the core to the user.
///
/// @note
///     Existing implementations are currently not necessarily thread-safe.
///
class CoreMessenger
{
public:
    virtual ~CoreMessenger() {}
    virtual void CoreMessage(CoreMessageClass mc, const char *format,...) = 0;
    virtual void CoreMessageAt(CoreMessageClass mc, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...) = 0;
};

class ObjectBase;
typedef ObjectBase * ObjectPtr;
typedef const ObjectBase * ConstObjectPtr;

typedef struct Transform_Struct TRANSFORM;

//******************************************************************************
///
/// @name Blend Map Stuff
/// @{

#if 0
#pragma mark * Blend Map
#endif

typedef struct Pattern_Struct TPATTERN;
typedef struct Texture_Struct TEXTURE;
typedef struct Pigment_Struct PIGMENT;
typedef struct Tnormal_Struct TNORMAL;
typedef struct Finish_Struct FINISH;

typedef TEXTURE* TexturePtr;

/// @}
///

class Media
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
        AttenuatingColour Absorption;
        LightColour Emission;
        AttenuatingColour Extinction;
        AttenuatingColour Scattering;

        DBL Ratio;
        DBL Confidence;
        DBL Variance;
        DBL *Sample_Threshold;

        DBL AA_Threshold;
        int AA_Level;

        vector<PIGMENT*> Density;

        Media();
        Media(const Media&);
        ~Media();

        Media& operator=(const Media&);

        void Transform(const TRANSFORM *trans);

        void PostProcess();
};

class SubsurfaceInterior;
class Interior
{
    public:
        int  hollow, Disp_NElems;
        SNGL IOR, Dispersion;
        SNGL Caustics, Old_Refract;
        SNGL Fade_Distance, Fade_Power;
        AttenuatingColour Fade_Colour;
        vector<Media> media;
        shared_ptr<SubsurfaceInterior> subsurface;

        Interior();
        Interior(const Interior&);
        ~Interior();

        void Transform(const TRANSFORM *trans);

        void PostProcess();
    private:
        Interior& operator=(const Interior&);
};

typedef shared_ptr<Interior> InteriorPtr;
typedef shared_ptr<const Interior> ConstInteriorPtr;

struct BasicPattern;

typedef shared_ptr<BasicPattern> PatternPtr;
typedef shared_ptr<const BasicPattern> ConstPatternPtr;


struct Pattern_Struct
{
    unsigned short Type;
    unsigned short Flags;
    PatternPtr pattern;
};

typedef struct Material_Struct MATERIAL;

struct Material_Struct
{
    TEXTURE *Texture;
    TEXTURE *Interior_Texture;
    InteriorPtr interior;
};

class LightSource;

template<typename T>
class RefPool
{
    public:
        RefPool() { }
        ~RefPool() { for(typename vector<T*>::iterator i(pool.begin()); i != pool.end(); i++) delete *i; pool.clear(); }

        T *alloc() { if(pool.empty()) return new T(); T *ptr(pool.back()); pool.pop_back(); return ptr; }
        void release(T *ptr) { pool.push_back(ptr); }
    private:
        vector<T*> pool;

        RefPool(const RefPool&);
        RefPool& operator=(RefPool&);
};

template<typename T>
struct RefClearDefault
{
    void operator()(T&) { }
};

template<typename T>
struct RefClearContainer
{
    void operator()(T& p) { p.clear(); }
};

template<typename T, class C = RefClearDefault<T> >
class Ref
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

        Ref();
        Ref(const Ref&);
        Ref& operator=(Ref&);
};

class ObjectDebugHelper
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

/// Ray-object intersection data.
///
/// This class holds various information on a ray-object intersection.
///
class Intersection
{
    public:

        /// Distance from the intersecting ray's origin.
        DBL Depth;
        /// Point of the intersection in global coordinate space.
        Vector3d IPoint;
        /// Unpertubed surface normal at the intersection point.
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

        /// @name Object-Specific Auxiliary Data
        /// These members hold information specific to particular object types, typically generated during
        /// intersection testing, to be re-used later for normal and/or UV coordinate computation.
        /// @note These values may be invalid or meaningless depending on the type of object intersected.
        /// @{

        /// Point of the intersection in local coordinate space (used by Blob and SpindleTorus)
        /// @note This value is invalid in Blob if haveLocalIPoint is false.
        Vector3d LocalIPoint;
        /// Flag to indicate whether INormal was computed during intersection testing (used by HField)
        /// @note Objects either always or never computing INormal during intersection testing don't use this flag.
        bool haveNormal : 1;
        /// Flag to indicate whether LocalIPoint has already been computed.
        bool haveLocalIPoint : 1;
        /// Generic auxiliary boolean data #1 (used by SpindleTorus)
        bool b1 : 1;
        /// Generic auxiliary integer data #1 (used by Sor, Prism, Isosurface, Lathe, Cones, Boxes)
        int i1;
        /// Generic auxiliary integer data #2 (used by Sor, Prism, Isosurface)
        int i2;
        /// Generic auxiliary float data #1 (used by Prism, Lathe)
        DBL d1;
        /// Generic auxiliary pointer data (used by Mesh)
        const void *Pointer;

        /// @}

        /// Root-level parent CSG object for cutaway textures.
        ObjectPtr Csg;

        Intersection() :
            Depth(BOUND_HUGE), Object(NULL), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o) :
            Depth(d), Object(o), IPoint(v), Iuv(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector3d& n, ObjectPtr o) :
            Depth(d), Object(o), IPoint(v), Iuv(v), INormal(n), haveNormal(true), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector2d& uv, ObjectPtr o) :
            Depth(d), Object(o), IPoint(v), Iuv(uv), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector3d& n, const Vector2d& uv, ObjectPtr o) :
            Depth(d), Object(o), IPoint(v), INormal(n), Iuv(uv), haveNormal(true), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, const void *a) :
            Depth(d), Object(o), Pointer(a), IPoint(v), Iuv(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector2d& uv, ObjectPtr o, const void *a) :
            Depth(d), Object(o), Pointer(a), IPoint(v), Iuv(uv), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a) :
            Depth(d), Object(o), i1(a), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, DBL a) :
            Depth(d), Object(o), d1(a), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a, int b) :
            Depth(d), Object(o), i1(a), i2(b), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a, DBL b) :
            Depth(d), Object(o), i1(a), d1(b), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a, int b, DBL c) :
            Depth(d), Object(o), i1(a), i2(b), d1(c), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, const Vector3d& lv, bool a) :
            Depth(d), Object(o), b1(a), IPoint(v), LocalIPoint(lv), haveNormal(false), haveLocalIPoint(true), Csg(NULL)
        {}

        ~Intersection() { }
};

typedef std::stack<Intersection, vector<Intersection> > IStackData;
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

class Ray;

struct RayObjectCondition
{
    virtual bool operator()(const Ray& ray, ConstObjectPtr object, DBL data) const = 0;
};

struct TrueRayObjectCondition : public RayObjectCondition
{
    virtual bool operator()(const Ray&, ConstObjectPtr, DBL) const { return true; }
};

struct PointObjectCondition
{
    virtual bool operator()(const Vector3d& point, ConstObjectPtr object) const = 0;
};

struct TruePointObjectCondition : public PointObjectCondition
{
    virtual bool operator()(const Vector3d&, ConstObjectPtr) const { return true; }
};

struct FrameSettings
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

struct BYTE_XYZ
{
    unsigned char x, y, z;
};

inline void VUnpack(Vector3d& dest_vec, const BYTE_XYZ * pack_vec)
{
    dest_vec[X] = ((double)pack_vec->x * (1.0 / 255.0)) * 2.0 - 1.0;
    dest_vec[Y] = ((double)pack_vec->y * (1.0 / 255.0));
    dest_vec[Z] = ((double)pack_vec->z * (1.0 / 255.0)) * 2.0 - 1.0;

    dest_vec.normalize(); // already good to about 1%, but we can do better
}

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

typedef shared_ptr<FractalRules> FractalRulesPtr;

struct QualityFlags
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

class TraceThreadData;

class GenericFunctionContext
{
    public:
        virtual ~GenericFunctionContext() {}
};

typedef GenericFunctionContext* GenericFunctionContextPtr;

class GenericFunctionContextFactory
{
    public:
        virtual GenericFunctionContextPtr CreateFunctionContext(TraceThreadData* pTd) = 0;
        virtual ~GenericFunctionContextFactory() {}
};

struct FunctionSourceInfo
{
    char* name;
    UCS2* filename;
    pov_base::ITextStream::FilePos filepos;
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
    virtual const FunctionSourceInfo* GetSourceInfo() const { return NULL; }
};

typedef GenericCustomFunction<double, double> GenericScalarFunction;
typedef GenericScalarFunction* GenericScalarFunctionPtr;

template<typename RETURN_T, typename ARG_T>
class GenericCustomFunctionInstance
{
public:
    inline GenericCustomFunctionInstance(GenericCustomFunction<RETURN_T,ARG_T>* pFn, TraceThreadData* pThreadData) :
        mpFunction(pFn), mpContext(pFn->AcquireContext(pThreadData)), mReInit(true)
    {
        POV_ASSERT(mpFunction != NULL);
        POV_ASSERT(mpContext  != NULL);
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
    GenericCustomFunctionInstance();
};

typedef GenericCustomFunctionInstance<double, double> GenericScalarFunctionInstance;
typedef GenericScalarFunctionInstance* GenericScalarFunctionInstancePtr;

}

#endif // POVRAY_CORE_CORETYPES_H
