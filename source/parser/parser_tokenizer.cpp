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

#include "base/stringutilities.h"
#include "base/version_info.h"

#include "core/material/noise.h"
#include "core/scene/scenedata.h"

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
    IStream *rfile = nullptr;
    UCS2String actualFileName;
    int c;

    pre_init_tokenizer ();

    rfile = Locate_File(sceneData->inputFile.c_str(),POV_File_Text_POV,actualFileName,true);
    if (rfile != nullptr)
    {
        Input_File->In_File = new IBufferedTextStream(sceneData->inputFile.c_str(), rfile);
        sceneData->inputFile = actualFileName;
    }

    if (Input_File->In_File == nullptr)
    {
        Error ("Cannot open input file.");
    }

    Input_File->R_Flag = false;

    Got_EOF  = false;

    /* Init conditional stack. */

    Cond_Stack = reinterpret_cast<CS_ENTRY*>(POV_MALLOC(sizeof(CS_ENTRY) * COND_STACK_SIZE, "conditional stack"));

    Cond_Stack[0].Cond_Type    = ROOT_COND;
    Cond_Stack[0].Switch_Value = 0.0;

    init_sym_tables();
    Max_Trace_Level = MAX_TRACE_LEVEL_DEFAULT;
    Had_Max_Trace_Level = false;

    /* ignore any leading characters if they have character codes above 127, this
       takes care of UTF-8 files with encoding info at the beginning of the file */
    for(c = Echo_getc(); c > 127; c = Echo_getc())
        sceneData->stringEncoding = kStringEncoding_UTF8; // switch to UTF-8 automatically [trf]
    Echo_ungetc(c);

#if POV_DEBUG
    gBreakpointCounter = 0;
#endif
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

    Token.Token_File_Pos.lineno = 0;
    Token.Token_File_Pos.offset = 0;
    Token.Token_Col_No = 0;
    Token.Token_String  = nullptr;
    Token.freeString    = false;
    Token.Unget_Token   = false;
    Token.End_Of_File   = false;
    Token.Data = nullptr;
    Token.FileHandle = nullptr;

    line_count = 10;
    token_count = 0;
    Current_Token_Count = 0;
    Include_File_Index = 0;
    Echo_Indx=0;

    // make sure these are `nullptr` otherwise cleanup() will crash if we terminate early
    Default_Texture = nullptr;

    CS_Index            = 0;
    Skipping            = false;
    Inside_Ifdef        = false;
    Inside_MacroDef     = false;
    Parsing_Directive   = false;
    parseRawIdentifiers = false;
    parseOptionalRValue = false;
    Cond_Stack          = nullptr;
    Table_Index         = -1;

    Input_File = &Include_Files[0];
    Include_Files[0].In_File = nullptr;

    // TODO - on modern machines it may be faster to do the comparisons for each token
    //        than to access the conversion table.
    for(i = 0; i < TOKEN_COUNT; i++)
    {
        Conversion_Util_Table[i] = i;
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
    Token.FileHandle = nullptr;

    while(Table_Index >= 0)
    {
        Destroy_Table(Table_Index--);
    }

    if (Input_File->In_File != nullptr)
    {
        delete Input_File->In_File;
        Input_File->In_File = nullptr;
        Got_EOF = false;
    }

    while(Include_File_Index >= 0)
    {
        Input_File = &Include_Files[Include_File_Index--];

        if (Input_File->In_File != nullptr)
        {
            delete Input_File->In_File;
            Input_File->In_File = nullptr;
            Got_EOF = false;
        }
    }

    if (Cond_Stack != nullptr)
    {
        for(int i = 0; i <= CS_Index; i++)
        {
            if((Cond_Stack[i].Cond_Type == INVOKING_MACRO_COND) && (Cond_Stack[i].Macro_Same_Flag == false))
                delete Cond_Stack[i].Macro_File;
        }
        POV_FREE(Cond_Stack);

        Cond_Stack = nullptr;
    }

    if ((String != nullptr) && (String != String_Fast_Buffer))
        POV_FREE(String);
    String = nullptr;

    if ((String2 != nullptr) && (String2 != String_Fast_Buffer))
        POV_FREE(String2);
    String2 = nullptr;
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
*   Read a token from the input file and store it in the Token variable.
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
    int c,c2;
    int col;

    if (Token.Unget_Token)
    {
        Token.Unget_Token = false;

        return;
    }

    if (Token.End_Of_File)
    {
        return;
    }

    Token.Token_Id = END_OF_FILE_TOKEN;
    Token.is_array_elem = false;
    Token.is_mixed_array_elem = false;
    Token.is_dictionary_elem = false;

    while (Token.Token_Id == END_OF_FILE_TOKEN)
    {
        if (Skipping && !Parsing_Directive)
            // If we're skipping, we're only interested in directives, such as `#else` or `#end`,
            // so we just pretend any other characters in between are blanks.
            SkipToDirective();
        else
            Skip_Spaces();

        Token.Token_Col_No = col = Echo_Indx;
        c = Echo_getc();

        if (c == EOF)
        {
            if (Input_File->R_Flag)
            {
                Token.Token_Id = END_OF_FILE_TOKEN;
                Token.is_array_elem = false;
                Token.is_mixed_array_elem = false;
                Token.is_dictionary_elem = false;
                Token.End_Of_File = true;
                return;
            }

            if (Include_File_Index == 0)
            {
                if (CS_Index !=0)
                    Error("End of file reached but #end expected.");

                Token.Token_Id = END_OF_FILE_TOKEN;
                Token.is_array_elem = false;
                Token.is_mixed_array_elem = false;
                Token.is_dictionary_elem = false;
                Token.End_Of_File = true;
                return;
            }

            if (Input_File->In_File == Token.FileHandle)
                Token.FileHandle = nullptr;

            delete Input_File->In_File; /* added to fix open file buildup JLN 12/91 */
            Input_File->In_File = nullptr;
            Got_EOF=false;

            Destroy_Table(Table_Index--);

            Input_File = &Include_Files[--Include_File_Index];
            if (Token.FileHandle == nullptr)
                Token.FileHandle = Input_File->In_File;

            continue;
        }

        Begin_String_Fast();

        String[0] = c; /* This isn't necessary but helps debugging */

        String[1] = '\0';

        /*String_Index = 0;*/

        switch (c)
        {
            case '\n':
                break;

            case '{' :
                Write_Token (LEFT_CURLY_TOKEN, col);
                break;

            case '}' :
                Write_Token (RIGHT_CURLY_TOKEN, col);
                break;

            case '@' :
                Write_Token (AT_TOKEN, col);
                break;

            case '&' :
                Write_Token (AMPERSAND_TOKEN, col);
                break;

            case '`' :
                Write_Token (BACK_QUOTE_TOKEN, col);
                break;

            case '\\':
                Write_Token (BACK_SLASH_TOKEN, col);
                break;

            case '|' :
                Write_Token (BAR_TOKEN, col);
                break;

            case ':' :
                Write_Token (COLON_TOKEN, col);
                break;

            case ',' :
                Write_Token (COMMA_TOKEN, col);
                break;

            case '-' :
                Write_Token (DASH_TOKEN, col);
                break;

            case '$' :
                Write_Token (DOLLAR_TOKEN, col);
                break;

            case '=' :
                Write_Token (EQUALS_TOKEN, col);
                break;

            case '!' :
                c2 = Echo_getc();
                if (c2 == (int)'=')
                {
                    Write_Token (REL_NE_TOKEN, col);
                }
                else
                {
                    Echo_ungetc(c2);
                    Write_Token (EXCLAMATION_TOKEN, col);
                }
                break;

            case '#' :
                Parse_Directive(true);
                /* Write_Token (HASH_TOKEN, col);*/
                break;

            case '^' :
                Write_Token (HAT_TOKEN, col);
                break;

            case '<' :
                c2 = Echo_getc();
                if (c2 == (int)'=')
                {
                    Write_Token (REL_LE_TOKEN, col);
                }
                else
                {
                    Echo_ungetc(c2);
                    Write_Token (LEFT_ANGLE_TOKEN, col);
                }
                break;

            case '(' :
                Write_Token (LEFT_PAREN_TOKEN, col);
                break;

            case '[' :
                Write_Token (LEFT_SQUARE_TOKEN, col);
                break;

            case '%' :
                Write_Token (PERCENT_TOKEN, col);
                break;

            case '+' :
                Write_Token (PLUS_TOKEN, col);
                break;

            case '?' :
                Write_Token (QUESTION_TOKEN, col);
                break;

            case '>' :
                c2 = Echo_getc();
                if (c2 == (int)'=')
                {
                    Write_Token (REL_GE_TOKEN, col);
                }
                else
                {
                    Echo_ungetc(c2);
                    Write_Token (RIGHT_ANGLE_TOKEN, col);
                }
                break;

            case ')' :
                Write_Token (RIGHT_PAREN_TOKEN, col);
                break;

            case ']' :
                Write_Token (RIGHT_SQUARE_TOKEN, col);
                break;

            case ';' : /* Parser doesn't use it, so let's ignore it */
                Write_Token (SEMI_COLON_TOKEN, col);
                break;

            case '\'':
                Write_Token (SINGLE_QUOTE_TOKEN, col);
                break;

                /* enable C++ style commenting */
            case '/' :
                c2 = Echo_getc();
                if(c2 != (int) '/' && c2 != (int) '*')
                {
                    Echo_ungetc(c2);
                    Write_Token (SLASH_TOKEN, col);
                    break;
                }
                if(c2 == (int)'*')
                {
                    Parse_C_Comments();
                    break;
                }
                while(c2 != (int)'\n')
                {
                    c2=Echo_getc();
                    if(c2==EOF)
                    {
                        Echo_ungetc(c2);
                        break;
                    }
                }
                break;

            case '*' :
                Write_Token (STAR_TOKEN, col);
                break;

            case '~' :
                Write_Token (TILDE_TOKEN, col);
                break;

            case '"' :
                Read_String_Literal ();
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
                Echo_ungetc(c);
                if (Read_Float () != true)
                    return;
                break;

            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':

            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case '_':
                Echo_ungetc(c);
                Read_Symbol ();
                break;
            case '\t':
            case '\r':
            case '\032':   /* Control Z - EOF on many systems */
            case '\0':
                break;

            default:
                Error("Illegal character in input file, value is %02x.", c);
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
    Token.Unget_Token = true;
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
*   Skip over spaces in the input file.
*
* CHANGES
*
******************************************************************************/

bool Parser::Skip_Spaces()
{
    int c;

    while(true)
    {
        c = Echo_getc();

        if (c == EOF)
            return false;

        if(!(isspace(c)))
            break;
    }

    Echo_ungetc(c);

    return true;
}



//*****************************************************************************

bool Parser::SkipToDirective()
{
    int c;

    while(true)
    {
        c = Echo_getc();

        switch (c)
        {
            case EOF:
                return false;

            case '#' :
                // Genuine Directive.
                // We need to actually parse this.
                Echo_ungetc(c);
                return true;

            case '/' :
                // Possibly a comment.
                // We need to examine the next character.
                c = Echo_getc();
                switch (c)
                {
                    case '*':
                        // C style comment.
                        // Ignore all characters until the terminating `*/`.
                        Parse_C_Comments();
                        break;

                    case '/':
                        // C++ style comment.
                        // Ignore all characters until the end of line.
                        do
                        {
                            c = Echo_getc();
                            if (c == EOF)
                                return false;
                        }
                        while (c != '\n');
                        break;

                    default:
                        // The first slash was just an ordinary slash.
                        // Look at the second character in more detail.
                        Echo_ungetc(c);
                        break;
                }
                break;

            case '"' :
                // String literal.
                // Ignore all characters until the end of the string.
                do
                {
                    c = Echo_getc();
                    if (c == EOF)
                        Error("No end quote for string.");
                    if (c == '\\')
                        // Escaped character.
                        // Completely ignore the next character.
                        (void)Echo_getc();
                }
                while (c != '"');
                break;

            default:
                // Just any odd character.
                // Ignore it.
                break;
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
*   C style comments with asterik and slash - CdW 8/91.
*
* CHANGES
*
******************************************************************************/

int Parser::Parse_C_Comments()
{
    int c, c2;
    bool End_Of_Comment = false;

    while(!End_Of_Comment)
    {
        c = Echo_getc();

        if(c == EOF)
            Error("No */ closing comment found.");

        if(c == (int) '*')
        {
            c2 = Echo_getc();

            if(c2 != (int) '/')
                Echo_ungetc(c2);
            else
                End_Of_Comment = true;
        }

        /* Check for and handle nested comments */

        if(c == (int) '/')
        {
            c2 = Echo_getc();

            if(c2 != (int) '*')
                Echo_ungetc(c2);
            else
                Parse_C_Comments();
        }
    }

    return true;
}



/* The following routines make it easier to handle strings.  They stuff
   characters into a string buffer one at a time making all the proper
   range checks.  Call Begin_String to start, Stuff_Character to put
   characters in, and End_String to finish.  The String variable contains
   the final string. */

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

inline void Parser::Begin_String()
{
    if ((String != nullptr) && (String != String_Fast_Buffer))
        POV_FREE(String);

    String = reinterpret_cast<char *>(POV_MALLOC(256, "C String"));
    String_Buffer_Free = 256;
    String_Index = 0;
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

inline void Parser::Stuff_Character(int chr)
{
    if(String_Buffer_Free <= 0)
    {
        Error("String too long.");
// This caused too many problems with buffer overflows [trf]
//      String = reinterpret_cast<char *>(POV_REALLOC(String, String_Index + 256, "String Literal Buffer"));
//      String_Buffer_Free += 256;
    }

    String[String_Index] = chr;
    String_Buffer_Free--;
    String_Index++;
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

inline void Parser::End_String()
{
    Stuff_Character(0);

    if(String_Buffer_Free > 0)
        String = reinterpret_cast<char *>(POV_REALLOC(String, String_Index, "String Literal Buffer"));

    String_Buffer_Free = 0;
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

inline void Parser::Begin_String_Fast()
{
    if ((String != nullptr) && (String != String_Fast_Buffer))
        POV_FREE(String);

    String = String_Fast_Buffer;
    String_Index = 0;
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

inline void Parser::Stuff_Character_Fast(int chr)
{
    String[String_Index & MAX_STRING_LEN_MASK] = chr;
    String_Index++;
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

inline void Parser::End_String_Fast()
{
    Stuff_Character_Fast(0);

    String_Index--; // Stuff_Character_Fast incremented this

    if(String_Index != (String_Index & MAX_STRING_LEN_MASK))
        Error("String too long.");
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
 *   Parse a string from the input file into a token.
 *
 *   Does NOT handle escape sequences EXCEPT that double quotes after an
 *   odd number of backslashes (`\"`, `\\\"`, `\\\\\"` etc.) are treated as
 *   regular characters rather than the end of the string.
 *
 * CHANGES
 *
******************************************************************************/

void Parser::Read_String_Literal()
{
    int c;
    int col = Echo_Indx;

    Begin_String();

    bool escaped = false;
    while(true)
    {
        c = Echo_getc();

        if(c == EOF)
            Error("No end quote for string.");

        if((c == '"') && !escaped)
            break;

        Stuff_Character(c);

        if((c == '\\') && !escaped)
            escaped = true;
        else
            escaped = false;
    }

    End_String();

    Write_Token(STRING_LITERAL_TOKEN, col);
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
*   Read a float from the input file and tokenize it as one token. The phase
*   variable is 0 for the first character, 1 for all subsequent characters
*   up to the decimal point, 2 for all characters after the decimal
*   point, 3 for the E+/- and 4 for the exponent.  This helps to insure
*   that the number is formatted properly. E format added 9/91 CEY
*
* CHANGES
*
******************************************************************************/

bool Parser::Read_Float()
{
    int c, Phase;
    bool Finished;
    int col = Echo_Indx;

    Finished = false;

    Phase = 0;

    Begin_String_Fast();

    while (!Finished)
    {
        c = Echo_getc();

        if (c == EOF)
        {
            Error ("Unexpected end of file.");
        }

        if (Phase > 1 && c == '.')
        {
            Error ("Unexpected additional '.' in floating-point number");
        }

        switch (Phase)
        {
            case 0:

                Phase = 1;

                if (isdigit(c))
                {
                    Stuff_Character_Fast(c);
                }
                else
                {
                    if (c == '.')
                    {
                        c = Echo_getc();

                        if (c == EOF)
                        {
                            Error ("Unexpected end of file");
                        }

                        if (isdigit(c))
                        {
                            Stuff_Character_Fast('0');
                            Stuff_Character_Fast('.');
                            Stuff_Character_Fast(c);

                            Phase = 2;
                        }
                        else
                        {
                            Echo_ungetc(c);

                            Write_Token (PERIOD_TOKEN, col);

                            return(true);
                        }
                    }
                    else
                    {
                        Error ("Invalid decimal number");
                    }
                }

                break;

            case 1:
                if (isdigit(c))
                {
                    Stuff_Character_Fast(c);
                }
                else
                {
                    if (c == (int) '.')
                    {
                        Stuff_Character_Fast(c); Phase = 2;
                    }
                    else
                    {
                        if ((c == 'e') || (c == 'E'))
                        {
                            Stuff_Character_Fast(c); Phase = 3;
                        }
                        else
                        {
                            Finished = true;
                        }
                    }
                }

                break;

            case 2:

                if (isdigit(c))
                {
                    Stuff_Character_Fast(c);
                }
                else
                {
                    if ((c == 'e') || (c == 'E'))
                    {
                        Stuff_Character_Fast(c); Phase = 3;
                    }
                    else
                    {
                        Finished = true;
                    }
                }

                break;

            case 3:

                if (isdigit(c) || (c == '+') || (c == '-'))
                {
                    Stuff_Character_Fast(c); Phase = 4;
                }
                else
                {
                    Finished = true;
                }

                break;

            case 4:

                if (isdigit(c))
                {
                    Stuff_Character_Fast(c);
                }
                else
                {
                    Finished = true;
                }

                break;
        }
    }

    Echo_ungetc(c);

    End_String_Fast();

    Write_Token (FLOAT_TOKEN, col);

    if (sscanf (String, POV_DBL_FORMAT_STRING, &Token.Token_Float) == 0)
    {
        return (false);
    }

    return (true);
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

void Parser::Read_Symbol()
{
    int c;
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

    Begin_String_Fast();

    while (true)
    {
        c = Echo_getc();

        if (c == EOF)
        {
            Error ("Unexpected end of file.");
        }

        if (isalpha(c) || isdigit(c) || c == (int) '_')
        {
            Stuff_Character_Fast(c);
        }
        else
        {
            Echo_ungetc(c);

            break;
        }
    }

    End_String_Fast();

    /* If its a reserved keyword, write it and return */
    Temp_Entry = Find_Symbol(SYM_TABLE_RESERVED, String);
    if (Temp_Entry != nullptr)
    {
        if (!Parsing_Directive && (Temp_Entry->Token_Number == LOCAL_TOKEN))
        {
            pseudoDictionary = Table_Index;
        }
        else if (!Parsing_Directive && (Temp_Entry->Token_Number == GLOBAL_TOKEN))
        {
            pseudoDictionary = SYM_TABLE_GLOBAL;
        }
        else if (Inside_Ifdef)
        {
            Warning("Tried to test whether a reserved keyword is defined. Test result may not be what you expect.");
        }
        else
        {
            Write_Token (Temp_Entry->Token_Number, Token.Token_Col_No);
            return;
        }
    }

    if (!Skipping && !parseRawIdentifiers)
    {
        if (pseudoDictionary >= 0)
        {
            Token.Token_Id = DICTIONARY_ID_TOKEN;
            Token.is_array_elem = false;
            Token.is_mixed_array_elem = false;
            Token.is_dictionary_elem = false;
            Token.NumberPtr = &(Temp_Entry->Token_Number);
            Token.DataPtr   = &(Temp_Entry->Data);

            table = nullptr;
        }
        else
        {
            /* Search tables from newest to oldest */
            int firstIndex = Table_Index;
            int lastIndex  = SYM_TABLE_RESERVED+1; // index SYM_TABLE_RESERVED is reserved for reserved words, not identifiers
            for (Local_Index = firstIndex; Local_Index >= lastIndex; Local_Index--)
            {
                /* See if it's a previously declared identifier. */
                Temp_Entry = Find_Symbol(Local_Index, String);
                if (Temp_Entry != nullptr)
                {
                    if (Temp_Entry->deprecated && !Temp_Entry->deprecatedShown)
                    {
                        Temp_Entry->deprecatedShown = Temp_Entry->deprecatedOnce;
                        Warning("%s", Temp_Entry->Deprecation_Message);
                    }

                    if ((Temp_Entry->Token_Number==MACRO_ID_TOKEN) && (!Inside_Ifdef))
                    {
                        Token.Data = Temp_Entry->Data;
                        if (Ok_To_Declare)
                        {
                            Invoke_Macro();
                        }
                        else
                        {
                            Token.Token_Id=MACRO_ID_TOKEN;
                            Token.is_array_elem = false;
                            Token.is_mixed_array_elem = false;
                            Token.is_dictionary_elem = false;
                            Token.NumberPtr = &(Temp_Entry->Token_Number);
                            Token.DataPtr   = &(Temp_Entry->Data);
                            Write_Token (Token.Token_Id, Token.Token_Col_No, Tables[Local_Index]);

                            Token.context = Local_Index;
                        }
                        return;
                    }

                    Token.Token_Id  =   Temp_Entry->Token_Number;
                    Token.is_array_elem = false;
                    Token.is_mixed_array_elem = false;
                    Token.is_dictionary_elem = false;
                    Token.NumberPtr = &(Temp_Entry->Token_Number);
                    Token.DataPtr   = &(Temp_Entry->Data);

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
                switch (Token.Token_Id)
                {
                    case ARRAY_ID_TOKEN:
                        {
                            if (dictIndex)
                                POV_FREE (dictIndex);

                            Skip_Spaces();
                            c = Echo_getc();
                            Echo_ungetc(c);

                            if (c!='[')
                            {
                                breakLoop = true;
                                break;
                            }

                            a = reinterpret_cast<POV_ARRAY *>(*(Token.DataPtr));
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

                            Token.DataPtr = &(a->DataPtrs[j]);
                            Token.is_mixed_array_elem = a->mixedType;
                            Token.NumberPtr = &(a->ElementType(j));
                            Token.Token_Id = *Token.NumberPtr;
                            Token.is_array_elem = true;
                            Token.is_dictionary_elem = false;
                        }
                        break;

                    case DICTIONARY_ID_TOKEN:
                        {
                            if (dictIndex)
                                POV_FREE (dictIndex);

                            Skip_Spaces();
                            c = Echo_getc();
                            Echo_ungetc(c);

                            SYM_TABLE* parentTable = table;
                            if (pseudoDictionary >= 0)
                            {
                                table = Tables [pseudoDictionary];
                                pseudoDictionary = -1;
                                if ((c != '[') && (c != '.'))
                                {
                                    Get_Token(); // ensures the error is reported at the right token
                                    Expectation_Error ("'[' or '.'");
                                }
                            }
                            else
                                table = reinterpret_cast<SYM_TABLE *>(*(Token.DataPtr));

                            if (c =='.')
                            {
                                if (table == nullptr)
                                {
                                    POV_PARSER_ASSERT (Token.is_array_elem);
                                    Error ("Attempt to access uninitialized array element.");
                                }

                                GET (PERIOD_TOKEN)
                                bool oldParseRawIdentifiers = parseRawIdentifiers;
                                parseRawIdentifiers = true;
                                Get_Token ();
                                parseRawIdentifiers = oldParseRawIdentifiers;

                                if (Token.Token_Id != IDENTIFIER_TOKEN)
                                    Expectation_Error ("dictionary element identifier");

                                Temp_Entry = Find_Symbol (table, Token.Token_String);
                            }
                            else if (c == '[')
                            {
                                if (table == nullptr)
                                {
                                    POV_PARSER_ASSERT (Token.is_array_elem);
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
                                Token.Token_Id      = Temp_Entry->Token_Number;
                                Token.NumberPtr     = &(Temp_Entry->Token_Number);
                                Token.DataPtr       = &(Temp_Entry->Data);
                            }
                            else
                            {
                                if (!LValue_Ok && !Inside_Ifdef && !parseOptionalRValue)
                                    Error ("Attempt to access uninitialized dictionary element.");
                                Token.Token_Id  = IDENTIFIER_TOKEN;
                                Token.DataPtr   = nullptr;
                                Token.NumberPtr = nullptr;
                            }
                            Token.is_array_elem = false;
                            Token.is_mixed_array_elem = false;
                            Token.is_dictionary_elem = true;
                        }
                        break;

                    case PARAMETER_ID_TOKEN:
                        {
                            if (dictIndex)
                                POV_FREE(dictIndex);

                            Par             = reinterpret_cast<POV_PARAM *>(Temp_Entry->Data);
                            Token.Token_Id  = *(Par->NumberPtr);
                            Token.is_array_elem = false;
                            Token.is_mixed_array_elem = false;
                            Token.is_dictionary_elem = false;
                            Token.NumberPtr = Par->NumberPtr;
                            Token.DataPtr   = Par->DataPtr;
                        }
                        break;

                    default:
                        breakLoop = true;
                        break;
                }
            }

            Write_Token (Token.Token_Id, Token.Token_Col_No, table);

            if (Token.DataPtr != nullptr)
                Token.Data = *(Token.DataPtr);
            Token.context = Local_Index;
            if (dictIndex != nullptr)
            {
                Token.Token_String = dictIndex;
                Token.freeString = true;
            }
            return;
        }
    }

    Write_Token (IDENTIFIER_TOKEN, Token.Token_Col_No);
}

inline void Parser::Write_Token (TOKEN Token_Id, int col, SYM_TABLE *table)
{
    if (Token.freeString)
    {
        POV_FREE (Token.Token_String);
        Token.freeString = false;
    }
    Token.Token_File_Pos = Input_File->In_File->tellg();
    Token.Token_Col_No   = col;
    Token.FileHandle     = Input_File->In_File;
    Token.Token_String   = String;
    Token.Data           = nullptr;
    Token.Token_Id       = Conversion_Util_Table[Token_Id];
    Token.Function_Id    = Token_Id;
    Token.table          = table;
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

const char *Parser::Get_Token_String (TOKEN Token_Id)
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

int Parser::Echo_getc()
{
    int c;

    if ((Input_File == nullptr) || (Input_File->In_File == nullptr) || (c = Input_File->In_File->getchar()) == EOF)
    {
        if (Got_EOF)
            return EOF;
        Got_EOF = true;
        Echo_Indx = 0;
        return ('\n');
    }

    Echo_Indx++;
    if(c == '\n')
        Echo_Indx = 0;

    return c;
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

void Parser::Echo_ungetc(int c)
{
    if(Echo_Indx > 0)
        Echo_Indx--;

    if (!Got_EOF)
        Input_File->In_File->ungetchar(c);
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

void Parser::Where_Error(POVMSObjectPtr msg)
{
    // return if no filename is specified
    if (Token.FileHandle == nullptr)
        return;

    (void)POVMSUtil_SetUCS2String(msg, kPOVAttrib_FileName, Token.FileHandle->name());
    (void)POVMSUtil_SetString(msg, kPOVAttrib_TokenName, Token.Token_String);
    (void)POVMSUtil_SetLong(msg, kPOVAttrib_Line, Token.Token_File_Pos.lineno);
    (void)POVMSUtil_SetInt(msg, kPOVAttrib_Column, Token.Token_Col_No);
    if (Token.FileHandle != nullptr)
        (void)POVMSUtil_SetLong(msg, kPOVAttrib_FilePosition, Token.FileHandle->tellg().offset);
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

void Parser::Where_Warning(POVMSObjectPtr msg)
{
    // return if no filename is specified
    if (Token.FileHandle == nullptr)
        return;

    (void)POVMSUtil_SetUCS2String(msg, kPOVAttrib_FileName, Token.FileHandle->name());
    (void)POVMSUtil_SetString(msg, kPOVAttrib_TokenName, Token.Token_String);
    (void)POVMSUtil_SetLong(msg, kPOVAttrib_Line, Token.Token_File_Pos.lineno);
    (void)POVMSUtil_SetInt(msg, kPOVAttrib_Column, Token.Token_Col_No);
    if (Token.FileHandle != nullptr)
        (void)POVMSUtil_SetLong(msg, kPOVAttrib_FilePosition, Token.FileHandle->tellg().offset);
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

void Parser::Parse_Directive(int After_Hash)
{
    DBL Value, Value2;
    int Flag;
    char *ts;
    Macro *PMac = nullptr;
    COND_TYPE Curr_Type = Cond_Stack[CS_Index].Cond_Type;
    POV_OFF_T Hash_Loc = Input_File->In_File->tellg().offset;

    if (Curr_Type == INVOKING_MACRO_COND)
    {
        POV_PARSER_ASSERT(Cond_Stack[CS_Index].PMac != nullptr);
        if ((Cond_Stack[CS_Index].PMac->Macro_End==Hash_Loc) &&
            (UCS2_strcmp(Cond_Stack[CS_Index].PMac->Macro_Filename, Input_File->In_File->name()) == 0))
        {
            Return_From_Macro();
            if (--CS_Index < 0)
            {
                Error("Mis-matched '#end'.");
            }
            Token.Token_Id = END_OF_FILE_TOKEN;
            Token.is_array_elem = false;
            Token.is_mixed_array_elem = false;
            Token.is_dictionary_elem = false;

            return;
        }
    }

    if (!Ok_To_Declare)
    {
        if (After_Hash)
        {
            Token.Token_Id=HASH_TOKEN;
            Token.is_array_elem = false;
            Token.is_mixed_array_elem = false;
            Token.is_dictionary_elem = false;
        }
        Token.Unget_Token = false;

        return;
    }

    Parsing_Directive = true;

    EXPECT  // we're normally running this loop only once, but a few directives cause it to be looped

        CASE(IFDEF_TOKEN)
            Parsing_Directive = false;
            Inc_CS_Index();

            if (Skipping)
            {
                Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                if (Parse_Ifdef_Param())
                {
                    Cond_Stack[CS_Index].Cond_Type=IF_TRUE_COND;
                }
                else
                {
                    Cond_Stack[CS_Index].Cond_Type=IF_FALSE_COND;
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
                Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                if (Parse_Ifdef_Param())
                {
                    Cond_Stack[CS_Index].Cond_Type=IF_FALSE_COND;
                    Skip_Tokens(IF_FALSE_COND);
                }
                else
                {
                    Cond_Stack[CS_Index].Cond_Type=IF_TRUE_COND;
                }
            }
            EXIT
        END_CASE

        CASE(IF_TOKEN)
            Parsing_Directive = false;
            Inc_CS_Index();

            if (Skipping)
            {
                Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                Value=Parse_Cond_Param();

                if (fabs(Value)>EPSILON)
                {
                    Cond_Stack[CS_Index].Cond_Type=IF_TRUE_COND;
                }
                else
                {
                    Cond_Stack[CS_Index].Cond_Type=IF_FALSE_COND;
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
                Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                Cond_Stack[CS_Index].Loop_File = Input_File->In_File;
                Cond_Stack[CS_Index].File_Pos  = Input_File->In_File->tellg();

                Value=Parse_Cond_Param();

                if (fabs(Value)>EPSILON)
                {
                    Cond_Stack[CS_Index].Cond_Type = WHILE_COND;
                }
                else
                {
                    Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
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
                Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                char* Identifier = nullptr;
                DBL End, Step;
                if (Parse_For_Param (&Identifier, &End, &Step))
                {
                    // execute loop
                    Cond_Stack[CS_Index].Cond_Type = FOR_COND;
                    Cond_Stack[CS_Index].Loop_File = Input_File->In_File;
                    Cond_Stack[CS_Index].File_Pos  = Input_File->In_File->tellg();
                    Cond_Stack[CS_Index].Loop_Identifier = Identifier;
                    Cond_Stack[CS_Index].For_Loop_End = End;
                    Cond_Stack[CS_Index].For_Loop_Step = Step;
                }
                else
                {
                    // terminate loop before it has even started
                    Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
                    Skip_Tokens(SKIP_TIL_END_COND);
                    // need to do some cleanup otherwise deferred via the Cond_Stack
                    POV_FREE(Identifier);
                }
            }
            EXIT
        END_CASE

        CASE(ELSE_TOKEN)
            Parsing_Directive = false;
            switch (Curr_Type)
            {
                case IF_TRUE_COND:
                    Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
                    Skip_Tokens(SKIP_TIL_END_COND);
                    break;

                case IF_FALSE_COND:
                    Cond_Stack[CS_Index].Cond_Type = ELSE_COND;
                    Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
                    Token.is_array_elem = false;
                    Token.is_mixed_array_elem = false;
                    Token.is_dictionary_elem = false;
                    UNGET
                    break;

                case CASE_TRUE_COND:
                case SKIP_TIL_END_COND:
                    break;

                case CASE_FALSE_COND:
                    Cond_Stack[CS_Index].Cond_Type = CASE_TRUE_COND;
                    if (Skipping)
                    {
                        Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
                        Token.is_array_elem = false;
                        Token.is_mixed_array_elem = false;
                        Token.is_dictionary_elem = false;
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
                    Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
                    Skip_Tokens(SKIP_TIL_END_COND);
                    break;

                case IF_FALSE_COND:
                    Value=Parse_Cond_Param();
                    if (fabs(Value)>EPSILON)
                    {
                        Cond_Stack[CS_Index].Cond_Type=IF_TRUE_COND;
                        Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
                        Token.is_array_elem = false;
                        Token.is_mixed_array_elem = false;
                        Token.is_dictionary_elem = false;
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
                Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
                Skip_Tokens(SKIP_TIL_END_COND);
            }
            else
            {
                Cond_Stack[CS_Index].Switch_Value=Parse_Cond_Param();
                Cond_Stack[CS_Index].Cond_Type=SWITCH_COND;
                Cond_Stack[CS_Index].Switch_Case_Ok_Flag=false;
                EXPECT_ONE
                    // NOTE: We actually expect a "#case" or "#range" here; however, this will trigger a nested call
                    // to Parse_Directive, so we'll encounter that CASE_TOKEN or RANGE_TOKEN here only by courtesy of the
                    // respective handler, which will UNGET the token and inform us via the Switch_Case_Ok_Flag
                    // that the CASE_TOKEN or RANGE_TOKEN we encounter here was properly preceded with a hash ("#").
                    CASE2(CASE_TOKEN,RANGE_TOKEN)
                        if (!Cond_Stack[CS_Index].Switch_Case_Ok_Flag)
                            Error("#switch not followed by #case or #range.");

                        if (Token.Token_Id==CASE_TOKEN)
                        {
                            Value=Parse_Cond_Param();
                            Flag = (fabs(Value-Cond_Stack[CS_Index].Switch_Value)<EPSILON);
                        }
                        else
                        {
                            Parse_Cond_Param2(&Value,&Value2);
                            Flag = ((Cond_Stack[CS_Index].Switch_Value >= Value) &&
                                    (Cond_Stack[CS_Index].Switch_Value <= Value2));
                        }

                        if(Flag)
                        {
                            Cond_Stack[CS_Index].Cond_Type=CASE_TRUE_COND;
                        }
                        else
                        {
                            Cond_Stack[CS_Index].Cond_Type=CASE_FALSE_COND;
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
                    if (Token.Token_Id==CASE_TOKEN)
                    {
                        Value=Parse_Cond_Param();
                        Flag = (fabs(Value-Cond_Stack[CS_Index].Switch_Value)<EPSILON);
                    }
                    else
                    {
                        Parse_Cond_Param2(&Value,&Value2);
                        Flag = ((Cond_Stack[CS_Index].Switch_Value >= Value) &&
                                (Cond_Stack[CS_Index].Switch_Value <= Value2));
                    }

                    if(Flag && (Curr_Type==CASE_FALSE_COND))
                    {
                        Cond_Stack[CS_Index].Cond_Type=CASE_TRUE_COND;
                        if (Skipping)
                        {
                            Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
                            Token.is_array_elem = false;
                            Token.is_mixed_array_elem = false;
                            Token.is_dictionary_elem = false;
                            UNGET
                        }
                    }
                    break;

                case SWITCH_COND:
                    Cond_Stack[CS_Index].Switch_Case_Ok_Flag=true;
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
                    POV_PARSER_ASSERT(false); // We should have identified the macro's proper `#end` at the beginning of this function.
                    Return_From_Macro();
                    if (--CS_Index < 0)
                    {
                        Error("Mis-matched '#end'.");
                    }
                    break;

                case IF_FALSE_COND:
                    Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
                    Token.is_array_elem = false;
                    Token.is_mixed_array_elem = false;
                    Token.is_dictionary_elem = false;
                    UNGET
                    // FALLTHROUGH
                case IF_TRUE_COND:
                case ELSE_COND:
                case CASE_TRUE_COND:
                case CASE_FALSE_COND:
                case DECLARING_MACRO_COND:
                case SKIP_TIL_END_COND:
                    if (Curr_Type==DECLARING_MACRO_COND)
                    {
                        if ((PMac=Cond_Stack[CS_Index].PMac) != nullptr)
                        {
                            PMac->Macro_End=Hash_Loc;
                            ITextStream::FilePos pos = Input_File->In_File->tellg();
                            POV_OFF_T macroLength = pos.offset - PMac->Macro_File_Pos.offset;
                            if (macroLength <= MaxCachedMacroSize)
                            {
                                PMac->CacheSize = macroLength;
                                PMac->Cache = new unsigned char[PMac->CacheSize];
                                if (PMac->Cache)
                                {
                                    Input_File->In_File->seekg(PMac->Macro_File_Pos);
                                    if (!Input_File->In_File->ReadRaw(PMac->Cache, PMac->CacheSize))
                                    {
                                        delete[] PMac->Cache;
                                        PMac->Cache = nullptr;
                                    }
                                    Input_File->In_File->seekg(pos);
                                }
                            }
                        }
                    }
                    if (--CS_Index < 0)
                    {
                        Error("Mis-matched '#end'.");
                    }
                    if (Skipping)
                    {
                        Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
                        Token.is_array_elem = false;
                        Token.is_mixed_array_elem = false;
                        Token.is_dictionary_elem = false;
                        UNGET
                    }
                    break;

                case WHILE_COND:
                    if (Cond_Stack[CS_Index].Loop_File != Input_File->In_File)
                    {
                        Error("#while loop did not end in file where it started.");
                    }

                    Got_EOF=false;
                    if (!Input_File->In_File->seekg(Cond_Stack[CS_Index].File_Pos))
                    {
                        Error("Unable to seek in input file for #while directive.");
                    }

                    Value=Parse_Cond_Param();

                    if (fabs(Value)<EPSILON)
                    {
                        Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
                        Skip_Tokens(SKIP_TIL_END_COND);
                    }
                    break;

                case FOR_COND:
                    if (Cond_Stack[CS_Index].Loop_File != Input_File->In_File)
                    {
                        Error("#for loop did not end in file where it started.");
                    }

                    Got_EOF=false;
                    if (!Input_File->In_File->seekg(Cond_Stack[CS_Index].File_Pos))
                    {
                        Error("Unable to seek in input file for #for directive.");
                    }

                    {
                        SYM_ENTRY* Entry = Find_Symbol(Table_Index, Cond_Stack[CS_Index].Loop_Identifier);
                        if ((Entry == nullptr) || (Entry->Token_Number != FLOAT_ID_TOKEN))
                            Error ("#for loop variable must remain defined and numerical during loop.");

                        DBL* CurrentPtr = reinterpret_cast<DBL *>(Entry->Data);
                        DBL  End        = Cond_Stack[CS_Index].For_Loop_End;
                        DBL  Step       = Cond_Stack[CS_Index].For_Loop_Step;

                        *CurrentPtr = *CurrentPtr + Step;

                        if ( ((Step > 0) && (*CurrentPtr > End + EPSILON)) ||
                             ((Step < 0) && (*CurrentPtr < End - EPSILON)) )
                        {
                            POV_FREE(Cond_Stack[CS_Index].Loop_Identifier);
                            Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
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
                Parse_Declare(Token.Token_Id == LOCAL_TOKEN, After_Hash);
                Curr_Type = Cond_Stack[CS_Index].Cond_Type;
                if (Token.Unget_Token)
                {
                    switch (Token.Token_Id)
                    {
                        case HASH_TOKEN:
                            Token.Unget_Token=false;
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
                switch(Token.Function_Id)
                {
                    case VERSION_TOKEN:
                        {
                            if (sceneData->languageVersionSet == false && token_count > 1)
                                sceneData->languageVersionLate = true;
                            Ok_To_Declare = false;
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
                                // standard include files that happens to have been updated since the scene was
                                // originally designed.)

                                if (Include_File_Index == 0)
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

                            Ok_To_Declare = true;
                            parsingVersionDirective = wasParsingVersionDirective;
                            Curr_Type = Cond_Stack[CS_Index].Cond_Type;
                            if (Token.Unget_Token && (Token.Token_Id==HASH_TOKEN))
                            {
                                Token.Unget_Token=false;
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
                Ok_To_Declare = false;
                EXPECT_ONE
                    CASE (IDENTIFIER_TOKEN)
                        Warning("Attempt to undef unknown identifier");
                    END_CASE

                    CASE (FILE_ID_TOKEN)
                        if (reinterpret_cast<DATA_FILE *>(Token.Data)->busyParsing)
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
                        Remove_Symbol (Token.table, Token.Token_String, Token.is_array_elem, Token.DataPtr, Token.Token_Id);
                        if (Token.is_mixed_array_elem)
                            *Token.NumberPtr = IDENTIFIER_TOKEN;
                    END_CASE

                    CASE2 (VECTOR_FUNCT_TOKEN, FLOAT_FUNCT_TOKEN)
                        switch(Token.Function_Id)
                        {
                            case VECTOR_ID_TOKEN:
                            case FLOAT_ID_TOKEN:
                                Remove_Symbol (Token.table, Token.Token_String, Token.is_array_elem, Token.DataPtr, Token.Token_Id);
                                if (Token.is_mixed_array_elem)
                                    *Token.NumberPtr = IDENTIFIER_TOKEN;
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
                Ok_To_Declare = true;
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
            Cond_Stack[CS_Index].Cond_Type = DECLARING_MACRO_COND;
            Cond_Stack[CS_Index].PMac      = PMac;
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

    if (Token.Unget_Token)
    {
        Token.Unget_Token = false;
    }
    else
    {
        Token.Token_Id = END_OF_FILE_TOKEN;
        Token.is_array_elem = false;
        Token.is_mixed_array_elem = false;
        Token.is_dictionary_elem = false;
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
    UCS2String actualFileName; // Name the file is known by on the local system.

    if (Skip_Spaces () != true)
        Error ("Expecting a string after INCLUDE.");

    asciiFileName = Parse_C_String(true);
    formalFileName = ASCIItoUCS2String(asciiFileName);
    POV_FREE(asciiFileName);

    Include_File_Index++;

    if (Include_File_Index >= MAX_INCLUDE_FILES)
    {
        Include_File_Index--;
        Error ("Too many nested include files.");
    }

    Echo_Indx = 0;

    Input_File = &Include_Files[Include_File_Index];

    // must set this to `nullptr` in case it's uninitialized - if Locate_File throws an
    // exception (e.g. I/O restriction error), the parser shut-down code will attempt
    // to free In_File if it's not `nullptr`.
    Input_File->In_File = nullptr;

    IStream *is = Locate_File(formalFileName, POV_File_Text_INC, actualFileName, true);
    if (is == nullptr)
    {
        Input_File->In_File = nullptr;  /* Keeps from closing failed file. */
        Error ("Cannot open include file %s.", UCS2toASCIIString(formalFileName).c_str());
    }
    else
        Input_File->In_File = new IBufferedTextStream(formalFileName.c_str(), is);

    Input_File->R_Flag=false;

    Add_Sym_Table();

    Token.Token_Id = END_OF_FILE_TOKEN;
    Token.is_array_elem = false;
    Token.is_mixed_array_elem = false;
    Token.is_dictionary_elem = false;
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
    int Temp      = CS_Index;
    int Prev_Skip = Skipping;

    Skipping=true;

    while ((CS_Index > Temp) || ((CS_Index == Temp) && (Cond_Stack[CS_Index].Cond_Type == cond)))
    {
        Get_Token();
    }

    Skipping=Prev_Skip;

    if (Token.Token_Id==HASH_TOKEN)
    {
        Token.Token_Id=END_OF_FILE_TOKEN;
        Token.is_array_elem = false;
        Token.is_mixed_array_elem = false;
        Token.is_dictionary_elem = false;
        Token.Unget_Token=false;
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

    while ( (CS_Index > 0) &&
            (Cond_Stack[CS_Index].Cond_Type != WHILE_COND) &&
            (Cond_Stack[CS_Index].Cond_Type != FOR_COND) &&
            (Cond_Stack[CS_Index].Cond_Type != CASE_TRUE_COND) &&
            (Cond_Stack[CS_Index].Cond_Type != INVOKING_MACRO_COND) )
    {
        Get_Token();
    }

    if (CS_Index == 0)
        Error ("Invalid context for #break");

    if (Cond_Stack[CS_Index].Cond_Type == INVOKING_MACRO_COND)
    {
        Skipping=Prev_Skip;
        Return_From_Macro();
        --CS_Index;
        return;
    }

    Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
    Skip_Tokens (SKIP_TIL_END_COND);

    Skipping=Prev_Skip;

    if (Token.Token_Id==HASH_TOKEN)
    {
        Token.Token_Id=END_OF_FILE_TOKEN;
        Token.is_array_elem = false;
        Token.is_mixed_array_elem = false;
        Token.is_dictionary_elem = false;
        Token.Unget_Token=false;
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
    int i;

    Add_Sym_Table();

    for (i = 0; Reserved_Words[i].Token_Name != nullptr; i++)
    {
        Add_Symbol(SYM_TABLE_RESERVED,Reserved_Words[i].Token_Name,Reserved_Words[i].Token_Number);
    }

    Add_Sym_Table();
}

Parser::SYM_TABLE *Parser::Create_Sym_Table (bool copyNames)
{
    SYM_TABLE *New = reinterpret_cast<SYM_TABLE *>(POV_MALLOC(sizeof(SYM_TABLE),"symbol table"));
    New->namesAreCopies = copyNames;

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

    Tables[Table_Index] = Create_Sym_Table (Table_Index != 0);
}

void Parser::Destroy_Sym_Table (Parser::SYM_TABLE *Table)
{
    SYM_ENTRY *Entry;

    for(int i = SYM_TABLE_SIZE - 1; i >= 0; i--)
    {
        Entry = Table->Table[i];

        while(Entry)
        {
            Entry = Destroy_Entry (Entry, Table->namesAreCopies);
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
    SYM_TABLE *newTable = Create_Sym_Table (true);
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

SYM_ENTRY *Parser::Create_Entry (const char *Name, TOKEN Number, bool copyName)
{
    SYM_ENTRY *New;

    New = reinterpret_cast<SYM_ENTRY *>(POV_MALLOC(sizeof(SYM_ENTRY), "symbol table entry"));

    New->Token_Number        = Number;
    New->Data                = nullptr;
    New->deprecated          = false;
    New->deprecatedOnce      = false;
    New->deprecatedShown     = false;
    New->Deprecation_Message = nullptr;
    New->ref_count           = 1;
    if (copyName)
        New->Token_Name = POV_STRDUP(Name);
    else
        New->Token_Name = const_cast<char*>(Name);

    return(New);
}

SYM_ENTRY *Parser::Copy_Entry (const SYM_ENTRY *oldEntry)
{
    SYM_ENTRY *newEntry;

    newEntry = reinterpret_cast<SYM_ENTRY *>(POV_MALLOC(sizeof(SYM_ENTRY), "symbol table entry"));

    newEntry->Token_Number        = oldEntry->Token_Number;
    newEntry->Data                = Copy_Identifier (oldEntry->Data, oldEntry->Token_Number);
    newEntry->deprecated          = false;
    newEntry->deprecatedOnce      = false;
    newEntry->deprecatedShown     = false;
    newEntry->Deprecation_Message = nullptr;
    newEntry->ref_count           = 1;
    newEntry->Token_Name          = POV_STRDUP (oldEntry->Token_Name);

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
        if (table->namesAreCopies) // NB reserved words reference hard-coded token names in code segment, rather than allocated on heap
        {
            POV_FREE(Entry->Token_Name);
            Destroy_Ident_Data (Entry->Data, Entry->Token_Number); // TODO - shouldn't this be outside the if() block?
        }
        if (Entry->Deprecation_Message != nullptr)
            POV_FREE(Entry->Deprecation_Message);

        POV_FREE(Entry);
    }
}

SYM_ENTRY *Parser::Destroy_Entry (SYM_ENTRY *Entry, bool destroyName)
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
        if (destroyName) // NB reserved words reference hard-coded token names in code segment, rather than allocated on heap
        {
            POV_FREE(Entry->Token_Name);
            Destroy_Ident_Data (Entry->Data, Entry->Token_Number); // TODO - shouldn't this be outside the if() block?
        }
        if (Entry->Deprecation_Message != nullptr)
            POV_FREE(Entry->Deprecation_Message);

        POV_FREE(Entry);
    }

    return Next;
}


void Parser::Add_Entry (SYM_TABLE *table, SYM_ENTRY *Table_Entry)
{
    int i = get_hash_value(Table_Entry->Token_Name);

    Table_Entry->next       = table->Table[i];
    table->Table[i] = Table_Entry;
}

void Parser::Add_Entry (int Index,SYM_ENTRY *Table_Entry)
{
    Add_Entry (Tables[Index], Table_Entry);
}


SYM_ENTRY *Parser::Add_Symbol (SYM_TABLE *table, const char *Name, TOKEN Number)
{
    SYM_ENTRY *New;

    New = Create_Entry (Name, Number, table->namesAreCopies);
    Add_Entry (table, New);

    return(New);
}

SYM_ENTRY *Parser::Add_Symbol (int Index,const char *Name,TOKEN Number)
{
    SYM_ENTRY *New;

    New = Create_Entry (Name, Number, (Index != SYM_TABLE_RESERVED));
    Add_Entry(Index,New);

    return(New);
}


SYM_ENTRY *Parser::Find_Symbol (const SYM_TABLE *table, const char *Name, int hash)
{
    SYM_ENTRY *Entry;

    Entry = table->Table[hash];

    while (Entry)
    {
        if (strcmp(Name, Entry->Token_Name) == 0)
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
            if (strcmp(Name, Entry->Token_Name) == 0)
            {
                *EntryPtr = Entry->next;
                Destroy_Entry (Entry, table->namesAreCopies);
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
    bool Old_Ok = Ok_To_Declare;
    MacroParameter newParameter;

    Check_Macro_Vers();

    Ok_To_Declare = false;

    EXPECT_ONE
        CASE (IDENTIFIER_TOKEN)
            Table_Entry = Add_Symbol (SYM_TABLE_GLOBAL,Token.Token_String,TEMPORARY_MACRO_ID_TOKEN);
        END_CASE

        CASE (MACRO_ID_TOKEN)
            Remove_Symbol(SYM_TABLE_GLOBAL, Token.Token_String, false, nullptr, 0);
            Table_Entry = Add_Symbol (SYM_TABLE_GLOBAL,Token.Token_String,TEMPORARY_MACRO_ID_TOKEN);
        END_CASE

        OTHERWISE
            Parse_Error(IDENTIFIER_TOKEN);
        END_CASE
    END_EXPECT

    New = new Macro(Token.Token_String);

    Table_Entry->Data=reinterpret_cast<void *>(New);

    New->Macro_Filename = nullptr;

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
            newParameter.name = POV_STRDUP(Token.Token_String);
            New->parameters.push_back(newParameter);
            Parse_Comma();
            newParameter.optional = false;
        END_CASE

        CASE2 (VECTOR_FUNCT_TOKEN, FLOAT_FUNCT_TOKEN)
            switch(Token.Function_Id)
            {
                case VECTOR_ID_TOKEN:
                case FLOAT_ID_TOKEN:
                    newParameter.name = POV_STRDUP(Token.Token_String);
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

    Ok_To_Declare = Old_Ok;

    Table_Entry->Token_Number = MACRO_ID_TOKEN;

    New->Macro_Filename = UCS2_strdup(Input_File->In_File->name());
    New->Macro_File_Pos = Input_File->In_File->tellg();
    New->Macro_File_Col = Echo_Indx;

    Parse_Paren_End();

    Check_Macro_Vers();

    return (New);
}


void Parser::Invoke_Macro()
{
    Macro *PMac=reinterpret_cast<Macro *>(Token.Data);
    SYM_ENTRY **Table_Entries = nullptr;
    int i,Local_Index;

    Inc_CS_Index();

    if (PMac == nullptr)
    {
        if (Token.DataPtr != nullptr)
            PMac = reinterpret_cast<Macro*>(*(Token.DataPtr));
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
            Table_Entries[i] = Create_Entry (PMac->parameters[i].name, IDENTIFIER_TOKEN, true);
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

                Destroy_Entry (Table_Entries[i], true);
                Table_Entries[i] = nullptr;
            }
            properlyDelimited = Parse_Comma();
        }
    }

    Parse_Paren_End();

    Cond_Stack[CS_Index].Cond_Type = INVOKING_MACRO_COND;

    Cond_Stack[CS_Index].File_Pos          = Input_File->In_File->tellg();
    Cond_Stack[CS_Index].Macro_Return_Col  = Echo_Indx;
    Cond_Stack[CS_Index].Macro_Return_Name = Input_File->In_File->name();
    Cond_Stack[CS_Index].PMac              = PMac;

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

    if (PMac->Cache || UCS2_strcmp(PMac->Macro_Filename,Input_File->In_File->name()))
    {
        UCS2String ign;
        /* Not in same file */
        Cond_Stack[CS_Index].Macro_Same_Flag=false;
        Cond_Stack[CS_Index].Macro_File = Input_File->In_File;
//  POV_DELETE(Input_File->In_File, IStream);
        Got_EOF=false;
        Input_File->R_Flag=false;
        IStream *is;
        if (PMac->Cache)
        {
            Input_File->In_File = new IMemTextStream(PMac->Macro_Filename, Cond_Stack[CS_Index].PMac->Cache, PMac->CacheSize, PMac->Macro_File_Pos);
        }
        else
        {
            is = Locate_File (PMac->Macro_Filename, POV_File_Text_Macro, ign, true);
            if (is == nullptr)
            {
                Input_File->In_File = nullptr;  /* Keeps from closing failed file. */
                Error ("Cannot open macro file '%s'.", UCS2toASCIIString(UCS2String(PMac->Macro_Filename)).c_str());
            }
            else
                Input_File->In_File = new IBufferedTextStream(PMac->Macro_Filename, is);
        }
    }
    else
    {
        Cond_Stack[CS_Index].Macro_Same_Flag=true;
    }

    Got_EOF=false;
    if (!Input_File->In_File->seekg(PMac->Macro_File_Pos))
    {
        Error("Unable to file seek in macro.");
    }
    Echo_Indx = PMac->Macro_File_Col;

    Token.Token_Id = END_OF_FILE_TOKEN;
    Token.is_array_elem = false;
    Token.is_mixed_array_elem = false;
    Token.is_dictionary_elem = false;

    Check_Macro_Vers();

}

void Parser::Return_From_Macro()
{
    Check_Macro_Vers();

    if (!Cond_Stack[CS_Index].Macro_Same_Flag)
    {
        if (Token.FileHandle == Input_File->In_File)
            Token.FileHandle = nullptr;
        delete Input_File->In_File;
        Input_File->R_Flag=false;
        Input_File->In_File = Cond_Stack[CS_Index].Macro_File;
        if (Token.FileHandle == nullptr)
            Token.FileHandle = Input_File->In_File;
    }

    Got_EOF=false;

    if (!Input_File->In_File->seekg(Cond_Stack[CS_Index].File_Pos))
    {
        Error("Unable to file seek in return from macro.");
    }

    Echo_Indx = Cond_Stack[CS_Index].Macro_Return_Col;

    // Always destroy macro locals
    Destroy_Table(Table_Index--);
}

Parser::Macro::Macro(const char *s) :
    Macro_Name(POV_STRDUP(s)),
    Macro_Filename(nullptr),
    Cache(nullptr)
{}

Parser::Macro::~Macro()
{
    int i;

    POV_FREE(Macro_Name);
    if (Macro_Filename != nullptr)
    {
        POV_FREE(Macro_Filename);
    }

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

    Ok_To_Declare = false;

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

    Ok_To_Declare = true;
    return(New);
};


Parser::SYM_TABLE *Parser::Parse_Dictionary_Declare()
{
    SYM_TABLE *newDictionary;
    SYM_ENTRY *newEntry;
    bool oldParseRawIdentifiers;
    char *dictIndex;

    newDictionary = Create_Sym_Table (true);

    // TODO REVIEW - maybe we need `Ok_To_Declare = false`?

    if (!Parse_Begin (false))
        return newDictionary;

    EXPECT
        CASE (PERIOD_TOKEN)
            oldParseRawIdentifiers = parseRawIdentifiers;
            parseRawIdentifiers = true;
            Get_Token();
            parseRawIdentifiers = oldParseRawIdentifiers;
            if (Token.Token_Id != IDENTIFIER_TOKEN)
                Expectation_Error ("dictionary element identifier");
            newEntry = Add_Symbol (newDictionary, Token.Token_String, IDENTIFIER_TOKEN);

            GET (COLON_TOKEN);

            if (!Parse_RValue (IDENTIFIER_TOKEN, &(newEntry->Token_Number), &(newEntry->Data), newEntry, false, false, true, true, false, MAX_NUMBER_OF_TABLES))
                Expectation_Error("RValue");

            Parse_Comma();
        END_CASE

        CASE (LEFT_SQUARE_TOKEN)
            UNGET
            Parse_Square_Begin();
            dictIndex = Parse_C_String();
            newEntry = Add_Symbol (newDictionary, dictIndex, IDENTIFIER_TOKEN);
            POV_PARSER_ASSERT (newDictionary->namesAreCopies);
            POV_FREE (dictIndex);
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
                            // We reserved one element too many.
                            a->Shrink();
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
    IStream *rfile = nullptr;
    OStream *wfile = nullptr;
    DATA_FILE *New;
    char *asciiFileName;
    UCS2String fileName;
    UCS2String ign;
    SYM_ENTRY *Entry;

    New=reinterpret_cast<DATA_FILE *>(POV_MALLOC(sizeof(DATA_FILE),"user file"));
    New->In_File = nullptr;
    New->Out_File = nullptr;

    // Safeguard against accidental nesting of other file access directives inside the `#fopen`
    // directive (or the user forgetting portions of the directive).
    New->busyParsing = true;

    GET(IDENTIFIER_TOKEN)
    Entry = Add_Symbol (SYM_TABLE_GLOBAL,Token.Token_String,FILE_ID_TOKEN);
    Entry->Data=reinterpret_cast<void *>(New);

    asciiFileName = Parse_C_String(true);
    fileName = ASCIItoUCS2String(asciiFileName);
    POV_FREE(asciiFileName);

    EXPECT_ONE
        CASE(READ_TOKEN)
            New->R_Flag = true;
            rfile = Locate_File(fileName.c_str(), POV_File_Text_User, ign, true);
            if (rfile != nullptr)
                New->In_File = new IBufferedTextStream(fileName.c_str(), rfile);
            else
                New->In_File = nullptr;

            if (New->In_File == nullptr)
                Error ("Cannot open user file %s (read).", UCS2toASCIIString(fileName).c_str());
        END_CASE

        CASE(WRITE_TOKEN)
            New->R_Flag = false;
            wfile = CreateFile(fileName.c_str(), POV_File_Text_User, false);
            if (wfile != nullptr)
                New->Out_File= new OTextStream(fileName.c_str(), wfile);
            else
                New->Out_File = nullptr;

            if (New->Out_File == nullptr)
                Error ("Cannot open user file %s (write).", UCS2toASCIIString(fileName).c_str());
        END_CASE

        CASE(APPEND_TOKEN)
            New->R_Flag = false;
            wfile = CreateFile(fileName.c_str(), POV_File_Text_User, true);
            if (wfile != nullptr)
                New->Out_File= new OTextStream(fileName.c_str(), wfile);
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
            Data=reinterpret_cast<DATA_FILE *>(Token.Data);
            if (Data->busyParsing)
                Error ("Can't nest directives accessing the same file.");
            // NB no need to set Data->busyParsing, as we're not reading any tokens where the
            // tokenizer might stumble upon nested file access directives
            if (Data->In_File != nullptr)
                delete Data->In_File;
            if (Data->Out_File != nullptr)
                delete Data->Out_File;
            Got_EOF=false;
            Data->In_File = nullptr;
            Data->Out_File = nullptr;
            Remove_Symbol(SYM_TABLE_GLOBAL, Token.Token_String, false, nullptr, 0);
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
    User_File=reinterpret_cast<DATA_FILE *>(Token.Data);
    if (User_File->busyParsing)
        Error ("Can't nest directives accessing the same file.");
    File_Id=POV_STRDUP(Token.Token_String);
    if (User_File->In_File == nullptr)
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
                Temp_Entry = Add_Symbol (SYM_TABLE_GLOBAL,Token.Token_String,IDENTIFIER_TOKEN);
                End_File=Parse_Read_Value (User_File,Token.Token_Id, &(Temp_Entry->Token_Number), &(Temp_Entry->Data));
                Token.is_array_elem = false;
                Token.is_mixed_array_elem = false;
                Token.is_dictionary_elem = false;
                Parse_Comma(); /* Scene file comma between 2 idents */
            }
        END_CASE

        CASE (STRING_ID_TOKEN)
            if (!End_File)
            {
                End_File=Parse_Read_Value (User_File,Token.Token_Id,Token.NumberPtr,Token.DataPtr);
                Token.is_array_elem = false;
                Token.is_mixed_array_elem = false;
                Token.is_dictionary_elem = false;
                Parse_Comma(); /* Scene file comma between 2 idents */
            }
        END_CASE

        CASE2 (VECTOR_FUNCT_TOKEN,FLOAT_FUNCT_TOKEN)
            switch(Token.Function_Id)
            {
                case VECTOR_ID_TOKEN:
                case FLOAT_ID_TOKEN:
                    if (!End_File)
                    {
                        End_File=Parse_Read_Value (User_File,Token.Function_Id,Token.NumberPtr,Token.DataPtr);
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
        delete User_File->In_File;
        Got_EOF=false;
        User_File->In_File = nullptr;
        Remove_Symbol(SYM_TABLE_GLOBAL, File_Id, false, nullptr, 0);
    }
    POV_FREE(File_Id);
}

int Parser::Parse_Read_Value(DATA_FILE *User_File,int Previous,int *NumberPtr,void **DataPtr)
{
    pov_base::ITextStream *Temp;
    bool Temp_R_Flag;
    DBL Val;
    int End_File=false;
    int i;
    EXPRESS Express;

    Temp = Input_File->In_File;
    Temp_R_Flag = Input_File->R_Flag;
    Input_File->In_File = User_File->In_File;
    Input_File->R_Flag = User_File->R_Flag;
    if (User_File->In_File == nullptr)
        Error("Cannot read from file '%s' because the file is open for writing only.", UCS2toASCIIString(UCS2String(User_File->Out_File->name())).c_str());
    User_File->In_File = nullptr; // take control over pointer

    try
    {
        EXPECT_ONE
            CASE3 (PLUS_TOKEN,DASH_TOKEN,FLOAT_FUNCT_TOKEN)
                UNGET
                Val=Parse_Signed_Float();
                *NumberPtr = FLOAT_ID_TOKEN;
                Test_Redefine(Previous,NumberPtr,*DataPtr);
                *DataPtr   = reinterpret_cast<void *>(Create_Float());
                *(reinterpret_cast<DBL *>(*DataPtr)) = Val;
                Parse_Comma(); /* data file comma between 2 data items  */
            END_CASE

            CASE (LEFT_ANGLE_TOKEN)
                UNGET
                Parse_Angle_Begin();

                i=1;
                Express[X]=Parse_Signed_Float();  Parse_Comma();
                Express[Y]=Parse_Signed_Float();  Parse_Comma();

                EXPECT
                    CASE3 (PLUS_TOKEN,DASH_TOKEN,FLOAT_FUNCT_TOKEN)
                        UNGET
                        if (++i>4)
                        {
                            Error("Vector data too long");
                        }
                        Express[i]=Parse_Signed_Float(); Parse_Comma();
                    END_CASE

                    CASE (RIGHT_ANGLE_TOKEN)
                        UNGET
                        EXIT
                    END_CASE

                    OTHERWISE
                        Expectation_Error("vector");
                    END_CASE
                END_EXPECT

                Parse_Angle_End();

                switch(i)
                {
                    case 1:
                        *NumberPtr = UV_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr);
                        *DataPtr   = reinterpret_cast<void *>(new Vector2d(Express));
                        break;

                    case 2:
                        *NumberPtr = VECTOR_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr);
                        *DataPtr   = reinterpret_cast<void *>(new Vector3d(Express));
                        break;

                    case 3:
                        *NumberPtr = VECTOR_4D_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr);
                        *DataPtr   = reinterpret_cast<void *>(Create_Vector_4D());
                        Assign_Vector_4D(reinterpret_cast<DBL *>(*DataPtr), Express);
                        break;

                    case 4:
                        *NumberPtr    = COLOUR_ID_TOKEN;
                        Test_Redefine(Previous,NumberPtr,*DataPtr);
                        *DataPtr      = reinterpret_cast<void *>(Create_Colour());
                        (*reinterpret_cast<RGBFTColour *>(*DataPtr)).Set(Express, 5); /* NK fix assign_colour bug */
                        break;
                }

                Parse_Comma(); // data file comma between 2 data items
            END_CASE

            CASE(STRING_LITERAL_TOKEN)
                *NumberPtr = STRING_ID_TOKEN;
                Test_Redefine(Previous,NumberPtr,*DataPtr);
                *DataPtr   = String_Literal_To_UCS2(Token.Token_String, false);
                Parse_Comma(); // data file comma between 2 data items
            END_CASE

            CASE (END_OF_FILE_TOKEN)
            END_CASE

            OTHERWISE
                Expectation_Error ("float, vector, or string literal");
            END_CASE
        END_EXPECT
    }
    catch (...)
    {
        // re-assign the file pointers so that they are properly disposed of later on
        User_File->In_File = Input_File->In_File;
        Input_File->In_File = Temp;
        Input_File->R_Flag = Temp_R_Flag;
        throw;
    }

    if (Token.Token_Id==END_OF_FILE_TOKEN)
        End_File = true;

    Token.End_Of_File = false;
    Token.Unget_Token = false;
    Got_EOF = false;
    User_File->In_File = Input_File->In_File; // return control over pointer
    Input_File->In_File = Temp;
    Input_File->R_Flag = Temp_R_Flag;

    return End_File;
}

void Parser::Parse_Write(void)
{
    char *temp;
    DATA_FILE *User_File;
    EXPRESS Express;
    int Terms;

    Parse_Paren_Begin();

    GET(FILE_ID_TOKEN)

    User_File=reinterpret_cast<DATA_FILE *>(Token.Data);
    if (User_File->busyParsing)
        Error ("Can't nest directives accessing the same file.");
    if (User_File->Out_File == nullptr)
        Error("Cannot write to file %s because the file is open for reading only.", UCS2toASCIIString(UCS2String(User_File->In_File->name())).c_str());

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
                switch (Token.Token_Id)
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
                    CASE_VECTOR
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

        CASE_VECTOR
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
    bool Old_Ok = Ok_To_Declare;
    bool Old_Sk = Skipping;
    DBL Val;

    Ok_To_Declare = false;
    Skipping      = false;

    Val=Parse_Float_Param();

    Ok_To_Declare = Old_Ok;
    Skipping      = Old_Sk;

    return(Val);
}

void Parser::Parse_Cond_Param2(DBL *V1,DBL *V2)
{
    bool Old_Ok = Ok_To_Declare;
    bool Old_Sk = Skipping;

    Ok_To_Declare = false;
    Skipping      = false;

    Parse_Float_Param2(V1,V2);

    Ok_To_Declare = Old_Ok;
    Skipping      = Old_Sk;
}

void Parser::Inc_CS_Index()
{
    if (++CS_Index >= COND_STACK_SIZE)
    {
        Error("Too many nested conditionals or macros.");
    }
    Cond_Stack[CS_Index].Cond_Type = BUSY_COND;
    Cond_Stack[CS_Index].Macro_File = nullptr;
    Cond_Stack[CS_Index].Macro_Return_Name = nullptr;
    Cond_Stack[CS_Index].PMac = nullptr;
    Cond_Stack[CS_Index].Loop_File = nullptr;
    Cond_Stack[CS_Index].Loop_Identifier = nullptr;
}

bool Parser::Parse_Ifdef_Param ()
{
    bool retval = false;

    Parse_Paren_Begin();

    Inside_Ifdef=true;
    Get_Token();
    String2 = POV_STRDUP(String);
    Inside_Ifdef=false;

    if (Token.is_array_elem)
        retval = (*Token.DataPtr != nullptr);
    else
        retval = (Token.Token_Id != IDENTIFIER_TOKEN);

    Parse_Paren_End();

    POV_FREE(String2);
    String2 = nullptr;

    return retval;
}

int Parser::Parse_For_Param (char** IdentifierPtr, DBL* EndPtr, DBL* StepPtr)
{
    int Previous=-1;
    SYM_ENTRY *Temp_Entry = nullptr;

    Parse_Paren_Begin();

    LValue_Ok = true;

    EXPECT_ONE
        CASE (IDENTIFIER_TOKEN)
            POV_PARSER_ASSERT(!Token.is_array_elem);
            if (Token.is_dictionary_elem)
                Error("#for loop variable must not be an array or dictionary element");
            Temp_Entry = Add_Symbol (Table_Index,Token.Token_String,IDENTIFIER_TOKEN);
            Token.NumberPtr = &(Temp_Entry->Token_Number);
            Token.DataPtr = &(Temp_Entry->Data);
            Previous = Token.Token_Id;
        END_CASE

        CASE3 (FILE_ID_TOKEN, MACRO_ID_TOKEN, PARAMETER_ID_TOKEN)
            // TODO - We should allow assignment if the identifier is non-local.
            if (Token.is_array_elem || Token.is_dictionary_elem)
                Error("#for loop variable must not be an array or dictionary element");
            Parse_Error(IDENTIFIER_TOKEN);
        END_CASE

        CASE2 (FUNCT_ID_TOKEN, VECTFUNCT_ID_TOKEN)
            if (Token.is_array_elem || Token.is_dictionary_elem)
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
            if (Token.is_array_elem || Token.is_dictionary_elem)
                Error("#for loop variable must not be an array or dictionary element");
            if (Token.context != Table_Index)
            {
                Temp_Entry = Add_Symbol (Table_Index,Token.Token_String,IDENTIFIER_TOKEN);
                Token.NumberPtr = &(Temp_Entry->Token_Number);
                Token.DataPtr   = &(Temp_Entry->Data);
                Previous        = IDENTIFIER_TOKEN;
            }
            else
            {
                Previous        = Token.Token_Id;
            }
        END_CASE

        CASE (EMPTY_ARRAY_TOKEN)
            POV_PARSER_ASSERT (Token.is_array_elem && !Token.is_mixed_array_elem);
            Error("#for loop variable must not be an array element");
            Previous = Token.Token_Id;
        END_CASE

        CASE2 (VECTOR_FUNCT_TOKEN, FLOAT_FUNCT_TOKEN)
            if (Token.is_array_elem || Token.is_dictionary_elem)
                Error("#for loop variable must not be an array or dictionary element");
            switch(Token.Function_Id)
            {
                case VECTOR_ID_TOKEN:
                case FLOAT_ID_TOKEN:
                    if (Token.context != Table_Index)
                    {
                        Temp_Entry = Add_Symbol (Table_Index,Token.Token_String,IDENTIFIER_TOKEN);
                        Token.NumberPtr = &(Temp_Entry->Token_Number);
                        Token.DataPtr   = &(Temp_Entry->Data);
                        Previous        = IDENTIFIER_TOKEN;
                    }
                    else
                    {
                        Previous        = Token.Function_Id;
                    }
                    break;

                default:
                    Parse_Error(IDENTIFIER_TOKEN);
                    break;
            }
        END_CASE

        OTHERWISE
            if (Token.is_array_elem || Token.is_dictionary_elem)
                Error("#for loop variable must not be an array or dictionary element");
            Parse_Error(IDENTIFIER_TOKEN);
        END_CASE
    END_EXPECT

    LValue_Ok = false;

    *Token.NumberPtr = FLOAT_ID_TOKEN;
    Test_Redefine(Previous,Token.NumberPtr,*Token.DataPtr, true);
    *Token.DataPtr   = reinterpret_cast<void *>(Create_Float());
    DBL* CurrentPtr = (reinterpret_cast<DBL *>(*Token.DataPtr));

    size_t len = strlen(Token.Token_String)+1;
    *IdentifierPtr = reinterpret_cast<char *>(POV_MALLOC(len, "loop identifier"));
    memcpy(*IdentifierPtr, Token.Token_String, len);

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

    if (++Include_File_Index >= MAX_INCLUDE_FILES)
    {
        Include_File_Index--;
        Error ("Too many nested include files.");
    }

    Echo_Indx = 0;

    Input_File = &Include_Files[Include_File_Index];
    Input_File->In_File = nullptr;
    IStream *is = Locate_File (formalFileName.c_str(),POV_File_Text_INC,actualFileName,true);
    if (is == nullptr)
    {
        Input_File->In_File = nullptr;  /* Keeps from closing failed file. */
        Error ("Cannot open include header file %s.", UCS2toASCIIString(formalFileName).c_str());
    }
    else
        Input_File->In_File = new IBufferedTextStream(formalFileName.c_str(), is);

    Input_File->R_Flag=false;

    Add_Sym_Table();

    Token.Token_Id = END_OF_FILE_TOKEN;
    Token.is_array_elem = false;
    Token.is_mixed_array_elem = false;
    Token.is_dictionary_elem = false;
}

}
