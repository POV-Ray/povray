//******************************************************************************
///
/// @file parser/tesselation.cpp
///
/// This module implements advanced mesh operations for the scene description files.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
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

#include "backend/frame.h"
#include "parser/parser.h"
#include "core/shape/mesh.h"
#include "core/material/pigment.h"
#include "core/shape/sphere.h"
#include "core/shape/cone.h"
#include "core/shape/csg.h"
#include "base/fileutil.h"
#include "core/render/trace.h"
#include "core/math/matrix.h"
#include "core/material/texture.h"
#include "core/material/pattern.h"
#include "core/scene/tracethreaddata.h"


#include <set>
namespace pov_parser
{
   void Parser::StartAddingTriangles(UNDERCONSTRUCTION *das)
  {
    das->tesselationMesh = new Mesh();

    das->number_of_normals = 0;
    das->number_of_triangles = 0;
    das->number_of_vertices = 0;
    das->number_of_textures = 0;
    das->max_normals = 256;
    das->max_vertices = 256;
    das->max_triangles = 256;
    das->max_textures = 16;

    das->Normals = (SnglVector3d*)POV_MALLOC(das->max_normals*sizeof(SnglVector3d),
        "temporary triangle mesh data (NORMALS)");
    das->Triangles = (MESH_TRIANGLE*)POV_MALLOC(das->max_triangles*
        sizeof(MESH_TRIANGLE),
        "temporary triangle mesh data (TRIANGLES)");
    das->Vertices = (SnglVector3d*)POV_MALLOC(das->max_vertices*sizeof(SnglVector3d),
        "temporary triangle mesh data (VERTICES)");
    das->Textures = (TEXTURE **)POV_MALLOC(das->max_textures*sizeof(TEXTURE *), 
        "temporary triangle mesh data (TEXTURE)"); 
    das->tesselationMesh->Create_Mesh_Hash_Tables();
  }

   void Parser::ExpandTrianglesTable(UNDERCONSTRUCTION *das)
  {
    if (das->number_of_triangles >= das->max_triangles)
    {
      if (das->max_triangles >= INT_MAX/2)
      {
        Error("Too many triangles in triangle mesh.\n");
      }
      das->max_triangles *= 2;
      das->Triangles = (MESH_TRIANGLE*)
        POV_REALLOC(das->Triangles,
            das->max_triangles*sizeof(MESH_TRIANGLE),
            "temporary triangle triangle mesh data");
    }
  }

   void Parser::AddVertices(Vector3d& P1, Vector3d& P2, Vector3d& P3,UNDERCONSTRUCTION *das )
  {

    das->tesselationMesh->Init_Mesh_Triangle(&(das->Triangles[das->number_of_triangles]));
    das->Triangles[das->number_of_triangles].P1 =
      das->tesselationMesh->Mesh_Hash_Vertex(&(das->number_of_vertices), &(das->max_vertices),
          &(das->Vertices), P1);
    das->Triangles[das->number_of_triangles].P2 =
      das->tesselationMesh->Mesh_Hash_Vertex(&(das->number_of_vertices), &(das->max_vertices), 
          &(das->Vertices), P2);
    das->Triangles[das->number_of_triangles].P3 =
      das->tesselationMesh->Mesh_Hash_Vertex(&(das->number_of_vertices), &(das->max_vertices), 
          &(das->Vertices), P3);
    das->Triangles[das->number_of_triangles].Texture = -1;
    das->Triangles[das->number_of_triangles].Texture2 = -1;
    das->Triangles[das->number_of_triangles].Texture3 = -1;
    das->Triangles[das->number_of_triangles].ThreeTex = false;


  }

  void Parser::AddTriangle(Vector3d& P1, Vector3d& P2, Vector3d& P3,
      TEXTURE *T1, TEXTURE *T2, TEXTURE *T3,
      UNDERCONSTRUCTION *das)
  {
    Vector3d N;
    Vector3d E1,E2,E3;
    E1 = P1;//E1 = P1;
    E2 = P2;//E2 = P2;
    E3 = P3;//E3 = P3;
    if (!das->tesselationMesh->Degenerate(E1, E2, E3))
    {

      ExpandTrianglesTable(das);

      AddVertices(E1, E2, E3, das);
      if ((T1 != T2)||(T2 != T3)||(T3 != T1))
      {
        das->Triangles[das->number_of_triangles].ThreeTex = true;
        if (!T1)
        {
          if (das->Default_Texture)
          {
            T1 = Copy_Texture_Pointer(das->Default_Texture);
          }
          else
          {
            T1 = Copy_Texture_Pointer(Default_Texture);
          }
        }
        if (!T2)
        {
          if (das->Default_Texture)
          {
            T2 = Copy_Texture_Pointer(das->Default_Texture);
          }
          else
          {
            T2 = Copy_Texture_Pointer(Default_Texture);
          }
        }
        if (!T3)
        {
          if (das->Default_Texture)
          {
            T3 = Copy_Texture_Pointer(das->Default_Texture);
          }
          else
          {
            T3 = Copy_Texture_Pointer(Default_Texture);
          }
        }
      }
      if (T1)
      {
        das->Triangles[das->number_of_triangles].Texture =
          das->tesselationMesh->Mesh_Hash_Texture(&(das->number_of_textures), &(das->max_textures),
              &(das->Textures), T1);                                       
      }
      if ((T2)&&(das->Triangles[das->number_of_triangles].ThreeTex))
      {
        das->Triangles[das->number_of_triangles].Texture2 =
          das->tesselationMesh->Mesh_Hash_Texture(&(das->number_of_textures), &(das->max_textures),
              &(das->Textures), T2);                                       
      }
      if ((T3)&&(das->Triangles[das->number_of_triangles].ThreeTex))
      {
        das->Triangles[das->number_of_triangles].Texture3 =
          das->tesselationMesh->Mesh_Hash_Texture(&(das->number_of_textures), &(das->max_textures),
              &(das->Textures), T3);                                       
      }
      das->tesselationMesh->Compute_Mesh_Triangle(&(das->Triangles[das->number_of_triangles]), 
          false,
          E1, E2, E3, N);
      das->Triangles[das->number_of_triangles].Normal_Ind =
        das->tesselationMesh->Mesh_Hash_Normal(&(das->number_of_normals), &(das->max_normals), 
            &(das->Normals), N);

      das->number_of_triangles++;
    }
  }

  void Parser::AddSmoothTriangle(Vector3d& P1, Vector3d& P2, Vector3d& P3,
      TEXTURE *T1, TEXTURE *T2, TEXTURE *T3,
      Vector3d & N1, Vector3d & N2, Vector3d & N3, UNDERCONSTRUCTION *das)
  {
    DBL l1, l2, l3;
    Vector3d N;
    Vector3d E1,E2,E3;
    int s1,s2,s3;
    E1 = P1;//E1 = P1;
    E2 = P2;//E2 = P2;
    E3 = P3;//E3 = P3;

    l1 = N1.length();// l1 =  N1.length();
    l2 = N2.length();// l2 =  N2.length();
    l3 = N3.length();// l3 =  N3.length();
    if (!das->tesselationMesh->Degenerate(E1, E2, E3))
    {
      ExpandTrianglesTable(das);
      AddVertices(E1, E2, E3,das);

      if ((T1 != T2)||(T2 != T3)||(T3 != T1))
      {
        das->Triangles[das->number_of_triangles].ThreeTex = true;
        if (!T1)
        {
          if (das->Default_Texture)
          {
          T1 = Copy_Texture_Pointer(das->Default_Texture);
          }
          else
          {
            T1 = Copy_Texture_Pointer(Default_Texture);
          }
        }
        if (!T2)
        {
          if (das->Default_Texture)
          {
            T2 = Copy_Texture_Pointer(das->Default_Texture);
          }
          else
          {
            T2 = Copy_Texture_Pointer(Default_Texture);
          }
        }
        if (!T3)
        {
          if (das->Default_Texture)
          {
            T3 = Copy_Texture_Pointer(das->Default_Texture);
          }
          else
          {
            T3 = Copy_Texture_Pointer(Default_Texture);
          }
        }
      }
      if (T1)
      {
        das->Triangles[das->number_of_triangles].Texture =
          das->tesselationMesh->Mesh_Hash_Texture(&(das->number_of_textures), &(das->max_textures),
              &(das->Textures), T1);                                       
      }
      if ((T2)&&(das->Triangles[das->number_of_triangles].ThreeTex))
      {
        das->Triangles[das->number_of_triangles].Texture2 =
          das->tesselationMesh->Mesh_Hash_Texture(&(das->number_of_textures), &(das->max_textures),
              &(das->Textures), T2);                                       
      }
      if ((T3)&&(das->Triangles[das->number_of_triangles].ThreeTex))
      {
        das->Triangles[das->number_of_triangles].Texture3 =
          das->tesselationMesh->Mesh_Hash_Texture(&(das->number_of_textures), &(das->max_textures),
              &(das->Textures), T3);                                       
      }
      das->tesselationMesh->Compute_Mesh_Triangle(&(das->Triangles[das->number_of_triangles]), true,
          E1, E2, E3, N);

      if((l1 != 0.0) && (l2 != 0.0) && (l3 != 0.0))
      {
        N1 /= l1;// VInverseScaleEq(N1, l1);
        N2 /= l2;// VInverseScaleEq(N2, l2);
        N3 /= l3;// VInverseScaleEq(N3, l3);

        l1 = dot( N, N1 );//l1 = dot( N, N1);
        l2 = dot( N, N2 );//l2 = dot( N, N2);
        l3 = dot( N, N3 );//l3 = dot( N, N3); 
        s1 = l1 < 0 ? 1:-1;
        s2 = l2 < 0 ? 1:-1;
        s3 = l3 < 0 ? 1:-1;
        if ((s1 == s2) && (s1 == s3) && (s2 == s3))
        {
          das->Triangles[das->number_of_triangles].N1 = 
            das->tesselationMesh->Mesh_Hash_Normal(&(das->number_of_normals), &(das->max_normals),
                &(das->Normals), N1);
          das->Triangles[das->number_of_triangles].N2 =
            das->tesselationMesh->Mesh_Hash_Normal(&(das->number_of_normals), &(das->max_normals),
                &(das->Normals), N2);
          das->Triangles[das->number_of_triangles].N3 =
            das->tesselationMesh->Mesh_Hash_Normal(&(das->number_of_normals), &(das->max_normals),
                &(das->Normals), N3);
        }
        else
        {
          das->Triangles[das->number_of_triangles].Smooth = false;
        }
      }
      else
      {
        das->Triangles[das->number_of_triangles].Smooth = false;
      }

      das->Triangles[das->number_of_triangles].Normal_Ind =
        das->tesselationMesh->Mesh_Hash_Normal(&(das->number_of_normals), &(das->max_normals),
            &(das->Normals), N);

      das->number_of_triangles++;
    }
  }

   void Parser::DoneAddingTriangles(UNDERCONSTRUCTION *das)
  {
    das->tesselationMesh->Destroy_Mesh_Hash_Tables();

    if (das->number_of_triangles == 0)
    {
      Warning("mesh operation results in 0 triangles. (Perhaps too low accuracy or null offset ?)\n");
    }

    das->tesselationMesh->Data =
      (MESH_DATA*)POV_MALLOC(sizeof(MESH_DATA),
          "tesselation triangle mesh data");
    das->tesselationMesh->Data->References = 1;
    das->tesselationMesh->Data->Tree = NULL;
    das->tesselationMesh->Data->UVCoords = NULL;

    das->tesselationMesh->Data->Normals   = NULL;
    das->tesselationMesh->Data->Triangles = NULL;
    das->tesselationMesh->Data->Vertices  = NULL;
    das->tesselationMesh->Textures  = NULL;

    das->tesselationMesh->Data->Number_Of_UVCoords = 0;
    das->tesselationMesh->Data->Number_Of_Normals = das->number_of_normals;
    das->tesselationMesh->Data->Number_Of_Triangles = das->number_of_triangles;
    das->tesselationMesh->Data->Number_Of_Vertices = das->number_of_vertices;
    das->tesselationMesh->Number_Of_Textures = das->number_of_textures;
    if (das->tesselationMesh->Number_Of_Textures)
    {
      Set_Flag(das->tesselationMesh, MULTITEXTURE_FLAG);
    }

    das->tesselationMesh->Data->Normals =
      (SnglVector3d*)POV_REALLOC(das->Normals,
          das->number_of_normals*sizeof(SnglVector3d),
          "tesselation triangle mesh data (normals)");
    das->tesselationMesh->Data->Triangles =
      (MESH_TRIANGLE*)POV_REALLOC(das->Triangles,
          das->number_of_triangles* sizeof(MESH_TRIANGLE),
          "tesselation triangle mesh data (triangles)");
    das->tesselationMesh->Data->Vertices =
      (SnglVector3d*)POV_REALLOC(das->Vertices,
          das->number_of_vertices*sizeof(SnglVector3d),
          "tesselation triangle mesh data (vertices)");

    das->tesselationMesh->Textures =
      (TEXTURE**)POV_REALLOC(das->Textures,
          das->number_of_textures*sizeof(TEXTURE *),
          "tesselation triangle mesh data (textures)");

  if( (fabs(das->Inside_Vect[X]) < EPSILON) &&  (fabs(das->Inside_Vect[Y]) < EPSILON) &&  (fabs(das->Inside_Vect[Z]) < EPSILON))
  {
    das->tesselationMesh->has_inside_vector=false;
    das->tesselationMesh->Type |= PATCH_OBJECT;
  }
  else
  {
    das->tesselationMesh->Data->Inside_Vect = das->Inside_Vect.normalized();
    das->tesselationMesh->has_inside_vector=true;
    das->tesselationMesh->Type &= ~PATCH_OBJECT;
  }

    das->tesselationMesh->Compute_BBox();
    Parse_Object_Mods((ObjectPtr)das->tesselationMesh);
    das->tesselationMesh->Build_Mesh_BBox_Tree();
  }
  /*----------------------------------------------------------------------
    GTS routines (Note: GTS is LGPL)
    ----------------------------------------------------------------------*/
   ObjectPtr  Parser::Gts_Load_Object(char *filename, GTSInfo *info,
      UNDERCONSTRUCTION *das)
   {
     unsigned int number_of_point;
     unsigned int number_of_edge;
     unsigned int number_of_face;
     unsigned int cursor;
     UCS2String ign;
     int got;
     double xx,yy,zz;
     unsigned int first,second;  
     unsigned int third;
     unsigned int v1,v2,v3;
     Vector3d * vertice;
     GTS_Edge * edge;

     IStream *filep;

     if ((filep = 
           Locate_File(ASCIItoUCS2String(filename).c_str(),POV_File_Data_GTS,ign,true)
         ) == NULL
        )
     {
       Error("Error opening GTS file.\n");
       return NULL;
     }
     char buffer[500];
     bool ok;
     ok = filep->getline( buffer, sizeof(buffer)-1);
     if (ok)
     {
     got = sscanf(buffer,"%u %u %u%*[^\n]",&number_of_point,&number_of_edge,
         &number_of_face);
     }
     if ((!ok)||(got < 3))
     {
       delete filep;
       Error("Error reading the first line (summary) of GTS file.\n");
       return NULL;
     }
     vertice = (Vector3d *)POV_MALLOC(number_of_point*sizeof(Vector3d),
         "temporary vertices data from GTS");
     edge = (GTS_Edge *)POV_MALLOC(number_of_edge*sizeof(GTS_Edge),
         "temporary edge data from GTS");
     for(cursor =0; cursor < number_of_point; cursor++)
     {
       ok = filep->getline( buffer, sizeof(buffer)-1);
       if (ok)
       {
       got = sscanf(buffer,"%lg %lg %lg%*[^\n]",&xx,&yy,&zz);
       }
       if ((!ok)||(got < 3))
       {
         delete filep;
         POV_FREE(vertice);
         POV_FREE(edge);
         Error("Error in reading vertice %d from GTS file.\n",cursor+1);
         return NULL;
       }
       if (info->reverse)
       {
         xx *= -1; 
       }
       vertice[cursor] = Vector3d(xx,yy,zz);
     }
     for(cursor=0;cursor <number_of_edge; cursor++)
     {
       ok = filep->getline( buffer, sizeof(buffer)-1);
       if (ok)
       {
       got = sscanf(buffer,"%i %i%*[^\n]",&first,&second);
       }
       if ((!ok)||(got < 2))
       { 
         delete filep;
         POV_FREE(vertice);
         POV_FREE(edge);
         Error("Error in reading edge %d from GTS file.(%d %d %d)\n",
             cursor+1,got,first,second);
         return NULL;
       } 
       edge[cursor].first = first;
       edge[cursor].second = second;
     }
     for(cursor = 0;cursor < number_of_face; cursor++)
     {
       ok = filep->getline(buffer, sizeof(buffer)-1);
       got = sscanf(buffer,"%i %i %i%*[^\n]",&first,&second,&third);
       if ((!ok)||(got < 3))
       {
         delete filep;
         POV_FREE(vertice);
         POV_FREE(edge);
         Error("Error in reading face %d from GTS file.\n",cursor+1);
         return NULL;
       }
       v1 = edge[first-1].first-1;
       v2 = edge[first-1].second-1;
       v3 = edge[second-1].first-1;
       if ((v3 == v2)||(v3 == v1))
       {
         v3 = edge[second-1].second-1;
       }
       AddTriangle(vertice[v1],vertice[v2],vertice[v3],NULL,NULL,NULL,das); 
       // Caveat: AddTriangle will slow down due to the lookup for the normal in the hashed normal array!
     }
     delete filep;
     POV_FREE(vertice);
     POV_FREE(edge);                                                         
     return (ObjectPtr )das->tesselationMesh;
   }
   void Parser::Gts_Save_Object(char *filename, GTSInfo *info,
      UNDERCONSTRUCTION *das)
   {
     Mesh *meshobj = (Mesh*)info->object;
     unsigned int number_of_point;
     unsigned int number_of_edge;
     unsigned int max_edge;
     unsigned int number_of_face;
     unsigned int cursor;
     unsigned int ind;
     unsigned int first,second;  
     unsigned int third,swappe;
     OStream *filep;
     int found1,found2,found3;

     number_of_point = meshobj->Data->Number_Of_Vertices;
     number_of_face = meshobj->Data->Number_Of_Triangles;
     number_of_edge = 0;
         typedef std::set<GTS_Edge> EdgeSet;// most std::set implementations would use a binary tree, far better than a home-made linear search
     EdgeSet edge;
         // generate the list of edges (something not in the mesh object)
     for(cursor = 0;cursor < number_of_face;cursor++)
     {
       first = meshobj->Data->Triangles[cursor].P1;
       second = meshobj->Data->Triangles[cursor].P2;
       third = meshobj->Data->Triangles[cursor].P3;
       if (first > second)
       {
         swappe = first;
         first = second;
         second = swappe;
       }
       if (second > third)
       {
         swappe = third;
         third = second;
         second = swappe;
       }
       if (first > second)
       {
         swappe = second;
         second = first;
         first = swappe;
       }
             /*
              * order is now: first < second < third
              *
              * expected : duplicated edge (already in set) are not inserted.
              */
             edge.insert(GTS_Edge(first,second));
             edge.insert(GTS_Edge(first,third));
             edge.insert(GTS_Edge(second,third));
     }
         
     if ((filep = CreateFile(ASCIItoUCS2String(filename).c_str(),POV_File_Data_GTS,false) ) == NULL
        )
     {
       Error("Error opening GTS file.\n");
       return;
     }
     filep->printf("%u %u %u\n",number_of_point,static_cast<unsigned int>(edge.size()),number_of_face);
     for(cursor =0; cursor < number_of_point; cursor++)
     {
       filep->printf("%.10g %.10g %.10g\n",
           meshobj->Data->Vertices[cursor][X],
           meshobj->Data->Vertices[cursor][Y],
           meshobj->Data->Vertices[cursor][Z]);
     }
         /* Avoid O(N*N) cost (looking at the index of each edge for each triangle) by generating the
          * index as the attribute of a map (so the final cost is O(N*log(N)) )
          */
         typedef std::map<GTS_Edge, size_t> EdgeMap;
         EdgeMap edgemap;
         size_t counter=0;
     for(EdgeSet::iterator ed= edge.begin();ed != edge.end();++ed)
     {
       filep->printf("%u %u\n",ed->first+1,ed->second+1);
             edgemap[*ed]=counter++;
     }
         edge.clear();// a bit late to reclaim memory, but let's do it now that we can forget about that set.
     for(cursor = 0;cursor < number_of_face; cursor++)
     {
       first = meshobj->Data->Triangles[cursor].P1;
       second = meshobj->Data->Triangles[cursor].P2;
       third = meshobj->Data->Triangles[cursor].P3;
       if (first > second)
       {
         swappe = first;
         first = second;
         second = swappe;
       }
       if (second > third)
       {
         swappe = third;
         third = second;
         second = swappe;
       }
       if (first > second)
       {
         swappe = second;
         second = first;
         first = swappe;
       }
             // once again, first < second < third
             found1 = edgemap.at( GTS_Edge(first,second));
             found2 = edgemap.at( GTS_Edge(first,third));
             found3 = edgemap.at( GTS_Edge(second,third));
       filep->printf("%i %i %i\n",found1+1,found2+1,found3+1);
     }
     delete filep;
   }
/* STL is in little endian 
 * alas, hton* and ntoh* are not usable, as they do nothing on big endian
 * and invert bytes in little endian... we need the opposite
 *
 * Note: PDP and other fancy ordering is not handled.
 * 
 * Also unchecked & assumed : float are 4 bytes long and in same order as int32
 */
#define TES_LITTLE_ENDIAN 0x41424344UL 
#define TES_BIG_ENDIAN    0x44434241UL
#define TES_PDP_ENDIAN    0x42414443UL
#define TES_ENDIAN_ORDER  ('ABCD') 
#if TES_ENDIAN_ORDER == TES_BIG_ENDIAN
#define SWAP_2(x) ( (((x) & 0xff) << 8) | ((unsigned short)(x) >> 8) )
#define SWAP_4(x) ( ((x) << 24) | \
         (((x) << 8) & 0x00ff0000) | \
         (((x) >> 8) & 0x0000ff00) | \
         ((x) >> 24) )
#define FIX_SHORT(x) (*(unsigned short *)&(x) = SWAP_2(*(unsigned short *)&(x)))
#define FIX_INT(x)   (*(unsigned int *)&(x)   = SWAP_4(*(unsigned int *)&(x)))
#define FIX_FLOAT(x) FIX_INT(x)
#elif TES_ENDIAN_ORDER == TES_LITTLE_ENDIAN
#define FIX_FLOAT(x)
#define FIX_SHORT(x)
#define FIX_INT(x)
#define SWAP_2(x)
#define SWAP_4(x)
#else
#error "Not supported, yet ?"
#endif
// read a binary STL file
   ObjectPtr Parser::Stl_Load_Object(char * filename, STLInfo *info, UNDERCONSTRUCTION *das)
   {
     uint_least32_t numberFace;
     STL_Entry entry;
     IStream *filep;
     Vector3d v1,v2,v3;
     UCS2String ign;

     if ((filep = 
           Locate_File(ASCIItoUCS2String(filename).c_str(),POV_File_Data_STL,ign,true)
         ) == NULL
        )
     {
       Error("Error opening STL file.\n");
       return NULL;
     }
     filep->ignore(80);// ignore header, no magic to check the file type
     filep->read( &numberFace, 4 );
     FIX_INT(numberFace);
     for(size_t i=0;i<numberFace;++i)
     {
       if (!filep->eof())
       {
         filep->read(&entry, 50);
         FIX_FLOAT(entry.normal[0]);
         FIX_FLOAT(entry.normal[1]);
         FIX_FLOAT(entry.normal[2]);
         FIX_FLOAT(entry.vertex1[0]);
         FIX_FLOAT(entry.vertex1[1]);
         FIX_FLOAT(entry.vertex1[2]);
         FIX_FLOAT(entry.vertex2[0]);
         FIX_FLOAT(entry.vertex2[1]);
         FIX_FLOAT(entry.vertex2[2]);
         FIX_FLOAT(entry.vertex3[0]);
         FIX_FLOAT(entry.vertex3[1]);
         FIX_FLOAT(entry.vertex3[2]);
         FIX_SHORT(entry.attribute);
         if (info->reverse)
         {
           entry.vertex1[0]*=-1.0;
           entry.vertex2[0]*=-1.0;
           entry.vertex3[0]*=-1.0;
         }
         //v1 = Vector3d( entry.vertex1[0], entry.vertex1[1], entry.vertex1[2]);
         v1 = Vector3d( entry.vertex1[0], entry.vertex1[1], entry.vertex1[2]);
         //v2 = Vector3d( entry.vertex2[0], entry.vertex2[1], entry.vertex2[2]);
         v2 = Vector3d( entry.vertex2[0], entry.vertex2[1], entry.vertex2[2]);
         //v3 = Vector3d( entry.vertex3[0], entry.vertex3[1], entry.vertex3[2]);
         v3 = Vector3d( entry.vertex3[0], entry.vertex3[1], entry.vertex3[2]);
         AddTriangle(v1, v2, v3, NULL,NULL,NULL, das);
         // Caveat: AddTriangle will slow down due to the lookup for duplicated vectors as mesh grows
       }
     }
     delete filep;
     return (ObjectPtr )das->tesselationMesh;

   }
   void Parser::Stl_Save_Object(char *filename, STLInfo *info)
   {
     Mesh *meshobj = (Mesh*)info->object;
     uint_least32_t number_of_face,write_nf;
     uint_least32_t cursor;
     long first,second;  
     long third;
     OStream *filep;
         STL_Entry entry;
         float of[3];
         
     int found1,found2,found3;

     number_of_face = meshobj->Data->Number_Of_Triangles;
     if ((filep = CreateFile(ASCIItoUCS2String(filename).c_str(),POV_File_Data_STL,false) ) == NULL
        )
     {
       Error("Error opening STL file.\n");
       return;
     }
         filep->printf("POVRAY %-73.73s",filename);
         write_nf = number_of_face;
         FIX_INT(write_nf);
         filep->write(&write_nf, 4);
         memset(&entry,0,sizeof(entry));
         of[0] = meshobj->Data->Tree->BBox.GetMinX();
         of[1] = meshobj->Data->Tree->BBox.GetMinY();
         of[2] = meshobj->Data->Tree->BBox.GetMinZ();
     for(cursor = 0;cursor < number_of_face; cursor++)
     {
             /* let normal be recomputed by loader, otherwise we need unit vector to outside */
             entry.normal[0] = 0;
             entry.normal[1] = 0;
             entry.normal[2] = 0;
             /* no choice, no color, between both variants of STL */
             entry.attribute = 0;
             /* get actual data */
       first = meshobj->Data->Triangles[cursor].P1;
       second = meshobj->Data->Triangles[cursor].P2;
       third = meshobj->Data->Triangles[cursor].P3;
             entry.vertex1[0] = (meshobj->Data->Vertices[first][X])-of[0];
             entry.vertex1[1] = (meshobj->Data->Vertices[first][Y])-of[1];
             entry.vertex1[2] = (meshobj->Data->Vertices[first][Z])-of[2];
             entry.vertex2[0] = (meshobj->Data->Vertices[second][X])-of[0];
             entry.vertex2[1] = (meshobj->Data->Vertices[second][Y])-of[1];
             entry.vertex2[2] = (meshobj->Data->Vertices[second][Z])-of[2];
             entry.vertex3[0] = (meshobj->Data->Vertices[third][X])-of[0];
             entry.vertex3[1] = (meshobj->Data->Vertices[third][Y])-of[1];
             entry.vertex3[2] = (meshobj->Data->Vertices[third][Z])-of[2];
             FIX_FLOAT(entry.normal[0]);
             FIX_FLOAT(entry.normal[1]);
             FIX_FLOAT(entry.normal[2]);
             FIX_FLOAT(entry.vertex1[0]);
             FIX_FLOAT(entry.vertex1[1]);
             FIX_FLOAT(entry.vertex1[2]);
             FIX_FLOAT(entry.vertex2[0]);
             FIX_FLOAT(entry.vertex2[1]);
             FIX_FLOAT(entry.vertex2[2]);
             FIX_FLOAT(entry.vertex3[0]);
             FIX_FLOAT(entry.vertex3[1]);
             FIX_FLOAT(entry.vertex3[2]);
             FIX_SHORT(entry.attribute);
             filep->write(&entry, 50);
     }
     delete filep;
   }
#undef FIX_SHORT
#undef FIX_FLOAT
#undef FIX_INT
#undef SWAP_4
#undef SWAP_2
#undef TES_LITTLE_ENDIAN
#undef TES_BIG_ENDIAN
#undef TES_PDP_ENDIAN
#undef TES_ENDIAN_ORDER
  /*----------------------------------------------------------------------
    Tesselation routine
    ----------------------------------------------------------------------*/


   void Parser::AssignTCoord(TetraCoordInfo* info, int cInd,
      DBL xc, DBL yc, DBL zc)
  {
    info->Coord[cInd][X] = xc;
    info->Coord[cInd][Y] = yc;
    info->Coord[cInd][Z] = zc;
    info->Inside[cInd] = Inside_Object(info->Coord[cInd], info->Object, GetParserDataPtr());
  }

   void Parser::CopyTCoord(TetraCoordInfo* info, int sInd, int dInd)
  {
    info->Coord[dInd] = info->Coord[sInd];
    info->Inside[dInd] = info->Inside[sInd];
  }

  void Parser::CopyIntersection(TetraCoordInfo* info,
      int sInd1, int sInd2, int dInd1, int dInd2)
  {
    if((info->Inside[sInd1] && !info->Inside[sInd2]) ||
        (!info->Inside[sInd1] && info->Inside[sInd2]))
    {
      if(info->Inside[sInd1])
      {
        info->Intersection[dInd2][dInd1][0] = info->Intersection[sInd2][sInd1][0];
        info->Intersection[dInd2][dInd1][1] = info->Intersection[sInd2][sInd1][1];
        info->Texture[dInd2][dInd1] = info->Texture[sInd2][sInd1]; 
      }
      else
      {
        info->Intersection[dInd1][dInd2][0] = info->Intersection[sInd1][sInd2][0];
        info->Intersection[dInd1][dInd2][1] = info->Intersection[sInd1][sInd2][1];
        info->Texture[dInd1][dInd2] = info->Texture[sInd1][sInd2]; 
      }
    }
  }

bool Find_TesselIntersection(Intersection *isect, DBL len, ObjectPtr object, const Ray& ray, TraceThreadData *threadData)
{
    DBL closest = len*1.11;//caller moved back the origin by 10%, and let's tolerate additional 1% for the end
    if( object->Bound.empty() == false )
    {
        if( Ray_In_Bound( ray, object->Bound, threadData ) == false )
        { return false; }
    }

    IStack depthstack( threadData->stackPool );
    assert( depthstack->empty() ); // verify that the IStack pulled from the pool is in a cleaned-up condition

    if( object->All_Intersections( ray, depthstack, threadData ) )
    {
        bool found = false;
        double tmpDepth = 0;

        while( depthstack->size() > 0 )
        {
            tmpDepth = depthstack->top().Depth;

            if( ( tmpDepth < closest ) && ( tmpDepth > 0 ) )
            {
                *isect = depthstack->top();
                closest = tmpDepth;
                found = true;
            }

            depthstack->pop();
        }

        return ( found == true );
    }

    assert( depthstack->empty() ); // verify that the IStack is in a cleaned-up condition (again)
    return false;
}

  static Intersection tess_intersect;

   void Parser::CalcIntersection(TetraCoordInfo* info, int P1ind, int P2ind,
      int Smooth)
  {
    if((info->Inside[P1ind] && !info->Inside[P2ind]) ||
        (!info->Inside[P1ind] && info->Inside[P2ind]))
    {
      int ins = info->Inside[P1ind] ? P1ind : P2ind;
      int outs = info->Inside[P1ind] ? P2ind : P1ind;
      Vector3d Dir = {
        info->Coord[ins][X] - info->Coord[outs][X],
        info->Coord[ins][Y] - info->Coord[outs][Y],
        info->Coord[ins][Z] - info->Coord[outs][Z] };
            DBL len ;
            len = Dir.length();
            // Move back by 10% of the direction
      Vector3d SP = { info->Coord[outs][X] - Dir[X]*len*.1,
        info->Coord[outs][Y] - Dir[Y]*len*.1,
        info->Coord[outs][Z] - Dir[Z]*len*.1 };

      TraceTicket ticket( 1, 0.0);
      Ray tess_Ray( ticket );
      info->Texture[outs][ins] = NULL;
      tess_Ray.Origin = SP;
      tess_Ray.Direction =  Dir;
      tess_Ray.Direction.normalize();
      if (Find_TesselIntersection(&tess_intersect, len, (ObjectPtr)info->Object,tess_Ray, GetParserDataPtr()))
      {
        info->Intersection[outs][ins][0] = tess_intersect.IPoint;
        if(Smooth)
        {  
          info->Intersection[outs][ins][1] = tess_intersect.INormal;
          //Normal(info->Intersection[outs][ins][1], tess_intersect.Object, &tess_intersect);
        }
        if ((tess_intersect.Object)
            &&(tess_intersect.Object->Texture))
        {
          /* Just keep a reference for now, a copy might be done
           ** only when adding the real triangle 
           */
          info->Texture[outs][ins] = tess_intersect.Object->Texture;
        }
      }
      else
      { /* Cheating */
        /* Because inside test said one was out and the other was in 
         ** So, There MUST be some surface between them
         ** the fact that a ray does not find it is no reason to
         ** get mad 
         */
        info->Intersection[outs][ins][0] = info->Coord[ins];
        Dir *= -1/2;
        info->Intersection[outs][ins][0] += Dir;
        if (Smooth)
        {
          info->Intersection[outs][ins][1] = Vector3d(0,0,0);
        }

      }
    }
  }

   void Parser::TesselateTetrahedron(TetraCoordInfo* info,
      int P1ind, int P2ind, int P3ind, int P4ind,
      int Smooth, UNDERCONSTRUCTION *das)
  {
    int Pind[4] = { P1ind, P2ind, P3ind, P4ind };
    int inside[4];
    int outside[4];
    int inside_amnt = 0, outside_amnt = 0, i, j;

    for(i=0; i<4; ++i)
    {
      if(info->Inside[Pind[i]]) { inside[inside_amnt++] = Pind[i]; }
      else { outside[outside_amnt++] = Pind[i]; }
    }

    if(inside_amnt > 0 && outside_amnt > 0)
    {
      Vector3d Coords[4], Normals[4];
      TEXTURE *Textu[4];
      memset(Textu,0,sizeof(Textu));

      switch(inside_amnt)
      {
        /* 3 outside, 1 inside */
        /*=====================*/
        case 1:
          for(i=0; i<3; ++i)
          {
            Coords[i] = info->Intersection[outside[i]][inside[0]][0];
            if(Smooth)
            {
              Normals[i] = info->Intersection[outside[i]][inside[0]][1];
            }
            if (!(das->albinos))
            {
              Textu[i] = Copy_Texture_Pointer(info->Texture[outside[i]][inside[0]]);
            } else
            {
              Textu[i] = NULL;
            }
          }

          if(Smooth)
          {
            AddSmoothTriangle(Coords[0], Coords[1], Coords[2],
                Textu[0],Textu[1],Textu[2],
                Normals[0], Normals[1], Normals[2],das);
          }
          else
          {
            AddTriangle(Coords[0], Coords[1], Coords[2],
                Textu[0],Textu[1],Textu[2],das);
          }
          break;

          /* 2 outside, 2 inside */
          /*=====================*/
        case 2:
          for(i=0; i<2; ++i)
            for(j=0; j<2; ++j)
            {
              Coords[i*2+j] = info->Intersection[outside[j]][inside[i]][0];
              if(Smooth)
              {
                Normals[i*2+j] = info->Intersection[outside[j]][inside[i]][1];
              }
              if (!(das->albinos))
              {
                Textu[i*2+j] = Copy_Texture_Pointer(info->Texture[outside[j]][inside[i]]);
              } else {
                Textu[i*2+j] = NULL;
              }
            }

          if(Smooth)
          {
            Vector3d Coord0,Coord3;
            Vector3d Normal0, Normal3;
            Coord0=Coords[0];
            Coord3=Coords[3];
            Normal0=Normals[0];
            Normal3=Normals[3];
            AddSmoothTriangle(Coords[1], Coords[0], Coords[3],
                Textu[1],Textu[0],Textu[3],
                Normals[1], Normals[0], Normals[3],das);
            AddSmoothTriangle(Coords[2], Coord0, Coord3,
                Textu[2],Textu[0],Textu[3],
                Normals[2], Normal0, Normal3,das);
          }
          else
          {
            Vector3d Coord0,Coord3;
            Coord0=Coords[0];
            Coord3=Coords[3];
            AddTriangle(Coords[1], Coords[0], Coords[3],
                Textu[1],Textu[0],Textu[3],das);
            AddTriangle(Coords[2], Coord0, Coord3,
                Textu[2],Textu[0],Textu[3],das);
          }
          break;

          /* 1 outside, 3 inside */
          /*=====================*/
        case 3:
          for(i=0; i<3; ++i)
          {
            Coords[i] = info->Intersection[outside[0]][inside[i]][0];
            if(Smooth)
            {
              Normals[i] = info->Intersection[outside[0]][inside[i]][1];
            }
            if (!(das->albinos))
            {
              Textu[i] = Copy_Texture_Pointer(info->Texture[outside[0]][inside[i]]);
            } else {
              Textu[i] = NULL;
            }
          }

          if(Smooth)
          {
            AddSmoothTriangle(Coords[0], Coords[1], Coords[2],
                Textu[0],Textu[1],Textu[2],
                Normals[0], Normals[1], Normals[2],das);
          }
          else
          {
            AddTriangle(Coords[0], Coords[1], Coords[2],
                Textu[0],Textu[1],Textu[2],das);
          }
          break;
      }
    }
  }

   ObjectPtr Parser::Tesselate_Object(ObjectPtr Obj,
      int XAccuracy, int YAccuracy, int ZAccuracy,
      int Sm, DBL BBoxOffs, UNDERCONSTRUCTION *das)
  {
    int IndX, IndY, IndZ;
    TetraCoordInfo info ;


    Obj->Compute_BBox();
    Vector3d Min = { Obj->BBox.lowerLeft[X]-BBoxOffs,
      Obj->BBox.lowerLeft[Y]-BBoxOffs,
      Obj->BBox.lowerLeft[Z]-BBoxOffs };
    Vector3d Size = { Obj->BBox.size[X]+BBoxOffs*2,
      Obj->BBox.size[Y]+BBoxOffs*2,
      Obj->BBox.size[Z]+BBoxOffs*2 };
    Debug_Info("tesselate bounding box: <%f, %f, %f> +<%f, %f, %f>\n",
        Min[X], Min[Y], Min[Z],
        Size[X], Size[Y], Size[Z]
        );

    info.Object =  Obj ;

    for(IndZ = 0; IndZ < ZAccuracy; ++IndZ)
    {
      DBL CoordZ1 = Min[Z] + IndZ*Size[Z]/ZAccuracy;
      DBL CoordZ2 = Min[Z] + (IndZ+1)*Size[Z]/ZAccuracy;

      for(IndY = 0; IndY < YAccuracy; ++IndY)
      {
         Cooperate();
        DBL CoordY1 = Min[Y] + IndY*Size[Y]/YAccuracy;
        DBL CoordY2 = Min[Y] + (IndY+1)*Size[Y]/YAccuracy;

        for(IndX = 0; IndX < XAccuracy; ++IndX)
        {
          DBL CoordX1 = Min[X] + IndX*Size[X]/XAccuracy;
          DBL CoordX2 = Min[X] + (IndX+1)*Size[X]/XAccuracy;

          if(IndX == 0)
          {
            AssignTCoord(&info, 0, CoordX1, CoordY1, CoordZ1);
            AssignTCoord(&info, 1, CoordX1, CoordY1, CoordZ2);
            AssignTCoord(&info, 2, CoordX1, CoordY2, CoordZ1);
            AssignTCoord(&info, 3, CoordX1, CoordY2, CoordZ2);
            CalcIntersection(&info, 0, 1, Sm);
            CalcIntersection(&info, 0, 2, Sm);
            CalcIntersection(&info, 1, 3, Sm);
            CalcIntersection(&info, 2, 3, Sm);
            CalcIntersection(&info, 1, 2, Sm);
          }
          AssignTCoord(&info, 4, CoordX2, CoordY1, CoordZ1);
          AssignTCoord(&info, 5, CoordX2, CoordY1, CoordZ2);
          AssignTCoord(&info, 6, CoordX2, CoordY2, CoordZ1);
          AssignTCoord(&info, 7, CoordX2, CoordY2, CoordZ2);

          CalcIntersection(&info, 4, 0, Sm);
          CalcIntersection(&info, 4, 2, Sm);
          CalcIntersection(&info, 4, 1, Sm);
          CalcIntersection(&info, 4, 6, Sm);
          CalcIntersection(&info, 4, 5, Sm);
          CalcIntersection(&info, 5, 1, Sm);
          CalcIntersection(&info, 5, 6, Sm);
          CalcIntersection(&info, 5, 3, Sm);
          CalcIntersection(&info, 5, 7, Sm);
          CalcIntersection(&info, 6, 2, Sm);
          CalcIntersection(&info, 6, 3, Sm);
          CalcIntersection(&info, 6, 1, Sm);
          CalcIntersection(&info, 6, 7, Sm);
          CalcIntersection(&info, 7, 3, Sm);

          TesselateTetrahedron(&info,0,4,2,1,Sm,das);
          TesselateTetrahedron(&info,6,4,2,1,Sm,das);
          TesselateTetrahedron(&info,6,3,2,1,Sm,das);
          TesselateTetrahedron(&info,4,5,6,1,Sm,das);
          TesselateTetrahedron(&info,5,6,3,1,Sm,das);
          TesselateTetrahedron(&info,6,3,7,5,Sm,das);

          if(IndX+1 < XAccuracy)
          {
            CopyTCoord(&info, 4, 0);
            CopyTCoord(&info, 5, 1);
            CopyTCoord(&info, 6, 2);
            CopyTCoord(&info, 7, 3);
            CopyIntersection(&info, 4,5, 0,1);
            CopyIntersection(&info, 6,7, 2,3);
            CopyIntersection(&info, 4,6, 0,2);
            CopyIntersection(&info, 5,7, 1,3);
            CopyIntersection(&info, 5,6, 1,2);
          }
        }
      }

    }

    return (ObjectPtr)das->tesselationMesh;
  }
  /*
     ================================================================================
     La table faite par Bourke
     ================================================================================
   */

  const short Parser::bourke_table[256][16] =
  {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
    {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
    {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
    {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
    {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
    {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
    {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
    {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
    {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
    {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
    {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
    {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
    {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
    {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
    {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
    {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
    {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
    {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
    {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
    {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
    {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
    {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
    {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
    {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
    {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
    {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
    {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
    {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
    {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
    {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
    {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
    {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
    {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};
  /*
     ================================================================================
     La table faite par Heller
     ================================================================================
   */


  const short Parser::heller_table[256][13]=
  {
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 9, 0, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 1, 8, 1, 9,-1,-1,-1,-1,-1,-1,-1},
    {10, 1, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 0, 1, 2,10,-1,-1,-1,-1,-1,-1,-1},
    { 9, 0, 2, 9, 2,10,-1,-1,-1,-1,-1,-1,-1},
    { 3, 2, 8, 2,10, 8, 8,10, 9,-1,-1,-1,-1},
    {11, 2, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {11, 2, 0,11, 0, 8,-1,-1,-1,-1,-1,-1,-1},
    {11, 2, 3, 0, 1, 9,-1,-1,-1,-1,-1,-1,-1},
    { 2, 1,11, 1, 9,11,11, 9, 8,-1,-1,-1,-1},
    {10, 1, 3,10, 3,11,-1,-1,-1,-1,-1,-1,-1},
    { 1, 0,10, 0, 8,10,10, 8,11,-1,-1,-1,-1},
    { 0, 3, 9, 3,11, 9, 9,11,10,-1,-1,-1,-1},
    { 8,10, 9, 8,11,10,-1,-1,-1,-1,-1,-1,-1},
    { 8, 4, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 4, 3, 4, 7,-1,-1,-1,-1,-1,-1,-1},
    { 1, 9, 0, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1},
    { 9, 4, 1, 4, 7, 1, 1, 7, 3,-1,-1,-1,-1},
    {10, 1, 2, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1},
    { 2,10, 1, 0, 4, 7, 0, 7, 3,-1,-1,-1,-1},
    { 4, 7, 8, 0, 2,10, 0,10, 9,-1,-1,-1,-1},
    { 2, 7, 3, 2, 9, 7, 7, 9, 4, 2,10, 9,-1},
    { 2, 3,11, 7, 8, 4,-1,-1,-1,-1,-1,-1,-1},
    { 7,11, 4,11, 2, 4, 4, 2, 0,-1,-1,-1,-1},
    { 3,11, 2, 4, 7, 8, 9, 0, 1,-1,-1,-1,-1},
    { 2, 7,11, 2, 1, 7, 1, 4, 7, 1, 9, 4,-1},
    { 8, 4, 7,11,10, 1,11, 1, 3,-1,-1,-1,-1},
    {11, 4, 7, 1, 4,11, 1,11,10, 1, 0, 4,-1},
    { 3, 8, 0, 7,11, 4,11, 9, 4,11,10, 9,-1},
    { 7,11, 4, 4,11, 9,11,10, 9,-1,-1,-1,-1},
    { 9, 5, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 8, 4, 9, 5,-1,-1,-1,-1,-1,-1,-1},
    { 5, 4, 0, 5, 0, 1,-1,-1,-1,-1,-1,-1,-1},
    { 4, 8, 5, 8, 3, 5, 5, 3, 1,-1,-1,-1,-1},
    { 2,10, 1, 9, 5, 4,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3, 5, 4, 9,10, 1, 2,-1,-1,-1,-1},
    {10, 5, 2, 5, 4, 2, 2, 4, 0,-1,-1,-1,-1},
    { 3, 4, 8, 3, 2, 4, 2, 5, 4, 2,10, 5,-1},
    {11, 2, 3, 9, 5, 4,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 4, 8,11, 2, 8, 2, 0,-1,-1,-1,-1},
    { 3,11, 2, 1, 5, 4, 1, 4, 0,-1,-1,-1,-1},
    { 8, 5, 4, 2, 5, 8, 2, 8,11, 2, 1, 5,-1},
    { 5, 4, 9, 1, 3,11, 1,11,10,-1,-1,-1,-1},
    { 0, 9, 1, 4, 8, 5, 8,10, 5, 8,11,10,-1},
    { 3, 4, 0, 3,10, 4, 4,10, 5, 3,11,10,-1},
    { 4, 8, 5, 5, 8,10, 8,11,10,-1,-1,-1,-1},
    { 9, 5, 7, 9, 7, 8,-1,-1,-1,-1,-1,-1,-1},
    { 0, 9, 3, 9, 5, 3, 3, 5, 7,-1,-1,-1,-1},
    { 8, 0, 7, 0, 1, 7, 7, 1, 5,-1,-1,-1,-1},
    { 1, 7, 3, 1, 5, 7,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2,10, 5, 7, 8, 5, 8, 9,-1,-1,-1,-1},
    { 9, 1, 0,10, 5, 2, 5, 3, 2, 5, 7, 3,-1},
    { 5, 2,10, 8, 2, 5, 8, 5, 7, 8, 0, 2,-1},
    {10, 5, 2, 2, 5, 3, 5, 7, 3,-1,-1,-1,-1},
    {11, 2, 3, 8, 9, 5, 8, 5, 7,-1,-1,-1,-1},
    { 9, 2, 0, 9, 7, 2, 2, 7,11, 9, 5, 7,-1},
    { 0, 3, 8, 2, 1,11, 1, 7,11, 1, 5, 7,-1},
    { 2, 1,11,11, 1, 7, 1, 5, 7,-1,-1,-1,-1},
    { 3, 9, 1, 3, 8, 9, 7,11,10, 7,10, 5,-1},
    { 9, 1, 0,10, 7,11,10, 5, 7,-1,-1,-1,-1},
    { 3, 8, 0, 7,10, 5, 7,11,10,-1,-1,-1,-1},
    {11, 5, 7,11,10, 5,-1,-1,-1,-1,-1,-1,-1},
    {10, 6, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 0,10, 6, 5,-1,-1,-1,-1,-1,-1,-1},
    { 0, 1, 9, 5,10, 6,-1,-1,-1,-1,-1,-1,-1},
    {10, 6, 5, 9, 8, 3, 9, 3, 1,-1,-1,-1,-1},
    { 1, 2, 6, 1, 6, 5,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3, 2, 6, 5, 2, 5, 1,-1,-1,-1,-1},
    { 5, 9, 6, 9, 0, 6, 6, 0, 2,-1,-1,-1,-1},
    { 9, 6, 5, 3, 6, 9, 3, 9, 8, 3, 2, 6,-1},
    { 3,11, 2,10, 6, 5,-1,-1,-1,-1,-1,-1,-1},
    { 6, 5,10, 2, 0, 8, 2, 8,11,-1,-1,-1,-1},
    { 1, 9, 0, 6, 5,10,11, 2, 3,-1,-1,-1,-1},
    { 1,10, 2, 5, 9, 6, 9,11, 6, 9, 8,11,-1},
    {11, 6, 3, 6, 5, 3, 3, 5, 1,-1,-1,-1,-1},
    { 0, 5, 1, 0,11, 5, 5,11, 6, 0, 8,11,-1},
    { 0, 5, 9, 0, 3, 5, 3, 6, 5, 3,11, 6,-1},
    { 5, 9, 6, 6, 9,11, 9, 8,11,-1,-1,-1,-1},
    {10, 6, 5, 4, 7, 8,-1,-1,-1,-1,-1,-1,-1},
    { 5,10, 6, 7, 3, 0, 7, 0, 4,-1,-1,-1,-1},
    { 5,10, 6, 0, 1, 9, 8, 4, 7,-1,-1,-1,-1},
    { 4, 5, 9, 6, 7,10, 7, 1,10, 7, 3, 1,-1},
    { 7, 8, 4, 5, 1, 2, 5, 2, 6,-1,-1,-1,-1},
    { 4, 1, 0, 4, 5, 1, 6, 7, 3, 6, 3, 2,-1},
    { 9, 4, 5, 8, 0, 7, 0, 6, 7, 0, 2, 6,-1},
    { 4, 5, 9, 6, 3, 2, 6, 7, 3,-1,-1,-1,-1},
    { 7, 8, 4, 2, 3,11,10, 6, 5,-1,-1,-1,-1},
    {11, 6, 7,10, 2, 5, 2, 4, 5, 2, 0, 4,-1},
    {11, 6, 7, 8, 0, 3, 1,10, 2, 9, 4, 5,-1},
    { 6, 7,11, 1,10, 2, 9, 4, 5,-1,-1,-1,-1},
    { 6, 7,11, 4, 5, 8, 5, 3, 8, 5, 1, 3,-1},
    { 6, 7,11, 4, 1, 0, 4, 5, 1,-1,-1,-1,-1},
    { 4, 5, 9, 3, 8, 0,11, 6, 7,-1,-1,-1,-1},
    { 9, 4, 5, 7,11, 6,-1,-1,-1,-1,-1,-1,-1},
    {10, 6, 4,10, 4, 9,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 0, 9,10, 6, 9, 6, 4,-1,-1,-1,-1},
    { 1,10, 0,10, 6, 0, 0, 6, 4,-1,-1,-1,-1},
    { 8, 6, 4, 8, 1, 6, 6, 1,10, 8, 3, 1,-1},
    { 9, 1, 4, 1, 2, 4, 4, 2, 6,-1,-1,-1,-1},
    { 1, 0, 9, 3, 2, 8, 2, 4, 8, 2, 6, 4,-1},
    { 2, 4, 0, 2, 6, 4,-1,-1,-1,-1,-1,-1,-1},
    { 3, 2, 8, 8, 2, 4, 2, 6, 4,-1,-1,-1,-1},
    { 2, 3,11, 6, 4, 9, 6, 9,10,-1,-1,-1,-1},
    { 0,10, 2, 0, 9,10, 4, 8,11, 4,11, 6,-1},
    {10, 2, 1,11, 6, 3, 6, 0, 3, 6, 4, 0,-1},
    {10, 2, 1,11, 4, 8,11, 6, 4,-1,-1,-1,-1},
    { 1, 4, 9,11, 4, 1,11, 1, 3,11, 6, 4,-1},
    { 0, 9, 1, 4,11, 6, 4, 8,11,-1,-1,-1,-1},
    {11, 6, 3, 3, 6, 0, 6, 4, 0,-1,-1,-1,-1},
    { 8, 6, 4, 8,11, 6,-1,-1,-1,-1,-1,-1,-1},
    { 6, 7,10, 7, 8,10,10, 8, 9,-1,-1,-1,-1},
    { 9, 3, 0, 6, 3, 9, 6, 9,10, 6, 7, 3,-1},
    { 6, 1,10, 6, 7, 1, 7, 0, 1, 7, 8, 0,-1},
    { 6, 7,10,10, 7, 1, 7, 3, 1,-1,-1,-1,-1},
    { 7, 2, 6, 7, 9, 2, 2, 9, 1, 7, 8, 9,-1},
    { 1, 0, 9, 3, 6, 7, 3, 2, 6,-1,-1,-1,-1},
    { 8, 0, 7, 7, 0, 6, 0, 2, 6,-1,-1,-1,-1},
    { 2, 7, 3, 2, 6, 7,-1,-1,-1,-1,-1,-1,-1},
    { 7,11, 6, 3, 8, 2, 8,10, 2, 8, 9,10,-1},
    {11, 6, 7,10, 0, 9,10, 2, 0,-1,-1,-1,-1},
    { 2, 1,10, 7,11, 6, 8, 0, 3,-1,-1,-1,-1},
    { 1,10, 2, 6, 7,11,-1,-1,-1,-1,-1,-1,-1},
    { 7,11, 6, 3, 9, 1, 3, 8, 9,-1,-1,-1,-1},
    { 9, 1, 0,11, 6, 7,-1,-1,-1,-1,-1,-1,-1},
    { 0, 3, 8,11, 6, 7,-1,-1,-1,-1,-1,-1,-1},
    {11, 6, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {11, 7, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3,11, 7, 6,-1,-1,-1,-1,-1,-1,-1},
    { 9, 0, 1,11, 7, 6,-1,-1,-1,-1,-1,-1,-1},
    { 7, 6,11, 3, 1, 9, 3, 9, 8,-1,-1,-1,-1},
    { 1, 2,10, 6,11, 7,-1,-1,-1,-1,-1,-1,-1},
    { 2,10, 1, 7, 6,11, 8, 3, 0,-1,-1,-1,-1},
    {11, 7, 6,10, 9, 0,10, 0, 2,-1,-1,-1,-1},
    { 7, 6,11, 3, 2, 8, 8, 2,10, 8,10, 9,-1},
    { 2, 3, 7, 2, 7, 6,-1,-1,-1,-1,-1,-1,-1},
    { 8, 7, 0, 7, 6, 0, 0, 6, 2,-1,-1,-1,-1},
    { 1, 9, 0, 3, 7, 6, 3, 6, 2,-1,-1,-1,-1},
    { 7, 6, 2, 7, 2, 9, 2, 1, 9, 7, 9, 8,-1},
    { 6,10, 7,10, 1, 7, 7, 1, 3,-1,-1,-1,-1},
    { 6,10, 1, 6, 1, 7, 7, 1, 0, 7, 0, 8,-1},
    { 9, 0, 3, 6, 9, 3, 6,10, 9, 6, 3, 7,-1},
    { 6,10, 7, 7,10, 8,10, 9, 8,-1,-1,-1,-1},
    { 8, 4, 6, 8, 6,11,-1,-1,-1,-1,-1,-1,-1},
    {11, 3, 6, 3, 0, 6, 6, 0, 4,-1,-1,-1,-1},
    { 0, 1, 9, 4, 6,11, 4,11, 8,-1,-1,-1,-1},
    { 1, 9, 4,11, 1, 4,11, 3, 1,11, 4, 6,-1},
    {10, 1, 2,11, 8, 4,11, 4, 6,-1,-1,-1,-1},
    {10, 1, 2,11, 3, 6, 6, 3, 0, 6, 0, 4,-1},
    { 0, 2,10, 0,10, 9, 4,11, 8, 4, 6,11,-1},
    { 2,11, 3, 6, 9, 4, 6,10, 9,-1,-1,-1,-1},
    { 3, 8, 2, 8, 4, 2, 2, 4, 6,-1,-1,-1,-1},
    { 2, 0, 4, 2, 4, 6,-1,-1,-1,-1,-1,-1,-1},
    { 1, 9, 0, 3, 8, 2, 2, 8, 4, 2, 4, 6,-1},
    { 9, 4, 1, 1, 4, 2, 4, 6, 2,-1,-1,-1,-1},
    { 8, 4, 6, 8, 6, 1, 6,10, 1, 8, 1, 3,-1},
    { 1, 0,10,10, 0, 6, 0, 4, 6,-1,-1,-1,-1},
    { 8, 0, 3, 9, 6,10, 9, 4, 6,-1,-1,-1,-1},
    {10, 4, 6,10, 9, 4,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 4, 7, 6,11,-1,-1,-1,-1,-1,-1,-1},
    { 4, 9, 5, 3, 0, 8,11, 7, 6,-1,-1,-1,-1},
    { 6,11, 7, 4, 0, 1, 4, 1, 5,-1,-1,-1,-1},
    { 6,11, 7, 4, 8, 5, 5, 8, 3, 5, 3, 1,-1},
    { 6,11, 7, 1, 2,10, 9, 5, 4,-1,-1,-1,-1},
    {11, 7, 6, 8, 3, 0, 1, 2,10, 9, 5, 4,-1},
    {11, 7, 6,10, 5, 2, 2, 5, 4, 2, 4, 0,-1},
    { 7, 4, 8, 2,11, 3,10, 5, 6,-1,-1,-1,-1},
    { 4, 9, 5, 6, 2, 3, 6, 3, 7,-1,-1,-1,-1},
    { 9, 5, 4, 8, 7, 0, 0, 7, 6, 0, 6, 2,-1},
    { 4, 0, 1, 4, 1, 5, 6, 3, 7, 6, 2, 3,-1},
    { 7, 4, 8, 5, 2, 1, 5, 6, 2,-1,-1,-1,-1},
    { 4, 9, 5, 6,10, 7, 7,10, 1, 7, 1, 3,-1},
    { 5, 6,10, 0, 9, 1, 8, 7, 4,-1,-1,-1,-1},
    { 5, 6,10, 7, 0, 3, 7, 4, 0,-1,-1,-1,-1},
    {10, 5, 6, 4, 8, 7,-1,-1,-1,-1,-1,-1,-1},
    { 5, 6, 9, 6,11, 9, 9,11, 8,-1,-1,-1,-1},
    { 0, 9, 5, 0, 5, 3, 3, 5, 6, 3, 6,11,-1},
    { 0, 1, 5, 0, 5,11, 5, 6,11, 0,11, 8,-1},
    {11, 3, 6, 6, 3, 5, 3, 1, 5,-1,-1,-1,-1},
    { 1, 2,10, 5, 6, 9, 9, 6,11, 9,11, 8,-1},
    { 1, 0, 9, 6,10, 5,11, 3, 2,-1,-1,-1,-1},
    { 6,10, 5, 2, 8, 0, 2,11, 8,-1,-1,-1,-1},
    { 3, 2,11,10, 5, 6,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 6, 3, 9, 6, 3, 8, 9, 3, 6, 2,-1},
    { 5, 6, 9, 9, 6, 0, 6, 2, 0,-1,-1,-1,-1},
    { 0, 3, 8, 2, 5, 6, 2, 1, 5,-1,-1,-1,-1},
    { 1, 6, 2, 1, 5, 6,-1,-1,-1,-1,-1,-1,-1},
    {10, 5, 6, 9, 3, 8, 9, 1, 3,-1,-1,-1,-1},
    { 0, 9, 1, 5, 6,10,-1,-1,-1,-1,-1,-1,-1},
    { 8, 0, 3,10, 5, 6,-1,-1,-1,-1,-1,-1,-1},
    {10, 5, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {11, 7, 5,11, 5,10,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 8, 7, 5,10, 7,10,11,-1,-1,-1,-1},
    { 9, 0, 1,10,11, 7,10, 7, 5,-1,-1,-1,-1},
    { 3, 1, 9, 3, 9, 8, 7,10,11, 7, 5,10,-1},
    { 2,11, 1,11, 7, 1, 1, 7, 5,-1,-1,-1,-1},
    { 0, 8, 3, 2,11, 1, 1,11, 7, 1, 7, 5,-1},
    { 9, 0, 2, 9, 2, 7, 2,11, 7, 9, 7, 5,-1},
    {11, 3, 2, 8, 5, 9, 8, 7, 5,-1,-1,-1,-1},
    {10, 2, 5, 2, 3, 5, 5, 3, 7,-1,-1,-1,-1},
    { 5,10, 2, 8, 5, 2, 8, 7, 5, 8, 2, 0,-1},
    { 9, 0, 1,10, 2, 5, 5, 2, 3, 5, 3, 7,-1},
    { 1,10, 2, 5, 8, 7, 5, 9, 8,-1,-1,-1,-1},
    { 1, 3, 7, 1, 7, 5,-1,-1,-1,-1,-1,-1,-1},
    { 8, 7, 0, 0, 7, 1, 7, 5, 1,-1,-1,-1,-1},
    { 0, 3, 9, 9, 3, 5, 3, 7, 5,-1,-1,-1,-1},
    { 9, 7, 5, 9, 8, 7,-1,-1,-1,-1,-1,-1,-1},
    { 4, 5, 8, 5,10, 8, 8,10,11,-1,-1,-1,-1},
    { 3, 0, 4, 3, 4,10, 4, 5,10, 3,10,11,-1},
    { 0, 1, 9, 4, 5, 8, 8, 5,10, 8,10,11,-1},
    { 5, 9, 4, 1,11, 3, 1,10,11,-1,-1,-1,-1},
    { 8, 4, 5, 2, 8, 5, 2,11, 8, 2, 5, 1,-1},
    { 3, 2,11, 1, 4, 5, 1, 0, 4,-1,-1,-1,-1},
    { 9, 4, 5, 8, 2,11, 8, 0, 2,-1,-1,-1,-1},
    {11, 3, 2, 9, 4, 5,-1,-1,-1,-1,-1,-1,-1},
    { 3, 8, 4, 3, 4, 2, 2, 4, 5, 2, 5,10,-1},
    {10, 2, 5, 5, 2, 4, 2, 0, 4,-1,-1,-1,-1},
    { 0, 3, 8, 5, 9, 4,10, 2, 1,-1,-1,-1,-1},
    { 2, 1,10, 9, 4, 5,-1,-1,-1,-1,-1,-1,-1},
    { 4, 5, 8, 8, 5, 3, 5, 1, 3,-1,-1,-1,-1},
    { 5, 0, 4, 5, 1, 0,-1,-1,-1,-1,-1,-1,-1},
    { 3, 8, 0, 4, 5, 9,-1,-1,-1,-1,-1,-1,-1},
    { 9, 4, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 7, 4,11, 4, 9,11,11, 9,10,-1,-1,-1,-1},
    { 3, 0, 8, 7, 4,11,11, 4, 9,11, 9,10,-1},
    {11, 7, 4, 1,11, 4, 1,10,11, 1, 4, 0,-1},
    { 8, 7, 4,11, 1,10,11, 3, 1,-1,-1,-1,-1},
    { 2,11, 7, 2, 7, 1, 1, 7, 4, 1, 4, 9,-1},
    { 3, 2,11, 4, 8, 7, 9, 1, 0,-1,-1,-1,-1},
    { 7, 4,11,11, 4, 2, 4, 0, 2,-1,-1,-1,-1},
    { 2,11, 3, 7, 4, 8,-1,-1,-1,-1,-1,-1,-1},
    { 2, 3, 7, 2, 7, 9, 7, 4, 9, 2, 9,10,-1},
    { 4, 8, 7, 0,10, 2, 0, 9,10,-1,-1,-1,-1},
    { 2, 1,10, 0, 7, 4, 0, 3, 7,-1,-1,-1,-1},
    {10, 2, 1, 8, 7, 4,-1,-1,-1,-1,-1,-1,-1},
    { 9, 1, 4, 4, 1, 7, 1, 3, 7,-1,-1,-1,-1},
    { 1, 0, 9, 8, 7, 4,-1,-1,-1,-1,-1,-1,-1},
    { 3, 4, 0, 3, 7, 4,-1,-1,-1,-1,-1,-1,-1},
    { 8, 7, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 9,10, 8,10,11,-1,-1,-1,-1,-1,-1,-1},
    { 0, 9, 3, 3, 9,11, 9,10,11,-1,-1,-1,-1},
    { 1,10, 0, 0,10, 8,10,11, 8,-1,-1,-1,-1},
    {10, 3, 1,10,11, 3,-1,-1,-1,-1,-1,-1,-1},
    { 2,11, 1, 1,11, 9,11, 8, 9,-1,-1,-1,-1},
    {11, 3, 2, 0, 9, 1,-1,-1,-1,-1,-1,-1,-1},
    {11, 0, 2,11, 8, 0,-1,-1,-1,-1,-1,-1,-1},
    {11, 3, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 8, 2, 2, 8,10, 8, 9,10,-1,-1,-1,-1},
    { 9, 2, 0, 9,10, 2,-1,-1,-1,-1,-1,-1,-1},
    { 8, 0, 3, 1,10, 2,-1,-1,-1,-1,-1,-1,-1},
    {10, 2, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 1, 3, 8, 9, 1,-1,-1,-1,-1,-1,-1,-1},
    { 9, 1, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 0, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
  };



  const short Parser::edgeTable[256]={
    0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };

  /*
     ================================================================================
     Bourke & Heller
     ================================================================================
   */
   void Parser::AssignMCCoord(MarchingCubeInfo* info, int cInd,
      DBL xc, DBL yc, DBL zc)
  {
    info->Coord[cInd][X] = xc;
    info->Coord[cInd][Y] = yc;
    info->Coord[cInd][Z] = zc;
    info->Inside[cInd] = Inside_Object(info->Coord[cInd], info->Object, GetParserDataPtr());
  }

   void Parser::CopyMCCoord(MarchingCubeInfo* info, int sInd, int dInd)
  {
    info->Coord[dInd] = info->Coord[sInd];
    info->Inside[dInd] = info->Inside[sInd];
  }

   void Parser::InterpolMC(int edge, int Preci, MarchingCubeInfo * info,
      int ind1, int ind2)
   {
     DBL factor,other;
     int count;
     int i;
     Vector3d test;
     Vector3d delta;
     count=1;
     if (Preci)
     {
       test = info->Coord[ind1];
       delta = info->Coord[ind2] - info->Coord[ind1];
       delta /= (Preci+1);
       i=0;
       while(i<Preci)
       {
         i++;
         test += delta;
         if (Inside_Object(test,info->Object, GetParserDataPtr()) == info->Inside[ind1])
         {
           count++;
         }
       }
     }
     factor = (count+0.0)/(Preci+2.0);
     other = 1.0 - factor;
     info->Vertex[edge]=other*info->Coord[ind1]+ factor*info->Coord[ind2];
   }

   ObjectPtr Parser::BourkeHeller_Object(ObjectPtr Obj, int which,
      int XAccuracy, int YAccuracy, int ZAccuracy,
      int Preci, DBL BBoxOffs, UNDERCONSTRUCTION *das)
  {
    int IndX, IndY, IndZ;
    MarchingCubeInfo info ;
    int i;
    int mask;

    Obj->Compute_BBox();
    Vector3d Min = { Obj->BBox.lowerLeft[X]-BBoxOffs,
      Obj->BBox.lowerLeft[Y]-BBoxOffs,
      Obj->BBox.lowerLeft[Z]-BBoxOffs };
    Vector3d Size = { Obj->BBox.size[X]+BBoxOffs*2,
      Obj->BBox.size[Y]+BBoxOffs*2,
      Obj->BBox.size[Z]+BBoxOffs*2 };
    Debug_Info("bourke or Heller bounding box: <%f, %f, %f> +<%f, %f, %f>\n",
        Min[X], Min[Y], Min[Z],
        Size[X], Size[Y], Size[Z]
        );

    info.Object =  Obj ;

    for(IndZ = 0; IndZ < ZAccuracy; ++IndZ)
    {
      DBL CoordZ1 = Min[Z] + IndZ*Size[Z]/ZAccuracy;
      DBL CoordZ2 = Min[Z] + (IndZ+1)*Size[Z]/ZAccuracy;

      for(IndX = 0; IndX < XAccuracy; ++IndX)
      {
         Cooperate();
        DBL CoordX1 = Min[X] + IndX*Size[X]/XAccuracy;
        DBL CoordX2 = Min[X] + (IndX+1)*Size[X]/XAccuracy;

        for(IndY = 0; IndY < YAccuracy; ++IndY)
        {
          DBL CoordY1 = Min[Y] + IndY*Size[Y]/YAccuracy;
          DBL CoordY2 = Min[Y] + (IndY+1)*Size[Y]/YAccuracy;

          if(IndY == 0)
          {
            AssignMCCoord(&info, 0, CoordX1, CoordY1, CoordZ1);
            AssignMCCoord(&info, 1, CoordX2, CoordY1, CoordZ1);
            AssignMCCoord(&info, 2, CoordX2, CoordY1, CoordZ2);
            AssignMCCoord(&info, 3, CoordX1, CoordY1, CoordZ2);
          }
          AssignMCCoord(&info, 4, CoordX1, CoordY2, CoordZ1);
          AssignMCCoord(&info, 5, CoordX2, CoordY2, CoordZ1);
          AssignMCCoord(&info, 6, CoordX2, CoordY2, CoordZ2);
          AssignMCCoord(&info, 7, CoordX1, CoordY2, CoordZ2);

          mask = 0;
          if (info.Inside[0]) {mask |= 1;}
          if (info.Inside[1]) {mask |= 2;}
          if (info.Inside[2]) {mask |= 4;}
          if (info.Inside[3]) {mask |= 8;}
          if (info.Inside[4]) {mask |= 16;}
          if (info.Inside[5]) {mask |= 32;}
          if (info.Inside[6]) {mask |= 64;}
          if (info.Inside[7]) {mask |= 128;}

          if (edgeTable[mask] & 1)
          { InterpolMC(0,Preci,&info,0,1); }
          if (edgeTable[mask] & 2)
          { InterpolMC(1,Preci,&info,1,2); }
          if (edgeTable[mask] & 4)
          { InterpolMC(2,Preci,&info,2,3); }
          if (edgeTable[mask] & 8)
          { InterpolMC(3,Preci,&info,3,0); }
          if (edgeTable[mask] & 16)
          { InterpolMC(4,Preci,&info,4,5); }
          if (edgeTable[mask] & 32)
          { InterpolMC(5,Preci,&info,5,6); }
          if (edgeTable[mask] & 64)
          { InterpolMC(6,Preci,&info,6,7); }
          if (edgeTable[mask] & 128)
          { InterpolMC(7,Preci,&info,7,4); }
          if (edgeTable[mask] & 256)
          { InterpolMC(8,Preci,&info,0,4); }
          if (edgeTable[mask] & 512)
          { InterpolMC(9,Preci,&info,1,5); }
          if (edgeTable[mask] & 1024)
          { InterpolMC(10,Preci,&info,2,6); }
          if (edgeTable[mask] & 2048)
          { InterpolMC(11,Preci,&info,3,7); }

          if (which & 1)
          {
            for(i=0;heller_table[mask][i]!=-1;i+=3)
            {
              AddTriangle(info.Vertex[heller_table[mask][i]],
                  info.Vertex[heller_table[mask][i+1]],
                  info.Vertex[heller_table[mask][i+2]],
                  NULL,NULL,NULL,
                  das);
            }
          }
          else
          {
            for(i=0;bourke_table[mask][i]!=-1;i+=3)
            {
              AddTriangle(info.Vertex[bourke_table[mask][i]],
                  info.Vertex[bourke_table[mask][i+1]],
                  info.Vertex[bourke_table[mask][i+2]],
                  NULL,NULL,NULL,
                  das);
            }
          }

          if(IndY+1 < XAccuracy)
          {
            CopyMCCoord(&info, 4, 0);
            CopyMCCoord(&info, 5, 1);
            CopyMCCoord(&info, 6, 2);
            CopyMCCoord(&info, 7, 3);
          }
        }
      }

    }

    return (ObjectPtr)das->tesselationMesh;
  }


  /*
     ================================================================================
     Tessel
     ================================================================================
   */
   void Parser::AssignGTCoord(GTetraCoordInfo* info, int cInd,
      DBL xc, DBL yc, DBL zc)
  {
    info->Coord[cInd][X] = xc;
    info->Coord[cInd][Y] = yc;
    info->Coord[cInd][Z] = zc;
  }
   void Parser::CopyGTCoord(GTetraCoordInfo* info, int sInd, int dInd)
  {
    info->Coord[dInd] =  info->Coord[sInd];
  }


  /*
     ================================================================================
     Tessel
     ================================================================================
   */
   ObjectPtr Parser::GTesselate_Object(ObjectPtr Obj,
      int XAccuracy, int YAccuracy, int ZAccuracy,
      int Sm, DBL BBoxOffs, UNDERCONSTRUCTION *das)
  {
    int IndX, IndY, IndZ;
    TetraCoordInfo info = { Obj };

    Obj->Compute_BBox();
    Vector3d Min = { Obj->BBox.lowerLeft[X]-BBoxOffs,
      Obj->BBox.lowerLeft[Y]-BBoxOffs,
      Obj->BBox.lowerLeft[Z]-BBoxOffs };
    Vector3d Size = { Obj->BBox.size[X]+BBoxOffs*2,
      Obj->BBox.size[Y]+BBoxOffs*2,
      Obj->BBox.size[Z]+BBoxOffs*2 };
    Debug_Info("tessel bounding box: <%f, %f, %f> +<%f, %f, %f>\n",
        Min[X], Min[Y], Min[Z],
        Size[X], Size[Y], Size[Z]
        );

    for(IndZ = 0; IndZ < ZAccuracy; ++IndZ)
    {
      DBL CoordZ1 = Min[Z] + (IndZ)*Size[Z]/ZAccuracy;
      DBL CoordZ2 = Min[Z] + (IndZ+1.00)*Size[Z]/ZAccuracy;

      for(IndY = 0; IndY < YAccuracy; ++IndY)
      {
        Cooperate();
        DBL CoordY1 = Min[Y] + (IndY)*Size[Y]/YAccuracy;
        DBL CoordY2 = Min[Y] + (IndY+1.00)*Size[Y]/YAccuracy;

        for(IndX = 0; IndX < XAccuracy; ++IndX)
        {
          DBL  CoordX1 = Min[X] + (IndX)*Size[X]/XAccuracy;
          DBL  CoordX2 = Min[X] + (IndX+1.00)*Size[X]/XAccuracy;

          if(IndX == 0)
          { 
            AssignTCoord(&info, 0, CoordX1, CoordY1, CoordZ1);
            AssignTCoord(&info, 1, CoordX1, CoordY1, CoordZ2);
            AssignTCoord(&info, 2, CoordX1, CoordY2, CoordZ1);
            AssignTCoord(&info, 3, CoordX1, CoordY2, CoordZ2);
            CalcIntersection(&info, 0, 1, Sm);
            CalcIntersection(&info, 0, 2, Sm);
            CalcIntersection(&info, 3, 1, Sm);
            CalcIntersection(&info, 3, 2, Sm);
          } 
          AssignTCoord(&info, 4, CoordX2, CoordY1, CoordZ1);
          AssignTCoord(&info, 5, CoordX2, CoordY1, CoordZ2);
          AssignTCoord(&info, 6, CoordX2, CoordY2, CoordZ1);
          AssignTCoord(&info, 7, CoordX2, CoordY2, CoordZ2);


          CalcIntersection(&info, 0, 4, Sm);
          CalcIntersection(&info, 6, 4, Sm);
          CalcIntersection(&info, 5, 4, Sm);
          CalcIntersection(&info, 5, 1, Sm);
          CalcIntersection(&info, 5, 7, Sm);
          CalcIntersection(&info, 6, 2, Sm);
          CalcIntersection(&info, 6, 7, Sm);
          CalcIntersection(&info, 3, 7, Sm);


          if (((IndX+IndY+IndZ)& 0x01))
          {
            CalcIntersection(&info, 0, 3, Sm); 
            CalcIntersection(&info, 0, 6, Sm);
            CalcIntersection(&info, 6, 5, Sm);
            CalcIntersection(&info, 0, 5, Sm);
            CalcIntersection(&info, 3, 5, Sm);
            CalcIntersection(&info, 3, 6, Sm);
            TesselateTetrahedron(&info,0,6,2,3,Sm,das);
            TesselateTetrahedron(&info,5,3,7,6,Sm,das);
            TesselateTetrahedron(&info,6,5,0,4,Sm,das);
            TesselateTetrahedron(&info,5,0,1,3,Sm,das);
            TesselateTetrahedron(&info,6,5,3,0,Sm,das);

          }
          else 
          {
            CalcIntersection(&info, 1, 2, Sm);
            CalcIntersection(&info, 4, 2, Sm);
            CalcIntersection(&info, 4, 7, Sm);
            CalcIntersection(&info, 4, 1, Sm);
            CalcIntersection(&info, 7, 1, Sm);
            CalcIntersection(&info, 7, 2, Sm);

            TesselateTetrahedron(&info,0,1,2,4,Sm,das);
            TesselateTetrahedron(&info,5,4,7,1,Sm,das);
            TesselateTetrahedron(&info,6,2,7,4,Sm,das);
            TesselateTetrahedron(&info,3,7,2,1,Sm,das);
            TesselateTetrahedron(&info,1,2,4,7,Sm,das);

          }

          if(IndX+1 < XAccuracy)
          {
            CopyTCoord(&info, 4, 0);
            CopyTCoord(&info, 5, 1);
            CopyTCoord(&info, 6, 2);
            CopyTCoord(&info, 7, 3);
            CopyIntersection(&info, 4,5, 0,1);
            CopyIntersection(&info, 6,7, 2,3);
            CopyIntersection(&info, 4,6, 0,2);
            CopyIntersection(&info, 5,7, 1,3);
          }
        }
      }

    }

    return (ObjectPtr)das->tesselationMesh;
  }

  /*
     ================================================================================
     GTS
     ================================================================================
     ================================================================================
   */

  ObjectPtr Parser::Parse_Gts_Load (void)
  {
    char * filename; 
    GTSInfo info;
    ObjectPtr Res;
    UNDERCONSTRUCTION das;
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    Parse_Begin();

    info.reverse=false;
    filename = Parse_C_String();
    EXPECT
      CASE(RIGHT_TOKEN)
      info.reverse=true;
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE 
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT

      StartAddingTriangles(&das);
    Res = Gts_Load_Object(filename, &info,&das); 

    Debug_Info("gts load : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    POV_FREE(filename);
    return Res;
  }
  void Parser::Parse_Gts_Save (void)
  {
    char * filename; 
    GTSInfo info;
    UNDERCONSTRUCTION das;
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    Parse_Begin();

    info.reverse=false;
    filename = Parse_C_String();
    Parse_Comma();
    info.object = Parse_Object();

    Gts_Save_Object(filename, &info,&das); 

    Destroy_Object(info.object);
    POV_FREE(filename);
    Parse_End();
  }
  void Parser::Parse_Stl_Save (void)
  {
    char * filename; 
    STLInfo info;
                info.reverse = false;
    Parse_Begin();

    filename = Parse_C_String();
    Parse_Comma();
    info.object = Parse_Object();

    Stl_Save_Object(filename, &info); 

    Destroy_Object(info.object);
    POV_FREE(filename);
    Parse_End();
  }
  ObjectPtr Parser::Parse_Stl_Load (void)
  {
    char * filename; 
    STLInfo info;
    ObjectPtr Res;
    UNDERCONSTRUCTION das;
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    Parse_Begin();

    info.reverse=false;
    filename = Parse_C_String();
    EXPECT
      CASE(RIGHT_TOKEN)
      info.reverse=true;
        END_CASE
      CASE( INSIDE_VECTOR_TOKEN )
        Parse_Vector(das.Inside_Vect);
    END_CASE 
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT

      StartAddingTriangles(&das);
    Res = Stl_Load_Object(filename, &info,&das); 

    Debug_Info("stl load : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    POV_FREE(filename);
    return Res;
  }
  /*
     ================================================================================
     Bourke & Heller
     ================================================================================
     ================================================================================
   */
  ObjectPtr Parser::Parse_Bourke(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    Vector3d Accuracy;
    int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
    DBL BBoxOffs = 0.0;
    int Precision =0;
    UNDERCONSTRUCTION das;
    Parse_Begin();
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);

    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
    END_CASE
      CASE(ACCURACY_TOKEN)
      Parse_Vector(Accuracy);
    XAccuracy = (int)(.5+Accuracy[X]);
    YAccuracy = (int)(.5+Accuracy[Y]);
    ZAccuracy = (int)(.5+Accuracy[Z]);
    if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
      Error("accuracy must be greater than 0.\n");
    END_CASE

      CASE(PRECISION_TOKEN)
      Precision = (int)Parse_Float();
    END_CASE

      CASE(OFFSET_TOKEN)
      BBoxOffs = Parse_Float();
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE

      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT

      if(!Object) Error("Object expected.\n");

    StartAddingTriangles(&das);
    Res = BourkeHeller_Object(Object,0, XAccuracy, YAccuracy, ZAccuracy,
        Precision,BBoxOffs,&das); 
    Debug_Info("bourke : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);

    return Res;
  }


  ObjectPtr Parser::Parse_Heller(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    Vector3d Accuracy;
    int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
    DBL BBoxOffs = 0.0;
    int Precision =0;
    UNDERCONSTRUCTION das;
    Parse_Begin();
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);


    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE
      CASE(ACCURACY_TOKEN)
      Parse_Vector(Accuracy);
    XAccuracy = (int)(.5+Accuracy[X]);
    YAccuracy = (int)(.5+Accuracy[Y]);
    ZAccuracy = (int)(.5+Accuracy[Z]);
    if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
      Error("accuracy must be greater than 0.\n");
    END_CASE

      CASE(PRECISION_TOKEN)
      Precision = (int)Parse_Float();
    END_CASE

      CASE(OFFSET_TOKEN)
      BBoxOffs = Parse_Float();
    END_CASE

      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");

    StartAddingTriangles(&das);
    Res = BourkeHeller_Object(Object,1, XAccuracy, YAccuracy, ZAccuracy,
        Precision, BBoxOffs,&das); 
    Debug_Info("heller : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);

    return Res;
  }


  /*
     ================================================================================
     Tesselation & Tessel
     ================================================================================

     ================================================================================
   */
  ObjectPtr Parser::Parse_Tesselation(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    Vector3d Accuracy;
    int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
    DBL BBoxOffs = 0.0;
    int Smooth = 0;
    UNDERCONSTRUCTION das;
    Parse_Begin();
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);


    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
      das.Default_Texture = Object->Texture; /* will be copied when needed */
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE
      CASE(ACCURACY_TOKEN)
      Parse_Vector(Accuracy);
    XAccuracy = (int)(.5+Accuracy[X]);
    YAccuracy = (int)(.5+Accuracy[Y]);
    ZAccuracy = (int)(.5+Accuracy[Z]);
    if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
      Error("accuracy must be greater than 0.\n");
    END_CASE

      CASE(SMOOTH_TOKEN)
      Smooth = 1;
    END_CASE
      CASE(ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE

      CASE(OFFSET_TOKEN)
      BBoxOffs = Parse_Float();
    END_CASE

      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");

    StartAddingTriangles(&das);
    Res = Tesselate_Object(Object, XAccuracy, YAccuracy, ZAccuracy,
        Smooth, BBoxOffs,&das); 
    Debug_Info("tesselate : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);

    return Res;
  }



  ObjectPtr Parser::Parse_Tessel(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    Vector3d Accuracy;
    int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
    DBL BBoxOffs = 0.0;
    int Smooth = 0;
    UNDERCONSTRUCTION das;

    Parse_Begin();
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);

    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
      das.Default_Texture = Object->Texture; /* will be copied when needed */
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE
      CASE(ACCURACY_TOKEN)
      Parse_Vector(Accuracy);
    XAccuracy = (int)(.5+Accuracy[X]);
    YAccuracy = (int)(.5+Accuracy[Y]);
    ZAccuracy = (int)(.5+Accuracy[Z]);
    if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
      Error("accuracy must be greater than 0.\n");
    END_CASE

      CASE(SMOOTH_TOKEN)
      Smooth = 1;
    END_CASE
      CASE(ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE

      CASE(OFFSET_TOKEN)
      BBoxOffs = Parse_Float();
    END_CASE

      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT

      if(!Object) Error("Object expected.\n");
    StartAddingTriangles(&das);
    Res = GTesselate_Object(Object, XAccuracy, YAccuracy, ZAccuracy,
        Smooth, BBoxOffs,&das); 
    Debug_Info("tessel : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);

    return Res;
  }



  /*------------------------------------------------------------------------ 
    || Cristal : a cube with clinale at 50 grades                           ||
    ------------------------------------------------------------------------*/

   void Parser::AssignCrCoord(CristalCoordInfo* info, int cInd,
      DBL xc, DBL yc, DBL zc)
  {
    info->Coord[cInd][X] = xc;
    info->Coord[cInd][Y] = yc;
    info->Coord[cInd][Z] = zc;
  }

   void Parser::CopyCrCoord(CristalCoordInfo* info, int sInd, int dInd)
  {
    info->Coord[dInd] =  info->Coord[sInd];
  }

   void Parser::CopyCrIntersection(CristalCoordInfo* info,
      int sInd1, int sInd2, int dInd1, int dInd2)
  {
    info->Inside[dInd1]= info->Inside[sInd1];
    info->Inside[dInd2]= info->Inside[sInd2];
  }

#define CheckInOut(a,info,b,c) (a)=((info)->Inside[(b)]) ^ ((info)->Inside[(c)])
#define CheckSum(a,info,b,c) (a)=((info)->Inside[(b)]) & ((info)->Inside[(c)])
#define CheckNot(a,info,b,c) (a)=!(((info)->Inside[(b)]) | ((info)->Inside[(c)]))

   void Parser::AddOneSurfaceCristal(CristalCoordInfo* info, int P1ind, int P2ind, int P3ind, int P4ind, 
      UNDERCONSTRUCTION *das
      )
  {
    Vector3d Coords[3];
    Vector3d One,Two,Dir;
    Vector3d Normal,N1,N2,N3;
    DBL l1;
    int X01,X02,X13,X23,X04,X15,X37,X26,X45,X67,X46,X57;
    int C1ind,C2ind;

    One =  info->Coord[0];
    Two =  info->Coord[7];
    Coords[0] =  info->Coord[7];
    Dir = One - Two;
    Dir *= 0.5;
    Coords[0] += Dir;
    /* [0] : always center of the cube, unless... */
    if (info->Count_Inside == 1)
    {
      One = Coords[0];
      if (info->Inside[P1ind])
      {
        Two = info->Coord[P1ind];
      }
      else
      {
        Two = info->Coord[P2ind];
      }
      Dir = Two - One;
      Dir *= 0.666;
      Coords[0] += Dir;
    }
    if  (info->Count_Inside == 7)
    {
      One = Coords[0];
      if (info->Inside[P1ind])
      {
        Two = info->Coord[P2ind];
      }
      else
      {
        Two = info->Coord[P1ind];
      }
      Dir = Two - One;
      Dir *= 0.666;
      Coords[0] += Dir;
    }
    if ((info->Count_Inside == 2)&&(P3ind != 9))
    {
      CheckSum(X01,info,0,1);
      CheckSum(X02,info,0,2);
      CheckSum(X13,info,1,3);
      CheckSum(X23,info,2,3);
      CheckSum(X04,info,0,4);
      CheckSum(X15,info,1,5);
      CheckSum(X37,info,3,7);
      CheckSum(X26,info,2,6);
      CheckSum(X45,info,4,5);
      CheckSum(X67,info,6,7);
      CheckSum(X46,info,4,6);
      CheckSum(X57,info,5,7);
      C1ind = (X01||X02||X04)?0:
        (X13||X15)?1:
        (X23||X26)?2:
        (X37)?3:
        (X45||X46)?4:
        (X57)?5:
        (X67)?6:-1;
      C2ind = (X01)?1:
        (X02)?2:
        (X13||X23)?3:
        (X04)?4:
        (X15||X45)?5:
        (X26||X46)?6:
        (X37||X57||X67)?7:-1;
      if ((C1ind != -1)&&(C2ind != -1))
      {
        One = info->Coord[C1ind];
        Two = info->Coord[C2ind];
        One -= Coords[0];
        Two -= Coords[0];
        One *= 0.5;
        Two *= 0.5;
        One += Two; 
        /* One: vector from center of cube to middle of segment */
        One *= 0.5;
        Coords[0] += One;
      }
    } 
    if ((info->Count_Inside == 6)&&(P3ind != 9))
    {
      CheckNot(X01,info,0,1);
      CheckNot(X02,info,0,2);
      CheckNot(X13,info,1,3);
      CheckNot(X23,info,2,3);
      CheckNot(X04,info,0,4);
      CheckNot(X15,info,1,5);
      CheckNot(X37,info,3,7);
      CheckNot(X26,info,2,6);
      CheckNot(X45,info,4,5);
      CheckNot(X67,info,6,7);
      CheckNot(X46,info,4,6);
      CheckNot(X57,info,5,7);
      C1ind = (X01||X02||X04)?0:
        (X13||X15)?1:
        (X23||X26)?2:
        (X37)?3:
        (X45||X46)?4:
        (X57)?5:
        (X67)?6:-1;
      C2ind = (X01)?1:
        (X02)?2:
        (X13||X23)?3:
        (X04)?4:
        (X15||X45)?5:
        (X26||X46)?6:
        (X37||X57||X67)?7:-1;
      if ((C1ind != -1)&&(C2ind != -1))
      {
        One = info->Coord[C1ind];
        Two = info->Coord[C2ind];
        One -= Coords[0];
        Two -= Coords[0];
        One *= 0.5;
        Two *= 0.5;
        One += Two; 
        /* One: vector from center of cube to middle of segment */
        One *= 0.5;
        Coords[0] += One;
      }
    } 

    One = info->Coord[P1ind];
    Two = info->Coord[P2ind];
    Coords[1] = info->Coord[P2ind];
    Dir = One - Two;
    Dir *= 0.5;
    Coords[1] += Dir;
    /* [1] ; always middle of segment */

    if (P3ind == 9)
    {
      One = info->Coord[P1ind];
      Two = info->Coord[P4ind];
      Coords[2] = info->Coord[P4ind];
      Dir = One - Two;
      Dir *= 0.5;
      Coords[2] += Dir;
      /* [2] : this time, middle of face */
    }
    else
    {
      One = info->Coord[P3ind];
      Two = info->Coord[P4ind];
      Coords[2] = info->Coord[P4ind];
      Dir = One - Two;
      Dir *= 0.5;
      Coords[2] += Dir;
      /* [2] ; this time, middle of segment */
    }
    N1 = Coords[0] - Coords[1];
    N2 = Coords[0] - Coords[2];
    Normal = cross(N1,N2);
    N1 = info->Coord[P1ind];
    N2 = N1 - Coords[1];
    l1 = dot(N2,Normal);
    if (((info->Inside[P1ind])&&(l1>0))
        || ((!info->Inside[P1ind])&&(l1<0)))
    {
      Normal *= -1;
    }
    Normal.normalize();
    N1 = Normal;
    N2 = Normal;
    N3 = Normal;
    One = Coords[0];
    Two = Coords[1];
    Dir = Coords[2];
    AddSmoothTriangle(One, Two, Dir,NULL,NULL,NULL,N1,N2,N3,das);
  }

   void Parser::AnalyseCubeFaceCristal(CristalCoordInfo* info, int P1ind,
      int P2ind, int P3ind, int P4ind,
      UNDERCONSTRUCTION *das)
  {
    int X12, X23, X34, X41;
    CheckInOut(X12,info,P1ind,P2ind);
    CheckInOut(X23,info,P2ind,P3ind);
    CheckInOut(X34,info,P3ind,P4ind);
    CheckInOut(X41,info,P4ind,P1ind);
    /* Note : either 4 are set, or only 2, or none 
     ** If none, nothing to do
     */
    if (X12 && X23 && X34 && X41)
    {
      /* Special Cross */
      /* It's nicer for deformations to have 4 triangles
       ** instead of only two intersecting ones.
       */
      AddOneSurfaceCristal(info, P1ind, P2ind, 9,P3ind,das);
      AddOneSurfaceCristal(info, P2ind, P3ind, 9,P4ind,das);
      AddOneSurfaceCristal(info, P3ind, P4ind, 9,P1ind,das);
      AddOneSurfaceCristal(info, P4ind, P1ind, 9,P2ind,das);
    }
    else
    {
      /* more classical */
      if (X12 && X34)
      {
        AddOneSurfaceCristal(info, P1ind, P2ind, P3ind, P4ind,das);
      }
      if (X12 && X23)
      {
        AddOneSurfaceCristal(info, P1ind, P2ind, P2ind, P3ind,das);
      }
      if (X12 && X41)
      {
        AddOneSurfaceCristal(info, P1ind, P2ind, P1ind, P4ind,das);
      }
      if (X23 && X34)
      {
        AddOneSurfaceCristal(info, P2ind, P3ind, P4ind, P3ind,das);
      }
      if (X23 && X41)
      {
        AddOneSurfaceCristal(info, P2ind, P3ind, P1ind, P4ind,das);
      }
      if (X34 && X41)
      {
        AddOneSurfaceCristal(info, P3ind, P4ind, P4ind, P1ind,das);
      }
    }
  }

   ObjectPtr Parser::Cristallise_Object(ObjectPtr Obj,
      int XAccuracy, int YAccuracy, int ZAccuracy,
      UNDERCONSTRUCTION *das
      )
  {
    int IndX, IndY, IndZ;
    DBL CoordZ1,CoordZ2,CoordX1,CoordX2,CoordY1,CoordY2;
    CristalCoordInfo info;

    Obj->Compute_BBox();
    Vector3d Min = { Obj->BBox.lowerLeft[X],
      Obj->BBox.lowerLeft[Y],
      Obj->BBox.lowerLeft[Z] };
    Vector3d Size = { Obj->BBox.size[X],
      Obj->BBox.size[Y],
      Obj->BBox.size[Z] };
    Debug_Info("cristal bounding box: <%f, %f, %f> +<%f, %f, %f>\n",
        Min[X], Min[Y], Min[Z],
        Size[X], Size[Y], Size[Z]
        );

    for(IndZ = 0; IndZ <= ZAccuracy; ++IndZ)
    {
      CoordZ1 = Min[Z] + (IndZ - 0.5)*Size[Z]/ZAccuracy;
      CoordZ2 = Min[Z] + (IndZ+ 0.5)*Size[Z]/ZAccuracy;

      for(IndY = 0; IndY <= YAccuracy; ++IndY)
      {
        Cooperate();
        CoordY1 = Min[Y] + (IndY - 0.5)*Size[Y]/YAccuracy;
        CoordY2 = Min[Y] + (IndY + 0.5)*Size[Y]/YAccuracy;

        for(IndX = 0; IndX <= XAccuracy; ++IndX)
        {
          CoordX1 = Min[X] + (IndX - 0.5)*Size[X]/XAccuracy;
          CoordX2 = Min[X] + (IndX + 0.5)*Size[X]/XAccuracy;

          if(IndX == 0)
          { 
            AssignCrCoord(&info, 0, CoordX1, CoordY1, CoordZ1);
            AssignCrCoord(&info, 1, CoordX1, CoordY1, CoordZ2);
            AssignCrCoord(&info, 2, CoordX1, CoordY2, CoordZ1);
            AssignCrCoord(&info, 3, CoordX1, CoordY2, CoordZ2); 
            info.Inside[0] = Obj->Inside(info.Coord[0],GetParserDataPtr())?1:0;
            info.Inside[1] = Obj->Inside(info.Coord[1],GetParserDataPtr())?1:0;
            info.Inside[2] = Obj->Inside(info.Coord[2],GetParserDataPtr())?1:0;
            info.Inside[3] = Obj->Inside(info.Coord[3],GetParserDataPtr())?1:0; 
          } 
          AssignCrCoord(&info, 4, CoordX2, CoordY1, CoordZ1);
          AssignCrCoord(&info, 5, CoordX2, CoordY1, CoordZ2);
          AssignCrCoord(&info, 6, CoordX2, CoordY2, CoordZ1);
          AssignCrCoord(&info, 7, CoordX2, CoordY2, CoordZ2); 

          info.Inside[4] = Obj->Inside(info.Coord[4],GetParserDataPtr())?1:0;
          info.Inside[5] = Obj->Inside(info.Coord[5],GetParserDataPtr())?1:0;
          info.Inside[6] = Obj->Inside(info.Coord[6],GetParserDataPtr())?1:0;
          info.Inside[7] = Obj->Inside(info.Coord[7],GetParserDataPtr())?1:0;
          info.Count_Inside = 
            info.Inside[0]
            +info.Inside[1]
            +info.Inside[2]
            +info.Inside[3]
            +info.Inside[4]
            +info.Inside[5]
            +info.Inside[6]
            +info.Inside[7];
          AnalyseCubeFaceCristal(&info,0,1,3,2,das);
          AnalyseCubeFaceCristal(&info,0,1,5,4,das);
          AnalyseCubeFaceCristal(&info,0,4,6,2,das);
          AnalyseCubeFaceCristal(&info,1,5,7,3,das);
          AnalyseCubeFaceCristal(&info,4,5,7,6,das);
          AnalyseCubeFaceCristal(&info,2,3,7,6,das);

          CopyCrCoord(&info, 4, 0);
          CopyCrCoord(&info, 5, 1);
          CopyCrCoord(&info, 6, 2);
          CopyCrCoord(&info, 7, 3); 
          CopyCrIntersection(&info, 4,5, 0,1);
          CopyCrIntersection(&info, 6,7, 2,3);
        }
      }

    }

    return (ObjectPtr)das->tesselationMesh;
  }

  ObjectPtr Parser::Parse_Cristal(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    Vector3d Accuracy;
    int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
    UNDERCONSTRUCTION das;

    Parse_Begin();
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);


    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE 
      CASE(ACCURACY_TOKEN)
      Parse_Vector(Accuracy);
    XAccuracy = (int)(.5+Accuracy[X]);
    YAccuracy = (int)(.5+Accuracy[Y]);
    ZAccuracy = (int)(.5+Accuracy[Z]);
    if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
      Error("accuracy must be greater than 0.\n");
    END_CASE 
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");

    StartAddingTriangles(&das);
    Res = Cristallise_Object(Object, XAccuracy, YAccuracy, ZAccuracy,&das); 
    Debug_Info("cristal : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);

    return Res;
  }


  /*------------------------------------------------------------------------
    || Cubicle : just a simple cube                                         ||
    ------------------------------------------------------------------------*/

   void Parser::AssignCuCoord(CubCoordInfo* info, int cInd,
      DBL xc, DBL yc, DBL zc)
  {
    info->Coord[cInd][X] = xc;
    info->Coord[cInd][Y] = yc;
    info->Coord[cInd][Z] = zc;
  }

   void Parser::CopyCuCoord(CubCoordInfo* info, int sInd, int dInd)
  {
    info->Coord[dInd] =  info->Coord[sInd];
  }

   void Parser::CopyCuIntersection(CubCoordInfo* info,
      int sInd1, int sInd2, int dInd1, int dInd2)
  {
    info->Inside[dInd1]= info->Inside[sInd1];
    info->Inside[dInd2]= info->Inside[sInd2];
  }

  const int Parser::orthotable[5][2]= { {0,0},{2,4},{1,4},{0,0},{1,2}};

  void Parser::AddOneSurfaceCube(CubCoordInfo* info, int P1ind, int P2ind,
      UNDERCONSTRUCTION *das
      )
  {
    Vector3d Coords[5];
    Vector3d One,Two,Dir,Ortho1,Ortho2;
    Vector3d Normal,N1,N2,N3;

    One =  info->Coord[P1ind];
    Two =  info->Coord[P2ind];
    Coords[0] =  info->Coord[P2ind];
    Dir = One - Two;
    Dir *= 0.5;
    Coords[0] += Dir;
    Coords[1] = Coords[0];
    Coords[2] = Coords[0];
    /* Middle of segment between P1ind and P2ind */

    One =  info->Coord[orthotable[abs(P1ind-P2ind)][0]];
    Two =  info->Coord[0];
    Ortho1 = One - Two;
    Ortho1 *= 0.5;
    Coords[1] += Ortho1;
    Coords[2] -= Ortho1;
    Coords[3] = Coords[1];
    Coords[4] = Coords[2];

    One =  info->Coord[orthotable[abs(P1ind-P2ind)][1]];
    Ortho2 = One - Two;
    Ortho2 *= 0.5;
    Coords[1] += Ortho2;
    Coords[2] += Ortho2;
    Coords[3] -= Ortho2;
    Coords[4] -= Ortho2;
    switch(abs(P1ind-P2ind))
    {
      case 4:
        Normal = Vector3d(1.0,0.0,0.0);
        break;
      case 2:
        Normal = Vector3d(0.0,1.0,0.0);
        break;
      case 1:
        Normal = Vector3d(0.0,0.0,1.0);
        break;
    }
    if (!(info->Inside[P1ind]))
    {
      Normal *= -1.0;
    }
    N1 = Normal;
    N2 = Normal;
    N3 = Normal;
    One = Coords[1];
    Two = Coords[2];
    Dir = Coords[0];
    AddSmoothTriangle(Dir, One, Two,NULL,NULL,NULL,N1,N2,N3,das);
    N1 = Normal;
    N2 = Normal;
    N3 = Normal;
    One = Coords[2];
    Two = Coords[4];
    Dir = Coords[0];
    AddSmoothTriangle(Dir, One, Two,NULL,NULL,NULL,N1,N2,N3,das);
    N1 = Normal;
    N2 = Normal;
    N3 = Normal;
    One = Coords[4];
    Two = Coords[3];
    Dir = Coords[0];
    AddSmoothTriangle(Dir, One, Two,NULL,NULL,NULL,N1,N2,N3,das);
    N1 = Normal;
    N2 = Normal;
    N3 = Normal;
    One = Coords[3];
    Two = Coords[1];
    Dir = Coords[0];
    AddSmoothTriangle(Dir, One, Two,NULL,NULL,NULL,N1,N2,N3,das);
  }

   ObjectPtr Parser::Tesselate_Object_Cube(ObjectPtr Obj,
      int XAccuracy, int YAccuracy, int ZAccuracy,
      UNDERCONSTRUCTION *das
      )
  {
    int IndX, IndY, IndZ;
    DBL CoordZ1,CoordZ2,CoordX1,CoordX2,CoordY1,CoordY2;
    CubCoordInfo info;

    Obj->Compute_BBox();
    Vector3d Min = { Obj->BBox.lowerLeft[X],
      Obj->BBox.lowerLeft[Y],
      Obj->BBox.lowerLeft[Z] };
    Vector3d Size = { Obj->BBox.size[X],
      Obj->BBox.size[Y],
      Obj->BBox.size[Z] };
    Debug_Info("cubicle bounding box: <%f, %f, %f> +<%f, %f, %f>\n",
        Min[X], Min[Y], Min[Z],
        Size[X], Size[Y], Size[Z]
        );

    for(IndZ = 0; IndZ <= ZAccuracy; ++IndZ)
    {
      CoordZ1 = Min[Z] + (IndZ - 0.5)*Size[Z]/ZAccuracy;
      CoordZ2 = Min[Z] + (IndZ+ 0.5)*Size[Z]/ZAccuracy;

      for(IndY = 0; IndY <= YAccuracy; ++IndY)
      {
        Cooperate();
        CoordY1 = Min[Y] + (IndY - 0.5)*Size[Y]/YAccuracy;
        CoordY2 = Min[Y] + (IndY + 0.5)*Size[Y]/YAccuracy;

        for(IndX = 0; IndX <= XAccuracy; ++IndX)
        {
          CoordX1 = Min[X] + (IndX - 0.5)*Size[X]/XAccuracy;
          CoordX2 = Min[X] + (IndX + 0.5)*Size[X]/XAccuracy;

          if(IndX == 0)
          { 
            AssignCuCoord(&info, 0, CoordX1, CoordY1, CoordZ1);
            AssignCuCoord(&info, 1, CoordX1, CoordY1, CoordZ2);
            AssignCuCoord(&info, 2, CoordX1, CoordY2, CoordZ1);
            /* AssignCCoord(&info, 3, CoordX1, CoordY2, CoordZ2); */
            info.Inside[0] = Obj->Inside(info.Coord[0],GetParserDataPtr());
            info.Inside[1] = Obj->Inside(info.Coord[1],GetParserDataPtr());
            info.Inside[2] = Obj->Inside(info.Coord[2],GetParserDataPtr());
            /* info.Inside[3] = Inside(info.Coord[3],Obj); */
          } 
          AssignCuCoord(&info, 4, CoordX2, CoordY1, CoordZ1);
          AssignCuCoord(&info, 5, CoordX2, CoordY1, CoordZ2);
          AssignCuCoord(&info, 6, CoordX2, CoordY2, CoordZ1);
          /* AssignCCoord(&info, 7, CoordX2, CoordY2, CoordZ2); */

          info.Inside[4] = Obj->Inside(info.Coord[4],GetParserDataPtr());
          info.Inside[5] = Obj->Inside(info.Coord[5],GetParserDataPtr());
          info.Inside[6] = Obj->Inside(info.Coord[6],GetParserDataPtr());
          /* info.Inside[7] = Inside(info.Coord[7],Obj); */
          if (info.Inside[0] != info.Inside[4])
          {
            AddOneSurfaceCube(&info,0,4,das);
          }
          if (info.Inside[0] != info.Inside[1])
          {
            AddOneSurfaceCube(&info,0,1,das);
          }
          if (info.Inside[0] != info.Inside[2])
          {
            AddOneSurfaceCube(&info,0,2,das);
          }

          CopyCuCoord(&info, 4, 0);
          CopyCuCoord(&info, 5, 1);
          CopyCuCoord(&info, 6, 2);
          /* CopyCCoord(&info, 7, 3); */
          CopyCuIntersection(&info, 4,5, 0,1);
          CopyCuIntersection(&info, 6,7, 2,3);
        }
      }

    }

    return (ObjectPtr)das->tesselationMesh;
  }

  ObjectPtr Parser::Parse_Cubicle(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    Vector3d Accuracy;
    int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
    UNDERCONSTRUCTION das;

    Parse_Begin();
    das.albinos = 0;
    das.Default_Texture = NULL;
    das.Inside_Vect = Vector3d( 0, 0, 0);

    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE 
      CASE(ACCURACY_TOKEN)
      Parse_Vector(Accuracy);
    XAccuracy = (int)(.5+Accuracy[X]);
    YAccuracy = (int)(.5+Accuracy[Y]);
    ZAccuracy = (int)(.5+Accuracy[Z]);
    if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
      Error("accuracy must be greater than 0.\n");
    END_CASE 
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");

    StartAddingTriangles(&das);
    Res = Tesselate_Object_Cube(Object, XAccuracy, YAccuracy, ZAccuracy,&das); 
    Debug_Info("cubicle : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);


    return Res;
  }


  /*------------------------------------------------------------------------
    || transformation of a mesh                                             ||
    ------------------------------------------------------------------------*/

  /* Save the FPU: do not repeat the multiplications */
   void Parser::Init_Rotation_Transform(ScrewCoordInfo * info)
  {
    DBL x,y,z;
    x=info->axis[X];
    y=info->axis[Y];
    z=info->axis[Z];
    info->x2=x*x;
    info->y2=y*y;
    info->z2=z*z;
    info->xy=x*y;
    info->xz=x*z;
    info->yz=y*z;
  }
   void Parser::Comp_Rotation_Transform(TRANSFORM *transform, ScrewCoordInfo * info,
      DBL amount)
  {
    DBL e,f,g;
    DBL x,y,z;
    x=info->axis[X];
    y=info->axis[Y];
    z=info->axis[Z];
    e=cos(amount*M_PI_180);
    f=sin(amount*M_PI_180);
    g=1-e;
    (transform->matrix)[0][0] = info->x2*g+e;
    (transform->matrix)[0][1] = info->xy*g+f*z;
    (transform->matrix)[0][2] = info->xz*g-f*y;
    (transform->matrix)[1][0] = info->xy*g-f*z;
    (transform->matrix)[1][1] = info->y2*g+e;
    (transform->matrix)[1][2] = info->yz*g+f*x;
    (transform->matrix)[2][0] = info->xz*g+f*y;
    (transform->matrix)[2][1] = info->yz*g-f*x;
    (transform->matrix)[2][2] = info->z2*g+e;

    (transform->matrix)[3][0] = 0;
    (transform->matrix)[3][1] = 0;
    (transform->matrix)[3][2] = 0;
    (transform->matrix)[0][3] = 0;
    (transform->matrix)[1][3] = 0;
    (transform->matrix)[2][3] = 0;
  }


   void Parser::Extract_Normal_Direct(SnglVector3d*n,
      int norma, SnglVector3d vertex, SnglVector3d extr1, SnglVector3d extr2, Vector3d& modif)
  {
    Vector3d S1,S2;
    if (norma != -1)
    {
      modif = Vector3d(n[norma][X],n[norma][Y],n[norma][Z]);
    }
    else
    {
      S1 = Vector3d(extr1) - Vector3d(vertex);
      S2 = Vector3d(extr2) - Vector3d(vertex);
      modif = cross(S1,S2);
      modif.normalize();
    }
  }
   ObjectPtr Parser::Screwlise_Object(ObjectPtr Obj,
      ScrewCoordInfo *info, 
      UNDERCONSTRUCTION *das
      )
  {
    Intersection intersection;
    TraceTicket ticket( 1, 0.0);
    Ray ray( ticket );
    Mesh * meshobj= (Mesh *)Obj;
    MESH_TRIANGLE*s = meshobj->Data->Triangles;
    SnglVector3d*v = meshobj->Data->Vertices;
    SnglVector3d*n = meshobj->Data->Normals;
    int tria1,tria2,tria3;
    int n1,n2,n3;
    DBL modulation[3];
    DBL amount[3];
    Vector3d vertex[3];
    Vector3d old_normal[3];
    Vector3d new_vertex[3];
    TEXTURE *t1,*t2,*t3;
    TRANSFORM trans[3];
    TEXTURE **old_texture;
    TransColour colour;
    int limit;
    int indice;
    int trigger = (info->angle.lengthSqr() != 0) ? 1 : 0;
    PIGMENT *pig= NULL;
    if (info->NPat)
    {
      pig = info->NPat->Pigment;
    }
    else
    {
      modulation[0]=1.0;
      modulation[1]=1.0;
      modulation[2]=1.0;
    }
    limit = meshobj->Data->Number_Of_Triangles;
    Init_Rotation_Transform(info);
    old_texture = meshobj->Textures;
    for(indice = 0; indice < limit; indice++)
    {
      tria1 = s[indice].P1;
      tria2 = s[indice].P2;
      tria3 = s[indice].P3;
      n1 = s[indice].N1;
      n2 = s[indice].N2;
      n3 = s[indice].N3; 
      if (trigger)
      {
        new_vertex[0] = Vector3d( v[tria1] );
        new_vertex[1] = Vector3d( v[tria2] );
        new_vertex[2] = Vector3d( v[tria3] );
        if (meshobj->Trans) 
        {
          MTransPoint(new_vertex[0],new_vertex[0],meshobj->Trans);
          MTransPoint(new_vertex[1],new_vertex[1],meshobj->Trans);
          MTransPoint(new_vertex[2],new_vertex[2],meshobj->Trans);
        }
        if (info->NPat)
        {
          Extract_Normal_Direct(n,
              n1,v[tria1],v[tria2],v[tria3],
              old_normal[0]);                                                                           
          Extract_Normal_Direct(n,
              n2,v[tria2],v[tria3],v[tria1],
              old_normal[1]);                                                                           
          Extract_Normal_Direct(n,
              n3,v[tria3],v[tria1],v[tria2],
              old_normal[2]);                                                                           
          intersection.INormal = old_normal[0];
          if (Compute_Pigment(colour,pig,new_vertex[0],&intersection,&ray,GetParserDataPtr()))
          {
            modulation[0] = colour.Greyscale();
          }
          else 
          {
            modulation[0] = 1.0;
          }
          intersection.INormal = old_normal[1];
          if (Compute_Pigment(colour,pig,new_vertex[1],&intersection,&ray,GetParserDataPtr()))
          {
            modulation[1] = colour.Greyscale();
          }
          else 
          {
            modulation[1] = 1.0;
          }
          intersection.INormal = old_normal[2];
          if (Compute_Pigment(colour,pig,new_vertex[2],&intersection,&ray,GetParserDataPtr()))
          {
            modulation[2] = colour.Greyscale();
          }
          else 
          {
            modulation[2] = 1.0;
          }
        }
        vertex[0] = new_vertex[0] - info->origin;
        vertex[1] = new_vertex[1] - info->origin;
        vertex[2] = new_vertex[2] - info->origin;
        amount[0] = dot(vertex[0],info->angle);
        amount[1] = dot(vertex[1],info->angle);
        amount[2] = dot(vertex[2],info->angle);
        if (info->have_min)
        {
          if (amount[0]<info->min_angle)
          {
            new_vertex[0][Z] = amount[0]-info->min_angle;
            amount[0] = info->min_angle;
          }
          if (amount[1]<info->min_angle)
          {
            new_vertex[1][Z] = amount[1]-info->min_angle;
            amount[1] = info->min_angle;
          }
          if (amount[2]<info->min_angle)
          {
            new_vertex[2][Z] = amount[2]-info->min_angle;
            amount[2] = info->min_angle;
          }
        }
        if (info->have_max)
        {
          if (amount[0]>info->max_angle)
          {
            new_vertex[0][Z] = amount[0]-info->max_angle;
            amount[0] = info->max_angle;
          }
          if (amount[1]>info->max_angle)
          {
            new_vertex[1][Z] = amount[1]-info->max_angle;
            amount[1] = info->max_angle;
          }
          if (amount[2]>info->max_angle)
          {
            new_vertex[2][Z] = amount[2]-info->max_angle;
            amount[2] = info->max_angle;
          }
        }
        amount[0] *= info->inverse * modulation[0];
        amount[1] *= info->inverse * modulation[1];
        amount[2] *= info->inverse * modulation[2];
        Comp_Rotation_Transform(&trans[0],info,amount[0]);
        Comp_Rotation_Transform(&trans[1],info,amount[1]);
        Comp_Rotation_Transform(&trans[2],info,amount[2]);
        MTransPoint(new_vertex[0],vertex[0],&trans[0]);
        MTransPoint(new_vertex[1],vertex[1],&trans[1]);
        MTransPoint(new_vertex[2],vertex[2],&trans[2]);
        new_vertex[0] += info->origin;
        new_vertex[1] += info->origin;
        new_vertex[2] += info->origin;
      }
      else
      {
        new_vertex[0] = Vector3d( v[tria1] );
        new_vertex[1] = Vector3d( v[tria2] );
        new_vertex[2] = Vector3d( v[tria3] );
        if (meshobj->Trans) 
        {
          MTransPoint(new_vertex[0],new_vertex[0],meshobj->Trans);
          MTransPoint(new_vertex[1],new_vertex[1],meshobj->Trans);
          MTransPoint(new_vertex[2],new_vertex[2],meshobj->Trans);
        }
      }
      t1 = t2 = t3 = NULL;
      if (!das->albinos)
      {
        if (!(s[indice].Texture<0))
        {
          t1 = (old_texture[s[indice].Texture]);
          if ((s[indice].ThreeTex) && (!(s[indice].Texture2<0)))
          {
            t2 = (old_texture[s[indice].Texture2]);
          }
          if ((s[indice].ThreeTex) && (!(s[indice].Texture3<0)))
          {
            t3 = (old_texture[s[indice].Texture3]);
          }
          if (!(s[indice].ThreeTex))
          {
            t2 = t3 = t1;
          }
        }
      }
      AddTriangle(new_vertex[0],new_vertex[1],new_vertex[2],t1,t2,t3,das);
    }
    return (ObjectPtr)das->tesselationMesh;
  }
 ObjectPtr Parser::Roll_Object(ObjectPtr Obj,
      ScrewCoordInfo *info,
      UNDERCONSTRUCTION *das
      )
  {
    Intersection intersection;
    TraceTicket ticket( 1, 0.0);
    Ray ray( ticket );
    Mesh * meshobj= (Mesh *)Obj;
    MESH_TRIANGLE*s = meshobj->Data->Triangles;
    SnglVector3d*v = meshobj->Data->Vertices;
    SnglVector3d*n = meshobj->Data->Normals;
    int tria1,tria2,tria3;
    int n1,n2,n3;
    TEXTURE *t1,*t2,*t3;
    Vector3d old_normal[3];
    TransColour colour;
    DBL modulation[3];
    DBL amount[3];
    Vector3d rotat[3];
    Vector3d vertex[3];
    Vector3d new_vertex[3];
    TRANSFORM trans[3];
    int limit;
    TEXTURE **old_texture;
    int indice;
    int trigger = info->angle.lengthSqr() != 0 ? 1 : 0;
    PIGMENT *pig=NULL;
    if (info->NPat)
    {
      pig = info->NPat->Pigment;
    }
    else
    {
      modulation[0]=1.0;
      modulation[1]=1.0;
      modulation[2]=1.0;
    }
    limit = meshobj->Data->Number_Of_Triangles;
    Init_Rotation_Transform(info);
    old_texture = meshobj->Textures;
    for(indice = 0; indice < limit; indice++)
    {
      tria1 = s[indice].P1;
      tria2 = s[indice].P2;
      tria3 = s[indice].P3;
      n1 = s[indice].N1;
      n2 = s[indice].N2;
      n3 = s[indice].N3; 
      if (trigger)
      {
        new_vertex[0] = Vector3d( v[tria1] );
        new_vertex[1] = Vector3d( v[tria2] );
        new_vertex[2] = Vector3d( v[tria3] );
        if (meshobj->Trans) 
        {
          MTransPoint(new_vertex[0],new_vertex[0],meshobj->Trans);
          MTransPoint(new_vertex[1],new_vertex[1],meshobj->Trans);
          MTransPoint(new_vertex[2],new_vertex[2],meshobj->Trans);
        }
        if (info->NPat)
        {
          Extract_Normal_Direct(n,
              n1,v[tria1],v[tria2],v[tria3],
              old_normal[0]);                                                                           
          Extract_Normal_Direct(n,
              n2,v[tria2],v[tria3],v[tria1],
              old_normal[1]);                                                                           
          Extract_Normal_Direct(n,
              n3,v[tria3],v[tria1],v[tria2],
              old_normal[2]);                                                                           
          intersection.INormal = old_normal[0];
          if (Compute_Pigment(colour,pig,new_vertex[0],&intersection,&ray,GetParserDataPtr()))
          {
            modulation[0] = colour.Greyscale();
          }
          else 
          {
            modulation[0] = 1.0;
          }
          intersection.INormal = old_normal[1];
          if (Compute_Pigment(colour,pig,new_vertex[1],&intersection,&ray,GetParserDataPtr()))
          {
            modulation[1] = colour.Greyscale();
          }
          else 
          {
            modulation[1] = 1.0;
          }
          intersection.INormal = old_normal[2];
          if (Compute_Pigment(colour,pig,new_vertex[2],&intersection,&ray,GetParserDataPtr()))
          {
            modulation[2] = colour.Greyscale();
          }
          else 
          {
            modulation[2] = 1.0;
          }
        }
        vertex[0] = new_vertex[0] - info->origin;
        vertex[1] = new_vertex[1] - info->origin;
        vertex[2] = new_vertex[2] - info->origin;
        rotat[0] = cross(info->angle,vertex[0]);
        rotat[1] = cross(info->angle,vertex[1]);
        rotat[2] = cross(info->angle,vertex[2]);
        amount[0] = rotat[0].length();
        amount[1] = rotat[1].length();
        amount[2] = rotat[2].length();
        if (info->have_min)
        {
          if (amount[0]<info->min_angle)
          {
            new_vertex[0][Z] = amount[0]-info->min_angle;
            amount[0] = info->min_angle;
          }
          if (amount[1]<info->min_angle)
          {
            new_vertex[1][Z] = amount[1]-info->min_angle;
            amount[1] = info->min_angle;
          }
          if (amount[2]<info->min_angle)
          {
            new_vertex[2][Z] = amount[2]-info->min_angle;
            amount[2] = info->min_angle;
          }
        }
        if (info->have_max)
        {
          if (amount[0]>info->max_angle)
          {
            new_vertex[0][Z] = amount[0]-info->max_angle;
            amount[0] = info->max_angle;
          }
          if (amount[1]>info->max_angle)
          {
            new_vertex[1][Z] = amount[1]-info->max_angle;
            amount[1] = info->max_angle;
          }
          if (amount[2]>info->max_angle)
          {
            new_vertex[2][Z] = amount[2]-info->max_angle;
            amount[2] = info->max_angle;
          }
        }
        amount[0] *= modulation[0];
        amount[1] *= modulation[1];
        amount[2] *= modulation[2];
        Comp_Rotation_Transform(&trans[0],info,amount[0]);
        Comp_Rotation_Transform(&trans[1],info,amount[1]);
        Comp_Rotation_Transform(&trans[2],info,amount[2]);
        MTransPoint(new_vertex[0],vertex[0],&trans[0]);
        MTransPoint(new_vertex[1],vertex[1],&trans[1]);
        MTransPoint(new_vertex[2],vertex[2],&trans[2]);
        new_vertex[0] += info->origin;
        new_vertex[1] += info->origin;
        new_vertex[2] += info->origin;
      }
      else
      {
        new_vertex[0] = Vector3d( v[tria1] );
        new_vertex[1] = Vector3d( v[tria2] );
        new_vertex[2] = Vector3d( v[tria3] );
        if (meshobj->Trans) 
        {
          MTransPoint(new_vertex[0],new_vertex[0],meshobj->Trans);
          MTransPoint(new_vertex[1],new_vertex[1],meshobj->Trans);
          MTransPoint(new_vertex[2],new_vertex[2],meshobj->Trans);
        }
      }
      t2 = t3 = t1 = NULL;
      if (!das->albinos)
      {
        if (!(s[indice].Texture<0))
        {
          t1 = (old_texture[s[indice].Texture]);
          if ((s[indice].ThreeTex) && (!(s[indice].Texture2<0)))
          {
            t2 = (old_texture[s[indice].Texture2]);
          }
          if ((s[indice].ThreeTex) && (!(s[indice].Texture3<0)))
          {
            t3 = (old_texture[s[indice].Texture3]);
          }
          if (!(s[indice].ThreeTex))
          {
            t2 = t3 = t1;
          }
        }
      }
      AddTriangle(new_vertex[0],new_vertex[1],new_vertex[2],t1,t2,t3,das);
    }
    return (ObjectPtr)das->tesselationMesh;
  }

   ObjectPtr Parser::Bend_Object(ObjectPtr Obj,
      ScrewCoordInfo *info,
      UNDERCONSTRUCTION *das
      )
  {
    Intersection intersection;
    TraceTicket ticket( 1, 0.0);
    Ray ray( ticket );
    Mesh * meshobj= (Mesh *)Obj;
    MESH_TRIANGLE*s = meshobj->Data->Triangles;
    SnglVector3d*v = meshobj->Data->Vertices;
    SnglVector3d*n = meshobj->Data->Normals;
    int tria1,tria2,tria3;
    int n1,n2,n3;
    DBL amount[3];
    DBL modulation[3];
    TEXTURE *t1,*t2,*t3;
    Vector3d vertex[3];
    Vector3d new_vertex[3];
    Vector3d project;
    Vector3d plan_normal;
    Vector3d plan_one;
    Vector3d plan_two;
    TRANSFORM projection;
    TRANSFORM trans[3];
    Vector3d old_normal[3];
    TransColour colour;
    TEXTURE **old_texture;
    int limit;
    int indice;
    PIGMENT *pig=NULL;
    if (info->NPat)
    {
      pig = info->NPat->Pigment;
    }
    else
    {
      modulation[0]=1.0;
      modulation[1]=1.0;
      modulation[2]=1.0;
    }
    limit = meshobj->Data->Number_Of_Triangles;
    old_texture = meshobj->Textures;
    plan_normal = cross(info->angle,info->no_move);
    plan_normal.normalize();
    plan_one = cross(plan_normal,info->angle);
    plan_two = cross(plan_normal,plan_one);

    plan_one.normalize();
    plan_two.normalize();
    MIdentity(projection.matrix);
    projection.matrix[0][0] = plan_one[X];
    projection.matrix[0][1] = plan_one[Y];
    projection.matrix[0][2] = plan_one[Z];
    projection.matrix[1][0] = plan_two[X];
    projection.matrix[1][1] = plan_two[Y];
    projection.matrix[1][2] = plan_two[Z];
    projection.matrix[2][0] = plan_normal[X];
    projection.matrix[2][1] = plan_normal[Y];
    projection.matrix[2][2] = plan_normal[Z];
    /* projection is now a rotation matrix into the new base */
    /* and the opposite of that is the transpose */
    MTranspose(projection.inverse,projection.matrix);
    MInvTransPoint(project,info->angle,&projection);
    info->axis = project;
    Init_Rotation_Transform(info);

    for(indice = 0; indice < limit; indice++)
    {
      tria1 = s[indice].P1;
      tria2 = s[indice].P2;
      tria3 = s[indice].P3;
      n1 = s[indice].N1;
      n2 = s[indice].N2;
      n3 = s[indice].N3; 
      new_vertex[0] = Vector3d( v[tria1] );
      new_vertex[1] = Vector3d( v[tria2] );
      new_vertex[2] = Vector3d( v[tria3] );
      if (meshobj->Trans) 
      {
        MTransPoint(new_vertex[0],new_vertex[0],meshobj->Trans);
        MTransPoint(new_vertex[1],new_vertex[1],meshobj->Trans);
        MTransPoint(new_vertex[2],new_vertex[2],meshobj->Trans);
      }
      if (info->NPat)
      {
        Extract_Normal_Direct(n,
            n1,v[tria1],v[tria2],v[tria3],
            old_normal[0]);                                                                           
        Extract_Normal_Direct(n,
            n2,v[tria2],v[tria3],v[tria1],
            old_normal[1]);                                                                           
        Extract_Normal_Direct(n,
            n3,v[tria3],v[tria1],v[tria2],
            old_normal[2]);                                                                           
        intersection.INormal = old_normal[0];
        if (Compute_Pigment(colour,pig,new_vertex[0],&intersection,&ray,GetParserDataPtr()))
        {
          modulation[0] = colour.Greyscale();
        }
        else 
        {
          modulation[0] = 1.0;
        }
        intersection.INormal = old_normal[1];
        if (Compute_Pigment(colour,pig,new_vertex[1],&intersection,&ray,GetParserDataPtr()))
        {
          modulation[1] = colour.Greyscale();
        }
        else 
        {
          modulation[1] = 1.0;
        }
        intersection.INormal = old_normal[2];
        if (Compute_Pigment(colour,pig,new_vertex[2],&intersection,&ray,GetParserDataPtr()))
        {
          modulation[2] = colour.Greyscale();
        }
        else 
        {
          modulation[2] = 1.0;
        }
      }
      vertex[0] = new_vertex[0] - info->origin;
      vertex[1] = new_vertex[1] - info->origin;
      vertex[2] = new_vertex[2] - info->origin;
      /* Now, project on a plane which is at the origin
         info->axis is part of it
         as well as info->no_move
         (Apply projection, nullify third coordinate and inverse)
         (Nota: keep the third coordinate for later use)
       */
      MInvTransPoint(new_vertex[0],vertex[0],&projection);
      MInvTransPoint(new_vertex[1],vertex[1],&projection);
      MInvTransPoint(new_vertex[2],vertex[2],&projection);
      amount[0] = new_vertex[0][Z];
      amount[1] = new_vertex[1][Z];
      amount[2] = new_vertex[2][Z];
      new_vertex[0][Z] = 0.0;
      new_vertex[1][Z] = 0.0;
      new_vertex[2][Z] = 0.0;
      if (info->have_min)
      {
        if (amount[0]<info->min_angle)
        {
          new_vertex[0][Z] = amount[0]-info->min_angle;
          amount[0] = info->min_angle;
        }
        if (amount[1]<info->min_angle)
        {
          new_vertex[1][Z] = amount[1]-info->min_angle;
          amount[1] = info->min_angle;
        }
        if (amount[2]<info->min_angle)
        {
          new_vertex[2][Z] = amount[2]-info->min_angle;
          amount[2] = info->min_angle;
        }
      }
      if (info->have_max)
      {
        if (amount[0]>info->max_angle)
        {
          new_vertex[0][Z] = amount[0]-info->max_angle;
          amount[0] = info->max_angle;
        }
        if (amount[1]>info->max_angle)
        {
          new_vertex[1][Z] = amount[1]-info->max_angle;
          amount[1] = info->max_angle;
        }
        if (amount[2]>info->max_angle)
        {
          new_vertex[2][Z] = amount[2]-info->max_angle;
          amount[2] = info->max_angle;
        }
      }
      amount[0] *= modulation[0]*info->amount;
      amount[1] *= modulation[1]*info->amount;
      amount[2] *= modulation[2]*info->amount;
      Comp_Rotation_Transform(&trans[0],info,amount[0]);
      Comp_Rotation_Transform(&trans[1],info,amount[1]);
      Comp_Rotation_Transform(&trans[2],info,amount[2]);

      MTransPoint(vertex[0],new_vertex[0],&trans[0]);
      MTransPoint(vertex[1],new_vertex[1],&trans[1]);
      MTransPoint(vertex[2],new_vertex[2],&trans[2]);

      MTransPoint(new_vertex[0],vertex[0],&projection);
      MTransPoint(new_vertex[1],vertex[1],&projection);
      MTransPoint(new_vertex[2],vertex[2],&projection);
      /* back in normal space */
      new_vertex[0] += info->origin;
      new_vertex[1] += info->origin;
      new_vertex[2] += info->origin;
      /* and away from origin */

      t2 = t3 = t1 = NULL;
      if (!das->albinos)
      {
        if (!(s[indice].Texture<0))
        {
          t1 = (old_texture[s[indice].Texture]);
          if ((s[indice].ThreeTex) && (!(s[indice].Texture2<0)))
          {
            t2 = (old_texture[s[indice].Texture2]);
          }
          if ((s[indice].ThreeTex) && (!(s[indice].Texture3<0)))
          {
            t3 = (old_texture[s[indice].Texture3]);
          }
          if (!(s[indice].ThreeTex))
          {
            t2 = t3 = t1;
          }
        }
      }
      AddTriangle(new_vertex[0],new_vertex[1],new_vertex[2],t1,t2,t3,das);
    }
    return (ObjectPtr)das->tesselationMesh;
  }


  typedef struct {
    int method;
    DBL amount;
  } SmoothInfo;
  typedef struct {
    ObjectPtr  bound;
    int inside;
    int outside;
    int bin;
    int bout;
  } SelectInfo;

   void Parser::Extract_Normal(SmoothInfo *info,SnglVector3d*n,
      int norma, SnglVector3d vertex, SnglVector3d extr1, SnglVector3d extr2, Vector3d& modif)
  {
    Vector3d S1,S2,S3;
    DBL angle;
    if (norma != -1)
    {
      modif = Vector3d(n[norma][X],n[norma][Y],n[norma][Z]);
    }
    else 
    {
      S1 = Vector3d( extr1 ) - Vector3d( vertex );
      S2 = Vector3d( extr2 ) - Vector3d( vertex );
      modif = cross(S1,S2);
      modif.normalize();
    }
    switch (info->method)
    {
      case 1:
        S1 = Vector3d( extr1 ) - Vector3d( vertex );
        S2 = Vector3d( extr2 ) - Vector3d( vertex );
        angle = S1.length();
        modif *= angle; 
        angle = S2.length();
        modif *= angle; 
        break;
      case 2:
        S1 = Vector3d( extr1 ) - Vector3d( vertex );
        S2 = Vector3d( extr2 ) - Vector3d( vertex );
        S1.normalize();
        S2.normalize();
        S3 = cross(S1,S2);
        angle = S3.length();
        modif *= angle; 
        break;
      case 3:
        S1 = Vector3d( extr1 ) - Vector3d( vertex );
        S2 = Vector3d( extr2 ) - Vector3d( vertex );
        S3 = cross(S1,S2);
        angle = S3.length();
        modif *= angle; 
        break;
      default:
        break;
    }
  }
   ObjectPtr Parser::Smooth_Object(ObjectPtr Obj,
      SmoothInfo *info,
      UNDERCONSTRUCTION *das
      )
  {
    Mesh * meshobj= (Mesh *)Obj;
    MESH_TRIANGLE*s = meshobj->Data->Triangles;
    SnglVector3d*v = meshobj->Data->Vertices;
    SnglVector3d*n = meshobj->Data->Normals;
    TEXTURE *t1,*t2,*t3;
    Vector3d *average_normal;
    int tria1,tria2,tria3;
    int norma;
    int n1,n2,n3;
    DBL amount[3];
    Vector3d modif;
    Vector3d new_vertex[3];
    Vector3d new_normal[3];
    TEXTURE **old_texture;
    Vector3d old_normal[3];
    Vector3d S1;
    DBL side;
    int limit;
    int indice,indix;
    limit = meshobj->Data->Number_Of_Triangles;
    old_texture = meshobj->Textures;
    norma = meshobj->Data->Number_Of_Normals;
    n1 = meshobj->Data->Number_Of_Vertices;
    average_normal = (Vector3d*)POV_MALLOC(meshobj->Data->Number_Of_Vertices*sizeof(Vector3d),
        "temporary smooth triangle mesh data"); 
    for(indice = 0; indice < n1; indice++)
    {
      average_normal[indice] = Vector3d(0.0,0.0,0.0);
    }
    for(indix = 0; indix < limit; indix++)
    {
      Cooperate();
      Extract_Normal(info,n,
          s[indix].N1,v[s[indix].P1],v[s[indix].P2],v[s[indix].P3],
          modif);
      side = dot(average_normal[s[indix].P1],modif);
      if (side<0)
      {
        average_normal[s[indix].P1] -= modif; 
      }
      else
      {
        average_normal[s[indix].P1] += modif;
      }

      Extract_Normal(info,n,
          s[indix].N2,v[s[indix].P2],v[s[indix].P3],v[s[indix].P1],
          modif);
      side = dot(average_normal[s[indix].P2],modif);
      if (side<0)
      {
        average_normal[s[indix].P2] -= modif;
      }
      else
      {
        average_normal[s[indix].P2] += modif;
      }
      Extract_Normal(info,n,
          s[indix].N3,v[s[indix].P3],v[s[indix].P1],v[s[indix].P2],
          modif);
      side = dot(average_normal[s[indix].P3],modif);
      if (side<0)
      {
        average_normal[s[indix].P3] -= modif;
      }
      else
      {
        average_normal[s[indix].P3] += modif;
      }
    }
    for(indice = 0; indice < limit; indice++)
    {
      Cooperate();
      tria1 = s[indice].P1;
      tria2 = s[indice].P2;
      tria3 = s[indice].P3;
      n1 = s[indice].N1;
      n2 = s[indice].N2;
      n3 = s[indice].N3;
      new_normal[0] = average_normal[tria1];
      new_normal[1] = average_normal[tria2];
      new_normal[2] = average_normal[tria3];
      new_vertex[0] = Vector3d( v[tria1] );
      new_vertex[1] = Vector3d( v[tria2] );
      new_vertex[2] = Vector3d( v[tria3] );

      amount[0] = new_normal[0].length();
      amount[1] = new_normal[1].length();
      amount[2] = new_normal[2].length();
      Extract_Normal_Direct(n,
          n1,v[tria1],v[tria2],v[tria3],
          old_normal[0]);                                                                           
      Extract_Normal_Direct(n,
          n2,v[tria2],v[tria3],v[tria1],
          old_normal[1]);                                                                           
      Extract_Normal_Direct(n,
          n3,v[tria3],v[tria1],v[tria2],
          old_normal[2]);                                                                           
      if (amount[0] > 0)
      {
        new_normal[0].normalize();
        side = dot(new_normal[0],old_normal[0]);
        if (side < 0)
        {
          new_normal[0] *= -1;
        }
        S1 = new_normal[0] - old_normal[0];
        S1 *= info->amount;
        old_normal[0] += S1;
      }
      new_normal[0] = old_normal[0];
      if (amount[1] > 0)
      {
        new_normal[1].normalize();
        side = dot(new_normal[1],old_normal[1]);
        if (side < 0)
        {
          new_normal[1] *= -1;
        }
        S1 = new_normal[1] - old_normal[1];
        S1 *= info->amount;
        old_normal[1] += S1;
      }
      new_normal[1] = old_normal[1];
      if (amount[2] > 0)
      {
        new_normal[2].normalize();
        side = dot(new_normal[2],old_normal[2]);
        if (side < 0)
        {
          new_normal[2] *= -1;
        }
        S1 = new_normal[2] - old_normal[2];
        S1 *= info->amount;
        old_normal[2] += S1;
      }
      new_normal[2] = old_normal[2];
      if (meshobj->Trans)
      {
        MTransPoint(new_vertex[0],new_vertex[0],meshobj->Trans);
        MTransPoint(new_vertex[1],new_vertex[1],meshobj->Trans);
        MTransPoint(new_vertex[2],new_vertex[2],meshobj->Trans);
        MTransNormal(new_normal[0],new_normal[0],meshobj->Trans);
        MTransNormal(new_normal[1],new_normal[1],meshobj->Trans);
        MTransNormal(new_normal[2],new_normal[2],meshobj->Trans);
      } 
      t2 = t3 = t1 = NULL;
      if (!das->albinos)
      {
        if (!(s[indice].Texture<0))
        {
          t1 = (old_texture[s[indice].Texture]);
          if ((s[indice].ThreeTex) && (!(s[indice].Texture2<0)))
          {
            t2 = (old_texture[s[indice].Texture2]);
          }
          if ((s[indice].ThreeTex) && (!(s[indice].Texture3<0)))
          {
            t3 = (old_texture[s[indice].Texture3]);
          }
          if (!(s[indice].ThreeTex))
          {
            t2 = t3 = t1;
          }
        }
      }
      AddSmoothTriangle(new_vertex[0],new_vertex[1],new_vertex[2],t1,t2,t3,
          new_normal[0],new_normal[1],new_normal[2],das);
    }
    POV_FREE(average_normal);
    return (ObjectPtr)das->tesselationMesh;
  }

   ObjectPtr Parser::Warp_Object(ObjectPtr Obj,
      WarpInfo& info,
      UNDERCONSTRUCTION *das
      )
  {
    Mesh * meshobj= (Mesh *)Obj;
    MESH_TRIANGLE*s = meshobj->Data->Triangles;
    SnglVector3d*v = meshobj->Data->Vertices;
    SnglVector3d*n = meshobj->Data->Normals;
    TransColour colour;
    int tria1,tria2,tria3;
    int n1,n2,n3;
    Vector3d vertex[3];
    DBL modulation[3];
    TEXTURE *t1,*t2,*t3;
    TEXTURE **old_texture;
    Vector3d new_vertex[3];
    Vector3d mov_vertex[3];
    Vector3d old_normal[3];
    Intersection intersection;
    TraceTicket ticket( 1, 0.0);
    Ray ray( ticket );
    int limit;
    int indice;
    PIGMENT *pig=NULL;
    if (info.NPat)
    {
      pig = info.NPat->Pigment;
    }
    else
    {
      modulation[0]=1.0;
      modulation[1]=1.0;
      modulation[2]=1.0;
    }
    limit = meshobj->Data->Number_Of_Triangles;
    old_texture = meshobj->Textures;
    for(indice = 0; indice < limit; indice++)
    {
      tria1 = s[indice].P1;
      tria2 = s[indice].P2;
      tria3 = s[indice].P3;
      n1 = s[indice].N1;
      n2 = s[indice].N2;
      n3 = s[indice].N3; 

      vertex[0] = Vector3d( v[tria1] );
      vertex[1] = Vector3d( v[tria2] );
      vertex[2] = Vector3d( v[tria3] );
      if (meshobj->Trans)
      {
        MTransPoint(vertex[0],vertex[0],meshobj->Trans);
        MTransPoint(vertex[1],vertex[1],meshobj->Trans);
        MTransPoint(vertex[2],vertex[2],meshobj->Trans);
      } 
      if (info.NPat)
      {
        Extract_Normal_Direct(n,
            n1,v[tria1],v[tria2],v[tria3],
            old_normal[0]);                                                                           
        Extract_Normal_Direct(n,
            n2,v[tria2],v[tria3],v[tria1],
            old_normal[1]);                                                                           
        Extract_Normal_Direct(n,
            n3,v[tria3],v[tria1],v[tria2],
            old_normal[2]);                                                                           
        intersection.INormal = old_normal[0];
        if (Compute_Pigment(colour,pig,vertex[0],&intersection,&ray,GetParserDataPtr()))
        {
          modulation[0] = colour.Greyscale();
        }
        else 
        {
          modulation[0] = 1.0;
        }
        intersection.INormal = old_normal[1];
        if (Compute_Pigment(colour,pig,vertex[1],&intersection,&ray,GetParserDataPtr()))
        {
          modulation[1] = colour.Greyscale();
        }
        else 
        {
          modulation[1] = 1.0;
        }
        intersection.INormal = old_normal[2];
        if (Compute_Pigment(colour,pig,vertex[2],&intersection,&ray,GetParserDataPtr()))
        {
          modulation[2] = colour.Greyscale();
        }
        else 
        {
          modulation[2] = 1.0;
        }
      }
      MTransPoint(new_vertex[0],vertex[0],&(info.transform));
      MTransPoint(new_vertex[1],vertex[1],&(info.transform));
      MTransPoint(new_vertex[2],vertex[2],&(info.transform));
      Warp_EPoint(mov_vertex[0],new_vertex[0],&info.dummy_texture);
      Warp_EPoint(mov_vertex[1],new_vertex[1],&info.dummy_texture);
      Warp_EPoint(mov_vertex[2],new_vertex[2],&info.dummy_texture);
      MInvTransPoint(new_vertex[0],mov_vertex[0],&(info.transform));
      MInvTransPoint(new_vertex[1],mov_vertex[1],&(info.transform));
      MInvTransPoint(new_vertex[2],mov_vertex[2],&(info.transform));
      new_vertex[0] = modulation[0]*new_vertex[0]+ (1.0 - modulation[0])*vertex[0];
      new_vertex[1] = modulation[1]*new_vertex[1]+ (1.0 - modulation[1])*vertex[1];
      new_vertex[2] = modulation[2]*new_vertex[2]+ (1.0 - modulation[2])*vertex[2];
      t1 = t2 = t3 = NULL;
      if (!das->albinos)
      {
        if (!(s[indice].Texture<0))
        {
          t1 = (old_texture[s[indice].Texture]);
          if ((s[indice].ThreeTex) && (!(s[indice].Texture2<0)))
          {
            t2 = (old_texture[s[indice].Texture2]);
          }
          if ((s[indice].ThreeTex) && (!(s[indice].Texture3<0)))
          {
            t3 = (old_texture[s[indice].Texture3]);
          }
          if (!(s[indice].ThreeTex))
          {
            t2 = t3 = t1;
          }
        }
      }
      AddTriangle(new_vertex[0],new_vertex[1],new_vertex[2],t1,t2,t3,das);
    }
    return (ObjectPtr)das->tesselationMesh;
  }
   ObjectPtr Parser::Move_Object(ObjectPtr Obj,
      MoveInfo *info,
      UNDERCONSTRUCTION *das
      )
  {
    Mesh * meshobj= (Mesh *)Obj;
    MESH_TRIANGLE*s = meshobj->Data->Triangles;
    SnglVector3d*v = meshobj->Data->Vertices;
    SnglVector3d*n = meshobj->Data->Normals;
    TransColour colour;
    int tria1,tria2,tria3;
    int n1,n2,n3;
    Vector3d vertex[3];
    DBL modulation[3];
    TEXTURE *t1,*t2,*t3;
    TEXTURE **old_texture;
    Vector3d new_vertex[3];
    Vector3d old_normal[3];
    Intersection intersection;
    TraceTicket ticket( 1, 0.0);
    Ray ray( ticket );
    int limit;
    int indice;
    PIGMENT *pig=NULL;
    if (info->NPat)
    {
      pig = info->NPat->Pigment;
    }
    else
    {
      modulation[0]=1.0;
      modulation[1]=1.0;
      modulation[2]=1.0;
    }
    limit = meshobj->Data->Number_Of_Triangles;
    old_texture = meshobj->Textures;

    for(indice = 0; indice < limit; indice++)
    {
      tria1 = s[indice].P1;
      tria2 = s[indice].P2;
      tria3 = s[indice].P3;
      n1 = s[indice].N1;
      n2 = s[indice].N2;
      n3 = s[indice].N3; 

      vertex[0] = Vector3d( v[tria1] );
      vertex[1] = Vector3d( v[tria2] );
      vertex[2] = Vector3d( v[tria3] );
      if (meshobj->Trans)
      {
        MTransPoint(vertex[0],vertex[0],meshobj->Trans);
        MTransPoint(vertex[1],vertex[1],meshobj->Trans);
        MTransPoint(vertex[2],vertex[2],meshobj->Trans);
      } 
      if (info->NPat)
      {
        Extract_Normal_Direct(n,
            n1,v[tria1],v[tria2],v[tria3],
            old_normal[0]);                                                                           
        Extract_Normal_Direct(n,
            n2,v[tria2],v[tria3],v[tria1],
            old_normal[1]);                                                                           
        Extract_Normal_Direct(n,
            n3,v[tria3],v[tria1],v[tria2],
            old_normal[2]);                                                                           
        intersection.INormal = old_normal[0];
        if (Compute_Pigment(colour,pig,vertex[0],&intersection,&ray,GetParserDataPtr()))
        {
          modulation[0] = colour.Greyscale();
        }
        else 
        {
          modulation[0] = 1.0;
        }
        intersection.INormal = old_normal[1];
        if (Compute_Pigment(colour,pig,vertex[1],&intersection,&ray,GetParserDataPtr()))
        {
          modulation[1] = colour.Greyscale();
        }
        else 
        {
          modulation[1] = 1.0;
        }
        intersection.INormal = old_normal[2];
        if (Compute_Pigment(colour,pig,vertex[2],&intersection,&ray,GetParserDataPtr()))
        {
          modulation[2] = colour.Greyscale();
        }
        else 
        {
          modulation[2] = 1.0;
        }
      }
      MTransPoint(new_vertex[0],vertex[0],&(info->transform));
      MTransPoint(new_vertex[1],vertex[1],&(info->transform));
      MTransPoint(new_vertex[2],vertex[2],&(info->transform));
      new_vertex[0]=modulation[0]*new_vertex[0]+ (1.0 - modulation[0])*vertex[0];
      new_vertex[1]=modulation[1]*new_vertex[1]+ (1.0 - modulation[1])*vertex[1];
      new_vertex[2]=modulation[2]*new_vertex[2]+ (1.0 - modulation[2])*vertex[2];
      t1 = t2 = t3 = NULL;
      if (!das->albinos)
      {
        if (!(s[indice].Texture<0))
        {
          t1 = (old_texture[s[indice].Texture]);
          if ((s[indice].ThreeTex) && (!(s[indice].Texture2<0)))
          {
            t2 = (old_texture[s[indice].Texture2]);
          }
          if ((s[indice].ThreeTex) && (!(s[indice].Texture3<0)))
          {
            t3 = (old_texture[s[indice].Texture3]);
          }
          if (!(s[indice].ThreeTex))
          {
            t2 = t3 = t1;
          }
        }
      }
      AddTriangle(new_vertex[0],new_vertex[1],new_vertex[2],t1,t2,t3,das);
    }
    return (ObjectPtr)das->tesselationMesh;
  }
   ObjectPtr Parser::Select_Object(ObjectPtr Obj,
      SelectInfo *info,
      UNDERCONSTRUCTION *das
      )
  {
    Mesh * meshobj= (Mesh *)Obj;
    ObjectPtr  boundary = info->bound;
    MESH_TRIANGLE*s = meshobj->Data->Triangles;
    SnglVector3d*v = meshobj->Data->Vertices;
    SnglVector3d*n = meshobj->Data->Normals;
    Vector3d vertex;
    Vector3d new_vertex[3],old_normal[3];
    TEXTURE *t1,*t2,*t3;
    int *inside;
    int tria1,tria2,tria3;
    int count;
    int n1,n2,n3;
    TEXTURE **old_texture;
    int limit;
    int indice,doit;
    limit = meshobj->Data->Number_Of_Triangles;
    old_texture = meshobj->Textures;
    n1 = meshobj->Data->Number_Of_Vertices;
    inside = (int*)POV_MALLOC(meshobj->Data->Number_Of_Vertices*sizeof(int),
        "temporary select triangle mesh data"); 
    boundary->Compute_BBox();
    Obj->Compute_BBox();
    for(indice = 0; indice < n1; indice++)
    {
      vertex = Vector3d( v[indice] ); 
      if (meshobj->Trans)
      { MTransPoint(vertex,vertex,meshobj->Trans); }
      inside[indice] = boundary->Inside(vertex, GetParserDataPtr())?1:0;
    }
    for(indice = 0; indice < limit; indice++)
    {
      doit = 0;
      tria1 = s[indice].P1;
      tria2 = s[indice].P2;
      tria3 = s[indice].P3;
      count = inside[tria1]+inside[tria2]+inside[tria3];
      switch(count)
      {
        case 0:
          doit = info->outside;
          break;
        case 1:
          doit = info->bout;
          break;
        case 2:
          doit = info->bin;
          break;
        case 3:
          doit = info->inside;
          break;
      }
      if (doit)
      {
        n1 = s[indice].N1;
        n2 = s[indice].N2;
        n3 = s[indice].N3;
        new_vertex[0] = Vector3d( v[tria1] );
        new_vertex[1] = Vector3d( v[tria2] );
        new_vertex[2] = Vector3d( v[tria3] );
        if (meshobj->Trans)
        {
          MTransPoint(new_vertex[0],new_vertex[0],meshobj->Trans);
          MTransPoint(new_vertex[1],new_vertex[1],meshobj->Trans);
          MTransPoint(new_vertex[2],new_vertex[2],meshobj->Trans);
        } 
        Extract_Normal_Direct(n,
            n1,v[tria1],v[tria2],v[tria3],
            old_normal[0]);                                                                           
        Extract_Normal_Direct(n,
            n2,v[tria2],v[tria3],v[tria1],
            old_normal[1]);                                                                           
        Extract_Normal_Direct(n,
            n3,v[tria3],v[tria1],v[tria2],
            old_normal[2]);                                                                           
        if (meshobj->Trans)
        {
          MTransNormal(old_normal[0],old_normal[0],meshobj->Trans);
          MTransNormal(old_normal[1],old_normal[1],meshobj->Trans);
          MTransNormal(old_normal[2],old_normal[2],meshobj->Trans);
        } 
        t1 = t2 = t3 = NULL;
        if (!das->albinos)
        {
          if (!(s[indice].Texture<0))
          {
            t1 = (old_texture[s[indice].Texture]);
            if ((s[indice].ThreeTex) && (!(s[indice].Texture2<0)))
            {
              t2 = (old_texture[s[indice].Texture2]);
            }
            if ((s[indice].ThreeTex) && (!(s[indice].Texture3<0)))
            {
              t3 = (old_texture[s[indice].Texture3]);
            }
            if (!(s[indice].ThreeTex))
            {
              t2 = t3 = t1;
            }
          }
        }
        AddSmoothTriangle(new_vertex[0],new_vertex[1],new_vertex[2],t1,t2,t3,
            old_normal[0],old_normal[1],old_normal[2],das);
      }
    }
    POV_FREE(inside);
    return (ObjectPtr)das->tesselationMesh;
  }

  ObjectPtr Parser::Parse_Screw(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    UNDERCONSTRUCTION das;
    ScrewCoordInfo info;

    Parse_Begin();


    info.NPat=NULL;
    info.have_min = 0;
    info.inverse = 1;
    info.have_max = 0;
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    info.origin = Vector3d(0,0,0);
    info.angle = Vector3d(0,0,0);
    EXPECT
      CASE (ORIGINAL_TOKEN)
      Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
             das.Inside_Vect  =  ThisMeshPtr->Data->Inside_Vect;
            }
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE
      CASE (ORIGIN_TOKEN)
      Parse_Vector(info.origin);
    END_CASE
      CASE(MODULATION_TOKEN)
      Parse_Begin();
    info.NPat=Parse_Texture();
    Parse_End();
    END_CASE 
      CASE (ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE
      CASE (RIGHT_TOKEN)
      info.inverse = -1;
    END_CASE
      CASE (DIRECTION_TOKEN)
      Parse_Vector(info.angle);
    END_CASE
      CASE (MINIMAL_TOKEN)
      info.min_angle = Parse_Float();
    info.have_min = 1;
    END_CASE
      CASE (MAXIMAL_TOKEN)
      info.max_angle = Parse_Float();
    info.have_max = 1;
    END_CASE
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");
    if (info.angle.lengthSqr() != 0)
    {
      info.axis = info.angle.normalized();
    }

    StartAddingTriangles(&das);
    Res = Screwlise_Object(Object, &info,&das); 
    Debug_Info("screw : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);
    Destroy_Textures(info.NPat);

    return Res;
  }



  ObjectPtr Parser::Parse_Roll(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    ScrewCoordInfo info;
    UNDERCONSTRUCTION das;

    Parse_Begin();


    info.NPat=NULL;
    info.have_min = 0;
    info.have_max = 0;
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    info.origin = Vector3d(0,0,0);
    info.angle = Vector3d(0,0,0);
    EXPECT
      CASE (ORIGINAL_TOKEN)
      Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
             das.Inside_Vect  =  ThisMeshPtr->Data->Inside_Vect;
            }
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE
      CASE (ORIGIN_TOKEN)
      Parse_Vector(info.origin);
    END_CASE
      CASE(MODULATION_TOKEN)
      Parse_Begin();
    info.NPat=Parse_Texture();
    Parse_End();
    END_CASE 
      CASE (ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE
      CASE (DIRECTION_TOKEN)
      Parse_Vector(info.angle);
    END_CASE
      CASE (MINIMAL_TOKEN)
      info.min_angle = Parse_Float();
    info.have_min = 1;
    END_CASE
      CASE (MAXIMAL_TOKEN)
      info.max_angle = Parse_Float();
    info.have_max = 1;
    END_CASE
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");
    if (info.angle.lengthSqr() != 0)
    {
      info.axis = info.angle.normalized();
    }

    StartAddingTriangles(&das);
    Res = Roll_Object(Object, &info,&das); 
    Debug_Info("roll : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);
    Destroy_Textures(info.NPat);

    return Res;
  }



  ObjectPtr Parser::Parse_Bend(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    ScrewCoordInfo info;
    UNDERCONSTRUCTION das;
    Vector3d check;

    Parse_Begin();
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    info.NPat=NULL;
    info.amount = 0;
    info.have_min = 0;
    info.have_max = 0;
    info.origin = Vector3d(0,0,0);
    info.angle = Vector3d(0,1,0);
    info.no_move = Vector3d(1,0,0);
    EXPECT
      CASE (ORIGINAL_TOKEN)
      Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
             das.Inside_Vect  =  ThisMeshPtr->Data->Inside_Vect;
            }
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE
      CASE (ORIGIN_TOKEN)
      Parse_Vector(info.origin);
    END_CASE
      CASE(MODULATION_TOKEN)
      Parse_Begin();
    info.NPat=Parse_Texture();
    Parse_End();
    END_CASE 
      CASE (FIXED_TOKEN)
      Parse_Vector(info.no_move);
    END_CASE
      CASE (DIRECTION_TOKEN)
      Parse_Vector(info.angle);
    END_CASE
      CASE (AMOUNT_TOKEN)
      info.amount = Parse_Float();
    END_CASE
      CASE (ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE
      CASE (MINIMAL_TOKEN)
      info.min_angle = Parse_Float();
    info.have_min = 1;
    END_CASE
      CASE (MAXIMAL_TOKEN)
      info.max_angle = Parse_Float();
    info.have_max = 1;
    END_CASE
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");
    check = cross(info.no_move,info.angle);
    if (!check.lengthSqr())
    {
      info.amount = 0; /* nothing to do, colinear vector */
    }

    StartAddingTriangles(&das);
    Res = Bend_Object(Object, &info,&das); 
    Debug_Info("bend : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);
    Destroy_Object(Object);
    Destroy_Textures(info.NPat);

    return Res;
  }


  ObjectPtr Parser::Parse_Smooth(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    SmoothInfo info;
    UNDERCONSTRUCTION das;

    Parse_Begin();


    info.method=0;
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    info.amount=1.0;
    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
             das.Inside_Vect  =  ThisMeshPtr->Data->Inside_Vect;
            }
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE 
      CASE(METHOD_TOKEN)
      info.method=(int)Allow_Float(0.0);
    END_CASE 
      CASE(AMOUNT_TOKEN)
      info.amount=Allow_Float(1.0);
    END_CASE 
      CASE (ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");

    StartAddingTriangles(&das);
    Res = Smooth_Object(Object, &info,&das); 
    Debug_Info("smooth : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);

    return Res;
  }

  ObjectPtr Parser::Parse_Select(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    SelectInfo info;
    UNDERCONSTRUCTION das;

    Parse_Begin();

    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    info.bound= NULL;
    info.outside = 0;
    info.bin = 0;
    info.bout = 0;
    info.inside = 0;
    EXPECT
      CASE (ORIGINAL_TOKEN)
      Object = Parse_Object();
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE
      CASE (WITH_TOKEN)
      info.bound = Parse_Object();
    END_CASE
      CASE (INNER_TOKEN)
      info.inside = 1;
    END_CASE
      CASE (OUTSIDE_TOKEN)
      info.outside = 1;
    END_CASE
      CASE (INBOUND_TOKEN)
      info.bin = 1;
    END_CASE
      CASE (ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE
      CASE (OUTBOUND_TOKEN)
      info.bout = 1;
    END_CASE
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");
    if(!info.bound) Error("Bounding Object expected.\n");

    StartAddingTriangles(&das);
    Res = Select_Object(Object, &info,&das); 
    Debug_Info("keep : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);
    Destroy_Object(info.bound);

    return Res;
  }


   void Parser::Compute_Normal(Vector3d& Norm,Vector3d& V0,Vector3d& V1,Vector3d& V2,Vector3d& old)
  {
    Vector3d S1,S2;
    DBL angle;

    S1 = V1 - V0;
    S2 = V2 - V0;
    Norm = cross(S1,S2);
    angle = dot(Norm,old);
    if (angle <0)
    {
      Norm *= -1;
    }
  }

   ObjectPtr Parser::Displace_Object(ObjectPtr Obj,
      DisplaceInfo *info, UNDERCONSTRUCTION *das)
  {
    Intersection intersection;
    TraceTicket ticket( 1, 0.0);
    Ray ray( ticket );
    Mesh * meshobj= (Mesh *)Obj;
    MESH_TRIANGLE*s = meshobj->Data->Triangles;
    SnglVector3d*v = meshobj->Data->Vertices;
    SnglVector3d*n = meshobj->Data->Normals;
    int tria1,tria2,tria3;
    int norma;
    int n1,n2,n3;
    TEXTURE *t1,*t2,*t3;
    DBL amount[3];
    TransColour colour;
    Vector3d new_vertex[3];
    TEXTURE **old_texture;
    Vector3d new_normal[3];
    Vector3d old_normal[3];
    PIGMENT *pig;
    int limit;
    int indice;
    pig = info->NPat->Pigment;
    limit = meshobj->Data->Number_Of_Triangles;
    old_texture = meshobj->Textures;
    norma = meshobj->Data->Number_Of_Normals;
    n1 = meshobj->Data->Number_Of_Vertices;
    for(indice = 0; indice < limit; indice++)
    {
      Cooperate();
      tria1 = s[indice].P1;
      tria2 = s[indice].P2;
      tria3 = s[indice].P3;
      n1 = s[indice].N1;
      n2 = s[indice].N2;
      n3 = s[indice].N3;
      new_vertex[0] = Vector3d( v[tria1] );
      new_vertex[1] = Vector3d( v[tria2] );
      new_vertex[2] = Vector3d( v[tria3] );

      if (meshobj->Trans) 
      {
        MTransPoint(new_vertex[0],new_vertex[0],meshobj->Trans);
        MTransPoint(new_vertex[1],new_vertex[1],meshobj->Trans);
        MTransPoint(new_vertex[2],new_vertex[2],meshobj->Trans);
      }
      Extract_Normal_Direct(n,
          n1,v[tria1],v[tria2],v[tria3],
          old_normal[0]);                                                                           
      Extract_Normal_Direct(n,
          n2,v[tria2],v[tria3],v[tria1],
          old_normal[1]);                                                                           
      Extract_Normal_Direct(n,
          n3,v[tria3],v[tria1],v[tria2],
          old_normal[2]);                                                                           
      intersection.INormal = old_normal[0];
      if (Compute_Pigment(colour,pig,new_vertex[0],&intersection,&ray,GetParserDataPtr()))
      {
        amount[0] = colour.Greyscale();
      }
      else 
      {
        amount[0] = info->offset;
      }
      intersection.INormal = old_normal[1];
      if (Compute_Pigment(colour,pig,new_vertex[1],&intersection,&ray,GetParserDataPtr()))
      {
        amount[1] = colour.Greyscale();
      }
      else 
      {
        amount[1] = info->offset;
      }
      intersection.INormal = old_normal[2];
      if (Compute_Pigment(colour,pig,new_vertex[2],&intersection,&ray,GetParserDataPtr()))
      {
        amount[2] = colour.Greyscale();
      }
      else 
      {
        amount[2] = info->offset;
      }
      if (info->have_inside)
      { /* flip normals if they are not in the right direction.
         *
         * It is right when the dot product of normal with relative vector of position is positive
         * ([JG]: that's just my choice).
         * notice, when 0, there is no "good" choice.
         */
        Vector3d rel[3];
        DBL sgn;
        rel[0] = new_vertex[0] - info->inside;
        sgn = dot( rel[0],old_normal[0]);
        if (sgn <0)
        {
          old_normal[0] *= -1.0;
        }

        rel[1] = new_vertex[1] - info->inside;
        sgn = dot( rel[1],old_normal[1]);
        if (sgn <0)
        {
          old_normal[1] *= -1.0;
        }

        rel[2] = new_vertex[2] - info->inside;
        sgn = dot( rel[2],old_normal[2]);
        if (sgn <0)
        {
          old_normal[2] *= -1.0;
        }
      }
      amount[0] -= info->offset;
      amount[0] *= info->amount;
      new_normal[0] = old_normal[0] * amount[0];
      new_vertex[0] += new_normal[0];

      amount[1] -= info->offset;
      amount[1] *= info->amount;
      new_normal[1] = old_normal[1] * amount[1];
      new_vertex[1] += new_normal[1];

      amount[2] -= info->offset;
      amount[2] *= info->amount;
      new_normal[2] = old_normal[2] * amount[2];
      new_vertex[2] += new_normal[2];

      Compute_Normal(new_normal[0],new_vertex[0],
          new_vertex[1],new_vertex[2],old_normal[0]); 
      Compute_Normal(new_normal[1],new_vertex[1],
          new_vertex[2],new_vertex[0],old_normal[1]); 
      Compute_Normal(new_normal[2],new_vertex[2],
          new_vertex[0],new_vertex[1],old_normal[2]); 
      t1 = t2 = t3 = NULL;
      if (!das->albinos)
      {
        if (!(s[indice].Texture<0))
        {
          t1 = (old_texture[s[indice].Texture]);
          if ((s[indice].ThreeTex) && (!(s[indice].Texture2<0)))
          {
            t2 = (old_texture[s[indice].Texture2]);
          }
          if ((s[indice].ThreeTex) && (!(s[indice].Texture3<0)))
          {
            t3 = (old_texture[s[indice].Texture3]);
          }
          if (!(s[indice].ThreeTex))
          {
            t2 = t3 = t1;
          }
        }
      }
      AddSmoothTriangle(new_vertex[0],new_vertex[1],new_vertex[2],t1,t2,t3,
          new_normal[0],new_normal[1],new_normal[2],das);
    }
    return (ObjectPtr)das->tesselationMesh;
  }
  ObjectPtr Parser::Parse_Displace(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    DisplaceInfo info;
    UNDERCONSTRUCTION das;

    Parse_Begin();


    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    info.NPat = NULL;
    info.amount=1.0;
    info.offset=0.5;
    info.have_inside=0;
    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
             das.Inside_Vect  =  ThisMeshPtr->Data->Inside_Vect;
            }
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE 
      CASE(MODULATION_TOKEN)
      Parse_Begin();
    info.NPat=Parse_Texture();
    Parse_End();
    END_CASE 
      CASE(AMOUNT_TOKEN)
      info.amount=Allow_Float(1.0);
    END_CASE 
      CASE (ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE
      CASE(OFFSET_TOKEN)
      info.offset=Allow_Float(0.5);
    END_CASE 
      CASE(INSIDE_POINT_TOKEN)
      Parse_Vector(info.inside);
      info.have_inside=1;
    END_CASE 
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");
    if(!info.NPat) Error("modulation { texture... } expected.\n");
    if(!info.NPat->Pigment) Error("pigment in modulation expected. (texture{ pigment{ ... pigment_map... rather than texture_map)\n");

    StartAddingTriangles(&das);
    Res = Displace_Object(Object, &info,&das); 
    Debug_Info("displace : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);
    Destroy_Textures(info.NPat);

    return Res;
  }

  /*----------------------------------------------------------------------------
    ==  Apply a warp to an object
    ----------------------------------------------------------------------------*/
  ObjectPtr Parser::Parse_Warp_Object(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    WarpInfo info;
    UNDERCONSTRUCTION das;

    Parse_Begin();

    info.NPat = NULL;
    Init_TPat_Fields(&info.dummy_texture);
    MIdentity(info.transform.matrix);
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
             das.Inside_Vect  =  ThisMeshPtr->Data->Inside_Vect;
            }
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE
      CASE(MODULATION_TOKEN)
      Parse_Begin();
    info.NPat=Parse_Texture();
    Parse_End();
    END_CASE 
      CASE(WARP_TOKEN)
      Parse_Warp (info.dummy_texture.pattern->warps);
    END_CASE
      CASE (ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE
      CASE (MOVE_TOKEN)
      Parse_Matrix(info.transform.matrix);
    END_CASE
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");
    if(info.dummy_texture.pattern->warps.empty()) Error("Warp expected.\n");
    MInvers(info.transform.inverse,info.transform.matrix);

    StartAddingTriangles(&das);
    Res = Warp_Object(Object, info,&das); 
    Debug_Info("warp : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);
    //Destroy_Warps(info.warp);
    Destroy_Textures(info.NPat);

    return Res;
  }

  /*----------------------------------------------------------------------------
    ==  Apply a matrix to an object
    ----------------------------------------------------------------------------*/

  ObjectPtr Parser::Parse_Move_Object(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    MoveInfo info;
    UNDERCONSTRUCTION das;

    Parse_Begin();

    info.NPat = NULL;
    MIdentity(info.transform.matrix);
    das.albinos = 0;
    das.Default_Texture = NULL;
        das.Inside_Vect = Vector3d( 0, 0, 0);
    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
             das.Inside_Vect  =  ThisMeshPtr->Data->Inside_Vect;
            }
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE
      CASE(MODULATION_TOKEN)
      Parse_Begin();
    info.NPat=Parse_Texture();
    Parse_End();
    END_CASE 
      CASE (ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE
      CASE (MOVE_TOKEN)
      Parse_Matrix(info.transform.matrix);
    END_CASE
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");
    MInvers(info.transform.inverse,info.transform.matrix);

    StartAddingTriangles(&das);
    Res = Move_Object(Object, &info,&das); 
    Debug_Info("move : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);
    Destroy_Textures(info.NPat);

    return Res;
  }


  /*======================================================================
    Triangle data extraction patch
    ======================================================================*/

   Mesh* Parser::ParseParameter(int OneParam)
  {
    Mesh* Obj = NULL;

    GET (LEFT_PAREN_TOKEN);

    EXPECT
      CASE (OBJECT_ID_TOKEN)
      if(((Mesh*)Token.Data)->Type == MESH_OBJECT)
      {
        Obj = (Mesh*)Token.Data;
      }
    EXIT
      END_CASE

      OTHERWISE
      Obj = NULL;
    UNGET
      EXIT
      END_CASE
      END_EXPECT

      if(!Obj)
      {
        Error("Mesh object identifier expected.\n");
      }

    if(OneParam)
    {
      GET (RIGHT_PAREN_TOKEN);
    }
    return Obj;
  }

   int Parser::ParseSecondParameter(void)
  {
    int val;
    Parse_Comma();
    val = (int)Parse_Float();
    GET (RIGHT_PAREN_TOKEN);
    return val;
  }

  DBL Parser::Parse_Is_Smooth_Triangle(void)
  {
    Mesh* meshobj = ParseParameter(false);
    int ind = ParseSecondParameter();
    if(ind < 0 || ind >= meshobj->Data->Number_Of_Triangles)
    {
      Error("Index value out of range.\n");
    }
    return meshobj->Data->Triangles[ind].Smooth;
  }

  DBL Parser::Parse_Get_Triangles_Amount(void)
  {
    Mesh* meshobj = ParseParameter(true);
    return meshobj->Data->Number_Of_Triangles;
  }

  DBL Parser::Parse_Get_Vertices_Amount(void)
  {
    Mesh* meshobj = ParseParameter(true);
    return meshobj->Data->Number_Of_Vertices;
  }

  DBL Parser::Parse_Get_Normals_Amount(void)
  {
    Mesh* meshobj = ParseParameter(true);
    return meshobj->Data->Number_Of_Normals;
  }

  void Parser::Parse_Get_Vertex(Vector3d& Vect)
  {
    Mesh* meshobj = ParseParameter(false);
    SnglVector3d* s = meshobj->Data->Vertices;
    int ind = ParseSecondParameter();

    if(ind < 0 || ind >= meshobj->Data->Number_Of_Vertices)
    {
      Error("Index value out of range.\n");
    }

    Vect[X] = s[ind][X];
    Vect[Y] = s[ind][Y];
    Vect[Z] = s[ind][Z];
  }

  void Parser::Parse_Get_Normal(Vector3d& Vect)
  {
    Mesh* meshobj = ParseParameter(false);
    SnglVector3d* s = meshobj->Data->Normals;
    int ind = ParseSecondParameter();

    if(ind < 0 || ind >= meshobj->Data->Number_Of_Normals)
    {
      Error("Index value out of range.\n");
    }

    Vect[X] = s[ind][X];
    Vect[Y] = s[ind][Y];
    Vect[Z] = s[ind][Z];
  }

  void Parser::Parse_Get_Vertex_Indices(Vector3d& Vect)
  {
    Mesh* meshobj = ParseParameter(false);
    MESH_TRIANGLE* s = meshobj->Data->Triangles;
    int ind = ParseSecondParameter();

    if(ind < 0 || ind >= meshobj->Data->Number_Of_Triangles)
    {
      Error("Index value out of range.\n");
    }

    Vect[X] = s[ind].P1;
    Vect[Y] = s[ind].P2;
    Vect[Z] = s[ind].P3;
  }

  void Parser::Parse_Get_Normal_Indices(Vector3d& Vect)
  {
    Mesh* meshobj = ParseParameter(false);
    MESH_TRIANGLE* s = meshobj->Data->Triangles;
    int ind = ParseSecondParameter();

    if(ind < 0 || ind >= meshobj->Data->Number_Of_Triangles)
    {
      Error("Index value out of range.\n");
    }

    Vect[X] = s[ind].N1;
    Vect[Y] = s[ind].N2;
    Vect[Z] = s[ind].N3;
  }

   ObjectPtr Parser::Planet_Object(ObjectPtr Obj,
      PlanetInfo *info, UNDERCONSTRUCTION *das)
  {
    Intersection intersection;
    TraceTicket ticket( 1, 0.0);
    Ray ray( ticket );
    Mesh * meshobj= (Mesh *)Obj;
    MESH_TRIANGLE*s = meshobj->Data->Triangles;
    SnglVector3d*v = meshobj->Data->Vertices;
    SnglVector3d*n = meshobj->Data->Normals;
    int vlimit;
    int indice;
    unsigned int iteration;
    vlimit = meshobj->Data->Number_Of_Vertices;
    SnglVector3d *sv=meshobj->Data->Vertices;
    int*counter= (int*)POV_MALLOC(vlimit*sizeof(int),
        "temporary offset data in planet (VERTICES)");
    for(indice=0;indice<vlimit;++indice)
    {
       counter[indice]=0;
    }
    Debug_Info("\nplanet : %d vertices, for %d iterations\n",vlimit,info->iteration);
    DBL theta,u;
    DBL plan_result;
    Vector3d plan;
    Vector3d vertex;
    DBL plan_offset;
    DBL evaluation;
    int key=stream_seed(info->seed);
    for(iteration=info->iteration;iteration;--iteration)
    {
       /* 
        * 1. Select anisotropic random direction : V
        * 2. Choose a random value between -jitter and +jitter : D
        * 3. create a plane with offset of D from info->center and normal vector V
        * 4. use the plane to split the universe in two parts
        *  4.1 for each vertices in the first part, add one to counter of that vertice index
        *  4.2 for each vertices in the second part, remove one to counter of that vertice index
        */
        // 1 : http://mathworld.wolfram.com/SpherePointPicking.html
        theta = TWO_M_PI*(stream_rand(key)); // 0 to 2.pi
        u = -1.0 + 2.0*(stream_rand(key));// -1 to 1
        plan[X]=sqrt(1.0-u*u)*cos(theta);
        plan[Y]=sqrt(1.0-u*u)*sin(theta);
        plan[Z]=u;
        plan_result = dot(plan,info->center);

        // 2.
        plan_offset = (-1.0 +2.0*stream_rand(key)) * info->jitter;
        // 3. 
        plan_result += plan_offset;
        // 4.
        for(indice=0;indice<vlimit;++indice)
        {
          vertex[X] = sv[indice][X];
          vertex[Y] = sv[indice][Y];
          vertex[Z] = sv[indice][Z];
          evaluation = dot( plan, vertex);
          if (evaluation < plan_result)
          {
             counter[indice]-=1;
          }
          else
          {
             counter[indice]+=1;
          }
        }
    }
    Debug_Info("planet : updating the %d vertices             \n",vlimit);
    // normalisation of counter
    int cmax=0;
    int cmin=0;
    for(indice =0;indice< vlimit;indice++)
    {
      cmax = std::max(cmax,counter[indice]);
      cmin = std::min(cmin,counter[indice]);
    }
    if (abs(cmax)>(abs(cmin)))
    {
      info->amount /= abs(cmax);
    }
    else
    {
      info->amount /= abs(cmin);
    }
    /* move vertices according to counter, 
     *     away from info->center when positive, 
     *     toward info->center when negative 
     * size of displacement is proportional to the value of counter and info->amount
     */
    
    int tria1,tria2,tria3;
    int norma;
    int n1,n2,n3;
    TEXTURE *t1,*t2,*t3;
    DBL amount[3];
    TransColour colour;
    Vector3d new_vertex[3];
    TEXTURE **old_texture;
    Vector3d new_normal[3];
    Vector3d old_normal[3];
    int limit;
    limit = meshobj->Data->Number_Of_Triangles;
    old_texture = meshobj->Textures;
    norma = meshobj->Data->Number_Of_Normals;
    n1 = meshobj->Data->Number_Of_Vertices;
    Debug_Info("planet : updating the %d triangles             \n",limit);
    for(indice = 0; indice < limit; indice++)
    {
      Cooperate();
      tria1 = s[indice].P1;
      tria2 = s[indice].P2;
      tria3 = s[indice].P3;
      n1 = s[indice].N1;
      n2 = s[indice].N2;
      n3 = s[indice].N3;
      new_vertex[0] = Vector3d( v[tria1] );
      new_vertex[1] = Vector3d( v[tria2] );
      new_vertex[2] = Vector3d( v[tria3] );

      if (meshobj->Trans) 
      {
        MTransPoint(new_vertex[0],new_vertex[0],meshobj->Trans);
        MTransPoint(new_vertex[1],new_vertex[1],meshobj->Trans);
        MTransPoint(new_vertex[2],new_vertex[2],meshobj->Trans);
      }
      old_normal[0] = new_vertex[0] - info->center;
      old_normal[0].normalize();
      amount[0] = info->amount * counter[tria1];
      new_vertex[0] += amount[0] *  old_normal[0];

      old_normal[1] = new_vertex[1] - info->center;
      old_normal[1].normalize();
      amount[1] = info->amount * counter[tria2];
      new_vertex[1] += amount[1] *  old_normal[1];

      old_normal[2] = new_vertex[2] - info->center;
      old_normal[2].normalize();
      amount[2] = info->amount * counter[tria3];
      new_vertex[2] += amount[2] *  old_normal[2];

      Compute_Normal(new_normal[0],new_vertex[0],
          new_vertex[1],new_vertex[2],old_normal[0]); 
      Compute_Normal(new_normal[1],new_vertex[1],
          new_vertex[2],new_vertex[0],old_normal[1]); 
      Compute_Normal(new_normal[2],new_vertex[2],
          new_vertex[0],new_vertex[1],old_normal[2]); 
      t1 = t2 = t3 = NULL;
      if (!das->albinos)
      {
        if (!(s[indice].Texture<0))
        {
          t1 = (old_texture[s[indice].Texture]);
          if ((s[indice].ThreeTex) && (!(s[indice].Texture2<0)))
          {
            t2 = (old_texture[s[indice].Texture2]);
          }
          if ((s[indice].ThreeTex) && (!(s[indice].Texture3<0)))
          {
            t3 = (old_texture[s[indice].Texture3]);
          }
          if (!(s[indice].ThreeTex))
          {
            t2 = t3 = t1;
          }
        }
      }
      AddSmoothTriangle(new_vertex[0],new_vertex[1],new_vertex[2],t1,t2,t3,
          new_normal[0],new_normal[1],new_normal[2],das);
    }
    Debug_Info("%d / %d triangles done\n",limit,limit);
    POV_FREE(counter);
    return (ObjectPtr)das->tesselationMesh;
  }
  ObjectPtr Parser::Parse_Planet(void)
  {
    ObjectPtr Object = NULL;
    ObjectPtr Res = NULL;
    PlanetInfo info;
    UNDERCONSTRUCTION das;

    Parse_Begin();


    das.albinos = 0;
    das.Default_Texture = NULL;
    info.center = Vector3d(0,0,0);
    info.amount=0.0;
    info.jitter=0.0;
    info.iteration=0;
    EXPECT
      CASE(ORIGINAL_TOKEN)
      Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
             das.Inside_Vect  =  ThisMeshPtr->Data->Inside_Vect;
            }
        END_CASE
            CASE( INSIDE_VECTOR_TOKEN )
      Parse_Vector(das.Inside_Vect);
    END_CASE 
      CASE(JITTER_TOKEN)
      info.jitter=Allow_Float(0.0);
    END_CASE 
      CASE(AMOUNT_TOKEN)
      info.amount=Allow_Float(0.0);
    END_CASE 
      CASE (ALBINOS_TOKEN)
      das.albinos = 1;
    END_CASE
      CASE(REPEAT_TOKEN)
      info.iteration=Allow_Float(0.0);
      Parse_Comma();
      info.seed=Allow_Float(0.0);
    END_CASE 
      CASE(ORIGIN_TOKEN)
      Parse_Vector(info.center);
    END_CASE 
      OTHERWISE
      UNGET
      EXIT
      END_CASE
      END_EXPECT
      if(!Object) Error("Object expected.\n");
      if(!info.iteration) Error("Must be repeated at least once");

    StartAddingTriangles(&das);
    Res = Planet_Object(Object, &info,&das); 
    Debug_Info("planet : %d triangles\n", das.number_of_triangles);
    DoneAddingTriangles(&das);

    Destroy_Object(Object);
    return Res;
  }
  /*===========================================================================
    || Apply to a part of a mesh only
    ===========================================================================*/
   void Parser::Apply_Texture(bool& full, UNDERCONSTRUCTION *das,int n_tri,TEXTURE *textu)
  {
    long i;
    long hash;
    if (textu)
    {
      hash =  das->tesselationMesh->Mesh_Hash_Texture(&(das->number_of_textures), 
          &(das->max_textures), &(das->Textures), textu); 
      for(i=n_tri;i<das->number_of_triangles;i++)
      {
        if (das->Triangles[i].Texture < 0)
        {
          das->Triangles[i].Texture = hash;
        }
        if (das->Triangles[i].Texture2 < 0)
        {
          das->Triangles[i].Texture2 = hash;
        }
        if (das->Triangles[i].Texture3 < 0)
        {
          das->Triangles[i].Texture3 = hash;
        }
      }
    }
    else
    {
      for(i=n_tri;i<das->number_of_triangles;i++)
      {
        if (das->Triangles[i].Texture < 0)
        {
          full = false;
          return;
        }
      }
    }
  }
  void Parser::Parse_Bourke_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      TEXTURE *texture = NULL;
      Vector3d Accuracy;
      int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
      DBL BBoxOffs = 0.0;
      int Precision =0;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_textures = *n_tex;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.max_triangles = *m_tri;
      das.max_textures = *m_tex;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Textures = *textu;
      das.Vertices = *verts;
      das.Normals = *norms;

      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      Parse_Begin();


      EXPECT
        CASE(ORIGINAL_TOKEN)
        Object = Parse_Object();
      END_CASE
        CASE(ACCURACY_TOKEN)
        Parse_Vector(Accuracy);
      XAccuracy = (int)(.5+Accuracy[X]);
      YAccuracy = (int)(.5+Accuracy[Y]);
      ZAccuracy = (int)(.5+Accuracy[Z]);
      if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
        Error("accuracy must be greater than 0.\n");
      END_CASE

        CASE(PRECISION_TOKEN)
        Precision = (int)Parse_Float();
      END_CASE

        CASE(OFFSET_TOKEN)
        BBoxOffs = Parse_Float();
      END_CASE
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 

        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT

        if(!Object) Error("Object expected.\n");
      BourkeHeller_Object(Object,0, XAccuracy, YAccuracy, ZAccuracy,
          Precision,BBoxOffs,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_tex = das.number_of_textures;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *m_tri = das.max_triangles;
      *m_tex = das.max_textures;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *textu = das.Textures;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);

      Parse_End();
    }
  void Parser::Parse_Warp_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      TEXTURE *texture = NULL;
      WarpInfo info;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;
      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }


      Parse_Begin();
      MIdentity(info.transform.matrix);
      info.NPat = NULL;
      Init_TPat_Fields(&info.dummy_texture);
      EXPECT
        CASE(ORIGINAL_TOKEN)
        Object = Parse_Object();
      END_CASE
        CASE(WARP_TOKEN)
        Parse_Warp (info.dummy_texture.pattern->warps);
      END_CASE
        CASE(MODULATION_TOKEN)
        Parse_Begin();
      info.NPat=Parse_Texture();
      Parse_End();
      END_CASE 
        CASE (ALBINOS_TOKEN)
        das.albinos = 1;
      END_CASE
        CASE (MOVE_TOKEN)
        Parse_Matrix(info.transform.matrix);
      END_CASE
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 

        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");
      if(info.dummy_texture.pattern->warps.empty()) Error("Warp expected.\n");
      MInvers(info.transform.inverse,info.transform.matrix);

      Warp_Object(Object, info,&das); 
      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);
      Destroy_Textures(info.NPat);

      Parse_End();
    }
  void Parser::Parse_Move_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      TEXTURE *texture = NULL;
      MoveInfo info;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;
      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }


      Parse_Begin();
      MIdentity(info.transform.matrix);
      info.NPat = NULL;
      EXPECT
        CASE(ORIGINAL_TOKEN)
        Object = Parse_Object();
      END_CASE
        CASE(MODULATION_TOKEN)
        Parse_Begin();
      info.NPat=Parse_Texture();
      Parse_End();
      END_CASE 
        CASE (ALBINOS_TOKEN)
        das.albinos = 1;
      END_CASE
        CASE (MOVE_TOKEN)
        Parse_Matrix(info.transform.matrix);
      END_CASE
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 

        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");
      MInvers(info.transform.inverse,info.transform.matrix);

      Move_Object(Object, &info,&das); 
      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);
      Destroy_Textures(info.NPat);

      Parse_End();
    }
  void Parser::Parse_Displace_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      TEXTURE *texture = NULL;
      DisplaceInfo info;
      UNDERCONSTRUCTION das;

      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;

      Parse_Begin();

      info.NPat = NULL;
      info.amount=1.0;
      info.offset=0.5;
      info.have_inside=0;
      EXPECT
        CASE(ORIGINAL_TOKEN)
        Object = Parse_Object();
      END_CASE 
        CASE(MODULATION_TOKEN)
        Parse_Begin();
      info.NPat=Parse_Texture();
      Parse_End();
      END_CASE 
        CASE(AMOUNT_TOKEN)
        info.amount=Allow_Float(1.0);
      END_CASE 
        CASE (ALBINOS_TOKEN)
        das.albinos = 1;
      END_CASE
        CASE(OFFSET_TOKEN)
        info.offset=Allow_Float(0.5);
      END_CASE 
        CASE(INSIDE_POINT_TOKEN)
        Parse_Vector(info.inside);
      info.have_inside=1;
      END_CASE 
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 
        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");
    if(!info.NPat) Error("modulation { texture... } expected.\n");
    if(!info.NPat->Pigment) Error("pigment in modulation expected. (texture{ pigment{ ... pigment_map... rather than texture_map)\n");
      Displace_Object(Object, &info,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);
      Destroy_Textures(info.NPat);

      Parse_End();
    }
  void Parser::Parse_Heller_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      Vector3d Accuracy;
      TEXTURE *texture = NULL;
      int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
      DBL BBoxOffs = 0.0;
      int Precision =0;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;

      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      Parse_Begin();

      EXPECT
        CASE(ORIGINAL_TOKEN)
        Object = Parse_Object();
      END_CASE
        CASE(ACCURACY_TOKEN)
        Parse_Vector(Accuracy);
      XAccuracy = (int)(.5+Accuracy[X]);
      YAccuracy = (int)(.5+Accuracy[Y]);
      ZAccuracy = (int)(.5+Accuracy[Z]);
      if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
        Error("accuracy must be greater than 0.\n");
      END_CASE

        CASE(PRECISION_TOKEN)
        Precision = (int)Parse_Float();
      END_CASE

        CASE(OFFSET_TOKEN)
        BBoxOffs = Parse_Float();
      END_CASE
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 

        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");

      BourkeHeller_Object(Object,1, XAccuracy, YAccuracy, ZAccuracy,
          Precision,BBoxOffs,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);

      Parse_End();
    }
  void Parser::Parse_Tesselation_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      Vector3d Accuracy;
      TEXTURE *texture = NULL;
      int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
      DBL BBoxOffs = 0.0;
      int Smooth = 0;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;

      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      Parse_Begin();

      EXPECT
        CASE(ORIGINAL_TOKEN)
        Object = Parse_Object();
      END_CASE
        CASE(ACCURACY_TOKEN)
        Parse_Vector(Accuracy);
      XAccuracy = (int)(.5+Accuracy[X]);
      YAccuracy = (int)(.5+Accuracy[Y]);
      ZAccuracy = (int)(.5+Accuracy[Z]);
      if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
        Error("accuracy must be greater than 0.\n");
      END_CASE

        CASE(SMOOTH_TOKEN)
        Smooth = 1;
      END_CASE
        CASE(ALBINOS_TOKEN)
        das.albinos = 1;
      END_CASE

        CASE(OFFSET_TOKEN)
        BBoxOffs = Parse_Float();
      END_CASE
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 

        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");

      Tesselate_Object(Object, XAccuracy, YAccuracy, ZAccuracy,
          Smooth, BBoxOffs,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);

      Parse_End();
    }
  void Parser::Parse_Tessel_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      Vector3d Accuracy;
      int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
      TEXTURE *texture = NULL;
      DBL BBoxOffs = 0.0;
      int Smooth = 0;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;

      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      Parse_Begin();

      EXPECT
        CASE(ORIGINAL_TOKEN)
        Object = Parse_Object();
      END_CASE
        CASE(ACCURACY_TOKEN)
        Parse_Vector(Accuracy);
      XAccuracy = (int)(.5+Accuracy[X]);
      YAccuracy = (int)(.5+Accuracy[Y]);
      ZAccuracy = (int)(.5+Accuracy[Z]);
      if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
        Error("accuracy must be greater than 0.\n");
      END_CASE

        CASE(SMOOTH_TOKEN)
        Smooth = 1;
      END_CASE
        CASE(ALBINOS_TOKEN)
        das.albinos = 1;
      END_CASE

        CASE(OFFSET_TOKEN)
        BBoxOffs = Parse_Float();
      END_CASE

        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 
        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT

        if(!Object) Error("Object expected.\n");
      GTesselate_Object(Object, XAccuracy, YAccuracy, ZAccuracy,
          Smooth, BBoxOffs,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);

      Parse_End();
    }
  void Parser::Parse_Cristal_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      Vector3d Accuracy;
      int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
      TEXTURE *texture = NULL;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;

      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      Parse_Begin();

      EXPECT
        CASE(ORIGINAL_TOKEN)
        Object = Parse_Object();
      END_CASE 
        CASE(ACCURACY_TOKEN)
        Parse_Vector(Accuracy);
      XAccuracy = (int)(.5+Accuracy[X]);
      YAccuracy = (int)(.5+Accuracy[Y]);
      ZAccuracy = (int)(.5+Accuracy[Z]);
      if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
        Error("accuracy must be greater than 0.\n");
      END_CASE 
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 
        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");
      Cristallise_Object(Object, XAccuracy, YAccuracy, ZAccuracy,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);

      Parse_End();
    }
  void Parser::Parse_Cubicle_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      TEXTURE *texture = NULL;
      Vector3d Accuracy;
      int XAccuracy = 10, YAccuracy = 10, ZAccuracy = 10;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;

      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      Parse_Begin();

      EXPECT
        CASE(ORIGINAL_TOKEN)
        Object = Parse_Object();
      END_CASE 
        CASE(ACCURACY_TOKEN)
        Parse_Vector(Accuracy);
      XAccuracy = (int)(.5+Accuracy[X]);
      YAccuracy = (int)(.5+Accuracy[Y]);
      ZAccuracy = (int)(.5+Accuracy[Z]);
      if(XAccuracy <= 0 || YAccuracy <= 0 || ZAccuracy <= 0)
        Error("accuracy must be greater than 0.\n");
      END_CASE 
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 
        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");

      Tesselate_Object_Cube(Object, XAccuracy, YAccuracy, ZAccuracy,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);

      Parse_End();
    }
  void Parser::Parse_Screw_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      ScrewCoordInfo info;
      TEXTURE *texture = NULL;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;
      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }


      Parse_Begin();

      info.NPat=NULL;
      info.inverse = 1;
      info.have_min = 0;
      info.have_max = 0;
      info.origin = Vector3d(0,0,0);
      info.angle = Vector3d(0,0,0);
      EXPECT
        CASE (ORIGINAL_TOKEN)
        Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
            }
      END_CASE
        CASE (ORIGIN_TOKEN)
        Parse_Vector(info.origin);
      END_CASE
        CASE(MODULATION_TOKEN)
        Parse_Begin();
      info.NPat=Parse_Texture();
      Parse_End();
      END_CASE 
        CASE (RIGHT_TOKEN)
        info.inverse = -1;
      END_CASE
        CASE (ALBINOS_TOKEN)
        das.albinos = 1;
      END_CASE
        CASE (DIRECTION_TOKEN)
        Parse_Vector(info.angle);
      END_CASE
        CASE (MINIMAL_TOKEN)
        info.min_angle = Parse_Float();
      info.have_min = 1;
      END_CASE
        CASE (MAXIMAL_TOKEN)
        info.max_angle = Parse_Float();
      info.have_max = 1;
      END_CASE
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 
        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");
      if (info.angle.lengthSqr() != 0)
      {
        info.axis = info.angle.normalized();
      }

      Screwlise_Object(Object, &info,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);
      Destroy_Textures(info.NPat);

      Parse_End();
    }
  void Parser::Parse_Roll_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      UNDERCONSTRUCTION das;
      TEXTURE *texture = NULL;
      ScrewCoordInfo info;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;
      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }


      Parse_Begin();

      info.NPat=NULL;
      info.have_min = 0;
      info.have_max = 0;
      info.origin = Vector3d(0,0,0);
      info.angle = Vector3d(0,0,0);
      EXPECT
        CASE (ORIGINAL_TOKEN)
        Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
            }
      END_CASE
        CASE (ORIGIN_TOKEN)
        Parse_Vector(info.origin);
      END_CASE
        CASE(MODULATION_TOKEN)
        Parse_Begin();
      info.NPat=Parse_Texture();
      Parse_End();
      END_CASE 
        CASE (ALBINOS_TOKEN)
        das.albinos = 1;
      END_CASE
        CASE (DIRECTION_TOKEN)
        Parse_Vector(info.angle);
      END_CASE
        CASE (MINIMAL_TOKEN)
        info.min_angle = Parse_Float();
      info.have_min = 1;
      END_CASE
        CASE (MAXIMAL_TOKEN)
        info.max_angle = Parse_Float();
      info.have_max = 1;
      END_CASE
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 
        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");
      if (info.angle.lengthSqr() != 0)
      {
        info.axis = info.angle.normalized();
      }
      Roll_Object(Object, &info,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);

      Parse_End();
    }
  void Parser::Parse_Bend_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      ScrewCoordInfo info;
      TEXTURE *texture = NULL;
      UNDERCONSTRUCTION das;
      Vector3d check;


      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.Vertices = *verts;
      das.Normals = *norms;

      Parse_Begin();
      info.NPat=NULL;
      info.amount = 0;
      info.have_min = 0;
      info.have_max = 0;
      info.origin = Vector3d(0,0,0);
      info.angle = Vector3d(0,1,0);
      info.no_move = Vector3d(1,0,0);
      EXPECT
        CASE (ORIGINAL_TOKEN)
        Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
            }
      END_CASE
        CASE (ORIGIN_TOKEN)
        Parse_Vector(info.origin);
      END_CASE
        CASE(MODULATION_TOKEN)
        Parse_Begin();
      info.NPat=Parse_Texture();
      Parse_End();
      END_CASE 
        CASE (FIXED_TOKEN)
        Parse_Vector(info.no_move);
      END_CASE
        CASE (DIRECTION_TOKEN)
        Parse_Vector(info.angle);
      END_CASE
        CASE (AMOUNT_TOKEN)
        info.amount = Parse_Float();
      END_CASE
        CASE (ALBINOS_TOKEN)
        das.albinos = 1;
      END_CASE
        CASE (MINIMAL_TOKEN)
        info.min_angle = Parse_Float();
      info.have_min = 1;
      END_CASE
        CASE (MAXIMAL_TOKEN)
        info.max_angle = Parse_Float();
      info.have_max = 1;
      END_CASE
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 
        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");
      check = cross(info.no_move,info.angle);
      if (!check.lengthSqr())
      {
        info.amount = 0; /* nothing to do, colinear vector */
      }

      Bend_Object(Object, &info,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *n_norm = das.number_of_normals;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);

      Parse_End();
    }
  void Parser::Parse_Smooth_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      TEXTURE *texture = NULL;
      SmoothInfo info;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;
      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      Parse_Begin();

      info.method=0;
      info.amount=1.0;
      EXPECT
        CASE(ORIGINAL_TOKEN)
        Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
            }
      END_CASE 
        CASE(METHOD_TOKEN)
        info.method=(int)Allow_Float(0.0);
      END_CASE 
        CASE(AMOUNT_TOKEN)
        info.amount=Allow_Float(1.0);
      END_CASE 
        CASE (ALBINOS_TOKEN)
        das.albinos = 1;
      END_CASE
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 
        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");
      Smooth_Object(Object, &info,&das); 

      Apply_Texture(full, &das,*n_tri,texture);

      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);

      Parse_End();
    }

  void Parser::Parse_Select_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      ObjectPtr Object = NULL;
      SelectInfo info;
      TEXTURE *texture = NULL;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;
      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      Parse_Begin();

      info.bound= NULL;
      info.outside = 0;
      info.bin = 0;
      info.bout = 0;
      info.inside = 0;
      EXPECT
        CASE (ORIGINAL_TOKEN)
        Object = Parse_Object();
            {
            Mesh * ThisMeshPtr = dynamic_cast<Mesh*>(Object);
            if (!ThisMeshPtr)
            {
        Error("Mesh expected.\n");
            }
            }
      END_CASE
        CASE (WITH_TOKEN)
        info.bound = Parse_Object();
      END_CASE
        CASE (INNER_TOKEN)
        info.inside = 1;
      END_CASE
        CASE (OUTSIDE_TOKEN)
        info.outside = 1;
      END_CASE
        CASE (INBOUND_TOKEN)
        info.bin = 1;
      END_CASE
        CASE (ALBINOS_TOKEN)
        das.albinos = 1;
      END_CASE
        CASE (OUTBOUND_TOKEN)
        info.bout = 1;
      END_CASE
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 
        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT
        if(!Object) Error("Object expected.\n");
      if(!info.bound) Error("Bounding Object expected.\n");

      Select_Object(Object, &info,&das); 
      Apply_Texture(full, &das,*n_tri,texture);


      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      Destroy_Object(Object);
      Destroy_Object(info.bound);

      Parse_End();
    }


  void Parser::Parse_Load_In_Mesh
    (Mesh* m, MESH_TRIANGLE** trs, TEXTURE *** textu,
     SnglVector3d** verts, SnglVector3d** norms, bool& full,
     int* m_tri, int* m_tex, int* m_vert, int* m_norm, 
     int* n_tri, int* n_tex,int* n_vert, int* n_norm)
    {
      char * filename; 
      GTSInfo info;
      TEXTURE *texture = NULL;
      UNDERCONSTRUCTION das;

      das.tesselationMesh = m;
      das.number_of_triangles = *n_tri;
      das.number_of_vertices = *n_vert;
      das.number_of_normals = *n_norm;
      das.number_of_textures = *n_tex;
      das.Textures = *textu;
      das.max_textures = *m_tex;
      das.max_triangles = *m_tri;
      das.max_vertices = *m_vert;
      das.max_normals = *m_norm;
      das.Triangles = *trs;
      das.Vertices = *verts;
      das.Normals = *norms;
      das.albinos = 0;
      das.Default_Texture = NULL;
      das.Inside_Vect = Vector3d(0, 0, 0);
      if (m->has_inside_vector)
      {
        das.Inside_Vect  =  m->Data->Inside_Vect;
      }

      Parse_Begin();

      info.reverse=false;
      filename = Parse_C_String();
      EXPECT
        CASE(RIGHT_TOKEN)
        info.reverse=true;
      END_CASE 
        CASE(TEXTURE_TOKEN)
        Parse_Begin();
      GET(TEXTURE_ID_TOKEN);
      texture = (TEXTURE *)Token.Data;
      Parse_End();
      END_CASE 
        OTHERWISE
        UNGET
        EXIT
        END_CASE
        END_EXPECT

        Gts_Load_Object(filename, &info,&das); 
      Apply_Texture(full, &das,*n_tri,texture);


      *n_tri = das.number_of_triangles;
      *n_vert = das.number_of_vertices;
      *n_norm = das.number_of_normals;
      *n_tex = das.number_of_textures;
      *m_tex = das.max_textures;
      *textu = das.Textures;
      *m_tri = das.max_triangles;
      *m_vert = das.max_vertices;
      *m_norm = das.max_normals;
      *trs = das.Triangles;
      *verts = das.Vertices;
      *norms = das.Normals;

      POV_FREE(filename);
      Parse_End();
    }
}
