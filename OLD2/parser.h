//
//  parser.h
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 02.11.2022.
//

#ifndef parser_h
#define parser_h


#include "token.h"
#include "tstack.h"
#include "symtable.h"


#include <stdio.h>

int readTokenPRINT(FILE *f, tToken token);
tTokenType strToToken(const char *tokenStr);
void prl(char *str);
void nextToken();
void matchTokenAndNext(tTokenType tokType);

void parse();
void parse_programs();
void parse_program();
void parse_statements(tSymTable* st);
void parse_statement(tSymTable* st);
void parse_returnValue(tStack* stack);
void parse_nextTerminal(tSymTable* st);
void parse_preExpression();
void parse_expression(tStack* stack);
void parse_expression2(tStack* stack);
void parse_arguments(tStack* stack);
void parse_argumentVars(tStack* stack);
void parse_parameters(tStack* stack);
void parse_parameters2(tStack* stack);
void parse_term(tStack* stack);
void parse_const(tStack* stack);
void parse_type(tStack* stack);

extern char* tokenName[tMaxToken];
extern int srcLine;
extern FILE* inf;

#endif /* parser_h */
