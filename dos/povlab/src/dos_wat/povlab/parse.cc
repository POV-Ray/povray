/* ---------------------------------------------------------------------------
*  PARSE.CC
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
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <fstream.h>

#include "config.h"
#include "font3d.h"

#define MAX_STRING_SIZE 1024
#define MAX_NUM_OPTIONS  200


/*==============================================================================================*/
/*  ReadOptions()                                                                               */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       INT  ReadOptions(char* fileName, int numArgs, char & * argument[]);           */
/*                                                                                              */
/*  DESCRIPTION:  This function processes a text file that contains a list of program           */
/*                options.  The options should be separated by a whitespace character,          */
/*                and will be stored in an array of strings 'argument' with their number        */
/*                in 'numArgs'.  Possible error conditions upon return from this function       */
/*                are:                                                                          */
/*                                                                                              */
/*                     Symbol                  Description                                      */
/*                 ------------------------------------------------------------------------     */
/*                     ERR_NoError.............The routine completed successfully.              */
/*                     ERR_OutOfMemory.........An attempt to allocate memory failed.            */
/*                     ERR_UnableToOpenFile....An attempt to open a file failed.                */
/*                                                                                              */
/*  RETURNS:      One of the defined error conditions in config.h.                              */
/*                                                                                              */
/*  NOTE:         Remember to delete 'argument' and the strings contained in it when you're     */
/*                done with it!                                                                 */
/*                                                                                              */
/*==============================================================================================*/

   int ReadOptions(char* fileName, int& numArgs, char ** &argument)
   {
      char currentString[MAX_STRING_SIZE+1];

      ifstream inputFile(fileName);
      if (!inputFile)
         return ERR_UnableToOpenFile;

      argument = new CHARPTR[MAX_NUM_OPTIONS];

      inputFile.width(MAX_STRING_SIZE);

      numArgs=0;

      do
      {
         inputFile>>currentString;
         if (!inputFile.fail())
         {
            argument[numArgs] = new char[strlen(currentString)+1];
            if (argument[numArgs]==NULL)
               return ERR_OutOfMemory;
            strcpy(argument[numArgs],currentString);
            numArgs++;
    
         }
      }
      while ((!inputFile.eof()) && (numArgs<MAX_NUM_OPTIONS));

      return ERR_NoError;
   }




/*==============================================================================================*/
/*  ParseOptions()                                                                              */
/*==============================================================================================*/
/*                                                                                              */
/*  SYNTAX:       INT  ParseOptions(int numArgs, CHAR* argument[],                              */
/*                                  Font3DOptions& defaultOptions,                              */
/*                                  USHORT& errorCode, USHORT& errorPos);                       */
/*                                                                                              */
/*  DESCRIPTION:  This function parses a list of option strings, storing any configuration      */
/*                information in a 'Font3DOptions' structure.  'numArgs' contains the           */
/*                number of option strings, 'argument[]' is the array of strings, and           */
/*                'defaultOptions' is the structure used to hold the information recovered      */
/*                by parsing the strings.  Two USHORT values 'errorCode' and 'errorPos'         */
/*                hold information about any errors that may have occurred.  'errorPos'         */
/*                reports the index of the string that caused the error, while 'errorCode'      */
/*                reports the type of error.  The following symbolic constants are defined      */
/*                in config.h:                                                                  */
/*                                                                                              */
/*                     Symbol           Description                                             */
/*                 ------------------------------------------------------------------------     */
/*                     ERR_NoError..........The routine completed successfully.                 */
/*                     ERR_OutOfMemory......An attempt to allocate memory failed.               */
/*                     ERR_NoOptionsFound...No option strings were found.                       */
/*                     ERR_InvalidOption....An invalid option was encountered.                  */
/*                                                                                              */
/*  RETURNS:      TRUE is returned if there were any option strings to process, and if they     */
/*                were all processed successfully; FALSE otherwise.                             */
/*                                                                                              */
/*  NOTES:        All option strings are assumed to be of the following form:                   */
/*                                                                                              */
/*                                    [-|/]optionname=optiondata                                */
/*                                                                                              */
/*==============================================================================================*/

   int ParseOptions(int numArgs, char* argument[], Font3DOptions& defaultOptions,
                    int& errorCode, int& errorPos)
   {
      char  tempChar;
      char* tempString;
      int   i,j,c;
      char* option;                          /* The entire option string                        */
      char* optionData;                      /* The data part of the option, after the '='      */
      int   optionDataLength;                /* The length of the data part of the option       */

      int    success;
      char*  configFileName;
      int    numConfigArgs;
      char** configArgs;
      int    configErrorCode;
      int    configErrorPos;

      for (i=0;i<numArgs;i++)
      {
         option = new CHAR[strlen(argument[i])+1];             /* Make a new character array to */
         if (option==NULL)                                     /*  hold the current option;     */
	 {                                                     /*  for in case we shouldn't     */
            errorCode = ERR_OutOfMemory;                       /*  modify the argument string   */
            errorPos = i;                                      /*  array.                       */
            return FALSE;
         }
         strcpy(option,argument[i]);
                                                               
         optionData = strchr(option,'=');                      /* Find the '=' char. If there   */
         if (optionData==NULL)                                 /*  isn't one, this is an inva-  */
         {                                                     /*  lid option.                  */
            errorCode = ERR_InvalidOption;
            errorPos  = i;
            return FALSE;
         }                                                     /* Now, replace the '=' with a   */
         *optionData = 0x00;                                   /*  null char so we can use the  */
         optionData++;                                         /*  string lib routines on both  */
                                                               /*  parts of the option string.  */

         optionDataLength = strlen(optionData);                /* Get the length of the data    */
                                                               /*  part of the option string    */
         if (optionDataLength==0)                              /*  (the part after the '=').    */
         {                                                     /*  If there is nothing there,   */
            errorCode = ERR_InvalidOption;                     /*  this is an invalid option.   */
            errorPos = i;
            return FALSE;
         }





     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   back-bevel                                                                    */
     /*-----------------------------------------------------------------------------------------*/

         if (strcmp(option,"ChanfreinArriŠre")==0)
         {
            if (strcmp(optionData,"1")==0)
               defaultOptions.backBevelVisible = TRUE;
            else if (strcmp(optionData,"0")==0)
               defaultOptions.backBevelVisible = FALSE;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   back-bevel-texture                                                            */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"back-bevel-texture")==0)
         {
            if (defaultOptions.backBevelTextureName!=NULL)
               delete defaultOptions.backBevelTextureName;

            defaultOptions.backBevelTextureName = new char[optionDataLength+1];
            if (defaultOptions.backBevelTextureName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }
            strcpy(defaultOptions.backBevelTextureName,optionData);
         }
         */
     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   back-face                                                                     */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"FaceArriŠre")==0)
         {
            if (strcmp(optionData,"1")==0)
               defaultOptions.backFaceVisible = TRUE;
            else if (strcmp(optionData,"0")==0)
               defaultOptions.backFaceVisible = FALSE;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   back-face-cut                                                                 */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"TailleChanfrein4")==0)
         {
            if (!sscanf(optionData,"%lf",&defaultOptions.backFaceCut))
	         {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   back-face-texture                                                             */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"back-face-texture")==0)
         {
            if (defaultOptions.backFaceTextureName!=NULL)
               delete defaultOptions.backFaceTextureName;

            defaultOptions.backFaceTextureName = new char[optionDataLength+1];
            if (defaultOptions.backFaceTextureName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }
            strcpy(defaultOptions.backFaceTextureName,optionData);
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   back-side-cut                                                                 */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"TailleChanfrein2")==0)
         {
            if (!sscanf(optionData,"%lf",&defaultOptions.backSideCut))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   bevel-texture                                                                 */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"bevel-texture")==0)
         {
            if (defaultOptions.frontBevelTextureName!=NULL)
               delete defaultOptions.frontBevelTextureName;
            if (defaultOptions.backBevelTextureName!=NULL)
               delete defaultOptions.backBevelTextureName;

            defaultOptions.frontBevelTextureName = new char[optionDataLength+1];
            defaultOptions.backBevelTextureName = new char[optionDataLength+1];

            if (   defaultOptions.frontBevelTextureName==NULL
                || defaultOptions.backBevelTextureName==NULL  )
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }

            strcpy(defaultOptions.frontBevelTextureName,optionData);
            strcpy(defaultOptions.backBevelTextureName,optionData);
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   bevel-type                                                                    */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"TypeChanfrein")==0)
         {
            if (strcmp(optionData,"ROUND")==0)
               defaultOptions.bevelType = ROUND;
            else if (strcmp(optionData,"PLAT")==0)
               defaultOptions.bevelType = FLAT;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   bevels                                                                        */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"Chanfrein")==0)
         {
            if (strcmp(optionData,"1")==0)
	    {
               defaultOptions.frontBevelVisible = TRUE;
               defaultOptions.backBevelVisible  = TRUE;
            }
            else if (strcmp(optionData,"0")==0)
	    {
               defaultOptions.frontBevelVisible = FALSE;
               defaultOptions.backBevelVisible  = FALSE;
	    }
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   char                                                                          */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"char")==0)
         {
            if (!sscanf(optionData,"%c",&tempChar))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
            else
               defaultOptions.c = tempChar;
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   code                                                                          */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"code")==0)
         {
            if (!sscanf(optionData,"%li",&defaultOptions.c))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   config                                                                        */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"config")==0)
	 {

            configFileName = new char[optionDataLength+1];

            if (configFileName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }

            strcpy(configFileName,optionData);

            success = ReadOptions(configFileName, numConfigArgs, configArgs);
            if (success==ERR_OutOfMemory)
	    {
               errorCode = ERR_OutOfMemory;
               errorPos = i;
               return FALSE;
	    }
            if (success==ERR_UnableToOpenFile) {
               cout<<endl;
               cout<<"   WARNING: Unable to open configuration file \""
                   <<configFileName<<"\"."<<endl;
               cout<<"            Ignoring..."<<endl<<endl;
	    }

            success = ParseOptions(numConfigArgs, configArgs, defaultOptions, 
                                   configErrorCode,configErrorPos);

            if (!success)
            {
               cout<<endl;
               cout<<"   ERROR: Invalid option \""<<configArgs[configErrorPos]
                   <<"\""<<endl;
               cout<<"          in configuration file \""<<configFileName
                   <<"\""<<endl<<endl;
               exit(1);
            }

            for (c=0;c<numConfigArgs;c++)
               delete configArgs[c];
            delete configArgs;

            delete configFileName;
            
	 }
         */
     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   cut                                                                           */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"cut")==0)
         {
            if (!sscanf(optionData,"%lf",&defaultOptions.frontFaceCut))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
            defaultOptions.backFaceCut = defaultOptions.frontFaceCut;
            defaultOptions.frontSideCut = defaultOptions.frontFaceCut;
            defaultOptions.backSideCut = defaultOptions.frontFaceCut;
         }
          */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   depth                                                                         */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"Profondeur")==0)
         {
            if (!sscanf(optionData,"%lf",&defaultOptions.depth))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   face-cut                                                                      */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"face-cut")==0)
         {
            if (!sscanf(optionData,"%lf",&defaultOptions.frontFaceCut))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
            defaultOptions.backFaceCut = defaultOptions.frontFaceCut;
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   face-texture                                                                  */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"face-texture")==0)
         {
            if (defaultOptions.frontFaceTextureName!=NULL)
               delete defaultOptions.frontFaceTextureName;
            if (defaultOptions.backFaceTextureName!=NULL)
               delete defaultOptions.backFaceTextureName;

            defaultOptions.frontFaceTextureName = new char[optionDataLength+1];
            defaultOptions.backFaceTextureName = new char[optionDataLength+1];

            if (   defaultOptions.frontFaceTextureName==NULL
                || defaultOptions.backFaceTextureName==NULL  )
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }

            strcpy(defaultOptions.frontFaceTextureName,optionData);
            strcpy(defaultOptions.backFaceTextureName,optionData);
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   faces                                                                         */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"Devant")==0)
         {
            if (strcmp(optionData,"1")==0)
	    {
               defaultOptions.frontFaceVisible = TRUE;
               defaultOptions.backFaceVisible  = TRUE;
            }
            else if (strcmp(optionData,"0")==0)
	    {
               defaultOptions.frontFaceVisible = FALSE;
               defaultOptions.backFaceVisible  = FALSE;
	    }
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   font                                                                          */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"Police")==0)
         {
            if (defaultOptions.fontFileName!=NULL)
            {
               delete defaultOptions.fontFileName;
            }

            defaultOptions.fontFileName = new char[optionDataLength+1];
            if (defaultOptions.fontFileName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }

            strcpy(defaultOptions.fontFileName,optionData);

         }

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   font-path                                                                     */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"font-path")==0)
         {
            if (defaultOptions.fontPathName!=NULL)
            {
               delete defaultOptions.fontPathName;
            }

            defaultOptions.fontPathName = new char[optionDataLength+1];
            if (defaultOptions.fontPathName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }
            strcpy(defaultOptions.fontPathName,optionData);
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   format                                                                        */
     /*-----------------------------------------------------------------------------------------*/
         else if (strcmp(option,"FormatSortie")==0)
         {
            if (strcmp(optionData,"RAW")==0)
              defaultOptions.outputFormat = RAW;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   front-bevel                                                                   */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"ChanfreinAvant")==0)
         {
            if (strcmp(optionData,"1")==0)
               defaultOptions.frontBevelVisible = TRUE;
            else if (strcmp(optionData,"0")==0)
               defaultOptions.frontBevelVisible = FALSE;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   front-bevel-texture                                                           */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"front-bevel-texture")==0)
         {
            if (defaultOptions.frontBevelTextureName!=NULL)
               delete defaultOptions.frontBevelTextureName;

            defaultOptions.frontBevelTextureName = new char[optionDataLength+1];
            if (defaultOptions.frontBevelTextureName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }
            strcpy(defaultOptions.frontBevelTextureName,optionData);
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   front-face                                                                    */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"FaceAvant")==0)
         {
            if (strcmp(optionData,"1")==0)
               defaultOptions.frontFaceVisible = TRUE;
            else if (strcmp(optionData,"0")==0)
               defaultOptions.frontFaceVisible = FALSE;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   front-face-cut                                                                */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"TailleChanfrein3")==0)
         {
            if (!sscanf(optionData,"%lf",&defaultOptions.frontFaceCut))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   front-face-texture                                                            */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"front-face-texture")==0)
         {
            if (defaultOptions.frontFaceTextureName!=NULL)
               delete defaultOptions.frontFaceTextureName;

            defaultOptions.frontFaceTextureName = new char[optionDataLength+1];
            if (defaultOptions.frontFaceTextureName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }
            strcpy(defaultOptions.frontFaceTextureName,optionData);
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   front-side-cut                                                                */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"TailleChanfrein1")==0)
         {
            if (!sscanf(optionData,"%lf",&defaultOptions.frontSideCut))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   map                                                                           */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"FormatTT")==0)
         {
            if (strcmp(optionData,"MC")==0)
               defaultOptions.mapType = MACINTOSH;
            else if (strcmp(optionData,"MS")==0)
               defaultOptions.mapType = MICROSOFT;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos  = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   name                                                                          */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"name")==0)
         {
            if (defaultOptions.objectName!=NULL)
               delete defaultOptions.objectName;

            defaultOptions.objectName = new char[optionDataLength+1];
            if (defaultOptions.objectName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos = i;
               return FALSE;
            }

            strcpy(defaultOptions.objectName,optionData);
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   output                                                                        */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"Sortie")==0)
         {
            if (defaultOptions.outputFileName!=NULL)
               delete defaultOptions.outputFileName;

            defaultOptions.outputFileName = new char[optionDataLength+1];
            if (defaultOptions.outputFileName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }

            strcpy(defaultOptions.outputFileName,optionData);
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   output-path                                                                   */
     /*-----------------------------------------------------------------------------------------*/
        /*
         else if (strcmp(option,"output-path")==0)
         {
            if (defaultOptions.outputPathName!=NULL)
               delete defaultOptions.outputPathName;

            defaultOptions.outputPathName = new char[optionDataLength+1];
            if (defaultOptions.outputPathName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }

            strcpy(defaultOptions.outputPathName,optionData);
         }
        */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   precision                                                                     */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"Pr‚cision")==0)
         {
            if (!sscanf(optionData,"%hu",&defaultOptions.outputPrecision))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   resolution                                                                    */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"R‚solution")==0)
         {
            if (!sscanf(optionData,"%hu",&defaultOptions.resolution))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   side-cut                                                                      */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"side-cut")==0)
         {
            if (!sscanf(optionData,"%lf",&defaultOptions.frontSideCut))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
            defaultOptions.backSideCut = defaultOptions.frontSideCut;
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   side-texture                                                                  */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"side-texture")==0)
         {
            if (defaultOptions.sideTextureName!=NULL)
               delete defaultOptions.sideTextureName;

            defaultOptions.sideTextureName = new char[optionDataLength+1];
            if (defaultOptions.sideTextureName==NULL)
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }
            strcpy(defaultOptions.sideTextureName,optionData);
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   sides                                                                         */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"C“t‚s")==0)
         {
            if (strcmp(optionData,"1")==0)
               defaultOptions.sideVisible = TRUE;
            else if (strcmp(optionData,"0")==0)
               defaultOptions.sideVisible = FALSE;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   string                                                                        */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"Texte")==0)
         {
            if (defaultOptions.string!=NULL)
            {
               delete defaultOptions.string;
               defaultOptions.stringLength = 0;
            }

            defaultOptions.string = new USHORT[optionDataLength+1];
            tempString = new CHAR[optionDataLength+1];
            if ((defaultOptions.string==NULL) || (tempString==NULL))
	         {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }
            strcpy(tempString,optionData);

            for (j=0;j<strlen(tempString);j++)
               defaultOptions.string[j]=tempString[j];

            defaultOptions.stringLength = strlen(tempString);
            delete tempString;
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   texture                                                                       */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"texture")==0)
         {
            if (defaultOptions.frontFaceTextureName!=NULL)
               delete defaultOptions.frontFaceTextureName;
            if (defaultOptions.backFaceTextureName!=NULL)
               delete defaultOptions.backFaceTextureName;
            if (defaultOptions.frontBevelTextureName!=NULL)
               delete defaultOptions.frontBevelTextureName;
            if (defaultOptions.backBevelTextureName!=NULL)
               delete defaultOptions.backBevelTextureName;
            if (defaultOptions.sideTextureName!=NULL)
               delete defaultOptions.sideTextureName;

            defaultOptions.frontFaceTextureName = new char[optionDataLength+1];
            defaultOptions.backFaceTextureName = new char[optionDataLength+1];
            defaultOptions.frontBevelTextureName = new char[optionDataLength+1];
            defaultOptions.backBevelTextureName = new char[optionDataLength+1];
            defaultOptions.sideTextureName = new char[optionDataLength+1];

            if (   defaultOptions.frontFaceTextureName==NULL
                || defaultOptions.backFaceTextureName==NULL
                || defaultOptions.frontBevelTextureName==NULL
                || defaultOptions.backBevelTextureName==NULL
                || defaultOptions.sideTextureName==NULL )
            {
               errorCode = ERR_OutOfMemory;
               errorPos  = i;
               return FALSE;
            }

            strcpy(defaultOptions.frontFaceTextureName,optionData);
            strcpy(defaultOptions.backFaceTextureName,optionData);
            strcpy(defaultOptions.frontBevelTextureName,optionData);
            strcpy(defaultOptions.backBevelTextureName,optionData);
            strcpy(defaultOptions.sideTextureName,optionData);
         }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   triangle-type                                                                 */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"TypeTriangle")==0)
         {
            if (strcmp(optionData,"FACETTE")==0)
               defaultOptions.triangleType = FLAT;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos  = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   xpos                                                                          */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"PositionSurX")==0)
         {
            if (strcmp(optionData,"GAUCHE")==0)
               defaultOptions.xPosition = LEFT;
            else if (strcmp(optionData,"CENTRE")==0)
               defaultOptions.xPosition = CENTER;
            else if (strcmp(optionData,"DROITE")==0)
               defaultOptions.xPosition = RIGHT;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   ypos                                                                          */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"PositionSurY")==0)
         {
            if (strcmp(optionData,"BAS")==0)
               defaultOptions.yPosition = BOTTOM;
            else if (strcmp(optionData,"ALIGNE")==0)
               defaultOptions.yPosition = BASELINE;
            else if (strcmp(optionData,"CENTRE")==0)
               defaultOptions.yPosition = CENTER;
            else if (strcmp(optionData,"HAUT")==0)
               defaultOptions.yPosition = TOP;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   zpos                                                                          */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"PositionSurZ")==0)
         {
            if (strcmp(optionData,"DERRIERE")==0)
               defaultOptions.zPosition = BACK;
            else if (strcmp(optionData,"CENTRE")==0)
               defaultOptions.zPosition = CENTER;
            else if (strcmp(optionData,"DEVANT")==0)
               defaultOptions.zPosition = FRONT;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }





     /*-----------------------------------------------------------------------------------------*/
     /*  These last few options are going to remain either unimplemented, or undocumented for   */
     /*  the time being...                                                                      */
     /*-----------------------------------------------------------------------------------------*/

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   codelist                                                                      */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"codelist")==0)
         {
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   spacing                                                                       */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"spacing")==0)
         {
            if (!sscanf(optionData,"%lf",&defaultOptions.spacing))
	    {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   spaceto                                                                       */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"spaceto")==0)
	 {
            if (!sscanf(optionData,"%lf",&defaultOptions.spaceto))
	    {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
	    }
	 }
         */

     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   smoothing-threshold                                                           */
     /*-----------------------------------------------------------------------------------------*/

         else if (strcmp(option,"TauxLissage")==0)
         {
            if (!sscanf(optionData,"%lf",&defaultOptions.threshold))
            {
               errorCode = ERR_InvalidOption;
               errorPos = i;
               return FALSE;
            }
            defaultOptions.threshold = defaultOptions.threshold*PI/180;
         }


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:   language                                                                      */
     /*-----------------------------------------------------------------------------------------*/
         /*
         else if (strcmp(option,"language")==0)
         {
            if (strcmp(optionData,"USENGLISH")==0)
               defaultOptions.language = USENGLISH;
            else if (strcmp(optionData,"UKENGLISH")==0)
               defaultOptions.language = UKENGLISH;
            else
            {
               errorCode = ERR_InvalidOption;
               errorPos  = i;
               return FALSE;
            }
         }
         */


     /*-----------------------------------------------------------------------------------------*/
     /* SWITCH:  UNKNOWN!                                                                       */
     /*-----------------------------------------------------------------------------------------*/

         else
         {
            errorCode = ERR_InvalidOption;
            errorPos=i;
            return FALSE;
         }

         delete option;


      } /* END Looping thru arguments */

      return TRUE;

   }
