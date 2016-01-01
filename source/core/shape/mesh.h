//******************************************************************************
///
/// @file core/shape/mesh.h
///
/// Declarations related to the mesh geometric primitive.
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

#ifndef POVRAY_CORE_MESH_H
#define POVRAY_CORE_MESH_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/scene/object.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define MESH_OBJECT (PATCH_OBJECT+HIERARCHY_OK_OBJECT) // NOTE: During parsing, the PATCH_OBJECT type flag may be cleared if an inside_vector is specified

typedef struct BBox_Tree_Struct BBOX_TREE;

/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Mesh_Data_Struct MESH_DATA;
typedef struct Mesh_Triangle_Struct MESH_TRIANGLE;

typedef struct Hash_Table_Struct HASH_TABLE;
typedef struct UV_Hash_Table_Struct UV_HASH_TABLE;

// TODO - a SnglVector2d should probably suffice for MeshUVVector, and reduce the Mesh's memory footprint by 8 bytes per triangle.
// TODO - on systems with 64-bit int type, int is probably overkill for MeshIndex; maybe we even want to make Mesh a template, using short for small meshes.

typedef SnglVector3d MeshVector;   ///< Data type used to store vertices and normals.
typedef Vector2d     MeshUVVector; ///< Data type used to store UV coordinates.
typedef signed int   MeshIndex;    ///< Data type used to store indices into vertices / normals / uv coordinate / texture tables. Must be signed and able to hold 2*max.

struct Mesh_Data_Struct
{
    int References;                    ///< Number of references to the mesh.
    MeshIndex Number_Of_UVCoords;      ///< Number of UV coords in the mesh.
    MeshIndex Number_Of_Normals;       ///< Number of normals in the mesh.
    MeshIndex Number_Of_Triangles;     ///< Number of trinagles in the mesh.
    MeshIndex Number_Of_Vertices;      ///< Number of vertices in the mesh.
    MeshVector *Normals, *Vertices;    ///< Arrays of normals and vertices.
    MeshUVVector *UVCoords;            ///< Array of UV coordinates
    MESH_TRIANGLE *Triangles;          ///< Array of triangles.
    BBOX_TREE *Tree;                   ///< Bounding box tree for mesh.
    Vector3d Inside_Vect;              ///< vector to use to test 'inside'
};

struct Mesh_Triangle_Struct
{
    MeshVector Perp;               ///< Vector used for smooth triangles.

    SNGL Distance;                 ///< Distance of triangle along normal.

    MeshIndex Normal_Ind;          ///< Index of unsmoothed triangle normal.
    MeshIndex P1, P2, P3;          ///< Indices of triangle vertices.
    MeshIndex Texture;             ///< Index of triangle texture.
    MeshIndex Texture2, Texture3;  ///< Color Triangle Patch.
    MeshIndex N1, N2, N3;          ///< Indices of smoothed triangle normals.
    MeshIndex UV1, UV2, UV3;       ///< Indicies of UV coordinate vectors

    unsigned int Smooth:1;         ///< Is this a smooth triangle.
    unsigned int Dominant_Axis:2;  ///< Dominant axis.
    unsigned int vAxis:2;          ///< Axis for smooth triangle.
    unsigned int ThreeTex:1;       ///< Color Triangle Patch.
};

struct Hash_Table_Struct
{
    MeshIndex Index;
    MeshVector P;
    HASH_TABLE *Next;
};

struct UV_Hash_Table_Struct
{
    MeshIndex Index;
    MeshUVVector P;
    UV_HASH_TABLE *Next;
};

class Mesh : public ObjectBase
{
    public:
        MESH_DATA *Data;                ///< Mesh data holding triangles.
        MeshIndex Number_Of_Textures;   ///< Number of textures in the mesh.
        TEXTURE **Textures;             ///< Array of texture references.
        bool has_inside_vector;

        Mesh();
        virtual ~Mesh();

        virtual ObjectPtr Copy();

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
        virtual bool Inside(const Vector3d&, TraceThreadData *) const;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;
        virtual void UVCoord(Vector2d&, const Intersection *, TraceThreadData *) const;
        virtual void Translate(const Vector3d&, const TRANSFORM *);
        virtual void Rotate(const Vector3d&, const TRANSFORM *);
        virtual void Scale(const Vector3d&, const TRANSFORM *);
        virtual void Transform(const TRANSFORM *);
        virtual void Compute_BBox();

        void Test_Mesh_Opacity();

        void Create_Mesh_Hash_Tables();
        bool Compute_Mesh_Triangle(MESH_TRIANGLE *Triangle, bool Smooth, Vector3d& P1, Vector3d& P2, Vector3d& P3, Vector3d& S_Normal);
        void Build_Mesh_BBox_Tree();
        bool Degenerate(const Vector3d& P1, const Vector3d& P2, const Vector3d& P3);
        void Init_Mesh_Triangle(MESH_TRIANGLE *Triangle);
        void Destroy_Mesh_Hash_Tables();
        MeshIndex Mesh_Hash_Vertex(MeshIndex *Number_Of_Vertices, MeshIndex *Max_Vertices, MeshVector **Vertices, const Vector3d& Vertex);
        MeshIndex Mesh_Hash_Normal(MeshIndex *Number_Of_Normals, MeshIndex *Max_Normals, MeshVector **Normals, const Vector3d& Normal);
        MeshIndex Mesh_Hash_Texture(MeshIndex *Number_Of_Textures, MeshIndex *Max_Textures, TEXTURE ***Textures, TEXTURE *Texture);
        MeshIndex Mesh_Hash_UV(MeshIndex *Number, MeshIndex *Max, MeshUVVector **Elements, const Vector2d& aPoint);
        void Smooth_Mesh_Normal(Vector3d& Result, const MESH_TRIANGLE *Triangle, const Vector3d& IPoint) const;

        void Determine_Textures(Intersection *, bool, WeightedTextureVector&, TraceThreadData *);
    protected:
        bool Intersect(const BasicRay& ray, IStack& Depth_Stack, TraceThreadData *Thread);
        void Compute_Mesh_BBox();
        void MeshUV(const Vector3d& P, const MESH_TRIANGLE *Triangle, Vector2d& Result) const;
        void compute_smooth_triangle(MESH_TRIANGLE *Triangle, const Vector3d& P1, const Vector3d& P2, const Vector3d& P3);
        bool intersect_mesh_triangle(const BasicRay& ray, const MESH_TRIANGLE *Triangle, DBL *Depth) const;
        bool test_hit(const MESH_TRIANGLE *Triangle, const BasicRay& OrigRay, DBL Depth, DBL len, IStack& Depth_Stack, TraceThreadData *Thread);
        void get_triangle_bbox(const MESH_TRIANGLE *Triangle, BoundingBox *BBox) const;
        bool intersect_bbox_tree(const BasicRay& ray, const BasicRay& Orig_Ray, DBL len, IStack& Depth_Stack, TraceThreadData *Thread);
        bool inside_bbox_tree(const BasicRay& ray, TraceThreadData *Thread) const;
        void get_triangle_vertices(const MESH_TRIANGLE *Triangle, Vector3d& P1, Vector3d& P2, Vector3d& P3) const;
        void get_triangle_normals(const MESH_TRIANGLE *Triangle, Vector3d& N1, Vector3d& N2, Vector3d& N3) const;
        void get_triangle_uvcoords(const MESH_TRIANGLE *Triangle, Vector2d& U1, Vector2d& U2, Vector2d& U3) const;
        static MeshIndex mesh_hash(HASH_TABLE **Hash_Table, MeshIndex *Number, MeshIndex *Max, MeshVector **Elements, const Vector3d& aPoint);

private:
        // these are used temporarily during parsing and are destroyed
        // when the parser has finished constructing the object
        static HASH_TABLE **Vertex_Hash_Table;
        static HASH_TABLE **Normal_Hash_Table;
        static UV_HASH_TABLE **UV_Hash_Table;
};

}

#endif // POVRAY_CORE_MESH_H
