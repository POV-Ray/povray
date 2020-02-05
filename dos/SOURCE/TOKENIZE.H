/****************************************************************************
*                   tokenize.h
*
*  This module contains all defines, typedefs, and prototypes for TOKENIZE.C.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1998 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's GO POVRAY Forum or visit
*  http://www.povray.org. The latest version of POV-Ray may be found at these sites.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/


#ifndef TOKENIZE_H
#define TOKENIZE_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/




/*****************************************************************************
* Global typedefs
******************************************************************************/


struct Token_Struct
{
  TOKEN Token_Id;
  TOKEN Function_Id;
  int Token_Line_No,Table_Index;
  char *Token_String;
  DBL Token_Float;
  int Unget_Token, End_Of_File;
  char *Filename;
  void *Data;
  int *NumberPtr;
  void **DataPtr;
};

#define MAX_PARAMETER_LIST 20

typedef struct Pov_Macro_Struct POV_MACRO;

struct Pov_Macro_Struct
{
  char *Macro_Name;
  char *Macro_Filename;
  long Macro_Pos,Macro_Line_No,Macro_End;
  int Num_Of_Pars;
  char *Par_Name[MAX_PARAMETER_LIST];
};

typedef struct Pov_Array_Struct POV_ARRAY;

struct Pov_Array_Struct
{
   int Dims, Type, Total;
   int Sizes[5];
   int Mags[5];
   void **DataPtrs;
};

typedef struct Pov_Param_Struct POV_PARAM;

struct Pov_Param_Struct
{
   int *NumberPtr;
   void **DataPtr;
};



/*****************************************************************************
* Global variables
******************************************************************************/

extern char String[MAX_STRING_INDEX];

extern struct Token_Struct Token;

extern struct Reserved_Word_Struct Reserved_Words [LAST_TOKEN];
extern int Table_Index;
extern int token_count;



/*****************************************************************************
* Global functions
******************************************************************************/

void Get_Token (void);
void Unget_Token (void);
void Where_Error (void);
void Parse_Directive (int After_Hash);
int Get_Include_File_Depth (void);
DATA_FILE * Get_Include_File_Array (void);
void Open_Include (void);
void pre_init_tokenizer (void);
void Initialize_Tokenizer (void);
void Terminate_Tokenizer (void);
SYM_ENTRY *Add_Symbol (int Index,char *Name,TOKEN Number);
void Destroy_Macro (POV_MACRO *PMac);
POV_ARRAY *Parse_Array_Declare (void);
SYM_ENTRY *Create_Entry (int Index,char *Name,TOKEN Number);
SYM_ENTRY *Destroy_Entry (SYM_ENTRY *Entry);
int Parse_Ifdef_Param (void);

#endif
