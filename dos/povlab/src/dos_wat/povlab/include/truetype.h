/*==============================================================================================*/
/*   truetype.h                                                                    Font3D       */
/*----------------------------------------------------------------------------------------------*/
/*                                                                                              */
/*   Copyright (c) 1994, 1995 by Todd A. Prater                                 Version 1.50    */
/*   All rights reserved.                                                                       */
/*                                                                                              */
/*==============================================================================================*/

#ifndef __TrueType_H__
#define __TrueType_H__

#include <stddef.h>
#include "config.h"

/*===============================================*/
/*  Platform Identifiers                         */
/*===============================================*/

#define PID_AppleUnicode                 0
#define PID_Macintosh                    1
#define PID_ISO                          2
#define PID_Microsoft                    3


/*===============================================*/
/*  Macintosh Specific Platforms                 */
/*===============================================*/

#define SID_MAC_Roman                    0
#define SID_MAC_Japanese                 1
#define SID_MAC_Chinese                  2
#define SID_MAC_Korean                   3
#define SID_MAC_Arabic                   4
#define SID_MAC_Hebrew                   5
#define SID_MAC_Greek                    6
#define SID_MAC_Russian                  7
#define SID_MAC_RSymbol                  8
#define SID_MAC_Devanagari               9
#define SID_MAC_Gurmukhi                10
#define SID_MAC_Gujarati                11
#define SID_MAC_Oriya                   12
#define SID_MAC_Bengali                 13
#define SID_MAC_Tamil                   14
#define SID_MAC_Telugu                  15
#define SID_MAC_Kannada                 16
#define SID_MAC_Malayalam               17
#define SID_MAC_Sinhalese               18
#define SID_MAC_Burmese                 19
#define SID_MAC_Khmer                   20
#define SID_MAC_Thai                    21
#define SID_MAC_Laotian                 22
#define SID_MAC_Georgian                23
#define SID_MAC_Armenian                24
#define SID_MAC_Maldivian               25
#define SID_MAC_Tibetian                26
#define SID_MAC_Mongolian               27
#define SID_MAC_Geez                    28
#define SID_MAC_Slavic                  29
#define SID_MAC_Vietnamese              30
#define SID_MAC_Sindhi                  31
#define SID_MAC_Uninterp                32


/*===============================================*/
/*  Microsoft Specific Platforms                 */
/*===============================================*/

#define SID_MS_Undefined                 0
#define SID_MS_UGL                       1


/*===============================================*/
/*  ISO Specific Platforms                       */
/*===============================================*/

#define SID_ISO_ASCII                    0
#define SID_ISO_10646                    1
#define SID_ISO_8859_1                   2


/*===============================================*/
/*  Microsoft Language Identifiers               */
/*===============================================*/

#define LID_MS_Arabic               0x0401
#define LID_MS_Bulgarian            0x0402
#define LID_MS_Catalan              0x0403
#define LID_MS_TraditionalChinese   0x0404
#define LID_MS_SimplifiedChinese    0x0804
#define LID_MS_Czech                0x0405
#define LID_MS_Danish               0x0406
#define LID_MS_German               0x0407
#define LID_MS_SwissGerman          0x0807
#define LID_MS_Greek                0x0408
#define LID_MS_USEnglish            0x0409
#define LID_MS_UKEnglish            0x0809
#define LID_MS_CastilianSpanish     0x040a
#define LID_MS_MexicanSpanish       0x080a
#define LID_MS_ModernSpanish        0x0c0a
#define LID_MS_Finnish              0x040b
#define LID_MS_French               0x040c
#define LID_MS_BelgianFrench        0x080c
#define LID_MS_CanadianFrench       0x0c0c
#define LID_MS_SwissFrench          0x100c
#define LID_MS_Hebrew               0x040d
#define LID_MS_Hungarian            0x040e
#define LID_MS_Icelandic            0x040f
#define LID_MS_Italian              0x0410
#define LID_MS_SwissItalian         0x0810
#define LID_MS_Japanese             0x0411
#define LID_MS_Korean               0x0412
#define LID_MS_Dutch                0x0413
#define LID_MS_BelgianDutch         0x0813
#define LID_MS_NorwegianBokmal      0x0414
#define LID_MS_NorwegianNynorsk     0x0814
#define LID_MS_Polish               0x0415
#define LID_MS_BrazilianPortuguese  0x0416
#define LID_MS_Portuguese           0x0816
#define LID_MS_RhaetoRomanic        0x0417
#define LID_MS_Romanian             0x0418
#define LID_MS_Russian              0x0419
#define LID_MS_CroatoSerbian        0x041a
#define LID_MS_SerboCroatian        0x081a
#define LID_MS_Slovakian            0x041b
#define LID_MS_Albanian             0x041c
#define LID_MS_Swedish              0x041d
#define LID_MS_Thai                 0x041e
#define LID_MS_Turkish              0x041f
#define LID_MS_Urdu                 0x0420
#define LID_MS_Bahasa               0x0421


/*===============================================*/
/*  Macintosh Language Identifiers               */
/*===============================================*/

#define LID_MAC_English                  0
#define LID_MAC_French                   1
#define LID_MAC_German                   2
#define LID_MAC_Italian                  3
#define LID_MAC_Dutch                    4
#define LID_MAC_Swedish                  5
#define LID_MAC_Spanish                  6
#define LID_MAC_Danish                   7
#define LID_MAC_Portuguese               8
#define LID_MAC_Norwegian                9
#define LID_MAC_Hebrew                  10
#define LID_MAC_Japanese                11
#define LID_MAC_Arabic                  12
#define LID_MAC_Finnish                 13
#define LID_MAC_Greek                   14
#define LID_MAC_Icelandic               15
#define LID_MAC_Maltese                 16
#define LID_MAC_Turkish                 17
#define LID_MAC_Yugoslavian             18
#define LID_MAC_Chinese                 19
#define LID_MAC_Urdu                    20
#define LID_MAC_Hindi                   21
#define LID_MAC_Thai                    22


/*===============================================*/
/*  Name Identifiers                             */
/*===============================================*/

#define NID_Copyright                    0
#define NID_Family                       1
#define NID_Subfamily                    2
#define NID_UniqueID                     3
#define NID_FullName                     4
#define NID_Version                      5
#define NID_PostscriptName               6
#define NID_Trademark                    7


/*===============================================*/
/*  CMAP Table Formats                           */
/*===============================================*/

#define CMAP_FORMAT0   0
#define CMAP_FORMAT4   4


#define OFFSET_TABLE_SIZE 12



/* THESE WILL PROBABLY BE REMOVED */

#define MACINTOSH  1
#define MICROSOFT  2




   class TTKernPair
   {
      public:  USHORT   left;
               USHORT   right;
               SHORT    value;
   };



   class TTPoint
   {
      friend class TTFont;

      public:  SHORT    x;
               SHORT    y;
               BYTE     type;
   };



   class TTContour
   {
      friend class TTFont;

      private: USHORT     numPoints;
               TTPoint*   point;

      public:  TTContour(void)
               {
                  numPoints = 0;
                  point     = NULL;
               }

               USHORT NumPoints()
               {
                  return numPoints;
               }

               TTPoint* Point(USHORT index)
               {
                  if (index<numPoints && point!=NULL)
                     return &(point[index]);
                  else
                     return NULL;
               }

   };



   class TTGlyph
   {
      friend class TTFont;

      private: SHORT      numContours;
               SHORT      xMin;
               SHORT      yMin;
               SHORT      xMax;
               SHORT      yMax;
               TTContour* contour;
               USHORT     advanceWidth;
               SHORT      leftSideBearing;
               SHORT      rightSideBearing;

      public:  TTGlyph(void)
               {
                  numContours      = 0;
                  xMin             = 0;
                  xMax             = 0;
                  yMin             = 0;
                  yMax             = 0;
                  advanceWidth     = 0;
                  leftSideBearing  = 0;
                  rightSideBearing = 0;
                  contour          = NULL;
               }

               USHORT AdvanceWidth()    { return advanceWidth;     }
               SHORT LeftSideBearing()  { return leftSideBearing;  }             
               SHORT RightSideBearing() { return rightSideBearing; }
               SHORT NumContours()      { return numContours;      }
               SHORT XMin()             { return xMin;             }
               SHORT YMin()             { return yMin;             }
               SHORT XMax()             { return xMax;             }
               SHORT YMax()             { return yMax;             }

               TTContour* Contour(USHORT index)
               {
                  if (index<numContours && numContours>0 && contour!=NULL)
                     return &(contour[index]);
                  else
                     return NULL;
               }
   };


   class TTFont
   {
      private: ULONG         fontDataSize;

               BYTE*         fontData;

               TTGlyph*      glyph;
               BYTE*         cmap;
               ULONG*        glyphOffsetArray;
               TTKernPair*   kernPair;

               USHORT        numberOfHMetrics;
               USHORT        numKernPairs;
               USHORT        numTables;
               USHORT        cmapFormat;

               USHORT        lastError;
               SHORT         indexToLocFormat;
               USHORT        numGlyphs;
               DOUBLE        revision;
               CHARPTR       copyright;
               CHARPTR       familyName;
               CHARPTR       fullName;
               CHARPTR       subfamilyName;
               CHARPTR       uniqueName;
               CHARPTR       versionName;
               SHORT         xMax;
               SHORT         xMin;
               SHORT         yMax;
               SHORT         yMin;
               USHORT        unitsPerEm;
               USHORT        ascender;
               USHORT        descender;
               USHORT        lineGap;
               USHORT        platformID;
               USHORT        specificID;
               USHORT        languageID;

               void   readFontData                 (CHARPTR fileName, CHARPTR pathName);
               void   getTableDirEntry             (ULONG tag, ULONG* checkSum, 
                                                    ULONG* offset, ULONG* length);
               void   processFontHeaderTable       (void);
               void   processMaximumProfileTable   (void);
               void   processNamingTable           (void);
               void   processIndexToLocationTable  (void);
               void   processCharacterMappingTable (void);
               void   processGlyphDataTable        (void);
               void   processHorizontalHeaderTable (void);
               void   processHorizontalMetricsTable(void);
               void   processKerningTable          (void);
               USHORT getGlyphIndex                (USHORT charCode);


      public:  TTFont(CHARPTR fontFileName, CHARPTR fontPathName, 
                      USHORT pid, USHORT sid, USHORT lid);

              /* THIS NEXT CONSTRUCTOR WILL BE EVENTUALLY REMOVED */

               TTFont(CHARPTR fontFileName,  CHARPTR fontPathName, INT mapType);



               USHORT   LastError()     { return lastError;     }
               USHORT   UnitsPerEm()    { return unitsPerEm;    }
               SHORT    XMax()          { return xMax;          }
               SHORT    XMin()          { return xMin;          }
               SHORT    YMax()          { return yMax;          }
               SHORT    YMin()          { return yMin;          }        
               SHORT    Ascender()      { return ascender;      }
               SHORT    Descender()     { return descender;     }
               SHORT    LineGap()       { return lineGap;       }
               CHARPTR  Copyright()     { return copyright;     }
               CHARPTR  Family()        { return familyName;    }
               CHARPTR  FullName()      { return fullName;      }
               CHARPTR  Subfamily()     { return subfamilyName; } 
               CHARPTR  UniqueID()      { return uniqueName;    }
               CHARPTR  Version()       { return versionName;   }

               TTGlyph* Glyph(USHORT code);
               SHORT    Kerning(USHORT,USHORT);



             /* THE FOLLOWING FUNCTIONS WILL PROBABLY BE REMOVED VERY SOON */
             /* NOW.  THEY ARE ONLY PROVIDED HERE, SO THAT THIS MODULE     */
             /* WILL WORK (HOPEFULLY) CORRECTLY WITH THE OTHER CODE YOU    */
             /* HAVE>                                                      */

               USHORT   CharacterMap(USHORT c)
               {
                  return getGlyphIndex(c);
               }

               USHORT NumGlyphs(void)
               {
                  return numGlyphs;
               }

               SHORT  NumContours(USHORT glyphnum)
               {
                  if (numGlyphs!=0 && glyphnum<numGlyphs)
                     return glyph[glyphnum].numContours;
                  else
                     return 0;
               }

               USHORT NumPoints(USHORT glyphnum, USHORT contournum)
               {
                  if (glyphnum<numGlyphs
                      && glyph[glyphnum].numContours>0
                      && contournum<glyph[glyphnum].numContours)
                     return glyph[glyphnum].contour[contournum].numPoints;
                  else
                     return 0;
               }

               LONG FontPointX (USHORT glyphnum, USHORT contournum, USHORT pointnum)
               {
                  if( glyphnum<numGlyphs
                      && glyph[glyphnum].numContours>0
                      && contournum<glyph[glyphnum].numContours
                      && glyph[glyphnum].contour[contournum].numPoints!=0
                      && glyph[glyphnum].contour[contournum].numPoints!=0 )
                     return glyph[glyphnum].contour[contournum].point[pointnum].x;
                  else
                     return 0;
               }

               LONG FontPointY (USHORT glyphnum, USHORT contournum, USHORT pointnum)
               {
                  if( glyphnum<numGlyphs
                      && glyph[glyphnum].numContours>0
                      && contournum<glyph[glyphnum].numContours
                      && glyph[glyphnum].contour[contournum].numPoints!=0 )
                     return glyph[glyphnum].contour[contournum].point[pointnum].y;
                  else
                     return 0;
               }

               SHORT FontPointType (USHORT glyphnum, USHORT contournum, USHORT pointnum)
               {
                  if( glyphnum<numGlyphs
                      && glyph[glyphnum].numContours>0
                      && contournum<glyph[glyphnum].numContours
                      && glyph[glyphnum].contour[contournum].numPoints!=0 )
                     return glyph[glyphnum].contour[contournum].point[pointnum].type;


                  else
                     return NO_POINT;
               }

               SHORT GlyphXMin(USHORT glyphnum)
               {
                  if (glyphnum<numGlyphs)
                     return glyph[glyphnum].xMin;
                  else
                     return 0;
               }

               SHORT GlyphYMin(USHORT glyphnum)
               {
                  if (glyphnum<numGlyphs)
                     return glyph[glyphnum].yMin;
                  else
                     return 0;
               }

               SHORT GlyphXMax(USHORT glyphnum)
               {
                  if (glyphnum<numGlyphs)
                     return glyph[glyphnum].xMax;
                  else
                     return 0;
               }

               SHORT GlyphYMax(USHORT glyphnum)
               {
                  if (glyphnum<numGlyphs)
                     return glyph[glyphnum].yMax;
                  else
                     return 0;
               }



   };


#endif
