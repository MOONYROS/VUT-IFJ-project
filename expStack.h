/**
 * @file expStack.h
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This file is an expression stack (different from parser stack).
 * @date 2022-12
 */

#ifndef expstack_h
#define expstack_h

#include <stdbool.h>

#include "token.h"

typedef struct expression {
    char *data;
    tTokenType type;
    bool isNonTerminal;
} tExpression;

typedef struct stackItem {
    tExpression *exp;
    struct stackItem *next;
} tExpStackItem;


typedef struct expStack {
    tExpStackItem *top;
    int size;
} tExpStack;

unsigned int expStackLength(tExpStack *stack);
void expStackInit(tExpStack **stack);
void expStackDispose(tExpStack *stack);
void expStackPush(tExpStack *stack, tExpression *exp);
void expStackTop(tExpStack *stack, tExpression *exp);
bool expStackPop(tExpStack *stack, tExpression *exp);
bool expIsEmpty(tExpStack *stack);

#endif // expstack_h