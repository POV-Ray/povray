/*==============================================================================================*/
/*   config.h                                                                      Font3D       */
/*----------------------------------------------------------------------------------------------*/
/*                                                                                              */
/*   Copyright (c) 1994, 1995 by Todd A. Prater.                                Version 1.50    */
/*   All rights reserved.                                                                       */
/*                                                                                              */
/*----------------------------------------------------------------------------------------------*/
/*                                                                                              */
/*   Type definitions and enumerations.  Hopefully, anything that is platform-specific can      */
/*   be put in here.                                                                            */
/*                                                                                              */
/*==============================================================================================*/

#ifndef __Config_H__
#define __Config_H__

/* ____ Define some useful symbolic constants ____ */

#define TRUE                  1
#define FALSE                 0

#define NO_POINT              0
#define OFF_CURVE             1
#define ON_CURVE              2

#define IGNORE                0
#define OUTSIDE               1
#define INSIDE                2
#define COMBINED              3

#define CLOCKWISE             0
#define COUNTER_CLOCKWISE     1

#define ERR_NoError           0
#define ERR_OutOfMemory       1
#define ERR_TableNotFound     2
#define ERR_NoCharMapFound    3
#define ERR_UnknownCmapFormat 4
#define ERR_CmapNotPresent    5
#define ERR_UnableToOpenFile  6
#define ERR_UnknownKernFormat 7
#define ERR_NoOptionsFound    8
#define ERR_InvalidOption     9
#define ERR_NoPolyFound      10

#define RAW                   0
#define POV                   1
#define RIB                   2
#define DXF                   3
#define VIVID                 4
#define ROUND                 5
#define FLAT                  6
#define SMOOTH                7
#define LEFT                  8
#define RIGHT                 9
#define CENTER               10
#define BOTTOM               11
#define BASELINE             12
#define TOP                  13
#define BACK                 14
#define FRONT                15

#define OFF                   0
#define ON                    1

#define MAC                   0
#define MS                    1

#define BIG              1.0e30
#define MIN_SHRINK_ANGLE   0.15
#ifndef PI
#define PI         3.1415926536
#endif

#define USENGLISH 0
#define UKENGLISH 1


/* ____ Data Types:                                                     ____ */
/* ____                                                                 ____ */
/* ____   BYTE......... An unsigned 8-bit integer data type.            ____ */
/* ____   CHAR......... A signed 8-bit integer data type.               ____ */
/* ____   USHORT....... An unsigned 16-bit integer data type.           ____ */
/* ____   SHORT........ A signed 16-bit integer data type.              ____ */
/* ____   ULONG........ An unsigned 32-bit integer data type.           ____ */
/* ____   LONG......... A signed 32-bit integer data type.              ____ */
/* ____   DOUBLE....... A double precision floating point data type.    ____ */

typedef unsigned char         BYTE;
typedef char                  CHAR;
typedef unsigned short int    USHORT;
typedef short int             SHORT;
typedef unsigned long int     ULONG;
typedef long int              LONG;
typedef double                DOUBLE;

typedef BYTE                 *BYTEPTR;
typedef CHAR                 *CHARPTR;
typedef USHORT               *USHORTPTR;
typedef SHORT                *SHORTPTR;
typedef ULONG                *ULONGPTR;
typedef LONG                 *LONGPTR;
typedef DOUBLE               *DOUBLEPTR;
typedef LONG                  INT;
typedef USHORT                uFWord;
typedef SHORT                 FWord;
typedef ULONG                 Fixed;
typedef USHORT                F2Dot14;

/* ____ Conversion Macros:                                                               ____*/
/* ____                                                                                  ____*/
/* ____    toDOUBLE(x)...........Converts a 'Fixed' data object to a DOUBLE              ____*/
/* ____    toUSHORT(b1,b2).......Combines two bytes 'b1' and 'b2' ('b1' is MSB) into a   ____*/
/* ____                          'USHORT' data object.                                   ____*/
/* ____    toSHORT (b1,b2).......Combines two bytes 'b1' and 'b2' ('b1' is MSB) into a   ____*/
/* ____                          'SHORT' data object.                                    ____*/
/* ____    toULONG (b1,b2,b3,b4).Combines four bytes 'b1', 'b2', 'b3', and 'b4' ('b1' is ____*/
/* ____                          MSB, 'b4' is LSB) into a 'ULONG' data object.           ____*/

#define toDOUBLE(x)           (  (((SHORT)(x/65536L))>0)\
                               ? ( ((SHORT)(x/65536L))\
                                  +(((DOUBLE)(x%65536L))/(0xffffffff)))\
                               : ( ((SHORT)(x/65536L))\
                                  -(((DOUBLE)(x%65536L))/(0xffffffff))))

#define toUSHORT(b1,b2)      (((USHORT)b1*256)+((USHORT)b2))
#define toSHORT(b1,b2)       ((SHORT)((USHORT)b1*256)+((USHORT)b2))
#define toULONG(b1,b2,b3,b4) (((ULONG)b1*16777216L)+((ULONG)b2*65536L)+((ULONG)b3*256)+((ULONG)b4))



/* ____ A Macro that evaluates to 1 if a bit position is set in a byte, 0 otherwise.    ____*/

#define isBitSet(byte,bit)    ((0x01<<bit)&byte)

extern "C" {
  #define EFFACE  0
  #define AFFICHE 1
  #define SAUVE   2
  #define MODIF      14

  void f_erreur(char *String,...);
  void message(char *String,...);
  void f_jauge(BYTE Nb,BYTE Travail,long PC,long CPC,char *Texte);
};

#endif
