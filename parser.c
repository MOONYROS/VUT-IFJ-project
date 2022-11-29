//
//  parser.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 02.11.2022.
//
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "support.h"
#include "token.h"
#include "lex.h"
#include "tstack.h"
#include "symtable.h"
#include "expression.h"

int level = 0;
tSymTable gst;
tToken assignId;
tSymTableItem* actFunc = NULL;

int ReadTokenPRINT(FILE *f, tToken *token)
{
    int ret;
    ret = ReadToken(f, token);
#if DEBUG_MSG == 11
    if(token->type == tInvalid)
        dbgMsg("INVALID: <<%s>>\n",token->data);
    else
        dbgMsg("TOKEN %s: <<%s>>\n", tokenName[token->type], token->data);
#endif
    return ret;
}

tTokenType strToToken(const char *tokenStr)
{
    for (int t = tTypeInt; t <= tEpilog; t++) {
        if (strcmp(tokenStr, tokenName[t]) == 0)
            return t;
    }
    return tInvalid;
}

void prl(char* str)
{
    level++;
    dbgMsg("%*s%s\n", level*2, "", str);
    return; // jen aby nebyl warning, ze se str nic nedelame, kdy mame zakomentovany nasleduji radek
}

void nextToken()
{
    ReadTokenPRINT(inf, &token);
    if (token.type == tInvalid)
        errorExit("invalid token", CERR_LEX);
}

void matchTokenAndNext(tTokenType tokType)
{
    if (token.type != tokType)
        errorExit("unexpected token", CERR_SYNTAX);
    nextToken();
}

void parse()
{
    if (SkipProlog(inf))
        dbgMsg("PROLOG OK\n");
    else
        errorExit("Invalid PROLOG", CERR_SYNTAX);
    token.data = safe_malloc(MAX_TOKEN_LEN);
    assignId.data = safe_malloc(MAX_TOKEN_LEN);
    st_init(&gst);
    nextToken();
    parse_programs();
    dbgMsg("Global symbol table:\n");
    st_print(&gst);
    free(assignId.data);
    free(token.data);

}

void parse_programs()
{
    prl("programs");

    switch (token.type)
    {
    case tEpilog:
        break;

    case tFunction:
    case tIf:
    case tWhile:
    case tFuncName:
    case tReturn:
    case tSemicolon:
    case tIdentifier:
    case tLPar:
    case tInt:
    case tReal:
    case tReal2:
    case tInt2:
    case tNull:
    case tLiteral:
        parse_program();
        parse_programs();
        break;

    default:
        // epsilon
        parse_program();
        parse_programs();
        break;
    }

    level--;
}

void parse_program()
{
    prl("program");

    switch (token.type)
    {
    case tFunction:
        // function declaration
        matchTokenAndNext(tFunction);
        dbgMsg("function declaration %s ( ", token.data);
        tStack* tmpStack = tstack_init();
        tSymTableItem* fce = st_insert(&gst, token.data);
        actFunc = fce;
        if (fce == NULL)
        {
            errorExit("function already declared", CERR_SEM_FUNC);
            return; // tohle je jen kvuli tomu, aby nerval IntelliSense o kousek dal na fce NULL, sem se to nikdy nedostane, protoze exit na error v prechozim radku
        }
        tToken tmpToken = {0,0};
        tmpToken.data = safe_malloc(MAX_TOKEN_LEN);

        fce->isFunction = 1;
        matchTokenAndNext(tFuncName);
        matchTokenAndNext(tLPar);
        // parse function arguments
        parse_arguments(tmpStack);
        // dbgMsg("- arguments:");
        // tstack_print(tmpStack);
        fce->localST = safe_malloc(sizeof(tSymTable));
        st_init(fce->localST);
        while (!tstack_isEmpty(tmpStack))
        {
            // add parameters to fucntion in symbol table
            tFuncParam* funcPar = safe_malloc(sizeof(tFuncParam));
            if (!tstack_pop(tmpStack, &tmpToken))
                errorExit("function declaration parameter error", CERR_INTERNAL);
            funcPar->dataType = tmpToken.type;
            dbgMsg("%s ", tokenName[funcPar->dataType]);
            if (!tstack_pop(tmpStack, &tmpToken))
                errorExit("function declaration parameter error", CERR_INTERNAL);
            funcPar->name = safe_malloc(strlen(tmpToken.data) + 1);
            strcpy(funcPar->name, tmpToken.data);
            dbgMsg("%s ", funcPar->name);
            funcPar->next = NULL; 
            if (fce->params==NULL)
            {
                fce->params = funcPar;
            }
            else
            {
                tFuncParam* last = fce->params;
                while (last->next != NULL)
                    last = last->next;
                last->next = funcPar;
            }
            // funcPar->next = fce->params;
            // fce->params = funcPar;
            
            // add parameter as local variable to function's symbol table
            tSymTableItem* sti = st_insert(fce->localST, funcPar->name);
            if(sti==NULL)
            {
                errorExit("duplicate function parameter", CERR_SEM_OTHER);
                return; // prevent C6011
            }
            sti->dataType = funcPar->dataType;
        }
        if(!tstack_isEmpty(tmpStack))
            errorExit("stack should be empty after processsing arguments", CERR_INTERNAL);
        matchTokenAndNext(tRPar);
        matchTokenAndNext(tColon);
        // parse function return type
        parse_type(tmpStack);
        dbgMsg(") : ");
        // tstack_print(tmpStack);
        if (!tstack_pop(tmpStack, &tmpToken))
            errorExit("function declaration parameter error", CERR_INTERNAL);
        fce->dataType = tmpToken.type;
        if (!tstack_isEmpty(tmpStack))
            errorExit("stack should be empty after processsing function type", CERR_INTERNAL);
        dbgMsg("%s {\n", tokenName[fce->dataType]);
        matchTokenAndNext(tLCurl);
        // parse function statements
        parse_statements(fce->localST);
        // dbgMsg("Function local symbol table:\n");
        // st_print(fce->localST);
        // free(fce->localST); neuvolnovat, jeste ji budou potrebovat dalsi

        matchTokenAndNext(tRCurl);

        dbgMsg("} end function declaration\n");
        actFunc = fce;  // uz jsme mimo funcki
        fce->isDefined = 1; // a ta byla timto plne nadefinovana
        free(tmpToken.data);
        tstack_free(&tmpStack);
        break;

    case tIf:
    case tWhile:
    case tFuncName:
    case tReturn:
    case tSemicolon:
    case tIdentifier:
    case tLPar:
    case tInt:
    case tReal:
    case tReal2:
    case tInt2:
    case tNull:
    case tLiteral:
        parse_statement(&gst);
        break;

    default:
        errorExit("unexpected token in program", CERR_SYNTAX);
        break;
    }

    level--;
}

void parse_statements(tSymTable *st)
{
    prl("statements");

    switch (token.type)
    {
    case tIf:
    case tWhile:
    case tFuncName:
    case tReturn:
    case tSemicolon:
    case tIdentifier:
    case tLPar:
    case tInt:
    case tReal:
    case tReal2:
    case tInt2:
    case tNull:
    case tLiteral:
        parse_statement(st);
        parse_statements(st);
        break;

    default:
        // epsilon
        break;
    }

    level--;
}

void parse_statement(tSymTable* st)
{
    prl("statement");

    switch (token.type)
    {
    case tIf:
        dbgMsg("IF statement\n");
        matchTokenAndNext(tIf);
        matchTokenAndNext(tLPar);
        parse_expression(NULL);
        matchTokenAndNext(tRPar);
        matchTokenAndNext(tLCurl);
        parse_statements(st);
        matchTokenAndNext(tRCurl);
        matchTokenAndNext(tElse);
        matchTokenAndNext(tLCurl);
        parse_statements(st);
        matchTokenAndNext(tRCurl);
        break;

    case tWhile:
        dbgMsg("WHILE statement\n");
        matchTokenAndNext(tWhile);
        matchTokenAndNext(tLPar);
        parse_expression(NULL);
        matchTokenAndNext(tRPar);
        matchTokenAndNext(tLCurl);
        parse_statements(st);
        matchTokenAndNext(tRCurl);
        break;

    case tFuncName:
        // function call
        dbgMsg("function call %s\n", token.data);
        matchTokenAndNext(tFuncName);
        matchTokenAndNext(tLPar);
        parse_parameters(NULL);
        matchTokenAndNext(tRPar);
        matchTokenAndNext(tSemicolon);
        break;

    case tReturn:
        // return from function
        matchTokenAndNext(tReturn);
        tStack* tmpStack = tstack_init();
        parse_returnValue(tmpStack);
        dbgMsg("return with ");
        // tstack_print(tmpStack);
        dbgMsg(" expression [ ");
        int cnt = tstack_count(tmpStack);
        tTokenType expType = evalExp(tmpStack, &gst);
        dbgMsg(" ] : %s\n", tokenName[expType]);
        if (!tstack_isEmpty(tmpStack))
            errorExit("stack should be empty after processsing return expression", CERR_INTERNAL);
        // tSymTableItem* sti = st_search(&gst, actFunc->name); // bych to vubec nemusel asi hledeat ;-) - do sti da actFunc - tohle jeste promyslet, muze se hodit dal
        // st_print(&gst);
        if (cnt == 0)
        {   // navrat bez paramtetru
            if (actFunc->dataType != tVoid)
                errorExit("missing value in return statement", CERR_SEM_RET);
        }
        else
        {
            if (actFunc->dataType == tVoid)
                errorExit("void function returning value", CERR_SEM_RET);
            else if (actFunc->dataType != expType)
                errorExit("wrong data type in return statement", CERR_SEM_ARG);
        }
        tstack_free(&tmpStack);
        matchTokenAndNext(tSemicolon);
        break;

    case tSemicolon:
        matchTokenAndNext(tSemicolon);
        break;

    case tIdentifier:
        // dbgMsg("Asi bude prirazeni do %s\n", token.data);
        assignId.type = token.type;
        strcpy(assignId.data, token.data);
        matchTokenAndNext(tIdentifier);
        parse_nextTerminal(st);
        break;

    case tLPar:
    case tInt:
    case tReal:
    case tReal2:
    case tInt2:
    case tNull:
    case tLiteral:
        parse_preExpression();
        break;

    default:
        errorExit("unexpected token in statement", CERR_SYNTAX);
        break;
    }

    level--;
}

void parse_returnValue(tStack* stack)
{
    prl("returnValue");

    switch (token.type)
    {
    case tLPar:
    case tIdentifier:
    case tInt:
    case tReal:
    case tReal2:
    case tInt2:
    case tNull:
    case tLiteral:
        parse_expression(stack);
        break;

    default:
        // epsilon
        break;
    }

    level--;
}


void parse_nextTerminal(tSymTable* st)
{
    prl("nextTerminal");

    switch (token.type)
    {
        case tAssign:
            matchTokenAndNext(tAssign);
            tStack* tmpStack = tstack_init();
            dbgMsg("Assignment %s = ", assignId.data);
            parse_expression(tmpStack);
            if (tstack_isEmpty(tmpStack))
                errorExit("assignment with an empty expression", CERR_SYNTAX);
            // tstack_print(tmpStack);
            dbgMsg("expression [ ");
            //int cnt = tstack_count(tmpStack);
            tTokenType expType = evalExp(tmpStack, st);
            dbgMsg(" ] : %s\n", tokenName[expType]);
            if (!tstack_isEmpty(tmpStack))
                errorExit("stack should be empty after processsing assignment expression", CERR_INTERNAL);
            // insert or find the symbol in symbol table
            tSymTableItem* sti = st_insert(st, assignId.data);
            if(sti==NULL)
            {
                // could not be inserted, so let us find it fo redefinition
                sti = st_search(st, assignId.data);
                if (sti == NULL)
                {
                    errorExit("assignment variable cannot be inserted nor found in the symbol table", CERR_INTERNAL);
                    return; // prevent C6011
                }
            }
            sti->dataType = expType;
            matchTokenAndNext(tSemicolon);
            break;
    
        case tPlus:
        case tMinus:
        case tMul:
        case tDiv:
        case tConcat:
        case tLess:
        case tLessEq:
        case tMore:
        case tMoreEq:
        case tIdentical:
        case tNotIdentical:
            parse_expression2(NULL);
            matchTokenAndNext(tSemicolon);
            break;

        default:
            // epsilon
            parse_expression2(NULL);
            matchTokenAndNext(tSemicolon);
            break;
    }

    level--;
}

void parse_preExpression()
{
    prl("preExpresssion");

    switch (token.type)
    {
        case tLPar:
            matchTokenAndNext(tLPar);
            parse_const(NULL);
            parse_expression2(NULL);
            matchTokenAndNext(tRPar);
            matchTokenAndNext(tSemicolon);
            break;

        case tInt:
        case tReal:
        case tReal2:
        case tInt2:
        case tNull:
        case tLiteral:
            parse_const(NULL);
            parse_expression2(NULL);
            matchTokenAndNext(tSemicolon);
            break;

        default:
            errorExit("unexpected token in expression", CERR_SYNTAX);
            break;
    }

    level--;
}

void parse_expression(tStack* stack)
{
    prl("expression");

    switch (token.type)
    {
        case tLPar:
            matchTokenAndNext(tLPar);
            parse_expression(stack);
            matchTokenAndNext(tRPar);
            parse_expression2(stack);
            break;

        case tIdentifier:
        case tInt:
        case tReal:
        case tReal2:
        case tInt2:
        case tNull:
        case tLiteral:
            parse_term(stack);
            parse_expression2(stack);
            break;

        default:
            //errorExit("unexpected token", CERR_SYNTAX);
            break;
    }

    level--;
}

void parse_expression2(tStack* stack)
{
    prl("expression2");

    switch (token.type)
    {
        case tPlus:
        case tMinus:
        case tMul:
        case tDiv:
        case tConcat:
        case tLess:
        case tLessEq:
        case tMore:
        case tMoreEq:
        case tIdentical:
        case tNotIdentical:
            tstack_pushl(stack, token);
            nextToken();
            parse_expression(stack);
            break;

        default:
            // epsilon
            break;
    }

    level--;
}

void parse_arguments(tStack* stack)
{
    prl("arguments");
    
    switch (token.type)
    {
    case tNullTypeInt:
    case tNullTypeFloat:
    case tNullTypeString:
    case tTypeInt:
    case tTypeFloat:
    case tTypeString:
    case tVoid:
        parse_type(stack);
        tstack_pushl(stack, token);
        matchTokenAndNext(tIdentifier);
        parse_argumentVars(stack);
        break;

    default:
        // epsilon
        break;
    }

    level--;
}

void parse_argumentVars(tStack* stack)
{
    prl("argumentVars");

    switch (token.type)
    {
    case tComma:
        matchTokenAndNext(tComma);
        parse_type(stack);
        tstack_pushl(stack, token);
        matchTokenAndNext(tIdentifier);
        parse_argumentVars(stack);
        break;

    default:
        // epsilon
        break;
    }

    level--;
}

void parse_parameters(tStack* stack)
{
    prl("parameters");

    switch (token.type)
    {
    case tIdentifier:
    case tInt:
    case tReal:
    case tReal2:
    case tInt2:
    case tNull:
    case tLiteral:
        parse_term(stack);
        parse_parameters2(stack);
        break;

    default:
        // epsilon
        break;
    }

    level--;
}

void parse_parameters2(tStack* stack)
{
    prl("paramaters2");

    switch (token.type)
    {
    case tComma:
        matchTokenAndNext(tComma);
        parse_term(stack);
        parse_parameters2(stack);
        break;

    default:
        // epsilon
        break;
    }

    level--;
}

void parse_term(tStack* stack)
{
    prl("term");

    switch (token.type)
    {
        case tInt:
        case tReal:
        case tReal2:
        case tInt2:
        case tNull:
        case tLiteral:
            parse_const(stack);
            break;

        case tIdentifier:
            tstack_pushl(stack, token);
            matchTokenAndNext(tIdentifier);
            break;

        default:
            errorExit("unexpected token in term", CERR_SYNTAX);
            break;
    }

    level--;
}

void parse_const(tStack* stack)
{
    prl("const");

    switch (token.type)
    {
        case tInt:
        case tReal:
        case tReal2:
        case tInt2:
        case tNull:
        case tLiteral:
            tstack_pushl(stack, token);
            nextToken();
            break;

        default:
            errorExit("const expected", CERR_SYNTAX);
            break;
    }

    level--;
}

void parse_type(tStack* stack)
{
    prl("type");

    switch (token.type)
    {
        case tNullTypeInt:
        case tNullTypeFloat:
        case tNullTypeString:
        case tTypeInt:
        case tTypeFloat:
        case tTypeString:
        case tVoid:
            tstack_pushl(stack, token);
            nextToken();
            break;

        default:
            errorExit("type expected", CERR_SYNTAX);
            break;
    }

    level--;
}
