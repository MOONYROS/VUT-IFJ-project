//
//  parser.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 02.11.2022.
//
#include <string.h>
#include <stdlib.h>

#include "support.h"
#include "token.h"
#include "lex.h"
#include "parser.h"

char *rule[SYNTAXRULES][RULEITEMS] = {
    {"if_statement",            "tIf", "tLPar", "condition", "tRPar", "tLCurl", "statements", "tRCurl", "tElse", "tLCurl", "statements", "tRCurl", ""},
    {"while_statement",         "tWhile", "tLPar", "condition", "tRPar", "tLCurl", "statements", "tRCurl", ""},
    {"functionDeclaration",     "tFunction", "tFuncName", "tLPar", "arguments", "tRPar", "tColon", "type", "tRCurl", "statements", "tLCurl", ""},
    {"arguments",               "variable", "variables", ""},
    {"arguments",               "EPS", ""},
    {"functionCall",            "tFuncName", "tLPar", "parameters", "tRPar", ""},
    {"parameters",              "parameter", "parameters2", ""},
    {"parameters",              "EPS", ""},
    {"parameters2",             "tComma", "parameter", "parameters2", ""},
    {"parameters2",             "EPS", ""},
    {"parameter",               "factor", ""},
    {"statements",              "statement", "statements", ""},
    {"statementsX",             "EPS", ""},
    {"statement",               "if_statement", ""},
    {"statement",               "while_statement", ""},
    {"statement",               "functionCall", "tSemicolon", ""},
    {"statement",               "tReturn", "expression", "tSemicolon", ""},
    {"statement",               "expression", "statement2", "tSemicolon", ""},
    {"statement2",              "tAssign", "statement3", ""},
    {"statement2",              "EPS", ""},
    {"statement3",              "expression", ""},
    {"statement3",              "functionCall", ""},
    {"condition",               "functionCall", "condition2", ""},
    {"condition",               "expression", "condition2", ""},
    {"condition2",              "relational_operators", "expression", ""},
    {"condition2",              "EPS", ""},
    {"expression",              "term", "expression2", ""},
    {"expression",              "EPS", ""},
    {"expression2",             "tPlus", "expression", ""},
    {"expression2",             "tMinus", "expression", ""},
    {"expression2",             "EPS", ""},
    {"term",                    "const", "term2", ""},
    {"term",                    "variable", "term2", ""},
    {"term2",                   "tMul", "term", ""},      // misto factor term
    {"term2",                   "tConcat", "term", ""},   // misto factor term
    {"term2",                   "tDiv", "term", ""},      // misto factor term
    {"term2",                   "EPS", ""},
    {"factor",                  "variable", ""},
    {"factor",                  "const", ""},
    {"variables",               "tComma", "variable", "variables", ""},
    {"variables",               "EPS", ""},
    {"variable",                "type", "tIdentifier", ""},
    {"variable",                "tIdentifier", ""},
    {"const",                   "tInt", ""},
    {"const",                   "tReal", ""},
    {"const",                   "tReal2", ""},
    {"const",                   "tInt2", ""},
    {"const",                   "tNull", ""},
    {"const",                   "tLiteral", ""},
    {"type",                    "tNullTypeInt", ""},
    {"type",                    "tNullTypeFloat", ""},
    {"type",                    "tNullTypeString", ""},
    {"type",                    "tTypeInt", ""},
    {"type",                    "tTypeFloat", ""},
    {"type",                    "tTypeString", ""},
    {"type",                    "tVoid", ""},
    {"relational_operators",    "tLess", ""},
    {"relational_operators",    "tLessEq", ""},
    {"relational_operators",    "tMore", ""},
    {"relational_operators",    "tMoreEq", ""},
    {"relational_operators",    "tIdentical", ""},
    {"relational_operators",    "tNotIdentical", ""},
};

void printParseTree(tParseTree* tree, int level)
{
    while (tree != NULL)
    {
        if (tree->is_nonterminal)
        {
            //printf("%*s[%s]\n", level * 2, "", tree->nonterm.name);
            if (tree->nonterm.tree != NULL)
                printParseTree(tree->nonterm.tree, level + 1);
        }
        else
        {
            printf("%*s%s\n", level * 2, "", tree->term.data);// , tokenName[tree->term.tType]);
        }
        tree = tree->next;
    }
}

int ReadTokenPRINT(FILE *f, tToken *token)
{
    int ret;
    ret = ReadToken(f, token);
    if(token->type == tInvalid)
        printf("INVALID: <<%s>>\n",token->data);
    else
        printf("TOKEN %s: <<%s>>\n", tokenName[token->type], token->data);
    return ret;
}

tTokenType strToToken(const char *tokenStr)
{
    for (int t = tTypeInt; t <= tEpilog; t++) {
        if (strcmp(tokenStr, tokenName[t]) == 0)
            return t;
    }
    return tInvalid;
}

tParseTree* newNonterminal(char* name, tParseTree* pPT)
{
    tParseTree* p = (tParseTree*)safe_malloc(sizeof(tParseTree));
    p->next = NULL; // neni potreba ???
    p->is_nonterminal = 1;
    p->nonterm.tree = pPT;
    p->nonterm.name = safe_malloc(strlen(name)+1);
    strcpy(p->nonterm.name, name);
    return p;
}

tParseTree* newTerminal(tToken *token, tParseTree* next)
{
    tParseTree* p = (tParseTree*)safe_malloc(sizeof(tParseTree));
    p->next = next;
    p->is_nonterminal = 0;
    p->term.tType = token->type;
    p->term.data = safe_malloc(strlen(token->data) + 1);
    strcpy(p->term.data, token->data);
    return p;
}


int parser(FILE *f, tToken *token, tParseTree **tree, const char *state, int iter, int line)
{
    printf("%*s%s (%d): %s - %s\n", iter * 2, "", state, line, tokenName[token->type], token->data);

    //if(feof(f))
       // return 1;
	*tree = NULL;
    for (int i = 0; i < SYNTAXRULES; i++) {
        if (strcmp(state, rule[i][0]) == 0)
        {
			tParseTree *lst = NULL;
            int j = 1;
            while (j != RULEITEMS)
            {
				int koncime = 0;
                if (strcmp(rule[i][j], "") == 0)
                {
                    //printf("%*sKONEC radku %d\n", iter*2, "", line);
					koncime = 1;
                } else if (strcmp(rule[i][j], "EPS") == 0)
                {
                    printf("%*sEpsilon na (%d) %s\n", iter*2, "", i+1, state);
					koncime = 1;
				}
				if (koncime) {
                    *tree = newNonterminal(rule[i][0], lst);
					return 1;
				}
                if ((strlen(rule[i][j] ) > 1) && (rule[i][j][0] == 't') && (rule[i][j][1] >= 'A') && (rule[i][j][1] <= 'Z'))
                {
                    if (token->type == strToToken(rule[i][j]))
                    {
                        printf("%*sMATCH (%d) %s -> %s\n", iter*2, "", i+1, state, rule[i][1]);

						lst = newTerminal(token, lst);

                        if (ReadTokenPRINT(f, token) == 0)
                        {
                            if (token->type == tEpilog) {
                                *tree = newNonterminal("EPILOG", lst);
                                return 1;
                            }
							
                            return 0;
                        }
                        else
                        {
                            if(token->type == tInvalid)
                            {
                                printf("INVALID TOKEN\n");
								
                                return 0;
                            }
                        }
                    }
                    else
                    {
                        if(j>1)
                        {
                            printf("neocekavany token\n");
							
							return 0;
                        }
                        else
                            break;
                    }
                }
                else
                {
					tParseTree *parsed_nonterm;
					// rule[i][0] rule[i][j]
                    if(parser(f, token, &parsed_nonterm, rule[i][j], iter + 1, i+1))
                    {
                        // parsing OK
                        parsed_nonterm->next = lst;
                        lst = parsed_nonterm;
                        if (token->type == tEpilog) {
                            *tree = newNonterminal("EPILOG2", lst);
                            return 1;
                        }
                    }
                    else
                    {
                        // parsing failed
                        if (j > 0)
                            break;
                        else
                        { // tohle asi nemuze nastat
							if (j > 1) {
								*tree = NULL;
								return 0;
							}
                        }
                        // return 0;
                    }
                }
                j++;
            }
            // printf("Sem bychom nemeli, jsme za koncem radku\n");
        }
    }

    //printf("%*s--KONEC prohledavani - navrat z parse\n", iter*2, "");
    return 0;
}

int parse(FILE *f, tParseTree **tree)
{
    if (SkipProlog(f))
        printf("PROLOG OK\n");
    else
    {
        printf("INVALID PROLOG\n");
        return 0;
    }
    tToken token;
    ReadTokenPRINT(f, &token);
    if(token.type == tInvalid)
    {
        printf("INVALID TOKEN\n");
        return 0;
    }
    return parser(f, &token, tree, "statements", 0, 0);
}

/*
int parseRPar(void)
{
    if(token->type == tRPar)
        return 0;
    else
        return 1; // TODO SYNTAX ERROR
}

int parseLPar(void)
{
    if(token->type == tLPar)
        return 0;
    else
        return 1; // TODO SYNTAX ERROR
}

int parseRCurl(void)
{
    if(token->type == tRCurl)
        return 0;
    else
        return 1; // TODO SYNTAX ERROR
}

int parseLCurl(void)
{
    if(token->type == tLCurl)
        return 0;
    else
        return 1; // TODO SYNTAX ERROR
}

void parser(void)
{

}
*/
