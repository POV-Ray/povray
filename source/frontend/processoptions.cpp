//******************************************************************************
///
/// @file frontend/processoptions.cpp
///
/// This module contains the C++ interface for option processing.
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
#include "frontend/processoptions.h"

// C++ variants of C standard header files
#include <cstdarg>
#include <cstdio>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/platformbase.h"
#include "base/povassert.h"
#include "base/stringutilities.h"
#include "base/textstream.h"

// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmsid.h"

// POV-Ray header files (frontend module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

ProcessOptions::ProcessOptions(INI_Parser_Table *pit, Cmd_Parser_Table *pct)
{
    parse_ini_table = pit;
    parse_cmd_table = pct;
}

ProcessOptions::~ProcessOptions()
{
}

int ProcessOptions::ParseFile(const char *filespec, POVMSObjectPtr obj)
{
    ITextStream *file = nullptr;
    const char *currentsection = nullptr;
    char *sectionname = nullptr;
    char *filename = nullptr;
    int err = kNoErr;
    POVMSObjectPtr section = nullptr;
    int currentline = 1;

    // split the INI files specification into filename and optional section name
    err = Parse_INI_Specification(filespec, filename, sectionname);
    if(err == kNoErr)
    {
        file = OpenFileForRead(filename, obj);
        if (file == nullptr)
        {
            // all errors here are non-fatal, the calling code has to decide if an error is fatal
            ParseError("Cannot open INI file '%s'.", filename);
            err= kCannotOpenFileErr;
        }
    }

    if(err == kNoErr)
    {
        int token = 0;

        // apply options prior to any named section
        section = obj;

        // read the whole file to the end
        while((file->eof() == false) && (err == kNoErr))
        {
            currentline += Parse_INI_Skip_Space(file, true);

            token = file->getchar();

            // INI file section name
            if(token == '[')
            {
                // free old section name, if any
                if (currentsection != nullptr)
                    delete[] currentsection;

                // read until the section name end marker
                currentsection = Parse_INI_String(file, ']');

                // if the user specified a section name, compare the two and enable reading
                if ((sectionname != nullptr) && (currentsection != nullptr))
                {
                    if(pov_stricmp(currentsection, sectionname) == 0)
                        section = obj; // named section matches specified section name, apply options
                    else
                        section = nullptr; // named section does not match specified section name, ignore options
                }
                // if there was no user specified section name, ignore all named sections
                else
                    section = nullptr; // no section name was specified, ignore options in named section
            }
            // skip lines that do not belong to the desired sections
            else if (section == nullptr)
            {
                currentline += Parse_INI_Skip_Line(file);
            }
            // fully process lines in desired sections
            else
            {
                switch(token)
                {
                    // end of file
                    case EOF:
                        break;
                    // quoted string
                    case '\"':
                    case '\'':
                        err = kFalseErr;
                        break;
                    // POV-Ray-style INI file with command-line switch
                    case '+':
                    case '-':
#if (POV_SLASH_IS_SWITCH_CHARACTER)
                    // POV-Ray-style INI file with system specific command-line switch on some systems (e.g. Windos)
                    case '/':
#endif
                        err = Parse_INI_Switch(file, token, section);
                        break;
                    // INI file comment
                    case ';':
                    case '#':
                        currentline += Parse_INI_Skip_Line(file);
                        break;
                    // INI file option
                    default:
                        if(isalnum(token) || (token == '_'))
                        {
                            file->ungetchar(token);
                            ITextStream::FilePos backtrackpos = file->tellg();
                            err = Parse_INI_Option(file, section);
                            // only one option is allowed per line
                            if(err == kNoErr)
                                currentline += Parse_INI_Skip_Line(file);
                            else if(err == kParseErr)
                            {
                                file->seekg(backtrackpos);
                                err = kFalseErr;
                            }
                        }
                        else
                            err = kFalseErr;
                        break;
                }

                // if nothing else was appropriate, assume it is some other kind of string requiring special attention
                if(err == kFalseErr)
                {
                    char *plainstring = nullptr;

                    if((token != '\"') && (token != '\''))
                    {
                        // if there were no quotes, just read up to the next space or newline
                        plainstring = Parse_INI_String(file, -2, true);

                        // see if it is probably a standard INI file entry which wasn't recognised
                        if (plainstring != nullptr)
                        {
                            if (strchr(plainstring, '=') != nullptr)
                                err = kParseErr;
                        }
                    }
                    else
                        plainstring = Parse_INI_String(file, token, true);

                    if(err == kFalseErr)
                        err = ProcessUnknownString(plainstring, obj);

                    if (plainstring != nullptr)
                        delete[] plainstring;
                }
            }
        }

        // all errors here are non-fatal, the calling code has to decide if an error is fatal
        if(err != kNoErr)
        {
            if (currentsection != nullptr)
            {
                ParseErrorAt(file,
                             "Cannot continue to process INI file '%s' due to a parse error in line %d section '%s'.\n"
                             "This is not a valid INI file. Check the file for syntax errors, correct them, and try again!\n"
                             "Valid options in INI files are explained in detail in the reference part of the documentation.",
                             filename, currentline, currentsection);
            }
            else
            {
                ParseErrorAt(file,
                             "Cannot continue to process INI file '%s' due to a parse error in line %d.\n"
                             "This is not a valid INI file. Check the file for syntax errors, correct them, and try again!\n"
                             "Valid options in INI files are explained in detail in the reference part of the documentation.",
                             filename, currentline);
            }
        }

        if (currentsection != nullptr)
            delete[] currentsection;
    }

    if (filename != nullptr)
        delete[] filename;

    if (sectionname != nullptr)
        delete[] sectionname;

    if (file != nullptr)
        delete file;

    return err;
}

int ProcessOptions::ParseString(const char *commandline, POVMSObjectPtr obj, bool singleswitch)
{
    int err = kNoErr;

    // read the whole command-line to the end
    while((*commandline != 0) && (err == kNoErr))
    {
        if(singleswitch == false) // see if quotes had been stripped outside POV-Ray
            Parse_CL_Skip_Space(commandline);

        switch(*commandline)
        {
            // end of command-line
            case 0:
                break;
            // quoted string
            case '\"':
            case '\'':
                err = kFalseErr;
                break;
            // switch
            case '+':
            case '-':
#if (POV_SLASH_IS_SWITCH_CHARACTER)
            // POV-Ray-style INI file with system specific command-line switch on some systems (e.g. Windos)
            case '/':
#endif
                commandline++;
                err = Parse_CL_Switch(commandline, *(commandline - 1), obj, singleswitch);
                break;
            // INI file style option
            default:
                if(isalnum(*commandline) || (*commandline == '_'))
                {
                    const char *cltemp = commandline;
                    err = Parse_CL_Option(commandline, obj, singleswitch);
                    if(err == kParseErr)
                    {
                        commandline = cltemp;
                        err = kFalseErr;
                    }
                }
                else
                    err = kFalseErr;
                break;
        }

        // if nothing else was appropriate, assume it is some other kind of string requiring special attention
        if(err == kFalseErr)
        {
            int chr = *commandline;
            char *plainstring = nullptr;

            if((chr == '\"') || (chr == '\''))
            {
                commandline++;

                plainstring = Parse_CL_String(commandline, chr);
            }
            // if there were no quotes, just read up to the next space or newline
            else if(singleswitch == false) // see if quotes had been stripped outside POV-Ray
            {
                const char *oldcmdline = commandline;

                plainstring = Parse_CL_String(commandline, -2);

                // check if it is probably a standard INI file entry which wasn't recognised
                if ((plainstring != nullptr) && (strchr(plainstring, '=') != nullptr))
                {
                    err = kParseErr;
                    commandline = oldcmdline; // so it will be printed
                }
            }
            else
                plainstring = Parse_CL_String(commandline, 0);

            if(err == kFalseErr)
                err = ProcessUnknownString(plainstring, obj);

            if (plainstring != nullptr)
                delete[] plainstring;
        }
    }

    // all errors here are non-fatal, the calling code has to decide if an error is fatal
    if(err != kNoErr)
    {
        if(*commandline != 0)
        {
            ParseError("Cannot process command-line at '%s' due to a parse error.\n"
                       "This is not a valid command-line. Check the command-line for syntax errors, correct them, and try again.\n"
                       "Valid command-line switches are explained in detail in the reference part of the documentation.",
                       commandline);
        }
        else
        {
            ParseError("Cannot process command-line due to a parse error.\n"
                       "This is not a valid command-line. Check the command-line for syntax errors, correct them, and try again.\n"
                       "Valid command-line switches are explained in detail in the reference part of the documentation.");
        }
    }

    return err;
}

int ProcessOptions::WriteFile(OTextStream *ini_file, POVMSObjectPtr obj)
{
    struct INI_Parser_Table *table = parse_ini_table;

    // find the keyword
    while (table->keyword != nullptr)
    {
        if((table->flags & kINIOptFlag_SuppressWrite) == 0)
            Output_INI_Option(table, obj, ini_file);
        table++;
    }

    return kNoErr;
}

int ProcessOptions::WriteFile(const char *filename, POVMSObjectPtr obj)
{
    OTextStream *ini_file;
    int err = kNoErr;

    if(!pov_base::PlatformBase::GetInstance().AllowLocalFileAccess (SysToUCS2String(filename), POV_File_Text_INI, true))
        return kCannotOpenFileErr;

    ini_file = OpenFileForWrite(filename, obj);
    if (ini_file == nullptr)
        return kCannotOpenFileErr;
    err = WriteFile (ini_file, obj);
    delete ini_file;

    return err;
}

bool ProcessOptions::IsTrue(const char *value)
{
    return (Matches("on",value)  || Matches("true",value) ||
            Matches("yes",value) || Matches("1",value));
}

bool ProcessOptions::IsFalse(const char *value)
{
    return (Matches("off",value)  || Matches("false",value) ||
            Matches("no",value)   || Matches("0",value));
}

bool ProcessOptions::Matches(const char *v1, const char *v2)
{
   int i = 0;
   int ans = 1;

   while((ans) && (v1[i] != 0) && (v2[i] != 0))
   {
      ans = ans && (int)(v1[i] == tolower(v2[i]));
      i++;
   }

   return (ans != 0);
}

int ProcessOptions::POVMSAttr_SetUTF8String(POVMSAttributePtr attr, POVMSType type, const char *s)
{
    UCS2String ustr = UTF8toUCS2String(UTF8String(s));
    size_t len = ustr.length();
    return POVMSAttr_Set(attr, type, (void *)ustr.c_str(), (int(len) * 2) + 2);
}

int ProcessOptions::POVMSAttr_GetUTF8String(POVMSAttributePtr attr, POVMSType type, UTF8String& s)
{
    int ulen = 0;
    int err = POVMSAttr_Size(attr, &ulen);
    UCS2 *ustr = new UCS2[ulen / 2];

    if(err == kNoErr)
        err = POVMSAttr_Get(attr, type, (void *)ustr, &ulen);

    if(err == kNoErr)
    {
        POV_FRONTEND_ASSERT(ustr[ulen / 2 - 1] == '\0');
        ConvertUCS2ToUTF8(ustr, s);
    }

    delete[] ustr;

    return err;
}

int ProcessOptions::POVMSUtil_SetUTF8String(POVMSObjectPtr object, POVMSType key, const char *s)
{
    UCS2String ustr = UTF8toUCS2String(UTF8String(s));
    return POVMSUtil_SetUCS2String(object, key, ustr.c_str());
}

size_t ProcessOptions::ConvertUTF16ToUTF8(const UTF16 *char_array, UTF8String& s)
{
    int i, len;
    UCS4 chr;

    for (len = 0, i = 0; char_array[i] != '\0'; ++i)
    {
        chr = char_array[i];
        if ((chr >= 0xD800u) && (chr < 0xDBFFu) && (char_array[i+1] >= 0xDC00u) && (char_array[i+1] <= 0xDFFFu))
        {
            // surrogate pair
            ++i;
            chr = ((chr & 0x3FF) << 10) | (char_array[i] & 0x3FF);
        }
        // no need to map lonely surrogates to 0xFFFD here, as it doesn't change the UTF-8 encoding length.

        if (chr < 0x0080u)
            len += 1;
        else if (chr < 0x0800u)
            len += 2;
        else if (chr < 0x10000u)
            len += 3;
        else
            len += 4;
    }

    s.reserve(len);

    for (i = 0; char_array[i] != 0; ++i)
    {
        chr = char_array[i];
        if ((chr >= 0xD800u) && (chr < 0xDBFFu) && (char_array[i+1] >= 0xDC00u) && (char_array[i+1] <= 0xDFFFu))
        {
            // surrogate pair
            ++i;
            chr = ((chr & 0x3FF) << 10) | (char_array[i] & 0x3FF);
        }
        else if ((chr >= 0xD800u) && (chr < 0xDBFFu))
            // unmatched surrogate; can't represent in well-formed UTF-8.
            chr = 0xFFFDu;

        if(chr < 0x0080u)
        {
            s.push_back(char(chr));
        }
        else if (chr < 0x0800u)
        {
            s.push_back(char((chr >> 6) | 0xC0));
            s.push_back(char((chr & 0x3F) | 0x80));
        }
        else if (chr < 0x10000u)
        {
            s.push_back(char((chr >> 12) | 0xE0));
            s.push_back(char(((chr >> 6) & 0x3F) | 0x80));
            s.push_back(char((chr & 0x3F) | 0x80));
        }
        else
        {
            s.push_back(char((chr >> 18) | 0xF0));
            s.push_back(char(((chr >> 12) & 0x3F) | 0x80));
            s.push_back(char(((chr >> 6) & 0x3F) | 0x80));
            s.push_back(char((chr & 0x3F) | 0x80));
        }
    }

    return size_t(len);
}

size_t ProcessOptions::ConvertUCS2ToUTF8(const UCS2 *char_array, UTF8String& s)
{
    // UCS2 is effectively a subset of UTF16.
    return ConvertUTF16ToUTF8(reinterpret_cast<const UTF16*>(char_array), s);
}


int ProcessOptions::ReadSpecialOptionHandler(INI_Parser_Table *, char *, POVMSObjectPtr)
{
    // do nothing by default
    return kNoErr;
}

int ProcessOptions::ReadSpecialSwitchHandler(Cmd_Parser_Table *, char *, POVMSObjectPtr, bool)
{
    // do nothing by default
    return kNoErr;
}

int ProcessOptions::WriteSpecialOptionHandler(INI_Parser_Table *, POVMSObjectPtr, OTextStream *)
{
    // do nothing by default
    return kNoErr;
}

bool ProcessOptions::ProcessUnknownSwitch(char *, char *, POVMSObjectPtr)
{
    // accept no unknown switches by default
    return false;
}

int ProcessOptions::ProcessUnknownString(char *, POVMSObjectPtr)
{
    // do nothing by default
    return kNoErr;
}

void ProcessOptions::ParseError(const char *format, ...)
{
    va_list marker;
    char error_buffer[1024];

    va_start(marker, format);
    std::vsnprintf(error_buffer, sizeof(error_buffer), format, marker);
    va_end(marker);

    fprintf(stderr, "%s\n", error_buffer);
}

void ProcessOptions::ParseErrorAt(ITextStream *file, const char *format, ...)
{
    va_list marker;
    char error_buffer[1024];

    va_start(marker, format);
    std::vsnprintf(error_buffer, sizeof(error_buffer), format, marker);
    va_end(marker);

    fprintf(stderr, "%s\nFile '%s' at line '%u'", error_buffer, UCS2toSysString(file->name()).c_str(), (unsigned int) file->line());
}

void ProcessOptions::WriteError(const char *format, ...)
{
    va_list marker;
    char error_buffer[1024];

    va_start(marker, format);
    std::vsnprintf(error_buffer, sizeof(error_buffer), format, marker);
    va_end(marker);

    fprintf(stderr, "%s\n", error_buffer);
}

int ProcessOptions::Output_INI_Option(INI_Parser_Table *option, POVMSObjectPtr obj, OTextStream *file)
{
    POVMSFloat floatval;
    POVMSBool b;
    POVMSInt intval;
    int err = 0;
    int l;
    POVMSAttribute item;
    char *bufptr;

    switch(option->type)
    {
        case kPOVMSType_Int:
            if(POVMSUtil_GetInt(obj, option->key, &intval) == 0)
                file->printf("%s=%d\n", option->keyword, (int)intval);
            break;
        case kPOVMSType_Float:
            if(POVMSUtil_GetFloat(obj, option->key, &floatval) == 0)
                file->printf("%s=%g\n", option->keyword, (float)floatval);
            break;
        case kPOVMSType_Bool:
            if(POVMSUtil_GetBool(obj, option->key, &b) == 0)
            {
                if(b == true)
                    file->printf("%s=On\n", option->keyword);
                else
                    file->printf("%s=Off\n", option->keyword);
            }
            break;
        case kPOVMSType_CString:
            err = POVMSObject_Get(obj, &item, option->key);
            if(err != 0)
                break;
            // get the file name and path string
            l = 0;
            err = POVMSAttr_Size(&item, &l);
            if(l > 0)
            {
                bufptr = new char[l];
                bufptr[0] = 0;
                if(POVMSAttr_Get(&item, kPOVMSType_CString, bufptr, &l) == 0)
                    file->printf("%s=\"%s\"\n", option->keyword, bufptr);
                delete[] bufptr;
            }
            (void)POVMSAttr_Delete(&item);
            break;
        case kPOVMSType_UCS2String:
            err = POVMSObject_Get(obj, &item, option->key);
            if(err != 0)
                break;
            // get the file name and path string
            l = 0;
            err = POVMSAttr_Size(&item, &l);
            if(l > 0)
            {
                UTF8String buf;
                if (POVMSAttr_GetUTF8String(&item, kPOVMSType_UCS2String, buf) == 0)
                    file->printf("%s=\"%s\"\n", option->keyword, buf.c_str());
            }
            (void)POVMSAttr_Delete(&item);
            break;
        case kPOVMSType_WildCard:
            WriteSpecialOptionHandler(option, obj, file);
            break;
        case 0:
            // deprecated option, skip
            break;
        default:
            WriteError("Ignoring unknown INI option.");
            break;
    }

    return err;
}

int ProcessOptions::Parse_INI_Specification(const char *filespec, char *&filename, char *&sectionname)
{
    const char *sectionpos = strchr(filespec, '[');

    // if there is no section string, this is the whole filename
    if (sectionpos == nullptr)
    {
        filename = new char[strlen(filespec) + 1];
        strcpy(filename, filespec);
    }
    // if there is a section string, the filename ends at the section beginning
    else
    {
        const char *sectionend = strchr(filespec, ']');

        // if there was no section end, this file specification is invalid
        if (sectionend == nullptr)
            return kParamErr;
        // if there valid section specification, use it
        else
        {
            // section string starts at sectionpos + 1 and has the length sectionend - sectionpos - 1 + terminating zero
            sectionname = new char[sectionend - sectionpos];
            strncpy(sectionname, sectionpos + 1, sectionend - sectionpos - 1);
            sectionname[sectionend - sectionpos - 1] = 0;

            // filename string ends at sectionpos and the the length sectionpos - filespec + terminating zero
            filename = new char[sectionpos - filespec + 1];
            strncpy(filename, filespec, sectionpos - filespec);
            filename[sectionpos - filespec] = 0;
        }
    }

    return kNoErr;
}

int ProcessOptions::Parse_INI_Skip_Space(ITextStream *file, bool countnewlines)
{
    int linecount = 0;

    // read until the end of file or until a non-space is found
    while(file->eof() == false)
    {
        int chr = file->getchar();

        // count newlinegets
        if((chr == 10) && (countnewlines == true))
        {
            int c2 = file->getchar();
            if(c2 != 13)
                file->ungetchar(c2);
            linecount++;
        }
        else if((chr == 13) && (countnewlines == true))
        {
            int c2 = file->getchar();
            if(c2 != 10)
                file->ungetchar(c2);
            linecount++;
        }
        // apart from a newline, spaces and tabs are considered "space"
        else if((chr != ' ') && (chr != '\t'))
        {
            file->ungetchar(chr);
            break;
        }
    }

    return linecount;
}

int ProcessOptions::Parse_INI_Skip_Line(ITextStream *file)
{
    int linecount = 0;

    // read until the end of file or until a newline is found
    while(file->eof() == false)
    {
        int chr = file->getchar();

        // count newlines
        if(chr == 10)
        {
            int c2 = file->getchar();
            if(c2 != 13)
                file->ungetchar(c2);
            linecount++;
            break;
        }
        else if(chr == 13)
        {
            int c2 = file->getchar();
            if(c2 != 10)
                file->ungetchar(c2);
            linecount++;
            break;
        }
    }

    return linecount;
}

int ProcessOptions::Parse_INI_Option(ITextStream *file, POVMSObjectPtr obj)
{
    struct INI_Parser_Table *table = parse_ini_table;
    char *value = nullptr;
    char *key = nullptr;
    int chr = 0;
    int err = kNoErr;

    // read the key string
    key = Parse_INI_String(file);
    if (key == nullptr)
    {
        ParseErrorAt(file, "Expected key in INI file, no key was found.");
        return kParseErr;
    }

    // find the keyword
    while (table->keyword != nullptr)
    {
        if(pov_stricmp(table->keyword, key) == 0)
            break;
        table++;
    }

    // return if no valid keyword has been found
    if (table->keyword == nullptr)
    {
        ParseErrorAt(file, "Unknown key '%s' in INI file.", key);
        delete[] key;
        return kParseErr;
    }
    else
    {
        delete[] key;
        key = nullptr;
    }

    // skip any spaces
    (void)Parse_INI_Skip_Space(file, false);

    // expect the equal sign
    if(file->getchar() != '=')
        return kParseErr;

    // skip any spaces
    (void)Parse_INI_Skip_Space(file, false);

    // TODO: whether or not to apply special parsing ought to be encoded into the INI_Parser_Table,
    // but for now it's hard-coded here. special raw parsing is applied to INI options that we don't
    // want to do much processing on (such as removing quotes or guessing if they are filenames).
    // currently shellout commands are in that category since they may contain quoted paths or
    // quoted/escaped options. We only apply this special case here, where we know the input data
    // is a file (as it's also possible to specify INI options on the command-line).
    if (toupper(table->keyword[strlen(table->keyword) - 1]) == 'D' && // the command and return keywords have the same kPOVAttrib_ values; we only want the command
        (table->key == kPOVAttrib_PostFrameCommand || table->key == kPOVAttrib_PreFrameCommand ||
         table->key == kPOVAttrib_PostSceneCommand || table->key == kPOVAttrib_PreSceneCommand ||
         table->key == kPOVAttrib_UserAbortCommand || table->key == kPOVAttrib_FatalErrorCommand))
    {
        value = Parse_Raw_INI_String(file);
    }
    else
    {
        // if it looks like a quoted string, attempt to extract and handle that
        chr = file->getchar();
        if((chr == '\"') || (chr == '\''))
            value = Parse_INI_String(file, chr);
        // if there were no quotes, just read up to the next space or newline
        else
        {
            file->ungetchar(chr);
            value = Parse_INI_String(file, -2, true);
        }
    }

    if (value == nullptr)
    {
        ParseErrorAt(file, "Expected value in INI file, no value was found.");
        return kParseErr;
    }

    if(table->key == 0)
        ParseErrorAt(file, "INI option '%s' is no longer used and will result in an error in future versions of POV-Ray.", table->keyword);
    else
        err = Process_INI_Option(table, value, obj);

    delete[] value;
    value = nullptr;

    // skip any spaces
    (void)Parse_INI_Skip_Space(file, false);

    // if there is a comma, parse more values and append them
    chr = file->getchar();
    if(chr == ',')
    {
        // read more value strings
        while((file->eof() == false) && (err == kNoErr))
        {
            // skip any spaces
            (void)Parse_INI_Skip_Space(file, false);

            // if the string is quoted, parse it matching quotes
            chr = file->getchar();
            if((chr == '\"') || (chr == '\''))
                value = Parse_INI_String(file, chr);
            // if there were no quotes, just read up to the next space or newline
            else
            {
                file->ungetchar(chr);
                value = Parse_INI_String(file, -2);
            }

            if (value == nullptr)
            {
                ParseErrorAt(file, "Expected value in INI file, no value was found.");
                return kParseErr;
            }

            err = Process_INI_Option(table, value, obj);
            delete[] value;
            value = nullptr;

            // skip any spaces
            (void)Parse_INI_Skip_Space(file, false);

            // if there is no other comma, stop parsing values
            chr = file->getchar();
            if(chr != ',')
            {
                file->ungetchar(chr);
                break;
            }
        }
    }
    else
        file->ungetchar(chr);

    return err;
}

int ProcessOptions::Parse_INI_Switch(ITextStream *file, int token, POVMSObjectPtr obj)
{
    struct Cmd_Parser_Table *table = parse_cmd_table;
    char *value = nullptr;
    char *key = nullptr;
    int err = kNoErr;
    int chr = 0;

    // read the switch string
    key = Parse_INI_String(file);
    if (key == nullptr)
    {
        ParseErrorAt(file, "Expected command-line switch in INI file, no command-line switch was found.");
        err = kParseErr;
    }
    else
    {
        // if there is a quoted string directly following the switch, parse it, matching quotes
        chr = file->getchar();
        if((chr == '\"') || (chr == '\''))
        {
            value = Parse_INI_String(file, chr);
            if (value == nullptr)
                ParseErrorAt(file, "Expected command-line switch in INI file to be followed by quoted parameter.");
        }
        else
            file->ungetchar(chr);

        // find the command-line switch
        while (table->command != nullptr)
        {
            char *srcptr = key;
            const char *dstptr = table->command;

            // compared ignoring case until the end of either string has been reached
            while((toupper(*srcptr) == toupper(*dstptr)) && (*srcptr != 0) && (*dstptr != 0))
            {
                srcptr++;
                dstptr++;
            }
            // if the end of the switch string in the table had been reached, see if there are parameters
            // to consider and if there are, expect the source string to be followed by those parameters
            if((*dstptr) == 0)
            {
                // if there was a quoted value string and the switch string is longer, this is an unknown switch
                if ((value != nullptr) && (*srcptr != 0))
                {
                    table = nullptr;
                    break;
                }
                // if there was a quoted value string and the switch matches, use the value string as parameter
                else if ((value != nullptr) && (*srcptr == 0))
                    srcptr = value;

                // only if a parameter is expected allow it, and vice versa
                if ((table->flags & kCmdOptFlag_Optional) ||
                    ((*srcptr >  ' ') && (table->type != kPOVMSType_Null)) ||
                    ((*srcptr <= ' ') && (table->type == kPOVMSType_Null)))
                {
                    err = Process_Switch(table, srcptr, obj, (token != '-'));
                }

                // We're aborting the search now either way, because due how the table is supposed to be sorted,
                // if the switch would match another entry we should have encountered that one earlier.
                break;
            }
            table++;
        }

        // if there was no successful match so far, see if it is a system specific switch
        if ((table == nullptr) || (table->command == nullptr))
        {
            if(ProcessUnknownSwitch(key, value, obj) == false)
            {
                if (value != nullptr)
                    ParseErrorAt(file, "Unknown switch '%s' with value '%s' in INI file.", key, value);
                else
                    ParseErrorAt(file, "Unknown switch '%s' in INI file.", key);
                err = kParseErr;
            }
            else
                err = kNoErr;
        }
    }

    if (key != nullptr)
        delete[] key;
    if (value != nullptr)
        delete[] value;

    return err;
}

char *ProcessOptions::Parse_INI_String(ITextStream *file, int endchr, bool smartmode)
{
    char *str = new char[65536];
    char *pos = str;

    while((pos - str) < 65535)
    {
        int chr = file->getchar();
        // terminate the string if the end of file has been reached
        if(chr == EOF)
            break;
        // parse to the next space or special token
        else if((endchr == -1) || (endchr == -2))
        {
            if((smartmode == true) && ((chr == ' ') || (chr == '\t')))
            {
                // In "smart mode" the function called below tries to detect if the
                // user is trying to use an unquoted path as value of an INI option.
                // The detection logic is detailed in the function below!
                file->ungetchar(chr);
                if(Parse_INI_String_Smartmode(file) == false)
                    break;
                else
                {
                    chr = file->getchar();
                    endchr = -3; // switch to special mode
                }
            }
            else if(isspace(chr) || (chr == ',') || (chr == ';') || (chr == '#') || (chr == '\"') || (chr == '\'') ||
                    ((endchr == -1) && ((chr == '[') || (chr == ']') || (chr == '='))))
            {
                file->ungetchar(chr);
                break;
            }
        }
        // this should only be switched on by "smart mode" and parses to either  the end of the line or the comment string
        else if(endchr == -3)
        {
            if((chr == ';') || (chr == '#') || (chr == 10) || (chr == 13))
            {
                file->ungetchar(chr);
                break;
            }
        }
        // parse to the next character specified by the caller in endchr
        else if(chr == endchr)
            break;

        *pos = chr;
        pos++;
    }

    *pos = 0;

    return str;
}

// special case of parsing an INI string: we don't want to interpret anything other than
// a comment character or end-of-line, as the string itself will be passed on to other
// code that requires it more or less verbatim. an example of this is the shellout commands,
// which may contain a quoted program name (perhaps because it embeds spaces) followed by
// unquoted or quoted parameters.
// NB returned string may have trailing blanks.
char *ProcessOptions::Parse_Raw_INI_String(ITextStream *file)
{
    char *str = new char[65536];
    char *pos = str;
    bool inSQ = false;
    bool inDQ = false;
    bool hadEscape = false;
    const char *msg = "Possible unbalanced quotes detected in INI option being parsed in raw mode";

    str[0] = 0;
    for (int i = 0; i < 65536; i++, *pos = 0)
    {
        int ch = file->getchar();
        if (hadEscape && (ch == '"' || ch == '\'' || ch == '\\'))
        {
            hadEscape = false;
            *pos++ = ch;
            continue;
        }
        hadEscape = false;
        switch (ch)
        {
            case EOF:
                if (inSQ || inDQ)
                    ParseErrorAt(file, msg);
                return str;

            case '\r':
            case '\n':
                file->ungetchar(ch);
                if (inSQ || inDQ)
                    ParseErrorAt(file, msg);
                return str;

            case ';':
            case '#':
                if (!inSQ && !inDQ)
                {
                    file->ungetchar(ch);
                    return str;
                }
                break;

            case '\\':
                hadEscape = true;
                break;

            case '"':
                inDQ = !inDQ;
                break;

            case '\'':
                inSQ = !inSQ;
                break ;

            default:
                break;
        }
        *pos++ = ch;
    }

    if (inSQ || inDQ)
        ParseErrorAt(file, msg);
    return str;
}

bool ProcessOptions::Parse_INI_String_Smartmode(ITextStream *file)
{
    ITextStream::FilePos backtrackpos = file->tellg();
    bool result = false; // false - end string here, true - continue parsing string
    struct INI_Parser_Table *table = parse_ini_table;
    char *key = nullptr;

    (void)Parse_INI_Skip_Space(file, false);

    switch(file->getchar())
    {
        // end of file
        case EOF:
            break; // return false, parsing more of the string simply is not possible
        // end of line
        case '\r':
        case '\n':
            break; // return false, this was just a case of trailing whitespace
        // INI file comment
        case ';':
        case '#':
            break; // return false, this is a comment which terminates the string
        // INI value list separator
        case ',':
            break; // return false, this is a value list of unquoted strings
        // POV-Ray-style INI file with command-line switch
        case '+':
        case '-':
#if (POV_SLASH_IS_SWITCH_CHARACTER)
        // POV-Ray-style INI file with system specific command-line switch on some systems (e.g. Windos)
        case '/':
#endif
            if(isalpha(file->getchar()))
                break; // return false, this is most likely a command-line
            else
                file->seekg(backtrackpos); // most likely an unquoted string, so allow parsing it as a whole
        // INI file option
        default:
            // read the key string
            key = Parse_INI_String(file);
            if (key != nullptr)
            {
                // find the keyword
                while (table->keyword != nullptr)
                {
                    if(pov_stricmp(table->keyword, key) == 0)
                        break;
                    table++;
                }

                // if no valid keyword has been found
                if (table->keyword == nullptr)
                {
                    result = true; // return true, this is most likely an unquoted path
                    ParseErrorAt(file,
                                 "Most likely detected an unquoted string with spaces in INI file. Assuming string ends at the of the line.\n"
                                 "Make sure all strings with spaces are properly quoted in the INI file.\n"
                                 "Use either \" or \' to quote strings. For details, please check the user manual!");
                }

                delete[] key;
                key = nullptr;
            }
            break; // return false, unless the code above did not find a valid keyword
    }

    file->seekg(backtrackpos);

    return result;
}

void ProcessOptions::Parse_CL_Skip_Space(const char *&commandline)
{
    // read until the end of the string or until a non-space is found
    while(*commandline != 0)
    {
        // spaces and tabs are considered "space"
        if((*commandline != ' ') && (*commandline != '\t'))
            break;
        commandline++;
    }
}

int ProcessOptions::Parse_CL_Switch(const char *&commandline, int token, POVMSObjectPtr obj, bool singleswitch)
{
    struct Cmd_Parser_Table *table = parse_cmd_table;
    char *value = nullptr;
    char *key = nullptr;
    int err = kNoErr;
    int chr = 0;

    // read the switch string
    if(singleswitch == false) // see if quotes had been stripped outside POV-Ray
        key = Parse_CL_String(commandline);
    else
        key = Parse_CL_String(commandline, 0);
    if (key == nullptr)
    {
        ParseError("Expected command-line switch on command-line, no command-line switch was found.");
        err = kParseErr;
    }
    else
    {
        // if there is a quoted string directly following the switch, parse it, matching quotes
        chr = *commandline;
        commandline++;
        if((chr == '\"') || (chr == '\''))
        {
            value = Parse_CL_String(commandline, chr);
            if (value == nullptr)
                ParseError("Expected command-line switch on command-line to be followed by quoted parameter.");
        }
        else
            commandline--;

        // find the command-line switch
        while (table->command != nullptr)
        {
            char *srcptr = key;
            const char *dstptr = table->command;

            // compared ignoring case until the end of either string has been reached
            while((toupper(*srcptr) == toupper(*dstptr)) && (*srcptr != 0) && (*dstptr != 0))
            {
                srcptr++;
                dstptr++;
            }
            // if the end of the switch string in the table had been reached, see if there are parameters
            // to consider and if there are, expect the source string to be followed by those parameters
            if((*dstptr) == 0)
            {
                // if there was a quoted value string and the switch string is longer, this is an unknown switch
                if ((value != nullptr) && (*srcptr != 0))
                {
                    table = nullptr;
                    break;
                }
                // if there was a quoted value string and the switch matches, use the value string as parameter
                else if ((value != nullptr) && (*srcptr == 0))
                    srcptr = value;

                // only if a parameter is expected allow it, and vice versa
                if ((table->flags & kCmdOptFlag_Optional) ||
                    ((*srcptr >  ' ') && (table->type != kPOVMSType_Null)) ||
                    ((*srcptr <= ' ') && (table->type == kPOVMSType_Null)))
                {
                    err = Process_Switch(table, srcptr, obj, (token != '-'));
                }

                // We're aborting the search now either way, because due how the table is supposed to be sorted,
                // if the switch would match another entry we should have encountered that one earlier.
                break;
            }
            table++;
        }

        // if there was no successful match so far, see if it is a system specific switch
        if ((table == nullptr) || (table->command == nullptr))
        {
            if(ProcessUnknownSwitch(key, value, obj) == false)
            {
                if (value != nullptr)
                    ParseError("Unknown switch '%s' with value '%s' on command-line.", key, value);
                else
                    ParseError("Unknown switch '%s' on command-line.", key);
                err = kParseErr;
            }
            else
                err = kNoErr;
        }
    }

    if (key != nullptr)
        delete[] key;
    if (value != nullptr)
        delete[] value;

    return err;
}

int ProcessOptions::Parse_CL_Option(const char *&commandline, POVMSObjectPtr obj, bool singleswitch)
{
    struct INI_Parser_Table *table = parse_ini_table;
    char *value = nullptr;
    char *key = nullptr;
    char chr = 0;
    int err = kNoErr;

    // read the key string
    key = Parse_CL_String(commandline);
    if (key == nullptr)
    {
        ParseError("Expected INI file key on command-line, no key was found.");
        return kParseErr;
    }

    // find the keyword
    while (table->keyword != nullptr)
    {
        if(pov_stricmp(table->keyword, key) == 0)
            break;
        table++;
    }

    // return false if no valid keyword has been found
    if (table->keyword == nullptr)
    {
        delete[] key;
        return kParseErr;
    }
    else
    {
        delete[] key;
        key = nullptr;
    }

    // expect the equal sign
    if(*commandline != '=')
        return kParseErr;
    commandline++;

    // if the string is quoted, parse it matching quotes
    chr = *commandline;
    if((chr == '\"') || (chr == '\''))
    {
        commandline++;

        value = Parse_CL_String(commandline, chr);
    }
    // if there were no quotes, just read up to the next space or newline
    else if(singleswitch == false) // see if quotes had been stripped outside POV-Ray
        value = Parse_CL_String(commandline, -2);
    else
        value = Parse_CL_String(commandline, 0);

    if (value == nullptr)
    {
        ParseError("Expected value on command-line, no value was found.");
        return kParseErr;
    }

    err = Process_INI_Option(table, value, obj);
    delete[] value;
    value = nullptr;

    return err;
}

char *ProcessOptions::Parse_CL_String(const char *&commandline, int endchr)
{
    int maxlen = strlen(commandline) + 1;
    char *str = new char[maxlen];
    char *pos = str;

    while(*commandline != 0)
    {
        int chr = *commandline;

        if(endchr <= -1)
        {
            if(isspace(chr) || (chr == ';') || (chr == '#') || (chr == '\"') || (chr == '\''))
                break;
            else if((endchr == -1) && ((chr == '[') || (chr == ']') || (chr == '=')))
                break;
        }

        commandline++;

        if(chr == endchr)
            break;

        *pos = chr;
        pos++;
    }

    *pos = 0;

    return str;
}

int ProcessOptions::Process_INI_Option(INI_Parser_Table *option, char *param, POVMSObjectPtr obj)
{
    double floatval = 0.0;
    int intval = 0;
    int err = kNoErr;

    switch(option->type)
    {
        case kPOVMSType_Int:
            if(sscanf(param, "%d", &intval) == 1)
                err = POVMSUtil_SetInt(obj, option->key, intval);
            else
            {
                ParseError("Integer parameter expected for option '%s', found '%s'.", option->keyword, param);
                err = kParseErr;
            }
            break;
        case kPOVMSType_Float:
            if(sscanf(param, "%lf", &floatval) == 1)
                err = POVMSUtil_SetFloat(obj, option->key, floatval);
            else
            {
                ParseError("Floating-point parameter expected for option '%s', found '%s'.", option->keyword, param);
                err = kParseErr;
            }
            break;
        case kPOVMSType_Bool:
            err = POVMSUtil_SetBool(obj, option->key, IsTrue(param));
            break;
        case kPOVMSType_CString:
            err = POVMSUtil_SetString(obj, option->key, param);
            break;
        case kPOVMSType_UCS2String:
            err = POVMSUtil_SetUTF8String(obj, option->key, param);
            break;
        case kPOVMSType_WildCard:
            err = ReadSpecialOptionHandler(option, param, obj);
            break;
        default:
            err = kParseErr;
            break;
    }

    return err;
}

int ProcessOptions::Process_Switch(Cmd_Parser_Table *option, char *param, POVMSObjectPtr obj, bool is_on)
{
    double floatval = 0.0;
    int intval = 0;
    int err = 0;

    if(option->is_switch != kPOVMSType_Null)
    {
        err = POVMSUtil_SetBool(obj, option->is_switch, is_on);
        if(err != kNoErr)
            return err;
    }

    // If parameter is optional and none is specified by the user, we're done.
    if ((option->flags & kCmdOptFlag_Optional) && (*param < ' '))
        return err;

    switch(option->type)
    {
        case kPOVMSType_Int:
            if(sscanf(param, "%d", &intval) == 1)
                err = POVMSUtil_SetInt(obj, option->key, intval);
            else
            {
                ParseError("Integer parameter expected for switch '%s', found '%s'.", option->command, param);
                err = kParseErr;
            }
            break;
        case kPOVMSType_Float:
            if(sscanf(param, "%lf", &floatval) == 1)
                err = POVMSUtil_SetFloat(obj, option->key, floatval);
            else
            {
                ParseError("Floating-point parameter expected for switch '%s', found '%s'.", option->command, param);
                err = kParseErr;
            }
            break;
        case kPOVMSType_Bool:
            err = POVMSUtil_SetBool(obj, option->key, IsTrue(param));
            break;
        case kPOVMSType_CString:
            err = POVMSUtil_SetString(obj, option->key, param);
            break;
        case kPOVMSType_UCS2String:
            err = POVMSUtil_SetUTF8String(obj, option->key, param);
            break;
        case kPOVMSType_WildCard:
            err = ReadSpecialSwitchHandler(option, param, obj, is_on);
            break;
        case kPOVMSType_Null:
            break;
        default:
            err = kParseErr;
            break;
    }

    return err;
}

}
// end of namespace pov_frontend
