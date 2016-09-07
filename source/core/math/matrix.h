//******************************************************************************
///
/// @file core/math/matrix.h
///
/// Declarations related to matrices.
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

#ifndef POVRAY_CORE_MATRIX_H
#define POVRAY_CORE_MATRIX_H

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

typedef DBL MATRIX[4][4]; ///< @todo       Make this obsolete.


typedef struct Transform_Struct TRANSFORM;

struct Transform_Struct
{
    MATRIX matrix;
    MATRIX inverse;
};


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

void MTransPoint        (Vector3d& result, const Vector3d& vector, const MATRIX *matrix);
void MTransDirection    (Vector3d& result, const Vector3d& vector, const MATRIX *matrix);
void MInvTransNormal    (Vector3d& result, const Vector3d& vector, const MATRIX *matrix);

inline void MTransPoint        (Vector3d& result, const Vector3d& vector, const TRANSFORM* trans) { MTransPoint     (result, vector, &trans->matrix);  }
inline void MInvTransPoint     (Vector3d& result, const Vector3d& vector, const TRANSFORM* trans) { MTransPoint     (result, vector, &trans->inverse); }
inline void MTransDirection    (Vector3d& result, const Vector3d& vector, const TRANSFORM* trans) { MTransDirection (result, vector, &trans->matrix);  }
inline void MInvTransDirection (Vector3d& result, const Vector3d& vector, const TRANSFORM* trans) { MTransDirection (result, vector, &trans->inverse); }
inline void MTransNormal       (Vector3d& result, const Vector3d& vector, const TRANSFORM* trans) { MInvTransNormal (result, vector, &trans->inverse); }
inline void MInvTransNormal    (Vector3d& result, const Vector3d& vector, const TRANSFORM* trans) { MInvTransNormal (result, vector, &trans->matrix);  }

inline void MTransRay    (BasicRay& res, const BasicRay& r, const TRANSFORM* t) { MTransPoint    (res.Origin, r.Origin, t); MTransDirection    (res.Direction, r.Direction, t); }
inline void MInvTransRay (BasicRay& res, const BasicRay& r, const TRANSFORM* t) { MInvTransPoint (res.Origin, r.Origin, t); MInvTransDirection (res.Direction, r.Direction, t); }

void Compute_Matrix_Transform (TRANSFORM *result, const MATRIX matrix);
void Compute_Scaling_Transform (TRANSFORM *result, const Vector3d& vector);
void Compute_Inversion_Transform (TRANSFORM *result);
void Compute_Translation_Transform (TRANSFORM *transform, const Vector3d& vector);
void Compute_Rotation_Transform (TRANSFORM *transform, const Vector3d& vector);
void Compute_Look_At_Transform (TRANSFORM *transform, const Vector3d& Look_At, const Vector3d& Up, const Vector3d& Right);
void Compose_Transforms (TRANSFORM *transform, const TRANSFORM *Additional_Transform);
void Compute_Axis_Rotation_Transform (TRANSFORM *transform, const Vector3d& AxisVect, DBL angle);
void Compute_Coordinate_Transform (TRANSFORM *trans, const Vector3d& origin, Vector3d& up, DBL r, DBL len);
TRANSFORM *Create_Transform (void);
TRANSFORM *Copy_Transform (const TRANSFORM *Old);
void Destroy_Transform (TRANSFORM *Trans);
DBL *Create_Float (void);
void MInvers (MATRIX r, const MATRIX m);
int MInvers3(const Matrix3x3& inM, Matrix3x3& outM);

}

#endif // POVRAY_CORE_MATRIX_H
