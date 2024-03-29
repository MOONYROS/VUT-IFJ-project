/**
 * @file symtable.c
 * Implementace prekladace imperativniho jazyka IFJ22
 * 
 * @author Ondrej Lukasek (xlukas15)
 * @author Ondrej Koumar (xkouma02)
 * @author Jonas Morkus (xmorku03)
 * @author Milan Menc (xmencm00)
 * 
 * @brief This file contains all the functions for symbol table.
 * @date 2022-11
 */

#include <stdlib.h>
#include <string.h>

#include "support.h"
#include "token.h"
#include "symtable.h"

extern char* tokenName[tMaxToken];

static const unsigned int crc32_table[] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

/**
 * @brief CRC32 function for hash calculation.
 * 
 * @param buf input data
 * @param len data length
 * @return unsigned int 
 */
unsigned int crc32(const unsigned char* buf, size_t len)
{
    unsigned int crc = 0xFFFFFFFF;
    while (len--)
    {
        crc = (crc >> 8) ^ crc32_table[(crc ^ *buf) & 0xFF];
        buf++;
    }
    return ~crc;
}

/**
 * @brief Function that gets the hash table index.
 * 
 * @param key hash key
 * @return int hash table index
 */
int get_hash(char *key) 
{
    return (crc32((const unsigned char *)key, strlen(key)) % ST_SIZE);
}

/**
 * @brief Function for initialization of symbol table.
 * 
 * @param table hash table
 */
void st_init(tSymTable *table) 
{
    for(int i = 0; i < ST_SIZE; i++)
        table->items[i] = NULL;
}

/**
 * @brief Function for searching in symbol table.
 * 
 * @param table symbol table
 * @param key key for finding correct item
 * @return tSymTableItem* item in symbol table
 */
tSymTableItem *st_search(tSymTable *table, char *key)
{
    if (table == NULL)
        return NULL;

    int index = get_hash(key);
    tSymTableItem *tmpItem = table->items[index];

    while(tmpItem != NULL)
    {
        if(strcmp(tmpItem->name, key) == 0)
            return tmpItem;
        
        tmpItem = tmpItem->next;
    }
    return NULL;
}

/**
 * @brief Function for inserting an item into symbol table.
 * 
 * @param table symbol table
 * @param key key for item
 * @return tSymTableItem* item in symbol table
 */
tSymTableItem* st_insert(tSymTable* table, char* key)
{
    if (table == NULL)
        return NULL;

    tSymTableItem *foundItem = st_search(table, key);
    if (foundItem != NULL)
        return NULL; // cannot insert, symbol alreard in symbol table
    else
    {
        int index = get_hash(key);
        tSymTableItem *tmpItem = table->items[index];
        tSymTableItem *newItem = safe_malloc(sizeof(tSymTableItem));
        if(newItem == NULL)
            return NULL;
        strcpy(newItem->name, key);
        newItem->dataType = tNone;
        newItem->isFunction = 0;
        newItem->isDefined = 0;
        newItem->hasReturn = 0;
        newItem->localST = NULL;
        newItem->params = NULL;
        if (tmpItem == NULL)
        {
            newItem->next = NULL;
            table->items[index] = newItem;
        }
        else
        {
            newItem->next = tmpItem;
            table->items[index] = newItem;
        }
        return newItem;
    }
}

/**
 * @brief Function for inserting function to symbol table.
 * 
 * @param table symbol table
 * @param key key for item
 * @param type token type
 * @return tSymTableItem* stored function
 */
tSymTableItem* st_insert_function(tSymTable* table, char* key, tTokenType type)
{
    if (table == NULL)
        return NULL;

    tSymTableItem* sti = st_insert(table, key);
    if (sti == NULL)
        return NULL;
    else
    {
        sti->dataType = type;
        sti->isFunction = 1;
    }
    sti->localST = safe_malloc(sizeof(tSymTable));
    st_init(sti->localST);
    return sti;
}

/**
 * @brief Function for adding parameters to symbol table.
 * 
 * @param table symbol table
 * @param key key for item
 * @param type token type
 * @param name name of the parameter
 * @return tSymTableItem* 
 */
tSymTableItem* st_add_params(tSymTable* table, char* key, tTokenType type, char* name)
{
    if (table == NULL)
        return NULL;

    tSymTableItem* fce = st_search(table, key);
    if (fce == NULL)
        return NULL;

    tFuncParam* funcPar = safe_malloc(sizeof(tFuncParam));
    if (funcPar == NULL)
        return NULL; // jen kvuli IntelliSense, safe_malloc nikdy NULL nevrati
    funcPar->dataType = type;
    funcPar->name = safe_malloc(strlen(name) + 1);
    strcpy(funcPar->name, name);
    funcPar->next = NULL;

    if (fce->params == NULL)
    {
        fce->params = funcPar;
    }
    else
    {
        tFuncParam* last = fce->params;
        while (last->next != NULL)
            last = last->next;
        last->next = funcPar;
    }

    // add parameter as local variable to function's symbol table
    tSymTableItem* sti = st_searchinsert(fce->localST, funcPar->name);
    if (sti == NULL)
    {
        errorExit("cannot insert or find parameter to function local symbol table", CERR_INTERNAL);
        return NULL; // prevent C6011
    }
    sti->dataType = funcPar->dataType;

    return fce;
}

/**
 * @brief 
 * 
 * @param table symbol table
 * @param key key for item
 * @return tSymTableItem* inserted item
 */
tSymTableItem* st_searchinsert(tSymTable* table, char* key)
{
    if (table == NULL)
        return NULL;

    tSymTableItem* foundItem = st_search(table, key);
    if (foundItem != NULL)
        return foundItem;
    return st_insert(table, key);

}

/**
 * @brief Function that counts number of parameters of a function.
 * 
 * @param table symbol table
 * @param key key for item
 * @return int number of paramaters
 */
int st_nr_func_params(tSymTable* table, char* key)
{
    tSymTableItem* sti = st_search(table, key);
    int cnt = 0;
    if (sti != NULL)
    {
        tFuncParam* param = sti->params;
        while (param != NULL)
        {
            cnt++;
            param = param->next;
        }
    }
    return cnt;
}

/**
 * @brief Function that gets the type of a token
 * 
 * @param table symbol table
 * @param key key for item
 * @return tTokenType token type
 */
tTokenType st_get_type(tSymTable* table, char* key)
{
    tSymTableItem* sti = st_search(table, key);
    if (sti != NULL)
        return sti->dataType;
    else
        return tNone;
}

/**
 * @brief Function for deleting parameters.
 * 
 * @param paramToDelete param that will be deleted
 */
void st_delete_params(tFuncParam **paramToDelete)
{
    tFuncParam* param = *paramToDelete;
    while (param != NULL)
    {
        tFuncParam* tmpParam = param->next;
        if(param->name!=NULL)
            safe_free(param->name);
        safe_free(param);
        param = tmpParam;
    }
    *paramToDelete = NULL;
}

/**
 * @brief Function for deletion from symbol table.
 * 
 * @param table symbol table
 * @param key key for item
 */
void st_delete(tSymTable *table, char *key) {
    if(table != NULL)
    {
        int index = get_hash(key);
        tSymTableItem *item = table->items[index];
        tSymTableItem *prevItem = NULL; // pomocny prvek, pokud se pri kolizi stane, ze nebude hledany prvek prvni v seznamu
        while(item != NULL)
        {
            if(strcmp(item->name, key) == 0)
            {
                tSymTableItem *toDelete = item;
                if(table->items[index] == item) //prvek je na prvnim miste
                    table->items[index] = item->next;
                else // prvek je na jinem nez prvnim miste (potreba pouzit prevItem)
                    prevItem->next = toDelete->next;

                st_delete_params(&(toDelete->params)); // smazat parametry
                st_delete_all(toDelete->localST); // smazat lokalni tabulku symbolu, pokud existuje
                safe_free(toDelete);

                return;
            }
            prevItem = item;
            item = item->next;
        }
    }
}

/**
 * @brief Function for deletion of all the items in symbol table.
 * 
 * @param table symbol table
 */
void st_delete_all(tSymTable *table) {
    tSymTableItem* toDelete;
    if (table == NULL)
        return;
    for (int i = 0; i < ST_SIZE; i++)
    {
        if (table->items[i] != NULL)
        {
            tSymTableItem *tmpItem = table->items[i]->next;
            while (tmpItem != NULL)
            {
                toDelete = tmpItem;
                tmpItem = tmpItem->next;
                st_delete_params(&(toDelete->params));
                st_delete_all(toDelete->localST); // smazat lokalni tabulku symbolu, pokud existuje
                safe_free(toDelete);
            }
            toDelete = table->items[i];
            st_delete_params(&(toDelete->params));
            st_delete_all(toDelete->localST); // smazat lokalni tabulku symbolu, pokud existuje
            safe_free(toDelete->localST);
            safe_free(table->items[i]);
            table->items[i] = NULL;
        }
        
    }
}

/*
void st_delete_scope(tSymTable* table, int scope)
{
    if (table == NULL)
        return;

    tSymTableItem* item;
    for (int i = 0; i < ST_SIZE; i++)
    {
        item = table->items[i];
        if (item != NULL)
        {
            tSymTableItem* ptr = item->next;
            while (ptr != NULL)
            {
                tSymTableItem* p = ptr;
                ptr = ptr->next;
                if (p->scope == scope)
                    st_delete(table, p->name);
            }
            if (item->scope == scope)
                st_delete(table, item->name);
        }
    }
} */

/**
 * @brief Function for printing out the symbol table.
 * 
 * @param table symbol table
 */
void st_print(tSymTable* table)
{
    if (table == NULL)
        return;

    tSymTableItem* item;
    for (int i = 0; i < ST_SIZE; i++)
    {
        item = table->items[i];
        if (item != NULL)
        {
            dbgMsg("[%3d] ", i);
            if(item->isFunction)
            {
                dbgMsg("%s (", item->name);
                tFuncParam* fp = item->params;
                while (fp != NULL)
                {
                    dbgMsg(" %s", tokenName[fp->dataType]);
                    dbgMsg(" %s", fp->name);
                    fp = fp->next;
                }
                dbgMsg(" ) : %s", tokenName[item->dataType]);
                if (item->localST != NULL)
                {
                    dbgMsg(" --> localST:");
                    tSymTableItem* litem;
                    for (int j = 0; j < ST_SIZE; j++)
                    {
                        litem = item->localST->items[j];
                        if (litem != NULL)
                        {
                            dbgMsg(" %s", litem->name);
                        }
                    }
                }
            }
            else
            {
                dbgMsg("$%s : %s", item->name, tokenName[item->dataType]);
            }
            while (item->next != NULL)
            {
                dbgMsg(", %s", item->next->name);
                item = item->next;
            }
            dbgMsg("\n");
        }
    }
}
