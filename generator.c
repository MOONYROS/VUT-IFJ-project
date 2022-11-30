//
//  generator.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "token.h"
#include "support.h"
#include "generator.h"

tCodeLine* codeFirst = NULL;
tCodeLine* codeLast = NULL;

const char tmpExpResultName[] = "tmpRes"; // tmpExpResultGigachad
extern int prgPass;

char* ifjCodeStr(char *outStr, char* str)
{
    char tmpStr[5];

    if (outStr == NULL)
        return NULL;

    strcpy(outStr, "string@");

    size_t srcLen = strlen(str);
    for (size_t i = 0; i < srcLen; i++)
    {
        size_t len = strlen(outStr);
        if ((str[i] >= 0 && str[i] <= 32) || (str[i]==35))
        {
            sprintf(tmpStr, "\\%03d", (int)str[i]);
            strcat(outStr, tmpStr);
        }
        else if (str[i] == 92) // '\'
        {
            if (i + 1 < srcLen)
            {
                i++;
                switch (str[i])
                {
                case 'a':
                    strcat(outStr, "\\007");
                    break;
                case 'b':
                    strcat(outStr, "\\008");
                    break;
                case 't':
                    strcat(outStr, "\\009");
                    break;
                case 'n':
                    strcat(outStr, "\\010");
                    break;
                case 'v':
                    strcat(outStr, "\\011");
                    break;
                case 'f':
                    strcat(outStr, "\\012");
                    break;
                case 'r':
                    strcat(outStr, "\\013");
                    break;
                case 'e':
                    strcat(outStr, "\\027");
                    break;
                case '\'':
                    strcat(outStr, "\\039");
                    break;
                case '\\':
                    strcat(outStr, "\\092");
                    break;
                default:
                    break;
                }
            }
            else
                strcat(outStr, "\\092");
        }
        else
        {
            outStr[len] = str[i];
            outStr[len+1] = '\0';

        }
    }
    return outStr;
}

char* ifjCodeInt(char* outStr, int val)
{
    if (outStr == NULL)
        return NULL;

    sprintf(outStr, "int@%d", val);
    return outStr;
}

char* ifjCodeReal(char* outStr, double val)
{
    if (outStr == NULL)
        return NULL;

    sprintf(outStr, "float@%a", val);
    return outStr;
}

void generateHead()
{
    addCode("# program START");
    addCode(".IFJcode22");
    //tady mozna potreba vytvorit GF jak se pise v zadani
}

void generateFuncDecl()
{
    addCode("# function declaration");
}

void generateFuncCall()
{
    addCode("# function call");
}

void generateIfStatement()
{
    addCode("# if statement");
}

void generateWhileStatement()
{
    addCode("# while statement");
}

void generateStatement()
{
    addCode("# statement");
}

int addCode(const char* fmt, ...)
{
    if (prgPass != 2)
        return 0;

    char code[MAX_IFJC_LEN];

    va_list args;
    va_start(args, fmt);
    int ret = vsprintf(code, fmt, args);
    va_end(args);

    tCodeLine* item = safe_malloc(sizeof(tCodeLine));
    item->code = safe_malloc(strlen(code) + 1);
    item->next = NULL;
    strcpy(item->code, code);
    if (codeFirst == NULL)
        codeFirst = item;
    else
        codeLast->next = item;
    codeLast = item;

    return ret;
}

void generateCode(FILE* f)
{
    tCodeLine* item = codeFirst;
    while (item != NULL)
    {
        fprintf(f, "%s\n", item->code);
        tCodeLine* nxt = item->next;
        safe_free(item->code);
        safe_free(item);
        item = nxt;
    }
}
