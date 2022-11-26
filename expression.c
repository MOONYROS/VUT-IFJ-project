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
#include "symtable.h"
#include "tstack.h"
#include "support.h"
#include "token.h"

const char prd_table[15][15] = {
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
    {'<','<','<','<','<','<','<','<','<','<','<','<','x','<','x'},  // (
    {'>','>','>','>','>','>','>','>','>','>','>','x','>','x','>'},  // )
    {'>','>','>','>','>','>','>','>','>','>','>','x','>','x','>'},  // id
    {'<','<','<','<','<','<','<','<','<','<','<','<','x','<','x'}   // $
};

bool isOperator(tToken *token)
{
    if (token->type == tMul || token->type == tDiv || token->type == tPlus || token->type == tMinus || token->type == tLess || token->type == tMore ||
        token->type == tLessEq || token->type == tMoreEq || token->type == tIdentical || token->type == tNotIdentical || token->type == tConcat)
        return true;
    else
        return false;
}

bool numberOp(tToken *token)
{
    if (token->type == tMul || token->type == tDiv || token->type == tPlus || token->type == tMinus || token->type == tLess || token->type == tMore ||
        token->type == tLessEq || token->type == tMoreEq || token->type == tIdentical || token->type == tNotIdentical)
        return true;
    else
        return false;
}

bool relationalOp(tToken *token)
{
    if (token->type == tLess || token->type == tMore || token->type == tLessEq || token->type == tMoreEq || token->type == tIdentical || token->type == tNotIdentical)
        return true;
    else
        return false;
}

bool stringOp(tToken *token)
{
    if (token->type == tIdentical || token->type == tNotIdentical || token->type == tConcat)
        return true;
    else
        return false;
}

bool isVariableOrConst(tToken *token)
{
    if (token->type == tIdentifier || token->type == tInt || token->type == tInt2 || token->type == tReal || token->type == tReal2 || token->type == tLiteral)
        return true;
    else
        return false;
}

bool isNumber(tSymTable *table, tToken *token)
{
    // If theres table as param, then we're searching for variable
    if (table != NULL)
    {
        if (variableType(table, token) == tTypeInt || variableType(table, token) == tTypeFloat || \
            variableType(table, token) == tNullTypeInt || variableType(table, token) == tNullTypeFloat)
            return true;
    }
    // If not, just check if current token is number
    else if (token->type == tInt || token->type == tInt2 || token->type == tReal || token->type == tReal2)
        return true;
    
    return false;    
}

bool isString(tSymTable *table, tToken *token)
{   
    // If theres table as param, then we're searching for variable
    if (table != NULL)
    {
        if (variableType(table, token) == tTypeString || variableType(table, token) == tNullTypeString)
            return true;
    }
    // If not, just check if current token is string literal
    else if (token->type == tLiteral)
        return true;

    return false;
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

int typeToIndex(tTokenType token)
{
    switch (token)
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
            // Any other token is considered finishing character.
            return 14;
    }
}

void expression(tStack *stack, tSymTable *table)
{
    /*
        Todle nebudou asi muset nutne byt ukazatele na zasobnik, mozna budou stacit nejaky promenny urcityho
        datovyho typu - domluvit se
    */
    tStackItem *stackTop;
    // We need these pointers to know what exactly should be reduced on the stack.
    tStackItem *second;
    tStackItem *third;

    // We want always want to push on the stack when the evaluation starts
    char precedence;
    tstack_push(stack, token);
    // We need both empty stack and finishing token to end this loop successfully.
    while (!tstack_isEmpty(stack) || typeToIndex(token.type) != 14) 
    {
        // There still are tokens waiting for evaluation but there is a wrong input token
        if (!tstack_isEmpty(stack) && typeToIndex(token.type) == 14)
            errorExit("Syntax error: Expected term or operator.\n", CERR_SYNTAX);


        stackTop = tstack_peek(stack);
        if (stackTop != NULL)
        {
            if (stackTop->next != NULL)
            {
                second = stackTop->next;
                if (second->next != NULL)
                    third = second->next;
            }
        }

        precedence = prd_table[typeToIndex(token.type)][typeToIndex(stack->top->token.type)];

        switch (precedence) 
        {
            case '<':
                // evaluating expression

                // The following token after var/const cannot be var/const or left par
                // and the following token after operator cannot be operator or right par
                if ((isVariableOrConst(stackTop) && !isOperator(&token) && token.type != tRPar) || \
                    (isOperator(stackTop)) && !isVariableOrConst(&token) && token.type != tLPar)
                {
                    dbgMsg("Syntax error: Var/const or left par after var/const || Operator or right par after operator.\n");
                    return;
                }

                // The following token after literal const or string var has to be any string operator
                if ((isString(NULL, stackTop) || isString(table, stackTop)) && !stringOp(&token))
                {
                    dbgMsg("Semantic error: No string operator after string var/const.\n");
                    return;
                }
                // Stack: string const/var, string operator are on stack but there is no string const/var as input token
                else if (second != NULL && \
                        (isString(NULL, &second->token) || isString(table, &second->token)) && \
                        (stringOp(stackTop) && !(isString(NULL, &token) || isString(table, &token))))
                {
                    dbgMsg("Semantic error: String var/const, string operator, next isn't string var/const.\n");
                    return;
                }
                // The following token after number var/const has to be any num operator
                else if ((isNumber(NULL, stackTop) || isNumber(table, stackTop)) && !numberOp(&token))
                {
                    dbgMsg("Semnatic error: No number operator after number var/const.\n");
                    return;
                }
                // Stack: number, number operator are on stack next token has to be number.
                else if (second != NULL && \
                        (isNumber(NULL, &second->token) || isNumber(table, &second->token)) && \
                        (numberOp(stackTop) && !(isNumber(NULL, &token) || isNumber(table, &token))))
                {
                    dbgMsg("Semantic error: Number var/const, number operator, next isn't number.\n");
                    return;
                }

                /*
                    Prirazeni neni (nevime volani funkci)
                    Vyhledani prommene v symtable - bude potreba prommena typu pointer na polozku v symtab
                    Je potreba vyresit kontrolu typu - jak? Moznosti - pushovani struktury na zasobnik (novej zasobnik ci neco take)
                                                                     - ukladani do promenne - docela prasarna 
                    Uvolnit token a vzit novej
                    break; asi?
                */
                /*
                    Stringy - operator musi byt string_op(), dalsi operand taky string nebo var typu string
                    Prvni token musi byt promenna nebo konstanta
                */
                tstack_push(stack, token);
                // code generation
                break;  

            case '>':
                // evaluating expression
                // code generation
                break;

            default:
                dbgMsg("Syntax error: expected different token.\n");
                return;
        }
        nextToken();
    } 
}

tTokenType evalExp(tStack* expStack, tSymTable* symTable)
{
    tToken token = { 0, NULL };
    // i kdyz je token lokalni promenna, tak jeji data jsou dymaicky alokovane
    token.data = safe_malloc(MAX_TOKEN_LEN);
    tTokenType typ = tNone;
    // projdu vsechny tokeny co mam na stacku a vypisu je pres dbgMsg (printf, ale da se vypnout v support.h pres DEBUG_MSG)
    // u identifikatoru (promennych) zkontroluju jestli jsou v symbol table
    // prvni rozumny datovy typ si vratim jako datovy typ celeho vyrazu
    // jinak to nic uziteneho nedela ;-)
    while (!tstack_isEmpty(expStack))
    {
        tstack_pop(expStack, &token);

        if(token.type==tIdentifier)
        {
            tSymTableItem* sti = st_search(symTable, token.data);
            if (sti != NULL)
            {
                dbgMsg("%s", token.data);
                // navratovy typ vyrazu nastvim podle prvni promenne, ktera mi prijde pod ruku ;-)
                if (typ == tNone)
                    typ = sti->dataType;
                else
                { // a pokud uz typ mame a prisla promenna, ktera je jineho typu, tak prozatim semanticka chybe, nez poradne dodelame evalExpStack()
                    if(typ != sti->dataType)
                        errorExit("expression with different variable data types", CERR_SEM_TYPE); // tady to vypise chybu exitne program uplne
                }
            }
            else
            {
                char errMsg[200];
                sprintf(errMsg, "variable '%s' not defined before use", token.data);
                errorExit(errMsg, CERR_SEM_UNDEF); // tady to vypise chybu exitne program uplne
            }
        }
        else
        {
            dbgMsg("%s", token.data);
            // nasledujici if krmici typ je jen dummy, aby mi to neco delalo, vyhodnoceni vyrazu to pak musi vratit samozrejme spravne
            // navratovy typ nastvim podle prvniho konstany se smysluplnym typem, ktery mi prijde pod ruku ;-)
            if ((typ == tNone) && ((token.type >= tInt) && (token.type <= tLiteral)))
            {
                // konstanty prevest na typ nebo primo typ
                switch (token.type)
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
                    typ = token.type;
                    break;
                }
            }
        }

    }
    free(token.data);
    return typ;
}