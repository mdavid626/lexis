
/*****************************************************
** Soubor:    table.c
** Encoding:  ISO-8859-2
** Datum:     2010/14/11
** Autor:     Miroslav Nemec, xnemec53
** Projekt:   Implementace interpretu imperativního jazyka IFJ10.
** Nazev:     Tanulka symbolu jazyka IFJ10.
** Popis:     Implementace tabulky symbolu pomoci BVS.
*****************************************************
**/

#include "table.h"
#include "assert.h"

//extern int offset;

//#define NDEBUG

/* ************************* Seznam tabulky symbolù ************************* */
/*                                                                            */

void InitList (tList *L)
{
    // inicializácia seznamu L
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

int InsertFirst (tList *L, tSymbolTable uk)
{
    // alokovani pameti pro novy prvek seznamu L
    tElemPtr pomocna = (tElemPtr) malloc (sizeof (struct tElem));    
    if (pomocna == NULL)                // kontrola uspesnosti alokovani
       return MALLOC_ERROR;             // chybovy kod
    else
    {
        pomocna->ptr = NULL;
        pomocna->table = uk;
        if (L->First == NULL)
            L->Last = pomocna;
            
        // novy zacatecni prvek bude odkazovat na puvodni prvni prvek seznamu L
        pomocna->ptr = L->First;                
        L->First = pomocna;             // vlozeni prvku na zacatek seznamu L
    }
    
    return TABLE_OK;
}

void First(tList *L)
{
    L->Act = L->First;      // nastavi aktivitu seznamu L na jeho prvni prvek
}

void Last(tList *L)
{
    L->Act = L->Last;       // nastavi aktivitu seznamu L na jeho posledni prvek
}

void Dispose_List_Table (tList *L)	       // smaze seznam i tabulku symbolu
{
    tElemPtr pomocna;
    while((L->First) != NULL)       // cyklus který smaze cely obsah seznamu L
    {
        TableDispose(&L->First->table);    // smaze tabulku symbolu
        pomocna = L->First;
        L->First = pomocna->ptr;   // do prveho prvku seznamu L vlozim druhy prvek seznamu L
        free(pomocna);             // smazu puvodni prvni prvek seznamu L
    }
    
    // inicializacia seznamu L
    L->Act = NULL;
    L->First = NULL;
}

/* konec table.c */
