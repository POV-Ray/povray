/* ---------------------------------------------------------------------------
*  GEOMETRY.CC
*
*  Code by Todd A. Prader.
*  from POVLAB 3D Modeller
*  Copyright 1994-1999 POVLAB Authors.
*  ---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POVLAB and to port the software to platforms other
*  than those supported by the POVLAB authors. There are strict rules under
*  which you are permitted to use this file. The rules are in the file
*  named LEGAL.TXT which should be distributed with this file.
*  If LEGAL.TXT is not available or for more info please contact the POVLAB
*  primary author by leaving a message on http://www.povlab.org
*  The latest and official version of POVLAB may be found at this site.
*
*  POVLAB was originally written by Denis Olivier.
*
*  ---------------------------------------------------------------------------*/

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <iostream.h>
#include "config.h"
#include "geometry.h"

/*==============================================================================================*/
/*  TRIANGLELIST::Empty()                                                             (PUBLIC)  */
/*----------------------------------------------------------------------------------------------*/
/*                                                                                              */
/*  This function removes all of the triangles from a triangle list.                            */
/*                                                                                              */
/*==============================================================================================*/

   void TRIANGLELIST::Empty(void)
   {
      INT i;
      TRIANGLELISTLINK* tempnext;
      gotoFirst();
      for (i=0;i<count;i++)
      {
         delete current->Obj();
         gotoNext();
      }
      gotoFirst();
      for (i=0;i<count;i++)
      {
         tempnext = current->Next();
         if (tempnext!=NULL) delete current;
         current = tempnext;
      }
      count=0;
      first=NULL;
      current=NULL;
      last=NULL;
   }


/*==============================================================================================*/
/*  TRIANGLELIST::Add()                                                               (PUBLIC)  */
/*----------------------------------------------------------------------------------------------*/
/*                                                                                              */
/*  This function adds a triangle to a triangle list.  If successful it returns TRUE, FALSE     */
/*  otherwise.                                                                                  */
/*                                                                                              */
/*==============================================================================================*/

   BYTE TRIANGLELIST::Add (TRIANGLE* object)
   {
      TRIANGLELISTLINK* newlink;

      newlink = new TRIANGLELISTLINK(object,NULL);

      if (!newlink)
      {
         return FALSE;
      }

      if (count==0)
      {
         first = newlink;
         current = newlink;
         last = newlink;
         last->setNext(NULL);
      }
      else
      {
         last->setNext(newlink);
         last = newlink;
      }

      count++;
      return TRUE;

   }


/*=============================================================================================*/
/*  ostream << POLYGON                                                                (DEBUG)  */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  This is provided as a debugging aid.  Sometimes it is useful to print out the contents     */
/*  of a polygon (ie. its number of vertices and the actual vertices).                         */
/*                                                                                             */
/*=============================================================================================*/

ostream& operator << (ostream& s, const POLYGON& p) {
    /*
    if (p.numpoints==0)
        s<<"EMPTY POLYGON\n";
    else
    {
       s<<"POLYGON with "<<p.numpoints<<" sides:\n";
       for (int i=0;i<p.numpoints;i++)
          s<<"   "<<p.pointlist[i]<<"\n";
    }
    */
    return s;
}
 

/*=============================================================================================*/
/*  POLYGON::POLYGON()                                                                (PUBLIC) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  These are the constructors for the POLYGON class.  The first creates a POLYGON with no     */
/*  vertices, the second creates a POLYGON given an array of n points, and the third is a      */
/*  copy constructor.                                                                          */
/*                                                                                             */
/*=============================================================================================*/

   POLYGON::POLYGON(void)
   {
      numpoints   = 0;              
      pointlist   = NULL;
      orientation = CLOCKWISE;
   }
                                  
   POLYGON::POLYGON(INT n, VECTOR* p)
   {
      numpoints   = n;
      pointlist   = p;
      orientation = findOrientation();
   }

   POLYGON::POLYGON(const POLYGON& P)
   {
      numpoints   = P.numpoints;
      pointlist   = new VECTOR[numpoints];
      for (int i=0;i<numpoints;i++)
         pointlist[i]=P.pointlist[i];
      orientation = P.orientation;
   }


/*=============================================================================================*/
/*  POLYGON::findOrientation()                                                       (PRIVATE) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  This function calculates the orientation of a POLYGON.                                     */
/*                                                                                             */
/*=============================================================================================*/

   USHORT POLYGON::findOrientation(void)
   {
      DOUBLE area;
      INT    i;

      area =  pointlist[numpoints-1].x * pointlist[0].y
            - pointlist[0].x * pointlist[numpoints-1].y;

      for (i=0;i<numpoints-1;i++)
          area +=  pointlist[i].x * pointlist[i+1].y
                 - pointlist[i+1].x * pointlist[i].y;

      if (area >= 0.0) 
          return COUNTER_CLOCKWISE;
      else
          return CLOCKWISE;
   }


/*=============================================================================================*/
/*  POLYGON::findDeterminant()                                                       (PRIVATE) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  Finds the orientation of the triangle formed by connecting the POLYGON's p1, p2, and       */
/*  p3 vertices.                                                                               */
/*                                                                                             */
/*=============================================================================================*/

   USHORT POLYGON::findDeterminant(INT p1, INT p2, INT p3)
   {

// #define DEBUG_POLYGON_Determinant

      DOUBLE determinant;

      determinant = (pointlist[p2].x-pointlist[p1].x)
                   *(pointlist[p3].y-pointlist[p1].y)
                   -(pointlist[p3].x-pointlist[p1].x)
                   *(pointlist[p2].y-pointlist[p1].y);

      if (determinant > 0.0)
          return COUNTER_CLOCKWISE;
      else if (determinant==0.0)
      {
         if(   pointlist[p1]==pointlist[p2] 
            || pointlist[p1]==pointlist[p3] 
            || pointlist[p2]==pointlist[p3])
         {

            return CLOCKWISE;
         }
         else
         {
            return COUNTER_CLOCKWISE;
         }
      }
      else
          return CLOCKWISE;
   }


/*=============================================================================================*/
/*  POLYGON::noneInside()                                                            (PRIVATE) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  Returns 'FALSE' if any of the POLYGON's vertices in 'vlist' are inside the triangle formed */
/*  by connecting the vertices p1, p2, and p3.  'n' is the number of vertices in 'vlist'.      */
/*  Returns 'TRUE' if no vertices are inside that triangle.                                    */
/*                                                                                             */
/*=============================================================================================*/

   USHORT POLYGON::noneInside(INT p1, INT p2, INT p3, INT n, INT* vlist)
   {
      INT i,p;

      for(i=0;i<n;i++)
      {
         p=vlist[i];
         if((p==p1)||(p==p2)||(p==p3)) continue;
         if (   (findDeterminant(p2,p1,p)==orientation)
             || (findDeterminant(p1,p3,p)==orientation)
             || (findDeterminant(p3,p2,p)==orientation))  continue;
         else
         {
            if (  (pointlist[p].x==pointlist[p1].x && pointlist[p].y==pointlist[p1].y)
                ||(pointlist[p].x==pointlist[p2].x && pointlist[p].y==pointlist[p2].y)
                ||(pointlist[p].x==pointlist[p3].x && pointlist[p].y==pointlist[p3].y))
               continue;
            else
               return FALSE;
         }
      }
      return TRUE;
   }



/*=============================================================================================*/
/*  POLYGON::Correct()                                                                (PUBLIC) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  This routine just makes sure there are no two consecutive points in the polygon that are   */
/*  the same.  If there are the duplicates are deleted from the vertex list.                   */
/*                                                                                             */
/*=============================================================================================*/

   void POLYGON::Correct() 
   { 
      int i,j; 
      for (i=0;i<numpoints-1;i++) 
      { 
         if (pointlist[i]==pointlist[i+1]) 
         {
            for (j=i;j<numpoints-1;j++) 
               pointlist[j]=pointlist[j+1]; 
            numpoints--; i--; 
         }
      }
   }   



/*=============================================================================================*/
/*  POLYGON::Triangulate()                                                            (PUBLIC) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  Slices the POLYGON up into a list of triangles.  The POLYGON must be non-intersecting.     */
/*  This is done by checking each vertex to see whether or not it can be 'chopped off'.        */
/*  If so, that vertex is removed, and the process is repeated until there are only three      */
/*  vertices left (only one triangle left).  Returns the following values upon completion:     */
/*                                                                                             */
/*       ERR_NoPolyFound .......... If there were more than three vertices left, but none of   */
/*                                  them could be 'chopped off'.  This usually happens if the  */
/*                                  polygon intersects itself.                                 */
/*                                                                                             */
/*       ERR_NoError .............. If the polygon was successfully triangulated.              */
/*                                                                                             */
/*                                                                                             */
/*=============================================================================================*/

   INT POLYGON::Triangulate(TRIANGLELIST& trilist)
   {

// #define DEBUG_POLYGON_Triangulate

      TRIANGLE*         current_triangle;
      INT               previous;
      INT               current;
      INT               next;
      INT*              rvl;
      INT               vertex_count;
      USHORT            current_determinant;
      USHORT            current_position;
      INT               i;
      VECTOR            p1,p2,p3;
      USHORT            done;
      VECTOR            n1(0,0,1);
      VECTOR            n2(0,0,1);
      VECTOR            n3(0,0,1);

      rvl = new INT[numpoints];
      for (i=0;i<numpoints;i++) 
         rvl[i]=i;

      vertex_count=numpoints;
      while (vertex_count>3)
      {

         done=FALSE;
         previous=vertex_count-1;
         current=0;
         next=1;
         while (current<vertex_count && !done)
         {
            previous = current-1;
            next     = current+1;

            if (current==0)
               previous=vertex_count-1;
            else if (current==vertex_count-1)
               next=0;

            current_determinant = findDeterminant(rvl[previous],
                                                  rvl[current],
                                                  rvl[next]);
            current_position = noneInside(rvl[previous] ,
                                          rvl[current]  ,
                                          rvl[next]     ,
                                          vertex_count  ,
                                          rvl          );

            if (   (current_determinant==orientation)
                && (current_position==TRUE))
            {
               done=TRUE;
            }
            else
            {
               current++;
            }
         }

         if (!done) {
            return ERR_NoPolyFound;
         }

         p1=VECTOR(pointlist[rvl[previous]]);
         p2=VECTOR(pointlist[rvl[current]]);
         p3=VECTOR(pointlist[rvl[next]]);

         current_triangle = new TRIANGLE(p1,p2,p3,n1,n2,n3);
         trilist.Add(current_triangle);

         vertex_count-=1;
         for (i=current;i<vertex_count;i++) rvl[i]=rvl[i+1];

      }

      p1=VECTOR(pointlist[rvl[0]]);
      p2=VECTOR(pointlist[rvl[1]]);
      p3=VECTOR(pointlist[rvl[2]]);

      current_triangle = new TRIANGLE(p1,p2,p3,n1,n2,n3);
      trilist.Add(current_triangle);

      delete rvl;

      return ERR_NoError;

   }


/*=============================================================================================*/
/*  POLYGON::Combine()                                                                (PUBLIC) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  This function combines two polygons by cutting between them at their point of closest      */
/*  approach (PCA).  The resulting polygon is found by tracing around it's own vertices from   */
/*  the PCA all the way back around to and including the PCA.  Then, adding an edge to the     */
/*  PCA of the polygon we're combining, tracing around the polygon we're combining (in the     */
/*  opposite direction), and finally adding an edge from the inner PCA to the outer PCA.       */
/*                                                                                             */
/*=============================================================================================*/

   void POLYGON::Combine(POLYGON& p)
   {
      register INT      i,ni,j;
      DOUBLE   current_dist;
      DOUBLE   min_dist;
      INT      min_i=0;
      INT      min_j=0;
      VECTOR   currToPrev, currToNext, minToPrev, minToNext;
      VECTOR   testVector;
      DOUBLE   distCP,distCN,distMP,distMN;
      VECTOR*  newpl;

      newpl = new VECTOR[numpoints+p.numpoints+2];


      min_dist = BIG;

      f_jauge(1,AFFICHE,0,0,"Computing geometry");

      for (i=0;i<numpoints;i++) {
         f_jauge(1,MODIF,i,numpoints,NULL);
         for (j=0;j<p.numpoints;j++) {
            current_dist = dist(pointlist[i],p.pointlist[j]);
            if (current_dist==min_dist) {
               if (i>0) currToPrev = pointlist[i-1]-pointlist[i];
               else currToPrev = pointlist[numpoints-1]-pointlist[i];
               if (i<numpoints-1) currToNext = pointlist[i+1]-pointlist[i];
               else currToNext = pointlist[0]-pointlist[i];

               if (min_i>0) minToPrev = pointlist[min_i-1]-pointlist[min_i];
               else minToPrev = pointlist[numpoints-1]-pointlist[min_i];
               if (min_i<numpoints-1) minToNext = pointlist[min_i+1]-pointlist[min_i];
               else minToNext = pointlist[0]-pointlist[min_i];

               testVector = p.pointlist[j]-pointlist[i];
 
               distCP = dist(~currToPrev,testVector); // Changed from being normalized...
               distCN = dist(~currToNext,testVector);
               distMP = dist(~minToPrev ,testVector);
               distMN = dist(~minToNext ,testVector);

               if (  (distCP+distCN)<(distMP+distMN)) {
                  min_dist=current_dist;
                  min_i=i;
                  min_j=j;
               }
            } else if (current_dist<min_dist) {
               min_dist = current_dist;
               min_i    = i;
               min_j    = j;
            }
         }
      }

      f_jauge(1,EFFACE,0,0,NULL);

      ni=0;

      for(i=0; i<=min_i;i++) {
         newpl[ni]=pointlist[i];
         ni++; 
      }

      for(i=min_j ; i<p.numpoints ;i++) {
         newpl[ni]=p.pointlist[i];
         ni++;
      }

      for(i=0     ; i<=min_j      ;i++) {
         newpl[ni]=p.pointlist[i];
         ni++; 
      }

      for(i=min_i ; i<numpoints   ;i++) {
         newpl[ni]=pointlist[i];
         ni++;
      }

      numpoints = ni;
      delete pointlist;
      pointlist = newpl;

   }


/*=============================================================================================*/
/*  POLYGON::isInside                                                                 (PUBLIC) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  This function determines whether or not a polygon (the one it is called upon) is entirely  */
/*  inside another polygon (the one passed as a parameter: 'p').  If it is, the function ret-  */
/*  urns TRUE, FALSE otherwise.                                                                */
/*                                                                                             */
/*=============================================================================================*/

   int POLYGON::isInside(POLYGON& p)
   {
      LONG   i,j,c=0;
      DOUBLE x,y;

      x = pointlist[0].x;
      y = pointlist[0].y;

      for (i=0, j=p.numpoints-1; i<p.numpoints; j=i++)
      {
         if ((((p.pointlist[i].y<=y) && (y<p.pointlist[j].y)) ||
              ((p.pointlist[j].y<=y) && (y<p.pointlist[i].y))) &&
             (x < (p.pointlist[j].x - p.pointlist[i].x) * (y - p.pointlist[i].y) /
             (p.pointlist[j].y - p.pointlist[i].y) + p.pointlist[i].x))

            c = !c;
      }
      return c;
   }


//=============================================================================================*/
//  POLYGON::Shrink                                                                   (PUBLIC) */
//---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  This function shifts each vertex of a polygon inward a small amount, along a direction     */
/*  vector that bisects the angle created by the vertex's two adjacent edges.  The amount      */
/*  of movement is specified by 'shrinkFactor'; if it is positive the movement is inward, if   */
/*  it is negative the movement is toward the outside of the polygon.  The resulting 'shrunk'  */
/*  polygon is stored in 'newPoly'.  If this polygon is not already empty (if it has any       */
/*  vertices at all), then its vertices are deleted before creating the new polygon.           */
/*                                                                                             */
/*  NOTE:  The algorithm used here is not foolproof.  If relatively large shrinkFactors are    */
/*         given, the resulting 'shrunk' polygon can become self-intersecting.                 */
/*                                                                                             */
/*         Also, two versions are provided; one that creates a new polygon, one that modifies  */
/*         the points of the old polygon.                                                      */
/*                                                                                             */
/*=============================================================================================*/

/*---------------------------------------------------------------------------------------------*/
/*  VERSION 1:  Does not modify the original polygon.                                          */
/*---------------------------------------------------------------------------------------------*/

   void POLYGON::Shrink(POLYGON& newPoly, DOUBLE shrinkFactor)
   {
  
      INT i;
      VECTOR current,previous,next;
      VECTOR toPrevious,toNext,inPrevious,inNext;
      VECTOR inward;
      DOUBLE angle;
      DOUBLE shrinkDist;
      VECTOR zDir(0,0,1);
      DOUBLE bisectorLength;

      if (shrinkFactor==0) return;

      if (newPoly.numpoints>0)
      {
         delete newPoly.pointlist;
         newPoly.numpoints = 0;
      }
 
      newPoly.pointlist = new VECTOR[numpoints];
      newPoly.numpoints = numpoints;

      previous = pointlist[numpoints-1];
      current = pointlist[0];
      next = pointlist[1];

      toPrevious = ~(previous-current);
      toNext     = ~(next-current);

      inPrevious = zDir^toPrevious;
      inNext     = toNext^zDir;

      bisectorLength = toPrevious%toNext;
      if (bisectorLength < -1.0) 
         bisectorLength = -1.0;
      else if (bisectorLength > 1.0)
         bisectorLength = 1.0;

      angle  = 0.5*acos(bisectorLength);
      if (angle<MIN_SHRINK_ANGLE) angle=MIN_SHRINK_ANGLE;
      shrinkDist = shrinkFactor/sin(angle);
      inward = ~(inPrevious+inNext)*shrinkDist;

      newPoly.pointlist[0]=current+inward;

      previous = pointlist[numpoints-2];
      current = pointlist[numpoints-1];
      next = pointlist[0];

      toPrevious = ~(previous-current);
      toNext     = ~(next-current);

      inPrevious = zDir^toPrevious;
      inNext     = toNext^zDir;

      bisectorLength = toPrevious%toNext;
      if (bisectorLength < -1.0) 
         bisectorLength = -1.0;
      else if (bisectorLength > 1.0)
         bisectorLength = 1.0;

      angle  = 0.5*acos(bisectorLength);
      if (angle<MIN_SHRINK_ANGLE) angle=MIN_SHRINK_ANGLE;
      shrinkDist = shrinkFactor/sin(angle);
      inward = ~(inPrevious+inNext)*shrinkDist;

      newPoly.pointlist[numpoints-1]=current+inward;

 
      for (i=1;i<numpoints-1;i++)
      {
         previous = pointlist[i-1];
         current = pointlist[i];
         next = pointlist[i+1];

         toPrevious = ~(previous-current);
         toNext     = ~(next-current);

         inPrevious = zDir^toPrevious;
         inNext     = toNext^zDir;

         bisectorLength = toPrevious%toNext;
         if (bisectorLength < -1.0) 
            bisectorLength = -1.0;
         else if (bisectorLength > 1.0)
            bisectorLength = 1.0;

         angle  = 0.5*acos(bisectorLength);
         if (angle<MIN_SHRINK_ANGLE) angle=MIN_SHRINK_ANGLE;
         shrinkDist = shrinkFactor/sin(angle);
         inward = ~(inPrevious+inNext)*shrinkDist;

         newPoly.pointlist[i]=current+inward;
      }

   }


/*---------------------------------------------------------------------------------------------*/
/*  VERSION 2:  Modifies the original polygon's points                                         */
/*---------------------------------------------------------------------------------------------*/

   void POLYGON::Shrink(DOUBLE shrinkFactor)
   {
      INT i;
      VECTOR current,previous,next;
      VECTOR toPrevious,toNext,inPrevious,inNext;
      VECTOR inward;
      DOUBLE angle;
      DOUBLE shrinkDist;
      VECTOR zDir(0,0,1);
      VECTOR* newpointlist;
      DOUBLE bisectorLength;

      if (shrinkFactor==0) return;

      newpointlist = new VECTOR[numpoints];

      previous = pointlist[numpoints-1];
      current = pointlist[0];
      next = pointlist[1];

      toPrevious = ~(previous-current);
      toNext     = ~(next-current);

      inPrevious = zDir^toPrevious;
      inNext     = toNext^zDir;

      bisectorLength = toPrevious%toNext;
      if (bisectorLength < -1.0) 
         bisectorLength = -1.0;
      else if (bisectorLength > 1.0)
         bisectorLength = 1.0;

      angle  = 0.5*acos(bisectorLength);
      if (angle<MIN_SHRINK_ANGLE) angle=MIN_SHRINK_ANGLE;
      shrinkDist = shrinkFactor/sin(angle);
      inward = ~(inPrevious+inNext)*shrinkDist;

      newpointlist[0]=current+inward;

      previous = pointlist[numpoints-2];
      current = pointlist[numpoints-1];
      next = pointlist[0];

      toPrevious = ~(previous-current);
      toNext     = ~(next-current);

      inPrevious = zDir^toPrevious;
      inNext     = toNext^zDir;

      bisectorLength = toPrevious%toNext;
      if (bisectorLength < -1.0) 
         bisectorLength = -1.0;
      else if (bisectorLength > 1.0)
         bisectorLength = 1.0;

      angle  = 0.5*acos(bisectorLength);
      if (angle<MIN_SHRINK_ANGLE) angle=MIN_SHRINK_ANGLE;
      shrinkDist = shrinkFactor/sin(angle);
      inward = ~(inPrevious+inNext)*shrinkDist;

      newpointlist[numpoints-1]=current+inward;

 
      for (i=1;i<numpoints-1;i++)
      {
         previous = pointlist[i-1];
         current = pointlist[i];
         next = pointlist[i+1];

         toPrevious = ~(previous-current);
         toNext     = ~(next-current);

         inPrevious = zDir^toPrevious;
         inNext     = toNext^zDir;

         bisectorLength = toPrevious%toNext;
         if (bisectorLength < -1.0) 
            bisectorLength = -1.0;
         else if (bisectorLength > 1.0)
            bisectorLength = 1.0;

         angle  = 0.5*acos(bisectorLength);
         if (angle<MIN_SHRINK_ANGLE) angle=MIN_SHRINK_ANGLE;
         shrinkDist = shrinkFactor/sin(angle);
         inward = ~(inPrevious+inNext)*shrinkDist;

         newpointlist[i]=current+inward;
      }

      delete pointlist;
      pointlist = newpointlist;

   }




/*=============================================================================================*/
/*  POLYGON::SetDepth()                                                               (PUBLIC) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  This function simply sets the z-coordinate of each of the vertices in the polygon to a     */
/*  specified value 'depth'.                                                                   */
/*                                                                                             */
/*=============================================================================================*/

   void POLYGON::SetDepth(DOUBLE depth)
   {
      for (INT i=0;i<numpoints;i++)
         pointlist[i].z = depth;
   }


/*=============================================================================================*/
/*  POLYGON::SetDepth()                                                               (PUBLIC) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  This function translates each point in the polygon by adding 'offset' to each of its       */
/*  points.                                                                                    */
/*                                                                                             */
/*=============================================================================================*/

   void POLYGON::Translate(VECTOR offset)
   {
      for (INT i=0;i<numpoints;i++)
         pointlist[i] = pointlist[i]+offset;
   }


/*=============================================================================================*/
/*  ApproximateQuadraticSpline()                                                      (PUBLIC) */
/*---------------------------------------------------------------------------------------------*/
/*                                                                                             */
/*  This function evaluates a quadratic B-spline curve, specified by its three control points  */
/*  (cp1,cp2,cp3), at an arbitrary point along that curve.  The return value is only meaning-  */
/*  ful if the parameter 't' is between 0 and 1, inclusively.                                  */
/*                                                                                             */
/*=============================================================================================*/

   VECTOR ApproximateQuadraticSpline(VECTOR cp1, VECTOR cp2, VECTOR cp3, DOUBLE t)
   {
      DOUBLE i1 = (1-t)*(1-t);
      DOUBLE i2 = 2*t*(1-t);
      DOUBLE i3 = t*t;

      DOUBLE tx = i1*cp1.x + i2*cp2.x + i3*cp3.x;
      DOUBLE ty = i1*cp1.y + i2*cp2.y + i3*cp3.y;
      DOUBLE tz = cp1.z;

      return VECTOR(tx,ty,tz);
   }
