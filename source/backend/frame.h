//******************************************************************************
///
/// @file backend/frame.h
///
/// Generic header for all back-end modules.
///
/// This header file is included by all C++ modules in the POV-Ray back-end.
/// It defines various ubiquitous types and constants.
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

#ifndef FRAME_H
#define FRAME_H

/// @file
///
/// @todo   The size of this file, and it being grouped into various thematical subsections,
///         indicate that its content should be split up into multiple files, to be included from
///         this one; furthermore, some contents may not really be that ubiquitous to warrant
///         including them in every module.

#include <new>

#include <cstdio>
#include <cstring>
#include <cmath>

#include <vector>

#include "base/configbase.h"
#include "base/colour.h"
#include "base/types.h"

#include "core/coretypes.h"
#include "core/bounding/boundingbox.h"

#include "backend/configbackend.h"

namespace pov
{

using namespace pov_base;

// from <algorithm>; we don't want to always type the namespace for these.
using std::min;
using std::max;

// from <cmath>; we don't want to always type the namespace for these.
using std::abs;
using std::acos;
using std::asin;
using std::atan;
using std::atan2;
using std::ceil;
using std::cos;
using std::cosh;
using std::exp;
using std::fabs;
using std::floor;
using std::fmod;
using std::frexp;
using std::ldexp;
using std::log;
using std::log10;
using std::modf;
using std::pow;
using std::sin;
using std::sinh;
using std::sqrt;
using std::tan;
using std::tanh;

//******************************************************************************
///
/// @name Forward Declarations
/// @{

class CompoundObject;

class Intersection;
class Ray;


/// @}
///
//******************************************************************************
///
/// @name Scalar, Colour and Vector Stuff
///
/// @{

inline void Destroy_Float(DBL *x)
{
    if(x != NULL)
        delete x;
}

/// @}
///
//******************************************************************************
///
/// @name Image Stuff
/// @{

// Image types.

#define IMAGE_FILE    GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define NORMAL_FILE   GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define MATERIAL_FILE GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define HF_FILE       GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+POT_FILE

#define DEFAULT_FRACTAL_EXTERIOR_TYPE 1
#define DEFAULT_FRACTAL_INTERIOR_TYPE 0
#define DEFAULT_FRACTAL_EXTERIOR_FACTOR 1
#define DEFAULT_FRACTAL_INTERIOR_FACTOR 1

/// @}
///
//******************************************************************************
///
/// @name Object Stuff
///
/// @{

#ifndef DUMP_OBJECT_DATA
#define DUMP_OBJECT_DATA 0
#endif

typedef struct Project_Struct PROJECT;
typedef struct Project_Tree_Node_Struct PROJECT_TREE_NODE;

struct Project_Struct
{
    int x1, y1, x2, y2;
};

/*
 * The following structure represent the bounding box hierarchy in 2d space.
 * Because is_leaf, Object and Project are the first elements in both
 * structures they can be accessed without knowing at which structure
 * a pointer is pointing.
 */

struct Project_Tree_Node_Struct
{
    unsigned short is_leaf;
    BBOX_TREE *Node;
    PROJECT Project;
    unsigned short Entries;
    PROJECT_TREE_NODE **Entry;
};

/// @}
///
//******************************************************************************

}

#endif
