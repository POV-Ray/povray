//******************************************************************************
///
/// @file parser/parser.h
///
/// This header file is included by all language parsing C++ modules in POV-Ray.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef PARSE_H
#define PARSE_H

#include "parser/configparser.h"

#include "base/stringutilities.h"
#include "base/textstream.h"
#include "base/textstreambuffer.h"

#include "core/material/pigment.h"
#include "core/shape/blob.h"

#include "parser/reservedwords.h"

#include "backend/control/messagefactory.h"
#include "backend/math/matrices.h"
#include "backend/math/vector.h"
#include "backend/scene/atmosph.h"
#include "backend/scene/camera.h"
#include "backend/scene/scene.h"
#include "backend/support/task.h"

namespace pov
{

using namespace pov_base;

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

struct FPUContext;
class ImageData;
struct GenericSpline;
struct ClassicTurbulence; // full declaration in core/material/warp.h
struct BlackHoleWarp; // full declaration in core/material/warp.h

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

typedef unsigned int FUNCTION;
typedef FUNCTION * FUNCTION_PTR;

// Caution: The compiler depends on the order of these constants!
enum
{
    OP_NONE = 0,    // 0

    OP_CMP_EQ,      // 1
    OP_CMP_NE,      // 2
    OP_CMP_LT,      // 3
    OP_CMP_LE,      // 4
    OP_CMP_GT,      // 5
    OP_CMP_GE,      // 6
    OP_ADD,         // 7
    OP_SUB,         // 8
    OP_OR,          // 9
    OP_MUL,         // 10
    OP_DIV,         // 11
    OP_AND,         // 12
    OP_POW,         // 13
    OP_NEG,         // 14
    OP_NOT,         // 15

    OP_LEFTMOST,    // 16
    OP_FIRST,       // 17

    OP_CONSTANT,    // 18
    OP_VARIABLE,    // 19
    OP_DOT,         // 20
    OP_MEMBER,      // 21
    OP_CALL,        // 22
    OP_TRAP         // 23
};


struct ExperimentalFlags
{
    bool    backsideIllumination    : 1;
    bool    functionHf              : 1;
    bool    meshCamera              : 1;
    bool    slopeAltitude           : 1;
    bool    spline                  : 1;
    bool    subsurface              : 1;
    bool    tiff                    : 1;

    ExperimentalFlags() :
        backsideIllumination(false),
        functionHf(false),
        meshCamera(false),
        slopeAltitude(false),
        spline(false),
        subsurface(false),
        tiff(false)
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

typedef DBL EXPRESS[5];   ///< @todo       Make this obsolete.


class Parser : public SceneTask
{
    public:
        class DebugTextStreamBuffer : public TextStreamBuffer
        {
            public:
                DebugTextStreamBuffer(POVMSAddress ba, POVMSAddress fa, RenderBackend::SceneId sid);
                ~DebugTextStreamBuffer();
            protected:
                virtual void lineoutput(const char *str, unsigned int chars);
                virtual void directoutput(const char *str, unsigned int chars);

                POVMSAddress backendAddress;
                POVMSAddress frontendAddress;
                RenderBackend::SceneId sceneId;
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
            int Table_Index;
            char *Token_String;                             ///< reference to token value (if it is a string literal) or character sequence comprising the token
            DBL Token_Float;                                ///< token value (if it is a float literal)
            int Unget_Token, End_Of_File;
            pov_base::ITextStream *FileHandle;              ///< location of this token in the scene or include file (file)
            void *Data;                                     ///< reference to token value (if it is a non-float identifier)
            int *NumberPtr;
            void **DataPtr;
            bool is_array_elem;                             ///< true if token is actually an array element reference
        };

        struct LValue
        {
            Token_Struct token; // TODO - we probably only need a small subset of the information
            int          previous;
            SYM_ENTRY*   tempEntry;
            bool         allowRedefine;
        };

        Token_Struct Token;

        struct Macro
        {
            Macro(const char *);
            ~Macro();
            char *Macro_Name;
            UCS2 *Macro_Filename;
            pov_base::ITextStream::FilePos Macro_File_Pos;
            POV_LONG Macro_End;
            int Num_Of_Pars;
            char *Par_Name[MAX_PARAMETER_LIST];
        };

        struct POV_ARRAY
        {
            int Dims, Type, Total;
            int Sizes[5];
            int Mags[5];
            void **DataPtrs;
        };

        struct POV_PARAM
        {
            int *NumberPtr;
            void **DataPtr;
            int Table_Index;
        };

        struct DATA_FILE
        {
            pov_base::ITextStream *In_File;
            pov_base::OTextStream *Out_File;
            bool R_Flag;
        };

        // fnsyntax.h/fnsyntax.cpp
        struct ExprNode;

        struct ExprNode
        {
            ExprNode *parent;
            ExprNode *child;
            ExprNode *prev;
            ExprNode *next;
            int stage;
            int op;
            union
            {
                char *variable;
                struct
                {
                    char *name;
                    TOKEN token;
                    FUNCTION fn;
                } call;
                unsigned int trap;
                DBL number;
            };
        };

        // constructor
        Parser(shared_ptr<SceneData> sd, bool useclock, DBL clock);

        void Run();
        void Stopped();
        void Finish();
        void Cleanup(void);

        inline TraceThreadData *GetParserDataPtr() { return reinterpret_cast<TraceThreadData *>(GetDataPtr()); }

        // parse.h/parse.cpp
        void Parse_Error (TOKEN Token_Id);
        void Found_Instead_Error (const char *exstr, const char *extokstr);
        void Parse_Begin (void);
        void Parse_End (void);
        void Parse_Comma (void);
        void Parse_Semi_Colon (bool force_semicolon);
        void Destroy_Frame (void);
        void MAError (const char *str, long size);
        void Warn_State (TOKEN Token_Id, TOKEN Type);
        void Only_In (const char *s1,const char *s2);
        void Not_With (const char *s1,const char *s2);
        void Warn_Compat (bool definite, const char *sym);
        void Link_Textures (TEXTURE **Old_Texture, TEXTURE *New_Texture);

        ObjectPtr Parse_Object_Mods (ObjectPtr Object);
        ObjectPtr Parse_Object (void);
        void Parse_Bound_Clip (vector<ObjectPtr>& objects, bool notexture = true);
        void Parse_Default (void);
        void Parse_Declare (bool is_local, bool after_hash);
        void Parse_Matrix (MATRIX Matrix);
        void Destroy_Ident_Data (void *Data, int Type);
        int Parse_RValue (int Previous, int *NumberPtr, void **DataPtr, SYM_ENTRY *sym, bool ParFlag, bool SemiFlag, bool is_local, bool allow_redefine, int old_table_index);
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

        int Debug_Info(const char *format,...);
        void FlushDebugMessageBuffer();

        static void Convert_Filter_To_Transmit(PIGMENT *Pigment); // NK layers - 1999 July 10 - for backwards compatiblity with layered textures

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
        SYM_ENTRY *Add_Symbol (int Index,const char *Name,TOKEN Number);
        POV_ARRAY *Parse_Array_Declare (void);
        SYM_ENTRY *Create_Entry (int Index,const char *Name,TOKEN Number);
        void Acquire_Entry_Reference (SYM_ENTRY *Entry);
        void Release_Entry_Reference (int Index, SYM_ENTRY *Entry);
        SYM_ENTRY *Destroy_Entry (int Index,SYM_ENTRY *Entry);
        int Parse_Ifdef_Param (void);
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
        FOG *Parse_Fog (void);
        RAINBOW *Parse_Rainbow (void);
        SKYSPHERE *Parse_Skysphere (void);
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
        template<typename DATA_T> void Parse_BlendMapData (int Blend_Type, DATA_T& rData);
        template<typename MAP_T> shared_ptr<MAP_T> Parse_Blend_Map (int Blend_Type, int Pat_Type);
        template<typename MAP_T> shared_ptr<MAP_T> Parse_Colour_Map (void);
        template<typename DATA_T> void Parse_BlendListData (int Blend_Type, DATA_T& rData);
        template<typename DATA_T> void Parse_BlendListData_Default (const ColourBlendMapData& Def_Entry, int Blend_Type, DATA_T& rData);
        template<typename MAP_T> shared_ptr<MAP_T> Parse_Blend_List (int Count, ColourBlendMapConstPtr Def_Map, int Blend_Type);
        template<typename MAP_T> shared_ptr<MAP_T> Parse_Item_Into_Blend_List (int Blend_Type);
        GenericSpline *Parse_Spline (void);

        /// Parses a FLOAT.
        DBL Parse_Float (void);

        /// Parses an optional FLOAT.
        DBL Allow_Float (DBL defval);

        int Allow_Vector (Vector3d& Vect);
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
        FUNCTION_PTR Parse_DeclareFunction(int *token_id, const char *fn_name, bool is_loca);
        void Destroy_Function(FUNCTION_PTR Function);
        static void Destroy_Function(FunctionVM *, FUNCTION_PTR Function);
        FUNCTION_PTR Copy_Function(FUNCTION_PTR Function);
        static FUNCTION_PTR Copy_Function(FunctionVM *, FUNCTION_PTR Function);

        // parsestr.h/parsestr.cpp
        char *Parse_C_String(bool pathname = false);
        UCS2 *Parse_String(bool pathname = false, bool require = true);

        UCS2 *String_Literal_To_UCS2(const char *str, bool pathname = false);
        UCS2 *String_To_UCS2(const char *str);
        char *UCS2_To_String(const UCS2 *str);

        UCS2 *UCS2_strcat(UCS2 *s1, const UCS2 *s2);
        int UCS2_strlen(const UCS2 *str);
        int UCS2_strcmp(const UCS2 *s1, const UCS2 *s2);
        void UCS2_strcpy(UCS2 *s1, const UCS2 *s2);
        void UCS2_strncpy(UCS2 *s1, const UCS2 *s2, int n);
        void UCS2_strupr(UCS2 *str);
        void UCS2_strlwr(UCS2 *str);
        UCS2 *UCS2_strdup(const UCS2 *s);

        // fnsyntax.h/fnsyntax.cpp
        bool expr_noop(ExprNode *&current, int stage, int op);
        bool expr_grow(ExprNode *&current, int stage, int op);
        bool expr_call(ExprNode *&current, int stage, int op);
        bool expr_put(ExprNode *&current, int stage, int op);
        bool expr_new(ExprNode *&current, int stage, int op);
        bool expr_ret(ExprNode *&current, int stage, int op);
        bool expr_err(ExprNode *&current, int stage, int op);

        shared_ptr<SceneData> sceneData; // TODO FIXME HACK - make private again once Locate_Filename is fixed [trf]
    private:
        FPUContext *fnVMContext;

        bool Had_Max_Trace_Level;
        int Max_Trace_Level;

        ExperimentalFlags mExperimentalFlags;
        BetaFlags mBetaFeatureFlags;

        DBL clockValue;
        bool useClock;

        // parse.h/parse.cpp
        short Not_In_Default;
        short Ok_To_Declare;
        short LValue_Ok;

        TOKEN *Brace_Stack;
        int Brace_Index;
        bool Destroying_Frame;

        Camera Default_Camera;

        // tokenize.h/tokenize.cpp
        typedef enum cond_type
        {
            ROOT_COND=0,
            WHILE_COND,
            FOR_COND,
            IF_TRUE_COND,
            IF_FALSE_COND,
            ELSE_COND,
            SWITCH_COND,
            CASE_TRUE_COND,
            CASE_FALSE_COND,
            SKIP_TIL_END_COND,
            INVOKING_MACRO_COND,
            DECLARING_MACRO_COND
        } COND_TYPE;

        struct SYM_TABLE
        {
            SYM_ENTRY *Table[SYM_TABLE_SIZE];
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
            bool Macro_Same_Flag;
            bool Switch_Case_Ok_Flag;
            Macro *PMac;
            pov_base::ITextStream::FilePos File_Pos;
            char* Loop_Identifier;
            DBL For_Loop_End;
            DBL For_Loop_Step;
        };

        CS_ENTRY *Cond_Stack;
        int CS_Index, Skipping, Inside_Ifdef, Inside_MacroDef;

        int Got_EOF; // WARNING: Changes to the use of this variable are very dangerous as it is used in many places assuming certain non-obvious side effects! [trf]

        TOKEN Conversion_Util_Table[LAST_TOKEN];

        // parstxtr.h/parstxtr.cpp
        TEXTURE *Default_Texture;

        // express.h/express.cpp
        short Have_Vector;
        unsigned int Number_Of_Random_Generators;
        unsigned int *next_rand;

        bool Allow_Identifier_In_Call, Identifier_In_Call;

        // parse.h/parse.cpp
        void Frame_Init(void);
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
        ObjectPtr Parse_Light_Source();
        ObjectPtr Parse_Object_Id();
        ObjectPtr Parse_Ovus();
        ObjectPtr Parse_Plane();
        ObjectPtr Parse_Poly(int order);
        ObjectPtr Parse_Polynom();
        ObjectPtr Parse_Polygon();
        ObjectPtr Parse_Prism();
        ObjectPtr Parse_Quadric();
        ObjectPtr Parse_Smooth_Triangle();
        ObjectPtr Parse_Sor();
        ObjectPtr Parse_Sphere();
        ObjectPtr Parse_Superellipsoid();
        ObjectPtr Parse_Torus();
        ObjectPtr Parse_Triangle();
        ObjectPtr Parse_Mesh();
        ObjectPtr Parse_Mesh2();
        TEXTURE *Parse_Mesh_Texture(TEXTURE **t2, TEXTURE **t3);
        ObjectPtr Parse_TrueType(void);
        void Parse_Blob_Element_Mods(Blob_Element *Element);

        void Parse_Mesh_Camera (Camera& Cam);
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

        ObjectPtr Parse_Isosurface(void);
        ObjectPtr Parse_Parametric(void);

        ObjectPtr Parse_Sphere_Sweep(void);
        int Parse_Three_UVCoords(Vector2d& UV1, Vector2d& UV2, Vector2d& UV3);

        // tokenize.h/tokenize.cpp
        void Echo_ungetc (int c);
        int Echo_getc (void);
        bool Skip_Spaces (void);
        int Parse_C_Comments (void);
        inline void Begin_String (void);
        inline void Stuff_Character (int c);
        inline void End_String (void);
        inline void Begin_String_Fast (void);
        inline void Stuff_Character_Fast (int c);
        inline void End_String_Fast (void);
        bool Read_Float (void);
        void Read_Symbol (void);
        SYM_ENTRY *Find_Symbol (int Index, const char *s);
        void Skip_Tokens (COND_TYPE cond);
        void Break (void);

        int get_hash_value (const char *s);
        inline void Write_Token (TOKEN Token_Id, int col);
        void Destroy_Table (int index);
        void init_sym_tables (void);
        void Add_Sym_Table ();
        void Remove_Symbol (int Index, const char *Name, bool is_array_elem, void **DataPtr, int ttype);
        Macro *Parse_Macro(void);
        void Invoke_Macro(void);
        void Return_From_Macro(void);
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
        void Inc_CS_Index(void);

        // parstxtr.h/parstxtr.cpp
        void Make_Pattern_Image(ImageData *image, FUNCTION_PTR fn, int token);

        void Parse_Image_Pattern (TPATTERN *TPattern);
        void Parse_Bump_Map (TNORMAL *Tnormal);
        void Parse_Image_Map (PIGMENT *Pigment);
        template<typename MAP_T, typename PATTERN_T> void Parse_Pattern (PATTERN_T *New, int TPat_Type);
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
        UCS2 *Parse_Datetime(bool pathname);
        UCS2 *Parse_Substr(bool pathname);
        UCS2 *Parse_Strupr(bool pathname);
        UCS2 *Parse_Strlwr(bool pathname);

        UCS4 *Convert_UTF8_To_UCS4(const unsigned char *text_array, int text_array_size, int *char_array_size);
        UCS2 *Convert_UTF8_To_UCS2(const unsigned char *text_array, int text_array_size, int *char_array_size);

        // express.h/express.cpp
        void Parse_Vector_Param (Vector3d& Vector);
        void Parse_Vector_Param2 (Vector3d& Vect1, Vector3d& Vect2);
        void Parse_Trace(Vector3d& Res);
        int Parse_Inside();
        bool Parse_Call();
        DBL Parse_Function_Call();
        void Parse_Vector_Function_Call(EXPRESS& Express, int *Terms);
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
        void Parse_Rel_String_Term (const UCS2 *lhs, EXPRESS& Express, int Terms);

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
};

}

#endif
