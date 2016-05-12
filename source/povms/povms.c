//******************************************************************************
///
/// @file povms/povms.c
///
/// This module contains POVMS data type handling and utility functions.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Note: Needed for function prototypes
#define POVMS_EXPORT_STREAM_FUNCTIONS
#include "povms.h"

#ifndef POVMS_DISCONNECTED
    /// @file
    /// @todo We shouldn't include this; instead, we should use our own error values (that _may_ be defined in terms of POV-Ray's error values in @ref povms/configpovms.h).
    #include "base/pov_err.h"

    // this must be the last file included
    #include "base/povdebug.h"
#endif

using namespace pov_base;

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

// #define _DEBUG_POVMS_DUMP_MESSAGES_
// #define _DEBUG_POVMS_TRACE_MEMORY_

#ifndef kDefaultTimeout
    #define kDefaultTimeout                 10
#endif

#ifndef POVMS_ASSERT
    #define POVMS_ASSERT(c,s) POVMS_AssertFunction(c, s, __FILE__, __LINE__)
#endif

typedef struct POVMS_Sys_QueueNode_Default POVMS_Sys_QueueNode_Default;
typedef struct POVMS_Sys_QueueDataNode_Default POVMS_Sys_QueueDataNode_Default;

struct POVMS_Sys_QueueDataNode_Default
{
    POVMS_Sys_QueueDataNode_Default *next;
    void *data;
    int len;
};

struct POVMS_Sys_QueueNode_Default
{
    int magic;
    int entries;
    POVMS_Sys_QueueDataNode_Default *first;
    POVMS_Sys_QueueDataNode_Default *last;
};

// Note: You should really override these!
#ifndef POVMS_Sys_Thread_Type
    #define POVMS_Sys_Thread_Type         int
#endif

#ifndef POVMS_Sys_Queue_Type
    #define POVMS_Sys_Queue_Type          POVMS_Sys_QueueNode_Default *
#endif

#ifndef POVMS_Sys_GetCurrentThread
    #define POVMS_Sys_GetCurrentThread()  POVMS_Sys_GetCurrentThread_Default()
#endif

#ifndef POVMS_Sys_QueueToAddress
    #define POVMS_Sys_QueueToAddress(q)   POVMS_Sys_QueueToAddress_Default(q)
#endif

#ifndef POVMS_Sys_AddressToQueue
    #define POVMS_Sys_AddressToQueue(a)   POVMS_Sys_AddressToQueue_Default(a)
#endif

#ifndef POVMS_Sys_QueueOpen
    #define POVMS_Sys_QueueOpen()         POVMS_Sys_QueueOpen_Default()
#endif

#ifndef POVMS_Sys_QueueClose
    #define POVMS_Sys_QueueClose(q)       POVMS_Sys_QueueClose_Default(q)
#endif

// Note: Blocking is optional, that is, even if the parameter is true,
// this function does not have to block! [trf]
#ifndef POVMS_Sys_QueueReceive
    #define POVMS_Sys_QueueReceive(q,l,b,y) POVMS_Sys_QueueReceive_Default(q, l, b, y)
#endif

#ifndef POVMS_Sys_QueueSend
    #define POVMS_Sys_QueueSend(q, p, l)  POVMS_Sys_QueueSend_Default(q, p, l)
#endif

#ifndef POVMS_Sys_AddressFromStream
    #define POVMS_Sys_AddressFromStream(a, s, z)      POVMS_Sys_AddressFromStream_Default((a), (s), (z))
#endif

#ifndef POVMS_Sys_AddressToStreamSize
    #define POVMS_Sys_AddressToStream(a, s, z)        POVMS_Sys_AddressToStream_Default((a), (s), (z))
#endif

#ifndef POVMS_Sys_AddressToStreamSize
    #define POVMS_Sys_AddressToStreamSize(a)          POVMS_Sys_AddressToStreamSize_Default((a))
#endif

// Note: Timer function which returns time in seconds.
// Other precisions are not supported! */
#ifndef POVMS_Sys_Timer
    #define POVMS_Sys_Timer()             (POVMSLong)time(NULL)
#endif

// Note: These low level functions allow building POVMS without a
// C library. Usually you do not have to override them! */
#ifndef POVMS_Sys_Strlen
    #define POVMS_Sys_Strlen(p)           strlen(p)
#endif

#ifndef POVMS_Sys_Memmove
    #define POVMS_Sys_Memmove(a, b, c)    memmove(a, b, c)
#endif

// Note: Usually no need to change this one! */
#ifndef POVMS_Sys_UCS2Strlen
    #define POVMS_Sys_UCS2Strlen(p)       POVMS_Sys_UCS2Strlen_Default(p)
#endif

#ifdef _DEBUG_POVMS_TRACE_MEMORY_
    #undef POVMS_Sys_Malloc
    #undef POVMS_Sys_Realloc
    #undef POVMS_Sys_Free

    #define POVMS_Sys_Malloc(s)           POVMS_Sys_Trace_Malloc(s, __LINE__)
    #define POVMS_Sys_Realloc(p,s)        POVMS_Sys_Trace_Realloc(p,s, __LINE__)
    #define POVMS_Sys_Free(p)             POVMS_Sys_Trace_Free(p, __LINE__)

    typedef struct POVMSMemoryTraceHeader POVMSMemoryTraceHeader;

    struct POVMSMemoryTraceHeader
    {
        char magichead[8];
        POVMSMemoryTraceHeader *last;
        POVMSMemoryTraceHeader *next;
        int line;
        int size;
        char magictrail[8];
    };
#endif


/*****************************************************************************
* Local typedefs
******************************************************************************/

typedef struct POVMSReceiveHandlerNode POVMSReceiveHandlerNode;
typedef struct POVMSContextData POVMSContextData;

struct POVMSReceiveHandlerNode
{
    POVMSReceiveHandlerNode *last;
    POVMSReceiveHandlerNode *next;
    POVMSType handledclass;
    POVMSType handledid;
    void *handlerprivatedata;
    int (*handler)(POVMSObjectPtr, POVMSObjectPtr, int, void *);
};

struct POVMSContextData
{
    POVMSBool valid;
    POVMSReceiveHandlerNode *receivehandlerroot;
    POVMS_Sys_Queue_Type queue;
    POVMS_Sys_Thread_Type thread;
    POVMSObject result;
    POVMSLong resultid;
    POVMSLong nextsequenceid;
};


/*****************************************************************************
* Local variables
******************************************************************************/

struct
{
    int int_write[4],   int_read[4];
    int long_write[8],  long_read[8];
    int float_write[4], float_read[4];
    int type_write[4],  type_read[4];
    int ucs2_write[2],  ucs2_read[2];
} POVMSStreamOrderTables;


/*****************************************************************************
* Local functions
******************************************************************************/

int POVMS_AssertFunction       (int cond, const char *str, const char *filename, int line);

POVMSReceiveHandlerNode *POVMS_AddReceiveHandlerNode  (POVMSContextData *context);
int POVMS_RemoveReceiveHandlerNode                    (POVMSContextData *context, POVMSReceiveHandlerNode *thn);
POVMSReceiveHandlerNode *POVMS_FindReceiveHandlerNode (POVMSContextData *context, POVMSType hclass, POVMSType hid);

void POVMSStream_Init          ();

void POVMSStream_BuildOrderTable                      (POVMSStream *srcdata, POVMSStream *dstdata, int *order, int datasize);

void POVMSStream_ReadDataOrdered                      (POVMSStream *stream, POVMSStream *data, int *order, int datasize);
void POVMSStream_ReadDataUnordered                    (POVMSStream *stream, POVMSStream *data, int datasize);

void POVMSStream_WriteDataOrdered                     (const POVMSStream *data, POVMSStream *stream, int *order, int datasize);
void POVMSStream_WriteDataUnordered                   (const POVMSStream *data, POVMSStream *stream, int datasize);

POVMSNode *POVMSObject_Find    (POVMSObjectPtr msg, POVMSType key);

#ifndef POVMS_NO_DUMP_SUPPORT
int POVMSStream_Dump           (FILE *file, POVMSStream *stream, int datasize);
int POVMSObject_DumpSpace      (FILE *file);
#endif

unsigned int POVMS_Sys_UCS2Strlen_Default                     (const POVMSUCS2 *s);

POVMS_Sys_Thread_Type POVMS_Sys_GetCurrentThread_Default      ();
POVMSAddress POVMS_Sys_QueueToAddress_Default                 (POVMS_Sys_QueueNode_Default *q);
POVMS_Sys_QueueNode_Default *POVMS_Sys_AddressToQueue_Default (POVMSAddress a);
POVMS_Sys_QueueNode_Default *POVMS_Sys_QueueOpen_Default      ();
void POVMS_Sys_QueueClose_Default                             (POVMS_Sys_QueueNode_Default *q);
void *POVMS_Sys_QueueReceive_Default                          (POVMS_Sys_QueueNode_Default *q, int *l, bool blocking, bool yielding);
int POVMS_Sys_QueueSend_Default                               (POVMS_Sys_QueueNode_Default *q, void *p, int l);
int POVMS_Sys_AddressFromStream_Default                       (POVMSAddress *a, POVMSStream *s, int z);
int POVMS_Sys_AddressToStream_Default                         (POVMSAddress a, POVMSStream *s, int *z);
int POVMS_Sys_AddressToStreamSize_Default                     (POVMSAddress a);

#ifdef _DEBUG_POVMS_TRACE_MEMORY_
void *POVMS_Sys_Trace_Malloc(size_t size, int line);
void *POVMS_Sys_Trace_Realloc(void *iptr, size_t size, int line);
void POVMS_Sys_Trace_Free(void *ptr, int line);
void POVMS_Sys_Trace_Set_Guard(char *ptr);
int POVMS_Sys_Trace_Check_Guard(char *ptr);
void POVMS_Sys_Trace_Insert(POVMSMemoryTraceHeader *ptr);
void POVMS_Sys_Trace_Remove(POVMSMemoryTraceHeader *ptr);
#endif


/*****************************************************************************
*
* FUNCTION
*   POVMS_OpenContext
*
* DESCRIPTION
*   Call to create data structures for receiving messages.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMS_OpenContext(POVMSContext *contextrefptr)
{
    POVMSContextData *context = NULL;
    int err = kNoErr;

    POVMS_LOG_OUTPUT("POVMS_Initialize");

    if(contextrefptr == NULL)
        return kParamErr;

    POVMSStream_Init();

    *contextrefptr = NULL;

    context = (POVMSContextData *)POVMS_Sys_Malloc(sizeof(POVMSContextData));
    if(POVMS_ASSERT(context != NULL, "POVMS_Open_Context failed, out of memory") == false)
        return kMemFullErr;

    context->receivehandlerroot = NULL;
    context->queue = POVMS_Sys_QueueOpen();
    context->thread = POVMS_Sys_GetCurrentThread();
    context->result.type = kPOVMSType_Null;
    context->resultid = 0;
    context->nextsequenceid = 1;

    context->valid = true;

    *contextrefptr = (POVMSContext)context;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_CloseContext
*
* DESCRIPTION
*   Call to destroy data structures for receiving messages.
*   Never call this inside a POVMS function!
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMS_CloseContext(POVMSContext contextref)
{
    POVMSContextData *context = (POVMSContextData *)contextref;

    POVMS_LOG_OUTPUT("POVMS_CloseContext");

    if(contextref == NULL)
        return kParamErr;
    if(context->valid == false)
        return kInvalidContextErr;

    context->valid = false;

    POVMS_Sys_QueueClose(context->queue);
    context->queue = NULL;

    while(context->receivehandlerroot != NULL)
        (void)POVMS_RemoveReceiveHandlerNode(context, context->receivehandlerroot);

    POVMS_Sys_Free((void *)context);

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_GetContextAddress
*
* DESCRIPTION
*   Get the POVMSAddress of the specified context.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMS_GetContextAddress(POVMSContext contextref, POVMSAddress *addrptr)
{
    POVMSContextData *context = (POVMSContextData *)contextref;

    POVMS_LOG_OUTPUT("POVMS_GetContextAddress");

    if(contextref == NULL)
        return kParamErr;
    if(context->valid == false)
        return kInvalidContextErr;
    if(addrptr == NULL)
        return kParamErr;

    *addrptr = POVMS_Sys_QueueToAddress(context->queue);

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_ASSERTFunction
*
* DESCRIPTION
*   Low level fatal error checking and output function.
*   Note: Currently it will not and may not terminate execution!
*
* CHANGES
*   -
*
******************************************************************************/

int POVMS_AssertFunction(int cond, const char *str, const char *filename, int line)
{
    if(cond == false)
    {
        POVMS_ASSERT_OUTPUT(str,filename,line);
        return false;
    }

    return true;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_InstallReceiver
*
* DESCRIPTION
*   Installs a function which will be called when a message with the given
*   class and id is received. You may specify kPOVMSType_WildCard as id, then
*   the function will get all messages of the class. Note that you may not
*   define other handlers for a class if you registered a handler function
*   with the kPOVMSType_WildCard id for that class. However, there is currently
*   no checking if a specified class and id are valid.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMS_InstallReceiver(POVMSContext contextref, int (*hfunc)(POVMSObjectPtr, POVMSObjectPtr, int, void *), POVMSType hclass, POVMSType hid, void *hpd)
{
    POVMSContextData *context = (POVMSContextData *)contextref;
    POVMSReceiveHandlerNode *currhn = NULL;

    POVMS_LOG_OUTPUT("POVMS_InstallReceiver");

    if(contextref == NULL)
        return kParamErr;
    if(context->valid == false)
        return kInvalidContextErr;
    if(hfunc == NULL)
        return kParamErr;

    currhn = POVMS_AddReceiveHandlerNode(context);
    if(currhn == NULL)
        return kMemFullErr;

    currhn->handledclass = hclass;
    currhn->handledid = hid;
    currhn->handlerprivatedata = hpd;
    currhn->handler = hfunc;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_RemoveReceiver
*

* DESCRIPTION
*   Removes a handler function for the specified class and id.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMS_RemoveReceiver(POVMSContext contextref, POVMSType hclass, POVMSType hid)
{
    POVMSContextData *context = (POVMSContextData *)contextref;
    int err = kNoErr;

    POVMS_LOG_OUTPUT("POVMS_RemoveReceiver");

    if(contextref == NULL)
        return kParamErr;
    if(context->valid == false)
        return kInvalidContextErr;

    if(err == kNoErr)
        err = POVMS_RemoveReceiveHandlerNode(context, POVMS_FindReceiveHandlerNode(context, hclass, hid));

    return err;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_ProcessMessages
*
* DESCRIPTION
*   Processes messages.  Return kFalseErr if there *might* be more messages
*   waiting to be processed.  Note that the blocking is optional!
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMS_ProcessMessages(POVMSContext contextref, POVMSBool blocking, POVMSBool yielding)
{
    POVMSContextData *context = (POVMSContextData *)contextref;
    POVMSObject msg;
    POVMSObject result;
    POVMSAddress saddr = POVMSInvalidAddress;
    POVMSAddress daddr = POVMSInvalidAddress;
    POVMSStream *stream = NULL;
    POVMSLong resultid = 0;
    POVMSInt msgsize = 0;
    POVMSInt mode = kPOVMSSendMode_Invalid;
    POVMSInt resultsize = 0;
    POVMSInt objectcnt = 0;
    POVMSInt totalsize = 0;
    POVMSInt datasize = 0;
    POVMSInt version = 0;
    char header[8];
    int err = kNoErr;
    int maxsize = 0;

    if(contextref == NULL)
        return kParamErr;
    if(context->valid == false)
        return kInvalidContextErr;
    if((context->result.type != kPOVMSType_Null) && (context->resultid != 0))
        return kOutOfSyncErr;

    stream = (POVMSStream *)POVMS_Sys_QueueReceive(context->queue, &maxsize, blocking, yielding);
    if((stream != NULL) && (maxsize > 16))
    {
        msg.type = kPOVMSType_Null;
        msg.size = 0;
        msg.root = NULL;

        result.type = kPOVMSType_Null;
        result.size = 0;
        result.root = NULL;

        datasize += POVMSStream_ReadString(header, stream, 8, &maxsize);               // header       8 byte
        if(!((header[0] == 'P') && (header[1] == 'O') && (header[2] == 'V') && (header[3] == 'R') &&
             (header[4] == 'A') && (header[5] == 'Y') && (header[6] == 'M') && (header[7] == 'S')))
            err = kCannotHandleDataErr;

        datasize += POVMSStream_ReadInt(&version, stream + datasize, &maxsize);        // version      4 byte
        if(version != 0x0351)
            err = kVersionErr;

        datasize += POVMSStream_ReadInt(&totalsize, stream + datasize, &maxsize);      // total size   4 byte
        if((totalsize - 16) != maxsize)
            err = kInvalidDataSizeErr;

        datasize += POVMSStream_ReadInt(&mode, stream + datasize, &maxsize);           // flags        4 byte

        datasize += POVMSStream_ReadInt(&objectcnt, stream + datasize, &maxsize);      // objects      4 byte

        datasize += POVMSStream_ReadInt(&msgsize, stream + datasize, &maxsize);        // object size  4 byte
        datasize += POVMSStream_Read(&msg, stream + datasize, &maxsize);               // message      x byte

        if(objectcnt == 2)
        {
            datasize += POVMSStream_ReadInt(&resultsize, stream + datasize, &maxsize); // object size  4 byte
            datasize += POVMSStream_Read(&result, stream + datasize, &maxsize);        // result       x byte
        }

        (void)POVMSUtil_GetLong(&msg, kPOVMSResultSequenceID, &resultid);

        if(err == kNoErr)
        {
            if((context->resultid == 0) || (context->resultid != resultid))
            {
                // set source and destination addresses
                if((objectcnt == 2) && (result.type != kPOVMSType_Null))
                {
                    if(err == kNoErr)
                    {
                        if((POVMSMsg_GetSourceAddress(&msg, &saddr) != kNoErr) || (saddr == POVMSInvalidAddress))
                            err = POVMSMsg_GetSourceAddress(&result, &saddr);
                        if(err == kNoErr)
                            err = POVMSMsg_SetDestinationAddress(&result, saddr);
                    }

                    if(err == kNoErr)
                    {
                        if((POVMSMsg_GetDestinationAddress(&msg, &daddr) != kNoErr) || (daddr == POVMSInvalidAddress))
                            err = POVMSMsg_GetDestinationAddress(&result, &daddr);
                        if(err == kNoErr)
                            (void)POVMSMsg_SetSourceAddress(&result, daddr);
                    }

                    if(err == kNoErr)
                        err = POVMSUtil_GetLong(&result, kPOVMSResultSequenceID, &resultid);
                }

                if(err == kNoErr)
                {
                    try
                    {
                        err = POVMS_Receive(context, &msg, &result, mode);
                    }
                    catch(...)
                    {
                        POVMSObject_Delete(&result);
                        POVMSObject_Delete(&msg);
                        POVMS_Sys_Free(stream);
                        throw;
                    }
                }

                if(((objectcnt == 2) && (result.type != kPOVMSType_Null)) && (err == kNoErr))
                {
                    if((objectcnt == 2) && (result.type != kPOVMSType_Null)) // enforce result sequence id
                        err = POVMSUtil_SetLong(&result, kPOVMSResultSequenceID, resultid);
                    if(err == kNoErr)
                        err = POVMS_Send(context, &result, NULL, kPOVMSSendMode_NoReply);
                }
                else
                    (void)POVMSObject_Delete(&result);

                (void)POVMSObject_Delete(&msg);

                POVMS_Sys_Free(stream);

                return kFalseErr;
            }
            else
            {
                context->result = msg;
                context->resultid = 0;

                POVMS_Sys_Free(stream);

                return kNoErr;
            }
        }
        else
            return kNoErr; // ignore all errors
    }

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_Send
*
* DESCRIPTION
*   Sends a message by adding it to a queue of message which will be handled
*   when POVMS_ProcessMessages is called. If the queue is full POVMS__Send
*   will not add the message. It will return the error code kQueueFullErr,
*   which is no fatal error - as soon as there is space available again in
*   the queue you can send the message.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMS_Send(POVMSContext contextref, POVMSObjectPtr msg, POVMSObjectPtr result, int mode)
{
    POVMSContextData *context = (POVMSContextData *)contextref;
    POVMSAddress addr = POVMSInvalidAddress;
    POVMSLong resultid = 0;
    int maxtime = kDefaultTimeout;
    int err = kNoErr;

    POVMS_LOG_OUTPUT("POVMS_Send");

    if(msg == NULL)
        err = kParamErr;
    else if(msg->type == kPOVMSType_LockedObject)
        err = kNotNowErr;
    else if(mode == kPOVMSSendMode_Invalid)
        err = kParamErr;
    else if((result != NULL) && (mode == kPOVMSSendMode_NoReply))
        err = kParamErr;
    else if((result != NULL) && (mode == kPOVMSSendMode_WantReceipt))
        err = kParamErr;
    else if((contextref == NULL) && (mode == kPOVMSSendMode_WantReceipt))
        err = kParamErr;
    else if(mode == kPOVMSSendMode_WaitReply)
    {
        if(result == NULL)
            err = kParamErr;
        if(contextref == NULL)
            err = kParamErr;
    }

    if(err == kNoErr)
    {
        if((POVMSMsg_GetSourceAddress(msg, &addr) != kNoErr) || (addr == POVMSInvalidAddress))
        {
            if(contextref == NULL)
                err = kParamErr;
            else
                err = POVMS_GetContextAddress(contextref, &addr);
            if(err == kNoErr)
                err = POVMSMsg_SetSourceAddress(msg, addr);
        }
    }

    if(err == kNoErr)
    {
        if(POVMSUtil_GetInt(msg, kPOVMSMessageTimeoutID, &maxtime) != kNoErr)
            maxtime = kDefaultTimeout; // kDefaultTimeout seconds is the default timeout
    }

    if(contextref != NULL)
    {
        if(err == kNoErr)
            err = POVMSUtil_SetLong(msg, kPOVMSMessageSequenceID, context->nextsequenceid);
        context->nextsequenceid++;

        if(mode == kPOVMSSendMode_WaitReply)
        {
            if(err == kNoErr)
            {
                if(POVMS_ASSERT(context->thread == POVMS_Sys_GetCurrentThread(), "POVMS_Send context not valid for this thread") == false)
                    err = kInvalidContextErr;
            }

            resultid = context->nextsequenceid;
            if(err == kNoErr)
                err = POVMSUtil_SetLong(result, kPOVMSResultSequenceID, context->nextsequenceid);
            context->nextsequenceid++;
        }
    }

    if(err == kNoErr)
        err = POVMSMsg_GetDestinationAddress(msg, &addr);

    if(err == kNoErr)
    {
        int msgsize = POVMSStream_Size(msg);
        int resultsize = POVMSStream_Size(result);
        int objectcnt = 0;
        int totalsize = 0;
        int datasize = 0;
        int maxsize = 0;

        if(result != NULL)
            objectcnt = 2;
        else
            objectcnt = 1;

        totalsize = 8 + 4 + 4 + 4 + 4 + 4 + msgsize;
        if(result != NULL)
            totalsize = totalsize + 4 + resultsize;
        maxsize = totalsize;

        POVMSStream *stream = (POVMSStream *)POVMS_Sys_Malloc(totalsize);

        if(stream != NULL)
        {
            datasize += POVMSStream_WriteString("POVRAYMS", stream, &maxsize);             // header       8 byte
            datasize += POVMSStream_WriteInt(0x0351, stream + datasize, &maxsize);         // version      4 byte
            datasize += POVMSStream_WriteInt(totalsize, stream + datasize, &maxsize);      // total size   4 byte
            datasize += POVMSStream_WriteInt(mode, stream + datasize, &maxsize);           // flags        4 byte
            datasize += POVMSStream_WriteInt(objectcnt, stream + datasize, &maxsize);      // objects      4 byte
            datasize += POVMSStream_WriteInt(msgsize, stream + datasize, &maxsize);        // object size  4 byte
            datasize += POVMSStream_Write(msg, stream + datasize, &maxsize);               // message      x byte
            if(result != NULL)
            {
                datasize += POVMSStream_WriteInt(resultsize, stream + datasize, &maxsize); // object size  4 byte
                datasize += POVMSStream_Write(result, stream + datasize, &maxsize);        // result       x byte
            }

            if(POVMS_Sys_QueueSend(POVMS_Sys_AddressToQueue(addr), stream, totalsize) != 0)
                err = kQueueFullErr;

            if(result != NULL)
            {
                POVMSType oldresulttype = context->result.type;
                POVMSLong oldresultid = context->resultid;
                POVMSLong t = POVMS_Sys_Timer();

                context->result.type = kPOVMSType_Null;
                context->resultid = resultid;

                while((context->resultid == resultid) && ((POVMS_Sys_Timer() - t) < maxtime))
                    (void)POVMS_ProcessMessages(context, true, true);

                if(context->resultid == 0)
                {
                    POVMSObject_Delete(result);
                    *result = context->result;
                }

                if((context->resultid == resultid) && (POVMS_Sys_Timer() - t) >= maxtime)
                {
                    err = kTimeoutErr;
                }

                context->result.type = oldresulttype;
                context->resultid = oldresultid;
            }
        }
        else
            err = kMemFullErr;
    }

#ifdef _DEBUG_POVMS_DUMP_MESSAGES_
    puts("");
    (void)POVMSObject_Dump(stdout, msg);
    puts("");
#endif

    (void)POVMSObject_Delete(msg);

    return err;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_Receive
*
* DESCRIPTION
*   Receive a message and dispatch it to the handler function. It returns
*   0 if everything is ok, kNotNowErr if the message object was locked,
*   kCannotHandleDataErr if no message receive handler for the message was
*   found and any other value is a return value of the receive handler
*   function.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMS_Receive(POVMSContext contextref, POVMSObjectPtr msg, POVMSObjectPtr result, int mode)
{
    POVMSContextData *context = (POVMSContextData *)contextref;
    POVMSReceiveHandlerNode *cur = NULL;
    POVMSType hclass, hid;
    int l = 0;
    int ret = kNoErr;

    POVMS_LOG_OUTPUT("POVMS_Receive");

    if(msg == NULL)
        return kParamErr;
    if(msg->type == kPOVMSType_LockedObject)
        return kNotNowErr;

    ret = POVMSUtil_GetType(msg, kPOVMSMessageClassID, &hclass);
    if(ret == kNoErr)
        ret = POVMSUtil_GetType(msg, kPOVMSMessageIdentID, &hid);
    if(ret == kNoErr)
    {
        for(cur = context->receivehandlerroot; cur != NULL; cur = cur->next)
        {
            if((cur->handledclass == hclass) && (cur->handledid == hid))
                break;
            else if((cur->handledclass == hclass) && (cur->handledid == kPOVMSType_WildCard))
                break;
        }

        if(cur == NULL)
            ret = kCannotHandleDataErr;
        else
        {
            if(cur->handler == NULL)
                ret = kNullPointerErr;
            else
            {
                ret = cur->handler(msg, result, mode, cur->handlerprivatedata);
                if(result != NULL)
                    (void)POVMSUtil_SetInt(result, kPOVMSMessageErrorID, ret);
            }
        }
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_AddReceiveHandlerNode
*
* DESCRIPTION
*   Create a new receive handler node and insert it into the thread nodes
*   list of receive handlers.
*
* CHANGES
*   -
*
******************************************************************************/

POVMSReceiveHandlerNode *POVMS_AddReceiveHandlerNode(POVMSContextData *context)
{
    POVMSReceiveHandlerNode *out = NULL;

    POVMS_LOG_OUTPUT("POVMS_AddReceiveHandlerNode");

    out = (POVMSReceiveHandlerNode *)POVMS_Sys_Malloc(sizeof(POVMSReceiveHandlerNode));
    if(POVMS_ASSERT(out != NULL, "POVMS_AddReceiveHandlerNode failed, out of memory") == false)
        return NULL;

    out->last = NULL;
    out->next = context->receivehandlerroot;
    out->handledclass = kPOVMSType_Null;
    out->handledid = kPOVMSType_Null;
    out->handler = NULL;

    context->receivehandlerroot = out;
    if(out->next != NULL)
        out->next->last = out;

    return out;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_RemoveReceiveHandlerNode
*
* DESCRIPTION
*   Removes a receive handler node from the thread nodes list of receive
*   handlers and frees the memory.
*
* CHANGES
*   -
*
******************************************************************************/

int POVMS_RemoveReceiveHandlerNode(POVMSContextData *context, POVMSReceiveHandlerNode *thn)
{
    POVMS_LOG_OUTPUT("POVMS_RemoveReceiveHandlerNode");

    if(thn == context->receivehandlerroot)
        context->receivehandlerroot = context->receivehandlerroot->next;

    if(thn->last != NULL)
        thn->last->next = thn->next;
    if(thn->next != NULL)
        thn->next->last = thn->last;

    POVMS_Sys_Free((void *)thn);

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_FindReceiveHandlerNode
*
* DESCRIPTION
*   Search for a receive handler node by handles message class and id in the
*   list of thread nodes.
*
* CHANGES
*   -
*
******************************************************************************/

POVMSReceiveHandlerNode *POVMS_FindReceiveHandlerNode(POVMSContextData *context, POVMSType hclass, POVMSType hid)
{
    POVMSReceiveHandlerNode *thn = NULL;

    POVMS_LOG_OUTPUT("POVMS_FindReceiveHandlerNode");

    for(thn = context->receivehandlerroot; thn != NULL; thn = thn->next)
    {
        if((thn->handledclass == hclass) && (thn->handledid == hid))
            break;
    }

    return thn;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_Init
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

void POVMSStream_Init()
{
#ifndef POVMS_NO_ORDERED_STREAM_DATA
    POVMSStream stream[8];
    POVMSInt data_int;
    POVMSLong data_long;
    POVMSIEEEFloat data_ieeefloat;
    POVMSType data_type;
    POVMSUCS2 data_ucs2;

    data_int = 16909060;
    SetPOVMSLong(&data_long, 16909060, 84281096);
    HexToPOVMSIEEEFloat(0x44663355u, data_ieeefloat); // 0x44663355u corresponds to 920.802063
    data_type = '1234';
    data_ucs2 = 258;

    stream[0] = 1;
    stream[1] = 2;
    stream[2] = 3;
    stream[3] = 4;
    stream[4] = 5;
    stream[5] = 6;
    stream[6] = 7;
    stream[7] = 8;

    POVMSStream_BuildOrderTable((POVMSStream *)&data_int, stream, POVMSStreamOrderTables.int_write, 4);
    POVMSStream_BuildOrderTable(stream, (POVMSStream *)&data_int, POVMSStreamOrderTables.int_read, 4);

    POVMSStream_BuildOrderTable((POVMSStream *)&data_long, stream, POVMSStreamOrderTables.long_write, 8);
    POVMSStream_BuildOrderTable(stream, (POVMSStream *)&data_long, POVMSStreamOrderTables.long_read, 8);

    stream[0] = 0x44;
    stream[1] = 0x66;
    stream[2] = 0x33;
    stream[3] = 0x55;

    POVMSStream_BuildOrderTable((POVMSStream *)&data_ieeefloat, stream, POVMSStreamOrderTables.float_write, 4);
    POVMSStream_BuildOrderTable(stream, (POVMSStream *)&data_ieeefloat, POVMSStreamOrderTables.float_read, 4);

    stream[0] = 0x31;
    stream[1] = 0x32;
    stream[2] = 0x33;
    stream[3] = 0x34;

    POVMSStream_BuildOrderTable((POVMSStream *)&data_type, stream, POVMSStreamOrderTables.type_write, 4);
    POVMSStream_BuildOrderTable(stream, (POVMSStream *)&data_type, POVMSStreamOrderTables.type_read, 4);

    stream[0] = 1;
    stream[1] = 2;

    POVMSStream_BuildOrderTable((POVMSStream *)&data_ucs2, stream, POVMSStreamOrderTables.ucs2_write, 2);
    POVMSStream_BuildOrderTable(stream, (POVMSStream *)&data_ucs2, POVMSStreamOrderTables.ucs2_read, 2);
#else
    // WARNING: The setup below makes cross-platform communication impossible! [trf]

    #define POVMS_INIT_ORDER4(d,o) d[o + 0] = o + 0; d[o + 1] = o + 1; d[o + 2] = o + 2; d[o + 3] = o + 3
    #define POVMS_INIT_ORDER2(d,o) d[o + 0] = o + 0; d[o + 1] = o + 1

    POVMS_INIT_ORDER4(POVMSStreamOrderTables.int_write, 0);
    POVMS_INIT_ORDER4(POVMSStreamOrderTables.int_read, 0);

    POVMS_INIT_ORDER4(POVMSStreamOrderTables.long_write, 0);
    POVMS_INIT_ORDER4(POVMSStreamOrderTables.long_write, 4);
    POVMS_INIT_ORDER4(POVMSStreamOrderTables.long_read, 0);
    POVMS_INIT_ORDER4(POVMSStreamOrderTables.long_read, 4);

    POVMS_INIT_ORDER4(POVMSStreamOrderTables.float_write, 0);
    POVMS_INIT_ORDER4(POVMSStreamOrderTables.float_read, 0);

    POVMS_INIT_ORDER4(POVMSStreamOrderTables.type_write, 0);
    POVMS_INIT_ORDER4(POVMSStreamOrderTables.type_read, 0);

    POVMS_INIT_ORDER2(POVMSStreamOrderTables.ucs2_write, 0);
    POVMS_INIT_ORDER2(POVMSStreamOrderTables.ucs2_read, 0);
#endif
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_BuildOrderTable
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

void POVMSStream_BuildOrderTable(POVMSStream *srcdata, POVMSStream *dstdata, int *order, int datasize)
{
    int searchindex;
    int byteindex;

    for(byteindex = 0; byteindex < datasize; byteindex++)
    {
        for(searchindex = 0; searchindex < datasize; searchindex++)
        {
            if(srcdata[byteindex] == dstdata[searchindex])
                order[byteindex] = searchindex;
        }
    }
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_ReadDataOrdered
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

void POVMSStream_ReadDataOrdered(POVMSStream *stream, POVMSStream *data, int *order, int datasize)
{
    int byteindex;

    for(byteindex = 0; byteindex < datasize; byteindex++)
        data[order[byteindex]] = stream[byteindex];
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_ReadDataUnordered
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

void POVMSStream_ReadDataUnordered(POVMSStream *stream, POVMSStream *data, int datasize)
{
    int byteindex;

    for(byteindex = 0; byteindex < datasize; byteindex++)
        data[byteindex] = stream[byteindex];
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_ReadString
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadString(char *data, POVMSStream *stream, int datasize, int *maxstreamsize)
{
    if(data == NULL)
        return 0;

    if(*maxstreamsize < datasize)
        return 0;

    POVMSStream_ReadDataUnordered(stream, (POVMSStream *)data, datasize);

    *maxstreamsize -= datasize;

    return datasize;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_ReadUCS2String
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadUCS2String(POVMSUCS2 *data, POVMSStream *stream, int datasize, int *maxstreamsize)
{
    int i = 0;

    if(data == NULL)
        return 0;

    if(*maxstreamsize < datasize)
        return 0;

    for(i = 0; i < datasize; i++)
        POVMSStream_ReadDataOrdered(&stream[i * 2], (POVMSStream *)(&data[i]), POVMSStreamOrderTables.ucs2_read, 2);

    *maxstreamsize -= (datasize * 2);

    return (datasize * 2);
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_ReadInt
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadInt(POVMSInt *data, POVMSStream *stream, int *maxstreamsize)
{
    if(data == NULL)
        return 0;

    if(*maxstreamsize < 4)
        return 0;

    POVMSStream_ReadDataOrdered(stream, (POVMSStream *)data, POVMSStreamOrderTables.int_read, 4);

    *maxstreamsize -= 4;

    return 4;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_ReadLong
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadLong(POVMSLong *data, POVMSStream *stream, int *maxstreamsize)
{
    if(data == NULL)
        return 0;

    if(*maxstreamsize < 8)
        return 0;

    POVMSStream_ReadDataOrdered(stream, (POVMSStream *)data, POVMSStreamOrderTables.long_read, 8);

    *maxstreamsize -= 8;

    return 8;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_ReadFloat
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadFloat(POVMSFloat *data, POVMSStream *stream, int *maxstreamsize)
{
    POVMSIEEEFloat ieee_data;

    if(data == NULL)
        return 0;

    if(*maxstreamsize < 4)
        return 0;

    POVMSStream_ReadDataOrdered(stream, (POVMSStream *)(&ieee_data), POVMSStreamOrderTables.float_read, 4);

    POVMSIEEEFloatToPOVMSFloat(ieee_data, *data);

    *maxstreamsize -= 4;

    return 4;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_ReadType
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_ReadType(POVMSType *data, POVMSStream *stream, int *maxstreamsize)
{
    if(data == NULL)
        return 0;

    *data = kPOVMSType_Null;

    if(*maxstreamsize < 4)
        return 0;

    POVMSStream_ReadDataOrdered(stream, (POVMSStream *)data, POVMSStreamOrderTables.type_read, 4);

    *maxstreamsize -= 4;

    return 4;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_Read
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_Read(struct POVMSData *data, POVMSStream *stream, int *maxstreamsize)
{
    int ret = 0;
    int cnt;

    if(data == NULL)
        return 0;

    ret += POVMSStream_ReadType(&(data->type), stream + ret, maxstreamsize);
    ret += POVMSStream_ReadInt(&(data->size), stream + ret, maxstreamsize);

    switch(data->type)
    {
        case kPOVMSType_Object:
        case kPOVMSType_LockedObject:
        case kPOVMSType_ResultObject:
            data->root = NULL;
            for(cnt = 0; cnt < data->size; cnt++)
            {
                POVMSNode *cur = (POVMSNode *)POVMS_Sys_Malloc(sizeof(POVMSNode));
                cur->last = NULL;
                cur->next = data->root;
                if(data->root != NULL)
                    data->root->last = cur;
                data->root = cur;
                ret += POVMSStream_ReadType(&(cur->key), stream + ret, maxstreamsize);
                ret += POVMSStream_Read(&(cur->data), stream + ret, maxstreamsize);
            }
            break;
        case kPOVMSType_List:
            data->ptr = (void *)POVMS_Sys_Malloc(sizeof(POVMSData) * data->size);
            for(cnt = 0; cnt < data->size; cnt++)
                ret += POVMSStream_Read(&(data->items[cnt]), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_CString:
            data->ptr = (void *)POVMS_Sys_Malloc(data->size + 1);
            ret += POVMSStream_ReadString((char *)(data->ptr), stream + ret, data->size, maxstreamsize);
            ((char *)(data->ptr))[data->size] = 0;
            data->size++;
            break;
        case kPOVMSType_UCS2String:
            data->ptr = (void *)POVMS_Sys_Malloc(data->size + 2);
            ret += POVMSStream_ReadUCS2String((POVMSUCS2 *)(data->ptr), stream + ret, data->size / 2, maxstreamsize);
            ((POVMSUCS2 *)(data->ptr))[data->size / 2] = 0;
            data->size += 2;
            break;
        case kPOVMSType_Int:
            data->size = sizeof(POVMSInt);
            data->ptr = (void *)POVMS_Sys_Malloc(data->size);
            ret += POVMSStream_ReadInt(((POVMSInt *)(data->ptr)), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_Long:
            data->size = sizeof(POVMSLong);
            data->ptr = (void *)POVMS_Sys_Malloc(data->size);
            ret += POVMSStream_ReadLong(((POVMSLong *)(data->ptr)), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_Float:
            data->size = sizeof(POVMSFloat);
            data->ptr = (void *)POVMS_Sys_Malloc(data->size);
            ret += POVMSStream_ReadFloat(((POVMSFloat *)(data->ptr)), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_Bool:
            data->size = 1;
            data->ptr = (void *)POVMS_Sys_Malloc(data->size);
            POVMSStream_ReadDataUnordered(stream + ret, (POVMSStream *)(data->ptr), data->size);
            ret += 1;
            break;
        case kPOVMSType_Type:
            data->size = sizeof(POVMSType);
            data->ptr = (void *)POVMS_Sys_Malloc(data->size);
            ret += POVMSStream_ReadType(((POVMSType *)(data->ptr)), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_VectorInt:
            data->ptr = (void *)POVMS_Sys_Malloc(sizeof(POVMSInt) * data->size);
            for(int i = 0; i < data->size; i++)
                ret += POVMSStream_ReadInt(&(((POVMSInt *)(data->ptr))[i]), stream + ret, maxstreamsize);
            data->size = sizeof(POVMSInt) * data->size;
            break;
        case kPOVMSType_VectorLong:
            data->ptr = (void *)POVMS_Sys_Malloc(sizeof(POVMSLong) * data->size);
            for(int i = 0; i < data->size; i++)
                ret += POVMSStream_ReadLong(&(((POVMSLong *)(data->ptr))[i]), stream + ret, maxstreamsize);
            data->size = sizeof(POVMSLong) * data->size;
            break;
        case kPOVMSType_VectorFloat:
            data->ptr = (void *)POVMS_Sys_Malloc(sizeof(POVMSFloat) * data->size);
            for(int i = 0; i < data->size; i++)
                ret += POVMSStream_ReadFloat(&(((POVMSFloat *)(data->ptr))[i]), stream + ret, maxstreamsize);
            data->size = sizeof(POVMSFloat) * data->size;
            break;
        case kPOVMSType_VectorType:
            data->ptr = (void *)POVMS_Sys_Malloc(sizeof(POVMSType) * data->size);
            for(int i = 0; i < data->size; i++)
                ret += POVMSStream_ReadType(&(((POVMSType *)(data->ptr))[i]), stream + ret, maxstreamsize);
            data->size = sizeof(POVMSType) * data->size;
            break;
        case kPOVMSType_Address:
            data->ptr = (void *)POVMS_Sys_Malloc(sizeof(POVMSAddress));
            ret += POVMS_Sys_AddressFromStream((POVMSAddress *)(data->ptr), stream + ret, data->size);
            data->size = sizeof(POVMSAddress);
            break;
        default:
            data->ptr = (void *)POVMS_Sys_Malloc(data->size);
            POVMSStream_ReadDataUnordered(stream + ret, (POVMSStream *)(data->ptr), data->size);
            ret += data->size;
            break;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_WriteDataOrdered
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

void POVMSStream_WriteDataOrdered(const POVMSStream *data, POVMSStream *stream, int *order, int datasize)
{
    int byteindex;

    for(byteindex = 0; byteindex < datasize; byteindex++)
        stream[order[byteindex]] = data[byteindex];
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_WriteDataUnordered
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

void POVMSStream_WriteDataUnordered(const POVMSStream *data, POVMSStream *stream, int datasize)
{
    int byteindex;

    for(byteindex = 0; byteindex < datasize; byteindex++)
        stream[byteindex] = data[byteindex];
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_WriteString
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteString(const char *data, POVMSStream *stream, int *maxstreamsize)
{
    int len = 0;

    if(data == NULL)
        return 0;

    len = (int)POVMS_Sys_Strlen(data);

    if(*maxstreamsize < len)
        return 0;

    POVMSStream_WriteDataUnordered(reinterpret_cast<const POVMSStream *>(data), stream , len);

    *maxstreamsize -= len;

    return len;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_WriteUCS2String
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteUCS2String(const POVMSUCS2 *data, POVMSStream *stream, int *maxstreamsize)
{
    int len = 0;
    int i = 0;

    if(data == NULL)
        return 0;

    len = (int)POVMS_Sys_UCS2Strlen(data);

    if(*maxstreamsize < len)
        return 0;

    for(i = 0; i < len; i++)
        POVMSStream_WriteDataOrdered(reinterpret_cast<const POVMSStream *>(&data[i]), &stream[i * 2], POVMSStreamOrderTables.ucs2_write, 2);

    *maxstreamsize -= (len * 2);

    return (len * 2);
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_WriteInt
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteInt(POVMSInt data, POVMSStream *stream, int *maxstreamsize)
{
    if(*maxstreamsize < 4)
        return 0;

    POVMSStream_WriteDataOrdered((POVMSStream *)(&data), stream, POVMSStreamOrderTables.int_write, 4);

    *maxstreamsize -= 4;

    return 4;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_WriteLong
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteLong(POVMSLong data, POVMSStream *stream, int *maxstreamsize)
{
    if(*maxstreamsize < 8)
        return 0;

    POVMSStream_WriteDataOrdered((POVMSStream *)(&data), stream, POVMSStreamOrderTables.long_write, 8);

    *maxstreamsize -= 8;

    return 8;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_WriteFloat
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteFloat(POVMSFloat data, POVMSStream *stream, int *maxstreamsize)
{
    POVMSIEEEFloat ieee_data;

    if(*maxstreamsize < 4)
        return 0;

    POVMSFloatToPOVMSIEEEFloat(data, ieee_data);

    POVMSStream_WriteDataOrdered((POVMSStream *)(&ieee_data), stream, POVMSStreamOrderTables.float_write, 4);

    *maxstreamsize -= 4;

    return 4;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_WriteType
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_WriteType(POVMSType data, POVMSStream *stream, int *maxstreamsize)
{
    if(*maxstreamsize < 4)
        return 0;

    POVMSStream_WriteDataOrdered((POVMSStream *)(&data), stream, POVMSStreamOrderTables.type_write, 4);

    *maxstreamsize -= 4;

    return 4;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_Write
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_Write(struct POVMSData *data, POVMSStream *stream, int *maxstreamsize)
{
    int ret = 0;
    int cnt ;
    POVMSNode *cur;

    if(data == NULL)
        return 0;

    ret += POVMSStream_WriteType(data->type, stream + ret, maxstreamsize);

    switch(data->type)
    {
        case kPOVMSType_Object:
        case kPOVMSType_ResultObject:
            ret += POVMSStream_WriteInt(data->size, stream + ret, maxstreamsize);
            for(cur = data->root; cur != NULL; cur = cur->next)
            {
                ret += POVMSStream_WriteType(cur->key, stream + ret, maxstreamsize);
                ret += POVMSStream_Write(&(cur->data), stream + ret, maxstreamsize);
            }
            break;
        case kPOVMSType_List:
            ret += POVMSStream_WriteInt(data->size, stream + ret, maxstreamsize);
            for(cnt = 0; cnt < data->size; cnt++)
                ret += POVMSStream_Write(&(data->items[cnt]), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_CString:
            ret += POVMSStream_WriteInt(data->size - 1, stream + ret, maxstreamsize);
            ret += POVMSStream_WriteString((char *)(data->ptr), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_UCS2String:
            ret += POVMSStream_WriteInt(data->size - 2, stream + ret, maxstreamsize);
            ret += POVMSStream_WriteUCS2String((POVMSUCS2 *)(data->ptr), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_Int:
            ret += POVMSStream_WriteInt(4, stream + ret, maxstreamsize);
            ret += POVMSStream_WriteInt(*((POVMSInt *)(data->ptr)), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_Long:
            ret += POVMSStream_WriteInt(8, stream + ret, maxstreamsize);
            ret += POVMSStream_WriteLong(*((POVMSLong *)(data->ptr)), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_Float:
            ret += POVMSStream_WriteInt(4, stream + ret, maxstreamsize);
            ret += POVMSStream_WriteFloat(*((POVMSFloat *)(data->ptr)), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_Bool:
            ret += POVMSStream_WriteInt(1, stream + ret, maxstreamsize);
            POVMSStream_WriteDataUnordered((POVMSStream *)(data->ptr), stream + ret, data->size);
            ret += 1;
            break;
        case kPOVMSType_Type:
            ret += POVMSStream_WriteInt(4, stream + ret, maxstreamsize);
            ret += POVMSStream_WriteType(*((POVMSType *)(data->ptr)), stream + ret, maxstreamsize);
            break;
        case kPOVMSType_VectorInt:
            ret += POVMSStream_WriteInt(data->size / 4, stream + ret, maxstreamsize);
            for(int i = 0; i < data->size / 4; i++)
                ret += POVMSStream_WriteInt(((POVMSInt *)(data->ptr))[i], stream + ret, maxstreamsize);
            break;
        case kPOVMSType_VectorLong:
            ret += POVMSStream_WriteInt(data->size / 8, stream + ret, maxstreamsize);
            for(int i = 0; i < data->size / 8; i++)
                ret += POVMSStream_WriteLong(((POVMSLong *)(data->ptr))[i], stream + ret, maxstreamsize);
            break;
        case kPOVMSType_VectorFloat:
            ret += POVMSStream_WriteInt(data->size / 4, stream + ret, maxstreamsize);
            for(int i = 0; i < data->size / 4; i++)
                ret += POVMSStream_WriteFloat(((POVMSFloat *)(data->ptr))[i], stream + ret, maxstreamsize);
            break;
        case kPOVMSType_VectorType:
            ret += POVMSStream_WriteInt(data->size / 4, stream + ret, maxstreamsize);
            for(int i = 0; i < data->size / 4; i++)
                ret += POVMSStream_WriteType(((POVMSType *)(data->ptr))[i], stream + ret, maxstreamsize);
            break;
        case kPOVMSType_Address:
            ret += POVMSStream_WriteInt(POVMS_Sys_AddressToStreamSize(*((POVMSAddress *)(data->ptr))), stream + ret, maxstreamsize);
            ret += POVMS_Sys_AddressToStream(*((POVMSAddress *)(data->ptr)), stream + ret, maxstreamsize);
            break;
        default:
            ret += POVMSStream_WriteInt(data->size, stream + ret, maxstreamsize);
            POVMSStream_WriteDataUnordered((POVMSStream *)(data->ptr), stream + ret, data->size);
            ret += data->size;
            break;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_Size
*
* DESCRIPTION
*   -
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_Size(struct POVMSData *data)
{
    int ret = 0;
    int cnt;
    POVMSNode *cur;

    if(data == NULL)
        return 0;

    ret += 8;

    switch(data->type)
    {
        case kPOVMSType_Object:
        case kPOVMSType_LockedObject:
        case kPOVMSType_ResultObject:
            for(cur = data->root; (cur != NULL) && (ret > 0); cur = cur->next)
            {
                ret += 4;
                ret += POVMSStream_Size(&(cur->data));
            }
            break;
        case kPOVMSType_List:
            for(cnt = 0; (cnt < data->size) && (ret > 0); cnt++)
                ret += POVMSStream_Size(&(data->items[cnt]));
            break;
        case kPOVMSType_CString:
            ret += (int)POVMS_Sys_Strlen((char *)(data->ptr));
            break;
        case kPOVMSType_UCS2String:
            ret += (int)POVMS_Sys_UCS2Strlen((POVMSUCS2 *)(data->ptr)) * 2;
            break;
        case kPOVMSType_Int:
            ret += 4;
            break;
        case kPOVMSType_Long:
            ret += 8;
            break;
        case kPOVMSType_Float:
            ret += 4;
            break;
        case kPOVMSType_Bool:
            ret += 1;
            break;
        case kPOVMSType_Type:
            ret += 4;
            break;
        case kPOVMSType_VectorInt:
            ret += data->size / sizeof(POVMSInt) * 4;
            break;
        case kPOVMSType_VectorLong:
            ret += data->size / sizeof(POVMSLong) * 8;
            break;
        case kPOVMSType_VectorFloat:
            ret += data->size / sizeof(POVMSFloat) * 4;
            break;
        case kPOVMSType_VectorType:
            ret += data->size / sizeof(POVMSType) * 4;
            break;
        case kPOVMSType_Address:
            ret += POVMS_Sys_AddressToStreamSize(*((POVMSAddress *)(data->ptr)));
            break;
        default:
            ret += data->size;
            break;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_Dump
*
* DESCRIPTION
*   Write the complete given object to the given file.
*
* CHANGES
*   -
*
******************************************************************************/

#ifndef POVMS_NO_DUMP_SUPPORT

int POVMSStream_Dump(FILE *file, POVMSStream *stream, int datasize)
{
    int ii, ti;

    for(int i = 0; i < datasize; i += 16)
    {
        for(ii = 0, ti = i; ii < 16; ii++, ti++)
        {
            if(ti >= datasize)
                fprintf(file, "  ");
            else
                fprintf(file, "%02x", (int)stream[ti]);

            if((ii == 3) || (ii == 7) || (ii == 11))
                fprintf(file, " ");
        }

        fprintf(file, "  ");

        for(ii = 0, ti = i; ii < 16; ii++, ti++)
        {
            if(ti >= datasize)
                fprintf(file, " ");
            else if(((int)stream[ti] < 32) || ((int)stream[ti] >= 128))
                fprintf(file, ".");
            else
                fprintf(file, "%c", (int)stream[ti]);
        }

        fprintf(file, "\n");
    }

    return kNoErr;
}

#endif


/*****************************************************************************
*
* FUNCTION
*   POVMSStream_CheckMessageHeader
*
* DESCRIPTION
*   Check a stream if it contains a valid message header. It returns 0 if
*   everything is ok and kIncompleteDataErr if there was not enough data to
*   determine if the header was valid (that is, the header was not complete,
*   which currently implies there were less than 16 bytes). It either returns
*   kCannotHandleDataErr, kVersionErr or kInvalidDataSizeErr if the header
*   was invalid. It returns kParamErr if the stream or totalsize pointers
*   were invalid. It returns 0 (kNoErr) and the total expected size of the
*   message in totalsize (including streamsize!). Use this function when
*   receiving message data over a streamed communication mechanism (for
*   example a TCP/IP stream or a pipe). Read as many bytes as returned by
*   totalsize and then insert a message of that size into your local message
*   queue for processing by POVMS. In turn POVMS_ProcessMessages can then
*   process the message in the queue.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSStream_CheckMessageHeader(POVMSStream *stream, int streamsize, int *totalsize)
{
    char header[8];
    int version = 0;
    int err = kNoErr;

    if((stream == NULL) || (totalsize == NULL))
        return kParamErr;

    *totalsize = 0;

    if(streamsize >= 16)
    {
        int datasize = 0;

        datasize = POVMSStream_ReadString(header, stream, 8, &streamsize);             // header       8 byte
        if(!((header[0] == 'P') && (header[1] == 'O') && (header[2] == 'V') && (header[3] == 'R') &&
             (header[4] == 'A') && (header[5] == 'Y') && (header[6] == 'M') && (header[7] == 'S')))
            err = kCannotHandleDataErr;

        datasize += POVMSStream_ReadInt(&version, stream + datasize, &streamsize);     // version      4 byte
        if(version != 0x0351)
            err = kVersionErr;

        datasize += POVMSStream_ReadInt(totalsize, stream + datasize, &streamsize);    // total size   4 byte
        if(*totalsize < 16)
            err = kInvalidDataSizeErr;
    }
    else if(streamsize < 16)
        err = kIncompleteDataErr;

    return err;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSMsg_SetupMessage
*
* DESCRIPTION
*   Sets the message class and identifier for a given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSMsg_SetupMessage(POVMSObjectPtr object, POVMSType msgclass, POVMSType msgid)
{
    POVMSAddress addr = POVMSInvalidAddress;
    int ret;

    POVMS_LOG_OUTPUT("POVMSMsg_SetupMessage");

    ret = POVMSMsg_SetMessageClass(object, msgclass);
    if(ret == kNoErr)
        ret = POVMSMsg_SetMessageIdentifier(object, msgid);
    if((ret == kNoErr) && (POVMSObject_Exist(object, kPOVMSSourceAddressID) != kNoErr))
        ret = POVMSMsg_SetSourceAddress(object, addr);
    if((ret == kNoErr) && (POVMSObject_Exist(object, kPOVMSDestinationAddressID) != kNoErr))
        ret = POVMSMsg_SetDestinationAddress(object, addr);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSMsg_GetMessageClass
*
* DESCRIPTION
*   Gets the message class for a given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSMsg_GetMessageClass(POVMSObjectPtr object, POVMSType *msgclass)
{
    POVMS_LOG_OUTPUT("POVMSMsg_GetMessageClass");

    if(object == NULL)
        return kParamErr;

    if(msgclass == NULL)
        return kParamErr;

    return POVMSUtil_GetType(object, kPOVMSMessageClassID, msgclass);
}


/*****************************************************************************
*
* FUNCTION
*   POVMSMsg_SetMessageClass
*
* DESCRIPTION
*   Sets the message class for a given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSMsg_SetMessageClass(POVMSObjectPtr object, POVMSType msgclass)
{
    POVMS_LOG_OUTPUT("POVMSMsg_SetMessageClass");

    if(object == NULL)
        return kParamErr;

    return POVMSUtil_SetType(object, kPOVMSMessageClassID, msgclass);
}


/*****************************************************************************
*
* FUNCTION
*   POVMSMsg_GetMessageIdentifier
*
* DESCRIPTION
*   Gets the message identifier for a given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSMsg_GetMessageIdentifier(POVMSObjectPtr object, POVMSType *msgid)
{
    POVMS_LOG_OUTPUT("POVMSMsg_GetMessageIdentifier");

    if(object == NULL)
        return kParamErr;

    if(msgid == NULL)
        return kParamErr;

    return POVMSUtil_GetType(object, kPOVMSMessageIdentID, msgid);
}


/*****************************************************************************
*
* FUNCTION
*   POVMSMsg_SetMessageIdentifier
*
* DESCRIPTION
*   Sets the message identifier for a given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSMsg_SetMessageIdentifier(POVMSObjectPtr object, POVMSType msgid)
{
    POVMS_LOG_OUTPUT("POVMSMsg_SetMessageIdentifier");

    if(object == NULL)
        return kParamErr;

    return POVMSUtil_SetType(object, kPOVMSMessageIdentID, msgid);
}


/*****************************************************************************
*
* FUNCTION
*   POVMSMsg_GetSourceAddress
*
* DESCRIPTION
*   Gets the message source.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSMsg_GetSourceAddress(POVMSObjectPtr object, POVMSAddress *value)
{
    POVMSAttribute attr;
    int l = sizeof(POVMSAddress);
    int ret,temp_ret;

    if(value == NULL)
        return -1;

    ret = POVMSObject_Get(object, &attr, kPOVMSSourceAddressID);
    if(ret == 0)
    {
        ret = POVMSAttr_Get(&attr, kPOVMSType_Address, (void *)value, &l);
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == 0)
            ret = temp_ret;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSMsg_SetSourceAddress
*
* DESCRIPTION
*   Sets the message source.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSMsg_SetSourceAddress(POVMSObjectPtr object, POVMSAddress value)
{
    POVMSAttribute attr;
    int ret;

    if(object == NULL)
        return -1;

    ret = POVMSAttr_New(&attr);
    if(ret == 0)
        ret = POVMSAttr_Set(&attr, kPOVMSType_Address, (void *)(&value), sizeof(POVMSAddress));
    if(ret == 0)
        ret = POVMSObject_Set(object, &attr, kPOVMSSourceAddressID);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSMsg_GetDestinationAddress
*
* DESCRIPTION
*   Gets the message destination.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSMsg_GetDestinationAddress(POVMSObjectPtr object, POVMSAddress *value)
{
    POVMSAttribute attr;
    int l = sizeof(POVMSAddress);
    int ret,temp_ret;

    if(value == NULL)
        return -1;

    ret = POVMSObject_Get(object, &attr, kPOVMSDestinationAddressID);
    if(ret == 0)
    {
        ret = POVMSAttr_Get(&attr, kPOVMSType_Address, (void *)value, &l);
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == 0)
            ret = temp_ret;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSMsg_SetDestinationAddress
*
* DESCRIPTION
*   Sets the message destination.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSMsg_SetDestinationAddress(POVMSObjectPtr object, POVMSAddress value)
{
    POVMSAttribute attr;
    int ret;

    if(object == NULL)
        return -1;

    ret = POVMSAttr_New(&attr);
    if(ret == 0)
        ret = POVMSAttr_Set(&attr, kPOVMSType_Address, (void *)(&value), sizeof(POVMSAddress));
    if(ret == 0)
        ret = POVMSObject_Set(object, &attr, kPOVMSDestinationAddressID);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_New
*
* DESCRIPTION
*   Setup of a new and empty object.
*   Remember that no data for the POVMSObject is allocated.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObject_New(POVMSObjectPtr object, POVMSType objclass)
{
    POVMS_LOG_OUTPUT("POVMSObject_New");

    if(object == NULL)
        return kParamErr;

    object->type = kPOVMSType_LockedObject;

    object->size = 0;
    object->root = NULL;

    object->type = kPOVMSType_Object;

    return POVMSUtil_SetType(object, kPOVMSObjectClassID, objclass);
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_Delete
*
* DESCRIPTION
*   Deletes the given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObject_Delete(POVMSObjectPtr object)
{
    POVMSNode *cur,*del;

    POVMS_LOG_OUTPUT("POVMSObject_Delete");

    if(object == NULL)
        return kParamErr;
    if(object->type == kPOVMSType_LockedObject)
        return kNotNowErr;

    object->type = kPOVMSType_LockedObject;

    for(cur = object->root; cur != NULL;)
    {
        del = cur;
        cur = cur->next;
        POVMSAttr_Delete(&del->data);

        POVMS_Sys_Free((void *)del);
    }

    object->type = kPOVMSType_Object;
    object->size = 0;
    object->root = NULL;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_Copy
*
* DESCRIPTION
*   Copies the given object into the second given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObject_Copy(POVMSObjectPtr sourceobject, POVMSObjectPtr destobject)
{
    POVMSNode *cur = NULL;
    POVMSAttribute attr;
    POVMSType t;
    int ret = kNoErr;

    POVMS_LOG_OUTPUT("POVMSObject_Copy");

    if(sourceobject == NULL)
        return kParamErr;
    if(destobject == NULL)
        return kParamErr;
    if(sourceobject == destobject)
        return kParamErr;

    if(POVMSUtil_GetType(sourceobject, kPOVMSObjectClassID, &t) != kNoErr)
        return kObjectAccessErr;

    if(POVMSObject_New(destobject, t) != kNoErr)
        return kObjectAccessErr;

    for(cur = sourceobject->root; cur != NULL; cur = cur->next)
    {
        if(POVMS_ASSERT(POVMSAttr_Copy(&(cur->data), &attr) == kNoErr, "POVMSObject_Copy failed, out of memory") == false)
        {
            ret = kOutOfMemoryErr;
            break;
        }

        if(POVMS_ASSERT(POVMSObject_Set(destobject, &attr, cur->key) == kNoErr, "POVMSObject_Copy failed, out of memory") == false)
        {
            ret = kOutOfMemoryErr;
            break;
        }
    }

    if(ret != kNoErr)
        (void)POVMSObject_Delete(destobject);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_Merge
*
* DESCRIPTION
*   Adds the attributes of the given object to the second given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObject_Merge(POVMSObjectPtr sourceobject, POVMSObjectPtr destobject)
{
    POVMSNode *cur = NULL;
    POVMSAttribute attr;
    POVMSType t1, t2;
    int ret = kNoErr;

    POVMS_LOG_OUTPUT("POVMSObject_Merge");

    if(sourceobject == NULL)
        return kParamErr;
    if(destobject == NULL)
        return kParamErr;

    if(POVMSUtil_GetType(sourceobject, kPOVMSObjectClassID, &t1) != kNoErr)
        return kObjectAccessErr;
    if(POVMSUtil_GetType(sourceobject, kPOVMSObjectClassID, &t2) != kNoErr)
        return kObjectAccessErr;
    if(t1 != t2)
        return kDataTypeErr;

    for(cur = sourceobject->root; cur != NULL; cur = cur->next)
    {
        if(POVMS_ASSERT(POVMSAttr_Copy(&(cur->data), &attr) == kNoErr, "POVMSObject_Merge failed, out of memory") == false)
        {
            ret = kOutOfMemoryErr;
            break;
        }

        if(POVMS_ASSERT(POVMSObject_Set(destobject, &attr, cur->key) == kNoErr, "POVMSObject_Merge failed, out of memory") == false)
        {
            ret = kOutOfMemoryErr;
            break;
        }
    }

    return ret;
}

/*****************************************************************************
*
* FUNCTION
*   POVMSObject_Get
*
* DESCRIPTION
*   Gets the attribute with the given key from the given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObject_Get(POVMSObjectPtr object, POVMSAttributePtr attr, POVMSType key)
{
    POVMSNode *in = NULL;
    int ret;

    POVMS_LOG_OUTPUT("POVMSObject_Get");

    if(object == NULL)
        return kNoErr;
    if(object->type == kPOVMSType_LockedObject)
        return kNotNowErr;
    if(attr == NULL)
        return kParamErr;

    object->type = kPOVMSType_LockedObject;

    in = POVMSObject_Find(object, key);
    if(in == NULL)
        ret = kParamErr;
    else
        ret = POVMSAttr_Copy(&(in->data), attr);

    object->type = kPOVMSType_Object;

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_Set
*
* DESCRIPTION
*   Sets the attribute with the given key of the given object. If the
*   attribute is not yet part of the object it will be added.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObject_Set(POVMSObjectPtr object, POVMSAttributePtr attr, POVMSType key)
{
    POVMSNode *in = NULL;
    int ret;

    POVMS_LOG_OUTPUT("POVMSObject_Set");

    if(object == NULL)
        return kParamErr;
    if(object->type == kPOVMSType_LockedObject)
        return kNotNowErr;
    if(attr == NULL)
        return kParamErr;

    object->type = kPOVMSType_LockedObject;

    in = POVMSObject_Find(object, key);
    if(in != NULL)
    {
        POVMSAttr_Delete(&in->data);

        in->data = *attr;

        ret = kNoErr;
    }
    else
    {
        in = (POVMSNode *)POVMS_Sys_Malloc(sizeof(POVMSNode));
        if(POVMS_ASSERT(in != NULL, "POVMSObject_Set failed, out of memory") == false)
            ret = kOutOfMemoryErr;
        else
        {
            in->last = NULL;
            in->next = object->root;
            in->data = *attr;
            in->key = key;
            if(in->next != NULL)
                in->next->last = in;

            object->root = in;

            object->size++;

            ret = kNoErr;
        }
    }

    object->type = kPOVMSType_Object;

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_Remove
*
* DESCRIPTION
*   Removes the attribute with the given key from the given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObject_Remove(POVMSObjectPtr object, POVMSType key)
{
    POVMSNode *del;
    int ret = kNoErr;

    POVMS_LOG_OUTPUT("POVMSObject_Remove");

    if(object == NULL)
        return kParamErr;
    if(object->type == kPOVMSType_LockedObject)
        return kNotNowErr;

    object->type = kPOVMSType_LockedObject;

    del = POVMSObject_Find(object, key);
    if(del == NULL)
        ret = kParamErr;
    else
    {
        if(del->data.ptr != NULL)
            POVMSAttr_Delete(&del->data);

        if(del == object->root)
            object->root = object->root->next;
        if(del->last != NULL)
            del->last->next = del->next;
        if(del->next != NULL)
            del->next->last = del->last;

        object->size--;

        POVMS_Sys_Free((void *)del);
    }

    object->type = kPOVMSType_Object;

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_Exist
*
* DESCRIPTION
*   Determines if an attribute with the specified key is part of the given
*   object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObject_Exist(POVMSObjectPtr object, POVMSType key)
{
    int ret = kFalseErr;

    POVMS_LOG_OUTPUT("POVMSObject_Exist");

    if(object == NULL)
        return kParamErr;
    if(object->type == kPOVMSType_LockedObject)
        return kNotNowErr;

    object->type = kPOVMSType_LockedObject;

    if(POVMSObject_Find(object, key) != NULL)
        ret = kNoErr;
    else
        ret = kFalseErr;

    object->type = kPOVMSType_Object;

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_Count
*
* DESCRIPTION
*   Counts the attributes of the given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObject_Count(POVMSObjectPtr object, int *cnt)
{
    POVMS_LOG_OUTPUT("POVMSObject_Count");

    if(object == NULL)
        return kParamErr;
    if(object->type == kPOVMSType_LockedObject)
        return kNotNowErr;
    if(cnt == NULL)
        return kParamErr;

    object->type = kPOVMSType_LockedObject;

    *cnt = object->size;

    object->type = kPOVMSType_Object;

    return kNoErr;
}



/*****************************************************************************
*
* FUNCTION
*   POVMSObject_Find
*
* DESCRIPTION
*   Search for a given key in the given object and return a pointer to its node.
*
* CHANGES
*   -
*
******************************************************************************/

POVMSNode *POVMSObject_Find(POVMSObjectPtr object, POVMSType key)
{
    POVMSNode *cur = NULL;

    POVMS_LOG_OUTPUT("POVMSObject_Find");

    if(object == NULL)
        return NULL;

    for(cur = object->root; cur != NULL; cur = cur->next)
    {
        if(cur->key == key)
            return cur;
    }

    return NULL;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_Dump
*
* DESCRIPTION
*   Write the complete given object to the given file.
*
* CHANGES
*   -
*
******************************************************************************/

#ifndef POVMS_NO_DUMP_SUPPORT

static int gPOVMSDumpLevel = 0;

POVMS_EXPORT int POVMS_CDECL POVMSObject_Dump(FILE *file, POVMSObjectPtr object)
{
    POVMSNode *cur = NULL;

    POVMS_LOG_OUTPUT("POVMSObject_Dump");

    if(file == NULL)
        return kParamErr;
    if(object == NULL)
        return kParamErr;

    POVMSObject_DumpSpace(file); fprintf(file, "Object ");

    fprintf(file, "%c%c%c%c\n", (char)((object->type) >> 24)
                              , (char)((object->type) >> 16)
                              , (char)((object->type) >> 8)
                              , (char)((object->type)));

    POVMSObject_DumpSpace(file); fprintf(file, "{\n");

    gPOVMSDumpLevel++;

    for(cur = object->root; cur != NULL; cur = cur->next)
    {
        if((cur->data.type != kPOVMSType_Object) && (cur->data.type != kPOVMSType_LockedObject))
        {
            POVMSObject_DumpSpace(file);
            fprintf(file, "%c%c%c%c = ", (char)((cur->key) >> 24)
                                       , (char)((cur->key) >> 16)
                                       , (char)((cur->key) >> 8)
                                       , (char)((cur->key)));

            (void)POVMSObject_DumpAttr(file, &(cur->data));
        }
        else
            (void)POVMSObject_Dump(file, &(cur->data));
    }

    gPOVMSDumpLevel--;

    POVMSObject_DumpSpace(file); fprintf(file, "}\n");

    return kNoErr;
}

#endif


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_DumpSpace
*
* DESCRIPTION
*   Write the given number of spaces to the given file.
*
* CHANGES
*   -
*
******************************************************************************/

#ifndef POVMS_NO_DUMP_SUPPORT

int POVMSObject_DumpSpace(FILE *file)
{
    int i;

    for(i = 0; i < gPOVMSDumpLevel; i++)
        fprintf(file, "  ");

    return kNoErr;
}

#endif


/*****************************************************************************
*
* FUNCTION
*   POVMSObject_DumpAttr
*
* DESCRIPTION
*   Write the given attribute to the given file.
*
* CHANGES
*   -
*
******************************************************************************/

#ifndef POVMS_NO_DUMP_SUPPORT

POVMS_EXPORT int POVMS_CDECL POVMSObject_DumpAttr(FILE *file, POVMSAttributePtr attr)
{
    POVMSNode *cur = NULL;
    int cnt;

    if(file == NULL)
        return kParamErr;
    if(attr == NULL)
        return kParamErr;

    fprintf(file, "(%c%c%c%c) ", (char)((attr->type) >> 24)
                               , (char)((attr->type) >> 16)
                               , (char)((attr->type) >> 8)
                               , (char)((attr->type)));

    switch(attr->type)
    {
        case kPOVMSType_List:
            fprintf(file, "List\n");
            POVMSObject_DumpSpace(file); fprintf(file, "{\n");

            gPOVMSDumpLevel++;

            for(cnt = 0; cnt < attr->size; cnt++)
            {
                if((attr->items[cnt].type != kPOVMSType_Object) && (attr->items[cnt].type != kPOVMSType_LockedObject))
                {
                    POVMSObject_DumpSpace(file);
                    (void)POVMSObject_DumpAttr(file, &(attr->items[cnt]));
                }
                else
                    (void)POVMSObject_Dump(file, &(attr->items[cnt]));
            }

            gPOVMSDumpLevel--;

            POVMSObject_DumpSpace(file); fprintf(file, "}\n");
            break;
        case kPOVMSType_CString:
            fprintf(file, "\"%s\"\n", (char *)(attr->ptr));
            break;
        case kPOVMSType_Int:
            fprintf(file, "%d\n", (int)*((POVMSInt *)(attr->ptr)));
            break;
        case kPOVMSType_Long:
            unsigned int l;
            int h;

            GetPOVMSLong(&h, &l, (*((POVMSLong *)(attr->ptr))));
            fprintf(file, "%.8x%.8x\n", h, l);
            break;
        case kPOVMSType_Float:
            fprintf(file, "%f\n", (float)*((POVMSFloat *)(attr->ptr)));
            break;
        case kPOVMSType_Bool:
            if((*((unsigned char *)(attr->ptr))) != 0x00)
                fprintf(file, "true\n");
            else
                fprintf(file, "false\n");
            break;
        case kPOVMSType_Type:
            fprintf(file, "\'%c%c%c%c\'\n", (char)((*((unsigned int *)(attr->ptr))) >> 24)
                                          , (char)((*((unsigned int *)(attr->ptr))) >> 16)
                                          , (char)((*((unsigned int *)(attr->ptr))) >> 8)
                                          , (char)((*((unsigned int *)(attr->ptr)))));
            break;
        default:
            fprintf(file, "[cannot dump data]\n");
            break;
    }

    return kNoErr;
}

#endif


/*****************************************************************************
*
* FUNCTION
*   POVMSObjectStream_Size
*
* DESCRIPTION
*   Computes the streamed size of the given object.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObjectStream_Size(POVMSObjectPtr object, int *streamsize)
{
    POVMS_LOG_OUTPUT("POVMSObject_StreamSize");

    if(object == NULL)
        return kParamErr;
    if(object->type == kPOVMSType_LockedObject)
        return kNotNowErr;
    if(streamsize == NULL)
        return kParamErr;

    *streamsize = POVMSStream_Size(object);

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObjectStream_Read
*
* DESCRIPTION
*   Reads an object from a stream. The object is created by this function!
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObjectStream_Read(POVMSObjectPtr object, POVMSStream *stream, int *maxstreamsize)
{
    POVMS_LOG_OUTPUT("POVMSObject_StreamRead");

    if(object == NULL)
        return kParamErr;
    if(object->type == kPOVMSType_LockedObject)
        return kNotNowErr;
    if(stream == NULL)
        return kParamErr;
    if(maxstreamsize == NULL)
        return kParamErr;

    if(POVMSStream_Read(object, stream, maxstreamsize) == 0)
        return kParamErr;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSObjectStream_Write
*
* DESCRIPTION
*   Write the object to the stream. The object is not deleted by this function!
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSObjectStream_Write(POVMSObjectPtr object, POVMSStream *stream, int *maxstreamsize)
{
    POVMS_LOG_OUTPUT("POVMSObject_StreamWrite");

    if(object == NULL)
        return kParamErr;
    if(object->type == kPOVMSType_LockedObject)
        return kNotNowErr;
    if(stream == NULL)
        return kParamErr;
    if(maxstreamsize == NULL)
        return kParamErr;

    if(POVMSStream_Write(object, stream, maxstreamsize) == 0)
        return kParamErr;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttr_New
*
* DESCRIPTION
*   Setup of a new and empty attribute.
*   Remember that no data for the POVMSAttribute is allocated.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttr_New(POVMSAttributePtr attr)
{
    POVMS_LOG_OUTPUT("POVMSAttr_New");

    if(attr == NULL)
        return kParamErr;

    attr->type = kPOVMSType_Null;
    attr->size = 0;
    attr->ptr = NULL;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttr_Delete
*
* DESCRIPTION
*   Deletes the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttr_Delete(POVMSAttributePtr attr)
{
    POVMS_LOG_OUTPUT("POVMSAttr_Delete");

    if(attr == NULL)
        return kParamErr;

    if(attr->type == kPOVMSType_Object)
    {
        POVMSObject_Delete(attr);
    }
    else if(attr->type == kPOVMSType_List)
    {
        POVMSAttrList_Delete(attr);
    }
    else if(attr->type == kPOVMSType_Address)
    {
        // NetPOVMS_DeleteAddress((POVMSAddressPtr)(attr->ptr));
        POVMS_Sys_Free((void *)(attr->ptr));
    }
    else if(attr->ptr != NULL)
        POVMS_Sys_Free((void *)(attr->ptr));

    attr->type = kPOVMSType_Null;
    attr->size = 0;
    attr->ptr = NULL;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttr_Copy
*
* DESCRIPTION
*   Copies the given attribute into the second given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttr_Copy(POVMSAttributePtr sourceattr, POVMSAttributePtr destattr)
{
    POVMS_LOG_OUTPUT("POVMSAttr_Copy");

    if(sourceattr == NULL)
        return kParamErr;
    if(destattr == NULL)
        return kParamErr;
    if(sourceattr == destattr)
        return kParamErr;
    if(sourceattr->size < 0)
        return kParamErr;

    if(sourceattr->type == kPOVMSType_Object)
    {
        return POVMSObject_Copy(sourceattr, destattr);
    }
    else if(sourceattr->type == kPOVMSType_List)
    {
        return POVMSAttrList_Copy(sourceattr, destattr);
    }
    else
    {
        *destattr = *sourceattr;

        if(sourceattr->ptr != NULL)
        {
            destattr->ptr =(void *)POVMS_Sys_Malloc(sourceattr->size);
            if(POVMS_ASSERT(destattr->ptr != NULL, "POVMSAttr_Copy failed, out of memory") == false)
                return kMemFullErr;

            POVMS_Sys_Memmove(destattr->ptr, sourceattr->ptr, sourceattr->size);

            //if(sourceattr->type == kPOVMSType_Address)
            //  NetPOVMS_CopyAddress((POVMSAddressPtr)(sourceattr->ptr), (POVMSAddressPtr)(destattr->ptr));
        }
    }

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttr_Get
*
* DESCRIPTION
*   Gets the data of the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttr_Get(POVMSAttributePtr attr, POVMSType type, void *data, int *maxdatasize)
{
    POVMS_LOG_OUTPUT("POVMSAttr_Get");

    if(attr == NULL)
        return kParamErr;
    if(data == NULL)
        return kParamErr;
    if(maxdatasize == NULL)
        return kParamErr;
    if(*maxdatasize < 0)
        return kParamErr;

    if(*maxdatasize < attr->size)
    {
        *maxdatasize = attr->size;
        return kInvalidDataSizeErr;
    }

    if(attr->type != type)
        return kDataTypeErr;

    POVMS_Sys_Memmove(data, attr->ptr, attr->size);
    *maxdatasize = attr->size;

    //if(attr->type == kPOVMSType_Address)
    //  return NetPOVMS_CopyAddress((POVMSAddressPtr)(attr->ptr), (POVMSAddressPtr)(data));

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttr_Set
*
* DESCRIPTION
*   Sets the data of the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttr_Set(POVMSAttributePtr attr, POVMSType type, const void *data, int datasize)
{
    POVMS_LOG_OUTPUT("POVMSAttr_Set");

    if(attr == NULL)
        return kParamErr;
    if(data == NULL)
        return kParamErr;
    if(datasize < 0)
        return kParamErr;
    if(attr->ptr != NULL)
        return kParamErr;
    if(attr->size != 0)
        return kParamErr;

    attr->ptr = (void *)POVMS_Sys_Malloc(datasize);
    if(POVMS_ASSERT(attr->ptr != NULL, "POVMSAttr_Set failed, out of memory") == false)
        return kMemFullErr;

    //if(attr->type == kPOVMSType_Address)
    //  (void)NetPOVMS_DeleteAddress((POVMSAddressPtr)(attr->ptr));

    POVMS_Sys_Memmove(attr->ptr, data, datasize);
    attr->type = type;
    attr->size = datasize;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttr_Size
*
* DESCRIPTION
*   Gets the data size of the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttr_Size(POVMSAttributePtr attr, int *size)
{
    POVMS_LOG_OUTPUT("POVMSAttr_Size");

    if(attr == NULL)
        return kParamErr;
    if(size == NULL)
        return kParamErr;

    *size = attr->size;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttr_Type
*
* DESCRIPTION
*   Gets the data type of the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttr_Type(POVMSAttributePtr attr, POVMSType *type)
{
    POVMS_LOG_OUTPUT("POVMSAttr_Type");

    if(attr == NULL)
        return kParamErr;
    if(type == NULL)
        return kParamErr;

    *type = attr->type;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_New
*
* DESCRIPTION
*   Setup of a new and empty attribute list. Remember that no data for the
*   POVMSAttribute is allocated.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_New(POVMSAttributeListPtr attr)
{
    POVMS_LOG_OUTPUT("POVMSAttrList_New");

    if(attr == NULL)
        return kParamErr;

    attr->type = kPOVMSType_List;
    attr->size = 0;
    attr->items = NULL;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_Delete
*
* DESCRIPTION
*   Deletes the given attribute list.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Delete(POVMSAttributeListPtr attr)
{
    int ret = kNoErr;

    POVMS_LOG_OUTPUT("POVMSAttrList_Delete");

    if(attr == NULL)
        return kParamErr;
    if(attr->items != NULL)
        ret = POVMSAttrList_Clear(attr);

    attr->type = kPOVMSType_Null;
    attr->size = 0;
    attr->items = NULL;

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_Delete
*
* DESCRIPTION
*   Deletes the given attribute list.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Copy(POVMSAttributeListPtr sourcelist, POVMSAttributeListPtr destlist)
{
    int cnt;
    int err = kNoErr;

    POVMS_LOG_OUTPUT("POVMSAttrList_Copy");

    if(sourcelist == NULL)
        return kParamErr;
    if(destlist == NULL)
        return kParamErr;
    if(sourcelist == destlist)
        return kParamErr;
    if(sourcelist->size < 0)
        return kParamErr;
    if(sourcelist->type != kPOVMSType_List)
        return kDataTypeErr;
    if((sourcelist->items == NULL) && (sourcelist->size > 0))
        return kParamErr;

    *destlist = *sourcelist;

    if(sourcelist->size > 0)
    {
        if(sourcelist->ptr != NULL)
        {
            destlist->ptr = (void *)POVMS_Sys_Malloc(sizeof(POVMSData) * sourcelist->size);
            if(POVMS_ASSERT(destlist->ptr != NULL, "POVMSAttrList_Copy failed, out of memory") == false)
                return -1;
        }

        POVMS_Sys_Memmove(destlist->ptr, sourcelist->ptr, sourcelist->size);

        for(cnt = 0; cnt < sourcelist->size; cnt++)
        {
            err = POVMSAttr_Copy(&(sourcelist->items[cnt]), &(destlist->items[cnt]));
            if(err != kNoErr)
                break;
        }
        if(err != kNoErr)
        {
            for(cnt--; cnt >= 0; cnt--)
            {
                err = POVMSAttr_Delete(&(destlist->items[cnt]));
                POVMS_ASSERT(err == kNoErr, "POVMSAttr_Delete in POVMSAttrList_Copy failed. Possible memory leak.");
            }
            POVMS_Sys_Free((void *)(destlist->items));
            err = kObjectAccessErr;
        }
    }

    return err;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_Append
*
* DESCRIPTION
*   Adds the data of the item to the end of the given attribute list.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Append(POVMSAttributeListPtr attr, POVMSAttributePtr item)
{
    POVMSData *temp_items;
    int err = kNoErr;

    POVMS_LOG_OUTPUT("POVMSAttrList_Append");

    if(attr == NULL)
        return kParamErr;
    if(attr->type != kPOVMSType_List)
        return kDataTypeErr;
    if(item == NULL)
        return kNoErr;

    temp_items = (POVMSData *)POVMS_Sys_Realloc((void *)(attr->items), sizeof(POVMSData) * (attr->size + 1));
    if(POVMS_ASSERT(temp_items != NULL, "POVMSAttrList_Append failed, out of memory") == false)
    {
        err = kNoErr;
    }
    else
    {
        attr->items = temp_items;
        attr->items[attr->size] = *item;
        attr->size++;
    }

    return err;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_AppendN
*
* DESCRIPTION
*   Adds the data of the item to the end of the given attribute list.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_AppendN(POVMSAttributeListPtr attr, int cnt, POVMSAttributePtr item)
{
    POVMSData *temp_items;
    int err = kNoErr;

    POVMS_LOG_OUTPUT("POVMSAttrList_AppendN");

    if(attr == NULL)
        return kParamErr;
    if(attr->type != kPOVMSType_List)
        return kDataTypeErr;
    if(item == NULL)
        return kNoErr;

    temp_items = (POVMSData *)POVMS_Sys_Realloc((void *)(attr->items), sizeof(POVMSData) * (attr->size + cnt));
    if(POVMS_ASSERT(temp_items != NULL, "POVMSAttrList_Append failed, out of memory") == false)
    {
        err = kNoErr;
    }
    else
    {
        attr->items = temp_items;
        attr->items[attr->size] = *item;
        attr->size += cnt;

        for(cnt = attr->size - cnt + 1; cnt < attr->size; cnt++)
        {
            err = POVMSAttr_Copy(item, &(attr->items[cnt]));
            if(err != kNoErr)
                break;
        }
        if(err != kNoErr)
        {
            for(cnt--; cnt >= 0; cnt--)
            {
                err = POVMSAttr_Delete(&(attr->items[cnt]));
                POVMS_ASSERT(err == kNoErr, "POVMSAttr_Delete in POVMSAttrList_AppendN failed. Possible memory leak.");
            }
            POVMS_Sys_Free((void *)(attr->items));
            err = kObjectAccessErr;
        }
    }

    return err;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_Remove
*
* DESCRIPTION
*   Removes the last item in the given attribute list.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Remove(POVMSAttributeListPtr attr)
{
    POVMS_LOG_OUTPUT("POVMSAttrList_Remove");

    if(attr == NULL)
        return kParamErr;
    if(attr->type != kPOVMSType_List)
        return kDataTypeErr;
    if(attr->size <= 0)
        return kParamErr;
    if(attr->items == NULL)
        return kParamErr;

    return POVMSAttrList_RemoveNth(attr, attr->size);
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_GetNth
*
* DESCRIPTION
*   Gets the data of the item with the given index in the given attribute list.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_GetNth(POVMSAttributeListPtr attr, int index, POVMSAttributePtr item)
{
    POVMS_LOG_OUTPUT("POVMSAttrList_GetNth");

    if(attr == NULL)
        return kParamErr;
    if(attr->type != kPOVMSType_List)
        return kDataTypeErr;
    if(item == NULL)
        return kParamErr;
    if(attr->items == NULL)
        return kParamErr;
    if(index < 1)
        return kParamErr;
    if(index > attr->size)
        return kParamErr;

    return POVMSAttr_Copy(&(attr->items[index - 1]), item);
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_SetNth
*
* DESCRIPTION
*   Sets the data of the item with the given index in the given attribute list.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_SetNth(POVMSAttributeListPtr attr, int index, POVMSAttributePtr item)
{
    int err;

    POVMS_LOG_OUTPUT("POVMSAttrList_SetNth");

    if(attr == NULL)
        return kParamErr;
    if(attr->type != kPOVMSType_List)
        return kDataTypeErr;
    if(item == NULL)
        return kParamErr;
    if(attr->items == NULL)
        return kParamErr;
    if(index < 1)
        return kParamErr;
    if(index > attr->size)
        return kParamErr;

    err = POVMSAttr_Delete(&(attr->items[index - 1]));
    if(err == kNoErr)
        attr->items[index - 1] = *item;

    return err;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_RemoveNth
*
* DESCRIPTION
*   Removes the item with the given index in the given attribute list.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_RemoveNth(POVMSAttributeListPtr attr, int index)
{
    POVMSData *temp_items;
    int err;

    POVMS_LOG_OUTPUT("POVMSAttrList_RemoveNth");

    if(attr == NULL)
        return kParamErr;
    if(attr->type != kPOVMSType_List)
        return kDataTypeErr;
    if(attr->items == NULL)
        return kParamErr;
    if(index < 1)
        return kParamErr;
    if(index > attr->size)
        return kParamErr;

    err = POVMSAttr_Delete(&(attr->items[index - 1]));
    if(err == kNoErr)
    {
        if(attr->size < index)
            POVMS_Sys_Memmove((void *)(&(attr->items[index - 1])), (void *)(&(attr->items[index])), sizeof(POVMSData) * (attr->size - index));
        temp_items = (POVMSData *)POVMS_Sys_Realloc((void *)(attr->items), sizeof(POVMSData) * (attr->size - 1));
        if(POVMS_ASSERT(temp_items != NULL, "POVMSAttrList_RemoveNth failed, out of memory") == false)
            err = kOutOfMemoryErr;
        else
            attr->items = temp_items;
        attr->size--;
    }

    return err;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_Clear
*
* DESCRIPTION
*   Clears the given attribute list.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Clear(POVMSAttributeListPtr attr)
{
    int cnt;
    int err;

    POVMS_LOG_OUTPUT("POVMSAttrList_Clear");

    if(attr == NULL)
        return kParamErr;
    if(attr->type != kPOVMSType_List)
        return kDataTypeErr;
    if(attr->items == NULL)
        return kParamErr;
    if(attr->size <= 0)
        return kParamErr;

    for(cnt = attr->size - 1; cnt >= 0; cnt--)
    {
        err = POVMSAttr_Delete(&(attr->items[cnt]));
        POVMS_ASSERT(err == kNoErr, "POVMSAttr_Delete in POVMSAttrList_Clear failed. Possible memory leak.");
    }

    if(attr->items != NULL)
        POVMS_Sys_Free((void *)(attr->items));

    attr->type = kPOVMSType_Null;
    attr->size = 0;
    attr->items = NULL;

    return kNoErr; // don't return err
}


/*****************************************************************************
*
* FUNCTION
*   POVMSAttrList_Count
*
* DESCRIPTION
*   Gets the number of items in the given attribute list.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSAttrList_Count(POVMSAttributeListPtr attr, int *cnt)
{
    POVMS_LOG_OUTPUT("POVMSAttrList_Count");

    if(attr == NULL)
        return kParamErr;
    if(attr->type != kPOVMSType_List)
        return kParamErr;
    if(cnt == NULL)
        return kParamErr;

    *cnt = attr->size;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_SetString
*
* DESCRIPTION
*   Stores a string (including terminating 0) in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetString(POVMSObjectPtr object, POVMSType key, const char *str) // Note: Strings may not contain \0 characters codes!
{
    POVMSAttribute attr;
    int ret;

    POVMS_LOG_OUTPUT("POVMSUtil_SetString");

    if(object == NULL)
        return kParamErr;
    if(str == NULL)
        return kParamErr;

    ret = POVMSAttr_New(&attr);
    if(ret == kNoErr)
        ret = POVMSAttr_Set(&attr, kPOVMSType_CString, reinterpret_cast<const void *>(str), (int)POVMS_Sys_Strlen(str) + 1);
    if(ret == kNoErr)
        ret = POVMSObject_Set(object, &attr, key);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_SetUCS2String
*
* DESCRIPTION
*   Stores a string (including terminating 0) in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetUCS2String(POVMSObjectPtr object, POVMSType key, const POVMSUCS2 *str) // Note: Strings may not contain \0 characters codes!
{
    POVMSAttribute attr;
    int ret;

    POVMS_LOG_OUTPUT("POVMSUtil_SetUCS2String");

    if(object == NULL)
        return kParamErr;
    if(str == NULL)
        return kParamErr;

    ret = POVMSAttr_New(&attr);
    if(ret == kNoErr)
        ret = POVMSAttr_Set(&attr, kPOVMSType_UCS2String, reinterpret_cast<const void *>(str), (int)(POVMS_Sys_UCS2Strlen(reinterpret_cast<const POVMSUCS2 *>(str)) + 1)*sizeof(POVMSUCS2));
    if(ret == kNoErr)
        ret = POVMSObject_Set(object, &attr, key);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_SetInt
*
* DESCRIPTION
*  Stores an integer value in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetInt(POVMSObjectPtr object, POVMSType key, POVMSInt value)
{
    POVMSAttribute attr;
    int ret;

    POVMS_LOG_OUTPUT("POVMSUtil_SetInt");

    if(object == NULL)
        return kParamErr;

    ret = POVMSAttr_New(&attr);
    if(ret == kNoErr)
        ret = POVMSAttr_Set(&attr, kPOVMSType_Int, (void *)(&value), sizeof(POVMSInt));
    if(ret == kNoErr)
        ret = POVMSObject_Set(object, &attr, key);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_SetLong
*
* DESCRIPTION
*  Stores a long integer value in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetLong(POVMSObjectPtr object, POVMSType key, POVMSLong value)
{
    POVMSAttribute attr;
    int ret;

    POVMS_LOG_OUTPUT("POVMSUtil_SetLong");

    if(object == NULL)
        return kParamErr;

    ret = POVMSAttr_New(&attr);
    if(ret == kNoErr)
        ret = POVMSAttr_Set(&attr, kPOVMSType_Long, (void *)(&value), sizeof(POVMSLong));
    if(ret == kNoErr)
        ret = POVMSObject_Set(object, &attr, key);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_SetFloat
*
* DESCRIPTION
*   Stores a float value in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetFloat(POVMSObjectPtr object, POVMSType key, POVMSFloat value)
{
    POVMSAttribute attr;
    int ret;

    POVMS_LOG_OUTPUT("POVMSUtil_SetFloat");

    if(object == NULL)
        return kParamErr;

    ret = POVMSAttr_New(&attr);
    if(ret == kNoErr)
        ret = POVMSAttr_Set(&attr, kPOVMSType_Float, (void *)(&value), sizeof(POVMSFloat));
    if(ret == kNoErr)
        ret = POVMSObject_Set(object, &attr, key);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_SetBool
*
* DESCRIPTION
*   Stores a bool value in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetBool(POVMSObjectPtr object, POVMSType key, POVMSBool boolvalue)
{
    POVMSAttribute attr;
    int ret;
    unsigned char c;

    POVMS_LOG_OUTPUT("POVMSUtil_SetBool");

    if(object == NULL)
        return kParamErr;

    if(boolvalue == false)
        c = 0x00;
    else
        c = 0xFF;

    ret = POVMSAttr_New(&attr);
    if(ret == kNoErr)
        ret = POVMSAttr_Set(&attr, kPOVMSType_Bool, (void *)(&c), sizeof(unsigned char));
    if(ret == kNoErr)
        ret = POVMSObject_Set(object, &attr, key);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_SetType
*
* DESCRIPTION
*  Stores an type value in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_SetType(POVMSObjectPtr object, POVMSType key, POVMSType typevalue)
{
    POVMSAttribute attr;
    int ret;

    POVMS_LOG_OUTPUT("POVMSUtil_SetType");

    if(object == NULL)
        return kParamErr;

    ret = POVMSAttr_New(&attr);
    if(ret == kNoErr)
        ret = POVMSAttr_Set(&attr, kPOVMSType_Type, (void *)(&typevalue), sizeof(POVMSType));
    if(ret == kNoErr)
        ret = POVMSObject_Set(object, &attr, key);

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_GetStringLength
*
* DESCRIPTION
*   Accesses a string length (including terminating 0) in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetStringLength(POVMSObjectPtr object, POVMSType key, int *len)
{
    POVMSAttribute attr;
    POVMSType attr_type = kPOVMSType_CString;
    int ret,temp_ret;

    POVMS_LOG_OUTPUT("POVMSUtil_GetStringLength");

    if(len == NULL)
        return kParamErr;

    ret = POVMSObject_Get(object, &attr, key);
    if(ret == kNoErr)
    {
        ret = POVMSAttr_Type(&attr, &attr_type);
        if((ret == kNoErr) && (attr_type == kPOVMSType_CString))
            ret = POVMSAttr_Size(&attr, len);
        else if((ret == kNoErr) && (attr_type != kPOVMSType_CString))
            ret = kDataTypeErr;
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == kNoErr)
            ret = temp_ret;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_GetString
*
* DESCRIPTION
*   Accesses a string (including terminating 0) in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetString(POVMSObjectPtr object, POVMSType key, char *str, int *maxlen)
{
    POVMSAttribute attr;
    int ret,temp_ret;

    POVMS_LOG_OUTPUT("POVMSUtil_GetString");

    if(str == NULL)
        return kParamErr;
    if(maxlen == NULL)
        return kParamErr;

    ret = POVMSObject_Get(object, &attr, key);
    if(ret == kNoErr)
    {
        ret = POVMSAttr_Get(&attr, kPOVMSType_CString, (void *)str, maxlen);
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == kNoErr)
            ret = temp_ret;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_GetStringLength
*
* DESCRIPTION
*   Accesses a string length (including terminating 0) in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetUCS2StringLength(POVMSObjectPtr object, POVMSType key, int *len)
{
    POVMSAttribute attr;
    POVMSType attr_type = kPOVMSType_UCS2String;
    int ret,temp_ret;

    POVMS_LOG_OUTPUT("POVMSUtil_GetUCS2StringLength");

    if(len == NULL)
        return kParamErr;

    ret = POVMSObject_Get(object, &attr, key);
    if(ret == kNoErr)
    {
        ret = POVMSAttr_Type(&attr, &attr_type);
        if((ret == kNoErr) && (attr_type == kPOVMSType_UCS2String))
            ret = POVMSAttr_Size(&attr, len) / 2;
        else if((ret == kNoErr) && (attr_type != kPOVMSType_UCS2String))
            ret = kDataTypeErr;
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == kNoErr)
            ret = temp_ret;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_GetUCS2String
*
* DESCRIPTION
*   Accesses a string (including terminating 0) in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetUCS2String(POVMSObjectPtr object, POVMSType key, POVMSUCS2 *str, int *maxlen)
{
    POVMSAttribute attr;
    int ret,temp_ret;

    POVMS_LOG_OUTPUT("POVMSUtil_GetUCS2String");

    if(str == NULL)
        return kParamErr;
    if(maxlen == NULL)
        return kParamErr;

    *maxlen *= 2;

    ret = POVMSObject_Get(object, &attr, key);
    if(ret == kNoErr)
    {
        ret = POVMSAttr_Get(&attr, kPOVMSType_UCS2String, (void *)str, maxlen);
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == kNoErr)
            ret = temp_ret;
    }

    *maxlen /= 2;

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_GetInt
*
* DESCRIPTION
*   Accesses an integer value in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetInt(POVMSObjectPtr object, POVMSType key, POVMSInt *value)
{
    POVMSAttribute attr;
    int l = sizeof(POVMSInt);
    int ret,temp_ret;

    POVMS_LOG_OUTPUT("POVMSUtil_GetInt");

    if(value == NULL)
        return kParamErr;

    ret = POVMSObject_Get(object, &attr, key);
    if(ret == kNoErr)
    {
        ret = POVMSAttr_Get(&attr, kPOVMSType_Int, (void *)value, &l);
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == 0)
            ret = temp_ret;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_GetLong
*
* DESCRIPTION
*   Accesses an integer value in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetLong(POVMSObjectPtr object, POVMSType key, POVMSLong *value)
{
    POVMSAttribute attr;
    int l = sizeof(POVMSLong);
    int ret,temp_ret;

    POVMS_LOG_OUTPUT("POVMSUtil_GetLong");

    if(value == NULL)
        return kParamErr;

    ret = POVMSObject_Get(object, &attr, key);
    if(ret == kNoErr)
    {
        ret = POVMSAttr_Get(&attr, kPOVMSType_Long, (void *)value, &l);
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == kNoErr)
            ret = temp_ret;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_GetFloat
*
* DESCRIPTION
*   Accesses a float value in the given attribute.
*   NOTE: It is legal to access an integer as float, it will be converted
*   automatically!
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetFloat(POVMSObjectPtr object, POVMSType key, POVMSFloat *value)
{
    POVMSAttribute attr;
    int l = sizeof(POVMSFloat);
    int ret,temp_ret;

    POVMS_LOG_OUTPUT("POVMSUtil_GetFloat");

    if(value == NULL)
        return kParamErr;

    ret = POVMSObject_Get(object, &attr, key);
    if(ret == kNoErr)
    {
        ret = POVMSAttr_Get(&attr, kPOVMSType_Float, (void *)value, &l);
        if(ret == kDataTypeErr)
        {
            POVMSInt i = 0;
            l = sizeof(POVMSInt);
            ret = POVMSAttr_Get(&attr, kPOVMSType_Int, (void *)(&i), &l);
            *value = POVMSFloat(i);
        }
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == kNoErr)
            ret = temp_ret;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_GetBool
*
* DESCRIPTION
*   Accesses a bool value in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetBool(POVMSObjectPtr object, POVMSType key, POVMSBool *boolvalue)
{
    POVMSAttribute attr;
    int l = sizeof(unsigned char);
    int ret,temp_ret;
    unsigned char c = 0x00;

    POVMS_LOG_OUTPUT("POVMSUtil_GetBool");

    if(boolvalue == NULL)
        return kParamErr;

    ret = POVMSObject_Get(object, &attr, key);
    if(ret == kNoErr)
    {
        ret = POVMSAttr_Get(&attr, kPOVMSType_Bool, (void *)(&c), &l);
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == kNoErr)
            ret = temp_ret;
    }

    if(c == 0x00)
        *boolvalue = false;
    else
        *boolvalue = true;

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_GetType
*
* DESCRIPTION
*   Accesses a type value in the given attribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_GetType(POVMSObjectPtr object, POVMSType key, POVMSType *typevalue)
{
    POVMSAttribute attr;
    int l = sizeof(POVMSType);
    int ret,temp_ret;

    POVMS_LOG_OUTPUT("POVMSUtil_GetType");

    if(typevalue == NULL)
        return kParamErr;

    ret = POVMSObject_Get(object, &attr, key);
    if(ret == kNoErr)
    {
        ret = POVMSAttr_Get(&attr, kPOVMSType_Type, (void *)typevalue, &l);
        temp_ret = POVMSAttr_Delete(&attr);
        if(ret == kNoErr)
            ret = temp_ret;
    }

    return ret;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_TempAlloc
*
* DESCRIPTION
*   Allocate a given amount of memory from POVMS memory storage block.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_TempAlloc(void **ptr, int datasize)
{
    POVMS_LOG_OUTPUT("POVMSUtil_TempAlloc");

    if(ptr == NULL)
        return kParamErr;

    *ptr = POVMS_Sys_Malloc(datasize);

    if(*ptr == NULL)
        return kMemFullErr;

    return kNoErr;
}


/*****************************************************************************
*
* FUNCTION
*   POVMSUtil_TempFree
*
* DESCRIPTION
*   Free memory in POVMS memory storage block.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_EXPORT int POVMS_CDECL POVMSUtil_TempFree(void *ptr)
{
    POVMS_LOG_OUTPUT("POVMSUtil_TempFree");

    if(ptr == NULL)
        return kParamErr;

    POVMS_Sys_Free(ptr);

    return kNoErr;
}


/*****************************************************************************
* Default system specific functions
******************************************************************************/

unsigned int POVMS_Sys_UCS2Strlen_Default(const POVMSUCS2 *s)
{
    unsigned int len = 0;

    if(s == NULL)
        return 0;

    while(*s != 0)
    {
        len++;
        s++;
    }

    return len;
}

POVMS_Sys_Thread_Type POVMS_Sys_GetCurrentThread_Default()
{
    return (POVMS_Sys_Thread_Type)0;
}

POVMSAddress POVMS_Sys_QueueToAddress_Default(POVMS_Sys_QueueNode_Default *q)
{
    return (POVMSAddress)q;
}

POVMS_Sys_QueueNode_Default *POVMS_Sys_AddressToQueue_Default(POVMSAddress a)
{
    return (POVMS_Sys_QueueNode_Default *)a;
}

POVMS_Sys_QueueNode_Default *POVMS_Sys_QueueOpen_Default()
{
    POVMS_Sys_QueueNode_Default *ptr = (POVMS_Sys_QueueNode_Default *)POVMS_Sys_Malloc(sizeof(POVMS_Sys_QueueNode_Default));

    if(ptr == NULL)
        return NULL;

    ptr->magic = 0x12345678;
    ptr->entries = 0;
    ptr->first = NULL;
    ptr->last = NULL;

    return (POVMS_Sys_QueueNode_Default *)ptr;
}

void POVMS_Sys_QueueClose_Default(POVMS_Sys_QueueNode_Default *q)
{
    POVMS_Sys_QueueNode_Default *ptr = (POVMS_Sys_QueueNode_Default *)q;

    if(ptr != NULL)
    {
        if(ptr->entries > 0)
        {
            POVMS_Sys_QueueDataNode_Default *node = ptr->first;
            POVMS_Sys_QueueDataNode_Default *nextnode = NULL;

            while(node != NULL)
            {
                nextnode = node->next;
                POVMS_Sys_Free(node);
                node = nextnode;
            }
        }

        POVMS_Sys_Free(ptr);
    }
}

void *POVMS_Sys_QueueReceive_Default(POVMS_Sys_QueueNode_Default *q, int *l, bool, bool)
{
    POVMS_Sys_QueueNode_Default *ptr = (POVMS_Sys_QueueNode_Default *)q;
    POVMS_Sys_QueueDataNode_Default *node = NULL;
    void *d = NULL;

    if(l == NULL)
        return NULL;

    *l = 0;

    if(ptr == NULL)
        return NULL;

    if(ptr->magic != 0x12345678)
        return NULL;

    if(ptr->entries <= 0)
        return NULL;

    if(ptr->first == NULL)
        return NULL;

    node = ptr->first;

    d = node->data;
    *l = node->len;

    if(node == ptr->last)
        ptr->last = NULL;

    ptr->first = node->next;

    ptr->entries--;

    POVMS_Sys_Free(node);

    return d;
}

int POVMS_Sys_QueueSend_Default(POVMS_Sys_QueueNode_Default *q, void *p, int l)
{
    POVMS_Sys_QueueNode_Default *ptr = (POVMS_Sys_QueueNode_Default *)q;
    POVMS_Sys_QueueDataNode_Default *node = NULL;

    if(ptr == NULL)
        return -1;

    if(ptr->magic != 0x12345678)
        return -2;

    node = (POVMS_Sys_QueueDataNode_Default *)POVMS_Sys_Malloc(sizeof(POVMS_Sys_QueueDataNode_Default));
    if(node == NULL)
        return -3;

    node->data = p;
    node->len = l;

    node->next = NULL;
    if(ptr->last != NULL)
        ptr->last->next = node;
    if(ptr->first == NULL)
        ptr->first = node;
    ptr->last = node;

    ptr->entries++;

    return 0;
}

int POVMS_Sys_AddressFromStream_Default(POVMSAddress *a, POVMSStream *s, int z)
{
    POVMSStream *b = s;
    int i;

    // default value in case decoding fails
    *a = POVMSInvalidAddress;

    // system specific address (byte code 1)
    if((z >= (2 + sizeof(POVMSAddress))) && (*s == 1))
    {
        s++;
        if(*s == sizeof(POVMSAddress))
        {
            s++;
            for(i = 0; i < sizeof(POVMSAddress); i++)
                ((POVMSStream *)a)[i] = s[i];
            s += sizeof(POVMSAddress);
        }

        z -= (s - b);
    }
    else if((z >= 2) && (*s == 1))
    {
        // skip over unknown system specific address size
        s += *s;
        z -= (s - b);
    }

    // IPv4 address and TCP port (byte code 4)
    if((z >= 7) && (*s == 4))
    {
        // skip
        s += 7;
        z -= 7;
    }

    // IPv6 address and TCP port (byte code 6)
    if((z >= 11) && (*s == 6))
    {
        // skip
        s += 11;
        z -= 11;
    }

    // skip over the remaining data
    s += z;

    // return that everything has been read
    return (s - b);
}

int POVMS_Sys_AddressToStream_Default(POVMSAddress a, POVMSStream *s, int *z)
{
    int i;

    // check if there is enough space (there should always be)
    if(*z < (2 + sizeof(POVMSAddress)))
        return 0;

    // write system specific address
    *s = 1;
    s++;
    *s = sizeof(POVMSAddress);
    s++;
    for(i = 0; i < sizeof(POVMSAddress); i++)
        s[i] = ((POVMSStream *)(&a))[i];
    s += sizeof(POVMSAddress);

    // subtract size written
    *z -= (2 + sizeof(POVMSAddress));

    // return size written
    return (2 + sizeof(POVMSAddress));
}

int POVMS_Sys_AddressToStreamSize_Default(POVMSAddress)
{
    // default implementation only supports writing system specific address
    return (2 + sizeof(POVMSAddress));
}

/*****************************************************************************
* POVMS memory debugging functions
******************************************************************************/

#ifdef _DEBUG_POVMS_TRACE_MEMORY_

POVMSMemoryTraceHeader *gPOVMSMemoryTraceRootPointer = NULL;

void *POVMS_Sys_Trace_Malloc(size_t size, int line)
{
    POVMSMemoryTraceHeader *ptr = (POVMSMemoryTraceHeader *)malloc(size + sizeof(POVMSMemoryTraceHeader) + (sizeof(char) * 8));

    POVMS_Sys_Trace_Set_Guard(&(ptr->magichead[0]));
    POVMS_Sys_Trace_Set_Guard(&(ptr->magictrail[0]));
    POVMS_Sys_Trace_Set_Guard(((char *)ptr) + size + sizeof(POVMSMemoryTraceHeader));

    ptr->line = line;
    ptr->size = size;

    POVMS_Sys_Trace_Insert(ptr);

    return (void *)(((char *)ptr) + sizeof(POVMSMemoryTraceHeader));
}

void *POVMS_Sys_Trace_Realloc(void *iptr, size_t size, int line)
{
    if(iptr != NULL)
    {
        iptr = (void *)(((char *)iptr) - sizeof(POVMSMemoryTraceHeader));
        POVMS_Sys_Trace_Remove((POVMSMemoryTraceHeader *)iptr);

        if((POVMS_Sys_Trace_Check_Guard(&(((POVMSMemoryTraceHeader *)iptr)->magichead[0])) != kNoError) ||
           (POVMS_Sys_Trace_Check_Guard(&(((POVMSMemoryTraceHeader *)iptr)->magictrail[0])) != kNoError) ||
           (POVMS_Sys_Trace_Check_Guard(((char *)iptr) + ((POVMSMemoryTraceHeader *)iptr)->size + sizeof(POVMSMemoryTraceHeader)) != kNoError))
            printf("POVMS damaged memory block header detected in line %d\n", line);
    }

    POVMSMemoryTraceHeader *ptr = (POVMSMemoryTraceHeader *)realloc((void *)iptr, size + sizeof(POVMSMemoryTraceHeader) + (sizeof(char) * 8));

    POVMS_Sys_Trace_Set_Guard(&(ptr->magichead[0]));
    POVMS_Sys_Trace_Set_Guard(&(ptr->magictrail[0]));
    POVMS_Sys_Trace_Set_Guard(((char *)ptr) + size + sizeof(POVMSMemoryTraceHeader));

    ptr->line = line;
    ptr->size = size;

    POVMS_Sys_Trace_Insert(ptr);

    return (void *)(((char *)ptr) + sizeof(POVMSMemoryTraceHeader));
}

void POVMS_Sys_Trace_Free(void *ptr, int line)
{
    if(ptr == NULL)
        return;

    ptr = (void *)(((char *)ptr) - sizeof(POVMSMemoryTraceHeader));

    if((POVMS_Sys_Trace_Check_Guard(&(((POVMSMemoryTraceHeader *)ptr)->magichead[0])) != kNoError) ||
       (POVMS_Sys_Trace_Check_Guard(&(((POVMSMemoryTraceHeader *)ptr)->magictrail[0])) != kNoError) ||
       (POVMS_Sys_Trace_Check_Guard(((char *)ptr) + ((POVMSMemoryTraceHeader *)ptr)->size + sizeof(POVMSMemoryTraceHeader)) != kNoError))
        printf("POVMS damaged memory block header detected in line %d\n", line);

    POVMS_Sys_Trace_Remove((POVMSMemoryTraceHeader *)ptr);

    free(ptr);
}

void POVMS_Sys_Trace_Set_Guard(char *ptr)
{
    ptr[0] = 'P';
    ptr[1] = 'O';
    ptr[2] = 'V';
    ptr[3] = 'T';
    ptr[4] = 'r';
    ptr[5] = 'a';
    ptr[6] = 'c';
    ptr[7] = 'e';
}

int POVMS_Sys_Trace_Check_Guard(char *ptr)
{
    if((ptr[0] == 'P') && (ptr[1] == 'O') && (ptr[2] == 'V') && (ptr[3] == 'T') &&
       (ptr[4] == 'r') && (ptr[5] == 'a') && (ptr[6] == 'c') && (ptr[7] == 'e'))
        return kNoError;

    return kChecksumErr;
}

void POVMS_Sys_Trace_Insert(POVMSMemoryTraceHeader *ptr)
{
    ptr->last = NULL;
    ptr->next = NULL;

    // WARNING: This is of course not thread-safe!!! [trf]
    ptr->next = gPOVMSMemoryTraceRootPointer;
    gPOVMSMemoryTraceRootPointer = ptr;
    if(ptr->next != NULL)
        ptr->next->last = ptr;
}

void POVMS_Sys_Trace_Remove(POVMSMemoryTraceHeader *ptr)
{
    // WARNING: This is of course not thread-safe!!! [trf]
    if(ptr == gPOVMSMemoryTraceRootPointer)
        gPOVMSMemoryTraceRootPointer = ptr->next;
    if(ptr->last != NULL)
        ptr->last->next = ptr->next;
    if(ptr->next != NULL)
        ptr->next->last = ptr->last;
}

POVMS_EXPORT int POVMS_TraceDump()
{
    POVMSMemoryTraceHeader *ptr = gPOVMSMemoryTraceRootPointer;

    while(ptr != NULL)
    {
        printf("POVMS leaked %d bytes of memory allocated in line %d\n", ptr->size, ptr->line);
        ptr = ptr->next;
    }

    return kNoError;
}

#endif
