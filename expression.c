// 
//  expression.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include "token.h"
#include "tstack.h"
#include "symtable.h"

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

void expression(tToken token, tStack stack){

    int precedence = prd_table[typeToIndex(token.type)][typeToIndex(stack.top->token.type)];

    switch (precedence) {
        case '=':;
            // evaluating expression
            // code generation
            break;

        case '>':;
            // evaluating expression
            // code generation
            break;

        case '<':;
            // evaluating expression
            // code generation
            break;
            
    }
}

tTokenType evalExp(tStack* expStack, tSymTable* symTable)
{
    tStackItem *top;
    // We need these pointers to know what exactly should be reduced on the stack.
    tStackItem *second;
    tStackItem *third;

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