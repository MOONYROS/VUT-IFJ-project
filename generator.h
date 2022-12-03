//
//  generator.h
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#ifndef generator_h
#define generator_h

#include "symtable.h"

#define MAX_IFJC_LEN	1024

typedef struct CodeLine {
	char* code;
	struct CodeLine* next;
} tCodeLine;

char* ifjCodeStr(char* outStr, char* str);
char* ifjCodeInt(char* outStr, int val);
char* ifjCodeReal(char* outStr, double val);
int addCode(const char* fmt, ...);
void addCodeVariableDefs(tSymTable* table);
void genCodeProlog(FILE* f);
void genCodeMain(FILE* f);
void generateCode(FILE* f);
void generateFuncCode(FILE* f);
void generateEmbeddedFunctions(FILE* f);

#endif /* generator_h */