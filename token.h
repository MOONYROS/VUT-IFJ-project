/**
 * @file token.h
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This header file for token.c, contains token types and token structure.
 * @date 2022-9
 */

#ifndef token_h
#define token_h

#define MAX_TOKEN_LEN 1024

typedef enum {
    tNone,
    tTypeInt, tTypeFloat, tTypeString, tNullTypeInt, tNullTypeFloat, tNullTypeString, tVoid,
    tIf, tElse, tWhile, tFunction, tReturn, tNull,
    tInvalid, tIdentifier, tFuncName, tType, tNullType,
    tPlus, tMinus, tConcat, tMul, tDiv, tLPar, tRPar, tLCurl, tRCurl, tColon, tSemicolon, tComma,
    tAssign, tIdentical,
    tExclamation, tNotIdentical,
    tLess, tLessEq, tMore, tMoreEq,
    tInt, tReal, tReal2, tInt2,
    tLiteral,
    tEpilog,
    tMaxToken
} tTokenType;

typedef struct{
    tTokenType type;
    char* data;
} tToken;

// vrati 0/1 (false/true) jestli se da do dst ulozit src
int typeIsCompatible(tTokenType dst, tTokenType src);

extern tToken token;
extern char* tokenName[tMaxToken];

#endif /* token_h */
