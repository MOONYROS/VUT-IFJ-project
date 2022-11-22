//
// Created by jonys on 17.11.2022.
//

#include <stdbool.h>
#include "stack.h"


tStack *stack_init(int size){
    tStack * stack = malloc(sizeof(struct tStack));
    if (stack == NULL){
        return NULL;
    }
    stack->size = size;
    stack->top = NULL;
    return stack;
}

void stack_push(tStack * stack, tExItem * item){

}

tExItem * stack_top(tStack *stack){

}

tExItem * stack_pop(tStack *stack){

}

bool stack_empty(tStack *stack){

}

void stack_dispose(tStack * stack){

}