//
//  expression.h
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#ifndef expression_h
#define expression_h

#include <stdio.h>
#include <stdbool.h>
#include "tstack.h"
#include "token.h"
#include "symtable.h"

// function to evaluate expression
// returns type of and exp expression
// generates code to ??? tohle musime upresnit, jak budeme delat
// in case of semantic failure calls errorExit(msg, errno)
tTokenType evalExp(tStack* exp, tSymTable* st);
tTokenType const2type(tTokenType ctype);

bool isOperator(tToken *token);
bool isNumberOp(tToken *token);
bool isRelationalOp(tToken *token);
bool isStringOp(tToken *token);
bool isConst(tToken *token);
bool isVar(tToken *token);
bool isNumber(tSymTable *table, tToken *token);
bool isString(tSymTable *table, tToken *token);
bool isReal(tSymTable *table, tToken *token);
bool isInt(tSymTable *table, tToken *token);
tTokenType variableType(tSymTable *table, tToken *token);
bool isDefined(tSymTable *table, tToken *token);
bool checkOpDefinition(tSymTable *table, tToken *third, tToken *top);
tTokenType getResultType(tSymTable *table, tToken *top, tToken *third, tTokenType operation);
void rearrangeStack(tStack *stack);
tTokenType expression(tStack *expStack, tSymTable *table);

// Converts token type (e.g. tMul) to index of precedence table.
int typeToIndex(tTokenType token);

extern const char *expResultName;

#endif /* expression_h */
