#include "token.h"

char *tokenName[tMaxToken] = {
    "tTypeInt", "tTypeFloat", "tTypeString", "tNullTypeInt", "tNullTypeFloat", "tNullTypeString",
    "tIf", "tElse", "tWhile", "tFunction", "tReturn", "tVoid", "tNull",
    "tInvalid", "tIdentifier", "tFuncName", "tType", "tNullType",
    "tPlus", "tMinus", "tConcat","tMul", "tDiv", "tLPar", "tRPar", "tLCurl", "tRCurl", "tColon", "tSemicolon", "tComma",
    "tAssign", "tIdentical",
    "tExclamation", "tNotIdentical",
    "tLess", "tLessEq", "tMore", "tMoreEq",
    "tInt", "tReal", "tReal2", "tInt2",
    "tLiteral", "tEpilog"
};
