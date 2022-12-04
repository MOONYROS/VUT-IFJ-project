//
//  token.h
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

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

#endif /* token_h */
