//
//  expression.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "expression.h"
#include "support.h"
#include "generator.h"
#include "expStack.h"

extern char* tokenName[tMaxToken];

unsigned int lbl = 0;

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

bool isOperator(tExpression *exp)
{
    return (exp->type == tMul || exp->type == tDiv || exp->type == tPlus || exp->type == tMinus || exp->type == tLess || exp->type == tMore ||
        exp->type == tLessEq || exp->type == tMoreEq || exp->type == tIdentical || exp->type == tNotIdentical || exp->type == tConcat);
}

bool isNumberOp(tExpression *exp)
{
    return (exp->type == tMul || exp->type == tDiv || exp->type == tPlus || exp->type == tMinus);
}

bool isRelationalOp(tExpression *exp)
{
    return (exp->type == tLess || exp->type == tMore || exp->type == tLessEq || exp->type == tMoreEq || exp->type == tIdentical || exp->type == tNotIdentical);
}

bool isStringOp(tExpression *exp)
{
    return (exp->type == tIdentical || exp->type == tNotIdentical || exp->type == tConcat);
}

bool isConst(tExpression *exp)
{
    return (exp->type == tInt || exp->type == tInt2 || exp->type == tReal || exp->type == tReal2 || exp->type == tLiteral);
}

bool isVar(tSymTable *table, tExpression *exp)
{
    if (exp->type == tIdentifier)
    {
        if (!isDefined(table, exp))
        {
            errorExit("Semantic error: Undefined variable.\n", CERR_SEM_UNDEF);
            return false; // Just for gcc to stop screaming at me
        }
        return true;
    }
    return false;
}

bool isNumber(tSymTable *table, tExpression *exp)
{
    if (isVar(table, exp))
        return (variableType(table, exp) == tTypeInt || variableType(table, exp) == tTypeFloat || \
            variableType(table, exp) == tNullTypeInt || variableType(table, exp) == tNullTypeFloat || \
            variableType(table, exp) == tInt || variableType(table, exp) == tInt2 || \
            variableType(table, exp) == tReal || variableType(table, exp) == tReal2);
    else 
        return (exp->type == tInt || exp->type == tInt2 || exp->type == tReal || exp->type == tReal2);
}

bool isString(tSymTable *table, tExpression *exp)
{   
    if (isVar(table, exp))
        return (variableType(table, exp) == tLiteral || variableType(table, exp) == tTypeString || variableType(table, exp) == tNullTypeString);
    else 
        return exp->type == tLiteral;
}

bool isReal(tSymTable *table, tExpression *exp)
{
    if (isVar(table, exp))
        return (variableType(table, exp) == tReal || variableType(table, exp) == tReal2 || \
                variableType(table, exp) == tTypeFloat || variableType(table, exp) == tNullTypeFloat);
    else 
        return (exp->type == tReal || exp->type == tReal2 || exp->type == tTypeFloat || exp->type == tNullTypeFloat);
}

bool isInt(tSymTable *table, tExpression *exp)
{
    if (isVar(table, exp))
        return (variableType(table, exp) == tInt || variableType(table, exp) == tInt2 || \
                variableType(table, exp) == tTypeInt || variableType(table, exp) == tNullTypeInt);
    else 
        return (exp->type == tInt || exp->type == tInt2 || exp->type == tTypeInt || exp->type == tNullTypeInt);
}

bool isNonTerminal(tExpression *exp)
{
    return exp->isNonTerminal;
}

tTokenType variableType(tSymTable *table, tExpression *exp)
{
    // kontrola, jestli je to definovany
    tSymTableItem *item = st_search(table, exp->data);
    return item->dataType;
}

bool isDefined(tSymTable *table, tExpression *exp)
{
    tSymTableItem *item = st_search(table, exp->data);
    return item != NULL;
}

bool isNullTypeVar(tSymTable *table, tExpression *exp)
{
    if (isVar(table, exp))
        return (variableType(table, exp) == tNullTypeInt || variableType(table, exp) == tNullTypeFloat || variableType(table, exp) == tNullTypeString);
    else
        return false;
}

bool isNull(tSymTable *table, tExpression *exp)
{
    if (isVar(table, exp))
    {
        tSymTableItem *item = st_search(table, exp->data);
        return item->dataType == tNull;
    }
    else
        return exp->type == tNull;
}

tTokenType getResultType(tSymTable *table, tExpression *top, tExpression *third, tTokenType operation)
{
    switch (operation)
    {
        case tPlus:
        case tMinus:
        case tMul:
        case tDiv:
            if (!isNullTypeVar(table, top) || !isNullTypeVar(table, third))
            {
                if (isNull(table, top) || isNull(table, third))
                    errorExit("NULL operand in arithmetic operation.\n", CERR_SEM_TYPE);
                
                if (isReal(table, top) && isReal(table, third))
                    return tTypeFloat;
                else if (isInt(table, top) && isInt(table, third))
                    return tTypeInt;
                else
                    errorExit("Operands have to be the same type when performing arithmetic operation.\n", CERR_SEM_TYPE);
                return tNone;
            }
            else 
            {
                if (isReal(table, top) && isReal(table, third))
                    return tNullTypeFloat;
                else if (isInt(table, top) && isInt(table, third))
                    return tNullTypeInt;
                else
                    errorExit("Operands have to be the same type when performing arithmetic operation.\n", CERR_SEM_TYPE);
                return tNone;
            }
            
        case tConcat:
            if (!isNullTypeVar(table, third) && !isNullTypeVar(table, top))\
                return tTypeString;
            else
                return tNullTypeString;

        case tLess:
        case tLessEq:
        case tMore:
        case tMoreEq:
        case tIdentical:
        case tNotIdentical:

            return tTypeInt;

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
    strcpy(zero.data, "0");
    tToken leftPar = {tLPar, NULL};
    leftPar.data = safe_malloc(sizeof("("));
    strcpy(leftPar.data, "(");
    tToken rightPar = {tRPar, NULL};
    rightPar.data = safe_malloc(sizeof(")"));
    strcpy(rightPar.data, ")");

    if (tmp->token.type == tMinus)
    {
        tstack_push(stack, zero);
        tstack_push(stack, leftPar);
        tmp = tmp->next;
        tstack_insertAfter(stack, tmp, rightPar);
    }

    tStackItem *operator;
    tExpression numOp;
    numOp.isNonTerminal = false;
    while (tmp != NULL)
    {
        numOp.type = tmp->token.type;
        if (tmp->token.data != NULL)
        {
            numOp.data = safe_malloc(MAX_TOKEN_LEN);
            strcpy(numOp.data, tmp->token.data);
        }
        if (isNumberOp(&numOp) || tmp->token.type == tLPar)
        {
            operator = tmp;
            tmp = tmp->next;
            if (tmp->token.type == tMinus)
            {
                tstack_insertAfter(stack, operator, leftPar);
                operator = operator->next;
                leftPar.data = safe_malloc(sizeof("("));
                strcpy(leftPar.data, "(");

                tstack_insertAfter(stack, operator, zero);
                zero.data = safe_malloc(sizeof("0"));
                strcpy(zero.data, "0");
                for (int i = 0; i < 3; i++)
                    operator = operator->next;

                tstack_insertAfter(stack, operator, rightPar);
                rightPar.data = safe_malloc(sizeof(")"));
                strcpy(rightPar.data, ")");
            }
        }
        else
            tmp = tmp->next;
    }
}

double convertToDouble(tExpression *exp)
{
    double tmp;
    sscanf(exp->data, "%lf", &tmp);
    return tmp;
}

int convertToInt(tExpression *exp)
{
    int tmp;
    sscanf(exp->data, "%d", &tmp);
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
        case tInt:
        case tInt2:
        case tReal:
        case tReal2:
        case tLiteral:
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

char *typeToString(char *tmpStr, tExpression *exp)
{
    if (exp->type == tIdentifier)
        sprintf(tmpStr, "LF@%s", exp->data);
    else
    {
        switch (exp->type) {
        case tLiteral:
            ifjCodeStr(tmpStr, exp->data);
            break;
        case tInt: // aaa osetrit jestli jsou to spravne typu INT
        case tInt2:
        {
            int tmpi;
            if (sscanf(exp->data, "%d", &tmpi) != 1)
                errorExit("wrong integer constant", CERR_INTERNAL);
            ifjCodeInt(tmpStr, tmpi);
        }
        break;
        case tReal: // aaa osetrit spravne type real
        case tReal2:
        {   
            double tmpd;
            if (sscanf(exp->data, "%lf", &tmpd) != 1)
                errorExit("wrong float constant", CERR_INTERNAL);
            ifjCodeReal(tmpStr, tmpd);
        }
        break;
        default:
            errorExit("unknown typeToString parameter", CERR_INTERNAL);
            break;
        }
    }
    return tmpStr;
}

tTokenType evalExp(char* tgtVar, tStack *expStack, tSymTable *table)
{
    tExpStack *evalStack = NULL;
    expStackInit(&evalStack);

    //char code[MAX_IFJC_LEN];
    char tmpStr[MAX_IFJC_LEN];
    //int tmpInt;
    //double tmpReal;
    //const char tmpNonTerminal[] = "nonTerminal";
    char precedence;

    dbgMsg("evalExp:\n");
    //addCode("DEFVAR LF@otoc");
    //addCode("DEFVAR LF@tmp");

    // Auxiliary variables for easier work with stacks and reducing.
    tExpression stackTop = {NULL, 0, false};
    stackTop.data = safe_malloc(MAX_TOKEN_LEN);
    tExpression second = {NULL, 0, false};
    second.data = safe_malloc(MAX_TOKEN_LEN);
    tExpression third = {NULL, 0, false};
    third.data = safe_malloc(MAX_TOKEN_LEN);

    // Pseudo non-terminal.
    tExpression nonTerminal = {NULL, tNone, true};
    nonTerminal.data = safe_malloc(MAX_TOKEN_LEN);
    strcpy(nonTerminal.data, "NT");
    nonTerminal.isNonTerminal = true;
    
    // Used for conversion between tExpression and tToken.
    tExpression inputExp = {NULL, 0, false}; 
    inputExp.data = safe_malloc(MAX_TOKEN_LEN);
    tToken inputToken = {0, NULL};
    inputToken.data = safe_malloc(MAX_TOKEN_LEN);

    // Only used for storing popped data from evalStack.
    tExpression uselessExp = {NULL, 0, false};
    uselessExp.data = safe_malloc(MAX_TOKEN_LEN);

    addCode("#EXPRESSION START");
    rearrangeStack(expStack);
    
    // Always pop and push first
    tstack_pop(expStack, &inputToken);
    if (inputToken.data != NULL)
        strcpy(inputExp.data, inputToken.data);
    inputExp.type = inputToken.type;
    inputExp.isNonTerminal = false;

    expStackPush(evalStack, &inputExp);

    // There was only one item on input stack
    if (tstack_isEmpty(expStack))
    {
        if (isVar(table, &inputExp))
        {
            addCode("MOVE %s LF@%s", tgtVar, inputExp.data);
            return variableType(table, &inputExp);
        }
        else
        {
            if (isString(table, &inputExp))
                addCode("MOVE %s %s", tgtVar, ifjCodeStr(tmpStr, inputExp.data));
            else if (isReal(table, &inputExp))
                addCode("MOVE %s %s", tgtVar, ifjCodeReal(tmpStr, convertToDouble(&inputExp)));
            else if (isInt(table, &inputExp))
                addCode("MOVE %s %s", tgtVar, ifjCodeInt(tmpStr, convertToInt(&inputExp))); 
            else if (isNull(table, &inputExp))
            {
                if (!isNullTypeVar(table, &inputExp))
                    errorExit("NULL in variable that isn't NULL type.\n", CERR_SEM_TYPE);
                else
                    addCode("MOVE %s %s", tgtVar, "nil@nil");
            }
            else
                errorExit("Podminka u jen jedne veci na inputstacku neco chybne\n", CERR_INTERNAL);

            return const2type(inputExp.type);
        }
    }

    // Pop another token and "convert" it to expression.
    tstack_pop(expStack, &inputToken);
    if (inputToken.data != NULL)
        strcpy(inputExp.data, inputToken.data);
    inputExp.type = inputToken.type;
    inputExp.isNonTerminal = false;

    while (!expIsEmpty(evalStack)) 
    {
        // Definition of stackTop, second and third, which are variables storing data of first three items on stack. 
        expStackTop(evalStack, &stackTop);
        tExpStackItem *tmp = evalStack->top->next; 
        if (tmp != NULL)
        {
            strcpy(second.data, tmp->exp->data);
            second.isNonTerminal = tmp->exp->isNonTerminal;
            second.type = tmp->exp->type;
            if ((tmp = tmp->next) != NULL)
            {
                strcpy(third.data, tmp->exp->data);
                third.isNonTerminal = tmp->exp->isNonTerminal;
                third.type = tmp->exp->type;
            }
            else
                third.type = tNone;
        }
        else
        {
            second.type = tNone;
            third.type = tNone;
        }
        
        // If inputExp.type is set to none, there's nothing left on input stack (aka expStack). Reduce everything.
        if (inputExp.type == tNone)
            precedence = '>';
        else if ( (isNonTerminal(evalStack->top->exp) || isConst(&stackTop) || isVar(table, &stackTop)) && isOperator(&inputExp) )
            precedence = prdTable[typeToIndex(second.type)][typeToIndex(inputExp.type)];
        else if (isConst(&inputExp))
            precedence = '<';
        else if (isNonTerminal(evalStack->top->exp) && inputExp.type == tRPar)
            precedence = prdTable[typeToIndex(second.type)][typeToIndex(inputExp.type)];
        else
            precedence = prdTable[typeToIndex(stackTop.type)][typeToIndex(inputExp.type)];
        
        switch (precedence)
        {
        case 'x':
            errorExit("Never should've got here (case 'x')", CERR_INTERNAL);
            break;
        case '=':
            expStackPush(evalStack, &inputExp);
            if (tstack_isEmpty(expStack))
            {
                strcpy(inputExp.data, "");
                inputExp.isNonTerminal = false;
                inputExp.type = tNone;
            }
            else
            {
                tstack_pop(expStack, &inputToken);
                inputExp.isNonTerminal = false;
                inputExp.type = inputToken.type;
                if (inputToken.data != NULL)
                    strcpy(inputExp.data, inputToken.data);
            }
            break;
        
        case '<':
            /* ------  CHECKING TYPES ------ */

            // There's string literal or variable containing string on top of stack
            if (isString(table, &stackTop))
            {
                // The following expression has to be string compatible operator.
                if (!isStringOp(&inputExp))
                    errorExit("Expected string operator.\n", CERR_SEM_TYPE);
            }

            // There's a string operator on top of stack
            else if (second.type != tNone && isString(table, &second) && \
                    isStringOp(&stackTop) && !isString(table, &inputExp))
                errorExit("Expected string variable or constant.\n", CERR_SEM_TYPE);


            // There's number on top of stack
            else if (isNumber(table, &stackTop))
            {   
                // The following expression has to be arithmetic or relational operator.
                if (!(isNumberOp(&inputExp) || isRelationalOp(&inputExp)))
                    errorExit("The following token after number constant or variable containing number has to be number operator.\n", CERR_SEM_TYPE);
            }

            // There's number operator on top of stack
            else if (isNumber(table, &second) && (isNumberOp(&stackTop) || isRelationalOp(&stackTop)) && \
                    !(isNumber(table, &inputExp) || inputExp.type == tLPar))
                    errorExit("Expected number variable or constant.", CERR_SEM_TYPE);
    
            if (!isNullTypeVar(table, &inputExp) && isNull(table, &inputExp))
                errorExit("NULL in variable that isn't NULL type.\n", CERR_SEM_TYPE);

            expStackPush(evalStack, &inputExp);

            if (tstack_isEmpty(expStack))
            {
                strcpy(inputExp.data, "");
                inputExp.isNonTerminal = false;
                inputExp.type = tNone;
            }
            else
            {
                tstack_pop(expStack, &inputToken);
                inputExp.isNonTerminal = false;
                inputExp.type = inputToken.type;
                if (inputToken.data != NULL)
                    strcpy(inputExp.data, inputToken.data);
            }
            
            break;  

        case '>':

            // This if should be redundant but just in case something screwed up...
            if (expIsEmpty(evalStack))
                errorExit("Empty stack in expression while trying to reduce.\n", CERR_INTERNAL);

            // Print whats being reduced on stack
            dbgMsg("%s %s %s\n", third.data, tokenName[second.type], stackTop.data);
            
            if (!isOperator(&second))
                nonTerminal.type = second.type;
            else
                nonTerminal.type = getResultType(table, &stackTop, &third, second.type);

            if (nonTerminal.type == tNone)
                errorExit("Should never get here (nonterminal type tNone).\n", CERR_INTERNAL);

            if (third.type == tLPar && stackTop.type == tRPar)
            {
                expStackPop(evalStack, &uselessExp);
                expStackPop(evalStack, &uselessExp);
                expStackPop(evalStack, &uselessExp);
                expStackPush(evalStack, &nonTerminal);
                if (evalStack->top->next == NULL && inputExp.type == tNone)
                    expStackPop(evalStack, &uselessExp);
                break;
            }
            else if (third.isNonTerminal == false && stackTop.isNonTerminal == false)
            {
                addCode("PUSHS %s", typeToString(tmpStr, &third));
                addCode("PUSHS %s", typeToString(tmpStr, &stackTop));
            }
            else if (third.isNonTerminal == false && stackTop.isNonTerminal == true)
            {
                addCode("POPS LF@otoc");
                addCode("PUSHS %s", typeToString(tmpStr, &third));
                addCode("PUSHS LF@otoc");
            }
            else if (third.isNonTerminal == true && stackTop.isNonTerminal == false)
                addCode("PUSHS %s", typeToString(tmpStr, &stackTop));

            switch (second.type)
            {
            case tPlus:
                addCode("ADDS");
                break;
            case tMinus:
                addCode("SUBS");
                break;
            case tMul:
                addCode("MULS");
                break;
            case tDiv:
                addCode("DIVS");
                break;
            case tConcat:
                addCode("POPS LF@otoc");
                addCode("POPS LF@tmp");
                addCode("CONCAT LF@tmp LF@tmp LF@otoc");
                addCode("PUSHS LF@tmp");
                break;
            case tMore:
                addCode("GTS");
                addCode("POPS LF@tmp");
                addCode("JUMPIFEQ $vetsi%05d LF@tmp bool@true", lbl);
                addCode("PUSHS int@0");
                addCode("JUMP $done%05d", lbl);
                addCode("LABEL $vetsi%05d", lbl);
                addCode("PUSHS int@1");
                addCode("LABEL $done%05d", lbl++);
                break;
            case tLess:
                addCode("LTS");
                addCode("POPS LF@tmp");
                addCode("JUMPIFEQ $mensi%05d LF@tmp bool@true", lbl);
                addCode("PUSHS int@0");
                addCode("JUMP $done%05d", lbl);
                addCode("LABEL $mensi%05d", lbl);
                addCode("PUSHS int@1");
                addCode("LABEL $done%05d", lbl++);
                break;
            case tMoreEq:
                addCode("POPS LF@otoc");
                addCode("POPS LF@tmp");
                addCode("JUMPIFEQ $rovno%05d LF@tmp LF@otoc", lbl);
                addCode("GT LF@tmp LF@tmp LF@otoc", lbl);
                addCode("JUMPIFEQ $rovno%05d LF@tmp bool@true", lbl);
                addCode("JUMP $mene$05d", lbl);
                addCode("LABEL $rovno%05d", lbl);
                addCode("PUSHS int@1");
                addCode("JUMP $done%05d", lbl);
                addCode("LABEL $mene$05d", lbl);
                addCode("PUSHS int@0");
                addCode("LABEL $done%05d", lbl++);
                break;
            case tLessEq:
                addCode("POPS LF@otoc");
                addCode("POPS LF@tmp");
                addCode("JUMPIFEQ $rovno%05d LF@tmp LF@otoc", lbl);
                addCode("LT LF@tmp LF@tmp LF@otoc");
                addCode("JUMPIFEQ $rovno%05d LF@tmp bool@true", lbl);
                addCode("JUMP $vice%05d", lbl);
                addCode("LABEL $rovno%05d", lbl);
                addCode("PUSHS int@1");
                addCode("JUMP $done%05d", lbl);
                addCode("LABEL $vice%05d", lbl);
                addCode("PUSHS int@0");
                addCode("LABEL $done%05d", lbl++);
                break;
            case tIdentical:
                addCode("JUMPIFEQS $iden%05d", lbl);
                addCode("PUSHS int@0");
                addCode("JUMP $notiden%05d", lbl);
                addCode("LABEL $iden%05d", lbl);
                addCode("PUSHS int@1");
                addCode("LABEL $notiden%05d", lbl++);
                break;
            case tNotIdentical:
                addCode("JUMPIFNEQS $notiden%05d", lbl);
                addCode("PUSHS int@0");
                addCode("JUMP $iden%05d", lbl);
                addCode("LABEL $notiden%05d", lbl);
                addCode("PUSHS int@1");
                addCode("LABEL $iden%05d", lbl++);
                break;
            default:
                errorExit("Wrong operator.\n", CERR_INTERNAL);
                break;
            }

            expStackPop(evalStack, &uselessExp);
            expStackPop(evalStack, &uselessExp);
            expStackPop(evalStack, &uselessExp);
            expStackPush(evalStack, &nonTerminal);
            
            // If there's last item on our stack, it shall be the last nonterminal,
            // we pop it so the while loop breaks.
            if (evalStack->top->next == NULL && inputExp.type == tNone)
                expStackPop(evalStack, &uselessExp);
        }            
    } 
    // addCode("MOVE %s TF@%s", tgtVar, tmpNonTerminal);
    addCode("POPS %s", tgtVar);

    safe_free(stackTop.data);
    safe_free(second.data);
    safe_free(third.data);
    safe_free(inputToken.data);
    safe_free(inputExp.data);
    safe_free(nonTerminal.data);
    safe_free(uselessExp.data);
    return nonTerminal.type;
}
