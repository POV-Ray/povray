/*==============================================================================================*/
/*   vector.h                                                                      Font3D       */
/*----------------------------------------------------------------------------------------------*/
/*                                                                                              */
/*   Copyright (c) 1994, 1995 by Todd A. Prater                                 Version 1.50    */
/*   All rights reserved.                                                                       */
/*                                                                                              */
/*----------------------------------------------------------------------------------------------*/
/*                                                                                              */
/*   A vector class.                                                                            */
/*                                                                                              */
/*   Constructors:                                                                              */
/*                                                                                              */
/*      VECTOR(void)                                                                            */
/*      VECTOR(DOUBLE,DOUBLE,DOUBLE)                                                            */
/*      VECTOR(VECTOR&)                                                                         */
/*                                                                                              */
/*   Member Operators:                                                                          */
/*                                                                                              */
/*      +=     Assign Sum                                                                       */
/*      -=     Assign Difference                                                                */
/*      *=     Assign Product                                                                   */
/*      /=     Assign Quotient                                                                  */
/*                                                                                              */
/*   Non-Member Operators:                                                                      */
/*                                                                                              */
/*      +      Sum            Binary      (VECTOR + VECTOR) -> VECTOR                           */
/*      -      Difference     Binary      (VECTOR - VECTOR) -> VECTOR                           */
/*      *      Scale          Binary      (VECTOR * DOUBLE) -> VECTOR                           */
/*      *      Scale          Binary      (DOUBLE * VECTOR) -> VECTOR                           */
/*      /      Scale          Binary      (VECTOR / DOUBLE) -> VECTOR                           */
/*      %      Dot Product    Binary      (VECTOR % VECTOR) -> DOUBLE                           */
/*      ^      Cross Product  Binary      (VECTOR ^ VECTOR) -> VECTOR                           */
/*      ~      Normalize      Unary       (~VECTOR)         -> VECTOR                           */
/*      -      Negative       Unary       (-VECTOR)         -> VECTOR                           */
/*                                                                                              */
/*      ==     Equal to       Binary      (VECTOR == VECTOR) -> INT                             */
/*      !=     Not equal to   Binary      (VECTOR !- VECTOR) -> INT                             */
/*                                                                                              */
/*      >>     Insertion      Binary      (ostream& >> VECTOR&) -> ostream&                     */
/*                                                                                              */
/*   Non-Member Functions:                                                                      */
/*                                                                                              */
/*      dist(VECTOR x, VECTOR y) ...... Returns the distance between x and y.                   */
/*      midpoint(VECTOR x, VECTOR y) .. Returns the midpoint between x and y.                   */
/*      length(VECTOR x) .............. Returns the length of x.                                */
/*                                                                                              */
/*                                                                                              */
/*==============================================================================================*/

#ifndef __Vector_H__
#define __Vector_H__

#include <math.h>
#include <iostream.h>
#include "config.h"

   class VECTOR
   {
     public:  DOUBLE  x,y,z;

              VECTOR ();
              VECTOR (DOUBLE _x, DOUBLE _y, DOUBLE _z);
              VECTOR (const VECTOR& v);

              VECTOR& operator += (VECTOR v);
              VECTOR& operator -= (VECTOR v);
              VECTOR& operator *= (DOUBLE f);
              VECTOR& operator /= (DOUBLE f);
   };


   /*_____ CONSTRUCTORS _____*/


   inline VECTOR::VECTOR(void)
   {
     x=0;y=0;z=0;
   }

   inline VECTOR::VECTOR(DOUBLE _x, DOUBLE _y, DOUBLE _z)
   {
     x=_x;y=_y;z=_z;
   }

   inline VECTOR::VECTOR(const VECTOR& v)
   {
     x=v.x;y=v.y;z=v.z;
   }


   /*_____ NON-MEMBER FUNCTIONS _____*/


   inline VECTOR operator + (VECTOR v1, VECTOR v2)
   {
     return VECTOR(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);
   }

   inline VECTOR operator - (VECTOR v1, VECTOR v2)
   {
     return VECTOR(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);
   }

   inline VECTOR operator * (VECTOR v, DOUBLE f)
   {
     return VECTOR(v.x*f,v.y*f,v.z*f);
   }

   inline VECTOR operator * (DOUBLE f, VECTOR v)
   {
     return VECTOR(v.x*f,v.y*f,v.z*f);
   }

   inline VECTOR operator / (VECTOR v, DOUBLE f)
   {
     return VECTOR(v.x/f,v.y/f,v.z/f);
   }

   inline DOUBLE operator % (VECTOR v1, VECTOR v2)
   {
     return (v1.x*v2.x+v1.y*v2.y+v1.z*v2.z);
   }

   inline VECTOR operator ^ (VECTOR v1, VECTOR v2)
   {
     DOUBLE  tx = v1.y*v2.z - v1.z*v2.y;
     DOUBLE  ty = v1.z*v2.x - v1.x*v2.z;
     DOUBLE  tz = v1.x*v2.y - v1.y*v2.x;
     return  VECTOR(tx,ty,tz);
   }

   inline VECTOR operator ~ (VECTOR v)
   {
     DOUBLE  mag = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
     DOUBLE  tx = v.x/mag;
     DOUBLE  ty = v.y/mag;
     DOUBLE  tz = v.z/mag;
     return  VECTOR(tx,ty,tz);
   }

   inline VECTOR operator - (VECTOR v)
   {
     return VECTOR(-v.x,-v.y,-v.z);
   }

   inline int operator == (VECTOR v1, VECTOR v2)
   {
     return (v1.x==v2.x&&v1.y==v2.y&&v1.z==v2.z);
   }

   inline int operator != (VECTOR v1, VECTOR v2)
   {
     return (v1.x!=v2.x&&v1.y!=v2.y&&v1.z!=v2.z);
   }


   inline DOUBLE dist (VECTOR v1, VECTOR v2)
   {
     VECTOR d = v2-v1;
     return (sqrt(d%d));
   }

   inline VECTOR midpoint (VECTOR v1, VECTOR v2)
   {
      return ((v1+v2)/2);
   }

   inline DOUBLE length (VECTOR v)
   {
      return (sqrt(v.x*v.x+v.y*v.y+v.z*v.z));
   }

   /*_____ MEMBER SHORTHAND OPERATORS _____*/

   inline VECTOR& VECTOR::operator += (VECTOR v)
   {
     x+=v.x; y+=v.y; z+=v.z;
     return *this;
   }

   inline VECTOR& VECTOR::operator -= (VECTOR v)
   {
     x-=v.x; y-=v.y; z-=v.z;
     return *this;
   }

   inline VECTOR& VECTOR::operator *= (DOUBLE f)
   {
     x*=f; y*=f; z*=f;
     return *this;
   }

   inline VECTOR& VECTOR::operator /= (DOUBLE f)
   {
     x/=f; y/=f; z/=f;
     return *this;
   }

   inline ostream& operator << (ostream& s, VECTOR v)
   {
      s<<"<"<<v.x<<","<<v.y<<","<<v.z<<">";
      return s;
   }


#endif
