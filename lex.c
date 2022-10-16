//
//  lex.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#include <stdio.h>
#include <string.h>

#include "token.h"
#include "lex.h"

#define SAVECHAR {\
    *pos = ch;\
    pos++;\
    *pos = 0;\
}

typedef enum{   // strings for Keywords
    sInit, sLiteral, sID, sInt, sFinish, sProlog
} tState;

#define KEYWORDS 5
char *keyword[] = {"if", "while", "for", "int", "float"}; // TODO

int isKeyword(char *str)
{
    for(int i = 0; i < KEYWORDS; i++)
        if(strcmp(str, keyword[i]) == 0)
            return 1;
    return 0;
}

int isAlpha(char ch)
{
    if((ch >= 'A') && (ch <= 'Z'))
        return 1;
    if((ch >= 'a') && (ch <= 'z'))
        return 1;
    return 0;
}

int isDigit(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return 1;
    return 0;
}

int isWhiteChar(char ch)
{
    switch (ch) {
        case ' ':
        case '\n':
        case '\r':
        case '\t':
            return 1;
    }
    return 0;
}

int SkipProlog(FILE *f)
{
    char ch;
    char line[255];
    char *pos;
    pos = &line[0];
    *pos = 0;
    
    ch = fgetc(f);
    for (int i = 0; i < 4; i++)
    {
        SAVECHAR;
        ch = fgetc(f);
        if(feof(f))
            return 0;
        if(isWhiteChar(ch))
            return 0;
    }
    SAVECHAR;
    if(strcmp(line, "<?php") != 0)
        return 0;
    ch = fgetc(f);
    while (!feof(f) && isWhiteChar(ch))
    {
        ch = fgetc(f);
    }
    if(feof(f))
        return 0;
    pos = &line[0];
    for (int i = 0; i < 23; i++)
    {
        SAVECHAR;
        ch = fgetc(f);
        if(feof(f))
            return 0;
        if(isWhiteChar(ch))
            return 0;
    }
    SAVECHAR;
    if(strcmp(line, "declare(strict_types=1);") != 0)
        return 0;
    ch = fgetc(f);
    while (!feof(f) && isWhiteChar(ch))
    {
        ch = fgetc(f);
    }
    if(feof(f))
        return 0;
    ungetc(ch, f);
    return 1;
}

int ReadToken(FILE *f, tToken *token)
{
    char ch;
    char *pos = token->data;
    *pos = 0;
    
    tState state = sInit;
    
    while(state != sFinish)
    {
        ch = fgetc(f);
        if(feof(f))
            return 0;   // Osetrit potencialne nactena data pred EOF
            
        switch (state) {
            case sInit:
                if(isAlpha(ch))     // TODO
                {
                    state = sID;
                    SAVECHAR;
                }
                else if (isDigit(ch))   // TODO
                {
                    state = sInt;
                    SAVECHAR;
                }
                else
                {
                    switch (ch) {
                        case ' ':
                        case '\t':
                            break;
                        case '\"':
                            state = sLiteral;
                        case '+':
                            state = sFinish;
                            token->type = tPlus;
                        case '-':
                            state = sFinish;
                            token->type = tMinus;
                        case '*':
                            state = sFinish;
                            token->type = tMul;
                        case '(':
                            state = sFinish;
                            token->type = tLPar;
                        case ')':
                            state = sFinish;
                            token->type = tRPar;
                        case '{':
                            state = sFinish;
                            token->type = tLCurl;
                        case '}':
                            state = sFinish;
                            token->type = tRCurl;
                        case ':':
                            state = sFinish;
                            token->type = tColon;
                        case ';':
                            state = sFinish;
                            token->type = tSemicolon;
                        case ',':
                            state = sFinish;
                            token->type = tComma;
                        case '?':
                            state = sFinish;
                            token->type = tQuestion;
                        case '$':
                            state = sFinish;
                            token->type = tDollar;
                        case '=':   // Kontrola prirazeni
                            ch = fgetc(f);
                            if(ch == '=')   // Kontrola rovnosti
                            {
                                ch = fgetc(f);
                                if (ch == '=')  // Kontrola identicnosti
                                {
                                    state = sFinish;
                                    token->type = tIdentical;
                                }
                                else
                                {
                                    ungetc(ch, f);
                                    state = sFinish;
                                    token->type = tEquals;
                                }
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sFinish;
                                token->type = tAssign;
                            }
                        case '!':
                            ch = fgetc(f);
                            if(ch == '=')
                            {
                                ch = fgetc(f);
                                if(ch == '=')
                                {
                                    state = sFinish;
                                    token->type = tNotEq;
                                }
                                else
                                {
                                    ungetc(ch, f);
                                    state = sFinish;
                                    token->type = tNeg;
                                }
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sFinish;
                                token->type = tExclamation;
                            }
                        case '<':
                            ch = fgetc(f);
                            if(ch == '=')
                            {
                                state = sFinish;
                                token->type = tLessEq;
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sFinish;
                                token->type = tLess;
                            }
                        case '>':
                            ch = fgetc(f);
                            if(ch == '=')
                            {
                                state = sFinish;
                                token->type = tMoreEq;
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sFinish;
                                token->type = tMore;
                            }
                        case '/':
                            SAVECHAR;
                            ch = fgetc(f);
                            SAVECHAR;
                            if(ch == '/')   // Single line comment
                            {
                                while (!feof(f) && (ch != '\n'))
                                {
                                    ch = fgetc(f);
                                    if(ch != '\n')
                                        SAVECHAR;
                                }
                                if(feof(f))
                                {
                                    printf("EOF COMMENT: %s\n", token->data);
                                    state = sFinish;
                                    token->type = tInvalid;
                                    return 0;
                                }
                                printf("COMMENT: %s\n", token->data);
                                pos = token->data;
                            }
                            else if(ch == '*')  // Start of block comment
                            {
                                while (!feof(f))
                                {
                                    ch = fgetc(f);
                                    if(ch == '*')
                                    {
                                        ch = fgetc(f);
                                        if(feof(f))
                                        {
                                            printf("EOF IN MULTILINE COMMENT\n");
                                            state = sFinish;
                                            token->type = tInvalid;
                                            return 0;
                                        }
                                        else if(ch == '/')
                                        {
                                            printf("MULTILINE COMMENT\n");
                                            break;
                                        }
                                    }
                                }
                                if(feof(f))
                                {
                                    printf("EOF IN MULTILINE COMMENT\n");
                                    state = sFinish;
                                    token->type = tInvalid;
                                    return 0;
                                }
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sFinish;
                                token->type = tDiv;
                            }
                        default:
                            break;
                    }
                }
                break;
            case sLiteral:      // TODO
                if(ch == '\"')
                    state = sFinish;
                else if((ch < 32) && (ch >= 0))
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                else
                {
                    SAVECHAR;
                }
                break;
            case sID:
                SAVECHAR;
                if(isKeyword(token->data))
                    state = sFinish;
                else if(!isAlpha(ch) && !isDigit(ch) && (ch != '_'))
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sInt:
                break;
            default:
                printf("NEOSETRENY STAV: Sem bychom se nikdy nemeli dostat.\n");
        }
    }
    
    /*
    while ((ch != EOF) && (ch != '\n') && (ch != '\r') && (ch != '\t') && (ch != ' ')) {
        *pos = ch;
        pos++;
        ch = fgetc(f);
    }
    *pos = '\0';
    if(ch == EOF)
    {
        if(pos == token->data)
            return 0;
        else
        {
            return 1;
        }
    }
    */
    
    return 1;
}
