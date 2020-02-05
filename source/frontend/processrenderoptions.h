//******************************************************************************
///
/// @file frontend/processrenderoptions.h
///
/// @todo   What's in here?
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

#ifndef POVRAY_FRONTEND_PROCESSRENDEROPTIONS_H
#define POVRAY_FRONTEND_PROCESSRENDEROPTIONS_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "frontend/configfrontend.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"

// POV-Ray header files (frontend module)
#include "frontend/processoptions.h"

namespace pov_frontend
{

using namespace pov_base;

class ProcessRenderOptions : public ProcessOptions
{
    public:
        ProcessRenderOptions();
        virtual ~ProcessRenderOptions() override;
    protected:
        virtual int ReadSpecialOptionHandler(INI_Parser_Table *, char *, POVMSObjectPtr) override;
        virtual int ReadSpecialSwitchHandler(Cmd_Parser_Table *, char *, POVMSObjectPtr, bool) override;
        virtual int WriteSpecialOptionHandler(INI_Parser_Table *, POVMSObjectPtr, OTextStream *) override;
        virtual int ProcessUnknownString(char *, POVMSObjectPtr) override;

        virtual ITextStream *OpenFileForRead(const char *, POVMSObjectPtr) override;
        virtual OTextStream *OpenFileForWrite(const char *, POVMSObjectPtr) override;

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
            const char* text;               // human-readable text
        };

        int ParseFileType(char, POVMSType, int*, bool* pHas16BitGreyscale = nullptr);
        static char UnparseFileType(int);
        int ParseGammaType(char*, int*);
        static const char* UnparseGammaType(int);
        static const char* GetGammaTypeText(int);
        static const char* GetDitherMethodText(int);
        static int ParseParameterCode(const ProcessRenderOptions::Parameter_Code_Table* codeTable, char* code, int* pInternalId);
        static const char* UnparseParameterCode(const ProcessRenderOptions::Parameter_Code_Table* codeTable, int internalId);
        static const char* GetParameterCodeText(const ProcessRenderOptions::Parameter_Code_Table* codeTable, int internalId);
};

}
// end of namespace pov_frontend

#endif // POVRAY_FRONTEND_PROCESSRENDEROPTIONS_H
