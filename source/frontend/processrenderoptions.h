//******************************************************************************
///
/// @file frontend/processrenderoptions.h
///
/// @todo   What's in here?
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

#ifndef POVRAY_FRONTEND_PROCESSRENDEROPTIONS_H
#define POVRAY_FRONTEND_PROCESSRENDEROPTIONS_H

#include "frontend/configfrontend.h"
#include "frontend/processoptions.h"

namespace pov_frontend
{

using namespace pov_base;

class ProcessRenderOptions : public ProcessOptions
{
    public:
        ProcessRenderOptions();
        ~ProcessRenderOptions();
    protected:
        virtual int ReadSpecialOptionHandler(INI_Parser_Table *, char *, POVMSObjectPtr);
        virtual int ReadSpecialSwitchHandler(Cmd_Parser_Table *, char *, POVMSObjectPtr, bool);
        virtual int WriteSpecialOptionHandler(INI_Parser_Table *, POVMSObjectPtr, OTextStream *);
        virtual int ProcessUnknownString(char *, POVMSObjectPtr);

        virtual ITextStream *OpenFileForRead(const char *, POVMSObjectPtr);
        virtual OTextStream *OpenFileForWrite(const char *, POVMSObjectPtr);

        ITextStream *OpenINIFileStream(const char *, unsigned int, POVMSObjectPtr);
    public:
        // TODO FIXME - The following are hacks of some sort, no idea what they are good for. They certainly use wrong types and probably contain other mistakes [trf]

        struct Output_FileType_Table
        {
            char        code;               // code used in INI and command line options
            POVMSType   attribute;          // attribute for which the entry is valid (0 = all)
            int         internalId;         // kPOVList_FileType_*
            bool        has16BitGrayscale;  // grayscale output at >> 8bpp is supported for this file type
            bool        hasAlpha;           // alpha channel output is officially(!) supported for this file type
        };

        struct Parameter_Code_Table
        {
            const char* code;               // code used in INI and command line options
            int         internalId;         // e.g. kPOVList_GammaType_*
        };

        int ParseFileType(char, POVMSType, int*, bool* pHas16BitGreyscale = NULL);
        char UnparseFileType(int);
        int ParseGammaType(char*, int*);
        const char* UnparseGammaType(int);
        int ParseParameterCode(const ProcessRenderOptions::Parameter_Code_Table* codeTable, char* code, int* pInternalId);
        const char* UnparseParameterCode(const ProcessRenderOptions::Parameter_Code_Table* codeTable, int internalId);
};

}

#endif // POVRAY_FRONTEND_PROCESSRENDEROPTIONS_H
