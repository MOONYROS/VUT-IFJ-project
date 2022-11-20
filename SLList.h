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
}tlist;

void list_init(tlist * list);
void list_first(tlist * list);
void dispose(tlist * list);
void list_next(tlist * list);
bool list_is_active(tlist * list);
void insert_first(tlist * list, tToken token);
void insert_after(tlist * list, tToken token);
tToken get_first(tlist * list);
tToken get_active(tlist * list);

#endif //IFJCODE22_SLLIST_H
