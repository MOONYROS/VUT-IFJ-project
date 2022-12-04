//
//  lex.h
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#ifndef lex_h
#define lex_h

#include <stdio.h>

#define CHAR_RESERVE 80

int SkipProlog(FILE *f);
int ReadToken(FILE *f, tToken *token);

#endif /* lex_h */
