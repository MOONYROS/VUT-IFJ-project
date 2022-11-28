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

bool numberOp(tToken *token)
{
    return (token->type == tMul || token->type == tDiv || token->type == tPlus || token->type == tMinus);
}

bool relationalOp(tToken *token)
{
    return (token->type == tLess || token->type == tMore || token->type == tLessEq || token->type == tMoreEq || token->type == tIdentical || token->type == tNotIdentical);
}

bool stringOp(tToken *token)
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

int convertTypes(tSymTable *table, tToken *top, tToken *third, tTokenType operation)
{
    bool notDiv;
    if (operation != tDiv)
        notDiv = true;
    else 
        notDiv = false;

    // Obe ciselne konstanty
    if (isNumber(NULL, top) && isNumber(NULL, third))
    {
        if (notDiv) 
        {
            if (isReal(top) || isReal(third))
            {
                top->type = tReal;
                third->type = tReal;
                // Vygeneruj instrukci pro typovou konverzi
            }
        }
        else
        {
            top->type = tReal;
            top->type = tReal;
            // Vygeneruj instrukci pro typovou konverzi
        }

        return 0;
    }
    // Prvni promenna, druha konstanta
    else if (isNumber(table, top) && isNumber(NULL, third))
    {
        
        return 1;
    }
    // Prvni konstanta, druha promenna
    else if (isNumber(NULL, top) && isNumber(table, third))
    {

        return 2;
    }
    // Dve promenne
    else // (isNumber(table, top) && isNumber(table, third))
    {
    
        return 3;
    }

    return -1;
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

/*
static int oper_prio(tToken token)
{
    switch (token.type)
    {
    case tMul:
    case tDiv:
        return 4;
        break;

    case tPlus:
    case tMinus:
        return 3;
        break;
    
    case tLess:
    case tLessEq:
    case tMore:
    case tMoreEq:
        return 2;
        break;

    case tIdentical:
    case tNotIdentical:
        return 1;
        break;

    default:
        break;
    }
}
*/

void expression(tStack *expStack, tSymTable *table)
{
    tStack *evalStack;
    tstack_init(evalStack);

    // We need these pointers to know what exactly should be reduced on the evaluation stack.
    // All the the operators are binary - we dont need more pointers.
    tStackItem *stackTop;
    tStackItem *second;
    tStackItem *third;
    // Pseudo nonterminals
    tStackItem intExpression;
    tStackItem floatExpression;
    tStackItem stringExpression;

    // Precedence of evaluation stack's top token and input token.
    char precedence;

    tToken inputToken; 
    tstack_pop(expStack, &inputToken);
    tstack_push(evalStack, inputToken);

    // We need both empty stack and finishing token to end this loop successfully.
    while (!tstack_isEmpty(evalStack) || typeToIndex(inputToken.type) != 14) 
    {
        
        // Definition of the three stack pointers 
        stackTop = tstack_peek(evalStack);
        if (stackTop != NULL)
        {
            if (stackTop->next != NULL)
            {
                second = stackTop->next;
                if (second->next != NULL)
                    third = second->next;
            }
        }
        /*
        Semantika a pseudokod voleni precedence

        if stack top je id
            if stack second je lpar
                precedence = shift
            else
                if stack second < input
                    precedence = shift
                else if stack second > input
                    precedence = redukce
                else
                    precedence = '='
        else
            precedence = shift
        */
        
        // We have var/const on top of stack
        if (isVar(stackTop) || isConst(stackTop))
            // shifting when stack second is left par

            /*
            if(isOperator(&inputToken))
            {
                
                // shifting when operator on stack has lower priority than input operator
                if (oper_prio(second->token) < oper_prio(inputToken))
                {
                    precedence = '<';
                }
                // reducing when operator on stack has highier priority than input operator
                else if (oper_prio(second->token) >= oper_prio(inputToken))
                {
                    precedence = '>';
                }

                if (second->token.type == tLPar)
                {
                    precedence = '<';
                }
            }
            else if(inputToken.type == tRPar)
            {
                if (second->token.type == tLPar)
                {
                    precedence = '=';
                }
                
                precedence = '>';
            }
            */
            // Semantika je stejna jako kod nahore, klidne mrkni do tabulky a over si to
            precedence = prd_table[typeToIndex(second->token.type)][typeToIndex(inputToken.type)];
        else
            // Toto je spravne, pokud top je lpar, operator nebo rpar, musime za kazdou cenu dale pushnout
            precedence = '<';


        switch (precedence) 
        {
            case '=':
                break;
            
            case '<':
                // evaluating expression
                // The following token after literal const or string var has to be any string operator
                if ((isString(NULL, stackTop) || isString(table, stackTop)) && !stringOp(&inputToken))
                {
                    dbgMsg("Semantic error: Missing string operator after string var/const.\n");
                    return;
                }

                // Stack: string const/var, string operator are on stack but there is no string const/var as input token
                else if (second != NULL && \
                        (isString(NULL, &second->token) || isString(table, &second->token)) && \
                        (stringOp(stackTop) && !(isString(NULL, &inputToken) || isString(table, &inputToken))))
                {
                    dbgMsg("Semantic error: String var/const, string operator, missing string var/const.\n");
                    return;
                }

                // The following token after number var/const has to be any num operator
                else if ((isNumber(NULL, stackTop) || isNumber(table, stackTop)) && !numberOp(&token) && !relationalOp(&token))
                {
                    dbgMsg("Semantic error: Missing number operator after number var/const.\n");
                    return;
                }

                // Stack: number, number operator are on stack next token has to be number.
                else if (second != NULL && (isNumber(NULL, &second->token) || isNumber(table, &second->token)) && \
                        ((numberOp(stackTop) || relationalOp(stackTop)) && !(isNumber(NULL, &inputToken) || isNumber(table, &inputToken))))
                {
                    dbgMsg("Semantic error: Number var/const, number operator, missing number.\n");
                    return;
                }

                tstack_pop(expStack, &inputToken);
                tstack_push(evalStack, inputToken);

                if (tstack_isFull(evalStack))
                    exitError("Expression stack overflow.\n", CERR_INTERNAL);

                break;  

            case '>':
                // evaluating expression
                /*
                    jak to bude fungovat:

                    Kdyz se dostaneme sem do redukce, tak nas uz nezajima, co to je za operator a jake ma operandy,
                    protoze syntakticke chyby se sem nedostanou a nektere semanticke uz mame (snad) vyresene u toho pushovani.
                    Potrebujem teda de facto vzit ty dva operandy a operator, provest semantickou akci, pripadne nejake
                    typove konverze, popnout dva operandy a operator a vysledek pushnout zpatky na zasobnik (evalStack). 
                    Pokud jsou obe promenne, tak provedu operaci a vysledek ulozim do tabulky symbolu (prvniho operandu),
                    pripadne tedy provedu typovou konverzi tak, ze prepisu typ promenne, kde se ulozil vysledek.
                    Pokud 1 var a 1 const, pak provedu operaci, vysledek ulozim do symtab, kde je var.
                    Pokud 2 const, vysledkem je novy token, ktery je taky const.
                    Trosku na picu, co nam hazi vidle pod nohy je to, ze vsechny data jsou ulozeny v poli znaku a musime si
                    z toho pripadne vytahnout cisla.
                    Otazkou je, jestli se s TS bude vubec pracovat a jestli nemame rovnou generovat instrukce.
                    Sorry chlapi, moc jsem toho nestih, vecer po zapase na to jeste kouknu.
                */

                if (tstack_isEmpty(evalStack))
                {
                    dbgMsg("Semantic error: Empty stack in expression while trying to reduce.\n");
                    return;
                }
                
                if (!checkOpDefinition(table, &third->token, &stackTop->token))
                {
                    dbgMsg("Semantic error: A variable is not defined.\n");
                    return;
                }

                dbgMsg("Redukujeme: dva operandy, first: %s, third: %s, operator (second): %s.\n", stackTop->token.data, third->token.data, tokenName[second->token.type]);
                switch (second->token.type)
                {
                    case tPlus:
                        if (convertTypes(table, &third->token, &stackTop->token, tPlus))
                        {
                            dbgMsg("An error occured when trying to convert types.\n");
                            return;
                        }
                        break;
                    case tMinus:

                        break;
                    case tMul:

                        break;
                    case tDiv:

                        break;
                    case tConcat:

                        break;
                    case tMore:

                        break;
                    case tLess:

                        break;
                    case tMoreEq:

                        break;
                    case tLessEq:

                        break;
                    case tIdentical:

                        break;
                    case tNotIdentical:

                        break;
                    default:
                        dbgMsg("Expression: Tady jsme se nikdy nemeli dostat, kurnik.\n");
                        break;
                }

                // code generation
                break;
        }
        nextToken();
    } 
}

tTokenType evalExp(tStack* expStack, tSymTable* symTable)
{
    tToken token = { 0, NULL };
    // i kdyz je token lokalni promenna, tak jeji data jsou dymaicky alokovane
    token.data = safe_malloc(MAX_TOKEN_LEN); //??
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