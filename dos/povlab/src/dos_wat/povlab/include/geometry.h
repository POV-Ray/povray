/*==============================================================================================*/
/*   geometry.h                                                                    Font3D       */
/*----------------------------------------------------------------------------------------------*/
/*                                                                                              */
/*   Copyright (c) 1994, 1995 by Todd A. Prater                                 Version 1.50    */
/*   All rights reserved.                                                                       */
/*                                                                                              */
/*==============================================================================================*/

#ifndef __Geometry_H__
#define __Geometry_H__

#include <iostream.h>
#include "config.h"
#include "vector.h"


/*----------------------------------------------------------------------------------------------*/
/*  TRIANGLE                                                                                    */
/*----------------------------------------------------------------------------------------------*/

   class TRIANGLE
   {
      public:  VECTOR v1,v2,v3;
               VECTOR n1,n2,n3;

               TRIANGLE(void) {}
               TRIANGLE(VECTOR _v1, VECTOR _v2, VECTOR _v3)
               {
                  v1=_v1; v2=_v2; v3=_v3;
                  n1=VECTOR(0,0,1);
                  n2=VECTOR(0,0,1); 
                  n3=VECTOR(0,0,1);
               }

               TRIANGLE(VECTOR _v1, VECTOR _v2, VECTOR _v3,
                        VECTOR _n1, VECTOR _n2, VECTOR _n3)
               {
	          v1=_v1; v2=_v2; v3=_v3;
                  n1=_n1; n2=_n2; n3=_n3;
               }

               TRIANGLE(const TRIANGLE& t)
               {
                  v1=t.v1; v2=t.v2; v3=t.v3;
                  n1=t.n1; n2=t.n2; n3=t.n3;
               }

               VECTOR Normal(void) 
               { 
                  return ~((v3-v1)^(v2-v1)); 
               }

               void Output(ostream& s, ULONG format);

               BYTE    Orientation(void)
               {
                  if (((v2.x-v1.x)*(v3.y-v1.y)
                      -(v3.x-v1.x)*(v2.y-v1.y)) >= 0.0)
                     return COUNTER_CLOCKWISE;
                  else
                     return CLOCKWISE;
               }
   };

   ostream& operator << (ostream& s, const TRIANGLE& t);


/*----------------------------------------------------------------------------------------------*/
/*  TRIANGLELISTLINK                                                                            */
/*----------------------------------------------------------------------------------------------*/

   class TRIANGLELISTLINK
   {
      private: TRIANGLE*         obj;
               TRIANGLELISTLINK* next;

      public:  TRIANGLELISTLINK (void)
               {
                  obj = NULL;
                  next = NULL;
               }

               TRIANGLELISTLINK (TRIANGLE* object, TRIANGLELISTLINK* tonext)
               {
                  obj  = object;
                  next = tonext;
               }

               TRIANGLE* Obj(void) 
               { 
                  return obj; 
               };

               TRIANGLELISTLINK* Next(void)
               { 
                  return next; 
               };

               void setObj(TRIANGLE* object) 
               {
                  obj = object; 
               };

               void setNext(TRIANGLELISTLINK* n)  
               { 
                  next = n; 
               };

   };


/*----------------------------------------------------------------------------------------------*/
/*  TRIANGLELIST                                                                                */
/*----------------------------------------------------------------------------------------------*/

   class TRIANGLELIST
   {
      private: INT               count;
               TRIANGLELISTLINK* first;
               TRIANGLELISTLINK* current;
               TRIANGLELISTLINK* last;

      public:  TRIANGLELIST(void)
               {
                  count = 0;
                  first = NULL;
                  current = NULL;
                  last = NULL;
               }

               void Empty(void);
               BYTE Add(TRIANGLE* t);

               ULONG Count(void)  
               { 
                  return count; 
               };

               TRIANGLE* First(void)  
               { 
                  return first->Obj();
               };

               TRIANGLE* Current(void)  
               { 
                  return current->Obj();
               };

               TRIANGLE* Last(void)  
               { 
                  return last->Obj();
               };

               TRIANGLE* Next(void)
               { 
                  return current->Next()->Obj();
               };

               void gotoFirst(void)
               { 
                  current = first;
               };

               void gotoLast(void)
               { 
                  current = last;
               };

               void gotoNext(void)  
               {
                  if (current!=last)
                     current = current->Next();
               };

   };


/*----------------------------------------------------------------------------------------------*/
/*  POLYGON                                                                                     */
/*----------------------------------------------------------------------------------------------*/

   class POLYGON
   {
       public:
           INT      numpoints;
           VECTOR*  pointlist;
           USHORT   orientation;

           USHORT   findOrientation(void);
           USHORT   findDeterminant(INT p1, INT p2, INT p3);
           USHORT   noneInside(INT p1, INT p2, INT p3, INT n, INT* vlist);



           POLYGON(void);
           POLYGON(INT n, VECTOR* p);
           POLYGON(const POLYGON& P);

           INT NumPoints(void) { return numpoints; }
           void Correct(void);
           INT Triangulate(TRIANGLELIST& trianglelist);
           int isInside(POLYGON& p);
           void Combine(POLYGON& p);
           void Shrink(POLYGON& p, DOUBLE shrinkFactor);
           void Shrink(DOUBLE shrinkFactor);
           void SetDepth(DOUBLE d);
           void Translate(VECTOR offset);

   };

   ostream& operator << (ostream& s, const POLYGON& p);


   VECTOR ApproximateQuadraticSpline(VECTOR cp1, VECTOR cp2, VECTOR cp3, DOUBLE t);


#endif
