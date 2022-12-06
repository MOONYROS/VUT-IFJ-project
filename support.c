/**
 * @file support.c
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This file contains helpful functions that are used all around the project.
 * @date 2022-11
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>

#include "support.h"
#include "token.h"

tAllocatedMemory* memoryAllocations = NULL;
size_t totAllocs = 0;
size_t memAlloc = 0;
size_t totDeAllocs = 0;
size_t memDeAlloc = 0;

/**
 * @brief Function basically works like a regular malloc, but saves the allocation into a list, so it can be all freed later.
 * 
 * @param size allocation size
 * @return 
 */
void* safe_malloc(size_t size)
{
    char* ptr = malloc(size);
    if (ptr == NULL)
    {
        errorExit("memory allocation failed", CERR_INTERNAL);
        return NULL;
    }
    else
    {
        for (size_t i=0; i < size; i++)
            *(ptr + i) = 0;
    }
    tAllocatedMemory* maPtr = malloc(sizeof(tAllocatedMemory));
    if (maPtr == NULL)
    {
        errorExit("memory allocation failed", CERR_INTERNAL);
        return NULL;
    }
    maPtr->ptr = ptr;
    maPtr->next = memoryAllocations;
    memoryAllocations = maPtr;
    //dbgMsg("Allocated %d bytes at %p\n", size, (void*)ptr);
    totAllocs++;
    memAlloc += size;
    return ptr;
}
size_t safe_msize(void* ptr)
{
#if defined(_WIN32) || defined(WIN32)
    return _msize(ptr);
#else
    return malloc_usable_size(ptr);
#endif
}

/**
 * @brief frees allocation done with safe_malloc and deletes it from the list
 * 
 * @param ptr
 */
void safe_free(void* ptr)
{
    if (ptr == NULL)
        return;

    if (memoryAllocations == NULL)
    {
        errorExit("no allocated memory to deallocate", CERR_INTERNAL);
        return;
    }

    tAllocatedMemory* maPtr = memoryAllocations;

    // budeme mazat hned ten prvni
    if (maPtr->ptr == ptr)
    {
        memoryAllocations = maPtr->next;
        //dbgMsg("Deallocating %d bytes at %p\n", sizeof(ptr), (void*)ptr);
        totDeAllocs++;
        memDeAlloc += safe_msize(ptr);
        free(ptr);
        free(maPtr);
        return;
    }

    // tak ne, je to mozna nejaky dalsi
    tAllocatedMemory* prev = memoryAllocations;
    maPtr = prev->next;
    while (maPtr != NULL)
    {
        if (maPtr->ptr == ptr)
        {
            prev->next = maPtr->next;
            // dbgMsg("Deallocating %d bytes at %p\n", sizeof(ptr), (void*)ptr);
            totDeAllocs++;
            memDeAlloc += safe_msize(ptr);
            free(ptr);
            free(maPtr);
            return;
        }
        prev = maPtr;
        maPtr = maPtr->next;
    }
    // vubec se nenasel, tak je chyba
    errorExit("did not find memory pointer do deallocate", CERR_INTERNAL);
}

/**
 * @brief Function frees all memory allocations done with safe_malloc.
 */
void safe_free_all()
{
    size_t sum = 0;
    size_t cnt = 0;
    tAllocatedMemory* maPtr;
    while (memoryAllocations != NULL)
    {
        maPtr = memoryAllocations;
        memoryAllocations = maPtr->next;
        // dbgMsg("Deallocating %d bytes at %p\n", safe_msize(maPtr->ptr), (void*)maPtr->ptr);
        sum += safe_msize(maPtr->ptr);
        cnt++;
        // dbgMsg("Deallocating %d bytes at %p\n", safe_msize(maPtr), (void*)maPtr);
        // sum += safe_msize(maPtr);
        // cnt++;
        free(maPtr->ptr);
        free(maPtr);
    }
    dbgMsg("free_all() deallocated %d bytes of memory in %d chunks\n", sum, cnt);
}

/**
 * @brief Function sends exit code depending on it's parameters.
 * 
 * @param msg desired message
 * @param errCode error code to call
 */
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
    dbgMsg("\nTotal memory allocated % d in %d chunks.\n", memAlloc, totAllocs);
    dbgMsg("Total memory deallocated % d in %d chunks.\n", memDeAlloc, totDeAllocs);
    dbgMsg("Delete rest: ");
    safe_free_all();
    exit(errCode);
}

/**
 * @brief Function works basically as a printf() but can be disabled based on its global value.
 * 
 * @param fmt message to show/print
 * @return 
 */
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

/**
 * @brief Function works basically as a printf() but can be disabled based on its global value. Also depends on number of program pass (1 or 2).
 *
 * @param fmt message to show/print
 * @return
 */
int dbgMsg2(const char* fmt, ...)
{
    if (prgPass == 2)
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
    return 0;
}

tToken token;