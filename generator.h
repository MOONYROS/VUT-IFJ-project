//
// Created by jonys on 17.11.2022.
//

#ifndef IFJCODE22_GENERATOR_H
#define IFJCODE22_GENERATOR_H
#include <stdbool.h>
#include "token.h"


bool is_operator(tToken *token);
int priority(tToken *token);
void infix_to_postfix(tToken *token);

#endif //IFJCODE22_GENERATOR_H
