//******************************************************************************
///
/// @file core/shape/isosurface.cpp
///
/// Implementation of the isosurface geometric primitive.
///
/// @author D.Skarda, T.Bily (original code)
/// @author R.Suzuki (modifications)
/// @author Thorsten Froehlich (porting to POV-Ray v3.5)
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
#include "core/shape/isosurface.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
#include "base/messenger.h"

// POV-Ray header files (core module)
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::min;
using std::max;

struct IsosurfaceCache final
{
    const IsoSurface *current;
    Vector3d Pglobal;
    Vector3d Dglobal;
    DBL fmax;
    IsosurfaceCache();
};

IsosurfaceCache::IsosurfaceCache() :
    current(nullptr)
{}

struct ISO_ThreadData final
{
    IsosurfaceCache cache;
    GenericScalarFunctionInstance* pFn;
    DBL Vlength;
    DBL tl;
    int Inv3;
};

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

struct ISO_Max_Gradient final
{
    DBL max_gradient, gradient;
    DBL eval_max, eval_cnt, eval_gradient_sum, eval_var;
    bool reported;

    ISO_Max_Gradient() :
        max_gradient(0.0),
        gradient(0.0),
        eval_max(0.0),
        eval_cnt(0.0),
        eval_gradient_sum(0.0),
        eval_var(0.0),
        reported(false),
        mRefCounter(0)
    {}

    bool IsShared() const { return mRefCounter > 1; }

private:
    mutable size_t mRefCounter;
    friend void intrusive_ptr_add_ref(ISO_Max_Gradient* f);
    friend void intrusive_ptr_release(ISO_Max_Gradient* f);
};

inline void intrusive_ptr_add_ref(ISO_Max_Gradient* f) { ++f->mRefCounter; }
inline void intrusive_ptr_release(ISO_Max_Gradient* f) { if (!(--f->mRefCounter)) delete f; }


/*****************************************************************************
*
* FUNCTION
*
*   All_IsoSurface_Intersections
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool IsoSurface::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int Side1 = 0, Side2 = 0, itrace = 0;
    DBL Depth1 = 0.0, Depth2 = 0.0;
    BasicRay New_Ray;
    Vector3d IPoint;
    Vector3d Plocal, Dlocal;
    DBL tmax = 0.0, tmin = 0.0, tmp = 0.0;
    DBL maxg = max_gradient;
    int i = 0 ; /* count of intervals in stack - 1      */
    int IFound = false;
    int begin = 0, end = 0;
    bool in_shadow_test = false;
    Vector3d VTmp;
    thread_local ISO_ThreadData isoData;

    Thread->Stats()[Ray_IsoSurface_Bound_Tests]++;

    if(container->Intersect(ray, Trans, Depth1, Depth2, Side1, Side2)) /* IsoSurface_Bound_Tests */
    {
        Thread->Stats()[Ray_IsoSurface_Bound_Tests_Succeeded]++;

        GenericScalarFunctionInstance fn(Function, Thread);

        in_shadow_test = ray.IsShadowTestRay();

        if(Depth1 < 0.0)
            Depth1 = 0.0;

        if (Trans != nullptr)
        {
            MInvTransPoint(Plocal, ray.Origin, Trans);
            MInvTransDirection(Dlocal, ray.Direction, Trans);
        }
        else
        {
            Plocal = ray.Origin;
            Dlocal = ray.Direction;
        }

        isoData.Inv3 = 1;

        if(closed != false)
        {
            VTmp = Plocal + Depth1 * Dlocal;
            tmp = EvaluatePolarized (fn, VTmp);
            if(Depth1 > accuracy)
            {
                if(tmp < 0.0)                   /* The ray hits the bounding shape */
                {
                    IPoint = ray.Evaluate(Depth1);
                    if(Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                    {
                        Depth_Stack->push(Intersection(Depth1, IPoint, this, 1, Side1));
                        IFound = true;
                        itrace++;
                        isoData.Inv3 *= -1;
                    }
                }
            }
            else
            {
                if(tmp < (maxg * accuracy * 4.0))
                {
                    Depth1 = accuracy * 5.0;
                    VTmp = Plocal + Depth1 * Dlocal;
                    if (IsInside (fn, VTmp))
                        isoData.Inv3 = -1;
                    /* Change the sign of the function (IPoint is in the bounding shpae.)*/
                }
                VTmp = Plocal + Depth2 * Dlocal;
                if (IsInside (fn, VTmp))
                {
                    IPoint = ray.Evaluate(Depth2);
                    if(Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                    {
                        Depth_Stack->push(Intersection(Depth2, IPoint, this, 1, Side2));
                        IFound = true;
                    }
                }
            }
        }

        /*  METHOD 2   by R. Suzuki */
        tmax = Depth2 = min(Depth2, BOUND_HUGE);
        tmin = Depth1 = min(Depth2, Depth1);
        if((tmax - tmin) < accuracy)
        {
            if (IFound)
                Depth_Stack->pop(); // we added an intersection already, so we need to undo that
            return (false);
        }
        Thread->Stats()[Ray_IsoSurface_Tests]++;
        if((Depth1 < accuracy) && (isoData.Inv3 == 1))
        {
            /* IPoint is on the isosurface */
            VTmp = Plocal + tmin * Dlocal;
            if (EvaluateAbs (fn, VTmp) < (maxg * accuracy * 4.0))
            {
                tmin = accuracy * 5.0;
                VTmp = Plocal + tmin * Dlocal;
                if (IsInside (fn, VTmp))
                    isoData.Inv3 = -1;
                /* change the sign and go into the isosurface */
            }
        }

        isoData.pFn = &fn;

        for (; itrace < max_trace; itrace++)
        {
            if(Function_Find_Root(isoData, Plocal, Dlocal, &tmin, &tmax, maxg, in_shadow_test, Thread) == false)
                break;
            else
            {
                IPoint = ray.Evaluate(tmin);
                if(Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                {
                    Depth_Stack->push(Intersection(tmin, IPoint, this, 0, 0 /*Side1*/));
                    IFound = true;
                }
            }
            tmin += accuracy * 5.0;
            if((tmax - tmin) < accuracy)
                break;
            isoData.Inv3 *= -1;
        }

        if(IFound)
            Thread->Stats()[Ray_IsoSurface_Tests_Succeeded]++;

        isoData.pFn = nullptr;
    }

    if(eval == true)
    {
        DBL temp_max_gradient = max_gradient; // TODO FIXME - works around nasty gcc (found using 4.0.1) bug failing to honor casting away of volatile on pass by value on template argument lookup [trf]
        max_gradient = max((DBL)temp_max_gradient, maxg); // TODO FIXME - This is not thread-safe but should be!!! [trf]
    }

    return (IFound);
}


/*****************************************************************************
*
* FUNCTION
*
*   Inside_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool IsoSurface::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    Vector3d New_Point;

    /* Transform the point into box space. */
    if (Trans != nullptr)
        MInvTransPoint(New_Point, IPoint, Trans);
    else
        New_Point = IPoint;

    if(!container->Inside(New_Point))
        return (Test_Flag(this, INVERTED_FLAG));

    GenericScalarFunctionInstance fn(Function, Thread);
    if (!IsInside (fn, New_Point))
        return (Test_Flag(this, INVERTED_FLAG));

    /* Inside the box. */
    return (!Test_Flag(this, INVERTED_FLAG));
}


double IsoSurface::GetPotential (const Vector3d& globalPoint, bool subtractThreshold, TraceThreadData *pThread) const
{
    Vector3d localPoint;

    if (Trans != nullptr)
        MInvTransPoint (localPoint, globalPoint, Trans);
    else
        localPoint = globalPoint;

    double potential = GenericScalarFunctionInstance(Function, pThread).Evaluate (localPoint);
    if (subtractThreshold)
        potential -= threshold;

    if (Test_Flag (this, INVERTED_FLAG))
        return -potential;
    else
        return  potential;
}


/*****************************************************************************
*
* FUNCTION
*
*   IsoSurface_Normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    Vector3d New_Point, TPoint;
    DBL funct;
    bool containerHit = Inter->i1;

    if (containerHit)
    {
        container->Normal(Inter->IPoint, Trans, Inter->i2, Result);
    }
    else
    {
        GenericScalarFunctionInstance fn(Function, Thread);

        /* Transform the point into the isosurface space */
        if (Trans != nullptr)
            MInvTransPoint(New_Point, Inter->IPoint, Trans);
        else
            New_Point = Inter->IPoint;

        TPoint = New_Point;
        funct = fn.Evaluate(TPoint);

        TPoint = New_Point;
        TPoint[X] += accuracy;
        Result[X] = fn.Evaluate(TPoint) - funct;
            TPoint = New_Point;

        TPoint[Y] += accuracy;
        Result[Y] = fn.Evaluate(TPoint) - funct;
            TPoint = New_Point;

        TPoint[Z] += accuracy;
        Result[Z] = fn.Evaluate(TPoint) - funct;

        if((Result[X] == 0) && (Result[Y] == 0) && (Result[Z] == 0))
            Result[X] = 1.0;
        Result.normalize();

        /* Transform the point into the boxes space. */
        if (Trans != nullptr)
        {
            MTransNormal(Result, Result, Trans);

            Result.normalize();
        }

        if (positivePolarity)
            Result.invert();
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Translate(const Vector3d&, const TRANSFORM* tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Rotate(const Vector3d&, const TRANSFORM* tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Scale(const Vector3d&, const TRANSFORM* tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Transform(const TRANSFORM* tr)
{
    if(Trans == nullptr)
        Trans = Create_Transform();

    Compose_Transforms(Trans, tr);

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

IsoSurface::IsoSurface() :
    ObjectBase(ISOSURFACE_OBJECT),
    positivePolarity(false)
{
    container = std::shared_ptr<ContainedByShape>(new ContainedByBox());

    Make_BBox(BBox, -1.0, -1.0, -1.0, 2.0, 2.0, 2.0);

    Trans = Create_Transform();

    Function = nullptr;
    accuracy = 0.001;
    max_trace = 1;

    eval_param[0] = 0.0; // 1.1; // not necessary
    eval_param[1] = 0.0; // 1.4; // not necessary
    eval_param[2] = 0.0; // 0.99; // not necessary
    eval = false;
    closed = true;

    max_gradient = 1.1;
    gradient = 0.0;
    threshold = 0.0;

    mginfo = boost::intrusive_ptr<ISO_Max_Gradient>(new ISO_Max_Gradient());
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

ObjectPtr IsoSurface::Copy()
{
    IsoSurface *New = new IsoSurface();
    Destroy_Transform(New->Trans);
    *New = *this;

    New->Function = Function->Clone();
    New->Trans = Copy_Transform(Trans);

    New->mginfo = mginfo;

    New->positivePolarity = positivePolarity;

    New->container = std::shared_ptr<ContainedByShape>(container->Copy());

    return (New);
}


/*****************************************************************************
*
* FUNCTION
*
*   Destroy_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

IsoSurface::~IsoSurface()
{
    delete Function;
}

/*****************************************************************************
*
* FUNCTION
*
*   DispatchShutdownMessages
*
* INPUT
*
*   messenger: destination of messages
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Chris Cason
*
* DESCRIPTION
*
*   If any max_gradient messages need to be sent to the user, they are dispatched
*   via the supplied CoreMessenger.
*
* CHANGES
*
******************************************************************************/

void IsoSurface::DispatchShutdownMessages(GenericMessenger& messenger)
{
    // TODO FIXME - works around nasty gcc (found using 4.0.1) bug failing to honor casting
    // away of volatile on pass by value on template argument lookup [trf]
    DBL temp_max_gradient = max_gradient;

    mginfo->gradient = max(gradient, mginfo->gradient);
    mginfo->max_gradient = max((DBL)temp_max_gradient, mginfo->max_gradient);

    if (mginfo->IsShared())
    {
        // Other instances shall take care of reporting the max gradient.
        mginfo.reset();
        return;
    }

    const CustomFunctionSourceInfo* fnInfo = Function->GetSourceInfo();

    if (fnInfo != nullptr)
    {
        if (eval == false)
        {
            // Only show the warning if necessary!
            // BTW, not being too picky here is a feature and not a bug ;-)  [trf]
            if ((mginfo->gradient > EPSILON) && (mginfo->max_gradient > EPSILON))
            {
                DBL diff = mginfo->max_gradient - mginfo->gradient;
                DBL prop = fabs(mginfo->max_gradient / mginfo->gradient);

                if (((prop <= 0.9) && (diff <= -0.5)) || (((prop <= 0.95) || (diff <= -0.1)) && (mginfo->max_gradient < 10.0)))
                {
                    messenger.WarningAt(kWarningGeneral, *fnInfo,
                                        "The maximum gradient found was %0.3f, but max_gradient of the\n"
                                        "isosurface was set to %0.3f. The isosurface may contain holes!\n"
                                        "Adjust max_gradient to get a proper rendering of the isosurface.",
                                        (float)(mginfo->gradient),
                                        (float)(mginfo->max_gradient));
                }
                else if ((diff >= 10.0) || ((prop >= 1.1) && (diff >= 0.5)))
                {
                    messenger.WarningAt(kWarningGeneral, *fnInfo,
                                        "The maximum gradient found was %0.3f, but max_gradient of\n"
                                        "the isosurface was set to %0.3f. Adjust max_gradient to\n"
                                        "get a faster rendering of the isosurface.",
                                        (float)(mginfo->gradient),
                                        (float)(mginfo->max_gradient));
                }
            }
        }
        else
        {
            DBL diff = (mginfo->eval_max / max(mginfo->eval_max - mginfo->eval_var, EPSILON));

            if ((eval_param[0] > mginfo->eval_max) || (eval_param[1] > diff))
            {
                mginfo->eval_cnt = max(mginfo->eval_cnt, 1.0); // make sure it won't be zero

                messenger.InfoAt(*fnInfo,
                                    "Evaluate found a maximum gradient of %0.3f and an average\n"
                                    "gradient of %0.3f. The maximum gradient variation was %0.3f.\n",
                                    (float)(mginfo->eval_max),
                                    (float)(mginfo->eval_gradient_sum / mginfo->eval_cnt),
                                    (float)(mginfo->eval_var));
            }
        }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Compute_IsoSurface_BBox
*
* INPUT
*
*   ISOSURFACE - IsoSurface
*
* OUTPUT
*
*   ISOSURFACE
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Calculate the bounding box of an Isosurface.
*
* CHANGES
*
******************************************************************************/

void IsoSurface::Compute_BBox()
{
    container->ComputeBBox(BBox);
    if (Trans != nullptr)
    {
        Recompute_BBox(&BBox, Trans);
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   IsoSurface_Function_Find_Root
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool IsoSurface::Function_Find_Root(ISO_ThreadData& itd, const Vector3d& PP, const Vector3d& DD, DBL* Depth1, DBL* Depth2, DBL& maxg, bool in_shadow_test, TraceThreadData* pThreadData)
{
    DBL dt, t21, l_b, l_e, oldmg;
    ISO_Pair EP1, EP2;
    Vector3d VTmp;

    pThreadData->Stats()[Ray_IsoSurface_Find_Root]++;

    itd.Vlength = DD.length();

    if(itd.cache.current == this)
    {
        pThreadData->Stats()[Ray_IsoSurface_Cache]++;
        VTmp = PP + *Depth1 * DD;
        VTmp -= itd.cache.Pglobal;
        l_b = VTmp.length();
        VTmp = PP + *Depth2 * DD;
        VTmp -= itd.cache.Dglobal;
        l_e = VTmp.length();
        if((itd.cache.fmax - maxg * max(l_b, l_e)) > 0.0)
        {
            pThreadData->Stats()[Ray_IsoSurface_Cache_Succeeded]++;
            return false;
        }
    }

    itd.cache.Pglobal = PP;
    itd.cache.Dglobal = DD;

    itd.cache.current = nullptr;
    EP1.t = *Depth1;
    EP1.f = Float_Function(itd, *Depth1);
    itd.cache.fmax = EP1.f;
    if((closed == false) && (EP1.f < 0.0))
    {
        itd.Inv3 *= -1;
        EP1.f *= -1;
    }

    EP2.t = *Depth2;
    EP2.f = Float_Function(itd, *Depth2);
    itd.cache.fmax = min(EP2.f, itd.cache.fmax);

    oldmg = maxg;
    t21 = (*Depth2 - *Depth1);
    if((eval == true) && (oldmg > eval_param[0]))
        maxg = oldmg * eval_param[2];
    dt = maxg * itd.Vlength * t21;
    if(Function_Find_Root_R(itd, &EP1, &EP2, dt, t21, 1.0 / (itd.Vlength * t21), maxg, pThreadData))
    {
        if(eval == true)
        {
            DBL curvar = fabs(maxg - oldmg);

            if(curvar > mginfo->eval_var)
                mginfo->eval_var = curvar;

            mginfo->eval_cnt++;
            mginfo->eval_gradient_sum += maxg;

            if(maxg > mginfo->eval_max)
                mginfo->eval_max = maxg;
        }

        *Depth1 = itd.tl;

        return true;
    }
    else if(!in_shadow_test)
    {
        itd.cache.Pglobal = PP + EP1.t * DD;
        itd.cache.Dglobal = PP + EP2.t * DD;
        itd.cache.current = this;

        return false;
    }

    return false;
}

/*****************************************************************************
*
* FUNCTION
*
*   IsoSurface_Function_Find_Root_R
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool IsoSurface::Function_Find_Root_R(ISO_ThreadData& itd, const ISO_Pair* EP1, const ISO_Pair* EP2, DBL dt, DBL t21, DBL len, DBL& maxg, TraceThreadData* pThreadData)
{
    ISO_Pair EPa;
    DBL temp;

    temp = fabs((EP2->f - EP1->f) * len);
    if(gradient < temp)
        gradient = temp;

    if((eval == true) && (maxg < temp * eval_param[1]))
    {
        maxg = temp * eval_param[1] * eval_param[1];
        dt = maxg * itd.Vlength * t21;
    }

    if(t21 < accuracy)
    {
        if(EP2->f < 0)
        {
            itd.tl = EP2->t;
            return true;
        }
        else
            return false;
    }

    if((EP1->f + EP2->f - dt) < 0)
    {
        t21 *= 0.5;
        dt *= 0.5;
        EPa.t = EP1->t + t21;
        EPa.f = Float_Function(itd, EPa.t);

        itd.cache.fmax = min(EPa.f, itd.cache.fmax);
        if(!Function_Find_Root_R(itd, EP1, &EPa, dt, t21, len * 2.0, maxg, pThreadData))
            return (Function_Find_Root_R(itd, &EPa, EP2, dt, t21, len * 2.0,maxg, pThreadData));
        else
            return true;
    }
    else
        return false;
}


/*****************************************************************************
*
* FUNCTION
*
*   Float_IsoSurface_Function
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

DBL IsoSurface::Float_Function(ISO_ThreadData& itd, DBL t) const
{
    Vector3d VTmp;

    VTmp = itd.cache.Pglobal + t * itd.cache.Dglobal;

    return ((DBL)itd.Inv3 * EvaluatePolarized (*itd.pFn, VTmp));
}


/*****************************************************************************/

DBL IsoSurface::EvaluateAbs (GenericScalarFunctionInstance& fn, Vector3d& p) const
{
    return fabs (threshold - fn.Evaluate (p));
}

DBL IsoSurface::EvaluatePolarized (GenericScalarFunctionInstance& fn, Vector3d& p) const
{
    if (positivePolarity)
        return threshold - fn.Evaluate (p);
    else
        return fn.Evaluate (p) - threshold;
}

bool IsoSurface::IsInside (GenericScalarFunctionInstance& fn, Vector3d& p) const
{
    if (positivePolarity)
        return threshold < fn.Evaluate (p);
    else
        return fn.Evaluate (p) < threshold;
}

}
// end of namespace pov
