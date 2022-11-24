#include <stdlib.h>
#include "symtable.h"

struct SymTable st;

void pridej(char* key)
{
    if (st_insert(&st, key) != NULL)
        printf("Key %s added.\n", key);
    else
        printf("Key %s already in Symbol Table.\n", key);
}

void main()
{
    printf("\nSYMBOL TABLE TEST:\n");
    st_init(&st);
    pridej("ahoj");
    pridej("ahojky");
    pridej("ahojda");
    pridej("mrkev");
    pridej("AHOJ");
    pridej("ahoj");
    pridej("ahoj1");
    pridej("ahoj2");
    pridej("ahoj3");
    st_print(&st);
    st_delete(&st, "ahoj1");
    st_delete(&st, "ahoj");
    st_delete(&st, "ahoj3");
    st_delete(&st, "ahojky");
    printf("deleted partially:\n");
    st_print(&st);
    st_delete_all(&st);
    printf("deleted:\n");
    st_print(&st);
}