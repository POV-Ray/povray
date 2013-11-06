/*******************************************************************************
 * povrayold.h
 *
 * Obsolete data structures that have not been cleaned up yet!
 * Do not add anything new to this file.  Clean up stuff you need
 * as soon as possible.
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
 * $File: //depot/public/povray/3.x/source/povrayold.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef POVRAY_H
#define POVRAY_H

#include <time.h>
#include <string>

#include "backend/frame.h"

namespace pov
{

using namespace pov_base;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define STAGE_PREINIT         0  // set in POVRAY.C
#define STAGE_STARTUP         1  // set in POVRAY.C
#define STAGE_BANNER          2  // set in POVRAY.C
#define STAGE_INIT            3  // set in POVRAY.C
#define STAGE_FILE_INIT       4  // set in POVRAY.C
#define STAGE_PARSING         5  // set in PARSE.C
#define STAGE_CONTINUING      6  // set in POVRAY.C
#define STAGE_RENDERING       7  // set in POVRAY.C
#define STAGE_SHUTDOWN        8  // set in POVRAY.C
#define STAGE_CLEANUP_PARSE   9  // set in PARSE.C
#define STAGE_SLAB_BUILDING  10  // set in POVRAY.C
#define STAGE_TOKEN_INIT     11  // set in TOKENIZE.C
#define STAGE_INCLUDE_ERR    12  // set in TOKENIZE.C
#define STAGE_FOUND_INSTEAD  13  // set in TOKENIZE.C
#define STAGECOUNT           14  // number of stages

//#define DISPLAY           0x000001L
//#define VERBOSE           0x000002L
//#define DISKWRITE         0x000004L
//#define PROMPTEXIT        0x000008L
//#define ANTIALIAS         0x000010L
//#define RGBSEPARATE       0x000020L
//#define EXITENABLE        0x000040L
//#define CONTINUE_TRACE    0x000080L
#define JITTER            0x000100L
//#define PREVIEW           0x000200L
#define SPLIT_UNION       0x000400L
#define USE_VISTA_BUFFER  0x000800L
#define USE_LIGHT_BUFFER  0x001000L
#define USE_VISTA_DRAW    0x002000L
#define REMOVE_BOUNDS     0x004000L
//#define CYCLIC_ANIMATION  0x008000L
//#define OUTPUT_ALPHA      0x010000L
//#define HF_GRAY_16        0x020000L
//#define GAMMA_CORRECT     0x040000L
//#define FROM_STDIN        0x080000L
//#define TO_STDOUT         0x100000L

#define Q_FULL_AMBIENT    0x000001L
#define Q_QUICKC          0x000002L
#define Q_SHADOW          0x000004L
#define Q_AREA_LIGHT      0x000008L
#define Q_REFRACT         0x000010L
#define Q_REFLECT         0x000020L
#define Q_NORMAL          0x000040L
#define Q_VOLUME          0x000080L
#define Q_ADVANCED_LIGHT  0x000100L
#define Q_SUBSURFACE      0x000200L

#define EF_SSLT    1
#define EF_SLOPEM  2
#define EF_ISOFN   4
#define EF_SPLINE  8
#define EF_TIFF    16
#define EF_BACKILL 32
#define EF_MESHCAM 64

#define BF_VIDCAP  1
#define BF_RTR     2

#define QUALITY_0  Q_QUICKC+Q_FULL_AMBIENT
#define QUALITY_1  QUALITY_0
#define QUALITY_2  QUALITY_1-Q_FULL_AMBIENT
#define QUALITY_3  QUALITY_2
#define QUALITY_4  QUALITY_3+Q_SHADOW
#define QUALITY_5  QUALITY_4+Q_AREA_LIGHT
#define QUALITY_6  QUALITY_5-Q_QUICKC+Q_REFRACT
#define QUALITY_7  QUALITY_6
#define QUALITY_8  QUALITY_7+Q_REFLECT+Q_NORMAL
#define QUALITY_9  QUALITY_8+Q_VOLUME+Q_ADVANCED_LIGHT+Q_SUBSURFACE


/*****************************************************************************
* Global typedefs
******************************************************************************/

struct RadiositySettings
{
	int Quality;  // Q-flag value for light gathering

	int File_ReadOnContinue;
	int File_SaveWhileRendering;
	int File_AlwaysReadAtStart;
	int File_KeepOnAbort;
	int File_KeepAlways;
	char *Load_File_Name; //[FILE_NAME_LENGTH];
	char *Save_File_Name; //[FILE_NAME_LENGTH];
};

struct QualitySettings
{
	int Quality;
	unsigned int Quality_Flags;
	int Tracing_Method;
	int AntialiasDepth;
	DBL Antialias_Threshold;
	DBL JitterScale;
};

typedef struct OPTIONS_STRUCT
{
	unsigned int Options;

	int Abort_Test_Counter;

	char *Ini_Output_File_Name;

	char *Header_File_Name;
} Opts;

}

/*****************************************************************************
* Global functions
******************************************************************************/

namespace pov
{

void Do_Cooperate(int level);

}

#endif
