//
//  parser.h
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 02.11.2022.
//

#ifndef parser_h
#define parser_h

#include <stdio.h>

//const char *moje[] = {
//  "ja"
//};

static const char *rule[62][13] = {
    {"if_statement",            "tIf", "tLPar", "condition", "tRPar", "tLCurl", "statements", "tRCurl", "tElse", "tLCurl", "statements", "tRCurl", ""},
    {"while_statement",         "tWhile", "tLPar", "condition", "tRPar", "tLCurl", "statements", "tRCurl", ""},
    {"functionDeclaration",     "tFunction", "tFuncName", "tLPar", "arguments", "tRPar", "tColon", "type", "tRCurl", "statements", "tLCurl", ""},
    {"arguments",               "variable", "variables", ""},
    {"arguments",               "EPS", ""},
    {"functionCall",            "tFuncName", "tLPar", "parameters", "tRPar", ""},
    {"parameters",              "parameter", "parameters2", ""},
    {"parameters",              "EPS", ""},
    {"parameters2",             "tComma", "parameter", "parameters2", ""},
    {"parameters2",             "EPS", ""},
    {"parameter",               "factor", ""},
    {"statements",              "statement", "statements", ""},
    {"statementsX",             "EPS", ""},
    {"statement",               "if_statement", ""},
    {"statement",               "while_statement", ""},
    {"statement",               "functionCall", "tSemicolon", ""},
    {"statement",               "tReturn", "expression", "tSemicolon", ""},
    {"statement",               "expression", "statement2", "tSemicolon", ""},
    {"statement2",              "tAssign", "statement3", ""},
    {"statement2",              "EPS", ""},
    {"statement3",              "expression", ""},
    {"statement3",              "functionCall", ""},
    {"condition",               "functionCall", "condition2", ""},
    {"condition",               "expression", "condition2", ""},
    {"condition2",              "relational_operators", "expression", ""},
    {"condition2",              "EPS", ""},
    {"expression",              "term", "expression2", ""},
    {"expression",              "EPS", ""},
    {"expression2",             "tPlus", "expression", ""},
    {"expression2",             "tMinus", "expression", ""},
    {"expression2",             "EPS", ""},
    {"term",                    "const", "term2", ""},
    {"term",                    "variable", "term2", ""},
    {"term2",                   "tMul", "factor", ""},
    {"term2",                   "tConcat", "factor", ""},
    {"term2",                   "tDiv", "factor", ""},
    {"term2",                   "EPS", ""},
    {"factor",                  "variable", ""},
    {"factor",                  "const", ""},
    {"variables",               "tComma", "variable", "variables", ""},
    {"variables",               "EPS", ""},
    {"variable",                "type", "tIdentifier", ""},
    {"variable",                "tIdentifier", ""},
    {"const",                   "tInt", ""},
    {"const",                   "tReal", ""},
    {"const",                   "tReal2", ""},
    {"const",                   "tInt2", ""},
    {"const",                   "tNull", ""},
    {"const",                   "tLiteral", ""},
    {"type",                    "tNullTypeInt", ""},
    {"type",                    "tNullTypeFloat", ""},
    {"type",                    "tNullTypeString", ""},
    {"type",                    "tTypeInt", ""},
    {"type",                    "tTypeFloat", ""},
    {"type",                    "tTypeString", ""},
    {"type",                    "tVoid", ""},
    {"relational_operators",    "tLess", ""},
    {"relational_operators",    "tLessEq", ""},
    {"relational_operators",    "tMore", ""},
    {"relational_operators",    "tMoreEq", ""},
    {"relational_operators",    "tIdentical", ""},
    {"relational_operators",    "tNotIdentical", ""},
};

typedef struct parseTree{
    char name[1024];
    struct parseTree *next;
} tParseTree;

int parse(FILE *f, tParseTree *tree);

/*
// PARSING FUNCTIONS
int parseType(void);
int parseRelationOp(void);
int parseConst(void);
int parseVariables(void);
int parseVariable(void);
int parseFactor(void);
int parseTerm2(void);
int parseTerm(void);
int parseExpression2(void);
int parseExpression(void);
int parseCondition2(void);
int parseCondition(void);
int parseStatement2(void);
int parseStatement(void);
int parseStatements(void);
int parseParameter(void);
int parseParameters2(void);
int parseParameters(void);
int parseFunctionCall(void);
int parseArguments(void);
int parseFunctionDec(void);
int parseWhile(void);
int parseIf(void);

//PARSING TOKENS
int parseRPar(void);
int parseLPar(void);
int parseRCurl(void);
int parseLCurl(void);
*/
 
#endif /* parser_h */
