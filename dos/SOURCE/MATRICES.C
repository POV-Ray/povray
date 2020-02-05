/****************************************************************************
*                matrices.c
*
*  This module contains code to manipulate 4x4 matrices.
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

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "matrices.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Initialize the matrix to the following values:
*
*     0.0   0.0   0.0   0.0
*     0.0   0.0   0.0   0.0
*     0.0   0.0   0.0   0.0
*     0.0   0.0   0.0   0.0
*
* CHANGES
*
*   -
*
******************************************************************************/

void MZero (MATRIX result)
{
  register int i, j;

  for (i = 0 ; i < 4 ; i++)
  {
    for (j = 0 ; j < 4 ; j++)
    {
      result[i][j] = 0.0;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Initialize the matrix to the following values:
*
*     1.0   0.0   0.0   0.0
*     0.0   1.0   0.0   0.0
*     0.0   0.0   1.0   0.0
*     0.0   0.0   0.0   1.0
*
* CHANGES
*
*   -
*
******************************************************************************/

void MIdentity (MATRIX result)
{
  register int i, j;

  for (i = 0 ; i < 4 ; i++)
  {
    for (j = 0 ; j < 4 ; j++)
    {
      if (i == j)
      {
        result[i][j] = 1.0;
      }
      else
      {
        result[i][j] = 0.0;
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void MTimes (MATRIX result, MATRIX  matrix1, MATRIX  matrix2)
{
  register int i, j, k;
  MATRIX temp_matrix;

  for (i = 0 ; i < 4 ; i++)
  {
    for (j = 0 ; j < 4 ; j++)
    {
      temp_matrix[i][j] = 0.0;

      for (k = 0 ; k < 4 ; k++)
      {
        temp_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
      }
    }
  }

  for (i = 0 ; i < 4 ; i++)
  {
    for (j = 0 ; j < 4 ; j++)
    {
      result[i][j] = temp_matrix[i][j];
    }
  }
}



/*  AAC - These are not used, so they are commented out to save code space...

void MAdd (MATRIX result, MATRIX matrix1, MATRIX matrix2)
{
  register int i, j;

  for (i = 0 ; i < 4 ; i++)
    for (j = 0 ; j < 4 ; j++)
     result[i][j] = (*matrix1)[i][j] + (*matrix2)[i][j];
}

void MSub (MATRIX result, MATRIX matrix1, MATRIX matrix2)
{
  register int i, j;

  for (i = 0 ; i < 4 ; i++)
    for (j = 0 ; j < 4 ; j++)
     result[i][j] = matrix1[i][j] - matrix2[i][j];
}

void MScale (MATRIX result, MATRIX matrix1, DBL amount)
{
  register int i, j;

  for (i = 0 ; i < 4 ; i++)
    for (j = 0 ; j < 4 ; j++)
      result[i][j] = matrix1[i][j] * amount;
}
... up to here! */



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void MTranspose (MATRIX result, MATRIX  matrix1)
{
  register int i, j;
  MATRIX temp_matrix;

  for (i = 0 ; i < 4 ; i++)
  {
    for (j = 0 ; j < 4 ; j++)
    {
      temp_matrix[i][j] = matrix1[j][i];
    }
  }

  for (i = 0 ; i < 4 ; i++)
  {
    for (j = 0 ; j < 4 ; j++)
    {
      result[i][j] = temp_matrix[i][j];
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 1994 : Modified to not calculate anser_array[3]. [DB]
*
******************************************************************************/

void MTransPoint (VECTOR result, VECTOR  vector, TRANSFORM *transform)
{
  register int i;
  DBL answer_array[4];
  MATRIX *matrix;

  matrix = (MATRIX *) transform->matrix;

  for (i = 0 ; i < 3 ; i++)
  {
    answer_array[i] = vector[X] * (*matrix)[0][i] +
                      vector[Y] * (*matrix)[1][i] +
                      vector[Z] * (*matrix)[2][i] + (*matrix)[3][i];
  }

  result[X] = answer_array[0];
  result[Y] = answer_array[1];
  result[Z] = answer_array[2];
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 1994 : Modified to not calculate anser_array[3]. [DB]
*
******************************************************************************/

void MInvTransPoint (VECTOR result, VECTOR  vector, TRANSFORM *transform)
{
  register int i;
  DBL answer_array[4];
  MATRIX *matrix;

  matrix = (MATRIX *) transform->inverse;

  for (i = 0 ; i < 3 ; i++)
  {
    answer_array[i] = vector[X] * (*matrix)[0][i] +
                      vector[Y] * (*matrix)[1][i] +
                      vector[Z] * (*matrix)[2][i] + (*matrix)[3][i];
  }

  result[X] = answer_array[0];
  result[Y] = answer_array[1];
  result[Z] = answer_array[2];
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 1994 : Modified to not calculate anser_array[3]. [DB]
*
******************************************************************************/

void MTransDirection (VECTOR result, VECTOR  vector, TRANSFORM *transform)
{
  register int i;
  DBL answer_array[4];
  MATRIX *matrix;

  matrix = (MATRIX *) transform->matrix;

  for (i = 0 ; i < 3 ; i++)
  {
    answer_array[i] = vector[X] * (*matrix)[0][i] +
                      vector[Y] * (*matrix)[1][i] +
                      vector[Z] * (*matrix)[2][i];
  }

  result[X] = answer_array[0];
  result[Y] = answer_array[1];
  result[Z] = answer_array[2];
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 1994 : Modified to not calculate anser_array[3]. [DB]
*
******************************************************************************/

void MInvTransDirection (VECTOR result, VECTOR  vector, TRANSFORM *transform)
{
  register int i;
  DBL answer_array[4];
  MATRIX *matrix;

  matrix = (MATRIX *) transform->inverse;

  for (i = 0 ; i < 3 ; i++)
  {
    answer_array[i] = vector[X] * (*matrix)[0][i] +
                      vector[Y] * (*matrix)[1][i] +
                      vector[Z] * (*matrix)[2][i];
  }

  result[X] = answer_array[0];
  result[Y] = answer_array[1];
  result[Z] = answer_array[2];
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void MTransNormal (VECTOR result, VECTOR  vector, TRANSFORM *transform)
{
  register int i;
  DBL answer_array[3];
  MATRIX *matrix;

  matrix = (MATRIX *) transform->inverse;

  for (i = 0 ; i < 3 ; i++)
  {
    answer_array[i] = vector[X] * (*matrix)[i][0] +
                      vector[Y] * (*matrix)[i][1] +
                      vector[Z] * (*matrix)[i][2];
  }

  result[X] = answer_array[0];
  result[Y] = answer_array[1];
  result[Z] = answer_array[2];
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void MInvTransNormal (VECTOR result, VECTOR  vector, TRANSFORM *transform)
{
  register int i;
  DBL answer_array[3];
  MATRIX *matrix;

  matrix = (MATRIX *) transform->matrix;

  for (i = 0 ; i < 3 ; i++)
  {
    answer_array[i] = vector[X] * (*matrix)[i][0] +
                      vector[Y] * (*matrix)[i][1] +
                      vector[Z] * (*matrix)[i][2];
  }

  result[X] = answer_array[0];
  result[Y] = answer_array[1];
  result[Z] = answer_array[2];
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Compute_Scaling_Transform (TRANSFORM *result, VECTOR vector)
{
  MIdentity (result->matrix);

  (result->matrix)[0][0] = vector[X];
  (result->matrix)[1][1] = vector[Y];
  (result->matrix)[2][2] = vector[Z];

  MIdentity (result->inverse);

  (result->inverse)[0][0] = 1.0 / vector[X];
  (result->inverse)[1][1] = 1.0 / vector[Y];
  (result->inverse)[2][2] = 1.0 / vector[Z];
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Matrix_Transform
*
* INPUT
*
*   matrix - matrix from which to create transform
*
* OUTPUT
*
*   result - complete transform
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Builds a complete transform from a matrix.
*
* CHANGES
*
*   June 1995 : Creation
*
******************************************************************************/

void Compute_Matrix_Transform (TRANSFORM *result, MATRIX matrix)
{
  register int i;

  for (i = 0; i < 4; i++)
  {
    (result->matrix)[i][0] = matrix[i][0];
    (result->matrix)[i][1] = matrix[i][1];
    (result->matrix)[i][2] = matrix[i][2];
    (result->matrix)[i][3] = matrix[i][3];
  }

  MInvers(result->inverse, result->matrix);
}



/* AAC - This is not used, so it's commented out...

void Compute_Inversion_Transform (TRANSFORM *result)
{
  MIdentity (result->matrix);

  (result->matrix)[0][0] = -1.0;
  (result->matrix)[1][1] = -1.0;
  (result->matrix)[2][2] = -1.0;
  (result->matrix)[3][3] = -1.0;


  (result->inverse)[0][0] = -1.0;
  (result->inverse)[1][1] = -1.0;
  (result->inverse)[2][2] = -1.0;
  (result->inverse)[3][3] = -1.0;
}
... up to here! */



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Compute_Translation_Transform (TRANSFORM *transform, VECTOR vector)
{
  MIdentity (transform->matrix);

  (transform->matrix)[3][0] = vector[X];
  (transform->matrix)[3][1] = vector[Y];
  (transform->matrix)[3][2] = vector[Z];

  MIdentity (transform->inverse);

  (transform->inverse)[3][0] = -vector[X];
  (transform->inverse)[3][1] = -vector[Y];
  (transform->inverse)[3][2] = -vector[Z];
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Compute_Rotation_Transform (TRANSFORM *transform, VECTOR vector)
{
  register DBL cosx, cosy, cosz, sinx, siny, sinz;
  MATRIX Matrix;
  VECTOR Radian_Vector;

  VScale (Radian_Vector, vector, M_PI_180);

  MIdentity (transform->matrix);

  cosx = cos (Radian_Vector[X]);
  sinx = sin (Radian_Vector[X]);
  cosy = cos (Radian_Vector[Y]);
  siny = sin (Radian_Vector[Y]);
  cosz = cos (Radian_Vector[Z]);
  sinz = sin (Radian_Vector[Z]);

  (transform->matrix) [1][1] = cosx;
  (transform->matrix) [2][2] = cosx;
  (transform->matrix) [1][2] = sinx;
  (transform->matrix) [2][1] = 0.0 - sinx;

  MTranspose (transform->inverse, transform->matrix);

  MIdentity (Matrix);

  Matrix [0][0] = cosy;
  Matrix [2][2] = cosy;
  Matrix [0][2] = 0.0 - siny;
  Matrix [2][0] = siny;

  MTimes (transform->matrix, transform->matrix, Matrix);

  MTranspose (Matrix, Matrix);

  MTimes (transform->inverse, Matrix, transform->inverse);

  MIdentity (Matrix);

  Matrix [0][0] = cosz;
  Matrix [1][1] = cosz;
  Matrix [0][1] = sinz;
  Matrix [1][0] = 0.0 - sinz;

  MTimes (transform->matrix, transform->matrix, Matrix);

  MTranspose (Matrix, Matrix);

  MTimes (transform->inverse, Matrix, transform->inverse);
}



/* AAC - This is not used so it's commented out...

void Compute_Look_At_Transform (TRANSFORM *result, VECTOR Look_At, VECTOR Up, VECTOR Right)
{
  MIdentity (result->inverse);

  (result->matrix)[0][0] = Right[X];
  (result->matrix)[0][1] = Right[Y];
  (result->matrix)[0][2] = Right[Z];

  (result->matrix)[1][0] = Up[X];
  (result->matrix)[1][1] = Up[Y];
  (result->matrix)[1][2] = Up[Z];

  (result->matrix)[2][0] = Look_At[X];
  (result->matrix)[2][1] = Look_At[Y];
  (result->matrix)[2][2] = Look_At[Z];

  MIdentity (result->matrix);

  MTranspose (result->matrix, result->inverse);
}
... up to here! */



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Compose_Transforms (TRANSFORM *Original_Transform, TRANSFORM  *New_Transform)
{
  MTimes(Original_Transform->matrix, Original_Transform->matrix,  New_Transform->matrix);

  MTimes(Original_Transform->inverse, New_Transform->inverse, Original_Transform->inverse);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Rotation about an arbitrary axis - formula from:
*
*     "Computational Geometry for Design and Manufacture", Faux & Pratt
*
*   NOTE: The angles for this transform are specified in radians.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Compute_Axis_Rotation_Transform (TRANSFORM *transform, VECTOR V1, DBL angle)
{
  DBL l, cosx, sinx;

  VLength(l, V1);
  VInverseScaleEq(V1, l);

  MIdentity(transform->matrix);

  cosx = cos(angle);
  sinx = sin(angle);

  transform->matrix[0][0] = V1[X] * V1[X] + cosx * (1.0 - V1[X] * V1[X]);
  transform->matrix[0][1] = V1[X] * V1[Y] * (1.0 - cosx) + V1[Z] * sinx;
  transform->matrix[0][2] = V1[X] * V1[Z] * (1.0 - cosx) - V1[Y] * sinx;

  transform->matrix[1][0] = V1[X] * V1[Y] * (1.0 - cosx) - V1[Z] * sinx;
  transform->matrix[1][1] = V1[Y] * V1[Y] + cosx * (1.0 - V1[Y] * V1[Y]);
  transform->matrix[1][2] = V1[Y] * V1[Z] * (1.0 - cosx) + V1[X] * sinx;

  transform->matrix[2][0] = V1[X] * V1[Z] * (1.0 - cosx) + V1[Y] * sinx;
  transform->matrix[2][1] = V1[Y] * V1[Z] * (1.0 - cosx) - V1[X] * sinx;
  transform->matrix[2][2] = V1[Z] * V1[Z] + cosx * (1.0 - V1[Z] * V1[Z]);

  MTranspose(transform->inverse, transform->matrix);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Given a point and a direction and a radius, find the transform
*   that brings these into a canonical coordinate system
*
* CHANGES
*
*   7/24/95 Eduard Schwan  - Changed "if" condition to use EPSILON, not equality
*  12/12/95 Steve Demlow   - Clipped abs(up[Z]) to 1 to avoid acos overflow
*
******************************************************************************/

void Compute_Coordinate_Transform(TRANSFORM *trans, VECTOR origin, VECTOR up, DBL radius, DBL length)
{
  TRANSFORM trans2;
  VECTOR tmpv;

  Make_Vector(tmpv, radius, radius, length);

  Compute_Scaling_Transform(trans, tmpv);

  if (fabs(up[Z]) > 1.0 - EPSILON)
  {
    Make_Vector(tmpv, 1.0, 0.0, 0.0)
    up[Z] = up[Z] < 0.0 ? -1.0 : 1.0;
  }
  else
  {
    Make_Vector(tmpv, -up[Y], up[X], 0.0)
  }

  Compute_Axis_Rotation_Transform(&trans2, tmpv, acos(up[Z]));

  Compose_Transforms(trans, &trans2);

  Compute_Translation_Transform(&trans2, origin);

  Compose_Transforms(trans, &trans2);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

TRANSFORM *Create_Transform()
{
  TRANSFORM *New;

  New = (TRANSFORM *)POV_MALLOC(sizeof (TRANSFORM), "transform");

  MIdentity (New->matrix);
  MIdentity (New->inverse);

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

TRANSFORM *Copy_Transform (TRANSFORM *Old)
{
  TRANSFORM *New;
  if (Old != NULL)
  {
    New  = Create_Transform ();
    *New = *Old;
  }
  else
  {
    New = NULL;
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

VECTOR *Create_Vector ()
{
  VECTOR *New;

  New = (VECTOR *)POV_MALLOC(sizeof (VECTOR), "vector");

  Make_Vector (*New, 0.0, 0.0, 0.0);

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

DBL *Create_Float ()
{
  DBL *New_Float;

  New_Float = (DBL *)POV_MALLOC(sizeof (DBL), "float");

  *New_Float = 0.0;

  return (New_Float);
}



/*****************************************************************************
*
* FUNCTION
*
*   MInvers
*
* INPUT
*
*   m - matrix to invert
*   r - inverted matrix
*   
* OUTPUT
*
*   r
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Invert a 4x4 matrix.
*
* CHANGES
*
*   May 1994 : Creation.
*
******************************************************************************/

void MInvers(MATRIX r, MATRIX  m)
{
  DBL d00, d01, d02, d03;
  DBL d10, d11, d12, d13;
  DBL d20, d21, d22, d23;
  DBL d30, d31, d32, d33;
  DBL m00, m01, m02, m03;
  DBL m10, m11, m12, m13;
  DBL m20, m21, m22, m23;
  DBL m30, m31, m32, m33;
  DBL D;

  m00 = m[0][0];  m01 = m[0][1];  m02 = m[0][2];  m03 = m[0][3];
  m10 = m[1][0];  m11 = m[1][1];  m12 = m[1][2];  m13 = m[1][3];
  m20 = m[2][0];  m21 = m[2][1];  m22 = m[2][2];  m23 = m[2][3];
  m30 = m[3][0];  m31 = m[3][1];  m32 = m[3][2];  m33 = m[3][3];

  d00 = m11*m22*m33 + m12*m23*m31 + m13*m21*m32 - m31*m22*m13 - m32*m23*m11 - m33*m21*m12;
  d01 = m10*m22*m33 + m12*m23*m30 + m13*m20*m32 - m30*m22*m13 - m32*m23*m10 - m33*m20*m12;
  d02 = m10*m21*m33 + m11*m23*m30 + m13*m20*m31 - m30*m21*m13 - m31*m23*m10 - m33*m20*m11;
  d03 = m10*m21*m32 + m11*m22*m30 + m12*m20*m31 - m30*m21*m12 - m31*m22*m10 - m32*m20*m11;

  d10 = m01*m22*m33 + m02*m23*m31 + m03*m21*m32 - m31*m22*m03 - m32*m23*m01 - m33*m21*m02;
  d11 = m00*m22*m33 + m02*m23*m30 + m03*m20*m32 - m30*m22*m03 - m32*m23*m00 - m33*m20*m02;
  d12 = m00*m21*m33 + m01*m23*m30 + m03*m20*m31 - m30*m21*m03 - m31*m23*m00 - m33*m20*m01;
  d13 = m00*m21*m32 + m01*m22*m30 + m02*m20*m31 - m30*m21*m02 - m31*m22*m00 - m32*m20*m01;

  d20 = m01*m12*m33 + m02*m13*m31 + m03*m11*m32 - m31*m12*m03 - m32*m13*m01 - m33*m11*m02;
  d21 = m00*m12*m33 + m02*m13*m30 + m03*m10*m32 - m30*m12*m03 - m32*m13*m00 - m33*m10*m02;
  d22 = m00*m11*m33 + m01*m13*m30 + m03*m10*m31 - m30*m11*m03 - m31*m13*m00 - m33*m10*m01;
  d23 = m00*m11*m32 + m01*m12*m30 + m02*m10*m31 - m30*m11*m02 - m31*m12*m00 - m32*m10*m01;

  d30 = m01*m12*m23 + m02*m13*m21 + m03*m11*m22 - m21*m12*m03 - m22*m13*m01 - m23*m11*m02;
  d31 = m00*m12*m23 + m02*m13*m20 + m03*m10*m22 - m20*m12*m03 - m22*m13*m00 - m23*m10*m02;
  d32 = m00*m11*m23 + m01*m13*m20 + m03*m10*m21 - m20*m11*m03 - m21*m13*m00 - m23*m10*m01;
  d33 = m00*m11*m22 + m01*m12*m20 + m02*m10*m21 - m20*m11*m02 - m21*m12*m00 - m22*m10*m01;

  D = m00*d00 - m01*d01 + m02*d02 - m03*d03;

  if (D == 0.0)
  {
    Error("Singular matrix in MInvers.\n");
  }

  r[0][0] =  d00/D; r[0][1] = -d10/D;  r[0][2] =  d20/D; r[0][3] = -d30/D;
  r[1][0] = -d01/D; r[1][1] =  d11/D;  r[1][2] = -d21/D; r[1][3] =  d31/D;
  r[2][0] =  d02/D; r[2][1] = -d12/D;  r[2][2] =  d22/D; r[2][3] = -d32/D;
  r[3][0] = -d03/D; r[3][1] =  d13/D;  r[3][2] = -d23/D; r[3][3] =  d33/D;
}

UV_VECT *Create_UV_Vect ()
{
  UV_VECT *New;

  New = (UV_VECT *)POV_MALLOC(sizeof (UV_VECT), "uv vector");

  (*New)[0]= 0.0;
  (*New)[1]= 0.0;

  return (New);
}

VECTOR_4D *Create_Vector_4D ()
{
  VECTOR_4D *New;

  New = (VECTOR_4D *)POV_MALLOC(sizeof (VECTOR_4D), "4d vector");

  (*New)[0]= 0.0;
  (*New)[1]= 0.0;
  (*New)[2]= 0.0;
  (*New)[3]= 0.0;

  return (New);
}

