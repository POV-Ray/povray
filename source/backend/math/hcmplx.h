/*******************************************************************************
 * hcmplx.h
 *
 * This module contains all defines, typedefs, and prototypes for HCMPLX.CPP.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/math/hcmplx.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef HCMPLX_H
#define HCMPLX_H

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

int F_Bound_HCompl (const Ray&, const Fractal *, DBL *, DBL *);
void Normal_Calc_HCompl (VECTOR, int, const Fractal *, DBL **);
int Iteration_HCompl (const VECTOR, const Fractal *, DBL **);
int D_Iteration_HCompl (const VECTOR, const Fractal *, const VECTOR &, DBL *, DBL **);

int F_Bound_HCompl_z3 (const Ray &, const Fractal *, DBL *, DBL *);
void Normal_Calc_HCompl_z3 (VECTOR, int, const Fractal *, DBL **);
int Iteration_HCompl_z3 (const VECTOR, const Fractal *, DBL **);
int D_Iteration_HCompl_z3 (const VECTOR, const Fractal *, const VECTOR &, DBL *, DBL **);

int F_Bound_HCompl_Reciprocal (const Ray &, const Fractal *, DBL *, DBL *);
void Normal_Calc_HCompl_Reciprocal (VECTOR, int, const Fractal *, DBL **);
int Iteration_HCompl_Reciprocal (const VECTOR, const Fractal *, DBL **);
int D_Iteration_HCompl_Reciprocal (const VECTOR, const Fractal *, const VECTOR &, DBL *, DBL **);

int F_Bound_HCompl_Func (const Ray &, const Fractal *, DBL *, DBL *);
void Normal_Calc_HCompl_Func (VECTOR, int, const Fractal *, DBL **);
int Iteration_HCompl_Func (const VECTOR, const Fractal *, DBL **);
int D_Iteration_HCompl_Func (const VECTOR, const Fractal *, const VECTOR &, DBL *, DBL **);

void Complex_Exp (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_Ln (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_Sin (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_ASin (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_Sinh (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_ASinh (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_Cos (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_ACos (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_Cosh (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_ACosh (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_Tan (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_ATan (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_Tanh (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_ATanh (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_Sqrt (CMPLX *target, const CMPLX *source, const CMPLX *);
void Complex_Pwr (CMPLX *target, const CMPLX *source1, const CMPLX *source2);
void Complex_Mult (CMPLX *target, const CMPLX *source1, const CMPLX *source2);
void Complex_Div (CMPLX *target, const CMPLX *source1, const CMPLX *source2);

}

#endif
