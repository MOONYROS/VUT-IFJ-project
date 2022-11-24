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

tTokenType evalExp(tStack* exp, tSymTable* st)
{
    tToken token = { 0, 0 };
    // i kdyz je token lokalni promenna, tak jeji data jsou dymaicky alokovane
    token.data = safe_malloc(MAX_TOKEN_LEN);
    tTokenType typ = tNone;
    // projdu vsechny tokeny co mam na stacku a vypisu je pres dbgMsg (printf, ale da se vypnout v support.h pres DEBUG_MSG)
    // u identifikatoru (promennych) zkontroluju jestli jsou v symbol table
    // prvni rozumny datovy typ si vratim jako datovy typ celeho vyrazu
    // jinak to nic uziteneho nedela ;-)
    while (!tstack_isEmpty(exp))
    {
        tstack_pop(exp, &token);

        if(token.type==tIdentifier)
        {
            tSymTableItem* sti = st_search(st, token.data);
            if (sti != NULL)
            {
                dbgMsg("%s", token.data);
                // navratovy typ vyrazu nastvim podle prvni promenne, ktera mi prijde pod ruku ;-)
                if (typ == tNone)
                    typ = sti->dataType;
                else
                { // a pokud uz typ mame a prisla promenna, ktera je jineho typu, tak prozatim semanticka chybe, nez poradne dodelame evalExp()
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