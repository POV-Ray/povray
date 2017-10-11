/* ---------------------------------------------------------------------------
*  TRUETYPE.CC
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

/*==============================================================================================*/
/*  truetype.cc                                                                    Font3D       */
/*==============================================================================================*/

#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <iostream.h>
#include "config.h"
#include "truetype.h"

#define MAX_FILENAME_SIZE 1024


/*==============================================================================================*/
/*  TTFont::readFontData()                                                           (PRIVATE)  */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void readFontData(CHARPTR fileName, CHARPTR pathName);                        */
/*                                                                                              */
/*  DESCRIPTION:  This function stores an entire TrueType font file in a BYTE array.  Upon      */
/*                successful completion 'TTFont.fontData' will contain the font data, and       */
/*                'TTFont.fontDataSize' will contain the length (in bytes) of that array.       */
/*                The font is specified by supplying its correct drive, path, and filename      */
/*                in 'fileName'.                                                                */
/*                                                                                              */
/*  NOTES:        This function can be made (much) simpler with the use of a library routine    */
/*                that returns the size (in bytes) of a particular file.  This is not a         */
/*                standard C function, however, so, in the interest of portability, this        */
/*                routine determines the file size from the first few bytes it reads in.        */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_OutOfMemory...........An attempt to allocate memory failed.            */
/*                   ERR_UnableToOpenFile......An attempt to open the specified file failed.    */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::readFontData(CHARPTR inputFileName, CHARPTR inputPathName)
   {

      INT    i;                                       /* Counter                                */
      ULONG  tablelength;                             /* Length of a single table               */

      BYTE   tfd[OFFSET_TABLE_SIZE];                  /* Holds the Offset Table data            */
      BYTE*  tabledir;                                /* Holds the Table Directory data         */

      ULONG  tabledirsize;                            /* Size of the Table Directory            */
      ULONG  tabledatasize;                           /* Sum of the lengths of each Table       */

      FILE*  inputFileStream;

      if (inputPathName==NULL)
      {
         inputFileStream = fopen((char*)inputFileName,"rb");
      }
      else
      {
	 char *p = inputPathName, *q, fileName[MAX_FILENAME_SIZE];
	 int done = 0;

	 while (!done) {
	    for (q = p; *q && *q != ';' && *q != ':'; q++);
	    if (!(*q)) done = 1;
	    *q++ = '\0';

	    sprintf(fileName, "%s/%s", p, inputFileName);
            inputFileStream = fopen((char*)fileName,"rb");
	    if (inputFileStream != NULL) done = 1;
	    p = q;
         }
	 
         if (inputFileStream==NULL)
            inputFileStream = fopen((char*)inputFileName,"rb");
      } 

      if (inputFileStream==NULL)
      {
         lastError = ERR_UnableToOpenFile;
         return;
      }

      for (i=0;i<OFFSET_TABLE_SIZE;i++)               /* Read in the Offset Table               */
         tfd[i]=(BYTE)getc(inputFileStream);

      numTables = toUSHORT(tfd[4],tfd[5]);            /* Get the number of Tables in this font  */
      tabledirsize = sizeof(ULONG)*4*numTables;       /* Calculate size of Table Directory      */
      tabledir = new BYTE[tabledirsize];              /* Allocate storage for Table Directory   */
      if (tabledir==NULL)
      {
         lastError = ERR_OutOfMemory;
         return;
      }

     /* ____  Next, store the Table Directory in a temporary array.  Each entry contains   ____ */
     /* ____  the length of that particular table, so keep track of the combined length of ____ */
     /* ____  all the tables while we're at it.  Each loop iteration reads in 16 bytes.    ____ */
     /* ____  Note that all tables are long aligned in the file itself, but the lengths    ____ */
     /* ____  given in the Table Directory are the actual lengths of the table data.  Any  ____ */
     /* ____  length not a multiple of four must be rounded up.                            ____ */
 
      tabledatasize=0;
      for (i=0;i<tabledirsize;i=i+16)
      {
         tabledir[i   ]=(BYTE)getc(inputFileStream); tabledir[i+1 ]=(BYTE)getc(inputFileStream);
         tabledir[i+2 ]=(BYTE)getc(inputFileStream); tabledir[i+3 ]=(BYTE)getc(inputFileStream);
         tabledir[i+4 ]=(BYTE)getc(inputFileStream); tabledir[i+5 ]=(BYTE)getc(inputFileStream);
         tabledir[i+6 ]=(BYTE)getc(inputFileStream); tabledir[i+7 ]=(BYTE)getc(inputFileStream);
         tabledir[i+8 ]=(BYTE)getc(inputFileStream); tabledir[i+9 ]=(BYTE)getc(inputFileStream);
         tabledir[i+10]=(BYTE)getc(inputFileStream); tabledir[i+11]=(BYTE)getc(inputFileStream);
         tabledir[i+12]=(BYTE)getc(inputFileStream); tabledir[i+13]=(BYTE)getc(inputFileStream);
         tabledir[i+14]=(BYTE)getc(inputFileStream); tabledir[i+15]=(BYTE)getc(inputFileStream);

         tablelength = toULONG(tabledir[i+12],tabledir[i+13],tabledir[i+14],tabledir[i+15]);
         if ((tablelength/4)*4 != tablelength) tablelength = (tablelength/4+1)*4;
         tabledatasize += tablelength;
      }

      fontDataSize = OFFSET_TABLE_SIZE               /* Calculate size of entire font file     */
                     + tabledirsize
                     + tabledatasize;

      fontData = new BYTE[fontDataSize];

      if (fontData==NULL)
      {                                               /* Allocate space for all that data       */
         lastError = ERR_OutOfMemory;
         return;
      }

      for (i=0;i<OFFSET_TABLE_SIZE;i++)                         /* Store the Offset Table       */
         fontData[i]=tfd[i];                                                                 
      for (;i<OFFSET_TABLE_SIZE+tabledirsize;i++)               /* Store the Table Directory    */
         fontData[i]=tabledir[i-OFFSET_TABLE_SIZE];
      for (;i<fontDataSize;i++)                                 /* Store the rest of the font   */
         fontData[i]=(BYTE)getc(inputFileStream);
 
      delete tabledir;                                          /* I got no more use for you    */

      lastError = ERR_NoError;
 
   }


/*==============================================================================================*/
/*  TTFont::getTableDirEntry()                                                      (PRIVATE)   */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void getTableDirEntry(ULONG tag, ULONG* checkSum,                             */
/*                                      ULONG* offset, ULONG* length);                          */
/*                                                                                              */
/*  DESCRIPTION:  This function scans the font's Table Directory for the length, offset and     */
/*                checksum information of a given table.                                        */
/*                                                                                              */
/*  NOTES:        Assumes 'readFontData()' has already been called successfully.                */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_TableNotFound.........The requested table was not found.               */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::getTableDirEntry(ULONG tag, ULONG* checkSum, ULONG* offset, ULONG* length)
   {
      INT      i;                                     /* Counter                                */
      ULONG    pos;                                   /* Current position in font data array    */
      USHORT   found;                                 /* TRUE if correct table is found         */
      ULONG    currentTag;                            /* Used for scanning tags in table dir.   */

      pos   = OFFSET_TABLE_SIZE;                      /* Table Dir starts after Offset Table    */

      found = FALSE;
      for (i=0;i<numTables;i++)
      {
         currentTag = toULONG(fontData[pos  ],fontData[pos+1],
                              fontData[pos+2],fontData[pos+3]);
         if (currentTag==tag)
         {
            *checkSum = toULONG(fontData[pos+ 4],fontData[pos+ 5],
                                fontData[pos+ 6],fontData[pos+ 7]);
            *offset   = toULONG(fontData[pos+ 8],fontData[pos+ 9],
                                fontData[pos+10],fontData[pos+11]);
            *length   = toULONG(fontData[pos+12],fontData[pos+13],
                                fontData[pos+14],fontData[pos+15]);
            found = TRUE;
            break;
         }
         pos+=16;
      }

      if (!found)
         lastError = ERR_TableNotFound;
      else
         lastError = ERR_NoError;

   }


/*==============================================================================================*/
/*  TTFont::processFontHeaderTable()                                                (PRIVATE)   */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void processFontHeaderTable();                                                */
/*                                                                                              */
/*  DESCRIPTION:  This function parses the font's Header Table.  The following values are       */
/*                needed from this table (all others are ignored):                              */
/*                                                                                              */
/*                   Value            Data Type    Description                                  */
/*                ---------------------------------------------------------------------------   */
/*                   unitsPerEm        USHORT      Granularity of the font's em square.         */
/*                   xMax              USHORT      Maximum X-coordinate for the entire font.    */
/*                   xMin              USHORT      Minimum X-coordinate for the entire font.    */
/*                   yMax              USHORT      Maximum Y-coordinate for the entire font.    */
/*                   yMin              USHORT      Minimum Y-coordinate for the entire font.    */
/*                   indexToLocFormat  SHORT       Used when processing the Index To Loc Table. */
/*                                                                                              */
/*  NOTES:        This function assumes that the font data has already been read in (by         */
/*                'readFontData()').  Also, this function must be called before the Index       */
/*                To Location Table is parsed.                                                  */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_TableNotFound.........The Font Header Table is not present in this     */
/*                                             font.                                            */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::processFontHeaderTable()
   {

      ULONG pos;
      ULONG tag,checkSum,offset,length;

      tag = toULONG((BYTE)'h',(BYTE)'e',(BYTE)'a',(BYTE)'d');   /* Get info about this table    */
      getTableDirEntry(tag,&checkSum,&offset,&length); 
      if (lastError != ERR_NoError) return;

      pos = offset;
      pos+=sizeof(Fixed);                                       /* Skip Table version number    */
      pos+=sizeof(Fixed);                                       /* Skip font revision number    */

      pos+=sizeof(ULONG);                                       /* Skip checksum adjustment     */
      pos+=sizeof(ULONG);                                       /* Skip magic number            */
      pos+=sizeof(USHORT);                                      /* Skip flags                   */

      unitsPerEm = toUSHORT(fontData[pos],fontData[pos+1]);     /* Get font units per em square */
                                  
      pos+=sizeof(USHORT);

      pos+=8;                                                   /* Skip date created            */
      pos+=8;                                                   /* Skip date modified           */

      xMin = toSHORT(fontData[pos],fontData[pos+1]);            /* get X-Min of all glyphs      */
      pos+=sizeof(SHORT);
      yMin = toSHORT(fontData[pos],fontData[pos+1]);            /* get Y-Min of all glyphs      */
      pos+=sizeof(SHORT);
      xMax = toSHORT(fontData[pos],fontData[pos+1]);            /* get X-Max of all glyphs      */
      pos+=sizeof(SHORT);
      yMax = toSHORT(fontData[pos],fontData[pos+1]);            /* get Y-Max of all glyphs      */
      pos+=sizeof(SHORT);
      pos+=sizeof(USHORT);                                      /* Skip mac-style               */
      pos+=sizeof(USHORT);                                      /* Skip lowest rec PPEM         */
      pos+=sizeof(SHORT);                                       /* Skip font direction hint     */

      indexToLocFormat=toSHORT(fontData[pos],fontData[pos+1]);  /* Get Index To Loc Format      */
                                       
      pos+=sizeof(SHORT);

      lastError = ERR_NoError;

   }
 

/*==============================================================================================*/
/*  TTFont::processMaximumProfileTable()                                             (PRIVATE)  */ 
/*==============================================================================================*/ 
/*                                                                                              */
/*  SYNTAX:       void processMaximumProfileTable();                                            */
/*                                                                                              */
/*  DESCRIPTION:  This function parses the font's Maximum Profile Table.  The following values  */
/*                are needed from this table (all others are ignored):                          */
/*                                                                                              */
/*                   Value            Data Type    Description                                  */
/*                ---------------------------------------------------------------------------   */
/*                   numGlyphs         USHORT      The number of glyphs in the font.            */
/*                                                                                              */
/*  NOTES:        This function assumes that the font data has already been read in (by         */
/*                'readFontData()').                                                            */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_TableNotFound.........The Maximum Profile Table is not present in this */
/*                                             font.                                            */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::processMaximumProfileTable()
   {

      ULONG pos;
      ULONG tag,checkSum,offset,length;       

      tag = toULONG((BYTE)'m',(BYTE)'a',(BYTE)'x',(BYTE)'p');   /* Get info about this table    */
      getTableDirEntry(tag,&checkSum,&offset,&length);
      if (lastError != ERR_NoError) return;

      pos = offset;                                             /* Go to the beginning of table */
      pos+=sizeof(Fixed);                                       /* Skip Table Version number    */

      numGlyphs = toUSHORT(fontData[pos],fontData[pos+1]);      /* Get the number of glyphs     */
                                                                /*   in this font               */
      lastError = ERR_NoError;

   }


/*==============================================================================================*/
/*  TTFont::processNamingTable()                                                     (PRIVATE)  */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void processNamingTable();                                                    */
/*                                                                                              */
/*  DESCRIPTION:  This function parses the font's Naming Table.  The following values           */
/*                are needed from this table (all others are ignored):                          */
/*                                                                                              */
/*                   Value            Data Type    Description                                  */
/*                ---------------------------------------------------------------------------   */
/*                   copyright         CHARPTR     The font's copyright notice.                 */
/*                   familyName        CHARPTR     The font's family name.                      */
/*                   subfamilyName     CHARPTR     The font's subfamily name.                   */
/*                   uniqueName        CHARPTR     A unique identifier for this font.           */
/*                   fullName          CHARPTR     The font's full name (a combination of       */
/*                                                 familyName and subfamilyName).               */
/*                   versionName       CHARPTR     The font's version string.                   */
/*                                                                                              */
/*  NOTES:        This function assumes that the font data has already been read in (by         */
/*                'readFontData()').                                                            */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_TableNotFound.........The Naming Table is not present in this font.    */
/*                   ERR_OutOfMemory...........An attempt to allocate memory failed.            */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::processNamingTable()
   {

      INT      i,c;                                   /* Counters                               */
      ULONG    pos;
      ULONG    tag,checkSum,offset,length;

      USHORT   numNameRecords;                        /* Number of name records in this table   */

      BYTE     nameFound;                             /* TRUE if a valid NameID has been found  */
      CHARPTR* stringToFill;                          /* Points to a CHAR string we are filling */

      ULONG    stringStorageOffset;                   /* These are used when looking through    */
      USHORT   thisPlatformID;                        /*  the name records                      */
      USHORT   thisSpecificID;
      USHORT   thisLanguageID;
      USHORT   thisNameID;
      USHORT   thisStringLength;
      USHORT   thisStringOffset;

      tag = toULONG((BYTE)'n',(BYTE)'a',(BYTE)'m',(BYTE)'e');   /* Get info about this table    */
      getTableDirEntry(tag,&checkSum,&offset,&length);
      if(lastError != ERR_NoError) return;

      pos = offset;                                             /* Go to the beginning of table */

      pos+=sizeof(USHORT);                                      /* Skip Format Selector         */

      numNameRecords = toUSHORT(fontData[pos],fontData[pos+1]); /* Get number of Name Records   */

      pos+=sizeof(USHORT);

      stringStorageOffset = toUSHORT(fontData[pos],
                                     fontData[pos+1]);          /* Get offset from beginning of */
      pos+=sizeof(USHORT);                                      /* table to string storage area */
      stringStorageOffset+=offset;                              /* Make this a file offset      */

      /*____ Search through all the Name Records, ignoring any that aren't for ____*/
      /*____ our particular platform or language.                              ____*/

      for (i=0;i<numNameRecords;i++)
      {
         thisPlatformID   = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);
         thisSpecificID   = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);
         thisLanguageID   = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);
         thisNameID       = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);
         thisStringLength = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);
         thisStringOffset = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);

         if (   (thisPlatformID != platformID)
              ||(thisSpecificID != specificID)
              ||(thisLanguageID != languageID) ) continue;

        /*____ See if this Name Record is one we need ____*/

         nameFound=FALSE;
         switch(thisNameID)
         {
            case NID_Copyright: stringToFill = &copyright;
                                nameFound=TRUE;
                                break;
            case NID_Family:    stringToFill = &familyName;
                                nameFound=TRUE;
                                break;
            case NID_Subfamily: stringToFill = &subfamilyName;
                                nameFound=TRUE;
                                break;
            case NID_UniqueID:  stringToFill = &uniqueName;
                                nameFound=TRUE;
                                break;
            case NID_FullName:  stringToFill = &fullName;
                                nameFound=TRUE;
                                break;
            case NID_Version:   stringToFill = &versionName;
                                nameFound=TRUE;
                                break;
         }

        /*____ If it is, 'stringToFill' will be pointing to a character string ____*/
        /*____ that we need to fill.  Allocate memory for it, and copy the     ____*/
        /*____ information from the string data in the table to our character  ____*/
        /*____ string.  Microsoft platforms use double-byte characters, all    ____*/
        /*____ others use single-byte characters.                              ____*/

         if(nameFound)
         {
            if (thisPlatformID==PID_Microsoft)
            {
               *stringToFill = new CHAR[thisStringLength/2+1];
               if (*stringToFill==NULL)
               {                                   
                  lastError = ERR_OutOfMemory;
                  return;
               }
               for(c=1;c<thisStringLength;c+=2)
                  (*stringToFill)[c/2]=fontData[stringStorageOffset+thisStringOffset+c];
               (*stringToFill)[c/2]=0;
            }
            else
            {
               *stringToFill = new CHAR[thisStringLength+1];
               if (*stringToFill==NULL)
               {                                   
                  lastError = ERR_OutOfMemory;
                  return;
               }
               for(c=0;c<thisStringLength;c++)
                  (*stringToFill)[c]=fontData[stringStorageOffset+thisStringOffset+c];
               (*stringToFill)[c]=0;
            }

         }
          
      }

      lastError = ERR_NoError;

   }


/*==============================================================================================*/
/*  TTFont::processIndexToLocationTable()                                            (PRIVATE)  */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void TTFont::processIndexToLocationTable();                                   */
/*                                                                                              */
/*  DESCRIPTION:  This function parses the font's Index To Location Table. The following values */
/*                are needed from this table (all others are ignored):                          */
/*                                                                                              */
/*                   Value            Data Type         Description                             */
/*                ---------------------------------------------------------------------------   */
/*                   glyphOffset    ULONG[numGlyphs]   An array that contains each glyph's      */
/*                                                     offset into the Glyph Data Table.        */
/*                                                                                              */
/*  NOTES:        This function assumes that the font data has already been read in (by         */
/*                'readFontData()'), and that the Header Table has already been                 */
/*                processed.                                                                    */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_TableNotFound.........The Index To Location Table is not present in    */
/*                                             this font.                                       */
/*                   ERR_OutOfMemory...........An attempt to allocate memory failed.            */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::processIndexToLocationTable()
   {

      INT      i;                               /* Counter                                      */
                                                                                                  
      ULONG    pos;
      ULONG    tag,checkSum,offset,length;      /* This table's length                          */
      ULONG    thisOffset;                      /* A glyph's offset into the Glyph Data Table   */


      tag = toULONG((BYTE)'l',(BYTE)'o',(BYTE)'c',(BYTE)'a');   /* Get info about this table    */
      getTableDirEntry(tag,&checkSum,&offset,&length);
      if(lastError != ERR_NoError) return;

      pos = offset;                                             /* Go to beginning of table     */
                                                                                                  
      glyphOffsetArray = new ULONG[numGlyphs+1];

      for(i=0;i<numGlyphs+1;i++)                                /* There are numGlyphs+1        */
      {                                                         /*  entries in this table       */
         if (indexToLocFormat==0)                               /* Format0: offset/2 is stored  */
         {
            thisOffset=(ULONG)toUSHORT(fontData[pos],fontData[pos+1]);
            thisOffset*=2;
            pos+=sizeof(USHORT);
         }
         else                                                   /* Format1: store actual offset */
         {
            thisOffset=toULONG(fontData[pos],fontData[pos+1],fontData[pos+2],fontData[pos+3]);
            pos+=sizeof(ULONG);
         }
         glyphOffsetArray[i]=thisOffset;
      }

      lastError = ERR_NoError;

   }


/*==============================================================================================*/
/*  TTFont::processHorizontalHeaderTable()                                           (PRIVATE)  */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void TTFont::processHorizontalHeaderTable();                                  */
/*                                                                                              */
/*  DESCRIPTION:  This function parses the font's Horizontal Header Table. The following values */
/*                are needed from this table (all others are ignored):                          */
/*                                                                                              */
/*                   Value            Data Type         Description                             */
/*                ---------------------------------------------------------------------------   */
/*                   ascender           SHORT          Typographic ascent.                      */
/*                   descender          SHORT          Typographic descent.                     */
/*                   lineGap            SHORT          Typographic lineGap.                     */
/*                   numberOfHMetrics   USHORT         Number hMetric entries in the HTMX       */
/*                                                     Table; may be smaller than the total     */
/*                                                     number of glyphs.                        */
/*                                                                                              */
/*  NOTES:        This function assumes that the font data has already been read in (by         */
/*                'readFontData()').                                                            */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_TableNotFound.........The Horizontal Header Table is not present in    */
/*                                             this font.                                       */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::processHorizontalHeaderTable()
   {

      ULONG    pos;                                   /* Placekeeper                            */
      ULONG    tag,checkSum,offset,length;            /* Table information                      */

      tag = toULONG((BYTE)'h',(BYTE)'h',(BYTE)'e',(BYTE)'a');       /* Get info about table     */
      getTableDirEntry(tag,&checkSum,&offset,&length);
      if(lastError != ERR_NoError) return;

      pos = offset;

      pos+=sizeof(ULONG);                                           /* Skip table version       */

      ascender = toSHORT(fontData[pos],fontData[pos+1]);            /* Typographic ascent       */
      pos+=sizeof(SHORT);

      descender = toSHORT(fontData[pos],fontData[pos+1]);           /* Typographic descent      */
      pos+=sizeof(SHORT);

      lineGap = toSHORT(fontData[pos],fontData[pos+1]);             /* Typographic line gap     */
      pos+=sizeof(SHORT);

      pos+=sizeof(USHORT);                                          /* Skip advanceWidthMax     */
      pos+=sizeof(SHORT);                                           /* Skip minLeftSideBearing  */
      pos+=sizeof(SHORT);                                           /* Skip minRightSideBearing */
      pos+=sizeof(SHORT);                                           /* Skip xMaxExtent          */
      pos+=sizeof(SHORT);                                           /* Skip caretSlopeRise      */
      pos+=sizeof(SHORT);                                           /* Skip caretSlopeRun       */
      pos+=5*sizeof(SHORT);                                         /* Skip 5 reserved SHORTs   */
      pos+=sizeof(SHORT);                                           /* Skip metricDataFormat    */

      numberOfHMetrics = toUSHORT(fontData[pos],fontData[pos+1]);   /* Number of hMetrics       */
                                                                    /*   entries in 'hmtx'      */
      lastError = ERR_NoError;

   }


/*==============================================================================================*/
/*  TTFont::processHorizontalMetricsTable()                                          (PRIVATE)  */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void TTFont::processHorizontalMetricsTable();                                 */
/*                                                                                              */
/*  DESCRIPTION:  This function extracts the advance width, left side bearing, and right        */
/*                side bearing for each glyph from the Horizontal Metrics Table.                */
/*                                                                                              */
/*  NOTES:        This function assumes that the following tables have already been read        */
/*                in:  HEAD, HHEA, HMTX, GLYF.                                                  */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_TableNotFound.........The Horizonta Metrics Table is not present in    */
/*                                             this font.                                       */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::processHorizontalMetricsTable()
   {

      INT      i;
      ULONG    pos;                                   /* Placekeeper                            */
      ULONG    tag,checkSum,offset,length;            /* Table information                      */
      SHORT    lastAdvanceWidth;

      tag = toULONG((BYTE)'h',(BYTE)'m',(BYTE)'t',(BYTE)'x');       /* Get info about table     */
      getTableDirEntry(tag,&checkSum,&offset,&length);
      if(lastError != ERR_NoError) return;

      pos=offset;

     /*____ In this table are the AW and LSB for each glyph.  At the beginning will be ____*/
     /*____ 'numberOfHMetrics' (found in the 'hhea' table) pairs of these values, fol- ____*/
     /*____ lowed by numGlyphs-numberOfHMetrics single LSB values.  The AW for these   ____*/
     /*____ last LSBs is the same as the last AW in the first seqence (of pairs).      ____*/ 

      for (i=0;i<numberOfHMetrics;i++)
      {
         glyph[i].advanceWidth = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);
         glyph[i].leftSideBearing = toSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(SHORT);
         glyph[i].rightSideBearing =  glyph[i].advanceWidth
                                    - glyph[i].leftSideBearing
                                    - glyph[i].xMax
                                    + glyph[i].xMin;
      }

      if (i==numGlyphs) return;

      lastAdvanceWidth = glyph[i].advanceWidth;

      for (;i<numGlyphs;i++)
      {
         glyph[i].advanceWidth = lastAdvanceWidth;
         glyph[i].leftSideBearing = toSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(SHORT);
         glyph[i].rightSideBearing =  glyph[i].advanceWidth
                                    - glyph[i].leftSideBearing
                                    - glyph[i].xMax
                                    + glyph[i].xMin;
      }

      lastError = ERR_NoError;

   }


/*==============================================================================================*/
/*  TTFont::processKerningTable()                                                    (PRIVATE)  */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void TTFont::processKerningTable();                                           */
/*                                                                                              */
/*  DESCRIPTION:  This function extracts the kerning information for pairs of glyphs.           */
/*                An array of 'TTKernPair's are created, and filled.                            */
/*                                                                                              */
/*  NOTES:        This function assumes that the font data has already been read in (by         */
/*                'readFontData()').                                                            */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_TableNotFound.........The Horizonta Metrics Table is not present in    */
/*                                             this font.                                       */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::processKerningTable()
   {

      INT      i;
      ULONG    pos;                                   /* Placekeeper                            */
      ULONG    tag,checkSum,offset,length;            /* Table information                      */
      BYTE     coverageHi, coverageLo;

      tag = toULONG((BYTE)'k',(BYTE)'e',(BYTE)'r',(BYTE)'n');       /* Get info about table     */
      getTableDirEntry(tag,&checkSum,&offset,&length);
      if(lastError != ERR_NoError) return;

      pos = offset;

      pos+=sizeof(USHORT);                            /* Skip table version number              */
      pos+=sizeof(USHORT);                            /* Skip number of subtables (we only look */
                                                      /*   at the first one anyway              */
      pos+=sizeof(USHORT);                            /* Skip subtable number                   */
      pos+=sizeof(USHORT);                            /* Skip length of subtable                */

      coverageHi = fontData[pos];                     /* Get Hi and Lo bytes of coverage flags  */
      coverageLo = fontData[pos+1];
      pos+=sizeof(USHORT);

      if ((isBitSet(coverageLo,1)) || (coverageHi!=0))   /* Make sure it is a kerning table,    */
      {                                                  /*   and format type 0.                */
         lastError = ERR_UnknownKernFormat;
         return;
      }

      numKernPairs=toUSHORT(fontData[pos],fontData[pos+1]);    /* Get number of kern pairs      */
      pos+=sizeof(USHORT);

      pos+=sizeof(USHORT);                                     /* Skip search range             */
      pos+=sizeof(USHORT);                                     /* Skip entry selector           */
      pos+=sizeof(USHORT);                                     /* Skip range shift              */

      kernPair = new TTKernPair[numKernPairs];        /* Allocate the kern pair array           */
      if (kernPair==NULL)
      {
         lastError = ERR_OutOfMemory;
         return;
      }

      for (i=0;i<numKernPairs;i++)                    /* Read the kerning information           */
      {
         kernPair[i].left = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);
         kernPair[i].right = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);
         kernPair[i].value = toSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(SHORT);
      }

      lastError = ERR_NoError;

   }


/*==============================================================================================*/
/*  TTFont::processCharacterMappingTable()                                           (PRIVATE)  */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void TTFont::processCharacterMappingTable();                                  */
/*                                                                                              */
/*  DESCRIPTION:  This function places the font's Character To Glyph Index Mapping Table into   */
/*                an array of BYTEs.  If an encoding with the correct PlatformID and            */
/*                EncodingID are not found, then 'TTFont::cmap' (the BYTE array) will remain    */
/*                empty.                                                                        */
/*                                                                                              */
/*  NOTES:        This function assumes that the font data has already been read in (by         */
/*                'readFontData()').                                                            */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_TableNotFound.........The Character Mapping Table is not present in    */
/*                                             this font.                                       */
/*                   ERR_OutOfMemory...........An attempt to allocate memory failed.            */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::processCharacterMappingTable()
   {

      INT      i;                                     /* Counter                                */
      ULONG    pos;                                   /* Placekeeper                            */
      ULONG    tag,checkSum,offset,length;            /* Table information                      */
      USHORT   numSubtables;                          /* Number of encoding subtables           */
      USHORT   subtableFound;                         /* TRUE if a proper subtable is found.    */
      USHORT   thisPlatformID;                        /* Used when looking through subtables    */
      USHORT   thisSpecificID;                        /*   ...                                  */
      ULONG    thisSubtableOffset; /* USHORT */       /*   ...                                  */
      ULONG    subtableOffset;     /* USHORT */       /* Correct subtable's offset              */
      USHORT   subtableLength;                        /* Length of encoding subtable            */

      tag = toULONG((BYTE)'c',(BYTE)'m',(BYTE)'a',(BYTE)'p');   /* Get info about this table    */
      getTableDirEntry(tag,&checkSum,&offset,&length);
      if(lastError != ERR_NoError) return;

      pos = offset;                                             /* Go to the beginning of table */

      pos+=sizeof(USHORT);                                      /* Skip table version number    */
                                                                                                  
      numSubtables = toUSHORT(fontData[pos],fontData[pos+1]);   /* Get number of subtables      */
                             
      pos+=sizeof(USHORT);

      subtableFound=FALSE; i=0;                             /* Look through each subtable entry */
      while ((!subtableFound) && (i<numSubtables))          /*   for one of the proper platform */
      {                                                     /*   and specific ID.               */
         thisPlatformID = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);
         thisSpecificID = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);
         thisSubtableOffset = toULONG(fontData[pos],fontData[pos+1],
                                      fontData[pos+2],fontData[pos+3]);
         pos+=sizeof(ULONG);

         if (  (thisPlatformID==platformID)
             &&(thisSpecificID==specificID))                    /*  If we find it, remember     */

         {                                                      /*    the offset, and set       */
            subtableOffset = thisSubtableOffset;                /*    a flag.                   */
            subtableFound = TRUE;
         }
 
         i++;
      }

      if (!subtableFound)                                       /* If a correct subtable wasn't */
      {                                                         /*   found set the error condi- */
         lastError = ERR_NoCharMapFound;                        /*   tion and return.           */
         return;
      }
                                                                                                  
      pos = offset+subtableOffset;                              /* Go to beginning of subtable  */

      cmapFormat = toUSHORT(fontData[pos],fontData[pos+1]);     /* Get the format of subtable   */

      pos+=sizeof(USHORT);
      subtableLength = toUSHORT(fontData[pos],fontData[pos+1]); /* Get length of subtable       */

      pos+=sizeof(USHORT);

      if (  (cmapFormat!=CMAP_FORMAT0)
          &&(cmapFormat!=CMAP_FORMAT4))                  /* Make sure the subtable is of either */
      {                                                  /*   Format 0, or Format 4.  These are */
         lastError = ERR_UnknownCmapFormat;              /*   the only ones supported.          */
         return;
      }

      cmap = new BYTE[subtableLength];                   /* Read in the subtable data.          */
      for (i=0;i<subtableLength;i++)
          cmap[i]=(fontData)[offset+subtableOffset+i];

      lastError = ERR_NoError;

   }


/*==============================================================================================*/
/*  TTFont::getGlyphIndex()                                                          (PRIVATE)  */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void getGlyphIndex(USHORT c);                                                 */
/*                                                                                              */
/*  DESCRIPTION:  This function finds the glyph index of a particular character code.  It       */
/*                returns zero (the missing glyph) if the character code is not mapped to       */
/*                a glyph.                                                                      */
/*                                                                                              */
/*  NOTES:        This function assumes that the font data has already been read in (by         */
/*                'readFontData()'), and that the CMAP table has already been processed         */
/*                (by 'processCharacterMappingTable()').                                        */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_CmapNotPresent........The Character Mapping Table is not present in    */
/*                                             this font, or hasn't been loaded yet.            */
/*                                                                                              */
/*==============================================================================================*/

   USHORT TTFont::getGlyphIndex(USHORT c)
   {

      USHORT  segCount;                               /* Number of Format4 segments             */
      BYTE*   endCount;                               /* Start of Format4 endCount array        */
      BYTE*   startCount;                             /* Start of Format4 startCount array      */
      BYTE*   idDelta;                                /* Start of Format4 idDelta array         */
      BYTE*   idRangeOffset;                          /* Start of Format4 idRangeOffset array   */
      BYTE*   glyphIdArray;                           /* Start of Format4 glyphIdArray array    */

      ULONG    end,start,delta,range,index;           /* Once we find the correct Format4       */
      ULONG    seg;                                   /*  segment, these hold needed seg info   */

      lastError = ERR_NoError;                        /* Nothing bad has happened yet!          */

      if (cmap==NULL)                                 /* Make sure the character mapping table  */
      {                                               /*  has been loaded                       */
         lastError=ERR_CmapNotPresent;
         return (USHORT)0;
      }
      
      switch(cmapFormat)
      {

         case CMAP_FORMAT0:                           /* We have a Byte Encoding Table          */

            glyphIdArray = cmap+6;                    /* The glyph ID array starts at the sixth */
                                                      /*  byte in the cmap data array           */
            if (c<256)                                /* Only codes 0..255 can are mapped in    */
               return (USHORT)glyphIdArray[c];        /*  this format                           */
            else                                      /* Return the 'missing glyph' if code     */
               return (USHORT)0;                      /*  is not in valid range                 */

         case CMAP_FORMAT4:                           /* We have a Segment Mapping Table        */
                                                         
            segCount=toUSHORT(cmap[6],                /* Get number of segments                 */
                              cmap[7])/2;

            endCount      = &cmap[14];                /* Find beg. of end count array           */
            startCount    = &cmap[16+2*segCount];     /* Find beg. of start count array         */
            idDelta       = &cmap[16+4*segCount];     /* Find beg. of delta array               */
            idRangeOffset = &cmap[16+6*segCount];     /* Find beg. of range array               */
            glyphIdArray  = &cmap[16+8*segCount];     /* Find beg. of glyph id array            */

            seg=0;
            end = toUSHORT(endCount[0],endCount[1]);     /* Search the end count array for the  */
            while (end<c)                                /*  first entry >= the char code we're */
            {                                            /*  looking for.  Once found, we have  */
               seg++;                                    /*  the right segment.                 */
               end = toUSHORT(endCount[seg*2  ],
                              endCount[seg*2+1]);
            }

            start = toUSHORT(startCount[seg*2],          /* Once we find the right segment,     */
                             startCount[seg*2+1]);       /*  get the other segment info we need */
            delta = toUSHORT(idDelta[seg*2],
                             idDelta[seg*2+1]);
            range = toUSHORT(idRangeOffset[seg*2],
                             idRangeOffset[seg*2+1]);

            if (start>c)                                 /* If this segment's start code is     */
               return 0;                                 /*  greater than the char code, then   */
                                                         /*  char is NOT mapped.                */

            if (range==0)                                /* Check the TTF spec. for info        */
            {                                            /*  on the expressions used to get     */
               index = (USHORT)c + (USHORT)delta;        /*  the glyph index from the segment   */
            }                                            /*  information                        */
            else
            {
	         index = range + (c-start)*2 + ((ULONG)(16+6*segCount)+seg*2);
                 index = toUSHORT(cmap[index],cmap[index+1]);
		 if (index!=0) index = (USHORT)index + (USHORT)delta;
            }

            return (USHORT)index;

      }
      return (USHORT)0;

   }


/*==============================================================================================*/
/*  TTFont::processGlyphDataTable()                                                  (PRIVATE)  */
/*==============================================================================================*/
/*                                                                                              */
/*  DESCRIPTION:  This function finds the glyph index of a particular character code.  It       */
/*                returns zero (the missing glyph) if the character code is not mapped to       */
/*                a glyph.                                                                      */
/*                                                                                              */
/*  NOTES:        This function assumes that the font data has already been read in (by         */
/*                TTFont::readFontData()), and that the CMAP table has already been processed   */
/*                (by TTFont::processCharacterMappingTable()).                                  */
/*                                                                                              */
/*==============================================================================================*/

   void TTFont::processGlyphDataTable()
   {

      USHORT  gi,i,j,k;
      ULONG   pos;
      ULONG   tag,checkSum,offset,length;

      ULONG   currGlyphOffset;
      ULONG   nextGlyphOffset;
      ULONG   currGlyphLength;

      SHORT   numberOfContours;
      USHORT* endPtsOfContours;
      USHORT  numberOfPoints;
      USHORT  instructionLength;
      BYTE*   flags;
      USHORT  repeatcount;
      USHORT  startPoint;
      USHORT  endPoint;
      BYTE    currentFlag;
      SHORT   xByte;
      SHORT   yByte;
      SHORT   xWord;
      SHORT   yWord;
      USHORT  tx,ty;


      tag = toULONG((BYTE)'g',(BYTE)'l',(BYTE)'y',(BYTE)'f');   /* Get info about this table    */
      getTableDirEntry(tag,&checkSum,&offset,&length);
      if(lastError != ERR_NoError) return;


      glyph = new TTGlyph[numGlyphs];
      if (glyph==NULL)
      {
         lastError = ERR_OutOfMemory;
         return;
      }


      for(gi=0;gi<numGlyphs;gi++)
      {

     /*-----------------------------------------------------------------------------------------*/
     /* BEGIN GLYPH PROCESSING LOOP                                                             */
     /*-----------------------------------------------------------------------------------------*/

         currGlyphOffset = glyphOffsetArray[gi];
         nextGlyphOffset = glyphOffsetArray[gi+1];
         currGlyphLength = nextGlyphOffset-currGlyphOffset;

         if (currGlyphLength==0)
         {
            glyph[gi].numContours = 0;
            glyph[gi].xMin        = 0;
            glyph[gi].yMin        = 0;
            glyph[gi].xMax        = 0;
            glyph[gi].yMax        = 0;
            glyph[gi].contour     = NULL;
            continue;
         }

         pos = offset + currGlyphOffset;

         glyph[gi].numContours = toSHORT(fontData[pos],fontData[pos+1]); pos+=sizeof(SHORT);
         numberOfContours = glyph[gi].numContours;

         glyph[gi].xMin        = toSHORT(fontData[pos],fontData[pos+1]); pos+=sizeof(SHORT);
         glyph[gi].yMin        = toSHORT(fontData[pos],fontData[pos+1]); pos+=sizeof(SHORT);
         glyph[gi].xMax        = toSHORT(fontData[pos],fontData[pos+1]); pos+=sizeof(SHORT);
         glyph[gi].yMax        = toSHORT(fontData[pos],fontData[pos+1]); pos+=sizeof(SHORT);

         if (numberOfContours<0)
         {
            glyph[gi].numContours = 0;
            glyph[gi].contour     = NULL;
            continue;
         }

         endPtsOfContours = new USHORT[numberOfContours];
         if(endPtsOfContours==NULL)
         {
            lastError = ERR_OutOfMemory;
            return;
         }

         for (i=0;i<numberOfContours;i++)
         {
            endPtsOfContours[i]=toUSHORT(fontData[pos],fontData[pos+1]);
            pos+=sizeof(USHORT);
         }

         glyph[gi].contour = new TTContour[numberOfContours];
         if(glyph[gi].contour==NULL)
         {
            lastError = ERR_OutOfMemory;
            return;
         }

         numberOfPoints = endPtsOfContours[numberOfContours-1]+1;

         instructionLength = toUSHORT(fontData[pos],fontData[pos+1]);
         pos+=sizeof(USHORT);

         pos+=instructionLength;

         flags = new BYTE[numberOfPoints];
         if(flags==NULL)
         {
            lastError = ERR_OutOfMemory;
            return;
         }


        /* ____ Read in the flags for this glyph.  The outer loop gathers the flags that ____ */
        /* ____ are actually contained in the table.  If the repeat bit is set in a flag ____ */
        /* ____ then the next byte is read from the table; this is the number of times   ____ */
        /* ____ to repeat the last flag.  The inner loop does this, incrementing the     ____ */
        /* ____ outer loops index each time.                                             ____ */

         for (i=0;i<numberOfPoints;i++)
         {
            flags[i]=fontData[pos];
            pos++;
            if(isBitSet(flags[i],3))
            {
               repeatcount=fontData[pos];
               pos++;
               for(;repeatcount>0;repeatcount--)
               {
                  i++;
                  flags[i]=flags[i-1];
               }
            }
         }




        /* ____ Now come the x- and y-coordinate data.  The font file stores all coord-  ____ */
        /* ____ inates as relative to the previous one.  This is how we read them in     ____ */
        /* ____ initially; we will convert to absolute coords later.  The x- and y-      ____ */
        /* ____ coordinates come separately, so we will have to have separate loops for  ____ */
        /* ____ each one.  Each outer loop iteration will read in a contour, while the   ____ */
        /* ____ inner loop reads in each point.                                          ____ */


        /*------------------------------------------------------------------------------------*/
        /* GET X COORDINATE DATA                                                              */
        /*------------------------------------------------------------------------------------*/

         startPoint = 0; 
         for(i=0;i<numberOfContours;i++)
         {

            /* ____ Find the end point of this contour. ____ */

            endPoint = endPtsOfContours[i];

            /* ____ Store the number of points in this contour. ____ */

            glyph[gi].contour[i].numPoints = endPoint-startPoint+1;

            /* ____ Allocate space for the points in this contour. ____ */

            glyph[gi].contour[i].point = new TTPoint[endPoint-startPoint+1];

            if(glyph[gi].contour[i].point==NULL)
            {
               lastError = ERR_OutOfMemory;
               return;
            }

            /* ____ Get each point in this contour.  Coordinates in the font file are     ____ */
            /* ____ indexed according to position in the entire glyph, while the          ____ */
            /* ____ coordinates stored are indexed according to their position in the     ____ */
            /* ____ current contour.  'startPoint' and 'endPoint' are glyph indexes, and  ____ */
            /* ____ j is the index into the current contour.                              ____ */

            /* ____ Process X-Coordinates. ____ */

            for(j=startPoint;j<=endPoint;j++)
            {
               /* ____ Get the current flag. ____ */

               currentFlag = flags[j];

               /* ____ If bit zero in the flag is set then this point is an on-curve      ____ */
               /* ____ point, if not, then it is an off-curve point.                      ____ */

               if(isBitSet(currentFlag,0))
                  glyph[gi].contour[i].point[j-startPoint].type = ON_CURVE;
               else
                  glyph[gi].contour[i].point[j-startPoint].type = OFF_CURVE;

               /* ____ First we check to see if bit one is set.  This would indicate that ____ */
               /* ____ the corresponding coordinate data in the table is 1 byte long.     ____ */
               /* ____ If the bit is not set, then the coordinate data is 2 bytes long.   ____ */

               if(isBitSet(currentFlag,1))
               {
                  /* ____ Read in one byte. ____ */

                  xByte = fontData[pos];
                  pos++;

                  /* ____ In this case, bit four is the sign of the byte just read in. ____ */

                  if(isBitSet(currentFlag,4))
                     glyph[gi].contour[i].point[j-startPoint].x = xByte;
                  else
                     glyph[gi].contour[i].point[j-startPoint].x = -xByte;
               }
               else
               {
                  /* ____ If bit four is set, then this coordinate is the same as the     ____ */
                  /* ____ last one, so the relative offset (of zero) is stored.  If bit   ____ */
                  /* ____ is not set, then read in two bytes and store it as a signed     ____ */
                  /* ____ value.                                                          ____ */

                  if(isBitSet(currentFlag,4))
                     glyph[gi].contour[i].point[j-startPoint].x = 0;
                  else
                  {
                     xWord=toSHORT(fontData[pos],fontData[pos+1]);
                     pos+=sizeof(SHORT);
                     glyph[gi].contour[i].point[j-startPoint].x = xWord;
                  }
               }

            }

            /* ____ Make the new starting point the one after the current ending point.   ____ */

            startPoint=endPoint+1;

         }


        /*------------------------------------------------------------------------------------*/
        /* GET Y COORDINATE DATA                                                              */
        /*------------------------------------------------------------------------------------*/

         startPoint = 0;
         for(i=0;i<numberOfContours;i++)
         {
            /* ____ Find the end point of this contour. ____ */

            endPoint = endPtsOfContours[i];

            /* ____ Get each point in this contour.  Coordinates in the font file are     ____ */
            /* ____ indexed according to position in the entire glyph, while the          ____ */
            /* ____ coordinates stored are indexed according to their position in the     ____ */
            /* ____ current contour.  'startPoint' and 'endPoint' are glyph indexes, and  ____ */
            /* ____ j is the index into the current contour.                              ____ */

            /* ____ Process Y-Coordinates. ____ */

            for(j=startPoint;j<=endPoint;j++)
            {
               /* ____ Get the current flag.                                              ____ */

               currentFlag = flags[j];

               /* ____ First we check to see if bit two is set.  This would indicate that ____ */
               /* ____ the corresponding coordinate data in the table is 1 byte long.     ____ */
               /* ____ If the bit is not set, then the coordinate data is 2 bytes long.   ____ */

               if(isBitSet(currentFlag,2))
               {
                  /* ____ Read in one byte. ____ */

                  yByte = fontData[pos];
                  pos++;

                  /* ____ In this case, bit five is the sign of the byte just read in. ____ */

                  if(isBitSet(currentFlag,5))
                     glyph[gi].contour[i].point[j-startPoint].y = yByte;
                  else
                     glyph[gi].contour[i].point[j-startPoint].y = -yByte;
               }
               else
               {
                  /* ____ If bit five is set, then this coordinate is the same as the     ____ */
                  /* ____ last one, so the relative offset (of zero) is stored.  If bit   ____ */
                  /* ____ is not set, then read in two bytes and store it as a signed     ____ */
                  /* ____ value.                                                          ____ */

                  if(isBitSet(currentFlag,5))
                     glyph[gi].contour[i].point[j-startPoint].y = 0;
                  else
                  {
                     yWord=toSHORT(fontData[pos],fontData[pos+1]);
                     pos+=sizeof(SHORT);
                     glyph[gi].contour[i].point[j-startPoint].y = yWord;
                  }
               }

            }

            /* ____ Make the new starting point the one after the current ending point.   ____ */

            startPoint = endPoint+1;

         }

         delete endPtsOfContours;
         delete flags;

      }

     /*-----------------------------------------------------------------------------------------*/
     /* END GLYPH PROCESSING LOOP                                                               */
     /*-----------------------------------------------------------------------------------------*/


      for(i=0;i<numGlyphs;i++)                        /* Now make all glyph coordinates         */
      {                                               /*   absolute, instead of relative to     */
         tx=0;ty=0;                                   /*   the first point in the glyph.        */
         if(glyph[i].numContours>0)
         {
            for(j=0;j<glyph[i].numContours;j++)
            {
               for(k=0;k<glyph[i].contour[j].numPoints;k++)
               {
                  tx+=glyph[i].contour[j].point[k].x;
                  ty+=glyph[i].contour[j].point[k].y;
                  glyph[i].contour[j].point[k].x=tx;
                  glyph[i].contour[j].point[k].y=ty;
               }
            }
         }
      }

      lastError = ERR_NoError;

   }


/*==============================================================================================*/
/*  TTFont::TTFont()                                                                 (PUBLIC)   */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       TTFont::TTFont(CHARPTR inputFileName, CHARPTR inputPathName,                  */
/*                               USHORT _platformID,                                            */
/*                               USHORT _specificID, USHORT _languageID);                       */
/*                                                                                              */
/*  DESCRIPTION:  This is the constructor for the TTFont class.  It takes a pointer to a        */
/*                character string 'inputFileName' that contains the drive, path, and file-     */
/*                name of the TrueType font to create.  The last three paramters specify        */
/*                the platform and language to look for when multiple character encodings       */
/*                are present.  See the header file 'TrueType.H' for a list of defined          */
/*                platform IDs, specific encoding IDs, and language ID's.  Normally, the        */
/*                PID and SID are either 3 and 1 (for Microsoft Fonts), or 1 and 0 (for         */
/*                Macintosh Fonts).  You can make sure construction was successful by           */
/*                calling TTFont::LastError() to check the current error condition.  A          */
/*                return of ERR_NoError indicates success.                                      */
/*                                                                                              */
/*  ERRORS:       Possible error conditions upon return from this function are:                 */
/*                                                                                              */
/*                   ERR_NoError...............Successful completion.                           */
/*                   ERR_OutOfMemory...........An attempt at memory allocation failed.          */
/*                   ERR_UnableToOpenFile......An attempt to open the font file failed.         */
/*                   ERR_TableNotFound.........One of the required tables in this font          */
/*                                             was not found.                                   */
/*                   ERR_UnknownCmapFormat.....An invalid or unsupported CMAP table format      */
/*                                             was encountered.                                 */
/*                   ERR_UnknownKernFormat.....An invalid or unsupported KERN table format      */
/*                                             was encountered.                                 */
/*                                                                                              */
/*==============================================================================================*/

   TTFont::TTFont(CHARPTR inputFileName, CHARPTR inputPathName, 
                  USHORT _platformID, USHORT _specificID, USHORT _languageID)
   {
      fontDataSize     = 0;
      fontData         = NULL;
      cmap             = NULL;
      cmapFormat       = 0;
      numTables        = 0;
      indexToLocFormat = 0;
      numGlyphs        = 0;
      glyphOffsetArray = NULL;
      revision         = 0.0;
      copyright        = NULL;
      familyName       = NULL;
      fullName         = NULL;
      subfamilyName    = NULL;
      uniqueName       = NULL;
      versionName      = NULL;
      xMax             = 0;
      xMin             = 0;
      yMax             = 0;
      yMin             = 0;
      unitsPerEm       = 0;
      ascender         = 0;
      descender        = 0;
      lineGap          = 0;
      numKernPairs     = 0;
      kernPair         = NULL;
      platformID       = _platformID;
      specificID       = _specificID;
      languageID       = _languageID;
      lastError        = ERR_NoError;

      readFontData(inputFileName, inputPathName);

      if (lastError==ERR_NoError) 
         processFontHeaderTable();
      else
         return;

      if (lastError==ERR_NoError) 
         processMaximumProfileTable();
      else
         return;

      if (lastError==ERR_NoError) 
         processNamingTable();
      else
         return;

      if (lastError==ERR_NoError) 
         processIndexToLocationTable();
      else
         return;

      if (lastError==ERR_NoError) 
         processCharacterMappingTable();
      else
         return;

      if (lastError==ERR_NoError) 
         processGlyphDataTable();
      else
         return;

      if (lastError==ERR_NoError) 
         processHorizontalHeaderTable();
      else
         return;

      if (lastError==ERR_NoError) 
         processHorizontalMetricsTable();
      else
         return;

      if (lastError==ERR_NoError) 
         processKerningTable();
      else
         return;

      if (lastError!=ERR_NoError)       // KERN is the only table we don't care about.
	 lastError = ERR_NoError;       //   If it was there, fine, but don't report an
                                        //   error if it isn't.

      if (fontData!=NULL) delete fontData;

   }


/*==============================================================================================*/
/*  TTFont::Kerning()                                                                (PUBLIC)   */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       void TTFont::Kerning(void);                                                   */
/*                                                                                              */
/*  DESCRIPTION:  This function returns the kerning value for a particular pair of glyphs.      */
/*                If no kerning value has been defined, zero is returned.                       */
/*                                                                                              */
/*==============================================================================================*/

   SHORT TTFont::Kerning(USHORT idx1, USHORT idx2)
   {
      LONG   beg,mid,end;
      USHORT thisLeft, thisRight;
      ULONG  thisCombined;
      USHORT found;
      ULONG  combined = (ULONG)idx1*65536L + idx2;

      if (numKernPairs==0) return 0;

      beg = 0; end = numKernPairs;

      found=FALSE;
      while (beg<=end && found==FALSE)
      {
         mid = (end+beg)/2;

         thisLeft = kernPair[mid].left;
         thisRight = kernPair[mid].right;

         thisCombined = (ULONG)thisLeft*65536L + thisRight;
         if (combined == thisCombined)
         {
            found=TRUE;
            break;
         }

         if (combined < thisCombined)
            end = mid-1;
         else
            beg = mid+1;

      }

      if (found==TRUE)
         return kernPair[mid].value;
      else
         return 0;

   }



/*==============================================================================================*/
/*  TTFont::Glyph()                                                                  (PUBLIC)   */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       TTGlyph* TTFont::Glyph(USHORT code);                                          */
/*                                                                                              */
/*  DESCRIPTION:  This function returns a pointer to the glyph that is mapped to a particular   */
/*                character code.  If no glyph is mapped, then a pointer to the 'missing        */
/*                glyph' is returned (this is a special glyph, present in most fonts, used      */
/*                to indicate a missing glyph).                                                 */
/*                                                                                              */
/*==============================================================================================*/

   TTGlyph* TTFont::Glyph(USHORT code)
   {
      if (glyph==NULL)
         return NULL;
      else
         return &(glyph[getGlyphIndex(code)]);
   }







   TTFont::TTFont(CHARPTR inputFileName, CHARPTR inputPathName, INT mapType)
   {

      switch (mapType)
      {
         case MICROSOFT: platformID = PID_Microsoft;
                         specificID = SID_MS_UGL;
                         languageID = LID_MS_USEnglish;
                         break;

         case MACINTOSH: platformID = PID_Macintosh;
                         specificID = SID_MAC_Roman;
                         languageID = LID_MAC_English;
                         break;
      }

      fontDataSize     = 0;
      fontData         = NULL;
      cmap             = NULL;
      cmapFormat       = 0;
      numTables        = 0;
      indexToLocFormat = 0;
      numGlyphs        = 0;
      glyphOffsetArray = NULL;
      revision         = 0.0;
      copyright        = NULL;
      familyName       = NULL;
      fullName         = NULL;
      subfamilyName    = NULL;
      uniqueName       = NULL;
      versionName      = NULL;
      xMax             = 0;
      xMin             = 0;
      yMax             = 0;
      yMin             = 0;
      unitsPerEm       = 0;
      ascender         = 0;
      descender        = 0;
      lineGap          = 0;
      numKernPairs     = 0;
      kernPair         = NULL;
      lastError        = ERR_NoError;

      readFontData(inputFileName, inputPathName);

      if (lastError==ERR_NoError) processFontHeaderTable();        
      if (lastError==ERR_NoError) processMaximumProfileTable();
      if (lastError==ERR_NoError) processNamingTable();
      if (lastError==ERR_NoError) processIndexToLocationTable();
      if (lastError==ERR_NoError) processCharacterMappingTable();
      if (lastError==ERR_NoError) processGlyphDataTable();
      if (lastError==ERR_NoError) processHorizontalHeaderTable();
      if (lastError==ERR_NoError) processHorizontalMetricsTable();
      if (lastError==ERR_NoError) processKerningTable();

      if (fontData!=NULL) delete fontData;

   }
