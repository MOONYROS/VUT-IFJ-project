//
//  parser.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 02.11.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "support.h"
#include "token.h"
#include "tstack.h"

extern char* tokenName[tMaxToken];

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

void tstack_free(tStack** stack)
{
    if (*stack != NULL)
    {
        tstack_deleteItems(*stack);
        safe_free(*stack);
    }
    *stack = NULL;
}

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

bool tstack_isEmpty(tStack* stack)
{
    return tstack_count(stack) == 0;
}

bool tstack_isFull(tStack* stack)
{
    return tstack_count(stack) >= MAX_STACK;
}

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

// Stack hodne na steroidech
void tstack_insertAfter(tStack* stack, tStackItem* active, tToken inserted)
{
    if (stack != NULL)
    {
        tStackItem* tmp = stack->top;
        while (tmp != active)
            tmp = tmp->next;
        tStackItem* newItem = safe_malloc(sizeof(tStackItem));
        newItem->token = inserted;
        // can be null
        newItem->next = tmp->next;
        tmp->next = newItem;
    }
}