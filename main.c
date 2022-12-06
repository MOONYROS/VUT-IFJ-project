/**
 * @file main.c
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This file is the project main file.
 * @date 2022-11
 */

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
        
        fclose(inf);
    }

    return CERR_OK;
}
