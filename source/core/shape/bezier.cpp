//******************************************************************************
///
/// @file core/shape/bezier.cpp
///
/// Implementation of the Bezier bicubic patch geometric primitive.
///
/// @author Alexander Enzmann
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/shape/bezier.h"

#include <algorithm>

#include "base/pov_err.h"

#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

const DBL BEZIER_EPSILON = 1.0e-10;
const DBL BEZIER_TOLERANCE = 1.0e-5;



/*****************************************************************************
*
* FUNCTION
*
*   create_new_bezier_node
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

BEZIER_NODE *BicubicPatch::create_new_bezier_node()
{
    BEZIER_NODE *Node = reinterpret_cast<BEZIER_NODE *>(POV_MALLOC(sizeof(BEZIER_NODE), "bezier node"));

    Node->Data_Ptr = NULL;

    return (Node);
}



/*****************************************************************************
*
* FUNCTION
*
*   create_bezier_vertex_block
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

BEZIER_VERTICES *BicubicPatch::create_bezier_vertex_block()
{
    BEZIER_VERTICES *Vertices;

    Vertices = reinterpret_cast<BEZIER_VERTICES *>(POV_MALLOC(sizeof(BEZIER_VERTICES), "bezier vertices"));

    return (Vertices);
}



/*****************************************************************************
*
* FUNCTION
*
*   create_bezier_child_block
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

BEZIER_CHILDREN *BicubicPatch::create_bezier_child_block()
{
    BEZIER_CHILDREN *Children;

    Children = reinterpret_cast<BEZIER_CHILDREN *>(POV_MALLOC(sizeof(BEZIER_CHILDREN), "bezier children"));

    return (Children);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_tree_builder
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

BEZIER_NODE *BicubicPatch::bezier_tree_builder(const ControlPoints *Patch, DBL u0, DBL u1, DBL v0, DBL v1, int depth, int& max_depth_reached)
{
    ControlPoints Lower_Left, Lower_Right;
    ControlPoints Upper_Left, Upper_Right;
    BEZIER_CHILDREN *Children;
    BEZIER_VERTICES *Vertices;
    BEZIER_NODE *Node = create_new_bezier_node();

    if (depth > max_depth_reached)
    {
        max_depth_reached = depth;
    }

    /* Build the bounding sphere for this subpatch. */

    bezier_bounding_sphere(Patch, Node->Center, &(Node->Radius_Squared));

    /*
     * If the patch is close to being flat, then just perform
     * a ray-plane intersection test.
     */

    if (flat_enough(Patch))
    {
        /* The patch is now flat enough to simply store the corners. */

        Node->Node_Type = BEZIER_LEAF_NODE;

        Vertices = create_bezier_vertex_block();

        Vertices->Vertices[0] = (*Patch)[0][0];
        Vertices->Vertices[1] = (*Patch)[0][3];
        Vertices->Vertices[2] = (*Patch)[3][3];
        Vertices->Vertices[3] = (*Patch)[3][0];

        Vertices->uvbnds[0] = u0;
        Vertices->uvbnds[1] = u1;
        Vertices->uvbnds[2] = v0;
        Vertices->uvbnds[3] = v1;

        Node->Data_Ptr = reinterpret_cast<void *>(Vertices);
    }
    else
    {
        if (depth >= U_Steps)
        {
            if (depth >= V_Steps)
            {
                /* We are at the max recursion depth. Just store corners. */

                Node->Node_Type = BEZIER_LEAF_NODE;

                Vertices = create_bezier_vertex_block();

                Vertices->Vertices[0] = (*Patch)[0][0];
                Vertices->Vertices[1] = (*Patch)[0][3];
                Vertices->Vertices[2] = (*Patch)[3][3];
                Vertices->Vertices[3] = (*Patch)[3][0];

                Vertices->uvbnds[0] = u0;
                Vertices->uvbnds[1] = u1;
                Vertices->uvbnds[2] = v0;
                Vertices->uvbnds[3] = v1;

                Node->Count = 0;

                Node->Data_Ptr = reinterpret_cast<void *>(Vertices);
            }
            else
            {
                bezier_split_up_down(Patch, &Lower_Left, &Upper_Left);

                Node->Node_Type = BEZIER_INTERIOR_NODE;

                Children = create_bezier_child_block();

                Children->Children[0] = bezier_tree_builder(&Lower_Left, u0, u1, v0, (v0 + v1) / 2.0, depth + 1, max_depth_reached);
                Children->Children[1] = bezier_tree_builder(&Upper_Left, u0, u1, (v0 + v1) / 2.0, v1, depth + 1, max_depth_reached);

                Node->Count = 2;

                Node->Data_Ptr = reinterpret_cast<void *>(Children);
            }
        }
        else
        {
            if (depth >= V_Steps)
            {
                bezier_split_left_right(Patch, &Lower_Left, &Lower_Right);

                Node->Node_Type = BEZIER_INTERIOR_NODE;

                Children = create_bezier_child_block();

                Children->Children[0] = bezier_tree_builder(&Lower_Left, u0, (u0 + u1) / 2.0, v0, v1, depth + 1, max_depth_reached);
                Children->Children[1] = bezier_tree_builder(&Lower_Right, (u0 + u1) / 2.0, u1, v0, v1, depth + 1, max_depth_reached);

                Node->Count = 2;

                Node->Data_Ptr = reinterpret_cast<void *>(Children);
            }
            else
            {
                bezier_split_left_right(Patch, &Lower_Left, &Lower_Right);

                bezier_split_up_down(&Lower_Left, &Lower_Left, &Upper_Left);

                bezier_split_up_down(&Lower_Right, &Lower_Right, &Upper_Right);

                Node->Node_Type = BEZIER_INTERIOR_NODE;

                Children = create_bezier_child_block();

                Children->Children[0] = bezier_tree_builder(&Lower_Left, u0, (u0 + u1) / 2.0, v0, (v0 + v1) / 2.0, depth + 1, max_depth_reached);
                Children->Children[1] = bezier_tree_builder(&Upper_Left, u0, (u0 + u1) / 2.0, (v0 + v1) / 2.0, v1, depth + 1, max_depth_reached);
                Children->Children[2] = bezier_tree_builder(&Lower_Right, (u0 + u1) / 2.0, u1, v0, (v0 + v1) / 2.0, depth + 1, max_depth_reached);
                Children->Children[3] = bezier_tree_builder(&Upper_Right, (u0 + u1) / 2.0, u1, (v0 + v1) / 2.0, v1, depth + 1, max_depth_reached);

                Node->Count = 4;

                Node->Data_Ptr = reinterpret_cast<void *>(Children);
            }
        }
    }

    return (Node);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_value
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Determine the position and normal at a single coordinate
*   point (u, v) on a Bezier patch.
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::bezier_value(const ControlPoints *cp, DBL u0, DBL  v0, Vector3d& P, Vector3d& N)
{
    const DBL C[] = { 1.0, 3.0, 3.0, 1.0 };
    int i, j;
    DBL c, t, ut, vt;
    DBL u[4], uu[4], v[4], vv[4];
    DBL du[4], duu[4], dv[4], dvv[4];
    DBL squared_u1, squared_v1;
    Vector3d U1, V1;

    /* Calculate binomial coefficients times coordinate positions. */

    u[0] = 1.0; uu[0] = 1.0; du[0] = 0.0; duu[0] = 0.0;
    v[0] = 1.0; vv[0] = 1.0; dv[0] = 0.0; dvv[0] = 0.0;

    for (i = 1; i < 4; i++)
    {
        u[i] = u[i - 1] * u0;  uu[i] = uu[i - 1] * (1.0 - u0);
        v[i] = v[i - 1] * v0;  vv[i] = vv[i - 1] * (1.0 - v0);

        du[i] = i * u[i - 1];  duu[i] = -i * uu[i - 1];
        dv[i] = i * v[i - 1];  dvv[i] = -i * vv[i - 1];
    }

    /* Now evaluate position and tangents based on control points. */

    P  = Vector3d(0.0, 0.0, 0.0);
    U1 = Vector3d(0.0, 0.0, 0.0);
    V1 = Vector3d(0.0, 0.0, 0.0);

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            c = C[i] * C[j];

            ut = u[i] * uu[3 - i];
            vt = v[j] * vv[3 - j];

            t = c * ut * vt;

            P += t * (*cp)[i][j];

            t = c * vt * (du[i] * uu[3 - i] + u[i] * duu[3 - i]);

            U1 += t * (*cp)[i][j];

            t = c * ut * (dv[j] * vv[3 - j] + v[j] * dvv[3 - j]);

            V1 += t * (*cp)[i][j];
        }
    }

    /* Make the normal from the cross product of the tangents. */

    N = cross(U1, V1);

    t = N.lengthSqr();

    squared_u1 = U1.lengthSqr();
    squared_v1 = V1.lengthSqr();
    if (t > (BEZIER_EPSILON * squared_u1 * squared_v1))
    {
        t = 1.0 / sqrt(t);

        N *= t;
    }
    else
    {
        N = Vector3d(1.0, 0.0, 0.0);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   subpatch_normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Calculate the normal to a subpatch (triangle) return the vector
*   <1.0 0.0 0.0> if the triangle is degenerate.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool BicubicPatch::subpatch_normal(const Vector3d& v1, const Vector3d& v2, const Vector3d& v3, Vector3d& Result, DBL *d)
{
    Vector3d V1, V2;
    DBL squared_v1, squared_v2;
    DBL Length;

    V1 = v1 - v2;
    V2 = v3 - v2;

    Result = cross(V1, V2);

    Length = Result.lengthSqr();
    squared_v1 = V1.lengthSqr();
    squared_v2 = V2.lengthSqr();

    if (Length <= (BEZIER_EPSILON * squared_v1 * squared_v2))
    {
        Result = Vector3d(1.0, 0.0, 0.0);

        *d = -v1[X];

        return false;
    }
    else
    {
        Length = sqrt(Length);

        Result /= Length;

        *d = -dot(Result, v1);

        return true;
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   intersect_subpatch
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool BicubicPatch::intersect_subpatch(const BasicRay &ray, const TripleVector3d& V1, const DBL uu[3], const DBL vv[3], DBL *Depth, Vector3d& P, Vector3d& N, DBL *u, DBL *v) const
{
    DBL squared_b0, squared_b1;
    DBL d, n, a, b, r;
    Vector3d Q;
    Vector3d T1;
    Matrix3x3 B, IB;
    Vector3d NN[3];

    B[0] = V1[1] - V1[0];
    B[1] = V1[2] - V1[0];

    B[2] = cross(B[0], B[1]);

    d = B[2].lengthSqr();

    squared_b0 = B[0].lengthSqr();
    squared_b1 = B[1].lengthSqr();
    if (d <= (BEZIER_EPSILON * squared_b1 * squared_b0))
    {
        return false;
    }

    d = 1.0 / sqrt(d);

    B[2] *= d;

    /* Degenerate triangle. */

    if (!MInvers3(B, IB))
    {
        return false;
    }

    d = dot(ray.Direction, IB[2]);

    if (fabs(d) < BEZIER_EPSILON)
    {
        return false;
    }

    Q = V1[0] - ray.Origin;

    n = dot(Q, IB[2]);

    *Depth = n / d;

    if (*Depth < BEZIER_TOLERANCE)
    {
        return false;
    }

    T1 = ray.Direction * (*Depth);

    P = ray.Origin + T1;

    Q = P - V1[0];

    a = dot(Q, IB[0]);
    b = dot(Q, IB[1]);

    if ((a < 0.0) || (b < 0.0) || (a + b > 1.0))
    {
        return false;
    }

    r = 1.0 - a - b;

    bezier_value(&Control_Points, uu[0], vv[0], T1, NN[0]);
    bezier_value(&Control_Points, uu[1], vv[1], T1, NN[1]);
    bezier_value(&Control_Points, uu[2], vv[2], T1, NN[2]);

    N = NN[0] * r
      + NN[1] * a
      + NN[2] * b;

    *u = r * uu[0] + a * uu[1] + b * uu[2];
    *v = r * vv[0] + a * vv[1] + b * vv[2];

    d = N.lengthSqr();

    if (d > BEZIER_EPSILON)
    {
        d = 1.0 / sqrt(d);

        N *= d;
    }
    else
    {
        N = Vector3d(1.0, 0.0, 0.0);
    }

    return true;
}



/*****************************************************************************
*
* FUNCTION
*
*   spherical_bounds_check
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool BicubicPatch::spherical_bounds_check(const BasicRay &ray, const Vector3d& center, DBL radiusSqr)
{
    Vector3d v;
    DBL dist1, dist2;

    v = center - ray.Origin;

    dist1 = v.lengthSqr();

    if (dist1 < radiusSqr)
    {
        /* ray starts inside sphere - assume it intersects. */

        return true;
    }
    else
    {
        dist2 = dot(v, ray.Direction);

        dist2 *= dist2;

        if ((dist2 > 0) && ((dist1 - dist2) <= (radiusSqr + BEZIER_EPSILON) ))
        {
            return true;
        }
    }

    return false;
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_bounding_sphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Find a sphere that bounds all of the control points of a Bezier patch.
*   The values returned are: the center of the bounding sphere, and the
*   square of the radius of the bounding sphere.
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::bezier_bounding_sphere(const ControlPoints *Patch, Vector3d& center, DBL *radiusSqr)
{
    int i, j;
    DBL r0, r1;
    Vector3d v0;

    center = Vector3d(0.0);
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            center += (*Patch)[i][j];
        }
    }

    center /= 16.0;

    r0 = 0.0;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            v0 = (*Patch)[i][j] - center;

            r1 = v0.lengthSqr();

            if (r1 > r0)
            {
                r0 = r1;
            }
        }
    }

    *radiusSqr = r0;
}



/*****************************************************************************
*
* FUNCTION
*
*   Precompute_Patch_Values
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Precompute grid points and normals for a bezier patch.
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::Precompute_Patch_Values()
{
    int max_depth_reached = 0;

    if (Patch_Type == 1)
    {
        if (Node_Tree != NULL)
        {
            bezier_tree_deleter(Node_Tree);
        }

        Node_Tree = bezier_tree_builder(&Control_Points, 0.0, 1.0, 0.0, 1.0, 0, max_depth_reached);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   point_plane_distance
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Determine the distance from a point to a plane.
*
* CHANGES
*
*   -
*
******************************************************************************/

DBL BicubicPatch::point_plane_distance(const Vector3d& p, const Vector3d& n, DBL d)
{
    DBL temp1, temp2;

    temp1 = dot(p, n);

    temp1 += d;

    temp2 = n.length();

    if (fabs(temp2) < EPSILON)
    {
        return (0.0);
    }

    temp1 /= temp2;

    return (temp1);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_subpatch_intersect
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

int BicubicPatch::bezier_subpatch_intersect(const BasicRay &ray, const ControlPoints *Patch, DBL u0, DBL  u1, DBL  v0, DBL  v1, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int cnt = 0;
    TripleVector3d V1;
    DBL u, v, Depth;
    DBL uu[3], vv[3];
    Vector3d P, N;
    Vector2d UV;
    Vector2d uv_point, tpoint;

    V1[0] = (*Patch)[0][0];
    V1[1] = (*Patch)[0][3];
    V1[2] = (*Patch)[3][3];

    uu[0] = u0; uu[1] = u0; uu[2] = u1;
    vv[0] = v0; vv[1] = v1; vv[2] = v1;

    if (intersect_subpatch(ray, V1, uu, vv, &Depth, P, N, &u, &v))
    {
        if (Clip.empty() || Point_In_Clip(P, Clip, Thread))
        {
            /* transform current point from uv space to texture space */
            uv_point[0] = v;
            uv_point[1] = u;
            Compute_Texture_UV(uv_point, ST, tpoint);

            UV[U] = tpoint[0];
            UV[V] = tpoint[1];
            Depth_Stack->push(Intersection(Depth, P, N, UV, this));

            cnt++;
        }
    }

    V1[1] = V1[2];
    V1[2] = (*Patch)[3][0];

    uu[1] = uu[2]; uu[2] = u1;
    vv[1] = vv[2]; vv[2] = v0;

    if (intersect_subpatch(ray, V1, uu, vv, &Depth, P, N, &u, &v))
    {
        if (Clip.empty() || Point_In_Clip(P, Clip, Thread))
        {
            /* transform current point from uv space to texture space */
            uv_point[0] = v;
            uv_point[1] = u;
            Compute_Texture_UV(uv_point, ST, tpoint);

            UV[U] = tpoint[0];
            UV[V] = tpoint[1];
            Depth_Stack->push(Intersection(Depth, P, N, UV, this));

            cnt++;
        }
    }

    return (cnt);
}




/*****************************************************************************
*
* FUNCTION
*
*   bezier_split_left_right
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::bezier_split_left_right(const ControlPoints *Patch, ControlPoints *Left_Patch, ControlPoints *Right_Patch)
{
    int i, j;
    Vector3d Half;
    Vector3d Temp1[4], Temp2[4];

    for (i = 0; i < 4; i++)
    {
        Temp1[0] = (*Patch)[0][i];

        Temp1[1] = midpoint( (*Patch)[0][i], (*Patch)[1][i] );
        Half     = midpoint( (*Patch)[1][i], (*Patch)[2][i] );
        Temp1[2] = midpoint( Temp1[1],       Half );
        Temp2[2] = midpoint( (*Patch)[2][i], (*Patch)[3][i] );
        Temp2[1] = midpoint( Half,           Temp2[2] );
        Temp1[3] = midpoint( Temp1[2],       Temp2[1] );

        Temp2[0] = Temp1[3];
        Temp2[3] = (*Patch)[3][i];

        for (j = 0; j < 4; j++)
        {
            (*Left_Patch)[j][i]  = Temp1[j];
            (*Right_Patch)[j][i] = Temp2[j];
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_split_up_down
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::bezier_split_up_down(const ControlPoints *Patch, ControlPoints *Bottom_Patch, ControlPoints *Top_Patch)
{
    int i, j;
    Vector3d Temp1[4], Temp2[4];
    Vector3d Half;

    for (i = 0; i < 4; i++)
    {
        Temp1[0] = (*Patch)[i][0];

        Temp1[1] = midpoint( (*Patch)[i][0], (*Patch)[i][1] );
        Half     = midpoint( (*Patch)[i][1], (*Patch)[i][2] );
        Temp1[2] = midpoint( Temp1[1],       Half );
        Temp2[2] = midpoint( (*Patch)[i][2], (*Patch)[i][3] );
        Temp2[1] = midpoint( Half,           Temp2[2] );
        Temp1[3] = midpoint( Temp1[2],       Temp2[1] );

        Temp2[0] = Temp1[3];
        Temp2[3] = (*Patch)[i][3];

        for (j = 0; j < 4; j++)
        {
            (*Bottom_Patch)[i][j] = Temp1[j];
            (*Top_Patch)[i][j]    = Temp2[j];
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   determine_subpatch_flatness
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   See how close to a plane a subpatch is, the patch must have at least
*   three distinct vertices. A negative result from this function indicates
*   that a degenerate value of some sort was encountered.
*
* CHANGES
*
*   -
*
******************************************************************************/

DBL BicubicPatch::determine_subpatch_flatness(const ControlPoints *Patch)
{
    int i, j;
    DBL d, dist, temp1;
    Vector3d n;
    Vector3d TempV;
    Vector3d vertices[3];

    vertices[0] = (*Patch)[0][0];
    vertices[1] = (*Patch)[0][3];

    TempV = vertices[0] - vertices[1];

    temp1 = TempV.length();

    if (fabs(temp1) < EPSILON)
    {
        /*
         * Degenerate in the V direction for U = 0. This is ok if the other
         * two corners are distinct from the lower left corner - I'm sure there
         * are cases where the corners coincide and the middle has good values,
         * but that is somewhat pathalogical and won't be considered.
         */

        vertices[1] = (*Patch)[3][3];

        TempV = vertices[0] - vertices[1];

        temp1 = TempV.length();

        if (fabs(temp1) < EPSILON)
        {
            return (-1.0);
        }

        vertices[2] = (*Patch)[3][0];

        TempV = vertices[0] - vertices[1];

        temp1 = TempV.length();

        if (fabs(temp1) < EPSILON)
        {
            return (-1.0);
        }

        TempV = vertices[1] - vertices[2];

        temp1 = TempV.length();

        if (fabs(temp1) < EPSILON)
        {
            return (-1.0);
        }
    }
    else
    {
        vertices[2] = (*Patch)[3][0];

        TempV = vertices[0] - vertices[1];

        temp1 = TempV.length();

        if (fabs(temp1) < EPSILON)
        {
            vertices[2] = (*Patch)[3][3];

            TempV = vertices[0] - vertices[2];

            temp1 = TempV.length();

            if (fabs(temp1) < EPSILON)
            {
                return (-1.0);
            }

            TempV = vertices[1] - vertices[2];

            temp1 = TempV.length();

            if (fabs(temp1) < EPSILON)
            {
                return (-1.0);
            }
        }
        else
        {
            TempV = vertices[1] - vertices[2];

            temp1 = TempV.length();

            if (fabs(temp1) < EPSILON)
            {
                return (-1.0);
            }
        }
    }

    /*
     * Now that a good set of candidate points has been found,
     * find the plane equations for the patch.
     */

    if (subpatch_normal(vertices[0], vertices[1], vertices[2], n, &d))
    {
        /*
         * Step through all vertices and see what the maximum
         * distance from the plane happens to be.
         */

        dist = 0.0;

        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                temp1 = fabs(point_plane_distance((*Patch)[i][j], n, d));

                if (temp1 > dist)
                {
                    dist = temp1;
                }
            }
        }

        return (dist);
    }
    else
    {
/*
        Debug_Info("Subpatch normal failed in determine_subpatch_flatness\n");
*/

        return (-1.0);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   flat_enough
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool BicubicPatch::flat_enough(const ControlPoints *Patch) const
{
    DBL Dist;

    Dist = determine_subpatch_flatness(Patch);

    if (Dist < 0.0)
    {
        return false;
    }
    else
    {
        if (Dist < Flatness_Value)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_subdivider
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

int BicubicPatch::bezier_subdivider(const BasicRay &ray, const ControlPoints *Patch, DBL u0, DBL  u1, DBL  v0, DBL  v1, int recursion_depth, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int cnt = 0;
    DBL ut, vt, radiusSqr;
    ControlPoints Lower_Left, Lower_Right;
    ControlPoints Upper_Left, Upper_Right;
    Vector3d center;

    /*
     * Make sure the ray passes through a sphere bounding
     * the control points of the patch.
     */

    bezier_bounding_sphere(Patch, center, &radiusSqr);

    if (!spherical_bounds_check(ray, center, radiusSqr))
    {
        return (0);
    }

    /*
     * If the patch is close to being flat, then just
     * perform a ray-plane intersection test.
     */

    if (flat_enough(Patch))
        return bezier_subpatch_intersect(ray, Patch, u0, u1, v0, v1, Depth_Stack, Thread);

    if (recursion_depth >= U_Steps)
    {
        if (recursion_depth >= V_Steps)
        {
            return bezier_subpatch_intersect(ray, Patch, u0, u1, v0, v1, Depth_Stack, Thread);
        }
        else
        {
            bezier_split_up_down(Patch, &Lower_Left, &Upper_Left);

            vt = (v1 + v0) / 2.0;

            cnt += bezier_subdivider(ray, &Lower_Left, u0, u1, v0, vt, recursion_depth + 1, Depth_Stack, Thread);
            cnt += bezier_subdivider(ray, &Upper_Left, u0, u1, vt, v1, recursion_depth + 1, Depth_Stack, Thread);
        }
    }
    else
    {
        if (recursion_depth >= V_Steps)
        {
            bezier_split_left_right(Patch, &Lower_Left, &Lower_Right);

            ut = (u1 + u0) / 2.0;

            cnt += bezier_subdivider(ray, &Lower_Left, u0, ut, v0, v1, recursion_depth + 1, Depth_Stack, Thread);
            cnt += bezier_subdivider(ray, &Lower_Right, ut, u1, v0, v1, recursion_depth + 1, Depth_Stack, Thread);
        }
        else
        {
            ut = (u1 + u0) / 2.0;
            vt = (v1 + v0) / 2.0;

            bezier_split_left_right(Patch, &Lower_Left, &Lower_Right);
            bezier_split_up_down(&Lower_Left, &Lower_Left, &Upper_Left) ;
            bezier_split_up_down(&Lower_Right, &Lower_Right, &Upper_Right);

            cnt += bezier_subdivider(ray, &Lower_Left, u0, ut, v0, vt, recursion_depth + 1, Depth_Stack, Thread);
            cnt += bezier_subdivider(ray, &Upper_Left, u0, ut, vt, v1, recursion_depth + 1, Depth_Stack, Thread);
            cnt += bezier_subdivider(ray, &Lower_Right, ut, u1, v0, vt, recursion_depth + 1, Depth_Stack, Thread);
            cnt += bezier_subdivider(ray, &Upper_Right, ut, u1, vt, v1, recursion_depth + 1, Depth_Stack, Thread);
        }
    }

    return (cnt);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_tree_deleter
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::bezier_tree_deleter(BEZIER_NODE *Node)
{
    int i;
    BEZIER_CHILDREN *Children;

    /* If this is an interior node then continue the descent. */

    if (Node->Node_Type == BEZIER_INTERIOR_NODE)
    {
        Children = reinterpret_cast<BEZIER_CHILDREN *>(Node->Data_Ptr);

        for (i = 0; i < Node->Count; i++)
        {
            bezier_tree_deleter(Children->Children[i]);
        }

        POV_FREE(Children);
    }
    else
    {
        if (Node->Node_Type == BEZIER_LEAF_NODE)
        {
            /* Free the memory used for the vertices. */

            POV_FREE(Node->Data_Ptr);
        }
    }

    /* Free the memory used for the node. */

    POV_FREE(Node);
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_tree_walker
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

int BicubicPatch::bezier_tree_walker(const BasicRay &ray, const BEZIER_NODE *Node, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int i, cnt = 0;
    DBL Depth, u, v;
    DBL uu[3], vv[3];
    Vector3d N, P;
    TripleVector3d V1;
    Vector2d UV;
    Vector2d uv_point, tpoint;
    const BEZIER_CHILDREN *Children;
    const BEZIER_VERTICES *Vertices;

    /*
     * Make sure the ray passes through a sphere bounding
     * the control points of the patch.
     */

    if (!spherical_bounds_check(ray, Node->Center, Node->Radius_Squared))
    {
        return (0);
    }

    /*
     * If this is an interior node then continue the descent,
     * else do a check against the vertices.
     */

    if (Node->Node_Type == BEZIER_INTERIOR_NODE)
    {
        Children = reinterpret_cast<const BEZIER_CHILDREN *>(Node->Data_Ptr);

        for (i = 0; i < Node->Count; i++)
        {
            cnt += bezier_tree_walker(ray, Children->Children[i], Depth_Stack, Thread);
        }
    }
    else if (Node->Node_Type == BEZIER_LEAF_NODE)
    {
        Vertices = reinterpret_cast<const BEZIER_VERTICES *>(Node->Data_Ptr);

        V1[0] = Vertices->Vertices[0];
        V1[1] = Vertices->Vertices[1];
        V1[2] = Vertices->Vertices[2];

        uu[0] = Vertices->uvbnds[0];
        uu[1] = Vertices->uvbnds[0];
        uu[2] = Vertices->uvbnds[1];
        vv[0] = Vertices->uvbnds[2];
        vv[1] = Vertices->uvbnds[3];
        vv[2] = Vertices->uvbnds[3];

        /*
         * Triangulate this subpatch, then check for
         * intersections in the triangles.
         */

        if (intersect_subpatch(ray, V1, uu, vv, &Depth, P, N, &u, &v))
        {
            if (Clip.empty() || Point_In_Clip(P, Clip, Thread))
            {
                /* transform current point from uv space to texture space */
                uv_point[0] = v;
                uv_point[1] = u;
                Compute_Texture_UV(uv_point, ST, tpoint);

                UV[U] = tpoint[0];
                UV[V] = tpoint[1];
                Depth_Stack->push(Intersection(Depth, P, N, UV, this));

                cnt++;
            }
        }

        V1[1] = V1[2];
        V1[2] = Vertices->Vertices[3];

        uu[1] = uu[2]; uu[2] = Vertices->uvbnds[1];
        vv[1] = vv[2]; vv[2] = Vertices->uvbnds[2];

        if (intersect_subpatch(ray, V1, uu, vv, &Depth, P, N, &u, &v))
        {
            if (Clip.empty() || Point_In_Clip(P, Clip, Thread))
            {
                /* transform current point from object space to texture space */
                uv_point[0] = v;
                uv_point[1] = u;
                Compute_Texture_UV(uv_point, ST, tpoint);

                UV[U] = tpoint[0];
                UV[V] = tpoint[1];
                Depth_Stack->push(Intersection(Depth, P, N, UV, this));

                cnt++;
            }
        }
    }
    else
    {
        throw POV_EXCEPTION_STRING("Bad Node type in bezier_tree_walker().");
    }

    return (cnt);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_bicubic_patch0
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

int BicubicPatch::intersect_bicubic_patch0(const BasicRay &ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    return (bezier_subdivider(ray, &Control_Points, 0.0, 1.0, 0.0, 1.0, 0, Depth_Stack, Thread));
}



/*****************************************************************************
*
* FUNCTION
*
*   All_Bicubic_Patch_Intersections
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool BicubicPatch::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int Found, cnt = 0;

    Found = false;

    Thread->Stats()[Ray_Bicubic_Tests]++;

    switch (Patch_Type)
    {
        case 0:

            cnt = intersect_bicubic_patch0(ray, Depth_Stack, Thread);

            break;

        case 1:

            cnt = bezier_tree_walker(ray, Node_Tree, Depth_Stack, Thread);

            break;

        default:

            throw POV_EXCEPTION_STRING("Bad patch type in All_Bicubic_Patch_Intersections.");
    }

    if (cnt > 0)
    {
        Thread->Stats()[Ray_Bicubic_Tests_Succeeded]++;

        Found = true;
    }

    return (Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Bicubic_Patch
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   A patch is not a solid, so an inside test doesn't make sense.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool BicubicPatch::Inside(const Vector3d&, TraceThreadData *) const
{
    return false;
}



/*****************************************************************************
*
* FUNCTION
*
*   Bicubic_Patch_Normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    /* Use preocmputed normal. */

    Result = Inter->INormal;
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Bicubic_Patch
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::Translate(const Vector3d& Vector, const TRANSFORM *)
{
    int i, j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            Control_Points[i][j] += Vector;
        }
    }

    Precompute_Patch_Values();

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Bicubic_Patch
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Bicubic_Patch
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::Scale(const Vector3d& Vector, const TRANSFORM *)
{
    int i, j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            Control_Points[i][j] *= Vector;
        }
    }

    Precompute_Patch_Values();

    Compute_BBox();
}




/*****************************************************************************
*
* FUNCTION
*
*   Transform_Bicubic_Patch
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::Transform(const TRANSFORM *tr)
{
    int i, j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            MTransPoint(Control_Points[i][j], Control_Points[i][j], tr);
        }
    }

    Precompute_Patch_Values();

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Bicubic_Patch
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

BicubicPatch::BicubicPatch() : NonsolidObject(BICUBIC_PATCH_OBJECT)
{
    Patch_Type = - 1;

    U_Steps = 0;
    V_Steps = 0;

    Flatness_Value = 0.0;
    accuracy = 0.01;

    Node_Tree = NULL;
    Weights = NULL;

    /*
     * NOTE: Control_Points[4][4] is initialized in Parse_Bicubic_Patch.
     * Bounding_Sphere_Center,Bounding_Sphere_Radius, Normal_Vector[], and
     * IPoint[] are initialized in Precompute_Patch_Values.
     */

    /* set the default uv-mapping coordinates */
    ST[0] = Vector2d(0.0, 0.0);
    ST[1] = Vector2d(1.0, 0.0);
    ST[2] = Vector2d(1.0, 1.0);
    ST[3] = Vector2d(0.0, 1.0);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Bicubic_Patch
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

ObjectPtr BicubicPatch::Copy()
{
    int i, j;
    BicubicPatch *New = new BicubicPatch();
    int m;

    /* Do not do *New = *Old so that Precompute works right */

    New->Patch_Type = Patch_Type;

    New->U_Steps = U_Steps;
    New->V_Steps = V_Steps;

    if (Weights != NULL)
    {
        New->Weights = reinterpret_cast<BEZIER_WEIGHTS *>(POV_MALLOC( sizeof(BEZIER_WEIGHTS),"bicubic patch" ));
        POV_MEMCPY( New->Weights, Weights, sizeof(BEZIER_WEIGHTS) );
    }

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            New->Control_Points[i][j] = Control_Points[i][j];
        }
    }

    New->Flatness_Value = Flatness_Value;

    New->Precompute_Patch_Values();

    /* copy the mapping */
    for (m = 0; m < 4; m++)
    {
        New->ST[m] = ST[m];
    }

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Bicubic_Patch
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

BicubicPatch::~BicubicPatch()
{
    if (Patch_Type == 1)
    {
        if (Node_Tree != NULL)
        {
            bezier_tree_deleter(Node_Tree);
        }
    }

    if (Weights != NULL)
        POV_FREE(Weights);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Bicubic_Patch_BBox
*
* INPUT
*
*   Bicubic_Patch - Bicubic patch
*
* OUTPUT
*
*   Bicubic_Patch
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a bicubic patch.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void BicubicPatch::Compute_BBox()
{
    int i, j;
    Vector3d Min, Max;

    Min = Vector3d(BOUND_HUGE);
    Max = Vector3d(-BOUND_HUGE);

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            Min[X] = min(Min[X], Control_Points[i][j][X]);
            Min[Y] = min(Min[Y], Control_Points[i][j][Y]);
            Min[Z] = min(Min[Z], Control_Points[i][j][Z]);
            Max[X] = max(Max[X], Control_Points[i][j][X]);
            Max[Y] = max(Max[Y], Control_Points[i][j][Y]);
            Max[Z] = max(Max[Z], Control_Points[i][j][Z]);
        }
    }

    Make_BBox_from_min_max(BBox, Min, Max);
}

/*****************************************************************************
*
* FUNCTION
*
*   Bicubic_Patch_UVCoord
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Nathan Kopp
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::UVCoord(Vector2d& Result, const Intersection *Inter, TraceThreadData *Thread) const
{
    /* Use preocmputed uv coordinates. */

    Result = Inter->Iuv;
}


/*****************************************************************************
*
* FUNCTION
*
*   Compute_UV_Point
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Mike Hough
*
* DESCRIPTION
*
*   Transform p from uv space to texture space (point t) using the
*   shape's ST mapping
*
* CHANGES
*
*   -
*
******************************************************************************/

void BicubicPatch::Compute_Texture_UV(const Vector2d& p, const Vector2d st[4], Vector2d& t)
{
    Vector2d u1, u2;

    u1[0] = st[0][0] + p[0] * (st[1][0] - st[0][0]);
    u1[1] = st[0][1] + p[0] * (st[1][1] - st[0][1]);

    u2[0] = st[3][0] + p[0] * (st[2][0] - st[3][0]);
    u2[1] = st[3][1] + p[0] * (st[2][1] - st[3][1]);

    t[0] = u1[0] + p[1] * (u2[0] - u1[0]);
    t[1] = u1[1] + p[1] * (u2[1] - u1[1]);
}

}
