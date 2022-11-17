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

int priority(tToken *token, tStack * stack) {
    int token_prio;
    int stack_prio;

    if (stack_top(stack).type == tMul || stack_top(stack).type == tDiv){
        stack_prio = 2;
    }
    else if (stack_top(stack).type == tPlus || stack_top(stack).type == tMinus){
        stack_prio = 1;
    }
    else{
        stack_prio = 0;
    }

    if (token->type == tMul || token->type == tDiv){
        token_prio = 2;
    }
    else if (token->type == tPlus || token->type == tMinus){
        token_prio = 1;
    }
    else{
        token_prio = 0;
    }

    if (token_prio > stack_prio){
        return 2;
    }
    else if (token_prio == stack_prio){ //pozor na zavorky ty se muzou pushovat na sebe
        return 1;
    }
    else{
        return 0;
    }
}

void infix_to_postfix(tToken * token){

    tStack *stack;
    stack_init(stack);

    while (token) {
        switch (token->type) {

            case tLPar:
                if (priority(token, stack) >= 1) {
                    stack_push(stack, *token);
                } else {
                    stack_pop(stack);
                    if (priority(token, stack) >= 1)
                        stack_push(stack, *token);
                    else {
                        stack_pop(stack);
                        stack_push(stack, *token);
                    }
                }

            case tRPar:
                while (stack_top(stack).type != tLPar) {
                    stack_pop(stack);
                }
                stack_pop(stack);

            case tMul:
                if (priority(token, stack) == 2) {
                    stack_push(stack, *token);
                } else {
                    stack_pop(stack);
                    stack_push(stack, *token);
                }

            case tDiv:
                if (priority(token, stack) == 2) {
                    stack_push(stack, *token);
                } else {
                    stack_pop(stack);
                    stack_push(stack, *token);
                }

            case tPlus:
                if (priority(token, stack) == 2) {
                    stack_push(stack, *token);
                } else if (priority(token, stack) == 1) {
                    stack_pop(stack);
                    stack_push(stack, *token);
                } else {
                    stack_pop(stack);
                    if (priority(token, stack) == 2)
                        stack_push(stack, *token);
                    else {
                        stack_pop(stack);
                        stack_push(stack, *token);
                    }
                }

            case tMinus:
                if (priority(token, stack) == 2) {
                    stack_push(stack, *token);
                } else if (priority(token, stack) == 1) {
                    stack_pop(stack);
                    stack_push(stack, *token);
                } else {
                    stack_pop(stack);
                    if (priority(token, stack) == 2)
                        stack_push(stack, *token);
                    else {
                        stack_pop(stack);
                        stack_push(stack, *token);
                    }
                }

            default:
                continue;
                
        }
    }
}
