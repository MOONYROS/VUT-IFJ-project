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
        tParseTree tree;
        if(parse(inf, &tree) == 0)
            printf("PARSING FAILED\n");
    }
    
    
    
    fclose(inf);
    
    return 0;
}
