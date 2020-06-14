//******************************************************************************
///
/// @file core/shape/uvmeshable.h
///
/// header for uv-meshable interface, for shapes that can be computed using u,v parameters
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

#ifndef POVRAY_CORE_UVMESHABLE_H
#define POVRAY_CORE_UVMESHABLE_H

#include "core/coretypes.h"

namespace pov
{
/** Interface for uv_* functions */
class UVMeshable
{
    public:
        /**
         * get position for u,v values
         * \param[out] r computed position
         * \param[in] u value of u
         * \param[in] v value of v
         * \param[in] Thread pointer to parser thread data
         */
        virtual void evalVertex( Vector3d& r, const DBL u, const DBL v, TraceThreadData * Thread )const=0;
        /**
         * get normal for u,v values
         * \param[out] r computed normal
         * \param[in] u value of u
         * \param[in] v value of v
         * \param[in] Thread pointer to parser thread data
         */
        virtual void evalNormal( Vector3d& r, const DBL u, const DBL v, TraceThreadData * Thread )const=0;
        /**
         * retrieve the minimal values for u,v
         * \param[out] r retrieved minimal values
         */
        virtual void minUV( Vector2d& r )const=0;
        /**
         * retrieve the maximal values for u,v
         * \param[out] r retrieved maximal values
         */
        virtual void maxUV( Vector2d& r )const=0;
};
}
#endif
