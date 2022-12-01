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
#include "expression.h"
#include "generator.h"
#include "lex.h"


extern char* tokenName[tMaxToken];
extern tToken token;
extern FILE* inf;
extern int srcLine;
int prgPass = 1; // syntax/semantic pass number
int level = 0; // nesting level of syntax/semantic analysis
tSymTable gst; // global symbol table
tToken assignId; // latest identifier that can be assigned to
tSymTableItem* actFunc = NULL; // active function if processing function definition body

typedef enum {
    isEmpty, notEmpty
} tStackState;

void insert_embedded_functions(tSymTable* st)
{
    if( (st_insert_function(st, "reads", tTypeString) == 0) ||
        (st_insert_function(st, "readi", tTypeInt) == 0) ||
        (st_insert_function(st, "readf", tTypeFloat) == 0) ||
        (st_insert_function(st, "write", tVoid) == 0) ||
        (st_insert_function(st, "strlen", tTypeInt) == 0) ||
        (st_insert_function(st, "substring", tTypeString) == 0) ||
        (st_insert_function(st, "ord", tTypeInt) == 0) ||
        (st_insert_function(st, "chr", tTypeString) == 0) )
        errorExit("cannot insert embedded functions to symbol table", CERR_INTERNAL);
    if( st_add_params(st, "strlen", tTypeString, "s") == 0 )
        errorExit("cannot insert function parameters to function in symbol table", CERR_INTERNAL);
    if( (st_add_params(st, "substring", tTypeString, "s") == 0) ||
        (st_add_params(st, "substring", tTypeInt, "i") == 0) ||
        (st_add_params(st, "substring", tTypeInt, "j") == 0) )
        errorExit("cannot insert function parameters to function in symbol table", CERR_INTERNAL);
    if (st_add_params(st, "ord", tTypeString, "c") == 0)
        errorExit("cannot insert function parameters to function in symbol table", CERR_INTERNAL);
    if (st_add_params(st, "chr", tTypeInt, "i") == 0)
        errorExit("cannot insert function parameters to function in symbol table", CERR_INTERNAL);
}

void addCodeProlog()
{
    addCode("# emptycode");
    addCode(".IFJcode22");
    addCode("JUMP $$main");
    addCode("");
    addCode("LABEL $$main");
    addCode("CREATEFRAME");
    addCode("PUSHFRAME");
    addCode("DEFVAR LF@%s", expResultName);
    addCode("");
}

void on_stack_state_error(tStack* stack, tStackState cond, char* msg, int errCode)
{
    switch (cond)
    {
    case isEmpty:
        if (tstack_isEmpty(stack))
            errorExit(msg, errCode);
        break;
    case notEmpty:
        if (!tstack_isEmpty(stack))
            errorExit(msg, errCode);
        break;
    default:
        errorExit("unknown stack condition", CERR_INTERNAL);
        break;
    }
}

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
    return; // jen aby nebyl warning, ze se str nic nedelame, kdy mame zakomentovany nasleduji radek
    dbgMsg("%*s%s\n", level*2, "", str);
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
        dbgMsg("PASS 1 - PROLOG OK\n");
    else
        errorExit("Invalid PROLOG", CERR_SYNTAX);
    // preparation
    token.data = safe_malloc(MAX_TOKEN_LEN);
    assignId.data = safe_malloc(MAX_TOKEN_LEN);
    st_init(&gst);
    insert_embedded_functions(&gst);
    actFunc = NULL;
    // first pass
    prgPass = 1;
    nextToken();
    parse_programs();
    //dbgMsg("Global symbol table after the first pass:\n");
    //st_print(&gst);
    // second pass
    prgPass = 2;
    srcLine = 1;
    fseek(inf, 0, SEEK_SET);
    if (SkipProlog(inf))
        dbgMsg("PASS 2 - PROLOG OK\n");
    else
        errorExit("Invalid PROLOG", CERR_SYNTAX);  // tohle by nemelo nastat, kdyz to pri prvnim pruchodu proslo, ale pro jistotu
    addCodeProlog();
    nextToken();
    parse_programs();
    dbgMsg("Global symbol table after the second pass:\n");
    st_print(&gst);

    st_delete_all(&gst);
    free(assignId.data);
    free(token.data);

    dbgMsg("..... code execution .....\n");
    FILE* outf = fopen("temp.ifjcode", "w");
    generateCode(outf);
    fclose(outf);
#if defined(_WIN32) || defined(WIN32)
    system("ic22int temp.ifjcode");
#else
    system("./ic22int temp.ifjcode");
#endif
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
        // function definition
        matchTokenAndNext(tFunction);
        tStack* tmpStack = tstack_init();
        tSymTableItem* fce = NULL;
        tToken tmpToken = { 0,0 };
        tmpToken.data = safe_malloc(MAX_TOKEN_LEN);

        if (prgPass == 1)
        {
            //dbgMsg(">>>>>>>> GST v program - tFunction >>>>>\n");
            //st_print(&gst);
            dbgMsg("function declaration %s ( ", token.data);
            fce = st_insert(&gst, token.data);
            if (fce == NULL)
            {
                errorExit("function already defined", CERR_SEM_FUNC);
                return; // prevention of IntelliSense C6011
            }
            fce->isFunction = 1;
        }
        else
        {
            dbgMsg("function definition %s ( ", token.data);
            fce = st_search(&gst, token.data);
            if (fce == NULL)
            {
                errorExit("function not found in symbol table", CERR_INTERNAL);
                return; // prevention of IntelliSense C6011
            }
        }
        actFunc = fce;

        matchTokenAndNext(tFuncName);
        matchTokenAndNext(tLPar);

        // parse function arguments
        parse_arguments(tmpStack);
        // dbgMsg("- arguments:");
        // tstack_print(tmpStack);
        if (prgPass == 1)
        {
            fce->localST = safe_malloc(sizeof(tSymTable));
            st_init(fce->localST);
            while (!tstack_isEmpty(tmpStack))
            {
                // add parameters to function in symbol table
                if (!tstack_pop(tmpStack, &tmpToken)) // pop type definition
                    errorExit("function definition parameter error", CERR_INTERNAL);
                tTokenType parType = tmpToken.type;
                dbgMsg("%s ", tokenName[parType]);
                if (!tstack_pop(tmpStack, &tmpToken)) // pop var identifier
                    errorExit("function definition parameter error", CERR_INTERNAL);
                dbgMsg("%s ", tmpToken.data);
                st_add_params(&gst, fce->name, parType, tmpToken.data);
            }
        }
        else
        {
            tstack_deleteItems(tmpStack);
        }
        on_stack_state_error(tmpStack, notEmpty, "stack should be empty after processsing arguments", CERR_INTERNAL);

        matchTokenAndNext(tRPar);
        matchTokenAndNext(tColon);

        // parse function return type
        parse_type(tmpStack);
        if (prgPass == 1)
        {
            dbgMsg(") : ");
            // tstack_print(tmpStack);
            if (!tstack_pop(tmpStack, &tmpToken))
                errorExit("function definition parameter error", CERR_INTERNAL);
            fce->dataType = tmpToken.type;
            if (!tstack_isEmpty(tmpStack))
                errorExit("stack should be empty after processsing function type", CERR_INTERNAL);
            dbgMsg("%s\n", tokenName[fce->dataType]);
        }
        else
        {
            tstack_deleteItems(tmpStack);
            dbgMsg(") : ");
            dbgMsg("%s {\n", tokenName[fce->dataType]);
        }

        matchTokenAndNext(tLCurl);
        // parse function statements
        if (prgPass == 1)
            parse_statements(NULL); // do not care about function body statements on the 1st pass
        else
            parse_statements(fce->localST);
        // dbgMsg("Function local symbol table:\n");
        // st_print(fce->localST);
        // free(fce->localST); neuvolnovat, jeste ji budou potrebovat dalsi
        matchTokenAndNext(tRCurl);

        if (prgPass == 2)
        {
            fce->isDefined = 1;
            if ((fce->dataType != tVoid) && (fce->hasReturn == 0))
                errorExit("missing return statement in non void function", CERR_SEM_RET);
            dbgMsg("} end function definition\n");

        }

        actFunc = NULL;
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

    tStack* tmpStack = tstack_init();
    tTokenType expType = tNone;
    char* funcName = safe_malloc(MAX_TOKEN_LEN);

    switch (token.type)
    {
    case tIf:
        dbgMsg("IF ");
        matchTokenAndNext(tIf);
        matchTokenAndNext(tLPar);
        parse_expression(tmpStack);
        if (prgPass == 1)
        {
            tstack_deleteItems(tmpStack);
        }
        else
        {
            dbgMsg(" ( ");
            expType = evalExp(tmpStack, st);
            dbgMsg(" ) ");
        }
        on_stack_state_error(tmpStack, notEmpty, "stack should be empty after processsing assignment expression", CERR_INTERNAL);
        matchTokenAndNext(tRPar);
        matchTokenAndNext(tLCurl);
        dbgMsg(" {\n");
        parse_statements(st);
        dbgMsg("} ");
        matchTokenAndNext(tRCurl);
        dbgMsg("ELSE ");
        matchTokenAndNext(tElse);
        matchTokenAndNext(tLCurl);
        dbgMsg("{\n");
        parse_statements(st);
        dbgMsg("} END IF\n");
        matchTokenAndNext(tRCurl);
        break;

    case tWhile:
        dbgMsg("WHILE ");
        matchTokenAndNext(tWhile);
        matchTokenAndNext(tLPar);
        parse_expression(tmpStack);
        if (prgPass == 1)
        {
            tstack_deleteItems(tmpStack);
        }
        else
        {
            dbgMsg(" ( ");
            expType = evalExp(tmpStack, st);
            dbgMsg(" ) ");
        }
        on_stack_state_error(tmpStack, notEmpty, "stack should be empty after processsing assignment expression", CERR_INTERNAL);
        matchTokenAndNext(tRPar);
        matchTokenAndNext(tLCurl);
        dbgMsg(" {\n");
        parse_statements(st);
        dbgMsg("} end WHILE\n");
        matchTokenAndNext(tRCurl);
        break;

    case tFuncName:
        // function call
        strcpy(funcName, token.data);
        matchTokenAndNext(tFuncName);
        matchTokenAndNext(tLPar);
        parse_parameters(tmpStack);
        if (prgPass == 1)
        {
            tstack_deleteItems(tmpStack);
        }
        else
        {
            tSymTableItem* sti = st_search(&gst, funcName);
            tToken tmpToken = { 0,0 };
            tmpToken.data = safe_malloc(MAX_TOKEN_LEN);
            if (sti == NULL)
            {
                // errorExit("function not defined", CERR_SEM_FUNC);
                // return;
                // function call not in symbol table -> forward function declartation
                dbgMsg("forward function declaration %s ( ", funcName);
                sti = st_insert(&gst, funcName);
                if (sti == NULL)
                {
                    errorExit("forward function declaration cannot be inserted to symbol table", CERR_INTERNAL);
                    return; // tohle je jen kvuli tomu, aby nerval IntelliSense o kousek dal na fce NULL, sem se to nikdy nedostane, protoze exit na error v prechozim radku
                }
                sti->isFunction = 1;
                // tstack_print(tmpStack);
                sti->localST = safe_malloc(sizeof(tSymTable));
                st_init(sti->localST);
                tStackItem* stackItem = tmpStack->top;
                while (stackItem != NULL)
                {
                    // add parameters to function in symbol table
                    dbgMsg("%s ", tokenName[stackItem->token.type]);
                    dbgMsg("%s ", stackItem->token.data);
                    st_add_params(&gst, sti->name, st_get_type(st, stackItem->token.data), stackItem->token.data);
                    stackItem = stackItem->next;
                }
                dbgMsg(" )\n");
            }
            dbgMsg("function call %s (", funcName);
            if (strcmp(sti->name, "write") == 0)
            {
                // specialni sekce pro funkci write, ktera ma libovolny pocet parametru
                while (!tstack_isEmpty(tmpStack))
                {
                    if (!tstack_pop(tmpStack, &tmpToken))
                        errorExit("stack error processing function parameters", CERR_INTERNAL);
                    dbgMsg(" %s", tmpToken.data);
                    char c[255], tmpStr[255];
                    strcpy(c, "WRITE ");
                    switch (tmpToken.type) {
                    case tLiteral:
                        strcat(c, ifjCodeStr(tmpStr, tmpToken.data));
                        break;
                    case tInt:
                    case tInt2:
                        {
                            int tmpi;
                            // tmpi = atoi(tmpToken.data);
                            if (sscanf(tmpToken.data, "%d", &tmpi) != 1)
                                errorExit("wrong integer constant", CERR_INTERNAL);
                            strcat(c, ifjCodeInt(tmpStr, tmpi));
                        }
                        break;
                    case tReal:
                    case tReal2:
                        {
                            double tmpd;
                            if (sscanf(tmpToken.data, "%lf", &tmpd) != 1)
                                errorExit("wrong integer constant", CERR_INTERNAL);
                            strcat(c, ifjCodeReal(tmpStr, tmpd));
                        }
                        break;
                    case tIdentifier:
                        strcat(c, "LF@");
                        strcat(c, tmpToken.data);
                        break;
                    default:
                        errorExit("unknow write() parameter", CERR_INTERNAL);
                        break;
                    }
                    addCode(c);

                }
            }
            else
            {
                // zkontrolujeme parametry volane funkce s nadefinovanou v global symbol table
                if (tstack_count(tmpStack) != st_nr_func_params(&gst, sti->name))
                    errorExit("wrong number of function parameters", CERR_SEM_ARG);
                tFuncParam* param = sti->params;
                while (!tstack_isEmpty(tmpStack))
                {
                    if (!tstack_pop(tmpStack, &tmpToken))
                        errorExit("stack error processing function parameters", CERR_INTERNAL);
                    tTokenType typ;
                    if ((tmpToken.type >= tInt) && (tmpToken.type <= tLiteral))
                        typ = const2type(tmpToken.type);
                    else
                        typ = st_get_type(st, tmpToken.data);
                    if (param->dataType != typ)
                    {
                        char msg[100];
                        sprintf(msg, "function argument '%s' type %s does not match declaration type %s", tmpToken.data, tokenName[typ], tokenName[param->dataType]);
                        errorExit(msg, CERR_SEM_ARG);
                    }
                    param = param->next;
                    if (!tstack_isEmpty(tmpStack) && param == NULL)
                    {
                        errorExit("less function parameters in symbol table than expected", CERR_INTERNAL);
                        return;
                    }
                    dbgMsg(" %s", tmpToken.data);

                }
            }
            dbgMsg(" )\n");
            free(tmpToken.data);
        }
        on_stack_state_error(tmpStack, notEmpty, "stack should be empty after processsing function arguments", CERR_INTERNAL);

        matchTokenAndNext(tRPar);
        matchTokenAndNext(tSemicolon);
        break;

    case tReturn:
        // return from function
        matchTokenAndNext(tReturn);
        if (prgPass == 2)
            dbgMsg("return with ");
        if (token.type == tFuncName)
        {
            // vracime volani funkce
            char fname[MAX_TOKEN_LEN];
            strcpy(fname, token.data);
            parse_statement(st); // vleze primo do functionCall
            //dbgMsg(">>>>>>>> GST v statment - tReturn >>>>>\n");
            //st_print(&gst);
            if (prgPass == 2)
            {
                tSymTableItem* fce = st_search(&gst, fname);
                if (actFunc != NULL) // check return type if in function definition body
                {
                    if (actFunc->dataType != fce->dataType)
                        errorExit("wrong data type in return statement", CERR_SEM_ARG);
                    actFunc->hasReturn++;
                }

            }
        }
        else
        {
            // vracime vyraz
            parse_returnValue(tmpStack);
            // tstack_print(tmpStack);
            if (prgPass == 1)
            {
                tstack_deleteItems(tmpStack);
            }
            else
            {
                dbgMsg(" expression [ ");
                int cnt = tstack_count(tmpStack);
                expType = evalExp(tmpStack, st);
                dbgMsg(" ] : %s\n", tokenName[expType]);
                on_stack_state_error(tmpStack, notEmpty, "stack should be empty after processsing return expression", CERR_INTERNAL);
                // st_print(&gst);
                if (actFunc != NULL) // check return type if in function definition body
                {
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
                    actFunc->hasReturn++;
                }
            }
            matchTokenAndNext(tSemicolon);
        }
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

    free(funcName);
    tstack_free(&tmpStack);
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
    char tmpStr[MAX_IFJC_LEN];

    switch (token.type)
    {
        case tAssign:
            matchTokenAndNext(tAssign);
            tStack* tmpStack = tstack_init();
            if (prgPass == 2)
                dbgMsg("Assignment %s = ", assignId.data);
            tTokenType expType = tNone;
            if (token.type == tFuncName)
            {
                if (prgPass == 1)
                {
                    parse_statement(NULL);
                }
                else
                {
                    char fname[MAX_TOKEN_LEN];
                    strcpy(fname, token.data);
                    parse_statement(st);
                    //dbgMsg(">>>>>>>> GST v nextTermina - tAssign function >>>>>\n");
                    //st_print(&gst);
                    tSymTableItem* fce = st_search(&gst, fname);
                    expType = fce->dataType;
                    //sprintf(tmpStr, "DEFVAR LF@%s", sti->name);
                    //addCode(tmpStr);
                }
            }
            else
            {
                parse_expression(tmpStack);
                if (prgPass == 1)
                {
                    tstack_deleteItems(tmpStack);
                }
                else
                {
                    on_stack_state_error(tmpStack, isEmpty, "assignment with an empty expression", CERR_SYNTAX);
                    // tstack_print(tmpStack);
                    tSymTableItem* sti = st_search(st, assignId.data);
                    if (sti == NULL)
                    { // variable not found in symbol table yet, so it shoold be defined in code too
                        sprintf(tmpStr, "DEFVAR LF@%s", assignId.data);
                        addCode(tmpStr);
                    }
                    dbgMsg("expression [ ");
                    //int cnt = tstack_count(tmpStack);
                    expType = evalExp(tmpStack, st);
                    addCode("MOVE LF@%s TF@%s", assignId.data, expResultName);
                    on_stack_state_error(tmpStack, notEmpty, "stack should be empty after processsing assignment expression", CERR_INTERNAL);
                    dbgMsg(" ]");
                    matchTokenAndNext(tSemicolon);
                    //dbgMsg(">>>>>>>> GST v nextTerminal - tAssign expression >>>>>\n");
                    //st_print(&gst);
                }
            }
            if (prgPass == 2)
            {
                dbgMsg(" : %s\n", tokenName[expType]);
                // insert or find the symbol in symbol table
                tSymTableItem* sti = st_searchinsert(st, assignId.data);
                if (sti == NULL)
                {
                    errorExit("assignment variable cannot be inserted nor found in the symbol table", CERR_INTERNAL);
                    return; // prevent C6011
                }
                sti->dataType = expType;
            }
            tstack_free(&tmpStack);
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
