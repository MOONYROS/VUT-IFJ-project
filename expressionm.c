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
extern const char funcPrefixName[];
extern tSymTable gst;

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

tTokenType evalExp(char* tgtVar, tStack* exp, tSymTable* st)
{
    tToken token = { 0, 0 };
    // i kdyz je token lokalni promenna, tak jeji data jsou dymaicky alokovane
    token.data = safe_malloc(MAX_TOKEN_LEN);
    tTokenType typ = tNone;
    char code[MAX_IFJC_LEN];
    char tmpStr[MAX_IFJC_LEN];
    int notResult = 0;

    if ((exp == NULL) || (exp->top == NULL))
        errorExit("empty expression to process", CERR_INTERNAL);

    if ((tgtVar == NULL) || (tgtVar[1] != 'F') || (tgtVar[2] != '@'))
        errorExit("incorrect target variable in expression", CERR_INTERNAL);

    //addCode("# expression START");
    //addCode("CREATEFRAME");
    //sprintf(tmpStr, "DEFVAR TF@%s", tmpExpResultName);
    //addCode(tmpStr);

    // projdu vsechny tokeny co mam na stacku a vypisu je pres dbgMsg (printf, ale da se vypnout v support.h pres DEBUG_MSG)
    // u identifikatoru (promennych) zkontroluju jestli jsou v symbol table
    // prvni rozumny datovy typ si vratim jako datovy typ celeho vyrazu
    // jinak to nic uziteneho nedela ;-)

    sprintf(code, "MOVE %s ", tgtVar); // pripravim si naplneni vysledne promenne prvnim tokenem, ktery by nemel byt operace

    while (!tstack_isEmpty(exp))
    {
        tstack_pop(exp, &token);

        switch (token.type)
        {
        case tIdentifier:
            {
                tSymTableItem* sti;
                char varName[MAX_TOKEN_LEN];
                if (strncmp(token.data, funcPrefixName, strlen(funcPrefixName)) == 0)
                {
                    char* zacatek = token.data + strlen(funcPrefixName);
                    if (strlen(token.data) > strlen(funcPrefixName) + 5)
                    {
                        int delka = (int)strlen(token.data) - (int)strlen(funcPrefixName) - 5;
                        strncpy(varName, zacatek, delka);
                        varName[delka] = '\0';
                    }
                    else
                        errorExit("variable with function prefix, but incorrect lenght", CERR_INTERNAL);
                    sti = st_search(&gst, varName); //token.data
                }
                else
                {
                    strcpy(varName, token.data);
                    sti = st_search(st, varName); //token.data
                }
                if (sti != NULL)
                {
                    dbgMsg("%s", token.data);
                    // navratovy typ vyrazu nastvim podle prvni promenne, ktera mi prijde pod ruku ;-)
                    if (typ == tNone)
                        typ = sti->dataType;
                    else
                    { // a pokud uz typ mame a prisla promenna, ktera je jineho typu, tak prozatim semanticka chybe, nez poradne dodelame evalExp() aaa
                        if (!typeIsCompatible(typ, sti->dataType))
                        {
                            // jestlize je to naopak kompatibilni, tak rozsirime typ vyrazu, jinak uz je to fakt chyba
                            if (typeIsCompatible(sti->dataType, typ))
                                typ = sti->dataType; 
                            else
                                errorExit("expression with different variable data types", CERR_SEM_TYPE); // tady to vypise chybu exitne program uplne
                        }
                    }
                    if (strstr(token.data, funcPrefixName) != NULL)
                        sprintf(tmpStr, "LF@%s", token.data);
                    else
                        sprintf(tmpStr, "LF@%s", token.data);
                    strcat(code, tmpStr);
                    addCode(code);
                    code[0] = '\0';
                    if (notResult) 
                    {
                        sprintf(tmpStr, "NOT %s %s", tgtVar, tgtVar);
                        addCode(tmpStr);
                        notResult = 0;
                    }
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
                if (notResult)
                {
                    sprintf(tmpStr, "NOT %s %s", tgtVar, tgtVar);
                    addCode(tmpStr);
                    notResult = 0;
                }
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
                if (notResult)
                {
                    sprintf(tmpStr, "NOT %s %s", tgtVar, tgtVar);
                    addCode(tmpStr);
                    notResult = 0;
                }
                if (typ == tNone)
                    // konstanty prevest na typ nebo primo typ
                    typ = const2type(token.type);

            } 
            break;
        case tLiteral:
            {
                dbgMsg("\"%s\"", token.data);
                strcat(code, ifjCodeStr(tmpStr, token.data));
                addCode(code);
                code[0] = '\0';
                if (notResult)
                {
                    sprintf(tmpStr, "NOT %s %s", tgtVar, tgtVar);
                    addCode(tmpStr);
                    notResult = 0;
                }
                // nasledujici if krmici typ je jen dummy, aby mi to neco delalo, vyhodnoceni vyrazu to pak musi vratit samozrejme spravne
                // delaji to i predhozi case tInt a TReal...
                // navratovy typ nastvim podle prvniho konstany se smysluplnym typem, ktery mi prijde pod ruku ;-)
                if (typ == tNone)
                    // konstanty prevest na typ nebo primo typ
                    typ = const2type(token.type);
            }
            break;
        case tNull:
            {
                dbgMsg("%s", token.data);
                strcat(code, "nil@nil");
                addCode(code);
                // WOJEB jeslti bylo EQ na NOT EQ
                //sprintf(tmpStr, "%s", tgtVar);
                //strcpy(tmpStr, "TF@%condRes00002");
                //if (strncmp(code, "EQ", 2) == 0)
                //    addCode("NOT %s %s", tmpStr, tmpStr);
                code[0] = '\0';
                if (notResult)
                {
                    sprintf(tmpStr, "NOT %s %s", tgtVar, tgtVar);
                    addCode(tmpStr);
                    notResult = 0;
                }
                if (typ == tNone)
                    typ = tNullType;
            }
            break;
        case tPlus:
            {
                dbgMsg("%s", token.data);
                sprintf(tmpStr, "ADD %s %s ", tgtVar, tgtVar);
                strcpy(code, tmpStr);
                notResult = 0;
        }
            break;
        case tMinus:
            {
                dbgMsg("%s", token.data);
                sprintf(tmpStr, "SUB %s %s ", tgtVar, tgtVar);
                strcpy(code, tmpStr);
            }
            break;
        case tMul:
            {
                dbgMsg("%s", token.data);
                sprintf(tmpStr, "MUL %s %s ", tgtVar, tgtVar);
                strcpy(code, tmpStr);
            }
            break;
        case tDiv:
            {
                dbgMsg("%s", token.data);
                sprintf(tmpStr, "DIV %s %s ", tgtVar, tgtVar);
                strcpy(code, tmpStr);
            }
            break;
        case tConcat:
            {
                dbgMsg("%s", token.data);
                sprintf(tmpStr, "CONCAT %s %s ", tgtVar, tgtVar);
                strcpy(code, tmpStr);
            }
            break;
        case tLess:
            {
                dbgMsg("%s", token.data);
                sprintf(tmpStr, "LT %s %s ", tgtVar, tgtVar);
                strcpy(code, tmpStr);
            }
            break;
        case tMore:
            {
                dbgMsg("%s", token.data);
                sprintf(tmpStr, "GT %s %s ", tgtVar, tgtVar);
                strcpy(code, tmpStr);
            }
            break;
        case tIdentical:
            {
                dbgMsg("%s", token.data);
                sprintf(tmpStr, "EQ %s %s ", tgtVar, tgtVar);
                strcpy(code, tmpStr);
            }
            break;
        case tNotIdentical:
            {
                dbgMsg("%s", token.data);
                sprintf(tmpStr, "EQ %s %s ", tgtVar, tgtVar);
                strcpy(code, tmpStr);
                notResult = 1;
            }
            break;

        case tLessEq:
        case tMoreEq:
        default:
            errorExit("unknown token in expression", CERR_SYNTAX); // tohle by se nemelo stat, pokud to projde syntaktickou analyzou, ale pro sichr
            break;
        }
    }
    if (strlen(code) != 0)
        errorExit("partial instruction in expression evaluation exit", CERR_INTERNAL); // this should not happe if everything properly parsed
    //addCode("# expression END");
    addCode("");

    safe_free(token.data);
    return typ;
}