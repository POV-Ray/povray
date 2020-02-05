/****************************************************************************
*                   julia.h
*
*  This module contains all defines, typedefs, and prototypes for JULIA.C.
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

#ifndef JULIA_H
#define JULIA_H

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

int F_Bound_Julia (RAY * Ray, FRACTAL * Fractal, DBL * Depth_Min, DBL * Depth_Max);
void Normal_Calc_Julia (VECTOR Result, int N_Max, FRACTAL *fractal);
void Normal_Calc_z3 (VECTOR Result, int N_Max, FRACTAL *fractal);
int Iteration_Julia (VECTOR point, FRACTAL * Julia);
int D_Iteration_Julia (VECTOR point, FRACTAL * Julia, DBL * Dist);
int Iteration_z3 (VECTOR point, FRACTAL * Julia);
int D_Iteration_z3 (VECTOR point, FRACTAL * Julia, DBL * Dist);

#endif
