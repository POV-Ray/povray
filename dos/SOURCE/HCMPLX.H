/****************************************************************************
*                   hcmplx.h
*
*  This module contains all defines, typedefs, and prototypes for HCMPLX.C.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1998 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's GO POVRAY Forum or visit
*  http://www.povray.org. The latest version of POV-Ray may be found at these sites.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

#ifndef HCMPLX_H
#define HCMPLX_H

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

int F_Bound_HCompl (RAY *, FRACTAL *, DBL *, DBL *);
void Normal_Calc_HCompl (VECTOR, int, FRACTAL *);
int Iteration_HCompl (VECTOR, FRACTAL *);
int D_Iteration_HCompl (VECTOR, FRACTAL *, DBL *);

int F_Bound_HCompl_z3 (RAY *, FRACTAL *, DBL *, DBL *);
void Normal_Calc_HCompl_z3 (VECTOR, int, FRACTAL *);
int Iteration_HCompl_z3 (VECTOR, FRACTAL *);
int D_Iteration_HCompl_z3 (VECTOR, FRACTAL *, DBL *);

int F_Bound_HCompl_Reciprocal (RAY *, FRACTAL *, DBL *, DBL *);
void Normal_Calc_HCompl_Reciprocal (VECTOR, int, FRACTAL *);
int Iteration_HCompl_Reciprocal (VECTOR, FRACTAL *);
int D_Iteration_HCompl_Reciprocal (VECTOR, FRACTAL *, DBL *);

int F_Bound_HCompl_Func (RAY *, FRACTAL *, DBL *, DBL *);
void Normal_Calc_HCompl_Func (VECTOR, int, FRACTAL *);
int Iteration_HCompl_Func (VECTOR, FRACTAL *);
int D_Iteration_HCompl_Func (VECTOR, FRACTAL *, DBL *);

void Complex_Exp (CMPLX *target, CMPLX *source);
void Complex_Log (CMPLX *target, CMPLX *source);
void Complex_Sin (CMPLX *target, CMPLX *source);
void Complex_ASin (CMPLX *target, CMPLX *source);
void Complex_Sinh (CMPLX *target, CMPLX *source);
void Complex_ASinh (CMPLX *target, CMPLX *source);
void Complex_Cos (CMPLX *target, CMPLX *source);
void Complex_ACos (CMPLX *target, CMPLX *source);
void Complex_Cosh (CMPLX *target, CMPLX *source);
void Complex_ACosh (CMPLX *target, CMPLX *source);
void Complex_Tan (CMPLX *target, CMPLX *source);
void Complex_ATan (CMPLX *target, CMPLX *source);
void Complex_Tanh (CMPLX *target, CMPLX *source);
void Complex_ATanh (CMPLX *target, CMPLX *source);
void Complex_Sqrt (CMPLX *target, CMPLX *source);
void Complex_Pwr (CMPLX *target, CMPLX *source);

void Complex_Mult (CMPLX *target, CMPLX *source1, CMPLX *source2);
void Complex_Div (CMPLX *target, CMPLX *source1, CMPLX *source2);
void Complex_Power (CMPLX *target, CMPLX *source1, CMPLX *source2);

#endif
