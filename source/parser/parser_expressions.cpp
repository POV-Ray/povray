//******************************************************************************
///
/// @file parser/parser_expressions.cpp
///
/// Implementations related to parsing of float, vector and colour expressions.
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
#include "parser/parser.h"

#include <cctype>

#include <algorithm>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "base/fileinputoutput.h"
#include "base/mathutil.h"

#include "core/material/blendmap.h"
#include "core/material/normal.h"
#include "core/material/pattern.h"
#include "core/material/pigment.h"
#include "core/material/texture.h"
#include "core/material/warp.h"
#include "core/math/matrix.h"
#include "core/math/spline.h"
#include "core/math/vector.h"
#include "core/render/trace.h"
#include "core/render/ray.h"
#include "core/scene/object.h"
#include "core/scene/scenedata.h"
#include "core/shape/heightfield.h"
#include "core/support/imageutil.h"

#include "vm/fnpovfpu.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using namespace pov_base;

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define ftrue(f) ((int)(fabs(f)>EPSILON))


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Parser::Parse_Float_Param()
{
    DBL Local;
    EXPRESS Express;
    int Terms = 1;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;

    GET(LEFT_PAREN_TOKEN);
    Parse_Express(Express,&Terms);

    if (Terms>1)
    {
        Error ("Float expected but vector or color expression found.");
    }

    Local = Express[0];

    GET(RIGHT_PAREN_TOKEN);

    Allow_Identifier_In_Call = old_allow_id;

    return (Local);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Float_Param2(DBL *Val1,DBL *Val2)
{
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;

    GET (LEFT_PAREN_TOKEN);
    *Val1 = Parse_Float();
    Parse_Comma();
    *Val2 = Parse_Float();
    GET (RIGHT_PAREN_TOKEN);

    Allow_Identifier_In_Call = old_allow_id;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Vector_Param(Vector3d& Vector)
{
    GET(LEFT_PAREN_TOKEN);
    Parse_Vector(Vector);
    GET(RIGHT_PAREN_TOKEN);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Vector_Param2(Vector3d& Val1, Vector3d& Val2)
{
    GET (LEFT_PAREN_TOKEN);
    Parse_Vector(Val1);
    Parse_Comma();
    Parse_Vector(Val2);
    GET (RIGHT_PAREN_TOKEN);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Trace(Vector3d& Res)
{
    ObjectPtr Object;
    Intersection intersect;
    TraceTicket ticket(1, 0.0);
    Ray ray(ticket);
    Vector3d Local_Normal;

    GET (LEFT_PAREN_TOKEN);

    EXPECT
        CASE (OBJECT_ID_TOKEN)
            Object = reinterpret_cast<ObjectPtr>(Token.Data);
            EXIT
        END_CASE

        OTHERWISE
            Object = NULL;
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if (Object == NULL)
        Error ("Object identifier expected.");

    Parse_Comma();

    Parse_Vector(ray.Origin);
    Parse_Comma();
    Parse_Vector(ray.Direction);
    ray.Direction.normalize();

    Parse_Comma();

    if ( Find_Intersection( &intersect, Object, ray, GetParserDataPtr()) )
    {
        Res = intersect.IPoint;

        intersect.Object->Normal( Local_Normal, &intersect, GetParserDataPtr());

        if (Test_Flag(intersect.Object,INVERTED_FLAG))
            Local_Normal.invert();
    }
    else
    {
        Res[X]=Res[Y]=Res[Z]=0;
        Local_Normal = Vector3d(0.0, 0.0, 0.0);
    }

    EXPECT
        CASE (VECTOR_FUNCT_TOKEN)
            /* All of these functions return a VECTOR result */
            if(Token.Function_Id == VECTOR_ID_TOKEN)
            {
                (*reinterpret_cast<Vector3d *>(Token.Data)) = Local_Normal;
            }
            else
            {
                UNGET
            }
            EXIT
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    GET (RIGHT_PAREN_TOKEN);

}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

int Parser::Parse_Inside()
{
    ObjectPtr Object;
    Vector3d Local_Vector;
    int Result = 0;

    GET (LEFT_PAREN_TOKEN);

    EXPECT
        CASE (OBJECT_ID_TOKEN)
            Object = reinterpret_cast<ObjectPtr>(Token.Data);
            EXIT
        END_CASE

        OTHERWISE
            Object = NULL;
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if (Object == NULL)
        Error ("Object identifier expected.");
    if((Object->Type & PATCH_OBJECT) == PATCH_OBJECT)
        Error ("Solid object identifier expected.");

    Parse_Comma();

    Parse_Vector(Local_Vector);

    if (Inside_Object(Local_Vector, Object, GetParserDataPtr()))
        Result = 1;
    else
        Result = 0;

    GET (RIGHT_PAREN_TOKEN);

    return Result;
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Utility function that determines if this is a call or access of an
*   identifier and stores the result in a global variable.  If an identifier
*   is allowed is determined by another global variable.
*
* CHANGES
*
******************************************************************************/

bool Parser::Parse_Call()
{
    if(Allow_Identifier_In_Call)
    {
        EXPECT_ONE
            CASE(LEFT_PAREN_TOKEN)
                Identifier_In_Call = false;
                return true;
            END_CASE
        END_EXPECT

        UNGET

        Identifier_In_Call = true;
        return false;
    }
    else
    {
        GET(LEFT_PAREN_TOKEN)
    }

    return true;
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   NOTE: Function Parse_RValue in parse.cpp depends on this function to be
*   able to only use Token.Data of the current token in order to have a kind
*   of two token look-ahead in Parse_RValue! [trf]
*
* CHANGES
*
******************************************************************************/

DBL Parser::Parse_Function_Call()
{
    FUNCTION_PTR fp = (FUNCTION_PTR )Token.Data;
    if (fp == NULL)
        // may happen if a #declare or #local inside a function declaration references the function
        Error("Illegal attempt to evaluate a function being currently declared; did you miss a closing brace?");

    // NB while parsing the call parameters, the parser may drop out of the current scope (macro or include file) before we get a chance to invoke the function,
    // in which case *fp will be destroyed, and an attempt made to drop the function. Therefore we copy *fp, and claim dibs on the function.
    // TODO - use smart pointers for this
    FUNCTION fn = *fp;
    FunctionCode *f = dynamic_cast<FunctionVM*>(sceneData->functionContextFactory)->GetFunctionAndReference(fn);

    unsigned int pmax = f->parameter_cnt - 1;
    unsigned int param = 0;
    DBL params[MAX_FUNCTION_PARAMETER_LIST];

    if(Parse_Call() == false)
    {
        // we claimed dibs on the function, so before we exit we must release it
        dynamic_cast<FunctionVM*>(sceneData->functionContextFactory)->RemoveFunction(fn);
        return 0.0;
    }

    // first store results in local array so recursive calls to
    // this function do not overwrite the global function stack
    // accessed by POVFPU_SetLocal [trf]
    for(param = 0; param < pmax; param++)
    {
        params[param] = Parse_Float();
        Parse_Comma();
    }
    params[param] = Parse_Float();

    GET (RIGHT_PAREN_TOKEN);

    for(param = 0; param < f->parameter_cnt; param++)
        fnVMContext->SetLocal(param, params[param]);

    DBL result = POVFPU_Run(fnVMContext, fn);

    // we claimed dibs on the function, so now that we're done with it we must say so
    dynamic_cast<FunctionVM*>(sceneData->functionContextFactory)->RemoveFunction(fn);

    return result;
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   NOTE: Function Parse_RValue in parse.cpp depends on this function to be
*   able to only use Token.Data of the current token in order to have a kind
*   of two token look-ahead in Parse_RValue! [trf]
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Vector_Function_Call(EXPRESS& Express, int *Terms)
{
    FUNCTION_PTR fp = (FUNCTION_PTR )Token.Data;
    if (fp == NULL)
        // may happen if a #declare or #local inside a function declaration references the function
        Error("Illegal attempt to evaluate a function being currently declared; did you miss a closing brace?");

    // NB while parsing the call parameters, the parser may drop out of the current scope (macro or include file) before we get a chance to invoke the function,
    // in which case *fp will be destroyed, and an attempt made to drop the function. Therefore we copy *fp, and claim dibs on the function.
    // TODO - use smart pointers for this
    FUNCTION fn = *fp;
    FunctionCode *f = dynamic_cast<FunctionVM*>(sceneData->functionContextFactory)->GetFunctionAndReference(fn);

    unsigned int pmax = f->parameter_cnt - 1;
    unsigned int param = 0;
    DBL params[MAX_FUNCTION_PARAMETER_LIST];

    if(Parse_Call() == false)
    {
        // we claimed dibs on the function, so before we exit we must release it
        dynamic_cast<FunctionVM*>(sceneData->functionContextFactory)->RemoveFunction(fn);
        return;
    }

    // first store results in local array so recursive calls to
    // this function do not overwrite the global function stack
    // accessed by POVFPU_SetLocal [trf]
    for(param = 0; param < pmax; param++)
    {
        params[param] = Parse_Float();
        Parse_Comma();
    }
    params[param] = Parse_Float();

    GET (RIGHT_PAREN_TOKEN);

    for(param = 0; param < f->parameter_cnt; param++)
        fnVMContext->SetLocal(param + f->return_size, params[param]);

    (void)POVFPU_Run(fnVMContext, fn);

    // we claimed dibs on the function, so now that we're done with it we must say so
    dynamic_cast<FunctionVM*>(sceneData->functionContextFactory)->RemoveFunction(fn);

    for(param = 0; param < f->return_size; param++)
        Express[param] = fnVMContext->GetLocal(param);

    *Terms = f->return_size;
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   NOTE: Function Parse_RValue in parse.cpp depends on this function to be
*   able to only use Token.Data of the current token in order to have a kind
*   of two token look-ahead in Parse_RValue! [trf]
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Spline_Call(EXPRESS& Express, int *Terms)
{
    GenericSpline *spline = reinterpret_cast<GenericSpline *>(Token.Data);
    DBL Val;

    // NB while parsing the call parameters, the parser may drop out of the current scope (macro or include file)
    // before we get a chance to evaluate the spline, so we claim dibs on it.
    // TODO - use smart pointers for this
    Acquire_Spline_Reference(spline);

    if(Parse_Call() == false)
    {
        Release_Spline_Reference(spline);
        return;
    }

    Val=Parse_Float();
    Get_Token();

    if(Token.Token_Id == COMMA_TOKEN)
    {
        /*If there is a second parameter, make a copy of the spline
        with a new type and evaluate that.*/

        // we claimed dibs on the original spline, but since we've chosen to use a copy instead, we'll release the original
        Release_Spline_Reference(spline);

        Get_Token();
        switch(Token.Token_Id)
        {
            case LINEAR_SPLINE_TOKEN:
                spline = new LinearSpline(*spline);
                break;
            case QUADRATIC_SPLINE_TOKEN:
                spline = new QuadraticSpline(*spline);
                break;
            case CUBIC_SPLINE_TOKEN:
                spline = new CatmullRomSpline(*spline);
                break;
            case NATURAL_SPLINE_TOKEN:
                spline = new NaturalSpline(*spline);
                break;
            default:
                Error("linear_spline, quadratic_spline, natural_spline, or cubic_spline expected.");
                break;
        }

        GET(RIGHT_PAREN_TOKEN);
        Get_Spline_Val(spline, Val, Express, Terms);
        Destroy_Spline(spline);
        spline = NULL;
    }
    else
    {
        UNGET
        GET(RIGHT_PAREN_TOKEN);
        Get_Spline_Val(spline, Val, Express, Terms);

        // we claimed dibs on the spline, so now that we're done with it we must say so
        Release_Spline_Reference(spline);
    }
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Num_Factor (EXPRESS& Express,int *Terms)
{
    int i = 0;
    int l1,l2;
    DBL Val,Val2;
    Vector3d Vect,Vect2,Vect3;
    ObjectPtr Object;
    TRANSFORM Trans;
    TurbulenceWarp Turb;
    UCS2 *Local_String, *Local_String2;
    char *Local_C_String;
    UCS2String ign;
    IStream *f;
    POV_ARRAY *a;
    int Old_Ok=Ok_To_Declare;
    DBL greater_val, less_val, equal_val;
    PIGMENT *Pigment; // JN2007: Image map dimensions

    Ok_To_Declare=true;

    EXPECT
        CASE (FLOAT_FUNCT_TOKEN)
            /* All of these functions return a DBL result */
            switch(Token.Function_Id)
            {
                case ABS_TOKEN:
                    Val = Parse_Float_Param();
                    Val = fabs(Val);
                    break;

                case ACOS_TOKEN:
                    Val = Parse_Float_Param();
                    if ( Val > 1.0 )
                    {
                        Warning("Domain error in acos.");
                        Val = 1.0;
                    }
                    else if (Val < -1.0)
                    {
                        Warning("Domain error in acos.");
                        Val = -1.0;
                    }
                    Val = acos(Val);
                    break;

                case VAL_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    Local_C_String=Parse_C_String();
                    Val = atof(Local_C_String);
                    POV_FREE(Local_C_String);
                    GET (RIGHT_PAREN_TOKEN);
                    break;

                case ASC_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    Local_String=Parse_String();
                    Val = (DBL)Local_String[0];
                    POV_FREE(Local_String);
                    GET (RIGHT_PAREN_TOKEN);
                    break;

                case ASIN_TOKEN:
                    Val = Parse_Float_Param();
                    if ( Val > 1.0 )
                    {
                        Warning("Domain error in asin.");
                        Val = 1.0;
                    }
                    else if (Val < -1.0)
                    {
                        Warning("Domain error in asin.");
                        Val = -1.0;
                    }
                    Val = asin(Val);
                    break;

                case ATAN_TOKEN:
                    Val = atan(Parse_Float_Param());
                    break;

                case ATAN2_TOKEN:
                    Parse_Float_Param2(&Val,&Val2);
                    if (ftrue(Val) || ftrue(Val2))
                        Val = atan2(Val,Val2);
                    else
                        Error("Domain error in atan2!");
                    break;

                case COSH_TOKEN:
                    Val = cosh(Parse_Float_Param());
                    break;
                case SINH_TOKEN:
                    Val = sinh(Parse_Float_Param());
                    break;
                case TANH_TOKEN:
                    Val = tanh(Parse_Float_Param());
                    break;
                case ACOSH_TOKEN:
                    Val = acosh(Parse_Float_Param());
                    break;
                case ASINH_TOKEN:
                    Val = asinh(Parse_Float_Param());
                    break;
                case ATANH_TOKEN:
                    Val = atanh(Parse_Float_Param());
                    break;
                case CEIL_TOKEN:
                    Val = ceil(Parse_Float_Param());
                    break;

                case CLOCK_TOKEN:
                    Val = clockValue;
                    break;

                case CLOCK_ON_TOKEN:
                    Val=(DBL) (useClock);
                break;

                case COS_TOKEN:
                    Val = cos(Parse_Float_Param());
                    break;

                case DEFINED_TOKEN:
                    Val = Parse_Ifdef_Param();
                    break;

                case DEGREES_TOKEN:
                    Val = Parse_Float_Param()/M_PI_180;
                    break;

                case DIV_TOKEN:
                    Parse_Float_Param2(&Val,&Val2);
                    Val=(DBL) ( (int)(Val/Val2) );
                    break;

                case EXP_TOKEN:
                    Val = exp(Parse_Float_Param());
                    break;

                case FILE_EXISTS_TOKEN:
                    GET (LEFT_PAREN_TOKEN);

                    Local_C_String=Parse_C_String();

                    Val = ((f=Locate_File(UCS2String(ASCIItoUCS2String(Local_C_String)),POV_File_Text_User,ign,false))==NULL) ? 0.0 : 1.0;
                    if (f != NULL)
                        delete f;

                    POV_FREE(Local_C_String);

                    GET (RIGHT_PAREN_TOKEN);
                    break;

                case FLOAT_ID_TOKEN:
                    Val = *(reinterpret_cast<DBL *>(Token.Data));
                    break;

                case FLOAT_TOKEN:
                    Val = Token.Token_Float;
                    break;

                case FLOOR_TOKEN:
                    Val = floor(Parse_Float_Param());
                    break;

                case INT_TOKEN:
                    Val = (DBL) ((int) Parse_Float_Param());
                    break;

                case INSIDE_TOKEN:
                    Val = (DBL) Parse_Inside();
                    break;

                case LN_TOKEN:
                    Val = Parse_Float_Param();
                    if (Val<=0.0)
                        Error("ln of negative number %lf",Val);
                    else
                        Val = log(Val);
                    break;

                case LOG_TOKEN:
                    Val = Parse_Float_Param();
                    if (Val<=0.0)
                        Error("log of negative number %lf",Val);
                    else
                        Val = log10(Val);
                    break;

                case BITWISE_AND_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    l1 = (int)Parse_Float();
                    EXPECT
                        CASE(COMMA_TOKEN)
                            l2 = (int)Parse_Float();
                            l1 &= l2;
                        END_CASE

                        OTHERWISE
                            UNGET
                            GET (RIGHT_PAREN_TOKEN);
                            EXIT
                        END_CASE
                    END_EXPECT
                    Val = (DBL)l1;
                    break;

                case BITWISE_XOR_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    l1 = (int)Parse_Float();
                    EXPECT
                        CASE(COMMA_TOKEN)
                            l2 = (int)Parse_Float();
                            l1 ^= l2;
                        END_CASE

                        OTHERWISE
                            UNGET
                            GET (RIGHT_PAREN_TOKEN);
                            EXIT
                        END_CASE
                    END_EXPECT
                    Val = (DBL)l1;
                    break;

                case BITWISE_OR_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    l1 = (int)Parse_Float();
                    EXPECT
                        CASE(COMMA_TOKEN)
                            l2 = (int)Parse_Float();
                            l1 |= l2;
                        END_CASE

                        OTHERWISE
                            UNGET
                            GET (RIGHT_PAREN_TOKEN);
                            EXIT
                        END_CASE
                    END_EXPECT
                    Val = (DBL)l1;
                    break;

                case MAX_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    Val = Parse_Float();
                    EXPECT
                        CASE(COMMA_TOKEN)
                            Val2 = Parse_Float();
                            Val = max(Val,Val2);
                        END_CASE

                        OTHERWISE
                            UNGET
                            GET (RIGHT_PAREN_TOKEN);
                            EXIT
                        END_CASE
                    END_EXPECT
                    break;

                case MIN_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    Val = Parse_Float();
                    EXPECT
                        CASE(COMMA_TOKEN)
                            Val2 = Parse_Float();
                            Val = min(Val,Val2);
                        END_CASE

                        OTHERWISE
                            UNGET
                            GET (RIGHT_PAREN_TOKEN);
                            EXIT
                        END_CASE
                    END_EXPECT
                    break;

                case SELECT_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    Val = Parse_Float();
                    Parse_Comma();
                    less_val = Parse_Float();
                    Parse_Comma();
                    equal_val = Parse_Float();
                    EXPECT
                        CASE(COMMA_TOKEN)
                            greater_val = Parse_Float();
                            GET (RIGHT_PAREN_TOKEN);
                            if(Val < 0.0)
                                Val = less_val;
                            else if(Val == 0.0)
                                Val = equal_val;
                            else
                                Val = greater_val;
                            EXIT
                        END_CASE

                        OTHERWISE
                            UNGET
                            GET (RIGHT_PAREN_TOKEN);
                            if(Val < 0.0)
                                Val = less_val;
                            else
                                Val = equal_val;
                            EXIT
                        END_CASE
                    END_EXPECT
                    break;

                case MOD_TOKEN:
                    Parse_Float_Param2(&Val,&Val2);
                    Val = fmod(Val,Val2);
                    break;

                case PI_TOKEN:
                    Val = M_PI;
                    break;

                case TAU_TOKEN:
                    Val = M_TAU;
                    break;

                case SQR_TOKEN:
                    Val = Parse_Float_Param();
                    Val = (Val*Val);
                    break;

                case POW_TOKEN:
                    Parse_Float_Param2(&Val,&Val2);
                    if((Val == 0.0) && (Val2 == 0.0))
                        Error("Domain error.");
                    Val=pow(Val,Val2);
                    break;

                case RADIANS_TOKEN:
                    Val = Parse_Float_Param()*M_PI_180;
                    break;

                case SIN_TOKEN:
                    Val = sin(Parse_Float_Param());
                    break;

                case SQRT_TOKEN:
                    Val = Parse_Float_Param();
                    if (Val<0.0)
                        Error("sqrt of negative number %lf",Val);
                    else
                        Val = sqrt(Val);
                    break;

                case STRCMP_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    Local_String=Parse_String();
                    Parse_Comma();
                    Local_String2=Parse_String();
                    Val = (DBL)UCS2_strcmp(Local_String, Local_String2);
                    POV_FREE(Local_String);
                    POV_FREE(Local_String2);
                    GET (RIGHT_PAREN_TOKEN);
                    break;

                case STRLEN_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    Local_String=Parse_String();
                    Val = (DBL)UCS2_strlen(Local_String);
                    POV_FREE(Local_String);
                    GET (RIGHT_PAREN_TOKEN);
                    break;

                case TAN_TOKEN:
                    Val = tan(Parse_Float_Param());
                    break;

                case VDOT_TOKEN:
                    Parse_Vector_Param2(Vect,Vect2);
                    Val = dot(Vect,Vect2);
                    break;

                case VLENGTH_TOKEN:
                    Parse_Vector_Param(Vect);
                    Val = Vect.length();
                    break;

                case VERSION_TOKEN:
                    if (!parsingVersionDirective)
                    {
                        // Normally, the `version` pseudo-variable needs to return the effective language version
                        // (which now defaults to 3.6.2) so that include files can properly switch back after
                        // temporarily overriding the `#version` setting.
                        Val = sceneData->EffectiveLanguageVersion() / 100.0;
                    }
                    else
                    {
                        // When used inside a `#version` statement, special handling is needed to support the
                        // `#version version` idiom to set the effective language version to whatever version
                        // of POV-Ray is actually used.
                        Val = sceneData->languageVersion / 100.0;
                    }
                    break;

                case TRUE_TOKEN:
                case YES_TOKEN:
                case ON_TOKEN:
                    Val = 1.0;
                    break;

                case FALSE_TOKEN:
                case NO_TOKEN:
                case OFF_TOKEN:
                    Val = 0.0;
                    break;

                case SEED_TOKEN:
                    Val = stream_seed((int)Parse_Float_Param());
                    break;

                case RAND_TOKEN:
                    i = (int)Parse_Float_Param();
                    if ((i < 0) || (i >= Number_Of_Random_Generators))
                        Error("Illegal random number generator.");
                    Val = stream_rand(i);
                    break;

                case DIMENSIONS_TOKEN:
                    GET(LEFT_PAREN_TOKEN)
                    GET(ARRAY_ID_TOKEN)
                    a = reinterpret_cast<POV_ARRAY *>(*(Token.DataPtr));
                    Val = a->Dims+1;
                    GET(RIGHT_PAREN_TOKEN)
                    break;

                case DIMENSION_SIZE_TOKEN:
                    GET(LEFT_PAREN_TOKEN)
                    GET(ARRAY_ID_TOKEN)
                    Parse_Comma();
                    a = reinterpret_cast<POV_ARRAY *>(*(Token.DataPtr));
                    i = (int)Parse_Float()-1.0;
                    if ((i < 0) || (i > a->Dims))
                        Val = 0.0;
                    else
                        Val = a->Sizes[i];
                    GET(RIGHT_PAREN_TOKEN)
                    break;

                case NOW_TOKEN:
                    {
                        static boost::posix_time::ptime y2k(boost::gregorian::date(2000,1,1));
                        boost::posix_time::ptime now(boost::posix_time::microsec_clock::universal_time());
                        Val = (now-y2k).total_microseconds() * (1.0e-6) / (24*60*60);
                    }
                    break;
            }
            for (i=0; i < *Terms; i++)
                Express[i]=Val;
            EXIT
        END_CASE

        CASE (VECTOR_FUNCT_TOKEN)
            /* All of these functions return a VECTOR result */
            switch(Token.Function_Id)
            {
                case VAXIS_ROTATE_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    Parse_Vector(Vect2);
                    Parse_Comma();
                    Parse_Vector(Vect3);
                    Parse_Comma();
                    Val=Parse_Float()*M_PI_180;
                    GET (RIGHT_PAREN_TOKEN);
                    Compute_Axis_Rotation_Transform(&Trans,Vect3,Val);
                    MTransPoint(Vect, Vect2, &Trans);
                    break;

                case VCROSS_TOKEN:
                    Parse_Vector_Param2(Vect2,Vect3);
                    Vect = cross(Vect2,Vect3);
                    break;

                case VECTOR_ID_TOKEN:
                    Vect = *reinterpret_cast<Vector3d *>(Token.Data);
                    break;

                case VNORMALIZE_TOKEN:
                    Parse_Vector_Param(Vect2);
                    if((Vect2[X] == 0.0) && (Vect2[Y] == 0.0) && (Vect2[Z] == 0.0))
                    {
                        if (sceneData->EffectiveLanguageVersion() >= 350)
                            PossibleError("Normalizing zero-length vector.");
                        Vect[X] = Vect[Y] = Vect[Z] = 0.0;
                    }
                    else
                        Vect = Vect2.normalized();
                    break;

                case VROTATE_TOKEN:
                    Parse_Vector_Param2(Vect2,Vect3);
                    Compute_Rotation_Transform (&Trans, Vect3);
                    MTransPoint(Vect, Vect2, &Trans);
                    break;

                case VTURBULENCE_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    Turb.Lambda = Parse_Float();
                    Parse_Comma();
                    Turb.Omega = Parse_Float();
                    Parse_Comma();
                    Turb.Octaves = (int)Parse_Float();
                    if(Turb.Octaves < 1)
                        Turb.Octaves = 1;
                    if(Turb.Octaves > 10) // avoid domain errors
                        Turb.Octaves = 10;
                    Parse_Comma();
                    Parse_Vector(Vect2); // input vector
                    Parse_Comma();
                    GET (RIGHT_PAREN_TOKEN);
                    DTurbulence(Vect, Vect2, &Turb);
                    break;

                case X_TOKEN:
                    Vect = Vector3d(1.0,0.0,0.0);
                    break;

                case Y_TOKEN:
                    Vect = Vector3d(0.0,1.0,0.0);
                    break;

                case Z_TOKEN:
                    Vect = Vector3d(0.0,0.0,1.0);
                    break;

                case TRACE_TOKEN:
                    Parse_Trace( Vect );
                    break;

                case MIN_EXTENT_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    EXPECT
                        CASE (OBJECT_ID_TOKEN)
                            Object = reinterpret_cast<ObjectPtr>(Token.Data);
                            if ( Object )
                                Vect = Vector3d(Object->BBox.lowerLeft);
                            EXIT
                        END_CASE

                        OTHERWISE
                            Object = NULL;
                            Vect = Vector3d(0.0,0.0,0.0);
                            UNGET
                            EXIT
                        END_CASE
                    END_EXPECT

                    GET (RIGHT_PAREN_TOKEN);
                    break;

                case MAX_EXTENT_TOKEN:
                    GET (LEFT_PAREN_TOKEN);
                    EXPECT
                        CASE (OBJECT_ID_TOKEN)
                            Object = reinterpret_cast<ObjectPtr>(Token.Data);
                            if ( Object )
                                Vect = Vector3d(Object->BBox.lowerLeft+Object->BBox.size);
                            EXIT
                        END_CASE

                        // JN2007: Image map dimensions:
                        CASE3 (DENSITY_ID_TOKEN,PIGMENT_ID_TOKEN,TNORMAL_ID_TOKEN)
                            Pigment = reinterpret_cast<PIGMENT *>(Token.Data);
                            if(Pigment->Type == BITMAP_PATTERN)
                            {
                                Vect[X] = dynamic_cast<ImagePattern*>(Pigment->pattern.get())->pImage->iwidth;
                                Vect[Y] = dynamic_cast<ImagePattern*>(Pigment->pattern.get())->pImage->iheight;
                                Vect[Z] = 0;
                            }
                            else if(Pigment->Type == DENSITY_FILE_PATTERN)
                            {
                                Vect[X] = dynamic_cast<DensityFilePattern*>(Pigment->pattern.get())->densityFile->Data->Sx;
                                Vect[Y] = dynamic_cast<DensityFilePattern*>(Pigment->pattern.get())->densityFile->Data->Sy;
                                Vect[Z] = dynamic_cast<DensityFilePattern*>(Pigment->pattern.get())->densityFile->Data->Sz;
                            }
                            else
                            {
                                Error("A pigment, normal or density parameter to max_extent must be based on an image or density file.");
                            }
                            EXIT
                        END_CASE

                        OTHERWISE
                            Object = NULL;
                            Vect = Vector3d(0.0,0.0,0.0);
                            UNGET
                            EXIT
                        END_CASE
                    END_EXPECT
                    GET (RIGHT_PAREN_TOKEN);
                    break;
            }

            *Terms = 3;
            for(i = 0; i < 3; i++)
                Express[i] = Vect[i];
            EXIT
        END_CASE

        CASE (FUNCT_ID_TOKEN)
            Val = Parse_Function_Call();
            for(i = 0; i < *Terms; i++)
                Express[i] = Val;
            EXIT
        END_CASE

        CASE (VECTFUNCT_ID_TOKEN)
            *Terms = 5; // will be adjusted by Parse_Vector_Function_Call
            for(i = 0; i < *Terms; i++)
                Express[i] = 0.0;
            Parse_Vector_Function_Call(Express, Terms);
            EXIT
        END_CASE

        CASE (SPLINE_ID_TOKEN)
            *Terms = 5; // will be adjusted by Parse_Spline_Call
            for(i = 0; i < *Terms; i++)
                Express[i] = 0.0;
            Parse_Spline_Call(Express, Terms);
            EXIT
        END_CASE

        CASE (COLOUR_ID_TOKEN)
            *Terms=5;
            (*reinterpret_cast<RGBFTColour *>(Token.Data)).Get(Express, *Terms);
            EXIT
        END_CASE

        CASE (UV_ID_TOKEN)
            *Terms=2;
            for (i=0; i<2; i++)
                Express[i]=(DBL)(  (*reinterpret_cast<Vector2d *>(Token.Data))[i]  );
            EXIT
        END_CASE

        CASE (VECTOR_4D_ID_TOKEN)
            *Terms=4;
            for (i=0; i<4; i++)
                Express[i]=(DBL)(  (reinterpret_cast<DBL *>(Token.Data))[i]  );
            EXIT
        END_CASE

        CASE (T_TOKEN)
            *Terms=4;
            Express[0]=0.0;
            Express[1]=0.0;
            Express[2]=0.0;
            Express[3]=1.0;
            EXIT
        END_CASE

        CASE (U_TOKEN)
            *Terms=2;
            Express[0]=1.0;
            Express[1]=0.0;
            EXIT
        END_CASE

        CASE (V_TOKEN)
            *Terms=2;
            Express[0]=0.0;
            Express[1]=1.0;
            EXIT
        END_CASE

        CASE (PLUS_TOKEN)
        END_CASE

        CASE (DASH_TOKEN)
            Ok_To_Declare=Old_Ok;
            Parse_Num_Factor(Express,Terms);
            Old_Ok=Ok_To_Declare;
            Ok_To_Declare=true;
            for (i=0; i<*Terms; i++)
                Express[i]=-Express[i];
            EXIT
        END_CASE

        CASE (EXCLAMATION_TOKEN)
            Ok_To_Declare=Old_Ok;
            Parse_Num_Factor(Express,Terms);
            Old_Ok=Ok_To_Declare;
            Ok_To_Declare=true;
            for (i=0; i<*Terms; i++)
                Express[i] = ftrue(Express[i])?0.0:1.0;
            EXIT
        END_CASE

        CASE (LEFT_PAREN_TOKEN)
            Parse_Express(Express,Terms);
            GET(RIGHT_PAREN_TOKEN);
            EXIT
        END_CASE

/* This case parses a 2, 3, 4, or 5 term vector.  First parse 2 terms.
   Note Parse_Comma won't crash if it doesn't find one.
 */

        CASE (LEFT_ANGLE_TOKEN)
            Express[X] = Parse_Float();   Parse_Comma();
            Express[Y] = Parse_Float();   Parse_Comma();
            *Terms=2;

            EXPECT
                CASE_EXPRESS
                    /* If a 3th float is found, parse it. */
                    Express[2] = Parse_Float(); Parse_Comma();
                    *Terms=3;
                    EXPECT
                        CASE_EXPRESS
                            /* If a 4th float is found, parse it. */
                            Express[3] = Parse_Float(); Parse_Comma();
                            *Terms=4;
                            EXPECT
                                CASE_EXPRESS
                                    /* If a 5th float is found, parse it. */
                                    Express[4] = Parse_Float();
                                    *Terms=5;
                                END_CASE

                                OTHERWISE
                                    /* Only 4 found. */
                                    UNGET
                                    GET (RIGHT_ANGLE_TOKEN)
                                    EXIT
                                END_CASE
                            END_EXPECT
                            EXIT
                        END_CASE

                        OTHERWISE
                            /* Only 3 found. */
                            UNGET
                            GET (RIGHT_ANGLE_TOKEN)
                            EXIT
                        END_CASE
                    END_EXPECT
                    EXIT
                END_CASE

                OTHERWISE
                    /* Only 2 found. */
                    UNGET
                    GET (RIGHT_ANGLE_TOKEN)
                    EXIT
                END_CASE
            END_EXPECT
            EXIT
        END_CASE

        OTHERWISE
            Expectation_Error ("numeric expression");
        END_CASE
    END_EXPECT

    Ok_To_Declare=Old_Ok;

    /* Parse VECTOR.x or COLOR.red type things */
    EXPECT
        CASE(PERIOD_TOKEN)
            EXPECT
                CASE (VECTOR_FUNCT_TOKEN)
                    switch(Token.Function_Id)
                    {
                        case X_TOKEN:
                            i=X;
                            break;

                        case Y_TOKEN:
                            i=Y;
                            break;

                        case Z_TOKEN:
                            i=Z;
                            break;

                        default:
                            Expectation_Error ("x, y, or z");
                    }
                    EXIT
                END_CASE

                CASE (COLOUR_KEY_TOKEN)
                    switch(Token.Function_Id)
                    {
                        case RED_TOKEN:
                            i=pRED;
                            break;

                        case GREEN_TOKEN:
                            i=pGREEN;
                            break;

                        case BLUE_TOKEN:
                            i=pBLUE;
                            break;

                        case FILTER_TOKEN:
                            i=pFILTER;
                            break;

                        case TRANSMIT_TOKEN:
                            i=pTRANSM;
                            break;

                        case GRAY_TOKEN:
                            *Terms=1;
                            Express[0]=PreciseRGBFTColour(Express).Greyscale();
                            return;

                        default:
                            Expectation_Error ("red, green, blue, filter, transmit, gray or vector component");
                    }
                    EXIT
                END_CASE

                CASE(U_TOKEN)
                    i=U;
                    EXIT
                END_CASE

                CASE(V_TOKEN)
                    i=V;
                    EXIT
                END_CASE

                CASE(T_TOKEN)
                    i=T;
                    EXIT
                END_CASE

                OTHERWISE
                    Expectation_Error ("x, y, z, u, v, t or color component");
                END_CASE
            END_EXPECT

            if (i>=*Terms)
                Error("Bad operands for period operator.");
            *Terms=1;
            Express[0]=Express[i];
            EXIT
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

/* If first operand of a 2-operand function had more terms than the second,
   then the parsing of the 2nd operand would have automatically promoted it.
   But if 2nd operand has more terms then we must go back promote the 1st
   operand before combining them  Promote_Express does it.  If Old_Terms=1
   then set all terms to Express[0].  Otherwise pad extra terms with 0.0.
*/

void Parser::Promote_Express(EXPRESS& Express,int *Old_Terms,int New_Terms)
{
    register int i;

    if (*Old_Terms >= New_Terms)
        return;

    if (*Old_Terms==1)
    {
        for(i=1;i<New_Terms;i++)
        {
            Express[i]=Express[0];
        }
    }
    else
    {
        for(i=(*Old_Terms);i<New_Terms;i++)
        {
            Express[i]=0.0;
        }
    }

    *Old_Terms=New_Terms;
}




/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
*    2000 : NK promotion bugfix
*
******************************************************************************/

void Parser::Parse_Num_Term (EXPRESS& Express,int *Terms)
{
    register int i;
    EXPRESS Local_Express;
    int Local_Terms;

    Parse_Num_Factor(Express,Terms);

    Local_Terms=*Terms;

    EXPECT
        CASE (STAR_TOKEN)
            Parse_Num_Factor(Local_Express,&Local_Terms);
            if (Local_Terms>*Terms)
                Promote_Express(Express,Terms,Local_Terms);
            else
                Promote_Express(Local_Express,&Local_Terms,*Terms);

            for(i=0;i<*Terms;i++)
                Express[i] *= Local_Express[i];
        END_CASE

        CASE (SLASH_TOKEN)
            Parse_Num_Factor(Local_Express,&Local_Terms);
            if (Local_Terms>*Terms)
                Promote_Express(Express,Terms,Local_Terms);
            else
                Promote_Express(Local_Express,&Local_Terms,*Terms);

            for(i=0;i<*Terms;i++)
            {
                if (Local_Express[i]==0.0) /* must be 0.0, not EPSILON */
                {
                    Express[i]=HUGE_VAL;
                    Warning("Divide by zero.");
                }
                else
                {
                    Express[i] /= Local_Express[i];
                }
            }
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
*    2000 :  NK promotion bugfix
*
******************************************************************************/

void Parser::Parse_Rel_Factor (EXPRESS& Express,int *Terms)
{
    register int i;
    EXPRESS Local_Express;
    int Local_Terms;

    Parse_Num_Term(Express,Terms);

    Local_Terms=*Terms;

    EXPECT
        CASE (PLUS_TOKEN)
            Parse_Num_Term(Local_Express,&Local_Terms);
            if (Local_Terms>*Terms)
            {
                Promote_Express(Express,Terms,Local_Terms);
            }
            else
            {
                Promote_Express(Local_Express,&Local_Terms,*Terms);
            }

            for(i=0;i<*Terms;i++)
                Express[i] += Local_Express[i];
        END_CASE

        CASE (DASH_TOKEN)
            Parse_Num_Term(Local_Express,&Local_Terms);
            if (Local_Terms>*Terms)
            {
                Promote_Express(Express,Terms,Local_Terms);
            }
            else
            {
                Promote_Express(Local_Express,&Local_Terms,*Terms);
            }

            for(i=0;i<*Terms;i++)
                Express[i] -= Local_Express[i];
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Rel_String_Term (const UCS2 *lhs, EXPRESS& Express, int Terms)
{
    int Val, i;
    UCS2 *rhs = NULL;

    EXPECT_ONE
        CASE (LEFT_ANGLE_TOKEN)
            rhs = Parse_String();
            Val = UCS2_strcmp(lhs, rhs);
            POV_FREE(rhs);

            for(i=0;i<Terms;i++)
                Express[i] = (DBL)(Val < 0);
        END_CASE

        CASE (REL_LE_TOKEN)
            rhs = Parse_String();
            Val = UCS2_strcmp(lhs, rhs);
            POV_FREE(rhs);

            for(i=0;i<Terms;i++)
                Express[i] = (DBL)(Val <= 0);
        END_CASE

        CASE (EQUALS_TOKEN)
            rhs = Parse_String();
            Val = UCS2_strcmp(lhs, rhs);
            POV_FREE(rhs);

            for(i=0;i<Terms;i++)
                Express[i] = (DBL)(Val == 0);
        END_CASE

        CASE (REL_NE_TOKEN)
            rhs = Parse_String();
            Val = UCS2_strcmp(lhs, rhs);
            POV_FREE(rhs);

            for(i=0;i<Terms;i++)
                Express[i] = (DBL)(Val != 0);
        END_CASE

        CASE (REL_GE_TOKEN)
            rhs = Parse_String();
            Val = UCS2_strcmp(lhs, rhs);
            POV_FREE(rhs);

            for(i=0;i<Terms;i++)
                Express[i] = (DBL)(Val >= 0);
        END_CASE

        CASE (RIGHT_ANGLE_TOKEN)
            rhs = Parse_String();
            Val = UCS2_strcmp(lhs, rhs);
            POV_FREE(rhs);

            for(i=0;i<Terms;i++)
                Express[i] = (DBL)(Val > 0);
        END_CASE

        OTHERWISE
            Expectation_Error("string comparison operator");
            EXIT
        END_CASE
    END_EXPECT
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Rel_Term (EXPRESS& Express,int *Terms)
{
    register int i;
    EXPRESS Local_Express;
    int Local_Terms;

    bool old_Ok_To_Declare = Ok_To_Declare;
    Ok_To_Declare=true;

    UCS2 *Local_String = Parse_String(false, false);
    if(Local_String != NULL)
    {
            Parse_Rel_String_Term(Local_String, Express, *Terms);
            POV_FREE(Local_String);
            Ok_To_Declare = old_Ok_To_Declare;
            return;
    }
    Ok_To_Declare = old_Ok_To_Declare;

    Parse_Rel_Factor(Express,Terms);

    Local_Terms=*Terms;

    EXPECT
        CASE (LEFT_ANGLE_TOKEN)
            Parse_Rel_Factor(Local_Express,&Local_Terms);
            Promote_Express(Express,Terms,Local_Terms);

            for(i=0;i<*Terms;i++)
                Express[i] = (DBL)(Express[i] < Local_Express[i]);
        END_CASE

        CASE (REL_LE_TOKEN)
            Parse_Rel_Factor(Local_Express,&Local_Terms);
            Promote_Express(Express,Terms,Local_Terms);

            for(i=0;i<*Terms;i++)
                Express[i] = (DBL)((Express[i] <= Local_Express[i]) || (!ftrue(Express[i]-Local_Express[i])));
        END_CASE

        CASE (EQUALS_TOKEN)
            Parse_Rel_Factor(Local_Express,&Local_Terms);
            Promote_Express(Express,Terms,Local_Terms);

            for(i=0;i<*Terms;i++)
                Express[i] = (DBL)(!ftrue(Express[i]-Local_Express[i]));
        END_CASE

        CASE (REL_NE_TOKEN)
            Parse_Rel_Factor(Local_Express,&Local_Terms);
            Promote_Express(Express,Terms,Local_Terms);

            for(i=0;i<*Terms;i++)
                Express[i] = (DBL)ftrue(Express[i]-Local_Express[i]);
        END_CASE

        CASE (REL_GE_TOKEN)
            Parse_Rel_Factor(Local_Express,&Local_Terms);
            Promote_Express(Express,Terms,Local_Terms);

            for(i=0;i<*Terms;i++)
                Express[i] = (DBL)((Express[i] >= Local_Express[i]) || (!ftrue(Express[i]-Local_Express[i])));
        END_CASE

        CASE (RIGHT_ANGLE_TOKEN)
            Parse_Rel_Factor(Local_Express,&Local_Terms);
            Promote_Express(Express,Terms,Local_Terms);

            for(i=0;i<*Terms;i++)
                Express[i] = (DBL)(Express[i] > Local_Express[i]);
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Logical (EXPRESS& Express,int *Terms)
{
    register int i;
    EXPRESS Local_Express;
    int Local_Terms;

    Parse_Rel_Term(Express,Terms);

    Local_Terms=*Terms;

    EXPECT
        CASE (AMPERSAND_TOKEN)
            Parse_Rel_Term(Local_Express,&Local_Terms);
            Promote_Express(Express,Terms,Local_Terms);

            for(i=0;i<*Terms;i++)
                Express[i] = (DBL)(ftrue(Express[i]) && ftrue(Local_Express[i]));
        END_CASE

        CASE (BAR_TOKEN)
            Parse_Rel_Term(Local_Express,&Local_Terms);
            Promote_Express(Express,Terms,Local_Terms);

            for(i=0;i<*Terms;i++)
                Express[i] = (DBL)(ftrue(Express[i]) || ftrue(Local_Express[i]));
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Express (EXPRESS& Express,int *Terms)
{
    EXPRESS Local_Express1, Local_Express2;
    EXPRESS *Chosen;
    int Local_Terms1, Local_Terms2;

    Local_Terms1 = 1;

    Parse_Logical(Express,&Local_Terms1);

    EXPECT
        CASE (QUESTION_TOKEN)
            if (Local_Terms1 != 1)
                Error("Conditional must evaluate to a float.");
            Local_Terms1 = Local_Terms2 = *Terms;
            Parse_Express(Local_Express1,&Local_Terms1);
            GET(COLON_TOKEN);
            Parse_Express(Local_Express2,&Local_Terms2);
            if (ftrue(Express[0]))
            {
                Chosen = reinterpret_cast<EXPRESS *>(&Local_Express1);
                *Terms = Local_Terms1;
            }
            else
            {
                Chosen = reinterpret_cast<EXPRESS *>(&Local_Express2);
                *Terms = Local_Terms2;
            }
            POV_MEMCPY(Express,Chosen,sizeof(EXPRESS));
            EXIT
        END_CASE

        OTHERWISE
            /* Not a (c)?a:b expression.  Since Express was parsed with
               Local_Terms1=1 then we may have to promote this.  Suppose
               Terms=3 but Local_Terms1=1.  If this had been a (c)?a:b
               then a float is ok but since it is not a condition then
               it must be promoted to Terms=3.  Note that the parameters
               below look wrong but they are not.
             */
            Promote_Express (Express,&Local_Terms1,*Terms);
            /* On the other hand, Local_Terms1 may be bigger than Terms.
               If so, Express already is promoted and Terms must reflect that.
             */
            *Terms=Local_Terms1;
            UNGET
            EXIT
        END_CASE
    END_EXPECT

}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Parser::Parse_Float ()
{
    EXPRESS Express;
    int Terms;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;

    Terms=1;

    if (sceneData->EffectiveLanguageVersion() < 150)
        Parse_Num_Factor(Express,&Terms);
    else
        Parse_Rel_Factor(Express,&Terms);

    if (Terms>1)
        Error ("Float expected but vector or color expression found.");

    Allow_Identifier_In_Call = old_allow_id;

    return (Express[0]);
}

int Parser::Parse_Int(const char* parameterName)
{
    DBL rawValue = Parse_Float();
    int value = int(rawValue); // TODO - Maybe we want round-to-nearest here.
    if (fabs(value - rawValue) >= EPSILON)
    {
        Warning("%s%sExpected integer; rounding down fractional value %lf to %i.",
                (parameterName != NULL ? parameterName : ""),
                (parameterName != NULL ? ": " : ""),
                rawValue,
                value);
    }
    return value;
}

int Parser::Parse_Int_With_Minimum(int minValue, const char* parameterName)
{
    int value = Parse_Int(parameterName);
    if (value < minValue)
    {
        Error("%s%sExpected at least %i, but found %i instead.",
              (parameterName != NULL ? parameterName : ""),
              (parameterName != NULL ? ": " : ""),
              minValue,
              value);
    }
    return value;
}

int Parser::Parse_Int_With_Range(int minValue, int maxValue, const char* parameterName)
{
    int value = Parse_Int(parameterName);
    if ((value < minValue) || (value > maxValue))
    {
        Error("%s%sExpected at %s %i, but found %i instead.",
              (parameterName != NULL ? parameterName : ""),
              (parameterName != NULL ? ": " : ""),
              (value < minValue ? "least" : "most"),
              minValue,
              value);
    }
    return value;
}

bool Parser::Parse_Bool(const char* parameterName)
{
    DBL rawValue = Parse_Float();
    int intValue = int(rawValue);
    bool value = (intValue != 0);
    if (fabs(intValue - rawValue) >= EPSILON)
    {
        Warning("%s%sExpected boolean; interpreting fractional value %lf as '%s'.",
                (parameterName != NULL ? parameterName : ""),
                (parameterName != NULL ? ": " : ""),
                rawValue,
                (value ? "on" : "off"));
    }
    return value;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Parser::Allow_Float (DBL defval)
{
    DBL retval;

    EXPECT
        CASE_EXPRESS
            retval = Parse_Float();
            EXIT
        END_CASE

        OTHERWISE
            UNGET
            retval = defval;
            EXIT
        END_CASE
    END_EXPECT

    return (retval);
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

int Parser::Allow_Vector (Vector3d& Vect)
{
    int retval;

    EXPECT
        CASE_EXPRESS
            Parse_Vector(Vect);
            retval = true;
            EXIT
        END_CASE

        OTHERWISE
            UNGET
            retval = false;
            EXIT
        END_CASE
    END_EXPECT

    return (retval);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Vector (Vector3d& Vector)
{
    EXPRESS Express;
    int Terms;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;

    /* Initialize expression. [DB 12/94] */

    for (Terms = 0; Terms < 5; Terms++)
    {
        Express[Terms] = 0.0;
    }

    Terms=3;

    if (sceneData->EffectiveLanguageVersion() < 150)
        Parse_Num_Factor(Express,&Terms);
    else
        Parse_Rel_Factor(Express,&Terms);

    if (Terms>3)
        Error ("Vector expected but color expression found.");

    for(Terms=0;Terms<3;Terms++)
        Vector[Terms]=Express[Terms];

    Allow_Identifier_In_Call = old_allow_id;
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Vector4D (VECTOR_4D Vector)
{
    EXPRESS Express;
    int Terms;
    int Dim = 4;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;

    /* Initialize expression. [DB 12/94] */

    for (Terms = 0; Terms < 5; Terms++)
    {
        Express[Terms] = 0.0;
    }

    Terms=Dim;

    if (sceneData->EffectiveLanguageVersion() < 150)
        Parse_Num_Factor(Express,&Terms);
    else
        Parse_Rel_Factor(Express,&Terms);

    if (Terms>Dim)
        Error ("Vector expected but color expression found.");

    for(Terms=0;Terms<Dim;Terms++)
        Vector[Terms]=Express[Terms];

    Allow_Identifier_In_Call = old_allow_id;
}




/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_UV_Vect (Vector2d& UV_Vect)
{
    EXPRESS Express;
    int Terms;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;

    /* Initialize expression. [DB 12/94] */

    for (Terms = 0; Terms < 5; Terms++)
    {
        Express[Terms] = 0.0;
    }

    Terms=2;

    if (sceneData->EffectiveLanguageVersion() < 150)
        Parse_Num_Factor(Express,&Terms);
    else
        Parse_Rel_Factor(Express,&Terms);

    if (Terms>2)
        Error ("UV_Vector expected but vector or color expression found.");

    for(Terms=0;Terms<2;Terms++)
        UV_Vect[Terms]=Express[Terms];

    Allow_Identifier_In_Call = old_allow_id;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

int Parser::Parse_Unknown_Vector(EXPRESS& Express, bool allow_identifier, bool *had_identifier)
{
    int Terms;
    bool old_allow_id = Allow_Identifier_In_Call;

    Allow_Identifier_In_Call = allow_identifier;
    Identifier_In_Call = false;

    /* Initialize expression. [DB 12/94] */

    for (Terms = 0; Terms < 5; Terms++)
    {
        Express[Terms] = 0.0;
    }

    Terms=1;

    if (sceneData->EffectiveLanguageVersion() < 150)
        Parse_Num_Factor(Express,&Terms);
    else
        Parse_Rel_Factor(Express,&Terms);

    if(had_identifier != NULL)
        *had_identifier = Identifier_In_Call;

    Allow_Identifier_In_Call = old_allow_id;

    return(Terms);
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Scale_Vector (Vector3d& Vector)
{
    Parse_Vector(Vector);

    if (Vector[X] == 0.0)
    {
        Vector[X] = 1.0;
        Warning("Illegal Value: Scale X by 0.0. Changed to 1.0.");
    }
    if (Vector[Y] == 0.0)
    {
        Vector[Y] = 1.0;
        Warning("Illegal Value: Scale Y by 0.0. Changed to 1.0.");
    }
    if (Vector[Z] == 0.0)
    {
        Vector[Z] = 1.0;
        Warning("Illegal Value: Scale Z by 0.0. Changed to 1.0.");
    }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::Parse_Colour (RGBFTColour& colour, bool expectFT)
{
    EXPRESS Express;
    int Terms;
    bool old_allow_id = Allow_Identifier_In_Call, sawFloatOrFloatFnct;
    Allow_Identifier_In_Call = false;

    /* Initialize expression. [DB 12/94] */

    for (Terms = 0; Terms < 5; Terms++)
    {
        Express[Terms] = 0.0;
    }

    colour.Clear();

    bool startedParsing = false;

    ALLOW(COLOUR_TOKEN)

    EXPECT
        CASE (COLOUR_KEY_TOKEN)
            switch(Token.Function_Id)
            {
                case ALPHA_TOKEN:
                    VersionWarning(155, "Keyword ALPHA discontinued. Use FILTER instead.");
                    // FALLTHROUGH
                case FILTER_TOKEN:
                    colour.filter() = (ColourChannel)Parse_Float();
                    if (!expectFT && (colour.filter() != 0))
                        Warning("Expected pure RGB color expression, unexpected filter component will have no effect.");
                    break;

                case BLUE_TOKEN:
                    colour.blue() = (ColourChannel)Parse_Float();
                    break;

                case GREEN_TOKEN:
                    colour.green() = (ColourChannel)Parse_Float();
                    break;

                case RED_TOKEN:
                    colour.red() = (ColourChannel)Parse_Float();
                    break;

                case TRANSMIT_TOKEN:
                    colour.transm() = (ColourChannel)Parse_Float();
                    if (!expectFT && (colour.transm() != 0))
                        Warning("Expected pure RGB color expression, unexpected transmit component will have no effect.");
                    break;

                case RGB_TOKEN:
                    if(startedParsing)
                    {
                        UNGET
                        EXIT
                    }
                    else
                    {
                        Terms=3;
                        Parse_Express(Express,&Terms);
                        if (Terms != 3)
                            Warning("Suspicious expression after rgb.");
                        colour.Set(Express, Terms);
                    }
                    break;

                case RGBF_TOKEN:
                    if(startedParsing)
                    {
                        UNGET
                        EXIT
                    }
                    else
                    {
                        Terms=4;
                        Parse_Express(Express,&Terms);
                        if (Terms != 4)
                            Warning("Suspicious expression after rgbf.");
                        colour.Set(Express, Terms);
                        if (!expectFT && (colour.filter() != 0))
                            Warning("Expected pure RGB color expression, unexpected filter component will have no effect.");
                    }
                    break;

                case RGBT_TOKEN:
                    if(startedParsing)
                    {
                        UNGET
                        EXIT
                    }
                    else
                    {
                        Terms=4;
                        Parse_Express(Express,&Terms);
                        if (Terms != 4)
                            Warning("Suspicious expression after rgbt.");
                        colour.Set(Express, Terms);
                        colour.transm()=colour.filter();
                        colour.filter()=0.0;
                        if (!expectFT && (colour.transm() != 0))
                            Warning("Expected pure RGB color expression, unexpected transmit component will have no effect.");
                    }
                    break;

                case RGBFT_TOKEN:
                    if(startedParsing)
                    {
                        UNGET
                        EXIT
                    }
                    else
                    {
                        Terms=5;
                        Parse_Express(Express,&Terms);
                        if (Terms != 5)
                            Warning("Suspicious expression after rgbft.");
                        colour.Set(Express, Terms);
                        if (!expectFT && ((colour.filter() != 0) || (colour.transm() != 0)))
                            Warning("Expected pure RGB color expression, unexpected filter and transmit components will have no effect.");
                    }
                    break;

#if 0 // sred, sgreen and sblue tokens not enabled at present
                case SBLUE_TOKEN:
                    if (!sceneData->workingGammaToSRGB)
                        Error("Cannot parse sRGB colors before assumed_gamma has been set.");
                    colour.blue() = sceneData->workingGammaToSRGB->Decode((ColourChannel)Parse_Float());
                    break;

                case SGREEN_TOKEN:
                    if (!sceneData->workingGammaToSRGB)
                        Error("Cannot parse sRGB colors before assumed_gamma has been set.");
                    colour.green() = sceneData->workingGammaToSRGB->Decode((ColourChannel)Parse_Float());
                    break;

                case SRED_TOKEN:
                    if (!sceneData->workingGammaToSRGB)
                        Error("Cannot parse sRGB colors before assumed_gamma has been set.");
                    colour.red() = sceneData->workingGammaToSRGB->Decode((ColourChannel)Parse_Float());
                    break;
#endif

                case SRGB_TOKEN:
                    if(startedParsing)
                    {
                        UNGET
                        EXIT
                    }
                    else
                    {
                        if (!sceneData->workingGammaToSRGB)
                            Error("Cannot parse sRGB colors before assumed_gamma has been set.");
                        Terms=3;
                        Parse_Express(Express,&Terms);
                        if (Terms != 3)
                            Warning("Suspicious expression after srgb.");
                        colour.Set(Express, Terms);
                        colour.rgb() = GammaCurve::Decode(sceneData->workingGammaToSRGB, colour.rgb());
                    }
                    break;

                case SRGBF_TOKEN:
                    if(startedParsing)
                    {
                        UNGET
                        EXIT
                    }
                    else
                    {
                        if (!sceneData->workingGammaToSRGB)
                            Error("Cannot parse sRGB colors before assumed_gamma has been set.");
                        Terms=4;
                        Parse_Express(Express,&Terms);
                        if (Terms != 4)
                            Warning("Suspicious expression after srgbf.");
                        colour.Set(Express, Terms);
                        colour.rgb() = GammaCurve::Decode(sceneData->workingGammaToSRGB, colour.rgb());
                        if (!expectFT && (colour.filter() != 0))
                            Warning("Expected pure RGB color expression, unexpected filter component will have no effect.");
                    }
                    break;

                case SRGBT_TOKEN:
                    if(startedParsing)
                    {
                        UNGET
                        EXIT
                    }
                    else
                    {
                        if (!sceneData->workingGammaToSRGB)
                            Error("Cannot parse sRGB colors before assumed_gamma has been set.");
                        Terms=4;
                        Parse_Express(Express,&Terms);
                        if (Terms != 4)
                            Warning("Suspicious expression after srgbt.");
                        colour.Set(Express, Terms);
                        colour.transm()=colour.filter();
                        colour.filter()=0.0;
                        colour.rgb() = GammaCurve::Decode(sceneData->workingGammaToSRGB, colour.rgb());
                        if (!expectFT && (colour.transm() != 0))
                            Warning("Expected pure RGB color expression, unexpected transmit component will have no effect.");
                    }
                    break;

                case SRGBFT_TOKEN:
                    if(startedParsing)
                    {
                        UNGET
                        EXIT
                    }
                    else
                    {
                        if (!sceneData->workingGammaToSRGB)
                            Error("Cannot parse sRGB colors before assumed_gamma has been set.");
                        Terms=5;
                        Parse_Express(Express,&Terms);
                        if (Terms != 5)
                            Warning("Suspicious expression after srgbft.");
                        colour.Set(Express, Terms);
                        colour.rgb() = GammaCurve::Decode(sceneData->workingGammaToSRGB, colour.rgb());
                        if (!expectFT && ((colour.filter() != 0) || (colour.transm() != 0)))
                            Warning("Expected pure RGB color expression, unexpected filter and transmit components will have no effect.");
                    }
                    break;
            }
            startedParsing = true;
        END_CASE

        CASE (COLOUR_ID_TOKEN)
            UNGET
            if (startedParsing)
            {
                EXIT
            }
            else
            {
                if (expectFT)
                    Terms = 5;
                else
                    Terms = 3;
                Parse_Express(Express,&Terms);
                colour.Set(Express, Terms);
                if (!expectFT && ((colour.filter() != 0) || (colour.transm() != 0)))
                    Warning("Expected pure RGB color expression, unexpected filter and transmit components will have no effect.");
                startedParsing = true;
            }
        END_CASE

        CASE_VECTOR
            UNGET
            if (startedParsing)
            {
                EXIT
            }
            else
            {
                // Note: Setting up for potential warning on single value float promote to
                // five value color vector. Under the Parse_Express call there is code which
                // promotes any single float to the full 'Terms' value on the call. This
                // usually results in filter and trasmit values >0, which cause shadow artifacts
                // back to at least version 3.6.1.
                if ((Token.Token_Id==FLOAT_FUNCT_TOKEN) || (Token.Token_Id==FUNCT_ID_TOKEN))
                    sawFloatOrFloatFnct = true;
                else
                    sawFloatOrFloatFnct = false;
                if (expectFT)
                    Terms = 5;
                else
                    Terms = 3;
                Parse_Express(Express,&Terms);
                if (expectFT && (Terms != 5))
                    Error("Color expression expected but float or vector expression found.");
                else if (!expectFT && ((Terms < 3) || Terms > 5))
                    Error("RGB color expression expected but float or vector expression found.");
                colour.Set(Express, Terms);
                if (((sawFloatOrFloatFnct) && (Terms==5)) && ((colour.filter() != 0) && (colour.transm() != 0)))
                    Warning("Float value promoted to full color vector where both filter and transmit >0.0.");
                if (!expectFT && ((colour.filter() != 0) || (colour.transm() != 0)))
                    Warning("Expected pure RGB color expression, unexpected filter and transmit components will have no effect.");
                startedParsing = true;
            }
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    Allow_Identifier_In_Call = old_allow_id;
}

void Parser::Parse_Colour (TransColour& colour, bool expectFT)
{
    RGBFTColour tempColour;
    Parse_Colour (tempColour, expectFT);
    colour = ToTransColour(tempColour);
}

void Parser::Parse_Colour (RGBColour& colour)
{
    RGBFTColour tempColour;
    Parse_Colour (tempColour, false);
    colour = tempColour.rgb();
}

void Parser::Parse_Colour (MathColour& colour)
{
    TransColour tempColour;
    Parse_Colour (tempColour, false);
    colour = tempColour.colour();
}

void Parser::Parse_Wavelengths (MathColour& colour)
{
#if (NUM_COLOUR_CHANNELS == 3)
    RGBFTColour tempColour;
    Parse_Colour (tempColour, false);
    colour = ToMathColour(tempColour.rgb());
#else
    #error TODO!
#endif
}

/*****************************************************************************
*
* FUNCTION
*
*   Parse_Blend_Map
*
* INPUT
*
*   Type of map to parse: pigment_map, normal_map etc
*
* OUTPUT
*
* RETURNS
*
*   Pointer to created blend map
*
* AUTHOR
*
*   Chris Young 11/94
*
* DESCRIPTION   :
*
* CHANGES
*
******************************************************************************/

template<>
void Parser::Parse_BlendMapData<ColourBlendMapData> (BlendMapTypeId Blend_Type, ColourBlendMapData& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Colour);
    Error("Type not implemented yet.");
}

template<>
void Parser::Parse_BlendMapData<PigmentBlendMapData> (BlendMapTypeId Blend_Type, PigmentBlendMapData& rData)
{
    switch (Blend_Type)
    {
        case kBlendMapType_Pigment:
            rData=Copy_Pigment(Default_Texture->Pigment);
            Parse_Pigment(&(rData));
            break;

        case kBlendMapType_Density:
            rData=NULL;
            Parse_Media_Density_Pattern (&(rData));
            break;

        default:
            POV_PARSER_ASSERT(false);
            break;
    }
}

template<>
void Parser::Parse_BlendMapData<SlopeBlendMapData> (BlendMapTypeId Blend_Type, SlopeBlendMapData& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Slope);
    Parse_UV_Vect(rData);
}

template<>
void Parser::Parse_BlendMapData<NormalBlendMapData> (BlendMapTypeId Blend_Type, NormalBlendMapData& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Normal);
    rData=Copy_Tnormal(Default_Texture->Tnormal);
    Parse_Tnormal(&(rData));
}

template<>
void Parser::Parse_BlendMapData<TexturePtr> (BlendMapTypeId Blend_Type, TexturePtr& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Texture);
    rData=Parse_Texture();
}

template<typename MAP_T>
shared_ptr<MAP_T> Parser::Parse_Blend_Map (BlendMapTypeId Blend_Type,int Pat_Type)
{
    shared_ptr<MAP_T>       New;
    GenericPigmentBlendMapPtr pigmentBlendMap;
    typename MAP_T::Entry   Temp_Ent;
    typename MAP_T::Vector  tempList;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;
    int blendMode = 0;
    GammaCurvePtr blendGamma;

    Parse_Begin ();

    EXPECT
        CASE2 (COLOUR_MAP_ID_TOKEN, PIGMENT_MAP_ID_TOKEN)
        CASE3 (NORMAL_MAP_ID_TOKEN, TEXTURE_MAP_ID_TOKEN, SLOPE_MAP_ID_TOKEN)
            New = Copy_Blend_Map (*(reinterpret_cast<shared_ptr<MAP_T> *> (Token.Data)));
            if (Blend_Type != New->Type)
            {
                Error("Wrong identifier type");
            }
            EXIT
        END_CASE

        CASE(BLEND_MODE_TOKEN)
            switch (Blend_Type)
            {
                case kBlendMapType_Pigment:
                case kBlendMapType_Colour:
                    blendMode = Parse_Float();
                    if ((blendMode < 0) || (blendMode > 3))
                        Error("blend_mode must be in the range 0 to 3");
                    break;

                default:
                    Only_In("blend_mode", "colour_map or pigment_map");
                    break;
            }
        END_CASE

        CASE(BLEND_GAMMA_TOKEN)
            switch (Blend_Type)
            {
                case kBlendMapType_Pigment:
                case kBlendMapType_Colour:
                    if (!sceneData->workingGamma)
                        Error("blend_gamma requires that assumed_gamma has been set.");
                    blendGamma = Parse_Gamma();
                    break;

                default:
                    Only_In("blend_gamma", "colour_map or pigment_map");
                    break;
            }
        END_CASE

        OTHERWISE
            UNGET

            EXPECT
                CASE (LEFT_SQUARE_TOKEN)
                    switch (Pat_Type)
                    {
                        case AVERAGE_PATTERN:
                            Temp_Ent.value = Allow_Float(1.0);
                            Parse_Comma();
                            break;

                        default:
                            Temp_Ent.value = Parse_Float();
                            Parse_Comma();
                            break;
                    }

                    Parse_BlendMapData<typename MAP_T::Data> (Blend_Type, Temp_Ent.Vals);
                    tempList.push_back(Temp_Ent);

                    GET (RIGHT_SQUARE_TOKEN);
                END_CASE

                OTHERWISE
                    UNGET
                    if (tempList.empty())
                        Error ("Must have at least one entry in map.");
                    New = Create_Blend_Map<MAP_T> (Blend_Type);
                    New->Set(tempList);
                    pigmentBlendMap = dynamic_pointer_cast<GenericPigmentBlendMap>(New);
                    if (pigmentBlendMap)
                    {
                        pigmentBlendMap->blendMode = blendMode;
                        if (blendGamma == NULL)
                            blendGamma = PowerLawGammaCurve::GetByDecodingGamma(2.5);
                        pigmentBlendMap->blendGamma = GammaCurvePtr(TranscodingGammaCurve::Get(sceneData->workingGamma, blendGamma));
                    }
                    EXIT
                END_CASE
            END_EXPECT
            EXIT
        END_CASE
    END_EXPECT

    Parse_End ();

    Allow_Identifier_In_Call = old_allow_id;

    return (New);
}

template<> GenericPigmentBlendMapPtr Parser::Parse_Blend_Map<GenericPigmentBlendMap> (BlendMapTypeId Blend_Type,int Pat_Type)
{
    switch (Blend_Type)
    {
        case kBlendMapType_Colour:
            return Parse_Blend_Map<ColourBlendMap> (Blend_Type, Pat_Type);
        case kBlendMapType_Pigment:
        case kBlendMapType_Density:
            return Parse_Blend_Map<PigmentBlendMap> (Blend_Type, Pat_Type);
        default:
            POV_BLEND_MAP_ASSERT(false);
            // unreachable code to satisfy the compiler's demands for a return value; an empty pointer will do
            return GenericPigmentBlendMapPtr();
    }
}

template<> GenericNormalBlendMapPtr Parser::Parse_Blend_Map<GenericNormalBlendMap> (BlendMapTypeId Blend_Type,int Pat_Type)
{
    switch (Blend_Type)
    {
        case kBlendMapType_Slope:
            return Parse_Blend_Map<SlopeBlendMap> (Blend_Type, Pat_Type);
        case kBlendMapType_Normal:
            return Parse_Blend_Map<NormalBlendMap> (Blend_Type, Pat_Type);
        default:
            POV_BLEND_MAP_ASSERT(false);
            // unreachable code to satisfy the compiler's demands for a return value; an empty pointer will do
            return GenericNormalBlendMapPtr();
    }
}

template ColourBlendMapPtr  Parser::Parse_Blend_Map<ColourBlendMap>     (BlendMapTypeId Blend_Type,int Pat_Type);
template PigmentBlendMapPtr Parser::Parse_Blend_Map<PigmentBlendMap>    (BlendMapTypeId Blend_Type,int Pat_Type);
template SlopeBlendMapPtr   Parser::Parse_Blend_Map<SlopeBlendMap>      (BlendMapTypeId Blend_Type,int Pat_Type);
template NormalBlendMapPtr  Parser::Parse_Blend_Map<NormalBlendMap>     (BlendMapTypeId Blend_Type,int Pat_Type);
template TextureBlendMapPtr Parser::Parse_Blend_Map<TextureBlendMap>    (BlendMapTypeId Blend_Type,int Pat_Type);

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

template<>
void Parser::Parse_BlendListData<ColourBlendMapData> (BlendMapTypeId Blend_Type, ColourBlendMapData& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Colour);
    Parse_Colour (rData);
}

template<>
void Parser::Parse_BlendListData<PigmentBlendMapData> (BlendMapTypeId Blend_Type, PigmentBlendMapData& rData)
{
    switch (Blend_Type)
    {
        case kBlendMapType_Pigment:
            rData=Copy_Pigment(Default_Texture->Pigment);
            Parse_Pigment(&(rData));
            break;

        case kBlendMapType_Density:
            rData=NULL;
            Parse_Media_Density_Pattern (&(rData));
            break;

        default:
            POV_BLEND_MAP_ASSERT(false);
            break;
    }
}

template<>
void Parser::Parse_BlendListData<SlopeBlendMapData> (BlendMapTypeId Blend_Type, SlopeBlendMapData& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Slope);
    Error("Type not implemented yet.");
}

template<>
void Parser::Parse_BlendListData<NormalBlendMapData> (BlendMapTypeId Blend_Type, NormalBlendMapData& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Normal);
    rData=Copy_Tnormal(Default_Texture->Tnormal);
    Parse_Tnormal(&(rData));
}

template<>
void Parser::Parse_BlendListData<TexturePtr> (BlendMapTypeId Blend_Type, TexturePtr& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Texture);
    rData=Parse_Texture();
}


template<>
void Parser::Parse_BlendListData_Default<ColourBlendMapData> (const ColourBlendMapData& rDefData, BlendMapTypeId Blend_Type, ColourBlendMapData& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Colour);
    rData = rDefData;
}

template<>
void Parser::Parse_BlendListData_Default<PigmentBlendMapData> (const ColourBlendMapData& rDefData, BlendMapTypeId Blend_Type, PigmentBlendMapData& rData)
{
    switch (Blend_Type)
    {
        case kBlendMapType_Pigment:
            rData=Copy_Pigment(Default_Texture->Pigment);
            break;

        case kBlendMapType_Density:
            rData=NULL;
            break;

        default:
            POV_BLEND_MAP_ASSERT(false);
            break;
    }
}

template<>
void Parser::Parse_BlendListData_Default<SlopeBlendMapData> (const ColourBlendMapData& rDefData, BlendMapTypeId Blend_Type, SlopeBlendMapData& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Slope);
    Error("Type not implemented yet.");
}

template<>
void Parser::Parse_BlendListData_Default<NormalBlendMapData> (const ColourBlendMapData& rDefData, BlendMapTypeId Blend_Type, NormalBlendMapData& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Normal);
    rData=Copy_Tnormal(Default_Texture->Tnormal);
}

template<>
void Parser::Parse_BlendListData_Default<TexturePtr> (const ColourBlendMapData& rDefData, BlendMapTypeId Blend_Type, TexturePtr& rData)
{
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Texture);
    rData=Copy_Textures(Default_Texture);
}


template<typename MAP_T>
shared_ptr<MAP_T> Parser::Parse_Blend_List (int Count, ColourBlendMapConstPtr Def_Map, BlendMapTypeId Blend_Type)
{
    shared_ptr<MAP_T>       New;
    typename MAP_T::Vector  tempList;
    int i;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;

    i = 0;

    tempList.resize(Count);

    switch(Blend_Type)
    {
        case kBlendMapType_Colour:
            EXPECT
                CASE_EXPRESS
                    Parse_BlendListData(Blend_Type,tempList[i].Vals);
                    Parse_Comma ();
                    tempList[i].value = (SNGL)i;
                    if (++i >= Count)
                        EXIT
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            break;

        case kBlendMapType_Pigment:
            EXPECT
                CASE(PIGMENT_TOKEN)
                    Parse_Begin ();
                    Parse_BlendListData(Blend_Type,tempList[i].Vals);
                    Parse_End ();
                    Parse_Comma ();
                    tempList[i].value = (SNGL)i;
                    if (++i >= Count)
                        EXIT
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            break;

        case kBlendMapType_Normal:
            EXPECT
                CASE(TNORMAL_TOKEN)
                    Parse_Begin ();
                    Parse_BlendListData(Blend_Type,tempList[i].Vals);
                    Parse_End ();
                    Parse_Comma ();
                    tempList[i].value = (SNGL)i;
                    if (++i >= Count)
                        EXIT
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            break;

        case kBlendMapType_Texture:
            EXPECT
                CASE(TEXTURE_TOKEN)
                    Parse_Begin ();
                    Parse_BlendListData(Blend_Type,tempList[i].Vals);
                    Parse_End ();
                    Parse_Comma ();
                    tempList[i].value = (SNGL)i;
                    if (++i >= Count)
                        EXIT
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            break;

        case kBlendMapType_Density:
            EXPECT
                CASE(DENSITY_TOKEN)
                    Parse_Begin ();
                    Parse_BlendListData(Blend_Type,tempList[i].Vals);
                    Parse_End ();
                    Parse_Comma ();
                    tempList[i].value = (SNGL)i;
                    if (++i >= Count)
                        EXIT
                END_CASE

                OTHERWISE
                    UNGET
                    EXIT
                END_CASE
            END_EXPECT
            break;

    }

    if ((Blend_Type==kBlendMapType_Normal) && (i==0))
    {
        return shared_ptr<MAP_T>(); // empty pointer
    }

    while (i < Count)
    {
        Parse_BlendListData_Default (Def_Map->Blend_Map_Entries[i].Vals, Blend_Type, tempList[i].Vals);
        tempList[i].value = (SNGL)i;
        i++;
    }

    New = Create_Blend_Map<MAP_T> (Blend_Type);
    New->Set(tempList);

    Allow_Identifier_In_Call = old_allow_id;

    return New;
}

template<>
shared_ptr<GenericPigmentBlendMap> Parser::Parse_Blend_List<GenericPigmentBlendMap> (int Count, ColourBlendMapConstPtr Def_Map, BlendMapTypeId Blend_Type)
{
    shared_ptr<GenericPigmentBlendMap> New;
    POV_BLEND_MAP_ASSERT(Blend_Type == kBlendMapType_Pigment);
    EXPECT
        CASE(PIGMENT_TOKEN)
            UNGET
            New = Parse_Blend_List<PigmentBlendMap> (Count, Def_Map, kBlendMapType_Pigment);
            EXIT
        END_CASE

        OTHERWISE
            UNGET
            New = Parse_Blend_List<ColourBlendMap> (Count, Def_Map, kBlendMapType_Colour);
            EXIT
        END_CASE
    END_EXPECT
    return New;
}

template<>
shared_ptr<GenericNormalBlendMap> Parser::Parse_Blend_List<GenericNormalBlendMap> (int Count, ColourBlendMapConstPtr Def_Map, BlendMapTypeId Blend_Type)
{
    shared_ptr<GenericNormalBlendMap> New;
    switch (Blend_Type)
    {
        case kBlendMapType_Slope:
            New = Parse_Blend_List<SlopeBlendMap> (Count, Def_Map, Blend_Type);
            break;

        case kBlendMapType_Normal:
            New = Parse_Blend_List<NormalBlendMap> (Count, Def_Map, Blend_Type);
            break;

        default:
            POV_BLEND_MAP_ASSERT(false);
    }
    return New;
}

template ColourBlendMapPtr  Parser::Parse_Blend_List<ColourBlendMap>    (int Count, ColourBlendMapConstPtr Def_Map, BlendMapTypeId Blend_Type);
template PigmentBlendMapPtr Parser::Parse_Blend_List<PigmentBlendMap>   (int Count, ColourBlendMapConstPtr Def_Map, BlendMapTypeId Blend_Type);
template SlopeBlendMapPtr   Parser::Parse_Blend_List<SlopeBlendMap>     (int Count, ColourBlendMapConstPtr Def_Map, BlendMapTypeId Blend_Type);
template NormalBlendMapPtr  Parser::Parse_Blend_List<NormalBlendMap>    (int Count, ColourBlendMapConstPtr Def_Map, BlendMapTypeId Blend_Type);
template TextureBlendMapPtr Parser::Parse_Blend_List<TextureBlendMap>   (int Count, ColourBlendMapConstPtr Def_Map, BlendMapTypeId Blend_Type);

/*****************************************************************************
*
* FUNCTION    Parse_Item_Into_Blend_List
*
* INPUT       Blend_Type
*
* OUTPUT
*
* RETURNS     BLEND_MAP
*
* AUTHOR      Nathan Kopp and others - copied & modified from Parse_Blend_List
*
* DESCRIPTION
*
*   This performs a similar funciton to Parse_Blend_List.  It was created
*   specifically for uv mapping.  It is different from Parse_Blend_List in
*   the following ways:
*     It will parse exactly one item (normal,pigment, or texture).
*     It will NOT parse any wrapping tokens, such as "texture{...}"
*       (It is looking only for the body of the item, not the entire item.)
*     Because it always parses exactly one item, no default blend list is
*       needed.
*
* CHANGES
*
******************************************************************************/

template<typename MAP_T>
shared_ptr<MAP_T> Parser::Parse_Item_Into_Blend_List (BlendMapTypeId Blend_Type)
{
    shared_ptr<MAP_T>       New;
    typename MAP_T::Entry   Temp_Ent;
    typename MAP_T::Vector  tempList;
    BlendMapTypeId Type;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;

    Type=Blend_Type;

    Temp_Ent.value = 0.0f;

    Parse_BlendListData (Type, Temp_Ent.Vals);
    tempList.push_back(Temp_Ent);

    New = Create_Blend_Map<MAP_T> (Type);
    New->Set(tempList);

    Allow_Identifier_In_Call = old_allow_id;

    return (New);
}

template<> GenericPigmentBlendMapPtr Parser::Parse_Item_Into_Blend_List<GenericPigmentBlendMap> (BlendMapTypeId Blend_Type)
{
    switch (Blend_Type)
    {
        case kBlendMapType_Colour:
            return Parse_Item_Into_Blend_List<ColourBlendMap> (Blend_Type);
        case kBlendMapType_Pigment:
            return Parse_Item_Into_Blend_List<PigmentBlendMap> (Blend_Type);
        default:
            POV_BLEND_MAP_ASSERT(false);
            // unreachable code to satisfy the compiler's demands for a return value; an empty pointer will do
            return GenericPigmentBlendMapPtr();
    }
}

template<> GenericNormalBlendMapPtr Parser::Parse_Item_Into_Blend_List<GenericNormalBlendMap> (BlendMapTypeId Blend_Type)
{
    switch (Blend_Type)
    {
        case kBlendMapType_Slope:
            return Parse_Item_Into_Blend_List<SlopeBlendMap> (Blend_Type);
        case kBlendMapType_Normal:
            return Parse_Item_Into_Blend_List<NormalBlendMap> (Blend_Type);
        default:
            POV_BLEND_MAP_ASSERT(false);
            // unreachable code to satisfy the compiler's demands for a return value; an empty pointer will do
            return GenericNormalBlendMapPtr();
    }
}

template ColourBlendMapPtr  Parser::Parse_Item_Into_Blend_List<ColourBlendMap>  (BlendMapTypeId Blend_Type);
template PigmentBlendMapPtr Parser::Parse_Item_Into_Blend_List<PigmentBlendMap> (BlendMapTypeId Blend_Type);
template SlopeBlendMapPtr   Parser::Parse_Item_Into_Blend_List<SlopeBlendMap>   (BlendMapTypeId Blend_Type);
template NormalBlendMapPtr  Parser::Parse_Item_Into_Blend_List<NormalBlendMap>  (BlendMapTypeId Blend_Type);
template TextureBlendMapPtr Parser::Parse_Item_Into_Blend_List<TextureBlendMap> (BlendMapTypeId Blend_Type);

/*****************************************************************************
*
* FUNCTION
*
*   Parse_Colour_Map
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   Pointer to newly created BLEND_MAP that has colors as all
*   its entries.
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : This seperate routine parses color_maps only.  It
*                 cannot be used for pigment_maps because it accomidates
*                 the old double entry color maps from vers 1.0
*
* CHANGES
*
******************************************************************************/

template<>
ColourBlendMapPtr Parser::Parse_Colour_Map<ColourBlendMap> ()
{
    ColourBlendMapPtr New;
    int c,p;
    EXPRESS Express;
    int Terms;
    ColourBlendMapEntry Temp_Ent, Temp_Ent_2;
    vector<ColourBlendMapEntry> tempList;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;
    int blendMode = 0;
    GammaCurvePtr blendGamma;

    Parse_Begin ();

    EXPECT
        CASE (COLOUR_MAP_ID_TOKEN)
            New = *(reinterpret_cast<ColourBlendMapPtr *>(Token.Data));
            EXIT
        END_CASE

        CASE(BLEND_MODE_TOKEN)
            blendMode = Parse_Float();
            if ((blendMode < 0) || (blendMode > 3))
                Error("blend_mode must be in the range 0 to 3");
        END_CASE

        CASE(BLEND_GAMMA_TOKEN)
            if (!sceneData->workingGamma)
                Error("blend_gamma requires that assumed_gamma has been set.");
            blendGamma = Parse_Gamma();
        END_CASE

        OTHERWISE
            UNGET

            EXPECT
                CASE (LEFT_SQUARE_TOKEN)
                    Temp_Ent.value = Parse_Float();  Parse_Comma();

                    EXPECT
                        /* After [ must be a float. If 2nd thing found is another
                           float then this is an old style color_map.
                         */
                        CASE_FLOAT
                            Terms=1;
                            Parse_Express(Express,&Terms);
                            if (Terms==1)
                            {
                                Temp_Ent_2.value = Express[0];
                                Parse_Colour (Temp_Ent.Vals);

                                GET (COLOUR_TOKEN);
                                Parse_Colour (Temp_Ent_2.Vals);
                                tempList.push_back(Temp_Ent);
                                tempList.push_back(Temp_Ent_2);
                            }
                            else
                                if (Terms==5)
                                {
                                    RGBFTColour rgbft;
                                    rgbft.Set(Express, Terms);
                                    Temp_Ent.Vals = ToTransColour (rgbft);
                                    tempList.push_back(Temp_Ent);
                                }
                                else
                                    Error("Illegal expression syntax in color_map.");
                            EXIT
                        END_CASE

                        CASE_COLOUR
                            Parse_Colour (Temp_Ent.Vals);
                            tempList.push_back(Temp_Ent);
                            EXIT
                        END_CASE

                        OTHERWISE
                            Expectation_Error("color");
                            UNGET
                            EXIT
                        END_CASE

                    END_EXPECT

                    GET (RIGHT_SQUARE_TOKEN);
                END_CASE

                OTHERWISE
                    UNGET
                    if (tempList.empty())
                        Error ("Must have at least one color in color map.");

                    /* Eliminate duplicates */
                    for (c = 1, p = 0; c<tempList.size(); c++)
                    {
                        if (memcmp(&(tempList[p]),
                                   &(tempList[c]),sizeof(ColourBlendMapEntry)) == 0)
                            p--;

                        tempList[++p] = tempList[c];
                    }
                    p++;
                    tempList.resize(p);
                    New = ColourBlendMapPtr (new ColourBlendMap);
                    New->Set(tempList);
                    New->blendMode = blendMode;
                    if (blendGamma == NULL)
                        blendGamma = PowerLawGammaCurve::GetByDecodingGamma(2.5);
                    New->blendGamma = GammaCurvePtr(TranscodingGammaCurve::Get(sceneData->workingGamma, blendGamma));
                    EXIT
                END_CASE
            END_EXPECT
            EXIT
        END_CASE
    END_EXPECT

    Parse_End ();

    Allow_Identifier_In_Call = old_allow_id;

    return (New);
}

template<>
GenericPigmentBlendMapPtr Parser::Parse_Colour_Map<GenericPigmentBlendMap> ()
{
    return Parse_Colour_Map<ColourBlendMap>();
}

template<>
PigmentBlendMapPtr Parser::Parse_Colour_Map<PigmentBlendMap> ()
{
    Error("Internal Error: Parse_Colour_Map called for non-colour blend map");
    // unreachable code to satisfy the compiler's demands for a return value; an empty pointer will do
    return PigmentBlendMapPtr();
}

template<>
GenericNormalBlendMapPtr Parser::Parse_Colour_Map<GenericNormalBlendMap> ()
{
    Error("Internal Error: Parse_Colour_Map called for non-colour blend map");
    // unreachable code to satisfy the compiler's demands for a return value; an empty pointer will do
    return GenericNormalBlendMapPtr();
}

template<>
SlopeBlendMapPtr Parser::Parse_Colour_Map<SlopeBlendMap> ()
{
    Error("Internal Error: Parse_Colour_Map called for non-colour blend map");
    // unreachable code to satisfy the compiler's demands for a return value; an empty pointer will do
    return SlopeBlendMapPtr();
}

template<>
NormalBlendMapPtr Parser::Parse_Colour_Map<NormalBlendMap> ()
{
    Error("Internal Error: Parse_Colour_Map called for non-colour blend map");
    // unreachable code to satisfy the compiler's demands for a return value; an empty pointer will do
    return NormalBlendMapPtr();
}

template<>
TextureBlendMapPtr Parser::Parse_Colour_Map<TextureBlendMap> ()
{
    Error("Internal Error: Parse_Colour_Map called for non-colour blend map");
    // unreachable code to satisfy the compiler's demands for a return value; an empty pointer will do
    return TextureBlendMapPtr();
}


/*****************************************************************************
*
* FUNCTION
*
*   Parse_Spline
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   Pointer to newly created Spline
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION   : This seperate routine parses pure splines only. Splines in
*   lathe objects and SOR are similar but not identical
*
* CHANGES
*   Chris Huff Nov 2000 : Added the ability to use one spline as the basis for
*        another
*   Mark Wagner Nov 2000 : Modified to work with the dynamic-allocation version
*        of splines.c
*
******************************************************************************/

GenericSpline *Parser::Parse_Spline()
{
    GenericSpline * Old = NULL;
    GenericSpline * New = NULL;
    int i = 0;
    EXPRESS Express;
    int Terms, MaxTerms;
    DBL par;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;

    MaxTerms = 2;

    /*Check for spline identifier*/
    EXPECT
        CASE(SPLINE_ID_TOKEN)
            Old = reinterpret_cast<GenericSpline *>(Token.Data);
            i = Old->SplineEntries.size();
            MaxTerms = Old->Terms;
            EXIT
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    /* Determine kind of spline */
    EXPECT
        CASE(LINEAR_SPLINE_TOKEN)
            if (Old)
                New = new LinearSpline(*Old);
            else
                New = new LinearSpline();
            Old = New;
        END_CASE

        CASE(QUADRATIC_SPLINE_TOKEN)
            if (Old)
                New = new QuadraticSpline(*Old);
            else
                New = new QuadraticSpline();
            Old = New;
        END_CASE

        CASE(CUBIC_SPLINE_TOKEN)
            if (Old)
                New = new CatmullRomSpline(*Old);
            else
                New = new CatmullRomSpline();
            Old = New;
        END_CASE

        CASE(NATURAL_SPLINE_TOKEN)
            if (Old)
                New = new NaturalSpline(*Old);
            else
                New = new NaturalSpline();
            Old = New;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if (!New)
    {
        if (Old)
            New = new LinearSpline(*Old);
        else
            New = new LinearSpline();
    }

    EXPECT
        CASE_FLOAT
            /* Entry has the form float,vector */
            par = Parse_Float();
            Parse_Comma();

            Terms = 2;
            Parse_Express(Express, &Terms);
            if(Terms > 5)
                    Error("Too many components in vector!\n");
            MaxTerms = max(MaxTerms, Terms);
            Parse_Comma();
            /* MWW 2000 -- Changed call for dynamic allocation version */
            Insert_Spline_Entry(New, par, Express);
            i++;
        END_CASE

        OTHERWISE
            UNGET
            EXIT
        END_CASE
    END_EXPECT

    if(i < 1)
            Error("Spline must have at least one entry.");

    New->Terms = MaxTerms; // keep number of supplied terms

    Allow_Identifier_In_Call = old_allow_id;

    return New;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::POV_strupr(char *s)
{
    int i,len;

    for (i = 0,len = (int)strlen(s); i < len; i++)
    {
        s[i] = (char)toupper((int)s[i]);
    }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parser::POV_strlwr(char *s)
{
    int i,len;

    for (i = 0,len = (int)strlen(s); i < len; i++)
    {
        s[i] = (char)tolower((int)s[i]);
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   stream_rand
*
* INPUT
*
*   stream - number of random stream
*
* OUTPUT
*
* RETURNS
*
*   DBL - random value
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Standard pseudo-random function.
*
* CHANGES
*
*   Feb 1996 : Creation.
*   Mar 1996 : Return 2^32 random values instead of 2^16 [AED]
*
******************************************************************************/

DBL Parser::stream_rand(int stream)
{
    return POV_rand(next_rand[stream]);
}



/*****************************************************************************
*
* FUNCTION
*
*   stream_seed
*
* INPUT
*
*   seed - Pseudo-random generator start value
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Set start value for pseudo-random generator.
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

int Parser::stream_seed(int seed)
{
    next_rand = reinterpret_cast<unsigned int *>(POV_REALLOC(next_rand, (Number_Of_Random_Generators+1)*sizeof(unsigned int), "random number generator"));

    next_rand[Number_Of_Random_Generators] = (unsigned int)seed;

    Number_Of_Random_Generators++;

    return (Number_Of_Random_Generators-1);
}



/*****************************************************************************
*
* FUNCTION
*
*   Init_Random_Generators
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Parser::Init_Random_Generators()
{
    Number_Of_Random_Generators = 0;

    next_rand = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Random_Generators
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Parser::Destroy_Random_Generators()
{
    if (next_rand != NULL)
    {
        POV_FREE(next_rand);
    }

    next_rand = NULL;

    Number_Of_Random_Generators = 0;
}

DBL Parser::Parse_Signed_Float(void)
{
    DBL Sign=1.0;
    DBL Val=0.0;
    bool old_allow_id = Allow_Identifier_In_Call;
    Allow_Identifier_In_Call = false;

    EXPECT
        CASE (PLUS_TOKEN)
        END_CASE

        CASE (DASH_TOKEN)
            Sign=-1.0;
            Get_Token();
            /* Deliberate fall through with no END_CASE */
        CASE (FLOAT_FUNCT_TOKEN)
            if (Token.Function_Id==FLOAT_TOKEN)
            {
                Val = Sign * Token.Token_Float;
                EXIT
            }
            else
            {
                Parse_Error(FLOAT_TOKEN);
            }
        END_CASE

        OTHERWISE
            Parse_Error(FLOAT_TOKEN);
        END_CASE
    END_EXPECT

    Allow_Identifier_In_Call = old_allow_id;

    return(Val);
}

}
