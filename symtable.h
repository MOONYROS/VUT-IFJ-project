/**
 * @file symtable.h
 * @author Ondrej Koumar, xkouma02@stud.fit.vutbr.cz
 * @brief Header file of symbol table (implemented as hash table) 
 *        for our gigachar compiler of IFJCode22 imperative language.
 * @version 0.1
 * @date 2022-11-15
 */

#ifndef symtable_h
#define symtable_h

#include "token.h"
#include <stdbool.h>

#define hashTableSize 1021;

extern int htSize;

typedef union tokenData
{
    char **literal;
    int intValue;
    float floatValue;
} tData;


typedef struct tableItem 
{
    char *key;
    bool isDefined;
    tData *tokenData;
    tTokenType tokenType;
    tItem *nextItem;
} tItem;

typedef tItem *hashTable[]; 



void htInit(hashTable *symTable);

unsigned int getHash(char *key);

tItem* searchItem(hashTable *symTable, char *key);

tData* getValue(hashTable *symTable, char *key);

void insertItem(hashTable *symTable, tData data, char *key);

void deleteItem(hashTable *symTable, char *key);

void htDispose(hashTable *symTable);

#endif // symtable_h