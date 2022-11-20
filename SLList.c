//
// Created by jonys on 20.11.2022.
//

#include "SLList.h"
#include <malloc.h>
#include <stdio.h>

void list_init(list * list){
    list->first = NULL;
    list->active = NULL;
}

void list_first(list * list){
    list->active = list->first;
}

void list_next(list * list){
    if(list->active != NULL){
        list->active = list->active->next_item;
    }
}

void insert_first(list * list, tToken token){
    list_item_ptr new_item = malloc(sizeof(struct list_item));
    if (new_item == NULL){
        printf("Chyba pri alokaci pameti");
        return;
    }

    new_item->token = token;
    new_item->next_item = list->first;
    list->first = new_item;
}

void insert_after(list * list, tToken token){
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

bool list_is_active(list * list){
    if(list->active == NULL){
        return false;
    }
    else{
        return true;
    }
}

void dispose(list * list){
    list_item_ptr item;
    while (list->first != NULL){
        item = list->first;
        list->first = item->next_item;
        free(item);
    }
}