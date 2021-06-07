//******************************************************************************
///
/// @file core/shape/bezier.h
///
/// Declarations related to the Bezier bicubic patch geometric primitive.
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

#ifndef POVRAY_CORE_BEZIER_H
#define POVRAY_CORE_BEZIER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/scene/object.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCoreShape
///
/// @{

//******************************************************************************
///
/// @name Object Types
///
/// @{

#define BICUBIC_PATCH_OBJECT (PATCH_OBJECT)

/// @}
///
//******************************************************************************


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef DBL BEZIER_WEIGHTS[4][4];

struct Bezier_Node_Struct final
{
    int Node_Type;      // Is this an interior node, or a leaf
    Vector3d Center;    // Center of sphere bounding the (sub)patch
    DBL Radius_Squared; // Radius of bounding sphere (squared)
    int Count;          // # of subpatches associated with this node
    void *Data_Ptr;     // Either pointer to vertices or pointer to children
};
using BEZIER_NODE = Bezier_Node_Struct; ///< @deprecated

struct Bezier_Child_Struct final
{
    BEZIER_NODE *Children[4];
};
using BEZIER_CHILDREN = Bezier_Child_Struct; ///< @deprecated

struct Bezier_Vertices_Struct final
{
    float uvbnds[4];
    Vector3d Vertices[4];
};
using BEZIER_VERTICES = Bezier_Vertices_Struct; ///< @deprecated

class BicubicPatch final : public NonsolidObject
{
    public:
        typedef Vector3d ControlPoints[4][4];

        int Patch_Type, U_Steps, V_Steps;
        ControlPoints Control_Points;
        Vector2d ST[4];
        DBL Flatness_Value;
        DBL accuracy;
        BEZIER_NODE *Node_Tree;
        BEZIER_WEIGHTS *Weights;

        BicubicPatch();
        virtual ~BicubicPatch() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void UVCoord(Vector2d&, const Intersection *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;

        void Precompute_Patch_Values();
    protected:
        typedef Vector3d TripleVector3d[3];
        typedef DBL      TripleDouble[3];

        static void bezier_value(const ControlPoints *cp, DBL u0, DBL v0, Vector3d& P, Vector3d& N);
        bool intersect_subpatch(const BasicRay&, const TripleVector3d&, const DBL [3], const DBL [3], DBL *, Vector3d&, Vector3d&, DBL *, DBL *) const;
        static bool spherical_bounds_check(const BasicRay &, const Vector3d& c, DBL);
        int intersect_bicubic_patch0(const BasicRay& , IStack&, TraceThreadData *Thread);
        static DBL point_plane_distance(const Vector3d&, const Vector3d&, DBL);
        static DBL determine_subpatch_flatness(const ControlPoints *);
        bool flat_enough(const ControlPoints *) const;
        static void bezier_bounding_sphere(const ControlPoints *, Vector3d&, DBL *);
        int bezier_subpatch_intersect(const BasicRay&, const ControlPoints *, DBL, DBL, DBL, DBL, IStack&, TraceThreadData *Thread);
        static void bezier_split_left_right(const ControlPoints *, ControlPoints *, ControlPoints *);
        static void bezier_split_up_down(const ControlPoints *, ControlPoints *, ControlPoints *);
        int bezier_subdivider(const BasicRay&, const ControlPoints *, DBL, DBL, DBL, DBL, int, IStack&, TraceThreadData *Thread);
        static void bezier_tree_deleter(BEZIER_NODE *Node);
        BEZIER_NODE *bezier_tree_builder(const ControlPoints *, DBL u0, DBL u1, DBL v0, DBL v1, int depth, int& max_depth_reached);
        int bezier_tree_walker(const BasicRay&, const BEZIER_NODE *, IStack&, TraceThreadData *Thread);
        static BEZIER_NODE *create_new_bezier_node(void);
        static BEZIER_VERTICES *create_bezier_vertex_block(void);
        static BEZIER_CHILDREN *create_bezier_child_block(void);
        static bool subpatch_normal(const Vector3d& v1, const Vector3d& v2, const Vector3d& v3, Vector3d& Result, DBL *d);
        static void Compute_Texture_UV(const Vector2d& p, const Vector2d st[4], Vector2d& t);
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_BEZIER_H
