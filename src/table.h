
/*****************************************************
** Soubor:    table.h
** Encoding:  ISO-8859-2
** Datum:     2010/14/11
** Autor:     Miroslav Nemec, xnemec53
** Projekt:   Implementace interpretu imperativního jazyka IFJ10.
** Nazev:     Tanulka symbolu jazyka IFJ10.
** Popis:     Hlavicka Tabulky symbolu.
*****************************************************
**/

#include <stdio.h>
#include <stdlib.h>
#include "str.h"
#include "i_list.h"

#ifndef TABLE_H
#define TABLE_H

//////////////////////////// pro tabulku symbolu

#define UNINITIALIZED 0
#define TRUE 1
#define FALSE 0

#define TABLE_OK 0
#define MALLOC_ERROR -5

enum data_types
{
    T_INT = 0,
    T_DOUBLE,
    T_STRING,
    T_FUNCTION,
    T_LABEL,
	T_UNKNOWN
};

typedef struct item* tItem;

/* tSymbolTable je alias na tItem, pretoze tItem je tabulka sama sebou */
typedef tItem tSymbolTable;

/* je potreba deklarovat driv, pro ujasneni viz. struct tElem pri deklaraci seznamu tabulek */
typedef struct tElem *tElemPtr;

struct t_function
{
    string params;
    tElemPtr localTable;
    tListItem* label;
    int defined;
    int declared;
};

typedef struct t_data tData;
struct t_variable
{
    int offset;
    tData* next_input;
};

union t_value
{
    struct t_variable variable;
    struct t_function function;
    tListItem* label;
};

struct t_data
{
    enum data_types type;
    union t_value value;
};        //Nikdy nebude alokovana mallocom

struct item
{
    string key;
    tData data;
    struct item* left;
    struct item* right;
};   //Vzdy sa alokuje mallocom









/**
 * @brief Inicializacia stromu
 * @param root Koren stromu. Bude nastavena na NULL
 */
void TableInit(tSymbolTable* root);

/**
 * @brief Hlada kluc v tabulke
 * @param root Koren tabulky
 * @param key Haldany kluc
 * @param found Priznak uspesnosti hladani
 * @return Vrati ukazatel na najdeny zaznam
 */
tData* TableSearch(tSymbolTable root, string key, int* found );

/**
 * @brief Vklada do tabulky zaznam tData
 * @param root Ukazatel na koren tabulky
 * @param key Kluc na ktory sa vklada
 * @param type typ zaznamu
 * @return Vrati ukayatel na najdeny zaznam
 */
tData* TableInsert (tSymbolTable* root, string key, enum data_types type);


/**
 * @brief Uvolni celu tabulku
 * @param root Ukazatel na koren tabulky
 */
void TableDispose(tSymbolTable* root);


////////////////////////////// pro seznam
//
struct tElem {
    struct tElem *ptr;
    tSymbolTable table;
};

typedef struct {
    tElemPtr Act;
    tElemPtr First;
    tElemPtr Last;
} tList;


void InitList (tList *L);
int InsertFirst (tList *L, tSymbolTable uk);
void First (tList *L);
void Last (tList *L);
void Dispose_List_Table (tList *L);

#endif

/* konec table.h */
