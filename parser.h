/**
 * @file parser.h
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This header file does all the parsing for the compiler, works woth symbol table, controls semantics and generates code.
 * @date 2022-10
 */

#ifndef parser_h
#define parser_h

#include <stdio.h>
#include "token.h"
#include "tstack.h"
#include "symtable.h"

void parse();
//void parse_programs();
void parse_program();
void parse_statements(tSymTable* st, tStack* stack);
void parse_statement(tSymTable* st, tStack* stack);
void parse_functionCall(tSymTable* st, tStack* stack);
void parse_returnValue(tSymTable* st, tStack* stack);
void parse_nextTerminal(tSymTable* st);
void parse_preExpression(tSymTable* st, tStack* stack);
void parse_expression(tSymTable* st, tStack* stack);
void parse_expression2(tSymTable* st, tStack* stack);
void parse_arguments(tStack* stack);
void parse_argumentVars(tStack* stack);
void parse_parameters(tSymTable* st, tStack* stack);
void parse_parameters2(tSymTable* st, tStack* stack);
void parse_term(tSymTable* st, tStack* stack);
void parse_minusTerm(tStack* stack);
void parse_const(tStack* stack);
void parse_type(tStack* stack);

#endif /* parser_h */
