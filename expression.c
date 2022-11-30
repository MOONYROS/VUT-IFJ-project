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

/*
static int oper_prio(tToken token)
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
*/

tTokenType evalExp(tStack *expStack, tSymTable *table)
{
    tStack *evalStack = NULL;
    tstack_init(evalStack);

    char code[MAX_IFJC_LEN];
    char tmpStr[MAX_IFJC_LEN];
    int tmpInt;
    double tmpReal;

    // We need these pointers to know what exactly should be reduced on the evaluation stack.
    // All the the operators are binary - we dont need more pointers.
    tStackItem *stackTop = NULL;
    tStackItem *second = NULL;
    tStackItem *third = NULL;
    // Pseudo nonterminal
    // Precedence of evaluation stack's top token and input token.
    char precedence = '<';
    
    // For knowledge of final type of result that is being returned to
    tTokenType resultType;

    tToken *nonTerminal = NULL;
    nonTerminal->data = safe_malloc(MAX_TOKEN_LEN);
    tToken *uselessToken = NULL;
    uselessToken->data = safe_malloc(MAX_TOKEN_LEN);
    tToken *topToken = NULL;
    topToken->data = safe_malloc(MAX_TOKEN_LEN);
    tToken inputToken = {0, NULL}; 
    inputToken.data = safe_malloc(MAX_TOKEN_LEN);

    tstack_pop(expStack, &inputToken);
    tstack_push(evalStack, inputToken);

    rearrangeStack(expStack);

    addCode("#EXPRESSION START");
    addCode("CREATEFRAME");
    addCode("DEFVAR TF@%s", expResultName);

    // The 'finishing symbol' is empty evaluation stack.
    while (!tstack_isEmpty(evalStack)) 
    {
        // Definition of the three stack pointers 
        topToken = tstack_peek(evalStack);
        stackTop->token = *topToken;
        if (stackTop != NULL)
        {
            if (stackTop->next != NULL)
            {
                second = stackTop->next;
                if (second->next != NULL)
                    third = second->next;
            }
        }
        
        // We have var/const on top of stack
        if (isVar(&stackTop->token) || isConst(&stackTop->token))
        {
            switch (precedence)
            {
            case '=':
            
            case '<':
                // evaluating expression
                // The following token after literal const or string var has to be any string operator.
                if (isString(table, &stackTop->token) && !isStringOp(&inputToken))
                    errorExit("Semantic error: Missing string operator after string var/const.\n", CERR_SEM_TYPE);


                // String const/var and string operator are on stack but there is no string const/var as input token.
                else if (second != NULL && isString(table, &second->token) && \
                        isStringOp(&stackTop->token) && !isString(table, &inputToken))
                    errorExit("Semantic error: String var/const, string operator, missing string var/const.\n", CERR_SEM_TYPE);

                // The following token after number var/const has to be any num operator
                else if (isNumber(table, &stackTop->token) && !isNumberOp(&token) && !isRelationalOp(&token))
                    errorExit("Semantic error: Missing number operator after number var/const.\n", CERR_SEM_TYPE);

                // Number and number operator are on stack, next token has to be number.
                else if (second != NULL && isNumber(table, &second->token) && \
                        ((isNumberOp(&stackTop->token) || isRelationalOp(&stackTop->token)) && !isNumber(table, &inputToken)))
                    errorExit("Semantic error: Number var/const, number operator, missing number.\n", CERR_SEM_TYPE);


                tstack_pop(expStack, &inputToken);
                tstack_push(evalStack, inputToken);
                
                break;  
            case '>':

                // These two ifs should be redundant but just in case something screwed up...
                if (tstack_isEmpty(evalStack))
                    errorExit("Semantic error: Empty stack in expression while trying to reduce.\n", CERR_SEM_OTHER);
    
                if (!checkOpDefinition(table, &third->token, &stackTop->token))
                    errorExit("Semantic error: A variable is not defined.\n", CERR_SEM_UNDEF);


                dbgMsg("Redukujeme: dva operandy, first: %s, third: %s, operator (second): %s.\n", stackTop->token.data, third->token.data, tokenName[second->token.type]);
                
                resultType = getResultType(table, &stackTop->token, &third->token, second->token.type);

                if (isNumber(table, &stackTop->token) && isNumber(table, &third->token))
                {
                    if (isVar(&stackTop->token) && isVar(&third->token))
                    {
                        nonTerminal->type = resultType;
                        // generate code
                        sprintf(tmpStr, "LF@%s", third->token.data);
                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';

                        sprintf(tmpStr, "LF@%s", stackTop->token.data);
                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';
                    }
                    else if (isConst(&stackTop->token) && isVar(&third->token))
                    {
                        nonTerminal->type = resultType;
                        // generate code
                        sprintf(tmpStr, "LF@%s", third->token.data);
                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';

                        if (isInt(NULL, &stackTop->token))
                        {
                            // musime vytahnout hodnotu a udelat z ni int
                            tmpInt = convertToInt(&stackTop->token);
                            ifjCodeInt(tmpStr,tmpInt);
                        }
                        else
                        {
                            // musime vytahnout hodnotu a udelat z ni double
                            tmpReal = convertToDouble(&stackTop->token);
                            ifjCodeReal(tmpStr, tmpReal);
                        }

                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';
                    }
                    else if (isVar(&stackTop->token) && isConst(&third->token))
                    {
                        nonTerminal->type = resultType;
                        // generate code
                        sprintf(tmpStr, "LF@%s", stackTop->token.data);
                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';

                        if (isInt(NULL,&third->token))
                        {
                            // musime vytahnout hodnotu a udelat z ni int
                            tmpInt = convertToInt(&third->token);
                            ifjCodeInt(tmpStr,tmpInt);
                        }
                        else
                        {
                            // musime vytahnout hodnotu a udelat z ni double
                            tmpReal = convertToDouble(&third->token);
                            ifjCodeReal(tmpStr, tmpReal);
                        }

                        strcat(code, tmpStr);
                        addCode(code);
                        code[0] = '\0';
                    }
                    else if (isConst(&stackTop->token) && isConst(&third->token))
                    {
                        nonTerminal->type = resultType;

                        if(isInt(NULL,&third->token) && isInt(NULL, &stackTop->token))
                        {
                            // musime vytahnout hodnotu a udelat z ni int
                            tmpInt = convertToInt(&third->token);
                            ifjCodeInt(tmpStr,tmpInt);
                            strcat(code, tmpStr);
                            addCode(code);
                            code[0] = '\0';

                            // musime vytahnout hodnotu a udelat z ni int
                            tmpInt = convertToInt(&stackTop->token);
                            ifjCodeInt(tmpStr,tmpInt);
                            strcat(code, tmpStr);
                            addCode(code);
                            code[0] = '\0';
                        }
                        else if(isReal(NULL,&third->token) && isReal(NULL, &stackTop->token))
                        {
                            // musime vytahnout hodnotu a udelat z ni double
                            tmpReal = convertToDouble(&third->token);
                            ifjCodeReal(tmpStr,tmpReal);
                            strcat(code, tmpStr);
                            addCode(code);
                            code[0] = '\0';

                            // musime vytahnout hodnotu a udelat z ni double
                            tmpReal = convertToDouble(&stackTop->token);
                            ifjCodeReal(tmpStr,tmpReal);
                            strcat(code, tmpStr);
                            addCode(code);
                            code[0] = '\0';
                        }
                        else
                            errorExit("Semantic error: Operands of different type.\n", CERR_SEM_TYPE);
                    }
                }
                else if (isNumber(table, &stackTop->token) && isString(table, &third->token))
                {
                    if (second->token.type != tIdentical && second->token.type != tNotIdentical)
                        errorExit("Semantic error: Numbers and string can only be compared with \"===\" and \"!===\".\n", CERR_SEM_TYPE);
                }
                else if (isString(table, &stackTop->token) && isNumber(table, &third->token))
                {
                    if (second->token.type != tIdentical && second->token.type != tNotIdentical)
                        errorExit("Semantic error: Numbers and string can only be compared with \"===\" and \"!===\".\n", CERR_SEM_TYPE);

                }
                else // if (isString(table, &stackTop->token) && isString(table, &third->token))
                {
                    strcat(code, ifjCodeStr(tmpStr, stackTop->token.data));
                    addCode(code);
                    code[0] = '\0';

                    strcat(code, ifjCodeStr(tmpStr, third->token.data));
                    addCode(code);
                    code[0] = '\0';
                }
            }
                tstack_push(evalStack, *nonTerminal);

                switch (second->token.type)
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
                        if (isInt(table, &stackTop->token))
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
                tstack_pop(evalStack, uselessToken);
                tstack_pop(evalStack, uselessToken);
                tstack_pop(evalStack, uselessToken);
                break;
        }
    } 
    safe_free(inputToken.data);
    safe_free(topToken->data);
    safe_free(uselessToken->data);
    safe_free(nonTerminal->data);
    return nonTerminal->type;
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




