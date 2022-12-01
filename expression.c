//
//  expression.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expression.h"
#include "support.h"
#include "generator.h"


char prdTable[15][15] = {
//    *   /   +   -   .   <   >  <=  >=  === !==  (   )  id   $
    {'>','>','>','>','>','>','>','>','>','>','>','<','>','<','>'},  // *
    {'>','>','>','>','>','>','>','>','>','>','>','<','>','<','>'},  // /   
    {'<','<','>','>','>','>','>','>','>','>','>','<','>','<','>'},  // +
    {'<','<','>','>','>','>','>','>','>','>','>','<','>','<','>'},  // -
    {'<','<','>','>','>','>','>','>','>','>','>','<','>','<','>'},  // .
    {'<','<','<','<','<','>','>','>','>','>','>','<','>','<','>'},  // <
    {'<','<','<','<','<','>','>','>','>','>','>','<','>','<','>'},  // >
    {'<','<','<','<','<','>','>','>','>','>','>','<','>','<','>'},  // <=
    {'<','<','<','<','<','>','>','>','>','>','>','<','>','<','>'},  // >=
    {'<','<','<','<','<','<','<','<','<','>','>','<','>','<','>'},  // ===
    {'<','<','<','<','<','<','<','<','<','>','>','<','>','<','>'},  // !==
    {'<','<','<','<','<','<','<','<','<','<','<','<','=','<','x'},  // (
    {'>','>','>','>','>','>','>','>','>','>','>','x','>','x','>'},  // )
    {'>','>','>','>','>','>','>','>','>','>','>','x','>','x','>'},  // id
    {'<','<','<','<','<','<','<','<','<','<','<','<','x','<','x'}   // $
};

bool isOperator(tToken *token)
{
    return (token->type == tMul || token->type == tDiv || token->type == tPlus || token->type == tMinus || token->type == tLess || token->type == tMore ||
        token->type == tLessEq || token->type == tMoreEq || token->type == tIdentical || token->type == tNotIdentical || token->type == tConcat);
}

bool isNumberOp(tToken *token)
{
    return (token->type == tMul || token->type == tDiv || token->type == tPlus || token->type == tMinus);
}

bool isRelationalOp(tToken *token)
{
    return (token->type == tLess || token->type == tMore || token->type == tLessEq || token->type == tMoreEq || token->type == tIdentical || token->type == tNotIdentical);
}

bool isStringOp(tToken *token)
{
    return (token->type == tIdentical || token->type == tNotIdentical || token->type == tConcat);
}

bool isConst(tToken *token)
{
    return (token->type == tInt || token->type == tInt2 || token->type == tReal || token->type == tReal2 || token->type == tLiteral);
}

bool isVar(tToken *token)
{
    return token->type == tIdentifier; 
}

bool isNumber(tSymTable *table, tToken *token)
{
    if (isVar(token))
    {
        if (!isDefined(table, token))
        {
            errorExit("Semantic error: Undefined variable.\n", CERR_SEM_UNDEF);
            return false;
        }
        return (variableType(table, token) == tTypeInt || variableType(table, token) == tTypeFloat || \
            variableType(table, token) == tNullTypeInt || variableType(table, token) == tNullTypeFloat);
    }
    else 
        return (token->type == tInt || token->type == tInt2 || token->type == tReal || token->type == tReal2);
}

bool isString(tSymTable *table, tToken *token)
{   
    if (isVar(token))
    {
        if (!isDefined(table, token))
        {
            errorExit("Semantic error: Undefined variable.\n", CERR_SEM_UNDEF);
            return false;
        }
        return (variableType(table, token) == tTypeString || variableType(table, token) == tNullTypeString);
    }
    else 
        return token->type == tLiteral;
}

bool isReal(tSymTable *table, tToken *token)
{
    if (isVar(token))
        return (variableType(table, token) == tTypeFloat || variableType(table, token) == tNullTypeFloat);
    else 
        return (token->type == tReal || token->type == tReal2);
}

bool isInt(tSymTable *table, tToken *token)
{
    if (isVar(token))
        return (variableType(table, token) == tTypeInt || variableType(table, token) == tNullTypeInt);
    else 
        return (token->type == tInt || token->type == tInt2);
}

tTokenType variableType(tSymTable *table, tToken *token)
{
    tSymTableItem *item = st_search(table, token->data);
    return item->dataType;
}

bool isDefined(tSymTable *table, tToken *token)
{
    tSymTableItem *item = st_search(table, token->data);
    return item->isDefined;
}

// This should be a redundant function thanks to isNumber and isString.
bool checkOpDefinition(tSymTable *table, tToken *third, tToken *top)
{
    tSymTableItem *item;
    if (isVar(top))
    {
        item = st_search(table, top->data);
        if (!item->isDefined)
            return false;
    }
    if (isVar(third))
    {
        item = st_search(table, third->data);
        if (!item->isDefined)
            return false;
    }
    return true;
}

bool isNull(tSymTable *table, tToken *token)
{
    tSymTableItem *item = st_search(table, token->data);
    return item->dataType == tNull;
}

tTokenType getResultType(tSymTable *table, tToken *top, tToken *third, tTokenType operation)
{
    switch (operation)
    {
        case tPlus:
        case tMinus:
        case tMul:
        case tDiv:
            if (isNull(table, top) || isNull(table, third))
                errorExit("Semantic error: NULL operand in arithmetic operation.\n", CERR_SEM_TYPE);
            
            if (isReal(table, top) && isReal(table, third))
                return tReal;
            else if (isInt(table, top) && isInt(table, third))
                return tInt;
            else 
                errorExit("Semantic error: Operands have to be the same type when performing arithmetic operation.\n", CERR_SEM_TYPE);
            return tNone;
            
        case tConcat:
            return tLiteral;

        case tLess:
        case tLessEq:
        case tMore:
        case tMoreEq:
        case tIdentical:
        case tNotIdentical:

            return tInt;

        default:
            // Semka bysme se asi nemeli nikdy dostat.
            return tNone;
    }
}

void rearrangeStack(tStack *stack)
{
    tStackItem *tmp = stack->top;

    tToken zero = {tInt, NULL};
    zero.data = safe_malloc(sizeof("0"));
    zero.data = "0";
    tToken leftPar = {tLPar, NULL};
    leftPar.data = safe_malloc(sizeof("("));
    leftPar.data = "(";
    tToken rightPar = {tRPar, NULL};
    rightPar.data = safe_malloc(sizeof(")"));
    rightPar.data = ")";

    if (stack->top->token.type == tMinus)
    {
        tstack_push(stack, zero);
        tstack_push(stack, leftPar);
        tmp = tmp->next;
        tstack_insertAfter(stack, tmp, rightPar);
    }

    tStackItem *operator;
    while (tmp != NULL)
    {
        if (isNumberOp(&tmp->token) || tmp->token.type == tLPar)
        {
            operator = tmp;
            tmp = tmp->next;
            if (tmp->token.type == tMinus)
            {
                tstack_insertAfter(stack, operator, leftPar);
                operator = operator->next;
                tstack_insertAfter(stack, operator, zero);
                for (int i = 0; i < 3; i++)
                    operator = operator->next;
                tstack_insertAfter(stack, operator, rightPar);
            }
        }
        tmp = tmp->next;
    }
}

double convertToDouble(tToken * token)
{
    double tmp;
    sscanf(token->data, "%lf", &tmp);
    return tmp;
}

int convertToInt(tToken * token)
{
    int tmp;
    sscanf(token->data, "%d", &tmp);
    return tmp;
}

int typeToIndex(tTokenType tokenType)
{
    switch (tokenType)
    {
        case tMul:
            return 0;
        case tDiv:
            return 1;
        case tPlus:
            return 2;
        case tMinus:
            return 3;
        case tConcat:
            return 4;
        case tLess:
            return 5;
        case tMore:
            return 6;
        case tLessEq:
            return 7;
        case tMoreEq:
            return 8;
        case tIdentical:
            return 9;
        case tNotIdentical:
            return 10;
        case tLPar:
            return 11;
        case tRPar:
            return 12;
        case tIdentifier:
            return 13;
        default:
            // Sem bychom se nikdy nemeli dostat
            return 14;
    }
}


tTokenType const2type(tTokenType ctype)
{
   tTokenType typ = tNone;
    switch (ctype)
    {
    case tInt:
        typ = tTypeInt;
        break;
    case tInt2:
        typ = tTypeInt;
        break;
    case tReal:
        typ = tTypeFloat;
        break;
    case tReal2:
        typ = tTypeFloat;
        break;
    case tLiteral:
        typ = tTypeString;
        break;
    default:
        typ = ctype;
        break;
    }
    return typ;
}


tTokenType evalExp(tStack *expStack, tSymTable *table)
{
    tStack *evalStack = NULL;
    evalStack = tstack_init();

    char code[MAX_IFJC_LEN];
    char tmpStr[MAX_IFJC_LEN];
    int tmpInt;
    double tmpReal;

    // We need these pointers to know what exactly should be reduced on the evaluation stack.
    // All the the operators are binary - we dont need more pointers.
    tToken stackTop = {0, NULL};
    stackTop.data = safe_malloc(MAX_TOKEN_LEN);
    tToken second = {0, NULL};
    token.data = safe_malloc(MAX_TOKEN_LEN);
    tToken third = {0, NULL};
    token.data = safe_malloc(MAX_TOKEN_LEN);
    // Pseudo nonterminal
    // Precedence of evaluation stack's top token and input token.
    char precedence;
    
    // For knowledge of final type of result that is being returned to
    tTokenType resultType;

    tToken nonTerminal = {0, NULL};
    nonTerminal.data = safe_malloc(MAX_TOKEN_LEN);
    tToken uselessToken = {0, NULL};
    uselessToken.data = safe_malloc(MAX_TOKEN_LEN);
    tToken topToken = {0, NULL};
    topToken.data = safe_malloc(MAX_TOKEN_LEN);
    tToken inputToken = {0, NULL}; 
    inputToken.data = safe_malloc(MAX_TOKEN_LEN);

    tstack_pop(expStack, &inputToken);
    tstack_push(evalStack, inputToken);
    tstack_pop(evalStack, &inputToken);

    rearrangeStack(expStack);

    addCode("#EXPRESSION START");
    addCode("CREATEFRAME");
    addCode("DEFVAR TF@%s", expResultName);

    
    // The 'finishing symbol' is empty evaluation stack.
    while (!tstack_isEmpty(evalStack)) 
    {
        // Definition of the three stack pointers 
        stackTop = *(tstack_peek(evalStack));
        tStackItem *tmp = evalStack->top->next; // smth like second
        if (tmp != NULL)
        {
            if (tmp->next != NULL)
            {
                second = tmp->next->token;
                if (tmp->next->next != NULL)
                    third = tmp->next->next->token;
            }
        }
        
        if (second.data != NULL && (isVar(&stackTop) || isConst(&stackTop)))
            precedence = prdTable[typeToIndex(second.type)][typeToIndex(inputToken.type)];
        else    
            precedence = '<';
        
        // We have var/const on top of stack
        if (isVar(&stackTop) || isConst(&stackTop))
        {
            switch (precedence)
            {
            case '=':
            
            case '<':
                // evaluating expression
                // The following token after literal const or string var has to be any string operator.
                if (isString(table, &stackTop) && !isStringOp(&inputToken))
                    errorExit("Semantic error: Missing string operator after string var/const.\n", CERR_SEM_TYPE);


                // String const/var and string operator are on stack but there is no string const/var as input token.
                else if (second.data != NULL && isString(table, &second) && \
                        isStringOp(&stackTop) && !isString(table, &inputToken))
                    errorExit("Semantic error: String var/const, string operator, missing string var/const.\n", CERR_SEM_TYPE);

                // The following token after number var/const has to be any num operator
                else if (isNumber(table, &stackTop) && !isNumberOp(&inputToken) && !isRelationalOp(&inputToken))
                    errorExit("Semantic error: Missing number operator after number var/const.\n", CERR_SEM_TYPE);

                // Number and number operator are on stack, next token has to be number.
                else if (second.data != NULL && isNumber(table, &second) && \
                        ((isNumberOp(&stackTop) || isRelationalOp(&stackTop)) && !isNumber(table, &inputToken)))
                    errorExit("Semantic error: Number var/const, number operator, missing number.\n", CERR_SEM_TYPE);


                tstack_pop(expStack, &inputToken);
                tstack_push(evalStack, inputToken);
                
                break;  
            case '>':

                // These two ifs should be redundant but just in case something screwed up...
                if (tstack_isEmpty(evalStack))
                    errorExit("Semantic error: Empty stack in expression while trying to reduce.\n", CERR_SEM_OTHER);
    
                if (!checkOpDefinition(table, &third, &stackTop))
                    errorExit("Semantic error: A variable is not defined.\n", CERR_SEM_UNDEF);


                dbgMsg("Redukujeme: dva operandy, first: %s, third: %s, operator (second): %s.\n", stackTop.data, third.data, tokenName[second.type]);
                
                resultType = getResultType(table, &stackTop, &third, second.type);

                if (isNumber(table, &stackTop) && isNumber(table, &third))
                {
                    if (isVar(&stackTop) && isVar(&third))
                    {
                        nonTerminal.type = resultType;
                        // generate code
                        sprintf(tmpStr, "LF@%s", third.data);
                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';

                        sprintf(tmpStr, "LF@%s", stackTop.data);
                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';
                    }
                    else if (isConst(&stackTop) && isVar(&third))
                    {
                        nonTerminal.type = resultType;
                        // generate code
                        sprintf(tmpStr, "LF@%s", third.data);
                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';

                        if (isInt(NULL, &stackTop))
                        {
                            // musime vytahnout hodnotu a udelat z ni int
                            tmpInt = convertToInt(&stackTop);
                            ifjCodeInt(tmpStr,tmpInt);
                        }
                        else
                        {
                            // musime vytahnout hodnotu a udelat z ni double
                            tmpReal = convertToDouble(&stackTop);
                            ifjCodeReal(tmpStr, tmpReal);
                        }

                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';
                    }
                    else if (isVar(&stackTop) && isConst(&third))
                    {
                        nonTerminal.type = resultType;
                        // generate code
                        sprintf(tmpStr, "LF@%s", stackTop.data);
                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';

                        if (isInt(NULL,&third))
                        {
                            // musime vytahnout hodnotu a udelat z ni int
                            tmpInt = convertToInt(&third);
                            ifjCodeInt(tmpStr,tmpInt);
                        }
                        else
                        {
                            // musime vytahnout hodnotu a udelat z ni double
                            tmpReal = convertToDouble(&third);
                            ifjCodeReal(tmpStr, tmpReal);
                        }

                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';
                    }
                    else if (isConst(&stackTop) && isConst(&third))
                    {
                        nonTerminal.type = resultType;

                        if(isInt(NULL,&third) && isInt(NULL, &stackTop))
                        {
                            // musime vytahnout hodnotu a udelat z ni int
                            tmpInt = convertToInt(&third);
                            ifjCodeInt(tmpStr,tmpInt);
                            strcat(code, tmpStr);
                            addCode(code);
                            code[0] = '\0';

                            // musime vytahnout hodnotu a udelat z ni int
                            tmpInt = convertToInt(&stackTop);
                            ifjCodeInt(tmpStr,tmpInt);
                            strcat(code, tmpStr);
                            addCode(code);
                            code[0] = '\0';
                        }
                        else if(isReal(NULL,&third) && isReal(NULL, &stackTop))
                        {
                            // musime vytahnout hodnotu a udelat z ni double
                            tmpReal = convertToDouble(&third);
                            ifjCodeReal(tmpStr,tmpReal);
                            strcat(code, tmpStr);
                            addCode(code);
                            code[0] = '\0';

                            // musime vytahnout hodnotu a udelat z ni double
                            tmpReal = convertToDouble(&stackTop);
                            ifjCodeReal(tmpStr,tmpReal);
                            strcat(code, tmpStr);
                            addCode(code);
                            code[0] = '\0';
                        }
                        else
                            errorExit("Semantic error: Operands of different type.\n", CERR_SEM_TYPE);
                    }
                }
                else if (isNumber(table, &stackTop) && isString(table, &third))
                {
                    if (second.type != tIdentical && second.type != tNotIdentical)
                        errorExit("Semantic error: Numbers and string can only be compared with \"===\" and \"!===\".\n", CERR_SEM_TYPE);
                }
                else if (isString(table, &stackTop) && isNumber(table, &third))
                {
                    if (second.type != tIdentical && second.type != tNotIdentical)
                        errorExit("Semantic error: Numbers and string can only be compared with \"===\" and \"!===\".\n", CERR_SEM_TYPE);

                }
                else // if (isString(table, &stackTop) && isString(table, &third))
                {
                    strcat(code, ifjCodeStr(tmpStr, stackTop.data));
                    addCode(code);
                    code[0] = '\0';

                    strcat(code, ifjCodeStr(tmpStr, third.data));
                    addCode(code);
                    code[0] = '\0';
                }
            }
                tstack_push(evalStack, nonTerminal);

                switch (second.type)
                {
                    case tPlus:
                        addCode("ADD TF@%s TF@%s ", expResultName, expResultName);
                        break;
                    case tMinus:
                        addCode("SUB TF@%s TF@%s ", expResultName, expResultName);
                        break;
                    case tMul:
                        addCode("MUL TF@%s TF@%s ", expResultName, expResultName);
                        break;
                    case tDiv:
                        if (isInt(table, &stackTop))
                        {
                            addCode("IDIV TF@%s TF@%s ", expResultName, expResultName);
                        }
                        else
                        {
                            addCode("DIV TF@%s TF@%s ", expResultName, expResultName);
                        }
                        break;
                    case tConcat:
                        addCode("CONCAT TF@%s TF@%s ", expResultName, expResultName);
                        break;
                    case tMore:
                        addCode("GT TF@%s TF@%s ", expResultName, expResultName);
                        break;
                    case tLess:
                        addCode("LT TF@%s TF@%s ", expResultName, expResultName);
                        break;
                    case tMoreEq:
                        addCode("JUMPIFEQ %s TF@%s TF@%s", expResultName, expResultName, expResultName);
                        addCode("GT TF@%s TF@%s ", expResultName, expResultName);
                        addCode("LABEL %s", expResultName);
                        break;
                    case tLessEq:
                        addCode("JUMPIFEQ %s TF@%s TF@%s", expResultName, expResultName, expResultName);
                        addCode("LT TF@%s TF@%s ", expResultName, expResultName);
                        addCode("LABEL %s", expResultName);
                        break;
                    case tIdentical:
                        addCode("JUMPIFEQ %s TF@%s TF@%s",  expResultName, expResultName, expResultName);
                        addCode("MOVE TF@%s 0", expResultName);
                        addCode("JUMP %s", expResultName);
                        addCode("LABEL %s", expResultName);
                        addCode("MOVE TF@%s 1", expResultName);
                        addCode("LABEL %s", expResultName);
                        break;
                    case tNotIdentical:
                        addCode("JUMPIFNEQ %s TF@%s TF@%s",  expResultName, expResultName, expResultName);
                        addCode("MOVE TF@%s 0", expResultName);
                        addCode("JUMP %s", expResultName);
                        addCode("LABEL %s", expResultName);
                        addCode("MOVE TF@%s 1", expResultName);
                        addCode("LABEL %s", expResultName);
                        break;
                    default:
                        errorExit("Syntax Error: Wrong operator.\n", CERR_SYNTAX);
                        break;
                }
                tstack_pop(evalStack, &uselessToken);
                tstack_pop(evalStack, &uselessToken);
                tstack_pop(evalStack, &uselessToken);
                break;
        }
    } 
    safe_free(stackTop.data);
    safe_free(second.data);
    safe_free(third.data);
    safe_free(inputToken.data);
    safe_free(topToken.data);
    safe_free(uselessToken.data);
    safe_free(nonTerminal.data);
    return nonTerminal.type;
}

/*
tTokenType evalExp(tStack* expStack, tSymTable* symTable)
{
    tToken token = { 0, NULL };
    // i kdyz je token lokalni promenna, tak jeji data jsou dymaicky alokovane
    token.data = safe_malloc(MAX_TOKEN_LEN); //??
    tTokenType typ = tNone;
    char code[MAX_IFJC_LEN];
    char tmpStr[MAX_IFJC_LEN];

    addCode("# expression START");
    addCode("CREATEFRAME");
    addCode("DEFVAR TF@%s", expResultName);

    // projdu vsechny tokeny co mam na stacku a vypisu je pres dbgMsg (printf, ale da se vypnout v support.h pres DEBUG_MSG)
    // u identifikatoru (promennych) zkontroluju jestli jsou v symbol table
    // prvni rozumny datovy typ si vratim jako datovy typ celeho vyrazu
    // jinak to nic uziteneho nedela ;-)

    sprintf(code, "MOVE TF@%s ", expResultName); // pripravim si naplneni docasne promenne prvnim tokenem, ktery by nemel byt operace

    while (!tstack_isEmpty(expStack))
    {
        tstack_pop(expStack, &token);

        switch (token.type)
        {
        case tIdentifier:
            {
                tSymTableItem* sti = st_search(symTable, token.data);
                if (sti != NULL)
                {
                    dbgMsg("%s", token.data);
                    // navratovy typ vyrazu nastvim podle prvni promenne, ktera mi prijde pod ruku ;-)
                    if (typ == tNone)
                        typ = sti->dataType;
                    else
                    { // a pokud uz typ mame a prisla promenna, ktera je jineho typu, tak prozatim semanticka chybe, nez poradne dodelame evalExp()
                        if (typ != sti->dataType)
                            errorExit("expression with different variable data types", CERR_SEM_TYPE); // tady to vypise chybu exitne program uplne
                    }
                    sprintf(tmpStr, "LF@%s", token.data);
                    strcat(code, tmpStr);
                    addCode(code);
                    code[0] = '\0';
                }
                else
                {
                    char errMsg[200];
                    sprintf(errMsg, "variable '%s' not defined before use", token.data);
                    errorExit(errMsg, CERR_SEM_UNDEF); // tady to vypise chybu exitne program uplne
                }
            }
            break;
        case tInt:
        case tInt2:
            {
                dbgMsg("%s", token.data);
                int tmpi;
                if (sscanf(token.data, "%d", &tmpi) != 1)
                    errorExit("wrong integer constant", CERR_INTERNAL);
                strcat(code, ifjCodeInt(tmpStr, tmpi));
                addCode(code);
                code[0] = '\0';
                if (typ == tNone)
                    // konstanty prevest na typ nebo primo typ
                    typ = const2type(token.type);
            } 
            break;
        case tReal:
        case tReal2:
            {
                double tmpd;
                if (sscanf(token.data, "%lf", &tmpd) != 1)
                    errorExit("wrong integer constant", CERR_INTERNAL);
                strcat(code, ifjCodeReal(tmpStr, tmpd));
                addCode(code);
                code[0] = '\0';
                if (typ == tNone)
                    // konstanty prevest na typ nebo primo typ
                    typ = const2type(token.type);

            } 
            break;
        case tLiteral:
            {
                strcat(code, ifjCodeStr(tmpStr, token.data));
                addCode(code);
                code[0] = '\0';
                // nasledujici if krmici typ je jen dummy, aby mi to neco delalo, vyhodnoceni vyrazu to pak musi vratit samozrejme spravne
                // delaji to i predhozi case tInt a TReal...
                // navratovy typ nastvim podle prvniho konstany se smysluplnym typem, ktery mi prijde pod ruku ;-)
                if (typ == tNone)
                    // konstanty prevest na typ nebo primo typ
                    typ = const2type(token.type);
            }
            break;
        case tPlus:
            {
                sprintf(tmpStr, "ADD TF@%s TF@%s ", expResultName, expResultName);
                strcpy(code, tmpStr);
            }
            break;
        case tMinus:
            {
                sprintf(tmpStr, "SUB TF@%s TF@%s ", expResultName, expResultName);
                strcpy(code, tmpStr);
            }
            break;
        case tMul:
            {
                sprintf(tmpStr, "MUL TF@%s TF@%s ", expResultName, expResultName);
                strcpy(code, tmpStr);
            }
            break;
        case tDiv:
            {
                sprintf(tmpStr, "DIV TF@%s TF@%s ", expResultName, expResultName);
                strcpy(code, tmpStr);
            }
            break;
        case tConcat:
            {
                sprintf(tmpStr, "CONCAT TF@%s TF@%s ", expResultName, expResultName);
                strcpy(code, tmpStr);
            }
            break;
        default:
            errorExit("unknown token in expression", CERR_SYNTAX); // tohle by se nemelo stat, pokud to projde syntaktickou analyzou, ale pro sichr
            break;
        }
    }
    if (strlen(code) != 0)
        errorExit("partial instruction in expression evaluation exit", CERR_INTERNAL); // this should not happe if everything properly parsed
    addCode("# expression END");
    free(token.data);
    return typ;
}*/




