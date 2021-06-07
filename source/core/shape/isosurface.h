//******************************************************************************
///
/// @file core/shape/isosurface.h
///
/// Declarations related to the isosurface geometric primitive.
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

#ifndef POVRAY_CORE_ISOSURFACE_H
#define POVRAY_CORE_ISOSURFACE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// Boost header files
#include <boost/intrusive_ptr.hpp>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/coretypes.h"
#include "core/scene/object.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCoreShape
///
/// @{

//******************************************************************************
///
/// @name Object Types
///
/// @{

#define ISOSURFACE_OBJECT      (BASIC_OBJECT+POTENTIAL_OBJECT)

/// @}
///
//******************************************************************************

#define ISOSURFACE_MAXTRACE    10


/*****************************************************************************
* Global typedefs
******************************************************************************/

class IsoSurface;

typedef unsigned char IsosurfaceMaxTrace;

struct ISO_Pair final
{
    DBL t, f;
};

struct ISO_Max_Gradient;
struct ISO_ThreadData;

class IsoSurface final : public ObjectBase
{
    public:

        GenericScalarFunctionPtr Function;
        volatile DBL max_gradient; // modified during render if `eval` is set
        DBL gradient;
        DBL threshold;
        DBL accuracy;
        DBL eval_param[3];
        IsosurfaceMaxTrace max_trace;
        bool closed             : 1;
        bool eval               : 1;
        bool positivePolarity   : 1; ///< `true` if values above threshold are considered inside, `false` if considered outside.

        std::shared_ptr<ContainedByShape> container;

        IsoSurface();
        virtual ~IsoSurface() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual double GetPotential (const Vector3d&, bool subtractThreshold, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;

        virtual void DispatchShutdownMessages(GenericMessenger& messenger) override;

    protected:
        bool Function_Find_Root(ISO_ThreadData& itd, const Vector3d&, const Vector3d&, DBL*, DBL*, DBL& max_gradient, bool in_shadow_test, TraceThreadData* pThreadData);
        bool Function_Find_Root_R(ISO_ThreadData& itd, const ISO_Pair*, const ISO_Pair*, DBL, DBL, DBL, DBL& max_gradient, TraceThreadData* pThreadData);

        inline DBL Float_Function(ISO_ThreadData& itd, DBL t) const;
        inline DBL EvaluateAbs (GenericScalarFunctionInstance& fn, Vector3d& p) const;
        inline DBL EvaluatePolarized (GenericScalarFunctionInstance& fn, Vector3d& p) const;
        inline bool IsInside (GenericScalarFunctionInstance& fn, Vector3d& p) const;

    private:

        boost::intrusive_ptr<ISO_Max_Gradient> mginfo; // global, but just a statistic (read: not thread safe but we don't care) [trf]
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_ISOSURFACE_H
