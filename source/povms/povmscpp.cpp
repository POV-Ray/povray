//******************************************************************************
///
/// @file povms/povmscpp.cpp
///
/// This module contains the C++ interface version of `povms.cpp`.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
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

#include <cstdlib>
#include <cstring>

#include <zlib.h>

#include "povmscpp.h"

#ifndef POVMS_DISCONNECTED
    // this must be the last file included
    #include "base/povdebug.h"
#endif

const POVMSType kPOVMSRawStreamEncoding = 'RAWS';
const POVMSType kPOVMSGZipStreamEncoding = 'GZIP';

POVMSUCS2String POVMS_SysToUCS2String(const char *s)
{
    POVMSUCS2String r;
    unsigned char ch;

    if (s != nullptr)
    {
        while(*s != '\0')
        {
            ch = *s++;
            r += (POVMSUCS2)(ch);
        }
    }

    return r;
}

std::string POVMS_UCS2toSysString(const POVMSUCS2String& s)
{
    std::string r;

    for(std::size_t i = 0; i < s.length(); i++)
    {
        if(s[i] > 0xFFu)
            r += ' ';
        else
            r += (char)(s[i]);
    }

    return r;
}

/*****************************************************************************
*
* CLASS
*   POVMS_Container
*
* DESCRIPTION
*   Base class for all other POVMS classes.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_Container::POVMS_Container()
{
    data.type = kPOVMSType_Null;
    data.size = 0;
    data.ptr = nullptr;
}

POVMS_Container::~POVMS_Container() noexcept(false)
{
    // do nothing for now
}

POVMSType POVMS_Container::Type() const
{
    return data.type;
}

size_t POVMS_Container::Size() const
{
    return data.size;
}

bool POVMS_Container::IsNull() const
{
    return (data.type == kPOVMSType_Null);
}

void POVMS_Container::DetachData()
{
    data.type = kPOVMSType_Null;
    data.size = 0;
    data.ptr = nullptr;
}


/*****************************************************************************
*
* CLASS
*   POVMS_Attribute
*
* DESCRIPTION
*   Class handling POVMSAttribute.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_Attribute::POVMS_Attribute()
{
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

POVMS_Attribute::POVMS_Attribute(const char *str)
{
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_CString, reinterpret_cast<const void *>(str), strlen(str) + 1);
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(const POVMSUCS2 *str)
{
    int len;
    int err;

    for(len = 0; str[len] != 0; len++) { }

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_UCS2String, reinterpret_cast<const void *>(str), (len + 1) * 2);
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(POVMSInt value)
{
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_Int, (void *)(&value), sizeof(POVMSLong));
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(POVMSLong value)
{
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_Long, (void *)(&value), sizeof(POVMSLong));
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(POVMSFloat value)
{
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_Float, (void *)(&value), sizeof(POVMSFloat));
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(bool b)
{
    POVMSBool value = (POVMSBool)b;
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_Bool, (void *)(&value), sizeof(POVMSBool));
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(POVMSType value)
{
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_Type, (void *)(&value), sizeof(POVMSType));
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(std::vector<POVMSInt>& value)
{
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_VectorInt, (void *)(&value[0]), sizeof(POVMSInt) * value.size());
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(std::vector<POVMSLong>& value)
{
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_VectorLong, (void *)(&value[0]), sizeof(POVMSLong) * value.size());
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(std::vector<POVMSFloat>& value)
{
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_VectorFloat, (void *)(&value[0]), sizeof(POVMSFloat) * value.size());
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(std::vector<POVMSType>& value)
{
    int err;

    err = POVMSAttr_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttr_Set(&data, kPOVMSType_VectorType, (void *)(&value[0]), sizeof(POVMSType) * value.size());
    if(err != pov_base::kNoErr)
    {
        (void)POVMSAttr_Delete(&data);
        throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Attribute::POVMS_Attribute(POVMSAttribute& convert)
{
    data = convert;
}

POVMS_Attribute::POVMS_Attribute(const POVMS_Attribute& source)
{
    int err;

    err = POVMSAttr_Copy(&source.data, &data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

POVMS_Attribute::~POVMS_Attribute() noexcept(false)
{
    int err;

    err = POVMSAttr_Delete(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

POVMS_Attribute& POVMS_Attribute::operator=(const POVMS_Attribute& source)
{
    int err;

    err = POVMSAttr_Delete(&data);
    if(err == pov_base::kNoErr)
        err = POVMSAttr_Copy(&source.data, &data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return *this;
}

void POVMS_Attribute::Get(POVMSType type, void *data, int *maxdatasize)
{
    int err;

    err = POVMSAttr_Get(&this->data, type, data, maxdatasize);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Attribute::Set(POVMSType type, const void *data, int datasize)
{
    int err;

    err = POVMSAttr_Set(&this->data, type, data, datasize);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

int POVMS_Attribute::GetStringLength() const // Note: Includes trailing \0 character code!
{
    return Size();
}

int POVMS_Attribute::GetString(char *str, int maxlen)
{
    Get(kPOVMSType_CString, (void *)str, &maxlen);

    return maxlen;
}

std::string POVMS_Attribute::GetString()
{
    int len = Size();

    char *strptr = new char[len];

    try
    {
        Get(kPOVMSType_CString, (void *)strptr, &len);

        std::string str(strptr);

        delete[] strptr;

        return str;
    }
    catch(pov_base::Exception&)
    {
        delete[] strptr;
        throw;
    }

    return std::string();
}

int POVMS_Attribute::GetUCS2StringLength() const // Note: Includes trailing \0 character code!
{
    return Size() / 2;
}

int POVMS_Attribute::GetUCS2String(POVMSUCS2 *str, int maxlen)
{
    maxlen /= 2;

    Get(kPOVMSType_UCS2String, (void *)str, &maxlen);

    return maxlen / 2;
}

POVMSUCS2String POVMS_Attribute::GetUCS2String()
{
    int len = Size();

    POVMSUCS2 *strptr = new POVMSUCS2[len / 2];

    try
    {
        Get(kPOVMSType_UCS2String, (void *)strptr, &len);

        POVMSUCS2String str(strptr);

        delete[] strptr;

        return str;
    }
    catch(pov_base::Exception&)
    {
        delete[] strptr;
        throw;
    }

    return POVMSUCS2String();
}

POVMSInt POVMS_Attribute::GetInt()
{
    POVMSInt result;
    int len = sizeof(result);

    Get(kPOVMSType_Int, (void *)&result, &len);

    return result;
}

POVMSLong POVMS_Attribute::GetLong()
{
    POVMSLong result;
    int len = sizeof(result);

    Get(kPOVMSType_Long, (void *)&result, &len);

    return result;
}

POVMSFloat POVMS_Attribute::GetFloat()
{
    POVMSFloat result;
    int len = sizeof(result);

    Get(kPOVMSType_Float, (void *)&result, &len);

    return result;
}

POVMSBool POVMS_Attribute::GetBool()
{
    POVMSBool result;
    int len = sizeof(result);

    Get(kPOVMSType_Bool, (void *)&result, &len);

    return result;
}

POVMSType POVMS_Attribute::GetType()
{
    POVMSType result;
    int len = sizeof(result);

    Get(kPOVMSType_Type, (void *)&result, &len);

    return result;
}

std::vector<POVMSInt> POVMS_Attribute::GetIntVector()
{
    std::vector<POVMSInt> result;
    result.resize(GetVectorSize());
    int len = result.size() * sizeof(POVMSInt);

    Get(kPOVMSType_VectorInt, (void *)&result[0], &len);

    return result;
}

std::vector<POVMSLong> POVMS_Attribute::GetLongVector()
{
    std::vector<POVMSLong> result;
    result.resize(GetVectorSize());
    int len = result.size() * sizeof(POVMSLong);

    Get(kPOVMSType_VectorLong, (void *)&result[0], &len);

    return result;
}

std::vector<POVMSFloat> POVMS_Attribute::GetFloatVector()
{
    std::vector<POVMSFloat> result;
    result.resize(GetVectorSize());
    int len = result.size() * sizeof(POVMSFloat);

    Get(kPOVMSType_VectorFloat, (void *)&result[0], &len);

    return result;
}

std::vector<POVMSType> POVMS_Attribute::GetTypeVector()
{
    std::vector<POVMSType> result;
    result.resize(GetVectorSize());
    int len = result.size() * sizeof(POVMSType);

    Get(kPOVMSType_VectorType, (void *)&result[0], &len);

    return result;
}

int POVMS_Attribute::GetVectorSize() const
{
    switch(Type())
    {
        case kPOVMSType_VectorInt:
            return Size() / sizeof(POVMSInt);
        case kPOVMSType_VectorLong:
            return Size() / sizeof(POVMSLong);
        case kPOVMSType_VectorFloat:
            return Size() / sizeof(POVMSFloat);
        case kPOVMSType_VectorType:
            return Size() / sizeof(POVMSType);
        default:
            throw POV_EXCEPTION_CODE(pov_base::kCannotHandleDataErr);
    }

    return 0;
}


/*****************************************************************************
*
* CLASS
*   POVMS_List
*
* DESCRIPTION
*   Class handling POVMSAttributeList.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_List::POVMS_List()
{
    int err;

    err = POVMSAttrList_New(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

POVMS_List::POVMS_List(POVMSAttributeList& convert)
{
    data = convert;
}

POVMS_List::POVMS_List(const POVMS_List& source)
{
    int err;

    err = POVMSAttrList_Copy(&source.data, &data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

POVMS_List::~POVMS_List() noexcept(false)
{
    int err;

    err = POVMSAttrList_Delete(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

POVMS_List& POVMS_List::operator=(const POVMS_List& source)
{
    int err;

    err = POVMSAttrList_Delete(&data);
    if(err == pov_base::kNoErr)
        err = POVMSAttrList_Copy(&source.data, &data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return *this;
}

void POVMS_List::Append(POVMS_Attribute& item)
{
    int err;

    err = POVMSAttrList_Append(&data, &item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    item.DetachData();
}

void POVMS_List::Append(POVMS_List& item)
{
    int err;

    err = POVMSAttrList_Append(&data, &item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    item.DetachData();
}

void POVMS_List::Append(POVMS_Object& item)
{
    int err;

    err = POVMSAttrList_Append(&data, &item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    item.DetachData();
}

void POVMS_List::AppendN(int cnt, POVMS_Attribute& item)
{
    for(; cnt > 0; cnt--) // TODO - make more efficient
        Append(item);
}

void POVMS_List::AppendN(int cnt, POVMS_List& item)
{
    for(; cnt > 0; cnt--) // TODO - make more efficient
        Append(item);
}

void POVMS_List::AppendN(int cnt, POVMS_Object& item)
{
    for(; cnt > 0; cnt--) // TODO - make more efficient
        Append(item);
}

void POVMS_List::GetNth(int index, POVMS_Attribute& item)
{
    int err;

    err = POVMSAttr_Delete(&item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttrList_GetNth(&data, index, &item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_List::GetNth(int index, POVMS_List& item)
{
    int err;

    err = POVMSAttrList_Delete(&item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttrList_GetNth(&data, index, &item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_List::GetNth(int index, POVMS_Object& item)
{
    int err;

    err = POVMSObject_Delete(&item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSAttrList_GetNth(&data, index, &item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_List::SetNth(int index, POVMS_Attribute& item)
{
    int err;

    err = POVMSAttrList_SetNth(&data, index, &item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    item.DetachData();
}

void POVMS_List::SetNth(int index, POVMS_List& item)
{
    int err;

    err = POVMSAttrList_SetNth(&data, index, &item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    item.DetachData();
}

void POVMS_List::SetNth(int index, POVMS_Object& item)
{
    int err;

    err = POVMSAttrList_SetNth(&data, index, &item.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    item.DetachData();
}

void POVMS_List::RemoveNth(int index)
{
    int err;

    err = POVMSAttrList_RemoveNth(&data, index);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_List::Clear()
{
    int err;

    err = POVMSAttrList_Clear(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

int POVMS_List::GetListSize()
{
    int len = 0;
    int err;

    err = POVMSAttrList_Count(&data, &len);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return len;
}


/*****************************************************************************
*
* CLASS
*   POVMS_Object
*
* DESCRIPTION
*   Class handling POVMSObject.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_Object::POVMS_Object()
{
    // Null object!
}

POVMS_Object::POVMS_Object(POVMSType objclass)
{
    int err;

    err = POVMSObject_New(&data, objclass);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

POVMS_Object::POVMS_Object(POVMSObject& convert)
{
    data = convert;
}

POVMS_Object::POVMS_Object(POVMSObjectPtr convert)
{
    if (convert != nullptr)
        data = *convert;
}

POVMS_Object::POVMS_Object(const POVMS_Object& source)
{
    if(source.IsNull() == false)
    {
        int err;

        err = POVMSObject_Copy(&source.data, &data);
        if(err != pov_base::kNoErr)
            throw POV_EXCEPTION_CODE(err);
    }
}

POVMS_Object::~POVMS_Object() noexcept(false)
{
    int err;

    err = POVMSObject_Delete(&data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

POVMS_Object& POVMS_Object::operator=(const POVMS_Object& source)
{
    int err;

    err = POVMSObject_Delete(&data);
    if((err == pov_base::kNoErr) && (IsNull() == false))
        err = POVMSObject_Copy(&source.data, &data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return *this;
}

void POVMS_Object::Get(POVMSType key, POVMS_Attribute& attr)
{
    int err;

    err = POVMSAttr_Delete(&attr.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSObject_Get(&data, &attr.data, key);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::Get(POVMSType key, POVMS_List& attr)
{
    int err;

    err = POVMSAttrList_Delete(&attr.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSObject_Get(&data, &attr.data, key);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::Get(POVMSType key, POVMS_Object& attr)
{
    int err;

    err = POVMSObject_Delete(&attr.data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
    err = POVMSObject_Get(&data, &attr.data, key);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::Set(POVMSType key, POVMS_Attribute& attr)
{
    int err;

    err = POVMSObject_Set(&data, &attr.data, key);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    attr.DetachData();
}

void POVMS_Object::Set(POVMSType key, POVMS_List& attr)
{
    int err;

    err = POVMSObject_Set(&data, &attr.data, key);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    attr.DetachData();
}

void POVMS_Object::Set(POVMSType key, POVMS_Object& attr)
{
    int err;

    err = POVMSObject_Set(&data, &attr.data, key);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    attr.DetachData();
}

void POVMS_Object::Remove(POVMSType key)
{
    int err;

    err = POVMSObject_Remove(&data, key);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

bool POVMS_Object::Exist(POVMSType key)
{
    return (POVMSObject_Exist(&data, key) == pov_base::kNoErr);
}

void POVMS_Object::Merge(POVMS_Object& source)
{
    int err;

    err = POVMSObject_Merge(&source.data, &data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

const POVMSObject& POVMS_Object::operator*() const
{
    return *((const POVMSObject*)(&data));
}

const POVMSObject* POVMS_Object::operator->() const
{
    return (const POVMSObject*)(&data);
}

POVMSObject POVMS_Object::operator()()
{
    POVMS_Object tc(*this);
    POVMSObject t = *((POVMSObjectPtr)(&tc.data));
    tc.DetachData();
    return t;
}

void POVMS_Object::SetString(POVMSType key, const char *str)
{
    int err;

    err = POVMSUtil_SetString(&data, key, str);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::SetUCS2String(POVMSType key, const POVMSUCS2 *str)
{
    int err;

    err = POVMSUtil_SetUCS2String(&data, key, str);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::SetInt(POVMSType key, POVMSInt value)
{
    int err;

    err = POVMSUtil_SetInt(&data, key, value);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::SetLong(POVMSType key, POVMSLong value)
{
    int err;

    err = POVMSUtil_SetLong(&data, key, value);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::SetFloat(POVMSType key, POVMSFloat value)
{
    int err;

    err = POVMSUtil_SetFloat(&data, key, value);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::SetBool(POVMSType key, POVMSBool value)
{
    int err;

    err = POVMSUtil_SetBool(&data, key, value);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::SetType(POVMSType key, POVMSType value)
{
    int err;

    err = POVMSUtil_SetType(&data, key, value);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::SetVoid(POVMSType key)
{
    int err;

    err = POVMSUtil_SetVoid(&data, key);
    if (err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Object::SetIntVector(POVMSType key, std::vector<POVMSInt>& value)
{
    POVMS_Attribute attr(value);

    Set(key, attr);
}

void POVMS_Object::SetLongVector(POVMSType key, std::vector<POVMSLong>& value)
{
    POVMS_Attribute attr(value);

    Set(key, attr);
}

void POVMS_Object::SetFloatVector(POVMSType key, std::vector<POVMSFloat>& value)
{
    POVMS_Attribute attr(value);

    Set(key, attr);
}

void POVMS_Object::SetTypeVector(POVMSType key, std::vector<POVMSType>& value)
{
    POVMS_Attribute attr(value);

    Set(key, attr);
}

int POVMS_Object::GetStringLength(POVMSType key)
{
    int len = 0;
    int err;

    err = POVMSUtil_GetStringLength(&data, key, &len);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return len;
}

int POVMS_Object::GetString(POVMSType key, char *str, int maxlen)
{
    int err;

    err = POVMSUtil_GetString(&data, key, str, &maxlen);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return maxlen;
}

std::string POVMS_Object::GetString(POVMSType key)
{
    int len = 0;
    int err;

    err = POVMSUtil_GetStringLength(&data, key, &len);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    char *strptr = new char[len];

    err = POVMSUtil_GetString(&data, key, strptr, &len);
    if(err != pov_base::kNoErr)
    {
        delete[] strptr;
        throw POV_EXCEPTION_CODE(err);
    }

    std::string str(strptr);

    delete[] strptr;

    return str;
}

int POVMS_Object::GetUCS2StringLength(POVMSType key)
{
    int len = 0;
    int err;

    err = POVMSUtil_GetUCS2StringLength(&data, key, &len);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return len;
}

int POVMS_Object::GetUCS2String(POVMSType key, POVMSUCS2 *str, int maxlen)
{
    int err;

    err = POVMSUtil_GetUCS2String(&data, key, str, &maxlen);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return maxlen;
}

POVMSUCS2String POVMS_Object::GetUCS2String(POVMSType key)
{
    int len = 0;
    int err;

    err = POVMSUtil_GetUCS2StringLength(&data, key, &len);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    POVMSUCS2 *strptr = new POVMSUCS2[len];

    err = POVMSUtil_GetUCS2String(&data, key, strptr, &len);
    if(err != pov_base::kNoErr)
    {
        delete[] strptr;
        throw POV_EXCEPTION_CODE(err);
    }

    POVMSUCS2String str(strptr);

    delete[] strptr;

    return str;
}

POVMSInt POVMS_Object::GetInt(POVMSType key)
{
    POVMSInt value;
    int err;

    err = POVMSUtil_GetInt(&data, key, &value);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return value;
}

POVMSLong POVMS_Object::GetLong(POVMSType key)
{
    POVMSLong value;
    int err;

    err = POVMSUtil_GetLong(&data, key, &value);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return value;
}

POVMSFloat POVMS_Object::GetFloat(POVMSType key)
{
    POVMSFloat value;
    int err;

    err = POVMSUtil_GetFloat(&data, key, &value);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return value;
}

POVMSBool POVMS_Object::GetBool(POVMSType key)
{
    POVMSBool value;
    int err;

    err = POVMSUtil_GetBool(&data, key, &value);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return value;
}

POVMSType POVMS_Object::GetType(POVMSType key)
{
    POVMSType value;
    int err;

    err = POVMSUtil_GetType(&data, key, &value);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return value;
}

std::vector<POVMSInt> POVMS_Object::GetIntVector(POVMSType key)
{
    POVMS_Attribute attr;

    Get(key, attr);

    return attr.GetIntVector();
}

std::vector<POVMSLong> POVMS_Object::GetLongVector(POVMSType key)
{
    POVMS_Attribute attr;

    Get(key, attr);

    return attr.GetLongVector();
}

std::vector<POVMSFloat> POVMS_Object::GetFloatVector(POVMSType key)
{
    POVMS_Attribute attr;

    Get(key, attr);

    return attr.GetFloatVector();
}

std::vector<POVMSType> POVMS_Object::GetTypeVector(POVMSType key)
{
    POVMS_Attribute attr;

    Get(key, attr);

    return attr.GetTypeVector();
}

std::string POVMS_Object::TryGetString(POVMSType key, const char *alt)
{
    if(Exist(key) == true)
        return GetString(key);

    return std::string(alt);
}

std::string POVMS_Object::TryGetString(POVMSType key, const std::string& alt)
{
    if(Exist(key) == true)
        return GetString(key);

    return alt;
}

POVMSUCS2String POVMS_Object::TryGetUCS2String(POVMSType key, const char *alt)
{
    if(Exist(key) == true)
        return GetUCS2String(key);

    return POVMS_SysToUCS2String(alt);
}

POVMSUCS2String POVMS_Object::TryGetUCS2String(POVMSType key, const POVMSUCS2String& alt)
{
    if(Exist(key) == true)
        return GetUCS2String(key);

    return alt;
}

POVMSInt POVMS_Object::TryGetInt(POVMSType key, POVMSInt alt)
{
    if(Exist(key) == true)
        return GetInt(key);

    return alt;
}

POVMSLong POVMS_Object::TryGetLong(POVMSType key, POVMSLong alt)
{
    if(Exist(key) == true)
        return GetLong(key);

    return alt;
}

POVMSFloat POVMS_Object::TryGetFloat(POVMSType key, POVMSFloat alt)
{
    if(Exist(key) == true)
        return GetFloat(key);

    return alt;
}

POVMSBool POVMS_Object::TryGetBool(POVMSType key, POVMSBool alt)
{
    if(Exist(key) == true)
        return GetBool(key);

    return alt;
}

POVMSType POVMS_Object::TryGetType(POVMSType key, POVMSType alt)
{
    if(Exist(key) == true)
        return GetType(key);

    return alt;
}

void POVMS_Object::Read(InputStream& stream, bool continued, bool headeronly)
{
    POVMSStream headerstream[16];
    POVMSStream *objectstream = nullptr;
    POVMSStream *compressedstream = nullptr;
    int err = pov_base::kNoErr;
    int maxheadersize = 0;
    int datasize = 0;

    try
    {
        err = POVMSObject_Delete(&data);
        if(err != pov_base::kNoErr)
            throw POV_EXCEPTION_CODE(err);

        if(continued == false)
        {
            char header[8];
            POVMSInt version = 0;

            if(!stream.read((void *)headerstream, 12))
                throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);

            datasize = 0;
            maxheadersize = 12; // header

            datasize += POVMSStream_ReadString(header, headerstream + datasize, 8, &maxheadersize);                // header       8 byte
            if(!((header[0] == 'P') && (header[1] == 'O') && (header[2] == 'V') && (header[3] == 'R') &&
                 (header[4] == 'A') && (header[5] == 'Y') && (header[6] == 'M') && (header[7] == 'S')))
                throw POV_EXCEPTION_CODE(pov_base::kVersionErr);

            datasize += POVMSStream_ReadInt(&version, headerstream + datasize, &maxheadersize);                    // version      4 byte
            if(version != 0x0370)
                throw POV_EXCEPTION_CODE(pov_base::kVersionErr);
        }

        if(headeronly == false)
        {
            POVMSInt objectsize = 0;
            POVMSType encoding = kPOVMSRawStreamEncoding;

            if(!stream.read((void *)headerstream, 8))
                throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);

            datasize = 0;
            maxheadersize = 8; // header

            datasize += POVMSStream_ReadInt(&objectsize, headerstream + datasize, &maxheadersize);                 // data size    4 byte
            if(objectsize == 0)
                throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);

            datasize += POVMSStream_ReadType(&encoding, headerstream + datasize, &maxheadersize);                  // encoding     4 byte
            if(encoding == kPOVMSRawStreamEncoding)
            {
                objectstream = new POVMSStream[objectsize];

                if(!stream.read((void *)objectstream, objectsize))                                                 // object       x byte
                    throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);

                if(POVMSStream_Read(&data, objectstream, &objectsize) == 0)
                    throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);
            }
            else if(encoding == kPOVMSGZipStreamEncoding)
            {
                int compressedsize = objectsize;

                datasize = 0;
                maxheadersize = 4; // header

                if(!stream.read((void *)headerstream, 4))
                    throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);

                datasize += POVMSStream_ReadInt(&objectsize, headerstream + datasize, &maxheadersize);             // object size  4 byte
                if(objectsize == 0)
                    throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);

                compressedstream = new POVMSStream[compressedsize];
                objectstream = new POVMSStream[objectsize];

                if(!stream.read((void *)compressedstream, compressedsize))                                         // data         x byte
                    throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);

                // uncompress stream data
                uLongf destlen = (uLongf)objectsize;
                if(uncompress((Bytef *)(objectstream), &destlen, (Bytef *)(compressedstream), (uLongf)(compressedsize)) != Z_OK)
                    throw POV_EXCEPTION_CODE(pov_base::kCannotHandleDataErr);

                if(POVMSStream_Read(&data, objectstream, &objectsize) == 0)
                    throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);
            }
            else
                throw POV_EXCEPTION_CODE(pov_base::kCannotHandleDataErr);
        }

        if (objectstream != nullptr)
            delete[] objectstream;
        objectstream = nullptr;

        if (compressedstream != nullptr)
            delete[] compressedstream;
        compressedstream = nullptr;
    }
    catch(...)
    {
        if (objectstream != nullptr)
            delete[] objectstream;

        if (compressedstream != nullptr)
            delete[] compressedstream;

        throw;
    }
}

void POVMS_Object::Write(OutputStream& stream, bool append, bool compress)
{
    POVMSType encoding = kPOVMSRawStreamEncoding;
    POVMSStream headerstream[16];
    POVMSStream *objectstream = nullptr;
    POVMSStream *compressedstream = nullptr;
    int maxheadersize = 0;
    int maxobjectsize = 0;
    int objectsize = 0;
    int datasize = 0;

    try
    {
        objectsize = POVMSStream_Size(&data);
        if(objectsize == 0)
            throw POV_EXCEPTION_CODE(pov_base::kNullPointerErr);

        if(append == false)
        {
            datasize = 0;
            maxheadersize = 12; // header

            datasize += POVMSStream_WriteString("POVRAYMS", headerstream + datasize, &maxheadersize);              // header       8 byte
            datasize += POVMSStream_WriteInt(0x0370, headerstream + datasize, &maxheadersize);                     // version      4 byte

            if(!stream.write((void *)headerstream, datasize))
                throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);
        }

        objectstream = new POVMSStream[objectsize];
        maxobjectsize = objectsize;
        (void)POVMSStream_Write(&data, objectstream, &maxobjectsize);

#ifdef POVMS_COMPRESSION_ENABLED
        if(compress == true)
        {
            compressedstream = new POVMSStream[objectsize * 2];

            // compress stream data
            uLongf destlen = (uLongf)(objectsize * 2);
            if(compress2((Bytef *)(compressedstream), &destlen, (Bytef *)(objectstream), (uLongf)(objectsize), Z_BEST_COMPRESSION) != Z_OK)
                throw POV_EXCEPTION_CODE(pov_base::kCannotHandleDataErr);

            datasize = 0;
            maxheadersize = 12; // header

            datasize += POVMSStream_WriteInt((POVMSInt)(destlen), headerstream + datasize, &maxheadersize);        // data size    4 byte
            datasize += POVMSStream_WriteType(kPOVMSGZipStreamEncoding, headerstream + datasize, &maxheadersize);  // encoding     4 byte
            datasize += POVMSStream_WriteInt(objectsize, headerstream + datasize, &maxheadersize);                 // object size  4 byte

            if(!stream.write((void *)headerstream, datasize))
                throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);

            if(!stream.write((void *)compressedstream, (int)(destlen)))                                            // data         x byte
                throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);
        }
        else
#endif
        {
            datasize = 0;
            maxheadersize = 8; // header

            datasize += POVMSStream_WriteInt(objectsize, headerstream + datasize, &maxheadersize);                 // object size  4 byte
            datasize += POVMSStream_WriteType(kPOVMSRawStreamEncoding, headerstream + datasize, &maxheadersize);   // encoding     4 byte

            if(!stream.write((void *)headerstream, datasize))
                throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);

            if(!stream.write((void *)objectstream, objectsize))                                                    // object       x byte
                throw POV_EXCEPTION_CODE(pov_base::kFileDataErr);
        }

        if (objectstream != nullptr)
            delete[] objectstream;
        objectstream = nullptr;

        if (compressedstream != nullptr)
            delete[] compressedstream;
        compressedstream = nullptr;
    }
    catch(...)
    {
        if (objectstream != nullptr)
            delete[] objectstream;

        if (compressedstream != nullptr)
            delete[] compressedstream;

        throw;
    }
}


/*****************************************************************************
*
* CLASS
*   POVMS_Message
*
* DESCRIPTION
*   Class handling messages contained in POVMSObjects.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_Message::POVMS_Message()
{
    // Null object!
}

POVMS_Message::POVMS_Message(POVMSType objclass, POVMSType msgclass, POVMSType msgid) : POVMS_Object(objclass)
{
    int err;

    err = POVMSMsg_SetupMessage(&data, msgclass, msgid);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

POVMS_Message::POVMS_Message(POVMS_Object& convert, POVMSType msgclass, POVMSType msgid) : POVMS_Object(convert)
{
    int err;

    err = POVMSMsg_SetupMessage(&data, msgclass, msgid);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

POVMS_Message::POVMS_Message(POVMSObject& convert) : POVMS_Object(convert)
{
}

POVMS_Message::POVMS_Message(POVMSObjectPtr convert) : POVMS_Object(convert)
{
}

POVMS_Message::POVMS_Message(const POVMS_Message& source) : POVMS_Object(source)
{
}

POVMS_Message& POVMS_Message::operator=(const POVMS_Message& source)
{
    int err;

    err = POVMSObject_Delete(&data);
    if((err == pov_base::kNoErr) && (source.IsNull() == false))
        err = POVMSObject_Copy(&source.data, &data);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return *this;
}

POVMSType POVMS_Message::GetClass()
{
    POVMSType type;
    int err;

    err = POVMSMsg_GetMessageClass(&data, &type);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return type;
}

POVMSType POVMS_Message::GetIdentifier()
{
    POVMSType type;
    int err;

    err = POVMSMsg_GetMessageIdentifier(&data, &type);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return type;
}

POVMSAddress POVMS_Message::GetSourceAddress()
{
    POVMSAddress addr;
    int err;

    err = POVMSMsg_GetSourceAddress(&data, &addr);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return addr;
}

POVMSAddress POVMS_Message::GetDestinationAddress()
{
    POVMSAddress addr;
    int err;

    err = POVMSMsg_GetDestinationAddress(&data, &addr);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    return addr;
}

void POVMS_Message::SetSourceAddress(POVMSAddress addr)
{
    int err;

    err = POVMSMsg_SetSourceAddress(&data, addr);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}

void POVMS_Message::SetDestinationAddress(POVMSAddress addr)
{
    int err;

    err = POVMSMsg_SetDestinationAddress(&data, addr);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);
}


/*****************************************************************************
*
* CLASS
*   POVMS_MessageReceiver
*
* DESCRIPTION
*   Class receiving messages.
*
* CHANGES
*   -
*
******************************************************************************/

POVMS_MessageReceiver::POVMS_MessageReceiver(POVMSContext contextref)
{
    context = contextref;
    receivers = nullptr;
}

POVMS_MessageReceiver::~POVMS_MessageReceiver()
{
    while(receivers != nullptr)
        RemoveNode(receivers);

    receivers = nullptr;
    context = nullptr;
}

void POVMS_MessageReceiver::Remove(POVMSType hclass, POVMSType hid)
{
    HandlerNode *nodeptr = nullptr;

    for(nodeptr = receivers; nodeptr != nullptr; nodeptr = nodeptr->next)
    {
        if((nodeptr->hclass == hclass) && ((nodeptr->hid == hid) || (hid == kPOVMSType_WildCard)))
        {
            RemoveNode(nodeptr);
            nodeptr = receivers;
        }
    }
}

POVMSResult POVMS_MessageReceiver::ReceiveHandler(POVMSObjectPtr msg, POVMSObjectPtr result, int mode, void *privatedataptr)
{
    POVMS_MessageReceiver *self = (POVMS_MessageReceiver *)privatedataptr;
    HandlerNode *nodeptr = nullptr;
    POVMSType hclass = kPOVMSType_Null;
    POVMSType hid = kPOVMSType_Null;
    POVMSResult err = pov_base::kNoErr;

    if (self == nullptr)
        err = pov_base::kParamErr;
    if(err == pov_base::kNoErr)
        err = POVMSMsg_GetMessageClass(msg, &hclass);
    if(err == pov_base::kNoErr)
        err = POVMSUtil_GetType(msg, kPOVMSMessageIdentID, &hid);
    for (nodeptr = self->receivers; nodeptr != nullptr && err == pov_base::kNoErr; nodeptr = nodeptr->next)
    {
        if((nodeptr->hclass == hclass) && ((nodeptr->hid == hid) || (nodeptr->hid == kPOVMSType_WildCard)))
        {
            if (nodeptr->handleroo != nullptr)
            {
                POVMS_Message result_obj(result);
                POVMS_Message msg_obj(msg);

                // Note: This try-catch block is only required because msg_obj.DetachData always has to be executed! [trf]
                try
                {
                    nodeptr->handleroo->Call(msg_obj, result_obj, mode);
                }
                catch(...)
                {
                    // result and msg no longer own their resources; if we don't clear them here,
                    // we can get a double-free in a parent method when this method throws an exception.
                    // an example of this is when create scene fails with an out-of-memory exception.
                    memset(result, 0, sizeof(*result));
                    memset(msg, 0, sizeof(*msg));

                    msg_obj.DetachData();
                    throw;
                }

                msg_obj.DetachData();

                if ((result != nullptr) && (result_obj.IsNull() == false))
                    *result = result_obj();
            }
            else if (nodeptr->handler != nullptr)
                nodeptr->handler->Call(msg, result, mode);
            else
                err = pov_base::kNullPointerErr;
        }
    }

    return err;
}

void POVMS_MessageReceiver::AddNodeFront(POVMSType hclass, POVMSType hid, HandlerOO *hooptr, Handler *hptr)
{
    HandlerNode *nodeptr = new HandlerNode;
    int err = pov_base::kNoErr;

    nodeptr->last = nullptr;
    nodeptr->next = nullptr;
    nodeptr->hclass = hclass;
    nodeptr->hid = hid;
    nodeptr->handleroo = hooptr;
    nodeptr->handler = hptr;

    err = POVMS_InstallReceiver(context, ReceiveHandler, hclass, hid, (void *)this);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    nodeptr->last = nullptr;
    nodeptr->next = receivers;
    if (nodeptr->next != nullptr)
        nodeptr->next->last = nodeptr;
    receivers = nodeptr;
}

void POVMS_MessageReceiver::AddNodeBack(POVMSType hclass, POVMSType hid, HandlerOO *hooptr, Handler *hptr)
{
    HandlerNode *nodeptr = new HandlerNode;
    HandlerNode *iptr = nullptr;
    int err = pov_base::kNoErr;

    nodeptr->last = nullptr;
    nodeptr->next = nullptr;
    nodeptr->hclass = hclass;
    nodeptr->hid = hid;
    nodeptr->handleroo = hooptr;
    nodeptr->handler = hptr;

    err = POVMS_InstallReceiver(context, ReceiveHandler, hclass, hid, (void *)this);
    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    if (receivers == nullptr)
    {
        nodeptr->last = nullptr;
        nodeptr->next = nullptr;
        if (nodeptr->next != nullptr)
            nodeptr->next->last = nodeptr;
        receivers = nodeptr;
    }
    else
    {
        iptr = receivers;
        while (iptr->next != nullptr)
            iptr = iptr->next;
        nodeptr->last = iptr;
        nodeptr->next = nullptr;
        iptr->next = nodeptr;
    }
}

void POVMS_MessageReceiver::RemoveNode(HandlerNode *nodeptr)
{
    if (nodeptr != nullptr)
    {
        (void)POVMS_RemoveReceiver(context, nodeptr->hclass, nodeptr->hid);

        if (nodeptr->last != nullptr)
            nodeptr->last->next = nodeptr->next;
        if (nodeptr->next != nullptr)
            nodeptr->next->last = nodeptr->last;
        if(receivers == nodeptr)
            receivers = nodeptr->next;

        if (nodeptr->handleroo != nullptr)
            delete nodeptr->handleroo;
        if (nodeptr->handler != nullptr)
            delete nodeptr->handler;

        delete nodeptr;
    }
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_Send
*
* DESCRIPTION
*   POVMS_SendMessage same as POVMS_Send in povms.cpp, but takes
*   only a POVMS_Message object as arguments and sends it using
*   kPOVMSSendMode_NoReply mode. Note that sending using this mode
*   does not require a POVMS context, which makes sending messages
*   from any thread really easy as long as no reply is needed.
*
* CHANGES
*   -
*
******************************************************************************/

void POVMS_SendMessage(POVMS_Message& msg)
{
    int err;

    err = POVMS_Send(nullptr, &msg.data, nullptr, kPOVMSSendMode_NoReply);

    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    msg.DetachData();
}


/*****************************************************************************
*
* FUNCTION
*   POVMS_Send
*
* DESCRIPTION
*   POVMS_SendMessage same as POVMS_Send in povms.cpp, but takes
*   POVMS_Messages objects as arguments.
*
* CHANGES
*   -
*
******************************************************************************/

void POVMS_SendMessage(POVMSContext contextref, POVMS_Message& msg, POVMS_Message *result, int mode)
{
    int err;

    if (result != nullptr)
        err = POVMS_Send(contextref, &msg.data, &result->data, mode);
    else
        err = POVMS_Send(contextref, &msg.data, nullptr, mode);

    if(err != pov_base::kNoErr)
        throw POV_EXCEPTION_CODE(err);

    msg.DetachData();
}
