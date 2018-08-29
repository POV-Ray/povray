// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: meshskeleton.inc
//    Last updated: August-2018
//    Description: This file contains macros for working with mesh
//    and creating skeleton from them.

#ifndef(MeshSkeleton_Inc_Temp)
#declare MeshSkeleton_Inc_Temp=version;
#version 3.8;

// macro MeshVertex
// Put a sphere at each vertex of the mesh
// 1st argument: a mesh
// 2nd argument: radius of spheres
// 3rd argument: texture of spheres
#macro MeshVertex( M, R, T )
#for(Vertex_index, 1, get_vertex_count(M))
  sphere{ get_vertex( M, Vertex_index -1 ), R texture { T } }
#end
#end

// macro MeshFace
// Put a sphere at each face of the mesh and connect it to the vertices of the face
// 1st argument: a mesh
// 2nd argument: radius of spheres
// 3rd argument: radius of connecting cylinders
// 4th argument: texture of spheres and connecting cylinders
#macro MeshFace( M, S, R, T )
#for(Face_index, 1, get_triangle_count(M))
#local Vertex_indices = get_vertex_indices( M, Face_index -1 );
#local P1 = get_vertex( M, Vertex_indices.x );
#local P2 = get_vertex( M, Vertex_indices.y );
#local P3 = get_vertex( M, Vertex_indices.z );
#local C = (P1+P2+P3)/3;
sphere { C, S texture { T } }
#if( vlength(P1 -C)>0)
  cylinder { P1, C, R texture { T }}
#end
#if( vlength(P2 -C)>0)
  cylinder { P2, C, R texture { T }}
#end
#if( vlength(P3 -C)>0)
  cylinder { P3, C, R texture { T }}
#end
#end
#end

// macro MeshEdge
// Put a cylinder along each edge of each face
// 1st argument: a mesh
// 2nd argument: radius of cylinders
// 3rd argument: texture of cylinders
#macro MeshEdge( M, R, T )
#for(Face_index, 1, get_triangle_count(M))
#local Vertex_indices = get_vertex_indices( M, Face_index -1 );
#local P1 = get_vertex( M, Vertex_indices.x );
#local P2 = get_vertex( M, Vertex_indices.y );
#local P3 = get_vertex( M, Vertex_indices.z );
#if( vlength(P1 -P2)>0)
  cylinder { P1, P2, R texture { T }}
#end
#if( vlength(P3 -P2)>0)
  cylinder { P2, P3, R texture { T }}
#end
#if( vlength(P3 -P1)>0)
  cylinder { P3, P1, R texture { T }}
#end
#end
#end

#version MeshSkeleton_Inc_Temp;
#end
