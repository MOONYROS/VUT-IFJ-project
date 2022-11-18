//
//  parser.h
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 02.11.2022.
//

#ifndef parser_h
#define parser_h

#include <stdio.h>
#include "token.h"

#define SYNTAXRULES 59 //63
#define RULEITEMS   13

char *rule[SYNTAXRULES][RULEITEMS];

typedef struct parseTree{

    struct parseTree *next;
	int is_nonterminal;
	union
	{
		struct {
			tTokenType tType;
			char *data;
		} term;
		struct {
			struct parseTree *tree;
			char *name;
		} nonterm;
	};
} tParseTree;

int parse(FILE *f, tParseTree **tree);

void printParseTree(tParseTree* tree, int level);

#endif /* parser_h */
