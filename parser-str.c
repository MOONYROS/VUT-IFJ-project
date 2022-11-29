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

extern char* tokenName[tMaxToken];

char* rule[SYNTAXRULES][RULEITEMS] = {
    {"programs",                "program", "programs", ""},
    {"programs",                "EPS", ""},
    {"program",                 "tFunction", "tFuncName", "tLPar", "arguments", "tRPar", "tColon", "type", "tLCurl", "statements", "tRCurl", ""},
    {"program",                 "statement", ""},
    {"arguments",               "argumentVar", "argumentVars", ""},
    {"arguments",               "EPS", ""},
    {"parameters",              "term", "parameters2", ""},
    {"parameters",              "EPS", ""},
    {"parameters2",             "tComma", "term", "parameters2", ""},
    {"parameters2",             "EPS", ""},
    {"statements",              "statement", "statements", ""},
    {"statements",              "EPS", ""},
    {"statement",               "tIf", "tLPar", "expression", "tRPar", "tLCurl", "statements", "tRCurl", "tElse", "tLCurl", "statements", "tRCurl", ""},
    {"statement",               "tWhile", "tLPar", "expression", "tRPar", "tLCurl", "statements", "tRCurl", ""},
    {"statement",               "tFuncName", "tLPar", "parameters", "tRPar", "tSemicolon", ""},
    {"statement",               "tReturn", "expression", "tSemicolon", ""},
    {"statement",               "preExpression1", ""},
    {"statement",               "preExpression2", ""},
    {"preExpression1",          "variable", "nextTerminal", ""},
    {"nextTerminal",            "tAssign", "expression", "tSemicolon", ""},
    {"nextTerminal",            "expression2", "tSemicolon", ""},
    {"preExpression2",          "const", "expression2", "tSemicolon", ""},
    {"preExpression2",          "tLPar", "const", "expression2", "tRPar", "tSemicolon", ""},
    {"expression",              "tLPar", "expression", "tRPar", "expression2", ""},
    {"expression",              "term", "expression2", ""},
    {"expression",              "EPS", ""},
    {"expression2",             "tPlus", "expression", ""},
    {"expression2",             "tMinus", "expression", ""},
    {"expression2",             "tMul", "expression", ""},
    {"expression2",             "tDiv", "expression", ""},
    {"expression2",             "tConcat", "expression", ""},
    {"expression2",             "tLess", "expression", ""},
    {"expression2",             "tLessEq", "expression", ""},
    {"expression2",             "tMore", "expression", ""},
    {"expression2",             "tMoreEq", "expression", ""},
    {"expression2",             "tIdentical", "expression", ""},
    {"expression2",             "tNotIdentical", "expression", ""},
    {"expression2",             "EPS", ""},
    {"term",                    "const", ""},
    {"term",                    "variable", ""},
    {"argumentVars",            "tComma", "argumentVar", "argumentVars", ""},   // pro argument deklarace funkce
    {"argumentVars",            "EPS", ""},                                     // pro argument deklarace funkce
    {"argumentVar",             "type", "tIdentifier", ""},                     // pro argument deklarace funkce
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
};

char* strNULL(tParseTree* item)
{
    if (item == NULL)
        return "NULL";
    else
    {
        if (item->is_nonterminal)
            return item->nonterm.name;
        else
            return item->term.data;
    }
}

void printParseTree(tParseTree* tree, int level)
{
    while (tree != NULL)
    {
        if (tree->is_nonterminal)
        {
            printf("TREE: %*s[%s] dolu %s, doprava %s\n", level * 2, "", tree->nonterm.name, strNULL(tree->nonterm.tree), strNULL(tree->next));
            if (tree->nonterm.tree != NULL)
                printParseTree(tree->nonterm.tree, level + 1);
        }
        else
        {
            printf("TREE: %*s%s, doprava %s\n", level * 2, "", tree->term.data, strNULL(tree->next));// , tokenName[tree->term.tType]);
        }
        tree = tree->next;
    }
}

int ReadTokenPRINT(FILE *f, tToken *token)
{
    int ret;
    ret = ReadToken(f, token);
#if DEBUG_MSG == 11
    if(token->type == tInvalid)
        printf("INVALID: <<%s>>\n",token->data);
    else
        printf("TOKEN %s: <<%s>>\n", tokenName[token->type], token->data);
#endif
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

void disposeTree(tParseTree** delTree)
{
    if (delTree == NULL)
        return;

    tParseTree* tree = *delTree;
    while (tree != NULL)
    {
        tParseTree* tmpTree = tree->next;
        if (tree->is_nonterminal)
        {
            // dbgMsg("MAZU: %*s[%s]\n", level * 2, "", tree->nonterm.name);
            if (tree->nonterm.tree != NULL)
                disposeTree(&(tree->nonterm.tree));
            free(tree->nonterm.name);
            free(tree);
            tree = NULL;
        }
        else
        {
            // dbgMsg("MAZU: %*s%s\n", level * 2, "", tree->term.data);// , tokenName[tree->term.tType]);
            free(tree->term.data);
            free(tree);
            tree = NULL;
        }
        tree = tmpTree;
    }
}

// return values:
// -1 = error
// 0 = not found
// 1 = found
int parser(FILE *f, tToken *token, tParseTree **tree, const char *state, int iter, int line)
{
    dbgMsg("%*s%s (%d): %s - %s\n", iter * 2, "", state, line, tokenName[token->type], token->data);

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
                    //dbgMsg("%*sKONEC radku %d\n", iter*2, "", line);
                    koncime = 1;
                } else if (strcmp(rule[i][j], "EPS") == 0)
                {
                    dbgMsg("%*sEpsilon na (%d) %s\n", iter*2, "", i+1, state);
                    koncime = 1;
                }
                /*if (((strcmp(state, "statement") == 0 && strcmp(rule[i][j], "tSemicolon")) || strcmp(state, "statements") == 0) && token->type == tRCurl)
                {
                    dbgMsg("%*sVOJEB ; } na (%d) %s\n", iter * 2, "", i + 1, state);
                    koncime = 1;
                }*/
                if (koncime) {
                    *tree = newNonterminal(rule[i][0], lst);
                    return 1;
                }
                if ((strlen(rule[i][j] ) > 1) && (rule[i][j][0] == 't') && (rule[i][j][1] >= 'A') && (rule[i][j][1] <= 'Z'))
                {
                    if (token->type == strToToken(rule[i][j]))
                    {
                        dbgMsg("%*sMATCH (%d) %s -> %s\n", iter*2, "", i+1, state, rule[i][1]);

                        lst = newTerminal(token, lst);

                        int rtres = ReadTokenPRINT(f, token);
                        if (rtres == 0)
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
                                dbgMsg("INVALID TOKEN\n");
                                return -1;
                            }
                        }
                    }
                    else
                    {
                        if(j>1)
                        {
                            dbgMsg("neocekavany token [(%d) %s]: %s misto %s (pos=%d)\n", i, rule[i][0], tokenName[token->type], rule[i][j], j);
                            return -1;
                        }
                        else
                            break;
                    }
                }
                else
                {
                    tParseTree *parsed_nonterm;
                    // rule[i][0] rule[i][j]
                    int res = parser(f, token, &parsed_nonterm, rule[i][j], iter + 1, i + 1);
                    if(res==1)
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
                        if (res == 0)
                        {
                            // dbgMsg("J = %d\n", j);
                            // parsing failed
                            if (j > 1) {
                                //m *tree = NULL;
                                return -1;
                            }
                            else
                                break; // jsme na zacatku, tak zkusime dalsi radek
                        }
                        else
                            return  -1;
                    }
                }
                j++;
            }
            // dbgMsg("Sem bychom nemeli, jsme za koncem radku\n");
        }
    }

    //dbgMsg("%*s--KONEC prohledavani - navrat z parse\n", iter*2, "");
    return 0;
}

int parse(FILE *f, tParseTree **tree)
{
    if (SkipProlog(f))
        dbgMsg("PROLOG OK\n");
    else
        errorExit("Invalid PROLOG", CERR_SYNTAX);
    tToken token = {0, safe_malloc(MAX_TOKEN_LEN)};
    ReadTokenPRINT(f, &token);
    if (token.type == tInvalid)
        errorExit("invalid first token", CERR_LEX);
    int ret = parser(f, &token, tree, "statements", 0, 0);
    free(token.data);
    return ret;
}
