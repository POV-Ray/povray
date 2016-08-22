//******************************************************************************
///
/// @file core/math/hypercomplex.h
///
/// Declarations related to hypercomplex Julia fractals.
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

#ifndef POVRAY_CORE_HYPERCOMPLEX_H
#define POVRAY_CORE_HYPERCOMPLEX_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/coretypes.h"
#include "core/math/vector.h"

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

struct Complex;

typedef void (*COMPLEX_FUNCTION_METHOD) (Complex *, const Complex *, const Complex *);

class HypercomplexBaseFractalRules : public FractalRules
{
    public:
        virtual ~HypercomplexBaseFractalRules() {}
        virtual bool Bound (const BasicRay&, const Fractal *, DBL *, DBL *) const;
};

class HypercomplexFractalRules : public HypercomplexBaseFractalRules
{
    public:
        virtual ~HypercomplexFractalRules() {}
        virtual void CalcNormal (Vector3d&, int, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, const Vector3d&, DBL *, DBL **) const;
};

class HypercomplexFunctionFractalRules : public HypercomplexBaseFractalRules
{
    public:
        HypercomplexFunctionFractalRules(COMPLEX_FUNCTION_METHOD fn) : ComplexFunction(fn) {}
        virtual ~HypercomplexFunctionFractalRules() {}
        virtual void CalcNormal (Vector3d&, int, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, const Vector3d&, DBL *, DBL **) const;
    protected:
        COMPLEX_FUNCTION_METHOD ComplexFunction;
        void HFunc(DBL *xr, DBL *yr, DBL *zr, DBL *wr, DBL x, DBL y, DBL z, DBL w, const Fractal * f) const;
};

class HypercomplexZ3FractalRules : public HypercomplexBaseFractalRules
{
    public:
        virtual ~HypercomplexZ3FractalRules() {}
        virtual void CalcNormal (Vector3d&, int, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, const Vector3d&, DBL *, DBL **) const;
};

class HypercomplexReciprocalFractalRules : public HypercomplexBaseFractalRules
{
    public:
        virtual ~HypercomplexReciprocalFractalRules() {}
        virtual void CalcNormal (Vector3d&, int, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, DBL **) const;
        virtual bool Iterate (const Vector3d&, const Fractal *, const Vector3d&, DBL *, DBL **) const;
};

void Complex_Exp (Complex *target, const Complex *source, const Complex *);
void Complex_Ln (Complex *target, const Complex *source, const Complex *);
void Complex_Sin (Complex *target, const Complex *source, const Complex *);
void Complex_ASin (Complex *target, const Complex *source, const Complex *);
void Complex_Sinh (Complex *target, const Complex *source, const Complex *);
void Complex_ASinh (Complex *target, const Complex *source, const Complex *);
void Complex_Cos (Complex *target, const Complex *source, const Complex *);
void Complex_ACos (Complex *target, const Complex *source, const Complex *);
void Complex_Cosh (Complex *target, const Complex *source, const Complex *);
void Complex_ACosh (Complex *target, const Complex *source, const Complex *);
void Complex_Tan (Complex *target, const Complex *source, const Complex *);
void Complex_ATan (Complex *target, const Complex *source, const Complex *);
void Complex_Tanh (Complex *target, const Complex *source, const Complex *);
void Complex_ATanh (Complex *target, const Complex *source, const Complex *);
void Complex_Sqrt (Complex *target, const Complex *source, const Complex *);
void Complex_Pwr (Complex *target, const Complex *source1, const Complex *source2);
void Complex_Mult (Complex *target, const Complex *source1, const Complex *source2);
void Complex_Div (Complex *target, const Complex *source1, const Complex *source2);

}

#endif // POVRAY_CORE_HYPERCOMPLEX_H
