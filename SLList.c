//
// Created by jonys on 20.11.2022.
//

#include "SLList.h"
#include <malloc.h>
#include <stdio.h>

void list_init(tlist * list){
    list->first = NULL;
    list->active = NULL;
}

void list_first(tlist * list){
    list->active = list->first;
}

void list_next(tlist * list){
    if(list->active != NULL){
        list->active = list->active->next_item;
    }
}

void insert_first(tlist * list, tToken token){
    list_item_ptr new_item = malloc(sizeof(struct list_item));
    if (new_item == NULL){
        printf("Chyba pri alokaci pameti");
        return;
    }

    new_item->token = token;
    new_item->next_item = list->first;
    list->first = new_item;
}

void insert_after(tlist * list, tToken token){
    if(list->active != NULL){
        list_item_ptr new_item = malloc(sizeof(struct list_item));
        if (new_item == NULL){
            printf("Chyba pri alokaci pameti");
            return;
        }
        new_item->token = token;
        new_item->next_item = list->active->next_item;
        list->active->next_item = new_item;
    }
}

tToken get_first(tlist * list){
    if (list->first != NULL) {
        return (list->first->token);
    }
}

tToken get_active(tlist * list){
    if (list->active != NULL){
        return list->active->token;
    }
}

bool list_is_active(tlist * list){
    if(list->active == NULL){
        return false;
    }
    else{
        return true;
    }
}

void dispose(tlist * list){
    list_item_ptr item;
    while (list->first != NULL){
        item = list->first;
        list->first = item->next_item;
        free(item);
    }
}