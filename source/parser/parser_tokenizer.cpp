//******************************************************************************
///
/// @file parser/parser_tokenizer.cpp
///
/// This module implements the first part of a two part parser for the scene
/// description files.  This phase changes the input file into tokens.
///
/// This module tokenizes the input file and sends the tokens created
/// to the parser (the second stage).  Tokens sent to the parser contain a
/// token ID, the line number of the token, and if necessary, some data for
/// the token.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
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
#include "parser/parser.h"

#include <cctype>

#include <limits>
#include <memory>

#include "base/fileinputoutput.h"
#include "base/stringutilities.h"
#include "base/version_info.h"

#include "core/material/noise.h"
#include "core/scene/scenedata.h"

#include "parser/scanner.h"
#include "parser/rawtokenizer.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

using namespace pov_base;
using namespace pov;

#if POV_DEBUG
unsigned int gBreakpointCounter = 0;
#endif

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define CALL(x) { if (!(x)) return (false); }

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Initialize_Tokenizer()
{
    shared_ptr<IStream> rfile;
    UCS2String actualFileName;

    pre_init_tokenizer();

    rfile = Locate_File(sceneData->inputFile.c_str(), POV_File_Text_POV, actualFileName, true);
    if (rfile == nullptr)
        Error("Cannot open input file.");

    SetInputStream(rfile);

    mHavePendingRawToken = false;

    Got_EOF = false;

    /* Init conditional stack. */

    Cond_Stack.emplace_back();
    Cond_Stack.back().Cond_Type = ROOT_COND;
    Cond_Stack.back().Switch_Value = 0.0;

    init_sym_tables();
    Max_Trace_Level = MAX_TRACE_LEVEL_DEFAULT;
    Had_Max_Trace_Level = false;

#if POV_DEBUG
    gBreakpointCounter = 0;
#endif

    CheckFileSignature();
}


//******************************************************************************


void Parser::CheckFileSignature()
{
    RawToken signature;
    if (GetRawToken(signature, false))
    {
        if (signature.expressionId == SIGNATURE_FUNCT_TOKEN)
        {
            // Found a signature. Switch to the corresponding encoding automatically.
            ///@todo Still need to work on the mechanism to handle string encoding.
            switch (signature.id)
            {
                case UTF8_SIGNATURE_TOKEN:  /* sceneData->stringEncoding = kStringEncoding_UTF8; */ break;
                default:                    POV_PARSER_PANIC();                                     break;
            }
        }
        else
            UngetRawToken(signature);
    }
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::pre_init_tokenizer ()
{
    int i;

    InitCurrentToken();

    line_count = 10;
    token_count = 0;
    Current_Token_Count = 0;

    // make sure these are `nullptr` otherwise cleanup() will crash if we terminate early
    Default_Texture = nullptr;

    Skipping            = false;
    Inside_Ifdef        = false;
    Inside_MacroDef     = false;
    Parsing_Directive   = false;
    parseRawIdentifiers = false;
    parseOptionalRValue = false;
    Table_Index         = -1;

    // TODO - on modern machines it may be faster to do the comparisons for each token
    //        than to access the conversion table.
    for(i = 0; i < TOKEN_COUNT; i++)
    {
        Conversion_Util_Table[i] = TokenId(i);
        if(i < FLOAT_FUNCT_TOKEN)
            Conversion_Util_Table[i] = FLOAT_FUNCT_TOKEN;
        else
        {
            if(i < VECTOR_FUNCT_TOKEN)
                Conversion_Util_Table[i] = VECTOR_FUNCT_TOKEN;
            else
            {
                if(i < COLOUR_KEY_TOKEN)
                    Conversion_Util_Table[i] = COLOUR_KEY_TOKEN;
            }
        }
    }

    // TODO - implement a mechanism to expose to user
    MaxCachedMacroSize = POV_PARSER_MAX_CACHED_MACRO_SIZE;
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Terminate_Tokenizer()
{
    while(Table_Index >= 0)
    {
        Destroy_Table(Table_Index--);
    }

    maIncludeStack.clear();

    Got_EOF = false;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   The main tokenizing routine.  Set up the files and continue parsing
*   until the end of file
*
*   Read a token from the input file and store it in the mToken variable.
*   If the token is an INCLUDE token, then set the include file name and
*   read another token.
*
*   This function performs most of the work involved in tokenizing.  It
*   reads the first character of the token and decides which function to
*   call to tokenize the rest.  For simple tokens, it simply writes them
*   out to the token buffer.
*
* CHANGES
*
******************************************************************************/

void Parser::Get_Token ()
{
    if (mToken.Unget_Token)
    {
        mToken.Unget_Token = false;

        return;
    }

    if (mToken.End_Of_File)
    {
        return;
    }

    InvalidateCurrentToken();

    while (CurrentTokenId() == END_OF_FILE_TOKEN)
    {
        bool fastForwardToDirective = (Skipping && !Parsing_Directive);

        if (!GetRawToken(mToken.raw, fastForwardToDirective))
        {
            if (maIncludeStack.empty())
            {
                if (Cond_Stack.size() != 1)
                    Error("End of file reached but #end expected.");

                InvalidateCurrentToken();
                mToken.End_Of_File = true;
                return;
            }

            // Returning from an include file.

            Got_EOF=false;

            Destroy_Table(Table_Index--);

            GoToBookmark(maIncludeStack.back()); // TODO handle errors
            maIncludeStack.pop_back();

            continue;
        }

        switch (mToken.raw.GetTokenId())
        {
            case IDENTIFIER_TOKEN:
                Read_Symbol(mToken.raw);
                break;

            case FLOAT_TOKEN:
                mToken.Token_Float = mToken.raw.floatValue;
                Write_Token(mToken.raw);
                break;

            case STRING_LITERAL_TOKEN:
                Write_Token(mToken.raw);
                break;

            case HASH_TOKEN:
                if (IsEndOfInvokedMacro())
                {
                    // The `#end` (or, more precisely, the `#`) of any macro currently being
                    // executed gets special treatment.
                    Return_From_Macro();
                    InvalidateCurrentToken();
                }
                else
                    // Start of a regular directive.
                    Parse_Directive(true);
                break;

            default:
                if (parseRawIdentifiers || (mToken.raw.isPseudoIdentifier && (!Parsing_Directive || Inside_Ifdef)))
                    Read_Symbol(mToken.raw);
                else
                {
                    if (mToken.raw.isReservedWord && Inside_Ifdef)
                        Warning("Trying to test whether a reserved keyword is defined. Test result may not be what you expect.");
                    Write_Token(mToken.raw);
                }
                break;
        }
    }

    Current_Token_Count++;
    token_count++;

    if(token_count > TOKEN_OVERFLOW_RESET_COUNT) // NEVER, ever change the operator here! Other code using token_count depends on it!!! [trf]
    {
        token_count = 0;

        if((ElapsedRealTime() - last_progress) > 1000) // update progress at most every second
        {
            SignalProgress(ElapsedRealTime(), Current_Token_Count);

            Cooperate();

            last_progress = ElapsedRealTime();
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Mark that the token has been put back into the input stream.  The next
*   call to Get_Token will return the last-read token instead of reading a
*   new one from the file.
*
* CHANGES
*
******************************************************************************/

void Parser::Unget_Token ()
{
    mToken.Unget_Token = true;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Read in a symbol from the input file. Check to see if it is a reserved
*   word. If it is, write out the appropriate token. Otherwise, write the
*   symbol out to the symbol table and write out an IDENTIFIER token. An
*   identifier token is a token whose token number is greater than the
*   highest reserved word.
*
* CHANGES
*
******************************************************************************/

void Parser::Read_Symbol(const RawToken& rawToken)
{
    int Local_Index, i;
    size_t j;
    int k;
    POV_ARRAY *a;
    SYM_ENTRY *Temp_Entry;
    POV_PARAM *Par;
    DBL val;
    SYM_TABLE *table = nullptr;
    char *dictIndex = nullptr;
    int pseudoDictionary = -1;
    RawToken nextRawToken;
    bool haveNextRawToken;

    if (rawToken.isReservedWord && !parseRawIdentifiers)
    {
        // Normally, this function shouldn't be called with reserved words.
        // Exceptions are a few keywords that behave like identifiers in certain contexts,
        // such as `global` and `local` which may behave like dictionaries.
        POV_PARSER_ASSERT(rawToken.isPseudoIdentifier);

        if (rawToken.id == LOCAL_TOKEN)
        {
            POV_PARSER_ASSERT(!Parsing_Directive || Inside_Ifdef);
            pseudoDictionary = Table_Index;
        }
        else if (rawToken.id == GLOBAL_TOKEN)
        {
            POV_PARSER_ASSERT(!Parsing_Directive || Inside_Ifdef);
            pseudoDictionary = SYM_TABLE_GLOBAL;
        }
        else
        {
            POV_PARSER_PANIC();
        }
    }

    if (!Skipping && !parseRawIdentifiers)
    {
        if (pseudoDictionary >= 0)
        {
            mToken.Token_Id = DICTIONARY_ID_TOKEN;
            mToken.is_array_elem = false;
            mToken.is_mixed_array_elem = false;
            mToken.is_dictionary_elem = false;
            mToken.NumberPtr = &(Temp_Entry->Token_Number);
            mToken.DataPtr   = &(Temp_Entry->Data);

            table = nullptr;
        }
        else
        {
            /* Search tables from newest to oldest */
            int firstIndex = Table_Index;
            int lastIndex  = SYM_TABLE_GLOBAL;
            for (Local_Index = firstIndex; Local_Index >= lastIndex; Local_Index--)
            {
                /* See if it's a previously declared identifier. */
                Temp_Entry = Find_Symbol(Local_Index, rawToken.lexeme.text.c_str());
                if (Temp_Entry != nullptr)
                {
                    if (Temp_Entry->deprecated && !Temp_Entry->deprecatedShown)
                    {
                        Temp_Entry->deprecatedShown = Temp_Entry->deprecatedOnce;
                        Warning("%s", Temp_Entry->Deprecation_Message);
                    }

                    if ((Temp_Entry->Token_Number==MACRO_ID_TOKEN) && (!Inside_Ifdef))
                    {
                        mToken.Data = Temp_Entry->Data;
                        if (IsOkToDeclare())
                        {
                            Invoke_Macro();
                        }
                        else
                        {
                            mToken.Token_Id=MACRO_ID_TOKEN;
                            mToken.is_array_elem = false;
                            mToken.is_mixed_array_elem = false;
                            mToken.is_dictionary_elem = false;
                            mToken.NumberPtr = &(Temp_Entry->Token_Number);
                            mToken.DataPtr   = &(Temp_Entry->Data);
                            Write_Token (mToken.Token_Id, rawToken, Tables[Local_Index]);

                            mToken.context = Local_Index;
                        }
                        return;
                    }

                    mToken.Token_Id  =   Temp_Entry->Token_Number;
                    mToken.is_array_elem = false;
                    mToken.is_mixed_array_elem = false;
                    mToken.is_dictionary_elem = false;
                    mToken.NumberPtr = &(Temp_Entry->Token_Number);
                    mToken.DataPtr   = &(Temp_Entry->Data);

                    table = Tables[Local_Index];

                    break;
                }
            }
        }

        if (table || (pseudoDictionary >= 0))
        {
            bool breakLoop = false;
            while (!breakLoop)
            {
                switch (mToken.Token_Id)
                {
                    case ARRAY_ID_TOKEN:
                        {
                            if (dictIndex)
                                POV_FREE (dictIndex);

                            haveNextRawToken = PeekRawToken(nextRawToken);

                            if (!haveNextRawToken || (nextRawToken.lexeme.category != Lexeme::kOther) || (nextRawToken.lexeme.text != "["))
                            {
                                breakLoop = true;
                                break;
                            }

                            a = reinterpret_cast<POV_ARRAY *>(*(mToken.DataPtr));
                            j = 0;

                            if (a == nullptr)
                                // This happens in e.g. `#declare Foo[A][B]=...` when `Foo` is an
                                // array of arrays and `Foo[A]` is uninitialized.
                                Error("Attempt to access uninitialized nested array.");

                            for (i=0; i <= a->maxDim; i++)
                            {
                                Parse_Square_Begin();
                                val=Parse_Float();
                                k=(int)(val + EPSILON);

                                if ((k < 0) || (val < -EPSILON))
                                {
                                    Error("Negative subscript");
                                }

                                if (k >= a->Sizes[i])
                                {
                                    if (a->resizable)
                                    {
                                        POV_PARSER_ASSERT (a->maxDim == 0);
                                        if (a->DataPtrs.size() <= k)
                                            a->GrowTo(k + 1);
                                    }
                                    else
                                        Error("Array subscript out of range");
                                }
                                j += k * a->Mags[i];
                                Parse_Square_End();
                            }

                            if (!LValue_Ok && !Inside_Ifdef)
                            {
                                // Note that this does not (and must not) trigger in e.g.
                                // `#declare Foo[A][B]=...` when `Foo` is an array of arrays and
                                // `Foo[A]` is uninitialized, because until now we've only seen
                                // `#declare Foo[A]`, which is no reason for concern as it may
                                // just as well be part of `#declare Foo[A]=...` which is fine.
                                if (!a->HasElement(j))
                                    Error("Attempt to access uninitialized array element.");
                            }

                            mToken.DataPtr = &(a->DataPtrs[j]);
                            mToken.is_mixed_array_elem = a->mixedType;
                            mToken.NumberPtr = &(a->ElementType(j));
                            mToken.Token_Id = *mToken.NumberPtr;
                            mToken.is_array_elem = true;
                            mToken.is_dictionary_elem = false;
                        }
                        break;

                    case DICTIONARY_ID_TOKEN:
                        {
                            if (dictIndex)
                                POV_FREE (dictIndex);

                            haveNextRawToken = PeekRawToken(nextRawToken);

                            SYM_TABLE* parentTable = table;
                            if (pseudoDictionary >= 0)
                            {
                                table = Tables [pseudoDictionary];
                                pseudoDictionary = -1;

                                if (!haveNextRawToken ||
                                    (nextRawToken.lexeme.category != Lexeme::kOther) ||
                                    ((nextRawToken.lexeme.text != "[") && (nextRawToken.lexeme.text != ".")))
                                {
                                    if (Inside_Ifdef)
                                    {
                                        Warning("Trying to test whether a reserved keyword is defined. Test result may not be what you expect.");
                                    }
                                    else
                                    {
                                        Get_Token(); // ensures the error is reported at the right token
                                        Expectation_Error("'[' or '.'");
                                    }
                                }
                            }
                            else
                                table = reinterpret_cast<SYM_TABLE *>(*(mToken.DataPtr));

                            if (haveNextRawToken && (nextRawToken.lexeme.category == Lexeme::kOther) && (nextRawToken.lexeme.text == "."))
                            {
                                if (table == nullptr)
                                {
                                    POV_PARSER_ASSERT (mToken.is_array_elem);
                                    Error ("Attempt to access uninitialized array element.");
                                }

                                GET (PERIOD_TOKEN)
                                bool oldParseRawIdentifiers = parseRawIdentifiers;
                                parseRawIdentifiers = true;
                                Get_Token ();
                                parseRawIdentifiers = oldParseRawIdentifiers;

                                if (mToken.Token_Id != IDENTIFIER_TOKEN)
                                    Expectation_Error ("dictionary element identifier");

                                Temp_Entry = Find_Symbol (table, CurrentTokenText().c_str());
                            }
                            else if (haveNextRawToken && (nextRawToken.lexeme.category == Lexeme::kOther) && (nextRawToken.lexeme.text == "["))
                            {
                                if (table == nullptr)
                                {
                                    POV_PARSER_ASSERT (mToken.is_array_elem);
                                    Error ("Attempt to access uninitialized array element.");
                                }

                                Parse_Square_Begin();
                                dictIndex = Parse_C_String();
                                Parse_Square_End();

                                Temp_Entry = Find_Symbol (table, dictIndex);
                            }
                            else
                            {
                                breakLoop = true;
                                table = parentTable;
                                break;
                            }

                            if (Temp_Entry)
                            {
                                mToken.Token_Id      = Temp_Entry->Token_Number;
                                mToken.NumberPtr     = &(Temp_Entry->Token_Number);
                                mToken.DataPtr       = &(Temp_Entry->Data);
                            }
                            else
                            {
                                if (!LValue_Ok && !Inside_Ifdef && !parseOptionalRValue)
                                    Error ("Attempt to access uninitialized dictionary element.");
                                mToken.Token_Id  = IDENTIFIER_TOKEN;
                                mToken.DataPtr   = nullptr;
                                mToken.NumberPtr = nullptr;
                            }
                            mToken.is_array_elem = false;
                            mToken.is_mixed_array_elem = false;
                            mToken.is_dictionary_elem = true;
                        }
                        break;

                    case PARAMETER_ID_TOKEN:
                        {
                            if (dictIndex)
                                POV_FREE(dictIndex);

                            Par              = reinterpret_cast<POV_PARAM *>(Temp_Entry->Data);
                            mToken.Token_Id  = *(Par->NumberPtr);
                            mToken.is_array_elem = false;
                            mToken.is_mixed_array_elem = false;
                            mToken.is_dictionary_elem = false;
                            mToken.NumberPtr = Par->NumberPtr;
                            mToken.DataPtr   = Par->DataPtr;
                        }
                        break;

                    default:
                        breakLoop = true;
                        break;
                }
            }

            Write_Token (mToken.Token_Id, rawToken, table);

            if (mToken.DataPtr != nullptr)
                mToken.Data = *(mToken.DataPtr);
            mToken.context = Local_Index;
            if (dictIndex != nullptr)
                mToken.raw.lexeme.text = dictIndex;
            return;
        }
    }

    Write_Token (IDENTIFIER_TOKEN, rawToken);
}

inline void Parser::Write_Token(const RawToken& rawToken, SYM_TABLE *table)
{
    POV_EXPERIMENTAL_ASSERT(mToken.sourceFile == mTokenizer.GetSource());
    mToken.raw            = rawToken;
    mToken.Data           = nullptr;
    mToken.Token_Id       = rawToken.expressionId;
    mToken.Function_Id    = rawToken.GetTokenId();
    mToken.table          = table;
}

inline void Parser::Write_Token(TokenId Token_Id, const RawToken& rawToken, SYM_TABLE *table)
{
    POV_EXPERIMENTAL_ASSERT(mToken.sourceFile == mTokenizer.GetSource());
    mToken.raw            = rawToken;
    mToken.Data           = nullptr;
    mToken.Token_Id       = Conversion_Util_Table[Token_Id];
    mToken.Function_Id    = Token_Id;
    mToken.table          = table;
}


//******************************************************************************

TokenId Parser::CurrentTokenId() const
{
    return mToken.Token_Id;
}

TokenId Parser::CurrentTokenFunctionId() const
{
    return mToken.Function_Id;
}

const UTF8String& Parser::CurrentTokenText() const
{
    return mToken.raw.lexeme.text;
}

const MessageContext& Parser::CurrentTokenMessageContext() const
{
    return mToken;
}

void Parser::InitCurrentToken()
{
    /// @todo Shouldn't these be initialized to -1?
    mToken.raw.lexeme.position.line     = 0;
    mToken.raw.lexeme.position.offset   = 0;
    mToken.raw.lexeme.position.column   = 0;
    mToken.raw.lexeme.text.clear();

    mToken.Unget_Token                  = false;
    mToken.End_Of_File                  = false;
    mToken.Data                         = nullptr;
}

void Parser::InvalidateCurrentToken()
{
    POV_EXPERIMENTAL_ASSERT(!mToken.Unget_Token);
    mToken.Token_Id = END_OF_FILE_TOKEN;
    mToken.is_array_elem        = false;
    mToken.is_mixed_array_elem  = false;
    mToken.is_dictionary_elem   = false;
}

void Parser::StopSkipping()
{
    mToken.Token_Id             = HASH_TOKEN; // TODO FIXME
    mToken.is_array_elem        = false;
    mToken.is_mixed_array_elem  = false;
    mToken.is_dictionary_elem   = false;
}

bool Parser::IsEndOfSkip() const
{
    return (mToken.Token_Id == HASH_TOKEN); // TODO FIXME
}

bool Parser::CurrentTokenIsArrayElement() const
{
    return mToken.is_array_elem;
}

bool Parser::CurrentTokenIsHomogenousArrayElement() const
{
    return (mToken.is_array_elem && !mToken.is_mixed_array_elem);
}

bool Parser::CurrentTokenIsDictionaryElement() const
{
    return mToken.is_dictionary_elem;
}

bool Parser::CurrentTokenIsContainerElement() const
{
    return (mToken.is_array_elem || mToken.is_dictionary_elem);
}

void Parser::SetOkToDeclare(bool ok)
{
    Ok_To_Declare = ok;
}

bool Parser::IsOkToDeclare() const
{
    return Ok_To_Declare;
}

bool Parser::HaveCurrentTokenData() const
{
    return (mToken.Data != nullptr);
}

bool Parser::HaveCurrentFile() const
{
    return mToken.sourceFile != nullptr;
}

const UCS2* Parser::CurrentFileName() const
{
    POV_PARSER_ASSERT(mToken.sourceFile != nullptr);
    return mToken.sourceFile->Name();
}

const LexemePosition& Parser::CurrentFilePosition() const
{
    return mToken.raw.lexeme.position;
}

bool Parser::HaveCurrentMessageContext() const
{
    return mToken.sourceFile != nullptr;
}

const MessageContext& Parser::CurrentMessageContext() const
{
    return mToken;
}

void Parser::SetInputStream(const shared_ptr<IStream>& stream)
{
    mTokenizer.SetInputStream(stream);
    mToken.sourceFile = mTokenizer.GetSource();
}

RawTokenizer::HotBookmark Parser::GetHotBookmark() const
{
    return mTokenizer.GetHotBookmark();
}

bool Parser::GoToBookmark(const RawTokenizer::HotBookmark& bookmark)
{
    if (!mTokenizer.GoToBookmark(bookmark))
        // In case of failure, don't update current token information. Required for proper error reporting.
        return false;
    mToken.sourceFile = mTokenizer.GetSource();
    return true;
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

const char *Parser::Get_Token_String (TokenId Token_Id)
{
    int i;

    for (i = 0; Reserved_Words[i].Token_Name != nullptr; i++)
        if (Reserved_Words[i].Token_Number == Token_Id)
            return (Reserved_Words[i].Token_Name);
    return ("");
}




/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Return a list of keywords seperated with a '\n'. The last keyword does not
*   have a LF after it. The caller is responsible for freeing the memory. This
*   function is intended to be used by GUI implementations that need a keyword
*   list for syntax-coloring edit windows.
*
* CHANGES
*
******************************************************************************/

char *Parser::Get_Reserved_Words (const char *additional_words)
{
    int length = 0;
    int i;

    // Compute the length required for the buffer.

    for (i = 0; Reserved_Words[i].Token_Name != nullptr; i++)
    {
        if (!isalpha (Reserved_Words [i].Token_Name [0]))
            continue;
        if (strchr (Reserved_Words [i].Token_Name, ' ') != nullptr)
            continue;
        length += (int)strlen (Reserved_Words[i].Token_Name) + 1;
    }
    length += (int)strlen (additional_words);

    // Create the buffer.

    char *result = reinterpret_cast<char *>(POV_MALLOC (++length, "Keyword List"));

    // Copy the caller-supplied additional words into the buffer.

    strcpy (result, additional_words);
    char *s = result + strlen (additional_words);

    // Copy our own keywords into the buffer.

    for (i = 0; Reserved_Words[i].Token_Name != nullptr; i++)
    {
        if (!isalpha (Reserved_Words [i].Token_Name [0]))
            continue;
        if (strchr (Reserved_Words [i].Token_Name, ' ') != nullptr)
            continue;
        s += sprintf (s, "%s\n", Reserved_Words[i].Token_Name);
    }
    *--s = '\0';

    return (result);
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

bool Parser::GetRawToken(RawToken& rawToken, bool fastForwardToDirective)
{
    if (mHavePendingRawToken)
    {
        rawToken = mPendingRawToken;
        mHavePendingRawToken = false;
        if (!fastForwardToDirective || ((rawToken.lexeme.category == Lexeme::kOther) && (rawToken.lexeme.text == "#")))
            return true;
    }

    if (fastForwardToDirective)
        return mTokenizer.GetNextDirective(rawToken);
    else
        return mTokenizer.GetNextToken(rawToken);
}

bool Parser::PeekRawToken(RawToken& rawToken)
{
    if (!GetRawToken(rawToken, false))
        return false;

    UngetRawToken(rawToken);
    return true;
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::UngetRawToken(const RawToken& rawToken)
{
    POV_PARSER_ASSERT(!mHavePendingRawToken);
    mPendingRawToken = rawToken;
    mHavePendingRawToken = true;
}


/*****************************************************************************
*
* FUNCTION    Parse_Directive
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR      Chris Young
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

bool Parser::IsEndOfInvokedMacro() const
{
    if (Cond_Stack.empty() || (Cond_Stack.back().Cond_Type != INVOKING_MACRO_COND))
        return false;

    POV_PARSER_ASSERT(Cond_Stack.back().PMac != nullptr);
    return (Cond_Stack.back().PMac->endPosition == CurrentFilePosition()) &&
           (Cond_Stack.back().PMac->source.sourceName == mTokenizer.GetSourceName());
}

void Parser::Parse_Directive(int After_Hash)
{
    DBL Value, Value2;
    int Flag;
    char *ts;
    Macro *PMac = nullptr;
    COND_TYPE Curr_Type = Cond_Stack.back().Cond_Type;
    LexemePosition hashPosition = CurrentFilePosition();

    if (!IsOkToDeclare())
    {
        // Directives aren't allowed here.
        // Re-issue the current token to trigger an expectation error downstream.
        if (After_Hash)
        {
            // Hash tokens normally never get processed into high-level tokens,
            // so we need to construct one now.
            mToken.Token_Id = HASH_TOKEN;
            mToken.is_array_elem = false;
            mToken.is_mixed_array_elem = false;
            mToken.is_dictionary_elem = false;
        }
        mToken.Unget_Token = false;

        return;
    }

    Parsing_Directive = true;

    EXPECT  // we're normally running this loop only once, but a few directives cause it to be looped

        CASE(IFDEF_TOKEN)
            Parsing_Directive = false;
            Inc_CS_Index();

            if (Skipping)
            {
                Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                if (Parse_Ifdef_Param())
                {
                    Cond_Stack.back().Cond_Type=IF_TRUE_COND;
                }
                else
                {
                    Cond_Stack.back().Cond_Type=IF_FALSE_COND;
                    Skip_Tokens(IF_FALSE_COND);
                }
            }
            EXIT
        END_CASE

        CASE(IFNDEF_TOKEN)
            Parsing_Directive = false;
            Inc_CS_Index();

            if (Skipping)
            {
                Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                if (Parse_Ifdef_Param())
                {
                    Cond_Stack.back().Cond_Type=IF_FALSE_COND;
                    Skip_Tokens(IF_FALSE_COND);
                }
                else
                {
                    Cond_Stack.back().Cond_Type=IF_TRUE_COND;
                }
            }
            EXIT
        END_CASE

        CASE(IF_TOKEN)
            Parsing_Directive = false;
            Inc_CS_Index();

            if (Skipping)
            {
                Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                Value=Parse_Cond_Param();

                if (fabs(Value)>EPSILON)
                {
                    Cond_Stack.back().Cond_Type=IF_TRUE_COND;
                }
                else
                {
                    Cond_Stack.back().Cond_Type=IF_FALSE_COND;
                    Skip_Tokens(IF_FALSE_COND);
                }
            }
            EXIT
        END_CASE

        CASE(WHILE_TOKEN)
            Parsing_Directive = false;
            Inc_CS_Index();

            if (Skipping)
            {
                Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                Cond_Stack.back().returnToBookmark = GetHotBookmark();

                Value=Parse_Cond_Param();

                if (fabs(Value)>EPSILON)
                {
                    Cond_Stack.back().Cond_Type = WHILE_COND;
                }
                else
                {
                    Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                    Skip_Tokens(SKIP_TIL_END_COND);
                }
            }
            EXIT
        END_CASE

        CASE(FOR_TOKEN)
            Parsing_Directive = false;
            Inc_CS_Index();

            if (Skipping)
            {
                Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                DBL End, Step;
                if (Parse_For_Param (Cond_Stack.back().Loop_Identifier, &End, &Step))
                {
                    // execute loop
                    Cond_Stack.back().Cond_Type = FOR_COND;
                    Cond_Stack.back().returnToBookmark = GetHotBookmark();
                    Cond_Stack.back().For_Loop_End = End;
                    Cond_Stack.back().For_Loop_Step = Step;
                }
                else
                {
                    // terminate loop before it has even started
                    Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                    Skip_Tokens(SKIP_TIL_END_COND);
                    // need to do some cleanup otherwise deferred via the Cond_Stack
                }
            }
            EXIT
        END_CASE

        CASE(ELSE_TOKEN)
            Parsing_Directive = false;
            switch (Curr_Type)
            {
                case IF_TRUE_COND:
                    Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                    Skip_Tokens(SKIP_TIL_END_COND);
                    break;

                case IF_FALSE_COND:
                    Cond_Stack.back().Cond_Type = ELSE_COND;
                    StopSkipping();
                    UNGET
                    break;

                case CASE_TRUE_COND:
                case SKIP_TIL_END_COND:
                    break;

                case CASE_FALSE_COND:
                    Cond_Stack.back().Cond_Type = CASE_TRUE_COND;
                    if (Skipping)
                    {
                        StopSkipping();
                        UNGET
                    }
                    break;

                default:
                    Error("Mis-matched '#else'.");
            }
            EXIT
        END_CASE

        CASE(ELSEIF_TOKEN)
            Parsing_Directive = false;
            switch (Curr_Type)
            {
                case IF_TRUE_COND:
                    Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                    Skip_Tokens(SKIP_TIL_END_COND);
                    break;

                case IF_FALSE_COND:
                    Value=Parse_Cond_Param();
                    if (fabs(Value)>EPSILON)
                    {
                        Cond_Stack.back().Cond_Type=IF_TRUE_COND;
                        StopSkipping();
                        UNGET
                    }
                    else
                    {
                        // nothing to do, we're staying in IF_FALSE_COND state.
                    }
                    break;

                case SKIP_TIL_END_COND:
                    break;

                default:
                    Error("Mis-matched '#elseif'.");
            }
            EXIT
        END_CASE

        CASE(SWITCH_TOKEN)
            Parsing_Directive = false;
            Inc_CS_Index();

            if (Skipping)
            {
                Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                Cond_Stack.back().Switch_Value=Parse_Cond_Param();
                Cond_Stack.back().Cond_Type=SWITCH_COND;
                Cond_Stack.back().Switch_Case_Ok_Flag=false;
                EXPECT_ONE
                    // NOTE: We actually expect a "#case" or "#range" here; however, this will trigger a nested call
                    // to Parse_Directive, so we'll encounter that CASE_TOKEN or RANGE_TOKEN here only by courtesy of the
                    // respective handler, which will UNGET the token and inform us via the Switch_Case_Ok_Flag
                    // that the CASE_TOKEN or RANGE_TOKEN we encounter here was properly preceded with a hash ("#").
                    CASE2(CASE_TOKEN,RANGE_TOKEN)
                        if (!Cond_Stack.back().Switch_Case_Ok_Flag)
                            Error("#switch not followed by #case or #range.");

                        if (CurrentTokenId() == CASE_TOKEN)
                        {
                            Value=Parse_Cond_Param();
                            Flag = (fabs(Value-Cond_Stack.back().Switch_Value)<EPSILON);
                        }
                        else
                        {
                            Parse_Cond_Param2(&Value,&Value2);
                            Flag = ((Cond_Stack.back().Switch_Value >= Value) &&
                                    (Cond_Stack.back().Switch_Value <= Value2));
                        }

                        if(Flag)
                        {
                            Cond_Stack.back().Cond_Type=CASE_TRUE_COND;
                        }
                        else
                        {
                            Cond_Stack.back().Cond_Type=CASE_FALSE_COND;
                            Skip_Tokens(CASE_FALSE_COND);
                        }
                    END_CASE

                    OTHERWISE
                        Error("#switch not followed by #case or #range.");
                    END_CASE
                END_EXPECT
            }
            EXIT
        END_CASE

        CASE(BREAK_TOKEN)
            Parsing_Directive = false;
            if (!Skipping)
                Break();
            EXIT
        END_CASE

        CASE2(CASE_TOKEN,RANGE_TOKEN)
            Parsing_Directive = false;
            switch(Curr_Type)
            {
                case CASE_TRUE_COND:
                case CASE_FALSE_COND:
                    if (CurrentTokenId() == CASE_TOKEN)
                    {
                        Value=Parse_Cond_Param();
                        Flag = (fabs(Value-Cond_Stack.back().Switch_Value)<EPSILON);
                    }
                    else
                    {
                        Parse_Cond_Param2(&Value,&Value2);
                        Flag = ((Cond_Stack.back().Switch_Value >= Value) &&
                                (Cond_Stack.back().Switch_Value <= Value2));
                    }

                    if(Flag && (Curr_Type==CASE_FALSE_COND))
                    {
                        Cond_Stack.back().Cond_Type=CASE_TRUE_COND;
                        if (Skipping)
                        {
                            StopSkipping();
                            UNGET
                        }
                    }
                    break;

                case SWITCH_COND:
                    Cond_Stack.back().Switch_Case_Ok_Flag=true;
                    UNGET
                    break;

                case SKIP_TIL_END_COND:
                    break;

                default:
                    Error("Mis-matched '#case' or '#range'.");
            }
            EXIT
        END_CASE

        CASE(END_TOKEN)
            Parsing_Directive = false;
            switch (Curr_Type)
            {
                case INVOKING_MACRO_COND:
                    POV_PARSER_PANIC(); // The macro's proper `#end` should have been handled separately by the caller.
                    break;

                case IF_FALSE_COND:
                    StopSkipping();
                    UNGET // TODO FIXME
                    // FALLTHROUGH
                case IF_TRUE_COND:
                case ELSE_COND:
                case CASE_TRUE_COND:
                case CASE_FALSE_COND:
                case DECLARING_MACRO_COND:
                case SKIP_TIL_END_COND:
                    if (Curr_Type==DECLARING_MACRO_COND)
                    {
                        if ((PMac=Cond_Stack.back().PMac) != nullptr)
                        {
                            if (PMac->source.sourceName != CurrentFileName())
                                Error("#macro did not end in file where it started.");

                            PMac->endPosition = hashPosition;
                            POV_OFF_T macroLength = CurrentFilePosition() - PMac->source;
                            /// @todo Re-enable cached macros.
                            if (macroLength <= MaxCachedMacroSize)
                            {
                                PMac->CacheSize = macroLength;
                                PMac->Cache = new unsigned char[PMac->CacheSize];
                                RawTokenizer::HotBookmark pos = GetHotBookmark();
                                mTokenizer.GoToBookmark(PMac->source);
                                if (!mTokenizer.GetRaw(PMac->Cache, PMac->CacheSize))
                                {
                                    delete[] PMac->Cache;
                                    PMac->Cache = nullptr;
                                }
                                GoToBookmark(pos); // TODO handle errors
                            }
                        }
                    }
                    Cond_Stack.pop_back();
                    if (Cond_Stack.empty())
                        Error("Mis-matched '#end'.");
                    if (Skipping)
                    {
                        StopSkipping();
                        UNGET
                    }
                    break;

                case WHILE_COND:
                    if (Cond_Stack.back().returnToBookmark.pSource != mTokenizer.GetSource())
                    {
                        Error("#while loop did not end in file where it started.");
                    }

                    Got_EOF=false;
                    if (!GoToBookmark(Cond_Stack.back().returnToBookmark))
                    {
                        Error("Unable to seek in input file for #while directive.");
                    }

                    Value=Parse_Cond_Param();

                    if (fabs(Value)<EPSILON)
                    {
                        Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                        Skip_Tokens(SKIP_TIL_END_COND);
                    }
                    break;

                case FOR_COND:
                    if (Cond_Stack.back().returnToBookmark.pSource != mTokenizer.GetSource())
                    {
                        Error("#for loop did not end in file where it started.");
                    }

                    Got_EOF=false;
                    if (!GoToBookmark(Cond_Stack.back().returnToBookmark))
                    {
                        Error("Unable to seek in input file for #for directive.");
                    }

                    {
                        SYM_ENTRY* Entry = Find_Symbol(Table_Index, Cond_Stack.back().Loop_Identifier.c_str());
                        if ((Entry == nullptr) || (Entry->Token_Number != FLOAT_ID_TOKEN))
                            Error ("#for loop variable must remain defined and numerical during loop.");

                        DBL* CurrentPtr = reinterpret_cast<DBL *>(Entry->Data);
                        DBL  End        = Cond_Stack.back().For_Loop_End;
                        DBL  Step       = Cond_Stack.back().For_Loop_Step;

                        *CurrentPtr = *CurrentPtr + Step;

                        if ( ((Step > 0) && (*CurrentPtr > End + EPSILON)) ||
                             ((Step < 0) && (*CurrentPtr < End - EPSILON)) )
                        {
                            Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
                            Skip_Tokens(SKIP_TIL_END_COND);
                        }
                    }
                    break;

                default:
                    Error("Mis-matched '#end'.");
            }
            EXIT
        END_CASE

        CASE2 (DECLARE_TOKEN,LOCAL_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
                EXIT
            }
            else
            {
                Parse_Declare(CurrentTokenId() == LOCAL_TOKEN, After_Hash);
                Curr_Type = Cond_Stack.back().Cond_Type;
                if (mToken.Unget_Token)
                {
                    switch (CurrentTokenId())
                    {
                        case HASH_TOKEN:
                            mToken.Unget_Token=false;
                            Parsing_Directive = true;
                            break;

                        case MACRO_ID_TOKEN:
                            Parsing_Directive = true;
                            break;

                        default:
                            EXIT
                    }
                }
                else
                {
                    EXIT
                }
            }
        END_CASE

        CASE (DEFAULT_TOKEN)
            Parsing_Directive = false;
            if ( Skipping )
            {
                UNGET
            }
            else
            {
                Parse_Default();
            }
            EXIT
        END_CASE

        CASE (INCLUDE_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                Open_Include();
            }
            EXIT
        END_CASE

        CASE (FLOAT_FUNCT_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
                EXIT
            }
            else
            {
                switch(CurrentTokenFunctionId())
                {
                    case VERSION_TOKEN:
                        {
                            if (sceneData->languageVersionSet == false && token_count > 1)
                                sceneData->languageVersionLate = true;
                            POV_EXPERIMENTAL_ASSERT(IsOkToDeclare());
                            SetOkToDeclare(false);
                            bool wasParsingVersionDirective = parsingVersionDirective;
                            parsingVersionDirective = true;
                            EXPECT_ONE
                                CASE (UNOFFICIAL_TOKEN)
#if POV_RAY_IS_OFFICIAL
                                    Get_Token();
                                    Error("This file was created for an unofficial version and\ncannot work as-is with this official version.");
#else
                                    // PATCH AUTHORS - you should not enable any extra features unless the
                                    // 'unofficial' keyword is set in the scene file.
#endif
                                END_CASE
                                OTHERWISE
                                    Unget_Token();
                                END_CASE
                            END_EXPECT

                            sceneData->languageVersion = (int)(Parse_Float() * 100 + 0.5);

                            if (sceneData->languageVersion == 371)
                            {
                                Warning("The version of POV-Ray originally developed as v3.7.1 was ultimately "
                                        "released as v3.8.0; '#version 3.71' will probably not work as expected. "
                                        "Use '#version 3.8' instead.");
                            }

                            if ((sceneData->languageVersionLate) && (sceneData->languageVersion >= 380))
                            {
                                // As of POV-Ray v3.7, all scene files are supposed to begin with a `#version` directive.
                                // As of POV-Ray v3.8, we no longer tolerate violation of that rule if the main scene
                                // file claims to be compatible with POV-Ray v3.8 anywhere further down the road.
                                // (We need to be more lax with include files though, as they may just as well be
                                // standard include files that happen to have been updated since the scene was
                                // originally designed.)

                                if (maIncludeStack.empty())
                                    Error("As of POV-Ray v3.7, the '#version' directive must be the first non-comment "
                                          "statement in the scene file. To indicate that your scene will dynamically "
                                          "adapt to whatever POV-Ray version is actually used, start your scene with "
                                          "'#version version;'.");
                            }

                            // Initialize various defaults depending on language version specified.
                            InitDefaults(sceneData->languageVersion);

                            // NB: This must be set _after_ parsing the value, in order for the `#version version`
                            // idiom to work properly, but _before_ any of the following code querying
                            // `sceneData->EffectiveLanguageVersion()`.
                            sceneData->languageVersionSet = true;

                            if (sceneData->explicitNoiseGenerator == false)
                                sceneData->noiseGenerator = (sceneData->EffectiveLanguageVersion() < 350 ?
                                                             kNoiseGen_Original : kNoiseGen_RangeCorrected);
                            // [CLi] if assumed_gamma is not specified in a pre-v3.7 scene, gammaMode defaults to kPOVList_GammaMode_None;
                            // this is enforced later anyway after parsing, but we may need this information /now/ during parsing already
                            switch (sceneData->gammaMode)
                            {
                                case kPOVList_GammaMode_None:
                                case kPOVList_GammaMode_AssumedGamma37Implied:
                                    if (sceneData->EffectiveLanguageVersion() < 370)
                                        sceneData->gammaMode = kPOVList_GammaMode_None;
                                    else
                                        sceneData->gammaMode = kPOVList_GammaMode_AssumedGamma37Implied;
                                    break;
                                case kPOVList_GammaMode_AssumedGamma36:
                                case kPOVList_GammaMode_AssumedGamma37:
                                    if (sceneData->EffectiveLanguageVersion() < 370)
                                        sceneData->gammaMode = kPOVList_GammaMode_AssumedGamma36;
                                    else
                                        sceneData->gammaMode = kPOVList_GammaMode_AssumedGamma37;
                                    break;
                            }
                            Parse_Semi_Colon(false);

                            if (sceneData->EffectiveLanguageVersion() > POV_RAY_VERSION_INT)
                            {
                                Error("Your scene file requires POV-Ray version %g or later!\n", (DBL)(sceneData->EffectiveLanguageVersion() / 100.0));
                            }

                            SetOkToDeclare(true);
                            parsingVersionDirective = wasParsingVersionDirective;
                            Curr_Type = Cond_Stack.back().Cond_Type;
                            if (mToken.Unget_Token && (CurrentTokenId() == HASH_TOKEN))
                            {
                                mToken.Unget_Token=false;
                                Parsing_Directive = true;
                            }
                            else
                            {
                                EXIT
                            }
                        }
                        break;

                    default:
                        UNGET
                        Expectation_Error ("object or directive.");
                        break;
                }
            }
        END_CASE

        CASE(WARNING_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                ts=Parse_C_String();
                if(strlen(ts) > 160) // intentional 160, not 128 [trf]
                {
                    ts[124] = ts[125] = ts[126] = '.';
                    ts[127] = 0;
                }
                Warning("%s", ts);
                POV_FREE(ts);
            }
            EXIT
        END_CASE

        CASE(ERROR_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                ts=Parse_C_String();
                if(strlen(ts) > 160) // intentional 160, not 128 [trf]
                {
                    ts[124] = ts[125] = ts[126] = '.';
                    ts[127] = 0;
                }
                Error("Parse halted by #error directive: %s", ts);
                POV_FREE(ts);
            }
            EXIT
        END_CASE

/* Note: The new message driven output system does not support
   generic user output to the render and statistics streams.
   Both streams are now directed into the debug stream. */
        CASE(RENDER_TOKEN)
        CASE(STATISTICS_TOKEN)
            Warning("#render and #statistics streams are no longer available.\nRedirecting output to #debug stream.");
            // Intentional, redirect output to debug stream.
            // FALLTHROUGH
        FALLTHROUGH_CASE
        CASE(DEBUG_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                ts=Parse_C_String();
                if(strlen(ts) > 200) // intentional 200, not 160
                {
                    ts[156] = ts[157] = ts[158] = '.';
                    ts[159] = 0;
                }
                Debug_Info("%s", ts);
                POV_FREE(ts);
            }
            EXIT
        END_CASE

        CASE(FOPEN_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                Parse_Fopen();
            }
            EXIT
        END_CASE

        CASE(FCLOSE_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                Parse_Fclose();
            }
            EXIT
        END_CASE

        CASE(READ_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                Parse_Read();
            }
            EXIT
        END_CASE

        CASE(WRITE_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                Parse_Write();
            }
            EXIT
        END_CASE

        CASE(UNDEF_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                POV_EXPERIMENTAL_ASSERT(IsOkToDeclare());
                SetOkToDeclare(false);
                EXPECT_ONE
                    CASE (IDENTIFIER_TOKEN)
                        Warning("Attempt to undef unknown identifier");
                    END_CASE

                    CASE (FILE_ID_TOKEN)
                        if (CurrentTokenDataPtr<DATA_FILE*>()->busyParsing)
                            Error("Can't undefine a file identifier inside a file directive that accesses it.");
                        else
                            PossibleError("Undefining an open file identifier. Use '#fclose' instead.");
                        // FALLTHROUGH
                    CASE2 (MACRO_ID_TOKEN, PARAMETER_ID_TOKEN)
                    CASE2 (FUNCT_ID_TOKEN, VECTFUNCT_ID_TOKEN)
                    // These have to match Parse_Declare in parse.cpp! [trf]
                    CASE4 (NORMAL_ID_TOKEN, FINISH_ID_TOKEN, TEXTURE_ID_TOKEN, OBJECT_ID_TOKEN)
                    CASE4 (COLOUR_MAP_ID_TOKEN, TRANSFORM_ID_TOKEN, CAMERA_ID_TOKEN, PIGMENT_ID_TOKEN)
                    CASE4 (SLOPE_MAP_ID_TOKEN, NORMAL_MAP_ID_TOKEN, TEXTURE_MAP_ID_TOKEN, COLOUR_ID_TOKEN)
                    CASE4 (PIGMENT_MAP_ID_TOKEN, MEDIA_ID_TOKEN, STRING_ID_TOKEN, INTERIOR_ID_TOKEN)
                    CASE4 (DENSITY_ID_TOKEN, ARRAY_ID_TOKEN, DENSITY_MAP_ID_TOKEN, UV_ID_TOKEN)
                    CASE4 (VECTOR_4D_ID_TOKEN, RAINBOW_ID_TOKEN, FOG_ID_TOKEN, SKYSPHERE_ID_TOKEN)
                    CASE3 (MATERIAL_ID_TOKEN, SPLINE_ID_TOKEN, DICTIONARY_ID_TOKEN)
                        Remove_Symbol (mToken.table, CurrentTokenText().c_str(), mToken.is_array_elem, mToken.DataPtr, CurrentTokenId());
                        if (mToken.is_mixed_array_elem)
                            *mToken.NumberPtr = IDENTIFIER_TOKEN;
                    END_CASE

                    CASE2 (VECTOR_FUNCT_TOKEN, FLOAT_FUNCT_TOKEN)
                        switch(CurrentTokenFunctionId())
                        {
                            case VECTOR_ID_TOKEN:
                            case FLOAT_ID_TOKEN:
                                Remove_Symbol (mToken.table, CurrentTokenText().c_str(), mToken.is_array_elem, mToken.DataPtr, CurrentTokenId());
                                if (mToken.is_mixed_array_elem)
                                    *mToken.NumberPtr = IDENTIFIER_TOKEN;
                                break;

                            default:
                                Parse_Error(IDENTIFIER_TOKEN);
                                break;
                        }
                    END_CASE

                    OTHERWISE
                        Parse_Error(IDENTIFIER_TOKEN);
                    END_CASE
                END_EXPECT
                SetOkToDeclare(true);
            }
            EXIT
        END_CASE

        CASE (MACRO_ID_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                Invoke_Macro();
            }
            EXIT
        END_CASE

        CASE (MACRO_TOKEN)
            Parsing_Directive = false;
            Inc_CS_Index();
            if (!Skipping)
            {
                if (Inside_MacroDef)
                {
                    Error("Cannot nest macro definitions");
                }
                Inside_MacroDef=true;
                PMac=Parse_Macro();
                Inside_MacroDef=false;
            }
            Cond_Stack.back().Cond_Type = DECLARING_MACRO_COND;
            Cond_Stack.back().PMac      = PMac;
            Skip_Tokens(DECLARING_MACRO_COND);
            EXIT
        END_CASE

#if POV_DEBUG
        CASE(BREAKPOINT_TOKEN)
            Parsing_Directive = false;
            if (Skipping)
            {
                UNGET
            }
            else
            {
                Parse_Breakpoint();
            }
            EXIT
        END_CASE
#endif

        OTHERWISE
            Parsing_Directive = false;
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parsing_Directive = false;

    if (mToken.Unget_Token)
    {
        mToken.Unget_Token = false;
    }
    else
    {
        InvalidateCurrentToken();
    }
}

#if POV_DEBUG
void Parser::Parse_Breakpoint()
{
    // This function is invoked in debug builds whenever the `#breakpoint` directive is encountered
    // in a scene or include file.
    // Control flow is honored, e.g. `#if(0) #breakpoint #else #breakpoint #end` will trigger this
    // function only on the second `#breakpoint` directive.

    // To use the `#breakpoint` directive to immediately break program execution, place an
    // unconditional breakpoint here.

    // To use the `#breakpoint` directive to prime a breakpoint elsewhere, make that breakpoint
    // conditional, testing for `gBreakpointCounter > 0`.

    ++gBreakpointCounter;
}
#endif


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Open_Include()
{
    char *asciiFileName;
    UCS2String formalFileName; // Name the file is known by to the user.

    asciiFileName = Parse_C_String(true);
    formalFileName = ASCIItoUCS2String(asciiFileName);
    POV_FREE(asciiFileName);

    IncludeHeader(formalFileName);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Skip_Tokens(COND_TYPE cond)
{
    int Temp      = Cond_Stack.size();
    int Prev_Skip = Skipping;

    Skipping=true;

    while ((Cond_Stack.size() > Temp) || ((Cond_Stack.size() == Temp) && (Cond_Stack.back().Cond_Type == cond)))
    {
        Get_Token();
    }

    Skipping=Prev_Skip;

    if (IsEndOfSkip())
    {
        InvalidateCurrentToken();
        mToken.Unget_Token = false;
    }
    else
    {
        UNGET
    }
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Break()
{
    int Prev_Skip = Skipping;

    Skipping=true;

    // Skip to the end of any non-break-able blocks, e.g. `#if` blocks.
    while ( (Cond_Stack.size() > 1) &&
            (Cond_Stack.back().Cond_Type != WHILE_COND) &&
            (Cond_Stack.back().Cond_Type != FOR_COND) &&
            (Cond_Stack.back().Cond_Type != CASE_TRUE_COND) &&
            (Cond_Stack.back().Cond_Type != INVOKING_MACRO_COND) )
    {
        Get_Token();
    }

    // We should now be immediately in a break-able block.
    if (Cond_Stack.size() == 1)
        Error ("Invalid context for #break");

    if (Cond_Stack.back().Cond_Type == INVOKING_MACRO_COND)
    {
        Skipping=Prev_Skip;
        Return_From_Macro();
        return;
    }

    Cond_Stack.back().Cond_Type = SKIP_TIL_END_COND;
    Skip_Tokens (SKIP_TIL_END_COND);

    Skipping=Prev_Skip;

    POV_EXPERIMENTAL_ASSERT(!IsEndOfSkip());
}


/*****************************************************************************
*
* FUNCTION
*
*   get_hash_value
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate hash value for a given string.
*
* CHANGES
*
*   Apr 1996 : Creation.
*
******************************************************************************/

int Parser::get_hash_value(const char *s)
{
    unsigned int i = 0;

    while (*s)
    {
        i = (i << 1) ^ *s++;
    }

    return((int)(i % SYM_TABLE_SIZE));
}



/*****************************************************************************
*
* FUNCTION
*
*   init_sym_tables
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Chris Young
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::init_sym_tables()
{
    Add_Sym_Table(); // global symbols
}

Parser::SYM_TABLE *Parser::Create_Sym_Table()
{
    SYM_TABLE *New = reinterpret_cast<SYM_TABLE *>(POV_MALLOC(sizeof(SYM_TABLE),"symbol table"));

    for (int i = 0; i < SYM_TABLE_SIZE; i++)
    {
        New->Table[i] = nullptr;
    }

    return New;
}

void Parser::Add_Sym_Table()
{
    if ((++Table_Index)==MAX_NUMBER_OF_TABLES)
    {
        Table_Index--;
        Error("Too many nested symbol tables");
    }

    Tables[Table_Index] = Create_Sym_Table();
}

void Parser::Destroy_Sym_Table (Parser::SYM_TABLE *Table)
{
    SYM_ENTRY *Entry;

    for(int i = SYM_TABLE_SIZE - 1; i >= 0; i--)
    {
        Entry = Table->Table[i];

        while(Entry)
        {
            Entry = Destroy_Entry (Entry);
        }

        Table->Table[i] = nullptr;
    }

    POV_FREE(Table);

}

void Parser::Destroy_Table(int index)
{
    Destroy_Sym_Table (Tables[index]);
}

Parser::SYM_TABLE *Parser::Copy_Sym_Table (const Parser::SYM_TABLE *table)
{
    SYM_TABLE *newTable = Create_Sym_Table();
    SYM_ENTRY *Entry, *newEntry;

    for(int i = SYM_TABLE_SIZE - 1; i >= 0; i--)
    {
        Entry = table->Table[i];

        while(Entry)
        {
            newEntry = Copy_Entry (Entry);
            newEntry->next = newTable->Table[i];
            newTable->Table[i] = newEntry;
            Entry = Entry->next;
        }
    }

    return newTable;
}

SYM_ENTRY *Parser::Create_Entry (const UTF8String& Name, TokenId Number)
{
    SYM_ENTRY *New;

    New = new SYM_ENTRY();

    New->Token_Number        = Number;
    New->Data                = nullptr;
    New->deprecated          = false;
    New->deprecatedOnce      = false;
    New->deprecatedShown     = false;
    New->Deprecation_Message = nullptr;
    New->ref_count           = 1;
    New->name                = Name;

    return(New);
}

SYM_ENTRY *Parser::Copy_Entry (const SYM_ENTRY *oldEntry)
{
    SYM_ENTRY *newEntry;

    newEntry = new SYM_ENTRY();

    newEntry->Token_Number        = oldEntry->Token_Number;
    newEntry->Data                = Copy_Identifier (oldEntry->Data, oldEntry->Token_Number);
    newEntry->deprecated          = false;
    newEntry->deprecatedOnce      = false;
    newEntry->deprecatedShown     = false;
    newEntry->Deprecation_Message = nullptr;
    newEntry->ref_count           = 1;
    newEntry->name                = oldEntry->name;

    return newEntry;
}

void Parser::Acquire_Entry_Reference (SYM_ENTRY *Entry)
{
    if (Entry == nullptr)
        return;
    if (Entry->ref_count >= std::numeric_limits<SymTableEntryRefCount>::max())
        Error("Too many unresolved references to symbol");
    Entry->ref_count ++;
}

void Parser::Release_Entry_Reference (SYM_TABLE *table, SYM_ENTRY *Entry)
{
    if (Entry == nullptr)
        return;
    if (Entry->ref_count <= 0)
        Error("Internal error: Symbol reference counter underflow");
    Entry->ref_count --;

    if (Entry->ref_count == 0)
    {
        Destroy_Ident_Data (Entry->Data, Entry->Token_Number);
        if (Entry->Deprecation_Message != nullptr)
            POV_FREE(Entry->Deprecation_Message);

        delete Entry;
    }
}

SYM_ENTRY *Parser::Destroy_Entry (SYM_ENTRY *Entry)
{
    SYM_ENTRY *Next;

    if (Entry == nullptr)
        return nullptr;

    // always unhook the entry from hash table (if it is still member of one)
    Next = Entry->next;
    Entry->next = nullptr;

    if (Entry->ref_count <= 0)
        Error("Internal error: Symbol reference counter underflow");
    Entry->ref_count --;

    if (Entry->ref_count == 0)
    {
        Destroy_Ident_Data (Entry->Data, Entry->Token_Number);
        if (Entry->Deprecation_Message != nullptr)
            POV_FREE(Entry->Deprecation_Message);

        delete Entry;
    }

    return Next;
}


void Parser::Add_Entry (SYM_TABLE *table, SYM_ENTRY *Table_Entry)
{
    int i = get_hash_value(Table_Entry->name.c_str());

    Table_Entry->next       = table->Table[i];
    table->Table[i] = Table_Entry;
}

void Parser::Add_Entry (int Index,SYM_ENTRY *Table_Entry)
{
    Add_Entry (Tables[Index], Table_Entry);
}


SYM_ENTRY *Parser::Add_Symbol (SYM_TABLE *table, const UTF8String& Name, TokenId Number)
{
    SYM_ENTRY *New;

    New = Create_Entry (Name, Number);
    Add_Entry (table, New);

    return(New);
}

SYM_ENTRY *Parser::Add_Symbol (int Index, const UTF8String& Name, TokenId Number)
{
    SYM_ENTRY *New;

    New = Create_Entry(Name, Number);
    Add_Entry(Index,New);

    return(New);
}


SYM_ENTRY *Parser::Find_Symbol (const SYM_TABLE *table, const char *Name, int hash)
{
    SYM_ENTRY *Entry;

    Entry = table->Table[hash];

    while (Entry)
    {
        if (strcmp(Name, Entry->name.c_str()) == 0)
        {
            return(Entry);
        }

        Entry = Entry->next;
    }

    return(Entry);
}

SYM_ENTRY *Parser::Find_Symbol (const SYM_TABLE *table, const char *name)
{
    return Find_Symbol (table, name, get_hash_value (name));
}

SYM_ENTRY *Parser::Find_Symbol (int index, const char *name)
{
    return Find_Symbol (Tables[index], name, get_hash_value(name));
}

SYM_ENTRY *Parser::Find_Symbol (const char *name)
{
    SYM_ENTRY *entry;
    int hash = get_hash_value (name);
    for (int index = Table_Index; index > 0; --index)
    {
        entry = Find_Symbol (Tables[index], name, hash);
        if (entry)
            return entry;
    }
    return nullptr;
}


void Parser::Remove_Symbol (SYM_TABLE *table, const char *Name, bool is_array_elem, void **DataPtr, int ttype)
{
    if(is_array_elem == true)
    {
        if (DataPtr == nullptr)
            Error("Invalid array element!");

        if(ttype == FLOAT_FUNCT_TOKEN)
            ttype = FLOAT_ID_TOKEN;
        else if(ttype == VECTOR_FUNCT_TOKEN)
            ttype = VECTOR_ID_TOKEN;
        else if(ttype == COLOUR_KEY_TOKEN)
            ttype = COLOUR_ID_TOKEN;

        Destroy_Ident_Data (*DataPtr, ttype);
        *DataPtr = nullptr;
    }
    else
    {
        SYM_ENTRY *Entry;
        SYM_ENTRY **EntryPtr;

        int i = get_hash_value(Name);

        EntryPtr = &(table->Table[i]);
        Entry    = *EntryPtr;

        while (Entry)
        {
            if (strcmp(Name, Entry->name.c_str()) == 0)
            {
                *EntryPtr = Entry->next;
                Destroy_Entry (Entry);
                return;
            }

            EntryPtr = &(Entry->next);
            Entry    = *EntryPtr;
        }

        Error("Tried to free undefined symbol");
    }
}

void Parser::Remove_Symbol (int Index, const char *Name, bool is_array_elem, void **DataPtr, int ttype)
{
    return Remove_Symbol (Tables[Index], Name, is_array_elem, DataPtr, ttype);
}

void Parser::Check_Macro_Vers(void)
{
    if (sceneData->EffectiveLanguageVersion() < 310)
    {
        Error("Macros require #version 3.1 or later but #version %x.%02d is set.",
               sceneData->EffectiveLanguageVersion() / 100, sceneData->EffectiveLanguageVersion() % 100);
    }
}

Parser::Macro *Parser::Parse_Macro()
{
    Macro *New;
    SYM_ENTRY *Table_Entry = nullptr;
    bool oldOkToDeclare = IsOkToDeclare();
    MacroParameter newParameter;

    Check_Macro_Vers();

    SetOkToDeclare(false);

    EXPECT_ONE
        CASE (IDENTIFIER_TOKEN)
            Table_Entry = Add_Symbol (SYM_TABLE_GLOBAL, CurrentTokenText(), TEMPORARY_MACRO_ID_TOKEN);
        END_CASE

        CASE (MACRO_ID_TOKEN)
            Remove_Symbol(SYM_TABLE_GLOBAL, CurrentTokenText().c_str(), false, nullptr, 0);
            Table_Entry = Add_Symbol (SYM_TABLE_GLOBAL, CurrentTokenText(), TEMPORARY_MACRO_ID_TOKEN);
        END_CASE

        OTHERWISE
            Parse_Error(IDENTIFIER_TOKEN);
        END_CASE
    END_EXPECT

    New = new Macro(CurrentTokenText().c_str());

    Table_Entry->Data=reinterpret_cast<void *>(New);

    EXPECT_ONE
        CASE (LEFT_PAREN_TOKEN)
            UNGET
        END_CASE
        CASE (TEMPORARY_MACRO_ID_TOKEN)
            Error( "Can't invoke a macro while declaring its parameters");
        END_CASE
        OTHERWISE
            Expectation_Error ("identifier");
        END_CASE
    END_EXPECT

    Parse_Paren_Begin();

    newParameter.optional = false;
    EXPECT
        CASE (OPTIONAL_TOKEN)
            newParameter.optional = true;
        END_CASE

        CASE3 (MACRO_ID_TOKEN, IDENTIFIER_TOKEN, PARAMETER_ID_TOKEN)
        CASE3 (FILE_ID_TOKEN,  FUNCT_ID_TOKEN, VECTFUNCT_ID_TOKEN)
        // These have to match Parse_Declare in parse.cpp! [trf]
        CASE4 (NORMAL_ID_TOKEN, FINISH_ID_TOKEN, TEXTURE_ID_TOKEN, OBJECT_ID_TOKEN)
        CASE4 (COLOUR_MAP_ID_TOKEN, TRANSFORM_ID_TOKEN, CAMERA_ID_TOKEN, PIGMENT_ID_TOKEN)
        CASE4 (SLOPE_MAP_ID_TOKEN, NORMAL_MAP_ID_TOKEN, TEXTURE_MAP_ID_TOKEN, COLOUR_ID_TOKEN)
        CASE4 (PIGMENT_MAP_ID_TOKEN, MEDIA_ID_TOKEN, STRING_ID_TOKEN, INTERIOR_ID_TOKEN)
        CASE4 (DENSITY_ID_TOKEN, ARRAY_ID_TOKEN, DENSITY_MAP_ID_TOKEN, UV_ID_TOKEN)
        CASE4 (VECTOR_4D_ID_TOKEN, RAINBOW_ID_TOKEN, FOG_ID_TOKEN, SKYSPHERE_ID_TOKEN)
        CASE3 (MATERIAL_ID_TOKEN, SPLINE_ID_TOKEN, DICTIONARY_ID_TOKEN)
            newParameter.name = POV_STRDUP(CurrentTokenText().c_str());
            New->parameters.push_back(newParameter);
            Parse_Comma();
            newParameter.optional = false;
        END_CASE

        CASE2 (VECTOR_FUNCT_TOKEN, FLOAT_FUNCT_TOKEN)
            switch(CurrentTokenFunctionId())
            {
                case VECTOR_ID_TOKEN:
                case FLOAT_ID_TOKEN:
                    newParameter.name = POV_STRDUP(CurrentTokenText().c_str());
                    New->parameters.push_back(newParameter);
                    Parse_Comma();
                    newParameter.optional = false;
                    break;

                default:
                    Expectation_Error ("identifier");
                    break;
            }
        END_CASE

        CASE(RIGHT_PAREN_TOKEN)
            UNGET
            if (newParameter.optional)
                Expectation_Error ("identifier");
            EXIT
        END_CASE

        CASE(TEMPORARY_MACRO_ID_TOKEN)
            Error( "Can't invoke a macro while declaring its parameters");
        END_CASE

        OTHERWISE
            Expectation_Error ("identifier");
        END_CASE
    END_EXPECT

    SetOkToDeclare(oldOkToDeclare);

    Table_Entry->Token_Number = MACRO_ID_TOKEN;

    New->source = mTokenizer.GetColdBookmark();

    Parse_Paren_End();

    Check_Macro_Vers();

    return (New);
}


void Parser::Invoke_Macro()
{
    Macro *PMac = CurrentTokenDataPtr<Macro*>();
    SYM_ENTRY **Table_Entries = nullptr;
    int i,Local_Index;

    Inc_CS_Index();

    if (PMac == nullptr)
    {
        if (mToken.DataPtr != nullptr)
            PMac = reinterpret_cast<Macro*>(*(mToken.DataPtr));
        else
            Error("Error in Invoke_Macro");
    }

    Check_Macro_Vers();

    Parse_Paren_Begin();

    if (PMac->parameters.size() > 0)
    {
        Table_Entries = reinterpret_cast<SYM_ENTRY **>(POV_MALLOC(sizeof(SYM_ENTRY *)*PMac->parameters.size(),"parameters"));

        /* We must parse all parameters before adding new symbol table
           or adding entries.  Otherwise recursion won't always work.
         */

        Local_Index = Table_Index;

        bool properlyDelimited; // true if we found the previous parameter properly delimited with a comma

        properlyDelimited = true;
        for (i=0; i<PMac->parameters.size(); i++)
        {
            bool finalParameter = (i == PMac->parameters.size()-1);
            Table_Entries[i] = Create_Entry (PMac->parameters[i].name, IDENTIFIER_TOKEN);
            if (!Parse_RValue(IDENTIFIER_TOKEN, &(Table_Entries[i]->Token_Number), &(Table_Entries[i]->Data), nullptr, true, false, true, true, true, Local_Index))
            {
                EXPECT_ONE
                    CASE (IDENTIFIER_TOKEN)
                        // an uninitialized identifier was passed
                        if (!PMac->parameters[i].optional)
                            Error("Cannot pass uninitialized identifier as non-optional macro parameter.");
                    END_CASE

                    CASE (RIGHT_PAREN_TOKEN)
                        if (!(finalParameter && properlyDelimited))
                            // the parameter list was closed prematurely
                            Error("Expected %d parameters but only %d found.",PMac->parameters.size(),i);
                        // the parameter was left empty
                        if (!PMac->parameters[i].optional)
                            Error("Cannot omit non-optional macro parameter %d.",i+1);
                        UNGET
                    END_CASE

                    CASE (COMMA_TOKEN)
                        // the parameter was left empty
                        if (!PMac->parameters[i].optional)
                            Error("Cannot omit non-optional macro parameter %d.",i+1);
                        UNGET
                    END_CASE

                    OTHERWISE
                        Expectation_Error("macro parameter");
                    END_CASE
                END_EXPECT

                Destroy_Entry (Table_Entries[i]);
                Table_Entries[i] = nullptr;
            }
            properlyDelimited = Parse_Comma();
        }
    }

    Parse_Paren_End();

    Cond_Stack.back().Cond_Type = INVOKING_MACRO_COND;

    Cond_Stack.back().returnToBookmark   = GetHotBookmark();
    Cond_Stack.back().PMac               = PMac;

    /* Gotta have new symbol table in case #local is used */
    Add_Sym_Table();

    if (PMac->parameters.size() > 0)
    {
        for (i=0; i<PMac->parameters.size(); i++)
        {
            if (Table_Entries[i] != nullptr)
                Add_Entry(Table_Index,Table_Entries[i]);
        }

        POV_FREE(Table_Entries);
    }

    if ((PMac->Cache != nullptr) || (PMac->source.sourceName != mTokenizer.GetSourceName()))
    {
        UCS2String ign;
        /* Not in same file */
        Cond_Stack.back().Macro_Same_Flag = false;
        Got_EOF=false;
        shared_ptr<IStream> is;
        if (PMac->Cache)
        {
            is = std::make_shared<IMemStream>(Cond_Stack.back().PMac->Cache, PMac->CacheSize, PMac->source.sourceName, PMac->source.offset);
        }
        else
        {
            is = Locate_File (PMac->source.sourceName, POV_File_Text_Macro, ign, true);
            if (is == nullptr)
                Error ("Cannot open macro file '%s'.", UCS2toASCIIString(PMac->source.sourceName).c_str());
        }
        mTokenizer.SetInputStream(is);
    }
    else
    {
        Cond_Stack.back().Macro_Same_Flag=true;
    }

    Got_EOF=false;
    if (!mTokenizer.GoToBookmark(PMac->source))
    {
        ErrorInfo(mToken, "Invoking macro from here.");
        Error(PMac->source, "Unable to file seek in macro invocation.");
    }

    mToken.sourceFile = mTokenizer.GetSource();

    InvalidateCurrentToken();

    Check_Macro_Vers();

}

void Parser::Return_From_Macro()
{
    Check_Macro_Vers();

    Got_EOF=false;

    if (!GoToBookmark(Cond_Stack.back().returnToBookmark))
    {
        ErrorInfo(mToken, "Returning from macro.");
        Error(Cond_Stack.back().returnToBookmark, "Unable to file seek in return from macro.");
    }

    // Always destroy macro locals
    Destroy_Table(Table_Index--);

    Cond_Stack.pop_back();
    if (Cond_Stack.empty())
        Error("Mis-matched '#end'.");
}

Parser::Macro::Macro(const char *s) :
    Macro_Name(POV_STRDUP(s)),
    Cache(nullptr)
{}

Parser::Macro::~Macro()
{
    int i;

    POV_FREE(Macro_Name);

    for (i=0; i < parameters.size(); i++)
    {
        POV_FREE(parameters[i].name);
    }

    if (Cache != nullptr)
        delete[] Cache;
}

Parser::POV_ARRAY *Parser::Parse_Array_Declare (void)
{
    POV_ARRAY *New;
    int i;
    size_t j;

    New = new POV_ARRAY;
    New->resizable = false;
    New->mixedType = AllowToken(MIXED_TOKEN);

    i=0;
    j=1;

    POV_EXPERIMENTAL_ASSERT(IsOkToDeclare());
    SetOkToDeclare(false);

    while (Parse_Square_Begin(false))
    {
        if (i >= POV_ARRAY::kMaxDimensions)
        {
            Error("Too many array dimensions");
        }
        New->Sizes[i]=(int)(Parse_Float() + EPSILON);
        j *= New->Sizes[i];
        if ( j <= 0) {
            Error("Invalid dimension size for an array");
        }
        i++;
        Parse_Square_End();
    }

    if (i == 0) {
        // new syntax: Dynamically sized one-dimensional array
        New->Sizes[0] = 0;
        New->resizable = true;
        New->maxDim     = 0;
    }
    else
    {
        New->maxDim     = i-1;
        New->DataPtrs.reserve(j);
        New->DataPtrs.assign(j, nullptr);
        if (New->mixedType)
        {
            New->Types.reserve(j);
            New->Types.assign(j, IDENTIFIER_TOKEN);
        }
    }
    New->Type_ = EMPTY_ARRAY_TOKEN;

    j = 1;

    for(i = New->maxDim; i>=0; i--)
    {
        New->Mags[i] = j;
        j *= New->Sizes[i];
    }

    for (i=0; i<New->DataPtrs.size(); i++)
    {
        POV_PARSER_ASSERT (New->DataPtrs[i] == nullptr);
    }

    EXPECT_ONE
        CASE2(LEFT_CURLY_TOKEN, OPTIONAL_TOKEN)
            UNGET
                Parse_Initalizer(0,0,New);
        END_CASE

        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT

    SetOkToDeclare(true);
    return(New);
};


Parser::SYM_TABLE *Parser::Parse_Dictionary_Declare()
{
    SYM_TABLE *newDictionary;
    SYM_ENTRY *newEntry;
    bool oldParseRawIdentifiers;
    UTF8String dictIndex;

    newDictionary = Create_Sym_Table();

    // TODO REVIEW - maybe we need `SetOkToDeclare(false)`?

    if (!Parse_Begin (false))
        return newDictionary;

    EXPECT
        CASE (PERIOD_TOKEN)
            oldParseRawIdentifiers = parseRawIdentifiers;
            parseRawIdentifiers = true;
            Get_Token();
            parseRawIdentifiers = oldParseRawIdentifiers;
            if (CurrentTokenId() != IDENTIFIER_TOKEN)
                Expectation_Error ("dictionary element identifier");
            newEntry = Add_Symbol (newDictionary, CurrentTokenText(), IDENTIFIER_TOKEN);

            GET (COLON_TOKEN);

            if (!Parse_RValue (IDENTIFIER_TOKEN, &(newEntry->Token_Number), &(newEntry->Data), newEntry, false, false, true, true, false, MAX_NUMBER_OF_TABLES))
                Expectation_Error("RValue");

            Parse_Comma();
        END_CASE

        CASE (LEFT_SQUARE_TOKEN)
            UNGET
            Parse_Square_Begin();
            ParseString(dictIndex);
            newEntry = Add_Symbol (newDictionary, dictIndex, IDENTIFIER_TOKEN);
            Parse_Square_End();

            GET (COLON_TOKEN);

            if (!Parse_RValue (IDENTIFIER_TOKEN, &(newEntry->Token_Number), &(newEntry->Data), newEntry, false, false, true, true, false, MAX_NUMBER_OF_TABLES))
                Expectation_Error("RValue");

            Parse_Comma();
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Parse_End();

    return newDictionary;

};

void Parser::Parse_Initalizer (int Sub, size_t Base, POV_ARRAY *a)
{
    int i;
    bool optional = false;

    EXPECT_ONE
        CASE(OPTIONAL_TOKEN)
            optional = true;
        END_CASE

        OTHERWISE
            UNGET
        END_CASE
    END_EXPECT

    Parse_Begin();
    if (Sub < a->maxDim)
    {
        for(i=0; i < a->Sizes[Sub]; i++)
        {
            Parse_Initalizer(Sub+1,i*a->Mags[Sub]+Base,a);
        }
    }
    else
    {
        bool properlyDelimited = true;
        bool finalParameter = (!a->resizable && (a->Sizes[Sub] == 0));
        for(i=0; !finalParameter; i++)
        {
            if (a->resizable)
                a->Grow();
            else
                finalParameter = (i == (a->Sizes[Sub]-1));

            if (!Parse_RValue (a->ElementType(Base+i), &(a->ElementType(Base+i)), &(a->DataPtrs[Base+i]),
                               nullptr, false, false, true, false, true, MAX_NUMBER_OF_TABLES))
            {
                EXPECT_ONE
                    CASE (IDENTIFIER_TOKEN)
                        // an uninitialized identifier was passed
                        if(!optional)
                            Error("Cannot pass uninitialized identifier to non-optional array initializer.");
                    END_CASE

                    CASE (RIGHT_CURLY_TOKEN)
                        if (a->resizable)
                        {
                            finalParameter = true;
                            // We reserved one element too many.
                            a->Shrink();
                        }
                        else
                        {
                            if (!(finalParameter && properlyDelimited))
                                // the parameter list was closed prematurely
                                Error("Expected %d initializers but only %d found.",a->Sizes[Sub],i);
                            // the parameter was left empty
                            if(!optional)
                                Error("Cannot omit elements of non-optional array initializer.");
                        }
                        UNGET
                    END_CASE

                    CASE (COMMA_TOKEN)
                        // the parameter was left empty
                        if(!optional)
                            Error("Cannot omit elements of non-optional array initializer.");
                        UNGET
                    END_CASE

                    OTHERWISE
                        Expectation_Error("array initializer element");
                    END_CASE
                END_EXPECT
            }
            properlyDelimited = Parse_Comma();
        }
    }
    Parse_End();
    Parse_Comma();
};

void Parser::Parse_Fopen(void)
{
    shared_ptr<IStream> rfile;
    OStream *wfile = nullptr;
    DATA_FILE *New;
    char *asciiFileName;
    UCS2String fileName;
    UCS2String ign;
    SYM_ENTRY *Entry;

    New = new DATA_FILE;

    // Safeguard against accidental nesting of other file access directives inside the `#fopen`
    // directive (or the user forgetting portions of the directive).
    New->busyParsing = true;

    GET(IDENTIFIER_TOKEN)
    Entry = Add_Symbol (SYM_TABLE_GLOBAL, CurrentTokenText(), FILE_ID_TOKEN);
    Entry->Data=reinterpret_cast<void *>(New);

    asciiFileName = Parse_C_String(true);
    fileName = ASCIItoUCS2String(asciiFileName);
    POV_FREE(asciiFileName);

    EXPECT_ONE
        CASE(READ_TOKEN)
            New->inTokenizer = std::make_shared<RawTokenizer>();
            rfile = Locate_File(fileName.c_str(), POV_File_Text_User, ign, true);
            if (rfile != nullptr)
                New->inTokenizer->SetInputStream(rfile);
            else
                Error ("Cannot open user file %s (read).", UCS2toASCIIString(fileName).c_str());
        END_CASE

        CASE(WRITE_TOKEN)
            wfile = CreateFile(fileName.c_str(), POV_File_Text_User, false);
            if (wfile != nullptr)
                New->Out_File = std::make_shared<OTextStream>(fileName.c_str(), wfile);
            else
                New->Out_File = nullptr;

            if (New->Out_File == nullptr)
                Error ("Cannot open user file %s (write).", UCS2toASCIIString(fileName).c_str());
        END_CASE

        CASE(APPEND_TOKEN)
            wfile = CreateFile(fileName.c_str(), POV_File_Text_User, true);
            if (wfile != nullptr)
                New->Out_File = std::make_shared<OTextStream>(fileName.c_str(), wfile);
            else
                New->Out_File = nullptr;

            if (New->Out_File == nullptr)
                Error ("Cannot open user file %s (append).", UCS2toASCIIString(fileName).c_str());
        END_CASE

        OTHERWISE
            Expectation_Error("read or write");
        END_CASE
    END_EXPECT

    New->busyParsing = false;
}

void Parser::Parse_Fclose(void)
{
    DATA_FILE *Data;

    EXPECT_ONE
        CASE(FILE_ID_TOKEN)
            Data = CurrentTokenDataPtr<DATA_FILE*>();
            if (Data->busyParsing)
                Error ("Can't nest directives accessing the same file.");
            // NB no need to set Data->busyParsing, as we're not reading any tokens where the
            // tokenizer might stumble upon nested file access directives
            Got_EOF=false;
            Data->inTokenizer = nullptr;
            Data->Out_File = nullptr;
            Remove_Symbol(SYM_TABLE_GLOBAL, CurrentTokenText().c_str(), false, nullptr, 0);
        END_CASE

        OTHERWISE
            // To allow `#fclose` to be invoked on an already-closed file,
            // we need to accept IDENTIFIER_TOKEN, but also any *_ID_TOKEN
            // since it may be used at a less local level.
            // (Caveat: This allows to accidentally close a file at a less local level.)
        END_CASE
    END_EXPECT
}

void Parser::Parse_Read()
{
    DATA_FILE *User_File;
    SYM_ENTRY *Temp_Entry;
    int End_File=false;
    char *File_Id;

    Parse_Paren_Begin();

    GET(FILE_ID_TOKEN)
    User_File = CurrentTokenDataPtr<DATA_FILE*>();
    if (User_File->busyParsing)
        Error ("Can't nest directives accessing the same file.");
    File_Id = POV_STRDUP(CurrentTokenText().c_str());
    if (User_File->inTokenizer == nullptr)
        Error("Cannot read from file %s because the file is open for writing only.", UCS2toASCIIString(UCS2String(User_File->Out_File->name())).c_str());

    // Safeguard against accidental nesting of other file access directives inside the `#fopen`
    // directive (or the user forgetting portions of the directive).
    User_File->busyParsing = true;

    Parse_Comma(); /* Scene file comma between File_Id and 1st data ident */

    LValue_Ok = true;

    EXPECT
        CASE (IDENTIFIER_TOKEN)
            if (!End_File)
            {
                Temp_Entry = Add_Symbol (SYM_TABLE_GLOBAL, CurrentTokenText(), IDENTIFIER_TOKEN);
                End_File = Parse_Read_Value (User_File, CurrentTokenId(), &(Temp_Entry->Token_Number), &(Temp_Entry->Data));
                mToken.is_array_elem = false;
                mToken.is_mixed_array_elem = false;
                mToken.is_dictionary_elem = false;
                Parse_Comma(); /* Scene file comma between 2 idents */
            }
        END_CASE

        CASE (STRING_ID_TOKEN)
            if (!End_File)
            {
                End_File = Parse_Read_Value (User_File, CurrentTokenId(), mToken.NumberPtr, mToken.DataPtr);
                mToken.is_array_elem = false;
                mToken.is_mixed_array_elem = false;
                mToken.is_dictionary_elem = false;
                Parse_Comma(); /* Scene file comma between 2 idents */
            }
        END_CASE

        CASE2 (VECTOR_FUNCT_TOKEN,FLOAT_FUNCT_TOKEN)
            switch(CurrentTokenFunctionId())
            {
                case VECTOR_ID_TOKEN:
                case FLOAT_ID_TOKEN:
                    if (!End_File)
                    {
                        End_File = Parse_Read_Value (User_File, CurrentTokenFunctionId(), mToken.NumberPtr, mToken.DataPtr);
                        Parse_Comma(); /* Scene file comma between 2 idents */
                    }
                    break;

                default:
                    Parse_Error(IDENTIFIER_TOKEN);
                    break;
            }
        END_CASE

        CASE(COMMA_TOKEN)
            if (!End_File)
            {
                Parse_Error(IDENTIFIER_TOKEN);
            }
        END_CASE

        CASE(RIGHT_PAREN_TOKEN)
            UNGET
            EXIT
        END_CASE

        OTHERWISE
            Parse_Error(IDENTIFIER_TOKEN);
        END_CASE
    END_EXPECT

    Parse_Paren_End();

    User_File->busyParsing = false;
    LValue_Ok = false;

    if (End_File)
    {
        Got_EOF=false;
        User_File->inTokenizer = nullptr;
        Remove_Symbol(SYM_TABLE_GLOBAL, File_Id, false, nullptr, 0);
    }
    POV_FREE(File_Id);
}

int Parser::Parse_Read_Value(DATA_FILE *User_File, TokenId Previous, TokenId *NumberPtr, void **DataPtr)
{
    int End_File=false;
    int numComponents;
    EXPRESS Express;
    DBL sign = 1.0;
    DBL val = 0.0;
    bool ungetToken = false;

    if (User_File->inTokenizer == nullptr)
        Error("Cannot read from file '%s' because the file is open for writing only.", UCS2toASCIIString(UCS2String(User_File->Out_File->name())).c_str());

    if (User_File->ReadNextToken())
    {
        switch (User_File->inToken.GetTokenId())
        {
            case DASH_TOKEN:
            case PLUS_TOKEN:
            case FLOAT_TOKEN:
                User_File->UnReadToken();
                if (!Parse_Read_Float_Value(val, User_File))
                    POV_PARSER_PANIC();
                *NumberPtr = FLOAT_ID_TOKEN;
                Test_Redefine(Previous,NumberPtr,*DataPtr);
                *DataPtr   = reinterpret_cast<void *>(Create_Float());
                *(reinterpret_cast<DBL *>(*DataPtr)) = val;
                break;

            case LEFT_ANGLE_TOKEN:
                numComponents = 0;
                while (Parse_Read_Float_Value(val, User_File))
                {
                    if (numComponents == 5)
                        Error(SourceInfo(User_File->inTokenizer->GetSourceName(), User_File->inToken.lexeme.position), "Too many components in vector.");
                    Express[numComponents++] = val;
                    if (!User_File->ReadNextToken())
                        Error(SourceInfo(User_File->inTokenizer->GetSourceName(), User_File->inToken.lexeme.position), "Incomplete vector.");
                    if (User_File->inToken.GetTokenId() != COMMA_TOKEN)
                        User_File->UnReadToken();
                }
                if (!User_File->ReadNextToken() || (User_File->inToken.GetTokenId() != RIGHT_ANGLE_TOKEN))
                    Error(SourceInfo(User_File->inTokenizer->GetSourceName(), User_File->inToken.lexeme.position), "Expected vector component or '>'.");
                if (numComponents < 2)
                    Error(SourceInfo(User_File->inTokenizer->GetSourceName(), User_File->inToken.lexeme.position), "Not enough components in vector.");

                switch (numComponents)
                {
                    case 2:
                        *NumberPtr = UV_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr);
                        *DataPtr   = reinterpret_cast<void *>(new Vector2d(Express));
                        break;

                    case 3:
                        *NumberPtr = VECTOR_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr);
                        *DataPtr   = reinterpret_cast<void *>(new Vector3d(Express));
                        break;

                    case 4:
                        *NumberPtr = VECTOR_4D_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr);
                        *DataPtr   = reinterpret_cast<void *>(Create_Vector_4D());
                        Assign_Vector_4D(reinterpret_cast<DBL *>(*DataPtr), Express);
                        break;

                    case 5:
                        *NumberPtr = COLOUR_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr);
                        *DataPtr   = reinterpret_cast<void *>(Create_Colour());
                        (*reinterpret_cast<RGBFTColour *>(*DataPtr)).Set(Express, 5); /* NK fix assign_colour bug */
                        break;

                    default:
                        POV_PARSER_PANIC();
                        break;
                }
                break;

            case STRING_LITERAL_TOKEN:
                *NumberPtr = STRING_ID_TOKEN;
                Test_Redefine(Previous,NumberPtr,*DataPtr);
                POV_PARSER_ASSERT(dynamic_pointer_cast<const StringValue>(User_File->inToken.value) != nullptr);
                *DataPtr   = UCS2_strdup(dynamic_pointer_cast<const StringValue>(User_File->inToken.value)->GetData().c_str());
                break;

            default:
                Error(SourceInfo(User_File->inTokenizer->GetSourceName(), User_File->inToken.lexeme.position), "Expected float, vector, or string literal");
                break;
        }

        if (User_File->ReadNextToken() && (User_File->inToken.GetTokenId() != COMMA_TOKEN))
            User_File->UnReadToken();
    }

    /// @todo Returning `true` in case of end-of-file is counter-intuitive.
    return (User_File->inToken.id == END_OF_FILE_TOKEN);
}

bool Parser::Parse_Read_Float_Value(DBL& val, DATA_FILE* User_File)
{
    DBL sign = 1.0;

    if (!User_File->ReadNextToken())
        return false;

    switch (User_File->inToken.GetTokenId())
    {
        case DASH_TOKEN:
            sign = -1.0;
            // FALLTHROUGH
        case PLUS_TOKEN:
            if (!User_File->ReadNextToken() || (User_File->inToken.GetTokenId() != FLOAT_TOKEN))
                Error(SourceInfo(User_File->inTokenizer->GetSourceName(), User_File->inToken.lexeme.position), "Expected float literal");
            // FALLTHROUGH
        case FLOAT_TOKEN:
            val = sign * User_File->inToken.floatValue;
            return true;

        default:
            User_File->UnReadToken();
            return false;
    }
}

void Parser::Parse_Write(void)
{
    char *temp;
    DATA_FILE *User_File;
    EXPRESS Express;
    int Terms;

    Parse_Paren_Begin();

    GET(FILE_ID_TOKEN)

    User_File = CurrentTokenDataPtr<DATA_FILE*>();
    if (User_File->busyParsing)
        Error ("Can't nest directives accessing the same file.");
    if (User_File->Out_File == nullptr)
        Error("Cannot write to file %s because the file is open for reading only.", UCS2toASCIIString(User_File->inTokenizer->GetSourceName()).c_str());

    // Safeguard against accidental nesting of other file access directives inside the `#fopen`
    // directive (or the user forgetting portions of the directive).
    User_File->busyParsing = true;

    Parse_Comma();

    EXPECT
        CASE5 (SINT8_TOKEN,SINT16BE_TOKEN,SINT16LE_TOKEN,SINT32BE_TOKEN,SINT32LE_TOKEN)
        CASE3 (UINT8_TOKEN,UINT16BE_TOKEN,UINT16LE_TOKEN)
            {
                POV_INT32 val_min;
                POV_INT32 val_max;
                int  num_bytes;
                bool big_endian = false;
                switch (CurrentTokenId())
                {
                    case SINT8_TOKEN:    val_min = SIGNED8_MIN;  val_max = SIGNED8_MAX;    num_bytes = 1; break;
                    case UINT8_TOKEN:    val_min = 0;            val_max = UNSIGNED8_MAX;  num_bytes = 1; break;
                    case SINT16BE_TOKEN: val_min = SIGNED16_MIN; val_max = SIGNED16_MAX;   num_bytes = 2; big_endian = true;  break;
                    case SINT16LE_TOKEN: val_min = SIGNED16_MIN; val_max = SIGNED16_MAX;   num_bytes = 2; big_endian = false; break;
                    case UINT16BE_TOKEN: val_min = 0;            val_max = UNSIGNED16_MAX; num_bytes = 2; big_endian = true;  break;
                    case UINT16LE_TOKEN: val_min = 0;            val_max = UNSIGNED16_MAX; num_bytes = 2; big_endian = false; break;
                    case SINT32BE_TOKEN: val_min = SIGNED32_MIN; val_max = SIGNED32_MAX;   num_bytes = 4; big_endian = true;  break;
                    case SINT32LE_TOKEN: val_min = SIGNED32_MIN; val_max = SIGNED32_MAX;   num_bytes = 4; big_endian = false; break;
                }
                EXPECT
                    CASE_VECTOR_UNGET
                        Terms = Parse_Unknown_Vector (Express);
                        if ((Terms >= 1) && (Terms <= 5))
                        {
                            for (int i = 0; i < Terms; i ++)
                            {
                                signed long val;
                                if (Express[i] <= val_min)
                                    val = val_min; // TODO - maybe we should warn the user
                                else if (Express[i] >= val_max)
                                    val = val_max; // TODO - maybe we should warn the user
                                else
                                    val = (signed long)(floor(Express[i]+0.5));
                                for (int j = 0; j < num_bytes; j ++)
                                {
                                    int bitShift = (big_endian? (num_bytes-1)-j : j) * 8;
                                    User_File->Out_File->putraw((val >> bitShift) & 0xFF);
                                }
                            }
                        }
                        else
                        {
                            Expectation_Error("expression");
                        }
                    END_CASE
                    CASE (RIGHT_PAREN_TOKEN)
                        UNGET
                        EXIT
                    END_CASE
                    CASE (COMMA_TOKEN)
                        UNGET
                        EXIT
                    END_CASE
                    OTHERWISE
                        Expectation_Error("expression");
                    END_CASE
                END_EXPECT
            }
        END_CASE

        CASE5 (STRING_LITERAL_TOKEN,CHR_TOKEN,SUBSTR_TOKEN,STR_TOKEN,VSTR_TOKEN)
        CASE5 (CONCAT_TOKEN,STRUPR_TOKEN,STRLWR_TOKEN,DATETIME_TOKEN,STRING_ID_TOKEN)
            UNGET
            temp=Parse_C_String();
            if(strlen(temp) > 512)
            {
                for(char *ptr = temp; *ptr != 0; ptr++)
                    User_File->Out_File->printf("%c", *ptr);
            }
            else
                User_File->Out_File->printf("%s", temp);
            POV_FREE(temp);
        END_CASE

        CASE_VECTOR_UNGET
            Terms = Parse_Unknown_Vector (Express);
            switch (Terms)
            {
                case 1:
                    User_File->Out_File->printf("%g",Express[X]);
                    break;

                case 2:
                    User_File->Out_File->printf("<%g,%g> ",Express[U],Express[V]);
                    break;

                case 3:
                    User_File->Out_File->printf("<%g,%g,%g> ",Express[X],Express[Y],Express[Z]);
                    break;

                case 4:
                    User_File->Out_File->printf("<%g,%g,%g,%g> ",Express[X],Express[Y],Express[Z],Express[T]);
                    break;

                case 5:
                    User_File->Out_File->printf("<%g,%g,%g,%g,%g> ",Express[X],Express[Y],Express[Z],Express[3],Express[4]);
                    break;

                default:
                    Expectation_Error("expression");
            }
        END_CASE

        CASE (RIGHT_PAREN_TOKEN)
            UNGET
            EXIT
        END_CASE

        CASE (COMMA_TOKEN)
        END_CASE

        OTHERWISE
            Expectation_Error("string");
        END_CASE
    END_EXPECT

    Parse_Paren_End();

    User_File->busyParsing = false;
}

DBL Parser::Parse_Cond_Param(void)
{
    bool oldOkToDeclare = IsOkToDeclare();
    bool Old_Sk = Skipping;
    DBL Val;

    SetOkToDeclare(false);
    Skipping      = false;

    Val=Parse_Float_Param();

    SetOkToDeclare(oldOkToDeclare);
    Skipping      = Old_Sk;

    return(Val);
}

void Parser::Parse_Cond_Param2(DBL *V1,DBL *V2)
{
    bool oldOkToDeclare = IsOkToDeclare();
    bool Old_Sk = Skipping;

    SetOkToDeclare(false);
    Skipping      = false;

    Parse_Float_Param2(V1,V2);

    SetOkToDeclare(oldOkToDeclare);
    Skipping      = Old_Sk;
}

void Parser::Inc_CS_Index()
{
    Cond_Stack.emplace_back();
}

bool Parser::Parse_Ifdef_Param ()
{
    bool retval = false;

    Parse_Paren_Begin();

    Inside_Ifdef=true;
    Get_Token();
    Inside_Ifdef=false;

    if (mToken.is_array_elem)
        retval = (*mToken.DataPtr != nullptr);
    else
        retval = (CurrentTokenId() != IDENTIFIER_TOKEN);

    Parse_Paren_End();

    return retval;
}

int Parser::Parse_For_Param (UTF8String& identifierName, DBL* EndPtr, DBL* StepPtr)
{
    TokenId Previous = NOT_A_TOKEN;
    SYM_ENTRY *Temp_Entry = nullptr;

    Parse_Paren_Begin();

    LValue_Ok = true;

    EXPECT_ONE
        CASE (IDENTIFIER_TOKEN)
            POV_PARSER_ASSERT(!mToken.is_array_elem); // Array elements should be tagged as the respective array type
            if (CurrentTokenIsContainerElement())
                Error("#for loop variable must not be an array or dictionary element");
            Temp_Entry = Add_Symbol (Table_Index, CurrentTokenText(), IDENTIFIER_TOKEN);
            mToken.NumberPtr = &(Temp_Entry->Token_Number);
            mToken.DataPtr = &(Temp_Entry->Data);
            Previous = CurrentTokenId();
        END_CASE

        CASE3 (FILE_ID_TOKEN, MACRO_ID_TOKEN, PARAMETER_ID_TOKEN)
            // TODO - We should allow assignment if the identifier is non-local.
            if (CurrentTokenIsContainerElement())
                Error("#for loop variable must not be an array or dictionary element");
            Parse_Error(IDENTIFIER_TOKEN);
        END_CASE

        CASE2 (FUNCT_ID_TOKEN, VECTFUNCT_ID_TOKEN)
            if (CurrentTokenIsContainerElement())
                Error("#for loop variable must not be an array or dictionary element");
            Error("Redeclaring functions is not allowed - #undef the function first!");
        END_CASE

        // These have to match Parse_Declare in parse.cpp!
        CASE4 (NORMAL_ID_TOKEN, FINISH_ID_TOKEN, TEXTURE_ID_TOKEN, OBJECT_ID_TOKEN)
        CASE4 (COLOUR_MAP_ID_TOKEN, TRANSFORM_ID_TOKEN, CAMERA_ID_TOKEN, PIGMENT_ID_TOKEN)
        CASE4 (SLOPE_MAP_ID_TOKEN, NORMAL_MAP_ID_TOKEN, TEXTURE_MAP_ID_TOKEN, COLOUR_ID_TOKEN)
        CASE4 (PIGMENT_MAP_ID_TOKEN, MEDIA_ID_TOKEN, STRING_ID_TOKEN, INTERIOR_ID_TOKEN)
        CASE4 (DENSITY_ID_TOKEN, ARRAY_ID_TOKEN, DENSITY_MAP_ID_TOKEN, UV_ID_TOKEN)
        CASE4 (VECTOR_4D_ID_TOKEN, RAINBOW_ID_TOKEN, FOG_ID_TOKEN, SKYSPHERE_ID_TOKEN)
        CASE3 (MATERIAL_ID_TOKEN, SPLINE_ID_TOKEN, DICTIONARY_ID_TOKEN)
            if (CurrentTokenIsContainerElement())
                Error("#for loop variable must not be an array or dictionary element");
            if (mToken.context != Table_Index)
            {
                Temp_Entry = Add_Symbol (Table_Index, CurrentTokenText(), IDENTIFIER_TOKEN);
                mToken.NumberPtr = &(Temp_Entry->Token_Number);
                mToken.DataPtr   = &(Temp_Entry->Data);
                Previous        = IDENTIFIER_TOKEN;
            }
            else
            {
                Previous        = CurrentTokenId();
            }
        END_CASE

        CASE (EMPTY_ARRAY_TOKEN)
            POV_PARSER_ASSERT (CurrentTokenIsHomogenousArrayElement());
            Error("#for loop variable must not be an array element");
            Previous = CurrentTokenId();
        END_CASE

        CASE2 (VECTOR_FUNCT_TOKEN, FLOAT_FUNCT_TOKEN)
            if (CurrentTokenIsContainerElement())
                Error("#for loop variable must not be an array or dictionary element");
            switch(CurrentTokenFunctionId())
            {
                case VECTOR_ID_TOKEN:
                case FLOAT_ID_TOKEN:
                    if (mToken.context != Table_Index)
                    {
                        Temp_Entry = Add_Symbol (Table_Index, CurrentTokenText(), IDENTIFIER_TOKEN);
                        mToken.NumberPtr = &(Temp_Entry->Token_Number);
                        mToken.DataPtr   = &(Temp_Entry->Data);
                        Previous        = IDENTIFIER_TOKEN;
                    }
                    else
                    {
                        Previous        = CurrentTokenFunctionId();
                    }
                    break;

                default:
                    Parse_Error(IDENTIFIER_TOKEN);
                    break;
            }
        END_CASE

        OTHERWISE
            if (CurrentTokenIsContainerElement())
                Error("#for loop variable must not be an array or dictionary element");
            Parse_Error(IDENTIFIER_TOKEN);
        END_CASE
    END_EXPECT

    LValue_Ok = false;

    *mToken.NumberPtr = FLOAT_ID_TOKEN;
    Test_Redefine(Previous,mToken.NumberPtr,*mToken.DataPtr, true);
    *mToken.DataPtr   = reinterpret_cast<void *>(Create_Float());
    DBL* CurrentPtr = (reinterpret_cast<DBL *>(*mToken.DataPtr));

    identifierName = CurrentTokenText();

    Parse_Comma();
    *CurrentPtr = Parse_Float();
    Parse_Comma();
    *EndPtr = Parse_Float();
    Parse_Comma();
    *StepPtr = Allow_Float(1.0);

    if (fabs(*StepPtr) < EPSILON)
        Error ("#for loop increment must be non-zero.");

    Parse_Paren_End();

    return ((*StepPtr > 0) && (*CurrentPtr < *EndPtr + EPSILON)) ||
           ((*StepPtr < 0) && (*CurrentPtr > *EndPtr - EPSILON));
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::IncludeHeader(const UCS2String& formalFileName)
{
    UCS2String actualFileName;

    if (formalFileName.empty())
        return;

    maIncludeStack.push_back(mTokenizer.GetHotBookmark());

    shared_ptr<IStream> is = Locate_File (formalFileName.c_str(),POV_File_Text_INC,actualFileName,true);
    if (is == nullptr)
        Error ("Cannot open include file %s.", UCS2toASCIIString(formalFileName).c_str());

    SetInputStream(is);

    Add_Sym_Table();

    InvalidateCurrentToken();

    CheckFileSignature();
}

}
