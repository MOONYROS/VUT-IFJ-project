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
#include "generator.h"

extern int prgPass;
extern const char tmpExpResultName[];

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

tTokenType evalExp(tStack* exp, tSymTable* st)
{
    tToken token = { 0, 0 };
    // i kdyz je token lokalni promenna, tak jeji data jsou dymaicky alokovane
    token.data = safe_malloc(MAX_TOKEN_LEN);
    tTokenType typ = tNone;
    char code[MAX_IFJC_LEN];
    char tmpStr[MAX_IFJC_LEN];

    addCode("# expression START");
    addCode("CREATEFRAME");
    addCode("DEFVAR TF@%s", tmpExpResultName);

    // projdu vsechny tokeny co mam na stacku a vypisu je pres dbgMsg (printf, ale da se vypnout v support.h pres DEBUG_MSG)
    // u identifikatoru (promennych) zkontroluju jestli jsou v symbol table
    // prvni rozumny datovy typ si vratim jako datovy typ celeho vyrazu
    // jinak to nic uziteneho nedela ;-)

    sprintf(code, "MOVE TF@%s ", tmpExpResultName); // pripravim si naplneni docasne promenne prvnim tokenem, ktery by nemel byt operace

    while (!tstack_isEmpty(exp))
    {
        tstack_pop(exp, &token);

        switch (token.type)
        {
        case tIdentifier:
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
                sprintf(tmpStr, "ADD TF@%s TF@%s ", tmpExpResultName, tmpExpResultName);
                strcpy(code, tmpStr);
            }
            break;
        case tMinus:
            {
                sprintf(tmpStr, "SUB TF@%s TF@%s ", tmpExpResultName, tmpExpResultName);
                strcpy(code, tmpStr);
            }
            break;
        case tMul:
            {
                sprintf(tmpStr, "MUL TF@%s TF@%s ", tmpExpResultName, tmpExpResultName);
                strcpy(code, tmpStr);
            }
            break;
        case tDiv:
            {
                sprintf(tmpStr, "DIV TF@%s TF@%s ", tmpExpResultName, tmpExpResultName);
                strcpy(code, tmpStr);
            }
            break;
        case tConcat:
            {
                sprintf(tmpStr, "CONCAT TF@%s TF@%s ", tmpExpResultName, tmpExpResultName);
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
}