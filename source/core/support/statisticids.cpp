//******************************************************************************
///
/// @file core/support/statisticids.cpp
///
/// Definition of plaintext labels for the various statistics gathered during
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/support/statisticids.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
// POV-Ray header files (core module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/


/*****************************************************************************
* Local typedefs
******************************************************************************/


/*****************************************************************************
* Local variables
******************************************************************************/


/*****************************************************************************
* Local preprocessor defines
******************************************************************************/


/*****************************************************************************
* Global variables
******************************************************************************/

const INTERSECTION_STATS_INFO intersection_stats[kPOVList_Stat_Last] =
{
    { kPOVList_Stat_RBezierTest,        Ray_RBezier_Tests, Ray_RBezier_Tests_Succeeded,
      "Bezier Patch" },
    { kPOVList_Stat_BicubicTest,        Ray_Bicubic_Tests, Ray_Bicubic_Tests_Succeeded,
      "Bicubic Patch" },
    { kPOVList_Stat_BlobTest,           Ray_Blob_Tests, Ray_Blob_Tests_Succeeded,
      "Blob" },
    { kPOVList_Stat_BlobCpTest,         Blob_Element_Tests, Blob_Element_Tests_Succeeded,
      "Blob Component" },
    { kPOVList_Stat_BlobBdTest,         Blob_Bound_Tests, Blob_Bound_Tests_Succeeded,
      "Blob Bound" },
    { kPOVList_Stat_BoxTest,            Ray_Box_Tests, Ray_Box_Tests_Succeeded,
      "Box" },
    { kPOVList_Stat_ConeCylTest,        Ray_Cone_Tests, Ray_Cone_Tests_Succeeded,
      "Cone/Cylinder" },
    { kPOVList_Stat_CSGIntersectTest,   Ray_CSG_Intersection_Tests, Ray_CSG_Intersection_Tests_Succeeded,
      "CSG Intersection" },
    { kPOVList_Stat_CSGMergeTest,       Ray_CSG_Merge_Tests, Ray_CSG_Merge_Tests_Succeeded,
      "CSG Merge" },
    { kPOVList_Stat_CSGUnionTest,       Ray_CSG_Union_Tests, Ray_CSG_Union_Tests_Succeeded,
      "CSG Union" },
    { kPOVList_Stat_DiscTest,           Ray_Disc_Tests, Ray_Disc_Tests_Succeeded,
      "Disc" },
    { kPOVList_Stat_FractalTest,        Ray_Fractal_Tests, Ray_Fractal_Tests_Succeeded,
      "Fractal" },
    { kPOVList_Stat_HFTest,             Ray_HField_Tests, Ray_HField_Tests_Succeeded,
      "Height Field" },
    { kPOVList_Stat_HFBoxTest,          Ray_HField_Box_Tests, Ray_HField_Box_Tests_Succeeded,
      "Height Field Box" },
    { kPOVList_Stat_HFTriangleTest,     Ray_HField_Triangle_Tests, Ray_HField_Triangle_Tests_Succeeded,
      "Height Field Triangle" },
    { kPOVList_Stat_HFBlockTest,        Ray_HField_Block_Tests, Ray_HField_Block_Tests_Succeeded,
      "Height Field Block" },
    { kPOVList_Stat_HFCellTest,         Ray_HField_Cell_Tests, Ray_HField_Cell_Tests_Succeeded,
      "Height Field Cell" },
    { kPOVList_Stat_IsosurfaceTest,     Ray_IsoSurface_Tests, Ray_IsoSurface_Tests_Succeeded,
      "Isosurface" },
    { kPOVList_Stat_IsosurfaceBdTest,   Ray_IsoSurface_Bound_Tests, Ray_IsoSurface_Bound_Tests_Succeeded,
      "Isosurface Container" },
    { kPOVList_Stat_IsosurfaceCacheTest,Ray_IsoSurface_Cache, Ray_IsoSurface_Cache_Succeeded,
      "Isosurface Cache" },
    { kPOVList_Stat_LatheTest,          Ray_Lathe_Tests, Ray_Lathe_Tests_Succeeded,
      "Lathe" },
    { kPOVList_Stat_LatheBdTest,        Lathe_Bound_Tests, Lathe_Bound_Tests_Succeeded,
      "Lathe Bound" },
    { kPOVList_Stat_LemonTest,          Ray_Lemon_Tests, Ray_Lemon_Tests_Succeeded,
      "Lemon" },
    { kPOVList_Stat_MeshTest,           Ray_Mesh_Tests, Ray_Mesh_Tests_Succeeded,
      "Mesh" },
    { kPOVList_Stat_OvusTest,           Ray_Ovus_Tests, Ray_Ovus_Tests_Succeeded,
      "Ovus" },
    { kPOVList_Stat_PlaneTest,          Ray_Plane_Tests, Ray_Plane_Tests_Succeeded,
      "Plane" },
    { kPOVList_Stat_PolygonTest,        Ray_Polygon_Tests, Ray_Polygon_Tests_Succeeded,
      "Polygon" },
    { kPOVList_Stat_PrismTest,          Ray_Prism_Tests, Ray_Prism_Tests_Succeeded,
      "Prism" },
    { kPOVList_Stat_PrismBdTest,        Prism_Bound_Tests, Prism_Bound_Tests_Succeeded,
      "Prism Bound" },
    { kPOVList_Stat_ParametricTest,     Ray_Parametric_Tests, Ray_Parametric_Tests_Succeeded,
      "Parametric" },
    { kPOVList_Stat_ParametricBoxTest,  Ray_Par_Bound_Tests, Ray_Par_Bound_Tests_Succeeded,
      "Parametric Bound" },
    { kPOVList_Stat_QuardicTest,        Ray_Quadric_Tests, Ray_Quadric_Tests_Succeeded,
      "Quadric" },
    { kPOVList_Stat_QuadPolyTest,       Ray_Poly_Tests, Ray_Poly_Tests_Succeeded,
      "Quartic/Poly" },
    { kPOVList_Stat_SphereTest,         Ray_Sphere_Tests, Ray_Sphere_Tests_Succeeded,
      "Sphere" },
    { kPOVList_Stat_SphereSweepTest,    Ray_Sphere_Sweep_Tests, Ray_Sphere_Sweep_Tests_Succeeded,
      "Sphere Sweep" },
    { kPOVList_Stat_SuperellipsTest,    Ray_Superellipsoid_Tests, Ray_Superellipsoid_Tests_Succeeded,
      "Superellipsoid" },
    { kPOVList_Stat_SORTest,            Ray_Sor_Tests, Ray_Sor_Tests_Succeeded,
      "Surface of Revolution" },
    { kPOVList_Stat_SORBdTest,          Sor_Bound_Tests, Sor_Bound_Tests_Succeeded,
      "Surface of Rev. Bound" },
    { kPOVList_Stat_TorusTest,          Ray_Torus_Tests, Ray_Torus_Tests_Succeeded,
      "Torus" },
    { kPOVList_Stat_TorusBdTest,        Torus_Bound_Tests, Torus_Bound_Tests_Succeeded,
      "Torus Bound" },
    { kPOVList_Stat_TriangleTest,       Ray_Triangle_Tests, Ray_Triangle_Tests_Succeeded,
      "Triangle" },
    { kPOVList_Stat_TTFontTest,         Ray_TTF_Tests, Ray_TTF_Tests_Succeeded,
      "True Type Font" },
    { kPOVList_Stat_BoundObjectTest,    Bounding_Region_Tests, Bounding_Region_Tests_Succeeded,
      "Bounding Object" },
    { kPOVList_Stat_ClipObjectTest,     Clipping_Region_Tests, Clipping_Region_Tests_Succeeded,
      "Clipping Object" },
    { kPOVList_Stat_BoundingBoxTest,    nChecked, nEnqueued,
      "Bounding Box" },
    { kPOVList_Stat_LightBufferTest,    LBuffer_Tests, LBuffer_Tests_Succeeded,
      "Light Buffer" },
    { kPOVList_Stat_VistaBufferTest,    VBuffer_Tests, VBuffer_Tests_Succeeded,
      "Vista Buffer" },
    { kPOVList_Stat_Last, MaxIntStat, MaxIntStat, nullptr }
};

}
// end of namespace pov
