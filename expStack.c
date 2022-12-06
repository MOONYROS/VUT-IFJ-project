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

#include <stdlib.h>
#include <string.h>
#include "expStack.h"
#include "support.h"

/**
 * @brief Function that calculates stack length.
 * 
 * @param stack stack
 * @return unsigned int length of the stack 
 */
unsigned int expStackLength(tExpStack *stack)
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

/**
 * @brief Function for initialization of stack
 * 
 * @param stack stack
 */
void expStackInit(tExpStack **stack)
{
    *stack = safe_malloc(sizeof(tExpStack));
    (*stack)->top = NULL;
}

/**
 * @brief Function for disposal of stack.
 * 
 * @param stack stack
 */
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

/**
 * @brief Function for pushing expressions on stack.
 * 
 * @param stack stack
 * @param exp expression
 */
void expStackPush(tExpStack* stack, tExpression* exp)
{
    if (stack == NULL)
        return;

    tExpStackItem* newItem = safe_malloc(sizeof(tExpStackItem));
    newItem->exp = safe_malloc(sizeof(tExpression));
    newItem->exp->data = safe_malloc(MAX_TOKEN_LEN);
    newItem->exp->type = exp->type;
    strcpy(newItem->exp->data, exp->data);
    newItem->exp->isNonTerminal = exp->isNonTerminal;
    newItem->next = stack->top;
    stack->top = newItem;
}

/**
 * @brief Function for popping out of stack.
 * 
 * @param stack stack
 * @param exp expression
 * @return true if pop was successful
 * @return false if stack is not initialized or is empty
 */
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

/**
 * @brief Function that tells whether the stack is empty.
 * 
 * @param stack stack
 * @return true if stack is empty
 * @return false if stack is not empty
 */
bool expIsEmpty(tExpStack *stack)
{
    return stack == NULL ? true : stack->top == NULL; 
}

/**
 * @brief Function that shows what's on top of the stack.
 * 
 * @param stack stack
 * @param exp expression
 */
void expStackTop(tExpStack *stack, tExpression *exp)
{
    if (expIsEmpty(stack))
        return;

    strcpy(exp->data, stack->top->exp->data);
    exp->type = stack->top->exp->type;
    exp->isNonTerminal = stack->top->exp->isNonTerminal;
}
