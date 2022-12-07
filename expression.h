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
bool isNull(tSymTable *table, tExpression *exp);
bool isDefined(tSymTable *table, tExpression *exp);

double getFloatValue(tExpression *exp);
int getIntValue(tExpression *exp);

void convertFloatToInt(tSymTable *table, tExpression *exp);
void convertIntToFloat(tSymTable *table, tExpression *exp);
void convertNullToInt(tSymTable *table, tExpression *exp);
void convertNullToFloat(tSymTable  *table, tExpression *Exp);

char *typeToString(char *tmpStr, tExpression *exp);
bool checkOpDefinition(tSymTable *table, tExpression *top, tExpression *third);
tTokenType variableType(tSymTable *table, tExpression *exp);
tTokenType getResultType(tSymTable *table, tExpression *top, tExpression *third, tTokenType operation);
tTokenType const2type(tTokenType ctype);
int typeToIndex(tTokenType token);

void rearrangeStack(tSymTable *table, tStack *stack);

tTokenType evalExp(char* tgtVar, tStack *expStack, tSymTable *table);

#endif /* expression_h */
