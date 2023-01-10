
/*****************************************************
** Soubor:    i_list.c
** Encoding:  ISO-8859-2
** Datum:     2010/30/11
** Autor:     Lukáš Langer, xlange02
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Implementace seznamu instrukcniho listu a pomocneho zasobniku.
**            Pomocny zasobnik je vyuzit u volani funkcii na zapamatovani
**            navratove adresy.
*****************************************************
**/

#include <malloc.h>
#include "i_list.h"

#define MALLOC_ERROR 5

#define LIST_OK 0


/** LIST **/

// inicializace listu instrukcii
void ListInit(tInstructionList *List)
{
    List->First = NULL;
    List->Last = NULL;
    List->Active = NULL;
}

// zruseni seznamu
void ListFree(tInstructionList *List)
{
    tListItem *temp;
    while (List->First != NULL)
    {
        temp = List->First;
        List->First = List->First->Next;
        free(temp);
    }
    List->First = NULL;
    List->Last = NULL;
    List->Active = NULL;
}

// vlozeni instrukce na konec seznamu
int ListInsertLast(tInstructionList *List, tTriCode Instruction)
{
    tListItem *temp = malloc(sizeof(struct list_item));
    if (temp == NULL)
    { // test alokace
        return MALLOC_ERROR;
    }
    else
    {
        temp->Instruction = Instruction;
        temp->Next = NULL;
        if (List->First == NULL)
        {
            List->First = temp;
        }
        else
        {
            List->Last->Next = temp;
        }
        List->Last = temp;
    }
    return LIST_OK;
}

// nastaveni aktivity na prvni polozku
void ListFirst(tInstructionList *List)
{
    List->Active = List->First;
}

// nastaveni aktivity na dalsi polozku
void ListNext(tInstructionList *List)
{
    if (List->Active != NULL)
    {
        List->Active = List->Active->Next;
    }
}

// nastaveni aktivity na ukazatel skoku
// narusuje strukturu seznamu
void ListGoto(tInstructionList *List, void *ptr)
{
    List->Active = (tListItem *)ptr;
}

// vraceni ukazatele na posledni instrukci
// pouziva se v pripade vytvoreni navesti a zapamatovani si jeho adresy
void *ListGetLast(tInstructionList *List)
{
    return (void*)List->Last;
}

// vraceni aktivni instrukce
tTriCode GetInstruction(tInstructionList *List)
{
    tTriCode result;
    if (List->Active != NULL)
    {
        result.type = List->Active->Instruction.type;
        result.op1 = List->Active->Instruction.op1;
        result.op2 = List->Active->Instruction.op2;
        result.op3 = List->Active->Instruction.op3;
    }
    return result;
}

/** STACK **/



/* konec i_list.h */
