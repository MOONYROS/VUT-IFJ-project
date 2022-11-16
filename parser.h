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

#define SYNTAXRULES 62
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

/*
// PARSING FUNCTIONS
int parseType(void);
int parseRelationOp(void);
int parseConst(void);
int parseVariables(void);
int parseVariable(void);
int parseFactor(void);
int parseTerm2(void);
int parseTerm(void);
int parseExpression2(void);
int parseExpression(void);
int parseCondition2(void);
int parseCondition(void);
int parseStatement2(void);
int parseStatement(void);
int parseStatements(void);
int parseParameter(void);
int parseParameters2(void);
int parseParameters(void);
int parseFunctionCall(void);
int parseArguments(void);
int parseFunctionDec(void);
int parseWhile(void);
int parseIf(void);

//PARSING TOKENS
int parseRPar(void);
int parseLPar(void);
int parseRCurl(void);
int parseLCurl(void);
*/

#endif /* parser_h */
