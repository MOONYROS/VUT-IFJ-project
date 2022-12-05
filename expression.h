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
#include "expStack.h"


// function to evaluate expression
// returns type of and exp expression
// generates code to ??? tohle musime upresnit, jak budeme delat
// in case of semantic failure calls errorExit(msg, errno)
tTokenType const2type(tTokenType ctype);


// Auxiliary functions for semantic controls etc.
bool isOperator(tExpression *exp);
bool isNumberOp(tExpression *exp);
bool isRelationalOp(tExpression *exp);
bool isStringOp(tExpression *exp);
bool isConst(tExpression *exp);
bool isVar(tSymTable *table, tExpression *exp);
bool isNumber(tSymTable *table, tExpression *exp);
bool isString(tSymTable *table, tExpression *exp);
bool isReal(tSymTable *table, tExpression *exp);
bool isInt(tSymTable *table, tExpression *exp);
bool isNonTerminal(tExpression *exp);
bool isNullTypeVar(tSymTable *table, tExpression *exp);
tTokenType variableType(tSymTable *table, tExpression *exp);
bool isDefined(tSymTable *table, tExpression *exp);
bool checkOpDefinition(tSymTable *table, tExpression *top, tExpression *third);
tTokenType getResultType(tSymTable *table, tExpression *top, tExpression *third, tTokenType operation);


// Functions actually doing something
void rearrangeStack(tSymTable *table, tStack *stack);
tTokenType evalExp(char* tgtVar, tStack *expStack, tSymTable *table);

// Converts token type (e.g. tMul) to index of precedence table.
int typeToIndex(tTokenType token);

extern int prgPass;
extern const char funcPrefixName[];
extern tSymTable gst;

#endif /* expression_h */
