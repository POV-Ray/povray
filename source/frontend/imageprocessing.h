//******************************************************************************
///
/// @file frontend/imageprocessing.h
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
//*******************************************************************************

#ifndef POVRAY_FRONTEND_IMAGEPROCESSING_H
#define POVRAY_FRONTEND_IMAGEPROCESSING_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "frontend/configfrontend.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// POV-Ray header files (base module)
#include "base/stringtypes.h"
#include "base/image/image_fwd.h"

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"

// POV-Ray header files (frontend module)
//  (none at the moment)

namespace pov_frontend
{

using namespace pov_base;

class ImageProcessing
{
    public:
        ImageProcessing(unsigned int width, unsigned int height);
        ImageProcessing(POVMS_Object& ropts);
        ImageProcessing(std::shared_ptr<Image>& img);
        virtual ~ImageProcessing();

        UCS2String WriteImage(POVMS_Object& ropts, POVMSInt frame = 0, int digits = 0);

        std::shared_ptr<Image>& GetImage();

        UCS2String GetOutputFilename(POVMS_Object& ropts, POVMSInt frame, int digits);
        bool OutputIsStdout(void) { return toStdout; }
        bool OutputIsStderr(void) { return toStderr; }
        virtual bool OutputIsStdout(POVMS_Object& ropts);
        virtual bool OutputIsStderr(POVMS_Object& ropts);

    protected:
        std::shared_ptr<Image> image;
        bool toStdout;
        bool toStderr;

    private:

        ImageProcessing() = delete;
        ImageProcessing(const ImageProcessing&) = delete;
        ImageProcessing& operator=(const ImageProcessing&) = delete;
};

}
// end of namespace pov_frontend

#endif // POVRAY_FRONTEND_IMAGEPROCESSING_H
