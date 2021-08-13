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
/// Copyright 1991-2021 Persistence of Vision Raytracer Pty. Ltd.
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
//------------------------------------------------------------------------------
// SPDX-License-Identifier: AGPL-3.0-or-later
//******************************************************************************

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "parser/parser.h"

// C++ variants of standard C header files
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// Standard C++ header files
#include <chrono>

// POV-Ray header files (base module)
#include "base/threadsafe.h"

// POV-Ray header files (core module)
#include "core/scene/scenedata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

using namespace pov;

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
    UCS2 *New = nullptr;
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


//****************************************************************************


std::string Parser::Parse_ASCIIString(bool pathname, bool require)
{
    UCS2 *cstr = Parse_String(pathname, require);
    std::string ret(UCS2toASCIIString(cstr));
    POV_FREE(cstr);
    return ret;
}


//****************************************************************************


UCS2String Parser::Parse_UCS2String(bool pathname, bool require)
{
    UCS2 *cstr = Parse_String(pathname, require);
    UCS2String ret(cstr);
    POV_FREE(cstr);
    return ret;
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

static bool FindDatetimeFormatSpecifier(const char** list, const char* first, const char* last)
{
    POV_PARSER_ASSERT((*first != '\0') && (*last != '\0'));

    if (first == last)
        return (std::strchr(list[0], *first) != nullptr);

    POV_PARSER_ASSERT(last - first == 1);

    for (int i = 1; list[i] != nullptr; ++i)
        if (list[i][0] == *first)
            return (std::strchr(list[i] + 1, *last) != nullptr);

    return false;
}

UCS2 *Parser::Parse_Datetime(bool pathname)
{
    // Enum representing a type of placeholder offense.
    enum OffenseType { kNoOffense, kLocaleSensitive, kTimezoneSensitive, kLegacyNonPortable, kNonPortable, kIncomplete };
    // Structure representing a placeholder and its offense.
    struct Offense
    {
        // Offense, in ascending order of severity.
        OffenseType type;
        // Start of offending placeholder.
        std::string field; // Offending placeholder
        // Default Constructor.
        Offense() : type(kNoOffense), field() {}
        // Constructor.
        Offense(OffenseType t, const char* first) : type(t), field(first) {}
        // Constructor.
        Offense(OffenseType t, const char* first, const char* last) : type(t), field(first, (last-first+1)) {}
        // Overwrite data, provided other offense is more severe.
        Offense& operator|=(const Offense& o) { if (o.type > type) { type = o.type; field = o.field; } return *this; }
        // Convert to boolean
        explicit operator bool() const { return type != kNoOffense; }
    };

    // Known prefixes of 2-character conversion specifiers.
    // NB: `#` is a Microsoft extension.
    static const char* knownPrefixes = "EO#";

    // Conversion specifiers _guaranteed_ to be supported across _all_ platforms by specific versions of POV-Ray,
    // as a nullptr-terminated list of strings.
    // First string is a plain list of valid single-character specifiers; all other strings start with the respective
    // prefix, followed by the corresponding list of valid secondary characters.
    // NB: These lists should exactly match the set of conversion specifiers defined by the minimum C++ standard
    // required to compile the respective version of POV-Ray.

    // POV-Ray v3.8, which requires C++11.
    static const char* portableFieldsCurrent[] {
        "aAbBcCdDeFgGhHIjmMnprRStTuUVwWxXyYzZ%",
        "EcCxXyY",
        "OdeHImMSuUVwWy",
        nullptr };
    // POV-Ray v3.7, which required only C++03.
    static const char* portableFieldsLegacy370[] {
        "aAbBcdHIjmMpSUwWxXyYZ%",
        nullptr };

    // Conversion specifiers sensitive to time zone information (same format as above).
    static const char* timezoneFields[] { "zZ", nullptr };

    // Locale-independent conversion specifiers (same format as above).
    // NB: The 2-character conversion specifiers defined in C/C++ are inherently locale-specific.
    // NB: `z` qualifies in that its format is locale-independent, even though the value is not.
    static const char* localeAgnosticFields[] { "CdDeFgGHIjmMnRStTuUVwWyYz%", nullptr };

    // Fields that might evaluate to empty strings (same format as above).
    // NB: `P` is a GNU extension.
    static const char* potentiallyEmptyFields[] { "pPzZ", nullptr };

    static constexpr int kMaxResultSize = 200; // Max number of chars in result, _including_ terminating NUL character.

    char *FormatStr = nullptr;
    bool CallFree = false;

    Offense offense;
    void(*convertTime)(std::tm*, const std::time_t* time);
    int vlen = 0;
    char val[kMaxResultSize]; // Arbitrary size, usually a date format string is far less

    Parse_Paren_Begin();

    if (localTime)
    {
        convertTime = &pov_base::SafeLocaltime;
        FormatStr = "%Y-%m-%d %H:%M:%S%z";
    }
    else
    {
        convertTime = &pov_base::SafeGmtime;
        FormatStr = "%Y-%m-%d %H:%M:%SZ";
    }

    const char** portableFieldsLegacy; // Fields reliably supported by whatever POV-Ray version is specified via `#version`.
    if (sceneData->EffectiveLanguageVersion() < 380)
        portableFieldsLegacy = portableFieldsLegacy370;
    else
        portableFieldsLegacy = nullptr; // skip legacy portability check

    FractionalDays daysSinceY2k(Parse_Float());
    Parse_Comma();
    EXPECT_ONE
        CASE(RIGHT_PAREN_TOKEN)
            UNGET
        END_CASE

        OTHERWISE
            UNGET
            CallFree = true;
            FormatStr = Parse_C_String(pathname);
            if (FormatStr[0] == '\0')
            {
                POV_FREE(FormatStr);
                Error("Empty 'datetime' format string.");
            }
            for (const char* pc = FormatStr; *pc != '\0'; ++pc)
            {
                // Skip over any non-placeholders.
                if (*pc != '%')
                    continue;

                // Keep track of start of placeholder, so we can more easily report it
                // in case it turns out to be offensive.
                const char* po = pc;

                if (*(++pc) == '\0')
                {
                    offense |= Offense(kIncomplete, po);
                    break;
                }

                // Keep track of first "payload" character of placeholder.
                const char* pc1 = pc;

                // Check if we appear to have a two-character placeholder, and if so,
                // advance to the next character.
                if (std::strchr(knownPrefixes, *pc) != nullptr)
                {
                    if (*(++pc) == '\0')
                    {
                        offense |= Offense(kIncomplete, po);
                        break;
                    }
                }

                // NB: The next character should be the last one comprising the placeholder.

                if (offense.type >= kNonPortable)
                    continue; // We won't find any worse issue with this placeholder; next please.

                // Check if placeholder is portable across all platforms for this version of POV-Ray.
                if (!FindDatetimeFormatSpecifier(portableFieldsCurrent, pc1, pc))
                    offense |= Offense(kNonPortable, po, pc);

                if (offense.type >= kLegacyNonPortable)
                    continue; // We won't find any worse issue with this placeholder; next please.

                // Check if placeholder is portable across all platforms for the version the scene was designed for.
                if ((portableFieldsLegacy != nullptr) && !FindDatetimeFormatSpecifier(portableFieldsLegacy, pc1, pc))
                    offense |= Offense(kLegacyNonPortable, po, pc);

                if (offense.type >= kTimezoneSensitive)
                    continue; // We won't find any worse issue with this placeholder; next please.

                // Check if placeholder requires time zone information that we might not have.
                if (!localTime && FindDatetimeFormatSpecifier(timezoneFields, pc1, pc))
                    offense |= Offense(kTimezoneSensitive, po, pc);

                if (offense.type >= kLocaleSensitive)
                    continue; // We won't find any worse issue with this placeholder; next please.

                // Check if placeholder may have different format depending on locale.
                if (!FindDatetimeFormatSpecifier(localeAgnosticFields, pc1, pc))
                    offense |= Offense(kLocaleSensitive, po, pc);
            }
            switch (offense.type)
            {
                case kIncomplete:
                    Error("Incomplete 'datetime' format placeholder ('%s').",
                          offense.field.c_str());
                    break;

                case kNonPortable:
                    WarningOnce(kWarningGeneral, HERE,
                                "Scene uses 'datetime' format placeholder(s) that may not be supported on all "
                                "platforms ('%s').",
                                offense.field.c_str());
                    break;

                case kLegacyNonPortable:
                    WarningOnce(kWarningGeneral, HERE,
                                "Legacy scene uses 'datetime' format placeholder(s) that POV-Ray v%s "
                                "did not support on all platforms ('%s').",
                                sceneData->EffectiveLanguageVersion().str().c_str(),
                                offense.field.c_str());
                    break;

                case kTimezoneSensitive:
                    WarningOnce(kWarningGeneral, HERE,
                                "Scene uses 'datetime' format placeholder(s) relating to time zone information "
                                "('%s') without 'local_time' global setting turned on, implying UTC mode of "
                                "operation. On some systems, this may cause the placeholder(s) to not work as "
                                "expected.",
                                offense.field.c_str());
                    break;

                case kLocaleSensitive:
                    WarningOnce(kWarningGeneral, HERE,
                                "Scene uses 'datetime' format placeholder(s) that may produce "
                                "different results depending on system locale settings ('%s').",
                                offense.field.c_str());
                    break;

                case kNoOffense:
                    break;
            }
        END_CASE
    END_EXPECT

    Parse_Paren_End();

    // NB don't wrap only the call to strftime() in the try, because Visual C++ will, in release mode,
    // optimize the try/catch away since it doesn't believe that the RTL can throw exceptions. Since
    // the windows version of POV hooks the invalid parameter handler RTL callback and throws an exception
    // if it's called, they can.
    try
    {
        auto timeSinceY2k = std::chrono::duration_cast<std::chrono::system_clock::duration>(daysSinceY2k);
        auto timeToPrint = mY2k + timeSinceY2k;
        std::time_t timestamp = std::chrono::system_clock::to_time_t(timeToPrint);

        std::tm t;
        convertTime(&t, &timestamp);

        vlen = std::strftime(val, kMaxResultSize, FormatStr, &t);

        if (vlen == 0)
        {
            // As per the the C/C++ standard, a `strftime` return value of zero _per se_ may not
            // necessarily indicate an error: The resulting string could simply be empty due to
            // the format consisting of only conversion specifiers that happen to evaluate to
            // empty strings for the current locale (e.g. `%p`).

            // We employ some strong logics as well as weaker heuristics to figure out whether
            // this is definitely an error; in that case, we report it to code further downstream
            // by setting the length value to the buffer size (which also happens to be how
            // libc 4.4.1 and earlier reported overflow), which can never happen in case of a
            // success.

            // TODO - we could avoid this whole mess by simply injecting some arbitrary character
            // at the start or end of the format string, and cut it away again after the
            // conversion. That way, even an "empty" string would give us at least 1 character.

            if (val[0] != '\0')
            {
                // In absence of an non-error, the result string would have to be empty,
                // but that's not what we're seeing here, confirming that this is indeed
                // unambiguously an error.
                vlen = kMaxResultSize;
            }
            else
            {
                // Check if the result string could conceivably be empty.
                for (const char* pc = FormatStr; *pc != '\0'; ++pc)
                {
                    if (*pc != '%')
                    {
                        // Any non-placeholder character would have to be copied verbatim,
                        // and should therefore have resulted in a non-empty string.
                        vlen = kMaxResultSize;
                        break;
                    }
                    ++pc;
                    const char* pc1 = pc;
                    if (std::strchr(knownPrefixes, *pc) != nullptr)
                        ++pc;
                    if (!FindDatetimeFormatSpecifier(potentiallyEmptyFields, pc1, pc))
                    {
                        // Placeholder is not in the list of those we expect to be replaced with empty strings.
                        vlen = kMaxResultSize;
                        break;
                    }
                }
            }
        }
    }
    catch (pov_base::Exception& e)
    {
        // The windows version of `strftime` calls the invalid parameter handler if
        // it gets a bad format string. This will in turn raise an exception of type
        // `kParamErr`. If the exception isn't that, allow normal exception processing
        // to continue, otherwise we issue a more useful error message.
        if ((e.codevalid() == false) || (e.code() != kParamErr))
            throw;
        Error("Invalid 'datetime' format placeholder");
    }
    if (CallFree)
        POV_FREE(FormatStr);

    if (vlen == kMaxResultSize)
        // Unambiguous error
        Error("Invalid 'datetime' format placeholder, or resulting string too long (>%i characters).",
              kMaxResultSize - 1);

    if (vlen == 0)
    {
        // Maybe an error, maybe just an empty result string
        PossibleError("'datetime' result string empty; if this is unexpected, check for "
                      "invalid 'datetime' format placeholders, or whether the resulting "
                      "string would have been too long (>%i characters).",
                      kMaxResultSize - 1);
    }

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
    UCS2 *char_string = nullptr;
    UCS2 *char_array = nullptr;
    int char_array_size = 0;
    int utf8arraysize = 0;
    unsigned char *utf8array = nullptr;
    int index_in = 0;
    int index_out = 0;
    char *dummy_ptr = nullptr;
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
            if (char_array == nullptr)
                Error("Cannot convert system specific text format to Unicode.");
            break;
        default:
            Error("Unsupported text encoding format.");
            break;
    }

    if (char_array == nullptr)
        Error("Cannot convert text to UCS2 format.");

    char_string = reinterpret_cast<UCS2 *>(POV_MALLOC((char_array_size + 1) * sizeof(UCS2), "UCS2 String"));
    for(index_in = 0, index_out = 0; index_in < char_array_size; index_in++, index_out++)
        char_string[index_out] = char_array[index_in];

    char_string[index_out] = 0;
    index_out++;

    if (char_array != nullptr)
        POV_FREE(char_array);

    return char_string;
}


/*****************************************************************************/

UCS2 *Parser::String_Literal_To_UCS2(const char *str, bool pathname)
{
    UCS2 *char_string = nullptr;
    UCS2 *char_array = nullptr;
    int char_array_size = 0;
    int utf8arraysize = 0;
    unsigned char *utf8array = nullptr;
    int index_in = 0;
    int index_out = 0;
    char buffer[8];
    char *dummy_ptr = nullptr;
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
            if (char_array == nullptr)
                Error("Cannot convert system specific text format to Unicode.");
            break;
        default:
            Error("Unsupported text encoding format.");
            break;
    }

    if (char_array == nullptr)
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
                    char_array = nullptr;
                    Error( "Illegal escape sequence in string." );
                    break;
            }
        }
        else
        {
            if ((char_array[index_in] == '\\') && pathname)
            {
                // Historically, escape sequences were ignored when parsing for a filename.
                // As of POV-Ray v3.8, this has been changed.

#if POV_BACKSLASH_IS_PATH_SEPARATOR
                Warning("Backslash encountered while parsing for a filename."
                        " In legacy (pre-v3.8) scenes, this is NOT interpreted as the start of an escape sequence."
                        " However, for future compatibility it is recommended to use a forward slash as path separator instead.");
#else
                Warning("Backslash encountered while parsing for a filename."
                        " In legacy (pre-v3.8) scenes, this is NOT interpreted as the start of an escape sequence.");
#endif
            }

            char_string[index_out] = char_array[index_in];
        }
    }

    char_string[index_out] = 0;
    index_out++;

    char_string = reinterpret_cast<UCS2 *>(POV_REALLOC(char_string, index_out * sizeof(UCS2), "UCS2 String"));

    if (char_array != nullptr)
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
        return nullptr;

    size_t size = (len+1)*sizeof(UCS2);

    UCS2 *char_array = reinterpret_cast<UCS2 *>(POV_MALLOC(size, "Character Array"));
    if (char_array == nullptr)
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
