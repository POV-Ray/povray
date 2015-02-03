//******************************************************************************
///
/// @file backend/math/quatern.h
///
/// This module contains all defines, typedefs, and prototypes for `quatern.cpp`.
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

#ifndef JULIA_H
#define JULIA_H

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/*****************************************************************************
* Global typedefs
******************************************************************************/

/*****************************************************************************
* Global variables
******************************************************************************/

/*****************************************************************************
* Global functions
******************************************************************************/

class QuaternionFractalRules : public FractalRules
{
    public:
        virtual ~QuaternionFractalRules() {}
        virtual bool Bound (const BasicRay&, const Fractal *, DBL *, DBL *) const;
};

class JuliaFractalRules : public QuaternionFractalRules
{
    public:
        virtual ~JuliaFractalRules() {}
        virtual void CalcNormal (Vector3d&, int, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, const Vector3d&, DBL *, DBL **) const;
};

class Z3FractalRules : public QuaternionFractalRules
{
    public:
        virtual ~Z3FractalRules() {}
        virtual void CalcNormal (Vector3d&, int, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, const Vector3d&, DBL *, DBL **) const;
};

}

#endif
