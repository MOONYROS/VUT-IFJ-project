//
// Created by jonys on 17.11.2022.
//

#ifndef IFJCODE22_GENERATOR_H
#define IFJCODE22_GENERATOR_H
#include <stdbool.h>
#include "token.h"
#include "stack.h"


typedef struct list_item{
    tToken token;
    struct list_item *next_item;
} *list_item_ptr;

typedef struct {
    list_item_ptr first;
    list_item_ptr active;
}list;

void list_init(list * list);
void list_first(list * list);
void dispose(list * list);
void list_next(list * list);
bool list_is_active(list * list);
void insert_first(list * list, tToken token);
void insert_after(list * list, tToken token);

int priority(tToken *token, tStack * stack);
void infix_to_postfix(tToken *token);
void tri_code_gen(list * list);

#endif //IFJCODE22_GENERATOR_H
