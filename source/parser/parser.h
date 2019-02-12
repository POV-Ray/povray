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

#ifndef POVRAY_PARSER_PARSE_H
#define POVRAY_PARSER_PARSE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "parser/configparser.h"
#include "parser/parser_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <chrono>
#include <memory>
#include <string>
#include <vector>

// Boost headers
#include <boost/intrusive_ptr.hpp>

// POV-Ray header files (base module)
#include "base/base_fwd.h"
#include "base/messenger_fwd.h"
#include "base/povassert.h"
#include "base/stringtypes.h"
#include "base/textstream_fwd.h"
#include "base/textstreambuffer.h"
#include "base/image/image_fwd.h"

// POV-Ray header files (core module)
#include "core/core_fwd.h"
#include "core/material/blendmap.h"
#include "core/material/pigment.h"
#include "core/material/warp.h"
#include "core/scene/atmosphere_fwd.h"
#include "core/scene/camera.h"

// POV-Ray header files (VM module)
#include "vm/fnpovfpu_fwd.h"

// POV-Ray header files (parser module)
#include "parser/fncode.h"
#include "parser/parsertypes.h"
#include "parser/reservedwords.h"
#include "parser/rawtokenizer.h"
#include "parser/symboltable.h"

namespace pov
{

class Blob_Element;
struct ContainedByShape;
struct GenericSpline;
class ImageData;
class Mesh;
struct PavementPattern;
struct TilingPattern;
struct TrueTypeFont;

}
// end of namespace pov

namespace pov_parser
{

using namespace pov_base;
using namespace pov;

const int TOKEN_OVERFLOW_RESET_COUNT = 2500;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Here we create our own little language for doing the parsing.  It
  makes the code easier to read. */

#define GET(x)          Get_Token(); \
                        if (CurrentTrueTokenId() != x) \
                            Parse_Error (x);

#define ALLOW(x)        Get_Token(); \
                        if (CurrentTrueTokenId() != x) \
                            Unget_Token();

#define UNGET           Unget_Token();

#define EXPECT          EXPECT_(CurrentTrueTokenId())
#define EXPECT_CAT      EXPECT_(CurrentCategorizedTokenId())
#define EXPECT_ONE      EXPECT_ONE_(CurrentTrueTokenId())
#define EXPECT_ONE_CAT  EXPECT_ONE_(CurrentCategorizedTokenId())

#define EXPECT_(x)      { \
                            bool exitExpect = false; \
                            while (!exitExpect) \
                            { \
                                Get_Token(); \
                                switch (x) \
                                {

#define EXPECT_ONE_(x)  { \
                            { \
                                Get_Token(); \
                                switch (x) \
                                {

#define CASE(x)                     case x:
#define CASE2(x, y)                 case x: case y:
#define CASE3(x, y, z)              case x: case y: case z:
#define CASE4(w, x, y, z)           case w: case x: case y: case z:
#define CASE5(v, w, x, y, z)        case v: case w: case x: case y: case z:
#define CASE6(u, v, w, x, y, z)     case u: case v: case w: case x: case y: case z:
#define OTHERWISE                   default:

#define CASE_FLOAT_RAW              CASE2 (LEFT_PAREN_TOKEN,FLOAT_TOKEN_CATEGORY) \
                                    CASE4 (PLUS_TOKEN,DASH_TOKEN,FUNCT_ID_TOKEN,EXCLAMATION_TOKEN)

#define CASE_VECTOR_RAW             CASE3 (VECTFUNCT_ID_TOKEN,VECTOR_TOKEN_CATEGORY,LEFT_ANGLE_TOKEN) \
                                    CASE5 (U_TOKEN,V_TOKEN,UV_ID_TOKEN,VECTOR_4D_ID_TOKEN,SPLINE_ID_TOKEN) \
                                    CASE_FLOAT_RAW

#define CASE_COLOUR_RAW             CASE3 (COLOUR_TOKEN,COLOUR_TOKEN_CATEGORY,COLOUR_ID_TOKEN)

#define CASE_EXPRESS_RAW            CASE_VECTOR_RAW CASE_COLOUR_RAW

#define CASE_FLOAT_UNGET            CASE_FLOAT_RAW      UNGET
#define CASE_VECTOR_UNGET           CASE_VECTOR_RAW     UNGET
#define CASE_COLOUR_UNGET           CASE_COLOUR_RAW     UNGET
#define CASE_EXPRESS_UNGET          CASE_EXPRESS_RAW    UNGET

#define END_CASE                        break;
#define FALLTHROUGH_CASE                // FALLTHROUGH
#define EXIT                            exitExpect = true;

#define END_EXPECT              } \
                            } \
                        }


struct ExperimentalFlags final
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

struct BetaFlags final
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

class Parser final
{
    public:

        using Options = ParserOptions;

        class DebugTextStreamBuffer final : public TextStreamBuffer
        {
            public:
                DebugTextStreamBuffer(GenericMessenger& m);
                virtual ~DebugTextStreamBuffer() override;
            protected:
                virtual void lineoutput(const char *str, unsigned int chars) override;
                virtual void directoutput(const char *str, unsigned int chars) override;

                GenericMessenger& mMessenger;
        };

        // tokenize.h/tokenize.cpp

        /// Structure holding information about the current token
        struct Token_Struct final : MessageContext
        {
            RawToken raw;
            ConstStreamPtr sourceFile;
            int context;                        ///< Context the token is local to (i.e., table index).
            DBL Token_Float;                    ///< Token value (if it is a float literal).
            void *Data;                         ///< Reference to token value (if it is a non-float identifier).
            TokenId *NumberPtr;
            void **DataPtr;
            SymbolTable* table;                 ///< Table or dictionary the token references an element of.
            bool Unget_Token            : 1;    ///< `true` if @ref Get_Token() must re-issue this token as-is.
            bool ungetRaw               : 1;    ///< `true` if @ref Get_Token() must re-evaluate this token from raw.
            bool End_Of_File            : 1;
            bool is_array_elem          : 1;    ///< `true` if token is an array element reference.
            bool is_mixed_array_elem    : 1;    ///< `true` if token is a mixed-type array element reference.
            bool is_dictionary_elem     : 1;    ///< `true` if token is a dictionary element reference.

            void SetTokenId(const RawToken& rawToken);
            void SetTokenId(TokenId tokenId);

            /// Token ID, or identifier type ID (`XXX_ID_TOKEN`) in case of identifier token.
            TokenId GetTrueTokenId() const;

            /// Token category if applicable, otherwise equal to @ref mTrueTokenId.
            TokenId GetCategorizedTokenId() const;

            virtual UCS2String GetFileName() const override;
            virtual POV_LONG GetLine() const override { return raw.lexeme.position.line; }
            virtual POV_LONG GetColumn() const override { return raw.lexeme.position.column; }
            virtual POV_OFF_T GetOffset() const override { return raw.lexeme.position.offset; }

        private:

            TokenId mTrueTokenId;               ///< Token ID, or identifier type ID in case of identifier token.
            TokenId mCategorizedTokenId;        ///< Token category if applicable, otherwise equal to @ref mTrueTokenId.
        };

        struct LValue final
        {
            TokenId*     numberPtr;
            void**       dataPtr;
            TokenId      previous;
            SYM_ENTRY*   symEntry;
            bool         allowRedefine : 1;
            bool         optional      : 1;
        };

        struct MacroParameter final
        {
            char*   name;
            bool    optional;
        };

        struct Macro final : public Assignable
        {
            Macro(const char *);
            virtual ~Macro() override;
            virtual Macro* Clone() const override { POV_PARSER_PANIC(); return nullptr; }
            char *Macro_Name;
            RawTokenizer::ColdBookmark source;
            LexemePosition endPosition; ///< The position _after_ the `#` in the terminating `#end` directive.
            std::vector<MacroParameter> parameters;
            unsigned char *Cache;
            size_t CacheSize;
        };

        struct POV_ARRAY final : public Assignable
        {
            static const int kMaxDimensions = 5;
            int maxDim;                             ///< Index of highest dimension.
            TokenId Type_;
            int Sizes[kMaxDimensions];
            size_t Mags[kMaxDimensions];
            std::vector<void*> DataPtrs;
            std::vector<TokenId> Types;
            bool resizable : 1;
            bool mixedType : 1;
            bool IsInitialized() const;
            bool HasElement(size_t i) const;
            const TokenId& ElementType(size_t i) const;
            TokenId& ElementType(size_t i);
            size_t GetLinearSize() const;
            void Grow();
            void GrowBy(size_t delta);
            void GrowTo(size_t delta);
            void Shrink();
            POV_ARRAY() = default;
            POV_ARRAY(const POV_ARRAY& obj);
            virtual ~POV_ARRAY() override;
            virtual POV_ARRAY* Clone() const override;
        };

        struct POV_PARAM final
        {
            TokenId *NumberPtr;
            void **DataPtr;
        };

        struct DATA_FILE final : public Assignable
        {
            std::shared_ptr<RawTokenizer>           inTokenizer;
            RawToken                                inToken;
            std::shared_ptr<pov_base::OTextStream>  Out_File;
            bool inUngetToken   : 1;
            bool busyParsing    : 1; ///< `true` if parsing a statement related to the file, `false` otherwise.

            bool ReadNextToken()
            {
                POV_PARSER_ASSERT(inTokenizer != nullptr);
                if (!inUngetToken && !inTokenizer->GetNextToken(inToken))
                {
                    inToken.id = END_OF_FILE_TOKEN;
                    return false;
                }
                inUngetToken = false;
                return true;
            }
            void UnReadToken()
            {
                POV_PARSER_ASSERT(!inUngetToken);
                if (inToken.id != END_OF_FILE_TOKEN)
                    inUngetToken = true;
            }

            DATA_FILE() : inUngetToken(false), busyParsing(false) {}
            virtual DATA_FILE* Clone() const override { POV_PARSER_PANIC(); return nullptr; }
        };

        /// @todo Refactor code to use @ref FunctionVM::CustomFunction instead.
        struct AssignableFunction final : public Assignable
        {
            FUNCTION_PTR                        fn;
            boost::intrusive_ptr<FunctionVM>    vm;
            AssignableFunction(const AssignableFunction& obj);
            AssignableFunction(FUNCTION_PTR fn, boost::intrusive_ptr<FunctionVM> vm);
            virtual ~AssignableFunction() override;
            virtual AssignableFunction* Clone() const override { return new AssignableFunction(*this); }
        };

        // constructor
        Parser(std::shared_ptr<SceneData> sd, const Options& opts,
               GenericMessenger& mf, FileResolver& fnr, ProgressReporter& pr, TraceThreadData& td);

        ~Parser();

        /// Called when the parser thread is started.
        /// @todo Obsolete, move content to end of constructor.
        void Run();

        /// Called when the parser thread is closing down.
        /// @todo Obsolete, move content to start of destructor.
        void Finish();

        /// Internal; called to clean up.
        /// @note This is _not_ an override of Task::Cleanup().
        /// @todo Refactor class to move content to destructor.
        void Cleanup();

        inline TraceThreadData *GetParserDataPtr() { return &mThreadData; }

        // parse.h/parse.cpp
        void Parse_Error (TokenId Token_Id);
        void Found_Instead_Error (const char *exstr, const char *extokstr);
        bool Parse_Begin (TokenId tokenId, bool mandatory);
        void Parse_End (TokenId openTokenId, TokenId expectTokenId);
        inline bool Parse_Begin (bool mandatory = true) { return Parse_Begin(LEFT_CURLY_TOKEN, mandatory); }
        inline void Parse_End (void) { Parse_End(LEFT_CURLY_TOKEN, RIGHT_CURLY_TOKEN); }
        inline bool Parse_Paren_Begin (bool mandatory = true) { return Parse_Begin(LEFT_PAREN_TOKEN, mandatory); }
        inline void Parse_Paren_End (void) { Parse_End(LEFT_PAREN_TOKEN, RIGHT_PAREN_TOKEN); }
        inline bool Parse_Angle_Begin (bool mandatory = true) { return Parse_Begin(LEFT_ANGLE_TOKEN, mandatory); }
        inline void Parse_Angle_End (void) { Parse_End(LEFT_ANGLE_TOKEN, RIGHT_ANGLE_TOKEN); }
        inline bool Parse_Square_Begin (bool mandatory = true) { return Parse_Begin(LEFT_SQUARE_TOKEN, mandatory); }
        inline void Parse_Square_End (void) { Parse_End(LEFT_SQUARE_TOKEN, RIGHT_SQUARE_TOKEN); }
        bool Parse_Comma (void);
        bool AllowToken(TokenId tokenId);
        bool Peek_Token (TokenId tokenId);
        void Parse_Semi_Colon (bool force_semicolon);
        void Destroy_Frame (void);
        void MAError (const char *str, long size);
        void Warn_State (TokenId Type);
        void Only_In (const char *s1,const char *s2);
        void Not_With (const char *s1,const char *s2);
        void Warn_Compat (bool definite, const char *sym);
        void Link_Textures (TEXTURE **Old_Texture, TEXTURE *New_Texture);

        /// @note This method includes an implied `Parse_End()`.
        ObjectPtr Parse_Object_Mods (ObjectPtr Object);

        ObjectPtr Parse_Object (void);
        void Parse_Bound_Clip (std::vector<ObjectPtr>& objects, bool notexture = true);
        void Parse_Default (void);
        void Parse_Declare (bool is_local, bool after_hash);
        void Parse_Matrix (MATRIX Matrix);
        bool PassParameterByReference (int oldTableIndex);
        bool Parse_RValue (TokenId Previous, TokenId *NumberPtr, void **DataPtr, SYM_ENTRY *sym, bool ParFlag, bool SemiFlag, bool is_local, bool allow_redefine, bool allowUndefined, int old_table_index);
        const char *Get_Token_String (TokenId Token_Id);
        void Test_Redefine(TokenId Previous, TokenId *NumberPtr, void *Data, bool allow_redefine = true);
        void Expectation_Error(const char *);
        TRANSFORM *Parse_Transform(TRANSFORM *Trans = nullptr);
        TRANSFORM *Parse_Transform_Block(TRANSFORM *New = nullptr);

        void SendFatalError(Exception& e);

        void Warning(const char *format,...);
        void Warning(const MessageContext& loc, const char *format, ...);
        void Warning(WarningLevel level, const char *format,...);
        void Warning(WarningLevel level, const MessageContext& loc, const char *format, ...);
        void VersionWarning(unsigned int sinceVersion, const char *format,...);
        void PossibleError(const char *format,...);
        void Error(const char *format,...);
        void Error(const MessageContext& loc, const char *format, ...);
        void ErrorInfo(const MessageContext& loc, const char *format,...);

        int Debug_Info(const char *format,...);
        void FlushDebugMessageBuffer();

        // NK layers - 1999 July 10 - for backwards compatibility with layered textures
        static void Convert_Filter_To_Transmit(PIGMENT *Pigment);
        static void Convert_Filter_To_Transmit(GenericPigmentBlendMap *pBlendMap);

        /// @param[in]  formalFileName  Name by which the file is known to the user.
        /// @param[out] actualFileName  Name by which the file is known to the parsing computer.
        std::shared_ptr<IStream> Locate_File(const UCS2String& formalFileName, unsigned int stype, UCS2String& actualFileName, bool err_flag = false);

        OStream *CreateFile(const UCS2String& filename, unsigned int stype, bool append);
        Image *Read_Image(int filetype, const UCS2 *filename, const ImageReadOptions& options);

        // tokenize.h/tokenize.cpp
        void Get_Token (void);
        void Unget_Token (void);

        TokenId CurrentCategorizedTokenId() const;
        TokenId CurrentTrueTokenId() const;
        const UTF8String& CurrentTokenText() const;
        const MessageContext& CurrentTokenMessageContext() const;
        void InitCurrentToken();
        void InvalidateCurrentToken();
        void StopSkipping();
        bool IsEndOfSkip() const;
        bool CurrentTokenIsArrayElement() const;
        bool CurrentTokenIsHomogenousArrayElement() const;
        bool CurrentTokenIsDictionaryElement() const;
        bool CurrentTokenIsContainerElement() const;
        void SetOkToDeclare(bool ok);
        bool IsOkToDeclare() const;

        bool HaveCurrentTokenData() const;
        template<typename T> const T& CurrentTokenData() const { return *reinterpret_cast<T*>(mToken.Data); }
        template<typename PTR_T> const PTR_T CurrentTokenDataPtr() const { return reinterpret_cast<PTR_T>(mToken.Data); }
        template<typename T> void SetCurrentTokenData(const T& val) { *reinterpret_cast<T*>(mToken.Data) = val; }

        bool HaveCurrentFile() const;
        const UCS2* CurrentFileName() const;
        const LexemePosition& CurrentFilePosition() const;
        bool HaveCurrentMessageContext() const;
        const MessageContext& CurrentMessageContext() const;
        void SetInputStream(const std::shared_ptr<IStream>& stream);
        RawTokenizer::HotBookmark GetHotBookmark();
        bool GoToBookmark(const RawTokenizer::HotBookmark& bookmark);

        bool IsEndOfInvokedMacro() const;
        void Parse_Directive();
#if POV_DEBUG
        void Parse_Breakpoint();
#endif
        void Parse_Version();
        void Open_Include (void);
        void IncludeHeader(const UCS2String& temp);
        void pre_init_tokenizer (void);
        void Initialize_Tokenizer (void);
        void Terminate_Tokenizer (void);
        void CheckFileSignature();
        POV_ARRAY *Parse_Array_Declare (void);
        SymbolTable* Parse_Dictionary_Declare();
        bool Parse_Ifdef_Param ();
        int Parse_For_Param (UTF8String&, DBL*, DBL*);

        // parstxtr.h/parstxtr.cpp
        TEXTURE *Parse_Texture (void);
        void Parse_Pigment (PIGMENT **);
        void Parse_Tnormal (TNORMAL **);
        void Parse_Finish (FINISH **);
        void Parse_Media (std::vector<Media>&);
        void Parse_Interior (InteriorPtr&);
        void Parse_Media_Density_Pattern (PIGMENT **);
        void Parse_Media_Density_Pattern (std::vector<PIGMENT*>&);
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
        template<typename MAP_T> std::shared_ptr<MAP_T> Parse_Blend_Map (BlendMapTypeId Blend_Type, int Pat_Type);
        template<typename MAP_T> std::shared_ptr<MAP_T> Parse_Colour_Map (void);
        template<typename DATA_T> void Parse_BlendListData (BlendMapTypeId Blend_Type, DATA_T& rData);
        template<typename DATA_T> void Parse_BlendListData_Default (const ColourBlendMapData& Def_Entry, BlendMapTypeId Blend_Type, DATA_T& rData);
        template<typename MAP_T> std::shared_ptr<MAP_T> Parse_Blend_List (int Count, ColourBlendMapConstPtr Def_Map, BlendMapTypeId Blend_Type);
        template<typename MAP_T> std::shared_ptr<MAP_T> Parse_Item_Into_Blend_List (BlendMapTypeId Blend_Type);
        GenericSpline *Parse_Spline (void);

        /// Parses a FLOAT.
        DBL Parse_Float (void);

        /// Parses an optional FLOAT.
        DBL Allow_Float (DBL defval);

        /// Parses a FLOAT as an integer value.
        int Parse_Int(const char* parameterName = nullptr);

        /// Parses a FLOAT as an integer value with a given minimum.
        int Parse_Int_With_Minimum(int minValue, const char* parameterName = nullptr);

        /// Parses a FLOAT as an integer value with a given range.
        int Parse_Int_With_Range(int minValue, int maxValue, const char* parameterName = nullptr);

        /// Parses a FLOAT as a boolean value.
        bool Parse_Bool(const char* parameterName = nullptr);

        int Allow_Vector (Vector3d& Vect);
        void Parse_UV_Vect (Vector2d& UV_Vect);
        void Parse_Vector (Vector3d& Vector);
        void Parse_Vector4D (VECTOR_4D Vector);
        int Parse_Unknown_Vector(EXPRESS& Express, bool allow_identifier = false, bool *had_identifier = nullptr);
        void Parse_Scale_Vector (Vector3d& Vector);
        DBL Parse_Float_Param (void);
        void Parse_Float_Param2 (DBL *Val1, DBL *Val2);
        void Init_Random_Generators (void);
        void Destroy_Random_Generators (void);

        // function.h/function.cpp
        FUNCTION_PTR Parse_Function(void);
        FUNCTION_PTR Parse_FunctionContent(void);
        FUNCTION_PTR Parse_FunctionOrContent(void);
        void Parse_FunctionOrContentList(GenericScalarFunctionPtr* apFn, unsigned int count, bool mandatory = true);
        FUNCTION_PTR Parse_DeclareFunction(TokenId *token_id, const char *fn_name, bool is_local);
        boost::intrusive_ptr<FunctionVM> GetFunctionVM() const;

        // parsestr.h/parsestr.cpp
        char *Parse_C_String(bool pathname = false);
        void ParseString(UTF8String& s, bool pathname = false);
        UCS2 *Parse_String(bool pathname = false, bool require = true);
        std::string Parse_SysString(bool pathname = false, bool require = true);

        UCS2 *String_Literal_To_UCS2(const std::string& str);
        UCS2 *String_To_UCS2(const char *str);
        char *UCS2_To_String(const UCS2 *str);

        static UCS2 *UCS2_strcat(UCS2 *s1, const UCS2 *s2);
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

        std::shared_ptr<SceneData> sceneData;

    private:

        GenericMessenger&   mMessageFactory;
        FileResolver&       mFileResolver;
        ProgressReporter&   mProgressReporter;
        TraceThreadData&    mThreadData;

        DebugTextStreamBuffer Debug_Message_Buffer;

        Token_Struct        mToken;

        struct BraceStackEntry final : LexemePosition, MessageContext
        {
            TokenId         openToken;
            ConstStreamPtr  file;
            BraceStackEntry(const Token_Struct& token) :
                LexemePosition(token.raw.lexeme.position), openToken(token.GetTrueTokenId()), file(token.sourceFile) {}
            virtual UCS2String GetFileName() const override;
            virtual POV_LONG GetLine() const override { return line; }
            virtual POV_LONG GetColumn() const override { return column; }
            virtual POV_OFF_T GetOffset() const override { return offset; }
        };

        boost::intrusive_ptr<FunctionVM> mpFunctionVM;
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

        std::vector<BraceStackEntry> maBraceStack;
        bool Destroying_Frame;

        Camera Default_Camera;

        // tokenize.h/tokenize.cpp
        typedef enum cond_type
        {
            ROOT_COND=0,            ///< No conditionals, loops or macros pending.
            BUSY_COND,              ///< Incomplete conditional, busy parsing the actual conditional or loop statement.
            WHILE_COND,             ///< Executing a `#while` loop body.
            FOR_COND,               ///< Executing a `#for` loop body.
            IF_TRUE_COND,           ///< Executing an `#if` (or `#elseif`) body.
            IF_FALSE_COND,          ///< Skipping an `#if` (or `#elseif`) body to execute an `#else` (or `#elseif`) body (if present).
            ELSE_COND,              ///< Executing an '#else` body.
            SWITCH_COND,            ///< Executing a `#switch` body, expecting a `#case` (or `#range`).
            CASE_TRUE_COND,         ///< executing a `#case` (or `#range`) body.
            CASE_FALSE_COND,        ///< Skipping a `#case` (or `#range`) body to execute a later one (if present).
            SKIP_TIL_END_COND,      ///< Skipping a conditional or loop body until a matching `#end`.
            INVOKING_MACRO_COND,    ///< Executing a `#macro` body.
            DECLARING_MACRO_COND    ///< Skipping a `#macro` body during its declaration.
        } COND_TYPE;

        SymbolStack mSymbolStack;

        POV_LONG    mTokenCount;
        int         mTokensSinceLastProgressReport;

        struct IncludeStackEntry final
        {
            RawTokenizer::HotBookmark   returnToBookmark;
            int                         condStackSize;
            int                         braceStackSize;

            IncludeStackEntry(const RawTokenizer::HotBookmark& rtb, int css, int bss) :
                returnToBookmark(rtb), condStackSize(css), braceStackSize(bss)
            {}
        };
        std::vector<IncludeStackEntry> maIncludeStack;

        struct CS_ENTRY final
        {
            COND_TYPE Cond_Type;
            DBL Switch_Value;
            RawTokenizer::HotBookmark returnToBookmark;
            bool Macro_Same_Flag;
            bool Switch_Case_Ok_Flag;
            Macro *PMac;
            UTF8String Loop_Identifier;
            DBL For_Loop_End;
            DBL For_Loop_Step;
            CS_ENTRY() : Cond_Type(BUSY_COND), PMac(nullptr) {}
            ~CS_ENTRY() {}
        };

        std::vector<CS_ENTRY> Cond_Stack;
        bool Skipping, Inside_Ifdef, Inside_MacroDef, Parsing_Directive, parseRawIdentifiers, parseOptionalRValue;

        bool Got_EOF; // WARNING: Changes to the use of this variable are very dangerous as it is used in many places assuming certain non-obvious side effects! [trf]

        RawTokenizer    mTokenizer;
        RawToken        mPendingRawToken;
        bool            mHavePendingRawToken;

        // parstxtr.h/parstxtr.cpp
        TEXTURE *Default_Texture;

        enum class DefaultsVersion : char
        {
            kLegacy,    ///< Pre-v3.8 defaults.
            k380,       ///< v3.8.0 defaults.
        };
        DefaultsVersion defaultsVersion;    ///< Language version active before the first `default` statement.
        bool defaultsModified   : 1;        ///< Whether a `default` statement has been encountered.

        // express.h/express.cpp
        short Have_Vector;
        unsigned int Number_Of_Random_Generators;
        unsigned int *next_rand;

        bool Allow_Identifier_In_Call, Identifier_In_Call;

        size_t MaxCachedMacroSize;

        std::chrono::system_clock::time_point mY2K;

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

#if POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
        void Parse_Obj (Mesh*);
#endif
        void Parse_Mesh1 (Mesh*);
        void Parse_Mesh2 (Mesh*);

        TEXTURE *Parse_Mesh_Texture(TEXTURE **t2, TEXTURE **t3);
        ObjectPtr Parse_TrueType(void);
        void Parse_Blob_Element_Mods(Blob_Element *Element);

        TrueTypeFont *OpenFontFile(const char *asciifn, int font_id, POV_UINT32 cmap, CharsetID charset,
                                   LegacyCharset legacyCharset);

        void Parse_Mesh_Camera (Camera& Cam);
        void Parse_User_Defined_Camera (Camera& Cam);
        void Parse_Camera(Camera& Cam);
        bool Parse_Camera_Mods(Camera& Cam);
        void Parse_Frame();

        void Link(ObjectPtr New_Object, std::vector<ObjectPtr>& Object_List_Root);
        void Link_To_Frame(ObjectPtr Object);
        void Post_Process(ObjectPtr Object, ObjectPtr Parent);

        void Parse_Global_Settings();
        void Global_Setting_Warn();

        void Set_CSG_Children_Flag(ObjectPtr, unsigned int, unsigned int, unsigned int);
        void Set_CSG_Tree_Flag(ObjectPtr, unsigned int, int);

        ObjectPtr Parse_Isosurface();
        ObjectPtr Parse_Parametric();
        void ParseContainedBy(std::shared_ptr<ContainedByShape>& container, ObjectPtr obj);

        ObjectPtr Parse_Sphere_Sweep(void);
        bool Parse_Three_UVCoords(Vector2d& UV1, Vector2d& UV2, Vector2d& UV3);

        // tokenize.h/tokenize.cpp
        void UngetRawToken(const RawToken& rawToken);
        bool GetRawToken(RawToken& rawToken, bool fastForwardToDirective);
        bool PeekRawToken(RawToken& rawToken);
        void Read_Symbol(const RawToken& rawToken);
        void Skip_Tokens (COND_TYPE cond);
        void Break (void);

        inline void Write_Token(const RawToken& rawToken, SymbolTable* table = nullptr);
        inline void Write_Token(TokenId Token_Id, const RawToken& rawToken, SymbolTable* table = nullptr);
        void init_sym_tables (void);
        Macro *Parse_Macro(void);
        void Invoke_Macro(void);
        void Return_From_Macro(void);
        void Parse_Initalizer (int Sub, size_t Base, POV_ARRAY *a);

        void Parse_Fopen(void);
        void Parse_Fclose(void);
        void Parse_Read(void);
        void Parse_Write(void);
        int Parse_Read_Value(DATA_FILE *User_File, TokenId Previous, TokenId *NumberPtr, void **DataPtr);
        bool Parse_Read_Float_Value(DBL& val, DATA_FILE *User_File);
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
        void VerifyPavementPattern(std::shared_ptr<PavementPattern> pattern);
        void VerifyTilingPattern(std::shared_ptr<TilingPattern> pattern);
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
        UCS2 *Parse_Datetime(bool pathname);
        UCS2 *Parse_Substr(bool pathname);
        UCS2 *Parse_Strupr(bool pathname);
        UCS2 *Parse_Strlwr(bool pathname);

        UCS2 *Convert_UTF8_To_UCS2(const unsigned char *text_array, int *char_array_size);

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
        TokenId expr_get_token();
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
};

}
// end of namespace pov_parser

#endif // POVRAY_PARSER_PARSE_H
