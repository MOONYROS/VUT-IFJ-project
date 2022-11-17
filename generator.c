//
// Created by jonys on 17.11.2022.
//

#include "generator.h"
#include "token.h"
#include "stack.h"


bool is_operator(tToken * token){
    if (token->type == tMinus || token->type == tPlus || token->type == tMul || token->type == tDiv){
        return true;
    }
    else{
        return false;
    }
}

int priority(tToken *token){
    if (token->type == tMul || token->type == tDiv){
        return 2;
    }
    else if (token->type == tPlus || token->type == tMinus){
        return 1;
    }
    else{
        return 0;
    }
}

void infix_to_postfix(tToken * token){
    char postfix_exp[20];
    int i;
    tStack *stack;
    stack_init(stack);

    while (token) {
        if (is_operator(token) == true){
            switch (token->type) {
                case tLPar:
                    if (priority(token) > priority(stack_top(stack))){

                    }
                    stack_push(stack, *token);

            }
        }
        else if (token->type == tIdentifier){
            return;
        }
    }
}