//******************************************************************************
///
/// @file core/shape/lemon.h
///
/// Declarations related to the ovus geometric primitive.
///
/// @author Jerome Grimbert
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_CORE_LEMON_H
#define POVRAY_CORE_LEMON_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

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

#define LEMON_OBJECT (STURM_OK_OBJECT)

/// @}
///
//******************************************************************************

class Lemon : public ObjectBase
{
    private:
        struct LEMON_INT
        {
            DBL d;  /* Distance of intersection point               */
            Vector3d n;/* Normal */
        };
    public:
        Lemon();
        virtual ~Lemon();

        virtual ObjectPtr Copy();

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
        virtual bool Inside(const Vector3d&, TraceThreadData *) const;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;
        virtual void UVCoord(Vector2d&, const Intersection *, TraceThreadData *) const;
        virtual void Translate(const Vector3d&, const TRANSFORM *);
        virtual void Rotate(const Vector3d&, const TRANSFORM *);
        virtual void Scale(const Vector3d&, const TRANSFORM *);
        virtual void Transform(const TRANSFORM *);
        virtual void Compute_BBox();

        void Compute_Lemon_Data(GenericMessenger& messenger, pov_base::ITextStream *FileHandle, pov_base::ITextStream::FilePos & Token_File_Pos, int Token_Col_No );

        Vector3d apex;          /* Center of the top of the lemon */
        Vector3d base;          /* Center of the bottom of the lemon */
        DBL apex_radius;        /* Radius of the lemon at the top */
        DBL base_radius;        /* Radius of the lemon at the bottom */
        DBL inner_radius;       /* Radius of the inner circle */
        DBL HorizontalPosition; /* horizontal position of the center of the inner circle */
        DBL VerticalPosition;   /* vertical position of the center of the inner circle */
    protected:
        int Intersect(const Vector3d& P, const Vector3d& D, LEMON_INT *Intersection, TraceThreadData *Thread) const;
        void CalcUV(const Vector3d& IPoint, Vector2d& Result) const;
};

/// @}
///
//##############################################################################

}

#endif // POVRAY_CORE_OVUS_H
