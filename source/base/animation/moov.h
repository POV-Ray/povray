//******************************************************************************
///
/// @file base/animation/moov.h
///
/// Declarations related to QuickTime (MooV) stream handling.
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

#ifndef POVRAY_BASE_MOOV_H
#define POVRAY_BASE_MOOV_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// POV-Ray base header files
#include "base/types.h"
#include "base/animation/animation.h"

namespace pov_base
{

namespace Moov
{

void *ReadFileHeader(IStream *file, float& lengthinseconds, unsigned int& lengthinframes, Animation::CodecType& codec, unsigned int& w, unsigned int& h, const Animation::ReadOptions& options, vector<string>& warnings);
void PreReadFrame(IStream *file, unsigned int frame, POV_LONG& bytes, Animation::CodecType& codec, const Animation::ReadOptions& options, vector<string>& warnings, void *state);
void PostReadFrame(IStream *file, unsigned int frame, POV_LONG bytes, Animation::CodecType& codec, const Animation::ReadOptions& options, vector<string>& warnings, void *state);
void FinishReadFile(IStream *file, vector<string>& warnings, void *state);

void *WriteFileHeader(OStream *file, Animation::CodecType& codec, unsigned int w, unsigned int h, const Animation::WriteOptions& options, vector<string>& warnings);
void PreWriteFrame(OStream *file, const Animation::WriteOptions& options, vector<string>& warnings, void *state);
void PostWriteFrame(OStream *file, POV_LONG bytes, const Animation::WriteOptions& options, vector<string>& warnings, void *state);
void FinishWriteFile(OStream *file, const Animation::WriteOptions& options, vector<string>& warnings, void *state);

}

}

#endif // POVRAY_BASE_MOOV_H
