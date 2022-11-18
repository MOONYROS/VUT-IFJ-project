//
// Created by jonys on 17.11.2022.
//

#ifndef GEN_STACK_H
#define GEN_STACK_H

#include <stdbool.h>
#include "generator.h"
#include "token.h"
#include "lex.h"

#define MAX 20

//struktura zasobniku
typedef struct {
    int top;
    tToken items[MAX];
}tStack;

//funkce pro praci se zasobnikem
void stack_init(tStack *stack);
void stack_push(tStack *stack, tToken item);
tToken stack_top(tStack *stack);
tToken stack_pop(tStack *stack);
bool stack_empty(tStack *stack);

#endif //GEN_STACK_H
