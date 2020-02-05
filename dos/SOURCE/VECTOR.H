/****************************************************************************
*                vector.h
*
*  This module contains macros to perform operations on vectors.
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

#ifndef VECTOR_H
#define VECTOR_H




/* Misc. Vector Math Macro Definitions */

extern DBL VTemp;

/* Vector Add */
#define VAdd(a, b, c) {(a)[X]=(b)[X]+(c)[X];(a)[Y]=(b)[Y]+(c)[Y];(a)[Z]=(b)[Z]+(c)[Z];}
#define VAddEq(a, b) {(a)[X]+=(b)[X];(a)[Y]+=(b)[Y];(a)[Z]+=(b)[Z];}

/* Vector Subtract */
#define VSub(a, b, c) {(a)[X]=(b)[X]-(c)[X];(a)[Y]=(b)[Y]-(c)[Y];(a)[Z]=(b)[Z]-(c)[Z];}
#define VSubEq(a, b) {(a)[X]-=(b)[X];(a)[Y]-=(b)[Y];(a)[Z]-=(b)[Z];}

/* Scale - Multiply Vector by a Scalar */
#define VScale(a, b, k) {(a)[X]=(b)[X]*(k);(a)[Y]=(b)[Y]*(k);(a)[Z]=(b)[Z]*(k);}
#define VScaleEq(a, k) {(a)[X]*=(k);(a)[Y]*=(k);(a)[Z]*=(k);}

/* Inverse Scale - Divide Vector by a Scalar */
#define VInverseScale(a, b, k) {(a)[X]=(b)[X]/(k);(a)[Y]=(b)[Y]/(k);(a)[Z]=(b)[Z]/(k);}
#define VInverseScaleEq(a, k) {(a)[X]/=(k);(a)[Y]/=(k);(a)[Z]/=(k);}

/* Dot Product - Gives Scalar angle (a) between two vectors (b) and (c) */
#define VDot(a, b, c) {a=(b)[X]*(c)[X]+(b)[Y]*(c)[Y]+(b)[Z]*(c)[Z];}

/* Cross Product - returns Vector (a) = (b) x (c) 
   WARNING:  a must be different from b and c.*/
#define VCross(a,b,c) {(a)[X]=(b)[Y]*(c)[Z]-(b)[Z]*(c)[Y]; \
                       (a)[Y]=(b)[Z]*(c)[X]-(b)[X]*(c)[Z]; \
                       (a)[Z]=(b)[X]*(c)[Y]-(b)[Y]*(c)[X];}

/* Evaluate - returns Vector (a) = Multiply Vector (b) by Vector (c) */
#define VEvaluate(a, b, c) {(a)[X]=(b)[X]*(c)[X];(a)[Y]=(b)[Y]*(c)[Y];(a)[Z]=(b)[Z]*(c)[Z];}
#define VEvaluateEq(a, b) {(a)[X]*=(b)[X];(a)[Y]*=(b)[Y];(a)[Z]*=(b)[Z];}

/* Divide - returns Vector (a) = Divide Vector (b) by Vector (c) */
#define VDiv(a, b, c) {(a)[X]=(b)[X]/(c)[X];(a)[Y]=(b)[Y]/(c)[Y];(a)[Z]=(b)[Z]/(c)[Z];}
#define VDivEq(a, b) {(a)[X]/=(b)[X];(a)[Y]/=(b)[Y];(a)[Z]/=(b)[Z];}

/* Simple Scalar Square Macro */
#define Sqr(a)  ((a)*(a))

/* Square a Vector (b) and Assign to another Vector (a) */
#define VSquareTerms(a, b) {(a)[X]=(b)[X]*(b)[X];(a)[Y]=(b)[Y]*(b)[Y];(a)[Z]=(b)[Z]*(b)[Z];}

/* Vector Length - returs Scalar Euclidean Length (a) of Vector (b) */
#define VLength(a, b) {a=sqrt((b)[X]*(b)[X]+(b)[Y]*(b)[Y]+(b)[Z]*(b)[Z]);}

/* Vector Distance - returs Scalar Euclidean Distance (a) between two
 * points/Vectors (b) and (c) */
#define VDist(a, b, c) {VECTOR tmp; VSub(tmp, b, c); VLength(a, tmp);}

/* Normalize a Vector - returns a vector (length of 1) that points at (b) */
#define VNormalize(a,b) {VTemp=1./sqrt((b)[X]*(b)[X]+(b)[Y]*(b)[Y]+(b)[Z]*(b)[Z]);(a)[X]=(b)[X]*VTemp;(a)[Y]=(b)[Y]*VTemp;(a)[Z]=(b)[Z]*VTemp;}
#define VNormalizeEq(a) {VTemp=1./sqrt((a)[X]*(a)[X]+(a)[Y]*(a)[Y]+(a)[Z]*(a)[Z]);(a)[X]*=VTemp;(a)[Y]*=VTemp;(a)[Z]*=VTemp;}

/* Compute a Vector (a) Halfway Between Two Given Vectors (b) and (c) */
#define VHalf(a, b, c) {(a)[X]=0.5*((b)[X]+(c)[X]);(a)[Y]=0.5*((b)[Y]+(c)[Y]);(a)[Z]=0.5*((b)[Z]+(c)[Z]);}

/* Calculate the sum of the sqares of the components of a vector.  (the square of its length) */
#define VSumSqr(a)  ((a)[X]*(a)[X] + (a)[Y]*(a)[Y] + (a)[Z]*(a)[Z])



/*
 * Linear combination of 2 vectors. [DB 7/94]
 *
 *   V = k1 * V1 + k2 * V2
 */
#define VLinComb2(V, k1, V1, k2, V2)            \
  { (V)[X] = (k1) * (V1)[X] + (k2) * (V2)[X];   \
    (V)[Y] = (k1) * (V1)[Y] + (k2) * (V2)[Y];   \
    (V)[Z] = (k1) * (V1)[Z] + (k2) * (V2)[Z]; }



/*
 * Linear combination of 3 vectors. [DB 7/94]
 *
 *   V = k1 * V1 + k2 * V2 + k3 * V3
 */
#define VLinComb3(V, k1, V1, k2, V2, k3, V3)                     \
  { (V)[X] = (k1) * (V1)[X] + (k2) * (V2)[X] + (k3) * (V3)[X];   \
    (V)[Y] = (k1) * (V1)[Y] + (k2) * (V2)[Y] + (k3) * (V3)[Y];   \
    (V)[Z] = (k1) * (V1)[Z] + (k2) * (V2)[Z] + (k3) * (V3)[Z]; }



/*
 * Evaluate a ray equation. [DB 7/94]
 *
 *   IPoint = Initial + depth * Direction
 */
#define VEvaluateRay(IPoint, Initial, depth, Direction)      \
  { (IPoint)[X] = (Initial)[X] + (depth) * (Direction)[X];   \
    (IPoint)[Y] = (Initial)[Y] + (depth) * (Direction)[Y];   \
    (IPoint)[Z] = (Initial)[Z] + (depth) * (Direction)[Z]; }



/*
 * Add a scaled vector. [DB 7/94]
 *
 *   V  = V1 + k * V2;
 *   V += k * V2;
 */
#define VAddScaled(V, V1, k, V2)         \
  { (V)[X] = (V1)[X] + (k) * (V2)[X];    \
    (V)[Y] = (V1)[Y] + (k) * (V2)[Y];    \
    (V)[Z] = (V1)[Z] + (k) * (V2)[Z]; }

#define VAddScaledEq(V, k, V2)  \
  { (V)[X] += (k) * (V2)[X];    \
    (V)[Y] += (k) * (V2)[Y];    \
    (V)[Z] += (k) * (V2)[Z]; }



/*
 * Subtract a scaled vector. [DB 8/94]
 *
 *   V  = V1 - k * V2;
 *   V -= k * V2;
 */
#define VSubScaled(V, V1, k, V2)         \
  { (V)[X] = (V1)[X] - (k) * (V2)[X];    \
    (V)[Y] = (V1)[Y] - (k) * (V2)[Y];    \
    (V)[Z] = (V1)[Z] - (k) * (V2)[Z]; }

#define VSubScaledEq(V, k, V2)  \
  { (V)[X] -= (k) * (V2)[X];    \
    (V)[Y] -= (k) * (V2)[Y];    \
    (V)[Z] -= (k) * (V2)[Z]; }



/*
 * Calculate the volume of a bounding box. [DB 8/94]
 */

#define BOUNDS_VOLUME(a, b)                                   \
  { (a) = (b).Lengths[X] * (b).Lengths[Y] * (b).Lengths[Z]; }


/*
 * Linear combination of 2 colours. [CEY]
 *
 *   C = k1 * C1 + k2 * C2
 */
#define CLinComb2(C, k1, C1, k2, C2)            \
  { (C)[RED]    = (k1) * (C1)[RED]    + (k2) * (C2)[RED];   \
    (C)[GREEN]  = (k1) * (C1)[GREEN]  + (k2) * (C2)[GREEN]; \
    (C)[BLUE]   = (k1) * (C1)[BLUE]   + (k2) * (C2)[BLUE];  \
    (C)[FILTER] = (k1) * (C1)[FILTER] + (k2) * (C2)[FILTER];\
    (C)[TRANSM] = (k1) * (C1)[TRANSM] + (k2) * (C2)[TRANSM];}


/* Misc. 4D Vector Math Macro Definitions */
/* Inverse Scale - Divide Vector by a Scalar */
#define V4D_InverseScale(a, b, k) {(a)[X]=(b)[X]/(k);(a)[Y]=(b)[Y]/(k);(a)[Z]=(b)[Z]/(k);(a)[T]=(b)[T]/(k);}
#define V4D_InverseScaleEq(a, k) {(a)[X]/=(k);(a)[Y]/=(k);(a)[Z]/=(k);(a)[T]/=(k);}

/* Dot Product - Gives Scalar angle (a) between two vectors (b) and (c) */
#define V4D_Dot(a, b, c) {a=(b)[X]*(c)[X]+(b)[Y]*(c)[Y]+(b)[Z]*(c)[Z]+(b)[T]*(c)[T];}

#endif


