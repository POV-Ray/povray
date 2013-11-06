/*******************************************************************************
 * texture.h
 *
 * This file contains defines and variables for the txt*.cpp files
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
 * $File: //depot/public/povray/3.x/source/backend/texture/texture.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

/* NOTE: FRAME.H contains other texture stuff. */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "backend/pattern/pattern.h"
#include "backend/pattern/warps.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/*
 * Macro to create random number in the [0; 1] range.
 */

#define FLOOR(x)  ((x) >= 0.0 ? floor(x) : (0.0 - floor(0.0 - (x)) - 1.0))

#define Hash3d(a,b,c) \
	hashTable[(int)(hashTable[(int)(hashTable[(int)((a) & 0xfff)] ^ ((b) & 0xfff))] ^ ((c) & 0xfff))]



/*****************************************************************************
* Global typedefs
******************************************************************************/

/*****************************************************************************
* Global variables
******************************************************************************/

#ifdef DYNAMIC_HASHTABLE
extern unsigned short *hashTable;
#else
extern ALIGN16 unsigned short hashTable[8192];
#endif


/*****************************************************************************
* Global functions
******************************************************************************/

void Initialize_Noise (void);
void Initialize_Waves(vector<double>& waveFrequencies, vector<Vector3d>& waveSources, unsigned int numberOfWaves);
void Free_Noise_Tables (void);
#if defined(USE_AVX_FMA4_FOR_NOISE)
extern DBL (*Noise) (const VECTOR EPoint, int noise_generator);
extern void (*DNoise) (VECTOR result, const VECTOR EPoint);
void Initialise_NoiseDispatch();
DBL AVX_FMA4_Noise(const VECTOR EPoint, int noise_generator);
void AVX_FMA4_DNoise(VECTOR result, const VECTOR EPoint);

#else
INLINE_NOISE DBL Noise (const VECTOR EPoint, int noise_generator);
INLINE_NOISE void DNoise (VECTOR result, const VECTOR EPoint);
#endif
DBL Turbulence (const VECTOR EPoint, const TURB *Turb, int noise_generator);
void DTurbulence (VECTOR result, const VECTOR EPoint, const TURB *Turb);
DBL cycloidal (DBL value);
DBL Triangle_Wave (DBL value);
void Transform_Textures (TEXTURE *Textures, const TRANSFORM *Trans);
void Destroy_Textures (TEXTURE *Textures);
void Post_Textures (TEXTURE *Textures);
FINISH *Create_Finish (void);
FINISH *Copy_Finish (const FINISH *Old);
TEXTURE *Copy_Texture_Pointer (TEXTURE *Texture);
TEXTURE *Copy_Textures (const TEXTURE *Textures);
TEXTURE *Create_Texture (void);
int Test_Opacity (const TEXTURE *Texture);
TURB *Create_Turb (void);

}

#endif
