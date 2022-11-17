//
// Created by jonys on 17.11.2022.
//

#include <stdio.h>
#include <stdbool.h>
#include "stack.h"

void stack_init(tStack * stack){
    stack->top = -1;
}

void stack_push(tStack * stack, tToken item){
    if (stack->top == MAX-1){
        printf("Stack overflow\n");
    } else{
        stack->top++;
        stack->items[stack->top] = item;
    }
}

tToken stack_top(tStack *stack){
    tToken *top;
    if(stack->top != -1){
        *top = stack->items[stack->top];
        return *top;
    }
}

tToken stack_pop(tStack *stack){
    if(stack->top == -1) {
        printf("Stack underflow");
    } else{
        stack->top--;
        return stack->items[stack->top];
    }
}

bool stack_empty(tStack *stack){
    if(stack->top == -1){
        return true;
    } else{
        return false;
    }
}