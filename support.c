//
//  support.h
//  IFJ-prekladac
//
//  Created by Ondrej Lukasek on 15.10.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "support.h"
#include "token.h"

extern int srcLine;

void* safe_malloc(size_t size)
{
    char* ptr = malloc(size);
    if (ptr == NULL)
    {
        errorExit("memory allocation failed", CERR_INTERNAL);
    }
    else
    {
        for (size_t i=0; i < size; i++)
            *(ptr + i) = 0;
    }
    return ptr;
}

void safe_free(void* ptr)
{
    free(ptr);
}

void errorExit(char* msg, int errCode)
{
    switch (errCode) 
    {
    case CERR_OK:
        fprintf(stderr, "NO ERROR: ");
        break;
    case CERR_LEX:
        fprintf(stderr, "Lexical error near line %d: ", srcLine);
        break;
    case CERR_SYNTAX:
        fprintf(stderr, "Syntax error near line %d: ", srcLine);
        break;
    case CERR_SEM_FUNC:
        fprintf(stderr, "Semantic error (function) near line %d: ", srcLine);
        break;
    case CERR_SEM_ARG:
        fprintf(stderr, "Semantic error (argument) near line %d: ", srcLine);
        break;
    case CERR_SEM_UNDEF:
        fprintf(stderr, "Semantic error (undefined) near line %d: ", srcLine);
        break;
    case CERR_SEM_RET:
        fprintf(stderr, "Semantic error (return) near line %d: ", srcLine);
        break;
    case CERR_SEM_TYPE:
        fprintf(stderr, "Semantic error (type) near line %d: ", srcLine);
        break;
    case CERR_SEM_OTHER:
        fprintf(stderr, "Semantic error (other) near line %d: ", srcLine);
        break;
    case CERR_INTERNAL:
        fprintf(stderr, "Internal compiler error near line %d: ", srcLine);
        break;
    }
    fprintf(stderr, "%s\n", msg);
    exit(errCode);
}

int dbgMsg(const char* fmt, ...)
{
#if DEBUG_MSG == 1
    va_list args;
    va_start(args, fmt);
    int ret = vfprintf(stderr, fmt, args);
    va_end(args);
    return ret;
#else
    return 0;
    return (int)fmt[0]; // toto jen kvuli tomu aby prekladace nehazel warning ze je nepouzite fmt, kdyz jsou vypnute debug hlasky
#endif
}

tToken token;