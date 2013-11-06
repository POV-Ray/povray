/*******************************************************************************
 * matrices.h
 *
 * This module contains all defines, typedefs, and prototypes for MATRICES.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/math/matrices.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef MATRICES_H
#define MATRICES_H

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

void MZero (MATRIX result);
void MIdentity (MATRIX result);
void MTimesA (MATRIX result, const MATRIX matrix2);
void MTimesB (const MATRIX matrix1, MATRIX result);
void MTimesC (MATRIX result, const MATRIX matrix1, const MATRIX matrix2);
void MAdd (MATRIX result, const MATRIX matrix1, const MATRIX matrix2);
void MSub (MATRIX result, const MATRIX matrix1, const MATRIX matrix2);
void MScale (MATRIX result, const MATRIX matrix1, DBL amount);
void MTranspose (MATRIX result);
void MTranspose (MATRIX result, const MATRIX matrix1);
void MTransPoint (VECTOR result, const VECTOR vector, const TRANSFORM *trans);
void MInvTransPoint (VECTOR result, const VECTOR vector, const TRANSFORM *trans);
void MTransDirection (VECTOR result, const VECTOR vector, const TRANSFORM *trans);
void MInvTransDirection (VECTOR result, const VECTOR vector, const TRANSFORM *trans);
void MTransNormal (VECTOR result, const VECTOR vector, const TRANSFORM *trans);
void MInvTransNormal (VECTOR result, const VECTOR vector, const TRANSFORM *trans);
void Compute_Matrix_Transform (TRANSFORM *result, const MATRIX matrix);
void Compute_Scaling_Transform (TRANSFORM *result, const VECTOR vector);
void Compute_Inversion_Transform (TRANSFORM *result);
void Compute_Translation_Transform (TRANSFORM *transform, const VECTOR vector);
void Compute_Rotation_Transform (TRANSFORM *transform, const VECTOR vector);
void Compute_Look_At_Transform (TRANSFORM *transform, const VECTOR Look_At, const VECTOR Up, const VECTOR Right);
void Compose_Transforms (TRANSFORM *transform, const TRANSFORM *Additional_Transform);
void Compute_Axis_Rotation_Transform (TRANSFORM *transform, const VECTOR AxisVect, DBL angle);
void Compute_Coordinate_Transform (TRANSFORM *trans, const VECTOR origin, VECTOR up, DBL r, DBL len);
TRANSFORM *Create_Transform (void);
TRANSFORM *Copy_Transform (const TRANSFORM *Old);
void Destroy_Transform (TRANSFORM *Trans);
UV_VECT *Create_UV_Vect (void);
VECTOR *Create_Vector (void);
VECTOR_4D *Create_Vector_4D (void);
DBL *Create_Float (void);
void MInvers (MATRIX r, const MATRIX m);
int MInvers3(const VECTOR inM[3], VECTOR outM[3]);
void MTransUVPoint(const DBL p[2], const DBL m[3][3], DBL t[2]);
void MSquareQuad(const UV_VECT st[4], DBL sq[3][3]);

}

#endif
