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

int lbl = 0;

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
            variableType(table, exp) == tNullTypeInt || variableType(table, exp) == tNullTypeFloat);
    else 
        return (exp->type == tInt || exp->type == tInt2 || exp->type == tReal || exp->type == tReal2);
}

bool isString(tSymTable *table, tExpression *exp)
{   
    if (isVar(table, exp))
        return (variableType(table, exp) == tLiteral);
    else 
        return exp->type == tLiteral;
}

bool isReal(tSymTable *table, tExpression *exp)
{
    if (isVar(table, exp))
        return (variableType(table, exp) == tReal || variableType(table, exp) == tReal2);
    else 
        return (exp->type == tReal || exp->type == tReal2);
}

bool isInt(tSymTable *table, tExpression *exp)
{
    if (isVar(table, exp))
        return (variableType(table, exp) == tInt || variableType(table, exp) == tInt2 || variableType(table, exp) == tTypeInt); // aaa - tohle asi upravit nejak komplexnejsi promenne jsou tTypeInt... ne tInt
    else 
        return (exp->type == tInt || exp->type == tInt2);
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
    if (item != NULL)
        return true;
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
    return false;
}

tTokenType getResultType(tSymTable *table, tExpression *top, tExpression *third, tTokenType operation)
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
    numOp.type = tmp->token.type;
    if (tmp->token.data != NULL)
    {
        numOp.data = safe_malloc(MAX_TOKEN_LEN);
        strcpy(numOp.data, tmp->token.data);
    }
    while (tmp != NULL)
    {
        if (isNumberOp(&numOp) || tmp->token.type == tLPar)
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

    char code[MAX_IFJC_LEN];
    char tmpStr[MAX_IFJC_LEN];
    int tmpInt;
    double tmpReal;
    const char tmpNonTerminal[] = "nonTerminal";
    const char tmp1[] = "tmp1";
    const char tmp2[] = "tmp2";

    dbgMsg("evalExp:\n");
    //addCode("DEFVAR LF@otoc");
    //addCode("DEFVAR LF@tmp");

    //Auxiliary variables for easier work with stacks and reducing.
    tExpression stackTop = {NULL, 0, false};
    stackTop.data = safe_malloc(MAX_TOKEN_LEN);
    tExpression second = {NULL, 0, false};
    second.data = safe_malloc(MAX_TOKEN_LEN);
    tExpression third = {NULL, 0, false};
    third.data = safe_malloc(MAX_TOKEN_LEN);
    tExpression nonTerminal = {NULL, tNone, true};
    nonTerminal.data = safe_malloc(MAX_TOKEN_LEN);
    strcpy(nonTerminal.data, "NT");
    nonTerminal.isNonTerminal = true;
    tExpression inputexp = {NULL, 0, false}; 
    inputexp.data = safe_malloc(MAX_TOKEN_LEN);
    // For pops from expStack
    tToken popexp = {0, NULL};
    popexp.data = safe_malloc(MAX_TOKEN_LEN);
    // exp struct for storing popped data from evalStack (and is kinda useless)
    tExpression uselessExp = {NULL, 0, false};
    uselessExp.data = safe_malloc(MAX_TOKEN_LEN);


    // Precedence of evaluation stack's top exp and input exp.
    char precedence;
    // For knowledge of final type of result that is being returned to
    tTokenType resultType;

    addCode("#EXPRESSION START");
    rearrangeStack(expStack);
    
    // Always pop and push first
    tstack_pop(expStack, &popexp);
    if (popexp.data != NULL)
        strcpy(inputexp.data, popexp.data);
    inputexp.type = popexp.type;
    inputexp.isNonTerminal = false;

    expStackPush(evalStack, &inputexp);

    // There was only one item on input stack
    if (tstack_isEmpty(expStack))
    {
        // Code generation?
        if (isVar(table, &inputexp))
        {
            addCode("MOVE %s LF@%s", tgtVar, inputexp.data);
            return variableType(table, &inputexp);
        }
        else
        {
            // Todle by mela byt hotovka, Jondo (jsem si to schvalne udelal, 
            // abych si zkusil, jak funguji ty fce v generatoru)
            if (isString(table, &inputexp))
                addCode("MOVE %s %s", tgtVar, ifjCodeStr(tmpStr, inputexp.data));
            else if (isReal(table, &inputexp))
                // je potrbea to prepsat do hexadecimalniho, jinak to funguje docela dobre
                addCode("MOVE %s %s", tgtVar, ifjCodeReal(tmpStr, convertToDouble(&inputexp)));
            else if (isInt(table, &inputexp))
                addCode("MOVE %s %s", tgtVar, ifjCodeInt(tmpStr, convertToInt(&inputexp))); 
            else 
                errorExit("Podminka u jen jedne veci na inputstacku neco chybne\n", CERR_INTERNAL);
            return inputexp.type;
        }
    }

    // Prepare another exp as input exp
    tstack_pop(expStack, &popexp);
    if (popexp.data != NULL)
        strcpy(inputexp.data, popexp.data);
    inputexp.type = popexp.type;
    inputexp.isNonTerminal = false;

    //addCode("CREATEFRAME");
    //addCode("DEFVAR TF@%s", tmp1);
    //addCode("DEFVAR TF@%s", tmp2);
    //addCode("DEFVAR TF@%s", tmpNonTerminal);
    //addCode("MOVE %s", tgtVar);

    // The 'finishing symbol' is empty evaluation stack.
    while (!expIsEmpty(evalStack)) 
    {
        // Definition of the three stack pointers 
        expStackTop(evalStack, &stackTop);
        tExpStackItem *tmp = evalStack->top->next; // smth like second
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
        
        // Uz nemame co shiftovatS
        if (inputexp.type == tNone)
            precedence = '>';
        else if ( (isNonTerminal(evalStack->top->exp) || isConst(&stackTop) || isVar(table, &stackTop)) && isOperator(&inputexp) )
            precedence = prdTable[typeToIndex(second.type)][typeToIndex(inputexp.type)];
        else if (isConst(&inputexp))
            precedence = '<';
        else
            precedence = prdTable[typeToIndex(stackTop.type)][typeToIndex(inputexp.type)];
        
        switch (precedence)
        {
        case '=':

            break;
        
        case '<':
            // evaluating expression
            // The following exp after literal const or string var has to be any string operator.
            if (isString(table, &stackTop) && !isStringOp(&inputexp))
                errorExit("Semantic error: Missing string operator after string var/const.\n", CERR_SEM_TYPE);


            // String const/var and string operator are on stack but there is no string const/var as input exp.
            else if (second.data != NULL && isString(table, &second) && \
                    isStringOp(&stackTop) && !isString(table, &inputexp))
                errorExit("Semantic error: String var/const, string operator, missing string var/const.\n", CERR_SEM_TYPE);

            // The following exp after number var/const has to be any num operator
            else if (isNumber(table, &stackTop) && !isNumberOp(&inputexp) && !isRelationalOp(&inputexp))
                errorExit("Semantic error: Missing number operator after number var/const.\n", CERR_SEM_TYPE);

            // Number and number operator are on stack, next exp has to be number.
            else if (second.data != NULL && isNumber(table, &second) && \
                    ((isNumberOp(&stackTop) || isRelationalOp(&stackTop)) && !isNumber(table, &inputexp)))
                errorExit("Semantic error: Number var/const, number operator, missing number.\n", CERR_SEM_TYPE);

            expStackPush(evalStack, &inputexp);

            if (tstack_isEmpty(expStack))
            {
                strcpy(inputexp.data, "");
                inputexp.isNonTerminal = false;
                inputexp.type = tNone;
            }
            else
            {
                tstack_pop(expStack, &popexp);
                inputexp.isNonTerminal = false;
                inputexp.type = popexp.type;
                if (popexp.data != NULL)
                    strcpy(inputexp.data, popexp.data);
            }
            
            break;  

        case '>':

            // This if should be redundant but just in case something screwed up...
            if (expIsEmpty(evalStack))
                errorExit("Empty stack in expression while trying to reduce.\n", CERR_INTERNAL);

            // Print whats being reduced on stack
            dbgMsg("%s %s %s\n", third.data, tokenName[second.type], stackTop.data);
            
            nonTerminal.type = getResultType(table, &stackTop, &third, second.type);

            if (third.isNonTerminal == false && stackTop.isNonTerminal == false)
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
                errorExit("Syntax Error: Wrong operator.\n", CERR_SYNTAX);
                break;
            }

            expStackPop(evalStack, &uselessExp);
            expStackPop(evalStack, &uselessExp);
            expStackPop(evalStack, &uselessExp);
            expStackPush(evalStack, &nonTerminal);
            
            // If there's last item on our stack, it shall be the last nonterminal,
            // we pop it so the while loop breaks.
            if (evalStack->top->next == NULL && inputexp.type == tNone)
                expStackPop(evalStack, &uselessExp);
        }            
    } 
    // addCode("MOVE %s TF@%s", tgtVar, tmpNonTerminal);
    addCode("POPS %s", tgtVar);

    safe_free(second.data);
    safe_free(popexp.data);
    safe_free(nonTerminal.data);
    return nonTerminal.type;
}
