//******************************************************************************
///
/// @file parser/parser_functions.cpp
///
/// This module implements the the function type used by iso surfaces and
/// the function pattern.
///
/// This module is inspired by code by D. Skarda, T. Bily and R. Suzuki.
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

// C++ variants of C standard header files
#include <cmath>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/mathutil.h"
#include "base/pov_mem.h"

// POV-Ray header files (core module)
#include "core/scene/scenedata.h"

// POV-Ray header files (VM module)
#include "vm/fnpovfpu.h"

// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

using namespace pov;

/*****************************************************************************
* Local typedefs
******************************************************************************/

struct ExprParserTableEntry final
{
    int stage;
    TokenId token;
    bool (Parser::*operation)(ExprNode *&, int, int);
    int next;
    int op;
};

struct ExprParserErrorEntry final
{
    int stage;
    const char *expected;
};


/*****************************************************************************
* Local variables
******************************************************************************/

const ExprParserErrorEntry expr_parser_error_table[] =
{
    { 35, "operator" },
    { 45, "." },
    { 40, "sign or operand" },
    { 50, "operand" },
    { 55, ")" },
    { 60, "color or vector member" },
    { -1, nullptr }
};

const ExprParserTableEntry expr_parser_table[] =
{
    // logical or
    {  5, BAR_TOKEN,         &Parser::expr_grow, 40, OP_OR       }, // 0
    // logical and
    { 10, AMPERSAND_TOKEN,   &Parser::expr_grow, 40, OP_AND      }, // 1
    // equal, not equal
    { 15, EQUALS_TOKEN,      &Parser::expr_grow, 40, OP_CMP_EQ   }, // 2
    { 15, REL_NE_TOKEN,      &Parser::expr_grow, 40, OP_CMP_NE   }, // 3
    // smaller and/or equal, greater and/or equal
    { 15, LEFT_ANGLE_TOKEN,  &Parser::expr_grow, 40, OP_CMP_LT   }, // 4
    { 15, REL_LE_TOKEN,      &Parser::expr_grow, 40, OP_CMP_LE   }, // 5
    { 15, RIGHT_ANGLE_TOKEN, &Parser::expr_grow, 40, OP_CMP_GT   }, // 6
    { 15, REL_GE_TOKEN,      &Parser::expr_grow, 40, OP_CMP_GE   }, // 7
    // plus, minus
    { 20, PLUS_TOKEN,        &Parser::expr_grow, 40, OP_ADD      }, // 8
    { 20, DASH_TOKEN,        &Parser::expr_grow, 40, OP_SUB      }, // 9
    // multiply, divide
    { 25, STAR_TOKEN,        &Parser::expr_grow, 40, OP_MUL      }, // 10
    { 25, SLASH_TOKEN,       &Parser::expr_grow, 40, OP_DIV      }, // 11
    // ')', '}' or ',' - end of function
    { 35, RIGHT_PAREN_TOKEN, &Parser::expr_ret,  -1, OP_NONE     }, // 12
    { 35, RIGHT_CURLY_TOKEN, &Parser::expr_ret,  -1, OP_NONE     }, // 13
    { 35, COMMA_TOKEN,       &Parser::expr_ret,  -1, OP_NONE     }, // 14
    { 35, TOKEN_COUNT_,      &Parser::expr_err,  -1, OP_NONE     }, // 15
    // vector/color member access
    { 45, PERIOD_TOKEN,      &Parser::expr_grow, 60, OP_DOT      }, // 16
    { 45, TOKEN_COUNT_,      &Parser::expr_err,  -1, OP_NONE     }, // 17
    // unary plus, unary minus, (logical not - disabled)
    { 40, PLUS_TOKEN,        &Parser::expr_noop, 50, OP_NONE     }, // 18
    { 40, DASH_TOKEN,        &Parser::expr_grow, 50, OP_NEG      }, // 19
    { 40, EXCLAMATION_TOKEN, &Parser::expr_err,  -1, OP_NOT      }, // 20
    // constant, variable, (expression), function
    { 50, FLOAT_TOKEN,       &Parser::expr_put,   5, OP_CONSTANT }, // 21
    { 50, FLOAT_ID_TOKEN,    &Parser::expr_put,   5, OP_VARIABLE }, // 22
    { 50, FUNCT_ID_TOKEN,    &Parser::expr_call,  5, OP_CALL     }, // 23
    { 50, VECTFUNCT_ID_TOKEN,&Parser::expr_call, 45, OP_CALL     }, // 24
    { 50, LEFT_PAREN_TOKEN,  &Parser::expr_new,  55, OP_FIRST    }, // 25
    { 50, TOKEN_COUNT_,      &Parser::expr_err,  -1, OP_NONE     }, // 26
    // (expression)
    { 55, RIGHT_PAREN_TOKEN, &Parser::expr_noop,  5, OP_NONE     }, // 27
    { 55, TOKEN_COUNT_,      &Parser::expr_err,  -1, OP_NONE     }, // 28
    // vector/color members
    { 60, FLOAT_ID_TOKEN,    &Parser::expr_put,   5, OP_MEMBER   }, // 29
    { 60, T_TOKEN,           &Parser::expr_put,   5, OP_MEMBER   }, // 30
    { 60, RED_TOKEN,         &Parser::expr_put,   5, OP_MEMBER   }, // 31
    { 60, GREEN_TOKEN,       &Parser::expr_put,   5, OP_MEMBER   }, // 32
    { 60, BLUE_TOKEN,        &Parser::expr_put,   5, OP_MEMBER   }, // 33
    { 60, FILTER_TOKEN,      &Parser::expr_put,   5, OP_MEMBER   }, // 34
    { 60, TRANSMIT_TOKEN,    &Parser::expr_put,   5, OP_MEMBER   }, // 35
    { 60, GRAY_TOKEN,        &Parser::expr_put,   5, OP_MEMBER   }, // 36
    { 60, TOKEN_COUNT_,      &Parser::expr_err,  -1, OP_NONE     }  // 37
};

// parse_expr has to start with first unary operator [trf]
const int START_LEFTMOST_PARSE_INDEX = 18;

/*****************************************************************************
*
* FUNCTION
*
*   FNSyntax_ParseExpression
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   ExprNode - parsed expression root pointer
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Parse and optimise an expression.
*
* CHANGES
*
*   -
*
******************************************************************************/

ExprNode *Parser::FNSyntax_ParseExpression()
{
    ExprNode *expression = nullptr;

    expression = parse_expr();
    optimise_expr(expression);

    return expression;
}


/*****************************************************************************
*
* FUNCTION
*
*   FNSyntax_GetTrapExpression
*
* INPUT
*
*   trap - number of the trap
*
* OUTPUT
*
* RETURNS
*
*   ExprNode - expression root pointer
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Generate an expression which only contains a trap.
*
* CHANGES
*
*   -
*
******************************************************************************/

ExprNode *Parser::FNSyntax_GetTrapExpression(unsigned int trap)
{
    ExprNode *expression = nullptr;

    expression = new_expr_node(0, OP_TRAP);
    expression->trap = trap;

    return expression;
}


/*****************************************************************************
*
* FUNCTION
*
*   FNSyntax_DeleteExpression
*
* INPUT
*
*   node - root node of the (sub-) tree to delete
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
*   Delete an expression (sub-) tree.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Parser::FNSyntax_DeleteExpression(ExprNode *node)
{
    ExprNode *temp = nullptr;

    for (ExprNode *i = node; i != nullptr; i = i->next)
    {
        if (temp != nullptr)
        {
            POV_FREE(temp);
        }

        FNSyntax_DeleteExpression(i->child);

        if((i->op == OP_VARIABLE) || (i->op == OP_MEMBER))
        {
            POV_FREE(i->variable);
        }
        else if(i->op == OP_CALL)
        {
            if((i->call.token == FUNCT_ID_TOKEN) || (i->call.token == VECTFUNCT_ID_TOKEN))
                mpFunctionVM->RemoveFunction(i->call.fn);
            POV_FREE(i->call.name);
        }

        temp = i;
    }

    if (temp != nullptr)
    {
        POV_FREE(temp);
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   parse_expr
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   ExprNode - parsed expression root pointer
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Parse an expression or subexpression.
*
* CHANGES
*
*   -
*
******************************************************************************/

ExprNode *Parser::parse_expr()
{
    ExprNode *current = nullptr;
    ExprNode *node = nullptr;
    TokenId token;
    int start_index;
    int i;

    current = node = new_expr_node(0, OP_FIRST);

    start_index = START_LEFTMOST_PARSE_INDEX;
    token = expr_get_token();

    while(true)
    {
        // search for matching token
        for(i = start_index; ; i++)
        {
            if((expr_parser_table[i].token == token) ||
               (expr_parser_table[i].token == TOKEN_COUNT))
                break;
        }

        // execute operation
        if((this->*(expr_parser_table[i].operation))(current, expr_parser_table[i].stage, expr_parser_table[i].op) == false)
            break;

        // find next index start
        if(expr_parser_table[i].next >= 0)
        {
            if(expr_parser_table[i].next < expr_parser_table[i].stage)
                start_index = 0;
            // searching the whole table allows forward references
            // to stages with a lower stage number [trf]
            while(expr_parser_table[start_index].stage != expr_parser_table[i].next)
                start_index++;
        }

        token = expr_get_token();
    }

    return node;
}


/*****************************************************************************
*
* FUNCTION
*
*   expr_get_token
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   TokenId - simplified token from Get_Token
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Gets a token and simplifies it for use by the expression parser.
*
* CHANGES
*
*   -
*
******************************************************************************/

TokenId Parser::expr_get_token()
{
    Get_Token();

    switch (CurrentTrueTokenId())
    {
        case X_TOKEN:
        case Y_TOKEN:
        case Z_TOKEN:
        case U_TOKEN:
        case V_TOKEN:
        case IDENTIFIER_TOKEN:
            return FLOAT_ID_TOKEN;

        case CLOCK_TOKEN:
            mToken.Token_Float = clockValue;
            return FLOAT_TOKEN;

        case PI_TOKEN:
            mToken.Token_Float = M_PI;
            return FLOAT_TOKEN;

        case TAU_TOKEN:
            mToken.Token_Float = M_TAU;
            return FLOAT_TOKEN;

        case FLOAT_TOKEN:
            // mToken.Token_Float already set
            return FLOAT_TOKEN;

        case FLOAT_ID_TOKEN:
            mToken.Token_Float = CurrentTokenData<DBL>();
            return FLOAT_TOKEN;

        default:
            if (CurrentCategorizedTokenId() == FLOAT_TOKEN_CATEGORY)
                return FUNCT_ID_TOKEN;
            else
                return CurrentTrueTokenId();
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   new_expr_node
*
* INPUT
*
*   stage - stage/precedence of new node
*   op - operation of new node
*
* OUTPUT
*
* RETURNS
*
*   ExprNode - new expression node structure
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Creates a new expression node structure.
*
* CHANGES
*
*   -
*
******************************************************************************/

ExprNode *Parser::new_expr_node(int stage, int op)
{
    ExprNode *node = nullptr;

    node = reinterpret_cast<ExprNode *>(POV_MALLOC(sizeof(ExprNode), "ExprNode"));
    node->parent = nullptr;
    node->child = nullptr;
    node->prev = nullptr;
    node->next = nullptr;
    node->stage = stage;
    node->op = op;

    return node;
}


/*****************************************************************************
*
* FUNCTION
*
*   expr_noop
*
* INPUT
*
*   current - current poistion in expression tree
*   stage - stage/precedence of operation
*   op - operation
*
* OUTPUT
*
* RETURNS
*
*   bool - continue to parse expression?
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Expression parser manipulation function that does nothing.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Parser::expr_noop(ExprNode *&, int, int)
{
    return true;
}


/*****************************************************************************
*
* FUNCTION
*
*   expr_grow
*
* INPUT
*
*   current - current poistion in expression tree
*   stage - stage/precedence of operation
*   op - operation
*
* OUTPUT
*
* RETURNS
*
*   bool - continue to parse expression?
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Expression parser manipulation function that grows the tree in the
*   correct place based on the stage/level of the other nodes in the tree.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Parser::expr_grow(ExprNode *&current, int stage, int op)
{
    ExprNode *node = nullptr;

    if (current == nullptr)
        return false;

    // the idea is this order: current, node, current->child
    if(current->stage < stage)
    {
        while (current->child != nullptr)
        {
            if(current->child->stage > stage)
                break;

            current = current->child;

            if(current->stage == stage)
                break;
        }
    }
    else if(current->stage > stage)
    {
        while (current->parent != nullptr)
        {
            current = current->parent;

            if(current->stage <= stage)
                break;
        }
    }

    if(current->stage == stage)
    {
        while (current->next != nullptr)
            current = current->next;

        node = new_expr_node(stage, op);

        node->parent = current->parent;
        node->prev = current;
        current->next = node;

        current = node;
    }
    else
    {
        node = new_expr_node(stage, OP_LEFTMOST);

        node->parent = current;
        node->child = current->child;
        current->child = node;
        for (ExprNode *ptr = node->child; ptr != nullptr; ptr = ptr->next)
            ptr->parent = node;

        current = new_expr_node(stage, op);
        current->prev = node;
        node->next = current;
        current->parent = node->parent;
    }

    return true;
}


/*****************************************************************************
*
* FUNCTION
*
*   expr_call
*
* INPUT
*
*   current - current poistion in expression tree
*   stage - stage/precedence of operation
*   op - operation
*
* OUTPUT
*
* RETURNS
*
*   bool - continue to parse expression?
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Expression parser manipulation function that handles a function call.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Parser::expr_call(ExprNode *&current, int stage, int op)
{
    ExprNode *node = nullptr;

    if (current == nullptr)
        return false;

    node = new_expr_node(stage, op);

    if (HaveCurrentTokenData())
    {
        node->call.fn = *CurrentTokenDataPtr<AssignableFunction*>()->fn;
        (void)mpFunctionVM->GetFunctionAndReference(node->call.fn);
    }
    else
        node->call.fn = 0;
    node->call.token = CurrentTrueTokenId();
    node->call.name = POV_STRDUP(CurrentTokenText().c_str());
    while (current->child != nullptr)
        current = current->child;

    current->child = node;
    node->parent = current;
    current = node;

    if(expr_get_token() != LEFT_PAREN_TOKEN)
        Expectation_Error("(");

    current->child = node = parse_expr();
    while(expr_get_token() == COMMA_TOKEN)
    {
        node->next = parse_expr();
        node->next->parent = node->parent;
        node = node->next;
    }

    if(CurrentTrueTokenId() != RIGHT_PAREN_TOKEN)
        Expectation_Error(")");

    return true;
}


/*****************************************************************************
*
* FUNCTION
*
*   expr_put
*
* INPUT
*
*   current - current poistion in expression tree
*   stage - stage/precedence of operation
*   op - operation
*
* OUTPUT
*
* RETURNS
*
*   bool - continue to parse expression?
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Expression parser manipulation function that adds a new node in the
*   current location in the tree.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Parser::expr_put(ExprNode *&current, int stage, int op)
{
    ExprNode *node = nullptr;

    if (current == nullptr)
        return false;

    if (current->child != nullptr)
        return false;

    node = new_expr_node(stage, op);

    if(op == OP_CONSTANT)
    {
        node->number = mToken.Token_Float;
    }
    else
    {
        node->variable = POV_STRDUP(CurrentTokenText().c_str());
    }

    current->child = node;
    node->parent = current;

    return true;
}


/*****************************************************************************
*
* FUNCTION
*
*   expr_new
*
* INPUT
*
*   current - current poistion in expression tree
*   stage - stage/precedence of operation
*   op - operation
*
* OUTPUT
*
* RETURNS
*
*   bool - continue to parse expression?
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Expression parser manipulation function that creates a new expression
*   or subexpression.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Parser::expr_new(ExprNode *&current, int /*stage*/, int /*op*/)
{
    ExprNode *node = nullptr;

    node = parse_expr();
    if (node == nullptr)
        return false;

    current->child = node;
    node->parent = current;
    node->stage = 10000; // needs to be higher than any other stage for expr_grow to work [trf]

    return true;
}


/*****************************************************************************
*
* FUNCTION
*
*   expr_ret
*
* INPUT
*
*   current - current poistion in expression tree
*   stage - stage/precedence of operation
*   op - operation
*
* OUTPUT
*
* RETURNS
*
*   bool - continue to parse expression?
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Expression parser manipulation function that marks the end of parsing
*   a expression or subexpression.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Parser::expr_ret(ExprNode *&, int, int)
{
    Unget_Token();

    return false;
}


/*****************************************************************************
*
* FUNCTION
*
*   expr_err
*
* INPUT
*
*   current - current poistion in expression tree
*   stage - stage/precedence of operation
*   op - operation
*
* OUTPUT
*
* RETURNS
*
*   bool - continue to parse expression?
*
* AUTHOR
*
*   Thorsten Froehlich
*
* DESCRIPTION
*
*   Expression parser manipulation function that terminates all parsing
*   and outputs a parse error message.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Parser::expr_err(ExprNode *&, int stage, int)
{
    int i;

    if(stage == 35)
        PossibleError("Suspicious identifier found in function!\n"
                      "If you want to call a function make sure the function you call has been declared.\n"
                      "If you call an internal function, make sure you have included 'functions.inc'.");

    for (i = 0; (expr_parser_error_table[i].stage >= 0) && (expr_parser_error_table[i].expected != nullptr); i++)
    {
        if(expr_parser_error_table[i].stage == stage)
            Expectation_Error(expr_parser_error_table[i].expected);
    }

    Expectation_Error("valid function expression");

    // unreachable, Expectation_Error stops parsing
    return false;
}


/*****************************************************************************
*
* FUNCTION
*
*   optimise_expr
*
* INPUT
*
*   node - root node of the expression (sub-) tree to optimise
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
*   Optimised an expression (sub-) tree.  Currently it only combines
*   constants next to each other.  Other optimisations are still waiting
*   to be implemented.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Parser::optimise_expr(ExprNode *node)
{
    ExprNode *left,*right,*ptr,*temp;
    DBL result;
    bool have_result;
    int op,cnt;

    if (node == nullptr)
        return;

    if(node->op == OP_CALL)
    {
        if(node->call.token == POW_TOKEN)
        {
            node->op = OP_FIRST;
            POV_FREE(node->call.name);
            if (node->child != nullptr)
            {
                node->child->op = OP_LEFTMOST;
                if (node->child->next != nullptr)
                {
                    node->child->next->op = OP_POW;
                    node->child->next->prev = node->child;
                }
            }
        }
    }

    if(node->op < OP_FIRST) // using switch statement might be better [trf]
    {
        ptr = node->next;
        if (ptr != nullptr)
        {
            if(ptr->op == OP_NEG)
            {
                op = ptr->op;
                cnt = 0;
                for (ptr = node->next; ptr != nullptr; ptr = ptr->next)
                {
                    cnt++;
                    if (ptr->child != nullptr)
                        break;
                }

                if (ptr != nullptr)
                {
                    optimise_expr(ptr->child);
                    if (ptr->child != nullptr)
                    {
                        left = ptr->child;
                        if(left->op == OP_CONSTANT)
                        {
                            ptr->child = nullptr;

                            if (node->next != nullptr)
                                FNSyntax_DeleteExpression(node->next);

                            if(op == OP_NEG)
                            {
                                if((cnt % 2) == 0)
                                    node->number = (left->number);
                                else
                                    node->number = -(left->number);
                            }
                            POV_FREE(left);
                            node->op = OP_CONSTANT;
                            node->child = nullptr;
                            node->prev = nullptr;
                            node->next = nullptr;
                            return; // early exit
                        }
                    }
                }
            }
        }

        optimise_expr(node->child);
        for (ptr = node->next; ptr != nullptr; ptr = ptr->next)
        {
            left = ptr->prev->child;
            right = ptr->child;

            if ((right != nullptr) && (ptr->op == OP_SUB))
            {
                if(right->op == OP_CONSTANT)
                {
                    ptr->op = OP_ADD;
                    right->number = -right->number;
                }
            }

            optimise_expr(right);

            if ((left != nullptr) && (right != nullptr) &&
                (((ptr->op != OP_MUL) && (ptr->op != OP_DIV)) ||
                 !left_subtree_has_variable_expr(ptr)))
            {
                if((left->op == OP_CONSTANT) && (right->op == OP_CONSTANT))
                {
                    have_result = true;

                    switch(ptr->op)
                    {
                        case OP_CMP_EQ:
                            result = (left->number == right->number);
                            break;
                        case OP_CMP_NE:
                            result = (left->number != right->number);
                            break;
                        case OP_CMP_LT:
                            result = (left->number < right->number);
                            break;
                        case OP_CMP_LE:
                            result = (left->number <= right->number);
                            break;
                        case OP_CMP_GT:
                            result = (left->number > right->number);
                            break;
                        case OP_CMP_GE:
                            result = (left->number >= right->number);
                            break;
                        case OP_ADD:
                            result = (left->number + right->number);
                            break;
                        case OP_SUB:
                            result = (left->number - right->number);
                            break;
                        case OP_OR:
                            result = (DBL)(((DBL)((((DBL)(left->number != 0)) + ((DBL)(right->number != 0))))) != 0); // match VM code
                            break;
                        case OP_MUL:
                            result = (left->number * right->number);
                            break;
                        case OP_DIV:
                            result = (left->number / right->number);
                            break;
                        case OP_AND:
                            result = (DBL)((((DBL)(left->number != 0)) * ((DBL)(right->number != 0)))); // match VM code
                            break;
                        case OP_POW:
                            result = pow(left->number, right->number);
                            break;
                        default:
                            have_result = false;
                            break;
                    }

                    if(have_result == true)
                    {
                        temp = ptr;
                        ptr->prev->next = ptr->next;
                        if(ptr->next != nullptr)
                            ptr->next->prev = ptr->prev;
                        ptr = ptr->prev;
                        POV_FREE(temp->child);
                        POV_FREE(temp);
                        left->number = result;
                    }
                }
            }
        }
        if ((node->next == nullptr) && (node->child != nullptr) && (node->op < OP_FIRST))
        {
            if ((node->child->op == OP_CONSTANT) && (node->child->next == nullptr))
            {
                node->number = left->number;
                node->op = OP_CONSTANT;
                POV_FREE(node->child);
                node->child = nullptr;
            }
        }
    }
    else
    {
        optimise_expr(node->child);

        optimise_call(node);

        if ((node->child != nullptr) && (node->op < OP_FIRST))
        {
            if ((node->child->op == OP_CONSTANT) && (node->child->next == nullptr))
            {
                node->number = node->child->number;
                POV_FREE(node->child);
                node->child = nullptr;
                node->op = OP_CONSTANT;
            }
        }
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   optimise_call
*
* INPUT
*
*   node - node of a function call
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
*   Optimises a function call if it has only constant arguments
*
* CHANGES
*
*   -
*
******************************************************************************/

void Parser::optimise_call(ExprNode *node)
{
    DBL result = 0.0;
    bool have_result = true;;

    if(node->op != OP_CALL)
        return;
    if (node->child == nullptr)
        return;
    if(node->child->op != OP_CONSTANT)
        return;

    switch(node->call.token)
    {
        case SIN_TOKEN:
            result = sin(node->child->number);
            break;
        case COS_TOKEN:
            result = cos(node->child->number);
            break;
        case TAN_TOKEN:
            result = tan(node->child->number);
            break;
        case ASIN_TOKEN:
            result = asin(node->child->number);
            break;
        case ACOS_TOKEN:
            result = acos(node->child->number);
            break;
        case ATAN_TOKEN:
            result = atan(node->child->number);
            break;
        case SINH_TOKEN:
            result = sinh(node->child->number);
            break;
        case COSH_TOKEN:
            result = cosh(node->child->number);
            break;
        case TANH_TOKEN:
            result = tanh(node->child->number);
            break;
        case ASINH_TOKEN:
            result = std::asinh(node->child->number);
            break;
        case ACOSH_TOKEN:
            result = std::acosh(node->child->number);
            break;
        case ATANH_TOKEN:
            result = std::atanh(node->child->number);
            break;
        case ABS_TOKEN:
            result = fabs(node->child->number);
            break;
        case RADIANS_TOKEN:
            result = node->child->number * M_PI / 180.0;
            break;
        case DEGREES_TOKEN:
            result = node->child->number * 180.0 / M_PI;
            break;
        case FLOOR_TOKEN:
            result = floor(node->child->number);
            break;
        case INT_TOKEN:
            result = (int)(node->child->number);
            break;
        case CEIL_TOKEN:
            result = ceil(node->child->number);
            break;
        case SQRT_TOKEN:
            result = sqrt(node->child->number);
            break;
        case EXP_TOKEN:
            result = exp(node->child->number);
            break;
        case LN_TOKEN:
            if(node->child->number > 0.0)
                result = log(node->child->number);
            else
                Error("Domain error in 'ln'.");
            break;
        case LOG_TOKEN:
            if(node->child->number > 0.0)
                result = log10(node->child->number);
            else
                Error("Domain error in 'log'.");
            break;
        case MIN_TOKEN:
            have_result = false;
            break;
        case MAX_TOKEN:
            have_result = false;
            break;
        case ATAN2_TOKEN:
            have_result = false;
            break;
        case POW_TOKEN:
            have_result = false;
            break;
        case MOD_TOKEN:
            have_result = false;
            break;
        case SELECT_TOKEN:
            have_result = false;
            break;
        case FUNCT_ID_TOKEN:
            have_result = false;
            break;
        case VECTFUNCT_ID_TOKEN:
            have_result = false;
            break;
        default:
            have_result = false;
            break;
    }

    if(have_result == true)
    {
        POV_FREE(node->call.name);
        node->number = result;
        node->op = OP_CONSTANT;
        POV_FREE(node->child);
        node->child = nullptr;
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   right_subtree_has_variable_expr
*
* INPUT
*
*   node - root node of the (sub-) tree to search for variables
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
*   Searches an expression tree to determine if it contains any variables
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Parser::right_subtree_has_variable_expr(ExprNode *node)
{
    if (node == nullptr)
        return false;

    for (ExprNode *i = node; i != nullptr; i = i->next)
    {
        if(i->op == OP_VARIABLE)
            return true;

        if (i->child != nullptr)
        {
            if(right_subtree_has_variable_expr(i->child) == true)
                return true;
        }
    }

    return false;
}


/*****************************************************************************
*
* FUNCTION
*
*   left_subtree_has_variable_expr
*
* INPUT
*
*   node - root node of the (sub-) tree to search for variables
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
*   Searches an expression tree to determine if it contains any variables
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Parser::left_subtree_has_variable_expr(ExprNode *node)
{
    if (node == nullptr)
        return false;

    for (ExprNode *i = node; i != nullptr; i = i->prev)
    {
        if(i->op == OP_VARIABLE)
            return true;

        if (i->child != nullptr)
        {
            if(right_subtree_has_variable_expr(i->child) == true)
                return true;
        }
    }

    return false;
}


/*****************************************************************************
*
* FUNCTION
*
*   dump_expr
*
* INPUT
*
*   f - file to dump to
*   node - root node of the (sub-) tree to write to file
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
*   Write an expression tree in inorder notation to a file. Useful for
*   debugging the function parser.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Parser::dump_expr(FILE *f, ExprNode *node)
{
    if(node->op == OP_FIRST)
        fprintf(f, "[");
    else
        fprintf(f, "(");

    fflush(f);

    for (ExprNode *i = node; i != nullptr; i = i->next)
    {
        switch(i->op)
        {
            case OP_CONSTANT:
                fprintf(f, "%f", (float)(i->number));
                break;
            case OP_VARIABLE:
                fprintf(f, "%s", i->variable);
                break;
            case OP_DOT:
                fprintf(f, ".");
                break;
            case OP_MEMBER:
                fprintf(f, "%s", i->variable);
                break;
            case OP_CALL:
                fprintf(f, "fn%d", (int)(i->call.fn));
                break;
            case OP_CMP_EQ:
                fprintf(f, " == ");
                break;
            case OP_CMP_NE:
                fprintf(f, " != ");
                break;
            case OP_CMP_LT:
                fprintf(f, " < ");
                break;
            case OP_CMP_LE:
                fprintf(f, " <= ");
                break;
            case OP_CMP_GT:
                fprintf(f, " > ");
                break;
            case OP_CMP_GE:
                fprintf(f, " >= ");
                break;
            case OP_ADD:
                fprintf(f, " + ");
                break;
            case OP_SUB:
                fprintf(f, " - ");
                break;
            case OP_OR:
                fprintf(f, " | ");
                break;
            case OP_MUL:
                fprintf(f, " * ");
                break;
            case OP_DIV:
                fprintf(f, " / ");
                break;
            case OP_AND:
                fprintf(f, " & ");
                break;
            case OP_POW:
                fprintf(f, " ^ ");
                break;
            case OP_NEG:
                fprintf(f, " -");
                break;
            case OP_NOT:
                fprintf(f, " !");
                break;
            default:
                break;
        }

        fflush(f);

        if (i->child != nullptr)
            dump_expr(f, i->child);
    }

    if(node->op == OP_FIRST)
        fprintf(f, "]");
    else
        fprintf(f, ")");

    fflush(f);
}

}
// end of namespace pov_parser
