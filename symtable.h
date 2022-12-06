/**
 * @file symtable.h
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This header file contains all the functions for symbol table.
 * @date 2022-11
 */

#ifndef symtable_h
#define symtable_h

#include <stdbool.h>

#define ST_SIZE 509


/* typedef enum {
    DTUnknown, DTInt, DTFloat, DTString, DTNullInt, DTNullFloat, DTNullString, DTVoid
} tDataType; */


typedef struct FuncParam {
    char* name; 
    tTokenType dataType;
    struct FuncParam* next;
} tFuncParam;

typedef struct SymTable tSymTable;

typedef struct SymTableItem {
    char name[MAX_TOKEN_LEN];             // name of variable or function
    tTokenType dataType;        // data type variable or function
    int isFunction;             // 1 (true) if name is function
    int isDefined;              // 1 if function is defined, 0 if only delcared by function call
    int hasReturn;              // function only: info if the function has correctly returned
    struct SymTable* localST;   // function only: pointer to local symbol table for funcion
    tFuncParam* params;         // function only : list of function paramaters
    struct SymTableItem* next;  // hash table items with the same hash
} tSymTableItem;

struct SymTable{
    tSymTableItem* items[ST_SIZE];
};

int get_hash(char *key);
void st_init(tSymTable *table);
tSymTableItem* st_search(tSymTable *table, char *key);
tSymTableItem* st_insert(tSymTable *table, char *key);
tSymTableItem* st_insert_function(tSymTable* table, char* key, tTokenType type);
tSymTableItem* st_add_params(tSymTable* table, char* key, tTokenType type, char* name);
tSymTableItem* st_searchinsert(tSymTable* table, char* key);
int st_nr_func_params(tSymTable* table, char* key);
tTokenType st_get_type(tSymTable* table, char* key);
void st_delete(tSymTable*table, char *key);
void st_delete_all(tSymTable* table);
void st_delete_scope(tSymTable* table, int scope);
void st_print(tSymTable* table);

#endif /* symtable_h */
