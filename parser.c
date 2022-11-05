//
//  parser.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 02.11.2022.
//
#include <string.h>

#include "parser.h"

#include "token.h"
#include "lex.h"

int ReadTokenPRINT(FILE *f, tToken *token)
{
    int ret;
    ret = ReadToken(f, token);
    if(token->type == tInvalid)
        printf("INVALID: <<%s>>\n",token->data);
    else
        printf("TOKEN %s: <<%s>>\n", tokenName[token->type], token->data);
    return ret;
}

tTokenType strToToken(const char *tokenStr)
{
    for (int t = tTypeInt; t <= tEpilog; t++) {
        if (strcmp(tokenStr, tokenName[t]) == 0)
            return t;
    }
    return tInvalid;
}

int parser(FILE *f, tToken *token, tParseTree *tree, const char *state, int iter, int line)
{
    printf("%*s%s (%d): %s - %s\n", iter * 2, "", state, line, tokenName[token->type], token->data);

    //if(feof(f))
       // return 1;
    
    for (int i = 0; i < 62; i++) {
        if (strcmp(state, rule[i][0]) == 0)
        {
            int j = 1;
            while (j != 13)
            {
                if (strcmp(rule[i][j], "") == 0)
                {
                    //printf("%*sKONEC radku %d\n", iter*2, "", line);
                    return 1;
                }
                if (strcmp(rule[i][j], "EPS") == 0)
                {
                    printf("%*sEpsilon na (%d) %s\n", iter*2, "", i+1, state);
                    return 1;
                }
                if ((strlen(rule[i][j] ) > 1) && (rule[i][j][0] == 't') && (rule[i][j][1] >= 'A') && (rule[i][j][1] <= 'Z'))
                {
                    if (token->type == strToToken(rule[i][j]))
                    {
                        printf("%*sMATCH (%d) %s -> %s\n", iter*2, "", i+1, state, rule[i][1]);
                        if(ReadTokenPRINT(f, token)==0)
                            return 1;
                        else
                        {
                            if(token->type == tInvalid)
                            {
                                printf("INVALID TOKEN\n");
                                return 0;
                            }
                            // return 1;
                        }
                    }
                    else
                    {
                        if(j>1)
                        {
                            printf("neocekavany token\n");
                            return 0;
                        }
                        else
                            break;
                    }
                }
                else
                {
                    if(parser(f, token, tree, rule[i][j], iter + 1, i+1) == 0)
                    {
                        if (j > 0)
                            break;
                        else
                        {
                            if(j > 1)
                                return 0;
                        }
                        // return 0;
                    }
                    else
                    {
                        if(token->type==tEpilog)
                            return 1;
                    }
                }
                j++;
            }
            // printf("Sem bychom nemeli, jsme za koncem radku\n");
        }
    }
    
    //printf("%*s--KONEC prohledavani - navrat z parse\n", iter*2, "");
    return 0;
}

int parse(FILE *f, tParseTree *tree)
{
    if (SkipProlog(f))
        printf("PROLOG OK\n");
    else
    {
        printf("INVALID PROLOG\n");
        return 0;
    }
    tToken token;
    ReadTokenPRINT(f, &token);
    if(token.type == tInvalid)
    {
        printf("INVALID TOKEN\n");
        return 0;
    }
    return parser(f, &token, tree, "statements", 0, 0);
}

/*
int parseRPar(void)
{
    if(token->type == tRPar)
        return 0;
    else
        return 1; // TODO SYNTAX ERROR
}

int parseLPar(void)
{
    if(token->type == tLPar)
        return 0;
    else
        return 1; // TODO SYNTAX ERROR
}

int parseRCurl(void)
{
    if(token->type == tRCurl)
        return 0;
    else
        return 1; // TODO SYNTAX ERROR
}

int parseLCurl(void)
{
    if(token->type == tLCurl)
        return 0;
    else
        return 1; // TODO SYNTAX ERROR
}

void parser(void)
{

}
*/
