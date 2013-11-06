/*******************************************************************************
 * messagefactory.h
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
 * $File: //depot/public/povray/3.x/source/backend/control/messagefactory.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef POVRAY_BACKEND_MESSAGEFACTORY_H
#define POVRAY_BACKEND_MESSAGEFACTORY_H

#include "base/povms.h"
#include "base/povmscpp.h"
#include "base/povmsgid.h"
#include "base/pov_err.h"
#include "backend/control/renderbackend.h"

namespace pov
{

using namespace pov_base;

class MessageFactory
{
	public:
		MessageFactory(unsigned int wl, unsigned int lv, const char *sn, POVMSAddress saddr, POVMSAddress daddr, RenderBackend::SceneId sid, RenderBackend::ViewId vid);

		void Warning(unsigned int level, const char *format,...);
		void WarningAt(unsigned int level, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);

		void PossibleError(const char *format,...);
		void PossibleErrorAt(const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);

		void Error(const char *format,...);
		void Error(const Exception& ex, const char *format,...);
		void Error(Exception& ex, const char *format,...);
		void ErrorAt(const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);
		void ErrorAt(const Exception& ex, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);
		void ErrorAt(Exception& ex, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);
		void SetLanguageVersion(unsigned int Val) { languageVersion = Val ; } // TODO FIXME - not here, not this way
		void SetWarningLevel(unsigned int Val) { warningLevel = Val ; } // TODO FIXME - not here, not this way

	private:
		unsigned int warningLevel;
		unsigned int languageVersion;
		const char *stageName;
		POVMSAddress sourceAddress;
		POVMSAddress destinationAddress;
		RenderBackend::SceneId sceneId;
		RenderBackend::ViewId viewId;

		void CleanupString(char *str);
		std::string SendError(const char *format, va_list arglist, const UCS2 *filename = NULL, POV_LONG line = -1, POV_LONG column = -1, POV_LONG offset = -1);
};

}

#endif // POVRAY_BACKEND_MESSAGEFACTORY_H
