//******************************************************************************
///
/// @file povms/povms.h
///
/// Declarations for the C-style API of the POVMS message-passing framework.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#ifndef POVMS_H
#define POVMS_H

/// @file
/// @deprecated The API declared herein should not be used directly. Use the C++
///             wrapper API declared in @ref povms/povmscpp.h instead.

#ifdef POVMS_DISCONNECTED
    #include "cnfpovms.h"
#else
    #include "povms/configpovms.h"
#endif

#ifndef POVMS_NO_DUMP_SUPPORT
    #include <stdio.h>
#endif

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/// @def POVMS_ASSERT_OUTPUT(s,f,l)
///
/// Fatal error message output.
///
/// This macro is invoked when POVMS encounters a fatal error, e.g. lack of memory, to output a message. By default it
/// outputs `POVMS_ASSERT failed in <f> line <l>: <s>` to `stderr` using `fprintf`. Of course, this should never happen,
/// but be prepared and override this function if you need a better user interface.
///
/// @param[in]  s   Error message.
/// @param[in]  f   Source file name.
/// @param[in]  l   Source file line number.
///
#ifndef POVMS_ASSERT_OUTPUT
    #define POVMS_ASSERT_OUTPUT(s,f,l) fprintf(stderr, "POVMS_ASSERT failed in %s line %d: %s\n", f, (int)l, s)
#endif

/// @def POVMS_ASSERT_OUTPUT(s)
/// Log file output.
///
/// This macro is invoked whenever a POVMS function is called, to allow for some basic debugging. By default it does
/// nothing.
///
/// @param[in]  s   Function name.
///
#ifndef POVMS_LOG_OUTPUT
    #define POVMS_LOG_OUTPUT(s)
#endif

/// @def POVMSType
/// POVMS message type and attribute identifier type.
///
/// This type must be capable of holding 32 bit unsigned integers and will be used to store four-character identifier
/// codes, e.g. `MyTp`, `any `, `4Mac`, etc.
///
#ifndef POVMSType
    #define POVMSType           unsigned int
#endif

/// @def POVMSInt
/// POVMS general purpose integer data type.
///
/// This type must be capable of holding 32 bit signed integers.
///
#ifndef POVMSInt
    #define POVMSInt            int
#endif

/// @def POVMSLong
/// POVMS large value integer data type.
///
/// This type must be capable of holding 64 bit unsigned integers. It may be a compound data type.
///
/// @def SetPOVMSLong
/// Macro to compose values of type @ref POVMSLong.
///
/// This macro must compose a 64 bit value from two 32 bit integers representing upper and lower half.
///
/// @def GetPOVMSLong
/// Macro to decompose values of type @ref POVMSLong.
///
/// This macro must decompose a 64 bit value into two 32 bit integers representing upper and lower half.
///
#ifndef POVMSLong
    #define POVMSLong           long long

    #define SetPOVMSLong(v,h,l) *v = (((((POVMSLong)(h)) & 0x00000000ffffffff) << 32) | (((POVMSLong)(l)) & 0x00000000ffffffff))
    #define GetPOVMSLong(h,l,v) *h = ((v) >> 32) & 0x00000000ffffffff; *l = (v) & 0x00000000ffffffff
#endif

/// @def POVMSFloat
/// POVMS general purpose floating point data type.
///
/// This type should provide at least the same precision as the IEEE 754 single precision floating point type.
///
#ifndef POVMSFloat
    #define POVMSFloat          float
#endif


/// @def POVMSBool
/// POVMS data type representing boolean (true or false) values.
#ifndef POVMSBool
    #define POVMSBool           int
#endif

/// @def POVMSUCS2
/// POVMS character data type.
///
/// This type must be capable of holding 16 bit character codes.
///
#ifndef POVMSUCS2
    #define POVMSUCS2           unsigned short
#endif

/// @def POVMSStream
/// POVMS byte stream data type.
///
/// This type must be suitable for holding an individual data unit of a byte stream.
///
#ifndef POVMSStream
    #define POVMSStream         unsigned char
#endif

/// @def POVMSIEEEFloat
/// POVMS serialized floating point data type.
///
/// Unless @ref POVMS_NO_ORDERED_STREAM_DATA is defined, this type must be 4 bytes wide and able to hold floating point
/// values encoded according to the IEEE 754 single precision format. No restrictions are placed on the byte ordering.
///
/// If @ref POVMS_NO_ORDERED_STREAM_DATA is defined (indicating that the sending and receiving systems use the very same
/// number formats and byte ordering) the data type still needs to have the same width and should provide the same
/// precision, but encoding may differ from IEEE 754.
///
/// @def POVMSFloatToPOVMSIEEEFloat(p,f)
/// Macro to convert from @ref POVMSFloat to @ref POVMSIEEEFloat.
///
/// @param[in]  p   The @ref POVMSFloat input value.
/// @param[out] f   The @ref POVMSIEEEFloat output value.
///
/// @def POVMSIEEEFloatToPOVMSFloat(f,p)
/// Macro to convert from @ref POVMSIEEEFloat to @ref POVMSFloat.
///
/// @param[in]  f   The @ref POVMSIEEEFloat input value.
/// @param[out] p   The @ref POVMSFloat output value.
///
/// @def HexToPOVMSIEEEFloat(h,f)
/// Macro to convert from 32 bit unsigned integer to @ref POVMSIEEEFloat.
///
/// This macro must take an unsigned integer constant and, re-interpreting its binary representation according to the
/// IEEE 754 single precision floating point format, convert it to @ref POVMSIEEEFloat.
///
/// This macro is only used in setting up POVMS byte ordering, and is not required if @ref POVMS_NO_ORDERED_STREAM_DATA
/// is defined.
///
/// @param[in]  h   The unsigned integer input value.
/// @param[out] f   The @ref POVMSIEEEFloat output value.
///
#ifndef POVMS_NO_ORDERED_STREAM_DATA
    #ifndef POVMSIEEEFloat
        #define POVMSIEEEFloat      float

        #define POVMSFloatToPOVMSIEEEFloat(p, f) f = p
        #define POVMSIEEEFloatToPOVMSFloat(f, p) p = f

        #define HexToPOVMSIEEEFloat(h, f) *(reinterpret_cast<POVMSInt *>(&f)) = h
    #endif
#else
    #ifndef POVMSIEEEFloat
        #define POVMSIEEEFloat      POVMSFloat

        #define POVMSFloatToPOVMSIEEEFloat(p, f) f = p
        #define POVMSIEEEFloatToPOVMSFloat(f, p) p = f
    #endif
#endif

/* Note: POVMSAddress needs work with the default copy constructor.
   Adjust it to fit your message addressing needs! */

/// @def POVMSAddress
/// POVMS message queue identifier.
///
/// This type must be a POD type or POD-only struct that can be sent around and uniquely describes a particular
/// POVMS message queue in a particular process and thread.
///
#ifndef POVMSAddress
    #define POVMSAddress        void *
    #define POVMSInvalidAddress NULL
#endif

/* Note: Use POVMS_EXPORT if you need a special export keyword
   in order to use the POVMS functions as part of a library. */
#ifndef POVMS_EXPORT
    #define POVMS_EXPORT
#endif

#ifndef POVMS_CDECL
    #define POVMS_CDECL
#endif

// Note: Remember that the POVMS cannot use the standard
// POV_MALLOC, POV_REALLOC, POV_FREE calls!
#ifndef POVMS_Sys_Malloc
    #define POVMS_Sys_Malloc(s)           malloc(s)
#endif

#ifndef POVMS_Sys_Realloc
    #define POVMS_Sys_Realloc(p,s)        realloc(p,s)
#endif

#ifndef POVMS_Sys_Free
    #define POVMS_Sys_Free(p)             free(p)
#endif

#undef POVMS_VERSION
#define POVMS_VERSION 2


/*****************************************************************************
* Global typedefs
******************************************************************************/

/* Note: All only upper case types are reserved for internal use. */
enum
{
    kPOVMSObjectClassID         = 'OCLA',
    kPOVMSMessageClassID        = 'MCLA',
    kPOVMSMessageIdentID        = 'MIDE',
    kPOVMSSourceAddressID       = 'MSRC',
    kPOVMSDestinationAddressID  = 'MDST',
    kPOVMSMessageTimeoutID      = 'TOUT',
    kPOVMSMessageErrorID        = 'MERR',
    kPOVMSMessageSequenceID     = 'MSEQ',
    kPOVMSResultSequenceID      = 'RSEQ'
};

enum
{
    kPOVMSType_Object           = 'OBJE',
    kPOVMSType_LockedObject     = 'LOCK',
    kPOVMSType_ResultObject     = 'RESU',
    kPOVMSType_Address          = 'ADDR',
    kPOVMSType_Null             = 'NULL',
    kPOVMSType_WildCard         = '****',
    kPOVMSType_CString          = 'CSTR',
    kPOVMSType_UCS2String       = 'U2ST',
    kPOVMSType_Int              = 'INT4',
    kPOVMSType_Long             = 'INT8',
    kPOVMSType_Float            = 'FLT4',
    kPOVMSType_Double           = 'FLT8',
    kPOVMSType_Bool             = 'BOOL',
    kPOVMSType_Type             = 'TYPE',
    kPOVMSType_List             = 'LIST',
    kPOVMSType_VectorInt        = 'VIN4',
    kPOVMSType_VectorLong       = 'VIN8',
    kPOVMSType_VectorFloat      = 'VFL4',
    kPOVMSType_VectorType       = 'VTYP',
};

typedef void * POVMSContext;

typedef struct POVMSData POVMSObject;
typedef struct POVMSData POVMSAttribute;
typedef struct POVMSData POVMSAttributeList;

typedef POVMSObject        *POVMSObjectPtr;
typedef POVMSAttribute     *POVMSAttributePtr;
typedef POVMSAttributeList *POVMSAttributeListPtr;

typedef struct POVMSNode POVMSNode;

struct POVMSData
{
    POVMSType type;
    int size;
    union
    {
        void *ptr;
        struct POVMSData *items;
        struct POVMSNode *root;
    };
};

struct POVMSNode
{
    struct POVMSNode *last;
    struct POVMSNode *next;
    POVMSType key;
    struct POVMSData data;
};

enum
{
    kPOVMSSendMode_Invalid = 0,
    kPOVMSSendMode_NoReply = 1,
    kPOVMSSendMode_WaitReply = 2,
    kPOVMSSendMode_WantReceipt = 3
};


/*****************************************************************************
* Global variables
******************************************************************************/


/*****************************************************************************
* Global functions
******************************************************************************/

// POVMS context functions
POVMS_EXPORT int POVMS_CDECL POVMS_OpenContext      (POVMSContext *contextrefptr);
POVMS_EXPORT int POVMS_CDECL POVMS_CloseContext     (POVMSContext contextref);
POVMS_EXPORT int POVMS_CDECL POVMS_GetContextAddress(POVMSContext contextref, POVMSAddress *addrptr);

// Message receive handler functions
POVMS_EXPORT int POVMS_CDECL POVMS_InstallReceiver  (POVMSContext contextref, int (*hfunc)(POVMSObjectPtr, POVMSObjectPtr, int, void *), POVMSType hclass, POVMSType hid, void *hpd);
POVMS_EXPORT int POVMS_CDECL POVMS_RemoveReceiver   (POVMSContext contextref, POVMSType hclass, POVMSType hid);

// Message receive functions
POVMS_EXPORT int POVMS_CDECL POVMS_ProcessMessages  (POVMSContext contextref, POVMSBool blocking, POVMSBool yielding);
POVMS_EXPORT int POVMS_CDECL POVMS_Receive          (POVMSContext contextref, POVMSObjectPtr msg, POVMSObjectPtr result, int mode);

// Message send functions
POVMS_EXPORT int POVMS_CDECL POVMS_Send             (POVMSContext contextref, POVMSObjectPtr msg, POVMSObjectPtr result, int mode);

// Message data functions
POVMS_EXPORT int POVMS_CDECL POVMSMsg_SetupMessage          (POVMSObjectPtr object, POVMSType msgclass, POVMSType msgid);
POVMS_EXPORT int POVMS_CDECL POVMSMsg_GetMessageClass       (POVMSObjectPtr object, POVMSType *msgclass);
POVMS_EXPORT int POVMS_CDECL POVMSMsg_SetMessageClass       (POVMSObjectPtr object, POVMSType msgclass);
POVMS_EXPORT int POVMS_CDECL POVMSMsg_GetMessageIdentifier  (POVMSObjectPtr object, POVMSType *msgid);
POVMS_EXPORT int POVMS_CDECL POVMSMsg_SetMessageIdentifier  (POVMSObjectPtr object, POVMSType msgid);
POVMS_EXPORT int POVMS_CDECL POVMSMsg_GetSourceAddress      (POVMSObjectPtr object, POVMSAddress *value);
POVMS_EXPORT int POVMS_CDECL POVMSMsg_SetSourceAddress      (POVMSObjectPtr object, POVMSAddress value);
POVMS_EXPORT int POVMS_CDECL POVMSMsg_GetDestinationAddress (POVMSObjectPtr object, POVMSAddress *value);
POVMS_EXPORT int POVMS_CDECL POVMSMsg_SetDestinationAddress (POVMSObjectPtr object, POVMSAddress value);

// Object functions
POVMS_EXPORT int POVMS_CDECL POVMSObject_New        (POVMSObjectPtr object, POVMSType objclass);
POVMS_EXPORT int POVMS_CDECL POVMSObject_Delete     (POVMSObjectPtr object);
POVMS_EXPORT int POVMS_CDECL POVMSObject_Copy       (POVMSObjectPtr sourceobject, POVMSObjectPtr destobject);
POVMS_EXPORT int POVMS_CDECL POVMSObject_Merge      (POVMSObjectPtr sourceobject, POVMSObjectPtr destobject);
POVMS_EXPORT int POVMS_CDECL POVMSObject_Get        (POVMSObjectPtr object, POVMSAttributePtr attr, POVMSType key);
POVMS_EXPORT int POVMS_CDECL POVMSObject_Set        (POVMSObjectPtr object, POVMSAttributePtr attr, POVMSType key);
POVMS_EXPORT int POVMS_CDECL POVMSObject_Remove     (POVMSObjectPtr object, POVMSType key);
POVMS_EXPORT int POVMS_CDECL POVMSObject_Exist      (POVMSObjectPtr object, POVMSType key);
POVMS_EXPORT int POVMS_CDECL POVMSObject_Count      (POVMSObjectPtr object, int  *cnt);
POVMS_EXPORT int POVMS_CDECL POVMSObject_GetClass   (POVMSObjectPtr object, POVMSType *objclass);

#ifndef POVMS_NO_DUMP_SUPPORT
// Object debug functions
POVMS_EXPORT int POVMS_CDECL POVMSObject_Dump       (FILE *file, POVMSObjectPtr object);
POVMS_EXPORT int POVMS_CDECL POVMSObject_DumpAttr   (FILE *file, POVMSAttributePtr attr);
#endif

// Object streaming functions
POVMS_EXPORT int POVMS_CDECL POVMSObjectStream_Size (POVMSObjectPtr object, int *streamsize);
POVMS_EXPORT int POVMS_CDECL POVMSObjectStream_Read (POVMSObjectPtr object, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSObjectStream_Write(POVMSObjectPtr object, POVMSStream *stream, int *maxstreamsize);

// Attribute functions
POVMS_EXPORT int POVMS_CDECL POVMSAttr_New          (POVMSAttributePtr attr);
POVMS_EXPORT int POVMS_CDECL POVMSAttr_Delete       (POVMSAttributePtr attr);
POVMS_EXPORT int POVMS_CDECL POVMSAttr_Copy         (POVMSAttributePtr sourceattr, POVMSAttributePtr destattr);
POVMS_EXPORT int POVMS_CDECL POVMSAttr_Get          (POVMSAttributePtr attr, POVMSType type, void *data, int *maxdatasize);
POVMS_EXPORT int POVMS_CDECL POVMSAttr_Set          (POVMSAttributePtr attr, POVMSType type, const void *data, int datasize);
POVMS_EXPORT int POVMS_CDECL POVMSAttr_Size         (POVMSAttributePtr attr, int *size);
POVMS_EXPORT int POVMS_CDECL POVMSAttr_Type         (POVMSAttributePtr attr, POVMSType *type);

// Attribute list functions
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_New      (POVMSAttributeListPtr attr);
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Delete   (POVMSAttributeListPtr attr);
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Copy     (POVMSAttributeListPtr sourcelist, POVMSAttributeListPtr destlist);
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Append   (POVMSAttributeListPtr attr, POVMSAttributePtr item);
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_AppendN  (POVMSAttributeListPtr attr, int cnt, POVMSAttributePtr item);
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Remove   (POVMSAttributeListPtr attr);
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_GetNth   (POVMSAttributeListPtr attr, int index, POVMSAttributePtr item);
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_SetNth   (POVMSAttributeListPtr attr, int index, POVMSAttributePtr item);
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_RemoveNth(POVMSAttributeListPtr attr, int index);
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Clear    (POVMSAttributeListPtr attr);
POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Count    (POVMSAttributeListPtr attr, int *cnt);

// Utility functions
POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetString    (POVMSObjectPtr object, POVMSType key, const char *str); // Note: Strings may not contain \0 character codes!
POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetUCS2String(POVMSObjectPtr object, POVMSType key, const POVMSUCS2 *str); // Note: Strings may not contain \0 character codes!
POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetInt       (POVMSObjectPtr object, POVMSType key, POVMSInt value);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetLong      (POVMSObjectPtr object, POVMSType key, POVMSLong value);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetFloat     (POVMSObjectPtr object, POVMSType key, POVMSFloat value);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetBool      (POVMSObjectPtr object, POVMSType key, POVMSBool boolvalue);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetType      (POVMSObjectPtr object, POVMSType key, POVMSType typevalue);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetStringLength(POVMSObjectPtr object, POVMSType key, int *len); // Note: Includes trailing \0 character code!
POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetString    (POVMSObjectPtr object, POVMSType key, char *str, int *maxlen);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetUCS2StringLength(POVMSObjectPtr object, POVMSType key, int *len); // Note: Includes trailing \0 character code!
POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetUCS2String(POVMSObjectPtr object, POVMSType key, POVMSUCS2 *str, int *maxlen);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetInt       (POVMSObjectPtr object, POVMSType key, POVMSInt *value);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetLong      (POVMSObjectPtr object, POVMSType key, POVMSLong *value);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetFloat     (POVMSObjectPtr object, POVMSType key, POVMSFloat *value);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetBool      (POVMSObjectPtr object, POVMSType key, POVMSBool *boolvalue);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetType      (POVMSObjectPtr object, POVMSType key, POVMSType *typevalue);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_TempAlloc    (void **ptr, int datasize);
POVMS_EXPORT int POVMS_CDECL POVMSUtil_TempFree     (void *ptr);

// Memory debug functions
#ifdef _DEBUG_POVMS_TRACE_MEMORY_
POVMS_EXPORT int POVMS_TraceDump                    ();
#endif

#endif /* POVMS_H */

/// @file
/// @todo The stream functions are C++ specific, and therefore shouldn't be in here.

#ifdef POVMSCPP_H
    #define POVMS_EXPORT_STREAM_FUNCTIONS
#endif

// Stream functions only available to the C++ interface
#ifdef POVMS_EXPORT_STREAM_FUNCTIONS

// Stream reading functions
POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadString         (char *data, POVMSStream *stream, int datasize, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadUCS2String     (POVMSUCS2 *data, POVMSStream *stream, int datasize, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadInt            (POVMSInt *data, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadLong           (POVMSLong *data, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadFloat          (POVMSFloat *data, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadType           (POVMSType *data, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_Read               (struct POVMSData *data, POVMSStream *stream, int *maxstreamsize);

// Stream writing functions
POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteString        (const char *data, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteUCS2String    (const POVMSUCS2 *data, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteInt           (POVMSInt data, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteLong          (POVMSLong data, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteFloat         (POVMSFloat data, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteType          (POVMSType data, POVMSStream *stream, int *maxstreamsize);
POVMS_EXPORT int POVMS_CDECL POVMSStream_Write              (struct POVMSData *data, POVMSStream *stream, int *maxstreamsize);

// Stream utility functions
POVMS_EXPORT int POVMS_CDECL POVMSStream_Size               (struct POVMSData *data);
POVMS_EXPORT int POVMS_CDECL POVMSStream_CheckMessageHeader (POVMSStream *stream, int streamsize, int *totalsize);

#endif
