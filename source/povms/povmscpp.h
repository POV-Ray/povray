//******************************************************************************
///
/// @file povms/povmscpp.h
///
/// This module contains all defines, typedefs, and prototypes for the
/// C++ interface version of `povms.cpp`.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVMSCPP_H
#define POVMSCPP_H

#include <string>
#include <vector>

#include "povms/povms.h"
#include "base/pov_err.h"

/// @file
/// @todo We could place this in a namespace.

class POVMS_Container;
class POVMS_Attribute;
class POVMS_List;
class POVMS_Object;
class POVMS_Message;
class POVMS_MessageReceiver;

typedef std::basic_string<POVMSUCS2> POVMSUCS2String;

POVMSUCS2String POVMS_ASCIItoUCS2String(const char *s);
std::string POVMS_UCS2toASCIIString(const POVMSUCS2String& s);

class POVMS_Container
{
        friend void POVMS_SendMessage(POVMS_Message&);
        friend void POVMS_SendMessage(POVMSContext, POVMS_Message&, POVMS_Message *, int);
        friend class POVMS_List;
        friend class POVMS_Object;
        friend class POVMS_MessageReceiver;
    public:
        POVMS_Container();
        virtual ~POVMS_Container();

        POVMSType Type() const;
        size_t Size() const;
        bool IsNull() const;
    protected:
        mutable POVMSData data;

        void DetachData();
};

class POVMS_Attribute : public POVMS_Container
{
    public:
        POVMS_Attribute();
        POVMS_Attribute(const char *str);
        POVMS_Attribute(const POVMSUCS2 *str);
        POVMS_Attribute(POVMSInt value);
        POVMS_Attribute(POVMSLong value);
        POVMS_Attribute(POVMSFloat value);
        POVMS_Attribute(bool value);
        POVMS_Attribute(POVMSType value);
        POVMS_Attribute(std::vector<POVMSInt>& value);
        POVMS_Attribute(std::vector<POVMSLong>& value);
        POVMS_Attribute(std::vector<POVMSFloat>& value);
        POVMS_Attribute(std::vector<POVMSType>& value);
        POVMS_Attribute(POVMSAttribute& convert);
        POVMS_Attribute(const POVMS_Attribute& source);
        virtual ~POVMS_Attribute();

        POVMS_Attribute& operator=(const POVMS_Attribute& source);

        void Get(POVMSType type, void *data, int *maxdatasize);
        void Set(POVMSType type, const void *data, int datasize);

        int GetStringLength() const; // Note: Includes trailing \0 character code!
        int GetString(char *str, int maxlen);
        std::string GetString();
        int GetUCS2StringLength() const; // Note: Includes trailing \0 character code!
        int GetUCS2String(POVMSUCS2 *str, int maxlen);
        POVMSUCS2String GetUCS2String();
        POVMSInt GetInt();
        POVMSLong GetLong();
        POVMSFloat GetFloat();
        POVMSBool GetBool();
        POVMSType GetType();

        std::vector<POVMSInt> GetIntVector();
        std::vector<POVMSLong> GetLongVector();
        std::vector<POVMSFloat> GetFloatVector();
        std::vector<POVMSType> GetTypeVector();

        int GetVectorSize() const;
};

class POVMS_List : public POVMS_Container
{
    public:
        POVMS_List();
        POVMS_List(POVMSAttributeList& convert);
        POVMS_List(const POVMS_List& source);
        virtual ~POVMS_List();

        POVMS_List& operator=(const POVMS_List& source);

        void Append(POVMS_Attribute& item);
        void Append(POVMS_List& item);
        void Append(POVMS_Object& item);
        void AppendN(int cnt, POVMS_Attribute& item);
        void AppendN(int cnt, POVMS_List& item);
        void AppendN(int cnt, POVMS_Object& item);
        void GetNth(int index, POVMS_Attribute& item);
        void GetNth(int index, POVMS_List& item);
        void GetNth(int index, POVMS_Object& item);
        void SetNth(int index, POVMS_Attribute& item);
        void SetNth(int index, POVMS_List& item);
        void SetNth(int index, POVMS_Object& item);
        void RemoveNth(int index);
        void Clear();

        int GetListSize();
};

class POVMS_Object : public POVMS_Container
{
        friend class POVMS_Message;
        friend class POVMS_MessageReceiver;

        class InputStream
        {
            public:
                InputStream() { }
                virtual ~InputStream() { }
                virtual bool read(void *, int) = 0;
        };

        class OutputStream
        {
            public:
                OutputStream() { }
                virtual ~OutputStream() { }
                virtual bool write(void *, int) = 0;
        };

        template<class T> class InputStreamT : public InputStream
        {
            public:
                InputStreamT(T& s) : stream(s) { }
                virtual ~InputStreamT() { }
                virtual bool read(void *ptr, int cnt) { return !(!stream.read(ptr, (size_t)cnt)); }
            private:
                T& stream;
        };

        template<class T> class OutputStreamT : public OutputStream
        {
            public:
                OutputStreamT(T& s) : stream(s) { }
                virtual ~OutputStreamT() { }
                virtual bool write(void *ptr, int cnt) { return !(!stream.write(ptr, (size_t)cnt)); }
            private:
                T& stream;
        };
    public:
        POVMS_Object();
        POVMS_Object(POVMSType objclass);
        POVMS_Object(POVMSObject& convert);
        POVMS_Object(POVMSObjectPtr convert);
        POVMS_Object(const POVMS_Object& source);
        ~POVMS_Object();

        POVMS_Object& operator=(const POVMS_Object& source);

        void Get(POVMSType key, POVMS_Attribute& attr);
        void Get(POVMSType key, POVMS_List& attr);
        void Get(POVMSType key, POVMS_Object& attr);
        void Set(POVMSType key, POVMS_Attribute& attr);
        void Set(POVMSType key, POVMS_List& attr);
        void Set(POVMSType key, POVMS_Object& attr);
        void Remove(POVMSType key);
        bool Exist(POVMSType key);
        void Merge(POVMS_Object& source);

        const POVMSObject& operator*() const;
        const POVMSObject* operator->() const;
        POVMSObject operator()();

        void SetString(POVMSType key, const char *str); // Note: Strings may not contain \0 characters codes!
        void SetUCS2String(POVMSType key, const POVMSUCS2 *str); // Note: Strings may not contain \0 characters codes!
        void SetInt(POVMSType key, POVMSInt value);
        void SetLong(POVMSType key, POVMSLong value);
        void SetFloat(POVMSType key, POVMSFloat value);
        void SetBool(POVMSType key, POVMSBool value);
        void SetType(POVMSType key, POVMSType value);

        void SetIntVector(POVMSType key, std::vector<POVMSInt>& value);
        void SetLongVector(POVMSType key, std::vector<POVMSLong>& value);
        void SetFloatVector(POVMSType key, std::vector<POVMSFloat>& value);
        void SetTypeVector(POVMSType key, std::vector<POVMSType>& value);

        int GetStringLength(POVMSType key); // Note: Includes trailing \0 character code!
        int GetString(POVMSType key, char *str, int maxlen);
        std::string GetString(POVMSType key);
        int GetUCS2StringLength(POVMSType key); // Note: Includes trailing \0 character code!
        int GetUCS2String(POVMSType key, POVMSUCS2 *str, int maxlen);
        POVMSUCS2String GetUCS2String(POVMSType key);
        POVMSInt GetInt(POVMSType key);
        POVMSLong GetLong(POVMSType key);
        POVMSFloat GetFloat(POVMSType key);
        POVMSBool GetBool(POVMSType key);
        POVMSType GetType(POVMSType key);

        std::vector<POVMSInt> GetIntVector(POVMSType key);
        std::vector<POVMSLong> GetLongVector(POVMSType key);
        std::vector<POVMSFloat> GetFloatVector(POVMSType key);
        std::vector<POVMSType> GetTypeVector(POVMSType key);

        std::string TryGetString(POVMSType key, const char *alt);
        std::string TryGetString(POVMSType key, const std::string& alt);
        POVMSUCS2String TryGetUCS2String(POVMSType key, const char *alt);
        POVMSUCS2String TryGetUCS2String(POVMSType key, const POVMSUCS2String& alt);
        POVMSInt TryGetInt(POVMSType key, POVMSInt alt);
        POVMSLong TryGetLong(POVMSType key, POVMSLong alt);
        POVMSFloat TryGetFloat(POVMSType key, POVMSFloat alt);
        POVMSBool TryGetBool(POVMSType key, POVMSBool alt);
        POVMSType TryGetType(POVMSType key, POVMSType alt);

        template<class T> void Read(T& stream)
        {
            InputStreamT<T> s(stream);
            Read(s, false, false);
        }

        template<class T> void Write(T& stream)
        {
            OutputStreamT<T> s(stream);
            Write(s, false, true);
        }
    private:
        void Read(InputStream& stream, bool continued, bool headeronly);
        void Write(OutputStream& stream, bool append, bool compress);
};

class POVMS_Message : public POVMS_Object
{
    public:
        POVMS_Message();
        POVMS_Message(POVMSType objclass, POVMSType msgclass = kPOVMSType_WildCard, POVMSType msgid = kPOVMSType_WildCard);
        POVMS_Message(POVMS_Object& convert, POVMSType msgclass = kPOVMSType_WildCard, POVMSType msgid = kPOVMSType_WildCard);
        POVMS_Message(POVMSObject& convert);
        POVMS_Message(POVMSObjectPtr convert);
        POVMS_Message(const POVMS_Message& source);

        POVMS_Message& operator=(const POVMS_Message& source);

        POVMSType GetClass();
        POVMSType GetIdentifier();

        POVMSAddress GetSourceAddress();
        POVMSAddress GetDestinationAddress();
        void SetSourceAddress(POVMSAddress);
        void SetDestinationAddress(POVMSAddress);
};

class POVMS_MessageReceiver
{
    private:
        class HandlerOO
        {
            public:
                virtual void Call(POVMS_Message&, POVMS_Message&, int) = 0;
        };
        class Handler
        {
            public:
                virtual void Call(POVMSObjectPtr, POVMSObjectPtr, int) = 0;
        };
    protected:
        template<class T> class MemberHandlerOO : public HandlerOO
        {
            public:
                typedef void (T::*MemberHandlerPtr)(POVMS_Message&, POVMS_Message&, int);

                MemberHandlerOO()
                {
                    classptr = NULL;
                    handlerptr = NULL;
                }

                MemberHandlerOO(T *cptr, MemberHandlerPtr hptr)
                {
                    classptr = cptr;
                    handlerptr = hptr;
                }

                void Call(POVMS_Message& msg, POVMS_Message& result, int mode)
                {
                    if((classptr != NULL) && (handlerptr != NULL))
                        (classptr->*handlerptr)(msg, result, mode);
                    else
                        throw POV_EXCEPTION_CODE(pov_base::kNullPointerErr);
                }
            private:
                MemberHandlerPtr handlerptr;
                T *classptr;
        };

        template<class T> class MemberHandler : public Handler
        {
            public:
                typedef void (T::*MemberHandlerPtr)(POVMSObjectPtr, POVMSObjectPtr, int);

                MemberHandler()
                {
                    classptr = NULL;
                    handlerptr = NULL;
                }

                MemberHandler(T *cptr, MemberHandlerPtr hptr)
                {
                    classptr = cptr;
                    handlerptr = hptr;
                }

                void Call(POVMSObjectPtr msg, POVMSObjectPtr result, int mode)
                {
                    if((classptr != NULL) && (handlerptr != NULL))
                        (classptr->*handlerptr)(msg, result, mode);
                    else
                        throw POV_EXCEPTION_CODE(pov_base::kNullPointerErr);
                }
            private:
                MemberHandlerPtr handlerptr;
                T *classptr;
        };

        class FunctionHandlerOO : public HandlerOO
        {
            public:
                typedef void (*FunctionHandlerPtr)(POVMS_Message&, POVMS_Message&, int, void *);

                FunctionHandlerOO()
                {
                    handlerptr = NULL;
                    privatedata = NULL;
                }

                FunctionHandlerOO(FunctionHandlerPtr hptr, void *pptr)
                {
                    handlerptr = hptr;
                    privatedata = pptr;
                }

                void Call(POVMS_Message& msg, POVMS_Message& result, int mode)
                {
                    if(handlerptr != NULL)
                        handlerptr(msg, result, mode, privatedata);
                    else
                        throw POV_EXCEPTION_CODE(pov_base::kNullPointerErr);
                }
            private:
                FunctionHandlerPtr handlerptr;
                void *privatedata;
        };

        class FunctionHandler : public Handler
        {
            public:
                typedef void (*FunctionHandlerPtr)(POVMSObjectPtr, POVMSObjectPtr, int, void *);

                FunctionHandler()
                {
                    handlerptr = NULL;
                    privatedata = NULL;
                }

                FunctionHandler(FunctionHandlerPtr hptr, void *pptr)
                {
                    handlerptr = hptr;
                    privatedata = pptr;
                }

                void Call(POVMSObjectPtr msg, POVMSObjectPtr result, int mode)
                {
                    if(handlerptr != NULL)
                        handlerptr(msg, result, mode, privatedata);
                    else
                        throw POV_EXCEPTION_CODE(pov_base::kNullPointerErr);
                }
            private:
                FunctionHandlerPtr handlerptr;
                void *privatedata;
        };

        POVMS_MessageReceiver(POVMSContext contextref);
        virtual ~POVMS_MessageReceiver();

        template<class T> void InstallFront(POVMSType hclass, POVMSType hid, T *cptr, typename MemberHandlerOO<T>::MemberHandlerPtr hptr)
        {
            AddNodeFront(hclass, hid, new MemberHandlerOO<T>(cptr, hptr), NULL);
        }

        template<class T> void InstallFront(POVMSType hclass, POVMSType hid, T *cptr, typename MemberHandler<T>::MemberHandlerPtr hptr)
        {
            AddNodeFront(hclass, hid, NULL, new MemberHandler<T>(cptr, hptr));
        }

        void InstallFront(POVMSType hclass, POVMSType hid, FunctionHandlerOO::FunctionHandlerPtr hptr, void *pptr)
        {
            AddNodeFront(hclass, hid, new FunctionHandlerOO(hptr, pptr), NULL);
        }

        void InstallFront(POVMSType hclass, POVMSType hid, FunctionHandler::FunctionHandlerPtr hptr, void *pptr)
        {
            AddNodeFront(hclass, hid, NULL, new FunctionHandler(hptr, pptr));
        }

        template<class T> void InstallBack(POVMSType hclass, POVMSType hid, T *cptr, typename MemberHandlerOO<T>::MemberHandlerPtr hptr)
        {
            AddNodeBack(hclass, hid, new MemberHandlerOO<T>(cptr, hptr), NULL);
        }

        template<class T> void InstallBack(POVMSType hclass, POVMSType hid, T *cptr, typename MemberHandler<T>::MemberHandlerPtr hptr)
        {
            AddNodeBack(hclass, hid, NULL, new MemberHandler<T>(cptr, hptr));
        }

        void InstallBack(POVMSType hclass, POVMSType hid, FunctionHandlerOO::FunctionHandlerPtr hptr, void *pptr)
        {
            AddNodeBack(hclass, hid, new FunctionHandlerOO(hptr, pptr), NULL);
        }

        void InstallBack(POVMSType hclass, POVMSType hid, FunctionHandler::FunctionHandlerPtr hptr, void *pptr)
        {
            AddNodeBack(hclass, hid, NULL, new FunctionHandler(hptr, pptr));
        }

        void Remove(POVMSType hclass, POVMSType hid);
    private:
        struct HandlerNode
        {
            struct HandlerNode *last;
            struct HandlerNode *next;
            POVMSType hclass;
            POVMSType hid;
            HandlerOO *handleroo;
            Handler *handler;
        };

        POVMSContext context;
        HandlerNode *receivers;

        POVMS_MessageReceiver(); // default constructor not allowed
        POVMS_MessageReceiver(const POVMS_MessageReceiver&); // no copies allowed
        POVMS_MessageReceiver& operator=(const POVMS_MessageReceiver&); // no copy assignments allowed

        static int ReceiveHandler(POVMSObjectPtr msg, POVMSObjectPtr result, int mode, void *privatedataptr);

        void AddNodeFront(POVMSType hclass, POVMSType hid, HandlerOO *hooptr, Handler *hptr);
        void AddNodeBack(POVMSType hclass, POVMSType hid, HandlerOO *hooptr, Handler *hptr);
        void RemoveNode(HandlerNode *nodeptr);
};

void POVMS_SendMessage(POVMS_Message& msg);
void POVMS_SendMessage(POVMSContext contextref, POVMS_Message& msg, POVMS_Message *result, int mode);

#endif
