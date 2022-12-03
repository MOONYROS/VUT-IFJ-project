/**
 * @file expStack.h
 * @author Ondrej Koumar
 * @brief expression stack (different from parser stack)
 * @version 0.1
 * @date 2022-12-02
 * 
 * @copyright Copyright (c) 2022
 */

#include <stdlib.h>
#include <string.h>
#include "expStack.h"
#include "support.h"

unsigned int stackLength(tExpStack *stack)
{
    if (stack == NULL)
        return 0;

    int count = 0;
    tExpStackItem *tmp = stack->top;
    while (tmp != NULL)
    {
        count++;
        tmp = tmp->next;
    }
    return count;
}

void expStackInit(tExpStack *stack)
{
    stack = safe_malloc(sizeof(tExpStack));
    stack->top = NULL;
}

void expStackDispose(tExpStack *stack)
{
    if (stack == NULL)
        return;
    
    tExpStackItem *toDelete = stack->top;
    tExpStackItem *next;
    while (toDelete != NULL)
    {
        next = toDelete->next;   
        safe_free(toDelete->exp->data);
        safe_free(toDelete);
        toDelete = next;
    }
    stack->top = NULL;
}

void expStackPush(tExpStack *stack, tExpression exp)
{
    if (stack == NULL)
        return;

    tExpStackItem *newItem = safe_malloc(sizeof(tExpStackItem));
    newItem->exp->data = safe_malloc(sizeof(MAX_TOKEN_LEN));
    newItem->exp->type = exp.type;
    strcpy(newItem->exp->data, exp.data);
    newItem->next = stack->top;
    stack->top = newItem;
}

bool expStackPop(tExpStack *stack, tExpression *exp)
{
    if (stack == NULL)
        return false;
    if (stack->top == NULL)
        return false;

    tExpStackItem *toDelete = stack->top;
    if (toDelete->exp->data != NULL) 
    {
        strcpy(exp->data, toDelete->exp->data);
        safe_free(toDelete->exp->data);
    }   
    exp->isNonTerminal = toDelete->exp->isNonTerminal;
    exp->type = toDelete->exp->type;
    stack->top = toDelete->next;
    safe_free(toDelete);
    return true;
}

bool isEmpty(tExpStack *stack)
{
    return stack == NULL ? true : stack->top == NULL; 
}

void expStackTop(tExpStack *stack, tExpression *exp)
{
    if (isEmpty(stack))
        return;

    strcpy(exp->data, stack->top->exp->data);
    exp->type = stack->top->exp->type;
    exp->isNonTerminal = stack->top->exp->isNonTerminal;
}
