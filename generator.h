/**
 * @file generator.h
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This header file is responsible for generating the code.
 * @date 2022-11
 */

#ifndef generator_h
#define generator_h

#include "symtable.h"
#include "expStack.h"

#define MAX_IFJC_LEN	1024

typedef struct CodeLine {
	char* code;
	struct CodeLine* next;
} tCodeLine;

char* ifjCodeStr(char* outStr, char* str);
char* ifjCodeInt(char* outStr, int val);
char* ifjCodeReal(char* outStr, double val);
char* ifjCodeNil(char* outStr);
int addCode(const char* fmt, ...);
void addCodeVariableDefs(tSymTable* table);
void genCodeProlog(FILE* f);
void genCodeMain(FILE* f);
void generateCode(FILE* f);
void generateFuncCode(FILE* f);
void generateEmbeddedFunctions(FILE* f);

extern const char funcPrefixName[];

#endif /* generator_h */