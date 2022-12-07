/**
 * @file tstack.c
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This file contains functions for token stack.
 * @date 2022-11
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "support.h"
#include "token.h"
#include "tstack.h"

/**
 * @brief Function for initialization of a stack.
 * 
 * @return tStack* 
 */
tStack* tstack_init()
{
    tStack* st = safe_malloc(sizeof(tStack));
    if (st != NULL)
    {
        st->top = NULL;
        st->last = NULL;
    }
    else
        errorExit("cannot allocate token stack", CERR_INTERNAL);

    return st;
}

/**
 * @brief Function for deallocation of a stack.
 * 
 * @param stack stack
 */
void tstack_free(tStack** stack)
{
    if (*stack != NULL)
    {
        tstack_deleteItems(*stack);
        safe_free(*stack);
    }
    *stack = NULL;
}

/**
 * @brief Function for deletion of all the items in stack. 
 * 
 * @param stack stack
 */
void tstack_deleteItems(tStack* stack)
{
    if (stack != NULL)
    {
        tStackItem* item = stack->top;
        while (item != NULL)
        {
            tStackItem* next = item->next;
            safe_free(item->token.data);
            safe_free(item);
            item = next;
        }
        stack->top = NULL;
        stack->last = NULL;
    }
}

/**
 * @brief Function for pushing to stack.
 * 
 * @param stack stack 
 * @param token token
 */
void tstack_push(tStack* stack, tToken token)
{
    if (stack != NULL)
    {
        if (tstack_isFull(stack))
            errorExit("parser stack is full", CERR_INTERNAL);
        tStackItem* item = safe_malloc(sizeof(tStackItem));
        if (item != NULL)
        {
            item->token.type = token.type;
            item->token.data = safe_malloc(strlen(token.data) + 1);
            strcpy(item->token.data, token.data);
            if (stack->top == NULL)
                stack->last = item;
            item->next = stack->top;
            stack->top = item;
        }
    }
}

/**
 * @brief Function to push token to the bottom of stack
 * 
 * @param stack stack
 * @param token token
 */
void tstack_pushl(tStack* stack, tToken token)
{
    if (stack != NULL)
    {
        if (tstack_isFull(stack))
            errorExit("parser stack is full", CERR_INTERNAL);
        tStackItem* item = safe_malloc(sizeof(tStackItem));
        if (item != NULL)
        {
            item->token.type = token.type;
            item->token.data = safe_malloc(strlen(token.data) + 1);
            strcpy(item->token.data, token.data);
            item->next = NULL;
            if (stack->last == NULL)
                stack->top = item;
            else
                stack->last->next = item;
            stack->last = item;
        }
    }
}

/**
 * @brief Function for poping out of stack.
 * 
 * @param stack stack
 * @param token token
 * @return true if pop was successful
 * @return false if stack is empty or uninitalized
 */
bool tstack_pop(tStack* stack, tToken *token)
{
    if (token == NULL)
        return false;

    if (stack != NULL)
    {
        tStackItem* toDelete = stack->top;
        if (toDelete != NULL)
        {
            token->type = toDelete->token.type;
            if (toDelete->token.data != NULL)
            {
                strcpy(token->data, toDelete->token.data);
                safe_free(toDelete->token.data);
            }
            
            stack->top = toDelete->next;
            safe_free(toDelete);
        }
        else
            return false;
        if (stack->top == NULL)
            stack->last = NULL;
        return true;
    }
    else
    {
        token->type = tNone;
        token->data = NULL;
        return false;
    }
}

/**
 * @brief Function for checking if stack is empty.
 * 
 * @param stack stack
 * @return true if stack is empty
 * @return false if 1 or more items are in stack
 */
bool tstack_isEmpty(tStack* stack)
{
    return tstack_count(stack) == 0;
}

/**
 * @brief Function for checking if stack is full
 * 
 * @param stack stack
 * @return true if stack is full
 * @return false in other cases
 */
bool tstack_isFull(tStack* stack)
{
    return tstack_count(stack) >= MAX_STACK;
}

/**
 * @brief Function counts the number of items in stack.
 * 
 * @param stack stack
 * @return int count of items
 */
int tstack_count(tStack* stack)
{
    int cnt = 0;
    if (stack != NULL)
    {
        tStackItem* item = stack->top;
        while (item != NULL)
        {
            cnt++;
            item = item->next;
        }
    }
    return cnt;
}

/**
 * @brief Function that looks onto the top of stack.
 * 
 * @param stack stack
 * @return tToken* pointer for token
 */
tToken* tstack_peek(tStack* stack)
{
    if (stack != NULL)
    {
        if (stack->top != NULL)
        {
            return &(stack->top->token);
        }
    }
    return NULL;
}

/**
 * @brief Function for printing out stack contents.
 * 
 * @param stack stack
 */
void tstack_print(tStack* stack)
{
    if (stack != NULL)
    {
        tStackItem* item = stack->top;
        // dbgMsg("TOKEN STACK:");
        while (item != NULL)
        {
            dbgMsg(" [%s %s]", tokenName[item->token.type], item->token.data);
            item = item->next;
        }
        dbgMsg("\n");
    }
}

/**
 * @brief Function for inserting behind active element in stack.
 * 
 * @param stack stack
 * @param active active item in stack
 * @param inserted inserted item
 */
void tstack_insertAfter(tStack* stack, tStackItem* active, tToken inserted)
{
    if (stack != NULL)
    {
        tStackItem* tmp = stack->top;
        while (tmp != active)
            tmp = tmp->next;
        tStackItem* newItem = safe_malloc(sizeof(tStackItem));
        newItem->token.data = safe_malloc(MAX_TOKEN_LEN);
        newItem->token.type = inserted.type;
        strcpy(newItem->token.data, inserted.data);
        // can be null
        newItem->next = tmp->next;
        tmp->next = newItem;
    }
}