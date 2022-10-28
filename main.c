//
//  main.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "token.h"
#include "lex.h"

char *tokName[] = {
    "tTypeInt", "tTypeFloat", "tTypeString", "tNullTypeInt", "tNullTypeFloat", "tNullTypeString",
    "tIf", "tElse", "tWhile", "tFunction", "tReturn", "tVoid", "tNull",
    "tInvalid", "tIdentifier", "tFuncName", "tType", "tNullType",
    "tPlus", "tMinus", "tConcat","tMul", "tDiv", "tLPar", "tRPar", "tLCurl", "tRCurl", "tColon", "tSemicolon", "tComma",
    "tAssign", "tIdentical",
    "tExclamation", "tNotIdentical",
    "tLess", "tLessEq", "tMore", "tMoreEq",
    "tInt", "tReal", "tReal2", "tInt2",
    "tLiteral", "tEpilog"
}; // "tKeyword",

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Gigachad compiler\n");
    
    FILE *inf;
    
    inf = fopen("vzor_01.php", "r");
    if(inf == NULL){
        printf("nemuzu otevrit soubor\n");
        printf("%s\n", strerror(errno));
    }
    else
    {
        if (SkipProlog(inf))
            printf("PROLOG OK\n");
        else
            printf("INVALID PROLOG\n");
        if(!feof(inf))
        {
            tToken token;
            while (ReadToken(inf, &token))
            {
                if(token.type == tInvalid)
                    printf("INVALID: <<%s>>\n",token.data);
                else
                    printf("TOKEN %s: <<%s>>\n",tokName[token.type], token.data);
            }
        }
    }
    
    fclose(inf);
    
    return 0;
}
