//
//  main.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "token.h"
#include "lex.h"
#include "parser.h"

tParseTree* otocParseTree(tParseTree* tree)
{
	tParseTree* tree2 = NULL;
	while (tree != NULL)
	{
		if (tree->is_nonterminal)
		{
			tree->nonterm.tree = otocParseTree(tree->nonterm.tree);
		}
		tParseTree* tmpptr = tree->next;
		tree->next = tree2;
		tree2 = tree;

		tree = tmpptr;
	}
	return tree2;
}


int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Gigachad compiler\n");

    FILE *inf;

    inf = fopen("vzor_01.php", "r");
    if(inf == NULL){
        printf("nemuzu otevrit soubor\n");
        printf("%s\n", strerror(errno));
    }
    else
    {
        tParseTree *tree;
        if(parse(inf, &tree) == 0)
            printf("PARSING FAILED\n");
        else
            printf("PARSING FINISHED OK\n");
        fclose(inf);
        printf("Parse tree:\n");
		tree = otocParseTree(tree);
        printParseTree(tree, 0);
    }

    return 0;
}
