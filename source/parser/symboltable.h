//******************************************************************************
///
/// @file parser/symboltable.h
///
/// Declarations for the symbol table handling of the parser.
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

#ifndef POVRAY_PARSER_SYMBOLTABLE_H
#define POVRAY_PARSER_SYMBOLTABLE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "parser/configparser.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// POV-Ray header files (base module)
#include "base/stringtypes.h"

// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (parser module)
#include "parser/parsertypes.h"
#include "parser/reservedwords.h"

namespace pov_parser
{

using namespace pov_base;

//------------------------------------------------------------------------------

const int MAX_NUMBER_OF_TABLES = 100;
const int SYM_TABLE_SIZE = 257;

typedef unsigned short SymTableEntryRefCount;

// Special symbol tables
enum
{
    SYM_TABLE_GLOBAL = 0,       ///< Identifiers declared using #declare (or #local in top-level file), #function, #macro, etc.
};

/// Structure holding information about a symbol
struct Sym_Table_Entry final
{
    Sym_Table_Entry *next;      ///< Reference to next symbol with same hash
    UTF8String name;            ///< Symbol name
    char *Deprecation_Message;  ///< Warning to print if the symbol is deprecated
    void *Data;                 ///< Reference to the symbol value
    TokenId Token_Number;       ///< Unique ID of this symbol
    bool deprecated : 1;
    bool deprecatedOnce : 1;
    bool deprecatedShown : 1;
    SymTableEntryRefCount ref_count; ///< normally 1, but may be greater when passing symbols out of macros
};
using SYM_ENTRY = Sym_Table_Entry; ///< @deprecated

//------------------------------------------------------------------------------

struct SymbolTable final
{
    SymbolTable();
    SymbolTable(const SymbolTable& obj);
    ~SymbolTable();

    // Entry Lifetime

    static SYM_ENTRY* Create_Entry(const UTF8String& Name, TokenId Number);
    static SYM_ENTRY* Copy_Entry(const SYM_ENTRY* oldEntry);
    static SYM_ENTRY* Destroy_Entry(SYM_ENTRY* Entry);

    // Payload Data Lifetime

    static void* Copy_Identifier(void* Data, int Type);
    static void Destroy_Ident_Data(void* Data, int Type);

    // Entry-Table Relationship

    void Add_Entry(SYM_ENTRY *Table_Entry);
    SYM_ENTRY *Add_Symbol(const UTF8String& Name, TokenId Number);
    SYM_ENTRY* Find_Symbol(const char* s) const;
    void Remove_Symbol(const char *Name, bool is_array_elem, void **DataPtr, int ttype);

    static void Acquire_Entry_Reference(SYM_ENTRY *Entry);
    static void Release_Entry_Reference(SYM_ENTRY *Entry);

protected:

    template<typename T> static void* CopyConstructData(const void*);
    template<typename T> static void* CloneData(const void*);
    template<typename T> static void DeleteData(void*);

    SYM_ENTRY* Find_Symbol(const char* s, int hash) const;
    static int get_hash_value(const char *s);

    friend class SymbolStack;

private:

    SYM_ENTRY* mapHashTable[SYM_TABLE_SIZE];
};

using SymbolTablePtr = std::shared_ptr<SymbolTable>;

//------------------------------------------------------------------------------

class SymbolStack final
{
public:

    //------------------------------------------------------------------------------

    int GetGlobalTableIndex() const;
    int GetLocalTableIndex() const;
    bool IsLocalTableIndex(int index) const;

    SymbolTable* GetTable(int index);
    SymbolTable* GetLocalTable();
    SymbolTable* GetGlobalTable();

    // Entry-Table Relationship

    void Add_Entry(int Index, SYM_ENTRY *Table_Entry);

    SYM_ENTRY *Add_Symbol(int Index, const UTF8String& Name, TokenId Number);
    SYM_ENTRY* Find_Symbol(int index, const char* s);
    SYM_ENTRY* Find_Symbol(const char* s, int* pIndex = nullptr);
    void Remove_Symbol(int Index, const char *Name, bool is_array_elem, void **DataPtr, int ttype);

    //------------------------------------------------------------------------------

    SymbolStack();
    ~SymbolStack();

    /// Destroy all symbol tables.
    void Clear();

    /// Create a new local symbol table.
    void PushTable();

    /// Remove the most local symbol table.
    void PopTable();

protected:

    SymbolTable* Tables[MAX_NUMBER_OF_TABLES];
    int Table_Index;
};

}
// end of namespace pov_parser

#endif // POVRAY_PARSER_SYMBOLTABLE_H
