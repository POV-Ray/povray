/* ---------------------------------------------------------------------------
*  FONT3D.CC
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

#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "geometry.h"
#include "truetype.h"
#include "font3d.h"

#define MAX_FILENAME_SIZE 1024

/*==============================================================================================*/
/*  main()                                                                                      */
/*==============================================================================================*/

extern "C" void message(char *String,...);
extern "C" void f_erreur(char *String,...);

extern "C" {
int font_3d(int argc, char* argv[]) {
   int           errorCode;
   int           errorPos;
   int           success;
   Font3DOptions options;
   USHORT        pid,sid,lid;

   int           numFileArgs;
   char**        fileArgs;

   char          fileName[MAX_FILENAME_SIZE];

   // PrintGreeting();

   success = ReadOptions("truetype.cmd", numFileArgs, fileArgs);

   if (success==ERR_OutOfMemory) {
      f_erreur("Plus de m‚moire.|Augmentez la taille de la m‚moire virtuelle.");
      return 1;
   }
   else if (success!=ERR_UnableToOpenFile)
   {
      success = ParseOptions(numFileArgs,fileArgs,options,errorCode,errorPos);

      for (int i=0;i<numFileArgs;i++) delete fileArgs[i];
      delete fileArgs;
   }

   if (argc>1) {
      success = ParseOptions(argc-1,&argv[1],options,errorCode,errorPos);
   }

   if (options.mapType==PID_Microsoft) {
      pid = PID_Microsoft;
      sid = SID_MS_UGL;
      lid = LID_MS_USEnglish;
   } else {
      pid = PID_Macintosh;
      sid = SID_MAC_Roman;
      lid = LID_MAC_English;
   }

   TTFont UserFont(options.fontFileName,options.fontPathName,pid,sid,lid);

   switch(UserFont.LastError()) {
      case ERR_NoError:
         break;

      case ERR_OutOfMemory:
         f_erreur("No more memory.|Please add more virtual memory.");
         return 1;

      case ERR_UnableToOpenFile:
         f_erreur("Unable to open TTF font file %s",options.fontFileName);
         return 1;

      case ERR_TableNotFound:
         f_erreur("Something is missing or bad in TT font.");
         return 1;

      case ERR_UnknownCmapFormat:
         f_erreur("Invalid or unsupported character coding method !|"\
                  "Change or check your font.");
         return 1;

      case ERR_UnknownKernFormat:
         f_erreur("Invalid error, check your TTF file");
         return 1;

      default:
         f_erreur("Unable to read TTF file.");
         return 1;
   }

   if (options.outputPathName==NULL) {
     strcpy(fileName, options.outputFileName);
   } else {
     sprintf(fileName,"%s/%s",options.outputPathName,options.outputFileName);
   }

   ofstream outputfile(fileName);

   if (!outputfile) {
      f_erreur("Unable to open tmpfile");
      return 1;
   }

   // if (options.outputFormat==RIB) PrintPixarCopyright();

   message("Generating font shape with %s, please wait...",UserFont.FullName());

   /*
   cout<<"   Output File:   "<<options.outputFileName<<endl;
   cout<<"   Object Name:   "<<options.objectName<<endl;
   cout<<endl;
   cout<<"   Please Wait..."<<endl;
   */

   outputfile<<"TRUETYPE\n";
   success = OutputObjects(outputfile,UserFont,options);

   switch (success) {
      case ERR_NoError:
         message("Font shape created !");
         break;
      case ERR_OutOfMemory:
         f_erreur("Out Of Memory.|"\
                  "Font shape will be probably corrupted.");
         break;
      case ERR_NoPolyFound:
          f_erreur("Can't polygonize this TTF file !|"\
                   "Font shape will be probably corrupted.");
         break;
      default:
         f_erreur("Fatal or unknown error.|"\
                  "Font shape will be probably corrupted.");
   }

   return success;

  }
};
