/**
 * @file lex.h
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This file is responsible for lexical analysis. Reads characters and saves them to tokens.
 * @date 2022-11
 */

#ifndef lex_h
#define lex_h

#include <stdio.h>

#define CHAR_RESERVE 80

int SkipProlog(FILE *f);
int ReadToken(FILE *f, tToken *token);

#endif /* lex_h */
