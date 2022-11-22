//
// Created by jonys on 17.11.2022.
//

#ifndef IFJCODE22_STACK_H
#define IFJCODE22_STACK_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX 40

typedef enum {
    eMul, eDiv,
    ePlus, eMinus, eConcat,
    eLess, eMore, eLessEq, eMoreEq,
    eIdentical, eNotIdentical,
    eLPar, eRPar,
    eIdentifier,
    eDollar
}exSymbol;

typedef enum {
    eInt, eInt2, eReal, eReal2, eLiteral
}exType;

typedef struct exItem{
    exType type;
    exSymbol symbol;
}tExItem;

typedef struct tStackItem{
    struct tStackItem * next;
    tExItem item;
}*tStackItem;

//struktura zasobniku
typedef struct tstack {
    int size;
    tStackItem top;
}tStack;

//funkce pro praci se zasobnikem
tStack * stack_init(int size);
void stack_push(tStack *stack, tExItem * item);
tExItem * stack_top(tStack *stack);
tExItem *  stack_pop(tStack *stack);
bool stack_empty(tStack *stack);
void stack_dispose(tStack * stack);

#endif //IFJCODE22_STACK_H
