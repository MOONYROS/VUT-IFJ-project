//
// Created by jonys on 17.11.2022.
//

#ifndef IFJCODE22_GENERATOR_H
#define IFJCODE22_GENERATOR_H
#include <stdbool.h>
#include "token.h"
#include "stack.h"


int priority(tToken *token, tStack * stack);
int token_type(tToken * token);
void infix_to_postfix(list * list);
void tri_code_gen(list * list);

#endif //IFJCODE22_GENERATOR_H
