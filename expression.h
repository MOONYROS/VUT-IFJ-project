//
//  expression.h
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#ifndef expression_h
#define expression_h

#include <stdio.h>
#include <stdbool.h>

// function to evaluate expression
// returns type of and exp expression
// generates code to ??? tohle musime upresnit, jak budeme delat
// in case of semantic failure calls errorExit(msg, errno)
tTokenType evalExp(tStack* expStack, tSymTable* symTable);

// Converts token type (e.g. tMul) to index of precedence table.
int typeToIndex(tTokenType token);

void expression();

#endif /* expression_h */
