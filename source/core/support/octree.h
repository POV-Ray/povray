//******************************************************************************
///
/// @file core/support/octree.h
///
/// Declarations related to the radiosity sample octree.
///
/// @author Jim McElhiney
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

/*
 * Implemented by and (c) 1994 Jim McElhiney, mcelhiney@acm.org or cserve 71201,1326
 * All standard POV distribution rights granted.  All other rights reserved.
*/

#ifndef POVRAY_CORE_OCTREE_H
#define POVRAY_CORE_OCTREE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"
#include "core/support/octree_fwd.h"

// C++ variants of C standard header files
#include <climits>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/fileinputoutput_fwd.h"

// POV-Ray header files (core module)
#include "core/coretypes.h"
#include "core/math/vector.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreSupportOctree Octree
/// @ingroup PovCore
///
/// @{

using namespace pov_base;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

// The addressing scheme of the nodes has an issue near the origin that impedes performance.
// We're working around this by adding a large positive bias when computing node IDs.
#define OT_BIAS 10000000.


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef unsigned short OT_TILE;
#define OT_TILE_MAX USHRT_MAX

using OT_PASS = OctreePass;                             ///< @deprecated
constexpr auto OT_PASS_INVALID = kOctreePassInvalid;    ///< @deprecated
constexpr auto OT_PASS_FIRST   = kOctreePassFirst;      ///< @deprecated
constexpr auto OT_PASS_FINAL   = kOctreePassFinal;      ///< @deprecated
constexpr auto OT_PASS_MAX     = kOctreePassMax;        ///< @deprecated

using OT_DEPTH = OctreeDepth;
constexpr auto OT_DEPTH_MAX    = kOctreeDepthMax;       ///< @deprecated

// Each node in the oct-tree has a (possibly null) linked list of these data blocks off it.
struct ot_block_struct final
{
    // TODO for memory efficiency we could probably use single-precision data types for the vector stuff
    ot_block_struct* next;  // next block in the same node
    Vector3d    Point;
    Vector3d    S_Normal;
    Vector3d    To_Nearest_Surface;
    MathColour  dx, dy, dz; // gradients, not colors, but used only to manipulate colors [trf]
    MathColour  Illuminance;
    SNGL        Brilliance; // surface brilliance for which the sample was computed
    SNGL        Harmonic_Mean_Distance;
    SNGL        Nearest_Distance;
    SNGL        Quality;    // quality of the data from which this sample was aggregated
    OT_TILE     TileId;     // tile in which this sample was taken
    OT_PASS     Pass;       // pass during which this sample was taken (OT_PASS_FINAL for final render)
    OT_DEPTH    Bounce_Depth;
};
using OT_BLOCK = ot_block_struct; ///< @deprecated

// This is the information necessary to name an oct-tree node.
struct ot_id_struct final
{
    int x, y, z;
    int Size;

    ot_id_struct() : x(0), y(0), z(0), Size(0) {}
};
using OT_ID = ot_id_struct; ///< @deprecated

// These are the structures that make up the oct-tree itself, known as nodes
struct ot_node_struct final
{
    OT_ID    Id;
    OT_BLOCK *Values;
    ot_node_struct *Kids[8];

    ot_node_struct() : Id(), Values(nullptr) { for (unsigned int i = 0; i < 8; i ++) { Kids[i] = nullptr; } }
};
using OT_NODE = ot_node_struct; ///< @deprecated

// These are informations the octree reader needs to know
struct ot_read_param_struct final
{
    DBL       RealErrorBound;
};
using OT_READ_PARAM = ot_read_param_struct; ///< @deprecated

// These are informations the octree reader generates
struct ot_read_info_struct final
{
    MathColour  Gather_Total;
    long        Gather_Total_Count;
    DBL         Brightness;
    bool        FirstRadiosityPass;
};
using OT_READ_INFO = ot_read_info_struct; ///< @deprecated

/*****************************************************************************
* Global functions
******************************************************************************/

void ot_ins (OT_NODE **root, OT_BLOCK *new_block, const OT_ID *new_id);
bool ot_dist_traverse (OT_NODE *subtree, const Vector3d& point, int bounce_depth, bool (*func)(OT_BLOCK *block, void *handle1), void *handle2);
void ot_index_sphere (const Vector3d& point, DBL radius, OT_ID *id);
void ot_index_box (const Vector3d& min_point, const Vector3d& max_point, OT_ID *id);
bool ot_save_tree (OT_NODE *root, OStream *fd);
bool ot_write_block (OT_BLOCK *bl, void * handle);
bool ot_free_tree (OT_NODE **root_ptr);
bool ot_read_file (OT_NODE **root, IStream * fd, const OT_READ_PARAM* param, OT_READ_INFO* info);
void ot_newroot (OT_NODE **root_ptr);
void ot_parent (OT_ID *dad, OT_ID *kid);

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_OCTREE_H
