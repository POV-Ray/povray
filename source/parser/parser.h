//******************************************************************************
///
/// @file parser/parser.h
///
/// Declarations related to the parser.
///
/// This header file is included by virtually all parser C++ files in POV-Ray.
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

#ifndef POVRAY_PARSER_PARSE_H
#define POVRAY_PARSER_PARSE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "parser/configparser.h"

#include <string>

#include "base/image/image.h"
#include "base/messenger.h"
#include "base/stringutilities.h"
#include "base/textstream.h"
#include "base/textstreambuffer.h"

#include "core/material/blendmap.h"
#include "core/material/pigment.h"
#include "core/material/warp.h"
#include "core/shape/mesh.h"
#include "core/scene/camera.h"

#include "parser/fncode.h"
#include "parser/reservedwords.h"

#include "backend/support/task.h"

namespace pov
{

class Blob_Element;
struct ContainedByShape;
struct Fog_Struct;
struct GenericSpline;
class ImageData;
class Mesh;
struct PavementPattern;
struct Rainbow_Struct;
class SceneData;
struct Skysphere_Struct;
struct TilingPattern;
struct TrueTypeFont;

}

namespace pov_parser
{

using namespace pov_base;
using namespace pov;

const int MAX_BRACES = 200;

const int MAX_NUMBER_OF_TABLES = 100;
const int TOKEN_OVERFLOW_RESET_COUNT = 2500;
const int MAX_PARAMETER_LIST = 56;

const int MAX_INCLUDE_FILES = 32;

const int COND_STACK_SIZE = 200;

// needs to be a power of two
const int MAX_STRING_LEN_FAST = 256;
// this needs to be usable as bit mask, so keep it MAX_STRING_LEN_FAST - 1
const int MAX_STRING_LEN_MASK = (MAX_STRING_LEN_FAST - 1);

const int SYM_TABLE_SIZE = 257;

typedef struct Sym_Table_Entry SYM_ENTRY;
typedef unsigned short SymTableEntryRefCount;

// Special symbol tables
enum {
    SYM_TABLE_RESERVED = 0,        // reserved words
    SYM_TABLE_GLOBAL,              // identifiers declared using #declare (or #local in top-level file), #function, #macro, etc.
};

/// Structure holding information about a symbol
struct Sym_Table_Entry
{
    SYM_ENTRY *next;            ///< Reference to next symbol with same hash
    char *Token_Name;           ///< Symbol name
    char *Deprecation_Message;  ///< Warning to print if the symbol is deprecated
    void *Data;                 ///< Reference to the symbol value
    TOKEN Token_Number;         ///< Unique ID of this symbol
    bool deprecated      : 1;
    bool deprecatedOnce  : 1;
    bool deprecatedShown : 1;
    SymTableEntryRefCount ref_count; ///< normally 1, but may be greater when passing symbols out of macros
};

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Here we create our own little language for doing the parsing.  It
  makes the code easier to read. */

#define EXPECT { int Exit_Flag; Exit_Flag = false; \
    while (!Exit_Flag) {Get_Token();  switch (Token.Token_Id) {
#define EXPECT_ONE { int Exit_Flag; Exit_Flag = false; \
    {Get_Token();  switch (Token.Token_Id) {
#define CASE(x) case x:
#define CASE2(x, y) case x: case y:
#define CASE3(x, y, z) case x: case y: case z:
#define CASE4(w, x, y, z) case w: case x: case y: case z:
#define CASE5(v, w, x, y, z) case v: case w: case x: case y: case z:
#define CASE6(u, v, w, x, y, z) case u: case v: case w: case x: case y: case z:
#define END_CASE break;
#define EXIT Exit_Flag = true;
#define OTHERWISE default:
#define END_EXPECT } } }
#define GET(x) Get_Token(); if (Token.Token_Id != x) Parse_Error (x);
#define ALLOW(x) Get_Token(); if (Token.Token_Id != x) Unget_Token();
#define UNGET Unget_Token();

#define CASE_FLOAT CASE2 (LEFT_PAREN_TOKEN,FLOAT_FUNCT_TOKEN)\
    CASE4 (PLUS_TOKEN,DASH_TOKEN,FUNCT_ID_TOKEN,EXCLAMATION_TOKEN) \
    UNGET

#define CASE_VECTOR CASE3 (VECTFUNCT_ID_TOKEN,VECTOR_FUNCT_TOKEN,LEFT_ANGLE_TOKEN) \
    CASE5 (U_TOKEN,V_TOKEN,UV_ID_TOKEN,VECTOR_4D_ID_TOKEN,SPLINE_ID_TOKEN) \
    CASE_FLOAT /* NOTE this has an UNGET in it! */

#define CASE_COLOUR CASE3 (COLOUR_TOKEN,COLOUR_KEY_TOKEN,COLOUR_ID_TOKEN) \
    UNGET

/*
CASE_EXPRESS is a CASE_COLOUR w/o an UNGET and a CASE_VECTOR (w/unget)

   NOTE: you cannot use CASE_VECTOR followed by CASE_COLOUR or vice-versa, because
         they both contain an UNGET which will cause problems!
*/
#define CASE_EXPRESS CASE3 (COLOUR_TOKEN,COLOUR_KEY_TOKEN,COLOUR_ID_TOKEN) \
    CASE_VECTOR /* NOTE this has an UNGET in it! */


struct ExperimentalFlags
{
    bool    backsideIllumination    : 1;
    bool    functionHf              : 1;
    bool    meshCamera              : 1;
    bool    objImport               : 1;
    bool    slopeAltitude           : 1;
    bool    spline                  : 1;
    bool    subsurface              : 1;
    bool    tiff                    : 1;
    bool    userDefinedCamera       : 1;

    ExperimentalFlags() :
        backsideIllumination(false),
        functionHf(false),
        meshCamera(false),
        objImport(false),
        slopeAltitude(false),
        spline(false),
        subsurface(false),
        tiff(false),
        userDefinedCamera(false)
    {}
};

struct BetaFlags
{
    bool    realTimeRaytracing  : 1;
    bool    videoCapture        : 1;

    BetaFlags() :
        realTimeRaytracing(false),
        videoCapture(false)
    {}
};

/*****************************************************************************
* Global typedefs
******************************************************************************/

class Parser : public SceneTask
{
    private:

        struct SYM_TABLE;

    public:

        class DebugTextStreamBuffer : public TextStreamBuffer
        {
            public:
                DebugTextStreamBuffer(GenericMessenger& m);
                ~DebugTextStreamBuffer();
            protected:
                virtual void lineoutput(const char *str, unsigned int chars);
                virtual void directoutput(const char *str, unsigned int chars);

                GenericMessenger& mMessenger;
        };

        DebugTextStreamBuffer Debug_Message_Buffer;

        // tokenize.h/tokenize.cpp

        /// Structure holding information about the current token
        struct Token_Struct
        {
            TOKEN Token_Id;                                 ///< reserved token (or token group) ID, or unique identifier ID
            TOKEN Function_Id;                              ///< token type ID, in case Token_Id is an identifier ID
            pov_base::ITextStream::FilePos Token_File_Pos;  ///< location of this token in the scene or include file (line number & file position)
            int Token_Col_No;                               ///< location of this token in the scene or include file (column)
            int context;                                    ///< context the token is local to (i.e., table index)
            char *Token_String;                             ///< reference to token value (if it is a string literal) or character sequence comprising the token
            DBL Token_Float;                                ///< token value (if it is a float literal)
            int Unget_Token, End_Of_File;
            pov_base::ITextStream *FileHandle;              ///< location of this token in the scene or include file (file)
            void *Data;                                     ///< reference to token value (if it is a non-float identifier)
            int *NumberPtr;
            void **DataPtr;
            SYM_TABLE *table;                               ///< table or dictionary the token references an element of
            bool is_array_elem          : 1;                ///< true if token is actually an array element reference
            bool is_mixed_array_elem    : 1;                ///< true if token is actually a mixed-type array element reference
            bool is_dictionary_elem     : 1;                ///< true if token is actually a dictionary element reference
            bool freeString             : 1;                ///< true if Token_String must be freed before being assigned a new value
        };

        struct LValue
        {
            int*         numberPtr;
            void**       dataPtr;
            int          previous;
            SYM_ENTRY*   symEntry;
            bool         allowRedefine : 1;
            bool         optional      : 1;
        };

        Token_Struct Token;

        struct MacroParameter
        {
            char*   name;
            bool    optional;
        };

        struct Macro
        {
            Macro(const char *);
            ~Macro();
            char *Macro_Name;
            UCS2 *Macro_Filename;
            pov_base::ITextStream::FilePos Macro_File_Pos;
            int Macro_File_Col;
            POV_OFF_T Macro_End; ///< The position _after_ the `#` in the terminating `#end` directive.
            vector<MacroParameter> parameters;
            unsigned char *Cache;
            size_t CacheSize;
        };

        struct POV_ARRAY
        {
            int Dims, Type;
            int Sizes[5];
            int Mags[5];
            vector<void*> DataPtrs;
            vector<int> Types;
            bool resizable;
        };

        struct POV_PARAM
        {
            int *NumberPtr;
            void **DataPtr;
        };

        struct DATA_FILE
        {
            pov_base::ITextStream *In_File;
            pov_base::OTextStream *Out_File;
            bool busyParsing    : 1; ///< `true` if parsing a statement related to the file, `false` otherwise.
            bool R_Flag         : 1;
        };

        // constructor
        Parser(shared_ptr<BackendSceneData> sd, bool useclock, DBL clock);

        ~Parser();

        void Run();
        void Stopped();
        void Finish();
        void Cleanup(void);

        inline TraceThreadData *GetParserDataPtr() { return reinterpret_cast<TraceThreadData *>(GetDataPtr()); }

        // parse.h/parse.cpp
        void Parse_Error (TOKEN Token_Id);
        void Found_Instead_Error (const char *exstr, const char *extokstr);
        bool Parse_Begin (TOKEN tokenId, bool mandatory);
        void Parse_End (TOKEN tokenId);
        inline bool Parse_Begin (bool mandatory = true) { return Parse_Begin(LEFT_CURLY_TOKEN, mandatory); }
        inline void Parse_End (void) { Parse_End(RIGHT_CURLY_TOKEN); }
        inline bool Parse_Paren_Begin (bool mandatory = true) { return Parse_Begin(LEFT_PAREN_TOKEN, mandatory); }
        inline void Parse_Paren_End (void) { Parse_End(RIGHT_PAREN_TOKEN); }
        inline bool Parse_Angle_Begin (bool mandatory = true) { return Parse_Begin(LEFT_ANGLE_TOKEN, mandatory); }
        inline void Parse_Angle_End (void) { Parse_End(RIGHT_ANGLE_TOKEN); }
        inline bool Parse_Square_Begin (bool mandatory = true) { return Parse_Begin(LEFT_SQUARE_TOKEN, mandatory); }
        inline void Parse_Square_End (void) { Parse_End(RIGHT_SQUARE_TOKEN); }
        bool Parse_Comma (void);
        bool Peek_Token (TOKEN tokenId);
        void Parse_Semi_Colon (bool force_semicolon);
        void Destroy_Frame (void);
        void MAError (const char *str, long size);
        void Warn_State (TOKEN Token_Id, TOKEN Type);
        void Only_In (const char *s1,const char *s2);
        void Not_With (const char *s1,const char *s2);
        void Warn_Compat (bool definite, const char *sym);
        void Link_Textures (TEXTURE **Old_Texture, TEXTURE *New_Texture);

        /// @note This method includes an implied `Parse_End()`.
        ObjectPtr Parse_Object_Mods (ObjectPtr Object);

        ObjectPtr Parse_Object (void);
        void Parse_Bound_Clip (vector<ObjectPtr>& objects, bool notexture = true);
        void Parse_Default (void);
        void Parse_Declare (bool is_local, bool after_hash);
        void Parse_Matrix (MATRIX Matrix);
        void Destroy_Ident_Data (void *Data, int Type);
        bool PassParameterByReference (int oldTableIndex);
        bool Parse_RValue (int Previous, int *NumberPtr, void **DataPtr, SYM_ENTRY *sym, bool ParFlag, bool SemiFlag, bool is_local, bool allow_redefine, bool allowUndefined, int old_table_index);
        const char *Get_Token_String (TOKEN Token_Id);
        void Test_Redefine(TOKEN Previous, TOKEN *NumberPtr, void *Data, bool allow_redefine = true);
        void Expectation_Error(const char *);
        void *Copy_Identifier(void *Data, int Type);
        TRANSFORM *Parse_Transform(TRANSFORM *Trans = NULL);
        TRANSFORM *Parse_Transform_Block(TRANSFORM *New = NULL);
        char *Get_Reserved_Words (const char *additional_words);

        void SendFatalError(Exception& e);

        void Warning(const char *format,...);
        void Warning(WarningLevel level, const char *format,...);
        void VersionWarning(unsigned int sinceVersion, const char *format,...);
        void PossibleError(const char *format,...);
        void Error(const char *format,...);
        void ErrorInfo(const SourceInfo& loc, const char *format,...);

        int Debug_Info(const char *format,...);
        void FlushDebugMessageBuffer();

        // NK layers - 1999 July 10 - for backwards compatibility with layered textures
        static void Convert_Filter_To_Transmit(PIGMENT *Pigment);
        static void Convert_Filter_To_Transmit(GenericPigmentBlendMap *pBlendMap);

        /// @param[in]  formalFileName  Name by which the file is known to the user.
        /// @param[out] actualFileName  Name by which the file is known to the parsing computer.
        IStream *Locate_File(const UCS2String& formalFileName, unsigned int stype, UCS2String& actualFileName, bool err_flag = false);

        OStream *CreateFile(const UCS2String& filename, unsigned int stype, bool append);
        Image *Read_Image(int filetype, const UCS2 *filename, const Image::ReadOptions& options);

        // tokenize.h/tokenize.cpp
        void Get_Token (void);
        void Unget_Token (void);
        void Read_String_Literal(void);
        void Where_Error (POVMSObjectPtr msg);
        void Where_Warning (POVMSObjectPtr msg);
        void Parse_Directive (int After_Hash);
        void Open_Include (void);
        void IncludeHeader(const UCS2String& temp);
        void pre_init_tokenizer (void);
        void Initialize_Tokenizer (void);
        void Terminate_Tokenizer (void);
        SYM_ENTRY *Add_Symbol (SYM_TABLE *table, const char *Name,TOKEN Number);
        SYM_ENTRY *Add_Symbol (int Index,const char *Name,TOKEN Number);
        POV_ARRAY *Parse_Array_Declare (void);
        SYM_TABLE *Parse_Dictionary_Declare();
        SYM_ENTRY *Create_Entry (const char *Name, TOKEN Number, bool copyName);
        SYM_ENTRY *Copy_Entry (const SYM_ENTRY *);
        void Acquire_Entry_Reference (SYM_ENTRY *Entry);
        void Release_Entry_Reference (SYM_TABLE *table, SYM_ENTRY *Entry);
        SYM_ENTRY *Destroy_Entry (SYM_ENTRY *Entry, bool destroyName);
        bool Parse_Ifdef_Param ();
        int Parse_For_Param (char**, DBL*, DBL*);

        // parstxtr.h/parstxtr.cpp
        TEXTURE *Parse_Texture (void);
        void Parse_Pigment (PIGMENT **);
        void Parse_Tnormal (TNORMAL **);
        void Parse_Finish (FINISH **);
        void Parse_Media (vector<Media>&);
        void Parse_Interior (InteriorPtr&);
        void Parse_Media_Density_Pattern (PIGMENT **);
        void Parse_Media_Density_Pattern (vector<PIGMENT*>&);
        Fog_Struct *Parse_Fog (void);
        Rainbow_Struct *Parse_Rainbow (void);
        Skysphere_Struct *Parse_Skysphere(void);
        ImageData *Parse_Image (int LegalTypes, bool GammaCorrect = false);
        SimpleGammaCurvePtr Parse_Gamma (void);
        void Parse_Material(MATERIAL *);
        void Parse_PatternFunction(TPATTERN *);

        // express.h/express.cpp
        void Parse_Colour (RGBFTColour& colour, bool expectFT = true);
        void Parse_Colour (TransColour& colour, bool expectFT = true);
        void Parse_Colour (RGBColour& colour);
        void Parse_Colour (MathColour& colour);
        void Parse_Wavelengths (MathColour& colour);
        template<typename DATA_T> void Parse_BlendMapData (BlendMapTypeId Blend_Type, DATA_T& rData);
        template<typename MAP_T> shared_ptr<MAP_T> Parse_Blend_Map (BlendMapTypeId Blend_Type, int Pat_Type);
        template<typename MAP_T> shared_ptr<MAP_T> Parse_Colour_Map (void);
        template<typename DATA_T> void Parse_BlendListData (BlendMapTypeId Blend_Type, DATA_T& rData);
        template<typename DATA_T> void Parse_BlendListData_Default (const ColourBlendMapData& Def_Entry, BlendMapTypeId Blend_Type, DATA_T& rData);
        template<typename MAP_T> shared_ptr<MAP_T> Parse_Blend_List (int Count, ColourBlendMapConstPtr Def_Map, BlendMapTypeId Blend_Type);
        template<typename MAP_T> shared_ptr<MAP_T> Parse_Item_Into_Blend_List (BlendMapTypeId Blend_Type);
        GenericSpline *Parse_Spline (void);

        /// Parses a FLOAT.
        DBL Parse_Float (void);

        /// Parses an optional FLOAT.
        DBL Allow_Float (DBL defval);

        /// Parses a FLOAT as an integer value.
        int Parse_Int(const char* parameterName = NULL);

        /// Parses a FLOAT as an integer value with a given minimum.
        int Parse_Int_With_Minimum(int minValue, const char* parameterName = NULL);

        /// Parses a FLOAT as an integer value with a given range.
        int Parse_Int_With_Range(int minValue, int maxValue, const char* parameterName = NULL);

        /// Parses a FLOAT as a boolean value.
        bool Parse_Bool(const char* parameterName = NULL);

        int Allow_Vector (Vector3d& Vect);
        int Allow_Vector4D (VECTOR_4D Vector);
        void Parse_UV_Vect (Vector2d& UV_Vect);
        void Parse_Vector (Vector3d& Vector);
        void Parse_Vector4D (VECTOR_4D Vector);
        int Parse_Unknown_Vector (EXPRESS& Express, bool allow_identifier = false, bool *had_identifier = NULL);
        void Parse_Scale_Vector (Vector3d& Vector);
        DBL Parse_Float_Param (void);
        void Parse_Float_Param2 (DBL *Val1, DBL *Val2);
        void Init_Random_Generators (void);
        void Destroy_Random_Generators (void);
        DBL Parse_Signed_Float(void);

        // function.h/function.cpp
        FUNCTION_PTR Parse_Function(void);
        FUNCTION_PTR Parse_FunctionContent(void);
        FUNCTION_PTR Parse_FunctionOrContent(void);
        void Parse_FunctionOrContentList(GenericScalarFunctionPtr* apFn, unsigned int count, bool mandatory = true);
        FUNCTION_PTR Parse_DeclareFunction(int *token_id, const char *fn_name, bool is_local);
        intrusive_ptr<FunctionVM> GetFunctionVM() const;

        // parsestr.h/parsestr.cpp
        char *Parse_C_String(bool pathname = false);
        UCS2 *Parse_String(bool pathname = false, bool require = true);
        std::string Parse_ASCIIString(bool pathname = false, bool require = true);
        UCS2String Parse_UCS2String(bool pathname = false, bool require = true);

        UCS2 *String_Literal_To_UCS2(const char *str, bool pathname = false);
        UCS2 *String_To_UCS2(const char *str);
        char *UCS2_To_String(const UCS2 *str);

        static UCS2 *UCS2_strcat(UCS2 *s1, const UCS2 *s2);
        static int UCS2_strlen(const UCS2 *str);
        static int UCS2_strcmp(const UCS2 *s1, const UCS2 *s2);
        static void UCS2_strcpy(UCS2 *s1, const UCS2 *s2);
        static void UCS2_strncpy(UCS2 *s1, const UCS2 *s2, int n);
        void UCS2_strupr(UCS2 *str); // not static because it may issue a parse warning
        void UCS2_strlwr(UCS2 *str); // not static because it may issue a parse warning
        static UCS2 *UCS2_strdup(const UCS2 *s);

        // fnsyntax.h/fnsyntax.cpp
        bool expr_noop(ExprNode *&current, int stage, int op);
        bool expr_grow(ExprNode *&current, int stage, int op);
        bool expr_call(ExprNode *&current, int stage, int op);
        bool expr_put(ExprNode *&current, int stage, int op);
        bool expr_new(ExprNode *&current, int stage, int op);
        bool expr_ret(ExprNode *&current, int stage, int op);
        bool expr_err(ExprNode *&current, int stage, int op);

        shared_ptr<BackendSceneData> backendSceneData; // TODO FIXME HACK - make private again once Locate_Filename is fixed [trf]
        shared_ptr<SceneData> sceneData;

        // tesselation.cpp
        ObjectPtr Parse_Tesselation(void);
        ObjectPtr Parse_Tessel(void);

        void Parse_Tesselation_In_Mesh(Mesh* m, MESH_TRIANGLE** trs,TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int *m_tex,int* m_vert,int* m_norm,
            int* n_tri,int *n_tex,int* n_vert,int* n_norm);
        void Parse_Tessel_In_Mesh(Mesh* m, MESH_TRIANGLE** trs,TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int *m_tex,int* m_vert,int* m_norm,
            int* n_tri,int *n_tex,int* n_vert,int* n_norm);

        ObjectPtr Parse_Bourke(void);
        ObjectPtr Parse_Heller(void);

        void Parse_Bourke_In_Mesh(Mesh* m, MESH_TRIANGLE** trs,TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int *m_tex,int* m_vert,int* m_norm,
            int* n_tri,int *n_tex,int* n_vert,int* n_norm);
        void Parse_Heller_In_Mesh(Mesh* m, MESH_TRIANGLE** trs,TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int *m_tex,int* m_vert,int* m_norm,
            int* n_tri,int *n_tex,int* n_vert,int* n_norm);

        void Parse_Get_Vertex(Vector3d& Vect);
        void Parse_Get_Normal(Vector3d& Vect);
        void Parse_Get_Vertex_Indices(Vector3d& Vect);
        void Parse_Get_Normal_Indices(Vector3d& Vect);

        DBL Parse_Get_Triangles_Amount(void);
        DBL Parse_Get_Vertices_Amount(void);
        DBL Parse_Get_Normals_Amount(void);
        DBL Parse_Is_Smooth_Triangle(void);

        ObjectPtr Parse_Cubicle(void);
        ObjectPtr Parse_Select(void);

        void Parse_Cubicle_In_Mesh(Mesh* m, MESH_TRIANGLE** trs,TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int *m_tex,int* m_vert,int* m_norm,
            int* n_tri,int *n_tex,int* n_vert,int* n_norm);

        void Parse_Select_In_Mesh(Mesh* m, MESH_TRIANGLE** trs,TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int *m_tex,int* m_vert,int* m_norm,
            int* n_tri,int *n_tex,int* n_vert,int* n_norm);

        ObjectPtr Parse_Grid(void);

        ObjectPtr Parse_Cristal(void);

        void Parse_Cristal_In_Mesh(Mesh* m, MESH_TRIANGLE** trs, TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int *m_tex,int* m_vert,int* m_norm,
            int* n_tri,int* n_tex,int* n_vert,int* n_norm);


        ObjectPtr Parse_Screw(void);

        void Parse_Screw_In_Mesh(Mesh* m, MESH_TRIANGLE** trs, TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int* m_tex,int* m_vert,int* m_norm,
            int* n_tri,int* n_tex,int* n_vert,int* n_norm);

        ObjectPtr Parse_Roll(void);

        void Parse_Roll_In_Mesh(Mesh* m, MESH_TRIANGLE** trs, TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int* m_tex,int* m_vert,int* m_norm,
            int* n_tri,int* n_tex,int* n_vert,int* n_norm);

        ObjectPtr Parse_Bend(void);
        ObjectPtr Parse_Displace(void);
        ObjectPtr Parse_Move_Object(void);
        ObjectPtr Parse_Warp_Object(void);
        ObjectPtr Parse_Planet(void);

        void Parse_Warp_In_Mesh(Mesh* m, MESH_TRIANGLE** trs, TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int* m_tex,int* m_vert,int* m_norm,
            int* n_tri,int* n_tex,int* n_vert,int* n_norm);

        void Parse_Bend_In_Mesh(Mesh* m, MESH_TRIANGLE** trs, TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int* m_tex,int* m_vert,int* m_norm,
            int* n_tri,int* n_tex,int* n_vert,int* n_norm);

        void Parse_Displace_In_Mesh(Mesh* m, MESH_TRIANGLE** trs, TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int* m_tex,int* m_vert,int* m_norm,
            int* n_tri,int* n_tex,int* n_vert,int* n_norm);

        void Parse_Move_In_Mesh(Mesh* m, MESH_TRIANGLE** trs, TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms,bool& full,
            int* m_tri,int* m_tex,int* m_vert,int* m_norm,
            int* n_tri,int* n_tex,int* n_vert,int* n_norm);

        ObjectPtr Parse_Smooth(void);

        void Parse_Smooth_In_Mesh(Mesh* m, MESH_TRIANGLE** trs, TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms, bool& full,
            int* m_tri,int* m_tex,int* m_vert,int* m_norm,
            int* n_tri,int* n_tex,int* n_vert,int* n_norm);

        void Parse_Gts_Save(void);
        void Parse_Stl_Save(void);
        ObjectPtr Parse_Gts_Load(void);
        ObjectPtr Parse_Stl_Load(void);

        void Parse_Load_In_Mesh(Mesh* m, MESH_TRIANGLE** trs, TEXTURE*** textu,
            SnglVector3d** verts, SnglVector3d** norms, bool& full,
            int* m_tri,int* m_tex,int* m_vert,int* m_norm,
            int* n_tri,int* n_tex,int* n_vert,int* n_norm);

    private:

        struct BraceStackEntry
        {
            TOKEN       openToken;
            SourceInfo  sourceInfo;
        };

        intrusive_ptr<FunctionVM> mpFunctionVM;
        FPUContext *fnVMContext;

        bool Had_Max_Trace_Level;
        int Max_Trace_Level;

        ExperimentalFlags mExperimentalFlags;
        BetaFlags mBetaFeatureFlags;

        DBL clockValue;
        bool useClock;

        // parse.h/parse.cpp
        bool Not_In_Default;
        bool Ok_To_Declare;
        bool LValue_Ok;

        /// true if a #version statement is being parsed
        bool parsingVersionDirective;

        vector<BraceStackEntry> maBraceStack;
        bool Destroying_Frame;

        Camera Default_Camera;

        // tokenize.h/tokenize.cpp
        typedef enum cond_type
        {
            ROOT_COND=0,            ///< no conditionals, loops or macros pending
            BUSY_COND,              ///< busy parsing the actual conditional or loop statement
            WHILE_COND,             ///< executing a `#while` loop body
            FOR_COND,               ///< executing a `#for` loop body
            IF_TRUE_COND,           ///< executing an `#if` body
            IF_FALSE_COND,          ///< skipping an `#if` body to execute any `#else` or `#elseif` body
            ELSE_COND,              ///< executing an '#else` body
            SWITCH_COND,            ///< executing a `#switch` body
            CASE_TRUE_COND,         ///< executing a `#case` or `#range` body
            CASE_FALSE_COND,        ///< skipping a `#case` or `#range` body to execute a later one
            SKIP_TIL_END_COND,      ///< skipping a conditional or loop body until a matching `#end`
            INVOKING_MACRO_COND,    ///< executing a `#macro` body
            DECLARING_MACRO_COND    ///< skipping a `#macro` body during its declaration
        } COND_TYPE;

        struct SYM_TABLE
        {
            SYM_ENTRY *Table[SYM_TABLE_SIZE];
            bool namesAreCopies;
        };

        SYM_TABLE *Tables[MAX_NUMBER_OF_TABLES];

        int Table_Index;

        char String_Fast_Buffer[MAX_STRING_LEN_FAST];

        int String_Index;
        int String_Buffer_Free;

        char *String;
        char *String2;

        POV_LONG last_progress;

        POV_LONG Current_Token_Count; // This variable really counts tokens! [trf]

        int token_count; // WARNING: This variable has very little to do with counting tokens! [trf]

        int line_count;

        struct InputFileData
        {
            pov_base::ITextStream *In_File;
            bool R_Flag;
        };

        int Include_File_Index;
        InputFileData *Input_File;
        InputFileData Include_Files[MAX_INCLUDE_FILES];

        int Echo_Indx;

        struct CS_ENTRY
        {
            COND_TYPE Cond_Type;
            DBL Switch_Value;
            pov_base::ITextStream *Loop_File;
            pov_base::ITextStream *Macro_File;
            const UCS2 *Macro_Return_Name;
            int Macro_Return_Col;
            bool Macro_Same_Flag;
            bool Switch_Case_Ok_Flag;
            Macro *PMac;
            pov_base::ITextStream::FilePos File_Pos;
            char* Loop_Identifier;
            DBL For_Loop_End;
            DBL For_Loop_Step;
        };

        CS_ENTRY *Cond_Stack;
        int CS_Index;
        bool Skipping, Inside_Ifdef, Inside_MacroDef, Parsing_Directive, parseRawIdentifiers, parseOptionalRValue;

        bool Got_EOF; // WARNING: Changes to the use of this variable are very dangerous as it is used in many places assuming certain non-obvious side effects! [trf]

        TOKEN Conversion_Util_Table[TOKEN_COUNT];

        // parstxtr.h/parstxtr.cpp
        TEXTURE *Default_Texture;

        // express.h/express.cpp
        short Have_Vector;
        unsigned int Number_Of_Random_Generators;
        unsigned int *next_rand;

        bool Allow_Identifier_In_Call, Identifier_In_Call;

        size_t MaxCachedMacroSize;

        // parse.h/parse.cpp
        void Frame_Init(void);
        void InitDefaults(int version);
        void Parse_Coeffs(int order, DBL *Coeffs);

        ObjectPtr Parse_Bicubic_Patch(void);
        ObjectPtr Parse_Blob(void);
        ObjectPtr Parse_Box(void);
        ObjectPtr Parse_Cone(void);
        ObjectPtr Parse_CSG(int CSG_Type);
        ObjectPtr Parse_Light_Group(void);
        ObjectPtr Parse_Cylinder(void);
        ObjectPtr Parse_Disc(void);
        ObjectPtr Parse_Julia_Fractal(void);
        ObjectPtr Parse_HField(void);
        ObjectPtr Parse_Lathe(void);
        ObjectPtr Parse_Lemon();
        ObjectPtr Parse_Light_Source();

        /// @note This method includes an implied `Parse_End()`.
        ObjectPtr Parse_Object_Id();

        ObjectPtr Parse_Ovus();
        ObjectPtr Parse_Nurbs();
        ObjectPtr Parse_Plane();
        ObjectPtr Parse_Poly(int order);
        ObjectPtr Parse_Polynom();
        ObjectPtr Parse_Polygon();
        ObjectPtr Parse_Polyline();
        ObjectPtr Parse_Prism();
        ObjectPtr Parse_Quadric();
        ObjectPtr Parse_Rational_Bezier_Patch();
        ObjectPtr Parse_Smooth_Triangle();
        ObjectPtr Parse_Sor();
        ObjectPtr Parse_Sphere();
        ObjectPtr Parse_Superellipsoid();
        ObjectPtr Parse_Torus();
        ObjectPtr Parse_Triangle();
        ObjectPtr Parse_Mesh();
        ObjectPtr Parse_Mesh2();

#if POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
        void Parse_Obj (Mesh*);
#endif
        void Parse_Mesh1 (Mesh*);
        void Parse_Mesh2 (Mesh*);

        TEXTURE *Parse_Mesh_Texture(TEXTURE **t2, TEXTURE **t3);
        ObjectPtr Parse_TrueType(void);
        void Parse_Blob_Element_Mods(Blob_Element *Element);

        TrueTypeFont *OpenFontFile(const char *asciifn, const int font_id);

        void Parse_Mesh_Camera (Camera& Cam);
        void Parse_User_Defined_Camera (Camera& Cam);
        void Parse_Camera(Camera& Cam);
        bool Parse_Camera_Mods(Camera& Cam);
        void Parse_Frame();

        void Link(ObjectPtr New_Object, vector<ObjectPtr>& Object_List_Root);
        void Link_To_Frame(ObjectPtr Object);
        void Post_Process(ObjectPtr Object, ObjectPtr Parent);

        void Parse_Global_Settings();
        void Global_Setting_Warn();

        void Set_CSG_Children_Flag(ObjectPtr, unsigned int, unsigned int, unsigned int);
        void Set_CSG_Tree_Flag(ObjectPtr, unsigned int, int);

        ObjectPtr Parse_Isosurface();
        ObjectPtr Parse_Parametric();
        void ParseContainedBy(shared_ptr<ContainedByShape>& container, ObjectPtr obj);

        ObjectPtr Parse_Sphere_Sweep(void);
        int Parse_Three_UVCoords(Vector2d& UV1, Vector2d& UV2, Vector2d& UV3);

        void SignalProgress(POV_LONG elapsedTime, POV_LONG tokenCount);

        // tokenize.h/tokenize.cpp
        void Echo_ungetc (int c);
        int Echo_getc (void);
        /// Advance to the next non-whitespace character.
        bool Skip_Spaces (void);
        /// Advance to the next hash sign.
        /// Hash signs inside comments or strings are ignored.
        bool SkipToDirective(void);
        int Parse_C_Comments (void);
        inline void Begin_String (void);
        inline void Stuff_Character (int c);
        inline void End_String (void);
        inline void Begin_String_Fast (void);
        inline void Stuff_Character_Fast (int c);
        inline void End_String_Fast (void);
        bool Read_Float (void);
        void Read_Symbol (void);
        SYM_ENTRY *Find_Symbol (const SYM_TABLE *table, const char *s, int hash);
        SYM_ENTRY *Find_Symbol (const SYM_TABLE *table, const char *s);
        SYM_ENTRY *Find_Symbol (int index, const char *s);
        SYM_ENTRY *Find_Symbol (const char *s);
        void Skip_Tokens (COND_TYPE cond);
        void Break (void);

        int get_hash_value (const char *s);
        inline void Write_Token (TOKEN Token_Id, int col, SYM_TABLE *table = NULL);
        void Destroy_Table (int index);
        void init_sym_tables (void);
        void Add_Sym_Table ();
        SYM_TABLE *Create_Sym_Table (bool copyNames);
        void Destroy_Sym_Table (SYM_TABLE *);
        SYM_TABLE *Copy_Sym_Table (const SYM_TABLE *);
        void Remove_Symbol (SYM_TABLE *table, const char *Name, bool is_array_elem, void **DataPtr, int ttype);
        void Remove_Symbol (int Index, const char *Name, bool is_array_elem, void **DataPtr, int ttype);
        Macro *Parse_Macro(void);
        void Invoke_Macro(void);
        void Return_From_Macro(void);
        void Add_Entry (SYM_TABLE *table, SYM_ENTRY *Table_Entry);
        void Add_Entry (int Index,SYM_ENTRY *Table_Entry);
        void Parse_Initalizer (int Sub, int Base, POV_ARRAY *a);

        void Parse_Fopen(void);
        void Parse_Fclose(void);
        void Parse_Read(void);
        void Parse_Write(void);
        int Parse_Read_Value(DATA_FILE *User_File,int Previous,int *NumberPtr,void **DataPtr);
        void Check_Macro_Vers(void);
        DBL Parse_Cond_Param(void);
        void Parse_Cond_Param2(DBL *V1,DBL *V2);
        void Inc_CS_Index();

        // parstxtr.h/parstxtr.cpp
        void Make_Pattern_Image(ImageData *image, FUNCTION_PTR fn, int token);

        PatternPtr ParseDensityFilePattern();
        PatternPtr ParseImagePattern();
        PatternPtr ParseJuliaPattern();
        PatternPtr ParseMagnetPattern();
        PatternPtr ParseMandelPattern();
        PatternPtr ParsePotentialPattern();
        PatternPtr ParseSlopePattern();
        template<typename PATTERN_T> PatternPtr ParseSpiralPattern();
        void VerifyPavementPattern(shared_ptr<PavementPattern> pattern);
        void VerifyTilingPattern(shared_ptr<TilingPattern> pattern);
        void VerifyPattern(PatternPtr pattern);
        void Parse_Bump_Map (TNORMAL *Tnormal);
        void Parse_Image_Map (PIGMENT *Pigment);
        template<typename MAP_T, typename PATTERN_T> void Parse_Pattern (PATTERN_T *New, BlendMapTypeId TPat_Type);
        TEXTURE *Parse_Vers1_Texture (void);
        TEXTURE *Parse_Tiles (void);
        TEXTURE *Parse_Material_Map (void);
        void Parse_Texture_Transform (TEXTURE *Texture);
        ClassicTurbulence *Check_Turb (WarpList& warps, bool patternHandlesTurbulence);
        void Parse_Warp (WarpList& warps);
        void Check_BH_Parameters (BlackHoleWarp *bh);

        // parsestr.h/parsestr.cpp
        UCS2 *Parse_Str(bool pathname);
        UCS2 *Parse_VStr(bool pathname);
        UCS2 *Parse_Concat(bool pathname);
        UCS2 *Parse_Chr(bool pathname);
        UCS2 *Parse_CameraType(bool pathname);
        UCS2 *Parse_Datetime(bool pathname);
        UCS2 *Parse_Substr(bool pathname);
        UCS2 *Parse_Strupr(bool pathname);
        UCS2 *Parse_Strlwr(bool pathname);

        UCS2 *Convert_UTF8_To_UCS2(const unsigned char *text_array, int *char_array_size);

        // express.h/express.cpp
        void Parse_Vector_Param (Vector3d& Vector);
        void Parse_Vector_Param2 (Vector3d& Vect1, Vector3d& Vect2);
        void Parse_Trace(Vector3d& Res);
        void Parse_UV_Vertex(Vector3d& Res);
        void Parse_UV_Normal(Vector3d& Res);
        void Parse_UV_Min(Vector3d& Res);
        void Parse_UV_Max(Vector3d& Res);
        int Parse_Inside();
        bool Parse_Call();
        DBL Parse_Function_Call();
        void Parse_Vector_Function_Call(EXPRESS& Express, int *Terms);
        void Parse_Camera_Access(Vector3d& Vector, const TOKEN tk);
        void Parse_Spline_Call(EXPRESS& Express, int *Terms);

        /// Parses a NUMERIC_FACTOR or VECTOR_FACTOR.
        void Parse_Num_Factor (EXPRESS& Express,int *Terms);

        /// Parses a NUMERIC_TERM or VECTOR_TERM.
        void Parse_Num_Term (EXPRESS& Express, int *Terms);

        /// Parses a FLOAT or VECTOR.
        void Parse_Rel_Factor (EXPRESS& Express,int *Terms);

        /// Parses a REL_TERM (including FLOAT) or VECTOR.
        void Parse_Rel_Term (EXPRESS& Express, int *Terms);

        /// Parses a REL_TERM comparing two strings.
        DBL Parse_Rel_String_Term (const UCS2 *lhs);

        /// Parses a LOGICAL_EXPRESSION (including FLOAT) or VECTOR.
        void Parse_Logical (EXPRESS& Express, int *Terms);

        /// Parses a FULL_EXPRESSION or VECTOR_FULL_EXPRESSION.
        void Parse_Express (EXPRESS& Express, int *Terms);

        void Promote_Express (EXPRESS& Express,int *Old_Terms,int New_Terms);
        void POV_strupr (char *s);
        void POV_strlwr (char *s);

        DBL stream_rand (int stream);
        int stream_seed (int seed);

        // fnsyntax.h/fnsyntax.cpp
        ExprNode *FNSyntax_ParseExpression();
        ExprNode *FNSyntax_GetTrapExpression(unsigned int);
        void FNSyntax_DeleteExpression(ExprNode *);

        ExprNode *parse_expr();
        TOKEN expr_get_token();
        ExprNode *new_expr_node(int stage, int op);

        void optimise_expr(ExprNode *node);
        void optimise_call(ExprNode *node);
        bool right_subtree_has_variable_expr(ExprNode *node);
        bool left_subtree_has_variable_expr(ExprNode *node);
        void dump_expr(FILE *f, ExprNode *node);

        // [CLi] moved this from photons to parser
        void CheckPassThru(ObjectPtr o, int flag);

        // TODO - obsolete
        RGBFTColour *Create_Colour (void);
        RGBFTColour *Copy_Colour (const RGBFTColour* Old);



        // tesselation.cpp
        struct UNDERCONSTRUCTION
        {
            Mesh* tesselationMesh;
            int number_of_normals, number_of_textures, number_of_triangles, number_of_vertices;
            int max_normals, max_textures, max_vertices, max_triangles;
            SnglVector3d *Normals, *Vertices;
            MESH_TRIANGLE *Triangles;
            TEXTURE **Textures;
            TEXTURE *Default_Texture;
            int albinos;
            Vector3d Inside_Vect;
        };

        struct GTSInfo
        {
            ObjectPtr  object;
            bool reverse; /* inverse right hand and left hand */
        };

        struct STLInfo
        {
            ObjectPtr  object;
            bool reverse;/* inverse right hand and left hand */
        };

        struct STL_Entry
        {
            float normal[3];
            float vertex1[3];
            float vertex2[3];
            float vertex3[3];
            uint_least16_t attribute;
        };

        struct GTS_Edge
        {
            unsigned int first;
            unsigned int second;
            bool operator<(const GTS_Edge& o) const
            {
                return (first<o.first)||((first == o.first)&&(second<o.second));
            }
            GTS_Edge(const unsigned int a, const unsigned int b):first(a),second(b){}
        };

        struct TetraCoordInfo
        {
            ObjectPtr Object;
            Vector3d Coord[8];
            int Inside[8];
            Vector3d Intersection[8][8][2];
            TEXTURE *Texture[8][8];
        };

        struct MarchingCubeInfo
        {
            ObjectPtr Object;
            Vector3d Coord[8]; /* cube vertex */
            int Inside[8];
            Vector3d Vertex[12]; /* vertex on the edge */
        };

        struct GTetraCoordInfo
        {
            ObjectPtr Object;
            Vector3d Coord[8];
            int Has_inter[8][8];
            Vector3d Intersection[8][8][2];
        };

        struct CristalCoordInfo
        {
            Vector3d Coord[8];
            int Inside[8];
            int Count_Inside;
        };


        struct CubCoordInfo
        {
            Vector3d Coord[8];
            int Inside[8];
        };

        struct ScrewCoordInfo
        {
            TEXTURE *NPat;
            Vector3d origin;
            Vector3d axis;
            Vector3d angle;
            DBL    x2,y2,z2,xy,xz,yz;
            Vector3d no_move;
            DBL amount;
            int have_min;
            int have_max;
            DBL min_angle;
            DBL max_angle;
            int inverse; /* only to reverse a screw from left to right */
        };

        struct DisplaceInfo
        {  TEXTURE *NPat;
            DBL amount;
            DBL offset;
            int have_inside;
            Vector3d inside;
        };

        struct PlanetInfo
        {
            Vector3d center;
            unsigned int iteration;
            DBL amount;
            DBL jitter;
            int seed;
        };



        void StartAddingTriangles(UNDERCONSTRUCTION *das);
        void ExpandTrianglesTable(UNDERCONSTRUCTION *das);
        void AddVertices(Vector3d& P1, Vector3d& P2, Vector3d& P3,UNDERCONSTRUCTION *das );
        void AddTriangle(Vector3d& P1, Vector3d& P2, Vector3d& P3,
                TEXTURE *T1, TEXTURE *T2, TEXTURE *T3,
                UNDERCONSTRUCTION *das);
        void AddSmoothTriangle(Vector3d& P1, Vector3d& P2, Vector3d& P3,
                TEXTURE *T1, TEXTURE *T2, TEXTURE *T3,
                Vector3d& N1, Vector3d& N2, Vector3d& N3, UNDERCONSTRUCTION *das);
        void DoneAddingTriangles(UNDERCONSTRUCTION *das);
        ObjectPtr  Gts_Load_Object(char *filename, GTSInfo *info,
                UNDERCONSTRUCTION *das);
        ObjectPtr  Stl_Load_Object(char *filename, STLInfo *info,
                UNDERCONSTRUCTION *das);
        void Stl_Save_Object(char *filename, STLInfo *info);
        void Gts_Save_Object(char *filename, GTSInfo *info,
                UNDERCONSTRUCTION *das);
        void AssignTCoord(TetraCoordInfo* info, int cInd, DBL xc, DBL yc, DBL zc);
        void CopyTCoord(TetraCoordInfo* info, int sInd, int dInd);
        void CopyIntersection(TetraCoordInfo* info,
                int sInd1, int sInd2, int dInd1, int dInd2);
        Intersection tess_intersect;
        void CalcIntersection(TetraCoordInfo* info, int P1ind, int P2ind,
                int Smooth);
        void TesselateTetrahedron(TetraCoordInfo* info,
                int P1ind, int P2ind, int P3ind, int P4ind,
				int Smooth, UNDERCONSTRUCTION *das);
        ObjectPtr Tesselate_Object(ObjectPtr Obj,
				int XAccuracy, int YAccuracy, int ZAccuracy,
				int Sm, DBL BBoxOffs, UNDERCONSTRUCTION *das);
        static const short bourke_table[256][16];
        static const short heller_table[256][13];
        static const short edgeTable[256];
        void AssignMCCoord(MarchingCubeInfo* info, int cInd,
                DBL xc, DBL yc, DBL zc);
        void CopyMCCoord(MarchingCubeInfo* info, int sInd, int dInd);
        void InterpolMC(int edge, int Preci, MarchingCubeInfo * info,
                int ind1, int ind2);
        ObjectPtr BourkeHeller_Object(ObjectPtr Obj, int which,
                int XAccuracy, int YAccuracy, int ZAccuracy,
                int Preci, DBL BBoxOffs, UNDERCONSTRUCTION *das);
        void AssignGTCoord(GTetraCoordInfo* info, int cInd,
                DBL xc, DBL yc, DBL zc);
        void CopyGTCoord(GTetraCoordInfo* info, int sInd, int dInd);
        ObjectPtr GTesselate_Object(ObjectPtr Obj,
                int XAccuracy, int YAccuracy, int ZAccuracy,
                int Sm, DBL BBoxOffs, UNDERCONSTRUCTION *das);
        void AssignCrCoord(CristalCoordInfo* info, int cInd,
                DBL xc, DBL yc, DBL zc);
        void CopyCrCoord(CristalCoordInfo* info, int sInd, int dInd);
        void CopyCrIntersection(CristalCoordInfo* info,
                int sInd1, int sInd2, int dInd1, int dInd2);
        void AddOneSurfaceCristal(CristalCoordInfo* info, int P1ind, int P2ind, int P3ind, int P4ind, 
                UNDERCONSTRUCTION *das);
        void AnalyseCubeFaceCristal(CristalCoordInfo* info, int P1ind,
                int P2ind, int P3ind, int P4ind,
                UNDERCONSTRUCTION *das);
        ObjectPtr Cristallise_Object(ObjectPtr Obj,
                int XAccuracy, int YAccuracy, int ZAccuracy,
                UNDERCONSTRUCTION *das);
        ObjectPtr Gridify_Object(ObjectPtr Obj, DBL factor, DBL nor);
        void AssignCuCoord(CubCoordInfo* info, int cInd,
				DBL xc, DBL yc, DBL zc);
        void CopyCuCoord(CubCoordInfo* info, int sInd, int dInd);
        void CopyCuIntersection(CubCoordInfo* info,
                int sInd1, int sInd2, int dInd1, int dInd2);
        static const int orthotable[5][2];
        void AddOneSurfaceCube(CubCoordInfo* info, int P1ind, int P2ind,
                UNDERCONSTRUCTION *das);
        ObjectPtr Tesselate_Object_Cube(ObjectPtr Obj,
                int XAccuracy, int YAccuracy, int ZAccuracy,
                UNDERCONSTRUCTION *das);
        void Init_Rotation_Transform(ScrewCoordInfo * info);
        void Comp_Rotation_Transform(TRANSFORM *transform, ScrewCoordInfo * info,
                DBL amount);
        void Extract_Normal_Direct(SnglVector3d*n,
                int norma, SnglVector3d vertex, SnglVector3d extr1, SnglVector3d extr2, Vector3d& modif);
        ObjectPtr Screwlise_Object(ObjectPtr Obj,
                ScrewCoordInfo *info, 
                UNDERCONSTRUCTION *das);
        ObjectPtr Roll_Object(ObjectPtr Obj,
                ScrewCoordInfo *info,
                UNDERCONSTRUCTION *das);
        ObjectPtr Bend_Object(ObjectPtr Obj,
                ScrewCoordInfo *info,
                UNDERCONSTRUCTION *das);
        struct SmoothInfo
        {
            int method;
            DBL amount;
        };
        struct SelectInfo
        {
            ObjectPtr  bound;
            int inside;
            int outside;
            int bin;
            int bout;
        };

        void Extract_Normal(SmoothInfo *info,SnglVector3d*n,
                int norma, SnglVector3d vertex, SnglVector3d extr1, SnglVector3d extr2, Vector3d& modif);
        ObjectPtr Smooth_Object(ObjectPtr Obj,
                SmoothInfo *info, UNDERCONSTRUCTION *das);
        struct WarpInfo
        {
            TPATTERN dummy_texture;// used for warps
            TEXTURE *NPat;
            TRANSFORM transform;
        };

        struct MoveInfo
        {  
            TEXTURE *NPat;
            TRANSFORM transform;
        };

        ObjectPtr Warp_Object(ObjectPtr Obj,
                WarpInfo &info, UNDERCONSTRUCTION *das);
        ObjectPtr Move_Object(ObjectPtr Obj,
                MoveInfo *info, UNDERCONSTRUCTION *das);
        ObjectPtr Select_Object(ObjectPtr Obj,
                SelectInfo *info, UNDERCONSTRUCTION *das);
        void Compute_Normal(Vector3d& Norm,Vector3d& V0,Vector3d& V1,Vector3d& V2,Vector3d& old);
        ObjectPtr Displace_Object(ObjectPtr Obj,
                DisplaceInfo *info, UNDERCONSTRUCTION *das);
        ObjectPtr Planet_Object(ObjectPtr Obj,
                PlanetInfo *info, UNDERCONSTRUCTION *das);
        Mesh* ParseParameter(int OneParam);
        int ParseSecondParameter(void);
        void Apply_Texture(bool& full, UNDERCONSTRUCTION *das,int n_tri,TEXTURE *textu);
};

}

#endif // POVRAY_PARSER_PARSE_H
