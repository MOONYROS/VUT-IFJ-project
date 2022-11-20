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

void infix_to_postfix(list * list){

    list_first(list);
    while (list_is_active(list) == true) {
        //vytahneme aktivni token
        tToken token = get_active(list);

        //TODO

        //posuneme aktivitu
        list_next(list);
    }
}

void tri_code_gen(list * list){
//TODO
}