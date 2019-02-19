//******************************************************************************
///
/// @file backend/scene/viewthreaddata.h
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

#ifndef POVRAY_BACKEND_VIEWTHREADDATA_H
#define POVRAY_BACKEND_VIEWTHREADDATA_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"
#include "backend/scene/viewthreaddata_fwd.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/scene/tracethreaddata.h"

// POV-Ray header files (backend module)
#include "backend/scene/view_fwd.h"

namespace pov
{

/**
 *  Class holding render thread specific data.
 */
class ViewThreadData final : public TraceThreadData
{
        friend class Scene;
    public:
        /**
         *  Create thread local data.
         *  @param  vd              View data defining view attributes
         *                          as well as view output.
         *  @param  seed            Seed for the stochastic random number generator;
         *                          should be unique for each render unless overridden by the user.
         */
        ViewThreadData(ViewData *vd, size_t seed);

        /**
         *  Get width of view.
         *  @return                 Width.
         */
        unsigned int GetWidth() const;

        /**
         *  Get height of view.
         *  @return                 Height.
         */
        unsigned int GetHeight() const;

        /**
         *  Get area of view to be rendered.
         *  @return                 Area rectangle.
         */
        const POVRect& GetRenderArea();
    protected:
        /// view data
        ViewData *viewData;
    private:

        ViewThreadData() = delete;
        ViewThreadData(const ViewThreadData&) = delete;
        ViewThreadData& operator=(const ViewThreadData&) = delete;

    public: // TODO FIXME - temporary workaround [trf]
        /**
         *  Destructor.
         */
        virtual ~ViewThreadData() override;
};

}
// end of namespace pov

#endif // POVRAY_BACKEND_VIEWTHREADDATA_H
