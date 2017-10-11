/*==============================================================================================*/
/*   font3d.h                                                                      Font3D       */
/*----------------------------------------------------------------------------------------------*/
/*                                                                                              */
/*   Copyright (c) 1994, 1995 by Todd A. Prater                                 Version 1.50    */
/*   All rights reserved.                                                                       */
/*                                                                                              */
/*==============================================================================================*/

#ifndef __Font3D_H__
#define __Font3D_H__

#include <fstream.h>
#include <stddef.h>

#include "config.h"
#include "vector.h"
#include "truetype.h"
#include "geometry.h"

class Font3DOptions
{
   public:
      CHARPTR fontFileName;
      CHARPTR fontPathName;
      CHARPTR outputFileName;
      CHARPTR outputPathName;
      CHARPTR objectName;
      USHORT  outputFormat;
      USHORT  bevelType;
      USHORT  triangleType;
      DOUBLE  depth;
      USHORT  xPosition;
      USHORT  yPosition;
      USHORT  zPosition;
      USHORT  frontFaceVisible;
      USHORT  backFaceVisible;
      USHORT  frontBevelVisible;
      USHORT  backBevelVisible;
      USHORT  sideVisible;
      USHORT  c;
      USHORT  stringLength;
      USHORT* string;
      DOUBLE  frontFaceCut;
      DOUBLE  backFaceCut;
      DOUBLE  frontSideCut;
      DOUBLE  backSideCut;
      char*   frontFaceTextureName;
      char*   backFaceTextureName;
      char*   frontBevelTextureName;
      char*   backBevelTextureName;
      char*   sideTextureName;
      DOUBLE  spacing;
      DOUBLE  spaceto;
      DOUBLE  threshold;
      USHORT  resolution;
      USHORT  mapType;
      USHORT  language;
      USHORT  outputPrecision;

      Font3DOptions()
      {
         fontFileName          = NULL;
         fontPathName          = NULL;

         outputFileName        = new char[12];
         strcpy (outputFileName,"font3d.inc");

         outputPathName        = NULL;

         objectName            = new char[14];
         strcpy (objectName,"FONT3D_OBJECT");

         outputFormat          = POV;
         bevelType             = FLAT;
         triangleType          = SMOOTH;
         depth                 = 0.2;
         xPosition             = CENTER;
         yPosition             = CENTER;
         zPosition             = CENTER;
         frontFaceVisible      = TRUE;
         backFaceVisible       = TRUE;
         frontBevelVisible     = TRUE;
         backBevelVisible      = TRUE;
         sideVisible           = TRUE;
         c                     = 'A';
         string                = NULL;
         stringLength          = 0;
         frontFaceCut          = 0.0;
         backFaceCut           = 0.0;
         frontSideCut          = 0.0;
         backSideCut           = 0.0;
         frontFaceTextureName  = NULL;
         backFaceTextureName   = NULL;
         frontBevelTextureName = NULL;
         backBevelTextureName  = NULL;
         sideTextureName       = NULL;
         spacing               = 0.0;
         spaceto               = 0.0;
         threshold             = 0.4;  /* ? */
         resolution            = 5;
         mapType               = PID_Microsoft;
         language              = LID_MS_USEnglish;
         outputPrecision       = 6;
      }

};

void OutputTriangles(ostream& outputFile, USHORT outputFormat,
                     USHORT triangleType, USHORT spacesToIndent,
                     TRIANGLELIST& triangleList);

USHORT OutputObjects(ostream& outputFile, TTFont& font,
                    Font3DOptions& options);

void PolygonizeContour(TTFont&, USHORT, USHORT, USHORT, DOUBLE, POLYGON&);

USHORT CreateFaces(TTFont& font,USHORT glyphnum, Font3DOptions& options,
                   VECTOR offset, TRIANGLELIST& frontTriangleList,
                   TRIANGLELIST& backTriangleList);

USHORT CreateSides(TTFont& font, USHORT glyphnum, Font3DOptions& options,
                   VECTOR offset, TRIANGLELIST& frontBevelTriangleList,
                   TRIANGLELIST& backBevelTriangleList,
                   TRIANGLELIST& sideTriangleList);

int ReadOptions(char* fileName, int& numArgs, char ** & args);
int ParseOptions(int numArgs, char* argument[], Font3DOptions& defaultOptions,
                 int& errorCode, int& errorPos);

#endif




