/**
 * @file lex.c
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This file is responsible for lexical analysis. Reads characters and saves them to tokens.
 * @date 2022-9
 */

#include <stdio.h>
#include <string.h>

#include "support.h"
#include "token.h"
#include "lex.h"

#define SAVECHAR {\
    *pos = ch;\
    pos++;\
    *pos = 0;\
    if (strlen(token->data) > (MAX_TOKEN_LEN - CHAR_RESERVE))\
    { \
        token->type = tInvalid; \
        return 1; \
    } \
}

#define SAVECHARP {\
    *pos = ch;\
    pos++;\
    *pos = 0;\
}

typedef enum{   // strings for Keywords
    sInit, sLiteral, sFunctionName, sNullType, sInt, sFinish, sProlog,
    sFloat, sIexp, sReal, sRexp, sReal2, sRexpS, sInt2, sIexpS,
    sEsc, sOcta1, sOcta2, sHexa1, sHexa2, sPercent, sDollar
} tState;

#define KEYWORDS 10
char *keyword[] = {"if", "else", "while", "function", "return", "null", "int", "float", "string", "void"}; // list of all keywords and types
tTokenType keywordToken[] = {tIf, tElse, tWhile, tFunction, tReturn, tNull, tTypeInt, tTypeFloat, tTypeString, tVoid};

#define NULLTYPES 3
char *nullType[] = {"int", "float", "string"};
tTokenType nullTypeToken[] = {tNullTypeInt, tNullTypeFloat, tNullTypeString};

int srcLine = 1;

/**
 * @brief Function turns character in string to upper case.
 * 
 * @param ch character
 * @return char in upper case
 */
char upCase(char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return ch - 32;
    else
        return ch;
}

/**
 * @brief Function turns hexadecimal number do decadic.
 * 
 * @param ch character
 * @return decadic number
 */
int hexToDec(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (ch >= 'A' && ch <= 'F')
        return ch - 55;
    else
        return 0;
}

/**
 * @brief Function reads next character from file.
 * 
 * @param stream file to read from
 * @return read character
 */
char nextChar(FILE* stream)
{
    char ch = fgetc(stream);
    if (ch == '\n')
        srcLine++;
    return ch;
}

/**
 * @brief Function compares whether string is the same as any keyword.
 * 
 * @param str string to compare
 * @return 1 or 0
 */
int isKeyword(char *str)
{
    for (int i = 0; i < KEYWORDS; i++)
        if (strcmp(str, keyword[i]) == 0)
            return i+1;
    return 0;
}

/**
 * @brief Function checks if parameter is null type.
 * 
 * @param str string
 * @return Returns 0 on fail.
 */
int isNullType(char *str)
{
    for (int i = 0; i < NULLTYPES; i++)
        if (strcmp(str, nullType[i]) == 0)
            return i+1;
    return 0;
}

/**
 * @brief Function checks if character is a regular alpha character.
 * 
 * @param ch character
 * @return 1 or 0
 */
int isAlpha(char ch)
{
    if ((ch >= 'A') && (ch <= 'Z'))
        return 1;
    if ((ch >= 'a') && (ch <= 'z'))
        return 1;
    return 0;
}

/**
 * @brief Function checks if character is a number character.
 *
 * @param ch character
 * @return 1 or 0
 */
int isDigit(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return 1;
    return 0;
}

/**
 * @brief Function checks if character is a white character.
 * 
 * @param ch character
 * @return 1 or 0
 */
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

/**
 * @brief Function skips white characters.
 * 
 * @param f file
 * @param ch character
 */
void skipWhites(FILE* f, char* ch)
{
    while (!feof(f) && isWhiteChar(*ch))
    {
        *ch = nextChar(f);
    }

}

/**
 * @brief Function handles loading and checking prolog.
 * 
 * @param f file
 * @return 1 is successful 0 on fail
 */
int SkipProlog(FILE *f)
{
    char ch;
    char line[MAX_TOKEN_LEN] = "";
    char *pos;
    line[0] = 0;
    pos = &line[0];
    *pos = 0;
    
    ch = nextChar(f);
    for (int i = 0; i < 4; i++)
    {
        SAVECHARP;
        ch = nextChar(f);
        if (feof(f))
            return 0;
        if (isWhiteChar(ch))
            return 0;
    }
    SAVECHARP;
    if (strcmp(line, "<?php") != 0)
        return 0;
    ch = nextChar(f);
    skipWhites(f, &ch);
    while (ch != 'd')
    {
        skipWhites(f, &ch);
        if (feof(f))
            return 0;
        if (ch == 'd')
            break;
        else if (ch == '/')
        {
            ch = nextChar(f);
            if (ch == '/')   // Single line comment
            {
                while (!feof(f) && (ch != '\n'))
                {
                    ch = nextChar(f);
                }
                if (feof(f))
                {
                    dbgMsg("EOF in comment in prolog\n");
                    return 0;
                }                
            }
            else if (ch == '*')  // Start of block comment
            {
                while (!feof(f))
                {
                    ch = nextChar(f);
                    if (ch == '*')
                    {
                        ch = nextChar(f);
                        if (feof(f))
                        {
                            dbgMsg("EOF in multiline comment in prolog\n");                            
                            return 0;
                        }
                        else if (ch == '/')
                        {
                            break;
                        }
                    }
                }
                if (feof(f))
                {
                    dbgMsg("EOF in multiline comment in prolog\n");
                    return 0;
                }
            }
            else
            {
                dbgMsg("/ without next / or * in prolog\n");
                return 0;
            }
        }
        else
        {
            dbgMsg("unexpected character in prolog between <?php and declare\n");
            return 0;
        }
        ch = nextChar(f);

    }
    pos = &line[0];
    for (int i = 0; i < 23; i++)
    {
        SAVECHARP;
        ch = nextChar(f);
        if(feof(f))
            return 0;
        if (isWhiteChar(ch))
            return 0;
    }
    SAVECHARP;
    if (strcmp(line, "declare(strict_types=1);") != 0)
        return 0;
    ch = nextChar(f);
    skipWhites(f, &ch);
    if(feof(f))
        return 0;
    ungetc(ch, f);
    return 1;
}

/**
 * @brief Function checks characters and assigns them to tokens.
 * 
 * @param f source file
 * @param token token
 * @return 
 */
int ReadToken(FILE *f, tToken *token)
{
    char ch;
    char *pos = token->data;
    *pos = 0;
    
    tState state = sInit;
    
    while (state != sFinish)
    {
        ch = nextChar(f);
        if (feof(f))
        {
            token->type = tEpilog;
            return 0;   // Take care of potencially unwanted data read before EOF.
        }
            
        switch (state) {
            case sInit:
                if (isAlpha(ch) || (ch == '_'))     // On alpha char or undescore
                {
                    state = sFunctionName;
                    SAVECHAR;
                }
                else if (isDigit(ch))
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
                            break;
                        case '+':
                            state = sFinish;
                            strcpy(token->data, "+");
                            token->type = tPlus;
                            break;
                        case '-':
                            state = sFinish;
                            strcpy(token->data, "-");
                            token->type = tMinus;
                            break;
                        case '.':
                            state = sFinish;
                            strcpy(token->data, ".");
                            token->type = tConcat;
                            break;
                        case '*':
                            state = sFinish;
                            strcpy(token->data, "*");
                            token->type = tMul;
                            break;
                        case '(':
                            state = sFinish;
                            strcpy(token->data, "(");
                            token->type = tLPar;
                            break;
                        case ')':
                            state = sFinish;
                            strcpy(token->data, ")");
                            token->type = tRPar;
                            break;
                        case '{':
                            state = sFinish;
                            strcpy(token->data, "{");
                            token->type = tLCurl;
                            break;
                        case '}':
                            state = sFinish;
                            strcpy(token->data, "}");
                            token->type = tRCurl;
                            break;
                        case ':':
                            state = sFinish;
                            strcpy(token->data, ":");
                            token->type = tColon;
                            break;
                        case ';':
                            state = sFinish;
                            strcpy(token->data, ";");
                            token->type = tSemicolon;
                            break;
                        case ',':
                            state = sFinish;
                            strcpy(token->data, ",");
                            token->type = tComma;
                            break;
                        case '?':
                            ch = nextChar(f);
                            if (ch == '>')   // equality control
                            {
                                while (!feof(f))
                                {
                                    ch = nextChar(f);
                                    if(isWhiteChar(ch) == 0 && !feof(f))
                                    {
                                        ungetc(ch, f);
                                        state = sFinish;
                                        token->type = tInvalid;
                                        strcpy(token->data, "Character after epilog!");
                                        return 1;
                                    }
                                }
                                state = sFinish;
                                token->type = tEpilog;
                                return 0;
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sNullType;
                            }
                            break;
                        case '$':
                            state = sDollar;
                            break;
                        case '=':   // assigment control
                            ch = nextChar(f);
                            if (ch == '=')   // equality control
                            {
                                ch = nextChar(f);
                                if (ch == '=')  // identicality control
                                {
                                    state = sFinish;
                                    strcpy(token->data, "===");
                                    token->type = tIdentical;
                                }
                                else    // if only 2 symbols (==) came, go to tInvalid, because our language does not support it
                                {
                                    state = sFinish;
                                    token->type = tInvalid;
                                }
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sFinish;
                                strcpy(token->data, "=");
                                token->type = tAssign;
                            }
                            break;
                        case '!':
                            ch = nextChar(f);
                            if(ch == '=')
                            {
                                ch = nextChar(f);
                                if (ch == '=')
                                {
                                    state = sFinish;
                                    strcpy(token->data, "!=");
                                    token->type = tNotIdentical;
                                }
                                else    // if only 2 symbols (!=) came, go to tInvalid, because our language does not support it
                                {
                                    state = sFinish;
                                    token->type = tInvalid;
                                }
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sFinish;
                                strcpy(token->data, "!");
                                token->type = tExclamation;
                            }
                            break;
                        case '<':
                            ch = nextChar(f);
                            if (ch == '=')
                            {
                                state = sFinish;
                                strcpy(token->data, "<=");
                                token->type = tLessEq;
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sFinish;
                                strcpy(token->data, "<");
                                token->type = tLess;
                            }
                            break;
                        case '>':
                            ch = nextChar(f);
                            if (ch == '=')
                            {
                                state = sFinish;
                                strcpy(token->data, ">=");
                                token->type = tMoreEq;
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sFinish;
                                strcpy(token->data, ">");
                                token->type = tMore;
                            }
                            break;
                        case '/':
                            SAVECHAR;
                            ch = nextChar(f);
                            if (ch == '/')   // Single line comment
                            {
                                SAVECHAR;
                                while (!feof(f) && (ch != '\n'))
                                {
                                    ch = nextChar(f);
                                    if(ch != '\n')
                                        SAVECHAR;
                                }
                                if (feof(f))
                                {
                                    state = sFinish;
                                    token->type = tEpilog;
                                    return 1;
                                }
                                pos = token->data;
                            }
                            else if (ch == '*')  // Start of block comment
                            {
                                while (!feof(f))
                                {
                                    ch = nextChar(f);
                                    if(ch == '*')
                                    {
                                        ch = nextChar(f);
                                        if(feof(f))
                                        {
                                            dbgMsg ("EOF IN MULTILINE COMMENT\n");
                                            state = sFinish;
                                            token->type = tInvalid;
                                            return 1;
                                        }
                                        else if (ch == '/')
                                        {
                                            pos = token->data;
                                            *pos = '\0';
                                            break;
                                        }
                                    }
                                }
                                if (feof(f))
                                {
                                    dbgMsg("EOF IN MULTILINE COMMENT\n");
                                    state = sFinish;
                                    token->type = tInvalid;
                                    return 1;
                                }
                            }
                            else
                            {
                                ungetc(ch, f);
                                state = sFinish;
                                token->type = tDiv;
                            }
                            break;
                        default:
                            break;
                    }
                }
                break;
            case sLiteral:  // quotation mark was read
                if (ch == '\"') // another quotation mark was read - end of string
                {
                    state = sFinish;
                    token->type = tLiteral;
                }
                else if (ch == '\\')    // with backslash we move to sEsc
                {
                    SAVECHAR;
                    state = sEsc;
                }
                else if (ch >= 32) // (ch <= 255) is alway true
                {
                    SAVECHAR;
                    if (ch == '$') // we cannot read dollar sign normally
                    {
                        state = sFinish;
                        token->type = tInvalid;
                    }                        
                }
                else    // any other character is invalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sEsc:
                if ((ch >= '0') && (ch <= '9'))   // if it's a number from interval <0; 7>, it has to be an octal number
                {
                    SAVECHAR;
                    state = sOcta1;
                }
                else if (ch == 'x') // on x, we'll definitely have hexadecimal number
                {
                    SAVECHAR;
                    state = sHexa1;
                }
                else if ((ch == '"') || (ch == 'n') || (ch == 't') || (ch == '$') || (ch == '\\')) // any one of these characters was read
                {
                    SAVECHAR;
                    state = sLiteral;
                }
                else    // invalid character in sequence
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sOcta1:
                if ((ch >= '0') && (ch <= '9'))   // if it's a number from interval <0; 7>, it is a COMPLETE octal number
                {
                    SAVECHAR;
                    state = sOcta2;
                }
                else    // anything else is invalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sOcta2:
                if ((ch >= '0') && (ch <= '9'))   // if it's a number from interval <0; 7>, it is a COMPLETE octal number, we'll get back to literal
                {
                    SAVECHAR;
                    int len = strlen(token->data);
                    int octNum = (token->data[len - 3] - '0') * 100;
                    octNum += (token->data[len - 2] - '0') * 10;
                    octNum += (token->data[len - 1] - '0') * 1;
                    if (octNum < 0 || octNum > 255)
                    {
                        state = sFinish;
                        token->type = tInvalid;
                        break;
                    }
                    state = sLiteral;
                }
                else    // anything else is invalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sHexa1:
                if(((ch >= '0') && (ch <= '9')) || ((upCase(ch) >= 'A') && (upCase(ch) <= 'F')))    // first character has to either be from interval <0; 9> or <A; F>
                {
                    ch = upCase(ch);
                    SAVECHAR;
                    state = sHexa2;
                }
                else    // cokoliv jine je tInvalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sHexa2:
                if (((ch >= '0') && (ch <= '9')) || ((upCase(ch) >= 'A') && (upCase(ch) <= 'F')))    // second character has the same condition
                {
                    ch = upCase(ch);
                    SAVECHAR;
                    int len = strlen(token->data);
                    int hexNum = hexToDec(token->data[len - 2]) * 16;
                    hexNum += hexToDec(token->data[len - 1]) * 1;
                    char tmp[10];
                    sprintf(tmp, "%03d", hexNum);
                    token->data[len - 3] = tmp[0];
                    token->data[len - 2] = tmp[1];
                    token->data[len - 1] = tmp[2];
                    state = sLiteral;
                }
                else    // anything else is invalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sFunctionName:
                while ((isAlpha(ch)) || (isDigit(ch)) || (ch == '_'))
                {
                    SAVECHAR;
                    ch = nextChar(f);
                }
                ungetc(ch, f);
                int i = isKeyword(token->data);
                if(i > 0)
                    token->type = keywordToken[i-1];
                else
                    token->type = tFuncName;
    
                state = sFinish;
                break;
            case sNullType:
                while (isAlpha(ch))
                {
                    SAVECHAR;
                    ch = nextChar(f);
                }
                ungetc(ch, f);
                int j = isNullType(token->data);
                if(j > 0)
                    token->type = nullTypeToken[j-1];
                else
                    token->type = tInvalid;
    
                state = sFinish;
                break;
            case sInt:             
                while (isDigit(ch)) // we'll save all the numbers
                {
                    SAVECHAR;
                    ch = nextChar(f);
                }
                if (ch == '.') // on dot, we save and move to sFloat
                {
                    SAVECHAR;
                    state = sFloat;
                }
                else if ((ch == 'e') || (ch == 'E'))    // on e or E, we move to sIexp
                {
                    SAVECHAR;
                    state = sIexp;
                }
                else    // If any of the above conditions do not match, we stay wit tInt
                {
                    ungetc(ch, f);
                    state = sFinish;
                    token->type = tInt;
                }
                break;
            case sFloat:
                if (isDigit(ch))
                {
                    SAVECHAR;
                    state = sReal;
                }
                else    // There has to be number behind dot
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sReal: // There is a number after the dot, se we definitely have real number
                while (isDigit(ch)) // We'll save all the other numbers
                {
                    SAVECHAR;
                    ch = nextChar(f);
                }
                if ((ch == 'e') || (ch == 'E')) // Real exponential
                {
                    SAVECHAR;
                    state = sRexp;
                }
                else    // Unexpected character after number, so we're done reading real number
                {
                    ungetc(ch, f);
                    state = sFinish;
                    token->type = tReal;
                }
                break;
            case sRexp:
                if (isDigit(ch))    // Number was read, we move straight to sReal2
                {
                    SAVECHAR;
                    state = sReal2;
                }
                else if ((ch == '+') || (ch == '-'))    //  + or - was read, so we move to sRexpS
                {
                    SAVECHAR;
                    state = sRexpS;
                }
                else    // unexpected character
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sRexpS:
                if(isDigit(ch)) // Number was read, we move to sReal2
                {
                    SAVECHAR;
                    state = sReal2;
                }
                else    // unexpected character
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sReal2:    // We already have one number, so it's definitely already a valid number
                while (isDigit(ch)) // we save all the coming numbers
                {
                    SAVECHAR;
                    ch = nextChar(f);
                }
                ungetc(ch, f);  // unexpected character finishes reading
                state = sFinish;
                token->type = tReal2;
                break;
            case sIexp:    // e or E was read
                if (isDigit(ch))    // number was read, we move to sInt2
                {
                    SAVECHAR;
                    state = sInt2;
                }
                else if ((ch == '+') || (ch == '-')) // + or -was read, so we move to sRexpS
                {
                    SAVECHAR;
                    state = sIexpS;
                }
                else    // unexpected character
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sIexpS:    // sign was read
                if (isDigit(ch))    // number was read
                {
                    SAVECHAR;
                    state = sInt2;
                }
                else    // unexpected character
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sInt2:
                while (isDigit(ch)) // we save all the oncoming numbers 
                {
                    SAVECHAR;
                    ch = nextChar(f);
                }
                ch = ungetc(ch, f); // character was not a number, so we're finished reading
                state = sFinish;
                token->type = tInt2;
                break;
            case sDollar:
                if ((isAlpha(ch)) || (ch == '_'))
                {
                    while ((isAlpha(ch)) || (isDigit(ch)) || (ch == '_')) // variable has to start with aplhabet character or with underscore

                    {
                        SAVECHAR;
                        ch = nextChar(f);
                    }
                }
                else
                {
                    SAVECHAR;
                    state = sFinish;
                    token->type = tInvalid;
                    break;
                }
                ungetc(ch, f);
                state = sFinish;
                token->type = tIdentifier;
                break;
            default:
                dbgMsg("NEOSETRENY STAV: Sem bychom se nikdy nemeli dostat.\n");
        }
    }
    
    return 1;
}
