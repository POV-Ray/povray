/****************************************************************************
*                triangle.c
*
*  This module implements primitives for triangles and smooth triangles.
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
#include "matrices.h"
#include "objects.h"
#include "triangle.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define DEPTH_TOLERANCE 1e-6

#define max3_coordinate(x,y,z) \
  ((x > y) ? ((x > z) ? X : Z) : ((y > z) ? Y : Z))



/*****************************************************************************
* Static functions
******************************************************************************/

static void find_triangle_dominant_axis (TRIANGLE *Triangle);
static int compute_smooth_triangle  (SMOOTH_TRIANGLE *Triangle);
static int Intersect_Triangle  (RAY *Ray, TRIANGLE *Triangle, DBL *Depth);
static int All_Triangle_Intersections  (OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack);
static int Inside_Triangle  (VECTOR IPoint, OBJECT *Object);
static void Triangle_Normal  (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static TRIANGLE *Copy_Triangle  (OBJECT *Object);
static void Translate_Triangle  (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Triangle  (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Triangle  (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Triangle  (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Triangle  (OBJECT *Object);
static void Smooth_Triangle_Normal  (VECTOR Result, OBJECT *Object, INTERSECTION *Inter);
static SMOOTH_TRIANGLE *Copy_Smooth_Triangle (OBJECT *Object);
static void Translate_Smooth_Triangle  (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Rotate_Smooth_Triangle  (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Scale_Smooth_Triangle  (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
static void Transform_Smooth_Triangle  (OBJECT *Object, TRANSFORM *Trans);
static void Invert_Smooth_Triangle  (OBJECT *Object);
static void Destroy_Triangle  (OBJECT *Object);



/*****************************************************************************
* Local variables
******************************************************************************/

METHODS Triangle_Methods =
{
  All_Triangle_Intersections,
  Inside_Triangle, Triangle_Normal,
  (COPY_METHOD)Copy_Triangle,
  Translate_Triangle, Rotate_Triangle,
  Scale_Triangle, Transform_Triangle, Invert_Triangle, Destroy_Triangle
};

METHODS Smooth_Triangle_Methods =
{
  All_Triangle_Intersections,
  Inside_Triangle, Smooth_Triangle_Normal,
  (COPY_METHOD)Copy_Smooth_Triangle,
  Translate_Smooth_Triangle, Rotate_Smooth_Triangle,
  Scale_Smooth_Triangle, Transform_Smooth_Triangle,
  Invert_Smooth_Triangle, Destroy_Triangle
};


/*****************************************************************************
*
* FUNCTION
*
*   find_triangle_dominant_axis
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

static void find_triangle_dominant_axis(TRIANGLE *Triangle)
{
  DBL x, y, z;

  x = fabs(Triangle->Normal_Vector[X]);
  y = fabs(Triangle->Normal_Vector[Y]);
  z = fabs(Triangle->Normal_Vector[Z]);

  Triangle->Dominant_Axis = max3_coordinate(x, y, z);
}



/*****************************************************************************
*
* FUNCTION
*
*   compute_smooth_triangle
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

static int compute_smooth_triangle(SMOOTH_TRIANGLE *Triangle)
{
  VECTOR P3MinusP2, VTemp1, VTemp2;
  DBL x, y, z, uDenominator, Proj;

  VSub(P3MinusP2, Triangle->P3, Triangle->P2);

  x = fabs(P3MinusP2[X]);
  y = fabs(P3MinusP2[Y]);
  z = fabs(P3MinusP2[Z]);

  Triangle->vAxis = max3_coordinate(x, y, z);

  VSub(VTemp1, Triangle->P2, Triangle->P3);

  VNormalize(VTemp1, VTemp1);

  VSub(VTemp2, Triangle->P1, Triangle->P3);

  VDot(Proj, VTemp2, VTemp1);

  VScaleEq(VTemp1, Proj);

  VSub(Triangle->Perp, VTemp1, VTemp2);

  VNormalize(Triangle->Perp, Triangle->Perp);

  VDot(uDenominator, VTemp2, Triangle->Perp);

  VInverseScaleEq(Triangle->Perp, -uDenominator);
  
  /* Degenerate if smooth normals are more than 90 from actual normal
     or its inverse. */
  VDot(x,Triangle->Normal_Vector,Triangle->N1);
  VDot(y,Triangle->Normal_Vector,Triangle->N2);
  VDot(z,Triangle->Normal_Vector,Triangle->N3);
  if ( ((x<0.0) && (y<0.0) && (z<0.0)) ||
       ((x>0.0) && (y>0.0) && (z>0.0)) )
  {
    return(TRUE);
  }
  Set_Flag(Triangle, DEGENERATE_FLAG);
  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Triangle
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

int Compute_Triangle(TRIANGLE *Triangle,int Smooth)
{
  int swap,degn;
  VECTOR V1, V2, Temp;
  DBL Length;

  VSub(V1, Triangle->P1, Triangle->P2);
  VSub(V2, Triangle->P3, Triangle->P2);

  VCross(Triangle->Normal_Vector, V1, V2);

  VLength(Length, Triangle->Normal_Vector);

  /* Set up a flag so we can ignore degenerate triangles */

  if (Length == 0.0)
  {
    Set_Flag(Triangle, DEGENERATE_FLAG);

    return(FALSE);
  }

  /* Normalize the normal vector. */

  VInverseScaleEq(Triangle->Normal_Vector, Length);

  VDot(Triangle->Distance, Triangle->Normal_Vector, Triangle->P1);

  Triangle->Distance *= -1.0;

  find_triangle_dominant_axis(Triangle);

  swap = FALSE;

  switch (Triangle->Dominant_Axis)
  {
    case X:

      if ((Triangle->P2[Y] - Triangle->P3[Y])*(Triangle->P2[Z] - Triangle->P1[Z]) <
          (Triangle->P2[Z] - Triangle->P3[Z])*(Triangle->P2[Y] - Triangle->P1[Y]))
      {
        swap = TRUE;
      }

      break;

    case Y:

      if ((Triangle->P2[X] - Triangle->P3[X])*(Triangle->P2[Z] - Triangle->P1[Z]) <
          (Triangle->P2[Z] - Triangle->P3[Z])*(Triangle->P2[X] - Triangle->P1[X]))
      {
        swap = TRUE;
      }

      break;

    case Z:

      if ((Triangle->P2[X] - Triangle->P3[X])*(Triangle->P2[Y] - Triangle->P1[Y]) <
          (Triangle->P2[Y] - Triangle->P3[Y])*(Triangle->P2[X] - Triangle->P1[X]))
      {
        swap = TRUE;
      }

      break;
  }

  if (swap)
  {
    Assign_Vector(Temp, Triangle->P2);
    Assign_Vector(Triangle->P2, Triangle->P1);
    Assign_Vector(Triangle->P1, Temp);

    if (Smooth)
    {
      Assign_Vector(Temp, ((SMOOTH_TRIANGLE *)Triangle)->N2);
      Assign_Vector(((SMOOTH_TRIANGLE *)Triangle)->N2, ((SMOOTH_TRIANGLE *)Triangle)->N1);
      Assign_Vector(((SMOOTH_TRIANGLE *)Triangle)->N1, Temp);
    }
  }
  
  degn=TRUE;

  if (Smooth)
  {
    degn=compute_smooth_triangle((SMOOTH_TRIANGLE *)Triangle);
  }

  /* Build the bounding information from the vertices. */

  Compute_Triangle_BBox(Triangle);

  return(degn);
}



/*****************************************************************************
*
* FUNCTION
*
*   All_Triangle_Intersections
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

static int All_Triangle_Intersections(OBJECT *Object, RAY *Ray, ISTACK *Depth_Stack)
{
  DBL Depth;
  VECTOR IPoint;

  if (Intersect_Triangle(Ray, (TRIANGLE *)Object, &Depth))
  {
    VEvaluateRay(IPoint, Ray->Initial, Depth, Ray->Direction);

    if (Point_In_Clip(IPoint,Object->Clip))
    {
      push_entry(Depth,IPoint,Object,Depth_Stack);

      return(TRUE);
    }
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Triangle
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

static int Intersect_Triangle(RAY *Ray, TRIANGLE *Triangle, DBL *Depth)
{
  DBL NormalDotOrigin, NormalDotDirection;
  DBL s, t;

  Increase_Counter(stats[Ray_Triangle_Tests]);

  if (Test_Flag(Triangle, DEGENERATE_FLAG))
  {
    return(FALSE);
  }

  VDot(NormalDotDirection, Triangle->Normal_Vector, Ray->Direction);

  if (fabs(NormalDotDirection) < EPSILON)
  {
    return(FALSE);
  }

  VDot(NormalDotOrigin, Triangle->Normal_Vector, Ray->Initial);

  *Depth = -(Triangle->Distance + NormalDotOrigin) / NormalDotDirection;

  if ((*Depth < DEPTH_TOLERANCE) || (*Depth > Max_Distance))
  {
    return(FALSE);
  }

  switch (Triangle->Dominant_Axis)
  {
    case X:

      s = Ray->Initial[Y] + *Depth * Ray->Direction[Y];
      t = Ray->Initial[Z] + *Depth * Ray->Direction[Z];

      if ((Triangle->P2[Y] - s) * (Triangle->P2[Z] - Triangle->P1[Z]) <
          (Triangle->P2[Z] - t) * (Triangle->P2[Y] - Triangle->P1[Y]))
      {
        return(FALSE);
      }

      if ((Triangle->P3[Y] - s) * (Triangle->P3[Z] - Triangle->P2[Z]) <
          (Triangle->P3[Z] - t) * (Triangle->P3[Y] - Triangle->P2[Y]))
      {
        return(FALSE);
      }

      if ((Triangle->P1[Y] - s) * (Triangle->P1[Z] - Triangle->P3[Z]) <
          (Triangle->P1[Z] - t) * (Triangle->P1[Y] - Triangle->P3[Y]))
      {
        return(FALSE);
      }

      Increase_Counter(stats[Ray_Triangle_Tests_Succeeded]);

      return(TRUE);

    case Y:

      s = Ray->Initial[X] + *Depth * Ray->Direction[X];
      t = Ray->Initial[Z] + *Depth * Ray->Direction[Z];

      if ((Triangle->P2[X] - s) * (Triangle->P2[Z] - Triangle->P1[Z]) <
          (Triangle->P2[Z] - t) * (Triangle->P2[X] - Triangle->P1[X]))
      {
        return(FALSE);
      }

      if ((Triangle->P3[X] - s) * (Triangle->P3[Z] - Triangle->P2[Z]) <
          (Triangle->P3[Z] - t) * (Triangle->P3[X] - Triangle->P2[X]))
      {
        return(FALSE);
      }

      if ((Triangle->P1[X] - s) * (Triangle->P1[Z] - Triangle->P3[Z]) <
          (Triangle->P1[Z] - t) * (Triangle->P1[X] - Triangle->P3[X]))
      {
        return(FALSE);
      }

      Increase_Counter(stats[Ray_Triangle_Tests_Succeeded]);

      return(TRUE);

    case Z:

      s = Ray->Initial[X] + *Depth * Ray->Direction[X];
      t = Ray->Initial[Y] + *Depth * Ray->Direction[Y];

      if ((Triangle->P2[X] - s) * (Triangle->P2[Y] - Triangle->P1[Y]) <
          (Triangle->P2[Y] - t) * (Triangle->P2[X] - Triangle->P1[X]))
      {
        return(FALSE);
      }

      if ((Triangle->P3[X] - s) * (Triangle->P3[Y] - Triangle->P2[Y]) <
          (Triangle->P3[Y] - t) * (Triangle->P3[X] - Triangle->P2[X]))
      {
        return(FALSE);
      }

      if ((Triangle->P1[X] - s) * (Triangle->P1[Y] - Triangle->P3[Y]) <
          (Triangle->P1[Y] - t) * (Triangle->P1[X] - Triangle->P3[X]))
      {
        return(FALSE);
      }

      Increase_Counter(stats[Ray_Triangle_Tests_Succeeded]);

      return(TRUE);
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Triangle
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

static int Inside_Triangle(VECTOR IPoint, OBJECT *Object)
{
  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Triangle_Normal
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

static void Triangle_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  Assign_Vector(Result, ((TRIANGLE *)Object)->Normal_Vector);
}



/*****************************************************************************
*
* FUNCTION
*
*   Smooth_Triangle_Normal
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
*   Calculate the Phong-interpolated vector within the triangle
*   at the given intersection point. The math for this is a bit
*   bizarre:
*
*      -         P1
*      |        /|\ \
*      |       / |Perp\
*      |      /  V  \   \
*      |     /   |    \   \
*    u |    /____|_____PI___\
*      |   /     |       \    \
*      -  P2-----|--------|----P3
*                Pbase    PIntersect
*          |-------------------|
*                         v
*
*   Triangle->Perp is a unit vector from P1 to Pbase. We calculate
*
*   u = (PI - P1) DOT Perp / ((P3 - P1) DOT Perp).
*
*   We then calculate where the line from P1 to PI intersects the line P2 to P3:
*   PIntersect = (PI - P1)/u.
*
*   We really only need one coordinate of PIntersect.  We then calculate v as:
*
*        v = PIntersect[X] / (P3[X] - P2[X])
*   or   v = PIntersect[Y] / (P3[Y] - P2[Y])
*   or   v = PIntersect[Z] / (P3[Z] - P2[Z])
*
*   depending on which calculation will give us the best answers.
*
*   Once we have u and v, we can perform the normal interpolation as:
*
*     NTemp1 = N1 + u(N2 - N1);
*     NTemp2 = N1 + u(N3 - N1);
*     Result = normalize (NTemp1 + v(NTemp2 - NTemp1))
*
*   As always, any values which are constant for the triangle are cached
*   in the triangle.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void Smooth_Triangle_Normal(VECTOR Result, OBJECT *Object, INTERSECTION *Inter)
{
  int Axis;
  DBL u, v;
  VECTOR PIMinusP1;
  SMOOTH_TRIANGLE *Triangle = (SMOOTH_TRIANGLE *)Object;

  VSub(PIMinusP1, Inter->IPoint, Triangle->P1);

  VDot(u, PIMinusP1, Triangle->Perp);

  if (u < EPSILON)
  {
    Assign_Vector(Result, Triangle->N1);

    return;
  }

  Axis = Triangle->vAxis;

  v = (PIMinusP1[Axis] / u + Triangle->P1[Axis] - Triangle->P2[Axis]) / (Triangle->P3[Axis] - Triangle->P2[Axis]);

  /* This is faster. [DB 8/94] */

  Result[X] = Triangle->N1[X] + u * (Triangle->N2[X] - Triangle->N1[X] + v * (Triangle->N3[X] - Triangle->N2[X]));
  Result[Y] = Triangle->N1[Y] + u * (Triangle->N2[Y] - Triangle->N1[Y] + v * (Triangle->N3[Y] - Triangle->N2[Y]));
  Result[Z] = Triangle->N1[Z] + u * (Triangle->N2[Z] - Triangle->N1[Z] + v * (Triangle->N3[Z] - Triangle->N2[Z]));

  VNormalize(Result, Result);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Triangle
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

static void Translate_Triangle(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  TRIANGLE *Triangle = (TRIANGLE *)Object;
  VECTOR Translation;

  if (!Test_Flag(Triangle, DEGENERATE_FLAG))
  {
    VEvaluate(Translation, Triangle->Normal_Vector, Vector);

    Triangle->Distance -= Translation[X] + Translation[Y] + Translation[Z];

    VAddEq(Triangle->P1, Vector);
    VAddEq(Triangle->P2, Vector);
    VAddEq(Triangle->P3, Vector);

    Compute_Triangle(Triangle, FALSE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Triangle
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

static void Rotate_Triangle(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  if (!Test_Flag(Object, DEGENERATE_FLAG))
  {
    Transform_Triangle(Object, Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Triangle
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

static void Scale_Triangle(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  DBL Length;
  TRIANGLE *Triangle = (TRIANGLE *)Object;

  if (!Test_Flag(Object, DEGENERATE_FLAG))
  {
    Triangle->Normal_Vector[X] = Triangle->Normal_Vector[X] / Vector[X];
    Triangle->Normal_Vector[Y] = Triangle->Normal_Vector[Y] / Vector[Y];
    Triangle->Normal_Vector[Z] = Triangle->Normal_Vector[Z] / Vector[Z];

    VLength(Length, Triangle->Normal_Vector);

    VInverseScaleEq(Triangle->Normal_Vector, Length);

    Triangle->Distance /= Length;

    VEvaluateEq(Triangle->P1, Vector);
    VEvaluateEq(Triangle->P2, Vector);
    VEvaluateEq(Triangle->P3, Vector);

    Compute_Triangle(Triangle, FALSE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Transfrom_Triangle
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

static void Transform_Triangle(OBJECT *Object, TRANSFORM *Trans)
{
  TRIANGLE *Triangle = (TRIANGLE *)Object;

  if (!Test_Flag(Object, DEGENERATE_FLAG))
  {
    MTransPoint(Triangle->Normal_Vector,Triangle->Normal_Vector, Trans);
    MTransPoint(Triangle->P1, Triangle->P1, Trans);
    MTransPoint(Triangle->P2, Triangle->P2, Trans);
    MTransPoint(Triangle->P3, Triangle->P3, Trans);

    Compute_Triangle(Triangle, FALSE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Triangle
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

static void Invert_Triangle(OBJECT *Object)
{
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Triangle
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

TRIANGLE *Create_Triangle()
{
  TRIANGLE *New;

  New = (TRIANGLE *)POV_MALLOC(sizeof(TRIANGLE), "triangle");

  INIT_OBJECT_FIELDS(New,TRIANGLE_OBJECT,&Triangle_Methods)

  Make_Vector(New->Normal_Vector, 0.0, 1.0, 0.0);

  New->Distance = 0.0;

  Make_Vector(New->P1, 0.0, 0.0, 0.0);
  Make_Vector(New->P2, 1.0, 0.0, 0.0);
  Make_Vector(New->P3, 0.0, 1.0, 0.0);

  /*
   * NOTE: Dominant_Axis is computed when Parse_Triangle calls
   * Compute_Triangle. vAxis is used only for smooth triangles.
   */

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Triangle
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

static TRIANGLE *Copy_Triangle(OBJECT *Object)
{
  TRIANGLE *New;

  New = Create_Triangle();

  *New = *((TRIANGLE *)Object);

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Triangle
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

static void Destroy_Triangle(OBJECT *Object)
{
  POV_FREE (Object);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Smooth_Triangle
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

static void Translate_Smooth_Triangle(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  SMOOTH_TRIANGLE *Triangle = (SMOOTH_TRIANGLE *)Object;
  VECTOR Translation;

  if (!Test_Flag(Object, DEGENERATE_FLAG))
  {
    VEvaluate(Translation, Triangle->Normal_Vector, Vector);

    Triangle->Distance -= Translation[X] + Translation[Y] + Translation[Z];

    VAddEq(Triangle->P1, Vector);
    VAddEq(Triangle->P2, Vector);
    VAddEq(Triangle->P3, Vector);

    Compute_Triangle((TRIANGLE *)Triangle, TRUE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Smooth_Triangle
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

static void Rotate_Smooth_Triangle(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  if (!Test_Flag(Object, DEGENERATE_FLAG))
  {
    Transform_Smooth_Triangle(Object, Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Smooth_Triangle
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

static void Scale_Smooth_Triangle(OBJECT *Object, VECTOR Vector, TRANSFORM *Trans)
{
  DBL Length;
  SMOOTH_TRIANGLE *Triangle = (SMOOTH_TRIANGLE *)Object;

  if (!Test_Flag(Object, DEGENERATE_FLAG))
  {
    Triangle->Normal_Vector[X] = Triangle->Normal_Vector[X] / Vector[X];
    Triangle->Normal_Vector[Y] = Triangle->Normal_Vector[Y] / Vector[Y];
    Triangle->Normal_Vector[Z] = Triangle->Normal_Vector[Z] / Vector[Z];

    VLength(Length, Triangle->Normal_Vector);
    VScaleEq(Triangle->Normal_Vector, 1.0 / Length);
    Triangle->Distance /= Length;

    VEvaluateEq(Triangle->P1, Vector);
    VEvaluateEq(Triangle->P2, Vector);
    VEvaluateEq(Triangle->P3, Vector);

    Compute_Triangle((TRIANGLE *)Triangle,TRUE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Smooth_Triangle
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

static void Transform_Smooth_Triangle(OBJECT *Object, TRANSFORM *Trans)
{
  SMOOTH_TRIANGLE *Triangle = (SMOOTH_TRIANGLE *)Object;

  if (!Test_Flag(Object, DEGENERATE_FLAG))
  {
    MTransPoint(Triangle->Normal_Vector,Triangle->Normal_Vector, Trans);
    MTransPoint(Triangle->P1, Triangle->P1, Trans);
    MTransPoint(Triangle->P2, Triangle->P2, Trans);
    MTransPoint(Triangle->P3, Triangle->P3, Trans);
    MTransPoint(Triangle->N1, Triangle->N1, Trans);
    MTransPoint(Triangle->N2, Triangle->N2, Trans);
    MTransPoint(Triangle->N3, Triangle->N3, Trans);

    Compute_Triangle((TRIANGLE *)Triangle, TRUE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Smooth_Triangle
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

static void Invert_Smooth_Triangle(OBJECT *Object)
{
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Smooth_Triangle
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

SMOOTH_TRIANGLE *Create_Smooth_Triangle()
{
  SMOOTH_TRIANGLE *New;

  New = (SMOOTH_TRIANGLE *)POV_MALLOC(sizeof(SMOOTH_TRIANGLE), "smooth triangle");

  INIT_OBJECT_FIELDS(New,SMOOTH_TRIANGLE_OBJECT,&Smooth_Triangle_Methods)

  Make_Vector(New->Normal_Vector, 0.0, 1.0, 0.0);

  New->Distance = 0.0;

  Make_Vector(New->P1, 0.0, 0.0, 0.0);
  Make_Vector(New->P2, 1.0, 0.0, 0.0);
  Make_Vector(New->P3, 0.0, 1.0, 0.0);
  Make_Vector(New->N1, 0.0, 1.0, 0.0);
  Make_Vector(New->N2, 0.0, 1.0, 0.0);
  Make_Vector(New->N3, 0.0, 1.0, 0.0);

  /*
   * NOTE: Dominant_Axis and vAxis are computed when
   * Parse_Triangle calls Compute_Triangle.
   */

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Smooth_Triangle
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

static SMOOTH_TRIANGLE *Copy_Smooth_Triangle(OBJECT *Object)
{
  SMOOTH_TRIANGLE *New;

  New = Create_Smooth_Triangle();

  *New = *((SMOOTH_TRIANGLE *)Object);

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Triangle_BBox
*
* INPUT
*
*   Triangle - Triangle
*   
* OUTPUT
*
*   Triangle
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the bounding box of a triangle.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Compute_Triangle_BBox(TRIANGLE *Triangle)
{
  VECTOR Min, Max, Epsilon;

  Make_Vector(Epsilon, EPSILON, EPSILON, EPSILON);

  Min[X] = min3(Triangle->P1[X], Triangle->P2[X], Triangle->P3[X]);
  Min[Y] = min3(Triangle->P1[Y], Triangle->P2[Y], Triangle->P3[Y]);
  Min[Z] = min3(Triangle->P1[Z], Triangle->P2[Z], Triangle->P3[Z]);

  Max[X] = max3(Triangle->P1[X], Triangle->P2[X], Triangle->P3[X]);
  Max[Y] = max3(Triangle->P1[Y], Triangle->P2[Y], Triangle->P3[Y]);
  Max[Z] = max3(Triangle->P1[Z], Triangle->P2[Z], Triangle->P3[Z]);

  VSubEq(Min, Epsilon);
  VAddEq(Max, Epsilon);

  Make_BBox_from_min_max(Triangle->BBox, Min, Max);
}


