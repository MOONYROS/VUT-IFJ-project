//
//  parser.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 02.11.2022.
//

#ifndef tstack_h
#define tstack_h

#include <stdbool.h>
#include "token.h"

#define MAX_STACK   500

typedef struct StackItem
{
    tToken token;
    struct StackItem* next;
} tStackItem;

typedef struct
{
    tStackItem* top;
    tStackItem* last;
} tStack;

tStack* tstack_init();
void tstack_free(tStack** stack);
void tstack_deleteItems(tStack* stack);
void tstack_push(tStack* stack, tToken token);
void tstack_pushl(tStack* stack, tToken token);
bool tstack_pop(tStack* stack, tToken *token);
bool tstack_isEmpty(tStack* stack);
bool tstack_isFull(tStack* stack);
int tstack_count(tStack* stack);
tToken* tstack_peek(tStack* stack);
void tstack_print(tStack* stack);

#endif // tstack_h