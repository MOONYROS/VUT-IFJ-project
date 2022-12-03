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
#include "expStack.h"


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
        return (variableType(table, exp) == tTypeString || variableType(table, exp) == tNullTypeString);
    else 
        return exp->type == tLiteral;
}

bool isReal(tSymTable *table, tExpression *exp)
{
    if (isVar(table, exp))
        return (variableType(table, exp) == tTypeFloat || variableType(table, exp) == tNullTypeFloat);
    else 
        return (exp->type == tReal || exp->type == tReal2);
}

bool isInt(tSymTable *table, tExpression *exp)
{
    if (isVar(table, exp))
        return (variableType(table, exp) == tTypeInt || variableType(table, exp) == tNullTypeInt);
    else 
        return (exp->type == tInt || exp->type == tInt2);
}

bool isNonTerminal(tExpStackItem *item)
{
    return item->exp->isNonTerminal == true;
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
    return item->isDefined;
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
    
    if (numOp.data != NULL)
        safe_free(numOp.data);
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

tTokenType evalExp(tStack *expStack, tSymTable *table)
{
    tExpStack *evalStack = NULL;
    expStackInit(evalStack);

    char code[MAX_IFJC_LEN];
    char tmpStr[MAX_IFJC_LEN];
    int tmpInt;
    double tmpReal;

    //Auxiliary variables for easier work with stacks and reducing.
    tExpression stackTop = {NULL, 0, false};
    stackTop.data = safe_malloc(MAX_TOKEN_LEN);
    tExpression second = {NULL, 0, false};
    second.data = safe_malloc(MAX_TOKEN_LEN);
    tExpression third = {NULL, 0, false};
    third.data = safe_malloc(MAX_TOKEN_LEN);
    tExpression nonTerminal = {NULL, 0, false};
    nonTerminal.data = safe_malloc(MAX_TOKEN_LEN);
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

    rearrangeStack(expStack);
    
    // Always pop and push first
    tstack_pop(expStack, &popexp);
    if (popexp.data != NULL)
        strcpy(inputexp.data, popexp.data);
    inputexp.type = popexp.type;
    inputexp.isNonTerminal = false;

    expStackPush(evalStack, inputexp);

    // There was only one item on input stack
    if (tstack_isEmpty(expStack))
    {
        // Code generation?
        if (isVar(table, &inputexp))
            return variableType(table, &inputexp);
        else
            return inputexp.type;
    }

    // Prepare another exp as input exp
    tstack_pop(expStack, &popexp);
    if (popexp.data != NULL)
        strcpy(inputexp.data, popexp.data);
    inputexp.type = popexp.type;
    inputexp.isNonTerminal = false;

    addCode("#EXPRESSION START");
    addCode("CREATEFRAME");
    addCode("DEFVAR TF@%s", expResultName);
    
    // The 'finishing symbol' is empty evaluation stack.
    while (!isEmpty(evalStack)) 
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
        }
        
        if (tstack_isEmpty(expStack))
            precedence = '>';
        else if (isNonTerminal(evalStack->top))
            precedence = prdTable[typeToIndex(second.type)][typeToIndex(inputexp.type)];
        else 
            precedence = prdTable[typeToIndex(stackTop.type)][typeToIndex(inputexp.type)];
        
        switch (precedence)
        {
        case '=':
        
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

            expStackPush(evalStack, inputexp);

            tstack_pop(expStack, &popexp);
            inputexp.isNonTerminal = false;
            inputexp.type = popexp.type;
            if (popexp.data != NULL)
                strcpy(inputexp.data, popexp.data);
            
            break;  

        case '>':

            // This if should be redundant but just in case something screwed up...
            if (isEmpty(evalStack))
                errorExit("Semantic error: Empty stack in expression while trying to reduce.\n", CERR_SEM_OTHER);

            // Print whats being reduced on stack
            dbgMsg("%s %s %s\n", stackTop.data, tokenName[second.type], third.data);
            
            resultType = getResultType(table, &stackTop, &third, second.type);

            if (isNumber(table, &stackTop) && isNumber(table, &third))
            {
                if (isVar(table, &stackTop) && isVar(table, &third))
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
                else if (isConst(&stackTop) && isVar(table, &third))
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
                else if (isVar(table, &stackTop) && isConst(&third))
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
                    addCode("IDIV TF@%s TF@%s ", expResultName, expResultName);
                else
                    addCode("DIV TF@%s TF@%s ", expResultName, expResultName);
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
                addCode("MOVE TF@%s int@0", expResultName);
                addCode("JUMP %s", expResultName);
                addCode("LABEL %s", expResultName);
                addCode("MOVE TF@%s int@1", expResultName);
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
            expStackPop(evalStack, &uselessExp);
            expStackPop(evalStack, &uselessExp);
            expStackPop(evalStack, &uselessExp);
            expStackPush(evalStack, nonTerminal);
            tstack_pop(expStack, &popexp);

            // If there's last item on our stack, it shall be the last nonterminal,
            // we pop it so the while loop breaks.
            if (evalStack->top->next == NULL)
                expStackPop(evalStack, &uselessExp);
        }            
        
    } 
    safe_free(second.data);
    safe_free(popexp.data);
    safe_free(nonTerminal.data);
    return nonTerminal.type;
}

/*
texpType evalExp(tStack* expStack, tSymTable* symTable)
{
    texp exp = { 0, NULL };
    // i kdyz je exp lokalni promenna, tak jeji data jsou dymaicky alokovane
    exp.data = safe_malloc(MAX_TOKEN_LEN); //??
    texpType typ = tNone;
    char code[MAX_IFJC_LEN];
    char tmpStr[MAX_IFJC_LEN];

    addCode("# expression START");
    addCode("CREATEFRAME");
    addCode("DEFVAR TF@%s", expResultName);

    // projdu vsechny expy co mam na stacku a vypisu je pres dbgMsg (printf, ale da se vypnout v support.h pres DEBUG_MSG)
    // u identifikatoru (promennych) zkontroluju jestli jsou v symbol table
    // prvni rozumny datovy typ si vratim jako datovy typ celeho vyrazu
    // jinak to nic uziteneho nedela ;-)

    sprintf(code, "MOVE TF@%s ", expResultName); // pripravim si naplneni docasne promenne prvnim expem, ktery by nemel byt operace

    while (!tstack_isEmpty(expStack))
    {
        tstack_pop(expStack, &exp);

        switch (exp.type)
        {
        case tIdentifier:
            {
                tSymTableItem* sti = st_search(symTable, exp.data);
                if (sti != NULL)
                {
                    dbgMsg("%s", exp.data);
                    // navratovy typ vyrazu nastvim podle prvni promenne, ktera mi prijde pod ruku ;-)
                    if (typ == tNone)
                        typ = sti->dataType;
                    else
                    { // a pokud uz typ mame a prisla promenna, ktera je jineho typu, tak prozatim semanticka chybe, nez poradne dodelame evalExp()
                        if (typ != sti->dataType)
                            errorExit("expression with different variable data types", CERR_SEM_TYPE); // tady to vypise chybu exitne program uplne
                    }
                    sprintf(tmpStr, "LF@%s", exp.data);
                    strcat(code, tmpStr);
                    addCode(code);
                    code[0] = '\0';
                }
                else
                {
                    char errMsg[200];
                    sprintf(errMsg, "variable '%s' not defined before use", exp.data);
                    errorExit(errMsg, CERR_SEM_UNDEF); // tady to vypise chybu exitne program uplne
                }
            }
            break;
        case tInt:
        case tInt2:
            {
                dbgMsg("%s", exp.data);
                int tmpi;
                if (sscanf(exp.data, "%d", &tmpi) != 1)
                    errorExit("wrong integer constant", CERR_INTERNAL);
                strcat(code, ifjCodeInt(tmpStr, tmpi));
                addCode(code);
                code[0] = '\0';
                if (typ == tNone)
                    // konstanty prevest na typ nebo primo typ
                    typ = const2type(exp.type);
            } 
            break;
        case tReal:
        case tReal2:
            {
                double tmpd;
                if (sscanf(exp.data, "%lf", &tmpd) != 1)
                    errorExit("wrong integer constant", CERR_INTERNAL);
                strcat(code, ifjCodeReal(tmpStr, tmpd));
                addCode(code);
                code[0] = '\0';
                if (typ == tNone)
                    // konstanty prevest na typ nebo primo typ
                    typ = const2type(exp.type);

            } 
            break;
        case tLiteral:
            {
                strcat(code, ifjCodeStr(tmpStr, exp.data));
                addCode(code);
                code[0] = '\0';
                // nasledujici if krmici typ je jen dummy, aby mi to neco delalo, vyhodnoceni vyrazu to pak musi vratit samozrejme spravne
                // delaji to i predhozi case tInt a TReal...
                // navratovy typ nastvim podle prvniho konstany se smysluplnym typem, ktery mi prijde pod ruku ;-)
                if (typ == tNone)
                    // konstanty prevest na typ nebo primo typ
                    typ = const2type(exp.type);
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
            errorExit("unknown exp in expression", CERR_SYNTAX); // tohle by se nemelo stat, pokud to projde syntaktickou analyzou, ale pro sichr
            break;
        }
    }
    if (strlen(code) != 0)
        errorExit("partial instruction in expression evaluation exit", CERR_INTERNAL); // this should not happe if everything properly parsed
    addCode("# expression END");
    free(exp.data);
    return typ;
}*/




