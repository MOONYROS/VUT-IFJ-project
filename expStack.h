/**
 * @file expStack.h
 * @author Ondrej Koumar
 * @brief expression stack (different from parser stack)
 * @version 0.1
 * @date 2022-12-02
 * 
 * @copyright Copyright (c) 2022
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