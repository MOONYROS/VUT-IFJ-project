//
// Created by jonys on 17.11.2022.
//

#include "generator.h"
#include "token.h"
#include "stack.h"
#include "SLList.h"


int priority(tToken *token, tStack * stack) {
    int token_prio;
    int stack_prio;

    //prirazeni priority tokenu na stacku
    if (stack_top(stack).type == tMul || stack_top(stack).type == tDiv){
        stack_prio = 2;
    }
    else if (stack_top(stack).type == tPlus || stack_top(stack).type == tMinus){
        stack_prio = 1;
    }
    else{
        stack_prio = 0;
    }

    //prirazeni priority aktualnimu tokenu
    if (token->type == tMul || token->type == tDiv){
        token_prio = 2;
    }
    else if (token->type == tPlus || token->type == tMinus){
        token_prio = 1;
    }
    else{
        token_prio = 0;
    }

    //vyhodnoceni priorit
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

    list *list;
    tStack *stack;
    list_init(list);
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
                    insert_after(list,stack_top(stack));
                    list_next(list);
                    stack_pop(stack);
                    stack_push(stack, *token);
                }

            case tDiv:
                if (priority(token, stack) == 2) {
                    stack_push(stack, *token);
                } else {
                    insert_after(list,stack_top(stack));
                    list_next(list);
                    stack_pop(stack);
                    stack_push(stack, *token);
                }

            case tPlus:
                if (priority(token, stack) == 2) {
                    stack_push(stack, *token);
                } else if (priority(token, stack) == 1) {
                    insert_after(list,stack_top(stack));
                    list_next(list);
                    stack_pop(stack);
                    stack_push(stack, *token);
                } else {
                    insert_after(list,stack_top(stack));
                    list_next(list);
                    stack_pop(stack);
                    if (priority(token, stack) == 2)
                        stack_push(stack, *token);
                    else {
                        insert_after(list,stack_top(stack));
                        list_next(list);
                        stack_pop(stack);
                        stack_push(stack, *token);
                    }
                }

            case tMinus:
                if (priority(token, stack) == 2) {
                    stack_push(stack, *token);
                } else if (priority(token, stack) == 1) {
                    insert_after(list,stack_top(stack));
                    list_next(list);
                    stack_pop(stack);
                    stack_push(stack, *token);
                } else {
                    insert_after(list,stack_top(stack));
                    list_next(list);
                    stack_pop(stack);
                    if (priority(token, stack) == 2)
                        stack_push(stack, *token);
                    else {
                        insert_after(list,stack_top(stack));
                        list_next(list);
                        stack_pop(stack);
                        stack_push(stack, *token);
                    }
                }

            case tIdentifier:
                if (list_is_active(list) == false){
                    insert_first(list, *token);
                    list_first(list);
                }
                else{
                    insert_after(list, *token);
                    list_next(list);
                }

            default:
                continue;

        }
    }
}

void tri_code_gen(list * list){
//TODO
}


void main(){

    tToken *token;
    list *list;

    infix_to_postfix(token);
    tri_code_gen(list);
    dispose(list);

}