//******************************************************************************
///
/// @file parser/symboltable.cpp
///
/// Implementation of the symbol table handling of the parser.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "parser/symboltable.h"

// C++ variants of C standard header files
#include <cstring>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_mem.h"
#include "base/povassert.h"
#include "base/stringutilities.h"

// POV-Ray header files (core module)
#include "core/material/blendmap.h"
#include "core/material/interior.h"
#include "core/material/normal.h"
#include "core/material/pigment.h"
#include "core/material/texture.h"
#include "core/math/matrix.h"
#include "core/math/spline.h"
#include "core/math/vector.h"
#include "core/scene/atmosphere.h"
#include "core/scene/camera.h"
#include "core/scene/object.h"

// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

using namespace pov;

//******************************************************************************

SymbolTable::SymbolTable()
{
    for (int i = 0; i < SYM_TABLE_SIZE; i++)
        mapHashTable[i] = nullptr;
}

SymbolTable::SymbolTable(const SymbolTable& obj)
{
    for (int i = SYM_TABLE_SIZE - 1; i >= 0; i--)
    {
        SYM_ENTRY* oldEntry = obj.mapHashTable[i];
        while (oldEntry)
        {
            SYM_ENTRY* newEntry = Copy_Entry(oldEntry);
            newEntry->next = obj.mapHashTable[i];
            mapHashTable[i] = newEntry;
            oldEntry = oldEntry->next;
        }
    }
}

SymbolTable::~SymbolTable()
{
    for (int i = SYM_TABLE_SIZE - 1; i >= 0; i--)
    {
        SYM_ENTRY *entry = mapHashTable[i];
        while (entry)
        {
            entry = Destroy_Entry(entry);
        }
    }
}

//------------------------------------------------------------------------------

SYM_ENTRY* SymbolTable::Create_Entry(const UTF8String& Name, TokenId Number)
{
    SYM_ENTRY *New = new SYM_ENTRY();

    New->Token_Number = Number;
    New->Data = nullptr;
    New->deprecated = false;
    New->deprecatedOnce = false;
    New->deprecatedShown = false;
    New->Deprecation_Message = nullptr;
    New->ref_count = 1;
    New->name = Name;

    return New;
}

SYM_ENTRY* SymbolTable::Copy_Entry(const SYM_ENTRY* oldEntry)
{
    SYM_ENTRY* newEntry;

    newEntry = new SYM_ENTRY();

    newEntry->Token_Number = oldEntry->Token_Number;
    newEntry->Data = Copy_Identifier(oldEntry->Data, oldEntry->Token_Number);
    newEntry->deprecated = false;
    newEntry->deprecatedOnce = false;
    newEntry->deprecatedShown = false;
    newEntry->Deprecation_Message = nullptr;
    newEntry->ref_count = 1;
    newEntry->name = oldEntry->name;

    return newEntry;
}

SYM_ENTRY* SymbolTable::Destroy_Entry(SYM_ENTRY *Entry)
{
    SYM_ENTRY *Next;

    if (Entry == nullptr)
        return nullptr;

    // always unhook the entry from hash table (if it is still member of one)
    Next = Entry->next;
    Entry->next = nullptr;

    if (Entry->ref_count <= 0)
        POV_PARSER_PANIC(); // Error("Internal error: Symbol reference counter underflow");
    Entry->ref_count--;

    if (Entry->ref_count == 0)
    {
        Destroy_Ident_Data(Entry->Data, Entry->Token_Number);
        if (Entry->Deprecation_Message != nullptr)
            POV_FREE(Entry->Deprecation_Message);

        delete Entry;
    }

    return Next;
}

//------------------------------------------------------------------------------

void* SymbolTable::Copy_Identifier(void* Data, int Type)
{
    VECTOR_4D* v4p;
    std::size_t len;
    void *New = nullptr;

    if (Data == nullptr)
    {
        return nullptr;
    }

    switch (Type)
    {
        case COLOUR_ID_TOKEN:
            POV_EXPERIMENTAL_ASSERT(Data != nullptr);
            New = CopyConstructData<RGBFTColour>(Data);
            break;
        case VECTOR_ID_TOKEN:
            New = CopyConstructData<Vector3d>(Data);
            break;
        case UV_ID_TOKEN:
            New = CopyConstructData<Vector2d>(Data);
            break;
        case VECTOR_4D_ID_TOKEN:
            v4p = Create_Vector_4D();
            Assign_Vector_4D((*v4p), (*(reinterpret_cast<VECTOR_4D*>(Data))));
            New = v4p;
            break;
        case FLOAT_ID_TOKEN:
            /*
            dp = Create_Float();
            *dp = *(reinterpret_cast<DBL*>(Data));
            New = dp;
            */
            New = CopyConstructData<DBL>(Data);
            break;
        case PIGMENT_ID_TOKEN:
        case DENSITY_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Pigment(reinterpret_cast<PIGMENT *>(Data)));
            break;
        case NORMAL_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Tnormal(reinterpret_cast<TNORMAL *>(Data)));
            break;
        case FINISH_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Finish(reinterpret_cast<FINISH *>(Data)));
            break;
        case MEDIA_ID_TOKEN:
            New = CopyConstructData<Media>(Data);
            break;
        case INTERIOR_ID_TOKEN:
            New = CopyConstructData<Interior>(Data);
            break;
        case MATERIAL_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Material(reinterpret_cast<MATERIAL *>(Data)));
            break;
        case TEXTURE_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Textures(reinterpret_cast<TEXTURE *>(Data)));
            break;
        case OBJECT_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Object(reinterpret_cast<ObjectPtr>(Data)));
            break;
        case COLOUR_MAP_ID_TOKEN:
            New = reinterpret_cast<void*>(new ColourBlendMapPtr(Copy_Blend_Map(*(reinterpret_cast<ColourBlendMapPtr *>(Data)))));
            break;
        case PIGMENT_MAP_ID_TOKEN:
        case DENSITY_MAP_ID_TOKEN:
            New = reinterpret_cast<void*>(new PigmentBlendMapPtr(Copy_Blend_Map(*(reinterpret_cast<PigmentBlendMapPtr *>(Data)))));
            break;
        case SLOPE_MAP_ID_TOKEN:
            New = reinterpret_cast<void*>(new SlopeBlendMapPtr(Copy_Blend_Map(*(reinterpret_cast<SlopeBlendMapPtr *>(Data)))));
            break;
        case NORMAL_MAP_ID_TOKEN:
            New = reinterpret_cast<void*>(new NormalBlendMapPtr(Copy_Blend_Map(*(reinterpret_cast<NormalBlendMapPtr *>(Data)))));
            break;
        case TEXTURE_MAP_ID_TOKEN:
            New = reinterpret_cast<void*>(new TextureBlendMapPtr(Copy_Blend_Map(*(reinterpret_cast<TextureBlendMapPtr *>(Data)))));
            break;
        case TRANSFORM_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Transform(reinterpret_cast<TRANSFORM *>(Data)));
            break;
        case CAMERA_ID_TOKEN:
            New = CopyConstructData<Camera>(Data);
            break;
        case RAINBOW_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Rainbow(reinterpret_cast<RAINBOW *>(Data)));
            break;
        case FOG_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Fog(reinterpret_cast<FOG *>(Data)));
            break;
        case SKYSPHERE_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Skysphere(reinterpret_cast<SKYSPHERE *>(Data)));
            break;
        case STRING_ID_TOKEN:
            len = UCS2_strlen(reinterpret_cast<UCS2*>(Data)) + 1;
            New = reinterpret_cast<UCS2*>(POV_MALLOC(len * sizeof(UCS2), "UCS2 String"));
            std::memcpy(reinterpret_cast<void*>(New), reinterpret_cast<void*>(Data), len * sizeof(UCS2));
            break;
        case ARRAY_ID_TOKEN:
            New = CloneData<Assignable>(Data);
            break;
        case DICTIONARY_ID_TOKEN:
            New = CopyConstructData<SymbolTable>(Data);
            break;
        case FUNCT_ID_TOKEN:
        case VECTFUNCT_ID_TOKEN:
            New = CloneData<Assignable>(Data);
            break;
        case SPLINE_ID_TOKEN:
            New = reinterpret_cast<void*>(Copy_Spline((GenericSpline *)Data));
            break;
        case MACRO_ID_TOKEN:
        case TEMPORARY_MACRO_ID_TOKEN:
        case FILE_ID_TOKEN:
            POV_PARSER_PANIC(); // Cannot copy identifiers of this type.
            break;
        default:
            POV_PARSER_PANIC(); // Unexpected type.
            break;
    }
    return New;
}

void SymbolTable::Destroy_Ident_Data(void *Data, int Type)
{
    if (Data == nullptr)
        return;

    switch (Type)
    {
        case COLOUR_ID_TOKEN:
            DeleteData<RGBFTColour>(Data);
            break;
        case VECTOR_ID_TOKEN:
            DeleteData<Vector3d>(Data);
            break;
        case UV_ID_TOKEN:
            DeleteData<Vector2d>(Data);
            break;
        case VECTOR_4D_ID_TOKEN:
            Destroy_Vector_4D(reinterpret_cast<VECTOR_4D *>(Data));
            break;
        case FLOAT_ID_TOKEN:
            DeleteData<DBL>(Data);
            break;
        case PIGMENT_ID_TOKEN:
        case DENSITY_ID_TOKEN:
            DeleteData<PIGMENT>(Data);
            break;
        case NORMAL_ID_TOKEN:
            DeleteData<TNORMAL>(Data);
            break;
        case FINISH_ID_TOKEN:
            DeleteData<FINISH>(Data);
            break;
        case MEDIA_ID_TOKEN:
            DeleteData<Media>(Data);
            break;
        case INTERIOR_ID_TOKEN:
            DeleteData<InteriorPtr>(Data);
            break;
        case MATERIAL_ID_TOKEN:
            Destroy_Material(reinterpret_cast<MATERIAL *>(Data));
            break;
        case TEXTURE_ID_TOKEN:
            Destroy_Textures(reinterpret_cast<TEXTURE *>(Data));
            break;
        case OBJECT_ID_TOKEN:
            Destroy_Object(reinterpret_cast<ObjectPtr>(Data));
            break;
        case COLOUR_MAP_ID_TOKEN:
            DeleteData<ColourBlendMapPtr>(Data);
            break;
        case PIGMENT_MAP_ID_TOKEN:
        case DENSITY_MAP_ID_TOKEN:
            DeleteData<PigmentBlendMapPtr>(Data);
            break;
        case SLOPE_MAP_ID_TOKEN:
            DeleteData<SlopeBlendMapPtr>(Data);
            break;
        case NORMAL_MAP_ID_TOKEN:
            DeleteData<NormalBlendMapPtr>(Data);
            break;
        case TEXTURE_MAP_ID_TOKEN:
            DeleteData<TextureBlendMapPtr>(Data);
            break;
        case TRANSFORM_ID_TOKEN:
            Destroy_Transform(reinterpret_cast<TRANSFORM *>(Data));
            break;
        case CAMERA_ID_TOKEN:
            DeleteData<Camera>(Data);
            break;
        case RAINBOW_ID_TOKEN:
            DeleteData<RAINBOW>(Data);
            break;
        case FOG_ID_TOKEN:
            DeleteData<FOG>(Data);
            break;
        case SKYSPHERE_ID_TOKEN:
            DeleteData<SKYSPHERE>(Data);
            break;
        case MACRO_ID_TOKEN:
        case TEMPORARY_MACRO_ID_TOKEN:
            DeleteData<Assignable>(Data);
            break;
        case STRING_ID_TOKEN:
            POV_FREE(Data);
            break;
        case ARRAY_ID_TOKEN:
            DeleteData<Assignable>(Data);
            break;
        case DICTIONARY_ID_TOKEN:
            DeleteData<SymbolTable>(Data);
            break;
        case PARAMETER_ID_TOKEN:
            POV_FREE(Data);
            break;
        case FILE_ID_TOKEN:
            DeleteData<Assignable>(Data);
            break;
        case FUNCT_ID_TOKEN:
        case VECTFUNCT_ID_TOKEN:
            DeleteData<Assignable>(Data);
            break;
        case SPLINE_ID_TOKEN:
            Destroy_Spline(reinterpret_cast<GenericSpline *>(Data));
            break;
        default:
            POV_PARSER_PANIC(); // Unexpected type.
            break;
    }
}

//------------------------------------------------------------------------------

void SymbolTable::Add_Entry(SYM_ENTRY *Table_Entry)
{
    int i = get_hash_value(Table_Entry->name.c_str());

    Table_Entry->next = mapHashTable[i];
    mapHashTable[i] = Table_Entry;
}

SYM_ENTRY *SymbolTable::Add_Symbol(const UTF8String& Name, TokenId Number)
{
    SYM_ENTRY *New;

    New = Create_Entry(Name, Number);
    Add_Entry(New);

    return New;
}

SYM_ENTRY* SymbolTable::Find_Symbol(const char* name) const
{
    return Find_Symbol(name, get_hash_value(name));
}

void SymbolTable::Remove_Symbol(const char *Name, bool is_array_elem, void **DataPtr, int ttype)
{
    if (is_array_elem == true)
    {
        POV_EXPERIMENTAL_ASSERT(DataPtr != nullptr);

        POV_EXPERIMENTAL_ASSERT((ttype != FLOAT_TOKEN_CATEGORY) &&
                                (ttype != FLOAT_ID_TOKEN) &&
                                (ttype != VECTOR_TOKEN_CATEGORY) &&
                                (ttype != COLOUR_TOKEN_CATEGORY));

        Destroy_Ident_Data(*DataPtr, ttype);
        *DataPtr = nullptr;
    }
    else
    {
        SYM_ENTRY *Entry;
        SYM_ENTRY **EntryPtr;

        int i = get_hash_value(Name);

        EntryPtr = &(mapHashTable[i]);
        Entry = *EntryPtr;

        while (Entry)
        {
            if (strcmp(Name, Entry->name.c_str()) == 0)
            {
                *EntryPtr = Entry->next;
                Destroy_Entry(Entry);
                return;
            }

            EntryPtr = &(Entry->next);
            Entry = *EntryPtr;
        }

        POV_PARSER_PANIC();
    }
}

void SymbolTable::Acquire_Entry_Reference(SYM_ENTRY *Entry)
{
    if (Entry == nullptr)
        return;
    if (Entry->ref_count >= std::numeric_limits<SymTableEntryRefCount>::max())
        throw POV_EXCEPTION_STRING("Too many unresolved references to symbol");
    Entry->ref_count++;
}

void SymbolTable::Release_Entry_Reference(SYM_ENTRY *Entry)
{
    if (Entry == nullptr)
        return;
    if (Entry->ref_count <= 0)
        throw POV_EXCEPTION_STRING("Internal error: Symbol reference counter underflow");
    Entry->ref_count--;

    if (Entry->ref_count == 0)
    {
        Destroy_Ident_Data(Entry->Data, Entry->Token_Number);
        if (Entry->Deprecation_Message != nullptr)
            POV_FREE(Entry->Deprecation_Message);

        delete Entry;
    }
}

//------------------------------------------------------------------------------

template<typename T> void* SymbolTable::CopyConstructData(const void* data)
{
    return new T(*reinterpret_cast<const T*>(data));
}

template<typename T> void* SymbolTable::CloneData(const void* data)
{
    return reinterpret_cast<const T*>(data)->Clone();
}

template<typename T> void SymbolTable::DeleteData(void* data)
{
    delete reinterpret_cast<T*>(data);
}

//------------------------------------------------------------------------------

SYM_ENTRY* SymbolTable::Find_Symbol(const char* Name, int hash) const
{
    SYM_ENTRY* Entry = mapHashTable[hash];

    while (Entry)
    {
        if (strcmp(Name, Entry->name.c_str()) == 0)
            return Entry;

        Entry = Entry->next;
    }

    return Entry;
}

int SymbolTable::get_hash_value(const char *s)
{
    unsigned int i = 0;

    while (*s)
    {
        i = (i << 1) ^ *s++;
    }

    return((int)(i % SYM_TABLE_SIZE));
}

//******************************************************************************

int SymbolStack::GetGlobalTableIndex() const
{
    return SYM_TABLE_GLOBAL;
}

int SymbolStack::GetLocalTableIndex() const
{
    return Table_Index;
}

bool SymbolStack::IsLocalTableIndex(int index) const
{
    return (index == Table_Index);
}

SymbolTable* SymbolStack::GetTable(int index)
{
    return Tables[index];
}

SymbolTable* SymbolStack::GetLocalTable()
{
    return Tables[Table_Index];
}

SymbolTable* SymbolStack::GetGlobalTable()
{
    return Tables[SYM_TABLE_GLOBAL];
}

//------------------------------------------------------------------------------

void SymbolStack::Add_Entry(int Index, SYM_ENTRY *Table_Entry)
{
    Tables[Index]->Add_Entry(Table_Entry);
}

SYM_ENTRY *SymbolStack::Add_Symbol(int Index, const UTF8String& Name, TokenId Number)
{
    SYM_ENTRY *New;

    New = SymbolTable::Create_Entry(Name, Number);
    Add_Entry(Index, New);

    return New;
}

SYM_ENTRY* SymbolStack::Find_Symbol(int index, const char* name)
{
    return Tables[index]->Find_Symbol(name, SymbolTable::get_hash_value(name));
}

SYM_ENTRY* SymbolStack::Find_Symbol(const char* name, int* pIndex)
{
    SYM_ENTRY *entry;
    int hash = SymbolTable::get_hash_value(name);
    for (int index = Table_Index; index >= SYM_TABLE_GLOBAL; --index)
    {
        entry = Tables[index]->Find_Symbol(name, hash);
        if (entry)
        {
            if (pIndex != nullptr)
                *pIndex = index;
            return entry;
        }
    }
    if (pIndex != nullptr)
        *pIndex = -1;
    return nullptr;
}

void SymbolStack::Remove_Symbol(int Index, const char *Name, bool is_array_elem, void **DataPtr, int ttype)
{
    return Tables[Index]->Remove_Symbol(Name, is_array_elem, DataPtr, ttype);
}

//------------------------------------------------------------------------------

SymbolStack::SymbolStack() :
    Table_Index(-1)
{}

SymbolStack::~SymbolStack()
{
    while (Table_Index >= 0)
        delete Tables[Table_Index--];
}

void SymbolStack::Clear()
{
    while (Table_Index >= 0)
        delete Tables[Table_Index--];
}

void SymbolStack::PushTable()
{
    if ((++Table_Index) == MAX_NUMBER_OF_TABLES)
    {
        Table_Index--;
        throw POV_EXCEPTION_STRING("Too many nested symbol tables");
    }

    Tables[Table_Index] = new SymbolTable();
}

void SymbolStack::PopTable()
{
    delete Tables[Table_Index--];
}

/*
SymbolTable& SymbolStack::GetLocalTable()
{}

SymbolTable& SymbolStack::GetGlobalTable()
{}
*/

//******************************************************************************

}
// end of namespace pov_parser
