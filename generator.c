//
// Created by jonys on 17.11.2022.
//

#include "generator.h"
#include "token.h"
#include "stack.h"
#include "SLList.h"


int token_type(tToken * token){
    switch (token->type) {
        case tIdentifier | tNull | tInt2 | tInt | tReal2 | tReal:  return 1;
        case tPlus | tMinus | tMul | tDiv | tLPar | tRPar | tLess | tLessEq | tMore | tMoreEq | tIdentical | tNotIdentical: return 2;
        case tLiteral: return 3;
        case tConcat: return 4;
        default: return 0;
    }
}

int priority(tToken *token, tStack * stack) {
    int token_prio;
    int stack_prio;
    tToken *tmp;
    (*tmp) = stack_top(stack);

    //priority tokenu
    if (token_type(token) == 2){
        switch (token->type) {
            case tMul | tDiv: token_prio = 4;
            case tPlus | tMinus: token_prio = 3;
            case tLess | tLessEq | tMore | tMoreEq: token_prio = 2;
            case tIdentical | tNotIdentical: token_prio = 1;
            case tLPar | tRPar: token_prio = 0;
            default: return 0;
        }
    }
    if (token_type(token) == 4){
        token_prio = 3;
    }

    //priority na stacku
    if(token_type(tmp) == 2){
        switch (tmp->type) {
            case tMul | tDiv: stack_prio = 4;
            case tPlus | tMinus: stack_prio = 3;
            case tLess | tLessEq | tMore | tMoreEq: stack_prio = 2;
            case tIdentical | tNotIdentical: stack_prio = 1;
            case tLPar | tRPar: stack_prio = 0;
            default: return 0;
        }
    }
    if (token_type(tmp) == 4){
        stack_prio = 3;
    }

    //vyhodnoceni priorit
    if (token_prio > stack_prio){
        return 2;
    }
    if (token_prio == stack_prio){
        return 1;
    }
    else{
        return 0;
    }
}

void infix_to_postfix(tlist * list){

    tlist * postfix;
    tToken * token;
    tStack * stack;
    stack_init(stack);
    list_init(postfix);
    list_first(list);

    //prochazeni listu s tokeny expressionu
    while (list_is_active(list) == true) {
        //vytahneme aktivni token
         (* token) = get_active(list);

        switch (token->type) {

            case tMul | tDiv:
                if (priority(token, stack) == 2){
                    stack_push(stack, *token);
                }
                else{
                    insert_after(postfix,stack_top(stack));
                    stack_pop(stack);
                    stack_push(stack,*token);
                }


            case tPlus | tMinus:
                if (priority(token, stack) == 2){
                    stack_push(stack, *token);
                }
                else if (priority(token, stack) == 1){
                    insert_after(postfix,stack_top(stack));
                    stack_pop(stack);
                    stack_push(stack,*token);
                }
                else{
                    insert_after(postfix,stack_top(stack));
                    stack_pop(stack);
                    if (priority(token,stack) == 1){
                        insert_after(postfix,stack_top(stack));
                        stack_pop(stack);
                        stack_push(stack,*token);
                    }
                    else{
                        stack_push(stack,*token);
                    }
                }


            case tLess | tLessEq | tMore | tMoreEq:
                if (priority(token, stack) == 2){
                    stack_push(stack, *token);
                }
                else if (priority(token, stack) == 1){
                    insert_after(postfix,stack_top(stack));
                    stack_pop(stack);
                    stack_push(stack,*token);
                }
                else{
                    insert_after(postfix,stack_top(stack));
                    stack_pop(stack);
                    if (priority(token, stack) == 0){
                        insert_after(postfix,stack_top(stack));
                        stack_pop(stack);
                        if (priority(token, stack) == 1){
                            insert_after(postfix,stack_top(stack));
                            stack_pop(stack);
                            stack_push(stack,*token);
                        }
                        else{
                            stack_push(stack,*token);
                        }
                    }
                    else if (priority(token, stack) == 1){
                        insert_after(postfix,stack_top(stack));
                        stack_pop(stack);
                        stack_push(stack,*token);
                    }
                    else{
                        stack_push(stack,*token);
                    }
                }


            case tIdentical | tNotIdentical: ;


            case tIdentifier | tNull | tInt2 | tInt | tReal2 | tReal:;


            case tConcat:;


            case tLiteral:;


            case tRPar:;


            //case tLPar:;
        }

        //posuneme aktivitu
        list_next(list);
    }
}

void tri_code_gen(tlist * list){
//TODO
}