//
//  token.h
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#ifndef token_h
#define token_h

typedef enum{
    tTypeInt, tTypeFloat, tTypeString, tNullTypeInt, tNullTypeFloat, tNullTypeString,
    tIf, tElse, tWhile, tFunction, tReturn, tVoid, tNull,
    tInvalid, tIdentifier, tKeyword, tFuncName, tType, tNullType,
    tPlus, tMinus, tConcat, tMul, tDiv, tLPar, tRPar, tLCurl, tRCurl, tColon, tSemicolon, tComma,
    tQuestion, tDollar,
    tAssign, tIdentical,
    tExclamation, tNotIdentical,
    tLess, tLessEq, tMore, tMoreEq,
    tInt, tReal, tReal2, tInt2,
    tLiteral,
    tEpilog
} tTokenType;




typedef struct{
    tTokenType type;
    char data[1024];
} tToken;

#endif /* token_h */
