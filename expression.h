/**
 * @file expression.h
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This file is responsible for handling expressions and generating code for them.
 * @date 2022-12
 */

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
bool isDefined(tSymTable *table, tExpression *exp);
bool checkOpDefinition(tSymTable *table, tExpression *top, tExpression *third);
double getFloatValue(tExpression *exp);
int getIntValue(tExpression *exp);
void convertFloatToInt(tSymTable *table, tExpression *exp);
void convertIntToFloat(tSymTable *table, tExpression *exp);
void convertNullToInt(tSymTable *table, tExpression *exp);
char *typeToString(char *tmpStr, tExpression *exp);
void convertNullToFloat(tSymTable  *table, tExpression *Exp);
tTokenType variableType(tSymTable *table, tExpression *exp);
tTokenType getResultType(tSymTable *table, tExpression *top, tExpression *third, tTokenType operation);


// Functions actually doing something
void rearrangeStack(tSymTable *table, tStack *stack);
tTokenType evalExp(char* tgtVar, tStack *expStack, tSymTable *table);

/**
 * @brief 
 * 
 */
int typeToIndex(tTokenType token);





#endif /* expression_h */
