/**
 * @file token.c
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This file for basic function for tokens.
 * @date 2022-9
 */

#include "token.h"

char *tokenName[tMaxToken] = {
    "tNone",
    "tTypeInt", "tTypeFloat", "tTypeString", "tNullTypeInt", "tNullTypeFloat", "tNullTypeString", "tVoid",
    "tIf", "tElse", "tWhile", "tFunction", "tReturn", "tNull",
    "tInvalid", "tIdentifier", "tFuncName", "tType", "tNullType",
    "tPlus", "tMinus", "tConcat","tMul", "tDiv", "tLPar", "tRPar", "tLCurl", "tRCurl", "tColon", "tSemicolon", "tComma",
    "tAssign", "tIdentical",
    "tExclamation", "tNotIdentical",
    "tLess", "tLessEq", "tMore", "tMoreEq",
    "tInt", "tReal", "tReal2", "tInt2",
    "tLiteral", "tEpilog"
};

/**
 * @brief Function that checks if two tokens have compatible types.
 * 
 * @param dst destination token type
 * @param src source token type
 * @return int 1 if compatible, otherwise 0
 */
int typeIsCompatible(tTokenType dst, tTokenType src)
{
    if (dst == src)
        return 1;

    switch (dst)
    {
    case tNullTypeInt:
        if (src == tTypeInt || src == tNullType || src == tInt || src == tInt2 || src == tNull)
            return 1;
        break;
    case tNullTypeFloat:
        if (src == tTypeFloat || src == tNullType || src == tReal || src == tReal2 || src == tNull)
            return 1;
        break;
    case tNullTypeString:
        if (src == tTypeString || src == tNullType || src == tLiteral || src == tNull)
            return 1;
        break;
    default:
        break;
    }
    return 0;
}