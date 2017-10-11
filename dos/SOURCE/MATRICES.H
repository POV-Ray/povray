/****************************************************************************
*                   matrices.h
*
*  This module contains all defines, typedefs, and prototypes for MATRICES.C.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1999 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by email to team-coord@povray.org or visit us on the web at
*  http://www.povray.org. The latest version of POV-Ray may be found at this site.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/


#ifndef MATRICES_H
#define MATRICES_H



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

void MZero (MATRIX result);
void MIdentity (MATRIX result);
void MTimes (MATRIX result, MATRIX matrix1, MATRIX matrix2);
void MAdd (MATRIX result, MATRIX matrix1, MATRIX matrix2);
void MSub (MATRIX result, MATRIX matrix1, MATRIX matrix2);
void MScale (MATRIX result, MATRIX matrix1, DBL amount);
void MTranspose (MATRIX result, MATRIX matrix1);
void MTransPoint (VECTOR result, VECTOR vector, TRANSFORM *trans);
void MInvTransPoint (VECTOR result, VECTOR vector, TRANSFORM *trans);
void MTransDirection (VECTOR result, VECTOR vector, TRANSFORM *trans);
void MInvTransDirection (VECTOR result, VECTOR vector, TRANSFORM *trans);
void MTransNormal (VECTOR result, VECTOR vector, TRANSFORM *trans);
void MInvTransNormal (VECTOR result, VECTOR vector, TRANSFORM *trans);
void Compute_Matrix_Transform (TRANSFORM *result, MATRIX matrix);
void Compute_Scaling_Transform (TRANSFORM *result, VECTOR vector);
void Compute_Inversion_Transform (TRANSFORM *result);
void Compute_Translation_Transform (TRANSFORM *transform, VECTOR vector);
void Compute_Rotation_Transform (TRANSFORM *transform, VECTOR vector);
void Compute_Look_At_Transform (TRANSFORM *transform, VECTOR Look_At, VECTOR Up, VECTOR Right);
void Compose_Transforms (TRANSFORM *Original_Transform, TRANSFORM *New_Transform);
void Compute_Axis_Rotation_Transform (TRANSFORM *transform, VECTOR V1, DBL angle);
void Compute_Coordinate_Transform (TRANSFORM *trans, VECTOR origin, VECTOR up, DBL r, DBL len);
TRANSFORM *Create_Transform (void);
TRANSFORM *Copy_Transform (TRANSFORM *Old);
UV_VECT *Create_UV_Vect (void);
VECTOR *Create_Vector (void);
VECTOR_4D *Create_Vector_4D (void);
DBL *Create_Float (void);
void MInvers (MATRIX r, MATRIX m);



#endif
