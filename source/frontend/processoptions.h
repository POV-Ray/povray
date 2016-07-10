//******************************************************************************
///
/// @file frontend/processoptions.h
///
/// This module contains all defines, typedefs, and prototypes for the
/// C++ interface of `processoptions.cpp`.
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

#ifndef PROCESSOPTIONS_H
#define PROCESSOPTIONS_H

#include "base/configbase.h"

#include "povms/povmscpp.h"

#include "base/textstream.h"

namespace pov_base
{

enum {
    kINIOptFlag_SuppressWrite = 0x0001,     ///< Suppress when writing complete list of options
};

class ProcessOptions
{
    public:
        struct INI_Parser_Table
        {
            const char  *keyword;
            POVMSType   key;
            POVMSType   type;
            int         flags;
        };

        struct Cmd_Parser_Table
        {
            const char *command;
            POVMSType key;
            POVMSType type;
            POVMSType is_switch;
        };

        ProcessOptions(INI_Parser_Table *, Cmd_Parser_Table *);
        ~ProcessOptions();

        int ParseFile(const char *, POVMSObjectPtr);
        int ParseString(const char *, POVMSObjectPtr, bool singleswitch = false);

        int WriteFile(const char *, POVMSObjectPtr);
        int WriteFile(OTextStream *, POVMSObjectPtr);

        static bool Matches(const char *, const char *);
        static bool IsTrue(const char *);
        static bool IsFalse(const char *);

        static int POVMSAttr_GetUTF8String(POVMSAttributePtr, POVMSType, char *, int *);
        static int POVMSAttr_SetUTF8String(POVMSAttributePtr, POVMSType, const char *);
        static int POVMSUtil_SetUTF8String(POVMSObjectPtr, POVMSType, const char *);
        static size_t ConvertUTF8ToUCS2(const char *, UCS2 *);
        static size_t ConvertUCS2ToUTF8(const UCS2 *, char *);
    protected:
        virtual int ReadSpecialOptionHandler(INI_Parser_Table *, char *, POVMSObjectPtr);
        virtual int ReadSpecialSwitchHandler(Cmd_Parser_Table *, char *, POVMSObjectPtr, bool);
        virtual int WriteSpecialOptionHandler(INI_Parser_Table *, POVMSObjectPtr, OTextStream *);
        virtual bool ProcessUnknownSwitch(char *, char *, POVMSObjectPtr);
        virtual int ProcessUnknownString(char *, POVMSObjectPtr);

        virtual ITextStream *OpenFileForRead(const char *, POVMSObjectPtr) = 0;
        virtual OTextStream *OpenFileForWrite(const char *, POVMSObjectPtr) = 0;

        virtual void ParseError(const char *, ...);
        virtual void ParseErrorAt(ITextStream *, const char *, ...);
        virtual void WriteError(const char *, ...);
    private:
        INI_Parser_Table *parse_ini_table;
        Cmd_Parser_Table *parse_cmd_table;

        int Output_INI_Option(INI_Parser_Table *, POVMSObjectPtr, OTextStream *);

        int Parse_INI_Specification(const char *, char *&, char *&);
        int Parse_INI_Skip_Space(ITextStream *, bool);
        int Parse_INI_Skip_Line(ITextStream *);
        int Parse_INI_Option(ITextStream *, POVMSObjectPtr);
        int Parse_INI_Switch(ITextStream *, int, POVMSObjectPtr);
        char *Parse_INI_String(ITextStream *, int endchr = -1, bool smartmode = false);
        bool Parse_INI_String_Smartmode(ITextStream *);
        char *Parse_Raw_INI_String(ITextStream *file);

        int Parse_CL(char *, POVMSObjectPtr, bool);
        void Parse_CL_Skip_Space(const char *&);
        int Parse_CL_Switch(const char *&, int , POVMSObjectPtr, bool);
        int Parse_CL_Option(const char *&, POVMSObjectPtr, bool);
        char *Parse_CL_String(const char *&, int endchr = -1);

        int Process_INI_Option(INI_Parser_Table *, char *, POVMSObjectPtr);
        int Process_Switch(Cmd_Parser_Table *, char *, POVMSObjectPtr, bool);
};

}

#endif
