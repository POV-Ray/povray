//******************************************************************************
///
/// @file core/support/statisticids.h
///
/// Declarations of identifiers for the various statistics gathered during
/// rendering.
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

#ifndef POVRAY_CORE_STATISTICIDS_H
#define POVRAY_CORE_STATISTICIDS_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
// POV-Ray header files (core module)
//  (none at the moment)

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreSupportStatistics Render Statistics
/// @ingroup PovCore
///
/// @{

// Add new stats ONLY at the end!!!
enum
{
    kPOVList_Stat_BicubicTest = 1,
    kPOVList_Stat_BlobTest,
    kPOVList_Stat_BlobCpTest,
    kPOVList_Stat_BlobBdTest,
    kPOVList_Stat_BoxTest,
    kPOVList_Stat_ConeCylTest,
    kPOVList_Stat_CSGIntersectTest,
    kPOVList_Stat_CSGMergeTest,
    kPOVList_Stat_CSGUnionTest,
    kPOVList_Stat_DiscTest,
    kPOVList_Stat_FractalTest,
    kPOVList_Stat_HFTest,
    kPOVList_Stat_HFBoxTest,
    kPOVList_Stat_HFTriangleTest,
    kPOVList_Stat_HFBlockTest,
    kPOVList_Stat_HFCellTest,
    kPOVList_Stat_IsosurfaceTest,
    kPOVList_Stat_IsosurfaceBdTest,
    kPOVList_Stat_IsosurfaceCacheTest,
    kPOVList_Stat_LatheTest,
    kPOVList_Stat_LatheBdTest,
    kPOVList_Stat_MeshTest,
    kPOVList_Stat_PlaneTest,
    kPOVList_Stat_PolygonTest,
    kPOVList_Stat_PrismTest,
    kPOVList_Stat_PrismBdTest,
    kPOVList_Stat_ParametricTest,
    kPOVList_Stat_ParametricBoxTest,
    kPOVList_Stat_QuardicTest,
    kPOVList_Stat_QuadPolyTest,
    kPOVList_Stat_SphereTest,
    kPOVList_Stat_SphereSweepTest,
    kPOVList_Stat_SuperellipsTest,
    kPOVList_Stat_SORTest,
    kPOVList_Stat_SORBdTest,
    kPOVList_Stat_TorusTest,
    kPOVList_Stat_TorusBdTest,
    kPOVList_Stat_TriangleTest,
    kPOVList_Stat_TTFontTest,
    kPOVList_Stat_BoundObjectTest,
    kPOVList_Stat_ClipObjectTest,
    kPOVList_Stat_BoundingBoxTest,
    kPOVList_Stat_LightBufferTest,
    kPOVList_Stat_VistaBufferTest,
    kPOVList_Stat_RBezierTest,
    kPOVList_Stat_OvusTest,
    kPOVList_Stat_LemonTest,
    kPOVList_Stat_Last
};

typedef enum INTSTATS
{
    /* Computations are performed on these three */
    Number_Of_Pixels = 0,
    Number_Of_Pixels_Supersampled,
    Number_Of_Samples,
    Number_Of_Rays,
    Calls_To_DNoise,
    Calls_To_Noise,
    ADC_Saves,

    /* intersecion stack */
    Istack_overflows,

    /* objects */
    Ray_RBezier_Tests,
    Ray_RBezier_Tests_Succeeded,
    Ray_Bicubic_Tests,
    Ray_Bicubic_Tests_Succeeded,
    Ray_Blob_Tests,
    Ray_Blob_Tests_Succeeded,
    Blob_Element_Tests,
    Blob_Element_Tests_Succeeded,
    Blob_Bound_Tests,
    Blob_Bound_Tests_Succeeded,
    Ray_Box_Tests,
    Ray_Box_Tests_Succeeded,
    Ray_Cone_Tests,
    Ray_Cone_Tests_Succeeded,
    Ray_CSG_Intersection_Tests,
    Ray_CSG_Intersection_Tests_Succeeded,
    Ray_CSG_Merge_Tests,
    Ray_CSG_Merge_Tests_Succeeded,
    Ray_CSG_Union_Tests,
    Ray_CSG_Union_Tests_Succeeded,
    Ray_Disc_Tests,
    Ray_Disc_Tests_Succeeded,
    Ray_Fractal_Tests,
    Ray_Fractal_Tests_Succeeded,
    Ray_HField_Tests,
    Ray_HField_Tests_Succeeded,
    Ray_HField_Box_Tests,
    Ray_HField_Box_Tests_Succeeded,
    Ray_HField_Triangle_Tests,
    Ray_HField_Triangle_Tests_Succeeded,
    Ray_HField_Block_Tests,
    Ray_HField_Block_Tests_Succeeded,
    Ray_HField_Cell_Tests,
    Ray_HField_Cell_Tests_Succeeded,
    Ray_IsoSurface_Tests,
    Ray_IsoSurface_Tests_Succeeded,
    Ray_IsoSurface_Bound_Tests,
    Ray_IsoSurface_Bound_Tests_Succeeded,
    Ray_IsoSurface_Cache,
    Ray_IsoSurface_Cache_Succeeded,
    Ray_Lathe_Tests,
    Ray_Lathe_Tests_Succeeded,
    Lathe_Bound_Tests,
    Lathe_Bound_Tests_Succeeded,
    Ray_Lemon_Tests,
    Ray_Lemon_Tests_Succeeded,
    Ray_Mesh_Tests,
    Ray_Mesh_Tests_Succeeded,
    Ray_Ovus_Tests,
    Ray_Ovus_Tests_Succeeded,
    Ray_Plane_Tests,
    Ray_Plane_Tests_Succeeded,
    Ray_Polygon_Tests,
    Ray_Polygon_Tests_Succeeded,
    Ray_Prism_Tests,
    Ray_Prism_Tests_Succeeded,
    Prism_Bound_Tests,
    Prism_Bound_Tests_Succeeded,
    Ray_Parametric_Tests,
    Ray_Parametric_Tests_Succeeded,
    Ray_Par_Bound_Tests,
    Ray_Par_Bound_Tests_Succeeded,
    Ray_Quadric_Tests,
    Ray_Quadric_Tests_Succeeded,
    Ray_Poly_Tests,
    Ray_Poly_Tests_Succeeded,
    Ray_Sphere_Tests,
    Ray_Sphere_Tests_Succeeded,
    Ray_Sphere_Sweep_Tests,
    Ray_Sphere_Sweep_Tests_Succeeded,
    Ray_Superellipsoid_Tests,
    Ray_Superellipsoid_Tests_Succeeded,
    Ray_Sor_Tests,
    Ray_Sor_Tests_Succeeded,
    Sor_Bound_Tests,
    Sor_Bound_Tests_Succeeded,
    Ray_Torus_Tests,
    Ray_Torus_Tests_Succeeded,
    Torus_Bound_Tests,
    Torus_Bound_Tests_Succeeded,
    Ray_Triangle_Tests,
    Ray_Triangle_Tests_Succeeded,
    Ray_TTF_Tests,
    Ray_TTF_Tests_Succeeded,

    /* crackle cache */
    CrackleCache_Tests,
    CrackleCache_Tests_Succeeded,

    /* bounding etc */
    Bounding_Region_Tests,
    Bounding_Region_Tests_Succeeded,
    Clipping_Region_Tests,
    Clipping_Region_Tests_Succeeded,

    /* isosurface and functions */
    Ray_IsoSurface_Find_Root,
    Ray_Function_VM_Calls,
    Ray_Function_VM_Instruction_Est,

    /* Vista and light buffer */
    VBuffer_Tests,
    VBuffer_Tests_Succeeded,
    LBuffer_Tests,
    LBuffer_Tests_Succeeded,

    /* Media */
    Media_Samples,
    Media_Intervals,

    /* Ray */
    Reflected_Rays_Traced,
    Refracted_Rays_Traced,
    Transmitted_Rays_Traced,
    Internal_Reflected_Rays_Traced,
    Shadow_Cache_Hits,
    Shadow_Rays_Succeeded,
    Shadow_Ray_Tests,

    nChecked,
    nEnqueued,
    totalQueues,
    totalQueueResets,
    totalQueueResizes,
    Polynomials_Tested,
    Roots_Eliminated,

    /* NK phmap */
    Number_Of_Photons_Shot,
    Number_Of_Photons_Stored,
    Number_Of_Global_Photons_Stored,
    Number_Of_Media_Photons_Stored,
    Priority_Queue_Add,
    Priority_Queue_Remove,
    Gather_Performed_Count,
    Gather_Expanded_Count,

    // [CLi] radiosity total stats (all pre- & final traces, all recursion depths)
    Radiosity_ReuseCount,             // ambient value queries satisfied without taking a new sample
    Radiosity_GatherCount,            // number of samples gathered
    Radiosity_UnsavedCount,           // number of samples gathered but not stored in cache
    Radiosity_RayCount,               // number of rays shot to gather samples
    Radiosity_OctreeNodes,            // number of nodes in octree
    Radiosity_OctreeLookups,          // number of blocks examined for sample lookup
    Radiosity_OctreeAccepts0,         // number of blocks accepted by pass & tile id check
    Radiosity_OctreeAccepts1,         // number of blocks accepted by quick out-of-range check
    Radiosity_OctreeAccepts2,         // number of blocks accepted by next more sophisticated check
    Radiosity_OctreeAccepts3,         // number of blocks accepted by next more sophisticated check
    Radiosity_OctreeAccepts4,         // number of blocks accepted by next more sophisticated check
    Radiosity_OctreeAccepts5,         // number of blocks accepted by next more sophisticated check
    // [CLi] radiosity "top level" recursion stats (all pre- & final traces)
    Radiosity_TopLevel_ReuseCount,    // ambient value queries satisfied without taking a new sample
    Radiosity_TopLevel_GatherCount,   // number of samples gathered
    Radiosity_TopLevel_RayCount,      // number of rays shot to gather samples
    // [CLi] radiosity final trace stats (all recursion depths)
    Radiosity_Final_ReuseCount,       // ambient value queries satisfied without taking a new sample
    Radiosity_Final_GatherCount,      // number of samples gathered
    Radiosity_Final_RayCount,         // number of rays shot to gather samples
    // [CLi] radiosity detailed sample stats
    Radiosity_SamplesTaken_PTS1_R0,   // number of samples gathered during pretrace step 1 at recursion depth 0
    Radiosity_SamplesTaken_PTS1_R1,   //  ...
    Radiosity_SamplesTaken_PTS1_R2,
    Radiosity_SamplesTaken_PTS1_R3,
    Radiosity_SamplesTaken_PTS1_R4ff, // number of samples gathered during pretrace step 1 at recursion depth 4 or deeper
    Radiosity_SamplesTaken_PTS2_R0,   //  ...
    Radiosity_SamplesTaken_PTS2_R1,
    Radiosity_SamplesTaken_PTS2_R2,
    Radiosity_SamplesTaken_PTS2_R3,
    Radiosity_SamplesTaken_PTS2_R4ff,
    Radiosity_SamplesTaken_PTS3_R0,
    Radiosity_SamplesTaken_PTS3_R1,
    Radiosity_SamplesTaken_PTS3_R2,
    Radiosity_SamplesTaken_PTS3_R3,
    Radiosity_SamplesTaken_PTS3_R4ff,
    Radiosity_SamplesTaken_PTS4_R0,
    Radiosity_SamplesTaken_PTS4_R1,
    Radiosity_SamplesTaken_PTS4_R2,
    Radiosity_SamplesTaken_PTS4_R3,
    Radiosity_SamplesTaken_PTS4_R4ff,
    Radiosity_SamplesTaken_PTS5ff_R0, // number of samples gathered during pretrace step 5 or deeper at recursion depth 0
    Radiosity_SamplesTaken_PTS5ff_R1, //  ...
    Radiosity_SamplesTaken_PTS5ff_R2,
    Radiosity_SamplesTaken_PTS5ff_R3,
    Radiosity_SamplesTaken_PTS5ff_R4ff,
    Radiosity_SamplesTaken_Final_R0,  // number of samples gathered during final render at recursion depth 0
    Radiosity_SamplesTaken_Final_R1,  //  ...
    Radiosity_SamplesTaken_Final_R2,
    Radiosity_SamplesTaken_Final_R3,
    Radiosity_SamplesTaken_Final_R4ff,
    Radiosity_QueryCount_R0,          // ambient value queries at recursion depth 0
    Radiosity_QueryCount_R1,          // ...
    Radiosity_QueryCount_R2,          // ...
    Radiosity_QueryCount_R3,          // ...
    Radiosity_QueryCount_R4ff,        // ...

    /* Must be the last */
    MaxIntStat

} IntStatsIndex;

typedef enum FPSTATS
{
    Radiosity_Weight_R0 = 0,          // summed-up weight of radiosity sample rays at recursion depth 0 during final trace
    Radiosity_Weight_R1,              //  ...
    Radiosity_Weight_R2,
    Radiosity_Weight_R3,
    Radiosity_Weight_R4ff,

    /* Must be the last */
    MaxFPStat
} FPStatsIndex;

struct intersection_stats_info final
{
    int povms_id;
    IntStatsIndex stat_test_id;
    IntStatsIndex stat_suc_id;
    const char *infotext;
};
using INTERSECTION_STATS_INFO = intersection_stats_info; ///< @deprecated

extern const INTERSECTION_STATS_INFO intersection_stats[];

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_STATISTICIDS_H
