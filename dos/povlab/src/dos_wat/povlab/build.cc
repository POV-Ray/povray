/* ---------------------------------------------------------------------------
*  BUILD.CC
*
*  Copyright (c) 1994, 1995 by Todd A. Prater
*
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

#include <stdlib.h>
#include <fstream.h>

#include "config.h"
#include "vector.h"
#include "truetype.h"
#include "geometry.h"
#include "font3d.h"


/*==============================================================================================*/
/*  PolygonizeContour()                                                                         */
/*==============================================================================================*/
/*                                                                                              */
/*  This function creates a polygon that approximates one of the contours in a glyph.  The      */
/*  accuracy of this approximation depends on the value 'resolution', which specifies the       */
/*  number of straight line segments  used to render a curved section of the contour.           */
/*                                                                                              */
/*    PARAMETER    DESCRIPTION                                                                  */
/*  ------------------------------------------------------------------------------------------  */
/*    font         The TrueType font to use.                                                    */
/*    glyphnum     The index of a glyph in 'font'.                                              */
/*    contournum   The index of the contour to approximate.                                     */
/*    resolution   The number of straight line segments to use when approximating curved        */
/*                 sections of the contour.                                                     */
/*    depth        The Z-depth of the generated polygon.  Each vertex will have this value as   */
/*                 its Z-component.                                                             */
/*    polygon      The polygon used to store the approximation.  If 'contournum' is an invalid  */
/*                 index, the function returns without modifying 'polygon'.  Otherwise, upon    */
/*                 completion of this routine, 'polygon' contains a list of vertices that       */
/*                 approximate the contour.  If 'polygon' was not empty before this function    */
/*                 was called, it's contents will be deleted.                                   */
/*                                                                                              */
/*==============================================================================================*/

void PolygonizeContour(TTFont&   font,
                       USHORT    glyphnum,
                       USHORT    contournum,
                       USHORT    resolution,
                       DOUBLE    depth,
                       POLYGON&  polygon    )
{

// #define DEBUG_BUILD_PolygonizeContour

   USHORT p1type,p2type,p3type;
   VECTOR p1,p2,p3;
   USHORT fontpointcount = font.NumPoints(glyphnum,contournum);
   USHORT polypointcount = 0;
   ULONG  polyi = 0;
   USHORT i;
   LONG   c;
   DOUBLE t;
   DOUBLE upem = font.UnitsPerEm();

  /*____ No need to do anything, unless this glyph actually has      ____*/
  /*____ some contours, and the contour index specified is valid.    ____*/

   if (font.NumContours(glyphnum)<1) return;
   if (contournum>=font.NumContours(glyphnum)) return;

  /*____ First, we need to find out how many points will eventually  ____*/
  /*____ be generated, so we can allocate the polygon.  We use the   ____*/
  /*____ the same loop as below, but only count the number of points ____*/
  /*____ this time (not generate them).                              ____*/

   for (i=1;i<fontpointcount;i++)
   {
      p1type = font.FontPointType(glyphnum,contournum,i-1);
      p2type = font.FontPointType(glyphnum,contournum,i);
      if (i==fontpointcount-1)
         p3type = font.FontPointType(glyphnum,contournum,0);
      else
         p3type = font.FontPointType(glyphnum,contournum,i+1);

      if (p1type==ON_CURVE)
      {
         if (p2type==ON_CURVE)
         {
            polypointcount++;
            if (i==fontpointcount-1)
	    {
               polypointcount++;
	    }
         }
         else
         {
            if (p3type==ON_CURVE)
            {
               polypointcount+=resolution;
               if (i==fontpointcount-2) 
	       {
                  polypointcount++;
	       }
               i++;
            }
            else
            {
               polypointcount+=resolution;
            }
         }
      }
      else
      {
         if (p2type==ON_CURVE)
         {
         }
         else
         {
            if (p3type==ON_CURVE)
            {
               polypointcount+=resolution;
               if (i==fontpointcount-2) 
	       {
                  polypointcount++;
	       }
               i++;
            }
            else
            {
               polypointcount+=resolution;
            }
         }
      }
   } /* END For */

  /*____ The total number of points that will be generated is now    ____*/
  /*____ in 'pointcount', so we can delete (if necesarry) the old    ____*/
  /*____ polygon and allocate the new one.                           ____*/

   polygon.numpoints = polypointcount;
   if (polygon.pointlist!=NULL) delete polygon.pointlist;
   polygon.pointlist = new VECTOR[polypointcount];


  /*____ We're ready to generate the polygon vertices, using the     ____*/
  /*____ the following rules:                                        ____*/
  /*____                                                             ____*/
  /*____    1.) Two consecutive ON_CURVE points describe a           ____*/
  /*____        straight line,                                       ____*/
  /*____    2.) An ON_CURVE point, followed by an OFF_CURVE point,   ____*/
  /*____        followed by another ON_CURVE point, describes a      ____*/
  /*____        curved segment.  We approximate this curve as a      ____*/
  /*____        sequence of 'resolution' line segments.              ____*/
  /*____    3.) Two consecutive OFF_CURVE points have an implied     ____*/
  /*____        ON_CURVE point at the midpoint of the line connect-  ____*/
  /*____        ing them.                                            ____*/


   for (i=1;i<fontpointcount;i++)
   {

      p1type = font.FontPointType(glyphnum,contournum,i-1);
      p1 = VECTOR((DOUBLE)font.FontPointX(glyphnum,contournum,i-1),
                  (DOUBLE)font.FontPointY(glyphnum,contournum,i-1),
                  depth);

      p2type = font.FontPointType(glyphnum,contournum,i);
      p2 = VECTOR((DOUBLE)font.FontPointX(glyphnum,contournum,i),
                  (DOUBLE)font.FontPointY(glyphnum,contournum,i),
                  depth);

      if (i==fontpointcount-1)
      {
         p3type = font.FontPointType(glyphnum,contournum,0);
         p3 = VECTOR((DOUBLE)font.FontPointX(glyphnum,contournum,0),
                     (DOUBLE)font.FontPointY(glyphnum,contournum,0),
                     depth);
      }
      else
      {
         p3type = font.FontPointType(glyphnum,contournum,i+1);
         p3 = VECTOR((DOUBLE)font.FontPointX(glyphnum,contournum,i+1),
                     (DOUBLE)font.FontPointY(glyphnum,contournum,i+1),
                     depth);
      }

      p1=p1/upem; p2=p2/upem; p3=p3/upem;

      if (p1type==ON_CURVE)
      {
         if (p2type==ON_CURVE)
         {
            polygon.pointlist[polyi++] = p1;
            if (i==fontpointcount-1)
               polygon.pointlist[polyi++] = p2;

         }
         else 
         {
            if (p3type==ON_CURVE)
            {
               for (c=0;c<resolution;c++)
               {
                  t=(DOUBLE)c/resolution;
                  polygon.pointlist[polyi++] =
                     ApproximateQuadraticSpline(p1,p2,p3,t);
               }
               if (i==fontpointcount-2) 
                  polygon.pointlist[polyi++] = p3;
               i++;
            }
            else
            {
               for (c=0;c<resolution;c++)
               {
                  t=(DOUBLE)c/resolution;
                  polygon.pointlist[polyi++] =
                     ApproximateQuadraticSpline(p1,p2,midpoint(p2,p3),t);
               }
            }
         }
      }
      else
      {
         if (p2type==ON_CURVE)
         {
           // cout<<"This should never happen..."<<endl;
         }
         else
         {
            if (p3type==ON_CURVE)
            {
               for (c=0;c<resolution;c++)
               {
                  t=(DOUBLE)c/resolution;
                  polygon.pointlist[polyi++] =
                     ApproximateQuadraticSpline(midpoint(p1,p2),p2,p3,t);
               }
               if (i==fontpointcount-2)
                  polygon.pointlist[polyi++] = p3;

               i++;
            }
            else
            {
               for (c=0;c<resolution;c++)
               {
                  t=(DOUBLE)c/resolution;
                  polygon.pointlist[polyi++] =
                     ApproximateQuadraticSpline(midpoint(p1,p2),p2,midpoint(p2,p3),t);
               }
            }
         }
      }
   } /* END For */

  /*____ Finally, determine the orientation of the polygon we     ____*/
  /*____ just made...                                             ____*/


   if (polygon.pointlist[0] == polygon.pointlist[polygon.numpoints-1])
      polygon.numpoints--;

   polygon.Correct();

   polygon.orientation = polygon.findOrientation();

}



/*==============================================================================================*/
/*  CreateFaces()                                                                               */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       USHORT CreateFaces(TTFont& font, USHORT glyphnum,                             */
/*                                   Font3DOptions& options, VECTOR offset,                     */
/*                                   TRIANGLELIST& frontTriangleList,                           */
/*                                   TRIANGLELIST& backTriangleList);                           */
/*                                                                                              */
/*  DESCRIPTION:  This function generates the triangles that make up the front and back faces   */
/*                of a glyph, adding them to the TRIANGLELISTs supplied.  Each triangle is      */
/*                translated a distance specified by 'offset', and any other options            */
/*                needed are provided in 'options'.                                             */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function:                     */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_OutOfMemory...........An attempt to allocate memory failed.            */
/*                                                                                              */
/*==============================================================================================*/


// #define DEBUG_BUILD_CreateFaces

   USHORT CreateFaces(TTFont& font,USHORT glyphnum, Font3DOptions& options,
                      VECTOR offset, TRIANGLELIST& frontTriangleList,
                      TRIANGLELIST& backTriangleList)
   {
      int       i,j,k;                       /* Counters                                        */
      int       outermostContour;
      USHORT    isAPair;                     /* Boolean flag                                    */
      SHORT     contourCount;                /* Number of contours in this glyph.               */
      POLYGON*  frontPolyArray=NULL;         /* One polygon for each contour in the glyph.      */
      POLYGON*  backPolyArray =NULL;         /*  "     "     "    "                             */
      POLYGON*  testPolyArray;               /* Poly to use for testing purposes                */
      BYTEPTR*  relationship;                /* Relationship matrix                             */
      DOUBLE    frontZPos;                   /* Z-depth of the front face                       */
      DOUBLE    backZPos;                    /* Z-depth of the back face                        */

      if (   !options.frontFaceVisible                   /* Make sure at least one face is      */
          && !options.backFaceVisible)                   /*  visible                            */
         return ERR_NoError;

      contourCount = font.NumContours(glyphnum);

      if (options.zPosition==BACK)                       /* Figure out the z-depth of the front */
      {                                                  /*  and back faces.                    */
         frontZPos = options.depth;
         backZPos  = 0.0;
      }
      else if (options.zPosition==CENTER)
      {
         frontZPos = options.depth/2;
         backZPos  = -options.depth/2;
      }
      else
      {
         frontZPos = 0.0;
         backZPos  = -options.depth;
      }

                                                         /* Make sure the glyph has at least    */
      if (contourCount<1) return ERR_NoError;            /*  one contour to triangulate         */

      if (options.frontFaceVisible)                      /* Allocate mem for the front face     */
      {                                                  /*  outlines if necessary              */
         frontPolyArray = new POLYGON[contourCount];
         if (frontPolyArray==NULL)
            return ERR_OutOfMemory;
      }

      if (options.backFaceVisible)                       /* Allocate mem for the back face      */
      {                                                  /*  outlines if necessary              */
         backPolyArray = new POLYGON[contourCount];      
         if (backPolyArray==NULL)                        /* If out of memory, clean up and      */
         {                                               /*  return                             */
            if (frontPolyArray)
               delete frontPolyArray;
            return ERR_OutOfMemory;
         }
      }

      relationship = new BYTEPTR[contourCount];          /* Allocate mem for the relationship   */
      if (relationship==NULL)                            /*  matrix                             */
      {
         if (frontPolyArray)                             /* If out of memory, clean up and      */
            delete frontPolyArray;                       /*  return                             */
         if (backPolyArray)
            delete backPolyArray;
         return ERR_OutOfMemory;
      }

      for (i=0;i<contourCount;i++) {
         relationship[i] = new BYTE[contourCount];
         if (relationship[i]==NULL)
         {
            if (frontPolyArray)
               delete frontPolyArray;
            if (backPolyArray)
               delete frontPolyArray;
            for (j=0;j<i;j++)
               delete relationship[j];
            delete relationship;
            return ERR_OutOfMemory;
         }
      }

#ifdef DEBUG_BUILD_CreateFaces
cout<<endl;
#endif


      if (options.frontFaceVisible)                      /* Since the relationships are the     */
      {
         testPolyArray = frontPolyArray;                 /*  same for both the front and back   */
      }
      else                                               /*  faces, get a pointer to the one we */
      {
         testPolyArray = backPolyArray;                  /*  will use for testing               */
      }


      for (i=0;i<contourCount;i++)                                /* Polygonize each contour in */
      {                                                           /*  the glyph.                */


         if (options.frontFaceVisible)                            /* Do the front face?         */
         {
            PolygonizeContour(font,glyphnum,i,
                              options.resolution,
                              frontZPos,frontPolyArray[i]);


            frontPolyArray[i].Shrink(options.frontFaceCut);       /*  shrink the front face     */


         }
         if (options.backFaceVisible)                             /* Do the back face?          */
         {
            PolygonizeContour(font,glyphnum,i,
                              options.resolution,
                              backZPos,backPolyArray[i]);


            backPolyArray[i].Shrink(options.backFaceCut);      /*  shrink the back face      */

         }


      }


      for (i=0;i<contourCount;i++)                                /* Determine the relationship */
         for (j=0;j<contourCount;j++)                             /*  (INSIDE or OUTSIDE) bet-  */
            if (i==j)                                             /*  ween a contour and every  */
               relationship[i][j]=IGNORE;                         /*  other one in the glyph    */
            else if (testPolyArray[j].isInside(testPolyArray[i])) 
               relationship[i][j]=INSIDE;
            else
               relationship[i][j]=OUTSIDE;

     /*____ This loop combines any counter-clockwise contours with the closest one that is  ____*/
     /*____ both clockwise and outside itself.  Upon completion, the clockwise polygons are ____*/
     /*____ the ones that need triangulating.                                               ____*/  

      for (i=0;i<contourCount;i++)
      {
         if (testPolyArray[i].orientation==CLOCKWISE)
         {
            for (j=0;j<contourCount;j++)
            {
               isAPair=FALSE;
               if (   relationship[i][j]==INSIDE
                   && testPolyArray[j].orientation==COUNTER_CLOCKWISE)
               {
                  isAPair=TRUE;
                  for (k=0;k<contourCount;k++)
                  {
                     if (   k==j || k==i
                         || relationship[k][j]==IGNORE )
		     {
                        continue;
		     }

                     if (relationship[k][j]==INSIDE)
                     {
                        if (relationship[i][k]==INSIDE)
                        {
                           isAPair=FALSE;
                           break;
                        }
                     }
                     else
		     {
                        continue;
		     }
                  }
               }
               if (isAPair)
               {
                  if (options.frontFaceVisible)                                               
                     frontPolyArray[i].Combine(frontPolyArray[j]);
                  if (options.backFaceVisible)
                     backPolyArray[i].Combine(backPolyArray[j]);
                  relationship[i][j]=COMBINED;
               }
            }
         }
      }


      for (i=0;i<contourCount;i++)                                /* Triangulate the clockwise  */
      {                                                           /*  polygons that have been   */
         if (options.frontFaceVisible)                            /*  previously combined       */
            if (frontPolyArray[i].orientation==CLOCKWISE)
            {
               frontPolyArray[i].Translate(offset);
               frontPolyArray[i].SetDepth(frontZPos);
               frontPolyArray[i].Triangulate(frontTriangleList);
            }



            else
            {
               outermostContour=TRUE;
               for (j=0;j<contourCount;j++)
	       {
                  if (relationship[j][i]==INSIDE ||
                      relationship[j][i]==COMBINED )
		  {
                     outermostContour=FALSE;
                     break;
		  }
	       }
               if (outermostContour)
	       {
                  frontPolyArray[i].Translate(offset);
                  frontPolyArray[i].SetDepth(frontZPos);
                  frontPolyArray[i].Triangulate(frontTriangleList);
	       }
	    }



         if (options.backFaceVisible)
            if (backPolyArray[i].orientation==CLOCKWISE)
            {
               backPolyArray[i].Translate(offset);
               backPolyArray[i].SetDepth(backZPos);
               backPolyArray[i].Triangulate(backTriangleList);
            }




            else
	    {
               outermostContour=TRUE;
               for (j=0;j<contourCount;j++)
	       {
                  if (relationship[j][i]==INSIDE ||
                      relationship[j][i]==COMBINED )
		  {
                     outermostContour=FALSE;
                     break;
		  }
	       }
               if (outermostContour)
	       {
                  backPolyArray[i].Translate(offset);
                  backPolyArray[i].SetDepth(backZPos);
                  backPolyArray[i].Triangulate(backTriangleList);
	       }
	    }


      }

      for (i=0;i<contourCount;i++)                                /* Clean up                   */
      {
         if (options.frontFaceVisible)
            delete frontPolyArray[i].pointlist;
         if (options.backFaceVisible)
            delete backPolyArray[i].pointlist;
         delete relationship[i];
      }
      if (options.frontFaceVisible)
         delete frontPolyArray;
      if (options.backFaceVisible)
         delete backPolyArray;
      delete relationship;


      return ERR_NoError;

   }







/*==============================================================================================*/
/*  CreateSides()                                                                               */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       USHORT CreateSides(TTFont& font, USHORT glyphnum,                             */
/*                                   Font3DOptions& options, VECTOR offset,                     */
/*                                   TRIANGLELIST& frontBevelTriangleList,                      */
/*                                   TRIANGLELIST& backBevelTriangleList,                       */
/*                                   TRIANGLELIST& sideTriangleList);                           */
/*                                                                                              */
/*  DESCRIPTION:  This function generates the triangles that make up the bevels and sides of    */
/*                a glyph, adding them to the TRIANGLELISTs supplied.  Each triangle is         */
/*                translated a distance specified by 'offset', and any other options            */
/*                needed are provided in 'options'.                                             */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function:                     */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_OutOfMemory...........An attempt to allocate memory failed.            */
/*                                                                                              */
/*  Need more comments!                                                                         */
/*                                                                                              */
/*==============================================================================================*/

   USHORT CreateSides(TTFont& font, USHORT glyphnum, Font3DOptions& options,
                      VECTOR offset, TRIANGLELIST& frontBevelTriangleList,
                      TRIANGLELIST& backBevelTriangleList, TRIANGLELIST& sideTriangleList)
   {

      USHORT      i,j;                       /* Counters                                        */

      TRIANGLE*   t1;                        /* Temporary triangle pointers                     */
      TRIANGLE*   t2;
      TRIANGLE*   t3;
      TRIANGLE*   t4;
      TRIANGLE*   t5;
      TRIANGLE*   t6;

      VECTOR      previous;                  /* Points on a particular contour                  */
      VECTOR      current1;
      VECTOR      current2;
      VECTOR      next;

      VECTOR      previousFacet;             /* Facets along a contour                          */
      VECTOR      currentFacet;
      VECTOR      nextFacet;

      VECTOR      frontShrunkCurrent1;
      VECTOR      frontShrunkCurrent2;
      VECTOR      backShrunkCurrent1;
      VECTOR      backShrunkCurrent2;
      VECTOR      previousFacetNormal;
      VECTOR      currentFacetNormal;
      VECTOR      nextFacetNormal;
      VECTOR      averageNormal1;
      VECTOR      averageNormal2;
      VECTOR      frontBevelNormal1;
      VECTOR      frontBevelNormal2;
      VECTOR      backBevelNormal1;
      VECTOR      backBevelNormal2;
      VECTOR      p1,p2,p3,p4,p5,p6,p7,p8;
      ULONG       numberOfPoints;
      SHORT       contourCount;
      POLYGON*    polyArray;
      POLYGON*    frontShrunkPolyArray;
      POLYGON*    backShrunkPolyArray;
      VECTOR      zDir(0,0,1);
      DOUBLE      angle1,angle2;
      DOUBLE      bisectorLength1, bisectorLength2;
      USHORT      doFrontBevels;
      USHORT      doBackBevels;

      DOUBLE      zpos1,zpos2,zpos3,zpos4;


      contourCount = font.NumContours(glyphnum);
      if (contourCount<1) return ERR_NoError;

      if (options.zPosition==BACK)                          /* If the glyph is positioned so   */
      {                                                     /*  that it's back face is flush   */
         zpos1 = options.depth;                             /*  with the z=0 plane...          */ 
         if (options.frontBevelVisible)                     
            zpos2 = options.depth - options.frontSideCut;
         else
            zpos2 = options.depth;
         if (options.backBevelVisible)
            zpos3 = options.backSideCut;
         else
            zpos3 = 0.0;
         zpos4 = 0.0;
      }
      else if (options.zPosition==CENTER)                   /* If the glyph is centered on the */
      {                                                     /*  z=0 plane...                   */
         zpos1 = options.depth/2;
         if (options.frontBevelVisible)
            zpos2 = options.depth/2 - options.frontSideCut;
         else
            zpos2 = options.depth/2;
         if (options.backBevelVisible)
            zpos3 = -options.depth/2 + options.backSideCut;
         else
            zpos3 = -options.depth/2;
         zpos4 = -options.depth/2;
      }
      else                                                  /* Else the glyph must be posit-   */
      {                                                     /*  ioned so that it's front face  */
         zpos1 = 0.0;                                       /*  flush with the z=0 plane...    */
         if (options.frontBevelVisible)
            zpos2 = -options.frontSideCut;
         else
            zpos2 = 0.0;
         if (options.backBevelVisible)
            zpos3 = -options.depth + options.backSideCut;
         else
            zpos3 = -options.depth;
         zpos4 = -options.depth;
      }


      polyArray = new POLYGON[contourCount];                /* Allocate a new polygon for each */
      if (polyArray==NULL)                                  /*  contour in the glyph           */
      {
          /* clean up */
         return ERR_OutOfMemory;
      }


      if (options.frontFaceCut==0.0 && options.frontSideCut==0.0)
         doFrontBevels=FALSE;
      else
         doFrontBevels=TRUE;

      if (options.backFaceCut==0.0 && options.backSideCut==0.0)
         doBackBevels=FALSE;
      else
         doBackBevels=TRUE;



      if (doFrontBevels)                                   /* If we are making front bevels,   */
      {                                                    /*  then we also need an array for  */
         frontShrunkPolyArray = new POLYGON[contourCount]; /*  the shrunk polygons that make   */
         if (frontShrunkPolyArray==NULL)                   /*  up the inside edge of the       */
         {                                                 /*  front bevel.                    */
            /* clean up */
            return ERR_OutOfMemory;
         }
      }
      if (doBackBevels)                                    /* Same if we're doing back bevels  */
      {
         backShrunkPolyArray = new POLYGON[contourCount];
         if (backShrunkPolyArray==NULL)
         {
            /* clean up */
            return ERR_OutOfMemory;
         }
      }


     /*____ This loop reads the outlines in from the font class for each contour in the ____*/
     /*____ glyph, and then translates each one by the specified offset.                ____*/

      for (i=0;i<contourCount;i++)
      {
         PolygonizeContour(font,glyphnum,i,options.resolution,0.0,polyArray[i]);
         if (doFrontBevels)
         {
            polyArray[i].Shrink(frontShrunkPolyArray[i],options.frontFaceCut);
            frontShrunkPolyArray[i].Translate(offset);
            frontShrunkPolyArray[i].SetDepth(zpos1);
         }
         if (doBackBevels)
         {
            polyArray[i].Shrink(backShrunkPolyArray[i],options.backFaceCut);
            backShrunkPolyArray[i].Translate(offset);
            frontShrunkPolyArray[i].SetDepth(zpos4);
         }
         polyArray[i].Translate(offset);
      }


     /*____ Now we're going to generate the side triangles.  Each iteration through   ____*/
     /*____ the loop we need to keep track of up to four consecutive points along the ____*/
     /*____ defining outlines (e.g front, front bevel, back bevel, and back).  These  ____*/
     /*____ points define up to three consecutive facets along the side of the glyph. ____*/
     /*____ We only generate a pair of triangles for the middle facet (and a pair for ____*/
     /*____ each bevel if necessary), but we need the facets on either side to smooth ____*/
     /*____ normals.                                                                  ____*/

     /*____ I know this isn't the best way to do this, but hey, give me a break...:)  ____*/

      for (i=0;i<contourCount;i++)
      {
         numberOfPoints  = polyArray[i].numpoints;

         for (j=0;j<numberOfPoints;j++)
         {
	   /*____ Here's the messy part, this section just makes sure the facets wrap ____*/
	   /*____ around properly at the ends of the outlines. Please don't hold this ____*/
	   /*____ against me, please, please, please...                               ____*/

            if (j==numberOfPoints-2)
            {
               previous = polyArray[i].pointlist[j-1];
               current1 = polyArray[i].pointlist[j];
               if (doFrontBevels) frontShrunkCurrent1 = frontShrunkPolyArray[i].pointlist[j];
               if (doBackBevels) backShrunkCurrent1 = backShrunkPolyArray[i].pointlist[j];
               current2 = polyArray[i].pointlist[j+1];
               if (doFrontBevels) frontShrunkCurrent2 = frontShrunkPolyArray[i].pointlist[j+1];
               if (doBackBevels) backShrunkCurrent2 = backShrunkPolyArray[i].pointlist[j+1];
               next     = polyArray[i].pointlist[0];
            }
            else if (j==numberOfPoints-1)
            {
               previous = polyArray[i].pointlist[j-1];
               current1 = polyArray[i].pointlist[j];
               if (doFrontBevels) frontShrunkCurrent1 = frontShrunkPolyArray[i].pointlist[j];
               if (doBackBevels) backShrunkCurrent1 = backShrunkPolyArray[i].pointlist[j];
               current2 = polyArray[i].pointlist[0];
               if (doFrontBevels) frontShrunkCurrent2 = frontShrunkPolyArray[i].pointlist[0];
               if (doBackBevels) backShrunkCurrent2 = backShrunkPolyArray[i].pointlist[0];
               next     = polyArray[i].pointlist[1];
            }
            else if (j==0)
            {
               previous = polyArray[i].pointlist[numberOfPoints-1];
               current1 = polyArray[i].pointlist[j];
               if (doFrontBevels) frontShrunkCurrent1 = frontShrunkPolyArray[i].pointlist[j];
               if (doBackBevels) backShrunkCurrent1 = backShrunkPolyArray[i].pointlist[j];
               current2 = polyArray[i].pointlist[j+1];
               if (doFrontBevels) frontShrunkCurrent2 = frontShrunkPolyArray[i].pointlist[j+1];
               if (doBackBevels) backShrunkCurrent2 = backShrunkPolyArray[i].pointlist[j+1];
               next     = polyArray[i].pointlist[j+2];
            }
            else
            {
               previous = polyArray[i].pointlist[j-1];
               current1 = polyArray[i].pointlist[j];
               if (doFrontBevels) frontShrunkCurrent1 = frontShrunkPolyArray[i].pointlist[j];
               if (doBackBevels) backShrunkCurrent1 = backShrunkPolyArray[i].pointlist[j];
               current2 = polyArray[i].pointlist[j+1];
               if (doFrontBevels) frontShrunkCurrent2 = frontShrunkPolyArray[i].pointlist[j+1];
               if (doBackBevels) backShrunkCurrent2= backShrunkPolyArray[i].pointlist[j+1];
               next     = polyArray[i].pointlist[j+2];
            }

           /*____ Now that we have the outline points that make up the corners of     ____*/
           /*____ each facet, calculate a vector along the facet, tangent to the      ____*/
	   /*____ outline.                                                            ____*/

            previousFacet = ~(current1-previous);
            currentFacet  = ~(current2-current1);
            nextFacet     = ~(next-current2);

	   /*____ Now calculate the normal of each facet (not the bevel facets yet,   ____*/
           /*____ just the side facets.                                               ____*/

            previousFacetNormal = zDir^previousFacet;
            currentFacetNormal  = zDir^currentFacet;
            nextFacetNormal     = zDir^nextFacet;

	   /*____ Calculate the angle between the normal of the middle facet, and the ____*/
	   /*____ normal of the facet on either side.                                 ____*/

            bisectorLength1 = previousFacetNormal%currentFacetNormal;
            if (bisectorLength1 < -1.0)
		bisectorLength1 = -1.0;             /* Make sure acos gets a number       */ 
            else if (bisectorLength1 > 1.0)         /*   between -1 and 1.                */
               bisectorLength1 = 1.0;

            bisectorLength2 = currentFacetNormal%nextFacetNormal;
            if (bisectorLength2 < -1.0)
               bisectorLength2 = -1.0;              /* Make sure acos gets a number       */
            else if (bisectorLength2 > 1.0)         /*   between -1 and 1.                */
               bisectorLength2 = 1.0;
            
            angle1 = acos(bisectorLength1);
            angle2 = acos(bisectorLength2);

	   /*____ If the angle between adjacent facets is less than the threshold     ____*/
           /*____ angle, then smooth their normals by averaging them together.        ____*/

            if (angle1<options.threshold)
               averageNormal1 = ~(previousFacetNormal+currentFacetNormal);
            else
               averageNormal1 = currentFacetNormal;

            if (angle2<options.threshold)
               averageNormal2 = ~(currentFacetNormal+nextFacetNormal);
            else
               averageNormal2 = currentFacetNormal;


	   /*____ If necessary, calculate the normals of each bevel's facet.         ____*/

            if (doFrontBevels)
               frontBevelNormal1 = ~(zDir*options.frontSideCut
                                     + averageNormal1*options.frontFaceCut);
            if (doBackBevels)
               backBevelNormal1  = ~(averageNormal1*options.backFaceCut
                                     - zDir*options.backSideCut );
            if (doFrontBevels)
               frontBevelNormal2 = ~(zDir*options.frontSideCut
                                     + averageNormal2*options.frontFaceCut);
            if (doBackBevels)
               backBevelNormal2  = ~(averageNormal2*options.backFaceCut
                                     - zDir*options.backSideCut);


	   /*____ Here is where we make sure that all the z-coordinates of each     ____*/
	   /*____ triangle vertex are correct.                                      ____*/

            if (doFrontBevels)
            {
               p1 = frontShrunkCurrent1;
               p1.z = zpos1;
            }
            if (doFrontBevels)
            {
               p2 = frontShrunkCurrent2;
               p2.z = zpos1;
            }
            p3 = current1      ; p3.z=zpos2;
            p4 = current2      ; p4.z=zpos2;
            p5 = current1      ; p5.z=zpos3;
            p6 = current2      ; p6.z=zpos3;
            if (doBackBevels)
            {
               p7 = backShrunkCurrent1;
               p7.z = zpos4;
            }
            if (doBackBevels)
            {
               p8 = backShrunkCurrent2;
               p8.z = zpos4;
            }

	   /*____ t3 and t4 are the side triangles. ____*/

            t3 = new TRIANGLE(p3,p5,p6,averageNormal1,averageNormal1,averageNormal2);
            if (t3==NULL) {
              f_erreur("Out of memory.|Try to add more virtual memory.");
              return (1);
            }
            t4 = new TRIANGLE(p3,p6,p4,averageNormal1,averageNormal2,averageNormal2);
            if (t4==NULL) {
              f_erreur("Out of memory.|Try to add more virtual memory.");
              return (1);
            }

	   /*____ t1 and t2 are the front bevel triangles. ____*/

            if (doFrontBevels)
            {
               if (options.bevelType==FLAT)
               {
                  t1 = new TRIANGLE(p1,p2,p3,frontBevelNormal1,frontBevelNormal2,
                                             frontBevelNormal1);
                  if (t1==NULL) {
                     f_erreur("Out of memory.|Try to add more virtual memory.");
                     return 1;
                  }
                  t2 = new TRIANGLE(p2,p3,p4,frontBevelNormal2,frontBevelNormal1,
                                             frontBevelNormal2);
                  if (t2==NULL) {
                     f_erreur("Out of memory.|Try to add more virtual memory.");
                     return 1;
                  }
               }
               else
               {
                  t1 = new TRIANGLE(p1,p2,p3,zDir,zDir,averageNormal1);
                  if (t1==NULL) {
                     f_erreur("Out of memory.|Try to add more virtual memory.");
                     return 1;
                  }
                  t2 = new TRIANGLE(p2,p3,p4,zDir,averageNormal1,averageNormal2);
                  if (t2==NULL) {
                     f_erreur("Out of memory.|Try to add more virtual memory.");
                     return 1;
                  }
               }
            }

	   /*____ t5 and t6 are the back bevel triangles. ____*/

            if (doBackBevels)
            {
              if (options.bevelType==FLAT)
              {
                t5 = new TRIANGLE(p5,p7,p6,backBevelNormal1,backBevelNormal1,
                                           backBevelNormal2);
                if (t5==NULL) {
                     f_erreur("Out of memory.|Try to add more virtual memory.");
                     return 1;
                  }
                t6 = new TRIANGLE(p7,p8,p6,backBevelNormal1,backBevelNormal2,
                                           backBevelNormal2);
                if (t6==NULL) {
                     f_erreur("Out of memory.|Try to add more virtual memory.");
                     return 1;
                  }
              }
              else
              {
                t5 = new TRIANGLE(p5,p7,p6,averageNormal1,-zDir,averageNormal2);
                if (t5==NULL) {
                     f_erreur("Out of memory.|Try to add more virtual memory.");
                     return 1;
                  }
                t6 = new TRIANGLE(p7,p8,p6,-zDir,-zDir,averageNormal2);
                if (t6==NULL) {
                     f_erreur("Out of memory.|Try to add more virtual memory.");
                     return 1;
                  }
              }
            }

	   /*____ Add the triangles to their appropriate lists. ____*/

            sideTriangleList.Add(t3);
            sideTriangleList.Add(t4);
            if (doFrontBevels)
            {
               frontBevelTriangleList.Add(t1);
               frontBevelTriangleList.Add(t2);
            }
            if (doBackBevels)
            {
               backBevelTriangleList.Add(t5);
               backBevelTriangleList.Add(t6);
            }
         }

      }

      /*____ Now clean up. ____*/

      for (i=0;i<contourCount;i++)
      {
        delete polyArray[i].pointlist;
        if (doFrontBevels) delete frontShrunkPolyArray[i].pointlist;
        if (doBackBevels) delete backShrunkPolyArray[i].pointlist;
      }

      delete polyArray;
      if (doFrontBevels) delete frontShrunkPolyArray;
      if (doBackBevels) delete backShrunkPolyArray;

      return ERR_NoError;

   }
