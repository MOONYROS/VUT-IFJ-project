//
//  lex.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#include <stdio.h>
#include <string.h>

#include "lex.h"

#define SAVECHAR {\
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
char *keyword[] = {"if", "else", "while", "function", "return", "null", "int", "float", "string", "void"}; // seznam vsech keywordu a typu naseho jazyka
tTokenType keywordToken[] = {tIf, tElse, tWhile, tFunction, tReturn, tNull, tTypeInt, tTypeFloat, tTypeString, tVoid};

#define NULLTYPES 3
char *nullType[] = {"int", "float", "string"};
tTokenType nullTypeToken[] = {tNullTypeInt, tNullTypeFloat, tNullTypeString};

int srcLine = 1;

char nextChar(FILE* stream)
{
    char ch = fgetc(stream);
    if (ch == '\n')
        srcLine++;
    return ch;
}

// porovna, jestli je retezec stejny s nekterym z klicovych slov
int isKeyword(char *str)
{
    for (int i = 0; i < KEYWORDS; i++)
        if (strcmp(str, keyword[i]) == 0)
            return i+1;
    return 0;
}

int isNullType(char *str)
{
    for (int i = 0; i < NULLTYPES; i++)
        if (strcmp(str, nullType[i]) == 0)
            return i+1;
    return 0;
}

// kotroluje, jestli je znak alpha znakem
int isAlpha(char ch)
{
    if ((ch >= 'A') && (ch <= 'Z'))
        return 1;
    if ((ch >= 'a') && (ch <= 'z'))
        return 1;
    return 0;
}

// kotroluje, jestli je znak cislem
int isDigit(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return 1;
    return 0;
}

// kotroluje, jestli je znak bilym znakem
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

void skipWhites(FILE* f, char* ch)
{
    while (!feof(f) && isWhiteChar(*ch))
    {
        *ch = nextChar(f);
    }

}

int SkipProlog(FILE *f)
{
    char ch;
    char line[255]="";
    char *pos;
    line[0] = 0;
    pos = &line[0];
    *pos = 0;
    
    ch = nextChar(f);
    for (int i = 0; i < 4; i++)
    {
        SAVECHAR;
        ch = nextChar(f);
        if (feof(f))
            return 0;
        if (isWhiteChar(ch))
            return 0;
    }
    SAVECHAR;
    if (strcmp(line, "<?php") != 0)
        return 0;
    ch = nextChar(f);
    skipWhites(f, &ch);
    if (feof(f))
        return 0;
    // tady je potreba jeste osetrit komentare uprostred prologu
    pos = &line[0];
    for (int i = 0; i < 23; i++)
    {
        SAVECHAR;
        ch = nextChar(f);
        if(feof(f))
            return 0;
        if (isWhiteChar(ch))
            return 0;
    }
    SAVECHAR;
    if (strcmp(line, "declare(strict_types=1);") != 0)
        return 0;
    ch = nextChar(f);
    skipWhites(f, &ch);
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
        ch = nextChar(f);
        if (feof(f))
        {
            token->type = tEpilog;
            return 0;   // Osetrit potencialne nactena data pred EOF
        }
            
        switch (state) {
            case sInit:
                if (isAlpha(ch) || (ch == '_'))     // Pokud prisel alpha znak nebo podtrzitko
                {
                    state = sFunctionName;
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
                            if (ch == '>')   // Kontrola rovnosti
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
                        case '=':   // Kontrola prirazeni
                            ch = nextChar(f);
                            if (ch == '=')   // Kontrola rovnosti
                            {
                                ch = nextChar(f);
                                if (ch == '=')  // Kontrola identicnosti
                                {
                                    state = sFinish;
                                    strcpy(token->data, "===");
                                    token->type = tIdentical;
                                }
                                else    // pokud prisly pouze 2 symboly =, jdeme do tInvalid, protoze to nas jazyk nepodporuje
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
                                else    // pokud prisly pouze 2 symboly (!=), jdeme do tInvalid, protoze to nas jazyk nepodporuje
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
                            // SAVECHAR;
                            if (ch == '/')   // Single line comment
                            {
                                while (!feof(f) && (ch != '\n'))
                                {
                                    ch = nextChar(f);
                                    if(ch != '\n')
                                        SAVECHAR;
                                }
                                if (feof(f))
                                {
                                    dbgMsg("EOF COMMENT: %s\n", token->data);
                                    state = sFinish;
                                    token->type = tInvalid;
                                    return 1;
                                }
                                //dbgMsg("COMMENT: %s\n", token->data);
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
                                            //dbgMsg("MULTILINE COMMENT\n");
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
            /* SPATNE NASTAVENE PODMINKY A PROBIHA SPATNE VYHODNOCOVANI ZNAKU */
            // aaa OPRAVIT aaa
            case sLiteral:  // prisla nam uvozovka
                if (ch == '\"') // prisla dalsi uvozovka - ukoncujeme retezec
                {
                    state = sFinish;
                    token->type = tLiteral;
                }
                // aaa tohle je jen temporary fix, je potreba to opravit aaa
                else if (ch == '\\')    // pokud prisel backslash, presouvame se do sEsc
                {
                    SAVECHAR;
                    state = sEsc;
                }
                else if (ch >= 32) // (ch <= 255) je vzdycky TRUE, takze neni treba zapisovat
                {
                    SAVECHAR;
                }
                //else if ((ch < 32) && (ch >= 0))    // pokud prijde jiny znak z ASCII tabulky, nez z intervalu <32; 255>, jdeme do tInvalid
                //{
                //    state = sFinish;
                //    token->type = tInvalid;
                //}
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
                break;
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
                // if (isDigit(ch))    // jestlize je znak cislo, ulozime ho
                //     SAVECHAR
                while (isDigit(ch))
                {
                    SAVECHAR;
                    ch = nextChar(f);
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
                    // dbgMsg("PRVNI CISLO\n");
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
                    ch = nextChar(f);
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
                    // dbgMsg("DRUHE CISLO\n");
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
                    ch = nextChar(f);
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
                    ch = nextChar(f);
                }
                ch = ungetc(ch, f); // neprislo cislo, tento znak vratime a jsme v koncovem stavu
                state = sFinish;
                token->type = tInt2;
                // dbgMsg("TRETI CISLO\n");
                break;
            case sDollar:
                while ((isAlpha(ch)) || (isDigit(ch)) || (ch == '_'))
                {
                    SAVECHAR;
                    ch = nextChar(f);
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
