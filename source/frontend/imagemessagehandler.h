/*******************************************************************************
 * imagemessagehandler.h
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/frontend/imagemessagehandler.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef POVRAY_FRONTEND_IMAGEMESSAGEHANDLER_H
#define POVRAY_FRONTEND_IMAGEMESSAGEHANDLER_H

#include "base/types.h"

#include "frontend/configfrontend.h"
#include "frontend/renderfrontend.h"

#include <vector>
#include <list>
#include <map>

namespace pov_frontend
{

using namespace pov_base;

class ImageMessageHandler
{
	public:
		ImageMessageHandler();
		virtual ~ImageMessageHandler();

		void HandleMessage(const SceneData&, const ViewData&, POVMSType, POVMS_Object&);
	protected:
		virtual void DrawPixelSet(const SceneData&, const ViewData&, POVMS_Object&);
		virtual void DrawPixelBlockSet(const SceneData&, const ViewData&, POVMS_Object&);
		virtual void DrawPixelRowSet(const SceneData&, const ViewData&, POVMS_Object&);
		virtual void DrawRectangleFrameSet(const SceneData&, const ViewData&, POVMS_Object&);
		virtual void DrawFilledRectangleSet(const SceneData&, const ViewData&, POVMS_Object&);
};

}

#endif // POVRAY_FRONTEND_IMAGEMESSAGEHANDLER_H
