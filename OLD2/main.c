//
//  main.c
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "support.h"
#include "token.h"
#include "symtable.h"
#include "lex.h"
#include "parser.h"
#include "generator.h"
#include "expression.h"

FILE* inf;

int main(int argc, const char * argv[]) {
    dbgMsg("Gigachad compiler\n");

    if (argc > 1)
        inf = fopen(argv[1], "r");
    else
        inf = stdin;
    if(inf == NULL){
        errorExit("cannot open file", CERR_INTERNAL);
    }
    else
    {
        parse();
        
        //dbgMsg("PARSING FINISHED OK\n");
        fclose(inf);
    }

    return CERR_OK;
}
