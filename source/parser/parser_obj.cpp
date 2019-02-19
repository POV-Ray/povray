//******************************************************************************
///
/// @file parser/parser_obj.cpp
///
/// This module implements import of Wavefront OBJ files.
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
#include "parser/parser.h"

#if POV_PARSER_EXPERIMENTAL_OBJ_IMPORT

// C++ variants of C standard header files
#include <cctype>

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/material/interior.h"
#include "core/shape/mesh.h"

// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

using namespace pov;

using std::max;
using std::vector;

static const int kMaxObjBufferSize = 1024;

#define PAIR(c1,c2)         ( ((int)(c1)) + (((int)(c2))<<8) )
#define TRIPLET(c1,c2,c3)   ( ((int)(c1)) + (((int)(c2))<<8) + (((int)(c3))<<16) )

struct FaceVertex final
{
    MeshIndex vertexId;
    MeshIndex normalId;
    MeshIndex uvId;
};

struct FaceData final
{
    FaceVertex vertexList[3];
    MeshIndex materialId;
};

struct MaterialData final
{
    std::string mtlName;
    TEXTURE *texture;
};

/// Fills the buffer with the next word from the current line.
/// A trailing null character will be appended.
static bool ReadWord (char *buffer, pov_base::ITextStream *file)
{
    int i;
    int c;
    for (i = 0; i < kMaxObjBufferSize-1; ++i)
    {
        c = file->getchar();
        if ((c == EOF) || isspace (c))
            break;
        buffer[i] = c;
    }
    buffer[i] = '\0';
    while ((c != '\n') && isspace (c))
        c = file->getchar();
    if (c != EOF)
        file->ungetchar (c);
    return (buffer[0] != '\0');
}

static bool HasMoreWords (pov_base::ITextStream *file)
{
    int c = file->getchar();
    file->ungetchar (c);
    return ((c != '\n') && (c != EOF));
}

static void AdvanceLine (pov_base::ITextStream *file)
{
    int c;
    do
        c = file->getchar();
    while ((c != '\n') && (c != EOF));
}

inline static bool ReadFloat (DBL& result, char *buffer, pov_base::ITextStream *file)
{
    if (!ReadWord (buffer, file))
        return false;
    if (sscanf (buffer, POV_DBL_FORMAT_STRING, &result) == 0)
        return false;
    return true;
}

inline static bool ReadFaceVertexField (MeshIndex& result, char **p)
{
    POV_LONG temp = 0;
    do
    {
        if (!isdigit (**p))
            return false;
        temp = (temp * 10) + (**p - '0');
        ++(*p);
    }
    while ((**p != '\0') && (**p != '/'));
    if ((temp < 1) || (temp > std::numeric_limits<MeshIndex>::max()))
        return false;
    result = (MeshIndex)temp;
    return true;
}

inline static bool ReadFaceVertex (FaceVertex& result, char *buffer, pov_base::ITextStream *file)
{
    if (!ReadWord (buffer, file))
        return false;

    char *p = buffer;
    // parse face id
    result.vertexId = 0;
    result.uvId     = 0;
    result.normalId = 0;
    if (!ReadFaceVertexField (result.vertexId, &p))
        return false;
    if (*p == '/')
    {
        ++p;
        if (*p != '/')
        {
            if (!ReadFaceVertexField (result.uvId, &p))
                return false;
        }
        if (*p == '/')
        {
            ++p;
            if (!ReadFaceVertexField (result.normalId, &p))
                return false;
        }
    }
    return true;
}


void Parser::Parse_Obj (Mesh* mesh)
{
    UCS2 *fileName;
    char *s;
    UCS2String ign;
    shared_ptr<IStream> stream;
    pov_base::ITextStream *textStream = nullptr;
    char wordBuffer [kMaxObjBufferSize];
    std::string materialPrefix;
    std::string materialSuffix;

    Vector3d insideVector(0.0);
    Vector3d v3;
    Vector2d v2;
    bool foundZeroNormal = false;
    bool fullyTextured = true;
    bool havePolygonFaces = false;

    FaceData face;
    MaterialData material;
    MATERIAL *povMaterial;

    vector<Vector3d> vertexList;
    vector<Vector3d> normalList;
    vector<Vector2d> uvList;
    vector<FaceData> faceList;
    vector<FaceData> flatFaceList;
    vector<MaterialData> materialList;
    size_t materialId = 0;

    fileName = Parse_String (true);

    EXPECT
        CASE (TEXTURE_LIST_TOKEN)
            Parse_Begin();
            EXPECT
                CASE5 (STRING_LITERAL_TOKEN,CHR_TOKEN,SUBSTR_TOKEN,STR_TOKEN,VSTR_TOKEN)
                CASE5 (CONCAT_TOKEN,STRUPR_TOKEN,STRLWR_TOKEN,DATETIME_TOKEN,STRING_ID_TOKEN)
                    UNGET
                    s = Parse_C_String();
                    material.mtlName = s;
                    POV_FREE (s);

                    EXPECT_ONE
                        CASE (TEXTURE_TOKEN)
                            Parse_Begin ();
                            material.texture = Parse_Texture();
                            Parse_End ();
                        END_CASE
                        CASE (MATERIAL_TOKEN)
                            povMaterial = Create_Material();
                            Parse_Material (povMaterial);
                            material.texture = Copy_Textures (povMaterial->Texture);
                            Destroy_Material (povMaterial);
                        END_CASE
                        OTHERWISE
                            Expectation_Error ("texture or material");
                        END_CASE
                    END_EXPECT

                    Post_Textures(material.texture);
                    materialList.push_back (material);
                END_CASE
                CASE (PREFIX_TOKEN)
                    s = Parse_C_String();
                    materialPrefix = s;
                    POV_FREE (s);
                END_CASE
                CASE (SUFFIX_TOKEN)
                    s = Parse_C_String();
                    materialSuffix = s;
                    POV_FREE (s);
                END_CASE
                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            Parse_End();
        END_CASE

        CASE (INSIDE_VECTOR_TOKEN)
            Parse_Vector (insideVector);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    // open obj file

    stream = Locate_File (fileName, POV_File_Text_OBJ, ign, true);
    if (stream != nullptr)
        textStream = new IBufferedTextStream (fileName, stream.get());
    if (!textStream)
        Error ("Cannot open obj file %s.", UCS2toSysString(fileName).c_str());

    // mark end of buffer with a non-null character to identify situations where a word may not have fit entirely inside the buffer.
    wordBuffer[kMaxObjBufferSize-1] = '*';

    while (!textStream->eof())
    {
        (void)ReadWord (wordBuffer, textStream);
        if (wordBuffer[0] == '\0')
            wordBuffer[1] = '\0';
        if (wordBuffer[1] == '\0')
            wordBuffer[2] = '\0';

        bool unsupportedCmd = false;
        bool skipLine = false;

        switch (TRIPLET (wordBuffer[0], wordBuffer[1], wordBuffer[2]))
        {
            case '\0': // empty line
            case '#': // comment
            case 'g': // group ("g NAME")
            case 'o': // object name ("o NAME")
                skipLine = true;
                break;

            case 'f': // face ("f VERTEXID VERTEXID VERTEXID ...")
                {
                    face.materialId = materialId;
                    if (materialId == 0)
                        fullyTextured = false;
                    int haveVertices = 0;
                    int haveUV = 0;
                    int haveNormal = 0;
                    int vertex = 0;
                    while (HasMoreWords (textStream))
                    {
                        if (!ReadFaceVertex (face.vertexList[vertex], wordBuffer, textStream))
                            Error ("Invalid or unsupported face index data '%s' in obj file %s line %i", wordBuffer, UCS2toSysString(fileName).c_str(), (int)textStream->line());
                        if (face.vertexList[vertex].vertexId > vertexList.size())
                            Error ("Vertex index out of range in obj file %s line %i", UCS2toSysString(fileName).c_str(), (int)textStream->line());
                        if (face.vertexList[vertex].uvId > uvList.size())
                            Error ("UV index out of range in obj file %s line %i", UCS2toSysString(fileName).c_str(), (int)textStream->line());
                        if (face.vertexList[vertex].normalId > normalList.size())
                            Error ("Normal index out of range in obj file %s line %i", UCS2toSysString(fileName).c_str(), (int)textStream->line());
                        ++haveVertices;
                        if (face.vertexList[vertex].uvId > 0)
                            ++haveUV;
                        if (face.vertexList[vertex].normalId > 0)
                            ++haveNormal;

                        if (haveVertices >= 3)
                        {
                            if (haveNormal == haveVertices)
                                faceList.push_back (face);
                            else
                                flatFaceList.push_back (face);
                            face.vertexList[1] = face.vertexList[2];
                        }
                        else
                            ++vertex;
                    }
                    if ((haveVertices > 3) && !havePolygonFaces)
                    {
                        Warning ("Non-triangular faces found in obj file %s. Faces will only import properly if they are convex and planar.", UCS2toSysString(fileName).c_str());
                        havePolygonFaces = true;
                    }
                    if (haveVertices < 3)
                        Error ("Insufficient number of vertices per face in obj file %s line %i", UCS2toSysString(fileName).c_str(), (int)textStream->line());
                    if ((haveUV != 0) && (haveUV != haveVertices))
                        Error ("Inconsistent use of UV indices in obj file %s line %i", UCS2toSysString(fileName).c_str(), (int)textStream->line());
                    if ((haveNormal != 0) && (haveNormal != haveVertices))
                        Error ("Inconsistent use of normal indices in obj file %s line %i", UCS2toSysString(fileName).c_str(), (int)textStream->line());
                    if (haveNormal > 0)
                        faceList.push_back (face);
                    else
                        flatFaceList.push_back (face);
                }
                break;

            case TRIPLET('m','t','l'): // presumably material library ("mtllib FILE FILE ...")
                if (strcmp (wordBuffer, "mtllib") == 0)
                {
                    // TODO
                    unsupportedCmd = true;
                }
                else
                    unsupportedCmd = true;
                break;

            case TRIPLET('u','s','e'): // presumably material selection ("usemtl NAME")
                if (strcmp (wordBuffer, "usemtl") == 0)
                {
                    if (!ReadWord (wordBuffer, textStream))
                        Error ("Invalid material name '%s' in obj file %s line %i", wordBuffer, UCS2toSysString(fileName).c_str(), (int)textStream->line());
                    for (materialId = 0; materialId < materialList.size(); ++materialId)
                    {
                        if (materialList[materialId].mtlName.compare (wordBuffer) == 0)
                            break;
                    }
                    if (materialId == materialList.size())
                    {
                        material.mtlName = wordBuffer;
                        material.texture = nullptr;
                        std::string identifier = materialPrefix + std::string(wordBuffer) + materialSuffix;
                        SYM_ENTRY *symbol = mSymbolStack.Find_Symbol (identifier.c_str());
                        if (symbol == nullptr)
                            Error ("No matching texture for obj file material '%s': Identifier '%s' not found.", wordBuffer, identifier.c_str());
                        else if (symbol->Token_Number == TEXTURE_ID_TOKEN)
                            material.texture = Copy_Textures(reinterpret_cast<TEXTURE *>(symbol->Data));
                        else if (symbol->Token_Number == MATERIAL_ID_TOKEN)
                            material.texture = Copy_Textures(reinterpret_cast<MATERIAL *>(symbol->Data)->Texture);
                        else
                            Error ("No matching texture for obj file material '%s': Identifier '%s' is not a texture or material.", wordBuffer, identifier.c_str());
                        Post_Textures (material.texture);
                        materialList.push_back (material);
                    }
                    materialId ++;
                }
                else
                    unsupportedCmd = true;
                break;

            case 'v': // vertex XYZ coordinates ("v FLOAT FLOAT FLOAT")
                for (int dimension = X; dimension <= Z; ++dimension)
                {
                    if (!ReadFloat (v3[dimension], wordBuffer, textStream))
                        Error ("Invalid coordinate value '%s' in obj file %s line %i", wordBuffer, UCS2toSysString(fileName).c_str(), (int)textStream->line());
                }
                vertexList.push_back (v3);
                break;

            case PAIR('v','n'): // vertex normal vector ("vn FLOAT FLOAT FLOAT")
                for (int dimension = X; dimension <= Z; ++dimension)
                {
                    if (!ReadFloat (v3[dimension], wordBuffer, textStream))
                        Error ("Invalid coordinate value '%s' in obj file %s line %i", wordBuffer, UCS2toSysString(fileName).c_str(), (int)textStream->line());
                }
                normalList.push_back (v3);
                break;

            case PAIR('v','t'): // vertex UV coordinates ("vt FLOAT FLOAT")
                for (int dimension = X; dimension <= Y; ++dimension)
                {
                    if (!ReadFloat (v2[dimension], wordBuffer, textStream))
                        Error ("Invalid coordinate value '%s' in obj file %s line %i", wordBuffer, UCS2toSysString(fileName).c_str(), (int)textStream->line());
                }
                uvList.push_back (v2);
                break;

            default:
                unsupportedCmd = true;
                break;
        }

        if (unsupportedCmd)
        {
            Warning("Unsupported command '%s' skipped in obj file %s line %i.", wordBuffer, UCS2toSysString(fileName).c_str(), (int)textStream->line());
            skipLine = true;
        }

        if (!skipLine && HasMoreWords (textStream))
            PossibleError("Unexpected extra data skipped in obj file %s line %i.", UCS2toSysString(fileName).c_str(), (int)textStream->line());

        // skip remainder of line
        AdvanceLine (textStream);

        if (wordBuffer[kMaxObjBufferSize-1] == '\0')
            PossibleError("Excessively long data in obj file %s line %i.", UCS2toSysString(fileName).c_str(), (int)textStream->line());
    }

    // close obj file
    delete textStream;

    size_t smoothFaces = faceList.size();
    faceList.insert (faceList.end(), flatFaceList.begin(), flatFaceList.end());

    MeshVector *vertexArray = nullptr;
    MeshVector *normalArray = nullptr;
    MeshUVVector *uvArray = nullptr;
    TEXTURE **textureArray = nullptr;
    MESH_TRIANGLE *triangleArray = nullptr;

    if (vertexList.empty())
        Error ("No vertices in obj file.");
    else if (vertexList.size() >= std::numeric_limits<int>::max())
        Error ("Too many UV vectors in obj file.");

    vertexArray = reinterpret_cast<MeshVector *>(POV_MALLOC(vertexList.size()*sizeof(MeshVector), "triangle mesh data"));
    for (size_t i = 0; i < vertexList.size(); ++i)
        vertexArray[i] = MeshVector (vertexList[i]);

    if (!normalList.empty())
    {
        if (normalList.size() >= std::numeric_limits<int>::max())
            Error ("Too many normal vectors in obj file.");

        normalArray = reinterpret_cast<MeshVector *>(POV_MALLOC((normalList.size()+faceList.size())*sizeof(MeshVector), "triangle mesh data"));
        for (size_t i = 0; i < normalList.size(); ++i)
        {
            Vector3d& n = normalList[i];
            if ((fabs(n.x()) < EPSILON) && (fabs(n.x()) < EPSILON) && (fabs(n.z()) < EPSILON))
            {
                n.x() = 1.0;  // make it nonzero
                if (!foundZeroNormal)
                    Warning("Normal vector in mesh2 cannot be zero - changing it to <1,0,0>.");
                foundZeroNormal = true;
            }
            normalArray[i] = MeshVector(n);
        }
    }

    // make sure we at least have one UV coordinate
    if (uvList.empty())
        uvList.push_back (Vector2d(0.0, 0.0));
    else if (uvList.size() >= std::numeric_limits<int>::max())
        Error ("Too many UV vectors in obj file.");

    uvArray = reinterpret_cast<MeshUVVector *>(POV_MALLOC(uvList.size() *sizeof(MeshUVVector), "triangle mesh data"));
    for (size_t i = 0; i < uvList.size(); ++i)
        uvArray[i] = MeshUVVector(uvList[i]);

    if (!materialList.empty())
    {
        if (materialList.size() >= std::numeric_limits<int>::max())
            Error ("Too many materials in obj file.");

        textureArray = reinterpret_cast<TEXTURE **>(POV_MALLOC(materialList.size() *sizeof(TEXTURE*), "triangle mesh data"));
        for (size_t i = 0; i < materialList.size(); ++i)
            textureArray[i] = materialList[i].texture;
    }

    if (faceList.empty())
        Error ("No faces in obj file.");

    triangleArray = reinterpret_cast<MESH_TRIANGLE *>(POV_MALLOC(faceList.size()*sizeof(MESH_TRIANGLE), "triangle mesh data"));
    for (size_t i = 0, j = normalList.size(); i < faceList.size(); ++i, ++j)
    {
        const FaceData& objTriangle = faceList[i];
        MESH_TRIANGLE& triangle = triangleArray[i];
        mesh->Init_Mesh_Triangle (&triangle);
        triangle.P1 = objTriangle.vertexList[0].vertexId - 1;
        triangle.P2 = objTriangle.vertexList[1].vertexId - 1;
        triangle.P3 = objTriangle.vertexList[2].vertexId - 1;
        triangle.Texture  = objTriangle.materialId - 1;
        triangle.UV1 = max(1, objTriangle.vertexList[0].uvId) - 1;
        triangle.UV2 = max(1, objTriangle.vertexList[1].uvId) - 1;
        triangle.UV3 = max(1, objTriangle.vertexList[2].uvId) - 1;
        triangle.Smooth = (i < smoothFaces);
        const Vector3d& P1 = vertexList[triangle.P1];
        const Vector3d& P2 = vertexList[triangle.P2];
        const Vector3d& P3 = vertexList[triangle.P3];
        Vector3d N;
        if (triangle.Smooth)
        {
            triangle.N1 = objTriangle.vertexList[0].normalId - 1;
            triangle.N2 = objTriangle.vertexList[1].normalId - 1;
            triangle.N3 = objTriangle.vertexList[2].normalId - 1;
            Vector3d& N1 = normalList[triangle.N1];
            Vector3d& N2 = normalList[triangle.N2];
            Vector3d& N3 = normalList[triangle.N3];

            // check for equal normals
            Vector3d D1 = N1 - N2;
            Vector3d D2 = N1 - N3;
            double l1 = D1.lengthSqr();
            double l2 = D2.lengthSqr();
            triangle.Smooth = ((fabs(l1) > EPSILON) || (fabs(l2) > EPSILON));
        }
        mesh->Compute_Mesh_Triangle (&triangle, triangle.Smooth, P1, P2, P3, N);
        triangle.Normal_Ind = j;
        normalArray[j] = MeshVector(N);
    }

    if (fullyTextured)
        mesh->Type |= TEXTURED_OBJECT;

    mesh->Data = reinterpret_cast<MESH_DATA *>(POV_MALLOC(sizeof(MESH_DATA), "triangle mesh data"));
    mesh->Data->References = 1;
    mesh->Data->Tree = nullptr;

    mesh->has_inside_vector = insideVector.IsNearNull (EPSILON);
    if (mesh->has_inside_vector)
    {
        mesh->Data->Inside_Vect = insideVector.normalized();
        mesh->Type &= ~PATCH_OBJECT;
    }
    else
    {
        mesh->Type |= PATCH_OBJECT;
    }

    mesh->Data->Normals   = normalArray;
    mesh->Data->Triangles = triangleArray;
    mesh->Data->Vertices  = vertexArray;
    mesh->Data->UVCoords  = uvArray;
    mesh->Textures        = textureArray;

    /* copy number of for normals, textures, triangles and vertices. */
    mesh->Data->Number_Of_Normals   = normalList.size() + faceList.size();
    mesh->Data->Number_Of_Triangles = faceList.size();
    mesh->Data->Number_Of_Vertices  = vertexList.size();
    mesh->Data->Number_Of_UVCoords  = uvList.size();
    mesh->Number_Of_Textures        = materialList.size();

    if (!materialList.empty())
        Set_Flag(mesh, MULTITEXTURE_FLAG);
}

}
// end of namespace pov_parser

#endif // POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
