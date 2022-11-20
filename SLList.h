//
// Created by jonys on 20.11.2022.
//

#ifndef IFJCODE22_SLLIST_H
#define IFJCODE22_SLLIST_H

#include <stdbool.h>
#include "token.h"

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
tToken get_first(list * list);
tToken get_active(list * list);

#endif //IFJCODE22_SLLIST_H
