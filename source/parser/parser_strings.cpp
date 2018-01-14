//******************************************************************************
///
/// @file parser/parser_strings.cpp
///
/// This module implements parsing and conversion of string expressions.
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

// C++ variants of C standard header files
#include <cctype>
#include <cstdio>
#include <cstdlib>

// Boost header files
#include <boost/date_time/posix_time/posix_time.hpp>

// POV-Ray header files (core module)
#include "core/scene/scenedata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

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

char *Parser::Parse_C_String(bool pathname)
{
    UCS2 *str = Parse_String(pathname);
    char *New = UCS2_To_String(str);

    POV_FREE(str);

    return New;
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

UCS2 *Parser::Parse_String(bool pathname, bool require)
{
    UCS2 *New = NULL;
    int len = 0;

    EXPECT
        CASE(STRING_LITERAL_TOKEN)
            New = String_Literal_To_UCS2(Token.Token_String, pathname);
            EXIT
        END_CASE

        CASE(STR_TOKEN)
            New = Parse_Str(pathname);
            EXIT
        END_CASE

        CASE(VSTR_TOKEN)
            New = Parse_VStr(pathname);
            EXIT
        END_CASE

        CASE(CONCAT_TOKEN)
            New = Parse_Concat(pathname);
            EXIT
        END_CASE

        CASE(CHR_TOKEN)
            New = Parse_Chr(pathname);
            EXIT
        END_CASE

        CASE(DATETIME_TOKEN)
            New = Parse_Datetime(pathname);
            EXIT
        END_CASE

        CASE(SUBSTR_TOKEN)
            New = Parse_Substr(pathname);
            EXIT
        END_CASE

        CASE(STRUPR_TOKEN)
            New = Parse_Strupr(pathname);
            EXIT
        END_CASE

        CASE(STRLWR_TOKEN)
            New = Parse_Strlwr(pathname);
            EXIT
        END_CASE

        CASE(STRING_ID_TOKEN)
            len = UCS2_strlen(reinterpret_cast<UCS2 *>(Token.Data)) + 1;
            New = reinterpret_cast<UCS2 *>(POV_MALLOC(len * sizeof(UCS2), "UCS2 String"));
            POV_MEMMOVE(reinterpret_cast<void *>(New), reinterpret_cast<void *>(Token.Data), len * sizeof(UCS2));
            EXIT
        END_CASE

        OTHERWISE
            if(require)
                Expectation_Error("string expression");
            else
            {
                UNGET
                EXIT
            }
        END_CASE
    END_EXPECT

    return New;
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

UCS2 *Parser::Parse_Str(bool pathname)
{
    char *p;
    char temp3[128];
    char temp4[256];
    DBL val;
    int l, d;

    Parse_Paren_Begin();
    val = Parse_Float();
    Parse_Comma();
    l = (int)Parse_Float();
    Parse_Comma();
    d = (int)Parse_Float();
    Parse_Paren_End();

    p = temp3;
    *p++ = '%';
    if (l > 0)
    {
        p += sprintf(p, "%d", l);
    }
    else
    {
        if (l)
            p += sprintf(p, "0%d", abs(l));
    }

    if (d >= 0)
        p += sprintf(p, ".%d", d);
    strcpy(p, "f");

    // a very large floating point value (e.g. 1e251) will overflow the buffer.
    // TODO: consider changing to %g rather than %f for large numbers (e.g.
    // anything over 1e+64 for example). for now, we will only use %g if the
    // snprintf filled the buffer.
    // NB `snprintf` may report errors via a negative return value.
    if (((l = std::snprintf(temp4, sizeof(temp4), temp3, val)) >= sizeof(temp4)) || (l < 0))
    {
        *p = 'g';

        // it should not be possible to overflow with %g. but just in case ...
        if (((l = std::snprintf(temp4, sizeof(temp4), temp3, val)) >= sizeof(temp4)) || (l < 0))
            strcpy(temp4, "<invalid>");
    }

    return String_To_UCS2(temp4);
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

UCS2 *Parser::Parse_VStr(bool pathname)
{
    char *p;
    char temp3[128];
    char temp4[768];
    int l, d, vl;
    EXPRESS Express;
    int Terms;
    int Dim = 5;
    UCS2 *str;
    UCS2 *str2;
    UCS2 *New;

    Parse_Paren_Begin();

    vl = (int)Parse_Float();
    Parse_Comma();

    if(vl < 2)
        vl = 2;
    else if(vl > 5)
        vl = 5;
    Dim = vl;

    Terms = Parse_Unknown_Vector(Express);

    Parse_Comma();
    str = Parse_String(pathname);
    Parse_Comma();
    l = (int)Parse_Float();
    Parse_Comma();
    d = (int)Parse_Float();

    Parse_Paren_End();

    p = temp3;
    *(p++) = '%';
    if (l > 0)
    {
        sprintf(p, "%d", l);
        while (*p != '\0')
            p++;
    }
    else
    {
        if (l)
        {
            sprintf(p, "0%d", abs(l));
            while (*p != '\0')
                p++;
        }
    }

    if (d >= 0)
    {
        *(p++) = '.';
        sprintf(p, "%d", d);
        while (*(++p))
            ;
    }
    *(p++) = 'f';
    *p = '\0';

    sprintf(temp4, temp3, Express[X]);
    New = String_To_UCS2(temp4);       // add first component

    for(Terms = 1; Terms < Dim; Terms++)
    {
        New = UCS2_strcat(New, str);   // add separator
        sprintf(temp4, temp3, Express[Terms]);
        str2 = String_To_UCS2(temp4);
        New = UCS2_strcat(New, str2);  // add component
        POV_FREE(str2);
    }

    POV_FREE(str);

    return New;
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

UCS2 *Parser::Parse_Concat(bool pathname)
{
    UCS2 *str;
    UCS2 *New;

    Parse_Paren_Begin();

    New = Parse_String();

    EXPECT
        CASE(RIGHT_PAREN_TOKEN)
            UNGET
            EXIT
        END_CASE

        OTHERWISE
            UNGET
            Parse_Comma();
            str = Parse_String(pathname);
            New = UCS2_strcat(New, str);
            POV_FREE(str);
        END_CASE
    END_EXPECT

    Parse_Paren_End();

    return New;
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

UCS2 *Parser::Parse_Chr(bool /*pathname*/)
{
    UCS2 *New;
    int d;

    New = reinterpret_cast<UCS2 *>(POV_MALLOC(sizeof(UCS2) * 2, "temporary string"));

    d = (int)Parse_Float_Param();
    if((d < 0) || (d > 65535))
        Error("Value %d cannot be used in chr(...).", d);

    New[0] = d;
    New[1] = 0;

    return New;
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

#define PARSE_NOW_VAL_LENGTH 200

UCS2 *Parser::Parse_Datetime(bool pathname)
{
    char *FormatStr = NULL;
    bool CallFree;
    int vlen = 0;
    char val[PARSE_NOW_VAL_LENGTH + 1]; // Arbitrary size, usually a date format string is far less

    Parse_Paren_Begin();

    std::time_t timestamp = floor((Parse_Float() + (365*30+7)) * 24*60*60 + 0.5);
    Parse_Comma();
    EXPECT_ONE
        CASE(RIGHT_PAREN_TOKEN)
            UNGET
            CallFree = false;
            // we use GMT as some platforms (e.g. windows) have different ideas of what to print when handling '%z'.
            FormatStr = (char *)"%Y-%m-%d %H:%M:%SZ";
        END_CASE

        OTHERWISE
            UNGET
            CallFree = true;
            FormatStr = Parse_C_String(pathname);
            if (FormatStr[0] == '\0')
            {
                POV_FREE(FormatStr);
                Error("Empty format string.");
            }
            if (strlen(FormatStr) > PARSE_NOW_VAL_LENGTH)
            {
                POV_FREE(FormatStr);
                Error("Format string too long.");
            }
        END_CASE
    END_EXPECT

    Parse_Paren_End();

    // NB don't wrap only the call to strftime() in the try, because visual C++ will, in release mode,
    // optimize the try/catch away since it doesn't believe that the RTL can throw exceptions. since
    // the windows version of POV hooks the invalid parameter handler RTL callback and throws an exception
    // if it's called, they can.
    try
    {
        std::tm t = boost::posix_time::to_tm(boost::posix_time::from_time_t(timestamp));
        // TODO FIXME - we should either have this locale setting globally, or avoid it completely; in either case it shouldn't be *here*.
        setlocale(LC_TIME,""); // Get the local prefered format
        vlen = strftime(val, PARSE_NOW_VAL_LENGTH, FormatStr, &t);
    }
    catch (pov_base::Exception& e)
    {
        // the windows version of strftime calls the invalid parameter handler if
        // it gets a bad format string. this will in turn raise an exception of type
        // kParamErr. if the exception isn't that, allow normal exception processing
        // to continue, otherwise we issue a more useful error message.
        if ((e.codevalid() == false) || (e.code() != kParamErr))
            throw;
        vlen = 0;
    }
    if (vlen == PARSE_NOW_VAL_LENGTH) // on error: max for libc 4.4.1 & before
        vlen = 0; // return an empty string on error (content of val[] is undefined)
    val[vlen]='\0'; // whatever, that operation is now safe (and superflous except for error)

    if (CallFree)
    {
        POV_FREE(FormatStr);
    }

    if (vlen == 0)
        Error("Invalid formatting code in format string, or resulting string too long.");

    return String_To_UCS2(val);
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

UCS2 *Parser::Parse_Substr(bool pathname)
{
    UCS2 *str;
    UCS2 *New;
    int l, d;

    Parse_Paren_Begin();

    str = Parse_String(pathname);
    Parse_Comma();
    l = (int)Parse_Float();
    Parse_Comma();
    d = (int)Parse_Float();

    Parse_Paren_End();

    if(((l + d - 1) > UCS2_strlen(str)) || (l < 0) || (d < 0))
        Error("Illegal parameters in substr.");

    New = reinterpret_cast<UCS2 *>(POV_MALLOC(sizeof(UCS2) * (d + 1), "temporary string"));
    UCS2_strncpy(New, &(str[l - 1]), d);
    New[d] = 0;

    POV_FREE(str);

    return New;
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

UCS2 *Parser::Parse_Strupr(bool pathname)
{
    UCS2 *New;

    Parse_Paren_Begin();

    New = Parse_String(pathname);
    UCS2_strupr(New);

    Parse_Paren_End();

    return New;
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

UCS2 *Parser::Parse_Strlwr(bool pathname)
{
    UCS2 *New;

    Parse_Paren_Begin();

    New = Parse_String(pathname);
    UCS2_strlwr(New);

    Parse_Paren_End();

    return New;
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

UCS2 *Parser::String_To_UCS2(const char *str)
{
    UCS2 *char_string = NULL;
    UCS2 *char_array = NULL;
    int char_array_size = 0;
    int utf8arraysize = 0;
    unsigned char *utf8array = NULL;
    int index_in = 0;
    int index_out = 0;
    char *dummy_ptr = NULL;
    int i = 0;

    if(strlen(str) == 0)
    {
        char_string = reinterpret_cast<UCS2 *>(POV_MALLOC(sizeof(UCS2), "UCS2 String"));
        char_string[0] = 0;

        return char_string;
    }

    switch(sceneData->stringEncoding)
    {
        case kStringEncoding_ASCII:
            char_array_size = (int)strlen(str);
            char_array = reinterpret_cast<UCS2 *>(POV_MALLOC(char_array_size * sizeof(UCS2), "Character Array"));
            for(i = 0; i < char_array_size; i++)
            {
                if(sceneData->EffectiveLanguageVersion() < 350)
                    char_array[i] = (unsigned char)(str[i]);
                else
                {
                    char_array[i] = str[i] & 0x007F;
                    if(char_array[i] != str[i])
                    {
                        char_array[i] = ' ';
                        PossibleError("Non-ASCII character has been replaced by space character.");
                    }
                }
            }
            break;
        case kStringEncoding_UTF8:
            char_array = Convert_UTF8_To_UCS2(reinterpret_cast<const unsigned char *>(str), &char_array_size);
            break;
        case kStringEncoding_System:
            char_array = POV_CONVERT_TEXT_TO_UCS2(reinterpret_cast<const unsigned char *>(str), &char_array_size);
            if(char_array == NULL)
                Error("Cannot convert system specific text format to Unicode.");
            break;
        default:
            Error("Unsupported text encoding format.");
            break;
    }

    if(char_array == NULL)
        Error("Cannot convert text to UCS2 format.");

    char_string = reinterpret_cast<UCS2 *>(POV_MALLOC((char_array_size + 1) * sizeof(UCS2), "UCS2 String"));
    for(index_in = 0, index_out = 0; index_in < char_array_size; index_in++, index_out++)
        char_string[index_out] = char_array[index_in];

    char_string[index_out] = 0;
    index_out++;

    if(char_array != NULL)
        POV_FREE(char_array);

    return char_string;
}


/*****************************************************************************/

UCS2 *Parser::String_Literal_To_UCS2(const char *str, bool pathname)
{
    UCS2 *char_string = NULL;
    UCS2 *char_array = NULL;
    int char_array_size = 0;
    int utf8arraysize = 0;
    unsigned char *utf8array = NULL;
    int index_in = 0;
    int index_out = 0;
    char buffer[8];
    char *dummy_ptr = NULL;
    int i = 0;

    if(strlen(str) == 0)
    {
        char_string = reinterpret_cast<UCS2 *>(POV_MALLOC(sizeof(UCS2), "UCS2 String"));
        char_string[0] = 0;

        return char_string;
    }

    switch(sceneData->stringEncoding)
    {
        case kStringEncoding_ASCII:
            char_array_size = (int)strlen(str);
            char_array = reinterpret_cast<UCS2 *>(POV_MALLOC(char_array_size * sizeof(UCS2), "Character Array"));
            for(i = 0; i < char_array_size; i++)
            {
                if(sceneData->EffectiveLanguageVersion() < 350)
                    char_array[i] = (unsigned char)(str[i]);
                else
                {
                    char_array[i] = str[i] & 0x007F;
                    if(char_array[i] != str[i])
                    {
                        char_array[i] = ' ';
                        PossibleError("Non-ASCII character has been replaced by space character.");
                    }
                }
            }
            break;
        case kStringEncoding_UTF8:
            char_array = Convert_UTF8_To_UCS2(reinterpret_cast<const unsigned char *>(str), &char_array_size);
            break;
        case kStringEncoding_System:
            char_array = POV_CONVERT_TEXT_TO_UCS2(reinterpret_cast<const unsigned char *>(str), &char_array_size);
            if(char_array == NULL)
                Error("Cannot convert system specific text format to Unicode.");
            break;
        default:
            Error("Unsupported text encoding format.");
            break;
    }

    if(char_array == NULL)
        Error("Cannot convert text to UCS2 format.");

    char_string = reinterpret_cast<UCS2 *>(POV_MALLOC((char_array_size + 1) * sizeof(UCS2), "UCS2 String"));
    for(index_in = 0, index_out = 0; index_in < char_array_size; index_in++, index_out++)
    {
        if((char_array[index_in] == '\\') && (sceneData->EffectiveLanguageVersion() >= 380 || !pathname))
        {
            // Historically, escape sequences were ignored when parsing for a filename.
            // As of POV-Ray v3.8, this has been changed.

#if POV_BACKSLASH_IS_PATH_SEPARATOR
            if (pathname)
            {
                Warning("Backslash encountered while parsing for a filename."
                        " As of POV-Ray v3.8, this is interpreted as an escape sequence just like in any other string literal."
                        " If this is supposed to be a path separator, use a forward slash instead.");
            }
#endif

            index_in++;

            switch(char_array[index_in])
            {
                case 'a':
                    char_string[index_out] = 0x07;
                    break;
                case 'b':
                    char_string[index_out] = 0x08;
                    break;
                case 'f':
                    char_string[index_out] = 0x0c;
                    break;
                case 'n':
                    char_string[index_out] = 0x0a;
                    break;
                case 'r':
                    char_string[index_out] = 0x0d;
                    break;
                case 't':
                    char_string[index_out] = 0x09;
                    break;
                case 'v':
                    char_string[index_out] = 0x0b;
                    break;
                case '\0':
                    // [CLi] shouldn't happen, as having a backslash as the last character of a string literal would invalidate the string terminator
                    Error("Unexpected end of escape sequence in text string.");
                    break;
                case '\'':
                case '\"':
                case '\\':
                    char_string[index_out] = char_array[index_in];
                    break;
                case 'u':
                    if(index_in + 4 >= char_array_size)
                        Error("Unexpected end of escape sequence in text string.");

                    buffer[0] = char_array[++index_in];
                    buffer[1] = char_array[++index_in];
                    buffer[2] = char_array[++index_in];
                    buffer[3] = char_array[++index_in];
                    buffer[4] = 0;

                    char_string[index_out] = (UCS2)std::strtoul(buffer, &dummy_ptr, 16);
                    break;
                default:
                    char_string[index_out] = char_array[index_in];
                    POV_FREE(char_array);
                    char_array = NULL;
                    Error( "Illegal escape sequence in string." );
                    break;
            }
        }
        else
        {
            if ((char_array[index_in] == '\\') && pathname)
            {
                // Historically, escape sequences were ignored when parsing for a filename.
                // As of POV-Ray 3.8, this has been changed.

#if POV_BACKSLASH_IS_PATH_SEPARATOR
                Warning("Backslash encountered while parsing for a filename."
                        " In legacy (pre-3.8) scenes, this is NOT interpreted as the start of an escape sequence."
                        " However, for future compatibility it is recommended to use a forward slash as path separator instead.");
#else
                Warning("Backslash encountered while parsing for a filename."
                        " In legacy (pre-3.8) scenes, this is NOT interpreted as the start of an escape sequence.");
#endif
            }

            char_string[index_out] = char_array[index_in];
        }
    }

    char_string[index_out] = 0;
    index_out++;

    char_string = reinterpret_cast<UCS2 *>(POV_REALLOC(char_string, index_out * sizeof(UCS2), "UCS2 String"));

    if(char_array != NULL)
        POV_FREE(char_array);

    return char_string;
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

char *Parser::UCS2_To_String(const UCS2 *str)
{
    char *str_out;
    char *strp;

    str_out = reinterpret_cast<char *>(POV_MALLOC(UCS2_strlen(str)+1, "C String"));

    for(strp = str_out; *str != 0; str++, strp++)
    {
        if((*str > 127) && (sceneData->EffectiveLanguageVersion() >= 350))
            *strp = ' ';
        else
            *strp = (char)(*str);
    }

    *strp = 0;

    return str_out;
}


/*****************************************************************************
*
* FUNCTION
*
*   Convert_UTF8_To_UCS2
*
* INPUT
*
*   Array of bytes, length of this sequence
*
* OUTPUT
*
*   Size of the array of UCS2s returned
*
* RETURNS
*
*   Array of UCS2s (allocated with POV_MALLOC)
*
* AUTHOR
*
* DESCRIPTION
*
*   Converts UTF8 to UCS2 characters, however all surrogates are dropped.
*
* CHANGES
*
*   -
*
******************************************************************************/

UCS2 *Parser::Convert_UTF8_To_UCS2(const unsigned char *text_array, int *char_array_size)
{
    POV_PARSER_ASSERT(text_array);
    POV_PARSER_ASSERT(char_array_size);

    UCS2String s = UTF8toUCS2String(UTF8String(reinterpret_cast<const char*>(text_array)));
    UCS2String::size_type len = s.length();
    *char_array_size = len;

    if (len == 0)
        return NULL;

    size_t size = (len+1)*sizeof(UCS2);

    UCS2 *char_array = reinterpret_cast<UCS2 *>(POV_MALLOC(size, "Character Array"));
    if(char_array == NULL)
        throw POV_EXCEPTION_CODE(kOutOfMemoryErr);

    memcpy(char_array, s.c_str(), size);

    return char_array;
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

UCS2 *Parser::UCS2_strcat(UCS2 *s1, const UCS2 *s2)
{
    int l1, l2;

    l1 = UCS2_strlen(s1);
    l2 = UCS2_strlen(s2);

    s1 = reinterpret_cast<UCS2 *>(POV_REALLOC(s1, sizeof(UCS2) * (l1 + l2 + 1), "UCS2 String"));

    UCS2_strcpy(&s1[l1], s2);

    return s1;
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

int Parser::UCS2_strlen(const UCS2 *str)
{
    int i;

    for(i = 0; *str != 0; str++, i++) { }

    return i;
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

int Parser::UCS2_strcmp(const UCS2 *s1, const UCS2 *s2)
{
    UCS2 t1, t2;

    while((t1 = *s1++) == (t2 = *s2++))
    {
        if(t1 == 0)
            return 0;
    }

    return (t1 - t2);
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

void Parser::UCS2_strcpy(UCS2 *s1, const UCS2 *s2)
{
    for(; *s2 != 0; s1++, s2++)
        *s1 = *s2;

    *s1 = 0;
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

void Parser::UCS2_strncpy(UCS2 *s1, const UCS2 *s2, int n)
{
    for(; (*s2 != 0) && (n > 0); s1++, s2++, n--)
        *s1 = *s2;

    *s1 = 0;
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

void Parser::UCS2_strupr(UCS2 *str)
{
    bool err = false;

    while(true)
    {
        if (((int) *str < 0) || (*str > 127))
            err = true;
        else if(*str == 0)
            break;

        *str = toupper(*str);
        str++;
    }

    if(err == true)
        Warning("Non-ASCII character in string, strupr may not work as expected.");
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

void Parser::UCS2_strlwr(UCS2 *str)
{
    bool err = false;

    while(true)
    {
        if (((int) *str < 0) || (*str > 127))
            err = true;
        else if(*str == 0)
            break;

        *str = tolower(*str);
        str++;
    }

    if(err == true)
        Warning("Non-ASCII character in string, strlwr may not work as expected.");
}

UCS2 *Parser::UCS2_strdup(const UCS2 *s)
{
    UCS2 *New;

    New=reinterpret_cast<UCS2 *>(POV_MALLOC((UCS2_strlen(s)+1) * sizeof(UCS2), UCS2toASCIIString(s).c_str()));
    UCS2_strcpy(New,s);
    return (New);
}

}
