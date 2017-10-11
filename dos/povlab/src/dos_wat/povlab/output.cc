/* ---------------------------------------------------------------------------
*  OUTPUT.CC
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

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

#include "config.h"
#include "font3d.h"
#include "truetype.h"
#include "geometry.h"


#define DEGENERATE_THRESHOLD 1.0e-9


/*==============================================================================================*/
/*  OutputTriangles()                                                                           */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void OutputTriangles(ostream& outputFile, USHORT outputFormat,                */
/*                                     USHORT triangleType, USHORT spacesToIndent,              */
/*                                     TRIANGLELIST& triangleList);                             */
/*                                                                                              */
/*  DESCRIPTION:  This function outputs a list of triangles to the stream 'outputFile'.         */
/*                The 'outputFormat' parameter specifies the output format (ie. POV, RAW,       */
/*                RIB, etc.);  the following symbolic constants are defined in config.h:        */
/*                                                                                              */
/*                     Symbol           Description                                             */
/*                 ------------------------------------------------------------------------     */
/*                     RAW..............Output raw vertex data (ie. nine space-delimited        */
/*                                      values per line, corresponding to the x-y-z coor-       */
/*                                      dinates of each vertex.)                                */
/*                     RIB..............Output RenderMan compatible triangles.                  */
/*                     POV..............Output Persistence of Vision V2.x compatible            */
/*                                      triangles.                                              */
/*                                                                                              */
/*                The 'triangleType' parameter specifies the type of triangle to output.        */
/*                The following symbolic constants are defined in config.h:                     */
/*                                                                                              */
/*                     Symbol           Description                                             */
/*                 ------------------------------------------------------------------------     */
/*                     FLAT.............Output flat triangles (only vertex data).               */
/*                     SMOOTH...........Output smooth triangles (vertices and normals).         */
/*                                                                                              */
/*                The 'spacesToIndent' parameter specifies the number of spaces to indent       */
/*                each line of output.                                                          */
/*                                                                                              */
/*  NOTES:        When adding a new output format, you must add a case to BOTH switch           */
/*                statements, even if they are both exactly the same.                           */
/*                                                                                              */
/*==============================================================================================*/

   void OutputTriangles(ostream&      outputFile,
                        USHORT        outputFormat,
                        USHORT        triangleType,
                        USHORT        spacesToIndent,
                        TRIANGLELIST& triangleList)
   {

      INT       i;
      CHARPTR   indentString;
      TRIANGLE* t;

      VECTOR    side1, side2;
      VECTOR    normal;

      indentString = new CHAR[spacesToIndent+1];
      for (i=0;i<spacesToIndent;i++) indentString[i]=' ';
      indentString[i]=0x00;

      triangleList.gotoFirst();

      f_jauge(2,AFFICHE,0,0,"Computing faces");

      for (i=0;i<triangleList.Count();i++) {
         f_jauge(2,MODIF,i,triangleList.Count(),NULL);
         t = triangleList.Current();

         side1  = t->v1 - t->v2;
         side2  = t->v3 - t->v2;
         normal = side1 ^ side2;

         if (length(normal) < DEGENERATE_THRESHOLD)
	 {
            triangleList.gotoNext();
            continue;
	 }

         
         if (outputFormat==RAW) {
            outputFile<<indentString;
            outputFile<<t->v1.x<<' '<<t->v1.y<<' '<<t->v1.z<<' ';
            outputFile<<t->v2.x<<' '<<t->v2.y<<' '<<t->v2.z<<' ';
            outputFile<<t->v3.x<<' '<<t->v3.y<<' '<<t->v3.z<<endl;
         }

         triangleList.gotoNext();
      }

      delete indentString;
      f_jauge(2,EFFACE,0,0,NULL);

   }



/*==============================================================================================*/
/*  OutputObjects()                                                                             */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       USHORT OutputObjects(ostream& outputFile, TTFont& font,                       */
/*                                     Font3DOptions& options);                                 */
/*                                                                                              */
/*  DESCRIPTION:  This function outputs the triangle information that makes up a group of       */
/*                glyphs to a specified stream.  The string to generate is found in the         */
/*                Font3DOptions structure 'options', the font to use is given by 'font',        */
/*                and the output stream is 'outputFile'.                                        */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function:                     */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_OutOfMemory...........An attempt to allocate memory failed.            */
/*                   There are more...                                                          */
/*                                                                                              */
/*==============================================================================================*/

   USHORT OutputObjects(ostream&       outputFile,
                        TTFont&        font,
                        Font3DOptions& options)
   {
      INT          i;
      INT          triangleCount;
      TRIANGLELIST frontFaceTriangleList;
      TRIANGLELIST backFaceTriangleList;
      TRIANGLELIST frontBevelTriangleList;
      TRIANGLELIST backBevelTriangleList;
      TRIANGLELIST sideTriangleList;
      USHORT       success;
      USHORT       code;
      DOUBLE       startX,endX;

      DOUBLE xmin,xmax,ymin,ymax,zmin,zmax,upem,xdelta,ydelta;
      DOUBLE thisymin,thisymax;
      VECTOR offset(0.0,0.0,0.0);

      USHORT       partsCount;

      outputFile<<setprecision(options.outputPrecision);
      code = options.c;
      upem = (DOUBLE)font.UnitsPerEm();

      if (options.string==NULL)
      {
         xmin = (DOUBLE)font.GlyphXMin(font.CharacterMap(code))/(DOUBLE)upem;
         xmax = (DOUBLE)font.GlyphXMax(font.CharacterMap(code))/(DOUBLE)upem;

         if (options.xPosition==LEFT) xdelta = -xmin;
         else if (options.xPosition==CENTER) xdelta = -(xmin+xmax)/2;
         else xdelta = -xmax;

         xmin+=xdelta;
         xmax+=xdelta;

         ymin = (DOUBLE)font.GlyphYMin(font.CharacterMap(code))/(DOUBLE)upem;
         ymax = (DOUBLE)font.GlyphYMax(font.CharacterMap(code))/(DOUBLE)upem;

         if (options.yPosition==BOTTOM) ydelta = -ymin;
         else if (options.yPosition==BASELINE) ydelta=0.0;
         else if (options.yPosition==CENTER) ydelta = -(ymin+ymax)/2;
         else ydelta = -ymax;

         ymin+=ydelta;
         ymax+=ydelta;

         if (options.zPosition==FRONT) { zmin=0; zmax=options.depth; }
         else if (options.zPosition==CENTER) { zmin=-options.depth/2; zmax=options.depth/2; }
         else { zmin=-options.depth; zmax=0; }

         offset = VECTOR(xdelta,ydelta,0.0);

         success = CreateFaces(font, font.CharacterMap(code),
                               options, offset, frontFaceTriangleList,
                               backFaceTriangleList);

         if (success!=ERR_NoError)
         {
            return success;
         }

         success = CreateSides(font, font.CharacterMap(code),
                               options, offset, frontBevelTriangleList,
                               backBevelTriangleList, sideTriangleList);

         if (success!=ERR_NoError)
         {
            frontFaceTriangleList.Empty();
            backFaceTriangleList.Empty();
            return success;
         }

      }
      else
      {
         startX = font.Glyph(options.string[0])->LeftSideBearing()/upem;
         endX=0;

         ymin = (DOUBLE)font.GlyphYMin(font.CharacterMap(options.string[0]))/(DOUBLE)upem;
         ymax = (DOUBLE)font.GlyphYMax(font.CharacterMap(options.string[0]))/(DOUBLE)upem;

         for (i=0;i<options.stringLength;i++)
         {
            endX+=( font.Glyph(options.string[i])->AdvanceWidth())/upem;
            if (i<options.stringLength) 
               endX += font.Kerning(font.CharacterMap(options.string[i]),
                                    font.CharacterMap(options.string[i+1]))/upem;

            thisymin=(DOUBLE)font.GlyphYMin(font.CharacterMap(options.string[i]))/(DOUBLE)upem;
            thisymax=(DOUBLE)font.GlyphYMax(font.CharacterMap(options.string[i]))/(DOUBLE)upem;

            if (ymin>thisymin) ymin=thisymin;
            if (ymax<thisymax) ymax=thisymax;

         }

         endX -= font.Glyph(options.string[i-1])->RightSideBearing()/upem;

         xmin = startX;
         xmax = endX;

         if (options.xPosition==LEFT) xdelta = -xmin;
         else if (options.xPosition==CENTER) xdelta = -(xmin+xmax)/2;
         else xdelta = -xmax;

         xmin+=xdelta;
         xmax+=xdelta;

         if (options.yPosition==BOTTOM) ydelta = -ymin;
         else if (options.yPosition==BASELINE) ydelta = 0.0;
         else if (options.yPosition==CENTER) ydelta = -(ymin+ymax)/2;
         else ydelta = -ymax;

         ymin+=ydelta;
         ymax+=ydelta;

         if (options.zPosition==FRONT) { zmin=0; zmax=options.depth; }
         else if (options.zPosition==CENTER) { zmin=-options.depth/2; zmax=options.depth/2; }
         else { zmin=-options.depth; zmax=0; }

         offset = VECTOR(xdelta,ydelta,0.0);

         f_jauge(0,AFFICHE,0,0,"Scanning string");

         for (i=0;i<options.stringLength;i++) {

            success = CreateFaces(font, font.CharacterMap(options.string[i]),
                                  options, offset, frontFaceTriangleList,
                                  backFaceTriangleList);

            if (success!=ERR_NoError) {
              f_jauge(0,EFFACE,0,0,NULL);
              return success;
            }

            success = CreateSides(font, font.CharacterMap(options.string[i]),
                                  options, offset, frontBevelTriangleList,
                                  backBevelTriangleList, sideTriangleList);

            if (success!=ERR_NoError) {
               frontFaceTriangleList.Empty();
               backFaceTriangleList.Empty();
               f_jauge(0,EFFACE,0,0,NULL);
               return success;
            }

            offset.x += (  font.Glyph(options.string[i])->AdvanceWidth() )/upem; 
            if (i<options.stringLength-1) {
               offset.x += font.Kerning(font.CharacterMap(options.string[i]),
                                        font.CharacterMap(options.string[i+1]))/upem;
            }
            f_jauge(0,MODIF,i,options.stringLength,NULL);
         }
         f_jauge(0,EFFACE,0,0,NULL);
      }


      triangleCount = 0;

      if (options.frontFaceVisible)
         triangleCount+=frontFaceTriangleList.Count();  
      if (options.backFaceVisible)
         triangleCount+=backFaceTriangleList.Count();
      if (options.frontBevelVisible
          && (options.frontFaceCut!=0.0 || options.frontSideCut!=0.0))
         triangleCount+=frontBevelTriangleList.Count();
      if (options.backBevelVisible
          && (options.backFaceCut!=0.0 || options.backSideCut!=0.0))
         triangleCount+=backBevelTriangleList.Count();
      if (options.sideVisible)
         triangleCount+=sideTriangleList.Count();



     /*---------------------------------------------------------------------------*/
     /*  Output RAW Triangles...                                                  */
     /*---------------------------------------------------------------------------*/

      if (options.outputFormat==RAW)
      {
         OutputTriangles(outputFile,RAW,FLAT,0,frontFaceTriangleList);
         OutputTriangles(outputFile,RAW,FLAT,0,backFaceTriangleList);
         OutputTriangles(outputFile,RAW,FLAT,0,frontBevelTriangleList);
         OutputTriangles(outputFile,RAW,FLAT,0,backBevelTriangleList);
         if (options.sideVisible) OutputTriangles(outputFile,RAW,FLAT,0,sideTriangleList);
      }

      frontFaceTriangleList.Empty();
      backFaceTriangleList.Empty();
      frontBevelTriangleList.Empty();
      backBevelTriangleList.Empty();
      sideTriangleList.Empty();

      return ERR_NoError;

   }
