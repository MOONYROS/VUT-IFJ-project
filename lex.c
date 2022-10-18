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
    sInit, sLiteral, sID, sInt, sFinish, sProlog,
    sFloat, sIexp, sReal, sRexp, sReal2, sRexpS, sInt2, sIexpS,
    sEsc, sOcta1, sOcta2, sHexa1, sHexa2, sPercent
} tState;

#define KEYWORDS 5
char *keyword[] = {"if", "while", "for", "int", "float"}; // TODO

int isKeyword(char *str)
{
    for (int i = 0; i < KEYWORDS; i++)
        if (strcmp(str, keyword[i]) == 0)
            return 1;
    return 0;
}

int isAlpha(char ch)
{
    if ((ch >= 'A') && (ch <= 'Z'))
        return 1;
    if ((ch >= 'a') && (ch <= 'z'))
        return 1;
    return 0;
}

int isDigit(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
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
        if (feof(f))
            return 0;
        if (isWhiteChar(ch))
            return 0;
    }
    SAVECHAR;
    if (strcmp(line, "<?php") != 0)
        return 0;
    ch = fgetc(f);
    while (!feof(f) && isWhiteChar(ch))
    {
        ch = fgetc(f);
    }
    if (feof(f))
        return 0;
    pos = &line[0];
    for (int i = 0; i < 23; i++)
    {
        SAVECHAR;
        ch = fgetc(f);
        if(feof(f))
            return 0;
        if (isWhiteChar(ch))
            return 0;
    }
    SAVECHAR;
    if (strcmp(line, "declare(strict_types=1);") != 0)
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
    
    while (state != sFinish)
    {
        ch = fgetc(f);
        if (feof(f))
            return 0;   // Osetrit potencialne nactena data pred EOF
            
        switch (state) {
            case sInit:
                if (isAlpha(ch))     // TODO
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
                            if (ch == '=')   // Kontrola rovnosti
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
                            break;
                        case '!':
                            ch = fgetc(f);
                            if(ch == '=')
                            {
                                ch = fgetc(f);
                                if (ch == '=')
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
                            if (ch == '=')
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
                            if (ch == '=')
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
                            if (ch == '/')   // Single line comment
                            {
                                while (!feof(f) && (ch != '\n'))
                                {
                                    ch = fgetc(f);
                                    if(ch != '\n')
                                        SAVECHAR;
                                }
                                if (feof(f))
                                {
                                    printf("EOF COMMENT: %s\n", token->data);
                                    state = sFinish;
                                    token->type = tInvalid;
                                    return 0;
                                }
                                printf("COMMENT: %s\n", token->data);
                                pos = token->data;
                            }
                            else if (ch == '*')  // Start of block comment
                            {
                                while (!feof(f))
                                {
                                    ch = fgetc(f);
                                    if(ch == '*')
                                    {
                                        ch = fgetc(f);
                                        if(feof(f))
                                        {
                                            printf ("EOF IN MULTILINE COMMENT\n");
                                            state = sFinish;
                                            token->type = tInvalid;
                                            return 0;
                                        }
                                        else if (ch == '/')
                                        {
                                            printf("MULTILINE COMMENT\n");
                                            break;
                                        }
                                    }
                                }
                                if (feof(f))
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
            case sLiteral:  // prisla nam uvozovka
                if (ch == '\"') // prisla dalsi uvozovka - ukoncujeme retezec
                {
                    state = sFinish;
                    token->type = tLiteral;
                }
                else if ((ch < 32) && (ch >= 0))    // pokud prijde jiny znak z ASCII tabulky, nez z intervalu <32; 255>, jdeme do tInvalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                else if (ch == '\\')    // pokud prisel backslash, presouvame se do sEsc
                {
                    SAVECHAR;
                    state = sEsc;
                }
                else if (ch == '%')
                {
                    SAVECHAR;
                    state = sPercent;
                }
                else    // jakykoliv jiny znak (napriklad EOF) je nepripustny
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sEsc:
                if ((ch >= 48) && (ch <= 55))   // jestlize se jedna o cislo z intervalu <0; 7>, jedna se o oktalove cislo
                {
                    SAVECHAR;
                    state = sOcta1;
                }
                else if (ch == 'x') // prislo x, takze se bude jednat o hexadecimalni cislo
                {
                    SAVECHAR;
                    state = sHexa1;
                }
                else if ((ch == '"') || (ch == 'n') || (ch == 't') || (ch == '$') || (ch == '\\') || (ch == '%'))   // prisel jeden z techto znaku
                {
                    SAVECHAR;
                    state = sLiteral;
                }
                else    // do escape sekvece prisel neplatny znak
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sOcta1:
                if ((ch >= 48) && (ch <= 55))   // jestlize se jedna o cislo z intervalu <0; 7>, jedna se o KOMPLETNI oktalove cislo
                {
                    SAVECHAR;
                    state = sOcta2;
                }
                else    // cokoliv jine je tInvalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
            case sOcta2:
                if ((ch >= 48) && (ch <= 55))   // jestlize se jedna o cislo z intervalu <0; 7>, jedna se o KOMPLETNI oktalove cislo, vracime se zpet do retezce
                {
                    SAVECHAR;
                    state = sLiteral;
                }
                else    // cokoliv jine je tInvalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sHexa1:
                if(((ch >= 48) && (ch <= 57)) || ((ch >= 65) && (ch <= 70)))    // prvni cislo musi byt bud 0-9 (prvni podminka) nebo A-F (druha podminka)
                {
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
                if(((ch >= 48) && (ch <= 57)) || ((ch >= 65) && (ch <= 70)))    // druhe cislo musi byt bud 0-9 (prvni podminka) nebo A-F (druha podminka)
                {
                    SAVECHAR;
                    state = sLiteral;
                }
                else    // cokoliv jine je tInvalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sPercent:
                if ((ch == 'a') || (ch == 'd'))
                {
                    SAVECHAR;
                    state = sLiteral;
                }
                else
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sID:
                SAVECHAR;
                if (isKeyword(token->data))
                    state = sFinish;
                else if (!isAlpha(ch) && !isDigit(ch) && (ch != '_'))
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sInt:
                // if (isDigit(ch))    // jestlize je znak cislo, ulozime ho
                //     SAVECHAR
                while (isDigit(ch))
                {
                    SAVECHAR;
                    ch = fgetc(f);
                }
                if (ch == '.') // jetlize prijde tecka, ulozime ji a presouvame se do sFloat
                {
                    SAVECHAR;
                    state = sFloat;
                }
                else if ((ch == 'e') || (ch == 'E'))    // jestlize prijde e nebo E, jdeme do sIexp
                {
                    SAVECHAR;
                    state = sIexp;
                }
                else    // Pokud ani jedna z podminek nevyhovuje, vracime znak zpatky a cislo (int) je nacteno
                {
                    ungetc(ch, f);
                    state = sFinish;
                    token->type = tInt;
                    printf("PRVNI CISLO\n");
                }
                break;
            case sFloat:    // Co kdyz prijde jenom 234.? je to 234.0 nebo tInvalid?
                if (isDigit(ch))
                {
                    SAVECHAR;
                    state = sReal;
                }
                else    // Zatim za teckou ocekavam cislo, takze pokud neprijde, hazim tInvalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sReal: // Za teckou prislo cislo, takze ted urcite mame realne cislo
                //if (isDigit(ch))
                //    SAVECHAR
                while (isDigit(ch)) // Za tecku prislo dalsi cislo, to ulozime
                {
                    SAVECHAR;
                    fgetc(f);
                }
                if ((ch == 'e') || (ch == 'E')) // Realne exponencialni
                {
                    SAVECHAR;
                    state = sRexp;
                }
                else    // Za cislem za desetinnou teckou prisel neocekavany znak, tak ho vratime zpatky a ulozime nastavime stav sFinish
                {
                    ungetc(ch, f);
                    state = sFinish;
                    token->type = tReal;
                    printf("DRUHE CISLO\n");
                }
                break;
            case sRexp:
                if (isDigit(ch))    // Prislo cislo, takze rovnou presouvame do sReal2
                {
                    SAVECHAR;
                    state = sReal2;
                }
                else if ((ch == '+') || (ch == '-'))    // prislo + nebo -, takze se presouvame do sRexpS
                {
                    SAVECHAR;
                    state = sRexpS;
                }
                else    // ocekavali jsme cislo nebo znak, ani jeden neprisel, takze konec a stav je tInvalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sRexpS:
                if(isDigit(ch)) // Prislo cislo, takze se presouvame do stavu sReal2
                {
                    SAVECHAR;
                    state = sReal2;
                }
                else    // Ocekavali jsme cislo, ktere neprislo, takze stav je tInvalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sReal2:    // jedno cislo nam uz prislo, takze cele cislo je urcite validni
                while (isDigit(ch)) // dokud prichazeji cisla, ukladame je
                {
                    SAVECHAR;
                    ch = fgetc(f);
                }
                ungetc(ch, f);  // prislo nam neco jineho nez cislo, takze se o znak vratime a dame se do stavu sFinish a typ tokenu je tReal2
                state = sFinish;
                token->type = tReal2;
                break;
            case sIexp:    // Prislo e nebo E
                if (isDigit(ch))    // Prislo cislo, takze se presouvame do stavu sInt2
                {
                    SAVECHAR;
                    state = sInt2;
                }
                else if ((ch == '+') || (ch == '-'))    // Prislo + nebo -, takze se presouvame do sIexpS
                {
                    SAVECHAR;
                    state = sIexpS;
                }
                else    // ocekavali jsme cislo nebo znamenko, jinak tInvalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sIexpS:    // Prislo znamenko
                if (isDigit(ch))    // Prislo cislo
                {
                    SAVECHAR;
                    state = sInt2;
                }
                else    // ocekavali jsme cislo, konec, stav tInvalid
                {
                    state = sFinish;
                    token->type = tInvalid;
                }
                break;
            case sInt2:
                while (isDigit(ch)) // dokud prichazeji cisla, ukladame je
                {
                    SAVECHAR;
                    fgetc(f);
                }
                ch = ungetc(ch, f); // neprislo cislo, tento znak vratime a jsme v koncovem stavu
                state = sFinish;
                token->type = tInt2;
                printf("TRETI CISLO\n");
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
