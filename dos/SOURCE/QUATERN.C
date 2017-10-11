/*****************************************************************************
*                quatern.c
*
*  This module implements Quaternion algebra julia fractals.
*
*  This file was written by Pascal Massimino.
*  Revised and updated for POV-Ray 3.x by Tim Wegner
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

#include "frame.h"
#include "povray.h"
#include "vector.h"
#include "povproto.h"
#include "fractal.h"
#include "quatern.h"
#include "spheres.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#ifndef Fractal_Tolerance
#define Fractal_Tolerance 1e-8
#endif

#define Deriv_z2(n1,n2,n3,n4)               \
{                                           \
  tmp = (n1)*x - (n2)*y - (n3)*z - (n4)*w;  \
  (n2) = (n1)*y + x*(n2);                   \
  (n3) = (n1)*z + x*(n3);                   \
  (n4) = (n1)*w + x*(n4);                   \
  (n1) = tmp;                               \
}

#define Deriv_z3(n1,n2,n3,n4)              \
{                                          \
  dtmp = 2.0*((n2)*y + (n3)*z + (n4)*w);   \
  dtmp2 = 6.0*x*(n1) - dtmp;               \
  (n1) = ( (n1)*x3 - x*dtmp )*3.0;         \
  (n2) = (n2)*x4 + y*dtmp2;                \
  (n3) = (n3)*x4 + z*dtmp2;                \
  (n4) = (n4)*x4 + w*dtmp2;                \
}


/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/



/*****************************************************************************
* Local variables
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
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

int Iteration_z3(VECTOR point, FRACTAL *Julia)
{
  int i;
  DBL x, y, z, w;
  DBL d, x2, tmp;
  DBL Exit_Value;

  Sx[0] = x = point[X];
  Sy[0] = y = point[Y];
  Sz[0] = z = point[Z];
  Sw[0] = w = (Julia->SliceDist  
                  - Julia->Slice[X]*x 
                  - Julia->Slice[Y]*y 
                  - Julia->Slice[Z]*z)/Julia->Slice[T]; 
  
  Exit_Value = Julia->Exit_Value;

  for (i = 1; i <= Julia->n; ++i)
  {
    d = y * y + z * z + w * w;

    x2 = x * x;

    if ((d + x2) > Exit_Value)
    {
      return (FALSE);
    }

    tmp = 3.0 * x2 - d;

    Sx[i] = x = x * (x2 - 3.0 * d) + Julia->Julia_Parm[X];
    Sy[i] = y = y * tmp + Julia->Julia_Parm[Y];
    Sz[i] = z = z * tmp + Julia->Julia_Parm[Z];
    Sw[i] = w = w * tmp + Julia->Julia_Parm[T];
  }

  return (TRUE);
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
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

int Iteration_Julia(VECTOR point, FRACTAL *Julia)
{
  int i;
  DBL x, y, z, w;
  DBL d, x2;
  DBL Exit_Value;

  Sx[0] = x = point[X];
  Sy[0] = y = point[Y];
  Sz[0] = z = point[Z];
  Sw[0] = w = (Julia->SliceDist  
                  - Julia->Slice[X]*x 
                  - Julia->Slice[Y]*y 
                  - Julia->Slice[Z]*z)/Julia->Slice[T]; 

  Exit_Value = Julia->Exit_Value;

  for (i = 1; i <= Julia->n; ++i)
  {
    d = y * y + z * z + w * w;

    x2 = x * x;

    if ((d + x2) > Exit_Value)
    {
      return (FALSE);
    }

    x *= 2.0;

    Sy[i] = y = x * y + Julia->Julia_Parm[Y];
    Sz[i] = z = x * z + Julia->Julia_Parm[Z];
    Sw[i] = w = x * w + Julia->Julia_Parm[T];
    Sx[i] = x = x2 - d + Julia->Julia_Parm[X];;

  }

  return (TRUE);
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
*   Pascal Massimino
*
* DESCRIPTION
*
* D_Iteration puts in *Dist a lower bound for the distance from *point to the
* set
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

/*----------- Distance estimator + iterations ------------*/

int D_Iteration_z3(VECTOR point, FRACTAL *Julia, DBL *Dist)
{
  int i, j;
  DBL Norm, d;
  DBL xx, yy, zz;
  DBL x, y, z, w;
  DBL tmp, x2;
  DBL Exit_Value;
  DBL Pow;

  x = Sx[0] = point[X];
  y = Sy[0] = point[Y];
  z = Sz[0] = point[Z];
  w = Sw[0] = (Julia->SliceDist  
                  - Julia->Slice[X]*x 
                  - Julia->Slice[Y]*y 
                  - Julia->Slice[Z]*z)/Julia->Slice[T]; 

  Exit_Value = Julia->Exit_Value;

  for (i = 1; i <= Julia->n; i++)
  {
    d = y * y + z * z + w * w;

    x2 = x * x;

    if ((Norm = d + x2) > Exit_Value)
    {
      /* Distance estimator */

      x = Sx[0];
      y = Sy[0];
      z = Sz[0];
      w = Sw[0];

      Pow = 1.0 / 3.0;

      for (j = 1; j < i; ++j)
      {
        xx = x * Sx[j] - y * Sy[j] - z * Sz[j] - w * Sw[j];
        yy = x * Sy[j] + y * Sx[j] - z * Sw[j] + w * Sz[j];
        zz = x * Sz[j] + y * Sw[j] + z * Sx[j] - w * Sy[j];
        w  = x * Sw[j] - y * Sz[j] + z * Sy[j] + w * Sx[j];

        x = xx;
        y = yy;
        z = zz;

        Pow /= 3.0;
      }

      *Dist = Pow * sqrt(Norm / (x * x + y * y + z * z + w * w)) * log(Norm);

      return (FALSE);
    }

    tmp = 3.0 * x2 - d;

    Sx[i] = x = x * (x2 - 3.0 * d) + Julia->Julia_Parm[X];
    Sy[i] = y = y * tmp + Julia->Julia_Parm[Y];
    Sz[i] = z = z * tmp + Julia->Julia_Parm[Z];
    Sw[i] = w = w * tmp + Julia->Julia_Parm[T];
  }

  *Dist = Precision;

  return (TRUE);
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
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

int D_Iteration_Julia(VECTOR point, FRACTAL *Julia, DBL *Dist)
{
  int i, j;
  DBL Norm, d;
  DBL Exit_Value;
  DBL x, y, z, w;
  DBL xx, yy, zz, x2;
  DBL Pow;

  x = Sx[0] = point[X];
  y = Sy[0] = point[Y];
  z = Sz[0] = point[Z];
  w = Sw[0] = (Julia->SliceDist  
                  - Julia->Slice[X]*x 
                  - Julia->Slice[Y]*y 
                  - Julia->Slice[Z]*z)/Julia->Slice[T]; 

  Exit_Value = Julia->Exit_Value;

  for (i = 1; i <= Julia->n; i++)
  {
    d = y * y + z * z + w * w;

    x2 = x * x;

    if ((Norm = d + x2) > Exit_Value)
    {
      /* Distance estimator */

      x = Sx[0];
      y = Sy[0];
      z = Sz[0];
      w = Sw[0];

      Pow = 1.0 / 2.0;

      for (j = 1; j < i; ++j)
      {
        xx = x * Sx[j] - y * Sy[j] - z * Sz[j] - w * Sw[j];
        yy = x * Sy[j] + y * Sx[j] + w * Sz[j] - z * Sw[j];
        zz = x * Sz[j] + z * Sx[j] + y * Sw[j] - w * Sy[j];
        w  = x * Sw[j] + w * Sx[j] + z * Sy[j] - y * Sz[j];

        x = xx;
        y = yy;
        z = zz;

        Pow /= 2.0;
      }

      *Dist = Pow * sqrt(Norm / (x * x + y * y + z * z + w * w)) * log(Norm);

      return (FALSE);
    }

    x *= 2.0;

    Sy[i] = y = x * y + Julia->Julia_Parm[Y];
    Sz[i] = z = x * z + Julia->Julia_Parm[Z];
    Sw[i] = w = x * w + Julia->Julia_Parm[T];
    Sx[i] = x = x2 - d + Julia->Julia_Parm[X];

  }

  *Dist = Precision;

  return (TRUE);
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
*   Pascal Massimino
*
* DESCRIPTION
*
* Provided the iterations sequence has been built, perform the computation of
* the Normal
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Normal_Calc_z3(VECTOR Result, int N_Max, FRACTAL *fractal)
{
  DBL
  n11 = 1.0, n12 = 0.0, n13 = 0.0, n14 = 0.0,
  n21 = 0.0, n22 = 1.0, n23 = 0.0, n24 = 0.0,
  n31 = 0.0, n32 = 0.0, n33 = 1.0, n34 = 0.0;

  DBL x, y, z, w;
  int i;
  DBL tmp, dtmp, dtmp2, x2, x3, x4;

  x = Sx[0];
  y = Sy[0];
  z = Sz[0];
  w = Sw[0];

  for (i = 1; i <= N_Max; i++)
  {
    tmp = y * y + z * z + w * w;

    x2 = x * x;
    x3 = x2 - tmp;
    x4 = 3.0 * x2 - tmp;

    Deriv_z3(n11, n12, n13, n14);
    Deriv_z3(n21, n22, n23, n24);
    Deriv_z3(n31, n32, n33, n34);

    x = Sx[i];
    y = Sy[i];
    z = Sz[i];
    w = Sw[i];
  }

  Result[X] = n11 * x + n12 * y + n13 * z + n14 * w;
  Result[Y] = n21 * x + n22 * y + n23 * z + n24 * w;
  Result[Z] = n31 * x + n32 * y + n33 * z + n34 * w;
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
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Normal_Calc_Julia(VECTOR Result, int N_Max, FRACTAL *fractal)
{
  DBL
  n11 = 1.0, n12 = 0.0, n13 = 0.0, n14 = 0.0,
  n21 = 0.0, n22 = 1.0, n23 = 0.0, n24 = 0.0,
  n31 = 0.0, n32 = 0.0, n33 = 1.0, n34 = 0.0;
  DBL tmp;
  DBL x, y, z, w;
  int i;

  x = Sx[0];
  y = Sy[0];
  z = Sz[0];
  w = Sw[0];

  for (i = 1; i <= N_Max; i++)
  {
    Deriv_z2(n11, n12, n13, n14);
    Deriv_z2(n21, n22, n23, n24);
    Deriv_z2(n31, n32, n33, n34);

    x = Sx[i];
    y = Sy[i];
    z = Sz[i];
    w = Sw[i];
  }

  Result[X] = n11 * x + n12 * y + n13 * z + n14 * w;
  Result[Y] = n21 * x + n22 * y + n23 * z + n24 * w;
  Result[Z] = n31 * x + n32 * y + n33 * z + n34 * w;
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
*   Pascal Massimino
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

int F_Bound_Julia(RAY *Ray, FRACTAL *Fractal, DBL *Depth_Min, DBL *Depth_Max)
{
  return (Intersect_Sphere(Ray, Fractal->Center, Fractal->Radius_Squared, Depth_Min, Depth_Max));
}
